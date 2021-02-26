#ifndef _VOICE_STREAM_MGR_H_
#define _VOICE_STREAM_MGR_H_
/*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/inc/VoiceStreamMgr.h#1 $
  $Author: pwbldsvc $
*/

#include "vsm_header_i.h"

/* Maximum number of concurrent voice sessions that can be supported. */
#define VSM_MAX_SUPPORTED_SESSION_ID ( 4 )

#define VOICE_STREAM_MGR_MAX_SVC_CMD_Q_ELEMENTS  32
#define VOICE_STREAM_MGR_MAX_SVC_RESP_Q_ELEMENTS  16

typedef struct voice_stream_mgr_t
{
   elite_svc_handle_t       service_handle;
   /**< Elite Service handle shared with clients.  */

   qurt_elite_queue_t*      respQ;

   elite_msg_any_t          msg;
   /**< Message from system cmdQ that is under processing. */

   vsm_cmd_ctrl_t cmd_control;
   /**< Contains command control information. */

   qurt_elite_channel_t     channel;
   /**< Channel associated with VoiceStreamManager Service. */

   uint32_t                 wait_mask;
   /**< Contains the current channel mask for signals to wait upon */

   uint32_t                 recvd_mask;       
   /**< Current signal received */

   uint32_t                 pending_mask;
   /**< Contains the mask for pending command queues */

   QURT_ELITE_ALIGN(char cmdQ_buf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VOICE_STREAM_MGR_MAX_SVC_CMD_Q_ELEMENTS)], 8);
   /**< Service Command Q buffer aligned to 8 bytes. */

   QURT_ELITE_ALIGN(char respQ_buf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VOICE_STREAM_MGR_MAX_SVC_RESP_Q_ELEMENTS)], 8);
   /**< Response queue for the service. */

   vsm_session_static_info_t static_info[VSM_MAX_SUPPORTED_SESSION_ID];
   /**< Contains static command queue mapping. */

   vsm_session_object_t *session_table[VSM_MAX_SUPPORTED_SESSION_ID];
   /**< Array of session object pointers. */

   bool_t vsm_csm_vote;
   /**< CSM vote status for VSM.  */
}
  voice_stream_mgr_t;

/* Thisis required temporarily till all other services migrate to above type.*/
typedef voice_stream_mgr_t VoiceStrmMgr_t;

  /* VoiceStreamMgr SysCmdQ handler type*/
typedef int ( *VSM_msg_handler_fn ) ( voice_stream_mgr_t* pMe );

/* VoiceStreamMgr session cmd queue handler type*/
typedef int ( *VSM_session_cmdQ_handler_fn ) ( vsm_session_object_t* );

/* VoiceStreamMgr session response queue handler type*/
typedef int ( *VSM_session_respQ_handler_fn ) ( vsm_session_object_t* );
  
ADSPResult VoiceStreamMgr_Create (uint32_t inputParam, void **handle);

#endif // _VOICE_STREAM_MGR_H_