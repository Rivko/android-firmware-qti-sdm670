/*========================================================================
   This file contains AFE Background Task Handler implementation

 Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: TBD
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEBgtHandler.h"
#include "AFESwMad.h"
#include "AFEInternal.h"
#include "qurt_elite.h"
#include "AFEPortManagement.h"

/*==========================================================================
  Global Variables
========================================================================== */

afe_bgt_svc_info_t afe_bgt_svc;
/*==========================================================================
  Function Declarations
========================================================================== */
/**
 * This function handles incoming afe bgt cmd
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_cmd_handler(void *bgt_info_handle);

/**
 * This function handles incoming afe bgt data
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_data_handler(void *bgt_info_handle);

/**
 * This function is the main workloop of afe bgt handler
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_task_workloop(void *arg_ptr);

/*==========================================================================
  Function definitions
========================================================================== */
void afe_bgt_pool_init(void)
{
  afe_bgt_svc_info_t *pool_ptr = &afe_bgt_svc;

  memset((void *)pool_ptr, 0, sizeof(pool_ptr->afe_bgt_pool));

  for (int i=0; i<AFE_BGT_POOL_SIZE; i++)
  {
    pool_ptr->afe_bgt_pool[i].is_usable = TRUE;
  }
  pool_ptr->bgt_pool_is_inited = TRUE;

  afe_svc_global_ptr->afe_bgt_pool_ptr = pool_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Afe BGT Service is Initialized");
  return;
}

ADSPResult afe_bgt_alloc_node(void **ptr)
{
  ADSPResult result = ADSP_EFAILED;

  if((NULL == ptr) || (NULL != *ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_bgt_alloc_node: Invalid pointer");
    return ADSP_EFAILED;
  }

  *ptr = NULL;
  for (int i=0; i<AFE_BGT_POOL_SIZE; i++)
  {
    if (afe_svc_global_ptr->afe_bgt_pool_ptr->afe_bgt_pool[i].is_usable == TRUE)
    {
      *ptr = &(afe_svc_global_ptr->afe_bgt_pool_ptr->afe_bgt_pool[i]);
      memset(*ptr,0,sizeof(afe_bgt_info_t));
      result = ADSP_EOK;
      break;
    }
  }

  return result;
}

ADSPResult afe_bgt_send_destroy_signal(void *bgt_info_handle)
{
  qurt_elite_bufmgr_node_t       buf_mgr_node;
  ADSPResult                     result;
  elite_msg_any_t                msg;
  int                            actual_size;
  elite_msg_custom_afe_header_t  *msg_payload_ptr = NULL;

  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_bgt_send_destroy_signal");

  if(NULL == bgt_info_handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }

  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &buf_mgr_node, &actual_size)))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed to malloc");
    return ADSP_EFAILED;
  }

  msg_payload_ptr = (elite_msg_custom_afe_header_t*) buf_mgr_node.pBuffer;
  if(!msg_payload_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL msg payload ptr");
    return ADSP_EFAILED;
  }

  msg_payload_ptr->buffer_returnq_ptr  = buf_mgr_node.pReturnQ;
  msg_payload_ptr->responseq_ptr       = NULL;
  msg_payload_ptr->client_token        = 0;
  msg_payload_ptr->sec_op_code         = ELITEMSG_CUSTOM_AFEBGT_DESTROY;
  msg_payload_ptr->payload_ptr			 = bgt_info_handle;
  msg_payload_ptr->payload_size			 = sizeof(afe_bgt_info_t);

  msg.unOpCode = ELITE_CUSTOM_MSG;
  msg.pPayload = (void*) msg_payload_ptr;

  if (ADSP_EOK != (result = qurt_elite_queue_push_back(qurt_elite_globalstate.pAfeStatSvcCmdQ, (uint64_t*)&msg)))
  {
    //return the msg buffer...
    elite_msg_release_msg(&msg);
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send destroy msg !!");
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Destroy msg sent");
  }

  return result;
}

