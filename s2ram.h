/*
 * Suspend-to-RAM
 *
 * Copyright 2006 Pavel Machek <pavel@suse.cz>
 * Distribute under GPLv2.
 */

/* from radeontool.c */
void radeon_cmd_light(int);
void map_radeon_cntl_mem(void);

/* from s2ram.c */
/* return codes for s2ram_is_supported */
#define S2RAM_OK        0
#define S2RAM_FAIL      1
#define S2RAM_NOFB      126
#define S2RAM_UNKNOWN   127

int s2ram_hacks(void);
int s2ram_is_supported(void);
void identify_machine(void);
void machine_known(void);
int s2ram_do(void);
void s2ram_resume(void);
void s2ram_add_flag(int opt, const char *arg);

