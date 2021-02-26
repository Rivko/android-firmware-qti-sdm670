//
// Copyright (c) 2014, 2016 Qualcomm Technologies, Inc. All rights reserved.
// Portions Copyright (c) 2011 - 2014 ARM LTD. All rights reserved.<BR>
// Portion of Copyright (c) 2014 NVIDIA Corporation. All rights reserved.<BR>
// Portions Copyright (c) 2016 HP Development Company, L.P.
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

#include <Chipset/AArch64.h>
#include <Library/PcdLib.h>
#include <AsmMacroIoLibV8.h>
#include <Protocol/DebugSupport.h> // for exception type definitions

/*
  This is the stack constructed by the exception handler (low address to high address).
  X0 to FAR makes up the EFI_SYSTEM_CONTEXT for AArch64.

  UINT64  X0;     0x000
  UINT64  X1;     0x008
  UINT64  X2;     0x010
  UINT64  X3;     0x018
  UINT64  X4;     0x020
  UINT64  X5;     0x028
  UINT64  X6;     0x030
  UINT64  X7;     0x038
  UINT64  X8;     0x040
  UINT64  X9;     0x048
  UINT64  X10;    0x050
  UINT64  X11;    0x058
  UINT64  X12;    0x060
  UINT64  X13;    0x068
  UINT64  X14;    0x070
  UINT64  X15;    0x078
  UINT64  X16;    0x080
  UINT64  X17;    0x088
  UINT64  X18;    0x090
  UINT64  X19;    0x098
  UINT64  X20;    0x0a0
  UINT64  X21;    0x0a8
  UINT64  X22;    0x0b0
  UINT64  X23;    0x0b8
  UINT64  X24;    0x0c0
  UINT64  X25;    0x0c8
  UINT64  X26;    0x0d0
  UINT64  X27;    0x0d8
  UINT64  X28;    0x0e0
  UINT64  FP;     0x0e8   // x29 - Frame Pointer
  UINT64  LR;     0x0f0   // x30 - Link Register
  UINT64  SP;     0x0f8   // x31 - Stack Pointer

  // FP/SIMD Registers. 128bit if used as Q-regs.
  UINT64  V0[2];  0x100
  UINT64  V1[2];  0x110
  UINT64  V2[2];  0x120
  UINT64  V3[2];  0x130
  UINT64  V4[2];  0x140
  UINT64  V5[2];  0x150
  UINT64  V6[2];  0x160
  UINT64  V7[2];  0x170
  UINT64  V8[2];  0x180
  UINT64  V9[2];  0x190
  UINT64  V10[2]; 0x1a0
  UINT64  V11[2]; 0x1b0
  UINT64  V12[2]; 0x1c0
  UINT64  V13[2]; 0x1d0
  UINT64  V14[2]; 0x1e0
  UINT64  V15[2]; 0x1f0
  UINT64  V16[2]; 0x200
  UINT64  V17[2]; 0x210
  UINT64  V18[2]; 0x220
  UINT64  V19[2]; 0x230
  UINT64  V20[2]; 0x240
  UINT64  V21[2]; 0x250
  UINT64  V22[2]; 0x260
  UINT64  V23[2]; 0x270
  UINT64  V24[2]; 0x280
  UINT64  V25[2]; 0x290
  UINT64  V26[2]; 0x2a0
  UINT64  V27[2]; 0x2b0
  UINT64  V28[2]; 0x2c0
  UINT64  V29[2]; 0x2d0
  UINT64  V30[2]; 0x2e0
  UINT64  V31[2]; 0x2f0

  // System Context
  UINT64  ELR;    0x300   // Exception Link Register
  UINT64  SPSR;   0x308   // Saved Processor Status Register
  UINT64  FPSR;   0x310   // Floating Point Status Register
  UINT64  ESR;    0x318   // Exception syndrome register
  UINT64  FAR;    0x320   // Fault Address Register
  UINT64  Padding;0x328   // Required for stack alignment
*/

