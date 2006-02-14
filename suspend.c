/*
 * suspend.c
 *
 * A simple user space suspend handler for swsusp.
 *
 * Copyright (C) 2005 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/vt.h>
#include <sys/wait.h>
#include <linux/kd.h>
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "swsusp.h"
#include "config.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static unsigned long pref_image_size = IMAGE_SIZE;
static int suspend_loglevel = SUSPEND_LOGLEVEL;

static struct config_par parameters[PARAM_NO] = {
	{
		.name = "snapshot device",
		.fmt = "%s",
		.ptr = snapshot_dev_name,
		.len = MAX_STR_LEN
	},
	{
		.name = "resume device",
		.fmt ="%s",
		.ptr = resume_dev_name,
		.len = MAX_STR_LEN
	},
	{
		.name = "image size",
		.fmt = "%lu",
		.ptr = &pref_image_size,
	},
	{
		.name = "suspend loglevel",
		.fmt = "%d",
		.ptr = &suspend_loglevel,
	},
	{
		.name = "max loglevel",
		.fmt = "%d",
		.ptr = NULL,
	}
};

static char buffer[PAGE_SIZE];
static struct swsusp_header swsusp_header;

static inline loff_t check_free_swap(int dev)
{
	int error;
	loff_t free_swap;

	error = ioctl(dev, SNAPSHOT_AVAIL_SWAP, &free_swap);
	if (!error)
		return free_swap;
	else
		printf("Error: errno = %d\n", errno);
	return 0;
}

static inline unsigned long get_swap_page(int dev)
{
	int error;
	loff_t offset;

	error = ioctl(dev, SNAPSHOT_GET_SWAP_PAGE, &offset);
	if (!error)
		return offset;
	return 0;
}

static inline int free_swap_pages(int dev)
{
	return ioctl(dev, SNAPSHOT_FREE_SWAP_PAGES, 0);
}

static inline int set_swap_file(int dev, dev_t blkdev)
{
	return ioctl(dev, SNAPSHOT_SET_SWAP_FILE, blkdev);
}

/**
 *	write_area - Write data to given swap location.
 *	@fd:		File handle of the resume partition
 *	@buf:		Pointer to the area we're writing.
 *	@offset:	Offset of the swap page we're writing to
 *	@size:		The number of bytes to save
 */

static int write_area(int fd, void *buf, loff_t offset, unsigned int size)
{
	int res = -EINVAL;
	ssize_t cnt = 0;

	if (offset) {
		if (lseek(fd, offset, SEEK_SET) == offset) 
			cnt = write(fd, buf, size);
		if (cnt == size)
			res = 0;
		else
			res = cnt < 0 ? cnt : -EIO;
	}
	return res;
}

static char write_buffer[BUFFER_SIZE];

/*
 *	The swap_map_handle structure is used for handling swap in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page cur;
	struct swap_area cur_area;
	loff_t cur_swap;
	unsigned int k;
	int dev, fd;
};

static inline int init_swap_writer(struct swap_map_handle *handle, int dev, int fd)
{
	memset(&handle->cur, 0, sizeof(struct swap_map_page));
	handle->cur_swap = get_swap_page(dev);
	if (!handle->cur_swap)
		return -ENOSPC;
	handle->cur_area.offset = get_swap_page(dev);
	if (!handle->cur_area.offset)
		return -ENOSPC;
	handle->cur_area.size = 0;
	handle->k = 0;
	handle->dev = dev;
	handle->fd = fd;
	return 0;
}

static int swap_write_page(struct swap_map_handle *handle, void *buf)
{
	loff_t offset;
	int error;

	if (!handle->cur_area.size) {
		/* No data in the write buffer */
		memcpy(write_buffer, buf, PAGE_SIZE);
		handle->cur_area.size = PAGE_SIZE;
		return 0;
	}

	offset = get_swap_page(handle->dev);
	if (!offset)
		return -ENOSPC;

	if (offset == handle->cur_area.offset + handle->cur_area.size &&
	    handle->cur_area.size + PAGE_SIZE <= BUFFER_SIZE) {
		/* Put the data into the write buffer */
		memcpy(write_buffer + handle->cur_area.size, buf, PAGE_SIZE);
		handle->cur_area.size += PAGE_SIZE;
		return 0;
	}

	/* The write buffer is full or the offset doesn't fit.  Flush */
	error = write_area(handle->fd, write_buffer, handle->cur_area.offset,
			handle->cur_area.size);
	if (error)
		return error;
	handle->cur.entries[handle->k].offset = handle->cur_area.offset;
	handle->cur.entries[handle->k].size = handle->cur_area.size;
	/* The write buffer has been flushed.  Fill it from the start */
	handle->cur_area.offset = offset;
	memcpy(write_buffer, buf, PAGE_SIZE);
	handle->cur_area.size = PAGE_SIZE;
	if (++handle->k >= MAP_PAGE_ENTRIES) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		handle->cur.next_swap = offset;
		error = write_area(handle->fd, &handle->cur, handle->cur_swap,
				sizeof(struct swap_map_page));
		if (error)
			return error;
		memset(&handle->cur, 0, sizeof(struct swap_map_page));
		handle->cur_swap = offset;
		handle->k = 0;
	}
	return 0;
}

