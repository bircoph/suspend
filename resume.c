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

#define _GNU_SOURCE
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
#ifdef CONFIG_COMPRESS
#include <lzf.h>
#else
#define lzf_decompress(a, b, c, d)	0
#endif

#include "swsusp.h"
#include "config.h"
#include "md5.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static int max_loglevel = MAX_LOGLEVEL;
static char verify_checksum;
#ifdef CONFIG_COMPRESS
static char decompress;
#else
#define decompress 0
#endif
#ifdef CONFIG_ENCRYPT
static char decrypt;
#else
#define decrypt 0
#endif

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
	BF_KEY key;
	unsigned char ivec[IVEC_SIZE];
	int num;
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
	if (decrypt)
		dst = handle->decrypt_buffer;
#endif
	error = read_area(handle->fd, dst,
			handle->areas[handle->k].offset,
			handle->area_size);
#ifdef CONFIG_ENCRYPT
	if (!error && decrypt)
		BF_cfb64_encrypt(dst, (unsigned char *)handle->read_buffer,
			handle->area_size, &handle->key, handle->ivec,
			&handle->num, BF_DECRYPT);
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

static int restore(struct swap_map_handle *handle, int disp)
{
	struct buf_block *block;
	void *buf = handle->page_buffer;

	block = (struct buf_block *)(handle->read_buffer + disp);
	if (decompress) {
		unsigned short cnt = block->size;

		if (cnt == page_size) {
			memcpy(buf, block->data, page_size);
		} else if (cnt < page_size) {
			cnt = lzf_decompress(block->data, cnt, buf, page_size);
			if (cnt != page_size)
				return -ENODATA;
		} else {
			return -EINVAL;
		}
		block->size += sizeof(short);
		return block->size;
	}
	memcpy(buf, block, page_size);
	return page_size;
}

