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

#define PASS_SIZE	128
#define KEY_SIZE	16
#define IVEC_SIZE	8

void encrypt_init(BF_KEY *key, unsigned char *ivec, int *num,
		char *pass_buf, void *key_buf, int vrfy);
#endif
