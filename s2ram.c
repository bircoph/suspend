/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Copyright 2011 Rodolfo García Peñas (kix) <kix@kix.es>
 * Distribute under GPLv2.
 */

#include <ctype.h>
#include "config.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifndef S2RAM
#define S2RAM
#endif
#include "vt.h"
#include "s2ram.h"
#include "config_parser.h"

static char s2ram_force;
static char s2ram_quirks[MAX_STR_LEN];

static struct config_par s2ram_parameters[] = {
	{
		.name = "s2ram_force",
		.fmt = "%c",
		.ptr = &s2ram_force,
	},
	{
		.name = "s2ram_quirks",
		.fmt = "%s",
		.ptr = s2ram_quirks,
		.len = MAX_STR_LEN
	},
	{
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};


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

/* Parse the configuration file */
int get_s2ram_config(void)
{
	int error;
	char *conf_name = CONFIG_FILE;
	struct stat stat_buf;
	char *ret_name = NULL;

	if (stat(conf_name, &stat_buf)) {
		fprintf(stderr, "%s: Could not stat configuration file\n", conf_name);
		return -ENOENT;
	} else {
		error = parse(ret_name, conf_name, s2ram_parameters);
		if (error) {
			fprintf(stderr, "%s: Could not parse config file\n", ret_name);
			return error;
		}
		if (s2ram_force == 'y') {
			force = 1;

			if (strlen(s2ram_quirks) != 0) {
				printf("Using s2ram_quirks %s\n", s2ram_quirks);
				s2ram_add_quirks(s2ram_quirks);
			}
			return 0;
		}
		return -1;
	}
}

void s2ram_add_quirks(char *s2ram_quirks)
{
	int i;

	/* Move to lowercase */
	for (i=0; s2ram_quirks[i]; i++)
		s2ram_quirks[i] = tolower(s2ram_quirks[i]);

	if (strstr(s2ram_quirks, "vbe_post") != NULL)
		flags |= VBE_POST;
	if (strstr(s2ram_quirks, "vbe_save") != NULL)
		flags |= VBE_POST;
	if (strstr(s2ram_quirks, "vbe_mode") != NULL)
		flags |= VBE_MODE;
	if (strstr(s2ram_quirks, "radeon_off") != NULL)
		flags |= RADEON_OFF;
	if (strstr(s2ram_quirks, "pci_save") != NULL)
		flags |= PCI_SAVE;
	if (strstr(s2ram_quirks, "s3_bios") != NULL)
		flags |= S3_BIOS;
	if (strstr(s2ram_quirks, "s3_mode") != NULL)
		flags |= S3_MODE;
	if (strstr(s2ram_quirks, "fb_nosuspend") != NULL)
		fb_nosuspend = 1;
}
