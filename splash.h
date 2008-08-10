/*
 * splash.h
 *
 * Splash related definitions for userspace suspend and resume tools.
 *
 * Copyright (C) 2006 Holger Macht <holger@homac.de>
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef SPLASH_H
#define SPLASH_H

#include <linux/input.h>
#include <termios.h>

#define ABORT_KEY_CODE	KEY_BACKSPACE
#define ABORT_KEY_NAME	"backspace"
#define REBOOT_KEY_CODE	KEY_R
#define REBOOT_KEY_NAME	"r"
#define ENTER_KEY_CODE	KEY_ENTER
#define ENTER_KEY_NAME	"ENTER"

#define SPL_SUSPEND 1
#define SPL_RESUME 2

#define SPLASH_GENERIC_MESSAGE_SIZE 1024

/* generic interface functions for an arbitary splash method */
struct splash {
	int (*finish) (void);
	int (*progress) (int p);
	void (*switch_to) (void);
	void (*read_password) (char *, int);
	int (*dialog) (const char *);
	int (*prepare_abort) (struct termios *, struct termios *);
	char (*key_pressed) (void);
	void (*restore_abort) (struct termios *);
	void (*set_caption) (const char *);
};

void splash_prepare(struct splash *splash, int mode);

extern struct splash splash;

#endif /* SPLASH_H */
