CC_FLAGS=-I/usr/local/inlcude -DCONFIG_COMPRESS
LD_FLAGS=-L/usr/local/lib -llzf

all: suspend resume s2ram

clean:
	rm md5.o config.o suspend resume s2ram

s2ram:	s2ram.c dmidecode.c
	gcc -Wall s2ram.c -o s2ram

md5.o:	md5.c md5.h
	gcc -Wall -o md5.o -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c

config.o:	config.c config.h
	gcc -Wall -c config.c

suspend:	md5.o config.o suspend.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o suspend.c -o suspend $(LD_FLAGS)

resume:	md5.o config.o resume.c swsusp.h config.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o config.o resume.c -static -o resume $(LD_FLAGS)

