#ifndef __QCOM_DPP_COMMON_UTIL_H_
#define __QCOM_DPP_COMMON_UTIL_H_

/** @file CommonUtil.h
   
  Qualcomm Dpp tool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/13   mic     Initial version
=============================================================================*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Guid/GlobalVariable.h>

#define MAC_ADDR_SIZE 6
#define WLAN_NUM_MAC_ADDR 4

#define OUTPUT_DIR_NAME   L"ProvDump"

typedef struct _MACADDR
{
  UINT8 bytes[ MAC_ADDR_SIZE ];
} MACADDR;

typedef struct 
{
  UINT8 version;
  UINT8 length;
  UINT8 numMacAddr;
  MACADDR macAddress[WLAN_NUM_MAC_ADDR];
} data_wlan_mac_addr;

typedef struct 
{
  UINT8 version;
  UINT8 length;
  MACADDR macAddress;
} data_bt_mac_addr;

#define OUTPUT_WIDTH  16

#define ERR_HANDLE(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      return Status; \
    } \
  } while (0)

#define HANDLE_ERROR_LABEL(Status, Function, Label) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
       AsciiPrint(": %a returned %r\n", #Function, Status); \
      goto Label; \
    }   \
  }while (0)

#define HANDLE_ERR_CONTINUE(Status, Function) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
      AsciiPrint(": %a returned %r...Continue\n", #Function, Status); \
    }   \
  }while (0)

#define DEBUG_ERR_LABEL(MESSAGE, Label) \
  do {  \
       DEBUG((EFI_D_ERROR, MESSAGE)); \
       goto Label; \
  }while (0)


#define ALIGN_TO_BLOCK(Size, BlockSize, AlignedSize) \
  { \
    AlignedSize = Size + (BlockSize - 1); \
    AlignedSize = AlignedSize & ~(BlockSize -1); \
  }

#define DPP_NAME L"DPP"
#define WP_FAT16_NAME L"PLAT"
#define WA_FAT16_NAME L"GPP_FAT16"
#define TYPE_MEDIA_DEVICE_PATH 4

EFI_STATUS
GetPartitionHandle(
    IN OUT EFI_HANDLE              *PartHandle,
    IN EFI_GUID                    *PartTypeGuid,
    IN CHAR16                      *PartitionName,
    IN EFI_GUID                    *Protocol
    );

EFI_STATUS
EFIAPI
PrintDataInHexAndChar (
  IN UINT8 *pData,
  IN UINT32 Size
  );

EFI_STATUS
AllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    );
#endif
