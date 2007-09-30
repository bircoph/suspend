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

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/vt.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <linux/kd.h>
#include <linux/tiocl.h>
#include <syscall.h>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#ifdef CONFIG_COMPRESS
#include <lzo/lzo1x.h>
#endif

#include "swsusp.h"
#include "config_parser.h"
#include "md5.h"
#include "splash.h"
#include "vt.h"
#include "loglevel.h"
#ifdef CONFIG_BOTH
#include "s2ram.h"
#endif

#define suspend_error(msg, args...) \
do { \
	fprintf(stderr, "%s: " msg " Reason: %m\n", my_name, ## args); \
} while (0)

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static loff_t resume_offset;
static unsigned long pref_image_size = IMAGE_SIZE;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static char compute_checksum;
#ifdef CONFIG_COMPRESS
static char do_compress;
static long long compr_diff;
#else
#define do_compress 0
#define compr_diff 0
#endif
#ifdef CONFIG_ENCRYPT
static char do_encrypt;
static char use_RSA;
static char key_name[MAX_STR_LEN] = KEY_FILE;
static char password[PASS_SIZE];
#else
#define do_encrypt 0
#define key_name NULL
#endif
#ifdef CONFIG_BOTH
static char s2ram;
#endif
static char early_writeout;
static char splash_param;
#define SHUTDOWN_LEN	16
static char shutdown_method_value[SHUTDOWN_LEN] = "";
static enum {
	SHUTDOWN_METHOD_SHUTDOWN,
	SHUTDOWN_METHOD_PLATFORM,
	SHUTDOWN_METHOD_REBOOT
} shutdown_method = SHUTDOWN_METHOD_PLATFORM;

static int suspend_swappiness = SUSPEND_SWAPPINESS;
static struct splash splash;
static struct vt_mode orig_vtm;
static int vfd;
static char *my_name;

static struct config_par parameters[] = {
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
		.name = "resume offset",
		.fmt = "%llu",
		.ptr = &resume_offset,
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
	},
	{
		.name = "compute checksum",
		.fmt = "%c",
		.ptr = &compute_checksum,
	},
#ifdef CONFIG_COMPRESS
	{
		.name = "compress",
		.fmt = "%c",
		.ptr = &do_compress,
	},
#endif
#ifdef CONFIG_ENCRYPT
	{
		.name = "encrypt",
		.fmt = "%c",
		.ptr = &do_encrypt,
	},
	{
		.name = "RSA key file",
		.fmt = "%s",
		.ptr = key_name,
		.len = MAX_STR_LEN
	},
#endif
	{
		.name = "early writeout",
		.fmt = "%c",
		.ptr = &early_writeout,
	},
	{
		.name = "splash",
		.fmt = "%c",
		.ptr = &splash_param,
	},
	{
		.name = "shutdown method",
		.fmt = "%s",
		.ptr = shutdown_method_value,
		.len = SHUTDOWN_LEN,
	},
	{
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};

static unsigned int page_size;
/* This MUST NOT be less than page_size */
static unsigned int max_block_size;
static unsigned int buffer_size;
static void *mem_pool;
#ifdef CONFIG_ENCRYPT
struct key_data *key_data;
gcry_cipher_hd_t cipher_handle;
#endif

static inline loff_t check_free_swap(int dev)
{
	int error;
	loff_t free_swap;

	error = ioctl(dev, SNAPSHOT_AVAIL_SWAP, &free_swap);
	if (!error)
		return free_swap;
	else
		suspend_error("check_free_swap failed.");
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

static inline int set_swap_file(int dev, u_int32_t blkdev, loff_t offset)
{
	struct resume_swap_area swap;
	int error;

	swap.dev = blkdev;
	swap.offset = offset;
	error = ioctl(dev, SNAPSHOT_SET_SWAP_AREA, &swap);
	if (error && !offset)
		error = ioctl(dev, SNAPSHOT_SET_SWAP_FILE, blkdev);

	return error;
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
		if (lseek64(fd, offset, SEEK_SET) == offset)
			cnt = write(fd, buf, size);
		if (cnt == size)
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
	char *page_buffer;
	char *write_buffer;
	struct swap_area *areas;
	unsigned short areas_per_page;
	loff_t *next_swap;
	struct swap_area cur_area;
	unsigned int cur_alloc;
	loff_t cur_swap;
	unsigned int k;
	int dev, fd;
	struct md5_ctx ctx;
#ifdef CONFIG_COMPRESS
	void *lzo_work_buffer;
#endif
#ifdef CONFIG_ENCRYPT
	unsigned char *encrypt_buffer;
#endif
};

static int
init_swap_writer(struct swap_map_handle *handle, int dev, int fd, void *buf)
{
	if (!buf)
		return -EINVAL;

	handle->areas = buf;
	buf += page_size;

	handle->page_buffer = buf;
	buf += page_size;

	handle->write_buffer = buf;
	buf += buffer_size;

#ifdef CONFIG_COMPRESS
	handle->lzo_work_buffer = buf;
	buf += LZO1X_1_MEM_COMPRESS;
#endif

#ifdef CONFIG_ENCRYPT
	handle->encrypt_buffer = buf;
#endif

	memset(handle->areas, 0, page_size);
	handle->areas_per_page = (page_size - sizeof(loff_t)) /
			sizeof(struct swap_area);
	handle->next_swap = (loff_t *)(handle->areas + handle->areas_per_page);

	handle->cur_swap = get_swap_page(dev);
	if (!handle->cur_swap)
		return -ENOSPC;
	handle->cur_area.offset = get_swap_page(dev);
	if (!handle->cur_area.offset)
		return -ENOSPC;
	handle->cur_area.size = 0;
	handle->cur_alloc = page_size;
	handle->k = 0;

	handle->dev = dev;
	handle->fd = fd;

	if (compute_checksum)
		md5_init_ctx(&handle->ctx);

	return 0;
}

static int prepare(struct swap_map_handle *handle, int disp)
{
	struct buf_block *block;
	void *buf = handle->page_buffer;

	block = (struct buf_block *)(handle->write_buffer + disp);
#ifdef CONFIG_COMPRESS
	if (do_compress) {
		lzo_uint cnt;

		lzo1x_1_compress(buf, page_size, (lzo_bytep)block->data, &cnt,
					handle->lzo_work_buffer);
		block->size = cnt;
		/* Sanity check, should not trigger */
		if (cnt + sizeof(short) > max_block_size)
			printf("WARNING!"
				" Data size too large after compression!\n");
		/* We have added a sizeof(short) field to the data */
		compr_diff += page_size - cnt - sizeof(short);
		return cnt + sizeof(short);
	}
#endif
	memcpy(block, buf, page_size);
	return page_size;
}

static int try_get_more_swap(struct swap_map_handle *handle)
{
	loff_t offset;

	while (handle->cur_area.size > handle->cur_alloc) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		if (offset == handle->cur_area.offset + handle->cur_alloc) {
			handle->cur_alloc += page_size;
		} else {
			handle->cur_area.offset = offset;
			handle->cur_alloc = page_size;
		}
	}
	return 0;
}

static int flush_buffer(struct swap_map_handle *handle)
{
	void *src = handle->write_buffer;
	int error =  0;

#ifdef CONFIG_ENCRYPT
	if (do_encrypt) {
		error = gcry_cipher_encrypt(cipher_handle,
			handle->encrypt_buffer, handle->cur_area.size,
			src, handle->cur_area.size);
		src = handle->encrypt_buffer;
	}
#endif
	if (!error)
		error = write_area(handle->fd, src, handle->cur_area.offset,
				handle->cur_area.size);
	if (error)
		return error;
	handle->areas[handle->k].offset = handle->cur_area.offset;
	handle->areas[handle->k].size = handle->cur_area.size;
	return 0;
}

static int swap_write_page(struct swap_map_handle *handle)
{
	loff_t offset = 0;
	int error;

	if (compute_checksum)
		md5_process_block(handle->page_buffer, page_size, &handle->ctx);

	if (!handle->cur_area.size) {
		/* No data in the write buffer */
		handle->cur_area.size = prepare(handle, 0);
		return try_get_more_swap(handle);
	}

	if (handle->cur_alloc + max_block_size <= buffer_size) {
		if (handle->cur_area.size + max_block_size <= handle->cur_alloc) {
			handle->cur_area.size += prepare(handle, handle->cur_area.size);
			return 0;
		}
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		if (offset == handle->cur_area.offset + handle->cur_alloc) {
			handle->cur_alloc += page_size;
			handle->cur_area.size += prepare(handle, handle->cur_area.size);
			return try_get_more_swap(handle);
		}
	}

	/* The write buffer is full or the offset doesn't fit.  Flush the buffer */
	error = flush_buffer(handle);
	if (error)
		return error;

	/* The write buffer has been flushed.  Fill it from the start */
	if (!offset) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
	}
	handle->cur_area.offset = offset;
	handle->cur_alloc = page_size;
	handle->cur_area.size = prepare(handle, 0);
	error = try_get_more_swap(handle);
	if (error)
		return error;
	if (++handle->k >= handle->areas_per_page) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		*handle->next_swap = offset;
		error = write_area(handle->fd, handle->areas, handle->cur_swap,
				page_size);
		if (error)
			return error;
		memset(handle->areas, 0, page_size);
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
	error = flush_buffer(handle);
	if (!error)
		/* Save the last swap map page */
		error = write_area(handle->fd, handle->areas, handle->cur_swap,
				page_size);

	return error;
}

