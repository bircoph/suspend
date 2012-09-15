#
# configure.ac
#
# Copyright (C) 2007 Alon Bar-Lev <alon.barlev@gmail.com>
#
# This file is released under the GPLv2.
#

AC_PREREQ([2.59])
AC_INIT([suspend-utils], [1.0])
AM_INIT_AUTOMAKE([${PACKAGE_NAME}], [${PACKAGE_VERSION}])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([suspend.c])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AC_ARG_ENABLE(
	[debug],
	[AC_HELP_STRING([--enable-debug], [enable debug mode])],
	,
	[enable_debug="no"]
)
AC_ARG_ENABLE(
	[strict],
	[AC_HELP_STRING([--enable-strict], [enable strict compile mode])],
	,
	[enable_strict="no"]
)
AC_ARG_ENABLE(
	[pedantic],
	[AC_HELP_STRING([--enable-pedantic], [enable pedantic compile mode])],
	,
	[enable_pedantic="no"]
)
AC_ARG_ENABLE(
	[minimal],
	[AC_HELP_STRING([--enable-minimal], [enable minimal build])],
	,
	[enable_minimal="no"]
)
AC_ARG_ENABLE(
	[compress],
	[AC_HELP_STRING([--enable-compress], [enable compress support])],
	,
	[enable_compress="no"]
)
AC_ARG_ENABLE(
	[encrypt],
	[AC_HELP_STRING([--enable-encrypt], [enable encryption support])],
	,
	[enable_encrypt="no"]
)
AC_ARG_ENABLE(
	[create-device],
	[AC_HELP_STRING([--enable-create-device], [enable create required devices, use only if you don't have udev])],
	,
	[enable_create_device="no"]
)
AC_ARG_ENABLE(
	[resume-static],
	[AC_HELP_STRING([--disable-resume-static], [build resume as dynamic module])],
	,
	[enable_resume_static="yes"]
)
AC_ARG_ENABLE(
	[splashy],
	[AC_HELP_STRING([--enable-splashy], [enable splashy support])],
	,
	[enable_splashy="no"]
)
AC_ARG_ENABLE(
	[fbsplash],
	[AC_HELP_STRING([--enable-fbsplash], [enable fbsplash support])],
	,
	[enable_fbsplash="no"]
)
AC_ARG_WITH(
	[devdir],
	[AC_HELP_STRING([--with-devdir=DIR], [use if --enable-create-device, put devices in this directory, default /dev])],
	[devdir="${withval}"],
	[devdir="/dev"]
)
AC_ARG_ENABLE(
	[threads],
	[AC_HELP_STRING([--enable-threads], [enable threads support])],
	,
	[enable_threads="no"]
)
AC_ARG_WITH(
	[initramfsdir],
	[AC_HELP_STRING([--with-initramfsdir=DIR], [put initramfs binaries in this directory, default LIBDIR/suspend])],
	[initramfsdir="${withval}"],
	[initramfsdir="\$(libdir)/suspend"]
)
AC_ARG_WITH(
	[libgcrypt-prefix],
	[AC_HELP_STRING([--with-libgcrypt-prefix=DIR], [define libgcrypt prefix, default /usr])],
	,
	[with_libgcrypt_prefix="/usr" ]
)
AC_ARG_WITH(
	[directfb-prefix],
	[AC_HELP_STRING([--with-directfb-prefix=DIR], [define directfb prefix, default /usr])],
	,
	[with_directfb_prefix="/usr" ]
)
AC_ARG_WITH(
	[resume-device],
	[AC_HELP_STRING([--with-resume-device=DEVICE], [define resume device (REQUIRED ONLY FOR BUILDING INITRAMFS DURING MAKE)])],
	[RESUME_DEVICE="${withval}"]
)
AC_ARG_WITH(
	[bootdir],
	[AC_HELP_STRING([--with-bootdir=DIR], [define boot directory, default /boot (REQUIRED ONLY FOR BUILDING INITRAMFS DURING MAKE)])],
	[bootdir="${withval}"],
	[bootdir="/boot" ]
)


AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL		# We are using libtool so it auto-find static dependencies
PKG_PROG_PKG_CONFIG
AC_CHECK_PROGS([M4], [m4])
AC_CHECK_PROG(PERL, perl, perl)

AC_SYS_LARGEFILE

if test "${enable_create_device}" = "yes"; then
	AC_CHECK_PROGS([MKNOD], [mknod])
	test -z "${MKNOD}" && AC_MSG_ERROR([mknod required for creating devices])
fi

case "${host}" in
	i?86-*)			ARCH="x86";;
	x86_*-*)		ARCH="x86";;
	ppc-*|powerpc-*)	ARCH="ppc";;
	ppc64-*|powerpc64-*)	ARCH="ppc";;
	s390*-*)		ARCH="s390";;
