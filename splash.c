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

#include "splash.h"
#include "bootsplash.h"
#include "encrypt.h"

/**
 *	dummy functions in case if no splash system was found or
 *	bootsplashing is disabled
 */
static int splash_dummy_int_void(void) { return 0; }
static int splash_dummy_int_int(int p) { return 0; }
static void splash_dummy_void_void(void) { return; }
#ifndef CONFIG_ENCRYPT
static void splash_dummy_readpass (char *a, int b) { }
#endif

/* Tries to find a splash system and initializes interface functions */
void splash_prepare(struct splash *splash, int enabled)
{
	int error = 0;

	splash->finish      = splash_dummy_int_void;
	splash->progress    = splash_dummy_int_int;
	splash->switch_to   = splash_dummy_void_void;
	splash->getchar	    = getchar;
#ifdef CONFIG_ENCRYPT
	splash->read_password   = read_password;
#else
	splash->read_password   = splash_dummy_readpass;
#endif
	if (!enabled)
		return;

	printf("Looking for splash system... ");

	if (!(error = bootsplash_open())) {
		splash->finish      = bootsplash_finish;
		splash->progress    = bootsplash_progress;
		splash->switch_to   = bootsplash_switch_to;
		splash->getchar	    = bootsplash_getchar;
		splash->read_password = bootsplash_read_password;
	} else if (0) {
		/* add another splash system here */
	} else {
		printf("none\n");
		return;
	}
	printf("found\n");

	splash->progress(0);
}
