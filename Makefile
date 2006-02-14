all: suspend resume s2ram

clean:
	rm config.o suspend resume s2ram

s2ram:	s2ram.c dmidecode.c
	gcc -Wall s2ram.c -o s2ram

config.o:	config.c config.h
	gcc -Wall -c config.c

suspend:	config.o suspend.c swsusp.h config.h
	gcc -Wall config.o suspend.c -o suspend

resume:	config.o resume.c swsusp.h config.h
	gcc -Wall config.o resume.c -static -o resume
