/**
@file AudDevMgr_DirectConnect.cpp
@brief This file contains the implementation of AudioDevMgr's
       direct connections which bypass the COPP and matrix.

 */

/*===========================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_DirectConnect.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/07/2013         Created file.
========================================================================== */


/* =======================================================================
Include Files
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "AudDevMgr.h"
#include "AudDevMgr_DirectConnect.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "adsp_asm_stream_commands.h"
#include "AudDevMgr_MtMxIf.h"
#include "AudDevMgr_AsmIf.h"
#include "AudDevMgr_CoppIf.h"
#include "AudDevMgr_mmpm.h"
#include "EliteData_Util.h"

#define DUMMY_HANDLE 0xDEADDEAD
/* =======================================================================
Private Function Declarations
========================================================================== */
/*
  Function to send push a generic msg into the ASM session cmd queue. This
  function will push the message and wait for the response.

   @param sessionID       [in] stream session ID to issue the cmd to.
   @param pMsg            [in] the msg to push to the ASM session cmd queue
   @return Success or Failure
 */
static ADSPResult Adm_SendCmdToAsmSessionAndWait(uint32_t unSessionID, elite_msg_any_t *pMsg);

/*
  Function to send device handle information to ASM. Device handle information will
  be used to connect the stream service with downstream service. Same function
  will be used to send disconnect command to ASM.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phDownstreamSvc [in] downstream handle to connect/disconnect.
   @param sessionID       [in] stream session ID to connect/disconnect.
   @param opCode          [in] Connect command or disconnect command.
   @return Success or Failure
 */
static ADSPResult Adm_SendCmdAsmStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                              elite_svc_handle_t  *phDownstreamSvc,
                                              uint32_t unSessionID,
                                              uint32_t opCode);

/*
  Function to query the ASM for the handle to the service which receives data in the Tx path, ie.
  the sink. The name is left generic as in the future the service acting as the "sink" may change.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param sessionID       [in] ASM session is receiving the data.
   @param phSinkHandle    [out] The service handle
   @return Success or Failure
 */
static ADSPResult Adm_SendCmdQueryAsmTxSinkHandle(AdmStatSvc_InfoType *pAdmStatSvc,
                                                   uint32_t unSessionID,
                                                   elite_svc_handle_t  **phSinkHandle);

/*
   Alerts the ASM that a disconnection is pending. The ASM should take any appropriate actions
   before disconnection upon receiving this message.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param sessionID       [in] ASM session is receiving the data.
   @return Success or Failure
 */
static ADSPResult Adm_SendCmdAsmPrepDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                               uint32_t unSessionID);

/*
   Adds an entry to the Session ID to ADM Primary Copp map used to store information about
   which COPP is the primary COPP for each session.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param unMatrixID      [in] ADM_MATRIX_ID_AUDIO_RX or ADM_MATRIX_ID_AUDIO_TX
   @param unCoppID        [in] The COPP ID
   @param unSessionID     [in] The session ID
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static void Adm_DC_AddPrimaryCoppToSessionIDMap(AdmStatSvc_InfoType *pAdmStatSvc,
                                                uint32_t unMatrixID,
                                                uint16_t unCoppID,
                                                uint32_t unSessionID,
                                                adm_client_type_t eClient);

/*
   Clears the Session ID to ADM Primary Copp map used to store information about which COPP is
   the primary COPP for each session.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param unMatrixID      [in] ADM_MATRIX_ID_AUDIO_RX or ADM_MATRIX_ID_AUDIO_TX
   @param unSessionID     [in] The session ID to remove.
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static void Adm_DC_RemovePrimaryCoppFromSessionIDMap(AdmStatSvc_InfoType *pAdmStatSvc,
                                                     uint32_t unMatrixID,
                                                     uint32_t unSessionID,
                                                     adm_client_type_t eClient);

/*
   Checks if the device open parameters are suitable for direct connections (LLNP/ULL).

   @param unAfeEndpoint    [in] AFE endpoint
   @param unDevNumChannels [in] Number of channels
   @param unBitWidth       [in] Bit width
   @param unSampleRate     [in] Sampling rate
   @return Success or Failure
 */
static ADSPResult Adm_DC_ValidateDeviceOpenParams(uint16_t unAfeEndpoint,
                                                  uint16_t unDevNumChannels,
                                                  uint16_t unBitWidth,
                                                  uint32_t unSampleRate);

/*
   Checks if the device open parameters are suitable for direct connections (ULLPP).

   @param unAfeEndpoint    [in] AFE endpoint
   @param unDevNumChannels [in] Number of channels
   @param unBitWidth       [in] Bit width
   @param unSampleRate     [in] Sampling rate
   @return Success or Failure
 */
static ADSPResult Adm_DC_ValidateUllppDeviceOpenParams(uint16_t unAfeEndpoint,
                                                       uint16_t unDevNumChannels,
                                                       uint16_t unBitWidth,
                                                       uint32_t unSampleRate);

/*
   Populates the cal port structure with the open device parameters.

   @param unAfeEndpoint    [in] AFE endpoint
   @param unDevNumChannels [in] Number of channels
   @param unBitWidth       [in] Bit width
   @param unSampleRate     [in] Sampling rate
   @return Success or Failure
 */
static ADSPResult Adm_DC_InitCalPort(AdmCalPort_Type *phCalPort,
                                     adm_cmd_device_open_v5_t *pCoppOpenParams);


/*
   Opens an ULLPP Rx device with a COPP used for direct connections between the stream and COPP.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure used to establish the connection
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param pCoppOpenParams [in] ADM open params
   @return Success or Failure
 */
static ADSPResult Adm_DC_OpenUllppRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                           AdmCalPort_Type *phCalPort,
                                           uint16_t unCoppID,
                                           adm_cmd_device_open_v5_t *pCoppOpenParams);

/*
   Opens a Rx device used for direct connections between the stream and AFE.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure used to establish the connection
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param pCoppOpenParams [in] ADM open params
   @return Success or Failure
 */
static ADSPResult Adm_DC_OpenRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                      AdmCalPort_Type *phCalPort,
                                      uint16_t unCoppID,
                                      adm_cmd_device_open_v5_t *pCoppOpenParams);

/*
   Checks for a connected Rx stream and disconnects it from the upstream service

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to disconnect from
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_DisconnectRxStream(AdmStatSvc_InfoType *pAdmStatSvc,
                                            AdmCalPort_Type *phCalPort,
                                            adm_client_type_t eClient);

/*
   Common close routine to set state variables

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to clear
   @return Success or Failure
 */
static ADSPResult Adm_DC_CloseCommonHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                            AdmCalPort_Type *phCalPort,
                                            uint16_t unCoppID);

/*
   Closes the ULLPP Rx device used for direct connections between the stream and COPP.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to close
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_CloseUllppRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                            AdmCalPort_Type *phCalPort,
                                            uint16_t unCoppID, 
                                            adm_client_type_t eClient);

/*
   Closes the Rx device used for direct connections between the stream and AFE.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to close
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_CloseRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                       AdmCalPort_Type *phCalPort,
                                       uint16_t unCoppID,
                                       adm_client_type_t eClient);

/*
   Establishes a direct Rx connection between a stream and a device.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to facilitate the connection
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param unSessionID     [in] The session ID to connect to
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_ConnectRxStreamToDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                                 AdmCalPort_Type *phCalPort,
                                                 uint16_t unCoppID,
                                                 uint32_t unSessionID,
                                                 adm_client_type_t eClient);

/*
   Closes the Rx connection between a stream and a device.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to remove connection from
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param unSessionID     [in] The session ID to disconnect
   @param pSessionInfo    [in] ADM session info used to store state information about the stream.
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_DisconnectRxStreamFromDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                                      AdmCalPort_Type *phCalPort,
                                                      uint16_t unCoppID,
                                                      uint32_t unSessionID,
                                                      AdmStreamSessionInfo_Type *pSessionInfo,
                                                      adm_client_type_t eClient);
/*
   Opens a Tx device used for direct connections between the stream and AFE.

   @param phCalPort       [in] Calport structure used to establish the connection
   @param pCoppOpenParams [in] ADM open parameters
   @return Success or Failure
 */
static ADSPResult Adm_DC_OpenTxDevice(AdmCalPort_Type *phCalPort,
                                      adm_cmd_device_open_v5_t *pCoppOpenParams);

/*
   Closes the Tx device used for direct connections between the stream and AFE.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure used to establish the connection
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_CloseTxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                       AdmCalPort_Type *phCalPort,
                                       uint16_t unCoppID,
                                       adm_client_type_t eClient);

/*
   Establishes a Tx connection between a stream and a device.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to facilitate the connection
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param unSessionID     [in] The session ID to connect to
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_ConnectTxStreamToDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                                 AdmCalPort_Type *phCalPort,
                                                 uint16_t unCoppID,
                                                 uint32_t unSessionID,
                                                 adm_client_type_t eClient);

/*
   Closes the Tx connection between a stream and a device.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure to remove connection from
   @param unCoppID        [in] The ADM COPP ID associated with the calport
   @param unSessionID     [in] The session ID to disconnect
   @param eClient         [in] The client type requesting the operation
   @return Success or Failure
 */
static ADSPResult Adm_DC_DisconnectTxStreamFromDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                                      AdmCalPort_Type *phCalPort,
                                                      uint16_t unCoppID,
                                                      uint32_t unSessionID,
                                                      adm_client_type_t eClient);


/*
   Disconnect the COPP and then reconnect it to the AFE

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure for COPP to disconnect/reconnect
   @return Success or Failure
 */
static ADSPResult Adm_DC_ReconnectAfeToCopp(AdmStatSvc_InfoType *pAdmStatSvc,
                                     AdmCalPort_Type *phCalPort,
                                     uint16_t unCoppID);

/*
   Sets the media type for ULLPP streams and devices.

   @param pAdmStatSvc     [in] This points to the ADM static service instance.
   @param phCalPort       [in] Calport structure for COPP
   @param
   @return None. All errors are treated as warnings.
 */
static void Adm_DC_SendMediaFormat(AdmStatSvc_InfoType *pAdmStatSvc,
                                 AdmCalPort_Type *phCalPort,
                                 uint16_t unCoppID,
                                 AdmStreamSessionInfo_Type *pSessionInfo,
                                 uint16_t unSessionID);


/* =======================================================================
Private Function Definitions
========================================================================== */
ADSPResult Adm_SendCmdToAsmSessionAndWait(uint32_t unSessionID, elite_msg_any_t *pMsg)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_queue_t *asmSessionCmdQ;

   // This function always expects a contemporary session ID
   uint16_t extractedSessionID = 0;
   extractedSessionID = (0xff00 & (uint16_t)(unSessionID)) >> 8;

   if ((extractedSessionID > ASM_MAX_SUPPORTED_SESSION_ID) || (extractedSessionID == 0))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Invalid ASM session ID [0x%x]",
            extractedSessionID);
      return ADSP_EBADPARAM;
   }

   if (qurt_elite_globalstate.pAsmSessionCmdQ[extractedSessionID] == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Failed to get ASM SessionID 0x%x cmdQ ptr", unSessionID);
      return ADSP_EBADPARAM;
   }
   asmSessionCmdQ = qurt_elite_globalstate.pAsmSessionCmdQ[extractedSessionID];

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(asmSessionCmdQ, (uint64_t*)(pMsg))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to ASM session command queue");
      return result;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(pMsg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      return result;
   }

   return result;
}

