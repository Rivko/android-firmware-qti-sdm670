/**========================================================================
 @file vpm_svc_apr_if.cpp
 
 @brief This file contains APR command disapatcher utilities
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_apr_if.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_svc_apr_data_base.h"
#include "adsp_vcmn_api.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static elite_apr_handle_t vpm_svc_apr_handle = 0;

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

ADSPResult voice_proc_mgr_verify_handle(elite_apr_port_t apr_port)
{
   /* Check whether the handle is in valid range */
   uint8_t session_id  = VPM_GET_SESSION_ID(apr_port);

   if (session_id >= VPM_MAX_NUM_SESSIONS)
   {
      return ADSP_EHANDLE;
   }

   return ADSP_EOK;
}

ADSPResult voice_proc_mgr_set_apr_handle(elite_apr_handle_t handle)
{
   if (0 != vpm_svc_apr_handle)
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "VPM: AprHandle Already Set 0x%lx, 0x%lx",
            (uint32_t)vpm_svc_apr_handle, (uint32_t)handle);

      return ADSP_EFAILED;
   }

   vpm_svc_apr_handle = handle;

   return ADSP_EOK;
}

elite_apr_handle_t voice_proc_mgr_get_apr_handle()
{
   return vpm_svc_apr_handle;
}

ADSPResult voice_proc_mgr_free_apr_pkt(elite_apr_packet_t *pPkt)
{
   return elite_apr_if_free(vpm_svc_apr_handle, pPkt);
}

ADSPResult vpm_fwd_apr_cmd_to_dyn_svc(elite_apr_packet_t *p_apr_pkt)
{
   ADSPResult              result = ADSP_EOK;
   uint32_t                session_id;
   elite_msg_any_t         msg_pkt;
   vpm_session_object_t    *session_obj = NULL;
   elite_apr_port_t        port = elite_apr_if_get_dst_port(p_apr_pkt);

   session_id = VPM_GET_SESSION_ID(port);

   /* Validate if the session ID is withing range */
   if (session_id >= VPM_MAX_NUM_SESSIONS)
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Invalid VPM session ID: 0x%lx, apr_port: 0x%lx",
            session_id, (uint32_t)port);

      return ADSP_EBADPARAM;
   }

   /* Check if the session with this ID exists */
   if (NULL == (session_obj = p_vpm_svc_global_obj->sessions[session_id]))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Session with ID: 0x%lx, does not exists, apr_port: 0x%lx",
            session_id, (uint32_t)port);

      return ADSP_EHANDLE;
   }

   /* Check if the dyn svc for this session has been created */
   if (!session_obj->dyn_svc_info.tx)
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Dyn svc for this session(0x%lx), does not exists, apr_port: 0x%lx",
            session_id, (uint32_t)port);

      return ADSP_ENOTREADY;
   }

   /* Compose the APR payload and send it to Tx dyn svc */
   msg_pkt.unOpCode = ELITE_APR_PACKET;
   msg_pkt.pPayload = (void *)p_apr_pkt;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                       (uint64_t *)(&msg_pkt))))
   {

      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Failed to push to queue 0x%p, port: 0x%lx",
            session_obj->dyn_svc_info.tx->cmdQ, (uint32_t)port);
   }

   return result;
}

