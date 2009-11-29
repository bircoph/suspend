/* whitelist.c
 * whitelist of machines that are known to work somehow
 * and all the workarounds
 * Script generated file, please do not edit by hand
 */

#include "config.h"
#include <stdlib.h>
#include "whitelist.h"

char *whitelist_version = "$Id: whitelist.c ver. 20091123 11:49:43 automatic generation - kix - Exp $";

struct machine_entry whitelist[] = {
	/* Michael Wagner <michael-wagner@gmx.de> */
	{ "4MBOL&S",	"7521 *",	"REV. A0",	"",	0 },
	/* Klaus Ade Johnstad <klaus@skolelinux.no> */
	{ "Acer *",	"AcerPower 2000",	"",	"",	0 },
	/* Jan Matuska <jan.matuska@s-help.sk>, Aspire One 110 */
	{ "Acer",	"AOA110",	"",	"",	0 },
	/* Matt Harlum <Matt@cactuar.net> Aspire One 150 */
	{ "Acer",	"AOA150",	"",	"",	0 },
	/* Simon Bachmann <simonbachmann@bluewin.ch> works with proprietary ATI driver */
	{ "Acer *",	"Aspire 1500 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* Andreas Wagner <A.Wagner@stud.uni-frankfurt.de> */
	{ "Acer",	"Aspire 1520",	"",	"",	VBE_POST|VBE_MODE },
	/* Acer Aspire 1620, Sascha Warner <PRX@gmx.net> */
	{ "Acer           ",	"Aspire 1620    ",	"0100           ",	"V1.05     ",	0 },
	/* Stefano Sabatini https://bugzilla.novell.com/show_bug.cgi?id=415675 */
	{ "Acer *",	"Aspire 1670     ",	"",	"",	S3_BIOS|S3_MODE },
	/* Acer 1810TZ, tom <thoemu@gmail.com> */
	{ "Acer",	"Aspire 1810TZ",	"v0.3115",	"v0.3115",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Acer",	"Aspire 2000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Eberhard Niendorf <Eberhard.Niendorf@t-online.de>, kernel 2.6.25.9 */
	{ "Acer *",	"Aspire 2920 *",	"",	"",	0 },
	/* Marek Stopka (M4r3k) <marekstopka@gmail.com> */
	{ "Acer *",	"Aspire 3100 *",	"",	"",	0 },
	/* From: Bontemps Camille <camille.bontemps@gmail.com> */
	{ "Acer",	"Aspire 3610",	"",	"",	S3_BIOS|S3_MODE },
	/* From: Saul Mena Avila <saul_2110@yahoo.com.mx> */
	{ "Acer",	"Aspire 3620",	"",	"",	S3_BIOS|S3_MODE },
	/* From: Witmaster <witmaster@op.pl> */
	{ "Acer            ",	"Aspire 3690 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Carlos Corbacho <cathectic@gmail.com> */
	{ "Acer",	"Aspire 5020",	"",	"",	VBE_POST },
	/* Fadain Tariq <fadain.tariq@gmail.com> reported S3_BIOS|S3_MODE, but for Donald Curtis <donald@curtisliberty.de> only VBE_* works... */
	{ "Acer *",	"Aspire 5100 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Damyan Ivanov <dmn@debian.org> */
	{ "Acer *",	"Aspire 5110 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Paul Kretek <pk@oz4.org> */
	{ "Acer            ",	"Aspire 5500Z *",	"",	"",	0 },
	/* Vadim Dashkevich <vadim.dashkevich@gmail.com> */
	{ "Acer            ",	"Aspire 5610Z *",	"",	"",	S3_BIOS|S3_MODE },
	/* Andrey Popov <potopi@gmx.net> */
	{ "Acer *",	"Aspire 5630 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Acer Aspire 5730Z, Will Donnelly <will.donnelly@gmail.com> */
	{ "Acer           ",	"Aspire 5730                    ",	"0100           ",	"V1.07          ",	0 },
	/* Christine Upadek <5upadek@informatik.uni-hamburg.de> */
	{ "Acer",	"Extensa 2900",	"",	"",	VBE_POST|VBE_MODE },
	/* Paolo Herms <paolo.herms@gmx.net> */
	{ "Acer",	"Extensa 3000 *",	"",	"",	S3_BIOS|S3_MODE },
	/* seife */
	{ "Acer            ",	"Extensa 4150 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Xavier Douville <s2ram@douville.org> */
	{ "Acer            ",	"Extensa 5620 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Libor Plucnar <Plucnar.Libor@seznam.cz> */
	{ "Acer           ",	"Extensa 6220 *",	"",	"",	S3_BIOS },
	/* Felix Rommel, https://bugzilla.novell.com/show_bug.cgi?id=228572 */
	{ "Acer,Inc.",	"Aspire 1350",	"",	"",	VBE_POST|VBE_MODE|NOFB },
	/* Giorgio Lando <patroclo7@gmail.com> */
	{ "Acer, inc.",	"Aspire 1640     ",	"",	"",	VBE_SAVE },
	/* Stefano Costa <steko@iosa.it> */
	{ "Acer, inc.",	"Aspire 1650     ",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "Acer, inc.",	"Aspire 1690     ",	"",	"",	VBE_POST|VBE_SAVE },
	/* Ricardo Garcia <sarbalap+freshmeat@gmail.com> */
	{ "Acer, inc.",	"Aspire 3000 *",	"",	"",	VBE_POST|VBE_MODE },
	/* <somewho@gmail.com> */
	{ "Acer, inc.",	"Aspire 4520 *",	"",	"",	0 },
	/* Jim Hague <jim.hague@acm.org> */
	{ "Acer, inc.",	"Aspire 5050 *",	"",	"",	S3_BIOS },
	/* From: "cj.yap" <chuanren.ye@gmail.com> */
	{ "Acer, inc.",	"Aspire 5570 *",	"",	"",	VBE_POST|VBE_MODE },
	/* <vanhtu1987@gmail.com>, probably only with nvidia driver :-( */
	{ "Acer, inc.",	"Aspire 5580 *",	"",	"",	0 },
	/* Adrian Levi <adrian.levi@gmail.com> */
	{ "Acer, inc.",	"Aspire 5920G *",	"",	"",	0 },
	/* Ronny Dahl <ronny.dahl@gmx.net> */
	{ "Acer, inc.",	"Aspire 5920 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Hannes Reinecke https://bugzilla.novell.com/show_bug.cgi?id=354728 */
	{ "Acer, inc.",	"Ferrari 1000    ",	"",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "Acer, inc.",	"Ferrari 4000    ",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* Russ Dill <russ.dill@gmail.com> * does not work with fglrx, trenn@suse.de */
	{ "Acer, inc.",	"Ferrari 5000    ",	"",	"",	VBE_POST|VBE_MODE },
	/* Quentin Denis <quentin.denis@gmail.com> */
	{ "Acer, inc.",	"TravelMate 2310 ",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "Acer, inc.",	"TravelMate 3000 ",	"",	"",	VBE_POST|VBE_SAVE },
	/* Norbert Preining <preining@logic.at>, kernel 2.6.23-rc2++ needed */
	{ "Acer, inc.",	"TravelMate 3010 ",	"",	"",	VBE_POST|VBE_MODE },
	/* Momsen Reincke <mreincke@gmx.net> */
	{ "Acer, inc.",	"TravelMate 3020 ",	"",	"",	VBE_POST|VBE_MODE },
	/* Rohan Dhruva <rohandhruva@gmail.com> */
	{ "Acer, inc.",	"TravelMate 3260 ",	"",	"",	VBE_POST|VBE_MODE },
	/* From: LucaB <lbonco@tin.it> */
	{ "Acer, inc.",	"TravelMate 4100 ",	"",	"",	VBE_POST|VBE_SAVE },
	/* Paul Korossy <korossy.p@freemail.hu> */
	{ "Acer, inc.",	"TravelMate 6291 ",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Acer, inc.",	"TravelMate 8100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Sven Foerster <madman-sf@gmx.de> */
	{ "Acer, inc.",	"TravelMate C200 ",	"",	"",	0 },
	/* decoder <decoder@own-hero.net> */
	{ "Acer, inc.",	"Xspire 1650 *",	"",	"",	0 },
	/* Arkadiusz Miskiewicz <arekm@maven.pl> */
	{ "Acer",	"TravelMate 2300",	"",	"",	VBE_POST|VBE_MODE },
	/* Sridar Dhandapani <sridar@gmail.com> */
	{ "Acer           ",	"TravelMate 230 ",	"",	"",	S3_BIOS|S3_MODE },
	/* Arjan Oosting <arjanoosting@home.nl> */
	{ "Acer",	"TravelMate 2350 *",	"",	"",	S3_BIOS|S3_MODE },
	/* */
	{ "Acer           ",	"TravelMate 240 ",	"",	"",	VBE_POST|VBE_SAVE },
	/* jan sekal <jsekal@seznam.cz> */
	{ "Acer",	"TravelMate 2410",	"",	"",	S3_BIOS|S3_MODE },
	/* Peter Stolz <p.stolz@web.de> */
	{ "Acer",	"TravelMate 2420",	"",	"",	S3_BIOS|S3_MODE },
	/* Erki Ferenc <ferki@3rdparty.hu> */
	{ "Acer            ",	"TravelMate 2450 ",	"",	"",	0 },
	/* Vit Cepela <vitek@shar.cz> */
	{ "Acer            ",	"TravelMate 2490 ",	"",	"",	S3_BIOS|S3_MODE },
	/* */
	{ "Acer            ",	"TravelMate 3220 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Andreas Juch <andreas.juch@tele2.at> */
	{ "Acer",	"TravelMate 4000",	"",	"",	S3_BIOS|S3_MODE },
	/* gebu@seznam.cz */
	{ "Acer",	"TravelMate 4400",	"",	"",	0 },
	/* */
	{ "Acer            ",	"TravelMate 4650 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Adrian C. <anrxc@sysphere.org> */
	{ "Acer *",	"TravelMate 5320 *",	"",	"",	S3_BIOS|S3_MODE },
	/* TravelMate 630Lci	Tim Dijkstra <tim@famdijkstra.org> */
	{ "Acer           ",	"TravelMate 630 ",	"",	"",	S3_BIOS|S3_MODE },
	/* Hubert Lepicki <hubert.lepicki@gmail.com> */
	{ "Acer",	"TravelMate 6492",	"",	"",	0 },
	/* Norbert Preining */
	{ "Acer",	"TravelMate 650",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Acer",	"TravelMate 660*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* <leva@ecentrum.hu> */
	{ "Acer",	"TravelMate 7520",	"",	"",	S3_BIOS },
	/* Martin Sack, this one may be NOFB, to be verified */
	{ "Acer",	"TravelMate 800",	"",	"",	VBE_POST },
	/* */
	{ "Acer           ",	"TravelMate C300",	"",	"",	VBE_SAVE },
	/* AMI :-), Mario Maas <mariomaas@googlemail.com> */
	{ "American Megatrends Inc.",	"Uknown",	"1.0",	"P2.00",	0 },
	/* Ian <no-spam@people.net.au> ASI Claro TW7M Notebook */
	{ "AnabelleB",	"Claro TW7M",	"",	"",	S3_BIOS|S3_MODE },
	/* Alexander Wirt reported "VBE_SAVE", but Penny Leach <penny@she.geek.nz> needs VBE_POST|VBE_MODE */
	{ "Apple Computer, Inc.",	"MacBook1,1",	"",	"",	VBE_POST|VBE_MODE },
	/* Marvin Stark <marv@der-marv.de> */
	{ "Apple Computer, Inc.",	"MacBook2,1",	"",	"",	VBE_POST },
	/* Brian Kroth <bpkroth@wisc.edu>, newer BIOS does not like VBE_POST?, (sys_ver:"   MBP22.88Z.00A5.B00.0610192105", VBE_POST|VBE_MODE, Julien BLACHE jb@jblache.org) */
	{ "Apple Computer, Inc.",	"MacBookPro2,2",	"",	"",	VBE_MODE },
	/* Mildred <ml.mildred593@online.fr>, needs VBE_SAVE from console, but that slows resuming */
	{ "Apple Inc.",	"MacBook2,1",	"1.0",	"",	0 },
	/* <skanabiz@gmail.com>  Kernel 2.6.26-rc9, probably with i910 */
	{ "Apple Inc.",	"MacBook3,1",	"1.0",	"",	0 },
	/* Tilman Vogel <tilman.vogel@web.de>, only works from X */
	{ "Apple Inc.",	"MacBookPro4,1",	"",	"",	0 },
	/* Macbook 5,5, Giorgio <giorgio@gilestro.tk> */
	{ "Apple Inc.",	"MacBookPro5,5",	"1.0",	"   MBP55.88Z.00AC.B03.0906151708",	0 },
	/* ASUS a7v600 motherboard, has no usable sys_* entries besides bios_version :-( reported by James Spencer */
	{ "",	"",	"",	"ASUS A7V600 ACPI BIOS Revision *",	S3_BIOS|S3_MODE },
	/* Gabriel <gabriel@opensuse.org> */
	{ "",	"",	"",	"ASUS A7V8X-X ACPI BIOS Revision *",	VBE_POST|VBE_MODE },
	/* ASUS A7V Mainboard, Michael Klein <michael.klein@puffin.lb.shuttle.de> */
	{ "",	"",	"",	"ASUS A7V ACPI BIOS Revision *",	0 },
	/* Tristan Hoffmann <info@tristanhoffmann.de> Newer bios revisions (1705) seem to need VBE_POST, bios_ver: "ASUS M2A-VM ACPI BIOS Revision 1101", S3_BIOS|VBE_MODE, Andreas Bolsch <Andreas.Bolsch@alumni.TU-Berlin.DE> */
	{ "",	"",	"",	"ASUS M2A-VM ACPI BIOS Revision *",	VBE_POST|VBE_MODE },
	/* Jan Schaefer <jan.schaefer@gmail.com> */
	{ "",	"",	"",	"ASUS M2A-VM HDMI ACPI BIOS Revision 1603",	VBE_POST|VBE_MODE },
	/* */
	{ "",	"",	"",	"ASUS M2N32-SLI DELUXE ACPI BIOS Revision 1101",	VBE_POST|VBE_MODE },
	/* another ASUS Mainboard, reported by mailinglist@prodigy7.de */
	{ "",	"",	"",	"ASUS M2N32-SLI DELUXE ACPI BIOS Revision 1201",	VBE_POST|VBE_MODE },
	/* ASUS M2N-E motherboars, not much usable in DMI :-( reported by Ingo <ingo.steiner@gmx.net> */
	{ "",	"",	"",	"ASUS M2N-E ACPI BIOS Revision *",	0 },
	/* another ASUS Mainboard, RidewithStyle <ridewithstyle@googlemail.com> */
	{ "",	"",	"",	"ASUS Striker ACPI BIOS Revision *",	0 },
	/* Cristian Rigamonti */
	{ "ASUSTEK ",	"A2H/L ",	"",	"",	0 },
	/* Daniel Willkomm <plabo@willkomms.de> EEE 1000H */
	{ "ASUSTeK Computer INC.",	"1000H",	"",	"",	0 },
	/* ASUS Eeepc 1002HA, Miguel Figueiredo <elmig@debianpt.org> */
	{ "ASUSTeK Computer INC.",	"1002HA",	"x.x",	"0802   ",	0 },
	/* ASUS Eeepc 1005HA, Raphael Scholer <raphael@scholer-online.de> */
	{ "ASUSTeK Computer INC.",	"1005HA",	"x.x",	"0703   ",	0 },
	/* Andrea Zucchelli <zukka77@gmail.com> EEE-PC 4G */
	{ "ASUSTeK Computer INC.",	"701",	"",	"",	VBE_POST|VBE_MODE },
	/* From: Julien Puydt <jpuydt@free.fr> */
	{ "ASUSTeK Computer Inc.",	"A2D       ",	"",	"",	S3_BIOS|S3_MODE },
	/* Asus A3N, Stephan Schaller <raw-jaw@gmx.de> */
	{ "ASUSTeK Computer Inc.        ",	"A3N       ",	"1.0       ",	"0215                     ",	S3_BIOS },
	/* Thomas Thrainer <tom_t@gmx.at> */
	{ "ASUSTeK Computer Inc.*",	"A6G       ",	"",	"",	0 },
	/* Piotr Drozdek <pioruns@o2.pl> */
	{ "ASUSTeK Computer Inc. ",	"A6JC *",	"",	"",	0 },
	/* Arnout Boelens <aboelens@engin.umass.edu> */
	{ "ASUSTeK Computer Inc. ",	"A6J *",	"",	"",	VBE_POST|VBE_MODE },
	/* Tom Gruben <tom.gruben@gmail.com>, maybe only with nvidia? */
	{ "ASUSTeK Computer Inc. ",	"A6K *",	"",	"",	0 },
	/* Patryk Zawadzki <patrys@icenter.pl> */
	{ "ASUSTeK Computer Inc. ",	"A6Km      ",	"",	"",	0 },
	/* Tom Malfrere <tom.malfrere@pandora.be> */
	{ "ASUSTeK Computer Inc. ",	"A6Kt      ",	"",	"",	S3_BIOS|S3_MODE },
	/* Szalai Akos <szalakos@gmail.com> */
	{ "ASUSTeK Computer Inc. ",	"A6M",	"",	"",	VBE_POST },
	/* Anthony Mahe <anthonymahe@gmail.com> */
	{ "ASUSTeK Computer INC. ",	"A6T",	"",	"",	0 },
	/* tias@ulyssis.org */
	{ "ASUSTeK Computer Inc. ",	"A6U       ",	"",	"",	VBE_POST|VBE_MODE },
	/* Daniel Bumke <danielbumke@gmail.com> */
	{ "ASUSTeK Computer INC. ",	"A6VA      ",	"",	"",	0 },
	/* Alex Myltsev <avm@altlinux.ru> */
	{ "ASUSTeK Computer Inc. ",	"A8He *",	"",	"",	S3_BIOS|S3_MODE },
	/* Oliver Moessinger <olivpass@web.de> */
	{ "ASUSTeK Computer Inc. ",	"A8JS *",	"",	"",	0 },
	/* Arkadiusz Miskiewicz <arekm@maven.pl> */
	{ "ASUSTeK Computer Inc. ",	"F2J *",	"",	"",	S3_BIOS },
	/* Julien Puydt <jpuydt@free.fr> works from console and with nvidia driver, S3_BIOS|S3_MODE (q0mw3fk02@sneakemail.com) */
	{ "ASUSTeK Computer Inc. ",	"F3F *",	"",	"",	0 },
	/* Daniel Kinzler <daniel@brightbyte.de> */
	{ "ASUSTeK Computer Inc. ",	"F3JC *",	"",	"",	S3_BIOS|S3_MODE },
	/* Marco Jorge <mbernardesjorge@users.sourceforge.net> */
	{ "ASUSTeK Computer Inc. ",	"F3JP *",	"",	"",	VBE_POST|VBE_MODE },
	/* Oliver Neukum, only works without FB, https://bugzilla.novell.com/show_bug.cgi?id=425071 */
	{ "ASUSTeK Computer Inc. ",	"F3Ka *",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* Anghinolfi Luca <anghi83@gmail.com> */
	{ "ASUSTeK Computer INC. *",	"F3Sg *",	"",	"",	0 },
	/* Vladimir Pouzanov <farcaller@gmail.com>, probably only with nvidia and from X */
	{ "ASUSTeK Computer INC. ",	"F3T",	"",	"",	0 },
	/* Asus F80L laptop, Manuel P�gouri�-Gonnard <mpg@elzevir.fr> */
	{ "ASUSTeK Computer Inc.        ",	"F80L                ",	"1.0       ",	"204    ",	VBE_POST },
	/* Enrico Zini <enrico@enricozini.org> */
	{ "ASUSTeK Computer Inc. *",	"F9E *",	"",	"",	0 },
	/* Paul Cousins <namain@gmail.com> */
	{ "ASUSTeK Computer Inc. *",	"G1S *",	"",	"",	0 },
	/* ASUS Mainboard, Christoph Jaeschke <chrjae@arcor.de> */
	{ "ASUSTek Computer Inc.",	"K8N-E-Deluxe",	"",	"",	S3_BIOS },
	/* Asus Mainboard, Ian McAnena <ianegg@gmail.com> */
	{ "ASUSTeK Computer Inc.",	"K8V-MX",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "ASUSTeK Computer Inc.",	"L7000G series Notebook PC*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Guenther Schwarz <guenther.schwarz@gmx.de> */
	{ "ASUSTeK Computer Inc.        ",	"M2N       ",	"",	"",	S3_BIOS|S3_MODE },
	/* Peter Suetterlin <P.Suetterlin@astro.uu.nl>, VBE_MODE does not work */
	{ "ASUSTeK Computer Inc.        ",	"M3N       ",	"",	"",	VBE_POST|VBE_SAVE },
	/* Galeffi Christian gallochri2@alice.it */
	{ "ASUSTeK Computer Inc.        ",	"M51Sr *",	"",	"",	S3_MODE },
	/* Charles de Miramon */
	{ "ASUSTeK Computer Inc.        ",	"M5N       ",	"",	"",	S3_BIOS|S3_MODE },
	/* */
	{ "ASUSTeK Computer Inc.        ",	"M6Ne      ",	"",	"",	S3_MODE },
	/* Manfred Tremmel, https://bugzilla.novell.com/show_bug.cgi?id=171107 */
	{ "ASUSTeK Computer Inc.        ",	"M6N       ",	"",	"",	S3_BIOS|S3_MODE },
	/* M6VA, seraphim@glockenbach.net */
	{ "ASUSTeK Computer Inc.        ",	"M6VA      ",	"",	"",	S3_BIOS|S3_MODE },
	/* M7N, Xavier Douville <s2ram-sourceforge@douville.org> */
	{ "ASUSTeK Computer Inc.        ",	"M7A       ",	"",	"",	S3_BIOS|S3_MODE },
	/* ASUS S5200N Philip Frei <pjf@gmx.de> */
	{ "ASUSTeK Computer Inc.        ",	"S5N       ",	"",	"",	S3_BIOS|S3_MODE },
	/* Tobias Rothe <ir268155@mail.inf.tu-dresden.de> */
	{ "ASUSTeK Computer Inc.        ",	"U3S *",	"",	"",	VBE_POST|VBE_SAVE },
	/* ASUS U5F, Krzysztof Krzyzaniak <eloy@kofeina.net> */
	{ "ASUSTeK Computer Inc. ",	"U5F *",	"",	"",	S3_BIOS },
	/* Szymon Olko <szymonolko@o2.pl> */
	{ "ASUSTeK Computer Inc. ",	"V1J *",	"",	"",	VBE_POST|VBE_MODE|NOFB },
	/* ASUS V6V, Johannes Engel <j-engel@gmx.de> */
	{ "ASUSTeK Computer INC.",	"V6V",	"",	"",	S3_MODE },
	/* Kanru Chen <kanru@csie.us> */
	{ "ASUSTeK Computer INC.",	"W3A",	"",	"",	S3_BIOS|S3_MODE },
	/* ASUS W5A, Riccardo Sama' <riccardo.sama@yetopen.it> */
	{ "ASUSTeK Computer Inc.        ",	"W5A       ",	"",	"",	S3_BIOS|S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "ASUSTeK Computer Inc.",	"W5A*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* ASUS W7J, https://bugzilla.novell.com/show_bug.cgi?id=351661 */
	{ "ASUSTeK Computer Inc.",	"W7J *",	"",	"",	VBE_MODE },
	/* Bernhard M. Wiedemann <s2rambmw@lsmod.de>, works with radeon and fglrx */
	{ "ASUSTeK Computer Inc.",	"X51RL *",	"",	"",	VBE_POST|VBE_SAVE },
	/* ASUS Z35Fm, Francois Marier <francois@debian.org> */
	{ "ASUSTeK Computer Inc.        ",	"Z35FM *",	"",	"",	S3_BIOS|PCI_SAVE },
	/* */
	{ "ASUSTEK ",	"L2000D",	"",	"",	S3_MODE },
	/* */
	{ "ASUSTEK ",	"L3000D",	"",	"",	VBE_POST|VBE_SAVE },
	/* Carl Troein <carl@thep.lu.se> */
	{ "ASUSTEK ",	"M2000E",	"",	"",	S3_BIOS },
	/* Ross Patterson <me@rpatterson.net> */
	{ "AVERATEC",	"1000 Series",	"",	"",	S3_BIOS|S3_MODE },
	/* Mark Stillwell */
	{ "AVERATEC",	"3700 Series",	"",	"",	S3_BIOS|S3_MODE },
	/* Joel Dare <joel@joeldare.com> */
	{ "AVERATEC",	"5500 Series",	"",	"",	S3_MODE },
	/* Sebastian Maus <sebastian.maus@bluemars.net> */
	{ "BENQ",	"JoyBook 7000",	"GRASSHOPPER2",	"",	S3_BIOS|S3_MODE },
	/* Andreas Schmitz */
	{ "BenQ           ",	"Joybook R22",	"",	"",	S3_BIOS|S3_MODE },
	/* Miroslav Hruz <miroslav.hruz@gmail.com> */
	{ "BenQ",	"Joybook S32",	"",	"",	VBE_POST|VBE_MODE },
	/* Paul Duncan <eresse@greenpixel.ch> */
	{ "BenQ",	"Joybook S52",	"",	"",	VBE_POST|VBE_MODE },
	/* Alexandre Viard <alexandre.viard@gmail.com> */
	{ "CLEVO CO.",	"TN120R",	"",	"",	0 },
	/* Gilles Grandou <gilles@grandou.net> */
	{ "CLEVO",	"D500P",	"",	"",	VBE_POST|NOFB },
	/* Francisco Neto <frc_neto@yahoo.com.br> */
	{ "CLEVO ",	"M5X0JE ",	"",	"",	S3_BIOS },
	/* Justin Lewis <jlew.blackout@gmail.com> */
	{ "COMPAL",	"HEL80C          ",	"",	"",	0 },
	/* Leszek Kubik <leszek.kubik@googlemail.com> */
	{ "COMPAL          ",	"HEL80I          ",	"",	"",	0 },
	/* Jason Clinton <jclinton@advancedclustering.com> */
	{ "COMPAL *",	"HEL81I *",	"",	"",	0 },
	/* This Armada m300, reported by benoit.monin@laposte.net seems not to have a useful model description, so to make sure i also added the bios_version */
	{ "Compaq",	"Armada                                     ",	"",	"1.35",	0 },
	/* seife has some of those */
	{ "Compaq",	"Armada    E500  *",	"",	"",	0 },
	/* <marmodoro@libero.it> */
	{ "Compaq",	"Armada    M700 *",	"",	"",	0 },
	/* Rod Schafter rschaffter@comcast.net */
	{ "Compaq",	"Armada    M70  *",	"",	"",	0 },
	/* Martin Heimes <martin.heimes@gmx.de> */
	{ "Compaq",	"Evo  D510 USDT",	"",	"",	0 },
	/* Viktor Placek <viktor.placek@fs.cvut.cz> */
	{ "Compaq",	"Evo N400c *",	"",	"",	0 },
	/* Joel Schaerer <joel.schaerer@insa-lyon.fr> has a later BIOS and needs different Options :-/ sys_ver:"F.05", VBE_POST|VBE_SAVE, Chris AtLee <chris@atlee.ca>, VBE_MODE does not work, text size changes. */
	{ "Compaq ",	"Evo N800w *",	"",	"",	S3_BIOS|S3_MODE|VBE_POST|VBE_MODE },
	/* */
	{ "Compaq*",	"N620c *",	"",	"",	S3_BIOS|S3_MODE },
	/* Paolo Saggese <pmsa4-gen@yahoo.it> */
	{ "Compaq",	"Presario 2701EA *",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* Ed Barrett <ebarrett@tadl.tcnet.org> */
	{ "Dell Computer Corp.",	"Latitude LS  ",	"",	"",	0 },
	/* Alain Prignet <alain.prignet@univ-mlv.fr> */
	{ "Dell Computer Corp.",	"Latitude X200",	"",	"",	VBE_POST|VBE_SAVE },
	/* Michael Witten mfwitten@MIT.EDU */
	{ "Dell Computer Corporation",	"Dimension 2400 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Allen <ducalen@sympatico.ca>, Desktop with onboard grephics */
	{ "Dell Computer Corporation",	"Dimension 3000 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Eitan Mosenkis <eitan@mosenkis.net> */
	{ "Dell Computer Corporation",	"Inspiron 1100 *",	"",	"",	VBE_POST|VBE_MODE },
	/* From bug 1544913@sf.net, Andrei - amaces */
	{ "Dell Computer Corporation",	"Inspiron 1150*",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 1200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Dell Inspiron 500m, Georg Zumstrull <Georg.Zumstrull@web.de> */
	{ "Dell Computer Corporation",	"Inspiron 500m*",	"",	"",	VBE_POST|VBE_MODE },
	/* Eric Sandall <eric@sandall.us> */
	{ "Dell Computer Corporation",	"Inspiron 5100 *",	"",	"",	NOFB },
	/* Another Inspiron 5100, Tony Ernst <tee@sgi.com> */
	{ "Dell Computer Corporation",	"Inspiron 5100",	"Revision A0",	"",	VBE_SAVE|VBE_MODE|PCI_SAVE },
	/* VBE_POST|VBE_SAVE works on text console, but not under X. Tested by Chirag Rajyaguru */
	{ "Dell Computer Corporation",	"Inspiron 5150*",	"",	"",	VBE_SAVE },
	/* Felipe Alvarez https://bugzilla.novell.com/show_bug.cgi?id=364429 */
	{ "Dell Computer Corporation",	"Inspiron 5160*",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 6000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Dell Inspiron 600m, Gavrie Philipson <gavrie@gmail.com> */
	{ "Dell Computer Corporation",	"Inspiron 600m *",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 700m*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* */
	{ "Dell Computer Corporation",	"Inspiron 8000 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 8100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 8200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* by Henare Degan <henare.degan@gmail.com> 8500 w. NVidia card. There are also 8500s w. ATI cards */
	{ "Dell Computer Corporation",	"Inspiron 8500 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 8600*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Inspiron 9300*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude 110L*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Bernie du Breuil <bmrcldb56@yahoo.com> */
	{ "Dell Computer Corporation",	"Latitude C400 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "Dell Computer Corporation",	"Latitude C600 *",	"",	"",	RADEON_OFF },
	/* */
	{ "Dell Computer Corporation",	"Latitude C610 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Tom Lear <tom@trap.mtview.ca.us> */
	{ "Dell Computer Corporation",	"Latitude C840 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* Manuel Jander */
	{ "Dell Computer Corporation",	"Latitude CPx J650GT*",	"",	"",	0 },
	/* Luke Myers */
	{ "Dell Computer Corporation",	"Latitude CPx J800GT*",	"",	"",	VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=366460 */
	{ "Dell Computer Corporation",	"Latitude D400 *",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude D410*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Roland Hagemann <mail@rolandhagemann.de> */
	{ "Dell Computer Corporation",	"Latitude D500 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude D510*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* */
	{ "Dell Computer Corporation",	"Latitude D600 *",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* */
	{ "Dell Computer Corporation",	"Latitude D800 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude D810*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude X1*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Latitude X300*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Seppe Hoogzaad <seppe.hoogzaad@gmail.com> */
	{ "Dell Computer Corporation",	"OptiPlex 170L *",	"",	"",	0 },
	/* Ian McAnena <ianegg@gmail.com>, zhengdao@iastate.edu */
	{ "Dell Computer Corporation",	"OptiPlex GX260 *",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Computer Corporation",	"Precision M20*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Per Øyvind Karlsen <pkarlsen@mandriva.com> VBE_POST will get console working, but break X */
	{ "Dell Computer Corporation",	"Precision M60*",	"",	"",	VBE_SAVE },
	/* Christoph Pacher <christoph.pacher@arcs.ac.at>, maybe only with nvidia driver :-( */
	{ "Dell Computer Corporation",	"Precision WorkStation 360*",	"",	"",	0 },
	/* Richard Osborne <r.osborne@bigfoot.com>, Dimension E520 */
	{ "Dell Inc. *",	"Dell DM061 *",	"",	"",	0 },
	/* Struan Bartlett <struan.bartlett@NewsNow.co.uk> */
	{ "Dell Inc.                ",	"Dell DXP051 *",	"",	"",	0 },
	/* Edward Webb https://bugzilla.novell.com/show_bug.cgi?id=428181 */
	{ "Dell Inc. *",	"Dimension 9100 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Dell Inspiron 1010, Alfredo Rezinovsky <alfredo@rezinovsky.com.ar> */
	{ "Dell Inc.",	"Inspiron 1010   ",	"A09",	"A09",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 1200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* joey <joey7d6@yahoo.com> */
	{ "Dell Inc.",	"Inspiron 1420 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Dell Inspiron 1501, https://bugzilla.novell.com/show_bug.cgi?id=304281 */
	{ "Dell Inc.",	"Inspiron 1501 ",	"",	"",	S3_BIOS|S3_MODE },
	/* Sebastian Rittau <srittau@jroger.in-berlin.de> */
	{ "Dell Inc.",	"Inspiron 1520 *",	"",	"",	0 },
	/* Horvath Andras / Log69.com <mail@log69.com> previously Klaus Thoennissen <klaus.thoennissen@web.de> (VBE_POST|VBE_MODE) */
	{ "Dell Inc.",	"Inspiron 1525 *",	"",	"",	VBE_SAVE },
	/* Christophe Gay <christophegay@free.fr>, maybe only with binary only driver */
	{ "Dell Inc.",	"Inspiron 1720 *",	"",	"",	0 },
	/* Dell Inspiron 510m, Jose Carlos Garcia Sogo <jsogo@debian.org> */
	{ "Dell Inc.",	"Inspiron 510m *",	"",	"",	VBE_POST },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 6000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 700m*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 8100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 8200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 8600*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Inspiron 9300*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Latitude 110L*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=220865 */
	{ "Dell Inc.",	"Latitude D410 *",	"",	"",	VBE_POST|VBE_MODE },
	/* tested by seife and Nenad ANTONIC <nenad.antonic@gmail.com> */
	{ "Dell Inc.",	"Latitude D420 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Benoit Gschwind <doth.gschwind@gmail.com> */
	{ "Dell Inc.",	"Latitude D430 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Ian Samule, https://bugzilla.novell.com/show_bug.cgi?id=175568 */
	{ "Dell Inc.",	"Latitude D505 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Latitude D510*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* tested by seife */
	{ "Dell Inc.",	"Latitude D520 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Jim <jim@zednet.org.uk> */
	{ "Dell Inc.",	"Latitude D530 *",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "Dell Inc.",	"Latitude D610 *",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* D620 reported by Miroslav Ruda <ruda@ics.muni.cz>, <nicolae.mihalache@spaceapplications.com> */
	{ "Dell Inc.",	"Latitude D620 *",	"",	"",	VBE_POST|VBE_MODE },
	/* D630 Ian Bezanson <irb@getchalk.com> and tested by seife */
	{ "Dell Inc.",	"Latitude D630 *",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Latitude D810*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Johannes Engel <j-engel@gmx.de> */
	{ "Dell Inc.",	"Latitude D820 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Gabriel Ambuehl <gabriel_ambuehl@buz.ch>, version with intel graphics */
	{ "Dell Inc.",	"Latitude D830 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Dell Latitude E4300, Nicolas Estibals <Nicolas.Estibals@gmail.com> & Frank Ploss <frank@frankploss.de> */
	{ "Dell Inc.",	"Latitude E4300                  ",	"",	"A09",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Latitude X1*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Latitude X300*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Dell inspiron 1300, Tim Dijkstra <tim@famdijkstra.org> */
	{ "Dell Inc.",	"ME051 *",	"",	"",	0 },
	/* Dell e1505, Alexander Antoniades / Inspiron 6400, James Cherryh <jcherryh@gmail.com> */
	{ "Dell Inc.",	"MM061 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Dell Inspiron E1702, Julian Krause <suspend@thecrypto.org> */
	{ "Dell Inc.",	"MP061 *",	"",	"",	0 },
	/* Dell Inspiron E1705, Paul Hummel <paulhummel@gmail.com> */
	{ "Dell Inc.",	"MP061",	"",	"",	0 },
	/* Dell Inspiron 630m, Fredrik Edemar */
	{ "Dell Inc.",	"MXC051 *",	"",	"",	0 },
	/* Dell Inspiron 640m, Daniel Drake <dsd@gentoo.org> */
	{ "Dell Inc.",	"MXC061 *",	"",	"",	VBE_POST },
	/* Dell XPS M1210 BryanL <fox_rlnocr@dynmail.no-ip.org> */
	{ "Dell Inc.",	"MXC062 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Dell XPS M1710 Harry Kuiper <hkuiper@xs4all.nl>, console only resumes with vesafb! */
	{ "Dell Inc.",	"MXG061 *",	"",	"",	0 },
	/* Ted Percival <ted@midg3t.net>, ATI Graphics, no FB :( */
	{ "Dell Inc.",	"OptiPlex 755 *",	"",	"",	VBE_POST|NOFB },
	/* */
	{ "Dell Inc.",	"OptiPlex FX160 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Dell FX160, James Willcox <snorp@novell.com> https://bugzilla.novell.com/show_bug.cgi?id=379774 */
	{ "Dell Inc.",	"Optiplex FX160 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Desktop with Intel graphics, Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Dell Inc.                ",	"OptiPlex GX520 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Jon Dowland <jon+suspend-devel@alcopop.org> */
	{ "Dell Inc. *",	"OptiPlex GX620 *",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Dell Inc.",	"Precision M20*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Bruno Friedmann <bruno@ioda-net.ch> */
	{ "Dell Inc.",	"Precision M4300 *",	"",	"",	0 },
	/* Chris Debrunner <chd@evcohs.com> */
	{ "Dell Inc.",	"Precision M6300 *",	"",	"",	VBE_POST|VBE_MODE },
	/* gaofi@gaofi.com */
	{ "Dell Inc.",	"Precision M65 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Manoj Srivastava <srivasta@debian.org> */
	{ "Dell Inc.",	"Precision M90 *",	"",	"",	0 },
	/* Florian Buether <fbuether@jasminefields.net> */
	{ "Dell Inc.",	"Vostro   1000 ",	"",	"",	S3_BIOS|S3_MODE },
	/* Kyle Kearney <elyk53@gmail.com> */
	{ "Dell Inc.",	"Vostro 1500 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Luciano A. Ferrer <laferrer@gmail.com> */
	{ "Dell Inc.",	"Vostro1710",	"",	"",	0 },
	/* Dell XPS M1330, Danny Kukawka <dkukawka@suse.de> */
	{ "Dell Inc.",	"XPS M1330 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Pierre <pinaraf@robertlan.eu.org> maybe only with nvidia driver...  Elie Roux <elie.roux@telecom-bretagne.eu> */
	{ "Dell Inc.",	"XPS M1530 *",	"",	"",	0 },
	/* Jarek Grzys <grzys@gazeta.pl> */
	{ "DIXONSXP",	"To be filled by O.E.M.",	"Ver.001",	"",	0 },
	/* Michael Dickson <mike@communicatezing.com> */
	{ "ECS",	"536",	"",	"",	VBE_POST|NOFB },
	/* imported from acpi-support 0.59 "half known". */
	{ "ECS",	"G556 Centrino*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Michael Bunk <michael.bunk@gmail.com> */
	{ "Elitegroup Co.",	"ECS G320",	"",	"",	VBE_POST|VBE_MODE },
	/* ASUS M2400N, Daniel Gollub */
	{ "ERGOUK                       ",	"M2N       ",	"",	"",	S3_BIOS|S3_MODE },
	/* Aurelien Jacobs <aurel@gnuage.org> */
	{ "FUJITSO",	"FMVLT70R",	"",	"",	S3_BIOS|S3_MODE|PCI_SAVE },
	/* Fujitsu S6120, Andre Kusuma <andrekusuma@gmail.com> */
	{ "FUJITSU",	"0000000000",	" ",	"Version 1.04 ",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU",	"Amilo M*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Stephen Gildea <stepheng@gildea.com> */
	{ "FUJITSU",	"LifeBook P7010D",	"",	"",	S3_BIOS },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU",	"LIFEBOOK P7010*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Andi Kleen, reported to work in 64bit mode */
	{ "FUJITSU",	"LifeBook S2110",	"",	"",	S3_BIOS },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU",	"LIFEBOOK S6120*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Srinath Madhavan <msrinath80@yahoo.com> */
	{ "FUJITSU",	"LifeBook S7110",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU",	"LifeBook S Series*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Duy Nguyen <d.nguyen4@ugrad.unimelb.edu.au> */
	{ "FUJITSU",	"LifeBook T2010",	"",	"",	S3_BIOS|S3_MODE },
	/* David Nolden <zwabel@googlemail.com> */
	{ "FUJITSU",	"LifeBook T Series",	"",	"",	0 },
	/* Loki Harfagr <l0k1@free.fr> */
	{ "FUJITSU SIEMENS",	"Amilo A1645 ",	"",	"",	S3_BIOS|S3_MODE },
	/* Ramon Schenkel <ramonschenkel@onlinehome.de> */
	{ "FUJITSU SIEMENS",	"Amilo A1650G",	"",	"",	VBE_MODE },
	/* stan ioan-eugen <stan.ieugen@gmail.com> */
	{ "FUJITSU SIEMENS",	"Amilo A1667G Serie",	"",	"",	S3_BIOS },
	/* Holger Macht <hmacht@suse.de>, with kernel 2.6.22+, x86_64 */
	{ "FUJITSU SIEMENS",	"Amilo A7640 ",	"",	"",	S3_BIOS|S3_MODE },
	/* reported by Thomas Halva Labella <hlabella@ulb.ac.be> */
	{ "FUJITSU SIEMENS",	"Amilo A7645 ",	"",	"",	VBE_SAVE|S3_BIOS|S3_MODE },
	/* Chris Hammond <christopher.hammond@gmail.com> Amilo A 1630 */
	{ "FUJITSU SIEMENS",	"Amilo A Series",	"0.01",	"",	VBE_POST|VBE_SAVE },
	/* Hendrik Golzke <hendrik.golzke@yahoo.de> */
	{ "FUJITSU SIEMENS",	"Amilo D Series",	"",	"",	VBE_POST|VBE_MODE },
	/* Marcel Klemmer <imklemmer@web.de> */
	{ "FUJITSU SIEMENS",	"AMILO L Series",	"",	"",	VBE_POST|VBE_MODE },
	/* Simon Maurer <Twiks@gmx.de> again, match exactly. */
	{ "FUJITSU SIEMENS",	"AMILO M        ",	"-1 *",	"R01-S0N *",	VBE_POST|VBE_SAVE|PCI_SAVE },
	/* Reiner Herrmann <reiner@reiner-h.de>, this one realls needs VBE_SAVE since there are many different models of the "AMILO M" series, i do a very exact match */
	{ "FUJITSU SIEMENS",	"AMILO M        ",	"-1 *",	"R01-S0Z *",	VBE_POST|VBE_SAVE },
	/* Moritz Heidkamp <moritz@invision-team.de> */
	{ "FUJITSU SIEMENS",	"AMILO M Series",	"",	"1.0G*",	VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU SIEMENS",	"Amilo M*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Sab <sabry84@gmail.com> */
	{ "FUJITSU SIEMENS",	"AMILO Pi 1505",	"",	"",	S3_BIOS|S3_MODE },
	/* Giorgos Mavrikas <mavrikas@gmail.com> */
	{ "FUJITSU SIEMENS",	"AMILO Pi 1536",	"",	"",	VBE_POST|VBE_MODE },
	/* Yasen Pramatarov <yasen@lindeas.com> */
	{ "FUJITSU SIEMENS",	"AMILO Pi 2515",	"",	"",	VBE_POST|VBE_MODE },
	/* Alexandr Kara <Alexandr.Kara@seznam.cz> */
	{ "FUJITSU SIEMENS",	"AMILO Pro Edition V3405",	"",	"",	S3_BIOS|S3_MODE },
	/* Igor A. Goncharovsky <igi-go@ya.ru> */
	{ "FUJITSU SIEMENS",	"AMILO Pro Edition V3505 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Juhasz Adam <jadaml@gmail.com> */
	{ "FUJITSU SIEMENS",	"AMILO Pro Series V3525 *",	"",	"",	0 },
	/* Aleksandar Markovic <acojlo@gmail.com> */
	{ "FUJITSU SIEMENS",	"AMILO PRO V2030",	"",	"",	VBE_POST|VBE_MODE },
	/* some unknown contributor */
	{ "FUJITSU SIEMENS",	"AMILO PRO V2035",	"",	"",	VBE_POST|VBE_MODE },
	/* Florian Herrmann <florian-herrmann@gmx.de> */
	{ "FUJITSU SIEMENS",	"AMILO Pro V2040",	"",	"",	0 },
	/* <arild.haugstad@gmx.net> / https://bugzilla.novell.com/show_bug.cgi?id=279944 */
	{ "FUJITSU SIEMENS",	"AMILO Pro V3205",	"",	"",	S3_BIOS|S3_MODE },
	/* AmiloPro V3515, Julian Wollrath <jwollrath@web.de> */
	{ "FUJITSU SIEMENS",	"AMILO PRO V3515",	"20",	"1.0M-2309-8A20",	VBE_POST|VBE_MODE },
	/* Steffen <mlsone@forevers.de> */
	{ "FUJITSU SIEMENS",	"AMILO PRO V8010 *",	"",	"",	VBE_POST|VBE_MODE },
	/* Markus Meyer <markus.meyer@koeln.de>, Christian Illy <christian.illy@gmx.de> */
	{ "FUJITSU SIEMENS",	"Amilo Si 1520",	"",	"",	S3_BIOS|S3_MODE },
	/* <noname1@onlinehome.de> */
	{ "FUJITSU SIEMENS",	"AMILO Xi 1546",	"",	"",	S3_BIOS|S3_MODE },
	/* Elmar Stellnberger <estellnb@gmail.com> */
	{ "FUJITSU SIEMENS",	"AMILO Xi 2550",	"                                ",	"1.11C",	VBE_POST|VBE_MODE },
	/* Guilhem Bonnefille <guilhem.bonnefille@gmail.com> */
	{ "FUJITSU SIEMENS",	"ESPRIMO Mobile D9500",	"",	"",	0 },
	/* Martin Wilck <martin.wilck@fujitsu-siemens.com> */
	{ "FUJITSU SIEMENS",	"ESPRIMO Mobile M9400",	"",	"",	0 },
	/* Jan Schaefer <jan.schaefer@informatik.fh-wiesbaden.de> */
	{ "FUJITSU SIEMENS",	"ESPRIMO P*",	"",	"",	0 },
	/* Clement LAGRANGE <clement.lagrange@gmail.com> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK B Series",	"",	"",	0 },
	/* wolfgang.aigner@gmx.de */
	{ "FUJITSU SIEMENS",	"LIFEBOOK E4010",	"",	"",	VBE_SAVE },
	/* Robin Knapp <mail@gizzmo.org> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK E8020",	"",	"",	VBE_POST|VBE_MODE },
	/* Florian Lohoff <flo@rfc822.org> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK E8110",	"",	"",	0 },
	/* seife */
	{ "FUJITSU SIEMENS",	"LIFEBOOK E8410",	"",	"",	S3_BIOS|S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU SIEMENS",	"LIFEBOOK P7010*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Michael Hillerstrom <michael@hillerstrom.org> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK P7230",	"",	"",	S3_BIOS|S3_MODE|PCI_SAVE },
	/* Bernd Speiser <bernd.speiser@t-online.de> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK P8010",	"",	"",	S3_BIOS|S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU SIEMENS",	"LIFEBOOK S6120*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Fred Lahuis <fred@sron.nl> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK S6410",	"",	"",	VBE_POST|VBE_MODE },
	/* <jan.lockenvitz.ext@siemens.com>, tested with X only */
	{ "FUJITSU SIEMENS",	"LIFEBOOK S7010",	"",	"",	0 },
	/* Antonio Cardoso Martins <digiplan.pt@gmail.com> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK S7020",	"",	"",	S3_BIOS|S3_MODE|PCI_SAVE },
	/* Samuel Tardieu <sam@rfc1149.net> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK S7110",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "FUJITSU SIEMENS",	"LifeBook S Series*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Alexander Clouter <alex@digriz.org.uk>, needs vbe_save or the vga mode is upset */
	{ "FUJITSU SIEMENS",	"LifeBook T2010",	"",	"",	VBE_POST|VBE_SAVE|PCI_SAVE },
	/* arnold seiler <arnold338250622@web.de> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK T3010",	"",	"",	0 },
	/* Eckhart Woerner <ew@ewsoftware.de> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK T4010",	"",	"",	S3_BIOS|S3_MODE },
	/* Jay <jay@jay.cz> */
	{ "FUJITSU SIEMENS",	"LIFEBOOK T4210",	"",	"",	S3_BIOS|S3_MODE|PCI_SAVE },
	/* Tomas Pospisek <tpo@sourcepole.ch> */
	{ "Fujitsu Siemens",	"P6VAP-AP",	"",	"",	0 },
	/* Ulf Lange <mopp@gmx.net> */
	{ "FUJITSU SIEMENS",	"SCENIC N300/N600",	"",	"",	VBE_POST|VBE_MODE },
	/* Guenther Schwarz <guenther.schwarz@gmx.de> */
	{ "FUJITSU SIEMENS",	"SCENIC P / SCENICO P",	"",	"",	VBE_POST|VBE_MODE },
	/* This is a desktop with onboard i810 video <seife@suse.de> */
	{ "FUJITSU SIEMENS",	"SCENIC W300/W600",	"",	"",	VBE_POST|VBE_MODE },
	/* Peer Heinlein <p.heinlein@jpberlin.de> */
	{ "FUJITSU SIEMENS",	"SCENIC W",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "FUJITSU SIEMENS",	"Stylistic ST5000",	"",	"",	S3_BIOS|S3_MODE },
	/* Mario Nigrovic <Mario@nigrovic.net> */
	{ "Gateway *",	"MT6707 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Gateway MX3228, The Space Man <thespaceman@verizon.net> */
	{ "Gateway",	"MX3228",	"73.08   ",	"73.08   ",	VBE_POST|VBE_MODE },
	/* Ilya Eremin <che_guevara_3@bk.ru> */
	{ "Gateway *",	"MX6922B *",	"",	"",	0 },
	/* Christoph Wiesmeyr <christoph.wiesmeyr@liwest.at> */
	{ "GERICOM",	"259IA1",	"",	"",	0 },
	/* booiiing@gmail.com */
	{ "Gericom",	"HUMMER",	"",	"1.03   ",	VBE_POST|VBE_MODE },
	/* From: Einon <einon@animehq.hu>, seems to work only from X :-( */
	{ "Gericom",	"Montara-GML ",	"FAB-2",	"",	0 },
	/* Nik Tripp https://bugzilla.novell.com/show_bug.cgi?id=400356 */
	{ "Gigabyte Technology Co., Ltd.",	"945GCM-S2L",	"",	"",	0 },
	/* Art Mitskevich <art@unlit.org> */
	{ "Gigabyte Technology Co., Ltd.",	"945GCMX-S2",	"",	"",	VBE_POST|VBE_MODE },
	/* Josef Zenisek <jzenisek@gmx.de> */
	{ "Gigabyte Technology Co., Ltd.",	"945GZM-S2",	"",	"",	0 },
	/* Florian Sesser <florian@sesser.at> */
	{ "Gigabyte Technology Co., Ltd.",	"GA-MA69G-S3H",	" ",	"F5",	0 },
	/* Andi Valachi <avalachi@2link.ca> */
	{ "Gigabyte Technology Co., Ltd.",	"GA-MA785GM-US2H",	" ",	"F5",	S3_BIOS|S3_MODE },
	/* Ian-Xue Li <da.mi.spirit@gmail.com> */
	{ "Gigabyte Technology Co., Ltd.",	"GA-MA78GPM-DS2H",	" ",	"F1",	S3_BIOS },
	/* Thorsten Goetzke <th.goetzke@gmx.net> */
	{ "Gigabyte Technology Co., Ltd.",	"P35-DS3",	" ",	"F6",	0 },
	/* Gigabyte P35-DS3L, Kainr�th Roland <kainrath.roland@gmail.com> */
	{ "Gigabyte Technology Co., Ltd.",	"P35-DS3L",	" ",	"F8",	0 },
	/* Roberto Lumbreras <rover@lumbreras.org> */
	{ "Gigabyte Technology Co., Ltd.",	"P35-DS4",	"",	"",	VBE_POST|VBE_MODE },
	/* Michal Sedkowski <msedkowski@gmail.com>,  VBE_POST (Michal Vyskocil <michal.vyskocil@gmail.com>), VBE_POST|VBE_MODE (Danny Kukawka <dkukawka@suse.de>), Hendrik Tews <tews@cs.ru.nl> "HP Compaq nc6320 (RH367ET*" */
	{ "Hewlett-Packard",	"",	"",	"68YDU*",	0 },
	/* Michael Meskes <meskes@debian.org>, this is a NX 7400 */
	{ "Hewlett-Packard",	"",	"",	"68YGU*",	VBE_POST },
	/* Jan Adlen <janne@rosehip.mine.nu> */
	{ "Hewlett-Packard",	"Compaq nc6000 *",	"",	"68BDD*",	S3_BIOS|S3_MODE },
	/* */
	{ "Hewlett-Packard*",	"Compaq nx5000 *",	"",	"68BCU*",	VBE_POST|VBE_MODE },
	/* Philippe Marzouk <phil@ozigo.org> */
	{ "Hewlett-Packard ",	"Compaq nx 7010 (DU394T#*",	"",	"68BAL*",	S3_BIOS|S3_MODE },
	/* François Münch <francois.munch@free.fr> */
	{ "Hewlett-Packard ",	"Compaq nx 7010 (PG588EA#*",	"",	"",	S3_BIOS },
	/* "Zirkel, Timothy K" <zirkelt@lafayette.edu> */
	{ "Hewlett-Packard ",	"Compaq Presario X1000 (DK454AV)*",	"",	"",	S3_BIOS },
	/* Marcos Pinto <markybob@gmail.com>, HP dv6000 */
	{ "Hewlett-Packard",	"EW434AVABA *",	"",	"",	S3_BIOS|S3_MODE },
	/* hp compaq 2133, https://bugzilla.novell.com/show_bug.cgi?id=409246 */
	{ "Hewlett-Packard",	"HP 2133",	"",	"68VGU*",	S3_BIOS|S3_MODE },
	/* arideyla@web.de */
	{ "Hewlett-Packard",	"HP 550",	"",	"68MVU*",	VBE_POST|VBE_MODE },
	/* hp compaq 2133, https://bugzilla.novell.com/show_bug.cgi?id=363240 */
	{ "Hewlett-Packard",	"HP Compaq 2133",	"",	"68VGU*",	S3_BIOS|S3_MODE },
	/* Stefan Kulow https://bugzilla.novell.com/show_bug.cgi?id=362928 */
	{ "Hewlett-Packard",	"HP Compaq 2510p *",	"",	"68MSP*",	VBE_POST|VBE_MODE },
	/* Holger Sickenberg https://bugzilla.novell.com/show_bug.cgi?id=376715 */
	{ "Hewlett-Packard",	"HP Compaq 2710p *",	"",	"68MOU*",	VBE_POST|VBE_MODE },
	/* Jamil Djadala <djadala@datamax.bg> */
	{ "Hewlett-Packard",	"HP Compaq 6510b (GB863EA*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* peeetax@googlemail.com */
	{ "Hewlett-Packard",	"HP Compaq 6510b (KE134EA*",	"",	"68DDU*",	0 },
	/* Jamalulkhair Khairedin <jalut78@gmail.com>, tested on console and X */
	{ "Hewlett-Packard",	"HP Compaq 6520s",	"F.0A",	"68MDU Ver. F.0A",	0 },
	/* Michal Sojka <sojkam1@fel.cvut.cz> */
	{ "Hewlett-Packard",	"HP Compaq 6710b (GB893EA*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* Harry ten Berge <htenberge@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6710b (GB893ET*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* Michal Skrzypek <mskrzypek886@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6710b (GR681EA*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* Jaromrir Cervenka <cervajz@cervajz.com> */
	{ "Hewlett-Packard",	"HP Compaq 6710b (KE123EA*",	"",	"68DDU*",	0 },
	/* Jakub Talas <kuba.talas@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6710b (KE124EA*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* Alberto Gonzalez <luis6674@yahoo.com> */
	{ "Hewlett-Packard",	"HP Compaq 6710s (GC014ET*",	"",	"68DDU*",	VBE_POST|VBE_MODE },
	/* Darek Nawrocki <dnawrock@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6715b (GB834EA*",	"",	"68YTT*",	VBE_POST|VBE_MODE },
	/* Radu Benea <kitanatahu@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6715b (GB835EA*",	"",	"68YTT*",	S3_BIOS|S3_MODE },
	/* Andrey Petrov <apetrov87@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6715b (RK156AV*",	"",	"68YTT*",	S3_BIOS|S3_MODE },
	/* Matthias Hopf, https://bugzilla.novell.com/show_bug.cgi?id=390271, needs strange combination */
	{ "Hewlett-Packard",	"HP Compaq 6715b (RM174UT*",	"",	"68YTT*",	S3_BIOS|VBE_MODE },
	/* Petteri Tolonen <pretzel@mbnet.fi> works with radeonhd and fglrx */
	{ "Hewlett-Packard",	"HP Compaq 6715s (GR656ET*",	"",	"68YTT*",	S3_BIOS },
	/* Paul Smet <paul.smet@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6720s",	"",	"68MDU*",	VBE_POST|VBE_MODE },
	/* Babarovic Ivica <ivica.babarovic@asist.si> */
	{ "Hewlett-Packard",	"HP Compaq 6820s",	"",	"68MDD*",	S3_BIOS },
	/* Robert Norris <rob@cataclysm.cx>, tested with fglrx */
	{ "Hewlett-Packard",	"HP Compaq 6910p",	"",	"68MCD*",	S3_BIOS|S3_MODE },
	/* Daniele Frijia <daniele@sase.de>, this machine has no model number in DMI. BIOS F.14 */
	{ "Hewlett-Packard",	"HP Compaq 6910p",	"",	"68MCU*",	VBE_POST|VBE_MODE },
	/* Ola Widlund <olwix@yahoo.com>, works with proprietary ATI driver, to be verified without */
	{ "Hewlett-Packard",	"HP Compaq 6910p (GB951EA*",	"",	"68MCD*",	VBE_MODE },
	/* HP Compaq 6910p, Alexander Mikhailian <mikhailian@mova.org> */
	{ "Hewlett-Packard",	"HP Compaq 6910p (GH717AW#UUG)",	"F.15",	"68MCD Ver. F.15",	VBE_POST|VBE_MODE },
	/* Tomas Kejzlar <t.kejzlar@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq 6910p (RH241AV)",	"",	"68MCU*",	VBE_POST|VBE_MODE },
	/* Krisztian Loki <krisztian.loki@freemail.hu>, does not work with proprietary ATI driver */
	{ "Hewlett-Packard",	"HP Compaq 8510p ",	"",	"68MVD*",	VBE_POST|VBE_MODE },
	/* Milan Znamenacek <mznamenacek@retia.cz>, only from X */
	{ "Hewlett-Packard",	"HP Compaq 8710p (GC102EA*",	"",	"68MAD*",	0 },
	/* Tim Harris <tim.harris@redhillconsulting.com.au> */
	{ "Hewlett-Packard",	"HP Compaq 8710p (GT647PA*",	"",	"68MAD*",	0 },
	/* Marek Stopka <mstopka@opensuse.org>, kernel 2.6.25 */
	{ "Hewlett-Packard",	"HP Compaq dc5800 Small Form Factor",	"",	"786F2*",	0 },
	/* hp compaq nc2400, tested by seife. sometimes has keyboard problems after resume */
	{ "Hewlett-Packard",	"HP Compaq nc2400*",	"",	"68YOP*",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Compaq nc4200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Antti Laine <antti.a.laine@tut.fi>, "did not work with older BIOS", now has F.09  sys_ver="F.08": VBE_POST|VBE_SAVE, /* Rene Seindal <rene@seindal.dk> */
	{ "Hewlett-Packard",	"HP Compaq nc4400*",	"",	"68YHV*",	VBE_POST|VBE_MODE },
	/* there seem to be versions with a " " after Hewlett-Packard and without. */
	{ "Hewlett-Packard*",	"HP Compaq nc6000 *",	"",	"68BDD*",	S3_BIOS|S3_MODE },
	/* Winfried Dobbe, https://bugzilla.novell.com/show_bug.cgi?id=159688 */
	{ "Hewlett-Packard",	"HP Compaq nc6120 *",	"",	"",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Compaq nc6220*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* */
	{ "Hewlett-Packard",	"HP Compaq nc6230 *",	"",	"",	VBE_SAVE|NOFB },
	/* https://bugzilla.novell.com/show_bug.cgi?id=260451 */
	{ "Hewlett-Packard",	"HP Compaq nc6400 (EH522AV)",	"",	"68YCU*",	VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=295489 */
	{ "Hewlett-Packard",	"HP Compaq nc6400 (EY582ES*",	"",	"68YCU*",	VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=326526 */
	{ "Hewlett-Packard",	"HP Compaq nc6400 (RH560EA*",	"",	"68YCU*",	VBE_POST|VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Compaq nc8230*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Stefan Seyfried <seife@suse.de> novell bug 259831, this machine is only available in one configuration (ATI Graphics) */
	{ "Hewlett-Packard",	"HP Compaq nw8440 *",	"",	"68YVD*",	VBE_POST|VBE_MODE },
	/* Magnus Hoglund <magnus.hoglund@hallsberg.se> */
	{ "Hewlett-Packard",	"HP Compaq nw9440 *",	"",	"68YAF*",	0 },
	/* */
	{ "Hewlett-Packard*",	"hp compaq nx5000 *",	"",	"68BCU*",	VBE_POST|VBE_MODE },
	/* Ed Hoo <whoishoo@gmail.com> */
	{ "Hewlett-Packard*",	"hp Compaq nx5000 *",	"",	"68BCU*",	VBE_POST|VBE_MODE },
	/* <gabriel.pettier@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq nx6110 *",	"",	"68DTD",	VBE_POST|VBE_MODE },
	/* Niv aharonovich <niv.kaha@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq nx6120 *",	"",	"68DTD*",	VBE_POST|VBE_MODE },
	/* */
	{ "Hewlett-Packard",	"HP Compaq nx6125 *",	"",	"",	VBE_SAVE|NOFB },
	/* Pavel Machek */
	{ "Hewlett-Packard",	"HP Compaq nx6315",	"",	"68MOU*",	S3_BIOS|S3_MODE },
	/* Benedikt Nimmervoll <nimmervoll.benedikt@googlemail.com>, EY350EA#ABD also works with version RD115AA#ABA <hmacht@suse.de> */
	{ "Hewlett-Packard",	"HP Compaq nx6325 *",	"",	"68TT2*",	VBE_POST|VBE_SAVE },
	/* Michael Biebl <biebl@teco.edu> */
	{ "Hewlett-Packard ",	"HP compaq nx7000 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* Valent Turkovic <valent.turkovic@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq nx7300 (RU374ES#*",	"",	"",	VBE_POST|VBE_MODE },
	/* Slawomir Skrzyniarz <diodak@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq nx7400 (RH412EA#)",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "Hewlett-Packard",	"HP Compaq nx8220 *",	"",	"",	VBE_SAVE|NOFB },
	/* HP Compaq nx9420, Adam <ajett2@gmail.com> */
	{ "Hewlett-Packard",	"HP Compaq nx9420 (RB549UT#ABA)",	"F.1D",	"68YAF Ver. F.1D",	S3_BIOS|S3_MODE },
	/* Georg Walser <gwa@aspedi.com> Model DG286A */
	{ "Hewlett-Packard",	"HP d330 uT*",	"",	"",	0 },
	/* Ryan Bair <ryandbair@gmail.com> */
	{ "Hewlett-Packard",	"HP d530 SFF*",	"",	"",	0 },
	/* HP OmniBook 500, Oliver Redner <oliver.redner@arcor.de> */
	{ "Hewlett-Packard",	"HP OmniBook PC         ",	"HP OmniBook 500 FA     ",	"FA.M2.71",	0 },
	/* Frederic Mothe <mothe@nancy.inra.fr> */
	{ "Hewlett Packard",	"",	"HP OmniBook XE3 GC *",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "Hewlett Packard",	"",	"HP OmniBook XE3 GF *",	"",	VBE_POST|VBE_SAVE },
	/* Bernd Rinn <bb@rinn.ch> */
	{ "Hewlett Packard",	"",	"HP OmniBook XT1000 *",	"",	S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Pavilion dv1000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Tarek Soliman <tarek-spam@zeusmail.bounceme.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (GA534UA#*",	"",	"",	0 },
	/* S.Çağlar Onur <caglar@pardus.org.tr>, works only from X! */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (GF759EA#*",	"",	"",	0 },
	/* Daniel Forrer <daniel.forrer@unipd.it> and Jimmi <jommi@7girello.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (RN066EA#*",	"",	"",	0 },
	/* Robert Hart <Robert.Hart@BuroHappold.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv2500 Notebook PC",	"",	"",	0 },
	/* HP Pavilion dv2, Mike Leimon <leimon@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv2 Notebook PC     ",	"F.03    ",	"F.03    ",	VBE_POST|VBE_SAVE },
	/* Benjamin Kudria <ben@kudria.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (EW434AV#*",	"",	"",	S3_BIOS|S3_MODE },
	/* Carlos Andrade <crandrad@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (EY798AV*",	"",	"",	0 },
	/* Valent Turkovic <valent.turkovic@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GA378UA#*",	"",	"",	S3_MODE },
	/* Marlin Forbes <marlinf@datashaman.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GF688EA*",	"",	"",	0 },
	/* Eric Brasseur <eric.brasseur@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GH907EA#*",	"",	"",	S3_MODE },
	/* Antony Dovgal <tony2001@phpclub.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RM469EA#*",	"",	"",	0 },
	/* Jens Kaiser <kaiser@caltech.edu>, maybe only with binary nvidia module? */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RP153UA#*",	"",	"",	S3_BIOS|S3_MODE },
	/* John Soros <sorosj@gmail.com> "HP dv6120ea" */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RP980EA#*",	"",	"",	0 },
	/* Fran Firman <fran@ipsm.net.nz>, no hacks, maybe only works due to the Nvidia driver?  Marco Nenciarini <mnencia@prato.linux.it> needs S3_BIOS */
	{ "Hewlett-Packard",	"HP Pavilion dv6500 Notebook PC *",	"",	"",	S3_BIOS },
	/* fsuzzi@libero.it, dv6855el */
	{ "Hewlett-Packard",	"HP Pavilion dv6700 Notebook PC *",	"",	"",	S3_BIOS|S3_MODE },
	/* Wael Nasreddine <wael.nasreddine@gmail.com */
	{ "Hewlett-Packard",	"HP Pavilion dv7 Notebook PC*",	"",	"",	0 },
	/* Bengt Gorden <bengan@bag.org> */
	{ "Hewlett-Packard",	"HP Pavilion dv9700 Notebook PC *",	"",	"",	0 },
	/* <admin@tomobiki.dyndns.org> hp pavilion xz275 */
	{ "Hewlett-Packard",	"HP Pavilion Notebook PC",	"HP Pavilion Notebook *",	" IC.M1.02",	0 },
	/* <veesah@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion tx1000 Notebook PC *",	"",	"",	0 },
	/* Chris Polderman <chris.polderman@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion ze2000 (EK791EA#*",	"",	"",	0 },
	/* Fuad Tabba <fuad@cs.auckland.ac.nz>, only available with integr. gfx */
	{ "Hewlett-Packard ",	"hp pavilion ze4900 (*",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Pavilion zt3000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Tablet PC TR1105*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"HP Tablet PC Tx1100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* HP xw4300 Workstation, Florian Reitmeir <florian@reitmeir.org> */
	{ "Hewlett-Packard",	"HP xw4300 Workstation",	" ",	"786D3 v01.11",	0 },
	/* tested by HP https://bugzilla.novell.com/show_bug.cgi?id=436284 */
	{ "Hewlett-Packard",	"HP xw4550 Workstation",	"",	"786F7*",	0 },
	/* tested by HP https://bugzilla.novell.com/show_bug.cgi?id=436284 */
	{ "Hewlett-Packard",	"HP xw6600 Workstation",	"",	"786F4*",	0 },
	/* tested by HP https://bugzilla.novell.com/show_bug.cgi?id=436284 */
	{ "Hewlett-Packard",	"HP xw8600 Workstation",	"",	"786F5*",	0 },
	/* tested by HP https://bugzilla.novell.com/show_bug.cgi?id=436284 */
	{ "Hewlett-Packard",	"HP xw9400 Workstation",	"",	"786D6*",	0 },
	/* Damien Raude-Morvan <drazzib@drazzib.com>, This is a dv4052ea, i915GM */
	{ "Hewlett-Packard",	"Pavilion dv4000 (EB911EA#*",	"",	"",	S3_BIOS },
	/* Fabio Comolli <fabio.comolli@gmail.com>, dv4378ea, ATI X700 */
	{ "Hewlett-Packard",	"Pavilion dv4000 (ES973EA#*",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* Daniel L Wells <daniel@bigmalletman.com> */
	{ "Hewlett-Packard",	"Pavilion dv4000 (PX306UA#*",	"",	"",	S3_BIOS|S3_MODE },
	/* Wally Valters <deepsky99@gmail.com> */
	{ "Hewlett-Packard",	"Pavilion dv5000 (EZ535UA#*",	"",	"",	0 },
	/* HP Pavilion dv5244eu, Adrian Connolly <aconnolly08@qub.ac.uk> */
	{ "Hewlett-Packard",	"Pavilion dv5000 (RA648EA#ABU)     ",	"F.42",	"F.42",	0 },
	/* Arun Gupta <axgupta1@gmail.com> */
	{ "Hewlett-Packard",	"Pavilion dv6000 (EY798AV#*",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "Hewlett-Packard",	"Pavilion zd7000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Etienne URBAH <etienne.urbah@free.fr> */
	{ "Hewlett-Packard",	"Pavilion zd8000 (EL030EA#*",	"",	"",	VBE_POST },
	/* Karthik <molecularbiophysics@gmail.com> */
	{ "Hewlett-Packard ",	"Pavilion zt3300 *",	"",	"68BAL*",	S3_BIOS },
	/* Frank Schröder <frank.schroeder@gmail.com>, zt3380us */
	{ "Hewlett-Packard ",	"Pavilion zt3300 (PF083UA#*",	"",	"",	0 },
	/* Jari Turkia <jmjt@lut.fi> */
	{ "Hewlett-Packard*",	"Presario 2100 (DP835E)*",	"",	"",	0 },
	/* Frederic Koehler <fkfire@gmail.com> */
	{ "Hewlett-Packard*",	"Presario 2200 (PR309UA#*",	"",	"",	0 },
	/* Matto Marjanovic <maddog@mir.com> */
	{ "Hewlett-Packard",	"Presario C500 (RZ341UA#*",	"",	"",	0 },
	/* Igor Tamara <igor@tamarapatino.org> */
	{ "Hewlett-Packard",	"Presario C700 (GV681LA#*",	"",	"",	0 },
	/* Robert Gomulka <r.gom1977@gmail.com>*/
	{ "Hewlett-Packard",	"Presario F500 (GF596UA#*",	"",	"",	0 },
	/* Fatih Alabas https://bugzilla.novell.com/show_bug.cgi?id=230528 */
	{ "Hewlett-Packard",	"Presario M2000 (EQ547PA#*",	"",	"",	S3_BIOS|VBE_MODE },
	/* Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Hewlett-Packard ",	"Presario R3000 *",	"",	"",	S3_BIOS|S3_MODE },
	/* Arthur Peters <amp@singingwizard.org> */
	{ "Hewlett-Packard*",	"Presario R3200 *",	"",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "Hewlett-Packard",	"Presario R4100 *",	"",	"",	S3_BIOS|S3_MODE },
	/* iamnoah@gmail.com */
	{ "Hewlett-Packard",	"Presario V2000 (EP381UA#*",	"",	"",	S3_BIOS },
	/* Cristian S. Rocha  crocha@dc.uba.ar */
	{ "Hewlett-Packard",	"Presario V3000 (EZ674UA#*",	"",	"",	S3_BIOS|S3_MODE },
	/* JEROME HAYNES-SMITH <jerome.smith@btinternet.com> */
	{ "Hewlett-Packard",	"Presario V3000 (EZ755UA#*",	"",	"",	0 },
	/* From: "Lim Chuen Ee, Mike" <chuenee@starhub.net.sg> */
	{ "Hewlett-Packard",	"Presario V3000 (RL377PA#*",	"",	"",	S3_BIOS|S3_MODE },
	/* Adi Nugroho https://bugzilla.novell.com/show_bug.cgi?id=364456 */
	{ "Hewlett-Packard",	"Presario V3700 Notebook PC",	"",	"",	S3_BIOS|S3_MODE },
	/* Michael Kimsal <mgkimsal@gmail.com> */
	{ "Hewlett-Packard",	"Presario V5000 (ET826UA#*",	"",	"",	0 },
	/* Battle Nick <Nick.Battle@uk.fujitsu.com>, also a nc6120 */
	{ "Hewlett-Packard",	"PY507ET#*",	"",	"68DTD*",	VBE_POST|VBE_MODE },
	/* Benjamin Cherian <benjamin.cherian@gmail.com> */
	{ "HP Pavilion 061",	"D7223K-ABA A650E",	"",	"",	0 },
	/* A desktop with integrated graphics, Bo Thysell <thysell@fusemail.com> */
	{ "HP Pavilion 061",	"EJ192AA-ABS t3250.se",	"",	"",	0 },
	/* Michael Zaugg <dev.miguel@netzbox.ch> */
	{ "HP Pavilion 061",	"EP198AA-UUZ t3335.ch",	"",	"",	0 },
	/* Stefan Friedrich <strfr@web.de> */
	{ "HP Pavilion 061",	"ES061AA-ABD t3320*",	"",	"",	0 },
	/* Levi Larsen <levi.larsen@gmail.com> */
	{ "HP Pavilion 061",	"PC098A-ABA M1070N",	"",	"",	0 },
	/* Philippe LEBOURG <ph.l@wanadoo.fr> */
	{ "HP-Pavilion",	"NB965AA-ABF s3715fr",	" ",	"5.08 ",	0 },
	/* Arun Gupta <axgupta1@gmail.com>, works only with nvidia driver :( */
	{ "HP-Pavilion",	"RP829AV-ABA d4790y",	"",	"",	0 },
	/* Gijs van Gemert <g.v.gemert@inter.nl.net> */
	{ "HP-Pavilion",	"RZ418AA-ABH s3020*",	"",	"",	VBE_MODE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1830*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1831*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1832*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1833*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* ThinkPad R50e, Ferenc Wagner <wferi@niif.hu> */
	{ "IBM",	"1834S5G",	"ThinkPad R50e",	"1WET90WW (2.10 )",	0 },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1836*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1840*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R50/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1841*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1846*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1847*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1848*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1849*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1850*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52, reported by Joscha Arenz */
	{ "IBM",	"1860*",	"",	"",	S3_BIOS|S3_MODE },
	/* X41 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1864*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X41 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1865*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R52 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1870*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1871*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1872*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1873*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1874*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1875*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T43 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"1876*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T30 */
	{ "IBM",	"2366*",	"",	"",	RADEON_OFF },
	/* X40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2369*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2370*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X40 confirmed by Christian Deckelmann */
	{ "IBM",	"2371*",	"ThinkPad X40",	"",	S3_BIOS|S3_MODE },
	/* X40, Confirmed by Ville Kov�cs (From X and console) */
	{ "IBM",	"2372*",	"ThinkPad X40",	"",	S3_BIOS },
	/* T42p confirmed by Joe Shaw, T41p by Christoph Thiel (both 2373) */
	{ "IBM",	"2373*",	"",	"",	S3_BIOS|S3_MODE },
	/* T41p, Stefan Gerber */
	{ "IBM",	"2374*",	"",	"",	S3_BIOS|S3_MODE },
	/* T40/T41/T42/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2375*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T42, Bjorn Mork <bjorn@mork.no> */
	{ "IBM",	"2376*",	"",	"",	S3_BIOS|S3_MODE },
	/* T40/T41/T42/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2378*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T40/T41/T42/p imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2379*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2382*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X40, Andrew Pimlott <andrew@pimlott.net> */
	{ "IBM",	"2386*",	"",	"",	S3_BIOS|S3_MODE },
	/* G40 confirmed by David Härdeman */
	{ "IBM",	"2388*",	"",	"",	0 },
	/* Z60t, Sascha Hunold <sascha_hunold@gmx.de> */
	{ "IBM",	"2511*",	"",	"",	S3_BIOS|S3_MODE },
	/* X41, Florian Ragwitz <rafl@debian.org>, */
	{ "IBM",	"2525*",	"",	"",	S3_BIOS|S3_MODE },
	/* X41, Johannes Zellner <johannes@zellner.org> */
	{ "IBM",	"2526*",	"",	"",	S3_BIOS|S3_MODE },
	/* X41 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2527*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X41 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2528*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Z60m, reported by Arkadiusz Miskiewicz */
	{ "IBM",	"2529*",	"",	"",	S3_BIOS|S3_MODE },
	/* A21m, Raymund Will */
	{ "IBM",	"2628*",	"",	"",	0 },
	/* A21p, Holger Sickenberg */
	{ "IBM",	"2629*",	"",	"",	0 },
	/* George says the newer BIOS INET36WW / kernel 2.6.22 works fine with these options / without X  VBE_POST|VBE_MODE, George Tellalov <gtellalov@emporikilife.gr> with X */
	{ "IBM",	"2645*",	"",	"",	VBE_SAVE },
	/* T23 Gernot Schilling <gernotschilling@gmx.de> */
	{ "IBM",	"2647*",	"",	"",	S3_BIOS|S3_MODE },
	/* T21 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2648*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* A30, Axel Braun. https://bugzilla.novell.com/show_bug.cgi?id=309742 Backlight stays on, but RADEON_OFF kills the machine on second suspend :-( */
	{ "IBM",	"2652*",	"",	"",	S3_BIOS|S3_MODE },
	/* A31p, Till D"orges <td@pre-secure.de> */
	{ "IBM",	"2653*",	"",	"",	RADEON_OFF },
	/* R32 */
	{ "IBM",	"2658*",	"",	"",	0 },
	/* X22, confirmed by Richard Neill */
	{ "IBM",	"2662*",	"",	"",	S3_BIOS|S3_MODE|RADEON_OFF },
	/* X31, confirmed by Bjoern Jacke */
	{ "IBM",	"2672*",	"",	"",	S3_BIOS|S3_MODE|RADEON_OFF },
	/* X31, Vladislav Korecky <v.korecky@gmail.com>, bios_version="1QET73WW (2.11 )" */
	{ "IBM",	"2673*",	"",	"",	S3_BIOS|S3_MODE },
	/* R40 */
	{ "IBM",	"2681*",	"",	"",	0 },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2682*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2683*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2692*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2693*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2696*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2698*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2699*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 */
	{ "IBM",	"2722*",	"",	"",	0 },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2723*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2724*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* X30 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2884*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	/* X30 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2885*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	/* G41, Steven Rostedt <rostedt@goodmis.org> */
	{ "IBM",	"2886*",	"",	"",	VBE_POST|VBE_MODE },
	/* X30 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2890*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	/* X30 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2891*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	/* R40 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"2897*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* T23 imported from acpi-support 0.59 "half known". */
	{ "IBM",	"475S*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* R51 confirmed by Christian Zoz and Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "IBM",	"",	"ThinkPad R51",	"",	0 },
	/* R51e, reported by nemec.jiri@gmail.com */
	{ "IBM",	"",	"ThinkPad R51e",	"",	S3_BIOS|S3_MODE },
	/* Joerg Platte jplatte@naasa.net, again emtpy sys_product... */
	{ "IBM",	"       ",	"ThinkPad T40 ",	"1RETDRWW*",	0 },
	/* T43p reported by Magnus Boman */
	{ "IBM",	"",	"ThinkPad T43p",	"",	S3_BIOS|S3_MODE },
	/* Some T43's reported by Sebastian Nagel */
	{ "IBM",	"",	"ThinkPad T43",	"",	S3_BIOS|S3_MODE },
	/* */
	{ "IBM",	"",	"ThinkPad X32",	"",	RADEON_OFF|S3_BIOS|S3_MODE },
	/* Ralph Thormann, notice the empty sys_product :-( */
	{ "IBM",	"       ",	"ThinkPad X40",	"1UETD2WW*",	0 },
	/* X41 Tablet, tested by Danny Kukawka */
	{ "IBM",	"",	"ThinkPad X41 Tablet",	"",	S3_BIOS|S3_MODE },
	/* Andrey Melentyev <andrey.melentyev@gmail.com> */
	{ "Infomash",	"RoverBook",	"",	"",	VBE_POST|VBE_MODE },
	/* Christian Beier <dontmind@freeshell.org> */
	{ "JVC",	"J2N       ",	"",	"",	VBE_SAVE },
	/* Dave Royal <dave@daveroyal.com>, R60e 32bit works with S3_MODE, 64bit needs VBE_MODE */
	{ "LENOVO",	"0657*",	"",	"",	S3_BIOS|VBE_MODE },
	/* Leo G <chudified@gmail.com> */
	{ "LENOVO",	"068928U",	"3000 N100 *",	"",	0 },
	/* AFAICT the 3000 C100 is only available with intel graphics Luc Levain <luc.levain@yahoo.co.uk>, works with old and new kernel. */
	{ "LENOVO",	"0761*",	"3000 C100 *",	"",	0 },
	/* Sebastian Schleehauf <S.Schleehauf@gmx.de>, 3000V100 */
	{ "LENOVO",	"0763*",	"LENOVO3000 V100",	"",	0 },
	/* these Lenovo 3000 N100 model 768 are available with different graphics chips, so i do not dare to wildcard anything here :-( https://bugzilla.novell.com/show_bug.cgi?id=299099 */
	{ "LENOVO",	"076804U",	"3000 N100 *",	"",	S3_BIOS|VBE_MODE },
	/* Bogdan <boogi77@o2.pl> */
	{ "LENOVO",	"076831G",	"3000 N100 *",	"",	0 },
	/* Tim Creech <dezgotspam@gmail.com> */
	{ "LENOVO",	"076835U",	"3000 N100 *",	"",	0 },
	/* Aviv Ben-Yosef <aviv.by@gmail.com> */
	{ "LENOVO",	"07686VG",	"3000 N100 *",	"",	0 },
	/* Ted Percival <ted@midg3t.net> */
	{ "LENOVO",	"07687MM",	"3000 N100 *",	"",	S3_MODE },
	/* Johannes Rohr <jorohr@gmail.com> */
	{ "LENOVO",	"0768BYG",	"3000 N100 *",	"",	0 },
	/* I'm not sure how to handle the 3000 N200 models... Thorsten Frey <thorsten@tfrey.de> reported no options, michael.monreal@gmail.com reports -a3 */
	{ "LENOVO",	"0769AC6",	"3000 N200 *",	"",	S3_BIOS|S3_MODE },
	/* Moritz Schaefer <mollitz@googlemail.com> */
	{ "LENOVO",	"0769AH9",	"3000 N200 *",	"",	0 },
	/* Lenovo 3000 N200, Kyle Jones -- Hotelicom <kyle@hotelicom.com> */
	{ "LENOVO",	"0769AVU",	"3000 N200                       ",	"68ET31WW",	0 },
	/* Hans Gunnarsson <hans.gunnarsson@gmail.com> */
	{ "LENOVO",	"0769B9G",	"3000 N200 *",	"",	0 },
	/* Stojan Jakotic <stojanjakotic@centrum.cz> */
	{ "LENOVO",	"0769BBG",	"3000 N200 *",	"",	S3_BIOS|S3_MODE },
	/* ozon <rootdesign@gmail.com> */
	{ "LENOVO",	"0769BEG",	"3000 N200 *",	"",	0 },
	/* X60 / X60s */
	{ "LENOVO",	"1702*",	"",	"",	S3_BIOS|S3_MODE },
	/* X60 / X60s */
	{ "LENOVO",	"1704*",	"",	"",	S3_BIOS|S3_MODE },
	/* X60 / X60s */
	{ "LENOVO",	"1706*",	"",	"",	S3_BIOS|S3_MODE },
	/* T60p, Stephane Liardet <sliardet@gmail.com> */
	{ "LENOVO",	"2007*",	"",	"",	S3_BIOS|S3_MODE },
	/* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",	"2478*",	"",	"",	S3_BIOS|S3_MODE },
	/* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",	"2479*",	"",	"",	S3_BIOS|S3_MODE },
	/* Lenovo T400, Chris Cain <zshazz@gmail.com> */
	{ "LENOVO",	"2764CTO",	"ThinkPad T400",	"7UET43WW (1.13 )",	VBE_POST|VBE_MODE },
	/* IBM T400, Erez Zarum <erez@erezzarum.com> */
	{ "LENOVO",	"2768EB7",	"ThinkPad T400",	"7UET71WW (3.01 )",	VBE_SAVE|VBE_MODE|PCI_SAVE },
	/* T61 / R61 2008-01-29, bug https://bugzilla.novell.com/show_bug.cgi?id=334522#c10 */
	{ "LENOVO",	"4108*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 / R61 2008-01-29, bug https://bugzilla.novell.com/show_bug.cgi?id=334522#c10 */
	{ "LENOVO",	"4141*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 / R61 2008-01-29, bug https://bugzilla.novell.com/show_bug.cgi?id=334522#c10 */
	{ "LENOVO",	"4143*",	"",	"",	S3_BIOS|S3_MODE },
	/* ThinkCentre M57p, <Joerg.Breuninger@de.bosch.com>, intel gfx, kernel 2.6.25.5 */
	{ "LENOVO",	"6088*",	"",	"",	0 },
	/* X60 Tablet, Carlos Lange, https://bugzilla.novell.com/show_bug.cgi?id=265613#c12 */
	{ "LENOVO",	"6363*",	"",	"",	S3_BIOS|S3_MODE },
	/* X60 Tablet, Gary Ekker, https://bugzilla.novell.com/show_bug.cgi?id=265613 */
	{ "LENOVO",	"6364*",	"",	"",	S3_BIOS|S3_MODE },
	/* X60 Tablet, Aaron Denney <wnoise@ofb.net>, needs at least BIOS "7JET25WW (1.10 )"*/
	{ "LENOVO",	"6365*",	"",	"",	S3_BIOS|S3_MODE },
	/*  Florian Wagner <florian@wagner-flo.net>, X60 Tablet */
	{ "LENOVO",	"6366*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6378*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6379*",	"",	"",	S3_BIOS|S3_MODE },
	/* Christian Borntraeger <borntraeger@de.ibm.com>, VBE_POST|VBE_MODE works without binary driver */
	{ "LENOVO",	"6457*",	"",	"",	VBE_POST|VBE_MODE },
	/* confirmed to work with VESA by Ryan Kensrod <rkensrud@novell.com> */
	{ "LENOVO",	"6458*",	"",	"",	VBE_POST|VBE_MODE },
	/* T61p, leighton 5 <ng1lei@gmail.com>, ... what's the difference to the other T61p? */
	{ "LENOVO",	"6459*",	"",	"",	0 },
	/* T61p, leighton 5 <ng1lei@gmail.com>, ... what's the difference to the other T61p? */
	{ "LENOVO",	"6460*",	"",	"",	S3_BIOS|VBE_MODE },
	/* T61 */
	{ "LENOVO",	"6463*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6464*",	"",	"",	S3_BIOS|S3_MODE },
	/* Amit Joshi <ajoshi@optonline.net>, T61 / No flags KyuHwa Lee <lee.kyuh@gmail.com> */
	{ "LENOVO",	"6465*",	"",	"",	S3_BIOS|VBE_MODE },
	/* T61 */
	{ "LENOVO",	"6466*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6467*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6468*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6471*",	"",	"",	S3_BIOS|S3_MODE },
	/* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",	"6476*",	"",	"",	S3_BIOS|S3_MODE },
	/* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",	"6477*",	"",	"",	S3_BIOS|S3_MODE },
	/* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",	"6478*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"6480*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7642*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7643*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7644*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7645*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7646*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7647*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7648*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7649*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7650*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7657*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61, Joe Nahmias <jello@debian.org>, debian bug #432018, 7658Q4U Frank Seidel <fseidel@suse.de> */
	{ "LENOVO",	"7658*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"7659*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"7660*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"7661*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 with NVidia card, https://bugzilla.novell.com/show_bug.cgi?id=290618 */
	{ "LENOVO",	"7663*",	"",	"",	S3_MODE },
	/* T61, Aku Pietikainen <aku.pietikainen@ee.oulu.fi>, works with console and X */
	{ "LENOVO",	"7664*",	"",	"",	0 },
	/* Rasmus Borup Hansen <rbh@math.ku.dk>, X61s reported 7666 w. S3_BIOS  Anders Engstrom <ankaan@gmail.com> reported 7666 w. S3_BIOS|S3_MODE */
	{ "LENOVO",	"7666*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61s */
	{ "LENOVO",	"7667*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61s */
	{ "LENOVO",	"7668*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61s, Hendrik-Jan Heins <hjh@passys.nl>. Probably does not restore the framebuffer mode correctly, but S3_MODE as well as VBE_MODE seem to fail sometimes on x86_64 on this machine :-( */
	{ "LENOVO",	"7669*",	"",	"",	S3_BIOS },
	/* X61s */
	{ "LENOVO",	"7670*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61s */
	{ "LENOVO",	"7671*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61, Marco Barreno <barreno@cs.berkeley.edu>, works without any hack | kix: S3_BIOS|S3_MODE */
	{ "LENOVO",	"7673*",	"",	"",	0 },
	/* X61, Holger Hetterich <hhetter@novell.com> */
	{ "LENOVO",	"7674*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61, Jan Grmela <grmela@gmail.com> */
	{ "LENOVO",	"7675*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61*/
	{ "LENOVO",	"7676*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61*/
	{ "LENOVO",	"7678*",	"",	"",	S3_BIOS|S3_MODE },
	/* X61*/
	{ "LENOVO",	"7679*",	"",	"",	S3_BIOS|S3_MODE },
	/* Thinkpad R61, Calvin Walton <calvin.walton@gmail.com>, | kix: probably needs S3_MODE */
	{ "LENOVO",	"7732*",	"",	"",	S3_BIOS },
	/* R61 */
	{ "LENOVO",	"7733*",	"",	"",	S3_BIOS|S3_MODE },
	/* R61 */
	{ "LENOVO",	"7734*",	"",	"",	S3_BIOS|S3_MODE },
	/* R61 */
	{ "LENOVO",	"7735*",	"",	"",	S3_BIOS|S3_MODE },
	/* R61 */
	{ "LENOVO",	"7736*",	"",	"",	S3_BIOS|S3_MODE },
	/* R61 */
	{ "LENOVO",	"7737*",	"",	"",	S3_BIOS|S3_MODE },
	/* Thinkpad R61, <dancingseb@yahoo.de> */
	{ "LENOVO",	"7738*",	"",	"",	S3_BIOS|S3_MODE },
	/* Stephane Delcroix https://bugzilla.novell.com/show_bug.cgi?id=362945 */
	{ "LENOVO",	"7762*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad X61 Tablet https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7763*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad X61 Tablet https://bugzilla.novell.com/show_bug.cgi?id=353896 or VBE_POST|VBE_MODE  Baylis Shanks <bshanks3@hotmail.com>, possible problems with video */
	{ "LENOVO",	"7764*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad X61 Tablet https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7767*",	"",	"",	S3_BIOS|S3_MODE },
	/* Danny Kukawka <dkukawka@suse.de>  ThinkPad X61 Tablet */
	{ "LENOVO",	"7768*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad X61 Tablet https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"7769*",	"",	"",	S3_BIOS|S3_MODE },
	/* ThinkCentre M55, Mansfield Jordan <mjordan10@ec.rr.com> */
	{ "LENOVO",	"8808*",	"",	"",	S3_BIOS|S3_MODE },
	/* R61 Discrete moritz.rogalli@student.uni-tuebingen.de, works with and without nvidia drivers */
	{ "LENOVO",	"8889*",	"",	"",	0 },
	/* T61, intel card <seife@suse.de> 32bit works with S3_MODE, but 64bit needs VBE_MODE */
	{ "LENOVO",	"8895*",	"",	"",	S3_BIOS|VBE_MODE },
	/* T61 */
	{ "LENOVO",	"8896*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8897*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8898*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8899*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8900*",	"",	"",	S3_BIOS|S3_MODE },
	/* Grzegorz Piektiewicz <gpietkie@proteus.pl>, R61, with nvidia driver */
	{ "LENOVO",	"8918*",	"",	"",	0 },
	/* <ruedt@physik.fu-berlin.de>, R61, 64bit, NVidia driver does not work */
	{ "LENOVO",	"8919*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8930*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8932*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8933*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8934*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8935*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8936*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8937*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8938*",	"",	"",	S3_BIOS|S3_MODE },
	/* T61 */
	{ "LENOVO",	"8939*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8942*",	"",	"",	S3_BIOS|S3_MODE },
	/* ThinkPad R61, peter.pille@tektronix.com */
	{ "LENOVO",	"8943*",	"",	"",	S3_BIOS|VBE_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8944*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8945*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8946*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8947*",	"",	"",	S3_BIOS|S3_MODE },
	/*ThinkPad R61 https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	{ "LENOVO",	"8948*",	"",	"",	S3_BIOS|S3_MODE },
	/* Rick van Hattem <Rick.van.Hattem@fawo.nl>, Z61p*/
	{ "LENOVO",	"9452*",	"",	"",	S3_BIOS|S3_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=210928, Thinkpad R60, 32bit? */
	{ "LENOVO",	"9456*",	"",	"",	S3_BIOS|S3_MODE },
	/* Xu Zhe <x2446421894@gmail.com> */
	{ "Lenovo",	"Soleil E660P *",	"",	"",	0 },
	/* From: gregor herrmann <gregor.herrmann@comodo.priv.at> */
	{ "LENOVO",	"",	"Thinkpad R60e",	"",	S3_BIOS|S3_MODE },
	/* Marcus Better <marcus@better.se>, this time with a capital "P" */
	{ "LENOVO",	"",	"ThinkPad R60",	"",	S3_BIOS|S3_MODE },
	/* T60p, had a report from a 2007* and a 2008* model */
	{ "LENOVO",	"",	"ThinkPad T60p",	"",	S3_BIOS|S3_MODE },
	/* T60 From: Jeff Nichols <jnichols@renkoo.net> and Kristofer Hallgren <kristofer.hallgren@gmail.com> */
	{ "LENOVO",	"",	"ThinkPad T60",	"",	S3_BIOS|S3_MODE },
	/* At least 1709 series thinkpads can be whitelisted by name, and we can probably delete entries above, too.... */
	{ "LENOVO",	"",	"ThinkPad X60",	"",	S3_BIOS|S3_MODE },
	/* Z61m From: Roderick Schertler <roderick@argon.org> */
	{ "LENOVO",	"",	"ThinkPad Z61m",	"",	S3_BIOS|S3_MODE },
	/* Paul Wilkinson <pwilko@gmail.com> */
	{ "LENOVO",	"",	"ThinkPad Z61p",	"",	S3_BIOS|S3_MODE },
	/* merkur@users.sourceforge.net LM50a */
	{ "LG Electronics",	"LM50-DGHE",	"",	"",	S3_BIOS|S3_MODE },
	/* jsusiluo@mappi.helsinki.fi */
	{ "LG Electronics",	"LW75-SGGV1",	"",	"",	S3_BIOS|S3_MODE },
	/* jsusiluo@mappi.helsinki.fi */
	{ "LG Electronics",	"M1-3DGBG",	"",	"",	S3_BIOS|S3_MODE },
	/* Neysan Schaefer <mail@neysan.de> */
	{ "LG Electronics",	"R500-U.CPCBG",	"",	"",	0 },
	/* simon kaucic <simon.kaucic@gmail.com> */
	{ "                    ",	"M37EW",	"None",	"",	S3_BIOS },
	/* seife */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-51E*",	"",	"",	VBE_POST|VBE_MODE },
	/* https://sourceforge.net/tracker/index.php?func=detail&aid=1601715&group_id=45483&atid=443088 */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-R2C*",	"0032",	"",	VBE_POST|VBE_MODE },
	/* Hanspeter Kunz <buzz.rexx@gmail.com> */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-R3E*",	"002",	"",	VBE_POST|VBE_MODE },
	/* Hanspeter Kunz <hkunz@ifi.uzh.ch> */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-R6A*",	"",	"",	VBE_POST|VBE_MODE },
	/* Itsuo HATONO <hatono@kobe-u.ac.jp> */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-R6M*",	"",	"",	VBE_POST|VBE_MODE },
	/* Michal Krasucki <mkrasucki@gmail.com> */
	{ "Matsushita Electric Industrial Co.,Ltd.",	"CF-W4G*",	"",	"",	0 },
	/* Joost Witteveen <joost@iliana.nl> */
	{ "MAXDATA",	"IMPERIO4045A *",	"",	"",	0 },
	/* Jan Gerrit <JanGerrit@Burg-Borgholz.de> */
	{ "MAXDATA",	"PRO600IW",	"",	"",	S3_BIOS|S3_MODE },
	/* Oleg Broytmann <phd@phd.pp.ru> */
	{ "MaxSelect",	"Mission_A330",	"",	"",	VBE_POST|VBE_SAVE },
	/* Nicolas HENRY <nicolas.hy@free.fr>, nvidia binary driver */
	{ "MEDION",	"M295M",	"",	"",	0 },
	/* Leszek Lesner <leszek.lesner@googlemail.com> Medion MD43100 */
	{ "MEDIONNB       ",	"WID2010        ",	"",	"",	VBE_POST|VBE_MODE },
	/* Ralf Auer <ralf.auer@physik.uni-erlangen.de>, tested both with nv and binary nvidia */
	{ "MEDIONNB       ",	"WIM 2000       ",	"",	"",	0 },
	/* Alberto Gonzalez <info@gnebu.es> */
	{ "MEDIONPC",	"MS-6714",	"",	"",	S3_BIOS|S3_MODE },
	/* Mike Galbraith <efault@gmx.de> needs X to get text console back */
	{ "MEDIONPC",	"MS-7012",	"",	"",	0 },
	/* Axel Braun <Axel.Braun@gmx.de> */
	{ "MEDION",	"WIM 2140",	"",	"",	0 },
	/* MSI Wind U100, fseidel@suse.de */
	{ "MICRO-STAR INTERNATIONAL CO., LTD",	"U-100",	"",	"",	S3_BIOS|S3_MODE },
	/* Frank Thomas <frank.thomas@uni-weimar.de> */
	{ "Micro-Star International",	"EX610",	"",	"",	VBE_POST|VBE_MODE },
	/* Baruch Even <baruch@ev-en.org> */
	{ "Micro-Star International",	"MS-1412",	"",	"",	VBE_POST|VBE_SAVE|NOFB },
	/* Enno Wein <ennowein@yahoo.com> */
	{ "Micro-Star International",	"PR210",	"MS-1222X",	"",	VBE_POST },
	/* Branislav Gajdos <brianzee@gmail.com> */
	{ "MICRO-STAR INT'L CO.,LTD",	"MS-1003",	"",	"",	0 },
	/* Andreas Kostyrka <andreas@kostyrka.org> */
	{ "MICRO-STAR INT'L CO.,LTD",	"MS-1012",	"",	"",	S3_BIOS },
	/* Matthijs Kooijman <matthijs@stdin.nl> */
	{ "MICRO-STAR INT'L CO.,LTD",	"MS-1013",	"",	"",	S3_BIOS },
	/* Gergely Csepany <cheoppy@gmail.com> */
	{ "MICRO-STAR INT'L CO.,LTD.",	"MS-1057",	"",	"",	S3_BIOS|S3_MODE },
	/* Armin Braunstein <armin.braunstein@gmx.at> */
	{ "                                ",	"                                ",	"                        ",	"MQ96510J.86A.0816.2006.0716.2308",	VBE_POST|VBE_MODE },
	/* MSI IM-945GSE, Martin Teufel <teufel@informatik.uni-frankfurt.de> */
	{ "MSI",	"A9830IMS",	"11",	"080015 ",	S3_BIOS|S3_MODE },
	/* Matej Tyc <matej.tyc@gmail.com> */
	{ "MSI",	"MS-6417",	"",	"",	0 },
	/* Petric Frank <pfrank@gmx.de> MSI K8T Neo-FIS2R */
	{ "MSI",	"MS-6702",	"",	"",	0 },
	/* uswsusp@faxm0dem.org */
	{ "MSI.",	"MS-7207PV",	"",	"",	0 },
	/* Nicholas T <ntung@ntung.com>, mainboard */
	{ "MSI",	"MS-7350",	"",	"",	0 },
	/* S.Çağlar Onur <caglar@pardus.org.tr>, only works from X */
	{ "MTC",	"Montara-GML ",	"FAB-2",	"",	0 },
	/* "Alex V. Myltsev" <avm@altlinux.ru */
	{ "NCA_GROUP_LTD",	"iRU_Notebook",	"0106",	"",	VBE_POST|VBE_MODE },
	/* Hugo Costelha */
	{ "NEC *",	"B7 *",	"",	"",	VBE_SAVE },
	/* Benoit Thiell <badzil@gmail.com> */
	{ "NEC COMPUTERS INTERNATIONAL",	"NEC Versa Premium",	"FAB-2",	"",	0 },
	/* Nicky Gerritsen <nicky@xept.nl> */
	{ "NEC Computers International *",	"VC2",	"",	"",	S3_BIOS|S3_MODE },
	/* Ronny Radke <ronnymichaelradke@web.de> medion MIM2080 */
	{ "NOTEBOOK",	"MIM2080",	"",	"",	S3_BIOS|S3_MODE },
	/* Jan Willies <jan@willies.info> medion md 96400 */
	{ "NOTEBOOK",	"SAM2000",	"0131",	"",	0 },
	/* Tomas Groth <tomasgroth@yahoo.dk> */
	{ "Notebook",	"SAM2010",	"1058",	"A1058AM7 V7.23",	VBE_SAVE },
	/* Thomas Braun <thomas.braun@newthinking.de>, rebranded Uniwill P53IN */
	{ "OEM",	"P53IN4",	"",	"",	0 },
	/* Wolf Geldmacher <wolf@womaro.ch> */
	{ "OQO",	"ZEPTO",	"00",	"",	VBE_POST|VBE_SAVE },
	/* Angelo Anfossi <aanfossi@gmail.com> "Easy Note A6010" */
	{ "Packard Bell NEC",	"00000000000000000000000",	"",	"A03 ",	0 },
	/* Per Waago <waago@stud.ntnu.no> */
	{ "Packard Bell NEC",	"00000000000000000000000",	"P820008416",	"",	0 },
	/* Florian Tham <ftham@unlimitedmail.org> Samsung P35 */
	{ "SAMSUNG",	"CoronaR",	"03RK",	"",	0 },
	/* Jeremie Delaitre <jeremie.delaitre@gmail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"Q35/Q36",	"",	"",	S3_BIOS|S3_MODE },
	/* Eduardo Robles Elvira <edulix@gmail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"R40/R41",	"",	"",	VBE_SAVE },
	/* Tim Fischer <mail0812@online.de> works with and without ATI binary driver */
	{ "SAMSUNG ELECTRONICS CO.,LTD",	"R50/R51 *",	"",	"",	0 },
	/* Daniel Koester <koester@math.uni-augsburg.de> does not yet work from text mode */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"R55S *",	"",	"",	0 },
	/* Pavel Machek, Samsung UMPC */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ1US",	"01ME",	"",	VBE_SAVE },
	/* Maximilian Mehnert <maximilian.mehnert@googlemail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ35S",	"",	"",	VBE_POST },
	/* Frank Ruell <frank@ruell.com> or J�r�mie <jeremie@lamah.info> no flags */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ45S70S",	"",	"",	S3_BIOS|S3_MODE },
	/* Jens Ott <jo@opteamax.de> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SR700",	"",	"",	0 },
	/* Konstantin Krasnov <krkhome@gmail.com>, only with kernel 2.6.24++ */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SX22S",	"07AJ",	"",	VBE_SAVE },
	/* Arnim Laeuger <arnim.laeuger@gmx.net> */
	{ "Samsung Electronics",	"NX05S*",	"",	"",	VBE_POST|VBE_MODE },
	/* Stephen Denham <stephen.denham@gmail.com> */
	{ "SAMSUNG Electronics",	"SENS V25",	"",	"",	VBE_POST|VBE_MODE },
	/* Gautier Hattenberger <ghattenb@laas.fr> */
	{ "Samsung Electronics",	"SM40S",	"",	"",	S3_BIOS|S3_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=290734 */
	{ "Samsung Electronics",	"SX20S",	"",	"",	VBE_POST|VBE_MODE },
	/* Martin Wilck <mwilck@arcor.de> */
	{ "Samsung Electronics",	"SX50S",	"Revision MP",	"",	VBE_POST },
	/* */
	{ "Samsung",	"SQ10",	"",	"",	VBE_POST|VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "SHARP Corporation",	"PC-MM20 Series*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* */
	{ "SHARP                           ",	"PC-AR10 *",	"",	"",	0 },
	/* <gregor.willms@web.de> & Frederic Boiteux <fboiteux@free.fr> */
	{ "Shuttle Inc",	"SD11V10",	"",	"",	S3_BIOS|S3_MODE },
	/* Tejun Heo, https://bugzilla.novell.com/show_bug.cgi?id=390127 */
	{ "Shuttle Inc",	"SD30V10",	"",	"",	S3_BIOS|S3_MODE },
	/* foren@nuxli.ch */
	{ "Shuttle Inc",	"SN41UV10",	"",	"",	VBE_POST|VBE_MODE },
	/* Thomas Peuss <thomas@peuss.de> */
	{ "Shuttle Inc",	"SN85V10",	"",	"",	0 },
	/* Russ Dill <russ.dill@gmail.com> */
	{ "Sony Corporation    ",	"PCG-F430(UC)        ",	"",	"",	0 },
	/* Brice Mealier <mealier_brice@yahoo.fr> */
	{ "Sony Corporation",	"PCG-FR215E*",	"",	"",	0 },
	/* Eric Wasylishen <ewasylishen@gmail.com>, really needs VBE_SAVE */
	{ "Sony Corporation",	"PCG-FX340*",	"",	"",	VBE_SAVE },
	/* Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Sony Corporation",	"PCG-FX405*",	"",	"",	S3_BIOS },
	/* Stefan Nobis <stefan@snobis.de> */
	{ "Sony Corporation",	"PCG-FX601*",	"",	"",	S3_BIOS|S3_MODE },
	/* Loki Harfagr <l0k1@free.fr> */
	{ "Sony Corporation",	"PCG-FX802*",	"",	"",	VBE_SAVE|VBE_MODE },
	/* Stefan Friedrich <strfr@web.de> */
	{ "Sony Corporation",	"PCG-FX805*",	"",	"",	VBE_POST|VBE_SAVE },
	/* Mattia Dongili <malattia@linux.it> */
	{ "Sony Corporation    ",	"PCG-GR7_K(J)        ",	"",	"",	0 },
	/* Marco Nenciarini <mnencia@prato.linux.it> */
	{ "Sony Corporation",	"PCG-GRT916Z*",	"",	"",	0 },
	/* */
	{ "Sony Corporation",	"PCG-GRT995MP*",	"",	"",	0 },
	/* Bruce Perens <bruce@perens.com> */
	{ "Sony Corporation",	"PCG-TR2A(UC)",	"",	"",	VBE_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "Sony Corporation",	"PCG-U101*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* mathieu bouillaguet <mathieu.bouillaguet@gmail.com> */
	{ "Sony Corporation",	"PCG-Z1RMP*",	"",	"",	VBE_POST|VBE_MODE },
	/* Tom Lear <tom@trap.mtview.ca.us> */
	{ "Sony Corporation",	"PCG-Z505HSK(UC)",	"",	"",	0 },
	/* jmj_sf@freenet.de */
	{ "Sony Corporation",	"PCV-RX612*",	"",	"",	S3_BIOS|S3_MODE },
	/* Daniel Sabanes Bove <daniel.sabanesbove@campus.lmu.de> */
	{ "Sony Corporation",	"VGN-A115B*",	"",	"",	0 },
	/* Dean Darlison <dean@dasco.ltd.uk>, maybe only with nvidia driver */
	{ "Sony Corporation",	"VGN-AR31S",	"",	"",	S3_BIOS|S3_MODE },
	/* Sony VGN-B1VP, M. Vefa Bicakci <bicave@superonline.com> */
	{ "Sony Corporation",	"VGN-B1VP(GB)",	"J000L04A",	"R0042G9",	S3_BIOS|S3_MODE },
	/* Daniel Morris <danielm@eCosCentric.com> */
	{ "Sony Corporation",	"VGN-B1XP*",	"",	"",	S3_BIOS },
	/* Dhananjaya Rao <phaze87@gmail.com> */
	{ "Sony Corporation",	"VGN-B55G(I)",	"",	"",	0 },
	/* Jorge Visca <xxopxe@gmail.com> */
	{ "Sony Corporation",	"VGN-C140G",	"",	"",	S3_BIOS|S3_MODE },
	/* Marc Collin https://bugzilla.novell.com/show_bug.cgi?id=410723 */
	{ "Sony Corporation",	"VGN-C240E",	"",	"",	S3_BIOS|S3_MODE },
	/* Tijn Schuurmans <t.schuurmans@hccnet.nl> */
	{ "Sony Corporation",	"VGN-C2S_G",	"",	"",	S3_BIOS },
	/* Gerhard Riener <gerhard.riener@gmail.com> */
	{ "Sony Corporation",	"VGN-C2S_H",	"",	"",	0 },
	/* David Durrleman <david.durrleman@ens.fr>, only from X :-( */
	{ "Sony Corporation",	"VGN-FE11M",	"",	"",	0 },
	/* Gert Huizenga <gert@hbrothers.nl> */
	{ "Sony Corporation",	"VGN-FE21B",	"",	"",	S3_BIOS },
	/* <lapseofreason0@gmail.com>, only from X, S3_MODE hangs the machine, all other leave the screen blank */
	{ "Sony Corporation",	"VGN-FE31M",	"",	"",	0 },
	/* Bernardo Freitas Paulo da Costa <bernardo.da-costa@polytechnique.edu> */
	{ "Sony Corporation",	"VGN-FE890E",	"",	"",	0 },
	/* */
	{ "Sony Corporation",	"VGN-FS115B",	"",	"",	S3_BIOS|S3_MODE },
	/* Olivier Saier <osaier@gmail.com>, apparently S3_BIOS locks the machine hard */
	{ "Sony Corporation",	"VGN-FS115Z",	"",	"",	S3_MODE },
	/* S.Çağlar Onur <caglar@pardus.org.tr> */
	{ "Sony Corporation",	"VGN-FS215B",	"",	"",	0 },
	/* Chris deLuXCios <deluxcios@hotmail.com> */
	{ "Sony Corporation",	"VGN-FS215M",	"",	"",	0 },
	/* Tim Felgentreff <timfelgentreff@gmail.com> */
	{ "Sony Corporation",	"VGN-FS485B",	"",	"",	S3_BIOS|S3_MODE },
	/* Joseph Smith <joe@uwcreations.com> */
	{ "Sony Corporation",	"VGN-FS660_W",	"",	"",	0 },
	/* phantomlrrp@gmail.com */
	{ "Sony Corporation",	"VGN-FS920",	"",	"",	S3_BIOS|S3_MODE },
	/* Rafal Milecki <zajec5@gmail.com> and Thomas Bayen <tbayen@bayen.de> */
	{ "Sony Corporation",	"VGN-FW11*",	"",	"",	VBE_POST|VBE_MODE },
	/* Stefano Avallone <stavallo@unina.it> */
	{ "Sony Corporation",	"VGN-FZ39VN",	"",	"",	0 },
	/* 03/08/2009, Sony Vaio VGN-N21Z_W, Raphael Scholer <raphael@scholer-online.de> */
	{ "Sony Corporation",	"VGN-N21Z_W",	"C3LNT6WK",	"R0070J4",	0 },
	/* Fredrik Roubert <roubert@df.lth.se> */
	{ "Sony Corporation",	"VGN-N320E",	"",	"",	0 },
	/* daryl@mail.bg */
	{ "Sony Corporation",	"VGN-SZ340P",	"",	"",	0 },
	/* Pedro M. S. Oliveira <pmsoliveira@gmail.com>, machine has two gfx cards, works only well with the intel, not with the nvidia */
	{ "Sony Corporation",	"VGN-SZ5XN_C",	"",	"",	0 },
	/* Michal Bozek <michal.bozek@gmail.com> */
	{ "Sony Corporation",	"VGN-SZ61XN_C",	"",	"",	S3_BIOS|S3_MODE },
	/* Anton Zahlheimer <anton.zahlheimer@landshut.org> */
	{ "Sony Corporation",	"VGN-T250P",	"",	"",	S3_BIOS|S3_MODE },
	/* Timo Hoenig <thoenig@suse.de> VGN-TX3HP */
	{ "Sony Corporation",	"VGN-TX3HP",	"",	"",	0 },
	/* Stephan Martin <stephan.martin@open-xchange.com> */
	{ "Sony Corporation",	"VGN-TX3XP_L",	"",	"",	S3_BIOS|S3_MODE },
	/* Danny Kukawka <dkukawka@suse.de>, TX5MN from Michael Matz <matz@novell.com> */
	{ "Sony Corporation",	"VGN-TX5MN_W",	"",	"",	VBE_MODE },
	/* rbm@math.mit.edu */
	{ "Sony Corporation",	"VGN-TX670P",	"",	"",	VBE_POST|VBE_MODE },
	/* */
	{ "Sony Corporation",	"VGN-TX770P",	"",	"",	VBE_POST|VBE_MODE },
	/* Pier Paolo Pittavino <pittavin@studenti.ph.unito.it>, Timo Hoenig <thoenig@suse.de> */
	{ "Sony Corporation",	"VGN-TZ11*",	"",	"",	0 },
	/* Raymond Russell <raymond@corvil.com> */
	{ "Sony Corporation",	"VGN-TZ170N",	"",	"",	S3_BIOS|S3_MODE },
	/* Hans Gunnarsson <hans.gunnarsson@gmail.com> */
	{ "Sony Corporation",	"VGN-TZ21XN_B",	"",	"",	0 },
	/* matt@thewaffleproject.com */
	{ "Sony Corporation",	"VGN-TZ37*",	"",	"",	S3_BIOS|S3_MODE },
	/* Jan P. O. Schuemann" <jan@hep1.phys.ntu.edu.tw> */
	{ "Sony Corporation",	"VGN-TZ91HS",	"",	"",	0 },
	/* Mattia Dongili <malattia@linux.it> */
	{ "Sony Corporation",	"VGN-UX50",	"",	"",	VBE_MODE },
	/* ISHIKAWA Mutsumi <ishikawa@hanzubon.jp> */
	{ "Sony Corporation",	"VGN-Z90S",	"",	"",	0 },
	/* ASUS Mainboard, also broken DMI strings... Luis Valino <valino@litec.csic.es> */
	{ "System manufacturer",	"P5KR",	"System Version",	"",	0 },
	/* ASUS Mainboard, Gary Andrew https://bugzilla.novell.com/show_bug.cgi?id=419813 */
	{ "System manufacturer",	"P5Q DELUXE",	"System Version",	"",	S3_BIOS|S3_MODE },
	/* Asus P4B266, s2ram -f -a 3 Olivier Berger <oberger@ouvaton.org> */
	{ "System Manufacturer",	"System Name",	"System Version",	"ASUS P4B266 ACPI BIOS Revision 1010",	S3_BIOS|S3_MODE },
	/* Pavol Otcenas <Pavol.Otcenas@2c.cz>, "Prestigio Nobile 159W" */
	{ "To Be Filled By O.E.M.",	"259IA1",	"To Be Filled By O.E.M.",	"080010 ",	S3_MODE },
	/* Petr Pudlak <naradapuri@gmail.com> */
	{ "To Be Filled By O.E.M.",	"MS-7210",	"100",	"",	0 },
	/* Kevin Fullerton <kevin.fullerton@shotgun-suicide.co.uk> */
	{ "TOSHIBA",	"EQUIUM A100",	"",	"",	S3_BIOS|S3_MODE },
	/* */
	{ "TOSHIBA",	"Libretto L5/TNK",	"",	"",	0 },
	/* */
	{ "TOSHIBA",	"Libretto L5/TNKW",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"libretto U100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Pavel Borzenkov <pavel.borzenkov@gmail.com>, Toshiba Portege 3490CT, works correctly for me */
	{ "TOSHIBA",	"P3490",	"",	"",	VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"P4000*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Hans-Hermann Jopski <hans@jopski.de> Tecra A2 PTA20E-4M301VGR */
	{ "TOSHIBA",	"Portable PC",	"Version 1.0",	"Version 1.20",	0 },
	/* a PORTEGE 100 :-( mykhal@centrum.cz */
	{ "TOSHIBA",	"Portable PC",	"Version 1.0",	"Version 1.60",	S3_MODE },
	/* this is a Toshiba Satellite 4080XCDT, believe it or not :-( */
	{ "TOSHIBA",	"Portable PC",	"Version 1.0",	"Version 7.80",	S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"PORTEGE A100*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"PORTEGE A200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Toshiba Portege A600-13Y, Gregor Zattler <grfz@gmx.de> */
	{ "TOSHIBA",	"PORTEGE A600",	"PPA61E-01700EGR",	"Version 1.50",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"PORTEGE M200*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Alan Mycroft <Alan.Mycroft@cl.cam.ac.uk> */
	{ "TOSHIBA",	"PORTEGE M300",	"",	"",	S3_MODE },
	/* Mathieu Lacage <mathieu.lacage@sophia.inria.fr> */
	{ "TOSHIBA",	"PORTEGE R100",	"",	"",	0 },
	/* hole@gmx.li */
	{ "TOSHIBA",	"PORTEGE R200",	"",	"",	S3_BIOS|S3_MODE },
	/* Bernhard Kausler <uabdc@stud.uni-karlsruhe.de> */
	{ "TOSHIBA",	"PORTEGE R500",	"",	"",	S3_BIOS|S3_MODE },
	/* Kai-Ove Pietsch <kai-ove.pietsch@web.de> */
	{ "TOSHIBA",	"Satellite 1130",	"",	"",	VBE_SAVE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"Satellite 1900*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* From: a.maurizi@isac.cnr.it */
	{ "TOSHIBA",	"Satellite A100",	"",	"",	S3_BIOS|S3_MODE },
	/* Eric Siegel <nticompass@gmail.com>, intel chipset, sys_version="PSAA8U-0MD02K" */
	{ "TOSHIBA",	"Satellite A105",	"",	"",	S3_BIOS|S3_MODE },
	/* Fabio Olivares <fabio.olivares@alice.it> sys_version  = "PSA10E-018EZ-6A" */
	{ "TOSHIBA",	"Satellite A10",	"",	"",	S3_BIOS|S3_MODE },
	/*  Jan Urbanski <j.urbanski@students.mimuw.edu.pl>, sys_version="PSAE0E-04600TPL" */
	{ "TOSHIBA",	"Satellite A200",	"",	"",	S3_BIOS|S3_MODE },
	/* Brian Conn <Brian_Conn@Brown.edu> sys_version="PSAE3U-087023" Andrew Barilla <andrew@exit66.com> "PSAE3U-07R025" */
	{ "TOSHIBA",	"Satellite A205",	"",	"",	S3_BIOS|S3_MODE },
	/* Gavin Kinsey <gavin.kinsey@gmail.com> */
	{ "TOSHIBA",	"Satellite A210",	"",	"",	S3_BIOS|S3_MODE },
	/* David Picella <dpicella@gmail.com> sys_version  = "PSAFGU-048002" */
	{ "TOSHIBA",	"Satellite A215",	"",	"",	S3_BIOS|S3_MODE|PCI_SAVE },
	/* Grzegorz Konopko <kolodko1@gmail.com> sys_version = "PSAK0E-00600GPL" */
	{ "TOSHIBA",	"Satellite A300D",	"",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "TOSHIBA",	"Satellite A30",	"",	"",	VBE_SAVE },
	/* TOSHIBA Satellite A45, Shawn A. Wilson <shawn@lannocc.com> */
	{ "TOSHIBA",	"Satellite A45",	"PSA40U-000HNV",	"Version 1.30",	0 },
	/* Diego Ocampo <diegoocampo8@gmail.com> */
	{ "TOSHIBA",	"Satellite A80",	"",	"",	0 },
	/* Christian Schneeman <cschneemann@suse.de> */
	{ "TOSHIBA",	"Satellite L10",	"",	"",	VBE_POST|VBE_MODE },
	/* Toshiba Satellite L305-S5955, Dan <daniel.kneezel@gmail.com> */
	{ "TOSHIBA",	"Satellite L305",	"PSLB8U-13T038",	"1.80",	VBE_POST|VBE_MODE },
	/* Gijs van Gemert <g.v.gemert@inter.nl.net> */
	{ "TOSHIBA",	"Satellite L30",	"",	"",	S3_BIOS|S3_MODE },
	/* Toshiba Satellite M100, Ji.� Appl <j.appl@seznam.cz> */
	{ "TOSHIBA",	"SATELLITE M100",	"PSMA0E-030014CZ",	"V1.60",	0 },
	/* Krzysztof Lubanski <luban@nerdshack.com>, sys_version = "PSMA0U-0F301U" */
	{ "TOSHIBA",	"Satellite M105",	"",	"",	S3_BIOS|S3_MODE },
	/* tnt.rocket@freenet.de, need to verify if S3_MODE is needed too */
	{ "TOSHIBA",	"Satellite M30X",	"",	"",	S3_BIOS },
	/* Michaell Gurski */
	{ "TOSHIBA",	"Satellite M35X",	"",	"",	S3_BIOS|S3_MODE },
	/* Mike Gilliam <mike.tx@suddenlink.net> */
	{ "TOSHIBA",	"Satellite M55",	"",	"",	VBE_POST|VBE_MODE },
	/* (BAD: clist@uah.es S3_BIOS|S3_MODE) */
	{ "TOSHIBA",	"Satellite M70",	"",	"",	0 },
	/* zu@ten.ch sys_version  = "PSPAGE-00H00RS4" */
	{ "TOSHIBA",	"Satellite P100",	"",	"",	0 },
	/* Antonis Tsolomitis <atsol@aegean.gr> */
	{ "TOSHIBA",	"Satellite P10",	"",	"",	S3_BIOS|S3_MODE },
	/* From: mire <mire@osadmin.com> */
	{ "TOSHIBA",	"Satellite P200",	"",	"",	S3_BIOS|S3_MODE },
	/* cl.kerr@gmail.com */
	{ "TOSHIBA",	"Satellite P205D",	"",	"",	S3_BIOS|S3_MODE },
	/* From: Jim <jim.t@vertigo.net.au> */
	{ "TOSHIBA",	"SatellitePro4600",	"",	"",	0 },
	/* Stephen Jarrett-Sprague <sjarrettsprague@googlemail.com> sys_version="PSAC1E-0J3021EN" */
	{ "TOSHIBA",	"Satellite Pro A120",	"",	"",	S3_BIOS|S3_MODE },
	/* Toshiba Satellite Pro M15, Mark Chace <markcha@chacefamily.org> */
	{ "TOSHIBA",	"Satellite Pro M15",	"PS630U-G205HV",	"Version 1.50",	0 },
	/* Francisco Palomo Lozano <francisco.palomo@uca.es> sys_version="PSM75E-00G00TSP" */
	{ "TOSHIBA",	"Satellite Pro M70",	"",	"",	S3_BIOS|S3_MODE },
	/* Neil Symington, https://bugzilla.novell.com/show_bug.cgi?id=386505, kernel 2.6.25-x86_64 */
	{ "TOSHIBA",	"Satellite Pro U300",	"",	"",	0 },
	/* Jean-Pierre Bergamin <james@ractive.ch>, kernel 2.6.25.3, sys_version "PLUA0E-01C007S4" */
	{ "TOSHIBA",	"SATELLITE U200",	"",	"",	0 },
	/* Robin James, <robin.james@gmail.com> sys_version="PSU30E-04L02QKS" */
	{ "TOSHIBA",	"SATELLITE U300",	"",	"",	0 },
	/* Romano Giannetti <romanol@upcomillas.es> sys_version "PSU34U-00L003" */
	{ "TOSHIBA",	"Satellite U305",	"",	"",	VBE_POST|VBE_MODE },
	/* Bram Senders <bram@luon.net> */
	{ "TOSHIBA",	"SP4600",	"Version 1.0",	"",	0 },
	/* Silverio Santos <SSantos@web.de>, Tecra 9000 */
	{ "TOSHIBA",	"T9000",	"",	"",	VBE_MODE },
	/* From: a v <adam_6515@hotmail.com> */
	{ "TOSHIBA",	"TECRA8100",	"",	"",	S3_MODE },
	/* Stefan Seyfried has one of those :-) S3_BIOS leads to "melting screen" */
	{ "TOSHIBA",	"TECRA 8200",	"",	"",	S3_MODE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"TECRA A2*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"TECRA A5*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Thomas Zander <zander@kde.org> sys_version "PTA83E-0E303LDU" */
	{ "TOSHIBA",	"TECRA A8",	"",	"",	0 },
	/* Arne Salveter <arne.salveter@googlemail.com> sys_version "PTS52E-00X00GGR" */
	{ "TOSHIBA",	"TECRA A9",	"",	"",	0 },
	/* imported from acpi-support 0.59 "half known". */
	{ "TOSHIBA",	"TECRA M2*",	"",	"",	VBE_POST|VBE_SAVE|UNSURE },
	/* Alexandru Romanescu https://bugzilla.novell.com/show_bug.cgi?id=398270 */
	{ "TOSHIBA",	"TECRA M4",	"",	"",	VBE_POST|VBE_MODE },
	/* Dr. Stephan Buehne <stephan.buehne@oracle.com> */
	{ "TOSHIBA",	"TECRA M9",	"",	"",	S3_MODE },
	/* Jad Naous <jnaous@stanford.edu> */
	{ "TOSHIBA",	"TECRA S1",	"",	"",	0 },
	/* Arjen Dragt <arjen_dragt@yahoo.ca> sys_version="PTS20C-SX501E" */
	{ "TOSHIBA",	"TECRA S2",	"",	"",	VBE_POST|VBE_SAVE },
	/* */
	{ "TOSHIBA",	"TECRA S3",	"",	"",	0 },
	/* Joachim Gleissner */
	{ "transtec AG",	"MS-1034",	"",	"",	S3_BIOS|S3_MODE },
	/* Gregory Colpart <reg@evolix.fr> 'Transtec Levio 210' */
	{ "transtec AG",	"MS-1057",	"",	"",	S3_BIOS|S3_MODE },
	/* Clevo D410J, Márton Németh <nm127@freemail.hu> */
	{ "VIA",	"K8N800",	"VT8204B ",	"",	VBE_POST|VBE_SAVE },
	/* Clevo D470K, Jasper Mackenzie <scarletpimpernal@hotmail.com> */
	{ "VIA",	"K8T400",	"VT8204B ",	"",	S3_BIOS|S3_MODE },
	/* Dolezal Lukas <dolezallukas@sssvt.cz> */
	{ "VIA Technologies, Inc.",	"KT400-8235",	"",	"",	S3_BIOS|S3_MODE },
	/* <dev_temp@ukr.net> */
	{ "VIA Technologies, Inc.",	"KT400A-8235",	"",	"",	S3_BIOS|S3_MODE },
	/* VIA EPIA M Mini-ITX Motherboard with onboard gfx, reported by Monica Schilling */
	{ "VIA Technologies, Inc.",	"VT8623-8235",	"",	"",	S3_MODE },
	/* Rene Seindal <rene@seindal.dk> */
	{ "ZEPTO",	"ZNOTE *",	"3215W *",	"",	0 },
	{ NULL, NULL, NULL, NULL, 0 }
};
