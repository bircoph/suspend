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
		if (cnt < PAGE_SIZE) {
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
		if ((ret = write(dev, buffer, PAGE_SIZE)) < PAGE_SIZE) {
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


	fd = open(resume_dev_name, O_RDWR);
	if (fd < 0) {
		printf("resume: Could not open resume device\n");
		return -ENOENT;
	}
	memset(&swsusp_header, 0, sizeof(swsusp_header));
	ret = read(fd, &swsusp_header, PAGE_SIZE);
	if (ret == PAGE_SIZE) {
		if (!memcmp(SWSUSP_SIG, swsusp_header.sig, 10)) {
			memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
			/* Reset swap signature now */
			if (lseek(fd, 0, SEEK_SET) == 0) {
				ret = write(fd, &swsusp_header, PAGE_SIZE);
				if (ret < PAGE_SIZE)
					error = ret < 0 ? ret : -EIO;
			} else {
				error = -EIO;
			}
		} else {
			error = -EINVAL;
		}
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
			if (ret < PAGE_SIZE)
				error = ret < 0 ? ret : -EIO;
		}
		if (!error)
			error = load_image(&handle, dev, nr_pages);
	}
	fsync(fd);
	close(fd);
	if (!error)
		printf("resume: Reading resume file was successful\n");
	else
		printf("resume: Error %d resuming\n", error);
	return error;
}

int main(int argc, char *argv[])
{
	char *snapshot_device_name, *resume_device_name;
	int dev;
	int error = 0;

	resume_device_name = argc <= 1 ? RESUME_DEVICE : argv[1];
	snapshot_device_name = SNAPSHOT_DEVICE;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "resume: Could not lock myself\n");
		return 1;
	}

	dev = open(snapshot_device_name, O_WRONLY);
	if (dev < 0)
		return -ENOENT;
	if (read_image(dev, resume_device_name)) {
		fprintf(stderr, "resume: Could not read the image\n");
		error = errno;
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
	return error;
}
