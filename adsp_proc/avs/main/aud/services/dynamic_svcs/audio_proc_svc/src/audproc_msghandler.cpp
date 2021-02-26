
/*==========================================================================
ELite Source File

This file implements the packet parsing functions for the Audio Post
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
1/3/2011   PC      Removing TSM and making POPP appi compliant
10/24/10   DG      Added support for ASM_MEDIA_FMT_MULTI_CHANNEL_PCM
09/09/10   SS      Adding data tapping support (Sim) for Postprocessing
06/09/10   DG      Created file and copied functions from AudDynaPPSvc.cpp.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "EliteTopology_db_if.h"
#include "AFEInterface.h"
#include "audproc_topo_bg_thread.h"

#ifndef MULTI_PORT_DEBUG
//#define MULTI_PORT_DEBUG
#endif

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
static bool_t AudPP_isBufferFull(const AudPP_BufInfo_t *pBufInfo);
static AdspAudioMediaFormatInfo_t AudPP_ParseMultichannelPCMMediaFmt(const elite_multi_channel_pcm_fmt_blk_t *pFormat);
static AdspAudioMediaFormatInfo_t AudPP_ParseCompressedMediaFmt(const elite_compressed_fmt_blk_t *pFormat);
static void AudPP_SoftPause_Terminate(ThisAudDynaPPSvc_t *me);
static bool AudPP_isDevicePath(const ThisAudDynaPPSvc_t *me);

static void AudPP_DataProc_SetParam(ThisAudDynaPPSvc_t *me,uint32_t port_id);
static void AudPP_HandleEOSState(ThisAudDynaPPSvc_t *me, uint32_t port_id, bool_t is_eos);
static void AudPP_sendEos_BgTh(ThisAudDynaPPSvc_t *me, uint32_t port_id, bool_t is_eos);
static void AudPP_Add_DataMsg_To_Cache(AudPPInputPort_t *pInPort, uint16_t objId);
static void AudPP_allocate_deliver_extra_out_buffer(ThisAudDynaPPSvc_t *me, uint32_t port_id, int64_t ss_compensated_drifts);
static ADSPResult AudPP_HandleResync(ThisAudDynaPPSvc_t *me, uint32_t port_id);



/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * Goes through the media type packet and converts to the internal
 * media format structure. All dependence
 * on packet format is contained in this function. Currently it
 * accepts APR packets only.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the current instance structure.
 * @param[in] pMediaFormatData
 * Pointer to the payload of the media type packet.
 * @param[in] pMediaFmt
 * Pointer to the media format structure which will be populated.
 *
 * @return ADSPResult
 * Return code to indicate status.
 */
ADSPResult AudPP_ParseMediaTypePacket(ThisAudDynaPPSvc_t *me, elite_msg_data_media_type_apr_t *pMediaFormatData, AdspAudioMediaFormatInfo_t *pMediaFmt,uint32_t port_id)
{
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Parse Media type Begin", me->objId,port_id);

   if (pMediaFormatData->unMediaTypeFormat != ELITEMSG_DATA_MEDIA_TYPE_APR)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Media Format is NOT in APR format!", me->objId,port_id);
      return ADSP_EBADPARAM;
   }

   switch (pMediaFormatData->unMediaFormatID)
   {
   case ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM:
   {
      elite_multi_channel_pcm_fmt_blk_t *pFormat = (elite_multi_channel_pcm_fmt_blk_t *) elite_msg_get_media_fmt_blk(pMediaFormatData);

        /* validate the media format */
        if((0 == pFormat->bits_per_sample) ||
           (0 == pFormat->num_channels) ||
           (0 == pFormat->sample_rate))
        {
          MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid media format. bits_per_sample %ld "
                                                "num_channels %ld sample_rate %ld", me->objId, pFormat->bits_per_sample, pFormat->num_channels, pFormat->sample_rate);
          return ADSP_EBADPARAM;
        }

      *pMediaFmt = AudPP_ParseMultichannelPCMMediaFmt(pFormat);
      break;
   }
   case ELITEMSG_MEDIA_FMT_COMPRESSED:
   {
      elite_compressed_fmt_blk_t *pFormat = (elite_compressed_fmt_blk_t *) elite_msg_get_media_fmt_blk(pMediaFormatData);
        /* validate the media format */
        if((0 == pFormat->bits_per_sample) ||
           (0 == pFormat->num_channels) ||
           (0 == pFormat->sample_rate))
        {
          MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid media format. bits_per_sample %ld "
                                                "num_channels %ld sample_rate %ld", me->objId, pFormat->bits_per_sample, pFormat->num_channels, pFormat->sample_rate);
          return ADSP_EBADPARAM;
        }
      *pMediaFmt = AudPP_ParseCompressedMediaFmt(pFormat);
      break;
   }
   default:
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Media format is NOT PCM.", me->objId,port_id);
      return ADSP_EBADPARAM;
   }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Parse Media type End", me->objId,port_id);

   return ADSP_EOK;
}

/*===========================================================================
FUNCTION AudPP_DataProc_MediaFormat()

DESCRIPTION
Message handler function for Media format data command.
PARAMETERS
Input:  me to the AudDynaPPSvc instance
pMyStat to svc/processing status

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
passing any residual output buffer to downstream
passing Media Format command to downstream
save corresponding media format information
Doing proper initialization for corresponding PP features.

NOTES:

===========================================================================*/
ADSPResult AudPP_DataProc_MediaFormat(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
	ADSPResult result = ADSP_EOK;

	AudPPInputPort_t *pInPort = me->in_port[port_id];

	elite_msg_data_media_type_apr_t* pMediaFormatData = (elite_msg_data_media_type_apr_t *)(pInPort->pInDataMsg->pPayload);

	AdspAudioMediaFormatInfo_t newInputMediaFormat;

	result = AudPP_ParseMediaTypePacket(me, pMediaFormatData, &newInputMediaFormat,port_id);
	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Error in parsing media format.", me->objId,port_id);
	}

	{
		ADSPResult errCode;
		errCode = elite_msg_release_msg(pInPort->pInDataMsg);
		if (ADSP_FAILED(errCode))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release MediaTypeMsg!", me->objId,port_id);
		}
		pInPort->pInDataMsg = NULL;
	}

	if (ADSP_SUCCEEDED(result))
	{
             result = AudPP_MainThsetInputMediaInfo(me, &newInputMediaFormat, port_id);
	     if(ADSP_FAILED(result))
             {
                 MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to set media type to main thread!", me->objId,port_id);
             }
             return result;
   	}

   return result;

}


ADSPResult AudPP_DataProc_MediaFormat_BGTh_Active(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
   ADSPResult result = ADSP_EOK;

	//AudPPStatus_t *pMyStat = &me->audPPStatus;
	AudPPInputPort_t *pInPort = me->in_port[port_id];

	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX AudPP_DataProc_MediaFormat_BGTh_Active in_port %x: Cached media format msg.", me->objId,port_id);

	elite_msg_data_media_type_apr_t* pMediaFormatData = (elite_msg_data_media_type_apr_t *)(pInPort->pInDataMsg->pPayload);

   AdspAudioMediaFormatInfo_t newInputMediaFormat;

	result = AudPP_ParseMediaTypePacket(me, pMediaFormatData, &newInputMediaFormat,port_id);
   if (ADSP_FAILED(result))
   {
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Error in parsing media format.", me->objId,port_id);
   }

   {
      //ADSPResult errCode;
      //errCode = elite_msg_release_msg(pInPort->pInDataMsg);
      //if (ADSP_FAILED(errCode))
     //{
	 // 	 MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release MediaTypeMsg!", me->objId,port_id);
     // }
      pInPort->pInDataMsg = NULL;
   }

   if (ADSP_SUCCEEDED(result))
   {
         result = topo_bg_set_media_type(me->bg_th_handle, &newInputMediaFormat, port_id);
         if(ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to set media type to bg thread!", me->objId,port_id);
         }
         topo_bg_set_dataQ_pending_flag(me->bg_th_handle, TRUE, port_id);
         return result;
   }

   return result;

}




