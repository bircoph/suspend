/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

/* from radeontool.c */
void radeon_cmd_light(int);
void map_radeon_cntl_mem(void);

/* from dmidecode.c */
void dmi_scan(void);

/* from s2ram.c */
int s2ram_check(int id);
int s2ram_hacks(void);
int s2ram_is_supported(void);
int s2ram_do(void);
void s2ram_resume(void);
void s2ram_add_flag(int opt, const char *arg);

#define HACKS_LONG_OPTS \
    {	{ "force",	no_argument,		NULL, 1}, \
	"\tforce suspending, even on unknown machines.\n\nThe following options are only available with --force:" }, \
    {	{ "vbe_save",	no_argument,		NULL, 2}, \
	"\tsave VBE state before suspending and restore after resume."}, \
    {	{ "vbe_post",	no_argument,		NULL, 3}, \
	"\tVBE POST the graphics card after resume."}, \
    {	{ "vbe_mode",	no_argument,		NULL, 4}, \
	"\tget VBE mode before suspend and set it after resume."}, \
    {	{ "radeontool",	no_argument,		NULL, 5}, \
	"\tturn off the backlight on radeons before suspending."}, \
    {	{ "pci_save",	no_argument,		NULL, 6}, \
	"\tsave the PCI config space for the VGA card."}, \
    {	{ "acpi_sleep",	required_argument,	NULL, 7}, \
	"set the acpi_sleep parameter before suspend\n\t\t\t1=s3_bios, 2=s3_mode, 3=both" }

