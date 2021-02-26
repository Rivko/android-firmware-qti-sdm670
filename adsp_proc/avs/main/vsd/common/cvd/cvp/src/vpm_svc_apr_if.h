/**========================================================================
 @file vpm_svc_apr_if.h
 
 @brief This header file contains function declarations for APR command
 dispatcher
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_apr_if.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SVC_APR_IF_H_
#define _VPM_SVC_APR_IF_H_

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"
#include "EliteAprIf.h"

/** @addtogroup audio_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
inline functions
------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/


/**
  Frees an APR packet that the ASM delivered to another service.

  @param[in] pPkt   APR packet to be freed.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult voice_proc_mgr_free_apr_pkt(  elite_apr_packet_t* pPkt );


/**
  Used only by the VPM to set the APR handle.

  The VPM obtains a handle from the APR when it registers with 
  the APR. It uses this handle to either request or free an APR 
  packet. 

  @param[in] handle  APR handle of the VPM.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult voice_proc_mgr_set_apr_handle(elite_apr_handle_t handle );

/**
  Gets the APR handle that belongs to the ASM.

  @return
  None.

  @dependencies
  None.
*/
elite_apr_handle_t voice_proc_mgr_get_apr_handle( );

/**
  Callback function invoked by the APR when a packet arrives, and the packet's
  destination domain is aDSP and the service is VPM.

  Currently, this function is used by only the VPM.

  @param[in] packet         Pointer to the APR packet. The APR packet is
                            already copied from shared memory into a memory
                            pool managed by the APR, and it can be reused by
                            the VPM.
  @param[in] dispatch_data  Registered service-defined data pointer provided
                            to the callback function when that callback
                            function is invoked.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
int32_t voice_proc_mgr_apr_cb_func(elite_apr_packet_t* packet, void* dispatch_data);


#if 0
/**
  ASM's acknowledgment function. Do not use it if you are not processing APR
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
                          - ASM_DATA_CMD_EOS
                          - ASM_DATA_CMD_WRITE_V2
                          - ASM_DATA_CMD_READ_V2

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult voice_proc_mgr_gen_ack( elite_apr_packet_t* pPkt,
                                   ADSPResult status,
                                   void *pAckPayload,
                                   uint32_t ulSize,
                                   uint32_t    ackOpcode ) ;
#endif


/** @} */ /* end_addtogroup audio_stream */

#endif // #ifndef _VPM_SVC_APR_IF_H_

