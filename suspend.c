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
static long long compr_size;
static unsigned int compress_buf_size;
#else
#define do_compress 0
#define compr_size 0
#define compress_buf_size 0
#endif
#ifdef CONFIG_ENCRYPT
static char do_encrypt;
static char use_RSA;
static char key_name[MAX_STR_LEN] = SUSPEND_KEY_FILE;
static char password[PASS_SIZE];
static unsigned long encrypt_buf_size;
#else
#define do_encrypt 0
#define key_name NULL
#define encrypt_buf_size 0
#endif
#ifdef CONFIG_BOTH
static char s2ram;
#endif
static char early_writeout;
static char splash_param;
#ifdef CONFIG_FBSPLASH
char fbsplash_theme[MAX_STR_LEN] = "";
#endif
#define SHUTDOWN_LEN	16
static char shutdown_method_value[SHUTDOWN_LEN] = "";
static enum {
	SHUTDOWN_METHOD_SHUTDOWN,
	SHUTDOWN_METHOD_PLATFORM,
	SHUTDOWN_METHOD_REBOOT
} shutdown_method = SHUTDOWN_METHOD_PLATFORM;
static int resume_pause;

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
#ifdef CONFIG_FBSPLASH
	{
		.name = "fbsplash theme",
		.fmt = "%s",
		.ptr = fbsplash_theme,
		.len = MAX_STR_LEN,
	},
#endif
	{
		.name = "resume pause",
		.fmt = "%d",
		.ptr = &resume_pause,
	},
	{
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};

static unsigned int page_size;
/* This MUST be an multipe of page_size */
static unsigned int buffer_size;
static void *mem_pool;
#ifdef CONFIG_ENCRYPT
struct key_data *key_data;
gcry_cipher_hd_t cipher_handle;
#endif

static loff_t check_free_swap(int dev)
{
	int error;
	loff_t free_swap;

	error = ioctl(dev, SNAPSHOT_AVAIL_SWAP_SIZE, &free_swap);
	if (error && errno == ENOTTY) {
		report_unsupported_ioctl("SNAPSHOT_AVAIL_SWAP_SIZE");
		error = ioctl(dev, SNAPSHOT_AVAIL_SWAP, &free_swap);
	}
	if (!error)
		return free_swap;

	suspend_error("check_free_swap failed.");
	return 0;
}

static loff_t get_image_size(int dev)
{
	int error;
	loff_t image_size;

	error = ioctl(dev, SNAPSHOT_GET_IMAGE_SIZE, &image_size);
	if (!error)
		return image_size;

	if (errno == ENOTTY)
		report_unsupported_ioctl("SNAPSHOT_GET_IMAGE_SIZE");
	suspend_error("get_image_size failed.");
	return 0;
}

static loff_t alloc_swap_page(int dev, int verbose)
{
	int error;
	loff_t offset;

	error = ioctl(dev, SNAPSHOT_ALLOC_SWAP_PAGE, &offset);
	if (error && errno == ENOTTY) {
		if (verbose)
			report_unsupported_ioctl("SNAPSHOT_ALLOC_SWAP_PAGE");
		error = ioctl(dev, SNAPSHOT_GET_SWAP_PAGE, &offset);
	}
	if (!error)
		return offset;
	return 0;
}

static inline loff_t get_swap_page(int dev)
{
	return alloc_swap_page(dev, 0);
}

static inline int free_swap_pages(int dev)
{
	return ioctl(dev, SNAPSHOT_FREE_SWAP_PAGES, 0);
}

static int set_swap_file(int dev, u_int32_t blkdev, loff_t offset)
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

static int atomic_snapshot(int dev, int *in_suspend)
{
	int error;

	error = ioctl(dev, SNAPSHOT_CREATE_IMAGE, in_suspend);
	if (error && errno == ENOTTY) {
		report_unsupported_ioctl("SNAPSHOT_CREATE_IMAGE");
		error = ioctl(dev, SNAPSHOT_ATOMIC_SNAPSHOT, in_suspend);
	}
	return error;
}

static inline int free_snapshot(int dev)
{
	return ioctl(dev, SNAPSHOT_FREE, 0);
}

static int set_image_size(int dev, unsigned int size)
{
	int error;

	error = ioctl(dev, SNAPSHOT_PREF_IMAGE_SIZE, size);
	if (error && errno == ENOTTY) {
		report_unsupported_ioctl("SNAPSHOT_PREF_IMAGE_SIZE");
		error = ioctl(dev, SNAPSHOT_SET_IMAGE_SIZE, size);
	}
	return error;
}

