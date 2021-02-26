/**
  @file ListenProcSvc.cpp
  @brief This file contains Listen processing service implementation. These
  include Session Variable management, Data/Command Queue Handling, etc.

  Object Oriented Concept is applied here, as in each Session can be considered
  as an Object, Session variables as Object Member Variables and Function
  implemented here as Object Member Functions.
*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*==============================================================================

  $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc.cpp#1 $

  Edit History

  when       who      what, where, why
  --------   ---      -------------------------------------------------------
  06/23/14   Unni     Added LSM topology support
  11/26/12   Sudhir   Initial version
==============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "ListenProcSvc_Includes.h"
#include "AFEInterface.h"
#include "EliteMsg_AfeCustom.h"
#include "ListenProcSvc_CustMsg.h"
#include "adsp_media_fmt.h"
#include "EliteTopology_db_if.h"
#include "ListenProcSvc_mmpm.h"
#include "EliteCmnTopology_db_if.h"
#include "adsp_core_api.h"
#include "Elite_fwk_extns_detection_engine.h"
#include "ListenProcSvc_Deprecate.h"

/*------------------------------------------------------------------------------
 * Constant / Define Declarations
 *----------------------------------------------------------------------------*/
static char THREAD_NAME[]="LPro";
#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

/*------------------------------------------------------------------------------
 * Local Function Declaration
 *----------------------------------------------------------------------------*/
static int listen_proc_svc_workloop(void* pInstance);
static int listen_proc_svc_process_cmd_q(listen_proc_svc_t* me_ptr);
static int listen_proc_svc_process_input_data_q(listen_proc_svc_t* me_ptr);

static inline void listen_proc_svc_destroy_cmd_q(listen_proc_svc_t* me_ptr);
static inline int listen_proc_svc_destroy_yourself(listen_proc_svc_t* me_ptr);
static inline void listen_proc_svc_destroy_input_data_q(listen_proc_svc_t* me_ptr);
static inline void listen_proc_svc_destroy_output_buf_q(listen_proc_svc_t* me_ptr);
static inline void listen_proc_svc_destroy_response_q(listen_proc_svc_t* me_ptr);
static inline ADSPResult listen_proc_svc_init_input_log(listen_proc_svc_t* me_ptr,uint32_t log_buf_size);
static inline ADSPResult listen_proc_svc_deinit_input_log(listen_proc_svc_t* me_ptr);
/*------------------------------------------------------------------------------
 * Static variables
 *----------------------------------------------------------------------------*/
typedef int (*qurt_elite_queue_handler)(listen_proc_svc_t* me_ptr);

/**< @brief Queue handlers table.*/
static qurt_elite_queue_handler pProcQHandler[LISTEN_PROC_NUM_Q]=
{
    listen_proc_svc_process_cmd_q,
    listen_proc_svc_process_input_data_q,
};

/*------------------------------------------------------------------------------
 * Function Definitions
 *----------------------------------------------------------------------------*/
/**
  Creates an instance of the  Listen Processing service.

  @param [in] init_param_ptr: Pointer to listen_proc_svc_init_params_t
  @param [out] handle_ptr_ptr: Service entry handle returned to the caller/LSM.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
 */
ADSPResult listen_proc_svc_create(const listen_proc_svc_init_params_t* init_param_ptr,
                                  void **handle)
{
  ADSPResult result = ADSP_EOK;
  char threadname[10];
  uint32_t stack_size;
  elite_cmn_topo_db_entry_t *topology_ptr = NULL;
  uint32_t actual_size = 0;
  uint32_t topo_handle = ELITE_CMN_TOPO_DB_INVALID_HANDLE;


  /* Sanity checks of input params */
  if (NULL == handle)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid input params!");
#endif
    return ADSP_EBADPARAM;
  }

  /* allocate instance struct */
  listen_proc_svc_t *me_ptr =
      (listen_proc_svc_t*) qurt_elite_memory_malloc(sizeof(listen_proc_svc_t),
                                                    QURT_ELITE_HEAP_DEFAULT);
  if (!me_ptr)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Failed to allocate memory for lpro svc instance!");
#endif
    *handle = NULL;
    return ADSP_ENOMEMORY;
  }

  /* zero out all the fields. */
  memset(me_ptr, 0, sizeof(listen_proc_svc_t));

  result = listen_proc_svc_create_svc_q(me_ptr);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Failed to create service Queues!!");