ADSPResult Adm_SendCmdAsmStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                          elite_svc_handle_t  *phDownstreamSvc,
                                          uint32_t unSessionID,
                                          uint32_t opCode)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t bufMgrNode;
   int nActualSize;

   result = elite_mem_get_buffer(sizeof(EliteMsg_CustomASMStreamDirectConnectType),
                                 &bufMgrNode,
                                 &nActualSize);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
      return result;
   }

   //Extract the session and stream ID from the incoming ID.
   uint16_t extractedSessionID = 0, extractedStreamID = 0;
   extractedSessionID = (0xff00 & (uint16_t)(unSessionID)) >> 8;
   extractedStreamID  = (0x00ff & (uint16_t)(unSessionID));

   EliteMsg_CustomASMStreamDirectConnectType *pConnectCmdPayload =
         (EliteMsg_CustomASMStreamDirectConnectType*)bufMgrNode.pBuffer;
   pConnectCmdPayload->pResponseQ = pAdmStatSvc->admResponseQ;
   pConnectCmdPayload->unClientToken = 0;
   pConnectCmdPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pConnectCmdPayload->phDownstreamDevice = phDownstreamSvc;
   pConnectCmdPayload->unSecOpCode = opCode;
   pConnectCmdPayload->unStreamID = extractedStreamID;

   if (ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT == opCode)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ADM: Sending cmd to ASM to connect SSID 0x%lX, SessionID %d, StreamID %d to svc 0x%lx, svc ID 0x%lx",
            unSessionID, extractedSessionID, extractedStreamID, phDownstreamSvc, phDownstreamSvc->unSvcId);
   }
   else
   {
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ADM: Sending cmd to ASM to disconnect SSID 0x%lX, SessionID %d, StreamID %d from svc 0x%lx, svc ID 0x%lx",
            unSessionID, extractedSessionID, extractedStreamID, phDownstreamSvc, phDownstreamSvc->unSvcId);
   }

   elite_msg_any_t msg;
   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void *)pConnectCmdPayload;

   // Synchronously send the custom command to the ASM
   result = Adm_SendCmdToAsmSessionAndWait(unSessionID, (&msg));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] sending cmd to ASM", result);
      goto __bailoutCmdAsmStreamAfeConnect;
   }

   result = (ADSPResult)pConnectCmdPayload->unResponseResult;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
      goto __bailoutCmdAsmStreamAfeConnect;
   }

   if (ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT == opCode)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ADM: Cmd to connect ASM SSID 0x%lX, SessionID %d, StreamID %d to device was successful",
            unSessionID, extractedSessionID, extractedStreamID);
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ADM: Cmd to disconnect ASM SSID 0x%lX, SessionID %d, StreamID %d from device was successful",
            unSessionID, extractedSessionID, extractedStreamID);
   }

__bailoutCmdAsmStreamAfeConnect:
   elite_msg_release_msg(&msg);
   return result;
}

ADSPResult Adm_SendCmdQueryAsmTxSinkHandle(AdmStatSvc_InfoType *pAdmStatSvc,
                                           uint32_t unSessionID,
                                           elite_svc_handle_t **phSinkHandle)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Querying the ASM for session ID 0x%x Tx sink handle",
         unSessionID);

   qurt_elite_bufmgr_node_t bufMgrNode;
   int nActualSize;
   result = elite_mem_get_buffer(sizeof(EliteMsg_CustomAsmCommandQueryTxSinkHandle),
                                 &bufMgrNode,
                                 &nActualSize);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create msg!");
      return result;
   }

   // Populate the custom message to query for the downstream service handle
   EliteMsg_CustomAsmCommandQueryTxSinkHandle *pPayload =
         (EliteMsg_CustomAsmCommandQueryTxSinkHandle *)bufMgrNode.pBuffer;
   pPayload->pResponseQ = pAdmStatSvc->admResponseQ;
   pPayload->unClientToken = 0;
   pPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pPayload->phSinkHandle = NULL; // this is provided in the response from the ASM.
   pPayload->unSecOpCode = ELITEMSG_CUSTOM_ASM_LLNP_STREAM_QUERY_TX_SINK_HANDLE;
   // Must extract the stream ID from the contemporary Stream/SessionID
   pPayload->unStreamID = (0x00ff & unSessionID);

   // wrap the custom msg in an ELITE_CUSTOM_MSG
   elite_msg_any_t msg;
   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void *)pPayload;

   // send the custom msg to the ASM
   result = Adm_SendCmdToAsmSessionAndWait(unSessionID, &msg);
   if (ADSP_FAILED(result) || ADSP_FAILED(pPayload->unResponseResult) || (NULL == (pPayload->phSinkHandle)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Error in query Tx sink handle cmd or NULL handle received; result code [0x%x]",
            result);
      *phSinkHandle = NULL;

       //Set return result explicitly to account for only NULL check being TRUE
       result = ADSP_EFAILED; 
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Received Tx sink handle 0x%p",
            pPayload->phSinkHandle);
      *phSinkHandle = pPayload->phSinkHandle;
   }

   elite_msg_release_msg(&msg);
   return result;
}

ADSPResult Adm_SendCmdAsmPrepDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                        uint32_t unSessionID)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Sending ASM prep disconnect for session ID 0x%x",
         unSessionID);

   qurt_elite_bufmgr_node_t bufMgrNode;
   int nActualSize;
   result = elite_mem_get_buffer(sizeof(EliteMsg_CustomAsmCommandPrepDisconnect),
                                 &bufMgrNode,
                                 &nActualSize);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create msg!");
      return result;
   }

   EliteMsg_CustomAsmCommandPrepDisconnect *pPayload =
         (EliteMsg_CustomAsmCommandPrepDisconnect *)bufMgrNode.pBuffer;
   pPayload->pResponseQ = pAdmStatSvc->admResponseQ;
   pPayload->unClientToken = 0;
   pPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pPayload->unSecOpCode = ELITEMSG_CUSTOM_ASM_LLNP_PREPARE_FOR_DISCONNECT;
   pPayload->unStreamID = (0x00ff & unSessionID);

   // wrap the custom msg in an ELITE_CUSTOM_MSG
   elite_msg_any_t msg;
   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void *)pPayload;

   // send the custom msg to the ASM
   result = Adm_SendCmdToAsmSessionAndWait(unSessionID, &msg);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to send prepare disconnect cmd [0x%x]",
            result);
   }

   if (ADSP_FAILED(pPayload->unResponseResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ASM prep disconnect cmd failed [0x%x]",
            pPayload->unResponseResult);
      result = pPayload->unResponseResult;
   }

   if (ADSP_SUCCEEDED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: ASM prep disconnect completed successfully");
   }

   elite_msg_release_msg(&msg);
   return result;
}

void Adm_DC_RemovePrimaryCoppFromSessionIDMap(AdmStatSvc_InfoType *pAdmStatSvc,
                                              uint32_t unMatrixID,
                                              uint32_t unSessionID,
                                              adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error: Unsupported for LSM Session");
     return;
   }
   uint32_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, unMatrixID, unSessionID, eClient);
   if(ADM_INVALID_INDEX == unIndex)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: COPP index not found!");
      return;
   }

   if (ADM_MATRIX_ID_AUDIO_RX == unMatrixID)
   {
      pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
   }
   else
   {
      pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
   }

   return;
}

void Adm_DC_AddPrimaryCoppToSessionIDMap(AdmStatSvc_InfoType *pAdmStatSvc,
                                         uint32_t unMatrixID,
                                         uint16_t unCoppID,
                                         uint32_t unSessionID,
                                         adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error: Unsupported for LSM Session");
      return;
   }
   uint32_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, unMatrixID, unSessionID, eClient);
   if(ADM_INVALID_INDEX == unIndex)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: COPP index not found!");
      return;
   }

   if (ADM_MATRIX_ID_AUDIO_RX == unMatrixID)
   {
      pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = unCoppID;
   }
   else
   {
      pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex] = unCoppID;
   }

   return;
}

ADSPResult Adm_DC_ValidateDeviceOpenParams(uint16_t unAfeEndpoint,
                                           uint16_t unDevNumChannels,
                                           uint16_t unBitWidth,
                                           uint32_t unSampleRate)
{
   // Check for validity of endpointID1
   if (ADSP_EOK != afe_port_id_validation(unAfeEndpoint))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid endpoint_id_1 [%u]", unAfeEndpoint);
      return ADSP_EBADPARAM;
   }

   // Check for validity of dev_num_channel
   if ((ADM_NUM_CHANNELS_EIGHT < unDevNumChannels) || (unDevNumChannels <= 0))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid dev_num_channel [%u], ULL supports up to 8 channels",
            unDevNumChannels);
      return ADSP_EBADPARAM;
   }

   // Check for validity of bit_width
   if ( (ADM_BITS_PER_SAMPLE_16 != unBitWidth ) && (ADM_BITS_PER_SAMPLE_24 != unBitWidth ) &&(ADM_BITS_PER_SAMPLE_32 != unBitWidth))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid bit_width [%u], ULL supports only 16-bit/24-bit/32-bit", unBitWidth);
      return ADSP_EBADPARAM;
   }

   // Check for validity of sample_rate
   if(ADSP_FAILED(is_valid_device_sampling_rate(unSampleRate)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid sample_rate [%lu]Hz", unSampleRate);
      return ADSP_EBADPARAM;
   }

   return ADSP_EOK;
}

ADSPResult Adm_DC_ValidateUllppDeviceOpenParams(uint16_t unAfeEndpoint,
                                                uint16_t unDevNumChannels,
                                                uint16_t unBitWidth,
                                                uint32_t unSampleRate)
{
   // Check for validity of endpointID1
   if (ADSP_EOK != afe_port_id_validation(unAfeEndpoint))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid endpoint_id_1 [0x%X]", unAfeEndpoint);
      return ADSP_EBADPARAM;
   }

   // Check for validity of dev_num_channel
   if ((ADM_NUM_CHANNELS_EIGHT < unDevNumChannels) || (unDevNumChannels <= 0))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid dev_num_channel [%u]",
            unDevNumChannels);
      return ADSP_EBADPARAM;
   }

   // Check for validity of bit_width
   if ( (ADM_BITS_PER_SAMPLE_16 != unBitWidth ) && (ADM_BITS_PER_SAMPLE_24 != unBitWidth ) && (ADM_BITS_PER_SAMPLE_32!= unBitWidth))
{
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid bit_width [%u]", unBitWidth);
      return ADSP_EBADPARAM;
   }

   // Check for validity of sample_rate
   if(ADSP_FAILED(is_valid_device_sampling_rate(unSampleRate)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid sample_rate [%lu]Hz", unSampleRate);
      return ADSP_EBADPARAM;
   }

   return ADSP_EOK;
   }

ADSPResult Adm_DC_InitCalPort(AdmCalPort_Type *phCalPort,
                              adm_cmd_device_open_v5_t *pCoppOpenParams)
{
   ADSPResult result = ADSP_EOK;
   phCalPort->unEID1.unAfePortID = pCoppOpenParams->endpoint_id_1;
   //second endpoint ID is ignored on playback rx path
   phCalPort->unEID2.unAfePortID = ADM_CMD_COPP_OPEN_END_POINT_ID_2_IGNORE;
   phCalPort->unCoppTopologyID = pCoppOpenParams->topology_id;
   phCalPort->unEID1.unNumChannels = pCoppOpenParams->dev_num_channel;
   for (uint16_t i = 0; i < pCoppOpenParams->dev_num_channel; i++)
   {
     phCalPort->unEID1.unChannelMap[i] = pCoppOpenParams->dev_channel_mapping[i];
   }
   phCalPort->unEID1.unBytesPerSample = elite_data_get_bytes_per_sample(pCoppOpenParams->bit_width);
   phCalPort->unEID1.unBitWidth = pCoppOpenParams->bit_width;
   phCalPort->unEID1.unSampleRate = pCoppOpenParams->sample_rate;
   phCalPort->unMtMxSampleRate = pCoppOpenParams->sample_rate;
   phCalPort->unCoppNumAfeFrames = ADM_LL_NUM_AFE_FRAMES;
   phCalPort->unCoppType = ADM_COPP_TYPE_AUDIO_COPP;

   return result;
}

