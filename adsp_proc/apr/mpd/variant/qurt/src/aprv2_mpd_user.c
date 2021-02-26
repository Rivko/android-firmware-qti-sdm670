/*
  Copyright (C) 2014,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/mpd/variant/qurt/src/aprv2_mpd_user.c#2 $
  $Author: pwbldsvc $
*/

#include "aprv2_mpd_i.h"
#include "aprv2_mpd_qurt_i.h"
#include "aprv2_api_inline.h"
#include "msg.h"
#ifdef FEATURE_CENTRALIZED_THREAD_PRIORITY
#include "rcinit.h"
#endif
#include "err.h"
#include "mmstd.h"
#include <qurt.h>
#include <qurt_qdi_driver.h>
#include <qurt_signal.h>
#include <qurt_thread.h>

#define APRV2_KDRV_NAME ( "qc_apr" )
#define APR_QDI_THREAD_PRI_ORDER ( 50 )
#define APR_QDI_THREAD_STACK_SIZE 8192

#define APR_QDI_SIG_EXIT 0x00000002
#define APR_QDI_SIG_DATA_RX APRV2_MPD_SIG_RX_PACKET //0x0000001
// #define APR_QDI_SIGNAL_
  /* MSG_ERROR(call##" failed with error %d", qurt_rc, 0,0);\ */
#define EC_QURT(call, op)\
do{\
int32_t qurt_rc = 0;\
if( !((qurt_rc = call) op 0) )\
{\
  MSG_ERROR(#call" failed with error %d", qurt_rc, 0,0);\
  goto error;\
}\
}while(0)
#define EC_GTEZ(call) EC_QURT(call, >=) // call >= 0
#define EC_GTZ(call) EC_QURT(call, >)   // call > 0
#define EC_EQZ(call) EC_QURT(call, ==)  // call == 0

typedef struct aprv2_mpd_user_ctxt_t
{
  uint32_t qdi_handle; 
  qurt_signal_t* qdi_signal_handle;

  aprv2_mpd_cb_fn_t rx_cb;
  struct
  {
    uint8_t stack[ APR_QDI_THREAD_STACK_SIZE ];
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    void* param;
    int32_t result;
  } listener_thread;

} aprv2_mpd_user_ctxt_t;

aprv2_mpd_user_ctxt_t aprv2_mpd_user_ctxt;


static void aprv2_qdi_thread_entry (
  void* param
)
{
  uint32_t signal_mask = 0;
  aprv2_packet_t packet_header;
  uint32_t packet_size = 0, bytes_read = 0;
  uint32_t alloc_type = 0;
  aprv2_packet_t* new_packet = NULL;
  int32_t rc = APR_EOK;



  MSG_HIGH("starting QDI User thread.", 0, 0,0);

  /* If there are any error scenarios handle them gracefully, log them and
   * continue. DO NOT EXIT.
  */
  while ( 1 )
  {
    signal_mask = qurt_signal_wait( aprv2_mpd_user_ctxt.qdi_signal_handle, 
                                    APR_QDI_SIG_EXIT|APR_QDI_SIG_DATA_RX, 
                                    QURT_SIGNAL_ATTR_WAIT_ANY  );

    if( signal_mask & APR_QDI_SIG_EXIT )
    {
      /* exit requested */
      //qurt_signal_clear( aprv2_mpd_user_ctxt.qdi_signal_handle
      //                   APR_QDI_SIG_EXIT ); 
      break;
    }

    if( signal_mask & APR_QDI_SIG_DATA_RX)
    {
      qurt_signal_clear(aprv2_mpd_user_ctxt.qdi_signal_handle, APR_QDI_SIG_DATA_RX);

      while (1)
      {
        if (APR_EOK != (rc = __aprv2_mpd_cmd_peek_rx_packet(
                         aprv2_mpd_user_ctxt.qdi_handle, &packet_header)))
        {
          /* Done reading packets */
          break;
        }

        packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet_header.header );

        alloc_type = ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet_header.header ) ==
                       APRV2_PKT_MSGTYPE_CMDRSP_V ) ?
                      APRV2_ALLOC_TYPE_RESPONSE_RAW :
                      APRV2_ALLOC_TYPE_COMMAND_RAW );

        /* SUPER_HACK: Allocate memory from destination service's packet pool */
        if (APR_EOK != (rc = __aprv2_cmd_alloc( packet_header.dst_addr,
                                 alloc_type, packet_size, &new_packet )))
        {
          MSG_HIGH("Packet allocation failed... ret=0x%x", rc, 0, 0);
          break;
        }


        if (APR_EOK != (rc = __aprv2_mpd_cmd_read_rx_packet(
                  aprv2_mpd_user_ctxt.qdi_handle, (uint8_t *)new_packet,
                                                   packet_size, &bytes_read)))
        {
          __aprv2_cmd_free(packet_header.dst_addr, new_packet);
          MSG_HIGH("Packet read failed... ret=0x%x", rc, 0, 0);
          break;
        }

      
        /* invoke clients call-back */
        if (APR_EOK != (rc = aprv2_mpd_user_ctxt.rx_cb(APRV2_MPD_EV_RX_DATA,
                                  (void*)new_packet, sizeof(aprv2_packet_t))))
        {
          MSG_HIGH("Callback to uAPR IPC failed... ret=0x%x", rc, 0, 0);
          break;
        }

        new_packet = NULL;
      }
    }
  }

  MSG_HIGH("Thread exiting ...", 0, 0, 0);

  return;
}


