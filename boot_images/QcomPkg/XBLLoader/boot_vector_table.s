//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                              boot_vector_table.s
//
// GENERAL DESCRIPTION
//   This file captures 64-bit vector table for ARMV8
//
// Copyright 2013, 2016, 2017 by Qualcomm Technologies, Incorporated.All Rights Reserved.
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                           EDIT HISTORY FOR FILE
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
//
// when       who     what, where, why
// --------   ---     --------------------------------------------------------
// 02/24/17   kpa     fix exception handler size overflow
// 01/26/17   yepings Fixed LLVM384 compiling issue    
// 06/23/16   digant  register save and exception handling updates
// 06/02/16   digant  xpu violation query for sync,serror
// 12/08/13   dhaval  vector table creation for ARMV8 64-bit mode
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


//============================================================================
//
//                            MODULE INCLUDES
//
//============================================================================
#include "boot_target.h"
#include "boot_error_handler.h"

//============================================================================
//
//                             MODULE IMPORTS
//
//============================================================================

//////////// For now, symbols are branched to itself in first version, those will 
//////////// be implemented and imported in these files eventually.
    .extern boot_sys_c_handler
    .extern boot_sync_c_handler
    .extern boot_irq_c_handler
    .extern boot_fiq_c_handler
    .extern boot_nested_exception_c_handler
    .extern exception_context
	
//============================================================================
//
//                             MODULE EXPORTS
//
//============================================================================

    // Export the sbl1_vector_table for VBAR configuration in initial boot 
	// assembly init routines.
    .global sbl1_vector_table
    .global get_sync_type

//============================================================================
//                             MODULE MACRO
//============================================================================

