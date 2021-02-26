
/*========================================================================

*//** @file voice_delivery.cpp

Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for the voice delivery service.
This is a dynamic service that serves as an adjunct for the voice timer
service. This service allows other dynamic services to queue up buffers
that have to be delivered at a particular time. At the correct time, VDS
will deliver these buffers at high priority in its context.
*//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  08/23/2012 ka      Created file
  ========================================================================== */


/* =======================================================================
   INCLUDE FILES FOR MODULE
   ========================================================================== */
#include "voice_delivery.h"
#include "avtimer_drv_api.h"
#include "VoiceCmnUtils.h"
#include "vds_clk_mgmt.h"
#include "cvd_diag_i.h"

//Global handle to VDS which is created during timer creation
extern elite_svc_handle_t *vds_handle;

#ifdef MDSPMODE
#if defined(__qdsp6__) && !defined(SIM)
#include "err.h"
#endif
#else
#include "adsp_err_fatal.h"
#endif
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

static const uint32_t VDS_CMD_MASK  = 0x00000001;
static const uint32_t VDS_DATA_MASK = 0x00000002;

/* We expect 4 clients for each call, i.e. vptx delivery, vprx delivery, venc delivery, and vdec pkt request
   Thus 30 clients can support upto 7 calls */
#define MAX_NUM_CLIENTS   30
#define VDS_DATAQ_SIZE    16
#define VDS_CMDQ_SIZE      8
#define VDS_STACK_SIZE  (1024*2 + 512)

#define DELIVERY_MISS_THRESHOLD 10

#define ENABLE_ENHANCED_CRASH_CODE 1

#define MAX_VFR_CYCLE 3

// save timestamp for last 10 misses
#define LAST_OVERSHOOT_SIZE 10

/*
 * additional clock frequency voted by VDS when boost is
 * requested
 */
#define VDS_CLK_BOOST_ADDER_KHZ  (115200)

extern uint32_t miss_threshold_sec[LAST_OVERSHOOT_SIZE-1];

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Data structures
 ** ----------------------------------------------------------------------- */

/** Structure for storing APR packet delivery request
 */
typedef struct vds_apr_request_t
{
   elite_apr_handle_t*     apr_handle_ptr;
   /**< Queue to push buffer to */
   elite_apr_packet_t*     apr_packet_ptr;
   /**< Pointer to elite message */
} vds_apr_request_t;

/** Structure for storing Elite buffer delivery request
 */
typedef struct vds_elite_request_t
{
   qurt_elite_queue_t      *peer_ptr;
   /**< Queue to pus buffer to */
   elite_msg_any_t         msg;
   /**< Elite message */
} vds_elite_request_t;

/** Structure for storing client request. Only one request per client can be queued
 */
typedef struct vds_client_request_t
{

   uint32_t active_flag;
   uint32_t request_type;
   union
   {
      vds_apr_request_t apr_request;
      vds_elite_request_t elite_request[MAX_VFR_CYCLE];
   } request_parameters;
} vds_client_request_t;

/** Structure representing VDS client
 */
typedef struct vds_client_t
{
   uint32_t               client_token;
   uint32_t               in_request_count;
   uint32_t               out_request_count;
   uint32_t               miss_count;
   uint32_t               miss_count_steady_state;       //steady state miss count calculated at delivery tick. This will increase count if client is not queueing data.
   uint32_t               miss_count_steady_state_queue; //steady state miss count calculated while queueing. This will increase count only when client pushes data after delivery miss.
   uint32_t               resync_pending_flag;
   qurt_elite_signal_t*   client_signal_ptr;
   qurt_elite_signal_t*   client_error_signal_ptr;
   vds_client_request_t   request;
   uint32_t               active_request_index;                 // to be updated by VDS for tracking how many requests are pending at any given time.
   bool_t                 delivery_miss_flag;
   uint64_t               req_begin_time;
   uint64_t               del_begin_time;
   uint32_t               overshoot_time_max;
   uint64_t               time_stamp_overshoot_max;
   uint32_t               undershoot_time_min;
   uint64_t               time_stamp_undershoot_min;
   uint32_t               overshoot_write_pos;
   uint64_t               last_overshoots[LAST_OVERSHOOT_SIZE];
   uint32_t               reinit_state;
   uint32_t               max_overshoot_time_possible;
   bool_t                 is_delivery_tick_reqd;
   bool_t                 is_tick_active;
   qurt_elite_signal_t*   resync_response_signal_ptr;
} vds_client_t;

/*
 * VDS recovers from boost state when successive N frames have under shoot
 * value greater than threshold for ALL THE VALID CLIENTS with VALID BUFFERS
 * Only VPRX and VENC threads fall in this category.
 */
#define VDS_REC_UNDERSHOOT_THRESH_US   (300)
#define VDS_REC_NUM_FRAME_THRESH      (200)

/*
 * Min and max values for thresholds. Limit the values received to avoid
 * permanent clock boost
 */
#define VDS_REC_UNDERSHOOT_THRESH_US_MAX   (1000) //1 msec
#define VDS_REC_UNDERSHOOT_THRESH_US_MIN   (300)  //0.3 msec

#define VDS_REC_NUM_FRAME_THRESH_MAX      (1000)  //10 sec for 2 clients
#define VDS_REC_NUM_FRAME_THRESH_MIN      (200)   //2  sec for 2 clients

#define IS_UNDERSHOOT_THRESH_VALID(x)  ( ((x) <= VDS_REC_UNDERSHOOT_THRESH_US_MAX) && ((x) >= VDS_REC_UNDERSHOOT_THRESH_US_MIN) )
#define IS_NUM_FRAME_THRESH_VALID(x)   ( ((x) <= VDS_REC_NUM_FRAME_THRESH_MAX) && ((x) >= VDS_REC_NUM_FRAME_THRESH_MIN) )

typedef enum vds_del_miss_response_t
{
   VDS_MISS_FORCE_NONE = 0,
   VDS_MISS_FORCE_CRASH,
   VDS_MISS_FORCE_CLK_BOOST,
}vds_del_miss_response_t;

typedef enum vds_state_t
{
   VDS_STATE_NOMINAL = 0,  //Nominal state
   VDS_STATE_BOOST,        //Boost is requested
} vds_boost_state_t;

typedef struct vds_rec_config_struct_t
{
   uint16_t                  rec_undershoot_thresh_us;
   uint16_t                  rec_num_frames_thresh;
}vds_rec_config_struct_t;

typedef struct vds_rec_info_struct_t
{
   uint32_t                  rec_num_frames;
   vds_rec_config_struct_t   rec_config;
}vds_rec_info_struct_t;

/** Main instance structure
 */
typedef struct vds_t
{
   elite_svc_handle_t        svc_handle;
   uint32_t                  instance_id;
   char_t                    dataq_name[QURT_ELITE_DEFAULT_NAME_LEN];
   char_t                    cmdq_name[QURT_ELITE_DEFAULT_NAME_LEN];
   qurt_elite_channel_t      channel;
   uint32_t                  wait_mask;
   uint32_t                  rcvd_mask;
   uint32_t                  bit_pos;
   uint32_t                  active_clients_mask;
   uint32_t                  active_clients_ctr;
   uint64_t                  steady_state_time_stamp;
   bool_t                    steady_state_flag;
   uint32_t                  reinit_ctr;
   vds_client_t              client_list[MAX_NUM_CLIENTS];
   vds_del_miss_response_t   del_miss_response_type;
   vds_boost_state_t         boost_state;
   vds_rec_config_struct_t   rec_config_cached;
   vds_rec_info_struct_t     rec_info;
   vcm_t                     *vcm_ptr;
   avtimer_drv_handle_t      avt_drv;
} vds_t;

