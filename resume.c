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

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <syscall.h>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef CONFIG_COMPRESS
#include <lzo/lzo1x.h>
#endif

#include "swsusp.h"
#include "config_parser.h"
#include "md5.h"
#include "splash.h"
#include "loglevel.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static loff_t resume_offset;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static int max_loglevel = MAX_LOGLEVEL;
static char verify_checksum;
#ifdef CONFIG_COMPRESS
static char do_decompress;
static unsigned int decompress_buf_size;
#else
#define do_decompress 0
#define decompress_buf_size 0
#endif
#ifdef CONFIG_ENCRYPT
static char do_decrypt;
#else
#define do_decrypt 0
#endif
static char splash_param;
#ifdef CONFIG_FBSPLASH
char fbsplash_theme[MAX_STR_LEN] = "";
#endif
static int use_platform_suspend;

static struct splash splash;
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
#ifdef CONFIG_COMPRESS
	{
		.name = "compress",
		.fmt = "%c",
		.ptr = NULL,
	},
#endif
#ifdef CONFIG_ENCRYPT
	{
		.name = "encrypt",
		.fmt = "%c",
		.ptr = NULL,
	},
	{
		.name = "RSA key file",
		.fmt = "%s",
		.ptr = NULL,
	},
#endif
	{
		.name = "early writeout",
		.fmt = "%c",
		.ptr = NULL,
	},
	{
		.name = "splash",
		.fmt = "%c",
		.ptr = &splash_param,
	},
	{
		.name = "shutdown method",
		.fmt = "%s",
		.ptr = NULL,
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
		.ptr = NULL,
	},
	{
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};

static inline int atomic_restore(int dev)
{
	return ioctl(dev, SNAPSHOT_ATOMIC_RESTORE, 0);
}

static unsigned int page_size;
static unsigned int buffer_size;
static void *mem_pool;

/**
 *	read_page - Read data from a swap location
 *	@fd:		File handle of the resume partition.
 *	@buf:		Pointer to the area we're reading into.
 *	@offset:	Swap offset of the place to read from.
 */
static int read_page(int fd, void *buf, loff_t offset)
{
	int res = 0;
	ssize_t cnt = 0;

	if (!offset)
		return 0;

	if (lseek(fd, offset, SEEK_SET) == offset)
		cnt = read(fd, buf, page_size);
	if (cnt < (ssize_t)page_size)
		res = -EIO;

	return res;
}

/*
 * The swap_map_handle structure is used for handling swap in a file-alike way.
 *
 * @extents:	Array of extents used for trackig swap allocations.  It is
 *		page_size bytes large and holds at most
 *		(page_size / sizeof(struct extent) - 1) extents.  The last slot
 *		must be all zeros and is the end marker.
 *
 * @cur_extent:		The extent currently used as the source of swap pages.
 *
 * @cur_offset:		The offset of the swap page that will be used next.
 *
 * @total_size:		The amount of data to read.
 *
 * @buffer:		Buffer used for storing image data pages.
 *
 * @read_buffer:	If compression is used, the compressed contents of
 *			@buffer are stored here.  Otherwise, it is equal to
 *			@buffer.
 *
 * @fd:			File handle associated with the swap.
 *
 * @ctx:		Used for checksum computing, if so configured.
 *
 * @lzo_work_buffer:	Work buffer used for decompression.
 *
 * @decrypt_buffer:	Buffer for storing encrypted pages (page_size bytes).
 *
 * cipher_handle:	gcrypt's data, used for decryption.
 */
struct swap_map_handle {
	struct extent *extents;
	struct extent *cur_extent;
	loff_t cur_offset;
	loff_t next_extents;
	loff_t total_size;
	void *buffer;
	void *read_buffer;
	int fd;
	struct md5_ctx ctx;
#ifdef CONFIG_COMPRESS
	void *lzo_work_buffer;
#endif
#ifdef CONFIG_ENCRYPT
	char *decrypt_buffer;
	gcry_cipher_hd_t cipher_handle;
#endif
};

