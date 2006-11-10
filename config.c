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
#include <getopt.h>

#include "config.h"
#include "encrypt.h"

/**
 *	parse - read and parse the configuration file
 *	note that a non-present config file is not considered an error here
 */

int parse(char *my_name, char *file_name, int parc, struct config_par *parv)
{
	char *str, *dst, *fmt, buf[MAX_STR_LEN];
	struct stat stat_buf;
	FILE *file;
	int error, i, j, k;

	if (stat(file_name, &stat_buf)) {
		fprintf(stderr, "%s: Could not stat configuration file\n",
			my_name);
		return 0;
	}
	file = fopen(file_name, "r");
	if (!file) {
		fprintf(stderr, "%s: Could not open configuration file\n",
			my_name);
		return 0;
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

void usage(char *my_name, struct option *options)
{
	struct option *opt;

	printf("Usage: %s ", my_name);
	for (opt = options; opt->name; opt++)
		if (opt->has_arg)
			printf("[-%c <%s>]", opt->val, opt->name);
		else
			printf("[-%c]", opt->val);

	printf(" [<resume_device>]\n");
}
