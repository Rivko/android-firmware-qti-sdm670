/**
  @file ListenProcSvc_Util.cpp
  @brief This file contains Listen Processing service utility functions. Utility
  functions are functions that assists the service by implementing certain
  logics/routines and off-loads these logics (burden) from Service layer. They
  aren't responsible for managing/updating session variables and ideally
  shouldn't have a view of the full session data.
*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_Util.cpp#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  06/23/14    Unni     Added LSM topology support
  11/26/12    Sudhir   Initial version
==============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "ListenProcSvc_Includes.h"
#include "ListenProcSvc_CustMsg.h"
#include "Elite_fwk_extns_detection_engine.h"
#include "Elite_fwk_extns_lab.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEInterface.h"
#include "adsp_lsm_api.h"
#include "audpp_util.h"
#include "topo_utils.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "ListenProcSvc_lab_handler.h"
#include "ListenStreamMgr.h"
#include "EliteLoggingUtils_i.h"
#include "adsp_media_fmt.h"
#include "ListenStreamMgr_AprIf.h"
#include "ListenProcSvc_Deprecate.h"
#include "EliteMsg_AdmCustom.h"
#include "MixerSvc_Interface.h"
#include "adsp_lsm_session_modules_params.h"


/*------------------------------------------------------------------------------
 * Constant / Define Declarations
 *----------------------------------------------------------------------------*/
static char LISTEN_PROC_SVC_DATA_Q_NAME[]= "LPSd";
static char LISTEN_PROC_SVC_CMD_Q_NAME[] = "LPSc";
static char LISTEN_PROC_SVC_RESP_Q_NAME[] = "LPSr";
static char LISTEN_PROC_SVC_OUT_Q_NAME[] = "LPSo";

#define LISTEN_INPUT_AFE_INTERNAL_FRAME_PER_SEC 100 
/* Connecting to AFE using 10ms buffers*/
#define AFE_OUT_FRAME_SIZE_10_MSEC 10

/* Number of buffers between LSM and MTMX */
#define NUM_BUFERS_LSM_AND_MTMX   4
/* Matrix always gives the buffers in 5 msec */
#define MATRIX_OUT_FRAME_SIZE_5_MSEC  5

/* Marco for ceil div, alter. (x + y - 1)/y includes overflow*/
#define ceil_div(x, y) (x)==0 ? 0 : 1 + (((x) - 1) / (y))
#define IS_NOT_FOUR_BYTE_ALLIGN(a) (a & 0x3)
#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

/** Shift factor for Q31 <=> Q28 conversion for 32-bit PCM
*/
#define QFORMAT_SHIFT_FACTOR            (PCM_32BIT_Q_FORMAT - ELITE_32BIT_PCM_Q_FORMAT)

extern qurt_elite_globalstate_t qurt_elite_globalstate;
static ADSPResult listen_proc_send_cmd_to_mtmx(listen_proc_svc_t *me_ptr, uint32_t cmd);
static ADSPResult listen_proc_send_cmd_to_adm(listen_proc_svc_t *me_ptr, uint32_t cmd);
static ADSPResult listen_proc_reconnect_to_port(listen_proc_svc_t *me_ptr, uint16_t port_id);

#define MASK_32BYTE_ALIGNMENT  0x1F

/* we can use the MASK_32BYTE_ALIGNMENT, but for targets where cache line is changing this is useful*/
#define  LSM_CACHE_LINE_SIZE  32 // Must be a power of 2
#define  LSM_CACHE_ALIGNMENT_MASK  (LSM_CACHE_LINE_SIZE - 1)

static inline bool_t check_cache_line_alignment(uint32_t addr)
{
  return ((addr & LSM_CACHE_ALIGNMENT_MASK) == 0);
}

/*This is included here since the corresonding definition has been moved to prop folder
However in case deprecated LSM_CMD_OPEN_TX is called then the module ID needs to be
mapped to the updated module ID. The framework needs to know if the deprecated module ID
is used. Hence defining it here until the API is deleted*/
#define LSM_MODULE_VOICE_WAKEUP           0x00012C00


/*------------------------------------------------------------------------------
 * Local Function Declarations
 *----------------------------------------------------------------------------*/
static inline
ADSPResult listen_proc_framework_set_get_params(listen_proc_svc_t *me_ptr,
                                                uint32_t param_id,
                                                uint32_t param_size,
                                                int8_t *params_buffer_ptr,
                                                bool_t set_flag,
                                                uint32_t* actual_buf_len);
// Used by ListenProcSvc_Deprecate.cpp - hence not static inline
ADSPResult listen_proc_iter_set_get_param(listen_proc_svc_t *me_ptr,
                                          int8_t *payload_address,
                                          uint32_t payload_size,
                                          bool_t set_flag, uint32_t opcode);
static inline
ADSPResult listen_proc_svc_send_data_read_done(listen_proc_svc_t* me_ptr, 
                                               uint32_t out_buf_size, 
                                               ADSPResult err_code);
static inline
ADSPResult listen_proc_svc_get_new_out_buffer(listen_proc_svc_t* me_ptr);

static inline
ADSPResult listen_proc_svc_get_output_buffer(listen_proc_svc_t* me_ptr, 
                                             elite_apr_packet_t ** apr_pkt_ptr_ptr, 
                                             lsm_data_cmd_read_t ** out_buf_ptr_ptr);
static inline
ADSPResult listen_proc_svc_log_input_data(listen_proc_svc_t* me_ptr,
                                    int8_t *buf_addr,
                                    uint32_t buf_size);
static inline
ADSPResult listen_proc_svc_log_output_data(listen_proc_svc_t* me_ptr,
                                    int8_t *buf_addr,
                                    uint32_t buf_size);
static inline
void listen_proc_svc_process_output_data_q(listen_proc_svc_t* me_ptr);

/*------------------------------------------------------------------------------
 * Function Definitions
 *----------------------------------------------------------------------------*/
/**
  This function creates all service queues

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_create_svc_q(listen_proc_svc_t *me_ptr)
{
  /* static variable counter to make queue and thread name strings unique limit
     to 16 bits so it will roll over and and only cost 4 characters in
     hexadecimal. Queues in existence likely to have unique names, but not
     required... */
  int count;
  char aDataQName[QURT_ELITE_DEFAULT_NAME_LEN]; /* data queue name */
  char aCmdQName[QURT_ELITE_DEFAULT_NAME_LEN];  /* command queue name */
  char aOutQName[QURT_ELITE_DEFAULT_NAME_LEN];  /* output data queue name */
  char aRespQName[QURT_ELITE_DEFAULT_NAME_LEN]; /* response buffer queue name */
  ADSPResult result;
  
  if(!me_ptr)
  {
    return ADSP_EBADPARAM;
  }

  count = qurt_elite_atomic_get(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
  qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

  /* name the queues. Name is concatenated with 5 items:
     - unique 3 letter code for service
     - letter abbreviating which queue, e.g. 'd' 'c' 'b' for data, cmd, buf.
     - number in case there are multiple queues of the same type,
       e.g. 2 buf q's
     - count (up to 5 digits)
       e.g. the first instance of this service would name its dataQ "/EPLd00"
       and cmd q "/EPLc00" */
  snprintf(aDataQName, QURT_ELITE_DEFAULT_NAME_LEN,
           "%s%x", LISTEN_PROC_SVC_DATA_Q_NAME, count);
  snprintf(aCmdQName, QURT_ELITE_DEFAULT_NAME_LEN,
           "%s%x", LISTEN_PROC_SVC_CMD_Q_NAME, count);
  snprintf(aOutQName, QURT_ELITE_DEFAULT_NAME_LEN,
           "%s%x", LISTEN_PROC_SVC_OUT_Q_NAME,count);
  snprintf(aRespQName, QURT_ELITE_DEFAULT_NAME_LEN,
           "%s%x", LISTEN_PROC_SVC_RESP_Q_NAME, count);

  if(ADSP_FAILED(result = qurt_elite_queue_create(aDataQName,
                          MAX_DATA_Q_ELEMENTS, &(me_ptr->svc_handle.dataQ)))  ||
     ADSP_FAILED(result = qurt_elite_queue_create(aCmdQName,
                          MAX_CMD_Q_ELEMENTS, &(me_ptr->svc_handle.cmdQ)))    ||
     ADSP_FAILED(result = qurt_elite_queue_create(aOutQName,
                          MAX_CMD_Q_ELEMENTS, &(me_ptr->svc_handle.gpQ)))     ||
     ADSP_FAILED(result = qurt_elite_queue_create(aRespQName,
                          MAX_CMD_Q_ELEMENTS, &(me_ptr->response_q_ptr)))       )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Lpro[%d] failed to create service queues!!", me_ptr->session_id);
#endif
    return result;
  }

#ifdef LSM_DEBUG
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "Lpro[%d] dataQ=0x%p, cmdQ=0x%p",
        me_ptr->session_id,me_ptr->svc_handle.dataQ, me_ptr->svc_handle.cmdQ);
#endif

  /*  set up channel */
  qurt_elite_channel_init(&me_ptr->channel);

  if(ADSP_FAILED(result = qurt_elite_channel_addq(&me_ptr->channel,
                          me_ptr->svc_handle.cmdQ,LISTEN_PROC_CMD_SIG))      ||
     ADSP_FAILED(result = qurt_elite_channel_addq(&me_ptr->channel,
                          me_ptr->svc_handle.dataQ,LISTEN_PROC_DATA_SIG))    ||
     ADSP_FAILED(result = qurt_elite_channel_addq(&me_ptr->channel,
                          me_ptr->svc_handle.gpQ,LISTEN_PROC_OUT_SIG))       ||
     ADSP_FAILED(result = qurt_elite_channel_addq(&me_ptr->channel,
                           me_ptr->response_q_ptr,LISTEN_PROC_RESP_SIG))        )
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] failed to add service queues to channel "
          "w/ result = %d!!", me_ptr->session_id, result);
#endif
    listen_proc_svc_destroy_svc(me_ptr);
    return result;
  }

  /* fill service ID */
  me_ptr->svc_handle.unSvcId = ELITE_DYN_ENCODER_SVCID;

  return ADSP_EOK;
}

/**
  Flushes buffers present in Queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
         q_type [in] Type of listen proc queue to flush
         response_result [in] (Only Output Queue)This is the response error result to be propagated to client
  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_flush_q(listen_proc_svc_t *me_ptr,
                                   listen_proc_q_type_t q_type,
                                   ADSPResult response_result)
{
  qurt_elite_queue_t* q_ptr = NULL;
  elite_msg_any_t* msg_ptr = NULL;

  if(NULL == me_ptr)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "NULL me_ptr");
#endif
    return ADSP_EBADPARAM;
  }

  switch(q_type)
  {
    case LISTEN_PROC_CMD_Q:
      q_ptr = me_ptr->svc_handle.cmdQ;
      msg_ptr = &me_ptr->cmd_msg;
      break;
    case LISTEN_PROC_INPUT_DATA_Q:
      q_ptr = me_ptr->svc_handle.dataQ;
      msg_ptr = &me_ptr->input_data_msg;
      break;
    case LISTEN_PROC_OUTPUT_DATA_Q:
      q_ptr = me_ptr->svc_handle.gpQ;
      msg_ptr = &me_ptr->output_data_msg;
      break;
    default:
      break;
  }

  if(NULL == q_ptr || NULL == msg_ptr)
  {
#ifdef LSM_DEBUG
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "q_type[0x%x] gave q_ptr[%p], msg_ptr[%p]",
          q_type, q_ptr, msg_ptr);
#endif
    return ADSP_ENOTEXIST;
  }

  do{
    /* If exists any message */
    if(NULL != msg_ptr->pPayload)
    {
      if(LISTEN_PROC_OUTPUT_DATA_Q == q_type) /* OUTPUT QUEUE */
      {
        /* For the already popped out buffer some data may be already filled in it.*/
        if(ADSP_EOK != listen_proc_svc_send_data_read_done(me_ptr,
                                                           me_ptr->out_buf_params.out_buf_wr_offset,
                                                           response_result))
        {
#ifdef LSM_DEBUG
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Lpro[%d]:Fail release output buf",
                me_ptr->session_id);
            /* but proceed to release other data messages*/
#endif
        }
      } /* OUTPUT QUEUE */
      else if (ELITE_APR_PACKET == msg_ptr->unOpCode)  /* APR CMD QUEUE */
      {
        elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t *) (msg_ptr->pPayload);
        lsm_generate_ack(apr_pkt_ptr, ADSP_ENOTREADY, NULL, 0, 0);
      } /* APR CMD QUEUE */
      else /* All other types of Queues */
      {
        if(ADSP_EOK != elite_msg_finish_msg(msg_ptr, ADSP_ENOTREADY))
        {
#ifdef LSM_DEBUG
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Lpro[%d]:Fail release msg in q_type[0x%x]!",
                me_ptr->session_id, q_type);
          /* but proceed to release other data messages */
#endif
        }
      } /* All other types of Queues */

      /* Clear the processed message */
      msg_ptr->pPayload = NULL;
    }
    /* Check if more message is present in queue */
  }while(ADSP_EOK == qurt_elite_queue_pop_front(q_ptr,(uint64_t*)msg_ptr));

  return ADSP_EOK;
}

