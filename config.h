/*
 * config.h
 *
 * Configuration file-related definitions for user space suspend and resume
 * tools.
 *
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#define MAX_STR_LEN	256

struct config_par {
	char *name;
	char *fmt;
	void *ptr;
	unsigned int len;
};

int get_config(char *my_name,
               int argc,
               char *argv[],
               int parc,
               struct config_par *parv,
               char *special);

#define CONFIG_FILE	"/etc/suspend.conf"
#define S2BOTH_NAME	"s2both"