#endif
    *handle = NULL;
    goto __bail_out;
  }


  /* Make a copy of LSM call back structure */
  memscpy( &me_ptr->callback_data, sizeof(lsm_callback_handle_t),
           init_param_ptr->callback_data_ptr, sizeof(lsm_callback_handle_t));

  /* Fill the media  format and other initialization */
  me_ptr->sample_rate = init_param_ptr->sampling_rate;
  me_ptr->bits_per_sample = init_param_ptr->bits_per_sample;
  me_ptr->num_channels = init_param_ptr->num_channels;
  me_ptr->afe_port_id = AFE_PORT_ID_INVALID;
  me_ptr->state = LISTEN_PROC_STATE_STOP;
  me_ptr->session_id = init_param_ptr->session_id;
  me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;
  me_ptr->media_fmt.f.bits_per_sample = (me_ptr->bits_per_sample == 16) ? 16 : 32;
  me_ptr->media_fmt.f.num_channels = me_ptr->num_channels;
  me_ptr->media_fmt.f.sampling_rate= me_ptr->sample_rate;
  me_ptr->media_fmt.h.data_format= CAPI_V2_FIXED_POINT;
  me_ptr->media_fmt.f.bitstream_format = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;
  me_ptr->media_fmt.f.q_factor = (me_ptr->media_fmt.f.bits_per_sample == 16) ?
                         PCM_16BIT_Q_FORMAT : ELITE_32BIT_PCM_Q_FORMAT;
  me_ptr->media_fmt.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
  me_ptr->media_fmt.f.data_is_signed = 1;
  uint8_t i;
  for(i=0;i<me_ptr->num_channels;i++)
  {
   me_ptr->media_fmt.f.channel_type[i] = i;
  }
  /* Register unique MMPM request per session */
  result = lsm_mmpm_register(&me_ptr->mmpm_info, me_ptr->session_id);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro[%d] failed to resgister to mmpm",
          me_ptr->session_id);
#endif
    *handle = NULL;
    goto __bail_out;
  }


  // Search the topology definition in common data base, if not, search in old data base
  result = elite_cmn_topo_db_get_topology(1 << AVCS_TOPO_CFG_AREA_LSM_BIT,
                                          init_param_ptr->topology_id, &topology_ptr, &actual_size, &topo_handle);

  if(ADSP_FAILED(result) || 0 == actual_size || 0 == topo_handle ||
     ELITE_CMN_TOPO_DB_INVALID_HANDLE == topo_handle || NULL == topology_ptr)
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPro[%d]: Topology %lx is not present in "
          "common database", me_ptr->session_id, init_param_ptr->topology_id);
#endif
    result = ADSP_EFAILED;
    /* Deprecated Code */
    result = listen_proc_svc_get_topo_old_db(init_param_ptr->topology_id,me_ptr);
    /* End - Deprecated Code */
    if(ADSP_FAILED(result))
    {
      goto __bail_out;
    }
  }
  else
  {
    //Cross check whether actual_size returned from common database is valid
    uint32_t num_modules = topology_ptr->topo_def.num_modules;
    uint32_t size_req = (sizeof(elite_cmn_topo_db_entry_t) - sizeof(avcs_module_instance_info_t)) + (num_modules  *
        sizeof(avcs_module_instance_info_t));
    if(size_req != actual_size)
    {
#ifdef LSM_DEBUG
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPro[%d]: Topology %lx is present in common database."
            " However, actual_size %lu does not match size_req %lu",
            me_ptr->session_id, init_param_ptr->topology_id, actual_size, size_req);
#endif
      result = ADSP_EBADPARAM;
      elite_cmn_topo_db_release_topology(topo_handle);
      goto __bail_out;
    }

    // create the topology
    if(ADSP_EOK != (result = topo_create_common_db(&me_ptr->topo_handle, topology_ptr, lsm_capi_v2_event_callback,
                                                   me_ptr, NULL)))
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the topology for topology id %ld :%d",
            init_param_ptr->topology_id, result);
#endif
      elite_cmn_topo_db_release_topology(topo_handle);
      goto __bail_out;
    }
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LPro[%d]: topology creation for Topology id %lX is done successfully ", 
          me_ptr->session_id, init_param_ptr->topology_id);
