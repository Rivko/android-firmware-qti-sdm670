/** @file FBPTTest.c
   
  FBPT Structure Definition

  Copyright (c) 2012-2013, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 13/01/2013   vk       Warning fix
 07/06/2012   vishalo  Initial revision
=============================================================================*/

#ifndef __FBPT_H__
#define __FBPT_H__

#include <Uefi.h>

#pragma pack(1)

typedef struct FBPTHeader
{
  CHAR8  Signature[4];
  UINT32 Length;
} FBPTHeader_t;

typedef struct FBBPDataRecord
{
  UINT16 PerformanceRecordType;
  UINT8  RecordLength;
  UINT8  Revision;
  UINT32 Reserved;
  UINT64 ResetEnd;
  UINT64 OSLoaderLoadImageStart;
  UINT64 OSLoaderStartImageStart;
  UINT64 ExitBootServicesEntry;
  UINT64 ExitBootServicesExit;
} FBBPDataRecord_t;

typedef struct UefiPerfRecord
{
  FBPTHeader_t UefiFBPTHeader;
  FBBPDataRecord_t UefiFBBPDataRecord;
} UefiPerfRecord_t;

#pragma pack()

#endif

