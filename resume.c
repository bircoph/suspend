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
#ifdef CONFIG_ENCRYPT
static char decrypt_buffer[BUFFER_SIZE];
#endif

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
#ifdef CONFIG_ENCRYPT
	BF_KEY key;
	unsigned char ivec[IVEC_SIZE];
	int num;
#endif
};

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static int fill_buffer(struct swap_map_handle *handle)
{
	void *dst = read_buffer;
	int error;

	handle->area_size = handle->cur.entries[handle->k].size;
#ifdef CONFIG_ENCRYPT
	if (decrypt)
		dst = decrypt_buffer;
#endif
	error = read_area(handle->fd, dst,
			handle->cur.entries[handle->k].offset,
			handle->area_size);
#ifdef CONFIG_ENCRYPT
	if (!error && decrypt)
		BF_cfb64_encrypt(dst, (unsigned char *)read_buffer,
			handle->area_size, &handle->key, handle->ivec,
			&handle->num, BF_DECRYPT);
#endif
	return error;
}

static int init_swap_reader(struct swap_map_handle *handle, int fd, loff_t start)
{
	int error;

	if (!start)
		return -EINVAL;
	error = read_area(fd, &handle->cur, start, sizeof(struct swap_map_page));
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

static int restore(void *buf, int disp)
{
	struct buf_block *block = (struct buf_block *)(read_buffer + disp);

	if (decompress) {
		unsigned short cnt = block->size;

		if (cnt == PAGE_SIZE) {
			memcpy(buf, block->data, PAGE_SIZE);
		} else if (cnt < PAGE_SIZE) {
			cnt = lzf_decompress(block->data, cnt, buf, PAGE_SIZE);
			if (cnt != PAGE_SIZE)
				return -ENODATA;
		} else {
			return -EINVAL;
		}
		block->size += sizeof(short);
		return block->size;
	}
	memcpy(buf, block, PAGE_SIZE);
	return PAGE_SIZE;
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
	if (!error)
		error = fill_buffer(handle);
	if (error)
		return error;

	size = restore(buf, 0);
	if (size < 0)
		return size;
	handle->cur_size = size;

MD5:
	if (verify_checksum)
		md5_process_block(buf, PAGE_SIZE, &handle->ctx);

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
		printf("%02hhx ", checksum[j]);
}

#ifdef CONFIG_ENCRYPT
static int decrypt_key(struct swsusp_info *header, BF_KEY *key, unsigned char *ivec)
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

	pass_buf = read_buffer;
	key_buf = (unsigned char *)read_buffer + PASS_SIZE;
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

	out = (unsigned char *)decrypt_buffer;
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
				error = decrypt_key(header, &handle.key, handle.ivec);
			} else {
				int j;

				printf("resume: Encrypted image\n");
				encrypt_init(&handle.key, handle.ivec, &handle.num,
						read_buffer, decrypt_buffer, 0);
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
			error = init_swap_reader(&handle, fd, header->map_start);
			nr_pages = header->pages - 1;
			ret = write(dev, header, PAGE_SIZE);
			if (ret < (int)PAGE_SIZE)
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
