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