ADSPResult Adm_DC_OpenUllppRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                    AdmCalPort_Type *phCalPort,
                                    uint16_t unCoppID,
                                    adm_cmd_device_open_v5_t *pCoppOpenParams)
{
   ADSPResult result;
   result = Adm_DC_ValidateUllppDeviceOpenParams(pCoppOpenParams->endpoint_id_1,
                                                 pCoppOpenParams->dev_num_channel,
                                                 pCoppOpenParams->bit_width,
                                                 pCoppOpenParams->sample_rate);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Rx open failed with result [%d] due to invalid open params", result);
      return result;
   }

   // Update calport structure with validated device open params
   result = Adm_DC_InitCalPort(phCalPort, pCoppOpenParams);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Rx open failed to init copp with result [%d]", result);
      return result;
   }
   // set the proper direction
   phCalPort->unCoppDirection = RX_DIR;

   // check the AFE frame size
   uint32_t unCoppAfeFrameSizeInSamples;
   result = afe_port_get_frame_size(AFE_PORT_DEFAULT_LATENCY_MODE, pCoppOpenParams->sample_rate, &unCoppAfeFrameSizeInSamples);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Invalid sampling rate [%lu] for Audio COPP topology %lu",
				pCoppOpenParams->sample_rate, pCoppOpenParams->topology_id);
      Adm_ResetCalPort(phCalPort);
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: As part of Rx ADM_CMD_DEVICE_OPEN, issuing CREATE_COPP cmd, topologyID: 0x%08x, sampling rate %lu",
         pCoppOpenParams->topology_id, pCoppOpenParams->sample_rate);

   AdmDevSessionDelayParam_Type *pSessionDelayParam =
         &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);

   //Increment the Number of RX Audio COPP opened
   pAdmStatSvc->unNumRxAudioCoppOpenIssued++;
   if(ADM_MAX_COPP_OPEN_CTR < pAdmStatSvc->unNumRxAudioCoppOpenIssued)
   {
      pAdmStatSvc->unNumRxAudioCoppOpenIssued = 0;
   }
   uint32_t unLogID = ADM_LOG_ID_BASE_RX + (pAdmStatSvc->unNumRxAudioCoppOpenIssued << 4);

   result = Adm_CreatePPSvc(pAdmStatSvc,
                            phCalPort,
                            pSessionDelayParam,
                            phCalPort->unCoppTopologyID,
                            DYNA_SVC_PP_TYPE_COPP,
                            unLogID);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: CREATE_COPP cmd failed, result = %d", result);
      return result;
   }

   // create the AFE client
   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing AFE_CONNECT_REQ cmd, EP1: [0x%X, SR %lu, num ch %lu, bit width %lu], afe frame size = %ld"
         "device_perf_mode is [%hu] & COPP Buffer Num Frames is [%lu] & unLogID is [0x%lX]",
         phCalPort->unEID1.unAfePortID, phCalPort->unEID1.unSampleRate,  phCalPort->unEID1.unNumChannels, phCalPort->unEID1.unBitWidth, unCoppAfeFrameSizeInSamples,
         phCalPort->unDevicePerfMode, phCalPort->unCoppNumAfeFrames, unLogID);

   // "Connect" the AFE - creates an AFE client structure.
   if (ADSP_FAILED(result = Adm_SendCmdConnectAFE(pAdmStatSvc,
                                                  phCalPort->unEID1.unAfePortID,
                                                  phCalPort->phDevCopp,
                                                  AFE_RX_AUD_MIXER_IN,
                                                  &(phCalPort->unEID1.phAfePort),
                                                  phCalPort->unEID1.unSampleRate,
                                                  phCalPort->unEID1.unNumChannels,
                                                  &(phCalPort->unEID1.unChannelMap[0]),
                                                  phCalPort->unCoppNumAfeFrames,
                                                  ADM_NORMAL_BUFFERING_BETWEEN_AFE_AND_CLIENT,
                                                  ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                                  phCalPort->unEID1.unBitWidth,
                                                  &(phCalPort->unEID1.pAfeDriftPtr),
                                                  &(pSessionDelayParam->unAFEDelay_EP1),
                                                  FALSE)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: AFE_CONNECT_REQ cmd failed, result = %d", result);
      goto __bailout_on_afe_connect;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Rx ADM_CMD_DEVICE_OPEN, issuing CONNECT_COPP cmd");
   result = AdmSendCoppMsg(phCalPort->phDevCopp,
                           pAdmStatSvc->admResponseQ,
                           phCalPort->unEID1.phAfePort,
                           ELITE_CMD_CONNECT);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: CONNECT_COPP cmd failed, result = %d", result);
      goto __bailout_on_copp_connect;
   }

   // Send the run command to the COPP
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Rx ADM_CMD_DEVICE_OPEN, issuing RUN_COPP cmd");
   result = Adm_SendCmdRunCopp(phCalPort->phDevCopp, pAdmStatSvc->admResponseQ);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: RUN_COPP cmd failed, result = %d", result);
      goto __bailout_on_copp_run;
   }

   // update COPP status
   {
      phCalPort->unCoppStatus = ADM_COPP_STATUS_ACTIVE;
   }

   return result;

__bailout_on_copp_run:
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: issuing COPP_DISCONNECT cmd");
   result = AdmSendCoppMsg(phCalPort->phDevCopp,
                           pAdmStatSvc->admResponseQ,
                           phCalPort->unEID1.phAfePort,
                           ELITE_CMD_DISCONNECT);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DISCONNECT cmd failed, result = %d", result);
   }

__bailout_on_copp_connect:
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: issuing AFE_PORT_DISCONNECT cmd, EP1: 0x%X", phCalPort->unEID1.unAfePortID);
   result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                     phCalPort->unEID1.unAfePortID,
                                     phCalPort->unEID1.phAfePort,
                                     ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: AFE_PORT_DISCONNECT cmd failed, result = %d", result);
   }
   
__bailout_on_afe_connect:
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: issuing COPP_DESTROY");
   result = AdmSendCoppMsg(phCalPort->phDevCopp,
                           pAdmStatSvc->admResponseQ,
                           NULL,
                           ELITE_CMD_DESTROY_SERVICE);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: COPP_DESTROY cmd failed, result = %d", result);
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Waiting for COPP to join");
   qurt_elite_thread_join(phCalPort->phDevCopp->threadId, &result);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP thread join failed, result = %d", result);
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: COPP thread joined");

   Adm_ResetCalPort(phCalPort);
		return ADSP_EFAILED;
   }

ADSPResult Adm_DC_OpenRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                               AdmCalPort_Type *phCalPort,
                               uint16_t unCoppID,
                               adm_cmd_device_open_v5_t *pCoppOpenParams)
{
   ADSPResult result = Adm_DC_ValidateDeviceOpenParams(pCoppOpenParams->endpoint_id_1,
                                                       pCoppOpenParams->dev_num_channel,
                                                       pCoppOpenParams->bit_width,
                                                       pCoppOpenParams->sample_rate);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Rx open failed with result [%d] due to invalid open params", result);
      return result;
   }

   result = Adm_DC_InitCalPort(phCalPort, pCoppOpenParams);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Rx open failed to init copp with result [%d]", result);
      return result;
   }

   // set the proper direction
   phCalPort->unCoppDirection = RX_DIR;

   // get the AFE frame size
   uint32_t unCoppAfeFrameSizeInSamples;
   elite_svc_get_frame_size(pCoppOpenParams->sample_rate, &unCoppAfeFrameSizeInSamples);
   
   // create the AFE client
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing AFE_CONNECT_REQ cmd, EP1: 0x%X, SR %lu,"
         "num ch %lu, bit-width %lu, afe frame size = %lu, device_perf_mode is [%hu] & COPP Buffer Num Frames is [%lu] ",
         phCalPort->unEID1.unAfePortID, pCoppOpenParams->sample_rate,  phCalPort->unEID1.unNumChannels, phCalPort->unEID1.unBitWidth, unCoppAfeFrameSizeInSamples,
         phCalPort->unDevicePerfMode, phCalPort->unCoppNumAfeFrames);

   /* During decoder service connection to AFE, assuming default values for sample rate,
      number of channels and  bits per sample. Actual values will be sent to AFE through
      media format and AFE will reconfigure accordingly. */
   AdmDevSessionDelayParam_Type *pSessionDelayParam;
   pSessionDelayParam = &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);
   if (ADSP_FAILED(result = Adm_SendCmdConnectAFE(pAdmStatSvc,
                                                  phCalPort->unEID1.unAfePortID,
                                                  NULL,              //client handle will be NULL
                                                  AFE_RX_AUD_MIXER_IN,
                                                  &(phCalPort->unEID1.phAfePort),
                                                  pCoppOpenParams->sample_rate,
                                                  phCalPort->unEID1.unNumChannels,
                                                  &phCalPort->unEID1.unChannelMap[0],
                                                  phCalPort->unCoppNumAfeFrames,
                                                  ADM_NORMAL_BUFFERING_BETWEEN_AFE_AND_CLIENT, /* ignored by AFE for Rx clients */
                                                  ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                                  phCalPort->unEID1.unBitWidth,
                                                  &(phCalPort->unEID1.pAfeDriftPtr),
                                                  &(pSessionDelayParam->unAFEDelay_EP1),
                                                  FALSE)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: AFE_CONNECT_REQ cmd failed, result = %d", result);
      return result;
   }

   // update COPP status
   phCalPort->unCoppStatus = ADM_COPP_STATUS_ACTIVE;
   
   return result;
}

ADSPResult Adm_DC_DisconnectRxStream(AdmStatSvc_InfoType *pAdmStatSvc, AdmCalPort_Type *phCalPort,adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   if (eLSM_TYPE == eClient)
   {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
    return ADSP_EBADPARAM;
   }

   if (1 == phCalPort->unNumConnectedSessionsRx)
   {
      // Session ID corresponding to this COPP ID. Since ULL/LLNP stream to device is one-to-one,
      // there will be only one stream session.
      uint32_t sessionID = phCalPort->unDirectConnSessionID;
      uint16_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc,
                                                    (uint16_t)RX_DIR,
                                                    (uint16_t)(sessionID),eClient);
      if(ADM_INVALID_INDEX == unIndex)
      {
         return ADSP_EBADPARAM;
      }

      // Send disconnect command to ASM to disconnect the upstream service from the COPP/AFE
      elite_svc_handle_t *downstream_svc = phCalPort->unEID1.phAfePort;
      if (NULL != phCalPort->phDevCopp)
      {
         // if there is a COPP we should disconnect from the COPP and not AFE
         downstream_svc = phCalPort->phDevCopp;
      }
      // This command is required only if stream session is not already closed.
      // Checking against stream performance mode to make sure stream session is not closed already.
      AdmStreamSessionInfo_Type *pSessionInfo;
      pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
      if(ASM_LEGACY_STREAM_SESSION != pSessionInfo->stream_perf_mode)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending ASM stream disconnect");
         result = Adm_SendCmdAsmStreamConnect(pAdmStatSvc,
                                              downstream_svc,
                                              sessionID,
                                              ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_DISCONNECT);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "ADM: ASM stream disconnected failed, result = %d", result);
            return result;
         }

      }
   }

      return result;
   }

