
#include <stdlib.h>
#include <memory.h>

#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"

void gen_CT_table_generic(
    void *des, void *scale, void *omega, void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    void *zeta = (void*)malloc(size);
    void *twiddle = (void*)malloc(size);

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < (NTT_N >> 1); i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

    des -= size * (NTT_N >> 1);
    bitreverse_generic(des, NTT_N >> 1, size);

}

// let y = x^{ARRAY_N / NTT_N}

// generate twiddle factors for NTT over y^NTT_N - 1 with
// CT butterflies
void gen_CT_table(int *des, int scale, int omega, int Q){

    int zeta, factor;

    zeta = omega;

    factor = scale;
    for(int j = 0; j < (NTT_N >> 1); j++){
        *(des++) = factor;
        factor = center_mul(factor, zeta, Q);
    }

    bitreverse(des - (NTT_N >> 1), NTT_N >> 1);

}

void gen_CT_negacyclic_table_generic(
    void *des, void *scale, void *omega, void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    void *zeta = (void*)malloc(size);

    memcpy(zeta, omega, size);

    memcpy(des, scale, size);
    for(size_t i = 1; i < NTT_N; i++){
        mulmod(des + i * size, des + (i - 1) * size, zeta, mod);
    }

    bitreverse_generic(des, NTT_N, size);

    for(size_t i = 1; i < NTT_N; i++){
        memcpy(des + (i - 1) * size, des + i * size, size);
    }

}

// generate twiddle factors for NTT over y^NTT_N + 1 with
// CT butterflies
// void gen_CT_negacyclic_table(int *des, int scale, int omega, int Q){

//     int zeta;

//     zeta = omega;

//     des[0] = scale;
//     for(int j = 1; j < NTT_N; j++){
//         des[j] = center_mul(des[j - 1], zeta, Q);
//     }

//     bitreverse(des, NTT_N);

//     for(int j = 1; j < NTT_N; j++){
//         des[j - 1] = des[j];
//     }

// }

// generate twiddle factors for invserse NTT over y^NTT_N - 1 with
// CT butterflies
void gen_inv_CT_table(int *des, int scale, int omega, int Q){

    int zeta, inv_factor;

    for(int level = 0; level < LOGNTT_N; level++){
        zeta = expmod(omega, (1 << LOGNTT_N) >> (level + 1), Q);
        inv_factor = scale;
        for(int j = 0; j < (1 << level); j++){
            *(des++) = inv_factor;
            inv_factor = center_mul(inv_factor, zeta, Q);
        }
    }

}

