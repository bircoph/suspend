/* vt.h - VT specific routines
 */
#include <fcntl.h>
int is_a_console(int fd);
int open_a_console(const char *fnam);
int getconsolefd(void);
int fgconsole(void);
void chvt(int num);
int is_framebuffer(void);
char *get_fbname(void);
