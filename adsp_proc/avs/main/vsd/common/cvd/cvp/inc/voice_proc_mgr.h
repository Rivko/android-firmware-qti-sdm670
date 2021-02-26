/**========================================================================
 @file voice_proc_mgr.h
 
 @brief This file contains public API's for Voice Proc Manager session control
 and configuration handling 
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/inc/voice_proc_mgr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/*==========================================================================
  Include files
========================================================================== */

#ifndef _VOICE_PROC_MGR_H_
#define _VOICE_PROC_MGR_H_

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

typedef enum
{
   VOICEPROC_CONNECT_DWN_STREAM_CMD = 0,
   /**< Connect to downstream peer */
   VOICEPROC_DISCONNECT_DWN_STREAM_CMD,
   /**< Disconnect from downstream peer */
   VOICEPROC_RECONFIG_CMD,
   /**< Reconfigure to a new topology/sampling rate/device */
   VOICEPROC_RUN_CMD,
   /**< Move to run state and start processing data */
   VOICEPROC_STOP_CMD,
   /**< Move to stop state and stop processing data */
   VOICEPROC_SET_PARAM_CMD,
   /**< Set a parameter */
   VOICEPROC_GET_PARAM_CMD,
   /**< Get a parameter */
   VOICEPROC_DESTROY_YOURSELF_CMD,
   /**< Destroy the service instance */
   VOICEPROC_CONNECT_RX_PEER,
   /**< Connect to rx path peer.

     Required for Automatic Volume Control/Receive voice enhancement algorithms
     to get feedback of rx path gain
     */
   VOICEPROC_DISCONNECT_RX_PEER,
   /**< Disconnect from rx path peer */
   VOICEPROC_SET_MUTE_CMD,
   /**< Control mute/unmute */
   VOICEPROC_CONFIG_HOST_PCM,
   /**< Configure Host PCM */
   VOICEPROC_SET_TIMING_CMD,
   /**< Set timing parameters */
   VOICEPROC_SET_TIMINGV2_CMD,
   /**< Set timing parameters with VPM specific API */
   VOICEPROC_GET_KPPS_CMD,
   /**< Command to get  KPPS */
   VOICEPROC_SET_TIMINGV3_CMD,
   /**< Set timing parameters with VPM specific API to support VSID */   
   VOICEPROC_GET_DELAY_CMD,
   /**< Command to get delay */
   VOICEPROC_SET_PARAM_V3_CMD,
   /**< Command to set compressed param */
   VOICEPROC_REGISTER_EVENT,
   /**< Command to register event */
   VOICEPROC_UNREGISTER_EVENT,
   /**< Command to unregister event */
   VOICEPROC_SET_EP_ID,
   /**< Command to set end point ids  */
   VOICEPROC_NUM_MSGS
   /**< Number of supported message */
} voiceproc_custom_msgs_t;

typedef enum
{
   VOICEDEV_NUM_MSGS = 0
   /**< Number of supported message */
} voicedev_custom_msgs_t;

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** @brief Structure of primary payload used in vptx/vprx run custom msgs.
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */
    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */
    uint32_t           unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */
    uint32_t           unResponseResult;
    /**< This is to be filled with ACK result by the client. */
    uint32_t           unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */
    void               *afe_drift_ptr;
    /**< Pointer to drift struct, return from AFE connect in VPM. */
    void               *far_src_drift_ptr;
    /**< Pointer to drift struct, return from AFE connect in VPM. */
    uint32_t           session_thread_clock_mhz;
    /**< Service may assume this to be the minimum thread clock while in Run state */
} elite_msg_custom_voiceproc_run_payload_t;

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
  Creates an instance of the Voice Proc Manager service.

  @param[in]  inputParam  unResponseResultis not used.
  @param[out] handle      Service entry handle that is returned to the caller.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult voice_proc_mgr_create (uint32_t inputParam, void **handle);

/** @} */ /* end_addtogroup audio_stream */

#endif /* _VOICE_PROC_MGR_H_ */