static inline int swap_read_page(struct swap_map_handle *handle)
{
	loff_t offset;
	size_t size;
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
		if (offset)
			error = read_area(handle->fd, handle->areas, offset, page_size);
		else
			error = -EINVAL;
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
		error = swap_read_page(handle);
		if (error)
			break;
		ret = write(dev, handle->page_buffer, page_size);
		if (ret < (int)page_size) {
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
		printf("%02hhx ", checksum[j]);
}

#ifdef CONFIG_ENCRYPT
static int decrypt_key(struct swsusp_info *header, BF_KEY *key, unsigned char *ivec,
	void *buffer)
{
	RSA *rsa;
	unsigned char *buf, *out, *key_buf;
	char *pass_buf;
	struct md5_ctx ctx;
	struct RSA_data *rsa_data;
	int n = 0, ret = 0;

	rsa = RSA_new();
	if (!rsa)
		return -ENOMEM;

	rsa_data = &header->rsa_data;
	buf = rsa_data->data;
	rsa->n = BN_mpi2bn(buf, rsa_data->n_size, NULL);
	buf += rsa_data->n_size;
	rsa->e = BN_mpi2bn(buf, rsa_data->e_size, NULL);
	buf += rsa_data->e_size;

	pass_buf = buffer;
	key_buf = (unsigned char *)pass_buf + PASS_SIZE;
	do {
		read_password(pass_buf, 0);
		memset(ivec, 0, IVEC_SIZE);
		strncpy((char *)ivec, pass_buf, IVEC_SIZE);
		md5_init_ctx(&ctx);
		md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
		md5_finish_ctx(&ctx, key_buf);
		BF_set_key(key, KEY_SIZE, key_buf);
		BF_ecb_encrypt(KEY_TEST_DATA, key_buf, key, BF_ENCRYPT);
		ret = memcmp(key_buf, rsa_data->key_test, KEY_TEST_SIZE);
		if (ret)
			printf("resume: Wrong passphrase, try again.\n");
	} while (ret);

	out = key_buf + KEY_SIZE;
	BF_cfb64_encrypt(buf, out, rsa_data->d_size, key, ivec, &n, BF_DECRYPT);
	rsa->d = BN_mpi2bn(out, rsa_data->d_size, NULL);
	if (!rsa->n || !rsa->e || !rsa->d) {
		ret = -EFAULT;
		goto Free_rsa;
	}

	ret = RSA_private_decrypt(header->key_data.size, header->key_data.data,
			out, rsa, RSA_PKCS1_PADDING);
	if (ret != (int)(KEY_SIZE + IVEC_SIZE)) {
		ret = -ENODATA;
	} else {
		BF_set_key(key, KEY_SIZE, out);
		memcpy(ivec, out + KEY_SIZE, IVEC_SIZE);
		ret = 0;
	}

Free_rsa:
	RSA_free(rsa);
	return ret;
}
#endif

static int read_image(int dev, char *resume_dev_name)
{
	static struct swsusp_header swsusp_header;
	static struct swap_map_handle handle;
	static unsigned char orig_checksum[16], checksum[16];
	int fd, ret, error = 0;
	struct swsusp_info *header = mem_pool;
	char *buffer = (char *)mem_pool + page_size;
	unsigned int nr_pages;
	unsigned int size = sizeof(struct swsusp_header);
	unsigned int shift = page_size - size;
	char c;

	fd = open(resume_dev_name, O_RDWR);
	if (fd < 0) {
		ret = -errno;
		printf("resume: Could not open the resume device\n");
		return ret;
	}
	if (lseek(fd, shift, SEEK_SET) != shift)
		return -EIO;
	ret = read(fd, &swsusp_header, size);
	if (ret == size) {
		if (memcmp(SWSUSP_SIG, swsusp_header.sig, 10))
			return -EINVAL;
	} else {
		error = ret < 0 ? ret : -EIO;
	}
	if (!error)
		error = read_area(fd, header, swsusp_header.image, page_size);
	if (!error) {
		if(header->image_flags & IMAGE_CHECKSUM) {
			memcpy(orig_checksum, header->checksum, 16);
			printf("resume: MD5 checksum ");
			print_checksum(orig_checksum);
			printf("\n");
			verify_checksum = 1;
		}
		if (header->image_flags & IMAGE_COMPRESSED) {
			printf("resume: Compressed image\n");
#ifdef CONFIG_COMPRESS
			decompress = 1;
#else
			printf("resume: Compression not supported\n");
			error = -EINVAL;
#endif
		}
		if (!error && (header->image_flags & IMAGE_ENCRYPTED)) {
#ifdef CONFIG_ENCRYPT
			if (header->image_flags & IMAGE_USE_RSA) {
				handle.num = 0;
				error = decrypt_key(header, &handle.key,
						handle.ivec, buffer);
			} else {
				int j;

				printf("resume: Encrypted image\n");
				encrypt_init(&handle.key, handle.ivec, &handle.num,
						buffer, buffer + page_size, 0);
				for (j = 0; j < IVEC_SIZE; j++)
					handle.ivec[j] ^= header->salt[j];
			}
			if (!error)
				decrypt = 1;
#else
			printf("resume: Encryption not supported\n");
			error = -EINVAL;
#endif
		}
		if (!error) {
			error = init_swap_reader(&handle, fd,
					header->map_start, buffer);
			nr_pages = header->pages - 1;
			ret = write(dev, header, page_size);
			if (ret < (int)page_size)
				error = ret < 0 ? ret : -EIO;
		}
		if (!error)
			error = load_image(&handle, dev, nr_pages);
	}
	if (error) {
		printf("\nresume: The system snapshot image could not be read.\n\n"
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
				printf("\nPress ENTER to continue ");
				getchar();
				error = -EINVAL;
			}
		}
		/* Reset swap signature now */
		memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
		if (lseek(fd, shift, SEEK_SET) != shift) {
			error = -EIO;
		} else {
			ret = write(fd, &swsusp_header, size);
			if (ret != size) {
				error = ret < 0 ? -errno : -EIO;
				fprintf(stderr,
					"resume: Could not restore the partition header\n");
			}
		}
	}
	fsync(fd);
	close(fd);
	if (!error) {
		printf("resume: Image successfully loaded\n");
	} else {
		printf("resume: Error %d loading the image\n"
			"\nPress ENTER to continue", error);
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
	unsigned int mem_size;
	struct stat stat_buf;
	int dev;
	int n, error = 0;

	page_size = getpagesize();
	buffer_size = BUFFER_PAGES * page_size;

#ifdef CONFIG_ENCRYPT
	mem_size = 3 * page_size + 2 * buffer_size;
#else
	mem_size = 3 * page_size + buffer_size;
#endif
	mem_pool = malloc(mem_size);
	if (!mem_pool) {
		error = errno;
		fprintf(stderr, "resume: Could not allocate memory\n");
		return error;
	}

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
		error = errno;
		fprintf(stderr, "resume: Could not freeze processes\n");
		goto Close;
	}
	atomic_restore(dev);
	unfreeze(dev);
Close:
	close(dev);

	set_kernel_console_loglevel(max_loglevel);

	free(mem_pool);

	return error;
}