static inline int flush_swap_writer(struct swap_map_handle *handle)
{
	int error;

	if (!handle->cur_area.offset || !handle->cur_swap)
		return -EINVAL;

	/* Flush the write buffer */
	error = write_area(handle->fd, write_buffer, handle->cur_area.offset,
			handle->cur_area.size);
	if (!error) {
		handle->cur.entries[handle->k].offset = handle->cur_area.offset;
		handle->cur.entries[handle->k].size = handle->cur_area.size;
		/* Save the last swap map page */
		error = write_area(handle->fd, &handle->cur, handle->cur_swap,
			sizeof(struct swap_map_page));
	}

	return error;
}

/**
 *	save_image - save the suspend image data
 */

static int save_image(struct swap_map_handle *handle,
                      unsigned int nr_pages)
{
	unsigned int m;
	int ret;
	int error = 0;

	printf("suspend: Saving image data pages (%u pages) ...     ", nr_pages);
	m = nr_pages / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	do {
		ret = read(handle->dev, buffer, PAGE_SIZE);
		if (ret > 0) {
			error = swap_write_page(handle, buffer);
			if (error)
				break;
			if (!(nr_pages % m))
				printf("\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}
	} while (ret > 0);
	if (ret < 0)
		error = -errno;
	if (!error)
		printf(" done (%u pages)\n", nr_pages);
	return error;
}

/**
 *	enough_swap - Make sure we have enough swap to save the image.
 *
 *	Returns TRUE or FALSE after checking the total amount of swap
 *	space avaiable from the resume partition.
 */

static int enough_swap(int dev, unsigned long size)
{
	loff_t free_swap = check_free_swap(dev);

	printf("suspend: Free swap: %lu bytes\n", (unsigned long) free_swap);
	return free_swap > size;
}

static int mark_swap(int fd, loff_t start)
{
	int error = 0;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &swsusp_header, PAGE_SIZE) < (ssize_t)PAGE_SIZE)
		return -EIO;
	if (!memcmp("SWAP-SPACE", swsusp_header.sig, 10) ||
	    !memcmp("SWAPSPACE2", swsusp_header.sig, 10)) {
		memcpy(swsusp_header.orig_sig, swsusp_header.sig, 10);
		memcpy(swsusp_header.sig, SWSUSP_SIG, 10);
		swsusp_header.image = start;
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &swsusp_header, PAGE_SIZE) < (ssize_t)PAGE_SIZE)
			error = -EIO;
	} else {
		printf("suspend: Device is not a swap space.\n");
		error = -ENODEV;
	}
	return error;
}

/**
 *	write_image - Write entire image and metadata.
 */

