;============================================================================
;
;                 TzRngLib
;
; Copyright (c) 2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
; Qualcomm Technologies Proprietary and Confidential.
;   
;============================================================================

;============================================================================
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/10/15   sm      Initial revision.
;============================================================================

;============================================================================
;                             MODULE EXPORTS
;============================================================================

    EXPORT GetRNGVal
    IMPORT ArmCleanDataCacheEntryByMVA
    IMPORT ArmInvalidateDataCacheEntryByMVA

;============================================================================
;                                 MACROS
;============================================================================

    MACRO
    SMCCALL
100 smc     0x0
    adds    r0, #0         ; Check return value sign
    bgt     %b100          ; If return value was positive redo smc
    MEND

    MACRO
    ZERO_BUFFER
    ldr r0, =BUFFER   ; r0 = Buffer
    mov r2, #0        ; r2 = 0
    str r2, [r0]      ; Buffer = 0
    bl ArmCleanDataCacheEntryByMVA
    MEND

    MACRO
    INVALIDATE_BUFFER
    ldr r0, =BUFFER   ; r0 = Buffer
    bl ArmInvalidateDataCacheEntryByMVA
    MEND

;============================================================================
;                                FUNCTIONS
;============================================================================

    CODE32
    PRESERVE8

    AREA RNG_CODE, ALIGN=2, CODE, READONLY

NUM_BYTES EQU 4

;============================================================================
;
;  UINTN GetRNGVal_armv7 (VOID)//
;
;============================================================================

CMD_ID    EQU 0x02801202 ; (0x2801 << 12) | (2 << 8) | 2

GetRNGVal_armv7 FUNCTION
    ZERO_BUFFER
    ldr r0, =CMD_ID  
    ldr r2, =BUFFER
    mov r3, #NUM_BYTES
    SMCCALL
    cmp r0, #0
    bne %f1
    INVALIDATE_BUFFER
    ldr r1, =BUFFER
    ldr r0, [r1]
1
    pop {pc}
    ENDFUNC

;============================================================================
;
;  UINTN GetRNGVal_armv8 (VOID)//
;
;============================================================================

SMC_ID    EQU 0x02000A01
PARAM_ID  EQU 0x00000022
    
;UINTN GetRNGVal_armv8(VOID)
GetRNGVal_armv8 FUNCTION
    ZERO_BUFFER
    ldr r0, =SMC_ID
    mov r1, #PARAM_ID
    ldr r2, =BUFFER
    mov r3, #NUM_BYTES
    SMCCALL
    cmp r0, #0
    bne %f1
    INVALIDATE_BUFFER
    ldr r1, =BUFFER
    ldr r0, [r1]
1
    pop {pc}
    ENDFUNC

;============================================================================
;
;  UINTN GetRNGVal (VOID)//
;
;============================================================================

IS_ARMV8_SMC_ID   EQU 0x02000601
IS_ARMV8_PARAM_ID EQU 0x00000001
IS_ARMV8_PARAM    EQU 0x02000602

; UINTN GetRNGVal(VOID)
GetRNGVal FUNCTION
    push {lr}           ; Use tail recursion
    ldr r0, =JUMP_OFFSET
    ldr r0, [r0]
    adr r1, jumptable
    ldr pc, [r1, r0]
jumptable
    dcd check_armv8
    dcd GetRNGVal_armv7
    dcd GetRNGVal_armv8
check_armv8
    ldr r0, =IS_ARMV8_SMC_ID
    mov r1, #IS_ARMV8_PARAM_ID
    ldr r2, =IS_ARMV8_PARAM
    SMCCALL
    cmp r0, #0
    ldr r1, =JUMP_OFFSET
    bne is_armv7
    mov r0, #8 ; Set jump offset to GetRNGVal_armv8
    str r0, [r1]
    b GetRNGVal_armv8
is_armv7
    mov r0, #4 ; Set jump offset to GetRNGVal_armv7
    str r0, [r1]
    b GetRNGVal_armv7
    ENDFUNC

//============================================================================
//                                 DATA
//============================================================================

    AREA GLOBAL_DATA, ALIGN=6, DATA
BUFFER
    dcd 0
JUMP_OFFSET
    dcd 0

    END