ADSPResult AudPP_MainThsetInputMediaInfo(ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pInputMediaFormat,uint32_t port_id)
{
	ADSPResult result = ADSP_EOK;

	 //AudPPStatus_t *pMyStat = &me->audPPStatus;
	 AudPPInputPort_t *pInPort = me->in_port[port_id];

		if(AudPP_isPrimaryInputPort(port_id))
		{
			 if( (!(AudPP_MediaFormatsEqual(pInputMediaFormat, &pInPort->inputMediaFormatInfo))))
			{
				// input Media formats are not equal so reinitialize modules
				 result = AudPP_setInputMediaInfo(me, pInputMediaFormat,port_id);
			}
			if (ADSP_SUCCEEDED(result))
			{
				pInPort->mediaFormatReceived = TRUE;
			}
			// After primary port , set held media formats on all other ports
			for(uint32_t port_index=1;port_index<me->numActiveInPorts;port_index++)
			{
				AudPPInputPort_t *pInPortTemp = me->in_port[port_index];
				// If media format was held, restore it to temp variable to set now to the port
				if(pInPortTemp->mediaFormatHeld)
				{
					 result = AudPP_setInputMediaInfo(me, &pInPortTemp->inputMediaFormatInfo,port_index);
					pInPortTemp->mediaFormatHeld = FALSE;
					if (ADSP_SUCCEEDED(result))
					{
						pInPortTemp->mediaFormatReceived = TRUE;
					}
				}
			}
		}
		else
		{
			// If primary port media format is received and already set, set current port media format, otherwise hold it.
			if(me->in_port[AUDPP_PRIMARY_IN_PORT]->mediaFormatReceived) // TBD : Replace with function with port_id input
			{
				 if( (!AudPP_MediaFormatsEqual(pInputMediaFormat, &pInPort->inputMediaFormatInfo)))
				{
					// input Media formats are not equal so reinitialize modules
					 result = AudPP_setInputMediaInfo(me, pInputMediaFormat,port_id);
					//pInPort->mediaFormatHeld = FALSE;
				}
				if (ADSP_SUCCEEDED(result))
				{
					pInPort->mediaFormatReceived = TRUE;
				}
			}
			else
			{
			        memscpy(&pInPort->inputMediaFormatInfo, sizeof(AdspAudioMediaFormatInfo_t), pInputMediaFormat, sizeof(AdspAudioMediaFormatInfo_t));
				pInPort->mediaFormatHeld = TRUE;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Holding MediaTypeMsg till primary port MediaTypeMsg is received!", me->objId,port_id);
			}
		}

	return result;

}

/*===========================================================================
FUNCTION AudPP_VoteBw()

DESCRIPTION
Message handler function for voting BW request event
PARAMETERS
Input:  me to the AudDynaPPSvc instance
        Input Media format
        Output Media format

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS

NOTES:

===========================================================================*/

ADSPResult AudPP_VoteBw(ThisAudDynaPPSvc_t* me)
{
   ADSPResult result = ADSP_EOK;
   if (me->is_processing_set_media_type) // This function will be called once all media type processing is done.
   {
      return ADSP_EOK;
   }

   if (!qurt_elite_adsppm_wrapper_is_registered(me->ulAdsppmClientId))
{
      return ADSP_EOK;
   }

   uint32_t newBw = AudPP_AggregateBw(me);

   if ( (me->prev_bw_vote != newBw) )
    {
      me->prev_bw_vote = newBw;

#if (ADSPPM_INTEGRATION==1)
      static const uint8_t NUM_REQUEST=1;
      MmpmRscParamType rscParam[NUM_REQUEST];
      MMPM_STATUS      retStats[NUM_REQUEST];
      MmpmRscExtParamType reqParam;
      uint8_t req_num=0;
      MmpmGenBwValType bwReqVal;
      MmpmGenBwReqType bwReq;

      reqParam.apiType                    = MMPM_API_TYPE_SYNC;
      reqParam.pExt                       = NULL;       //for future
      reqParam.pReqArray                  = rscParam;
      reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
      reqParam.reqTag                     = 0;          //for async only



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

      reqParam.numOfReq                   = req_num;

      if (newBw==0)
      {
         result = qurt_elite_adsppm_wrapper_release(me->ulAdsppmClientId, &me->adsppmClientPtr, &reqParam);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by P%hX (%lu). Result %lu",me->objId, me->ulAdsppmClientId, result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_request(me->ulAdsppmClientId, &me->adsppmClientPtr, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by P%hX (%lu). Result %lu", newBw/1024, me->objId, me->ulAdsppmClientId, result);
      }
#endif //#if (ADSPPM_INTEGRATION==1)
    }


    return result;
}



/*===========================================================================
FUNCTION AudPP_DataProc_EosFormat()

DESCRIPTION
Message handler function for Eos format data command.
PARAMETERS
Input:  me to the AudDynaPPSvc instance

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
passing any residual output buffer to downstream
translating EOS command into internal EOS cmd if necessary.
passing EOS command to downstream

NOTES:

===========================================================================*/
ADSPResult AudPP_DataProc_EosFormat(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
   ADSPResult result;
   AudPPStatus_t *pMyStat = &me->audPPStatus;
   AudPPInputPort_t *pInPort = me->in_port[port_id];

   if(AudPP_isPrimaryInputPort(port_id))
   {
     if (!AudPP_isDevicePath(me))
     {
        if (pMyStat->pOutDataMsgDataBuf != NULL)
        {
           // currently there's buffer being processed.
           // From program flow, this can ONLY be PCM buffer.
           // deliver current buffer if it has samples. Else just release it.
      	  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: EOS Delivering out %8lx ", me->objId, (int32) pMyStat->pOutDataMsgDataBuf);
      	  AudPP_deliverCurrentOutputBuffer(me);
        }  // if (pOutDataMsgDataBuf !=NULL)
     }

     elite_msg_data_eos_header_t *pelite_msg_data_eos_header_t =  (elite_msg_data_eos_header_t *)(pInPort->pInDataMsg->pPayload);

     if ( (ELITEMSG_DATA_RESET_SESSION_CLK   == pelite_msg_data_eos_header_t->unEosFormat) || /* Don't reset for gapless EOS */
           (ELITEMSG_DATA_DTMF_EOT   == pelite_msg_data_eos_header_t->unEosFormat) ||          /* Indicates DTMF tone end, not a stream */
           AudPP_isDevicePath(me) ) /* Multiple streams can be playing concurrently on the device leg, so don't reset */
     {
     }
     else
     {
        topo_reset(me->pTopologyObj);
     }

     // enqueue EOS Msg to downstream bufQ.
     result = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)(pInPort->pInDataMsg));
     if (ADSP_FAILED(result))
     {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can't enqueue Eos Msg to downstream dataQ!", me->objId);
     }

     //incr log-id after EoS
     me->ulDataLogId++;
   }
   else
   {
	  elite_msg_release_msg(pInPort->pInDataMsg);
	  topo_reset_port_buffers(me->pTopologyObj,port_id);  // reset the intermediate topo layer buffers for that port
   }

   AudPP_HandleEOSState(me,port_id,pInPort->eosReceived);
   // finish media format processing, need to reset input indication pointer.
   pInPort->pInDataMsg = NULL;

   return ADSP_EOK;
}

