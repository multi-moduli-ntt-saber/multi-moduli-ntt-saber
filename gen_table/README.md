
This folder is about how to generate the tables of twiddle factors.
The codes are currently functioning as expected and samples for generating tables for our implementations are in the corresponding folders. Documentation will be added in the future.

# Supported features

The C functions are much stronger than the ones in our paper.
In particular, the followings are supported.

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