#endif
    elite_cmn_topo_db_release_topology(topo_handle);
  }
  topo_send_output_media_fmt_t pParams;

  pParams.bits_per_sample = me_ptr->bits_per_sample;
  pParams.num_channels = me_ptr->num_channels;
  pParams.sampling_rate = me_ptr->sample_rate;
  for(i=0;i<me_ptr->num_channels;i++)
  {
    pParams.channel_mapping[i] = i;
  }
  result = topo_send_capiv2_output_media_format(me_ptr->topo_handle, &pParams);


  snprintf(threadname, 10,"%s%x",THREAD_NAME,me_ptr->session_id);

  stack_size = ((topo_t*)me_ptr->topo_handle)->stack_size;

  /* Launch the thread */
  result = qurt_elite_thread_launch(&me_ptr->svc_handle.threadId,
                                    threadname,
                                    NULL,
                                    stack_size,
                                    elite_get_thrd_prio(ELITETHREAD_DYNA_LSM_SVC_PRIO_ID),
                                    listen_proc_svc_workloop,
                                    (void*)me_ptr,
                                    QURT_ELITE_HEAP_DEFAULT);
  if(ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro[%d] failed to launch service thread!! Stack size = %lu",
          me_ptr->session_id, stack_size);
#endif
    *handle = NULL;
    goto __bail_out;
  }

  *handle = &(me_ptr->svc_handle);

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ListenProc[%d] creation success!! "
        "Stack size = %lu",
        me_ptr->session_id,
        stack_size);
  return ADSP_EOK;

__bail_out:
  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail:Creating topo:0x%x",
        result);
  listen_proc_svc_destroy_svc(me_ptr);
  return result;


}

/** Destroy the dynamic service

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/
void listen_proc_svc_destroy_svc(listen_proc_svc_t* me_ptr)
{
  if(!me_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Bad Param" );
    return;
  }

  /* Deregister from mmpm */
  lsm_mmpm_deregister(&me_ptr->mmpm_info);

  /* Destroy topology */
  topo_destroy((topo_handle_t)me_ptr->topo_handle, NULL);
  me_ptr->topo_handle = NULL;

  /* call utility function to destroy data Q */
  listen_proc_svc_destroy_input_data_q(me_ptr);

  /* call utility function to destroy output buffer Q */
  listen_proc_svc_destroy_output_buf_q(me_ptr);

  /* call utility function to destroy cmd Q */
  listen_proc_svc_destroy_cmd_q(me_ptr);

  listen_proc_svc_destroy_response_q(me_ptr);

  listen_proc_svc_deinit_input_log(me_ptr);

  qurt_elite_channel_destroy(&me_ptr->channel);

  /* free memory */
  if(NULL != me_ptr->output_buf_ptr)
  {
    qurt_elite_memory_free(me_ptr->output_buf_ptr);
    me_ptr->output_buf_ptr = NULL;
  }

  /* free memory */
  if(NULL != me_ptr->lab_struct_ptr)
  {
    qurt_elite_memory_free(me_ptr->lab_struct_ptr);
    me_ptr->lab_struct_ptr = NULL;
  }
  qurt_elite_memory_free(me_ptr);

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
      "LPro svc destroyed");
}

/*------------------------------------------------------------------------------
 * Static Function Definitions
 *----------------------------------------------------------------------------*/

/**
  Destroys internal input data queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/
static void listen_proc_svc_destroy_input_data_q(listen_proc_svc_t* me_ptr)
{
  (void)listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_INPUT_DATA_Q, ADSP_EOK);

  /* clean up data queue */
  if (me_ptr->svc_handle.dataQ)
  {
    /* destroy the q. */
    qurt_elite_queue_destroy(me_ptr->svc_handle.dataQ);
  }
}


static void listen_proc_svc_destroy_cmd_q(listen_proc_svc_t* me_ptr)
{
  /* this will be flushed if any commands are pushed on to cmdQ after sending close and before
     deregistering this dyn svc cmdQ with APR callback */
  (void)listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_CMD_Q, ADSP_EOK);

  /* clean up cmd queue */
  if (me_ptr->svc_handle.cmdQ)
  {
    /* destroy the q. */
    qurt_elite_queue_destroy(me_ptr->svc_handle.cmdQ);
  }
}

