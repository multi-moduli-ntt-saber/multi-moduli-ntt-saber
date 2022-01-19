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
  unsigned char key_c[CRYPTO_BYTES], key_d[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
  unsigned long long t0, t1;
  int i;
  int crypto_i;
  sk_masked_s sk_masked;

  hal_setup(CLOCK_BENCHMARK);

  for(i = 0; i < 60; i++){
      hal_send_str("==========================");
  }

  for(crypto_i = 0; crypto_i < ITERATIONS; i++){

    // Masked key-pair generation
    crypto_kem_keypair(pk, sk);
    crypto_kem_keypair_sk_masked(&sk_masked, sk);

    // Encapsulation
    crypto_kem_enc(ct, key_d, pk);

    // Masked decapsulation
    t0 = hal_get_time();
    crypto_kem_dec_masked(key_c, ct, &sk_masked);
    t1 = hal_get_time();
    printcycles("masked decaps cycles:", t1-t0);

    if(memcmp(key_c, key_d, CRYPTO_BYTES)){
      hal_send_str("ERROR KEYS (masked)\n");
    }
    else {
      hal_send_str("OK KEYS\n");
    }

    hal_send_str("#");

  }

  while(1);
  return 0;
}
