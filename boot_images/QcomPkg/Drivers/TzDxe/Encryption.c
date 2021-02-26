/** @file Encryption.c

  Encryption UEFI Driver code

  Copyright (c) 2012 - 2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/11/16   pr      Commented out Clock code
 10/12/15   mic     Support updated Dpp protocol
 08/15/14   sm      Switched to new SCM API
 08/06/14   sm      Removed everything not related to SetHDCP call
 07/08/14   rp      setHDCP() fix adjusting crypto clocks
 11/27/13   sk      Changed some debug levels to EFI_D_ERROR
 10/09/13   niting  Removed SCM dependencies
 08/01/13   wuc     Added support for setting hdcp provisionig data
 07/25/13   shl     Changed printing level.
 07/11/13   mic     Updated HDCP provisioning support using Discretix API
 05/23/13   rsb     Support for TZ_MSM_HW_KEY algorithm
 03/13/13   rs      Changes to send only 2KB of metadata to ParseMD
 02/21/13   shl     ScmDxe cleanup.
 02/15/13   rs      Added EnableClocks() and DisableClocks() for Crypto clocks
 02/07/13   rs      Added changes for Decryption-in-chunks TZ API
 02/04/13   rsb     Use new method to query WinSecApp ID
 01/08/13   rsb     Fix DEBUG error message and struct size for zero memory
 07/13/12   mic     Added WinSecApp OPM handshake support
 06/25/12   mic     Added Fix Key decryption/SSD key decryption/MSM HW
                    key encryption protocol
 02/21/12   rs      Initial revision
 07/14/17	dp		Removed EnableClocks() and DisableClocks() function
 					and calls. Function contents were commented previously.
=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/ArmLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Include/scm_sip_interface.h>

/**
  Encryption PROTOCOL interfaces
 */
#include <Protocol/EFIEncryption.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFIDpp.h>
/**
  EFI interfaces
 */
#include <Library/DebugLib.h>

/* String identifier for dxhdcp app name*/
#define DXHDCP2_APP_NAME "dxhdcp2"

/* String identifier for Crypto AHB Clock */
#define AHB_CLK_NAME "gcc_ce1_ahb_clk"

/* String identifier for Crypto AXI Clock */
#define AXI_CLK_NAME "gcc_ce1_axi_clk"

/* String identifier and Freq for Crypto Clock */
#define CLK_NAME "gcc_ce1_clk"
#define CLK_FREQ 100000000 //100Mhz

/*-----------------------------------------------------------
  Begin -- Structures for KEY_PROVISION syscalls
-------------------------------------------------------------*/

#define TZEXEC_SVC_KEY_PROV_ID                  0x100
#define SERVICE_KEY_PROVISION_CMD(x)           (TZEXEC_SVC_KEY_PROV_ID | x)

#define KEY_PROVISION_ENCRYPT_BLOB               SERVICE_KEY_PROVISION_CMD(1)
#define KEY_PROVISION_SET_CEK_BLOB               SERVICE_KEY_PROVISION_CMD(2)
#define KEY_PROVISION_SET_PROVISIONING_DATA_BLOB SERVICE_KEY_PROVISION_CMD(3)


#define VERSION_OFFSET                     0
#define VERSION_SIZE                       1
#define IV_OFFSET                          (VERSION_OFFSET+VERSION_SIZE)
#define AES_IV_SIZE                        16
#define HMAC_OFFSET                        (IV_OFFSET + AES_IV_SIZE)
#define SHA256_HMAC_SIZE                   32
#define NUM_PAD_BYTES_OFFSET               (HMAC_OFFSET + SHA256_HMAC_SIZE)
#define NUM_PAD_BYTES_SIZE                 1
#define PAD_BYTES_OFFSET                   (NUM_PAD_BYTES_OFFSET + NUM_PAD_BYTES_SIZE)