static inline int suspend_to_ram(int dev)
{
	return ioctl(dev, SNAPSHOT_S2RAM, 0);
}

static int platform_enter(int dev)
{
	int error;

	error = ioctl(dev, SNAPSHOT_POWER_OFF, 0);
	if (error  && errno == ENOTTY) {
		report_unsupported_ioctl("SNAPSHOT_POWER_OFF");
		error = ioctl(dev, SNAPSHOT_PMOPS, PMOPS_ENTER);
	}
	return error;
}

/**
 *	alloc_swap - allocate a number of swap pages
 *	@dev:		Swap device to use for allocations.
 *	@extents:	Array of extents to track the allocations.
 *	@nr_extents:	Number of extents already in the array.
 *	@size_p:	Points to the number of bytes to allocate, used to
 *			return the number of allocated bytes.
 *
 *	Allocate the number of swap pages sufficient for saving the number of
 *	bytes pointed to by @size_p.  Use the array @extents to track the
 *	allocations.  This array has to be page_size big and may already
 *	contain some initial elements (in that case @nr_extents must be the
 *	number of these elements).
 *	Each element of the array represents an area of allocated swap space.
 *	These areas may be extended when swap pages that can be added to them
 *	are found.  They also can be merged with one another.
 *	The function returns when the requested amount of swap space is
 *	allocated or if there is no room for more extents.  In the latter case
 *	the last extent created is put at the end of the array and may be passed
 *	to alloc_swap() as the initial extent when it is invoked next time.
 */
static int
alloc_swap(int dev, struct extent *extents, int nr_extents, loff_t *size_p)
{
	const int max_extents = page_size / sizeof(struct extent) - 1;
	loff_t size, total_size, offset;

	total_size = *size_p;
	if (nr_extents <= 0) {
		offset = get_swap_page(dev);
		if (!offset)
			return -ENOSPC;
		extents->start = offset;
		extents->end = offset + page_size;
		nr_extents = 1;
		size = page_size;
	} else {
		size = 0;
	}
	while (size < total_size && nr_extents <= max_extents) {
		int i, j;

		offset = get_swap_page(dev);
		if (!offset)
			return -ENOSPC;
		/* Check if we have a matching extent. */
		i = 0;
		j = nr_extents - 1;
		do {
			struct extent *ext;
			int k = (i + j) / 2;

 Repeat:
			ext = extents + k;
			if (offset == ext->start - page_size) {
				ext->start = offset;
				/* Check if we can merge extents */
				if (k > 0 && extents[k-1].end == offset) {
					extents[k-1].end = ext->end;
					/* Pull the 'later' extents forward */
					memmove(ext, ext + 1,
						(nr_extents - k - 1) *
							sizeof(*ext));
					nr_extents--;
				}
				offset = 0;
				break;
			} else if (offset == ext->end) {
				ext->end += page_size;
				/* Check if we can merge extents */
				if (k + 1 < nr_extents
				    && ext->end == extents[k+1].start) {
					ext->end = extents[k+1].end;
					/* Pull the 'later' extents forward */
					memmove(ext + 1, ext + 2,
						(nr_extents - k - 2) *
							sizeof(*ext));
					nr_extents--;
				}
				offset = 0;
				break;
			} else if (offset > ext->end) {
				if (i == k) {
					if (i < j) {
						/* This means i == j + 1 */
						k = j;
						i = j;
						goto Repeat;
					}
				} else {
					i = k;
				}
			} else {
				/* offset < ext->start - page_size */
				j = k;
			}
		} while (i < j);
		if (offset > 0) {
			/* No match.  Create a new extent. */
			struct extent *ext;

			if (nr_extents < max_extents) {
				ext = extents + i;
				/*
				 * We want to always replace the extent 'i' with
				 * the new one.
				 */
				if (offset > ext->end) {
					i++;
					ext++;
				}
				/* Push the 'later' extents backwards. */
				memmove(ext + 1, ext,
					(nr_extents - i) * sizeof(*ext));
			} else {
				ext = extents + nr_extents;
			}
			ext->start = offset;
			ext->end = offset + page_size;
			nr_extents++;
		}
		size += page_size;
	}
	*size_p = size;
	return nr_extents;
}

/**
 *	write_page - Write page_size data to given swap location.
 *	@fd:		File handle of the resume partition.
 *	@buf:		Pointer to the area we're writing.
 *	@offset:	Offset of the swap page we're writing to.
 */
