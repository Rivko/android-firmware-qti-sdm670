/** @file  
                    EFIMeasureBoot.h

 
  Copyright (c) 2012-2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/13/17   pr       Changes to fix PCR4 measurment issue
 12/03/14   sk       Added to change library to driver
 08/22/13   shl      Added offline crash dump interface
 12/10/12   shl      Warning fix 
 07/17/12   SHL      Add back two interfaces
 05/15/12   shl      Added support for OEM FW update authentication 
 03/23/12   shl      Added functions for store acpi before and after fixup.
 11/28/11   shl      Moved SetExitBootServicesRegSignal() to SecurityDxe
 08/18/11   shl      Initial version

=============================================================================*/

/** @cond */
typedef struct _EFI_QCOM_MEASUREBOOT_PROTOCOL EFI_QCOM_MEASUREBOOT_PROTOCOL;
/** @endcond */

/* External reference to the gQcomMeasureBootProtocolGuid GUID */
/** @cond */
extern EFI_GUID gQcomMeasureBootProtocolGuid;
/** @endcond */

/*===========================================================================
  FUNCTION DECLARATIONS
===========================================================================*/

/* EFI_MEASUREBOOT_REGISTER_MEASURE_PEIMAGE */ 
/** @ingroup efi_measureboot_register_measure_peimage
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_REGISTER_MEASURE_PEIMAGE)( IN  EFI_QCOM_MEASUREBOOT_PROTOCOL   *This);


/* EFI_MEASUREBOOT_MEASURE_UEFI_VERSIONID */ 
/** @ingroup efi_measureboot_measure_uefi_versionid
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_UEFI_VERSIONID)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_ACPI */ 
/** @ingroup efi_measureboot_measure_acpi
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_ACPI)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_SMBIOS */ 
/** @ingroup efi_measureboot_measure_smbios
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_SMBIOS)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_OFFLINECRASHDUMP */ 
/** @ingroup efi_measureboot_measure_offlinecrashdump
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_OFFLINECRASHDUMP)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_SECURITYVARIABLES */ 
/** @ingroup efi_measureboot_measure_securityvariables
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_SECURITYVARIABLES)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_CALLEFIAPPFROMBOOTOPTION */ 
/** @ingroup efi_measureboot_measure_callefiappfrombootoption
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_CALLEFIAPPFROMBOOTOPTION)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_SEPARATOR_ALL */ 
/** @ingroup efi_measureboot_measure_separator_all
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_SEPARATOR_ALL)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_GPT */ 
/** @ingroup efi_measureboot_measure_gpt
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_GPT)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_SUCCESS */ 
/** @ingroup efi_measureboot_measure_exitbootservice_success
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_SUCCESS)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_FAILURE */ 
/** @ingroup efi_measureboot_measure_exitbootservice_failure
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_FAILURE)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_BOOT_SETOEMFWUPDATE_FLAG */ 
/** @ingroup efi_measureboot_measure_boot_setoemfwupdate_flag
  @par Summary
  This is the function to set the flag to indicate this is OEM FW update.
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_BOOT_SETOEMFWUPDATE_FLAG)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_INVOCATION */ 
/** @ingroup efi_measureboot_measure_exitbootservice_invocation
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_INVOCATION)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_MEASURE_RETURNFROMEFIAPP_FROMBOOTOPTION */ 
/** @ingroup efi_measureboot_measure_returnfromefiapp_frombootoption
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_RETURNFROMEFIAPP_FROMBOOTOPTION)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_INIT_CERTBUFFER_FORMEASURE */ 
/** @ingroup efi_measureboot_init_certbuffer_formeasure
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef VOID (EFIAPI *EFI_MEASUREBOOT_INIT_CERTBUFFER_FORMEASURE)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);


/* EFI_MEASUREBOOT_STORE_CERTBUFFER_FORMEASURE */ 
/** @ingroup efi_measureboot_store_certbuffer_formeasure
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef VOID (EFIAPI *EFI_MEASUREBOOT_STORE_CERTBUFFER_FORMEASURE)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This, UINT8 * Cert, UINT32 CertSize, EFI_GUID *OwnerGuid);


/* EFI_MEASUREBOOT_MEASURE_CERTINBUFFER */ 
/** @ingroup efi_measureboot_measure_certbuffer
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_CERTINBUFFER)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This);

/** @ingroup efi_measureboot_measure_peimageall
  @par Summary
    
  @param[in]     This                  Pointer to the EFI_QCOM_MEASUREBOOT_PROTOCOL 
                                       instance.
  @param[in]     AuthenticationStatus                
  
  @param[in]     DevicePath                  
  
  @param[in]     FileBuffer                  
  
  @param[in]     FileSize                  
  
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MEASUREBOOT_MEASURE_PEIMAGEALL)( IN EFI_QCOM_MEASUREBOOT_PROTOCOL *This, UINT32 AuthenticationStatus, CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath, VOID *FileBuffer, UINTN  FileSize);
/*
// Called from SecurityDxe
// EFIAPI EFI_STATUS MeasureAcpi ( VOID );
// EFIAPI EFI_STATUS MeasureSmbios(VOID);
// EFIAPI EFI_STATUS MeasureUefiVersionId(VOID);
// EFIAPI EFI_STATUS MeasureGPT(VOID);
// EFIAPI EFI_STATUS MeasureSeparatorAll(VOID);
// EFIAPI EFI_STATUS MeasureCallEfiAppFromBootOption(VOID);
// EFIAPI EFI_STATUS MeasureExitBootServiceSuccess(VOID);
// EFIAPI EFI_STATUS MeasureExitBootServiceFailure(VOID);
// EFIAPI EFI_STATUS MeasureExitBootServiceInvocation(VOID);
// EFIAPI EFI_STATUS MeasureOfflineCrashDump(VOID);
// EFI_STATUS EFIAPI RegisterMeasurePeImage( VOID );
// EFI_STATUS MeasureBootSetOemFwUpdateFlag( VOID );
// EFIAPI EFI_STATUS MeasureSecurityVariables(VOID);
// EFIAPI EFI_STATUS MeasureReturnFromEfiAppFromBootOption(VOID);

// Called from SecBootLib.c
// EFIAPI VOID       InitCertBufferForMeasure(VOID);
// EFIAPI VOID       StoreCertBufferForMeasure( UINT8 * Cert, UINT32 CertSize, EFI_GUID *OwnerGuid);
// EFIAPI EFI_STATUS MeasureCertInBuffer( VOID);
*/


