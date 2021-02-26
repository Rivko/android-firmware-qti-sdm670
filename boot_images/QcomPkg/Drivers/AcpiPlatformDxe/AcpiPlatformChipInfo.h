/*============================================================================
  @file AcpiPlatfromChipInfo.h

AcpiPlatfrom Callback functions for chipinfo 
               Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
               
               
============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/09/15   wayne   Added for Chip Serial number callback
 10/17/14   wayne   Created for chipinfo acpi callback 
=============================================================================*/


#ifndef ACPIPLATFORM_CHIPINFO_H
#define ACPIPLATFORM_CHIPINFO_H

extern EFI_STATUS EFIAPI AmlUpdateChipId (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateChipChipIdString (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateMajorChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateMinorChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateChipFamily(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateModemInfo(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);

extern EFI_STATUS EFIAPI AmlUpdateChipSerialNum(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
);


#endif
