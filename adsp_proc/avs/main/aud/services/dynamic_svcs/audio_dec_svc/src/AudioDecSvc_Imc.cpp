/*========================================================================

 *//** @file AudioEncSvc_Imc.cpp
 This file contains IMC functions for decoder

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
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "inter_module_comm.h"
#include "AudioDecSvc_Structs.h"
#include "adsp_avs_common_api.h"
#include "AudioEncSvc.h"
/*--------------------------------------------------------------------------
 Global Variables
 ----------------------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
#define  ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)

/* -----------------------------------------------------------------------
 ** Local function declarations
 ** ----------------------------------------------------------------------- */
capi_v2_err_t AudioDecSvc_IMCRegisterSrc(imc_src_dst_info_t *imc_src_dst_info, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32_t buf_size,imc_src_dst_t src_dest);

/* -----------------------------------------------------------------------
 ** Function definitions
 ** ----------------------------------------------------------------------- */

/**
  @brief Perform IMC enable/disable depending on received param

  if enable, calls AudioDecSvc_IMCRegisterSrc()
  if diable, calls AudioDecSvc_IMCDestroy()

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult AudioDecSvc_IMCCheckInit(AudioDecSvc_t* me_ptr, elite_msg_param_cal_t *param_msg_ptr)
{
	ADSPResult result= ADSP_EOK;
	capi_v2_err_t res = CAPI_V2_EOK;
	dec_to_enc_communication_payload_t* payload_ptr = (dec_to_enc_communication_payload_t*)param_msg_ptr->pnParamData;
	uint32_t max_payload_size,max_q_length,alloc_buffer;
	imc_src_dst_t src_dest;

	/*Extract the IMC key (purpose + communication instance)*/
	int8_t * comm_key = ((int8_t*)payload_ptr + sizeof(payload_ptr->comm_dir) + sizeof(payload_ptr->enable));
	uint32_t comm_key_size = sizeof(payload_ptr->purpose) + sizeof(payload_ptr->comm_instance);

	/* Check purpose of the message*/
	if(payload_ptr->purpose != AVS_ENCDEC_PURPOSE_ID_BT_INFO)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX: Error! Received unsupported purpose id for imc init");
	   result = ADSP_EUNSUPPORTED;
	   return result;
	}
	else
	{
	   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: received init command for IMC, param length(%lu)",param_msg_ptr->unSize);

	   /* Check if flag is for enabling or disabling IMC connection*/
	   if(payload_ptr->enable)
	   {
		  /* if IMC pointer is not NULL and connection is already opened*/
		  if((me_ptr->imc_info_ptr) && ((me_ptr->imc_info_ptr->state == IMC_STATE_SOURCE_OPENED_COMMUNICATION) || (me_ptr->imc_info_ptr->state == IMC_STATE_REGISTERED)))
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: Communication already enabled");
		  }
		  else
		  {
			 /* Register for IMC */
		     if(payload_ptr->comm_dir == TRANSMIT)
			 {
			    src_dest = IMC_SOURCE;
			    if(me_ptr->imc_info_ptr)
			    {
			   	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX: Error! trying to allocate and initialize again. Source IMC already set up");
			   	   return ADSP_EFAILED;
			    }

			    //Allocating memory for source IMC struct
			    me_ptr->imc_info_ptr = (imc_src_dst_info_t*)qurt_elite_memory_malloc(sizeof(imc_src_dst_info_t), QURT_ELITE_HEAP_DEFAULT);
			    if(NULL == me_ptr->imc_info_ptr)
			   	{
			   	   	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX: Error! IMC pointer is null");
			   	   	return ADSP_EFAILED;
			   	}
			    me_ptr->imc_info_ptr->state = IMC_STATE_NONE;
     		    max_q_length = alloc_buffer = AUD_DEC_SVC_MAX_BUF_Q_ELEMENTS;
			    max_payload_size = ROUNDTO8(sizeof(dec_to_enc_communication_payload_header_t) + sizeof(dec_to_enc_communication_set_param_payload_t) + sizeof(dec_to_enc_comm_bit_rate_level_payload_t));
			    res= AudioDecSvc_IMCRegisterSrc(me_ptr->imc_info_ptr, comm_key, comm_key_size , max_q_length, alloc_buffer,max_payload_size, src_dest);
			 }
			 else
			 {
			    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX: Error! Decoder needs to transmit and not receive data");
			    result= ADSP_EFAILED;
			    return result;
			 }
		  }
	   }
	   else
	   {
		  /* If disable communication param is received*/
		  if((me_ptr->imc_info_ptr) && ((me_ptr->imc_info_ptr->state == IMC_STATE_SOURCE_OPENED_COMMUNICATION) || (me_ptr->imc_info_ptr->state == IMC_STATE_REGISTERED)))
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: Closing communication, received disable flag ");
		     AudioDecSvc_IMCDestroy(me_ptr);
		  }
		  else
		  {
		     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: Communication already disabled");
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
  @brief Performs IMC key registration for source and also attempts to open communication

  @return  CAPI_V2_EOK on success, an error code on error

 */
capi_v2_err_t AudioDecSvc_IMCRegisterSrc(imc_src_dst_info_t *imc_src_dst_info, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32_t buf_size, imc_src_dst_t src_dst)
  {
	capi_v2_err_t imc_result, result= CAPI_V2_EOK;

	//Check if the key is already received
	if(IMC_STATE_NONE != imc_src_dst_info->state)
	{
	   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error!! trying to set multiple Key values for same instance, IMC state(%d)",
	         (int)imc_src_dst_info->state);
	   return CAPI_V2_EBADPARAM;
	}

	//clear Key
	memset(imc_src_dst_info->key,0,sizeof(imc_src_dst_info->key));

	imc_src_dst_info->key_length = keylen;
	memscpy(imc_src_dst_info->key,sizeof(imc_src_dst_info->key),key,keylen);

	//Now register for intermodule communication
	if(IMC_SOURCE == src_dst)
	{
	   //Register source handle to database
	   imc_result = imc_src_register(&imc_src_dst_info->source_handle,max_bufq_len);
	   if(CAPI_V2_FAILED(imc_result))
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error!! source failed to register, result(0x%lx)",imc_result);
	      return imc_result;
	   }

	   //Allocates buffers and fills its own queue
	   imc_result = imc_src_allocate_push_self_buf(imc_src_dst_info->source_handle, buf_size, alloc_num_buf);
	   if(CAPI_V2_FAILED(imc_result))
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error!! source failed to push buffers, result(0x%lx)",imc_result);
	      return imc_result;
	   }
	   else
	   {
	      imc_src_dst_info->state =  IMC_STATE_REGISTERED;
	      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_TX: Source registered for IMC");
	   }
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX: Error! Decoder should be IMC source ");
	    imc_result = CAPI_V2_EFAILED;
	    return imc_result;
	}

	//If source is registered, try to open IMC
	if(IMC_STATE_REGISTERED == imc_src_dst_info->state)
	{
		//Query the database with the key value, return destination handle if it is also registered with same key
		result = imc_src_open_comm(imc_src_dst_info->source_handle, &imc_src_dst_info->dest_handle,
			(const int8_t *)imc_src_dst_info->key,imc_src_dst_info->key_length);
		if(CAPI_V2_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_TX: Failed to open communication during initialization, will try later");
			imc_src_dst_info->src_frame_num++;
		}
		else
		{
			imc_src_dst_info->state = IMC_STATE_SOURCE_OPENED_COMMUNICATION;
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_TX: Opened communication for inter-module communication during initialization");
		}
	}
	return imc_result;
  }


