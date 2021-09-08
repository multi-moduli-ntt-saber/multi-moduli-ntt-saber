#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"
#include "ntt_c.h"

#include <stdio.h>
#include <stdlib.h>

struct compress_profile profile;

int streamlined_CT_negacyclic_table_Q1Q2[NTT_N];
int streamlined_CT_negacyclic_table_Q1[(NTT_N - 1) + (1 << 0) + (1 << 3)];
int streamlined_CT_negacyclic_table_Q2[(NTT_N - 1) + (1 << 0) + (1 << 3)];

int mul_Rmod_table_Q1Q2[NTT_N >> 1];
int mul_Rmod_table_Q1[NTT_N >> 1];
int mul_Rmod_table_Q2[NTT_N >> 1];

int streamlined_inv_CT_negacyclic_table_Q1Q2[NTT_N << 1];
int streamlined_inv_CT_negacyclic_table_Q1[(NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N];
int streamlined_inv_CT_negacyclic_table_Q2[(NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N];

int streamlined_CT_negacyclic_table[NTT_N];
int negacyclic_mul_table[NTT_N >> 1];
int streamlined_inv_CT_table[NTT_N];
int inv_twist_table[NTT_N];

int main(){

    int poly1[3 * ARRAY_N];
    int poly2[3 * ARRAY_N];
    int polyout[3 * ARRAY_N];
    int flag;

// ================================

    for(int i = 0; i < 3 * ARRAY_N; i++){
        poly1[i] = center_mul(rand(), 1, Q1Q2);
        poly2[i] = center_mul(rand(), 1, Q1Q2);
    }

    naive_mulR(polyout + 0 * ARRAY_N, poly1 + 0 * ARRAY_N, poly2 + 0 * ARRAY_N, ARRAY_N, -1, Q1Q2);
    naive_mulR(polyout + 1 * ARRAY_N, poly1 + 1 * ARRAY_N, poly2 + 1 * ARRAY_N, ARRAY_N, -1, Q1Q2);
    naive_mulR(polyout + 2 * ARRAY_N, poly1 + 2 * ARRAY_N, poly2 + 2 * ARRAY_N, ARRAY_N, -1, Q1Q2);

    for(int i = 0; i < ARRAY_N; i++){
        polyout[i] = center_mul(polyout[0 * ARRAY_N + i] +
                                polyout[1 * ARRAY_N + i] +
                                polyout[2 * ARRAY_N + i],
                                1, Q1Q2);
    }


    profile.compressed_layers = 2;
    profile.merged_layers[0] = 3;
    profile.merged_layers[1] = 3;

    // gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q1Q2, RmodQ1Q2, omegaQ1Q2, Q1Q2, &profile, 0);
    // print_table(streamlined_CT_negacyclic_table_Q1Q2, NTT_N);

    gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table, 1, omegaQ1Q2, Q1Q2, &profile, 0);
    compressed_CT_NTT(poly1 + 0 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);
    compressed_CT_NTT(poly2 + 0 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);
    compressed_CT_NTT(poly1 + 1 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);
    compressed_CT_NTT(poly2 + 1 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);
    compressed_CT_NTT(poly1 + 2 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);
    compressed_CT_NTT(poly2 + 2 * ARRAY_N, 0, 1, streamlined_CT_negacyclic_table, Q1Q2, &profile);

    // gen_mul_table(mul_Rmod_table_Q1Q2, center_mul(RmodQ1Q2, omegaQ1Q2, Q1Q2), expmod(omegaQ1Q2, 2, Q1Q2), Q1Q2);
    // print_table(mul_Rmod_table_Q1Q2, NTT_N >> 1);

    gen_mul_table(negacyclic_mul_table, omegaQ1Q2, expmod(omegaQ1Q2, 2, Q1Q2), Q1Q2);
    school_book(poly1 + 0 * ARRAY_N, poly2 + 0 * ARRAY_N, negacyclic_mul_table, Q1Q2);
    school_book(poly1 + 1 * ARRAY_N, poly2 + 1 * ARRAY_N, negacyclic_mul_table, Q1Q2);
    school_book(poly1 + 2 * ARRAY_N, poly2 + 2 * ARRAY_N, negacyclic_mul_table, Q1Q2);

    for(int i = 0; i < ARRAY_N; i++){
        poly1[i] = center_mul(poly1[0 * ARRAY_N + i] +
                              poly1[1 * ARRAY_N + i] +
                              poly1[2 * ARRAY_N + i],
                              1, Q1Q2);
    }

    profile.compressed_layers = 2;
    profile.merged_layers[0] = 3;
    profile.merged_layers[1] = 3;

    // gen_streamlined_inv_CT_negacyclic_table(
    //     streamlined_inv_CT_negacyclic_table_Q1Q2,
    //     RmodQ1Q2, expmod(invomegaQ1Q2, 2, Q1Q2),
    //     center_mul(expmod(RmodQ1Q2, 2, Q1Q2), invNQ1Q2, Q1Q2), invomegaQ1Q2,
    //     Q1Q2, &profile, 0);
    // print_table(streamlined_inv_CT_negacyclic_table_Q1Q2, NTT_N << 1);

    gen_streamlined_inv_CT_table(streamlined_inv_CT_table, 1, expmod(invomegaQ1Q2, 2, Q1Q2), Q1Q2, &profile, 0);
    compressed_inv_CT_NTT(poly1 + 0 * ARRAY_N, 0, 1, streamlined_inv_CT_table, Q1Q2, &profile);
    // compressed_inv_CT_NTT(poly1 + 1 * ARRAY_N, 0, 1, streamlined_inv_CT_table, Q1Q2, &profile);
    // compressed_inv_CT_NTT(poly1 + 2 * ARRAY_N, 0, 1, streamlined_inv_CT_table, Q1Q2, &profile);


    gen_twist_table(inv_twist_table, 1, invomegaQ1Q2, Q1Q2);
    point_mul(poly1 + 0 * ARRAY_N, inv_twist_table, NTT_N, ARRAY_N >> LOGNTT_N, Q1Q2);
    // point_mul(poly1 + 1 * ARRAY_N, inv_twist_table, NTT_N, ARRAY_N >> LOGNTT_N, Q1Q2);
    // point_mul(poly1 + 2 * ARRAY_N, inv_twist_table, NTT_N, ARRAY_N >> LOGNTT_N, Q1Q2);


    for(int i = 0; i < ARRAY_N; i++){
        poly1[0 * ARRAY_N + i] = center_mul(poly1[0 * ARRAY_N + i], invNQ1Q2, Q1Q2);
        // poly1[1 * ARRAY_N + i] = center_mul(poly1[1 * ARRAY_N + i], invNQ1Q2, Q1Q2);
        // poly1[2 * ARRAY_N + i] = center_mul(poly1[2 * ARRAY_N + i], invNQ1Q2, Q1Q2);

    }

    flag = 0;
    for(int i = 0; i < ARRAY_N; i++){
        if(polyout[i] != poly1[i]){
            flag = 1;
            printf("%4d: %12d, %12d\n", i, polyout[i], poly1[i]);
        }
    }

    if(flag){
        printf("Test failed\n");
    }else{
        printf("Test successful\n");
    }

// ================================

    // for(int i = 0; i < ARRAY_N; i++){
    //     poly1[i] = center_mul(rand(), 1, Q1);
    //     poly2[i] = center_mul(rand(), 1, Q1);
    // }

    // naive_mulR(polyout, poly1, poly2, ARRAY_N, -1, Q1);

    // profile.compressed_layers = 2;
    // profile.merged_layers[0] = 3;
    // profile.merged_layers[1] = 3;

    // gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q1, RmodQ1, omegaQ1, Q1, &profile, 1);
    // print_table(streamlined_CT_negacyclic_table_Q1, (NTT_N - 1) + (1 << 0) + (1 << 3));

    // gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table, 1, omegaQ1, Q1, &profile, 0);
    // compressed_CT_NTT(poly1, 0, 1, streamlined_CT_negacyclic_table, Q1, &profile);
    // compressed_CT_NTT(poly2, 0, 1, streamlined_CT_negacyclic_table, Q1, &profile);

    // gen_mul_table(mul_Rmod_table_Q1, center_mul(RmodQ1, omegaQ1, Q1), expmod(omegaQ1, 2, Q1), Q1);
    // print_table(mul_Rmod_table_Q1, NTT_N >> 1);

    // gen_mul_table(negacyclic_mul_table, omegaQ1, expmod(omegaQ1, 2, Q1), Q1);
    // school_book(poly1, poly2, negacyclic_mul_table, Q1);

    // profile.compressed_layers = 2;
    // profile.merged_layers[0] = 3;
    // profile.merged_layers[1] = 3;

    // gen_streamlined_inv_CT_negacyclic_table(
    //     streamlined_inv_CT_negacyclic_table_Q1,
    //     RmodQ1, expmod(invomegaQ1, 2, Q1),
    //     center_mul(expmod(RmodQ1, 2, Q1), invNQ1, Q1), invomegaQ1,
    //     Q1, &profile, 1);
    // print_table(streamlined_inv_CT_negacyclic_table_Q1, (NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N);

    // gen_streamlined_inv_CT_table(streamlined_inv_CT_table, 1, expmod(invomegaQ1, 2, Q1), Q1, &profile, 0);
    // compressed_inv_CT_NTT(poly1, 0, 1, streamlined_inv_CT_table, Q1, &profile);

    // gen_twist_table(inv_twist_table, 1, invomegaQ1, Q1);
    // point_mul(poly1, inv_twist_table, NTT_N, ARRAY_N >> LOGNTT_N, Q1);

    // for(int i = 0; i < ARRAY_N; i++){
    //     poly1[i] = center_mul(poly1[i], invNQ1, Q1);
    // }

    // flag = 0;
    // for(int i = 0; i < ARRAY_N; i++){
    //     if(polyout[i] != poly1[i]){
    //         flag = 1;
    //         printf("%4d: %12d, %12d\n", i, polyout[i], poly1[i]);
    //     }
    // }

    // if(flag){
    //     printf("Test failed\n");
    // }else{
    //     printf("Test successful\n");
    // }

// ================================

    // for(int i = 0; i < ARRAY_N; i++){
    //     poly1[i] = center_mul(rand(), 1, Q2);
    //     poly2[i] = center_mul(rand(), 1, Q2);
    // }

    // naive_mulR(polyout, poly1, poly2, ARRAY_N, -1, Q2);

    // profile.compressed_layers = 2;
    // profile.merged_layers[0] = 3;
    // profile.merged_layers[1] = 3;

    // gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q2, RmodQ2, omegaQ2, Q2, &profile, 1);
    // print_table(streamlined_CT_negacyclic_table_Q2, (NTT_N - 1) + (1 << 0) + (1 << 3));

    // gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table, 1, omegaQ2, Q2, &profile, 0);
    // compressed_CT_NTT(poly1, 0, 1, streamlined_CT_negacyclic_table, Q2, &profile);
    // compressed_CT_NTT(poly2, 0, 1, streamlined_CT_negacyclic_table, Q2, &profile);

    // gen_mul_table(mul_Rmod_table_Q2, center_mul(RmodQ2, omegaQ2, Q2), expmod(omegaQ2, 2, Q2), Q2);
    // print_table(mul_Rmod_table_Q2, NTT_N >> 1);

    // gen_mul_table(negacyclic_mul_table, omegaQ2, expmod(omegaQ2, 2, Q2), Q2);
    // school_book(poly1, poly2, negacyclic_mul_table, Q2);

    // profile.compressed_layers = 2;
    // profile.merged_layers[0] = 3;
    // profile.merged_layers[1] = 3;

    // gen_streamlined_inv_CT_negacyclic_table(
    //     streamlined_inv_CT_negacyclic_table_Q2,
    //     RmodQ2, expmod(invomegaQ2, 2, Q2),
    //     center_mul(expmod(RmodQ2, 2, Q2), invNQ2, Q2), invomegaQ2,
    //     Q2, &profile, 1);
    // print_table(streamlined_inv_CT_negacyclic_table_Q2, (NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N);

    // gen_streamlined_inv_CT_table(streamlined_inv_CT_table, 1, expmod(invomegaQ2, 2, Q2), Q2, &profile, 0);
    // compressed_inv_CT_NTT(poly1, 0, 1, streamlined_inv_CT_table, Q2, &profile);

    // gen_twist_table(inv_twist_table, 1, invomegaQ2, Q2);
    // point_mul(poly1, inv_twist_table, NTT_N, ARRAY_N >> LOGNTT_N, Q2);

    // for(int i = 0; i < ARRAY_N; i++){
    //     poly1[i] = center_mul(poly1[i], invNQ2, Q2);
    // }

    // flag = 0;
    // for(int i = 0; i < ARRAY_N; i++){
    //     if(polyout[i] != poly1[i]){
    //         flag = 1;
    //         printf("%4d: %12d, %12d\n", i, polyout[i], poly1[i]);
    //     }
    // }

    // if(flag){
    //     printf("Test failed\n");
    // }else{
    //     printf("Test successful\n");
    // }

}











