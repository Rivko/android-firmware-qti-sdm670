/** 
  @file  EFIEncryption.h
  @brief Encryption and Decryption Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/01/13   wuc     Added TZ_MSM_HW_KEY_DATA and TZ_MSM_HW_KEY_CEK algorithm type
                    Added Set Hdcp provisioning data support
 05/23/13   rsb     Added TZ_MSM_HW_KEY algorithm type
 07/13/12   mic     Added WinSecApp OPM handshake support
 06/05/12   mic     Added SSD/fixed/MSMHWKey encryption algorithm type
 02/10/12   rs      Created
=============================================================================*/
#ifndef __EFIENCRYPTION_H__
#define __EFIENCRYPTION_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** Protocol version. */
#define EFI_ENCRYPTION_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_encrypt_protocol */
#define EFI_ENCRYPTION_PROTOCOL_GUID \
   { 0xf78552e0, 0xf751, 0x427f, { 0x94, 0x51, 0x93, 0xe, 0x60, 0xd8, 0x39, 0xb8 } }


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/** 
  External reference to the Encryption Protocol GUID.
*/
extern EFI_GUID gEfiEncryptionProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** Protocol declaration.  */
typedef struct _EFI_ENCRYPTION_PROTOCOL EFI_ENCRYPTION_PROTOCOL;
/** @endcond */

/**
  Enumeration for the encryption algorithm.
**/
typedef enum {
  TZ_SSD = 0,
  TZ_AES_FIXED_KEY_TO_MSM,
  TZ_HDCP_KEY_CEK,
  TZ_HDCP_KEY_DATA
} EFI_ENCRYPTION_ALGORITHM_TYPE;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ENCRYPT */
/** @ingroup efi_encrypt
  @par Summary
  Takes an unencrypted buffer and encrypts it
   
  @param[in]  Algorithm           The algorithm to use for 
   							      encryption.
  @param[in]  BufferIn            The input buffer to encrypt 
  @param[in]  BufferInSize        The size in bytes of the input buffer 
  @param[out] EncryptedData       Returns pointer to encrypted data
                                  within the input buffer
  @param[out] EncryptedDataSize   Size in bytes of the encrypted data 


  @par Description
  This function takes in an input buffer and encrypts it. The 
  encryption is done in-place so the resulting encrypted data pointer is
  within the input buffer. The input buffer needs to be
  non-secure, uncached and physically contiguous, allocated using 
  UncachedAllocatePool(). The input buffer should be freed only 
  after the encrypted data is consumed and no longer needed. If the
  input buffer is already encrypted, it is a no-op and it is
  returned as-is. 

  Note: For Algorithm type TZ_MSM_HW_KEY only, the caller MUST free 
  EncryptedData and use gBS->FreePool().

  @return
  EFI_SUCCESS           -- Function completed successfully.
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/

typedef
EFI_STATUS
(EFIAPI *EFI_ENCRYPT)(
  IN  EFI_ENCRYPTION_ALGORITHM_TYPE  Algorithm,
  IN  UINT8                         *BufferIn,
  IN  UINT32                         BufferInSize,
  OUT UINT8                        **EncryptedData,
  OUT UINT32                        *EncryptedDataSize
);

/* EFI_DECRYPT */
/** @ingroup efi_decrypt
  @par Summary
  Takes an encrypted buffer and decrypts it
   
  @param[in]  Algorithm           The algorithm used for  
   						          decryption
  @param[in]  BufferIn            The input buffer to decrypt 
  @param[in]  BufferInSize        The size in bytes of the input buffer 
  @param[out] DecryptedData       Returns pointer to decrypted data
                                  within the input buffer
  @param[out] DecryptedDataSize   Size in bytes of the decrypted data 


  @par Description
  This function takes in an input buffer and decrypts it. The 
  decryption is done in-place so the resulting decrypted data pointer 
  is within the input buffer. The input buffer needs to be
  non-secure, uncached and physically contiguous, allocated using 
  UncachedAllocatePool(). The input buffer should be freed only 
  after the decrypted data is consumed and no longer needed. If the
  input buffer is already decrypted, it is a no-op and it is
  returned as-is.   

  @return
  EFI_SUCCESS           -- Function completed successfully.
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DECRYPT)(
  IN  EFI_ENCRYPTION_ALGORITHM_TYPE Algorithm,
  IN  UINT8                        *BufferIn,
  IN  UINT32                        BufferInSize,
  OUT UINT8                       **DecryptedData,
  OUT UINT32                       *DecryptedDataSize
);

/* EFI_HANDSHAKE */
/** @ingroup efi_handshake
  @par Summary
  Sends a buffer to WinSecApp for handshake
   
  @param[in]  BufferIn            The input buffer to handshake
  @param[in]  BufferInSize        The size in bytes of the input buffer 

  @par Description
  This function sends a buffer to WinSecApp for handshaking before requesting 
  OPM crypto service.

  @return
  EFI_SUCCESS           -- Function completed successfully.
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_HANDSHAKE)(
  IN  UINT8                        *BufferIn,
  IN  UINT32                        BufferInSize
);

/* EFI_SETHDCP */
/** @ingroup efi_sethdcp
  @par Summary
  Set Hdcp provisioning data

  @par Description
  This function sends decryption requests to DxHdcpApp2 for setting Hdcp provisioning
  data blob and cek blob.

  @return
  EFI_SUCCESS           -- Function completed successfully.
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SETHDCP)();

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/

struct _EFI_ENCRYPTION_PROTOCOL {
  UINT64                       Revision;
  EFI_ENCRYPT                  Encrypt;
  EFI_DECRYPT                  Decrypt;
  EFI_HANDSHAKE                Handshake; 
  EFI_SETHDCP                  SetHdcp;  
};

#endif /* __EFIENCRYPTION_H__ */

