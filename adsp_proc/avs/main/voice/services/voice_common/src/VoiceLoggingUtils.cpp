/*============================================================================

                                 E l i t e
                    V o c p r o c  L o g g i n g  U t i l s

*//** @file VocprocLoggingUtils.cpp
  This file contains utilities used by multiple vocproc modules.

@par EXTERNALIZED FUNCTIONS
  (none)

@par INITIALIZATION AND SEQUENCING REQUIREMENTS
  (none)

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*========================================================================*/

/*============================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_common/src/VoiceLoggingUtils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


============================================================================*/

/*============================================================================
 *                       INCLUDE FILES FOR MODULE
 *==========================================================================*/
#include "qurt_elite.h"
#include "VoiceLoggingUtils.h"
#include "EliteLoggingUtils_i.h"
#include "mvm_api_i.h" //for calibration utility for set param


#if defined(__qdsp6__) && !defined(SIM)
#include "log.h"
#include "log_codes.h"

#if !defined(LOG_ADSP_VCP_VOICEPROC_RTM_C)
#define LOG_ADSP_VCP_VOICEPROC_RTM_C ( 0x184B )           // local defination in case its not defined in logs_code.h
#endif
// This is the legacy log format used for 0x13B1/0x14EE logging

/* Log container type */
#define AUDIOLOG_CONTAINER_DATA_FORMAT       0x0003
#define AUDIOLOG_CHAN_MAX_DATA_CHANNELS 1

/* Basic container definition */
typedef PACK(struct)
{
  uint16_t                  size;
  uint16_t                  tag;
} audiolog_container;

/* User session descriptor */
typedef PACK(struct)
{
  audiolog_container        container_usid;
  uint32_t                  user_session_id;
} audiolog_user_session_descriptor;

/* Data format descriptor */
typedef PACK(struct)
{
  audiolog_container        container_dfmt;
  uint16_t                  data_format;
} audiolog_data_format_descriptor;

/* PCM 16-linear format */
typedef PACK(struct)
{
  audiolog_data_format_descriptor     dfmt;
  uint32_t                            sample_rate;
  uint16_t                            channel_cnt;
  uint16_t                            chan_interleave_tbl[AUDIOLOG_CHAN_MAX_DATA_CHANNELS];
} audiolog_pcm_16l_format;

typedef PACK(struct)
{
  audiolog_container                  container_lgh;
  uint32_t                            log_session_id;
  uint32_t                            log_seq_number;
  uint32_t                            segment_size;
  uint32_t                            fragment_offset;
  audiolog_user_session_descriptor    user_session_info;
  union
  {
     audiolog_pcm_16l_format           pcm_16l;
  };
} audiolog_header;

// internal definition (diag services has legacy naming)
#define LOG_AUDVOC_DATA_PATHS_PACKET_ID        0x13B1   // Elite device session PCM loggin

LOG_RECORD_DEFINE(LOG_AUDVOC_DATA_PATHS_PACKET_C)
  audiolog_header         log_header;
  uint8                   payload[1];
LOG_RECORD_END


// translate GUIDs to legacy format
static uint32_t voice_log_translate_to_legacy_id( uint32_t log_id)
{
   switch( log_id)
   {
      case VOICE_LOG_TAP_POINT_VPTX_ECNEAR_IN:
      {
         return VOICE_LOG_CHAN_PCM_TX_ECINPUTNEAR;
      }
      case VOICE_LOG_TAP_POINT_VPTX_ECFAR_IN:
      {
         return VOICE_LOG_CHAN_PCM_TX_ECINPUTFAR;
      }
      case VOICE_LOG_TAP_POINT_VPTX_OUT:
      {
         return VOICE_LOG_CHAN_PCM_TX_ECOUTPUT;
      }
      case VOICE_LOG_TAP_POINT_VPRX_OUT:
      {
         return VOICE_LOG_CHAN_PCM_RX_ECOUTPUT;
      }
      case VOICE_LOG_TAP_POINT_VPTX_ECINTR_OUT:
      {
         return VOICE_LOG_CHAN_PCM_TX_ECINTR_OUTPUT;
      }
      default:
      {
         return 0;
      }
   }
}

