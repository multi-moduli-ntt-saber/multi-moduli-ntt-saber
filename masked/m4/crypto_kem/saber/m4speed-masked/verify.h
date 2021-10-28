#ifndef VERIFY_H
#define VERIFY_H

#include "SABER_params.h"
#include "fips202.h"
#include <stddef.h>
#include <stdint.h>

/* b = 1 means mov, b = 0 means don't mov*/
void cmov(uint8_t *r, const uint8_t *x, size_t len, uint8_t b);

////////////////////////////////////////////////////////////////////////////////////////////////
///                                   Masking functions                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////

void cmov_masked(uint8_t r[SABER_SHARES][64], const uint8_t *x, size_t len, uint8_t b);
uint8_t verify_masked(shake128incctx shake_masked_compare[2]);

#endif
