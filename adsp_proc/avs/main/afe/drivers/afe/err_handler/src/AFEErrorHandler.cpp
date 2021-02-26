/*========================================================================
   This file contains AFE Error handler implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: 
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "AFEErrorHandler_i.h"
#include "AFEErrorHandler.h"
#include "AFEInternal.h"
#include "AFEDeviceDriver.h"
#ifndef MDSPMODE
#include "adsp_err_fatal.h"
#else
#if defined(__qdsp6__) && !defined(SIM)
#include "err.h"
#endif
#endif
static afe_device_recovery_params_t dev_rec_params_global ;

afe_device_recovery_params_t *dev_rec_params_global_ptr;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

void afe_debug_crash()
{
  /* Crashing only for internal modem builds*/
  /* Adding this because we dont have correpsonding API for ADSPERRFATAl in modem builds*/
   #if defined(AFE_ERR_HNDLR_INTERNAL) && defined(MDSPMODE)
        
	   #if defined(__qdsp6__) && !defined(SIM)   
		ERR_FATAL("FATAL_ERR: Force crash Q6 due to AFE signal miss", 0, 0, 0);
	   #endif
	   
   #endif  //AFE_ERR_HNDLR_INTERNAL

  /**< Place holder to force crash the device,
   * if customer asked to allow few recoveries and then crash.*/

  return;
}

/**
 * Performs afe error handler driver initialization.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_init()
{
  ADSPResult result = ADSP_EOK;

  dev_rec_params_global_ptr = &dev_rec_params_global;

  dev_rec_params_global_ptr->max_num_allowed_recoveries   = MAX_NUM_ALLOWED_RECOVERIES;
  dev_rec_params_global_ptr->window_duration              = ERR_HNDLR_REC_WINDOW_DURATION;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " AFE ERR CONFIG during init: max_num_allowed_recoveries: %d, window_duration: %ld us",\
        dev_rec_params_global_ptr->max_num_allowed_recoveries, dev_rec_params_global_ptr->window_duration);

  if (ADSP_EOK != (result = afe_err_handler_debug_init()))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error handler debug Initialization failed");
  }

  return result;
}

/**
 * Performs afe error handler driver de-initialization.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_deinit()
{
  ADSPResult result = ADSP_EOK;

  /* Do nothing
   *  kept this function for symmetry*/

  return result;
}

/**
 * This function pushes error recovery message (by dynamic thread
 * in error, informs) to AFE service cmdQ (AFE service thread).
 * Thereby AFEservice thread will take care of invoking
 * error recovery.
 *
 * @param[in] port_id, afe port id
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_hndlr_send_recovery_msg(uint32_t port_id)
{
  ADSPResult                       result = ADSP_EOK;
  qurt_elite_bufmgr_node_t         bufMgrNode;
  int                              nActualSize;
  elite_msg_any_t                  msg;
  elite_msg_custom_afe_header_t    *msg_payload;

  /* Get buf from the buf mgr */
  if ( ADSP_EOK != (result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &bufMgrNode, &nActualSize)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get buf for sending device recovery msg, port_id: 0x%lx, result: %lu",
          port_id, result);

    return result;
  }

  /* Compose the payload */
  msg_payload = (elite_msg_custom_afe_header_t*) bufMgrNode.pBuffer;

  msg_payload->afe_id                         =  port_id;
  msg_payload->responseq_ptr                  =  NULL;
  msg_payload->client_token                   =  0;
  msg_payload->buffer_returnq_ptr             =  bufMgrNode.pReturnQ;
  msg_payload->sec_op_code                    =  ELITEMSG_CUSTOM_DEV_ERR_RECOVERY;

  msg.unOpCode        =  ELITE_CUSTOM_MSG;
  msg.pPayload        =  (void*) msg_payload;

  /* Push to the AFE service command queue */
  result = qurt_elite_queue_push_back(afe_svc_global_ptr->static_svc_cmdq_ptr, (uint64_t*)&msg);

  if (ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to push buf to AfeSvc cmd queue, result: %lu, port_id: 0x%lx",
          result, port_id);

    /* Return the msg buffer */
    elite_msg_release_msg(&msg);

    return result;
  }

  return ADSP_EOK;
}