static int write_page(int fd, void *buf, loff_t offset)
{
	int res = 0;
	ssize_t cnt = 0;

	if (!offset)
		return -EINVAL;

	if (lseek(fd, offset, SEEK_SET) == offset)
		cnt = write(fd, buf, page_size);
	if (cnt != page_size)
		res = -EIO;
	return res;
}

/*
 * The swap_map_handle structure is used for handling swap in a file-alike way.
 *
 * @extents:	Array of extents used for trackig swap allocations.  It is
 *		page_size bytes large and holds at most
 *		(page_size / sizeof(struct extent) - 1) extents.  The last slot
 *		is used to hold the extent that will be used as an initial one
 *		for the next batch of allocations.
 *
 * @nr_extents:		Number of entries in @extents actually used.
 *
 * @cur_extent:		The extent currently used as the source of swap pages.
 *
 * @cur_extent_idx:	The index of @cur_extent.
 *
 * @cur_offset:		The offset of the swap page that will be used next.
 *
 * @swap_needed:	The amount of swap needed for saving the image.
 *
 * @written_data:	The amount of data actually saved.
 *
 * @extents_spc:	The swap page to which to save @extents.
 *
 * @buffer:		Buffer used for storing image data pages.
 *
 * @write_buffer:	If compression is used, the compressed contents of
 *			@buffer are stored here.  Otherwise, it is equal to
 *			@buffer.
 *
 * @page_buffer:	Pointer to the addres to write the next image page to.
 *
 * @dev:		Snapshot device handle used for reading image pages and
 *			invoking ioctls.
 *
 * @fd:			File handle associated with the swap.
 *
 * @ctx:		Used for checksum computing, if so configured.
 *
 * @lzo_work_buffer:	Work buffer used for compression.
 *
 * @encrypt_buffer:	Buffer for storing encrypted pages (page_size bytes).
 *
 * @encrypt_ptr:	Address to store the next encrypted page at.
 */
struct swap_map_handle {
	struct extent *extents;
	int nr_extents;
	struct extent *cur_extent;
	int cur_extent_idx;
	loff_t cur_offset;
	loff_t swap_needed;
	loff_t written_data;
	loff_t extents_spc;
	void *buffer;
	void *write_buffer;
	void *page_buffer;
	int dev, fd;
	struct md5_ctx ctx;
#ifdef CONFIG_COMPRESS
	void *lzo_work_buffer;
#endif
#ifdef CONFIG_ENCRYPT
	void *encrypt_buffer;
	void *encrypt_ptr;
#endif
};

/**
 *	init_swap_writer - initialize the structure used for saving the image
 *	@handle:	Structure to initialize.
 *	@dev:		Special device file to read image pages from.
 *	@fd:		File descriptor associated with the swap.
 *	@buf:		Memory pool to use for buffers.
 *
 *	It doesn't preallocate swap, so preallocate_swap() has to be called on
 *	@handle after this.
 */
static int
init_swap_writer(struct swap_map_handle *handle, int dev, int fd, void *buf)
{
	loff_t offset;

	if (!buf)
		return -EINVAL;

	handle->extents = buf;
	buf += page_size;

	handle->buffer = buf;
	handle->page_buffer = buf;
	handle->write_buffer = buf;
	buf += buffer_size;

#ifdef CONFIG_ENCRYPT
	if (do_encrypt) {
		handle->encrypt_buffer = buf;
		handle->encrypt_ptr = buf;
		buf += encrypt_buf_size;
	}
#endif

#ifdef CONFIG_COMPRESS
	if (do_compress) {
		handle->write_buffer = buf;
		buf += compress_buf_size;
		handle->lzo_work_buffer = buf;
		/* This buffer must hold at least LZO1X_1_MEM_COMPRESS bytes */
	}
#endif

	handle->dev = dev;
	handle->fd = fd;
	handle->written_data = 0;

	memset(handle->extents, 0, page_size);
	handle->nr_extents = 0;
	offset = get_swap_page(dev);
	if (!offset)
		return -ENOSPC;
	handle->extents_spc = offset;

	if (compute_checksum)
		md5_init_ctx(&handle->ctx);

	return 0;
}

/**
 *	preallocate_swap - use alloc_swap() to preallocate the number of pages
 *			given by @handle->swap_needed
 *	@handle:	Pointer to the structure in which to store information
 *			about the preallocated swap pool.
 *
 *	Returns the offset of the first swap page available from the
 *	preallocated pool.
 */
