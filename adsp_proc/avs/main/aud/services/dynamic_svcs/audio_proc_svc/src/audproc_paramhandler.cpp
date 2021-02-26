
/*==========================================================================
ELite Source File

This file implements the functions for setting the parameters for the Audio Post
Processing Dynamic Service.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
 */

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/20/10   DG      Added support for the case of 5.1 bypass, when the number
                   of input and output channels will be 6.
06/09/10   DG      Created file and copied functions from AudDynaPPSvc.cpp.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "EliteTopology_db_if.h"
#include "audproc_topo_bg_thread.h"

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/

#define KPPS_VOTE_MEDIA_FMT_CHANGE 50000 // 50 MPPS
#define BW_VOTE_MEDIA_FMT_CHANGE 20*1024*1024 //20 MBPS

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
static void AudPP_PrintMediaFormat(const ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pMediaFormat, bool_t is_input,uint32_t port_id);

static void AudPP_topo_KPPS_event(topo_event_client_t *obj_ptr, uint32_t new_KPPS);
static void AudPP_topo_bandwidth_event(topo_event_client_t *obj_ptr, uint32_t new_bandwidth);
static void AudPP_topo_output_media_format_event(topo_event_client_t *obj_ptr, const AdspAudioMediaFormatInfo_t *new_format);
static void AudPP_topo_algorithmic_delay_event(topo_event_client_t *obj_ptr, uint32_t new_delay);
static void AudPP_topo_custom_event(topo_event_client_t *obj_ptr, void *payload, uint32_t payloadsize);
static ADSPResult AudPP_populateCb(audproc_event_query_payload *clientStruct, audproc_event_node_t **event_root_node);
/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
const topo_event_vtable_t topo_event_vtable = {
      AudPP_topo_KPPS_event,
      AudPP_topo_bandwidth_event,
      AudPP_topo_output_media_format_event,
      AudPP_topo_algorithmic_delay_event,
	  AudPP_topo_custom_event
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

ADSPResult AudPP_setInputMediaInfo(ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pInputMediaFormat,uint32_t port_id)
{
	ADSPResult result = ADSP_EOK;

	//MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Setting media format", me->objId);
	AudPP_PrintMediaFormat(me, pInputMediaFormat, TRUE, port_id);

	me->is_processing_set_media_type = TRUE;

	AdspAudioMediaFormatInfo_t inputFormat = *pInputMediaFormat;
	if (!isCompressedFormat(pInputMediaFormat))
	{
		inputFormat.isInterleaved = FALSE; // Deinterleaving is done by the format converter
	}

	uint32_t prev_vote_kpps,prev_vote_bw;
	prev_vote_kpps = me->topologyKpps;
	prev_vote_bw = me->topologyBandwidth;

	//Set KPPS and BW needed for reinit sequence if existing vote does not suffice
	me->topologyKpps  = (me->topologyKpps < KPPS_VOTE_MEDIA_FMT_CHANGE) ? KPPS_VOTE_MEDIA_FMT_CHANGE : me->topologyKpps; // 50 MPPS Voting during media format update
	me->topologyBandwidth = (me->topologyBandwidth < BW_VOTE_MEDIA_FMT_CHANGE ) ? BW_VOTE_MEDIA_FMT_CHANGE : me->topologyBandwidth; // 20 MBPS BW Voting during media format update

	// Bypass the flag temporarily to ensure that BW vote is raised here
	me->is_processing_set_media_type = FALSE;
	// Vote for MIPS and BW needed for media format reinit sequence
	(void)AudPP_ProcessKppsBw(me, FALSE, TRUE);
	me->is_processing_set_media_type = TRUE;

	//Restore KPPS and BW to previous values
	me->topologyKpps  = prev_vote_kpps;
	me->topologyBandwidth = prev_vote_bw;

	topo_change_input_media_type(me->pTopologyObj,
			&inputFormat,port_id);

	bool_t deinterleavingNeeded = pInputMediaFormat->isInterleaved && !isCompressedFormat(pInputMediaFormat);
	result = AudPP_SetupFormatConv(&(me->in_port[port_id]->formatConverter),
			pInputMediaFormat->channels,
			pInputMediaFormat->channels, // The format converter cannot change channels, this is done in the topology.
			deinterleavingNeeded,
			pInputMediaFormat->samplingRate);
	if (ADSP_EOK != result)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc in_port %x: Failed to set up the format converter.\n", me->objId,port_id);
		AudPP_GotoFatalState(me);
	}

	if (ADSP_SUCCEEDED(result))
	{
		// All went well, so commit the media format.
		me->in_port[port_id]->inputMediaFormatInfo = *pInputMediaFormat;
	}


   if(topo_isBgThreadReq(me->pTopologyObj))
   {
     if(!topo_is_bg_thread_created(me->bg_th_handle))
     {
        result = topo_bg_thread_create(me->pTopologyObj, (void **)&me->bg_th_handle, topo_get_heap_id(FALSE, me->ppCfgInfo.dynaPPSvcType),
			                           me->numActiveInPorts, me->numActiveOutPorts, me->objId, &me->bg_th_cb_info);
        if (ADSP_EOK != result)
        {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create background thread instance.\n", me->objId);
           AudPP_GotoFatalState(me);
        }
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "background thread created\n");
     }

     result = topo_bg_thread_reset_input_buffers(me->bg_th_handle, port_id, &me->in_port[port_id]->inputMediaFormatInfo);
     if (ADSP_FAILED(result))
     {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate/reset background thread input buffers.\n", me->objId);
        AudPP_GotoFatalState(me);
     }
   }




	me->is_processing_set_media_type = FALSE;

	if (me->is_media_format_change_pending)
	{
		AudPP_UpdateOutputMediaFormat(me, &me->new_media_format);
	}


    (void)AudPP_ProcessKppsBw(me, FALSE, TRUE);

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Set input media format end", me->objId,port_id);

	return result;
}

