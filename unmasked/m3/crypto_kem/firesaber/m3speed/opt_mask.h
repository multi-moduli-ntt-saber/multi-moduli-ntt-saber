#ifndef OPT_MASK
#define OPT_MASK

// #define STACK

#ifdef STACK

#if SABER_L == 2
    #define MASK 0x8884
#elif SABER_L == 3
    #define MASK 0x8882
#elif SABER_L == 4
    #define MASK 0x8882
#endif

#else

#if SABER_L == 2
    #define MASK 0x1111
#elif SABER_L == 3
    #define MASK 0x1111
#elif SABER_L == 4
    #define MASK 0x1111
#endif

#endif

#endif

