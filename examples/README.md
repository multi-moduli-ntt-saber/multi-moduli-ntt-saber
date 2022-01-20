

# Examples
This folder contains examples of how one can develope assembly implementations with the aid of the C functions.
Examples are written in the file `test.c`. Hope it will be useful for you while testing different strategies for your desired platforms.

Since examples are based on Cortex-M4 implementations, this folder contains all the files for cross-compiling the code.
Furthermore, unless stated otherwise, `NTT`s and `iNTT`s are all implemented with CT butterflies. Our C functions can also generate twiddle factors used for GS butterflies by using `CT` for the `iNTT` with GS butterflies and `inv_CT` for the `NTT` with GS butterflies.

# Requirements
- `arm-none-eabi-gcc` with version 10.2.0
- Board `stm32f4discovery` (we use floating-point registers)
- `stlink`





