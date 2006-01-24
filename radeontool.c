 
/* Radeontool   v1.4
 * by Frederick Dean <software@fdd.com>
 * Copyright 2002-2004 Frederick Dean
 * Use hereby granted under the zlib license.
 *
 * Warning: I do not have the Radeon documents, so this was engineered from 
 * the radeon_reg.h header file.  
 *
 * USE RADEONTOOL AT YOUR OWN RISK
 *
 * Thanks to Deepak Chawla, Erno Kuusela, Rolf Offermanns, and Soos Peter
 * for patches.
 *
 * Stripped down to bare bones by Pavel Machek <pavel@suse.cz> -- for use in s2ram.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <asm/page.h>

#define RADEON_LVDS_GEN_CNTL                0x02d0
#       define RADEON_LVDS_ON               (1   <<  0)
#       define RADEON_LVDS_DISPLAY_DIS      (1   <<  1)
#       define RADEON_LVDS_PANEL_TYPE       (1   <<  2)
#       define RADEON_LVDS_PANEL_FORMAT     (1   <<  3)
#       define RADEON_LVDS_EN               (1   <<  7)
#       define RADEON_LVDS_DIGON            (1   << 18)
#       define RADEON_LVDS_BLON             (1   << 19)
#       define RADEON_LVDS_SEL_CRTC2        (1   << 23)


int debug;
int skip;

/* *radeon_cntl_mem is mapped to the actual device's memory mapped control area. */
/* Not the address but what it points to is volatile. */
unsigned char * volatile radeon_cntl_mem;

static void fatal(char *why)
{
    fprintf(stderr,why);
    exit (-1);
}

static unsigned long radeon_get(unsigned long offset,char *name)
{
    unsigned long value;
    if(debug) 
        printf("reading %s (%lx) is ",name,offset);
    if(radeon_cntl_mem == NULL) {
        printf("internal error\n");
	exit(-2);
    };
    value = *(unsigned long * volatile)(radeon_cntl_mem+offset);  
    if(debug) 
        printf("%08lx\n",value);
    return value;
}

static void radeon_set(unsigned long offset,char *name,unsigned long value)
{
    if(debug) 
        printf("writing %s (%lx) -> %08lx\n",name,offset,value);
    if(radeon_cntl_mem == NULL) {
        printf("internal error\n");
	exit(-2);
    };
    *(unsigned long * volatile)(radeon_cntl_mem+offset) = value;  
}

static void usage(void)
{
    printf("usage: radeontool [options] [command]\n");
    printf("         light [on|off]     - power down the backlight \n");
    exit(-1);
}


/* Ohh, life would be good if we could simply address all memory addresses */
/* with /dev/mem, then I could write this whole program in perl, */
/* but sadly this is only the size of physical RAM.  If you */
/* want to be truely bad and poke into device memory you have to mmap() */
static unsigned char * map_devince_memory(unsigned int base,unsigned int length) 
{
    int mem_fd;
    unsigned char *device_mem;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR) ) < 0) {
        fatal("can't open /dev/mem\nAre you root?\n");
    }

    /* mmap graphics memory */
    if ((device_mem = malloc(length + (PAGE_SIZE-1))) == NULL) {
        fatal("allocation error \n");
    }
    if ((unsigned long)device_mem % PAGE_SIZE)
        device_mem += PAGE_SIZE - ((unsigned long)device_mem % PAGE_SIZE);
    device_mem = (unsigned char *)mmap(
        (caddr_t)device_mem, 
        length,
        PROT_READ|PROT_WRITE,
        MAP_SHARED|MAP_FIXED,
        mem_fd, 
        base
    );
    if ((long)device_mem < 0) {
        if(debug)
            fprintf(stderr,"mmap returned %d\n",(int)device_mem);
        fatal("mmap error \n");
    }
    return device_mem;
}

void radeon_cmd_light(char *param)
{
    unsigned long lvds_gen_cntl;

    lvds_gen_cntl = radeon_get(RADEON_LVDS_GEN_CNTL,"RADEON_LVDS_GEN_CNTL");
    if(param == NULL) {
        printf("The radeon backlight looks %s\n",(lvds_gen_cntl&(RADEON_LVDS_ON))?"on":"off");
        exit (-1);
    } else if(strcmp(param,"on") == 0) {
        lvds_gen_cntl |= RADEON_LVDS_ON;
    } else if(strcmp(param,"off") == 0) {
        lvds_gen_cntl &= ~ RADEON_LVDS_ON;
    } else {
        usage();	    
    };
    radeon_set(RADEON_LVDS_GEN_CNTL,"RADEON_LVDS_GEN_CNTL",lvds_gen_cntl);
}



