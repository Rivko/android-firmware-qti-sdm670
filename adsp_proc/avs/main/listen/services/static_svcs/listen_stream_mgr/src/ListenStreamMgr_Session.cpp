/**
@file ListenStreamMgr_Session.cpp

@brief This file contains the implementation for session-control-related functions 
for ListenStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_Session.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir   Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "ListenStreamMgr_Session.h"
#include "ListenStreamMgr_AprIf.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// maximum number of commands expected ever in command queue.
#define LISTEN_STREAM_MGR_MAX_CMD_Q_ELEMENTS  16
#define LISTEN_STREAM_MGR_MAX_RESP_Q_ELEMENTS 16

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/* =======================================================================
 **                          Session State Machine Create/Destroy
 ** ======================================================================= */

/* This function creates all sessions state machine here. */
ADSPResult lsm_create_all_session_states(lsm_t *pMe) 
{
  int session_idx;
  lsm_session_state_type_t *pSession;

  for (session_idx =0; session_idx< LSM_MAX_SESSION_ID ; session_idx ++ )
  {
    pSession = &(pMe->aSessions[session_idx]);
    /* Session is in CLOSED state*/
    pSession->session_state = LISTEN_STREAM_MGR_SESSION_DEINIT;
    pSession->ucInternalSessionId = session_idx;
  }
  return ADSP_EOK;
}

/**
 * This function destroy session state assuming this session is
 * no longer in use. This function should only be called when
 * the whole static service is destroyed
 */
ADSPResult lsm_session_destroy_state(lsm_t *pMe, uint8_t ucSessionId)
{
  lsm_session_state_type_t *pSession = NULL;

  //even though session id is gettign in range of 1-8, still this check is required
  //to avoid KW error
  if(ucSessionId < LSM_MAX_SESSION_ID)
  {
    pSession = &(pMe->aSessions[ucSessionId]);
  }
  else
  {
    return  ADSP_EFAILED;
  }

  if (pSession == NULL)
  {
    return  ADSP_EFAILED;
  }

  if(LISTEN_STREAM_MGR_SESSION_DEINIT != pSession->session_state)
  {
    //Send a destroy to the dynamic service
    ADSPResult result = ADSP_EOK;
    int status;
    uint32_t nSize = sizeof(elite_msg_cmd_destroy_svc_t);
    elite_msg_any_t destroyMsg;
    elite_svc_handle_t * pDynSvc = pSession->dyn_svc.handle;

    if(ADSP_FAILED(result = elite_msg_create_msg( &destroyMsg, &nSize, ELITE_CMD_DESTROY_SERVICE, NULL, 0, ADSP_EOK)))
    {
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to create msg  : %d", result);
#endif
    }

    if(ADSP_FAILED(result = qurt_elite_queue_push_back(pDynSvc->cmdQ, (uint64_t* )&destroyMsg)))
    {
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to push msg: %d", result);
#endif
    }
    //this is blocking call until the dynamic thread exited.
    qurt_elite_thread_join(pDynSvc->threadId, &status);

    elite_msg_release_msg(&destroyMsg);
  }

  memset(pSession, 0, sizeof(lsm_session_state_type_t));

  return ADSP_EOK;
}


ADSPResult lsm_session_fill_cb_data_for_dynamic_svc(lsm_t *pMe, 
                                                    lsm_session_state_type_t *pSession,
                                                    lsm_callback_handle_t *pCbData)
{
  if ( pCbData )
  {
    elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pMe->msg.pPayload);
    pCbData->apr_handle   = lsm_get_apr_handle();
    pCbData->client_addr  = elite_apr_if_get_src_addr(pPkt);
    pCbData->client_port  = elite_apr_if_get_src_port(pPkt);
    pCbData->self_addr  = elite_apr_if_get_dst_addr(pPkt);
    pCbData->self_port   = elite_apr_if_get_dst_port(pPkt);
    return ADSP_EOK;
  }
  else
  {
    return ADSP_EBADPARAM;
  }
}
