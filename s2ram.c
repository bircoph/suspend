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

static int test_mode;
static int active_console = 0;
static void *vbe_buffer;

static void machine_known(void)
{
	if (test_mode)
		exit(0);
}

static void half_known(void)
{
	printf("Machine is in the whitelist, but perhaps it is using vbetool,\n");
	printf("unneccessarily. Please try to find minimal options.\n");
	if (test_mode)
		exit(0);
}

static void radeon_backlight_off(void)
{
	/* switch to console 1 first, since we might be in X */
	if (!active_console) {
		active_console = fgconsole();
		chvt(1);
	}
	map_radeon_cntl_mem();
	radeon_cmd_light("off");
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


static void vbe_state_save(void)
{
	int size;
	/* vbe_state_save (and especially restore) should not happen behind
	 * X's back, otherwise bad things might happen. So we switch to console 1.
	 */
	if (!active_console) {
		active_console = fgconsole();
		chvt(1);
	}
	vbetool_init();
	vbe_buffer = __save_state(&size);
}

static void identify_machine(void)
{
	printf("bios_version = \"%s\", sys_vendor = \"%s\", sys_product = \"%s\", sys_version = \"%s\"\n",
	       bios_version, sys_vendor, sys_product, sys_version);
	printf("See /usr/src/linux/Doc*/power/video.txt for details,\n"
	       "then reimplement neccessary steps here and mail patch to\n"
	       "pavel@suse.cz. Good luck!\n");
}

static int is_product(char *s)
{
	return !(strncmp(sys_product, s, strlen(s)));
}

static void machine_table(void)
{
	if (!strcmp(sys_vendor, "IBM")) {
		if (!strcmp(sys_version, "ThinkPad X32")) {
			machine_known();
			set_acpi_video_mode(3);
			radeon_backlight_off();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Hewlett-Packard")) {
		if (!strcmp(sys_version, "HP OmniBook XE3 GF           ")) {
			machine_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "ASUSTEK ")) {
		if (!strcmp(sys_product, "L3000D")) {
			machine_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "TOSHIBA")) {
		if (is_product("Libretto L5/TNK") || is_product("Libretto L5/TNKW")) {
			machine_known();
			return;
		}
	}

	/* Entries copied from acpi-support_0.52 */
	if (!strcmp(sys_vendor, "ASUSTeK Computer Inc.")) {
		if (is_product("L7000G series Notebook PC") || is_product("M6Ne") || is_product("W5A")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Acer") || !strcmp(sys_vendor, "Acer, inc.")) {
		if (is_product("Aspire 2000") || is_product("TravelMate 290") || is_product("TravelMate 660") ||
		    is_product("Aspire 3000") || is_product("TravelMate 8100")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Compaq")) {
		if (is_product("Armada    E500")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Dell Inc.") || !strcmp(sys_vendor, "Dell Computer Corporation")) {
		if (is_product("Inspiron 700m") || is_product("Inspiron 1200") || is_product("Inspiron 6000") ||
		    is_product("Inspiron 8100") || is_product("Inspiron 8200") || is_product("Inspiron 8600") ||
		    is_product("Inspiron 9300") || is_product("Latitude 110L") || is_product("Latitude D410") ||
		    is_product("Latitude D510") || is_product("Latitude D600") || is_product("Latitude D610") ||
		    is_product("Latitude D800") || is_product("Latitude D810") || is_product("Latitude X1") ||
		    is_product("Latitude X300") || is_product("Precision M20")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "ECS")) {
		if (is_product("G556 Centrino")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "FUJITSU SIEMENS") || !strcmp(sys_vendor, "FUJITSU")) {
		if (is_product("Amilo A7640") || is_product("AMILO M") || is_product("LifeBook S Series") ||
		    is_product("LIFEBOOK S6120") || is_product("LIFEBOOK P7010")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Hewlett-Packard")) {
		if (is_product("HP Compaq nc4200") || is_product("HP Compaq nc6000") || is_product("HP Compaq nx6110") ||
                    is_product("HP Compaq nc6120") || is_product("HP Compaq nc6220") || 
                    is_product("HP Compaq nc6230") || is_product("HP Compaq nx8220") || 
                    is_product("HP Compaq nc8230") || is_product("HP Pavilion dv1000") || 
                    is_product("HP Pavilion zt3000") || is_product("HP Tablet PC Tx1100") || 
                    is_product("HP Tablet PC TR1105") || is_product("Pavilion zd7000")) {
	/* HP Compaq nx6120: POST_VIDEO=false -- something special: should not do do_post(). */
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "IBM")) {
		if (is_product("2681") || is_product("2682") || is_product("2683") ||
		    is_product("2892") || is_product("2893") || is_product("2896") ||
		    is_product("2898") || is_product("2899") ||
		    is_product("2722") || is_product("2723") || is_product("2724") || is_product("2897") ||
		    is_product("1829") || is_product("1830") || is_product("1831") || is_product("1832") || is_product("1833") || is_product("1836") || is_product("1840") || is_product("1841") ||
		    is_product("1846") || is_product("1847") || is_product("1848") || is_product("1849") || is_product("1850") || is_product("1870") ||
		    is_product("2647") || is_product("2648") ||
		    is_product("475S") ||
		    is_product("2373") || is_product("2374") || is_product("2375") || is_product("2376") || is_product("2378") || is_product("2379") ||
		    is_product("1871") || is_product("1872") || is_product("1873") || is_product("1874") || is_product("1875") || is_product("1876") ||
		    is_product("2668") || is_product("2669") || is_product("2678") || is_product("2679") || is_product("2686") || is_product("2687") ||
		    is_product("2369") || is_product("2370") || is_product("2371") || is_product("2372") || is_product("2382") || is_product("2386") ||
		    is_product("1864") || is_product("1865") || is_product("2525") || is_product("2526") || is_product("2527") || is_product("2528") ||
		    is_product("1866") || is_product("1867") || is_product("1869")) {
			half_known();
			vbe_state_save();
			return;
		}

		/* These needs something special: R50e
		is_product("1834") || is_product("1842") || is_product("2670") ||
			ACPI_SLEEP=true;
			SAVE_VIDEO_PCI_STATE=true;
			SAVE_VBE_STATE=false;

			should save/restore /sys/bus/pci/devices/ * /config .
		*/

		/* X30 */
		if (is_product("2672") || is_product("2673") || is_product("2884") ||
		    is_product("2885") || is_product("2890") || is_product("2891")) {
			half_known();
			vbe_state_save();
			radeon_backlight_off();
			return;
		}
	}

	if (!strcmp(sys_vendor, "SHARP Corporation")) {
		if (is_product("PC-MM20 Series")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Samsung Electronics")) {
		if (is_product("NX05S")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Sony Corporation")) {
		if (is_product("PCG-U101")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "Sony Corporation")) {
		if (is_product("PCG-U101")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	if (!strcmp(sys_vendor, "TOSHIBA")) {
		if (is_product("libretto U100") || is_product("P4000") || is_product("PORTEGE A100") ||
                    is_product("PORTEGE A200") || is_product("PORTEGE M200") || 
                    is_product("PORTEGE R200") || is_product("Satellite 1900") || 
                    is_product("TECRA A2") || is_product("TECRA A5") || 
                    is_product("TECRA M2")) {
			half_known();
			vbe_state_save();
			return;
		}
	}

	printf("Sorry, unknown machine.\n");
	identify_machine();
	exit(1);
}

/* Code that can only be run on non-frozen system. It does not matter now
 * but will matter once we'll do suspend-to-both.
 */
void s2ram_prepare(void)
{
	dmi_scan();
	machine_table();
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
	int i, force = 0, vbe_save = 0, radeontool = 0, acpi_sleep = -1;
	struct option options[] = {
		{ "test",	no_argument,		NULL, 'n'},
		{ "help",	no_argument,		NULL, 'h'},
		{ "force",	no_argument,		NULL, 'f'},
		{ "vbe_save",	no_argument,		NULL, 's'},
		{ "radeontool",	no_argument,		NULL, 'r'},
		{ "identify",	no_argument,		NULL, 'i'},
		{ "acpi_sleep",	required_argument,	NULL, 'a'},
		{ NULL,		0,			NULL,  0 }
	};

	while ((i = getopt_long(argc, argv, "nhfsria:", options, NULL)) != -1) {
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
			vbe_save = 1;
			break;
		case 'r':
			radeontool = 1;
			break;
		case 'a':
			acpi_sleep = atoi(optarg);
			break;
		default:
			usage();
			exit(1);
			break;
		}
	}
	if (((acpi_sleep > -1) || vbe_save || radeontool) && !force) {
		printf("acpi_sleep, vbe_save and radeontool parameter "
		       "must be used with --force\n\n");
		usage();
	}
	if (force && test_mode) {
		printf("force and test mode do not make sense together.\n\n");
		usage();
	}

	if (force) {
		if (acpi_sleep > -1) {
			if (acpi_sleep < 4)
				set_acpi_video_mode(acpi_sleep);
			else
				printf("acpi_sleep parameter out of rande (0-3), ignored.\n");
		}
		if (vbe_save)
			vbe_state_save();
		if (radeontool)
			radeon_backlight_off();
	} else {
		s2ram_prepare();
	}
	s2ram_do();
	s2ram_resume();
	return 0;
}