/**
 *	load_extents_page - load the array of extents
 *	handle:	Structure holding the pointer to the array of extents etc.
 *
 *	Read the table of extents from the swap location pointed to by
 *	@handle->next_extents and store it at the address in @handle->extents.
 *	Read the swap location of the next array of extents from the last
 *	element of the array and fill this element with zeros.  Initialize
 *	@handle->cur_extent and @handle->cur_offset as appropriate.
 */
static int load_extents_page(struct swap_map_handle *handle)
{
	int error, n;

	error = read_page(handle->fd, handle->extents, handle->next_extents);
	if (error)
		return error;
	n = page_size / sizeof(struct extent) - 1;
	handle->next_extents = handle->extents[n].start;
	memset(handle->extents + n, 0, sizeof(struct extent));
	handle->cur_extent = handle->extents;
	handle->cur_offset = handle->cur_extent->start;
	if (posix_fadvise(handle->fd, handle->cur_offset,
			handle->cur_extent->end - handle->cur_offset,
			POSIX_FADV_NOREUSE))
		perror("posix_fadvise");
	return 0;
}

/**
 *	init_swap_reader - initialize the structure used for loading the image
 *	@handle:	Structure to initialize.
 *	@fd:		File descriptor associated with the swap.
 *	@start:		Swap location (offset) of the first image page.
 *	@buf:		Memory pool to use for buffers.
 *	@image_size:	Total size of the image data.
 *
 *	Initialize buffers and related fields of @handle and load the first
 *	array of extents.
 */
static int init_swap_reader(struct swap_map_handle *handle, int fd,
				loff_t start, void *buf, loff_t image_size)
{
	int error;

	if (!start || !buf)
		return -EINVAL;

	handle->fd = fd;
	handle->total_size = image_size;

	handle->extents = buf;
	buf += page_size;

	handle->buffer = buf;
	handle->read_buffer = buf;
	buf += buffer_size;

#ifdef CONFIG_ENCRYPT
	if (do_decrypt) {
		handle->decrypt_buffer = buf;
		buf += page_size;
	}
#endif

#ifdef CONFIG_COMPRESS
	if (do_decompress) {
		handle->read_buffer = buf;
		buf += decompress_buf_size;
		handle->lzo_work_buffer = buf;
		/* This buffer must hold at least LZO1X_1_MEM_COMPRESS bytes */
	}
#endif

	/* Read the table of extents */
	handle->next_extents = start;
	error = load_extents_page(handle);
	if (error)
		return error;

	if (verify_checksum)
		md5_init_ctx(&handle->ctx);

	return 0;
}

/**
 *	find_next_image_page - find the next swap location holding image data
 */
static void find_next_image_page(struct swap_map_handle *handle)
{
	struct extent ext;
	int error;

	handle->cur_offset += page_size;
	if (handle->cur_offset < handle->cur_extent->end)
		return;
	/* We have exhausted the current extent.  Forward to the next one */
	handle->cur_extent++;
	if (handle->cur_extent->start < handle->cur_extent->end) {
		handle->cur_offset = handle->cur_extent->start;
		if (posix_fadvise(handle->fd, handle->cur_offset,
				handle->cur_extent->end - handle->cur_offset,
				POSIX_FADV_NOREUSE))
			perror("posix_fadvise");
		return;
	}
	/* No more extents.  Load the next extents page. */
	error = load_extents_page(handle);
	if (error)
		handle->cur_offset = 0;
}

/**
 *	load_and_decrypt_page - load a page of data from swap and decrypt it,
 *			if necessary.
 */
static int load_and_decrypt_page(struct swap_map_handle *handle, void *dst)
{
	int error;
	void *buf = dst;

	if (!handle->cur_offset)
		return -EINVAL;

#ifdef CONFIG_ENCRYPT
	if (do_decrypt)
		buf = handle->decrypt_buffer;
#endif

	error = read_page(handle->fd, buf, handle->cur_offset);

#ifdef CONFIG_ENCRYPT
	if (!error && do_decrypt)
		error = gcry_cipher_decrypt(handle->cipher_handle,
					dst, page_size, buf, page_size);
#endif

	if (!error) {
		handle->total_size -= page_size;
		find_next_image_page(handle);
	}
	return error;
}

/**
 *	load_buffer - load (and decrypt, if necessary) a block od data from the
 *			swap, decompress it if necessary and store it in the
 *			image data buffer.
 */