ADSPResult Adm_DC_CloseCommonHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                     AdmCalPort_Type *phCalPort,
                                     uint16_t unCoppID)
{
   ADSPResult result;

   //Remove this Device ID (COPP ID) from any existing session mapping masks
   Adm_RemoveCoppIDFromSessionMappingMask(pAdmStatSvc, unCoppID, (uint16_t)RX_DIR);

   // Reset the calport
   Adm_ResetCalPort(phCalPort);

   // Reset the session delay parameters
   AdmDevSessionDelayParam_Type *pSessionDelayParam;
   pSessionDelayParam = &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);
   pSessionDelayParam->unCoppBufDelay = 0;
   pSessionDelayParam->unCoppAlgDelay = 0;
   pSessionDelayParam->unAFEDelay_EP1 = 0;
   pSessionDelayParam->unAFEDelay_EP2 = 0;
   pSessionDelayParam->unAFEDelay_EP3 = 0;

   // Detect primary device of all active streams and devices.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of ADM_CMD_COPP_CLOSE, updating primary device/port");
   if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDevicePcm(pAdmStatSvc)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: updating primary device/port failed, result = %d", result);
      return result;
   }

   return result;
}

ADSPResult Adm_DC_CloseUllppRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                            AdmCalPort_Type *phCalPort,
                                            uint16_t unCoppID,adm_client_type_t eClient)
{
   ADSPResult result;
   
   if (eLSM_TYPE == eClient)
   {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
    return ADSP_EBADPARAM;
   }

   // check for connected streams and disconnect if necessary
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "As part of Rx ADM_CMD_COPP_CLOSE, sending cmd to ASM to disconnect from device");
   result = Adm_DC_DisconnectRxStream(pAdmStatSvc, phCalPort,eClient);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Failed to disconnect stream!");
      return result;
   }

   // Disconnect COPP from AFE
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of Rx ADM_CMD_DEVICE_CLOSE, issuing COPP_DISCONNECT cmd");
   result = AdmSendCoppMsg(phCalPort->phDevCopp,
                           pAdmStatSvc->admResponseQ,
                           phCalPort->unEID1.phAfePort,
                           ELITE_CMD_DISCONNECT);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DISCONNECT cmd failed, result = %d", result);
      return result;
   }

   //Send disconnect command to AFE to disconnect from up-stream service.
   result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                     phCalPort->unEID1.unAfePortID,
                                     phCalPort->unEID1.phAfePort,
                                     ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: AFE_PORT_DISCONNECT cmd failed, result = %d", result);
      return result;
   }

   // Destroy COPP
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of Rx ADM_CMD_DEVICE_CLOSE, issuing COPP_DESTROY cmd");
   qurt_elite_thread_t coppThreadID = phCalPort->phDevCopp->threadId;
   result = AdmSendCoppMsg(phCalPort->phDevCopp,
                           pAdmStatSvc->admResponseQ,
                           NULL,
                           ELITE_CMD_DESTROY_SERVICE);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DESTROY cmd failed, result = %d", result);
      return result;
   }

   // Join the COPP thread
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "ADM: As part of ADM_CMD_DEVICE_CLOSE Waiting for COPP to join");
   int jResult;
   qurt_elite_thread_join(coppThreadID, &(jResult));
   result = (ADSPResult)jResult;
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP thread join failed, result = %d", result);
      return result;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "ADM: COPP thread joined in Device in ADM_CMD_DEVICE_CLOSE ");

   // Final port structure state variable clean-up
   result = Adm_DC_CloseCommonHandler(pAdmStatSvc, phCalPort, unCoppID);

   return result;
}

ADSPResult Adm_DC_CloseRxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                AdmCalPort_Type *phCalPort,
                                uint16_t unCoppID,
                                adm_client_type_t eClient)
{
   ADSPResult result;
   if (eLSM_TYPE == eClient)
   {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
    return ADSP_EBADPARAM;
   }
   // check for connected streams and disconnect if necessary
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "As part of Rx ADM_CMD_COPP_CLOSE, sending cmd to ASM to disconnect from AFE");
   result = Adm_DC_DisconnectRxStream(pAdmStatSvc, phCalPort,eClient);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Failed to disconnect stream!");
      return result;
   }

   //Send disconnect command to AFE to disconnect from up-stream service.
   result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                     phCalPort->unEID1.unAfePortID,
                                     phCalPort->unEID1.phAfePort,
                                     ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: AFE_PORT_DISCONNECT cmd failed, result = %d", result);
      return result;
   }

   result = Adm_DC_CloseCommonHandler(pAdmStatSvc, phCalPort, unCoppID);

   return result;
}

ADSPResult Adm_DC_ConnectRxStreamToDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                          AdmCalPort_Type *phCalPort,
                                          uint16_t unCoppID,
                                          uint32_t unSessionID,
                                          adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error: Unsupported for LSM Session");
      return ADSP_EBADPARAM;
   }

   // Can connect to only one session
   if( 0 != phCalPort->unNumConnectedSessionsRx)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect multiple ULL sessions to ULL COPP ID:[%u]", unCoppID );
      return ADSP_EBADPARAM;
   }

   // Send downstream device service handle
   elite_svc_handle_t *phDevSvcHandle = NULL;
   if (ADM_ULTRA_LOW_LATENCY_POST_PROC_DEVICE_SESSION == phCalPort->unDevicePerfMode)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of matrix map routing, Send COPP handle to ASM");
      if (NULL == phCalPort->phDevCopp)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: connect to COPP failed. NULL service handle!");
         return ADSP_EFAILED;
      }
      phDevSvcHandle = phCalPort->phDevCopp;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of matrix map routing, Send AFE handle to ASM");
      phDevSvcHandle = phCalPort->unEID1.phAfePort;
   }
   uint32_t cmd = ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT;
   result = Adm_SendCmdAsmStreamConnect(pAdmStatSvc,
                                           phDevSvcHandle,
                                           unSessionID,
                                           cmd);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: cmd to connect stream failed, result = %d", result);
      return ADSP_EFAILED;
   }

   // Update session mapping mask.
   Adm_UpdateSessionMappingMask(pAdmStatSvc, unSessionID, unCoppID, ADM_MATRIX_ID_AUDIO_RX,eClient);

   // Update the Session -> Primary COPP table mapping.
   Adm_DC_AddPrimaryCoppToSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_RX, unCoppID, unSessionID,eClient);

   // Session ID corresponding to this COPP ID.
   phCalPort->unDirectConnSessionID = unSessionID;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: RX Session ID [0x%lx] --> Primary COPP ID [%u]", unSessionID, unCoppID);

   return result;
}

ADSPResult Adm_DC_DisconnectRxStreamFromDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                     AdmCalPort_Type *phCalPort,
                                     uint16_t unCoppID,
                                     uint32_t unSessionID,
                                     AdmStreamSessionInfo_Type *pSessionInfo,
                                     adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   
   if (eLSM_TYPE == eClient)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
       return ADSP_EBADPARAM;
   }

   if (1 == phCalPort->unNumConnectedSessionsRx)
   {
      // Send disconnect command to ASM to disconnect decoder service.
      // This command is required only if stream session is not already closed.
      // Checking against stream performance mode to make sure stream session is not closed already.
      if(ASM_LEGACY_STREAM_SESSION != pSessionInfo->stream_perf_mode)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
             "ADM: Sending cmd to ASM to disconnect Rx session from AFE");
         result = Adm_SendCmdAsmStreamConnect(pAdmStatSvc,
                                                 phCalPort->unEID1.phAfePort,
                                                 unSessionID,
                                                 ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_DISCONNECT);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "ADM: cmd to disconnect ASM and AFE failed, result = %d", result);
            return result;
         }

      }

      // Reset the session mapping mask which also decrements the phCalPort->unNumConnectedSessionsRx
      Adm_ResetSessionMappingMask(pAdmStatSvc, unSessionID, ADM_MATRIX_ID_AUDIO_RX, eClient);
   }

   Adm_DC_RemovePrimaryCoppFromSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_RX, unSessionID, eClient);
   phCalPort->unDirectConnSessionID = 0;
   return result;
}

ADSPResult Adm_DC_OpenTxDevice(AdmCalPort_Type *phCalPort,
                               adm_cmd_device_open_v5_t *pCoppOpenParams)
{
   ADSPResult result = ADSP_EOK;
   result = Adm_DC_ValidateDeviceOpenParams(pCoppOpenParams->endpoint_id_1,
                                            pCoppOpenParams->dev_num_channel,
                                            pCoppOpenParams->bit_width,
                                            pCoppOpenParams->sample_rate);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Tx open failed with result [%d]. Invalid open params", result);
      return result;
   }

   // Update calport with validated device open params
   phCalPort->unEID1.unAfePortID = pCoppOpenParams->endpoint_id_1;
   phCalPort->unCoppTopologyID = pCoppOpenParams->topology_id;
   phCalPort->unEID1.unNumChannels = pCoppOpenParams->dev_num_channel;
   for (uint32_t i = 0; i < phCalPort->unEID1.unNumChannels; i++)
   {
      phCalPort->unEID1.unChannelMap[i] = pCoppOpenParams->dev_channel_mapping[i];
   }
   phCalPort->unEID1.unBytesPerSample = elite_data_get_bytes_per_sample(pCoppOpenParams->bit_width);
   phCalPort->unEID1.unBitWidth = pCoppOpenParams->bit_width;
   phCalPort->unEID1.unSampleRate = pCoppOpenParams->sample_rate;
   phCalPort->unMtMxSampleRate = pCoppOpenParams->sample_rate;
   uint32_t unCoppAfeFrameSizeInSamples;
   elite_svc_get_frame_size(pCoppOpenParams->sample_rate, &unCoppAfeFrameSizeInSamples);
   phCalPort->unCoppDirection = TX_DIR;
   phCalPort->unCoppNumAfeFrames = ADM_LL_NUM_AFE_FRAMES;
   phCalPort->unCoppType = ADM_COPP_TYPE_AUDIO_COPP;
   phCalPort->unCoppStatus = ADM_COPP_STATUS_ACTIVE;
   uint32_t unBufDurationInMs = (unCoppAfeFrameSizeInSamples *(phCalPort->unCoppNumAfeFrames)*1000)/(phCalPort->unEID1.unSampleRate);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: device_perf_mode is [%u] & COPP Buffer Num Frames is [%u] & COPP Buffer Duration is [%u] ms",
				(unsigned int)phCalPort->unDevicePerfMode,(unsigned int)phCalPort->unCoppNumAfeFrames, (unsigned int)unBufDurationInMs);
   
   return result;
}

ADSPResult Adm_DC_CloseTxDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                AdmCalPort_Type *phCalPort,
                                uint16_t unCoppID,
                                adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   
   if (eLSM_TYPE == eClient)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
       return ADSP_EBADPARAM;
   }

   // If map routing was called then we need to tear down the stream/device connection
   if (1 == phCalPort->unNumConnectedSessionsTx)
   {
      result = Adm_DC_DisconnectTxStreamFromDevice(pAdmStatSvc,
                                                   phCalPort,
                                                   unCoppID,
                                                   phCalPort->unDirectConnSessionID, eClient);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "ADM: failed to disconnect stream from device, result = %d", result);
         return result;
      }
   }

   // Reset the calport
   Adm_ResetCalPort(phCalPort);

   // Reset the session delay parameters
   AdmDevSessionDelayParam_Type *pSessionDelayParam;
   pSessionDelayParam = &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);
   pSessionDelayParam->unCoppAlgDelay = 0;
   pSessionDelayParam->unCoppBufDelay = 0;
   pSessionDelayParam->unAFEDelay_EP1 = 0;
   pSessionDelayParam->unAFEDelay_EP2 = 0;
   pSessionDelayParam->unAFEDelay_EP3 = 0;

   // Detect primary device of all active streams and devices.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of ADM_CMD_COPP_CLOSE, updating primary device/port");
   if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDevicePcm(pAdmStatSvc)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: updating primary device/port failed, result = %d", result);
      return result;
   }

   return result;
}

