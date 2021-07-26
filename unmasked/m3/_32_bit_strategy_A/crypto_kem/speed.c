#include "api.h"
#include "hal.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "poly.h"
#include "SABER_indcpa.h"
#include "SABER_params.h"
#include "pack_unpack.h"

#include "NTT.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void)
{
  unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
  unsigned long long t0, t1;
  int j, k;
  int crypto_i;

  hal_setup(CLOCK_BENCHMARK);

  hal_send_str("==========================");

  for (crypto_i = 0; crypto_i < ITERATIONS; crypto_i++)
  {
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

    //


    int32_t s_NTT[SABER_N];
    int32_t acc_NTT[SABER_L * SABER_N];
    int32_t A_NTT[SABER_N];

    t0 = hal_get_time();
    for (j = 0; j < SABER_L; j++)
    {

      ntt_asm_const(s_NTT, twiddles);

      for (k = 0; k < SABER_L; k++)
      {

        ntt_asm_leak(A_NTT, twiddles_reord);

        if (j == 0)
        {
          NTT_mul(acc_NTT + k * SABER_N, s_NTT, A_NTT);
        }
        else
        {
          NTT_mul_acc(acc_NTT + k * SABER_N, s_NTT, A_NTT);
        }
      }
    }

    for (j = 0; j < SABER_L; j++)
    {
      intt_asm_const(acc_NTT + j * SABER_N, twiddles_inv_RinvN);
    }
    t1 = hal_get_time();
    printcycles("32-bit MatrixVectorMul cycles:", t1 - t0);

    t0 = hal_get_time();
    for (j = 0; j < SABER_L; j++)
    {

      ntt_asm_leak(A_NTT, twiddles_reord);

      if (j == 0)
      {
        NTT_mul(acc_NTT, s_NTT, A_NTT);
      }
      else
      {
        NTT_mul_acc(acc_NTT, s_NTT, A_NTT);
      }
    }

    intt_asm_const(acc_NTT, twiddles_inv_RinvN);

    t1 = hal_get_time();
    printcycles("32-bit InnderProd(Encrypt) cycles:", t1 - t0);

    t0 = hal_get_time();
    for (j = 0; j < SABER_L; j++)
    {

      ntt_asm_leak(A_NTT, twiddles);

      ntt_asm_const(s_NTT, twiddles_reord);

      if (j == 0)
      {
        NTT_mul(acc_NTT, A_NTT, s_NTT);
      }
      else
      {
        NTT_mul_acc(acc_NTT, A_NTT, s_NTT);
      }
    }

    intt_asm_const(acc_NTT, twiddles_inv_RinvN);

    t1 = hal_get_time();
    printcycles("32-bit InnderProd(Decrypt) cycles:", t1 - t0);

    t0 = hal_get_time();
    ntt_asm_const(s_NTT, twiddles);
    t1 = hal_get_time();
    printcycles("32-bit NTT cycles:", t1 - t0);

    t0 = hal_get_time();
    ntt_asm_leak(s_NTT, twiddles_reord);
    t1 = hal_get_time();
    printcycles("32-bit NTT_leak cycles:", t1 - t0);

    t0 = hal_get_time();
    intt_asm_const(s_NTT, twiddles_inv_RinvN);
    t1 = hal_get_time();
    printcycles("32-bit iNTT cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_mul(s_NTT, A_NTT, s_NTT);
    t1 = hal_get_time();
    printcycles("32-bit basemul cycles:", t1 - t0);

    if (memcmp(key_a, key_b, CRYPTO_BYTES))
    {
      hal_send_str("ERROR KEYS\n");
    }
    else
    {
      hal_send_str("OK KEYS\n");
    }

    hal_send_str("#");

  }




  return 0;
}