//Need to be called form C file
extern "C" 
{
VOID_DIAG_PACKED_PTR vds_diag_cmd_processing(VOID_DIAG_PACKED_PTR req_pkt, uint16_t pkt_len);
}

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

static ADSPResult vds_destroy_yourself(void* instance_ptr, elite_msg_any_t* msg_ptr);
static void vds_destroy(vds_t* vds_ptr);
static ADSPResult vds_thread(void* instance_ptr);
static ADSPResult vds_cmd_handler(void* instance_ptr);
static ADSPResult vds_data_handler(void* instance_ptr);
static ADSPResult vds_custom_msg(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vds_subscribe(void *instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vds_unsubscribe(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vds_queue_request(void* instance_ptr, elite_msg_any_t*  msg_ptr);
static ADSPResult vds_resync(void* instance_ptr, elite_msg_any_t*  msg_ptr);
static ADSPResult vds_drop_now(void* instance_ptr, elite_msg_any_t*  msg_ptr);
static ADSPResult vds_delivery_handler(void* instance_ptr);
static ADSPResult vds_release_request(vds_client_t* client_ptr);
static void vds_release_client(void* instance_ptr, uint32_t client_id);
static ADSPResult vds_validate_client(void* instance_ptr, uint32_t client_id, uint32_t client_token);


//functions for delivery miss response (induced crash or core clock boost) handling
static void vds_reset_rec_info(vds_t *vds_ptr);
static void vds_rec_process_undershoot_duration(vds_t *vds_ptr, uint32_t undershoot_us);
static void vds_set_rec_config(vds_t *vds_ptr);
static void vds_init_del_miss_response(vds_t *vds_ptr);
static void vds_del_miss_force_crash(vds_t *vds_ptr);
static void vds_del_miss_force_boost(vds_t *vds_ptr);
static void vds_del_miss_response(vds_t *vds_ptr);
static void vds_enter_steady_state(vds_t *vds_ptr);
static void vds_exit_steady_state(vds_t *vds_ptr);

/* -----------------------------------------------------------------------
 ** Message handler
 ** ----------------------------------------------------------------------- */

// Function handler for channel
static ADSPResult (*vds_handler_ptr[])(void *) =
{
   vds_delivery_handler, //client id 0, MSB of channel
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler,
   vds_delivery_handler, //client id 29
   vds_data_handler,
   vds_cmd_handler //cmd handler, lsb of channel
};

// Function handler for FADD msgs
static elite_svc_msg_handler_func elite_handler_ptr[] =
{
   vds_custom_msg,                 //0  - ELITE_CUSTOM_MSG
   elite_svc_unsupported,          //1  - ELITE_CMD_START_SERVICE
   vds_destroy_yourself,           //2  - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,          //3  - ELITE_CMD_CONNECT
   elite_svc_unsupported,          //4  - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,          //5  - ELITE_CMD_PAUSE
   elite_svc_unsupported,          //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7  - ELITE_CMD_FLUSH
   elite_svc_unsupported,          //8  - ELITE_CMD_SET_PARAM
   elite_svc_unsupported,          //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   elite_svc_unsupported,          //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
   elite_svc_unsupported           //15 - ELITE_APR_PACKET_OPCODE
};

// Function handler for custom messages
static elite_svc_msg_handler_func vds_custom_msg_handler[VDS_MSGS_END] =
{
   vds_subscribe,
   vds_unsubscribe,
   vds_queue_request, //elite buffer delivery
   vds_queue_request, // APR packet delivery
   vds_resync,
   vds_drop_now
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
static void vds_reset_rec_info(vds_t *vds_ptr)
{
#ifdef VDS_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vds_reset_rec_info() resetting rec_num_frames");
#endif

   vds_ptr->rec_info.rec_num_frames = 0;
}

static void vds_rec_process_undershoot_duration(vds_t *vds_ptr, uint32_t undershoot_us)
{
#ifdef VDS_DEBUG
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vds_rec_process_undershoot_duration() undershoot_us(%lu) rec_num_frames(%lu) rec_num_frames_thresh(%lu)",
         undershoot_us, vds_ptr->rec_info.rec_num_frames, vds_ptr->rec_info.rec_config.rec_num_frames_thresh);
#endif

   //check if undershoot is greater than threshold
   if(undershoot_us > vds_ptr->rec_info.rec_config.rec_undershoot_thresh_us)
   {
      vds_ptr->rec_info.rec_num_frames++;

      if(vds_ptr->rec_info.rec_num_frames >= vds_ptr->rec_info.rec_config.rec_num_frames_thresh)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vds_rec_process_undershoot_duration() rec_num_frames(%lu) exceeds thresh (%lu). Requesting deboost",
               vds_ptr->rec_info.rec_num_frames, vds_ptr->rec_info.rec_config.rec_num_frames_thresh);
         vds_reset_rec_info(vds_ptr);
         vcm_deboost(vds_ptr->vcm_ptr);
         vds_ptr->boost_state = VDS_STATE_NOMINAL;
      }
   }
   else
   {
      //if undershoot is less than threshold then clear frame counter
      vds_reset_rec_info(vds_ptr);
   }
}

static void vds_set_rec_config(vds_t *vds_ptr)
{
   vds_ptr->rec_info.rec_config = vds_ptr->rec_config_cached;
   vds_reset_rec_info(vds_ptr);
}

static void vds_init_del_miss_response(vds_t *vds_ptr)
{
   //check what should be the response
   ADSPResult result = ADSP_EOK;

   //reset to NONE
   vds_ptr->del_miss_response_type = VDS_MISS_FORCE_NONE;

   if( ADSP_SUCCEEDED( result = vcm_init(&vds_ptr->vcm_ptr, &vds_ptr->avt_drv)) )
   {
      vds_ptr->del_miss_response_type = VDS_MISS_FORCE_CLK_BOOST;

      vds_ptr->boost_state = VDS_STATE_NOMINAL;

      vds_ptr->rec_config_cached.rec_num_frames_thresh = VDS_REC_NUM_FRAME_THRESH;
      vds_ptr->rec_config_cached.rec_undershoot_thresh_us = VDS_REC_UNDERSHOOT_THRESH_US;

      vds_set_rec_config(vds_ptr);
   }
   else
   {
      //if VCM boost is not successful fall-back to crash for internal builds
#ifdef MDSPMODE
#ifdef AUDIO_INTERNAL_FLAG
      //for MPSS internal builds enable crash code by default
      vds_ptr->del_miss_response_type = VDS_MISS_FORCE_CRASH;
#endif
#else
      //for ADSP builds enable crash code by default
      vds_ptr->del_miss_response_type = VDS_MISS_FORCE_CRASH;
#endif
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vds_init_del_miss_response() vds_miss response (%d)",
         vds_ptr->del_miss_response_type );

   return;
}

static void vds_del_miss_force_crash(vds_t *vds_ptr)
{

   /* crash is enabled for Internal MPSS and ADSP builds.
    * Use compatible err fatal functions to trigger crash. */
#ifdef MDSPMODE

//For MDSP builds call ERR_FATA to crash
#if defined(__qdsp6__) && !defined(SIM)
      ERR_FATAL("VCP: ERROR VDS delivery miss counter exceeded threshold.", 0, 0, 0);
#endif

#else
/**< In post-CS ADSP builds, AdspfatalerrApi(), implementation will be
* stubbed out, and call to this API is NOP, or does nothing.
* Whereas, in pre-CS builds, this API call forces the crash */

   char err_str[] ="VCP: ERROR VDS delivery miss counter exceeded threshold.";
   uint16_t err_str_len = strlen(err_str);

   AdspfatalerrApi(err_str,err_str_len);
#endif

   return;
}

static void vds_del_miss_force_boost(vds_t *vds_ptr)
{
   MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:vds_miss_force_boost(): VDS requesting for clock boost. curr state %lu",vds_ptr->boost_state);

   vcm_boost(vds_ptr->vcm_ptr, VDS_CLK_BOOST_ADDER_KHZ);

   vds_ptr->boost_state = VDS_STATE_BOOST;

   uint32_t mask = vds_ptr->active_clients_mask;

   while (mask != 0)
   {

      uint32_t client_id;
      client_id = voice_get_signal_pos(mask);

      vds_client_t *client_ptr = &vds_ptr->client_list[client_id];

      MSG_2( MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP:vds_miss_force_boost(): clearing overshoot value and counters for client %lx. client_id(%ld)",
            client_ptr->client_token, client_id);

      memset(client_ptr->last_overshoots,0,sizeof(client_ptr->last_overshoots));
      client_ptr->overshoot_write_pos = 0;
      client_ptr->miss_count_steady_state_queue = 0;
      // the counter below is delibrately commented to preseve the count through out the life of call.
      //client_ptr->miss_count_steady_state = 0;

      mask &= ~(1<<(31- client_id));
   }
}


static void vds_del_miss_response(vds_t *vds_ptr)
{
   //respond to VDS miss
   if(VDS_MISS_FORCE_CRASH == vds_ptr->del_miss_response_type)
   {
      vds_del_miss_force_crash(vds_ptr);
   }
   else if(VDS_MISS_FORCE_CLK_BOOST == vds_ptr->del_miss_response_type)
   {
      vds_del_miss_force_boost(vds_ptr);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: No action taken for vds miss. miss_respone(%d)", vds_ptr->del_miss_response_type);
   }

}

static void vds_enter_steady_state(vds_t *vds_ptr)
{
   //creating a function to handle everything associated with entering stead state
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS Steady State entered! ");
   vds_ptr->steady_state_flag = TRUE;

   //apply cached recover config data and reset state
   vds_set_rec_config(vds_ptr);
}

static void vds_exit_steady_state(vds_t *vds_ptr)
{
   //creating a function to handle everything associated with exiting stead state

   vds_ptr->steady_state_flag = FALSE;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vds_exit_steady_state(): VDS is exiting steady state ");

   //if we are in boost state, the call deboost
   if(VDS_STATE_BOOST == vds_ptr->boost_state)
   {
      vcm_deboost(vds_ptr->vcm_ptr);

      //clear recovery struct
      vds_reset_rec_info(vds_ptr);
   }

   vds_ptr->boost_state = VDS_STATE_NOMINAL;
}

ADSPResult vds_create(elite_svc_handle_t **svc_handle, uint32_t instance_id)
{
   ADSPResult result;
   vds_t *vds_ptr;
   char_t vds_thread_name[] = "VDS$";
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS create begin, instance(%x)", (int)instance_id);
   vds_ptr = (vds_t*)qurt_elite_memory_malloc(sizeof(vds_t), QURT_ELITE_HEAP_DEFAULT);
   if(!vds_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Malloc for VDS failed!");
      return ADSP_ENOMEMORY;
   }
   memset(vds_ptr, 0, sizeof(vds_t));
   vds_ptr->svc_handle.dataQ = (qurt_elite_queue_t*)qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VDS_DATAQ_SIZE),
         QURT_ELITE_HEAP_DEFAULT);
   vds_ptr->svc_handle.cmdQ = (qurt_elite_queue_t*)qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VDS_CMDQ_SIZE),
         QURT_ELITE_HEAP_DEFAULT);
   if((NULL == vds_ptr->svc_handle.cmdQ) || (NULL == vds_ptr->svc_handle.dataQ))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for VDS comdq and/or data q !!");
      vds_destroy(vds_ptr);
      return ADSP_ENOMEMORY;
   }
   qurt_elite_channel_init(&vds_ptr->channel);
   snprintf(vds_ptr->dataq_name, QURT_ELITE_DEFAULT_NAME_LEN, "vds_dataq%2x",(int)instance_id);
   snprintf(vds_ptr->cmdq_name, QURT_ELITE_DEFAULT_NAME_LEN, "vds_cmdq%2x",(int)instance_id);

   if(
         ADSP_FAILED(result = qurt_elite_queue_init(vds_ptr->cmdq_name, VDS_CMDQ_SIZE, vds_ptr->svc_handle.cmdQ)) ||
         ADSP_FAILED(result = qurt_elite_queue_init(vds_ptr->dataq_name, VDS_DATAQ_SIZE, vds_ptr->svc_handle.dataQ)) ||
         ADSP_FAILED(result = qurt_elite_channel_addq(&vds_ptr->channel, vds_ptr->svc_handle.cmdQ, VDS_CMD_MASK)) ||
         ADSP_FAILED(result = qurt_elite_channel_addq(&vds_ptr->channel, vds_ptr->svc_handle.dataQ, VDS_DATA_MASK))
     )
   {
      vds_destroy(vds_ptr);
      return result;
   }

   /* Launch the thread! */
   *svc_handle = &(vds_ptr->svc_handle);
   vds_thread_name[3] = ((instance_id + 48) & 0xff); //int to ascii conversion
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(vds_ptr->svc_handle.threadId), vds_thread_name, NULL,
               VDS_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_VOICE_DELIVERY_SVC_PRIO_ID), vds_thread, (void*)vds_ptr, QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error! VDS thread creation failed, instance(%x)", (int)instance_id);
      vds_destroy(vds_ptr);
      *svc_handle = NULL;
      return result;
   }

   //prepares when need to be done when VDS misses are observed
   vds_init_del_miss_response(vds_ptr);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS create end, instance(%x)", (int)instance_id);

   return ADSP_EOK;
}