/**
  Function to reinit LSM modlules for next detection session

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @is_start [in] Indicate whether to start or just sent media format

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_svc_reinit(listen_proc_svc_t* me_ptr, bool_t is_start)
{
  ADSPResult result = ADSP_EOK;
  uint32_t frame_size_in_samples = 0;
  if(TRUE == is_start)
  {
    frame_size_in_samples = (me_ptr->input_frame_size * me_ptr->media_fmt.f.sampling_rate)/1000;
    /* Start the Topology chain */
    result = topo_start(me_ptr->topo_handle,
                        &me_ptr->media_fmt,
                        frame_size_in_samples,
                        NULL);
  }
  else
  {
    /* Re-init modules for new detection session */
    result = topo_set_input_media_type(me_ptr->topo_handle,
                                       &me_ptr->media_fmt,
                                       NULL);
  }
  return result;
}
/**
  Function which process the start command

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_svc_start_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  ADSPResult ack_result = ADSP_EOK;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;
  uint32_t bytes_per_sample = 0;
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t*) (me_ptr->cmd_msg.pPayload);
  uint32_t output_buf_size = 0;
  uint32_t frame_size_in_samples = 0;
  /* Check the state whether it is  already in RUN state or in STOP state.
     If it is already in RUN state return error */
  if(me_ptr->state == LISTEN_PROC_STATE_RUN)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Lpro[%d] already in RUN state , State: %d",
          me_ptr->session_id, me_ptr->state );
    result =  ADSP_EALREADY;
    goto __bailout;
  }

  bytes_per_sample = (me_ptr->bits_per_sample>16)?4:2;
  output_buf_size = (me_ptr->sample_rate * bytes_per_sample * me_ptr->num_channels * me_ptr->input_frame_size)/1000;

  if(NULL != me_ptr->output_buf_ptr)
  {
    if(output_buf_size != me_ptr->output_buf_size)
    {
      qurt_elite_memory_free(me_ptr->output_buf_ptr);
      me_ptr->output_buf_ptr = NULL;
    }
  }
  me_ptr->output_buf_size = output_buf_size;
  if(NULL == me_ptr->output_buf_ptr)
  {
    /* allocate instance struct */
    me_ptr->output_buf_ptr =
        (int8_t*) qurt_elite_memory_malloc(me_ptr->output_buf_size,
                                           QURT_ELITE_HEAP_DEFAULT);
  }
  if (!me_ptr->output_buf_ptr)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Failed to allocate output buffer memoty!");
#endif
    qurt_elite_memory_free(me_ptr->output_buf_ptr);
    me_ptr->output_buf_ptr = NULL;
    result = ADSP_ENORESOURCE;
    goto __bailout;
  }

  /* Start the Topology chain */
  /* Attempt to start modules with default parameters and propogate error -
     Without known Media Format or Media Data, is good to start here so that
     					client can handle errors non related to Media Format */
  frame_size_in_samples = (me_ptr->input_frame_size * me_ptr->media_fmt.f.sampling_rate)/1000;
  result = topo_start(me_ptr->topo_handle,
                      &me_ptr->media_fmt,
                      frame_size_in_samples,
                      NULL);
  /* In case of LEC COPP + SVA use case, ADM will first send MEDIA FMT data
     command with 48Khz sampling rate. me_ptr->media_fmt will be updated 
	 with that sampling rate. Since SVA wont support 48Khz it will return 
	 EUNSUPPORTED. However after MATRIX MAP ROUNTING Command COPP will 
	 send again MEDIA FMT Data command with supported sampling rate.
	 So ignoring the EUNSUPPORTED Error here.*/
  if((ADSP_EOK != result) && (ADSP_EUNSUPPORTED != result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] Fail to start the process for modules, status:: 0x%x",
          me_ptr->session_id, result);
    goto __bailout;
  }

  if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable))
  {
    result = listen_proc_lab_init(lab_struct_ptr,
                         me_ptr->sample_rate,
                         me_ptr->bits_per_sample,
                         me_ptr->kw_end_max_delay_bytes,
                         me_ptr->num_channels,
                         me_ptr->output_buf_size);
    if(ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] fail to Init the LAB, status:: %d",
            me_ptr->session_id, result);
      goto __bailout;
    }
  }

  /* Voting */
  result = lsm_mmpm_config(&me_ptr->mmpm_info,
                           (topo_t*)me_ptr->topo_handle,
                           LSM_MMPM_VOTE_ALL);
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] Failed mmpm voting :: 0x%x",
          me_ptr->session_id, result);
    goto __bailout;
  }

  if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
  {
    result = listen_proc_send_cmd_to_mtmx(me_ptr, ELITEMSG_CUSTOM_MT_MX_RUN);
  }
  else if(LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
  {
    /* Send enable comamnd to AFE */
    result = listen_proc_send_cmd_to_afe(me_ptr,
                                         me_ptr->afe_port_id,
                                         ELITEMSG_CUSTOM_AFECLIENTENABLE,
                                         NULL);
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] Not connected to any client",
          me_ptr->session_id);
    result = ADSP_EFAILED;
    goto __bailout;
  }
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] AFE Client enable/MTMX RUN is failed, status:: 0x%x",
          me_ptr->session_id, result);
    goto __bailout;
  }
  else
  {
    me_ptr->state = LISTEN_PROC_STATE_RUN;
  }

__bailout:
  /* not prinitng fail messages here, as lsm-generate-ack have necessary dbg messages*/
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, NULL, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;
}

/**
  Function which process the stop command

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_svc_stop_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  ADSPResult temp_result = ADSP_EOK, ack_result = ADSP_EOK; 
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t*) (me_ptr->cmd_msg.pPayload);
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;

  /* Check the state whether it is already in STOP state or in RUN state.
     If it is already in STOP state return error */
  if(me_ptr->state == LISTEN_PROC_STATE_STOP)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Lpro[%d] already in STOP state , State: %d",
          me_ptr->session_id, me_ptr->state );
    result =  ADSP_EALREADY;
  }
  else
  {
    /* Send disable client command */
    if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
    {
      result = listen_proc_send_cmd_to_mtmx(me_ptr, 
                                            ELITEMSG_CUSTOM_MT_MX_PAUSE);
    }
    else if(LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
    {
      result = listen_proc_send_cmd_to_afe(me_ptr,
                                           me_ptr->afe_port_id,
                                           ELITEMSG_CUSTOM_AFECLIENTDISABLE,
                                           NULL);
    }
    if(ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] Dis-connect to AFE port/ MTMX PAUSE cmd failed, status:: 0x%x",
            me_ptr->session_id, result);
    }

    // Make the Algo status to None so that VW algo can run once again
    me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;

    /* Flush the buffers */
    listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_INPUT_DATA_Q, ADSP_EOK);

    listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_OUTPUT_DATA_Q, ADSP_EOK);

    if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable))
    {
       if(ADSP_FAILED(result = listen_proc_lab_deinit(me_ptr)))
       {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d] fail to deinit the LAB, status:: %d",
        		me_ptr->session_id, result);
        /* Avoid over-write of initail failure */
        result = ADSP_FAILED(result)?result:temp_result;
        }
      }

    /* Stop topology */
    temp_result = topo_stop(me_ptr->topo_handle,NULL);
    if(ADSP_FAILED(temp_result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] Failed to stop toplogy, status:: 0x%x",
            me_ptr->session_id, temp_result);
      /* Avoid over-write of initail failure */
      result = ADSP_FAILED(result)?result:temp_result;
    }

    /* DeVoting */
    temp_result = lsm_mmpm_config(&me_ptr->mmpm_info,
                                  (topo_t*)me_ptr->topo_handle,
                                  LSM_MMPM_VOTE_NONE);
    if(ADSP_FAILED(temp_result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] Failed kpps devoting :: 0x%x",
            me_ptr->session_id, result);
      /* Avoid over-write of initail failure */
      result = ADSP_FAILED(result)?result:temp_result;
    }

    me_ptr->state =  LISTEN_PROC_STATE_STOP;
  }

  /* not prinitng fail messages here, as lsm-generate-ack have necessary dbg messages*/
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, NULL, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;
}

/**
  Iterate through parameter list and set each parameter

  @param me_ptr [in/out]: This points to the instance of listen_proc_svc_t
  @param payload_address [in/out]: Pointer to list of parameters
  @param payload_size [in]: size of the entire list
  @param opcode [in]: command opcode

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_iter_set_get_param(listen_proc_svc_t *me_ptr,
                                          int8_t *payload_address,
                                          uint32_t payload_size,
                                          bool_t set_flag, uint32_t opcode)
{
  int8_t *param_ptr=NULL;
  uint32_t byte_size_counter =0;
  ADSPResult local_result = ADSP_EOK, aggregated_result = ADSP_EOK;
  uint32_t actual_buf_len = 0;
  
  uint32_t module_id =0, param_id=0, param_size=0, payload_headersize=0;
  uint16_t instance_id=0;
  
  do
  {
    if ((LSM_SESSION_CMD_SET_PARAMS_V3 == opcode) || (LSM_SESSION_CMD_GET_PARAMS_V3 == opcode))
    {
      payload_headersize = sizeof(lsm_session_param_data_v3_t);
    }
    else
    {
      payload_headersize = sizeof(lsm_session_param_data_v2_t);
    }

    param_ptr = (int8_t*)payload_address + payload_headersize;

    /* For every iteration if the setparam is failing with EUNSUPPORTED we are updating the
       param_ptr and moving to the next setparam. In this way there could be possibility that
       setparam has received some junk data and because of this its failing with EUNSUPPORTED
       and param_ptr is updated with unalligned address which leads to crash. To circumvent this the
       below check is added */
    if (IS_NOT_FOUR_BYTE_ALLIGN((uint32_t)param_ptr))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Param ptr not aligned 0x%x", (uint32_t)param_ptr);
      return  ADSP_EBADPARAM;
    }
    
    if ((LSM_SESSION_CMD_SET_PARAMS_V3 == opcode) || (LSM_SESSION_CMD_GET_PARAMS_V3 == opcode))
    {
      lsm_session_param_data_v3_t *param_data_ptr = (lsm_session_param_data_v3_t *)payload_address;
      module_id = param_data_ptr->module_id;
      instance_id = param_data_ptr->instance_id;
      param_size = param_data_ptr->param_size;
      param_id = param_data_ptr->param_id;
    }
    else
    {
      lsm_session_param_data_v2_t *param_data_ptr = (lsm_session_param_data_v2_t *)(payload_address);
      module_id = param_data_ptr->module_id;
      instance_id = 0;
      param_size = param_data_ptr->param_size;
      param_id = param_data_ptr->param_id;
    }
    
    /* Deprecated MODULE_ID - Backward compatibility re-mapping */
    if(LSM_MODULE_VOICE_WAKEUP == module_id)
    {
      module_id = listen_proc_remap_deprecated_module_id(me_ptr, param_id);
    }
    
    switch(module_id)
    {
      /* Framework Aware Module */
      case LSM_MODULE_FRAMEWORK:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          local_result = listen_proc_framework_set_get_params(me_ptr,
                                                              param_id,
                                                              param_size,
                                                              param_ptr,
                                                              set_flag,
                                                              &actual_buf_len);
        }
        else
        {
          local_result = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:LSM_MODULE_FRAMEWORK module Set-get Param rcvd unsupported instance_id:%hu ", instance_id);
        }
        break;
      }
      case LSM_MODULE_LAB:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          local_result = listen_proc_lab_set_get_param(me_ptr,
                                                       param_id,
                                                       param_size,
                                                       param_ptr,
                                                       set_flag,
                                                       &actual_buf_len);
        }
        else
        {
          local_result = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:LSM_MODULE_LAB module Set-get Param rcvd unsupported instance_id:%hu ", instance_id);
        }
        break;
      }
      /* All remaining Modules */
      default:
      {
        if(TRUE == set_flag)
        {
          local_result = topo_set_param(me_ptr->topo_handle,
                                        module_id,
                                        instance_id,
                                        param_id,
                                        param_size,
                                        (int8_t*)param_ptr,
                                        NULL);
        }
        else
        {
          local_result = topo_get_param(me_ptr->topo_handle,
                                        module_id,
                                        instance_id,
                                        param_id,
                                        param_size,
                                        param_ptr,
                                        &actual_buf_len,
                                        NULL);
        }
        break;
      }
    }

    /* Return ADSP_EBADPARAM by wrappers only when param_size is less than the
       required size. Only in this case we are bailing out, for other errors
       we will continue parsing */
    if(ADSP_EBADPARAM == local_result)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Lpro[%d] SetParam bailing out !!!",
                me_ptr->session_id);
      aggregated_result = ADSP_EBADPARAM;
      break; /*  in case of any error don't go forward with parsing */
    }
    else if(ADSP_FAILED(local_result))
    {
      aggregated_result = ADSP_EFAILED;
    }
    if(FALSE == set_flag)
    {
      /* Get Param - only one param */
      if (LSM_SESSION_CMD_GET_PARAMS_V3 == opcode)
      {
        lsm_session_param_data_v3_t *param_data_ptr = (lsm_session_param_data_v3_t *)(payload_address);
        param_data_ptr->param_size = actual_buf_len;
      }
      else
      {
        lsm_session_param_data_v2_t *param_data_ptr = (lsm_session_param_data_v2_t *)(payload_address);
        param_data_ptr->param_size = actual_buf_len;
      }
      break;
    }
    byte_size_counter += (payload_headersize +  param_size);
    payload_address += (payload_headersize + param_size);

  }while(byte_size_counter < payload_size);

  return aggregated_result;
}

/*
   This function processes  LSM_SESSION_CMD_SET_PARAM_V2/V3 command that is sent to a given session.


   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */

ADSPResult listen_proc_svc_set_param_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK,ack_result = ADSP_EOK;
  elite_apr_packet_t *apr_pkt_ptr  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  uint8_t session_id   = me_ptr->session_id;
  int8_t *calib_data_ptr;
  uint32_t opcode = 0;
  elite_mem_shared_memory_map_t bufferMemNode;  
  uint32_t mem_map_handle, cmd_headersize, addr_lsw, addr_msw, param_payload_size;

  opcode = elite_apr_if_get_opcode(apr_pkt_ptr);
  
  if(LSM_SESSION_CMD_SET_PARAMS_V3 == opcode)
  {
    //Get APR payload
    lsm_session_cmd_set_params_v3_t *param_payload_ptr = (lsm_session_cmd_set_params_v3_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    cmd_headersize = sizeof(lsm_session_cmd_set_params_v3_t);
    mem_map_handle = param_payload_ptr->mem_map_handle;
    addr_lsw = param_payload_ptr->data_payload_addr_lsw;
    addr_msw = param_payload_ptr->data_payload_addr_msw;
    param_payload_size = param_payload_ptr->data_payload_size;
  }
  else
  {
    //Get APR payload
    lsm_session_cmd_set_params_v2_t *param_payload_ptr = (lsm_session_cmd_set_params_v2_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    cmd_headersize = sizeof(lsm_session_cmd_set_params_v2_t);
    mem_map_handle = param_payload_ptr->mem_map_handle;
    addr_lsw = param_payload_ptr->data_payload_addr_lsw;
    addr_msw = param_payload_ptr->data_payload_addr_msw;
    param_payload_size = param_payload_ptr->data_payload_size;
  }

  if(0 == mem_map_handle) /* in-band */
  {
      /* Address just beyond the Set params header points to lsm_session_param_data_v2_t/lsm_session_param_data_v3_t */
      calib_data_ptr  = ((int8_t*)elite_apr_if_get_payload_ptr(apr_pkt_ptr) + cmd_headersize);

  } /* in-band */
  else /* Out-band */ 
  {
    if (!check_cache_line_alignment(addr_lsw))
    {
      result = ADSP_EBADPARAM;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem unaligned",
                session_id, result);

      goto __bailout;
    }

    bufferMemNode.unMemMapClient     = lsm_memory_map_client;
    bufferMemNode.unMemMapHandle     = mem_map_handle;
    bufferMemNode.unMemSize          = param_payload_size;
    bufferMemNode.unPhysAddrLsw      = addr_lsw;
    bufferMemNode.unPhysAddrMsw      = addr_msw;
    bufferMemNode.unVirtAddr         = 0;

    result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw,
                                           bufferMemNode.unPhysAddrMsw,
                                           bufferMemNode.unMemSize,
                                           &bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem map:0x%x",
                session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }

    /* Since this buffer will be read, need to invalidate the cache. */
    result = elite_mem_invalidate_cache(&bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem invalidate cache:0x%x",
                session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }
    calib_data_ptr  = (int8_t*)(bufferMemNode.unVirtAddr);

  }/* Out-band */ 

  /* Execute set param */
  result = listen_proc_iter_set_get_param(me_ptr,calib_data_ptr, param_payload_size, TRUE, opcode);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "LSM[%u]:iter params:0x%x", result);
  }