/*===========================================================================
  POSSIBLE FUTURE SUPPORT
===========================================================================*/
/*
EFIAPI EFI_STATUS MeasurePeImageAll(              // Never used anywhere
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
);
EFIAPI EFI_STATUS MeasurePeImage(                 // Never used anywhere
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
);
EFIAPI EFI_STATUS MeasureSal(VOID);               // Never called from outside
EFIAPI EFI_STATUS MeasureBootVariables(VOID);     // Never called from outside
EFIAPI EFI_STATUS MeasureSeparator(VOID);         // Never called from outside
*/

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_measureboot_protocol
  @par Summary
  Contains modules to measure various data and code. 

  @par Parameters
  @inputprotoparams{measureboot_proto_params.tex} 
*/
struct _EFI_QCOM_MEASUREBOOT_PROTOCOL
{
  EFI_MEASUREBOOT_REGISTER_MEASURE_PEIMAGE                  RegisterMeasurePeImage;
  EFI_MEASUREBOOT_MEASURE_UEFI_VERSIONID                    MeasureUefiVersionId;
  EFI_MEASUREBOOT_MEASURE_ACPI                              MeasureAcpi;
  EFI_MEASUREBOOT_MEASURE_SMBIOS                            MeasureSmbios;
  EFI_MEASUREBOOT_MEASURE_OFFLINECRASHDUMP                  MeasureOfflineCrashDump;
  EFI_MEASUREBOOT_MEASURE_SECURITYVARIABLES                 MeasureSecurityVariables; 
  EFI_MEASUREBOOT_MEASURE_CALLEFIAPPFROMBOOTOPTION          MeasureCallEfiAppFromBootOption;
  EFI_MEASUREBOOT_MEASURE_SEPARATOR_ALL                     MeasureSeparatorAll;
  EFI_MEASUREBOOT_MEASURE_GPT                               MeasureGPT;
  EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_SUCCESS           MeasureExitBootServiceSuccess;
  EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_FAILURE           MeasureExitBootServiceFailure;
  EFI_MEASUREBOOT_MEASURE_BOOT_SETOEMFWUPDATE_FLAG          MeasureBootSetOemFwUpdateFlag;
  EFI_MEASUREBOOT_MEASURE_EXITBOOTSERVICE_INVOCATION        MeasureExitBootServiceInvocation;
  EFI_MEASUREBOOT_MEASURE_RETURNFROMEFIAPP_FROMBOOTOPTION   MeasureReturnFromEfiAppFromBootOption;
  EFI_MEASUREBOOT_INIT_CERTBUFFER_FORMEASURE                InitCertBufferForMeasure;
  EFI_MEASUREBOOT_STORE_CERTBUFFER_FORMEASURE               StoreCertBufferForMeasure;
  EFI_MEASUREBOOT_MEASURE_CERTINBUFFER                      MeasureCertInBuffer;
  EFI_MEASUREBOOT_MEASURE_PEIMAGEALL                        MeasurePeImgAll;
}; 

