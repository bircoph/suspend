/*
 *	DMI decoder for s2ram
 *
 *	(C) 2000,2001 Alan Cox <alan@redhat.com>
 *      
 *      2-July-2001 Matt Domsch <Matt_Domsch@dell.com>
 *      Additional structures displayed per SMBIOS 2.3.1 spec
 *
 *	(C) 2006 Pavel Machek <pavel@suse.cz>
 *
 *	Licensed under the GNU General Public license v2.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct dmi_header
{
	u8	type;
	u8	length;
	u16	handle;
};

char bios_version[1024], sys_vendor[1024], sys_product[1024], sys_version[1024];

#define PRINTF(a...)

static char *dmi_string(struct dmi_header *dm, u8 s)
{
	u8 *bp=(u8 *)dm;
	if (!s) return "";
	
	bp+=dm->length;
	while (s>1) {
		bp+=strlen(bp);
		bp++;
		s--;
	}
	return bp;
}


static void dmi_table(int fd, u32 base, int len, int num)
{
	char *buf=malloc(len);
	struct dmi_header *dm;
	u8 *data;
	int i=0;
	
	if (lseek(fd, (long)base, 0)==-1) {
		perror("dmi: lseek");
		return;
	}
	if (read(fd, buf, len)!=len) {
		perror("dmi: read");
		return;
	}
	data = buf;
	while (i<num) {
		u32 u;
		u32 u2;
		dm=(struct dmi_header *)data;
		switch(dm->type)
		{
		case  0:
			PRINTF("\tBIOS Information Block\n");
			PRINTF("\t\tVendor: %s\n", 
			       dmi_string(dm, data[4]));
			strcpy(bios_version, dmi_string(dm, data[5]));
			PRINTF("\t\tVersion: %s\n", bios_version);
			PRINTF("\t\tRelease: %s\n",
			       dmi_string(dm, data[8]));
			PRINTF("\t\tBIOS base: 0x%04X0\n",
			       data[7]<<8|data[6]);
			PRINTF("\t\tROM size: %dK\n",
			       64*data[9]);
			PRINTF("\t\tCapabilities:\n");
			u=data[13]<<24|data[12]<<16|data[11]<<8|data[10];		
			u2=data[17]<<24|data[16]<<16|data[15]<<8|data[14];
			PRINTF("\t\t\tFlags: 0x%08X%08X\n",
			       u2,u);
			break;
			
		case 1:
			PRINTF("\tSystem Information Block\n");
			strcpy(sys_vendor, dmi_string(dm, data[4]));
			strcpy(sys_product, dmi_string(dm, data[5]));
			strcpy(sys_version, dmi_string(dm, data[6]));
			PRINTF("\t\tVendor: %s\n", sys_vendor);
			PRINTF("\t\tProduct: %s\n", sys_product);
			PRINTF("\t\tVersion: %s\n", sys_version);
			PRINTF("\t\tSerial Number: %s\n",
			       dmi_string(dm, data[7]));
			break;
			
		case 2:
			PRINTF("\tBoard Information Block\n");
			PRINTF("\t\tVendor: %s\n",
			       dmi_string(dm, data[4]));
			PRINTF("\t\tProduct: %s\n",
			       dmi_string(dm, data[5]));
			PRINTF("\t\tVersion: %s\n",
			       dmi_string(dm, data[6]));
			PRINTF("\t\tSerial Number: %s\n",
			       dmi_string(dm, data[7]));
			break;
			
		default:
			break;
		}
		data+=dm->length;
		while(*data || data[1])
			data++;
		data+=2;
		i++;
	}
	free(buf);
}

void dmi_scan(void)
{
	unsigned char buf[20];
	int fd=open("/dev/mem", O_RDONLY);
	long fp=0xE0000L;
	if (fd==-1) {
		perror("/dev/mem");
		exit(1);
	}
	if (lseek(fd,fp,0)==-1)	{
		perror("seek");
		exit(1);
	}
	
	
	fp -= 16;
	
	while( fp < 0xFFFFF)
	{
		fp+=16;
		if (read(fd, buf, 16)!=16)
			perror("read");
		if (memcmp(buf, "_DMI_", 5)==0) {
			u16 num=buf[13]<<8|buf[12];
			u16 len=buf[7]<<8|buf[6];
			u32 base=buf[11]<<24|buf[10]<<16|buf[9]<<8|buf[8];
			
			PRINTF("DMI %d.%d present.\n",
			       buf[14]>>4, buf[14]&0x0F);
			PRINTF("%d structures occupying %d bytes.\n",
			       buf[13]<<8|buf[12],
			       buf[7]<<8|buf[6]);
			PRINTF("DMI table at 0x%08X.\n",
			       buf[11]<<24|buf[10]<<16|buf[9]<<8|buf[8]);
			dmi_table(fd, base,len, num);
		}
	}
	close(fd);
	return 0;
}

#ifndef S2RAM
int main(int argc, char *argv[])
{
	dmi_scan();
}
#endif
