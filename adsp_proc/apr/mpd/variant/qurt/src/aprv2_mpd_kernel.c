/*
  Copyright (C) 2014-2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //components/rel/apr.common/2.0/mpd/variant/qurt/src/aprv2_mpd_kernel.c#5 $
  $Author: pwbldsvc $
*/
#include <qurt.h>
#include "qurt_qdi_driver.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#include "apr_lock.h"
#include "apr_list.h"
#include "aprv2_packet.h"
#include "aprv2_mpd_i.h"
#include "aprv2_mpd_qurt_i.h"
#include "apr_dbg_msg.h"
#include "apr_thread.h"
#include "apr_event.h"
#include "servreg_common.h"
#include "qurt_anysignal.h"
#include "servreg_locator.h"
#include "servreg_locator_qurt.h"
#include "servreg_monitor.h"
#include "servreg_monitor_qurt.h"
#include "apr_timer.h"
#include "apr_misc.h"
#include "aprv2_ipc_config.h"
#ifdef PDR_FEATURE //dependent on aprv2_ipc_config.h, PDR_FEATURE flag defined there
#include "EliteThread_Priority.h"
#endif

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Max. no. of uAPRs allowed */
#define APRV2_MPD_MAX_UAPRS                   10

/* Total number of packets */
#define APRV2_MPD_NUM_PACKETS                 5 * APRV2_MPD_MAX_UAPRS

#ifdef PDR_FEATURE
/* service registry defines */
#define APR_REMOTE_SERVICE_STATUS_MASK 0x4 // listener signal mask for upd notifications
#define SERVREG_TEST_SIG_LOC_AVAILABLE 0x1 //Signal mask used for service locatory availability
#define APR_PDR_THREAD_ACK 0x5             //signal mask used to signal that apr pdr workerthread
#define APR_PDR_THREAD_COMPLETED_ACK 0x6   //signal mask used for sucessfully terminate a thread
#define TASK_PRIORITY( task_pri_macro ) ( 255 - task_pri_macro )
#define APR_HIGH_TASK_PRIORITY ( TASK_PRIORITY( ELITETHREAD_CVP_HIGH_PRIO ) )
#define APR_PDR_TASK_NAME_INIT ( "APR_PDR_THREAD_INIT" )
#define APR_TASK_STACK_SIZE ( 4 * 1024 )
char apr_svc_name[32] = "avs/audio";  //pdr service name
int32_t is_pdr_cb_already_invoked = 0;

/* Service registry   */
qurt_anysignal_t servreg_pdr_apr_status_signal;
qurt_anysignal_t apr_pdr_worker_thread_ack; //for apr_pdr_worker_th thread ack
SERVREG_DL_HANDLE remote_apr_mon_handle;
SERVREG_SERVICE_STATE apr_state = SERVREG_SERVICE_STATE_UNINIT;
SERVREG_SERVICE_STATE upd_status = SERVREG_SERVICE_STATE_UNINIT;
apr_thread_t apr_pdr_handle_init;


/* typedefs */
typedef struct qurt_apr_thread_list_t qurt_apr_thread_list_t;
struct qurt_apr_thread_list_t
{
  void *stack; //allocated stack pointer
  uint32_t tid; //tid 
};

static qurt_apr_thread_list_t qurt_apr_thread_list={NULL,0}; //used to store thread id and stack pointer to free
#endif

/*****************************************************************************
 * Data Structures                                                           *
 ****************************************************************************/

typedef struct aprv2_mpd_packet_item_s {
   apr_list_node_t link;
   aprv2_packet_t *packet;
} aprv2_mpd_packet_item_t;

struct aprv2_mpd_uapr_item_s;

typedef struct aprv2_mpd_uapr_s {
   /* QDI framework related fields */
   qurt_qdi_obj_t qdiobj;
   int client_handle;                /* Client handle for QDI heap reference */

   /* Driver defined fields */
   struct aprv2_mpd_uapr_item_s *nodeptr;       /* Ptr to its container item */
   qurt_signal_t * rx_signal;     /* To signal uAPR about incoming rx packet */
   uint8_t domain_id;                /* Assuming only one domain id per uAPR */
   //aprv2_mpd_packet_item_t *rx_packet;             /* Rx packet for the uAPR */
   apr_list_t rx_packet_q;                   /* Rx packet queue for the uAPR */
} aprv2_mpd_uapr_t;

typedef struct aprv2_mpd_uapr_item_s {
   apr_list_node_t link;
   aprv2_mpd_uapr_t uapr;
} aprv2_mpd_uapr_item_t;

typedef struct aprv2_mpd_qdi_opener_s {
   qurt_qdi_obj_t qdiobj;
} aprv2_mpd_qdi_opener;

/*****************************************************************************
 * Declarations                                                              *
 ****************************************************************************/

static int aprv2_mpd_qdi_invocation
(
   int client_handle,
   qurt_qdi_obj_t *obj,
   int method,
   qurt_qdi_arg_t a1,
   qurt_qdi_arg_t a2,
   qurt_qdi_arg_t a3,
   qurt_qdi_arg_t a4,
   qurt_qdi_arg_t a5,
   qurt_qdi_arg_t a6,
   qurt_qdi_arg_t a7,
   qurt_qdi_arg_t a8,
   qurt_qdi_arg_t a9
);

static void aprv2_mpd_qdi_release(qurt_qdi_obj_t *qdiobj);
static int32_t aprv2_mpd_allocate_uapr(aprv2_mpd_uapr_t **ppuapr);
static int32_t aprv2_mpd_free_uapr(aprv2_mpd_uapr_t *puapr);
static int32_t aprv2_mpd_free_packet_item(aprv2_mpd_packet_item_t *ppacket_item);
static int32_t aprv2_mpd_allocate_packet_item(aprv2_mpd_packet_item_t **pppacket_item);

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

/*
 * Data Definitions
*/
static aprv2_mpd_qdi_opener aprv2_mpd_opener = {
   { aprv2_mpd_qdi_invocation,
     QDI_REFCNT_PERM,
     NULL }
};