static ssize_t load_buffer(struct swap_map_handle *handle)
{
	void *dst;
	ssize_t size;
	int error;

#ifdef CONFIG_COMPRESS
	if (do_decompress) {
		struct buf_block *block = handle->read_buffer;
		lzo_uint cnt;

		/* Read the block size from the first block page. */
		error = load_and_decrypt_page(handle, block);
		if (error)
			return 0;
		size = page_size;
		dst = block;
		dst += page_size;
		/* Load the rest of the block pages */
		while (size < block->size + sizeof(size_t)) {
			error = load_and_decrypt_page(handle, dst);
			if (error)
				return 0;
			size += page_size;
			dst += page_size;
		}
		/* Decompress block */
		error = lzo1x_decompress((lzo_bytep)block->data, block->size,
						handle->buffer, &cnt,
						handle->lzo_work_buffer);
		if (error)
			return 0;
		size = cnt;
		goto Checksum;
	}
#endif
	dst = handle->buffer;
	size = 0;
	while (size < buffer_size && handle->total_size > 0) {
		error = load_and_decrypt_page(handle, dst);
		if (error)
			return 0;
		size += page_size;
		dst += page_size;
	}

 Checksum:
	if (verify_checksum)
		md5_process_block(handle->buffer, size, &handle->ctx);

	return size;
}

/**
 *	load_image - load a hibernation image
 *	@handle:	Structure containing image information.
 *	@dev:		Special device file to write image data pages to.
 *	@nr_pages:	Number of image data pages.
 */
static int load_image(struct swap_map_handle *handle, int dev,
                      unsigned int nr_pages)
{
	unsigned int m, n;
	ssize_t buf_size;
	ssize_t ret;
	void *buf;
	int error = 0;
	char message[SPLASH_GENERIC_MESSAGE_SIZE];

	sprintf(message, "Loading image data pages (%u pages)...", nr_pages);
	splash.set_caption(message);
	printf("%s     ", message);

	m = nr_pages / 100;
	if (!m)
		m = 1;
	n = 0;
	buf_size = 0;
	do {
		if (buf_size <= 0) {
			buf_size = load_buffer(handle);
			if (buf_size <= 0) {
				printf("\n");
				return -EIO;
			}
			buf = handle->buffer;
		}
		ret = write(dev, buf, page_size);
		if (ret < page_size) {
			if (ret < 0)
				perror("\nError while writing an image page");
			else
				printf("\n");
			return -EIO;
		}
		buf += page_size;
		buf_size -= page_size;

		if (!(n % m)) {
			printf("\b\b\b\b%3d%%", n / m);
			if (n / m > 15)
				splash.progress(n / m);
		}
		n++;
	} while (n < nr_pages);
	if (!error)
		printf(" done\n");
	return error;
}

static char *print_checksum(char * buf, unsigned char *checksum)
{
	int j;

	for (j = 0; j < 16; j++)
		buf += sprintf(buf, "%02hhx ", checksum[j]);

	return buf;
}