/* Destroy routine. This routine only works if there are no active clients
   It is the creators responsibility to ensure that the above condition is met
   before issuing the destroy command */
static ADSPResult vds_destroy_yourself(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   if(vds_ptr->active_clients_mask)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Unable to destroy VDS, active clients present %x, instance(%x)", (int)vds_ptr->active_clients_mask, (int)vds_ptr->instance_id);
      elite_svc_send_ack(msg_ptr, ADSP_EFAILED);
      return ADSP_EFAILED;
   }
   vds_destroy(vds_ptr);
   elite_svc_send_ack(msg_ptr, ADSP_EOK);
   return ADSP_ETERMINATED;
}

static void vds_destroy(vds_t* vds_ptr)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS destroy begin, instance(%x)", (int)vds_ptr->instance_id);
   /* Since there are no active clients, we need not handle any signal frees or pending data queue items.
      Additionally since data handler has priority over command handler, anything in the data queue before the
      destroy was received should have already been processed. */
   if(NULL != vds_ptr->svc_handle.dataQ)
   {
      /* The VDS data queue is actually more like a command queue, since it handles only custom messages
         Thus we still use deinit cmd queue. Deinit data queue will not work correctly here, since it only
         works with ELITE_DATA_BUFFER type messages */
      elite_svc_deinit_cmd_queue(vds_ptr->svc_handle.dataQ);
      qurt_elite_memory_free(vds_ptr->svc_handle.dataQ);
      vds_ptr->svc_handle.dataQ = NULL;
   }
   if(NULL != vds_ptr->svc_handle.cmdQ)
   {
      elite_svc_deinit_cmd_queue(vds_ptr->svc_handle.cmdQ);
      qurt_elite_memory_free(vds_ptr->svc_handle.cmdQ);
      vds_ptr->svc_handle.cmdQ = NULL;
   }
   if(vds_ptr->vcm_ptr)
   {
      vcm_deinit(vds_ptr->vcm_ptr);
   }

   /* Finally free the instance structure */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS destroy end, instance(%x)", (int)vds_ptr->instance_id);
   qurt_elite_memory_free(vds_ptr);
   return;
}

