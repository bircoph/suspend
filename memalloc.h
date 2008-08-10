/*
 * memalloc.h
 *
 * Memory allocation definitions for user land hibernation utilities.
 *
 * Copyright (C) 2008 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

struct mem_slot {
	void *addr;
	size_t size;
};

#define ALIGN_QWORD	8

#define BUFFER_PAGES	32

extern unsigned int page_size;
/* This MUST be an multipe of page_size */
extern unsigned int buffer_size;

extern void get_page_and_buffer_sizes(void);
extern size_t round_up_page_size(size_t size);
extern size_t round_down_page_size(size_t size);
extern int init_memalloc(size_t aux, size_t pool);
extern void *getmem(size_t size);
extern void freemem(void *address);
extern void free_memalloc(void);
