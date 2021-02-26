/*========================================================================
avtimer_server.c

This file contains the implementation of avtimer server.

 Copyright (c) 2010-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_server.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "avtimer_server.h"
#include "avtimer_drv_api.h"
#include "hwd_devcfg.h"
#include "avtimer_hal.h"
#include "Elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

//#define DEBUG_AVTIMER_SERVER

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
static int hw_interrupt_signal_cnt = 0;
static int sw_notify_signal_cnt = 0;
static int sw_client_start_cnt = 0;
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
/* -----------------------------------------------------------------------
 **                  Global Object Definitions
 ** ----------------------------------------------------------------------- */


/**
  Defines a function pointer signature for avtimer server cmd
  handler function.

  @param[in] server_inst_ptr  pointer to avtimer server instance
        structure.
  @param[in] cmd_ptr        Pointer to the command to process.

  @return
  Depends on the command, which is implicitly agreed upon
  between the caller and callee.

  @dependencies
  None.
 */
typedef ADSPResult (*avtimer_server_cmd_handler_func) (void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**
 * function handler to create a timer
 *
 *
 * @return ADSPResult
 */
static ADSPResult avtimer_server_create_timer(void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**
 * function handler to delete a timer
 *
 *
 * @return ADSPResult
 */
static ADSPResult avtimer_server_delete_timer(void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**
 * function handler to get the attributes of  a timer
 *
 *
 * @return ADSPResult
 */
static ADSPResult avtimer_server_getattr_timer(void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**
 * function handler to restart a timer
 *
 *
 * @return ADSPResult
 */
static ADSPResult avtimer_server_restart_timer(void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**
 * function handler to stop a timer
 *
 *
 * @return ADSPResult
 */
static ADSPResult avtimer_server_stop_timer(void* /*server_inst_ptr*/, avtimer_msg_t* /*cmd_ptr*/);

/**< avtimer server thread name */
static char AVTIMER_SERVER_THREAD_NAME[]="AVT";

/**< avtimer server thread stack size */
//ToDo need to revisit if we need 4096?? may be 1024 should be ok?
static const uint32_t AVTIMER_SERVER_THREAD_STACK_SIZE = 4096;


//avtimer server instance pointer
avtimer_server_inst_t *avtimer_server_global_inst_ptr;

/**
 * avtimer server global instance
 */
avtimer_server_inst_t avtimer_server_global_inst;


/* avtimer_server_cmd_handler_table.
 */
static avtimer_server_cmd_handler_func cmd_handler_func_ptr_table[] =
{
    avtimer_server_create_timer,        //0 - AVTIMER_SERVER_CREATE_CMD
    avtimer_server_delete_timer,        //1 - AVTIMER_SERVER_DELETE_CMD
    avtimer_server_getattr_timer,      //2 - AVTIMER_SERVER_GET_ATTR_CMD
    avtimer_server_restart_timer,       //3 - AVTIMER_SERVER_RESTART_CMD
    avtimer_server_stop_timer,          //4 - AVTIMER_SERVER_STOP_CMD

};


/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/**
 * destroy avtimer resource if avtimer init failed.
 *
 * @param[out] me_ptr avtimer_server_inst_t ptr
 * @return     none
 *
 */
static void avtimer_server_destroy(avtimer_server_inst_t* me_ptr);

/**
 * avtimer server workloop to process commands and interrupts
 *
 *
 *
 * @param[out] me_ptr instance pointer of avtimer_server_inst_t
 *
 * @return int
 */
static int avtimer_server_workloop(void* server_inst_ptr);


/**
 * process the command queues in a loop until it is empty , but
 * will return early if timer interrupt occurs by polling for
 * channel status periodically.
 *
 *
 *
 * @param server_inst_ptr avtimer_server_inst_t pointer
 * @param cmd_handler_func_ptr_table function pointer table
 *                                   consisting of command
 *                                   hanlder funtions.
 * @return none
 *
 */
ADSPResult avtimer_server_process_cmdq(void* server_inst_ptr,avtimer_server_cmd_handler_func cmd_func_ptr_table[]);



/**
 * function to process the timer interrupt for hw version v2
 *
 *
 *
 * @param server_inst_ptr pointer to avtimer_server_inst_t
 *                        instance struct
 *
 * @return ADSPResult one of the ADSPResult error codes.
 */
ADSPResult avtimer_server_process_interrupt_v2(void* server_inst_ptr);


/**
 * initialize the active and inactive timer list
 *
 *
 * @param set_timer_ptr pointer to  avtimer_set_timer_t type
 */
void avtimer_list_timer_init(avtimer_set_timer_t *set_timer_ptr);

/**
 *
 * insert the client timer to the input list(active or inactive)
 *
 *
 * @param list_ptr point either to a  active list or inactive
 *                 list
 * @param node_ptr client node to be inserted
 *
 * @return ADSPResult
 */
ADSPResult avtimer_server_insert_client_timer(avtimer_client_node_t **list_dptr, avtimer_client_node_t *node_ptr);


/**
 *
 * remove the client timer from input list(active or inactive)
 *
 *
 * @param list_ptr point either to a  active list or inactive
 *                 list
 * @param node_ptr client node to be removed
 *
 * @return ADSPResult
 */
ADSPResult avtimer_server_remove_client_timer(avtimer_client_node_t **list_dptr, avtimer_client_node_t *node_ptr);



/**
 * Since the active list has changed, the next expiry might also
 * change, hence hardware needs to be reprogrammed.
 * This is only for hw version v2
 *
 * @param set_timer_ptr
 */
static void avtimer_server_config_next_expiry_event_v2(avtimer_set_timer_t *set_timer_ptr);


ADSPResult avtimer_init(void)
{
  qurt_elite_queue_t *cmdq_ptr;
  qurt_elite_channel_t *channel_ptr;

  ADSPResult rc = ADSP_EOK;

  // read device config data
  if(HwdDevCfg_HWSupport(HWD_MODULE_TYPE_AVTIMER))
  {
    //get the global instance of avtimer server
    avtimer_server_global_inst_ptr = &avtimer_server_global_inst;
    cmdq_ptr = (qurt_elite_queue_t *)avtimer_server_global_inst_ptr->avtimer_server_cmdq;
    channel_ptr = (qurt_elite_channel_t *)&avtimer_server_global_inst_ptr->avtimer_server_channel;

    //init the channel
    qurt_elite_channel_init(channel_ptr);

    if (ADSP_FAILED(rc = qurt_elite_queue_init("AVT_SERVER_CMDQ",
                                               AVTIMER_SERVER_MAX_CMD_Q_ELEMENTS,cmdq_ptr))
        || ADSP_FAILED(rc = qurt_elite_signal_init(&avtimer_server_global_inst_ptr->avtimer_server_intrpt_signal))
        || ADSP_FAILED(rc = qurt_elite_signal_init(&avtimer_server_global_inst_ptr->avtimer_server_exit_signal))
    )
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create avtimer server resources with status %d\n",rc);
      avtimer_server_destroy(avtimer_server_global_inst_ptr);
      return rc;
    }

    if (ADSP_FAILED(rc = qurt_elite_channel_addq(channel_ptr,cmdq_ptr,0))
        || ADSP_FAILED(rc = qurt_elite_channel_add_signal(channel_ptr,&avtimer_server_global_inst_ptr->avtimer_server_intrpt_signal,0))
        || ADSP_FAILED(rc = qurt_elite_channel_add_signal(channel_ptr,&avtimer_server_global_inst_ptr->avtimer_server_exit_signal,0))
    )
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure avtimer server resources!");
      avtimer_server_destroy(avtimer_server_global_inst_ptr);
      return rc;

    }

    // Launch the thread
    if (ADSP_FAILED(rc = qurt_elite_thread_launch(&avtimer_server_global_inst_ptr->avtimer_server_tid, AVTIMER_SERVER_THREAD_NAME, NULL,
                                                  AVTIMER_SERVER_THREAD_STACK_SIZE,  elite_get_thrd_prio(ELITETHREAD_AVTIMER_SERVER_PRIO_ID), avtimer_server_workloop, (void*)avtimer_server_global_inst_ptr, QURT_ELITE_HEAP_DEFAULT)))
    {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create avtimer server Thread!");
      avtimer_server_destroy(avtimer_server_global_inst_ptr);
      return rc;
    }

    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_server created successfully");
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer init:avtimer is not supported");
  }

  return rc;
}


static void avtimer_server_destroy(avtimer_server_inst_t* me_ptr)
{
  assert(me_ptr);
  //since the destroy will only be called in the init, no need to pop all queue entries
  // and freeing the command buffers.
  qurt_elite_queue_deinit((qurt_elite_queue_t *)me_ptr->avtimer_server_cmdq);
  qurt_elite_signal_deinit(&me_ptr->avtimer_server_intrpt_signal);
  qurt_elite_signal_deinit(&me_ptr->avtimer_server_exit_signal);
  qurt_elite_channel_destroy(&me_ptr->avtimer_server_channel);
}

ADSPResult avtimer_deinit(void)
{
  ADSPResult rc = ADSP_EOK;
  avtimer_msg_t dummy_cmd;

  if(HwdDevCfg_HWSupport(HWD_MODULE_TYPE_AVTIMER))
  {
    avtimer_drv_deinit();

    //at this point the queue should be empty.
    if ( ADSP_ENEEDMORE != (rc = qurt_elite_queue_pop_front((qurt_elite_queue_t*)avtimer_server_global_inst.avtimer_server_cmdq,(uint64_t*)&dummy_cmd))) {

      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Queue is not empty");
    }

    //check if active list is null. it should be NULL
    if (NULL != avtimer_server_global_inst.list_timer.active_list) {

      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The active list is not empty");
    }


    //send the exit signal to avtimer server
    qurt_elite_signal_send(&avtimer_server_global_inst.avtimer_server_exit_signal);

    //wait for the thread to exit and join
    qurt_elite_thread_join(avtimer_server_global_inst.avtimer_server_tid,&rc);

    //destroy the avtimer server resources
    if (ADSP_EOK == rc ) {
      avtimer_server_destroy(&avtimer_server_global_inst);
    }
  }

  return rc;

}

static int avtimer_server_workloop(void* server_inst_ptr)
{
  ADSPResult rc = ADSP_EOK;
  uint32_t channel_status;
  uint32  hw_revision =0;

  avtimer_server_inst_t* me_ptr  = (avtimer_server_inst_t *)server_inst_ptr;

  //get the bitfield of cmdq and signal mask from the channel.
  me_ptr->msgq_mask = qurt_elite_queue_get_channel_bit((qurt_elite_queue_t *)(me_ptr->avtimer_server_cmdq));
  me_ptr->intrpt_sig_mask = qurt_elite_signal_get_channel_bit(&me_ptr->avtimer_server_intrpt_signal);
  me_ptr->exit_sig_mask = qurt_elite_signal_get_channel_bit(&me_ptr->avtimer_server_exit_signal);

  //formultate the channel wait mask
  me_ptr->wait_mask = me_ptr->msgq_mask | me_ptr->intrpt_sig_mask | me_ptr->exit_sig_mask;

  //initialize the list timer
  avtimer_list_timer_init(&me_ptr->list_timer);

  avtimer_drv_init(&hw_revision);

  switch(hw_revision)
  {
    case AVTIMER_HW_VERSION_2:
    case AVTIMER_HW_VERSION_2_1:
    {
      me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f = avtimer_server_config_next_expiry_event_v2;
      me_ptr->avtimer_server_funcs_table.avtimer_server_process_interrupt_f = avtimer_server_process_interrupt_v2 ;
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Non supported avtimer HW version %d",hw_revision);
    }
  }

  while (1)
  {
    //block on queue and signal
    channel_status = qurt_elite_channel_wait(&me_ptr->avtimer_server_channel,me_ptr->wait_mask);

    //check if we received the interrupt
    if (channel_status & me_ptr->intrpt_sig_mask)
    {
      //clear the interrupt signal
      qurt_elite_signal_clear(&me_ptr->avtimer_server_intrpt_signal);

      //process the interrrupt immediately
      (void)me_ptr->avtimer_server_funcs_table.avtimer_server_process_interrupt_f(me_ptr);
    }

    //check if we have messages to process
    if(channel_status & me_ptr->msgq_mask)
    {
      rc = avtimer_server_process_cmdq(me_ptr,cmd_handler_func_ptr_table);

#ifdef DEBUG_AVTIMER_SERVER
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "result of avtimer_server_process_cmdq is %d!",rc);
#endif

    }
    //server exit signal is received.
    if (channel_status & me_ptr->exit_sig_mask) {

      //clear the exit signal
      qurt_elite_signal_clear(&me_ptr->avtimer_server_exit_signal);
      break;
    }

  }
  return rc;
}


ADSPResult avtimer_server_process_cmdq(void* server_inst_ptr,avtimer_server_cmd_handler_func cmd_func_ptr_table[])
{
  ADSPResult rc;

  avtimer_server_inst_t *me_ptr = (avtimer_server_inst_t *)server_inst_ptr;

  avtimer_msg_t recvd_cmd;

  while (1) {

    //get the next command
    rc = qurt_elite_queue_pop_front((qurt_elite_queue_t*)me_ptr->avtimer_server_cmdq,(uint64_t*)&recvd_cmd);

    //process the command
    if (ADSP_EOK == rc) {

      //check if it is a valid command
      if (AVTIMER_SERVER_NUM_COMMANDS <= recvd_cmd.msg_type )
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported avtimer server command %lu!!\n", recvd_cmd.msg_type);
        return ADSP_EUNSUPPORTED;

      }

      //call the corresponding comamnd handler function
      rc = cmd_handler_func_ptr_table[recvd_cmd.msg_type](me_ptr,&recvd_cmd);

      //store the result of this command and return to the caller
      recvd_cmd.any_cmd_ptr->result = rc;

      //signal back the avtimer client ,who is waiting for this call to finish
      qurt_signal2_set(recvd_cmd.any_cmd_ptr->ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    }
    else if ( ADSP_ENEEDMORE == rc) {
      //there are no more commands in the queue, return immediately
      return ADSP_EOK;
    }
    else {
      //if any others return immediately
      return rc;
    }

    /* poll for interrupt signal, if interrupt signal is received it must be processed immediately
           hence need to return immediately
     */

    if (qurt_elite_channel_poll(&me_ptr->avtimer_server_channel,me_ptr->intrpt_sig_mask)){

      return ADSP_EOK;
    }
  }
}

void avtimer_server_hw_program_next_expiry(avtimer_set_timer_t *set_timer_ptr)
{
  ADSPResult                  rc = ADSP_EOK;
  avtimer_tick_type           curr_time, first_client_time;
  avtimer_client_node_t       *client_node_ptr;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  first_client_time = AVTIMER_MAX_DURATION_TICKS;

  /* Check if the are more active clients, then program the next expiry
       All the clients within the AVTIMER_TICK_ERROR_MARGIN of current time are already served above 

       Below infinite while() loop configures the next expiry. Because of the AV timer context switches, if next expiry(ies)
       also falls within AVTIMER_TICK_ERROR_MARGIN, we continue to signal the clients.

       Break condition for this while() loop is,  if the next expiry is future in time above AVTIMER_TICK_ERROR_MARGIN.

       This while() loop helps serving the clients immdiately if the expiry is within the AVTIMER_TICK_ERROR_MARGIN
       and make the code robust against FIQ callback scheduling latencies. */

  while (1)
  {
    if(NULL == set_timer_ptr->active_list)
    {
      break;
    }
    else /* Active list is non-null */
    {
      client_node_ptr = set_timer_ptr->active_list;

      /* Get the next client's expiry tick */
      first_client_time = set_timer_ptr->active_list->expiry;

      /* Get the current time again to account for any thread context switches */
      curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

      /* Check if there are clients within the AVTIMER_TICK_ERROR_MARGIN.
               Signal these clients immediately without programming the timer */
      if(curr_time + AVTIMER_TICK_ERROR_MARGIN  >= first_client_time)
      {
        /* Point the active list to the next client timer */
        set_timer_ptr->active_list =  client_node_ptr->next_ptr;

        client_node_ptr->next_ptr = NULL;

        /* Check if the client timer is periodic or not */
        if (0 < client_node_ptr->reload )
        {
          /* Periodic timer, reload the timer and put it back in to active list */
          client_node_ptr->expiry += client_node_ptr->reload;

          if (client_node_ptr->expiry <= (curr_time + AVTIMER_TICK_ERROR_MARGIN))
          {
            /* Need to miss this expiry, since it is too close,
                           set up the next expiry instead */
            client_node_ptr->expiry += client_node_ptr->reload;
          }

          /* Add the client timer to the active list */
          if(ADSP_EOK != (rc = avtimer_server_insert_client_timer(&set_timer_ptr->active_list, client_node_ptr)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in avtimer_server_insert_client_timer() with return code: %d", rc);
          }
        }
        else /* One shot timer */
        {
          /* One shot timer, set timer state to expired */
          client_node_ptr->state = AVTIMER_EXPIRED;
        }

        /* Signal the sw timer the expiry event */
        qurt_elite_signal_send(&client_node_ptr->cb_data.cb_signal);

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
        sw_notify_signal_cnt++;
#endif /* #if defined(PRINT_AVTIMER_DRV_DBG_MSG) */

        first_client_time = AVTIMER_MAX_DURATION_TICKS;

        /* If there are still active clients */
        if (NULL != set_timer_ptr->active_list )
        {
          /* Get the first client item expiry */
          first_client_time = set_timer_ptr->active_list->expiry;
        }
      }
      else  /*If next expiry is above AVTIMER_TICK_ERROR_MARGIN, break the loop */
      {
#ifdef DEBUG_AVTIMER_SERVER
        MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "t_time![0x%x%x], c_time![0x%x%x]", (unsigned int)(first_client_time >> 32), (unsigned int)first_client_time, (unsigned int)(curr_time >> 32), (unsigned int)curr_time);
#endif
        break;
      }
    } /* If client list is non-null */
  } /* while(1) */

  /* Configure HW to "first_client_time". The next ISR from HW will be at first_client_time
       If the active client list has become empty, next expiry will be set to AVTIMER_MAX_DURATION_TICKS */

  hal_avtimer_prog_hw_timer_int0(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode, (uint64_t)first_client_time);

  /* Store the next expiry */
  set_timer_ptr->next_expiry = first_client_time;

  return;
}


ADSPResult avtimer_server_process_interrupt_v2(void* server_inst_ptr)
{
  ADSPResult rc = ADSP_EOK;
  avtimer_tick_type curr_time,first_client_time;
  avtimer_client_node_t *client_node_ptr;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  avtimer_server_inst_t* me_ptr = (avtimer_server_inst_t*)server_inst_ptr;

  //get the current hw time tick
  curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  /* Traverse the active list until current time is greater than
       the client expiry by an error margin. reload the periodic timers
       and remove the one shot expired timers*/

  while ((client_node_ptr = me_ptr->list_timer.active_list, NULL != client_node_ptr)  &&
      (client_node_ptr->expiry <= (curr_time + AVTIMER_TICK_ERROR_MARGIN)))
  {

    //point the active list to the next client timer
    me_ptr->list_timer.active_list =  client_node_ptr->next_ptr;

    client_node_ptr->next_ptr = NULL;

    //check if the client timer is periodic or not
    if (0 < client_node_ptr->reload )
    {

      /*periodic timer, reload the timer and put it back in to active list*/
      client_node_ptr->expiry += client_node_ptr->reload;

      if (client_node_ptr->expiry <= (curr_time + AVTIMER_TICK_ERROR_MARGIN))
      {
        //need to miss this expiry, since it is too close,
        //set up the next expiry instead.
        client_node_ptr->expiry += client_node_ptr->reload;
      }

      //add the client timer to the active list
      rc = avtimer_server_insert_client_timer(&me_ptr->list_timer.active_list, client_node_ptr);


    }
    else
    {
      //one shot timer, set it to expire
      client_node_ptr->state = AVTIMER_EXPIRED;

    }

    //signal the sw timer the expiry event
    qurt_elite_signal_send(&client_node_ptr->cb_data.cb_signal);

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
    sw_notify_signal_cnt++;
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

    //get the current time again to count any slip from the previous hal_avtimer_get_current_hw_timer_tick() call (big concern is context switch is happened in between)
    curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);
  }

  /* Now the active list has changed.*/
  //make sure the first item in the active list is configured as the next expiry event

  /*init first_client_time with max duration. This is required, when all client timers
      are deleted or expired, the next expiry must be set to the max duration value,
      so that there are no more spurious interruppt*/

  first_client_time = AVTIMER_MAX_DURATION_TICKS;


  /* If there are still active clients */
  if (NULL != me_ptr->list_timer.active_list )
  {
    /* Get the first client item expiry */
    first_client_time = me_ptr->list_timer.active_list->expiry;

    /* Check if the first client expiry time is same as the next expiry.. then nothing to do */
    if (first_client_time == me_ptr->list_timer.next_expiry)
    {
      //if the current time is close to target time, go ahead and signal client immediately
      if((curr_time + AVTIMER_TICK_ERROR_MARGIN) >= first_client_time)
      {
#ifdef DEBUG_AVTIMER_SERVER
        MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVTimer-check t_time![0x%x%x], c_time![0x%x%x]\n", (unsigned int)(first_client_time >> 32), (unsigned int)first_client_time, (unsigned int)(curr_time >> 32), (unsigned int)curr_time);
#endif
      }
      else /* Error condition, programmed only one timer previously, but still next_expiry is set in future */
      {
#ifdef DEBUG_AVTIMER_SERVER           
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVT ERROR: Next expiry already set, t_time[MSB:0x%lx, LSB:0x%lx],"
              "c_time[MSB:0x%lx, LSB:0x%lx]",
              (uint32_t)(first_client_time >> 32), (uint32_t)first_client_time,
              (uint32_t)(curr_time >> 32), (uint32_t)curr_time);
#endif
      }
    }
  }

  /* Program AVT h/w for next expiry */
  avtimer_server_hw_program_next_expiry(&me_ptr->list_timer);

  return rc;
}