/*===========================================================================
FUNCTION AudPP_DataProc_DataBufFormatProcSetUp()

DESCRIPTION
DataBuffer format data command processing setup function.
PARAMETERS
Input:  me to the AudDynaPPSvc instance
pMyStat to svc/processing status

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
inSample set up properly according to data cmd & PP configuration.
PP processing/svc status could be setup accordingly.

NOTES:

===========================================================================*/
ADSPResult AudPP_DataProc_DataBufFormatProcSetUp(ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
   ADSPResult result = ADSP_EOK;
   uint32_t inSizeInSample = 0;
   AudPPInputPort_t *pInPort = me->in_port[port_id];

   uint32_t bytes_per_sample_in = (pInPort->inputMediaFormatInfo.bitsPerSample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   if(pInPort->mediaFormatReceived)
   {
      // if data buffer from upstream service
      elite_msg_data_buffer_t *pInDataMsgDataBuf;

      pInDataMsgDataBuf = (elite_msg_data_buffer_t*) (pInPort->pInDataMsg->pPayload);
      pInPort->inputBufInfo.pBuf = (int8_t *) (&(pInDataMsgDataBuf->nDataBuf));

      QURT_ELITE_ASSERT(pInDataMsgDataBuf->nActualSize >= 0);

      uint32_t bufferSize = (uint32_t)(pInDataMsgDataBuf->nActualSize);

      inSizeInSample = bufferSize / (pInPort->inputMediaFormatInfo.channels * (bytes_per_sample_in));
      bool_t isTimeStampValid = (0 == GET_DATA_BUFFER_TIME_STAMP_BIT(pInDataMsgDataBuf->nFlag)) ? FALSE : TRUE;

      pInPort->inputBufInfo.sizeInSamples = inSizeInSample;
      pInPort->inputBufInfo.offsetInSamples = 0;
      if(isTimeStampValid)
      {
    	  pInPort->inputBufInfo.flags.is_timestamp_valid = isTimeStampValid;
      }
      pInPort->inputBufInfo.timestamp = pInDataMsgDataBuf->ullTimeStamp;

      if (isCompressedInput(me,port_id))
      {
         result = AudPP_CheckAndAdjustCompressedBufSize(me, pInDataMsgDataBuf->nActualSize, pInDataMsgDataBuf->nMaxSize, inSizeInSample);
      }

      if (ADSP_SUCCEEDED(result))
      {
     	  //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : Data buf setup, logging", me->objId);
         /* Logging the input data to PP module */
         result = AudPP_LogData(me,port_id);

         // Do deinterleaving if needed.
         result = AudPP_ProcessFormatConv(&pInPort->formatConverter, pInPort->inputBufInfo.pBuf, inSizeInSample, bytes_per_sample_in);

         if (ADSP_ENOMEMORY == result)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Ran out of memory in format conversion function.", me->objId);
            AudPP_GotoFatalState(me);
         }
      }
   }
   else
   {
      // Not received a media format packet yet, just return with error
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %d: Data buffer received before getting media format command. Dropping the buffer.", me->objId,port_id);
      result = ADSP_ENOTREADY;
   }

   return result;
}



