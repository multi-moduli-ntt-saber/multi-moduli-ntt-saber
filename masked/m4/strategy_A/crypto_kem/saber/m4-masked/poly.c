#include "poly.h"
#include "poly_mul.h"
#include "cbd.h"
#include "cbd-masked.h"
#include "fips202.h"
#include "fips202-masked.h"
#include "pack_unpack.h"
#include "A2B.h"
#include "api.h"
#include "randombytes.h"
#include "masksONOFF.h"

#include "NTT.h"

#include <stdio.h>


// #define C
// #define D


#if !defined(C) && !defined(D)
#define A
#endif

#define h1 (1 << (SABER_EQ - SABER_EP - 1))
#define h2 ((1 << (SABER_EP - 2)) - (1 << (SABER_EP - SABER_ET - 1)) + (1 << (SABER_EQ - SABER_EP - 1)))
#define MAX(a,b) (((a)>(b))?(a):(b))

static inline shake128incctx shake128_absorb_seed(const uint8_t seed[SABER_SEEDBYTES])
{
    shake128incctx ctx;
    shake128_inc_init(&ctx);
    shake128_inc_absorb(&ctx, seed, SABER_SEEDBYTES);
    shake128_inc_finalize(&ctx);
    return ctx;
}

// noinline such that toom-cook can reclaim the memory of shake_out
static void __attribute__((noinline)) shake128_squeeze_poly(uint16_t poly[SABER_N], shake128incctx *ctx)
{
    uint8_t shake_out[SABER_POLYBYTES];

    shake128_inc_squeeze(shake_out, SABER_POLYBYTES, ctx);
    BS2POLq(shake_out, poly);
}

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

extern void ClearStack(size_t len);

void GenSecret_masked(uint16_t s[SABER_SHARES][SABER_L][SABER_N], const uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES]){

    size_t i;

    uint8_t shake_out[SABER_SHARES * SABER_L * SABER_POLYCOINBYTES];

    // acquire masked hash of secret seed
    shake128_masked(SABER_L * SABER_POLYCOINBYTES, shake_out, SABER_NOISE_SEEDBYTES, seed_s);

    // masked cbd accompanied with B2A
    for(i = 0; i < SABER_L; i++){
        cbd_masked_single(s[0][i], s[1][i],
            &shake_out[0 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES],
            &shake_out[1 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES]);
    }
}

void MatrixVectorMulKeyPair_masked(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N]){

    size_t i, j, l;

#ifdef A
    uint32_t s_NTT_32[SABER_SHARES][SABER_N];
    // it is intended to declare b in this way
    uint32_t acc_32[SABER_L][SABER_SHARES][SABER_N];
#endif
    uint16_t acc_16[SABER_L][SABER_SHARES][SABER_N];

    uint32_t buff_32[SABER_N];
    uint16_t buff_16[SABER_N];

    uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;

    shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);

    for(i = 0; i < SABER_L; i++){

#ifdef A
        for(l = 0; l < SABER_SHARES; l++){
            NTT_forward_32(s_NTT_32[l], s_NTT_16[l][i]);
            NTT_forward_16(s_NTT_16[l][i], s_NTT_16[l][i]);
        }
#endif

        for(j = 0; j < SABER_L; j++){

            shake128_squeeze_poly(buff_16, &shake_A_ctx);

            NTT_forward_32(buff_32, buff_16);
            NTT_forward_16(buff_16, buff_16);

            for(l = 0; l < SABER_SHARES; l++){

#if defined(C) || defined(D)
                ClearStack(1600);
#endif

                if(i == 0){
#ifdef A
                    NTT_mul_32(acc_32[j][l], buff_32, s_NTT_32[l]);
                    NTT_mul_16(acc_16[j][l], buff_16, s_NTT_16[l][i]);
#else
                    poly_mul_NTT(acc_16[j][l], buff_32, buff_16, s_NTT_16[l][i]);
#endif
                }else{
#ifdef A
                    NTT_mul_acc_32(acc_32[j][l], buff_32, s_NTT_32[l]);
                    NTT_mul_acc_16(acc_16[j][l], buff_16, s_NTT_16[l][i]);
#else
                    poly_mul_acc_NTT(acc_16[j][l], buff_32, buff_16, s_NTT_16[l][i]);
#endif
                }
            }

        }

    }

