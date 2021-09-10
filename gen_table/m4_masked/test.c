

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"

#define BUFF_MAX (NTT_N << 3)

struct compress_profile profile;

// ================

static const int32_t streamlined_CT_negacyclic_table_Q1[NTT_N] = {
-17815616, 20179042, 5173450, -10771126, 16264029, 7771222, 22209164, 20168288, 12843351, -21065524, 9549694, -18485124, 13730545, -3408104, -12756821, -2111760, 9083870, 12322149, 17357617, 15860553, 22071560, 10680947, 528447, -18818674, -7231958, 14373826, -3110586, -13073414, 12881717, 17638719, -15509723, -10953473, -16918429, -19802953, 16381402, 3052338, -9581747, 8857767, 17005820, -16701106, -20029869, 21170404, 16873658, 18074717, -11294276, 18712276, -5233444, -14669727, -14263517, 18655939, 19085761, 1633657, 13490703, -4090736, 7989938, -16270819, 3219765, 742202, 2078156, 12706786, 19999052, 21125353, 406480, 0
};

static const int16_t streamlined_CT_negacyclic_table_Q2[(NTT_N - 1) + (1 << 0) + (1 << 3)] = {
0, -164, -81, 361, 186, -3, -250, -120, 0, -308, -76, -98, 147, -114, -272, 54, 0, 129, 36, -75, -2, -124, -80, -346, 0, -16, -339, -255, 86, -51, 364, 267, 0, -223, 282, -203, 161, -15, 288, 169, 0, -362, -34, 199, 191, 307, -50, -24, 0, -143, 178, 270, -170, 226, 121, -188, 0, 131, -10, 149, -380, 279, 180, -375, 0, -337, 369, -192, -157, 263, -128, -246
};

// ================

static const int32_t streamlined_inv_CT_negacyclic_table_Q1[NTT_N << 1] = {
5361568, 5361568, 17815616, 5361568, -5173450, 17815616, -20179042, 5361568, 5361568, 17815616, 5361568, -5173450, 17815616, -20179042, 5211980, 10191609, -18958318, -2536408, -20370394, 6798571, 17653340, 9256845, -22209164, -3219765, -18655939, -2078156, -1633657, -742202, -19085761, -3913294, -9518846, -6761239, -22261278, -13080044, 4166159, -12172318, -22181021, -5173450, -22209164, -7771222, -3219765, -16873658, -18655939, -3052338, -9980694, 1477987, -15907284, -20235687, 21681607, 8100971, -16574872, -8749497, -16264029, -12881717, -10680947, 15509723, 18818674, -17638719, -528447, 17287360, -15924680, -1954003, 12671720, -187509, -5761119, -18776506, -8460216, 17815616, -5173450, -20179042, -22209164, -16264029, -7771222, 10771126, -2901148, 11565223, 21558827, -18751213, 21416902, 16292070, 14968358, -19786940, -7771222, -16873658, -3052338, 11294276, -8857767, -18074717, 9581747, 18051007, -11458335, -4297738, 402981, -654814, 19144908, 15910857, -13448571, -20179042, -16264029, 10771126, -12881717, 12756821, -10680947, -20168288, -6753887, 6198868, 5533187, -16866197, -17400052, 10085233, -11482435, 8453394, 10771126, 12756821, -20168288, -9083870, 21065524, 2111760, -12843351, 9582513, -14672722, -16368664, -10330663, 17818646, -7297800, 16215382, -9371731, 0
};

