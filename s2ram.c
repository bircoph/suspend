/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#ifndef S2RAM
#define S2RAM
#endif
#include "vt.h"
#include "s2ram.h"
#include "config_parser.h"


/* Actually enter the suspend. May be ran on frozen system. */
int s2ram_generic_do(void)
{
	int ret = 0;
	FILE *f = fopen("/sys/power/state", "w");
	if (!f) {
		printf("/sys/power/state does not exist; what kind of ninja mutant machine is this?\n");
		return ENODEV;
	}
	if (fprintf(f, "mem") < 0) {
		ret = errno;
		perror("s2ram_do");
	}
	/* usually only fclose fails, not fprintf, so it does not matter
	 * that we might overwrite the previous error.
	 */
	if (fclose(f) < 0) {
		ret = errno;
		perror("s2ram_do");
	}
	return ret;
} 

#ifndef CONFIG_BOTH
int main(int argc, char *argv[])
{
	int i, ret = 0;
	int active_console = -1;
	struct option options[] = {
	    	{ 
		    "help\0\tthis text.",	
		    no_argument,    NULL,   'h'
		},
	    	{ 
		    "test\0\ttest if the machine is in the database.", 
		    no_argument,    NULL,   'n'
		},
	    	{ 
		    "identify\0prints a string that identifies the machine.",
		    no_argument,    NULL,   'i'
		},
		HACKS_LONG_OPTS
	    	{   NULL,   0,	    NULL,   0	}
	};
	const char *optstring = "hni" "fspmrva:";

	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage("s2ram", options, optstring);
			exit(0);
		case 'i':
			identify_machine();
			exit(0);
		case 'n':
			ret = machine_known();
			exit(ret);
		case '?':
			usage("s2ram", options, optstring);
			exit(1);
			break;
		default:
			s2ram_add_flag(i,optarg);
			break;
		}
	}

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

	/* switch to console 1 first, since we might be in X */
	active_console = fgconsole();
	printf("Switching from vt%d to vt1\n", active_console);
	chvt(1);

	ret = s2ram_hacks();
	if (ret)
		goto out;
	ret = s2ram_do();
	s2ram_resume();

 out:
	/* if we switched consoles before suspend, switch back */
	if (active_console > 0) {
		printf("switching back to vt%d\n", active_console);
		chvt(active_console);
	}

	return ret;
}
#endif
