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
#include <sys/mman.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct dmi_header
{
	u8	type;
	u8	length;
	u16	handle;
};

#ifdef S2RAM
extern char bios_version[1024], sys_vendor[1024], sys_product[1024], sys_version[1024];
#define PRINTF(a...)
#else
char bios_version[1024], sys_vendor[1024], sys_product[1024], sys_version[1024];
#define PRINTF printf
#endif

static char *dmi_string(struct dmi_header *dm, u8 s)
{
	char *bp=(char *)dm;
	if (!s) return "";
	
	bp+=dm->length;
	while (s>1) {
		bp+=strlen(bp);
		bp++;
		s--;
	}
	return bp;
}

/*
 * Copy a physical memory chunk into a memory buffer.
 * This function allocates memory.
 */
void *mem_chunk(size_t base, size_t len)
{
	void *p;
	int fd;
	size_t mmoffset;
	void *mmp;

	if((fd=open("/dev/mem", O_RDONLY))==-1){
		perror("/dev/mem");
		return NULL;
        }

	if((p=malloc(len))==NULL) {
		perror("/dev/mem malloc");
		return NULL;
	}

	mmoffset=base%getpagesize();
	/*
	 * Please note that we don't use mmap() for performance reasons here,
	 * but to workaround problems many people encountered when trying
	 * to read from /dev/mem using regular read() calls.
	 */
	mmp=mmap(0, mmoffset+len, PROT_READ, MAP_SHARED, fd, base-mmoffset);
	if(mmp==MAP_FAILED) {
		perror("mmap");
		free(p);
		return NULL;
	}

	memcpy(p, (u8 *)mmp+mmoffset, len);

	if(munmap(mmp, mmoffset+len)==-1)
		perror("/dev/mem munmap");

	if(close(fd)==-1)
		perror("/dev/mem");

	return p;
}

static void dmi_table(u32 base, int len, int num)
{
	char *buf;
	struct dmi_header *dm;
	char *data;
	int i=0;
	
	if ((buf=mem_chunk(base, len))==0) {
		printf("DMI Table is unreachable\n");
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
	size_t fp;
	u8 *buf;

	if((buf=mem_chunk(0xF0000, 0x10000))==NULL)
		return;

	for(fp=0; fp<=0xFFF0; fp+=16) {
		if(memcmp(buf+fp, "_DMI_", 5)==0) {
			u8 *cur = buf+fp;
			u16 num=cur[13]<<8|cur[12];
			u16 len=cur[7]<<8|cur[6];
			u32 base=cur[11]<<24|cur[10]<<16|cur[9]<<8|cur[8];

			PRINTF("DMI %d.%d present.\n",
			       cur[14]>>4, cur[14]&0x0F);
			PRINTF("%d structures occupying %d bytes.\n",
			       cur[13]<<8|cur[12],
			       cur[7]<<8|cur[6]);
			PRINTF("DMI table at 0x%08X.\n",
			       cur[11]<<24|cur[10]<<16|cur[9]<<8|cur[8]);
			dmi_table(base,len,num);
		}
	}
	free(buf);
	return;
}

#ifndef S2RAM
int main(int argc, char *argv[])
{
	dmi_scan();
}
#endif
