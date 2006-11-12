/*
 * swap-file.c
 *
 * This program determines the location of the swap offset for given swap file.
 *
 * Copyright (C) 2006 Luca Tettamanti <kronos.it@gmail.com>
 *
 * This file is released under the GPLv2.
 *
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <errno.h>

#define SWAP_SIG	"SWAPSPACE2"
#define SWAP_SIG_SIZE	10

int main(int argc, char **argv) {
	int block, last_block, first_block, blocks_per_page;
	int size, blk_size;
	int fd;
	int i;
	struct stat stat;
	unsigned char buf[SWAP_SIG_SIZE];
	int err = 0;
	int const page_size = getpagesize();

	if (argc < 2) {
		fprintf(stderr, "Usage: swap-offset <file_name>\n");
		return EINVAL;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		err = errno;
		perror("open()");
		return err;
	}

	/* Check swap signature */
	if (lseek(fd, page_size - SWAP_SIG_SIZE, SEEK_SET) < 0) {
		err = errno;
		perror("lseek()");
		goto out;
	}
	i = read(fd, buf, SWAP_SIG_SIZE);
	if (i < 0) {
		err = errno;
		perror("read()");
		goto out;
	} else if (i < SWAP_SIG_SIZE) {
		fprintf(stderr, "Failed to read swap signature: file is too short.\n");
		err = EINVAL;
		goto out;
	}

	if (memcmp(buf, SWAP_SIG, SWAP_SIG_SIZE)) {
		fprintf(stderr, "Swap signature not found.\n");
		err = EINVAL;
		goto out;
	}

	if (fstat(fd, &stat)) {
		err = errno;
		perror("fstat()");
		goto out;
	}
	if (ioctl(fd, FIGETBSZ, &blk_size)) {
		err = errno;
		perror("ioctl(FIGETBSZ) failed");
		goto out;
	}

	blocks_per_page = page_size / blk_size;

	/* Check that the header is contiguous */
	last_block = 0;
	first_block = 0;
	size = 0;
	for (i = 0; i < blocks_per_page; i++) {
		block = i;

		if (ioctl(fd, FIBMAP, &block)) {
			err = errno;
			perror("ioctl(FIBMAP) failed");
			goto out;
		}

		if (last_block && block != last_block + 1)
			break;

		if (!first_block)
			first_block = block;

		size += blk_size;
		last_block = block;
	}
	if (size < page_size) {
		fprintf(stderr, "Swapfile header is not contiguous and cannot be used"
				"for suspension.\n");
		err = EINVAL;
	} else {
		printf("resume offset = %d\n", first_block * blk_size / page_size);
	}

out:
	close(fd);

	return err;
}
