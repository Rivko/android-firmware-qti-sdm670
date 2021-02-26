/**
@file MixerSvc_Interface.h
@brief This file contains the interface structures for Audio
       Matrix Mixer.
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
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/inc/MixerSvc_Interface.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/01/09   AAA      Created file.
10/20/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
==========================================================================*/

#ifndef MT_MX_INTERFACE_H
#define MT_MX_INTERFACE_H

#include "AudioStreamMgr.h"
#include "adsp_adm_api.h"
#include "AFEAvtDrift.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** @addtogroup matrix_mixer
@{ */

/** Typedef for the MtMxInPortHandle_t structure.
*/
typedef struct MtMxInPortHandle_t MtMxInPortHandle_t;

/** @brief Port handle to an input port on the audio matrix.
*/
struct MtMxInPortHandle_t
{
   uint32_t             inPortID;      /**< Input port ID. */
   elite_svc_handle_t   portHandle;    /**< Handle containing the port queue
                                            information. */
};

/** Typedef for the MtMxOutPortHandle_t structure.
*/
typedef struct MtMxOutPortHandle_t MtMxOutPortHandle_t;

/** @brief Port handle to an input port on the audio matrix.
*/
struct MtMxOutPortHandle_t
{
   uint32_t             outPortID;     /**< Output port ID. */
   elite_svc_handle_t   portHandle;    /**< Handle containing the port queue
                                            information. */
};

/** Typedef for the the MtMxParams_t structure.
*/
typedef struct MtMxParams_t MtMxParams_t;

/**
  @brief Holds the configuration parameters that are used when creating an
  instance of the audio matrix.
*/
struct MtMxParams_t
{
    uint32_t maxInPorts;            /**< Maximum number of input ports in the
                                         matrix. */
    uint32_t maxOutPorts;           /**< Maximum number of output ports in the
                                         matrix. */
    uint32_t matrixID;              /**< Unique ID of the matrix. */
};

/** Typedef for the MtMxInPortCfgParams_t structure.
*/
typedef struct MtMxInPortCfgParams_t MtMxInPortCfgParams_t;

/**
  @brief Holds parameters that are used to configure an input port on the
  audio matrix.
*/
struct MtMxInPortCfgParams_t
{
    int           index;         /**< Input port ID. */
    uint32_t      configuration; /**< Input port configuration. Possible
                                      values:\n
                                      - NEW_INPUT
                                      - CLOSE_INPUT */
    uint32_t      strMask;       /**< Bitmask that indicates the output ports
                                      into which this input port is feeding. */
    uint32_t      numChannels;   /**< Number of channels in the input buffer
                                      that this port will receive. */
    bool_t        bIsInPortLive; /**< Flag that indicates whether the input port
                                      is live or non-live.\n
                                      - A live input port drops the input
                                        buffer if no output buffer is
                                        available at the time, and the input
                                        port is ready to receive the next input
                                        buffer. The input port repeats this
                                        until an output buffer is available. Then
                                        the input port sends data from the input
                                        buffer downstream via the output buffer.
                                      - A non-live input port does not drop
                                        an input buffer if an output buffer is
                                        not available at the time. The input port
                                        simply holds the buffer until an output
                                        buffer becomes available. Then it sends
                                        data from the input buffer downstream
                                        via the output buffer. */
    bool_t    bIsInPortBurstMode; /**< Flag that indicates whether the input port 
                                      is in burst node or non-burst mode.*/
    Elite_CbType  *pEliteCb;     /**< Pointer to the ASM callback structure. */

    uint32_t       unDataLogId;  /**< data logging id: In case of stream-side port: 
                                      upper 16 bits: asm apr port id lower 16 bits seq num.  */

    volatile uint32_t    *punMtMxDelay;		/**<ADM passes the address of the MtMx delay.
    											MtMx is supposed to fill this in */

    uint16_t      perfMode;     /**< indicates the performance mode that this input port
                                      is opened. Supported values are ADM_LEGACY_DEVICE_SESSION
                                       & ADM_LOW_LATENCY_DEVICE_SESSION */
    volatile const afe_drift_info_t **ppAfeDriftPtr; /*Address of the AFE drift pointer */
    bool_t        bIsAdmLoopback; /**< 1-> ADM loopback 0-> not */
};

/** Typedef for the CfgInputPortsPayload_t structure.
*/
typedef struct CfgInputPortsPayload_t CfgInputPortsPayload_t;

/**
  @brief Sent as part of the Matrix Configure Input Ports command, which
  configures an  input port on the audio matrix. Possible configurations
  include opening a new input port or closing an existing port.
*/
struct CfgInputPortsPayload_t
{
    uint32_t               numInPorts;        /**< Number of input ports being
                                                   configured. */
    MtMxInPortCfgParams_t  *pInPortCfgParams; /**< Pointer to the structure
                                                   that contains the
                                                   configuration parameters
                                                   for the input port. */
    MtMxInPortHandle_t     **pAck;            /**< Pointer to the input port
                                                   handle that is returned to
                                                   the caller. */
};

/** Operation code for the Matrix Configure Input Ports message.
*/
#define ELITEMSG_CUSTOM_CFG_INPUT_PORTS      0x00180001

/**
  Enumeration to indicate that the input port is being configured to reassign
  it to a different output port.
*/
#define REASSIGN_INPUT 0

/** Enumeration to indicate that a new input port is being configured.
*/
#define NEW_INPUT 1

/** Enumeration to indicate that an existing input port is being closed.
*/
#define CLOSE_INPUT 2

/** Payload format for the Matrix Configure Input Ports message.
*/
typedef struct EliteMsg_CustomCfgInPortsType EliteMsg_CustomCfgInPortsType;

/**
  @brief Payload structure for the Matrix Configure Input Ports message.

  This custom message can be used to perform one of the following:

  - Create a new input port.
  - Reassign an existing input port to different output ports.
  - Close an existing input port on the audio matrix.

  The input port's configuration parameters are stored in a structure of type
  CfgInputPortsPayload_t.
*/
struct EliteMsg_CustomCfgInPortsType
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
                                            of the payload. */

    CfgInputPortsPayload_t    cfgInPortsPayload;
                                       /**< Configuration parameters for the
                                            input ports. */
};

