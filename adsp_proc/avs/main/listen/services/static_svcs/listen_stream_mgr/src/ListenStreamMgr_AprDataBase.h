/**
@file lsm_type.h
@brief This file declares some common data types used by ListenStreamMgr.

 */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_AprDataBase.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_APR_DATABASE_H
#define LISTEN_STREAM_MGR_APR_DATABASE_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "ListenStreamMgr_Type.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /*----------------------------------------------------------------------------
   * Preprocessor Definitions and Constants
   * -------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * Type Declarations
   * -------------------------------------------------------------------------*/


  typedef struct
  {
    qurt_elite_queue_t  *bufQ;                          // Buffer queue address
    qurt_elite_queue_t  *dyn_svc_cmdQ;                 // dynamic svc queue address

  } lsm_node_t;


  /*----------------------------------------------------------------------------
   * Global Data Definitions
   * -------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * Static Variable Definitions
   * -------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * Static Function Declarations and Definitions
   * -------------------------------------------------------------------------*/

  static inline uint8_t LSM_GET_SESSION_ID(elite_apr_port_t port)
  {
    return (uint8_t) (port & 0x00FF );
  }

  /**
  Init APR router

  @param[in]  pInst  LSM Instance 

  @return
  Indication of success or failure.

  @dependencies
  None.
   */
  ADSPResult lsm_apr_router_init(lsm_t *pInst);

  /**
This function gets the system command Q

@param pCmdQ [out] This is the system commnd Q
@return Success or failure. 
   */
  ADSPResult lsm_apr_router_get_svc_cmd_q(qurt_elite_queue_t **pCmdQ);


  /**
This function gets the output buffer Q

@param port [in] This is the port id
@param pDataQ [out] This is the output buffer Q
@return Success or failure.
   */
  ADSPResult  lsm_apr_router_get_buf_q( elite_apr_port_t port,
                                        qurt_elite_queue_t **pBufQ);
  /**
This function registers the output buffer Q

@param port [in] This is the port id
@param pgpQ [in] This is the output buffer Q
@return Success or failure.
   */
  ADSPResult lsm_apr_router_register_buf_q( elite_apr_port_t port, qurt_elite_queue_t *pBufQ );

  /**
This function deregisters the output buffer Q

@param port [in] This is the port id
@return Success or failure.
   */
  ADSPResult lsm_apr_router_deregister_buf_q( elite_apr_port_t port);

  /**
This function gets the dyn service command Q
@param port [in] This is the port id
@param pCmdQ [out] This is the dyn service commnd Q
@return Success or failure. 
   */
  ADSPResult  lsm_apr_router_get_dyn_svc_cmd_q(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ);


  /**
This function deregisters the dyn Svc CmdQ

@param port [in] This is the port id
@return Success or failure.
   */
  ADSPResult lsm_apr_router_deregister_dyn_svc_cmd_q( elite_apr_port_t port);

  /**
This function registers the dyn Svc CmdQ

@param port [in] This is the port id 
@param pCmdQ [out] This is the dyn service commnd Q 
@return Success or failure.
   */
  ADSPResult lsm_apr_router_register_dyn_svc_cmd_q( elite_apr_port_t port, qurt_elite_queue_t *pCmdQ );


#ifdef __cplusplus
}
#endif //__cplusplus

#endif
