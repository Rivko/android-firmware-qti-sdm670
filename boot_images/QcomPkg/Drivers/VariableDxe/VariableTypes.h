/** @file VariableTypes.h
   
  Type definitions required for Qualcomm UEFI Variable storage.

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/13/11   niting  Cleanup.
 07/29/11   niting  Initial revision.

=============================================================================*/
#ifndef __VARIABLETYPES_H__
#define __VARIABLETYPES_H__

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>

#include <Guid/ImageAuthentication.h>

#define EFI_CERT_TYPE_PKCS7_GUID \
{0x4aafd29d, 0x68df, 0x49ee,{0x8a, 0xa9, 0x34, 0x7d, 0x37, 0x56, 0x65, 0xa7}}

#define CALCULATE_CRC32(Data, DataSize, Crc32) \
        CalculateCrc32(Data, DataSize, Crc32)

#define BS_V_NS_ATTRIBUTES   EFI_VARIABLE_BOOTSERVICE_ACCESS

#define BS_NV_NS_ATTRIBUTES  (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE)

/* 
Currently this table is not supported 
#define BS_V_S_ATTRIBUTES    (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
*/

#define BS_NV_S_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

#define RT_V_NS_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS)

#define RT_NV_NS_ATTRIBUTES  (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE)

/* 
Currently this table is not supported 
#define RT_V_S_ATTRIBUTES    (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
*/

#define RT_NV_S_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

#endif /* __VARIABLETYPES_H__ */
