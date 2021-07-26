#ifndef POLY_H
#define POLY_H

#include "SABER_params.h"
#include "fips202.h"
#include "api.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

void GenSecret_masked(uint16_t s[SABER_SHARES][SABER_L][SABER_N], const uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES]);
void MatrixVectorMulKeyPair_masked(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint16_t s[SABER_SHARES][SABER_L][SABER_N]);
void MatrixVectorMulEnc_masked(uint8_t ct0[SABER_POLYVECCOMPRESSEDBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t m[SABER_SHARES][SABER_KEYBYTES]);
void InnerProdDec_masked(uint8_t m[SABER_SHARES][SABER_KEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint16_t s[SABER_SHARES][SABER_L][SABER_N]);
void MatrixVectorMulEncCmp_masked(uint8_t ct0[SABER_POLYVECCOMPRESSEDBYTES], uint8_t ct1[SABER_SCALEBYTES_KEM], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t seed_sp[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t m[SABER_SHARES][SABER_KEYBYTES], shake128incctx shake_masking_ctx[2]);

#endif
