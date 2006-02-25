#define VENDOR(x) if (!strcmp(sys_vendor, x))
#define VENDOR2(a...) if (!stranycmp(sys_vendor, a, NULL))
#define PRODUCT(x) if (!strcmp(sys_product, x))
#define PRODUCT2(a...) if (!stranycmp(sys_product, a, NULL))
/* Warning: these only looks at substrings! -- for compatibility only! */
#define PRODUCT_(x) if (!strxcmp(sys_product, x))
#define PRODUCT2_(a...) if (!stranyxcmp(sys_product, a, NULL))
#define HALF_KNOWN() do { half_known(__LINE__); vbe_state_save(); return; } while (0)


VENDOR("IBM") {
	if (!strcmp(sys_version, "ThinkPad X32")) {
		machine_known(__LINE__);
		set_acpi_video_mode(3);
		radeon_backlight_off();
		return;
	}
}

VENDOR("Hewlett-Packard") {
	if (!strcmp(sys_version, "HP OmniBook XE3 GF           ")) {
		machine_known(__LINE__);
		vbe_state_save();
		return;
	}
}

VENDOR("ASUSTEK ") {
	PRODUCT("L3000D") {
		machine_known(__LINE__);
		vbe_state_save();
		return;
	}
}

VENDOR("TOSHIBA") {
	PRODUCT2("Libretto L5/TNK", "Libretto L5/TNKW") {
		machine_known(__LINE__);
		return;
	}
}

/* Entries copied from acpi-support_0.52 */
VENDOR("ASUSTeK Computer Inc.") {
	PRODUCT2_("L7000G series Notebook PC", "M6Ne", "W5A")
		HALF_KNOWN();
}

VENDOR2("Acer", "Acer, inc.") {
	PRODUCT2_("Aspire 2000", "TravelMate 290", "TravelMate 660", "Aspire 3000", 
		 "TravelMate 8100")
		HALF_KNOWN();
}

VENDOR("Compaq") {
	PRODUCT_("Armada    E500")		HALF_KNOWN();
}

VENDOR2("Dell Inc.", "Dell Computer Corporation") {
	PRODUCT2_("Inspiron 700m", "Inspiron 1200", "Inspiron 6000",
		 "Inspiron 8100", "Inspiron 8200", "Inspiron 8600",
		 "Inspiron 9300", "Latitude 110L", "Latitude D410",
		 "Latitude D510", "Latitude D600", "Latitude D610",
		 "Latitude D800", "Latitude D810", "Latitude X1",
		 "Latitude X300", "Precision M20")
		HALF_KNOWN();
}

VENDOR("ECS") {
	PRODUCT_("G556 Centrino")
		HALF_KNOWN();
}

VENDOR2("FUJITSU SIEMENS", "FUJITSU") {
	PRODUCT2_("Amilo A7640", "AMILO M", "LifeBook S Series",
		 "LIFEBOOK S6120", "LIFEBOOK P7010")
		HALF_KNOWN();
}

VENDOR("Hewlett-Packard") {
	/* HP Compaq nx6120: POST_VIDEO=false -- something special: should not do do_post(). */

	PRODUCT2_("HP Compaq nc4200", "HP Compaq nc6000", "HP Compaq nx6110",
		 "HP Compaq nc6120", "HP Compaq nc6220", "HP Compaq nc6230",
		 "HP Compaq nx8220", "HP Compaq nc8230", "HP Pavilion dv1000",
		 "HP Pavilion zt3000", "HP Tablet PC Tx1100",
		 "HP Tablet PC TR1105", "Pavilion zd7000")
		HALF_KNOWN();
}

VENDOR("IBM") {
	PRODUCT2_("2681", "2682", "2683", "2892", "2893", "2896", "2898", "2899",
		 "2722", "2723", "2724", "2897", "1829", "1830", "1831", "1832",
		 "1833", "1836", "1840", "1841", "1846", "1847", "1848", "1849",
		 "1850", "1870", "2647", "2648", "475S", "2373", "2374", "2375",
		 "2376", "2378", "2379", "1871", "1872", "1873", "1874", "1875",
		 "1876", "2668", "2669", "2678", "2679", "2686", "2687", "2369",
		 "2370", "2371", "2372", "2382", "2386", "1864", "1865", "2525",
		 "2526", "2527", "2528", "1866", "1867", "1869")
		HALF_KNOWN();

	/* These needs something special: R50e
	   is_product("1834") || is_product("1842") || is_product("2670") ||
	   ACPI_SLEEP=true;
	   SAVE_VIDEO_PCI_STATE=true;
	   SAVE_VBE_STATE=false;

	   should save/restore /sys/bus/pci/devices/ * /config .
	*/

	/* X30 */
	PRODUCT2_("2672", "2673", "2884", "2885", "2890", "2891") {
		half_known(__LINE__);
		vbe_state_save();
		radeon_backlight_off();
		return;
	}
}

VENDOR("SHARP Corporation") {
	PRODUCT_("PC-MM20 Series")	HALF_KNOWN();
}

VENDOR("Samsung Electronics") {
	PRODUCT_("NX05S")		HALF_KNOWN();
}

VENDOR("Sony Corporation") {
	PRODUCT_("PCG-U101")		HALF_KNOWN();
}

VENDOR("TOSHIBA") {
	PRODUCT2_("libretto U100", "P4000", 
		 "PORTEGE A100", "PORTEGE A200", "PORTEGE M200", "PORTEGE R200",
		 "Satellite 1900", 
		 "TECRA A2", "TECRA A5", "TECRA M2")
		HALF_KNOWN();
}
