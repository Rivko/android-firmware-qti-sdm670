#ifndef QSEE_HMAC_H
#define QSEE_HMAC_H

/**
@file qsee_hmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
   ===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_hmac.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/09/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef void qsee_hmac_ctx;

/** Supported HMAC algorithms   */
typedef enum
  {
    QSEE_HMAC_SHA1          = 1,
    QSEE_HMAC_SHA256        = 2,
    QSEE_HMAC_INVALID = 0x7FFFFFFF,
  } QSEE_HMAC_ALGO_ET;

typedef enum
  {
    QSEE_HMAC_PARAM_KEY  = 1,
    QSEE_HMAC_PARAM_INVALID = 0x7FFFFFFF,
  } QSEE_HMAC_PARAM_ET;

/*---------------------------------------------------------------------------
 * Digest Sizes
 *--------------------------------------------------------------------------*/
#define QSEE_HMAC_DIGEST_SIZE_SHA1    20
#define QSEE_HMAC_DIGEST_SIZE_SHA256  32

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 *
 * @return 0 on success, negative on failure
 */
int qsee_hmac(QSEE_HMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint16_t key_len, uint8_t *msg_digest);

/**
 * @brief Initialize a hash MAC per FIPS pub 198 using the 
 *        specified hash algorithm.
 *
 * @param[out] ctx         - Pointer to hold context 
 * @param[in]  alg         - algorithm for Hash MAC
 *
 * @return 0 on success, negative on failure
 */
int qsee_hmac_init(QSEE_HMAC_ALGO_ET alg, qsee_hmac_ctx **ctx);

/**
 * @brief Updates hash MAC per FIPS pub 198 using the specified 
 *        hash algorithm.
 *
 * @param[in] ctx             - Context pointer
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 *
 * @return 0 on success, negative on failure
 */
int qsee_hmac_update(qsee_hmac_ctx *ctx, const uint8_t *msg, uint32_t msg_len);

/**
 * @brief Final operation for hash MAC per FIPS pub 198 using 
 *        the specified hash algorithm.
 *
 * @param[out] msg_digest        - Pointer to message digest 
 *                                 (memory provided by caller)
 * @param[in] msg_digest_length  - Length of message in bytes
 *
   * @return 0 on success, negative on failure
*/
int qsee_hmac_final(qsee_hmac_ctx *ctx, uint8_t *msg_digest, uint32_t msg_digest_length);

/**
 * @brief Release all resources with a given hmac context.
 *
 * @param hmac_ctx[in] hmac context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hmac_free_ctx(qsee_hmac_ctx *hmac_ctx);

/**
 * @brief Modify the parameters for a given hmac operation.
 *
 * @param hmac_ctx[in]   Hmac context
 * @param param_id[in]   The parameter to modify
 * @param param[in]      The parameter value to set.
 * @param param_len[in]  The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hmac_set_param(qsee_hmac_ctx *hmac_ctx,
                        QSEE_HMAC_PARAM_ET param_id,
                        const void *param,
                        uint32_t param_len);

#endif /*QSEE_HMAC_H*/
