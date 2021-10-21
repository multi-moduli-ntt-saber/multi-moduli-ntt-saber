
#include <string.h>
#include <stdlib.h>

#include "SABER_indcpa.h"
#include "randombytes.h"
#include "fips202.h"
#include "poly.h"
#include "opt_mask.h"


#define KEYPAIR_MASK ((MASK >> 12) & 0xf)
#define ENC_MASK ((MASK >> 8) & 0xf)
#define ENCCMP_MASK ((MASK >> 4) & 0xf)
#define DEC_MASK ((MASK >> 0) & 0xf)

void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES]){

    uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES; // pk[1] <- seed_A
    uint8_t *seed_s = sk; // temporary storage

    randombytes(seed_A, SABER_SEEDBYTES);
    randombytes(seed_s, SABER_NOISE_SEEDBYTES);
    shake128(seed_A, SABER_SEEDBYTES, seed_A, SABER_SEEDBYTES); // for not revealing system RNG state

#if KEYPAIR_MASK == 1
    MatrixVectorMulKeyPairNTT_A(pk, sk);
#elif KEYPAIR_MASK == 2
    MatrixVectorMulKeyPairNTT_B(pk, sk);
#elif KEYPAIR_MASK == 4
#error "KEYPAIR_MASK not supported"
#elif KEYPAIR_MASK == 8
    MatrixVectorMulKeyPairNTT_D(pk, sk);
#else
#error "Invalid KEYPAIR_MASK"
#endif

}


void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_s[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC]){

    const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    uint8_t *ct0 = ciphertext;
    uint8_t *ct1 = ciphertext + SABER_POLYVECCOMPRESSEDBYTES;

#if ENC_MASK == 1
    MatrixVectorMulEncNTT_A(ct0, ct1, seed_s, seed_A, pk, m, 0);
#elif ENC_MASK == 2
#error "ENC_MASK not supported"
#elif ENC_MASK == 4
    MatrixVectorMulEncNTT_C(ct0, ct1, seed_s, seed_A, pk, m, 0);
#elif ENC_MASK == 8
    MatrixVectorMulEncNTT_D(ct0, ct1, seed_s, seed_A, pk, m, 0);
#else
#error "Invalid ENC_MASK"
#endif

}

uint8_t indcpa_kem_enc_cmp(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_s[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC]){

    uint32_t fail = 0;
    const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    const uint8_t *ct0 = ciphertext;
    const uint8_t *ct1 = ciphertext + SABER_POLYVECCOMPRESSEDBYTES;

#if ENCCMP_MASK == 1
    fail |= MatrixVectorMulEncNTT_A((uint8_t*)ct0, (uint8_t*)ct1, seed_s, seed_A, pk, m, 1);
#elif ENCCMP_MASK == 2
#error "ENCCMP_MASK not supported"
#elif ENCCMP_MASK == 4
    fail |= MatrixVectorMulEncNTT_C((uint8_t*)ct0, (uint8_t*)ct1, seed_s, seed_A, pk, m, 1);
#elif ENCCMP_MASK == 8
    fail |= MatrixVectorMulEncNTT_D((uint8_t*)ct0, (uint8_t*)ct1, seed_s, seed_A, pk, m, 1);
#else
#error "Invalid ENCCMP_MASK"
#endif

    fail = (~fail + 1);
    fail >>= 31;

    return (uint8_t)fail;
}

void indcpa_kem_dec(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES]){

#if DEC_MASK == 1
    InnerProdDecNTT(m, ciphertext, sk); // m <- Pack(Round(b'*s - cm))
#elif DEC_MASK == 2
#error "DEC_MASK not supported"
#elif DEC_MASK == 4
#error "DEC_MASK not supported"
#elif DEC_MASK == 8
    InnerProdDecNTT_stack(m, ciphertext, sk); // m <- Pack(Round(b'*s - cm))
#else
#error "Invalid DEC_MASK"
#endif

}