static ADSPResult voice_log_buffer_legacy( int8_t *channel_buf_ptr[],
                             uint32_t log_id,
                             uint32_t session_id,
                             uint64_t timestamp,
                             uint16_t data_format,
                             uint32_t sampling_rate,
                             uint32_t buf_size,
                             void *custom_data)
{

      ADSPResult result = ADSP_EOK;
      // use this type for 0x13B1 and 0x14EE logging
      LOG_AUDVOC_DATA_PATHS_PACKET_C_type* log_pkt_ptr = NULL;
      uint32_t log_pkt_size = 0;

      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Voice Log log_id(%lx) session_id(%lx) buf_size(%lu)",log_id,session_id,buf_size);

      switch( log_id)
      {
          case VOICE_LOG_CHAN_PCM_TX_ECINPUTNEAR:
          case VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS:
          case VOICE_LOG_CHAN_PCM_TX_ECINTR_OUTPUT:
             {
                if( NULL == channel_buf_ptr[0])
                {
                   return ADSP_EFAILED;
                }

                if( data_format == VOICE_LOG_DATA_FORMAT_PCM_MONO)
                {
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + buf_size;
                }
                else if( data_format == VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV)
                {
                   if (NULL == channel_buf_ptr[1])
                   {
                      return ADSP_EFAILED;
                   }
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + (buf_size << 1);
                }
                else if( data_format == VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV)
                {
                   if( NULL == channel_buf_ptr[1] ||
                       NULL == channel_buf_ptr[2] )
                   {
                      return ADSP_EFAILED;
                   }
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + (buf_size * 3);
                }
                else if( data_format == VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV)
                {
                   if( NULL == channel_buf_ptr[1] ||
                       NULL == channel_buf_ptr[2] ||
                       NULL == channel_buf_ptr[3] )
                   {
                      return ADSP_EFAILED;
                   }
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + (buf_size << 2);
                }
                else if( data_format == VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV)
                {
                   if( NULL == channel_buf_ptr[1] ||
                       NULL == channel_buf_ptr[2] ||
                       NULL == channel_buf_ptr[3] ||
                       NULL == channel_buf_ptr[4] )
                   {
                      return ADSP_EFAILED;
                   }
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + (buf_size * 5);
                }
                else if( data_format == VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV)
                {
                   if( NULL == channel_buf_ptr[1] ||
                       NULL == channel_buf_ptr[2] ||
                       NULL == channel_buf_ptr[3] ||
                       NULL == channel_buf_ptr[4] ||
                       NULL == channel_buf_ptr[5])
                   {
                      return ADSP_EFAILED;
                   }
                   log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + (buf_size * 6);
                }
                else
                {
                   return ADSP_EFAILED;
                }

                log_pkt_ptr = reinterpret_cast<LOG_AUDVOC_DATA_PATHS_PACKET_C_type*>( log_alloc(LOG_AUDVOC_DATA_PATHS_PACKET_ID, log_pkt_size));
                break;
             }
          case VOICE_LOG_CHAN_PCM_RX_ECINPUT:
          case VOICE_LOG_CHAN_PCM_TX_ECOUTPUT:
          case VOICE_LOG_CHAN_PCM_TX_ECINPUTFAR:
          case VOICE_LOG_CHAN_PCM_RX_ECOUTPUT:
          case VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS_SEG:
          {
             /* validate these are mono */
             if( data_format != VOICE_LOG_DATA_FORMAT_PCM_MONO)
             {
                 return ADSP_EFAILED;
             }
             if( NULL == channel_buf_ptr[0])
             {
                 return ADSP_EFAILED;
             }
             log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + buf_size;
             log_pkt_ptr    = reinterpret_cast<LOG_AUDVOC_DATA_PATHS_PACKET_C_type*>( log_alloc(LOG_AUDVOC_DATA_PATHS_PACKET_ID, log_pkt_size));
             break;
          }
          case VOICE_LOG_CHAN_PKT_TX_STREAM:
          case VOICE_LOG_CHAN_PKT_RX_STREAM:
          {
             /* validate these are mono */
             if( data_format != VOICE_LOG_DATA_FORMAT_PCM_MONO)
             {
                 return ADSP_EFAILED;
             }
             if( NULL == channel_buf_ptr[0])
             {
                 return ADSP_EFAILED;
             }
             log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + sizeof(uint32_t) + buf_size ;       //extra uint32 for packing media type
             log_pkt_ptr    = reinterpret_cast<LOG_AUDVOC_DATA_PATHS_PACKET_C_type*>( log_alloc(LOG_AUDIO_VOCODER_STREAM_DATA_PATH_C, log_pkt_size));
             break;
          }
          case VOICE_LOG_CHAN_VSM_SET_PARAM:
          case VOICE_LOG_CHAN_VSM_SET_PARAM_V2:
          case VOICE_LOG_CHAN_VPM_SET_PARAM:
          case VOICE_LOG_CHAN_VPM_SET_PARAM_V2:
          case VOICE_LOG_CHAN_VSM_GET_PARAM:
          case VOICE_LOG_CHAN_VSM_GET_PARAM_V2:
          case VOICE_LOG_CHAN_VPM_GET_PARAM:
          case VOICE_LOG_CHAN_VPM_GET_PARAM_V2:
          {
             /* validate these are mono */
             if( data_format != VOICE_LOG_DATA_FORMAT_PCM_MONO)
             {
                 return ADSP_EFAILED;
             }
             if( NULL == channel_buf_ptr[0])
             {
                 return ADSP_EFAILED;
             }
             log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + buf_size;
             log_pkt_ptr    = reinterpret_cast<LOG_AUDVOC_DATA_PATHS_PACKET_C_type*>( log_alloc(LOG_AUDIO_VOCODER_STREAM_DATA_PATH_C, log_pkt_size));
             break;
          }
          case VOICE_LOG_CHAN_VSM_RX_CTM_CHAR:
          case VOICE_LOG_CHAN_VSM_TX_CTM_CHAR:
          case VOICE_LOG_CHAN_VSM_RX_OOBTTY_CHAR:
          case VOICE_LOG_CHAN_VSM_TX_OOBTTY_CHAR:
          {
             /* validate these are mono */
             if( data_format != VOICE_LOG_DATA_FORMAT_PCM_MONO)
             {
                 return ADSP_EFAILED;
             }
             if( NULL == channel_buf_ptr[0])
             {
                 return ADSP_EFAILED;
             }
             log_pkt_size = (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) + buf_size;
             log_pkt_ptr    = reinterpret_cast<LOG_AUDVOC_DATA_PATHS_PACKET_C_type*>( log_alloc(LOG_AUDIO_VOCODER_STREAM_DATA_PATH_C, log_pkt_size));
             break;
          }
          default:
          {
              return ADSP_EFAILED;
              break;
          }
      }

      if (log_pkt_ptr != NULL)
      {

         log_pkt_ptr->log_header.container_lgh.tag = AUDIOLOG_CONTAINER_LOG_HEADER;
         log_pkt_ptr->log_header.container_lgh.size = sizeof(audiolog_header);
         log_pkt_ptr->log_header.pcm_16l.sample_rate = sampling_rate;
         log_pkt_ptr->log_header.pcm_16l.channel_cnt = AUDIOLOG_CHAN_MAX_DATA_CHANNELS;
         log_pkt_ptr->log_header.pcm_16l.chan_interleave_tbl[0] = static_cast<uint16_t> (log_id);
         log_pkt_ptr->log_header.pcm_16l.dfmt.data_format = data_format;
         log_pkt_ptr->log_header.pcm_16l.dfmt.container_dfmt.size = sizeof(audiolog_pcm_16l_format);
         log_pkt_ptr->log_header.pcm_16l.dfmt.container_dfmt.tag = AUDIOLOG_CONTAINER_DATA_FORMAT;
         log_pkt_ptr->log_header.user_session_info.user_session_id = 0;
         log_pkt_ptr->log_header.user_session_info.container_usid.size = sizeof(audiolog_user_session_descriptor);
         log_pkt_ptr->log_header.user_session_info.container_usid.tag = AUDIOLOG_CONTAINER_USER_SESSION;
         log_pkt_ptr->log_header.log_session_id = static_cast<uint32_t>(session_id);
         log_pkt_ptr->log_header.log_seq_number = static_cast<uint32_t>(timestamp);   // legacy log overloads timestamp in log_seq_number
         log_pkt_ptr->log_header.fragment_offset = 0;

         if ( VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS == log_id)
         {
            if(NULL != custom_data)
            {
            int16_t* offset = (int16_t*)custom_data;
            log_pkt_ptr->log_header.fragment_offset = *offset;
            }
         }

         switch( data_format)
         {
             case VOICE_LOG_DATA_FORMAT_PCM_MONO:
             {
                 /* mono */
                if( ( VOICE_LOG_CHAN_PKT_TX_STREAM == log_id ||
                    VOICE_LOG_CHAN_PKT_RX_STREAM == log_id ) &&
                    (NULL != custom_data)
                   )
                {
                   // insert media type for parsing purposes
                   log_pkt_ptr->log_header.segment_size = buf_size + sizeof(uint32_t);
                   memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), custom_data, sizeof(uint32_t));
                   memsmove(log_pkt_ptr->payload + sizeof(uint32_t), log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - sizeof(uint32_t), channel_buf_ptr[0], buf_size);
                }
                else
                {
                   log_pkt_ptr->log_header.segment_size = buf_size;
                   memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                }
                break;
             }
             case VOICE_LOG_DATA_FORMAT_PCM_STEREO:
             {
                /* assume interleaved samples in logBufL */
                log_pkt_ptr->log_header.segment_size = buf_size;
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                break;
             }
             case VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV:
             {
                /* stereo non intlv...buf_size is size of each channel */
                log_pkt_ptr->log_header.segment_size = (buf_size << 1);
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - buf_size, channel_buf_ptr[1], buf_size);
                break;
             }
             case VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV:
             {
                /* 4 channel non intlv...buf_size is size of each channel */
                log_pkt_ptr->log_header.segment_size = (buf_size * 3);
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - buf_size, channel_buf_ptr[1], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*2, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 2*buf_size, channel_buf_ptr[2], buf_size);
                break;
             }
             case VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV:
             {
                /* 4 channel non intlv...buf_size is size of each channel */
                log_pkt_ptr->log_header.segment_size = (buf_size << 2);
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - buf_size, channel_buf_ptr[1], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*2, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 2*buf_size, channel_buf_ptr[2], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*3, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 3*buf_size, channel_buf_ptr[3], buf_size);
                break;
             }
            case VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV:
             {
                /* 6 channel non intlv...buf_size is size of each channel */
                log_pkt_ptr->log_header.segment_size = (buf_size * 5);
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - buf_size, channel_buf_ptr[1], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*2, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 2*buf_size, channel_buf_ptr[2], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*3, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 3*buf_size, channel_buf_ptr[3], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*4, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 4*buf_size, channel_buf_ptr[4], buf_size);
                break;
             }
             case VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV:
             {
                /* 6 channel non intlv...buf_size is size of each channel */
                log_pkt_ptr->log_header.segment_size = (buf_size * 6);
                memsmove(log_pkt_ptr->payload, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1), channel_buf_ptr[0], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - buf_size, channel_buf_ptr[1], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*2, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 2*buf_size, channel_buf_ptr[2], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*3, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 3*buf_size, channel_buf_ptr[3], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*4, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 4*buf_size, channel_buf_ptr[4], buf_size);
                memsmove(((int8_t *)log_pkt_ptr->payload) + buf_size*5, log_pkt_size - (sizeof(LOG_AUDVOC_DATA_PATHS_PACKET_C_type) - 1) - 5*buf_size, channel_buf_ptr[5], buf_size);
                break;
             }
         }
         log_commit(log_pkt_ptr);
      }
      else
      {
          result = ADSP_EFAILED;
      }

      return result;

}

