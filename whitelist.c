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
	{ "Acer, inc.",			"Aspire 1690     ",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* From: Bontemps Camille <camille.bontemps@gmail.com> */
	{ "Acer",			"Aspire 3610",		"",	"", S3_BIOS|S3_MODE },
	/* From: Saul Mena Avila <saul_2110@yahoo.com.mx> */
	{ "Acer",			"Aspire 3620",		"",	"", S3_BIOS|S3_MODE },
	{ "Acer            ",		"Extensa 4150 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer           ",		"TravelMate 240 ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer           ",		"TravelMate C300",	"",	"", VBE_SAVE },
	/* TravelMate 630Lci	Tim Dijkstra <tim@famdijkstra.org> */
	{ "Acer           ",		"TravelMate 630 ",	"",	"", S3_BIOS|S3_MODE },
	/* Norbert Preining */
	{ "Acer",			"TravelMate 650",	"",	"", VBE_POST|VBE_SAVE },
	/* Martin Sack, this one may be NOFB, to be verified */
	{ "Acer",			"TravelMate 800",	"",	"", VBE_POST },
	{ "Acer, inc.",			"TravelMate 3000 ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer            ",		"TravelMate 3220 *",	"",	"", S3_BIOS|S3_MODE },
	/* From: LucaB <lbonco@tin.it> */
	{ "Acer, inc.",			"TravelMate 4100 ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer            ",		"TravelMate 4650 *",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer, inc.",			"Ferrari 4000    ",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Cristian Rigamonti */
	{ "ASUSTEK ",			"A2H/L ",		"",	"", 0 },
	{ "ASUSTEK ",			"L2000D",		"",	"", S3_MODE },
	{ "ASUSTEK ",			"L3000D",		"",	"", VBE_POST|VBE_SAVE },
	/* Manfred Tremmel, https://bugzilla.novell.com/show_bug.cgi?id=171107 */
	{ "ASUSTeK Computer Inc.        ",	"M6N       ",	"",	"", S3_BIOS|S3_MODE },
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
	/* ASUS a7v600 motherboard, has no usable sys_* entries besides bios_version :-(
	   reported by James Spencer */
	{ "",		"",	"",	"ASUS A7V600 ACPI BIOS Revision *", S3_BIOS|S3_MODE },
	/* Mark Stillwell */
	{ "AVERATEC",			"3700 Series",		"",	"", S3_BIOS|S3_MODE },
	/* Andreas Schmitz */
	{ "BenQ           ",		"Joybook R22",		"",	"", S3_BIOS|S3_MODE },
	{ "Compaq",			"Armada    E500  *",	"",	"", 0 },
	/* Chris AtLee <chris@atlee.ca>, VBE_MODE does not work, text size changes. */
	{ "Compaq ",			"Evo N800w *",		"",	"", VBE_POST|VBE_SAVE },
	{ "Compaq",			"N620c *",		"",	"", S3_BIOS|S3_MODE },
	/* From bug 1544913@sf.net, Andrei - amaces */
	{ "Dell Computer Corporation",  "Inspiron 1150*",       "",     "", 0 },
	/* Dell Inspiron 500m, Per Øyvind Karlsen <pkarlsen@mandriva.com> */
	{ "Dell Computer Corporation",	"Inspiron 500m*",	"",	"", S3_BIOS },
	/* Dell Inspiron 510m, Jose Carlos Garcia Sogo <jsogo@debian.org> */
	{ "Dell Inc.",			"Inspiron 510m *",	"",	"", VBE_POST },
	/* Eric Sandall <eric@sandall.us> */
	{ "Dell Computer Corporation",  "Inspiron 5100 *",	"",	"", NOFB },
	/* VBE_POST|VBE_SAVE works on text console, but not under X. Tested by Chirag Rajyaguru */
	{ "Dell Computer Corporation",  "Inspiron 5150*",	"",	"", VBE_SAVE },
	{ "Dell Computer Corporation",  "Inspiron 8000 *",	"",	"", VBE_POST|VBE_SAVE },
	/* by Henare Degan <henare.degan@gmail.com> 8500 w. NVidia card. There are also 8500s w. ATI cards */
	{ "Dell Computer Corporation",  "Inspiron 8500 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",  "Latitude C600 *",	"",	"", RADEON_OFF },
	{ "Dell Computer Corporation",  "Latitude C610 *",	"",	"", VBE_POST|VBE_MODE },
	{ "Dell Inc.",			"Latitude D410 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Ian Samule, https://bugzilla.novell.com/show_bug.cgi?id=175568 */
	{ "Dell Inc.",			"Latitude D505 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",  "Latitude D600 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	{ "Dell Inc.",			"Latitude D610 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* D620 reported by Miroslav Ruda <ruda@ics.muni.cz> */
	{ "Dell Inc.",			"Latitude D620 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",	"Latitude D800 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Dell e1505, Alexander Antoniades */
	{ "Dell Inc.",			"MM061 *",		"",	"", 0 },
	/* Dell inspiron 1300, Tim Dijkstra <tim@famdijkstra.org> */
	{ "Dell Inc.",			"ME051 *",		"",	"", 0 },
	/* Dell Inspiron 630m, Fredrik Edemar */
	{ "Dell Inc.",			"MXC051 *",		"",	"", 0 },
	/* Dell Inspiron 640m, Daniel Drake <dsd@gentoo.org> */
	{ "Dell Inc.",			"MXC061 *",		"",	"", VBE_POST },
	/* Per Øyvind Karlsen <pkarlsen@mandriva.com> VBE_POST will get console working, but break X */
	{ "Dell Computer Corporation",	"Precision M60*",	"",	"", VBE_SAVE },
	/* Andi Kleen, reported to work in 64bit mode */
	{ "FUJITSU",			"LifeBook S2110",	"",	"", S3_BIOS },
	/* Those two x86_64 amilos are a bit strange. I seldom see s3_bios + vbe_* ... */
	{ "FUJITSU SIEMENS",		"Amilo A7640 ",		"",	"", VBE_POST|VBE_SAVE|S3_BIOS },
	/* reported by Thomas Halva Labella <hlabella@ulb.ac.be> */
	{ "FUJITSU SIEMENS",		"Amilo A7645 ",		"",	"", VBE_SAVE|S3_BIOS|S3_MODE },
	/* <jan.lockenvitz.ext@siemens.com>, tested with X only */
	{ "FUJITSU SIEMENS",		"LIFEBOOK S7010",	"",	"", 0 },
	/* Eckhart Woerner <ew@ewsoftware.de> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK T4010",	"",	"", S3_BIOS|S3_MODE },
	{ "FUJITSU SIEMENS",		"Stylistic ST5000",	"",	"", S3_BIOS|S3_MODE },
	/* This is a desktop with onboard i810 video */
	{ "FUJITSU SIEMENS",		"SCENIC W300/W600",	"",	"", VBE_POST|VBE_SAVE },
	/* From: Einon <einon@animehq.hu>, seems to work only from X :-( */
	{ "Gericom",			"Montara-GML ",	"FAB-2",	"", 0 },
	{ "Hewlett-Packard ",		"Compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard*",		"hp compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard",		"HP Compaq nc6000 *",	"",	"68BDD*", S3_BIOS|S3_MODE },
	/* Winfried Dobbe, https://bugzilla.novell.com/show_bug.cgi?id=159688 */
	{ "Hewlett-Packard",		"HP Compaq nc6120 *",	"",	"", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard",		"HP Compaq nx6125 *",	"",	"", VBE_SAVE|NOFB },
	{ "Hewlett-Packard",		"HP Compaq nc6230 *",	"",	"", VBE_SAVE|NOFB },
	/* Michael Biebl <biebl@teco.edu> */
	{ "Hewlett-Packard ",		"HP compaq nx7000 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Hewlett-Packard",		"HP Compaq nx8220 *",	"",	"", VBE_SAVE|NOFB },
	/* Arthur Peters <amp@singingwizard.org> */
	{ "Hewlett-Packard",		"Presario R3200 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Hewlett-Packard",		"Presario R4100 *",	"",	"", S3_BIOS|S3_MODE },
	/* Fabio Comolli <fabio.comolli@gmail.com> */
	{ "Hewlett-Packard",		"Pavilion dv4000 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
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
	/* X41 Tablet, tested by Danny Kukawka */
	{ "IBM",			"",	"ThinkPad X41 Tablet",	"", S3_BIOS|S3_MODE },
	/* T42p confirmed by Joe Shaw, T41p by Christoph Thiel (both 2373) */
	{ "IBM",			"2373*",		"",	"", S3_BIOS|S3_MODE },
	/* T41p, Stefan Gerber */
	{ "IBM",			"2374*",		"",	"", S3_BIOS|S3_MODE },
	/* Some T43's reported by Sebastian Nagel */
	{ "IBM",			"",		"ThinkPad T43",	"", S3_BIOS|S3_MODE },
	/* T43p reported by Magnus Boman */
	{ "IBM",			"",	"ThinkPad T43p",	"", S3_BIOS|S3_MODE },
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
	/* https://bugzilla.novell.com/show_bug.cgi?id=210928, note the small "p" in Thinkpad */
	{ "LENOVO",			"",		"Thinkpad R60",	"", S3_BIOS|S3_MODE },
	/* At least 1709 series thinkpads can be whitelisted by name,
	   and we can probably delete entries above, too.... */
	{ "LENOVO",			"",		"ThinkPad X60",	"", S3_BIOS|S3_MODE },
	/* T60p, had a report from a 2007* and a 2008* model */
	{ "LENOVO",			"",	"ThinkPad T60p",	"", S3_BIOS|S3_MODE },
	/* T60 From: Jeff Nichols <jnichols@renkoo.net> and Kristofer Hallgren <kristofer.hallgren@gmail.com> */
	{ "LENOVO",			"",	"ThinkPad T60",		"", S3_BIOS|S3_MODE },

	{ "LG Electronics",		"M1-3DGBG",		"",	"", S3_BIOS|S3_MODE },
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-51E*",	"",	"", VBE_POST|VBE_MODE },
	/* Joost Witteveen <joost@iliana.nl> */
	{ "MAXDATA",			"IMPERIO4045A *",	"",	"", 0 },
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
	/* From: Jim <jim.t@vertigo.net.au> */
	{ "TOSHIBA",			"SatellitePro4600",	"",	"", 0 },
	{ "TOSHIBA",			"TECRA S3",		"",	"", 0 },
	/* Stefan Seyfried has one of those :-) S3_BIOS leads to "melting screen" */
	{ "TOSHIBA",			"TECRA 8200",		"",	"", S3_MODE },
	{ "Samsung",			"SQ10",			"",	"", VBE_POST|VBE_SAVE },
	{ "Samsung Electronics",	"SX20S",		"",	"", S3_BIOS|S3_MODE },
	{ "SHARP                           ",	"PC-AR10 *",	"",	"", 0 },
	{ "Sony Corporation",		"VGN-FS115B",		"",	"", S3_BIOS|S3_MODE },
	/* Olivier Saier <osaier@gmail.com>, apparently S3_BIOS locks the machine hard */
	{ "Sony Corporation",		"VGN-FS115Z",		"",	"", S3_MODE },
	/* S.Çağlar Onur <caglar@pardus.org.tr> */
	{ "Sony Corporation",		"VGN-FS215B",	"",	"", 0 },
	/* Joseph Smith <joe@uwcreations.com> */
	{ "Sony Corporation",		"VGN-FS660_W",		"",	"", 0 },
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
