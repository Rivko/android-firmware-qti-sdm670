/*========================================================================

*//** @file voice_multi_ch_circ_buf.h

Copyright (c) 2010-2014,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file provides interfaces to create and control multi channel circular buffers for voice threads.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/inc/voice_multi_ch_circ_buf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/10           Created file.

========================================================================== */

#ifndef __VOICE_MULTI_CH_CIRCBUF__
#define __VOICE_MULTI_CH_CIRCBUF__


#define MAX_CIRCBUF_CHANNELS 6

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "VoiceCmnUtils.h"

typedef struct {
   circbuf_struct circ_buf[MAX_CIRCBUF_CHANNELS]; 
   int8_t *buf_ptr[MAX_CIRCBUF_CHANNELS];
   int32_t num_channels;
} voice_multi_ch_circ_buf_t;

circbuf_status_enum voice_multi_ch_circ_buf_init(voice_multi_ch_circ_buf_t *structPtr, 
      int32_t num_channels, int32_t max_samples, int32_t bitsPerChannel);

circbuf_status_enum voice_multi_ch_circ_buf_read(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *(*toPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples);

circbuf_status_enum voice_multi_ch_circ_buf_block_read(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *toPtr, int32_t numSamples);

circbuf_status_enum voice_multi_ch_circ_buf_write(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *(*fromPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples);

circbuf_status_enum voice_multi_ch_circ_buf_block_write(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *fromPtr, int32_t numSamples);

circbuf_status_enum voice_multi_ch_circ_buf_request(voice_multi_ch_circ_buf_t *structPtr,
       int32_t numSamples, int32_t *availableSamplesPtr);

circbuf_status_enum voice_multi_ch_circ_buf_reset(voice_multi_ch_circ_buf_t *structPtr);

circbuf_status_enum voice_multi_ch_circ_buf_prefill(voice_multi_ch_circ_buf_t *structPtr,
        int8_t *(*fromPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples);

#endif//#ifndef __VOICE_MULTI_CH_CIRCBUF__
