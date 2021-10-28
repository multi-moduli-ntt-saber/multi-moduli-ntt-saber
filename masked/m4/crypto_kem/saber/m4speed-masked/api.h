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
