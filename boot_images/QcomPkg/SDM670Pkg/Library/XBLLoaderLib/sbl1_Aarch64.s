//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                              SBL1 Aarch64
//
// GENERAL DESCRIPTION
//   This file bootstraps the processor. The Start-up Primary Bootloader
//   (SBL1) performs the following functions:
//
//      - Set up the hardware to continue boot process.
//      - Initialize DDR memory
//      - Load Trust-Zone OS
//      - Load RPM firmware
//      - Load APPSBL and continue boot process
//
//   The SBL1 is written to perform the above functions with optimal speed.
//   It also attempts to minimize the execution time and hence reduce boot time.
//
// Copyright 2013 - 2017 by Qualcomm Technologies, Inc.
// All Rights Reserved.
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                           EDIT HISTORY FOR FILE
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
// $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLLoaderLib/sbl1_Aarch64.s#1 $
//
// when       who     what, where, why
// --------   ---     --------------------------------------------------------
// 06/23/16   digant  enable external aborts and some cleanup for EL1
// 03/22/16   kedara  Zero init stack region.
// 03/18/16   kedara  relocate sbl1_get_current_el to debug image address space
// 03/09/16   kedara  Update sbl1_enable_serror for msm8998.
// 11/09/15   plc     Add extra support for running in EL1 or EL3
// 10/22/15   plc     Update __main to be Exception Level aware to support EL1
// 07/09/15   kedara  Update sbl1_enable_serror.
// 02/12/15   ck      Removing sbl1_recover_from_reset as this was for CCI workaround
// 11/14/14   kedara  Unmask pending aborts.
// 08/20/14   kedara  Update Stack base address.
// 07/18/14   kedara  Update for llvm compilation, remove sysini for 8996.
// 05/22/14   plc     Add sbl1_recover_from_reset call before sysini
// 05/20/14   plc     Add a53_aarch64_sysini delivered from HW team
// 04/03/14   plc     Fix PBL shared-data pointer to reference 32-bit register
// 01/23/14   plc     Add Setup for VBAR, and update SP logic
// 01/07/14   plc     Adding initial bootstrap and loop_forever
// 11/01/13   plc     Initial revision for 64-bit Aarch64 sbl1
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


//============================================================================
//
//                            MODULE INCLUDES
//
//============================================================================
#include "boot_msm.h"
#include "boot_target.h"

//============================================================================
//
//                             MODULE DEFINES
//
//============================================================================
//
.set  Mode_SVC,  0x13
.set  Mode_ABT,  0x17
.set  Mode_UND,  0x1b
.set  Mode_USR,  0x10
.set  Mode_FIQ,  0x11
.set  Mode_IRQ,  0x12
.set  Mode_SYS,  0x1F

.set  I_Bit,  0x80
.set  F_Bit,  0x40
.set  SCR_EL3_EA_Bit,  0x08
.set  ISR_EL1_A_BIT,   0x100

.set  Int_D_Bit, 0x08
.set  Int_A_Bit, 0x04
.set  Int_I_Bit, 0x02
.set  Int_F_Bit, 0x01

//============================================================================
//
//                             MODULE IMPORTS
//
//============================================================================

    // Import the external symbols that are referenced in this module.
  .extern a53_aarch64_sysini
  .extern sbl1_vector_table
  .extern sbl1_main_ctl

//============================================================================
//
//                             MODULE EXPORTS
//
//============================================================================

  // Export the external symbols that are referenced in this module.
  .global sbl_loop_here
 
  // Export the symbols __main and _main to prevent the linker from
  // including the standard runtime library and startup routine.
  .global   __main
  .global   _main
  .global sbl1_entry
  .global sbl1_disable_serror
  .global sbl1_enable_serror  
	
//============================================================================
//
//                             MODULE DATA AREA
//
//============================================================================


//============================================================================
// Qualcomm SECONDARY BOOT LOADER 1 ENTRY POINT
//============================================================================

    //AREA  SBL1_ENTRY, CODE, READONLY, ALIGN=4
    .section  SBL1_ENTRY, "ax", %progbits
    .p2align 4
    
    //CODE
    //ENTRY
    
__main:
_main:

//============================================================================
//   sbl1_entry
//============================================================================
sbl1_entry:
  // Save the passing parameter from PBL
  MOV x7, x0
 
  // Configure SBL1 vector table base for EL1 VBARs (XBL context)
  // -------------------------------
  LDR x0, =sbl1_vector_table  
  MSR VBAR_EL1, x0
  // Check for pending aborts
  // ------------------------------- 
  MRS X0, ISR_EL1
  AND X0, X0, #ISR_EL1_A_BIT
  CBNZ X0, boot_loop_here

  // Enable external aborts
  // -------------------------------  
  MSR DAIFClr, #Int_A_Bit
  isb

  // retrieve stack-base
  // -------------------------------
  MOV x0,  SBL1_STACK_BASE & 0xffff
  MOVK x0, SBL1_STACK_BASE >> 16, LSL #16

  // configure stack for SBL (EL1)
  // -------------------------------
  MOV SP, x0

  // Zero init stack region
  // -------------------------------    
  MOV  x0, SCL_SBL1_STACK_BASE & 0xffff
  MOVK x0, SCL_SBL1_STACK_BASE >> 16, LSL #16 

  MOV  x1, SCL_SBL1_STACK_SIZE & 0xffff
  MOVK x1, SCL_SBL1_STACK_SIZE >> 16, LSL #16  

  // size expected to be 16 (0x10) byte aligned
  BIC  x1, x1, #0xF  
  MOV  x2, #0     // used as counter
  
write_16bytes_loop:
  stp xzr,xzr,[x0],  #0x10
  add     x2, x2, #0x10 // increment by 16 bytes
  cmp     x1, x2
  b.ne write_16bytes_loop  

sbl1_entry_init_stack:  
  // -------------------------------
  // add more assembly init code here for entering sbl1_main_ctl	
  // 
	
  // restore PBL parameter and enter sbl1_main_ctl
  // -------------------------------
  MOV w0, w7
  BL sbl1_main_ctl
	
  // For safety
  BL boot_loop_here  // never returns, keep lr in r14 for debug


//======================================================================
// Called by sbl1_error_handler only. We clean up the registers and loop
// here until JTAG is connected.
//======================================================================
sbl_loop_here:

boot_loop_here:
  b boot_loop_here

  
sbl1_disable_serror:  
  // If in EL3, Disable EA bit. Do Not Take external aborts, sError interrupt
  // -------------------------------  
  // Check the current Exception Level 
  //--------------------------------
  MRS x0, CurrentEL
  LSR x0, x0, #2   
  CMP x0, #3  
  BGE disable_ea_bit_el3
  B   disable_ea_bit_end
disable_ea_bit_el3:
  MRS X0, SCR_EL3
  BIC X0, X0, #SCR_EL3_EA_Bit
  MSR SCR_EL3, X0
disable_ea_bit_end:
  RET    

sbl1_enable_serror:  

  // If in EL3, Enable EA bit. Take external aborts, sError interrupt
  // -------------------------------  
  // Check the current Exception Level 
  //--------------------------------
  MRS x0, CurrentEL
  LSR x0, x0, #2   
  CMP x0, #3  
  BGE enable_serror_ea_bit_el3
  B   enable_serror_ea_bit_end
enable_serror_ea_bit_el3:
  MRS X0, SCR_EL3
  ORR X0, X0, #SCR_EL3_EA_Bit
  MSR SCR_EL3, X0
enable_serror_ea_bit_end:
  RET

  .end
