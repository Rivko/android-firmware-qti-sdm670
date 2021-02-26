/*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_apr_router.cpp#1 $
  $Author: pwbldsvc $
*/

#include "Elite.h"
#include "qurt_elite.h"
#include "vsm_apr_router.h"
#include "VoiceStreamMgr.h"


/* System command Queue and Response Queue */
static qurt_elite_queue_t* VoiceStreamMgr_SystemCmdQueue;
static qurt_elite_queue_t* VoiceStreamMgr_SystemRespQueue;

static vsm_apr_router_data_base_t vsm_apr_router_data_base;

void VoiceStreamMgr_AprRouterLock( )
{
  return qurt_elite_mutex_lock( &( vsm_apr_router_data_base.apr_router_lock ) );
}

void VoiceStreamMgr_AprRouterUnLock( )
{
  return qurt_elite_mutex_unlock( &( vsm_apr_router_data_base.apr_router_lock ) );
}

ADSPResult VSM_apr_router_get_svc_cmdQ( qurt_elite_queue_t** destQ_ptr )
{
  *destQ_ptr = VoiceStreamMgr_SystemCmdQueue;
  return ADSP_EOK;
}

ADSPResult VSM_apr_router_get_svc_respQ( qurt_elite_queue_t** destQ_ptr )
{
  *destQ_ptr = VoiceStreamMgr_SystemRespQueue;
  return ADSP_EOK;
}

/*
   This utility function get the command queue of corresponding session
*/
ADSPResult VSM_apr_router_get_cmdQ( uint8_t session_index, qurt_elite_queue_t **dest_cmdQ )
{

  *dest_cmdQ = NULL;

  if ( session_index < VSM_MAX_SUPPORTED_SESSION_ID )
  {
    vsm_apr_Rnode_t *node = &( vsm_apr_router_data_base.apr_router_LUT[ session_index ] );

    *dest_cmdQ = node->cmdQ;
  }

  if ( NULL == ( *dest_cmdQ ) ) {
    return ADSP_EHANDLE;
  }
  
  return ADSP_EOK;
}

/*
   This utility function get the response queue of corresponding session
*/
ADSPResult VSM_apr_router_get_respQ( uint8_t session_index, qurt_elite_queue_t **dest_respQ )
{
  *dest_respQ = NULL;

  if ( session_index < VSM_MAX_SUPPORTED_SESSION_ID )
  {
    vsm_apr_Rnode_t *node = &( vsm_apr_router_data_base.apr_router_LUT[ session_index ] );

    *dest_respQ = node->respQ;
  }

  if ( NULL == ( *dest_respQ ) ) {
    return ADSP_EHANDLE;
  }
  
  return ADSP_EOK;
}

ADSPResult VoiceStreamMgr_AprRouterDeinit( void )
{
  VoiceStreamMgr_SystemCmdQueue = NULL;
  VoiceStreamMgr_SystemRespQueue = NULL;
  qurt_elite_mutex_destroy( &( vsm_apr_router_data_base.apr_router_lock ) );
  memset( vsm_apr_router_data_base.apr_router_LUT, 0, sizeof( vsm_apr_router_data_base.apr_router_LUT ) );

  return ADSP_EOK;
}

/*
   This function is called after VoiceStreamManager is created. It fills in the
   command queue of different sessions.
   Given that the queues are created at init, we already know the command queue
   of each session in creation time. Hence, fill it here.
*/
ADSPResult VoiceStreamMgr_AprRouterInit( voice_stream_mgr_t *pInst )
{

  memset( vsm_apr_router_data_base.apr_router_LUT, 0, sizeof( vsm_apr_router_data_base.apr_router_LUT ) );

  qurt_elite_mutex_init( &( vsm_apr_router_data_base.apr_router_lock ) );

  for ( uint32_t i = 0; i < VSM_MAX_SUPPORTED_SESSION_ID; i ++ )
  {
    if ( ADSP_FAILED( VoiceStreamMgr_AprRouterRegisterCmdQ( i, pInst->static_info[i].cmdQ ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "AudioStreamMgr: Error in router init %lu", i );
      return ADSP_ENOTREADY;
    }

    if ( ADSP_FAILED( VoiceStreamMgr_AprRouterRegisterRespQ( i, pInst->static_info[i].respQ ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "AudioStreamMgr: Error in router init %lu", i );
      return ADSP_ENOTREADY;
    }
  }

  //Register system command queue and response queue seperately.
  VoiceStreamMgr_SystemCmdQueue = pInst->service_handle.cmdQ;
  VoiceStreamMgr_SystemRespQueue = pInst->respQ;

  return ADSP_EOK;
}

/*
   This is the function to register the command queue with the APR router.
*/
 ADSPResult VoiceStreamMgr_AprRouterRegisterCmdQ( uint8_t major, qurt_elite_queue_t *cmdQ )
{
    vsm_apr_Rnode_t *node;

    if ( major >= VSM_MAX_SUPPORTED_SESSION_ID || NULL == cmdQ )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
       "AudioStreamMgr_AprRouterRegister::BadParam: address[%u], cmdQ=0x%lx", major, (uint32_t) cmdQ );
      return ADSP_EBADPARAM;
    }

    node = &( vsm_apr_router_data_base.apr_router_LUT[ major ] );
    node->cmdQ = cmdQ;

    return ADSP_EOK;
}

/*
   This is the function to register the response queue with the APR router.
*/
 ADSPResult VoiceStreamMgr_AprRouterRegisterRespQ( uint8_t major, qurt_elite_queue_t *respQ )
{
    vsm_apr_Rnode_t *node;

    if ( major >= VSM_MAX_SUPPORTED_SESSION_ID || NULL == respQ )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
       "AudioStreamMgr_AprRouterRegister::BadParam: address[%u], respQ=0x%lx", major, (uint32_t) respQ );
      return ADSP_EBADPARAM;
    }

    node = &( vsm_apr_router_data_base.apr_router_LUT[ major ] );
    node->respQ = respQ;

    return ADSP_EOK;
}

