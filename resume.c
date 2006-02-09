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

#include "swsusp.h"

static char buffer[PAGE_SIZE];
static struct swsusp_header swsusp_header;

/**
 *	read_page - Read one page from a swap location.
 *	@fd:	File handle of the resume partition
 *	@buf:	Pointer to the area we're reading into.
 *	@off:	Swap offset of the place to read from.
 */

static int read_page(int fd, void *buf, loff_t offset)
{
	int res = 0;
	ssize_t cnt = 0;

	if (offset) {
		if (lseek(fd, offset, SEEK_SET) == offset) 
			cnt = read(fd, buf, PAGE_SIZE);
		if (cnt < (ssize_t)PAGE_SIZE) {
			if (cnt < 0)
				res = cnt;
			else
				res = -EIO;
		}
	}
	return res;
}

/*
 *	The swap_map_handle structure is used for handling the swap map in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page cur;
	unsigned int k;
	int fd;
};

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static inline int init_swap_reader(struct swap_map_handle *handle,
                                      int fd, loff_t start)
{
	int error;

	if (!start)
		return -EINVAL;
	memset(&handle->cur, 0, PAGE_SIZE);
	error = read_page(fd, &handle->cur, start);
	if (error)
		return error;
	handle->fd = fd;
	handle->k = 0;
	return 0;
}

static inline int swap_read_page(struct swap_map_handle *handle, void *buf)
{
	loff_t offset;
	int error;

	offset = handle->cur.entries[handle->k++];
	if (!offset)
		return -EFAULT;
	error = read_page(handle->fd, buf, offset);
	if (error)
		return error;
	if (handle->k >= MAP_PAGE_ENTRIES) {
		handle->k = 0;
		offset = handle->cur.next_swap;
		if (offset)
			error = read_page(handle->fd, &handle->cur, offset);
		else
			error = -EINVAL;
	}
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
		error = swap_read_page(handle, buffer);
		if (error)
			break;
		ret = write(dev, buffer, PAGE_SIZE);
		if (ret < (int)PAGE_SIZE) {
			error = ret < 0 ? ret : -ENOSPC;
			break;
		}
		if (!(n % m))
			printf("\b\b\b\b%3d%%", n / m);
		n++;
	} while (n < nr_pages);
	if (!error)
		printf("\b\b\b\bdone\n");
	return error;
}

static int read_image(int dev, char *resume_dev_name)
{
	static struct swap_map_handle handle;
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
	if (!error) {
		error = init_swap_reader(&handle, fd, swsusp_header.image);
		if (!error) {
			header = (struct swsusp_info *)buffer;
			error = swap_read_page(&handle, header);
		}
		if (!error) {
			nr_pages = header->pages - 1;
			ret = write(dev, buffer, PAGE_SIZE);
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
		fscanf(stdin, "%c", &c);
		ret = (c == 'n' || c == 'N');
		if (ret) {
			close(fd);
			reboot();
		}
	}
	if (!error || !ret) {
		memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
		/* Reset swap signature now */
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
	if (!error)
		printf("resume: Image successfully loaded\n");
	else
		printf("resume: Error %d loading the image\n", error);
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
	char *resume_device_name;
	struct stat stat_buf;
	int dev;
	int n, error = 0;

	resume_device_name = argc <= 1 ? RESUME_DEVICE : argv[1];

	while (stat(resume_device_name, &stat_buf)) {
		printf("resume: Could not stat the resume device file.\n"
			"\tPlease type in the file name to try again"
			"\tor press ENTER to boot the system: ");
		fgets(buffer, 256, stdin);
		n = strlen(buffer) - 1;
		if (!n)
			return ENOENT;
		if (buffer[n] == '\n')
			buffer[n] = '\0';
		resume_device_name = buffer;
	}

	set_kernel_console_loglevel(SUSPEND_LOGLEVEL);

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "resume: Could not lock myself\n");
		return 1;
	}

	dev = open(SNAPSHOT_DEVICE, O_WRONLY);
	if (dev < 0)
		return ENOENT;
	error = read_image(dev, resume_device_name);
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

	set_kernel_console_loglevel(MAX_LOGLEVEL);

	return error;
}