static ADSPResult vds_thread(void* instance_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   vds_ptr->wait_mask = VDS_CMD_MASK|VDS_DATA_MASK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Entering VDS workloop, instance (%x)", (int)vds_ptr->instance_id);
   while(1)
   {
      vds_ptr->rcvd_mask = qurt_elite_channel_wait(&vds_ptr->channel, vds_ptr->wait_mask);
      while(vds_ptr->rcvd_mask)
      {
         vds_ptr->bit_pos = voice_get_signal_pos(vds_ptr->rcvd_mask);
         result = vds_handler_ptr[vds_ptr->bit_pos](vds_ptr);
         if(ADSP_FAILED(result))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: VDS handler error! Bit pos %x, result %d, instance(%x)", (int)vds_ptr->bit_pos, (int)result, (int)vds_ptr->instance_id);
         }
         if(ADSP_ETERMINATED == result)
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS thread exiting");
            return ADSP_EOK;
         }
         vds_ptr->rcvd_mask = qurt_elite_channel_poll(&vds_ptr->channel, vds_ptr->wait_mask);
      }
   }
}

static ADSPResult vds_cmd_handler(void* instance_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   elite_msg_any_t msg;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vds_cmd_handler begin, instance(%x)", vds_ptr->instance_id);

   // Take next msg off the cmdQ
   result = qurt_elite_queue_pop_front(vds_ptr->svc_handle.cmdQ, (uint64_t*)&msg);
   // Process the msg if success
   if (ADSP_EOK == result)
   {
      uint32_t cmd_table_size = sizeof(elite_handler_ptr)/sizeof(elite_handler_ptr[0]);
      if (msg.unOpCode >= cmd_table_size)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8lx!!", msg.unOpCode);
         return ADSP_EUNSUPPORTED;
      }
      //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VDS cmd handler, Opcode=%d",msg.unOpCode);
      result = elite_handler_ptr[msg.unOpCode](instance_ptr, &msg);
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vds_cmd_handler end, result = %d", instance(%x)", result, vds_ptr->instance_id);
   return result;
}

/* This data handler is not the type that is typically implemented by other dynamic services.
   Those data handlers correspond to the ELITE_DATA_BUFFER primary opcode. This on the other hand is
   still dealing with custom messages. In that sense, the data and command handler are exactly identical,
   except acks are not sent for data. They also share a common table of secondary opcode handlers,
   of which some are used by cmd handler and the others by data handler */
static ADSPResult vds_data_handler(void* instance_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   elite_msg_any_t msg;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vds_data_handler begin, instance(%x)", vds_ptr->instance_id);

   // Take next msg off the cmdQ
   result = qurt_elite_queue_pop_front(vds_ptr->svc_handle.dataQ, (uint64_t*)&msg);
   // Process the msg if success
   if (ADSP_EOK == result)
   {
      uint32_t cmd_table_size = sizeof(elite_handler_ptr)/sizeof(elite_handler_ptr[0]);
      if (msg.unOpCode >= cmd_table_size)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8lx!!", msg.unOpCode);
         return ADSP_EUNSUPPORTED;
      }
      //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VDS data handler, Opcode=%d",msg.unOpCode);
      result = elite_handler_ptr[msg.unOpCode](instance_ptr, &msg);
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vds_data_handler end, result = %d", instance(%x)", result, vds_ptr->instance_id);
   return result;
}

static ADSPResult vds_custom_msg(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   //vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   elite_msg_custom_header_t *custom_ptr = (elite_msg_custom_header_t *) msg_ptr->pPayload;

   if( custom_ptr->unSecOpCode >= VDS_MSGS_END)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported custom message ID 0x%8lx!!", custom_ptr->unSecOpCode);
      return ADSP_EUNSUPPORTED;
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VDS custom cmd handler, Opcode=%d, instance(%x)",custom_ptr->unSecOpCode, (int)vds_ptr->instance_id);
   result = vds_custom_msg_handler[custom_ptr->unSecOpCode](instance_ptr, msg_ptr);
   return result;
}

/* Custom handler for secondary opcode VDS_SUBSCRIBE */
static ADSPResult vds_subscribe(void *instance_ptr, elite_msg_any_t* msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   elite_msg_custom_vds_sub_type* sub_msg_ptr = (elite_msg_custom_vds_sub_type*)msg_ptr->pPayload;
   vds_sub_t* sub_params = sub_msg_ptr->sub_req_ptr;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS subscribe begin, token 0x%x, instance(%x)", (int)sub_params->client_token, (int)vds_ptr->instance_id);
   /* Find the first available bit */
   uint32_t client_id = voice_get_signal_pos(~vds_ptr->active_clients_mask);
   if(MAX_NUM_CLIENTS <= client_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VDS unable to accept more clients, token 0x%x, instance(%x)", (int)sub_params->client_token, (int)vds_ptr->instance_id);
      elite_svc_send_ack(msg_ptr, ADSP_ENORESOURCE);
      return ADSP_EFAILED;
   }
   /* Mark client active and update counter along with steady state flag */
   vds_ptr->active_clients_mask = voice_set_bit(vds_ptr->active_clients_mask, (31 - client_id));
   if(4 == vds_ptr->active_clients_ctr++)
   {
      vds_exit_steady_state(vds_ptr);
   }
   vds_client_t* client_ptr = &vds_ptr->client_list[client_id];
   /* Allocate signal */
   qurt_elite_signal_t* signal_ptr = (qurt_elite_signal_t*)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   if(NULL == signal_ptr)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS signal allocation failed, token 0x%x, instance(%x)", (int)sub_params->client_token, (int)vds_ptr->instance_id);
      vds_release_client(vds_ptr, client_id);
      elite_svc_send_ack(msg_ptr, ADSP_ENOMEMORY);
      return ADSP_ENOMEMORY;
   }
   /* Initialize signal and add to channel */
   if(
         ADSP_FAILED(result = qurt_elite_signal_init(signal_ptr)) ||
         ADSP_FAILED(result = qurt_elite_channel_add_signal(&vds_ptr->channel, signal_ptr, (1<<(31-client_id))))
     )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS signal init failed, token 0x%x, instance(%x)", (int)sub_params->client_token, (int)vds_ptr->instance_id);
      vds_release_client(vds_ptr, client_id);
      elite_svc_send_ack(msg_ptr, result);
      //fix for KW warning. proper error handling by deallocating memory.
      qurt_elite_memory_free(signal_ptr);
      return result;
   }
   client_ptr->client_token = sub_params->client_token;
   client_ptr->is_delivery_tick_reqd = sub_params->is_delivery_tick_reqd;
   client_ptr->client_error_signal_ptr = sub_params->error_signal_ptr;
   client_ptr->resync_response_signal_ptr = sub_params->resync_response_signal_ptr;
   /* Populate params to be returned to client */
   sub_params->client_id = client_id;
   sub_params->signal_ptr = signal_ptr;
   client_ptr->client_signal_ptr = signal_ptr;
   client_ptr->max_overshoot_time_possible = 10000; //Use some max value.
   /* Add to wait mask */
   vds_ptr->wait_mask = voice_set_bit(vds_ptr->wait_mask, 31-client_id);
   /* Send ack back */
   elite_svc_send_ack(msg_ptr, ADSP_EOK);
   /* if this is first client then register with avtimer */
   if( vds_ptr->active_clients_ctr == 1 )
   {
	  avtimer_open_param_t open_param;
	  open_param.client_name = (char*)"VDS"; // use the same client name as VTM to avoid extra registrations
	  open_param.flag = 0;
	  if (ADSP_EOK != (result = avtimer_drv_hw_open(&(vds_ptr->avt_drv), &open_param)))
	  {
	     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to Open AVTimer Driver for VDS");
	     vds_ptr->avt_drv = NULL;
	     return ADSP_EFAILED;
	  }
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS subscribe end");
   return ADSP_EOK;
}

