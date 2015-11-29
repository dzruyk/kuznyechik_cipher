#include <assert.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kuzn.h"

#define GENERATING_POLY	0x03	/* x + 1 */
#define KUZN_POLY 0xC3

unsigned char gf_exp[256];
unsigned char gf_log[256];

unsigned char pibox[] = {
	/*
	0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
	*/
	0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16, 0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
	0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA, 0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
	0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21, 0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
	0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0, 0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
	0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB, 0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
	0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12, 0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
	0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7, 0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
	0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E, 0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
	0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9, 0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,
	0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC, 0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,
	0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44, 0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,
	0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F, 0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,
	0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7, 0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,
	0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE, 0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,
	0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B, 0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,
	0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0, 0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6
};

uint8_t pibox_inv[] = {
	0xa5, 0x2d, 0x32, 0x8f, 0x0e, 0x30, 0x38, 0xc0, 0x54, 0xe6, 0x9e, 0x39, 0x55, 0x7e, 0x52, 0x91,
	0x64, 0x03, 0x57, 0x5a, 0x1c, 0x60, 0x07, 0x18, 0x21, 0x72, 0xa8, 0xd1, 0x29, 0xc6, 0xa4, 0x3f,
	0xe0, 0x27, 0x8d, 0x0c, 0x82, 0xea, 0xae, 0xb4, 0x9a, 0x63, 0x49, 0xe5, 0x42, 0xe4, 0x15, 0xb7,
	0xc8, 0x06, 0x70, 0x9d, 0x41, 0x75, 0x19, 0xc9, 0xaa, 0xfc, 0x4d, 0xbf, 0x2a, 0x73, 0x84, 0xd5,
	0xc3, 0xaf, 0x2b, 0x86, 0xa7, 0xb1, 0xb2, 0x5b, 0x46, 0xd3, 0x9f, 0xfd, 0xd4, 0x0f, 0x9c, 0x2f,
	0x9b, 0x43, 0xef, 0xd9, 0x79, 0xb6, 0x53, 0x7f, 0xc1, 0xf0, 0x23, 0xe7, 0x25, 0x5e, 0xb5, 0x1e,
	0xa2, 0xdf, 0xa6, 0xfe, 0xac, 0x22, 0xf9, 0xe2, 0x4a, 0xbc, 0x35, 0xca, 0xee, 0x78, 0x05, 0x6b,
	0x51, 0xe1, 0x59, 0xa3, 0xf2, 0x71, 0x56, 0x11, 0x6a, 0x89, 0x94, 0x65, 0x8c, 0xbb, 0x77, 0x3c,
	0x7b, 0x28, 0xab, 0xd2, 0x31, 0xde, 0xc4, 0x5f, 0xcc, 0xcf, 0x76, 0x2c, 0xb8, 0xd8, 0x2e, 0x36,
	0xdb, 0x69, 0xb3, 0x14, 0x95, 0xbe, 0x62, 0xa1, 0x3b, 0x16, 0x66, 0xe9, 0x5c, 0x6c, 0x6d, 0xad,
	0x37, 0x61, 0x4b, 0xb9, 0xe3, 0xba, 0xf1, 0xa0, 0x85, 0x83, 0xda, 0x47, 0xc5, 0xb0, 0x33, 0xfa,
	0x96, 0x6f, 0x6e, 0xc2, 0xf6, 0x50, 0xff, 0x5d, 0xa9, 0x8e, 0x17, 0x1b, 0x97, 0x7d, 0xec, 0x58,
	0xf7, 0x1f, 0xfb, 0x7c, 0x09, 0x0d, 0x7a, 0x67, 0x45, 0x87, 0xdc, 0xe8, 0x4f, 0x1d, 0x4e, 0x04,
	0xeb, 0xf8, 0xf3, 0x3e, 0x3d, 0xbd, 0x8a, 0x88, 0xdd, 0xcd, 0x0b, 0x13, 0x98, 0x02, 0x93, 0x80,
	0x90, 0xd0, 0x24, 0x34, 0xcb, 0xed, 0xf4, 0xce, 0x99, 0x10, 0x44, 0x40, 0x92, 0x3a, 0x01, 0x26,
	0x12, 0x1a, 0x48, 0x68, 0xf5, 0x81, 0x8b, 0xc7, 0xd6, 0x20, 0x0a, 0x08, 0x00, 0x4c, 0xd7, 0x74
};