static ADSPResult avtimer_server_create_timer(void* server_inst_ptr, avtimer_msg_t* msg_ptr)
{

  ADSPResult rc = ADSP_EOK;
  avtimer_client_node_t *client_node_ptr;
  avtimer_cmd_create_t *create_cmd_ptr;
  avtimer_tick_type curr_time;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  avtimer_server_inst_t* me_ptr = (avtimer_server_inst_t*)server_inst_ptr;

  //create a client node
  if (NULL==(client_node_ptr = (avtimer_client_node_t *)qurt_elite_memory_malloc
      (sizeof(avtimer_client_node_t),QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate av timer client node ");
    return ADSP_ENOMEMORY;
  }

  //get the create command info
  create_cmd_ptr = msg_ptr->create_cmd_ptr;

  //populate the client node

  client_node_ptr->type = create_cmd_ptr->type;
  client_node_ptr->cb_data = create_cmd_ptr->cb;

  qurt_elite_signal_init(&client_node_ptr->cb_data.cb_signal);
  client_node_ptr->cb_data.cb_signal.pChannel = client_node_ptr->cb_data.cb_channel_ptr;
  client_node_ptr->cb_data.cb_signal.unMyChannelBit = client_node_ptr->cb_data.mask;

#ifdef DEBUG_AVTIMER_SERVER
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "type is 0x%x",client_node_ptr->type);
#endif

  //get the current hw time tick (1time tick = 1usec)
  curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  //save the start time, useful for calculating in remaining time and duration
  client_node_ptr->start = curr_time;

  if (AVTIMER_ONESHOT_ABSOLUTE ==  client_node_ptr->type)
  {
    client_node_ptr->expiry = create_cmd_ptr->expiry;
  }
  else {
    /* periodic or one shot duration*/
    client_node_ptr->expiry = client_node_ptr->start + create_cmd_ptr->duration;
  }

  //configure reload and check if the expiry is too close.
  if (AVTIMER_PERIODIC == client_node_ptr->type)
  {
    client_node_ptr->reload = create_cmd_ptr->duration;
    client_node_ptr->state = AVTIMER_ACTIVE;
  }
  else
  { /* AVTIMER_ONESHOT_ABSOLUTE or AVTIMER_ONESHOT_DUATION */

    client_node_ptr->state = AVTIMER_CREATED;
    //reload is zero for one shot timers
    client_node_ptr->reload = 0;
    //check if the one shot expiry is too close
    if (client_node_ptr->expiry <= (client_node_ptr->start + AVTIMER_TICK_ERROR_MARGIN))
    {
      //free the client before returning not ready
      qurt_elite_memory_free(client_node_ptr);
      return ADSP_ENOTREADY;
    }

  }

  /** start the periodic timer right away. But for one shot
        just create it, but dont start it.
   */

  if (AVTIMER_PERIODIC == client_node_ptr->type)
  {
    //add the client timer to the active list
    rc = avtimer_server_insert_client_timer(&me_ptr->list_timer.active_list, client_node_ptr);

    //configure the next expiry event , since the active list has changed.
    if(NULL != me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f)
    {
      me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f(&me_ptr->list_timer);
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer server is not ready");
    }
  }


  //return the client_node_ptr to the caller
  create_cmd_ptr->timer = (avtimer_t)client_node_ptr;

  return rc;
}


static ADSPResult avtimer_server_stop_timer(void* server_inst_ptr, avtimer_msg_t* msg_ptr)
{

  ADSPResult rc = ADSP_EOK;
  avtimer_client_node_t *client_node_ptr;
  avtimer_cmd_stop_t *stop_cmd_ptr;

  avtimer_server_inst_t* me_ptr = (avtimer_server_inst_t*)server_inst_ptr;

  //get the stop command
  stop_cmd_ptr = msg_ptr->stop_cmd_ptr;

  //get the client timer object
  client_node_ptr = (avtimer_client_node_t *)stop_cmd_ptr->timer;

  assert(client_node_ptr);

  //only one shot timers can only be stopped. return error if periodic timer is requested to stop
  if (0 != client_node_ptr->reload)
  {
    return ADSP_EFAILED;
  }

  //check if the timer has expired already

  if (AVTIMER_EXPIRED == client_node_ptr->state)
  {
    //nothing to do, since the timer has already expired.
    return ADSP_EOK;
  }
  else if (AVTIMER_ACTIVE == client_node_ptr->state)
  {
    //stop an active running timer

    //first remove the timer from the active list
    if (ADSP_FAILED (rc = avtimer_server_remove_client_timer
                     (&me_ptr->list_timer.active_list, client_node_ptr)))
    {
      return rc;
    }
    //mark the timer as expired
    client_node_ptr->state = AVTIMER_EXPIRED;

    //configure the next expiry event , since the active list has changed.
    if(NULL != me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f)
    {
      me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f(&me_ptr->list_timer);
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer server is not ready");
    }
  }
  else
  {
    return ADSP_EFAILED;
  }

  return rc;

}

static ADSPResult avtimer_server_delete_timer(void* server_inst_ptr, avtimer_msg_t* msg_ptr)
{

  ADSPResult rc = ADSP_EOK;
  avtimer_client_node_t *client_node_ptr;
  avtimer_cmd_delete_t *delete_cmd_ptr;

  avtimer_server_inst_t* me_ptr = (avtimer_server_inst_t*)server_inst_ptr;

  //get the delete command
  delete_cmd_ptr = msg_ptr->delete_cmd_ptr;

  //get the client timer object
  client_node_ptr = (avtimer_client_node_t *)delete_cmd_ptr->timer;

  assert(client_node_ptr);

  //delete only active or expired or created timer
  if ((AVTIMER_ACTIVE != client_node_ptr->state)
      && (AVTIMER_EXPIRED != client_node_ptr->state)
      && (AVTIMER_CREATED != client_node_ptr->state)
  )
  {
    return ADSP_EFAILED;
  }

  //if the timer is expired or just created, free its memory
  if ((AVTIMER_EXPIRED == client_node_ptr->state) || (AVTIMER_CREATED == client_node_ptr->state) )
  {
    //free the client timer memory
    qurt_elite_memory_free(client_node_ptr);
  }
  else
  {
    //now removing the active running timer
    //first remove the timer from the active list
    if (ADSP_FAILED (rc = avtimer_server_remove_client_timer
                     (&me_ptr->list_timer.active_list, client_node_ptr)))
    {
      return rc;
    }
    //mark the timer as invalid
    client_node_ptr->state = AVTIMER_INVALID;

    //configure the next expiry event , since the active list has changed.
    if(NULL != me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f)
    {
      me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f(&me_ptr->list_timer);
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer server is not ready");
    }
    //free the client timer memory
    qurt_elite_memory_free(client_node_ptr);

  }
  return rc;

}

static ADSPResult avtimer_server_getattr_timer(void* server_inst_ptr, avtimer_msg_t* msg_ptr)
{

  ADSPResult rc = ADSP_EOK;
  avtimer_client_node_t *client_node_ptr;
  avtimer_cmd_getattr_t *getattr_cmd_ptr;
  avtimer_attr_t *attr_ptr;
  avtimer_tick_type curr_time;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  //get the getattr command
  getattr_cmd_ptr = msg_ptr->getattr_cmd_ptr;

  //get the client timer object
  client_node_ptr = (avtimer_client_node_t *)getattr_cmd_ptr->timer;

  assert(client_node_ptr);

  attr_ptr = getattr_cmd_ptr->attr_ptr;

  //get the attributes of only active or expired timer
  if ((AVTIMER_ACTIVE != client_node_ptr->state) && (AVTIMER_EXPIRED != client_node_ptr->state))
  {
    return ADSP_EFAILED;
  }

  //copy the current state of timer
  attr_ptr->state = client_node_ptr->state;

  attr_ptr->type = client_node_ptr->type;

  //get the current hw timer tick
  curr_time = (uint64_t)hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  //update the remianing if expiry is in future otherwise set to zero
  if (0 >=(attr_ptr->remaining = (client_node_ptr->expiry - curr_time)))
  {
    attr_ptr->remaining = 0;
  }

  if (0 < client_node_ptr->reload)
  {
    //for periodic timers, duration is nothing but the reload value
    attr_ptr->duration = client_node_ptr->reload;
  }
  else
  {
    //for one shot timers
    attr_ptr->expiry = client_node_ptr->expiry;
    attr_ptr->duration = client_node_ptr->expiry - client_node_ptr->start;

  }
  return rc;

}


static ADSPResult avtimer_server_restart_timer(void* server_inst_ptr, avtimer_msg_t* msg_ptr)
{

  ADSPResult rc = ADSP_EOK;
  avtimer_client_node_t *client_node_ptr;
  avtimer_cmd_restart_t *restart_cmd_ptr;

  avtimer_server_inst_t* me_ptr = (avtimer_server_inst_t*)server_inst_ptr;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  //get the restart command
  restart_cmd_ptr = msg_ptr->restart_cmd_ptr;

  //get the client timer object
  client_node_ptr = (avtimer_client_node_t *)restart_cmd_ptr->timer;

  assert(client_node_ptr);

  //the restart is only allowed for one shot timers.
  if (AVTIMER_PERIODIC == client_node_ptr->type )
  {
    //return failed if restart is requested for periodic timers
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_server_restart_timer ERROR-check client_node_ptr->type = %d\n", client_node_ptr->type);
    return ADSP_EFAILED;
  }

  //check if timer has already expired or not
  if (AVTIMER_ACTIVE == client_node_ptr->state)
  {
    /*restart an active timer */
    //first remove from the active list. Removing it from active list makes it expired.
    rc = avtimer_server_remove_client_timer(&me_ptr->list_timer.active_list, client_node_ptr);

  }

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
  sw_client_start_cnt++;
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

  //get the currnt time
  client_node_ptr->start = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  if (AVTIMER_ONESHOT_ABSOLUTE == client_node_ptr->type)
  {
    client_node_ptr->expiry = restart_cmd_ptr->duration;
#ifdef DEBUG_AVTIMER_SERVER
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "is type abs 0x%x",client_node_ptr->type);
#endif
  }
  else
  {
    client_node_ptr->expiry = client_node_ptr->start + restart_cmd_ptr->duration;
#ifdef DEBUG_AVTIMER_SERVER
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "dura %d",(int)restart_cmd_ptr->duration);
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "expiry![0x%x%x]\n", (unsigned int)(client_node_ptr->expiry >> 32), (unsigned int)client_node_ptr->expiry);
#endif
  }

  //reload is zero for one shot timers
  client_node_ptr->reload = 0;

  //check if the one shot expiry is too close
  if (client_node_ptr->expiry <= (client_node_ptr->start + AVTIMER_TICK_ERROR_MARGIN))
  {
    //set the signal immediately
    qurt_elite_signal_send(&client_node_ptr->cb_data.cb_signal);

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
    sw_notify_signal_cnt++;
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

    //set the state to expired
    client_node_ptr->state = AVTIMER_EXPIRED;

    //return success
    return ADSP_EOK;
  }

  client_node_ptr->state = AVTIMER_ACTIVE;

  //add the client timer to the active list
  rc = avtimer_server_insert_client_timer(&me_ptr->list_timer.active_list, client_node_ptr);

  //configure the next expiry event , since the active list has changed.
  if(NULL != me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f)
  {
    me_ptr->avtimer_server_funcs_table.avtimer_server_config_next_expiry_event_f(&me_ptr->list_timer);
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer server is not ready");
  }

  return rc;

}

