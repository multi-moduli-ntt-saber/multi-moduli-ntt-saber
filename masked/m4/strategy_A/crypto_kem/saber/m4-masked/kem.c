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

int crypto_kem_keypair_masked(uint8_t *pk, sk_masked_s *sk_masked){

    size_t i, l;
    uint32_t rand, hpk0, hpkl;

    indcpa_kem_keypair_masked(pk, sk_masked); // sk[0:SABER_INDCPA_SECRETKEYBYTES-1] <-- sk

    // copy public key
    memcpy(sk_masked->pk, pk, SABER_INDCPA_PUBLICKEYBYTES);

    // hash(pk) is appended
    sha3_256(sk_masked->hpk[0], pk, SABER_INDCPA_PUBLICKEYBYTES);

    // mask the hash of public key
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

    // Remaining part of sk contains a pseudo-random number, this is output when check in crypto_kem_dec() fails.
    randombytes(sk_masked->z, SABER_KEYBYTES);

    return (0);
}

int crypto_kem_enc_masked(uint8_t *c, uint8_t *k, const uint8_t *pk){

    uint8_t m[SABER_SHARES][SABER_KEYBYTES];
    uint8_t hpk[SABER_SHARES][32];
    uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES];
    uint8_t buf[SABER_SHARES][64];
    uint8_t kr[SABER_SHARES][64]; // Will contain key, coins
    uint8_t k_buf[SABER_SHARES][SABER_KEYBYTES];

    size_t i;
    uint32_t rand;

    randombytes(m[0], 32);

    // random message (will be used as the key for client) Note: hash doesnot release system RNG output
    sha3_256(m[0], m[0], 32);

    // Hash(public key);  Multitarget countermeasure for coins + contributory KEM
    sha3_256(hpk[0], pk, SABER_INDCPA_PUBLICKEYBYTES);

    // mask the message
    for(i = 0; i < SABER_KEYBYTES; i += 4){
        rand = random_uint32();
        m[1][i + 0] = (uint8_t)(rand >> 0);
        m[0][i + 0] ^= (uint8_t)(rand >> 0);
        m[1][i + 1] = (uint8_t)(rand >> 8);
        m[0][i + 1] ^= (uint8_t)(rand >> 8);
        m[1][i + 2] = (uint8_t)(rand >> 16);
        m[0][i + 2] ^= (uint8_t)(rand >> 16);
        m[1][i + 3] = (uint8_t)(rand >> 24);
        m[0][i + 3] ^= (uint8_t)(rand >> 24);
    }

    // mask the hash of public key
    for(i = 0; i < 32; i += 4){
        rand = random_uint32();
        hpk[1][i + 0] = (uint8_t)(rand >> 0);
        hpk[0][i + 0] ^= (uint8_t)(rand >> 0);
        hpk[1][i + 1] = (uint8_t)(rand >> 8);
        hpk[0][i + 1] ^= (uint8_t)(rand >> 8);
        hpk[1][i + 2] = (uint8_t)(rand >> 16);
        hpk[0][i + 2] ^= (uint8_t)(rand >> 16);
        hpk[1][i + 3] = (uint8_t)(rand >> 24);
        hpk[0][i + 3] ^= (uint8_t)(rand >> 24);
    }

    memcpy(&buf[0][0], m[0], 32);
    memcpy(&buf[1][0], m[1], 32);

    memcpy(&buf[0][32], hpk[0], 32);
    memcpy(&buf[1][32], hpk[1], 32);

    // hash the concatenation of message and hpk
    sha3_512_masked(kr, 64, buf);

    // acquire the shares of seed_s
    memcpy(seed_s[0], &kr[0][32], SABER_NOISE_SEEDBYTES);
    memcpy(seed_s[1], &kr[1][32], SABER_NOISE_SEEDBYTES);

    indcpa_kem_enc_masked(m, seed_s, pk, c);

    sha3_256(&kr[0][32], c, SABER_BYTES_CCA_DEC);

    for(i = 32; i < 64; i += 4){
        rand = random_uint32();
        kr[1][i + 0] = (uint8_t)(rand >> 0);
        kr[0][i + 0] ^= (uint8_t)(rand >> 0);
        kr[1][i + 1] = (uint8_t)(rand >> 8);
        kr[0][i + 1] ^= (uint8_t)(rand >> 8);
        kr[1][i + 2] = (uint8_t)(rand >> 16);
        kr[0][i + 2] ^= (uint8_t)(rand >> 16);
        kr[1][i + 3] = (uint8_t)(rand >> 24);
        kr[0][i + 3] ^= (uint8_t)(rand >> 24);
    }

    // hash concatenation of pre-k and h(c) to k
    sha3_256_masked(k_buf, 64, kr);

    // derive the key computed
    for(i = 0; i < 32; i++){
        k[i] = k_buf[0][i] ^ k_buf[1][i];
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


