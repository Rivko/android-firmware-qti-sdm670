/**
  @file EFIAuthentication.h
  @brief Declaration of PE/COFF image authentication Interface  
*/
/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 04/25/17    shl    Added PKCS7 verify API support using QC solution
 10/09/12    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 08/31/12    shl     Initial version.

=============================================================================*/
#ifndef __EFI_AUTHENTICATION_PROTOCOL_H__
#define __EFI_AUTHENTICATION_PROTOCOL_H__

/** @cond */
typedef struct _EFI_QCOM_AUTHENTICATION_PROTOCOL EFI_QCOM_AUTHENTICATION_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_AUTHENTICATION_PE_IMAGE */
/** @ingroup efi_auth_pe_image
  @par Summary
  Verifies a PE/COFF image.   

  @param[in]     This        Pointer to the EFI_QCOM_AUTHENTICATION_PROTOCOL 
                             instance.
  @param[in,out] Buffer      Pointer to the start of the file buffer.
  @param[in]     BufferSize  Size of the file.

  @par Description
  This function is a shortcut of the existing security framework and it 
  implements exactly what the security framework does. The caller can locate 
  the protocol and then call this function to verify the image. 
  @par
  If the secure boot is not enabled, the function returns EFI_SUCCESS. 
  If the secure boot is enabled, it returns the result from the authentication 
  routine. If the secure boot is unknown, it returns a security violation for 
  conservation. 

  @return
  EFI_SUCCESS            -- Authentication passed.
  @par
  EFI_SECURITY_VIOLATION -- Authentication failed.
  @par
  EFI_INVALID_PARAMETER  -- Either the Buffer parameter is NULL or the 
                            BufferSize parameter \n
                            is 0. 
*/
typedef EFI_STATUS (EFIAPI *EFI_AUTHENTICATION_PE_IMAGE)(
    IN EFI_QCOM_AUTHENTICATION_PROTOCOL *This,
    IN OUT UINT8                        *Buffer,
    IN UINTN                             BufferSize);


/* EFI_VERIFY_PKCS7_DATA */
/** @ingroup efi_verify_pkcs7_data
  @par Summary
  Verifies a PKCS7 data.   

  @param[in]  This                Pointer to the EFI_QCOM_AUTHENTICATION_PROTOCOL 
  @param[in]  Pkcs7Data           Pointer to the PKCS#7 message to verify.
  @param[in]  Pkcs7Length         Length of the PKCS#7 message in bytes.
  @param[in]  TrustedCert         Pointer to a trusted/root certificate encoded in DER, which
                                  is used for certificate chain verification.
  @param[in]  TrustedCertLength   Length of the trusted certificate in bytes.
  @param[in]  Data                Pointer to the content to be verified.
  @param[in]  DataLength          Length of InData in bytes.

  @par Description
  Verifies the validity of a PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard". The input signed data could be wrapped
  in a ContentInfo structure. 
  @par
  If PKcs7Data, TrustedCert or Data is NULL, then return FALSE.
  If Pkcs7DataLength, CertLength or DataLength overflow, then return FALSE.
  Caution: This function may receive untrusted input.
  UEFI Authenticated Variable is external input, so this function will do basic
  check for PKCS#7 data structure. 

  @return
  TRUE  The specified PKCS#7 signed data is valid.
  @par
  FALSE Invalid PKCS#7 signed data.
*/
typedef BOOLEAN (EFIAPI *EFI_VERIFY_PKCS7_DATA)(
    IN EFI_QCOM_AUTHENTICATION_PROTOCOL *This,
    IN CONST UINT8                      *Pkcs7Data,
    IN UINTN                             Pkcs7DataLen,
    IN CONST UINT8                      *TrustedCert,
    IN UINTN                             TrustedCertLen,
    IN CONST UINT8                      *Data,
    IN UINTN                             DataLen);


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_authentication_protocol
  @par Summary
  Qualcomm Authentication Protocol interface.

  @par Parameters
  @inputprotoparams{authentication_proto_params.tex} 
*/
struct _EFI_QCOM_AUTHENTICATION_PROTOCOL
{
  UINT64                          Revision;
  EFI_AUTHENTICATION_PE_IMAGE     AuthenticatePeImage;
  EFI_VERIFY_PKCS7_DATA           VerifyPkcs7Data;
}; 

/** @ingroup efi_authentication_constants */
/** Protocol version. */
#define AUTHENTICATION_REVISION 0x0000000000020000

/** @ingroup efi_authentication_protocol */
extern EFI_GUID gQcomAuthenticationProtocolGuid;

#endif
