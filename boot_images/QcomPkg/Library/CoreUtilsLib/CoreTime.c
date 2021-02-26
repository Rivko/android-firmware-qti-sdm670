/******************************************************************//**
 * @file CoreTime.c
 *
 * @brief Time tick get function for UEFI targets
 *
 * Copyright (C) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
03/27/14   rli     Updated to use TimeLib instead of QTimer directly
05/01/13   rli     Updated to support 64bit timer
03/10/11   bjs     Initial checkin
========================================================================*/

#include "CoreTime.h"

#ifdef TARGET_UEFI

uint64 UEFITimeGet64(void)
{
  uint64 result;
  result = GetPerformanceCounter();
  return(result);
}

uint32 UEFITimeGet(void)
{
  uint32 result;                   //should be 32 KHz ticks
  uint64 ticks = UEFITimeGet64(); //19.2 MHz

  result =  (uint32) (ticks/600); // 600 = 19200/32

  return(result);
}

#endif // TARGET_UEFI
