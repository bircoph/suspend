/*
 * config.c
 *
 * Configuration file parser for userland suspend tools
 *
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "encrypt.h"

/**
 *	parse - read and parse the configuration file
 */

static int parse(char *my_name, char *file_name, int parc, struct config_par *parv)
{
	char *str, *dst, *fmt, buf[MAX_STR_LEN];
	FILE *file;
	int error, i, j, k;

	file = fopen(file_name, "r");
	if (!file) {
		fprintf(stderr, "%s: Could not open configuration file\n",
			my_name);
		return -errno;
	}
	error = 0;
	i = 0;
	do {
		str = fgets(buf, MAX_STR_LEN, file);
		if (!str)
			break;
		i++;
		/* Ignore comments */
		if (*str == '#')
			continue;
		/* Skip white space */
		while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n')
			str++;
		/* Skip the lines containing white space only */
		if (!*str)
			continue;
		/* Compare with parameter names */
		for (j =  0; j < parc; j++) {
			k = strlen(parv[j].name);
			if (!strncmp(parv[j].name, str, k)) {
				if (!parv[j].ptr)
					break;
				str += k;
				while (*str == ' ' || *str == '\t')
					str++;
				if (*str != ':' && *str != '=') {
					error = -EINVAL;
					break;
				}
				str++;
				while (*str == ' ' || *str == '\t')
					str++;
				if (*str) {
					fmt = parv[j].fmt;
					if (!strncmp(fmt, "%s", 2)) {
						dst = parv[j].ptr;
						k = parv[j].len;
						strncpy(dst, str, k - 1);
						k = strlen(dst) - 1;
						if (dst[k] == '\n')
							dst[k] = '\0';
					} else {
						k = sscanf(str, fmt, parv[j].ptr);
						if (k <= 0)
							error = -EINVAL;
					}
					break;
				}
			}
		}
		if (j >= parc)
			error = -EINVAL;
	} while (!error);
	fclose(file);
	if (error)
		fprintf(stderr, "%s: Error in configuration file, line %d\n",
			my_name, i);
	return error;
}

int get_config(char *my_name, int argc, char *argv[],
	       int parc, struct config_par *parv, char *special)
{
	struct stat stat_buf;
	int ret = 0;
#ifdef CONFIG_ENCRYPT
	if (!strcmp(argv[1], "-p")) {
		static char buf[PASS_SIZE];
		extern char *passphrase;

		argc--;
		argv++;
		fgets(buf, 10240, stdin);
		passphrase = buf;

	}
#endif

	if (argc <= 2) {
		if (!stat(CONFIG_FILE, &stat_buf))
			ret = parse(my_name, CONFIG_FILE, parc, parv);
		if (ret < 0 || argc < 2)
			return ret;
		strncpy(special, argv[1], MAX_STR_LEN - 1);
		return 0;
	}

	if (strncmp(argv[1], "-f", 2)) {
		fprintf(stderr, "Usage: %s [-p][-f config][resume_device]\n",
			my_name);
		return -EINVAL;
	}

	ret = parse(my_name, argv[2], parc, parv);
	if (ret)
		return ret;

	if (argc > 3)
		strncpy(special, argv[3], MAX_STR_LEN - 1);

	return 0;
}