/** Operation code for the Matrix port state change command.
*/
#define ELITEMSG_CUSTOM_MT_MX_PORT_STATE_CHANGE 0x0018000D

/** Payload format for the Matrix Flush message.
*/
typedef struct EliteMsg_CustomMtMxPortStateChange EliteMsg_CustomMtMxPortStateChange;

/**
  @brief Payload structure for the Matrix Port State Change 
         commands.
*/
struct EliteMsg_CustomMtMxPortStateChange
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< ID of the port that is to be
                                            changed. */

    uint32_t         unPortDirection;  /**< Direction of the port that is
                                            to be changed. This field indicates
                                            whether the port is an input or
                                            output port. */

    uint32_t         unTargetSampleRate; /**< This tells the audio matrix
                                            the sampling rate to expect in it's subsequent media type 
                                            message. */
};

/** Operation code for the Matrix Pause message.
*/
#define ELITEMSG_CUSTOM_MT_MX_PAUSE    0x00180003

/** Enumeration to indicate that the port being configured is an input port.
*/
#define MATRIX_MIXER_PORT_DIR_INPUT                         0x1

/** Enumeration to indicate that the port being configured is an output port.
*/
#define MATRIX_MIXER_PORT_DIR_OUTPUT                        0x2

/** Payload format for the Matrix Pause message.
*/
typedef struct EliteMsg_CustomMtMxPauseFlushType EliteMsg_CustomMtMxPauseType;

/** Operation code for the Matrix Flush message.
*/
#define ELITEMSG_CUSTOM_MT_MX_FLUSH 0x00180005

/** Operation code for the Matrix Suspend message.
*/
#define ELITEMSG_CUSTOM_MT_MX_SUSPEND     0x00180011

/** Payload format for the Matrix Flush message.
*/
typedef struct EliteMsg_CustomMtMxPauseFlushType EliteMsg_CustomMtMxFlushType;

/** Payload format for the Matrix Suspend message.
*/
typedef struct EliteMsg_CustomMtMxPauseFlushType EliteMsg_CustomMtMxSuspendType;

