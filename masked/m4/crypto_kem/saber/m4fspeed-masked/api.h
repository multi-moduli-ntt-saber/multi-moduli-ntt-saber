
/*
 * MIT License
 *
 * Copyright (c) 2021: imec-COSIC KU Leuven, 3001 Leuven, Belgium
 * Author: Michiel Van Beirendonck <michiel.vanbeirendonck@esat.kuleuven.be>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef API_H
#define API_H

#include "SABER_params.h"
#include <stdint.h>

#if SABER_L == 2
    #define CRYPTO_ALGNAME "LightSaber"
#elif SABER_L == 3
    #define CRYPTO_ALGNAME "Saber"
#elif SABER_L == 4
    #define CRYPTO_ALGNAME "FireSaber"
#else
    #error "Unsupported SABER parameter."
#endif

#define CRYPTO_SECRETKEYBYTES SABER_SECRETKEYBYTES
#define CRYPTO_PUBLICKEYBYTES SABER_PUBLICKEYBYTES
#define CRYPTO_BYTES SABER_KEYBYTES
#define CRYPTO_CIPHERTEXTBYTES SABER_BYTES_CCA_DEC

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint16_t s[SABER_SHARES][SABER_L][SABER_N]; // shared
    uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES];
    uint8_t hpk[SABER_SHARES][32]; // shared
    uint8_t z[SABER_KEYBYTES];
} sk_masked_s;

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_keypair_sk_masked(sk_masked_s *sk_masked, const unsigned char *sk);
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int crypto_kem_enc_masked(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec_masked(uint8_t *k, const uint8_t *c, sk_masked_s *sk_masked);


#endif
