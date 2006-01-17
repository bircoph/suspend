all: suspend resume

suspend:	suspend.c swsusp.h
	gcc -Wall suspend.c -o suspend

resume:	resume.c swsusp.h
	gcc -Wall resume.c -o resume