#define X_func(out, in, a) do {				\
	int i;						\
							\
	for (i = 0; i < 0x10; i++)			\
		(out)[i] = (in)[i] ^ (a)[i];		\
} while(0)

#define S_func(block) do {				\
	int i;						\
							\
	for (i = 0; i < 0x10; i++)			\
		block[i] = pibox[block[i]];		\
} while(0)

#define Sinv_func(block) do {				\
	int i;						\
							\
	for (i = 0; i < 0x10; i++)			\
		block[i] = pibox_inv[block[i]];		\
} while(0)

#define C_func(out, i, ctx) do {				\
	memset((out), 0, sizeof(out));			\
	out[15] = i;					\
	L_func(out, out, ctx);				\
} while (0)

#define LSX(out, in, c, ctx) do {			\
	X_func(out, in, c);				\
	S_func(out);					\
	L_func(out, out, ctx);				\
} while (0)

#define LSXinv(out, in, c, ctx) do {			\
	X_func(out, in, c);				\
	Linv(out, out, ctx);				\
	Sinv_func(out);					\
} while (0)

#define PRINT_BLOCK(block) do {				\
	int _i;						\
	unsigned char *p = block;			\
							\
	for (_i = 0; _i < 0x10; _i++)			\
		printf("%2.2x.", p[_i]);		\
	printf("\n");					\
} while(0)


// Auxiliary functions

// Algorithm from wiki, lol.
// https://en.wikipedia.org/wiki/Finite_field_arithmetic#Multiplication
static uint8_t
galois256_mul_slow(uint8_t a, uint8_t b, uint8_t polynomial)
{
	uint8_t prod;
	
	prod = 0;
	while (b) {
		if (b & 0x1) prod ^= a;

		a = (a << 1) ^ ((a & 0x80) ? polynomial : 0);

		b >>= 1;
	}

	return prod;
}

static uint8_t
galois256_mul(uint8_t a, uint8_t b)
{
	int c;

	if (a == 0 || b == 0)
		return 0;

	c = gf_log[a] + gf_log[b];

	return gf_exp[c % 255];
}

static void
galois_init_tables(uint8_t poly)
{
	int c;
	int i;

	c = 1;
	for (i = 0; i < 256; i++) {
		gf_log[c] = i;
		gf_exp[i] = c;
		c = galois256_mul_slow(c, GENERATING_POLY, poly);
	}
}

static inline uint8_t
l_func(const uint8_t *in, const struct kuzn_ctx *ctx)
{
	uint8_t prod;

	prod  = galois256_mul(in[0x0], 148);
	prod ^= galois256_mul(in[0x1], 32);
	prod ^= galois256_mul(in[0x2], 133);
	prod ^= galois256_mul(in[0x3], 16);
	prod ^= galois256_mul(in[0x4], 194);
	prod ^= galois256_mul(in[0x5], 192);
	prod ^= in[0x6];
	prod ^= galois256_mul(in[0x7], 251);
	prod ^= in[0x8];
	prod ^= galois256_mul(in[0x9], 192);
	prod ^= galois256_mul(in[0xa], 194);
	prod ^= galois256_mul(in[0xb], 16);
	prod ^= galois256_mul(in[0xc], 133);
	prod ^= galois256_mul(in[0xd], 32);
	prod ^= galois256_mul(in[0xe], 148);
	prod ^= in[0xf];

	return prod;
}

//TODO: wipe magic constant
static void
R_func(uint8_t *out, const uint8_t *in, const struct kuzn_ctx *ctx)
{
	uint8_t tmp;
	tmp = l_func(in, ctx);

	memmove(out + 1, in, 0x0f);
	out[0] = tmp;
}

static void
Rinv_func(uint8_t *out, const uint8_t *in, const struct kuzn_ctx *ctx)
{
	uint8_t tmp;

	tmp = in[0];
	memmove(out, in + 1, 0x0f);
	out[15] = tmp;

	out[15] = l_func(out, ctx);
}

