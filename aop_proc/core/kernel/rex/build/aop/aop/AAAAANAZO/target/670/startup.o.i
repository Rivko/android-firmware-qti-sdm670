//=========================================================================
//
//*
// This file contains the implementation for AOP to boot
//
//@par EXTERNALIZED FUNCTIONS
// (none)
//
//@par INITIALIZATION AND SEQUENCING REQUIREMENTS
// (none)
//
//Copyright (C) 2014 QUALCOMM Technologies Incorporated.
//All Rights Reserved. QUALCOMM Proprietary and Confidential.
//
//*
//=========================================================================
// Edit History
//$Header:
//$Date: 2018/02/16 $
//
// when who what, where, why
// -------- --- ---------------------------------------------------
//
//=========================================================================
        //// IMPORT ||Image$$CODE_RAM_SAVE$$Base||
//-------------------------------------------------------------------------
// DATA
//-------------------------------------------------------------------------
                .extern __aop_stack_limit
                .section .text, "a", %progbits
                .p2align 2
__Vectors: // Minimal Vector Table entries at 0x0 for boot
                .word __aop_stack_limit
                                        // Initial Main SP
                .word Reset_Handler // Reset Handler
//-------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//-------------------------------------------------------------------------
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
