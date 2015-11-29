#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "kuzn.h"

#define N_ITERATIONS 10000

int
main(int argc, const char *argv[])
{
	struct kuzn_ctx ctx;
	int i;
	clock_t start, elapsed;

	uint8_t buf[] = {
		0x11, 0x22, 0x33, 0x44,
		0x55, 0x66, 0x77, 0x00,
		0xff, 0xee, 0xdd, 0xcc,
		0xbb, 0xaa, 0x99, 0x88, 
	};

	kuzn_context_init(&ctx);

	start = clock();

	for (i = 0; i < N_ITERATIONS; i++) {
		kuzn_encrypt(&ctx, buf, buf);
	}
	for (i = 0; i < N_ITERATIONS; i++) {
		kuzn_decrypt(&ctx, buf, buf);
	}

	elapsed = clock() - start;

	printf("elapsed %d\n", elapsed);

	return 0;
}