/*===========================================================================
FUNCTION AudPP_DataProc_DataBufFormatProc()

DESCRIPTION
Message handler function for DataBuffer format data command.
PARAMETERS
Input:  me to the AudDynaPPSvc instance

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
For input data buffer, do address mapping if necessary.
Process input data by running through AudProc funcs.
deliver output buffer (if fully filled) to downstream buffer

NOTES:

===========================================================================*/
void AudPP_DataProc_DataBufFormatProc(ThisAudDynaPPSvc_t* me)
{
   ADSPResult result;
   AudPPStatus_t *pMyStat = &me->audPPStatus;
   uint32_t *pMyCurrentBitfield = &pMyStat->unCurrentBitfield;
   uint32_t unMyBufBitfield = pMyStat->unBufBitfield;
   uint32_t unMyDataBitfield =  pMyStat->unCommandOnlyBitField;
   AudPPInputPort_t *pPrimaryInPort = me->in_port[AUDPP_PRIMARY_IN_PORT];
   bool_t consume_only_input = FALSE;


   for(uint32_t port_num=0;port_num<me->numActiveInPorts;port_num++)
   {
	   unMyDataBitfield |= pMyStat->unDataBitfield[port_num];
   }

   *pMyCurrentBitfield = pMyStat->unCommandOnlyBitField;

   if(!(topo_bg_is_bg_thread_processing(me->bg_th_handle)))
   {
      for(uint32_t port_num=0;port_num<me->numActiveInPorts;port_num++)
      {
	     if(me->in_port[port_num]->pInDataMsg != NULL)
	     {
	        if(topo_bg_is_partial_proc_req(me->bg_th_handle, port_num) && (AudPP_isPortRunning(me,port_num)))
	        {
                   consume_only_input = TRUE;
	        }
	     }
      }
   }


   for(uint32_t port_num=0;port_num<me->numActiveInPorts;port_num++)
   {
	   if(me->in_port[port_num]->pInDataMsg == NULL)
	   {
	      *pMyCurrentBitfield |= pMyStat->unDataBitfield[port_num];
		   if(AudPP_isPrimaryInputPort(port_num) && (!consume_only_input))
			   return;
		   else if(AudPP_isPortRunning(me,port_num) && (!consume_only_input))
			   return;
		   else
		   {
			   // Do nothing i.e., do not return if non primary port is not running, return in all other cases when there is no input buffer.
		   }
	   }
   }

   // listen to output field
   *pMyCurrentBitfield = unMyBufBitfield;
   int64_t ss_compensated_drift = 0;
   // only when no current output data buffer possessed, need to listen
   // to empty buffer.
   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: %hX: TODO : Output buf debug 1", me->objId);
   if (pMyStat->pOutDataMsgDataBuf == NULL)
   {
      AudPP_TryGetOutputBuffer(me);
   }    // if (pMyStat->pOutDataMsgDataBuf == NULL)

   //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: %hX: TODO : Output buf debug 2, databuf %x", me->objId,pMyStat->pOutDataMsgDataBuf);
   // below only when pOutDataMsgDataBuf !=NULL can go through
   if (pMyStat->pOutDataMsgDataBuf != NULL)
   {
#ifdef MULTI_PORT_DEBUG
	   if(me->numActiveInPorts > 1)
	   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc : TODO : Data buf, have output buffer!", me->objId);
#endif
      // set up time stamp for output buf when necessary
      QURT_ELITE_ASSERT(!isCompressedInput(me,AUDPP_PRIMARY_IN_PORT) || (pMyStat->outputBufInfo.offsetInSamples == 0));
      if ((pMyStat->outputBufInfo.offsetInSamples == 0)  && (!consume_only_input))
      {
         // data buffer comes from upstream service
         elite_msg_data_buffer_t *pInputBuffer = (elite_msg_data_buffer_t*) (pPrimaryInPort->pInDataMsg->pPayload);
         elite_msg_data_buffer_t *pOutputBuffer = pMyStat->pOutDataMsgDataBuf;
         bool_t isTimeStampValid = (0 == GET_DATA_BUFFER_TIME_STAMP_BIT(pInputBuffer->nFlag)) ? FALSE : TRUE;


         if (isTimeStampValid)
         {
            pOutputBuffer->nFlag = SET_DATA_BUFFER_TIME_STAMP_BIT(pOutputBuffer->nFlag);
            uint32_t algorithmic_delay = 0;
            uint32_t internal_buffer_delay = 0;
            if (NULL != me->pAlgorithmicDelay)
            {
               algorithmic_delay = *me->pAlgorithmicDelay;
            }
            internal_buffer_delay = topo_get_internal_buffer_delay(me->pTopologyObj);

            pOutputBuffer->ullTimeStamp = pInputBuffer->ullTimeStamp +
                                          (((uint64_t)(pPrimaryInPort->inputBufInfo.offsetInSamples)*1000000))/pPrimaryInPort->inputMediaFormatInfo.samplingRate
                                          - internal_buffer_delay - algorithmic_delay;


            if(AudPP_isDevicePath(me))
            {
               uint32_t   usActualParamSize = 0;
               audproc_sampleslip_sample_adjust_t  sampleslippayload;

               if (topo_is_module_present(me->pTopologyObj, AUDPROC_MODULE_ID_SAMPLESLIP))
               {
                  result = topo_get_param(me->pTopologyObj,
                        &sampleslippayload,
                        sizeof(audproc_sampleslip_sample_adjust_t), /* ParamMaxSize */
                        AUDPROC_MODULE_ID_SAMPLESLIP, /* ModuleId */
						0, //This will be ignored as mode is FIRST_INSTANCE
                        AUDPROC_PARAM_ID_SAMPLESLIP_SAMPLE_ADJUST, /* ParamId */
                        &usActualParamSize,
                  	    FIRST_INSTANCE);

                  if (ADSP_EBADPARAM == result) // Error returned from the get_param, as module index is not found in the 	topology.
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc:  SampleSlip Module not part of the Topology", me->objId);
                     sampleslippayload.compensated_drift = 0;
                  }
                  else if (ADSP_ENEEDMORE == result) // Error returned from the sample slip module
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc:  SampleSlip Module : GetParam Failed", me->objId);
                     sampleslippayload.compensated_drift = 0;
                  }
                  else if (ADSP_EOK != result)
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc:  Failed to get the SS drift and update the timestamp", me->objId);
                     sampleslippayload.compensated_drift = 0;
                  }
               }
               else
               {
                  sampleslippayload.compensated_drift = 0;
               }

               if(DYNA_SVC_PP_TYPE_COPREP == me->ppCfgInfo.dynaPPSvcType)
               {
                  if (sampleslippayload.compensated_drift < 0)
                  {
                     pOutputBuffer->ullTimeStamp = (uint64_t)((int64_t)pOutputBuffer->ullTimeStamp - ((int64_t)(sampleslippayload.compensated_drift)>>32));
                     ss_compensated_drift = -((int64_t)(sampleslippayload.compensated_drift)>>32);
                  }
                  else
                  {
                     pOutputBuffer->ullTimeStamp = (uint64_t)((int64_t)pOutputBuffer->ullTimeStamp + ((int64_t)(sampleslippayload.compensated_drift)>>32));
                     ss_compensated_drift = ((int64_t)(sampleslippayload.compensated_drift)>>32);
                  }
               }
               if(DYNA_SVC_PP_TYPE_COPP == me->ppCfgInfo.dynaPPSvcType)
               {
                  if (sampleslippayload.compensated_drift < 0)
                  {
                     pOutputBuffer->ullTimeStamp = (uint64_t)((int64_t)pOutputBuffer->ullTimeStamp + ((int64_t)(sampleslippayload.compensated_drift)>>32));
                     ss_compensated_drift = ((int64_t)(sampleslippayload.compensated_drift)>>32);
                  }
                  else
                  {
                     pOutputBuffer->ullTimeStamp = (uint64_t)((int64_t)pOutputBuffer->ullTimeStamp - ((int64_t)(sampleslippayload.compensated_drift)>>32));
                     ss_compensated_drift = -((int64_t)(sampleslippayload.compensated_drift)>>32);
                  }
               }
            }
         }
         else
         {
            pOutputBuffer->nFlag = CLR_DATA_BUFFER_TIME_STAMP_BIT(pOutputBuffer->nFlag);
            pOutputBuffer->ullTimeStamp = 0;
         }
      }


      //------------------------------------------------------------------------------------------------------------
      // Intermediate Buffers could have samples so empty intermediate buffer to output buffer and then send eos downstream
      //------------------------------------------------------------------------------------------------------------

      uint32_t bytes_per_sample_out = (me->outputMediaFormatInfo.bitsPerSample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

      if (pMyStat->m_fKeepProcessingonEos)
      {
         bool_t m_fdoneEmptyingBuf = FALSE;

         if ((DYNA_SVC_PP_TYPE_COPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_COPREP == me->ppCfgInfo.dynaPPSvcType))
         {
            // Don't empty the internal buffer on the device leg.
            m_fdoneEmptyingBuf = TRUE;
         }
         else if (isCompressedInput(me,AUDPP_PRIMARY_IN_PORT))
         {
            m_fdoneEmptyingBuf = TRUE;
         }
		 // check if POPP and while switchingin gapless
             // if yes then do not flush out history buffers, let the second stream flush the
             // next stream from history buffers
         else if ((DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType) && (switchInGapless(me)))
         {
            m_fdoneEmptyingBuf = TRUE;
         }

         else
         {
            uint32_t initial_outbuf_offset = pMyStat->outputBufInfo.offsetInSamples;
            me->is_processing_data = TRUE;
            QURT_ELITE_ASSERT(1 == me->numActiveInPorts); // TODO : Additional checks here?
            topo_buffer_reset(me->pTopologyObj,
                  &pMyStat->outputBufInfo,
                  &(me->in_port[AUDPP_PRIMARY_IN_PORT]->prevInputBufInfo),
                  &m_fdoneEmptyingBuf,
                  topo_is_bg_thread_created(me->bg_th_handle), me->bg_th_handle);
            me->is_processing_data = FALSE;

            if((initial_outbuf_offset == pMyStat->outputBufInfo.offsetInSamples) &&
                  (!me->is_media_format_change_pending))
            {
               topo_free_eos_buffer(me->pTopologyObj,
                                            &m_fdoneEmptyingBuf);
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: freed the eos buffer ", me->objId);

            }

            pMyStat->pOutDataMsgDataBuf->nActualSize = (int32_t)(pMyStat->outputBufInfo.offsetInSamples *
                  (me->outputMediaFormatInfo.channels) *
                  (bytes_per_sample_out));


            // when output buffer is full
            if (AudPP_isBufferFull(&pMyStat->outputBufInfo))
            {
               pMyStat->pOutDataMsgDataBuf->ullTimeStamp =
                     (uint64_t)(pMyStat->outputBufInfo.timestamp + ss_compensated_drift);
               AudPP_deliverCurrentOutputBuffer(me);
            }

            if (me->is_media_format_change_pending)
            {
               AudPP_UpdateOutputMediaFormat(me, &me->new_media_format);
            }
         }

         //---------------------------------------------------------------------------------------------------------
         // Intermediate Buffer is empty so pass EOS to downstream
         //---------------------------------------------------------------------------------------------------------
         if (m_fdoneEmptyingBuf) //intermediate buffer is empty
         {
#ifdef MULTI_PORT_DEBUG
        	  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : Process call, processing EOS ", me->objId);
#endif
            //Set primary port data bit field along with remaining ports, as EOS is processed now
            *pMyCurrentBitfield = unMyDataBitfield;//(unMyDataBitfield | pMyStat->unDataBitfield[AUDPP_PRIMARY_IN_PORT] | pMyStat->unCommandOnlyBitField);
            pMyStat->m_fKeepProcessingonEos = FALSE;
            (void) AudPP_DataProc_EosFormat(me,AUDPP_PRIMARY_IN_PORT);
         } // if ( pMyStat->SamplesPresentinIntermediateBuf  == 0 )
      } //if (pMyStat->m_fKeepProcessingonEos)
      else
      {
         //------------------------------------------------------------------------------------------------------------
         //Regular processing, input buffer has samples, generate output buffer
         //------------------------------------------------------------------------------------------------------------

          me->is_processing_data = TRUE;
          AudPP_BufInfo_t *inPortBufs[me->numActiveInPorts];
          AudPP_BufInfo_t *outPortBufs[me->numActiveOutPorts];
          for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
          {
        	  inPortBufs[port_id] = &me->in_port[port_id]->inputBufInfo;
          }
          //Assumption is that numActiveOutPorts is 1 for now. If not, output ports have to be created similar to in ports.
          for(uint32_t port_id=0;port_id<me->numActiveOutPorts;port_id++)
          {
        	  outPortBufs[port_id] = &pMyStat->outputBufInfo;
          }

          if(topo_is_bg_thread_created(me->bg_th_handle))
          {
             // remove it if not needed
             for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
             {
                result = topo_bg_thread_reset_input_buffers(me->bg_th_handle, port_id, &me->in_port[port_id]->inputMediaFormatInfo);
                if (ADSP_FAILED(result))
                {
                   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate/reset background thread input buffers.\n", me->objId);
                   AudPP_GotoFatalState(me);
                }
             }

             for(uint32_t port_id=0;port_id<me->numActiveOutPorts;port_id++)
             {
                result = topo_bg_thread_reset_output_buffers(me->bg_th_handle, port_id, &me->outputMediaFormatInfo);
                if (ADSP_FAILED(result))
                {
                   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate/reset background thread output buffers.\n", me->objId);
                   AudPP_GotoFatalState(me);
                }
             }

#ifdef MULTI_PORT_DEBUG
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: input size in samples %lu and output size in samples %lu.", me->objId, inPortBufs[0]->sizeInSamples, outPortBufs[0]->sizeInSamples);
#endif
             result = topo_bg_thread_start_process(me->bg_th_handle,inPortBufs,
                                                outPortBufs);
          }
          else
          {
             //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: input size in samples %lu and output size in samples %lu.", me->objId, inPortBufs[0]->sizeInSamples, outPortBufs[0]->sizeInSamples);
             result = topo_process(me->pTopologyObj,
        		  me->numActiveInPorts,
        		  inPortBufs,
        		  me->numActiveOutPorts,
        		  outPortBufs);
          }
          me->is_processing_data = FALSE;

         if (ADSP_ENOMEMORY == result)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Ran out of memory in process function.", me->objId);
            AudPP_GotoFatalState(me);
            return;
         }

         // set up output buffer meta data when needed.
         QURT_ELITE_ASSERT(pMyStat->pOutDataMsgDataBuf->nActualSize >= 0);

         pMyStat->pOutDataMsgDataBuf->nActualSize = (int32_t)(pMyStat->outputBufInfo.offsetInSamples *
               (me->outputMediaFormatInfo.channels) *
               bytes_per_sample_out);

         // NOTE: changes needed if we're going to support 8bit PCM samples.
         // || AudPP_isBufferFull(&pMyStat->inputBufInfo));
         unMyDataBitfield = 0;//
         for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
         {
        	 QURT_ELITE_ASSERT(!isCompressedInput(me,port_id)); // TODO : below check should include compressed check also?
        	if (AudPP_isBufferFull(&me->in_port[port_id]->inputBufInfo))
        	{
        		AudPP_FreeCurrentInputBuffer(me,port_id);
                // Adjust master mask to listen for data commands again
        		unMyDataBitfield |= pMyStat->unDataBitfield[port_id];
        	}
         }
         if(unMyDataBitfield)
         {
        	 *pMyCurrentBitfield = (unMyDataBitfield | pMyStat->unCommandOnlyBitField);
         }

#ifdef MULTI_PORT_DEBUG
	   if(me->numActiveInPorts > 1)
   	  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : Process done, databitfield %x", me->objId,*pMyCurrentBitfield);
#endif

         // when output buffer is full
         QURT_ELITE_ASSERT(!isCompressedInput(me,AUDPP_PRIMARY_OUT_PORT) || AudPP_isBufferFull(&pMyStat->outputBufInfo));
         if (AudPP_isBufferFull(&pMyStat->outputBufInfo))
         {
            pMyStat->pOutDataMsgDataBuf->ullTimeStamp = (uint64_t)(pMyStat->outputBufInfo.timestamp + ss_compensated_drift);
            AudPP_deliverCurrentOutputBuffer(me);
         }

         if(topo_is_bg_thread_created(me->bg_th_handle))
         {
            for(uint32_t port_id = 0; port_id < me->numActiveOutPorts; port_id++)
            {
               if(topo_bg_is_extra_out_buff_req(me->bg_th_handle, port_id))
               {
                  AudPP_allocate_deliver_extra_out_buffer(me,port_id, ss_compensated_drift);
               }
            }
         }


         if (me->is_media_format_change_pending)
         {
            AudPP_UpdateOutputMediaFormat(me, &me->new_media_format);
         }

      }  //else portion for if (pMyStat->m_fKeepProcessingonEos )
   } // if (pMyStat->pOutDataMsgDataBuf != NULL)
}


