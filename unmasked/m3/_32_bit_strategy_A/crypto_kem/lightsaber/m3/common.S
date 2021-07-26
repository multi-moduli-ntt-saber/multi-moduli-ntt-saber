// based on: https://github.com/dilithium-cortexm/dilithium-cortexm/blob/master/dilithium/m3/ntt1_asm.S

/* COMMON ALL */

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

// Schoolbook long multiplication (smull_const)
// INPUT:   a = 2^16 a1 (signed) + a0 (unsigned),
//          b = 2^16 b1 (signed) + b0 (unsigned),
//          c = 2^32 c1 (signed) + c0 (unsigned)
// OUTPUT:  c = a * b = 2^32 c1 (signed) + c0 (unsigned)
.macro smull_const c0, c1, a0, a1, b0, b1, tmp
    mul     \c0, \a0, \b0
    mul     \c1, \a1, \b1
    mul     \tmp, \a1, \b0
    mla     \tmp, \a0, \b1, \tmp
    adds    \c0, \c0, \tmp, lsl #16
    adc     \c1, \c1, \tmp, asr #16
.endm

// Montgomery multiplication
// INPUT:   a = 2^16 a_h (signed) + a_l (unsigned), b = 2^16 b_h (signed) + b_l (unsigned),
//          q = 2^16 q_h (signed) + q_l (unsigned), q_inv (signed)
// OUTPUT:  (a * b) mod q in temp0
// R = 2^32
.macro montgomery_mul a_l, a_h, b_l, b_h, q_l, q_h, q_inv, temp_res, temp0, temp1
    // a * b = 2^32 temp_res + temp0 = (temp_res||temp0)
    smull_const  \temp0, \temp_res, \a_l, \a_h, \b_l, \b_h, \temp1
    // (a*b) * q_inv mod R = temp0 * q_inv mod R
    mul \a_h, \temp0, \q_inv
    // -> radix-2^16
    ubfx \a_l, \a_h, #0, #16
    sbfx \a_h, \a_h, #16, #16
    // ((temp0||temp_res) + (a_h||a_l) * q_prime) / R, /R by taking upper half as the result
    smlal_const \temp0, \temp_res, \a_l, \a_h, \q_l, \q_h, \temp1
.endm


// Load twiddle factor
// This macro loads a twiddle factor in radix-2^16 representation and increases the pointer
.macro load_twiddle
    ldrh twiddle_l,[ptr_twiddle],#2
    ldrh twiddle_h,[ptr_twiddle],#2
.endm

/* COMMON FORWARD */

// Cooley-Tukey Butterfly using montgomery multiplication
// INPUT:   coeff0 (signed), coeff1 = 2^16 coeff1_h (signed) + coeff1_l (unsigned),
//          twiddle = 2^16 twiddle_h (signed) + twiddle_l (unsigned),
//          q = 2^16 q_h (signed) + q_l (unsigned), q_inv (signed)
// OUTPUT:  coeff1_l (signed) = U - V * twiddle
//          coeff0 (signed) = U + V * twiddle
.macro ct_butterfly coeff0, coeff1_l,coeff1_h, twiddle_l, twiddle_h, q_l, q_h, q_inv, temp0, temp1, temp2
    // V * twiddle
    montgomery_mul \coeff1_l, \coeff1_h, \twiddle_l, \twiddle_h, \q_l, \q_h, \q_inv, \temp0, \temp1, \temp2
    // U - V
    sub \coeff1_l, \coeff0, \temp0
    // U + V
    add.w \coeff0, \coeff0, \temp0
.endm

// Wraps the Cooley-Tukey Butterfly
// INPUT:   twiddle = 2^16 twiddle_h (signed) + twiddle_l (unsigned),
//          q = 2^16 q_h (signed) + q_l (unsigned), q_inv (signed),
//          ptr_p (pointer to working array), const, incr
.macro wrap_ct_butterfly const, incr
    // Load coeff0 as 32-bit
    ldr.w coeff0, [ptr_p]
    // Load coeff1 as radix-2^16
    ldrh.w coeff1_l, [ptr_p, #\const]
    ldrsh coeff1_h, [ptr_p, #\const+2]
    ct_butterfly coeff0, coeff1_l, coeff1_h, twiddle_l, twiddle_h, q_l, q_h, q_inv, temp0, temp1, temp2
    // Store and set pointer
    str.w coeff0, [ptr_p],#4+\incr
    // store coeff1 as the result
    str.w coeff1_l, [ptr_p, #\const-\incr-4]
.endm