ADSPResult avtimer_server_insert_client_timer(avtimer_client_node_t **list_dptr, avtimer_client_node_t *node_ptr)
{
  avtimer_client_node_t **curr_dptr;
  avtimer_tick_type client_expiry;

  curr_dptr = list_dptr;
  client_expiry = node_ptr->expiry;

  //insert the client timer in the ascending order of expiry
  while ( (NULL != (*curr_dptr)) && (client_expiry >= (*curr_dptr)->expiry))
  {
    curr_dptr = &(*curr_dptr)->next_ptr;
  }

  node_ptr->next_ptr = *curr_dptr;
  *curr_dptr = node_ptr;

  return ADSP_EOK;
}


ADSPResult avtimer_server_remove_client_timer(avtimer_client_node_t **list_dptr, avtimer_client_node_t *node_ptr)
{
  avtimer_client_node_t **curr_dptr;

  curr_dptr = list_dptr;

  //find the client timer in the list
  while ( (NULL != (*curr_dptr)) && (node_ptr != (*curr_dptr)))
  {
    curr_dptr = &(*curr_dptr)->next_ptr;
  }

  // if the client timer is found in the list, remove it
  if (node_ptr == *curr_dptr)
  {
    *curr_dptr = node_ptr->next_ptr;
    node_ptr->next_ptr = NULL;
  }
  else {
    return ADSP_ENOTEXIST;

  }

  return ADSP_EOK;
}