static apr_lock_t aprv2_mpd_packets_lock;
static aprv2_mpd_packet_item_t aprv2_mpd_packets[APRV2_MPD_NUM_PACKETS];
static apr_list_t aprv2_mpd_free_packet_q;
static apr_lock_t apr_free_buffer_lock; //for locking the free time buffers

static aprv2_mpd_uapr_item_t aprv2_mpd_uaprs[APRV2_MPD_MAX_UAPRS];
static apr_list_t aprv2_mpd_used_uapr_q;
static apr_list_t aprv2_mpd_free_uapr_q;

/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

static void aprv2_mpd_isr_lock_fn (void)
{
   apr_lock_enter(aprv2_mpd_packets_lock);
}

static int32_t aprv2_uapr_free_pkts( aprv2_mpd_uapr_t *puapr)
{
    int32_t ret=APR_EOK;
    aprv2_packet_t *packet;
    aprv2_mpd_packet_item_t *ppacket_item;
    
    while(puapr->rx_packet_q.size > 0)
    {
         APR_DBG_1(APR_DBG_HIGH,"Clearing packets queue_size=%d", puapr->rx_packet_q.size);
         apr_list_remove_head(&puapr->rx_packet_q,
            (apr_list_node_t **)&ppacket_item);
         packet = ppacket_item->packet;

         /* Client is notified on packet failure from main thread context, so freeing up local packets is fine to avoid deadlocks */
         ret = __aprv2_cmd_free(packet->dst_addr, packet);
         if(APR_EOK != ret)
         {
      //remove this as acessing a pointer after a pointer is freed is not safe..
            APR_DBG_2(APR_DBG_ERROR,
               "Failed to send END CMD to dst=0x%X ret=0x%X",
               packet->dst_addr, ret);
         }
         ppacket_item->packet = NULL;
         aprv2_mpd_free_packet_item(ppacket_item);
    }
  return ret; 
}

static void aprv2_mpd_isr_unlock_fn (void)
{
   apr_lock_leave(aprv2_mpd_packets_lock);
}

APR_INTERNAL int32_t aprv2_mpd_init ( void )
{
	
	int32_t ret;
	#ifdef PDR_FEATURE
	int32_t ret_pdr = APR_EOK;
	#endif
    uint32_t index;
   
   /* 
    *  Initializing the pre-allocated pool of packets
   */
   ret = apr_lock_create(APR_LOCK_TYPE_MUTEX, &aprv2_mpd_packets_lock);

   apr_list_init(&aprv2_mpd_free_packet_q,
                            aprv2_mpd_isr_lock_fn, aprv2_mpd_isr_unlock_fn);

   for (index = 0; index < APRV2_MPD_NUM_PACKETS; index++)
   {
      apr_list_add_tail(&aprv2_mpd_free_packet_q,
                             &aprv2_mpd_packets[index].link);
   }

   /* 
    *  Initializing the pre-allocated array of aprv2_mpd_uapr_t objects.
   */
   apr_list_init(&aprv2_mpd_used_uapr_q, NULL, NULL);
   apr_list_init(&aprv2_mpd_free_uapr_q, NULL, NULL);

   for (index = 0; index < APRV2_MPD_MAX_UAPRS; index++)
   {
      apr_list_init(&aprv2_mpd_uaprs[index].uapr.rx_packet_q, 
                             aprv2_mpd_isr_lock_fn, aprv2_mpd_isr_unlock_fn);
      apr_list_add_tail(&aprv2_mpd_free_uapr_q, &aprv2_mpd_uaprs[index].link);
   }

   /* 
    * Registering APR MPD DeviceName with QuRT.
    *
   */
   if(qurt_qdi_devname_register(
               APRV2_MPD_DEVICE_NAME, &aprv2_mpd_opener.qdiobj))
   {
      APR_DBG_0(APR_DBG_ERROR,"Failed to register APR MPD Device Name");
      ret = APR_EFAILED;
   }
   
   #ifdef PDR_FEATURE
   ret_pdr = apr_pdr_init();
   
   if(ret != APR_EOK)
   {
      APR_DBG_0(APR_DBG_ERROR,"PD Restart mode failed, disabling pd restart mode..!");
   }
   else
   {
      APR_DBG_0(APR_DBG_HIGH,"PD Restart mode sucessfully enabled..!");
   }
   #endif
   
   return ret;
}

APR_INTERNAL int32_t aprv2_mpd_deinit ( void )
{
    int32_t ret = APR_EOK;

   /* TODO:
    *
    * 1. Do the required clean-up as kAPR is going down.
    *
   */
   #ifdef PDR_FEATURE
     ret= apr_pdr_deinit();
   #endif
   return ret;
}

/*
  Register a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_register(uint16_t addr )
{
   /* This is no-op on kAPR. Returning APR_EOK
   */
   return APR_EOK;
}

/*
  Deregister a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_deregister(uint16_t addr )
{
   /* This is a no-op on kAPR. Returning APR_EOK*/
   return APR_EOK;
}

