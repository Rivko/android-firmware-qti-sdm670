#ifndef SECBOOT_X509_H
#define SECBOOT_X509_H

/** @file SecBoot_x509.h
 
DESCRIPTION:
 	Secure Boot X.509 PARSER header file
 
  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who                  what, where, why
--------   ----                 -------------------------------------------

===========================================================================*/


/*==========================================================================

                          Include Files

==========================================================================*/
#include "SecBoot_i.h"
#include "SecBoot_asn1.h"

/*==========================================================================

                          Constant Declaration

==========================================================================*/
#define SECX509_NUM_CERTS         NUM_CERTS
#define SECX509_MAX_CERT_LEN                2000
#define SECX509_MAX_VRFY_SIG_LEN (MAX_KEY_SIZE/8) // in bytes, sign size
                                                      // same as key length

/* Key Usage Masks */
#define SECX509_KEY_USAGE_DIG_SIG   (0x0100) /* digital signature */
#define SECX509_KEY_USAGE_NON_REP   (0x0080) /* non-repudiation   */
#define SECX509_KEY_USAGE_KEY_ENC   (0x0040) /* key encipherment  */
#define SECX509_KEY_USAGE_DAT_ENC   (0x0020) /* data encipherment */
#define SECX509_KEY_USAGE_KEY_ARG   (0x0010) /* key agreement     */
#define SECX509_KEY_USAGE_KEY_CRT   (0x0008) /* key cert sign     */
#define SECX509_KEY_USAGE_CRL_SIG   (0x0004) /* CRL sign          */
#define SECX509_KEY_USAGE_ENC_OLY   (0x0002) /* encipher only     */
#define SECX509_KEY_USAGE_DEC_OLY   (0x0001) /* decipher only     */
/* Extended Key Usage Masks */
#define SECX509_EX_KEY_USAGE_SAUTH  (0x0001) /* TLS Web Server Authentication*/
#define SECX509_EX_KEY_USAGE_CAUTH  (0x0002) /* TLS Web Client Authentication*/
#define SECX509_EX_KEY_USAGE_CODE   (0x0004) /* Downloadable Code Signing    */
#define SECX509_EX_KEY_USAGE_EMAIL  (0x0008) /* Email Protection             */
#define SECX509_EX_KEY_USAGE_TIME   (0x0010) /* Time Stamping                */
#define SECX509_EX_KEY_USAGE_SGC    (0x0020) /* Secured Gated Crypto         */

/*==========================================================================

                       Type Definitions

==========================================================================*/



/* Certificate list struct */
typedef struct secx509_cert_list_struct
{
  secx509_cert_type  cert[SECX509_NUM_CERTS];
  UINT32               size;

} secx509_cert_list_type;

/* certificate list context type */
typedef struct
{
  UINT32                       purpose;
  UINT32                       trust;
  UINT32                       depth;
  secx509_cert_list_type*  ca_list;

} secx509_cert_ctx_type;


typedef enum
{
  E_X509_FAILURE = 0,
  E_X509_SUCCESS,
  E_X509_NO_DATA,
  E_X509_DATA_INVALID,
  E_X509_BAD_DATA,
  E_X509_DATA_TOO_LARGE,
  E_X509_DATA_EXPIRED,
  E_X509_NO_MEMORY,
  E_X509_INVALID_ARG,
  E_X509_NOT_SUPPORTED,
  E_X509_RESERVED       = 0x7FFFFFFF
} secx509_errno_enum_type;

#ifdef IMEM_BYTE_ACCESS
/*===================================================================
 Inserted here by sid ... used now and should be useful later on,
 will want to see if unless memcmp would work in this scenario ....
====================================================================*/

extern UINT16 secutil_byte_compare
(
  const UINT8*  value1_ptr,
  const UINT8*  value2_ptr,
  const UINT16  byte_len
);
#endif

//#define MIN(x,y) (((x) <= (y)) ? (x) : (y))


/*==========================================================================

                      GLOBAL FUNCTION DECLARATIONS

==========================================================================*/

