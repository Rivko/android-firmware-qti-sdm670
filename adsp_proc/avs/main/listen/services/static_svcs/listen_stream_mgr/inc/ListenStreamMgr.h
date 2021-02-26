/**
@file ListenStreamMgr.h
@brief This file declares ListenStreamMgr.

This is a service for the Listen Stream Managment. This service
receives and processes all control commands, creates/control/destroy
all listen dynamic services for voice wakeup and target sound 
detection 
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/inc/ListenStreamMgr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------- 
12/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_H
#define LISTEN_STREAM_MGR_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_core_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/** 
  @brief Argument passed to a dynamic service  to directly raise an event with a client. 
*/
typedef struct {

    /* The following is necessary information for direct communication with client
      and for talking with APR */
    uint32_t apr_handle ;          /**< APR handle that is required in sending
                                        an acknowledgment. */
    elite_apr_addr_t client_addr;  /**< Client's address (16bit)*/
    elite_apr_port_t client_port;  /**< Client's port (16bit) */
    elite_apr_addr_t self_addr;    /**< Service's address (16bit) */
    elite_apr_port_t self_port;    /**< Service's port (16bit) */
} lsm_callback_handle_t;



/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
 * This function returns the LSM service version
 *
 * @param[out] svc_info_ptr pointer to the service info structure
 * @return error code
 */
ADSPResult lsm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

/**
  Creates an instance of the ListenStreamMgr service.

  @param[in]  dummy            Unused.
  @param[out] handle      Service entry handle that is returned to the caller.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult aud_create_stat_lsm_svc (uint32_t dummy,void **handle);


/**
  LSM's acknowledgment function. Do not use it if you are not processing APR
  packets.

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
  @param[in] ackOpcode    Some commands require a different acknowledgment
                          code.\n
                          If this field is zero, APR_IBASIC_RSP_RESULT is
                          used.\n
                          The function automatically picks an acknowledgment
                          code for the following commands. For other commands,
                          the caller must provide a valid acknowledgment
                          operation code.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult lsm_generate_ack(  elite_apr_packet_t* pPkt,
                                       ADSPResult status,
                                       void *pAckPayload,
                                       uint32_t ulSize,
                                       uint32_t    ackOpcode ) ;

ADSPResult lsm_add_static_topos_to_cmn_db(void);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LISTEN_STREAM_MGR_H