static void AudPP_allocate_deliver_extra_out_buffer(ThisAudDynaPPSvc_t *me, uint32_t port_id, int64_t ss_compensated_drifts)
{
    AudPPStatus_t *pMyStat = &me->audPPStatus;
    uint32_t bytes_per_sample_out = (me->outputMediaFormatInfo.bitsPerSample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

    if (pMyStat->pOutDataMsgDataBuf == NULL)
    {
       AudPP_TryGetOutputBuffer(me);
    }     // if (pMyStat->pOutDataMsgDataBuf == NULL)

    if (pMyStat->pOutDataMsgDataBuf != NULL)
    {
       topo_bg_fill_out_buff_info(me->bg_th_handle, &pMyStat->outputBufInfo, port_id);
       QURT_ELITE_ASSERT(pMyStat->pOutDataMsgDataBuf->nActualSize >= 0);

       pMyStat->pOutDataMsgDataBuf->nActualSize = (int32_t)(pMyStat->outputBufInfo.offsetInSamples *
               (me->outputMediaFormatInfo.channels) *
               bytes_per_sample_out);

       if (AudPP_isBufferFull(&pMyStat->outputBufInfo))
       {
          pMyStat->pOutDataMsgDataBuf->ullTimeStamp = (uint64_t)(pMyStat->outputBufInfo.timestamp + ss_compensated_drifts);
          AudPP_deliverCurrentOutputBuffer(me);
       }

    }
    return;
}

static void AudPP_HandleEOSState(ThisAudDynaPPSvc_t *me, uint32_t port_id, bool_t is_eos)
{
    topo_handle_eos(me->pTopologyObj,port_id,is_eos);
}

static void AudPP_sendEos_BgTh(ThisAudDynaPPSvc_t *me, uint32_t port_id, bool_t is_eos)
{
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc port_id %x: Received EOS while BG th is active, sending it to bg thread", me->objId,port_id);
    topo_bg_send_eos_flag(me->bg_th_handle,port_id,is_eos);
    topo_bg_set_dataQ_pending_flag(me->bg_th_handle, TRUE, port_id);
}


ADSPResult AudPP_Alloc_DataMsg_Cache(ThisAudDynaPPSvc_t *me)
{
   uint32_t idx;
   for(idx = 0; idx < me->numActiveInPorts; idx++)
   {
      AudPPInputPort_t *pInPort = me->in_port[idx];
      if(NULL == pInPort->elite_data_msg_cache.pCachedInDataMsg)
      {
         uint32_t req_size = AUDPP_MAX_PENDING_DATA_MSGS*sizeof(elite_msg_any_t);
	 pInPort->elite_data_msg_cache.pCachedInDataMsg = (elite_msg_any_t* )qurt_elite_memory_malloc(req_size, topo_get_heap_id(FALSE, me->ppCfgInfo.dynaPPSvcType));
         if(NULL == pInPort->elite_data_msg_cache.pCachedInDataMsg)
         {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc failed to allocate memory for data msg cache!", me->objId);
             return ADSP_ENOMEMORY;
         }

         memset(pInPort->elite_data_msg_cache.pCachedInDataMsg, 0, req_size);
         pInPort->elite_data_msg_cache.num_pending_data_msgs = 0;
      }
   }
   return ADSP_EOK;
}

void AudPP_Dealloc_DataMsg_Cache(ThisAudDynaPPSvc_t *me)
{
   uint32_t idx;

   AudPP_DataProc_ReleasePendingPackets(me);
   for(idx = 0; idx < me->numActiveInPorts; idx++)
   {
      AudPPInputPort_t *pInPort = me->in_port[idx];
      if(NULL != pInPort->elite_data_msg_cache.pCachedInDataMsg)
      {
         qurt_elite_memory_free(pInPort->elite_data_msg_cache.pCachedInDataMsg);
         pInPort->elite_data_msg_cache.pCachedInDataMsg = NULL;
         pInPort->elite_data_msg_cache.num_pending_data_msgs = 0;
      }
   }
   return;
}


/**
 * Handles the processing of a packet from the data queue.
 *   - Dependencies: There should not be any command currently
 *     being processed.
 *   - Side Effects: Performs actions depending on the command.
 *     If it is a data buffer and cannot be fully processed if
 *     there is no output buffer, the command is remembered in
 *     the pInDataMsg variable.
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *     A pointer to the current instance structure.
 * @param[in] pInDataMsg
 *     Pointer to the input data message
 */
void AudPP_DataProc_ProcessNewPacket(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pInDataMsg,uint32_t port_id)
{

   // remember input data cmd which is being processed.
   AudPPInputPort_t *pInPort = me->in_port[port_id];
   QURT_ELITE_ASSERT(NULL == pInPort->pInDataMsg);
   QURT_ELITE_ASSERT(NULL != pInDataMsg);
   pInPort->pInDataMsg = pInDataMsg;
   //MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: TODO : Received %x command!", me->objId,port_id,pInDataMsg->unOpCode);
   switch (pInDataMsg->unOpCode)
   {
   case ELITE_DATA_MEDIA_TYPE:

      // For current ELite architecture, MEDIA TYPE command is
      // always in ELite internal cmd format.

   {

        //discard FAR media type if EC is not present
       if( !AudPP_isPrimaryInputPort(port_id)  && !topo_is_ec_present(me->pTopologyObj) )
       {
           MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX EC is not present in topo. Discarding media fmt for nonPri port(%x)", me->objId,port_id);
           ADSPResult errCode;
           errCode = elite_msg_release_msg(pInPort->pInDataMsg);
           if (ADSP_FAILED(errCode))
           {
              MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release MediaTypeMsg!", me->objId,port_id);
           }
           pInPort->pInDataMsg = NULL;
           break;
        }

        (void) AudPP_DataProc_MediaFormat(me,port_id);
	   pInPort->eosReceived = FALSE;
	   AudPP_HandleEOSState(me,port_id,pInPort->eosReceived);

	   if(topo_is_bg_thread_created(me->bg_th_handle))
           {
              AudPP_Alloc_DataMsg_Cache(me);
           }
      // still listen to input dataQ, no need to change.
}
      break;

   case ELITE_DATA_EOS:
      // This EOS command is coming from upstream svc, not SC.
   {
	   pInPort->eosReceived = TRUE;
	   if(AudPP_isPrimaryInputPort(port_id))
	   {
		   me->audPPStatus.m_fKeepProcessingonEos = TRUE;
		   AudPP_DataProc_DataBufFormatProc(me);
	   }
	   else
	   {
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: Received EOS on non-primary port!", me->objId,port_id);
		   AudPP_DataProc_EosFormat(me, port_id);
		   AudPP_DataProc_DataBufFormatProc(me); // TODO : Dummy call to consume primary port data and start waiting on primary again
	   }
   }
   // still listen to input dataQ, no need to change.
   break;
   case ELITE_DATA_MARK_BUFFER:
	   // enqueue EOS Msg to downstream bufQ.
	   if(AudPP_isPrimaryInputPort(port_id))
	   {
		   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Audproc service received mark buffer");
		   if (ADSP_FAILED(qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)(pInDataMsg))))
		   {
			   AudPP_DiscardMarkBuffer(me,pInDataMsg);
			   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: Can't enqueue Mark Msg to downstream dataQ!", me->objId,port_id);
		   }
	   }
	   else
	   {
		   AudPP_DiscardMarkBuffer(me,pInDataMsg);
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: Discarding Mark Msg on non-primary input port!", me->objId,port_id);
	   }
	   // finish media format processing, need to reset input indication pointer.
	   pInPort->pInDataMsg = NULL;
	   break;
   case ELITE_DATA_BUFFER:
   {
	   ADSPResult result;

	   result = AudPP_DataProc_DataBufFormatProcSetUp(me,port_id);
	   if (ADSP_SUCCEEDED(result))
	   {
		   if(pInPort->eosReceived == TRUE)
		   {
			   pInPort->eosReceived = FALSE;
			   AudPP_HandleEOSState(me,port_id,pInPort->eosReceived);
		   }
		   AudPP_DataProc_DataBufFormatProc(me);
	   }
	   else
	   {
		   // Just return the packet
		   result = elite_msg_return_payload_buffer(pInDataMsg);
		   if (ADSP_FAILED(result))
		   {
			   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release input buffer", me->objId,port_id);
		   }
		   pInPort->pInDataMsg = NULL;
	   }
   }
   break;

   case ELITE_DATA_SET_PARAM:
      AudPP_DataProc_SetParam(me,port_id);
      break;

   case ELITEMSG_DATA_RESYNC:
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Received resync command", me->objId,port_id);

       if(AudPP_isPrimaryInputPort(port_id))
       {
           //If there is input buffer that has to be consumed, trigger process call.
           if(!(AudPP_isBufferFull(&me->in_port[port_id]->inputBufInfo)))
           {
                AudPP_DataProc_DataBufFormatProc(me);
           }
           //Else, try to deliver any pending output buffers.
           else
           {
               AudPP_deliverCurrentOutputBuffer(me);
           }
       }
       if(ADSP_FAILED(AudPP_HandleResync(me,AUDPP_PRIMARY_IN_PORT)))
       {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to send resync buffer", me->objId,port_id);
       }
       if (ADSP_FAILED(elite_msg_return_payload_buffer(pInDataMsg)))
       {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release input buffer", me->objId,port_id);
       }

       pInPort->pInDataMsg = NULL;
       break;

   default:
      // unsupported data msg, need to release input msg.
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Unsupported Data Cmd type.", me->objId);
      (void) elite_msg_release_msg(pInDataMsg);
      pInPort->pInDataMsg = NULL;
      break;
   } // switch data Msg opcode
}


