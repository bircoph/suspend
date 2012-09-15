/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Copyright 2009 Rodolfo Garcia <kix@kix.es>
 * Copyright 2011 Rafael J. Wysocki <rjw@sisk.pl>, Novell Inc.
 * Distribute under GPLv2.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#ifndef S2RAM
#define S2RAM
#endif
#include "vt.h"
#include "whitelist.h"
#include "s2ram.h"
#include "config_parser.h"


int main(int argc, char *argv[])
{
	int i, ret = 0;
	int active_console = -1;
	struct option options[] = {
		{
			"help\0\tthis text.",
			no_argument,	NULL, 'h'
		},
		{
			"version\0\t\t\tversion information",
			no_argument,	NULL, 'V'
		},
		{
			"test\0\ttest if the machine is in the database.",
			no_argument,	NULL, 'n'
		},
		{
			"kmstest\0\ttest if the kernel has KMS support.",
			no_argument,	NULL, 'K'
		},
		{
			"identify\0prints a string that identifies the machine.",
			no_argument,	NULL, 'i'
		},
		HACKS_LONG_OPTS
		{	NULL, 0,	NULL, 0	}
	};
	const char *optstring = "hVnKi" "fspmrva:k";

	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage("s2ram", options, optstring);
			exit(0);
		case 'V':
			version("s2ram", whitelist_version);
			exit(EXIT_SUCCESS);
		case 'i':
			identify_machine();
			exit(0);
		case 'n':
			ret = machine_known();
			exit(ret);
		case 'K':
			ret = s2ram_check_kms();
			if (!ret)
				printf("This kernel has KMS support.\n");
			else
				printf("This kernel doesn't have KMS support.\n");
			exit(ret);
		default:
			s2ram_add_flag(i, optarg);
			break;
		}
	}

	/* Check if the KMS support is disabled by user */
	if (!no_kms_flag) {
		/* KMS */
		ret = s2ram_check_kms();
		if (!ret) {
			printf("KMS graphics driver is in use, skipping quirks.\n");
			return s2ram_generic_do();
		}
	} else {
		printf("KMS disabled by user.\n");
	}

	/* Test if s2ram will use quirks from the configuration file */
	ret = get_s2ram_config();

	if (ret) {
		/* No configuration file, using quirks form database */
		ret = s2ram_is_supported();

		if (ret == S2RAM_UNKNOWN) {
			printf("Machine is unknown.\n");
			identify_machine();
			goto out;
		}

		if (ret == S2RAM_NOFB)
			printf("This machine can only suspend without framebuffer.\n");

		if (ret)
			goto out;
	} else {
		printf("Using quirks from the configuration file.\n");
	}

	/* switch to console 1 first, since we might be in X */
	active_console = fgconsole();
	printf("switching from vt%d to vt1... ", active_console);
	if (chvt(1))
		printf("succeeded\n");
	else
		printf("failed\n");


	ret = s2ram_hacks();
	if (ret)
		goto out;
	ret = s2ram_do();
	s2ram_resume();

 out:
	/* if we switched consoles before suspend, switch back */
	if (active_console > 0) {
		printf("switching back to vt%d... ", active_console);
		if (chvt(active_console))
			printf("succeeded\n");
		else
			printf("failed\n");
	}

	return ret;
}
