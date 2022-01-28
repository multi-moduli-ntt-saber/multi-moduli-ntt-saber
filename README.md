
This repository accompanies the paper **Multi-moduli NTTs for Saber on Cortex-M3 and Cortex-M4** available at https://tches.iacr.org/index.php/TCHES/article/view/9292/8858.

Authors:
- Amin Abdulrahman `amin.abdulrahman@rub.de`
- Jiun-Peng Chen `jpchen@citi.sinica.edu.tw`
- Yu-Jia Chen `yujia@email.ikv- tech.com.tw`
- Vincent Hwang `vincentvbh7@gmail.com`
- [Matthias J. Kannwischer](https://kannwischer.eu/) `<matthias@kannwischer.eu>`
- [Bo-Yin Yang](https://homepage.iis.sinica.edu.tw/pages/byyang/) `<by@crypto.tw>`

In this repository, we provide code for unmasked Saber on Cortex-M3 and Cortex-M4 and masked Saber on Cortex-M4. Please go to `libopencm3`, and type `make -j4` first.
If you acquire this package by downloading the artifact, then this package contains all files required.
If you are github user, please clone with
```
git clone --recursive https://github.com/multi-moduli-ntt-saber/multi-moduli-ntt-saber.git
```

Our contribution is listed as follows.

- Unmasked Saber.
    - Cortex-M3. We provide four implementations. Three of them are reported in our paper and the newly included one is claimed in our paper.
        - `m3speed`: 16-bit NTT approach with strategy A for KeyGen, Encap, and Decap. This is the fastest approach for Saber on Cortex-M3.
        - `m3speedstack`: 16-bit NTT approach with strategy B for KeyGen, and strategy C for Encap and Decap. This is the one claimed in our paper and we provide the benchmarks now.
        - `m3stack`: 16-bit NTT approach with strategy D for KeyGen, Encap, and Decap. This is the implementation with smallest stack usage for Saber on Cortex-M3.
        - `m3_32bit`: 32-bit NTT approach with strategy A for KeyGen, Encap, and Decap.
    - Cortex-M4. We provide two implementations. All of them are reported in our paper.
        - `m4fspeed`: 32-bit NTT approach with strategy A for KeyGen, Encap, and Decap. This is the fastest approach for Saber on Cortex-M4.
        - `m4fstack`: Our multi-moduli approach achieving a new record on the stack usage on Cortex-M4. This is the implementation with the smallest stack usage for Saber on Cortex-M4.
- Masked Saber. Our multiplier is a generic multiplier for Saber and can be used for any masking order. However, we only provide experinments with the first-order masked Saber. The non-linear parts are from https://eprint.iacr.org/2020/733.
    - Cortex-M4. We provide three implementations and only focus on the decapsulation for this part. All of them are multi-moduli approaches reported in our paper.
        - `m4speed`: Strategy A. This is the fastest approach for masked Saber on Cortex-M4.
        - `m4speedstack`: Strategy C.
        - `m4stack`: Strategy D. This is the implementations with the smallest stack usage for Saber on Cortex-M4.

Additionally, we also provide C functions that are used for developing the assembly code and generating the tables.

# Structure of this repository
- `unmasked` contains our implementations for unmasked Saber.
- `masked` contains our implementations for masked Saber.
- `gen_table` contains the files for generating the tables.
- `examples` contains examples about how we use the code in `gen_table` for developing assembly code.


# Requirements
- `arm-none-eabi-gcc` with version 10.2.0
- `python3` with `pyserial` and `numpy`
- For Cortex-M3:
    - Board `nucleo-f207zg`
    - `openocd` version 0.11.0, see https://openocd.org/openocd-0-11-0-released.html
- For Cortex-M4F:
    - Board `stm32f4discovery` (we use floating-point registers)
    - `stlink`

# `libopencm3`
The `libopencm3` in this repository is the commit `6763681c260cf280487d70ca0d1996a8b79fff30` of https://github.com/libopencm3/libopencm3.

# License
Several masked non-linear functions for masked Saber in the folder `masked` are covered by MIT license by https://github.com/KULeuven-COSIC/SABER-masking. The MIT license is added at the beginning of the files. `libopencm3` is licensed under GPL version 3, see https://github.com/libopencm3/libopencm3.

All remaining code is covered by CC0.








