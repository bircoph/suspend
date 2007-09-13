 
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
 * Rework of the map_radeon_cntl_mem function and various cleanups 
 *                                  by Stefan Seyfried <seife@suse.de>
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pci/pci.h>

#define RADEON_LVDS_GEN_CNTL                0x02d0
#       define RADEON_LVDS_ON               (1   <<  0)
#       define RADEON_LVDS_DISPLAY_DIS      (1   <<  1)
#       define RADEON_LVDS_PANEL_TYPE       (1   <<  2)
#       define RADEON_LVDS_PANEL_FORMAT     (1   <<  3)
#       define RADEON_LVDS_EN               (1   <<  7)
#       define RADEON_LVDS_DIGON            (1   << 18)
#       define RADEON_LVDS_BLON             (1   << 19)
#       define RADEON_LVDS_SEL_CRTC2        (1   << 23)

/* *radeon_cntl_mem is mapped to the actual device's memory mapped control area. */
/* Not the address but what it points to is volatile. */
volatile unsigned char * radeon_cntl_mem;

static unsigned long radeon_get(unsigned long offset)
{
    unsigned long value;

    if(radeon_cntl_mem == NULL) {
        fprintf(stderr, "radeon_get: radeon_cntl_mem == NULL");
        return 0;
    }

    value = *(volatile unsigned long *)(radeon_cntl_mem+offset);
    return value;
}

static void radeon_set(unsigned long offset, unsigned long value)
{
    if(radeon_cntl_mem == NULL) {
        fprintf(stderr, "radeon_set: radeon_cntl_mem == NULL");
        return;
    }

    *(volatile unsigned long *)(radeon_cntl_mem+offset) = value;
}

/* Ohh, life would be good if we could simply address all memory addresses */
/* with /dev/mem, then I could write this whole program in perl, */
/* but sadly this is only the size of physical RAM.  If you */
/* want to be truely bad and poke into device memory you have to mmap() */
static volatile unsigned char * map_device_memory(unsigned int base,unsigned int length) 
{
    int mem_fd;
    volatile unsigned char *device_mem;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR) ) < 0) {
        perror("radeontool /dev/mem");
        return NULL;
    }

    /* mmap graphics memory */
    if ((device_mem = malloc(length + (getpagesize()-1))) == NULL) {
        perror("radeontool malloc");
        close(mem_fd);
        return NULL;
    }
    if ((unsigned long)device_mem % getpagesize())
        device_mem += getpagesize() - ((unsigned long)device_mem % getpagesize());
    device_mem = (volatile unsigned char *)mmap(
        (caddr_t)device_mem, 
        length,
        PROT_READ|PROT_WRITE,
        MAP_SHARED|MAP_FIXED,
        mem_fd, 
        base
    );
    if (device_mem == (volatile unsigned char *)-1) {
        perror("radeontool mmap");
        return NULL;
    }
    return device_mem;
}

void radeon_cmd_light(int param)
{
    unsigned long lvds_gen_cntl;

    lvds_gen_cntl = radeon_get(RADEON_LVDS_GEN_CNTL);

    if (param)
        lvds_gen_cntl |= RADEON_LVDS_ON;
    else
        lvds_gen_cntl &= ~ RADEON_LVDS_ON;

    radeon_set(RADEON_LVDS_GEN_CNTL, lvds_gen_cntl);
}

/* I have not got much feedback on my radeontool rework, hence i leave the old
 * code #if 0-ed in for reference.
 * Once some time passes without complaints about the new code, i will remove it.
 * Sorry for the clutter - seife.
 */
#if 0
/* Here we fork() and exec() the lspci command to look for the Radeon hardware address. */
void map_radeon_cntl_mem(void)
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
             break;
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
    radeon_cntl_mem = map_device_memory(base,0x2000);
}
#else

static u32
get_conf_long(unsigned char *d, unsigned int pos)
{
    return d[pos] | (d[pos+1] << 8) | (d[pos+2] << 16) | (d[pos+3] << 24);
}

/* Find out where the radeon memory is. The idea is taken from radeontool-1.5,
 * but we no longer fork lspci and parse its output :-)
 * The logic seems to be: the memory range that is not I/O space and smaller
 * than 1 MB is the radeon chip control range. Let's hope that this is true.
 */
void map_radeon_cntl_mem(void)
{
    struct pci_access *pacc;
    struct pci_dev *dev;
    unsigned int class;
    int i;
    int base = -1;
    unsigned char *config;

    radeon_cntl_mem = NULL;

    config = malloc(64);
    if (!config) {
        perror("map_radeon_cntl_mem malloc(64)");
        return;
    }
    pacc = pci_alloc();		/* Get the pci_access structure */
    pci_init(pacc);		/* Initialize the PCI library */
    pci_scan_bus(pacc);		/* We want to get the list of devices */
    for (dev=pacc->devices; dev; dev=dev->next) {           /* Iterate over all devices */
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES);/* Fill in header info we need */
        class = pci_read_word(dev, PCI_CLASS_DEVICE);       /* Read config register directly */
        if (dev->vendor_id == 0x1002 && class == 0x300) {   /* ATI && Graphics card */
            pci_read_block(dev, 0, config, 64);
            for (i=0; i<6; i++){
                u32 flag = get_conf_long(config, PCI_BASE_ADDRESS_0 + 4*i);
                if (flag & PCI_BASE_ADDRESS_SPACE_IO)   /* I/O-Ports, not memory */
                    continue;
                /* the original code parsed lspci for "emory" and "K", so it
                 * has to be at least 1K and less than 1M
                 */
                if (dev->size[i] >=1024 && dev->size[i] < 1024*1024) {
                    base = dev->base_addr[i];
                    goto found;
                }
            }
        }
    }
 found:
    pci_cleanup(pacc);
    free(config);
    if (base == -1) {
        fprintf(stderr, "radeontool: Radeon not found.\n");
        return;
    }
    radeon_cntl_mem = map_device_memory(base,0x2000);
}
#endif

#ifndef S2RAM
int main(int argc,char *argv[]) 
{
    map_radeon_cntl_mem();
    if (radeon_cntl_mem == NULL) {
        fprintf(stderr, "Fatal error: radeon_cntl_mem == NULL.\n");
        return 1;
    }
    if (argc == 3) {
        if(strcmp(argv[1],"light") == 0) {
            radeon_cmd_light(strcmp(argv[2],"off"));
            return 0;
        }
    };
    return 1;
}
#endif
