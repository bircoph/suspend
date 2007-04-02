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
		{ "force",	no_argument,		NULL, 1}, \
		{ "vbe_save",	no_argument,		NULL, 2}, \
		{ "vbe_post",	no_argument,		NULL, 3}, \
		{ "vbe_mode",	no_argument,		NULL, 4}, \
		{ "radeontool",	no_argument,		NULL, 5}, \
		{ "pci_save",	no_argument,		NULL, 6}, \
		{ "acpi_sleep",	required_argument,	NULL, 7},


