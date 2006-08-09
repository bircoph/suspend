/* whitelist.c
 * whitelist of machines that are known to work somehow
 * and all the workarounds
 */

struct machine_entry
{
	const char *sys_vendor;
	const char *sys_product;
	const char *sys_version;
	const char *bios_version;
	unsigned int flags;
};

struct machine_entry whitelist[] = {
	{ "IBM",			"",		"ThinkPad X32",	"", RADEON_OFF|S3_BIOS|S3_MODE },
	{ "Hewlett Packard",	"",	"HP OmniBook XE3 GF           ","", VBE_POST|VBE_SAVE },
	/* Alexander Wirt */
	{ "Apple Computer, Inc.",	"MacBook1,1",		"",	"", VBE_SAVE },
	{ "Acer            ",		"Extensa 4150 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer           ",		"TravelMate 240 ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer           ",		"TravelMate C300",	"",	"", VBE_SAVE },
	/* TravelMate 630Lci	Tim Dijkstra <tim@famdijkstra.org> */
	{ "Acer           ",		"TravelMate 630 ",	"",	"", S3_BIOS|S3_MODE },
	/* Norbert Preining */
	{ "Acer",			"TravelMate 650",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer, inc.",			"TravelMate 3000 ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer            ",		"TravelMate 3220 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer            ",		"TravelMate 4650 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer, inc.",			"Aspire 1690     ",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	{ "Acer, inc.",			"Ferrari 4000    ",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Cristian Rigamonti */
	{ "ASUSTEK ",			"A2H/L ",		"",	"", 0 },
	{ "ASUSTEK ",			"L2000D",		"",	"", S3_MODE },
	{ "ASUSTEK ",			"L3000D",		"",	"", VBE_POST|VBE_SAVE },
	{ "ASUSTeK Computer Inc.        ",	"M6N       ",	"",	"", S3_MODE },
	{ "ASUSTeK Computer Inc.        ",	"M6Ne      ",	"",	"", S3_MODE },
	/* Charles de Miramon */
	{ "ASUSTeK Computer Inc.        ",	"M5N       ",	"",	"", S3_BIOS|S3_MODE },
	/* M6VA, seraphim@glockenbach.net */
	{ "ASUSTeK Computer Inc.        ",	"M6VA      ",	"",	"", S3_BIOS|S3_MODE },
	/* M7N, Xavier Douville <s2ram-sourceforge@douville.org> */
	{ "ASUSTeK Computer Inc.        ",	"M7A       ",	"",	"", S3_BIOS|S3_MODE },
	/* ASUS V6V, Johannes Engel <j-engel@gmx.de> */
	{ "ASUSTeK Computer INC.",	"V6V",			"",	"", S3_MODE },
	/* ASUS M2400N, Daniel Gollub */
	{ "ERGOUK                       ",	"M2N       ",	"",	"", S3_BIOS|S3_MODE },
	/* Mark Stillwell */
	{ "AVERATEC",			"3700 Series",		"",	"", S3_BIOS|S3_MODE },
	{ "Compaq",			"Armada    E500  *",	"",	"", 0 },
	{ "Compaq",			"N620c *",		"",	"", S3_BIOS|S3_MODE },
	/* VBE_POST|VBE_SAVE works on text console, but not under X. Tested by Chirag Rajyaguru */
	{ "Dell Computer Corporation",  "Inspiron 5150*",	"",	"", VBE_SAVE },
	{ "Dell Computer Corporation",  "Inspiron 8000 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",  "Latitude C600 *",	"",	"", RADEON_OFF },
	{ "Dell Inc.",			"Latitude D410 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",  "Latitude D600 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	{ "Dell Inc.",			"Latitude D610 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	{ "Dell Computer Corporation",	"Latitude D800 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Dell e1505, Alexander Antoniades */
	{ "Dell Inc.",			"MM061 *",		"",	"", 0 },
	/* Dell inspiron 1300, Tim Dijkstra <tim@famdijkstra.org> */
	{ "Dell Inc.",			"ME051 *",		"",	"", 0 },
	/* Dell Inspiron 630m, Fredrik Edemar */
	{ "Dell Inc.",			"MXC051 *",		"",	"", 0 },
	/* Andi Kleen, reported to work in 64bit mode */
	{ "FUJITSU",			"LifeBook S2110",	"",	"", S3_BIOS },
	{ "FUJITSU SIEMENS",		"Amilo A7640 ",		"",	"", VBE_POST|VBE_SAVE|S3_BIOS },
	{ "FUJITSU SIEMENS",		"Stylistic ST5000",	"",	"", S3_BIOS|S3_MODE },
	/* This is a desktop with onboard i810 video */
	{ "FUJITSU SIEMENS",		"SCENIC W300/W600",	"",	"", VBE_POST|VBE_SAVE },
	{ "Hewlett-Packard ",		"Compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard*",		"hp compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard",		"HP Compaq nc6000 *",	"",	"68BDD*", S3_BIOS|S3_MODE },
	{ "Hewlett-Packard",		"HP Compaq nx6125 *",	"",	"", VBE_SAVE|NOFB },
	{ "Hewlett-Packard",		"HP Compaq nc6230 *",	"",	"", VBE_SAVE|NOFB },
	{ "Hewlett-Packard",		"HP Compaq nx8220 *",	"",	"", VBE_SAVE|NOFB },
	{ "Hewlett-Packard",		"Presario R4100 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Hewlett-Packard ",		"Pavilion zt3300 *",	"",	"", S3_BIOS },
	/* R51 and T43 confirmed by Christian Zoz */
	{ "IBM",			"1829*",	"ThinkPad R51",	"", 0 },
	/* R52, reported by Joscha Arenz */
	{ "IBM",			"1860*",		"",	"", S3_BIOS|S3_MODE },
	/* T30 */
	{ "IBM",			"2366*",		"",	"", RADEON_OFF },
	/* X22, confirmed by Richard Neill */
	{ "IBM",			"2662*",		"",	"", S3_BIOS|S3_MODE|RADEON_OFF },
	/* X31, confirmed by Bjoern Jacke */
	{ "IBM",			"2672*",		"",	"", S3_BIOS|S3_MODE|RADEON_OFF },
	/* X40 confirmed by Christian Deckelmann */
	{ "IBM",			"2371*",	"ThinkPad X40",	"", S3_BIOS|S3_MODE },
	/* T42p confirmed by Joe Shaw, T41p by Christoph Thiel (both 2373) */
	{ "IBM",			"2373*",		"",	"", S3_BIOS|S3_MODE },
	/* T41p, Stefan Gerber */
	{ "IBM",			"2374*",		"",	"", S3_BIOS|S3_MODE },
	{ "IBM",			"2668*",	"ThinkPad T43",	"", S3_BIOS|S3_MODE },
	/* G40 confirmed by David H"ademan */
	{ "IBM",			"2388*",		"",	"", VBE_SAVE },
	/* R32 */
	{ "IBM",			"2658*",		"",	"", 0 },
	/* R40 */
	{ "IBM",			"2681*",		"",	"", 0 },
	{ "IBM",			"2722*",		"",	"", 0 },
	/* Z60m, reported by Arkadiusz Miskiewicz */
	{ "IBM",			"2529*",		"",	"", S3_BIOS|S3_MODE },
	/* A21m, Raymund Will */
	{ "IBM",			"2628*",		"",	"", 0 },
	/* X60 / X60s */
	{ "LENOVO",			"1702*",		"",	"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"1704*",		"",	"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"1706*",		"",	"", S3_BIOS|S3_MODE },
	/* At least 1709 series thinkpads can be whitelisted by name,
	   and we can probably delete entries above, too.... */
	{ "LENOVO",			"",		"ThinkPad X60",	"", S3_BIOS|S3_MODE },
	/* T60p */
	{ "LENOVO",			"2007*",		"",	"", S3_BIOS|S3_MODE },

	{ "LG Electronics",		"M1-3DGBG",		"",	"", S3_BIOS|S3_MODE },
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-51E*",	"",	"", VBE_POST|VBE_SAVE },
	/* Hugo Costelha */
	{ "NEC *",			"B7 *",			"",	"", VBE_SAVE }, 
	{ "TOSHIBA",			"Libretto L5/TNK",	"",	"", 0 },
	{ "TOSHIBA",			"Libretto L5/TNKW",	"",	"", 0 },
	/* this is a Toshiba Satellite 4080XCDT, believe it or not :-( */
	{ "TOSHIBA",		"Portable PC",	"Version 1.0",	"Version 7.80", S3_MODE },
	{ "TOSHIBA",			"Satellite A30",	"",	"", VBE_SAVE },
	{ "TOSHIBA",			"Satellite L10",	"",	"", VBE_POST|VBE_SAVE },
	/* Michaell Gurski */
	{ "TOSHIBA",			"Satellite M35X",	"",	"", S3_BIOS|S3_MODE },
	{ "TOSHIBA",			"TECRA S3",		"",	"", 0 },
	{ "Samsung",			"SQ10",			"",	"", VBE_POST|VBE_SAVE },
	{ "Samsung Electronics",	"SX20S",		"",	"", S3_BIOS|S3_MODE },
	{ "SHARP                           ",	"PC-AR10 *",	"",	"", 0 },
	{ "Sony Corporation",		"VGN-FS115B",		"",	"", S3_BIOS|S3_MODE },
	{ "Sony Corporation",		"PCG-GRT995MP*",	"",	"", 0 },
	/* VIA EPIA M Mini-ITX Motherboard with onboard gfx, reported by Monica Schilling */
	{ "VIA Technologies, Inc.",	"VT8623-8235",		"",	"", S3_MODE },

	// entries below are imported from acpi-support 0.59 and though "half known".
	{ "ASUSTeK Computer Inc.",	"L7000G series Notebook PC*", "","", VBE_POST|VBE_SAVE|UNSURE },
	{ "ASUSTeK Computer Inc.",	"W5A*",			"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"TravelMate 290*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"TravelMate 660*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"Aspire 2000*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer, inc.",			"TravelMate 8100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer, inc.",			"Aspire 3000*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 700m*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 1200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 6000*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 8100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 8200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 8600*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Inspiron 9300*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Latitude 110L*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Latitude D510*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Latitude D810*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Latitude X1*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Latitude X300*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Inc.",			"Precision M20*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 700m*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 1200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 6000*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 8100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 8200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 8600*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Inspiron 9300*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude 110L*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude D410*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude D510*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude D810*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude X1*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Latitude X300*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Dell Computer Corporation",	"Precision M20*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "ECS",			"G556 Centrino*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU",			"Amilo M*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU",			"LifeBook S Series*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU",			"LIFEBOOK S6120*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU",			"LIFEBOOK P7010*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU SIEMENS",		"Amilo M*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU SIEMENS",		"LifeBook S Series*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU SIEMENS",		"LIFEBOOK S6120*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "FUJITSU SIEMENS",		"LIFEBOOK P7010*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Compaq nc4200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Compaq nx6110*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Compaq nc6120*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Compaq nc6220*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Compaq nc8230*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Pavilion dv1000*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Pavilion zt3000*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Tablet PC Tx1100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"HP Tablet PC TR1105*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Hewlett-Packard",		"Pavilion zd7000*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// R40
	{ "IBM",			"2682*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2683*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2692*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2693*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2696*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2698*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2699*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2723*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2724*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2897*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// R50/p
	{ "IBM",			"1829*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1830*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1831*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1832*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1833*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1836*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1840*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1841*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	/* R50e needs not yet implemented save_video_pci_state :-(
	{ "IBM",			"1834*",		"",	"", UNSURE },
	{ "IBM",			"1842*",		"",	"", UNSURE },
	{ "IBM",			"2670*",		"",	"", UNSURE },
	*/
	// R52
	{ "IBM",			"1846*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1847*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1848*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1849*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1850*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1870*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T21
	{ "IBM",			"2647*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2648*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T23
	{ "IBM",			"475S*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T40/T41/T42/p
	{ "IBM",			"2375*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2376*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2378*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2379*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T43
	{ "IBM",			"1871*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1872*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1873*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1874*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1875*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1876*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T43/p
	{ "IBM",			"2668*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2669*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2678*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2679*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2686*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2687*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// X30
	{ "IBM",			"2673*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2884*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2885*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2890*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2891*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	// X40
	{ "IBM",			"2369*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2370*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2372*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2382*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2386*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// X41
	{ "IBM",			"1864*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1865*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2525*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2526*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2527*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2528*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// X41 Tablet
	{ "IBM",			"1866*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1867*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1869*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },

	{ "Samsung Electronics",	"NX05S*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "SHARP Corporation",		"PC-MM20 Series*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Sony Corporation",		"PCG-U101*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },

	{ "TOSHIBA",			"libretto U100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"P4000*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE A100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE A200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE M200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE R200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"Satellite 1900*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA A2*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA A5*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA M2*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ NULL }
};
