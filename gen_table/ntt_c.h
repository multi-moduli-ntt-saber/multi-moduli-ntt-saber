#ifndef NTT_C_H
#define NTT_C_H

#include "tools.h"

void CT_butterfly(int *src, int indx_a, int indx_b, int twiddle, int _Q);

void _m_layer_CT_butterfly(int *src, int layers, int step, int *_root_table, int _Q);
void _m_layer_inv_CT_butterfly(int *src, int layers, int step, int *_root_table, int Q);

void compressed_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int _Q, struct compress_profile *_profile);
void compressed_inv_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int _Q, struct compress_profile *_profile);

#endif