/**
  @brief Payload structure for the Matrix Pause and Matrix Flush commands.

  Because this is a port-specific message, the client is to specify the port
  ID. Additionally, the client is to also specify the port direction so the
  audio matrix can figure out whether it is an input port or output port.
*/
struct EliteMsg_CustomMtMxPauseFlushType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< ID of the port that is to be
                                            paused/flushed/suspended. */

    uint32_t         unPortDirection;  /**< Direction of the port that is
                                            to be flushed. This field indicates
                                            whether the port is an input or
                                            output port. */
};

/** Operation code for the Matrix Run message.
*/
#define ELITEMSG_CUSTOM_MT_MX_RUN      0x00180004

/**
  Enumeration to indicate that the input port rendering process is to begin
  immediately.
*/
#define MATRIX_MIXER_RUN_START_TIME_RUN_IMMEDIATE           0

/**
  Enumeration to indicate that the input port rendering process is to begin at
  the specified absolute, wall-clock time.
*/
#define MATRIX_MIXER_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME    1

/**
  Enumeration to indicate that the input port rendering process is to begin at
  the specified relative (from the current time) time.
*/
#define MATRIX_MIXER_RUN_START_TIME_RUN_AT_RELATIVE_TIME    2

/** Payload format for the Matrix Run message.
*/
typedef struct EliteMsg_CustomMtMxRunType EliteMsg_CustomMtMxRunType;

/**
  @brief Payload structure for the Matrix Run message.

  This custom message is used to kick start the rendering process of an audio
  stream connected to the audio matrix. The command can instruct the audio
  matrix to begin the rendering process immediately, at a specified absolute
  time, or at a specified relative time.
*/
struct EliteMsg_CustomMtMxRunType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< Port that is to be run. */

    uint32_t         unPortDirection;  /**< Indicates whether the port is an
                                            input or output port. */

    int64_t         ullStartTime;     /**< Time (in microseconds) to which to
                                            align the session origin time
                                            (timestamp=0). */

    uint32_t         unStartFlag;      /**< Indicates whether to run immediately
                                            or at a specific rendering time. */
};

/** Operation code for the Matrix Get Session Time message.
*/
#define ELITEMSG_CUSTOM_MT_MX_GET_SESSION_TIME     0x00180007

/** Payload format for the Matrix Get Session Time message.
*/
typedef struct EliteMsg_CustomMtMxGetSessionType EliteMsg_CustomMtMxGetSessionType;

/**
  @brief Payload structure for the Matrix Get Session Time message.

  This custom message is sent to the audio matrix to retrieve information about
  an audio stream's session time (how much content has been rendered to the
  device). This information can be used by the client for Audio-Video
  Synchronization (AV-Sync) or progress bar updates.
*/
struct EliteMsg_CustomMtMxGetSessionType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< ID of the port whose session time
                                            is requested. */

    uint32_t         unPortDirection;  /**< Direction of the port whose
                                            session time is requested.
                                            Indicates whether the port is an
                                            input or output port. */

    uint64_t         ullSessionTime;   /**< Session time (in microseconds). */

    uint64_t         ullAbsoluteTime;  /**< Absolute time (in microseconds)
                        corresponding to above session time. Goes in pairs. */
};

/** Typedef for the MtMxOutPortCfgParams_t structure. */
typedef struct MtMxOutPortCfgParams_t MtMxOutPortCfgParams_t;

/**
   @brief Holds parameters that are used to configure an output port on the audio
   matrix.
*/
struct MtMxOutPortCfgParams_t
{
    int                    index;            /**< Output port ID. */

    uint32_t               configuration;    /**< Output port configuration.
                                                  Possible values:\n
                                                  - NEW_OUTPUT
                                                  - CLOSE_OUTPUT
                                                  - DISABLE_OUTPUT */

    uint32_t               numOutputBufs;    /**< Number of output buffers for
                                                  this output port. */

    uint32_t               unBitWidth; 		/**< Bit width per PCM sample. */

    uint32_t               maxBufQCapacity;  /**< Maximum capacity of the
                                                  buffer queue. */

    elite_svc_handle_t*    pSvcHandle;       /**< Pointer to the downstream peer
                                                  service handle. */

    uint32_t               outputMode;       /**< Output port mode of operation.
                                                  Possible values:\n
                                                  - PULL_MODE */

    uint32_t               numChannels;      /**< Output port's number of
                                                  channels. */

