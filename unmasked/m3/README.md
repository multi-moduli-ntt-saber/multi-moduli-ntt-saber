
# TODO
Interpretation of the numbers.

We use the board `nucleo-f207zg`.
Please check the name of the device that is recognized by your computer.
Our setting is in the file `config.py`.
If you're using a macOS, the prefix of the name name will be `/dev/tty.usbmodem`.

# How to Compile
```
make -j4
```

# Scripts
We provide scripts for producing the benchmarks.

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
The numbers will be in the file `f_16_benchmarks.txt` and `f_32_benchmarks.txt`.

# How to test and produce benchmarks manually

## One terminal reading from board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test for successful key exchange
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_test.elf reset exit"
```

### Benchmark for speed
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_speed.elf reset exit"
```

### Benchmark for stack
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_{lightsaber, saber, firesaber}_{m3speed, m3speedstack, m3stack, m3_32bit}_stack.elf reset exit"
```

# Interpretation of the numbers
TBA

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




