/** @file  
                    MeasureBootLib.h

 
  Copyright (c) 2012 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 08/22/13   shl      Added offline crash dump interface
 12/10/12   shl      Warning fix 
 07/17/12   SHL      Add back two interfaces
 05/15/12   shl      Added support for OEM FW update authentication 
 03/23/12   shl      Added functions for store acpi before and after fixup.
 11/28/11   shl      Moved SetExitBootServicesRegSignal() to SecurityDxe
 08/18/11   shl      Initial version

=============================================================================*/

// function declarations.
EFIAPI VOID InitCertBufferForMeasure(VOID);
EFIAPI VOID StoreCertBufferForMeasure( UINT8 * Cert, UINT32 CertSize, EFI_GUID *OwnerGuid);
EFIAPI EFI_STATUS MeasureCertInBuffer( VOID);
EFIAPI EFI_STATUS MeasureAcpi ( VOID );
EFIAPI EFI_STATUS MeasureSmbios(VOID);
EFIAPI EFI_STATUS MeasureSal(VOID);
EFIAPI EFI_STATUS MeasureUefiVersionId(VOID);
EFIAPI EFI_STATUS MeasureGPT(VOID);
EFIAPI EFI_STATUS MeasureBootVariables(VOID);
EFIAPI EFI_STATUS MeasureSecurityVariables(VOID);
EFIAPI EFI_STATUS MeasureSeparator(VOID);
EFIAPI EFI_STATUS MeasureSeparatorAll(VOID);
EFIAPI EFI_STATUS MeasureCallEfiAppFromBootOption(VOID);
EFIAPI EFI_STATUS MeasureReturnFromEfiAppFromBootOption(VOID);
EFIAPI EFI_STATUS MeasureExitBootServiceSuccess(VOID);
EFIAPI EFI_STATUS MeasureExitBootServiceFailure(VOID);
EFIAPI EFI_STATUS MeasureExitBootServiceInvocation(VOID);
EFIAPI EFI_STATUS MeasureOfflineCrashDump(VOID);
EFI_STATUS EFIAPI RegisterMeasurePeImage( VOID );
EFIAPI EFI_STATUS MeasurePeImageAll(
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
);

EFIAPI EFI_STATUS MeasurePeImage(
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
);

/**
  This is the function to set the flag to indicate this is OEM FW update.

  @param[in]  VOID

  @retval EFI_SUCCESS              Updated successfully.
**/
EFI_STATUS MeasureBootSetOemFwUpdateFlag( VOID );
