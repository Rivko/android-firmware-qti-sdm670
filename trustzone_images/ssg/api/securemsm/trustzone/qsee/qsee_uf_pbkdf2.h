#ifndef QSEE_UF_PBKDF2_H
#define QSEE_UF_PBKDF2_H

/**
@file qsee_uf_pbkdf2.h
@brief Provide Software PBKDF2 API 
*/

/*===========================================================================
   Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_uf_pbkdf2.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/05/17   shl      Initial version.

===========================================================================*/
#include <qsee_uf_sha.h>


/**
 * @brief This functions derives key based on password and salt
 *
 * @param hmacAlgo      [in] HMAC algorithm
 * @param password      [in] password pointer
 * @param passwordLen   [in] password length
 * @param salt          [in] salt pointer
 * @param saltLen       [in] salt length
 * @param iterations    [in] iterations 
 * @param derivedKey    [in/out] derivedKey pointer
 * @param derivedKeyLen [in] derivedKey length
 *
 * @return sw_crypto_errno_enum_type 
 *
 */ 
sw_crypto_errno_enum_type qsee_SW_pbkdf2
(
  SW_Auth_Alg_Type   hmacAlgo,
  const uint8_t     *password,
  size_t             passwordLen,
  const uint8_t     *salt,
  size_t             saltLen,
  size_t             iterations,
  uint8_t           *derivedKey,
  size_t             derivedKeyLen
);

#endif /* QSEE_UF_PBKDF2_H */
