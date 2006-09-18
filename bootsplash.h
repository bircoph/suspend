/*
 * bootsplash.h
 *
 * Bootsplash related definitions for bootsplash.org's method
 *
 * Copyright (C) 2006 Holger Macht <holger@homac.de>
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef BOOTSPLASH_H
#define BOOTSPLASH_H

int bootsplash_open(void);
int bootsplash_prepare(void);
int bootsplash_finish(void);
int bootsplash_progress(int p);
void bootsplash_switch_to(void);
void bootsplash_read_password(char *, int);
int bootsplash_dialog(const char *);

#endif /* BOOTSPLASH_H */
