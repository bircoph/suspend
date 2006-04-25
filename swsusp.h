/*
 * swsusp.h
 *
 * Common definitions for user space suspend and resume handlers.
 *
 * Copyright (C) 2005 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <stdint.h>

#include "encrypt.h"

#define SNAPSHOT_IOC_MAGIC	'3'
#define SNAPSHOT_FREEZE			_IO(SNAPSHOT_IOC_MAGIC, 1)
#define SNAPSHOT_UNFREEZE		_IO(SNAPSHOT_IOC_MAGIC, 2)
#define SNAPSHOT_ATOMIC_SNAPSHOT	_IOW(SNAPSHOT_IOC_MAGIC, 3, void *)
#define SNAPSHOT_ATOMIC_RESTORE		_IO(SNAPSHOT_IOC_MAGIC, 4)
#define SNAPSHOT_FREE			_IO(SNAPSHOT_IOC_MAGIC, 5)
#define SNAPSHOT_SET_IMAGE_SIZE		_IOW(SNAPSHOT_IOC_MAGIC, 6, unsigned long)
#define SNAPSHOT_AVAIL_SWAP		_IOR(SNAPSHOT_IOC_MAGIC, 7, void *)
#define SNAPSHOT_GET_SWAP_PAGE		_IOR(SNAPSHOT_IOC_MAGIC, 8, void *)
#define SNAPSHOT_FREE_SWAP_PAGES	_IO(SNAPSHOT_IOC_MAGIC, 9)
#define SNAPSHOT_SET_SWAP_FILE		_IOW(SNAPSHOT_IOC_MAGIC, 10, unsigned int)
#define SNAPSHOT_S2RAM			_IO(SNAPSHOT_IOC_MAGIC, 11)
#define SNAPSHOT_IOC_MAXNR	11

#define	LINUX_REBOOT_MAGIC1	0xfee1dead
#define	LINUX_REBOOT_MAGIC2	672274793

#define LINUX_REBOOT_CMD_RESTART	0x01234567
#define LINUX_REBOOT_CMD_HALT		0xCDEF0123
#define LINUX_REBOOT_CMD_POWER_OFF	0x4321FEDC
#define LINUX_REBOOT_CMD_RESTART2	0xA1B2C3D4
#define LINUX_REBOOT_CMD_SW_SUSPEND	0xD000FCE2

struct new_utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
	char domainname[65];
};

struct swsusp_info {
	struct new_utsname	uts;
	uint32_t		version_code;
	unsigned long		num_physpages;
	int			cpus;
	unsigned long		image_pages;
	unsigned long		pages;
	unsigned long		size;
	/* The following fields are added by the userland */
	loff_t			map_start;
	uint32_t		image_flags;
	unsigned char		checksum[16];
#ifdef CONFIG_ENCRYPT
	char			salt[IVEC_SIZE];
	struct RSA_data		rsa_data;
	struct encrypted_key	key_data;
#endif
};

#define IMAGE_CHECKSUM		0x0001
#define IMAGE_COMPRESSED	0x0002
#define IMAGE_ENCRYPTED		0x0004
#define IMAGE_USE_RSA		0x0008

#define SWSUSP_SIG	"ULSUSPEND"

struct swsusp_header {
	loff_t image;
	char	orig_sig[10];
	char	sig[10];
} __attribute__((packed));

static inline int freeze(int dev)
{
	return ioctl(dev, SNAPSHOT_FREEZE, 0);
}

static inline int unfreeze(int dev)
{
	return ioctl(dev, SNAPSHOT_UNFREEZE, 0);
}

static inline int atomic_snapshot(int dev, int *in_suspend)
{
	return ioctl(dev, SNAPSHOT_ATOMIC_SNAPSHOT, in_suspend);
}

static inline int atomic_restore(int dev)
{
	return ioctl(dev, SNAPSHOT_ATOMIC_RESTORE, 0);
}

static inline int free_snapshot(int dev)
{
	return ioctl(dev, SNAPSHOT_FREE, 0);
}

static inline int set_image_size(int dev, unsigned int size)
{
	return ioctl(dev, SNAPSHOT_SET_IMAGE_SIZE, size);
}

static inline int suspend_to_ram(int dev)
{
	return ioctl(dev, SNAPSHOT_S2RAM, 0);
}

static inline void reboot(void)
{
	syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
		LINUX_REBOOT_CMD_RESTART, 0);
}

static inline void power_off(void)
{
	syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
		LINUX_REBOOT_CMD_POWER_OFF, 0);
}

#ifndef SYS_sync_file_range
 #ifdef __i386__
  #define SYS_sync_file_range	314
 #endif
 #ifdef __x86_64__
  #define SYS_sync_file_range	277
 #endif
 #define SYNC_FILE_RANGE_WRITE	2
#endif

static inline int start_writeout(int fd)
{
#ifdef SYS_sync_file_range
	return syscall(SYS_sync_file_range, fd,
			(loff_t)0, (loff_t)0, SYNC_FILE_RANGE_WRITE);
#else
	errno = ENOSYS;
	return -1;
#endif
}

struct swap_area {
	loff_t offset;
	unsigned int size;
};

/* The number 4096 below is arbitrary.  The actual size of data[] is variable */
struct buf_block {
	unsigned short size;
	char data[4096];
} __attribute__((packed));

#define BUFFER_PAGES	32

#define SNAPSHOT_DEVICE	"/dev/snapshot"
#define RESUME_DEVICE ""

#define IMAGE_SIZE	(500 * 1024 * 1024)

#define SUSPEND_LOGLEVEL	1
#define MAX_LOGLEVEL		8

#define SUSPEND_SWAPPINESS	100

#define GEN_PARAM	8

#ifdef CONFIG_COMPRESS
#define COMPRESS_PARAM	1
#else
#define COMPRESS_PARAM	0
#endif

#ifdef CONFIG_ENCRYPT
#define ENCRYPT_PARAM	2
#else
#define ENCRYPT_PARAM	0
#endif

#define PARAM_NO	(GEN_PARAM + COMPRESS_PARAM + ENCRYPT_PARAM)