APR_INTERNAL int32_t aprv2_mpd_init ( void )
{
  void* pStack = NULL, *pStackAligned = NULL;
  unsigned short prio = 0;
  uint32_t alignedStackSize = 0;

  mmstd_memset( (void*)&aprv2_mpd_user_ctxt, 0, sizeof( aprv2_mpd_user_ctxt_t ) );

  /* open the QDI handle to kernel */
  aprv2_mpd_user_ctxt.qdi_handle = -1; 
  EC_GTEZ ( aprv2_mpd_user_ctxt.qdi_handle = qurt_qdi_open( APRV2_KDRV_NAME ) );

  /* Get the signal object from Kernel, this will be signalled when
     ever kernel needs to send a message to us */
  //get_rx_signal.ret_signal_group_handle = &aprv2_mpd_user_ctxt.qdi_signal_group_handle;
  EC_EQZ( __aprv2_mpd_cmd_get_rx_signal(aprv2_mpd_user_ctxt.qdi_handle, &aprv2_mpd_user_ctxt.qdi_signal_handle));

#ifdef FEATURE_CENTRALIZED_THREAD_PRIORITY
  { /* Lookup task info */
    RCINIT_INFO info_handle = NULL;
    RCINIT_PRIO rcinit_prio = 0;

    info_handle = rcinit_lookup("APR_QDI_USR");

    if (!info_handle)
    {
       ERR_FATAL("APR_QDI_USR task info not found", 0, 0, 0);
    }
    else
    {
       rcinit_prio = rcinit_lookup_prio_info(info_handle);
       alignedStackSize = (uint32_t)rcinit_lookup_stksz_info(info_handle);
       if ((rcinit_prio > 255) || (alignedStackSize == 0)) 
       {
         ERR_FATAL("Invalid Priority:%d or Stack Size: %d", rcinit_prio, alignedStackSize, 0);
       }

       /*
        * Normalizing priority to apr_thread_priority (0=Highest and 255=Lowest)
        * as priorities obtained from rcinit (rcinit_task_prio.csv) follow the
        * opposite i.e. 0=Lowest and 255=Highest.
       */
       prio = 255 - (unsigned short)rcinit_prio;
    }
  }
#else
  prio = APR_QDI_THREAD_PRI_ORDER;
  alignedStackSize = APR_QDI_THREAD_STACK_SIZE;
#endif

  /* Calculate aligned stack pointer */
  pStack = ( void* )( &aprv2_mpd_user_ctxt.listener_thread.stack );
  pStackAligned = ( void* )( (uint32_t)((uint32_t)pStack + 127) & (~127) );

  /* Calculate aligned stack size. */
  alignedStackSize = 
    (unsigned int)(alignedStackSize-((uint32_t)pStackAligned - (uint32_t)pStack));
  alignedStackSize -= alignedStackSize % 128;

  qurt_thread_attr_init (&aprv2_mpd_user_ctxt.listener_thread.attr );
  qurt_thread_attr_set_stack_size( 
    &aprv2_mpd_user_ctxt.listener_thread.attr, alignedStackSize );
  qurt_thread_attr_set_stack_addr( 
    &aprv2_mpd_user_ctxt.listener_thread.attr, pStackAligned );
  qurt_thread_attr_set_priority ( 
    &aprv2_mpd_user_ctxt.listener_thread.attr, prio );
  qurt_thread_attr_set_name( 
    &aprv2_mpd_user_ctxt.listener_thread.attr,  "APR_QDI_USR");

  EC_EQZ( qurt_thread_create( &aprv2_mpd_user_ctxt.listener_thread.tid,             
                              &aprv2_mpd_user_ctxt.listener_thread.attr,              
                              aprv2_qdi_thread_entry,      
                              &aprv2_mpd_user_ctxt));


  return APR_EOK;

error: 
  if( aprv2_mpd_user_ctxt.qdi_handle != -1)
  {
    qurt_qdi_close( aprv2_mpd_user_ctxt.qdi_handle );
  }
  if( aprv2_mpd_user_ctxt.listener_thread.tid != 0 )
  {
    int status = 0;
    qurt_signal_set((qurt_signal_t*)aprv2_mpd_user_ctxt.qdi_signal_handle, APR_QDI_SIG_EXIT);
    qurt_thread_join(aprv2_mpd_user_ctxt.listener_thread.tid, &status);
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t aprv2_mpd_deinit ( void )
{
  int status = 0;
  /* signal QDI thread to exit */
  qurt_signal_set((qurt_signal_t*)aprv2_mpd_user_ctxt.qdi_signal_handle, APR_QDI_SIG_EXIT);
  qurt_thread_join(aprv2_mpd_user_ctxt.listener_thread.tid, &status);
  if( status != QURT_EOK && status != QURT_ENOTHREAD)
  {
    goto error;
  }


  EC_EQZ( qurt_qdi_close( aprv2_mpd_user_ctxt.qdi_handle ));

  return APR_EOK;

error:
  return APR_EFAILED;
}

/*
  Register a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_register(uint16_t addr )
{
  EC_GTZ ( aprv2_mpd_user_ctxt.qdi_handle );
  EC_EQZ( __aprv2_mpd_cmd_register(aprv2_mpd_user_ctxt.qdi_handle, addr));


  return APR_EOK;

error: 
  return APR_EFAILED;

}

/*
  Deregister a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_deregister(uint16_t addr )
{
  if(aprv2_mpd_user_ctxt.qdi_handle )
  {
    EC_EQZ( __aprv2_mpd_cmd_deregister(aprv2_mpd_user_ctxt.qdi_handle, addr));
  }
  return APR_EOK;

error: 
  return APR_EFAILED;
}

/*
  Send a packet to the kernel. 
 
  @param packet apr packet to send
*/
APR_INTERNAL int32_t aprv2_mpd_send(aprv2_packet_t* packet )
{
  uint32_t size;

  if(packet)
  {
    size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );

    EC_EQZ( __aprv2_mpd_cmd_async_send(aprv2_mpd_user_ctxt.qdi_handle, (uint8_t *)packet, size));

    __aprv2_cmd_free(packet->dst_addr, packet);

  }
  else
  {
    return APR_EBADPARAM;
  }

  return APR_EOK;

error: 
  return APR_EFAILED;
}


/*
  Register callback with mpd service. This call-back will get invoked
  for every received message.
 
  @param pfn_rx_cb callback function
*/
APR_INTERNAL int32_t aprv2_mpd_set_rx_cb(aprv2_mpd_cb_fn_t pfn_rx_cb)
{
  aprv2_mpd_user_ctxt.rx_cb = pfn_rx_cb;
  
  EC_EQZ( __aprv2_mpd_cmd_set_upd_state(aprv2_mpd_user_ctxt.qdi_handle, 1));
 

  return APR_EOK;
  
  error: 
    return APR_EFAILED;

}
