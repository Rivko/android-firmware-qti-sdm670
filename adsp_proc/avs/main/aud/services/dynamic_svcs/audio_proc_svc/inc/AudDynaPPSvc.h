/*========================================================================*/
/**
@file AudDynaPPSvc.h
@brief This file declares the postprocessing and preprocessing modules for
       Elite.

This file implement the PP (audio postprocessor and preprocessor) process and setup functions specific to Elite

This is a service for audio postprocessing and preprocessing.
It implements the algorithms for processing uncompressed audio
data. Multiple instances of this module can exist, typically one
per stream/device.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_processing"
      group description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/12/09   JG      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
12/17/10   sw      (TP) For Rev B of PDF, edited Doxygen comments on lines
                   112-115 and 241-249.
========================================================================== */
#ifndef ELITE_DYNAPPSVC_H
#define ELITE_DYNAPPSVC_H

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup audio_processing
@{ */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** Service types for the postprocessing service.
*/
enum AudPP_AudProcType
{
   DYNA_SVC_PP_TYPE_POPP = 0,  /**< ID for per object (stream leg)
                                    postprocessing. */
   DYNA_SVC_PP_TYPE_COPP,      /**< ID for common object (common leg or device
                                    leg) postprocessing. */
   DYNA_SVC_PP_TYPE_POPREP,    /**< ID for per object (stream leg)
                                    preprocessing. */
   DYNA_SVC_PP_TYPE_COPREP,    /**< ID for common object (common leg or
                                    device leg) preprocessing. */
   DYNA_SVC_PP_NUM_TYPES       /**< Number of audio processor service types. */
};

/**
  Type for the postprocessor/preprocessor callback function to be called when
  host packets are passed directly to the postprocessor.
 */
typedef ADSPResult (*AudPP_MsgCbType)(void *pCtxt, elite_msg_any_t msg);

/**
  @brief Output Media Type information to be passed to the postprocessor/preprocessor.
 */
struct AudPPSvcOutputMediaTypeParams_t
{
    bool_t             keepInputSamplingRate;
                                       /**< If TRUE, the value of
                                            outputSamplingRate is ignored. */

    uint32_t           outputSamplingRate;
                                       /**< Output sampling rate in Hz for the
                                            postprocessor/preprocessor. */

    bool_t             useNativeNumChannels;
                                       /**< Flag that indicates whether to use
                                            the native number of channels.

                                            If TRUE, the value of
                                            outputNumChannels is ignored.
                                            It is then set to the corresponding
                                            default value by each module */

    uint32_t           outputNumChannels;
                                       /**< Number of output channels for the
                                            postprocessor/preprocessor. */
    uint8_t            outChannelMap[8];

    bool_t             useNativeBitsPerSample;
                                      /**< Flag that indicates whether to use
                                           the native bits per sample.

                                           If TRUE, the value of
                                           outputBitsPerSample is ignored.
                                            It is then set to the corresponding
                                            default value by each module */

    uint32_t           outputBitsPerSample;
                                       /**< Number bits per sample for the
                                            postprocessor/preprocessor. */
};

/**
  @brief Parameters to be passed to the postprocessor/preprocessor during
  initialization.
 */
struct AudPPSvcInitParams_t
{
   AudPP_AudProcType type;        /**< Indicates where in the processing chain
                                       this module resides. */

   uint32_t topology;             /**< Selects the order and type of
                                       postprocessing/preprocessing components. */

   uint16_t outputBufferNumAfeFramesPerChannel;
				   /**< Identifies the number of AFE frames to be used in the Output Buffer Frame*/
   AudPPSvcOutputMediaTypeParams_t  outputMediaTypeInitParams;
                                   /**< output media type init params. */                                  

   uint32_t memoryMapClient;      /**< Memory map client passed in by the ASM
                                       and ADM. */

   AudPP_MsgCbType *pMsgCb;       /**< Callback function for passing messages
                                       directly from the host is returned
                                       here. */

   void **pCtxt;                  /**< Context to pass to the
                                       postprocessor/preprocessor callback
                                       function is returned here. */

   volatile uint32_t      *pBufDelay; /**< Pointer to a memory location where the buffering delay will be maintained.
                                       For COPP, this will be the sum of the delays due to all the buffers that are allocated.
                                       For other PP types, this will be the delay of a single buffer.
                                       The delay value will be in microseconds.
                                       Note: This memory should not be written to by anyone else. */

   volatile uint32_t      *pAlgorithmicDelay; /**< Pointer to a memory location where the current total
                                       algorithmic delay of the post processing chain will be written.
                                       The delay value will be in microseconds.
                                       Note: This memory should not be written to by anyone else. */

   uint32_t  ulDataLogId;         /**< data logging id: upper 16 bits:apr port id. lower 16 bits seq num>  */

   bool_t      initializeWithCompressedFormat;  /**< Indicates that the topology must be initialized for
                                       compressed format. The actual format will be extracted from the media type. */
   uint16_t  perf_mode;
                                 /**< indicates which performance mode of PP session..
                                      Supported values are only ASM_LEGACY_STREAM_SESSION,
                                      ASM_LOW_LATENCY_STREAM_SESSION &
                                      ASM_ULTRA_LOW_LATENCY_STREAM_SESSION for POPP and PoPrep. 
                                      Supported values are ADM_LEGACY_DEVICE_SESSION or
                                      ADM_LOW_LATENCY_DEVICE_SESSION or 
                                      ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION. for COPP and CoPrep.*/
									  
   void   *pCbData;
      /**< ASM callback handle for event generation/acknowledgment. */

   bool_t	is_input_output_realtime;		/**< This variable is set by ASM/ADM to indicate if data
   	   	   	   	   	   	   	   	   	   feeding into PP service is realtime. In existing use cases,
   	   	   	   	   	   	   	   	   	   this is set in ASM loopback only*/
   bool_t	is_echo_canceller_session;		/**< Indicates whether this is an EC session in which
   	   	   	   	   	   	   	   	   	   case multi-input ports have to be created*/
};

/** Operation code for the EliteMsg_CustomCfgPPOutputType message.
*/
#define ELITEMSG_CUSTOM_CFG_PP_OUTPUT 0x000E0001

/** Typedef for the EliteMsg_CustomCfgPPOutputType structure.
*/
typedef struct EliteMsg_CustomCfgPPOutputType EliteMsg_CustomCfgPPOutputType;

/**
  @brief Custom message to set the parameters for the output media type of the
  postprocessor/preprocessor.
 */
struct EliteMsg_CustomCfgPPOutputType
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

    AudPPSvcOutputMediaTypeParams_t params;  /**< The output media type to be set. */
};