/* Here we fork() and exec() the lspci command to look for the Radeon hardware address. */
static void map_radeon_cntl_mem(void)
{
    int pipefd[2];
    int forkrc;
    FILE *fp;
    char line[1000];
    int base;

    if(pipe(pipefd)) {
        fatal("pipe failure\n");
    }
    forkrc = fork();
    if(forkrc == -1) {
        fatal("fork failure\n");
    } else if(forkrc == 0) { /* if child */
        close(pipefd[0]);
        dup2(pipefd[1],1);  /* stdout */
        setenv("PATH","/sbin:/usr/sbin:/bin:/usr/bin",1);
        execlp("lspci","lspci","-v",NULL);
        fatal("exec lspci failure\n");
    }
    close(pipefd[1]);
    fp = fdopen(pipefd[0],"r");
    if(fp == NULL) {
        fatal("fdopen error\n");
    }
#if 0
  This is an example output of "lspci -v" ...

00:1f.6 Modem: Intel Corp. 82801CA/CAM AC 97 Modem (rev 01) (prog-if 00 [Generic])
	Subsystem: PCTel Inc: Unknown device 4c21
	Flags: bus master, medium devsel, latency 0, IRQ 11
	I/O ports at d400 [size=256]
	I/O ports at dc00 [size=128]

01:00.0 VGA compatible controller: ATI Technologies Inc Radeon Mobility M6 LY (prog-if 00 [VGA])
	Subsystem: Dell Computer Corporation: Unknown device 00e3
	Flags: bus master, VGA palette snoop, stepping, 66Mhz, medium devsel, latency 32, IRQ 11
	Memory at e0000000 (32-bit, prefetchable) [size=128M]
	I/O ports at c000 [size=256]
	Memory at fcff0000 (32-bit, non-prefetchable) [size=64K]
	Expansion ROM at <unassigned> [disabled] [size=128K]
	Capabilities: <available only to root>

02:00.0 Ethernet controller: 3Com Corporation 3c905C-TX/TX-M [Tornado] (rev 78)
	Subsystem: Dell Computer Corporation: Unknown device 00e3
	Flags: bus master, medium devsel, latency 32, IRQ 11
	I/O ports at ec80 [size=128]
	Memory at f8fffc00 (32-bit, non-prefetchable) [size=128]
	Expansion ROM at f9000000 [disabled] [size=128K]
	Capabilities: <available only to root>

We need to look through it to find the smaller region base address f8fffc00.

#endif
    while(1) { /* for every line up to the "Radeon" string */
       if(fgets(line,sizeof(line),fp) == NULL) {  /* if end of file */
          fatal("Radeon hardware not found in lspci output.\n");
       }
       if(strstr(line,"Radeon") || strstr(line,"ATI Tech")) {  /* if line contains a "radeon" string */
          if(skip-- < 1) {
             break;
          }
       }
    };
    if(debug) 
       printf("%s",line);
    while(1) { /* for every line up till memory statement */
       if(fgets(line,sizeof(line),fp) == NULL || line[0] != '\t') {  /* if end of file */
          fatal("Radeon control memory not found.\n");
       }
       if(debug) 
          printf("%s",line);
       if(strstr(line,"emory") && strstr(line,"K")) {  /* if line contains a "Memory" and "K" string */
          break;
       }
    };
    if(sscanf(line,"%*s%*s%x",&base) == 0) { /* third token as hex number */
       fatal("parse error of lspci output (control memory not found)\n");
    }
    if(debug)
        printf("Radeon found. Base control address is %x.\n",base);
    radeon_cntl_mem = map_devince_memory(base,0x2000);
}

#ifndef S2RAM
int main(int argc,char *argv[]) 
{
    if(strcmp(argv[1],"--debug") == 0) {
        debug=1;
        argv++; argc--;
    };
    map_radeon_cntl_mem();
    if (argc == 3) {
        if(strcmp(argv[1],"light") == 0) {
            radeon_cmd_light(argv[2]);
            return 0;
        }
    };

    usage();
    return 1;
}
#endif
