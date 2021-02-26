/** @file
                        boot_verifier.c

  This files contains the main internal functions that are used for image verification.

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 09/15/16   SA     Use ASNX509 Protocol
 07/28/16   SA     Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Hash2.h>
#include "verify_signature.h"
#include "boot_verifier.h"
#include "oem_certificate.h"
#include "hash_defs.h"

#define PAGE_SIZE 4096
#define EFI_FAILURE (80)

/**

	Read Android Verified Boot Signature appended at the end of image
    and verify whether it follows the ASN1 standard format.

	@param[in]      *img			Pointer to the start address of the image.
	@param[in]      img_len         Size of the image (Actual size of the image...size of image minus the appended VB signature).
	@param[out]     *vb_signature	Verified VB signature (output).

    @retval         status          Indicates reading VB signature and verifying its format was successful


**/
EFI_STATUS vb_read_embedded_vb_signature(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *img, UINTN img_len,
                                         VERIFIED_BOOT_SIG *vb_signature)
{

  EFI_STATUS status = EFI_FAILURE;
  UINT8 *vb_sig_addr = img + img_len;

  status = pEfiQcomASN1X509Protocol->ASN1X509VerifyVBSig(pEfiQcomASN1X509Protocol, (CONST UINT8 *)vb_sig_addr, PAGE_SIZE, vb_signature);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Read the OEM certificate from the embedded header file
	and verify whether it follows the ASN1 standard format

	@param[out] 	*certificate	Verified certificate (output)

	@retval         status          Indicates whether reading certificate and verifying its format was successful

**/
EFI_STATUS vb_read_oem_certificate(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, CERTIFICATE *certificate)
{
  EFI_STATUS status = EFI_FAILURE;
  CONST UINT8 *input = OEM_CERTIFICATE;
  UINTN len = sizeof(OEM_CERTIFICATE);

  status = pEfiQcomASN1X509Protocol->ASN1X509VerifyOEMCertificate(pEfiQcomASN1X509Protocol, input, len, certificate);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Calculate the image hash.

	@param[in]	*pname			Name of image.
	@param[in]  *img			Pointer to the start address of the image.
	@param[in]  img_len			Size of the image (actual size of image...size of image minus the appended VB signature).
	@param[in]  *vb_signature	Pointer to the valid (verified) VB signature.
    @param[out] out_hash		Calculated image hash (output).
 	@param[in]  out_hash_size   Size of the hash.
	@param[in]	hash_algorithm  Hash algorithm.

	@retval status              Indicates whether calculating image hash was successful.

**/
EFI_STATUS vb_get_image_hash(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *pname, UINT8 *img, UINTN img_len,
                             VERIFIED_BOOT_SIG *vb_signature, UINT8 *out_hash,
                             UINTN out_hash_size, VB_HASH hash_algorithm)
{

  EFI_STATUS status = EFI_FAILURE;
  EFI_GUID *HashAlgorithm;
  UINTN DigestSize = 0;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;
  CONST UINT8 *auth_attr_ptr = vb_signature->auth_attr;
  UINTN auth_attr_len = vb_signature->auth_attr_len;
  CONST UINT8 *auth_data = auth_attr_ptr;
  secasn1_data_type target;
  secasn1_data_type length;

  switch (hash_algorithm) {

  case VB_SHA256:
    HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
    break;
  default:
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: not supported algorithm: %d \n", hash_algorithm));
    status = EFI_UNSUPPORTED;
    goto exit;
  }
  /* check the name and length of image matches the image name in the authenticated attributes */
  status = pEfiQcomASN1X509Protocol->ASN1X509GetAuthAttrFields(pEfiQcomASN1X509Protocol, auth_data, auth_attr_len, &target, &length);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: auth attrs err!\n"));
    goto exit;
  }
  if (target.len != AsciiStrLen((CONST CHAR8 *)pname)) 
  {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: Invalid len for image name!\n"));
	status = EFI_FAILURE;
    goto exit;
  }
  if (AsciiStrnCmp((CONST CHAR8 *)target.data, (CONST CHAR8 *)pname, target.len) !=
      0) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: name mismatch!\n"));
    status = EFI_FAILURE;
    goto exit;
  }
  UINT32 len = *((UINT32 *)(length.data));
  if (length.len > sizeof(UINT32)) {
  	DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: invalid len\n"));
    status = EFI_FAILURE;
    goto exit;
  }
  UINT32 shift_bytes = sizeof(UINT32) - length.len;
  if (shift_bytes > 0) {
    len = len << (shift_bytes * 8);
  }
  len = SwapBytes32(len);

  if (len != img_len) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: size mismatch\n"));
    status = EFI_FAILURE;
    goto exit;
  }
  if (pEfiHash2Protocol == NULL) {
    status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid,
                                 NULL,
                                 (VOID **)&pEfiHash2Protocol);
    if (status != EFI_SUCCESS) {

      DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: LocateProtocol unsuccessful! status: %d\n", status));
      goto exit;
    }
  }
  status = pEfiHash2Protocol->GetHashSize(pEfiHash2Protocol, HashAlgorithm,
                                          &DigestSize);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: GetHashSize unsuccessful! status: %d\n", status));
    goto exit;
  }
  if (out_hash_size != DigestSize) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: Invalid size! out_hash_size: %d, DigestSize: %d\n", out_hash_size, DigestSize));
    status = EFI_FAILURE;
    goto exit;
  }
  status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashInit unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, img, img_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashUpdate unsuccessful(img)! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, auth_attr_ptr,
                                         auth_attr_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashUpdate unsuccessful(auth_attr)! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);

  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashFinal unsuccessful! status = %d\n", status));
    goto exit;
  }
  CopyMemS(out_hash, out_hash_size, (CONST UINT8 *)&Hash2Output, DigestSize);
  status = EFI_SUCCESS;

