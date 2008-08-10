/*
 * memalloc.c
 *
 * Memory allocation functions for user land hibernation utilities.
 *
 * Copyright (C) 2008 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "memalloc.h"

unsigned int page_size;
unsigned int buffer_size;

/* Mask used for page aligning */
static size_t page_mask;
/* Memory pool used for memory allocations */
static void *mem_pool;
/* Size of the pool */
static size_t pool_size;
/* The array of objects representig memory allocations */
static struct mem_slot *slots;
/* Size of the slots array */
static unsigned int nr_slots;
/* The struct mem_slot object to be used next */
static struct mem_slot *cur_slot;

void get_page_and_buffer_sizes(void)
{
	page_size = getpagesize();
	page_mask = ~((size_t)page_size - 1);
	buffer_size = page_size * BUFFER_PAGES;
}

/**
 *	round_up_page_size - round given number up to the closest multiple of
 *                           page size
 *	@size: the number to round
 */
size_t round_up_page_size(size_t size)
{
	return ((size + page_size - 1) & page_mask);
}

/**
 *	round_down_page_size - round given number down to the closest multiple
 *                             of the page size lesser than that number
 *	@size: the number to round
 */
size_t round_down_page_size(size_t size)
{
	return size & page_mask;
}

/**
 *	init_memalloc - initialize memory allocation structures
 *	@aux: Number of bytes for the internal use of the allocator
 *	@pool: Number of bytes to preallocate for the users
 */
int init_memalloc(size_t aux, size_t pool)
{
	void *mem;
	unsigned long addr;
	size_t size, slots_size;

	slots_size = round_up_page_size(aux);
	pool_size = round_up_page_size(pool);
	/* The additional page_size may be necessary for page alignment */
	mem = malloc(slots_size + pool_size + page_size);
	if (!mem)
		return -ENOMEM;
	slots = mem;
	nr_slots = slots_size / sizeof(struct mem_slot);
	cur_slot = slots;
	/* Make the pool address be page aligned */
	mem += slots_size;
	addr = ((unsigned long)mem & ~((unsigned long)page_size - 1));
	if (addr < (unsigned long)mem)
		mem = (void *)(addr + page_size);
	mem_pool = mem;
	return 0;
}

/**
 *	getmem - get some memory from the preallocated pool
 *	@size: Number of bytes needed
 *
 *	Return the address of @size bytes of memory, aligned to the first
 *	natural power of 2 greater than or equal to @size and not greater than
 *	page_size.
 */
void *getmem(size_t size)
{
	void *addr = mem_pool;
	size_t used;

	if (cur_slot - slots >= nr_slots) {
		fprintf(stderr, "WARNING: Not enough memory slots\n");
		return NULL;
	}
	if (cur_slot > slots) {
		unsigned int align_size, align_mask;
		struct mem_slot *last_slot = cur_slot - 1;

		if (size >= page_size) {
			align_size = page_size;
			align_mask = page_mask;
		} else {
			align_size = ALIGN_QWORD;
			while (align_size < size)
				align_size <<= 1;
			align_mask = ~(align_size - 1);
		}
		addr += ((last_slot->addr - mem_pool + last_slot->size +
			align_size - 1) & align_mask);
	}
	used = addr - mem_pool;
	if (used > pool_size || size > pool_size - used) {
		fprintf(stderr, "WARNING: Not enough memory in the pool\n");
		return NULL;
	}
	cur_slot->addr = addr;
	cur_slot->size = size;
	cur_slot++;
	return addr;
}

/**
 *	free_slot - mark given memory slot as freed and try to remove it
 *	@slot: Number of memory slot to free
 */
static void free_slot(int slot)
{
	struct mem_slot *last_slot = cur_slot - 1;

	/*
	 * To free the slot, set its size to zero.  If this is the most recently
	 * allocated one, remove it from the array along with some other already
	 * freed slots (the size of the last remaining slot must be greater than
	 * zero).
	 */
	slots[slot].size = 0;
	if (slots + slot == last_slot) {
		do
			last_slot--;
		while (!last_slot->size && last_slot >= slots);
		cur_slot = last_slot + 1;
	}
}

/**
 *	freemem - free memory allocated by getmem
 *	@address: Start of the memory area to free
 */
void freemem(void *address)
{
	int i, j, k;

	/* Check if there is anything to do */
	if (cur_slot <= slots)
		return;
	/* Find the slot to free, using the observation that they are sorted */
	i = 0;
	j = cur_slot - slots - 1;
	do {
		k = (i + j) / 2;
		if (slots[k].addr == address) {
			free_slot(k);
			break;
		} else if (address > slots[k].addr) {
			if (i != k) {
				i = k;
			} else {
				if (slots[j].addr == address)
					free_slot(j);
				break;
			}
		} else {
			j = k;
		}
	} while (i < j);
}

/**
 *	end_memalloc - free memory used by the allocator
 */
void free_memalloc(void)
{
	if (slots)
		free(slots);
}
