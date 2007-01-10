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

#include <termios.h>

/* generic interface functions for an arbitary splash method */
struct splash {
	int (*finish) (void);
	int (*progress) (int p);
	void (*switch_to) (void);
	void (*read_password) (char *, int);
	int (*dialog) (const char *);
	int (*prepare_abort) (struct termios *, struct termios *);
	int (*key_pressed) (const char key);
	void (*restore_abort) (struct termios *);
};

void splash_prepare(struct splash *splash, int enabled);

#endif /* SPLASH_H */
