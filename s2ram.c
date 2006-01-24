/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

#define S2RAM
#include "dmidecode.c"
#include <getopt.h>

void ram_suspend(void)
{
	FILE *f = fopen("/sys/power/state", "w");
	fprintf(f, "mem");
	fflush(f);
	fclose(f);
}

int test_mode;

void machine_known(void)
{
	if (test_mode)
		exit(0);
}

void machine_table(void)
{
	if (!strcmp(sys_vendor, "IBM")) {
		if (!strcmp(sys_version, "ThinkPad X32")) {
			machine_known();
			ram_suspend();
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

	dmi_scan();
	machine_table();
	return 0;
}
