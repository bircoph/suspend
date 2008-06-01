/*
 * resume.c
 *
 * A simple user space resume handler for swsusp.
 *
 * Copyright (C) 2005 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <syscall.h>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef CONFIG_COMPRESS
#include <lzo/lzo1x.h>
#endif

#include "swsusp.h"
#include "memalloc.h"
#include "config_parser.h"
#include "md5.h"
#include "splash.h"
#include "loglevel.h"

static char snapshot_dev_name[MAX_STR_LEN] = SNAPSHOT_DEVICE;
static char resume_dev_name[MAX_STR_LEN] = RESUME_DEVICE;
static loff_t resume_offset;
static int suspend_loglevel = SUSPEND_LOGLEVEL;
static int max_loglevel = MAX_LOGLEVEL;
static char splash_param;
#ifdef CONFIG_FBSPLASH
char fbsplash_theme[MAX_STR_LEN] = "";
#endif
static int use_platform_suspend;

static struct config_par parameters[] = {
	{
		.name = "snapshot device",
		.fmt = "%s",
		.ptr = snapshot_dev_name,
		.len = MAX_STR_LEN
	},
	{
		.name = "resume device",
		.fmt ="%s",
		.ptr = resume_dev_name,
		.len = MAX_STR_LEN
	},
	{
		.name = "resume offset",
		.fmt = "%llu",
		.ptr = &resume_offset,
	},
	{
		.name = "suspend loglevel",
		.fmt = "%d",
		.ptr = &suspend_loglevel,
	},
	{
		.name = "max loglevel",
		.fmt = "%d",
		.ptr = &max_loglevel,
	},
	{
		.name = "image size",
		.fmt = "%lu",
		.ptr = NULL,
	},
	{
		.name = "compute checksum",
		.fmt = "%c",
		.ptr = NULL,
	},
#ifdef CONFIG_COMPRESS
	{
		.name = "compress",
		.fmt = "%c",
		.ptr = NULL,
	},
#endif
#ifdef CONFIG_ENCRYPT
	{
		.name = "encrypt",
		.fmt = "%c",
		.ptr = NULL,
	},
	{
		.name = "RSA key file",
		.fmt = "%s",
		.ptr = NULL,
	},
#endif
	{
		.name = "early writeout",
		.fmt = "%c",
		.ptr = NULL,
	},
	{
		.name = "splash",
		.fmt = "%c",
		.ptr = &splash_param,
	},
	{
		.name = "shutdown method",
		.fmt = "%s",
		.ptr = NULL,
	},
#ifdef CONFIG_FBSPLASH
	{
		.name = "fbsplash theme",
		.fmt = "%s",
		.ptr = fbsplash_theme,
		.len = MAX_STR_LEN,
	},
#endif
	{
		.name = "resume pause",
		.fmt = "%d",
		.ptr = NULL,
	},
	{
		.name = "debug verify image",
		.fmt = "%c",
		.ptr = NULL,
	},
#ifdef CONFIG_THREADS
	{
		.name = "threads",
		.fmt = "%c",
		.ptr = NULL,
	},
#endif
	{
		.name = NULL,
		.fmt = NULL,
		.ptr = NULL,
		.len = 0,
	}
};

static inline int atomic_restore(int dev)
{
	return ioctl(dev, SNAPSHOT_ATOMIC_RESTORE, 0);
}

static int open_resume_dev(char *resume_dev_name,
                           struct swsusp_header *swsusp_header)
{
	ssize_t size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;
	ssize_t ret;
	int fd;

	fd = open(resume_dev_name, O_RDWR);
	if (fd < 0) {
		ret = -errno;
		fprintf(stderr, "%s: Could not open the resume device\n",
				my_name);
		return ret;
	}
	if (lseek(fd, shift, SEEK_SET) != shift)
		return -EIO;
	ret = read(fd, swsusp_header, size);
	if (ret == size) {
		if (memcmp(SWSUSP_SIG, swsusp_header->sig, 10)) {
			close(fd);
			return -ENOMEDIUM;
		}
	} else {
		ret = ret < 0 ? ret : -EIO;
		return ret;
	}
	
	return fd;
}

static void pause_resume(int pause)
{
	struct termios newtrm, savedtrm;
	char message[SPLASH_GENERIC_MESSAGE_SIZE];
	int wait_possible = !splash.prepare_abort(&savedtrm, &newtrm);

	if (!wait_possible)
		pause = -1;

	sprintf(message, "Image successfully loaded\nPress " ENTER_KEY_NAME
			" to continue\n");
	splash.set_caption(message);
	printf("%s: %s", my_name, message);

	if (pause > 0)
		printf("%s: Continuing automatically in %2d seconds",
			my_name, pause);

	while (pause) {
		if (splash.key_pressed() == ENTER_KEY_CODE)
			break;
		sleep(1);
		if (pause > 0)
			printf("\b\b\b\b\b\b\b\b\b\b%2d seconds", --pause);
	}
	printf("\n");

	if (wait_possible)
		splash.restore_abort(&savedtrm);
}

static int read_image(int dev, int fd, struct swsusp_header *swsusp_header)
{
	ssize_t size = sizeof(struct swsusp_header);
	unsigned int shift = (resume_offset + 1) * page_size - size;
	struct image_header_info *header;
	int ret, error;
	char c;

	header = getmem(page_size);

	error = read_or_verify_image(dev, fd, header, swsusp_header->image, 0);
	if (error) {
		c = splash.dialog(
			"\nThe system snapshot image could not be read.\n\n"
#ifdef CONFIG_ENCRYPT
			"\tThis might be a result of booting a wrong kernel\n"
			"\tor typing in a wrong passphrase.\n\n"
#else
			"\tThis might be a result of booting a wrong kernel.\n\n"
#endif
			"\tYou can continue to boot the system and lose the saved state\n"
			"\tor reboot and try again.\n\n"
		        "\t[Notice that you may not mount any filesystem between\n"
		        "\tnow and successful resume. That would badly damage\n"
		        "\taffected filesystems.]\n\n"
			"\tDo you want to continue boot (Y/n)? ");
		ret = (c == 'n' || c == 'N');
		if (ret) {
			close(fd);
			reboot();
			fprintf(stderr, "%s: Reboot failed, please reboot manually.\n",
					my_name);
                       while(1)
			   sleep(1);
		}
	} else {
		if (header->flags & PLATFORM_SUSPEND)
			use_platform_suspend = 1;
	}

	/* Reset swap signature now */
	memcpy(swsusp_header->sig, swsusp_header->orig_sig, 10);
	if (lseek(fd, shift, SEEK_SET) != shift) {
		error = -EIO;
	} else {
		ret = write(fd, swsusp_header, size);
		if (ret != size) {
			error = ret < 0 ? -errno : -EIO;
			fprintf(stderr,
				"%s: Could not restore the partition header\n",
				my_name);
		}
	}

	fsync(fd);
	close(fd);

	if (error) {
		char message[SPLASH_GENERIC_MESSAGE_SIZE];

		sprintf(message, "%s: Error %d loading the image\nPress "
			ENTER_KEY_NAME " to continue\n", my_name, error);
		splash.dialog(message);
	} else if (header->resume_pause != 0) {
		pause_resume(header->resume_pause);
	} else {
		printf("%s: Image successfully loaded\n", my_name);
	}

	freemem(header);

	return error;
}