__bailout:
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, 0, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;

}

/*
   This function processes  LSM_SESSION_CMD_GET_PARAM_V2/V3 command that is sent to a given session.


   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */

ADSPResult listen_proc_svc_get_param_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result=ADSP_EOK,ack_result = ADSP_EOK;
  elite_apr_packet_t *apr_pkt_ptr  = (elite_apr_packet_t*) ((me_ptr->cmd_msg).pPayload);
  uint8_t session_id   = me_ptr->session_id;
  int8_t *calib_data_ptr;
  uint32_t opcode = 0, respOpCode =0;
  elite_mem_shared_memory_map_t bufferMemNode;
  elite_apr_packet_t *temp_apr_pkt_ptr = NULL;
  uint32_t ack_payload_size = 0;
  int8_t* apr_packet_base_ptr = NULL;
  
  uint32_t mem_map_handle, addr_lsw, addr_msw, param_max_size, resp_payload_size;
  uint32_t module_id=0, param_id=0;
  uint16_t instance_id=0;

  opcode = elite_apr_if_get_opcode(apr_pkt_ptr);
  
  if(LSM_SESSION_CMD_GET_PARAMS_V3 == opcode)
  {
    //Get APR payload
    lsm_session_cmd_get_params_v3_t *param_payload_ptr = (lsm_session_cmd_get_params_v3_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    mem_map_handle = param_payload_ptr->mem_map_handle;
    addr_lsw = param_payload_ptr->data_payload_addr_lsw;
    addr_msw = param_payload_ptr->data_payload_addr_msw;
    param_max_size = param_payload_ptr->param_max_size;
    module_id = param_payload_ptr->module_id;
    instance_id = param_payload_ptr->instance_id;
    param_id = param_payload_ptr->param_id;
    resp_payload_size = sizeof(lsm_session_cmdrsp_get_params_v3_t);
    respOpCode = LSM_SESSION_CMDRSP_GET_PARAMS_V3;
  }
  else
  {
    //Get APR payload
    lsm_session_cmd_get_params_v2_t *param_payload_ptr = (lsm_session_cmd_get_params_v2_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    mem_map_handle = param_payload_ptr->mem_map_handle;
    addr_lsw = param_payload_ptr->data_payload_addr_lsw;
    addr_msw = param_payload_ptr->data_payload_addr_msw;
    param_max_size = param_payload_ptr->param_max_size;
    module_id = param_payload_ptr->module_id;
    param_id = param_payload_ptr->param_id;
    resp_payload_size = sizeof(lsm_session_cmdrsp_get_params_v2_t);
    respOpCode = LSM_SESSION_CMDRSP_GET_PARAMS_V2;
  }
  
  if(0 == param_max_size)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM [%u]: param_max_size=0 for get_param",session_id);
    goto __bailout;
  }

  if(0 == mem_map_handle) /* in-band */
  {
    
    /* This service needs to allocate large enough packet for the response*/
    ack_payload_size = resp_payload_size + param_max_size;

    /*Allocate the APR packet*/
    result = elite_apr_if_alloc_cmd_rsp(lsm_get_apr_handle(),
                                        elite_apr_if_get_dst_addr(apr_pkt_ptr),
                                        elite_apr_if_get_dst_port(apr_pkt_ptr),
                                        elite_apr_if_get_src_addr(apr_pkt_ptr),
                                        elite_apr_if_get_src_port(apr_pkt_ptr),
                                        elite_apr_if_get_client_token(apr_pkt_ptr),
                                        respOpCode,
                                        ack_payload_size,
                                        &temp_apr_pkt_ptr);

    if(NULL == temp_apr_pkt_ptr)
    {
      result = ADSP_EFAILED;
    }

    if(ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:Alloc rsp GetParam:0x%x",
                session_id, result);
      goto __bailout;
    }

    /*assign the current message to the allocate response packet*/
    me_ptr->cmd_msg.pPayload = temp_apr_pkt_ptr;
    apr_packet_base_ptr = (int8_t *)elite_apr_if_get_payload_ptr(temp_apr_pkt_ptr);
    /*assign the param data ptr of the internal message to the correct offset in APR packet*/
    calib_data_ptr = apr_packet_base_ptr + sizeof(uint32_t); /*Offset by the status to access the module ID.*/
    /*Free up the incoming APR packet for in-band since we have created another for the response*/
    elite_apr_if_free(lsm_get_apr_handle(), apr_pkt_ptr);
    apr_pkt_ptr = temp_apr_pkt_ptr;

  } /* in-band */
  else /* Out-band */ /* SET PARAM  and GET PARAM */
  {
    if (!check_cache_line_alignment(addr_lsw))
    {
      result = ADSP_EBADPARAM;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem unaligned",
                session_id, result);

      goto __bailout;
    }

    bufferMemNode.unMemMapClient     = lsm_memory_map_client;
    bufferMemNode.unMemMapHandle     = mem_map_handle;
    bufferMemNode.unMemSize          = param_max_size;
    bufferMemNode.unPhysAddrLsw      = addr_lsw;
    bufferMemNode.unPhysAddrMsw      = addr_msw;
    bufferMemNode.unVirtAddr         = 0;

    result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw,
                                           bufferMemNode.unPhysAddrMsw,
                                           bufferMemNode.unMemSize,
                                           &bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem map:0x%x",
                session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }

    /* Since this buffer will be read, need to invalidate the cache. */
    result = elite_mem_invalidate_cache(&bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]:mem invalidate cache:0x%x",
                session_id, result);
      result = ADSP_EBADPARAM;
      goto __bailout;
    }
    calib_data_ptr  = (int8_t*)(bufferMemNode.unVirtAddr);

  }/* Out-band */ 

  /*initialize the response payload to 0 for consistency*/
  memset(calib_data_ptr, 0, param_max_size);

  /*populate the param data header*/
  if(LSM_SESSION_CMD_GET_PARAMS_V3 == opcode)
  {
    lsm_session_param_data_v3_t *param_header_ptr = (lsm_session_param_data_v3_t *)calib_data_ptr;
    param_header_ptr->module_id = module_id;
    param_header_ptr->instance_id = instance_id;
    param_header_ptr->param_id = param_id;
    /*initalize this to the max size for the actual payload*/
    param_header_ptr->param_size = param_max_size - sizeof(lsm_session_param_data_v3_t);
  }
  else
  {
    lsm_session_param_data_v2_t *param_header_ptr = (lsm_session_param_data_v2_t *)calib_data_ptr;
    param_header_ptr->module_id = module_id;
    param_header_ptr->param_id = param_id;
    /*initalize this to the max size for the actual payload*/
    param_header_ptr->param_size = param_max_size - sizeof(lsm_session_param_data_v2_t);
  }
  
  /* Execute get param */
  result = listen_proc_iter_set_get_param(me_ptr,calib_data_ptr, param_max_size, FALSE, opcode);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "LSM[%u]:iter params:0x%x", result);
  }

  /* Get param response handling */

  /*Generate ACK. For in-band, apr_packet_ptr is created above */
  if((LSM_SESSION_CMDRSP_GET_PARAMS_V3 == respOpCode) && (0 == mem_map_handle))
  {
    lsm_session_cmdrsp_get_params_v3_t *ret_payload_ptr =
                    (lsm_session_cmdrsp_get_params_v3_t*)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    ret_payload_ptr->status = result;
  }
  else if((LSM_SESSION_CMDRSP_GET_PARAMS_V2 == respOpCode) && (0 == mem_map_handle))
  {
    lsm_session_cmdrsp_get_params_v2_t *ret_payload_ptr =
                    (lsm_session_cmdrsp_get_params_v2_t*)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    ret_payload_ptr->status = result;
  }
  /*For out-of-band apr_packet_ptr is created by client*/
  else
  {
    /*Flush the cache*/
    result = elite_mem_flush_cache(&bufferMemNode);

    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM[%u]: Out-band Getparam flush cache fail",
                session_id );

    }
    /*send the ACK*/
    ack_result = lsm_generate_ack(apr_pkt_ptr, result,&result, sizeof(result), 0 );
    result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
    return result;
  }

__bailout:
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, 0, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;  
}

ADSPResult listen_proc_disconnect_from_afe_cmd_sequece(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  /*send disconnect cmd to already connected AFE port id.
    First send Disable client command
    Second flush input buffer
    Third send disconnect command to AFE */

  /* send disable client command */
  result = listen_proc_send_cmd_to_afe(me_ptr,
                                       me_ptr->afe_port_id,
                                       ELITEMSG_CUSTOM_AFECLIENTDISABLE,
                                       NULL);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Client disable is failed, status:: %d",
          result);
    return result;
#endif
  }

  /* flush the buffers */
  listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_INPUT_DATA_Q, ADSP_EOK);

  /* send disconnect client command */
  result = listen_proc_send_cmd_to_afe(me_ptr,
                                       me_ptr->afe_port_id,
                                       ELITEMSG_CUSTOM_AFEDISCONNECT_REQ,
                                       NULL);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Client disconnect  is failed, status::0x%x",
          result);
    return result;
#endif
  }
  me_ptr->lsm_client_connection = LSM_CONNECTED_TO_NO_CLIENT;
  me_ptr->afe_port_id = AFE_PORT_ID_INVALID;

  return result;
}



ADSPResult listen_proc_disconnect_from_adm_cmd_sequece(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  /*send disconnect cmd to ADM.
    First send Disable client command
    Second flush input buffer
    Third send disconnect command to ADM */

  /* send disable client command */
  result = listen_proc_send_cmd_to_adm(me_ptr, ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISABLE);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Client disable is failed, status:: %d",
          result);
    return result;
#endif
  }

  /* flush the buffers */
  listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_INPUT_DATA_Q, ADSP_EOK);

  /* send disconnect client command */
  result = listen_proc_send_cmd_to_adm(me_ptr, ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Client disconnect is failed, status::0x%x",
          result);
    return result;
#endif
  }
  return result;
}
/**
  LSM_PARAM_ID_CONNECT_TO_PORT implementation

  @param me_ptr [in/out]: This points to the instance of listen_proc_svc_t
  @param param_size [in]: size of parameter buffer
  @param params_buffer_ptr [in/out]: Pointer to buffer containing the parameters

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_connect_to_port(listen_proc_svc_t *me_ptr,
                                       uint32_t param_size,
                                       int8_t *params_buffer_ptr)
{
  ADSPResult result = ADSP_EOK;
  uint16_t afe_port_id=0;
  lsm_param_id_connect_port_t *connect_to_port_param_ptr = NULL;
  /* Choose the appropriate config version  */
  uint32_t connect_port_minor_version =  *((uint32_t *)params_buffer_ptr);

  if(connect_port_minor_version > LSM_API_VERSION_CONNECT_TO_PORT)
  {
    connect_port_minor_version = LSM_API_VERSION_CONNECT_TO_PORT;
  }

  if(0x1 == connect_port_minor_version)
  {
    if(param_size < sizeof(lsm_param_id_connect_port_t))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Set PARAM_CONNECT_TO_PORT: "
            "Invalid payload size: %lu",
            param_size);
      return ADSP_EBADPARAM;
    }

    connect_to_port_param_ptr = (lsm_param_id_connect_port_t *)params_buffer_ptr;

    afe_port_id  =  connect_to_port_param_ptr->port_id;

    /* Check port id to see it is AFE or ADM connection */
    /* Enter into this condition if it is ADM port */
    if (PORT_ID_FOR_CONNECTING_TO_ADM == afe_port_id) 
    { 
      /*check if is already connected to ADM , if so just 
          return the success*/
      if(LSM_CONNECTED_TO_ADM== me_ptr->lsm_client_connection)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "LSM already connected to ADM, ignoring this command",
              result);
        return ADSP_EALREADY;
      }
      /*Now check if this LSM session already connected to AFE. if it 
          is connected, disconnect LSM from AFE*/
      if (LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
      {
        if(ADSP_FAILED(result = listen_proc_disconnect_from_afe_cmd_sequece(me_ptr)))
        {
          return result;
        }
      }
      /* Send Connect command to ADM */
      result = listen_proc_send_cmd_to_adm(me_ptr,ELITEMSG_CUSTOM_ADM_LSM_STREAM_CONNECT);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Connect to ADM  is failed, status:: %d",
              result);
        return result;
      }
      /* After ADM connect command success, send MTMX Reconfig command */
      result = listen_proc_send_cmd_to_adm(me_ptr,ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG);
      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "MtMX Reconfig command  is failed, status:: %d",
              result);
        /* If reconfig is failed, send disconnect command */
        /* Failure occurs only if channle mapping info is wrong */
        if(ADSP_FAILED(result = listen_proc_send_cmd_to_adm(me_ptr,ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT)))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Connect to ADM  is failed, status:: %d",
                result);
        }
        return result;
      }
    } 
    /*ADM_PORT_ID == afe_port_id)*/
    else
    {
      /* check if its already connected to ADM */
      /*if it is connected, disconnect from ADM, before connecting to AFE*/
      if(LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
      {
        if(ADSP_FAILED(listen_proc_disconnect_from_adm_cmd_sequece(me_ptr)))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Disconnect from ADM  is failed, status:: %d",
                result);
          return result;
        }
      }
      /* check whether the dynamic svc is connected to  AFE  or not. */
      if(me_ptr->afe_port_id == AFE_PORT_ID_INVALID)
      {
        /* send connect command to AFE */
        result = listen_proc_send_cmd_to_afe(me_ptr,
                                             afe_port_id,
                                             ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                             NULL);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Connect to AFE port is failed, status:: %d",
                result);
          return result;
        }
      }
      /* if it is connected check whether client send same port id or
       different port id. If it is different port id, first disconnect
       existing connection and send connect command to new port */
      else if(afe_port_id != me_ptr->afe_port_id)
      {
        if(ADSP_FAILED(result = listen_proc_disconnect_from_afe_cmd_sequece(me_ptr)))
        {
          return result;
        }

        /* Now send connect cmd to new AFE port id */
        result = listen_proc_send_cmd_to_afe(me_ptr,
                                             afe_port_id,
                                             ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                             NULL);
        if(ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Connect to AFE port is failed, status::0x%x",
                result);
          return result;
        }
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "This port is already connected to Listen Dyn Svc");
        return ADSP_EOK;
      }
    }
    MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,
          "LSM_PARAM_ID_CONNECT_TO_PORT succesfully processed, status::0x%x",
          result);
  }
  return result;
}

