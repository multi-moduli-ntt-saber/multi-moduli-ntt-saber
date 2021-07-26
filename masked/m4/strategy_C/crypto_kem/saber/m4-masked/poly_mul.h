#ifndef POLYMUL_H
#define POLYMUL_H

#include "SABER_params.h"

#define NTT

void poly_mul_NTT(uint16_t res[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]);
void poly_mul_acc_NTT(uint16_t res[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]);

void poly_mul_NTT_C(uint16_t res_16[SABER_N], uint32_t res_32[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]);
void poly_mul_acc_NTT_C(uint16_t res_16[SABER_N], uint32_t res_32[SABER_N], uint32_t a_NTT_32[SABER_N], uint16_t a_NTT_16[SABER_N], uint16_t b[SABER_N]);


void poly_mul(uint16_t a[SABER_N], uint16_t b[SABER_N], uint16_t res[SABER_N]);
void poly_mul_acc(uint16_t a[SABER_N], uint16_t b[SABER_N], uint16_t res[SABER_N]);

#endif
