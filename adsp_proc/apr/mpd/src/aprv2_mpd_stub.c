/*
  Copyright (C) 2010-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/mpd/src/aprv2_mpd_stub.c#1 $
  $Author: pwbldsvc $
*/
#include "apr_errcodes.h"
#include "aprv2_packet.h"
#include "aprv2_mpd_i.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/


/*****************************************************************************
 * Data Structures                                                           *
 ****************************************************************************/


/*****************************************************************************
 * Declarations                                                              *
 ****************************************************************************/


/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/


/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/


APR_INTERNAL int32_t aprv2_mpd_init ( void )
{

   /* Stubbing out
   */
   return APR_EOK;
}

APR_INTERNAL int32_t aprv2_mpd_deinit ( void )
{

   /* Stubbing out
   */
   return APR_EOK;
}

/*
  Register a service
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_register(uint16_t addr )
{

   /* Stubbing out
   */
   return APR_EOK;
}

/*
  Deregister a service
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_deregister(uint16_t addr )
{

   /* Stubbing out
   */
   return APR_EOK;
}

/*
 * Send a packet to the dst address specified in the packet.
 *
 * @param packet apr packet to send
*/
APR_INTERNAL int32_t aprv2_mpd_send(aprv2_packet_t* packet )
{

   /* Stubbing out
   */
   return APR_ENOTEXIST;
}

/*
  Register callback with mpd service. This call-back will get invoked
  for every received message.
 
  @param pfn_rx_cb callback function
*/
APR_INTERNAL int32_t aprv2_mpd_set_rx_cb(aprv2_mpd_cb_fn_t pfn_rx_cb)
{

   /* Stubbing out
   */

   return APR_EOK;
}
