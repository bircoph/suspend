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

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "md5.h"
#include "encrypt.h"

#define MIN_KEY_BITS	1024
#define MAX_KEY_BITS	4096
#define MAX_OUT_SIZE	(sizeof(struct RSA_data))
#define MAX_STR_LEN	256
#define DEFAULT_FILE	"suspend.key"

static char in_buffer[MAX_STR_LEN];
static char pass_buf[MAX_STR_LEN];
static struct RSA_data rsa;
static unsigned char encrypt_buffer[RSA_DATA_SIZE];

int main(int argc, char *argv[])
{
	gcry_sexp_t rsa_pair, rsa_spec, rsa_priv;
	int len = MIN_KEY_BITS, ret = EXIT_SUCCESS;
	struct termios termios;
	char *vrfy_buf;
	struct md5_ctx ctx;
	unsigned char key_buf[PK_KEY_SIZE];
	gcry_cipher_hd_t sym_hd;
	unsigned char ivec[PK_CIPHER_BLOCK];
	unsigned short size;
	int j, fd;
	const char *rsa_keys="nedpqu";

	printf("libgcrypt version: %s\n", gcry_check_version(NULL));
	gcry_control(GCRYCTL_INIT_SECMEM, 4096, 0);

	do {
		printf("Key bits (between %d and %d inclusive, must be multiple of 8) [%d]: ",
			MIN_KEY_BITS, MAX_KEY_BITS, len);
		fgets(in_buffer, MAX_STR_LEN, stdin);
		if (strlen(in_buffer) && *in_buffer != '\n')
			sscanf(in_buffer, "%d", &len);
	} while (len < MIN_KEY_BITS || len > MAX_KEY_BITS || len % 8);

	ret = gcry_sexp_build(&rsa_spec, NULL, "(genkey (rsa (nbits %u)))", len);
	if (ret) {
		fprintf(stderr, "Key parameters generation failed: %s\n", gcry_strerror(ret));
		return ret;
	}

Retry:
	printf("Generating %d-bit RSA keys.  Please wait.\n", len);
	ret = gcry_pk_genkey(&rsa_pair, rsa_spec);
	if (ret) {
		fprintf(stderr, "Key generation failed: %s\n", gcry_strerror(ret));
		goto Free_RSA_spec;
	}
	rsa_priv = gcry_sexp_find_token(rsa_pair, "private-key", 0);
	if (!rsa_priv) {
		fputs("Key generation failed: private key not found\n", stderr);
		ret = EXIT_FAILURE;
		goto Free_RSA_pair;
	}
	printf("Testing the private key.  Please wait.\n");
	ret = gcry_pk_testkey(rsa_priv);
	if (ret) {
		fprintf(stderr, "RSA key test failed: %s. Retrying.\n", gcry_strerror(ret));
		gcry_sexp_release(rsa_priv);
		gcry_sexp_release(rsa_pair);
		goto Retry;
	}

	// read password
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

	memset(ivec, 0, PK_CIPHER_BLOCK);
	strncpy((char *)ivec, pass_buf, PK_CIPHER_BLOCK);
	md5_init_ctx(&ctx);
	md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
	md5_finish_ctx(&ctx, key_buf);

	/* First, we encrypt the key test */
	ret = gcry_cipher_open(&sym_hd, PK_CIPHER, GCRY_CIPHER_MODE_CFB,
				GCRY_CIPHER_SECURE);
	if (ret) {
		fprintf(stderr, "Failed to open cipher: %s\n", gcry_strerror(ret));
		goto Free_RSA;
	}

	ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
	if (!ret)
		ret = gcry_cipher_setiv(sym_hd, ivec, PK_CIPHER_BLOCK);

	if (!ret)
		ret = gcry_cipher_encrypt(sym_hd, rsa.key_test, KEY_TEST_SIZE,
					KEY_TEST_DATA, KEY_TEST_SIZE);

	gcry_cipher_close(sym_hd);
	if (ret) {
		fprintf(stderr, "Failed to init cipher: %s\n", gcry_strerror(ret));
		goto Free_RSA;
	}

	/* Now, we can encrypt the private RSA key */
	ret = gcry_cipher_open(&sym_hd, PK_CIPHER, GCRY_CIPHER_MODE_CFB,
				GCRY_CIPHER_SECURE);
	if (ret) {
		fprintf(stderr, "Failed to open cipher: %s\n", gcry_strerror(ret));
		goto Free_RSA;
	}

	ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
	if (!ret)
		ret = gcry_cipher_setiv(sym_hd, ivec, PK_CIPHER_BLOCK);

	if (ret) {
		fprintf(stderr, "Failed to init cipher: %s\n", gcry_strerror(ret));
		goto Free_sym;
	}

	/* Copy the key components to struct RSA_data */
	size_t s, offset = 0;
	gcry_sexp_t token;
	const unsigned char *rsa_buf;
	for (j = 0; j < RSA_FIELDS && !ret; j++) {

		rsa.field[j][0] = rsa_keys[j];
		rsa.field[j][1] = '\0';

		// now get S-expression for each parameter...
		token = gcry_sexp_find_token(rsa_priv, rsa.field[j], 0);
		if (!token) {
			fprintf(stderr, "Can't find RSA parameter %s\n", rsa.field[j]);
			ret = -EINVAL;
			break;
		}
		// copy RSA parameter from S-expr to cipher buffer
		rsa_buf = gcry_sexp_nth_data(token, 1, &s);
		if (!rsa_buf) {
			fprintf(stderr, "Can't parse RSA parameter %s, idx %i\n", rsa.field[j], j);
			ret = -EINVAL;
			goto Release_token;
		}
		memcpy(rsa.data + offset, rsa_buf, s);

		/* Encrypt private RSA components in place */
		if (j >= RSA_FIELDS_PUB) {
			ret = gcry_cipher_encrypt(sym_hd, rsa.data + offset, s, NULL, 0);
			if (ret) {
				fprintf(stderr, "Failed to encrypt RSA key: %s\n", gcry_strerror(ret));
				goto Release_token;
			}
		}

		rsa.size[j] = s;
		offset += s;

	Release_token:
		gcry_sexp_release(token);
	}
	if (ret)
		goto Free_sym;

	size = offset + sizeof(struct RSA_data) - RSA_DATA_SIZE;

	printf("File name [%s]: ", DEFAULT_FILE);
	fgets(in_buffer, MAX_STR_LEN, stdin);
	if (!strlen(in_buffer) || *in_buffer == '\n')
		strcpy(in_buffer, DEFAULT_FILE);
	else if (in_buffer[strlen(in_buffer)-1] == '\n')
		in_buffer[strlen(in_buffer)-1] = '\0';
	fd = open(in_buffer, O_RDWR | O_CREAT | O_TRUNC, 00600);
	if (fd >= 0) {
		write(fd, &rsa, size);
		close(fd);
	} else {
		fprintf(stderr, "Could not open the file %s\n", in_buffer);
		ret = EXIT_FAILURE;
	}
Free_sym:
	gcry_cipher_close(sym_hd);
Free_RSA:
	gcry_sexp_release(rsa_priv);
Free_RSA_pair:
	gcry_sexp_release(rsa_pair);
Free_RSA_spec:
	gcry_sexp_release(rsa_spec);

	return ret;
}
