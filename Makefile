CC_FLAGS=-I/usr/local/include
# add "-DCONFIG_COMPRESS" for compression
LD_FLAGS=-L/usr/local/lib
# add "-llzf" for compression

all: suspend resume s2ram

clean:
	rm -f md5.o config.o suspend resume s2ram vbetool/*.o

s2ram:	s2ram.c dmidecode.c vbetool/lrmi.o vbetool/x86-common.o
	gcc -Wall s2ram.c vbetool/lrmi.o vbetool/x86-common.o -lpci -o s2ram

vbetool/lrmi.o:	vbetool/lrmi.c
	gcc -Wall -O2 -c vbetool/lrmi.c -o vbetool/lrmi.o

vbetool/x86-common.o:	vbetool/x86-common.c
	gcc -Wall -O2 -c vbetool/x86-common.c -o vbetool/x86-common.o

md5.o:	md5.c md5.h
	gcc -Wall -o md5.o -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c

config.o:	config.c config.h
	gcc -Wall -c config.c

suspend:	md5.o config.o suspend.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o suspend.c -o suspend $(LD_FLAGS)

resume:	md5.o config.o resume.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o resume.c -static -o resume $(LD_FLAGS)

