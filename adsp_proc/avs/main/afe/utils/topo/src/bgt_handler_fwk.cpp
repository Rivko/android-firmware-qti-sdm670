/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/src/bgt_handler_fwk.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

/*==========================================================================
  Include files
  ========================================================================== */
#include "bgt_handler_fwk.h"


//Mask for cmdQ
#define BGT_CMDQ_MASK          (0x1 << 0)
//mask for dataQ
#define BGT_DATAQ_MASK         (0x1 << 1)
//mask for signal
#define BGT_SIG_MASK           (0x1 << 2)
//Mask for RespQ
#define BGT_RESPQ_MASK         (0x1 << 3)
//Mask for BufQ
#define BGT_BUFQ_MASK          (0x1 << 4)

#define BGT_MAX_CMDQ_ELEMENTS  32
#define BGT_MAX_DATAQ_ELEMENTS 32


/*==========================================================================
  Function Declarations
========================================================================== */
/**
 * This function handles incoming  bgt cmd
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_cmd_handler(module_bgt_info_t *bgt_info_ptr);

/**
 * This function handles incoming bgt data
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_data_handler(module_bgt_info_t *bgt_info_ptr);

/**
 * This function is the main workloop of bgt handler
 *
 * @param[in] ptr to the bgt_info struct created by _bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_task_workloop(void *arg_ptr);

/*==========================================================================
  Function definitions
========================================================================== */

ADSPResult bgt_send_destroy_signal(module_bgt_info_t *bgt_info_ptr)
{
   qurt_elite_bufmgr_node_t buf_mgr_node;
   ADSPResult               result;
   elite_msg_any_t          msg;
   int                      actual_size;
   elite_msg_custom_bgt_t   *msg_payload_ptr = NULL;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bgt_send_destroy_signal");

	if(NULL == bgt_info_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
		return ADSP_EFAILED;
	}

	if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_bgt_t), &buf_mgr_node, &actual_size)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to malloc");
		return ADSP_EFAILED;
	}

	msg_payload_ptr = (elite_msg_custom_bgt_t*) buf_mgr_node.pBuffer;
	if(!msg_payload_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL msg payload ptr");
		return ADSP_EFAILED;
	}

   msg_payload_ptr->buffer_returnq_ptr = buf_mgr_node.pReturnQ;
   msg_payload_ptr->responseq_ptr      = NULL;
   msg_payload_ptr->client_token       = 0;
   msg_payload_ptr->sec_op_code        = bgt_info_ptr->svc_bgt_destroy_opcode;
   msg_payload_ptr->payload_ptr        = (void *)bgt_info_ptr;

   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void*)msg_payload_ptr;

	if (ADSP_EOK != (result = qurt_elite_queue_push_back(bgt_info_ptr->svc_cmd_q_ptr, (uint64_t*)&msg)))
	{
		//return the msg buffer...
      elite_msg_release_msg(&msg);
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to send destroy msg !!");
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Destroy msg sent");
	}

	return result;
}


/**
 * This function creates bgt thread and initializes parameters
 *
 * @param[in] address of the pointer to the bgt_info_pool.
 * @param[in] algo type of the created bgt handler
 *
 * @return ADSPResult
 */
