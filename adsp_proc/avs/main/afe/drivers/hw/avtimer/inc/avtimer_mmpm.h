/*========================================================================
  ELite

  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  9/2/10     rb      Created file.

  ========================================================================== */
/**
@file avtimer_mmpm.h

@brief avtimer functions and constants to interface with MMPM.
\note works only when MMPM_INTEGRATION is set to one.
*/

#ifndef AVTIMER_MMPM_H_
#define AVTIMER_MMPM_H_

#include "typedef_drivers.h"
#include "hwd_mmpm.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define AVTIMER_MMPM_VOTE_AVTIMER_CLK                      0x1
#define AVTIMER_MMPM_VOTE_AVSYNC_BT_CLK                    0x2
#define AVTIMER_MMPM_VOTE_AVSYNC_FM_CLK                    0x4

/**
 * Register with MMPM to get a handle for avtimer/avsync hw clks.
 * @param[out] ADSP result
 */
ADSPResult avtimer_mmpm_register(void);

/**
 * Deregister with MMPM
 * @param[out] ADSP result
 */
ADSPResult avtimer_mmpm_deregister(void);

/**
 * avtimer/avsync MMPM custom voting
 * @param[in] vote_type -> vote type
 * @param[out] ADSP result
 */
ADSPResult avtimer_mmpm_voting(uint32 vote_type);

/**
 * avtimer/avsync MMPM custom devoting
 * @param[in] vote_type -> vote type
 * @param[out] ADSP result
 */
ADSPResult avtimer_mmpm_devoting(uint32 vote_type);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* AVTIMER_MMPM_H_ */
