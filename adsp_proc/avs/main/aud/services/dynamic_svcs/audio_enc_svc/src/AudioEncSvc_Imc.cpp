/*========================================================================

 *//** @file AudioEncSvc_Imc.cpp
 This file contains IMC functions for encoder

Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
 Edit History

 $Header: /


 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 02/07/18   npendeka      Created file.

 ========================================================================== */

#include "AudioEncSvc_PushMode.h"
#include "AudioEncSvc_CapiV2Util.h"
#include "inter_module_comm.h"

/*--------------------------------------------------------------------------
 Global Variables
 ----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Local function declarations
 ** ----------------------------------------------------------------------- */
capi_v2_err_t AudioEncSvc_IMCRegisterDest(imc_src_dst_info_t *imc_src_dst_info, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,imc_src_dst_t src_dst);

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

/**
  @brief Perform IMC enable/disable depending on received param

  if enable, calls AudioEncSvc_IMCRegisterDest()
  if diable, calls AudioEncSvc_IMCDestroy()

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult AudioEncSvc_IMCCheckInit(AudioEncSvc_t* me_ptr, elite_msg_param_cal_t *param_msg_ptr)
{
	ADSPResult result= ADSP_EOK;
	capi_v2_err_t res = CAPI_V2_EOK;
	dec_to_enc_communication_payload_t* payload_ptr = (dec_to_enc_communication_payload_t*)param_msg_ptr->pnParamData;
    int8_t * actual_key = ((int8_t*)payload_ptr + sizeof(payload_ptr->comm_dir) + sizeof(payload_ptr->enable));
    uint32_t actual_key_size = sizeof(payload_ptr->purpose) + sizeof(payload_ptr->comm_instance);

	imc_src_dst_t src_dest= IMC_DESTINATION;

	if(payload_ptr->purpose != AVS_ENCDEC_PURPOSE_ID_BT_INFO)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_RX: Error! Received unsupported purpose id during init");
	   result = ADSP_EUNSUPPORTED;
	   return result;
	}
	else
	{
	   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_RX: received init command for IMC, param length(%lu)",param_msg_ptr->unSize);
	   /* Check if flag is for enabling or disabling IMC connection*/
	   if( payload_ptr->enable)
	   {
	      /* if IMC pointer is not NULL and connection is already opened*/
		  if((me_ptr->imc_info_ptr) && ((me_ptr->imc_info_ptr->state == IMC_STATE_SOURCE_OPENED_COMMUNICATION) || (me_ptr->imc_info_ptr->state == IMC_STATE_REGISTERED)))
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_RX: Communication already enabled");
		  }
		  else
		  {
			 /* Register for IMC */
			 if(payload_ptr->comm_dir == RECEIVE)
		     {
		        if(me_ptr->imc_info_ptr)
		        {
		           MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AUD_IMC_RX: Error! Trying to allocate and initialize again. Destination IMC already set up");
		           return ADSP_EFAILED;
		        }
		        src_dest = IMC_DESTINATION;
		        me_ptr->imc_info_ptr = (imc_src_dst_info_t*)qurt_elite_memory_malloc(sizeof(imc_src_dst_info_t), QURT_ELITE_HEAP_DEFAULT);
		        if(NULL == me_ptr->imc_info_ptr)
		        {
		        	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_RX: Error! IMC pointer is null");
		        	return ADSP_EFAILED;
		        }
		        me_ptr->imc_info_ptr->state = IMC_STATE_NONE;
		        res = AudioEncSvc_IMCRegisterDest(me_ptr->imc_info_ptr, actual_key,actual_key_size,MAX_BUF_Q_ELEMENTS, src_dest);
		     }
    		 else
		     {
		        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AUD_IMC_RX: Error! Encoder needs to transmit and not receive data");
		        result = ADSP_EFAILED;
		     }
	      }
	   }
	   else
	   {
	      /* if disable communication param is received*/
		  if((me_ptr->imc_info_ptr) && ((me_ptr->imc_info_ptr->state == IMC_STATE_SOURCE_OPENED_COMMUNICATION) || (me_ptr->imc_info_ptr->state == IMC_STATE_REGISTERED)))
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_RX: Closing communication, received disable flag ");
		     AudioEncSvc_IMCDestroy(me_ptr);
		  }
		  else
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_RX: Communication already disabled");
     	  }
	   }
	}
	if(CAPI_V2_FAILED(res))
    {
	   result = ADSP_EFAILED;
    }

	return result;
}

/**
  @brief Performs IMC key registration for destination

  @return  CAPI_V2_EOK on success, an error code on error

 */
capi_v2_err_t AudioEncSvc_IMCRegisterDest(imc_src_dst_info_t *imc_src_dst_info, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,imc_src_dst_t src_dst)
{
	capi_v2_err_t imc_result = CAPI_V2_EOK;

	//Check if the key is already received
    if(IMC_STATE_NONE != imc_src_dst_info->state)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Error!! trying to set multiple key values for same instance, IMC state(%d)",(int)imc_src_dst_info->state);
        return CAPI_V2_EBADPARAM;
    }

    //Clear Key
    memset(imc_src_dst_info->key,0,sizeof(imc_src_dst_info->key));
    imc_src_dst_info->key_length = keylen;
    memscpy(imc_src_dst_info->key,sizeof(imc_src_dst_info->key),key,keylen);

    if(src_dst == IMC_DESTINATION)
    {
       //Now register for intermodule communication
       imc_result = imc_dest_register(&imc_src_dst_info->dest_handle,(int8_t*)imc_src_dst_info->key,imc_src_dst_info->key_length,max_bufq_len);
       if(CAPI_V2_FAILED(imc_result))
       {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," AUD_IMC_RX: Error!! destination failed to register, result(0x%lx)",imc_result);
           return imc_result;
       }
       else
       {
           imc_src_dst_info->state =  IMC_STATE_REGISTERED;
           MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_RX: Destination registered for IMC");
       }
    }
    else
	{
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_RX: Error! Encoder should be IMC destination");
       imc_result= CAPI_V2_EFAILED;
	}
    return imc_result;
}

