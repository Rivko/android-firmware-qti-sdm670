#ifndef VOICE_LOGGING_UTILS_H
#define VOICE_LOGGING_UTILS_H
/*========================================================================

                                 E l i t e
                 V o c p r o c  L o g g i n g  D e f i n i t i o n s

*/
/** @file VoiceLoggingUtils.h
  @brief  Contains common vocproc definitions
*/
/****************************************************************************
Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/
/*====================================================================== */

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_common/inc/VoiceLoggingUtils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


========================================================================== */

/* =======================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "qurt_elite.h"
#include "VoiceCmnUtils.h"

/* Log data format */

#define VOICE_LOG_DATA_FORMAT_PCM_MONO                   0x0001
#define VOICE_LOG_DATA_FORMAT_PCM_STEREO                 0x0002
#define VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV       0x0003
#define VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV        0x0004    // quad mic
#define VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV        0x0005    // three mic
#define VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV        0x0006
#define VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV        0x0007


// the following are the GUID based tap point IDs

#define VOICE_LOG_TAP_POINT_VPTX_ECNEAR_IN               0x00010F60
#define VOICE_LOG_TAP_POINT_VPTX_ECFAR_IN                0x00010F61
#define VOICE_LOG_TAP_POINT_VPTX_OUT                     0x00010F62
#define VOICE_LOG_TAP_POINT_VPRX_OUT                     0x00010F63
#define VOICE_LOG_TAP_POINT_VENC_IN                      0x00010F64
#define VOICE_LOG_TAP_POINT_VDEC_OUT                     0x00010F65
#define VOICE_LOG_TAP_POINT_VPTX_ECINTR_OUT              0x00010F66             
#define VOICE_LOG_TAP_POINT_VPTX_RTM                     0x00010F67

// the following are the legacy tap point IDs (for PCM tap points, priority is given to the new log codes, if enabled)

#define VOICE_LOG_CHAN_PCM_RX_ECINPUT                    0x0200
#define VOICE_LOG_CHAN_PCM_TX_ECINPUTNEAR                0x0201
#define VOICE_LOG_CHAN_PCM_TX_ECOUTPUT                   0x0202
#define VOICE_LOG_CHAN_PCM_TX_ECINPUTFAR                 0x0203
#define VOICE_LOG_CHAN_PCM_RX_ECOUTPUT                   0x0204
#define VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS                 0x0205
#define VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS_SEG             0x0206     // segmented AF coeff logging for multimic
#define VOICE_LOG_CHAN_PCM_TX_ECINTR_OUTPUT              0x0207

#define VOICE_LOG_CHAN_PCM_RXINOUT_STREAM                0x0300
#define VOICE_LOG_CHAN_PCM_TXINOUT_STREAM                0x0301
#define VOICE_LOG_CHAN_PKT_RX_STREAM                     0x0302
#define VOICE_LOG_CHAN_PKT_TX_STREAM                     0x0303

#define VOICE_LOG_CHAN_VSM_SET_PARAM                     0x0400
#define VOICE_LOG_CHAN_VPM_SET_PARAM                     0x0401
#define VOICE_LOG_CHAN_VSM_GET_PARAM                     0x0402
#define VOICE_LOG_CHAN_VPM_GET_PARAM                     0x0403

#define VOICE_LOG_CHAN_VSM_SET_PARAM_V2                  0x0404
#define VOICE_LOG_CHAN_VPM_SET_PARAM_V2                  0x0405
#define VOICE_LOG_CHAN_VSM_GET_PARAM_V2                  0x0406
#define VOICE_LOG_CHAN_VPM_GET_PARAM_V2                  0x0407 

#define VOICE_LOG_CHAN_VSM_RX_CTM_CHAR                   0x0500
#define VOICE_LOG_CHAN_VSM_TX_CTM_CHAR                   0x0501

#define VOICE_LOG_CHAN_VSM_RX_OOBTTY_CHAR                0x0502
#define VOICE_LOG_CHAN_VSM_TX_OOBTTY_CHAR                0x0503


ADSPResult voice_log_buffer( int8_t *channelBuf[], uint32_t log_id, uint32_t session_id, uint64_t timestamp, uint16_t data_format, uint32_t sampling_rate, uint32_t buf_size, void *custom_data);

ADSPResult voice_log_get_buffer(void** log_ptr, uint32_t log_id, uint32_t buf_size);

ADSPResult voice_log_free_buffer(void* log_ptr, uint32_t log_id);

ADSPResult voice_log_bitstream_buffer(void *log_ptr, uint32_t log_id, uint32_t session_id, uint64_t timestamp, uint32_t buf_size);

void voice_log_cal_data(uint32_t cal_handle, uint32_t log_id, uint32_t session_id);
#endif /* VOICE_LOGGING_UTILS_H */

