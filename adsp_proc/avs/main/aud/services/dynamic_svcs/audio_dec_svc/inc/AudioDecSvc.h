/*========================================================================*/
/**
@file AudioDecSvc.h

@brief Decoder service public header.

This file contains audio decoder service belonging to the Elite framework.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_decoder" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/inc/AudioDecSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/16/09    SJ      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */


#ifndef AUDIODECODERSVC_H
#define AUDIODECODERSVC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "AudioStreamMgr.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"

/** @addtogroup audio_decoder
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

typedef enum eDecoderMode
{
   DEC_SVC_DATA_CMD_MODE = 0,    //the decoder interacts with the client using data commands
   DEC_SVC_PULL_MODE,            //pull mode: no data commands.
   DEC_SVC_UNKNOWN_MODE
} eDecoderMode;

enum eDecoderIOFormatConvType
{
   DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO = 0,           //decode raw to PCM OR encode PCM to raw.
   DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_PCM,       //depacketizer->decodes
   DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937,     //pass through
   DEC_SVC_IO_FORMAT_CONV_TYPE_61937_CONV_61937,   //depacketizer->conv->packetize
   DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW,       //depacketize
   DEC_SVC_IO_FORMAT_CONV_TYPE_RAW_TO_61937,       //packetize
   DEC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937,       //encode->packetize.
   DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_MULTI,      //input to MIMO decoder is 61937, output is many
   DEC_SVC_IO_FORMAT_CONV_TYPE_COP_TO_PCM          //depacketize COP and decode.
};

struct AudioDecSvcPullModeParams_t
{
   union
   {
      asm_multi_channel_pcm_fmt_blk_v4_t pcm;
   } MediaFmt;

   uint32_t shared_circ_buf_addr;
   uint32_t shared_circ_buf_size;
   asm_pull_mode_write_position_buffer_t *pos_buf_ptr;
   uint32_t num_water_mark_levels;
   asm_pull_mode_watermark_level_t *water_mark_levels_ptr;
};

enum AudioDecSvcIOStreamType
{
      AUDIO_DEC_SVC_MIMO_INPUT_STREAM,
      AUDIO_DEC_SVC_MIMO_OUTPUT_STREAM,
      AUDIO_DEC_SVC_IN_OUT_STREAM //one stream id for output as well as input (SISO)
};

/**
 * to specify these of a MIMO decoder, input stream must be opened in ASM.
 */
struct AudioDecSvcInStreamInitParam_t
{
   eDecoderMode            ulMode;

   bool_t                  bEnableSrCmEvent;    /**< Flag for enabling sampling rate / channel mode
                                                       change notification event. */

   bool_t                  no_apr_dec;    /** indicates that this decoder doesn't have an APR client but an internal client to provide input data */


   AudioDecSvcPullModeParams_t pull_mode; /**< pull mode related params */


  eDecoderIOFormatConvType  io_fmt_conv; /** input format type: this is only about depacketize,decode,packetize */

  uint32_t     cop_depacketizer_module_id;
};

/**
 * to specify these of a MIMO decoder, output stream must be opened in ASM.
 */
struct AudioDecSvcOutStreamInitParam_t
{
   volatile uint32_t* downstream_buf_delay_ptr;   /** pointer to downstream buffer delay, us*/
};

/**
 *
 */
struct AudioDecSvcIOStreamInitParams_t
{
   uint8_t                 stream_id;  //needed to identify the stream (say for set-param)
   AudioDecSvcIOStreamType io_type;
   uint32_t                format_id; /**< Media format ID of the decoder to be created. Input format OR output format depending on io_type of this stream. */
   uint16_t                stream_type;   /**< depends on io_type */
   uint32_t                mimo_dec_fmt_id;  /** valid only for input and output streams */

   AudioStreamMgr_CallBackHandleType   *pCbData; /**< Audio Stream Manager callback
                                                   handle for event generation/ack. */

   volatile uint32_t *pDelay;   /** Delay pointer*/

   qurt_elite_queue_t                  *pEventQ;   // Messages will be queued here to raise asynchronous events with the static service.
   uint32_t        eventToken;       /**< This is the token which will be used to identify this instance
                                      in any asynchronous event raised. */

   uint16_t        bits_per_sample; /**< Bits per sample processed by ASM modules
                                                   (decoder and post-processor chain) */
   uint32_t        ulDataLogId;      /**< data logging id. upper 16 bits: apr port id. lower 16 bits: seq num */

   uint16_t        perf_mode;        /**< performance mode in which the decoder should be opened
                                          Supported values are ASM_LEGACY_STREAM_SESSION &
                                          ASM_LOW_LATENCY_STREAM_SESSION. This mode is
                                          currently only applicable to PCM playback */

   uint32_t        decoderConverterMode; /**< decoder converter mode */

   AudioDecSvcInStreamInitParam_t  in;    //used for io_type == input or in_out;
   AudioDecSvcOutStreamInitParam_t out;   //used for io_type == output, or in_out
};

/**
  @brief Data structure for initializing the Audio Decoder service on an open
  command.
*/
typedef struct AudioDecSvcInitParams_t
{
    uint32_t      ulThreadPrio;     /**< Priority of this Audio Decoder service
                                       thread. */

    qurt_elite_queue_t *pEventQ;   /**< Messages will be queued here to raise asynchronous events
                                       with the static service. */

    bool_t is_rt_dec; /** If decoder is in real-time (AFE), then special buffering is needed */

    AudioDecSvcIOStreamInitParams_t io_param;   /** parameters to open the decoder */


} AudioDecSvcInitParams_t;

