CC_FLAGS=-I/usr/local/include
# add "-DCONFIG_COMPRESS" for compression
LD_FLAGS=-L/usr/local/lib
# add "-llzf" for compression

all: suspend resume s2ram

S2RAMOBJ=vbetool/lrmi.o vbetool/x86-common.o

ifeq ($(ARCH), x86_64)
S2RAMOBJ=vbetool/thunk.o vbetool/x86-common.o vbetool/x86emu/libx86emu.a
endif

clean:
	rm -f md5.o config.o suspend resume s2ram vbetool/*.o vbetool/x86emu/*.o vbetool/x86emu/*.a

s2ram:	s2ram.c dmidecode.c $(S2RAMOBJ)
	gcc -Wall s2ram.c $(S2RAMOBJ) -lpci -o s2ram

vbetool/lrmi.o:	vbetool/lrmi.c
	gcc -Wall -O2 -c vbetool/lrmi.c -o vbetool/lrmi.o

vbetool/x86-common.o:	vbetool/x86-common.c
	gcc -Wall -O2 -c vbetool/x86-common.c -o vbetool/x86-common.o

vbetool/x86emu/libx86emu.a:
	make -C vbetool/x86emu -f makefile.linux

vbetool/thunk.o:	vbetool/thunk.c
	gcc -Wall -O2 -c vbetool/thunk.c -o vbetool/thunk.o

md5.o:	md5.c md5.h
	gcc -Wall -o md5.o -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c

config.o:	config.c config.h
	gcc -Wall -c config.c

suspend:	md5.o config.o suspend.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o suspend.c -o suspend $(LD_FLAGS)

resume:	md5.o config.o resume.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o resume.c -static -o resume $(LD_FLAGS)

