/*
 * Suspend-to-RAM - PPC code
 *
 * Copyright 2006 Tim Dijkstra <newsuser@fa...>
 * Copyright 2006 Luca Tettamanti <kronos.it@gm...>
 * Distribute under GPLv2.
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/pmu.h>

#include "s2ram.h"

void s2ram_add_flag(int opt, const char *arg)
{
	/* We don't take any options */
}

int s2ram_prepare(void)
{
	return 0;
}

void s2ram_resume(void)
{
 /* nop */
}

void identify_machine(void)
{
	printf("We don't have quirks and hence no whitelist on powerpc\n");
}

int machine_known(void)
{
	printf("We don't have quirks and hence no whitelist on powerpc\n");
	return 0;
}

int s2ram_hacks(void)
{
	return 0;
}

int s2ram_is_supported(void)
{
	int fd, ret = 0;
	unsigned long arg = 0;

	/* PMU_IOC_CAN_SLEEP is going away, so we only say unsupported
	* if PMU_IOC_CAN_SLEEP explicitly says we can't */
	fd = open("/dev/pmu", O_RDWR);
	if (fd < 0)
		return 0;

	ret = ioctl(fd, PMU_IOC_CAN_SLEEP, &arg);
	if (!ret && arg != 1)
		ret = ENOTSUP;

	close(fd);

	return 0;
}

static int s2ram_do_pmu (void)
{
	int fd;
	int ret = 0;
	unsigned long arg = 0;

	fd = open("/dev/pmu", O_RDWR);
	if (fd < 0)
		return errno;

	ret = ioctl(fd, PMU_IOC_CAN_SLEEP, &arg);
	if (!ret && arg != 1)
		ret = ENOTSUP;

	if (!ret)
		ret = ioctl(fd, PMU_IOC_SLEEP, 0);

	if (ret)
		ret = errno;

	close(fd);

	return ret;
}

int s2ram_do(void)
{
 int ret;

 /* If this works we're done. Else we just continue as if nothing
 * happened, future kernels will work with /s/p/s.
 */
 ret = s2ram_do_pmu();
 if (!ret)
	return ret;

 return s2ram_generic_do();
}