/**
 * This function creates afe bgt thread and initializes parameters
 *
 * @param[in] address of the pointer to the bgt_info_pool.
 * @param[in] algo type of the created afe bgt handler
 *
 * @return ADSPResult
 */
ADSPResult afe_bgt_create(void **bgt_info_handle, afe_algo_type algo, void *port_ptr)
{
  int32_t thrd_priority;
  uint32_t thrd_stack_size;
  afe_bgt_info_t *bgt_info_ptr = NULL;
  afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)port_ptr;
  ADSPResult result = ADSP_EOK;

  if(NULL == bgt_info_handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid BGT_info handle ptr");
    return ADSP_EFAILED;
  }

  if(NULL != *bgt_info_handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid BGT info handle");
    return ADSP_EFAILED;
  }

  afe_bgt_alloc_node(bgt_info_handle);
  if (NULL == *bgt_info_handle)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in allocating BGT info handle");
    return ADSP_EFAILED;
  }
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " BGT info handle address: %lx", (uint32_t)(*bgt_info_handle));

  bgt_info_ptr = (afe_bgt_info_t *)(*bgt_info_handle);
  bgt_info_ptr->algo_type = algo;

  switch(algo)
  {
    case AFE_SWMAD:
    {
      thrd_priority = elite_get_thrd_prio(ELITETHREAD_STAT_AFE_SW_MAD_PRIO_ID);
      thrd_stack_size = SW_MAD_STACK_SIZE;
      bgt_info_ptr->init_func_ptr = afe_swmad_init_data;
      bgt_info_ptr->deinit_func_ptr = afe_swmad_deinit_data;
      bgt_info_ptr->process_func_ptr = afe_swmad_process;
      if(NULL != afe_port_ptr)
      {
        bgt_info_ptr->port_ptr = (void *)afe_port_ptr;
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port Structure being NULL, BGT init failed");
        afe_bgt_destroy(*bgt_info_handle);
        return ADSP_EFAILED;
      }
    }
    break;

    default:
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT algo type not supported");
      afe_bgt_destroy(*bgt_info_handle);
      return ADSP_EFAILED;
    }
  }

  //Create Channel and Command Q
  qurt_elite_channel_init(&bgt_info_ptr->channel);
  if (ADSP_FAILED(result = (qurt_elite_queue_create((char *)"BgtIpCmdQ", AFE_MAX_CMD_Q_ELEMENTS, &bgt_info_ptr->bgt_svc_handle.cmdQ))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Bgt Input Cmd Queue Creation Failed");
    return result;
  }

  if (ADSP_FAILED(result = (qurt_elite_queue_create((char *)"BgtIpDataQ", AFE_MAX_CMD_Q_ELEMENTS, &bgt_info_ptr->bgt_svc_handle.dataQ))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT Input Data Queue Creation Failed");
    return result;
  }


  if (ADSP_FAILED(result = (qurt_elite_queue_create((char *)"BgtResQ", AFE_MAX_CMD_Q_ELEMENTS, &bgt_info_ptr->resp_q_ptr))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "BGT Resp Queue Creation Failed");
    return result;
  }

  if (NULL != bgt_info_ptr->bgt_signal_ptr)
  {
    if (ADSP_FAILED(result = qurt_elite_channel_add_signal( &(bgt_info_ptr->channel), bgt_info_ptr->bgt_signal_ptr, AFE_BGT_SIG_MASK)))
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to add signal to chanenl");
      return result;
    }
  }
  //After success of adding the Q to channel update glaobal Q ptr.
  //Only input queues to be processed by the task.
  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(bgt_info_ptr->channel), bgt_info_ptr->bgt_svc_handle.cmdQ,AFE_BGT_CMDQ_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to add cmdQ to channel = %d!!\n", result);
    return result;
  }

  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(bgt_info_ptr->channel), bgt_info_ptr->bgt_svc_handle.dataQ,AFE_BGT_DATAQ_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to add dataQ to channel = %d!!\n", result);
    return result;
  }

  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(bgt_info_ptr->channel), bgt_info_ptr->resp_q_ptr,AFE_BGT_RESPQ_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to add dataQ to channel = %d!!\n", result);
    return result;
  }

  qurt_elite_mutex_init(&bgt_info_ptr->bgt_mutex);

  // update the bgt service handle with queue info

  //Init thread
  if (ADSP_FAILED(result = qurt_elite_thread_launch(&(bgt_info_ptr->threadID), (char*)"BGT_TASK", NULL,
                                                    thrd_stack_size, thrd_priority, afe_bgt_task_workloop, (void *)bgt_info_ptr, QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to launch BTG task thread thread %d!! \n",result);
    return result;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE BGT CREATED, Algo type =  %d",bgt_info_ptr->algo_type);

  return ADSP_EOK;
}

/**
  This function Is the main workloop for afe bgt thread
  @param[in] void arg_ptr, which is actually not used. Keep for uniformity

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_bgt_task_workloop(void* arg_ptr)
{
  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t *)arg_ptr;
  uint32_t channel_status;
  ADSPResult result=ADSP_EOK;
  afe_dev_port_t *port_ptr = (afe_dev_port_t *)bgt_info_ptr->port_ptr;

  if(NULL == arg_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }

  qurt_elite_mutex_lock(&bgt_info_ptr->bgt_mutex);

  for(;;)
  {
    channel_status =   qurt_elite_channel_wait(&bgt_info_ptr->channel, AFE_BGT_CMDQ_MASK|AFE_BGT_DATAQ_MASK|AFE_BGT_SIG_MASK);
    //If something happens on the cmd queue, process cmds.
    if (channel_status & AFE_BGT_CMDQ_MASK)
    {
      result = afe_bgt_cmd_handler(bgt_info_ptr);
      //De-init msg will result in ADSP_ETERMINATED status,
      //If so, break out of the infinite loop.
      if(ADSP_ETERMINATED == result) break;
    }

    if (channel_status & AFE_BGT_DATAQ_MASK)
    {
      result = afe_bgt_data_handler(bgt_info_ptr);
    }

    if (channel_status & AFE_BGT_SIG_MASK)
    {
      result = afe_bgt_data_handler(bgt_info_ptr);
      qurt_elite_signal_clear(&port_ptr->dma_signal);
    }

  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE BGT exiting workloop");
  afe_bgt_send_destroy_signal(arg_ptr);

  return result;
}

/**
 * This function destroys afe bgt thread
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_destroy(void *bgt_info_handle)
{
  ADSPResult result = ADSP_EOK;
  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t *)bgt_info_handle;

  if(NULL == bgt_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEBGT: de-init");

  //Join thread
  if(bgt_info_ptr->threadID)
  {
    qurt_elite_thread_join(bgt_info_ptr->threadID, &result);
  }


  qurt_elite_mutex_destroy(&bgt_info_ptr->bgt_mutex);

  //Deinit queues
  if(NULL != bgt_info_ptr->bgt_svc_handle.cmdQ)
  {
    qurt_elite_queue_destroy(bgt_info_ptr->bgt_svc_handle.cmdQ);
    bgt_info_ptr->bgt_svc_handle.cmdQ = NULL;
  }

  if(NULL != bgt_info_ptr->bgt_svc_handle.dataQ)
  {
    qurt_elite_queue_destroy(bgt_info_ptr->bgt_svc_handle.dataQ);
    bgt_info_ptr->bgt_svc_handle.dataQ = NULL;
  }

  if(NULL != bgt_info_ptr->resp_q_ptr)
  {
    qurt_elite_queue_destroy(bgt_info_ptr->resp_q_ptr);
    bgt_info_ptr->resp_q_ptr = NULL;
  }


  if(NULL != bgt_info_ptr->bgt_algo_h)
  {
    qurt_elite_memory_free(bgt_info_ptr->bgt_algo_h);
    bgt_info_ptr->bgt_algo_h = NULL;
  }

  memset(bgt_info_ptr, 0, sizeof(afe_bgt_info_t));

  // Mark useable
  bgt_info_ptr->is_usable = TRUE;
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEBGT: de-inited");
  return ADSP_EOK;
}

/**
 * This function handles incoming afe bgt data
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_data_handler(void *bgt_info_handle)
{
  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t*)bgt_info_handle;

  if(NULL == bgt_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }
  return ((*bgt_info_ptr->process_func_ptr)(bgt_info_ptr));
}

/**
 * This function sends afe bgt commands
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @param[in] ptr to the payload of the cmd, generally algo cfg structs.
 * @param[in] opCode of the cmd to be sent.
 *
 * @return ADSPResult
 */
ADSPResult afe_bgt_send_cmd(void *bgt_info_handle, void* afe_algo_ptr, uint32_t opCode)
{
  qurt_elite_bufmgr_node_t       buf_mgr_node;
  ADSPResult                     result;
  elite_msg_any_t                msg;
  int                            actual_size;
  elite_msg_custom_afe_header_t  *msg_payload_ptr = NULL;

  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t*)bgt_info_handle;
  if(NULL == bgt_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bgt send cmd, 0x%lx", opCode);
  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &buf_mgr_node, &actual_size)))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to malloc");
    return ADSP_EFAILED;
  }

  msg_payload_ptr = (elite_msg_custom_afe_header_t*) buf_mgr_node.pBuffer;
  if(NULL == msg_payload_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL msg payload ptr");
    qurt_elite_queue_push_back(buf_mgr_node.pReturnQ, (uint64_t*)&buf_mgr_node);
    return ADSP_EFAILED;
  }
  msg_payload_ptr->buffer_returnq_ptr  = buf_mgr_node.pReturnQ;
  msg_payload_ptr->responseq_ptr       = NULL;
  msg_payload_ptr->client_token        = 0;
  msg_payload_ptr->sec_op_code         = opCode;
  msg_payload_ptr->payload_ptr		 = afe_algo_ptr;

  msg.unOpCode = ELITE_CUSTOM_MSG;
  msg.pPayload = (void*) msg_payload_ptr;


  bgt_info_ptr->cmd_result = ADSP_EOK;

  if (ADSP_EOK != (result = qurt_elite_queue_push_back(bgt_info_ptr->bgt_svc_handle.cmdQ, (uint64_t*)&msg)))
  {
    //return the msg buffer...
    elite_msg_release_msg(&msg);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send input msg with opcode %lx !!\n",opCode);
    return result;
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Input Msg with opcode %lx sent \n",opCode);
  }

  qurt_elite_mutex_lock(&bgt_info_ptr->bgt_mutex);

  /* Capturing the result of init or deinit in bg thread processing */
  result = bgt_info_ptr->cmd_result;

  qurt_elite_mutex_unlock(&bgt_info_ptr->bgt_mutex);

  return result;
}

