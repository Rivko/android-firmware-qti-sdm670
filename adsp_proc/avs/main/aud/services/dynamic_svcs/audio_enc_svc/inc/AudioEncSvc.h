/*========================================================================*/
/**
@file AudioEncSvc.h

@brief Encoder service public header.

This file contains the structure definitions and function declarations
for creating an instance of encoder service
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_encoder" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/inc/AudioEncSvc.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/15/10   WJ      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef ELITEENCODERSVC_H
#define ELITEENCODERSVC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Elite.h"
#include "qurt_elite.h"
#include "AudioStreamMgr.h"
#include "EliteMsg.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"

/** @addtogroup audio_encoder
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

enum eEncoderMode
{
   ENC_SVC_DATA_CMD_MODE = 0,    //the decoder interacts with the client using data commands
   ENC_SVC_PUSH_MODE,            //push mode: no data commands.
};

enum eEncoderIOFormatConvType
{
   ENC_SVC_IO_FORMAT_CONV_TYPE_AUTO = 0,           //decode raw to PCM OR encode PCM to raw.
   ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_PCM,       //depacketizer->decodes
   ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937,     //pass through
   ENC_SVC_IO_FORMAT_CONV_TYPE_61937_CONV_61937,   //depacketizer->conv->packetize
   ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW,       //depacketize
   ENC_SVC_IO_FORMAT_CONV_TYPE_RAW_TO_61937,       //packetize
   ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937,       //encode->packetize.
   ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_COP,         //encode->packetize into COP (compressed-over-pcm)
   ENC_SVC_IO_FORMAT_CONV_TYPE_COP_TO_PCM          //depacketize COP and decode.

};

struct AudioEncSvcPushModeParams_t
{
   union
   {
      asm_multi_channel_pcm_enc_cfg_v4_t pcm;
   } EncCfg;

   uint32_t shared_circ_buf_addr;
   uint32_t shared_circ_buf_size;
   asm_push_mode_read_position_buffer_t *pos_buf_ptr;
   uint32_t num_water_mark_levels;
   asm_push_mode_watermark_level_t *water_mark_levels_ptr;
};


/**
  @brief Data structure for initializing the Audion Encoder service on an open
  command.
*/
typedef struct
{
    eEncoderMode                        encMode;
      /**< Encoder mode: push mode or with data commands */

    uint32_t                            ulThreadPrio;
      /**< Priority of this Audio Encoder service thread. */

    uint32_t                            ulConfigId;
      /**< Media format ID of the Audio Encoder to be created. */

    bool_t                              bFrameMetaDataEnable;
      /**< Flag to enable adding frame metadata into the output buffer given
           by the client. */

    AudioStreamMgr_CallBackHandleType   *pCbData;
      /**< ASM callback handle for event generation/acknowledgment. */

	uint16_t                            bits_per_sample;
      /**< The number of bits per sample processed by ASM modules (input bits per sample). */
    uint32_t                            ulDataLogId;
      /**<data logging id: . upper 16 bits: apr port id. lower 16 bits: seq num. */

    uint32_t                            frames_per_buffer;
     /**<Number of frames per buffer in the compressed use case */

    bool_t                              no_apr_enc;
     /**<Flag to indicate if encoder should allocate output buffers by itself (instead of using APR client buffers) */

    AudioEncSvcPushModeParams_t         push_mode;

    uint16_t                            perf_mode;        /**< performance mode in which the decoder is opened
                                                              Supported values are ASM_LEGACY_STREAM_SESSION,
                                                              ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION, &
                                                              ASM_LOW_LATENCY_STREAM_SESSION. This mode is
                                                              currently only applicable to PCM playback */
    qurt_elite_queue_t                  *pEventQ;         /**< Messages will be queued here to raise asynchronous events
                                                               with the static service. */
    uint32_t                             eventToken;       /**< This is the token which will be used to identify this instance
                                                                     in any asynchronous event raised. */
    eEncoderIOFormatConvType             io_fmt_conv;

    bool_t                               is_rt_enc;     /** If encoder is in real time (AFE), then special buffering is needed */

    uint32_t out_sampling_rate;
    /** sampling rate at output.
     * useful only for COP packetization
     */
    uint16_t out_bits_per_sample;
    /**
     * bits per sample at output. useful only for COP packetization
     */
    uint16_t out_num_channels;
    /** zero means no change is expected, input = output. useful only for COP packetization where output can have less num channels compared to input.*/
    uint16_t out_channel_type[8];
    /** zero means no change is expected, input = output. useful only for COP packetization*/
    uint32_t     cop_packetizer_module_id;

    volatile uint32_t                   *delay_ptr; /** shared delay var. updated only by Enc*/

} AudioEncSvcInitParams_t;

