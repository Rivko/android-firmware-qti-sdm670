/**
@file EliteMsg_AdmCustom.h
@brief This file contains the interface structures for AudioDevMgr.
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
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/inc/EliteMsg_AdmCustom.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger. 
==========================================================================*/

#ifndef ELITEMSG_ADMCUSTOM_H
#define ELITEMSG_ADMCUSTOM_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "EliteMsg.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr.h"
/*This file has dependency on afe_client_t and afe_dma_config_t. Hence keep it separate
from AFEInterface.h
*/
#include "AFEInterface.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup audio_device
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
/** @cond
*/
/* Indicates that the rest of the payload is Start Device.
[This define is obsolete, not used anymore, and will be deprecated soon] */
#define ELITEMSG_CUSTOM_ADM_START_DEV           0x00300001

/* Indicates that the rest of the payload is Stop Device.
[This define is obsolete, not used anymore, and will be deprecated soon] */
#define ELITEMSG_CUSTOM_ADM_STOP_DEV            0x00300002
/** @endcond */

//Indicates that the rest of the message payload is Stream Connect.
#define ELITEMSG_CUSTOM_ADM_STREAM_CONNECT      0x00300003

//Indicates that the rest of the message payload is Stream Disconnect.
#define ELITEMSG_CUSTOM_ADM_STREAM_DISCONNECT   0x00300004

//Indicates that the rest of the message payload is Stream Disable.
#define ELITEMSG_CUSTOM_ADM_STREAM_DISABLE      0x00300005

//Indicates that the rest of the message payload is Matrix Reconfig.
#define ELITEMSG_CUSTOM_ADM_STREAM_MTMX_RECONFIG 0x00300006

//Indicates that the rest of the message payload is ASM AFE Connect.
#define ELITEMSG_CUSTOM_ADM_ASM_AFE_CONNECT      0x00300007

//Indicates that the rest of the message payload is ASM AFE Disconnect.
#define ELITEMSG_CUSTOM_ADM_ASM_AFE_DISCONNECT      0x00300008

//Indicates that the rest of the message payload is Get device path delay.
#define ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY  0x00300009

//Indicates that the rest of the message payload is LSM Stream Connect.
#define ELITEMSG_CUSTOM_ADM_LSM_STREAM_CONNECT      0x0030000A

//Indicates that the rest of the message payload is LSM Stream Disconnect.
#define ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT   0x0030000B

//Indicates that the rest of the message payload is LSM Stream Disable.
#define ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISABLE      0x0030000C

//Indicates that the rest of the message payload is Matrix Reconfig from LSM
#define ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG 0x0030000D

// Indicates the Rx direction.
#define ADM_RX_DIR                            0x0

// Indicates the Tx direction.
#define ADM_TX_DIR                            0x1

/** The following #defines are used for the unPortMode in 
EliteMsg_CustomAdmStreamConnectType

0: Invalid TS
1: Valid Relative TS
2: Valid Absolute TS */
#define ADM_STREAM_CONNECT_PORT_MODE_INVALID_TS      0x0
#define ADM_STREAM_CONNECT_PORT_MODE_RELATIVE_TS    0x1
#define ADM_STREAM_CONNECT_PORT_MODE_ABSOLUTE_TS   0x2

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/** @cond
 */
/* Payload structure for ELITE_CUSTOM_MSG and
unSecOpCode == ELITEMSG_CUSTOM_AFECONNECT/STARTDMA Header
[This structure is obsolete, not used anymore, and will be deprecated soon] */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ; /* Queue to which this payload
                                                  buffer must be returned. */
    qurt_elite_queue_t             *pResponseQ;   /* Queue to which to send the
                                                acknowledgment. NULL
                                                indicates that no response
                                                is required. */
    uint32_t                  unClientToken; /* Token to be given in the
                                                acknowledgment. This is
                                                different from the
                                                unResponseResult and can be
                                                used to identify which
                                                service sent the response to
                                                the server. */
    uint32_t                  unResponseResult; /* Response result that the
                                                   server sends back to the
                                                   client. */
    uint32_t                  unSecOpCode;   /* Secondary operation code that
                                                indicates the format for the
                                                rest of the payload. */
    uint32_t                  unAdmId;       /* AFE port ID and direction. */
} elite_msg_custom_adm_header_t;
/** @endcond */