ADSPResult Adm_DC_ConnectTxStreamToDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                          AdmCalPort_Type *phCalPort,
                                          uint16_t unCoppID,
                                          uint32_t unSessionID,
                                          adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   
   if (eLSM_TYPE == eClient)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error: Unsupported for LSM");
       return ADSP_EBADPARAM;
   }

   // query for the downstream service handle (the sink for Tx path)
   elite_svc_handle_t *phSinkHandle = NULL;
   result = Adm_SendCmdQueryAsmTxSinkHandle(pAdmStatSvc, unSessionID, &phSinkHandle);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: Failed to get the downstream service handle with result 0x%x", result);
      return result;
   }

   // Tell the AFE to connect to the sink.
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Tx COPP [%u]. Connecting handle 0x%p to AFE",
         unCoppID, phSinkHandle);
   AdmDevSessionDelayParam_Type *pSessionDelayParam;
   pSessionDelayParam = &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);
   result = Adm_SendCmdConnectAFE(pAdmStatSvc,
                                  phCalPort->unEID1.unAfePortID,
                                  phSinkHandle,
                                  AFE_PP_OUT,
                                  &(phCalPort->unEID1.phAfePort),
                                  phCalPort->unEID1.unSampleRate,
                                  phCalPort->unEID1.unNumChannels,
                                  phCalPort->unEID1.unChannelMap,
                                  phCalPort->unCoppNumAfeFrames,
                                  ADM_HIGH_BUFFERING_BETWEEN_AFE_AND_CLIENT,
                                  ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                  phCalPort->unEID1.unBitWidth,
                                  &(phCalPort->unEID1.pAfeDriftPtr),
                                  &(pSessionDelayParam->unAFEDelay_EP1),
                                  FALSE);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: AFE_CONNECT_REQ cmd failed, result = %d", result);
      return result;
   }

   // Update the session mapping mask and ADM Bookkeeping
   Adm_UpdateSessionMappingMask(pAdmStatSvc, unSessionID, unCoppID, ADM_MATRIX_ID_AUDIO_TX,eClient);
   Adm_DC_AddPrimaryCoppToSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_TX, unCoppID, unSessionID,eClient);
   phCalPort->unDirectConnSessionID = unSessionID;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: Primary COPP ID [%u] --> TX Session ID [0x%lx]", unCoppID, unSessionID);
   return result;
}

ADSPResult Adm_DC_DisconnectTxStreamFromDevice(AdmStatSvc_InfoType *pAdmStatSvc,
                                               AdmCalPort_Type *phCalPort,
                                               uint16_t unCoppID,
                                               uint32_t unSessionID,
                                               adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EOK;
   
   if (eLSM_TYPE == eClient)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM Session");
       return ADSP_EBADPARAM;
   }

   // The AFE handle is stored only on AFE connect during map routing for Tx path
   // Therefore, if it is non-NULL then we must tear down the connection
   if (NULL != phCalPort->unEID1.phAfePort)
   {
      // The name is misleading, it's actually sending a disable to the AFE port.
      // We can safely issue disable without having to worry if the AFE is disabled already
      result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                        phCalPort->unEID1.unAfePortID,
                                        phCalPort->unEID1.phAfePort,
                                        ELITEMSG_CUSTOM_AFECLIENTDISABLE);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "ADM: Failed to disable the AFE with result 0x%x", result);
         return result;
      }

      // If the ASM session is still open
      if (0 != phCalPort->unDirectConnSessionID)
      {
         // query for the downstream service handle (sink)
         elite_svc_handle_t *phSinkHandle = NULL;
         result = Adm_SendCmdQueryAsmTxSinkHandle(pAdmStatSvc,
                                                  phCalPort->unDirectConnSessionID,
                                                  &phSinkHandle);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "ADM: Failed to get the downstream service handle with result 0x%x", result);
            return result;
         }

         // It is possible that the stream was closed first. If this was the case the following
         // calls were performed on the ASM side.
         if (NULL != phSinkHandle)
         {
            result = Adm_SendCmdAsmPrepDisconnect(pAdmStatSvc, unSessionID);
            if (ADSP_FAILED(result))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                     "ADM: Cmd to prepare ASM session %lu for disconnect failed", unSessionID);
               return result;
            }
         }
      }

      // Disconnect the AFE
      result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                        phCalPort->unEID1.unAfePortID,
                                        phCalPort->unEID1.phAfePort,
                                        ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "ADM: Failed to disconnect the AFE with result 0x%x", result);
         return result;
      }

      // Since we are no longer connected to the AFE clear the handle and drift pointer
      phCalPort->unEID1.phAfePort = NULL;
      phCalPort->unEID1.pAfeDriftPtr = NULL;
   }

   // Reset session mapping mask which decrements the number of connected sessions
   Adm_ResetSessionMappingMask(pAdmStatSvc, unSessionID, ADM_MATRIX_ID_AUDIO_TX,eClient);
   Adm_DC_RemovePrimaryCoppFromSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_TX, unSessionID,eClient);
   phCalPort->unDirectConnSessionID = 0;

   return result;
}

ADSPResult Adm_DC_ReconnectAfeToCopp(AdmStatSvc_InfoType *pAdmStatSvc,
                                     AdmCalPort_Type *phCalPort,
                                     uint16_t unCoppID)
{
   ADSPResult result = ADSP_EFAILED;

   result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                     phCalPort->unEID1.unAfePortID,
                                     phCalPort->unEID1.phAfePort,
                                     ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: AFE_PORT_DISCONNECT cmd failed, result = %d", result);
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: Reconnecting COPP [%d] to AFE. issuing AFE_CONNECT_REQ cmd, EP1: %d",
         unCoppID,
         phCalPort->unEID1.unAfePortID);
   phCalPort->unEID1.phAfePort = NULL;
   AdmDevSessionDelayParam_Type *pSessionDelayParam;
   pSessionDelayParam = &(pAdmStatSvc->pDevSessionDelayParam[unCoppID]);
   if (ADSP_FAILED(result = Adm_SendCmdConnectAFE(pAdmStatSvc,
               phCalPort->unEID1.unAfePortID,
               NULL,
               AFE_RX_AUD_MIXER_IN,
               &(phCalPort->unEID1.phAfePort),
               phCalPort->unEID1.unSampleRate,
               phCalPort->unEID1.unNumChannels,
               &phCalPort->unEID1.unChannelMap[0],
               phCalPort->unCoppNumAfeFrames,
               ADM_NORMAL_BUFFERING_BETWEEN_AFE_AND_CLIENT,
               ELITEMSG_CUSTOM_AFECONNECT_REQ,
               phCalPort->unEID1.unBitWidth,
               &(phCalPort->unEID1.pAfeDriftPtr),
               &(pSessionDelayParam->unAFEDelay_EP1),
               FALSE)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: AFE_CONNECT_REQ cmd failed, result = %d", result);
      return result;

   }

   return result;
}

void Adm_DC_SendMediaFormat(AdmStatSvc_InfoType *pAdmStatSvc,
                            AdmCalPort_Type *phCalPort,
                            uint16_t unCoppID,
                            AdmStreamSessionInfo_Type *pSessionInfo,
                            uint16_t unSessionID)
{
   ADSPResult result = ADSP_EFAILED;

   // check if a device and stream are in are in ULLPP mode
   bool_t bIsCoppAvailable = FALSE;
   bool_t bIsPoppAvailable = FALSE;
   if ((phCalPort->phDevCopp != NULL) &&
         (phCalPort->unDevicePerfMode == ADM_ULTRA_LOW_LATENCY_POST_PROC_DEVICE_SESSION))
   {
      bIsCoppAvailable = TRUE;
   }
   if (pSessionInfo->stream_perf_mode == ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION)
   {
      bIsPoppAvailable = TRUE;
   }

   if (!(bIsCoppAvailable) && !(bIsPoppAvailable))
   {
      // neither of the PPs are available
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ADM: PP services not available for SSID 0x%lx, COPP #%lu. Will not send media format...",
            unSessionID, unCoppID);
      return;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: sending media format information to services. sampling rate: %lu, # of ch: %lu, bytes per sample: %lu",
         phCalPort->unEID1.unSampleRate, phCalPort->unEID1.unNumChannels, phCalPort->unEID1.unBytesPerSample);

   // ***
   // Step 1. Try to set the sampling rate on the POPP falling back to the COPP in case of failure.
   // ***
   // A sampling rate of 0Hz indicates an invalid sampling rate.
   uint32_t unSessionSamplingRate = 0;
   uint32_t unDeviceSamplingRate = 0;
   if (bIsPoppAvailable)
   {
      result = Adm_SendCmdAsmSetOutputSamplingRate(pAdmStatSvc,
                                                   phCalPort->unEID1.unSampleRate,
                                                   unSessionID);
      if (ADSP_FAILED(result) && bIsCoppAvailable)
      {
         // fall back to the COPP
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "WARN: cannot set the sampling rate on session [0x%x]. Falling back to COPP [0x%x].",
               unSessionID, unCoppID);

         result = Adm_SendCmdCoppSetOutputSamplingRate(phCalPort->phDevCopp,
                                                       pAdmStatSvc->admResponseQ,
                                                       phCalPort->unEID1.unSampleRate);
         unDeviceSamplingRate = (ADSP_SUCCEEDED(result)) ? phCalPort->unEID1.unSampleRate : 0;
      }
      else
      {
         unSessionSamplingRate = phCalPort->unEID1.unSampleRate;
      }
   }
   else if (bIsCoppAvailable)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "POPP not available for session [0x%x]. Setting sampling rate on COPP [0x%x].",
            unSessionID, unCoppID);

      result = Adm_SendCmdCoppSetOutputSamplingRate(phCalPort->phDevCopp,
                                                    pAdmStatSvc->admResponseQ,
                                                    phCalPort->unEID1.unSampleRate);
      unDeviceSamplingRate = (ADSP_SUCCEEDED(result)) ? phCalPort->unEID1.unSampleRate : 0;
   }
   else
   {
      result = ADSP_EFAILED;
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "WARNING: could not set sampling rate. Continuing...");
      result = ADSP_EOK;
   }

   // ***
   // Step 2. Set the ch mapping and bw on the COPP falling back to the POPP in case of failure.
   // ***
   uint32_t unBitsPerSample = phCalPort->unEID1.unBitWidth;

   if (bIsCoppAvailable)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Setting channel and bitwidth params on COPP [0x%x].", unCoppID);
      result = adm_config_pp_output_media_format(phCalPort->phDevCopp,
                                                 pAdmStatSvc->admResponseQ,
                                                 phCalPort->unEID1.unNumChannels,
                                                 phCalPort->unEID1.unChannelMap,
                                                 unDeviceSamplingRate,
                                                 unBitsPerSample);
      // fall back to the POPP
      if (ADSP_FAILED(result) && bIsPoppAvailable)
      {
         MSG_2(MSG_SSID_QDSP6,
               DBG_HIGH_PRIO,
               "WARN: cannot set media format on COPP [0x%x]. Falling back to POPP in ASM session [0x%x].",
               unCoppID,
               unSessionID);

         result = Adm_SendCmdAsmSetOutputMediaFormat(pAdmStatSvc,
                                                     unSessionID,
                                                     unSessionSamplingRate,
                                                     phCalPort->unEID1.unNumChannels,
                                                     phCalPort->unEID1.unChannelMap,
                                                     unBitsPerSample);
      }
   }
   else if (bIsPoppAvailable)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "COPP not available for device [0x%x]. Setting media format on POPP in session [0x%x].",
            unCoppID, unSessionID);
      result = Adm_SendCmdAsmSetOutputMediaFormat(pAdmStatSvc,
                                                  unSessionID,
                                                  unSessionSamplingRate,
                                                  phCalPort->unEID1.unNumChannels,
                                                  phCalPort->unEID1.unChannelMap,
                                                  unBitsPerSample);
   }
   else
   {
      result = ADSP_EFAILED;
   }

   /*
   if (isPoppAvailable)
   {
      result = Adm_SendCmdAsmSetOutputSamplingRate(pAdmStatSvc,
                                                   phCalPort->unEID1.unSampleRate,
                                                   unSessionID);
      if (ADSP_FAILED(result) && isCoppAvailable)
      {
         // fall back to the COPP
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "WARN: cannot set the sampling rate on session [0x%x]. Falling back to COPP [0x%x].",
               unSessionID, unCoppID);

         result = Adm_SendCmdCoppSetOutputSamplingRate(phCalPort->phDevCopp,
                                                       pAdmStatSvc->admResponseQ,
                                                       phCalPort->unEID1.unSampleRate);
      }
   }
   else // checks above guarantee that either the COPP or POPP are available.
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "POPP not available for session [0x%x]. Setting sampling rate on COPP [0x%x].",
            unSessionID, unCoppID);

      result = Adm_SendCmdCoppSetOutputSamplingRate(phCalPort->phDevCopp,
                                                    pAdmStatSvc->admResponseQ,
                                                    phCalPort->unEID1.unSampleRate);
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "WARNING: could not set sampling rate. Continuing...");
      result = ADSP_EOK;
   }

   // ***
   // Step 2. Set the ch mapping and bw on the COPP falling back to the POPP in case of failure.
   // ***
   if (isCoppAvailable)
   {

      result = Adm_SendCmdCoppSetOutputChannelAndBitwidthParams(phCalPort->phDevCopp,
                                                                pAdmStatSvc->admResponseQ,
                                                                phCalPort->unEID1.unNumChannels,
                                                                phCalPort->unEID1.unChannelMap,
                                                                phCalPort->unEID1.unBytesPerSample);
      // fall back to the POPP
      if (ADSP_FAILED(result) && isPoppAvailable)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "WARN: cannot set media type on COPP [0x%x]. Falling back to POPP in ASM session [0x%x].",
               unCoppID, unSessionID);

         result = Adm_SendCmdAsmSetOutputChannelAndBitwidthParams(pAdmStatSvc,
                                                                  unSessionID,
                                                                  phCalPort->unEID1.unNumChannels,
                                                                  phCalPort->unEID1.unChannelMap,
                                                                  phCalPort->unEID1.unBytesPerSample);
      }
   }
   else if (isPoppAvailable)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "COPP not available for device [0x%x]. Setting media type on POPP in session [0x%x].",
            unCoppID, unSessionID);
      result = Adm_SendCmdAsmSetOutputChannelAndBitwidthParams(pAdmStatSvc,
                                                               unSessionID,
                                                               phCalPort->unEID1.unNumChannels,
                                                               phCalPort->unEID1.unChannelMap,
                                                               phCalPort->unEID1.unBytesPerSample);
   }
   else
   {
      result = ADSP_EFAILED;
   }

   */

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "WARNING: could not set remaining media format. Continuing...");
      result = ADSP_EOK;
   }

   return;
}