#ifdef A
    for(j = 0; j < SABER_L; j++){
        for(l = 0; l < SABER_SHARES; l++){
            NTT_inv_32(acc_32[j][l]);
            NTT_inv_16(acc_16[j][l]);
            solv_CRT(acc_16[j][l], acc_32[j][l], acc_16[j][l]);
        }
    }
#endif

    shake128_inc_ctx_release(&shake_A_ctx);

    // add constant for rounding
    for(i = 0; i < SABER_L; i++){
        for(j = 0; j < SABER_N; j++){
            acc_16[i][0][j] += h1;
        }
    }

    // masked logical shift
    for(i = 0; i < SABER_L; i++){
        poly_A2A(acc_16[i], SABER_Q - 1, 1);
    }

    // add the arithmetic shares
    for(i = 0; i < SABER_L; i++){
        for(j = 0; j < SABER_N; j++){
            acc_16[i][0][j] += acc_16[i][1][j];
        }
    }

    // Pack
    for(i = 0; i < SABER_L; i++){
        POLp2BS(&pk[i * SABER_POLYCOMPRESSEDBYTES], acc_16[i][0]);
    }


}

void MatrixVectorMulEnc_masked(uint8_t ct0[SABER_POLYVECCOMPRESSEDBYTES], uint8_t ct1[SABER_SCALEBYTES_KEM], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t seed_s[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t m[SABER_SHARES][SABER_KEYBYTES]){

    size_t i, j, l;

#ifdef A
    uint32_t s_NTT_32[SABER_SHARES][SABER_L][SABER_N];
    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

    uint32_t buff_32[SABER_N];
    uint16_t buff_16[SABER_N];

    uint32_t acc_32[SABER_SHARES * MAX(SABER_N, (SABER_L * SABER_POLYCOINBYTES + 8) / 8)];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t *shake_out = (uint8_t*)acc_32;

#elif defined(C)

    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

    uint32_t buff_32[SABER_N];
    uint16_t buff_16[SABER_N];

    uint32_t acc_32[SABER_SHARES * MAX(SABER_N, (SABER_L * SABER_POLYCOINBYTES + 8) / 8)];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t *shake_out = (uint8_t*)acc_32;

#else

    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t shake_out[SABER_SHARES * SABER_L * SABER_POLYCOINBYTES];

    uint32_t *buff_32 = (uint32_t*)shake_out;
    uint8_t *tmp = shake_out + 4 * SABER_N;
    uint16_t *buff_16 = (uint16_t*)tmp;

#endif



// ================

    // acquire masked hash of secret seed
    shake128_masked(SABER_L * SABER_POLYCOINBYTES, shake_out, SABER_NOISE_SEEDBYTES, seed_s);

    // masked cbd accompanied with B2A
    for(i = 0; i < SABER_L; i++){
        cbd_masked_single(s_NTT_16[0][i], s_NTT_16[1][i],
            &shake_out[0 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES],
            &shake_out[1 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES]);
    }

#ifdef A
    for(i = 0; i < SABER_L; i++){
        for(l = 0; l < SABER_SHARES; l++){
            NTT_forward_32(s_NTT_32[l][i], s_NTT_16[l][i]);
            NTT_forward_16(s_NTT_16[l][i], s_NTT_16[l][i]);
        }
    }
#endif

// ================

    shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);

    for(i = 0; i < SABER_L; i++){

        for(j = 0; j < SABER_L; j++){

            shake128_squeeze_poly(buff_16, &shake_A_ctx);

            NTT_forward_32(buff_32, buff_16);
            NTT_forward_16(buff_16, buff_16);

            for(l = 0; l < SABER_SHARES; l++){

#if defined(C) || defined(D)
                ClearStack(1600);
#endif

                if(j == 0){
#ifdef A
                    NTT_mul_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                    NTT_mul_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                    poly_mul_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                    poly_mul_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif

                }else{
#ifdef A
                    NTT_mul_acc_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                    NTT_mul_acc_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                    poly_mul_acc_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                    poly_mul_acc_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
                }
            }

        }

#if defined(A) || defined(C)
        for(l = 0; l < SABER_SHARES; l++){
            NTT_inv_32(acc_32 + l * SABER_N);
            NTT_inv_16(acc_16[l]);
            solv_CRT(acc_16[l], acc_32 + l * SABER_N, acc_16[l]);
        }
#endif

        for(j = 0; j < SABER_N; j++){
            acc_16[0][j] += h1;
        }

        poly_A2A(acc_16, SABER_Q - 1, 1);

        for(j = 0; j < SABER_N; j++){
            acc_16[0][j] += acc_16[1][j];
        }

        POLp2BS(&ct0[i * SABER_POLYCOMPRESSEDBYTES], acc_16[0]);

    }

    shake128_inc_ctx_release(&shake_A_ctx);

// ================

    for (j = 0; j < SABER_L; j++) {

        BS2POLp(&pk[j * SABER_POLYCOMPRESSEDBYTES], buff_16);

        NTT_forward_32(buff_32, buff_16);
        NTT_forward_16(buff_16, buff_16);

        for(l = 0; l < SABER_SHARES; l++){

#if defined(C) || defined(D)
                ClearStack(1600);
#endif

            if(j == 0){
#ifdef A
                NTT_mul_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                NTT_mul_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                poly_mul_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                poly_mul_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
            }else{
#ifdef A
                NTT_mul_acc_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                NTT_mul_acc_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                poly_mul_acc_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                poly_mul_acc_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
            }
        }

    }

#if defined(A) || defined(C)
    for(l = 0; l < SABER_SHARES; l++){
        NTT_inv_32(acc_32 + l * SABER_N);
        NTT_inv_16(acc_16[l]);
        solv_CRT(acc_16[l], acc_32 + l * SABER_N, acc_16[l]);
    }
#endif

    for(l = 0; l < SABER_SHARES; l++){
        BS2POLmsg(m[l], buff_16);
        for(j = 0; j < SABER_N; j++){
            acc_16[l][j] = acc_16[l][j] - (buff_16[j] << (SABER_EP - 1));
            if(l == 0){
                acc_16[l][j] += h1;
            }
        }
    }

    poly_A2A(acc_16, SABER_P - 1, 2);

    for(i = 0; i < SABER_N; i++){
        acc_16[0][i] += acc_16[1][i];
    }

    POLT2BS(ct1, acc_16[0]);

}

