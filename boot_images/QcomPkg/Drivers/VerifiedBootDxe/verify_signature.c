/** @file
                        verify_signature.c

  Wrapper function for ce_rsa_verify_signature

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 09/15/16   SA     Use SecRSA Protocol
 07/28/16   SA     Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Hash2.h>
#include <Protocol/EFISecRSA.h>
#include "verify_signature.h"

#define EFI_FAILURE (80)

EFI_STATUS VerifySignature(
    CONST UINT8 *signature_ptr,
    UINT32 signature_len,
    UINT8 *hash,
    VB_HASH hash_algorithm,
    CONST UINT8 *modulus,
    UINT32 modulus_len,
    CONST UINT8 *public_exp,
    UINT32 public_exp_len)
{

  EFI_STATUS status = EFI_FAILURE;
  CE_RSA_KEY key;
  BigInt modulus_bi;
  BigInt public_exp_bi;
  INT32 hashidx;
  INT32 hash_len;
  UINT32 padding_type;
  VOID *padding_info = NULL;
  QCOM_SECRSA_PROTOCOL *pEfiQcomSecRSAProtocol = NULL;
  SetMem(&key, sizeof(CE_RSA_KEY), 0);

  switch (hash_algorithm) {
  case VB_SHA256:
    hashidx = CE_HASH_IDX_SHA256;
    hash_len = VB_SHA256_SIZE;
    break;
  default:
    DEBUG((EFI_D_ERROR, "VB: VerifySignature: Hash algorithm not supported\n"));
    status = EFI_UNSUPPORTED;
    goto exit;
  }
  key.N = AllocatePool(sizeof(S_BIGINT));
  if (key.N == NULL) {
    DEBUG((EFI_D_ERROR, "VB: VerifySignature: mem allocation err for key.N\n"));
    goto exit;
  }
  key.e = AllocatePool(sizeof(S_BIGINT));
  if (key.e == NULL) {
    DEBUG((EFI_D_ERROR, "VB: VerifySignature: mem allocation err for key.e\n"));
    goto exit;
  }
  status = gBS->LocateProtocol(&gEfiQcomSecRSAProtocolGuid, NULL, (VOID **) &pEfiQcomSecRSAProtocol);
  if ( status != EFI_SUCCESS)
  {
     DEBUG((EFI_D_ERROR, "VB: VerifySignature: LocateProtocol failed, status = %x\n", status));
    goto exit;
  }
  status = pEfiQcomSecRSAProtocol->SecRSABigIntReadBin(pEfiQcomSecRSAProtocol, modulus, modulus_len, &modulus_bi);
  if ( status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "VB: VerifySignature: SecRSABigIntReadBin for modulus failed!, ret = %x\n", status));
    goto exit;
  }
  status = pEfiQcomSecRSAProtocol->SecRSABigIntReadBin(pEfiQcomSecRSAProtocol, public_exp, public_exp_len,  &public_exp_bi);
  if ( status != EFI_SUCCESS)
  {
	DEBUG((EFI_D_ERROR, "VB: VerifySignature: SecRSABigIntReadBin for modulus failed!, ret = %x\n", status));
	goto exit;
  }

  key.N->bi = modulus_bi;
  key.e->bi = public_exp_bi;
  key.e->sign = S_BIGINT_POS;
  key.type = CE_RSA_KEY_PUBLIC;
  padding_type = CE_RSA_PAD_PKCS1_V1_5_SIG;

  status = pEfiQcomSecRSAProtocol->SecRSAVerifySig(pEfiQcomSecRSAProtocol, &key, padding_type, padding_info, hashidx,
                                   hash, hash_len, (UINT8*)signature_ptr, signature_len);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  if (key.N != NULL) {
    FreePool(key.N);
  }
  if (key.e != NULL) {
    FreePool(key.e);
  }
  return status;
}