    uint8_t                channelMapping[8];/**< Channel mapping of buffers
                                                  that the audio matrix will
                                                  send downstream.

                                                  Channel mapping indicates how
                                                  various channels in an audio
                                                  stream are mapped to different
                                                  speakers in the receiving
                                                  device. */

    uint32_t               interleaveFlag;   /**< Indicates whether data is
                                                  interleaved or deinterleaved
                                                  from this output port. */

    uint32_t               inPortsMask;      /**< Bitmask that indicates the
                                                  input ports feeding into this
                                                  output port. */

    Elite_CbType           *pEliteCb;        /**< Pointer to the ASM callback
                                                  structure. */

    uint32_t              unPortMode;
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
    uint32_t             unDataLogId;      /**< data logging id: In case of stream-side port: 
                                                upper 16 bits: asm apr port id lower 16 bits seq num.  */
    uint32_t             unSampleRate;      /**< sample rate: sample rate of output port  */                                                
    volatile uint32_t    *punCoppBufDelay;     /**<ADM passes the address of the COPP buffering delay. COPP is supposed to fill this in */
    volatile uint32_t    *punCoppAlgDelay;     /**<ADM passes the address of the COPP algorithmic delay. COPP is supposed to fill this in */
    volatile uint32_t    *punAFEDelay;      /**<ADM passes the address of the AFE delay. AFE is supposed to fill this in */
    volatile uint32_t    *punMtMxDelay;    /**<ADM passes the address of the MtMx delay. MtMx is supposed to fill this in */
    uint16_t      perfMode;                /**< indicates the performance mode that this input port
                                                 is opened. Supported values are ADM_LEGACY_DEVICE_SESSION
                                                    & ADM_LOW_LATENCY_DEVICE_SESSION */
    uint8_t unNativeModeFlags; /**< Flag used to indicate native mode operation.*/
    uint32_t      unConnectedAsmSessionID; /**< The session/stream ID of connected ASM session */
    volatile const afe_drift_info_t **ppAfeDriftPtr; /*Address of the AFE drift pointer */
};

/** Typedef for the CfgOutputPortsPayload_t structure. */
typedef struct CfgOutputPortsPayload_t CfgOutputPortsPayload_t;

/**
  @brief Sent as part of the Matrix Configure Output Ports command, which
  configures an output port on the audio matrix.

  Possible configurations include:

  - Opening a new output port.
  - Disabling an existing output port.
  - Closing an existing output port.
  - Re-configuring an existing output port.
*/
struct CfgOutputPortsPayload_t
{
    uint32_t                  numOutPorts;   /**< Number of output ports
                                                  being configured. */
    MtMxOutPortCfgParams_t    *pOutPortCfgParams;
                                       /**< Pointer to the structure that
                                            contains the configuration
                                            parameters for the output port. */

    MtMxOutPortHandle_t       **pAck;  /**< Pointer to the output port handle
                                            that is returned to the caller. */
};

/** Operation code for the Matrix Configure Output Ports message.
*/
#define ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS     0x00180002

/** Enumeration to indicate that a new output port is being opened.
*/
#define NEW_OUTPUT 1

/** Enumeration to indicate that an existing output port is being closed.
*/
#define CLOSE_OUTPUT 2

/** Enumeration to indicate that an existing output port is being disabled.
*/
#define DISABLE_OUTPUT 3

/** Enumeration to indicate that an existing output port is being reconfigured.
*/
#define RECONFIGURE_OUTPUT 4

/** Enumeration to indicate that an existing output port is being reconfigured
    with sample rate information.
*/
#define RECONFIGURE_OUTPUT_SAMPLE_RATE 5

/** Enumeration to indicate the Pull mode of operation of the output port.
*/
#define PULL_MODE 1

/** Payload format for the Matrix Configure Output Ports message.
*/
typedef struct EliteMsg_CustomCfgOutPortsType EliteMsg_CustomCfgOutPortsType;

/**
  @brief Payload structure for the Matrix Configure Output Ports message.

  This custom message can be used to:

  - Create a new output port.
  - Disable an existing output port.
  - Close an existing output port on the audio matrix.

  The output port's configuration parameters are stored in a structure of
    type CfgOutputPortsPayload_t.
*/
struct EliteMsg_CustomCfgOutPortsType
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
                                            of the payload. */

    CfgOutputPortsPayload_t   cfgOutPortsPayload;
                                       /**< Configuration parameters for the
                                            output ports. */
};

