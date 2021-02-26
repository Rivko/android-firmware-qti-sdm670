#ifndef QSEE_PRNG_H
#define QSEE_PRNG_H

/**
@file qsee_prng.h
@brief Provide prng API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_prng.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_MAX_PRNG    512  /* Maximum number of PRNG bytes read */

/**
 * @brief Release all resources with a given prng context.
 *
 * @param[in] out     The output data buffer
 * @param[in] out_len The output data length. The out_len
 *                    must be at most QSEE_MAX_PRNG bytes.
 *
 * @return number of bytes read
 *
 */
uint32_t qsee_prng_getdata(uint8_t *out, uint32_t out_len);

#endif /*QSEE_PRNG_H*/

