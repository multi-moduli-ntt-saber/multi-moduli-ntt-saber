
# How to compile
`sh makeAll.sh`

# How to test and produce benchmarks manually
You can produce the benchmark manually.

## One terminal reading from the board
```
python3 read_serial.py
```
## One terminal flashing as follows

### Test for successful key exchange
```
st-flash write bin/crypto_kem_saber_{m4speed, m4speedstack, m4stack}-masked_test.bin 0x8000000
```
### Benchmark for speed
```
st-flash write bin/crypto_kem_saber_{m4speed, m4speedstack, m4stack}-masked_speed.bin 0x8000000
st-flash write bin/crypto_kem_saber_m4speed-masked_f_speed.bin 0x8000000
```

### Benchmark for stack
```
st-flash write bin/crypto_kem_saber_{m4speed, m4speedstack, m4stack}-masked_stack.bin 0x8000000
```
