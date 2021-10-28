#include "verify.h"

static uint8_t verify(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint32_t r;
    size_t i;

    r = 0;
    for (i = 0; i < len; i++) {
        r |= a[i] ^ b[i];
    }

    r = (~r + 1); // Two's complement
    r >>= 31;
    return (uint8_t)r;
}

/* b = 1 means mov, b = 0 means don't mov*/
void cmov(uint8_t *r, const uint8_t *x, size_t len, uint8_t b)
{
    size_t i;

    b = -b;
    for (i = 0; i < len; i++) {
        r[i] ^= b & (x[i] ^ r[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

// Section 4.1.5
uint8_t verify_masked(shake128incctx shake_masked_compare[2])
{
    uint8_t hash[2][32];

    shake128_inc_finalize(&shake_masked_compare[0]);
    shake128_inc_finalize(&shake_masked_compare[1]);

    shake128_inc_squeeze(hash[0], 32, &shake_masked_compare[0]);
    shake128_inc_squeeze(hash[1], 32, &shake_masked_compare[1]);

    return verify(hash[0], hash[1], 32);
}

/* b = 1 means mov, b = 0 means don't mov and unmask*/
void cmov_masked(uint8_t r[SABER_SHARES][64], const uint8_t *x, size_t len, uint8_t b)
{
    size_t i;

    b = -b;
    for (i = 0; i < len; i++) {
        r[1][i] &= ~b;
    }

    for (i = 0; i < len; i++) {
        r[0][i] ^= (b & (x[i] ^ r[0][i])) | r[1][i];
    }
}


