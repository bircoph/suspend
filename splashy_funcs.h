/*
 * splashy.h
 *
 * Boot splash related definitions for splashy method
 *
 * Copyright (C) 2006 Tim Dijkstra <tim@famdijkstra.org>
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef SPLASHY_FUNCS_H
#define SPLASHY_FUNCS_H
#ifdef CONFIG_SPLASHY

int splashy_open(void);
int splashy_finish(void);
int splashy_progress(int p);
void splashy_read_password(char *, int);
int splashy_getchar(void);
void splashy_print(const char *);

#endif
#endif /* SPLASHY_FUNCS_H */