void AudPP_DataProc_ProcessNewPacket_BGth_active(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pInDataMsg,
	                                                          uint32_t port_id)
{

   // remember input data cmd which is being processed.
   AudPPInputPort_t *pInPort = me->in_port[port_id];
   QURT_ELITE_ASSERT(NULL == pInPort->pInDataMsg);
   QURT_ELITE_ASSERT(NULL != pInDataMsg);
   pInPort->pInDataMsg = pInDataMsg;
   //MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: TODO : Received %x command!", me->objId,port_id,pInDataMsg->unOpCode);
   switch (pInDataMsg->unOpCode)
   {
   case ELITE_DATA_MEDIA_TYPE:

      // For current ELite architecture, MEDIA TYPE command is
      // always in ELite internal cmd format.
{
	AudPP_Add_DataMsg_To_Cache(pInPort, me->objId);
       (void) AudPP_DataProc_MediaFormat_BGTh_Active(me,port_id);
	pInPort->eosReceived = FALSE;
	AudPP_HandleEOSState(me,port_id,pInPort->eosReceived);
      // still listen to input dataQ, no need to change.
}
      break;

   case ELITE_DATA_EOS:
   // This EOS command is coming from upstream svc, not SC.
   {
           AudPP_Add_DataMsg_To_Cache(pInPort, me->objId);
	   pInPort->eosReceived = TRUE;
	   AudPP_sendEos_BgTh(me,port_id,pInPort->eosReceived);
           pInPort->pInDataMsg = NULL;
	   if(!AudPP_isPrimaryInputPort(port_id))
	   {
		   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc port_id %x: Received EOS on non-primary port!. Calling func to wait for prim port data", me->objId,port_id);
		   AudPP_DataProc_DataBufFormatProc(me); // TODO : Dummy call to consume primary port data and start waiting on primary again
	   }

   }
   // still listen to input dataQ, no need to change.
   break;
   case ELITE_DATA_MARK_BUFFER:
	   // enqueue EOS Msg to downstream bufQ.
	   if(AudPP_isPrimaryInputPort(port_id))
	   {
		   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Audproc service received mark buffer");
		   if (ADSP_FAILED(qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)(pInDataMsg))))
		   {
			   AudPP_DiscardMarkBuffer(me,pInDataMsg);
			   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: Can't enqueue Mark Msg to downstream dataQ!", me->objId,port_id);
		   }
	   }
	   else
	   {
		   AudPP_DiscardMarkBuffer(me,pInDataMsg);
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc port_id %x: Discarding Mark Msg on non-primary input port!", me->objId,port_id);
	   }
	   // finish media format processing, need to reset input indication pointer.
	   pInPort->pInDataMsg = NULL;
	   break;
   case ELITE_DATA_BUFFER:
   {
	   ADSPResult result;

	   result = AudPP_DataProc_DataBufFormatProcSetUp(me,port_id);
	   if (ADSP_SUCCEEDED(result))
	   {
		   if(pInPort->eosReceived == TRUE)
		   {
			   pInPort->eosReceived = FALSE;
			   AudPP_HandleEOSState(me,port_id,pInPort->eosReceived);
		   }
		   AudPP_DataProc_DataBufFormatProc(me);
	   }
	   else
	   {
		   // Just return the packet
		   result = elite_msg_return_payload_buffer(pInDataMsg);
		   if (ADSP_FAILED(result))
		   {
			   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Failed to release input buffer", me->objId,port_id);
		   }
		   pInPort->pInDataMsg = NULL;
	   }
   }
   break;

   case ELITE_DATA_SET_PARAM:
    {
	   AudPP_Add_DataMsg_To_Cache(pInPort, me->objId);
	   topo_bg_set_dataQ_pending_flag(me->bg_th_handle, TRUE, port_id);
	   pInPort->pInDataMsg = NULL;
   	}
      break;

   default:
      // unsupported data msg, need to release input msg.
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Unsupported Data Cmd type.", me->objId);
      (void) elite_msg_release_msg(pInDataMsg);
      pInPort->pInDataMsg = NULL;
      break;
   } // switch data Msg opcode
}


static void AudPP_Add_DataMsg_To_Cache(AudPPInputPort_t *pInPort, uint16_t objId)
{
   if(pInPort->elite_data_msg_cache.num_pending_data_msgs < AUDPP_MAX_PENDING_DATA_MSGS)
   {
 	   memscpy(&pInPort->elite_data_msg_cache.pCachedInDataMsg[pInPort->elite_data_msg_cache.num_pending_data_msgs++],
	           sizeof(elite_msg_any_t), pInPort->pInDataMsg, sizeof(elite_msg_any_t));
   }
   else
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX AudPP_Add_DataMsg_To_Cache: Data msg cache is full", objId);
   }
   return;
}