#ifdef CONFIG_ENCRYPT
static int decrypt_key(struct image_header_info *header, unsigned char *key,
			unsigned char *ivec, void *buffer)
{
	gcry_ac_handle_t rsa_hd;
	gcry_ac_data_t rsa_data_set, key_set;
	gcry_ac_key_t rsa_priv;
	gcry_mpi_t mpi;
	unsigned char *buf, *out, *key_buf, *ivec_buf;
	char *pass_buf;
	struct md5_ctx ctx;
	struct RSA_data *rsa;
	gcry_cipher_hd_t sym_hd;
	int j, ret;

	rsa = &header->rsa;

	ret = gcry_ac_open(&rsa_hd, GCRY_AC_RSA, 0);
	if (ret)
		return ret;

	ret = gcry_cipher_open(&sym_hd, PK_CIPHER, GCRY_CIPHER_MODE_CFB,
					GCRY_CIPHER_SECURE);
	if (ret)
		goto Free_rsa;

	pass_buf = buffer;
	key_buf = (unsigned char *)pass_buf + PASS_SIZE;
	ivec_buf = key_buf + PK_KEY_SIZE;
	out = ivec_buf + PK_CIPHER_BLOCK;
	do {
		splash.read_password(pass_buf, 0);
		memset(ivec_buf, 0, PK_CIPHER_BLOCK);
		strncpy((char *)ivec_buf, pass_buf, PK_CIPHER_BLOCK);
		md5_init_ctx(&ctx);
		md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
		md5_finish_ctx(&ctx, key_buf);
		ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
		if (!ret)
			ret = gcry_cipher_setiv(sym_hd, ivec_buf,
						PK_CIPHER_BLOCK);

		if (!ret)
			ret = gcry_cipher_encrypt(sym_hd,
					out, KEY_TEST_SIZE,
					KEY_TEST_DATA, KEY_TEST_SIZE);

		if (ret)
			break;

		ret = memcmp(out, rsa->key_test, KEY_TEST_SIZE);

		if (ret)
			printf("%s: Wrong passphrase, try again.\n", my_name);
	} while (ret);

	gcry_cipher_close(sym_hd);
	if (!ret)
		ret = gcry_cipher_open(&sym_hd, PK_CIPHER,
				GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_SECURE);
	if (ret)
		goto Free_rsa;

	ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
	if (!ret)
		ret = gcry_cipher_setiv(sym_hd, ivec_buf, PK_CIPHER_BLOCK);
	if (!ret)
		ret = gcry_ac_data_new(&rsa_data_set);
	if (ret)
		goto Close_cypher;

	buf = rsa->data;
	for (j = 0; j < RSA_FIELDS; j++) {
		size_t s = rsa->size[j];

		/* We need to decrypt some components */
		if (j >= RSA_FIELDS_PUB) {
			/* We use the in-place decryption */
			ret = gcry_cipher_decrypt(sym_hd, buf, s, NULL, 0);
			if (ret)
				break;
		}

		gcry_mpi_scan(&mpi, GCRYMPI_FMT_USG, buf, s, NULL);
		ret = gcry_ac_data_set(rsa_data_set, GCRY_AC_FLAG_COPY,
					rsa->field[j], mpi);
		gcry_mpi_release(mpi);
		if (ret)
			break;

		buf += s;
	}
	if (!ret)
		ret = gcry_ac_key_init(&rsa_priv, rsa_hd,
					GCRY_AC_KEY_SECRET, rsa_data_set);
	if (ret)
		goto Destroy_data_set;

	ret = gcry_ac_data_new(&key_set);
	if (ret)
		goto Destroy_key;

	gcry_mpi_scan(&mpi, GCRYMPI_FMT_USG, header->key.data,
			header->key.size, NULL);
	ret = gcry_ac_data_set(key_set, GCRY_AC_FLAG_COPY, "a", mpi);
	if (ret)
		goto Destroy_key_set;

	gcry_mpi_release(mpi);
	ret = gcry_ac_data_decrypt(rsa_hd, 0, rsa_priv, &mpi, key_set);
	if (!ret) {
		unsigned char *res;
		size_t s;

		gcry_mpi_aprint(GCRYMPI_FMT_USG, &res, &s, mpi);
		if (s == KEY_SIZE + CIPHER_BLOCK) {
			memcpy(key, res, KEY_SIZE);
			memcpy(ivec, res + KEY_SIZE, CIPHER_BLOCK);
		} else {
			ret = -ENODATA;
		}
		gcry_free(res);
	}

Destroy_key_set:
	gcry_mpi_release(mpi);
	gcry_ac_data_destroy(key_set);

Destroy_key:
	gcry_ac_key_destroy(rsa_priv);

Destroy_data_set:
	gcry_ac_data_destroy(rsa_data_set);

Close_cypher:
	gcry_cipher_close(sym_hd);

Free_rsa:
	gcry_ac_close(rsa_hd);

	return ret;
}
#endif

static int open_resume_dev(char *resume_dev_name, 
			    struct swsusp_header *swsusp_header)
{
	ssize_t size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;
	ssize_t ret;
	int fd;