/** Operation code for the Matrix Map Routings message.
*/
#define ELITEMSG_CUSTOM_MT_MX_MAP_ROUTINGS      0x00180008

/** Payload format for the Matrix Map Routings message.
*/
typedef struct EliteMsg_CustomMtMxMapRoutingsType EliteMsg_CustomMtMxMapRoutingsType;

/**
  @brief Payload structure for the Matrix Map Routings message.

  This custom message is used to indicate the mapping between input and output
  ports of the audio matrix, i.e., it specifies which to output ports an input
  port is routed. This command can be used to send the routing information of
  multiple input ports at one time.
*/
struct EliteMsg_CustomMtMxMapRoutingsType
{
    qurt_elite_queue_t                      *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t                      *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t                           unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t                           unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

    uint32_t                           unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    adm_cmd_matrix_map_routings_v5_t      *pRoutingPayload;
                                       /**< Pointer to the payload containing
                                            the input port to output port map
                                            routing. */
};

/** Operation code for the Matrix Register Underflow/Overflow message.
*/
#define ELITEMSG_CUSTOM_MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT      0x00180009

/** Enumeration to enable under/overflow events.
*/
#define MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT_ENABLE  1

/** Enumeration to disable under/overflow events.
*/
#define MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT_DISABLE 0

/** Payload format for the Matrix Register Underflow/Overflow message.
*/
typedef struct EliteMsg_CustomMtMxUnderOverFlowEventType EliteMsg_CustomMtMxUnderOverFlowEventType;

/**
  @brief Payload structure for the Matrix Register Underflow/Overflow message.

  This custom message is used to enable/disable the Rx or Tx audio matrix from
  reporting underflow or overflow events, respectively.

  -  The Rx audio matrix reports an underflow event to the client when an
     output port does not have data (the input buffer was not available in
     time), and it sends zeros downstream.
  -  The Tx audio matrix reports an overflow event to the client when an input
     port does not have output buffers.
     If the i/p port is live, the buffer is dropped.
     If the i/p port is non-live:
       For ADSP_ENEEDMORE error code, the buffer is retained until the resource (buffer) is available.
       However, for non-recoverable errors (all other error codes), that buffer is dropped.
*/
struct EliteMsg_CustomMtMxUnderOverFlowEventType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< Port ID. */

    uint32_t         unPortDirection;  /**< Indicates whether the port is an
                                            input or output port. */

    uint16_t         usEnable;         /**< Enables (1) or disables (0) the
                                            following event report:

                                            - ASM_SESSION_EVENT_TX_OVERFLOW
                                              for the Tx matrix output port.
                                            - ASM_SESSION_EVENT_RX_UNDERFLOW
                                              for the Rx matrix input port. */
};

/** Operation code for the Matrix Ramp Gains message.
*/
#define ELITEMSG_CUSTOM_MT_MX_RAMP_GAINS     0x0018000A

/** Payload format for the Matrix Ramp Gains message.
*/
typedef struct EliteMsg_CustomMtMxRampGainsType EliteMsg_CustomMtMxRampGainsType;

/**
   @brief Payload structure for the Matrix Ramp Gains message.

   This custom message is used to specify the gains on
   input-port-to-output-port paths. The gains are specified per-channel, and
   this message supports up to 8 channels. The gains can be applied either
   immediately or in a ramp fashion to avoid pop due to gain change.

   This command can be used to apply per-stream-per-device gains and for soft
   stepping during stream volume change.
*/
struct EliteMsg_CustomMtMxRampGainsType
{
   qurt_elite_queue_t                    *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

   qurt_elite_queue_t                    *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates that
                                            no response is required. */

   uint32_t                         unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

   uint32_t                         unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

   uint32_t                         unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

   adm_cmd_matrix_ramp_gains_v5_t      *pRampGainsPayload;
                                       /**< Pointer to the payload containing
                                            the input-port-to-output-port
                                            ramp gains. */
};

/** Operation code for the Matrix Adjust Session Clock message.
*/
#define ELITEMSG_CUSTOM_MT_MX_ADJUST_SESSION_CLK     0x0018000B

/** Payload format for the Matrix Adjust Session Clock message.
*/
typedef struct EliteMsg_CustomMtMxAdjustSessionClkType EliteMsg_CustomMtMxAdjustSessionClkType;

