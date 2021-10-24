
# TODO
- Sync the presentation of interpretations.
- Possibly add some scripts.
- Double check the name of the strategies.

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
st-flash write bin/crypto_kem_{lightsaber, saber, firesaber}_{m4fspeed, m4fstack}_test.bin 0x8000000
```

### Benchmark for speed
```
st-flash write bin/crypto_kem_{lightsaber, saber, firesaber}_{m4fspeed, m4fstack}_speed.bin 0x8000000
```

### Benchmark for stack
```
st-flash write bin/crypto_kem_{lightsaber, saber, firesaber}_{m4fspeed, m4fstack}_stack.bin 0x8000000
```

# Interpretation of the numbers

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

- Saber's `MatrixVectorMul` and `InnerProd`: These numbers depend only on the chosen security level.
    - `MatrixVectorMul`. There are two numbers as follows.
        - `MatrixVectorMul speed`: `MatrixVectorMul` with the most speed-optimized strategy -- A.
        - `MatrixVectorMul stack`: `MatrixVectorMul` with the most stack-optimized strategy -- D.
    - `InnerProd`. There are three numbers as follows.
        - `InnerProd (Encrypt) speed`: `InnerProd` for encryption with the most speed-optimized strategy -- A. Note that one of the vectors is already transformed.
        - `InnerProd (Decrypt) speed`: `InnerProd` for decryption with the most speed-optimized strategy -- A.
        - `InnerProd stack`: `InnerProd` with the most stack-optimized strategy -- D. Note that such strategy results in the same implementation for `InnerProd` used in encryption and decryption.

- Basic building blocks: These numbers are independent from the chosen security level and optimization strategy.
    - NTT-related. There are four numbers as follows.
        - `32-bit NTT`: 32-bit NTT defined over the modulus `3329 x 7681`
        - `16-bit NTT`: 16-bit NTT defined over the modulus `7681`
        - `16-bit NTT light`: 16-bit NTT defined over the modulus `3329`
        - `32-bit NTT inverse`: 32-bit iNTT defined over the modulus `3329 x 7681` followed by reduction to signed `mod 8192`
    - base_mul-related. There are three numbers as follows.
        - `32-bit`: 32-bit base multiplication
        - `32x16-bit`: 16-bit base multiplication, but one of the multiplicand is over the modulus `3329 x 7681`. One modular reduction to a 16-bit prime is performed before the base multiplication.
        - `16-bit`: 16-bit base multiplication.
    - auxiliary. There are two numbers as follows.
        - `16x16 CRT`: Solving CRT from the moduli `3329` and `7681`. The result is a 32-bit number over the modulus `3329 x 7681`.
        - `One mod`: One modular reduction reducing the coefficients a polynomial over the modulus `3329 x 7681` to one of the moduli `3329` or `7681`.


## Stack
### Sample output
After flashing with `st-flash write bin/crypto_kem_saber_m4fspeed_stack.bin 0x8000000`, we'll get something close to the following.

```
==========================
keypair stack usage:
6780
encaps stack usage:
7444
decaps stack usage:
7452
OK KEYS

#
```

The three numbers correspond to the stack usage of key generation, encapsulation, and decapsulation.

# Structure of this folder
```
.
├── Makefile
├── README.md
├── common
│   ├── fips202.c
│   ├── fips202.h
│   ├── hal-stm32f4.c
│   ├── hal.h
│   ├── keccakf1600.S
│   ├── keccakf1600.h
│   ├── randombytes.c
│   ├── randombytes.h
│   └── sendfn.h
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