/* Custom handler for secondary opcode VDS_UNSUBSCRIBE */
static ADSPResult vds_unsubscribe(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   elite_msg_custom_vds_sub_type* sub_msg_ptr = (elite_msg_custom_vds_sub_type*)msg_ptr->pPayload;
   vds_sub_t* sub_params = sub_msg_ptr->sub_req_ptr;
   uint32_t client_id = sub_params->client_id;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS unsubscribe begin, token 0x%x, instance(%x)", (int)sub_params->client_token, (int)vds_ptr->instance_id);
   /* Validate client */
   result = vds_validate_client(vds_ptr, client_id, sub_params->client_token);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS client invalid, result %d, instance(%x)", (int)result, (int)vds_ptr->instance_id);
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }
   /* We are now good, free any outstanding requests from client */
   vds_client_t* client_ptr = &vds_ptr->client_list[client_id];
   /* Print out client statistics */
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS Requests received: %d, Requests delivered: %d", (int)client_ptr->in_request_count, (int)client_ptr->out_request_count);
   // if usecase was only one call.
   // if anyone of the 4 vds clients unsubscribes, , reset steady state flag, to avoid false overshoot and undershoot times in cases like
   // handovers etc and decrease re-init counter for each client unsubscription.
   // reinit_state holds the bumber of buffers a client is delivered in transient state. In steady state is value is 2
   // reinit_ctr holds accumulate value of every clients state
   if (4 == vds_ptr->active_clients_ctr--)
   {
      vds_exit_steady_state(vds_ptr);
   }
   vds_ptr->reinit_ctr -= client_ptr->reinit_state;
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vds_unsubscribe : steady_state miss count (%u) Max Overshoot is (%lu), Min Undershoot is (%lu), client token 0x%x,reinit_ctr %ld instance(%x)", client_ptr->miss_count_steady_state,client_ptr->overshoot_time_max,client_ptr->undershoot_time_min,(int)client_ptr->client_token,vds_ptr->reinit_ctr,(int)vds_ptr->instance_id);

   vds_release_request(client_ptr);
   /* Finally release client */
   vds_release_client(vds_ptr, client_id);
   /* Send ack back */
   elite_svc_send_ack(msg_ptr, ADSP_EOK);
   // de-register from avtimer if no vds clients are subscribed
   if (vds_ptr->active_clients_ctr == 0)
   {
      avtimer_drv_hw_close(vds_ptr->avt_drv);
      vds_ptr->avt_drv = NULL;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS unsubscribe end");
   return ADSP_EOK;
}

/* Custom handler for secondary opcode VDS_DELIVER_ELITE_BUFFER and VDS_DELIVER_APR_PACKET */
static ADSPResult vds_queue_request(void* instance_ptr, elite_msg_any_t*  msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result;
   uint32_t client_id, client_token, buffer_id;
   uint64_t delivery_timestamp = 0;
   elite_msg_custom_vds_deliver_elite_type* elite_msg_ptr = (elite_msg_custom_vds_deliver_elite_type*)msg_ptr->pPayload;
   vds_deliver_elite_msg_t* elite_request_ptr = &elite_msg_ptr->delivery_request;

   elite_msg_custom_vds_deliver_apr_type* apr_msg_ptr = (elite_msg_custom_vds_deliver_apr_type*)msg_ptr->pPayload;
   vds_deliver_apr_packet_t* apr_request_ptr = &apr_msg_ptr->delivery_request;

   if(VDS_DELIVER_ELITE_BUFFER == elite_msg_ptr->unSecOpCode)
   {
      client_id = elite_request_ptr->client_id;
      client_token = elite_request_ptr->client_token;
      buffer_id = elite_request_ptr->buffer_id;
   }
   else if(VDS_DELIVER_APR_PACKET == elite_msg_ptr->unSecOpCode)
   {
      client_id = apr_request_ptr->client_id;
      client_token = apr_request_ptr->client_token;
      delivery_timestamp = apr_request_ptr->delivery_timestamp;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS request has invalid opcode %x, instance(%x)",
            (int)elite_msg_ptr->unSecOpCode, (int)vds_ptr->instance_id);
      elite_msg_return_payload_buffer(msg_ptr);
      return ADSP_EBADPARAM;
   }
   result = vds_validate_client(vds_ptr, client_id, client_token);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS client invalid, result %d, instance(%x)", (int)result, (int)vds_ptr->instance_id);
      elite_msg_return_payload_buffer(msg_ptr);
      return result;
   }

   vds_client_t *client_ptr = &vds_ptr->client_list[client_id];
   vds_client_request_t *request_ptr = &client_ptr->request;

   // Get the time stamp when buffer is queued to vds,
   if( (VDS_DELIVER_ELITE_BUFFER == elite_msg_ptr->unSecOpCode) )
   {
      if(VDS_UNTIMED_BUFFER != buffer_id )
      {
         client_ptr->req_begin_time = avtimer_drv_get_time( vds_ptr->avt_drv );
      }
#ifdef VDS_DEBUG
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VDS queue request begin UNTIMED buffer id. Not updating req_begin_time, client_token %lx", (int)client_token);
      }
#endif
   }
   else if(VDS_DELIVER_APR_PACKET == elite_msg_ptr->unSecOpCode)
   {
      client_ptr->req_begin_time = avtimer_drv_get_time( vds_ptr->avt_drv );
   }

   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS queue timestamp %lld",client_ptr->req_begin_time);

   if(TRUE == client_ptr->is_delivery_tick_reqd)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS queue request begin, token %x instance(%x)", (int)client_token, (int)vds_ptr->instance_id);
   }
   else if (delivery_timestamp != 0)
   {
      client_ptr->del_begin_time =  delivery_timestamp;
      if ( client_ptr->req_begin_time > delivery_timestamp )
      {
         client_ptr->delivery_miss_flag = TRUE;
      }
   }
   else
   {
      client_ptr->del_begin_time = 0;
   }

   //  if miss flag is TRUE that means vds delivery tick for this client
   // has already been triggered but client could not queue buffer in time.
   // So, calculate overshoot time i:e by how much time client got delayed.
   // if it is more than previous max overshoot time then update the maximum overshoot time for that client.

   if (TRUE == client_ptr->delivery_miss_flag)
   {
      uint32_t time = client_ptr->req_begin_time-client_ptr->del_begin_time;
      if (time > client_ptr->overshoot_time_max)
      {
         if(time > client_ptr->max_overshoot_time_possible)
         {
              MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vds queue request : overshoot time(%lu) exceed permissible value(%lu). Ignoring. client token 0x%x, instance(%x)",
                    time,client_ptr->max_overshoot_time_possible,(int)client_ptr->client_token, (int)vds_ptr->instance_id);
         }
         else
         {
           client_ptr->overshoot_time_max = time;
           client_ptr->time_stamp_overshoot_max = client_ptr->req_begin_time;
         }
         // if our voting is correct this should not get hit in steady state. Print a msg for debugging.
      }
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vds queue request : curr(%lu)/max(%lu) overshoot in usec. client token 0x%x, instance(%x)",
            time,client_ptr->overshoot_time_max,(int)client_ptr->client_token, (int)vds_ptr->instance_id);

      if(vds_ptr->steady_state_flag)
      {
         //save timestamp for miss
         client_ptr->last_overshoots[client_ptr->overshoot_write_pos] = client_ptr->del_begin_time;
         client_ptr->overshoot_write_pos++;
         if(LAST_OVERSHOOT_SIZE <= client_ptr->overshoot_write_pos)
         {
            client_ptr->overshoot_write_pos = 0;
         }

         //increment steady state miss count at queue. It might happen that because of some reason client is not queuing the data but del miss is detected
         client_ptr->miss_count_steady_state_queue++;
      }

     /*
      * Moving crash code to queue to get last overshoot time as well and also to avoid crash when client is not able to push data.
      */