static loff_t preallocate_swap(struct swap_map_handle *handle)
{
	const int max = page_size / sizeof(struct extent) - 1;
	loff_t size;
	int nr_extents;

	if (handle->swap_needed < page_size)
		return 0;
	size = handle->swap_needed;
	if (do_compress && size > page_size)
		size /= 2;
	nr_extents = alloc_swap(handle->dev, handle->extents,
					handle->nr_extents, &size);
	if (nr_extents <= 0)
		return 0;
	handle->nr_extents = nr_extents < max ? nr_extents : max;
	handle->cur_extent = handle->extents;
	handle->cur_extent_idx = 0;
	handle->cur_offset = handle->cur_extent->start;
	return handle->cur_offset;
}

/**
 *	save_extents - save the array of extents
 *	handle:	Structure holding the pointer to the array of extents etc.
 *	finish:	If set, the last element of the extents array has to be filled
 *		with zeros.
 *
 *	Save the buffer (page) holding the array of extents to the swap
 *	location pointed to by @handle->extents_spc (this must be allocated
 *	earlier).  Before saving the last element of the array is used to store
 *	the swap offset of the next extents page (we allocate a swap page for
 *	this purpose).
 */
static int save_extents(struct swap_map_handle *handle, int finish)
{
	loff_t offset = 0;
	int error;

	if (!finish) {
		struct extent *last_extent;

		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		last_extent = handle->extents +
				page_size / sizeof(struct extent) - 1;
		last_extent->start = offset;
	}
	error = write_page(handle->fd, handle->extents, handle->extents_spc);
	handle->extents_spc = offset;
	return error;
}

/**
 *	next_swap_page - take one swap page out of the pool allocated using
 *			alloc_swap() before
 *	@handle:	Pointer to the structure containing information about
 *			the preallocated swap pool.
 */
static loff_t next_swap_page(struct swap_map_handle *handle)
{
	struct extent ext;
	int error;

	handle->cur_offset += page_size;
	if (handle->cur_offset < handle->cur_extent->end)
		return handle->cur_offset;
	/* We have exhausted the current extent.  Forward to the next one */
	handle->cur_extent++;
	handle->cur_extent_idx++;
	if (handle->cur_extent_idx < handle->nr_extents) {
		handle->cur_offset = handle->cur_extent->start;
		return handle->cur_offset;
	}
	/* No more extents.  Is there anything to pass to alloc_swap()? */
	if (handle->cur_extent->start < handle->cur_extent->end) {
		ext = *handle->cur_extent;
		memset(handle->cur_extent, 0, sizeof(struct extent));
	} else {
		memset(&ext, 0, sizeof(struct extent));
	}
	if (save_extents(handle, 0))
		return 0;
	memset(handle->extents, 0, page_size);
	*handle->extents = ext;
	return preallocate_swap(handle);
}

/**
 *	encrypt_and_save_page - encrypt a page of data and write it to the swap
 */
static int encrypt_and_save_page(struct swap_map_handle *handle, void *src)
{
	int error;
	loff_t offset;

#ifdef CONFIG_ENCRYPT
	if (do_encrypt) {
		error = gcry_cipher_encrypt(cipher_handle,
			handle->encrypt_ptr, page_size, src, page_size);
		if (error)
			return error;
		src = handle->encrypt_ptr;
		handle->encrypt_ptr += page_size;
		if (handle->encrypt_ptr - handle->encrypt_buffer
		    >= encrypt_buf_size)
			handle->encrypt_ptr = handle->encrypt_buffer;
	}
#endif
	offset = next_swap_page(handle);
	if (!offset)
		return -ENOSPC;
	error = write_page(handle->fd, src, offset);
	if (error)
		return error;
	handle->swap_needed -= page_size;
	handle->written_data += page_size;
	return 0;
}

/**
 *	save_buffer - save data stored in the buffer to the swap
 */