esac

CONFIG_FEATURES=""

if test "${ARCH}" = "x86"; then
	AC_DEFINE([CONFIG_ARCH_X86], [1], [Define if x86 arch])

	PKG_CHECK_MODULES(
		[LIBPCI],
		[libpci >= 2.2.4],
		,
		[
			if test -z "${LIBPCI_LIBS}"; then
				AC_CHECK_LIB(
					[pci],
					[pci_init],
					[LIBPCI_LIBS="-lpci"],
					[
					#force rescan
					unset ac_cv_lib_pci_pci_init
					AC_CHECK_LIB(
						[pci],
						[pci_init],
						[LIBPCI_LIBS="-lpci -lz"],
						[AC_MSG_ERROR([Required pciutils >= 2.2.4 not found])],
						[-lz]
					)]
				)
			fi
		]
	)

	if test -z "${LIBX86_LIBS}"; then
		AC_ARG_VAR([LIBX86_CFLAGS], [C compiler flags for libx86])
		AC_ARG_VAR([LIBX86_LIBS], [linker flags for libx86])
		AC_CHECK_LIB(
			[x86],
			[LRMI_common_init],
			[LIBX86_LIBS="-lx86"],
			[AC_MSG_ERROR([Required libx86 was not found])]
		)
	fi
elif test "${ARCH}" = "ppc"; then
	AC_DEFINE([CONFIG_ARCH_PPC], [1], [Define if ppc arch])
elif test "${ARCH}" = "s390"; then
	AC_DEFINE([CONFIG_ARCH_S390], [1], [Define if s390 arch])
else
	AC_MSG_ERROR([Unsupported architecture ${host}])
fi

if test "${enable_compress}" = "yes"; then
	CONFIG_FEATURES="${CONFIG_FEATURES} compress"
	AC_DEFINE([CONFIG_COMPRESS], [1], [Define if compression enabled])
	if test -z "${LZO_LIBS}"; then
		AC_ARG_VAR([LZO_CFLAGS], [C compiler flags for lzo])
		AC_ARG_VAR([LZO_LIBS], [linker flags for lzo])
		AC_CHECK_LIB(
			[lzo2],
			[lzo1x_1_compress],
			[LZO_LIBS="-llzo2"],
			[AC_CHECK_LIB(
				[lzo],
				[lzo1x_1_compress],
				[LZO_LIBS="-llzo"],
				[AC_MSG_ERROR([Required lzo library not found])]
			)]
		)
	fi
fi

if test "${enable_encrypt}" = "yes"; then
	CONFIG_FEATURES="${CONFIG_FEATURES} encrypt"
	AC_DEFINE([CONFIG_ENCRYPT], [1], [Define if encryption enabled])

	AC_MSG_CHECKING([for libgcrypt])
	if ! test -x "${with_libgcrypt_prefix}/bin/libgcrypt-config"; then
		AC_MSG_ERROR([Cannot locate libgcrypt])
	else
		AC_MSG_RESULT([found])
		LIBGCRYPT_CFLAGS=`"${with_libgcrypt_prefix}/bin/libgcrypt-config" --cflags`
		LIBGCRYPT_LIBS=`"${with_libgcrypt_prefix}/bin/libgcrypt-config" --libs`
	fi
