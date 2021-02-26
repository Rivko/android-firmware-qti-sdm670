#ifndef __SECBOOTLIB_H__
#define __SECBOOTLIB_H__

/** @file  
                             SecBootLib.h

  This is for function declaration of PE/COFF authentication.

  Copyright (c) 2011-2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/17   shl     Added PKCS7 verify API support, this API is defined in EDK2.
06/13/17   pr      Changes to fix PCR4 Measurment issue
08/12/81   shl     Added more APIs for authenticating image
05/15/12   shl     Added support for OEM FW update authentication 
07/18/11   shl     Initial version.
===========================================================================*/

/**
  This is the function registeres SecBootVerifyPeImage as a callback in 
  security framework, so it will be called to verify the PE/COFF image.

  @param[in]  VOID

  @retval EFI_SUCCESS              The handlers were registered successfully.
**/

EFI_STATUS EFIAPI RegisterSecBootVerifyPeImage( VOID );

/**
  This is the function registeres SecBootMeasurePeImageAll as a callback in 
  security framework, so it will be called to measure the PE/COFF image.

  @param[in]  VOID

  @retval EFI_SUCCESS              The handlers were registered successfully.
**/

EFI_STATUS EFIAPI RegisterSecBootMeasurePeImage( VOID );

/**
  This is the function to set the flag to indicate this is OEM FW update.

  @param[in]  VOID

  @retval EFI_SUCCESS              Updated successfully.
**/
EFI_STATUS SecBootSetOemFwUpdateFlag( VOID );

/**
This the function verifies PE/COFF image against all databases.

  @param[in]  AuthenticationStatus  Not really used. Legacy format.
  @param[in]  *File                 Not really used. Legacy format.
  @param[in]  *FileBuffer           Points to PE/COFF image.
  @param[in]  FileSize              PE/COFF image size

  @retval EFI_SUCCESS               Verified successfully.
  @retval EFI_INVALID_PARAMETER     Parmeter is not right or data not recoginized
  @retval EFI_SECURITY_VIOLATION    Verification not supported.
**/

EFI_STATUS EFIAPI SecBootVerifyPeImage
(
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
);

/**
This is the function will be return secure boot enabled or not.

  @param[in/out]            secure boot enable state.

  @retval EFI_SUCCESS       query successfully.
  @retval EFI_INTERNAL_     query failed

**/

EFI_STATUS EFIAPI SecBootEnabled( BOOLEAN *Enabled );

/**
  Verifies the validity of a PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard". The input signed data could be wrapped
  in a ContentInfo structure.
  If PKcs7Data, TrustedCert or Data is NULL, then return FALSE.
  If Pkcs7DataLength, CertLength or DataLength overflow, then return FALSE.
  Caution: This function may receive untrusted input.
  UEFI Authenticated Variable is external input, so this function will do basic
  check for PKCS#7 data structure.
  @param[in]  Pkcs7Data           Pointer to the PKCS#7 message to verify.
  @param[in]  Pkcs7Length         Length of the PKCS#7 message in bytes.
  @param[in]  TrustedCert         Pointer to a trusted/root certificate encoded in DER, which
                                  is used for certificate chain verification.
  @param[in]  TrustedCertLength   Length of the trusted certificate in bytes.
  @param[in]  Data                Pointer to the content to be verified.
  @param[in]  DataLength          Length of InData in bytes.
  @retval  TRUE  The specified PKCS#7 signed data is valid.
  @retval  FALSE Invalid PKCS#7 signed data.
**/

BOOLEAN EFIAPI Pkcs7Verify 
(
  IN  CONST UINT8   *Pkcs7Data,
  IN  UINTN          Pkcs7DataLen,
  IN  CONST UINT8   *TrustedCert,
  IN  UINTN          TrustedCertLen,
  IN  CONST UINT8   *Data,
  IN  UINTN          DataLen
); 

#endif /* __SECBOOTLIB_H__ */