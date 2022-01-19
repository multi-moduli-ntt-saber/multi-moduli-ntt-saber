#include "api.h"
#include "randombytes.h"
#include "hal.h"
#include "sendfn.h"

#include <string.h>

#define MAX_SIZE 0x1B000

// https://stackoverflow.com/a/1489985/1711232

#define send_stack_usage(S, U) send_unsigned((S), (U))

unsigned int canary_size = MAX_SIZE;
volatile unsigned char *p;
unsigned int c;
uint8_t canary = 0x42;

unsigned char key_c[CRYPTO_BYTES], key_d[CRYPTO_BYTES];
unsigned char pk[CRYPTO_PUBLICKEYBYTES];
unsigned char sk[CRYPTO_SECRETKEYBYTES];
unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
unsigned int stack_key_gen_masked, stack_encaps_masked, stack_decaps_masked;
sk_masked_s sk_masked;

#define FILL_STACK()                                                           \
  p = &a;                                                                      \
  while (p > &a - canary_size)                                                 \
    *(p--) = canary;
#define CHECK_STACK()                                                          \
  c = canary_size;                                                             \
  p = &a - canary_size + 1;                                                    \
  while (*p == canary && p < &a) {                                             \
    p++;                                                                       \
    c--;                                                                       \
  }

static int test_keys(void) {
  volatile unsigned char a;

  // Alice generates a public key
  crypto_kem_keypair(pk, sk);
  crypto_kem_keypair_sk_masked(&sk_masked, sk);

  // Bob derives a secret key and creates a response
  crypto_kem_enc(sendb, key_d, pk);

  // Alice uses Bobs response to get her secret key (masked)
  FILL_STACK()
  crypto_kem_dec_masked(key_c, sendb, &sk_masked);
  CHECK_STACK()
  if(c >= canary_size) return -1;
  stack_decaps_masked = c;

  if (memcmp(key_c, key_d, CRYPTO_BYTES)) {
    return -1;
  } else {
    send_stack_usage("masked decaps stack usage:", stack_decaps_masked);
    hal_send_str("OK KEYS\n");
    return 0;
  }
}

int main(void) {
  hal_setup(CLOCK_FAST);

  // marker for automated benchmarks

  int i;

  for(i = 0; i < 60; i++){
    hal_send_str("==========================");
  }

  canary_size = 0x1000;
  while(test_keys()){
    canary_size += 0x1000;
    if(canary_size >= MAX_SIZE) {
      hal_send_str("failed to measure stack usage.\n");
      break;
    }
  }
  // marker for automated benchmarks
  hal_send_str("#");

  while (1);

  return 0;
}