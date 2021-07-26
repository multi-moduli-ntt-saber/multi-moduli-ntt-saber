
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
```
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_test.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_test.elf reset exit"

openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_speed.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_speed.elf reset exit"

openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_lightsaber_m3_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_saber_m3_stack.elf reset exit"
openocd -f nucleo-f2.cfg -c "program elf/crypto_kem_firesaber_m3_stack.elf reset exit"
```

