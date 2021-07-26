
# Speed-Optimized Unmasked Saber with 32-bit NTT on M4

# How to Compile
```
sh makeAll.sh
```

# How to produce benchmark

## One terminal reading from board
```
python3 read_serial.py
```

## One terminal flashing as follows
```
st-flash write bin/crypto_kem_lightsaber_m4f_test.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4f_test.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4f_test.bin 0x8000000

st-flash write bin/crypto_kem_lightsaber_m4f_speed.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4f_speed.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4f_speed.bin 0x8000000

st-flash write bin/crypto_kem_lightsaber_m4f_stack.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4f_stack.bin 0x8000000
st-flash write bin/crypto_kem_firesaber_m4f_stack.bin 0x8000000
```



