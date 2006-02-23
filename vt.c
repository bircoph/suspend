/* vt.c - VT specific routines
 * most of these are simplified versions of routines that can
 * be found in the kbd package.
 *
 * Released under GPL V2.
 * (C) 2006 Stefan Seyfried <seife@suse.de>
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <linux/vt.h>

int is_a_console(int fd)
{
	char arg;

	arg = 0;
	return (ioctl(fd, KDGKBTYPE, &arg) == 0
		&& ((arg == KB_101) || (arg == KB_84)));
}

int open_a_console(const char *fnam)
{
	int fd;

	fd = open(fnam, O_RDWR);
	if (fd < 0)
		return -1;
	if (!is_a_console(fd)) {
		close(fd);
		return -1;
	}
	return fd;
}

int getconsolefd(void)
{
	int fd;

	fd = open_a_console("/dev/tty");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/tty0");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/vc/0");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/console");
	if (fd >= 0)
		return fd;

	for (fd = 0; fd < 3; fd++)
		if (is_a_console(fd))
			return fd;

	return -1;
}

int fgconsole(void)
{
	int fd;
	struct vt_stat vtstat;

	fd = getconsolefd();
	if (fd < 0) {
		perror("fgconsole: getconsolefd");
		return -1;
	}

	if (ioctl(fd, VT_GETSTATE, &vtstat)) {
		perror("fgconsole: VT_GETSTATE");
		return -1;
	}
	return vtstat.v_active;
}

void chvt(int num)
{
	int fd = getconsolefd();
	if (ioctl(fd, VT_ACTIVATE, num)) {
		perror("chvt: VT_ACTIVATE");
	}
	if (ioctl(fd, VT_WAITACTIVE, num)) {
		perror("VT_WAITACTIVE");
	}
}
