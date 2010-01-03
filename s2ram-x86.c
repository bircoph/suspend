/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 *	     2007 Stefan, Rafael, Tim, Luca
 * Distribute under GPLv2.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <pci/pci.h>

#include "vbetool/vbetool.h"
#include "vt.h"
#include "s2ram.h"
#include "config_parser.h"
#include "whitelist.h"

/* From dmidecode.c */
void dmi_scan(void);

static void *vbe_buffer;
static unsigned char vga_pci_state[256];
static struct pci_dev vga_dev;
static struct pci_access *pacc;
/* Flags set from whitelist */
static int flags, vbe_mode = -1, dmi_scanned;
static int force;
static int fb_nosuspend;

/* return codes for s2ram_is_supported */
#define S2RAM_OK	0
#define S2RAM_FAIL	1
#define S2RAM_NOFB	126
#define S2RAM_UNKNOWN	127

void identify_machine(void)
{
	if (!dmi_scanned) {
		dmi_scan();
		dmi_scanned = 1;
	}

	printf("This machine can be identified by:\n");
	printf("    sys_vendor   = \"%s\"\n"
	       "    sys_product  = \"%s\"\n"
	       "    sys_version  = \"%s\"\n"
	       "    bios_version = \"%s\"\n",
	       sys_vendor, sys_product, sys_version, bios_version);
	printf("See http://suspend.sf.net/s2ram-support.html for details.\n"
	       "\n"
	       "If you report a problem, please include the complete output above.\n"
	       "If you report success, please make sure you tested it from both X and\n"
	       "the text console and preferably without proprietary drivers.\n");
}

static int set_acpi_video_mode(int mode)
{
	unsigned long acpi_video_flags;
	FILE *f = fopen("/proc/sys/kernel/acpi_video_flags", "r");
	if (!f) {
		printf("/proc/sys/kernel/acpi_video_flags does not exist; you need a kernel >=2.6.16.\n");
		return S2RAM_FAIL;
	}
	/* read the old setting from /proc */
	if (fscanf(f, "%ld", &acpi_video_flags) != 1) {
		printf("/proc/sys/kernel/acpi_video_flags format is invalid\n");
		return S2RAM_FAIL;
	}
	/* rewind() seems not to work on /proc files, so close and reopen it */
	fclose(f);
	f = fopen("/proc/sys/kernel/acpi_video_flags", "w");
	if (!f) {
		printf("cannot open /proc/sys/kernel/acpi_video_flags "
			"for writing (not running as root?)\n");
		return S2RAM_FAIL;
	}
	/* mask out bits 0 and 1 */
	acpi_video_flags = acpi_video_flags & (~0UL - S3_BIOS - S3_MODE);
	fprintf(f, "%ld", acpi_video_flags | mode);
	fflush(f);
	fclose(f);
	return S2RAM_OK;
}

static int match(const char *t, const char *s)
{
	int len = strlen(s);
	/* empty string matches always */
	if (len == 0)
		return 1;

	if (s[len-1] == '*') {
		len--;
		return !strncmp(t, s, len);
	} else {
		return !strcmp(t,s);
	}
}

static int machine_match(void)
{
	if (!dmi_scanned) {
		dmi_scan();
		dmi_scanned = 1;
	}

	int i;
	/* sys_vendor = NULL terminates the whitelist array */
	for (i = 0; whitelist[i].sys_vendor; i++) {
		if (match(sys_vendor,   whitelist[i].sys_vendor)  &&
		    match(sys_product,  whitelist[i].sys_product) &&
		    match(sys_version,  whitelist[i].sys_version) &&
		    match(bios_version, whitelist[i].bios_version)) {
			return i;
		}
	}
	return -1;
}

static void fbcon_state(int state)
{
	DIR *d;
	FILE *f;
	struct dirent *entry;
	char statefile[255];

	if ((d = opendir("/sys/class/graphics")) == NULL)
		return;
	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.')
			continue;
		snprintf(statefile, 255, "/sys/class/graphics/%s/state", entry->d_name);
		if (!access(statefile, W_OK)) {
			printf("fbcon %s state %d\n", entry->d_name, state);
			f = fopen(statefile, "w");
			if (!f) {
				printf("s2ram: cannot write to %s\n", statefile);
				continue;
			}
			fprintf(f, "%d", state);
			fclose(f);
		}
	}
}

static void suspend_fbcon(void)
{
	fbcon_state(1);
}

static void resume_fbcon(void)
{
	fbcon_state(0);
}

int s2ram_check(int id)
{
	int ret = S2RAM_OK;

	if (id < 0) {
		ret = S2RAM_UNKNOWN;
	} else {
		flags = whitelist[id].flags;
		if ((flags & NOFB) && is_framebuffer())
			ret = S2RAM_NOFB;
		if (flags & UNSURE)
			printf("ATTENTION:\nYour machine is in the whitelist "
			       " but the entry has not been confirmed.\n"
			       "Please try to find the best options and "
			       "report them as explained on\n"
			       "http://suspend.sf.net/s2ram-support.html.\n\n");
	}

	return ret;
}

