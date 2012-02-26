/*
 * s2ram-x86.h
 *
 * Copyright 2007 Tim Dijkstra <tim@famdijkstra.org>
 *
 * Distribute under GPLv2.
 */
#define S3_BIOS     0x01        /* machine needs acpi_sleep=s3_bios */
#define S3_MODE     0x02        /* machine needs acpi_sleep=s3_mode */
#define VBE_SAVE    0x04        /* machine needs "vbetool save / restore" */
#define VBE_POST    0x08        /* machine needs "vbetool post" */
#define RADEON_OFF  0x10        /* machine needs "radeontool light off" */
#define UNSURE      0x20        /* unverified entries from acpi-support 0.59 */
#define NOFB        0x40        /* must not use a frame buffer */
#define VBE_MODE    0x80        /* machine needs "vbetool vbemode get / set" */
#define PCI_SAVE   0x100        /* we need to save the VGA PCI registers */

/* Item size */
#define ITEMSZ  1024

char bios_version[ITEMSZ], sys_vendor[ITEMSZ], sys_product[ITEMSZ], sys_version[ITEMSZ];

int flags, force, fb_nosuspend, no_kms_flag;

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
	},\
	{\
		"no_kms\0\tdo not set KMS (Kernel Mode Set).",\
		required_argument,	NULL, 9 \
	},
