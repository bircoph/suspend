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
	gcry_ac_data_t rsa_data_set;
	gcry_ac_handle_t rsa_hd;
	gcry_ac_key_t rsa_priv;
	gcry_ac_key_pair_t rsa_key_pair;
	gcry_mpi_t mpi;
	size_t offset;
	int len = MIN_KEY_BITS, ret = EXIT_SUCCESS;
	struct termios termios;
	char *vrfy_buf;
	struct md5_ctx ctx;
	unsigned char key_buf[PK_KEY_SIZE];
	gcry_cipher_hd_t sym_hd;
	unsigned char ivec[PK_CIPHER_BLOCK];
	unsigned short size;
	int j, fd;

	printf("libgcrypt version: %s\n", gcry_check_version(NULL));
	gcry_control(GCRYCTL_INIT_SECMEM, 4096, 0);
	ret = gcry_ac_open(&rsa_hd, GCRY_AC_RSA, 0);
	if (ret)
		return ret;

	do {
		printf("Key bits (between %d and %d inclusive) [%d]: ",
			MIN_KEY_BITS, MAX_KEY_BITS, len);
		fgets(in_buffer, MAX_STR_LEN, stdin);
		if (strlen(in_buffer) && *in_buffer != '\n')
			sscanf(in_buffer, "%d", &len);
	} while (len < MIN_KEY_BITS || len > MAX_KEY_BITS);

Retry:
	printf("Generating %d-bit RSA keys.  Please wait.\n", len);
	ret = gcry_ac_key_pair_generate(rsa_hd, len, NULL, &rsa_key_pair, NULL);
	if (ret) {
		fprintf(stderr, "Key generation failed: %s\n", gcry_strerror(ret));
		ret = EXIT_FAILURE;
		goto Close_RSA;
	}
	printf("Testing the private key.  Please wait.\n");
	rsa_priv = gcry_ac_key_pair_extract(rsa_key_pair, GCRY_AC_KEY_SECRET);
	ret = gcry_ac_key_test(rsa_hd, rsa_priv);
	if (ret) {
		printf("RSA key test failed.  Retrying.\n");
		goto Retry;
	}

	rsa_data_set = gcry_ac_key_data_get(rsa_priv);
	if (gcry_ac_data_length(rsa_data_set) != RSA_FIELDS) {
		fprintf(stderr, "Wrong number of key fields\n");
		ret = -EINVAL;
		goto Free_RSA;
	}

	/* Convert the key length into bytes */
	size = (len + 7) >> 3;
	/* Copy the public key components to struct RSA_data */
	offset = 0;
	for (j = 0; j < RSA_FIELDS_PUB; j++) {
		char *str;
		size_t s;

		if (offset + size >= RSA_DATA_SIZE)
			goto Free_RSA;

		gcry_ac_data_get_index(rsa_data_set, GCRY_AC_FLAG_COPY, j,
					(const char **)&str, &mpi);
		gcry_mpi_print(GCRYMPI_FMT_USG, rsa.data + offset,
					size, &s, mpi);
		rsa.field[j][0] = str[0];
		rsa.field[j][1] = '\0';
		rsa.size[j] = s;
		offset += s;
		gcry_mpi_release(mpi);
		gcry_free(str);
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

	memset(ivec, 0, PK_CIPHER_BLOCK);
	strncpy((char *)ivec, pass_buf, PK_CIPHER_BLOCK);
	md5_init_ctx(&ctx);
	md5_process_bytes(pass_buf, strlen(pass_buf), &ctx);
	md5_finish_ctx(&ctx, key_buf);

	/* First, we encrypt the key test */
	ret = gcry_cipher_open(&sym_hd, PK_CIPHER, GCRY_CIPHER_MODE_CFB,
				GCRY_CIPHER_SECURE);
	if (ret)
		goto Free_RSA;

	ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
	if (!ret)
		ret = gcry_cipher_setiv(sym_hd, ivec, PK_CIPHER_BLOCK);

	if (!ret)
		ret = gcry_cipher_encrypt(sym_hd, rsa.key_test, KEY_TEST_SIZE,
					KEY_TEST_DATA, KEY_TEST_SIZE);

	gcry_cipher_close(sym_hd);
	if (ret)
		goto Free_RSA;

	/* Now, we can encrypt the private RSA key */
	ret = gcry_cipher_open(&sym_hd, PK_CIPHER, GCRY_CIPHER_MODE_CFB,
				GCRY_CIPHER_SECURE);
	if (ret)
		goto Free_RSA;

	ret = gcry_cipher_setkey(sym_hd, key_buf, PK_KEY_SIZE);
	if (!ret)
		ret = gcry_cipher_setiv(sym_hd, ivec, PK_CIPHER_BLOCK);

	if (ret)
		goto Free_sym;

	/* Copy the private key components (encrypted) to struct RSA_data */
	for (j = RSA_FIELDS_PUB; j < RSA_FIELDS; j++) {
		char *str;
		size_t s;

		if (offset + size >= RSA_DATA_SIZE)
			goto Free_sym;

		gcry_ac_data_get_index(rsa_data_set, GCRY_AC_FLAG_COPY, j,
					(const char **)&str, &mpi);
		gcry_mpi_print(GCRYMPI_FMT_USG, rsa.data + offset,
					size, &s, mpi);

		/* We encrypt the data in place */
		ret = gcry_cipher_encrypt(sym_hd, rsa.data + offset, s, NULL, 0);
		if (ret)
			goto Free_sym;

		rsa.field[j][0] = str[0];
		rsa.field[j][1] = '\0';
		rsa.size[j] = s;
		offset += s;
		gcry_mpi_release(mpi);
		gcry_free(str);
	}

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
	gcry_ac_data_destroy(rsa_data_set);
Close_RSA:
	gcry_ac_close(rsa_hd);

	return ret;
}
