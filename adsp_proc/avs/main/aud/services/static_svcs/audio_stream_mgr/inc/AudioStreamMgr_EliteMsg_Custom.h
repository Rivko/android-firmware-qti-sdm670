/**
@file AudioStreamMgr_EliteMsg_Custom.h
@brief This file contains the interface structures for
       AudioStreamMgr.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/AudioStreamMgr_EliteMsg_Custom.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/07/12   aprasad created file.
==========================================================================*/

#ifndef AUDIO_STREAM_MGR_ELITEMSG_CUSTOM_H
#define AUDIO_STREAM_MGR_ELITEMSG_CUSTOM_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "EliteMsg.h"
#include "EliteMsg_Custom.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup audio_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/** Operation code for the EliteMsg_CustomASMCfgPPOutputSampleRateType message.
*/
#define ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT_SAMPLE_RATE 0x000c0001

/** Typedef for the EliteMsg_CustomASMCfgPPOutputSampleRateType structure.
*/
typedef struct EliteMsg_CustomASMCfgPPOutputSampleRateType EliteMsg_CustomASMCfgPPOutputSampleRateType;

/**
  @brief Custom message to set the output sample rate of the postprocessor/preprocessor.

  This message can be sent only before sending the first Run command to the
  postprocessor/preprocessor. After that, the output media type parameters are
  fixed.
 */
struct EliteMsg_CustomASMCfgPPOutputSampleRateType
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t           outputSamplingRate;
                                       /**< Output sampling rate in Hz for the
                                            postprocessor/preprocessor. */

    uint16_t           unStreamID;
                                       /**< Destination Stream ID for this message. */
};

/** Operation code for the custom message to connect a stream session to downstream device
    bypassing the matrix mixer. The downstream device may be the AFE in ULL/LLNP modes or the
    COPP in ULLPP mode. This should be transparent to the ASM.
*/
#define ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT 0x000c0002

/** Operation code for the custom message to disconnect a stream session from the downstream device.
*/
#define ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_DISCONNECT 0x000c0003

/** Typedef for the EliteMsg_CustomASMStreamAfeConnect structure.
*/
typedef struct EliteMsg_CustomASMStreamDirectConnectType EliteMsg_CustomASMStreamDirectConnectType;

/**
  @brief Custom message to connect stream session to AFE.

  This message is intended for only Ultra low latency(ULL) stream sessions.
  ASM will connect decoder service with AFE port handle available in 
  this message. No POPP processing is involved in ULL stream sessions.
  Decoder directly writes data to AFE port. ADM sends this command during matirx
  map routing command.
 */
struct EliteMsg_CustomASMStreamDirectConnectType
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    elite_svc_handle_t  *phDownstreamDevice;   
                                       /**< Pointer to the downstream device handle */

    uint16_t           unStreamID;
                                       /**< Destination Stream ID for this message. */
};

/**
  @brief Custom message to query ASM for the service handle the AFE should connect to

  This message is intended for only Low-Latency No Processing (LLNP) Tx stream sessions.
  ASM will provide the handle to the service the AFE should connect to during an LLNP Tx service.
  This handle is usually the encoder. AFE directly passes data to this service.
*/
#define ELITEMSG_CUSTOM_ASM_LLNP_STREAM_QUERY_TX_SINK_HANDLE 0x000c0004

typedef struct EliteMsg_CustomAsmCommandQueryTxSinkHandle EliteMsg_CustomAsmCommandQueryTxSinkHandle;

struct EliteMsg_CustomAsmCommandQueryTxSinkHandle
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    elite_svc_handle_t  *phSinkHandle;
                                       /**< Pointer to the service handle who
                                            receives the upstream connection */
    uint16_t           unStreamID;
                                       /**< Destination Stream ID for this message. */
};

/**
  @brief Custom message to alert the ASM of a pending disconnection from upstream service

  This message is intended for only Low-Latency No Processing (LLNP) Tx stream sessions.
  This is to alert the ASM that a disconnection is pending and that it should return all
  buffers that any service may be holding on to and to take any required actions before
  disconnection.
 */
#define ELITEMSG_CUSTOM_ASM_LLNP_PREPARE_FOR_DISCONNECT 0x000c0005

typedef struct EliteMsg_CustomAsmCommandPrepDisconnect EliteMsg_CustomAsmCommandPrepDisconnect;

struct EliteMsg_CustomAsmCommandPrepDisconnect
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */
    uint16_t           unStreamID;
                                       /**< Destination Stream ID for this message. */
};

/** @} */ /* end_addtogroup audio_stream */

/** Operation code for the EliteMsg_CustomASMCfgPPOutput message.
*/
#define ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT 0x000c0006

/** Typedef for the EliteMsg_CustomASMCfgPPOutput structure.
*/
typedef struct EliteMsg_CustomASMCfgPPOutput EliteMsg_CustomASMCfgPPOutput;

/**
  @brief Custom message to set the output media type of the postprocessor/preprocessor.

  This message can be sent only before sending the first Run command to the
  postprocessor/preprocessor. After that, the output media type parameters are
  fixed.
 */
struct EliteMsg_CustomASMCfgPPOutput
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t           outputSamplingRate;
                                       /**< Output sampling rate for the
                                            postprocessor/preprocessor. */

    uint32_t           outputNumChannels;
                                       /**< Number of output channels for the
                                            postprocessor/preprocessor. */
    uint8_t            outputChannelMap[8];
                                       /**< Channel map for the
                                            postprocessor/preprocessor. */

    uint32_t           outputBitsPerSample;
                                       /**< Number bits per sample for the
                                            postprocessor/preprocessor. */

    uint16_t           unStreamID;
                                       /**< Destination Stream ID for this message. */
};

/** Operation code for the EliteEvent_CustomASMListenToEoSAck message.
 *
 * for rendered_eos_v2 event.
*/
#define ELITE_EVENT_CUSTOM_ASM_LISTEN_TO_EOS_ACK 0x000c0007

/** Typedef for the EliteEvent_CustomASMListenToEoSAck structure.
*/
typedef struct EliteEvent_CustomASMListenToEoSAck EliteEvent_CustomASMListenToEoSAck;

/**
  @brief Custom message to set the output media type of the postprocessor/preprocessor.

   this command enables the ASM to listen to the EoS ACK.
   After getting the first EoS ACK after this, the wait is cleared.

   the stream-id is obtained from the client token
 */
struct EliteEvent_CustomASMListenToEoSAck
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */
};
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_ELITEMSG_CUSTOM_H

