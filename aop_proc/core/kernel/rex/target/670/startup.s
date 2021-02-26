;=========================================================================
;
;*//** @file startup.s
;  This file contains the implementation for AOP to boot
;
;@par EXTERNALIZED FUNCTIONS
;  (none)
;
;@par INITIALIZATION AND SEQUENCING REQUIREMENTS
;  (none)
;
;Copyright (C) 2014 QUALCOMM Technologies Incorporated.
;All Rights Reserved. QUALCOMM Proprietary and Confidential.
;
;*//*=====================================================================

;=========================================================================
;                             Edit History
;$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/target/670/startup.s#1 $
;$Date: 2018/02/16 $
;
; when       who       what, where, why
; --------   ---       ---------------------------------------------------
;
;=========================================================================
        ;; IMPORT  ||Image$$CODE_RAM_SAVE$$Base|| 

;-------------------------------------------------------------------------
;                           DATA
;-------------------------------------------------------------------------

#if defined __ARMCC_VERSION__
                EXPORT Reset_Handler
                IMPORT ||Image$$ARM_LIB_STACK$$ZI$$Limit||
                AREA    AOP_ENTRY, DATA, READONLY
__Vectors                               ; Minimal Vector Table entries at 0x0 for boot
                DCD     ||Image$$ARM_LIB_STACK$$ZI$$Limit||
                                        ; Initial Main SP
                DCD     Reset_Handler   ; Reset Handler
               ;DCD     NMI_Handler       ?
               ;DCD     HardFault_Handler ?

#elif defined __clang__
                .extern __aop_stack_limit
                .section .text, "a", %progbits
                .p2align 2
__Vectors:                               ; Minimal Vector Table entries at 0x0 for boot
                .word   __aop_stack_limit
                                        ; Initial Main SP
                .word   Reset_Handler   ; Reset Handler

#endif /* __clang__ */

;-------------------------------------------------------------------------
;                           FUNCTION DEFINITIONS
;-------------------------------------------------------------------------

#if defined __ARMCC_VERSION__
                AREA    |.text|, CODE, READONLY
Reset_Handler
                IMPORT  __main
                IMPORT rex_init
                LDR     R0, =rex_init
                BLX     R0
                LDR     R0, =__main
                BX      R0

                END

#elif defined __clang__
                .section .text, "ax", %progbits
                .global Reset_Handler
                .type Reset_Handler, %function
                .code 16
Reset_Handler:
                .global  Reset_Handler
                .extern __main
                .extern rex_init

                LDR     R0, =rex_init
                BLX     R0
                LDR     R0, =__main
                BX      R0

                .end

#endif /* __clang__ */

