/*=============================================================================
                        System Debug Utility File 

GENERAL DESCRIPTION     
  This module defines structures for the system debug driver.
  
	Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   --------------------------------------------------
05/09/2014   kedara  Include com_dtypes.h instead of comdef.h
02/26/2014   ck    Added DEBUG_UI_UNLOCK_CODE
02/18/2014   ck    Removed SDI local copies of 32 to 64 switch function.
                   Using central single copy located in SDI code space.
02/06/2014   ck    Added prototypes for sys_debug A53 32 to 64 switch functions
01/02/2014   ck    Initial release for Bear family

===========================================================================*/

#ifndef BOOT_SYS_DEBUG_H
#define BOOT_SYS_DEBUG_H

#include "com_dtypes.h"


#define DEBUG_UI_UNLOCK_CODE    0xC5ACCE55


typedef struct
{
  uint64 entry_point;
  uint32 elf_class;
} sys_debug_qsee_info_type;


/* Internal CPU dump structure */
typedef struct
{
  uint64 r0;
  uint64 r1;
  uint64 r2;
  uint64 r3;
  uint64 r4;
  uint64 r5;
  uint64 r6;
  uint64 r7;
  uint64 r8;
  uint64 r9;
  uint64 r10;
  uint64 r11;
  uint64 r12;
  uint64 r13_usr;
  uint64 r14_usr;
  uint64 r13_hyp;
  uint64 r14_irq;
  uint64 r13_irq;
  uint64 r14_svc;
  uint64 r13_svc;
  uint64 r14_abt;
  uint64 r13_abt;
  uint64 r14_und;
  uint64 r13_und;
  uint64 r8_fiq;
  uint64 r9_fiq;
  uint64 r10_fiq;
  uint64 r11_fiq;
  uint64 r12_fiq;
  uint64 r13_fiq;
  uint64 r14_fiq;
  uint64 pc;
  uint64 r13_mon;
  uint64 r14_mon;
  uint64 r14_hyp;
  uint64 reset_status;
  uint64 __reserved1;
  uint64 __reserved2;
  uint64 __reserved3;
  uint64 __reserved4;
} sys_debug_cpu_ctxt_regs_type;


#endif /* BOOT_SYS_DEBUG_H */