typedef struct _KEY_PROVISION_ENCRYPT_REQ
{
 /* Command ID */
  UINT32                        commandId;
 /* Offset of input buffer*/
  UINT32                        inBufferOffset;
 /* Input Buffer Size*/
  UINT32                        inBufferSize;
} __attribute__ ((packed)) KEY_PROVISION_ENCRYPT_REQ, *PKEY_PROVISION_ENCRYPT_REQ;

/*------------------------------------------------------------
-------------------------------------------------------------*/

typedef struct _KEY_PROVISION_ENCRYPT_RSP
{
 /* Command ID */
  UINT32                         commandId;
 /* status indicate ERROR */
  UINT32                         status;
 /* Offset of output buffer*/
  UINT32                         outBufferOffset;
 /* output Size*/
  UINT32                         outBufferSize;
} __attribute__ ((packed)) KEY_PROVISION_ENCRYPT_RSP, *PKEY_PROVISION_ENCRYPT_RSP;

 /*------------------------------------------------------------
  -------------------------------------------------------------*/

typedef struct _KEY_PROVISION_DECRYPT_REQ
{
  /* Command ID */
   UINT32                        commandId;
  /* Offset of input buffer*/
   UINT32                        inBufferOffset;
  /* Input Buffer Size*/
   UINT32                        inBufferSize;
} __attribute__ ((packed)) KEY_PROVISION_DECRYPT_REQ, *PKEY_PROVISION_DECRYPT_REQ;

/*------------------------------------------------------------
-------------------------------------------------------------*/

  typedef struct _KEY_PROVISION_DECRYPT_RSP
  {
    /* Command ID */
    UINT32                         commandId;
   /* status indicate ERROR */
    UINT32                         status;
   /* Offset of output buffer*/
    UINT32                         outBufferOffset;
   /* output Size*/
    UINT32                         outBufferSize;
} __attribute__ ((packed)) KEY_PROVISION_DECRYPT_RSP, *PKEY_PROVISION_DECRYPT_RSP;


/*-----------------------------------------------------------
  End -- Structures for KEY_PROVISION syscalls
-------------------------------------------------------------*/

extern EFI_GUID gQcomTokenSpaceGuid;

// Scm protocol pointer
STATIC QCOM_SCM_PROTOCOL      *pQcomScmProtocol = NULL;


/**
*  QueryDxHDCP2AppId ()
*  @brief
*  Query the Discretix HDCP2 application ID
*
*  @param[out]  pAppId   Integer returning the DxHDCP2 App ID
*  @return
*  EFI_STATUS
*/