	fd = open(resume_dev_name, O_RDWR);
	if (fd < 0) {
		ret = -errno;
		fprintf(stderr, "%s: Could not open the resume device\n",
				my_name);
		return ret;
	}
	if (lseek(fd, shift, SEEK_SET) != shift)
		return -EIO;
	ret = read(fd, swsusp_header, size);
	if (ret == size) {
		if (memcmp(SWSUSP_SIG, swsusp_header->sig, 10)) {
			close(fd);
			return -ENOMEDIUM;
		}
	} else {
		ret = ret < 0 ? ret : -EIO;
		return ret;
	}
	
	return fd;
}

static void pause_resume(int pause)
{
	struct termios newtrm, savedtrm;
	char message[SPLASH_GENERIC_MESSAGE_SIZE];
	int wait_possible = !splash.prepare_abort(&savedtrm, &newtrm);

	if (!wait_possible)
		pause = -1;

	sprintf(message, "Image successfully loaded\nPress " ENTER_KEY_NAME
			" to continue\n");
	splash.set_caption(message);
	printf("%s: %s", my_name, message);

	if (pause > 0)
		printf("%s: Continuing automatically in %2d seconds",
			my_name, pause);

	while (pause) {
		if (splash.key_pressed() == ENTER_KEY_CODE)
			break;
		sleep(1);
		if (pause > 0)
			printf("\b\b\b\b\b\b\b\b\b\b%2d seconds", --pause);
	}
	printf("\n");

	if (wait_possible)
		splash.restore_abort(&savedtrm);
}

