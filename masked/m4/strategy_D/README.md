
# Speed Optimized Masked Saber

# How to compile
```
sh makeAll.sh
```

# How to produce benchmark

## One terminal reading from board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test
```
st-flash write bin/crypto_kem_saber_m4-masked_test.bin 0x8000000
```

### Speed
```
st-flash write bin/crypto_kem_saber_m4-masked_speed.bin 0x8000000
```

### Stack
```
st-flash write bin/crypto_kem_saber_m4-masked_stack.bin 0x8000000
```

