#ifndef TOOLS_H
#define TOOLS_H

struct compress_profile{
    int compressed_layers;
    int merged_layers[16];
};

void cmod_int16(void *des, void *src, void *mod);
void cmod_int32(void *des, void *src, void *mod);

void mul_int16(void *des, void *src1, void *src2, void *mod);
void mul_int32(void *des, void *src1, void *src2, void *mod);


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