/**
 * Function to set the output frequency and number of channels.
 *   - Dependencies: Must not be called when the data path is
 *     active.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *   Pointer to the instance structure.
 * @param[in] pParams
 *   The parameters for setting the output media format.
 *
 * @return ADSPResult
 *   Error code indicating status
 */
ADSPResult AudPP_setOutputMediaInfo(ThisAudDynaPPSvc_t* me, const AudPPSvcOutputMediaTypeParams_t *pParams)
{

   MSG_7( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Set output media Begin: keepInputFreq: %d, outputFreq: %lu, useNativeNumChannels: %d, outputChannels: %lu, useNativeBitsPerSample: %d, outputBitsPerSample: %lu", me->objId, pParams->keepInputSamplingRate, pParams->outputSamplingRate, pParams->useNativeNumChannels, pParams->outputNumChannels, pParams->useNativeBitsPerSample, pParams->outputBitsPerSample);

   AudPPInputPort_t *pPrimaryInPort = me->in_port[AUDPP_PRIMARY_IN_PORT];
   ADSPResult result = ADSP_EOK;
   me->is_processing_set_media_type = TRUE;

   /* send output media format to all the capi_v2 modules present in the topology
    * that have registered for the interface extensions "change media format". */
   result = topo_send_capiv2_output_media_format(me->pTopologyObj, pParams, 0);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Error in setting output media format.", me->objId);
      return result;
   }

   if (pPrimaryInPort->mediaFormatReceived)
   {
      topo_gen_out_media_fmt(me->pTopologyObj);
   }

   me->is_processing_set_media_type = FALSE;

   if (me->is_media_format_change_pending)
   {
      AudPP_UpdateOutputMediaFormat(me, &me->new_media_format);
   }
   else
   {
      /* We would reach this condition only if :
       * 1. There is no change in output media format.
       * 2. One of the module has not raised the output media format not but is waiting to raise it later after processing its data.
       */
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Output media format has not changed after setting new output media format", me->objId);
   }

   me->ppCfgInfo.outputParams = *pParams;

   MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Set output media End", me->objId);

   return ADSP_EOK;
}