void MatrixVectorMulEncCmp_masked(uint8_t ct0[SABER_POLYVECCOMPRESSEDBYTES], uint8_t ct1[SABER_SCALEBYTES_KEM], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t seed_sp[SABER_SHARES][SABER_NOISE_SEEDBYTES], const uint8_t m[SABER_SHARES][SABER_KEYBYTES], shake128incctx shake_masking_ctx[2]){

    size_t i, j, l;

#ifdef A
    uint32_t s_NTT_32[SABER_SHARES][SABER_L][SABER_N];
    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

    uint32_t buff_32[SABER_N];
    uint16_t buff_16[SABER_N];

    uint32_t acc_32[SABER_SHARES * MAX(SABER_N, (SABER_L * SABER_POLYCOINBYTES + 8) / 8)];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t *shake_out = (uint8_t*)acc_32;

#elif defined(C)

    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

    uint32_t buff_32[SABER_N];
    uint16_t buff_16[SABER_N];

    uint32_t acc_32[SABER_SHARES * MAX(SABER_N, (SABER_L * SABER_POLYCOINBYTES + 8) / 8)];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t *shake_out = (uint8_t*)acc_32;

#else

    uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    uint8_t shake_out[SABER_SHARES * SABER_L * SABER_POLYCOINBYTES];

    uint32_t *buff_32 = (uint32_t*)shake_out;
    uint8_t *tmp = shake_out + 4 * SABER_N;
    uint16_t *buff_16 = (uint16_t*)tmp;

#endif

    uint8_t *cmp = shake_out;

// ================

    shake128_masked(SABER_L * SABER_POLYCOINBYTES, shake_out, SABER_NOISE_SEEDBYTES, seed_sp);

    for(i = 0; i < SABER_L; i++){
        cbd_masked_single(s_NTT_16[0][i], s_NTT_16[1][i],
            &shake_out[0 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES],
            &shake_out[1 * SABER_L * SABER_POLYCOINBYTES + i * SABER_POLYCOINBYTES]);
    }

#ifdef A
    for(i = 0; i < SABER_L; i++){
        for(j = 0; j < SABER_SHARES; j++){
            NTT_forward_32(s_NTT_32[j][i], s_NTT_16[j][i]);
            NTT_forward_16(s_NTT_16[j][i], s_NTT_16[j][i]);
        }
    }
#endif

// ================

    shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);

    for (i = 0; i < SABER_L; i++) {

        for (j = 0; j < SABER_L; j++) {

            shake128_squeeze_poly(buff_16, &shake_A_ctx);

            NTT_forward_32(buff_32, buff_16);
            NTT_forward_16(buff_16, buff_16);

            for(l = 0; l < SABER_SHARES; l++){

#if defined(C) || defined(D)
                ClearStack(1600);
#endif

                if(j == 0){
#ifdef A
                    NTT_mul_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                    NTT_mul_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                    poly_mul_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                    poly_mul_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
                }else{
#ifdef A
                    NTT_mul_acc_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                    NTT_mul_acc_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                    poly_mul_acc_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                    poly_mul_acc_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
                }

            }

        }

#if defined(A) || defined(C)
        for(l = 0; l < SABER_SHARES; l++){
            NTT_inv_32(acc_32 + l * SABER_N);
            NTT_inv_16(acc_16[l]);
            solv_CRT(acc_16[l], acc_32 + l * SABER_N, acc_16[l]);
        }
#endif

        for (j = 0; j < SABER_N; j++) {
            acc_16[0][j] += h1;
        }

        poly_A2A(acc_16, SABER_Q - 1, 1);

        POLp2BS_sub(cmp + 0 * SABER_POLYCOMPRESSEDBYTES, &ct0[i * SABER_POLYCOMPRESSEDBYTES], acc_16[0]);
        POLp2BS(cmp + 1 * SABER_POLYCOMPRESSEDBYTES, acc_16[1]);

        shake128_inc_absorb(&shake_masking_ctx[0], cmp + 0 * SABER_POLYCOMPRESSEDBYTES, SABER_POLYCOMPRESSEDBYTES);
        shake128_inc_absorb(&shake_masking_ctx[1], cmp + 1 * SABER_POLYCOMPRESSEDBYTES, SABER_POLYCOMPRESSEDBYTES);

    }
    shake128_inc_ctx_release(&shake_A_ctx);

