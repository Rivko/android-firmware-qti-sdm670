/*=============================================================================
  Copyright (C) 2010-2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/src/apr_glink.c
  $Author: svutukur $

when         who   what, where, why
----------   ---   ------------------------------------------------------------

=============================================================================*/
#include "msg.h"
#include "mmstd.h"
#include "apr_dbg_msg.h"
#include "aprv2p1_packet.h"
#include "aprv2_ids_domains.h"
#include "aprv2_api_inline.h"
#include "aprv2_diag.h"
#include "aprv2_ipc.h"
#include "apr_glink.h"
#include "aprv2_mpd_i.h"
#include "aprv2_ipc_config.h"
/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/
#ifdef USE_INIT_IPC_HACK
aprv2_ipc_send_fn_t aprv2_ipc_send_fn = aprv2_ipc_send_dummy;
#endif /*USE_INIT_IPC_HACK*/

/*****************************************************************************
 * Core Routine Implementations                                              *
 ****************************************************************************/
APR_INTERNAL int32_t aprv2_mpd_cb (aprv2_mpd_event_t ev, void* payload, uint32_t sz)
{
  int32_t rc = APR_EOK;
  int32_t handle;
  aprv2_packet_t* new_packet;
  
  switch(ev)
  {
      case APRV2_MPD_EV_RX_DATA: /* received rx data */
         new_packet = ( ( aprv2_packet_t* ) payload );
         handle = ( ( uint32_t ) ( ( ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, new_packet->dst_addr ) ) << 8 ) |
                              ( APR_GET_FIELD( APRV2_PKT_SERVICE_ID, new_packet->dst_addr ) ) ) );
         ( void ) __aprv2_cmd_forward( handle, new_packet );
         break;

      case APRV2_MPD_EV_SEND_FAILED:
         rc = APR_EFAILED;
         break;
  }

  if( rc )
  {
    MSG_ERROR( "ADSP aprv2_mpd_cb failed, rc = %d." , rc, 0, 0 );
  }
  return rc;
}

APR_INTERNAL int32_t aprv2_ipc_get_default_domain_id ( uint16_t *ret_domain_id )
{
  if ( ret_domain_id == NULL )
  {
    return APR_EBADPARAM;
  }
#ifdef modem_proc
  *ret_domain_id = APRV2_IDS_DOMAIN_ID_MODEM_V;
#elif adsp_proc
   *ret_domain_id = APRV2_IDS_DOMAIN_ID_ADSP_V;
#elif cdsp_proc
    *ret_domain_id = APRV2_IDS_DOMAIN_ID_CDSP_V;
#else
	*ret_domain_id = APRV2_IDS_DOMAIN_ID_SDSP_V;
#endif

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_ipc_init ( void )
{
  int32_t rc = APR_EOK;
  
  APR_DBG_0(APR_DBG_HIGH,"MODEM aprv2_ipc_init starting initialization sequence.");


    
       aprv2_mpd_set_rx_cb(aprv2_mpd_cb); 
       aprv2_ipc_send_dispatch_table=aprv2_ipc_glink_send_dispatch_table;
       ipc_ports_pointer=glink_port_data;
       return APR_EOK;
       APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_init succeed.");
   

    return rc;
}

APR_INTERNAL int32_t aprv2_ipc_mpd_send(uint32_t port_handle, void *packet, uint32_t length)
{
   return aprv2_mpd_send(packet);
}

APR_INTERNAL int32_t aprv2_ipc_deinit ( void )
{
  APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_deinit start.");
  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_ipc_is_domain_local (uint16_t domain_id)
{
  if ((domain_id == APRV2_PL_DEFAULT_DOMAIN_ID_V) ||(domain_id == APRV2_PL_DEFAULT2_DOMAIN_ID_V))
  {
     return APR_EOK;
  }
  else
  { 
    return APR_EFAILED;
  }
}

APR_INTERNAL int32_t aprv2_ipc_send (aprv2_packet_t* packet)
{
  uint16_t dst_domain_id;
  uint32_t packet_len, port_handle;

  if ( packet == NULL )
  {
    return APR_EBADPARAM;
  }

  packet_len =  APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );
  port_handle = aprv2_ipc_send_dispatch_table[dst_domain_id].port_handle_index;
  return aprv2_ipc_send_dispatch_table[dst_domain_id].dispatch_data(port_handle, packet,packet_len);
}

APR_INTERNAL int32_t aprv2_ipc_unsupport_domain(uint32_t port_handle, void *buf,uint32_t length)
{

   return APR_EUNSUPPORTED;
}

APR_INTERNAL int32_t aprv2_ipc_local(uint32_t port_handle, void *buf, uint32_t length)
{
   
   return APR_ELPC;
}



APR_INTERNAL int32_t aprv2_ipc_free (aprv2_packet_t *packet)
{
   return APR_EOK;
}