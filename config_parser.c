/*
 * config_parser.c
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

#include "config_parser.h"
#include "encrypt.h"

int parse_line(char *str, struct config_par *parv)
{
	char *dst, *fmt;
	int error = 0;
	int i, k;

	/* Skip white space */
	while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n')
		str++;
	/* Skip the lines containing white space only */
	if (!*str)
		goto cleanup;
	/* Ignore comments */
	if (*str == '#')
		goto cleanup;

	/* Compare with parameter names */
	for (i=0; parv[i].name; i++) {
		k = strlen(parv[i].name);
		if (!strncmp(parv[i].name, str, k)) {
			if (!parv[i].ptr)
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
				fmt = parv[i].fmt;
				if (!strncmp(fmt, "%s", 2)) {
					dst = parv[i].ptr;
					k = parv[i].len;
					strncpy(dst, str, k - 1);
					k = strlen(dst) - 1;
					if (dst[k] == '\n')
						dst[k] = '\0';
				} else {
					k = sscanf(str, fmt, parv[i].ptr);
					if (k <= 0)
						error = -EINVAL;
				}
				break;
			}
		}
	}

	if (!parv[i].name) {
		error = -EINVAL;
	}

cleanup:
	return error;
}

/**
 *	parse - read and parse the configuration file
 *	note that a non-present config file is not considered an error here
 */

int parse(char *my_name, char *file_name, struct config_par *parv)
{
	char buf[MAX_STR_LEN];
	char *str;
	struct stat stat_buf;
	FILE *file;
	int error, i;

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
		error = parse_line(str, parv);
	} while (!error);
	fclose(file);
	if (error)
		fprintf(stderr, "%s: Error in configuration file, line %d\n",
			my_name, i);
	return error;
}

/* We're abusing struct option a bit. usage() expects an \0 in the
 * name string, and after that a comment.
 */
void usage(char *my_name, struct option *options, const char *short_options)
{
	struct option *opt;

	printf("Usage: %s [options]", my_name);
	for (opt = options; opt->name; opt++)
	{
		const char *descr = opt->name + strlen(opt->name) + 1;
		if (strchr(short_options,opt->val))
			printf("\n  -%c, --%s", opt->val, opt->name);
		else
			printf("\n  --%s", opt->name);

		if (opt->has_arg)
			printf(" <%s>", opt->name);

		if (strlen(descr))
			printf("\t%s",descr);
	}

	printf("\n");
}