/**
 *	save_image - save the suspend image data
 */

static int save_image(struct swap_map_handle *handle,
                      unsigned int nr_pages)
{
	unsigned int m, writeout_rate;
	int ret, abort_possible;
	struct termios newtrm, savedtrm;
	int error = 0;

	/* Switch the state of the terminal so that we can read the keyboard
	 * without blocking and with no echo.
	 *
	 * stdin must be attached to the terminal now.
	 */
	abort_possible = !splash.prepare_abort(&savedtrm, &newtrm);

	if (abort_possible)
		printf("%s: Saving %u image data pages "
			"(press " ABORT_KEY_NAME " to abort) ...     ",
			my_name, nr_pages);
	else
		printf("%s: Saving image data pages (%u pages) ...     ",
			my_name, nr_pages);

	m = nr_pages / 100;
	if (!m)
		m = 1;

	if (early_writeout)
		writeout_rate = m;
	else
		writeout_rate = nr_pages;

	for (nr_pages = 0; ; nr_pages++) {
		ret = read(handle->dev, handle->page_buffer, page_size);
		if (ret <= 0)
			break;

		error = swap_write_page(handle);
		if (error)
			break;

		if (!(nr_pages % m)) {
			printf("\b\b\b\b%3d%%", nr_pages / m);
			splash.progress(20 + (nr_pages / m) * 0.75);

			switch (splash.key_pressed()) {
				case ABORT_KEY_CODE:
					if (abort_possible) {
						printf(" aborted!\n");
						error = -EINTR;
						goto Exit;
					}
					break;
				case REBOOT_KEY_CODE:
					printf (" reboot enabled\b\b\b\b\b\b\b"
						"\b\b\b\b\b\b\b\b");
					shutdown_method =
							SHUTDOWN_METHOD_REBOOT;
					break;
			}
		}

		if (!(nr_pages % writeout_rate))
			start_writeout(handle->fd);
	}

	if (ret < 0)
		error = -errno;

	if (!error)
		printf(" done (%u pages)\n", nr_pages);

Exit:
	if (abort_possible)
		splash.restore_abort(&savedtrm);

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

	printf("%s: Free swap: %lu kilobytes\n", my_name,
		(unsigned long)free_swap / 1024);
	return free_swap > size;
}

