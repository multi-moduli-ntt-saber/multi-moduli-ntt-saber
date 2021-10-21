#ifndef OPT_MASK
#define OPT_MASK

#define _16_bit

#if SABER_L == 2
    #define MASK 0x8888
#elif SABER_L == 3
    #define MASK 0x8881
#elif SABER_L == 4
    #define MASK 0x8881
#endif

#endif