/*
 * Send a packet to the dst address specified in the packet. In case
 * of UAPR the packet is sent to Kernel. In-case of KAPR the packet gets
 * sent to the correct destination based on dst ID. 
 *
 * Assumption: Ownership of the memory allocated for packet is transferred to
 *             APR MPD.
 *
 * @param packet apr packet to send
*/
APR_INTERNAL int32_t aprv2_mpd_send(aprv2_packet_t* packet )
{
   /*
    * Based on the book keeping information maintained by kAPR determine
    * which uAPR hosts this service and send the message.
    *
    * Although uAPR informs kAPR on each service registration. Routing of
    * messages would be made solely based on the domain id.
    *
    * Per design, each uAPR shall be assigned one or more domain ids. But, no
    * two uAPRs shall have the same domain id.
    *
   */

   uint16_t dst_domain_id;
   apr_list_node_t *cur_node, *dummy_node;
   aprv2_mpd_uapr_item_t *cur_uapr_item;
   aprv2_mpd_packet_item_t *ppacket_item;
   int32_t ret=0;

   if (NULL == packet)
   {
      APR_DBG_0(APR_DBG_ERROR,"NULL packet");
      return APR_EBADPARAM;
   }

   dst_domain_id = APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr);
   /* Find if there exists an uAPR with a matching domain ID*/
   dummy_node = &aprv2_mpd_used_uapr_q.dummy;
   cur_node = dummy_node->next;
   while (cur_node != dummy_node)
   {
      cur_uapr_item = (aprv2_mpd_uapr_item_t *)cur_node;
      if (dst_domain_id == cur_uapr_item->uapr.domain_id)
         break;
      cur_node = cur_node->next;
   }
   if (cur_node == dummy_node)
   {
      /* Looks like UPD has not done registration with kAPR
       * or it is crashed. Therefore, it is required to inform
       * sender about packet delivery failure.*/
      APR_DBG_1(APR_DBG_ERROR,"Destination service=0x%04X is not available",packet->dst_addr);
      ret = __aprv2_cmd_end_command(packet->dst_addr, packet, APR_ENOSERVICE);
      if(ret != APR_EOK)
      {
        APR_DBG_1( APR_DBG_ERROR, "Failed to send end command to ret=0x%X",
          ret );
      }
      return ret;
   }

#ifdef PDR_FEATURE
    if (upd_status != SERVREG_SERVICE_STATE_UP)
    { 
  //Invoke the aprv2_pd_restart_Call her
  //If this block is invoked then we are freeing uapr obj as well so untill pd is up 
  //no packets will reach here and will continue to return 
      ret = __aprv2_cmd_end_command(packet->dst_addr, packet, APR_ENOPROCESS);
      APR_DBG_1(APR_DBG_HIGH,"Sent end command response from pd down state ret=0x%X   Audio pd is not yet up",ret);
      if(ret != APR_EOK)
      {
        APR_DBG_1( APR_DBG_ERROR, "Failed to send end command to ret=0x%X",
          ret );
      }
      return APR_EOK; /* Command is already freed so sending other than EOK will result in double free as command is already freed */
    }
  
#endif
   /* Queue the packet to this uAPR and issue a signal to the same.*/
   ret = aprv2_mpd_allocate_packet_item(&ppacket_item);
   if (APR_EOK != ret)
   {
      APR_DBG_1( APR_DBG_ERROR,
        "Unable to allocate memory to received packet ret=%d", ret );
      return APR_ENOMEMORY;
   }

   ppacket_item->packet = packet;

   /* Adding to the uAPR Rx packet queue*/
   //cur_uapr_item->uapr.rx_packet = ppacket_item;
   apr_list_add_tail(&cur_uapr_item->uapr.rx_packet_q, (apr_list_node_t *)ppacket_item);

   /* Signal uAPR */
   qurt_signal_set(cur_uapr_item->uapr.rx_signal, APRV2_MPD_SIG_RX_PACKET);
   return APR_EOK;
}

/*
  Register callback with mpd service. This call-back will get invoked
  for every received message.

  @param pfn_rx_cb callback function
*/
APR_INTERNAL int32_t aprv2_mpd_set_rx_cb(aprv2_mpd_cb_fn_t pfn_rx_cb)
{

   /* TODO: Add code to store the callback function*/
   return APR_EOK;
}

