/**
@file AudioDevMgr_DirectConnect.h
@brief This file declares functions that are used by AudioDevMgr's
       direct connections which bypass the COPP and matrix.

*/

/*===========================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_DirectConnect.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/07/2013         Created file.
==========================================================================*/

#ifndef AUD_DEV_MGR_DIRECT_CONNECT_H
#define AUD_DEV_MGR_DIRECT_CONNECT_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"

#include "AudDevMgr_AprIf.h"
#include "AudDevMgr_AfeIf.h"
#include "EliteMsg_AdmCustom.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/*
   Function to handle ULL or ULLPP stream connection to ADM.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPayload        [in] Stream conect command payload.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_DirectStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                   EliteMsg_CustomAdmStreamConnectType *pPayload,
                                   adm_client_type_t eClient);

/*
   Function to handle ULL or ULLPP stream disconnection to ADM.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPayload        [in] Stream conect command payload.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_DirectStreamDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                      EliteMsg_CustomAdmStreamConnectType *pPayload,
                                      adm_client_type_t eClient);

/*
   Map routing handler to map ULL/ULLPP stream sessions to ULL/ULLPP device session.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pSessionMapNode [in] Session map node of ULLPP stream session.
   @param matrixID        [in] Matrix ID.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_DirectMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                        adm_session_map_node_v5_t *pSessionMapNode,
                                        uint32_t matrixID,
                                        adm_client_type_t eClient);

/*
  This function is Device open command handler to open ULLPP copp.

   @param pAdmStatSvc [in] This points to the ADM static service instance.
   @param pPkt        [in] APR packet for Device open command.
   @param unCoppID    [in] COPP ID of ULLPP COPP.
   @return Success or Failure
*/
ADSPResult Adm_UllppMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                   elite_apr_packet_t *pPkt,
                                                   uint16_t unCoppID);

/*
   Device close command handler to close ULLPP COPP.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPkt            [in] APR packet for Device close command.
   @param unCoppID        [in] COPP ID of ULLPP COPP.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_UllppDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                        elite_apr_packet_t *pPkt,
                                        uint16_t unCoppID,
                                        adm_client_type_t eClient);

/*
  This function is Device open command handler to open ULL copp.

   @param pAdmStatSvc [in] This points to the ADM static service instance.
   @param pPkt        [in] APR packet for Device open command.
   @param unCoppID    [in] COPP ID of ULL COPP.
   @return Success or Failure
*/
ADSPResult Adm_UllMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                   elite_apr_packet_t *pPkt,
                                                   uint16_t unCoppID);

/*
   Device close command handler to close ULL COPP.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPkt            [in] APR packet for Device close command.
   @param unCoppID        [in] COPP ID of ULL COPP.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_UllDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                        elite_apr_packet_t *pPkt,
                                        uint16_t unCoppID,
                                        adm_client_type_t eClient);

/*
  This function is Device open command handler to open LLNP COPP.

   @param pAdmStatSvc [in] This points to the ADM static service instance.
   @param pPkt        [in] APR packet for Device open command.
   @param unCoppID    [in] COPP ID of LLNP COPP.
   @return Success or Failure
*/
ADSPResult Adm_LlnpMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                    elite_apr_packet_t *pPkt,
                                                    uint16_t unCoppID);

/*
   Matrix map routing handler to map LLNP stream session to LLNP device session.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pSessionMapNode [in] Session map node of LLNP stream session.
   @param matrixID        [in] Matrix ID.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_LlnpMtMxMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                          adm_session_map_node_v5_t *pSessionMapNode,
                                          uint32_t matrixID,
                                          adm_client_type_t eClient);

/*
   Device close command handler to close LLNP COPP.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPkt            [in] APR packet for Device close command.
   @param unCoppID        [in] COPP ID of LLNP COPP.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_LlnpDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                         elite_apr_packet_t *pPkt,
                                         uint16_t unCoppID,
                                         adm_client_type_t eClient);

/*
   Function to handle LLNP stream connection to ADM.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPayload        [in] Stream connect command payload.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_LlnpStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                 EliteMsg_CustomAdmStreamConnectType *pPayload,
                                 adm_client_type_t eClient);

/*
   Function to handle LLNP stream disconnection from ADM.
   As part of disconnection, AFE will also be disconnected.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param pPayload        [in] Stream disconnect command payload.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_LlnpStreamDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                    EliteMsg_CustomAdmStreamConnectType *pPayload,
                                    adm_client_type_t eClient);

/*
   Function to handle LLNP stream disable from ADM.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param unSessionID     [in] Stream session ID to disable.
   @param eClient         [in] The client type requesting the operation.
   @return Success or Failure
*/
ADSPResult Adm_LlnpStreamDisable(AdmStatSvc_InfoType *pAdmStatSvc,
                                 uint16_t unSessionID,
                                 adm_client_type_t eClient);

#ifdef __cplusplus
}


#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_DIRECT_CONNECT_H

