#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "SABER_indcpa.h"
#include "SABER_params.h"
#include "pack_unpack.h"
#include "poly.h"
#include "NTT.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

extern void __asm_poly_add_16(uint16_t *des, uint16_t *src1, uint16_t *src2);
extern void __asm_poly_add_32(uint32_t *des, uint32_t *src1, uint32_t *src2);

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void){

    unsigned long long t0, t1;
    int i, j;
    int crypto_i;

    hal_setup(CLOCK_BENCHMARK);

    for(i = 0; i < 60; i++){
        hal_send_str("==========================");
    }

    for(crypto_i = 0; crypto_i < ITERATIONS; crypto_i++){

        uint32_t s_NTT[SABER_N];
        uint32_t acc_NTT[SABER_L * SABER_N];
        uint32_t A_NTT[SABER_N];
        uint16_t poly[SABER_N];
        uint16_t acc[SABER_L * SABER_N];

        // Don't change the order of buff1, buff2, and buff3
        uint16_t buff1[SABER_N];
        uint16_t buff2[SABER_N];
        uint16_t buff3[MAX(SABER_N, MAX(SABER_POLYCOINBYTES, SABER_POLYBYTES) / 2)];

        uint32_t *buff1_32 = (uint32_t*)buff1;
        uint32_t *buff2_32 = (uint32_t*)buff2;
        uint32_t *buff3_32 = (uint32_t*)buff3;

        t0 = hal_get_time();
        for(i = 0; i < SABER_L; i++){
            NTT_forward_32(s_NTT, poly);
            for(j = 0; j < SABER_L; j++){
                NTT_forward_32(A_NTT, poly);
                if(i == 0){
                    NTT_mul_32(acc_NTT + j * SABER_N, A_NTT, s_NTT);
                }else{
                    NTT_mul_acc_32(acc_NTT + j * SABER_N, A_NTT, s_NTT);
                }
            }
        }
        for (i = 0; i < SABER_L; i++) {
            NTT_inv_32(poly, acc_NTT + i * SABER_N);
        }
        t1 = hal_get_time();
        printcycles("MatrixVectorMul_A cycles:", t1 - t0);

        t0 = hal_get_time();
        for(i = 0; i < SABER_L; i++){
            for (j = 0; j < SABER_L; j++){
                NTT_forward_32(buff1_32, buff3);
                NTT_forward2(buff3_32, buff3);
                NTT_mul_32x16_2(buff3_32, buff1_32, buff3_32);
                MOD_1(buff1_32, buff1_32);
                NTT_forward1(buff2_32, buff2);
                NTT_mul_16_1(buff2_32, buff1_32, buff2_32);
                solv_CRT(buff1_32, buff2_32, buff3_32);
                if(i == 0){
                    NTT_inv_32(acc + j * SABER_N, buff1_32);
                }else{
                    NTT_inv_32(buff3, buff1_32);
                    __asm_poly_add_16(acc + j * SABER_N, acc + j * SABER_N, buff3);
                }

            }
        }
        t1 = hal_get_time();
        printcycles("MatrixVectorMul_D cycles:", t1 - t0);

        t0 = hal_get_time();
        for(j = 0; j < SABER_L; j++){
            NTT_forward_32(A_NTT, poly);
            if(j == 0){
                NTT_mul_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
            }else{
                NTT_mul_32(A_NTT, A_NTT, s_NTT + j * SABER_N);
                __asm_poly_add_32(acc_NTT, acc_NTT, A_NTT);
            }

        }
        NTT_inv_32(acc, acc_NTT);
        t1 = hal_get_time();
        printcycles("InnerProd (Encrypt) cycles:", t1 - t0);

        t0 = hal_get_time();
        for(j = 0; j < SABER_L; j++){
            NTT_forward_32(s_NTT + j * SABER_N, buff3);
            NTT_forward_32(A_NTT, poly);
            if(j == 0){
                NTT_mul_32(acc_NTT, A_NTT, s_NTT + j * SABER_N);
            }else{
                NTT_mul_32(A_NTT, A_NTT, s_NTT + j * SABER_N);
                __asm_poly_add_32(acc_NTT, acc_NTT, A_NTT);
            }
        }
        NTT_inv_32(acc, acc_NTT);
        t1 = hal_get_time();
        printcycles("InnerProd (Decrypt) cycles:", t1 - t0);

        t0 = hal_get_time();
        for(j = 0; j < SABER_L; j++){
            NTT_forward_32(buff1_32, buff3);
            NTT_forward2(buff3_32, buff3);
            NTT_mul_32x16_2(buff3_32, buff1_32, buff3_32);
            MOD_1(buff1_32, buff1_32);
            NTT_forward1(buff2_32, buff2);
            NTT_mul_16_1(buff2_32, buff1_32, buff2_32);
            solv_CRT(buff1_32, buff2_32, buff3_32);
            if(j == 0){
                NTT_inv_32(acc, buff1_32);
            }else{
                NTT_inv_32(buff3, buff1_32);
                __asm_poly_add_16(acc, acc, buff3);
            }
        }
        t1 = hal_get_time();
        printcycles("InnerProd_stack cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_forward_32(buff1_32, buff1);
        t1 = hal_get_time();
        printcycles("32-bit NTT forward cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_forward1(buff1_32, buff1);
        t1 = hal_get_time();
        printcycles("16-bit NTT forward cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_forward2(buff1_32, buff1);
        t1 = hal_get_time();
        printcycles("16-bit NTT forward light cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_inv_32(buff1, buff1_32);
        t1 = hal_get_time();
        printcycles("32-bit NTT inverse cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_32(buff1_32, buff1_32, buff1_32);
        t1 = hal_get_time();
        printcycles("32-bit base_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_acc_32(buff1_32, buff1_32, buff1_32);
        t1 = hal_get_time();
        printcycles("32-bit base_mul_acc cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_32x16_2(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();
        printcycles("32x16-bit base_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_16_1(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();
        printcycles("16-bit base_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        solv_CRT(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();
        printcycles("16x16 CRT cycles:", t1 - t0);

        t0 = hal_get_time();
        MOD_1(buff1_32, buff1_32);
        t1 = hal_get_time();
        printcycles("One mod cycles:", t1 - t0);


        hal_send_str("OK KEYS\n");

        hal_send_str("#");
    }

    while(1);
    return 0;
}
