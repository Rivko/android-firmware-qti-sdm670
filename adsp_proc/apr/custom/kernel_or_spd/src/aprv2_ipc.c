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
#ifdef USE_INIT_IPC_HACK
#include "aprv2_api.h"
#include "aprv2_api_i.h"
#endif /*USE_INIT_IPC_HACK*/
#include "aprv2_pl_config.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/
#ifdef USE_INIT_IPC_HACK
aprv2_ipc_send_fn_t aprv2_ipc_send_fn = aprv2_ipc_send_dummy;
#endif /*USE_INIT_IPC_HACK*/
#define S APR_UPD 
#define T APR_RPD 
#define U APR_SPD 
/*****************************************************************************
 * Core Routine Implementations                                              *
 ****************************************************************************/
void aprv2_ipc_glink_link_state_cb (glink_link_info_type *link_info, void *priv)
{
#ifndef SIM_DEFINED 
  int32_t rc = APR_EOK;
  char_t *port_name;
  char_t *remote_ss;
  int32_t channel_id;
  int32_t port_index, index;
 
  APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): QDSP Starting to open GLINK channels");

  if ( link_info->link_state != GLINK_LINK_STATE_UP )
  {
     APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): link_info->link_state != GLINK_LINK_STATE_UP ");
     return;
  }


  for (index=0; index < GLINK_PORTS; index++) 
  {
     remote_ss = ipc_ports_pointer[index].remote_ss;
     if ( mmstd_strncmp( link_info->remote_ss, sizeof(link_info->remote_ss), remote_ss, sizeof(remote_ss)) == 0 )
     {
        port_name = ipc_ports_pointer[index].port_name;
        channel_id= ipc_ports_pointer[index].channel_id;
        port_index = ipc_ports_pointer[index].index;
        remote_ss = ipc_ports_pointer[index].remote_ss;
        if (APR_EOK != (rc = apr_glink_open(port_name,sizeof(port_name),channel_id, port_index, remote_ss)))
        {
           (void)apr_glink_close(port_index);  
        }
        APR_DBG_2(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): Glink Open for port_index: %d, result: %d", port_index, rc);
     }
  }
#endif /* SIM_DEFINED */
  return;
}

APR_INTERNAL int32_t aprv2_ipc_init ( void )
{

 
#ifndef SIM_DEFINED
  int32_t rc = APR_EOK;
  uint32_t glink_init_completed = 0;
  #ifdef modem_proc
  uint32_t diag_init_completed=0;
  #endif
  

  APR_DBG_0(APR_DBG_HIGH, "QDSP aprv2_ipc_init starting initialization sequence.");



  for ( ;; )
  {

    if (DIAG_PORTS != 0)
    {
#ifdef modem_proc
      rc = aprv2_diag_init( );
      if ( rc ) break;
      diag_init_completed = 1;
#endif
      
    }
    
    if ( GLINK_PORTS != 0 )
    {
       ipc_ports_pointer = glink_port_data;
       aprv2_ipc_send_dispatch_table=aprv2_ipc_glink_send_dispatch_table;
       rc = apr_glink_init();
       if ( rc ) break;
       glink_init_completed = 1;
     }
       
      APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_init succeed.");
      return APR_EOK;
   } //for end
   
   
#ifdef modem_proc
  if (diag_init_completed == 1 ){(void) aprv2_diag_deinit();  }
#endif
  if (glink_init_completed == 1 ){(void) apr_glink_deinit();  }


  APR_DBG_1(APR_DBG_ERROR,"MODEM aprv2_ipc_init failed after checkpoint=rc.", rc);
  return rc;
#endif /* SIM_DEFINED */ 

#ifdef SIM_DEFINED
  aprv2_ipc_send_dispatch_table=aprv2_ipc_glink_send_dispatch_table;
  return 0;
#endif

}

APR_INTERNAL int32_t aprv2_ipc_deinit ( void )
{
#ifndef SIM_DEFINED
  int32_t port_index;
  APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_deinit start.");


    if (GLINK_PORTS != 0) 
    {
       for ( port_index = 0; port_index < GLINK_PORTS; port_index++)
       {
          apr_glink_close(port_index);
       }
       (void) apr_glink_deinit( );
    }
  
#ifdef modem_proc
    if (DIAG_PORTS != 0){ ( void ) aprv2_diag_deinit( ); }
#endif
#endif /* SIM_DEFINED */

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_ipc_is_domain_local (uint16_t domain_id)
{
  if ( ( domain_id == APRV2_PL_DEFAULT_DOMAIN_ID_V ) || ( domain_id == APRV2_PL_DEFAULT2_DOMAIN_ID_V ) )
  {
    return APR_EOK;
  }
  else
  { 
    return APR_EFAILED;
  }
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

APR_INTERNAL int32_t apr_ipc_write_fn(uint32_t port_index,void *buf, uint32_t length)
{
  
#ifndef SIM_DEFINED
   return apr_glink_tx(port_index, buf, length);
#endif

#ifdef SIM_DEFINED
   return APR_EOK;
#endif
}

APR_INTERNAL int32_t aprv2_ipc_send (aprv2_packet_t* packet)
{
  uint16_t dst_domain_id;
  uint32_t packet_len,port_handle;

  if ( packet == NULL )
  {
    return APR_EBADPARAM;
  }
 
  packet_len =  APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );
  port_handle = aprv2_ipc_send_dispatch_table[dst_domain_id].port_handle_index;
  return aprv2_ipc_send_dispatch_table[dst_domain_id].dispatch_data(port_handle, packet,packet_len);
}

APR_INTERNAL int32_t apr_diag_send_fn(uint32_t port_handle, void *buf, uint32_t length)
{
   int32_t rc=APR_EOK;
   
#ifdef modem_proc
   rc = aprv2_diag_send( buf );
#endif

   return rc;
}

APR_INTERNAL int32_t aprv2_ipc_free (aprv2_packet_t *packet)
{
#ifndef SIM_DEFINED
  uint16_t src_domain_id;
  int32_t port_index;

  if (packet == NULL)
  {
    return APR_EBADPARAM;
  }

   src_domain_id  = APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->src_addr);
   port_index = aprv2_ipc_send_dispatch_table[src_domain_id].port_handle_index;
   return apr_glink_rx_done(port_index, packet);
#endif

#ifdef SIM_DEFINED
   return APR_EOK;
#endif
}


APR_INTERNAL int32_t aprv2_ipc_mpd_send(uint32_t port_handle, void *packet, uint32_t length)
{
   
   return aprv2_mpd_send(packet);
}

APR_INTERNAL int32_t aprv2_ipc_unsupport_domain(uint32_t port_handle, void *buf,uint32_t length)
{
   return APR_EUNSUPPORTED;
}

APR_INTERNAL int32_t aprv2_ipc_local(uint32_t port_handle, void *buf, uint32_t length)
{
   return APR_ELPC;
}