void gen_streamlined_CT_table(int *des, int scale, int omega, int Q, struct compress_profile *_profile, int pad){

    int zeta, factor, start_level;
    int tmp[NTT_N >> 1];
    int *level_ptr[LOGNTT_N];

    zeta = omega;

    gen_CT_table(tmp, scale, zeta, Q);

    for(int i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp;
    }

    start_level = 0;
    for(int i = 0; i < _profile->compressed_layers; i++){
        for(int j = 0; j < (1 << start_level); j++){
            if(pad){
                *(des++) = 0;
            }
            for(int k = 0; k < (_profile->merged_layers[i]); k++){
                for(int h = 0; h < (1 << k); h++){
                    *(des++) = level_ptr[start_level + k][j * (1 << k) + h];
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}

void gen_streamlined_CT_negacyclic_table(
    int *des, int scale, int omega, int Q,
    struct compress_profile *_profile, int pad){

    int zeta, factor, start_level;
    int tmp[NTT_N];
    int *level_ptr[LOGNTT_N];

    int scale_v = scale;
    int Q_v = Q;

    zeta = omega;

    gen_CT_negacyclic_table_generic(
        (void*)tmp, (void*)&scale_v, (void*)&zeta, (void*)&Q_v,
        sizeof(int16_t),
        mulmod_int16);

    _16_to_32(tmp, NTT_N);


    for(int i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + ((1 << i) - 1);
    }

    start_level = 0;
    for(int i = 0; i < _profile->compressed_layers; i++){
        for(int j = 0; j < (1 << start_level); j++){
            if(pad){
               *(des++) = 0;
            }
            for(int k = 0; k < (_profile->merged_layers[i]); k++){
                for(int h = 0; h < (1 << k); h++){
                    *(des++) = level_ptr[start_level + k][j * (1 << k) + h];
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}

void gen_streamlined_inv_CT_table(int *des, int scale, int omega, int Q, struct compress_profile *_profile, int pad){

    int zeta, factor, start_level;
    int tmp[NTT_N];
    int *level_ptr[LOGNTT_N];

    zeta = omega;

    gen_inv_CT_table(tmp, scale, zeta, Q);

    for(int i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + ((1 << i) - 1);
    }

    start_level = 0;
    for(int i = 0; i < _profile->compressed_layers; i++){
        for(int j = 0; j < (1 << start_level); j++){
            if(pad){
                *(des++) = 0;
            }
            for(int k = 0; k < (_profile->merged_layers[i]); k++){
                for(int h = 0; h < (1 << k); h++){
                    *(des++) = level_ptr[start_level + k][j + (h << start_level)];
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}

void gen_streamlined_inv_CT_negacyclic_table(int *des, int scale1, int omega, int scale2, int twist_omega, int Q, struct compress_profile *_profile, int pad){

    int zeta, factor, start_level;
    int tmp[NTT_N], tmp2[NTT_N];
    int *level_ptr[LOGNTT_N], *twist_ptr;

    zeta = omega;

    gen_inv_CT_table(tmp, scale1, zeta, Q);

    for(int i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + ((1 << i) - 1);
    }

    zeta = twist_omega;

    gen_twist_table(tmp2, scale2, zeta, Q);

    twist_ptr = tmp2;

    start_level = 0;
    for(int i = 0; i < _profile->compressed_layers; i++){
        for(int j = 0; j < (1 << start_level); j++){
            if(pad){
                *(des++) = 0;
            }
            for(int k = 0; k < (_profile->merged_layers[i]); k++){
                for(int h = 0; h < (1 << k); h++){
                    *(des++) = level_ptr[start_level + k][j + (h << start_level)];
                }
            }
            if(i == ((_profile->compressed_layers) - 1)){
                for(int k = 0; k < (1 << (_profile->merged_layers[i])); k++){
                    *(des++) = twist_ptr[j + k * (NTT_N >> (_profile->merged_layers[i]))];
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}



// generate twiddle factors for
// twisting y^NTT_N - omega^NTT_N to y^NTT_N - 1;
// finally, the table is multiplied by scale
void gen_twist_table(int *des, int scale, int omega, int Q){

    int zeta, factor;

    zeta = omega;

    factor = scale;
    for(int j = 0; j < NTT_N; j++){
        *(des++) = factor;
        factor = center_mul(factor, zeta, Q);
    }

}

// generate twiddle factors for rings
// (y^{omega^0} - 1, y^{omega^0} + 1),
// (y^{omega^1} - 1, y^{omega^1} + 1),
// ...
// (y^{omega^{NTT_N - 1}} - 1, y^{omega^{NTT_N - 1}} + 1)
// in bit-reversed order;
// finally, the table is multiplied by scale
void gen_mul_table(int *des, int scale, int omega, int Q){

    int zeta, factor;

    zeta = omega;

    factor = scale;
    for(int j = 0; j < (NTT_N >> 1); j++){
        *(des++) = factor;
        factor = center_mul(factor, zeta, Q);
    }

    bitreverse(des - (NTT_N >> 1), NTT_N >> 1);

}

// generate twiddle factors for rings
// y^{omega^0} - 1,
// y^{omega^0} + 1,
// ...
// y^{omega^{NTT_N - 1}} - 1,
// y^{omega^{NTT_N - 1}} + 1,
// in bit-reversed order:
// finally, the table is multiplied by scale
void gen_all_mul_table(int *des, int scale, int omega, int Q){

    int tmp[NTT_N >> 1];

    gen_mul_table(tmp, scale, omega, Q);

    for(int j = 0; j < (NTT_N >> 1); j++){
        *(des++) = tmp[j];
        *(des++) = -tmp[j];
    }

}








