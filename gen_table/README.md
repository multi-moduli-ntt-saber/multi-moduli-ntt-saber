
This folder is about how to generate the tables of twiddle factors.
The codes are currently functioning as expected and samples for generating tables for our implementations are in the corresponding folders.

# Supported features

The C functions support the follows.

> Customized coefficient ring.

> Customized strategy for generating twiddle factors.

> Customized and separable NTT call(s) in C to access memory exactly as what is planned to be later implemented in assembly.

## Customized coefficient ring

One has to provide the defining operations of a commutative ring:
```
void addmod(void*, void*, void*, void*);
void submod(void*, void*, void*, void*);
void mulmod(void*, void*, void*, void*);
void expmod(void*, void*, size_t, void*);
```

## Customized strategy for generating twiddle factors

> Merging layers.

> Optional padding.

## Customized and separable NTT call(s)

> Can be interleaved freely with assembly functions for testing units.

# Requirement
A C compiler

# Reserved symbols
```
ARRAY_N
NTT_N
LOGNTT_N
```

Notice that `NTT_N` must be a power of 2 and `NTT_N = 1 << LOGNTT_N`.
Furthermore, `NTT_N` must divide `ARRAY_N`.
However, `ARRAY_N` need not to be a power of 2.

```
.
├── README.md
├── common
│   ├── gen_table.c
│   ├── gen_table.h
│   ├── naive_mult.c
│   ├── naive_mult.h
│   ├── ntt_c.c
│   ├── ntt_c.h
│   ├── tools.c
│   └── tools.h
├── m3_unmasked
│   ├── Makefile
│   ├── NTT_params.h
│   ├── call_gen.c
│   ├── gen_table.c -> ../common/gen_table.c
│   ├── gen_table.h -> ../common/gen_table.h
│   ├── tools.c -> ../common/tools.c
│   └── tools.h -> ../common/tools.h
├── m4_masked
│   ├── Makefile
│   ├── NTT_params.h
│   ├── call_gen.c
│   ├── gen_table.c -> ../common/gen_table.c
│   ├── gen_table.h -> ../common/gen_table.h
│   ├── tools.c -> ../common/tools.c
│   └── tools.h -> ../common/tools.h
└── m4_unmasked
    ├── Makefile
    ├── NTT_params.h
    ├── call_gen.c
    ├── gen_table.c -> ../common/gen_table.c
    ├── gen_table.h -> ../common/gen_table.h
    ├── tools.c -> ../common/tools.c
    └── tools.h -> ../common/tools.h
```




