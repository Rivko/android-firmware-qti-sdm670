/**
@file ListenProcSvc_Deprecate.cpp

@brief This file contains functionalities that are potential candidates
       for deprecation. They need to be supported as HLOS clients
       have active solution using these older approach. When there is no
       longer clients to below functions, we ought to remove.


*/

/*========================================================================

 $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_Deprecate.cpp#1 $

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
#include "ListenProcSvc_Deprecate.h"
#include "EliteMem_Util.h"
#include "topo_utils.h"
#include "ListenStreamMgr_AprIf.h"
#include "EliteTopology_db_if.h"

/* we can use the MASK_32BYTE_ALIGNMENT, but for targets where cache line is changing this is useful*/
#define  LSM_CACHE_LINE_SIZE  32 // Must be a power of 2
#define  LSM_CACHE_ALIGNMENT_MASK  (LSM_CACHE_LINE_SIZE - 1)

static inline bool_t check_cache_line_alignment(uint32_t addr)
{
  return ((addr & LSM_CACHE_ALIGNMENT_MASK) == 0);
}

static inline uint8_t LSM_GET_SESSION_ID(elite_apr_port_t port)
{
  return (uint8_t) (port & 0x00FF );
}

/*
   This function processes  LSM_SESSION_CMD_SET_PARAM command that is sent to a given session.
   (Alternatively, use LSM_SESSION_CMD_SET_PARAM_V2)

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
ADSPResult listen_proc_svc_set_param_v1_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK;
  elite_apr_packet_t *pPkt  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  lsm_session_cmd_set_params_t *pParamPayload;
  pParamPayload = (lsm_session_cmd_set_params_t *)elite_apr_if_get_payload_ptr(pPkt);
  int32_t *paramBase = NULL;
  elite_apr_port_t port = elite_apr_if_get_dst_port( pPkt );
  uint8_t session_id   = LSM_GET_SESSION_ID( port );

  if (0 == pParamPayload->mem_map_handle) /* in-band */
  {
    /* Address just beyond the Set params header lsm_session_cmd_set_params_t points to lsm_session_param_data_t */
    paramBase  = (int32_t *)(pParamPayload + 1);
  }
  else /* out-of-band */
  {
    if (!check_cache_line_alignment(pParamPayload->data_payload_addr_lsw))
    {
      result = ADSP_EBADPARAM;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM [%u]: Physical memory address is not aligned to cache line, in set param.",session_id);
      return lsm_generate_ack(pPkt, result, NULL, 0, 0 );
    }
    else
    {
      elite_mem_shared_memory_map_t bufferMemNode;
      bufferMemNode.unMemMapClient     = lsm_memory_map_client;
      bufferMemNode.unMemMapHandle     = pParamPayload->mem_map_handle;
      bufferMemNode.unMemSize          = pParamPayload->data_payload_size;
      bufferMemNode.unPhysAddrLsw      = pParamPayload->data_payload_addr_lsw;
      bufferMemNode.unPhysAddrMsw      = pParamPayload->data_payload_addr_msw;
      bufferMemNode.unVirtAddr         = 0;

      result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
                                             bufferMemNode.unMemSize, &bufferMemNode);
      if (ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM [%u,]:Failed to map physical memory for SetParam.",  session_id);
        return lsm_generate_ack(pPkt, result, NULL, 0, 0 );
      }

      /* Since this buffer will be read, need to invalidate the cache. */
      result = elite_mem_invalidate_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM [%u]: Failed to invalidate cache for SetParam.",session_id);
        return lsm_generate_ack(pPkt, result, NULL, 0, 0 );
      }
      paramBase  = (int32_t*)(bufferMemNode.unVirtAddr);
    }
  }

  lsm_session_param_data_t* set_param_v1_iter =
      (lsm_session_param_data_t*)(paramBase);

  /* Reserved field Zero Check for Set_Param V1 */
  while((int8_t*)set_param_v1_iter <
      (int8_t*)paramBase + pParamPayload->data_payload_size)
  {
    if(set_param_v1_iter->reserved != 0)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Non-zero reserved filed in LSM_SESSION_CMD_SET_PARAMS %x",
            set_param_v1_iter->reserved );
      result = ADSP_EBADPARAM;
      return lsm_generate_ack(pPkt, result, NULL, 0, 0 );
    }
    set_param_v1_iter = (lsm_session_param_data_t*)(
        (int8_t*)set_param_v1_iter +
        sizeof(lsm_session_param_data_t) +
        set_param_v1_iter->param_size);
  }

  return listen_proc_svc_set_param_handler(me_ptr);
}

