/**========================================================================
 @file vpm_svc_apr_data_base.cpp
 
 @brief This file defines functions, variables for managing APR routing
 table. 
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_apr_data_base.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "vpm_svc_apr_data_base.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* Look up table for APR packet router */
static struct
{
    vpm_rnode_t                        apr_router_lut[VPM_MAX_NUM_SESSIONS];
    /**< at IPC level, Session ID 1- CVP_MAX_NUM_SESSIONS .
         For easy of implementation, add empty space ahead.
         */
    qurt_elite_mutex_t                 apr_router_lock;
    /**< To avoid hazard of multiple write from different thread to the data base.
         Lock of AudioStreamMgr_AprRouterLUT for writing.
         */
    qurt_elite_queue_t                 *vpm_svc_cmd_q;
    /**< Service command queue */

    qurt_elite_queue_t                 *vpm_svc_rsp_q;
    /**< Service response queue */
} vpm_apr_data_base;


typedef enum vpm_session_queue_t
{
   CMD_QUEUE = 0,
   RSP_QUEUE = 1
}vpm_session_queue_t;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
void  voice_proc_mgr_apr_router_lock()
{
    return qurt_elite_mutex_lock( &(vpm_apr_data_base.apr_router_lock) );
}

void  voice_proc_mgr_apr_router_unlock()
{
    return qurt_elite_mutex_unlock( &(vpm_apr_data_base.apr_router_lock) );
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/


/*
   This is the function to register the command queue with the APR router.
*/
 ADSPResult vpm_apr_router_reg_queue(uint16_t session_idx,
                                     qurt_elite_queue_t *p_session_q,
                                     vpm_session_queue_t q_type)
{
    vpm_rnode_t *pNode;

    if (  session_idx > VPM_MAX_NUM_SESSIONS || NULL == p_session_q)
    {
        MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "Failed to register cmdQ, invalid session_id: %lu and/or cmd_q: 0xlx",
              session_idx, (uint32_t) p_session_q);

        return ADSP_EBADPARAM;
    }

    pNode = &(vpm_apr_data_base.apr_router_lut[session_idx]);

    if ( CMD_QUEUE == q_type )
    {
       pNode->cmdQ = p_session_q;
    }
    else /* Response Q */
    {
       pNode->rspQ = p_session_q;
    }

    return ADSP_EOK;
}


/*
   This function is called after VP< is created. It fills in the
   command queue of different sessions.
   Given that the client is generating handle, we already know the command queue
   of each session in creation time. Hence, fill it here.
*/
ADSPResult voice_proc_mgr_apr_router_init(voice_proc_mgr_t *pMe)
{
   uint32_t session_idx;

    /* Init Lookup Table */
    memset( vpm_apr_data_base.apr_router_lut, 0,
            sizeof(vpm_apr_data_base.apr_router_lut) );

    qurt_elite_mutex_init( &(vpm_apr_data_base.apr_router_lock) );

    for ( session_idx = 0; session_idx < VPM_MAX_NUM_SESSIONS; session_idx++ )
    {
       if (ADSP_FAILED(vpm_apr_router_reg_queue(session_idx,
                                                pMe->session_static_info[session_idx].p_session_cmd_q,
                                                CMD_QUEUE)))
       {
          MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "VoiceProcMgr: Error in router init, session_idx: %lu", session_idx);

          return ADSP_ENOTREADY;
       }

       if (ADSP_FAILED(vpm_apr_router_reg_queue(session_idx,
                                                pMe->session_static_info[session_idx].p_session_resp_Q,
                                                RSP_QUEUE)))
       {
          MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "VoiceProcMgr: Error in router init, session_idx: %lu", session_idx);

          return ADSP_ENOTREADY;
       }
    }

    /* Register service command queue */
    vpm_apr_data_base.vpm_svc_cmd_q = pMe->svc_handle.cmdQ;

    /* Register service response queue */
    vpm_apr_data_base.vpm_svc_rsp_q = (qurt_elite_queue_t *)pMe->svc_rsp_q;

    return ADSP_EOK;
}

/*
   This utility function get the command queue of corresponding command queue
*/
ADSPResult  voice_proc_mgr_apr_router_get_session_cmd_q(elite_apr_port_t port,
                                                qurt_elite_queue_t **p_cmd_q)
{
   uint8_t session_id = VPM_GET_SESSION_ID( port);

   if (  session_id >= VPM_MAX_NUM_SESSIONS ) 
   {
       MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VoiceProcMgr: invalid session_id: %lu, apr_port_id: 0x%lx", session_id, port);

       return ADSP_EBADPARAM;
   }

   if (NULL == p_vpm_svc_global_obj->sessions[session_id])
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VoiceProcMgr: Session_id: %lu, apr_port_id: 0x%lx does not exist", session_id, port);

       return ADSP_EHANDLE;
   }

   vpm_rnode_t *pNode = &(vpm_apr_data_base.apr_router_lut[session_id]);

   *p_cmd_q = pNode->cmdQ;

   if ( NULL == (*p_cmd_q) )
   {
     return ADSP_EBADPARAM;
   }

   return ADSP_EOK;
}

ADSPResult  voice_proc_mgr_apr_router_get_session_rsp_q(elite_apr_port_t port,
                                                        qurt_elite_queue_t **p_rsp_q)
{
   uint8_t session_id = VPM_GET_SESSION_ID( port);

   if (  session_id >= VPM_MAX_NUM_SESSIONS ) 
   {
       MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VoiceProcMgr: invalid session_id: %lu, apr_port_id: 0x%lx", session_id, port);

       return ADSP_EBADPARAM;
   }

   if (NULL == p_vpm_svc_global_obj->sessions[session_id])
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VoiceProcMgr: Session_id: %lu, apr_port_id: 0x%lx does not exist", session_id, port);

       return ADSP_EHANDLE;
   }

   vpm_rnode_t *pNode = &(vpm_apr_data_base.apr_router_lut[session_id]);

   *p_rsp_q = pNode->rspQ;

   if ( NULL == (*p_rsp_q) )
   {
     return ADSP_EBADPARAM;
   }

   return ADSP_EOK;
}

/*
   This utility function get the service command queue
*/
ADSPResult voice_proc_mgr_apr_router_get_svc_cmd_q(qurt_elite_queue_t **p_cmd_q)
{  
   *p_cmd_q = vpm_apr_data_base.vpm_svc_cmd_q;

   return ADSP_EOK;
}

/*
   This utility function get the service response queue
*/
ADSPResult voice_proc_mgr_apr_router_get_svc_rsp_q(qurt_elite_queue_t **p_rsp_q)
{  
   *p_rsp_q = vpm_apr_data_base.vpm_svc_rsp_q;

   return ADSP_EOK;
}