/**
  Destroys internal output buffer queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/

static void listen_proc_svc_destroy_output_buf_q(listen_proc_svc_t* me_ptr)
{
    // clean up buffer queue
   (void)listen_proc_svc_flush_q(me_ptr, LISTEN_PROC_OUTPUT_DATA_Q, ADSP_EOK);

   if (me_ptr->svc_handle.gpQ)
   {
      // destroy the q.
      qurt_elite_queue_destroy(me_ptr->svc_handle.gpQ);
   }
}



/**
  Destroys response queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/
static void listen_proc_svc_destroy_response_q(listen_proc_svc_t* me_ptr)
{
  /* clean up response  queue */
  if (me_ptr->response_q_ptr)
  {
    qurt_elite_queue_destroy(me_ptr->response_q_ptr);
  }
}

/**
  This function is the main work loop for the service. Commands are handled with
  the highest priority. Data processing is handled only when command queue is
  empty.

  @param pInstance [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static int listen_proc_svc_workloop(void* pInstance)
{
  listen_proc_svc_t *me_ptr = (listen_proc_svc_t*)pInstance;
  ADSPResult result=ADSP_EOK;

  me_ptr->curr_bit_field = LISTEN_PROC_CMD_SIG |LISTEN_PROC_DATA_SIG ;
  /*  Enter forever loop */
  for(;;)
  {
    uint32_t  nMask;
    /*  block on any one or more of selected queues to get a msg */
    me_ptr->channel_status = qurt_elite_channel_wait(&(me_ptr->channel),
                                                     me_ptr->curr_bit_field);
    for ( uint32_t i =0; i < LISTEN_PROC_NUM_Q; i++ )
    {
      nMask = 0x1 << (31 - i);
      if ( nMask & me_ptr->channel_status )
      {
        result = pProcQHandler[i](me_ptr);

        /* Service is destroyed, return immediately */
        if ( result == ADSP_ETERMINATED )
        {
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "WorkLoop terminated");
          return ADSP_EOK;
        }
      }
    }
  }
  return result;
}

ADSPResult listen_proc_cmd_q_apr_pkt_handler( listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (me_ptr->cmd_msg.pPayload);

  uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );

  switch ( ulOpCode )
  {
    case LSM_SESSION_CMD_SET_PARAMS_V2:
    case LSM_SESSION_CMD_SET_PARAMS_V3:
    {
      result   = listen_proc_svc_set_param_handler(me_ptr);
      break;
    }

    case LSM_SESSION_CMD_GET_PARAMS_V2:
    case LSM_SESSION_CMD_GET_PARAMS_V3:
    {
      result   = listen_proc_svc_get_param_handler(me_ptr);
      break;
    }

    case LSM_SESSION_CMD_START:
    {
      result =  listen_proc_svc_start_handler(me_ptr);
      break;
    }

    case LSM_SESSION_CMD_STOP:
    {
      result =  listen_proc_svc_stop_handler(me_ptr);
      break;
    }
    case LSM_SESSION_CMD_EOB :
    {
      result =  listen_proc_svc_eob_handler(me_ptr);
      break;
    }
    /* Deprecated */
    case LSM_SESSION_CMD_REGISTER_SOUND_MODEL:
    {
      result = listen_proc_svc_register_sound_model_handler(me_ptr);
      break;
    }
    case LSM_SESSION_CMD_DEREGISTER_SOUND_MODEL:
    {
      result = listen_proc_svc_deregister_sound_model_handler(me_ptr);
      break;
    }
    case LSM_SESSION_CMD_SET_PARAMS:
    {
      result = listen_proc_svc_set_param_v1_handler(me_ptr);
      break;
    }
    case LSM_SESSION_CMD_GET_PARAMS:
    {
      result = listen_proc_svc_get_param_v1_handler(me_ptr);
      break;
    }
    /* End - deprecated */
    default:
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM: unsupported command 0x%lx at port [0x%4lx]",
            ulOpCode, (uint32_t)elite_apr_if_get_dst_port( pPkt ) );
      result = ADSP_EUNSUPPORTED;
      /* Generate ACK and free apr packet */
      lsm_generate_ack( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0);

    }
  }

  return result;
}

