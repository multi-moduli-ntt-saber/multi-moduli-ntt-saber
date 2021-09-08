#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"
#include "ntt_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct compress_profile profile;

// gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q1Q2, RmodQ1Q2, omegaQ1Q2, Q1Q2, &profile, 0);
// print_table(streamlined_CT_negacyclic_table_Q1Q2, NTT_N);
int32_t streamlined_CT_negacyclic_table_Q1Q2[NTT_N];

// gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q1, RmodQ1, omegaQ1, Q1, &profile, 1);
// print_table(streamlined_CT_negacyclic_table_Q1, (NTT_N - 1) + (1 << 0) + (1 << 3));
int16_t streamlined_CT_negacyclic_table_Q1[((NTT_N - 1) / 7) << 3];

// gen_streamlined_CT_negacyclic_table(streamlined_CT_negacyclic_table_Q2, RmodQ2, omegaQ2, Q2, &profile, 1);
// print_table(streamlined_CT_negacyclic_table_Q2, (NTT_N - 1) + (1 << 0) + (1 << 3));
int16_t streamlined_CT_negacyclic_table_Q2[((NTT_N - 1) / 7) << 3];

// gen_mul_table(mul_Rmod_table_Q1Q2, center_mul(omegaQ1Q2, RmodQ1Q2, Q1Q2), expmod(omegaQ1Q2, 2, Q1Q2), Q1Q2);
// print_table(mul_Rmod_table_Q1Q2, NTT_N >> 1);
int32_t mul_Rmod_table_Q1Q2[NTT_N >> 1];

// gen_mul_table(mul_Rmod_table_Q1, center_mul(omegaQ1, RmodQ1, Q1), expmod(omegaQ1, 2, Q1), Q1);
// print_table(mul_Rmod_table_Q1, NTT_N >> 1);
int16_t mul_Rmod_table_Q1[NTT_N >> 1];

// gen_mul_table(mul_Rmod_table_Q2, center_mul(omegaQ2, RmodQ2, Q2), expmod(omegaQ2, 2, Q2), Q2);
// print_table(mul_Rmod_table_Q2, NTT_N >> 1);
int16_t mul_Rmod_table_Q2[NTT_N >> 1];

// gen_streamlined_inv_CT_negacyclic_table(streamlined_inv_CT_negacyclic_table_Q1Q2, RmodQ1Q2, expmod(invomegaQ1Q2, 2, Q1Q2), center_mul(expmod(RmodQ1Q2, 2, Q1Q2), invNQ1Q2, Q1Q2), invomegaQ1Q2, Q1Q2, &profile, 0);
// print_table(streamlined_inv_CT_negacyclic_table_Q1Q2, NTT_N << 1);
int32_t streamlined_inv_CT_negacyclic_table_Q1Q2[NTT_N << 1];

int main(){

    int flag;

    int int_array[NTT_N >> 1];
    int16_t int16_array[NTT_N];
    int16_t scale_int16;
    int16_t omega_int16;
    int16_t mod_int16;

    gen_CT_table(int_array, 1, omegaQ1, Q1);

    scale_int16 = 1;
    omega_int16 = omegaQ1;
    mod_int16 = Q1;
    gen_CT_table_generic(int16_array,
        (void*)&scale_int16, (void*)&omega_int16, (void*)&mod_int16,
        sizeof(int16_t),
        mulmod_int16);

    flag = 0;
    for(int i = 0; i < (NTT_N >> 1); i++){
        if(int_array[i] != int16_array[i]){
            flag = 1;
            printf("%d: %d, %d\n", i, int_array[i], int16_array[i]);
            printf("Broken generic construction!\n");
        }
    }

    if(flag == 0){
        printf("Awesome!\n");
    }

}












