/**
@file AudStreamMgr_CmdParser.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_CmdParser.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/28/2013 RB     created file
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_GetSetBits.h"

#include "adsp_media_fmt.h"
#include "adsp_error_codes.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"

/**
 * does only partial parsing.
 */
ADSPResult ASM_ParseOpenReadPush(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_PUSH_MODE_READ:
      {
         asm_stream_cmd_open_push_mode_read_t *pOpenCmd =
               (asm_stream_cmd_open_push_mode_read_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->stream_perf_mode = (bool_t)asm_get_stream_perf_mode_flag_in_open_pull_mode_write_cmd(pOpenCmd->mode_flags);
         pOpen->sink_fmt_id = pOpenCmd->enc_cfg_id;
         pOpen->src_endpoint_type = pOpenCmd->src_endpoint_type;
         pOpen->sink_endpoint_type = ASM_SINK_CLIENT;
         pOpen->bits_per_sample = pOpenCmd->topo_bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->preproc_topo_id;
      }
      break;
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open push mode read cmd: Opcode 0x%x, perf_mode=%u, enc_cfg=0x%lx,  bps=%u, preproc=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->stream_perf_mode, pOpen->sink_fmt_id, pOpen->bits_per_sample, pOpen->pp_topo_id);

   if (((ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 != pOpen->sink_fmt_id) && (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 != pOpen->sink_fmt_id)) ||
         ((pOpen->bits_per_sample != 16) && (pOpen->bits_per_sample != 24) && (pOpen->bits_per_sample != 32)) ||
         ((pOpen->stream_perf_mode != ASM_LOW_LATENCY_STREAM_SESSION)&&
               (pOpen->stream_perf_mode != ASM_LEGACY_STREAM_SESSION) &&
               (pOpen->stream_perf_mode != ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION)))
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }
   return result;
}

/**
 * does only partial parsing.
 */
ADSPResult ASM_ParseOpenWritePull(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE:
      {
         asm_stream_cmd_open_pull_mode_write_t *pOpenCmd =
               (asm_stream_cmd_open_pull_mode_write_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->stream_perf_mode = (bool_t)asm_get_stream_perf_mode_flag_in_open_pull_mode_write_cmd(pOpenCmd->mode_flags);
         pOpen->src_fmt_id = pOpenCmd->dec_fmt_id;
         pOpen->sink_fmt_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;
         pOpen->src_endpoint_type = ASM_SOURCE_CLIENT;
         pOpen->sink_endpoint_type = pOpenCmd->sink_endpoint_type;
         pOpen->bits_per_sample = pOpenCmd->topo_bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->postproc_topo_id;
      }
      break;
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open pull mode write cmd: Opcode 0x%x, perf_mode=%u, dec_fmt=0x%lx,  bps=%u, postproc=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->stream_perf_mode, pOpen->src_fmt_id, pOpen->bits_per_sample, pOpen->pp_topo_id);

   if (((ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 != pOpen->src_fmt_id) && (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 != pOpen->src_fmt_id)) ||
         ((pOpen->bits_per_sample != 16) && (pOpen->bits_per_sample != 24) && (pOpen->bits_per_sample != 32)) ||
         ((ASM_LEGACY_STREAM_SESSION!=pOpen->stream_perf_mode) &&
          (pOpen->stream_perf_mode != ASM_LOW_LATENCY_STREAM_SESSION) &&
          (pOpen->stream_perf_mode != ASM_ULTRA_LOW_LATENCY_STREAM_SESSION) &&
          (pOpen->stream_perf_mode != ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION) &&
          (pOpen->stream_perf_mode != ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION)))
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }
   return result;
}

