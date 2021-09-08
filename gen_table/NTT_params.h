#ifndef NTT_PARAMS_H
#define NTT_PARAMS_H

#define ARRAY_N 256

#define NTT_N 64
#define LOGNTT_N 6

#define Q1 7681
#define Q1pr 13
#define omegaQ1 5258
#define invomegaQ1 3449
#define RmodQ1 -3593
#define Q1prime 7679
#define Q1Q1prime 503389695
#define invNQ1 7561

#define Q2 7937
#define Q2pr 3
#define omegaQ2 2458
#define invomegaQ2 578
#define RmodQ2 2040
#define Q2prime 7935
#define Q2Q2prime 520167167
#define invNQ2 7813

// 7681 * 7937 = 60964097
#define Q1Q2 60964097

// omegaQ1 = CRT(5258, 2458)
#define omegaQ1Q2 3899525
// invomegaQ1 = omegaQ1^{-1} mod Q1
#define invomegaQ1Q2 -13008165
// RmodQ1 = 2^32 mod^{+-} Q1
#define RmodQ1Q2 27480506
// Q1prime = -Q1^{+-} mod 2^32
#define Q1Q2prime 1712930047
// invNQ1 = NTT_N^{-1} mod Q1
#define invNQ1Q2 60011533

#endif