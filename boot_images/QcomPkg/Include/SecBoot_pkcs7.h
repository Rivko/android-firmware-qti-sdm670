#ifndef _SECBOOT_PKCS7_H
#define _SECBOOT_PKCS7_H

/** @file SecBoot_pkcs7.h

  This header file contains declarations and definitions for the 
  APPSBL interface to PKCS7 API's.

  Copyright (c)2013 Qualcomm Technologies, Inc. All Rights Reserved.
**/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/13   sm      Added get_content_by_oid to Public API
11/18/11   shl     Added get PE hash support and more api
09/11/11   ky      Initial version

============================================================================*/

#include "com_dtypes.h"


typedef enum
{
  E_PKCS7_FAILURE = 0,
  E_PKCS7_SUCCESS,
  E_PKCS7_NO_DATA,
  E_PKCS7_DATA_INVALID,
  E_PKCS7_BAD_DATA,
  E_PKCS7_BAD_STATE,
  E_PKCS7_INVALID_ARG,
  E_PKCS7_X509_BAD_DATA,
  E_PKCS7_TRY_AGAIN,
  E_PKCS7_NOT_SUPPORTED,
  E_PKCS7_RESERVED       = 0x7FFFFFFF
} secpkcs7_errno_enum_type;

typedef void* secpkcs7_handle_type;

secpkcs7_errno_enum_type secpkcs7_new_ctx
(
  secpkcs7_handle_type       *pkcs7_handle_ptr
);

/* Must call parse buffer before any secpkcs7_get APIs */
secpkcs7_errno_enum_type secpkcs7_parse_buffer
(
  secpkcs7_handle_type        pkcs7_handle,
  const uint8                 *pkcs7_data, 
  uint32                      pkcs7_data_len
);

secpkcs7_errno_enum_type secpkcs7_get_cert_by_index
( 
  secpkcs7_handle_type        pkcs7_handle,
  uint32                      index,
  void                        **cert_ctx,
  uint32                      *cert_len_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_si_hash_alg
(
  secpkcs7_handle_type      pkcs7_handle,
  uint16                    si_index,
  void                      *hash_alg_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_si_sig_alg
(
  secpkcs7_handle_type      pkcs7_handle,
  uint16                    si_index,
  void                      *sig_alg_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_si_sig
(
  secpkcs7_handle_type      pkcs7_handle,
  uint16                    si_index,
  const uint8               **sig_ptr,
  uint32                    *len_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_cert_chain_from_sig_index
(
  secpkcs7_handle_type        pkcs7_handle,
  uint32                      si_index,
  void                       *cert_list,
  uint8*                      cert_chain_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_pubkey_from_sig_index
(
  secpkcs7_handle_type        pkcs7_handle,
  uint32                      si_index,
  void                       *pubkey_ptr
);

secpkcs7_errno_enum_type secpkcs7_verify_si
(  
  secpkcs7_handle_type        pkcs7_handle,
  int32                       si_index,
  const uint8                *digest_ptr,
  int32                       digest_len,
  uint8                       *hash
);

secpkcs7_errno_enum_type secpkcs7_get_pe_hash
(
  secpkcs7_handle_type      pkcs7_handle,
  const uint8               **hash_ptr,
  uint32                    *len_ptr
);

secpkcs7_errno_enum_type secpkcs7_get_content_by_oid
(
  secpkcs7_handle_type        pkcs7_handle_ptr,
  const uint8                *oid_ptr,
  uint32                      oid_len,
  const uint8               **content_ptr,
  uint32                     *content_len_ptr
);

secpkcs7_errno_enum_type secpkcs7_delete_ctx
(
  secpkcs7_handle_type        pkcs7_handle_ptr
);

#endif
