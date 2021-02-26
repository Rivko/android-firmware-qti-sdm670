/** @file ClockTestUEFI.h
   
  This file contains list of test clocks and settings for UEFI 
  platform. 

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/23/10   vs      Created.

=============================================================================*/


#ifndef __UEFICLOCKTEST_H__
#define __UEFICLOCKTEST_H__


#include <Uefi.h>
#include <Library/BaseLib.h>

/*
 * Clock test settings 
 */
typedef struct ClockTestSettings
{
  CHAR8      *szName;
  UINT32     nFreqHz;
}ClockTestSettings;

extern const ClockTestSettings ClockTestList[];

#endif /* __UEFICLOCKTEST_H__ */