/** @brief Payload structure for Stream Connect/Disconnect/Disable messages.
 */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t             *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t                  unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t                  unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

    uint32_t                  unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload.

   The secondary operation code for ELITE_CUSTOM_MSG is 32 bits and is
   globally unique. A service can define a globally unique secondary operation
   code as follows:

   - The most significant 16 bits are the most significant 16 bits of the
     service ID that defines this secondary operation code.
   - The least significant 16 bits are defined by the service.

   @note1hang If the most significant 16 bits of the secondary operation code
   are all zeros, this message is reserved for a common custom message. */

    elite_svc_handle_t        *phPort;     /**< Pointer to the handle of the
                                                peer to be connected to or
                                                disconnected from. */

    uint32_t                  unSessionID; /**< Descriptor of the stream that
                                                must be connected to the
                                                Tx/Rx audio matrix. The
                                                direction helps decide which
                                                matrix. This ID is a composite ID consisting of
                                                session ID on the top 8 bits and stream ID on the
                                                bottom 8 bits */

    uint32_t                  direction;   /**< Indicates whether the stream
                                                is Rx (0) or Tx (1). For an
                                                Rx stream, connect to the Rx
                                                audio matrix. For a Tx stream,
                                                connect to the Tx audio
                                                matrix. */

    Elite_CbType              eliteCb;    /**< Callback handle provided by
                                               the ASM to allow the audio
                                               matrix to report
                                               underflow/overflow events
                                               to the client. */

    uint32_t            bits_per_sample;           
                                          /**< New bits per sample. */

	uint32_t            unPortMode;
	                                      /**<Sets the mode of the port being connected.
										  Currently, supported only on the TX path.
										  Client should set this Mode to the following:
										  0: Invalid Time Stamp mode: This setting will be ignored as the audio matrix decides 
										      whether TS is valid or not (based on the liveness of the connected inputs).
										  1: Relative Time Stamp mode: TS starts from 0 always. 
										      Resets on Run and Flush. Data time-synchronization will not be enabled.
											  This is the default value (and mode).
                                          2: Absolute Time Stamp mode: TS is synchronized with the inputs to absolute AVTimer time.
										      Does not reset on Run and Flush. Data time-synchronization will be enabled. */
	uint32_t             unDataLogId;      /**< data logging id: upper 16 bits: asm apr port id. lower 16 bits: seq num.  */
	bool_t               is_compressed;    /**< indicates if the input/output is compressed or uncompressed
	                                            uncompressed - use matrix mixer
	                                            compressed   - use stream router*/
	uint32_t             format_id;       /**< Only required for compressed output*/
                                          
	uint32_t             buffering_ms;     /**< this buffering in ms decides the number of buffers &
	                                 may be length of the buffers. Dont care for Rx matrix.*/
                                          
	uint32_t             stream_perf_mode;        /**< stream performance mode, supported values are
                                                           ASM_LEGACY_STREAM_SESSION &
                                                           ASM_LOW_LATENCY_STREAM_SESSION */
                                          
} EliteMsg_CustomAdmStreamConnectType;

/** @brief Payload structure for Stream-side matrix reconfigure messages.
 */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t             *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t                  unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t                  unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

    uint32_t                  unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload.

   The secondary operation code for ELITE_CUSTOM_MSG is 32 bits and is
   globally unique. A service can define a globally unique secondary operation
   code as follows:

   - The most significant 16 bits are the most significant 16 bits of the
     service ID that defines this secondary operation code.
   - The least significant 16 bits are defined by the service.

   @note1hang If the most significant 16 bits of the secondary operation code
   are all zeros, this message is reserved for a common custom message. */

    elite_svc_handle_t        *phPort;     /**< Pointer to the handle of the
                                                peer to be connected to or
                                                disconnected from. */

    uint32_t                  unSessionID; /**< Descriptor of the stream that
                                                is connected to this marix port
												which is being configured.
												This ID is a composite ID consisting of
                                                session ID on the top 8 bits and stream ID on the
                                                bottom 8 bits */

    uint32_t                  direction;   /**< Indicates whether the stream
                                                is Rx (0) or Tx (1). For an
                                                Rx stream, connect to the Rx
                                                audio matrix. For a Tx stream,
                                                connect to the Tx audio
                                                matrix. */

    uint32_t               num_channels;             
	                                   /**< New number of channels. */
    uint32_t            bits_per_sample;           
	                                   /**< New bits per sample. */
     uint8_t          channel_mapping[8];        
	                                   /**< New channel mapping. */
      bool_t   bMtMxPassNativeNumChannels; 
	                                   /**< Native num ch. mode in matrix.
                                                If this flag is set, the i/p 
			     						        channels are propogated to the o/p 
										        (upto a max. of 8 channels). */
    uint32_t             buffering_ms;     /**< this buffering in ms decides the number of
                 buffers & may be length of the buffers.  Dont care for Rx matrix.*/

} EliteMsg_CustomAdmStreamMtMxReConfig;