ADSPResult ASM_ParseOpenReadCompr(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_READ_COMPRESSED:
      {
         asm_stream_cmd_open_read_compressed_t *pOpenCmd =
               (asm_stream_cmd_open_read_compressed_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->meta_info_flag = (bool_t) asm_get_meta_info_flag(pOpenCmd->mode_flags);
         pOpen->enc_frames_per_buffer = pOpenCmd->frames_per_buf;

         pOpen->enc_io_fmt_conv = ASM_IO_FORMAT_CONV_61937_TO_RAW;
         pOpen->bits_per_sample = 16;

         break;
      }
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open read compr cmd: Opcode 0x%x, metainfo=%u, frames per buf=%lu",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->meta_info_flag, pOpen->enc_frames_per_buffer);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_ParseOpenWriteCompr(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   bool_t is_IEC_60958_stream = false;
   bool_t is_input_IEC61937 = false;
   bool_t is_packetizer = false;

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED:
      {
         asm_stream_cmd_open_write_compressed_t *pOpenCmd =
               (asm_stream_cmd_open_write_compressed_t *)elite_apr_if_get_payload_ptr(pPkt);

         is_input_IEC61937 = (bool_t) asm_get_IEC_61937_stream_flag(pOpenCmd->flags);
         is_IEC_60958_stream = (bool_t) asm_get_IEC_60958_stream_flag(pOpenCmd->flags);
         pOpen->decoder_conv_mode = asm_get_decoder_converter_mode(pOpenCmd->flags);
         pOpen->src_fmt_id = pOpenCmd->fmt_id;  //can be zero for compressed passthrough
         pOpen->sink_fmt_id = pOpenCmd->fmt_id;
         pOpen->src_endpoint_type = ASM_SOURCE_CLIENT;
         pOpen->sink_endpoint_type = ASM_SINK_MATRIX_STRTR;

         pOpen->bits_per_sample = 16;
         is_packetizer = true;

         break;
      }
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open write compr cmd: Opcode 0x%x, 61937 flag=%u, 60958 flag=%u, fmt=0x%lx, dec conv mode=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, is_input_IEC61937,is_IEC_60958_stream, pOpen->src_fmt_id, pOpen->decoder_conv_mode);
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open write compr cmd: src ep=%u,sink ep=%u",
         int_session_id, stream_id, pOpen->src_endpoint_type,pOpen->sink_endpoint_type);

   if(is_IEC_60958_stream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: IEC 60958 packetized streams not accepted in pass-through mode",
            int_session_id, stream_id );
      return ADSP_EUNSUPPORTED;
   }

   if (is_packetizer)
   {
      if (is_input_IEC61937)
      {
         pOpen->dec_io_fmt_conv = ASM_IO_FORMAT_CONV_61937_TO_61937;

      }
      else
      {
         pOpen->dec_io_fmt_conv = ASM_IO_FORMAT_CONV_RAW_TO_61937;
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_ParseOpenLoopback(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_LOOPBACK_V2:
      {
         asm_stream_cmd_open_loopback_v2_t *pOpenCmd =
               (asm_stream_cmd_open_loopback_v2_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->src_endpoint_type = pOpenCmd->src_endpoint_type;
         pOpen->sink_endpoint_type = pOpenCmd->sink_endpoint_type;
         pOpen->bits_per_sample = pOpenCmd->bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->postproc_topo_id;
         pOpen->src_fmt_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;
         pOpen->sink_fmt_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;

         pOpen->stream_type = ASM_STREAM_TYPE_PRIMARY;

         pOpen->no_apr_enc_dec = FALSE;
         break;
      }
   case ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK:
      {
         asm_stream_cmd_open_transcode_loopback_t *pOpenCmd =
               (asm_stream_cmd_open_transcode_loopback_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->stream_perf_mode = asm_get_stream_perf_mode_flag_in_open_transcode_loopback_cmd(pOpenCmd->mode_flags);
         pOpen->decoder_conv_mode = asm_get_decoder_converter_mode(pOpenCmd->mode_flags);
         pOpen->src_endpoint_type = pOpenCmd->src_endpoint_type;
         pOpen->sink_endpoint_type = pOpenCmd->sink_endpoint_type;
         pOpen->bits_per_sample = pOpenCmd->bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->audproc_topo_id;
         pOpen->src_fmt_id = pOpenCmd->src_format_id;
         pOpen->sink_fmt_id = pOpenCmd->sink_format_id;

         pOpen->stream_type = ASM_STREAM_TYPE_PRIMARY;

         pOpen->no_apr_enc_dec = TRUE;
         pOpen->is_rt_dec = TRUE;
         break;
      }
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open loopback cmd: Opcode 0x%x, perf_mode=%u, src fmt=0x%lx,sink fmt=0x%lx, stream type %u",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->stream_perf_mode, pOpen->src_fmt_id, pOpen->sink_fmt_id, pOpen->stream_type);
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open loopback cmd: bps=%u,src=%u, sink=%u, preproc=0x%lx, dec conv mode=0x%lx",
         int_session_id, stream_id, pOpen->bits_per_sample,pOpen->src_endpoint_type, pOpen->sink_endpoint_type, pOpen->pp_topo_id,pOpen->decoder_conv_mode);

   if (ASM_SOURCE_MATRIX_STRTR != pOpen->src_endpoint_type)
   {
      result |= ADSP_EBADPARAM;
   }

   if ( (ASM_SINK_MATRIX_STRTR != pOpen->sink_endpoint_type))
   {
      result |= ADSP_EBADPARAM;
   }

   if (pOpen->stream_type != ASM_STREAM_TYPE_PRIMARY)
   {
      result |= ADSP_EFAILED;
   }

   if ( (16 != pOpen->bits_per_sample) && (24 != pOpen->bits_per_sample) && (pOpen->bits_per_sample != 32))
   {
      result |= ADSP_EFAILED;
   }

   if (pOpen->stream_perf_mode > ASM_LOW_LATENCY_STREAM_SESSION)
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_ParseOpenRead(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_READ_V3:
      {
         asm_stream_cmd_open_read_v3_t *pOpenCmd =
               (asm_stream_cmd_open_read_v3_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->meta_info_flag = (bool_t) asm_get_meta_info_flag(pOpenCmd->mode_flags);
         pOpen->timestamp_type = asm_get_timestamp_type_flag(pOpenCmd->mode_flags);
         pOpen->stream_perf_mode = asm_get_stream_perf_mode_flag_in_open_read_cmd(pOpenCmd->mode_flags);
         pOpen->sink_fmt_id = pOpenCmd->enc_cfg_id;
         pOpen->src_endpoint_type = pOpenCmd->src_endpoint_type;
         pOpen->sink_endpoint_type = ASM_SINK_CLIENT;
         pOpen->bits_per_sample = pOpenCmd->bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->preproc_topo_id;

         break;
      }
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open read cmd: Opcode 0x%x, metainfo=%u, sr_cm=%u, perf_mode=%u, enc_cfg=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->meta_info_flag, pOpen->sr_cm_event_needed, pOpen->stream_perf_mode, pOpen->sink_fmt_id);
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open read cmd: bps=%u,preproc=0x%lx",
         int_session_id, stream_id, pOpen->bits_per_sample,pOpen->pp_topo_id);

   if ( (16 != pOpen->bits_per_sample) && (24 != pOpen->bits_per_sample) && (pOpen->bits_per_sample != 32))
   {
      result |= ADSP_EFAILED;
   }

   if ((pOpen->stream_perf_mode > ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION)
         || (pOpen->stream_perf_mode == ASM_ULTRA_LOW_LATENCY_STREAM_SESSION)
         || (pOpen->stream_perf_mode == ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION))
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_ParseOpenReadWrite(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_READWRITE_V2:
      {
         asm_stream_cmd_open_readwrite_v2_t *pOpenCmd =
               (asm_stream_cmd_open_readwrite_v2_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->meta_info_flag = (bool_t) asm_get_meta_info_flag(pOpenCmd->mode_flags);
         pOpen->src_fmt_id = pOpenCmd->dec_fmt_id;
         pOpen->sink_fmt_id = pOpenCmd->enc_cfg_id;
         pOpen->sr_cm_event_needed = asm_get_sr_cm_notify_flag(pOpenCmd->mode_flags);
         pOpen->src_endpoint_type = ASM_SOURCE_CLIENT;
         pOpen->sink_endpoint_type = ASM_SINK_CLIENT;
         pOpen->bits_per_sample = pOpenCmd->bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->postproc_topo_id;

         break;
      }
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open readwrite cmd: Opcode 0x%x, metainfo=%u, sr_cm=%u, dec_fmt=0x%lx, enc_cfg=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->meta_info_flag, pOpen->sr_cm_event_needed, pOpen->src_fmt_id, pOpen->sink_fmt_id);
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open readwrite cmd: bps=%u,postproc=0x%lx",
         int_session_id, stream_id, pOpen->bits_per_sample,pOpen->pp_topo_id);

   if ( (16 != pOpen->bits_per_sample) && (24 != pOpen->bits_per_sample) && (pOpen->bits_per_sample != 32))
   {
      result |= ADSP_EFAILED;
   }

   if (pOpen->stream_perf_mode > ASM_LOW_LATENCY_STREAM_SESSION)
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_ParseOpenWrite(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   memset(pOpen, 0, sizeof(ASM_OpenStreamParams_t));

   switch(pPkt->opcode)
   {
   case ASM_STREAM_CMD_OPEN_WRITE_V3:
      {
         asm_stream_cmd_open_write_v3_t *pOpenCmd =
               (asm_stream_cmd_open_write_v3_t *)elite_apr_if_get_payload_ptr(pPkt);

         pOpen->gapless_mode =  asm_get_gapless_mode_flag(pOpenCmd->mode_flags);
         pOpen->stream_perf_mode = (uint16_t)asm_get_stream_perf_mode_flag_in_open_write_cmd(pOpenCmd->mode_flags);
         pOpen->src_fmt_id = pOpenCmd->dec_fmt_id;
         pOpen->sink_fmt_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;
         pOpen->sr_cm_event_needed = asm_get_sr_cm_notify_flag(pOpenCmd->mode_flags);
         pOpen->src_endpoint_type = ASM_SOURCE_CLIENT;
         pOpen->sink_endpoint_type = pOpenCmd->sink_endpoint_type;
         pOpen->bits_per_sample = pOpenCmd->bits_per_sample;
         pOpen->pp_topo_id = pOpenCmd->postproc_topo_id;
      }
      break;
   default:
      result = ADSP_EFAILED;
   }

   pOpen->default_pp_sample_rate = ASM_DEFAULT_PP_SAMPLE_RATE;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open write cmd: Opcode 0x%x, gapless=%u, perf_mode=%u, sr_cm=%u, dec_fmt=0x%lx",
         int_session_id, stream_id, (unsigned int)pPkt->opcode, pOpen->gapless_mode, pOpen->stream_perf_mode, pOpen->sr_cm_event_needed, pOpen->src_fmt_id);
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Parsing open write cmd: src=%u, sink=%u, bps=%u,postproc=0x%lx",
         int_session_id, stream_id, pOpen->src_endpoint_type, pOpen->sink_endpoint_type, pOpen->bits_per_sample,pOpen->pp_topo_id);

   if ( (ASM_SOURCE_CLIENT != pOpen->src_endpoint_type) )
   {
      result |= ADSP_EFAILED;
   }

   if ( (ASM_SINK_MATRIX_STRTR != pOpen->sink_endpoint_type))
   {
      result |= ADSP_EFAILED;
   }

   if ( (16 != pOpen->bits_per_sample) && (24 != pOpen->bits_per_sample) && (pOpen->bits_per_sample != 32))
   {
      result |= ADSP_EFAILED;
   }

   if (pOpen->stream_perf_mode > ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION)
   {
      result |= ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Parsing of open cmd failed",
            int_session_id, stream_id);
      result = ADSP_EFAILED;
   }
   return result;
}


