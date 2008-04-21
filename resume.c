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
#else
#define do_decompress 0
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
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};

static unsigned int page_size;
static unsigned int buffer_size;
static void *mem_pool;

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

	if (!offset)
		return 0;

	if (lseek(fd, offset, SEEK_SET) == offset)
		cnt = read(fd, buf, size);
	if (cnt < (ssize_t)size)
		res = cnt < 0 ? cnt : -EIO;

	return res;
}

/*
 *	The swap_map_handle structure is used for handling the swap map in
 *	a file-alike way
 */

struct swap_map_handle {
	char *page_buffer;
	char *read_buffer;
	struct swap_area *areas;
	unsigned short areas_per_page;
	loff_t *next_swap;
	unsigned int area_size;
	unsigned int cur_size;
	unsigned int k;
	int fd;
	struct md5_ctx ctx;
#ifdef CONFIG_ENCRYPT
	char *decrypt_buffer;
	gcry_cipher_hd_t cipher_handle;
#endif
};

#define READ_AHEAD_SIZE (8*1024*1024)

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static int fill_buffer(struct swap_map_handle *handle)
{
	void *dst = handle->read_buffer;
	int error;

	int read_ahead_areas = READ_AHEAD_SIZE/buffer_size;
	int advise_first, advise_last, i;

	if (!handle->k) {
		/* we've got a new map page */
		advise_first = 0;
		advise_last  = read_ahead_areas;
	} else {
		advise_first = handle->k + read_ahead_areas;
		advise_last  = advise_first;
	}

	for (i = advise_first;
	     i <= advise_last && i < handle->areas_per_page;
	     i++) {
		if (handle->areas[i].offset == 0)
			break;

		error = posix_fadvise(handle->fd, handle->areas[i].offset,
				handle->areas[i].size, POSIX_FADV_NOREUSE);
		if (error) {
			perror("posix_fadvise");
			break;
		}
	}

	handle->area_size = handle->areas[handle->k].size;
	if (handle->area_size > buffer_size)
		return -ENOMEM;
#ifdef CONFIG_ENCRYPT
	if (do_decrypt)
		dst = handle->decrypt_buffer;
#endif
	error = read_area(handle->fd, dst,
			handle->areas[handle->k].offset,
			handle->area_size);
#ifdef CONFIG_ENCRYPT
	if (!error && do_decrypt)
		error = gcry_cipher_decrypt(handle->cipher_handle,
				(void *)handle->read_buffer, handle->area_size,
				dst, handle->area_size);
#endif
	return error;
}

static int init_swap_reader(struct swap_map_handle *handle, int fd, loff_t start, void *buf)
{
	int error;

	if (!start || !buf)
		return -EINVAL;
	handle->areas = buf;
	handle->areas_per_page = (page_size - sizeof(loff_t)) /
			sizeof(struct swap_area);
	handle->next_swap = (loff_t *)(handle->areas + handle->areas_per_page);
	handle->page_buffer = (char *)buf + page_size;
	handle->read_buffer = handle->page_buffer + page_size;
#ifdef CONFIG_ENCRYPT
	handle->decrypt_buffer = handle->read_buffer + buffer_size;
#endif
	error = read_area(fd, handle->areas, start, page_size);
	if (error)
		return error;
	handle->fd = fd;
	handle->k = 0;
	error = fill_buffer(handle);
	if (error)
		return error;
	handle->cur_size = 0;
	if (verify_checksum)
		md5_init_ctx(&handle->ctx);
	return 0;
}

static ssize_t restore(struct swap_map_handle *handle, int disp)
{
	struct buf_block *block;
	void *buf = handle->page_buffer;

	block = (struct buf_block *)(handle->read_buffer + disp);
#ifdef CONFIG_COMPRESS
	if (do_decompress) {
		int error;
		lzo_uint cnt = page_size;

		error = lzo1x_decompress_safe((lzo_bytep)block->data,
						block->size,
						buf, &cnt, NULL);
		if (error)
			return -EINVAL;
		return cnt == page_size ?
				block->size + sizeof(short) : -ENODATA;
	}
#endif
	memcpy(buf, block, page_size);
	return page_size;
}

static int swap_read_page(struct swap_map_handle *handle)
{
	loff_t offset;
	ssize_t size;
	int error = 0;

	if (handle->cur_size < handle->area_size) {
		/* Get the data from the read buffer */
		size = restore(handle, handle->cur_size);
		if (size < 0)
			return size;
		handle->cur_size += size;
		goto MD5;
	}

	/* There are no more data in the read buffer.  Read more */
	if (++handle->k >= handle->areas_per_page) {
		handle->k = 0;
		offset = *handle->next_swap;
		error = offset ?
			read_area(handle->fd, handle->areas, offset, page_size)
			: -EINVAL;
	}
	if (!error)
		error = fill_buffer(handle);
	if (error)
		return error;

	size = restore(handle, 0);
	if (size < 0)
		return size;
	handle->cur_size = size;

MD5:
	if (verify_checksum)
		md5_process_block(handle->page_buffer, page_size, &handle->ctx);

	return 0;
}

/**
 *	load_image - load the image using the swap map handle
 *	@handle and the snapshot handle @snapshot
 *	(assume there are @nr_pages pages to load)
 */