//-------------------------------------
// Push Link register
// sure SP is 16 bytes aligned
//-------------------------------------
.macro PUSH_LR
  stp     x28, x30, [sp, #-16]!
.endm

//-------------------------------------
// Push all registers on stack making 
// except Link register
// sure SP is 16 bytes aligned
//-------------------------------------
.macro PUSHALLREG_EXCEPT_LR
  stp     x0, x1, [sp, #-16]!
  stp     x2, x3, [sp, #-16]!
  stp     x4, x5, [sp, #-16]!
  stp     x6, x7, [sp, #-16]!
  stp     x8, x9, [sp, #-16]!
  stp     x10, x11, [sp, #-16]!
  stp     x12, x13, [sp, #-16]!
  stp     x14, x15, [sp, #-16]!
  stp     x16, x17, [sp, #-16]!
  stp     x18, x19, [sp, #-16]!
  stp     x20, x21, [sp, #-16]!
  stp     x22, x23, [sp, #-16]!
  stp     x24, x25, [sp, #-16]!
  stp     x26, x27, [sp, #-16]!
  stp     x28, x29, [sp, #-16]!
.endm

//-------------------------------------
// Pop all registers from stack
// except Link register
//-------------------------------------
.macro POPALLREG_EXCEPT_LR
  ldp     x28, x29, [sp],#16
  ldp     x26, x27, [sp],#16
  ldp     x24, x25, [sp],#16
  ldp     x22, x23, [sp],#16
  ldp     x20, x21, [sp],#16
  ldp     x18, x19, [sp],#16
  ldp     x16, x17, [sp],#16
  ldp     x14, x15, [sp],#16
  ldp     x12, x13, [sp],#16
  ldp     x10, x11, [sp],#16
  ldp     x8, x9,   [sp],#16
  ldp     x6, x7,   [sp],#16
  ldp     x4, x5,   [sp],#16
  ldp     x2, x3,   [sp],#16
  ldp     x0, x1,   [sp],#16
.endm

//-------------------------------------
// Pop all registers from stack
// except Link register
//-------------------------------------
.macro POP_LR
  ldp     x28, x30, [sp],#16
.endm  
  
//============================================================================
//
//                             MODULE DATA AREA
//
//============================================================================

    .section SBL1_VECTOR_TABLE, "ax", %progbits
    .p2align 11

//for SBL, vector table is same for all ELs(EL3 and EL1)
sbl1_vector_table:
  //--------------------------------------
  // +0x180 SError/vSError	 	
  // +0x100 FIQ/vFIQ
  // +0x080 IRQ/vIRQ	
  // VBAR_ELn+0x000 Synchronous (at base)
  //--------Current EL with SP0-----------
  //--------------------------------------
sbl1_csp0_sync:
  b sbl1_csp0_sync

  .align 7
//sbl1_csp0_irq  b sbl1_csp0_irq
sbl1_csp0_irq:
  b sbl1_csp0_irq 

  .align 7
sbl1_csp0_fiq:
  b sbl1_csp0_fiq 

  .align 7
sbl1_csp0_serror:
  b sbl1_csp0_serror 

  //--------------------------------------
  // +0x380 SError/vSError	 	
  // +0x300 FIQ/vFIQ
  // +0x280 IRQ/vIRQ	
  // +0x200 Synchronous 
  //--------Current EL with SPx-----------
  //--------------------------------------

//-----------------------------------
// we save all registers and restore 
// them back after querying for xpu violation
// so that we have the values 
// when error occured
//----------------------------------------  
  .align 7
sbl1_cspx_sync:
  // explicitly save link reg on stack
  // A macro is used for register push since the inline
  // expansion shall consume exception handler space
  // A api is used for register pop to avoid overflowing
  // handler size (128 bytes). Goal is to consume optimum
  // handler space and not waste any. However this forces 
  // link register push-pop to be handled explicitly.
  PUSH_LR
  PUSHALLREG_EXCEPT_LR
  bl boot_query_xpu_violation
  bl pop_all_registers
  // explicitly restore link reg from stack
  POP_LR  
  mov x14, x30
  bl  save_exception_context
  bl  check_re_entry
  ldr x5,=boot_sync_c_handler
  br  x5
  
  .align 7
sbl1_cspx_irq:
  mov x14, x30
  bl  save_exception_context
  bl  check_re_entry
  ldr x5,=boot_irq_c_handler
  br  x5
  
  .align 7
sbl1_cspx_fiq:
  mov x14, x30
  bl  save_exception_context
  bl  check_re_entry
  ldr x5,=boot_fiq_c_handler
  br  x5
  
//-----------------------------------
// we save all registers and restore 
// them back after querying for xpu violation
// so that we have the values 
// when error occured
//----------------------------------------
  .align 7
sbl1_cspx_serror:
  // explicitly save link reg on stack
  PUSH_LR
  PUSHALLREG_EXCEPT_LR
  bl boot_query_xpu_violation
  bl pop_all_registers
  // explicitly restore link reg from stack
  POP_LR
  mov x14, x30 
  bl  save_exception_context
  bl  check_re_entry
  ldr x5,=boot_sys_c_handler
  br  x5
  
  //--------------------------------------
  // +0x580 SError/vSError	 	
  // +0x500 FIQ/vFIQ
  // +0x480 IRQ/vIRQ	
  // +0x400 Synchronous 
  //--------Lower EL using AArch64-----------
  //--------------------------------------
  .align 7
sbl1_lel64_sync:
  b sbl1_lel64_sync

  .align 7
sbl1_lel64_irq:
  b sbl1_lel64_irq

  .align 7
sbl1_lel64_fiq:
  b sbl1_lel64_fiq

  .align 7
sbl1_lel64_serror:
 b sbl1_lel64_serror

  //--------------------------------------
  // +0x780 SError/vSError	 	
  // +0x700 FIQ/vFIQ
  // +0x680 IRQ/vIRQ	
  // +0x600 Synchronous 
  //--------Lower EL using AArch32-----------
  //--------------------------------------
  .align 7
sbl1_lel32_sync:
  b sbl1_lel32_sync

  .align 7
sbl1_lel32_irq:
  b sbl1_lel32_irq

  .align 7
sbl1_lel32_fiq:
  b sbl1_lel32_fiq

  .align 7
sbl1_lel32_serror:
  b sbl1_lel32_serror

    .section SBL1_EXCEPTION_HANDLERS, "ax", %progbits
    .p2align 11

//============================================================================
// pop_all_registers
//  INPUT       -      
//  DESCRIPTION -
//        Restores all registers, popping off from stack
// Note:
//     A macro is used for register push since the inline
//     expansion shall consume exception handler space
//     A api is used for register pop to avoid overflowing
//     handler size (128 bytes). Goal is to consume optimum
//     handler space and not waste any. However this forces 
//     link register push-pop to be handled explicitly.
//============================================================================
pop_all_registers:
  POPALLREG_EXCEPT_LR
  ret
    
//============================================================================
// get_sync_type
//  INPUT       -      
//  DESCRIPTION -
//        Reads exception syndrome in ESR_EL3 register, bits 26-31
//        Return this value in X0 register
//============================================================================
get_sync_type:
  MRS X0, ESR_EL1
  AND X0, X0, #0xFC000000
  LSR X0, X0, #26
  ret

//============================================================================
// check_re_entry
//  INPUT       -      
//  DESCRIPTION -
//        checks if we have re-entered from an exception,
//        and if we have then go to nested handler
//============================================================================  
check_re_entry:
  MOV x20, #1
  LDR x21, =re_entry
  LDR x22, [x21]
  CMP x20, x22
  BNE not_nested
  LDR X5,=boot_nested_exception_c_handler// we require nested handler
  BR  X5
not_nested:
  STR x20, [x21]
  ret
    
//============================================================================
// save_exception_context
//  INPUT       -      
//  DESCRIPTION -
//        Saves important registers to boot_exception_context[] structure, 
//        to be dumped in debug log later
//============================================================================
save_exception_context:
  // First save used registers to stack, so they can be restored and saved
  STP     x20, x21, [sp, #-16]!
  
  // Load address of exception context, and set cookie
  LDR     x20, =exception_context
  LDR     x21, =SBL1_EXC_CONTEXT_VALID_COOKIE
  STR     x21, [x20], #8
  
  STP     x0, x1, [x20], #16

  // Restore saved x20 register and use X0 as the pointer into structure
  MOV     x0, x20
  LDP     x20, x21, [sp], #16
  STP     x2, x3, [x0], #16
  STP     x4, x5, [x0], #16
  STP     x6, x7, [x0], #16
  STP     x8, x9, [x0], #16
  STP     x10, x11, [x0], #16
  STP     x12, x13, [x0], #16
  STP     x14, x15, [x0], #16
  STP     x16, x17, [x0], #16
  STP     x18, x19, [x0], #16
  STP     x20, x21, [x0], #16
  STP     x22, x23, [x0], #16
  STP     x24, x25, [x0], #16
  STP     x26, x27, [x0], #16
  STP     x28, x29, [x0], #16

  MRS     x20, ELR_EL1  

  // Restore x30 contents which caller saved in x14, so exception context can be dumped  
  MOV     x29, x30
  MOV     x30, x14  
  STP     x30, x20, [x0], #16

  // Now restore the valid return address of the caller into this function
  MOV     x30, x29
  
  MRS     x20, ESR_EL1
  
  STR     x20, [x0], #8
  
  ret  
  
.data
  re_entry:
         .4byte 0

  .end 
