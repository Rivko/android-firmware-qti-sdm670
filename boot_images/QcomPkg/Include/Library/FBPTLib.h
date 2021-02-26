/** @file FBPTLib.h
   
  Contains FBPT specific library functions.

  Copyright (c) 2012, 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/09/14   vishalo Update signature for absolute address in cfg
 07/06/12   vishalo Created
=============================================================================*/
#ifndef __FBPTLIB_H__
#define __FBPTLIB_H__

#include <Uefi.h>

/**
  Creates FBPT structure, and initialize Reset Time field
 
  @param MemoryBase     MemoryBase for DDR
                        FBPTPayLoad offset 
                        will be added internaly 
 
  @return EFI_STATUS    Success or failure
**/
EFI_STATUS
InitializeFBPT (VOID);

/**
  Get Time from power on 

  @return Time in nanoseconds
          0 if overflow detected
**/
UINT32 
BootGetTimeNanoSec(VOID);

/**
  Convert ARM Performance Monitor counter cycle
  count to time in nano seconds.
 
  @param CycleCount     Number of PMon cycles
  @return Time NanoSeconds
**/
UINT64
CycleToNanoSec(UINT32 CycleCount);

#endif /* __FBPTLIB_H__ */
