#ifndef _VSM_APR_ROUTER_H_
#define _VSM_APR_ROUTER_H_
/*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/inc/vsm_apr_router.h#1 $
  $Author: pwbldsvc $
*/

#include "qurt_elite.h"
#include "VoiceStreamMgr.h"

typedef struct vsm_apr_Rnode_t{
  qurt_elite_queue_t  *cmdQ;    // Only one command q per group
  qurt_elite_queue_t  *respQ;    // Only one command q per group
  /* Note: Response queue is added temporarily as dynamic services
     are still sending some events as APR packets.*/
  /* TBD: the responses/events to static services shall be sent through elite.*/
} 
  vsm_apr_Rnode_t;

// look up table for audio router
typedef struct vsm_apr_router_data_base_t{
    vsm_apr_Rnode_t     apr_router_LUT[VSM_MAX_SUPPORTED_SESSION_ID + 1 ];
    /**< at IPC level, Session ID 1- VSM_MAX_SUPPORTED_SESSION_ID .
         For easy of implementation, add empty space ahead.
         */
    qurt_elite_mutex_t  apr_router_lock;
    /**< To avoid hazard of multiple write from different thread to the data base.
         Lock of apr_router_LUT for writing.
         */
} 
  vsm_apr_router_data_base_t;

void VoiceStreamMgr_AprRouterLock( void );

void VoiceStreamMgr_AprRouterUnLock( void );

/*
   This utility function gets the command queue for the service
*/
ADSPResult VSM_apr_router_get_svc_cmdQ( qurt_elite_queue_t **dest_cmdQ );

/*
   This utility function gets the response queue for the service
*/
ADSPResult VSM_apr_router_get_svc_respQ( qurt_elite_queue_t **dest_respQ );

/*
   This utility function gets the command queue of corresponding session
*/
ADSPResult VSM_apr_router_get_cmdQ( uint8_t session_index, qurt_elite_queue_t **dest_cmdQ );

/*
   This utility function gets the response queue of corresponding session
*/
ADSPResult VSM_apr_router_get_respQ( uint8_t session_index, qurt_elite_queue_t **dest_respQ );

/*
   This function is called after VoiceStreamManager is created. It fills in the
   command queue of different sessions.
   Given that the queues are created at init, we already know the command queue
   of each session in creation time. Hence, fill it here.
*/
ADSPResult VoiceStreamMgr_AprRouterInit( voice_stream_mgr_t *pInst );

ADSPResult VoiceStreamMgr_AprRouterDeinit( void );

/*
   This is the function to register the command queue with the APR router.
*/
 ADSPResult VoiceStreamMgr_AprRouterRegisterCmdQ( uint8_t major, qurt_elite_queue_t *cmdQ );
 
/*
   This is the function to register the response queue with the APR router.
*/
 ADSPResult VoiceStreamMgr_AprRouterRegisterRespQ( uint8_t major, qurt_elite_queue_t *respQ );
 
 #endif //_VSM_APR_ROUTER_H_ 
