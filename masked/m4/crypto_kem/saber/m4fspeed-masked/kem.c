
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

#include "api.h"
#include "verify.h"
#include "fips202.h"
#include "fips202-masked.h"
#include "randombytes.h"
#include "SABER_indcpa.h"
#include "pack_unpack.h"
#include "masksONOFF.h"
#include <string.h>
#include <stdio.h>

int crypto_kem_keypair(uint8_t *pk, uint8_t *sk)
{
    indcpa_kem_keypair(pk, sk); // sk[0:SABER_INDCPA_SECRETKEYBYTES-1] <-- sk

    memcpy(sk + SABER_INDCPA_SECRETKEYBYTES, pk, SABER_INDCPA_PUBLICKEYBYTES); // sk[SABER_INDCPA_SECRETKEYBYTES:SABER_INDCPA_SECRETKEYBYTES+SABER_INDCPA_SECRETKEYBYTES-1] <-- pk

    sha3_256(sk + SABER_SECRETKEYBYTES - 64, pk, SABER_INDCPA_PUBLICKEYBYTES); // Then hash(pk) is appended.

    randombytes(sk + SABER_SECRETKEYBYTES - SABER_KEYBYTES, SABER_KEYBYTES); // Remaining part of sk contains a pseudo-random number, this is output when check in crypto_kem_dec() fails.

    return (0);
}

int crypto_kem_enc(uint8_t *c, uint8_t *k, const uint8_t *pk)
{
    uint8_t kr[64]; // Will contain key, coins
    uint8_t buf[64];

    randombytes(buf, 32);

    sha3_256(buf, buf, 32); // BUF[0:31] <-- random message (will be used as the key for client) Note: hash doesnot release system RNG output

    sha3_256(buf + 32, pk, SABER_INDCPA_PUBLICKEYBYTES); // BUF[32:63] <-- Hash(public key);  Multitarget countermeasure for coins + contributory KEM

    sha3_512(kr, buf, 64); // kr[0:63] <-- Hash(buf[0:63]), K^ <-- kr[0:31], noiseseed (r) <-- kr[32:63]

    indcpa_kem_enc(buf, kr + 32, pk, c); // buf[0:31] contains message; kr[32:63] contains randomness r;

    sha3_256(kr + 32, c, SABER_BYTES_CCA_DEC);

    sha3_256(k, kr, 64); // hash concatenation of pre-k and h(c) to k

    return (0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

int crypto_kem_keypair_sk_masked(sk_masked_s *sk_masked, const uint8_t *sk)
{
    size_t i, j, l;
    uint32_t rand, hpk0, hpkl;

    BS2POLVECmu(sk, sk_masked->s[0]);
    memcpy(sk_masked->pk, sk + SABER_INDCPA_SECRETKEYBYTES, SABER_INDCPA_PUBLICKEYBYTES);
    memcpy(sk_masked->hpk[0], sk + SABER_SECRETKEYBYTES - 64, 32);
    memcpy(sk_masked->z, sk + SABER_SECRETKEYBYTES - SABER_KEYBYTES, SABER_KEYBYTES);

    for (i = 0; i < SABER_L; i++) {
        for (j = 0; j < SABER_N ; j += 2) {
            for (l = 1; l < SABER_SHARES; l++) {
                rand = random_uint32();
                sk_masked->s[l][i][j] = (uint16_t)rand;
                sk_masked->s[0][i][j] -= (uint16_t)rand;
                sk_masked->s[l][i][j + 1] = (uint16_t)(rand >> 16);
                sk_masked->s[0][i][j + 1] -= (uint16_t)(rand >> 16);
            }
        }
    }

    for (i = 0; i < 32; i += 4) {
        for (l = 1; l < SABER_SHARES; l++) {
            rand = random_uint32();
            memcpy(&hpk0, &sk_masked->hpk[0][i], 4);
            memcpy(&hpkl, &sk_masked->hpk[l][i], 4);
            hpkl = rand;
            hpk0 ^= rand;
            memcpy(&sk_masked->hpk[0][i], &hpk0, 4);
            memcpy(&sk_masked->hpk[l][i], &hpkl, 4);
        }
    }

    return (0);
}

volatile uint32_t randclear = 0;

int crypto_kem_dec_masked(uint8_t *k, const uint8_t *c, sk_masked_s *sk_masked)
{
    uint8_t fail;
    uint8_t buf[SABER_SHARES][64];
    uint8_t kr[SABER_SHARES][64]; // Will contain key, coins
    uint8_t m[SABER_SHARES][SABER_KEYBYTES];
    uint8_t r[SABER_SHARES][SABER_SEEDBYTES];

    const uint8_t *pk = sk_masked->pk;

    indcpa_kem_dec_masked(sk_masked->s, c, m); // buf[0:31] <-- message

    for (size_t l = 0; l < SABER_SHARES; l++) {
        memcpy(&buf[l][0], &m[l][0], 32);
        memcpy(&buf[l][32], sk_masked->hpk[l], 32);
    }

    sha3_512_masked(kr, 64, buf);

    for (size_t l = 0; l < SABER_SHARES; l++) {
        memcpy(r[l], &kr[l][32], 32);
    }

    fail = indcpa_kem_enc_cmp_masked(m, r, pk, c); //in-place verification of the re-encryption

    sha3_256(&kr[0][32], c, SABER_BYTES_CCA_DEC); // overwrite coins in kr with h(c)

    cmov_masked(kr, sk_masked->z, SABER_KEYBYTES, fail);

    sha3_256(k, kr[0], 64); // hash concatenation of pre-k and h(c) to k

    return (0);
}


