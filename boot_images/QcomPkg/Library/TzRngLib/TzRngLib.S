//============================================================================
//
//            TzRngLib
//
// GENERAL DESCRIPTION
//   TZ RNG Functions
//
// Copyright 2015 by Qualcomm Technologies, Inc.  All Rights Reserved 
//============================================================================

//============================================================================
//
//                       EDIT HISTORY FOR MODULE
//
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
//
// when      who     what, where, why
// --------  -----   ----------------------------------------------------------
// 03/11/15  sm      Initial revision.
//============================================================================

//============================================================================
//                             MODULE EXPORTS
//============================================================================

  GCC_ASM_EXPORT(GetRNGVal)

  GCC_ASM_IMPORT(ArmCleanDataCacheEntryByMVA)
  GCC_ASM_IMPORT(ArmInvalidateDataCacheEntryByMVA)

//============================================================================
//                                 MACROS 
//============================================================================

#define FuncPrologue                        \
    stp           x29, x30, [sp, #-0x10]!;  \
    mov           x29,sp


#define FuncEpilogue                        \
    ldp           x29, x30, [sp], #0x10  ;  \
    ret

#define SmcCall(func)                 \
smc_loop_##func:                  ;   \
    smc           #0x0            ;   \
    cmp           w0, #0          ;   \
    b.gt          smc_loop_##func

#define ZeroBuffer              \
    ldr x0, =BUFFER           ; \
    str xzr, [x0]             ; \
    bl ArmCleanDataCacheEntryByMVA ; \
    dsb     sy ; \
    isb

#define InvalidateBuffer        \
    ldr x0, =BUFFER           ; \
    bl ArmInvalidateDataCacheEntryByMVA ; \
    dsb     sy  ; \
    isb



//============================================================================
//                               FUNCTIONS 
//============================================================================

.text
.align 3

#define NUM_BYTES 8

//============================================================================
//
//  UINTN GetRNGVal_armv7 (VOID)//
//
//============================================================================

#define CMD_ID    0x02801202 // (0x2801 << 12) | (2 << 8) | 2

GetRNGVal_armv7:
    ZeroBuffer
    ldr x0, =CMD_ID
    ldr x2, =BUFFER
    mov x3, #NUM_BYTES
    SmcCall(armv7)
    cmp w0, #0
    b.ne armv7_fail
    InvalidateBuffer
    ldr x1, =BUFFER
    ldr x0, [x1]
armv7_fail:
    FuncEpilogue

//============================================================================
//
//  UINTN GetRNGVal_armv8 (VOID)//
//
//============================================================================

#define SMC_ID    0x02000A01
#define PARAM_ID  0x00000022
    
GetRNGVal_armv8:
    ZeroBuffer
    ldr x0, =SMC_ID
    mov x1, #PARAM_ID
    ldr x2, =BUFFER
    mov x3, #NUM_BYTES
    SmcCall(armv8)
    cmp w0, #0
    b.ne armv8_fail
    InvalidateBuffer
    ldr x1, =BUFFER
    ldr x0, [x1]
armv8_fail:
    FuncEpilogue

//============================================================================
//
//  UINTN GetRNGVal (VOID)//
//
//============================================================================

#define IS_ARMV8_SMC_ID   0x02000601
#define IS_ARMV8_PARAM_ID 0x00000001
#define IS_ARMV8_PARAM    0x02000602

GetRNGVal:
    FuncPrologue
    ldr x0, =JUMP_OFFSET
    ldr x0, [x0]
    adr x1, jumptable
    ldr x2, [x1, x0]
    br x2
jumptable:
    .quad check_armv8
    .quad GetRNGVal_armv7
    .quad GetRNGVal_armv8
check_armv8:
    ldr x0, =IS_ARMV8_SMC_ID
    mov x1, #IS_ARMV8_PARAM_ID
    ldr x2, =IS_ARMV8_PARAM
    SmcCall(check_armv8)
    cmp w0, #0
    ldr x1, =JUMP_OFFSET
    b.ne is_armv7
    mov x0, #16 // Set jump offset to GetRNGVal_armv8
    str x0, [x1]
    b GetRNGVal_armv8
is_armv7:
    mov x0, #8 // Set jump offset to GetRNGVal_armv7
    str x0, [x1]
    b GetRNGVal_armv7

//============================================================================
//                                   DATA
//============================================================================

   .data
   .align 6
BUFFER:
    .quad 0
JUMP_OFFSET:
    .quad 0
