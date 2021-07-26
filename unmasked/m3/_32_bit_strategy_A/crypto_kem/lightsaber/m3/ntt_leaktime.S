// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// author: Markus Krausz
// date: 18.03.18

// based on: https://github.com/dilithium-cortexm/dilithium-cortexm/blob/master/dilithium/m3/ntt_leaktime.S
// minor edits: Amin Abdulrahman

.syntax unified

// ##############################
// ##########   NTT    ##########
// ##############################

//CT butterfly with Montgomery reduction  -- SIGNED
.macro ct_butterfly_montg coeff0, coeff1, twiddle, q, q_inv, th, tl
  smull \tl, \th, \coeff1, \twiddle
  mul \coeff1, \tl, \q_inv  // q is -q_inv
  smlal \tl, \th, \coeff1, \q
  sub \coeff1, \coeff0, \th
  add.w \coeff0, \coeff0, \th
.endm

//void ntt_asm_leak(int32_t p[N], const uint32_t twiddles_reord[N]);
.global ntt_asm_leak
.type ntt_asm_leak,%function
.align 2
ntt_asm_leak:
  ptr_p       .req R0
  ptr_twiddle .req R1
  q_inv       .req R2
  q           .req R3
  cntr        .req R4
  coeff0      .req R5
  coeff1      .req R6
  coeff2      .req R7
  coeff3      .req R8
  temp0       .req R9
  temp1       .req R10
  twiddle0    .req R11
  twiddle1    .req R12
  twiddle2    .req R14

  //preserve registers
  push {R4-R11, R14}
  add ptr_twiddle, #4
  //load constants, ptr
  ldr.w q_inv, inv_ntt_asm_smull_q_inv  //-q_inv_signed
  ldr.w q, inv_ntt_asm_smull_q
  //stage 1 and 2
  ldr.w cntr, inv_ntt_asm_smull_64

  ldr twiddle1, [ptr_twiddle, #4]  //z2
  ldr twiddle2, [ptr_twiddle, #8]  //z3
  ldr twiddle0, [ptr_twiddle], #12  //z1
  1:
    ldr.w coeff0, [ptr_p]
    ldr coeff1, [ptr_p, #256]  //64*4
    ldr coeff2, [ptr_p, #512]  //128*4
    ldr coeff3, [ptr_p, #768]  //192*4
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage1
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage1
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage2
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage2

    str coeff1, [ptr_p, #256]
    str coeff2, [ptr_p, #512]
    str coeff3, [ptr_p, #768]
    str coeff0, [ptr_p], #4
    subs cntr, #1
    bne 1b
  sub ptr_p, #256      // on coeff0 again

  //stage 3 and 4
  movw cntr, #16
  ldr twiddle1, [ptr_twiddle, #4]  //z8
  ldr twiddle2, [ptr_twiddle, #8]  //z9
  ldr twiddle0, [ptr_twiddle], #12  //z4
  1:
    ldr.w coeff0, [ptr_p]  //16*4
    ldr.w coeff1, [ptr_p, #64]
    ldr.w coeff2, [ptr_p, #128]
    ldr.w coeff3, [ptr_p, #192]
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage4
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage4

    str.w coeff1, [ptr_p, #64]
    str.w coeff2, [ptr_p, #128]
    str.w coeff3, [ptr_p, #192]
    str coeff0, [ptr_p], #4
    subs cntr, #1
    bne 1b
  add.w ptr_p, ptr_p, #192      //(64-16)*4

  movw cntr, #16
  ldr twiddle1, [ptr_twiddle, #4]  //z10
  ldr twiddle2, [ptr_twiddle, #8]  //z11
  ldr twiddle0, [ptr_twiddle], #12  //z5
  1:
    ldr.w coeff0, [ptr_p]
    ldr.w coeff1, [ptr_p, #64]
    ldr.w coeff2, [ptr_p, #128]
    ldr.w coeff3, [ptr_p, #192]
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage4
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage4

    str.w coeff1, [ptr_p, #64]
    str.w coeff2, [ptr_p, #128]
    str.w coeff3, [ptr_p, #192]  //(16*3-1)*4
    str coeff0, [ptr_p], #4
    subs cntr, #1
    bne 1b
  add ptr_p, ptr_p, #192      //(64-16)*4

  movw cntr, #16

  ldr.w twiddle1, [ptr_twiddle, #4]  //z12
  ldr.w twiddle2, [ptr_twiddle, #8]  //z13
  ldr twiddle0, [ptr_twiddle], #12  //z6
  1:
    ldr.w coeff0, [ptr_p]  //16*4
    ldr.w coeff1, [ptr_p, #64]
    ldr.w coeff2, [ptr_p, #128]
    ldr.w coeff3, [ptr_p, #192]  //(16*3)*4
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage4
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage4
    str.w coeff1, [ptr_p, #64]
    str.w coeff2, [ptr_p, #128]
    str.w coeff3, [ptr_p, #192]
    str coeff0, [ptr_p], #4
    subs cntr, #1
    bne 1b
  add ptr_p, #192      //(64-16)*4

  movw cntr, #16
  ldr.w twiddle1, [ptr_twiddle, #4]  //z14
  ldr.w twiddle2, [ptr_twiddle, #8]  //z15
  ldr twiddle0, [ptr_twiddle], #12  //z7
  1:
    ldr.w coeff0, [ptr_p]  //16*4
    ldr.w coeff1, [ptr_p, #64]
    ldr.w coeff2, [ptr_p, #128]
    ldr.w coeff3, [ptr_p, #192]
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage3
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage4
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage4
    str.w coeff1, [ptr_p, #64]
    str.w coeff2, [ptr_p, #128]
    str.w coeff3, [ptr_p, #192] //(16*3-1)*4
    str coeff0, [ptr_p], #4
    subs cntr, #1
    bne 1b
  sub ptr_p, #832      //(208)*4

  //stage 5 and 6
  movw cntr, #16
  1:
    ldr.w twiddle1, [ptr_twiddle, #4]  //z32, ..., z62
    ldr.w twiddle2, [ptr_twiddle, #8]  //z33, ..., z63
    ldr twiddle0, [ptr_twiddle], #12  //z16, ..., z31

    ldr.w coeff0, [ptr_p]  //4*4
    ldr.w coeff1, [ptr_p, #16]
    ldr.w coeff2, [ptr_p, #32]
    ldr.w coeff3, [ptr_p, #48]  //(4*3)*4
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage6
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage6

    str.w coeff1, [ptr_p, #16]
    str.w coeff2, [ptr_p, #32]
    str.w coeff3, [ptr_p, #48]  //(4*3-1)*4
    str coeff0, [ptr_p], #4

    ldr.w coeff0, [ptr_p]
    ldr.w coeff1, [ptr_p, #16]
    ldr.w coeff2, [ptr_p, #32]
    ldr.w coeff3, [ptr_p, #48]
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage6
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage6

    str.w coeff1, [ptr_p, #16]
    str.w coeff2, [ptr_p, #32]
    str.w coeff3, [ptr_p, #48]
    str coeff0, [ptr_p], #4

    ldr.w coeff0, [ptr_p] //4*4
    ldr.w coeff1, [ptr_p, #16]
    ldr.w coeff2, [ptr_p, #32]
    ldr.w coeff3, [ptr_p, #48]
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage6
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage6

    str.w coeff1, [ptr_p, #16]
    str.w coeff2, [ptr_p, #32]
    str.w coeff3, [ptr_p, #48] //(4*3-1)*4
    str coeff0, [ptr_p], #4

    ldr.w coeff0, [ptr_p]  //4*4
    ldr.w coeff1, [ptr_p, #16]
    ldr.w coeff2, [ptr_p, #32]
    ldr.w coeff3, [ptr_p, #48]  //(4*3)*4
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage5
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage6
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage6

    str.w coeff1, [ptr_p, #16]
    str.w coeff2, [ptr_p, #32]
    str.w coeff3, [ptr_p, #48]
    str coeff0, [ptr_p], #52

    subs.w cntr, cntr, #1
    bne 1b
  sub ptr_p, #1024      //256*4

  //stage 7 and 8
  mov cntr, #64
  1:

    ldr.w twiddle1, [ptr_twiddle, #4]  //z128,..., z254
    ldr.w twiddle2, [ptr_twiddle, #8]  //z129,..., z255
    ldr twiddle0, [ptr_twiddle], #12  //z64, ..., z127
    ldr.w coeff0, [ptr_p]  //1*4
    ldr.w coeff1, [ptr_p, #4]
    ldr.w coeff2, [ptr_p, #8]
    ldr.w coeff3, [ptr_p, #12]  //3*4
    ct_butterfly_montg coeff0, coeff2, twiddle0, q, q_inv, temp0, temp1  //stage7
    ct_butterfly_montg coeff1, coeff3, twiddle0, q, q_inv, temp0, temp1  //stage7
    ct_butterfly_montg coeff0, coeff1, twiddle1, q, q_inv, temp0, temp1  //stage8
    ct_butterfly_montg coeff2, coeff3, twiddle2, q, q_inv, temp0, temp1  //stage8

    str.w coeff1, [ptr_p, #4]
    str.w coeff2, [ptr_p, #8]
    str.w coeff3, [ptr_p, #12]
    str coeff0, [ptr_p], #16
    subs cntr, #1
    bne 1b

    //restore registers
    pop {R4-R11, PC}

    //unbind aliases
    .unreq ptr_p
    .unreq ptr_twiddle
    .unreq q_inv
    .unreq q
    .unreq cntr
    .unreq coeff0
    .unreq coeff1
    .unreq coeff2
    .unreq coeff3
    .unreq temp0
    .unreq temp1
    .unreq twiddle0
    .unreq twiddle1
    .unreq twiddle2

inv_ntt_asm_smull_q_inv:
.word 0x979c15ff
.align 2
inv_ntt_asm_smull_q:
.word 25171457
.align 2
inv_ntt_asm_smull_64:
.word 64