static int aprv2_mpd_qdi_invocation
(
   int client_handle,
   qurt_qdi_obj_t *obj,
   int method,
   qurt_qdi_arg_t a1,
   qurt_qdi_arg_t a2,
   qurt_qdi_arg_t a3,
   qurt_qdi_arg_t a4,
   qurt_qdi_arg_t a5,
   qurt_qdi_arg_t a6,
   qurt_qdi_arg_t a7,
   qurt_qdi_arg_t a8,
   qurt_qdi_arg_t a9
)
{
   aprv2_mpd_uapr_t *puapr = (aprv2_mpd_uapr_t *)obj;
   int ret = APR_EOK;
   switch (method)
   {
      case QDI_OPEN:

         APR_DBG_3(APR_DBG_HIGH,"aprv2_mpd_qdi_invocation handle=0x%X obj=0x%X   refcnt=0x%X",client_handle,obj,puapr->qdiobj.refcnt);
         ret = aprv2_mpd_allocate_uapr(&puapr);
         if (APR_EOK != ret)
         {
            APR_DBG_1( APR_DBG_ERROR, "Unable to allocate memory. ret=%d",
               ret );
            return -1;
         }

         puapr->qdiobj.invoke = aprv2_mpd_qdi_invocation;
         puapr->qdiobj.refcnt = QDI_REFCNT_INIT;
         puapr->qdiobj.release = aprv2_mpd_qdi_release;

         puapr->client_handle = client_handle;
         puapr->domain_id = 0x4; //TODO: Temporary fix. Should be a1.num;

         /* Creating signal to inform uAPR on Rx packets*/
         puapr->rx_signal =
                 (qurt_signal_t *) qurt_qdi_user_malloc(client_handle,
                                                sizeof(*puapr->rx_signal));
         if (NULL == puapr->rx_signal)
         {
            aprv2_mpd_free_uapr(puapr);
            APR_DBG_0(APR_DBG_ERROR,"Failed to allocate memory for Rx Signal");
            return -2;
         }
         qurt_signal_init(puapr->rx_signal);

         /* Create a handle for the object and return*/
         ret = qurt_qdi_handle_create_from_obj_t(client_handle,
                                               (qurt_qdi_obj_t *)puapr);
         if (ret < 0)
         {
            qurt_signal_destroy(puapr->rx_signal);
            qurt_qdi_user_free(client_handle, puapr->rx_signal);
            aprv2_mpd_free_uapr(puapr);
         }
         else
		 {
			//AUDIO PD UP notification should be recieved from servregistry, but the signal should come from
			//audio pd to hlos to audio root pd. In order to reduce this turnaround time, this change will immediately help
			//in getting pd up notification.
			//When a pkt is sent from hlos to audio upd & all the failure packets are sent in glink context.
			//This change will help in fixing this issue as well.
		    //upd_status = SERVREG_SERVICE_STATE_UP;
		 }
		 
         return ret;  /* Returning the handle obtained */
      
	  case APRV2_MPD_CMDID_SET_UPD_STATE:
	  {
		APR_DBG_0(APR_DBG_HIGH,"Setting the mpd upd state to up");
		upd_status = SERVREG_SERVICE_STATE_UP;
		return ret;
	  }
	  
      case APRV2_MPD_CMDID_GET_RX_SIGNAL:
      {
         qurt_signal_t ** ret_signal;
         
         APR_DBG_1(APR_DBG_MEDIUM,"APRV2_MPD_CMDID_GET_RX_SIGNAL   refcnt=0x%X",puapr->qdiobj.refcnt);
         ret = qurt_qdi_buffer_lock(client_handle, a1.ptr,
                 sizeof(qurt_signal_t **), QDI_PERM_W, (void **)&ret_signal);
         if (0 == ret)
         {
             *ret_signal = puapr->rx_signal;
             ret = APR_EOK;
         }
         else
         {
             APR_DBG_1(
               APR_DBG_ERROR,
               "Failed to acquire QDI buffer lock. ret=%d",
               ret
             );
             ret = APR_EFAILED;
         }
         return ret;
      }

      case APRV2_MPD_CMDID_REGISTER:
      case APRV2_MPD_CMDID_DEREGISTER:
         /*
          * Register/De-register are no-op for now as each uAPR is assigned one
          * or more unique domaind Ids. No two uAPRs shall have the same domain
          * Id.
          * In future, this information shall be maintained and used
          * for service id level up/down notifications.
         */
         return ret;

      case APRV2_MPD_CMDID_ASYNC_SEND:
      {
         /*
          * Check if the destination belongs to one of the uAPRs.
          * If so, try to send it to the uAPR. Otherwise, give it to kAPR core.
         */
         aprv2_packet_t *packet, *packet_copy;
         uint32_t size, packet_size, alloc_type;

         size = a2.num;

         ret = qurt_qdi_buffer_lock(client_handle,
                               a1.ptr, size, QDI_PERM_R, (void **)&packet);
         if (0 == ret)
         {
            /* Allocate a packet, copy and send*/
            packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );

            alloc_type =
                ((APR_GET_FIELD(APRV2_PKT_MSGTYPE, packet->header)
                == APRV2_PKT_MSGTYPE_CMDRSP_V) ? APRV2_ALLOC_TYPE_RESPONSE_RAW:
                                                APRV2_ALLOC_TYPE_COMMAND_RAW );
            ret = __aprv2_cmd_alloc(packet->src_addr,
                                        alloc_type, packet_size, &packet_copy);
            if (APR_EOK != ret)
            {
               APR_DBG_1(
                APR_DBG_ERROR,
                "Failed to allocate APR packet. ret=%d",
                ret
              );
               return ret;
            }

            memcpy(packet_copy, packet, packet_size);
            ret = __aprv2_cmd_async_send(packet_copy->src_addr, packet_copy);
            if ( ret )
            {
               __aprv2_cmd_free(packet_copy->src_addr, packet_copy);
            }
         }
         else
         {
            APR_DBG_1(
              APR_DBG_ERROR,
              "Failed to acquire QDI buffer lock. ret=%d",
              ret
            );
            ret = APR_EFAILED;
         }
         return ret;
      }

      case APRV2_MPD_CMDID_PEEK_RX_PACKET:
      {
         aprv2_packet_t *ret_packet_header;
         aprv2_mpd_packet_item_t *packet_item;
        
         if (puapr->rx_packet_q.dummy.next == &puapr->rx_packet_q.dummy)
         {  
            return APR_ENOTEXIST;
         }

         ret = qurt_qdi_buffer_lock(client_handle, a1.ptr,
                 sizeof(aprv2_packet_t), QDI_PERM_W, (void **)&ret_packet_header);

         if (0 == ret)
         {
            packet_item = (aprv2_mpd_packet_item_t *)puapr->rx_packet_q.dummy.next;
            memcpy(ret_packet_header, packet_item->packet, sizeof(aprv2_packet_t));
            ret = APR_EOK;
         }
         else
         {
            APR_DBG_1(
              APR_DBG_ERROR,
              "Failed to acquire QDI buffer lock. ret=%d",
              ret
            );
            ret = APR_EFAILED;
         }
         return ret;
      }

      case APRV2_MPD_CMDID_READ_RX_PACKET:
      {
         aprv2_packet_t *packet, *ret_packet;
         uint32_t size, packet_size, *ret_bytes_copied;
         aprv2_mpd_packet_item_t *ppacket_item;

         if (puapr->rx_packet_q.dummy.next == &puapr->rx_packet_q.dummy)
         { 
            return APR_ENOTEXIST;
         }

         size = a2.num;
         ret = qurt_qdi_buffer_lock(client_handle,
                               a1.ptr, size, QDI_PERM_W, (void **)&ret_packet);

         if (0 == ret)
         {
            apr_list_remove_head(&puapr->rx_packet_q, (apr_list_node_t **)&ppacket_item);
            packet = ppacket_item->packet;
            packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);

            memcpy(ret_packet, packet, packet_size);

            __aprv2_cmd_free(packet->dst_addr, packet);
            ppacket_item->packet = NULL;
            aprv2_mpd_free_packet_item(ppacket_item);

            ret = qurt_qdi_buffer_lock(client_handle,
                               a3.ptr, sizeof(uint32_t), QDI_PERM_W, (void **)&ret_bytes_copied);
            if (0 == ret)
               *ret_bytes_copied = packet_size;
            ret = APR_EOK;
         }
         else
         {
            APR_DBG_1(
               APR_DBG_ERROR,
              "Failed to acquire QDI buffer lock. ret=%d",
               ret
            );
            ret = APR_EFAILED;
         }

         return ret;
      }

      default:
      {
         APR_DBG_1(APR_DBG_HIGH,"Received unknown method=0x%X", method);
         return qurt_qdi_method_default(
            client_handle, obj, method,
            a1, a2, a3, a4, a5, a6, a7, a8, a9
         );
      }
   }
}

