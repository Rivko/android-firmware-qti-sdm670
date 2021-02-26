#ifndef SECBOOT_API_H
#define SECBOOT_API_H

/** @file SecBoot_api.h

  This header file contains declarations and definitions for the 
  APPSBL interface to the software that authenticates the HLOS.

  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "com_dtypes.h"
#include "SecBoot_types.h"
#include "SecBoot_errno.h"

/*===========================================================================
FUNCTION SECBOOT_VERIFY_CODE_SIGNATURE

DESCRIPTION
  This function validates a signature over the concatenattion of code_1 
  (typically the header) and code_2 (typically the code image).
  Either of both of these can be of zero length, in which case,
  code_ptr_? an cbe NULL.

DEPENDENCIES
  None

PARAMETERS
  code_ptr_1            - Address of the code image 1
  code_len_1            - Size of the code image 1 in bytes
  code_ptr_2            - Address of the code image 2 
  code_len_2            - Size of the code image 2in bytes
  msm_hw_id             - ID of the MSM (read from MSM register)
  sw_vsn_id             - Software version ID
  signature_ptr         - Address of the code signature to check
  signature_len         - Length of the code signature to check in bytes
  attestation_cert_info - Pointer to parsed info about attestation cert
                          (returned from secboot_verify_x509_chain). 

RETURN VALUE
  TRUE if the signature check passed.

SIDE EFFECTS
  None
===========================================================================*/
boolean secboot_verify_code_signature
(
  const uint8*            code_ptr_1,
  uint32                  code_len_1,
  const uint8*            code_ptr_2,
  uint32                  code_len_2,
  uint64                  msm_hw_id,
  uint64                  sw_vsn_id,
  const uint8*            signature_ptr,
  uint32                  signature_len,
  const secx509_cert_type*  attestation_cert_info
);

/*===========================================================================
FUNCTION SECBOOT_VERIFY_X509_CHAIN

DESCRIPTION
  This function parses a list of certificates authenticating a code
  signature.

DEPENDENCIES
  None

PARAMETERS
  x509_chain_ptr     - Address of certificate chain
  x509_chain_len     - Length of cert chain, including padding
  root_of_trust_is_cert - Tells how to interpret root_of_trust_ptr and
                              size
  root_of_trust_ptr  - Address of the fingerprint (hash) of the associated
                       root cert or it can be Address of the root cert
  root_of_trust_size - Size of the fingerprint (hash) of the associated
                       root cert or it can be size of the root cert
  qc_ca_ptr          - Address of Qualcomm CA storage, filled in by function
                       if the chain is using the Qualcomm CA
  oem_ca_ptr         - Address of OEM CA storage, filled in by function
                       if the chain is using the OEM CA
  ou_field_info_ptr  - Address of ou field info from attestation certificate,
                       filled in by function
  qfprom_info_ptr    - Address of qfprom info regarding the root cert to select
                       among list of OEM provisioned root certs

RETURN VALUE
  Pointer to the attestation certificate info, NULL if validation failed.
  Outside the security code, this should only be tested against NULL, and 
  passed to secboot_verify_code.

SIDE EFFECTS
  None
===========================================================================*/
secx509_cert_type* secboot_verify_x509_chain
(
  const uint8*                  x509_chain_ptr,
  uint32                        x509_chain_len, /* includes padding */
  boolean                       root_of_trust_is_cert,
  const uint8*                  root_of_trust_ptr,
  uint32                        root_of_trust_size,
  secx509_cert_type**       qc_ca_ptr,
  secx509_cert_type**       oem_ca_ptr,
  secboot_ou_field_info_type*   ou_field_info_ptr,
  secboot_qfprom_info_type*     qfprom_info_ptr
);

#endif /* SECBOOT_API_H */
