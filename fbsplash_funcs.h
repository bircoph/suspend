/*
 * fbsplash.h
 *
 * Boot splash related definitions for fbsplash method
 *
 * Copyright (C) 2007 Alon Bar-Lev <alon.barlev@gmail.com>
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef FBSPLASH_FUNCS_H
#define FBSPLASH_FUNCS_H
#ifdef CONFIG_FBSPLASH

int fbsplashfuncs_open(int mode);
int fbsplashfuncs_finish(void);
int fbsplashfuncs_progress(int p);
void fbsplashfuncs_read_password(char *, int);
int fbsplashfuncs_dialog(const char *);
void fbsplashfuncs_print(const char *);
char fbsplashfuncs_key_pressed(void);
void fbsplashfuncs_set_caption(const char *);
void fbsplashfuncs_switch_to(void);

#endif
#endif /* FBSPLASH_FUNCS_H */

