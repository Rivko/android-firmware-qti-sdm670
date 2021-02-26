/**
@file AudioStreamMgr_AprIf.h
@brief This file declares APR ISR functions of AudioStreamMgr.
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

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/AudioStreamMgr_AprIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
10/20/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_APR_IF_H
#define AUDIO_STREAM_MGR_APR_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"
#include "EliteAprIf.h"
#include "AudioStreamMgr.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup audio_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
/** Hardcoded DSP domain.
*/
static const uint16_t APRV2_IDS_DOMAIN_ID_ADSP_V  =  ( 4 );

/** Hardcoded ASM service ID.
*/
static const uint16_t APRV2_IDS_SERVICE_ID_ADSP_ASM_V = (0x07);

/** ASM's APR address.
*/
const uint16_t AUDIO_STREAM_MGR_APR_ADDR = ((APRV2_IDS_DOMAIN_ID_ADSP_V << 8) | APRV2_IDS_SERVICE_ID_ADSP_ASM_V);

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
ADSPResult AudioStreamMgr_GenerateAck(  elite_apr_packet_t* pPkt,
                                       ADSPResult status,
                                       void *pAckPayload,
                                       uint32_t ulSize,
                                       uint32_t    ackOpcode ) ;
/**
  Called by a dynamic service when it is to directly raise an event to the
  client.

  Currently, only ASM_DATA_EVENT_EOS is supported.

  @param[in] pCb            Handle that the ASM provides to a
                            dynamic service when that service is created.
  @param[in] eventOpcode    Operation code for the event.
  @param[in] token          Token to be set in the event.
  @param[in] pEventPayload  Payload for the event.
  @param[in] ulPayloadSize  Size of the event payload.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudioStreamMgr_GenerateClientEventFromCb( void *pCb,
                                               uint32_t eventOpcode,
                                               uint32_t token,
                                                void *pEventPayload,
                                                uint32_t ulPayloadSize);

/**
  Frees an APR packet that the ASM delivered to another service.

  @param[in] pPkt   APR packet to be freed.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudioStreamMgr_FreeAprPkt(  elite_apr_packet_t* pPkt );


/**
  Used only by the ASM to set the APR handle.

  The ASM obtains a handle from the APR when it registers with the APR. It
  uses this handle to either request or free an APR packet.

  @param[in] handle  APR handle of the ASM.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudioStreamMgr_SetAprHandle(elite_apr_handle_t handle );

/**
  Gets the APR handle that belongs to the ASM.

  @return
  None.

  @dependencies
  None.
*/
elite_apr_handle_t AudioStreamMgr_GetAprHandle( );

/**
  Callback function invoked by the APR when a packet arrives, and the packet's
  destination domain is aDSP and the service is ASM.

  Currently, this function is used by only the ASM.

  @param[in] packet         Pointer to the APR packet. The APR packet is
                            already copied from shared memory into a memory
                            pool managed by the APR, and it can be reused by
                            the ASM.
  @param[in] dispatch_data  Registered service-defined data pointer provided
                            to the callback function when that callback
                            function is invoked.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
int32_t AudioStreamMgr_AprCallBackFct(elite_apr_packet_t* packet, void* dispatch_data);

/**
  Generates a rendered end-of-stream event based on client information.

  @param[in] pClientInfo   Pointer to the information that is required for
                           generating an acknowledgment.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudioStreamMgr_GenerateRenderedEosEvent(elite_msg_data_eos_info_t *pClientInfo);

/** @} */ /* end_addtogroup audio_stream */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_APR_IF_H