/**
 * This function handles incoming afe bgt cmd
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @return ADSPResult
 */
ADSPResult afe_bgt_cmd_handler(void *bgt_info_handle)
{
  ADSPResult                     result = ADSP_EOK;
  elite_msg_any_t                msg;
  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t*)bgt_info_handle;

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

  elite_msg_custom_afe_header_t    *msg_payload_ptr = (elite_msg_custom_afe_header_t *)msg.pPayload;
  switch(msg_payload_ptr->sec_op_code)
  {
    case ELITEMSG_CUSTOM_AFEBGT_INIT:
    {
      if(NULL == bgt_info_ptr->init_func_ptr)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL init func ptr");
        result = ADSP_ETERMINATED;
        break;
      }

      if(ADSP_EOK
          != (result = (*bgt_info_ptr->init_func_ptr)(msg_payload_ptr->payload_ptr,
              &(bgt_info_ptr->bgt_algo_h))))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "bg init function %lx failed ",(uint32_t)bgt_info_ptr->init_func_ptr);
      }
      bgt_info_ptr->cmd_result = result;
      qurt_elite_mutex_unlock(&bgt_info_ptr->bgt_mutex);


    }
    break;
    case ELITEMSG_CUSTOM_AFEBGT_DEINIT:
    {
      if(NULL == (bgt_info_ptr->deinit_func_ptr))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL deinit func ptr");
        result = ADSP_ETERMINATED;
        break;
      }
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bgt_algo ptr: %lx",(uint32_t)bgt_info_ptr->bgt_algo_h);

      result = (*bgt_info_ptr->deinit_func_ptr)(&(bgt_info_ptr->bgt_algo_h));
      bgt_info_ptr->cmd_result = result;

      if (ADSP_EOK == result)
      {
        result = ADSP_ETERMINATED;
      }
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
/**
 * This function sends afe bgt data to bgt clients
 *
 * @param[in] pDevPort: ptr to the port structure where clients info is stored.
 * @param[in] pClientInfo: ptr to the information of the recipient
 * @param[in] out_buf_ptr: ptr to the output buffer.
 * @param[in] num_data_bytes: Number of bytes of the outgoing data.
 * @return ADSPResult
 */
