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
#include "vbetool/vbetool.c"
#include "vt.h"

static int test_mode, force;
static int active_console;
static void *vbe_buffer;
/* Flags set from whitelist */
static int flags;

#define S3_BIOS     0x01	/* machine needs acpi_sleep=s3_bios */
#define S3_MODE     0x02	/* machine needs acpi_sleep=s3_mode */
#define VBE_SAVE    0x04	/* machine needs "vbetool save / restore" */
#define VBE_NOPOST  0x08	/* machine does not need / may not use "vbetool post" */
#define RADEON_OFF  0x10	/* machine needs "radeontool light off" */
#define UNSURE      0x20	/* unverified entries from acpi-support 0.59 */

#include "whitelist.c"

static void identify_machine(void)
{
	printf("This machine can be identified by:\n");
	printf("    bios_version = \"%s\"\n"
	       "    sys_vendor = \"%s\"\n"
	       "    sys_product = \"%s\"\n"
	       "    sys_version = \"%s\"\n",
	       bios_version, sys_vendor, sys_product, sys_version);
	printf("See /usr/src/linux/Doc*/power/video.txt for details,\n"
	       "then reimplement neccessary steps here and mail patch to\n"
	       "pavel@suse.cz. Good luck!\n");
}

static void machine_known(int i)
{
	printf("Machine matched: %d: sys_vendor='%s' sys_product='%s' "
	       "sys_version='%s' bios_version='%s'\n", i,
	       whitelist[i].sys_vendor, whitelist[i].sys_product,
	       whitelist[i].sys_version, whitelist[i].bios_version);
	printf("Fixes: vbe_save = %d, vbe_nopost = %d, radeontool = %d, acpi_sleep = %d\n",
	       flags & VBE_SAVE, flags & VBE_NOPOST, flags & RADEON_OFF, 
	       flags & (S3_BIOS | S3_MODE));
	if (flags & UNSURE)
		printf("Machine is in the whitelist but perhaps using "
		       "vbetool unnecessarily.\n"
		       "Please try to find minimal options.\n");
	identify_machine();
}

static void set_acpi_video_mode(int mode)
{
	FILE *f = fopen("/proc/sys/kernel/acpi_video_flags", "w");
	if (!f) {
		printf("/proc/sys/kernel/acpi_video_flags does not exist; perhaps you need -mm kernel?\n");
		exit(1);
	}
	fprintf(f, "%d", mode);
	fflush(f);
	fclose(f);
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
 */
void s2ram_prepare(void)
{
	int i;

	dmi_scan();
	i = machine_match();

	if (!force) {
		if (i < 0) {
			printf("Machine is unknown.\n");
			identify_machine();
			exit(127);
		} else {
			flags = whitelist[i].flags;
		}
	}

	/* force && test_mode are caught earlier, so i >= 0 here */
	if (test_mode) {
		machine_known(i);
		exit(0);
	}

	if (flags & (S3_BIOS | S3_MODE))
		set_acpi_video_mode(flags & (S3_BIOS | S3_MODE));
	else
		printf("acpi_sleep parameter out of range (0-3), ignored.\n");

	/* switch to console 1 first, since we might be in X */
	active_console = fgconsole();
	chvt(1);

	if (flags & VBE_SAVE) {
		int size;
		vbetool_init();
		vbe_buffer = __save_state(&size);
	}
	if (flags & RADEON_OFF) {
		map_radeon_cntl_mem();
		radeon_cmd_light("off");
	}
}

/* Actually enter the suspend. May be ran on frozen system. */
void s2ram_do(void)
{
	FILE *f = fopen("/sys/power/state", "w");
	if (!f) {
		printf("/sys/power/state does not exist; what kind of ninja mutant machine is this?\n");
		exit(1);
	}
	fprintf(f, "mem");
	fflush(f);
	fclose(f);
} 

void s2ram_resume(void)
{
	if (vbe_buffer) {
		vbetool_init();
		if (!(flags & VBE_NOPOST))
			do_post();
		restore_state_from(vbe_buffer);
	}

	/* if we switched consoles before suspend, switch back */
	if (active_console > 0)
		chvt(active_console);
}

void usage(void)
{
	printf("Usage: s2ram [-nhi] [-fsra]\n"
	       "\n"
	       "Options:\n"
	       "    -h, --help:       this text.\n"
	       "    -n, --test:       test if the machine is in the database.\n"
	       "                      returns 0 if known and supported\n"
	       "    -i, --identify:   prints a string that identifies the machine.\n"
	       "    -f, --force:      force suspending, even on unknown "
	       "machines.\n"
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
		{ "vbe_nopost",	no_argument,		NULL, 'p'},
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
			flags |= VBE_NOPOST;
			break;
		case 'r':
			flags |= RADEON_OFF;
			break;
		case 'a':
			flags |= (atoi(optarg) & (S3_BIOS | S3_MODE));
			break;
		default:
			usage();
			exit(1);
			break;
		}
	}
	if (flags && !force) {
		printf("acpi_sleep, vbe_save, vbe_nopost and radeontool parameter "
		       "must be used with --force\n\n");
		usage();
	}
	if (force && test_mode) {
		printf("force and test mode do not make sense together.\n\n");
		usage();
	}

	s2ram_prepare();
	s2ram_do();
	s2ram_resume();
	return 0;
}