/**
   @brief Payload structure for the Matrix Adjust Session Clock message.

   The custom Matrix Adjust Session Clock message is sent to the audio matrix to
   initiate adjustment of the session clock by a required amount. This is useful
   in ISDB-T cases where the AV-Sync clock is not the audio session clock and there
   is a need to correct the drift between the two clocks.
*/
struct EliteMsg_CustomMtMxAdjustSessionClkType
{
    qurt_elite_queue_t    *pBufferReturnQ;    /**< Queue to which this payload buffer
                                              is to be returned. */

    qurt_elite_queue_t    *pResponseQ;        /**< Queue to which to send the
                                              acknowledgment. NULL indicates that
                                              no response is required. */

    uint32_t         unClientToken;      /**< Token that is to be given in the
                                              acknowledgment. This is different
                                              from the unResponseResult and can be
                                              used to identify who send the
                                              acknowledgment back by the server. */

    uint32_t         unResponseResult;   /**< Filled with acknowledgment results
                                              by the client. */

    uint32_t         unSecOpCode;        /**< Secondary operation code that
                                              indicates the format for the rest
                                              of the payload. */

    uint32_t         unPortID;           /**< ID of the port whose session clock
                                              is to be adjusted. */

    uint32_t         unPortDirection;    /**< Direction of the port whose session
                                              clock is to be adjusted. This field
                                              indicates whether the port is an input
                                              or output port. */

    int64_t          llAdjustTime;       /**< Session clock adjustment (in
                                              microseconds).
                                              - Positive values indicate that the
                                                session clock is to be advanced.
                                              - Negative values indicate that the
                                                session clock is to be delayed. */

    int64_t          llActualAdjustTime; /**< Actual adjustment (in microseconds)
                                              that is performed by the matrix.
                                              This is filled by the matrix as a
                                              command response. */

    uint64_t         ullCmdLatency;      /**< Command latency (in microseconds)
                                              for completing the session clock
                                              adjustment. This is filled by the
                                              matrix as a command response. */
};

/** Operation code for the Matrix Mute message.
*/
#define ELITEMSG_CUSTOM_MT_MX_MUTE     0x0018000C

/** Payload format for the Matrix Mute message.
*/
typedef struct EliteMsg_CustomMtMxMuteType EliteMsg_CustomMtMxMuteType;

/**
   @brief Payload structure for the Matrix Mute message.

   This custom message is used to specify the mute/un-mute state for 
   input-port-to-output-port paths per channel. This message supports up to 8 
   channels. The mutes can be applied either immediately (default) or in a 
   linear ramp fashion. 
*/
struct EliteMsg_CustomMtMxMuteType
{
   qurt_elite_queue_t                    *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

   qurt_elite_queue_t                    *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates that
                                            no response is required. */

   uint32_t                             unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

   uint32_t                             unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

   uint32_t                             unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

   adm_cmd_matrix_mute_v5_t                 *pMutePayload;
                                       /**< Pointer to the payload containing
                                            the input-port-to-output-port
                                            mute/un-mute state. */
};

/** Operation code for the Matrix Set Param message.
*/
#define ELITEMSG_CUSTOM_MT_MX_SET_PARAM     0x0018000E

/** Operation code for the Matrix Get Param message.
*/
#define ELITEMSG_CUSTOM_MT_MX_GET_PARAM     0x0018000F

/** Payload format for the Matrix Get/Set Param message.
*/
typedef struct EliteMsg_CustomMtMxGetSetParamType EliteMsg_CustomMtMxGetSetParamType;

/**
  @brief Payload structure for the Matrix Get/Set Param message.

  This custom message is sent to the audio matrix to set any parameters on it/get any parameters from it.
*/
struct EliteMsg_CustomMtMxGetSetParamType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< ID of the port where the get/set
                                            param is requested. */

    uint32_t         unPortDirection;  /**< Indicates whether the port is an
                                            input or output port. */

   uint32_t         unParamId;        /**< Elite parameter ID; supports only
                                           ELITEMSG_PARAM_ID_CAL. The rest of
                                           the payload is derived based on this
                                           ID and the message. */

   int32_t          *pnParamData;     /**< Points to *_param_data_t
                                           (e.g:asm_session_param_data_t) */

   uint16_t          unSize;          /**< Payload size in bytes for the memory
                                           pointed by pnParamData.
                                           For GetParams,  this will be the max size of the
                                           expected data to be returned.*/
};