/*===========================================================================
FUNCTION AudPP_SoftPause_Start()

DESCRIPTION
start soft pause timer
PARAMETERS
Input:  me to the AudDynaPPSvc instance

RETURN VALUE: None.

===========================================================================*/
ADSPResult AudPP_SoftPause_Start(ThisAudDynaPPSvc_t *me)
{
   ADSPResult result = ADSP_EFAILED;

   uint32_t   unOutBufDurationMs = 0;
   uint32_t   usActualParamSize = 0;
   audproc_soft_pause_params_t softPauseParamPayload;

   /* PP svc get the duration from CSoftVolMultichannelConfig (CSoftVolMultichannelLib.h) */
   result = topo_get_param(me->pTopologyObj,
         &softPauseParamPayload,
         sizeof(audproc_soft_pause_params_t), /* ParamMaxSize */
         AUDPROC_MODULE_ID_VOL_CTRL, /* ModuleId: SoftPause is done via SoftVolumeControls */
		 0, //This will be ignored as mode is set to FIRST_INSTANCE
         AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS, /* ParamId */
         &usActualParamSize,
		 FIRST_INSTANCE);
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: soft pause failed getparam", me->objId);
      return result;
   }

   // MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: softpause enable=%d, period=%d, step=%d, curve=%d",
   //        me->objId,
   //        softPauseParamPayload.enable_flag,
   //        softPauseParamPayload.period,
   //        softPauseParamPayload.step,
   //        softPauseParamPayload.ramping_curve);

   if (0 == softPauseParamPayload.enable_flag)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: soft pause not enabled", me->objId);
      return ADSP_EFAILED;
   }

   result = qurt_elite_timer_oneshot_cancel( &(me->softPauseTimer) );
   if (ADSP_FAILED(result))
   {
      /* timer cancel may fail in case it's not yet started/already finished,
       * hence this is not an error */
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "P%hX audproc_svc: soft pause timer could not be cancelled/currently inactive", me->objId);
   }

   if (FALSE == me->in_port[AUDPP_PRIMARY_IN_PORT]->mediaFormatReceived)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Soft pause not Ready yet, issuing normal pause", me->objId);
      return ADSP_EFAILED;
   }

   //Derive all frame durations from the AFE Frame Size
   uint32_t unAfeFrameSize;
   uint32_t unFreq = 48000;
   if(me->outputMediaFormatInfo.samplingRate)
   {
	   unFreq = me->outputMediaFormatInfo.samplingRate;
   }

   elite_svc_get_frame_size(unFreq, &unAfeFrameSize);

   unOutBufDurationMs = ((me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames)* unAfeFrameSize * 1000)/(unFreq);

   // We need a guard duration to allow the matrix to play out the data we send
   const int64_t MATRIX_INTERNAL_BUF_DURATION = (5 * unAfeFrameSize * 1000)/(unFreq);// ms
   int64_t guardDurationMs = me->nBufsAllocated * unOutBufDurationMs + MATRIX_INTERNAL_BUF_DURATION;

   int64_t durationUs = (softPauseParamPayload.period + guardDurationMs)*1000;
   result = qurt_elite_timer_oneshot_start_duration(&(me->softPauseTimer), durationUs);

   if (ADSP_EOK==result)
   {
      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: soft pause buffer duration=%d", me->objId, unOutBufDurationMs);

      me->audPPStatus.nSoftPauseBufCounter = (softPauseParamPayload.period / unOutBufDurationMs) + 1;
      topo_set_start_pause(me->pTopologyObj, &softPauseParamPayload);

      //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Soft Pause after getparam &me=0x%x, me=0x%x", me->objId, &me, me);

      me->audPPStatus.unCurrentBitfield |= (me->audPPStatus.unSoftPauseSignalOnlyBitfield);
      me->audPPStatus.rampOnResume = TRUE;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Soft Pause started with %d", me->objId, result);

   return result;
}

/*===========================================================================
FUNCTION AudPP_SoftPause_Terminate()

DESCRIPTION
Soft pause timer expired or the desired number of buffers are already delivered,
 set PP into sleep state.
PARAMETERS
Input:  me to the AudDynaPPSvc instance

RETURN VALUE: None.

===========================================================================*/
static void AudPP_SoftPause_Terminate(ThisAudDynaPPSvc_t *me)
{
   if (NULL != me->audPPStatus.pOutDataMsgDataBuf)
   {
      /*
       * Free up any partially filled output buffer. If we are holding a buffer
       * at this point, it means that the input got delayed somehow. Playing it
       * back will cause a pop. So just discard it.
       */
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Soft Pause terminated, freeing partial output buffer %8lx ", me->objId, (uint32) me->audPPStatus.pOutDataMsgDataBuf);
      AudPP_FreeCurrentOutputBuffer(me);
   }

   //see if we need to send down an EOS marker
   EliteMsg_CustomPPPausePrepareType *pCustomPauseMsg = me->pPauseMsgPayload;

   if(pCustomPauseMsg->sendDownEosMarker)
   {
      ADSPResult result;
      MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX: AudProc received pause, generating internal eos ", me->objId);
      result = AudPP_GenerateEos(me);
      if (ADSP_FAILED(result))
      {
         MSG_1( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"P%hX audproc_svc: Unable to send EOS message. Aborting pause.", me->objId);
         return ;
      }
   }

   //reset the buffer counter to zero
   me->audPPStatus.nSoftPauseBufCounter = 0;

   AudPP_GotoSleepState(me);
   topo_set_volume1_state_to_pause(me->pTopologyObj);
   return;
}

void AudPP_SoftPauseTimerExpireHandler(ThisAudDynaPPSvc_t *me)
{
   qurt_elite_signal_clear(&(me->softPauseExpireSignal));
   if (ADSP_FAILED(qurt_elite_timer_oneshot_cancel(&(me->softPauseTimer))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: failed cancel soft pause timer on termination", me->objId);
   }


   AudPP_SoftPause_Terminate(me);

   if (NULL == me->pPauseMsgPayload)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: NULL Pause message ", me->objId);
      return;
   }

   elite_msg_any_t pauseMsg;
   pauseMsg.unOpCode = ELITE_CUSTOM_MSG;
   pauseMsg.pPayload = (void*) (me->pPauseMsgPayload);

   elite_svc_send_ack(&pauseMsg, ADSP_EOK);

   me->pPauseMsgPayload = NULL;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause Handler End with Timer expired, sent ack", me->objId);
}

ADSPResult AudPP_GenerateEos(ThisAudDynaPPSvc_t *me)
{
   elite_msg_any_t eosMsg;
   ADSPResult result;
   uint32_t unEosPayloadSize = sizeof( elite_msg_data_tx_eos_t );

   result = elite_msg_create_msg( &eosMsg, &unEosPayloadSize,
         ELITE_DATA_EOS,
         NULL,
         0,
         ADSP_EFAILED );
   if (ADSP_FAILED(result))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"P%hX audproc_svc: failed to create eos message", me->objId);
      return result;
   }

   elite_msg_data_tx_eos_t *pEosPayload = (elite_msg_data_tx_eos_t *) (eosMsg.pPayload);
   pEosPayload->unEosFormat   = ELITEMSG_DATA_TX_EOS;

   result = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)&eosMsg);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Error in sending tx eos !", me->objId);
      return result;
   }

   return ADSP_EOK;
}

/**
 * Handles the output format message.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *   Pointer to the instance structure.
 * @param[in] pMsg
 *   Pointer to the output media type config message.
 *
 * @return ADSPResult
 *   Error code indicating status
 */
ADSPResult AudPP_ProcessOutputFormatMsg(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pMsg)
{
   ADSPResult result;

   const EliteMsg_CustomCfgPPOutputType *pOutputCfgMsg = (const EliteMsg_CustomCfgPPOutputType*)(pMsg->pPayload);

   result = AudPP_setOutputMediaInfo(me, &(pOutputCfgMsg->params));

   elite_svc_send_ack(pMsg, result);

   return result;
}

ADSPResult AudPP_ProcessSetOutputSamplingRate(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pMsg)
{
   ADSPResult result = ADSP_EOK;

   const EliteMsg_CustomCfgPPOutputSampleRateType *pOutputCfgMsg = (const EliteMsg_CustomCfgPPOutputSampleRateType*)(pMsg->pPayload);

   AudPPSvcOutputMediaTypeParams_t params = me->ppCfgInfo.outputParams;
   params.keepInputSamplingRate = pOutputCfgMsg->keepInputSamplingRate;
   params.outputSamplingRate = pOutputCfgMsg->outputSamplingRate;

   result = AudPP_setOutputMediaInfo(me, &params);

   elite_svc_send_ack(pMsg, result);
   return result;
}

