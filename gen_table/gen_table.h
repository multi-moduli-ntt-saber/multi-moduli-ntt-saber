#ifndef GEN_TABLE_H
#define GEN_TABLE_H

#include <stddef.h>

#include "tools.h"

void gen_CT_table_generic(
    void *des, void *scale, void *omega, void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void gen_CT_negacyclic_table_generic(
    void *des, void *scale, void *omega, void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void gen_CT_table(int *des, int scale, int _omega, int _Q);
void gen_inv_CT_table(int *des, int scale, int _omega, int _Q);

void gen_twist_table(int *des, int scale, int _omega, int _Q);
void gen_CT_negacyclic_table(int *des, int scale, int _omega, int _Q);

void gen_mul_table(int *des, int scale, int _omega, int _Q);
void gen_all_mul_table(int *des, int scale, int _omega, int _Q);

void gen_streamlined_CT_table(int *des, int scale, int _omega, int _Q, struct compress_profile *_profile, int pad);
void gen_streamlined_CT_negacyclic_table(int *des, int scale, int _omega, int _Q, struct compress_profile *_profile, int pad);
void gen_streamlined_inv_CT_table(int *des, int scale, int _omega, int _Q, struct compress_profile *_profile, int pad);

void gen_streamlined_inv_CT_negacyclic_table(int *des, int scale1, int _omega, int scale2, int twist_omega, int _Q, struct compress_profile *_profile, int pad);

#endif