// log using the new GUID based scheme, with AUDIOLOG_CONTAINER_PCM_DATA_FORMAT log format
static ADSPResult voice_log_buffer_pcm_ext( int8_t *channel_buf_ptr[],
                                            uint32_t log_id,
                                            uint32_t session_id,
                                            uint64_t timestamp,
                                            uint16_t data_format,
                                            uint32_t sampling_rate,
                                            uint32_t buf_size)
{

   pcm_data_info         format;
   log_pkt_pcm_data      *log;
   uint16                diag_code;
   uint16                channel_index;

   format.sampling_rate   = sampling_rate;
   format.num_channels    = 1;
   format.bits_per_sample = 16;
   format.interleaved     = FALSE;
   format.channel_mapping = NULL;

   uint32 total_buf_size = 0;

   if( NULL == channel_buf_ptr )
   {
      return ADSP_EBADPARAM;
   }

   switch( data_format)
   {
      case VOICE_LOG_DATA_FORMAT_PCM_MONO:
      {
          if( NULL == channel_buf_ptr[0] )
          {
             return ADSP_EBADPARAM;
          }
          total_buf_size = buf_size;
          break;
      }
      case VOICE_LOG_DATA_FORMAT_PCM_STEREO:
      {
          if( NULL == channel_buf_ptr[0])
          {
             return ADSP_EBADPARAM;
          }

          format.num_channels   = 2;
          format.interleaved    = TRUE;
          total_buf_size = buf_size;                  /* assume interleaved samples in logBufL */

          break;
      }
      case VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV:
      {
          if( NULL == channel_buf_ptr[0] ||
              NULL == channel_buf_ptr[1])
          {
             return ADSP_EBADPARAM;
          }

          format.num_channels   = 2;
          total_buf_size        = buf_size << 1;

          break;
      }
      case VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV:
      {
          if( NULL == channel_buf_ptr[0] ||
              NULL == channel_buf_ptr[1] ||
              NULL == channel_buf_ptr[2] )
          {
             return ADSP_EBADPARAM;
          }
          format.num_channels = 3;
          total_buf_size = buf_size * 3;
          break;
      }

      case VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV:
      {
          if( NULL == channel_buf_ptr[0] ||
              NULL == channel_buf_ptr[1] ||
              NULL == channel_buf_ptr[2] ||
              NULL == channel_buf_ptr[3] )
          {
             return ADSP_EBADPARAM;
          }
          format.num_channels = 4;
          total_buf_size = buf_size << 2;
          break;
      }
      case VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV:
      {
          if( NULL == channel_buf_ptr[0] ||
              NULL == channel_buf_ptr[1] ||
              NULL == channel_buf_ptr[2] ||
              NULL == channel_buf_ptr[3] ||
              NULL == channel_buf_ptr[4])
          {
             return ADSP_EBADPARAM;
          }
          format.num_channels = 5;
          total_buf_size = buf_size * 5;
          break;
      }
      case VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV:
      {
          if( NULL == channel_buf_ptr[0] ||
              NULL == channel_buf_ptr[1] ||
              NULL == channel_buf_ptr[2] ||
              NULL == channel_buf_ptr[3] ||
              NULL == channel_buf_ptr[4] ||
              NULL == channel_buf_ptr[5])
          {
             return ADSP_EBADPARAM;
          }
          format.num_channels = 6;
          total_buf_size = buf_size * 6;
          break;
      }

      default:
      {
          return ADSP_EBADPARAM;
      }
   }

   switch( log_id)
   {
      case VOICE_LOG_TAP_POINT_VPTX_ECNEAR_IN:
      case VOICE_LOG_TAP_POINT_VPTX_ECFAR_IN:
      case VOICE_LOG_TAP_POINT_VPTX_OUT:
      case VOICE_LOG_TAP_POINT_VPRX_OUT:
      case VOICE_LOG_TAP_POINT_VPTX_ECINTR_OUT:
      {
          diag_code = LOG_ADSP_VCP_VOICEPROC_RX_TX_C;
          break;
      }
      case VOICE_LOG_TAP_POINT_VENC_IN:
      case VOICE_LOG_TAP_POINT_VDEC_OUT:
      {
          diag_code = LOG_ADSP_VCP_VOICESTRM_RX_TX_C;
          break;
      }
      default:
      {
          return ADSP_EBADPARAM;
          break;
      }
   }

   log =  reinterpret_cast<log_pkt_pcm_data*>( log_alloc(diag_code, sizeof(log_pkt_pcm_data) - sizeof(log->payload) + total_buf_size));

   if( NULL == log)
   {
      return ADSP_ENOMEMORY;
   }

   log->log_header_pcm.cmn_struct.tag             = AUDIOLOG_CONTAINER_LOG_HEADER;
   log->log_header_pcm.cmn_struct.size            = sizeof(audiolog_header_pcm_data);
   log->log_header_pcm.cmn_struct.log_session_id  = session_id;
   log->log_header_pcm.cmn_struct.log_seg_number  = 0;
   log->log_header_pcm.cmn_struct.segment_size    = total_buf_size;
   log->log_header_pcm.cmn_struct.fragment_offset = 0;

   log->log_header_pcm.cmn_struct.user_session_info.tag             = AUDIOLOG_CONTAINER_USER_SESSION;
   log->log_header_pcm.cmn_struct.user_session_info.size            = sizeof(audiolog_user_session);
   log->log_header_pcm.cmn_struct.user_session_info.user_session_id = 0;
   log->log_header_pcm.cmn_struct.user_session_info.time_stamp      = timestamp;

   log->log_header_pcm.pcm_data_fmt.tag           = AUDIOLOG_CONTAINER_PCM_DATA_FORMAT;
   log->log_header_pcm.pcm_data_fmt.size          = sizeof(audiolog_pcm_data_format);
   log->log_header_pcm.pcm_data_fmt.log_tap_id    = log_id;
   log->log_header_pcm.pcm_data_fmt.sampling_rate = format.sampling_rate;
   log->log_header_pcm.pcm_data_fmt.num_channels  = format.num_channels;
   log->log_header_pcm.pcm_data_fmt.pcm_width     = format.bits_per_sample;
   log->log_header_pcm.pcm_data_fmt.interleaved   = format.interleaved;

   // set default mapping
   for( channel_index = 0; channel_index < format.num_channels; channel_index++)
   {
      log->log_header_pcm.pcm_data_fmt.channel_mapping[channel_index] = channel_index + 1;
   }

   for( channel_index = format.num_channels; channel_index < ELITE_MAX_NUM_CH; channel_index++)
   {
      log->log_header_pcm.pcm_data_fmt.channel_mapping[channel_index] = 0;
   }

   switch( data_format)
   {
       case VOICE_LOG_DATA_FORMAT_PCM_MONO:
       {
          memsmove(log->payload, total_buf_size, channel_buf_ptr[0], buf_size);
          break;
       }
       case VOICE_LOG_DATA_FORMAT_PCM_STEREO:
       {
          /* assume interleaved samples in logBufL */
          memsmove(log->payload, total_buf_size, channel_buf_ptr[0], buf_size);
          break;
       }
       case VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV:
       {
          /* stereo non intlv...buf_size is size of each channel */
          memsmove(log->payload, total_buf_size, channel_buf_ptr[0], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size, total_buf_size - buf_size, channel_buf_ptr[1], buf_size);
          break;
       }
       case VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV:
       {
          memsmove(log->payload,  total_buf_size, channel_buf_ptr[0], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size,  total_buf_size - buf_size, channel_buf_ptr[1], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*2,  total_buf_size - 2*buf_size, channel_buf_ptr[2], buf_size);
          break;
       }
       case VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV:
       {
          /* 4 channel non intlv...buf_size is size of each channel */
          memsmove(log->payload,  total_buf_size, channel_buf_ptr[0], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size,  total_buf_size - buf_size, channel_buf_ptr[1], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*2,  total_buf_size - 2*buf_size, channel_buf_ptr[2], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*3,  total_buf_size - 3*buf_size, channel_buf_ptr[3], buf_size);
          break;
       }
       case VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV:
       {
          /* 4 channel non intlv...buf_size is size of each channel */
          memsmove(log->payload,  total_buf_size, channel_buf_ptr[0], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size,  total_buf_size - buf_size, channel_buf_ptr[1], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*2,  total_buf_size - 2*buf_size, channel_buf_ptr[2], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*3,  total_buf_size - 3*buf_size, channel_buf_ptr[3], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*4,  total_buf_size - 4*buf_size, channel_buf_ptr[4], buf_size);
          break;
       }

       case VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV:
       {
          /* 4 channel non intlv...buf_size is size of each channel */
          memsmove(log->payload,  total_buf_size, channel_buf_ptr[0], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size,  total_buf_size - buf_size, channel_buf_ptr[1], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*2,  total_buf_size - 2*buf_size, channel_buf_ptr[2], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*3,  total_buf_size - 3*buf_size, channel_buf_ptr[3], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*4,  total_buf_size - 4*buf_size, channel_buf_ptr[4], buf_size);
          memsmove(((int8_t *)log->payload) + buf_size*5,  total_buf_size - 5*buf_size, channel_buf_ptr[5], buf_size);
          break;
       }

   }

   log_commit( log);

   return( ADSP_EOK);
}

