/** @file FwCrypto.c
   
  Cryptography functions used for firmware update

  Copyright (c) 2012,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2017/01/27      ai           Fix use of retail
 2013/10/03     na          Close security hole
 2013/10/02     na          Bypass encryption call based on cfg value 
 2012/10/04     mic         Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIToolSupport.h>
#include <Protocol/EFIEncryption.h>
#include <Guid/ImageAuthentication.h>
#include <Library/UefiCfgLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>

#include "FwCrypto.h"

/**
 * . Function to check if buffer is SSD encrypted
 * 
 * @param buffer          - buffer
 * @param bufferSize      - buffer size
 * @param bEncrypted      - buffer is encrypted or not
 * @return : EFI_STATUS
 */
const char* SsdMagicBytes = "tzssdimg";
#define SSD_SIGNATURE_LEN 8

EFI_STATUS 
FwCryptoSsdChecking(
  IN VOID     *buffer, 
  IN UINTN    bufferSize,
  OUT BOOLEAN *bEncrypted
  )
{
  if((buffer == NULL) ||
     (bEncrypted == NULL) ||
     (bufferSize < SSD_SIGNATURE_LEN))
  {
    return EFI_INVALID_PARAMETER;
  }
  
  //Check the first 8-bytes for the TZ magic Number
  if( CompareMem((void*)SsdMagicBytes,buffer,SSD_SIGNATURE_LEN) == 0)
  {
    *bEncrypted = TRUE;
  }
  else
  {
     *bEncrypted = FALSE;
  }

  return EFI_SUCCESS;
}
/**
 * . Function to for SSD decryption
 * 
 * @param InBuf         - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf        - Output buffer
 * @param OutBufSize    - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FwCryptoSsdDecrypt(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  )
{
  EFI_STATUS              Status          = EFI_SUCCESS;
  VOID                    *UnCachedInBuf  = NULL;
  VOID                    *UnCachedOutBuf = NULL;
  EFI_ENCRYPTION_PROTOCOL *Encryption     = NULL;
  UINT32                  BypassEncrypt    = 0;

  if((InBuf == NULL) ||
     (OutBuf == NULL) ||
     (OutBufSize == NULL)||
     (InBufSize < SSD_SIGNATURE_LEN)
     )
  {
    return EFI_INVALID_PARAMETER;
  }

  *OutBuf = NULL;
  UnCachedInBuf = UncachedAllocatePool(InBufSize);
  if(UnCachedInBuf == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FwCryptoSsdDecryptExit;
  }

  CopyMem(UnCachedInBuf,InBuf,InBufSize);
  
  if (!RETAIL)
  {
    Status = GetConfigValue ("DisableUpdateImageSecurity", &BypassEncrypt);
    if (Status != EFI_SUCCESS)
    {
      BypassEncrypt = 0;
    }
  }
  
  if (BypassEncrypt == 0)
  {
    Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                                 NULL,
                                 (VOID**)&Encryption);

    HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwCryptoSsdDecryptExit);

    /*Send buffer to Encrypt*/
    Status = Encryption->Decrypt(TZ_SSD,
                                 (UINT8*)UnCachedInBuf,
                                 InBufSize,
                                 (UINT8**)&UnCachedOutBuf,                
                                 (UINT32*)OutBufSize);
    HANDLE_ERROR_LABEL(Status, Encryption->Encrypt, FwCryptoSsdDecryptExit);

    Status = gBS->AllocatePool(EfiBootServicesData, *OutBufSize ,(VOID**)OutBuf);
    HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, FwCryptoSsdDecryptExit);

    CopyMem(*OutBuf,UnCachedOutBuf,*OutBufSize);
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "Security Based Firmware Update Disabled!\n"));

    Status = gBS->AllocatePool(EfiBootServicesData, InBufSize ,(VOID**)OutBuf);
    HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, FwCryptoSsdDecryptExit);
    *OutBufSize = InBufSize;

    CopyMem(*OutBuf,InBuf,*OutBufSize);
  }  
  
  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  return EFI_SUCCESS;

FwCryptoSsdDecryptExit:

  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  if(*OutBuf != NULL)
  {
    gBS->FreePool(*OutBuf);
    *OutBuf = NULL;
  }
  return Status;
}