exit:
  return status;
}

/**

	Get hash of the buffer containing modulus, public exponent and lock status of device to send as the root of trust.

	@param[in]	*modulus		Pointer to the modulus.
	@param[in]  modulus_len     size of the modulus.
	@param[in]  *exp			Pointer to public exponent.
	@param[in]  exp_len			size of the public exponent.
	@param[in]  devinfo         lock status of device.
    @param[out] out_hash		Calculated hash (output).
 	@param[in]  out_hash_size   Size of the hash.
	@param[in]	hash_algorithm  Hash algorithm.

	@retval     status          Indicates whether calculating image hash was successful.

**/
EFI_STATUS vb_get_digest_locked_device(CONST UINT8 *modulus, UINTN modulus_len,
                                       CONST UINT8 *exp, UINTN exp_len,
                                       device_info_vb_t devinfo, UINT8 *out_hash,
                                       VB_HASH hash_algorithm, UINTN out_hash_size)
{

  EFI_STATUS status = EFI_FAILURE;
  EFI_GUID *HashAlgorithm;
  UINTN DigestSize = 0;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;

  switch (hash_algorithm) {

  case VB_SHA256:
    HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
    break;
  default:
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: not supported algorithm: %d\n", hash_algorithm));
    status = EFI_UNSUPPORTED;
    goto exit;
  }
  if (pEfiHash2Protocol == NULL) {
    status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid,
                                 NULL,
                                 (VOID **)&pEfiHash2Protocol);
    if (status != EFI_SUCCESS) {

      DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: LocateProtocol unsuccessful! status: %d\n", status));
      goto exit;
    }
  }
  status = pEfiHash2Protocol->GetHashSize(pEfiHash2Protocol, HashAlgorithm,
                                          &DigestSize);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: GetHashSize unsuccessful! status: %d\n", status));
    goto exit;
  }
  if (out_hash_size != DigestSize) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: Invalid size! out_hash_size: %d, DigestSize: %d\n", out_hash_size, DigestSize));
    status = EFI_FAILURE;
    goto exit;
  }
  status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: HashInit unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, modulus,
                                         modulus_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: HashUpdate unsuccessful(modulus)! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, exp, exp_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: HashUpdate unsuccessful(public_exp)! status: %d\n", status));
    goto exit;
  }

  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, &devinfo.is_unlocked, sizeof(devinfo.is_unlocked));
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: HashUpdate unsuccessful! status = %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);

  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_locked_device: second HashFinal unsuccessful! status = %d\n", status));
    goto exit;
  }
  CopyMemS(out_hash, out_hash_size, (CONST UINT8 *)&Hash2Output, DigestSize);
  status = EFI_SUCCESS;