static void aprv2_mpd_qdi_release(qurt_qdi_obj_t *qdiobj)
{

   APR_DBG_0(APR_DBG_HIGH,"aprv2_mpd_qdi_release");
  apr_lock_enter(apr_free_buffer_lock);
   if (NULL != qdiobj){ APR_DBG_0(APR_DBG_HIGH,"aprv2_mpd_qdi_release_not_null");
        aprv2_mpd_uapr_t *puapr = (aprv2_mpd_uapr_t *) qdiobj;
    
   if (NULL != puapr)
   {
      int32_t ret;

      APR_DBG_2(APR_DBG_HIGH,"refcnt=0x%X, client_handle=0x%X", puapr->qdiobj.refcnt, puapr->client_handle);

      /* Destroying and Free Rx signal*/
      APR_DBG_0(APR_DBG_HIGH,"Destroying RX signal");
      qurt_signal_destroy(puapr->rx_signal);

      APR_DBG_0(APR_DBG_HIGH,"Freeing RX signal handler");
      qurt_qdi_user_free(puapr->client_handle, puapr->rx_signal);

      /* Clear Rx packet queue if any*/
      APR_DBG_1(APR_DBG_HIGH,"Total pending packets queue_size=%d", puapr->rx_packet_q.size);
      ret=aprv2_uapr_free_pkts(puapr);

      /* Object cleanup*/
      puapr->client_handle = 0;
      puapr->domain_id = 0;
      puapr->qdiobj.invoke = NULL;
      puapr->qdiobj.refcnt = 0;
      puapr->qdiobj.release = NULL;
      puapr->rx_signal = NULL;

      APR_DBG_0(APR_DBG_HIGH,"Freeing uAPR object");

      /* Free UAPR Object*/
      aprv2_mpd_free_uapr(puapr);
   }
   else
   {
      APR_DBG_0(APR_DBG_ERROR,"NULL parameters");
   }
   }
  apr_lock_leave(apr_free_buffer_lock); //Release the lock before returning
   return;
}

static int32_t aprv2_mpd_allocate_uapr(aprv2_mpd_uapr_t **ppuapr)
{
   int ret = APR_EOK;
   aprv2_mpd_uapr_item_t *puapr_item = NULL;

   if (NULL == ppuapr)
   {
      APR_DBG_0(APR_DBG_ERROR,"Invalid Parameter");
      return APR_EBADPARAM;
   }

   ret = apr_list_remove_head(&aprv2_mpd_free_uapr_q, (apr_list_node_t**) &puapr_item);
   if (APR_EOK == ret)
   {
       apr_list_add_tail(&aprv2_mpd_used_uapr_q, &puapr_item->link);
       puapr_item->uapr.nodeptr = (struct aprv2_mpd_uapr_item_s *)puapr_item;
       *ppuapr = &puapr_item->uapr;
   }
   return ret;
}

static int32_t aprv2_mpd_free_uapr(aprv2_mpd_uapr_t *puapr)
{
   int ret = APR_EOK;
   aprv2_mpd_uapr_item_t *puapr_item = NULL;

   if (NULL == puapr)
   {
      APR_DBG_0(APR_DBG_ERROR,"Invalid Parameter");
      return APR_EBADPARAM;
   }

   puapr_item = (aprv2_mpd_uapr_item_t *)puapr->nodeptr;

   ret = apr_list_delete(&aprv2_mpd_used_uapr_q, (apr_list_node_t *)puapr_item);
   if (APR_EOK == ret)
   {
       apr_list_add_tail(&aprv2_mpd_free_uapr_q, &puapr_item->link);
   }

   APR_DBG_2(APR_DBG_HIGH,"aprv2_mpd_free_uapr used_list_size=%d    free_list_size=%d", aprv2_mpd_used_uapr_q.size,aprv2_mpd_free_uapr_q.size);
   return ret;
}

static int32_t aprv2_mpd_allocate_packet_item(aprv2_mpd_packet_item_t **pppacket_item)
{
   int ret = APR_EOK;
   aprv2_mpd_packet_item_t *ppacket_item = NULL;
   if (NULL == pppacket_item)
   {
      APR_DBG_0(APR_DBG_ERROR,"Invalid Parameter");
      return APR_EBADPARAM;
   }

   ret = apr_list_remove_head(&aprv2_mpd_free_packet_q, (apr_list_node_t**) &ppacket_item);
   if (APR_EOK == ret)
   {

       *pppacket_item = ppacket_item;
   }
   APR_DBG_1(APR_DBG_MEDIUM,"aprv2_mpd_allocate_packet_item free_pkt_list_size=%d", aprv2_mpd_free_packet_q.size);
   return ret;
}

static int32_t aprv2_mpd_free_packet_item(aprv2_mpd_packet_item_t *ppacket_item)
{
   int ret = APR_EOK;
   if (NULL == ppacket_item)
   {
      APR_DBG_0(APR_DBG_ERROR,"Invalid Parameter");
      return APR_EBADPARAM;
   }


       apr_list_add_tail(&aprv2_mpd_free_packet_q, &ppacket_item->link);

   APR_DBG_1(APR_DBG_MEDIUM,"aprv2_mpd_free_packet_item free_pkt_list_size=%d", aprv2_mpd_free_packet_q.size);
   return ret;
}

/* APR PDR Feature api's */

#ifdef PDR_FEATURE

APR_INTERNAL void apr_sr_locator_available (SERVREG_CB_PARAM data, SERVREG_CB_PARAM ignore)
{  
  /* Signal task to start pdr registration */
  APR_DBG_0(APR_DBG_HIGH,"TMS cb triggered");

  //Triggering callback more than once, should not be impacted. Below check should take care on this
  is_pdr_cb_already_invoked = is_pdr_cb_already_invoked + 1;
  
  if (is_pdr_cb_already_invoked == 1) //check is to ensure setting the signal only once.
  {
	  APR_DBG_0(APR_DBG_HIGH,"Signal set for servreg availability...");
      qurt_anysignal_set(&servreg_pdr_apr_status_signal, SERVREG_TEST_SIG_LOC_AVAILABLE); //set the signal when servreg is available..
  }
}


