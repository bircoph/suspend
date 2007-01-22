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
#include <gcrypt.h>

/* Maximum length of a passphrase, in characters */
#define PASS_SIZE	128
/* Symmetric cipher used for image encryption, the size of its key and its
 * block, in bytes
 */
#define IMAGE_CIPHER	GCRY_CIPHER_BLOWFISH
#define KEY_SIZE	16
#define CIPHER_BLOCK	8
/* Symmetric cipher used for encrypting RSA private keys, the size of its key
 * and its block, in bytes
 */
#define PK_CIPHER	GCRY_CIPHER_AES
#define PK_KEY_SIZE	16
#define PK_CIPHER_BLOCK	16
/* Auxiliary constants */
#define RSA_DATA_SIZE	3072
#define KEY_DATA_SIZE	512
#define RSA_FIELDS	6
#define RSA_FIELDS_PUB	2
#define KEY_TEST_SIZE	8
#define KEY_TEST_DATA	(unsigned char *)"12345678"

struct RSA_data {
	char		field[RSA_FIELDS][2];
	unsigned short	size[RSA_FIELDS];
	unsigned char	key_test[KEY_TEST_SIZE];
	unsigned char	data[RSA_DATA_SIZE];
};

struct encrypted_key {
	unsigned short	size;
	unsigned char	data[KEY_DATA_SIZE];
};

struct key_data {
	unsigned char	key[KEY_SIZE];
	unsigned char	ivec[CIPHER_BLOCK];
	struct RSA_data	rsa;
	struct encrypted_key	encrypted_key;
};

void read_password(char *pass_buf, int vrfy);
void encrypt_init(unsigned char *, unsigned char *, char *);
void get_random_salt(unsigned char *salt, size_t size);

#define KEY_FILE	""
#endif
