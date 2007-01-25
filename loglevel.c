/* loglevel.c - routines to modify kernel console loglevel
 *
 * Released under GPL v2.
 * (c) 2007 Tim Dijkstra
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>


static FILE *printk_file;
static int proc_mounted = 0;

inline void open_printk(void)
{
	struct stat stat_buf;
	char *procname = "/proc/sys/kernel/printk";

	if (stat(procname, &stat_buf) && errno == ENOENT) {
		if (mount("none", "/proc", "proc", 0, NULL)) {
			fprintf(stderr, "resume: Could not mount proc\n");
			return;
		} else
			proc_mounted = 1;
	}

        printk_file = fopen(procname, "r+");
}

inline int get_kernel_console_loglevel(void)
{
        int level = -1;

        if (printk_file) {
                rewind(printk_file);
                fscanf(printk_file, "%d", &level);
        }
        return level;
}

inline void set_kernel_console_loglevel(int level)
{
        if (printk_file) {
                rewind(printk_file);
                fprintf(printk_file, "%d\n", level);
                fflush(printk_file);
        }

}

inline void close_printk(void)
{
        if (printk_file)
                fclose(printk_file);

	if (proc_mounted)
		umount("/proc");
}

