/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             SYSTEM DEBUG SECURE KERNEL MODULE 

 GENERAL DESCRIPTION
    This file contains initialization code for system debug. This performs
    check on reset debug status and calls into tzbsp_monitor for saving ctxt.


 Copyright 2014 - 2015 by QUALCOMM Technologies Incorporated.All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---    --------------------------------------------------------
04/06/2015 AJC     Two stack mechanism to avoid clobbering
01/14/2015 AJC     New for HoneyBadger
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*============================================================================
*
*                             MODULE IMPORTS
*
*============================================================================*/
  .extern sysdbg_cpu_ctxt_regs
  .extern system_debug_reset_entry
  .extern sysdbg_save_ctxt_arm
  .extern sysdbg_save_ctxt_hydra

/*============================================================================
*
*                             MODULE EXPORTS
*
*===========================================================================*/

    .global system_debug_reset_handler

/*============================================================================
; Handles the init routine and main entry point for System Debug Driver in
; the reset path

;============================================================================*/
.type system_debug_reset_handler, %function
system_debug_reset_handler:
    /* Save all the registers we plan to clobber */
    MSR TPIDR_EL0, X0           // save x0
    MSR TPIDR_EL1, X1           // save x1
    MSR TPIDRRO_EL0, X30        // save x30
    /* Check the type of the CPU. 
    Use ARM CPU saving or Custom CPU saving routine. Currently, either ARM or one other CPU - Hydra */
    MRS X1, MIDR_EL1
    LSR X1, X1, #24
    CMP X1, #0x41
    LDR X30, =sysdbg_save_ctxt_done
    /* Move the buffer location into X0 */
    LDR X0, =sysdbg_cpu_ctxt_regs
    B.EQ sysdbg_save_ctxt_arm
    B.NE sysdbg_save_ctxt_hydra

.type sysdbg_save_ctxt_done, %function
sysdbg_save_ctxt_done:
    /* Setup the stack using one of the two */
    LDR X0, =stack_counter
    LDR X1, [X0]
    CBNZ X1, setup_stack1
    LDR X2, =sysdbg_reset_stack0
    MOV SP, X2
    B jump_sysdbg
setup_stack1:
    LDR X2, =sysdbg_reset_stack1
    MOV SP, X2

jump_sysdbg:
    /* Bump our stack counter up */
    EOR X1, X1, #1 
    STR X1, [X0]

    /* Jump to our reset handler */
    BL system_debug_reset_entry
    /* Shouldn't reach here */
forever:
    B forever





/*=======================================================================
*
*       Data for the module 
*
*======================================================================= */
    //.section SYSDBG_QSEE_DATA, "aw", %progbits
    .p2align 3
stack_counter:  .space 0x8

    .p2align 7

sysdbg_reset_stack_space0:  .space 0x100
sysdbg_reset_stack0:

sysdbg_reset_stack_space1:  .space 0x100
sysdbg_reset_stack1:


    .end

