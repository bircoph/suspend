/*
 * keygen.c
 *
 * RSA key generator for the suspend and resume tools.
 *
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <openssl/blowfish.h>
#include <openssl/rsa.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "md5.h"
#include "encrypt.h"

#define MIN_KEY_BITS	1024
#define MAX_KEY_BITS	4096
#define MAX_OUT_SIZE	1200
#define MAX_STR_LEN	256
#define IVEC_SIZE	8
#define DEFAULT_FILE	"suspend.key"

static char in_buffer[MAX_STR_LEN];
static char pass_buf[MAX_STR_LEN];
static struct RSA_data rsa_data;
static unsigned char encrypt_buffer[RSA_DATA_SIZE];

int main(int argc, char *argv[])
{
	RSA *rsa;
	int len = MIN_KEY_BITS, ret = EXIT_SUCCESS;
	unsigned char *buf;
	struct termios termios;
	char *vrfy_buf;
	unsigned short size;
	int fd;

	do {
		printf("Key bits (between %d and %d inclusive) [%d]: ",
			MIN_KEY_BITS, MAX_KEY_BITS, len);
		fgets(in_buffer, MAX_STR_LEN, stdin);
		if (strlen(in_buffer) && *in_buffer != '\n')
			sscanf(in_buffer, "%d", &len);
	} while (len < MIN_KEY_BITS || len > MAX_KEY_BITS);

	printf("Generating %d-bit RSA keys.\n", len);
	rsa = RSA_generate_key(len, RSA_F4, NULL, NULL);
	if (!rsa) {
		fprintf(stderr, "Key generation failed.\n");
		ret = EXIT_FAILURE;
		goto Free_RSA;
	}

	tcgetattr(0, &termios);
	termios.c_lflag &= ~ECHO;
	termios.c_lflag |= ICANON | ECHONL;
	tcsetattr(0, TCSANOW, &termios);
	vrfy_buf = (char *)encrypt_buffer;
	do {
		do {
			printf("Passphrase please (must be non-empty): ");
			fgets(pass_buf, MAX_STR_LEN, stdin);
			len = strlen(pass_buf) - 1;
		} while (len <= 0);
		if (pass_buf[len] == '\n')
			pass_buf[len] = '\0';
		printf("Confirm passphrase: ");
		fgets(vrfy_buf, MAX_STR_LEN, stdin);
		if (vrfy_buf[len] == '\n')
			vrfy_buf[len] = '\0';
	} while (strncmp(pass_buf, vrfy_buf, MAX_STR_LEN));
	termios.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &termios);

	buf = rsa_data.data;
	size = BN_bn2mpi(rsa->n, NULL);
	if (size <= RSA_DATA_SIZE) {
		rsa_data.n_size = BN_bn2mpi(rsa->n, buf);
		buf += rsa_data.n_size;
	}
	size += BN_bn2mpi(rsa->e, NULL);
	if (size <= RSA_DATA_SIZE) {
		rsa_data.e_size = BN_bn2mpi(rsa->e, buf);
		buf += rsa_data.e_size;
	}
	size += BN_bn2mpi(rsa->d, NULL);
	if (size <= RSA_DATA_SIZE) {
		struct md5_ctx ctx;
		unsigned char key_buf[KEY_SIZE];
		BF_KEY key;
		unsigned char ivec[IVEC_SIZE];
		int n = 0;

		memset(ivec, 0, IVEC_SIZE);
		strncpy((char *)ivec, pass_buf, IVEC_SIZE);
		md5_init_ctx(&ctx);
		md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
		md5_finish_ctx(&ctx, key_buf);
		BF_set_key(&key, KEY_SIZE, key_buf);
		BF_ecb_encrypt(KEY_TEST_DATA, rsa_data.key_test, &key, BF_ENCRYPT);
		rsa_data.d_size = BN_bn2mpi(rsa->d, encrypt_buffer);
		BF_cfb64_encrypt(encrypt_buffer, buf, rsa_data.d_size,
				&key, ivec, &n, BF_ENCRYPT);
	}
	if (size > RSA_DATA_SIZE) {
		fprintf(stderr, "Buffer is too small.  Giving up.\n");
		ret = EXIT_FAILURE;
		goto Free_RSA;
	}
	size += 3 * sizeof(short) + KEY_TEST_SIZE;

	printf("File name [%s]: ", DEFAULT_FILE);
	fgets(in_buffer, MAX_STR_LEN, stdin);
	if (!strlen(in_buffer) || *in_buffer == '\n')
		strcpy(in_buffer, DEFAULT_FILE);
	else if (in_buffer[strlen(in_buffer)-1] == '\n')
		in_buffer[strlen(in_buffer)-1] = '\0';
	fd = open(in_buffer, O_RDWR | O_CREAT | O_TRUNC, 00600);
	if (fd >= 0) {
		write(fd, &rsa_data, size);
		close(fd);
	} else {
		fprintf(stderr, "Could not open the file %s\n", in_buffer);
		ret = EXIT_FAILURE;
	}

Free_RSA:
	RSA_free(rsa);

	return ret;
}
