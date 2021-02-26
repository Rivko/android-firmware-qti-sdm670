/**
@file ListenStreamMgr_SessionCmdHandler.h
@brief This file declares session cmd handler functions for ListenStreamMgr.

 */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_SessionCmdHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir      Initial version
==========================================================================*/
/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_SESSION_CMD_HANDLER_H
#define LISTEN_STREAM_MGR_SESSION_CMD_HANDLER_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

// System
#include "qurt_elite.h"
#include "Elite.h"

// Listen
#include "ListenStreamMgr_Type.h"



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

  /*
   This function process the client command that are send to a given LSM session and service.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
   */
  ADSPResult lsm_svc_and_session_apr_pkt_handler( lsm_t *pMe);


  /*
   This function process LSM_SESSION_CMD_OPEN_TX command that are send to a given session.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
   */
  ADSPResult lsm_session_cmd_q_open_tx_stream_handler(lsm_t* pMe, elite_apr_packet_t *pkt_ptr);


  /*
   This function process   LSM_SESSION_CMD_CLOSE command 

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
   */
  ADSPResult  lsm_session_cmd_q_close_tx_stream_handler(lsm_t *pMe, elite_apr_packet_t *pkt_ptr);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LISTEN_STREAM_MGR_SESSION_CMD_HANDLER_H