/**
  LSM_PARAM_ID_FWK_MODE_CONFIG implementation

  @param me_ptr [in/out]: This points to the instance of listen_proc_svc_t
  @param param_size [in]: size of parameter buffer
  @param params_buffer_ptr [in/out]: Pointer to buffer containing the parameters

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_set_fwk_mode(listen_proc_svc_t *me_ptr,
                                    uint32_t param_size,
                                    int8_t *params_buffer_ptr)
{
  ADSPResult result = ADSP_EOK;

  lsm_param_id_fwk_mode_config_t *mode_param_ptr = NULL;
  /* Choose the appropriate config version  */
  uint32_t mode_minor_version =  *((uint32_t *)params_buffer_ptr);

  if(mode_minor_version > LSM_API_VERSION_FWK_MODE_CONFIG)
  {
    mode_minor_version = LSM_API_VERSION_FWK_MODE_CONFIG;
  }

  if(0x1 == mode_minor_version)
  {
    if(param_size < sizeof(lsm_param_id_fwk_mode_config_t))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Set PARAM_FWK MODE CONFIG: "
            "Invalid payload size: %lu",
            param_size);
      return ADSP_EBADPARAM;
    }

    mode_param_ptr = (lsm_param_id_fwk_mode_config_t *)params_buffer_ptr;

    me_ptr->mode =  mode_param_ptr->mode;
  }

  return result;
}

/**
  LSM_MODULE_FRAMEWORK implementation, LSM_MODULE_FRAMEWORK is a reserved
  module id to sent Parameters to Framework. A Parameter is taken as input and
  based on param id, the appropriate task is called.

  @param me_ptr [in/out]: This points to the instance of listen_proc_svc_t
  @param param_id [in]: Param id of the Parameter passed below
  @param param_size [in]: size of parameter buffer
  @param params_buffer_ptr [in/out]: Pointer to buffer containing the parameters
  @param set_flag [in]: True=set param, False=get param
  @param actual_buf_len [in]: actual size of buffer filled (only for get param)

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult listen_proc_framework_set_get_params(listen_proc_svc_t *me_ptr,
                                                uint32_t param_id,
                                                uint32_t param_size,
                                                int8_t *params_buffer_ptr,
                                                bool_t set_flag,
                                                uint32_t* actual_buf_len)
{
  ADSPResult result = ADSP_EOK;
  switch(param_id)
  {
    case LSM_PARAM_ID_CONNECT_TO_PORT:
    {
      lsm_param_id_connect_port_t *payload_ptr = NULL;
      if(TRUE == set_flag) /* Set Param */
      {
        listen_proc_connect_to_port(me_ptr, param_size, params_buffer_ptr);
      }
      else /* Get Param */
      {
        payload_ptr = (lsm_param_id_connect_port_t *)params_buffer_ptr;
        if(param_size < sizeof(lsm_param_id_connect_port_t))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Getparam size:%lu",
                param_size);
          return ADSP_EBADPARAM;
        }
        payload_ptr->minor_version = LSM_API_VERSION_CONNECT_TO_PORT;
        payload_ptr->port_id =  me_ptr->afe_port_id;
        payload_ptr->reserved = 0;
        *actual_buf_len = sizeof(lsm_param_id_connect_port_t);
      }
      break;
    }
    case LSM_PARAM_ID_FWK_MODE_CONFIG:
    {
      if(TRUE == set_flag) /* Set Param */
      {
        listen_proc_set_fwk_mode(me_ptr, param_size, params_buffer_ptr);
      }
      else /* Get param*/
      {
        lsm_param_id_fwk_mode_config_t *payload_ptr = (lsm_param_id_fwk_mode_config_t *)params_buffer_ptr;
        if(param_size < sizeof(lsm_param_id_fwk_mode_config_t))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Getparam size:%lu",
                param_size);
          return ADSP_EBADPARAM;
        }
        payload_ptr->minor_version = LSM_API_VERSION_FWK_MODE_CONFIG;
        payload_ptr->mode          =  me_ptr->mode;
      }
      break;
    }
    case LSM_PARAM_ID_MEDIA_FMT:
    {
      lsm_param_id_media_fmt_v2_t *payload_ptr = (lsm_param_id_media_fmt_v2_t *)params_buffer_ptr;
      uint8_t i = 0;
      if(TRUE == set_flag) /* Set Param */
      {
        if(LISTEN_PROC_STATE_RUN == me_ptr->state)
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Set PARAM_MEDIA FMT is not supported while session is in RUN state");

          return ADSP_EUNSUPPORTED;
        }
        /* Choose the appropriate config version  */
        uint32_t media_fmt_minor_version =  *((uint32_t *)params_buffer_ptr);
        if(media_fmt_minor_version > LSM_API_VERSION_MEDIA_FMT)
        {
          media_fmt_minor_version = LSM_API_VERSION_MEDIA_FMT;
        }

        if(0x2 == media_fmt_minor_version)
        {
          if(param_size < sizeof(lsm_param_id_media_fmt_v2_t))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Set PARAM_MEDIA FMT: "
                  "Invalid payload size: %lu",
                  param_size);
            return ADSP_EBADPARAM;
          }
          topo_send_output_media_fmt_t pParams;
          bool_t port_reconfig = FALSE;
          memset(&pParams,0,sizeof(pParams));
          if(((me_ptr->bits_per_sample!= payload_ptr->bit_width) || (me_ptr->num_channels != payload_ptr->num_channels) ||
              (me_ptr->sample_rate != payload_ptr->sampling_rate)) && (me_ptr->lsm_client_connection != LSM_CONNECTED_TO_NO_CLIENT))
          {
            port_reconfig = TRUE;
          }
          if((16 != payload_ptr->bit_width) &&
            (24 != payload_ptr->bit_width))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM received unsupported bit_per_sample = %lu; %lu, %lu are  supported",
                 payload_ptr->bit_width);
            return CAPI_V2_EBADPARAM;
          }
          if((8000 != payload_ptr->sampling_rate) &&
            (16000 != payload_ptr->sampling_rate) &&
            (32000 != payload_ptr->sampling_rate) &&
            (48000 != payload_ptr->sampling_rate))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM received unsupported sampling rate = %lu",
                 payload_ptr->sampling_rate);
            return CAPI_V2_EBADPARAM;
          }
          if(LSM_MAX_NUM_CHANNELS < payload_ptr->num_channels )
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM received unsupported number of channels = %lu",
                 payload_ptr->num_channels);
            return CAPI_V2_EBADPARAM;
          }
          me_ptr->bits_per_sample = payload_ptr->bit_width;
          me_ptr->num_channels = payload_ptr->num_channels;
          me_ptr->sample_rate = payload_ptr->sampling_rate;

          pParams.bits_per_sample = me_ptr->bits_per_sample;
          pParams.num_channels = me_ptr->num_channels;
          pParams.sampling_rate = me_ptr->sample_rate;
          me_ptr->media_fmt.f.bits_per_sample = (me_ptr->bits_per_sample == 16) ? 16 : 32;
          me_ptr->media_fmt.f.num_channels = me_ptr->num_channels;
          me_ptr->media_fmt.f.sampling_rate= me_ptr->sample_rate;

          for(i=0;i<LSM_MAX_NUM_CHANNELS;i++)
          {
            me_ptr->media_fmt.f.channel_type[i] = (uint16_t)payload_ptr->channel_mapping[i];
            me_ptr->channel_mapping[i] = payload_ptr->channel_mapping[i];
          }
          for(i=0;i<LSM_MAX_NUM_CHANNELS;i++)
          {
            pParams.channel_mapping[i] = payload_ptr->channel_mapping[i];
          }
          result = topo_send_capiv2_output_media_format(me_ptr->topo_handle, &pParams);
          if(TRUE == port_reconfig)
          {
            listen_proc_reconnect_to_port(me_ptr, me_ptr->afe_port_id);
          }
          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Input Media Fmt to LSM with sampling rate :%lu, bit width : %lu, num_ channels : %lu",
                me_ptr->sample_rate, pParams.bits_per_sample, pParams.num_channels);
          MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Input Media Fmt to LSM with channel_mapping : %d, %d, %d, %d, %d, %d, %d, %d",
                me_ptr->channel_mapping[0], me_ptr->channel_mapping[1], me_ptr->channel_mapping[2], me_ptr->channel_mapping[3],
                me_ptr->channel_mapping[4], me_ptr->channel_mapping[5], me_ptr->channel_mapping[6], me_ptr->channel_mapping[7]);
        }
      }
      else /* Get Param */
      {
        payload_ptr = (lsm_param_id_media_fmt_v2_t *)params_buffer_ptr;
        if(param_size < sizeof(lsm_param_id_media_fmt_v2_t))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Getparam size:%lu",
                param_size);
          return ADSP_EBADPARAM;
        }
        payload_ptr->minor_version = LSM_API_VERSION_MEDIA_FMT;
        payload_ptr->bit_width =  me_ptr->bits_per_sample;
        payload_ptr->num_channels = me_ptr->num_channels;
        payload_ptr->sampling_rate = me_ptr->sample_rate;
        for(i=0;i<LSM_MAX_NUM_CHANNELS;i++)
        {
          payload_ptr->channel_mapping[i] = me_ptr->channel_mapping[i];
        }
        *actual_buf_len = sizeof(lsm_param_id_media_fmt_v2_t);
      }
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Unknown Param id :0x%lx",
            param_id);
      result = ADSP_EUNSUPPORTED;
      break;
    }
  }

  return result;
}

/**
  Function which sends commands to AFE

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @param afe_port_id [in]  afe port id to which cmd needs to be send
  @param cmd [in]   type of command send to AFe
  @param params [in]   Arguments (if any) to populate the cmd msg

  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_send_cmd_to_afe(listen_proc_svc_t *me_ptr,
                                       uint16_t afe_port_id,
                                       uint32_t cmd,
                                       void* params)
{
  ADSPResult                     result               = ADSP_EOK;
  elite_msg_any_t                afe_msg;
  int                            nActualSize          = 0;
  elite_msg_custom_afe_connect_t *afe_connect_msg_ptr = NULL;
  qurt_elite_queue_t             *afe_cmd_q           = qurt_elite_globalstate.pAfeStatSvcCmdQ;
  qurt_elite_bufmgr_node_t       bufMgrNode;
  elite_msg_custom_afe_client_criteria_register_t *afe_register_msg_ptr = NULL;
  lsm_send_afe_reg_criteria_t *param_ptr = NULL;
  const uint32_t buffer_size = MAX(sizeof(elite_msg_custom_afe_connect_t),
                                   sizeof(elite_msg_custom_afe_client_criteria_register_t));

  result = elite_mem_get_buffer(buffer_size, &bufMgrNode, &nActualSize);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to create mesage result=0x%x",
          result);
    return result;
  }

  /*  Needed for all msgs */
  afe_connect_msg_ptr                     = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;
  afe_connect_msg_ptr->sec_op_code        = cmd;
  afe_connect_msg_ptr->responseq_ptr      = me_ptr->response_q_ptr;
  afe_connect_msg_ptr->client_token       = 0;
  afe_connect_msg_ptr->buffer_returnq_ptr = bufMgrNode.pReturnQ;
  afe_connect_msg_ptr->afe_id             = afe_port_id;
  afe_connect_msg_ptr->svc_handle_ptr     = me_ptr->afe_svc_handle;

  afe_msg.pPayload = (void*)bufMgrNode.pBuffer;
  afe_msg.unOpCode = ELITE_CUSTOM_MSG;

  memset(&afe_connect_msg_ptr->afe_client, 0, sizeof(afe_client_t));

  switch(cmd)
  {
    case ELITEMSG_CUSTOM_AFECONNECT_REQ:
    case ELITEMSG_CUSTOM_AFEDISCONNECT_REQ:
    case ELITEMSG_CUSTOM_AFECLIENTENABLE:
    case ELITEMSG_CUSTOM_AFECLIENTDISABLE:
#ifndef DISABLE_DC_RESET  //<-- Need to remove once Algo update is available
    case ELITEMSG_CUSTOM_AFECLIENT_DC_RESET:
#endif
    {
      afe_connect_msg_ptr                     = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;
      
      afe_connect_msg_ptr->afe_client.sample_rate            = me_ptr->sample_rate;
      afe_connect_msg_ptr->afe_client.data_path              = AFE_TX_PP_AUDIO_SENSE_OUT;
      afe_connect_msg_ptr->afe_client.cust_proc.audio_sense.type = AFE_MAD_AUDIO;
      afe_connect_msg_ptr->afe_client.cust_proc.subscribe_to_avt_drift = FALSE;
      afe_connect_msg_ptr->afe_client.channel                = me_ptr->num_channels;
      afe_connect_msg_ptr->afe_client.buf_size               = me_ptr->sample_rate/LISTEN_INPUT_AFE_INTERNAL_FRAME_PER_SEC;
      afe_connect_msg_ptr->afe_client.num_buffers            = NUM_DATA_BUFFERS;
      afe_connect_msg_ptr->afe_client.bytes_per_channel      = (me_ptr->bits_per_sample > 16 ? 4 : 2);
      afe_connect_msg_ptr->afe_client.bit_width              = me_ptr->bits_per_sample;
      afe_connect_msg_ptr->afe_client.interleaved            = INTERLEAVED_DATA;  /*  0 always */
      afe_connect_msg_ptr->afe_client.afe_client_handle      = &me_ptr->svc_handle;
      afe_connect_msg_ptr->afe_drift_ptr                     = NULL;
      afe_connect_msg_ptr->afe_client.afe_dp_delay_ptr       = NULL;
      break;
    }
    case ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER:
    {
      afe_register_msg_ptr = (elite_msg_custom_afe_client_criteria_register_t*) bufMgrNode.pBuffer;
      param_ptr = (lsm_send_afe_reg_criteria_t*)params;

      afe_register_msg_ptr->criteria           = param_ptr->criteria;
      afe_register_msg_ptr->criteria_enable    = param_ptr->criteria_enable;
      break;
    }
   default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to send command to AFE, unexpected cmd: %lu",
            cmd);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  /* push message into AFE cmd Q */
  result = qurt_elite_queue_push_back(afe_cmd_q,
                                      (uint64_t* )&afe_msg);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AFEConnectCmd Failed to send custom AFE connect msg");
    goto __bailout;
  }

  /* wait for connect Ack from AFE svc */
  result = elite_svc_wait_for_ack(&afe_msg);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Wait for AFEConnectCmd ack failed");
    goto __bailout;
  }

  /* check response result */
  if(ADSP_EOK != afe_connect_msg_ptr->response_result)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in AFE connect message Ack");
    result = afe_connect_msg_ptr->response_result;
    goto __bailout;
  }

  /*  This is updated only for connect command */
  if(cmd == ELITEMSG_CUSTOM_AFECONNECT_REQ)
  {
    me_ptr->afe_port_id    = afe_port_id;
    me_ptr->afe_svc_handle = afe_connect_msg_ptr->svc_handle_ptr;
    me_ptr->lsm_client_connection = LSM_CONNECTED_TO_AFE;
    me_ptr->input_frame_size  = AFE_OUT_FRAME_SIZE_10_MSEC;
  }

