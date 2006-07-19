AUTOMAKE_OPTIONS = foreign

sbin_PROGRAMS = vbetool

SUBDIRS = x86emu

if WITH_X86EMU
x86 = thunk.c x86-common.c
vbetool_LDADD = x86emu/libx86emu.a /usr/lib/libpci.a
else
x86 = lrmi.c x86-common.c
x86lib = ""
vbetool_LDADD = /usr/lib/libpci.a
endif

man_MANS = vbetool.1
vbetool_SOURCES = vbetool.c $(x86)
noinst_HEADERS = include/lrmi.h vbetool.h

EXTRA_DIST = vbetool.1 LRMI

maintainer-clean-local: 
	$(RM) Makefile.in aclocal.m4 config.h.in stamp-h.in configure

AM_CFLAGS = -g -Wall -pedantic -std=gnu99
AM_LDFLAGS = 