static void avtimer_server_config_next_expiry_event_v2(avtimer_set_timer_t *set_timer_ptr)
{
  avtimer_tick_type       curr_time, first_client_time;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  //get the current hw time tick
  curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  // if the timer interrupt is already occured or about to occur
  if (set_timer_ptr->next_expiry <= (curr_time + AVTIMER_TICK_ERROR_MARGIN ))
  {
    //let the isr program the match value of the timer
    //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_server_config_next_expiry_event ERROR-check next_expiry = %d\n", set_timer_ptr->next_expiry);
    return;
  }

  /*since the active list has changed,make sure the first item of the active list is what is programmed
     as the match value of timer interrupt.
   */

  /*init first_client_time with max duration. This is required, when all client timers
      are deleted or expired, the next expiry must be set to the max duration value,
      so that there are no more spurious interruppt
   */
  first_client_time = AVTIMER_MAX_DURATION_TICKS;


  if (NULL != set_timer_ptr->active_list )
  {
    //get the expiry time of the first timer in the active list
    first_client_time = set_timer_ptr->active_list->expiry;

    //check if the first_client_time is the next expiry event.
    if (first_client_time == set_timer_ptr->next_expiry)
    {
      //no need to program the h/w, since active list did not modify the first item in the list
      //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_server_config_next_expiry_event ERROR-check next_expiry = %d\n", set_timer_ptr->next_expiry);
      return;
    }
  }


  /* Since here,the new item is added/deleted to the first in the active list
       program the hw with the new match value */    

  avtimer_server_hw_program_next_expiry(set_timer_ptr);

  return;
}