ADSPResult voice_log_buffer( int8_t *channel_buf_ptr[],
                             uint32_t log_id,
                             uint32_t session_id,
                             uint64_t timestamp,
                             uint16_t data_format,
                             uint32_t sampling_rate,
                             uint32_t buf_size,
                             void *custom_data)
{


   switch( log_id)
   {

      case VOICE_LOG_TAP_POINT_VPTX_ECNEAR_IN:
      case VOICE_LOG_TAP_POINT_VPTX_ECFAR_IN:
      case VOICE_LOG_TAP_POINT_VPTX_OUT:
      case VOICE_LOG_TAP_POINT_VPRX_OUT:
      case VOICE_LOG_TAP_POINT_VENC_IN:
      case VOICE_LOG_TAP_POINT_VDEC_OUT:
      case VOICE_LOG_TAP_POINT_VPTX_ECINTR_OUT:
      {
          // prioritize new logging codes/format over legacy.
          if( ADSP_EOK == voice_log_buffer_pcm_ext( channel_buf_ptr, log_id, session_id, timestamp, data_format, sampling_rate, buf_size))
          {
             return ADSP_EOK;
          }
          else
          {
             uint32_t log_id_translated = voice_log_translate_to_legacy_id( log_id);
             return voice_log_buffer_legacy( channel_buf_ptr, log_id_translated, session_id, timestamp, data_format, sampling_rate, buf_size, custom_data);
          }
          break;
      }

      // for remaining legacy codes, directly use legacy log codes and format
      case VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS:
      case VOICE_LOG_CHAN_PCM_TX_ECAFCOEFFS_SEG:
      case VOICE_LOG_CHAN_PKT_RX_STREAM:
      case VOICE_LOG_CHAN_PKT_TX_STREAM:
      case VOICE_LOG_CHAN_VSM_SET_PARAM:
      case VOICE_LOG_CHAN_VPM_SET_PARAM:
      case VOICE_LOG_CHAN_VSM_GET_PARAM:
      case VOICE_LOG_CHAN_VPM_GET_PARAM:
      case VOICE_LOG_CHAN_VSM_SET_PARAM_V2:
      case VOICE_LOG_CHAN_VPM_SET_PARAM_V2:
      case VOICE_LOG_CHAN_VSM_GET_PARAM_V2:
      case VOICE_LOG_CHAN_VPM_GET_PARAM_V2:
      case VOICE_LOG_CHAN_VSM_RX_CTM_CHAR:
      case VOICE_LOG_CHAN_VSM_TX_CTM_CHAR:
      case VOICE_LOG_CHAN_VSM_RX_OOBTTY_CHAR:
      case VOICE_LOG_CHAN_VSM_TX_OOBTTY_CHAR:
      {
         return voice_log_buffer_legacy( channel_buf_ptr, log_id, session_id, timestamp, data_format, sampling_rate, buf_size, custom_data);
         break;
      }

      default:
      {
         return ADSP_EBADPARAM;
         break;
      }
   }
}