__bailout:
  if (cmd == ELITEMSG_CUSTOM_AFEDISCONNECT_REQ)
  {
    me_ptr->lsm_client_connection = LSM_CONNECTED_TO_NO_CLIENT;
  }

  /* release AFE custom connect message */
  elite_msg_release_msg(&afe_msg);

  return result;
}

/*
  This function do the actual processing on data

  @param [in/out] topo_ptr : pointer to topology handler
  @param [in] inp_buf_size : size of input buffer
  @param [in/out] inp_buf_ptr : pointer to input data buffer
  @param [in] timestamp : TimeStamp populated from upstream
  @param [in] is_timestamp_valid : TimeStamp validity field populated from upstream

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_process(listen_proc_svc_t *me_ptr,
                                   uint32_t inp_buf_size,
                                   int8_t *inp_buf_ptr,
                                   uint64_t timestamp,
                                   bool_t is_timestamp_valid)
{
  ADSPResult result = ADSP_EOK;
  topo_t *topo_ptr = (topo_t *)me_ptr->topo_handle;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;
  if(inp_buf_size != topo_ptr->topo_input_buf_size * me_ptr->num_channels)
  {
#ifdef LSM_DEBUG
    /* Today, topo_process doesn't take variable buffer sizes during process.
       Buffer size are fixed during topo_start and should be available during
       topo_process */
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] Unexpected buffer size, needed[%lu], got[%lu]",
          me_ptr->session_id, topo_ptr->topo_input_buf_size, inp_buf_size);
#endif
    return ADSP_ENEEDMORE;
  }
  listen_proc_svc_log_input_data(me_ptr,
                                 inp_buf_ptr,
                                 inp_buf_size);
  /* Call the algortihm now to process data */
  result = topo_process(me_ptr->topo_handle,inp_buf_ptr,me_ptr->output_buf_ptr,NULL,
                        timestamp, is_timestamp_valid);
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d] process failed, result[%x]",
          me_ptr->session_id, result);
  }

  if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable) && (TRUE == lab_struct_ptr->start_lab))
  {
    if(ADSP_FAILED(result = listen_proc_lab_start(me_ptr)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] listen_proc_lab_start failed with status ::0x%x",
            me_ptr->session_id, result);
      /* Flushing the buffers if any already pushed into the output queue.*/
      listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_OUTPUT_DATA_Q, result);
      /* If LSM connceted to ADM, we are not supporting DC Reset command*/
      /*So reiniting the algo, through media type*/
      if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
      {
        if(ADSP_FAILED(listen_proc_svc_reinit(me_ptr, FALSE)))
        {
          MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
              "Error in Lpro reinit");
        }
        // Make the Algo status to None so that VW algo can run once again
        me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;
      }
      else if (LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
      {
#ifndef DISABLE_DC_RESET  //<-- Need to remove once Algo update is available
        /* send Duty cycle reset command to AFE */
        result = listen_proc_send_cmd_to_afe(me_ptr,
                                             me_ptr->afe_port_id,
                                             ELITEMSG_CUSTOM_AFECLIENT_DC_RESET,
                                             NULL);
#endif
      }
      return result;
    }
  }

  /* Buffer the data right from the beginning of first frame, when LAB is enabled.
     For LAB feature (kwd history buffer disabled) this is extra MPPS  , but as it is 
     simple memcpy its ok to enable  copy  from first frame.  
     This helps to remove DISCOVERY state from fwk */

  if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable))
  {
    listen_proc_lab_process(me_ptr);
    if(LSM_DETECTION_STATUS_DETECTED == me_ptr->detection_status)
    {
      listen_proc_svc_process_output_data_q(me_ptr);
      return ADSP_EOK;
    }
  }

#ifndef DISABLE_DC_RESET  //<-- Need to remove once Algo update is available
  if((me_ptr->detection_status == LSM_DETECTION_STATUS_DETECTED) ||
      (me_ptr->detection_status == LSM_DETECTION_STATUS_REJECTED)   )
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Lpro[%d] status by Detection Engine ::0x%x",
          me_ptr->session_id, me_ptr->detection_status);
    /* If LSm connceted to ADm, we are not supproting DC Reset command*/
    /*So reiniting the algo, through media type*/
     if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
    {
      if(ADSP_FAILED(listen_proc_svc_reinit(me_ptr, FALSE)))
      {
        MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
            "Error in Lpro reinit");
      }
      // Make the Algo status to None so that VW algo can run once again
      me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;
    }
     else if (LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
    {
      /* send Duty cycle reset command to AFE */
      result = listen_proc_send_cmd_to_afe(me_ptr,
                                           me_ptr->afe_port_id,
                                           ELITEMSG_CUSTOM_AFECLIENT_DC_RESET, NULL);
      if(ADSP_FAILED(result))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d] DC Reset is failed, status:: %d",
              me_ptr->session_id, result);
      }
      /* After success make data_to_process = FALSE, so that listen wont
          process any data coming from AFE */
      me_ptr->is_data_to_process = FALSE;
    }
  }
#endif

  return result;
}

/*
  Call back for CAPI_V2 events handled by LSM

  @param [in/out] topo_ptr : topology handler
  @param [in] module_index : index of the module in topology chain
  @param [in] param_size : Size of event parameter
  @param [in/out] param_payload : Payload of parameter sent along with event

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static capi_v2_err_t listen_proc_event_detection_status(topo_t* topo_ptr,
                                                        uint32_t module_index,
                                                        uint32_t param_size,
                                                        void* param_payload)
{
  ADSPResult result = ADSP_EOK;
  detection_engine_status_t* detection_status;
  uint32_t apr_payload_size, event_id;
  listen_proc_svc_t* me_ptr = (listen_proc_svc_t*)topo_ptr->session_ptr;
  lsm_callback_handle_t *cb_data_ptr = NULL;

  if(NULL == me_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Lpro event handler: NULL session pointer ");
    return CAPI_V2_EBADPARAM;
  }

  if(param_size < sizeof(detection_engine_status_t))
  {
    MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Lpro[%d]: Module with id 0x%lX provided insufficient size for "
          "detection_status. Required: %d bytes, provided: %lu bytes.",
          me_ptr->session_id,
          topo_ptr->module[module_index].module_id,
          sizeof(detection_engine_status_t),
          param_size);
    return CAPI_V2_ENOMEMORY;
  }

  detection_status = (detection_engine_status_t*)param_payload;

  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "Lpro[%d] status by Detection Engine ::0x%x, all events to send %x Time stamp = %ld",
        me_ptr->session_id, 
        detection_status->status,
        detection_status->is_debug_mode,
        detection_status->detection_timestamp);

   /* Populate the algo status to be used when LAB is enabled*/
   me_ptr->detection_status = detection_status->status;

  /* Send  event to client in either of following cases:
     1. If client requested all events  irrespective of detection/rejection OR
     2. Send the event to client only in case of successful detection
     [Note] Any scenario like RUNNING, extra which the module doesn't wishes to
            send to LSM client must be filtered in module itself */
  if(LSM_DETECTION_STATUS_DETECTED == detection_status->status  ||
     (TRUE == detection_status->is_debug_mode &&
      LSM_DETECTION_STATUS_REJECTED == detection_status->status)  )
  {
    cb_data_ptr =  &me_ptr->callback_data;

     if(LSM_TIME_STAMP_MODE == me_ptr->mode)
     {
       event_id = LSM_SESSION_EVENT_DETECTION_STATUS_V3;
       apr_payload_size = sizeof(lsm_session_event_detection_status_v3_t) +
           detection_status->payload_size;
     }
     else
     {
       event_id = LSM_SESSION_EVENT_DETECTION_STATUS_V2;
       apr_payload_size = sizeof(lsm_session_event_detection_status_v2_t) +
                   detection_status->payload_size;
     }

    /* Now send the event to client */
     result = listen_proc_svc_send_event_to_client(cb_data_ptr,detection_status, apr_payload_size,
                                                   event_id,detection_status->status);
    if(ADSP_SUCCEEDED (result))
    {
       MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO,
             "LProSvc[%d] Sent LSM_SESSION_EVENT_DETECTION_STATUS Event to apps for mode %ld, status:%d",
             me_ptr->session_id, me_ptr->mode, detection_status->status);
    }
  }

  return adsp_result_to_capi_v2_err(result);
}

capi_v2_err_t lsm_capi_v2_event_callback(void *context_ptr,
                                         capi_v2_event_id_t id,
                                         capi_v2_event_info_t *event_ptr)
{
  capi_v2_err_t result = CAPI_V2_EOK;
  topo_t *topo_ptr = topo_event_get_struct_ptr(context_ptr);
  uint32_t module_index = topo_event_get_module_index(context_ptr);
  listen_proc_svc_t* me_ptr = (listen_proc_svc_t*)topo_ptr->session_ptr;
  uint16_t bytes_per_sample = (me_ptr->bits_per_sample>16)?4:2;
  ADSPResult adsp_result = ADSP_EOK;

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "Lpro[%d]:Mod id[0x%lx] raised event id[0x%lx]",
        me_ptr->session_id, topo_ptr->module[module_index].module_id, (uint32_t)(id));

  switch (id)
  {
    case CAPI_V2_EVENT_KPPS:
    {
      if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_KPPS_t))
      {
        MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d]:Mod id[0x%lx] event id[0x%lx]"
              "Required:%lu, got:%lu bytes.",
              me_ptr->session_id,
              topo_ptr->module[module_index].module_id,
              (uint32_t)(id),
              sizeof(capi_v2_event_KPPS_t),
              event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_KPPS_t *kpps_ptr =
                            (capi_v2_event_KPPS_t *)event_ptr->payload.data_ptr;

      uint32_t old_kpps = topo_ptr->module[module_index].kpps;
      uint32_t new_kpps = kpps_ptr->KPPS;
      uint32_t topo_total_kpps = topo_ptr->kpps;

      if(old_kpps == new_kpps)
      {
        /* Skip updating */
        break;
      }

      topo_total_kpps -= old_kpps;
      topo_total_kpps += new_kpps;

      /* Update session mcps:
         Session mcps is sum of topology and internal utils like lab */
      /* Update topo kpps */
      topo_ptr->kpps = topo_total_kpps;

      /* Update Module kpps */
      topo_ptr->module[module_index].kpps = new_kpps;

      if(LISTEN_PROC_STATE_RUN == me_ptr->state)
      {
        adsp_result = lsm_mmpm_config(&me_ptr->mmpm_info,
                                      topo_ptr,
                                      LSM_MMPM_VOTE_UPDATE_KPPS_ONLY);
        result = adsp_result_to_capi_v2_err(adsp_result);
      }

      break;
    }

    case CAPI_V2_EVENT_BANDWIDTH:
    {
#ifndef LSM_MMPM_DISABLE_KBPS_VOTING
      if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_bandwidth_t))
      {
        MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d]:Mod id[0x%lx] event id[0x%lx]"
              "Required: %lu,got: %lu bytes.",
              me_ptr->session_id,
              topo_ptr->module[module_index].module_id,
              (uint32_t)(id),
              sizeof(capi_v2_event_bandwidth_t),
              event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_bandwidth_t* bandwidth_ptr =
                     (capi_v2_event_bandwidth_t *)(event_ptr->payload.data_ptr);

      uint32_t old_bps = topo_ptr->module[module_index].bps;
      uint32_t new_code_bandwidth = bandwidth_ptr->code_bandwidth;
      uint32_t new_data_bandwidth = bandwidth_ptr->data_bandwidth;

      /* Currently assuming LSM will not have special handling */
      uint32_t new_bps = new_code_bandwidth + new_data_bandwidth;

      uint32_t topo_total_bps = topo_ptr->bps;

      if(old_bps == new_bps)
      {
        /* Skip updating, as there is no change */
        break;
      }

      topo_total_bps -= old_bps;
      topo_total_bps += new_bps;

      /* Update topo kbps */
      topo_ptr->bps = topo_total_bps;

      /* Update Module kbps */
      topo_ptr->module[module_index].bps = new_bps;

      if(LISTEN_PROC_STATE_RUN == me_ptr->state)
      {
        lsm_mmpm_vote_t update_type = LSM_MMPM_VOTE_UPDATE_BPS_ONLY;
        adsp_result = lsm_mmpm_config(&me_ptr->mmpm_info,
                                      topo_ptr,
                                      update_type);
        result = adsp_result_to_capi_v2_err(adsp_result);
      }