/* =======================================================================
Public Function Definitions
========================================================================== */
ADSPResult Adm_DirectStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                EliteMsg_CustomAdmStreamConnectType *pPayload,
                                adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: LLNP Stream Connect received for LSM service. Unsupported Combination!");
	   return ADSP_EUNSUPPORTED;
   }
   ADSPResult result = ADSP_EOK;
   if (NULL == pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Direct Stream Connect received NULL payload ptr");
      return ADSP_EFAILED;
   }

   //check if session is compressed stream session
   if (TRUE == pPayload->is_compressed)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: Direct stream connections do not support compressed data! session ID:[0x%lX]",
            pPayload->unSessionID);
      return ADSP_EFAILED;
   }

   if (ADM_RX_DIR !=  pPayload->direction)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: Invalid ULL Stream session ID:[0x%lX]! direction (TX)", pPayload->unSessionID);
      return ADSP_EFAILED;
   }

   //Allocate an index for this connection
   uint16_t unIndex = Adm_CalculateNextAvlIndex(pAdmStatSvc, ADM_RX_DIR, eClient);
   if (ADM_INVALID_INDEX == unIndex)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Error Adm_DirectStreamConnect: No more valid indices available.");
      result = ADSP_ENORESOURCE;
      return result;
   }

   //Associate this index with this session/stream ID and grab hold of pSessionInfo.
   AdmStreamSessionInfo_Type *pSessionInfo;
   pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
   if (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pPayload->stream_perf_mode)
   {
      pSessionInfo->stream_perf_mode = ASM_ULTRA_LOW_LATENCY_STREAM_SESSION;
   }
   else if (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pPayload->stream_perf_mode)
   {
      pSessionInfo->stream_perf_mode = ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION;
   }
   else
   {
      return ADSP_EBADPARAM;
   }
   pAdmStatSvc->rxIndexToSessionStreamID[unIndex] = (uint16_t)(pPayload->unSessionID);
   pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
   pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
   pPayload->phPort = (elite_svc_handle_t *)DUMMY_HANDLE;

   return result;
}

ADSPResult Adm_DirectStreamDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                      EliteMsg_CustomAdmStreamConnectType *pPayload, adm_client_type_t eClient)
{
   ADSPResult result = ADSP_EFAILED;
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }
   if (NULL == pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Direct Stream Disconnect received NULL payload ptr");
      return ADSP_EFAILED;
   }

   //check if session is compressed stream session
   if (TRUE == pPayload->is_compressed)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: Direct Stream session ID:[0x%lX] does not support compressed data!",
            pPayload->unSessionID);
      return ADSP_EFAILED;
   }

   if (ADM_RX_DIR != pPayload->direction)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: Invalid Direct Stream session ID:[0x%lX] direction (TX)",
            pPayload->unSessionID);
      return ADSP_EFAILED;
   }

   uint32_t sessionID = pPayload->unSessionID;
   uint16_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_RX_DIR, (uint16_t)(sessionID), eClient);
   if(ADM_INVALID_INDEX == unIndex)
   {
      result = ADSP_EBADPARAM;
      return result;
   }

   // Disconnect the stream if connected to a device
   if (0 != pAdmStatSvc->rxsessionMappingMask[unIndex])
   {
      uint16_t unCoppId = pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex];
      AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppId];

      // remove the session from the CalPort structure
      Adm_DC_RemovePrimaryCoppFromSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_RX, sessionID, eClient);
      phCalPort->unDirectConnSessionID = 0;

      if (ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION == phCalPort->unDevicePerfMode)
      {
         // disconnect/reconnect to the AFE - this releases any held buffers if the AFE is stopped.
         // The connection between "COPP" and AFE is maintained by reconnecting
         (void)Adm_DC_ReconnectAfeToCopp(pAdmStatSvc, phCalPort, unCoppId);
      }
      else // ULLPP device case
      {
         // Flush the COPP before disconnecting so the COPP will return its buffers upstream.
         (void)Adm_FlushCopp(pAdmStatSvc, phCalPort, unCoppId);
      }
   }

   //Reset stream session Info
   AdmStreamSessionInfo_Type *pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
   pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
   pSessionInfo->stream_perf_mode = ASM_LEGACY_STREAM_SESSION;

   //Reset session mapping mask.
   Adm_ResetSessionMappingMask(pAdmStatSvc, sessionID, ADM_RX_DIR, eClient);

   // Detect primary device of all active streams and devices.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of direct stream disconnect, updating primary device/port");
   if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDevicePcm(pAdmStatSvc)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: updating primary device/port failed, result = %d", result);
      return result;
   }

   //Reset this index mapping for this closed session/stream
   pAdmStatSvc->rxIndexToSessionStreamID[unIndex] = 0;
   pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;

   return result;
}

ADSPResult Adm_UllppMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                   elite_apr_packet_t *pPkt,
                                                   uint16_t unCoppID)
{   
	ADSPResult  result = ADSP_EOK;
	AdmCalPort_Type  *phCalPort;
	adm_cmd_device_open_v5_t     *pCoppOpenCmdPayload;

   if ((NULL == pPkt) || (ADM_MAX_COPPS <= unCoppID))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ULLPP Open received bad params");
      return ADSP_EFAILED;
   }

	pCoppOpenCmdPayload = (adm_cmd_device_open_v5_t *)elite_apr_if_get_payload_ptr(pPkt);
	phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

   // Reinit calport
   Adm_ResetCalPort(phCalPort);

   // Open the COPP
	if (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_RX_PATH_COPP != pCoppOpenCmdPayload->mode_of_operation)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid mode_of_operation [%u], ULLPP supports only playback/record",
				pCoppOpenCmdPayload->mode_of_operation);
		result = ADSP_EBADPARAM;
      goto __bailout;
	}

   phCalPort->unDevicePerfMode =  ADM_ULTRA_LOW_LATENCY_POST_PROC_DEVICE_SESSION;
   result = Adm_DC_OpenUllppRxDevice(pAdmStatSvc, phCalPort, unCoppID, pCoppOpenCmdPayload);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: failed to open Rx COPP [%u]", unCoppID);
      goto __bailout;
   }


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Opened COPP [%u]. ULLPP COPP", unCoppID);
   AudDevMgr_RequestHwResources(pAdmStatSvc);
	return result;

__bailout:
   // on failure clear any modifications made to the calport
   Adm_ResetCalPort(phCalPort);
	elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
	return result;
}

ADSPResult Adm_DirectMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                         adm_session_map_node_v5_t *pSessionMapNode,
                                         uint32_t matrixID,
                                         adm_client_type_t eClient)
{
  if (eLSM_TYPE == eClient)
  {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }
	ADSPResult result = ADSP_EOK;
	uint16_t unCoppID, unPrevCoppID, unIndex = ADM_INVALID_INDEX;
	AdmCalPort_Type *phCalPort, *phCalPrevPort;

	if (NULL == pSessionMapNode)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Map Routings received NULL session map node");
      return ADSP_EFAILED;
   }

   // ULLPP supports only one copp per stream session (SSSD)
	if (1 != pSessionMapNode->num_copps)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect direct stream session:[0x%hX] to multiple COPPs:[%u]",
				pSessionMapNode->session_id, pSessionMapNode->num_copps);
		return ADSP_EBADPARAM;
	}

   // ULLPP supports only playback
	if (ADM_MATRIX_ID_AUDIO_RX != matrixID)
	{
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ULL/ULLPP is not supported in Tx direction");
      return ADSP_EBADPARAM;
   }

   // check for validity of COPP ID
   uint16_t *pCoppID = (uint16_t*)(pSessionMapNode + 1);
   unCoppID = *pCoppID;
   if (ADSP_FAILED(result = Adm_CheckCoppValidity(pAdmStatSvc,unCoppID,ADM_INVALID_DIRECTION)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid/inactive COPP ID [%d]", unCoppID);
      return result;
   }
   phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

   // Check COPP direction.
   if (phCalPort->unCoppDirection != matrixID)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid direction for COPP ID [%d], COPP Dir=%d, MtMx Dir=%lu",
            unCoppID, phCalPort->unCoppDirection, matrixID);
      return ADSP_EBADPARAM;
   }

   // check for validity of copp performance mode. COPP should be ULL/ULLPP COPP
   if ((ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION != phCalPort->unDevicePerfMode)
         && (ADM_ULTRA_LOW_LATENCY_POST_PROC_DEVICE_SESSION != phCalPort->unDevicePerfMode))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect direct stream session:[0x%hX] to COPP with unsupported PerfMode:[%u]",
            pSessionMapNode->session_id, phCalPort->unDevicePerfMode);
		return ADSP_EBADPARAM;
	}

   // check if this session is already connected to other COPP.
   // if connected, disconnect it as direct stream session can connect to only one COPP
	unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc,
	                                     ADM_MATRIX_ID_AUDIO_RX,
	                                     pSessionMapNode->session_id, eClient);
	if(ADM_INVALID_INDEX == unIndex)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: COPP index not found!");
		return ADSP_EBADPARAM;
	}

   AdmStreamSessionInfo_Type *pSessionInfo;
   pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
	if (0 != pAdmStatSvc->rxsessionMappingMask[unIndex])
	{
      unPrevCoppID = pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex];
      phCalPrevPort = &pAdmStatSvc->deviceCalPort[unPrevCoppID];
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Disconnecting previously connected stream from COPP [%u]", unPrevCoppID);
      result = Adm_DC_DisconnectRxStreamFromDevice(pAdmStatSvc,
                                                   phCalPrevPort,
                                                   unPrevCoppID,
                                                   pSessionMapNode->session_id,
                                                   pSessionInfo,
                                                   eClient);
	   if (ADSP_FAILED(result))
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Error: failed to disconnect stream from Rx COPP [%u]", unPrevCoppID);
	      return result;
	   }
	}

   // Send media type info
   Adm_DC_SendMediaFormat(pAdmStatSvc,
                          phCalPort,
                          unCoppID,
                          pSessionInfo,
                          pSessionMapNode->session_id);

	// Connect the stream to device
   result = Adm_DC_ConnectRxStreamToDevice(pAdmStatSvc,
                                           phCalPort,
                                           unCoppID,
                                           pSessionMapNode->session_id,
                                           eClient);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: failed to connect stream [%u] to Rx COPP [%u]",
            pSessionMapNode->session_id, unCoppID);
      return result;
   }

	return result;
}