static int read_image(int dev, int fd, struct swsusp_header *swsusp_header)
{
	static struct swap_map_handle handle;
	static unsigned char orig_checksum[16], checksum[16];
	int ret, error = 0;
	struct image_header_info *header = mem_pool;
	char *buffer = (char *)mem_pool + page_size;
	ssize_t size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;
	char c;

	error = read_page(fd, header, swsusp_header->image);
	if (error)
		goto Reboot_question;

	if (header->flags & PLATFORM_SUSPEND)
		use_platform_suspend = 1;

	if (header->flags & IMAGE_CHECKSUM) {
		memcpy(orig_checksum, header->checksum, 16);
		print_checksum(buffer, orig_checksum);
		printf("%s: MD5 checksum %s\n", my_name, buffer);
		verify_checksum = 1;
	}
	splash.progress(10);
	if (header->flags & IMAGE_COMPRESSED) {
		printf("%s: Compressed image\n", my_name);
#ifdef CONFIG_COMPRESS
		if (lzo_init() == LZO_E_OK) {
			do_decompress = 1;
		} else {
			fprintf(stderr, "%s: Failed to initialize LZO\n",
					my_name);
			error = -EFAULT;
		}
#else
		fprintf(stderr, "%s: Compression not supported\n", my_name);
		error = -EINVAL;
#endif
	}
	if (error)
		goto Reboot_question;

	if (header->flags & IMAGE_ENCRYPTED) {
#ifdef CONFIG_ENCRYPT
		static unsigned char key[KEY_SIZE], ivec[CIPHER_BLOCK];

		printf("%s: Encrypted image\n", my_name);
		if (header->flags & IMAGE_USE_RSA) {
			error = decrypt_key(header, key, ivec, buffer);
		} else {
			int j;

			splash.read_password(buffer, 0);
			encrypt_init(key, ivec, buffer);
			for (j = 0; j < CIPHER_BLOCK; j++)
				ivec[j] ^= header->salt[j];
		}
		splash.progress(15);
		if (!error)
			error = gcry_cipher_open(&handle.cipher_handle,
					IMAGE_CIPHER, GCRY_CIPHER_MODE_CFB,
					GCRY_CIPHER_SECURE);
		if (!error) {
			error = gcry_cipher_setkey(handle.cipher_handle,
						key, KEY_SIZE);
			if (!error)
				error = gcry_cipher_setiv(handle.cipher_handle,
							ivec, CIPHER_BLOCK);
			if (error)
				gcry_cipher_close(handle.cipher_handle);
			else
				do_decrypt = 1;
		}
		if (error)
			fprintf(stderr, "%s: libgcrypt error: %s\n", my_name,
					gcry_strerror(error));
#else
		fprintf(stderr, "%s: Encryption not supported\n", my_name);
		error = -EINVAL;
#endif
	}
	if (error)
		goto Reboot_question;

	error = init_swap_reader(&handle, fd, header->map_start, buffer,
					header->image_data_size);
	if (!error) {
		struct timeval begin, end;
		double delta, mb;

		gettimeofday(&begin, NULL);
		error = load_image(&handle, dev, header->pages);
		if (!error && verify_checksum) {
			md5_finish_ctx(&handle.ctx, checksum);
			if (memcmp(orig_checksum, checksum, 16)) {
				fprintf(stderr,
					"%s: MD5 checksum does not match\n",
					my_name);
				print_checksum(buffer, checksum);
				fprintf(stderr,
					"%s: Computed MD5 checksum %s\n",
					my_name, buffer);
				error = -EINVAL;
			}
		}
		if (error)
			goto Reboot_question;
		gettimeofday(&end, NULL);

		timersub(&end, &begin, &end);
		delta = end.tv_usec / 1000000.0 + end.tv_sec;
		mb = (header->pages * (page_size / 1024.0)) / 1024.0;

		printf("wrote %0.1lf MB in %0.1lf seconds (%0.1lf MB/s)\n",
			mb, header->writeout_time, mb / header->writeout_time);

		printf("read %0.1lf MB in %0.1lf seconds (%0.1lf MB/s)\n",
			mb, delta, mb / delta);

		mb *= 2.0;
		delta += header->writeout_time;
		printf("total image i/o %0.1lf MB in %0.1lf seconds "
			"(%0.1lf MB/s)\n", mb, delta, mb / delta);
	}

Reboot_question:
	if (error) {
		c = splash.dialog(
			"\nThe system snapshot image could not be read.\n\n"
#ifdef CONFIG_ENCRYPT
			"\tThis might be a result of booting a wrong kernel\n"
			"\tor typing in a wrong passphrase.\n\n"
#else
			"\tThis might be a result of booting a wrong kernel.\n\n"
#endif
			"\tYou can continue to boot the system and lose the saved state\n"
			"\tor reboot and try again.\n\n"
		        "\t[Notice that you may not mount any filesystem between\n"
		        "\tnow and successful resume. That would badly damage\n"
		        "\taffected filesystems.]\n\n"
			"\tDo you want to continue boot (Y/n)? ");
		ret = (c == 'n' || c == 'N');
		if (ret) {
			close(fd);
			reboot();
			fprintf(stderr, "%s: Reboot failed, please reboot manually.\n",
					my_name);
                       while(1)
			   sleep(1);
		}
	}

	/* Reset swap signature now */
	memcpy(swsusp_header->sig, swsusp_header->orig_sig, 10);
	if (lseek(fd, shift, SEEK_SET) != shift) {
		error = -EIO;
	} else {
		ret = write(fd, swsusp_header, size);
		if (ret != size) {
			error = ret < 0 ? -errno : -EIO;
			fprintf(stderr,
				"%s: Could not restore the partition header\n",
				my_name);
		}
	}

	fsync(fd);
	close(fd);
#ifdef CONFIG_ENCRYPT
	if (do_decrypt)
		gcry_cipher_close(handle.cipher_handle);
#endif

	if (error) {
		sprintf(buffer, "%s: Error %d loading the image\nPress "
			ENTER_KEY_NAME " to continue\n", my_name, error);
		splash.dialog(buffer);
	} else if (header->resume_pause != 0) {
		pause_resume(header->resume_pause);
	} else {
		printf("%s: Image successfully loaded\n", my_name);
	}

	return error;
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
 		       "parameter\0\toverride config file parameter.",
 		       required_argument,	NULL, 'P'
 		   },
		   { NULL,		0,			NULL,  0 }
	};
	int i, error;
	char *conf_name = CONFIG_FILE;
	const char *optstring = "hVf:o:r:P:";
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
		case 'o':
			resume_offset = atoll(optarg);
			break;
		case 'r':
			strncpy(resume_dev_name, optarg, MAX_STR_LEN -1);
			break;
 		case 'P':
 			error = parse_line(optarg, parameters);
 			if (error) {
 				fprintf(stderr,
					"%s: Could not parse config string '%s'\n",
						my_name, optarg);
 				return error;
 			}
 			break;
		default:
			usage(my_name, options, optstring);
			return -EINVAL;
		}
	}

	if (optind < argc)
		strncpy(resume_dev_name, argv[optind], MAX_STR_LEN - 1);

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned int mem_size;
	struct stat stat_buf;
	int dev, resume_dev;
	int n, error, orig_loglevel;
	static struct swsusp_header swsusp_header;

	my_name = basename(argv[0]);

	error = get_config(argc, argv);
	if (error)
		return -error;

	if (splash_param != 'y' && splash_param != 'Y')
		splash_param = 0;
	else
		splash_param = SPL_RESUME;

	page_size = getpagesize();
	buffer_size = BUFFER_PAGES * page_size;
	mem_size = 2 * page_size + buffer_size;