/** Operation code for the PSPD Matrix Set Param message.
*/
#define ELITEMSG_CUSTOM_MT_MX_PSPD_SET_PARAM     0x00180010

/** Payload format for the PSPD Matrix Get/Set Param message.
*/
typedef struct EliteMsg_CustomMtMxPspdGetSetParamType EliteMsg_CustomMtMxPspdGetSetParamType;

/**
  @brief Payload structure for the PSPD Matrix Get/Set Param message.

  This custom message is sent to the audio matrix to set any parameters on it/get any parameters from it.
*/
struct EliteMsg_CustomMtMxPspdGetSetParamType
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

   uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

   uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

   uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

   uint32_t         unInPortID;         /**< ID of the i/p port where the PSPD get/set
                                            param is requested. */

   uint32_t         unOutPortID;         /**< ID of the o/p port where the PSPD get/set
                                            param is requested. */

   uint32_t         unParamId;        /**< Elite parameter ID; supports only
                                           ELITEMSG_PARAM_ID_CAL. The rest of
                                           the payload is derived based on this
                                           ID and the message. */

   int32_t          *pnParamData;     /**< Points to *_param_data_t
                                           (e.g:adm_pspd_param_data_t) */

   uint16_t          unSize;          /**< Payload size in bytes for the memory
                                           pointed by pnParamData.
                                           For GetParams,  this will be the max size of the
                                           expected data to be returned.*/
};

/** Operation code for setting primary output port of input ports. This is applicable
    only for RX Matrix.
*/
#define ELITEMSG_CUSTOM_MT_MX_SET_PRIMARY_PORT   0x0000AAAA

/** payload for the primary output port set message.
*/
typedef struct EliteMsg_CustomSetPrimaryPortType EliteMsg_CustomSetPrimaryPortType;

/**
  @brief Payload structure for the primary output port set message.

  This custom message is used to indicate the primary output port of all
  active input ports. This custom message will be applicable only for RX matrix.
  Output ports that are connected to pullable PLLs will become primary ports.   

  The payload for this event comprises one instance of
  EliteMsg_CustomSetPrimaryPortType, followed immediately by
  unNumSessions number of instances of EliteMsg_CustomPrimaryPortMappingInfoType  
*/
struct EliteMsg_CustomSetPrimaryPortType
{
    qurt_elite_queue_t                 *pBufferReturnQ;
                                       /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t                 *pResponseQ;
                                       /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t                           unClientToken;
                                       /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t                           unResponseResult;
                                       /**< Response result that the server
                                            sends back to the client. */

    uint32_t                           unSecOpCode;
                                       /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t                           unNumSessions;
                                       /**< Number of active sessions. 
                                            For each session mapping information of
                                            session (input port) to output port 
                                            will be followed after this.*/
};

typedef struct EliteMsg_CustomPrimaryPortMappingInfoType EliteMsg_CustomPrimaryPortMappingInfoType;

/**
  @brief Mapping information of session (input port) to primary output port
*/

struct EliteMsg_CustomPrimaryPortMappingInfoType
{
    uint16_t                           unInputPort;
                                       /**< Input port. */

    uint16_t                           unPrimaryOutputPort;
                                       /**< Primary output port of above 
                                            input port. */											
};


/** Operation code for the Matrix Set Ratematching message.
*/
#define ELITEMSG_CUSTOM_MT_MX_SET_RATEMATCHING_PARAM     0x00180012

/** Payload format for the Matrix Set Ratematching message.
*/
typedef struct EliteMsg_CustomMtMxSetRateMatchingParamType EliteMsg_CustomMtMxSetRateMatchingParamType;

/**
  @brief Payload structure for the Matrix Set Ratematching Param message.

  This custom message is sent to the audio matrix to set Ratematching parameters on it.
*/
struct EliteMsg_CustomMtMxSetRateMatchingParamType
{
    qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

    qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t         unClientToken;    /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t         unResponseResult; /**< Response result that the server
                                            sends back to the client. */

    uint32_t         unSecOpCode;      /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */

    uint32_t         unPortID;         /**< ID of the port where the get/set
                                            param is requested. */

