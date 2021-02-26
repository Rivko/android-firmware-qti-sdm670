# 1 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/startup.s"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 324 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/startup.s" 2
;=========================================================================
;
;*
; This file contains the implementation for AOP to boot
;
;@par EXTERNALIZED FUNCTIONS
; (none)
;
;@par INITIALIZATION AND SEQUENCING REQUIREMENTS
; (none)
;
;Copyright (C) 2014 QUALCOMM Technologies Incorporated.
;All Rights Reserved. QUALCOMM Proprietary and Confidential.
;
;*

;=========================================================================
; Edit History
;$Header:
;$Date: 2018/02/16 $
;
; when who what, where, why
; -------- --- ---------------------------------------------------
;
;=========================================================================
        ;; IMPORT ||Image$$CODE_RAM_SAVE$$Base||

;-------------------------------------------------------------------------
; DATA
;-------------------------------------------------------------------------
# 44 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/startup.s"
                .extern __aop_stack_limit
                .section .text, "a", %progbits
                .p2align 2
__Vectors: ; Minimal Vector Table entries at 0x0 for boot
                .word __aop_stack_limit
                                        ; Initial Main SP
                .word Reset_Handler ; Reset Handler



;-------------------------------------------------------------------------
; FUNCTION DEFINITIONS
;-------------------------------------------------------------------------
# 71 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/startup.s"
                .section .text, "ax", %progbits
                .global Reset_Handler
                .type Reset_Handler, %function
                .code 16
Reset_Handler:
                .global Reset_Handler
                .extern __main
                .extern rex_init

                LDR R0, =rex_init
                BLX R0
                LDR R0, =__main
                BX R0

                .end