/**
  This function processes commands from the LSM

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static int listen_proc_svc_process_cmd_q(listen_proc_svc_t* me_ptr)
{
  ADSPResult result=ADSP_EOK;

  /* Take next msg off the q */
  result = qurt_elite_queue_pop_front(me_ptr->svc_handle.cmdQ,
                                      (uint64_t*)&(me_ptr->cmd_msg) );

  /* Process the msg */
  if (ADSP_EOK == result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "ProcessCmdQueue[%d]: Message ID 0x%lu!!",
          me_ptr->session_id,me_ptr->cmd_msg.unOpCode);
    switch (me_ptr->cmd_msg.unOpCode)
    {

      case ELITE_APR_PACKET:
      {
        result = listen_proc_cmd_q_apr_pkt_handler(me_ptr);
        break;
      }
      case ELITE_CMD_DESTROY_SERVICE:
      {
        result = listen_proc_svc_destroy_yourself(me_ptr);
        break;
      }
      /* this will never trigger */
      default:
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Unsupported message ID 0x%lx!!",
              me_ptr->cmd_msg.unOpCode);
      }
      /*for apr packet apr ack will be send from the above function*/
      elite_msg_finish_msg(&me_ptr->cmd_msg, ADSP_EUNSUPPORTED);
      break;
    }
  }
  else
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPro[%d] Failed to dequeue command message!",
    		me_ptr->session_id);
#endif
  }

  return result;
}


/**
  This function processes input data media type

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @param data_media_ptr [in/out] This points to the data media type

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult static listen_proc_svc_process_media_type(listen_proc_svc_t *me_ptr,
                                elite_msg_data_media_type_apr_t *data_media_ptr)
{
  ADSPResult result=ADSP_EOK;
  uint32_t iter = 0;
  uint32_t frame_size_in_samples = 0;
  uint16_t bytes_per_sample = 0;
  uint32_t log_buf_size = 0;
  MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "LPro[%d] Received Media Format command",
      me_ptr->session_id);
  /* Checks for validating Media type got is inline Start Configuration.
     During Start, we have temporary buffer allocation based on
     - num_channels
     - bytes_per_channel
     - int_samples_per_period
     Need to ensure that media format got is in accordance to the configuration
     used during connection */
  if(ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM == data_media_ptr->unMediaFormatID)
  {
    elite_multi_channel_pcm_fmt_blk_t *fmt_ptr =
        (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(
                                                                data_media_ptr);

    if(me_ptr->bits_per_sample != fmt_ptr->bits_per_sample)
    {
#ifdef LSM_DEBUG
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LPro[%d] started with bits_per_sample=%d but media type got was=%d",
            me_ptr->session_id,me_ptr->bits_per_sample,fmt_ptr->bits_per_sample);
#endif
      result =  ADSP_EUNEXPECTED;
      goto __bailout;
    }
    if(me_ptr->num_channels != fmt_ptr->num_channels)
    {
#ifdef LSM_DEBUG
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LPro[%d] started with num_channels=%d but media type got was=%d",
            me_ptr->session_id,me_ptr->num_channels,fmt_ptr->num_channels);
#endif
      result =  ADSP_EUNEXPECTED;
      goto __bailout;
    }
    
    /* Translation from Elite media fmt to CAPI_V2 media fmt */
    me_ptr->media_fmt.f.bits_per_sample   = (fmt_ptr->bits_per_sample == 16)?16:32;
    me_ptr->media_fmt.f.num_channels      = fmt_ptr->num_channels;
    me_ptr->media_fmt.f.sampling_rate     = fmt_ptr->sample_rate;
    me_ptr->media_fmt.f.data_is_signed    = fmt_ptr->is_signed;
    me_ptr->media_fmt.f.bitstream_format  = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;
    me_ptr->media_fmt.f.q_factor          = (fmt_ptr->bits_per_sample == 16) ?
                                    PCM_16BIT_Q_FORMAT : ELITE_32BIT_PCM_Q_FORMAT;

    if(fmt_ptr->is_interleaved)
    {
      me_ptr->media_fmt.f.data_interleaving = CAPI_V2_INTERLEAVED;
    }
    else
    {
      me_ptr->media_fmt.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
    }
    for( iter=0 ; iter < LSM_MAX_NUM_CHANNELS ; iter++ )
    {
      me_ptr->media_fmt.f.channel_type[iter] = fmt_ptr->channel_mapping[iter];
    }
    me_ptr->media_fmt.h.data_format = CAPI_V2_FIXED_POINT;
  
  }
  else
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro[%d] Unknown Media Format Id [%x]",
          me_ptr->session_id,data_media_ptr->unMediaFormatID);
