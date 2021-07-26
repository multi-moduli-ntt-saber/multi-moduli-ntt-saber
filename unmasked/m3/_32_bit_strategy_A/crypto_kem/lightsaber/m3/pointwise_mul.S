// based on: https://github.com/dilithium-cortexm/dilithium-cortexm/blob/master/dilithium/m3/pointwise_mul.S

.syntax unified
.thumb

// Schoolbook long multiplication (smull_const, reordered) 
// INPUT:   a = 2^16 a1 (signed) + a0 (unsigned),
//          b = 2^16 b1 (signed) + b0 (unsigned),
//          c = 2^32 c1 (signed) + c0 (unsigned)
// OUTPUT:  c = a * b = 2^32 c1 (signed) + c0 (unsigned)
.macro smull_const_reord c0, c1, a0, a1, b0, b1, tmp
  mul     \tmp, \a1, \b0
  mla     \tmp, \a0, \b1, \tmp
  mul     \c0, \a0, \b0
  mul     \c1, \a1, \b1
  adds.w    \c0, \c0, \tmp, lsl #16
  adc.w     \c1, \c1, \tmp, asr #16
.endm
// Schoolbook long multiplication with accumulation (smlal_const)
// INPUT:   a = 2^16 a1 (signed) + a0 (unsigned),
//          b = 2^16 b1 (signed) + b0 (unsigned),
//          c = 2^32 c1 (signed) + c0 (unsigned)
// OUTPUT:  c = a * b = 2^32 c1 (signed) + c0 (unsigned)
.macro smlal_const acc0, acc1, a0, a1, b0, b1, tmp
  mul     \tmp, \a0, \b0
  adds    \acc0, \acc0, \tmp
  mul     \tmp, \a1, \b1
  adc     \acc1, \acc1, \tmp
  mul     \tmp, \a1, \b0
  mla     \tmp, \a0, \b1, \tmp
  adds    \acc0, \acc0, \tmp, lsl #16
  adc     \acc1, \acc1, \tmp, asr #16
.endm

// Montgomery multiplication
// INPUT:   a = 2^16 a_h (signed) + a_l (unsigned), b = 2^16 b_h (signed) + b_l (unsigned),
//          q = 2^16 q_h (signed) + q_l (unsigned), q_inv (signed)
// OUTPUT:  (a * b) mod q in b_h
// R = 2^32
// Variant of the montgomery_mul macro from common.S.
// Here we are allowed to overwrite the contents from a, as well as b
// This is because no twiddle factors need to be preserved
.macro montgomery_mul a_l, a_h, b_l, b_h, q_l, q_h, q_inv, tmp
  // T = a * b
  smull_const_reord  \b_l, \b_h, \a_l, \a_h, \b_l, \b_h, \tmp
  // tmp1 = T_low * q_inv
  mul \a_h, \b_l, \q_inv
  // split result into upper and lower half in order to use in multiplication
  ubfx \a_l, \a_h, #0, #16
  sbfx \a_h, \a_h, #16, #16
  // (/R by taking upper half)
  smlal_const \b_l, \b_h, \a_l, \a_h, \q_l, \q_h, \tmp
.endm

// void pointwise_mul_asm_const(int32_t c[SABER_N], const int32_t a[SABER_N], const int32_t b[SABER_N]);
.global pointwise_mul_asm_const
.type pointwise_mul_asm_const, %function
.align 2
pointwise_mul_asm_const:
  push.w {r4-r9, r14}
  c_ptr .req r0
  a_ptr .req r1
  b_ptr .req r2
  q_inv  .req r3
  q_l    .req r4
  q_h    .req r5
  coeff_a_l   .req r6
  coeff_a_h   .req r7
  coeff_b_l   .req r8
  coeff_b_h   .req r9
  temp0  .req r14
  ctr   .req r12

  // q = 0x1801601
  movw q_l, #0x1601
  movw q_h, #0x180
  movw ctr, #256
  // q_inv = 0x979c15ff
  movw q_inv, #:lower16:0x979c15ff
  movt q_inv, #:upper16:0x979c15ff
  1:
    // load as radix-2^16
    ldrsh.w coeff_a_h, [a_ptr, #2]
    ldrh coeff_a_l, [a_ptr], #4
    ldrsh.w coeff_b_h, [b_ptr, #2]
    ldrh coeff_b_l, [b_ptr], #4
    
    montgomery_mul coeff_a_l, coeff_a_h, coeff_b_l, coeff_b_h, q_l, q_h, q_inv, temp0

    str coeff_b_h, [c_ptr], #4

    subs.w ctr, ctr, #1
    bne.w 1b
    pop.w {r4-r9, pc}

// void pointwise_mul_asm_const_acc(int32_t c[SABER_N], const int32_t a[SABER_N], const int32_t b[SABER_N]);
.global pointwise_mul_asm_const_acc
.type pointwise_mul_asm_const_acc, %function
.align 2
pointwise_mul_asm_const_acc:
  push.w {r4-r10, r14}
  c_ptr       .req r0
  a_ptr       .req r1
  b_ptr       .req r2
  q_inv       .req r3
  q_l         .req r4
  q_h         .req r5
  coeff_a_l   .req r6
  coeff_a_h   .req r7
  coeff_b_l   .req r8
  coeff_b_h   .req r9
  coeff_c     .req r10
  temp0       .req r14
  ctr         .req r12

  // q = 0x1801601
  movw q_l, #0x1601
  movw q_h, #0x180
  movw ctr, #256
  // q_inv = 0x979c15ff
  movw q_inv, #:lower16:0x979c15ff
  movt q_inv, #:upper16:0x979c15ff
  1:
    // load as radix-2^16
    ldrsh.w coeff_a_h, [a_ptr, #2]
    ldrh coeff_a_l, [a_ptr], #4
    ldrsh.w coeff_b_h, [b_ptr, #2]
    ldrh coeff_b_l, [b_ptr], #4
    ldr.w coeff_c, [c_ptr]

    montgomery_mul coeff_a_l, coeff_a_h, coeff_b_l, coeff_b_h, q_l, q_h, q_inv, temp0

    add.w coeff_b_h, coeff_b_h, coeff_c
    str coeff_b_h, [c_ptr], #4

    subs.w ctr, ctr, #1
    bne.w 1b
  pop.w {r4-r10, pc}

// -q^(-1) mod 2^32
.align 2
pointwise_mul_asm_const_q_inv:
.word 0x979c15ff