int machine_known(void)
{
	int i = machine_match();
	if (i < 0) {
		printf("Machine unknown\n");
		identify_machine();
		return 1;
	}

	s2ram_check(i);

	printf("Machine matched entry %d:\n"
	       "    sys_vendor   = '%s'\n"
	       "    sys_product  = '%s'\n"
	       "    sys_version  = '%s'\n"
	       "    bios_version = '%s'\n", i,
	       whitelist[i].sys_vendor, whitelist[i].sys_product,
	       whitelist[i].sys_version, whitelist[i].bios_version);
	printf("Fixes: 0x%x  %s%s%s%s%s%s%s%s%s\n", flags,
	       (flags & VBE_SAVE) ? "VBE_SAVE " : "",
	       (flags & VBE_POST) ? "VBE_POST " : "",
	       (flags & VBE_MODE) ? "VBE_MODE " : "",
	       (flags & RADEON_OFF) ? "RADEON_OFF " : "",
	       (flags & S3_BIOS) ? "S3_BIOS " : "",
	       (flags & S3_MODE) ? "S3_MODE " : "",
	       (flags & NOFB) ? "NOFB " : "",
	       (flags & PCI_SAVE) ? "PCI_SAVE " : "",
	       (flags & UNSURE) ? "UNSURE " : "");
	/* in case of a bugreport we might need to find a better match than
	 * the one we already have (additional BIOS version e.g)...
	 */
	identify_machine();
	return (flags & UNSURE);
}

static int find_vga(void)
{
	struct pci_dev *dev;
	unsigned int class;

	pci_scan_bus(pacc);	/* We want to get the list of devices */

	for (dev=pacc->devices; dev; dev=dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT);
		class = pci_read_word(dev, PCI_CLASS_DEVICE);
		if (class == 0x300)
			break;
	}

	if (!dev)
		return 0;

	memcpy(&vga_dev, dev, sizeof(*dev));
	vga_dev.next = NULL;

	return 1;
}

static void save_vga_pci(void)
{
	pci_read_block(&vga_dev, 0, vga_pci_state, 256);
}

static void restore_vga_pci(void)
{
	pci_write_block(&vga_dev, 0, vga_pci_state, 256);
}

/* warning: we have to be on a text console when calling this */
int s2ram_hacks(void)
{
	int ret = 0;

	ret = set_acpi_video_mode(flags & (S3_BIOS | S3_MODE));

	if (ret)
		return ret;

	if (flags & VBE_SAVE) {
		int size;
		vbetool_init();
		printf("Calling save_state\n");
		vbe_buffer = __save_state(&size);
	}
	if (flags & VBE_MODE) {
		vbetool_init();
		printf("Calling get_mode\n");
		vbe_mode = __get_mode();
	}
	if (flags & RADEON_OFF) {
		map_radeon_cntl_mem();
		printf("Calling radeon_cmd_light(0)\n");
		radeon_cmd_light(0);
	}
	if (flags & PCI_SAVE) {
		pacc = pci_alloc();     /* Get the pci_access structure */
		pci_init(pacc);         /* Initialize the PCI library */

		if (find_vga()) {
			printf("saving PCI config of device %02x:%02x.%d\n",
				vga_dev.bus, vga_dev.dev, vga_dev.func);
			save_vga_pci();
		} else
			/* pci_save requested, no VGA device found => abort */
			return 1;
	}
	if (fb_nosuspend)
		printf("ATTENTION: --nofbsuspend is a debugging tool only.\n"
			"\tIf your machine needs this to work, please report "
			"this as a bug.\n");
	else
		suspend_fbcon();

	return 0;
}

int s2ram_is_supported(void)
{
	int ret = 0, id;

	if (flags && !force) {
		printf("The acpi_sleep, vbe_save, vbe_post, radeontool and "
			"pci_save parameters must be used with --force\n\n");
		return EINVAL;
	}

	if (!force) {
		id = machine_match();
		ret = s2ram_check(id);
	} 

	return ret;
}

/* Actually enter the suspend. May be ran on frozen system. */
int s2ram_do(void)
{
	return s2ram_generic_do();
} 

void s2ram_resume(void)
{
	if (flags & PCI_SAVE) {
		printf("restoring PCI config of device %02x:%02x.%d\n",
			vga_dev.bus, vga_dev.dev, vga_dev.func);
		restore_vga_pci();

		pci_cleanup(pacc);
	}
	// FIXME: can we call vbetool_init() multiple times without cleaning up?
	if (flags & VBE_POST) {
		vbetool_init();
		printf("Calling do_post\n");
		do_post();
	}
	if (vbe_buffer) {
		vbetool_init();
		printf("Calling restore_state_from\n");
		restore_state_from(vbe_buffer);
	}
	if (vbe_mode >= 0) {
		vbetool_init();
		printf("Calling set_vbe_mode\n");
		do_set_mode(vbe_mode, 0);
	}
	if (!fb_nosuspend)
		resume_fbcon();
	if (flags & RADEON_OFF) {
		printf("Calling radeon_cmd_light(1)\n");
		radeon_cmd_light(1);
	}
}

void s2ram_add_flag(int opt, const char *opt_arg)
{
	/* The characters are the `deprecated' short options. They will not
	 * clash with the new labels untill we reach quirk 65... */
	switch (opt) {
		case 1:
		case 'f':
			force = 1;
			break;
		case 2:
		case 's':
			flags |= VBE_SAVE;
			break;
		case 3:
		case 'p':
			flags |= VBE_POST;
			break;
		case 4:
		case 'm':
			flags |= VBE_MODE;
			break;
		case 5:
		case 'r':
			flags |= RADEON_OFF;
			break;
		case 6:
		case 'v':
			flags |= PCI_SAVE;
			break;
		case 7:
		case 'a':
			flags |= (atoi(opt_arg) & (S3_BIOS | S3_MODE));
			break;
		case 8:
			fb_nosuspend = 1;
			break;
	}
}