/**
 * This function performs device error recovery
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_port_dev_intf_reset(afe_dev_port_t *pDevPort)
{
  ADSPResult  result = ADSP_EFAILED;

  if (NULL == pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "INTF RESET FAILED, AFE port NULL pointer access");
    return result;
  }

  /* Device recovery is done only when port is in SUSPEND state */
  if ((AFE_PORT_STATE_SUSPEND != pDevPort->port_state))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "INTF RESET FAILED, AFE port not in SUSPEND state, port_id: 0x%lx, port_state :%d ",\
          pDevPort->intf, pDevPort->port_state );
    return result;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port reset started, port_id: 0x%lx", pDevPort->intf);

  afe_port_mutex_lock(pDevPort);
  pDevPort->dev_proc_seq = AFE_DEV_RECOVERY;

  /* Stop the device interface */
  if(ADSP_EOK != (result = afe_port_modules_deinit_and_dev_stop(pDevPort)))
  {
    afe_port_mutex_unlock(pDevPort);
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "INTF RESET FAILED, failed to stop the device, port 0x%X", pDevPort->intf);
    goto _dev_reset_bail_out;
  }
  afe_port_mutex_unlock(pDevPort);

  if(ADSP_EOK != (result = afe_restore_dev_config_param(pDevPort)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "INTF RESET FAILED, failed to restore device config params, port 0x%X", pDevPort->intf);
    goto _dev_reset_bail_out;
  }

  /* Start the device interface */
  /*No need to acquire mutex to perform modules init and dev start, since now port is in STOP state. */
  /* ADSP_ECONTINUE will be for h/w mad or s/w duty enabled case */
  if(ADSP_ECONTINUE == (result = afe_port_modules_init_and_dev_start(pDevPort)))
  {
    result = ADSP_EOK;
  }
  else if(ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "INTF RESET FAILED, failed to start the device, port 0x%X", pDevPort->intf);

    /* Set the port state to CONFIG. This is needed to execute stop cmd sequence completely.
     *
     * Assume, afe_port_modules_deinit_and_dev_stop() succeeds (so port state moves to STOP),
     * and also assume afe_port_modules_init_and_dev_start() fails (this means port state
     *  still in STOP state). If we try to execute afe_port_apr_dma_stop(), now stop cmd will
     *  think that port is already stopped (due to port state check). Therefore, resources
     * would not be deallocated and modules not be de-initialized */
    pDevPort->dev_proc_seq = AFE_NORMAL_SEQ;

    pDevPort->port_state = AFE_PORT_STATE_CONFIG;

    /* execute stop cmd sequence */
    afe_port_apr_dma_stop(pDevPort, NULL);

    goto _dev_reset_bail_out;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port reset succesfully completed, port_id: 0x%lx", pDevPort->intf);

  _dev_reset_bail_out:
  afe_port_mutex_lock(pDevPort);
  pDevPort->dev_proc_seq = AFE_NORMAL_SEQ;
  afe_port_mutex_unlock(pDevPort);
  return result;
}

/**
 * AFE service thread calls (through cmdQ) this function
 *  to invoke device error recovery
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg from port cmdQ
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_handler_dev_recovery_msg(afe_dev_port_t *pDevPort, elite_msg_any_t *pMsg)
{
  ADSPResult        result = ADSP_EOK;
  uint32_t          port_idx;
  afe_dev_port_t    *p_dev_port_l = NULL;

#ifdef DBG_DEV_RECOVERY
  uint64_t          start_time;
  uint32_t          delta_time = 0;
  avtimer_drv_handle_t h_avtimer_drv = pDevPort->avt_drv;

  /* Check if this port is member of group device */
  if(NULL != pDevPort->p_group_device_state)
  {
    h_avtimer_drv = pDevPort->p_group_device_state->avt_drv;
  }

  if(NULL != h_avtimer_drv)
  {
    start_time = afe_time_srvr_timer_get_time(h_avtimer_drv);
  }
#endif

  /* Check if this port is member of group device */
  if (NULL != pDevPort->p_group_device_state)
  {
    for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
    {
      p_dev_port_l = pDevPort->p_group_device_state->p_dev_port[port_idx];

      /* Perform device recovery, only when member port is in Suspend state*/
      if( (NULL != p_dev_port_l) && (AFE_PORT_STATE_SUSPEND == p_dev_port_l->port_state) )
      {
        /* Initiate device reset */
        result |= afe_port_dev_intf_reset(p_dev_port_l);
      }
    }
  }
  else /* Standalone port */
  {
    result = afe_port_dev_intf_reset(pDevPort);
  }

