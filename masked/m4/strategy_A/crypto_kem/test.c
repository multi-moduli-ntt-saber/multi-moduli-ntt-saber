#include "api.h"
#include "randombytes.h"
#include "hal.h"
#include "fips202.h"

#include <string.h>

#define NTESTS 10

const uint8_t canary[8] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
};

/* allocate a bit more for all keys and messages and
 * make sure it is not touched by the implementations.
 */
static void write_canary(uint8_t *d){
    for(size_t i = 0; i < 8; i++){
        d[i] = canary[i];
    }
}

static int check_canary(const uint8_t *d){
    for(size_t i = 0; i < 8; i++){
        if (d[i] != canary[i]){
            return -1;
        }
    }
    return 0;
}

static int test_keys(void)
{
  unsigned char key_c[CRYPTO_BYTES+16], key_d[CRYPTO_BYTES+16];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES+16];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES+16];

  struct //__attribute__((__packed__)) //TODO: check alignment
  {
    unsigned char canary_front[8];
    sk_masked_s sk_masked;
    unsigned char canary_back[8];
  } sk_masked_with_canaries;

  write_canary(key_c); write_canary(key_c+sizeof(key_c)-8);
  write_canary(key_d); write_canary(key_d+sizeof(key_c)-8);

  write_canary(pk); write_canary(pk+sizeof(pk)-8);
  write_canary(sendb); write_canary(sendb+sizeof(sendb)-8);
  write_canary(sk_masked_with_canaries.canary_front); write_canary(sk_masked_with_canaries.canary_back);

  int i;

  for(i=0; i<NTESTS; i++)
  {

    //Alice generates a public key (masked)
    crypto_kem_keypair_masked(pk+8, &(sk_masked_with_canaries.sk_masked));
    hal_send_str("DONE masked key pair generation!");

    //Bob derives a secret key and creates a response
    crypto_kem_enc_masked(sendb+8, key_d+8, pk+8);
    hal_send_str("DONE masked encapsulation!");

    //Alice uses Bobs response to get her secret key (masked)
    crypto_kem_dec_masked(key_c+8, sendb+8, &(sk_masked_with_canaries.sk_masked));
    hal_send_str("DONE masked decapsulation!");

    if(memcmp(key_c+8, key_d+8, CRYPTO_BYTES)){
      hal_send_str("ERROR KEYS (masked)!");
    }
    else if(check_canary(key_c) || check_canary(key_c+sizeof(key_c)-8) ||
            check_canary(key_d) || check_canary(key_d+sizeof(key_d)-8) ||
            check_canary(pk) || check_canary(pk+sizeof(pk)-8) ||
            check_canary(sendb) || check_canary(sendb+sizeof(sendb)-8) ||
            check_canary(sk_masked_with_canaries.canary_front) || check_canary(sk_masked_with_canaries.canary_back))
    {
      hal_send_str("ERROR canary overwritten\n");
    }
    else
    {
      hal_send_str("OK KEYS\n");
    }
  }

  return 0;
}

static int test_invalid_sk_a(void)
{
  unsigned char key_c[CRYPTO_BYTES], key_d[CRYPTO_BYTES], key_cca_c[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  uint8_t kr[64]; // Will contain key, coins
  sk_masked_s sk_masked;
  int i;

  for(i=0; i<NTESTS; i++)
  {

    //Alice generates a public key (masked)
    crypto_kem_keypair_masked(pk, &sk_masked);

    //Bob derives a secret key and creates a response (masked)
    crypto_kem_enc_masked(sendb, key_d, pk);

    //Replace secret key with random values
    randombytes((uint8_t*)(void*)(&sk_masked), 2 * SABER_SHARES * SABER_L * SABER_N
                          + SABER_INDCPA_PUBLICKEYBYTES
                          + SABER_SHARES * 32
                          + SABER_KEYBYTES);

    //Alice uses Bobs response to get her secret key (masked)
    crypto_kem_dec_masked(key_c, sendb, &sk_masked);

    // The following part is specific to SABER's CCA transform
    sha3_256(kr + 32, sendb, CRYPTO_CIPHERTEXTBYTES); // overwrite coins in kr with h(c)
    memcpy(kr, sk_masked.z, CRYPTO_BYTES);
    sha3_256(key_cca_c, kr, 64); // hash concatenation of pre-k and h(c) to k

    if((!memcmp(key_c, key_d, CRYPTO_BYTES)) | memcmp(key_c, key_cca_c, CRYPTO_BYTES)){
      hal_send_str("ERROR invalid sk_a (masked)\n");
    }else{
      hal_send_str("OK invalid sk_a\n");
    }
  }

  return 0;
}

static int test_invalid_ciphertext(void)
{
  unsigned char key_c[CRYPTO_BYTES], key_d[CRYPTO_BYTES], key_cca_c[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  uint8_t kr[64]; // Will contain key, coins
  sk_masked_s sk_masked;
  int i;
  size_t pos;

  for(i=0; i<NTESTS; i++)
  {
    randombytes((unsigned char *)&pos, sizeof(size_t));

    //Alice generates a public key (masked)
    crypto_kem_keypair_masked(pk, &sk_masked);

    //Bob derives a secret key and creates a response (masked)
    crypto_kem_enc_masked(sendb, key_d, pk);

    // Change ciphertext to random value
    randombytes(sendb, sizeof(sendb));

    //Alice uses Bobs response to get her secret key (masked)
    crypto_kem_dec_masked(key_c, sendb, &sk_masked);

    // The following part is specific to SABER's CCA transform
    sha3_256(kr + 32, sendb, CRYPTO_CIPHERTEXTBYTES); // overwrite coins in kr with h(c)
    memcpy(kr, sk_masked.z, CRYPTO_BYTES);
    sha3_256(key_cca_c, kr, 64); // hash concatenation of pre-k and h(c) to k

    if((!memcmp(key_c, key_d, CRYPTO_BYTES)) | memcmp(key_c, key_cca_c, CRYPTO_BYTES)){
      hal_send_str("ERROR invalid ciphertext (masked)\n");
    }else{
      hal_send_str("OK invalid ciphertext\n");
    }
  }

  return 0;
}

int main(void){

    hal_setup(CLOCK_FAST);

    int i;
    // marker for automated testing
    for(i = 0; i < 10; i++){
        hal_send_str("==========================");
    }

    test_keys();
    test_invalid_sk_a();
    test_invalid_ciphertext();


    hal_send_str("#");

    while(1);

    return 0;
}
