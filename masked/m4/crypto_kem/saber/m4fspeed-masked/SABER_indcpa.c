#include "SABER_indcpa.h"
#include "randombytes.h"
#include "fips202.h"
#include "fips202-masked.h"
#include "poly.h"
#include "verify.h"
#include "api.h"
#include "pack_unpack.h"
#include "masksONOFF.h"
#include <string.h>
#include <stdlib.h>

void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES]){

    uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES; // pk[1] <- seed_A
    uint8_t *seed_s = sk; // temporary storage

    randombytes(seed_A, SABER_SEEDBYTES);
    randombytes(seed_s, SABER_NOISE_SEEDBYTES);
    shake128(seed_A, SABER_SEEDBYTES, seed_A, SABER_SEEDBYTES); // for not revealing system RNG state

    MatrixVectorMulKeyPairNTT_A(pk, sk);

}

void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_s[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC]){

    const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    uint8_t *ct0 = ciphertext;
    uint8_t *ct1 = ciphertext + SABER_POLYVECCOMPRESSEDBYTES;

    MatrixVectorMulEncNTT_A(ct0, ct1, seed_s, seed_A, pk, m, 0);

}

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t indcpa_kem_enc_cmp_masked(const uint8_t m[SABER_SHARES][SABER_KEYBYTES], const uint8_t seed_sp[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC])
{
    const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    const uint8_t *ct0 = ciphertext;
    const uint8_t *ct1 = ciphertext + SABER_POLYVECCOMPRESSEDBYTES;

    shake128incctx shake_masked_compare[2];
    shake128_inc_init(&shake_masked_compare[0]);
    shake128_inc_init(&shake_masked_compare[1]);

    MatrixVectorMulEncCmp_masked((uint8_t*)ct0, (uint8_t*)ct1, pk, seed_A, seed_sp, m, shake_masked_compare); // verify(ct[0], Pack(Round(A*s')))

    return verify_masked(shake_masked_compare);
}

void indcpa_kem_dec_masked(uint16_t s[SABER_SHARES][SABER_L][SABER_N], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_SHARES][SABER_KEYBYTES])
{
    InnerProdDec_masked(m, ciphertext, s); // m <- Pack(Round(b'*s - cm))
}