static struct swsusp_header swsusp_header;

static int mark_swap(int fd, loff_t start)
{
	int error = 0;
	unsigned int size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;

	if (lseek(fd, shift, SEEK_SET) != shift)
		return -EIO;

	if (read(fd, &swsusp_header, size) < size)
		return -EIO;

	if (!memcmp("SWAP-SPACE", swsusp_header.sig, 10) ||
	    !memcmp("SWAPSPACE2", swsusp_header.sig, 10)) {
		memcpy(swsusp_header.orig_sig, swsusp_header.sig, 10);
		memcpy(swsusp_header.sig, SWSUSP_SIG, 10);
		swsusp_header.image = start;
		if (lseek(fd, shift, SEEK_SET) != shift)
			return -EIO;

		if (write(fd, &swsusp_header, size) < size)
			error = -EIO;
	} else {
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
	struct swsusp_info *header = mem_pool;
	loff_t start;
	int error;
 	struct timeval begin;

	printf("%s: System snapshot ready. Preparing to write\n", my_name);
	start = get_swap_page(snapshot_fd);
	if (!start)
		return -ENOSPC;
	error = read(snapshot_fd, header, page_size);
	if (error < (int)page_size)
		return error < 0 ? error : -EFAULT;
	printf("%s: Image size: %lu kilobytes\n", my_name, header->size / 1024);
	if (!enough_swap(snapshot_fd, header->size) && !do_compress) {
		fprintf(stderr, "%s: Not enough free swap\n", my_name);
		return -ENOSPC;
	}
	error = init_swap_writer(&handle, snapshot_fd, resume_fd,
			(char *)mem_pool + page_size);
	if (error)
		goto Exit;

	header->map_start = handle.cur_swap;
	header->image_flags = 0;
	max_block_size = page_size;
	if (compute_checksum)
		header->image_flags |= IMAGE_CHECKSUM;

	if (do_compress) {
		header->image_flags |= IMAGE_COMPRESSED;
		/*
		 * The formula below follows from the worst-case expansion
		 * calculation for LZO1.  sizeof(short) is due to the size field
		 * in 'struct buf_block'.
		 */
		max_block_size += (page_size >> 4) + 67 + sizeof(short);
	}

#ifdef CONFIG_ENCRYPT
	if (!do_encrypt)
		goto Save_image;

	if (use_RSA) {
		error = gcry_cipher_setkey(cipher_handle, key_data->key,
						KEY_SIZE);
		if (error)
			goto No_RSA;

		error = gcry_cipher_setiv(cipher_handle, key_data->ivec,
						CIPHER_BLOCK);
		if (error)
			goto No_RSA;

		header->image_flags |= IMAGE_ENCRYPTED | IMAGE_USE_RSA;
		memcpy(&header->rsa, &key_data->rsa, sizeof(struct RSA_data));
		memcpy(&header->key, &key_data->encrypted_key,
						sizeof(struct encrypted_key));
	} else {
		int j;

No_RSA:
		encrypt_init(key_data->key, key_data->ivec, password);
		splash.progress(20);
		get_random_salt(header->salt, CIPHER_BLOCK);
		for (j = 0; j < CIPHER_BLOCK; j++)
			key_data->ivec[j] ^= header->salt[j];

		error = gcry_cipher_setkey(cipher_handle, key_data->key,
						KEY_SIZE);
		if (!error)
			error = gcry_cipher_setiv(cipher_handle, key_data->ivec,
						CIPHER_BLOCK);
		if (!error)
			header->image_flags |= IMAGE_ENCRYPTED;
	}

	if (error) {
		fprintf(stderr,"%s: libgcrypt error: %s\n", my_name,
			gcry_strerror(error));
		goto Exit;
	}

Save_image:
#endif
	gettimeofday(&begin, NULL);

	error = save_image(&handle, header->pages - 1);
	if (error)
		goto Exit;

	/*
	 * NOTICE: This needs to go after save_image(), because the user may
	 * modify the behavior.
	 */
	if (shutdown_method == SHUTDOWN_METHOD_PLATFORM)
		header->image_flags |= PLATFORM_SUSPEND;

	error = flush_swap_writer(&handle);
	if (!error) {
		struct timeval end;

		fsync(resume_fd);

		if (compute_checksum)
			md5_finish_ctx(&handle.ctx, header->checksum);

		gettimeofday(&end, NULL);
		timersub(&end, &begin, &end);
		header->writeout_time = end.tv_usec / 1000000.0 + end.tv_sec;

		error = write_area(resume_fd, header, start, page_size);
	}

	if (!error) {
		if (do_compress) {
			double delta = header->size - compr_diff;

			printf("%s: Compression ratio %4.2lf\n", my_name,
				delta / header->size);
		}
		printf( "S" );
		error = mark_swap(resume_fd, start);
	}

	fsync(resume_fd);

	if (!error)
		printf( "|" );

Exit:
	printf("\n");
	return error;
}

#ifdef CONFIG_BOTH
static int reset_signature(int fd)
{
	int ret, error = 0;
	unsigned int size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;

	if (lseek(fd, shift, SEEK_SET) != shift)
		return -EIO;

	memset(&swsusp_header, 0, size);
	ret = read(fd, &swsusp_header, size);
	if (ret == size) {
		if (memcmp(SWSUSP_SIG, swsusp_header.sig, 10)) {
			/* Impossible? We wrote signature and it is not there?! */
			error = -EINVAL;
		}
	} else {
		error = ret < 0 ? ret : -EIO;
	}

	if (!error) {
		/* Reset swap signature now */
		memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
		if (lseek(fd, shift, SEEK_SET) == shift) {
			ret = write(fd, &swsusp_header, size);
			if (ret != size)
				error = ret < 0 ? ret : -EIO;
		} else {
			error = -EIO;
		}
	}
	fsync(fd);
	if (error) {
		fprintf(stderr,
			"reset_signature: Error %d resetting the image.\n"
			"There should be valid image on disk. Powerdown and do normal resume.\n"
			"Continuing with this booted system will lead to data corruption.\n", 
			error);
		while(1);
	}
	return error;
}
#endif

static void suspend_shutdown(int snapshot_fd)
{
	if (shutdown_method == SHUTDOWN_METHOD_REBOOT) {
		reboot();
	} else if (shutdown_method == SHUTDOWN_METHOD_PLATFORM) {
		int ret = platform_enter(snapshot_fd);
		if (ret < 0)
			suspend_error("pm_ops->enter failed, calling power_off.");
	}
	power_off();
	/* Signature is on disk, it is very dangerous to continue now.
	 * We'd do resume with stale caches on next boot. */
	fprintf(stderr,"Powerdown failed. That's impossible.\n");
	while(1)
		sleep (60);
}

int suspend_system(int snapshot_fd, int resume_fd)
{
	loff_t avail_swap;
	unsigned long image_size;
	int attempts, in_suspend, error = 0;

	avail_swap = check_free_swap(snapshot_fd);
	if (avail_swap > pref_image_size)
		image_size = pref_image_size;
	else
		image_size = avail_swap;
	if (!avail_swap) {
		fprintf(stderr, "%s: No swap space for suspend\n", my_name);
		return ENOSPC;
	}

	error = freeze(snapshot_fd);

	/* This a hack for a bug in bootsplash. Apparently it will
	 * drop to 'verbose mode' after the freeze() call.
	 */
	splash.switch_to();
	splash.progress(15);

	if (error)
		goto Unfreeze;

	if (shutdown_method == SHUTDOWN_METHOD_PLATFORM) {
		int ret = platform_prepare(snapshot_fd);
		if (ret < 0) {
			suspend_error("pm_ops->prepare failed, using "
				"'shutdown mode = shutdown'.");
			shutdown_method = SHUTDOWN_METHOD_SHUTDOWN;
		}
	}

	printf("%s: Snapshotting system\n", my_name);
	attempts = 2;
	do {
		if (set_image_size(snapshot_fd, image_size)) {
			error = errno;
			break;
		}
		if (atomic_snapshot(snapshot_fd, &in_suspend)) {
			error = errno;
			break;
		}
		if (!in_suspend) {
			/* first unblank the console, see console_codes(4) */
			printf("\e[13]");
			printf("%s: returned to userspace\n", my_name);
			free_snapshot(snapshot_fd);
			break;
		}

		error = write_image(snapshot_fd, resume_fd);
		if (error) {
			free_swap_pages(snapshot_fd);
			free_snapshot(snapshot_fd);
			image_size = 0;
			error = -error;
			if (error != ENOSPC)
				break;
		} else {
			splash.progress(100);
#ifdef CONFIG_BOTH
			if (s2ram) {
				/* If we die (and allow system to continue)
				 * between now and reset_signature(), very bad
				 * things will happen. */
				error = suspend_to_ram(snapshot_fd);
				if (error)
					goto Shutdown;
				reset_signature(resume_fd);
				free_swap_pages(snapshot_fd);
				free_snapshot(snapshot_fd);
				s2ram_resume();
				goto Unfreeze;
			}
Shutdown:
#endif
			suspend_shutdown(snapshot_fd);
		}
	} while (--attempts);

	/* We get here during the resume or when we failed to suspend.
	 * Remember, suspend_shutdown() never returns!
	 */
	if (shutdown_method == SHUTDOWN_METHOD_PLATFORM)
		platform_finish(snapshot_fd);

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

#ifndef TIOCL_GETKMSGREDIRECT
#define TIOCL_GETKMSGREDIRECT	17
#endif

static int set_kmsg_redirect;

/**
 *	prepare_console - find a spare virtual terminal, open it and attach
 *	the standard streams to it.  The number of the currently active
 *	virtual terminal is saved via @orig_vc
 */

static int prepare_console(int *orig_vc, int *new_vc)
{
	int fd, error, vt = -1;
	char *vt_name = mem_pool;
	struct vt_stat vtstat;
	char clear_vt, tiocl[2];

	fd = console_fd("/dev/console");
	if (fd < 0)
		return fd;

	tiocl[0] = TIOCL_GETKMSGREDIRECT;
	if (!ioctl(fd, TIOCLINUX, tiocl)) {
		if (tiocl[0] > 0)
			vt = tiocl[0];
	}

	clear_vt = 0;
	error = ioctl(fd, VT_GETSTATE, &vtstat);
	if (!error) {
		*orig_vc = vtstat.v_active;
		if (vt < 0) {
			clear_vt = 1;
			error = ioctl(fd, VT_OPENQRY, &vt);
		}
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
		suspend_error("Could not activate the VT %d.", vt);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, vt);
	if (error) {
		suspend_error("VT %d activation failed.", vt);
		fflush(stderr);
		goto Close_fd;
	}

	if (clear_vt) {
		char *msg = "\33[H\33[J";
		write(fd, msg, strlen(msg));
	}

	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	*new_vc = vt;

	set_kmsg_redirect = !tiocl[0];
	if (set_kmsg_redirect) {
		tiocl[0] = TIOCL_SETKMSGREDIRECT;
		tiocl[1] = vt;
		if (ioctl(fd, TIOCLINUX, tiocl)) {
			suspend_error("Failed to redirect kernel messages "
				"to VT %d.", vt);
			fflush(stderr);
			set_kmsg_redirect = 0;
		}
	}

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
		suspend_error("Could not activate the VT %d.", orig_vc);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, orig_vc);
	if (error) {
		suspend_error("VT %d activation failed.", orig_vc);
		fflush(stderr);
	}
	if (set_kmsg_redirect) {
		char tiocl[2];

		tiocl[0] = TIOCL_SETKMSGREDIRECT;
		tiocl[1] = 0;
		ioctl(fd, TIOCLINUX, tiocl);
	}
Close_fd:
	close(fd);
}

static FILE *swappiness_file;

static inline void open_swappiness(void)
{
	swappiness_file = fopen("/proc/sys/vm/swappiness", "r+");
}

static inline int get_swappiness(void)
{
	int swappiness = -1;

	if (swappiness_file) {
		rewind(swappiness_file);
		fscanf(swappiness_file, "%d", &swappiness);
	}
	return swappiness;
}

static inline void set_swappiness(int swappiness)
{
	if (swappiness_file) {
		rewind(swappiness_file);
		fprintf(swappiness_file, "%d\n", swappiness);
		fflush(swappiness_file);
	}
}

static inline void close_swappiness(void)
{
	if (swappiness_file)
		fclose(swappiness_file);
}

#ifdef CONFIG_ENCRYPT
static void generate_key(void)
{
	gcry_ac_handle_t rsa_hd;
	gcry_ac_data_t rsa_data_set, key_set;
	gcry_ac_key_t rsa_pub;
	gcry_mpi_t mpi;
	int ret, fd, rnd_fd;
	struct RSA_data *rsa;
	unsigned char *buf;
	unsigned short size;
	int j;

	if (!key_data)
		return;

	fd = open(key_name, O_RDONLY);
	if (fd < 0)
		return;

	rsa = &key_data->rsa;
	if (read(fd, rsa, sizeof(struct RSA_data)) <= 0)
		goto Close;

	ret = gcry_ac_open(&rsa_hd, GCRY_AC_RSA, 0);
	if (ret)
		goto Close;

	buf = rsa->data;
	ret = gcry_ac_data_new(&rsa_data_set);
	if (ret)
		goto Free_rsa;

	for (j = 0; j < RSA_FIELDS_PUB; j++) {
		size_t s = rsa->size[j];

		gcry_mpi_scan(&mpi, GCRYMPI_FMT_USG, buf, s, NULL);
		ret = gcry_ac_data_set(rsa_data_set, GCRY_AC_FLAG_COPY,
					rsa->field[j], mpi);
		gcry_mpi_release(mpi);
		if (ret)
			break;

		buf += s;
	}
	if (!ret)
		ret = gcry_ac_key_init(&rsa_pub, rsa_hd, GCRY_AC_KEY_PUBLIC,
					rsa_data_set);

	if (ret)
		goto Destroy_data_set;

	ret = gcry_ac_data_new(&key_set);
	if (ret)
		goto Destroy_key;

	rnd_fd = open("/dev/urandom", O_RDONLY);
	if (rnd_fd <= 0)
		goto Destroy_key_set;

	size = KEY_SIZE + CIPHER_BLOCK;
	if (read(rnd_fd, key_data->key, size) != size)
		goto Close_urandom;

	gcry_mpi_scan(&mpi, GCRYMPI_FMT_USG, key_data->key, size, NULL);
	ret = gcry_ac_data_encrypt(rsa_hd, 0, rsa_pub, mpi, &key_set);
	gcry_mpi_release(mpi);
	if (!ret) {
		struct encrypted_key *key = &key_data->encrypted_key;
		char *str;
		size_t s;

		gcry_ac_data_get_index(key_set, GCRY_AC_FLAG_COPY, 0,
						(const char **)&str, &mpi);
		gcry_free(str);
		ret = gcry_mpi_print(GCRYMPI_FMT_USG, key->data, KEY_DATA_SIZE,
					&s, mpi);
		gcry_mpi_release(mpi);
		if (!ret) {
			key->size = s;
			use_RSA = 'y';
		}
	}

Close_urandom:
	close(rnd_fd);

Destroy_key_set:
	gcry_ac_data_destroy(key_set);

Destroy_key:
	gcry_ac_key_destroy(rsa_pub);

Destroy_data_set:
	gcry_ac_data_destroy(rsa_data_set);

Free_rsa:
	gcry_ac_close(rsa_hd);

Close:
	close(fd);
}
#endif

static void unlock_vt(void)
{
	ioctl(vfd, VT_SETMODE, &orig_vtm);
	close(vfd);
}

static int lock_vt(void)
{
	struct sigaction sa;
	struct vt_mode vtm;
	struct vt_stat vtstat;
	char *vt_name = mem_pool;
	int fd, error;

	fd = console_fd("/dev/console");
	if (fd < 0)
		return fd;

	error = ioctl(fd, VT_GETSTATE, &vtstat);
	close(fd);
	
	if (error < 0)
		return error;

	sprintf(vt_name, "/dev/tty%d", vtstat.v_active);
	vfd = open(vt_name, O_RDWR);
	if (vfd < 0)
		return vfd;

	error = ioctl(vfd, VT_GETMODE, &vtm);
	if (error < 0) 
		return error;

	/* Setting vt mode to VT_PROCESS means this process
	 * will handle vt switching requests.
	 * We just ignore all request by installing SIG_IGN.
	 */
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGUSR1, &sa, NULL);

	orig_vtm = vtm;
	vtm.mode = VT_PROCESS;
	vtm.relsig = SIGUSR1;
	vtm.acqsig = SIGUSR1;
	error = ioctl(vfd, VT_SETMODE, &vtm);
	if (error < 0)
		return error;

	return 0;
}