APR_INTERNAL int32_t apr_pdr_init(void){
    
    uint64_t signal_ack = 0;
    int32_t ret=APR_EOK;
    char name[64];
    strlcpy(name, "APR_PDR_INIT_THREAD", sizeof(name));   
    //void* stack;
    qurt_apr_thread_list.stack = malloc(APR_TASK_STACK_SIZE);
    qurt_thread_t tid;
    qurt_thread_attr_t attr;
    qurt_thread_attr_init(&attr);   //Init the params
    qurt_thread_attr_set_name(&attr, name);
    qurt_thread_attr_set_stack_addr(&attr, qurt_apr_thread_list.stack);
    qurt_thread_attr_set_stack_size(&attr, APR_TASK_STACK_SIZE);
    qurt_thread_attr_set_priority(&attr, APR_HIGH_TASK_PRIORITY);
    //qurt_thread_attr_set_affinity(&attr, QURT_THREAD_ATTR_AFFINITY_DEFAULT);
           
   //launch apr_pd_init as a spearte thread..
   //This thread is responsible to register with service locatory
   apr_lock_create(APR_LOCK_TYPE_MUTEX, &apr_free_buffer_lock);
   
   //Try to init the signal..
   qurt_anysignal_init (&apr_pdr_worker_thread_ack);
   qurt_anysignal_init (&servreg_pdr_apr_status_signal);
   

   ret=qurt_thread_create(&tid, &attr, apr_pdr_workerthread, NULL);
   if (QURT_EOK != ret)
   {
     MSG_ERROR ("apr_pdr_workerthread creation failed: (%d) \n", ret, 0, 0); 
     free(qurt_apr_thread_list.stack);
     qurt_apr_thread_list.stack=NULL;
     return ret;  
   }

   //Check if thread is launched and then signal back from launched thread to be sure that thread is launched
   signal_ack = ( uint32_t )qurt_anysignal_wait(&apr_pdr_worker_thread_ack, APR_PDR_THREAD_ACK );
   if(APR_PDR_THREAD_ACK & signal_ack)
   {
         MSG_HIGH("apr pdr init sucessfull: (%d) \n", ret, 0, 0); 
         ret=APR_EOK;
   }
   else
   { 
    ret = APR_EFAILED;
   }
   
   //Register to service registry callback
   //Check for service state and if service status is up, go ahead and trigger registered callback else
   //wait for callback to be triggered from TMS
   
    /* Signal start task upon service locator availability */
	if (SERVREG_SUCCESS != servreg_register_servloc_availability_cb(apr_sr_locator_available, NULL, NULL))
	{
		APR_DBG_0(APR_DBG_HIGH,"Failed to register for locator availability");
		ret = APR_EFAILED;
	}

	if (SERVREG_SERVICE_STATE_UP == servreg_get_servloc_availability())
	{
       APR_DBG_0(APR_DBG_HIGH,"Servreg service is up to trigger pdr register");        
	   ret = APR_EOK;
	   apr_sr_locator_available(0,0);
	}

   return ret; 
}

