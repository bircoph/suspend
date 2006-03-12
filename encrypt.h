/*
 * encrypt.h
 *
 * Encryption-related definitions for user space suspend and resume
 * tools.
 *
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#ifdef CONFIG_ENCRYPT
#include <openssl/blowfish.h>
#include <openssl/rsa.h>

#define PASS_SIZE	128
#define KEY_SIZE	16
#define IVEC_SIZE	8
#define RSA_DATA_SIZE	1200
#define KEY_DATA_SIZE	800
#define KEY_TEST_SIZE	8
#define KEY_TEST_DATA	(unsigned char *)"12345678"

struct RSA_data {
	unsigned short	n_size;
	unsigned short	e_size;
	unsigned short	d_size;
	unsigned char	key_test[KEY_TEST_SIZE];
	unsigned char	data[RSA_DATA_SIZE];
};

struct key_data {
	unsigned char	key[KEY_SIZE];
	unsigned char	ivec[IVEC_SIZE];
	struct RSA_data	rsa_data;
};

struct encrypted_key {
	unsigned short	size;
	unsigned char	data[KEY_DATA_SIZE];
};

void read_password(char *pass_buf, int vrfy);
void encrypt_init(BF_KEY *key, unsigned char *ivec, int *num,
		char *pass_buf, void *key_buf, int vrfy);
void get_random_salt(char *salt, size_t size);

#define KEY_FILE	"/etc/suspend.key"
#endif
