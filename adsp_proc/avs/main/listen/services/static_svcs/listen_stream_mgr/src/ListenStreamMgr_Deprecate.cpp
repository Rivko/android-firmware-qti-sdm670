/**
@file ListenStreamMgr_Deprecate.cpp

@brief This file contains functionalities that are potential candidates
       for deprecation. They need to be supported as HLOS clients
       have active solution using these older approach. When there is no
       longer clients to below functions, we ought to remove.


*/

/*========================================================================

 $Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_Deprecate.cpp#1 $

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

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "ListenStreamMgr_Deprecate.h"
#include "ListenStreamMgr_Type.h"
#include "ListenStreamMgr_AprIf.h"
#include "EliteMem_Util.h"
#include "adsp_lsm_service_commands.h"
#include "EliteTopology_db_if.h"
#include "EliteMem_Util.h"
#include "ListenStreamMgr_SessionCmdHandler.h"
#include "adsp_lsm_service_modules_params.h"

/*----------------------------------------------------------------------------
 * Constant / Define Declarations
 *--------------------------------------------------------------------------*/
extern uint32_t lsm_memory_map_client;

/*
   This function processes  LSM_CMD_ADD_TOPOLOGIES command that is sent to LSM
   service.
   (Alternatively, the newer approach is using AVCS_CMD_REGISTER_TOPOLOGIES)

   @param me_ptr[in/out] This points to the instance of ListenStreamMgr.

   @param pkt_ptr[in/out] This points to the elite_apr_packet that needs to be
                          processed.

   @return Success or Failure
*/
ADSPResult lsm_service_cmd_q_add_topologies_handler(lsm_t *me_ptr,
                                                    elite_apr_packet_t *pkt_ptr)
{
  ADSPResult result = ADSP_EOK;
  lsm_cmd_add_topologies_t *palyoad_ptr = (lsm_cmd_add_topologies_t*)
                                          elite_apr_if_get_payload_ptr(pkt_ptr);

  /* Map the buffer */
  uint32_t phy_addr_lsw = (uint32_t)(palyoad_ptr->data_payload_addr_lsw);
  uint32_t phy_addr_msw = (uint32_t)(palyoad_ptr->data_payload_addr_msw);

  static const uint32_t CACHE_LINE_SIZE = 32; /* Must be a power of 2  */
  static const uint32_t CACHE_ALIGNMENT_MASK = (CACHE_LINE_SIZE - 1);
  bool_t is_aligned_cache_line = ((phy_addr_lsw & CACHE_ALIGNMENT_MASK) == 0);
  audproc_custom_topologies_t *header_ptr = NULL;
  uint8_t *virt_addr_ptr = NULL;
  uint32_t buffer_size = 0;
  uint32_t size_required = 0;

  if(!is_aligned_cache_line)
  {
    result = ADSP_EBADPARAM;
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "LSM: Physical address is not aligned to cache line");
    goto done;
  }



  elite_mem_shared_memory_map_t buffer_mem_node;
  buffer_mem_node.unMemMapHandle = palyoad_ptr->mem_map_handle;
  buffer_mem_node.unMemMapClient = lsm_memory_map_client;
  result =  elite_mem_map_get_shm_attrib(phy_addr_lsw,
                                         phy_addr_msw,
                                         palyoad_ptr->buffer_size,
                                         &buffer_mem_node);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM: Failed to map physical memory");
    goto done;
  }

  /* Since this buffer will be read, need to invalidate the cache. */
  result = elite_mem_invalidate_cache(&buffer_mem_node);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LSM: Failed to invalidate memory, result[0x%x]",
          result);
    goto done;
  }

  virt_addr_ptr = (uint8_t*)(buffer_mem_node.unVirtAddr);
  buffer_size = palyoad_ptr->buffer_size;

  if(buffer_size < sizeof(audproc_custom_topologies_t))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LSM: Size of custom topologies buffer %lu bytes is insufficient",
          buffer_size);
    result = ADSP_ENEEDMORE;
    goto done;
  }

  header_ptr = (audproc_custom_topologies_t*)(virt_addr_ptr);
  virt_addr_ptr += sizeof(audproc_custom_topologies_t);
  buffer_size -= sizeof(audproc_custom_topologies_t);
  size_required = (header_ptr->num_topologies *
                   sizeof(audproc_topology_definition_t));

  if(buffer_size < size_required)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LSM: Size of custom topologies array %lu bytes is insufficient,"
          " %lu bytes needed",
          buffer_size, size_required);
    result = ADSP_ENEEDMORE;
    goto done;
  }

  result = topo_db_add_topologies(TOPO_DB_CLIENT_TYPE_LSM,
                                  header_ptr->num_topologies,
                           (const audproc_topology_definition_t*)virt_addr_ptr);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LSM: Failed to insert custom topologies for LSM into the database,"
          "result[0x%x]",
          result);
    goto done;
  }

done:
  lsm_generate_ack(pkt_ptr, result, 0, 0, 0);
  return result;
}

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
                                                       elite_apr_packet_t *apr_pkt_ptr)
{
  lsm_session_cmd_open_tx_v2_t *cmd_open_tx_ptr = NULL;
  elite_apr_if_get_payload( (void **)&cmd_open_tx_ptr, apr_pkt_ptr );

  /* Only one pre-defined topology is supported in old open command */
  cmd_open_tx_ptr->topology_id = LSM_TX_TOPOLOGY_ID_VOICE_WAKEUP_V2;

  return lsm_session_cmd_q_open_tx_stream_handler(pMe, apr_pkt_ptr);
}

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
                                         uint32_t ulSize)
{
  ADSPResult result = ADSP_EFAILED;
  uint32_t opCode = elite_apr_if_get_opcode(pPkt);

  switch (opCode)
  {
    case LSM_SESSION_CMDRSP_GET_PARAMS:
    {
      /*For in-band GET_PARAM, LSM has already allocated APR packet with necessary fields*/
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Sending LSM_SESSION_CMDRSP_GET_PARAMS for LSM:Port=0x%x: status 0x%lx, token %lu, Size %lu",
            pPkt->dst_port,(uint32_t) status, pPkt->token, ulSize);
      result = elite_apr_if_async_send( lsm_apr_handle, pPkt );
      break;
    }
    /* Deprecated */
    case LSM_SESSION_CMD_GET_PARAMS:
    {
      uint32_t response_opcode = LSM_SESSION_CMDRSP_GET_PARAMS;

      /* For out-of-band GET_PARAM command need to allocate response packet*/
      result = elite_apr_if_alloc_send_ack(lsm_get_apr_handle(),
                                           pPkt->dst_addr,   /// Switch src/dst address field in ack
                                           pPkt->dst_port,
                                           pPkt->src_addr,
                                           pPkt->src_port,
                                           pPkt->token,
                                           response_opcode, ///opcode,
                                           pAckPayload,
                                           ulSize );
      elite_apr_if_free(lsm_apr_handle, pPkt);
      break;
    }
  }

  return result;
}