static void AudPP_PrintMediaFormat(const ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pMediaFormat, bool_t is_input,uint32_t port_id)
{
   if(is_input)
   {
      MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Input MediaFmt in_port %x: Number of channels: %lu, Sampling rate: %lu ,Bits per sample: %lu, "
            "Is Interleaved: %lu, Is Signed: %lu, Bitstream format: %lu ",
            me->objId, port_id, pMediaFormat->channels,pMediaFormat->samplingRate,pMediaFormat->bitsPerSample,
            pMediaFormat->isInterleaved, pMediaFormat->isSigned, pMediaFormat->bitstreamFormat );
      MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Input MediaFmt in_port %x: Channel mapping for channels 1 to 6: %d, %d, %d, %d, %d, %d,", me->objId, port_id,
            pMediaFormat->channelMapping[0], pMediaFormat->channelMapping[1], pMediaFormat->channelMapping[2],
            pMediaFormat->channelMapping[3], pMediaFormat->channelMapping[4], pMediaFormat->channelMapping[5]);
      if(pMediaFormat->channels > 6)
      {
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Input MediaFmt in_port %x: Channel mapping for channels 7 and 8 :: %d, %d,", me->objId, port_id,
               pMediaFormat->channelMapping[6], pMediaFormat->channelMapping[7]);
      }
   }
   else
   {
      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Output MediaFmt: Number of channels: %lu, Sampling rate: %lu ,Bits per sample: %lu,"
            "Is Interleaved: %lu, Is Signed: %lu, Bitstream format: %lu ",
            me->objId, pMediaFormat->channels,pMediaFormat->samplingRate,pMediaFormat->bitsPerSample,
            pMediaFormat->isInterleaved, pMediaFormat->isSigned, pMediaFormat->bitstreamFormat );
      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Output MediaFmt: Channel mapping for channels 1 to 6: %d, %d, %d, %d, %d, %d,", me->objId,
            pMediaFormat->channelMapping[0], pMediaFormat->channelMapping[1], pMediaFormat->channelMapping[2],
            pMediaFormat->channelMapping[3], pMediaFormat->channelMapping[4], pMediaFormat->channelMapping[5]);
      if(pMediaFormat->channels > 6)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX: Output MediaFmt: Channel mapping for channels 7 and 8: %d, %d,", me->objId,
               pMediaFormat->channelMapping[6], pMediaFormat->channelMapping[7]);
      }
   }
}

/**
 * Compares two structures of type AdspAudioMediaFormatInfo_t
 * for equality, by checking the value of each member.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pMF1
 *  Pointer to the first structure.
 * @param[in] pMF2
 *  Pointer to the second structure.
 *
 * @return bool_t
 *  TRUE if all the members of pMF1 are equal to those of pMF2.
 *  FALSE otherwise.
 */
bool_t AudPP_MediaFormatsEqual(const AdspAudioMediaFormatInfo_t *pMF1, const AdspAudioMediaFormatInfo_t *pMF2)
{
   bool_t returnVal = TRUE;

   returnVal = returnVal && (pMF1->channels == pMF2->channels);
   returnVal = returnVal && (pMF1->bitsPerSample == pMF2->bitsPerSample);
   returnVal = returnVal && (pMF1->samplingRate == pMF2->samplingRate);
   returnVal = returnVal && (pMF1->isSigned == pMF2->isSigned);
   returnVal = returnVal && (pMF1->isInterleaved == pMF2->isInterleaved);
   returnVal = returnVal && (pMF1->bitstreamFormat == pMF2->bitstreamFormat);
   for (uint32_t i = 0; i < pMF1->channels; i++)
   {
      returnVal = returnVal && (pMF1->channelMapping[i] == pMF2->channelMapping[i]);
   }

   return returnVal;
}

static ThisAudDynaPPSvc_t *AudPP_topo_cb_to_svc(topo_event_client_t *cb_obj_ptr)
{
   uint8_t *cb_obj_mem_ptr = reinterpret_cast<uint8_t*>(cb_obj_ptr);
   uint8_t *pp_obj_mem_ptr = cb_obj_mem_ptr - offsetof(ThisAudDynaPPSvc_t, topo_event_client);
   return reinterpret_cast<ThisAudDynaPPSvc_t*>(pp_obj_mem_ptr);
}