/* Bitstream logging utilities for RTM */
ADSPResult voice_log_get_buffer(void** log_ptr, uint32_t log_id, uint32_t buf_size)
{
   if(NULL == log_ptr)
   {
      return ADSP_EBADPARAM;
   }
   uint16 diag_code;
   *log_ptr = NULL;
   switch(log_id)
   {
      case VOICE_LOG_TAP_POINT_VPTX_RTM:
      {
         log_pkt_bitstream_data *log = NULL;
         diag_code = LOG_ADSP_VCP_VOICEPROC_RTM_C;
         log = reinterpret_cast<log_pkt_bitstream_data*> (log_alloc(diag_code, sizeof(log_pkt_bitstream_data) - sizeof(log->payload) + buf_size));
         // Adjust the pointer to be past the header and be at the payload
         if(NULL == log)
         {
            return ADSP_EOK;
         }
         *log_ptr = (void*)(&log->payload);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Invalid log id %lu passed to voice_log_get_buffer", log_id);
         return ADSP_EBADPARAM;
      }
   }
   return ADSP_EOK;
}

ADSPResult voice_log_bitstream_buffer(void* log_ptr, uint32_t log_id, uint32_t session_id, uint64_t timestamp, uint32_t buf_size)
{
   if(NULL == log_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! NULL pointer passed to voice_log_bitstream_buffer");
      return ADSP_EBADPARAM;
   }

   // Get pointer of packet, the function takes in pointer to payload
   log_pkt_bitstream_data *log = reinterpret_cast<log_pkt_bitstream_data*> ( (uint8_t*)log_ptr - sizeof(log_pkt_bitstream_data) + sizeof(log->payload) );
   // header size not part of segment_size
   //buf_size += sizeof(log_pkt_bitstream_data) - sizeof(log->payload);
   log->log_header_bs.cmn_struct.tag             = AUDIOLOG_CONTAINER_LOG_HEADER;
   log->log_header_bs.cmn_struct.size            = sizeof(audiolog_header_bitstream_data);
   log->log_header_bs.cmn_struct.log_session_id  = session_id;
   log->log_header_bs.cmn_struct.log_seg_number  = 0;
   log->log_header_bs.cmn_struct.segment_size    = buf_size;
   log->log_header_bs.cmn_struct.fragment_offset = 0;

   log->log_header_bs.cmn_struct.user_session_info.tag             = AUDIOLOG_CONTAINER_USER_SESSION;
   log->log_header_bs.cmn_struct.user_session_info.size            = sizeof(audiolog_user_session);
   log->log_header_bs.cmn_struct.user_session_info.user_session_id = 0;
   log->log_header_bs.cmn_struct.user_session_info.time_stamp      = timestamp;

   log->log_header_bs.bs_data_fmt.tag = AUDIOLOG_CONTAINER_BS_DATA_FORMAT;
   log->log_header_bs.bs_data_fmt.size = sizeof(audiolog_bitstream_data_format);
   log->log_header_bs.bs_data_fmt.log_tap_id = log_id;
   log->log_header_bs.bs_data_fmt.media_fmt_id = 0;

   log_commit( log);

   return ADSP_EOK;
}