/* Parse the command line and/or configuration file */
static inline int get_config(int argc, char *argv[])
{
	static struct option options[] = {
		   {
		       "help\0\t\t\tthis text",
		       no_argument,		NULL, 'h'
		   },
		   {
		       "config\0\t\talternative configuration file.",
		       required_argument,	NULL, 'f'
		   },
		   {
		       "resume_device\0device that contains swap area",	
		       required_argument,	NULL, 'r'
		   },
		   {
		       "resume_offset\0offset of swap file in resume device.",	
		       required_argument,	NULL, 'o'
		   },
		   {
		       "image_size\0\tdesired size of the image.",
		       required_argument,	NULL, 's'
		   },
		   {
		       "parameter\0\toverride config file parameter.",
		       required_argument,	NULL, 'P'
		   },
#ifdef CONFIG_BOTH
		   HACKS_LONG_OPTS
#endif
		   { NULL,		0,			NULL,  0 }
	};
	int i, error;
	char *conf_name = CONFIG_FILE;
	const char *optstring = "hf:s:o:r:P:";
	struct stat stat_buf;
	int fail_missing_config = 0;

	/* parse only config file argument */
	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage(my_name, options, optstring);
			exit(EXIT_SUCCESS);
		case 'f':
			conf_name = optarg;
			fail_missing_config = 1;
			break;
		}
	}

	if (stat(conf_name, &stat_buf)) {
		if (fail_missing_config) {
			fprintf(stderr, "%s: Could not stat configuration file\n",
				my_name);
			return -ENOENT;
		}
	}
	else {
		error = parse(my_name, conf_name, parameters);
		if (error) {
			fprintf(stderr, "%s: Could not parse config file\n", my_name);
			return error;
		}
	}

	optind = 0;
	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
		case 'f':
			/* already handled */
			break;
		case 's':
			pref_image_size = atoll(optarg);
			break;
		case 'o':
			resume_offset = atoll(optarg);
			break;
		case 'r':
			strncpy(resume_dev_name, optarg, MAX_STR_LEN -1);
			break;
		case 'P':
			error = parse_line(optarg, parameters);
			if (error) {
				fprintf(stderr, "%s: Could not parse config string '%s'\n", my_name, optarg);
				return error;
			}
			break;
		default:
#ifdef CONFIG_BOTH
			s2ram_add_flag(i, optarg);
			break;
#else
			usage(my_name, options, optstring);
			return -EINVAL;
#endif
		}
	}

	if (optind < argc)
		strncpy(resume_dev_name, argv[optind], MAX_STR_LEN - 1);

#ifdef CONFIG_BOTH
	s2ram = s2ram_is_supported();
	/* s2ram_is_supported returns EINVAL if there was something wrong
	 * with the options that where added with s2ram_add_flag.
	 * On any other error (unsupported) we will just continue with s2disk.
	 */
	if (s2ram == EINVAL)
		return -EINVAL;
	
	s2ram = !s2ram;
#endif

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned int mem_size;
	struct stat stat_buf;
	int resume_fd, snapshot_fd, vt_fd, orig_vc = -1, suspend_vc = -1;
	dev_t resume_dev;
	int orig_loglevel, orig_swappiness, ret;
	struct rlimit rlim;
	static char chroot_path[MAX_STR_LEN];

	my_name = basename(argv[0]);

	/* Make sure the 0, 1, 2 descriptors are open before opening the
	 * snapshot and resume devices
	 */
	do {
		ret = open("/dev/null", O_RDWR);
		if (ret < 0) {
			ret = errno;
			suspend_error("Could not open /dev/null.");
			return ret;
		}
	} while (ret < 3);
	close(ret);

	ret = get_config(argc, argv);
	if (ret)
		return -ret;

	if (compute_checksum != 'y' && compute_checksum != 'Y')
		compute_checksum = 0;