ADSPResult afe_bgt_send_data_to_client(void *pDevPort, void *pClientInfo, int8_t* out_buf_ptr, uint16_t num_data_bytes)
{
  uint32_t min_bytes;
  int8_t *ptr_dst_client_buf;
  qurt_elite_bufmgr_node_t outBufMgrNode;
  elite_msg_any_t*        pPeerDataQMsg;
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *port_ptr = (afe_dev_port_t *)pDevPort;
  afe_client_info_t *client_info_ptr = (afe_client_info *)pClientInfo;

  while (1)
  {
    if (client_info_ptr->remaining_samples == 0)
    {
      uint32_t                   unChannelStatus;
      qurt_elite_bufmgr_node_t   outBufMgrNode;
      elite_msg_data_buffer_t    *psPcmBuf;

      unChannelStatus = qurt_elite_channel_poll(qurt_elite_queue_get_channel(client_info_ptr->bufQ), qurt_elite_queue_get_channel_bit(client_info_ptr->bufQ));
      if (unChannelStatus)
      {
        if (ADSP_EOK == (result = qurt_elite_queue_pop_front(client_info_ptr->bufQ, (uint64_t*)&outBufMgrNode)))
        {
          psPcmBuf = (elite_msg_data_buffer_t *)outBufMgrNode.pBuffer;
          client_info_ptr->remaining_samples = (client_info_ptr->actual_buf_size);
          client_info_ptr->ps_curr_pcm_buff = psPcmBuf;
          client_info_ptr->ps_curr_pcm_buff->nActualSize = client_info_ptr->actual_buf_size;
          client_info_ptr->curr_buff_ptr = (int8_t *) &(psPcmBuf->nDataBuf);
          client_info_ptr->ps_curr_pcm_buff->nFlag = 1<<31;
          client_info_ptr->ps_curr_pcm_buff->ullTimeStamp = port_ptr->port_av_timestamp;
        }
      }
      else
      {
        uint64_t currTime, diff;
        client_info_ptr->under_run_counter++;

        // Use the wallk clk time just to avoid the flood of under-run messages
        currTime = qurt_elite_timer_get_time();
        diff = currTime - client_info_ptr->prev_time;
        if (diff >= 10000)
        {
          MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort intf=%x, dir=%d: Audio buffer not available in Tx path for client 0x%lX. CurrTime-PrevTime = %ld us. Num Underrun = %lu!!\n",\
                port_ptr->intf,port_ptr->dir, client_info_ptr->client_id,  (uint32_t)diff,client_info_ptr->under_run_counter);
          client_info_ptr->prev_time = currTime;
          client_info_ptr->under_run_counter = 0;
        }
        return(ADSP_EOK);
      }
    } //if(client_info_ptr->remaining_samples == NULL)

    // take the minimum bytes needed for or available for filling the buffer
    min_bytes = num_data_bytes;
    if (num_data_bytes > client_info_ptr->remaining_samples)
    {
      min_bytes = client_info_ptr->remaining_samples;
    }

    ptr_dst_client_buf = client_info_ptr->curr_buff_ptr;

    memscpy(ptr_dst_client_buf,					/* Destination buf pointer*/
            client_info_ptr->remaining_samples,	/* Dest buf available length in bytes */
            out_buf_ptr, 						/* Source buf ptr */
            min_bytes);							/* Number of bytes to copy */

    out_buf_ptr += min_bytes;
    client_info_ptr->curr_buff_ptr += min_bytes;
    client_info_ptr->remaining_samples -= min_bytes;
    num_data_bytes -= min_bytes;

    if (client_info_ptr->remaining_samples == 0)
    {
      /* Send output buffers to downstream service. */
      outBufMgrNode.pReturnQ = client_info_ptr->bufQ;
      outBufMgrNode.pBuffer = client_info_ptr->ps_curr_pcm_buff;
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: TX sending buf: %X", (uint32_t) client_info_ptr->ps_curr_pcm_buff);
      pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
          &outBufMgrNode,
          ELITE_DATA_BUFFER,
          NULL,                   /* do not need response */
          0,                      /* token */
          0);                     /* do not care response result*/
      result = qurt_elite_queue_push_back(client_info_ptr->afe_client.afe_client_handle->dataQ, (uint64_t*)pPeerDataQMsg);
#ifdef DBG_MSG_ENABLE		 
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SW MAD SENDING DATA CLIENTS");
#endif		 

      if(ADSP_FAILED(result))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
        (void) elite_msg_push_payload_to_returnq(client_info_ptr->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
      }
    }

    if(num_data_bytes == 0)
    {
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

/**
 * This function sends afe bgt data to bgt handlers
 *
 * @param[in] ptr to the bgt_info struct created by afe_bgt_create
 * @param[in] ptr to the payload of the data msg, generally data to be sent.
 * @param[in] size of the data payload to be sent.
 * @return ADSPResult
 */
ADSPResult afe_bgt_send_data(void *bgt_info_handle, void *data_ptr, uint32_t size)
{
  qurt_elite_bufmgr_node_t buf_mgr_node;
  ADSPResult result;
  elite_msg_any_t msg;
  int actual_size;
  elite_msg_custom_afe_header_t *msg_payload_ptr = NULL;

  afe_bgt_info_t *bgt_info_ptr = (afe_bgt_info_t*)bgt_info_handle;
  if(NULL == bgt_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
    return ADSP_EFAILED;
  }

  if(ADSP_FAILED(
      elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &buf_mgr_node, &actual_size)))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed to malloc");
    return ADSP_EFAILED;
  }

  msg_payload_ptr = (elite_msg_custom_afe_header_t*)buf_mgr_node.pBuffer;
  if(!msg_payload_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL msg payload ptr");
    return ADSP_EFAILED;
  }

  msg_payload_ptr->buffer_returnq_ptr = buf_mgr_node.pReturnQ;
  msg_payload_ptr->responseq_ptr = NULL;
  msg_payload_ptr->client_token = 0;
  msg_payload_ptr->sec_op_code = ELITE_DATA_BUFFER;
  msg_payload_ptr->payload_ptr = data_ptr;
  msg_payload_ptr->payload_size = size;

  msg.unOpCode = ELITE_CUSTOM_MSG;
  msg.pPayload = (void*)msg_payload_ptr;

  if(ADSP_EOK
      != (result = qurt_elite_queue_push_back(bgt_info_ptr->bgt_svc_handle.dataQ, (uint64_t*)&msg)))
  {
    //return the msg buffer...
    elite_msg_release_msg(&msg);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send input msg with size %lx !!\n",size);
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Input msg with size %lx sent \n",size);
  }
  return result;
}