/**
 * . Function to decrypt and encrypt OPM private key
 * 
 * @param InBuf         - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf        - Output buffer
 * @param OutBufSize    - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FWCryptoOpmPrivKey(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  )
{
  EFI_STATUS                     Status              = EFI_SUCCESS;
  VOID                          *UnCachedInBuf       = NULL; 
  VOID                          *UnCachedOutBuf      = NULL;
  EFI_ENCRYPTION_PROTOCOL       *Encryption          = NULL;
  EFI_QCOM_TOOLSUPPORT_PROTOCOL *ToolSupportProtocol = NULL;
  UINT8                         *key1                = NULL;
  UINT8                          key1_len8           = 0;
  UINTN                          key1_len32          = 0;

  if((InBuf == NULL) ||
     (OutBuf == NULL) ||
     (OutBufSize == NULL))
     
  {
    return EFI_INVALID_PARAMETER;
  }

  *OutBuf = NULL;
  UnCachedInBuf = UncachedAllocatePool(InBufSize);
  if(UnCachedInBuf == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FWCryptoOpmPrivKeyExit;
  }

  CopyMem(UnCachedInBuf,InBuf,InBufSize);
  
  /*locate encryption protocol*/
  Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                                NULL,
                                (VOID**)&Encryption);
  HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,FWCryptoOpmPrivKeyExit);


  /*locate tool support potocal*/
  Status = gBS->LocateProtocol(&gQcomToolSupportProtocolGuid,
                                NULL,
                                (VOID**)&ToolSupportProtocol); 
  HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,FWCryptoOpmPrivKeyExit);
  
    
  /*handshake before encryption*/
  /*get handshake key length*/
  Status = ToolSupportProtocol->GetDataLenProtocol(ToolSupportProtocol,
                                                   &key1_len8);
  HANDLE_ERROR_LABEL(Status,ToolSupportProtocol->GetDataLenProtocol,FWCryptoOpmPrivKeyExit);

  key1_len32 = (UINTN)key1_len8;
  
  /*allocate buffer for handshake key*/  
  Status = gBS->AllocatePool(EfiBootServicesData, key1_len32, (VOID**)&key1);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FWCryptoOpmPrivKeyExit);
  
  ZeroMem(key1, key1_len32);
  
  /*get handshake key*/  
  Status = ToolSupportProtocol->GetDataProtocol(ToolSupportProtocol,key1);
  HANDLE_ERROR_LABEL(Status,ToolSupportProtocol->GetDataProtocol,FWCryptoOpmPrivKeyExit);
  
  /*perform handshake*/  
  Status = Encryption->Handshake(key1,key1_len32);
  HANDLE_ERROR_LABEL(Status,Encryption->Handshake,FWCryptoOpmPrivKeyExit);

  Status = Encryption->Encrypt(TZ_AES_FIXED_KEY_TO_MSM,
                               (UINT8*)UnCachedInBuf,
                               InBufSize,
                               (UINT8**)&UnCachedOutBuf,                
                               (UINT32*)OutBufSize);
  HANDLE_ERROR_LABEL(Status,Encryption->Encrypt,FWCryptoOpmPrivKeyExit);

  Status = gBS->AllocatePool(EfiBootServicesData, *OutBufSize ,(VOID**)OutBuf);
  HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, FWCryptoOpmPrivKeyExit);

  CopyMem(*OutBuf,UnCachedOutBuf,*OutBufSize);

  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  if (key1 != NULL)
  {
    gBS->FreePool(key1);
    key1 = NULL;
  }

  return EFI_SUCCESS;
  
FWCryptoOpmPrivKeyExit:

  if (key1 != NULL)
  {
    gBS->FreePool(key1);
    key1 = NULL;
  }

  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  if(*OutBuf != NULL)
  {
    gBS->FreePool(*OutBuf);
    *OutBuf = NULL;
  }
  return Status;
}

/**
 * . Function to encrypt SSD key
 * 
 * @param InBuf           - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf         - Output buffer
 * @param OutBufSize   - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FwCryptoMsmEncrypt(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  )
{
  EFI_STATUS              Status          = EFI_SUCCESS;
  VOID                    *UnCachedInBuf  = NULL;
  VOID                    *UnCachedOutBuf = NULL;
  EFI_ENCRYPTION_PROTOCOL *Encryption     = NULL;

  if((InBuf == NULL) ||
     (OutBuf == NULL) ||
     (OutBufSize == NULL)||
     (InBufSize < SSD_SIGNATURE_LEN)
     )
  {
    return EFI_INVALID_PARAMETER;
  }

  *OutBuf = NULL;
  UnCachedInBuf = UncachedAllocatePool(InBufSize);
  if(UnCachedInBuf == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FwCryptoMsmEncryptExit;
  }

  CopyMem(UnCachedInBuf,InBuf,InBufSize);
  
  Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                               NULL,
                               (VOID**)&Encryption);

  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwCryptoMsmEncryptExit);

  /*Send buffer to Encrypt*/
  Status = Encryption->Encrypt(TZ_SSD,
                               (UINT8*)UnCachedInBuf,
                               InBufSize,
                               (UINT8**)&UnCachedOutBuf,                
                               (UINT32*)OutBufSize);
  HANDLE_ERROR_LABEL(Status, Encryption->Encrypt, FwCryptoMsmEncryptExit);

  Status = gBS->AllocatePool(EfiBootServicesData, *OutBufSize ,(VOID**)OutBuf);
  HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, FwCryptoMsmEncryptExit);
    
  CopyMem(*OutBuf,UnCachedOutBuf,*OutBufSize);
  
  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  return EFI_SUCCESS;

FwCryptoMsmEncryptExit:
  
  if(UnCachedInBuf != NULL)
  {
    UncachedFreePool(UnCachedInBuf);
    UnCachedInBuf = NULL;
  }

  if(*OutBuf != NULL)
  {
    gBS->FreePool(*OutBuf);
    *OutBuf = NULL;
  }   
  return Status;
}


