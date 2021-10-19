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

    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];

    unsigned long long t0, t1;
    int i, j;
    int crypto_i;

    hal_setup(CLOCK_BENCHMARK);

    for(i = 0; i < 50; i++){
        hal_send_str("==========================");
    }

    for(crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++){

        memset(key_a, 0, sizeof key_a);
        memset(key_b, 0, sizeof key_a);
        memset(sk, 0, sizeof sk);
        memset(pk, 0, sizeof pk);
        memset(ct, 0, sizeof ct);

        // Key-pair generation
        t0 = hal_get_time();
        crypto_kem_keypair(pk, sk);
        t1 = hal_get_time();
        printcycles("keypair cycles:", t1 - t0);

        // Encapsulation
        t0 = hal_get_time();
        crypto_kem_enc(ct, key_a, pk);
        t1 = hal_get_time();
        printcycles("encaps cycles:", t1 - t0);

        // Decapsulation
        t0 = hal_get_time();
        crypto_kem_dec(key_b, ct, sk);
        t1 = hal_get_time();
        printcycles("decaps cycles:", t1 - t0);

        if(memcmp(key_a, key_b, CRYPTO_BYTES)) {
            hal_send_str("ERROR KEYS\n");
        }
        else{
            hal_send_str("OK KEYS\n");
        }


// arrays for speed opt

        uint32_t s_NTT[SABER_N];
        uint32_t acc_NTT[SABER_L * SABER_N];
        uint32_t A_NTT[SABER_N];

        uint16_t poly[SABER_N];

// arrays for stack opt

        uint16_t acc[SABER_L * SABER_N];

        // Black magic here, don't change the order of buff1, buff2, and buff3
        uint16_t buff1[SABER_N];
        uint16_t buff2[SABER_N];
        uint16_t buff3[MAX(SABER_N, MAX(SABER_POLYCOINBYTES, SABER_POLYBYTES) / 2)];

        uint32_t *buff1_32 = (uint32_t*)buff1;
        uint32_t *buff2_32 = (uint32_t*)buff2;
        uint32_t *buff3_32 = (uint32_t*)buff3;


// benchmarking NTT functions

        hal_send_str("\nCycles for NTT-related functions\n");

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

        hal_send_str("\nCycles for base_mul-related functions\n");

        t0 = hal_get_time();
        NTT_mul_32(buff1_32, buff1_32, buff1_32);
        t1 = hal_get_time();

        printcycles("32-bit base_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_32x16_2(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();

        printcycles("32x16-bit base_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        NTT_mul_16_1(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();

        printcycles("16-bit base_mul cycles:", t1 - t0);

// benchmarking auxiliary functions

        hal_send_str("\nCycles for auxiliary functions\n");

        t0 = hal_get_time();
        solv_CRT(buff1_32, buff1_32, buff2_32);
        t1 = hal_get_time();

        printcycles("16x16 CRT cycles:", t1 - t0);

        t0 = hal_get_time();
        MOD_1(buff1_32, buff1_32);
        t1 = hal_get_time();

        printcycles("One mod cycles:", t1 - t0);

// benchmarking {speed, stack}-opt MatrixVectorMul

        hal_send_str("\nCycles for {speed, stack}-opt MatrixVectorMul\n");

// benchmarking speed-opt MatrixVectorMul

        t0 = hal_get_time();

        for(i = 0; i < SABER_L; i++){

            NTT_forward_32(s_NTT, poly);

            for(j = 0; j < SABER_L; j++){

                NTT_forward_32(A_NTT, poly);

                if(i == 0){
                    NTT_mul_32(acc_NTT + j * SABER_N, A_NTT, s_NTT);
                }else{
                    NTT_mul_32(A_NTT, A_NTT, s_NTT);
                    __asm_poly_add_32(acc_NTT + j * SABER_N, acc_NTT + j * SABER_N, A_NTT);
                }

            }
        }

        for (i = 0; i < SABER_L; i++) {
            NTT_inv_32(poly, acc_NTT + i * SABER_N);
        }

        t1 = hal_get_time();

        printcycles("MatrixVectorMul speed opt cycles:", t1 - t0);

// benchmarking stack-opt MatrixVectorMul

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

        printcycles("MatrixVectorMul stack opt cycles:", t1 - t0);

// benchmarking {speed, stack}-opt InnerProd

        hal_send_str("\nCycles for {speed, stack}-opt InnerProd\n");

// benchmarking speed-opt InnerProd(Encrypt)

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

        printcycles("InnerProd(Encrypt) speed opt cycles:", t1 - t0);

// benchmarking speed-opt InnerProd(Decrypt)

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

        printcycles("InnerProd(Decrypt) speed opt cycles:", t1 - t0);

// benchmarking stack-opt InnerProd

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

        printcycles("InnerProd stack opt cycles:", t1 - t0);


        hal_send_str("#");
    }

    while(1);
    return 0;
}