#ifdef DBG_DEV_RECOVERY
  if(NULL != h_avtimer_drv)
  {
    delta_time = (afe_time_srvr_timer_get_time(h_avtimer_drv) - start_time);
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port reset delta_time: %lu us, port_id: 0x%lx", delta_time, pDevPort->intf);
#endif

  if(NULL != signal_miss_pool_global_ptr)
  {
    afe_err_handler_incr_recovery_index_counter(pDevPort->intf);
  }

  /* Return the msg buffer */
  elite_msg_release_msg(pMsg);

  return result;
}

void* afe_dev_get_err_handler_drv_handle(uint32_t port_id, uint64_t *latest_ts_ptr)
{
  ADSPResult        result = ADSP_EOK;
  afe_dev_port_t    *p_dev_port = NULL;
  afe_err_handler_drv_handle_t h_err_handler_drv = NULL;
  avtimer_drv_handle_t h_avtimer_drv = NULL;

  /* Fetch the pointer to the port structure */
  if (ADSP_EOK != (result = afe_svc_get_port(port_id, &p_dev_port)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to get pointer to port obj, port_id: 0x%lx", port_id);
    return NULL;
  }

  /* Check if this port is member of group device */
  if (NULL != p_dev_port->p_group_device_state)
  {
    h_err_handler_drv = p_dev_port->p_group_device_state->err_handler_drv;
    h_avtimer_drv     = p_dev_port->p_group_device_state->avt_drv;
  }
  else
  {
    h_err_handler_drv = p_dev_port->err_handler_drv;
    h_avtimer_drv     = p_dev_port->avt_drv;
  }

  if(NULL != h_avtimer_drv)
  {
    /* Update the upcoming time stamp into afe error handler driver*/
    *latest_ts_ptr = (uint64_t)afe_time_srvr_timer_get_time((avtimer_drv_handle_t)h_avtimer_drv);
  }

  return (h_err_handler_drv);
}

/**
 * This function de-registers afe port from afe error handler
 *
 * @param[in] err_handler_h, pointer to error handler handle
 * @return ADSP_EOK, error code
 */
ADSPResult afe_err_handler_driver_deregister(afe_err_handler_drv_handle_t *err_handler_h)
{
  ADSPResult            result = ADSP_EOK;
  if(NULL == err_handler_h)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to error_handler handle is NULL");
    return ADSP_EFAILED;
  }

  if (NULL != *err_handler_h)
  {
    afe_err_handler_info_t* err_handler_info_ptr = (afe_err_handler_info_t *)(*err_handler_h);
    if(NULL != err_handler_info_ptr->ts_queue_ptr)
    {
      qurt_elite_memory_free(err_handler_info_ptr->ts_queue_ptr);
      err_handler_info_ptr->ts_queue_ptr = NULL;
    }

    qurt_elite_memory_free((void *)*err_handler_h);
    *err_handler_h = NULL;
  }

  return result;
}

/**
 * This function registers afe port to have afe error handler
 *
 * @param[in] err_handler_h, pointer to error handler handle
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_handler_driver_register(afe_err_handler_drv_handle_t *err_handler_h)
{
  ADSPResult            result = ADSP_EOK;

  if(NULL == err_handler_h)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to error_handler handle is NULL");
    return ADSP_EFAILED;
  }

  afe_err_handler_info_t* err_handler_info_ptr = (afe_err_handler_info_t *)qurt_elite_memory_malloc(sizeof(afe_err_handler_info_t), QURT_ELITE_HEAP_DEFAULT);
  if(NULL == err_handler_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate afe error handler info object");

    *err_handler_h = NULL;
    result         = ADSP_ENOMEMORY;
  }
  else
  {
    uint32_t ts_queue_length = dev_rec_params_global_ptr->max_num_allowed_recoveries;

    /**< Time stamp queue length is derived based on max_num_allowed_recoveries
     * If max_num_allowed_recoveries is zero, then making queue length as 1, to
     * avoid memory alloc failure.*/
    if(0 == ts_queue_length)
    {
      ts_queue_length = 1;
    }

    uint64_t *ts_queue_ptr = (uint64_t *)qurt_elite_memory_malloc(ts_queue_length*sizeof(uint64_t), QURT_ELITE_HEAP_DEFAULT);

    if( (NULL == ts_queue_ptr) )
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate afe error handler ts_queue");

      qurt_elite_memory_free((void *)*err_handler_h);
      *err_handler_h = NULL;
      result         = ADSP_ENOMEMORY;
    }
    else
    {
      memset(err_handler_info_ptr, 0, sizeof(afe_err_handler_info_t));
      memset(ts_queue_ptr, 0, ts_queue_length*sizeof(uint64_t));
      err_handler_info_ptr->ts_queue_ptr = ts_queue_ptr;
      *err_handler_h = err_handler_info_ptr;
    }
  }
  return result;
}