// ================

    for (j = 0; j < SABER_L; j++) {

        BS2POLp(&pk[j * SABER_POLYCOMPRESSEDBYTES], buff_16);

        NTT_forward_32(buff_32, buff_16);
        NTT_forward_16(buff_16, buff_16);

        for(l = 0; l < SABER_SHARES; l++){

#if defined(C) || defined(D)
                ClearStack(1600);
#endif

            if(j == 0){
#ifdef A
                NTT_mul_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                NTT_mul_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                poly_mul_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                poly_mul_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
            }else{
#ifdef A
                NTT_mul_acc_32(acc_32 + l * SABER_N, buff_32, s_NTT_32[l][j]);
                NTT_mul_acc_16(acc_16[l], buff_16, s_NTT_16[l][j]);
#elif defined(C)
                poly_mul_acc_NTT_C(acc_16[l], acc_32 + l * SABER_N, buff_32, buff_16, s_NTT_16[l][j]);
#else
                poly_mul_acc_NTT(acc_16[l], buff_32, buff_16, s_NTT_16[l][j]);
#endif
            }
        }

    }

#if defined(A) || defined(C)
    for(l = 0; l < SABER_SHARES; l++){
        NTT_inv_32(acc_32 + l * SABER_N);
        NTT_inv_16(acc_16[l]);
        solv_CRT(acc_16[l], acc_32 + l * SABER_N, acc_16[l]);
    }