/**
  @brief Populate and send buffers from source through IMC

 */
void AudioDecSvc_IMCSendBuf(AudioDecSvc_t* me_ptr, fwk_extn_event_id_compr_over_pcm_bit_rate_level_t *data_ptr)
{
	if(me_ptr->imc_info_ptr==NULL)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: IMC Communication closed, received null IMC pointer");
	   return;
	}
	capi_v2_err_t res = CAPI_V2_EOK;
	capi_v2_err_t resp_result;
	bool_t is_queue_empty;
	uint32_t payload_size;
	dec_to_enc_communication_payload_header_t *payload_header_ptr;
	imc_src_dst_info_t *imc_instance = me_ptr->imc_info_ptr;

	/* Return error if source has not registered successfully */
	if( IMC_STATE_NONE == me_ptr->imc_info_ptr->state)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO," AUD_IMC_TX: Error! Source has not registered yet");
	}

	/* Check if tx has opened communication, if not open it
	 * Keep trying every process cycle, waiting for destination to finish registering */
	if((IMC_STATE_SOURCE_CLOSED_COMMUNICATION == me_ptr->imc_info_ptr->state) ||
	(IMC_STATE_REGISTERED == me_ptr->imc_info_ptr->state))
    {
	   res = imc_src_open_comm( imc_instance->source_handle, &imc_instance->dest_handle,
	   (const int8_t *)imc_instance->key,imc_instance->key_length);

	   if(CAPI_V2_FAILED(res))
	   {
	   	  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_TX: Failed to open Communication, will try later, frame number %d",imc_instance->src_frame_num);
	   	  imc_instance->src_frame_num++;
	   	  return;
	   }
	   else
	   {
		  imc_instance->state = IMC_STATE_SOURCE_OPENED_COMMUNICATION;
		  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AUD_IMC_TX: Opened imc communication");
	   }
    }

	/* Once open send buffers */
	res = imc_src_pop_buf(imc_instance->source_handle,(void**)&payload_header_ptr, &resp_result,&is_queue_empty);
	if(is_queue_empty == FALSE)
	{
       if(CAPI_V2_FAILED(res))
	   {
		  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX:  FAILED to pop buffers from the queue. result(%lx),is_queue_empty(%d)",res,(int)is_queue_empty);
	   }

       if(NULL== payload_header_ptr)
       {
    	   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: Payload header pointer is NULL");
    	   return;
       }
       /*Populating IMC message payload */
       /*Populating decoder to encoder communication payload header */
       payload_header_ptr->opcode = AVS_ENC_SVC_IMC_SET_PARAM;
       payload_header_ptr->size = sizeof(dec_to_enc_communication_set_param_payload_t) + sizeof(dec_to_enc_comm_bit_rate_level_payload_t);
       payload_size = payload_header_ptr->size + sizeof(dec_to_enc_communication_payload_header_t);

       /*Populating set param payload */
       dec_to_enc_communication_set_param_payload_t * payload_ptr = (dec_to_enc_communication_set_param_payload_t *)((int8_t*)payload_header_ptr + sizeof(dec_to_enc_communication_payload_header_t));
       payload_ptr->purpose = AVS_ENCDEC_PURPOSE_ID_BT_INFO;
       payload_ptr->param_id = AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL;
       payload_ptr->param_size = sizeof(dec_to_enc_comm_bit_rate_level_payload_t);

       /*Populating bit rate level info*/
       dec_to_enc_comm_bit_rate_level_payload_t* payload_data_ptr = (dec_to_enc_comm_bit_rate_level_payload_t*)((int8_t*)payload_ptr + sizeof(dec_to_enc_communication_set_param_payload_t));
       payload_data_ptr->level = data_ptr->bit_rate_level;

       /* push the populated buffers to RX*/
	   res = imc_src_push_buf(imc_instance->source_handle,imc_instance->dest_handle,payload_header_ptr,payload_size);
	   if( CAPI_V2_FAILED(res))
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX:  FAILED to push buffers from the queue. result(%lx)",res);
		  res= imc_src_return_buf(imc_instance->source_handle, payload_header_ptr,CAPI_V2_EOK);

		  //IF the destination is not active then Close the connection and try again to open connection in next frame
		  if(CAPI_V2_EUNSUPPORTED == res)
		  {
		     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AUD_IMC_TX:   Rx handle is not active. Closing the connection");
			 imc_src_close_connection(imc_instance->source_handle,imc_instance->dest_handle);
			 imc_instance->state = IMC_STATE_SOURCE_CLOSED_COMMUNICATION;
			 imc_instance->src_frame_num = 0;
		  }
	   }
	   else
	   {
	      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: bit rate level being sent in payload ptr (%lx)",payload_data_ptr->level);
	   }
	}
	else
	{
	   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: Queue is empty when trying to pop buffers to send. result(%lx)",res);
	}
}