EFI_STATUS QueryDxHDCP2AppId(UINT32 *pAppId)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT32        AppId = 0;
  UINT8         AppName[] = DXHDCP2_APP_NAME;

  /* pQcomScmProtocol is pre-initialized in EncryptionProtocolInit */
  if ((pQcomScmProtocol == NULL) || (pAppId == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  Status = pQcomScmProtocol->ScmSendCommand(
                                       pQcomScmProtocol,
                                       APP_QUERY_APPID_CMD,
                                       &AppId,
                                       AppName,
                                       sizeof(AppName),
                                       NULL,
                                       0
                                       );
  if (EFI_ERROR(Status))
  {
    goto Exit;
  }

  *pAppId = AppId;

Exit:

  return Status;
}

/**
*  GetEncryptedBlockSize ()
*
*  @brief
*  given an input buffer needing encryption, returns the size of the output encrypted buffer required
*  for key provision service
*
*  @param[in]  ptLen             size of input buffer (non-encrypted) in bytes
*  @param[out] pEncLen           size of output buffer (encrypted) in bytes
*  @return
*  EFI_STATUS
*/
EFI_STATUS GetEncryptedBlockSize(
  IN  UINT32 ptLen,
  OUT UINT32 *pEncLen)
{
  UINT32 padLen = 0;
  UINT32 encLen = 0;

  if (pEncLen == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  padLen = ((SHA256_HMAC_SIZE + NUM_PAD_BYTES_SIZE + ptLen)%16) ?(16 - ((SHA256_HMAC_SIZE + NUM_PAD_BYTES_SIZE + ptLen)%16)): 0;
  encLen = VERSION_SIZE + AES_IV_SIZE + SHA256_HMAC_SIZE + NUM_PAD_BYTES_SIZE + padLen + ptLen;

  *pEncLen = encLen;

  return EFI_SUCCESS;
}

/**
*  tzt_key_provision_service_encrypt_syscall ()
*
*  @brief
*  Encrypt function for Key Provision Service used for HDCP 2.x provisioning.
*  Input buffer can be cached or uncached.  CryptoData will be a separately
*  allocated cached buffer that must be freed using FreePool.
*
*  @param[in]  BufferIn          input buffer
*  @param[in]  BufferInSize      input buffer size
*  @param[out] CryptoData        output buffer
*  @param[out] CryptoDataSize    output buffer size
*  @return
*  EFI_STATUS
*/
EFI_STATUS tzt_key_provision_service_encrypt_syscall(
   IN UINT8                          *BufferIn,
   IN UINT32                          BufferInSize,
   OUT UINT8                        **CryptoData,
   OUT UINT32                        *CryptoDataSize
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  PKEY_PROVISION_ENCRYPT_REQ pReq = NULL;
  PKEY_PROVISION_ENCRYPT_RSP pRsp = NULL;
  UINT32 encLen = 0;
  UINT32 KeyProvReqLen = 0;
  UINT32 KeyProvRspLen = 0;
  VOID  *pKeyProvReq = NULL;
  VOID  *pKeyProvRsp = NULL;
  UINT32 AppId = 0;

  /* pQcomScmProtocol is pre-initialized in EncryptionProtocolInit */
  if ((pQcomScmProtocol == NULL) || (BufferIn == NULL) || (CryptoData == NULL) || (CryptoDataSize == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  // Determine the size of the output encrypted buffer based on the input data size.
  Status = GetEncryptedBlockSize(BufferInSize, &encLen);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN,"GetEncryptedBlockSize returned = (0x%x)\r\n",Status));
    goto Exit_tzt_key_provision_service_syscall;
  }

  KeyProvReqLen = sizeof(KEY_PROVISION_ENCRYPT_REQ) + BufferInSize;

  // Allocate memory for request structure including data to encrypt
  pKeyProvReq = (VOID *)AllocateZeroPool(KeyProvReqLen);
  if (pKeyProvReq == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate req\r\n"));
    goto Exit_tzt_key_provision_service_syscall;
  }

  pReq = (PKEY_PROVISION_ENCRYPT_REQ)pKeyProvReq;

  KeyProvRspLen = sizeof(KEY_PROVISION_ENCRYPT_RSP) + encLen;

  // Allocate memory for response structure including encrypted data
  pKeyProvRsp = (VOID *)AllocateZeroPool(KeyProvRspLen);
  if (pKeyProvRsp == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate rsp\r\n"));
    goto Exit_tzt_key_provision_service_syscall;
  }

  pRsp = (PKEY_PROVISION_ENCRYPT_RSP)pKeyProvRsp;

  // Setup the key provision command parameters
  pReq->commandId = KEY_PROVISION_ENCRYPT_BLOB;
  pReq->inBufferOffset = sizeof(KEY_PROVISION_ENCRYPT_REQ);
  pReq->inBufferSize = BufferInSize;

  CopyMem((((UINT8 *)pKeyProvReq) + pReq->inBufferOffset),BufferIn,BufferInSize);

  Status = QueryDxHDCP2AppId(&AppId);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN, "Failed to retrieve Discretix AppId from Image, 0x%x\n\r", Status));
    goto Exit_tzt_key_provision_service_syscall;
  }
  

  Status = pQcomScmProtocol->ScmSendCommand(
                                             pQcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                            &AppId,
                                             pKeyProvReq,
                                             KeyProvReqLen,
                                             pKeyProvRsp,
                                             KeyProvRspLen
                                            );
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN, "Failed ScmSendCommand, 0x%x\n\r", Status));
    goto Exit_tzt_key_provision_service_syscall;
  }


  if (pRsp->status != 0)
  {
    DEBUG((EFI_D_WARN,"Encryption failed with status = (0x%x)\r\n", pRsp->status));
    Status = EFI_DEVICE_ERROR;
    goto Exit_tzt_key_provision_service_syscall;
  }

  if (pRsp->outBufferSize != encLen)
  {
    DEBUG((EFI_D_WARN,"Encryption response parameter invalid output size (0x%x vs 0x%x)\r\n", pRsp->outBufferSize, encLen));
    Status = EFI_DEVICE_ERROR;
    goto Exit_tzt_key_provision_service_syscall;
  }

  /* Return the Crypto data, caller must free the buffer. */
  *CryptoData = (UINT8 *)AllocateZeroPool(pRsp->outBufferSize);
  if (*CryptoData == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate return data\r\n"));
    goto Exit_tzt_key_provision_service_syscall;
  }

  CopyMem(*CryptoData, ((UINT8 *)pKeyProvRsp) + pRsp->outBufferOffset, pRsp->outBufferSize);

  *CryptoDataSize = pRsp->outBufferSize;

 Exit_tzt_key_provision_service_syscall:

  if (pKeyProvReq != NULL)
  {
    gBS->FreePool(pKeyProvReq);
    pKeyProvReq = NULL;
  }

  if (pKeyProvRsp != NULL)
  {
    gBS->FreePool(pKeyProvRsp);
    pKeyProvRsp = NULL;
  }

  return Status;
}