#endif

    for (l = 0; l < SABER_SHARES; l++) {
        BS2POLmsg(m[l], buff_16);
        for (j = 0; j < SABER_N; j++) {
            acc_16[l][j] = acc_16[l][j] - (buff_16[j] << (SABER_EP - 1));
            if (l == 0) {
                acc_16[l][j] += h1;
            }
        }
    }

    poly_A2A(acc_16, SABER_P - 1, 2);

    POLT2BS_sub(cmp + 0 * SABER_POLYCOMPRESSEDBYTES, ct1, acc_16[0]);
    POLT2BS(cmp + 1 * SABER_POLYCOMPRESSEDBYTES, acc_16[1]);

    shake128_inc_absorb(&shake_masking_ctx[0], cmp + 0 * SABER_POLYCOMPRESSEDBYTES, SABER_SCALEBYTES_KEM);
    shake128_inc_absorb(&shake_masking_ctx[1], cmp + 1 * SABER_POLYCOMPRESSEDBYTES, SABER_SCALEBYTES_KEM);


}

void InnerProdDec_masked(uint8_t m[SABER_SHARES][SABER_KEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint16_t s[SABER_SHARES][SABER_L][SABER_N]){

    size_t i, j;

    uint32_t cipher_NTT_32[SABER_N];
    uint16_t cipher_NTT_16[SABER_N];

    uint32_t s_NTT_32[SABER_SHARES][SABER_N];
    uint16_t s_NTT_16[SABER_SHARES][SABER_N];

    uint32_t acc_32[SABER_SHARES][SABER_N];
    uint16_t acc_16[SABER_SHARES][SABER_N];

    for(i = 0; i < SABER_L; i++){

        BS2POLp(&ciphertext[i * SABER_POLYCOMPRESSEDBYTES], cipher_NTT_16);

        NTT_forward_32(cipher_NTT_32, cipher_NTT_16);
        NTT_forward_16(cipher_NTT_16, cipher_NTT_16);

        for(j = 0; j < SABER_SHARES; j++){

            NTT_forward_32(s_NTT_32[j], s[j][i]);
            NTT_forward_16(s_NTT_16[j], s[j][i]);

            if(i == 0){
                NTT_mul_32(acc_32[j], cipher_NTT_32, s_NTT_32[j]);
                NTT_mul_16(acc_16[j], cipher_NTT_16, s_NTT_16[j]);
            }else{
                NTT_mul_acc_32(acc_32[j], cipher_NTT_32, s_NTT_32[j]);
                NTT_mul_acc_16(acc_16[j], cipher_NTT_16, s_NTT_16[j]);
            }

        }

    }

    for(j = 0; j < SABER_SHARES; j++){
        NTT_inv_32(acc_32[j]);
        NTT_inv_16(acc_16[j]);
        solv_CRT(acc_16[j], acc_32[j], acc_16[j]);
    }

    BS2POLT(ciphertext + SABER_POLYVECCOMPRESSEDBYTES, cipher_NTT_16);

    for (i = 0; i < SABER_N; i++) {
        acc_16[0][i] += h2 - (cipher_NTT_16[i] << (SABER_EP - SABER_ET));
    }

    poly_A2A(acc_16, SABER_P - 1, 3);

    for (i = 0; i < SABER_SHARES; i++) {
        POLmsg2BS(m[i], acc_16[i]);
    }

}














