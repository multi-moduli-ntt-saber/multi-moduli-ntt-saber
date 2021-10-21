# include "common.S"
.syntax unified
// based on: https://github.com/dilithium-cortexm/dilithium-cortexm/blob/master/dilithium/m3/ntt1_asm.S

// void ntt_asm_const(int32_t p[SABER_N], const uint32_t twiddles[SABER_N]);
.global ntt_asm_const
.type ntt_asm_const,%function
.align 2
ntt_asm_const:
  ptr_p       .req R0
  ptr_twiddle .req R1
  q_h         .req R2
  q_l         .req R3
  q_inv       .req R4
  coeff0      .req R5
  coeff1_h    .req R6
  coeff1_l    .req R7
  temp0       .req R8
  temp1       .req R9
  temp2       .req R10
  twiddle_h   .req R11
  twiddle_l   .req R12

  // preserve registers
  push {r4-r11}
  // load q and q inverse
  // q_inv = 0x979c15ff
  movw q_inv, #:lower16:0x979c15ff
  movt q_inv, #:upper16:0x979c15ff
  // q = 0x1801601
  movw q_l, #0x1601
  movw q_h, #0x180

  add ptr_twiddle, #4

  // layer_7_0
  load_twiddle
  .rept 128
  wrap_ct_butterfly 512, 0
  .endr
  sub ptr_p, #512
  // layer_6_0
  load_twiddle
  .rept 64
  wrap_ct_butterfly 256, 0
  .endr
  add ptr_p, #256
  // layer_6_1
  load_twiddle
  .rept 64
  wrap_ct_butterfly 256, 0
  .endr
  sub ptr_p, #768
  // layer_5_0
  load_twiddle
  .rept 31
  wrap_ct_butterfly 128, 0
  .endr
  wrap_ct_butterfly 128, 128
  // layer_5_1
  load_twiddle
  .rept 31
  wrap_ct_butterfly 128, 0
  .endr
  wrap_ct_butterfly 128, 128
  // layer_5_2
  load_twiddle
  .rept 31
  wrap_ct_butterfly 128, 0
  .endr
  wrap_ct_butterfly 128, 128
  // layer_5_3
  load_twiddle
  .rept 32
  wrap_ct_butterfly 128, 0
  .endr
  sub ptr_p, #896
  // layer_4_0
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_1
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_2
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_3
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_4
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_5
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_6
  load_twiddle
  .rept 15
  wrap_ct_butterfly 64, 0
  .endr
  wrap_ct_butterfly 64, 64
  // layer_4_7
  load_twiddle
  .rept 16
  wrap_ct_butterfly 64, 0
  .endr
  sub ptr_p, #960
  // layer_3_0
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_1
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_2
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_3
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_4
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_5
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_6
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_7
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_8
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_9
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_10
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_11
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_12
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_13
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_14
  load_twiddle
  .rept 7
  wrap_ct_butterfly 32, 0
  .endr
  wrap_ct_butterfly 32, 32
  // layer_3_15
  load_twiddle
  .rept 8
  wrap_ct_butterfly 32, 0
  .endr
  sub ptr_p, #992
  // layer_2_0
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_1
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_2
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_3
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_4
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_5
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_6
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_7
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_8
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_9
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_10
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_11
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_12
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_13
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_14
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_15
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_16
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_17
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_18
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_19
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_20
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_21
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_22
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_23
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_24
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_25
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_26
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_27
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_28
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_29
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_30
  load_twiddle
  .rept 3
  wrap_ct_butterfly 16, 0
  .endr
  wrap_ct_butterfly 16, 16
  // layer_2_31
  load_twiddle
  .rept 4
  wrap_ct_butterfly 16, 0
  .endr
  sub ptr_p, #1008
  // layer_1_0
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_1
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_2
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_3
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_4
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_5
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_6
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_7
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_8
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_9
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_10
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_11
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_12
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_13
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_14
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_15
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_16
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_17
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_18
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_19
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_20
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_21
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_22
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_23
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_24
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_25
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_26
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_27
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_28
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_29
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_30
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_31
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_32
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_33
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_34
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_35
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_36
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_37
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_38
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_39
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_40
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_41
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_42
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_43
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_44
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_45
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_46
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_47
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_48
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_49
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_50
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_51
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_52
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_53
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_54
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_55
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_56
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_57
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_58
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_59
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_60
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_61
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_62
  load_twiddle
  .rept 1
  wrap_ct_butterfly 8, 0
  .endr
  wrap_ct_butterfly 8, 8
  // layer_1_63
  load_twiddle
  .rept 2
  wrap_ct_butterfly 8, 0
  .endr
  sub ptr_p, #1016
  // layer_0_0
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_1
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_2
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_3
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_4
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_5
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_6
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_7
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_8
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_9
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_10
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_11
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_12
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_13
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_14
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_15
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_16
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_17
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_18
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_19
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_20
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_21
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_22
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_23
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_24
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_25
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_26
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_27
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_28
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_29
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_30
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_31
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_32
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_33
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_34
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_35
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_36
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_37
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_38
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_39
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_40
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_41
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_42
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_43
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_44
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_45
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_46
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_47
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_48
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_49
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_50
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_51
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_52
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_53
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_54
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_55
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_56
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_57
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_58
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_59
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_60
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_61
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_62
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_63
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_64
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_65
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_66
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_67
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_68
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_69
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_70
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_71
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_72
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_73
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_74
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_75
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_76
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_77
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_78
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_79
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_80
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_81
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_82
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_83
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_84
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_85
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_86
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_87
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_88
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_89
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_90
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_91
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_92
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_93
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_94
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_95
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_96
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_97
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_98
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_99
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_100
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_101
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_102
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_103
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_104
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_105
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_106
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_107
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_108
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_109
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_110
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_111
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_112
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_113
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_114
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_115
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_116
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_117
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_118
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_119
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_120
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_121
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_122
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_123
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_124
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_125
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_126
  load_twiddle
  wrap_ct_butterfly 4, 4
  // layer_0_127
  load_twiddle
  .rept 1
  wrap_ct_butterfly 4, 0
  .endr

  //restore registers
  pop {r4-r11}
  mov PC, r14

  //unbind aliases
  .unreq ptr_p
  .unreq ptr_twiddle
  .unreq q_inv
  .unreq q_h
  .unreq coeff0
  .unreq coeff1_h
  .unreq coeff1_l
  .unreq q_l
  .unreq temp0
  .unreq temp1
  .unreq twiddle_h
  .unreq twiddle_l
  .unreq temp2
