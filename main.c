#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <assert.h>

#include "kuzn.h"

#define MULTIPLIER 1000
#define DATA_SZ	1 * 1024 * 1024

int
kuzn_encrypt_ecb(struct kuzn_ctx *ctx, uint8_t *in, uint8_t *out, size_t length)
{
	int i;

	assert(ctx != NULL && in != NULL && out != NULL);

	for (i = 0; i < length / KUZN_BLOCK_LEN; ++i) {
		kuzn_encrypt(ctx, in, out);

		in += KUZN_BLOCK_LEN;
		out += KUZN_BLOCK_LEN;
	}

	return 0;
}

unsigned long
gost_kuzn_timetest(int *cnt)
{
	struct kuzn_ctx ctx;
	uint8_t *buf;	
	int i;
	time_t start, elapsed;

	*cnt = 0;
	buf = malloc(DATA_SZ);

	kuzn_context_init(&ctx);

	start = clock();
	while (clock() < start + CLOCKS_PER_SEC) {
		kuzn_encrypt_ecb(&ctx, buf, buf, DATA_SZ);
		(*cnt)++;
	}

	start = clock();
	for (i = 0; i < *cnt; i++) {
		kuzn_encrypt_ecb(&ctx, buf, buf, DATA_SZ);
	}

	elapsed = clock() - start;

	free(buf);

	//printf("before test %X cnt %X\n", elapsed, *cnt);
	return (*cnt * CLOCKS_PER_SEC * MULTIPLIER) / elapsed;
}

int
main(int argc, const char *argv[])
{
	unsigned long kuzntime;
	int ncycles;

	kuzntime = gost_kuzn_timetest(&ncycles);

	printf("Gost kuzn %ld.%03ld mb/s. Cycles= %d\n", kuzntime / MULTIPLIER, kuzntime % MULTIPLIER, ncycles);

	return 0;
}
