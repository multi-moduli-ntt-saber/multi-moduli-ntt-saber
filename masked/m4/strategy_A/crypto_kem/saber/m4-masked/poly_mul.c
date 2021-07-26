#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "poly_mul.h"
#include "NTT.h"

extern void __asm_poly_add_16(uint16_t *des, uint16_t *src1, uint16_t *src2);

void poly_mul_NTT(uint16_t res[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]){

    uint32_t ntt2_32[SABER_N];
    uint16_t ntt2_16[SABER_N];

    NTT_forward_32(ntt2_32, b);
    NTT_mul_32(ntt2_32, a_NTT_32, ntt2_32);
    NTT_inv_32(ntt2_32);

    NTT_forward_16(ntt2_16, b);
    NTT_mul_16(ntt2_16, a_NTT_16, ntt2_16);
    NTT_inv_16(ntt2_16);

    solv_CRT(res, ntt2_32, ntt2_16);

}

void poly_mul_acc_NTT(uint16_t res[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]){

    uint32_t ntt2_32[SABER_N];
    uint16_t ntt2_16[SABER_N];
    uint16_t *tmp = ntt2_16;

    NTT_forward_32(ntt2_32, b);
    NTT_mul_32(ntt2_32, a_NTT_32, ntt2_32);
    NTT_inv_32(ntt2_32);

    NTT_forward_16(ntt2_16, b);
    NTT_mul_16(ntt2_16, a_NTT_16, ntt2_16);
    NTT_inv_16(ntt2_16);

    solv_CRT(tmp, ntt2_32, ntt2_16);

    __asm_poly_add_16(res, res, tmp);

}

void poly_mul_NTT_C(uint16_t res_16[SABER_N], uint32_t res_32[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]){

    uint32_t ntt2_32[SABER_N];
    uint16_t ntt2_16[SABER_N];

    NTT_forward_32(ntt2_32, b);
    NTT_mul_32(res_32, a_NTT_32, ntt2_32);

    NTT_forward_16(ntt2_16, b);
    NTT_mul_16(res_16, a_NTT_16, ntt2_16);


}

void poly_mul_acc_NTT_C(uint16_t res_16[SABER_N], uint32_t res_32[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]){

    uint32_t ntt2_32[SABER_N];
    uint16_t ntt2_16[SABER_N];

    NTT_forward_32(ntt2_32, b);
    NTT_mul_acc_32(res_32, a_NTT_32, ntt2_32);

    NTT_forward_16(ntt2_16, b);
    NTT_mul_acc_16(res_16, a_NTT_16, ntt2_16);

}

void poly_mul(uint16_t a[SABER_N], uint16_t b[SABER_N], uint16_t res[SABER_N]){

    uint32_t ntt1_32[SABER_N], ntt2_32[SABER_N];

    uint16_t *ntt1_16 = (uint16_t*)ntt2_32;
    uint16_t *ntt2_16 = ntt1_16 + SABER_N;

    NTT_forward_32(ntt1_32, a);
    NTT_forward_32(ntt2_32, b);
    NTT_mul_32(ntt1_32, ntt1_32, ntt2_32);
    NTT_inv_32(ntt1_32);

    NTT_forward_16(ntt1_16, a);
    NTT_forward_16(ntt2_16, b);
    NTT_mul_16(ntt1_16, ntt1_16, ntt2_16);
    NTT_inv_16(ntt1_16);

    solv_CRT(res, ntt1_32, ntt1_16);

}

void poly_mul_acc(uint16_t a[SABER_N], uint16_t b[SABER_N], uint16_t res[SABER_N]){

    uint32_t ntt1_32[SABER_N], ntt2_32[SABER_N];

    uint16_t *ntt1_16 = (uint16_t*)ntt2_32;
    uint16_t *ntt2_16 = ntt1_16 + SABER_N;
    uint16_t *tmp = (uint16_t*)ntt2_16;

    NTT_forward_32(ntt1_32, a);
    NTT_forward_32(ntt2_32, b);
    NTT_mul_32(ntt1_32, ntt1_32, ntt2_32);
    NTT_inv_32(ntt1_32);

    NTT_forward_16(ntt1_16, a);
    NTT_forward_16(ntt2_16, b);
    NTT_mul_16(ntt1_16, ntt1_16, ntt2_16);
    NTT_inv_16(ntt1_16);

    solv_CRT(tmp, ntt1_32, ntt1_16);

    __asm_poly_add_16(res, res, tmp);

}