/**
*  tzt_key_provision_service_decrypt_syscall ()
*
*  @brief
*  Decrypt function for Key Provision Service used for HDCP 2.x provisioning.
*  Input buffer can be cached or uncached.  CryptoData will be a separately
*  allocated cached buffer that must be freed using FreePool.
*
*  @param[in]  BufferIn          input buffer
*  @param[in]  BufferInSize      input buffer size
*  @param[out] CryptoData        output buffer
*  @param[out] CryptoDataSize    output buffer size
*  @param[in]  CommandId         HDCP key type (data/cek)
*  @return
*  EFI_STATUS
*/
EFI_STATUS tzt_key_provision_service_decrypt_syscall(
   IN UINT8                          *BufferIn,
   IN UINT32                          BufferInSize,
   OUT UINT8                        **CryptoData,
   OUT UINT32                        *CryptoDataSize,
   IN UINT32                          CommandId
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  PKEY_PROVISION_DECRYPT_REQ pReq = NULL;
  PKEY_PROVISION_DECRYPT_RSP pRsp = NULL;
  UINT32 KeyProvReqLen = 0;
  UINT32 KeyProvRspLen = 0;
  VOID  *pKeyProvReq = NULL;
  VOID  *pKeyProvRsp = NULL;
  UINT32 AppId = 0;

  /* pQcomScmProtocol is pre-initialized in EncryptionProtocolInit */
  if ((pQcomScmProtocol == NULL) || (BufferIn == NULL) || (CryptoData == NULL) || (CryptoDataSize == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  KeyProvReqLen = sizeof(KEY_PROVISION_DECRYPT_REQ) + BufferInSize;

  // Allocate memory for request structure including data to encrypt
  pKeyProvReq = (void *)AllocateZeroPool(KeyProvReqLen);
  if (pKeyProvReq == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate req\r\n"));
    goto Exit_tzt_key_provision_service_syscall;
  }

  pReq = (PKEY_PROVISION_DECRYPT_REQ)pKeyProvReq;

  KeyProvRspLen = sizeof(KEY_PROVISION_DECRYPT_RSP) + BufferInSize;

  // Allocate memory for response structure including encrypted data
  pKeyProvRsp = (void *)AllocateZeroPool(KeyProvRspLen);
  if (pKeyProvRsp == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate rsp\r\n"));
    goto Exit_tzt_key_provision_service_syscall;
  }

  pRsp = (PKEY_PROVISION_DECRYPT_RSP)pKeyProvRsp;

  // Setup the key provision command parameters
  pReq->commandId = CommandId;
  pReq->inBufferOffset = sizeof(KEY_PROVISION_DECRYPT_REQ);
  pReq->inBufferSize = BufferInSize;

  CopyMem((((UINT8 *)pKeyProvReq) + pReq->inBufferOffset),BufferIn,BufferInSize);

  Status = QueryDxHDCP2AppId(&AppId);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN, "Failed to retrieve AppId from Image, 0x%x\n\r", Status));
    goto Exit_tzt_key_provision_service_syscall;
  }


  Status = pQcomScmProtocol->ScmSendCommand(pQcomScmProtocol,
                                            APP_SEND_DATA_CMD,
                                            &AppId,
                                            pKeyProvReq,
                                            KeyProvReqLen,
                                            pKeyProvRsp,
                                            KeyProvRspLen
                                            );
   if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed ScmSendCommand, 0x%x\n\r", Status));
    goto Exit_tzt_key_provision_service_syscall;
  }

  if (pRsp->status != 0)
  {
    DEBUG((EFI_D_ERROR,"Encryption failed with status = (0x%x)\r\n", pRsp->status));
    Status = EFI_DEVICE_ERROR;
    goto Exit_tzt_key_provision_service_syscall;
  }

  if (pRsp->outBufferSize != 0)
  {
      /* Return the Crypto data, caller must free the buffer. */
      *CryptoData = (UINT8 *)AllocateZeroPool(pRsp->outBufferSize);
      if (*CryptoData == NULL)
      {
           Status = EFI_OUT_OF_RESOURCES;
           DEBUG((EFI_D_WARN, "AllocateZeroPool failed to allocate return data\r\n"));
           goto Exit_tzt_key_provision_service_syscall;
      }

      CopyMem(*CryptoData, ((UINT8 *)pKeyProvRsp) + pRsp->outBufferOffset, pRsp->outBufferSize);
   }

  *CryptoDataSize = pRsp->outBufferSize;

 Exit_tzt_key_provision_service_syscall:

  if (pKeyProvReq != NULL)
  {
    gBS->FreePool(pKeyProvReq);
    pKeyProvReq = NULL;
  }

  if (pKeyProvRsp != NULL)
  {
    gBS->FreePool(pKeyProvRsp);
    pKeyProvRsp = NULL;
  }

  return Status;
}