#ifdef CONFIG_COMPRESS
	if (do_compress != 'y' && do_compress != 'Y') {
		do_compress = 0;
	} else if (lzo_init() != LZO_E_OK) {
		suspend_error("Failed to initialize LZO. "
				"Compression disabled.\n");
		do_compress = 0;
	}
#endif
#ifdef CONFIG_ENCRYPT
	if (do_encrypt != 'y' && do_encrypt != 'Y')
		do_encrypt = 0;
#endif
	if (splash_param != 'y' && splash_param != 'Y')
		splash_param = 0;
	else
		splash_param = SPL_SUSPEND;

	if (early_writeout != 'n' && early_writeout != 'N')
		early_writeout = 1;

	if (!strcmp (shutdown_method_value, "shutdown")) {
		shutdown_method = SHUTDOWN_METHOD_SHUTDOWN;
	}
	else if (!strcmp (shutdown_method_value, "platform")) {
		shutdown_method = SHUTDOWN_METHOD_PLATFORM;
	}
	else if (!strcmp (shutdown_method_value, "reboot")) {
		shutdown_method = SHUTDOWN_METHOD_REBOOT;
	}

	page_size = getpagesize();
	buffer_size = BUFFER_PAGES * page_size;

	mem_size = 3 * page_size + buffer_size;