/*===========================================================================

FUNCTION SECX509_PARSE_CERTIFICATE

DESCRIPTION
  Parses an X509 certificate from the data_ptr, if *cert == NULL then the
  certificate is allocated from the memory pool, if the certificate is
  already allocated then the function just uses the pre-allocated memory

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the raw certificate data
  data_bound - cert must end before here
  cert - pointer to the certificate
  ou_field_info_ptr - pointer to store OU Field values

RETURN VALUE
  E_SUCCESS if the certificate is parsed properly
  E_DATA_INVALID if the certificate cannot be parsed properly
  E_NO_MEMORY if no more memory slots are available for the certs
  E_NOT_SUPPORTED if an algorithm found in the cert is not supported
  E_INVALID_ARG if a pointer argument is NULL
  E_FAILURE if the *cert is pre-allocated but not *cert->cert or if the
            certificate data length does not match cert_len

SIDE EFFECTS
  None
===========================================================================*/
extern secx509_errno_enum_type secx509_parse_certificate
(
  const UINT8                *cert_data,  /* cert data in the DER format */
  const UINT8                *data_bound, /* parsing must stop before this address */
  secx509_cert_type      *cert,       /* pointer to the parsed struct*/
  secboot_ou_field_info_type *ou_field_info_ptr
);

/*===========================================================================
FUNCTION secx509_parse_cert_buffer

DESCRIPTION
  Parses binary x509 certificates from memory into the buffer.
  parsing is done in order (attest cert first, CA cert next and
  root cert last).

DEPENDENCIES
  None

PARAMETERS
  root_cert_ptr           - points to x509 Root certificate (in ROM)
  cert_chain_ptr          - points to x509 certificate chain (in Flash)
                            CA cert and Attestation cert are here
  secx509_cert_list   - destination for all the certificate
                            after they have been parsed in order
                            of chaining.
  ou_field_info_ptr       - pointer to store OU Field values                            
  qfprom_info_ptr         - points to info about the root cert to select among
                            list of OEM provisioned root certs

RETURN VALUE
  E_SUCCESS on successful parsing of all certs
  E_FAILURE on unsuccessful parsing of all certs

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type secx509_parse_cert_buffer
(
  const UINT8                 *cert_chain_ptr,
  UINT32                       cert_chain_len, /* buffer len, to check for bogus internal len */
  secx509_cert_list_type  *secx509_cert_list,
  secboot_ou_field_info_type  *ou_field_info_ptr,
  secboot_qfprom_info_type    *qfprom_info_ptr
);

/*===========================================================================
FUNCTION secx509_check_cert_list

DESCRIPTION
  Simpy checks if the certificate information, over the whole chain
  is valid and that the issuer and subject ID's are chained
  consecutively. Cryptographic signature verification down the chain
  is _NOT_ done here.

DEPENDENCIES
  None

PARAMETERS
  *cert_list_ptr  - Pointer to array of parsed certificates
  security_alert_mask_ptr - Set any check alerts in this quantity (flag bits)
  *check_ctx              - Sets of the parameters for the checking process

RETURN VALUE
  E_SUCCESS       - if no error
  E_DATA_INVALID  - Invalid Data
  E_NOT_SUPPORTED - Unknown Data

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type secx509_check_cert_list
(
  secx509_cert_list_type      *cert_list_ptr,
  UINT32* security_alert_mask_ptr,
  secx509_cert_ctx_type       *check_ctx
);

/*===========================================================================

FUNCTION SECX509_GET_ASN1_CERT_SIZE

DESCRIPTION
  Gets the size (in bytes) of the certificate

DEPENDENCIES
  None

PARAMETERS
  data_ptr          - pointer to the raw certificate data
  data_bound        - address beyond which the certificate does not cross into
  asn1_size_in_bytes - pointer to the data to be filled with the asn1 size

RETURN VALUE
  E_SUCCESS - if the data was parsed properly to get the asn1 buffer size
  E_INVALID_ARG - if a pointer argument is NULL
  E_DATA_INVALID - if the length is greater than SECASN1_MAX_LEN or is more than
                   data_bound argument

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type secx509_get_asn1_cert_size
(
  const UINT8*                 data_ptr,
  const UINT8*                 data_bound,
  UINT32*                      asn1_size_in_bytes_ptr
);

/*===========================================================================

FUNCTION SECX509_SKIP_NUM_CERTS

DESCRIPTION
  Skips over number of certificates and sets the chain ptr to point to the
  next certificate

DEPENDENCIES
  None

PARAMETERS
  cert_chain_pptr         - points to ptr of x509 certificate chain
  cert_chain_len          - upper bound on length of chain (parse fails
                            if the length is crossed when skipping
                            certs)
  num_certs_to_skip       - number of certs to skip in the chain

RETURN VALUE
  E_SUCCESS on successful skipping of certs
  E_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type secx509_skip_num_certs(
  const UINT8**                 cert_chain_pptr,
  const UINT8*                  cert_chain_bound,
  UINT8                         num_certs_to_skip
);

#endif /* SECBOOT_X509_H */