static int load_image(struct swap_map_handle *handle, int dev,
                      unsigned int nr_pages)
{
	unsigned int m, n;
	int ret;
	int error = 0;
	char message[SPLASH_GENERIC_MESSAGE_SIZE];

	sprintf(message, "Loading image data pages (%u pages)...", nr_pages);
	splash.set_caption(message);
	printf("%s     ", message);

	m = nr_pages / 100;
	if (!m)
		m = 1;
	n = 0;
	do {
		error = swap_read_page(handle);
		if (error)
			break;
		ret = write(dev, handle->page_buffer, page_size);
		if (ret < (int)page_size) {
			error = ret < 0 ? -errno : -ENOSPC;
			break;
		}
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
static int decrypt_key(struct swsusp_info *header, unsigned char *key,
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
	int j, ret = 0;

	rsa = &header->rsa;

	ret = gcry_ac_open(&rsa_hd, GCRY_AC_RSA, 0);
	if (ret)
		return ret;

	if (!ret)
		ret = gcry_cipher_open(&sym_hd, PK_CIPHER,
				GCRY_CIPHER_MODE_CFB, GCRY_CIPHER_SECURE);

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

static int read_image(int dev, int fd, struct swsusp_header *swsusp_header)
{
	static struct swap_map_handle handle;
	static unsigned char orig_checksum[16], checksum[16];
	int ret, error = 0;
	struct swsusp_info *header = mem_pool;
	char *buffer = (char *)mem_pool + page_size;
	unsigned int nr_pages = 0;
	ssize_t size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;
	char c;

	error = read_area(fd, header, swsusp_header->image, page_size);
	if (error)
		goto Reboot_question;

	if (header->image_flags & PLATFORM_SUSPEND)
		use_platform_suspend = 1;

	if (header->image_flags & IMAGE_CHECKSUM) {
		memcpy(orig_checksum, header->checksum, 16);
		print_checksum(buffer, orig_checksum);
		printf("%s: MD5 checksum %s\n", my_name, buffer);
		verify_checksum = 1;
	}
	splash.progress(10);
	if (header->image_flags & IMAGE_COMPRESSED) {
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

	if (header->image_flags & IMAGE_ENCRYPTED) {
#ifdef CONFIG_ENCRYPT
		static unsigned char key[KEY_SIZE], ivec[CIPHER_BLOCK];

		printf("%s: Encrypted image\n", my_name);
		if (header->image_flags & IMAGE_USE_RSA) {
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
			do_decrypt = 1;
			error = gcry_cipher_setkey(handle.cipher_handle,
						key, KEY_SIZE);
		}
		if (!error)
			error = gcry_cipher_setiv(handle.cipher_handle,
						ivec, CIPHER_BLOCK);

		if (error) {
			if (do_decrypt)
				gcry_cipher_close(handle.cipher_handle);
			do_decrypt = 0;
			fprintf(stderr, "%s: libgcrypt error: %s\n",
					my_name,
					gcry_strerror(error));
		}
#else
		fprintf(stderr, "%s: Encryption not supported\n",
				my_name);
		error = -EINVAL;
#endif
	}

	if (error)
		goto Reboot_question;

	error = init_swap_reader(&handle, fd, header->map_start, buffer);
	nr_pages = header->pages - 1;
	ret = write(dev, header, page_size);
	if (ret < (int)page_size)
		error = ret < 0 ? ret : -EIO;

	if (!error) {
		struct timeval begin, end;
		double delta, mb = header->size / (1024.0 * 1024.0);

		gettimeofday(&begin, NULL);
		error = load_image(&handle, dev, nr_pages);
		gettimeofday(&end, NULL);

		timersub(&end, &begin, &end);
		delta = end.tv_usec / 1000000.0 + end.tv_sec;

		printf("wrote %0.1lf MB in %0.1lf seconds (%0.1lf MB/s)\n",
			mb, header->writeout_time, mb / header->writeout_time);

		printf("read %0.1lf MB in %0.1lf seconds (%0.1lf MB/s)\n",
			mb, delta, mb / delta);

		mb *= 2.0;
		delta += header->writeout_time;
		printf("total image i/o %0.1lf MB in %0.1lf seconds (%0.1lf MB/s)\n",
			mb, delta, mb / delta);
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

	/* I guess this is after the 'continue boot'-question because
	 * there is no sense in not reseting the signature on error */
	if (!error && verify_checksum) {
		md5_finish_ctx(&handle.ctx, checksum);
		if (memcmp(orig_checksum, checksum, 16)) {
			fprintf(stderr,"%s: MD5 checksum does not match\n",
					my_name);
			print_checksum(buffer, checksum);
			fprintf(stderr, "%s: Computed MD5 checksum %s\n",
					my_name, buffer);
			error = -EINVAL;
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
		sprintf(buffer, "%s: Error %d loading the image\n"
			"\nPress ENTER to continue\n", my_name, error);
		splash.dialog(buffer);
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
#ifdef CONFIG_ENCRYPT
	printf("%s: libgcrypt version: %s\n", my_name,
		gcry_check_version(NULL));
	gcry_control(GCRYCTL_INIT_SECMEM, page_size, 0);
	mem_size = 3 * page_size + 2 * buffer_size;
#else
	mem_size = 3 * page_size + buffer_size;
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
		int ret = platform_prepare(dev);
		if (ret < 0) {
			fprintf(stderr, "%s: pm_ops->prepare returned "
					"error %d\n", my_name, ret);
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