    uint32_t         unPortDirection;  /**< Indicates whether the port is an
                                            input or output port. */

   uint32_t          renderingDecison; /**< Payload to hold flag to indicate
                                            Rendering Decison */

   uint32_t          ClkRecoveryMode;  /**< Payload to hold flag to indicate
                                            Clock recovery method */

   uint16_t          unSize;          /**< Payload size in bytes for the memory
                                           pointed by pnParamData.
                                           For GetParams,  this will be the max size of the
                                           expected data to be returned.*/
};


/** Operation code for the Matrix Output Port Set Param message.
*/
#define ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_SET_PARAM     0x00180015

/** Payload format for the Matrix Output Port Set Param message.
*/
typedef struct EliteMsg_CustomMtMxOutputPortSetParamType EliteMsg_CustomMtMxOutputPortSetParamType;

/**
  @brief Payload structure for the Matrix Output Port Set Param message.

  This custom message is sent to the audio matrix output port to set any parameters on it.
*/
struct EliteMsg_CustomMtMxOutputPortSetParamType
{
   qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

   qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

   uint32_t   unClientToken;    /**< Token to be given in the
                                     acknowledgment. This is different
                                     from the unResponseResult and can
                                     be used to identify which service
                                     sent the response to the server. */

   uint32_t   unResponseResult; /**< Response result that the server
                                     sends back to the client. */

   uint32_t   unSecOpCode;      /**< Secondary operation code that
                                     indicates the format for the rest
                                     of the payload. */

   uint32_t   unOutPortID;      /**< ID of the matrix o/p port where 
                                     the set param to be set.*/

   uint32_t   unParamId;        /**< Elite parameter ID; supports only
                                     ELITEMSG_PARAM_ID_CAL. The rest of
                                     the payload is derived based on this
                                     ID and the message. */

   int32_t    *pnParamData;     /**< Points to *_param_data_t
                                     (e.g:adm_param_data_v6_t) */

   uint32_t   unSize;           /**< Payload size in bytes for the memory
                                     pointed by pnParamData.*/
};

/** Operation code for the Matrix Output Port Get Param message.
*/
#define ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_GET_PARAM     0x00180016

/** Payload format for the Matrix Output Port Get Param message.
*/
typedef struct EliteMsg_CustomMtMxOutputPortGetParamType EliteMsg_CustomMtMxOutputPortGetParamType;

/**
  @brief Payload structure for the Matrix Output Port Get Param message.

  This custom message is sent to the audio matrix output port to get any parameters on it.
*/
struct EliteMsg_CustomMtMxOutputPortGetParamType
{
   qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                            must be returned. */

   qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

   uint32_t   unClientToken;    /**< Token to be given in the
                                     acknowledgment. This is different
                                     from the unResponseResult and can
                                     be used to identify which service
                                     sent the response to the server. */

   uint32_t   unResponseResult; /**< Response result that the server
                                     sends back to the client. */

   uint32_t   unSecOpCode;      /**< Secondary operation code that
                                     indicates the format for the rest
                                     of the payload. */

   uint32_t   unOutPortID;      /**< ID of the matrix o/p port where 
                                     the get param to be query.*/

   uint32_t   unParamId;        /**< Elite parameter ID; supports only
                                     ELITEMSG_PARAM_ID_CAL. The rest of
                                     the payload is derived based on this
                                     ID and the message. */

   int32_t    *pnParamData;     /**< Points to *_param_data_t
                                     (e.g:adm_param_data_v6_t) */

   uint32_t   unSize;           /**< Payload size in bytes for the memory
                                     pointed by pnParamData.*/
};

/** @} */ /* end_addtogroup matrix_mixer */
/** @addtogroup matrix_mixer
@{ */

/*----------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/**
  Creates an instance of the audio matrix.

  @param[in] inputParam    Obsolete. To be deprecated.
  @param[in] pMtMxParams   Pointer to a structure containing the configuration
                           parameters for this instance of the audio matrix.
  @param[out] handle       Service entry handle returned to the caller.

  @return
  Indication of success or failure.
*/
ADSPResult AudCreateMatrixMixerSvc (uint32_t inputParam, void **handle, MtMxParams_t* pMtMxParams);

/** @} */ /* end_addtogroup matrix_mixer */

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MT_MX_INTERFACE_H
