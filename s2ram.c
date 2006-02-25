/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

#include <getopt.h>

#define S2RAM
#include "dmidecode.c"
#include "radeontool.c"
#include "vbetool/vbetool.c"
#include "vt.h"
#include <stdarg.h>

static int test_mode;
static int active_console = 0;
static void *vbe_buffer;

static void machine_known(int line)
{
	if (test_mode) {
		printf("Machine known: whitelist.c: %d\n", line);
		exit(0);
	}
}

static void half_known(int line)
{
	printf("Machine is in the whitelist, but perhaps it is using vbetool,\n");
	printf("unneccessarily. Please try to find minimal options.\n");
	if (test_mode) {
		printf("Machine half known: whitelist.c: %d\n", line);
		exit(0);
	}
}

static void radeon_backlight_off(void)
{
	/* switch to console 1 first, since we might be in X */
	if (!active_console) {
		active_console = fgconsole();
		chvt(1);
	}
	map_radeon_cntl_mem();
	radeon_cmd_light("off");
}

static void set_acpi_video_mode(int mode)
{
	FILE *f = fopen("/proc/sys/kernel/acpi_video_flags", "w");
	if (!f) {
		printf("/proc/sys/kernel/acpi_video_flags does not exist; perhaps you need -mm kernel?\n");
		exit(1);
	}
	fprintf(f, "%d", mode);
	fflush(f);
	fclose(f);
}


static void vbe_state_save(void)
{
	int size;
	/* vbe_state_save (and especially restore) should not happen behind
	 * X's back, otherwise bad things might happen. So we switch to console 1.
	 */
	if (!active_console) {
		active_console = fgconsole();
		chvt(1);
	}
	vbetool_init();
	vbe_buffer = __save_state(&size);
}

static void identify_machine(void)
{
	printf("bios_version = \"%s\", sys_vendor = \"%s\", sys_product = \"%s\", sys_version = \"%s\"\n",
	       bios_version, sys_vendor, sys_product, sys_version);
	printf("See /usr/src/linux/Doc*/power/video.txt for details,\n"
	       "then reimplement neccessary steps here and mail patch to\n"
	       "pavel@suse.cz. Good luck!\n");
}

static int
stranycmp(const char *s, ...)
{
	char *t;
	va_list args;
	va_start(args, s);
	while ((t=va_arg(args, char *))) {
		if (!strcmp(s, t)) {
			va_end(args);
			return 0;
		}
	}
	va_end(args);
	return 1;
} 

static int strxcmp(const char *t, const char *s)
{
	return strncmp(t, s, strlen(s));
}

static int
stranyxcmp(const char *s, ...)
{
	char *t;
	va_list args;
	va_start(args, s);
	while ((t=va_arg(args, char *))) {
		if (!strxcmp(s, t)) {
			va_end(args);
			return 0;
		}
	}
	va_end(args);
	return 1;
} 

static void machine_table(void)
{
#include "whitelist.c"
	printf("Sorry, unknown machine.\n");
	identify_machine();
	exit(1);
}

/* Code that can only be run on non-frozen system. It does not matter now
 * but will matter once we'll do suspend-to-both.
 */
void s2ram_prepare(void)
{
	dmi_scan();
	machine_table();
}

/* Actually enter the suspend. May be ran on frozen system. */
void s2ram_do(void)
{
	FILE *f = fopen("/sys/power/state", "w");
	if (!f) {
		printf("/sys/power/state does not exist; what kind of ninja mutant machine is this?\n");
		exit(1);
	}
	fprintf(f, "mem");
	fflush(f);
	fclose(f);
} 

void s2ram_resume(void)
{
	if (vbe_buffer) {
		do_post();
		restore_state_from(vbe_buffer);
	}

	/* if we switched consoles before suspend, switch back */
	if (active_console > 0)
		chvt(active_console);
}

void usage(void)
{
	printf("Usage: s2ram [-nhi] [-fsra]\n"
	       "\n"
	       "Options:\n"
	       "    -h, --help:       this text.\n"
	       "    -n, --test:       test if the machine is in the database.\n"
	       "                      returns 0 if known and supported\n"
	       "    -i, --identify:   prints a string that identifies the machine.\n"
	       "    -f, --force:      force suspending, even on unknown "
	       "machines.\n"
	       "\n"
	       "the following options are only available with --force:\n"
	       "    -s, --vbe_save:   save VBE state before suspending and "
	       "restore after resume.\n"
	       "    -r, --radeontool: turn off the backlight on radeons "
	       "before suspending.\n"
	       "    -a, --acpi_sleep: set the acpi_sleep parameter before "
	       "suspend\n"
	       "                      1=s3_bios, 2=s3_mode, 3=both\n"
	       "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int i, force = 0, vbe_save = 0, radeontool = 0, acpi_sleep = -1;
	struct option options[] = {
		{ "test",	no_argument,		NULL, 'n'},
		{ "help",	no_argument,		NULL, 'h'},
		{ "force",	no_argument,		NULL, 'f'},
		{ "vbe_save",	no_argument,		NULL, 's'},
		{ "radeontool",	no_argument,		NULL, 'r'},
		{ "identify",	no_argument,		NULL, 'i'},
		{ "acpi_sleep",	required_argument,	NULL, 'a'},
		{ NULL,		0,			NULL,  0 }
	};

	while ((i = getopt_long(argc, argv, "nhfsria:", options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage();
			break;
		case 'i':
			dmi_scan();
			identify_machine();
			exit(0);
		case 'n':
			test_mode = 1;
			break;
		case 'f':
			force = 1;
			break;
		case 's':
			vbe_save = 1;
			break;
		case 'r':
			radeontool = 1;
			break;
		case 'a':
			acpi_sleep = atoi(optarg);
			break;
		default:
			usage();
			exit(1);
			break;
		}
	}
	if (((acpi_sleep > -1) || vbe_save || radeontool) && !force) {
		printf("acpi_sleep, vbe_save and radeontool parameter "
		       "must be used with --force\n\n");
		usage();
	}
	if (force && test_mode) {
		printf("force and test mode do not make sense together.\n\n");
		usage();
	}

	if (force) {
		if (acpi_sleep > -1) {
			if (acpi_sleep < 4)
				set_acpi_video_mode(acpi_sleep);
			else
				printf("acpi_sleep parameter out of rande (0-3), ignored.\n");
		}
		if (vbe_save)
			vbe_state_save();
		if (radeontool)
			radeon_backlight_off();
	} else {
		s2ram_prepare();
	}
	s2ram_do();
	s2ram_resume();
	return 0;
}
