/* whitelist.h
 * whitelist of machines that are known to work somehow
 * and all the workarounds
 */

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
