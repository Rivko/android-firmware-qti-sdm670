/**
@file ListenStreamMgr_Session.h
@brief This file declares session-control-related functions for ListenStreamMgr.

 */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_Session.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_SESSION_H
#define LISTEN_STREAM_MGR_SESSION_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "ListenStreamMgr_Type.h"
#include "ListenStreamMgr.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/


  /*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/


  /* This function creates all sessions state machine here.
   @param pMe [in/out]  This points to the instance of ListenStreamMgr in which session state
                        structures are to be created.

   @return Success or fail 
   */
  ADSPResult lsm_create_all_session_states(lsm_t *pMe) ;

  /*
  This function destroy the necessary session state
  structure given the internal session ID that it needs to
  destroy. Currently only supports up to 15 sessions.

   @param pMe [in/out] This points to the instance of ListenStreamMgr in which session state
                        machines are to be destroyed.
   @param ucSessionId [in]  This is the ID of the session state that are to be destroyed. 
   @return Success or fail         
   */
  ADSPResult lsm_session_destroy_state(lsm_t *pMe, uint8_t ucSessionId);

  /*
   This utility function is used to destory all services on the graph. 
   It should be called only after the services are disconnected to avoid
   race conditions. 

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session in which the graph needs to be destroyed. 
   @return Success or Failure                        
   */
  ADSPResult  lsm_session_destroy(lsm_t *pMe,
                                  lsm_session_state_type_t *pSession,
                                  uint8_t stream_id);



  /*This function is used to store the cleint address, which will be used to send the
events from dynamic service										  

@param pMe[in] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
    @param pSession [in]  This points to the session .
    @param pCbData [out]   Call abck data
    @return Success or Failure   */
  ADSPResult lsm_session_fill_cb_data_for_dynamic_svc(lsm_t *pMe,
                                                      lsm_session_state_type_t *pSession,
                                                      lsm_callback_handle_t *pCbData);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LISTEN_STREAM_MGR_SESSION_H

