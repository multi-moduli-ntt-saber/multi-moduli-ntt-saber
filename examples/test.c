
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hal.h"
#include "tools.h"
#include "NTT_params.h"
#include "gen_table.h"
#include "ntt_c.h"
#include "NTT.h"
#include "naive_mult.h"

char out[128];
char *out_ptr;
uint64_t oldcount, newcount;

#define BUFF_MAX (NTT_N << 4)

struct compress_profile profile;

extern void __asm_negacyclic_ntt_16_light_0_1_2(int16_t *des, const int16_t *root_table, uint32_t QQprime, int16_t *src, uint32_t RmodQ);


int main(void){

    hal_setup(CLOCK_FAST);

    hal_send_str("\n============ IGNORE OUTPUT BEFORE THIS LINE ============\n");

    int16_t poly1_int16[ARRAY_N];
    int16_t poly2_int16[ARRAY_N];
    int16_t res_int16[ARRAY_N];

    int32_t poly1_NTT_Q1Q2[ARRAY_N];
    int32_t poly2_NTT_Q1Q2[ARRAY_N];
    int32_t res_NTT_Q1Q2[ARRAY_N];

    int16_t poly1_NTT_Q1[ARRAY_N];
    int16_t poly1_NTT_Q2[ARRAY_N];
    int16_t poly2_NTT_Q1[ARRAY_N];
    int16_t poly2_NTT_Q2[ARRAY_N];
    int16_t res_NTT_Q1[ARRAY_N];
    int16_t res_NTT_Q2[ARRAY_N];

    int16_t res_NTT[ARRAY_N];

    int32_t table_int32[BUFF_MAX];
    int16_t table_int16[BUFF_MAX];

    int32_t mod_int32;
    int32_t omega_int32;
    int32_t scale_int32;

    int16_t tmp_int16;
    int16_t mod_int16;
    int16_t omega_int16;
    int16_t twiddle_int16;
    int16_t scale_int16;

// ================================
// generate polynomials and compute their product in C,
// the result will be used as the reference implementation for comparison

    for(size_t i = 0; i < ARRAY_N; i++){
        tmp_int16 = rand() % SABER_Q;
        poly1_int16[i] = tmp_int16 - (SABER_Q >> 1);
    }

    for(size_t i = 0; i < ARRAY_N; i++){
        tmp_int16 = rand() % SABER_MU;
        poly2_int16[i] = tmp_int16 - (SABER_MU >> 1);
    }

    mod_int16 = SABER_Q;
    twiddle_int16 = -1;
    naive_mulR(
        res_int16, poly1_int16, poly2_int16,
        ARRAY_N, &twiddle_int16, &mod_int16,
        sizeof(int16_t), addmod_int16, mulmod_int16
    );

// ================================
// first example

// ================
// call assembly for NTT

    NTT_forward_32(poly1_NTT_Q1Q2, poly1_int16);

// ================
// generate twiddle factors for NTT in C

    profile.compressed_layers = 2;
    profile.merged_layers[0] = 3;
    profile.merged_layers[1] = 3;

    // since we are using mulmod_int32 rather than Montgomery multiplication,
    // we set the scaling of twiddle factors scale_int32 as 1
    // one can also set scale_int32 as RmodQ1Q2 (so the table is exactly the one
    // used for assembly) and replace the mulmod_int32
    // with a function realizing Montgomery multiplication
    scale_int32 = 1;
    omega_int32 = omegaQ1Q2;
    mod_int32 = Q1Q2;
    gen_streamlined_CT_negacyclic_table_generic(
        table_int32,
        &scale_int32, &omega_int32,
        &mod_int32,
        sizeof(int32_t),
        mulmod_int32,
        &profile, 0
        );

// ================
// call the C function for NTT

    // since the C function is in-place, we first copy the 16-bit polynomial
    // poly2_int16 to the 32-bit array poly2_NTT_Q1Q2
    for(size_t i = 0; i < ARRAY_N; i++){
        poly2_NTT_Q1Q2[i] = poly2_int16[i];
    }

    // call the C function for NTT
    mod_int32 = Q1Q2;
    compressed_CT_NTT_generic(
        poly2_NTT_Q1Q2,
        0, 1,
        table_int32,
        &mod_int32,
        &profile,
        sizeof(int32_t),
        m_layer_CT_butterfly_int32
        );

// ================
// call assembly for base multiplication

    NTT_mul_32(res_NTT_Q1Q2, poly1_NTT_Q1Q2, poly2_NTT_Q1Q2);

// ================
// call assembly for iNTT

    NTT_inv_32(res_NTT, res_NTT_Q1Q2);

// ================
// test for correctness

    mod_int16 = SABER_Q;
    for(size_t i = 0; i < ARRAY_N; i++){
        cmod_int16(&tmp_int16, res_NTT + i, &mod_int16);
        if(res_int16[i] != tmp_int16){
            sprintf(out, "%4zu: %12d, %12d\n", i, res_int16[i], tmp_int16);
            hal_send_str(out);
        }
    }

    hal_send_str("polymul passed!\n");

// ================================
// second example

// ================
// call assembly for 32-bit NTT and transforming the result to 16-bit

    NTT_forward_32(poly1_NTT_Q1Q2, poly1_int16);
    MOD_1(poly1_NTT_Q1, poly1_NTT_Q1Q2);
    MOD_2(poly1_NTT_Q2, poly1_NTT_Q1Q2);

// ================
// generate twiddle factors used for 16-bit NTT

    profile.compressed_layers = 2;
    profile.merged_layers[0] = 3;
    profile.merged_layers[1] = 3;

    scale_int16 = RmodQ1;
    omega_int16 = omegaQ1;
    mod_int16 = Q1;
    gen_streamlined_CT_negacyclic_table_generic(
        table_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        &profile, 1
        );

    __asm_negacyclic_ntt_16(poly2_NTT_Q1, table_int16, Q1Q1prime, poly2_int16, RmodQ1);

// ================
// generate twiddle factors for 16-bit NTT over another prime

    profile.compressed_layers = 2;
    profile.merged_layers[0] = 3;
    profile.merged_layers[1] = 3;

    scale_int16 = RmodQ2;
    omega_int16 = omegaQ2;
    mod_int16 = Q2;
    gen_streamlined_CT_negacyclic_table_generic(
        table_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        &profile, 1
        );

// ================
// call assembly for the first three layers of NTT
// this assembly is not in our implementation, but it is involved in the development

    __asm_negacyclic_ntt_16_light_0_1_2(poly2_NTT_Q2, table_int16, Q2Q2prime, poly2_int16, RmodQ2);

// ================
// we can also switch to another strategy for merging layers
// as long as the computation is computing the right layer

    profile.compressed_layers = 4;
    profile.merged_layers[0] = 2;
    profile.merged_layers[1] = 1;
    profile.merged_layers[2] = 1;
    profile.merged_layers[3] = 2;

// ================
// generate twiddle factors for the NTT in C

    scale_int16 = 1;
    omega_int16 = omegaQ2;
    mod_int16 = Q2;
    gen_streamlined_CT_negacyclic_table_generic(
        table_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        &profile, 0
        );

// ================
// call the C function for NTT,
// note that we call the computation
// from layer sum_{i = 0}^{start_level -1} profile.merged_layers[i]
// to layer sum_{i = 0}^{end_level} profile.merged_layers[i] ) - 1
// where start_level = 2 and end_level = 3

    mod_int16 = Q2;
    compressed_CT_NTT_generic(
        poly2_NTT_Q2,
        2, 3,
        table_int16,
        &mod_int16,
        &profile,
        sizeof(int16_t),
        m_layer_CT_butterfly_int16
    );

// ================
// call assembly for base multiplications

    NTT_mul_16_1(res_NTT_Q1, poly1_NTT_Q1, poly2_NTT_Q1);
    NTT_mul_16_2(res_NTT_Q2, poly1_NTT_Q2, poly2_NTT_Q2);

// ================
// call assembly for solving CRT

    solv_CRT(res_NTT_Q1Q2, res_NTT_Q1, res_NTT_Q2);

// ================

    NTT_inv_32(res_NTT, res_NTT_Q1Q2);

// ================
// test for correctness

    mod_int16 = SABER_Q;
    for(size_t i = 0; i < ARRAY_N; i++){
        cmod_int16(&tmp_int16, res_NTT + i, &mod_int16);
        if(res_int16[i] != tmp_int16){
            sprintf(out, "%4zu: %12d, %12d\n", i, res_int16[i], tmp_int16);
            hal_send_str(out);
        }
    }

    hal_send_str("polymul passed!\n");


}