/**
  Creates an instance of the Elite Audio Decoder service.

  @param [in] pInitParams  Pointer to AudioDecSvcInitParams_t,
  @param [out] handle      Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult AudioDecSvc_Create (AudioDecSvcInitParams_t *pInitParams, void **handle);

/**
  ID for an internal command for DTMF CTL.
*/
#define ELITEMSG_CUSTOM_DTMF_CTL     (0x00150003UL)

/**
  ID for for suspend command to decoder services.
*/
#define ELITEMSG_CUSTOM_DEC_SUSPEND     (0x00150004UL)

/** @brief DTMF CMD  parameters.
*/
typedef struct {
    qurt_elite_queue_t *pBufferReturnQ;     /**< Queue to which the payload buffer
                                            must be returned.*/

    qurt_elite_queue_t *pResponseQ;         /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required.*/

    uint32_t      unClientToken;       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t      unResponseResult;    /**< Response result that the server
                                            sends back to the client. */

    uint32_t      unSecOpCode;         /**< This is the secondary opcode indicating
                                            the format for the rest of payload */


    uint16_t      tone_1;              /**< The DTMF tone1.*/


    uint16_t      tone_2;              /**< The DTMF tone2.*/


    uint16_t      gain;               /**< Dtmf Tone linear gain
                                            Gain in Q13 format   .*/

    int16_t      duration;            /**< Duration of the DTMF tone in ms.
                                              -1      -> continuous,
                                               0      -> disable
                                               >0   -> finite */
    elite_msg_data_eos_info_t dtmfAprInfo;  /**<  This will store the information
                                               of src port and destination port to send the
                                                tone ended event */

} EliteMsg_CustomDtmfParamsType;


/**
  ID for opening a new input or output stream to a MIMO decoder.
*/
#define ELITEMSG_CUSTOM_DEC_OPEN_STREAM     (0x00150005UL)

/** @brief MIMO decoder input, output open params.
*/
typedef struct {
    qurt_elite_queue_t *pBufferReturnQ;     /**< Queue to which the payload buffer
                                            must be returned.*/

    qurt_elite_queue_t *pResponseQ;         /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required.*/

    uint32_t      unClientToken;       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t      unResponseResult;    /**< Response result that the server
                                            sends back to the client. */

    uint32_t      unSecOpCode;         /**< This is the secondary opcode indicating
                                            the format for the rest of payload */

    AudioDecSvcIOStreamInitParams_t openParams;
                                      /**< parameters to open a stream: can be input or output. */

    elite_svc_handle_t  *pHandle;      /** handle to this stream of the decoder */


} EliteMsg_CustomDecOpenStreamType;


/**
  ID for closing a new input or output stream to a MIMO decoder.
*/
#define ELITEMSG_CUSTOM_DEC_CLOSE_STREAM     (0x00150006UL)

/** @brief MIMO decoder input, output open params.
*/
typedef struct {
    qurt_elite_queue_t *pBufferReturnQ;     /**< Queue to which the payload buffer
                                            must be returned.*/

    qurt_elite_queue_t *pResponseQ;         /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required.*/

    uint32_t      unClientToken;       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t      unResponseResult;    /**< Response result that the server
                                            sends back to the client. */

    uint32_t      unSecOpCode;         /**< This is the secondary opcode indicating
                                            the format for the rest of payload */

    elite_svc_handle_t  *pHandle;      /** handle to this stream of the decoder */


} EliteMsg_CustomDecCloseStreamType;


/**
  ID for connecting a service to output of a decoder.
*/
#define ELITEMSG_CUSTOM_DEC_CONNECT_STREAM     (0x00150007UL)

/** @brief command to connect output of decoder to a svc
*/
typedef struct {
    qurt_elite_queue_t *pBufferReturnQ;     /**< Queue to which the payload buffer
                                            must be returned.*/

    qurt_elite_queue_t *pResponseQ;         /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required.*/

    uint32_t      unClientToken;       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t      unResponseResult;    /**< Response result that the server
                                            sends back to the client. */

    uint32_t      unSecOpCode;         /**< This is the secondary opcode indicating
                                            the format for the rest of payload */

    uint8_t       stream_id;           /**<stream id which should be connected to downstream. this must be an output stream */
    elite_svc_handle_t  *pDownStreamHandle;      /** handle to the downstream of the decoder */


} EliteMsg_CustomDecConnectStreamType;

/**
  ID for sending set PP param to decoder service
*/
#define ELITEMSG_CUSTOM_DEC_SET_PP_PARAM     (0x00150008UL)

/**
  ID for sending get PP param to decoder service
*/
#define ELITEMSG_CUSTOM_DEC_GET_PP_PARAM     (0x00150009UL)

/** @brief command to do set/get PP param to decoder service
*/
typedef struct {
    qurt_elite_queue_t *pBufferReturnQ;     /**< Queue to which the payload buffer
                                            must be returned.*/

    qurt_elite_queue_t *pResponseQ;         /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required.*/

    uint32_t      unClientToken;       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t      unResponseResult;    /**< Response result that the server
                                            sends back to the client. */

    uint32_t      unSecOpCode;         /**< This is the secondary opcode indicating
                                            the format for the rest of payload */

   uint32_t         unParamId;         /**< Parameter ID. The rest of
                                            the payload is derived based on this ID and the message. */

   int32_t          *pnParamData;      /**< Points to *_param_data_t (for example, adm_param_data_v5_t,
                                            voice_param_data_t, asm_stream_param_data_v2_t). */

   uint32_t          unSize;           /**< Payload size in bytes for the memory pointed to by pnParamData. */

} EliteMsg_CustomDecSetGetPpParamType;
/** @} */ /* end_addtogroup audio_decoder */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDCMNUTIL_H