static void AudPP_topo_KPPS_event(topo_event_client_t *obj_ptr, uint32_t new_KPPS)
{
   ThisAudDynaPPSvc_t *me = AudPP_topo_cb_to_svc(obj_ptr);

   if (me->topologyKpps != new_KPPS)
   {
      me->topologyKpps = new_KPPS;
   }

   if (!qurt_elite_adsppm_wrapper_is_registered(me->ulAdsppmClientId))
   {
      return;
   }
   new_KPPS = AudPP_AggregateKpps(me);

   if (me->prev_kpps_vote !=  new_KPPS)
   {
      me->prev_kpps_vote =  new_KPPS;
#if (ADSPPM_INTEGRATION==1)
      ADSPResult result=ADSP_EOK;
      static const uint8_t NUM_REQUEST=1;
      MmpmRscParamType rscParam[NUM_REQUEST];
      MMPM_STATUS      retStats[NUM_REQUEST];
      MmpmRscExtParamType reqParam;
      uint8_t req_num=0;
      MmpmMppsReqType mmpmMppsParam;

      reqParam.apiType                    = MMPM_API_TYPE_SYNC;
      reqParam.pExt                       = NULL;       //for future
      reqParam.pReqArray                  = rscParam;
      reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
      reqParam.reqTag                     = 0;          //for async only

      uint32_t mpps = (new_KPPS > 0)   ? (new_KPPS+1000)/1000   : 0; //round up if not zero

      //Requesting 0 will be equivalent to releasing particular resource
      mmpmMppsParam.mppsTotal                  = mpps;
      mmpmMppsParam.adspFloorClock             = 0;
      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

      req_num++;

      reqParam.numOfReq                   = req_num;

      if (new_KPPS == 0)
      {
         result = qurt_elite_adsppm_wrapper_release(me->ulAdsppmClientId, &me->adsppmClientPtr, &reqParam);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by P%hX  (%lu). Result %lu", me->objId, me->ulAdsppmClientId, result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_request(me->ulAdsppmClientId, &me->adsppmClientPtr, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by P%hX (%lu). Result %lu",  mpps, me->objId, me->ulAdsppmClientId,result);
      }
#endif // #if (ADSPPM_INTEGRATION==1)
   }
}

static void AudPP_topo_bandwidth_event(topo_event_client_t *obj_ptr, uint32_t new_bandwidth)
{
   ThisAudDynaPPSvc_t *me = AudPP_topo_cb_to_svc(obj_ptr);

   me->topologyBandwidth = new_bandwidth;

   AudPP_VoteBw(me);
}

static void AudPP_topo_output_media_format_event(topo_event_client_t *obj_ptr, const AdspAudioMediaFormatInfo_t *new_format)
{
   ThisAudDynaPPSvc_t *me = AudPP_topo_cb_to_svc(obj_ptr);

   AudPP_UpdateOutputMediaFormat(me, new_format);
}

static void AudPP_topo_algorithmic_delay_event(topo_event_client_t *obj_ptr, uint32_t new_delay)
{
   ThisAudDynaPPSvc_t *me = AudPP_topo_cb_to_svc(obj_ptr);

   if (NULL != me->pAlgorithmicDelay)
   {
      *me->pAlgorithmicDelay = new_delay;
   }
}

