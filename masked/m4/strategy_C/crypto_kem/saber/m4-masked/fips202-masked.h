#ifndef FIPS202_MASKED_H
#define FIPS202_MASKED_H

#define NROUNDS 24

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE  72

#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

#include "SABER_params.h"
#include <stdint.h>
#include <stddef.h>

void shake128_absorb_masked(uint64_t s[SABER_SHARES][25], uint32_t r, size_t mlen, const uint8_t m[SABER_SHARES][mlen], uint8_t p);
void shake128_squeeze_masked(size_t outlen, size_t tlen, uint8_t *output, uint64_t s[SABER_SHARES][25]);

void shake128_masked(size_t outlen, uint8_t *output, size_t inlen, const uint8_t input[SABER_SHARES][inlen]);
void sha3_256_masked(uint8_t output[SABER_SHARES][32], size_t inlen, const uint8_t input[SABER_SHARES][inlen]);
void sha3_512_masked(uint8_t output[SABER_SHARES][64], size_t inlen, const uint8_t input[SABER_SHARES][inlen]);

#endif