// Frees an allocated buffer without committing it
ADSPResult voice_log_free_buffer(void* log_ptr, uint32_t /*log_id*/)
{
   // log id currently unused, assuming bitstream header
   if(NULL == log_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! NULL pointer passed to voice_log_free_buffer");
      return ADSP_EBADPARAM;
   }

   log_pkt_bitstream_data *log = reinterpret_cast<log_pkt_bitstream_data*> ( (uint8_t*)log_ptr - sizeof(log_pkt_bitstream_data) + sizeof(log->payload));
   log_free(log);

   return ADSP_EOK;
}

enum
{
   VOICE_LOG_ITER_SIZE = 0,
   VOICE_LOG_ITER_LOG
};

typedef struct voice_cal_log_info_t
{
   // indicates what the pointer represents
   // VOICE_LOG_ITER_SIZE indicates it points to a variable holding the total size
   // VOICE_LOG_ITER_LOG indicates it points to a pointer to write the cal data to
   uint32_t log_iter;
   void* ptr;
   int32_t buf_size;
} voice_cal_log_info_t;


static void voice_log_cb_func(void* cal_params_payload_ptr, void* cb_data)
{
   voice_cal_log_info_t* log_info_ptr = (voice_cal_log_info_t*)cb_data;
   uint32_t param_size,header_size;
   void *param_data;

   uint32_t minor_cal_version = *(uint32_t *)cal_params_payload_ptr;

   if(VOICE_LOG_ITER_SIZE == log_info_ptr->log_iter)
   {
      //increment the curr_size by the payload size for current mid/pid pair
      if(0 == minor_cal_version)
      {
         cvd_cal_param_t *cal_params_ptr = (cvd_cal_param_t *)cal_params_payload_ptr;
         param_size = cal_params_ptr->param_data_size;
         header_size = sizeof(voice_param_data_t);
      }
      else
      {
         cvd_cal_param_v1_t *cal_params_ptr = (cvd_cal_param_v1_t *)cal_params_payload_ptr;
         param_size = cal_params_ptr->param_data_size;
         header_size = sizeof(voice_param_data_v2_t);
      }

      uint32_t* curr_size_ptr = (uint32_t*)(log_info_ptr->ptr);
      *curr_size_ptr = (*curr_size_ptr) + header_size + param_size;
   }
   else
   {
      //update the buffer with the payload of current mid/pid pair
      uint8_t* payload_ptr = (uint8_t*)(log_info_ptr->ptr);
      if(0 == minor_cal_version)
      {
         cvd_cal_param_t *cal_params_ptr = (cvd_cal_param_t *)cal_params_payload_ptr;

         voice_param_data_t* param_data_ptr = (voice_param_data_t*)(log_info_ptr->ptr);
         param_data_ptr->module_id = cal_params_ptr->module_id;
         param_data_ptr->param_id = cal_params_ptr->param_id;
         param_data_ptr->param_size = cal_params_ptr->param_data_size;
         param_data_ptr->reserved = 0;
         param_data = cal_params_ptr->param_data;
         param_size = cal_params_ptr->param_data_size;

         header_size = sizeof(voice_param_data_t);
      }
      else
      {
         cvd_cal_param_v1_t *cal_params_ptr = (cvd_cal_param_v1_t *)cal_params_payload_ptr;

         voice_param_data_v2_t* param_data_ptr = (voice_param_data_v2_t*)(log_info_ptr->ptr);
         param_data_ptr->module_id = cal_params_ptr->module_id;
         param_data_ptr->param_id = cal_params_ptr->param_id;
         param_data_ptr->param_size = cal_params_ptr->param_data_size;
         param_data_ptr->reserved = 0;
         param_data_ptr->instance_id = cal_params_ptr->instance_id;
         param_data = cal_params_ptr->param_data;
         param_size = cal_params_ptr->param_data_size;

         header_size = sizeof(voice_param_data_v2_t);
      }

      payload_ptr += header_size;

      memscpy(payload_ptr, log_info_ptr->buf_size, param_data, param_size);

      log_info_ptr->buf_size -= (header_size + param_size);
      log_info_ptr->ptr = (void*)((uint8_t*)(log_info_ptr->ptr) + (header_size + param_size));
   }
   return;
}

