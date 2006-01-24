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

int test_mode;

static void machine_known(void)
{
	if (test_mode)
		exit(0);
}

static void radeon_backlight_off(void)
{
	map_radeon_cntl_mem();
	radeon_cmd_light("off");
}

static void machine_table(void)
{
	if (!strcmp(sys_vendor, "IBM")) {
		if (!strcmp(sys_version, "ThinkPad X32")) {
			machine_known();
			radeon_backlight_off();
			return;
		}
	}

	printf("Sorry, unknown machine.\n");
	printf("bios_version = \"%s\", sys_vendor = \"%s\", sys_product = \"%s\", sys_version = \"%s\"\n",
	       bios_version, sys_vendor, sys_product, sys_version);
	printf("See /usr/src/linux/Doc*/power/video.txt for details,\n"
	       "then reimplement neccessary steps here and mail patch to\n"
	       "pavel@suse.cz. Good luck!\n");
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
	fprintf(f, "mem");
	fflush(f);
	fclose(f);
} 

int main(int argc, char *argv[])
{
	int i;
	struct option options[] = {
		{ "test",         no_argument,	     NULL, 'n'},
		{ "help",      	  no_argument,       NULL, 'h'},
		{ NULL,           0,                 NULL,  0 }
	};

	while ((i = getopt_long(argc, argv, "nh", options, NULL)) != -1) {
		switch (i) {
		case 'h':
			printf("Usage: s2ram [-nh]\n");
			exit(1);
			break;
		case 'n':
			test_mode = 1;
			break;
		}
	}
	s2ram_prepare();
	s2ram_do();
	return 0;
}
