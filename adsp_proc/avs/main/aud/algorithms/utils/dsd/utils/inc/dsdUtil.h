/*========================================================================*/
/**
@file dsdUtil.h

@brief DSD decoding/packetization utility header file.

This file contains DSD DOP related utilities.
 */

/*========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------
11/13/2015 yefeiw  Created file.
========================================================================== */


#ifndef _DSDUTIL_H_
#define _DSDUTIL_H_

#if defined CAPI_V2_STANDALONE
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"
#include "capi_v2_test.h"
#include "capi_v2_util.h"
#elif defined DEC_CAPI_V2_STANDALONE
#include "dec_capi_v2_test.h"
#include "dec_capi_v2_util.h"
#elif defined APPI_EXAMPLE_STANDALONE
#include "appi_test.h"
#include "appi_util.h"
#else
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


   /* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
   /** @addtogroup dsdutils
@{ */

   /* -----------------------------------------------------------------------
    ** Global definitions/forward declarations
    ** ----------------------------------------------------------------------- */
   /** samples from 2 channels together. */
#define DSD_NUMBER_PREAMBLE_BYTES          4
#define DSD_DSF_SYNC_WORD1                 0x4453
#define DSD_DSF_SYNC_WORD2                 0x4420
#define DSD_DSF_COMBINED_SYNC_WORD         0x20445344
#define DSD_DSF_COMBINED_DATA_WORD         0x61746164
#define DSD_DFF_SYNC_WORD1                 0x4652
#define DSD_DFF_SYNC_WORD2                 0x4D38
#define DSD_DFF_COMBINED_SYNC_WORD         0x384D5246
#define DSD_DFF_COMBINED_DATA_WORD         0x20445344
#define DSD_PAUSE_WORD_BYTE                0x69
#define DSD_BITSPERWRD                     16
#define DSD_DSF_CHANNEL_BLOCK_SIZE         4096
#define DSD_DFF_CHANNEL_BLOCK_SIZE         1
#define DSD_DOP_CHANNEL_BLOCK_SIZE         2
#define DSD_DOP_NUM_CHANNELS               2
#define DSD_DOP_MAX_BUFFER_SIZE           DSD_DSF_CHANNEL_BLOCK_SIZE * DSD_DOP_NUM_CHANNELS

   static const uint16_t DSD_SYNC_BYTES = 4;
   static const uint16_t DSD_BITS_PER_BYTE = 8;
   static const uint16_t DSD_MAX_FRAME_SIZE = 61440;
   static const uint32_t DSD_DOP_BYTES_PER_SAMPLE = 2;
   static const uint32_t DSD_DOP_BITS_PER_SAMPLE = 16;
   static const uint32_t DSD_DEFAULT_PAUSE_DURATION_US = 5000;




   void DSD_fill_pause_bursts(uint32_t sampling_rate,
         uint8_t *data_buf_ptr,
         uint32_t *buf_size,
         uint32_t pause_duration_us,
         uint32_t num_channels);


   /** @} */ /* end_addtogroup dsdutils */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _DSDUTIL_H_

