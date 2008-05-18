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
#include <linux/fs.h>

#include "suspend_ioctls.h"
#include "encrypt.h"

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
};

#define SNAPSHOT_ATOMIC_SNAPSHOT	_IOW(SNAPSHOT_IOC_MAGIC, 3, void *)
#define SNAPSHOT_SET_IMAGE_SIZE		_IOW(SNAPSHOT_IOC_MAGIC, 6, unsigned long)
#define SNAPSHOT_AVAIL_SWAP		_IOR(SNAPSHOT_IOC_MAGIC, 7, void *)
#define SNAPSHOT_GET_SWAP_PAGE		_IOR(SNAPSHOT_IOC_MAGIC, 8, void *)
#define SNAPSHOT_SET_SWAP_FILE	_IOW(SNAPSHOT_IOC_MAGIC, 10, unsigned int)
#define SNAPSHOT_PMOPS		_IOW(SNAPSHOT_IOC_MAGIC, 12, unsigned int)

#define PMOPS_PREPARE	1
#define PMOPS_ENTER	2
#define PMOPS_FINISH	3

struct image_header_info {
	unsigned long		pages;
	uint32_t		flags;
	loff_t			map_start;
	loff_t			image_data_size;
	unsigned char		checksum[16];
#ifdef CONFIG_ENCRYPT
	unsigned char		salt[CIPHER_BLOCK];
	struct RSA_data	rsa;
	struct encrypted_key	key;
#endif
	double			writeout_time;
};

#define IMAGE_CHECKSUM		0x0001
#define IMAGE_COMPRESSED	0x0002
#define IMAGE_ENCRYPTED		0x0004
#define IMAGE_USE_RSA		0x0008
#define PLATFORM_SUSPEND	0x0010

#define SWSUSP_SIG	"ULSUSPEND"

struct swsusp_header {
	loff_t image;
	char	orig_sig[10];
	char	sig[10];
} __attribute__((packed));

static inline void report_unsupported_ioctl(char *name)
{
	printf("The %s ioctl is not supported by the kernel\n", name);
}

static inline int freeze(int dev)
{
	return ioctl(dev, SNAPSHOT_FREEZE, 0);
}

static inline int unfreeze(int dev)
{
	return ioctl(dev, SNAPSHOT_UNFREEZE, 0);
}

static inline int platform_prepare(int dev)
{
	int error;

	error = ioctl(dev, SNAPSHOT_PLATFORM_SUPPORT, 1);
	if (error && errno == ENOTTY) {
		report_unsupported_ioctl("SNAPSHOT_PLATFORM_SUPPORT");
		error = ioctl(dev, SNAPSHOT_PMOPS, PMOPS_PREPARE);
	}
	return error;
}

static inline int platform_finish(int dev)
{
	return ioctl(dev, SNAPSHOT_PMOPS, PMOPS_FINISH);
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

struct extent {
	loff_t start;
	loff_t end;
};

/* The number 1 below is arbitrary.  The actual size of data[] is variable */
struct buf_block {
	size_t size;
	char data[1];
} __attribute__((packed));

#define BUFFER_PAGES	32

#define SNAPSHOT_DEVICE	"/dev/snapshot"
#define RESUME_DEVICE ""

#define IMAGE_SIZE	(500 * 1024 * 1024)

#define SUSPEND_LOGLEVEL	1
#define MAX_LOGLEVEL		8

#define SUSPEND_SWAPPINESS	100

#define ABORT_KEY_CODE	127
#define ABORT_KEY_NAME	"backspace"
#define REBOOT_KEY_CODE	'r'
#define REBOOT_KEY_NAME	"r"