// Iterates over the cal handle corresponding to the cvd utility and logs all the cal data
void voice_log_cal_data(uint32_t cal_handle, uint32_t log_id, uint32_t session_id)
{
   uint32_t log_size = 0;
   voice_cal_log_info_t log_info;
   void* temp_buffer;

   // first set of iterations is to ensure that we have the right size
   log_info.log_iter = VOICE_LOG_ITER_SIZE;
   log_info.ptr = (void*)(&log_size);
   log_info.buf_size = sizeof(uint32_t); //not currently used for this value of log_iter

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = cal_handle;
   mvm_payload.cb_fn = voice_log_cb_func;
   mvm_payload.client_data = &log_info;

   (void)mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   if(0 == log_size)
   {
      // nothing to log
      return;
   }

   temp_buffer = qurt_elite_memory_malloc(log_size, QURT_ELITE_HEAP_DEFAULT);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_log_cal_data got total payload size of %lu bytes, ptr 0x%lx", log_size, temp_buffer);
   if(((void*)0) ==  temp_buffer)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate temp buffer while logging voice cal data !");
      return;
   }


   // second set of iterations for actual logging
   log_info.log_iter = VOICE_LOG_ITER_LOG;
   log_info.ptr = temp_buffer;
   log_info.buf_size = log_size;

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   mvm_payload.query_handle = cal_handle;
   mvm_payload.cb_fn = voice_log_cb_func;
   mvm_payload.client_data = &log_info;

   (void)mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   // sanity check to see if buf_size is zero, it is expected to be
   if(0 != log_info.buf_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! voice_log_cal_data did not fill buffer correctly, remaining size %ld", log_info.buf_size);
      return;
   }

   int8_t* log_buffer_ptr[4] = {(int8_t*)temp_buffer, NULL, NULL, NULL};

   // now call logging utility to log this buffer
   voice_log_buffer( log_buffer_ptr,
         log_id,
         session_id,
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_MONO,
         1,   /* dummy value */
         log_size,
         NULL);

   // free malloc'ed buffer
   qurt_elite_memory_free(temp_buffer);
   return;
}

#endif  //defined(__qdsp6__) && !defined(SIM)

