
# TODO
- Add benchmarks for `MatrixVectorMul_B` and `MatrixVectorMul_C` for 16-bit.
- Add sample output for `test` and `stack`.

We use the board `nucleo-f207zg`.
Please check the name of the device recognized by your computer.
Our setting is in the file `config.py`.
If you're using a macOS, the prefix of the name name will be `/dev/tty.usbmodem`.

# How to Compile
```
make -j4
```

# How to test and produce benchmarks manually
You can also produce the benchmark manually.

## One terminal reading from board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test for successful key exchange
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_test.elf reset exit"
```

TODO: Add sample output

### Benchmark for speed
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_{speed, f_speed}.elf reset exit"
```

For the interpretations of the numbers, please go to the Section `Interpretation of the numbers (cycles)`.

### Benchmark for stack
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_stack.elf reset exit"
```

TODO: Add sample output

# Scripts
We provide scripts for producing the benchmarks of cycles.

## Scripts for the schemes
```
python3 benchmarks.py
```
The numbers will be in the file `benchmarks.txt`.

## Scripts for individual functions
```
python3 f_16_benchmarks.py
python3 f_32_benchmarks.py
```
The numbers will be in the files `f_16_benchmarks.txt` for `f_16_benchmarks.py` and `f_32_benchmarks.txt` for `f_32_benchmarks.py`.

# Interpretation of the numbers (cycles)

## `benchmarks.py`
Running `python3 benchmarks.py` will produce benchmarks for the implementations. For each of the parameters `lightsaber`, `saber`, and `firesaber`, we report four different implementations. They are distinguished by the chosen strategy and the size of the arithmetic (16-bit or 32-bit). Each implementation is reported as the following:
```
M3 results for {scheme} (impl={impl})
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
- `m3speedstack`
- `m3stack`
- `m3_32bit`

Note that in our paper, we only report `m3speed`, `m3stack`, and `m3_32bit`. Implementations tagged with `m3speedstack` are benchmarked for illustrating a better resolution of time-memory trade-offs.

## `f_16_benchmarks.py`
Running `python3 f_16_benchmarks.py` will prduce the benchmarks for `MatrixVectorMul`, `InnerProd`, and NTT-related functions used in the implementations `m3speed`, `m3speedstack`, and `m3stack`.

The numbers are categorized into two groups:
- Saber's `MatrixVectorMul` and `InnerProd`. These numbers are dependent on the chosen security level and optimization strategy.
    - `MatrixVectorMul_A`: `MatrixVectorMul` with the most speed-optimized strategy -- A.
    - `MatrixVectorMul_B`: TBA. `MatrixVectorMul` with strategy B for key generation.
    - `MatrixVectorMul_C`: TBA. `MatrixVectorMul` with strategy C for encryption.
    - `MatrixVectorMul_D`: `MatrixVectorMul` with the most stack-optimized strategy -- D.
    - `InnerProd (Encrypt)`: `InnerProd` for encryption with the most speed-optimized strategy.
    - `InnerProd (Decrypt)`: `InnerProd` for decryption with the most speed-optimized strategy.
    - `InnerProd stack`: `InnerProd` with the most stack-optimized strategy.
- NTT-related.
    - `two 16-bit NTTs`: The cycles of applying two NTTs, one over `3329` and one over `7681`.
    - `two 16-bit base_mul`: The cycles of applying two `base_mul`'s.
    - `two 16-bit iNTTs`: The cycles of applying two iNTTs.
    - `16-bit by 16-bit CRT`: The cycles of solving `CRT` from `3329` and `7681`. The 16-bit results in signed `mod 8192` are derived.


## `f_32_benchmarks.py`
Running `python3 f_32_benchmarks.py` will produce the benchmarks for `MatrixVectorMul`, `InnerProd`, and NTT-related functions used in the implementation `m3_32bit`.

The numbers are categorized into two groups:
- Saber's `MatrixVectorMul` and `InnerProd`. These numbers are dependent on the chosen security level and optimization strategy.
    - `MatrixVectorMul_A`: `MatrixVectorMul` with the most speed-optimized strategy -- A.
    - `InnerProd (Encrypt)`: `InnerProd` for encryption with the most speed-optimized strategy.
    - `InnerProd (Decrypt)`: `InnerProd` for decryption with the most speed-optimized strategy.
- NTT-related.
    - `32-bit NTT`: The cycles of applying one NTT with constant time emulation for 32-bit arithmetic.
    - `32-bit NTT_leak`: The cycles of applying one NTT with variable time 32-bit arithmetic.
    - `32-bit base_mul`: The cycles of applying one `base_mul` with constant time emulation for 32-bit arithmetic.
    - `32-bit iNTT`: The cycles of applying oen iNTT with constant time emulation for 32-bit arithmetic.



```
.
├── Makefile
├── README.md
├── benchmarks.py
├── benchmarks.txt
├── common
│   ├── fips202.c
│   ├── fips202.h
│   ├── hal-opencm3.c
│   ├── hal.h
│   ├── keccakf1600.S
│   ├── keccakf1600.c
│   ├── keccakf1600.h
│   ├── randombytes.c
│   ├── randombytes.h
│   ├── sendfn.h
│   └── test.c
├── config.py
├── crypto_kem
│   ├── f_speed.c
│   ├── firesaber
│   │   ├── m3_32bit
│   │   ├── m3speed
│   │   ├── m3speedstack
│   │   └── m3stack
│   ├── lightsaber
│   │   ├── m3_32bit
│   │   ├── m3speed
│   │   ├── m3speedstack
│   │   └── m3stack
│   ├── saber
│   │   ├── m3_32bit
│   │   ├── m3speed
│   │   ├── m3speedstack
│   │   └── m3stack
│   ├── speed.c
│   ├── stack.c
│   └── test.c
├── f_16_benchmarks.py
├── f_16_benchmarks.txt
├── f_32_benchmarks.py
├── f_32_benchmarks.txt
├── libopencm3 -> ../../libopencm3/
├── mk
│   ├── nucleo-f207zg.mk
│   └── opencm3.mk
├── nucleo-f2.cfg
└── read_serial.py
```




