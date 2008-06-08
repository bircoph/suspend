/*
 * load.c
 *
 * Image loading for s2disk/s2both and resume.
 *
 * Copyright (C) 2008 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
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
#include "memalloc.h"
#include "md5.h"
#include "splash.h"

char *my_name;

static char verify_checksum;
#ifdef CONFIG_COMPRESS
unsigned int compress_buf_size;
static char do_decompress;
#else
#define do_decompress 0
#endif
#ifdef CONFIG_ENCRYPT
static char do_decrypt;
static char password[PASSBUF_SIZE];
#else
#define do_decrypt 0
#endif

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
 * The swap_reader structure is used for handling swap in a file-alike way.
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
 */
struct swap_reader {
	struct extent *extents;
	struct extent *cur_extent;
	loff_t cur_offset;
	loff_t next_extents;
	loff_t total_size;
	void *buffer;
	void *read_buffer;
	int fd;
	struct md5_ctx ctx;
	void *lzo_work_buffer;
	char *decrypt_buffer;
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
static int load_extents_page(struct swap_reader *handle)
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
 *	free_swap_reader - free memory allocated for loading the image
 *	@handle:	Structure containing pointers to memory buffers to free.
 */
static void free_swap_reader(struct swap_reader *handle)
{
	if (do_decompress) {
		freemem(handle->lzo_work_buffer);
		freemem(handle->read_buffer);
	}
	if (do_decrypt)
		 freemem(handle->decrypt_buffer);
	freemem(handle->buffer);
	freemem(handle->extents);
}

/**
 *	init_swap_reader - initialize the structure used for loading the image
 *	@handle:	Structure to initialize.
 *	@fd:		File descriptor associated with the swap.
 *	@start:		Swap location (offset) of the first image page.
 *	@image_size:	Total size of the image data.
 *
 *	Initialize buffers and related fields of @handle and load the first
 *	array of extents.
 */
static int init_swap_reader(struct swap_reader *handle, int fd, loff_t start,
                            loff_t image_size)
{
	int error;

	if (!start)
		return -EINVAL;

	handle->fd = fd;
	handle->total_size = image_size;

	handle->extents = getmem(page_size);

	handle->buffer = getmem(buffer_size);
	handle->read_buffer = handle->buffer;

	if (do_decrypt)
		handle->decrypt_buffer = getmem(page_size);

	if (do_decompress) {
		handle->read_buffer = getmem(compress_buf_size);
		handle->lzo_work_buffer = getmem(LZO1X_1_MEM_COMPRESS);
	}

	/* Read the table of extents */
	handle->next_extents = start;
	error = load_extents_page(handle);
	if (error) {
		free_swap_reader(handle);
		return error;
	}

	if (verify_checksum)
		md5_init_ctx(&handle->ctx);

	return 0;
}

/**
 *	find_next_image_page - find the next swap location holding image data
 */
static void find_next_image_page(struct swap_reader *handle)
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
static int load_and_decrypt_page(struct swap_reader *handle, void *dst)
{
	int error;
	void *buf = dst;

	if (!handle->cur_offset)
		return -EINVAL;

	if (do_decrypt)
		buf = handle->decrypt_buffer;

	error = read_page(handle->fd, buf, handle->cur_offset);

#ifdef CONFIG_ENCRYPT
	if (!error && do_decrypt)
		error = gcry_cipher_decrypt(cipher_handle, dst, page_size,
							buf, page_size);
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
static ssize_t load_buffer(struct swap_reader *handle)
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
static int load_image(struct swap_reader *handle, int dev,
                      unsigned int nr_pages, int verify_only)
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
		ret = verify_only ? page_size : write(dev, buf, page_size);
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
			unsigned char *ivec)
{
	gcry_ac_handle_t rsa_hd;
	gcry_ac_data_t rsa_data_set, key_set;
	gcry_ac_key_t rsa_priv;
	gcry_mpi_t mpi;
	unsigned char *buf, *out, *key_buf, *ivec_buf;
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

	key_buf = getmem(PK_KEY_SIZE);
	ivec_buf = getmem(PK_CIPHER_BLOCK);
	out = getmem(KEY_TEST_SIZE);
	do {
		splash.read_password(password, 0);
		memset(ivec_buf, 0, PK_CIPHER_BLOCK);
		strncpy(ivec_buf, password, PK_CIPHER_BLOCK);
		md5_init_ctx(&ctx);
		md5_process_bytes(password, strlen(password), &ctx);
		md5_finish_ctx(&ctx, key_buf);
		ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
		if (!ret)
			ret = gcry_cipher_setiv(sym_hd, ivec_buf,
						PK_CIPHER_BLOCK);

		if (!ret)
			ret = gcry_cipher_encrypt(sym_hd,
						out, KEY_TEST_SIZE,
						KEY_TEST_DATA, KEY_TEST_SIZE);

		if (ret) {
			fprintf(stderr, "%s: libgcrypt error: %s\n", my_name,
					gcry_strerror(ret));
			break;
		}

		ret = memcmp(out, rsa->key_test, KEY_TEST_SIZE);

		if (ret)
			printf("%s: Wrong passphrase, try again.\n", my_name);
	} while (ret);

	gcry_cipher_close(sym_hd);
	if (!ret)
		ret = gcry_cipher_open(&sym_hd, PK_CIPHER,
				GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_SECURE);
	if (ret)
		goto Free_buffers;

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

Free_buffers:
	freemem(out);
	freemem(ivec_buf);
	freemem(key_buf);

Free_rsa:
	gcry_ac_close(rsa_hd);

	return ret;
}

static int restore_key(struct image_header_info *header)
{
	static unsigned char key[KEY_SIZE], ivec[CIPHER_BLOCK];
	int error;

	if (header->flags & IMAGE_USE_RSA) {
		error = decrypt_key(header, key, ivec);
	} else {
		int j;

		splash.read_password(password, 0);
		encrypt_init(key, ivec, password);
		for (j = 0; j < CIPHER_BLOCK; j++)
			ivec[j] ^= header->salt[j];
	}
	if (!error)
		error = gcry_cipher_open(&cipher_handle,
					IMAGE_CIPHER, GCRY_CIPHER_MODE_CFB,
					GCRY_CIPHER_SECURE);
	if (!error) {
		error = gcry_cipher_setkey(cipher_handle, key, KEY_SIZE);
		if (!error)
			error = gcry_cipher_setiv(cipher_handle, ivec,
							CIPHER_BLOCK);
		if (error)
			gcry_cipher_close(cipher_handle);
	}
	return error;
}
#endif

int read_or_verify_image(int dev, int fd, struct image_header_info *header,
                         loff_t start, int verify)
{
	static struct swap_reader handle;
	static unsigned char orig_checksum[16], checksum[16], csum_buf[48];
	int error = 0;

	error = read_page(fd, header, start);
	if (error)
		return error;

	if ((header->flags & IMAGE_CHECKSUM) || verify) {
		memcpy(orig_checksum, header->checksum, 16);
		print_checksum(csum_buf, orig_checksum);
		printf("%s: MD5 checksum %s\n", my_name, csum_buf);
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
		return error;

	if (header->flags & IMAGE_ENCRYPTED) {
#ifdef CONFIG_ENCRYPT
		printf("%s: Encrypted image\n", my_name);
		error = verify ?
			gcry_cipher_setiv(cipher_handle, key_data.ivec,
						CIPHER_BLOCK) :
			restore_key(header);
		if (error) {
			fprintf(stderr, "%s: libgcrypt error: %s\n", my_name,
					gcry_strerror(error));
		} else {
			do_decrypt = 1;
			splash.progress(15);
		}
#else
		fprintf(stderr, "%s: Encryption not supported\n", my_name);
		error = -EINVAL;
#endif
	}
	if (error)
		goto Exit_encrypt;

	error = init_swap_reader(&handle, fd, header->map_start,
					header->image_data_size);
	if (!error) {
		struct timeval begin, end;
		double delta, mb;

		gettimeofday(&begin, NULL);
		error = load_image(&handle, dev, header->pages, verify);
		if (!error && verify_checksum) {
			md5_finish_ctx(&handle.ctx, checksum);
			if (memcmp(orig_checksum, checksum, 16)) {
				fprintf(stderr,
					"%s: MD5 checksum does not match\n",
					my_name);
				print_checksum(csum_buf, checksum);
				fprintf(stderr,
					"%s: Computed MD5 checksum %s\n",
					my_name, csum_buf);
				error = -EINVAL;
			}
		}
		free_swap_reader(&handle);
		if (error)
			goto Exit_encrypt;
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
		printf("total image i/o: %0.1lf MB in %0.1lf seconds "
			"(%0.1lf MB/s)\n", mb, delta, mb / delta);

		if (do_decompress) {
			double real_size = header->image_data_size;

			printf("%s: Compression ratio %4.2lf\n", my_name,
				real_size / (header->pages * page_size));
			real_size /= (1024.0 * 1024.0);
			delta -= header->writeout_time;

			printf("wrote %0.1lf MB of compressed data in %0.1lf "
				"seconds (%0.1lf MB/s)\n", real_size,
				header->writeout_time,
				real_size / header->writeout_time);

			printf("read %0.1lf MB of compressed data in %0.1lf "
				"seconds (%0.1lf MB/s)\n", real_size,
				delta, real_size / delta);

			real_size *= 2.0;
			delta += header->writeout_time;
			printf("total compressed data i/o: %0.1lf MB in %0.1lf "
				"seconds (%0.1lf MB/s)\n", real_size, delta,
				real_size / delta);
		}
	}

 Exit_encrypt:
#ifdef CONFIG_ENCRYPT
	if (do_decrypt && !verify)
		gcry_cipher_close(cipher_handle);
#endif

	return error;
}