static void
L_func(uint8_t *out, const uint8_t *in, const struct kuzn_ctx *ctx)
{
	int i;

	for (i = 0; i < 0x10; i++)
		R_func(out, in, ctx);
}

static void
Linv(uint8_t *out, const uint8_t *in, const struct kuzn_ctx *ctx)
{
	int i;

	for (i = 0; i < 0x10; i++)
		Rinv_func(out, in, ctx);
}

static void
Ffunc(uint8_t *out, const uint8_t *in, const uint8_t *c, const struct kuzn_ctx *ctx)
{
	uint8_t tmp[16];

	LSX(tmp, in, c, ctx);

	X_func(tmp, tmp, in + 0x10);

	memcpy(out + 0x10, in, 0x10);
	memcpy(out, tmp, 0x10);
}

static void
kuzn_expand_key(struct kuzn_ctx *ctx)
{
	int i, j;
	uint8_t *ptr;
	uint8_t C[16];

	ptr = ctx->keys + 32;

	for (i = 0; i < 4; i++) {
		memcpy(ptr, ptr - 32, 32);
		for (j = 0; j < 8; j++) {
			C_func(C, 8*i + j + 1, ctx);
			Ffunc(ptr, ptr, C, ctx);
		}
		ptr += 32;
	}
}

// API functions

void
kuzn_context_init(struct kuzn_ctx *ctx)
{
	assert(ctx != NULL);

	memset(ctx, 0, sizeof(*ctx));

	// p(x) = x**8 + x**7 + x**6 + x + 1
	ctx->polynomial = KUZN_POLY;

	galois_init_tables(ctx->polynomial);
}

void
kuzn_encrypt(struct kuzn_ctx *ctx, const uint8_t *in, uint8_t *out)
{
	uint8_t tmp[16];

	assert(ctx != NULL && in != NULL && out != NULL);

	LSX(tmp, in, ctx->keys, ctx);			//K1
	LSX(tmp, tmp, ctx->keys + 16, ctx);		//K2
	LSX(tmp, tmp, ctx->keys + 16 * 2, ctx);		//K3
	LSX(tmp, tmp, ctx->keys + 16 * 3, ctx);		//K4
	LSX(tmp, tmp, ctx->keys + 16 * 4, ctx);		//K5
	LSX(tmp, tmp, ctx->keys + 16 * 5, ctx);		//K6
	LSX(tmp, tmp, ctx->keys + 16 * 6, ctx);		//K7
	LSX(tmp, tmp, ctx->keys + 16 * 7, ctx);		//K8
	LSX(tmp, tmp, ctx->keys + 16 * 8, ctx);		//K9
	X_func(out, tmp, ctx->keys + 16 * 9);		//K10
}

void
kuzn_decrypt(struct kuzn_ctx *ctx, const uint8_t *in, uint8_t *out)
{
	uint8_t tmp[16];

	assert(ctx != NULL && in != NULL && out != NULL);

	LSXinv(tmp, in, ctx->keys + 16 * 9, ctx);	//K10
	LSXinv(tmp, tmp, ctx->keys + 16 * 8, ctx);	//K9
	LSXinv(tmp, tmp, ctx->keys + 16 * 7, ctx);	//K8
	LSXinv(tmp, tmp, ctx->keys + 16 * 6, ctx);	//K7
	LSXinv(tmp, tmp, ctx->keys + 16 * 5, ctx);	//K6
	LSXinv(tmp, tmp, ctx->keys + 16 * 4, ctx);	//K5
	LSXinv(tmp, tmp, ctx->keys + 16 * 3, ctx);	//K4
	LSXinv(tmp, tmp, ctx->keys + 16 * 2, ctx);	//K3
	LSXinv(tmp, tmp, ctx->keys + 16, ctx);		//K2
	X_func(out, tmp, ctx->keys);			//K1
}

void
kuzn_set_key(struct kuzn_ctx *ctx, const uint8_t *key)
{
	assert(ctx != NULL && key != NULL);

	memcpy(ctx->keys, key, 32);
	kuzn_expand_key(ctx);
}