/* Parse the command line and/or configuration file */
static inline int get_config(int argc, char *argv[])
{
	static struct option options[] = {
		   {
		       "help\0\t\t\tthis text",
		       no_argument,		NULL, 'h'
		   },
		   {
		       "version\0\t\t\tversion information",
		       no_argument,		NULL, 'V'
		   },
		   {
		       "config\0\t\talternative configuration file.",
		       required_argument,	NULL, 'f'
		   },
		   {
		       "resume_device\0device that contains swap area",	
		       required_argument,	NULL, 'r'
		   },
		   {
		       "resume_offset\0offset of swap file in resume device.",	
		       required_argument,	NULL, 'o'
		   },
 		   {
 		       "parameter\0\toverride config file parameter.",
 		       required_argument,	NULL, 'P'
 		   },
		   { NULL,		0,			NULL,  0 }
	};
	int i, error;
	char *conf_name = CONFIG_FILE;
	const char *optstring = "hVf:o:r:P:";
	struct stat stat_buf;
	int fail_missing_config = 0;

	/* parse only config file argument */
	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'h':
			usage(my_name, options, optstring);
			exit(EXIT_SUCCESS);
		case 'V':
			version(my_name, NULL);
			exit(EXIT_SUCCESS);
		case 'f':
			conf_name = optarg;
			fail_missing_config = 1;
			break;
		}
	}

	if (stat(conf_name, &stat_buf)) {
		if (fail_missing_config) {
			fprintf(stderr, "%s: Could not stat configuration file\n",
				my_name);
			return -ENOENT;
		}
	}
	else {
		error = parse(my_name, conf_name, parameters);
		if (error) {
			fprintf(stderr, "%s: Could not parse config file\n", my_name);
			return error;
		}
	}

	optind = 0;
	while ((i = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (i) {
		case 'f':
			/* already handled */
			break;
		case 'o':
			resume_offset = atoll(optarg);
			break;
		case 'r':
			strncpy(resume_dev_name, optarg, MAX_STR_LEN -1);
			break;
 		case 'P':
 			error = parse_line(optarg, parameters);
 			if (error) {
 				fprintf(stderr,
					"%s: Could not parse config string '%s'\n",
						my_name, optarg);
 				return error;
 			}
 			break;
		default:
			usage(my_name, options, optstring);
			return -EINVAL;
		}
	}

	if (optind < argc)
		strncpy(resume_dev_name, argv[optind], MAX_STR_LEN - 1);

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned int mem_size;
	struct stat stat_buf;
	int dev, resume_dev;
	int n, error, orig_loglevel;
	static struct swsusp_header swsusp_header;

	my_name = basename(argv[0]);

	error = get_config(argc, argv);
	if (error)
		return -error;

	if (splash_param != 'y' && splash_param != 'Y')
		splash_param = 0;
	else
		splash_param = SPL_RESUME;

	get_page_and_buffer_sizes();

	mem_size = 2 * page_size + buffer_size;
#ifdef CONFIG_ENCRYPT
	printf("%s: libgcrypt version: %s\n", my_name,
		gcry_check_version(NULL));
	gcry_control(GCRYCTL_INIT_SECMEM, page_size, 0);
	mem_size += page_size;
#endif
#ifdef CONFIG_COMPRESS
	/*
	 * The formula below follows from the worst-case expansion calculation
	 * for LZO1 (size / 16 + 67) and the fact that the size of the
	 * compressed data must be stored in the buffer (sizeof(size_t)).
	 */
	compress_buf_size = buffer_size +
			round_up_page_size((buffer_size >> 4) + 67 +
						sizeof(size_t));
	mem_size += compress_buf_size +
			round_up_page_size(LZO1X_1_MEM_COMPRESS);
#endif

	error = init_memalloc(page_size, mem_size);
	if (error) {
		fprintf(stderr, "%s: Could not allocate memory\n", my_name);
		return error;
	}

	open_printk();
	orig_loglevel = get_kernel_console_loglevel();
	set_kernel_console_loglevel(suspend_loglevel);

	while (stat(resume_dev_name, &stat_buf)) {
		fprintf(stderr, 
			"%s: Could not stat the resume device file '%s'\n"
			"\tPlease type in the full path name to try again\n"
			"\tor press ENTER to boot the system: ", my_name,
			resume_dev_name);
		fgets(resume_dev_name, MAX_STR_LEN - 1, stdin);
		n = strlen(resume_dev_name) - 1;
		if (n <= 0) {
			error = EINVAL;
			goto Free;
		}

		if (resume_dev_name[n] == '\n')
			resume_dev_name[n] = '\0';
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (mlockall(MCL_CURRENT | MCL_FUTURE)) {
		error = errno;
		fprintf(stderr, "%s: Could not lock myself\n", my_name);
		goto Free;
	}


	dev = open(snapshot_dev_name, O_WRONLY);
	if (dev < 0) {
		error = ENOENT;
		goto Free;
	}

	resume_dev = open_resume_dev(resume_dev_name, &swsusp_header);
	if (resume_dev == -ENOMEDIUM) {
		error = 0;
		goto Close;
	} else if (resume_dev < 0) {
		error = -resume_dev;
		goto Close;
	}

	splash_prepare(&splash, splash_param);
	splash.progress(5);

	error = read_image(dev, resume_dev, &swsusp_header);
	if (error) {
		fprintf(stderr, "%s: Could not read the image\n", my_name);
		error = -error;
		goto Close_splash;
	}

	if (freeze(dev)) {
		error = errno;
		fprintf(stderr, "%s: Could not freeze processes\n", my_name);
		goto Close_splash;
	}
	if (use_platform_suspend) {
		int err = platform_prepare(dev);

		if (err) {
			fprintf(stderr, "%s: Unable to use platform "
					"hibernation support, error code %d\n",
					my_name, err);
			use_platform_suspend = 0;
		}
	}
	atomic_restore(dev);
	/* We only get here if the atomic restore fails.  Clean up. */
	if (use_platform_suspend)
		platform_finish(dev);

	unfreeze(dev);

Close_splash:
	splash.finish();
Close:
	close(dev);
Free:
	if (error)
	    set_kernel_console_loglevel(max_loglevel);
	else if (orig_loglevel >= 0)
	    set_kernel_console_loglevel(orig_loglevel);

	close_printk();

	free_memalloc();

	return error;
}
