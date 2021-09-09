

#include "NTT_params.h"

#include "tools.h"

void cmod_int16(void *des, void *src, void *mod){
    int16_t mod_v = *(int16_t*)mod;
    int16_t t = (*(int16_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int16_t*)des = t;
}

void cmod_int32(void *des, void *src, void *mod){
    int32_t mod_v = *(int32_t*)mod;
    int32_t t = (*(int32_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int32_t*)des = t;
}

void cmod_int64(void *des, void *src, void *mod){
    int64_t mod_v = *(int64_t*)mod;
    int64_t t = (*(int64_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int64_t*)des = t;
}

void mulmod_int16(void *des, void *src1, void *src2, void *mod){

    int32_t src1_v;
    int32_t src2_v;
    int32_t tmp_v;
    int32_t mod_v;
    int32_t des_v;

    src1_v = (int32_t)(*(int16_t*)src1);
    src2_v = (int32_t)(*(int16_t*)src2);
    tmp_v = src1_v * src2_v;
    mod_v = (int32_t)(*(int16_t*)mod);

    cmod_int32(&des_v, &tmp_v, &mod_v);

    *(int16_t*)des = (int16_t)des_v;

}

void mulmod_int32(void *des, void *src1, void *src2, void *mod){

    int64_t src1_v;
    int64_t src2_v;
    int64_t tmp_v;
    int64_t mod_v;
    int64_t des_v;

    src1_v = (int64_t)(*(int32_t*)src1);
    src2_v = (int64_t)(*(int32_t*)src2);
    tmp_v = src1_v * src2_v;
    mod_v = (int64_t)(*(int32_t*)mod);

    cmod_int64(&des_v, &tmp_v, &mod_v);

    *(int32_t*)des = (int32_t)des_v;

}

void expmod_int16(void *des, void *src, size_t e, void *mod){

    int16_t src_v = *(int16_t*)src;
    int16_t tmp_v;

    tmp_v = 1;
    for(; e; e >>= 1){
        if(e & 1){
            mulmod_int16(&tmp_v, &tmp_v, &src_v, mod);
        }
        mulmod_int16(&src_v, &src_v, &src_v, mod);
    }

    memcpy(des, &tmp_v, sizeof(int16_t));

}

void expmod_int32(void *des, void *src, size_t e, void *mod){

    int32_t src_v = *(int32_t*)src;
    int32_t tmp_v;

    tmp_v = 1;
    for(; e; e >>= 1){
        if(e & 1){
            mulmod_int32(&tmp_v, &tmp_v, &src_v, mod);
        }
        mulmod_int32(&src_v, &src_v, &src_v, mod);
    }

    memcpy(des, &tmp_v, sizeof(int32_t));

}

void bitreverse_generic(void *src, size_t len, size_t size){

    char tmp[size];

    for(size_t i = 0, j = 0; i < len; i++){
        if(i < j){
            memcpy(tmp, src + i * size, size);
            memcpy(src + i * size, src + j * size, size);
            memcpy(src + j * size, tmp, size);
        }
        for(size_t k = len >> 1; (j ^= k) < k; k >>= 1);
    }

}


// sync below in the future

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

void _16_to_32(int *des, int len){
    int16_t *ptr = (int16_t*)des;
    for(int i = len - 1; i >= 0; i--){
        des[i] = ptr[i];
    }
}

void _32_to_16(int *des, int len){
    int16_t *ptr = (int16_t*)des;
    for(int i = 0; i < len; i++){
        ptr[i] = des[i];
    }
}



void bitreverse(int *src, int n){
    for(int i = 0, j = 0; i < n; i++){
        if(i < j){
            src[i] += src[j];
            src[i] -= (src[j] = (src[i] - src[j]));
        }
        for(int k = n >> 1; (j ^= k) < k; k >>=1);
    }
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














