
# TODO
finish the readme

# How to compile
```
sh makeAll.sh
```

# How to produce benchmark

## One terminal reading from the board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test for successful key exchange
```
st-flash write bin/crypto_kem_lightsaber_m4fspeed_test.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fspeed_test.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fspeed_test.bin 0x8000000

st-flash write bin/crypto_kem_lightsaber_m4fstack_test.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fstack_test.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fstack_test.bin 0x8000000
```

### Benchmark for speed
```
st-flash write bin/crypto_kem_lightsaber_m4fspeed_speed.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fspeed_speed.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fspeed_speed.bin 0x8000000

st-flash write bin/crypto_kem_lightsaber_m4fstack_speed.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fstack_speed.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fstack_speed.bin 0x8000000
```

### Benchmark for stack
```
st-flash write bin/crypto_kem_lightsaber_m4fspeed_stack.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fspeed_stack.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fspeed_stack.bin 0x8000000

st-flash write bin/crypto_kem_lightsaber_m4fstack_stack.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4fstack_stack.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4fstack_stack.bin 0x8000000
```

# Meaning of the numbers

## Speed
### Sample output

After flashing with `st-flash write bin/crypto_kem_saber_m4fspeed_speed.bin 0x8000000`, we'll get something close to the following:

```
==========================
keypair cycles:
648131
encaps cycles:
827772
decaps cycles:
779666
OK KEYS


Cycles for NTT-related functions

32-bit NTT forward cycles:
5856
16-bit NTT forward cycles:
4823
16-bit NTT forward light cycles:
4374
32-bit NTT inverse cycles:
7141

Cycles for base_mul-related functions

32-bit base_mul cycles:
4185
32x16-bit base_mul cycles:
3733
16-bit base_mul cycles:
2965

Cycles for auxiliary functions

16x16 CRT cycles:
2435
One mod cycles:
1172

Cycles for {speed, stack}-opt MatrixVectorMul

MatrixVectorMul speed opt cycles:
135551
MatrixVectorMul stack opt cycles:
293064

Cycles for {speed, stack}-opt InnerProd

InnerProd(Encrypt) speed opt cycles:
39409
InnerProd(Decrypt) speed opt cycles:
56842
InnerProd stack opt cycles:
97694
#
```

The first part is the numbers for the each of key generation, encapsulation, and decapsulation. Since they are dependent on the chosen security level and optimization strategy, you'll see different numbers by flashing different binary files.

The second part is the numbers for each of the functionalities. They can be categorized into two groups:

- Basic building blocks:

- Saber's `MatrixVectorMul` and `InnerProd`:

## Stack

# Structure of this folder
```
.
├── Makefile
├── README.md
├── common
│   ├── crypto_hashblocks_sha512.c
│   ├── crypto_hashblocks_sha512.h
│   ├── crypto_hashblocks_sha512_inner32.s
│   ├── fips202.c
│   ├── fips202.h
│   ├── hal-stm32f4.c
│   ├── hal.h
│   ├── keccakf1600.S
│   ├── keccakf1600.h
│   ├── randombytes.c
│   ├── randombytes.h
│   ├── sendfn.h
│   ├── sha2.c
│   └── sha2.h
├── crypto_kem
│   ├── firesaber
│   │   ├── m4fspeed
│   │   └── m4fstack
│   ├── lightsaber
│   │   ├── m4fspeed
│   │   └── m4fstack
│   ├── saber
│   │   ├── m4fspeed
│   │   └── m4fstack
│   ├── speed.c
│   ├── stack.c
│   └── test.c
├── libopencm3 -> ../../libopencm3/
├── makeAll.sh
├── read_serial.py
└── stm32f405x6.ld
```
