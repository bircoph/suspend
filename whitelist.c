/* whitelist.c
 * whitelist of machines that are known to work somehow
 * and all the workarounds
 */

#include "config.h"
#include <stdlib.h>
#include "whitelist.h"

char *whitelist_version = "$Id: whitelist.c,v 1.130 2008/07/24 22:23:46 tdykstra Exp $";

struct machine_entry whitelist[] = {
	{ "IBM",			"",		"ThinkPad X32",	"", RADEON_OFF|S3_BIOS|S3_MODE },
	/* Michael Wagner <michael-wagner@gmx.de> */
	{ "4MBOL&S",			"7521 *",	"REV. A0",	"", 0 },
	/* Alexander Wirt reported "VBE_SAVE", but Penny Leach <penny@she.geek.nz>
	   needs VBE_POST|VBE_MODE */
	{ "Apple Computer, Inc.",	"MacBook1,1",		"",	"", VBE_POST|VBE_MODE },
	/* Mildred <ml.mildred593@online.fr>, needs VBE_SAVE from console, but that slows resuming */
	{ "Apple Inc.",			"MacBook2,1",		"1.0",	"", 0 },
	/* Marvin Stark <marv@der-marv.de> */
	{ "Apple Computer, Inc.",	"MacBook2,1",		"",	"", VBE_POST },
	/* <skanabiz@gmail.com>  Kernel 2.6.26-rc9, probably with i910 */
	{ "Apple Inc.",			"MacBook3,1",		"1.0",	"", 0 },
	/* Julien BLACHE jb@jblache.org */
	{ "Apple Computer, Inc.",	"MacBookPro2,2",	"",	"   MBP22.88Z.00A5.B00.0610192105", VBE_POST|VBE_MODE },
	/* Brian Kroth <bpkroth@wisc.edu>, newer BIOS does not like VBE_POST? */
	{ "Apple Computer, Inc.",	"MacBookPro2,2",	"",	"", VBE_MODE },
	/* Tilman Vogel <tilman.vogel@web.de>, only works from X */
	{ "Apple Inc.",			"MacBookPro4,1",	"",	"", 0 },
	/* Klaus Ade Johnstad <klaus@skolelinux.no> */
	{ "Acer *",			"AcerPower 2000",	"",	"", 0 },
	/* Felix Rommel, https://bugzilla.novell.com/show_bug.cgi?id=228572 */
	{ "Acer,Inc.",			"Aspire 1350",		"",	"", VBE_POST|VBE_MODE|NOFB },
	/* Simon Bachmann <simonbachmann@bluewin.ch> works with proprietary ATI driver */
	{ "Acer *",			"Aspire 1500 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Andreas Wagner <A.Wagner@stud.uni-frankfurt.de> */
	{ "Acer",			"Aspire 1520",		"",	"", VBE_POST|VBE_MODE },
	/* Giorgio Lando <patroclo7@gmail.com> */
	{ "Acer, inc.",			"Aspire 1640     ",	"",	"", VBE_SAVE },
	/* Stefano Costa <steko@iosa.it> */
	{ "Acer, inc.",			"Aspire 1650     ",	"",	"", VBE_POST|VBE_MODE },
	{ "Acer, inc.",			"Aspire 1690     ",	"",	"", VBE_POST|VBE_SAVE },
	/* Eberhard Niendorf <Eberhard.Niendorf@t-online.de>, kernel 2.6.25.9 */
	{ "Acer *",			"Aspire 2920 *",	"",	"", 0 },
	/* Ricardo Garcia <sarbalap+freshmeat@gmail.com> */
	{ "Acer, inc.",			"Aspire 3000 *",	"",	"", VBE_POST|VBE_MODE },
	/* Marek Stopka (M4r3k) <marekstopka@gmail.com> */
	{ "Acer *",			"Aspire 3100 *",	"",	"", 0 },
	/* From: Bontemps Camille <camille.bontemps@gmail.com> */
	{ "Acer",			"Aspire 3610",		"",	"", S3_BIOS|S3_MODE },
	/* From: Saul Mena Avila <saul_2110@yahoo.com.mx> */
	{ "Acer",			"Aspire 3620",		"",	"", S3_BIOS|S3_MODE },
	/* From: Witmaster <witmaster@op.pl> */
	{ "Acer            ",		"Aspire 3690 *",	"",	"", S3_BIOS|S3_MODE },
	/* Carlos Corbacho <cathectic@gmail.com> */
	{ "Acer",			"Aspire 5020",		"",	"", VBE_POST },
	/* Jim Hague <jim.hague@acm.org> */
	{ "Acer, inc.",			"Aspire 5050 *",	"",	"", S3_BIOS },
	/* Fadain Tariq <fadain.tariq@gmail.com> reported S3_BIOS|S3_MODE,
	   but for Donald Curtis <donald@curtisliberty.de> only VBE_* works... */
	{ "Acer *",			"Aspire 5100 *",	"",	"", VBE_POST|VBE_MODE },
	/* Damyan Ivanov <dmn@debian.org> */
	{ "Acer *",			"Aspire 5110 *",	"",	"", VBE_POST|VBE_MODE },
	/* Paul Kretek <pk@oz4.org> */
	{ "Acer            ",		"Aspire 5500Z *",	"",	"", 0 },
	/* From: "cj.yap" <chuanren.ye@gmail.com> */
	{ "Acer, inc.",			"Aspire 5570 *",	"",	"", VBE_POST|VBE_MODE },
	/* <vanhtu1987@gmail.com>, probably only with nvidia driver :-( */
	{ "Acer, inc.",			"Aspire 5580 *",	"",	"", 0 },
	/* Vadim Dashkevich <vadim.dashkevich@gmail.com> */
	{ "Acer            ",		"Aspire 5610Z *",	"",	"", S3_BIOS|S3_MODE },
	/* Andrey Popov <potopi@gmx.net> */
	{ "Acer *",			"Aspire 5630 *",	"",	"", S3_BIOS|S3_MODE },
	/* Ronny Dahl <ronny.dahl@gmx.net> */
	{ "Acer, inc.",			"Aspire 5920 *",	"",	"", VBE_POST|VBE_MODE },
	/* Christine Upadek <5upadek@informatik.uni-hamburg.de> */
	{ "Acer",			"Extensa 2900",		"",	"", VBE_POST|VBE_MODE },
	/* Paolo Herms <paolo.herms@gmx.net> */
	{ "Acer",			"Extensa 3000 *",	"",	"", S3_BIOS|S3_MODE },
	/* seife */
	{ "Acer            ",		"Extensa 4150 *",	"",	"", S3_BIOS|S3_MODE },
	/* Xavier Douville <s2ram@douville.org> */
	{ "Acer            ",		"Extensa 5620 *",	"",	"", S3_BIOS|S3_MODE },
	/* Libor Plucnar <Plucnar.Libor@seznam.cz> */
	{ "Acer           ",		"Extensa 6220 *",	"",	"", S3_BIOS },
	/* Hannes Reinecke https://bugzilla.novell.com/show_bug.cgi?id=354728 */
	{ "Acer, inc.",			"Ferrari 1000    ",	"",	"", VBE_POST|VBE_SAVE },
	{ "Acer, inc.",			"Ferrari 4000    ",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Russ Dill <russ.dill@gmail.com> * does not work with fglrx, trenn@suse.de */
	{ "Acer, inc.",			"Ferrari 5000    ",	"",	"", VBE_POST|VBE_MODE },
	/* Sridar Dhandapani <sridar@gmail.com> */
	{ "Acer           ",		"TravelMate 230 ",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer           ",		"TravelMate 240 ",	"",	"", VBE_POST|VBE_SAVE },
	/* Sven Foerster <madman-sf@gmx.de> */
	{ "Acer, inc.",			"TravelMate C200 ",	"",	"", 0 },
	{ "Acer           ",		"TravelMate C300",	"",	"", VBE_SAVE },
	/* TravelMate 630Lci	Tim Dijkstra <tim@famdijkstra.org> */
	{ "Acer           ",		"TravelMate 630 ",	"",	"", S3_BIOS|S3_MODE },
	/* Norbert Preining */
	{ "Acer",			"TravelMate 650",	"",	"", VBE_POST|VBE_SAVE },
	/* Martin Sack, this one may be NOFB, to be verified */
	{ "Acer",			"TravelMate 800",	"",	"", VBE_POST },
	/* Arkadiusz Miskiewicz <arekm@maven.pl> */
	{ "Acer",			"TravelMate 2300",	"",	"", VBE_POST|VBE_MODE },
	/* Quentin Denis <quentin.denis@gmail.com> */
	{ "Acer, inc.",			"TravelMate 2310 ",	"",	"", VBE_POST|VBE_MODE },
	/* Arjan Oosting <arjanoosting@home.nl> */
	{ "Acer",			"TravelMate 2350 *",	"",	"", S3_BIOS|S3_MODE },
	/* jan sekal <jsekal@seznam.cz> */
	{ "Acer",			"TravelMate 2410",	"",	"", S3_BIOS|S3_MODE },
	/* Peter Stolz <p.stolz@web.de> */
	{ "Acer",			"TravelMate 2420",	"",	"", S3_BIOS|S3_MODE },
	/* Erki Ferenc <ferki@3rdparty.hu> */
	{ "Acer            ",		"TravelMate 2450 ",	"",	"", 0 },
	/* Vit Cepela <vitek@shar.cz> */
	{ "Acer            ",		"TravelMate 2490 ",	"",	"", S3_BIOS|S3_MODE },
	{ "Acer, inc.",			"TravelMate 3000 ",	"",	"", VBE_POST|VBE_SAVE },
	/* Norbert Preining <preining@logic.at>, kernel 2.6.23-rc2++ needed */
	{ "Acer, inc.",			"TravelMate 3010 ",	"",	"", VBE_POST|VBE_MODE },
	/* Momsen Reincke <mreincke@gmx.net> */
	{ "Acer, inc.",			"TravelMate 3020 ",	"",	"", VBE_POST|VBE_MODE },
	{ "Acer            ",		"TravelMate 3220 *",	"",	"", S3_BIOS|S3_MODE },
	/* Rohan Dhruva <rohandhruva@gmail.com> */
	{ "Acer, inc.",			"TravelMate 3260 ",	"",	"", VBE_POST|VBE_MODE },
	/* Andreas Juch <andreas.juch@tele2.at> */
	{ "Acer",			"TravelMate 4000",	"",	"", S3_BIOS|S3_MODE },
	/* From: LucaB <lbonco@tin.it> */
	{ "Acer, inc.",			"TravelMate 4100 ",	"",	"", VBE_POST|VBE_SAVE },
	/* gebu@seznam.cz */
	{ "Acer",			"TravelMate 4400",	"",	"", 0 },
	{ "Acer            ",		"TravelMate 4650 *",	"",	"", S3_BIOS|S3_MODE },
	/* Adrian C. <anrxc@sysphere.org> */
	{ "Acer *",			"TravelMate 5320 *",	"",	"", S3_BIOS|S3_MODE },
	/* <leva@ecentrum.hu> */
	{ "Acer",			"TravelMate 7520",	"",	"", S3_BIOS },
	/* Andrea Zucchelli <zukka77@gmail.com> EEE-PC 4G */
	{ "ASUSTeK Computer INC.",	"701",			"",	"", VBE_POST|VBE_MODE },
	/* From: Julien Puydt <jpuydt@free.fr> */
	{ "ASUSTeK Computer Inc.",	"A2D       ",		"",	"", S3_BIOS|S3_MODE },
	/* Cristian Rigamonti */
	{ "ASUSTEK ",			"A2H/L ",		"",	"", 0 },
	/* Thomas Thrainer <tom_t@gmx.at> */
	{ "ASUSTeK Computer Inc.*",	"A6G       ",		"",	"", 0 },
	/* Arnout Boelens <aboelens@engin.umass.edu> */
	{ "ASUSTeK Computer Inc. ",	"A6J *",		"",	"", VBE_POST|VBE_MODE },
	/* Piotr Drozdek <pioruns@o2.pl> */
	{ "ASUSTeK Computer Inc. ",	"A6JC *",		"",	"", 0 },
	/* Tom Gruben <tom.gruben@gmail.com>, maybe only with nvidia? */
	{ "ASUSTeK Computer Inc. ",	"A6K *",		"",	"", 0 },
	/* Patryk Zawadzki <patrys@icenter.pl> */
	{ "ASUSTeK Computer Inc. ",	"A6Km      ",		"",	"", 0 },
	/* Tom Malfrere <tom.malfrere@pandora.be> */
	{ "ASUSTeK Computer Inc. ",	"A6Kt      ",		"",	"", S3_BIOS|S3_MODE },
	/* Szalai Akos <szalakos@gmail.com> */
	{ "ASUSTeK Computer Inc. ",	"A6M",			"",	"", VBE_POST },
	/* Anthony Mahe <anthonymahe@gmail.com> */
	{ "ASUSTeK Computer INC. ",	"A6T",			"",	"", 0 },
	/* tias@ulyssis.org */
	{ "ASUSTeK Computer Inc. ",	"A6U       ",		"",	"", VBE_POST|VBE_MODE },
	/* Daniel Bumke <danielbumke@gmail.com> */
	{ "ASUSTeK Computer INC. ",	"A6VA      ",		"",	"", 0 },
	/* Alex Myltsev <avm@altlinux.ru> */
	{ "ASUSTeK Computer Inc. ",	"A8He *",		"",	"", S3_BIOS|S3_MODE },
	/* q0mw3fk02@sneakemail.com */
	{ "ASUSTeK Computer Inc. ",	"F3F *",		"",	"", S3_BIOS|S3_MODE },
	/* Julien Puydt <jpuydt@free.fr> works from console and with nvidia driver */
	{ "ASUSTeK Computer Inc. ",	"F3F *",		"",	"", 0 },
	/* Daniel Kinzler <daniel@brightbyte.de> */
	{ "ASUSTeK Computer Inc. ",	"F3JC *",		"",	"", S3_BIOS|S3_MODE },
	/* Marco Jorge <mbernardesjorge@users.sourceforge.net> */
	{ "ASUSTeK Computer Inc. ",	"F3JP *",		"",	"", VBE_POST|VBE_MODE },
	/* Anghinolfi Luca <anghi83@gmail.com> */
	{ "ASUSTeK Computer INC. *",	"F3Sg *",		"",	"", 0 },
	/* Vladimir Pouzanov <farcaller@gmail.com>, probably only with nvidia and from X */
	{ "ASUSTeK Computer INC. ",	"F3T",			"",	"", 0 },
	/* Enrico Zini <enrico@enricozini.org> */
	{ "ASUSTeK Computer Inc. *",	"F9E *",		"",	"", 0 },
	/* Paul Cousins <namain@gmail.com> */
	{ "ASUSTeK Computer Inc. *",	"G1S *",		"",	"", 0 },
	/* Asus Mainboard, Ian McAnena <ianegg@gmail.com> */
	{ "ASUSTeK Computer Inc.",	"K8V-MX",		"",	"", 0 },
	{ "ASUSTEK ",			"L2000D",		"",	"", S3_MODE },
	{ "ASUSTEK ",			"L3000D",		"",	"", VBE_POST|VBE_SAVE },
	/* Carl Troein <carl@thep.lu.se> */
	{ "ASUSTEK ",			"M2000E",		"",	"", S3_BIOS },
	/* Guenther Schwarz <guenther.schwarz@gmx.de> */
	{ "ASUSTeK Computer Inc.        ",	"M2N       ",	"",	"", S3_BIOS|S3_MODE },
	/* Peter Suetterlin <P.Suetterlin@astro.uu.nl>, VBE_MODE does not work */
	{ "ASUSTeK Computer Inc.        ",	"M3N       ",	"",	"", VBE_POST|VBE_SAVE },
	/* Manfred Tremmel, https://bugzilla.novell.com/show_bug.cgi?id=171107 */
	{ "ASUSTeK Computer Inc.        ",	"M6N       ",	"",	"", S3_BIOS|S3_MODE },
	{ "ASUSTeK Computer Inc.        ",	"M6Ne      ",	"",	"", S3_MODE },
	/* Galeffi Christian gallochri2@alice.it */
	{ "ASUSTeK Computer Inc.        ",	"M51Sr *",	"",	"", S3_MODE },
	/* Charles de Miramon */
	{ "ASUSTeK Computer Inc.        ",	"M5N       ",	"",	"", S3_BIOS|S3_MODE },
	/* M6VA, seraphim@glockenbach.net */
	{ "ASUSTeK Computer Inc.        ",	"M6VA      ",	"",	"", S3_BIOS|S3_MODE },
	/* M7N, Xavier Douville <s2ram-sourceforge@douville.org> */
	{ "ASUSTeK Computer Inc.        ",	"M7A       ",	"",	"", S3_BIOS|S3_MODE },
	/* ASUS S5200N Philip Frei <pjf@gmx.de> */
	{ "ASUSTeK Computer Inc.        ",	"S5N       ",	"",	"", S3_BIOS|S3_MODE },
	/* Tobias Rothe <ir268155@mail.inf.tu-dresden.de> */
	{ "ASUSTeK Computer Inc.        ",	"U3S *",	"",	"", VBE_POST|VBE_SAVE },
	/* ASUS U5F, Krzysztof Krzyzaniak <eloy@kofeina.net> */
	{ "ASUSTeK Computer Inc. ",		"U5F *",	"",	"", S3_BIOS },
	/* ASUS V6V, Johannes Engel <j-engel@gmx.de> */
	{ "ASUSTeK Computer INC.",	"V6V",			"",	"", S3_MODE },
	/* Kanru Chen <kanru@csie.us> */
	{ "ASUSTeK Computer INC.",		"W3A",		"",	"", S3_BIOS|S3_MODE },
	/* ASUS W5A, Riccardo Sama' <riccardo.sama@yetopen.it> */
	{ "ASUSTeK Computer Inc.        ",	"W5A       ",	"",	"", S3_BIOS|S3_MODE },
	/* ASUS W7J, https://bugzilla.novell.com/show_bug.cgi?id=351661 */
	{ "ASUSTeK Computer Inc.",		"W7J *",	"",	"", VBE_MODE },
	/* Bernhard M. Wiedemann <s2rambmw@lsmod.de>, works with radeon and fglrx */
	{ "ASUSTeK Computer Inc.",		"X51RL *",	"",	"", VBE_POST|VBE_SAVE },
	/* ASUS Z35Fm, Francois Marier <francois@debian.org> */
	{ "ASUSTeK Computer Inc.        ",	"Z35FM *",	"",	"", S3_BIOS|PCI_SAVE },
	/* ASUS M2400N, Daniel Gollub */
	{ "ERGOUK                       ",	"M2N       ",	"",	"", S3_BIOS|S3_MODE },
	/* ASUS Mainboard, Christoph Jaeschke <chrjae@arcor.de> */
	{ "ASUSTek Computer Inc.",	"K8N-E-Deluxe",		"",	"", S3_BIOS },
	/* ASUS A7V Mainboard, Michael Klein <michael.klein@puffin.lb.shuttle.de> */
	{ "",		"",	"",	"ASUS A7V ACPI BIOS Revision *", 0 },
	/* ASUS a7v600 motherboard, has no usable sys_* entries besides bios_version :-(
	   reported by James Spencer */
	{ "",		"",	"",	"ASUS A7V600 ACPI BIOS Revision *", S3_BIOS|S3_MODE },
	/* Andreas Bolsch <Andreas.Bolsch@alumni.TU-Berlin.DE> */
	{ "",		"",	"",	"ASUS M2A-VM ACPI BIOS Revision 1101", S3_BIOS|VBE_MODE },
	/* Tristan Hoffmann <info@tristanhoffmann.de> Newer bios revisions (1705) seem to need VBE_POST*/
	{ "",		"",	"",	"ASUS M2A-VM ACPI BIOS Revision *", VBE_POST|VBE_MODE },
	/* Jan Schaefer <jan.schaefer@gmail.com> */
	{ "",	"",	"",	"ASUS M2A-VM HDMI ACPI BIOS Revision 1603", VBE_POST|VBE_MODE },
	/* ASUS M2N-E motherboars, not much usable in DMI :-(
	   reported by Ingo <ingo.steiner@gmx.net> */
	{ "",		"",	"",	"ASUS M2N-E ACPI BIOS Revision *",  0 },
	/* another ASUS Mainboard, reported by mailinglist@prodigy7.de */
	{ "",	"",	"",	"ASUS M2N32-SLI DELUXE ACPI BIOS Revision 1201", VBE_POST|VBE_MODE },
	{ "",	"",	"",	"ASUS M2N32-SLI DELUXE ACPI BIOS Revision 1101", VBE_POST|VBE_MODE },
	/* another ASUS Mainboard, RidewithStyle <ridewithstyle@googlemail.com> */
	{ "",	"",	"",	"ASUS Striker ACPI BIOS Revision *",	0 },
	/* ASUS Mainboard, also broken DMI strings... Luis Valino <valino@litec.csic.es> */
	{ "System manufacturer",	"P5KR",	"System Version",	"", 0 },
	/* Mark Stillwell */
	{ "AVERATEC",			"3700 Series",		"",	"", S3_BIOS|S3_MODE },
	/* Ross Patterson <me@rpatterson.net> */
	{ "AVERATEC",			"1000 Series",		"",	"", S3_BIOS|S3_MODE },
	/* Joel Dare <joel@joeldare.com> */
	{ "AVERATEC",			"5500 Series",		"",	"", S3_MODE },
	/* Sebastian Maus <sebastian.maus@bluemars.net> */
	{ "BENQ",			"JoyBook 7000",	"GRASSHOPPER2",	"", S3_BIOS|S3_MODE },
	/* Andreas Schmitz */
	{ "BenQ           ",		"Joybook R22",		"",	"", S3_BIOS|S3_MODE },
	/* Miroslav Hruz <miroslav.hruz@gmail.com> */
	{ "BenQ",			"Joybook S32",		"",	"", VBE_POST|VBE_MODE },
	/* Paul Duncan <eresse@greenpixel.ch> */
	{ "BenQ",			"Joybook S52",		"",	"", VBE_POST|VBE_MODE },
	/* Gilles Grandou <gilles@grandou.net> */
	{ "CLEVO",			"D500P",		"",	"", VBE_POST|NOFB },
	/* Francisco Neto <frc_neto@yahoo.com.br> */
	{ "CLEVO ",			"M5X0JE ",		"",	"", S3_BIOS },
	/* Alexandre Viard <alexandre.viard@gmail.com> */
	{ "CLEVO CO.",			"TN120R",		"",	"", 0 },
	/* Justin Lewis <jlew.blackout@gmail.com> */
	{ "COMPAL",			"HEL80C          ",	"",	"", 0 },
	/* Leszek Kubik <leszek.kubik@googlemail.com> */
	{ "COMPAL          ",		"HEL80I          ",	"",	"", 0 },
	/* Jason Clinton <jclinton@advancedclustering.com> */
	{ "COMPAL *",			"HEL81I *",		"",	"", 0 },

	/* This Armada m300, reported by benoit.monin@laposte.net seems not to have a useful
	   model description, so to make sure i also added the bios_version */
	{ "Compaq",			"Armada                                     ",	"",	"1.35", 0 },
	/* seife has some of those */
	{ "Compaq",			"Armada    E500  *",	"",	"", 0 },
	/* Rod Schafter rschaffter@comcast.net */
	{ "Compaq",			"Armada    M70  *",	"",	"", 0 },
	/* <marmodoro@libero.it> */
	{ "Compaq",			"Armada    M700 *",	"",	"", 0 },
	/* Martin Heimes <martin.heimes@gmx.de> */
	{ "Compaq",			"Evo  D510 USDT",	"",	"", 0 },
	/* Viktor Placek <viktor.placek@fs.cvut.cz> */
	{ "Compaq",			"Evo N400c *",		"",	"", 0 },
	/* Chris AtLee <chris@atlee.ca>, VBE_MODE does not work, text size changes. */
	{ "Compaq ",			"Evo N800w *",		"F.05",	"", VBE_POST|VBE_SAVE },
	/* Joel Schaerer <joel.schaerer@insa-lyon.fr> has a later BIOS and needs different Options :-/ */
	{ "Compaq ",			"Evo N800w *",		"",	"", S3_BIOS|S3_MODE|VBE_POST|VBE_MODE },
	{ "Compaq*",			"N620c *",		"",	"", S3_BIOS|S3_MODE },
	/* Paolo Saggese <pmsa4-gen@yahoo.it> */
	{ "Compaq",			"Presario 2701EA *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Struan Bartlett <struan.bartlett@NewsNow.co.uk> */
	{ "Dell Inc.                ",	"Dell DXP051 *",	"",	"", 0 },
	/* From bug 1544913@sf.net, Andrei - amaces */
	{ "Dell Computer Corporation",  "Inspiron 1150*",       "",     "", 0 },
	/* Dell Inspiron 500m, Per Øyvind Karlsen <pkarlsen@mandriva.com>
	{ "Dell Computer Corporation",	"Inspiron 500m*",	"",	"", S3_BIOS },
	 */
	/* Dell Inspiron 500m, Georg Zumstrull <Georg.Zumstrull@web.de> */
	{ "Dell Computer Corporation",	"Inspiron 500m*",	"",	"", VBE_POST|VBE_MODE },
	/* Dell Inspiron 510m, Jose Carlos Garcia Sogo <jsogo@debian.org> */
	{ "Dell Inc.",			"Inspiron 510m *",	"",	"", VBE_POST },
	/* Dell Inspiron 600m, Gavrie Philipson <gavrie@gmail.com> */
	{ "Dell Computer Corporation",  "Inspiron 600m *",      "",     "", VBE_POST|VBE_MODE },
	/* Eitan Mosenkis <eitan@mosenkis.net> */
	{ "Dell Computer Corporation",  "Inspiron 1100 *",      "",     "", VBE_POST|VBE_MODE },
	/* joey <joey7d6@yahoo.com> */
	{ "Dell Inc.",			"Inspiron 1420 *",	"",	"", VBE_POST|VBE_MODE },
	/* Dell Inspiron 1501, https://bugzilla.novell.com/show_bug.cgi?id=304281 */
	{ "Dell Inc.",			"Inspiron 1501 ",	"",	"", S3_BIOS|S3_MODE },
	/* Sebastian Rittau <srittau@jroger.in-berlin.de> */
	{ "Dell Inc.",			"Inspiron 1520 *",	"",	"", 0 },
	/* Klaus Thoennissen <klaus.thoennissen@web.de> */
	{ "Dell Inc.",			"Inspiron 1525 *",	"",	"", VBE_POST|VBE_MODE },
	/* Christophe Gay <christophegay@free.fr>, maybe only with binary only driver */
	{ "Dell Inc.",			"Inspiron 1720 *",	"",	"", 0 },
	/* Another Inspiron 5100, Tony Ernst <tee@sgi.com> */
	{ "Dell Computer Corporation",	"Inspiron 5100", "Revision A0",	"", VBE_SAVE|VBE_MODE|PCI_SAVE },
	/* Eric Sandall <eric@sandall.us> */
	{ "Dell Computer Corporation",  "Inspiron 5100 *",	"",	"", NOFB },
	/* VBE_POST|VBE_SAVE works on text console, but not under X. Tested by Chirag Rajyaguru */
	{ "Dell Computer Corporation",  "Inspiron 5150*",	"",	"", VBE_SAVE },
	/* Felipe Alvarez https://bugzilla.novell.com/show_bug.cgi?id=364429 */
	{ "Dell Computer Corporation",  "Inspiron 5160*",	"",	"", VBE_POST|VBE_MODE },
	{ "Dell Computer Corporation",  "Inspiron 8000 *",	"",	"", VBE_POST|VBE_SAVE },
	/* by Henare Degan <henare.degan@gmail.com> 8500 w. NVidia card. There are also 8500s w. ATI cards */
	{ "Dell Computer Corporation",  "Inspiron 8500 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Bernie du Breuil <bmrcldb56@yahoo.com> */
	{ "Dell Computer Corporation",  "Latitude C400 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Dell Computer Corporation",  "Latitude C600 *",	"",	"", RADEON_OFF },
	{ "Dell Computer Corporation",  "Latitude C610 *",	"",	"", VBE_POST|VBE_MODE },
	/* Tom Lear <tom@trap.mtview.ca.us> */
	{ "Dell Computer Corporation",  "Latitude C840 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Manuel Jander */
	{ "Dell Computer Corporation",	"Latitude CPx J650GT*",	"",	"", 0 },
	/* Luke Myers */
	{ "Dell Computer Corporation",	"Latitude CPx J800GT*",	"",	"", VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=366460 */
	{ "Dell Computer Corporation",	"Latitude D400 *",	"",	"", VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=220865 */
	{ "Dell Inc.",			"Latitude D410 *",	"",	"", VBE_POST|VBE_MODE },
	/* tested by seife and Nenad ANTONIC <nenad.antonic@gmail.com> */
	{ "Dell Inc.",			"Latitude D420 *",	"",	"", VBE_POST|VBE_MODE },
	/* Benoit Gschwind <doth.gschwind@gmail.com> */
	{ "Dell Inc.",			"Latitude D430 *",	"",	"", VBE_POST|VBE_MODE },
	/* Roland Hagemann <mail@rolandhagemann.de> */
	{ "Dell Computer Corporation",  "Latitude D500 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Ian Samule, https://bugzilla.novell.com/show_bug.cgi?id=175568 */
	{ "Dell Inc.",			"Latitude D505 *",	"",	"", VBE_POST|VBE_SAVE },
	/* tested by seife */
	{ "Dell Inc.",			"Latitude D520 *",	"",	"", VBE_POST|VBE_MODE },
	/* Jim <jim@zednet.org.uk> */
	{ "Dell Inc.",			"Latitude D530 *",	"",	"", VBE_POST|VBE_MODE },
	{ "Dell Computer Corporation",  "Latitude D600 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	{ "Dell Inc.",			"Latitude D610 *",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* D620 reported by Miroslav Ruda <ruda@ics.muni.cz>, <nicolae.mihalache@spaceapplications.com> */
	{ "Dell Inc.",			"Latitude D620 *",	"",	"", VBE_POST|VBE_MODE },
	/* D630 Ian Bezanson <irb@getchalk.com> and tested by seife */
	{ "Dell Inc.",			"Latitude D630 *",	"",	"", VBE_POST|VBE_MODE },
	{ "Dell Computer Corporation",	"Latitude D800 *",	"",	"", VBE_POST|VBE_SAVE },
	/* Johannes Engel <j-engel@gmx.de> */
	{ "Dell Inc.",			"Latitude D820 *",	"",	"", VBE_POST|VBE_MODE },
	/* Gabriel Ambuehl <gabriel_ambuehl@buz.ch>, version with intel graphics */
	{ "Dell Inc.",			"Latitude D830 *",	"",	"", VBE_POST|VBE_MODE },
	/* Alain Prignet <alain.prignet@univ-mlv.fr> */
	{ "Dell Computer Corp.",	"Latitude X200",	"",	"", VBE_POST|VBE_SAVE },
	/* Dell e1505, Alexander Antoniades / Inspiron 6400, James Cherryh <jcherryh@gmail.com> */
	{ "Dell Inc.",			"MM061 *",		"",	"", VBE_POST|VBE_MODE },
	/* Dell inspiron 1300, Tim Dijkstra <tim@famdijkstra.org> */
	{ "Dell Inc.",			"ME051 *",		"",	"", 0 },
	/* Dell Inspiron E1702, Julian Krause <suspend@thecrypto.org> */
	{ "Dell Inc.",			"MP061 *",		"",	"", 0 },
	/* Dell Inspiron E1705, Paul Hummel <paulhummel@gmail.com> */
	{ "Dell Inc.",			"MP061",		"",	"", 0 },
	/* Dell Inspiron 630m, Fredrik Edemar */
	{ "Dell Inc.",			"MXC051 *",		"",	"", 0 },
	/* Dell Inspiron 640m, Daniel Drake <dsd@gentoo.org> */
	{ "Dell Inc.",			"MXC061 *",		"",	"", VBE_POST },
	/* Dell XPS M1210 BryanL <fox_rlnocr@dynmail.no-ip.org> */
	{ "Dell Inc.",			"MXC062 *",		"",	"", S3_BIOS|S3_MODE },
	/* Dell XPS M1710 Harry Kuiper <hkuiper@xs4all.nl>, console only resumes with vesafb! */
	{ "Dell Inc.",			"MXG061 *",		"",	"", 0 },
	/* Dell XPS M1330, Danny Kukawka <dkukawka@suse.de> */
	{ "Dell Inc.",			"XPS M1330 *",		"",	"", VBE_POST|VBE_MODE },
	/* Pierre <pinaraf@robertlan.eu.org> maybe only with nvidia driver...
	   Elie Roux <elie.roux@telecom-bretagne.eu> */
	{ "Dell Inc.",			"XPS M1530 *",		"",	"", 0 },
	/* Ted Percival <ted@midg3t.net>, ATI Graphics, no FB :( */
	{ "Dell Inc.",			"OptiPlex 755 *",	"",	"", VBE_POST|NOFB },
	/* Ian McAnena <ianegg@gmail.com> */
	{ "Dell Computer Corporation",	"OptiPlex GX260 *",	"",	"", 0 },
	/* Desktop with Intel graphics, Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Dell Inc.                ",	"OptiPlex GX520 *",	"",	"", VBE_POST|VBE_MODE },
	/* Jon Dowland <jon+suspend-devel@alcopop.org> */
	{ "Dell Inc. *",		"OptiPlex GX620 *",	"",	"", 0 },
	/* Christoph Pacher <christoph.pacher@arcs.ac.at>, maybe only with nvidia driver :-( */
	{ "Dell Computer Corporation",	"Precision WorkStation 360*","","", 0 },
	/* Per Øyvind Karlsen <pkarlsen@mandriva.com> VBE_POST will get console working, but break X */
	{ "Dell Computer Corporation",	"Precision M60*",	"",	"", VBE_SAVE },
	/* gaofi@gaofi.com */
	{ "Dell Inc.",			"Precision M65 *",	"",	"", VBE_POST|VBE_MODE },
	/* Manoj Srivastava <srivasta@debian.org> */
	{ "Dell Inc.",			"Precision M90 *",	"",	"", 0 },
	/* Bruno Friedmann <bruno@ioda-net.ch> */
	{ "Dell Inc.",			"Precision M4300 *",	"",	"", 0 },
	/* Chris Debrunner <chd@evcohs.com> */
	{ "Dell Inc.",			"Precision M6300 *",	"",	"", VBE_POST|VBE_MODE },
	/* Kyle Kearney <elyk53@gmail.com> */
	{ "Dell Inc.",			"Vostro 1500 *",	"",	"", VBE_POST|VBE_MODE },
	/* Michael Witten mfwitten@MIT.EDU */
	{ "Dell Computer Corporation",	"Dimension 2400 *",	"",	"", VBE_POST|VBE_MODE },
	/* Allen <ducalen@sympatico.ca>, Desktop with onboard grephics */
	{ "Dell Computer Corporation",	"Dimension 3000 *",	"",	"", VBE_POST|VBE_MODE },

	/* Jarek Grzys <grzys@gazeta.pl> */
	{ "DIXONSXP",		"To be filled by O.E.M.",	"Ver.001", "", 0 },
	/* Michael Dickson <mike@communicatezing.com> */
	{ "ECS",			"536",			"",	"", VBE_POST|NOFB },
	/* Michael Bunk <michael.bunk@gmail.com> */
	{ "Elitegroup Co.",		"ECS G320",		"",	"", VBE_POST|VBE_MODE },

	/* Loki Harfagr <l0k1@free.fr> */
	{ "FUJITSU SIEMENS",		"Amilo A1645 ",		"",	"", S3_BIOS|S3_MODE },
	/* stan ioan-eugen <stan.ieugen@gmail.com> */
	{ "FUJITSU SIEMENS",		"Amilo A1667G Serie",	"",	"", S3_BIOS },
	/* Holger Macht <hmacht@suse.de>, with kernel 2.6.22+, x86_64 */
	{ "FUJITSU SIEMENS",		"Amilo A7640 ",		"",	"", S3_BIOS|S3_MODE },
	/* reported by Thomas Halva Labella <hlabella@ulb.ac.be> */
	{ "FUJITSU SIEMENS",		"Amilo A7645 ",		"",	"", VBE_SAVE|S3_BIOS|S3_MODE },
	/* Chris Hammond <christopher.hammond@gmail.com> Amilo A 1630 */
	{ "FUJITSU SIEMENS",		"Amilo A Series",	"0.01",	"", VBE_POST|VBE_SAVE },
	/* Hendrik Golzke <hendrik.golzke@yahoo.de> */
	{ "FUJITSU SIEMENS",		"Amilo D Series",	"",	"", VBE_POST|VBE_MODE },
	/* Marcel Klemmer <imklemmer@web.de> */
	{ "FUJITSU SIEMENS",		"AMILO L Series",	"",	"", VBE_POST|VBE_MODE },
	/* Reiner Herrmann <reiner@reiner-h.de>, this one realls needs VBE_SAVE
	   since there are many different models of the "AMILO M" series, i do a very exact match */
	{ "FUJITSU SIEMENS",		"AMILO M        ",	"-1 *",	"R01-S0Z *", VBE_POST|VBE_SAVE },
	/* Simon Maurer <Twiks@gmx.de> again, match exactly. */
	{ "FUJITSU SIEMENS",		"AMILO M        ",	"-1 *",	"R01-S0N *", VBE_POST|VBE_SAVE|PCI_SAVE },
	/* Moritz Heidkamp <moritz@invision-team.de> */
	{ "FUJITSU SIEMENS",		"AMILO M Series",	"",	"1.0G*", VBE_SAVE },
	/* Markus Meyer <markus.meyer@koeln.de>, Christian Illy <christian.illy@gmx.de> */
	{ "FUJITSU SIEMENS",		"Amilo Si 1520",	"",	"", S3_BIOS|S3_MODE },
	/* Sab <sabry84@gmail.com> */
	{ "FUJITSU SIEMENS",		"AMILO Pi 1505",	"",	"", S3_BIOS|S3_MODE },
	/* Giorgos Mavrikas <mavrikas@gmail.com> */
	{ "FUJITSU SIEMENS",		"AMILO Pi 1536",	"",	"", VBE_POST|VBE_MODE },
	/* Yasen Pramatarov <yasen@lindeas.com> */
	{ "FUJITSU SIEMENS",		"AMILO Pi 2515",	"",	"", VBE_POST|VBE_MODE },
	/* Aleksandar Markovic <acojlo@gmail.com> */
	{ "FUJITSU SIEMENS",		"AMILO PRO V2030",	"",	"", VBE_POST|VBE_MODE },
	/* some unknown contributor */
	{ "FUJITSU SIEMENS",		"AMILO PRO V2035",	"",	"", VBE_POST|VBE_MODE },
	/* Florian Herrmann <florian-herrmann@gmx.de> */
	{ "FUJITSU SIEMENS",		"AMILO Pro V2040",	"",	"", 0 },
	/* <arild.haugstad@gmx.net> / https://bugzilla.novell.com/show_bug.cgi?id=279944 */
	{ "FUJITSU SIEMENS",		"AMILO Pro V3205",	"",	"", S3_BIOS|S3_MODE },
	/* Alexandr Kara <Alexandr.Kara@seznam.cz> */
	{ "FUJITSU SIEMENS",		"AMILO Pro Edition V3405", "",	"", S3_BIOS|S3_MODE },
	/* Igor A. Goncharovsky <igi-go@ya.ru> */
	{ "FUJITSU SIEMENS",		"AMILO Pro Edition V3505 *","",	"", S3_BIOS|S3_MODE },
	/* Steffen <mlsone@forevers.de> */
	{ "FUJITSU SIEMENS",		"AMILO PRO V8010 *",	"",	"", VBE_POST|VBE_MODE },
	/* <noname1@onlinehome.de> */
	{ "FUJITSU SIEMENS",		"AMILO Xi 1546",	"",	"", S3_BIOS|S3_MODE },
	/* Clement LAGRANGE <clement.lagrange@gmail.com> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK B Series",	"",	"", 0 },
	/* wolfgang.aigner@gmx.de */
	{ "FUJITSU SIEMENS",		"LIFEBOOK E4010",	"",	"", VBE_SAVE },
	/* Robin Knapp <mail@gizzmo.org> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK E8020",	"",	"", VBE_POST|VBE_MODE },
	/* seife */
	{ "FUJITSU SIEMENS",		"LIFEBOOK E8410",	"",	"", S3_BIOS|S3_MODE },
	/* Stephen Gildea <stepheng@gildea.com> */
	{ "FUJITSU",			"LifeBook P7010D",	"",	"", S3_BIOS },
	/* Michael Hillerstrom <michael@hillerstrom.org> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK P7230",	"",	"", S3_BIOS|S3_MODE|PCI_SAVE },
	/* Andi Kleen, reported to work in 64bit mode */
	{ "FUJITSU",			"LifeBook S2110",	"",	"", S3_BIOS },
	/* Fred Lahuis <fred@sron.nl> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK S6410",	"",	"", VBE_POST|VBE_MODE },
	/* <jan.lockenvitz.ext@siemens.com>, tested with X only */
	{ "FUJITSU SIEMENS",		"LIFEBOOK S7010",	"",	"", 0 },
	/* Antonio Cardoso Martins <digiplan.pt@gmail.com> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK S7020",	"",	"", S3_BIOS|S3_MODE|PCI_SAVE },
	/* Samuel Tardieu <sam@rfc1149.net> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK S7110",	"",	"", 0 },
	/* David Nolden <zwabel@googlemail.com> */
	{ "FUJITSU",			"LifeBook T Series",	"",	"", 0 },
	/* Alexander Clouter <alex@digriz.org.uk>, needs vbe_save or the vga mode is upset */
	{ "FUJITSU SIEMENS",		"LifeBook T2010",	"",	"", VBE_POST|VBE_SAVE|PCI_SAVE },
	/* Eckhart Woerner <ew@ewsoftware.de> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK T4010",	"",	"", S3_BIOS|S3_MODE },
	/* Jay <jay@jay.cz> */
	{ "FUJITSU SIEMENS",		"LIFEBOOK T4210",	"",	"", S3_BIOS|S3_MODE|PCI_SAVE },
	/* Tomas Pospisek <tpo@sourcepole.ch> */
	{ "Fujitsu Siemens",		"P6VAP-AP",		"",	"", 0 },
	{ "FUJITSU SIEMENS",		"Stylistic ST5000",	"",	"", S3_BIOS|S3_MODE },
	/* Martin Wilck <martin.wilck@fujitsu-siemens.com> */
	{ "FUJITSU SIEMENS",		"ESPRIMO Mobile M9400",	"",	"", 0 },
	/* Jan Schaefer <jan.schaefer@informatik.fh-wiesbaden.de> */
	{ "FUJITSU SIEMENS",		"ESPRIMO P*",		"",	"", 0 },
	/* Guilhem Bonnefille <guilhem.bonnefille@gmail.com> */
	{ "FUJITSU SIEMENS",		"ESPRIMO Mobile D9500",	"",	"", 0 },
	/* Ulf Lange <mopp@gmx.net> */
	{ "FUJITSU SIEMENS",		"SCENIC N300/N600",	"",	"", VBE_POST|VBE_MODE },
	/* Guenther Schwarz <guenther.schwarz@gmx.de> */
	{ "FUJITSU SIEMENS",		"SCENIC P / SCENICO P",	"",	"", VBE_POST|VBE_MODE },
	/* This is a desktop with onboard i810 video <seife@suse.de> */
	{ "FUJITSU SIEMENS",		"SCENIC W300/W600",	"",	"", VBE_POST|VBE_MODE },
	/* Peer Heinlein <p.heinlein@jpberlin.de> */
	{ "FUJITSU SIEMENS",		"SCENIC W",		"",	"", VBE_POST|VBE_MODE },

	/* Ilya Eremin <che_guevara_3@bk.ru> */
	{ "Gateway *",			"MX6922B *",		"",	"", 0 },
	/* Mario Nigrovic <Mario@nigrovic.net> */
	{ "Gateway *",			"MT6707 *",		"",	"", S3_BIOS|S3_MODE },
	/* Christoph Wiesmeyr <christoph.wiesmeyr@liwest.at> */
	{ "GERICOM",			"259IA1",		"",	"", 0 },
	/* booiiing@gmail.com */
	{ "Gericom",			"HUMMER",	"",	"1.03   ",  VBE_POST|VBE_MODE },
	/* From: Einon <einon@animehq.hu>, seems to work only from X :-( */
	{ "Gericom",			"Montara-GML ",	"FAB-2",	"", 0 },
	/* S.Çağlar Onur <caglar@pardus.org.tr>, only works from X */
	{ "MTC",			"Montara-GML ",	"FAB-2",	"", 0 },
	/* Nik Tripp https://bugzilla.novell.com/show_bug.cgi?id=400356 */
	{ "Gigabyte Technology Co., Ltd.",	"945GCM-S2L",	"",	"", 0 },
	/* Josef Zenisek <jzenisek@gmx.de> */
	{ "Gigabyte Technology Co., Ltd.",	"945GZM-S2",	"",	"", 0 },
	/* Art Mitskevich <art@unlit.org> */
	{ "Gigabyte Technology Co., Ltd.",	"945GCMX-S2",	"",	"", VBE_POST|VBE_MODE },
	/* Roberto Lumbreras <rover@lumbreras.org> */
	{ "Gigabyte Technology Co., Ltd.",	"P35-DS4",	"",	"", VBE_POST|VBE_MODE },
	/* Andrey Melentyev <andrey.melentyev@gmail.com> */
	{ "Infomash",			"RoverBook",		"",	"", VBE_POST|VBE_MODE },

	/* Stefan Kulow https://bugzilla.novell.com/show_bug.cgi?id=362928 */
	{ "Hewlett-Packard",	"HP Compaq 2510p Notebook PC",	"",	"68MSP*", VBE_POST|VBE_MODE },
	/* Holger Sickenberg https://bugzilla.novell.com/show_bug.cgi?id=376715 */
	{ "Hewlett-Packard",		"HP Compaq 2710p *",	"",	"68MOU*", VBE_POST|VBE_MODE },
	/* i wonder if we can just match for "HP Compaq 6510b.... */
	/* Jamil Djadala <djadala@datamax.bg> */
	{ "Hewlett-Packard",		"HP Compaq 6510b (GB863EA*", "","68DDU*", VBE_POST|VBE_MODE },
	/* peeetax@googlemail.com */
	{ "Hewlett-Packard",		"HP Compaq 6510b (KE134EA*", "","68DDU*", 0 },
	/* Michal Sojka <sojkam1@fel.cvut.cz> */
	{ "Hewlett-Packard",		"HP Compaq 6710b (GB893EA*", "","68DDU*", VBE_POST|VBE_MODE },
	/* Harry ten Berge <htenberge@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6710b (GB893ET*", "","68DDU*", VBE_POST|VBE_MODE },
	/* Michal Skrzypek <mskrzypek886@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6710b (GR681EA*", "","68DDU*", VBE_POST|VBE_MODE },
	/* Jakub Talas <kuba.talas@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6710b (KE124EA*", "","68DDU*", VBE_POST|VBE_MODE },
	/* Alberto Gonzalez <luis6674@yahoo.com> */
	{ "Hewlett-Packard",		"HP Compaq 6710s (GC014ET*", "","68DDU*", VBE_POST|VBE_MODE },
	/* Darek Nawrocki <dnawrock@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6715b (GB834EA*", "","68YTT*", VBE_POST|VBE_MODE },
	/* Radu Benea <kitanatahu@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6715b (GB835EA*", "","68YTT*", S3_BIOS|S3_MODE },
	/* Andrey Petrov <apetrov87@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6715b (RK156AV*", "","68YTT*", S3_BIOS|S3_MODE },
	/* Matthias Hopf, https://bugzilla.novell.com/show_bug.cgi?id=390271, needs strange combination */
	{ "Hewlett-Packard",		"HP Compaq 6715b (RM174UT*", "","68YTT*", S3_BIOS|VBE_MODE },
	/* Petteri Tolonen <pretzel@mbnet.fi> works with radeonhd and fglrx */
	{ "Hewlett-Packard",		"HP Compaq 6715s (GR656ET*", "","68YTT*", S3_BIOS },
	/* Paul Smet <paul.smet@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6720s",	 "",	"68MDU*", VBE_POST|VBE_MODE },
	/* Babarovic Ivica <ivica.babarovic@asist.si> */
	{ "Hewlett-Packard",		"HP Compaq 6820s",	 "",	"68MDD*", S3_BIOS },
	/* Ola Widlund <olwix@yahoo.com>, works with proprietary ATI driver, to be verified without */
	{ "Hewlett-Packard",		"HP Compaq 6910p (GB951EA*", "","68MCD*", VBE_MODE },
	/* Tomas Kejzlar <t.kejzlar@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq 6910p (RH241AV)", "","68MCU*", VBE_POST|VBE_MODE },
	/* Daniele Frijia <daniele@sase.de>, this machine has no model number in DMI. BIOS F.14 */
	{ "Hewlett-Packard",		"HP Compaq 6910p",	"",	"68MCU*", VBE_POST|VBE_MODE },
	/* Krisztian Loki <krisztian.loki@freemail.hu>, does not work with proprietary ATI driver */
	{ "Hewlett-Packard",		"HP Compaq 8510p ",	"",	"68MVD*", VBE_POST|VBE_MODE },
	/* Milan Znamenacek <mznamenacek@retia.cz>, only from X */
	{ "Hewlett-Packard",		"HP Compaq 8710p (GC102EA*","",	"68MAD*", 0 },
	/* Tim Harris <tim.harris@redhillconsulting.com.au> */
	{ "Hewlett-Packard",		"HP Compaq 8710p (GT647PA*","",	"68MAD*", 0 },
	/* Marek Stopka <mstopka@opensuse.org>, kernel 2.6.25 */
	{ "Hewlett-Packard",	"HP Compaq dc5800 Small Form Factor","","786F2*", 0 },
	/* hp compaq nc2400, tested by seife. sometimes has keyboard problems after resume */
	{ "Hewlett-Packard",		"HP Compaq nc2400*",	"",	"68YOP*", VBE_POST|VBE_MODE },
	/* Rene Seindal <rene@seindal.dk> */
	{ "Hewlett-Packard",		"HP Compaq nc4400*",	"F.08",	"68YHV*", VBE_POST|VBE_SAVE },
	/* Antti Laine <antti.a.laine@tut.fi>, "did not work with older BIOS", now has F.09 */
	{ "Hewlett-Packard",		"HP Compaq nc4400*",	"",	"68YHV*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard*",		"Compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard*",		"hp compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	/* Ed Hoo <whoishoo@gmail.com> */
	{ "Hewlett-Packard*",		"hp Compaq nx5000 *",	"",	"68BCU*", VBE_POST|VBE_MODE },
	/* there seem to be versions with a " " after Hewlett-Packard and without. */
	{ "Hewlett-Packard*",		"HP Compaq nc6000 *",	"",	"68BDD*", S3_BIOS|S3_MODE },
	/* Jan Adlen <janne@rosehip.mine.nu> */
	{ "Hewlett-Packard",		"Compaq nc6000 *",	"",	"68BDD*", S3_BIOS|S3_MODE },
	/* <gabriel.pettier@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq nx6110 *",	"",	"68DTD", VBE_POST|VBE_MODE },
	/* Winfried Dobbe, https://bugzilla.novell.com/show_bug.cgi?id=159688 */
	{ "Hewlett-Packard",		"HP Compaq nc6120 *",	"",	"", VBE_POST|VBE_MODE },
	/* Battle Nick <Nick.Battle@uk.fujitsu.com>, also a nc6120 */
	{ "Hewlett-Packard",		"PY507ET#*",		"",	"68DTD*", VBE_POST|VBE_MODE },
	/* Niv aharonovich <niv.kaha@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq nx6120 *",	"",	"68DTD*", VBE_POST|VBE_MODE },
	{ "Hewlett-Packard",		"HP Compaq nx6125 *",	"",	"", VBE_SAVE|NOFB },
	{ "Hewlett-Packard",		"HP Compaq nc6230 *",	"",	"", VBE_SAVE|NOFB },
	/* Michal Vyskocil <michal.vyskocil@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq nx6310 *",	"",	"68YDU*", VBE_POST },
	/* Hendrik Tews <tews@cs.ru.nl> */
	{ "Hewlett-Packard",		"HP Compaq nc6320 (RH367ET*","","68YDU*", VBE_POST|VBE_MODE },
	/* Danny Kukawka <dkukawka@suse.de> */
	{ "Hewlett-Packard",		"HP Compaq nc6320 *",	"",	"68YDU*", VBE_POST|VBE_MODE },
	/* Michal Sedkowski <msedkowski@gmail.com>, another nx6310 with different options? :-( */
	{ "Hewlett-Packard",		"",			"",	"68YDU*", 0 },
	/* Benedikt Nimmervoll <nimmervoll.benedikt@googlemail.com>, EY350EA#ABD
	   also works with version RD115AA#ABA <hmacht@suse.de> */
	{ "Hewlett-Packard",		"HP Compaq nx6325 *",	"",	"68TT2*", VBE_POST|VBE_SAVE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=260451 */
	{ "Hewlett-Packard",		"HP Compaq nc6400 (EH522AV)","","68YCU*", VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=295489 */
	{ "Hewlett-Packard",		"HP Compaq nc6400 (EY582ES*","","68YCU*", VBE_POST|VBE_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=326526 */
	{ "Hewlett-Packard",		"HP Compaq nc6400 (RH560EA*","","68YCU*", VBE_POST|VBE_MODE },
	/* Michael Biebl <biebl@teco.edu> */
	{ "Hewlett-Packard ",		"HP compaq nx7000 *",	"",	"", VBE_POST|VBE_SAVE },
	/* François Münch <francois.munch@free.fr> */
	{ "Hewlett-Packard ",		"Compaq nx 7010 (PG588EA#*","",	"", S3_BIOS },
	/* Philippe Marzouk <phil@ozigo.org> */
	{ "Hewlett-Packard ",		"Compaq nx 7010 (DU394T#*",	"",	"68BAL*", S3_BIOS|S3_MODE },
	/* Valent Turkovic <valent.turkovic@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq nx7300 (RU374ES#*",	"",	"", VBE_POST|VBE_MODE },
	/* Slawomir Skrzyniarz <diodak@gmail.com> */
	{ "Hewlett-Packard",		"HP Compaq nx7400 (RH412EA#)",	"",	"", VBE_POST|VBE_MODE },
	/* Michael Meskes <meskes@debian.org>, this is a NX 7400 */
	{ "Hewlett-Packard",		"",			"",	"68YGU*", VBE_POST },
	{ "Hewlett-Packard",		"HP Compaq nx8220 *",	"",	"", VBE_SAVE|NOFB },
	/* Stefan Seyfried <seife@suse.de> novell bug 259831, this machine is only available in one
	   configuration (ATI Graphics) */
	{ "Hewlett-Packard",	"HP Compaq nw8440 *",		"",	"68YVD*", VBE_POST|VBE_MODE },
	/* Georg Walser <gwa@aspedi.com> Model DG286A */
	{ "Hewlett-Packard",		"HP d330 uT*",		"",	"", 0 },
	/* Ryan Bair <ryandbair@gmail.com> */
	{ "Hewlett-Packard",		"HP d530 SFF*",		"",	"", 0 },
	/* Frederic Mothe <mothe@nancy.inra.fr> */
	{ "Hewlett Packard",		"",	"HP OmniBook XE3 GC *",	"", VBE_POST|VBE_SAVE },
	{ "Hewlett Packard",		"",	"HP OmniBook XE3 GF *",	"", VBE_POST|VBE_SAVE },
	/* Bernd Rinn <bb@rinn.ch> */
	{ "Hewlett Packard",		"",	"HP OmniBook XT1000 *",	"", S3_MODE },
	/* Jari Turkia <jmjt@lut.fi> */
	{ "Hewlett-Packard *",		"Presario 2100 (DP835E)*",	"",	"", 0 },
	/* Matto Marjanovic <maddog@mir.com> */
	{ "Hewlett-Packard",		"Presario C500 (RZ341UA#*",	"",	"", 0 },
	/* Fatih Alabas https://bugzilla.novell.com/show_bug.cgi?id=230528 */
	{ "Hewlett-Packard",		"Presario M2000 (EQ547PA#*",	"",	"", S3_BIOS|VBE_MODE },
	/* Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Hewlett-Packard ",		"Presario R3000 *",	"",	"", S3_BIOS|S3_MODE },
	/* Arthur Peters <amp@singingwizard.org> */
	{ "Hewlett-Packard",		"Presario R3200 *",	"",	"", VBE_POST|VBE_SAVE },
	{ "Hewlett-Packard",		"Presario R4100 *",	"",	"", S3_BIOS|S3_MODE },
	/* iamnoah@gmail.com */
	{ "Hewlett-Packard",		"Presario V2000 (EP381UA#*","",	"", S3_BIOS },
	/* From: "Lim Chuen Ee, Mike" <chuenee@starhub.net.sg> */
	{ "Hewlett-Packard",		"Presario V3000 (RL377PA#*","",	"", S3_BIOS|S3_MODE },
	/* Cristian S. Rocha  crocha@dc.uba.ar */
	{ "Hewlett-Packard",		"Presario V3000 (EZ674UA#*","",	"", S3_BIOS|S3_MODE },
	/* JEROME HAYNES-SMITH <jerome.smith@btinternet.com> */
	{ "Hewlett-Packard",		"Presario V3000 (EZ755UA#*","",	"", 0 },
	/* Michael Kimsal <mgkimsal@gmail.com> */
	{ "Hewlett-Packard",		"Presario V5000 (ET826UA#*","",	"", 0 },
	/* Adi Nugroho https://bugzilla.novell.com/show_bug.cgi?id=364456 */
	{ "Hewlett-Packard",	"Presario V3700 Notebook PC",	"",	"", S3_BIOS|S3_MODE },
	/* "Zirkel, Timothy K" <zirkelt@lafayette.edu> */
	{ "Hewlett-Packard ",	"Compaq Presario X1000 (DK454AV)*", "",	"", S3_BIOS },
	/* S.Çağlar Onur <caglar@pardus.org.tr>, works only from X! */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (GF759EA#*", "",	"", 0 },
	/* Tarek Soliman <tarek-spam@zeusmail.bounceme.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (GA534UA#*", "",	"", 0 },
	/* Daniel Forrer <daniel.forrer@unipd.it> */
	{ "Hewlett-Packard",	"HP Pavilion dv2000 (RN066EA#*", "",	"", 0 },
	/* Robert Hart <Robert.Hart@BuroHappold.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv2500 Notebook PC", "",	"", 0 },
	/* Fabio Comolli <fabio.comolli@gmail.com>, dv4378ea, ATI X700 */
	{ "Hewlett-Packard",	"Pavilion dv4000 (ES973EA#*",	"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Damien Raude-Morvan <drazzib@drazzib.com>, This is a dv4052ea, i915GM */
	{ "Hewlett-Packard",	"Pavilion dv4000 (EB911EA#*",	"",	"", S3_BIOS },
	/* Wally Valters <deepsky99@gmail.com> */
	{ "Hewlett-Packard",	"Pavilion dv5000 (EZ535UA#*",	"",	"", 0 },
	/* Marcos Pinto <markybob@gmail.com>, HP dv6000 */
	{ "Hewlett-Packard",		"EW434AVABA *",		"",	"", S3_BIOS|S3_MODE },
	/* Valent Turkovic <valent.turkovic@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GA378UA#*", "",	"", S3_MODE },
	/* Eric Brasseur <eric.brasseur@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GH907EA#*", "",	"", S3_MODE },
	/* Benjamin Kudria <ben@kudria.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (EW434AV#*", "",	"", S3_BIOS|S3_MODE },
	/* Jens Kaiser <kaiser@caltech.edu>, maybe only with binary nvidia module? */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RP153UA#*", "",	"", S3_BIOS|S3_MODE },
	/* Arun Gupta <axgupta1@gmail.com> */
	{ "Hewlett-Packard",	"Pavilion dv6000 (EY798AV#*",	"",	"", 0 },
	/* Carlos Andrade <crandrad@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (EY798AV*",	"",	"", 0 },
	/* Marlin Forbes <marlinf@datashaman.com> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (GF688EA*",	"",	"", 0 },
	/* Antony Dovgal <tony2001@phpclub.net> */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RM469EA#*","",	"", 0 },
	/* John Soros <sorosj@gmail.com> "HP dv6120ea" */
	{ "Hewlett-Packard",	"HP Pavilion dv6000 (RP980EA#*","",	"", 0 },
	/* Fran Firman <fran@ipsm.net.nz>, no hacks, maybe only works due to the Nvidia driver?
	   Marco Nenciarini <mnencia@prato.linux.it> needs S3_BIOS */
	{ "Hewlett-Packard",	"HP Pavilion dv6500 Notebook PC *", "",	"", S3_BIOS },
	/* <veesah@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion tx1000 Notebook PC *", "",	"", 0 },
	/* Etienne URBAH <etienne.urbah@free.fr> */
	{ "Hewlett-Packard",	"Pavilion zd8000 (EL030EA#*","",	"", VBE_POST },
	/* Chris Polderman <chris.polderman@gmail.com> */
	{ "Hewlett-Packard",	"HP Pavilion ze2000 (EK791EA#*","",	"", 0 },
	/* Fuad Tabba <fuad@cs.auckland.ac.nz>, only available with integr. gfx */
	{ "Hewlett-Packard ",	"hp pavilion ze4900 (*",	"",	"", 0 },
	/* Frank Schröder <frank.schroeder@gmail.com>, zt3380us */
	{ "Hewlett-Packard ",	"Pavilion zt3300 (PF083UA#*",	"",	"", 0 },
	/* Karthik <molecularbiophysics@gmail.com> */
	{ "Hewlett-Packard ",		"Pavilion zt3300 *",	"",	"68BAL*", S3_BIOS },
	/* Arun Gupta <axgupta1@gmail.com>, works only with nvidia driver :( */
	{ "HP-Pavilion",		"RP829AV-ABA d4790y",	"",	"", 0 },
	/* Gijs van Gemert <g.v.gemert@inter.nl.net> */
	{ "HP-Pavilion",		"RZ418AA-ABH s3020*",	"",	"", VBE_MODE },
	/* Benjamin Cherian <benjamin.cherian@gmail.com> */
	{ "HP Pavilion 061",		"D7223K-ABA A650E",	"",	"", 0 },
	/* A desktop with integrated graphics, Bo Thysell <thysell@fusemail.com> */
	{ "HP Pavilion 061",		"EJ192AA-ABS t3250.se",	"",	"", 0 },
	/* Michael Zaugg <dev.miguel@netzbox.ch> */
	{ "HP Pavilion 061",		"EP198AA-UUZ t3335.ch",	"",	"", 0 },
	/* Stefan Friedrich <strfr@web.de> */
	{ "HP Pavilion 061",		"ES061AA-ABD t3320*",	"",	"", 0 },

	/* R51 confirmed by Christian Zoz and Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "IBM",			"",		"ThinkPad R51",	"", 0 },
	/* R52, reported by Joscha Arenz */
	{ "IBM",			"1860*",		"",	"", S3_BIOS|S3_MODE },
	/* R51e, reported by nemec.jiri@gmail.com */
	{ "IBM",			"",	"ThinkPad R51e",	"", S3_BIOS|S3_MODE },
	/* T30 */
	{ "IBM",			"2366*",		"",	"", RADEON_OFF },
	/* Thinkpad 600e, George Tellalov <gtellalov@emporikilife.gr>, needs X running */
	{ "IBM",			"2645*",		"",	"INET17WW", VBE_POST|VBE_MODE },
	/* George says the newer BIOS INET36WW / kernel 2.6.22 works fine with these options / without X */
	{ "IBM",			"2645*",		"",	"", VBE_SAVE },
	/* T23 Gernot Schilling <gernotschilling@gmx.de> */
	{ "IBM",			"2647*",		"",	"", S3_BIOS|S3_MODE },
	/* X22, confirmed by Richard Neill */
	{ "IBM",			"2662*",		"",	"", S3_BIOS|S3_MODE|RADEON_OFF },
	/* X31, confirmed by Bjoern Jacke */
	{ "IBM",			"2672*",		"",	"", S3_BIOS|S3_MODE|RADEON_OFF },
	/* X31, Vladislav Korecky <v.korecky@gmail.com>, bios_version="1QET73WW (2.11 )" */
	{ "IBM",			"2673*",		"",	"", S3_BIOS|S3_MODE },
	/* X40 confirmed by Christian Deckelmann */
	{ "IBM",			"2371*",	"ThinkPad X40",	"", S3_BIOS|S3_MODE },
	/* X40, Andrew Pimlott <andrew@pimlott.net> */
	{ "IBM",			"2386*",		"",	"", S3_BIOS|S3_MODE },
	/* X41 Tablet, tested by Danny Kukawka */
	{ "IBM",			"",	"ThinkPad X41 Tablet",	"", S3_BIOS|S3_MODE },
	/* T42p confirmed by Joe Shaw, T41p by Christoph Thiel (both 2373) */
	{ "IBM",			"2373*",		"",	"", S3_BIOS|S3_MODE },
	/* T41p, Stefan Gerber */
	{ "IBM",			"2374*",		"",	"", S3_BIOS|S3_MODE },
	/* T42, Bjorn Mork <bjorn@mork.no> */
	{ "IBM",			"2376*",		"",	"", S3_BIOS|S3_MODE },
	/* Some T43's reported by Sebastian Nagel */
	{ "IBM",			"",		"ThinkPad T43",	"", S3_BIOS|S3_MODE },
	/* T43p reported by Magnus Boman */
	{ "IBM",			"",	"ThinkPad T43p",	"", S3_BIOS|S3_MODE },
	/* G40 confirmed by David Härdeman */
	{ "IBM",			"2388*",		"",	"", 0 },
	/* A30, Axel Braun. https://bugzilla.novell.com/show_bug.cgi?id=309742
	   Backlight stays on, but RADEON_OFF kills the machine on second suspend :-( */
	{ "IBM",			"2652*",		"",	"", S3_BIOS|S3_MODE },
	/* R32 */
	{ "IBM",			"2658*",		"",	"", 0 },
	/* R40 */
	{ "IBM",			"2681*",		"",	"", 0 },
	{ "IBM",			"2722*",		"",	"", 0 },
	/* Z60t, Sascha Hunold <sascha_hunold@gmx.de> */
	{ "IBM",			"2511*",		"",	"", S3_BIOS|S3_MODE },
	/* Z60m, reported by Arkadiusz Miskiewicz */
	{ "IBM",			"2529*",		"",	"", S3_BIOS|S3_MODE },
	/* A21m, Raymund Will */
	{ "IBM",			"2628*",		"",	"", 0 },
	/* A21p, Holger Sickenberg */
	{ "IBM",			"2629*",		"",	"", 0 },
	/* A31p, Till D"orges <td@pre-secure.de> */
	{ "IBM",			"2653*",		"",	"", RADEON_OFF },
	/* X41, Florian Ragwitz <rafl@debian.org>, */
	{ "IBM",			"2525*",		"",	"", S3_BIOS|S3_MODE },
	/* X41, Johannes Zellner <johannes@zellner.org> */
	{ "IBM",			"2526*",		"",	"", S3_BIOS|S3_MODE },
	/* Ralph Thormann, notice the empty sys_product :-( */
	{ "IBM",			"       ",	"ThinkPad X40",	"1UETD2WW*", 0 },
	/* Joerg Platte jplatte@naasa.net, again emtpy sys_product... */
	{ "IBM",			"       ",	"ThinkPad T40 ","1RETDRWW*", 0 },

	/* Bunch of entries from _Lenovo_ */
	/* those with "no hack needed" are machines with NVidia graphics, working only
	   with (probably a specific version of) the nvidia driver.
	   I have to think about what to do with those... */
#if 0
	/* T61 */
	{ "LENOVO",			"6457*",	"",		"", 0 },
	{ "LENOVO",			"6459*",	"",		"", 0 },
	{ "LENOVO",			"6460*",	"",		"", 0 },
	{ "LENOVO",			"6461*",	"",		"", 0 },
	{ "LENOVO",			"6462*",	"",		"", 0 },
	{ "LENOVO",			"6470*",	"",		"", 0 },
	{ "LENOVO",			"7662*",	"",		"", 0 },
	{ "LENOVO",			"7663*",	"",		"", 0 },
	{ "LENOVO",			"7664*",	"",		"", 0 },
	{ "LENOVO",			"7665*",	"",		"", 0 },
	{ "LENOVO",			"6377*",	"",		"", 0 },
	{ "LENOVO",			"1959*",	"",		"", 0 },
	{ "LENOVO",			"6481*",	"",		"", 0 },
#endif
	/* confirmed to work with VESA by Ryan Kensrod <rkensrud@novell.com> */
	{ "LENOVO",			"6458*",	"",		"", VBE_POST|VBE_MODE },
	/* R61 Discrete */
	/* moritz.rogalli@student.uni-tuebingen.de, works with and without nvidia drivers */
	{ "LENOVO",			"8889*",	"",		"", 0 },
#if 0
	{ "LENOVO",			"8890*",	"",		"", 0 },
	{ "LENOVO",			"8891*",	"",		"", 0 },
	{ "LENOVO",			"8892*",	"",		"", 0 },
	{ "LENOVO",			"8893*",	"",		"", 0 },
	{ "LENOVO",			"8894*",	"",		"", 0 },
	/* R61 */
	{ "LENOVO",			"7742*",	"",		"", 0 },
	{ "LENOVO",			"7743*",	"",		"", 0 },
	{ "LENOVO",			"7744*",	"",		"", 0 },
	{ "LENOVO",			"7751*",	"",		"", 0 },
	{ "LENOVO",			"7753*",	"",		"", 0 },
	{ "LENOVO",			"7754*",	"",		"", 0 },
	{ "LENOVO",			"7755*",	"",		"", 0 },
	//
	{ "LENOVO",			"8914*",	"",		"", 0 },
	{ "LENOVO",			"8918*",	"",		"", 0 },
	{ "LENOVO",			"8919*",	"",		"", 0 },
	{ "LENOVO",			"8920*",	"",		"", 0 },
	{ "LENOVO",			"8927*",	"",		"", 0 },
	{ "LENOVO",			"8928*",	"",		"", 0 },
	{ "LENOVO",			"8929*",	"",		"", 0 },
#endif
	/* T61 */
	{ "LENOVO",			"6463*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6464*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6465*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6466*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6467*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6468*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6471*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8938*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8939*",	"",		"", S3_BIOS|S3_MODE },
	
	{ "LENOVO",			"7658*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7659*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7660*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7661*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6378*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6379*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6480*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8895*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8896*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8897*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8898*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8899*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8900*",	"",		"", S3_BIOS|S3_MODE },
	/* R61 */
	{ "LENOVO",			"7732*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7733*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7734*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7735*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7736*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7737*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7738*",	"",		"", S3_BIOS|S3_MODE },
	/* X61s */
	{ "LENOVO",			"7666*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7667*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7668*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7669*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7670*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7671*",	"",		"", S3_BIOS|S3_MODE },
	/* X61*/
	{ "LENOVO",			"7673*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7674*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7675*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7676*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7678*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7679*",	"",		"", S3_BIOS|S3_MODE },
	/* T61 / R61 2008-01-29, bug https://bugzilla.novell.com/show_bug.cgi?id=334522#c10 */
	{ "LENOVO",			"4108*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"4141*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"4143*",	"",		"", S3_BIOS|S3_MODE },
#if 0
	{ "LENOVO",			"4109*",	"",		"", 0 },
	{ "LENOVO",			"4142*",	"",		"", 0 },
	{ "LENOVO",			"4144*",	"",		"", 0 },
#endif
	/* https://bugzilla.novell.com/show_bug.cgi?id=353896 */
	/*ThinkPad X61 Tablet*/
	{ "LENOVO",			"7762*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7763*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7764*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7767*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7768*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7769*",	"",		"", S3_BIOS|S3_MODE },
	/*ThinkPad R61*/							
	{ "LENOVO",			"7642*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7643*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7644*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7645*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7646*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7647*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7648*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7649*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7650*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"7657*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8930*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8932*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8933*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8934*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8935*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8936*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8937*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8942*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8943*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8944*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8945*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8946*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8947*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"8948*",	"",		"", S3_BIOS|S3_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=353896 end */
        /* LENOVO X300, 2008-04-03 https://bugzilla.novell.com/show_bug.cgi?id=370690 */
	{ "LENOVO",			"6476*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6477*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"6478*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"2478*",	"",		"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"2479*",	"",		"", S3_BIOS|S3_MODE },
	/* EndOfLenovoBunch */
	
	/* X60 / X60s */
	{ "LENOVO",			"1702*",		"",	"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"1704*",		"",	"", S3_BIOS|S3_MODE },
	{ "LENOVO",			"1706*",		"",	"", S3_BIOS|S3_MODE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=210928, Thinkpad R60, 32bit? */
	{ "LENOVO",			"9456*",		"",	"", S3_BIOS|S3_MODE },
	/* Marcus Better <marcus@better.se>, this time with a capital "P" */
	{ "LENOVO",			"",		"ThinkPad R60",	"", S3_BIOS|S3_MODE },
	/* From: gregor herrmann <gregor.herrmann@comodo.priv.at> */
	{ "LENOVO",			"",		"Thinkpad R60e", "", S3_BIOS|S3_MODE },
	/* Dave Royal <dave@daveroyal.com>, R60e 32bit works with S3_MODE, 64bit needs VBE_MODE */
	{ "LENOVO",			"0657*",		"",	"", S3_BIOS|VBE_MODE },
	/* X60 Tablet, Carlos Lange, https://bugzilla.novell.com/show_bug.cgi?id=265613#c12 */
	{ "LENOVO",			"6363*",		"",	"", S3_BIOS|S3_MODE },
	/* X60 Tablet, Gary Ekker, https://bugzilla.novell.com/show_bug.cgi?id=265613 */
	{ "LENOVO",			"6364*",		"",	"", S3_BIOS|S3_MODE },
	/* X60 Tablet, Aaron Denney <wnoise@ofb.net>, needs at least BIOS "7JET25WW (1.10 )"*/
	{ "LENOVO",			"6365*",		"",	"", S3_BIOS|S3_MODE },
	/*  Florian Wagner <florian@wagner-flo.net>, X60 Tablet */
	{ "LENOVO",			"6366*",		"",	"", S3_BIOS|S3_MODE },
	/* Joerg Rueppel <sharky-x@gmx.net>, T61p, does not work with the NVidia driver */
	{ "LENOVO",			"6457*",		"",	"", S3_BIOS|VBE_MODE },
	/* T61p, leighton 5 <ng1lei@gmail.com>, ... what's the difference to the other T61p? */
	{ "LENOVO",			"6459*",		"",	"", 0 },
	{ "LENOVO",			"6460*",		"",	"", S3_BIOS|VBE_MODE },
	/* Amit Joshi <ajoshi@optonline.net>, T61 */
	{ "LENOVO",			"6465*",		"",	"", S3_BIOS|VBE_MODE },
	/* T61, Joe Nahmias <jello@debian.org>, debian bug #432018 */
	{ "LENOVO",			"7658*",	"",		"", S3_BIOS|S3_MODE },
	/* T61 with NVidia card, https://bugzilla.novell.com/show_bug.cgi?id=290618 */
	{ "LENOVO",			"7663*",	"",		"", S3_MODE },
	/* T61, Aku Pietikainen <aku.pietikainen@ee.oulu.fi>, works with console and X */
	{ "LENOVO",			"7664*",	"",		"", 0 },
	/* Rasmus Borup Hansen <rbh@math.ku.dk>, X61s reported 7666 w. S3_BIOS */
	/* Anders Engstrom <ankaan@gmail.com> reported 7666 w. S3_BIOS|S3_MODE */
	{ "LENOVO",			"7666*",	"",		"", S3_BIOS|S3_MODE },
	/* X61s, Hendrik-Jan Heins <hjh@passys.nl>. Probably does not restore the
	   framebuffer mode correctly, but S3_MODE as well as VBE_MODE seem to fail
	   sometimes on x86_64 on this machine :-( */
	{ "LENOVO",			"7669*",	"",		"", S3_BIOS },
	/* X61, Marco Barreno <barreno@cs.berkeley.edu>, works without any hack */
	{ "LENOVO",			"7673*",	"",		"", 0 },
	/* X61, Holger Hetterich <hhetter@novell.com> */
	{ "LENOVO",			"7674*",	"",		"", S3_BIOS|S3_MODE },
	/* X61, Jan Grmela <grmela@gmail.com> */
	{ "LENOVO",			"7675*",	"",		"", S3_BIOS|S3_MODE },
	/* Calvin Walton <calvin.walton@gmail.com>, R61i */
	{ "LENOVO",			"7732*",	"",		"", S3_BIOS },
	/* <dancingseb@yahoo.de>, R61 */
	{ "LENOVO",			"7738*",	"",		"", S3_BIOS|S3_MODE },
	/* Stephane Delcroix https://bugzilla.novell.com/show_bug.cgi?id=362945 */
	{ "LENOVO",			"7762*",	"",		"", S3_BIOS|S3_MODE },
	/* Danny Kukawka <dkukawka@suse.de>  ThinkPad X61 Tablet */
	{ "LENOVO",			"7768*",	"",		"", S3_BIOS|S3_MODE },
	/* T61, intel card <seife@suse.de> 32bit works with S3_MODE, but 64bit needs VBE_MODE */
	{ "LENOVO",			"8895*",	"",		"", S3_BIOS|VBE_MODE },
	/* Grzegorz Piektiewicz <gpietkie@proteus.pl>, R61, with nvidia driver */
	{ "LENOVO",			"8918*",	"",		"", 0 },
	/* <ruedt@physik.fu-berlin.de>, R61, 64bit, NVidia driver does not work */
	{ "LENOVO",			"8919*",	"",		"", S3_BIOS|S3_MODE },
	/* peter.pille@tektronix.com, R61 */
	{ "LENOVO",			"8943*",	"",		"", S3_BIOS|VBE_MODE },
	/* Rick van Hattem <Rick.van.Hattem@fawo.nl>, Z61p*/
	{ "LENOVO",			"9452*",	"",		"", S3_BIOS|S3_MODE },
	/* At least 1709 series thinkpads can be whitelisted by name,
	   and we can probably delete entries above, too.... */
	{ "LENOVO",			"",		"ThinkPad X60",	"", S3_BIOS|S3_MODE },
	/* T60p, had a report from a 2007* and a 2008* model */
	{ "LENOVO",			"",	"ThinkPad T60p",	"", S3_BIOS|S3_MODE },
	/* T60 From: Jeff Nichols <jnichols@renkoo.net> and Kristofer Hallgren <kristofer.hallgren@gmail.com> */
	{ "LENOVO",			"",	"ThinkPad T60",		"", S3_BIOS|S3_MODE },
	/* T61, 7658Q4U, From: Frank Seidel <fseidel@suse.de> */
	{ "LENOVO",			"7658*", "ThinkPad T61",	"", S3_BIOS|S3_MODE },
	/* Paul Wilkinson <pwilko@gmail.com> */
	{ "LENOVO",			"",	"ThinkPad Z61p",	"", S3_BIOS|S3_MODE },
	/* Z61m From: Roderick Schertler <roderick@argon.org> */
	{ "LENOVO",			"",	"ThinkPad Z61m",	"", S3_BIOS|S3_MODE },
	
	/* LENOVO non-Thinkpads go here... */
	/* Xu Zhe <x2446421894@gmail.com> */
	{ "Lenovo",			"Soleil E660P *",	"",	"", 0 },
	/* Leo G <chudified@gmail.com> */
	{ "LENOVO",			"068928U",	"3000 N100 *",	"", 0 },
	/* AFAICT the 3000 C100 is only available with intel graphics */
	/* Luc Levain <luc.levain@yahoo.co.uk>, works with old and new kernel. */
	{ "LENOVO",			"0761*",	"3000 C100 *",	"", 0 },
	/* Sebastian Schleehauf <S.Schleehauf@gmx.de>, 3000V100 */
	{ "LENOVO",		"0763*",	"LENOVO3000 V100",	"", 0 },
	/* these Lenovo 3000 N100 model 768 are available with different graphics chips, so
	   i do not dare to wildcard anything here :-( */
	/* https://bugzilla.novell.com/show_bug.cgi?id=299099 */
	{ "LENOVO",			"076804U",	"3000 N100 *",	"", S3_BIOS|VBE_MODE },
	/* Aviv Ben-Yosef <aviv.by@gmail.com> */
	{ "LENOVO",			"07686VG",	"3000 N100 *",	"", 0 },
	/* Bogdan <boogi77@o2.pl> */
	{ "LENOVO",			"076831G",	"3000 N100 *",	"", 0 },
	/* Tim Creech <dezgotspam@gmail.com> */
	{ "LENOVO",			"076835U",	"3000 N100 *",	"", 0 },
	/* Ted Percival <ted@midg3t.net> */
	{ "LENOVO",			"07687MM",	"3000 N100 *",	"", S3_MODE },
	/* Johannes Rohr <jorohr@gmail.com> */
	{ "LENOVO",			"0768BYG",	"3000 N100 *",	"", 0 },
	/* I'm not sure how to handle the 3000 N200 models... */
	/* Thorsten Frey <thorsten@tfrey.de> */
	{ "LENOVO",			"0769AC6",	"3000 N200 *",	"", 0 },
	/* Hans Gunnarsson <hans.gunnarsson@gmail.com> */
	{ "LENOVO",			"0769B9G",	"3000 N200 *",	"", 0 },
	/* Stojan Jakotic <stojanjakotic@centrum.cz> */
	{ "LENOVO",			"0769BBG",	"3000 N200 *",	"", S3_BIOS|S3_MODE },
	/* ThinkCentre M57p, <Joerg.Breuninger@de.bosch.com>, intel gfx, kernel 2.6.25.5 */
	{ "LENOVO",			"6088*",	"",		"", 0 },

	/* merkur@users.sourceforge.net LM50a */
	{ "LG Electronics",		"LM50-DGHE",		"",	"", S3_BIOS|S3_MODE },
	/* jsusiluo@mappi.helsinki.fi */
	{ "LG Electronics",		"LW75-SGGV1",		"",	"", S3_BIOS|S3_MODE },
	{ "LG Electronics",		"M1-3DGBG",		"",	"", S3_BIOS|S3_MODE },
	/* Neysan Schaefer <mail@neysan.de> */
	{ "LG Electronics",		"R500-U.CPCBG",		"",	"", 0 },
	/* seife */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-51E*",	"",	"", VBE_POST|VBE_MODE },
	/* Hanspeter Kunz <buzz.rexx@gmail.com> */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-R3E*",	"002",	"", VBE_POST|VBE_MODE },
	/* https://sourceforge.net/tracker/index.php?func=detail&aid=1601715&group_id=45483&atid=443088 */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-R2C*",	"0032",	"", VBE_POST|VBE_MODE },
	/* Hanspeter Kunz <hkunz@ifi.uzh.ch> */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-R6A*",	"",	"", VBE_POST|VBE_MODE },
	/* Itsuo HATONO <hatono@kobe-u.ac.jp> */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-R6M*",	"",	"", VBE_POST|VBE_MODE },
	/* Michal Krasucki <mkrasucki@gmail.com> */
	{ "Matsushita Electric Industrial Co.,Ltd.", "CF-W4G*",	"",	"", 0 },
	/* Joost Witteveen <joost@iliana.nl> */
	{ "MAXDATA",			"IMPERIO4045A *",	"",	"", 0 },
	/* Jan Gerrit <JanGerrit@Burg-Borgholz.de> */
	{ "MAXDATA",			"PRO600IW",		"",	"", S3_BIOS|S3_MODE },
	/* Oleg Broytmann <phd@phd.pp.ru> */
	{ "MaxSelect",			"Mission_A330",		"",	"", VBE_POST|VBE_SAVE },
	/* Nicolas HENRY <nicolas.hy@free.fr>, nvidia binary driver */
	{ "MEDION",			"M295M",		"",	"", 0 },
	/* Leszek Lesner <leszek.lesner@googlemail.com> Medion MD43100 */
	{ "MEDIONNB       ",		"WID2010        ",	"",	"", VBE_POST|VBE_MODE },
	/* Ralf Auer <ralf.auer@physik.uni-erlangen.de>, tested both with nv and binary nvidia */
	{ "MEDIONNB       ",		"WIM 2000       ",	"",	"", 0 },
	/* Alberto Gonzalez <info@gnebu.es> */
	{ "MEDIONPC",			"MS-6714",		"",	"", S3_BIOS|S3_MODE },
	/* Mike Galbraith <efault@gmx.de> needs X to get text console back */
	{ "MEDIONPC",			"MS-7012",		"",	"", 0 },
	/* Andreas Kostyrka <andreas@kostyrka.org> */
	{ "MICRO-STAR INT'L CO.,LTD",	"MS-1012",		"",	"", S3_BIOS },
	/* Matthijs Kooijman <matthijs@stdin.nl> */
	{ "MICRO-STAR INT'L CO.,LTD",	"MS-1013",		"",	"", S3_BIOS },
	/* Gergely Csepany <cheoppy@gmail.com> */
	{ "MICRO-STAR INT'L CO.,LTD.",	"MS-1057",		"",	"", S3_BIOS|S3_MODE },
	/* Enno Wein <ennowein@yahoo.com> */
	{ "Micro-Star International",	"PR210",	"MS-1222X",	"", VBE_POST },
	/* Baruch Even <baruch@ev-en.org> */
	{ "Micro-Star International",	"MS-1412",		"",	"", VBE_POST|VBE_SAVE|NOFB },
	/* Matej Tyc <matej.tyc@gmail.com> */
	{ "MSI",			"MS-6417",		"",	"", 0 },
	/* uswsusp@faxm0dem.org */
	{ "MSI.",			"MS-7207PV",		"",	"", 0 },
	/* Petr Pudlak <naradapuri@gmail.com> */
	{ "To Be Filled By O.E.M.",	"MS-7210",		"100",	"", 0 },
	/* Nicholas T <ntung@ntung.com>, mainboard */
	{ "MSI",			"MS-7350",		"",	"", 0 },
	/* "Alex V. Myltsev" <avm@altlinux.ru */
	{ "NCA_GROUP_LTD",		"iRU_Notebook",		"0106",	"", VBE_POST|VBE_MODE },
	/* Hugo Costelha */
	{ "NEC *",			"B7 *",			"",	"", VBE_SAVE }, 
	/* Benoit Thiell <badzil@gmail.com> */
	{ "NEC COMPUTERS INTERNATIONAL","NEC Versa Premium",	"FAB-2","", 0 },
	/* Nicky Gerritsen <nicky@xept.nl> */
	{ "NEC Computers International *",	"VC2",		"",	"", S3_BIOS|S3_MODE },
	/* Ronny Radke <ronnymichaelradke@web.de> medion MIM2080 */
	{ "NOTEBOOK",			"MIM2080",		"",	"", S3_BIOS|S3_MODE },
	/* Jan Willies <jan@willies.info> medion md 96400 */
	{ "NOTEBOOK",			"SAM2000",		"0131",	"", S3_BIOS|S3_MODE },
	/* Thomas Braun <thomas.braun@newthinking.de>, rebranded Uniwill P53IN */
	{ "OEM",			"P53IN4",		"",	"", 0 },
	/* Wolf Geldmacher <wolf@womaro.ch> */
	{ "OQO",			"ZEPTO",		"00",	"", VBE_POST|VBE_SAVE },
	/* NOTE: I DO NOT TRUST THOSE PACKARD BELL DMI STRINGS!
	         but i don't have a better idea right now. So i am only taking
	         very exact matches for them now  -- seife */
	/* Per Waago <waago@stud.ntnu.no> */
	{ "Packard Bell NEC",	"00000000000000000000000", "P820008416","", 0 },
	/* Angelo Anfossi <aanfossi@gmail.com> "Easy Note A6010" */
	{ "Packard Bell NEC",	"00000000000000000000000",	"",	"A03 ", 0 },
	/* Pavol Otcenas <Pavol.Otcenas@2c.cz>, "Prestigio Nobile 159W" */
	{ "To Be Filled By O.E.M.", "259IA1", "To Be Filled By O.E.M.", "080010 ", S3_MODE },
	/* Florian Tham <ftham@unlimitedmail.org> Samsung P35 */
	{ "SAMSUNG",			"CoronaR",		"03RK",	"", 0 },
	/* Bastian Pfau <bastian.pfau@bessy.de> Samsung P35 */
	{ "SAMSUNG",			"CoronaR",		"04RK",	"", VBE_POST|VBE_MODE },
	/* Jeremie Delaitre <jeremie.delaitre@gmail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"Q35/Q36",	"",	"", S3_BIOS|S3_MODE },
	/* Eduardo Robles Elvira <edulix@gmail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"R40/R41",	"",	"", VBE_SAVE },
	/* Tim Fischer <mail0812@online.de> works with and without ATI binary driver */
	{ "SAMSUNG ELECTRONICS CO.,LTD",	"R50/R51 *",	"",	"", 0 },
	/* Daniel Koester <koester@math.uni-augsburg.de> does not yet work from text mode */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"R55S *",	"",	"", 0 },
	/* Stephen Denham <stephen.denham@gmail.com> */
	{ "SAMSUNG Electronics",	"SENS V25",		"",	"", VBE_POST|VBE_MODE },
	/* Gautier Hattenberger <ghattenb@laas.fr> */
	{ "Samsung Electronics",	"SM40S",		"",	"", S3_BIOS|S3_MODE },
	/* Pavel Machek, Samsung UMPC */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ1US",	"01ME",	"", VBE_SAVE },
	/* Maximilian Mehnert <maximilian.mehnert@googlemail.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ35S",	"",	"", VBE_POST },
	/* Frank Ruell <frank@ruell.com> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SQ45S70S",	"",	"", S3_BIOS|S3_MODE },
	/* Jens Ott <jo@opteamax.de> */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SR700",	"",	"", 0 },
	/* Konstantin Krasnov <krkhome@gmail.com>, only with kernel 2.6.24++ */
	{ "SAMSUNG ELECTRONICS CO., LTD.",	"SX22S",	"07AJ", "", VBE_SAVE },
	/* Martin Wilck <mwilck@arcor.de> */
	{ "Samsung Electronics",	"SX50S",	"Revision MP",	"", VBE_POST },
	/* <gregor.willms@web.de> & Frederic Boiteux <fboiteux@free.fr> */
	{ "Shuttle Inc",		"SD11V10",		"",	"", S3_BIOS|S3_MODE },
	/* Tejun Heo, https://bugzilla.novell.com/show_bug.cgi?id=390127 */
	{ "Shuttle Inc",		"SD30V10",		"",	"", S3_BIOS|S3_MODE },
	/* foren@nuxli.ch */
	{ "Shuttle Inc",		"SN41UV10",		"",	"", VBE_POST|VBE_MODE },
	/* Thomas Peuss <thomas@peuss.de> */
	{ "Shuttle Inc",		"SN85V10",		"",	"", 0 },

	/* Kevin Fullerton <kevin.fullerton@shotgun-suicide.co.uk> */
	{ "TOSHIBA",			"EQUIUM A100",		"",	"", S3_BIOS|S3_MODE },
	{ "TOSHIBA",			"Libretto L5/TNK",	"",	"", 0 },
	{ "TOSHIBA",			"Libretto L5/TNKW",	"",	"", 0 },
	/* Hans-Hermann Jopski <hans@jopski.de> Tecra A2 PTA20E-4M301VGR */
	{ "TOSHIBA",		"Portable PC",	"Version 1.0",	"Version 1.20", 0 },
	/* a PORTEGE 100 :-( mykhal@centrum.cz */
	{ "TOSHIBA",		"Portable PC",	"Version 1.0",	"Version 1.60", S3_MODE },
	/* this is a Toshiba Satellite 4080XCDT, believe it or not :-( */
	{ "TOSHIBA",		"Portable PC",	"Version 1.0",	"Version 7.80", S3_MODE },
	/* Pavel Borzenkov <pavel.borzenkov@gmail.com>, Toshiba Portege 3490CT, works correctly for me */
	{ "TOSHIBA",			"P3490",		"",	"", VBE_SAVE },
	/* Alan Mycroft <Alan.Mycroft@cl.cam.ac.uk> */
	{ "TOSHIBA",			"PORTEGE M300",		"",	"", S3_MODE },
	/* Mathieu Lacage <mathieu.lacage@sophia.inria.fr> */
	{ "TOSHIBA",			"PORTEGE R100",		"",	"", 0 },
	/* hole@gmx.li */
	{ "TOSHIBA",			"PORTEGE R200",		"",	"", S3_BIOS|S3_MODE },
	/* Bernhard Kausler <uabdc@stud.uni-karlsruhe.de> */
	{ "TOSHIBA",			"PORTEGE R500",		"",	"", S3_BIOS|S3_MODE },
	/* Fabio Olivares <fabio.olivares@alice.it> sys_version  = "PSA10E-018EZ-6A" */
	{ "TOSHIBA",			"Satellite A10",	"",	"", S3_BIOS|S3_MODE },

	{ "TOSHIBA",			"Satellite A30",	"",	"", VBE_SAVE },
	/* Diego Ocampo <diegoocampo8@gmail.com> */
	{ "TOSHIBA",			"Satellite A80",	"",	"", 0 },
	/* From: a.maurizi@isac.cnr.it */
	{ "TOSHIBA",			"Satellite A100",	"",	"", S3_BIOS|S3_MODE },
	/* Eric Siegel <nticompass@gmail.com>, intel chipset, sys_version="PSAA8U-0MD02K" */
	{ "TOSHIBA",			"Satellite A105",	"",	"", S3_BIOS|S3_MODE },
	/*  Jan Urbanski <j.urbanski@students.mimuw.edu.pl>, sys_version="PSAE0E-04600TPL" */
	{ "TOSHIBA",			"Satellite A200",	"",	"", S3_BIOS|S3_MODE },
	/* Gavin Kinsey <gavin.kinsey@gmail.com> */
	{ "TOSHIBA",			"Satellite A210",	"",	"", S3_BIOS|S3_MODE },
	/* David Picella <dpicella@gmail.com> sys_version  = "PSAFGU-048002" */
	{ "TOSHIBA",			"Satellite A215",	"",	"", S3_BIOS|S3_MODE|PCI_SAVE },
	/* Christian Schneeman <cschneemann@suse.de> */
	{ "TOSHIBA",			"Satellite L10",	"",	"", VBE_POST|VBE_MODE },
	/* Gijs van Gemert <g.v.gemert@inter.nl.net> */
	{ "TOSHIBA",			"Satellite L30",	"",	"", S3_BIOS|S3_MODE },
	/* Krzysztof Lubanski <luban@nerdshack.com>, sys_version = "PSMA0U-0F301U" */
	{ "TOSHIBA",			"Satellite M105",	"",	"", S3_BIOS|S3_MODE },
	/* tnt.rocket@freenet.de, need to verify if S3_MODE is needed too */
	{ "TOSHIBA",			"Satellite M30X",	"",	"", S3_BIOS },
	/* Michaell Gurski */
	{ "TOSHIBA",			"Satellite M35X",	"",	"", S3_BIOS|S3_MODE },
	/* Mike Gilliam <mike.tx@suddenlink.net> */
	{ "TOSHIBA",			"Satellite M55",	"",	"", VBE_POST|VBE_MODE },
	/* clist@uah.es */
	{ "TOSHIBA",			"Satellite M70",	"",	"", S3_BIOS|S3_MODE },
	/* Antonis Tsolomitis <atsol@aegean.gr> */
	{ "TOSHIBA",			"Satellite P10",	"",	"", S3_BIOS|S3_MODE },
	/* zu@ten.ch sys_version  = "PSPAGE-00H00RS4" */
	{ "TOSHIBA",			"Satellite P100",	"",	"", 0 },
	/* From: mire <mire@osadmin.com> */
	{ "TOSHIBA",			"Satellite P200",	"",	"", S3_BIOS|S3_MODE },
	/* cl.kerr@gmail.com */
	{ "TOSHIBA",			"Satellite P205D",	"",	"", S3_BIOS|S3_MODE },
	/* Stephen Jarrett-Sprague <sjarrettsprague@googlemail.com> sys_version="PSAC1E-0J3021EN" */
	{ "TOSHIBA",			"Satellite Pro A120",	"",	"", S3_BIOS|S3_MODE },
	/* Francisco Palomo Lozano <francisco.palomo@uca.es> sys_version="PSM75E-00G00TSP" */
	{ "TOSHIBA",			"Satellite Pro M70",	"",	"", S3_BIOS|S3_MODE },
	/* Neil Symington, https://bugzilla.novell.com/show_bug.cgi?id=386505, kernel 2.6.25-x86_64 */
	{ "TOSHIBA",			"Satellite Pro U300",	"",	"", 0 },
	/* From: Jim <jim.t@vertigo.net.au> */
	{ "TOSHIBA",			"SatellitePro4600",	"",	"", 0 },
	/* Jean-Pierre Bergamin <james@ractive.ch>, kernel 2.6.25.3, sys_version "PLUA0E-01C007S4" */
	{ "TOSHIBA",			"SATELLITE U200",	"",	"", 0 },
	/* Romano Giannetti <romanol@upcomillas.es> sys_version "PSU34U-00L003" */
	{ "TOSHIBA",			"Satellite U305",	"",	"", VBE_POST|VBE_MODE },
	/* Bram Senders <bram@luon.net> */
	{ "TOSHIBA",			"SP4600",	"Version 1.0",	"", 0 },
	/* Thomas Zander <zander@kde.org> sys_version "PTA83E-0E303LDU" */
	{ "TOSHIBA",			"TECRA A8",		"",	"", 0 },
	/* Arne Salveter <arne.salveter@googlemail.com> sys_version "PTS52E-00X00GGR" */
	{ "TOSHIBA",			"TECRA A9",		"",	"", 0 },
	/* Dr. Stephan Buehne <stephan.buehne@oracle.com> */
	{ "TOSHIBA",			"TECRA M9",		"",	"", S3_MODE },
	/* Jad Naous <jnaous@stanford.edu> */
	{ "TOSHIBA",			"TECRA S1",		"",	"", 0 },
	{ "TOSHIBA",			"TECRA S3",		"",	"", 0 },
	/* From: a v <adam_6515@hotmail.com> */
	{ "TOSHIBA",			"TECRA8100",		"",	"", S3_MODE },
	/* Stefan Seyfried has one of those :-) S3_BIOS leads to "melting screen" */
	{ "TOSHIBA",			"TECRA 8200",		"",	"", S3_MODE },
	/* Silverio Santos <SSantos@web.de>, Tecra 9000 */
	{ "TOSHIBA",			"T9000",		"",	"", VBE_MODE },
	/* Arnim Laeuger <arnim.laeuger@gmx.net> */ 
	{ "Samsung Electronics",	"NX05S*",		"",	"", VBE_POST|VBE_MODE },
	{ "Samsung",			"SQ10",			"",	"", VBE_POST|VBE_SAVE },
	/* https://bugzilla.novell.com/show_bug.cgi?id=290734 */
	{ "Samsung Electronics",	"SX20S",		"",	"", VBE_POST|VBE_MODE },
	{ "SHARP                           ",	"PC-AR10 *",	"",	"", 0 },
	/* Daniel Sabanes Bove <daniel.sabanesbove@campus.lmu.de> */
	{ "Sony Corporation",		"VGN-A115B*",		"",	"", 0 },
	/* Dean Darlison <dean@dasco.ltd.uk>, maybe only with nvidia driver */
	{ "Sony Corporation",		"VGN-AR31S",		"",	"", S3_BIOS|S3_MODE },
	/* Daniel Morris <danielm@eCosCentric.com> */
	{ "Sony Corporation",		"VGN-B1XP*",		"",	"", S3_BIOS },
	/* Dhananjaya Rao <phaze87@gmail.com> */
	{ "Sony Corporation",		"VGN-B55G(I)",		"",	"", 0 },
	/* Jorge Visca <xxopxe@gmail.com> */
	{ "Sony Corporation",		"VGN-C140G",		"",	"", S3_BIOS|S3_MODE },
	/* Tijn Schuurmans <t.schuurmans@hccnet.nl> */
	{ "Sony Corporation",		"VGN-C2S_G",		"",	"", S3_BIOS },
	/* David Durrleman <david.durrleman@ens.fr>, only from X :-( */
	{ "Sony Corporation",		"VGN-FE11M",		"",	"", 0 },
	/* Gert Huizenga <gert@hbrothers.nl> */
	{ "Sony Corporation",		"VGN-FE21B",		"",	"", S3_BIOS },
	/* <lapseofreason0@gmail.com>, only from X, S3_MODE hangs the machine, all other leave the screen blank */
	{ "Sony Corporation",		"VGN-FE31M",		"",	"", 0 },
	{ "Sony Corporation",		"VGN-FS115B",		"",	"", S3_BIOS|S3_MODE },
	/* Olivier Saier <osaier@gmail.com>, apparently S3_BIOS locks the machine hard */
	{ "Sony Corporation",		"VGN-FS115Z",		"",	"", S3_MODE },
	/* S.Çağlar Onur <caglar@pardus.org.tr> */
	{ "Sony Corporation",		"VGN-FS215B",		"",	"", 0 },
	/* Tim Felgentreff <timfelgentreff@gmail.com> */
	{ "Sony Corporation",		"VGN-FS485B",		"",	"", S3_BIOS|S3_MODE },
	/* Joseph Smith <joe@uwcreations.com> */
	{ "Sony Corporation",		"VGN-FS660_W",		"",	"", 0 },
	/* Stefano Avallone <stavallo@unina.it> */
	{ "Sony Corporation",		"VGN-FZ39VN",		"",	"", 0 },
	/* daryl@mail.bg */
	{ "Sony Corporation",		"VGN-SZ340P",		"",	"", 0 },
	/* Pedro M. S. Oliveira <pmsoliveira@gmail.com>, machine has two gfx cards,
	   works only well with the intel, not with the nvidia */
	{ "Sony Corporation",		"VGN-SZ5XN_C",		"",	"", 0 },
	/* Michal Bozek <michal.bozek@gmail.com> */
	{ "Sony Corporation",		"VGN-SZ61XN_C",		"",	"", S3_BIOS|S3_MODE },
	/* Anton Zahlheimer <anton.zahlheimer@landshut.org> */
	{ "Sony Corporation",		"VGN-T250P",		"",	"", S3_BIOS|S3_MODE },
	/* Timo Hoenig <thoenig@suse.de> VGN-TX3HP */
	{ "Sony Corporation",		"VGN-TX3HP",		"",	"", 0 },
	/* Stephan Martin <stephan.martin@open-xchange.com> */
	{ "Sony Corporation",		"VGN-TX3XP_L",		"",	"", S3_BIOS|S3_MODE },
	/* Danny Kukawka <dkukawka@suse.de>, TX5MN from Michael Matz <matz@novell.com> */
	{ "Sony Corporation",		"VGN-TX5MN_W",		"",	"", VBE_MODE },
	
	{ "Sony Corporation",		"VGN-TX770P",		"",	"", VBE_POST|VBE_MODE },
	/* Pier Paolo Pittavino <pittavin@studenti.ph.unito.it>, Timo Hoenig <thoenig@suse.de> */
	{ "Sony Corporation",		"VGN-TZ11*",		"",	"", 0 },
	/* Hans Gunnarsson <hans.gunnarsson@gmail.com> */
	{ "Sony Corporation",		"VGN-TZ21XN_B",		"",	"", 0 },
	/* matt@thewaffleproject.com */
	{ "Sony Corporation",		"VGN-TZ37*",		"",	"", S3_BIOS|S3_MODE },
	/* Raymond Russell <raymond@corvil.com> */
	{ "Sony Corporation",		"VGN-TZ170N",		"",	"", S3_BIOS|S3_MODE },
	/* Jan P. O. Schuemann" <jan@hep1.phys.ntu.edu.tw> */
	{ "Sony Corporation",		"VGN-TZ91HS",		"",	"", 0 },
	/* Mattia Dongili <malattia@linux.it> */
	{ "Sony Corporation",		"VGN-UX50",		"",	"", VBE_MODE },
	/* Russ Dill <russ.dill@gmail.com> */
	{ "Sony Corporation    ",	"PCG-F430(UC)        ",	"",	"", 0 },
	/* Brice Mealier <mealier_brice@yahoo.fr> */
	{ "Sony Corporation",		"PCG-FR215E*",		"",	"", 0 },
	/* Eric Wasylishen <ewasylishen@gmail.com>, really needs VBE_SAVE */
	{ "Sony Corporation",		"PCG-FX340*",		"",	"", VBE_SAVE },
	/* Sitsofe Wheeler <sitsofe@yahoo.com> */
	{ "Sony Corporation",		"PCG-FX405*",		"",	"", S3_BIOS },
	/* Stefan Nobis <stefan@snobis.de> */
	{ "Sony Corporation",		"PCG-FX601*",		"",	"", S3_BIOS|S3_MODE },
	/* Loki Harfagr <l0k1@free.fr> */
	{ "Sony Corporation",		"PCG-FX802*",		"",	"", VBE_SAVE|VBE_MODE },
	/* Stefan Friedrich <strfr@web.de> */
	{ "Sony Corporation",		"PCG-FX805*",		"",	"", VBE_POST|VBE_SAVE },
	/* Marco Nenciarini <mnencia@prato.linux.it> */
	{ "Sony Corporation",		"PCG-GRT916Z*",		"",	"", 0 },
	{ "Sony Corporation",		"PCG-GRT995MP*",	"",	"", 0 },
	/* Mattia Dongili <malattia@linux.it> */
	{ "Sony Corporation    ",	"PCG-GR7_K(J)        ",	"",	"", 0 },
	/* Bruce Perens <bruce@perens.com> */
	{ "Sony Corporation",		"PCG-TR2A(UC)",		"",	"", VBE_MODE },
	/* mathieu bouillaguet <mathieu.bouillaguet@gmail.com> */
	{ "Sony Corporation",		"PCG-Z1RMP*",		"",	"", VBE_POST|VBE_MODE },
	/* Tom Lear <tom@trap.mtview.ca.us> */
	{ "Sony Corporation",		"PCG-Z505HSK(UC)",	"",	"", 0 },
	/* jmj_sf@freenet.de */
	{ "Sony Corporation",		"PCV-RX612*",		"",	"", S3_BIOS|S3_MODE },

	/* Gregory Colpart <reg@evolix.fr> 'Transtec Levio 210' */
	{ "transtec AG",		"MS-1057",		"",	"", S3_BIOS|S3_MODE },
	/* Joachim Gleissner */
	{ "transtec AG",		"MS-1034",		"",	"", S3_BIOS|S3_MODE },

	/* Clevo D470K, Jasper Mackenzie <scarletpimpernal@hotmail.com> */
	{ "VIA",			"K8T400",	"VT8204B ",	"", S3_BIOS|S3_MODE },
	/* Dolezal Lukas <dolezallukas@sssvt.cz> */
	{ "VIA Technologies, Inc.",	"KT400-8235",		"",	"", S3_BIOS|S3_MODE },
	/* <dev_temp@ukr.net> */
	{ "VIA Technologies, Inc.",	"KT400A-8235",		"",	"", S3_BIOS|S3_MODE },
	/* VIA EPIA M Mini-ITX Motherboard with onboard gfx, reported by Monica Schilling */
	{ "VIA Technologies, Inc.",	"VT8623-8235",		"",	"", S3_MODE },

	/* Rene Seindal <rene@seindal.dk> */
	{ "ZEPTO",			"ZNOTE *",	"3215W *",	"", 0 },
	/* simon kaucic <simon.kaucic@gmail.com> */
	{ "                    ",	"M37EW",		"None",	"", S3_BIOS },

	// entries below are imported from acpi-support 0.59 and though "half known".
	{ "ASUSTeK Computer Inc.",	"L7000G series Notebook PC*", "","", VBE_POST|VBE_SAVE|UNSURE },
	{ "ASUSTeK Computer Inc.",	"W5A*",			"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"TravelMate 290*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"TravelMate 660*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer",			"Aspire 2000*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Acer, inc.",			"TravelMate 8100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
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
	{ "IBM",			"2648*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T23
	{ "IBM",			"475S*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// T40/T41/T42/p
	{ "IBM",			"2375*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
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
	{ "IBM",			"2884*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2885*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2890*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	{ "IBM",			"2891*",		"",	"", VBE_POST|VBE_SAVE|UNSURE|RADEON_OFF },
	// X40
	{ "IBM",			"2369*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2370*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2372*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2382*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	// X41
	{ "IBM",			"1864*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"1865*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2527*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "IBM",			"2528*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },

	{ "SHARP Corporation",		"PC-MM20 Series*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "Sony Corporation",		"PCG-U101*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },

	{ "TOSHIBA",			"libretto U100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"P4000*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE A100*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE A200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"PORTEGE M200*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"Satellite 1900*",	"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA A2*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA A5*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ "TOSHIBA",			"TECRA M2*",		"",	"", VBE_POST|VBE_SAVE|UNSURE },
	{ NULL, NULL, NULL, NULL, 0 }
};
