#ifndef SECBOOT_H
#define SECBOOT_H

/** @file SecBoot.h

  This header file contains declarations and definitions for the 
  APPSBL interface to the software that authenticates the HLOS.

DESCRIPTION: 
		This is the interface into the
        secure boot API. The secure boot API checks if the
        certificate chain is correct (attest cert -> qualcomm
        certification authority certificate -> root cert in ROM).
        Then the attestation certificate is used to validate
        the secondary boot loader image.

EXTERNALIZED FUNCTIONS
        secboot_verify_x509_chain() - Checks a certificate
                chain to see if it is valid and chains back
                to the trusted root.
        secboot_verify_code_signature() - Checks if
                the code image can be verified by the attestation
                certificate to match the signature.

 Hierarchy of dependencies in files for the secure boot process

                      secboot.h (this file, the entry point for this API)
                          |
                      secboot.c--------- secboot_i.h/c (internal, helper
                                                        functions)
                          |
               +--------------+-------+----------------------+
               |                      |                      |
  secboot_x509.h/c       secboot_rsa.h/c        secboot_sha1.h/c
   ( x509 parser)       (RSA operations)        (SHA1 interfaces)
         |
         |
  secboot_asn1.h/c
   ( asn1 library)


	 Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
   **/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------


=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "SecBoot_i.h"
#include "SecBoot_rsa_math.h"
#include "SecBoot_hash.h"
#include "SecBoot_x509.h"

typedef struct
{
  /* General info */
  boolean    info_is_valid;
  UINT8       common_name[SECBOOT_COMMON_NAME_SIZE];

  /* Public key info of the CA */
  struct
  {
    UINT8    exponent[MAX_KEY_SIZE];
    UINT8    modulus[MAX_KEY_SIZE];
    UINT32   size;
  } pubkey_info;

  /* Signature of the CA cert, used to compare with CA cert in chain */
  UINT8      signature[SECBOOT_SIGNATURE_SIZE];

} secboot_ca_type;

/*===========================================================================

                              INTERFACE FUNCTIONS

===========================================================================*/


#endif /* SECBOOT_H */

