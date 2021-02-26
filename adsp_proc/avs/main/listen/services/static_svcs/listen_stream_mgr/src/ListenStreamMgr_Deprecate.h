/**
@file ListenStreamMgr_Deprecate.h

@brief This file contains functionalities that are potential candidates
       for deprecation. They need to be supported as HLOS clients
       have active solution using these older approach. When there is no
       longer clients to below functions, we ought to remove.

*/

/*========================================================================

 $Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_Deprecate.h#1 $

 Edit History

   when       who     what, where, why
  --------    ---       -----------------------------------------------------
 10/29/2015  Unni      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTENSTREAMMGR_DEPRECATE_H_
#define LISTENSTREAMMGR_DEPRECATE_H_

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "ListenStreamMgr_Type.h"

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/

/*
   This function processes  LSM_CMD_ADD_TOPOLOGIES command that is sent to LSM
   service.
   (Alternatively, the newer approach is using AVCS_CMD_REGISTER_TOPOLOGIES)

   @param me_ptr[in/out] This points to the instance of ListenStreamMgr.

   @param pkt_ptr[in/out] This points to the elite_apr_packet that needs to be
                          processed.

   @return Success or Failure
*/
ADSPResult  lsm_service_cmd_q_add_topologies_handler(lsm_t *me_ptr,
                                                    elite_apr_packet_t *pkt_ptr);

/*
   This function process LSM_SESSION_CMD_OPEN_TX(v1) command that are send to a
   given session.
   (Alternatively, the newer approach is using LSM_SESSION_CMD_OPEN_TX_V2)

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The
                      channel bit in this instance that associated with the
                      given session might be turned on/off depending on command
                      processing.
   @param pSession[in/out] This points to the session that are to process the
                           client command.
   @return Success or Failure
 */
ADSPResult lsm_session_cmd_q_open_tx_v1_stream_handler(lsm_t *pMe,
                                                       elite_apr_packet_t *apr_pkt_ptr);

/*
   This function process LSM_SESSION_CMD_GET_PARAMS(v1) command that are send to a
   given session.
   (Alternatively, the newer approach is using LSM_SESSION_CMD_GET_PARAMS_V2)

  @param[in] pPkt         APR command that demands an acknowledgment. The
                          function assumes the APR packet is allocated by the
                          APR, and it frees this APR packet.
  @param[in] status       Status of the command.
  @param[in] pAckPayload  Pointer to the acknowledgment payload.
                          Some commands require an acknowledgment other than
                          APR_IBASIC_RSP_RESULT. In those cases, the caller is
                          responsible for assembling an acknowledgment
                          payload.\n
                          If the pointer is NULL, APR_IBASIC_RSP_RESULT is used.
  @param[in] ulSize       Size of pAckPayload.

   @return Success or Failure
 */
ADSPResult lsm_generate_ack_get_param_v1(elite_apr_packet_t* pPkt,
                                         ADSPResult status,
                                         void *pAckPayload,
                                         uint32_t ulSize);

#endif /* LISTENSTREAMMGR_DEPRECATE_H_ */
