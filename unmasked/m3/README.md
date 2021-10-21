

# TODO
Documentation

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