/**
 * Callback function called by static services to directly
 * queue a message into PP. This function is meant to be called
 * from APR's context.
 *   - Dependencies: PP must be correctly initialized.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pCtxt
 *   This should be a pointer to the PP ThisAudDynaPPSvc_t
 *   structure.
 * @param[in] msg
 *   APR message to be queued.
 *
 * @return ADSPResult
 *   Error code indicating status
 */
ADSPResult AudPP_AprMsgCallback(void *pCtxt, elite_msg_any_t msg)
{
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)(pCtxt);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: APR Message callback Begin", me->objId);

   if (msg.unOpCode != ELITE_APR_PACKET)
   {
      return ADSP_EUNEXPECTED;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: APR Message callback End", me->objId);
   return ADSP_EUNSUPPORTED;
}

static bool_t AudPP_isBufferFull(const AudPP_BufInfo_t *pBufInfo)
{
   return (pBufInfo->offsetInSamples == pBufInfo->sizeInSamples);
}

/**
 * Goes through the multichannel PCM media type packet and converts
 * to the internal media format type.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pFormat
 * Pointer to the multichannel PCM format block.
 *
 * @return AdspAudioMediaFormatInfo_t
 * The media format in the internal type.
 */
static AdspAudioMediaFormatInfo_t AudPP_ParseMultichannelPCMMediaFmt(const elite_multi_channel_pcm_fmt_blk_t *pFormat)
{
   AdspAudioMediaFormatInfo_t mediaInfo;

   mediaInfo.channels = pFormat->num_channels;
   memscpy(mediaInfo.channelMapping,sizeof(mediaInfo.channelMapping), pFormat->channel_mapping, sizeof(mediaInfo.channelMapping));
   mediaInfo.bitsPerSample = pFormat->bits_per_sample;
   mediaInfo.isSigned      = pFormat->is_signed;
   mediaInfo.isInterleaved = pFormat->is_interleaved;
   mediaInfo.samplingRate  = pFormat->sample_rate;
   mediaInfo.bitstreamFormat = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;

   return mediaInfo;
}

/**
 * Goes through the compressed PCM media type packet and converts
 * to the internal media format type.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pFormat
 * Pointer to the compressed PCM format block.
 *
 * @return AdspAudioMediaFormatInfo_t
 * The media format in the internal type.
 */
static AdspAudioMediaFormatInfo_t AudPP_ParseCompressedMediaFmt(const elite_compressed_fmt_blk_t *pFormat)
{
   AdspAudioMediaFormatInfo_t mediaInfo;

   mediaInfo.channels = pFormat->num_channels;
   for (uint32_t i = 0; i < SIZE_OF_ARRAY(mediaInfo.channelMapping); i++)
   {
      mediaInfo.channelMapping[i] = PCM_CHANNEL_INVALID;
   }
   mediaInfo.bitsPerSample = pFormat->bits_per_sample;
   mediaInfo.isSigned      = 1;
   mediaInfo.isInterleaved = 1;
   mediaInfo.samplingRate  = pFormat->sample_rate;
   mediaInfo.bitstreamFormat = pFormat->media_format;

   return mediaInfo;
}

ADSPResult AudPP_sendMediaTypeMsg(ThisAudDynaPPSvc_t *me)
{
   elite_msg_any_t msg;
   ADSPResult result;

   if (NULL == me->pDownstreamPeer)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: Downstream connection not established. Will not send media type.", me->objId);
      return ADSP_EOK;
   }

   uint32_t size = sizeof(elite_multi_channel_pcm_fmt_blk_t);
   if (isCompressedFormat(&me->outputMediaFormatInfo))
   {
      size = sizeof(elite_compressed_fmt_blk_t);
   }

   uint32_t nPayloadSize = sizeof(elite_msg_data_media_type_apr_t) + size;
   result = elite_msg_create_msg(&msg,&nPayloadSize, ELITE_DATA_MEDIA_TYPE, NULL,0,0 ) ;
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: Fail to create media fmt message", me->objId);
      return result;
   }

   elite_msg_data_media_type_apr_t *pCmdMsgPayload = (elite_msg_data_media_type_apr_t*) (msg.pPayload);
   pCmdMsgPayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;

   if (isCompressedFormat(&me->outputMediaFormatInfo))
   {
      pCmdMsgPayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_COMPRESSED;

      elite_compressed_fmt_blk_t *pFormatBlock =
            (elite_compressed_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));
      AdspAudioMediaFormatInfo_t *pOutputFmt = &me->outputMediaFormatInfo;

      pFormatBlock->num_channels = pOutputFmt->channels;
      pFormatBlock->bits_per_sample = pOutputFmt->bitsPerSample;
      pFormatBlock->sample_rate = pOutputFmt->samplingRate;
      pFormatBlock->media_format = pOutputFmt->bitstreamFormat;
   }
   else
   {
      pCmdMsgPayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

      elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock =
            (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));
      AdspAudioMediaFormatInfo_t *pOutputFmt = &me->outputMediaFormatInfo;

      pPcmFormatBlock->num_channels = pOutputFmt->channels;
      memscpy( pPcmFormatBlock->channel_mapping,  sizeof(pPcmFormatBlock->channel_mapping) , pOutputFmt->channelMapping, sizeof(pOutputFmt->channelMapping));
      pPcmFormatBlock->bits_per_sample = pOutputFmt->bitsPerSample;
      pPcmFormatBlock->sample_rate = pOutputFmt->samplingRate;
      pPcmFormatBlock->is_signed = pOutputFmt->isSigned;
      pPcmFormatBlock->is_interleaved = pOutputFmt->isInterleaved;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Sending MediaTypeMsg downstream of type %lu", me->objId, pCmdMsgPayload->unMediaFormatID);
   result = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)&msg );
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can't enqueue MediaTypeMsg to downstream dataQ!", me->objId);
   }

   return result;
}

static bool AudPP_isDevicePath(const ThisAudDynaPPSvc_t *me)
{
   return ((DYNA_SVC_PP_TYPE_COPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_COPREP == me->ppCfgInfo.dynaPPSvcType));
}

/**
 * Handles the data path set param message.
 *   - Dependencies: None
 *   - Side Effects: Any partially filled output buffer is sent.
 *   - Re-entrant: Yes
 *
 * @param[in] me
 *   Pointer to the instance object.
 */
static void AudPP_DataProc_SetParam(ThisAudDynaPPSvc_t *me,uint32_t port_id)
{
   AudPPInputPort_t *pInPort = me->in_port[port_id];
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Data Set param start", me->objId,port_id);

   // First send down any pending partial output buffer
   AudPP_deliverCurrentOutputBuffer(me);

   ADSPResult result = ADSP_EOK;
   elite_msg_any_t* pInPortDataMsg = pInPort->pInDataMsg;

   elite_msg_data_set_param_t *pMsgPkt = (elite_msg_data_set_param_t*)(pInPortDataMsg->pPayload);

   // Call set param on the topology
   result = topo_set_param(me->pTopologyObj, pMsgPkt + 1, pMsgPkt->unPayloadSize, FIRST_INSTANCE);
   if (ADSP_FAILED(result))
   {
      // This message may contain params for some modules in downstream services, so this is not
      // an error case.
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "P%hX audproc_svc: Error in data path set param!", me->objId);
   }

   // Send the message downstream, since some missing modules may be present downstream
   result = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)(pInPortDataMsg));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can't enqueue data path set param Msg to downstream dataQ!", me->objId);
      result = elite_msg_release_msg(pInPortDataMsg);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Couldn't release the data set param message!", me->objId);
      }
   }

   pInPort->pInDataMsg = NULL;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc in_port %x: Data Set param end", me->objId,port_id);
}

static ADSPResult AudPP_HandleResync(ThisAudDynaPPSvc_t *me, uint32_t port_id)
{
    elite_msg_any_t resyncMsg;
    uint32_t unPayloadSize = sizeof(elite_msg_data_resync_t);
    ADSPResult nRes = ADSP_EOK;
    nRes = elite_msg_create_msg( &resyncMsg,
               &unPayloadSize,
               ELITEMSG_DATA_RESYNC,
               NULL,
               0,
               0);

    if(ADSP_FAILED(nRes))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can't create Resync Msg!", me->objId);
        return nRes;
    }

    // enqueue Resync Msg to downstream bufQ.
    nRes = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)(&resyncMsg));
    if (ADSP_FAILED(nRes))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can't enqueue Resync Msg to downstream dataQ!", me->objId);
        elite_msg_release_msg(&resyncMsg);
        return nRes;
    }
    return nRes;

}
