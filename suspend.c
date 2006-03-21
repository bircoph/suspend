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
#include <sys/mount.h>
#include <sys/vt.h>
#include <sys/wait.h>
#include <linux/kd.h>
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
#define lzf_compress(a, b, c, d)	0
#endif

#include "swsusp.h"
#include "config.h"
#include "md5.h"
#include "s2ram.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static unsigned long pref_image_size = IMAGE_SIZE;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static char compute_checksum;
#ifdef CONFIG_COMPRESS
static char compress;
#else
#define compress 0
#endif
static unsigned long compr_diff;
#ifdef CONFIG_ENCRYPT
static char encrypt;
static char use_RSA;
static char key_name[MAX_STR_LEN] = KEY_FILE;
#else
#define encrypt 0
#define key_name NULL
#endif
static char s2ram;

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
		.ptr = &compress,
	},
#endif
#ifdef CONFIG_ENCRYPT
	{
		.name = "encrypt",
		.fmt = "%c",
		.ptr = &encrypt,
	},
	{
		.name = "RSA key file",
		.fmt = "%s",
		.ptr = key_name,
		.len = MAX_STR_LEN
	},
#endif
	{
		.name = "suspend to both",
		.fmt = "%c",
		.ptr = &s2ram,
	},
};

static char page_buffer[PAGE_SIZE];
static struct swsusp_header swsusp_header;

static inline loff_t check_free_swap(int dev)
{
	int error;
	loff_t free_swap;

	error = ioctl(dev, SNAPSHOT_AVAIL_SWAP, &free_swap);
	if (!error)
		return free_swap;
	else
		printf("Error: errno = %d\n", errno);
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

static inline int set_swap_file(int dev, dev_t blkdev)
{
	return ioctl(dev, SNAPSHOT_SET_SWAP_FILE, blkdev);
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
		if (lseek(fd, offset, SEEK_SET) == offset)
			cnt = write(fd, buf, size);
		if (cnt == size)
			res = 0;
		else
			res = cnt < 0 ? cnt : -EIO;
	}
	return res;
}

static char write_buffer[BUFFER_SIZE];
/* This MUST NOT be less than PAGE_SIZE */
static unsigned int max_block_size = PAGE_SIZE;
#ifdef CONFIG_ENCRYPT
static unsigned char encrypt_buffer[BUFFER_SIZE];
#endif