void avtimer_list_timer_init(avtimer_set_timer_t *set_timer_ptr){

  set_timer_ptr->active_list = NULL;
  set_timer_ptr->inactive_list = NULL;
  set_timer_ptr->next_expiry = AVTIMER_MAX_DURATION;

}

void avtimer_server_send_interrupt_signal(void)
{
  qurt_elite_signal_send(&avtimer_server_global_inst.avtimer_server_intrpt_signal);

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
  hw_interrupt_signal_cnt++;
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
}

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

/*
How to enable avtimer driver level message?
1. Go to \adsp_proc\avs\afe\drivers\hw\avtimer\avtimer_driver\inc\avtimer_drv_api.h
    Enable define of #define PRINT_AVTIMER_DRV_DBG_MSG

 */
void avtimer_server_dump_dbg_msg(void)
{
  avtimer_tick_type curr_time, curr_match_time = 0xFFFFFFFF;

  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  //get the current hw time tick
  curr_time = hal_avtimer_get_current_hw_timer_tick(AVTIMER_HW_INSTANCE_INDEX_0, hw_mode);

  //hw interrupt cnt
  //hw_interrupt_signal_cnt

  //sw signal cnt
  //sw_notify_signal_cnt

  //timer restart cnt
  //sw_client_start_cnt

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hw interrupt cnt %d sw signal cnt %d timer restart cnt %d",hw_interrupt_signal_cnt, sw_notify_signal_cnt, sw_client_start_cnt);
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "curr_time![MSB %lu   LSB %lu]\n", (uint32_t)(curr_time >> 32), (uint32_t)curr_time);
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "curr_match_time![MSB %lu   LSB %lu]\n", (uint32_t)(curr_match_time >> 32), (uint32_t)curr_match_time);
}
#endif

