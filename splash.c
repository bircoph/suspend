/*
 * splash.c
 *
 * Bootsplash integration into the suspend and resume tools
 *
 * Copyright (C) 2006 Holger Macht <holger@homac.de>
 *
 * This file is released under the GPLv2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "splash.h"
#include "bootsplash.h"
#include "splashy_funcs.h"
#include "encrypt.h"

#define INPUT_PATH "/dev/input/by-path"
#define MAX_INPUT_FD 8
static struct {
	int fds[MAX_INPUT_FD];
	int count;
	int highest;
} input_fds;

/**
 *	dummy functions in case if no splash system was found or
 *	bootsplashing is disabled
 */
static int splash_dummy_int_void(void) { return 0; }
static int splash_dummy_int_int(int p) { return 0; }
static void splash_dummy_void_void(void) { return; }
#ifndef CONFIG_ENCRYPT
static void splash_dummy_readpass(char *a, int b) { }
#endif
static int splash_dialog(const char *prompt) 
{
	printf(prompt);
	return getchar();
}
static int prepare_abort(struct termios *oldtrm, struct termios *newtrm) 
{
	int ret;

	ret = tcgetattr(0, oldtrm);
	if (!ret) {
		*newtrm = *oldtrm;
		newtrm->c_cc[VMIN] = 0;
		newtrm->c_cc[VTIME] = 0;
		newtrm->c_iflag = IGNBRK | IGNPAR | ICRNL | IMAXBEL;
		newtrm->c_lflag = 0;
		ret = tcsetattr(0, TCSANOW, newtrm);
	}

	return ret;
}

static char key_pressed(void)
{
	int err, i, active;
	struct input_event ev;
	struct timeval timeout = {0, 0};
	fd_set fds;

	if (!input_fds.count)
		return 0;

	active = -1; /* GCC STFU */
	FD_ZERO(&fds);
	for (i = 0; i < input_fds.count; i++)
		FD_SET(input_fds.fds[i], &fds);

	err = select(input_fds.highest + 1, &fds, NULL, NULL, &timeout);
	if (err < 0) {
		perror("select()");
		return 0;
	}
	if (err == 0)
		return 0;

	/* Get only the fist active fd */
	for (i = 0; i < input_fds.count; i++) {
		if (FD_ISSET(input_fds.fds[i], &fds)) {
			active = input_fds.fds[i];
			break;
		}
	}

	while ((err = read(active, &ev, sizeof(struct input_event))) != -1) {
		/* we only need key release events */
		if (ev.type == EV_KEY && ev.value == 0)
			return ev.code;
	}

	return 0;
}

static void restore_abort(struct termios *oldtrm) 
{
	tcsetattr(0, TCSANOW, oldtrm);
}

static int open_input_fd(void)
{
	int fd, i;
	struct dirent *it;
	DIR *dir;
	char input_dev[PATH_MAX];
	int err = 0;

	if (!(dir = opendir(INPUT_PATH))) {
		perror("Cannot open input directory");
		return -1;
	}

	i = 0;
	while ((it = readdir(dir))) {
		if (i == MAX_INPUT_FD)
			break;

		if (!strstr(it->d_name, "-event-kbd"))
			continue;

		sprintf(input_dev, "%s/%s", INPUT_PATH, it->d_name);

		fd = open(input_dev, O_RDONLY|O_NONBLOCK);
		if (fd == -1) {
			perror("opening input fd");
			continue;
		}
		input_fds.fds[i++] = fd;
		input_fds.count++;
		if (fd > input_fds.highest)
			input_fds.highest = fd;
	}

	if (!it && errno) {
		perror("readdir()");
		err = -errno;
	}
	closedir(dir);

	return err;
}

/* Tries to find a splash system and initializes interface functions */
void splash_prepare(struct splash *splash, int mode)
{
	splash->finish      = splash_dummy_int_void;
	splash->progress    = splash_dummy_int_int;
	splash->switch_to   = splash_dummy_void_void;
	splash->dialog	    = splash_dialog;
#ifdef CONFIG_ENCRYPT
	splash->read_password   = read_password;
#else
	splash->read_password   = splash_dummy_readpass;
#endif
	splash->prepare_abort	= prepare_abort;
	splash->restore_abort	= restore_abort;
	splash->key_pressed	= key_pressed;

	if (open_input_fd())
		fprintf(stderr, "Could not open keyboard input device\n");

	if (!mode)
		return;

	printf("Looking for splash system... ");

	if (!bootsplash_open()) {
		splash->finish      = bootsplash_finish;
		splash->progress    = bootsplash_progress;
		splash->switch_to   = bootsplash_switch_to;
		splash->dialog	    = bootsplash_dialog;
		splash->read_password = bootsplash_read_password;
#ifdef CONFIG_SPLASHY
	} else if (!splashy_open(mode)) {
		splash->finish      = splashy_finish;
		splash->progress    = splashy_progress;
		splash->dialog	    = splashy_dialog;
		splash->read_password   = splashy_read_password;
#endif
	} else if (0) {
		/* add another splash system here */
	} else {
		printf("none\n");
		return;
	}
	printf("found\n");

	splash->progress(0);
}