/*
 *	The swap_map_handle structure is used for handling swap in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page cur;
	struct swap_area cur_area;
	unsigned int cur_alloc;
	loff_t cur_swap;
	unsigned int k;
	int dev, fd;
	struct md5_ctx ctx;
#ifdef CONFIG_ENCRYPT
	BF_KEY key;
	unsigned char ivec[IVEC_SIZE];
	int num;
#endif
};

static int init_swap_writer(struct swap_map_handle *handle, int dev, int fd)
{
	memset(&handle->cur, 0, sizeof(struct swap_map_page));
	handle->cur_swap = get_swap_page(dev);
	if (!handle->cur_swap)
		return -ENOSPC;
	handle->cur_area.offset = get_swap_page(dev);
	if (!handle->cur_area.offset)
		return -ENOSPC;
	handle->cur_area.size = 0;
	handle->cur_alloc = PAGE_SIZE;
	handle->k = 0;
	handle->dev = dev;
	handle->fd = fd;
	if (compute_checksum)
		md5_init_ctx(&handle->ctx);
	return 0;
}

static int prepare(void *buf, int disp)
{
	struct buf_block *block = (struct buf_block *)(write_buffer + disp);

	if (compress) {
		unsigned short cnt;

		cnt = lzf_compress(buf, PAGE_SIZE,
				block->data, PAGE_SIZE - sizeof(short));
		if (!cnt) {
			memcpy(block->data, buf, PAGE_SIZE);
			cnt = PAGE_SIZE;
		} else {
			compr_diff += PAGE_SIZE - cnt;
		}
		compr_diff -= sizeof(short);
		block->size = cnt;
		cnt += sizeof(short);
		return cnt;
	}
	memcpy(block, buf, PAGE_SIZE);
	return PAGE_SIZE;
}

static int try_get_more_swap(struct swap_map_handle *handle)
{
	loff_t offset;

	while (handle->cur_area.size > handle->cur_alloc) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		if (offset == handle->cur_area.offset + handle->cur_alloc) {
			handle->cur_alloc += PAGE_SIZE;
		} else {
			handle->cur_area.offset = offset;
			handle->cur_alloc = PAGE_SIZE;
		}
	}
	return 0;
}

static int flush_buffer(struct swap_map_handle *handle)
{
	void *src = write_buffer;
	int error;

#ifdef CONFIG_ENCRYPT
	if (encrypt) {
		BF_cfb64_encrypt(src, encrypt_buffer, handle->cur_area.size,
			&handle->key, handle->ivec, &handle->num, BF_ENCRYPT);
		src = encrypt_buffer;
	}
#endif
	error = write_area(handle->fd, src, handle->cur_area.offset,
			handle->cur_area.size);
	if (error)
		return error;
	handle->cur.entries[handle->k].offset = handle->cur_area.offset;
	handle->cur.entries[handle->k].size = handle->cur_area.size;
	return 0;
}

static int swap_write_page(struct swap_map_handle *handle, void *buf)
{
	loff_t offset = 0;
	int error;

	if (compute_checksum)
		md5_process_block(buf, PAGE_SIZE, &handle->ctx);

	if (!handle->cur_area.size) {
		/* No data in the write buffer */
		handle->cur_area.size = prepare(buf, 0);
		return try_get_more_swap(handle);
	}

	if (handle->cur_alloc + max_block_size <= BUFFER_SIZE) {
		if (handle->cur_area.size + max_block_size <= handle->cur_alloc) {
			handle->cur_area.size += prepare(buf, handle->cur_area.size);
			return 0;
		}
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		if (offset == handle->cur_area.offset + handle->cur_alloc) {
			handle->cur_alloc += PAGE_SIZE;
			handle->cur_area.size += prepare(buf, handle->cur_area.size);
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
	handle->cur_alloc = PAGE_SIZE;
	handle->cur_area.size = prepare(buf, 0);
	error = try_get_more_swap(handle);
	if (error)
		return error;
	if (++handle->k >= MAP_PAGE_ENTRIES) {
		offset = get_swap_page(handle->dev);
		if (!offset)
			return -ENOSPC;
		handle->cur.next_swap = offset;
		error = write_area(handle->fd, &handle->cur, handle->cur_swap,
				sizeof(struct swap_map_page));
		if (error)
			return error;
		memset(&handle->cur, 0, sizeof(struct swap_map_page));
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
		error = write_area(handle->fd, &handle->cur, handle->cur_swap,
				sizeof(struct swap_map_page));

	return error;
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
		ret = read(handle->dev, page_buffer, PAGE_SIZE);
		if (ret > 0) {
			error = swap_write_page(handle, page_buffer);
			if (error)
				break;
			if (!(nr_pages % m))
				printf("\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}
	} while (ret > 0);
	if (ret < 0)
		error = -errno;
	if (!error)
		printf(" done (%u pages)\n", nr_pages);
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

	printf("suspend: Free swap: %lu bytes\n", (unsigned long) free_swap);
	return free_swap > size;
}

static int mark_swap(int fd, loff_t start)
{
	int error = 0;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &swsusp_header, PAGE_SIZE) < (ssize_t)PAGE_SIZE)
		return -EIO;
	if (!memcmp("SWAP-SPACE", swsusp_header.sig, 10) ||
	    !memcmp("SWAPSPACE2", swsusp_header.sig, 10)) {
		memcpy(swsusp_header.orig_sig, swsusp_header.sig, 10);
		memcpy(swsusp_header.sig, SWSUSP_SIG, 10);
		swsusp_header.image = start;
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &swsusp_header, PAGE_SIZE) < (ssize_t)PAGE_SIZE)
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
	static char header_buffer[PAGE_SIZE];
	struct swsusp_info *header = (struct swsusp_info *)header_buffer;
	loff_t start;
	int error;

	printf("suspend: System snapshot ready. Preparing to write\n");
	start = get_swap_page(snapshot_fd);
	if (!start)
		return -ENOSPC;
	error = read(snapshot_fd, header_buffer, PAGE_SIZE);
	if (error < (int)PAGE_SIZE)
		return error < 0 ? error : -EFAULT;
	printf("suspend: Image size: %lu kilobytes\n", header->size / 1024);
	if (!enough_swap(snapshot_fd, header->size) && !compress) {
		printf("suspend: Not enough free swap\n");
		return -ENOSPC;
	}
	error = init_swap_writer(&handle, snapshot_fd, resume_fd);
	if (!error) {
		header->map_start = handle.cur_swap;
		header->image_flags = 0;
		if (compute_checksum)
			header->image_flags |= IMAGE_CHECKSUM;
		if (compress) {
			header->image_flags |= IMAGE_COMPRESSED;
			max_block_size = sizeof(struct buf_block);
		}
#ifdef CONFIG_ENCRYPT
		if (encrypt) {
			if (use_RSA) {
				struct key_data *key_data;

				key_data = (struct key_data *)encrypt_buffer;
				BF_set_key(&handle.key, KEY_SIZE, key_data->key);
				memcpy(handle.ivec, key_data->ivec, IVEC_SIZE);
				handle.num = 0;
				header->image_flags |= IMAGE_ENCRYPTED | IMAGE_USE_RSA;
				memcpy(&header->rsa_data, &key_data->rsa_data,
					sizeof(struct RSA_data));
				memcpy(&header->key_data,
					encrypt_buffer + sizeof(struct key_data),
					sizeof(struct encrypted_key));
			} else {
				int j;

				encrypt_init(&handle.key, handle.ivec, &handle.num,
						write_buffer, encrypt_buffer, 1);
				get_random_salt(header->salt, IVEC_SIZE);
				for (j = 0; j < IVEC_SIZE; j++)
					handle.ivec[j] ^= header->salt[j];
				header->image_flags |= IMAGE_ENCRYPTED;
			}
		}
#endif
		error = save_image(&handle, header->pages - 1);
	}
	if (!error) {
		error = flush_swap_writer(&handle);
		if (compute_checksum)
			md5_finish_ctx(&handle.ctx, header->checksum);
	}
	if (!error)
		error = write_area(resume_fd, header, start, PAGE_SIZE);
	if (!error) {
		if (compress) {
			double delta = header->size - compr_diff;

			printf("suspend: Compression ratio %4.2lf\n",
				delta / header->size);
		}
		printf( "S" );
		error = mark_swap(resume_fd, start);
	}
	fsync(resume_fd);
	if (!error)
		printf( "|" );
	printf("\n");
	return error;
}

