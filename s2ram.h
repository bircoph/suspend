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
int s2ram_check(void);
int s2ram_hacks(void);
int s2ram_prepare(void);
int s2ram_do(void);
void s2ram_resume(void);



