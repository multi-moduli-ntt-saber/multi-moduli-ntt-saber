#ifndef NAIVE_MULT
#define NAIVE_MULT

// don't use this file with the generation of tables now,
// there are collision of function names
// the renaming will be done in the future

int center_mul(int src1, int src2, int mod);
int expmod(int a, int b, int mod);
void naive_mulR(int *des, int *src1, int *src2, int len, int twiddle, int mod);
void point_mul(int *src1, int *src2, int len, int jump, int mod);
void school_book(int *src1, int *src2, int *_mul_table, int Q);
void print_table(int *table, int table_len);

#endif