#endif
    result =  ADSP_EUNEXPECTED;
    goto __bailout;
  }
  frame_size_in_samples = (me_ptr->input_frame_size * me_ptr->media_fmt.f.sampling_rate)/1000;
  bytes_per_sample = (me_ptr->media_fmt.f.bits_per_sample>16)?4:2;
  log_buf_size = frame_size_in_samples*bytes_per_sample*me_ptr->media_fmt.f.num_channels;
  /* Intialise input logging*/ 
  listen_proc_svc_init_input_log(me_ptr,log_buf_size);

  result = topo_start((topo_handle_t)me_ptr->topo_handle,
                                     &me_ptr->media_fmt,
                                     frame_size_in_samples,
                                     NULL);
  if(ADSP_SUCCEEDED(result))
  {
    /* Set is_data_to_process for successful media format */
    me_ptr->is_data_to_process = TRUE;
  }

  return result;

  __bailout:
  me_ptr->is_data_to_process = FALSE;
  return result;
}

/**
  This function processes input data from AFE

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static int listen_proc_svc_process_input_data_q(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;

  /*Take next msg off the q*/
  result= qurt_elite_queue_pop_front(me_ptr->svc_handle.dataQ,
                                     (uint64_t*)&me_ptr->input_data_msg);
  if (ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LPro[%d] ProcessSourceInpDataQueue failed to dequeue input data msg! 0x%p",
          me_ptr->session_id, me_ptr->svc_handle.dataQ);
#endif
    me_ptr->input_data_msg.pPayload = NULL;
    return result;
  }

  if(!me_ptr->input_data_msg.pPayload)
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "LPro[%d] ProcessSourceInpDataQueue Input data msg payload is NULL!",
        me_ptr->session_id);
#endif
    elite_msg_release_msg( &me_ptr->input_data_msg );
    return ADSP_EBADPARAM;
  }

  switch (me_ptr->input_data_msg.unOpCode)
  {
    case ELITE_DATA_MEDIA_TYPE:
    {
      elite_msg_data_media_type_apr_t *data_media_ptr =
              (elite_msg_data_media_type_apr_t*)me_ptr->input_data_msg.pPayload;

      /* Process media type */
      result = listen_proc_svc_process_media_type(me_ptr, data_media_ptr);
      if(ADSP_FAILED(result))
      {
#ifdef LSM_DEBUG
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "LPro[%d] Failed to process data buffer result=0x%x",
              me_ptr->session_id, result);
#endif
      }

      result = elite_msg_finish_msg(&me_ptr->input_data_msg, result);
      me_ptr->input_data_msg.pPayload = NULL;
      break;
    }

    case ELITE_DATA_BUFFER:
    {
      /* if it is not in run state return the buffers  OR
         if data_to_process == FALSE return the buffers */
      if((LISTEN_PROC_STATE_RUN != me_ptr->state) ||
         (FALSE == me_ptr->is_data_to_process)      )
      {
#ifdef LSM_DEBUG
        /* Return payload buffer to buffer manger queue */
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "LPro[%d] Returning buffers without processing,"
              "Listen in STOP state or currently data is not processing: "
              "State:%d, Ready to process:%d",
              me_ptr->session_id, me_ptr->state,me_ptr->is_data_to_process);
#endif
        result = elite_msg_return_payload_buffer(&me_ptr->input_data_msg);
        me_ptr->input_data_msg.pPayload = NULL;
      }
      else
      {
        elite_msg_data_buffer_t* inp_data_buf_ptr =
                     (elite_msg_data_buffer_t*) me_ptr->input_data_msg.pPayload;

        if(!inp_data_buf_ptr->nActualSize)
        {
#ifdef LSM_DEBUG
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPro[%d] Got empty data buffer",
        		me_ptr->session_id);
#endif
          result = elite_msg_release_msg( &me_ptr->input_data_msg );
          me_ptr->input_data_msg.pPayload = NULL;
          break;
        }

        bool_t isTimeStampValid = (0 == GET_DATA_BUFFER_TIME_STAMP_BIT(inp_data_buf_ptr->nFlag)) ? FALSE : TRUE;
        /* Process directly on Elite Data Buffer, no buffering in Listen
           Service */
        result = listen_proc_svc_process(me_ptr,
                                         inp_data_buf_ptr->nActualSize,
                                         (int8_t *)&inp_data_buf_ptr->nDataBuf,
                                         inp_data_buf_ptr->ullTimeStamp,
                                         isTimeStampValid);
        if(ADSP_FAILED(result))
        {
#ifdef LSM_DEBUG
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "LPro[%d] Failed to process data buffer result=0x%x",
                me_ptr->session_id, result);