//Register for audio pd domain status.    
APR_INTERNAL int32_t apr_pdr_register(void)
{
    uint8_t i = 0;
    SERVREG_DL_HANDLE pdr_handle;
    SERVREG_NAME domain = SERVREG_NULL;
	char * apr_local_domain = SERVREG_NULL;
    SERVREG_RESULT result;
	boolean found_domain = FALSE;
	
	/*
	Get the local domain. It can be either of the one.
	mdm/modem/audio_pd (use case for audio running as a UserPD in MDM)
    msm/adsp/audio_pd   (use case for audio running as a UserPD in ADSP/MSM)
    mdm/modem/root_pd   (use case for audio running in rootPD in MDM)
	*/
    apr_local_domain=servreg_get_local_domain(); //Retrieves one entry from above list.
	
    //Get the handle by sending allocate request..
    //Note: Make sure to free this handle when ever an excpetion is caught..    
    pdr_handle = servreg_alloc_domainlist_handle();
    if(SERVREG_NULL != pdr_handle)
    {
	    //Step1: Get domainlist handle. Make sure to free this handle when ever an exception is caught.
        result = servreg_get_domainlist(apr_svc_name, pdr_handle);
        if(SERVREG_SUCCESS == result)
        {
          for (i = 0; i < servreg_get_num_entries(pdr_handle); i++)
          {
          
		    //Get soc/domain/subdomain from the servreg table
			result = servreg_get_entry(pdr_handle, i+1, &domain);
			if((SERVREG_SUCCESS != result) || (SERVREG_NULL == domain))
            {
               MSG_ERROR ("apr_pdr_register(): servreg_get_entry() failed \n", 0, 0, 0);
               break;
			}
            		
		    //Try cascade if's
			//Try to match "domain" with any of the apr_local_domain entries
			//If it matches any entry then break the loop and register to that entry for status notifications
			
			//use case for audio running as a UserPD in MDM
			if(strncmp(domain, "mdm/modem/audio_pd", strlen("mdm/modem/audio_pd") ) == 0)
		    {
				found_domain = TRUE;
				break;
			}
			
			//use case for audio running as a UserPD in ADSP/MSM 
            if(strncmp(domain, "msm/adsp/audio_pd", strlen("msm/adsp/audio_pd") ) == 0)
			{
				found_domain = TRUE;
				break;
			}
			
			//use case for audio running in rootPD in MDM
            if(strncmp(domain, "mdm/modem/root_pd", strlen("mdm/modem/root_pd") ) == 0)
		    {
				found_domain = TRUE;
				break;
			}
			
           } //end of for loop
		  
		  if (FALSE == found_domain) 
          {
		      
              MSG_ERROR ("apr_pdr_register(): servreg_get_entry() failed to get expected domain. hence exiting from PD init \n", 0, 0, 0);
              /* Free the domain list handle  */
              if(SERVREG_SUCCESS != servreg_free_domainlist_handle(pdr_handle))
              {
                 MSG_ERROR ("apr_pdr_register(): pdr_handle free failed", 0, 0, 0);
              }
			  return APR_EFAILED;
          }
         
        } //if ends for get domain list..
        else
        {
            MSG_ERROR ("apr_pdr_register(): servreg_get_domainlist() failed \n", 0, 0, 0);
			if(SERVREG_SUCCESS != servreg_free_domainlist_handle(pdr_handle))
            {
               MSG_ERROR ("apr_pdr_register(): servreg_test_case_register_listener_remote_qdi_service() pdr_handle free failed", 0, 0, 0);
            }
			return APR_EFAILED;
        }
    } //if end for pdr handle
	
	if(SERVREG_NULL == pdr_handle)
    {
      MSG_ERROR ("apr_pdr_register(): servreg_alloc_domainlist_handle() failed \n", 0, 0, 0);
	  return APR_EFAILED;
    } 
	
    if(SERVREG_NULL != domain)
    {
       remote_apr_mon_handle = servreg_alloc_monitor_handle(domain, apr_svc_name);

	   /* Free the domain list handle once monitor allocation is done */
	   if(SERVREG_SUCCESS != servreg_free_domainlist_handle(pdr_handle))
	   {
		  MSG_ERROR ("apr_pdr_register(): servreg_test_case_register_listener_remote_qdi_service() pdr_handle free failed", 0, 0, 0);
	   }
	 
       if(SERVREG_NULL != remote_apr_mon_handle)
       {
          result = servreg_register_listener_qurt(remote_apr_mon_handle, &servreg_pdr_apr_status_signal, APR_REMOTE_SERVICE_STATUS_MASK);
          if(SERVREG_SUCCESS == result)
          {
            MSG_HIGH ("apr_pdr_register(): APR Client registered successfully with servreg_register_listener_qurt() via QDI to check apr status \n", 0, 0, 0);
			SERVREG_SERVICE_STATE apr_state = servreg_get_service_curr_state(remote_apr_mon_handle);
        
            if (SERVREG_SERVICE_STATE_DOWN == apr_state)
            {
			  upd_status =  SERVREG_SERVICE_STATE_DOWN;
			  MSG_ERROR("apr_pdr_register(): APR UPD recieved DOWN state :(%d)", apr_state, 0, 0);
            }
			else if (SERVREG_SERVICE_STATE_UP == apr_state)
			{ 
			  upd_status =  SERVREG_SERVICE_STATE_UP;
			  MSG_ERROR("apr_pdr_register(): apr_pdr recieved UPD state :(%d)", apr_state, 0, 0);
			}
			else
			{
			  MSG_ERROR("apr_pdr_register: apr_pdr recieved invalid UPD state =(%d)", apr_state, 0 , 0);
			}
          } // end of servreg_register_listener_qurt
          if(SERVREG_SUCCESS != result)
          {
               MSG_ERROR ("apr_pdr_register():APR Client registration failed with servreg_register_listener_qurt() via QDI to check apr status \n", 0, 0, 0);
               result = servreg_free_monitor_handle(remote_apr_mon_handle);
               if(SERVREG_SUCCESS != result)
               {
                  MSG_ERROR ("apr_pdr_register(): could not free the monitor handle \n", 0, 0, 0);
               }
               return APR_EFAILED;
          }
      }
      else
      {
          MSG_ERROR ("apr_pdr_register(): remote_apr_mon_handle is null", 0, 0, 0);
          return APR_EFAILED;;
      }
  }
  else
  {
     MSG_ERROR ( "apr_pdr_register(): domain entry from remote locator is null", 0, 0, 0);
	 return APR_EFAILED;
  }
  return APR_EOK;
}

//De-init sequence used for apr pdr
APR_INTERNAL uint32_t apr_pdr_deinit (  void)
{
      uint32_t result = APR_EOK;
    SERVREG_RESULT ret;
    int dummy;
    uint32_t tid;
    //Deregister from service registry
    ret = servreg_deregister_listener_qurt(remote_apr_mon_handle, &servreg_pdr_apr_status_signal, APR_REMOTE_SERVICE_STATUS_MASK);
      
    if (SERVREG_SUCCESS != ret)
    {
      MSG_ERROR ("apr_pdr_deinit(): servreg_deregister_listener_qurt() failed", ret, 0, 0);
      result = APR_EFAILED;
    }
          //Free the allocated monitor handle.
    result = servreg_free_monitor_handle(remote_apr_mon_handle);
      if(SERVREG_SUCCESS != result)
    {
      MSG_ERROR ("apr_pdr_deinit(): could not free the monitor handle \n", 0, 0, 0);
    }
    
    qurt_signal_set(&servreg_pdr_apr_status_signal, APR_PDR_THREAD_COMPLETED_ACK);
    tid=qurt_apr_thread_list.tid;
    qurt_thread_join(tid,&dummy); //wait for thread to get completed.
      if (qurt_apr_thread_list.stack != NULL)
      { //Making sure double time free is not called
        free(qurt_apr_thread_list.stack);//free up the stack
    }
      
      qurt_signal_destroy(&apr_pdr_worker_thread_ack);
    qurt_signal_destroy(&servreg_pdr_apr_status_signal);
  return result;
}