#ifdef CONFIG_COMPRESS
	if (do_compress)
		mem_size += LZO1X_1_MEM_COMPRESS;
#endif
#ifdef CONFIG_ENCRYPT
	if (do_encrypt) {
		printf("%s: libgcrypt version: %s\n", my_name,
			gcry_check_version(NULL));
		gcry_control(GCRYCTL_INIT_SECMEM, page_size, 0);
		ret = gcry_cipher_open(&cipher_handle, IMAGE_CIPHER,
				GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_SECURE);
		if (ret) {
			fprintf(stderr, "%s: libgcrypt error %s\n", my_name, 
				gcry_strerror(ret));
			do_encrypt = 0;
		} else {
			mem_size += buffer_size;
		}
	}
#endif
	mem_pool = malloc(mem_size);
	if (!mem_pool) {
		ret = errno;
		suspend_error("Could not allocate memory.");
		return ret;
	}
#ifdef CONFIG_ENCRYPT
	if (do_encrypt) {
		mem_size -= buffer_size;
		key_data = (struct key_data *)((char *)mem_pool + mem_size);
		generate_key();
	}
#endif

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		ret = errno;
		suspend_error("Could not lock myself.");
		return ret;
	}

	snprintf(chroot_path, MAX_STR_LEN, "/proc/%d", getpid());
	if (mount("none", chroot_path, "tmpfs", 0, NULL)) {
		ret = errno;
		suspend_error("Could not mount tmpfs on %s.", chroot_path);
		return ret;
	}

	ret = 0;
	if (stat(resume_dev_name, &stat_buf)) {
		suspend_error("Could not stat the resume device file.");
		ret = ENODEV;
		goto Umount;
	}
	if (!S_ISBLK(stat_buf.st_mode)) {
		fprintf(stderr, "%s: Invalid resume device\n", my_name);
		ret = EINVAL;
		goto Umount;
	}
	if (chdir(chroot_path)) {
		ret = errno;
		suspend_error("Could not change directory to %s.",
			chroot_path);
		goto Umount;
	}
	resume_dev = stat_buf.st_rdev;
	if (mknod("resume", S_IFBLK | 0600, resume_dev)) {
		ret = errno;
		suspend_error("Could not create %s/%s.", chroot_path, "resume");
		goto Umount;
	}
	resume_fd = open("resume", O_RDWR);
	if (resume_fd < 0) {
		ret = errno;
		suspend_error("Could not open the resume device.");
		goto Umount;
	}

	if (stat(snapshot_dev_name, &stat_buf)) {
		suspend_error("Could not stat the snapshot device file.");
		ret = ENODEV;
		goto Close_resume_fd;
	}

	if (!S_ISCHR(stat_buf.st_mode)) {
		fprintf(stderr, "%s: Invalid snapshot device\n", my_name);
		ret = EINVAL;
		goto Close_resume_fd;
	}
	snapshot_fd = open(snapshot_dev_name, O_RDONLY);
	if (snapshot_fd < 0) {
		ret = errno;
		suspend_error("Could not open the snapshot device.");
		goto Close_resume_fd;
	}

	if (set_swap_file(snapshot_fd, resume_dev, resume_offset)) {
		ret = errno;
		suspend_error("Could not use the resume device "
			"(try swapon -a).");
		goto Close_snapshot_fd;
	}

	vt_fd = prepare_console(&orig_vc, &suspend_vc);
	if (vt_fd < 0) {
		ret = errno;
		suspend_error("Could not open a virtual terminal.");
		goto Close_snapshot_fd;
	}

	splash_prepare(&splash, splash_param);

	if (lock_vt() < 0) {
		ret = errno;
		suspend_error("Could not lock the terminal.");
		goto Restore_console;
	}

	splash.progress(5);

