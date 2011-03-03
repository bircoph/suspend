/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

#include "config.h"
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#ifndef S2RAM
#define S2RAM
#endif
#include "vt.h"
#include "s2ram.h"
#include "config_parser.h"


int s2ram_check_kms(void)
{
	DIR *sysfs_dir;
	struct dirent *dentry;
	int ret = -ENOENT;

	sysfs_dir = opendir("/sys/class/drm");
	if (!sysfs_dir)
		return errno;

	while ((dentry = readdir(sysfs_dir))) {
		if (!strncmp(dentry->d_name, "card0-", 6)) {
			ret = 0;
			break;
		}
	}

	closedir(sysfs_dir);

	return ret;
}


/* Actually enter the suspend. May be ran on frozen system. */
int s2ram_generic_do(void)
{
	int ret = 0;
	FILE *f = fopen("/sys/power/state", "w");
	if (!f) {
		printf("/sys/power/state does not exist; what kind of ninja mutant machine is this?\n");
		return ENODEV;
	}
	if (fprintf(f, "mem") < 0) {
		ret = errno;
		perror("s2ram_do");
	}
	/* usually only fclose fails, not fprintf, so it does not matter
	 * that we might overwrite the previous error.
	 */
	if (fclose(f) < 0) {
		ret = errno;
		perror("s2ram_do");
	}
	return ret;
} 
