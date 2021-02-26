/**
  @file AslrLib.c
 
  @brief ASLR functionality
 
  Copyright (c) 2016 by Qualcomm Technologies, Inc.
  All Rights Reserved.
**/
 
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    --------------------------------------------------- 
 12/08/16   bh      Initial version

========================================================================*/

#include <Library/AslrLib.h>
#include <Library/TzRngLib.h>

UINTN AslrAdjustRNGVal(UINTN Shift)
{
#ifdef ENABLE_ASLR
  STATIC UINTN RngShift = 0;
  STATIC UINTN RNGVal = 0;
  UINTN Adjusted;
  UINTN Mask;

  if (RNGVal == 0)
    RNGVal = GetRNGVal();

  Mask = ((0x1 << Shift) - 1);

  do {
    Adjusted = (RNGVal & (Mask << RngShift)) >> RngShift;

    RngShift += Shift;
    if (RngShift >= (8*sizeof(UINTN)))
      RngShift = 0;

  } while(Adjusted == 0);

  return Adjusted;
#else
  return 0;
#endif
}

/* Returns new stack end pointer when ASLR is enabled */
UINTN RandomizeStack(UINTN StackSize)
{
  return (StackSize - (AslrAdjustRNGVal(ASLR_STACK_RNG_BITS) * ASLR_STACK_ALIGN));
}


