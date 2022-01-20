
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

#include "opt_mask.h"

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

void MatrixVectorMulKeyPairNTT_A(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES]){

    uint32_t s_NTT[SABER_N];
    uint32_t acc_NTT[SABER_L * SABER_N];
    uint32_t A_NTT[SABER_N];

    uint16_t poly[SABER_N];

    uint8_t shake_out[MAX(SABER_POLYBYTES, SABER_POLYCOINBYTES)];

    uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;
    uint8_t *seed_s = sk;

    size_t i, j;

    shake128incctx shake_s_ctx = shake128_absorb_seed(seed_s);
    shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);

    for (i = 0; i < SABER_L; i++) {

        shake128_inc_squeeze(shake_out, SABER_POLYCOINBYTES, &shake_s_ctx);
        cbd(poly, shake_out);

        POLq2BS(sk + i * SABER_POLYSECRETBYTES, poly); // sk <- s
        NTT_forward_32(s_NTT, poly);

        for (j = 0; j < SABER_L; j++) {

            shake128_inc_squeeze(shake_out, SABER_POLYBYTES, &shake_A_ctx);
            BS2POLq(shake_out, poly);

            NTT_forward_32(A_NTT, poly);

            if(i == 0){
                NTT_mul_32(acc_NTT + j * SABER_N, A_NTT, s_NTT);
            }else{
                NTT_mul_acc_32(acc_NTT + j * SABER_N, A_NTT, s_NTT);
            }

        }
    }

    shake128_inc_ctx_release(&shake_A_ctx);
    shake128_inc_ctx_release(&shake_s_ctx);

    for (i = 0; i < SABER_L; i++) {

        NTT_inv_central_32(acc_NTT + i * SABER_N);

        for (j = 0; j < SABER_N; j++) {
            poly[j] = ((acc_NTT[i * SABER_N + j] + h1) >> (SABER_EQ - SABER_EP));
        }

        POLp2BS(pk + i * SABER_POLYCOMPRESSEDBYTES, poly);
    }

}

uint32_t MatrixVectorMulEncNTT_A(uint8_t ct0[SABER_POLYVECCOMPRESSEDBYTES], uint8_t ct1[SABER_SCALEBYTES_KEM], const uint8_t seed_s[SABER_NOISE_SEEDBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t m[SABER_KEYBYTES], int compare){

    uint32_t acc_NTT[SABER_N];
    uint32_t A_NTT[SABER_N];
    uint32_t s_NTT[SABER_L * SABER_N];

    uint16_t poly[SABER_N];
    uint16_t acc[SABER_N];

    uint8_t shake_out[MAX(SABER_POLYBYTES, SABER_POLYCOINBYTES)];

    uint16_t *mp = poly;

    size_t i, j;
    uint32_t fail = 0;

    shake128incctx shake_s_ctx = shake128_absorb_seed(seed_s);

    for(i = 0; i < SABER_L; i++){
        shake128_inc_squeeze(shake_out, SABER_POLYCOINBYTES, &shake_s_ctx);
        cbd(poly, shake_out);
        NTT_forward_32(s_NTT + i * SABER_N, poly);
    }

    shake128_inc_ctx_release(&shake_s_ctx);

    shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);

    for (i = 0; i < SABER_L; i++) {

        for (j = 0; j < SABER_L; j++) {

            shake128_inc_squeeze(shake_out, SABER_POLYBYTES, &shake_A_ctx);
            BS2POLq(shake_out, poly);

            NTT_forward_32(A_NTT, poly);

            if (j == 0) {
                NTT_mul_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
            } else {
                NTT_mul_acc_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
            }
        }

        NTT_inv_central_32(acc_NTT);

        for (j = 0; j < SABER_N; j++) {
            acc[j] = ((acc_NTT[j] + h1) >> (SABER_EQ - SABER_EP));
        }

        if (compare) {
            fail |= POLp2BS_cmp(ct0 + i * SABER_POLYCOMPRESSEDBYTES, acc);
        } else {
            POLp2BS(ct0 + i * SABER_POLYCOMPRESSEDBYTES, acc);
        }
    }

    shake128_inc_ctx_release(&shake_A_ctx);

    for(j = 0; j < SABER_L; j++){

        BS2POLp(pk + j * SABER_POLYCOMPRESSEDBYTES, poly);

        NTT_forward_32(A_NTT, poly);

        if(j == 0){
            NTT_mul_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
        }else{
            NTT_mul_acc_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
        }

    }

    NTT_inv_central_32(acc_NTT);

    BS2POLmsg(m, mp);

    for(j = 0; j < SABER_N; j++){
        acc[j] = (acc_NTT[j] - (mp[j] << (SABER_EP - 1)) + h1) >> (SABER_EP - SABER_ET);
    }

    if(compare){
        fail |= POLT2BS_cmp(ct1, acc);
    }else{
        POLT2BS(ct1, acc);
    }

    return fail;

}

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

extern void ClearStack(size_t len);

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