static int save_buffer(struct swap_map_handle *handle)
{
	ssize_t size;
	char *src;
	int error = 0;

	/* Check if there is anything to do */
	if (handle->page_buffer <= handle->buffer)
		return 0;

	size = handle->page_buffer - handle->buffer;
	if (compute_checksum)
		md5_process_block(handle->buffer, size, &handle->ctx);

	/* Compress the buffer, if necessary */
#ifdef CONFIG_COMPRESS
	if (do_compress) {
		struct buf_block *block = handle->write_buffer;
		lzo_uint cnt;

		lzo1x_1_compress(handle->buffer, size,
					(lzo_bytep)block->data, &cnt,
						handle->lzo_work_buffer);
		block->size = cnt;
		size = cnt + sizeof(size_t);
		compr_size += size;
	}
#endif
	/* If there's no compression, handle->buffer == handle->write_buffer */
	for (src = handle->write_buffer;
	     size > 0; src += page_size, size -= page_size) {
		error = encrypt_and_save_page(handle, src);
		if (error)
			break;
	}
	return error;
}

/**
 *	save_image - save the hibernation image data
 */
static int save_image(struct swap_map_handle *handle, unsigned int nr_pages)
{
	unsigned int m, writeout_rate;
	ssize_t ret;
	struct termios newtrm, savedtrm;
	int abort_possible, error = 0;
	char message[SPLASH_GENERIC_MESSAGE_SIZE];

	/* Switch the state of the terminal so that we can read the keyboard
	 * without blocking and with no echo.
	 *
	 * stdin must be attached to the terminal now.
	 */
	abort_possible = !splash.prepare_abort(&savedtrm, &newtrm);

	sprintf(message, "Saving %u image data pages", nr_pages);
	if (abort_possible)
		strcat(message, " (press " ABORT_KEY_NAME " to abort) ");
	strcat(message, "...");
	printf("%s: %s     ", my_name, message);
	splash.set_caption(message);

	m = nr_pages / 100;
	if (!m)
		m = 1;

	if (early_writeout)
		writeout_rate = m;
	else
		writeout_rate = nr_pages;

	/* The buffer may be partially filled at this point */
	for (nr_pages = 0; ; nr_pages++) {
		ret = read(handle->dev, handle->page_buffer, page_size);
		if (ret <= 0) {
			if (ret) {
				error = -errno;
				perror("\nError writing an image page");
			}
			break;
		}
		handle->page_buffer += page_size;

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
					splash.set_caption("Reboot enabled");
					shutdown_method =
							SHUTDOWN_METHOD_REBOOT;
					break;
			}
		}

		if (!(nr_pages % writeout_rate))
			start_writeout(handle->fd);

		if (handle->page_buffer - handle->buffer >= buffer_size) {
			/* The buffer is full, flush it */
			error = save_buffer(handle);
			if (error)
				break;
			handle->page_buffer = handle->buffer;
		}
	}

	if (!error) {
		/* Flush whatever's left in the buffer and save the extents */
		error = save_buffer(handle);
		if (!error)
			error = save_extents(handle, 1);
		if (!error)
			printf(" done (%u pages)\n", nr_pages);
	}

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
static int enough_swap(struct swap_map_handle *handle)
{
	loff_t free_swap = check_free_swap(handle->dev);
	loff_t size = do_compress ?
			handle->swap_needed / 2 : handle->swap_needed;

	printf("%s: Free swap: %llu kilobytes\n", my_name,
		(unsigned long long)free_swap / 1024);
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
	struct image_header_info *header;
	loff_t start;
	loff_t image_size;
	unsigned long nr_pages = 0;
	int error;
	int image_header_read = 0;
	struct timeval begin;

	printf("%s: System snapshot ready. Preparing to write\n", my_name);
	/* Allocate a swap page for the additional "userland" header */
	start = alloc_swap_page(snapshot_fd, 1);
	if (!start)
		return -ENOSPC;

	error = init_swap_writer(&handle, snapshot_fd, resume_fd,
						mem_pool + page_size);
	if (error)
		return error;

	image_size = get_image_size(snapshot_fd);
	if (image_size > 0) {
		nr_pages = (unsigned long)((image_size + page_size - 1) /
						page_size);
	} else {
		/*
		 * The kernel doesn't allow us to get the image size via ioctl,
		 * so we need to read it from the image header.
		 */
		struct swsusp_info *image_header;
		ssize_t ret;

		image_header = (struct swsusp_info *)handle.page_buffer;
		ret = read(snapshot_fd, image_header, page_size);
		if (ret < page_size)
			return ret < 0 ? ret : -EFAULT;
		handle.page_buffer += page_size;
		image_size = image_header->size;
		nr_pages = image_header->pages;
		if (!nr_pages)
			return -ENODATA;
		/* We have already read one page */
		nr_pages--;
	}
	printf("%s: Image size: %lu kilobytes\n", my_name, image_size / 1024);

	handle.swap_needed = image_size;
	if (!enough_swap(&handle)) {
		fprintf(stderr, "%s: Not enough free swap\n", my_name);
		return -ENOSPC;
	}
	if (!preallocate_swap(&handle)) {
		fprintf(stderr, "%s: Failed to allocate swap\n", my_name);
		return -ENOSPC;
	}
	/* Shift handle.cur_offset for the first call to next_swap_page() */
	handle.cur_offset -= page_size;

	header  = mem_pool;
	memset(header, 0, page_size);
	header->pages = nr_pages;
	header->flags = 0;
	header->map_start = handle.extents_spc;
	if (compute_checksum)
		header->flags |= IMAGE_CHECKSUM;

	if (do_compress)
		header->flags |= IMAGE_COMPRESSED;

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

		header->flags |= IMAGE_ENCRYPTED | IMAGE_USE_RSA;
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
			header->flags |= IMAGE_ENCRYPTED;
	}

	if (error) {
		fprintf(stderr,"%s: libgcrypt error: %s\n", my_name,
			gcry_strerror(error));
		goto Exit;
	}