static const int16_t streamlined_inv_CT_negacyclic_table_Q2[(NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N] = {
0, 171, 171, 164, 171, -361, 164, 81, 0, 171, 171, 164, 171, -361, 164, 81, -228, -160, 225, -4, 294, -77, -108, 248, 0, 120, 337, -131, 192, -149, -369, 10, -328, 269, -162, 372, 342, 240, 47, 6, 0, -361, 120, 250, 337, 143, -131, 362, -256, 360, -314, 9, -277, -19, 243, 211, 0, 3, 223, 16, 203, 255, -282, 339, -31, 356, -20, -68, 384, 229, -298, 371, 0, 164, -361, 81, 120, 3, 250, -186, -193, -41, 322, 172, -338, 235, 30, 102, 0, 250, 143, 362, -270, -199, -178, 34, 262, -32, 45, 153, -95, -323, 286, -258, 0, 81, 3, -186, 223, -129, 16, 308, 242, -100, -340, 382, 376, 48, 317, 155, 0, -186, -129, 308, 75, 98, -36, 76, -205, 72, 91, -152, -363, 150, -259, 196
};

// ================

static const int32_t mul_Rmod_table_Q1[NTT_N >> 1] = {
9549694, -18485124, 13730545, -3408104, 12322149, 17357617, 15860553, 22071560, -7231958, 14373826, -3110586, -13073414, -10953473, -16918429, -19802953, 16381402, 17005820, -16701106, -20029869, 21170404, 18712276, -5233444, -14669727, -14263517, 13490703, -4090736, 7989938, -16270819, 12706786, 19999052, 21125353, 406480
};

static const int16_t mul_Rmod_table_Q2[NTT_N >> 1] = {
147, -114, -272, 54, -2, -124, -80, -346, 86, -51, 364, 267, 161, -15, 288, 169, 191, 307, -50, -24, -170, 226, 121, -188, -380, 279, 180, -375, -157, 263, -128, -246
};

// ================


int main(){

    profile.compressed_layers = 2;
    for(size_t i = 0; i < profile.compressed_layers; i++){
        profile.merged_layers[i]= 3;
    }

    int32_t twiddle_int32[BUFF_MAX];
    int16_t twiddle_int16[BUFF_MAX];

    int32_t scale_int32;
    int32_t omega_int32;
    int32_t mod_int32;
    int32_t scale2_int32;
    int32_t twist_omega_int32;


    int16_t scale_int16;
    int16_t omega_int16;
    int16_t mod_int16;
    int16_t scale2_int16;
    int16_t twist_omega_int16;

// ================

// ================

    scale_int32 = RmodQ1;
    omega_int32 = omegaQ1;
    mod_int32 = Q1;
    gen_streamlined_CT_negacyclic_table_generic(
        twiddle_int32,
        &scale_int32, &omega_int32,
        &mod_int32,
        sizeof(int32_t),
        mulmod_int32,
        &profile, 0
    );

    for(int i = 0; i < (NTT_N - 1); i++){
        assert(streamlined_CT_negacyclic_table_Q1[i] == twiddle_int32[i]);
    }

// ================

// ================

    scale_int16 = RmodQ2;
    omega_int16 = omegaQ2;
    mod_int16 = Q2;
    gen_streamlined_CT_negacyclic_table_generic(
        twiddle_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        &profile, 1
    );

    for(int i = 0; i < (NTT_N - 1) + (1 << 0) + (1 << 3); i++){
        assert(streamlined_CT_negacyclic_table_Q2[i] == twiddle_int16[i]);
    }

// ================

// ================

    scale_int32 = RmodQ1;
    scale2_int32 = invNQ1;
    twist_omega_int32 = invomegaQ1;
    mod_int32 = Q1;
    expmod_int32(&omega_int32, &twist_omega_int32, 2, &mod_int32);
    mulmod_int32(&scale2_int32, &scale2_int32, &scale_int32, &mod_int32);
    mulmod_int32(&scale2_int32, &scale2_int32, &scale_int32, &mod_int32);
    gen_streamlined_inv_CT_negacyclic_table_generic(
        twiddle_int32,
        &scale_int32, &omega_int32,
        &scale2_int32, &twist_omega_int32,
        &mod_int32,
        sizeof(int32_t),
        mulmod_int32,
        expmod_int32,
        &profile, 0
    );

    for(int i = 0; i < (NTT_N << 1) - 1; i++){
        assert(streamlined_inv_CT_negacyclic_table_Q1[i] == twiddle_int32[i]);
    }

// ================

// ================

    scale_int16 = RmodQ2;
    scale2_int16 = invNQ2;
    twist_omega_int16 = invomegaQ2;
    mod_int16 = Q2;
    expmod_int16(&omega_int16, &twist_omega_int16, 2, &mod_int16);
    mulmod_int16(&scale2_int16, &scale2_int16, &scale_int16, &mod_int16);
    mulmod_int16(&scale2_int16, &scale2_int16, &scale_int16, &mod_int16);
    gen_streamlined_inv_CT_negacyclic_table_generic(
        twiddle_int16,
        &scale_int16, &omega_int16,
        &scale2_int16, &twist_omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        expmod_int16,
        &profile, 1
    );

    for(int i = 0; i < (NTT_N - 1) + (1 << 0) + (1 << 3) + NTT_N; i++){
        assert(streamlined_inv_CT_negacyclic_table_Q2[i] == twiddle_int16[i]);
    }

// ================

// ================

    scale_int32 = RmodQ1;
    omega_int32 = omegaQ1;
    mod_int32 = Q1;
    mulmod_int32(&scale_int32, &scale_int32, &omega_int32, &mod_int32);
    expmod_int32(&omega_int32, &omega_int32, 2, &mod_int32);
    gen_mul_table_generic(
        twiddle_int32,
        &scale_int32, &omega_int32,
        &mod_int32,
        sizeof(int32_t),
        mulmod_int32
        );

    for(int i = 0; i < (NTT_N >> 1); i++){
        assert(mul_Rmod_table_Q1[i] == twiddle_int32[i]);
    }

// ================

// ================

    scale_int16 = RmodQ2;
    omega_int16 = omegaQ2;
    mod_int16 = Q2;
    mulmod_int16(&scale_int16, &scale_int16, &omega_int16, &mod_int16);
    expmod_int16(&omega_int16, &omega_int16, 2, &mod_int16);
    gen_mul_table_generic(
        twiddle_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16
        );

    for(int i = 0; i < (NTT_N >> 1); i++){
        assert(mul_Rmod_table_Q2[i] == twiddle_int16[i]);
    }

    printf("We are all good!\n");

}