/**
  EFI_Encrypt ()

  @brief
  Implementation of Encrypt of EFI_ENCRYPTION_PROTOCOL

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

  @return
  EFI_SUCCESS           -- Function completed successfully.
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  EFI_UNSUPPORTED       -- Function is not supported in this context.

 */

EFI_STATUS
Encrypt(
   IN EFI_ENCRYPTION_ALGORITHM_TYPE   Algorithm,
   IN UINT8                          *BufferIn,
   IN UINT32                          BufferInSize,
   OUT UINT8                        **EncryptedData,
   OUT UINT32                        *EncryptedDataSize
   )
{

  EFI_STATUS Status = EFI_SUCCESS;

  if ((BufferIn == NULL) || (EncryptedData == NULL) || (EncryptedDataSize == NULL) || (BufferInSize == 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (Algorithm)
  {
    case TZ_HDCP_KEY_CEK:
    case TZ_HDCP_KEY_DATA:
      Status = tzt_key_provision_service_encrypt_syscall(BufferIn, BufferInSize, EncryptedData, EncryptedDataSize);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }
  return Status;

}

/**
  EFI_Decrypt ()

  @brief
  Implementation of Decrypt of EFI_ENCRYPTION_PROTOCOL

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

EFI_STATUS
Decrypt(
   IN EFI_ENCRYPTION_ALGORITHM_TYPE   Algorithm,
   IN UINT8                         *BufferIn,
   IN UINT32                          BufferInSize,
   OUT UINT8                        **DecryptedData,
   OUT UINT32                        *DecryptedDataSize
   )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if ((BufferIn == NULL) || (DecryptedData == NULL) || (DecryptedDataSize == NULL) || (BufferInSize == 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (Algorithm)
  {
    case TZ_HDCP_KEY_CEK:
      Status = tzt_key_provision_service_decrypt_syscall(BufferIn, BufferInSize, DecryptedData, DecryptedDataSize, KEY_PROVISION_SET_CEK_BLOB);
      break;
    case TZ_HDCP_KEY_DATA:
      Status = tzt_key_provision_service_decrypt_syscall(BufferIn, BufferInSize, DecryptedData, DecryptedDataSize, KEY_PROVISION_SET_PROVISIONING_DATA_BLOB);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }
  return Status;

}
/**
*  Handshake ()
*
*  @brief
*  perform handshake with WinSecApp
*
*  @param[in]  BufferIn          handshake key buffer
*  @param[in]  BufferInSize      handshake key buffer size
*
*  @return
*  EFI_STATUS
*/
EFI_STATUS
Handshake(
   IN UINT8                         *BufferIn,
   IN UINT32                          BufferInSize
   )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if ((BufferIn == NULL) || (BufferInSize == 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
*  SetHdcp()
*
*  @brief
*  Set Hdcp provisioning data blob and cek blob
*
*  @return
*  EFI_STATUS
*/

EFI_STATUS
SetHdcp()
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_DPP_PROTOCOL                *DppProtocol = NULL;
	UINTN                            HdcpDataSize = 0;
    UINTN                            HdcpCekSize = 0;
    UINT8                           *HdcpDataBuffer = NULL;
    UINT8                           *HdcpDataDecryptedBuffer = NULL;
    UINT8                           *HdcpCekBuffer = NULL;
    UINT8                           *HdcpCekDecryptedBuffer = NULL;

    Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_INFO,"SetHcdp: LocateDppProcotol failed: %r \n", Status));
    goto Exit;
    }

    /* Get the size of HDCP Cek. */
    Status = DppProtocol->GetDPP(DppProtocol,
                                 L"QCOM",
                                 L"HDCPCEK.PROVISION",
                                 NULL,
                                 &HdcpCekSize);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN,"SetHcdp: GetDPP returned: %r \n", Status));
    goto Exit;
    }
    DEBUG((EFI_D_INFO,"SetHcdp: HDCPCEK size = %d \n",HdcpCekSize));

    HdcpCekBuffer = (UINT8 *)AllocateZeroPool(HdcpCekSize);
    if(HdcpCekBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
        DEBUG((EFI_D_WARN,"SetHcdp: Failed to allocate memory for HdcpCekBuffer\n"));
    goto Exit;
    }
    /* Read HDCP Cek into a temporary buffer. */
    Status = DppProtocol->GetDPP(DppProtocol,
                                 L"QCOM",
                                 L"HDCPCEK.PROVISION",
                                 HdcpCekBuffer,
                                 &HdcpCekSize);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN,"SetHcdp: GetDPP returned: %r\n",Status));
    goto Exit;
    }
    else
    {
        /*Set HDCP Cek by decrypting the temporary buffer, note that
         the decrypted HDCP Cek data is not returned/zeroed out in output buffer */
        Status = Decrypt(TZ_HDCP_KEY_CEK,
                         HdcpCekBuffer,
                         (UINT32)HdcpCekSize,
                         &HdcpCekDecryptedBuffer,
                         (UINT32*)&HdcpCekSize);

        if(EFI_ERROR(Status))
        {
            DEBUG((EFI_D_WARN,"SetHcdp: HDCP Cek Decryption request returned: %r \n",Status));
            goto Exit;
        }
        DEBUG((EFI_D_INFO,"SetHcdp: HDCP Cek Decryption success\n"));
    }

    /* Get the size of HDCP Data. */
    Status = DppProtocol->GetDPP(DppProtocol,
                                 L"QCOM",
                                 L"HDCPDATA.PROVISION",
                                 NULL,
                                 &HdcpDataSize);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN,"SetHcdp: GetDPP data size returned: %r \n", Status));
    goto Exit;
    }
    DEBUG((EFI_D_INFO,"SetHcdp: HDCP Data size = %d \n",HdcpDataSize));

    HdcpDataBuffer = (UINT8 *)AllocateZeroPool(HdcpDataSize);
    if(HdcpDataBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
        DEBUG((EFI_D_WARN,"SetHcdp: Failed to allocate memory for HdcpDataBuffer\n"));
    goto Exit;
    }

    /* Read HDCP Data into a temporary buffer. */
    Status = DppProtocol->GetDPP(DppProtocol,
                                 L"QCOM",
                                 L"HDCPDATA.PROVISION",
                                 HdcpDataBuffer,
                                 &HdcpDataSize);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN,"SetHcdp: GetDPP cek size returned: %r \n",Status));
    goto Exit;
    }

    /*Set HDCP Data by decrypting the temporary buffer, note that
         the decrypted HDCP data is not returned/zeroed out in output buffer */
    Status = Decrypt(TZ_HDCP_KEY_DATA,
                     HdcpDataBuffer,
                     (UINT32)HdcpDataSize,
                     &HdcpDataDecryptedBuffer,
                     (UINT32*)&HdcpDataSize);

    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN,"SetHcdp: HDCP Data Decryption request returned: %r \n",Status));
    goto Exit;
    }
    DEBUG((EFI_D_INFO,"SetHcdp: HDCP Data Decryption success\n"));

