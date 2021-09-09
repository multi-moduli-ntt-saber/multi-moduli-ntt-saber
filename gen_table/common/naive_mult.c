
#include "naive_mult.h"

int center_mul(int src1, int src2, int mod){
    int t = (int)(((long long)src1 * (long long)src2) % (long long)mod);
    if(t > (mod >> 1)){
        return t - mod;
    }
    if(t < - (mod >> 1)){
        return t + mod;
    }
    return t;
}

int expmod(int a, int b, int mod){
    if(b == 0){
        return 1;
    }
    if(b == 1){
        return a;
    }
    int e = 1;
    for(; b; b >>=1){
        if(b & 1){
            e = center_mul(e, a, mod);
        }
        a = center_mul(a, a, mod);
    }
    return e;
}

void naive_mulR(int *des, int *src1, int *src2, int len, int twiddle, int mod){
    int *tmp = (int*)malloc(sizeof(int) * (len << 1));
    memset(tmp, 0, sizeof(int) * (len << 1));

    for(int i = 0; i < len; i++){
        for(int j = 0; j < len; j++){
            tmp[i + j] = center_mul(tmp[i + j] + center_mul(src1[i], src2[j], mod), 1, mod);
        }
    }

    for(int i = ((len - 1) << 1); i >= len; i--){
        des[i - len] = center_mul(tmp[i - len] + center_mul(tmp[i], twiddle, mod), 1, mod);
    }
    des[len - 1] = center_mul(tmp[len - 1], 1, mod);

    free(tmp);
}

void point_mul(int *src1, int *src2, int len, int jump, int mod){
    for(int i = 0; i < len; i++){
        for(int j = 0; j < jump; j++){
            src1[i * jump + j] = center_mul(src1[i * jump + j], src2[i], mod);
        }
    }
}

void school_book(int *src1, int *src2, int *_mul_table, int Q){
    int p1[ARRAY_N >> LOGNTT_N], p2[ARRAY_N >> LOGNTT_N];

    int offset;

    for(int i = 0; i < (NTT_N >> 1); i++){
        offset = 0;
        for(int j = 0; j < (ARRAY_N >> LOGNTT_N); j++){
            p1[j] = src1[i * ((ARRAY_N >> LOGNTT_N) << 1) + offset + j];
            p2[j] = src2[i * ((ARRAY_N >> LOGNTT_N) << 1) + offset + j];
        }
        naive_mulR(src1 + i * ((ARRAY_N >> LOGNTT_N) << 1) + offset, p1, p2, ARRAY_N >> LOGNTT_N, _mul_table[i], Q);
        offset = ARRAY_N >> LOGNTT_N;
        for(int j = 0; j < (ARRAY_N >> LOGNTT_N); j++){
            p1[j] = src1[i * ((ARRAY_N >> LOGNTT_N) << 1) + offset + j];
            p2[j] = src2[i * ((ARRAY_N >> LOGNTT_N) << 1) + offset + j];
        }
        naive_mulR(src1 + i * ((ARRAY_N >> LOGNTT_N) << 1) + offset, p1, p2, ARRAY_N >> LOGNTT_N, -_mul_table[i], Q);
    }
}

void print_table(int *table, int table_len){

    printf("{\n");
    for(int i = 0; i < table_len - 1; i++){
        printf("%d, ", table[i]);
    }
    printf("%d\n};\n", table[table_len - 1]);

}






