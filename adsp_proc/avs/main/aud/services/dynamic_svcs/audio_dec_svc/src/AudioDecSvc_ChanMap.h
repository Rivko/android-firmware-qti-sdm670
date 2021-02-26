/** @file AudioDecSvc_ChanMap.h
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2011-2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/** 
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_ChanMap.h#1 $


when       who     what, where, why 
--------   ---     -------------------------------------------------------
07/22/11   rkc     Created file.

========================================================================== 
*/ 

#ifndef AUDIODECSVC_CHAN_MAP_H
#define AUDIODECSVC_CHAN_MAP_H

#include "qurt_elite.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc.h"
#include "adsp_asm_api.h"

ADSPResult AudioDecSvc_GetOutChanMap(DecChannelMap_t *in_chan_map,
                                     dec_out_chan_map_t *out_chan_map);

ADSPResult AudioDecSvc_GetOutput(AudioDecSvc_t *pMe,
                                 AudioDecSvc_OutStream_t *pOutStrm,
                                 dec_CAPI_container_t *capi_cont,
                                 DecChannelMap_t *in_chan_map,
                                 void *out_buf, 
                                 uint32_t samples_per_ch, 
                                 uint32_t bytes_per_samp,
                                 uint32_t *out_buf_size);

ADSPResult AudioDecSvc_ProcessOutChanMapCmd(AudioDecSvc_t *me, AudioDecSvc_OutStream_t *pOutStrm,
      asm_dec_out_chan_map_param_t *cmd);

bool_t AudioDecSvc_IsChanMapChanged(uint32_t num_chan_old, uint8_t *chan_map_old,
                                    uint32_t num_chan_new, uint8_t *chan_map_new);

#endif //AUDIODECSVC_CHAN_MAP_H