static ADSPResult AudPP_populateCb(audproc_event_query_payload *clientStruct, audproc_event_node_t **event_root_node)
{
	audproc_event_node_t *node = AudPP_find_node(clientStruct,event_root_node );
	if(NULL == node)
	{
		return ADSP_EFAILED;
	}
	clientStruct->dest_addr = node->receivedPayload.src_addr;
	clientStruct->dest_port = node->receivedPayload.src_port;
	clientStruct->src_addr  = node->receivedPayload.dest_addr;
	clientStruct->src_port  = node->receivedPayload.dest_port;
	clientStruct->apr_token = node->receivedPayload.apr_token;

	return ADSP_EOK;
}
static void AudPP_topo_custom_event(topo_event_client_t *obj_ptr, void *payload, uint32_t payloadsize)
{
	audproc_event_query_payload client = {0};
	ThisAudDynaPPSvc_t *me = AudPP_topo_cb_to_svc(obj_ptr);
	audproc_event_response_payload_header_t *event_info = (audproc_event_response_payload_header_t*)payload;
	client.eventID = event_info->event_id;
	client.instanceID = event_info->instance_id;
	client.moduleID = event_info->module_id;
	ADSPResult result;
	result  = AudPP_populateCb(&client,&me->event_root_node);
	if (result != ADSP_EOK)
	{
		return;
	}
	if((DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_POPREP == me->ppCfgInfo.dynaPPSvcType))
	{
		AudioStreamMgr_CallBackHandleType client_cb;
		client_cb.selfAddr = client.src_addr;
		client_cb.selfPort = client.src_port;
		client_cb.aprHandle = me->CbData.aprHandle;
		client_cb.clientAddr = client.dest_addr;
		client_cb.clientPort = client.dest_port;
		AudioStreamMgr_GenerateClientEventFromCb(&client_cb,
												ASM_STREAM_PP_EVENT,
												client.apr_token, payload, payloadsize);
	}
	else //if(DYNA_SVC_PP_TYPE_COPP == pInitParams->type ||  DYNA_SVC_PP_TYPE_COPREP == pInitParams->type)
	{
		AdmEventCbData_t adm_event_cbdata;
		adm_event_cbdata.token     = client.apr_token;
		adm_event_cbdata.dest_addr = client.dest_addr;
		adm_event_cbdata.dest_port = client.dest_port;
		adm_event_cbdata.src_addr  = client.src_addr;
		adm_event_cbdata.src_port  = client.src_port;
		AudDevMgr_GenerateClientCb(&adm_event_cbdata, ADM_PP_EVENT, payload, payloadsize);
	}
}
void AudPP_UpdateOutputMediaFormat(ThisAudDynaPPSvc_t *me, const AdspAudioMediaFormatInfo_t *new_format)
{
   ADSPResult result;
   if (me->is_processing_data || me->is_processing_set_media_type)
   {
      me->is_media_format_change_pending = TRUE;
      me->new_media_format = *new_format;
      return;
   }

   AdspAudioMediaFormatInfo_t oldOutputMediaFormat = me->outputMediaFormatInfo;
   me->outputMediaFormatInfo = *new_format;

   if(topo_is_bg_thread_created(me->bg_th_handle))
   {
       result = topo_bg_thread_reset_output_buffers(me->bg_th_handle, 0, &me->outputMediaFormatInfo);
       if (ADSP_FAILED(result))
       {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate/reset background thread output buffers.\n", me->objId);
           //AudPP_GotoFatalState(me);
       }
   }

   me->is_media_format_change_pending = FALSE;

   // need to check whether currently there's data being processed
   // in output buffer.  If yes, need to deliver that buffer first
   // before sending media format setup command.
   if (me->audPPStatus.pOutDataMsgDataBuf != NULL)
   {
      // currently there's buffer being processed.
      // From program flow, this can ONLY be PCM buffer.
      // deliver current buffer
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: MF Delivering out %p ", me->objId, me->audPPStatus.pOutDataMsgDataBuf);
      AudPP_deliverCurrentOutputBuffer(me, oldOutputMediaFormat.channels, oldOutputMediaFormat.bitsPerSample);
   }  // if (pOutDataMsgDataBuf !=NULL)

   // enqueue the input Media Type Msg to downstream bufQ.
   result = AudPP_sendMediaTypeMsg(me);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Couldn't send media type downstream!", me->objId);
   }

   // Allocate bigger output buffers if needed for the PCM use case. The buffers for the compressed use case
   // are allocated based on the input size.
   if (!isCompressedFormat(&me->outputMediaFormatInfo))
   {
      result = AudPP_CheckAndAdjustPCMBufSize(me);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Couldn't allocate output buffers!", me->objId);
      }
   }
   AudPP_VoteBw(me);

   AudPP_PrintMediaFormat(me, &me->outputMediaFormatInfo, FALSE,0);
}
