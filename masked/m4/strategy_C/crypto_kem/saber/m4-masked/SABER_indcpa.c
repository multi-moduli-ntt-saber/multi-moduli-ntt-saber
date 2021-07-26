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

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

void indcpa_kem_keypair_masked(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], sk_masked_s *sk_masked){

    uint16_t s[SABER_SHARES][SABER_L][SABER_N];
    uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES];

    uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES; // pk[1] <- seed_A

    size_t i, j, l;
    uint32_t rand;

    randombytes(seed_A, SABER_SEEDBYTES);
    shake128(seed_A, SABER_SEEDBYTES, seed_A, SABER_SEEDBYTES); // for not revealing system RNG state

    randombytes(seed_s[0], SABER_NOISE_SEEDBYTES);

    // mask the secret seed
    for(i = 0; i < SABER_NOISE_SEEDBYTES; i += 4){
        rand = random_uint32();
        seed_s[1][i + 0] = (uint8_t)(rand >> 0);
        seed_s[0][i + 0] ^= (uint8_t)(rand >> 0);
        seed_s[1][i + 1] = (uint8_t)(rand >> 8);
        seed_s[0][i + 1] ^= (uint8_t)(rand >> 8);
        seed_s[1][i + 2] = (uint8_t)(rand >> 16);
        seed_s[0][i + 2] ^= (uint8_t)(rand >> 16);
        seed_s[1][i + 3] = (uint8_t)(rand >> 24);
        seed_s[0][i + 3] ^= (uint8_t)(rand >> 24);
    }

    GenSecret_masked(s, seed_s);

    // store the shares of secret polynomial
    for(i = 0; i < SABER_L; i++){
        for(j = 0; j < SABER_N; j++){
            for(l = 0; l < SABER_SHARES; l++){
                sk_masked->s[l][i][j] = s[l][i][j];
            }
        }
    }

    MatrixVectorMulKeyPair_masked(pk, s); // pk[0] <- Pack(Round((A^T)*s)), sk <- s

}

void indcpa_kem_enc_masked(const uint8_t m[SABER_SHARES][SABER_KEYBYTES], const uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC])
{
    const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    uint8_t *ct0 = ciphertext;
    uint8_t *ct1 = ciphertext + SABER_POLYVECCOMPRESSEDBYTES;

    MatrixVectorMulEnc_masked(ct0, ct1, pk, seed_A, seed_s, m);

}

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