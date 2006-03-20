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

#define S2RAM
#include "vbetool/vbetool.h"
#include "vt.h"
#include "s2ram.h"

static int test_mode, force;
static int active_console;
static void *vbe_buffer;
/* Flags set from whitelist */
static int flags;
char bios_version[1024], sys_vendor[1024], sys_product[1024], sys_version[1024];

/* return codes for s2ram_prepare */
#define S2RAM_OK	0
#define S2RAM_FAIL	1
#define S2RAM_NOFB	126
#define S2RAM_UNKNOWN	127

/* flags for the whitelist */
#define S3_BIOS     0x01	/* machine needs acpi_sleep=s3_bios */
#define S3_MODE     0x02	/* machine needs acpi_sleep=s3_mode */
#define VBE_SAVE    0x04	/* machine needs "vbetool save / restore" */
#define VBE_POST    0x08	/* machine does not need / may not use "vbetool post" */
#define RADEON_OFF  0x10	/* machine needs "radeontool light off" */
#define UNSURE      0x20	/* unverified entries from acpi-support 0.59 */
#define NOFB        0x40	/* must not use a frame buffer */

#include "whitelist.c"

static void identify_machine(void)
{
	printf("This machine can be identified by:\n");
	printf("    sys_vendor   = \"%s\"\n"
	       "    sys_product  = \"%s\"\n"
	       "    sys_version  = \"%s\"\n"
	       "    bios_version = \"%s\"\n",
	       sys_vendor, sys_product, sys_version, bios_version);
	printf("See /usr/src/linux/Doc*/power/video.txt for details,\n"
	       "then reimplement neccessary steps here and mail patch to\n"
	       "pavel@suse.cz. Good luck!\n"
	       "\n"
	       "If you report a problem, please include the complete output "
	       "above.\n");
}

static void machine_known(int i)
{
	printf("Machine matched entry %d:\n"
	       "    sys_vendor   = '%s'\n"
	       "    sys_product  = '%s'\n"
	       "    sys_version  = '%s'\n"
	       "    bios_version = '%s'\n", i,
	       whitelist[i].sys_vendor, whitelist[i].sys_product,
	       whitelist[i].sys_version, whitelist[i].bios_version);
	printf("Fixes: 0x%x  %s%s%s%s%s%s\n\n", flags,
	       (flags & VBE_SAVE) ? "VBE_SAVE " : "",
	       (flags & VBE_POST) ? "VBE_POST " : "",
	       (flags & RADEON_OFF) ? "RADEON_OFF " : "",
	       (flags & S3_BIOS) ? "S3_BIOS " : "",
	       (flags & S3_MODE) ? "S3_MODE " : "",
	       (flags & NOFB) ? "NOFB " : "");
	if (flags & UNSURE)
		printf("Machine is in the whitelist but perhaps using "
		       "vbetool unnecessarily.\n"
		       "Please try to find minimal options.\n\n");
	/* in case of a bugreport we might need to find a better match than
	 * the one we already have (additional BIOS version e.g)...
	 */
	identify_machine();
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

/* Code that can only be run on non-frozen system. It does not matter now
 * but will matter once we'll do suspend-to-both.
 * returns 0 if everything went ok.
 */
int s2ram_prepare(void)
{
	int i, ret = S2RAM_OK;

	dmi_scan();
	i = machine_match();

	if (!force) {
		if (i < 0) {
			printf("Machine is unknown.\n");
			identify_machine();
			ret = S2RAM_UNKNOWN;
		} else {
			flags = whitelist[i].flags;
			if ((flags & NOFB) && is_framebuffer()) {
				printf("This machine can only suspend without framebuffer.\n");
				ret = S2RAM_NOFB;
			}
			if (test_mode) {
				machine_known(i);
				return ret;
			}
		}
	}

	/* 0 means: don't touch what was set on kernel commandline */
	if (!ret && (flags & (S3_BIOS | S3_MODE)))
		ret = set_acpi_video_mode(flags & (S3_BIOS | S3_MODE));

	/* beyond here, we should not return unsuccessfully */
	if (ret)
		return ret;

	/* switch to console 1 first, since we might be in X */
	active_console = fgconsole();
	printf ("Switching from vt%d to vt1\n", active_console);
	chvt(1);

	if (flags & VBE_SAVE) {
		int size;
		vbetool_init();
		printf("Calling save_state\n");
		vbe_buffer = __save_state(&size);
	}
	if (flags & RADEON_OFF) {
		map_radeon_cntl_mem();
		printf("Calling radeon_cmd_light\n");
		radeon_cmd_light(0);
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

	/* if we switched consoles before suspend, switch back */
	if (active_console > 0) {
		printf("switching back to vt%d\n", active_console);
		chvt(active_console);
	}
}

#ifndef CONFIG_BOTH
static void usage(void)
{
	printf("Usage: s2ram [-nhi] [-fsra]\n"
	       "\n"
	       "Options:\n"
	       "    -h, --help:       this text.\n"
	       "    -n, --test:       test if the machine is in the database.\n"
	       "                      returns 0 if known and supported\n"
	       "    -i, --identify:   prints a string that identifies the machine.\n"
	       "    -f, --force:      force suspending, even on unknown machines.\n"
	       "\n"
	       "the following options are only available with --force:\n"
	       "    -s, --vbe_save:   save VBE state before suspending and "
				       "restore after resume.\n"
	       "    -p, --vbe_post:   VBE POST the graphics card after resume\n"
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
	int i;
	struct option options[] = {
		{ "test",	no_argument,		NULL, 'n'},
		{ "help",	no_argument,		NULL, 'h'},
		{ "force",	no_argument,		NULL, 'f'},
		{ "vbe_save",	no_argument,		NULL, 's'},
		{ "vbe_post",	no_argument,		NULL, 'p'},
		{ "radeontool",	no_argument,		NULL, 'r'},
		{ "identify",	no_argument,		NULL, 'i'},
		{ "acpi_sleep",	required_argument,	NULL, 'a'},
		{ NULL,		0,			NULL,  0 }
	};

	while ((i = getopt_long(argc, argv, "nhfspria:", options, NULL)) != -1) {
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
			flags |= VBE_SAVE;
			break;
		case 'p':
			flags |= VBE_POST;
			break;
		case 'r':
			flags |= RADEON_OFF;
			break;
		case 'a':
			flags |= (atoi(optarg) & (S3_BIOS | S3_MODE));
			break;
		default:
			usage();
			break;
		}
	}
	if (flags && !force) {
		printf("acpi_sleep, vbe_save, vbe_post and radeontool parameter "
		       "must be used with --force\n\n");
		usage();
	}
	if (force && test_mode) {
		printf("force and test mode do not make sense together.\n\n");
		usage();
	}

	i = s2ram_prepare();
	if (i || test_mode)
		return i;
	i = s2ram_do();
	s2ram_resume();
	return i;
}
#endif