Exit:

  if (HdcpDataBuffer != NULL)
  {
    gBS->FreePool(HdcpDataBuffer);
        HdcpDataBuffer = NULL;
  }
  if (HdcpDataDecryptedBuffer != NULL)
  {
    gBS->FreePool(HdcpDataDecryptedBuffer);
        HdcpDataDecryptedBuffer = NULL;
  }
  if (HdcpCekBuffer != NULL)
  {
    gBS->FreePool(HdcpCekBuffer);
        HdcpCekBuffer = NULL;
  }
  if (HdcpCekDecryptedBuffer != NULL)
  {
    gBS->FreePool(HdcpCekDecryptedBuffer);
        HdcpCekDecryptedBuffer = NULL;
  }
  return Status;
}

/**
  Encryption UEFI Protocol implementation
 */
EFI_ENCRYPTION_PROTOCOL EncryptionProtocol =
{
  EFI_ENCRYPTION_PROTOCOL_REVISION,
  Encrypt,
  Decrypt,
  Handshake,
  SetHdcp
};

/**
  Encryption DXE Driver Entry Point
 */
EFI_STATUS
EncryptionProtocolInit(
   IN EFI_HANDLE         ImageHandle,
   IN EFI_SYSTEM_TABLE   *SystemTable
   )
{
  EFI_STATUS              Status;

  // Locate QCOM_SCM_PROTOCOL.
  Status = gBS->LocateProtocol(&gQcomScmProtocolGuid,
                               NULL,
                               (VOID **)&pQcomScmProtocol
                               );
  if (EFI_ERROR(Status) || (pQcomScmProtocol == NULL))
  {
    DEBUG((EFI_D_WARN, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    goto Exit;
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
     &ImageHandle,
     &gEfiEncryptionProtocolGuid,
     (void **)&EncryptionProtocol,
     NULL);

Exit:

  return Status;
}