exit:
  return status;
}

/**

	Get hash of the buffer (digest) containing to send it as the root of trust when device is unlocked.

	@param[in]	*digest			Input buffer for hashing.
	@param[in]  digest_len		size of inout buffer (digest).
    @param[out] out_hash		Calculated hash of digest (output).
 	@param[in]  out_hash_size   Size of the hash.
	@param[in]	hash_algorithm  Hash algorithm.

	@retval     status          Indicates whether calculating the hash was successful.

**/
EFI_STATUS vb_get_digest_unlocked_device(UINT8 *digest, UINTN digest_len,
                                         UINT8 *out_hash, VB_HASH hash_algorithm, UINTN out_hash_size)
{
  EFI_STATUS status = EFI_FAILURE;
  EFI_GUID *HashAlgorithm;
  UINTN DigestSize = 0;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;

  switch (hash_algorithm) {

  case VB_SHA256:
    HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
    break;
  default:
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: not supported algorithm: %d\n", hash_algorithm));
    status = EFI_UNSUPPORTED;
    goto exit;
  }
  if (pEfiHash2Protocol == NULL) {
    status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid,
                                 NULL,
                                 (VOID **)&pEfiHash2Protocol);
    if (status != EFI_SUCCESS) {

      DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: LocateProtocol unsuccessful! status: %d\n", status));
      goto exit;
    }
  }
  status = pEfiHash2Protocol->GetHashSize(pEfiHash2Protocol, HashAlgorithm,
                                          &DigestSize);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: GetHashSize unsuccessful! status: %d\n", status));
    goto exit;
  }
  if (out_hash_size != DigestSize) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: Invalid size! out_hash_size: %d, DigestSize: %d\n", out_hash_size, DigestSize));
    status = EFI_FAILURE;
    goto exit;
  }
  status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: HashInit unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, digest, digest_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_digest_unlocked_device: HashUpdate unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB:vb_get_digest_unlocked_device: HashFinal unsuccessful! status: %d\n", status));
    goto exit;
  }
  CopyMemS(out_hash, out_hash_size, (CONST UINT8 *)&Hash2Output, DigestSize);
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Verify whether the image hash matches the encrypted hash provided by Android VB signature (appended to the image).
	The encrypted hash should be decrypted by the keys provided by Android VB signature (appended to the image).

	@param[in]      *hash			Hash of the image.
	@param[in]      hash_algorithm  Hash algorithm.
	@param[in]      *vb_signature   Validated (verified) Andoroid Verified Boot Signature.

    @retval         status          Indicates whether the image hash matches with the hash in the VB signature.