#ifndef ENABLE_ENHANCED_CRASH_CODE
      if (DELIVERY_MISS_THRESHOLD < client_ptr->miss_count_steady_state)
      {
         //call error handler
         MSG_3( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VDS delivery miss counter exceeded threshold. It is not expected. VDS delivery_miss_count=(%ld), client_token = 0x%x, del_miss_response(%d)"
                                                          , client_ptr->miss_count_steady_state, (int)client_ptr->client_token,vds_ptr->del_miss_response_type);

         vds_del_miss_response(vds_ptr);
      }
#else
      /*
       * enhanced crash code. The goal is to
       * 1. detect if the misses are happening in a burst(2-3 misses in 5sec). If they are, then it is because of some aperiodic thread in the systems hogging cpu. Crash immediately to catch such cases.
       * 2. Also early detection of scenarios where misses are not in a bust but are periodic (1 misses every 10sec).
       * 3. If the misses are far apart, then dont crash.
       */

       //run this code only when we have two or more misses
       if(client_ptr->miss_count_steady_state_queue>1)
       {
          int32_t read_pos = client_ptr->overshoot_write_pos-1;
          uint64_t curr_miss_time, prev_miss_time,time_diff_usec,threshold_usec;

          //read curr miss timestamp
          if(read_pos<0)  read_pos+=LAST_OVERSHOOT_SIZE;
          curr_miss_time = client_ptr->last_overshoots[read_pos--];

          //read previos miss timestamp
          if(read_pos<0)  read_pos+=LAST_OVERSHOOT_SIZE;
          prev_miss_time = client_ptr->last_overshoots[read_pos--];

          //if the difference between two successive miss timestamps is > max overshoot threshold, then dont need to go run this logic
          if( (curr_miss_time-prev_miss_time) <=  (miss_threshold_sec[LAST_OVERSHOOT_SIZE-2]*1000000))
          {
             for(uint32_t i=1; (i<LAST_OVERSHOOT_SIZE) && (i<client_ptr->miss_count_steady_state_queue); i++)
             {
                //time difference in microsec between curr miss and ith miss
                time_diff_usec = curr_miss_time-prev_miss_time;
                threshold_usec = miss_threshold_sec[i-1]*1000000;

                //crash if the difference is less than expected threshold
                if(time_diff_usec <= threshold_usec)
                {
                   MSG_3( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VDS %d misses happened in %lu usec not expected.  client_token = 0x%x, Taking response action!!"
                               , i+1,(long int)time_diff_usec, (int)client_ptr->client_token);

                   vds_del_miss_response(vds_ptr);
                   break;
                }

                //read ith miss
                if(read_pos<0)  read_pos+=LAST_OVERSHOOT_SIZE;
                prev_miss_time = client_ptr->last_overshoots[read_pos--];
             }
          }
       }
#endif
      client_ptr->delivery_miss_flag = FALSE;
   }
   /* Store the request */
   if(VDS_DELIVER_ELITE_BUFFER == elite_msg_ptr->unSecOpCode)
   {
      if(VDS_INACTIVE_BUFFER == buffer_id)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Received invalid buffer from client. Dropping request. token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         elite_msg_return_payload_buffer(msg_ptr);
         return ADSP_EOK;
      }
      /* Peer pointer cant be NULL */
      if(NULL == elite_request_ptr->peer_ptr)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS request has NULL pointers, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         elite_msg_return_payload_buffer(msg_ptr);
         return ADSP_EBADPARAM;
      }
      /* if timed buffer delivery is not yet done and another similar request comes then it is resync case */
      if (VDS_TIMED_BUFFER == buffer_id )
      {
         if ( client_ptr->is_tick_active )
         {
            vds_release_request(client_ptr);
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS dropping existing buffers because of resync");
         }
         client_ptr->is_tick_active = TRUE;
      }
      /* Drop last buffer request if, maximum requests are already stored */
      if ( client_ptr->active_request_index >= MAX_VFR_CYCLE )
      {
         client_ptr->active_request_index = MAX_VFR_CYCLE-1;
         elite_msg_return_payload_buffer(&request_ptr->request_parameters.elite_request[MAX_VFR_CYCLE-1].msg);
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS dropping existing buffer as max requests reached");
      }
      /* Now we can store the request */
      request_ptr->active_flag = TRUE;
      request_ptr->request_type = VDS_DELIVER_ELITE_BUFFER;
      request_ptr->request_parameters.elite_request[client_ptr->active_request_index].peer_ptr = elite_request_ptr->peer_ptr;
      /* Copy over message. Note that this is only 8 bytes. The reason for not transferring
         a pointer instead is that most of the times the message is on the originating threads stack.
         Since the originating thread does not wait for acks for delivery requests, this is a problem
         as the stack contents can change by the time VDS thread actually reads the message.*/
      request_ptr->request_parameters.elite_request[client_ptr->active_request_index].msg = elite_request_ptr->msg;
      client_ptr->active_request_index++;
   }
   else if(VDS_DELIVER_APR_PACKET == elite_msg_ptr->unSecOpCode)
   {
      if(request_ptr->active_flag)
      {
         /* There is an active request that needs to be dropped. This is typically an error scenario */
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS dropping existing request, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         vds_release_request(client_ptr);
      }
      /* Convert to correct data type */
      /* Neither pointer can be NULL */
      if((NULL == apr_request_ptr->apr_handle_ptr) || (NULL == apr_request_ptr->apr_packet_ptr))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS request has NULL pointers, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         elite_msg_return_payload_buffer(msg_ptr);
         return ADSP_EBADPARAM;
      }
      /* Now we can store the request */
      request_ptr->active_flag = TRUE;
      request_ptr->request_type = VDS_DELIVER_APR_PACKET;
      request_ptr->request_parameters.apr_request.apr_handle_ptr = apr_request_ptr->apr_handle_ptr;
      request_ptr->request_parameters.apr_request.apr_packet_ptr = apr_request_ptr->apr_packet_ptr;
   }

   /* Increment count of incoming requests since everything was successful */
   client_ptr->in_request_count++;
   elite_msg_return_payload_buffer(msg_ptr);
   // dbg: MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS queue request end, instance(%x)", (int)vds_ptr->instance_id);

   if(client_ptr->resync_pending_flag)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS dropping request as part of resync while queuing, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
      vds_release_request(client_ptr);
      client_ptr->resync_pending_flag = FALSE;
      client_ptr->request.active_flag = FALSE;
      if(client_ptr->resync_response_signal_ptr)
      {
         qurt_elite_signal_send(client_ptr->resync_response_signal_ptr);
      }
   }
   if((FALSE == client_ptr->is_delivery_tick_reqd)||((FALSE == client_ptr->is_tick_active )&&(VDS_DELIVER_ELITE_BUFFER == client_ptr->request.request_type )))
   {
       qurt_elite_signal_send(client_ptr->client_signal_ptr);     // for vptx delivery enable signal as soon as buffer is queued
   }
   return ADSP_EOK;
}