/** @cond
 */
/* Payload structure for ELITE_CUSTOM_MSG and
  unSecOpCode == ELITEMSG_CUSTOM_AFESTARTDMA / ELITEMSG_CUSTOM_AFESTOPDMA
 [This structure is obsolete, not used anymore, and will be deprecated soon] */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ; /* Queue to which this payload
                                                  buffer must be returned. */
    qurt_elite_queue_t             *pResponseQ;   /* Queue to which to
                                                send the acknowledgment. NULL
                                                indicates that no response is
                                                required. */
    uint32_t                  unClientToken; /* Token to be given in the
                                                acknowledgment. This is
                                                different from the
                                                unResponseResult and can be
                                                used to identify which service
                                                sent the response to the
                                                server. */
    uint32_t                  unResponseResult; /* Response result that the
                                                   server sends back to the
                                                   client. */
    uint32_t                  unSecOpCode;   /* Secondary operation code that
                                                indicates the format for the
                                                rest of the payload. */
    uint32_t                  unAfeId;       /* AFE port ID. */
    elite_svc_handle_t        *phCopp;       /* Pointer to the handle of the
                                                common object
                                                postprocessor/preprocessor. */
} EliteMsg_CustomAdmDevType;
/** @endcond */

/* Payload structure for ELITE_CUSTOM_MSG and
    unSecOpCode == ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY
    This command message is sent to ADM from ASM upon ASM_SESSION_CMD_GET_PATH_DELAY.
    Uncompressed: ADM gives the delay due to Tx/Rx matrix, COPP/COPreP and AFE. In case of SSMD, it chooses a primary path delay.
    Compressed: ADM gives the delay of Stream Router and the AFE.*/
typedef struct
{
    qurt_elite_queue_t        *pBufferReturnQ; /* Queue to which this payload
                                                  buffer must be returned. */
    qurt_elite_queue_t        *pResponseQ;   /* Queue to which to
                                                send the acknowledgment. NULL
                                                indicates that no response is
                                                required. */
    uint32_t                  unClientToken; /* Token to be given in the
                                                acknowledgment. This is
                                                different from the
                                                unResponseResult and can be
                                                used to identify which service
                                                sent the response to the
                                                server. */
    uint32_t                  unResponseResult; /* Response result that the
                                                   server sends back to the
                                                   client. */
    uint32_t                  unSecOpCode;   /* Secondary operation code that
                                                indicates the format for the
                                                rest of the payload. */
    uint16_t                   session_id;    /*Session id of the session requesting
                                               connection/disconnection*/
    uint32_t                  direction;   /**< Indicates whether the matrix/streamrouter in the path
                                                   is Rx (0) or Tx (1).  */
	bool_t                    is_compressed;    /**< indicates if the input/output is compressed or uncompressed
	                                            uncompressed - use matrix mixer
	                                            compressed   - use stream router*/
    uint32_t                  device_path_buf_delay;   /* device path buffering delay in micro seconds. This is filled by ADM as response. */
    uint32_t                  device_path_alg_delay;   /* device path algorithmic delay in micro seconds. This is filled by ADM as response. */
} EliteMsg_CustomAdmGetDevicePathDelayCmd;


/** @} */ /* end_addtogroup audio_device */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_ADMCUSTOM_H

