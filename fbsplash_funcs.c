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

#ifdef CONFIG_FBSPLASH
#include <string.h>
#include <stdio.h>
#include <fbsplash.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "swsusp.h"
#include "splash.h"
#include "fbsplash_funcs.h"

extern char fbsplash_theme[];
static struct fbspl_theme *theme;
static int was_silent;
static int have_input;
static int force_verbose;

int fbsplashfuncs_open(int mode)
{
	fbspl_cfg_t *cfg;
	int have_render = 0;
	int ret = 0;

	cfg = fbsplash_lib_init(fbspl_bootup);
	if (!cfg) {
		ret = -1;
		goto cleanup;
	}
	cfg->verbosity = FBSPL_VERB_QUIET;

	fbsplash_parse_kcmdline(false);

	if (strlen(fbsplash_theme))
		fbsplash_acc_theme_set(fbsplash_theme);

	if (fbsplashr_init(false)) {
		ret = -3;
		goto cleanup;
	}
	have_render = 1;

	theme = fbsplashr_theme_load();
	if (!theme) {
		ret = -4;
		goto cleanup;
	}

	was_silent = fbsplash_is_silent();
	if (!was_silent) {
		fbsplash_set_silent();
	}

	fbsplashr_tty_silent_init(true);
	fbsplashr_tty_silent_update();
	fbsplashr_message_set(theme,
		(mode==SPL_RESUME) ? "Resuming..." : "Suspending..." );

	fbsplashr_render_screen(theme, true, false, was_silent ? FBSPL_EFF_NONE : FBSPL_EFF_FADEIN);

	have_input = !fbsplashr_input_init();

cleanup:
	if (ret) {
		if (have_input) {
			fbsplashr_input_cleanup();
			have_input = 0;
		}

		if (have_render) {
			fbsplashr_cleanup();
			have_render = 0;
		}

		if (cfg) {
			fbsplash_lib_cleanup();
			cfg = NULL;
		}
	}

	return ret;
}


int fbsplashfuncs_finish(void)
{
	if (have_input) {
		fbsplashr_input_cleanup();
		have_input = 0;
	}

	if (fbsplash_is_silent()) {
		fbsplashr_render_screen(theme, true, false, was_silent ? FBSPL_EFF_NONE : FBSPL_EFF_FADEOUT);
	}

	fbsplashr_tty_silent_cleanup();

	if (!was_silent && fbsplash_is_silent())
		fbsplash_set_verbose(0);

	fbsplashr_theme_free(theme);
	theme = NULL;

	fbsplashr_cleanup();
	fbsplash_lib_cleanup();

	return 0;
}

int fbsplashfuncs_progress(int p)
{
	static int old = 0;

	if (p < old || p - old >= 10) {
		old = p;

		fbsplashr_progress_set(theme, FBSPL_PROGRESS_MAX/100*p);
		if (fbsplash_is_silent())
			fbsplashr_render_screen(theme, true, false, FBSPL_EFF_NONE);
	}

	return 0;
}

void fbsplashfuncs_read_password(char *buf, int vrfy)
{
#if CONFIG_ENCRYPT
	char *pass;

	if (fbsplash_is_silent())
		fbsplash_set_verbose(0);

	do {
		do {
			pass = getpass("Passphrase please (must be non-empty): ");
		} while (strlen(pass) == 0 || strlen(pass) >= PASS_SIZE);

		strcpy(buf, pass);

		if (vrfy)
			pass = getpass("Verify passphrase: ");
	} while (vrfy && strcmp(buf, pass));

	if (!force_verbose) {
		fbsplash_set_silent();
		fbsplashr_tty_silent_update();
	}
#endif
}

int fbsplashfuncs_dialog(const char *prompt)
{
	int c;

	if (fbsplash_is_silent())
		fbsplash_set_verbose(0);

	printf("%s", prompt);
	c = getchar();
	if (!force_verbose) {
		fbsplash_set_silent();
		fbsplashr_tty_silent_update();
	}
	return c;
}

char fbsplashfuncs_key_pressed(void)
{
	if (!have_input)
		return 0;
	else {
		char c;
		c = (char)fbsplashr_input_getkey(false);

		/*
		 * Well... maybe someday it will work...
		 * But currently we cannot switch to different vt
		 */
		if (c == KEY_V) {
			force_verbose = !force_verbose;
			if (force_verbose)
				fbsplash_set_verbose(0);
			else
				fbsplash_set_silent();
			return 0;
		} else {
			switch (c) {
				case KEY_R:
					c = REBOOT_KEY_CODE;
					break;
				case KEY_BACKSPACE:
					c = ABORT_KEY_CODE;
					break;
				case KEY_ENTER:
					c = ENTER_KEY_CODE;
					break;
				case 0:
					break;
				default:
					c = '?';
					break;
			}

			return c;
		}
	}
}

void fbsplashfuncs_set_caption(const char *message)
{
	fbsplashr_message_set(theme, message);
	if (fbsplash_is_silent())
		fbsplashr_render_screen(theme, true, false, FBSPL_EFF_NONE);
}

#endif