static ADSPResult vds_resync(void* instance_ptr, elite_msg_any_t*  msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_vds_resync_type* resync_msg_ptr = (elite_msg_custom_vds_resync_type*)msg_ptr->pPayload;
   vds_resync_t* resync_params_ptr = &resync_msg_ptr->resync_params;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS resync, token %x instance(%x)", (int)resync_params_ptr->client_token, (int)vds_ptr->instance_id);
   result = vds_validate_client(vds_ptr, resync_params_ptr->client_id, resync_params_ptr->client_token);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS client invalid, result %d, instance(%x)", (int)result, (int)vds_ptr->instance_id);
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }
   /* Drop existing request, if any */
   vds_client_t *client_ptr = &vds_ptr->client_list[resync_params_ptr->client_id];
   vds_client_request_t *request_ptr = &client_ptr->request;
   if(request_ptr->active_flag)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS dropping request as part of resync, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
      vds_release_request(client_ptr);
      request_ptr->active_flag = FALSE;
      if (client_ptr->resync_response_signal_ptr)
      {
    	  qurt_elite_signal_send(client_ptr->resync_response_signal_ptr);
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS issuing resync response to client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);

      }
   }
   /* If no current request, mark as pending so the next one can be dropped */
   else
   {
      client_ptr->resync_pending_flag = TRUE;
   }

   uint32_t mask = 0xFFFF0000 & vds_ptr->active_clients_mask;

   vds_exit_steady_state(vds_ptr);
   vds_ptr->reinit_ctr = 0;
   while (mask != 0)
   {
      uint32_t client_id;
      client_id = voice_get_signal_pos(mask);
      vds_client_t *client_ptr = &vds_ptr->client_list[client_id];
      client_ptr->reinit_state =0;
      client_ptr->delivery_miss_flag =FALSE;
      // the counter below is delibrately commented to preseve the count through out the life of call, in case services
      // dont unsbscribe during HO
      //client_ptr->miss_count_steady_state = 0;
      //client_ptr->miss_count_steady_state_queue =0;
      mask &= ~(1<<(31- client_id));
   }
   elite_svc_send_ack(msg_ptr, result);
   return ADSP_EOK;
}

static ADSPResult vds_drop_now(void* instance_ptr, elite_msg_any_t*  msg_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_vds_resync_type* resync_msg_ptr = (elite_msg_custom_vds_resync_type*)msg_ptr->pPayload;
   vds_resync_t* resync_params_ptr = &resync_msg_ptr->resync_params;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS drop now requested for client token %x instance(%x)", (int)resync_params_ptr->client_token, (int)vds_ptr->instance_id);
   result = vds_validate_client(vds_ptr, resync_params_ptr->client_id, resync_params_ptr->client_token);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS client invalid, result %d, instance(%x)", (int)result, (int)vds_ptr->instance_id);
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }

   /* Drop existing request, if any */
   vds_client_t *client_ptr = &vds_ptr->client_list[resync_params_ptr->client_id];
   vds_client_request_t *request_ptr = &client_ptr->request;
   if(request_ptr->active_flag)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS dropping request, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
      vds_release_request(client_ptr);
      request_ptr->active_flag = FALSE;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VDS drop now requested but not no packet is queued result %d, instance(%x)", (int)result, (int)vds_ptr->instance_id);
   }

   elite_svc_send_ack(msg_ptr, result);
   return ADSP_EOK;
}

static ADSPResult vds_delivery_handler(void* instance_ptr)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   ADSPResult result = ADSP_EBADPARAM;
   uint32_t client_id = vds_ptr->bit_pos;
   vds_client_t *client_ptr = &vds_ptr->client_list[client_id];
   uint64_t delivery_timestamp = avtimer_drv_get_time( vds_ptr->avt_drv );
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VDS delivery handler begin, token %x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
   /* Clear timer signal */
   qurt_elite_signal_clear(client_ptr->client_signal_ptr);
   /* Check if we have anything to deliver */
   if(!client_ptr->request.active_flag)
   {
      // This is a change in VDS behavior to support CR 832470.
      // Clients need to be able to gracefully handle error signals sent by VDS.
      // since sonly vprx sends resync information the second caondition is only for vprx.
      if((NULL != client_ptr->client_error_signal_ptr)&&(FALSE==client_ptr->resync_pending_flag))
      {
        // MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS issuing delivery miss response to client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         qurt_elite_signal_send(client_ptr->client_error_signal_ptr);
      }
      /* Increment miss count */
      client_ptr->miss_count++;
      if (TRUE == vds_ptr->steady_state_flag)
      {
         client_ptr->miss_count_steady_state++;  //Steady state miss counter of each client, this will start incrementing once steady state is reached i:e all clients have delivered atleast one buffer
         client_ptr->delivery_miss_flag = TRUE;  //If delivery miss happens in steady state then set this flag to calculate overshoot time

         /* This is generally an error scenario, but need not always be. Expect a few at startup. */
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS had nothing to deliver, client token 0x%x, steady state miss count %u, out request count(%ld), instance(%x)",
             (int)client_ptr->client_token, (unsigned int)client_ptr->miss_count_steady_state, client_ptr->out_request_count, (int)vds_ptr->instance_id);
      }
      else
      {
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error! VDS had nothing to deliver, client token 0x%x, miss count %u, instance(%x), (%d)",
             		 (int)client_ptr->client_token, (unsigned int)client_ptr->miss_count, (int)vds_ptr->instance_id,vds_ptr->del_miss_response_type);
      }

      client_ptr->del_begin_time = delivery_timestamp;

      if(VDS_STATE_BOOST == vds_ptr->boost_state)
      {
         vds_reset_rec_info(vds_ptr);
      }

      /* Return OK, no need to propagate error */
      return ADSP_EOK;
   }

   /* There is a pending resync, i.e. buffer needs to be dropped */
   // below code will never hit since moved to vds_queue_request()
   if(client_ptr->resync_pending_flag)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VDS dropping in delivery as part of resync, client token 0x%x, instance(%x)", (int)client_ptr->client_token, (int)vds_ptr->instance_id);
      vds_release_request(client_ptr);
      client_ptr->resync_pending_flag = FALSE;
      client_ptr->request.active_flag = FALSE;
      return ADSP_EOK;
   }

   /* VDS has something to deliver */
   vds_client_request_t* request_ptr = &client_ptr->request;
   bool_t is_tick_active = TRUE;
   if(VDS_DELIVER_ELITE_BUFFER == request_ptr->request_type)
   {
      for (uint32_t i =0; i< client_ptr->active_request_index ; i++)
      {
         result = qurt_elite_queue_push_back(request_ptr->request_parameters.elite_request[i].peer_ptr, (uint64_t*)(&request_ptr->request_parameters.elite_request[i].msg));
         /* Need to return buffer to sender if push fails */
         if(ADSP_FAILED(result))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Elite delivery failed with result %d, token %x, instance(%x)", (int)result, (int)client_ptr->client_token, (int)vds_ptr->instance_id);
            elite_msg_return_payload_buffer(&request_ptr->request_parameters.elite_request[i].msg);
         }
      }
      is_tick_active = client_ptr->is_tick_active;
      client_ptr->active_request_index = 0;
      client_ptr->is_tick_active = FALSE;
   }
   else if(VDS_DELIVER_APR_PACKET == request_ptr->request_type)
   {
      result = elite_apr_if_async_send(*request_ptr->request_parameters.apr_request.apr_handle_ptr, request_ptr->request_parameters.apr_request.apr_packet_ptr);
      /* Need to free packet if send fails */
      if(ADSP_FAILED(result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Packet delivery failed with result %d, token %x, instance(%x)", (int)result, (int)client_ptr->client_token, (int)vds_ptr->instance_id);
         elite_apr_if_free(*request_ptr->request_parameters.apr_request.apr_handle_ptr, request_ptr->request_parameters.apr_request.apr_packet_ptr);
      }
      else if(FALSE == client_ptr->is_delivery_tick_reqd)
      {
         if(client_ptr->del_begin_time < client_ptr->req_begin_time)
         {
            request_ptr->active_flag = FALSE;
            return ADSP_EOK; // this is overshoot case which is already taken care in vds_queue_request()
         }
         else
         {
            delivery_timestamp = client_ptr->del_begin_time;
         }
      }
   }
   /* Mark request inactive regardless of result */
   request_ptr->active_flag = FALSE;
   if(ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS failed to deliver, client 0x%x, result %d, instance(%x)", (int)client_ptr->client_token, (int)result, (int)vds_ptr->instance_id);
      /* Signal client back if requested */
      if(NULL != client_ptr->client_error_signal_ptr)
      {
         qurt_elite_signal_send(client_ptr->client_error_signal_ptr);
      }
      return result;
   }

   // check if the use case is single voice call and if steady state is reached or not.
   // if not then reset the debugging parameters for a client whenever the first delivery happens for that client.
   // When all 4 clients have delivered one buffer then set the steady state flag.
   if ((FALSE == vds_ptr->steady_state_flag) && (4 == vds_ptr->active_clients_ctr))
   {
      if (((4 > ((client_ptr->client_token)&(0xF))) && (2!= client_ptr->reinit_state)) )

      {
         client_ptr->time_stamp_overshoot_max = 0;
         client_ptr->overshoot_time_max = 0;
         client_ptr->undershoot_time_min = 0x7FFFFFFF;
         client_ptr->time_stamp_undershoot_min = 0;
         client_ptr->reinit_state ++;;
         vds_ptr->reinit_ctr++;

      }
      if (8 == vds_ptr->reinit_ctr)        // once all 4 clients have delivered two buffer then its safe to assume that steady state is reached
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VDS STEADY_STATE reached");
         vds_ptr->steady_state_time_stamp = avtimer_drv_get_time( vds_ptr->avt_drv ); // latch start time stamp when first deliveries of 4 clients successful
         vds_enter_steady_state(vds_ptr);
      }
   }

   // If buffer was queued to vds on time then calculate how long before vds delivery tick got triggered,
   // data was queued by the client. If that is less than previous minimum undershoot time then update the
   // minimum undershoot time for that client
   uint32_t time;
   time = delivery_timestamp - client_ptr->req_begin_time;
   client_ptr->del_begin_time = delivery_timestamp;

   if(is_tick_active && delivery_timestamp)
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS delivery handler begin time stamp %lld", client_ptr->del_begin_time );
#ifndef SIM
      vds_update_histogram( client_ptr->client_token, time);
