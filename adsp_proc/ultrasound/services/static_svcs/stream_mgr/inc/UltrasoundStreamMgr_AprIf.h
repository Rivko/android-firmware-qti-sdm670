/**
@file UltrasoundStreamMgr_AprIf.h
@brief This file declares APR ISR functions of UltrasoundStreamMgr. 
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/
 
/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/12/12   NR      Add support for family - B memory APIs  
01/04/12   lb      Added support for signal detection mode
10/13/10   NR      File created
==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.          
-----------------------------------------------------------------------*/

#ifndef ULTRASOUND_STREAM_MGR_APR_IF_H
#define ULTRASOUND_STREAM_MGR_APR_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Ultrasound and Audio */
#include "Elite.h"
#include "EliteAprIf.h"
#include "UltrasoundStreamMgr.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup ultrasound_stream
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
  USM's acknowledgment function. Do not use it if you are not processing APR
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
                          - USM_DATA_CMD_WRITE_V2
                          - USM_DATA_CMD_READ_V2

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult usm_generate_ack(elite_apr_packet_t* pPkt,
                            ADSPResult status,
                            void *pAckPayload,
                            uint32_t ulSize,
                            uint32_t    ackOpcode ) ;


/**
  Sends an signal detection result event to the client.

  The USM obtains a handle from the APR when it registers with the APR. It
  uses this handle to either request or free an APR packet.

  @param[in] pPkt  pointer to original signal detection mode command APR
                   packet

  @param[in] pAckPayload	pointer to result payload to be sent to client

  @param[in] ulSize			size of the result payload

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult usm_send_signal_detect_result_event(elite_apr_packet_t* pPkt,
                            void *pAckPayload,
                            uint32_t ulSize ) ;



/**   
  Used only by the USM to set the APR handle.

  The USM obtains a handle from the APR when it registers with the APR. It
  uses this handle to either request or free an APR packet.

  @param[in] handle  APR handle of the USM.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult usm_set_apr_handle(elite_apr_handle_t handle );

/**   
  Gets the APR handle that belongs to the USM.

  @return
  None.

  @dependencies
  None.
*/
elite_apr_handle_t usm_get_apr_handle( );

/**
  Callback function invoked by the APR when a packet arrives, and the packet's
  destination domain is aDSP and the service is USM.

  Currently, this function is used by only the USM.

  @param[in] packet         Pointer to the APR packet. The APR packet is
                            already copied from shared memory into a memory
                            pool managed by the APR, and it can be reused by
                            the USM.
  @param[in] dispatch_data  Registered service-defined data pointer provided
                            to the callback function when that callback
                            function is invoked.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/

int32_t usm_apr_callback_function(elite_apr_packet_t* packet,
                                  void* dispatch_data);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ULTRASOUND_STREAM_MGR_APR_IF_H