**/
EFI_STATUS vb_verify_hash_embedded_key(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *hash, VB_HASH hash_algorithm,
                                       VERIFIED_BOOT_SIG *vb_signature)
{
  EFI_STATUS status = EFI_FAILURE;
  RSA key = {0};
  secasn1_data_type modulus = {0};
  secasn1_data_type public_exp = {0};
  secasn1_data_type signature = {0};

  status = pEfiQcomASN1X509Protocol->ASN1X509GetRSAFromVBSig(pEfiQcomASN1X509Protocol, vb_signature, &key);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = pEfiQcomASN1X509Protocol->ASN1X509GetKeymaterial(pEfiQcomASN1X509Protocol, &key, &modulus, &public_exp);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = pEfiQcomASN1X509Protocol->ASN1X509GetSigFromVBSig(pEfiQcomASN1X509Protocol, vb_signature, &signature);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = VerifySignature(signature.data, signature.len, hash, hash_algorithm,
                           modulus.data, modulus.len, public_exp.data,
                           public_exp.len);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_verify_hash_embedded_key: Verifying signature unsuccessful!\n"));
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Verify whether the image hash matches the hash provided by by Android VB signature (appended to the image).
	The encrypted hash should be decrypted by the keys provided by OEM certificate (embedded in the VerifiedBootDxe driver (oem_certificate.h)).

	@param[in]      *hash			Hash of the image.
	@param[in]      hash_algorithm  Hash algorithm.
	@param[in]      *certificate    Validated (verified) certificate.
	@param[in]      *vb_signature   Validated (verified) Andoroid Verified Boot Signature.

    @retval         status             Indicates whether the image hash matches with the hash in the VB signature (decrypted by the key provided by OEM certificate).
    								In case of success, ret is set to 1.

**/
EFI_STATUS vb_verify_hash_oem_certificate(QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol, UINT8 *hash, VB_HASH hash_algorithm,
                                          CERTIFICATE *certificate,
                                          VERIFIED_BOOT_SIG *vb_signature)
{
  EFI_STATUS status = EFI_FAILURE;
  RSA key = {0};
  secasn1_data_type modulus = {0};
  secasn1_data_type public_exp = {0};
  secasn1_data_type signature = {0};

  status = pEfiQcomASN1X509Protocol->ASN1X509GetRSAFromCert(pEfiQcomASN1X509Protocol, certificate, &key);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = pEfiQcomASN1X509Protocol->ASN1X509GetKeymaterial(pEfiQcomASN1X509Protocol, &key, &modulus, &public_exp);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = pEfiQcomASN1X509Protocol->ASN1X509GetSigFromVBSig(pEfiQcomASN1X509Protocol, vb_signature, &signature);
  if (status != EFI_SUCCESS) {
    goto exit;
  }
  status = VerifySignature(signature.data, signature.len, hash, hash_algorithm,
                           modulus.data, modulus.len, public_exp.data,
                           public_exp.len);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_verify_hash_oem_certificate: Verifying signature unsuccessful!\n"));
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Get hash of certificate in the Yellow state.

	@param[in]      *cetificate		pointer to certificate in DER format.
	@param[in]      certificate_len length of certificte.
	@param[out]     *buf            output hash.
	@param[in]      buf_len         length of output.

    @retval         status          Indicates whether optaining certificate hash (fingerprint) is succeeded.

**/

EFI_STATUS vb_get_cert_fingerprint(CONST UINT8 *certificate, UINTN certificate_len, UINT8 *buf, UINTN buf_len, UINTN *out_len)
{

  EFI_STATUS status = EFI_FAILURE;
  EFI_GUID *HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
  UINTN DigestSize = 0;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;

  status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid,
                               NULL,
                               (VOID **)&pEfiHash2Protocol);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: LocateProtocol unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->GetHashSize(pEfiHash2Protocol, HashAlgorithm,
                                          &DigestSize);
  if (status != EFI_SUCCESS) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: GetHashSize unsuccessful! status: %d\n", status));
    goto exit;
  }
  if (buf_len < DigestSize) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: buf_len is too small\n"));
	status = EFI_BUFFER_TOO_SMALL;
	goto exit;
  }
  status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: HashInit unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, certificate, certificate_len);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: HashUpdate unsuccessful! status: %d\n", status));
    goto exit;
  }
  status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);
  if (EFI_SUCCESS != status) {

    DEBUG((EFI_D_ERROR, "VB: vb_get_cert_fingerprint: HashFinal unsuccessful! status: %d\n", status));
    goto exit;
  }
  *out_len = DigestSize;
  CopyMemS(buf, buf_len, (UINT8 *)&Hash2Output, DigestSize);
  status = EFI_SUCCESS;
exit:
  return status;
}