/*  When the UPD restarts, this function is used to clean up the existing UPD packets
At the same time informs the packet owners on the packet delivery failure */
APR_INTERNAL int32_t aprv2_pd_restart_call(void)
{
  aprv2_mpd_uapr_t *puapr;
  //aprv2_mpd_packet_item_t *ppacket_item;
  int32_t ret;
  //aprv2_packet_t *packet;
  uint16_t dst_domain_id=0x4;
  apr_list_node_t *cur_node, *dummy_node;
  aprv2_mpd_uapr_item_t *cur_uapr_item;

  /* Find if there exists an uAPR with a matching domain ID*/
  /* As the UPD packets are also freed up by qdi release too. So lock it before cleaning up */
  apr_lock_enter(apr_free_buffer_lock); 
  dummy_node = &aprv2_mpd_used_uapr_q.dummy;
  cur_node = dummy_node->next;

  MSG_HIGH ("UPD restarted, cleaning up UPD resources", 0, 0, 0);
  while (cur_node != dummy_node)
  {
      cur_uapr_item = (aprv2_mpd_uapr_item_t *)cur_node;
      if (dst_domain_id == cur_uapr_item->uapr.domain_id)
      break;
      cur_node = cur_node->next;
  }
  //If NO UPD exists, then we assume that cur_node and next node(dummy node) are equal
  if (cur_node == dummy_node)
  {        
  MSG_HIGH ("No UPD exists to clean up", 0, 0, 0);
  apr_lock_leave(apr_free_buffer_lock); //Make sure to release the lock while returning.
  return APR_EOK;
  }
  //step2: Free up all the rx packet queue for the uapr(freeing up rx packet is based on domain)
  //This will dynamically pull in the pointer based on the Process domain we need to clean up
  puapr=&cur_uapr_item->uapr;
  qurt_signal_destroy(puapr->rx_signal); //Destroy the signal so that uapr is not signalled on any packet arrival
  qurt_qdi_user_free(puapr->client_handle, puapr->rx_signal);  //Free qdi user
  
  //Iterate through on rx buffer queue and free it 
  APR_DBG_1(APR_DBG_HIGH,"Size of APR UPD packets before freeing :(%d)",puapr->rx_packet_q.size );
  ret=aprv2_uapr_free_pkts(puapr);
  if (ret != APR_EOK)
  {
      APR_DBG_0(APR_DBG_ERROR,"Error in freeing the uapar resources");
      
  }
   APR_DBG_1(APR_DBG_HIGH,"Size of APR UPD packets after freeing: (%d)",puapr->rx_packet_q.size );  
     
   apr_lock_leave(apr_free_buffer_lock); //Release the lock before returning
   return APR_EOK;
}

//This is called from the thread created as a part of APR bootup
//This will talk to service registry
//When ever service locatory is available, apr will register itself for UPD service notificaitons
//If UPD receives down notification then UPD resources are freed up
APR_INTERNAL void apr_pdr_workerthread(void *param)
{   
   qurt_signal_set(&apr_pdr_worker_thread_ack, APR_PDR_THREAD_ACK);// This will ensure that thread had spawned worker thread sucessfully...
   SERVREG_RESULT ret = SERVREG_FAILURE;
   int32_t rc = APR_EOK;
   uint64_t signal = 0;
   uint32_t transaction_id = 0;
      
   //Get the tid
   qurt_apr_thread_list.tid=qurt_thread_get_id();
  //Wait on a signal for UPD notifications  
   for(;;)
   {
       APR_DBG_0(APR_DBG_HIGH,"Start waiting on a signal for UPD notification");   
       signal = ( uint32_t )qurt_anysignal_wait(&servreg_pdr_apr_status_signal, SERVREG_TEST_SIG_LOC_AVAILABLE|APR_REMOTE_SERVICE_STATUS_MASK|APR_PDR_THREAD_COMPLETED_ACK );
       APR_DBG_1(APR_DBG_HIGH,"Received signal with mask or signal : (%d)",signal); 
       
    //Kill the child thread once the signal is rcved from deinit
       if (signal == APR_PDR_THREAD_COMPLETED_ACK)
       {
           MSG_HIGH ("Received de-init signal, exiting from this thread: (%d) ", signal ,0 ,0);
           qurt_thread_exit(0);
           break;
       }
   
       if(SERVREG_TEST_SIG_LOC_AVAILABLE & signal)
       { 
       //signal is available for service registry , now call domainlist
           qurt_anysignal_clear(&servreg_pdr_apr_status_signal, SERVREG_TEST_SIG_LOC_AVAILABLE);
           ret = apr_pdr_register(); //register for AUDIO PD status notifications.
           if(ret)
           { 
                MSG_ERROR ("apr_pdr_register failed:: %d ", ret ,0 ,0);
                break; 
           }
       }//End service registration signal handling
       
       //This mask is set when UPD notifications are available.
       if (APR_REMOTE_SERVICE_STATUS_MASK & signal)
       {
           //Check on what signal is received. This can be queried by using the below API
           apr_state = servreg_get_service_curr_state(remote_apr_mon_handle);
          APR_DBG_1(APR_DBG_HIGH,"Querying apr state done, Received UPD status as : (%d)", apr_state);
          
          //Clear the signal which was set so that further signals can be handled
          qurt_anysignal_clear(&servreg_pdr_apr_status_signal, APR_REMOTE_SERVICE_STATUS_MASK);
          
          switch (apr_state)
          {

            case SERVREG_SERVICE_STATE_DOWN:
                //Set the UPD status first & then free up the UPD resources.
                upd_status =  SERVREG_SERVICE_STATE_DOWN;
                rc=aprv2_pd_restart_call(); //Clean up the UPD resources
                if (rc){MSG_ERROR( "PD restart clean up failed :(%d)", rc, 0, 0 );}
                break;
            case SERVREG_SERVICE_STATE_UP:
                //As of now re-enable the variable so that apr_mpd_send can perform its task...
                upd_status =  SERVREG_SERVICE_STATE_UP;
                APR_DBG_1(APR_DBG_HIGH,"apr pd state is up %d", apr_state);
                break;
            default:
                MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"apr pdr(): apr_pdr recieved invalid apr state =(%d)",apr_state );
                break;
          }
          //Get the transcation id so that we can send the ack.
          //Sending ack is mandatory
          transaction_id = servreg_get_transaction_id(remote_apr_mon_handle);
          ret = servreg_set_ack(remote_apr_mon_handle, transaction_id); 
       }
   }
  return;
}


#endif