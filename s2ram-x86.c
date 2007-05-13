/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 *	     2007 Stefan, Rafael, Tim, Luca
 * Distribute under GPLv2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <pci/pci.h>

#include "vbetool/vbetool.h"
#include "vt.h"
#include "s2ram.h"
#include "config.h"

/* From dmidecode.c */
void dmi_scan(void);

static void *vbe_buffer;
static unsigned char vga_pci_state[256];
static struct pci_dev vga_dev;
static struct pci_access *pacc;
/* Flags set from whitelist */
static int flags = 0, vbe_mode = -1, dmi_scanned = 0;
int force;
char bios_version[1024], sys_vendor[1024], sys_product[1024], sys_version[1024];

/* return codes for s2ram_is_supported */
#define S2RAM_OK	0
#define S2RAM_FAIL	1
#define S2RAM_NOFB	126
#define S2RAM_UNKNOWN	127

/* flags for the whitelist */
#define S3_BIOS     0x01	/* machine needs acpi_sleep=s3_bios */
#define S3_MODE     0x02	/* machine needs acpi_sleep=s3_mode */
#define VBE_SAVE    0x04	/* machine needs "vbetool save / restore" */
#define VBE_POST    0x08	/* machine needs "vbetool post" */
#define RADEON_OFF  0x10	/* machine needs "radeontool light off" */
#define UNSURE      0x20	/* unverified entries from acpi-support 0.59 */
#define NOFB        0x40	/* must not use a frame buffer */
#define VBE_MODE    0x80	/* machine needs "vbetool vbemode get / set" */
#define PCI_SAVE   0x100	/* we need to save the VGA PCI registers */

#include "whitelist.c"

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
	printf("See http://en.opensuse.org/S2ram for details.\n"
	       "\n"
	       "If you report a problem, please include the complete output "
	       "above.\n");
}

static int set_acpi_video_mode(int mode)
{
	FILE *f = fopen("/proc/sys/kernel/acpi_video_flags", "w");
	if (!f) {
		printf("/proc/sys/kernel/acpi_video_flags does not exist; you need a kernel >=2.6.16.\n");
		return S2RAM_FAIL;
	}
	fprintf(f, "%d", mode);
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

void machine_known(void)
{
	int i = machine_match();
	if (i < 0) {
		printf("Machine unknown\n");
		identify_machine();
	}

	printf("Machine matched entry %d:\n"
	       "    sys_vendor   = '%s'\n"
	       "    sys_product  = '%s'\n"
	       "    sys_version  = '%s'\n"
	       "    bios_version = '%s'\n", i,
	       whitelist[i].sys_vendor, whitelist[i].sys_product,
	       whitelist[i].sys_version, whitelist[i].bios_version);
	printf("Fixes: 0x%x  %s%s%s%s%s%s%s%s\n", flags,
	       (flags & VBE_SAVE) ? "VBE_SAVE " : "",
	       (flags & VBE_POST) ? "VBE_POST " : "",
	       (flags & VBE_MODE) ? "VBE_MODE " : "",
	       (flags & RADEON_OFF) ? "RADEON_OFF " : "",
	       (flags & S3_BIOS) ? "S3_BIOS " : "",
	       (flags & S3_MODE) ? "S3_MODE " : "",
	       (flags & NOFB) ? "NOFB " : "",
	       (flags & PCI_SAVE) ? "PCI_SAVE " : "");
	if (flags & UNSURE)
		printf("Machine is in the whitelist but perhaps using "
		       "vbetool unnecessarily.\n"
		       "Please try to find minimal options.\n\n");
	/* in case of a bugreport we might need to find a better match than
	 * the one we already have (additional BIOS version e.g)...
	 */
	identify_machine();
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
	}

	return ret;
}

int find_vga(void)
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

void save_vga_pci(void)
{
	pci_read_block(&vga_dev, 0, vga_pci_state, 256);
}

void restore_vga_pci(void)
{
	pci_write_block(&vga_dev, 0, vga_pci_state, 256);
}

/* warning: we have to be on a text console when calling this */
int s2ram_hacks(void)
{
	int ret = 0;

	/* 0 means: don't touch what was set on kernel commandline */
	if (flags & (S3_BIOS | S3_MODE))
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
			flags |= (atoi(optarg) & (S3_BIOS | S3_MODE));
			break;

	}
}