/**
 * This function decides and returns the type of afe error handler
 *
 * @param[in] port_id, afe port id, used to get port's instance
 * @returns afe error handler type
 */
afe_err_hndlr_type_t afe_get_err_handler_type(uint32_t port_id)
{
  uint64_t latest_ts=0, ts_diff=0;

  uint64_t window_duration            = dev_rec_params_global_ptr->window_duration;
  uint32_t max_num_allowed_recoveries = dev_rec_params_global_ptr->max_num_allowed_recoveries;


  /* fetches error handler driver handle and also gets the latest time stamp using port's avtimer handle*/
  afe_err_handler_info_t* err_handler =  (afe_err_handler_info_t *)afe_dev_get_err_handler_drv_handle(port_id, &latest_ts);

  if(NULL == err_handler)
  {
    MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "error handler obj is NULL/not available");
    return AFE_ERR_HNDL_NONE;
  }

  uint64_t *curr_tsq_ptr = err_handler->ts_queue_ptr;

  if(NULL == curr_tsq_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "error handler time stamp queue is NULL");
    return AFE_ERR_HNDL_NONE;
  }

  /**< If error recovery count is zero, then the time stamp of incoming signal miss
   *  will be the first entry into the queue and hence ts_diff should need to be zero
   *  (by default ts_diff is zero, so no need to calculate).*/
  if(err_handler->err_recovery_count > 0)
  {
    curr_tsq_ptr = err_handler->ts_queue_ptr + err_handler->front_idx;
    ts_diff = latest_ts - *curr_tsq_ptr;
  }

  /*Checks whether incoming signal miss occurred in window.
   * If yes, then depends on error recovery count, allows device recovery or crash.
   * Otherwise, moves the window interval by popping the old time-stamps, till the
   * difference in latest and front index time-stamps is within the window,
   * and after updating queue, allows afe device recovery  */

  /**< Checks whether current signal miss happened inside window interval or not.*/
  if(ts_diff <= window_duration)
  {
    /**< Now, current signal miss happened inside the window.*/

    /**< Check for queue is full or not.*/
    if(max_num_allowed_recoveries == err_handler->err_recovery_count)
    {
#ifdef DBG_DEV_RECOVERY
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CRASH SCENARIO ts_Q : port_id: 0x%x, latest_ts: %ld%ld, ts_diff :%ld%ld ",\
            port_id, (uint32_t)(latest_ts>>32),(uint32_t)latest_ts, (uint32_t)(ts_diff>>32),(uint32_t)ts_diff);

      for(uint8_t index=0; index< max_num_allowed_recoveries; index++)
      {
        curr_tsq_ptr = err_handler->ts_queue_ptr + index;
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ts_Q : Q[%ld]: %ld%ld ",\
              index, (uint32_t)(*curr_tsq_ptr>>32),(uint32_t)*curr_tsq_ptr);
      }
#endif //DBG_DEV_RECOVERY

      /**< error recovery count indicates, already maximum number of recoveries taken place.
       * And this signal miss also occurred inside the window, hence crash.*/
      return AFE_DEBUG_CRASH;
    }
  }
  else
  {
    /**<Current signal miss happened outside the window, so, need to move the window
     * by popping the old time stamps.*/

    /*Pop old time-stamp and check now signal miss is inside the current moved window
     * If not, pop one more timestamp and check again*/
    do
    {
      /**<Checks whether queue is empty.*/
      if(err_handler->err_recovery_count > 0)
      {
        /**< reset the value of queue at front index to zero.*/
        curr_tsq_ptr = err_handler->ts_queue_ptr + err_handler->front_idx;
        *curr_tsq_ptr = 0;

        /**< Now front index should point to second oldest time stamp
         * in the current window and thereby window will be moved.
         * Increment the index modularly, since it is circular queue.*/
        err_handler->front_idx++;
        if(max_num_allowed_recoveries == err_handler->front_idx)
        {
          err_handler->front_idx = 0;
        }
        /**< Update with current error recovery count.*/
        err_handler->err_recovery_count--;
#ifdef DBG_DEV_RECOVERY
        MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "POP ts_Q : port_id: 0x%x, latest_ts: %ld%ld, ts_diff :%ld%ld ",\
              port_id, (uint32_t)(latest_ts>>32),(uint32_t)latest_ts, (uint32_t)(ts_diff>>32),(uint32_t)ts_diff);
        for(uint8_t index=0; index< max_num_allowed_recoveries; index++)
        {
          curr_tsq_ptr = err_handler->ts_queue_ptr + index;
          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ts_Q : Q[%ld]: %ld%ld ",\
                index, (uint32_t)(*curr_tsq_ptr>>32),(uint32_t)*curr_tsq_ptr);
        }
#endif //DBG_DEV_RECOVERY

      }
      else if(0 == err_handler->err_recovery_count)
      {
        /**< queue becomes empty, nothing left to pop.*/
        break;
      }

      /**<Calculate time stamp difference between latest signal miss and
       * front index time stamp, to decide the starting point of the window.*/
      curr_tsq_ptr = err_handler->ts_queue_ptr + err_handler->front_idx;
      ts_diff = latest_ts - *curr_tsq_ptr;

    }while(ts_diff > window_duration);
  }

  /*push time stamp of recent signal miss into queue and allow afe device recovery*/

  /**< Store latest time stamp into queue pointed by
   * back index and modularly increment back index.*/
  curr_tsq_ptr = err_handler->ts_queue_ptr + err_handler->back_idx;
  *curr_tsq_ptr = latest_ts;
  err_handler->back_idx++;
  if(max_num_allowed_recoveries == err_handler->back_idx)
  {
    err_handler->back_idx = 0;
  }
  /**< Update error recovery counter.*/
  err_handler->err_recovery_count++;

