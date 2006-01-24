/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

#define S2RAM
#include "dmidecode.c"

void ram_suspend(void)
{
	FILE *f = fopen("/sys/power/state", "w");
	fprintf(f, "mem");
	fflush(f);
	fclose(f);
}

void machine_table(void)
{
	if (!strcmp(sys_vendor, "IBM")) {
		if (!strcmp(sys_version, "ThinkPad X32")) {
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
}

int main(int argc, char *argv[])
{
	dmi_scan();
	machine_table();
	return 0;
}
