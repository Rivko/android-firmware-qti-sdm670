/*========================================================================*/
/**
@file IEC61937.h

@brief IEC61937 header file.

This file contains IEC 61937 related utilities.
 */

/*========================================================================
Copyright (c) 2013-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/IEC61937/common/lib/inc/IEC61937.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
4/9/2013    rbhatnk      Created file.
========================================================================== */


#ifndef _IEC61937_H_
#define _IEC61937_H_

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
   /** @addtogroup iec61937
@{ */

   /* -----------------------------------------------------------------------
    ** Global definitions/forward declarations
    ** ----------------------------------------------------------------------- */
#define IEC61937_BYTES_PER_IEC60958_FRAME  4
   /** samples from 2 channels together. */
#define IEC61937_NUM_SAMPLES_PER_IEC60958_FRAME  2
#define IEC61937_BYTES_PER_IEC_60958_SAMPLE  (IEC61937_BYTES_PER_IEC60958_FRAME/IEC61937_NUM_SAMPLES_PER_IEC60958_FRAME)

   static const uint16_t IEC61937_SYNC_WORD1 = 0xF872;
   static const uint16_t IEC61937_SYNC_WORD2 = 0x4E1F;
   static const uint32_t IEC61937_COMBINED_SYNC_WORD = 0x4E1FF872;
   static const uint16_t IEC61937_PAUSE_DATA_TYPE = 3;
   static const uint16_t IEC61937_NUM_PREAMBLE_BYTES = 8;
   static const uint16_t IEC61937_NUM_PREAMBLE_FRAMES = 2;
   static const uint16_t IEC61937_NUM_NULL_SYNC_FRAMES = 2;
   static const uint16_t IEC61937_NUMBER_PREAMBLE_BYTES = 8;
   static const uint16_t IEC61937_SYNC_BYTES = 4;
   static const uint16_t IEC61937_PAUSE_TYPE = 0x3;
   static const uint16_t IEC61937_NULL_TYPE = 0x0;
   static const uint16_t IEC61937_BITSPERWRD = 16;
   static const uint16_t IEC61937_BITS_PER_BYTE = 8;
   static const uint16_t IEC61937_DATA_TYPE_MASK = 0x1F;
   static const uint16_t IEC61937_MAX_FRAME_SIZE = 61440;

   /** returns the pause repitition period for the corresponding format.
    */
   uint32_t IEC61937_get_pause_rep_period(uint32_t format);

   /** rounds up given number of IEC60958 samples to a multiple of pause repetition period of the
    * format.
    */
   uint32_t IEC61937_align_samples_to_pause_repetition_period(uint32_t samples, uint32_t format_id);

   /** rounds up given number of bytes to a multiple of pause repetition period of the
    * format.
    */
   uint32_t IEC61937_align_bytes_to_pause_repetition_period(uint32_t bytes, uint32_t format_id);
   /**
    * 60958 samples per channel to bytes (for all channels)
    */
   uint32_t IEC61937_60958_samples_to_bytes(uint32_t samples, uint32_t num_channels);

   /**
    * bytes for all channels to samples per channel.
    */
   uint32_t IEC61937_bytes_to_60958_samples(uint32_t bytes, uint32_t num_channels);

   /**
    * finds the first sync word location (in terms of byte index)
    */
   ADSPResult IEC61937_get_first_syncword_locn(uint8_t *data_buf_ptr,
         uint32_t buf_size,
         uint32_t *locn);
   /**
    * finds the first sync word location in terms of byte index.
    * The location will be after the specified limit.
    */
   ADSPResult IEC61937_get_syncword_locn_after_limit(uint8_t *data_buf_ptr,
         uint32_t buf_size,
         uint32_t limit,
         uint32_t *locn);
   /**
    * fills pause specified samples per channel of bursts if possible
    * (constraints: out buf size & integral multiple of pause repetition period)
    */
   void IEC61937_fill_pause_bursts(uint32_t format_id,
         uint8_t *data_buf_ptr,
         uint32_t *buf_size,
         uint32_t *num_pause_samples_per_ch,
         uint32_t num_channels);

   /**
    * fills pause burst for the entire buffer size provided. pause_burst_start_sample_index will
    * indicate the position to start in the pause burst. Also updates the same pointer with
    * number of samples filled of a pause burst in the current frame of buffer.
    */
   void IEC61937_fill_misaligned_pause_bursts(uint32_t format_id,
         uint8_t *data_buf_ptr,
         uint32_t buf_size,
         uint32_t *pause_burst_start_sample_index);
   /**
    * fills pause burst for the entire buffer size provided with indicated number of IEC 60958 samples.
    * pause_burst_start_sample_index will  indicate the position to start in the pause burst.
    * Also updates the same pointer with number of IEC 60958 frames filled of a pause burst
    * in the current frame of buffer.
    */
   void IEC61937_fill_pause_bursts_from_IEC60958_frames(uint32_t format_id,
         uint8_t *data_buf_ptr,
         uint32_t *num_pause_samples);

   /**
    * buffer size requirement depending on format id.
    */
   ADSPResult IEC61937_get_buffer_size(uint32_t fmt, uint32_t *buf_size);

   /*Find the next syncword and update the current location,
    * note it preserves the initial position value
    * Used for 16-bit pointer only
    */

   ADSPResult IEC61937_get_next_syncword_locn_and_update(uint16_t *inputbuf,
         const uint32_t bufsize,
         uint32_t *samples_read);
   /*
    * Parse the Pc value in the compressed header
    * to tell the media format and repetition period
    */
   ADSPResult IEC61937_parse_media_info(uint16_t Pc, uint32_t *samples_per_frame, uint32_t *media_format);

   /** @} */ /* end_addtogroup iec61937 */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _IEC61937_H_

