/*
 * resume.c
 *
 * A simple user space resume handler for swsusp.
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
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <lzf.h>

#include "swsusp.h"
#include "config.h"
#include "md5.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static int max_loglevel = MAX_LOGLEVEL;
static char verify_checksum;
static char decompress;

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
		.name = "suspend loglevel",
		.fmt = "%d",
		.ptr = &suspend_loglevel,
	},
	{
		.name = "max loglevel",
		.fmt = "%d",
		.ptr = &max_loglevel,
	},
	{
		.name = "image size",
		.fmt = "%lu",
		.ptr = NULL,
	},
	{
		.name = "compute checksum",
		.fmt = "%c",
		.ptr = NULL,
	},
	{
		.name = "compress",
		.fmt = "%c",
		.ptr = NULL,
	}
};

static char page_buffer[PAGE_SIZE];
static struct swsusp_header swsusp_header;

/**
 *	read_area - Read data from a swap location.
 *	@fd:		File handle of the resume partition
 *	@buf:		Pointer to the area we're reading into
 *	@offset:	Swap offset of the place to read from
 *	@size:		The number of bytes to read
 */

static int read_area(int fd, void *buf, loff_t offset, unsigned int size)
{
	int res = 0;
	ssize_t cnt = 0;

	if (offset) {
		if (lseek(fd, offset, SEEK_SET) == offset) 
			cnt = read(fd, buf, size);
		if (cnt < (ssize_t)size) {
			if (cnt < 0)
				res = cnt;
			else
				res = -EIO;
		}
	}
	return res;
}

static char read_buffer[BUFFER_SIZE];

/*
 *	The swap_map_handle structure is used for handling the swap map in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page cur;
	unsigned int area_size;
	unsigned int cur_size;
	unsigned int k;
	int fd;
	struct md5_ctx ctx;
};

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static int init_swap_reader(struct swap_map_handle *handle, int fd, loff_t start)
{
	int error;

	if (!start)
		return -EINVAL;
	error = read_area(fd, &handle->cur, start, sizeof(struct swap_map_page));
	if (error)
		return error;
	handle->area_size = handle->cur.entries[0].size;
	error = read_area(fd, read_buffer, handle->cur.entries[0].offset,
			handle->area_size);
	if (error)
		return error;
	handle->fd = fd;
	handle->k = 0;
	handle->cur_size = 0;
	if (verify_checksum)
		md5_init_ctx(&handle->ctx);
	return 0;
}

static inline int restore(void *buf, int disp)
{
	char *ptr = read_buffer + disp;
	unsigned short size = PAGE_SIZE;

	if (decompress) {
		size_t cnt;

		size = *((unsigned short *)ptr);
		ptr += sizeof(short);
		if (size == PAGE_SIZE) {
			size += sizeof(short);
		} else if (size < PAGE_SIZE) {
			cnt = lzf_decompress(ptr, size, buf, PAGE_SIZE);
			return cnt == PAGE_SIZE ? size + sizeof(short) : -ENODATA;
		} else {
			return -EINVAL;
		}
	}
	memcpy(buf, ptr, PAGE_SIZE);
	return size;
}

static inline int swap_read_page(struct swap_map_handle *handle, void *buf)
{
	loff_t offset;
	size_t size;
	int error = 0;

	if (handle->cur_size < handle->area_size) {
		/* Get the data from the read buffer */
		size = restore(buf, handle->cur_size);
		if (size < 0)
			return size;
		handle->cur_size += size;
		goto MD5;
	}

	/* There are no more data in the read buffer.  Read more */
	if (++handle->k >= MAP_PAGE_ENTRIES) {
		handle->k = 0;
		offset = handle->cur.next_swap;
		if (offset)
			error = read_area(handle->fd, &handle->cur, offset,
				sizeof(struct swap_map_page));
		else
			error = -EINVAL;
	}
	if (!error) {
		handle->area_size = handle->cur.entries[handle->k].size;
		error = read_area(handle->fd, read_buffer,
				handle->cur.entries[handle->k].offset,
				handle->area_size);
	}
	if (!error) {
		size = restore(buf, 0);
		if (size < 0)
			return size;
		handle->cur_size = size;
	}

MD5:
	if (verify_checksum)
		md5_process_block(buf, PAGE_SIZE, &handle->ctx);

	return error;
}

/**
 *	load_image - load the image using the swap map handle
 *	@handle and the snapshot handle @snapshot
 *	(assume there are @nr_pages pages to load)
 */

static inline int load_image(struct swap_map_handle *handle, int dev,
                      unsigned int nr_pages)
{
	unsigned int m, n;
	int ret;
	int error = 0;

	printf("Loading image data pages (%u pages) ...     ", nr_pages);
	m = nr_pages / 100;
	if (!m)
		m = 1;
	n = 0;
	do {
		error = swap_read_page(handle, page_buffer);
		if (error)
			break;
		ret = write(dev, page_buffer, PAGE_SIZE);
		if (ret < (int)PAGE_SIZE) {
			error = ret < 0 ? -errno : -ENOSPC;
			break;
		}
		if (!(n % m))
			printf("\b\b\b\b%3d%%", n / m);
		n++;
	} while (n < nr_pages);
	if (!error)
		printf(" done\n");
	return error;
}

static inline void print_checksum(unsigned char *checksum)
{
	int j;

	for (j = 0; j < 16; j++)
		printf("%02hhx", checksum[j]);
}