int32_t voice_proc_mgr_apr_cb_func(elite_apr_packet_t *packet, void *dispatch_data)
{
   int32_t              result = APR_EOK;
   uint32_t             ulOpCode = elite_apr_if_get_opcode(packet);
   elite_apr_port_t     port     = elite_apr_if_get_dst_port(packet);
   elite_msg_any_t      msg;
   qurt_elite_queue_t   *pDestQ = NULL;

   msg.unOpCode = ELITE_APR_PACKET;
   msg.pPayload = (void *)packet;

   MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: Rec cmd 0x%lx at [Addr=0x%lx, Port=0x%lx]",
         ulOpCode, (uint32_t)(elite_apr_if_get_dst_addr(packet)), (uint32_t)(port));

   /* Lock data base */
   voice_proc_mgr_apr_router_lock();


   /* Put different msgs into different queue based on opcode */
   switch (ulOpCode)
   {
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION:
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2:
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V3:
      case VOICE_CMD_SHARED_MEM_MAP_REGIONS:
      case VOICE_CMD_SHARED_MEM_UNMAP_REGIONS:
      case VSS_ISSR_CMD_CLEANUP:
      {
         if (ADSP_FAILED(result = voice_proc_mgr_apr_router_get_svc_cmd_q(&pDestQ)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr: Failed to get VPM svc cmd queue");
         }
         break;
      }
      case VSS_ICOMMON_RSP_GET_PARAM:
      case VSS_ICOMMON_RSP_GET_PARAM_V3:
      case APRV2_IBASIC_RSP_RESULT:
      {
         /* If non-zero port ID is present, it indicates response is for any session.
            If port ID is zero, response is for service */
         if (port)
         {
            if ((ADSP_FAILED(result = voice_proc_mgr_verify_handle(port))) ||
                (ADSP_FAILED(result = voice_proc_mgr_apr_router_get_session_rsp_q(port, &pDestQ))))
            {
               MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "VoiceProcMgr: Failed to get VPM session response queue,"
                     "opcode: 0x%08X, apr_port: 0x%lx, result; 0x%lx",
                     ulOpCode, port, result);
            }
         }
         else /* Port ID is NULL, route response to service response Q */
         {
            if (ADSP_FAILED(result = voice_proc_mgr_apr_router_get_svc_rsp_q(&pDestQ)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr: Failed to get VPM svc rsp queue");
            }
         }

         break;
      }
      case VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION:
      {
         if ((ADSP_FAILED(result = voice_proc_mgr_verify_handle(port))) ||
             (ADSP_FAILED(result = vpm_fwd_apr_cmd_to_dyn_svc(packet))))
         {
            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "VoiceProcMgr: Failed to get fwd cmd to dyn svc,"
                  "opcode: 0x%08X, apr_port: 0x%lx, result; 0x%lx",
                  ulOpCode, port, result);
         }
         break;
      }
      default:
      {
         if ((ADSP_FAILED(result = voice_proc_mgr_verify_handle(port))) ||
             (ADSP_FAILED(result = voice_proc_mgr_apr_router_get_session_cmd_q(port, &pDestQ))))
         {
            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "VoiceProcMgr: Failed to get VPM session cmd queue,"
                  "opcode: 0x%08X, apr_port: 0x%lx, result; 0x%lx",
                  ulOpCode, port, result);
         }
         break;
      }
   } /* End of switch case */

__bail_out_apr_packet:

   if (ADSP_FAILED(result))
   {
      voice_proc_mgr_apr_router_unlock();

      /* Send error message to client. This also frees the packet */
      uint32_t rc = elite_apr_if_end_cmd(vpm_svc_apr_handle, packet, result);

      if (ADSP_FAILED(rc))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "Failed at elite_apr_if_end_cmd(), result: 0x%lx. Ack not sent, apr_port: 0x%lx",
               rc, (uint32_t)port);
      }

      /* Returning fail causes another free in APR. therefore return success */
      return APR_EOK;
   }

   /* Push msg to the detination queue */

   if (NULL != pDestQ)
   {
      if (ADSP_FAILED(result = qurt_elite_queue_push_back(pDestQ, (uint64_t *)(&msg))))
      {

         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "VoiceProcMgr: Failed to push to queue 0x%p, port: 0x%lx", pDestQ, (uint32_t)port);

         /* Send error message to client. This also frees the packet */
         uint32_t rc = elite_apr_if_end_cmd(vpm_svc_apr_handle, packet, result);

         if (ADSP_FAILED(rc))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "Failed at elite_apr_if_end_cmd(), result: 0x%lx. Ack not sent, apr_port: 0x%lx",
                  rc, (uint32_t)port);
         }

         /* Returning fail causes another free in APR. therefore return success */
         result = APR_EOK;
      }
   }

   voice_proc_mgr_apr_router_unlock();

   return result;
}