#ifdef DBG_DEV_RECOVERY
  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PUSH ts_Q : port_id: 0x%x, latest_ts: %ld%ld, ts_diff :%ld%ld ",\
        port_id, (uint32_t)(latest_ts>>32),(uint32_t)latest_ts, (uint32_t)(ts_diff>>32),(uint32_t)ts_diff);
  for(uint8_t index=0; index< max_num_allowed_recoveries; index++)
  {
    curr_tsq_ptr = err_handler->ts_queue_ptr + index;
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ts_Q : Q[%ld]: %ld%ld ",\
          index, (uint32_t)(*curr_tsq_ptr>>32),(uint32_t)*curr_tsq_ptr);
  }
#endif //DBG_DEV_RECOVERY

  return AFE_ERR_RECOVERY;
}

/**
 * AFE error handler
 *
 * @param[in] port_id, afe port id
 * @returns ADSP_EOK, error code
 */
ADSPResult afe_err_handler(uint32_t port_id)
{
  afe_err_hndlr_type_t err_hndlr_type = AFE_ERR_HNDL_NONE;
#ifndef MDSPMODE
  char err_str[] ="FATAL_ERR: Force crash Q6 due to AFE signal miss";
  uint16_t err_str_len = strlen(err_str);
  

  /**< In post-CS builds, AdspfatalerrApi(), implementation will be
   * stubbed out, and call to this API is NOP, or does nothing.
   *
   * Whereas, in pre-CS builds, this API call forces the crash */
  AdspfatalerrApi(err_str,err_str_len);
#else //MDSPMODE
  afe_debug_crash();
   
#endif   //MDSPMODE
		
  /**< Gets the error handler type, depends on allowed recoveries
   *  in a certain window.*/
  err_hndlr_type = afe_get_err_handler_type(port_id);

  switch(err_hndlr_type)
  {
    case AFE_DEBUG_CRASH:
      afe_debug_crash();
      break;
    case AFE_ERR_RECOVERY:
    {
      /* First move the port state from RUN to SUSPEND state.
       * Device recovery will be done only if the port is in Suspend state.
       *
       * For example in group devices, the member ports which is earlier
       * in RUN state will only be restarted (SUSPEND state is used to
       * detect the actual member port to be restarted)*/
      if(ADSP_EOK == afe_device_move_to_suspend_state(port_id))
      {
        /* Send the recovery msg to AFE service thread. AFE service thread
         * will take care of resetting the device state */
        afe_err_hndlr_send_recovery_msg(port_id);
      }
    }
    break;
    default:
      /* No Action */
      break;
  }

  return ADSP_EOK;
}