ADSPResult Adm_UllppDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                        elite_apr_packet_t *pPkt,
                                        uint16_t unCoppID,
                                        adm_client_type_t eClient)
{
  if (eLSM_TYPE == eClient)
  {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
       elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, ADSP_EBADPARAM);
       return ADSP_EBADPARAM;
  }
	ADSPResult result = ADSP_EOK;
	AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

	// Check COPP direction.
	if (RX_DIR != phCalPort->unCoppDirection)
	{
		result = ADSP_EBADPARAM;
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid direction for ULLPP COPP ID [%d], COPP Dir=%d",
				unCoppID, phCalPort->unCoppDirection);
		elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

	// Check if we are connected to multiple sessions
	if (!((1 == phCalPort->unNumConnectedSessionsRx) || (0 == phCalPort->unNumConnectedSessionsRx)))
	{
		result = ADSP_EBADPARAM;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ULLPP COPP [%u] is connected to multiple sessions", unCoppID);
		elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

   result = Adm_DC_CloseUllppRxDevice(pAdmStatSvc, phCalPort, unCoppID, eClient);
	if (ADSP_FAILED(result))
	{
	   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ULLPP Rx COPP [%u] failed to disconnect w/ result %d", unCoppID, result);
      elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
      return result;
	}

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: ULLPP Rx COPP [%u] closed.", unCoppID);
   AudDevMgr_RequestHwResources(pAdmStatSvc);
	elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
	return result;
}

ADSPResult Adm_UllMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                   elite_apr_packet_t *pPkt,
                                                   uint16_t unCoppID)
{
	ADSPResult  result = ADSP_EOK;
	AdmCalPort_Type  *phCalPort;
	adm_cmd_device_open_v5_t     *pCoppOpenCmdPayload;

   if ((NULL == pPkt) || (ADM_MAX_COPPS <= unCoppID))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ULL Open received bad params");
      return ADSP_EFAILED;
   }

	/* COPP open payload pointer*/
	pCoppOpenCmdPayload = (adm_cmd_device_open_v5_t *)elite_apr_if_get_payload_ptr(pPkt);

	/* Pointer to calport structure (copp metadata) */
	phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

   // Reinit calport
   Adm_ResetCalPort(phCalPort);

	/* Check the mode_of_operation. ULL only supports RX */
	if (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_RX_PATH_COPP != pCoppOpenCmdPayload->mode_of_operation)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
		      "Error: ADM rcvd invalid mode_of_operation [%u], ULL supports only playback",
				pCoppOpenCmdPayload->mode_of_operation);
		result = ADSP_EBADPARAM;
		goto __bailoutUllMultiChannelCoppOpen;
	}
   phCalPort->unDevicePerfMode =  ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION;

   /* Attempt to open the device. */
   result = Adm_DC_OpenRxDevice(pAdmStatSvc, phCalPort, unCoppID, pCoppOpenCmdPayload);
   if (ADSP_FAILED(result))
	{
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM failed to open ULL COPP!");
      goto __bailoutUllMultiChannelCoppOpen;
	}
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Opened COPP [%u]. ULL COPP", unCoppID);

   AudDevMgr_RequestHwResources(pAdmStatSvc);

	return result;

__bailoutUllMultiChannelCoppOpen:
	/* Failed to create ULL COPP RETURN ERROR */
	elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
	return result;
}

ADSPResult Adm_UllDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                        elite_apr_packet_t *pPkt,
                                        uint16_t unCoppID,
                                        adm_client_type_t eClient)
{
	ADSPResult result = ADSP_EOK;
	AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];
  
  if (eLSM_TYPE == eClient)
  {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
       elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, ADSP_EBADPARAM);
       return ADSP_EBADPARAM;
  }

	// Check COPP direction.
	if (RX_DIR != phCalPort->unCoppDirection)
   {
		result = ADSP_EBADPARAM;
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
		      "Error: ADM rcvd invalid direction for ULL COPP ID [%d], COPP Dir=%d",
				unCoppID, phCalPort->unCoppDirection);
		elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
   }

	// Check if we are connected to multiple sessions
	if (!((1 == phCalPort->unNumConnectedSessionsRx) || (0 == phCalPort->unNumConnectedSessionsRx)))
	{
		result = ADSP_EBADPARAM;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ULL COPP [%u] is connected to multiple sessions", unCoppID);
		elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}
   
	result = Adm_DC_CloseRxDevice(pAdmStatSvc, phCalPort, unCoppID, eClient);
	if (ADSP_FAILED(result))
   {
	   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
	         "Error: ULL Rx COPP [%u] failed to disconnect w/ result %d", unCoppID, result);
      elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
      return result;
   }

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: ULL Rx COPP [%u] closed.", unCoppID);
   AudDevMgr_RequestHwResources(pAdmStatSvc);
	elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
	return result;
}

ADSPResult Adm_LlnpMultiChannelDeviceOpenCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                                    elite_apr_packet_t *pPkt,
                                                    uint16_t unCoppID)
{
   ADSPResult  result = ADSP_EOK;
   AdmCalPort_Type  *phCalPort;
   adm_cmd_device_open_v5_t     *pCoppOpenCmdPayload;

   if ((NULL == pPkt) || (ADM_MAX_COPPS <= unCoppID))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: LLNP Open received bad params");
      return ADSP_EFAILED;
   }

   // COPP open payload pointer
   pCoppOpenCmdPayload = (adm_cmd_device_open_v5_t *)elite_apr_if_get_payload_ptr(pPkt);

   // Pointer to calport structure (copp metadata)
   phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

   // Reinit calport
   Adm_ResetCalPort(phCalPort);

   // Open the COPP
   phCalPort->unDevicePerfMode = ADM_LOW_LATENCY_NO_PROC_DEVICE_SESSION;
   if (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_RX_PATH_COPP == pCoppOpenCmdPayload->mode_of_operation)
   {
      result = Adm_DC_OpenRxDevice(pAdmStatSvc, phCalPort, unCoppID, pCoppOpenCmdPayload);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: failed to open Rx COPP [%u]", unCoppID);
         goto __bailout;
      }
   }
   else if ((ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_LIVE_COPP ==
               pCoppOpenCmdPayload->mode_of_operation) ||
            (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_NON_LIVE_COPP ==
               pCoppOpenCmdPayload->mode_of_operation))
   {
      result = Adm_DC_OpenTxDevice(phCalPort, pCoppOpenCmdPayload);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: failed to open Tx COPP [%u]", unCoppID);
         goto __bailout;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid mode_of_operation [%u], LLNP supports only playback/record",
            pCoppOpenCmdPayload->mode_of_operation);
      result = ADSP_EBADPARAM;
      goto __bailout;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Opened LLNP COPP [%u] w/ mode %u.", unCoppID,
         pCoppOpenCmdPayload->mode_of_operation);
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: sampling rate %lu, bits per sample %u, #ch %u",
         pCoppOpenCmdPayload->sample_rate, pCoppOpenCmdPayload->bit_width,
         pCoppOpenCmdPayload->dev_num_channel);

   AudDevMgr_RequestHwResources(pAdmStatSvc);

   return result;

__bailout:
   // on failure clear any modifications made to the calport
   Adm_ResetCalPort(phCalPort);
   elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);

   return result;
}

ADSPResult Adm_LlnpMtMxMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                          adm_session_map_node_v5_t *pSessionMapNode,
                                          uint32_t matrixID, adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }
   ADSPResult result = ADSP_EOK;
   uint16_t unCoppID, unPrevCoppID, unIndex = ADM_INVALID_INDEX;
   AdmCalPort_Type *phCalPort = NULL, *phCalPrevPort = NULL;

   if (NULL == pSessionMapNode)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: LLNP Map Routings received NULL session map node");
      return ADSP_EFAILED;
   }

   // LLNP supports only one copp per stream session (SSSD)
   if (1 != pSessionMapNode->num_copps)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect LLNP session:[0x%hX] to multiple COPPs:[%u]",
            pSessionMapNode->session_id, pSessionMapNode->num_copps);
      return ADSP_EBADPARAM;
   }

   // Check if the session ID maps to a valid COPP index
   unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, matrixID, pSessionMapNode->session_id, eClient);
   if(ADM_INVALID_INDEX == unIndex)
   {
      return ADSP_EBADPARAM;
   }

   // Get the appropriate session mapping mask and primary COPP ID map depending if Rx/Tx direction
   if (ADM_MATRIX_ID_AUDIO_RX == matrixID)
   {
      if (0 != pAdmStatSvc->rxsessionMappingMask[unIndex])
      {
         unPrevCoppID = pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex];
         phCalPrevPort = &pAdmStatSvc->deviceCalPort[unPrevCoppID];
         AdmStreamSessionInfo_Type *pSessionInfo;
         pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "Disconnecting previously connected Rx stream from COPP [%u]", unPrevCoppID);
         result = Adm_DC_DisconnectRxStreamFromDevice(pAdmStatSvc,
                                                      phCalPrevPort,
                                                      unPrevCoppID,
                                                      pSessionMapNode->session_id,
                                                      pSessionInfo,
                                                      eClient);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: failed to disconnect LLNP Rx COPP [%u]",
                  unPrevCoppID);
            return result;
         }
      }
   }
   else
   {
      if (0 != pAdmStatSvc->txsessionMappingMask[unIndex])
      {
         unPrevCoppID = pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex];
         phCalPrevPort = &pAdmStatSvc->deviceCalPort[unPrevCoppID];
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "Disconnecting previously connected Tx stream from COPP [%u]", unPrevCoppID);
         result = Adm_DC_DisconnectTxStreamFromDevice(pAdmStatSvc,
                                                      phCalPrevPort,
                                                      unPrevCoppID,
                                                      pSessionMapNode->session_id,
                                                      eClient);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Error: failed to disconnect stream from LLNP Tx COPP [%u]", unPrevCoppID);
            return result;
         }
      }
   }

   uint8_t *pTmp = (uint8_t *)pSessionMapNode;
   uint16_t *pCoppID = (uint16_t*)(pTmp + sizeof(adm_session_map_node_v5_t));
   unCoppID = *pCoppID;

   // check for validity of COPP ID
   if (ADSP_FAILED(result = Adm_CheckCoppValidity(pAdmStatSvc,unCoppID,ADM_INVALID_DIRECTION)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid/inactive COPP ID [%d]", unCoppID);
      return ADSP_EBADPARAM;
   }

   phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];
   // Check COPP direction.
   if (phCalPort->unCoppDirection != matrixID)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: ADM rcvd invalid direction for COPP ID [%d], COPP Dir=%d, MtMx Dir=%lu",
            unCoppID, phCalPort->unCoppDirection, matrixID);
      return ADSP_EBADPARAM;
   }

   // check for validity of copp performance mode. COPP should be LLNP copp
   if (ADM_LOW_LATENCY_NO_PROC_DEVICE_SESSION != phCalPort->unDevicePerfMode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect LLNP session:[0x%hX] to non-LLNP COPP, CoppPerfMode:[%u]",
            pSessionMapNode->session_id, phCalPort->unDevicePerfMode);
      return ADSP_EBADPARAM;
   }

   // LLNP copp can connect to only one LLNP session
   if (((ADM_RX_DIR == phCalPort->unCoppDirection) && (0 != phCalPort->unNumConnectedSessionsRx)) || 
       ((ADM_TX_DIR == phCalPort->unCoppDirection) && (0 != phCalPort->unNumConnectedSessionsTx)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: attempt to connect multiple LLNP sessions to LLNP COPP ID %u", unCoppID );
      return ADSP_EUNSUPPORTED;
   }

   // Connnect the ASM and AFE
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of LLNP matrix map routing, connecting stream and device");
   if (ADM_MATRIX_ID_AUDIO_RX == matrixID)
   {
      // Connect Rx stream to device
      result = Adm_DC_ConnectRxStreamToDevice(pAdmStatSvc,
                                              phCalPort,
                                              unCoppID,
                                              pSessionMapNode->session_id,
                                              eClient);
   }
   else
   {
      // Connect the Tx stream to device
      result = Adm_DC_ConnectTxStreamToDevice(pAdmStatSvc,
                                              phCalPort,
                                              unCoppID,
                                              pSessionMapNode->session_id,
                                              eClient);
   }
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: cmd to connect ASM and AFE failed, result = %d", result);
      return ADSP_EFAILED;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: LLNP Map Routing Successful");

   return result;
}

