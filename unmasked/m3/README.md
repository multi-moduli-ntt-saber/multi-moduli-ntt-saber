

# TODO
Documentation

Describe the port for the board `nucleo-f207zg`

Possibly split the strategies.

# How to Compile

```
make -j4
```

# How to produce benchmark

## One terminal reading from board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test for successful key exchange
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speed_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speedstack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3stack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_32bit_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speed_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speedstack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3stack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_32bit_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speed_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speedstack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3stack_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_32bit_test.elf reset exit"
```

### Benchmark for speed
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speed_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speedstack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3stack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_32bit_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speed_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speedstack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3stack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_32bit_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speed_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speedstack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3stack_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_32bit_speed.elf reset exit"
```

### Benchmark for stack
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speed_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3speedstack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3stack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_32bit_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speed_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3speedstack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3stack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_32bit_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speed_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3speedstack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3stack_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_32bit_stack.elf reset exit"
```

# Interpretation of the numbers
TBA

```
.
├── Makefile
├── README.md
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
├── crypto_kem
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
├── libopencm3 -> ../../libopencm3/
├── mk
│   ├── nucleo-f207zg.mk
│   └── opencm3.mk
├── nucleo-f2.cfg
└── read_serial.py
```




