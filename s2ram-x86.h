/*
 * s2ram-x86.h
 *
 * Copyright 2007 Tim Dijkstra <tim@famdijkstra.org>
 *
 * Distribute under GPLv2.
 */

#define HACKS_LONG_OPTS \
	{\
		"nofbsuspend\0\tdo not suspend the framebuffer (debugging only!).",\
		no_argument,		NULL, 8 \
	},\
    	{\
		"force\0\tforce suspending, even on unknown machines.\n\nThe following options are only available with --force:",\
		 no_argument,		NULL, 1	\
	},\
    	{\
		"vbe_save\0\tsave VBE state before suspending and restore after resume.",\
		no_argument,		NULL, 2	\
	},\
    	{\
		"vbe_post\0\tVBE POST the graphics card after resume.",\
		no_argument,		NULL, 3	\
	},\
    	{\
		"vbe_mode\0\tget VBE mode before suspend and set it after resume.",\
		no_argument,		NULL, 4 \
	},\
	{\
		"radeontool\0\tturn off the backlight on radeons before suspending.",\
		no_argument,		NULL, 5 \
	},\
    	{\
		"pci_save\0\tsave the PCI config space for the VGA card.",\
		no_argument,		NULL, 6 \
	},\
    	{\
		"acpi_sleep\0set the acpi_sleep parameter before suspend\n\t\t\t1=s3_bios, 2=s3_mode, 3=both",\
		required_argument,	NULL, 7 \
	},
