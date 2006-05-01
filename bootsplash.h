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
int bootsplash_to_silent(void);
int bootsplash_to_verbose(void);
void bootsplash_switch_to(void);

#endif /* BOOTSPLASH_H */