GCC_ASM_EXPORT(SaveCpuCtxt)
GCC_ASM_IMPORT(SaveUefiContext)

.text

#define GP_CONTEXT_SIZE    (32 *  8) 
#define FP_CONTEXT_SIZE    (32 * 16)
#define SYS_CONTEXT_SIZE   ( 6 *  8) // 5 SYS regs + Alignment requirement (ie: the stack must be aligned on 0x10)

ASM_PFX(SaveCpuCtxt):
  // Make room for FP and SYS regs on the stack
  sub sp, sp, #(FP_CONTEXT_SIZE + SYS_CONTEXT_SIZE)

  // Make room and push GP registers on the stack
  stp      x0, x1, [sp, #-GP_CONTEXT_SIZE]!
  stp      x2, x3, [sp, #0x10]
  stp      x4, x5, [sp, #0x20]
  stp      x6, x7, [sp, #0x30]
  stp      x8,  x9,  [sp, #0x40]
  stp      x10, x11, [sp, #0x50]
  stp      x12, x13, [sp, #0x60]
  stp      x14, x15, [sp, #0x70]
  stp      x16, x17, [sp, #0x80]
  stp      x18, x19, [sp, #0x90]
  stp      x20, x21, [sp, #0xa0]
  stp      x22, x23, [sp, #0xb0]
  stp      x24, x25, [sp, #0xc0]
  stp      x26, x27, [sp, #0xd0]
  stp      x28, x29, [sp, #0xe0]

  mov      x27, sp
  stp      x30, x27,  [sp, #0xf0]

  // Advance pointer after saving the GP registers
  add      x27, sp, #GP_CONTEXT_SIZE + FP_CONTEXT_SIZE + SYS_CONTEXT_SIZE

  // Back up CPSR to SPSR in dumps
  mrs      x26, NZCV 

  // Back up PC to ELR in dumps
  adr      x1, .

  // Push SYS regs and advance pointer
  stp      x1,   x26,  [x27, #-SYS_CONTEXT_SIZE]!
  stp      xzr,  xzr,  [x27, #0x10]
  str      xzr,  [x27, #0x20]

  // Push FP regs and advance pointer 
  stp      q0,  q1,  [x27, #-FP_CONTEXT_SIZE]!
  stp      q2,  q3,  [x27, #0x20]
  stp      q4,  q5,  [x27, #0x40]
  stp      q6,  q7,  [x27, #0x60]
  stp      q8,  q9,  [x27, #0x80]
  stp      q10, q11, [x27, #0xa0]
  stp      q12, q13, [x27, #0xc0]
  stp      q14, q15, [x27, #0xe0]
  stp      q16, q17, [x27, #0x100]
  stp      q18, q19, [x27, #0x120]
  stp      q20, q21, [x27, #0x140]
  stp      q22, q23, [x27, #0x160]
  stp      q24, q25, [x27, #0x180]
  stp      q26, q27, [x27, #0x1a0]
  stp      q28, q29, [x27, #0x1c0]
  stp      q30, q31, [x27, #0x1e0]

  // Call SaveUefiContext
  mov      x0, sp
  bl       ASM_PFX(SaveUefiContext)

  // Restore registers
  ldp      x0,  x1,  [sp]
  ldp      x2,  x3,  [sp, #0x10]
  ldp      x4,  x5,  [sp, #0x20]
  ldp      x6,  x7,  [sp, #0x30]
  ldp      x8,  x9,  [sp, #0x40]
  ldp      x10, x11, [sp, #0x50]
  ldp      x12, x13, [sp, #0x60]
  ldp      x14, x15, [sp, #0x70]
  ldp      x16, x17, [sp, #0x80]
  ldp      x18, x19, [sp, #0x90]
  ldp      x20, x21, [sp, #0xa0]
  ldp      x22, x23, [sp, #0xb0]
  ldp      x24, x25, [sp, #0xc0]
  ldp      x26, x27, [sp, #0xd0]
  ldp      x28, x29, [sp, #0xe0]
  ldr      x30, [sp, #0xf0]
  ret

