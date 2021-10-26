
# TODO
Add requirement

# How to compile
```
sh makeAll.sh
```

# How to test and produce benchmarks manually
You can produce the benchmark manually.

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
st-flash write bin/crypto_kem_{lightsaber, saber, firesaber}_{m4fspeed, m4fstack}_{speed, f_speed}.bin 0x8000000
```

For the interpretations of the numbers, please go to the Section Interpretation of the numbers (`speed` and `f_speed`).

### Benchmark for stack
```
st-flash write bin/crypto_kem_{lightsaber, saber, firesaber}_{m4fspeed, m4fstack}_stack.bin 0x8000000
```

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

# Scripts
We also provide scripts for producing the benchmarks of cycles.

## Scripts for the schemes
```
python3 benchmarks.py
```
The numbers will be in the file `benchmarks.txt`.

## Scripts for individual functions
```
python3 f_benchmarks.py
```
The numbers will be in the files `f_benchmarks.txt`.

# Interpretation of the numbers (`speed` and `f_speed`)

## benchmarks.py
Running `python3 benchmarks.py` will produce benchmarks for the implementations. For each of the parameters `lightsaber`, `saber`, and `firesaber`, we report four different implementations. They are distinguished by the chosen strategy. Each implementation is reported as the following:
```
M4 results for {scheme} (impl={impl})
{scheme}{impl}keygen: XXXk
{scheme}{impl}encaps: XXXk
{scheme}{impl}decaps: XXXk
```
where `scheme` is one of the following:
- `lightsaber`
- `saber`
- `firesaber`

and `impl` is one of the following:
- `m3speed`
- `m3stack`

## `f_benchmarks.py`
Running `python3 f_benchmarks.py` will prduce the benchmarks for `MatrixVectorMul`, `InnerProd`, and NTT-related functions used in the implementations `m4fspeed` and `m4fstack`.

The numbers are categorized into two groups:
- Saber's `MatrixVectorMul` and `InnerProd`. These numbers are dependent on the chosen security level and optimization strategy.
    - `MatrixVectorMul_A`: `MatrixVectorMul` with the most speed-optimized strategy -- A.
    - `MatrixVectorMul_D`: `MatrixVectorMul` with the most stack-optimized strategy -- D.
    - `InnerProd (Encrypt)`: `InnerProd` for encryption with the most speed-optimized strategy.
    - `InnerProd (Decrypt)`: `InnerProd` for decryption with the most speed-optimized strategy.
    - `InnerProd stack`: `InnerProd` with the most stack-optimized strategy.
- NTT-related.
    - `32-bit NTT`: The cycles of applying one 32-bit NTT over `3329 * 7681`.
    - `16-bit NTT`: The cycles of applying one 16-bit NTT over `7681`.
    - `16-bit NTT light`: The cycles of applying one 16-bit NTT over `3329`.
    - `32-bit iNTT`: The cycles of applying one 32-bit iNTT over `3329 * 7681`. The result is then reduced to signed `mod 8192`.
    - `32-bit base_mul`: The cycles of applying oen 32-bit `base_mul`.
    - `32x16-bit base_mul`: The cycles of applying oen 16-bit `base_mul`. One of the multiplicand is a 32-bit value over `3329 * 7681` and it is reduced to one of the moduli first without extra memory operations.
    - `16-bit base_mul`: The cycles of applying oen 16-bit `base_mul`.
    - `16x16 CRT`: Solving CRT from the moduli `3329` and `7681`. The result is a value over `3329 * 7681`
    - `One mod`: Reduce a polynomial over `3329 * 7681` to one of the moduli `3329` or `7681`.


# Structure of this folder
```
.
├── Makefile
├── README.md
├── benchmarks.py
├── benchmarks.txt
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
├── config.py
├── crypto_kem
│   ├── f_speed.c
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
├── f_benchmarks.py
├── f_benchmarks.txt
├── libopencm3 -> ../../libopencm3/
├── makeAll.sh
├── read_serial.py
└── stm32f405x6.ld
```