ADSPResult Adm_LlnpDeviceCloseCmdHandler(AdmStatSvc_InfoType *pAdmStatSvc,
                                         elite_apr_packet_t *pPkt,
                                         uint16_t unCoppID,
                                         adm_client_type_t eClient)
{
   if ((NULL == pPkt) || (ADM_MAX_COPPS <= unCoppID))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: LLNP Close received bad params");
      return ADSP_EFAILED;
   }
   
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }

   ADSPResult result = ADSP_EOK;
   AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];

   if (ADM_RX_DIR == phCalPort->unCoppDirection)
   {
      // LLNP copp can connect to only one LLNP session
      if (!((1 == phCalPort->unNumConnectedSessionsRx) || //stream session not yet closed.
            (0 == phCalPort->unNumConnectedSessionsRx))) //stream session closed
      {
         result = ADSP_EBADPARAM;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Error: LLNP COPP [%u] connected to multiple sessions", unCoppID);
         elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
         return result;
      }
   
      result = Adm_DC_CloseRxDevice(pAdmStatSvc, phCalPort, unCoppID, eClient);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Error: LLNP Rx COPP [%u] failed to disconnect w/ result %d", unCoppID, result);
         elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
         return result;
      }
   }
   else
   {
      // LLNP copp can connect to only one LLNP session
      if (!((1 == phCalPort->unNumConnectedSessionsTx) || //stream session not yet closed.
            (0 == phCalPort->unNumConnectedSessionsTx))) //stream session closed
      {
         result = ADSP_EBADPARAM;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Error: LLNP COPP [%u] connected to multiple sessions", unCoppID);
         elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
         return result;
      }
	  
      result = Adm_DC_CloseTxDevice(pAdmStatSvc, phCalPort, unCoppID, eClient);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Error: LLNP Tx COPP [%u] failed to disconnect w/ result %d", unCoppID, result);
         elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
         return result;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: LLNP COPP/COPreP [%u] closed.", unCoppID);
   AudDevMgr_RequestHwResources(pAdmStatSvc);
   elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);

   return result;
}

ADSPResult Adm_LlnpStreamConnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                 EliteMsg_CustomAdmStreamConnectType *pPayload,
                                 adm_client_type_t eClient)
{
  if (eLSM_TYPE == eClient)
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: LLNP Stream Connect received for LSM service. Unsupported Combination!");
	   return ADSP_EUNSUPPORTED;
   }
   if (NULL == pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: LLNP Stream Connect received NULL payload ptr");
      return ADSP_EFAILED;
   }

   uint32_t sessionID = pPayload->unSessionID;
   uint32_t unDirection = (0 == pPayload->direction) ? ADM_RX_DIR : ADM_TX_DIR;

   //check if session is compressed stream session
   if (TRUE == pPayload->is_compressed)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: LLNP Stream session ID:[0x%lX] do not support compressed data!", sessionID);
      return ADSP_EFAILED;
   }

   //Allocate an index for this connection
   uint16_t unIndex = Adm_CalculateNextAvlIndex(pAdmStatSvc, unDirection,eClient);
   if (ADM_INVALID_INDEX == unIndex)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Error Adm_LlnpStreamConnect: No more valid indices available.");
      return ADSP_ENORESOURCE;
   }

   AdmStreamSessionInfo_Type *pSessionInfo;
   if (ADM_RX_DIR == unDirection)
   {
      //Associate this index with this session/stream ID and grab hold of pSessionInfo.
      pAdmStatSvc->rxIndexToSessionStreamID[unIndex] = (uint16_t)(sessionID);
      pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
      pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
      // LLNP Rx will provide a dummy handle.
      pPayload->phPort = (elite_svc_handle_t *)DUMMY_HANDLE;
   }
   else
   {
      //Associate this index with this session/stream ID and grab hold of pSessionInfo.
      pAdmStatSvc->txIndexToSessionStreamID[unIndex] = (uint16_t)(sessionID);
      pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
      pSessionInfo = &(pAdmStatSvc->txStreamSessionInfo[unIndex]);
      // LLNP Tx will not provide a handle at stream connection time as it is not needed by the ASM.
      pPayload->phPort = NULL;
   }
   pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
   pSessionInfo->stream_perf_mode = ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LLNP Stream session ID [0x%lX] connected successfully!",
         sessionID);
   return ADSP_EOK;
}

ADSPResult Adm_LlnpStreamDisconnect(AdmStatSvc_InfoType *pAdmStatSvc,
                                    EliteMsg_CustomAdmStreamConnectType *pPayload,
                                    adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }
   if (NULL == pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: LLNP Stream Disconnect received NULL payload ptr");
      return ADSP_EFAILED;
   }

   ADSPResult result = ADSP_EOK;
   uint32_t unSessionID = pPayload->unSessionID;

   //check if session is compressed stream session
   if (TRUE == pPayload->is_compressed)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Error: LLNP Stream session ID:[0x%lX] does not support compressed data!", unSessionID);
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Disconnecting stream ID 0x%x", unSessionID);
   AdmStreamSessionInfo_Type *pSessionInfo;
   uint16_t unIndex;
   uint32_t unDirection = (ADM_RX_DIR == pPayload->direction) ? ADM_RX_DIR : ADM_TX_DIR;
   if (ADM_RX_DIR == unDirection)
   {
      unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_RX_DIR, (uint16_t)(unSessionID), eClient);
      if(ADM_INVALID_INDEX == unIndex)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Rx COPP index not found!");
         return ADSP_EBADPARAM;
      }

      // Disconnect the stream if connected to a device 
      if (0 != pAdmStatSvc->rxsessionMappingMask[unIndex])
      {
         uint16_t unCoppId = pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex];
         AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppId];

         // remove the session from the CalPort structure
         Adm_DC_RemovePrimaryCoppFromSessionIDMap(pAdmStatSvc, ADM_MATRIX_ID_AUDIO_RX, unSessionID, eClient);
         phCalPort->unDirectConnSessionID = 0;

         // disconnect/reconnect to the AFE - this releases any held buffers if the AFE is stopped.
         // The connection between "COPP" and AFE is maintained by reconnecting
         Adm_DC_ReconnectAfeToCopp(pAdmStatSvc, phCalPort, unCoppId);
      }

      // Reset the Rx Session
      Adm_ResetSessionMappingMask(pAdmStatSvc, unSessionID, ADM_RX_DIR, eClient);
      pAdmStatSvc->rxIndexToSessionStreamID[unIndex] = 0;
      pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
   }
   else // TX Direction
   {
      unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_TX_DIR, (uint16_t)(unSessionID), eClient);
      if(ADM_INVALID_INDEX == unIndex)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Tx COPP index not found!");
         return ADSP_EBADPARAM;
      }

      // Disconnect the stream from the AFE port if the stream is connected
      if (0 != pAdmStatSvc->txsessionMappingMask[unIndex])
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "ADM: Detected connected device to stream ID 0x%x, issuing AFE disable on disconnect",
               unSessionID);
         uint32_t unCoppID = pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex];
         AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];
         result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                           phCalPort->unEID1.unAfePortID,
                                           phCalPort->unEID1.phAfePort,
                                           ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "ADM: Failed to disconnect stream ID 0x%x from device", unSessionID);
            return result;
         }
      }

      // Reset the Tx Session
      Adm_ResetSessionMappingMask(pAdmStatSvc, unSessionID, ADM_TX_DIR, eClient);
      pAdmStatSvc->txIndexToSessionStreamID[unIndex] = 0;
      pSessionInfo = &(pAdmStatSvc->txStreamSessionInfo[unIndex]);
   }

   // Common to both Rx & Tx, reset the session info structure to a default state
   pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
   pSessionInfo->stream_perf_mode = ASM_LEGACY_STREAM_SESSION;

   // Detect primary device of all active streams and devices.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "As part of LLNP stream disconnect, updating primary device/port");
   if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDevicePcm(pAdmStatSvc)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM: updating primary device/port failed, result = %d", result);
      return result;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LLNP Stream session ID [0x%lX] disconnected successfully",
         unSessionID);
   return result;
}

ADSPResult Adm_LlnpStreamDisable(AdmStatSvc_InfoType *pAdmStatSvc,
                                 uint16_t unSessionID,
                                 adm_client_type_t eClient)
{
   if (eLSM_TYPE == eClient)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Unsupported for LSM session");
      return ADSP_EBADPARAM;
   }
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Adm_LlnpStreamDisable: disabling stream session 0x%x",
         unSessionID);
   uint32_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_TX_DIR, (uint16_t)(unSessionID), eClient);
   if(ADM_INVALID_INDEX == unIndex)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Tx COPP index not found!");
      return ADSP_EBADPARAM;
   }

   uint32_t unCoppID = pAdmStatSvc->txSessionIDToPrimCOPPIDMap[unIndex];
   AdmCalPort_Type *phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];
   if (NULL != phCalPort->unEID1.phAfePort)
   {
      // The name is misleading, it's actually sending a disable to the AFE port.
      // We can safely issue disable without having to worry if the AFE is disabled already
      result = Adm_SendCmdDisconnectAFE(pAdmStatSvc,
                                        phCalPort->unEID1.unAfePortID,
                                        phCalPort->unEID1.phAfePort,
                                        ELITEMSG_CUSTOM_AFECLIENTDISABLE);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "ADM: Failed to disable the AFE with result 0x%x", result);
         return result;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Adm_LlnpStreamDisable: successfully disabled stream session 0x%x", unSessionID);
   return result;
}
