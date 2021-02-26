
/*========================================================================
Elite

This file defines functions, variables for managing APR routing
table.

Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_AprDataBase.cpp#1 $$DateTime: 2017/11/07 23:00:25 $$Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/30/09   DC      Created file.

========================================================================== */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AudioStreamMgr_AprDataBase.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


//extern char AUDSTATPCMSVC_CMD_Q_NAME[];
extern char AUDIO_STREAM_MGR_SYS_CMD_Q_NAME[];
//extern char AUDSTAT_SC_TO_PLAYBACKSVC_CMD_Q_NAME[];
//char AUDSTAT_SC_TO_PLAYBACKSVC_CMD_Q_NAME[] = "PBSc00" ; //hack: remove this


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// look up table for audio router
static struct {
    AudioStreamMgr_RNodeType          aAprRouterLUT[ASM_MAX_SUPPORTED_SESSION_ID + 1 ];
    /**< at IPC level, Session ID 1- ASM_MAX_SUPPORTED_SESSION_ID .
         For easy of implementation, add empty space ahead.
         */
    qurt_elite_mutex_t  AprRouterLock;
    /**< To avoid hazard of multiple write from different thread to the data base.
         Lock of AudioStreamMgr_AprRouterLUT for writing.
         */
} AudioStreamMgr_AprRouterDataBase;

// Store the system command queue.
static qurt_elite_queue_t* AudioStreamMgr_SystemCmdQueue;

/*
   This is an event call back function handle provided from ASM
   to the dynamic services

*/
/*typedef AprEventSendFunc
static struct {

} AudioStreamMgr_CbHandleType;
*/
/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
void  AudioStreamMgr_AprRouterLock()
{
    return qurt_elite_mutex_lock( &(AudioStreamMgr_AprRouterDataBase.AprRouterLock) );
}

void  AudioStreamMgr_AprRouterUnLock()
{
    return qurt_elite_mutex_unlock( &(AudioStreamMgr_AprRouterDataBase.AprRouterLock) );
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*
   This function is called after AudioStreamMgr is created. It fills in the
   command queue of different sessions.
   Given that the client is generating handle, we already know the command queue
   of each session in creation time. Hence, fill it here.
*/
ADSPResult AudioStreamMgr_AprRouterInit(AudioStreamMgr_t *pInst)
{

    // Init LUT
    memset( AudioStreamMgr_AprRouterDataBase.aAprRouterLUT, 0, sizeof(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT) );
    qurt_elite_mutex_init( &(AudioStreamMgr_AprRouterDataBase.AprRouterLock) );

    for ( uint32_t i = 0; i < ASM_MAX_SUPPORTED_SESSION_ID; i ++ ) {
       if (ADSP_FAILED(AudioStreamMgr_AprRouterRegisterCmdQ(i+1, pInst->aStSessions[i].pSessionCmdQ))){
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
              "AudioStreamMgr: Error in router init %lu", i);
          return ADSP_ENOTREADY;
       }
    }
	//Register system command queue seperately.
	AudioStreamMgr_SystemCmdQueue = pInst->serviceHandle.cmdQ;
    return ADSP_EOK;
}

/*
   Potential race condition,
    1. if ASM deregister a data queue, while apr is try to push it into queue.
         The safest way to avoid this race condition is to have
         mutex lock into the queue entry on register/deregister function
         and APR call back function.
         Having mutex per session is too many mutexes. We can just
         lock the entire data base in register/deregister function.
         In APR-ISR function, we can lock the data base, get the dataqueu
         pushe the message to data queue.
*/

/*
This will be called to register dynamic service data queue.
*/
ADSPResult AudioStreamMgr_AprRouterRegisterDatQ( elite_apr_port_t port,
                                                 uint32_t   unDirection, /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                 qurt_elite_queue_t *pDatQ

                                                   )
{

   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID ||
         unDirection > 1) {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AudioStreamMgr:Address Out of Range:[%u,%u,%lu]", major,minor, unDirection);
       return ADSP_EBADPARAM;
   }

   AudioStreamMgr_AprRouterLock();
   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);
   pNode->dataQ[minor][unDirection] = pDatQ;
   AudioStreamMgr_AprRouterUnLock();
   return ADSP_EOK;

}

ADSPResult AudioStreamMgr_AprRouterDeRegisterDatQ( elite_apr_port_t port,
                                                   uint32_t   unDirection /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                   )
{

   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID ||
         unDirection > 1) {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AudioStreamMgr:Address Out of Range:[%u,%u,%lu]", major,minor, unDirection);
       return ADSP_EBADPARAM;
   }

   AudioStreamMgr_AprRouterLock();
   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);
   pNode->dataQ[minor][unDirection] = NULL;
   AudioStreamMgr_AprRouterUnLock();

   return ADSP_EOK;

}