int write_image(int snapshot_fd, int resume_fd)
{
	static struct swap_map_handle handle;
	struct swsusp_info *header;
	loff_t start;
	int error;

	printf("suspend: System snapshot ready. Preparing to write\n");
	error = read(snapshot_fd, buffer, PAGE_SIZE);
	if (error < (int)PAGE_SIZE)
		return error < 0 ? error : -EFAULT;
	header = (struct swsusp_info *)buffer;
	printf("suspend: Image size: %lu bytes\n", header->size);
	if (!enough_swap(snapshot_fd, header->size)) {
		printf("suspend: Not enough free swap\n");
		return -ENOSPC;
	}
	error = init_swap_writer(&handle, snapshot_fd, resume_fd);
	if (!error) {
		start = handle.cur_swap;
		error = swap_write_page(&handle, header);
	}
	if (!error)
		error = save_image(&handle, header->pages - 1);
	if (!error) {
		flush_swap_writer(&handle);
		printf( "S" );
		error = mark_swap(resume_fd, start);
		fsync(resume_fd);
		printf( "|\n" );
	} else {
		fsync(resume_fd);
	}
	return error;
}

#define switch_vt_mode(fd, vt, ioc1, ioc2)	ioctl(fd, KDSKBMODE, ioc1); \
						ioctl(fd, VT_ACTIVATE, vt); \
						ioctl(fd, VT_WAITACTIVE, vt); \
						ioctl(fd, KDSKBMODE, ioc2);

int suspend_system(int snapshot_fd, int resume_fd, int vt_fd, int vt_no)
{
	loff_t avail_swap;
	unsigned long image_size;
	int attempts, in_suspend, error = 0;

	avail_swap = check_free_swap(snapshot_fd);
	if (avail_swap > pref_image_size)
		image_size = pref_image_size;
	else
		image_size = avail_swap;
	if (!image_size) {
		fprintf(stderr, "suspend: No swap space for suspend\n");
		return ENOSPC;
	}

	switch_vt_mode(vt_fd, vt_no, K_MEDIUMRAW, KD_GRAPHICS);
	error = freeze(snapshot_fd);
	switch_vt_mode(vt_fd, vt_no, KD_TEXT, K_XLATE);
	if (error)
		goto Unfreeze;

	printf("suspend: Snapshotting system\n");
	attempts = 2;
	do {
		if (set_image_size(snapshot_fd, image_size)) {
			error = errno;
			break;
		}
		if (!atomic_snapshot(snapshot_fd, &in_suspend)) {
			if (!in_suspend)
				break;
			if (!write_image(snapshot_fd, resume_fd)) {
				power_off();
			} else {
				free_swap_pages(snapshot_fd);
				free_snapshot(snapshot_fd);
				image_size = 0;
				error = errno;
			}
		} else {
			error = errno;
			break;
		}
	} while (--attempts);

Unfreeze:
	unfreeze(snapshot_fd);

	return error;
}

/**
 *	console_fd - get file descriptor for given file name and verify
 *	if that's a console descriptor (based on the code of openvt)
 */

static inline int console_fd(const char *fname)
{
	int fd;
	char arg;

	fd = open(fname, O_RDONLY);
	if (fd < 0 && errno == EACCES)
		fd = open(fname, O_WRONLY);
	if (fd >= 0 && (ioctl(fd, KDGKBTYPE, &arg) || (arg != KB_101 && arg != KB_84))) {
		close(fd);
		return -ENOTTY;
	}
	return fd;
}

/**
 *	prepare_console - find a spare virtual terminal, open it and attach
 *	the standard streams to it.  The number of the currently active
 *	virtual terminal is saved via @orig_vc
 */

static int prepare_console(int *orig_vc, int *new_vc)
{
	int fd, error, vt = -1;
	char *vt_name = buffer;
	struct vt_stat vtstat;

	fd = console_fd("/dev/console");
	if (fd < 0)
		return fd;
	error = ioctl(fd, VT_GETSTATE, &vtstat);
	if (!error) {
		*orig_vc = vtstat.v_active;
		error = ioctl(fd, VT_OPENQRY, &vt);
	}
	close(fd);
	if (error || vt < 0)
		return -1;
	sprintf(vt_name, "/dev/tty%d", vt);
	fd = open(vt_name, O_RDWR);
	if (fd < 0)
		return fd;
	error = ioctl(fd, VT_ACTIVATE, vt);
	if (error) {
		fprintf(stderr, "Could not activate the VT %d\n", vt);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, vt);
	if (error) {
		fprintf(stderr, "VT %d activation failed\n", vt);
		fflush(stderr);
		goto Close_fd;
	}
	char *msg = "\33[H\33[J";
	write(fd, msg, strlen(msg));
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	*new_vc = vt;
	return fd;
Close_fd:
	close(fd);
	return error;
}