/** Operation code for the EliteMsg_CustomCfgPPOutputSampleRateType message.
*/
#define ELITEMSG_CUSTOM_CFG_PP_OUTPUT_SAMPLE_RATE 0x000E0003

/** Typedef for the EliteMsg_CustomCfgPPOutputSampleRateType structure.
*/
typedef struct EliteMsg_CustomCfgPPOutputSampleRateType EliteMsg_CustomCfgPPOutputSampleRateType;

/**
  @brief Custom message to set the output sample rate of the postprocessor/preprocessor.

  This message can be sent only before sending the first Run command to the
  postprocessor/preprocessor. After that, the output media type parameters are
  fixed.
 */
struct EliteMsg_CustomCfgPPOutputSampleRateType
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
    bool_t             keepInputSamplingRate;
                                       /**< If TRUE, the value of
                                            outputSamplingRate is ignored. */                                           

    uint32_t           outputSamplingRate;
                                       /**< Output sampling rate in Hz for the
                                            postprocessor/preprocessor. */
};

/** Operation code for the EliteMsg_CustomPPPauseType message.
*/
#define ELITEMSG_CUSTOM_PP_PAUSE                    0x000E0002

/** Typedef for the EliteMsg_CustomPPPauseType structure.
*/
typedef struct EliteMsg_CustomPPPauseType EliteMsg_CustomPPPauseType;

