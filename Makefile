all: suspend resume s2ram

s2ram:	s2ram.c dmidecode.c
	gcc -Wall s2ram.c -o s2ram

suspend:	suspend.c swsusp.h
	gcc -Wall suspend.c -o suspend

resume:	resume.c swsusp.h
	gcc -Wall resume.c -o resume