static int reset_signature(int fd)
{
	int ret, error = 0;

	error = lseek(fd, 0, SEEK_SET);

	if (!error) {
		memset(&swsusp_header, 0, sizeof(swsusp_header));
		ret = read(fd, &swsusp_header, PAGE_SIZE);
		if (ret == PAGE_SIZE) {
			if (memcmp(SWSUSP_SIG, swsusp_header.sig, 10)) {
				/* Impossible? We wrote signature and it is not there?! */
				error = -EINVAL;
			}
		} else {
			error = ret < 0 ? ret : -EIO;
		}
	}

	if (!error) {
		/* Reset swap signature now */
		memcpy(swsusp_header.sig, swsusp_header.orig_sig, 10);
		error = lseek(fd, 0, SEEK_SET);
	}
	if (!error) {
		ret = write(fd, &swsusp_header, PAGE_SIZE);
		error = (ret != PAGE_SIZE);
	}
	fsync(fd);
	if (error) {
		printf("reset_signature: Error %d resetting the image.\n"
		       "There should be valid image on disk. Powerdown and do normal resume.\n"
		       "Continuing with this booted system will lead to data corruption.\n", 
		       error);
		while(1);
	}
	return error;
}

#define switch_vt_mode(fd, vt, ioc1, ioc2)	ioctl(fd, KDSKBMODE, ioc1); \
						ioctl(fd, VT_ACTIVATE, vt); \
						ioctl(fd, VT_WAITACTIVE, vt); \
						ioctl(fd, KDSKBMODE, ioc2);