#ifdef _KUZ_DEBUG
int
test_S()
{
	unsigned char vals[5][16] = {
		{ 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
		  0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00 },
		{ 0xb6, 0x6c, 0xd8, 0x88, 0x7d, 0x38, 0xe8, 0xd7,
		  0x77, 0x65, 0xae, 0xea, 0x0c, 0x9a, 0x7e, 0xfc },
		{ 0x55, 0x9d, 0x8d, 0xd7, 0xbd, 0x06, 0xcb, 0xfe,
		  0x7e, 0x7b, 0x26, 0x25, 0x23, 0x28, 0x0d, 0x39 },
		{ 0x0c, 0x33, 0x22, 0xfe, 0xd5, 0x31, 0xe4, 0x63,
		  0x0d, 0x80, 0xef, 0x5c, 0x5a, 0x81, 0xc5, 0x0b },
		{ 0x23, 0xae, 0x65, 0x63, 0x3f, 0x84, 0x2d, 0x29,
		  0xc5, 0xdf, 0x52, 0x9c, 0x13, 0xf5, 0xac, 0xda }
	};

	unsigned char buf[16];
	int i, ret;

	memcpy(buf, vals[0], 16);

	for (i = 0; i < 4; i++) {
		S_func(buf);
		ret = memcmp(buf, vals[i + 1], 16);
		if (ret != 0) {
			printf("S_func Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i + 1]);
			return 1;
		}
	}

	for (i = 4; i > 0; i--) {
		Sinv_func(buf);
		ret = memcmp(buf, vals[i - 1], 16);
		if (ret != 0) {
			printf("Sinv_func Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i - 1]);
			return 1;
		}
	}

	return 0;
}

int
test_R(struct kuzn_ctx *ctx)
{
	uint8_t vals[5][16] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 },
		{ 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
		{ 0xa5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x64, 0xa5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x0d, 0x64, 0xa5, 0x94, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};
	unsigned char buf[16];
	int i, ret;

	memcpy(buf, vals[0], 16);

	for (i = 0; i < 4; i++) {
		R_func(buf, buf, ctx);
		ret = memcmp(buf, vals[i + 1], 16);
		if (ret != 0) {
			printf("R_func Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i + 1]);
			return 1;
		}
	}

	for (i = 4; i > 0; i--) {
		Rinv_func(buf, buf, ctx);
		ret = memcmp(buf, vals[i - 1], 16);
		if (ret != 0) {
			printf("Rinv_func Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i - 1]);
			return 1;
		}
	}

	return 0;
}

int
test_L(struct kuzn_ctx *ctx)
{
	uint8_t vals[5][16] = {
		{ 0x64, 0xa5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0xd4, 0x56, 0x58, 0x4d, 0xd0, 0xe3, 0xe8, 0x4c,
		  0xc3, 0x16, 0x6e, 0x4b, 0x7f, 0xa2, 0x89, 0x0d },
		{ 0x79, 0xd2, 0x62, 0x21, 0xb8, 0x7b, 0x58, 0x4c,
		  0xd4, 0x2f, 0xbc, 0x4f, 0xfe, 0xa5, 0xde, 0x9a },
		{ 0x0e, 0x93, 0x69, 0x1a, 0x0c, 0xfc, 0x60, 0x40,
		  0x8b, 0x7b, 0x68, 0xf6, 0x6b, 0x51, 0x3c, 0x13 },
		{ 0xe6, 0xa8, 0x09, 0x4f, 0xee, 0x0a, 0xa2, 0x04,
		  0xfd, 0x97, 0xbc, 0xb0, 0xb4, 0x4b, 0x85, 0x80 }
	};
	unsigned char buf[16];
	int i, ret;

	memcpy(buf, vals[0], 16);

	for (i = 0; i < 4; i++) {
		L_func(buf, buf, ctx);
		ret = memcmp(buf, vals[i + 1], 16);
		if (ret != 0) {
			printf("L_func Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i + 1]);
			return 1;
		}
	}

	for (i = 4; i > 0; i--) {
		Linv(buf, buf, ctx);
		ret = memcmp(buf, vals[i - 1], 16);
		if (ret != 0) {
			printf("Linv Fail. iter %d, key: \n", i);
			PRINT_BLOCK(buf);
			PRINT_BLOCK(vals[i - 1]);
			return 1;
		}
	}

	return 0;
}

int
test_keygen(struct kuzn_ctx *ctx)
{
	uint8_t vals[] = {
		0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
		0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
		0xdb, 0x31, 0x48, 0x53, 0x15, 0x69, 0x43, 0x43, 0x22, 0x8d, 0x6a, 0xef, 0x8c, 0xc7, 0x8c, 0x44, 
		0x3d, 0x45, 0x53, 0xd8, 0xe9, 0xcf, 0xec, 0x68, 0x15, 0xeb, 0xad, 0xc4, 0x0a, 0x9f, 0xfd, 0x04, 
		0x57, 0x64, 0x64, 0x68, 0xc4, 0x4a, 0x5e, 0x28, 0xd3, 0xe5, 0x92, 0x46, 0xf4, 0x29, 0xf1, 0xac, 
		0xbd, 0x07, 0x94, 0x35, 0x16, 0x5c, 0x64, 0x32, 0xb5, 0x32, 0xe8, 0x28, 0x34, 0xda, 0x58, 0x1b, 
		0x51, 0xe6, 0x40, 0x75, 0x7e, 0x87, 0x45, 0xde, 0x70, 0x57, 0x27, 0x26, 0x5a, 0x00, 0x98, 0xb1, 
		0x5a, 0x79, 0x25, 0x01, 0x7b, 0x9f, 0xdd, 0x3e, 0xd7, 0x2a, 0x91, 0xa2, 0x22, 0x86, 0xf9, 0x84, 
		0xbb, 0x44, 0xe2, 0x53, 0x78, 0xc7, 0x31, 0x23, 0xa5, 0xf3, 0x2f, 0x73, 0xcd, 0xb6, 0xe5, 0x17, 
		0x72, 0xe9, 0xdd, 0x74, 0x16, 0xbc, 0xf4, 0x5b, 0x75, 0x5d, 0xba, 0xa8, 0x8e, 0x4a, 0x40, 0x43, 
	};
	uint8_t key[] = {
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff,
		0x00, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0xfe, 0xdc, 0xba, 0x98,
		0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67,
		0x89, 0xab, 0xcd, 0xef, 
	};

	kuzn_set_key(ctx, key);

	return memcmp(vals, ctx->keys, sizeof(vals));
}

int
test_encryption(struct kuzn_ctx *ctx)
{
	uint8_t plaintext[] = {
		0x11, 0x22, 0x33, 0x44,
		0x55, 0x66, 0x77, 0x00,
		0xff, 0xee, 0xdd, 0xcc,
		0xbb, 0xaa, 0x99, 0x88, 
	};
	uint8_t val[] = {
		0x7f, 0x67, 0x9d, 0x90, 0xbe, 0xbc, 0x24, 0x30, 0x5a, 0x46, 0x8d, 0x42, 0xb9, 0xd4, 0xed, 0xcd
	};
	uint8_t chiphertext[16], decrypted[16];

	kuzn_encrypt(ctx, plaintext, chiphertext);
	kuzn_decrypt(ctx, chiphertext, decrypted);
	
	if (memcmp(plaintext, decrypted, sizeof(decrypted)) != 0)
		return 1;
	if (memcmp(chiphertext, val, sizeof(decrypted)) != 0)
		return 1;

	return 0;
}

int
test_kuzn()
{
	struct kuzn_ctx ctx;

	kuzn_context_init(&ctx);

	if (test_S() != 0) {
		printf("[-] S function test failed\n");
		return 1;
	} else {
		printf("[+] S test OK!\n");
	}
	
	if (test_R(&ctx) != 0) {
		printf("[-] R function test failed\n");
		return 1;
	} else {
		printf("[+] R test OK!\n");
	}

	if (test_L(&ctx) != 0) {
		printf("[-] L function test failed\n");
		return 1;
	} else {
		printf("[+] L test OK!\n");
	}

	if (test_keygen(&ctx) != 0) {
		printf("[-] expand key test failed\n");
		return 1;
	} else {
		printf("[+] expand key test OK!\n");
	}

	if (test_encryption(&ctx) != 0) {
		printf("[-] Encrypt/decrypt test failed\n");
		return 1;
	} else {
		printf("[+] Encrypt/decrypt test OK!\n");
	}

	return 0;
}

#endif
