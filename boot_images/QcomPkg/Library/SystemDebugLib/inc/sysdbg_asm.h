
/*=============================================================================
                       S Y S D B G   A S S E M B L Y   M A C R O S

GENERAL DESCRIPTION     
  Defines common assembly macros to be used 
  
 Copyright 2012 - 2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SystemDebugLib/inc/sysdbg_asm.h#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
10/20/14   AJCheriyan   Ported to Istari
06/14/14   AJCheriyan   Removed blr from macros
06/26/12   AJCheriyan	First cut for Aragorn. 

===========================================================================*/


#ifndef _SYSDBG_ASM_H
#define _SYSDBG_ASM_H

inline void __DSB(void)
{
	__asm("DSB SY");
}

inline void __DMB(void) 
{
	__asm("DMB SY");
}

inline void __ISB(void) 
{    
	__asm("ISB");
}

inline uint64 __current_pc(void)
{
   uint64 PC;
    __asm ("ADR %0, 0" : "=r" (PC) : :);
    return PC;
}

inline uint64 __current_sp(void)
{
    uint64 SP;
    __asm ("MOV %0, sp" : "=r" (SP) : :);
    return SP;
}

inline uint64 __clz(uint64 data)
{
    uint64 count;
    __asm("CLZ %0, %1" : "=r" (count) : "r" (data): );
    return count;
}

inline uint64 __return_address(void)
{
    uint64 LR;
    __asm("MOV %0, X30" : "=r" (LR) : : );
    return LR;
}

inline void __set_stack(uint64 *stack)
{
    __asm("mov SP, X0" : : "r" (stack) : );
}

#endif /* _SYSDBG_ASM_H */