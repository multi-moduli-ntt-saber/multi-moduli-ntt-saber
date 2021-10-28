
This repository accompanies the paper **Multi-moduli NTTs for Saber on Cortex-M3 and Cortex-M4** available at https://eprint.iacr.org/2021/995.pdf.

Authors:
- Amin Abdulrahman `amin.abdulrahman@rub.de`
- Jiun-Peng Chen `jpchen@citi.sinica.edu.tw`
- Yu-Jia Chen `yujia@email.ikv- tech.com.tw`
- Vincent Hwang `vincentvbh7@gmail.com`
- [Matthias J. Kannwischer](https://kannwischer.eu/) `<matthias@kannwischer.eu>`
- [Bo-Yin Yang](https://homepage.iis.sinica.edu.tw/pages/byyang/) `<by@crypto.tw>`

In this repository, we provide code for unmasked Saber on Cortex-M3 and Cortex-M4 and masked Saber on Cortex-M4. Please `git clone --recursive https://github.com/multi-moduli-ntt-saber/multi-moduli-ntt-saber.git`. You might want to go to `libopencm3` and `make -j4` first.

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

We also provide C functions that are used for developing the assembly code and generating the tables. Please refer to the folder `gen_table`. You can find examples of how the tables are generated.

# Requirements

- `libopencm3`. You can just `sh setup_libopencm3.sh`
- `arm-none-eabi-gcc` with version 10.2.0
- `python3` with `pyserial`
- For Cortex-M3:
    - Board `nucleo-f207zg`
    - `openocd`
- For Cortex-M4:
    - Board `stm32f4discovery`
    - `stlink`



