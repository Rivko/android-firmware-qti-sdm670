/**
@file AudDevMgr_AprIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with APR.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_device" group
      description in the elite_audio_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/inc/AudDevMgr_AprIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger. 
==========================================================================*/

#ifndef AUD_DEV_MGR_APR_IF_H
#define AUD_DEV_MGR_APR_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"
#include "adsp_core_api.h"

typedef struct AdmEventCbData_t
   {
      uint16_t dest_port;
      uint16_t dest_addr;
      uint16_t src_port;
      uint16_t src_addr;
      uint32_t token;
      uint32_t registered;
      uint16_t device_id;
   }AdmEventCbData_t;



#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup audio_device
@{ */

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
* This function returns the ADM service version
*
* @param[out] svc_info_ptr    pointer to service info structure
*/
ADSPResult adm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);


/**
  Sets the ADM's APR handle.

  @param[in] handle   ADM's APR handle.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudDevMgr_SetAprHandle(elite_apr_handle_t handle);

/**
  Sets the ADM's command queue.

  @param[in] admCmdQ   Pointer to the ADM's command queue.

  @return
  None.

  @dependencies
  None.
*/
void AudDevMgr_SetCmdQueue(qurt_elite_queue_t *admCmdQ );

/**
  Frees an APR packet that the ADM received.

  @param[in] pPkt   Pointer to the APR packet that the ADM received.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudDevMgr_FreeAprPkt(elite_apr_packet_t* pPkt);

/**
  Invokes a callback function associated with the APR packet that the ADM
  received.

  @param[in] packet          Pointer to the APR packet that the ADM received.
  @param[in] dispatch_data   Obsolete. To be deprecated.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
int32_t AudDevMgr_AprCallBackFct(elite_apr_packet_t* packet, void* dispatch_data);

/**
  Generates an acknowledgment in response to an APR packet.

  @param[in] pPkt           Pointer to the APR packet that the ADM received.
  @param[in] status         Error code that must go with the acknowledgment.
  @param[in] pAckPayload    Pointer to the payload of the acknowledgment.
  @param[in] ulSize         Size of the payload of the acknowledgment.
  @param[in] ulAckOpcode    Operation code of the acknowledgment.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudDevMgr_GenerateAck(elite_apr_packet_t* pPkt,
                                ADSPResult status,
                                void *pAckPayload,
                                uint32_t ulSize,
                                uint32_t    ulAckOpcode);

/** @} */ /* end_addtogroup audio_device */

   ADSPResult AudDevMgr_GenerateClientCb(AdmEventCbData_t *callback_ptr, uint32_t opcode, void * payload_ptr, uint32_t payload_size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_APR_IF_H
