
/*========================================================================
Elite

This file defines functions, variables for managing APR routing
table.

  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.


 */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_AprDataBase.cpp#1 $$DateTime: 2017/11/07 23:00:25 $$Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir      Initial version
========================================================================== */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "ListenStreamMgr_AprDataBase.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// look up table for listen router
static struct {
  lsm_node_t          apr_router_nodes[LSM_MAX_SESSION_ID + 1 ];
  /**< at IPC level, Session ID 1- LSM_MAX_SESSION_ID .
         For easy of implementation, add empty space ahead.
   */
  qurt_elite_mutex_t  apr_router_lock;
  /**< Mutex for synchronisation b/w APR callback function and dynamic threads*/
} lsm_apr_router_database;

// Store the system command queue.
static qurt_elite_queue_t* lsm_svc_cmd_q;


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*
   This function is called after ListenStreamMgr is created. It fills in the
   command queue of different sessions.
   Given that the client is generating handle, we already know the command queue
   of each session in creation time. Hence, fill it here.
 */
ADSPResult lsm_apr_router_init(lsm_t *me_ptr)
{
  // Init LUT
  memset( lsm_apr_router_database.apr_router_nodes, 0, sizeof(lsm_apr_router_database.apr_router_nodes) );
  qurt_elite_mutex_init(&lsm_apr_router_database.apr_router_lock);

  //Register system command queue seperately.
  lsm_svc_cmd_q = me_ptr->serviceHandle.cmdQ;
  return ADSP_EOK;
}

/*
This function will register the output buffer queue of dynamic session
 */
ADSPResult lsm_apr_router_register_buf_q( elite_apr_port_t port, qurt_elite_queue_t *pBufQ )
{

  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if ((ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }
  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);
  pNode->bufQ = pBufQ;
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
  return ADSP_EOK;
}

/**
This function deregisters dynamic service output buffer Q

@param port [in] This is the port id
@return Success or failure.
 */
ADSPResult lsm_apr_router_deregister_buf_q( elite_apr_port_t port)
{

  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if ((ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }
  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);
  pNode->bufQ = NULL;
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);

  return ADSP_EOK;
}

/**
This function gets the dynamic service output buffer Q

@param port [in] This is the port id
@param pDataQ [out] This is the output buffer Q
@return Success or failure.
 */
ADSPResult  lsm_apr_router_get_buf_q( elite_apr_port_t port,
                                      qurt_elite_queue_t **pBufQ)
{

  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if (  (ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }

  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);

  *pBufQ = pNode->bufQ;

  if ( NULL == (*pBufQ) )
  {
    qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"lsm_apr_router_get_data_q ENOTREADY");
#endif
    return ADSP_ENOTREADY;
  }
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
  return ADSP_EOK;

}

/**
This function gets the system command Q

@param pCmdQ [out] This is the system commnd Q
@return Success or failure. 
 */
ADSPResult lsm_apr_router_get_svc_cmd_q(qurt_elite_queue_t **pCmdQ)
{
  *pCmdQ = lsm_svc_cmd_q;
  return ADSP_EOK;
}

/**
This function registers the dyn Svc CmdQ

@param port [in] This is the port id 
@param pCmdQ [out] This is the dyn service commnd Q 
@return Success or failure.
 */
ADSPResult lsm_apr_router_register_dyn_svc_cmd_q( elite_apr_port_t port, qurt_elite_queue_t *pCmdQ )
{

  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if ((ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }
  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);
  pNode->dyn_svc_cmdQ = pCmdQ;
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
  return ADSP_EOK;
}

/**
This function deregisters the dyn Svc CmdQ

@param port [in] This is the port id
@return Success or failure.
 */
ADSPResult lsm_apr_router_deregister_dyn_svc_cmd_q( elite_apr_port_t port)
{

  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if ((ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }
  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);
  pNode->dyn_svc_cmdQ = NULL;
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
  return ADSP_EOK;
}

/**
This function gets the dyn service command Q
@param port [in] This is the port id
@param pCmdQ [out] This is the dyn service commnd Q
@return Success or failure. 
 */
ADSPResult  lsm_apr_router_get_dyn_svc_cmd_q(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ)
{
  uint8_t ssn_id = LSM_GET_SESSION_ID( port);

  if ((ssn_id == 0) || (ssn_id > LSM_MAX_SESSION_ID) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id:[%u]", ssn_id);
#endif
    return ADSP_EBADPARAM;
  }
  qurt_elite_mutex_lock(&lsm_apr_router_database.apr_router_lock);
  lsm_node_t *pNode = &(lsm_apr_router_database.apr_router_nodes[ssn_id]);

  *pCmdQ = pNode->dyn_svc_cmdQ;

  if ( NULL == (*pCmdQ) )
  {
    qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"lsm_apr_router_get_dynsvc_cmd_q ENOTREADY");
    return ADSP_ENOTREADY;
  }
  qurt_elite_mutex_unlock(&lsm_apr_router_database.apr_router_lock);

  return ADSP_EOK;
}
