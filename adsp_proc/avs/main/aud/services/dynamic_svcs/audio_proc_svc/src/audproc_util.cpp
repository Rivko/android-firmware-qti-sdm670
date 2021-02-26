/*==========================================================================
ELite Source File

This file implement Audio Post Processing Dynamic Service logic.

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------


=========================================================================== */
/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "EliteLoggingUtils_i.h"
#include "audproc_comdef.h"

/*===========================================================================
FUNCTION AudPP_LogData()

DESCRIPTION
Returns the QXDM log code as per PP type

PARAMETERS
Input:  Pointer to PP dynamic service structure

RETURN VALUE
ADSPResult

DEPENDENCIES
None

SIDE EFFECTS
None

NOTES:

===========================================================================*/

ADSPResult AudPP_LogData(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
    ADSPResult          result = ADSP_EOK;

    AudPPInputPort_t	*pInPort = me->in_port[port_id];
    elite_log_info      log_info_obj;
    uint32_t            buf_size;

    buf_size = (pInPort->inputBufInfo.sizeInSamples) * (pInPort->inputMediaFormatInfo.channels);

    if(pInPort->inputMediaFormatInfo.bitsPerSample > 16)
    {
        buf_size = buf_size << 2;
    }
    else
    {
        buf_size = buf_size << 1;
    }

    /* Populate the packet to be sent to logging utility */ 
    log_info_obj.qxdm_log_code = me->qxdm_log_code_in;
    log_info_obj.buf_ptr = (uint8_t *)pInPort->inputBufInfo.pBuf;
    log_info_obj.buf_size = buf_size;
    log_info_obj.session_id = me->ulDataLogId;
    log_info_obj.log_tap_id = (AUDIOLOG_AUD_PP_IN_TAP_ID << 8)| port_id;
    log_info_obj.log_time_stamp = qurt_elite_timer_get_time();

	if (!isCompressedInput(me, port_id)) {
		log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

		pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);
		pcm_data->num_channels = pInPort->inputMediaFormatInfo.channels;
		pcm_data->sampling_rate = pInPort->inputMediaFormatInfo.samplingRate;
		pcm_data->bits_per_sample = pInPort->inputMediaFormatInfo.bitsPerSample;
		pcm_data->interleaved = pInPort->inputMediaFormatInfo.isInterleaved;
		pcm_data->channel_mapping =
				pInPort->inputMediaFormatInfo.channelMapping;
	} else {
		log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
		log_info_obj.data_info.media_fmt_id =
				pInPort->inputMediaFormatInfo.bitstreamFormat;
	}
    /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
     */
    result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

    return result;
}

ADSPResult AudPP_LogData_Output(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
    ADSPResult          result = ADSP_EOK;

    AudPPStatus_t       *pMyStat = &me->audPPStatus;
    elite_log_info      log_info_obj;  

	 /* Populate the packet to be sent to logging utility */ 
    log_info_obj.qxdm_log_code = me->qxdm_log_code_out;
    log_info_obj.buf_ptr = (uint8_t *)&(pMyStat->pOutDataMsgDataBuf->nDataBuf);
    log_info_obj.buf_size = pMyStat->pOutDataMsgDataBuf->nActualSize;
    log_info_obj.session_id = me->ulDataLogId;
    log_info_obj.log_tap_id = (AUDIOLOG_AUD_PP_OUT_TAP_ID << 8) | port_id;
    log_info_obj.log_time_stamp = qurt_elite_timer_get_time();
    
if (!isCompressedInput(me,AUDPP_PRIMARY_IN_PORT))
    {
    log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

    	pcm_data_info       *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

    pcm_data->num_channels = me->outputMediaFormatInfo.channels;
    pcm_data->sampling_rate = me->outputMediaFormatInfo.samplingRate;
    pcm_data->bits_per_sample = me->outputMediaFormatInfo.bitsPerSample == 16 ? 16 : 32;
    pcm_data->interleaved = me->outputMediaFormatInfo.isInterleaved;
    pcm_data->channel_mapping = me->outputMediaFormatInfo.channelMapping;
    }
    else
    {
    	log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
    	log_info_obj.data_info.media_fmt_id = me->in_port[AUDPP_PRIMARY_IN_PORT]->inputMediaFormatInfo.bitstreamFormat;
    }

    /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
     */
    result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

    return result;
}