#if defined(SIM)
// Test utilities for cal on sim
enum
{
   VOICE_LOG_ITER_SIZE = 0,
   VOICE_LOG_ITER_LOG
};

typedef struct voice_cal_log_info_t
{
   // indicates what the pointer represents
   // VOICE_LOG_ITER_SIZE indicates it points to a variable holding the total size
   // VOICE_LOG_ITER_LOG indicates it points to a pointer to write the cal data to
   uint32_t log_iter;
   void* ptr;
   int32_t buf_size;
} voice_cal_log_info_t;

static void voice_log_cb_func(void* cal_params_payload_ptr, void* cb_data)
{
   voice_cal_log_info_t* log_info_ptr = (voice_cal_log_info_t*)cb_data;
   uint32_t param_size,header_size;
   void *param_data;

   uint32_t minor_cal_version = *(uint32_t *)cal_params_payload_ptr;

   if(VOICE_LOG_ITER_SIZE == log_info_ptr->log_iter)
   {
      //increment the curr_size by the payload size for current mid/pid pair
      if(0 == minor_cal_version)
      {
         cvd_cal_param_t *cal_params_ptr = (cvd_cal_param_t *)cal_params_payload_ptr;
         param_size = cal_params_ptr->param_data_size;
         header_size = sizeof(voice_param_data_t);
      }
      else
      {
         cvd_cal_param_v1_t *cal_params_ptr = (cvd_cal_param_v1_t *)cal_params_payload_ptr;
         param_size = cal_params_ptr->param_data_size;
         header_size = sizeof(voice_param_data_v2_t);
      }

      uint32_t* curr_size_ptr = (uint32_t*)(log_info_ptr->ptr);
      *curr_size_ptr = (*curr_size_ptr) + header_size + param_size;
   }
   else
   {
      //update the buffer with the payload of current mid/pid pair
      uint8_t* payload_ptr = (uint8_t*)(log_info_ptr->ptr);
      if(0 == minor_cal_version)
      {
         cvd_cal_param_t *cal_params_ptr = (cvd_cal_param_t *)cal_params_payload_ptr;

         voice_param_data_t* param_data_ptr = (voice_param_data_t*)(log_info_ptr->ptr);
         param_data_ptr->module_id = cal_params_ptr->module_id;
         param_data_ptr->param_id = cal_params_ptr->param_id;
         param_data_ptr->param_size = cal_params_ptr->param_data_size;
         param_data_ptr->reserved = 0;
         param_data = cal_params_ptr->param_data;
         param_size = cal_params_ptr->param_data_size;

         header_size = sizeof(voice_param_data_t);
      }
      else
      {
         cvd_cal_param_v1_t *cal_params_ptr = (cvd_cal_param_v1_t *)cal_params_payload_ptr;

         voice_param_data_v2_t* param_data_ptr = (voice_param_data_v2_t*)(log_info_ptr->ptr);
         param_data_ptr->module_id = cal_params_ptr->module_id;
         param_data_ptr->param_id = cal_params_ptr->param_id;
         param_data_ptr->param_size = cal_params_ptr->param_data_size;
         param_data_ptr->reserved = 0;
         param_data_ptr->instance_id = cal_params_ptr->instance_id;
         param_data = cal_params_ptr->param_data;
         param_size = cal_params_ptr->param_data_size;

         header_size = sizeof(voice_param_data_v2_t);
      }

      payload_ptr += header_size;

      memscpy(payload_ptr, log_info_ptr->buf_size, param_data, param_size);

      log_info_ptr->buf_size -= (header_size + param_size);
      log_info_ptr->ptr = (void*)((uint8_t*)(log_info_ptr->ptr) + (header_size + param_size));
   }
   return;
}

// Iterates over the cal handle corresponding to the cvd utility and logs all the cal data
void voice_log_cal_data_sim(uint32_t cal_handle, uint32_t log_id, uint32_t session_id)
{
   uint32_t log_size = 0;
   voice_cal_log_info_t log_info;
   void* temp_buffer;

   // first set of iterations is to ensure that we have the right size
   log_info.log_iter = VOICE_LOG_ITER_SIZE;
   log_info.ptr = (void*)(&log_size);
   log_info.buf_size = sizeof(uint32_t); //not currently used for this value of log_iter

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = cal_handle;
   mvm_payload.cb_fn = voice_log_cb_func;
   mvm_payload.client_data = &log_info;

   (void)mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   temp_buffer = qurt_elite_memory_malloc(log_size, QURT_ELITE_HEAP_DEFAULT);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_log_cal_data got total payload size of %lu bytes, ptr 0x%lx", log_size, temp_buffer);

   // second set of iterations for actual logging
   log_info.log_iter = VOICE_LOG_ITER_LOG;
   log_info.ptr = temp_buffer;
   log_info.buf_size = log_size;

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   mvm_payload.query_handle = cal_handle;
   mvm_payload.cb_fn = voice_log_cb_func;
   mvm_payload.client_data = &log_info;

   (void)mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   // sanity check to see if buf_size is zero, it is expected to be
   if(0 != log_info.buf_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! voice_log_cal_data did not fill buffer correctly, remaining size %ld", log_info.buf_size);
      return;
   }

   FILE * fp = fopen("vpm_cal.bin", "a");
   fwrite(temp_buffer, 1, log_size, fp);
   fclose(fp);
   // free malloc'ed buffer
   qurt_elite_memory_free(temp_buffer);
   return;
}
#endif

