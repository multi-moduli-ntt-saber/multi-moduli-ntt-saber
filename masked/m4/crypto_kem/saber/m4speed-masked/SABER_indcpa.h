#ifndef INDCPA_H
#define INDCPA_H

#include "SABER_params.h"
#include "api.h"
#include <stdint.h>

void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES]);
void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC]);

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t indcpa_kem_enc_cmp_masked(const uint8_t m[SABER_SHARES][SABER_KEYBYTES], const uint8_t seed_sp[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC]);
void indcpa_kem_dec_masked(uint16_t s[SABER_SHARES][SABER_L][SABER_N], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_SHARES][SABER_KEYBYTES]);

#endif