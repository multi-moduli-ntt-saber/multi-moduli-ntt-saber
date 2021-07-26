
This repository accompanies the paper **Multi-moduli NTTs for Saber on Cortex-M3 and Cortex-M4** available at XXX.

Authors:
- Amin Abdulrahman `amin.abdulrahman@rub.de`
- Jiun-Peng Chen `jpchen@citi.sinica.edu.tw`
- Yu-Jia Chen `yujia@email.ikv- tech.com.tw`
- Vincent Hwang `vincentvbh7@gmail.com`
- [Matthias J. Kannwischer](https://kannwischer.eu/) `<matthias@kannwischer.eu>`
- [Bo-Yin Yang](https://homepage.iis.sinica.edu.tw/pages/byyang/) `<by@crypto.tw>`

To set up `libopencm3`, execute `sh setup_libopencm3.sh`.

For building implementations, please refer to the `README.md`'s in the folders `masked` and `unmasked`.

```
.
├── README.md
├── masked
│   ├── README.md
│   └── m4
│       ├── README.md
│       ├── strategy_A
│       ├── strategy_C
│       └── strategy_D
└── unmasked
    ├── README.md
    ├── m3
    │   ├── README.md
    │   ├── _16_bit_strategy_A
    │   ├── _16_bit_strategy_D
    │   └── _32_bit_strategy_A
    └── m4
        ├── README.md
        ├── strategy_A
        └── strategy_D
```

