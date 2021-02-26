#ifndef __APRV2_MPD_I_H__
#define __APRV2_MPD_I_H__

/*===========================================================================
 
  @file aprv2_mpd_if.h
 
  Generic interface used to communicate from user-space APR to kernel-space
  and vice-versa.
 
 
  User-space APR instance communicates with the Kernel-space APR instance and
  vice-versa. Both use the interface defined here for: registering/deregistering
  and sending/receiving messages. The implmentation of this interface is HLOS
  specific. This module hides all HLOS specific detailes from the rest of APR.
 
  As the nature of interaction from user->kernel is different than kernel->user
  there will be a separate implementation of this module for user and kernel APR.



Copyright (C) 1997-2009 by QUALCOMM Technologies, Inc.
All Rights Reserved.
===========================================================================*/
/*
  Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/mpd/inc/aprv2_mpd_i.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"


typedef enum aprv2_mpd_event{
  APRV2_MPD_EV_RX_DATA,
  APRV2_MPD_EV_SEND_FAILED, 
} aprv2_mpd_event_t;

typedef int32_t ( *aprv2_mpd_cb_fn_t) (aprv2_mpd_event_t ev, void* payload, uint32_t sz);

APR_INTERNAL int32_t aprv2_mpd_init ( void );
APR_INTERNAL int32_t aprv2_mpd_deinit ( void );
APR_INTERNAL int32_t aprv2_pd_restart_call(void);
APR_INTERNAL int32_t apr_pdr_register(void);
APR_INTERNAL void apr_pdr_workerthread(void *param);
APR_INTERNAL int32_t apr_pd_service_monitor(void *param);
APR_INTERNAL uint32_t apr_pdr_deinit ( void);
APR_INTERNAL int32_t apr_pdr_init ( void);


/*
  Register a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_register(uint16_t addr );

/*
  Deregister a service with Kernel APR.
 
  @param addr address of service being registered.
*/
APR_INTERNAL int32_t aprv2_mpd_deregister(uint16_t addr );

/*
  Send a packet to he dst address specified in the packet. In case
  of UAPR the packet is sent to Kernel. In-case of KAPR the packet gets
  sent to the correct destination based on dst ID. 
 
  @param packet apr packet to send
*/
APR_INTERNAL int32_t aprv2_mpd_send(aprv2_packet_t* packet );


/*
  Register callback with mpd service. This call-back will get invoked
  for every received message.
 
  @param pfn_rx_cb callback function
*/
APR_INTERNAL int32_t aprv2_mpd_set_rx_cb(aprv2_mpd_cb_fn_t pfn_rx_cb);

#endif /* __APRV2_MPD_I_H__ */