static int read_image(int dev, char *resume_dev_name)
{
	static struct swap_map_handle handle;
	static unsigned char orig_checksum[16], checksum[16];
	int fd, ret, error = 0;
	struct swsusp_info *header;
	unsigned int nr_pages;
	char c;

	fd = open(resume_dev_name, O_RDWR);
	if (fd < 0) {
		printf("resume: Could not open the resume device\n");
		return -errno;
	}
	memset(&swsusp_header, 0, sizeof(swsusp_header));
	ret = read(fd, &swsusp_header, PAGE_SIZE);
	if (ret == PAGE_SIZE) {
		if (memcmp(SWSUSP_SIG, swsusp_header.sig, 10))
			return -EINVAL;
	} else {
		error = ret < 0 ? ret : -EIO;
	}
	if (!error)
		error = read_area(fd, page_buffer, swsusp_header.image, PAGE_SIZE);
	if (!error) {
		header = (struct swsusp_info *)page_buffer;
		if(header->image_flags & IMAGE_CHECKSUM) {
			memcpy(orig_checksum, header->checksum, 16);
			printf("resume: MD5 checksum ");
			print_checksum(orig_checksum);
			printf("\n");
			verify_checksum = 1;
		}
		if (header->image_flags & IMAGE_COMPRESSED) {
			printf("resume: Compressed image\n");
			decompress = 1;
		}
		error = init_swap_reader(&handle, fd, header->map_start);
		if (!error) {
			nr_pages = header->pages - 1;
			ret = write(dev, page_buffer, PAGE_SIZE);
			if (ret < (int)PAGE_SIZE)
				error = ret < 0 ? ret : -EIO;
		}
		if (!error)
			error = load_image(&handle, dev, nr_pages);
	}
	if (error) {
		printf("resume: The system snapshot image could not be read.\n\n"
			"\tThis might be a result of booting a wrong kernel.\n\n"
			"\tYou can continue to boot the system and lose the saved state\n"
			"\tor reboot and try again.\n\n"
		        "\t[Notice that you may not mount any filesystem between\n"
		        "\tnow and successful resume. That would badly damage\n"
		        "\taffected filesystems.]\n\n"
			"\tDo you want to continue boot (Y/n)? ");
		c = getchar();
		ret = (c == 'n' || c == 'N');
		if (ret) {
			close(fd);
			reboot();
		}
	}
	if (!error || !ret) {
		if (!error && verify_checksum) {
			md5_finish_ctx(&handle.ctx, checksum);
			if (memcmp(orig_checksum, checksum, 16)) {
				printf("resume: MD5 checksum does not match\n");
				printf("resume: Computed MD5 checksum ");
				print_checksum(checksum);
				printf("\n");
				error = -EINVAL;
			}
		}
		/* Reset swap signature now */
		memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
		ret = lseek(fd, 0, SEEK_SET);
		if (!ret)
			ret = write(fd, &swsusp_header, PAGE_SIZE);
		if (ret < (int)PAGE_SIZE) {
			fprintf(stderr, "resume: Could not restore the partition header\n");
			error = ret < 0 ? -errno : -EIO;
		}
	}
	fsync(fd);
	close(fd);
	if (!error) {
		printf("resume: Image successfully loaded\n");
	} else {
		printf("resume: Error %d loading the image\n\nPress ENTER to continue ",
			error);
		getchar();
	}
	return error;
}

static void set_kernel_console_loglevel(int level)
{
	FILE *file;
	struct stat stat_buf;
	char *procname = "/proc/sys/kernel/printk";
	int proc_mounted = 0;

	if (stat(procname, &stat_buf) && errno == ENOENT) {
		if (mount("none", "/proc", "proc", 0, NULL)) {
			printf("resume: Could not mount proc\n");
			return;
		} else
			proc_mounted = 1;
	}
	file = fopen(procname, "w");
	if (file) {
		fprintf(file, "%d\n", level);
		fclose(file);
	}
	if (proc_mounted)
		umount("/proc");
}

int main(int argc, char *argv[])
{
	struct stat stat_buf;
	int dev;
	int n, error = 0;

	if (get_config("resume", argc, argv, PARAM_NO, parameters, resume_dev_name))
		return EINVAL;

	while (stat(resume_dev_name, &stat_buf)) {
		printf("resume: Could not stat the resume device file.\n"
			"\tPlease type in the file name to try again"
			"\tor press ENTER to boot the system: ");
		fgets(resume_dev_name, MAX_STR_LEN - 1, stdin);
		n = strlen(resume_dev_name) - 1;
		if (n <= 0)
			return ENOENT;
		if (resume_dev_name[n] == '\n')
			resume_dev_name[n] = '\0';
	}

	set_kernel_console_loglevel(suspend_loglevel);

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "resume: Could not lock myself\n");
		return 1;
	}

	dev = open(snapshot_dev_name, O_WRONLY);
	if (dev < 0)
		return ENOENT;
	error = read_image(dev, resume_dev_name);
	if (error) {
		fprintf(stderr, "resume: Could not read the image\n");
		error = -error;
		goto Close;
	}
	if (freeze(dev)) {
		fprintf(stderr, "resume: Could not freeze processes\n");
		error = errno;
		goto Close;
	}
	atomic_restore(dev);
	unfreeze(dev);
Close:
	close(dev);

	set_kernel_console_loglevel(max_loglevel);

	return error;
}
