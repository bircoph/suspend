/* whitelist.h
 * whitelist of machines that are known to work somehow
 * and all the workarounds
 */

/* flags for the whitelist */
#define S3_BIOS     0x01	/* machine needs acpi_sleep=s3_bios */
#define S3_MODE     0x02	/* machine needs acpi_sleep=s3_mode */
#define VBE_SAVE    0x04	/* machine needs "vbetool save / restore" */
#define VBE_POST    0x08	/* machine needs "vbetool post" */
#define RADEON_OFF  0x10	/* machine needs "radeontool light off" */
#define UNSURE      0x20	/* unverified entries from acpi-support 0.59 */
#define NOFB        0x40	/* must not use a frame buffer */
#define VBE_MODE    0x80	/* machine needs "vbetool vbemode get / set" */
#define PCI_SAVE   0x100	/* we need to save the VGA PCI registers */

struct machine_entry
{
	const char *sys_vendor;
	const char *sys_product;
	const char *sys_version;
	const char *bios_version;
	unsigned int flags;
};

extern char *whitelist_version;
extern struct machine_entry whitelist[];