/* Function that returns true when there is switch from stream 1 to 
   stream 2 */

 bool_t switchInGapless ( ThisAudDynaPPSvc_t* me)
{
    //AudPPStatus_t *pMyStat = &me->audPPStatus;
	
    elite_msg_data_eos_header_t *pelite_msg_data_eos_header_t =  (elite_msg_data_eos_header_t *)(me->in_port[AUDPP_PRIMARY_IN_PORT]->pInDataMsg->pPayload);
	
    return (ELITEMSG_DATA_RESET_SESSION_CLK  == pelite_msg_data_eos_header_t->unEosFormat);
	   
}

 bool_t AudPP_isPrimaryInputPort(uint32_t port_id)
 {
 	// Only port_id 0 is considered as primary port
 	return (AUDPP_PRIMARY_IN_PORT == port_id );
 }

 bool_t AudPP_isPortRunning(ThisAudDynaPPSvc_t* pMe,uint32_t port_id)
  {
  	// Only port_id 0 is considered as primary port
	 AudPPInputPort_t *pInPort = pMe->in_port[port_id];
  	return (pInPort->mediaFormatReceived && !(pInPort->eosReceived));
  }

 ADSPResult AudPP_RegisterWithAdsppm(ThisAudDynaPPSvc_t* pMe)
 {
    ADSPResult result = ADSP_EOK;
 #if (ADSPPM_INTEGRATION==1)
    MmpmRegParamType regParam;
    char threadname[16];
    qurt_thread_get_name(threadname, 16-1);

    regParam.rev             = MMPM_REVISION;
    regParam.instanceId      = MMPM_CORE_INSTANCE_0;
    regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
    regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
    regParam.MMPM_Callback   = NULL;
    regParam.cbFcnStackSize  = 0;

    regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
    regParam.pClientName     = threadname;

    result = qurt_elite_adsppm_wrapper_register(&regParam, &pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by P%hX. Result %lu. Client id %lu", pMe->objId, result, pMe->ulAdsppmClientId);

 #endif
    return result;
 }

 ADSPResult AudPP_DeregisterWithAdsppm(ThisAudDynaPPSvc_t* pMe)
 {
    ADSPResult result = ADSP_EOK;
    if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
    {
        return ADSP_EOK;
    }
    uint32_t client_id=pMe->ulAdsppmClientId;
    result = qurt_elite_adsppm_wrapper_deregister(&pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by P%hX. Result %lu. Client id %lu", pMe->objId, result, client_id);

    return result;
 }

uint32_t AudPP_AggregateBw(ThisAudDynaPPSvc_t *pMe)
{
   uint32_t newBw = 0;
   const AdspAudioMediaFormatInfo_t *pInputMediaFormat = &pMe->in_port[AUDPP_PRIMARY_IN_PORT]->inputMediaFormatInfo;
   const AdspAudioMediaFormatInfo_t *pOutputMediaFormat = &pMe->outputMediaFormatInfo;
   uint32_t bytes_per_sample_in = pInputMediaFormat->bitsPerSample == 16 ? BYTES_PER_SAMPLE_TWO : BYTES_PER_SAMPLE_FOUR;
   uint32_t bytes_per_sample_out = pOutputMediaFormat->bitsPerSample == 16 ? BYTES_PER_SAMPLE_TWO : BYTES_PER_SAMPLE_FOUR;;

   newBw = (bytes_per_sample_in * pInputMediaFormat->channels * pInputMediaFormat->samplingRate) +
               (bytes_per_sample_out * pOutputMediaFormat->channels * pOutputMediaFormat->samplingRate) ;

   if (pInputMediaFormat->samplingRate > 48000)
   {
      newBw += 1*1024*1024;
   }
   if (pOutputMediaFormat->samplingRate > 48000)
   {
      newBw += 1*1024*1024;
   }
   if (pInputMediaFormat->channels>2)
   {
      newBw += 1*1024*1024;
   }
   if (pOutputMediaFormat->channels>2)
   {
      newBw += 1*1024*1024;
   }

   //for 44.1k multiple vote extra.
   if ( (pInputMediaFormat->samplingRate % 11025 == 0) || (pOutputMediaFormat->samplingRate % 11025 == 0) )
   {
      if ( (bytes_per_sample_in==BYTES_PER_SAMPLE_TWO) || (bytes_per_sample_out==BYTES_PER_SAMPLE_TWO))
      {
         newBw += 1*1024*1024;
      }
      else
      {
         newBw += 1024*1024/2;
      }
   }

   newBw += pMe->topologyBandwidth; // Add any extra bandwidth requested by the modules in the topology

   return newBw;
}

uint32_t AudPP_AggregateKpps(ThisAudDynaPPSvc_t *pMe)
{
   uint32_t kpps;
   kpps = pMe->topologyKpps ;

   return kpps;
}
 /**
  * force_vote doesn't matter if is_release=TRUE
  *
  * is_release helps in releasing BW even when aggregated BW is nonzero, useful for suspend.
  *
  * force_vote helps in voting BW due to changes in svc & not due to CAPI V2 events.
  *
  */
 ADSPResult AudPP_ProcessKppsBw(ThisAudDynaPPSvc_t *pMe, bool_t is_release, bool_t force_vote)
 {
    ADSPResult result = ADSP_EOK;

    if (pMe->is_processing_set_media_type) // This function will be called once all media type processing is done.
    {
       return ADSP_EOK;
    }
    if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
    {
        return ADSP_EFAILED;
    }

    uint32_t newBw = 0;
    uint32_t kpps=0;
    if (!is_release || force_vote)
    {
       newBw = AudPP_AggregateBw(pMe);

       kpps = AudPP_AggregateKpps(pMe);
    }

    //If there was no event or no release-call, or no foce vote or there was no change, return.
    if (!( is_release || (pMe->prev_kpps_vote != kpps) || (pMe->prev_bw_vote != newBw)))
    {
       return ADSP_EOK;
    }

 #if (ADSPPM_INTEGRATION==1)
    static const uint8_t NUM_REQUEST=2;
    MmpmRscParamType rscParam[NUM_REQUEST];
    MMPM_STATUS      retStats[NUM_REQUEST];
    MmpmRscExtParamType reqParam;
    uint8_t req_num=0;
    MmpmMppsReqType mmpmMppsParam;
    MmpmGenBwValType bwReqVal;
    MmpmGenBwReqType bwReq;
    uint32_t mpps=0;

    reqParam.apiType                    = MMPM_API_TYPE_SYNC;
    reqParam.pExt                       = NULL;       //for future
    reqParam.pReqArray                  = rscParam;
    reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
    reqParam.reqTag                     = 0;          //for async only

    /** check if mips and bw are both releases or requests. both_diff => each != request or release. */
    bool_t both_diff = ( (kpps > 0) ^ (newBw > 0) );
    //whether mips req is made
    bool_t is_mips_req = (pMe->prev_kpps_vote != kpps) || is_release ;
    //whether bw req is made
    bool_t is_bw_req = ( (pMe->prev_bw_vote != newBw) || is_release );
    bool_t req_done = false;

    if ( is_mips_req )
    {
       pMe->prev_kpps_vote = kpps;

       mpps =  (kpps+999)/1000;

       //Requesting 0 will be equivalent to releasing particular resource
       mmpmMppsParam.mppsTotal                  = mpps;
       mmpmMppsParam.adspFloorClock             = 0;
       rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
       rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

       req_num++;

       if (both_diff || !is_bw_req) //request separately if either bw and mips are differet types (req/rel), or if there's no BW req.
       {
          reqParam.numOfReq                   = req_num;

          if (kpps == 0)
          {
             result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by P%hX  (%lu). Result %lu", pMe->objId, pMe->ulAdsppmClientId, result);
          }
          else
          {
             result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
             MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by P%hX (%lu). Result %lu",  mpps, pMe->objId, pMe->ulAdsppmClientId,result);
          }
          req_done = true;
          req_num = 0; //reset req_num as req is already made.
       }
    }

    if ( is_bw_req )
    {
       pMe->prev_bw_vote = newBw;

       bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
       bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
       bwReqVal.bwValue.busBwValue.bwBytePerSec     = newBw;
       bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
       bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

       bwReq.numOfBw            = 1;
       bwReq.pBandWidthArray    = &bwReqVal;

       rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
       rscParam[req_num].rscParam.pGenBwReq      = &bwReq;
       req_num++;
       if (both_diff || !is_mips_req) //request separately if either bw and mips are differet types (req/rel), or if there's no mips req.
       {
          reqParam.numOfReq                   = req_num;

          if (newBw==0)
          {
             result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by P%hX (%lu). Result %lu",pMe->objId, pMe->ulAdsppmClientId, result);
          }
          else
          {
             result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
             MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by P%hX (%lu). Result %lu", newBw/1024, pMe->objId, pMe->ulAdsppmClientId, result);
          }
          req_done = true;
          req_num = 0; //reset req_num as req is already made.
       }
    }

    if (req_num && !req_done)
    {
       reqParam.numOfReq                   = req_num;

       if ( (kpps == 0) && (newBw == 0) )
       {
          result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by P%hX (%lu). Result %lu", pMe->objId, pMe->ulAdsppmClientId,result);
       }
       else
       {
          result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
          MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by P%hX (%lu). Result %lu", mpps, newBw/1024, pMe->objId, pMe->ulAdsppmClientId, result);
       }
    }

 #endif //#if (ADSPPM_INTEGRATION==1)
    return result;
 }
