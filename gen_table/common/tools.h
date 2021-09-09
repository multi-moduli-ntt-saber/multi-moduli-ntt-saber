#ifndef TOOLS_H
#define TOOLS_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct compress_profile{
    size_t compressed_layers;
    size_t merged_layers[16];
};

void cmod_int16(void *des, void *src, void *mod);
void cmod_int32(void *des, void *src, void *mod);
void cmod_int64(void *des, void *src, void *mod);

void mulmod_int16(void *des, void *src1, void *src2, void *mod);
void mulmod_int32(void *des, void *src1, void *src2, void *mod);

void expmod_int16(void *des, void *src, size_t e, void *mod);
void expmod_int32(void *des, void *src, size_t e, void *mod);

void bitreverse_generic(void *src, size_t len, size_t size);

// sync below in the future

void _16_to_32(int *des, int len);
void _32_to_16(int *des, int len);
void bitreverse(int *src, int n);

int center_mul(int src1, int src2, int mod);
int expmod(int a, int b, int mod);

void naive_mulR(int *des, int *src1, int *src2, int len, int twiddle, int mod);
void point_mul(int *src1, int *src2, int len, int jump, int mod);
void school_book(int *src1, int *src2, int *_mul_table, int _Q);

void print_table(int *table, int table_len);

#endif