/*
   This is the function to register the command queue with the APR router.
*/
 ADSPResult AudioStreamMgr_AprRouterRegisterCmdQ( uint8_t major, qurt_elite_queue_t *pCmdQ )
{
    AudioStreamMgr_RNodeType *pNode;

    if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
          NULL == pCmdQ
          ) {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
         "AudioStreamMgr_AprRouterRegister::BadParam: address[%u], cmdQ=0x%lx", major, (uint32_t) pCmdQ);
        return ADSP_EBADPARAM;
    }

    pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[ major ]);
    pNode->cmdQ = pCmdQ;

    return ADSP_EOK;
}


/*
   This utility function get the data queue of corresponding
*/
 ADSPResult  AudioStreamMgr_AprRouterGetDataQ( elite_apr_port_t port,
                                             uint32_t unDirection, /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                    qurt_elite_queue_t **pDataQ
                                             )
{
   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID ||
         unDirection > 1) {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
           "AudioStreamMgr:Address Out of Range:[%u,%u,%lu]", major,minor, unDirection);
       return ADSP_EBADPARAM;
   }

   // Lock: avoid the race condition between ASM update data queue and client continue to send data
   // ???? Think again.
   // We do not lock here, the caller lock AudioStreamMgr_AprRouterLock();

   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);

   *pDataQ = pNode->dataQ[minor][unDirection];

   //  AudioStreamMgr_AprRouterUnLock();


   if ( NULL == (*pDataQ) ) {
      return  ADSP_ENOTREADY;
   }
   return ADSP_EOK;
}

/*
   This utility function get the command queue of corresponding command queue
*/
ADSPResult  AudioStreamMgr_AprRouterGetCmdQ(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ)
{
   uint8_t major = ASM_GET_GROUP_ID( port);

   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ) {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AudioStreamMgr_AprRouterRegister:Address Out of Range:[%u]", major);
       return ADSP_EBADPARAM;
   }

   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);

   *pCmdQ = pNode->cmdQ;

   if ( NULL == (*pCmdQ) ) {
     return ADSP_EBADPARAM;
   }
   return ADSP_EOK;

}

/*
   This utility function get the system command queue
*/
ADSPResult AudioStreamMgr_GetSysCmdQ(qurt_elite_queue_t **pCmdQ)
{
	*pCmdQ = AudioStreamMgr_SystemCmdQueue;
	return ADSP_EOK;
}

ADSPResult AudioStreamMgr_AprRouterRegisterPPCb(elite_apr_port_t port,
                                                AudioStreamMgr_PPCallbackType callback)
{


   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID ) {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AudioStreamMgr:Address Out of Range:[%u,%u]", major,minor);
       return ADSP_EBADPARAM;
   }

   AudioStreamMgr_AprRouterLock();
   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);
   pNode->PPCallback[minor] = callback;
   AudioStreamMgr_AprRouterUnLock();
   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_AprRouterDeRegisterPPCb( elite_apr_port_t port )
{

   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID) {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AudioStreamMgr:Address Out of Range:[%u,%u]", major,minor);
       return ADSP_EBADPARAM;
   }

   AudioStreamMgr_AprRouterLock();
   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);
   pNode->PPCallback[minor].function = NULL;
   pNode->PPCallback[minor].context = NULL;
   AudioStreamMgr_AprRouterUnLock();

   return ADSP_EOK;
}

ADSPResult  AudioStreamMgr_CallPPCb(elite_apr_port_t port,
                                    elite_msg_any_t msg)
{
   uint8_t major = ASM_GET_GROUP_ID( port);
   uint8_t minor = ASM_GET_STREAM_ID( port);


   if (  major > ASM_MAX_SUPPORTED_SESSION_ID ||
         minor > ASM_MAX_SUPPORTED_STREAM_ID ) {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
           "AudioStreamMgr:Address Out of Range:[%u,%u]", major,minor);
       return ADSP_EBADPARAM;
   }

   // Lock: avoid the race condition between ASM update data queue and client continue to send data
   // We do not lock here, the caller lock AudioStreamMgr_AprRouterLock();

   AudioStreamMgr_RNodeType *pNode = &(AudioStreamMgr_AprRouterDataBase.aAprRouterLUT[major]);
   AudioStreamMgr_PPCallbackType *pCallback = &(pNode->PPCallback[minor]);

   if ( NULL == pCallback->function ) {
      return  ADSP_ENOTREADY;
   }

   return pCallback->function(pCallback->context, msg);
}