#endif
      break;
    }

    case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
    {
      if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
      {
        MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d]:Mod id[0x%lx] event id[0x%lx]"
              "Required: %lu,got: %lu bytes.",
              me_ptr->session_id,
              topo_ptr->module[module_index].module_id,
              (uint32_t)(id),
              sizeof(capi_v2_event_data_to_dsp_service_t),
              event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_data_to_dsp_service_t *data_ptr =
             (capi_v2_event_data_to_dsp_service_t *)event_ptr->payload.data_ptr;

      /* Routing Supporting Parameters */
      switch(data_ptr->param_id)
      {
        case PARAM_ID_DETECTION_ENGINE_STATUS:
        {
          result = listen_proc_event_detection_status(topo_ptr,
                                                      module_index,
                                                      data_ptr->payload.actual_data_len,
                                                      data_ptr->payload.data_ptr);
        }
        break;
        case PARAM_ID_KW_END_POSITION:
        {
          if(data_ptr->payload.actual_data_len < sizeof(kw_end_position_t))
          {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Lpro[%d]:Mod id[0x%lx], Required: %lu,got: %lu bytes.",
                  me_ptr->session_id,
                  topo_ptr->module[module_index].module_id,
                  sizeof(kw_end_position_t),
                  data_ptr->payload.actual_data_len);
            return CAPI_V2_ENOMEMORY;
          }
          kw_end_position_t *kw_end_position = (kw_end_position_t *)data_ptr->payload.data_ptr;
          lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;

          if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable))
          {
            lab_struct_ptr->start_lab = TRUE;
            lab_struct_ptr->kwed_position_bytes = kw_end_position->kw_end_position_samples*bytes_per_sample*me_ptr->num_channels;
          }
         }
        break;
        case PARAM_ID_KW_END_MAX_BUFFER_REQ:
        {
          if(data_ptr->payload.actual_data_len < sizeof(kw_end_max_delay_t))
          {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Lpro[%d]:Mod id[0x%lx], Required: %lu,got: %lu bytes.",
                  me_ptr->session_id,
                  topo_ptr->module[module_index].module_id,
                  sizeof(kw_end_max_delay_t),
                  data_ptr->payload.actual_data_len);
        	  return CAPI_V2_ENOMEMORY;
          }
          kw_end_max_delay_t *kw_end_max_delay = (kw_end_max_delay_t *)data_ptr->payload.data_ptr;
          me_ptr->kw_end_max_delay_bytes = kw_end_max_delay->kw_end_max_delay_samples*bytes_per_sample*me_ptr->num_channels;
          MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "MAX KWD END DELAY Bytes %u",me_ptr->kw_end_max_delay_bytes );
        }
        break;
        default:
        {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Lpro[%d]:Mod id[0x%lx] Param id[0x%lx]:Unsupported.",
                me_ptr->session_id,
                topo_ptr->module[module_index].module_id,
                data_ptr->param_id);
          return CAPI_V2_EUNSUPPORTED;
        }
      }

      break;
    }
    case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
    {
      if(event_ptr->payload.actual_data_len < sizeof(topo_media_fmt_t))
      {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id %lu provided insufficient size for header for event id %lu. Required: %d bytes, provided: %lu bytes.",topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id), sizeof(topo_media_fmt_t), event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }

      topo_media_fmt_t *format_ptr = (topo_media_fmt_t*)(event_ptr->payload.data_ptr);

      if (!event_ptr->port_info.is_valid)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id 0x%lX provided invalid port for event id %lu. Only output port 0 is supported.", topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id));
        return CAPI_V2_EUNSUPPORTED;
      }
      if (event_ptr->port_info.is_input_port)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id 0x%lX provided input port for event id %lu. Only output port 0 is supported.", topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id));
        return CAPI_V2_EUNSUPPORTED;
      }

      if (CAPI_V2_FIXED_POINT != format_ptr->h.data_format)
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id 0x%lX provided data_format %lu for event id %lu. Only port FIXED_POINT(0) is  supported.",topo_ptr->module[module_index].module_id, static_cast<uint32_t>(format_ptr->h.data_format), static_cast<uint32_t>(id));
        return CAPI_V2_EUNSUPPORTED;
      }

      topo_ptr->module[module_index].output_media_fmt = *format_ptr;
      topo_ptr->module[module_index].is_output_media_event_raised = TRUE;
      break;
    }
    case CAPI_V2_EVENT_PROCESS_STATE:
    {
      if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_process_state_t))
      {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id 0x%lX provided insufficient size for header for event id %lu. Required: %d bytes, provided: %lu bytes.", topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id), sizeof(capi_v2_event_process_state_t), event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_process_state_t *payload_ptr = (capi_v2_event_process_state_t*)(event_ptr->payload.data_ptr);

      bool_t old_process_state = topo_ptr->module[module_index].is_enabled;
      topo_ptr->module[module_index].is_enabled = payload_ptr->is_enabled;

      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Module with id 0x%lX changed process state from %hu to %hu.",topo_ptr->module[module_index].module_id, old_process_state, topo_ptr->module[module_index].is_enabled);

       break;
    }
    case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
    {
      if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_algorithmic_delay_t))
      {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module with id 0x%lX provided insufficient size for event id %lu. Required: %d bytes, provided: %lu bytes.",
              topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id), sizeof(capi_v2_event_algorithmic_delay_t), event_ptr->payload.actual_data_len);
        return CAPI_V2_ENEEDMORE;
      }

      uint32_t old_delay = topo_ptr->module[module_index].delay_in_us;
      uint32_t new_delay = reinterpret_cast<capi_v2_event_algorithmic_delay_t *>(event_ptr->payload.data_ptr)->delay_in_us;
      uint32_t old_total_delay = topo_ptr->delay_in_us;
      uint32_t new_total_delay = old_total_delay;

      new_total_delay -= old_delay;
      new_total_delay += new_delay;

      topo_ptr->module[module_index].delay_in_us = new_delay;
      topo_ptr->delay_in_us = new_total_delay;
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Module with id 0x%lX changed delay from %lu us to %lu us. PP total delay changed from %lu us to %lu us.",
            topo_ptr->module[module_index].module_id, old_delay, new_delay, old_total_delay, new_total_delay);
      break;
    }
    default:
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d]:Mod id[0x%lx] event id[0x%lx]:Unsupported.",
            me_ptr->session_id,
            topo_ptr->module[module_index].module_id,
            (uint32_t)(id));
      return CAPI_V2_EUNSUPPORTED;
  }

  return result;
}

/**
  Function which process the eob command

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
*/

ADSPResult listen_proc_svc_eob_handler(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK, ack_result = ADSP_EOK, lab_result = ADSP_EOK;
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t*) (me_ptr->cmd_msg.pPayload);
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;

  // First check the state whether it is already in STOP state or in RUN state
  // If it is not in RUN state return error

  if (me_ptr->state != LISTEN_PROC_STATE_RUN)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Lpro[%d] EOB is expected in RUN state after detection, State: %d,algo status = %d",
          me_ptr->session_id, me_ptr->state, me_ptr->detection_status);
    result = ADSP_EUNEXPECTED;
    goto __bailout;
  }
  else if((NULL != lab_struct_ptr) && (TRUE == lab_struct_ptr->lab_enable))
  {

    listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_OUTPUT_DATA_Q, ADSP_EOK);

    if(ADSP_FAILED(lab_result = listen_proc_lab_stop(me_ptr)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Lpro[%d] fail to stop the LAB, status:: %d", me_ptr->session_id, result);
      /* Even LAB stop fails we are going ahead and doing DC reset*/
    }

    /* If LSM connected to ADM, we are not supporting DC Reset command*/
    /*So reiniting the algo, through media type*/
    if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
    {
      if(ADSP_FAILED(listen_proc_svc_reinit(me_ptr, FALSE)))
      {
        MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
            "Error in listenproc reinit");
      }
      // Make the Algo status to None so that VW algo can run once again
      me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;
    }
    else if (LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
    {
      // make data_to_process = FALSE, so that listen wont process any data coming from AFE
      me_ptr->is_data_to_process = FALSE;
      // Make the Algo status to None so that VW algo can run once again
      me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;

#ifndef DISABLE_DC_RESET  //<-- Need to remove once Algo update is available
      //send Duty cycle reset command to AFE
      if (ADSP_FAILED(result = listen_proc_send_cmd_to_afe(me_ptr,me_ptr->afe_port_id,
                                                           ELITEMSG_CUSTOM_AFECLIENT_DC_RESET, NULL)))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Lpro[%d] DC Reset is failed, status:: %d", me_ptr->session_id, result);
      }
      result = ADSP_FAILED(result|lab_result)?ADSP_EFAILED:ADSP_EOK;
#endif
    }
  }

  __bailout:
  /* not prinitng fail messages here, as lsm-generate-ack have necessary dbg messages*/
  ack_result = lsm_generate_ack(apr_pkt_ptr, result, NULL, 0, 0);
  result =   ADSP_FAILED(result|ack_result)?ADSP_EFAILED:result;
  return result;
}


/*
  This function is to send buffers to clients

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/
void listen_proc_svc_process_output_data_q(listen_proc_svc_t* me_ptr)
{
    ADSPResult result = ADSP_EOK;
    lsm_proc_svc_out_buf_param_t  *out_buf_params_ptr = &me_ptr->out_buf_params;
    lsm_lab_struct_t * lsm_lab_Struct = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;

    while(0 != lsm_lab_Struct->circ_buf_struct.unread_bytes)
    {
      if (0 == (out_buf_params_ptr->empty_bytes))
      {
        if (ADSP_EOK != (result = listen_proc_svc_get_new_out_buffer(me_ptr)))
        {
#ifdef LSM_DEBUG
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "Lpro[%d] Failed in processing get_new_out_buffer with result = %d",
        		me_ptr->session_id,result);
#endif
          return;
        }
      }
      listen_proc_lab_copy_data_to_out_buf(lsm_lab_Struct,out_buf_params_ptr);

      if (0 == (out_buf_params_ptr->empty_bytes))
      {
        listen_proc_svc_send_data_read_done(me_ptr, out_buf_params_ptr->out_buf_wr_offset, ADSP_EOK);
      }
    }
}

/**
  Gets available buffer present in output buffer Queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_get_new_out_buffer(listen_proc_svc_t* me_ptr)
{
    uint32_t                        channel_status;
    uint32_t                        apr_op_code;
    ADSPResult                      result = ADSP_EOK;
    elite_apr_packet_t* apr_pkt_ptr = NULL;
    lsm_data_cmd_read_t* lsm_data_cmd_ptr = NULL;
    lsm_proc_svc_out_buf_param_t *out_buf_params_ptr = NULL;
    elite_mem_shared_memory_map_t   *shared_mem_map_type_node = &(me_ptr->out_buf_params.shared_memmap_type_node);

    channel_status = qurt_elite_channel_poll(qurt_elite_queue_get_channel(me_ptr->svc_handle.gpQ),
    		                                 qurt_elite_queue_get_channel_bit(me_ptr->svc_handle.gpQ));
   if (channel_status)
   {
       if ( ADSP_FAILED(result=qurt_elite_queue_pop_front(me_ptr->svc_handle.gpQ,
    		                                              (uint64_t*)&(me_ptr->output_data_msg) )) )
       {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        		"Lpro[%d] failed in popping output buffers %d",
        		me_ptr->session_id, result );
          me_ptr->output_data_msg.pPayload = NULL;
          return result;
       }

       if (ELITE_APR_PACKET != (me_ptr->output_data_msg.unOpCode))
       {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        		 "Lpro[%d] Received non-APR message on output data q w/ opcode = 0x%lx" ,
                  me_ptr->session_id, me_ptr->output_data_msg.unOpCode);
           result = elite_msg_finish_msg(&me_ptr->output_data_msg, ADSP_EUNSUPPORTED);
           me_ptr->output_data_msg.pPayload = NULL;
           return result;
       }

       apr_pkt_ptr = (elite_apr_packet_t*) (me_ptr->output_data_msg.pPayload);
       if(!apr_pkt_ptr)
       {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Lpro[%d] APR packet pointer is NULL!",
        		 me_ptr->session_id);
           //return error
           return ADSP_EBADPARAM;
       }

       apr_op_code = elite_apr_if_get_opcode( apr_pkt_ptr ) ;

       switch (apr_op_code)
       {
            case LSM_DATA_CMD_READ:
            {
#ifdef LSM_DEBUG
                MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] Received LSM_DATA_CMD_READ",
                      me_ptr->session_id);
#endif
                lsm_data_cmd_ptr = (lsm_data_cmd_read_t *)elite_apr_if_get_payload_ptr(apr_pkt_ptr);

                if (NULL == lsm_data_cmd_ptr)
                {
                    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Lpro[%d] NULL payload from APR packet!",
                          me_ptr->session_id);
                    listen_proc_svc_send_data_read_done(me_ptr, 0, ADSP_EBADPARAM);
                    return ADSP_EBADPARAM;
                }

                //if output buffer is not 32 byte aligned return it
                if(lsm_data_cmd_ptr->buf_addr_lsw & MASK_32BYTE_ALIGNMENT)
                {
                    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    	  "Lpro[%d] Output buffer not 32 byte aligned, returning it!",
                    	  me_ptr->session_id);
                    listen_proc_svc_send_data_read_done(me_ptr, 0, ADSP_EBADPARAM);
                    return ADSP_EBADPARAM;
                }

                shared_mem_map_type_node->unMemMapClient = lsm_memory_map_client;
                shared_mem_map_type_node->unMemMapHandle = lsm_data_cmd_ptr->mem_map_handle;
                shared_mem_map_type_node->unPhysAddrLsw = lsm_data_cmd_ptr->buf_addr_lsw;
                shared_mem_map_type_node->unPhysAddrMsw = lsm_data_cmd_ptr->buf_addr_msw;
                shared_mem_map_type_node->unVirtAddr = 0;

                if (ADSP_FAILED(result = elite_mem_map_get_shm_attrib_ref_counted(lsm_data_cmd_ptr->buf_addr_lsw,
                		                                                          lsm_data_cmd_ptr->buf_addr_msw,
                                                                                  lsm_data_cmd_ptr->buf_size,
                                                                                  shared_mem_map_type_node )))
                {
                    MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                          "Lpro[%d] Phy to Virt Failed(PA,VA)-->(%lx%lx,%lx)",
                          me_ptr->session_id,
                          shared_mem_map_type_node->unPhysAddrMsw,
                          shared_mem_map_type_node->unPhysAddrLsw,
                          shared_mem_map_type_node->unVirtAddr);
                    listen_proc_svc_send_data_read_done(me_ptr, 0, ADSP_EBADPARAM);
                    return ADSP_EBADPARAM;
                }
#ifdef LSM_DEBUG
                MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                      "Lpro[%d] output buffer(PAMsw,PALsw,VA,BufSize)-->(%lx,%lx,%lx,%lu)",
                      me_ptr->session_id,
                      shared_mem_map_type_node->unPhysAddrMsw,
                      shared_mem_map_type_node->unPhysAddrLsw,
                      shared_mem_map_type_node->unVirtAddr,
                      lsm_data_cmd_ptr->buf_size);
#endif
                //Update output buffer params
                out_buf_params_ptr = &me_ptr->out_buf_params;
                out_buf_params_ptr->out_buf_wr_offset = 0;
                out_buf_params_ptr->empty_bytes = lsm_data_cmd_ptr->buf_size;

                break;
            } /* case LSM_DATA_CMD_READ */
            default:
            {
#ifdef LSM_DEBUG
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                      "Lpro[%d] Unexpected opCode for data write 0x%lx",
                      me_ptr->session_id, apr_op_code );
