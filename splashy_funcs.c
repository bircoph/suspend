/*
 * splashy.c
 *
 * Splashy (userspace splash) splash method support
 *
 * Copyright (C) 2006 Tim Dijkstra <tim@famdijkstra.org>
 *
 * This file is released under the GPLv2.
 *
 */

#ifdef CONFIG_SPLASHY
#include <string.h>
#include <stdio.h>
#include <splashy.h>

#include "splash.h"
#include "encrypt.h"
#include "splashy_funcs.h"

int splashy_open(int mode)
{
	int ret;
	if ((ret = splashy_init (NULL,(mode==SPL_RESUME?"resume":"suspend"))) < 0)
	{
		fprintf(stderr,"splashy_init: error %d",ret);
		return -1;
	}
	
	if ((ret = splashy_start_splash ()) < 0) {
		fprintf(stderr,"splashy_start_splash: error %d",ret);
		return -2;
	}
	
	return 0;
}


inline int splashy_finish(void)
{

	splashy_stop_splash();

	return 0;
}

inline int splashy_progress(int p)
{

	splashy_update_progressbar (p);
    
	return 0;
}

void splashy_read_password(char *buf, int vrfy)
{
#if CONFIG_ENCRYPT
        char *vrfy_buf = vrfy ? buf + PASS_SIZE : buf;

        do {
            while (splashy_get_password(buf,PASS_SIZE, 
                        "Passphrase please (must be non-empty): ") <= 0);
            
            if (vrfy)
                while (splashy_get_password(vrfy_buf,PASS_SIZE,
                            "Verify passphrase: ") <= 0);
            
    	} while (vrfy && strncmp(buf, vrfy_buf, PASS_SIZE));
#endif
}

int splashy_dialog(const char *prompt) 
{
	splashy_printline(prompt);
	return splashy_getchar();
}

#endif
