/*
 * bootsplash.c
 *
 * Bootsplash.org splash method support
 *
 * Copyright (C) 2006 Holger Macht <holger@homac.de>
 *
 * This file is released under the GPLv2.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "vt.h"
#include "encrypt.h"

#define BOOTSPLASH_PROC "/proc/splash"
#define MAX_LINE_SIZE   1024

static FILE *splash_file = NULL;

int bootsplash_finish(void)
{
	if (splash_file) {
		fclose(splash_file);
		splash_file = NULL;
	}

	return 0;
}

static int bootsplash_write(const char *buf)
{
	int error = 0;

	if (!splash_file)
		return -1;

	error = fputs(buf, splash_file);
	if (error == EOF) {
		fprintf(stderr, "Could not write '%s' to %s: %s\n",
			buf, BOOTSPLASH_PROC, strerror(errno));
		bootsplash_finish();
		return error;
	}
	fflush(splash_file);
	return 0;
}

static int bootsplash_to_silent(void)
{
	return bootsplash_write("silent\n");
}

static int bootsplash_to_verbose(void)
{
	return bootsplash_write("verbose\n");
}

int bootsplash_open(void)
{
	char *str, buf[MAX_LINE_SIZE];

	splash_file = fopen(BOOTSPLASH_PROC, "r+");
	if (!splash_file)
		return -1;

	str = fgets(buf, MAX_LINE_SIZE, splash_file);
	if (!str)
		goto close;

	if (!strstr(buf, ": on"))
		goto close;

	bootsplash_to_silent();
	return 0;
close:
	bootsplash_finish();
	return -1;
}

void bootsplash_switch_to(void)
{
	if (splash_file) {
		/* assume that bootsplash is on console 1, there's no way
		 * to figure this out AFAIK */
		chvt(1);
	}
}

inline int bootsplash_progress(int p)
{
	char buf[MAX_LINE_SIZE];

	if (!splash_file)
		return -1;

        /* 65535 corresponds to 100 % on the progress bar */
	snprintf(buf, MAX_LINE_SIZE, "show %d\n", p * (65535 / 100));
	return bootsplash_write(buf);
}

void bootsplash_read_password(char * buf, int vrfy) 
{
#if CONFIG_ENCRYPT
	bootsplash_to_verbose();
	read_password(buf, vrfy);
	bootsplash_to_silent();
#endif
}

int bootsplash_dialog(const char *prompt)
{
	int ret;
	bootsplash_to_verbose();
	printf(prompt);
	ret = getchar();
	bootsplash_to_silent();

	return ret;
}