#endif
                result = lsm_generate_ack(apr_pkt_ptr, ADSP_EFAILED, NULL, 0, 0);
                me_ptr->output_data_msg.pPayload = NULL;
                break;
            }

       }// end of switch(me_ptr->gpQMsg.unOpCode)

   }
   else
   {
#ifdef LSM_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] No output buffers received in outputq",
    	   me_ptr->session_id);
#endif
     return ADSP_ENOTREADY;
   }

   return result;
}

/*
 *Function Name: listen_get_output_buffer
 *
 *Parameters: listen_proc_svc_t* me_ptr: Listen Service state
 *            elite_apr_packet_t ** apr_pkt_ptr_ptr: pointer to
 *                                   APR packet handle
 *            lsm_data_cmd_read_t ** out_buf_ptr_ptr: pointer to
 *                                   output buffer handle
 *
 *Description: Returns the APR packet handle and output buffer handle
 *
 *Returns: ADSPResult
 *
 */
ADSPResult listen_proc_svc_get_output_buffer(listen_proc_svc_t* me_ptr,
      elite_apr_packet_t ** apr_pkt_ptr_ptr,
      lsm_data_cmd_read_t ** out_buf_ptr_ptr)
{

   if((NULL == apr_pkt_ptr_ptr) || (NULL == out_buf_ptr_ptr) || (NULL == me_ptr->output_data_msg.pPayload))
   {
#ifdef LSM_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] received bad parameters",
    	   me_ptr->session_id);
#endif
     return ADSP_EFAILED;
   }

   *apr_pkt_ptr_ptr = ( elite_apr_packet_t *) (me_ptr->output_data_msg.pPayload);
   *out_buf_ptr_ptr = NULL;

   elite_apr_if_get_payload( (void**)(out_buf_ptr_ptr), *apr_pkt_ptr_ptr );
   if (NULL == *out_buf_ptr_ptr)
   {
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    		"Lpro[%d] no output buffer to write listen data", me_ptr->session_id);
#endif
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

/*
 *Function Name: listen_proc_svc_send_data_read_done
 *
 *Parameters: listen_proc_svc_t* me_ptr: listen service state
 *            uint32_t out_buf_size: output buffer size to be filled in ack msg
 *            ADSPResult err_code: error code to fill in ack msg
 *
 *Description: Sends a read_done event to client and free the APR message
 *             coming from client
 *
 *Returns: ADSPResult
 *
 */
ADSPResult listen_proc_svc_send_data_read_done(listen_proc_svc_t* me_ptr,
      uint32_t out_buf_size,
      ADSPResult err_code)
{
   elite_apr_packet_t  *apr_pkt_ptr = NULL;
   lsm_data_cmd_read_t *out_buf_ptr = NULL;
   lsm_data_event_read_done_t read_done_evt;
   ADSPResult result = ADSP_EOK;

   if(ADSP_FAILED(result = listen_proc_svc_get_output_buffer(me_ptr, &apr_pkt_ptr, &out_buf_ptr)))
   {
#ifdef LSM_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] lsm_get_output_buffer failed",
    	   me_ptr->session_id);
#endif
     return ADSP_EFAILED;
   }

   //fill up read_done_evt values
   read_done_evt.buf_addr_lsw = out_buf_ptr->buf_addr_lsw;
   read_done_evt.buf_addr_msw = out_buf_ptr->buf_addr_msw;
   read_done_evt.mem_map_handle = out_buf_ptr->mem_map_handle;
   read_done_evt.total_size = out_buf_size;
   read_done_evt.status = (uint32_t) err_code;
   read_done_evt.offset = 0;
   read_done_evt.flags = 0;
   read_done_evt.timestamp_lsw = 0;
   read_done_evt.timestamp_msw = 0;

   if((out_buf_size > 0) && (0 != me_ptr->out_buf_params.shared_memmap_type_node.unVirtAddr))
   {
     //Flush the shared mem map region
     elite_mem_flush_cache( &(me_ptr->out_buf_params.shared_memmap_type_node) );

     listen_proc_svc_log_output_data(me_ptr,
    		                         (int8_t *)me_ptr->out_buf_params.shared_memmap_type_node.unVirtAddr,
    		                         out_buf_size);
   }
   if (0 != me_ptr->out_buf_params.shared_memmap_type_node.unVirtAddr)
   {
      elite_mem_map_release_shm(&(me_ptr->out_buf_params.shared_memmap_type_node));
   }

   //Send LSM_DATA_EVENT_READ_DONE event to client. OpCode will be filled by lsm_generate_ack function
   result= lsm_generate_ack(apr_pkt_ptr,
                            err_code,
                            (void*)&read_done_evt,
                            sizeof(read_done_evt),
                            NULL);

   //set payload of gpQMsg to null to indicate that we are not holding on to any output buffer
   me_ptr->output_data_msg.pPayload = NULL;

   //reset the out_buf_params in case the next buffers are flushed
   me_ptr->out_buf_params.out_buf_wr_offset = 0;
   me_ptr->out_buf_params.empty_bytes = 0;
   memset(&me_ptr->out_buf_params.shared_memmap_type_node,0,sizeof(elite_mem_shared_memory_map_t));

   return result;
}

/*
 *Function Name: listen_proc_svc_log_input_data
 *
 *Parameters: buf_addr : pointer to buffer to be logged
 *            buf_size : buffer size in bytes
 *            me_ptr : pointer to listen_proc_svc_t structure
 *
 *Description: Logs the input PCM data
 *
 *Returns: ADSPResult
 *
*/
static inline
ADSPResult listen_proc_svc_log_input_data(listen_proc_svc_t* me_ptr,
                                    int8_t *buf_addr,
                                    uint32_t buf_size)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;
   uint32_t bits_per_sample = me_ptr->media_fmt.f.bits_per_sample;
   uint32_t num_channels = me_ptr->media_fmt.f.num_channels;
   uint32_t sampling_rate = me_ptr->media_fmt.f.sampling_rate;
   uint16_t bytes_per_sample = (bits_per_sample>16)?4:2;
   uint32_t num_samples = buf_size/bytes_per_sample;
   uint32_t num_samples_per_ch = num_samples/num_channels;

   uint8_t channel_mapping_array[ELITE_MAX_NUM_CH];
   uint8_t i = 0;
   memset(&channel_mapping_array,0,sizeof(uint8_t)*ELITE_MAX_NUM_CH);

   for(i=0;i<me_ptr->num_channels;i++)
   {
     channel_mapping_array[i] = me_ptr->media_fmt.f.channel_type[i];
   }

   if(CAPI_V2_DEINTERLEAVED_UNPACKED == me_ptr->media_fmt.f.data_interleaving)
   {
     lsm_interleave_samples(buf_addr,me_ptr->input_log_buf_ptr, num_channels,
                                 num_samples_per_ch, bytes_per_sample);
   }
   else
   {
     memscpy(me_ptr->input_log_buf_ptr,me_ptr->input_log_buf_size,
             buf_addr,buf_size);
   }
   if(4 == bytes_per_sample)
   {
      /* convert the format Q27 to Q31 */
      int32_t *pBuf_frm_ptr32 = (int32_t *)(me_ptr->input_log_buf_ptr);
      for(uint32_t i=0; i<num_samples; i++)
      {
         pBuf_frm_ptr32[i] = (pBuf_frm_ptr32[i] << QFORMAT_SHIFT_FACTOR);
      }
   }
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_LSM_OUTPUT;
   log_info_obj.buf_ptr = (uint8_t *)me_ptr->input_log_buf_ptr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = me_ptr->session_id;
   log_info_obj.log_tap_id = LSMLOG_IN_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();


   log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

   pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

   pcm_data->num_channels = num_channels;
   pcm_data->sampling_rate = sampling_rate;
   pcm_data->bits_per_sample = (bits_per_sample == 16) ? 16 : 32;
   pcm_data->interleaved = TRUE; //output is always interleaved.
   pcm_data->channel_mapping = channel_mapping_array;

   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
   }

/*
 *Function Name: listen_proc_svc_log_output_data
 *
 *Parameters: buf_addr : pointer to buffer to be logged
 *            buf_size : buffer size in bytes
 *            me_ptr : pointer to listen_proc_svc_t structure
 *
 *Description: Logs the LAB PCM data
 *
 *Returns: ADSPResult
 *
*/
static inline
ADSPResult listen_proc_svc_log_output_data(listen_proc_svc_t* me_ptr, int8_t *buf_addr, uint32_t buf_size)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;

   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_LSM_OUTPUT;
   log_info_obj.buf_ptr = (uint8_t *)buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = me_ptr->session_id;
   log_info_obj.log_tap_id = LSMLOG_OUT_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();


   log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

   pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

   pcm_data->num_channels = me_ptr->num_channels;
   pcm_data->sampling_rate = me_ptr->sample_rate;
   pcm_data->bits_per_sample = me_ptr->bits_per_sample;
   pcm_data->interleaved = TRUE; //output is always interleaved.
   pcm_data->channel_mapping = me_ptr->channel_mapping;

   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}
ADSPResult listen_proc_svc_send_event_to_client(lsm_callback_handle_t *cb_data_ptr,
                                                void* det_status,
                                                uint32_t apr_payload_size,
                                                uint32_t event_id, uint32_t status)
{
  ADSPResult result = ADSP_EOK;
  ADSPResult temp_result = ADSP_EOK;
  detection_engine_status_t* detection_status = (detection_engine_status_t *)det_status;
  elite_apr_packet_t* apr_pkt_ptr = NULL;
  lsm_session_event_detection_status_v2_t *det_status_v2 = NULL;
  lsm_session_event_detection_status_v3_t *det_status_v3 = NULL;
  int8_t *apr_payload_ptr = NULL;
  if (ADSP_FAILED(result = elite_apr_if_alloc_event(cb_data_ptr->apr_handle,
                                                    cb_data_ptr->self_addr,
                                                    cb_data_ptr->self_port,
                                                    cb_data_ptr->client_addr,
                                                    cb_data_ptr->client_port,
                                                    0,
                                                    event_id,
                                                    apr_payload_size, &apr_pkt_ptr)))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro:Could not allocate status event packet=0x%x!",
          result);
#endif
    return result ;
  }
  if(NULL == apr_pkt_ptr )
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "LPro:Not enough memory to allocate apr pkt");
#endif
    return ADSP_ENOMEMORY;
  }

  apr_payload_ptr = (int8_t *)elite_apr_if_get_payload_ptr(apr_pkt_ptr);
  memset(apr_payload_ptr, 0, apr_payload_size);
  switch(event_id)
  {
    case LSM_SESSION_EVENT_DETECTION_STATUS_V2:
      det_status_v2 = (lsm_session_event_detection_status_v2_t *)apr_payload_ptr;
      det_status_v2->detection_status = (uint8_t)status;
      det_status_v2->payload_size = detection_status->payload_size;

      /*  Now copy the actual result status returned by library */
      memscpy(apr_payload_ptr + sizeof(lsm_session_event_detection_status_v2_t),
              apr_payload_size - sizeof(lsm_session_event_detection_status_v2_t),
              detection_status->payload_ptr, detection_status->payload_size);
      break;
    case LSM_SESSION_EVENT_DETECTION_STATUS_V3:      
      det_status_v3 = (lsm_session_event_detection_status_v3_t *)apr_payload_ptr;

      det_status_v3->timestamp_lsw      = (uint32_t)detection_status->detection_timestamp;
      det_status_v3->timestamp_msw      = (uint32_t)( (detection_status->detection_timestamp)>>32 );
      det_status_v3->detection_status   = (uint8_t)status;
      det_status_v3->payload_size       = detection_status->payload_size;

      /*  Now copy the actual result status returned by library */
      memscpy(apr_payload_ptr + sizeof(lsm_session_event_detection_status_v3_t),
              apr_payload_size - sizeof(lsm_session_event_detection_status_v3_t),
              detection_status->payload_ptr, detection_status->payload_size);

      break;
    case LSM_DATA_EVENT_STATUS:
      ((lsm_data_event_status_t *)apr_payload_ptr)->status = status;
      break;
    default:
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LProSvc unsupported 0x%x event",
            event_id);