/** @brief Custom message used to pause any postprocessor/preprocessor instance.
 */
struct EliteMsg_CustomPPPauseType
{
    qurt_elite_queue_t   *pBufferReturnQ;   /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;       /**< Queue to which to send the
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

    bool_t           sendDownEosMarker; /**< Flag that indicates whether an EOS
                                             marker must be sent to the
                                             downstream service upon
                                             completion of pause processing. */
};

/** Operation code for the EliteMsg_CustomPPPauseBeginType message.
*/
#define ELITEMSG_CUSTOM_PP_PAUSE_PREPARE                    0x000E0004

/** Typedef for the EliteMsg_CustomPPPauseType structure.
*/
typedef struct EliteMsg_CustomPPPausePrepareType EliteMsg_CustomPPPausePrepareType;

/** @brief Custom message used to pause any postprocessor/preprocessor instance.
 */
struct EliteMsg_CustomPPPausePrepareType
{
    qurt_elite_queue_t   *pBufferReturnQ;   /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;       /**< Queue to which to send the
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

    bool_t           sendDownEosMarker; /**< Flag that indicates whether an EOS
                                             marker must be sent to the
                                             downstream service upon
                                             completion of pause processing. */

};

/** Operation code for the EliteMsg_Custom_Get_PP_Topology_Modules.
*/
#define ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES         0x000E0005

/** Operation code for the EliteMsg_Custom_Get_PP_Topology_Modules_V2.
*/
#define ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES_V2         0x000E0008

/** Typedef for the EliteMsg_Custom_Get_PP_Topology_Modules structure.
*/
typedef struct EliteMsg_CustomCfg_GetPPTopologyModuleInfo EliteMsg_CustomCfg_GetPPTopologyModuleInfo;

/**
  @brief Custom message to get module information in pp topology.

 */
struct EliteMsg_CustomCfg_GetPPTopologyModuleInfo
{
    qurt_elite_queue_t   *pBufferReturnQ;   /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;       /**< Queue to which to send the
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
    int32_t          *pnParamData;
	/**< Points to module info structure */

    uint32_t                 unSize;
    /**< Payload size in bytes for the memory pointed by pnParamData and the list of modules */
};

/** Operation code for the EliteMsg_Custom_Suspend.
    There is no payload for this command.
*/
#define ELITEMSG_CUSTOM_PP_SUSPEND         0x000E0006


/** Operation code for the EliteMsg_CustomPPGetNumOutputBuffers
*/
#define ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS     0x000E0007

/** Typedef for the EliteMsg_CustomPPGetNumOutputBuffers structure.
*/
typedef struct EliteMsg_CustomPPGetNumOutputBuffers EliteMsg_CustomPPGetNumOutputBuffers;

/**
  @brief Custom message to get the number of output buffers in the steady state.

 */
struct EliteMsg_CustomPPGetNumOutputBuffers
{
    qurt_elite_queue_t   *pBufferReturnQ;   /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;       /**< Queue to which to send the
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
    uint32_t         unNumBuffers;     /**< The number of output buffers used in
                                            the steady state. */
};


/*----------------------------------------------------------------------------
* Class Definitions
* -------------------------------------------------------------------------*/

/** @} */ /* end_addtogroup audio_processing */
/** @addtogroup audio_processing
@{ */

/*----------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/**
  Creates an instance of the Audio Dynamic Postprocessor/Preprocessor service.

  @param[in]  pParams   Parameters required during initialization.
  @param[out] handle    Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult AudPP_CreateSvc (const AudPPSvcInitParams_t *pParams, void **handle, void **far_handle);

/** @} */ /* end_addtogroup audio_processing */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_DYNAPPSVC_H


