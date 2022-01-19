#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"
#include "NTT.h"
#include "poly_mul.h"
#include "pack_unpack.h"

#include <stdint.h>
#include <string.h>

extern void ClearStack(size_t len);

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void)
{
  unsigned long long t0, t1;

  hal_setup(CLOCK_BENCHMARK);

  int i;
  int crypto_i;

  for(i = 0; i < 60; i++){
    hal_send_str("==========================");
  }

  // declare arrays

  uint32_t s_NTT_32[SABER_SHARES][SABER_L][SABER_N];
  uint16_t s_NTT_16[SABER_SHARES][SABER_L][SABER_N];

// benchmark for individual functions

  for(crypto_i = 0; crypto_i < ITERATIONS; crypto_i++){

    t0 = hal_get_time();
    NTT_forward_32(s_NTT_32[0][0], s_NTT_16[0][0]);
    t1 = hal_get_time();
    printcycles("32-bit NTT cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_mul_32(s_NTT_32[0][0], s_NTT_32[0][0], s_NTT_32[0][0]);
    t1 = hal_get_time();
    printcycles("32-bit NTT_mul cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_inv_32(s_NTT_32[0][0]);
    t1 = hal_get_time();
    printcycles("32-bit NTT_inv cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_forward_16(s_NTT_16[0][0], s_NTT_16[0][0]);
    t1 = hal_get_time();
    printcycles("16-bit NTT cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_mul_16(s_NTT_16[0][0], s_NTT_16[0][0], s_NTT_16[0][0]);
    t1 = hal_get_time();
    printcycles("16-bit NTT_mul cycles:", t1 - t0);

    t0 = hal_get_time();
    NTT_inv_16(s_NTT_16[0][0]);
    t1 = hal_get_time();
    printcycles("16-bit NTT_inv cycles:", t1 - t0);

    t0 = hal_get_time();
    solv_CRT(s_NTT_16[0][0], s_NTT_32[0][0], s_NTT_16[0][0]);
    t1 = hal_get_time();
    printcycles("CRT for 32x16 cycles:", t1 - t0);

    hal_send_str("OK KEYS\n");

    hal_send_str("#");

  }

  while(1);
  return 0;
}
