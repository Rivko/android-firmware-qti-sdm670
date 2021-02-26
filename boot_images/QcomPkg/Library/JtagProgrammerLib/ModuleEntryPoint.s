;/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                      S T A R T U P   C O D E
;
; GENERAL DESCRIPTION
;   This module contains the entry point for our Flash Programming Plug-In 
;   for Lauterbach JTAG/ICD TRACE32. All it does is setups stack and calls
;   our main 'C' function "main_c".
;
;   The C function does return and expects a breakpoint there. This is the
;   case because the FLASH programming plug-in specification requires it.
;   The break-point is used to give control back to TRACE32 Debugger so 
;   that it may fill up the buffer and call our plug-in again. This cycle
;   continues until FLASH programming is complete.
;
; EXTERNALIZED FUNCTIONS
;   None.  
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;   This module must be linked first, specifically the AREA "StartHere", 
;   since this is our entry point. This is done in our scatter load file.
;
;  Copyright (c) 2014 Qualcomm Technologies, Inc.
;  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*======================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
-----------   ---     -----------------------------------------------------
2014-11-07    bn      Ported from Boot's Jtag Programmer
===========================================================================*/
;/*============================================================================
;
;                  REGISTER DEFINITIONS
;
;============================================================================*/

;/*============================================================================
;
;                  STACK DEFINITIONS
;
;============================================================================*/


;/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/

.text
.align 3

GCC_ASM_IMPORT (jsdcc_init)
GCC_ASM_EXPORT (_ModuleEntryPoint)

  
_ModuleEntryPoint:
  LDR x0, =Image$$JTAG_PROGRAMMER_STACK$$Start
  MOV SP, x0

  b jsdcc_init

dead:  
  b dead                      /* We should never get here */
