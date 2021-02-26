#ifndef ELITEDATA_UTIL_H
#define ELITEDATA_UTIL_H

/* ========================================================================*/
/**
 @file EliteData_Util.h
 Utility header for data processing across the elite framework
 */

/*======================================================================
   Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 08/19/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
 05/31/16   rr      Created file.
 ========================================================================= */

#include "shared_aud_cmn_lib.h"

/** @addtogroup elite_msg_datatypes
@{ */

/** Provides the translation between bit width and Q format to the caller. */
uint32_t elite_data_get_q_format_factor(uint32_t bit_width);

/** Provides the bytes per sample corresponding to the input bit width. */
uint32_t elite_data_get_bytes_per_sample(uint32_t bit_width);

/** @} */ /* end_addtogroup elite_msg_datatypes */

#endif /* #ifndef ELITEDATA_UTIL_H */
