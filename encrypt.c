/*
 * encrypt.c
 *
 * Encryption initialization for the suspend and resume tools
 *
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#ifdef CONFIG_ENCRYPT
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "md5.h"
#include "encrypt.h"

char *passphrase;

void read_password(char *pass_buf, int vrfy)
{
	struct termios termios;
	char *vrfy_buf = vrfy ? pass_buf + PASS_SIZE : pass_buf;
	int len;

	tcgetattr(0, &termios);
	termios.c_lflag &= ~ECHO;
	termios.c_lflag |= ICANON | ECHONL;
	tcsetattr(0, TCSANOW, &termios);

	do {
		do {
			printf("Passphrase please (must be non-empty): ");
			fgets(pass_buf, PASS_SIZE, stdin);
			len = strlen(pass_buf) - 1;
		} while (len <= 0);
		if (pass_buf[len] == '\n')
			pass_buf[len] = '\0';
		if (vrfy) {
			printf("Verify passphrase: ");
			fgets(vrfy_buf, PASS_SIZE, stdin);
			if (vrfy_buf[len] == '\n')
				vrfy_buf[len] = '\0';
		}
	} while (vrfy && strncmp(pass_buf, vrfy_buf, PASS_SIZE));
	termios.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &termios);
}

/**
 *	encrypt_init - set up the encryption key, initialization vector and mumber
 *	@pass_buf - auxiliary buffer that must be at least 2*PASS_SIZE bytes long
 *	if @vrfy is non-zero or at least PASS_SIZE long otherwise
 *	@key_buf - auxiliary buffer that must be at least max(KEY_SIZE,16) bytes
 *	long
 */

void encrypt_init(BF_KEY *key, unsigned char *ivec, int *num,
		char *pass_buf, void *key_buf, int vrfy)
{

	struct md5_ctx ctx;

	if (passphrase)
		strncpy(pass_buf, passphrase, PASS_SIZE);
	else
		read_password(pass_buf, vrfy);

	memset(ivec, 0, IVEC_SIZE);
	strncpy((char *)ivec, pass_buf, IVEC_SIZE);
	md5_init_ctx(&ctx);
	md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
	md5_finish_ctx(&ctx, key_buf);
	BF_set_key(key, KEY_SIZE, key_buf);
	*num = 0;
}

void get_random_salt(char *salt, size_t size)
{
	int fd;

	memset(salt, 0, size);
	fd = open("/dev/random", O_RDONLY);
	if (fd >= 0) {
		read(fd, salt, size);
		close(fd);
	}
}
#endif
