/** @file
                        boot_verifier.h

  Header file containing the main image verification prototypes

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 09/15/16   SA     Use ASN1X509 protocol
 07/28/16   SA     Initial version

=============================================================================*/

#ifndef __BOOT_VERIFIER_H
#define __BOOT_VERIFIER_H

#include <Uefi.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Protocol/EFIVerifiedBoot.h>
#include <Protocol/EFIASN1X509.h>
#include "hash_defs.h"

/*Struct def for boot image header*/
typedef struct boot_img_hdr
{
  CHAR8 magic[8];
  UINT32 kernel_size;  /* size in bytes */
  UINT32 kernel_addr;  /* physical load addr */
  UINT32 ramdisk_size; /* size in bytes */
  UINT32 ramdisk_addr; /* physical load addr */
  UINT32 second_size;  /* size in bytes */
  UINT32 second_addr;  /* physical load addr */
  UINT32 tags_addr;    /* physical addr for kernel tags */
  UINT32 page_size;    /* flash page size we assume */
  UINT32 dt_size;      /* device_tree in bytes */
  UINT32 os_version;   /* version << 11 | patch_level */
  UINT8 name[16]; /* ascii product name */
  UINT8 cmdline[512];
  UINT32 id[8]; /* timestamp / checksum / sha1 / etc */
}boot_img_hdr;

EFI_STATUS vb_read_oem_certificate(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, CERTIFICATE *certificate);

EFI_STATUS vb_get_image_hash(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *pname, UINT8 *img, UINTN img_len, VERIFIED_BOOT_SIG *vb_signature, UINT8 *out_hash, UINTN out_hash_size, VB_HASH hash_algorithm);

EFI_STATUS vb_get_digest_locked_device(CONST UINT8 *modules, UINTN modules_len, CONST UINT8 *exp, UINTN exp_len, device_info_vb_t devinfo, UINT8 *out_hash, VB_HASH hash_algorithm, UINTN hash_size);

EFI_STATUS vb_get_digest_unlocked_device(UINT8 *digest, UINTN digest_len, UINT8 *out_hash, VB_HASH hash_algorithm, UINTN hash_size);

EFI_STATUS vb_read_embedded_vb_signature(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *img, UINTN img_len, VERIFIED_BOOT_SIG *vb_signature);

EFI_STATUS vb_verify_hash_embedded_key(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *hash, VB_HASH hash_algorithm, VERIFIED_BOOT_SIG *vb_signature);

EFI_STATUS vb_verify_hash_oem_certificate(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *img_hash, VB_HASH hash_algorithm, CERTIFICATE *certificate, VERIFIED_BOOT_SIG *vb_signature);

EFI_STATUS vb_get_cert_fingerprint(CONST UINT8 *certificate, UINTN certificate_len, UINT8 *buf, UINTN buf_len, UINTN *out_len);


#endif