#endif
      temp_result = ADSP_EUNSUPPORTED;
      goto __bailout;
      break;
  }

  //Now send the event to client
  if (ADSP_FAILED (result = elite_apr_if_async_send(cb_data_ptr->apr_handle, apr_pkt_ptr)))
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro:Failed to send 0x%x event = %d!",
          event_id,result);
#endif
    temp_result = result;
    goto __bailout;
  }
  return result;
__bailout:
  if (ADSP_FAILED(result = elite_apr_if_free(cb_data_ptr->apr_handle, apr_pkt_ptr)))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Freeing Packet failed 0x%x!", result);
#endif
  }
  return (result|temp_result);
}


static ADSPResult listen_proc_send_cmd_to_adm(listen_proc_svc_t *me_ptr, uint32_t cmd)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg;
  int nActualSize = 0;
  qurt_elite_queue_t *adm_cmd_q = qurt_elite_globalstate.pAdmStatSvcCmdQ;
  qurt_elite_bufmgr_node_t bufMgrNode;
  uint32_t reqd_payload_size = sizeof(EliteMsg_CustomAdmStreamConnectType);
  EliteMsg_CustomAdmStreamConnectType *adm_connect_msg_ptr = NULL;
  EliteMsg_CustomAdmStreamMtMxReConfig *mtmx_reconfig_msg_ptr = NULL;
  uint8_t channel_mapping[8] = {PCM_CHANNEL_C, 0, 0, 0, 0, 0, 0, 0};
  uint8_t i = 0;
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] Sending cmd 0x%x to ADM",me_ptr->session_id,cmd);

  if (ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG == cmd) 
  {
    reqd_payload_size = sizeof(EliteMsg_CustomAdmStreamMtMxReConfig);
  }

  result = elite_mem_get_buffer(reqd_payload_size,
                                &bufMgrNode,
                                &nActualSize);

  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to create mesage result=0x%x",
          result);
    return result;
  }
  switch (cmd)
  {
    case ELITEMSG_CUSTOM_ADM_LSM_STREAM_CONNECT:
    case ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT:
    case ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISABLE:
    {
      adm_connect_msg_ptr = (EliteMsg_CustomAdmStreamConnectType*) bufMgrNode.pBuffer;
      memset(adm_connect_msg_ptr, 0, sizeof(EliteMsg_CustomAdmStreamConnectType));
      adm_connect_msg_ptr->unSecOpCode = cmd;
      adm_connect_msg_ptr->pResponseQ = me_ptr->response_q_ptr;
      adm_connect_msg_ptr->unClientToken = 0;
      adm_connect_msg_ptr->pBufferReturnQ = bufMgrNode.pReturnQ;
      adm_connect_msg_ptr->phPort = &me_ptr->svc_handle;
      adm_connect_msg_ptr->unSessionID = me_ptr->session_id+1; //actual sesion id to be sent
      adm_connect_msg_ptr->direction = ADM_TX_DIR;
      adm_connect_msg_ptr->bits_per_sample = me_ptr->bits_per_sample;
      adm_connect_msg_ptr->unPortMode = ADM_STREAM_CONNECT_PORT_MODE_RELATIVE_TS;
      adm_connect_msg_ptr->is_compressed = FALSE; 
      adm_connect_msg_ptr->buffering_ms = NUM_BUFERS_LSM_AND_MTMX*MATRIX_OUT_FRAME_SIZE_5_MSEC;  //macro
      adm_connect_msg_ptr->stream_perf_mode = ADM_LEGACY_DEVICE_SESSION; 

      msg.unOpCode = ELITE_CUSTOM_MSG;
      msg.pPayload = (void*)adm_connect_msg_ptr;
    }
    break;

    case ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG:
    {
      /*ADM is expecting channel mapping as part of reconfig, otherwise it returns error if we dont 
           fill the values. As SVA is always mono we are filling  below values as per ADM expectations
           for MONO*/
      if(me_ptr->num_channels == 1)
      {
         channel_mapping[0] = PCM_CHANNEL_C;
      }
      else
      {
        for(i=0;i<LSM_MAX_NUM_CHANNELS;i++)
        {
          channel_mapping[i] = me_ptr->media_fmt.f.channel_type[i];
        }
      }
      mtmx_reconfig_msg_ptr = (EliteMsg_CustomAdmStreamMtMxReConfig*) bufMgrNode.pBuffer;
      memset(mtmx_reconfig_msg_ptr,0, sizeof(EliteMsg_CustomAdmStreamMtMxReConfig));

      mtmx_reconfig_msg_ptr->unSecOpCode = ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG;
      mtmx_reconfig_msg_ptr->pResponseQ      = me_ptr->response_q_ptr;
      mtmx_reconfig_msg_ptr->unClientToken = 0;
      mtmx_reconfig_msg_ptr->pBufferReturnQ = bufMgrNode.pReturnQ;
      mtmx_reconfig_msg_ptr->phPort   = &me_ptr->svc_handle;
      mtmx_reconfig_msg_ptr->unSessionID = me_ptr->session_id+1; //actual sesion id to be sent
      mtmx_reconfig_msg_ptr->direction  = ADM_TX_DIR;
      mtmx_reconfig_msg_ptr->num_channels = me_ptr->num_channels;
      mtmx_reconfig_msg_ptr->bits_per_sample  =  me_ptr->bits_per_sample;
      mtmx_reconfig_msg_ptr->bMtMxPassNativeNumChannels = FALSE;

      memscpy(mtmx_reconfig_msg_ptr->channel_mapping, sizeof(mtmx_reconfig_msg_ptr->channel_mapping),\
              channel_mapping, sizeof(channel_mapping));
      mtmx_reconfig_msg_ptr->buffering_ms  = NUM_BUFERS_LSM_AND_MTMX*MATRIX_OUT_FRAME_SIZE_5_MSEC;  //macro
      msg.unOpCode = ELITE_CUSTOM_MSG;
      msg.pPayload = (void*)mtmx_reconfig_msg_ptr;
    }
    break;

    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to send command to ADM, unexpected cmd: %lu",
            cmd);
      result = ADSP_EFAILED;
      msg.pPayload = (void *)bufMgrNode.pBuffer;
      goto __bailout;
    }

  }

  /* push it to ADM CMD Q */
  result = qurt_elite_queue_push_back(adm_cmd_q ,(uint64_t *)&msg);

  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Lpro[%d] failed to connect to ADM",
          me_ptr->session_id);
    return result;
  }

  /* wait for connect Ack from ADM svc */
  result = elite_svc_wait_for_ack(&msg);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Wait for ADMConnect ack failed");
    goto __bailout;
  }

  if (ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG != cmd ) 
  {
    /* check response result for diable, disconnect, connect commands */
    if(ADSP_EOK != adm_connect_msg_ptr->unResponseResult)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in ADM connect message Ack");
      result = adm_connect_msg_ptr->unResponseResult;
      goto __bailout;
    }
    /* For connect command ADM will give the handle and MTMX port id, which needs to be stored */
    /* These will be used to send START, PAUSE commands to MtMX Out port */
    if (ELITEMSG_CUSTOM_ADM_LSM_STREAM_CONNECT == cmd) 
    {
      me_ptr->mtmx_svc_handle = adm_connect_msg_ptr->phPort; 
      me_ptr->mtmx_out_port_id = adm_connect_msg_ptr->unSessionID;
      me_ptr->lsm_client_connection = LSM_CONNECTED_TO_ADM;
      /* MtMx always sends 5 msec of data. input_fram_size equals to 5msec samples of data */
      me_ptr->input_frame_size   = MATRIX_OUT_FRAME_SIZE_5_MSEC;
    }
  }
  else
  {   
    /* check response result for Reconfig message */
    if(ADSP_EOK != mtmx_reconfig_msg_ptr->unResponseResult)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in mtmx reconfig message Ack");
      result = mtmx_reconfig_msg_ptr->unResponseResult;
      goto __bailout;
    }
  }

  __bailout:
  if (ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT == cmd) 
  {
    me_ptr->lsm_client_connection = LSM_CONNECTED_TO_NO_CLIENT;
  }
  /* release custom message */
  elite_msg_release_msg(&msg);
  return result;
}


static ADSPResult listen_proc_send_cmd_to_mtmx(listen_proc_svc_t *me_ptr, uint32_t cmd)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg;
  int nActualSize = 0;
  EliteMsg_CustomMtMxRunType *mtmx_run_msg_ptr = NULL;
  EliteMsg_CustomMtMxPauseFlushType *mtmx_pause_msg_ptr = NULL;
  qurt_elite_queue_t *mtmx_cmd_q = me_ptr->mtmx_svc_handle->cmdQ;
  qurt_elite_bufmgr_node_t bufMgrNode;

  uint32_t reqd_payload_size = sizeof(EliteMsg_CustomMtMxPauseFlushType);

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Lpro[%d] Sending cmd 0x%x to MtMx",me_ptr->session_id,cmd);

  if (ELITEMSG_CUSTOM_MT_MX_RUN == cmd) 
  {
    reqd_payload_size = sizeof(EliteMsg_CustomMtMxRunType);
  }

  result = elite_mem_get_buffer(reqd_payload_size,
                                &bufMgrNode,
                                &nActualSize);

  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to create mesage result=0x%x",
          result);
    return result;
  }

  switch (cmd)
  {
    case ELITEMSG_CUSTOM_MT_MX_RUN:
    {
      mtmx_run_msg_ptr = (EliteMsg_CustomMtMxRunType*) bufMgrNode.pBuffer;
      memset(mtmx_run_msg_ptr, 0, sizeof(EliteMsg_CustomMtMxRunType));
      mtmx_run_msg_ptr->unSecOpCode = cmd;
      mtmx_run_msg_ptr->pResponseQ = me_ptr->response_q_ptr;
      mtmx_run_msg_ptr->unClientToken = 0;
      mtmx_run_msg_ptr->pBufferReturnQ = bufMgrNode.pReturnQ;
      mtmx_run_msg_ptr->unPortID = me_ptr->mtmx_out_port_id;
      mtmx_run_msg_ptr->unPortDirection = MATRIX_MIXER_PORT_DIR_OUTPUT;
      mtmx_run_msg_ptr->ullStartTime = 0;
      mtmx_run_msg_ptr->unStartFlag = 0;
      /* populate the msg with payload*/
      msg.unOpCode = ELITE_CUSTOM_MSG;
      msg.pPayload = (void*)mtmx_run_msg_ptr;
    }
    break;

    case ELITEMSG_CUSTOM_MT_MX_PAUSE:
    {
      mtmx_pause_msg_ptr = (EliteMsg_CustomMtMxPauseFlushType*) bufMgrNode.pBuffer;
      memset(mtmx_pause_msg_ptr, 0, sizeof(EliteMsg_CustomMtMxPauseFlushType));
      mtmx_pause_msg_ptr->unSecOpCode = cmd;
      mtmx_pause_msg_ptr->pResponseQ      = me_ptr->response_q_ptr;
      mtmx_pause_msg_ptr->unClientToken = 0;
      mtmx_pause_msg_ptr->pBufferReturnQ = bufMgrNode.pReturnQ;
      mtmx_pause_msg_ptr->unPortID   = me_ptr->mtmx_out_port_id;
      mtmx_pause_msg_ptr->unPortDirection = MATRIX_MIXER_PORT_DIR_OUTPUT;
      /* populate the msg with payload*/
      msg.unOpCode = ELITE_CUSTOM_MSG;
      msg.pPayload = (void*)mtmx_pause_msg_ptr;
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to send command to MtMx, unexpected cmd: %lu",
            cmd);
      result = ADSP_EFAILED;
      msg.pPayload = (void *)bufMgrNode.pBuffer;
      goto __bailout;
    }

  }
  /* push it to MTMX CMD Q */
  result = qurt_elite_queue_push_back(mtmx_cmd_q ,(uint64_t *)&msg);
  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Lpro[%d] failed to RUN/PAUSE to MTMX",
          me_ptr->session_id);
    return result;
  }
  /* wait for connect Ack from MTMX svc */
  result = elite_svc_wait_for_ack(&msg);
  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Wait for MTMX RUN/PAUSE Cmd ack failed");
    goto __bailout;
  }

  if (ELITEMSG_CUSTOM_MT_MX_RUN == cmd) 
  {
    if (ADSP_EOK != mtmx_run_msg_ptr->unResponseResult) 
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in MTMX RUN message Ack");
      result = mtmx_run_msg_ptr->unResponseResult;
      goto __bailout;
    }
  }
  else
  {
    if (ADSP_EOK != mtmx_pause_msg_ptr->unResponseResult) 
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in MTMX PAUSE message Ack");
      result = mtmx_pause_msg_ptr->unResponseResult;
      goto __bailout;
    }
  }

  __bailout:
  /* release custom message */
  elite_msg_release_msg(&msg);
  return result;
}

static ADSPResult listen_proc_reconnect_to_port(listen_proc_svc_t *me_ptr, uint16_t port_id)
{
  ADSPResult result = ADSP_EOK;
  if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
  {
    /* After ADM connect command success, send MTMX Reconfig command */
    result = listen_proc_send_cmd_to_adm(me_ptr,ELITEMSG_CUSTOM_ADM_LSM_STREAM_MTMX_RECONFIG);
    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "MtMX Reconfig command is failed, status:: %d",
            result);
      /* If reconfig is failed, send disconnect command */
      /* Failure occurs only if channle mapping info is wrong */
      if(ADSP_FAILED(result = listen_proc_send_cmd_to_adm(me_ptr,ELITEMSG_CUSTOM_ADM_LSM_STREAM_DISCONNECT)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Connect to ADM  is failed, status:: %d",
              result);
      }
      return result;
    }
  }
  /*ADM_PORT_ID == afe_port_id)*/
  else if(LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
  {
    /* check whether the dynamic svc is connected to  AFE  or not. */
    if(port_id == AFE_PORT_ID_INVALID)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "INVALID AFE PORT");
      return result;
    }
    if(ADSP_FAILED(result = listen_proc_disconnect_from_afe_cmd_sequece(me_ptr)))
    {
      return result;
    }

    /* Now send connect cmd to new AFE port id */
    result = listen_proc_send_cmd_to_afe(me_ptr,
                                         port_id,
                                         ELITEMSG_CUSTOM_AFECONNECT_REQ,
                                         NULL);
    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Connect to AFE port is failed, status::0x%x",
            result);
      return result;
    }
  }
  return result;
}