/**
  ID for an internal flush readbufs command for. Encoder service
  returns all client buffers.
*/
#define ELITEMSG_CUSTOM_FLUSH_READBUFS      0x00170002

/* *
  Secondary opcode suspend for Encoder service
  */
#define ELITEMSG_CUSTOM_ENC_SUSPEND         0x00170003
/** @brief Encoder flush readbufs command structure. No payload,
   same as Elite custom message header.
*/

/**
  Secondary opcode to have the encoder prepare for disconnection
  Used by LLNP Tx sessions to warn pending disconnection from
  the upstream service.
*/
#define ELITEMSG_CUSTOM_ENC_PREPARE_DISCONNECT 0x00170004

typedef elite_msg_custom_header_t EliteMsg_CustomFlushReadbufsType;

/**
  Secondary opcode to set up encoder output media fmt.
*/
#define ELITEMSG_CUSTOM_ENC_SET_OUT_MEDIA_FMT 0x00170005

/* ELITEMSG_CUSTOM_ENC_SET_OUT_MEDIA_FMT message payload.
*/
typedef struct elite_msg_custom_enc_set_out_media_fmt_t
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unSecOpCode;
    /**< Secondary opcode that indicates the format for the rest of
         the payload.

         The secondary opcode for #ELITE_CUSTOM_MSG is 32 bits and
         is globally unique. A service can define a globally unique secondary
         operation  code as  follows:

         - The most significant 16 bits are the most significant 16 bits of
           the service ID that defines this secondary opcode.
         - The least significant 16 bits are defined by the service.

         @note1hang If the most significant 16 bits of the secondary operation
                    code are all zeros, this message is reserved for a common
                    custom message. */

    uint32_t sampling_rate;
    /** zero means no change is expected, input = output*/
    uint16_t bits_per_sample;
    /** zero means no change is expected, input = output*/
    uint16_t num_channels;
    /** zero means no change is expected, input = output*/
    uint16_t channel_type[8];
    /** zero means no change is expected, input = output*/

} elite_msg_custom_enc_set_out_media_fmt_t;


/* Opcode to perform set param through IMC */
#define AVS_ENC_SVC_IMC_SET_PARAM 0x00170006

/* IMC message structure */
/* Decoder to encoder payload header structure */
typedef struct dec_to_enc_communication_payload_header_t
{
   uint32_t opcode;
   /**< Parameter ID of the payload.*/

   uint32_t size;
   /**< Size (in bytes) for the payload specified in param_id. */

}dec_to_enc_communication_payload_header_t;

/* Set param payload structure*/
typedef struct dec_to_enc_communication_set_param_payload_t
{
   uint32_t purpose;
   /* purpose of the message*/

   uint32_t param_id;
   /**< Parameter ID of the payload.*/

   uint32_t param_size;
   /**< Size (in bytes) for the payload specified in param_id. */

#if 0
   uint8_t payload[param_size];
#endif

}dec_to_enc_communication_set_param_payload_t;

/**
  Creates an instance of the Elite Audio Encoder service.

  @param[in]  inputParam   Pointer to AudioEncSvcInitParams_t, expressed as
                           a uint32_t parameter.
  @param[out] handle       Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult AudioEncSvc_Create (uint32_t inputParam, void **handle);

/** @} */ /* end_addtogroup audio_encoder */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDCMNUTIL_H