/**
  Iterate through parameter list and set each parameter

  @param me_ptr [in/out]: This points to the instance of listen_proc_svc_t
  @param payload_address [in/out]: Pointer to list of parameters
  @param payload_size [in]: size of the entire list
  @param opcode [in]: command opcode

  @return
  Success/failure

  @dependencies
  None.
 */
extern ADSPResult listen_proc_iter_set_get_param(listen_proc_svc_t *me_ptr,
                                                 int8_t *payload_address,
                                                 uint32_t payload_size,
                                                 bool_t set_flag, uint32_t opcode);
/*
   This function processes  LSM_SESSION_CMD_GET_PARAM command that is sent to a given session.
   (Alternatively, use LSM_SESSION_CMD_GET_PARAM_V2)

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
ADSPResult listen_proc_svc_get_param_v1_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK,ack_result = ADSP_EOK;
  elite_apr_packet_t *apr_pkt_ptr  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  uint8_t session_id  = me_ptr->session_id;
  int8_t *calib_data_ptr;
  lsm_session_cmd_get_params_t* param_payload_ptr =
      (lsm_session_cmd_get_params_t *)elite_apr_if_get_payload_ptr(apr_pkt_ptr);
  /* If no mem_map_handle, then in_band , else out-band */
  bool_t in_band = FALSE;
  uint32_t opcode = 0;
  uint32_t cmd_opcode = elite_apr_if_get_opcode(apr_pkt_ptr);
  elite_mem_shared_memory_map_t bufferMemNode;
  uint32_t data_payload_size;
  lsm_session_param_data_t *param_header_ptr = NULL;

  data_payload_size = param_payload_ptr->param_max_size;

  in_band = (0 == param_payload_ptr->mem_map_handle)? TRUE : FALSE;

  if(TRUE == in_band) /* in-band */
  {
    elite_apr_packet_t *temp_apr_pkt_ptr = NULL;
    /* This service needs to allocate large enough packet for the response*/
    uint32_t ack_payload_size = sizeof(lsm_session_cmdrsp_get_params_t) + param_payload_ptr->param_max_size;
    int8_t* apr_packet_base_ptr;
    opcode = LSM_SESSION_CMDRSP_GET_PARAMS;

    /*Allocate the APR packet*/
    result = elite_apr_if_alloc_cmd_rsp(lsm_get_apr_handle(),
                                        elite_apr_if_get_dst_addr(apr_pkt_ptr),
                                        elite_apr_if_get_dst_port(apr_pkt_ptr),
                                        elite_apr_if_get_src_addr(apr_pkt_ptr),
                                        elite_apr_if_get_src_port(apr_pkt_ptr),
                                        elite_apr_if_get_client_token(apr_pkt_ptr),
                                        opcode,
                                        ack_payload_size,
                                        &temp_apr_pkt_ptr);

    if(NULL == temp_apr_pkt_ptr)
    {
      result = ADSP_EFAILED;
    }

    if(ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:Alloc rsp GetParam:0x%x",
            session_id, result);
      goto __bailout;
    }

    /*assign the current message to the allocate response packet*/
    me_ptr->cmd_msg.pPayload = temp_apr_pkt_ptr;
    apr_packet_base_ptr = (int8_t *)elite_apr_if_get_payload_ptr(temp_apr_pkt_ptr);
    /*assign the param data ptr of the internal message to the correct offset in APR packet*/
    calib_data_ptr = apr_packet_base_ptr + sizeof(uint32_t); /*Offset by the status to access the module ID.*/
    /*Free up the incoming APR packet for in-band since we have created another for the response*/
    elite_apr_if_free(lsm_get_apr_handle(), apr_pkt_ptr);
    apr_pkt_ptr = temp_apr_pkt_ptr;

  } /* in-band */
  else /* Out-band */
  {
    if (!check_cache_line_alignment(param_payload_ptr->data_payload_addr_lsw))
    {
      result = ADSP_EBADPARAM;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem unaligned",
            session_id, result);

      goto __bailout;
    }

    bufferMemNode.unMemMapClient     = lsm_memory_map_client;
    bufferMemNode.unMemMapHandle     = param_payload_ptr->mem_map_handle;
    bufferMemNode.unMemSize          = data_payload_size;
    bufferMemNode.unPhysAddrLsw      = param_payload_ptr->data_payload_addr_lsw;
    bufferMemNode.unPhysAddrMsw      = param_payload_ptr->data_payload_addr_msw;
    bufferMemNode.unVirtAddr         = 0;

    result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw,
                                           bufferMemNode.unPhysAddrMsw,
                                           bufferMemNode.unMemSize,
                                           &bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem map:0x%x",
            session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }

    /* Since this buffer will be read, need to invalidate the cache. */
    result = elite_mem_invalidate_cache(&bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem invalidate cache:0x%x",
            session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }
    calib_data_ptr  = (int8_t*)(bufferMemNode.unVirtAddr);

  }/* Out-band */


  /*initialize the response payload to 0 for consistency*/
  memset(calib_data_ptr, 0, param_payload_ptr->param_max_size);

  /*populate the param data header*/
  param_header_ptr = (lsm_session_param_data_t *)calib_data_ptr;
  param_header_ptr->module_id = param_payload_ptr->module_id;
  param_header_ptr->param_id = param_payload_ptr->param_id;
  /*initalize this to the max size for the actual payload*/
  param_header_ptr->param_size = param_payload_ptr->param_max_size - sizeof(lsm_session_param_data_t);

  /* Execute get param */
  result = listen_proc_iter_set_get_param(me_ptr,calib_data_ptr, data_payload_size, FALSE, cmd_opcode);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "LSM[%u]:iter params:0x%x", result);
  }

  /* Get param response handling */
  /*Generate ACK. For in-band, apr_packet_ptr is created above */
  if(LSM_SESSION_CMDRSP_GET_PARAMS == opcode)
  {
    lsm_session_cmdrsp_get_params_t *ret_payload_ptr =
        (lsm_session_cmdrsp_get_params_t*)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    ret_payload_ptr->status = result;

  }
  /*For out-of-band apr_packet_ptr is created by client*/
  else
  {
    /*Flush the cache*/
    result = elite_mem_flush_cache(&bufferMemNode);

    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]: Out-band Getparam flush cache fail",
            session_id );

    }
    /*send the ACK*/
    ack_result = lsm_generate_ack(apr_pkt_ptr, result,&result, sizeof(result), 0 );
    result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
    return result;
  }