#ifdef CONFIG_BOTH
	/* If s2ram_hacks returns != 0, better not try to suspend to RAM */
	if (s2ram) 
		s2ram = !s2ram_hacks();
#endif
#ifdef CONFIG_ENCRYPT
        if (do_encrypt && ! use_RSA) {
                splash.read_password((char *)mem_pool,1);
                strncpy(password,(char *)mem_pool,PASS_SIZE);
        }
#endif

	open_printk();
	orig_loglevel = get_kernel_console_loglevel();
	set_kernel_console_loglevel(suspend_loglevel);

	open_swappiness();
	orig_swappiness = get_swappiness();
	set_swappiness(suspend_swappiness);

	sync();

	splash.progress(10);

	rlim.rlim_cur = 0;
	rlim.rlim_max = 0;
	setrlimit(RLIMIT_NOFILE, &rlim);
	setrlimit(RLIMIT_NPROC, &rlim);
	setrlimit(RLIMIT_CORE, &rlim);

	ret = suspend_system(snapshot_fd, resume_fd);

	if (orig_loglevel >= 0)
		set_kernel_console_loglevel(orig_loglevel);

	close_printk();

	if(orig_swappiness >= 0)
		set_swappiness(orig_swappiness);
	close_swappiness();

	unlock_vt();
Restore_console:
	splash.finish();
	restore_console(vt_fd, orig_vc);
Close_snapshot_fd:
	close(snapshot_fd);
Close_resume_fd:
	close(resume_fd);
Umount:
	if (chdir("/")) {
		ret = errno;
		suspend_error("Could not change directory to /");
	} else {
		umount(chroot_path);
	}
#ifdef CONFIG_ENCRYPT
	if (do_encrypt)
		gcry_cipher_close(cipher_handle);
#endif
	free(mem_pool);

	return ret;
}