/**
 *	restore_console - switch to the virtual console that was active before
 *	suspend
 */

static void restore_console(int fd, int orig_vc)
{
	int error;

	error = ioctl(fd, VT_ACTIVATE, orig_vc);
	if (error) {
		fprintf(stderr, "Could not activate the VT %d\n", orig_vc);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, orig_vc);
	if (error) {
		fprintf(stderr, "VT %d activation failed\n", orig_vc);
		fflush(stderr);
	}
Close_fd:
	close(fd);
}

static inline int get_kernel_console_loglevel(void)
{
	FILE *file;
	int level = -1;

	file = fopen("/proc/sys/kernel/printk", "r");
	if (file) {
		fscanf(file, "%d", &level);
		fclose(file);
	}
	return level;
}

static inline void set_kernel_console_loglevel(int level)
{
	FILE *file;

	file = fopen("/proc/sys/kernel/printk", "w");
	if (file) {
		fprintf(file, "%d\n", level);
		fclose(file);
	}
}

int main(int argc, char *argv[])
{
	char *chroot_path = buffer;
	struct stat stat_buf;
	int resume_fd, snapshot_fd, vt_fd, orig_vc, suspend_vc;
	dev_t resume_dev;
	int orig_loglevel, ret = 0;

	if (get_config("suspend", argc, argv, PARAM_NO, parameters, resume_dev_name))
		return EINVAL;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "suspend: Could not lock myself\n");
		return errno;
	}

	if (stat(resume_dev_name, &stat_buf)) {
		fprintf(stderr, "suspend: Could not stat the resume device file\n");
		return ENODEV;
	} else {
	}
	if (!S_ISBLK(stat_buf.st_mode)) {
		fprintf(stderr, "suspend: Invalid resume device\n");
		return EINVAL;
	}
	resume_fd = open(resume_dev_name, O_RDWR);
	if (resume_fd < 0) {
		fprintf(stderr, "suspend: Could not open the resume device\n");
		return errno;
	}
	resume_dev = stat_buf.st_rdev;

	vt_fd = prepare_console(&orig_vc, &suspend_vc);
	if (vt_fd < 0) {
		fprintf(stderr, "suspend: Could not open a virtual terminal\n");
		ret = errno;
		goto Close_resume_fd;
	}

	if (stat(snapshot_dev_name, &stat_buf)) {
		fprintf(stderr, "suspend: Could not stat the snapshot device file\n");
		ret = ENODEV;
		goto Restore_console;
	}
	if (!S_ISCHR(stat_buf.st_mode)) {
		fprintf(stderr, "suspend: Invalid snapshot device\n");
		ret = EINVAL;
		goto Restore_console;
	}
	snapshot_fd = open(snapshot_dev_name, O_RDONLY);
	if (snapshot_fd < 0) {
		fprintf(stderr, "suspend: Could not open the snapshot device\n");
		ret = errno;
		goto Restore_console;
	}

	if (set_swap_file(snapshot_fd, resume_dev)) {
		fprintf(stderr, "suspend: Could not use the resume device\n");
		ret = errno;
		goto Close_snapshot_fd;
	}

	sprintf(chroot_path, "/proc/%d", getpid());
	if (chroot(chroot_path)) {
		fprintf(stderr, "suspend: Could not chroot to %s\n", chroot_path);
		ret = errno;
		goto Close_snapshot_fd;
	}
	chdir("/");

	orig_loglevel = get_kernel_console_loglevel();
	set_kernel_console_loglevel(suspend_loglevel);

	sync();

	ret = suspend_system(snapshot_fd, resume_fd, vt_fd, suspend_vc);

	if (orig_loglevel >= 0)
		set_kernel_console_loglevel(orig_loglevel);

Close_snapshot_fd:
	close(snapshot_fd);
Restore_console:
	restore_console(vt_fd, orig_vc);
Close_resume_fd:
	close(resume_fd);

	return ret;
}
