/*
 * fbsplash.c
 *
 * fbsplash (framebuffer splash) splash method support
 *
 * Copyright (C) 2007 Alon Bar-Lev <alon.barlev@gmail.com>
 *
 * This file is released under the GPLv2.
 *
 */

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <linux/types.h>
#include <syscall.h>

#include "swsusp.h"
#include "splash.h"
#include "fbsplash_funcs.h"

char fbsplash_theme[] = "";

int main (void)
{
	char c;
	int r;
	int i;
	printf("fbsplashfuncs_open...\n");
	r = fbsplashfuncs_open(SPL_SUSPEND);
	printf("fbsplashfuncs_open=%d\n", r);
	if (r) {
		return 1;
	}

	for (i=0; i<=100; i+=10) {
		printf("fbsplashfuncs_progress (%d)...\n", i);
		fbsplashfuncs_progress(i);
		sleep(1);
		if (i == 50) {
			printf("fbsplashfuncs_dialog()...\n");
			printf("fbsplashfuncs_dialog=%c\n", fbsplashfuncs_dialog ("Hello world!\nPlease press a key: "));
		}
#ifdef CONFIG_ENCRYPT
		else if (i==60) {
			char pass[PASS_SIZE];
			printf("fbsplashfuncs_read_password(,0)..\n");
			fbsplashfuncs_read_password(pass, 0);
			printf("fbsplashfuncs_read_password=%s\n", pass);
		}
		else if (i==80) {
			char pass[PASS_SIZE];
			printf("fbsplashfuncs_read_password(,1)..\n");
			fbsplashfuncs_read_password(pass, 1);
			printf("fbsplashfuncs_read_password=%s\n", pass);
		}
#endif
		c = fbsplashfuncs_key_pressed();
		if (c) {
			char buffer[SPLASH_GENERIC_MESSAGE_SIZE];
			sprintf(buffer, "Key %c (%02x) pressed", c, (unsigned char)c);
			fbsplashfuncs_set_caption(buffer);
		}
	}

	printf("fbsplashfuncs_finish...\n");
	fbsplashfuncs_finish();
	return 0;
}
