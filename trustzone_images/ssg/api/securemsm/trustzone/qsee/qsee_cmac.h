#ifndef QSEE_CMAC_H
#define QSEE_CMAC_H

/**
@file qsee_cmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_cmac.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/29/13   ejt      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define QSEE_CMAC_DIGEST_SIZE      16

#define QSEE_CMAC_SUCCESS          0
#define QSEE_CMAC_FAILURE          -1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Supported CMAC algorithms   */
typedef enum
{
   QSEE_CMAC_ALGO_AES_128          = 1,
   QSEE_CMAC_ALGO_AES_256          = 2
}  QSEE_CMAC_ALGO_ET;  

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Create a cipher MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to CMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] cmac_digest    - Pointer to cmac digest (memory provided by caller)
 * @param[in] cmac_len        - Length of CMAC in bytes
 *
 * @return 0 on success, negative on failure
*/
int qsee_cmac(QSEE_CMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint32_t key_len, uint8_t *cmac_digest,
              uint32_t cmac_len);


#endif /*QSEE_CMAC_H*/