__bailout:
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, 0, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;

}

/**
  Function which process the sound model register command
  (Alternatively, use SET_PARAM with LSM_PARAM_ID_REGISTER_SOUND_MODEL)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_register_sound_model_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK,ack_result = ADSP_EOK;
  uint32_t iter = 0;
  topo_t *topo_ptr = (topo_t*)me_ptr->topo_handle;
  elite_apr_packet_t *apr_pkt_ptr  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  lsm_session_cmd_register_sound_model_t *payload_ptr =
     (lsm_session_cmd_register_sound_model_t *)elite_apr_if_get_payload_ptr(apr_pkt_ptr);

  /*Accept this command only when it is STOP state*/
  if(me_ptr->state != LISTEN_PROC_STATE_STOP)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LProSvc[%d] must be in stop state, current state::%d,",
          me_ptr->session_id, me_ptr->state );
    result = ADSP_EUNSUPPORTED;
    goto __bailout;
  }

  me_ptr->model_params.unMemMapHandle = payload_ptr->mem_map_handle;
  me_ptr->model_params.unMemMapClient = lsm_memory_map_client;

  /* Convert Physical address of the shared memory buffer to virtual address */
  result = elite_mem_map_get_shm_attrib(payload_ptr->model_addr_lsw,
                                        payload_ptr->model_addr_msw,
                                        payload_ptr->model_size,
                                        &(me_ptr->model_params));
  if(ADSP_FAILED(result))
  {
    MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LProSvc[%d]:Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)",
          me_ptr->session_id,
          me_ptr->model_params.unPhysAddrMsw,
          me_ptr->model_params.unPhysAddrLsw,
          me_ptr->model_params.unVirtAddr);
    result = ADSP_EFAILED;
    goto __bailout;
  }

  /* Invalidate cache */
  result = elite_mem_invalidate_cache(&me_ptr->model_params);
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LProSvc[%d] Failed to invalidate cache for Soundmodel %d",
          me_ptr->session_id, result);
    result = ADSP_EFAILED;
    goto __bailout;
  }

  if(0 == topo_ptr->num_modules)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "LProSvc[%d]:No Module present to register Sound Model",
        me_ptr->session_id);
        result = ADSP_EFAILED;
        goto __bailout;
  }

  for(iter = 0; iter <  topo_ptr->num_modules ; iter++)
  {
    result = topo_set_param(me_ptr->topo_handle,
                            topo_ptr->module[iter].module_id,
                            topo_ptr->module[iter].instance_id,
                            LSM_PARAM_ID_REGISTER_SOUND_MODEL,
                            payload_ptr->model_size,
                            (int8_t*)me_ptr->model_params.unVirtAddr,
                            NULL);
    if(ADSP_FAILED(result))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LProSvc[%d] Failed in processing register snd model with module[0x%lx]::%lx",
            me_ptr->session_id, topo_ptr->module[iter].module_id, result);
      continue;
    }

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "LProSvc[%d] Successfully registerd snd model with module[0x%x]",
          me_ptr->session_id, topo_ptr->module[iter].module_id);
    break;
  }