int suspend_system(int snapshot_fd, int resume_fd, int vt_fd, int vt_no)
{
	loff_t avail_swap;
	unsigned long image_size;
	int attempts, in_suspend, error = 0;

	avail_swap = check_free_swap(snapshot_fd);
	if (avail_swap > pref_image_size)
		image_size = pref_image_size;
	else
		image_size = avail_swap;
	if (!image_size) {
		fprintf(stderr, "suspend: No swap space for suspend\n");
		return ENOSPC;
	}

	switch_vt_mode(vt_fd, vt_no, K_MEDIUMRAW, KD_GRAPHICS);
	error = freeze(snapshot_fd);
	switch_vt_mode(vt_fd, vt_no, KD_TEXT, K_XLATE);
	if (error)
		goto Unfreeze;

	printf("suspend: Snapshotting system\n");
	attempts = 2;
	do {
		if (set_image_size(snapshot_fd, image_size)) {
			error = errno;
			break;
		}
		if (!atomic_snapshot(snapshot_fd, &in_suspend)) {
			if (!in_suspend)
				break;
			error = write_image(snapshot_fd, resume_fd);
			if (!error) {
				if (!s2ram) {
					power_off();
					/* Signature is on disk, it is very dangerous now.
					 * We'd do resume with stale caches on next boot. */
					printf("Powerdown failed. That's impossible.\n");
					while(1);
				} else {
					/* If we die (and allow system to continue) between
                                         * now and reset_signature(), very bad things will
                                         * happen. */
					error = ioctl(snapshot_fd, SNAPSHOT_S2RAM);
					if (error) {
						power_off();
						printf("Powerdown failed. That's impossible.\n");
						while(1);
					}
					reset_signature(resume_fd);
					free_swap_pages(snapshot_fd);
					free_snapshot(snapshot_fd);
					s2ram_resume();
					goto Unfreeze;
				}
			} else {
				free_swap_pages(snapshot_fd);
				free_snapshot(snapshot_fd);
				image_size = 0;
				error = -error;
				if (error != ENOSPC)
					break;
			}
		} else {
			error = errno;
			break;
		}
	} while (--attempts);

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

/**
 *	prepare_console - find a spare virtual terminal, open it and attach
 *	the standard streams to it.  The number of the currently active
 *	virtual terminal is saved via @orig_vc
 */

static int prepare_console(int *orig_vc, int *new_vc)
{
	int fd, error, vt = -1;
	char *vt_name = page_buffer;
	struct vt_stat vtstat;

	fd = console_fd("/dev/console");
	if (fd < 0)
		return fd;
	error = ioctl(fd, VT_GETSTATE, &vtstat);
	if (!error) {
		*orig_vc = vtstat.v_active;
		error = ioctl(fd, VT_OPENQRY, &vt);
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
		fprintf(stderr, "Could not activate the VT %d\n", vt);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, vt);
	if (error) {
		fprintf(stderr, "VT %d activation failed\n", vt);
		fflush(stderr);
		goto Close_fd;
	}
	char *msg = "\33[H\33[J";
	write(fd, msg, strlen(msg));
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	*new_vc = vt;
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
		fprintf(stderr, "Could not activate the VT %d\n", orig_vc);
		fflush(stderr);
		goto Close_fd;
	}
	error = ioctl(fd, VT_WAITACTIVE, orig_vc);
	if (error) {
		fprintf(stderr, "VT %d activation failed\n", orig_vc);
		fflush(stderr);
	}
Close_fd:
	close(fd);
}

static FILE *printk_file;

static inline void open_printk(void)
{
	printk_file = fopen("/proc/sys/kernel/printk", "r+");
}

static inline int get_kernel_console_loglevel(void)
{
	int level = -1;

	if (printk_file) {
		rewind(printk_file);
		fscanf(printk_file, "%d", &level);
	}
	return level;
}

static inline void set_kernel_console_loglevel(int level)
{
	if (printk_file) {
		rewind(printk_file);
		fprintf(printk_file, "%d\n", level);
	}
}

static inline void close_printk(void)
{
	if (printk_file)
		fclose(printk_file);
}

#ifdef CONFIG_ENCRYPT
static void generate_key(void)
{
	RSA *rsa;
	int fd, rnd_fd;
	struct key_data *key_data;
	struct RSA_data *rsa_data;
	unsigned char *buf;

	fd = open(key_name, O_RDONLY);
	if (fd < 0)
		return;

	rsa = RSA_new();
	if (!rsa)
		goto Close;

	key_data = (struct key_data *)encrypt_buffer;
	rsa_data = &key_data->rsa_data;
	if (read(fd, rsa_data, sizeof(struct RSA_data)) <= 0)
		goto Free_rsa;

	buf = rsa_data->data;
	rsa->n = BN_mpi2bn(buf, rsa_data->n_size, NULL);
	buf += rsa_data->n_size;
	rsa->e = BN_mpi2bn(buf, rsa_data->e_size, NULL);
	if (!rsa->n || !rsa->e)
		goto Free_rsa;

	rnd_fd = open("/dev/urandom", O_RDONLY);
	if (rnd_fd > 0) {
		unsigned short size = KEY_SIZE + IVEC_SIZE;

		if (read(rnd_fd, key_data->key, size) == size) {
			struct encrypted_key *enc_key;
			int ret;

			enc_key = (struct encrypted_key *)(encrypt_buffer
					+ sizeof(struct key_data));
			ret = RSA_public_encrypt(size, key_data->key,
					enc_key->data, rsa, RSA_PKCS1_PADDING);
			if (ret > 0) {
				enc_key->size = ret;
				use_RSA = 'y';
			}
		}
		close(rnd_fd);
	}
Free_rsa:
	RSA_free(rsa);
Close:
	close(fd);
}
#endif