#endif
        }

        result = elite_msg_finish_msg(&me_ptr->input_data_msg, result);
        me_ptr->input_data_msg.pPayload = NULL;
      }
      break;
    }

    case ELITE_DATA_EOS:
    {
      MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "LPro[%d] Received EOS command",me_ptr->session_id);
      /*
       * After receiving EOS, don't process any new data.
       * EOS will come in two cases:
       * 1. When DC reset command send by listen proc, afe will send EOS
       * 2. When AFe port stops to which this is connected as client
       */
      me_ptr->is_data_to_process = FALSE;
      me_ptr->detection_status = LSM_DETECTION_STATUS_NONE;
      result = elite_msg_finish_msg(&me_ptr->input_data_msg, ADSP_EOK);
      me_ptr->input_data_msg.pPayload = NULL;
      break;
    }
    default:
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "LPro[%d] Unexpected Opcoded for input data msg 0x%lx",
            me_ptr->session_id, me_ptr->input_data_msg.unOpCode);
#endif
      result = elite_msg_finish_msg(&me_ptr->input_data_msg, ADSP_EUNEXPECTED);
      me_ptr->input_data_msg.pPayload = NULL;
      break;
    }
  }

  /* At this point, result should point to the error code corresponding how
     input_data_msg was released */
  if(ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "LPro[%d] Failed to release input data message %d",
          me_ptr->session_id, result);
  }
  return (result);
}

/**
  This function destroys the listen Processing service

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static int listen_proc_svc_destroy_yourself(listen_proc_svc_t* me_ptr)
{
  /* store the msg in temp variable, as part of free me_ptr will be destoryed and we cant guarantee
   the contents present in msg */
  elite_msg_any_t cmd_msg = me_ptr->cmd_msg;
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LProc[%d] DestroyYourself",me_ptr->session_id);

  if (LSM_CONNECTED_TO_AFE == me_ptr->lsm_client_connection)
  {
    /*not printing any failure messages,as this function has error messages*/
    /*we dont treat this error result anyway in this function*/
    listen_proc_disconnect_from_afe_cmd_sequece(me_ptr);
  }
  else if (LSM_CONNECTED_TO_ADM == me_ptr->lsm_client_connection)
    {
    /*not printing any failure messages,as this function has error messages*/
    /*we dont treat this error result anyway in this function*/
    listen_proc_disconnect_from_adm_cmd_sequece(me_ptr);
  }
  listen_proc_svc_destroy_svc(me_ptr);
  elite_msg_finish_msg(&cmd_msg, ADSP_EOK);

  return ADSP_ETERMINATED;
    }


static inline ADSPResult listen_proc_svc_init_input_log(listen_proc_svc_t* me_ptr,
                                    uint32_t log_buf_size)
{
  ADSPResult result = ADSP_EOK;

  if(NULL != me_ptr->input_log_buf_ptr)
  {
    if(log_buf_size != me_ptr->input_log_buf_size)
    {
      qurt_elite_memory_free(me_ptr->input_log_buf_ptr);
      me_ptr->input_log_buf_ptr = NULL;
    }
  }
  me_ptr->input_log_buf_size = log_buf_size;
  if(NULL == me_ptr->input_log_buf_ptr)
  {
    /* allocate instance struct */
    me_ptr->input_log_buf_ptr =
        (int8_t*) qurt_elite_memory_malloc(me_ptr->input_log_buf_size,
                                           QURT_ELITE_HEAP_DEFAULT);
  }
  if (NULL == me_ptr->input_log_buf_ptr)
    {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "Failed to allocate Log buffer memoty!");
#endif
    return ADSP_ENORESOURCE;
    }
  return result;
  }

static inline ADSPResult listen_proc_svc_deinit_input_log(listen_proc_svc_t* me_ptr)
{
  if(NULL != me_ptr->input_log_buf_ptr)
  {
    qurt_elite_memory_free(me_ptr->input_log_buf_ptr);
    me_ptr->input_log_buf_ptr = NULL;
  }
  return ADSP_EOK;
}