fi

if test "${enable_splashy}" = "yes"; then
	CONFIG_FEATURES="${CONFIG_FEATURES} splashy"
	AC_DEFINE([CONFIG_SPLASHY], [1], [Define if splashy enabled])

	# Workaround a wierd splashy issue
	AC_CHECK_LIB([gcc_s], [strlen])

	if test -z "${SPLASHY_LIBS}"; then
		AC_ARG_VAR([SPLASHY_CFLAGS], [C compiler flags for libsplashy])
		AC_ARG_VAR([SPLASHY_LIBS], [linker flags for libsplashy])
		AC_CHECK_LIB(
			[splashy],
			[splashy_init],
			[SPLASHY_LIBS="-lsplashy"],
			[AC_MSG_ERROR([Required libsplashy was not found])]
		)
	fi
fi

if test "${enable_fbsplash}" = "yes"; then
	CONFIG_FEATURES="${CONFIG_FEATURES} fbsplash"
	AC_DEFINE([CONFIG_FBSPLASH], [1], [Define if fbsplash enabled])

	PKG_CHECK_MODULES(
		[FBSPLASH],
		[libfbsplashrender >= 0.1],
		,
		[AC_MSG_ERROR([Required libfbsplashrender was not found])]
	)
fi

if test "${enable_threads}" = "yes"; then
	CONFIG_FEATURES="${CONFIG_FEATURES} threads"
	AC_DEFINE([CONFIG_THREADS], [1], [Define if threads enabled])
	if test -z "${PTHREAD_LIBS}"; then
		AC_ARG_VAR([PTHREAD_LIBS], [linker flags for threads])
		AC_CHECK_LIB(
			[pthread],
			[pthread_create],
			[PTHREAD_LIBS="-lpthread"],
			[AC_MSG_ERROR([Required pthread library not found])]
		)
	fi
fi

AC_DEFINE_UNQUOTED([CONFIG_FEATURES], ["${CONFIG_FEATURES## }"], [String representation of available features])

AC_HEADER_STDC
AC_CHECK_HEADERS([ \
	inttypes.h \
	stdint.h \
])

if test "${enable_pedantic}" = "yes"; then
	enable_strict="yes";
	CFLAGS="${CFLAGS} -pedantic"
fi
if test "${enable_strict}" = "yes"; then
	CFLAGS="${CFLAGS} -Wall -Wextra"
fi

# workaround for old autoconf
if test -z "${docdir}"; then
	docdir="\$(datarootdir)/doc/\$(PACKAGE_NAME)"
	AC_SUBST([docdir])
fi
# end workaround

AC_SUBST([devdir])
AC_SUBST([initramfsdir])
AC_SUBST([bootdir])
AC_SUBST([LIBGCRYPT_CFLAGS])
AC_SUBST([LIBGCRYPT_LIBS])
AC_SUBST([RESUME_DEVICE])
AM_CONDITIONAL([ENABLE_DEBUG], [test "${enable_debug}" = "yes"])
AM_CONDITIONAL([ENABLE_MINIMAL], [test "${enable_minimal}" = "yes"])
AM_CONDITIONAL([ARCH_X86], [test "${ARCH}" = "x86"])
AM_CONDITIONAL([ARCH_PPC], [test "${ARCH}" = "ppc"])
AM_CONDITIONAL([ENABLE_ENCRYPT], [test "${enable_encrypt}" = "yes"])
AM_CONDITIONAL([ENABLE_RESUME_STATIC], [test "${enable_resume_static}" = "yes"])
AM_CONDITIONAL([ENABLE_CREATE_DEVICE], [test "${enable_create_device}" = "yes"])
AM_CONDITIONAL([ENABLE_SPLASHY], [test "${enable_splashy}" = "yes"])
AM_CONDITIONAL([ENABLE_FBSPLASH], [test "${enable_fbsplash}" = "yes"])
AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	manpages/Makefile
	scripts/Makefile
	scripts/suse-10.1/Makefile
])
AC_OUTPUT

