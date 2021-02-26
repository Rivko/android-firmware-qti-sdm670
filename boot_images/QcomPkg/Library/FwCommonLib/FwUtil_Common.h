/** @file FwUtil_Common.h
   
  Header for FwUtil_Common.c functions
   
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2011/10/12     jthompso    Initial version

=============================================================================*/

#ifndef __QCOM_FW_UTIL_COMMON_H__
#define __QCOM_FW_UTIL_COMMON_H__

#include <Uefi.h>
/**
 * Used to fill a 
 * function pointer in 
 * the case where there is no 
 * applicable function.
 * 
 * 
 * @return EFI_STATUS - EFI_SUCCESS
 */
EFI_STATUS
DummyFunction(void);


/**
 * Wrapper to gBS->Allocatepool. 
 *  
 * Clears buffer returned by 
 * gBS->Allocatepool. 
 * 
 * 
 * @param PoolType - PoolType for AllocatPool 
 * @param Size -     Size of buffer
 * @param Buffer -   Buffer 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
AllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    );

/**
 * print guid
 *  
 * @param guid - guid 
 * 
 * @return 
 */
VOID
FwCommonPrintGuid(
  IN EFI_GUID  guid,
  IN UINT32 DebugLevel
  );

EFI_STATUS
FwUpdateBlockFileAlignFileBuffer2(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            FileSize,
    IN VOID             *SrcBuffer,
    OUT VOID            **OutBuffer,
    IN UINTN            FileSizeAligned
    );

EFI_STATUS
FwUpdateBlockFileAlignFileBuffer(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            FileSize,
    OUT VOID            **FileBuffer,
    IN UINTN            FileSizeAligned
    );


#endif