int main(int argc, char *argv[])
{
	char *chroot_path = page_buffer;
	struct stat stat_buf;
	int resume_fd, snapshot_fd, vt_fd, orig_vc = -1, suspend_vc = -1;
	dev_t resume_dev;
	int orig_loglevel, ret = 0;

	if (get_config("suspend", argc, argv, PARAM_NO, parameters, resume_dev_name))
		return EINVAL;
	if (compute_checksum != 'y' && compute_checksum != 'Y')
		compute_checksum = 0;
#ifdef CONFIG_COMPRESS
	if (compress != 'y' && compress != 'Y')
		compress = 0;
#endif
#ifdef CONFIG_ENCRYPT
	if (encrypt == 'y' || encrypt == 'Y')
		generate_key();
	else
		encrypt = 0;
#endif
	if (s2ram != 'y' && s2ram != 'Y')
		s2ram = 0;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		fprintf(stderr, "suspend: Could not lock myself\n");
		return errno;
	}

	if (stat(resume_dev_name, &stat_buf)) {
		fprintf(stderr, "suspend: Could not stat the resume device file\n");
		return ENODEV;
	}
	if (!S_ISBLK(stat_buf.st_mode)) {
		fprintf(stderr, "suspend: Invalid resume device\n");
		return EINVAL;
	}
	resume_fd = open(resume_dev_name, O_RDWR);
	if (resume_fd < 0) {
		fprintf(stderr, "suspend: Could not open the resume device\n");
		return errno;
	}
	resume_dev = stat_buf.st_rdev;

	if (stat(snapshot_dev_name, &stat_buf)) {
		fprintf(stderr, "suspend: Could not stat the snapshot device file\n");
		ret = ENODEV;
		goto Close_resume_fd;
	}

	if (s2ram) {
		/* if s2ram_prepare returned != 0, better not try to suspend to RAM */
		s2ram = !s2ram_prepare();
	}

	if (!S_ISCHR(stat_buf.st_mode)) {
		fprintf(stderr, "suspend: Invalid snapshot device\n");
		ret = EINVAL;
		goto Close_resume_fd;
	}
	snapshot_fd = open(snapshot_dev_name, O_RDONLY);
	if (snapshot_fd < 0) {
		fprintf(stderr, "suspend: Could not open the snapshot device\n");
		ret = errno;
		goto Close_resume_fd;
	}

	if (set_swap_file(snapshot_fd, resume_dev)) {
		fprintf(stderr, "suspend: Could not use the resume device "
			"(try swapon -a)\n");
		ret = errno;
		goto Close_snapshot_fd;
	}

	vt_fd = prepare_console(&orig_vc, &suspend_vc);
	if (vt_fd < 0) {
		fprintf(stderr, "suspend: Could not open a virtual terminal\n");
		ret = errno;
		goto Close_snapshot_fd;
	}

	open_printk();
	orig_loglevel = get_kernel_console_loglevel();
	set_kernel_console_loglevel(suspend_loglevel);

	sprintf(chroot_path, "/proc/%d", getpid());
	if (!s2ram && chroot(chroot_path)) {
		fprintf(stderr, "suspend: Could not chroot to %s\n", chroot_path);
		ret = errno;
		goto Restore_console;
	}
	chdir("/");

	sync();

	ret = suspend_system(snapshot_fd, resume_fd, vt_fd, suspend_vc);

	if (orig_loglevel >= 0)
		set_kernel_console_loglevel(orig_loglevel);
	close_printk();

Restore_console:
	restore_console(vt_fd, orig_vc);
Close_snapshot_fd:
	close(snapshot_fd);
Close_resume_fd:
	close(resume_fd);

	return ret;
}
