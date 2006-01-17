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
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "swsusp.h"

static int dev;
static char buffer[PAGE_SIZE];
static struct swsusp_header swsusp_header;

static inline unsigned int check_free_swap(void)
{
	int error;
	unsigned int free_swap;

	error = ioctl(dev, SNAPSHOT_IOCAVAIL_SWAP, &free_swap);
	if (!error)
		return free_swap;
	else
		printf("Error: errno = %d\n", errno);
	return 0;
}

static inline unsigned long get_swap_page(void)
{
	int error;
	unsigned long offset;

	error = ioctl(dev, SNAPSHOT_IOCGET_SWAP_PAGE, &offset);
	if (!error)
		return offset;
	return 0;
}

static inline int free_swap_pages(void)
{
	return ioctl(dev, SNAPSHOT_IOCFREE_SWAP_PAGES, 0);
}

static inline int set_swap_file(dev_t blkdev)
{
	return ioctl(dev, SNAPSHOT_IOCSET_SWAP_FILE, blkdev);
}

/**
 *	write_page - Write one page to given swap location.
 *	@fd:		File handle of the resume partition
 *	@buf:		Pointer to the area we're writing.
 *	@swap_offset:	Offset of the swap page we're writing to.
 */

static int write_page(int fd, void *buf, unsigned long swap_offset)
{
	off_t offset;
	int res = -EINVAL;
	ssize_t cnt = 0;

	if (swap_offset) {
		offset = swap_offset * PAGE_SIZE;
		if (lseek(fd, offset, SEEK_SET) == offset) 
			cnt = write(fd, buf, PAGE_SIZE);
		if (cnt == PAGE_SIZE)
			res = 0;
		else
			res = cnt < 0 ? cnt : -EIO;
	}
	return res;
}

/*
 *	The swap_map_handle structure is used for handling swap in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page cur;
	unsigned long cur_swap;
	unsigned int k;
	int fd;
};

static inline int init_swap_writer(struct swap_map_handle *handle, int fd)
{
	memset(&handle->cur, 0, PAGE_SIZE);
	handle->cur_swap = get_swap_page();
	if (!handle->cur_swap)
		return -ENOSPC;
	handle->k = 0;
	handle->fd = fd;
	return 0;
}

static int swap_write_page(struct swap_map_handle *handle, void *buf)
{
	int error;
	unsigned long offset;

	offset = get_swap_page();
	error = write_page(handle->fd, buf, offset);
	if (error)
		return error;
	handle->cur.entries[handle->k++] = offset;
	if (handle->k >= MAP_PAGE_ENTRIES) {
		offset = get_swap_page();
		if (!offset)
			return -ENOSPC;
		handle->cur.next_swap = offset;
		error = write_page(handle->fd, &handle->cur, handle->cur_swap);
		if (error)
			return error;
		memset(&handle->cur, 0, PAGE_SIZE);
		handle->cur_swap = offset;
		handle->k = 0;
	}
	return 0;
}

static inline int flush_swap_writer(struct swap_map_handle *handle)
{
	if (handle->cur_swap)
		return write_page(handle->fd, &handle->cur, handle->cur_swap);
	else
		return -EINVAL;
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
		ret = read(dev, buffer, PAGE_SIZE);
		if (ret > 0) {
			error = swap_write_page(handle, buffer);
			if (error)
				break;
			if (!(nr_pages % m))
				printf("\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}
	} while (ret > 0);
	if (!error)
		printf(" done\n");
	return error;
}

/**
 *	enough_swap - Make sure we have enough swap to save the image.
 *
 *	Returns TRUE or FALSE after checking the total amount of swap
 *	space avaiable from the resume partition.
 */

static int enough_swap(unsigned int nr_pages)
{
	unsigned int free_swap = check_free_swap();

	printf("suspend: Free swap pages: %u\n", free_swap);
	return free_swap > nr_pages;
}

static int mark_swap(int fd, unsigned long start)
{
	int error = 0;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &swsusp_header, PAGE_SIZE) < PAGE_SIZE)
		return -EIO;
	if (!memcmp("SWAP-SPACE", swsusp_header.sig, 10) ||
	    !memcmp("SWAPSPACE2", swsusp_header.sig, 10)) {
		memcpy(swsusp_header.orig_sig, swsusp_header.sig, 10);
		memcpy(swsusp_header.sig, SWSUSP_SIG, 10);
		swsusp_header.image = start;
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &swsusp_header, PAGE_SIZE) < PAGE_SIZE)
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

int write_image(char *resume_dev_name)
{
	static struct swap_map_handle handle;
	struct swsusp_info *header;
	unsigned long start;
	int fd;
	int error;

	fd = open(resume_dev_name, O_RDWR | O_SYNC);
	if (fd < 0) {
		printf("suspend: Could not open resume device\n");
		return error;
	}
	error = read(dev, buffer, PAGE_SIZE);
	if (error < PAGE_SIZE)
		return error < 0 ? error : -EFAULT;
	header = (struct swsusp_info *)buffer;
	if (!enough_swap(header->pages)) {
		printf("suspend: Not enough free swap\n");
		return -ENOSPC;
	}
	error = init_swap_writer(&handle, fd);
	if (!error) {
		start = handle.cur_swap;
		error = swap_write_page(&handle, header);
	}
	if (!error)
		error = save_image(&handle, header->pages - 1);
	if (!error) {
		flush_swap_writer(&handle);
		printf( "S" );
		error = mark_swap(fd, start);
		printf( "|\n" );
	}
	fsync(fd);
	close(fd);
	return error;
}

int main(int argc, char *argv[])
{
	char *snapshot_device_name, *resume_device_name;
	struct stat *stat_buf;
	int image_size;
	int error;
	int in_suspend;

	resume_device_name = argc <= 2 ? RESUME_DEVICE : argv[2];
	snapshot_device_name = argc <= 1 ? SNAPSHOT_DEVICE : argv[1];

	stat_buf = (struct stat *)buffer;
	if (stat(resume_device_name, stat_buf)) {
		fprintf(stderr, "Could not stat the resume device file\n");
		return -ENODEV;
	}
	if (!S_ISBLK(stat_buf->st_mode)) {
		fprintf(stderr, "Invalid resume device\n");
		return -EINVAL;
	}

	sync();
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "Could not lock myself\n");
		return 1;
	}

	error = 0;
	dev = open(snapshot_device_name, O_RDONLY);
	if (dev < 0)
		return -ENOENT;
	if (set_swap_file(stat_buf->st_rdev)) {
		fprintf(stderr, "Could not set the resume device file\n");
		error = errno;
		goto Close;
	}
	image_size = check_free_swap();
	if (image_size <= 0) {
		fprintf(stderr, "No swap space for suspend\n");
		error = -ENOSPC;
		goto Close;
	}
	if (image_size > DEFAULT_IMAGE_SIZE)
		image_size = DEFAULT_IMAGE_SIZE;
	if (freeze(dev)) {
		fprintf(stderr, "Could not freeze processes\n");
		error = errno;
		goto Close;
	}
	while (image_size >= 0) {
		if (set_image_size(dev, image_size)) {
			fprintf(stderr, "Could not set the image size\n");
			error = errno;
			goto Unfreeze;
		}
		if (!atomic_snapshot(dev, &in_suspend)) {
			if (!in_suspend)
				break;
			if (!write_image(resume_device_name)) {
				power_off();
			} else {
				free_swap_pages();
				free_snapshot(dev);
				image_size -= DEFAULT_IMAGE_SIZE;
			}
		}
	}
Unfreeze:
	unfreeze(dev);
Close:
	close(dev);

	return error;
}