#ifdef CONFIG_ENCRYPT
	printf("%s: libgcrypt version: %s\n", my_name,
		gcry_check_version(NULL));
	gcry_control(GCRYCTL_INIT_SECMEM, page_size, 0);
	mem_size += page_size;
#endif
#ifdef CONFIG_COMPRESS
	/*
	 * The formula below follows from the worst-case expansion calculation
	 * for LZO1 (size / 16 + 67) and the fact that the size of the
	 * compressed data must be stored in the buffer (sizeof(size_t)).
	 * Additionally, we want the buffer size to be a multiple of page_size.
	 */
	decompress_buf_size = buffer_size +
		((page_size + (buffer_size >> 4) + 66 + sizeof(size_t))
			& ~(page_size - 1));
	mem_size += decompress_buf_size + LZO1X_1_MEM_COMPRESS;
#endif
	mem_pool = malloc(mem_size);
	if (!mem_pool) {
		error = errno;
		fprintf(stderr, "%s: Could not allocate memory\n", my_name);
		return error;
	}

	open_printk();
	orig_loglevel = get_kernel_console_loglevel();
	set_kernel_console_loglevel(suspend_loglevel);

	while (stat(resume_dev_name, &stat_buf)) {
		fprintf(stderr, 
			"%s: Could not stat the resume device file '%s'\n"
			"\tPlease type in the full path name to try again\n"
			"\tor press ENTER to boot the system: ", my_name,
			resume_dev_name);
		fgets(resume_dev_name, MAX_STR_LEN - 1, stdin);
		n = strlen(resume_dev_name) - 1;
		if (n <= 0) {
			error = EINVAL;
			goto Free;
		}

		if (resume_dev_name[n] == '\n')
			resume_dev_name[n] = '\0';
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		error = errno;
		fprintf(stderr, "%s: Could not lock myself\n", my_name);
		goto Free;
	}


	dev = open(snapshot_dev_name, O_WRONLY);
	if (dev < 0) {
		error = ENOENT;
		goto Free;
	}

	resume_dev = open_resume_dev(resume_dev_name, &swsusp_header);
	if (resume_dev == -ENOMEDIUM) {
		error = 0;
		goto Close;
	} else if (resume_dev < 0) {
		error = -resume_dev;
		goto Close;
	}

	splash_prepare(&splash, splash_param);
	splash.progress(5);

	error = read_image(dev, resume_dev, &swsusp_header);
	if (error) {
		fprintf(stderr, "%s: Could not read the image\n", my_name);
		error = -error;
		goto Close_splash;
	}

	if (freeze(dev)) {
		error = errno;
		fprintf(stderr, "%s: Could not freeze processes\n", my_name);
		goto Close_splash;
	}
	if (use_platform_suspend) {
		int err = platform_prepare(dev);

		if (err) {
			fprintf(stderr, "%s: Unable to use platform "
					"hibernation support, error code %d\n",
					my_name, err);
			use_platform_suspend = 0;
		}
	}
	atomic_restore(dev);
	/* We only get here if the atomic restore fails.  Clean up. */
	if (use_platform_suspend)
		platform_finish(dev);

	unfreeze(dev);

Close_splash:
	splash.finish();
Close:
	close(dev);
Free:
	if (error)
	    set_kernel_console_loglevel(max_loglevel);
	else if (orig_loglevel >= 0)
	    set_kernel_console_loglevel(orig_loglevel);

	close_printk();

	free(mem_pool);

	return error;
}
