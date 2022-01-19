#ifndef CBD_MASKED_H
#define CBD_MASKED_H

#include "SABER_params.h"
#include <stdint.h>

void cbd_masked_single(uint16_t s0[SABER_N], uint16_t s1[SABER_N], const uint8_t coins0[SABER_POLYCOINBYTES], const uint8_t coins1[SABER_POLYCOINBYTES]);
void cbd_masked(uint16_t s[SABER_SHARES][SABER_L][SABER_N], const uint8_t coins[SABER_SHARES][SABER_L * SABER_POLYCOINBYTES]);

#endif
