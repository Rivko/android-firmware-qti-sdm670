/**
@file lsm_type.h
@brief This file declares some common data types used by ListenStreamMgr.

 */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_Type.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_TYPE_H
#define LISTEN_STREAM_MGR_TYPE_H


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Listen
#include "Elite.h"

#include "adsp_lsm_api.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
  /*Max number of channel bits.*/
#define LISTEN_STREAM_MGR_MAX_NUM_CHANNEL_BITS      32


  /* This is the state of each session */
  enum lsm_session_state_t
  {
    /* Stable State*/
    LISTEN_STREAM_MGR_SESSION_DEINIT = 0,    /**< In close state, the session is no longer active
                                             and is ready to be OPEN again. */
    LISTEN_STREAM_MGR_SESSION_INIT ,      /**< Session is in INIT state. No data processing happens
                                              in this state. */
    LISTEN_STREAM_MGR_SESSION_ACTIVE ,    /**< Svcs in the session is running and processing data*/

  };

#define LISTEN_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS  16



  /*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/



  /* Utility fuction to log the header of msg when error happens. */
  static inline void lsm_print_error_msg(elite_msg_any_t *pMsg)
  {
    elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t*) pMsg->pPayload;
    MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ListenStreamMgr: Fail Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
          pMsg->unOpCode, (uint32_t) (pPayload),
          (uint32_t) (pPayload->pBufferReturnQ), (uint32_t) (pPayload->pResponseQ),
          pPayload->unClientToken,pPayload->unResponseResult);
  }


  /* This structure represent the dynamic service under LSM control. */
  struct dyn_svc_t {
    elite_svc_handle_t   *handle; /**< service handle also have service ID*/
    qurt_elite_thread_t  thread_id; /**< Need to save service thread ID so that LSM can
                                       thread join the destroyed dynamic service thread.*/
  };


  /*
The is the session state structure. It contains the necessary information
related with control a session
   */
  /** @brief This is the session state structure. It contains the necessary
information related with control a session */
  typedef struct
  {
    dyn_svc_t                   dyn_svc; /**< The dynamic services associated with this session.*/

    lsm_session_state_t  	      session_state; /**< State of this session.  */

    uint8_t                     ucInternalSessionId;/**< which group is this session. This is internal
                                             session ID starting from 0,
                                             which is different from ISOD session ID
                                             starting from 1.
                                             A utility function
                                             MAP_INTERNAL_SESSION_ID_TO_EXTERNAL can be
                                             used to map internal v.s. external session ID.*/


  } lsm_session_state_type_t;


  /*
This is the ListenStreamMgr structure.
   */
  typedef struct
  {


    elite_svc_handle_t serviceHandle;       /**< handle to give out to others.
                                               The command queueue of this handle
                                               is the system queue that
                                               1. receives open command from APPS Proc.
                                               2. recevies Elite system level message
                                               such as start_svc, destroy_svc.
     */

    elite_msg_any_t    msg;                 // Message from system cmdQ that is under processing

    qurt_elite_channel_t   channel;           //mask for MQ's owned by this obj
    uint32_t         curr_bit_field;      //contains the current channel mask or signals to act upon
    uint32_t         channel_status;        //current signal received
    //Self Command Q buffer aligned to 8 bytest
    QURT_ELITE_ALIGN(char lsmCmdQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(LISTEN_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS)], 8);

    lsm_session_state_type_t  aSessions[LSM_MAX_SESSION_ID];

  }lsm_t;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LISTEN_STREAM_MGR_TYPE_H