__bailout:
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, 0, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;
}

/**
  Function which process the sound model deregister command
  (Alternatively, use SET_PARAM with LSM_PARAM_ID_DEREGISTER_SOUND_MODEL)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult  listen_proc_svc_deregister_sound_model_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK,ack_result = ADSP_EOK;
  uint32_t iter = 0;
  elite_apr_packet_t *apr_pkt_ptr  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  topo_t *topo_ptr = (topo_t*)me_ptr->topo_handle;


  /* Accept this command only when it is STOP state */
  if(me_ptr->state != LISTEN_PROC_STATE_STOP)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LProSvc[%d] must be in stop state, current state::%d",
          me_ptr->session_id, me_ptr->state );
    result = ADSP_EUNSUPPORTED;
    goto __bailout;
  }

  if(0 == topo_ptr->num_modules)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "LProSvc[%d]:No Module present to deregister Sound Model",
        me_ptr->session_id);
    result = ADSP_EFAILED;
    goto __bailout;
  }

  for(iter = 0; iter <  topo_ptr->num_modules ; iter++)
  {
    result = topo_set_param(me_ptr->topo_handle,
                            topo_ptr->module[iter].module_id,
                            topo_ptr->module[iter].instance_id,
                            LSM_PARAM_ID_DEREGISTER_SOUND_MODEL,
                            0, NULL,  /* NULL Payload */
                            NULL); /* NULL Callback */
    if(ADSP_FAILED(result))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LProSvc[%d] Failed in processing de-register snd model with module[0x%lx]::%lx",
            me_ptr->session_id, topo_ptr->module[iter].module_id, result);
      continue;
    }
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LProSvc[%d] Successfully deregisterd snd model with module[0x%lx]",
          me_ptr->session_id, topo_ptr->module[iter].module_id);
    break;
  }

__bailout:
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, 0, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;
}

/**
  Function which process the re-mapping LSM_MODULE_VOICE_WAKEUP
  (Alternatively, use SET_PARAM with LSM_MODULE_VOICE_WAKEUP_V2)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @param param_id [in] Param id associated with the module

  @return appropriate module id

  @dependencies
  None.
 */
uint32_t listen_proc_remap_deprecated_module_id(listen_proc_svc_t* me_ptr,
                                                uint32_t param_id)
{
  uint32_t module_id;

  if(LSM_PARAM_ID_CONNECT_TO_PORT == param_id)
  {
    /* This PARAM id is now handled by new module reserved by framework*/
    module_id = LSM_MODULE_FRAMEWORK;
  }
  else
  {
    /* Only valid module id in app_id approach */
    module_id = ((topo_t*)(me_ptr->topo_handle))->module[0].module_id;
  }
  return module_id;
}

/**
  Function which retrieves topology from old database
  (Alternatively, use AVCS_CMD_REGISTER_TOPOLOGIES)

  @param me_ptr [in] Topology id to be instantiated
  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult  listen_proc_svc_get_topo_old_db(uint32_t topology_id,
                                            listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  const lsm_topology_definition_t *lsm_topo_def_ptr = NULL;

  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ListenProc[%d]: Topology %lx is not present in "
        "common database, searching in old database", me_ptr->session_id, topology_id);

  /* Get topology information based on id */
  result = topo_db_get_topology(TOPO_DB_CLIENT_TYPE_LSM,
                                topology_id,
                                (const audproc_topology_definition_t**)&lsm_topo_def_ptr);
  if(ADSP_FAILED(result) || NULL == lsm_topo_def_ptr)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "ListenProc[%d] failed to get topology definition for topology id %lx, status[%x]!!",
          me_ptr->session_id,topology_id, result);
    return ADSP_EFAILED;
  }

  /* Create topology */
  result = topo_create(&me_ptr->topo_handle,
                       (topology_definition_t*)lsm_topo_def_ptr,
                       lsm_capi_v2_event_callback,
                       me_ptr,
                       NULL);
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "ListenProc[%d] failed to create topology for topology id %lx!!",
          me_ptr->session_id,topology_id);
    return result;
  }

  return ADSP_EOK;
}
