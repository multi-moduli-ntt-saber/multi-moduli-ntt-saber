
We use the board `stm32f4discovery`.
Please check the name of the device recognized by your computer.
Our setting is in the file `config.py`.

# How to compile
```
make -j4
```

# How to test and produce benchmarks manually
You can test and produce the benchmarks manually.

## One terminal reading from the board
```
python3 read_serial.py
```

## One terminal flashing as follows

### Test for successful key exchange
```
st-flash write bin/crypto_kem_saber_{m4fspeed, m4fspeedstack, m4fstack}-masked_test.bin 0x8000000
```

### Benchmark for speed
```
st-flash write bin/crypto_kem_saber_{m4fspeed, m4fspeedstack, m4fstack}-masked_{speed, f_speed}.bin 0x8000000
```

### Benchmark for stack
```
st-flash write bin/crypto_kem_saber_{m4fspeed, m4fspeedstack, m4fstack}-masked_stack.bin 0x8000000
```

# Scripts
We also provide scripts for testing and producing the benchmarks.

## Scipt for testing key exchange
```
python3 test.py
```
If the python script returns an error, then the key exchange fails. Otherwise, the key exchange is successful.

## Script for overall cycles for schemes
```
python3 speed.py
```
The numbers will be written into the file `speed.txt`.

## Script for individual functions
```
python3 f_speed.py
```
The numbers will be written into the file `f_speed.txt`.

## Script for stack usage in bytes
```
python3 stack.py
```
The numbers will be written into the file `stack.txt`.

For the interpretations of the numbers, please go to the Section Interpretation of the numbers (`speed.py`, `f_speed.py`, and `stack.py`).

# Interpretation of the numbers (`speed.py`, `f_speed.py`, and `stack.py`)
In this Section, we explain the meaning of the numbers produced by the scripts. Although the explanation is only for numbers by scripts, one can also derive the numbers by manually benchmarking them and there is a one-to-one correspondence between numbers reported by the programs and the scripts. The one-to-one correspondence is the list `testedList` in the beginning of the scripts.

## `speed.py`
Running `python3 speed.py` will report the cycles for the implementations. For the parameter `saber`, we report three different implementations. They are distinguished by the chosen strategy. Each implementation is reported as the following:
```
m4f results for saber (impl={impl})
saber{impl}decaps: XXXk
```

and `impl` is one of the following:
- `m4fspeed-masked`
- `m4fspeedstack-masked`
- `m4fstack-masked`

All of the implementations are reported in our paper.

## `f_speed.py`
Running `python3 f_speed.py` will prduce the benchmarks for NTT-related functions.

- `32-bit NTT`: The cycles of applying one 32-bit NTT over `44683393`.
- `32-bit base_mul`: The cycles of applying one 32-bit `base_mul`.
- `32-bit iNTT`: The cycles of applying one 32-bit iNTT over `44683393`.
- `16-bit NTT`: The cycles of applying one 16-bit NTT over `769`.
- `16-bit base_mul`: The cycles of applying one 16-bit `base_mul`.
- `16-bit iNTT`: The cycles of applying one 16-bit iNTT over `769` .
- `32x16 CRT`: Solving CRT from the moduli `44683393` and `769`. The result is then reduced to signed `mod 8192`.


## `stack.py`
Running `python3 stack.py` will report the overall stack usage (bytes) for the implementations. For the parameter `saber`, we report three different implementations. They are distinguished by the chosen strategy. Each implementation is reported as the following:
```
m4f results for saber (impl={impl})
saber{impl}decaps: XXX
```

and `impl` is one of the following:
- `m4fspeed-masked`
- `m4fspeedstack-masked`
- `m4fstack-masked`

All of the implementations are reported in our paper.