/**
  @brief Close IMC connection, De-register source IMC and free memory

 */
void AudioDecSvc_IMCDestroy(AudioDecSvc_t *me_ptr)
{
	capi_v2_err_t res = CAPI_V2_EOK;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_IMC_TX: source IMC destroy");
    if(me_ptr == NULL)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error! Received bad pointer");
       return;
    }

	if(IMC_STATE_SOURCE_OPENED_COMMUNICATION == me_ptr->imc_info_ptr->state)
	{
	   res = imc_src_close_connection(me_ptr->imc_info_ptr->source_handle,me_ptr->imc_info_ptr->dest_handle);
	   if(CAPI_V2_FAILED(res))
	   {
		  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error in closing communication");
		  return;
	   }
	   me_ptr->imc_info_ptr->state = IMC_STATE_SOURCE_CLOSED_COMMUNICATION;
	}

	if(IMC_STATE_NONE != me_ptr->imc_info_ptr->state)
	{
	   res = imc_src_deregister(me_ptr->imc_info_ptr->source_handle);
	   if(CAPI_V2_FAILED(res))
	   {
	      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AUD_IMC_TX: Error in de-registering source");
	   	  return;
	   }
	   me_ptr->imc_info_ptr->state = IMC_STATE_NONE;
	}

	//free source imc memory
	qurt_elite_memory_free(me_ptr->imc_info_ptr);
	me_ptr->imc_info_ptr = NULL;
}