/**
  @brief Receiving IMC buffers on destination, does set param to appropriate module

 */
void AudioEncSvc_HandleIMCBeforeProcess(AudioEncSvc_t* me_ptr)
{
	if(NULL == me_ptr->imc_info_ptr)
	{
		return;
	}
	capi_v2_err_t imc_result, resp_result = CAPI_V2_EOK;
    ADSPResult res = ADSP_EOK;
    dec_to_enc_communication_payload_header_t *payload_header = NULL;
	uint32_t payload_size,payload_max_size;
	bool_t is_queue_empty = FALSE;
	imc_src_dst_info_t *imc_handle = me_ptr->imc_info_ptr;

	if(me_ptr->imc_info_ptr->state!= IMC_STATE_REGISTERED)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Error! IMC destination still not registered");
		return;
	}

	//until queue is empty keep popping buffers at destination so that encoder can chose last received bit rate level to encode the next frame
	while(!is_queue_empty)
	{
	   if(payload_header)
	   {
	      imc_result = imc_dest_return_buf(imc_handle->dest_handle,payload_header,resp_result);
		  if(CAPI_V2_FAILED(imc_result))
		  {
		     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," AUD_IMC_RX: Error in returning buffer to source. result(%lx)",imc_result);
			 return;
		  }
	   }
	   //pop destination buffers
	   imc_result = imc_dest_pop_buf(imc_handle->dest_handle,(void **)&payload_header,&payload_size,&payload_max_size,&is_queue_empty);
	   if(CAPI_V2_FAILED(imc_result))
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," AUD_IMC_RX: Error in popping IMC payload, result(0x%lx)",imc_result);
	      return;
	   }
    }

	//If payload header isn't populated yet, return
	if(!payload_header)
	{
		return;
	}
	else
	{
	   switch(payload_header->opcode)
	   {
	      case AVS_ENC_SVC_IMC_SET_PARAM:
	      {
	    	 dec_to_enc_communication_set_param_payload_t *payload_ptr = (dec_to_enc_communication_set_param_payload_t*)((int8_t*)payload_header + sizeof(dec_to_enc_communication_payload_header_t));
	     	 if(payload_ptr->purpose == AVS_ENCDEC_PURPOSE_ID_BT_INFO)
	     	 {
	     		dec_to_enc_comm_bit_rate_level_payload_t *data_payload = (dec_to_enc_comm_bit_rate_level_payload_t*)((int8_t*)payload_ptr + sizeof(dec_to_enc_communication_set_param_payload_t));
	     		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_RX: bit rate level received in payload ptr (%lx)",data_payload->level);

	     		if(payload_ptr->param_size < sizeof(dec_to_enc_comm_bit_rate_level_payload_t))
	     	  	{
	     	    	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Payload has insufficient size");
	     	    	return;
	     	  	}
	     		//Check io fmt to send to encoder capi followed by cop packetizer
	     	    if(me_ptr->io_fmt_conv == ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_COP)
	     	    {
	     	       if(me_ptr->capiContainer[0]->capi_ptr)
	     	       {
	                  res = aud_enc_svc_set_param(me_ptr->capiContainer[0]->capi_ptr, payload_ptr->param_id , (int8_t*)data_payload, payload_ptr->param_size); //sending bit rate level info only
	                  if(ADSP_FAILED(res))
	                  {
	                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: aud enc svc set param failed");
	                	 return;
	                  }
    	           }
	     	       else
	     	       {
	     	    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Error! Encoder CAPI ptr is NULL ");
	     	       }
      	        }
	     	    else
	     	    {
	     	    	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Error! Wrong io fmt conv for encoder received ");
	     	    }
             }
	     	 else
	     	 {
	     		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Unsupported payload purpose (0x%lx) received in imc message ",payload_ptr->purpose);
	     	 }
            break;
	      }
	      default:
	      {
	         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Unsupported payload id(0x%lx) received in imc message",payload_header->opcode);
	         break;
	      }
	   }
	}
}


/**
  @brief De-register destination IMC and free memory

 */
void AudioEncSvc_IMCDestroy(AudioEncSvc_t* me_ptr)
{
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " AUD_IMC_RX: destination imc destroy");
	capi_v2_err_t result = CAPI_V2_EOK;

	if(IMC_STATE_NONE != me_ptr->imc_info_ptr->state)
	{
	   result= imc_dest_deregister( me_ptr->imc_info_ptr->dest_handle);
	   if(CAPI_V2_FAILED(result))
	   {
	      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_RX: Error! Destination de-register failed ");
		  return;
	   }
	   me_ptr->imc_info_ptr->state = IMC_STATE_NONE;
	}

	qurt_elite_memory_free(me_ptr->imc_info_ptr);
	me_ptr->imc_info_ptr = NULL;
}