ADSPResult bgt_create(module_bgt_info_t **bgt_info_pptr, bgt_create_param_t *bgt_create_param_ptr)
{
   ADSPResult result = ADSP_EOK;
   module_bgt_info_t *bgt_info_ptr = NULL;
   void *queue_buf_ptr = NULL;
   void *bgt_mem_ptr = NULL;

   if(NULL == bgt_info_pptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid BGT_info ptr");
      return ADSP_EFAILED;
   }

   if(NULL == bgt_create_param_ptr->bgt_module_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid BGT module info ptr");
      return ADSP_EFAILED;
   }

   /* Allocate memory for BGT */
   if(NULL
         == (bgt_mem_ptr = qurt_elite_memory_malloc(sizeof(module_bgt_info_t),
               QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating BGT!!");
      return ADSP_ENOMEMORY;
   }

   memset(bgt_mem_ptr, 0, sizeof(module_bgt_info_t));
   *bgt_info_pptr = (module_bgt_info_t *)bgt_mem_ptr;


   bgt_info_ptr = (module_bgt_info_t *)bgt_mem_ptr;

   /* This will be cmd q where finally this thread destroy msg will be sent */
   bgt_info_ptr->svc_cmd_q_ptr          = bgt_create_param_ptr->svc_cmd_q_ptr;
   bgt_info_ptr->svc_bgt_destroy_opcode = bgt_create_param_ptr->svc_bgt_destroy_opcode;

   /* Copy the information retrived from the module */
   memscpy(&bgt_info_ptr->bgt_module_info, sizeof(capi_v2_bgt_module_info_t),
         bgt_create_param_ptr->bgt_module_info_ptr, sizeof(capi_v2_bgt_module_info_t));

   //Create Channel and Command Q
   qurt_elite_channel_init(&bgt_info_ptr->channel);

   /* Creating the cmd Q */
   queue_buf_ptr = qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(BGT_MAX_CMDQ_ELEMENTS),
         QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT cmd Q Creation Failed");
      return ADSP_ENOMEMORY;
   }
   bgt_info_ptr->bgt_svc_handle.cmdQ = (qurt_elite_queue_t *)queue_buf_ptr;

   /* Creating the resp Q */
   queue_buf_ptr = NULL;
   queue_buf_ptr = qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(BGT_MAX_CMDQ_ELEMENTS),
         QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT resp Q Creation Failed");
      return ADSP_ENOMEMORY;
   }
   bgt_info_ptr->resp_q_ptr = (qurt_elite_queue_t *)queue_buf_ptr;

   /* Creating the data Q */
   queue_buf_ptr = NULL;
   queue_buf_ptr = qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(BGT_MAX_DATAQ_ELEMENTS),
         QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT data Q Creation Failed");
      return ADSP_ENOMEMORY;
   }
   bgt_info_ptr->bgt_svc_handle.dataQ = (qurt_elite_queue_t *)queue_buf_ptr;

   /* Creating the buf Q */
   queue_buf_ptr = NULL;
   queue_buf_ptr = qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(BGT_MAX_DATAQ_ELEMENTS),
         QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT buf Q Creation Failed");
      return ADSP_ENOMEMORY;
   }
   bgt_info_ptr->buf_q_ptr = (qurt_elite_queue_t *)queue_buf_ptr;


   qurt_elite_signal_init(&bgt_info_ptr->signal);
   if(ADSP_FAILED(
         result = qurt_elite_channel_add_signal( &(bgt_info_ptr->channel), &bgt_info_ptr->signal, BGT_SIG_MASK)))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to add signal to chanenl");
      return result;
   }

   //After success of adding the Q to channel update global Q ptr.
   //Only input queues to be processed by the task.
   if (ADSP_FAILED(result = qurt_elite_queue_init((char *)"BgtIpCmdQ", BGT_MAX_CMDQ_ELEMENTS, bgt_info_ptr->bgt_svc_handle.cmdQ)) ||
               ADSP_FAILED(result = qurt_elite_channel_addq(&bgt_info_ptr->channel, bgt_info_ptr->bgt_svc_handle.cmdQ, BGT_CMDQ_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT FAILED to init cmd Q or add cmd Q to channel %d!!\n", result);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_queue_init((char *)"BgtResQ", BGT_MAX_CMDQ_ELEMENTS, bgt_info_ptr->resp_q_ptr)) ||
               ADSP_FAILED(result = qurt_elite_channel_addq(&bgt_info_ptr->channel, bgt_info_ptr->resp_q_ptr, BGT_RESPQ_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT FAILED to init resp Q or add resp Q to channel %d!!\n", result);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_queue_init((char *)"BgtIpDataQ", BGT_MAX_DATAQ_ELEMENTS, bgt_info_ptr->bgt_svc_handle.dataQ)) ||
               ADSP_FAILED(result = qurt_elite_channel_addq(&bgt_info_ptr->channel, bgt_info_ptr->bgt_svc_handle.dataQ, BGT_DATAQ_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT FAILED to init data Q or add data Q to channel %d!!\n", result);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_queue_init((char *)"BgtIpBufQ", BGT_MAX_DATAQ_ELEMENTS, bgt_info_ptr->buf_q_ptr)) ||
               ADSP_FAILED(result = qurt_elite_channel_addq(&bgt_info_ptr->channel, bgt_info_ptr->buf_q_ptr, BGT_BUFQ_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT FAILED to init buf Q or add buf Q to channel %d!!\n", result);
      return result;
   }

   //Init thread
   if(ADSP_FAILED(
         result = qurt_elite_thread_launch(&(bgt_info_ptr->threadID),
                                          bgt_create_param_ptr->thread_name,
                                          NULL,
                                          bgt_info_ptr->bgt_module_info.stack_size,
                                          bgt_info_ptr->bgt_module_info.thread_prio,
                                          bgt_task_workloop,
                                          (void *)bgt_info_ptr,
                                          QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to launch BTG task thread thread %d!! \n",result);
      return result;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BGT CREATED successfully!");

   return ADSP_EOK;
}



/**
  This function Is the main workloop for bgt thread
  @param[in] void arg_ptr, which is actually not used. Keep for uniformity

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult bgt_task_workloop(void* arg_ptr)
{
   module_bgt_info_t *bgt_info_ptr = (module_bgt_info_t *)arg_ptr;
	uint32_t channel_status;
	ADSPResult result = ADSP_EOK;
	
	if(NULL == arg_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
		return ADSP_EFAILED;
	}

	for(;;)
	{
		channel_status =   qurt_elite_channel_wait(&bgt_info_ptr->channel, BGT_CMDQ_MASK|BGT_DATAQ_MASK|BGT_SIG_MASK);
		//If something happens on the cmd queue, process cmds.
		if (channel_status & BGT_CMDQ_MASK)
		{
			result = bgt_cmd_handler(bgt_info_ptr);
			//De-init msg will result in ADSP_ETERMINATED status,
			//If so, break out of the infinite loop.
			if(ADSP_ETERMINATED == result) break;
		}

		if (channel_status & BGT_DATAQ_MASK)
		{
			result = bgt_data_handler(bgt_info_ptr);
		}
                
        if (channel_status & BGT_SIG_MASK)
		{			
			result = bgt_data_handler(bgt_info_ptr);
			qurt_elite_signal_clear(&bgt_info_ptr->signal);
		}
  
	}
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BGT exiting workloop");
	bgt_send_destroy_signal(bgt_info_ptr);

	return result;
}


/**
 * This function destroys bgt thread
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_destroy(module_bgt_info_t *bgt_info_ptr)
{
	ADSPResult result = ADSP_EOK;

	if(NULL == bgt_info_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
		return ADSP_EFAILED;
	}
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BGT: Begin Destroy");

	//Join thread
	if(bgt_info_ptr->threadID)
	{
		qurt_elite_thread_join(bgt_info_ptr->threadID, &result);
	}

   qurt_elite_signal_deinit(&bgt_info_ptr->signal);

	//Deinit queues
	if(NULL != bgt_info_ptr->bgt_svc_handle.cmdQ)
	{
	   elite_svc_deinit_cmd_queue(bgt_info_ptr->bgt_svc_handle.cmdQ);
      qurt_elite_memory_free(bgt_info_ptr->bgt_svc_handle.cmdQ);
		bgt_info_ptr->bgt_svc_handle.cmdQ = NULL;
	}

	/* Drain any queued buffers and de-init the data queue */
	if(NULL != bgt_info_ptr->bgt_svc_handle.dataQ)
	{
      elite_svc_deinit_data_queue(bgt_info_ptr->bgt_svc_handle.dataQ);
      qurt_elite_memory_free(bgt_info_ptr->bgt_svc_handle.dataQ);
		bgt_info_ptr->bgt_svc_handle.dataQ = NULL;
	}

	if(NULL != bgt_info_ptr->resp_q_ptr)
	{
      elite_svc_deinit_cmd_queue(bgt_info_ptr->resp_q_ptr);
      qurt_elite_memory_free(bgt_info_ptr->resp_q_ptr);
		bgt_info_ptr->resp_q_ptr = NULL;
	}

	/* The buffer Q if used by FGT should be properly drained of the buffers created by FGT */
   if(NULL != bgt_info_ptr->buf_q_ptr)
   {
      elite_svc_deinit_buf_queue(bgt_info_ptr->buf_q_ptr, bgt_info_ptr->num_buffs_created);
      qurt_elite_memory_free(bgt_info_ptr->buf_q_ptr);
      bgt_info_ptr->buf_q_ptr = NULL;
   }

	if(NULL != bgt_info_ptr->bgt_data_ptr)
	{
		qurt_elite_memory_free(bgt_info_ptr->bgt_data_ptr);
		bgt_info_ptr->bgt_data_ptr = NULL;
	}

   qurt_elite_memory_free(bgt_info_ptr);
   bgt_info_ptr = NULL;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BGT: Destroyed!");
	return ADSP_EOK;
}

/**
 * This function handles incoming bgt data
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_data_handler(module_bgt_info_t *bgt_info_ptr)
{
   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   (*bgt_info_ptr->bgt_module_info.process)(bgt_info_ptr, bgt_info_ptr->bgt_data_ptr);

   return ADSP_EOK;
}


/**
 * This function sends bgt commands
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @param[in] ptr to the payload of the cmd, generally algo cfg structs.
 * @param[in] opCode of the cmd to be sent.
 *
 * @return ADSPResult
 */
ADSPResult bgt_send_cmd(module_bgt_info_t *bgt_info_ptr, uint32_t opCode)
{
   qurt_elite_bufmgr_node_t buf_mgr_node;
   ADSPResult result;
   elite_msg_any_t msg;
   int actual_size;
   elite_msg_custom_bgt_t *msg_payload_ptr = NULL;

   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bgt send cmd, 0x%lx", opCode);
   if(ADSP_FAILED(
         elite_mem_get_buffer(sizeof(elite_msg_custom_bgt_t), &buf_mgr_node, &actual_size)))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to malloc");
      return ADSP_EFAILED;
   }

   msg_payload_ptr = (elite_msg_custom_bgt_t*)buf_mgr_node.pBuffer;
   if(NULL == msg_payload_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL msg payload ptr");
      qurt_elite_queue_push_back(buf_mgr_node.pReturnQ, (uint64_t*)&buf_mgr_node);
      return ADSP_EFAILED;
   }
   msg_payload_ptr->buffer_returnq_ptr = buf_mgr_node.pReturnQ;
   msg_payload_ptr->responseq_ptr      = NULL;
   msg_payload_ptr->client_token       = 0;
   msg_payload_ptr->sec_op_code        = opCode;
   msg_payload_ptr->payload_ptr        = (void *)bgt_info_ptr;

   msg.unOpCode                        = ELITE_CUSTOM_MSG;
   msg.pPayload                        = (void*)msg_payload_ptr;

   if(ADSP_EOK
         != (result = qurt_elite_queue_push_back(bgt_info_ptr->bgt_svc_handle.cmdQ, (uint64_t*)&msg)))
   {
      //return the msg buffer...
      elite_msg_release_msg(&msg);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send input msg with opcode %lx !!\n", opCode);
      return result;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Input Msg with opcode %lx sent \n", opCode);
   }

    return result;
}

 /**
  * This function handles incoming bgt cmd
  *
  * @param[in] ptr to the bgt_info struct created by bgt_create
  * @return ADSPResult
  */
ADSPResult bgt_cmd_handler(module_bgt_info_t *bgt_info_ptr)
{
	ADSPResult        result        = ADSP_EOK;
	capi_v2_err_t     capi_result   = CAPI_V2_EOK;
   elite_msg_any_t   msg;

	if(NULL == bgt_info_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
		return ADSP_EFAILED;
	}

	if (ADSP_EOK != (result = qurt_elite_queue_pop_front(bgt_info_ptr->bgt_svc_handle.cmdQ, (uint64_t*) &msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Error getting msg from queue, exiting");
		return result;
	}

	elite_msg_custom_bgt_t    *msg_payload_ptr = (elite_msg_custom_bgt_t *)msg.pPayload;
	switch(msg_payload_ptr->sec_op_code)
	{
		case BGT_HANDLER_CMD_DEINIT:
		{
		   if(NULL == (bgt_info_ptr->bgt_module_info.deinit))
		   {
		      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL deinit func ptr");
		      result = ADSP_ETERMINATED;
		      break;
		   }

		   if(NULL == bgt_info_ptr->bgt_data_ptr)
		   {
		      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT module not yet initialized, skipping deinit!");
		      result = ADSP_ETERMINATED;
		      break;
		   }
		   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bgt_algo ptr: %lx",(uint32_t)bgt_info_ptr->bgt_data_ptr);

		   capi_result = (*bgt_info_ptr->bgt_module_info.deinit)(&(bgt_info_ptr->bgt_data_ptr));
		   if(CAPI_V2_FAILED(capi_result))
		   {
	         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT de-init failed with result: %d", result);
		   }
		}
      break;

      case BGT_HANDLER_CMD_DESTROY:
      {
         /* Terminating the thread */
         result = ADSP_ETERMINATED;
      }
		break;

		default:
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unexpected Opcode %lx",msg_payload_ptr->sec_op_code);
			result = ADSP_EUNEXPECTED;
		}
	}

	//return msg back to buffer manager
	elite_msg_return_payload_buffer(&msg);

	return result;
}