#endif

      if( (TRUE == client_ptr->is_delivery_tick_reqd) &&
            (VDS_CLIENT_VDEC_PKT_REQUEST != (client_ptr->client_token&0x0000FFFF) ) )
      {
         if(VDS_STATE_BOOST == vds_ptr->boost_state)
         {
            vds_rec_process_undershoot_duration(vds_ptr,time);
         }
      }

#ifdef VDS_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VDS delivery handler, token %x under_shoot_time(%ld)", (int)client_ptr->client_token, (int)time);
#endif

      if (time < client_ptr->undershoot_time_min)
      {
         client_ptr->undershoot_time_min = time;
         client_ptr->time_stamp_undershoot_min = avtimer_drv_get_time( vds_ptr->avt_drv );
      }
   }
   /* Since we are successful, increment delivered request count */
   client_ptr->out_request_count++;
   return ADSP_EOK;
}

/* Utility function to drop VDS requests appropriately based on request type */
static ADSPResult vds_release_request(vds_client_t* client_ptr)
{
   ADSPResult result = ADSP_EBADPARAM;
   vds_client_request_t *request_ptr = &client_ptr->request;
   if(!request_ptr->active_flag)
   {
      /* Nothing to release */
      return ADSP_EOK;
   }
   if(VDS_DELIVER_ELITE_BUFFER == request_ptr->request_type)
   {
	  for (uint32_t i =0; i < client_ptr->active_request_index; i++ )
	  {
         result = elite_msg_return_payload_buffer(&request_ptr->request_parameters.elite_request[i].msg);
         if(ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS failed to return buffer, result %d", (int)result);
         }
	  }
        client_ptr->active_request_index = 0;
   }
   else if(VDS_DELIVER_APR_PACKET == request_ptr->request_type)
   {
      result = elite_apr_if_free(*request_ptr->request_parameters.apr_request.apr_handle_ptr, request_ptr->request_parameters.apr_request.apr_packet_ptr);
      if(ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VDS failed to return buffer, result %d", (int)result);
      }
   }
      return result;
   }

/* Utility function to clear mask bit for client and deallocate signal */
static void vds_release_client(void* instance_ptr, uint32_t client_id)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   vds_client_t *client_ptr = &vds_ptr->client_list[client_id];
   /* Free memory allocated to signal, if any */
   if(NULL != client_ptr->client_signal_ptr)
   {
      qurt_elite_signal_deinit(client_ptr->client_signal_ptr);
      qurt_elite_memory_free(client_ptr->client_signal_ptr);
   }
   /* Clear bit from mask */
   vds_ptr->active_clients_mask &= (~(1 << (31 - client_id)));
   /* Remove from wait mask also */
   vds_ptr->wait_mask &= (~(1 << (31 - client_id)));
   /* Memset back to zero to clear all relevant fields */
   memset(client_ptr, 0, sizeof(vds_client_t));
   return;
}

/* Utility function to validate that a client exists and is current */
static ADSPResult vds_validate_client(void* instance_ptr, uint32_t client_id, uint32_t client_token)
{
   vds_t* vds_ptr = (vds_t*)instance_ptr;
   /* Ensure client is actually active and in range */
   if((MAX_NUM_CLIENTS <= client_id) || (FALSE == voice_tst_bit(vds_ptr->active_clients_mask, 31 - client_id)))
   {
      return ADSP_EFAILED;
   }
   /* Get client */
   vds_client_t* client_ptr = &vds_ptr->client_list[client_id];
   /* Validate client token */
   if(client_ptr->client_token != client_token)
   {
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

#define VDS_DIAG_SET_PARAM 0

VOID_DIAG_PACKED_PTR vds_diag_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16_t pkt_len
)
{
   int32_t rc = APR_EOK;
   cvd_diag_rsp_t* rsp_pkt = NULL;
   uint8_t cmd = ( ( cvd_diag_cmd_req_t* ) req_pkt )->cmd;

   vds_t * vds_ptr = (vds_t *)vds_handle;

   MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"vds_diag_cmd_processing(): Command:(%d)", cmd );

   switch ( cmd )
   {
      case VDS_DIAG_SET_PARAM:
      {
         vds_rec_config_struct_t rec_conf;
         memscpy(&rec_conf,sizeof(vds_rec_config_struct_t),
               &(( ( cvd_diag_cmd_req_t* ) req_pkt )->payload1),sizeof(vds_rec_config_struct_t));

         if(  IS_NUM_FRAME_THRESH_VALID(rec_conf.rec_num_frames_thresh) &&  IS_UNDERSHOOT_THRESH_VALID(rec_conf.rec_undershoot_thresh_us)   )
         {
            vds_ptr->rec_config_cached = rec_conf;

            MSG_2( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "vds_diag_cmd_processing(): set_param: threshold %d num_frames %d",
                  vds_ptr->rec_config_cached.rec_num_frames_thresh, vds_ptr->rec_config_cached.rec_undershoot_thresh_us );

         }
         else
         {
            rc = APR_EBADPARAM;
            MSG_2( MSG_SSID_QDSP6, DBG_ERROR_PRIO, "vds_diag_cmd_processing(): set_param failed: threshold %d or/and num_frames %d exceed range",
                  rec_conf.rec_num_frames_thresh, rec_conf.rec_undershoot_thresh_us );
            break;
         }
      }
      break;
      default:
      {
         rc = APR_EUNSUPPORTED;
         break;
      }
   }

   rsp_pkt = ( cvd_diag_rsp_t* )diagpkt_subsys_alloc( DIAG_SUBSYS_CVD,
                                           CVD_DIAG_COMMAND,
                                           sizeof( cvd_diag_rsp_t ) );
   if ( rsp_pkt == NULL )
   {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vds_diag_cmd_processing(): Diag Packet could not be allocated");
     return NULL;
   }

   rsp_pkt->result = rc;
   diagpkt_commit( ( void* )rsp_pkt );

   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vds_diag_cmd_processing(): Rsp(0x%08x)",  rsp_pkt->result );

   return rsp_pkt;
}