Save_image:
#endif
	gettimeofday(&begin, NULL);

	error = save_image(&handle, nr_pages);
	if (!error) {
		struct timeval end;

		fsync(resume_fd);

		header->image_data_size = handle.written_data;

		/*
		 * NOTICE: This needs to go after save_image(), because the
		 * user may modify the behavior.
		 */
		if (shutdown_method == SHUTDOWN_METHOD_PLATFORM)
			header->flags |= PLATFORM_SUSPEND;

		if (compute_checksum)
			md5_finish_ctx(&handle.ctx, header->checksum);

		gettimeofday(&end, NULL);
		timersub(&end, &begin, &end);
		header->writeout_time = end.tv_usec / 1000000.0 + end.tv_sec;

		header->resume_pause = resume_pause;

		error = write_page(resume_fd, header, start);
	}

	if (!error) {
		if (do_compress) {
			printf("%s: Compression ratio %4.2lf\n", my_name,
				(double)compr_size / image_size);
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
	splash.set_caption("Done.");

	if (shutdown_method == SHUTDOWN_METHOD_REBOOT) {
		reboot();
	} else if (shutdown_method == SHUTDOWN_METHOD_PLATFORM) {
		if (platform_enter(snapshot_fd))
			suspend_error("Could not enter the hibernation state, "
					"calling power_off.");
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
	char message[SPLASH_GENERIC_MESSAGE_SIZE];

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
		if (platform_prepare(snapshot_fd)) {
			suspend_error("Unable to use platform hibernation "
					"support, using shutdown mode.");
			shutdown_method = SHUTDOWN_METHOD_SHUTDOWN;
		}
	}

	sprintf(message, "Snapshotting system");
	printf("%s: %s\n", my_name, message);
	splash.set_caption(message);
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
		       "version\0\t\t\tversion information",
		       no_argument,		NULL, 'V'
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
	const char *optstring = "hVf:s:o:r:P:";
	struct stat stat_buf;
	int fail_missing_config = 0;

	/* parse only config file argument */
	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage(my_name, options, optstring);
			exit(EXIT_SUCCESS);
		case 'V':
			version(my_name, NULL);
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

	if (resume_pause > RESUME_PAUSE_MAX)
		resume_pause = RESUME_PAUSE_MAX;

	page_size = getpagesize();
	buffer_size = BUFFER_PAGES * page_size;

	mem_size = 2 * page_size + buffer_size;
#ifdef CONFIG_COMPRESS
	if (do_compress) {
		/*
		 * The formula below follows from the worst-case expansion
		 * calculation for LZO1 (size / 16 + 67) and the fact that the
		 * size of the compressed data must be stored in the buffer
		 * (sizeof(size_t)).  Additionally, we want the buffer size to
		 * be a multiple of page_size.
		 */
		compress_buf_size = buffer_size +
			((page_size + (buffer_size >> 4) + 66 + sizeof(size_t))
				& ~(page_size - 1));
		mem_size += compress_buf_size + LZO1X_1_MEM_COMPRESS;
	}
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
			encrypt_buf_size = ENCRYPT_BUF_PAGES * page_size;
			mem_size += encrypt_buf_size;
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
		if (vt_fd == -ENOTTY)
			suspend_error("No local tty. Remember to specify local " \
					"console after the remote.");
		else
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
