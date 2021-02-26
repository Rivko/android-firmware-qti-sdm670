/******************************************************************************

                          N E T M G R _ T C . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_tc.h
  @brief   Network Manager traffic control header file

  DESCRIPTION
  Header file for NetMgr Linux traffic control interface.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010, 2016, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_TC_H__
#define __NETMGR_TC_H__

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "ds_list.h"
#include "netmgr_config.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Constant used when QoS flow specification not available
---------------------------------------------------------------------------*/
#define NETMGR_TC_DEFAULT_PRIORITY   NETMGR_TC_CLASS_PRIO_BESTEFFORT
#define NETMGR_TC_DEFAULT_DATARATE   (8UL)  /* bps units; tc rejects 0 */
#define NETMGR_TC_DEFAULT_BURST      1600
/* Maximum bandwidth for network interface root qdisc. */
/* Note: each kernel interface will use same value, but underlying
 * transport may not be able to support n*MAX bandwidth. */
#define NETMGR_TC_MAX_DATARATE    (800000000UL)  /* bps units */
#define NETMGR_TC_BASE_OFFSET     (5000000UL)  /* bps units */


/*---------------------------------------------------------------------------
   Type representing enumeration of traffic control flow states
---------------------------------------------------------------------------*/
typedef enum {
  NETMGR_TC_FLOW_NULL,                   /* Internal value           */
  NETMGR_TC_FLOW_INIT,                   /* Initialization state     */
  NETMGR_TC_FLOW_ACTIVE,                 /* QoS scheduling active    */
  NETMGR_TC_FLOW_SUSPENDED,              /* QoS scheduling inactive  */
  NETMGR_TC_FLOW_DISABLED                /* Datapath flow controlled */
} netmgr_tc_flow_state_t;


/*---------------------------------------------------------------------------
  Type representing enumeration of TC class priority.
  Note: Precedence values are in descending value order
---------------------------------------------------------------------------*/
typedef enum {
  NETMGR_TC_CLASS_PRIO_MIN             = 7,
  NETMGR_TC_CLASS_PRIO_BESTEFFORT      = NETMGR_TC_CLASS_PRIO_MIN,
  NETMGR_TC_CLASS_PRIO_BACKGROUND      = 4,
  NETMGR_TC_CLASS_PRIO_INTERACTIVE     = 3,
  NETMGR_TC_CLASS_PRIO_STREAMING       = 2,
  NETMGR_TC_CLASS_PRIO_CONVERSATIONAL  = 1,
  NETMGR_TC_CLASS_PRIO_MAX             = 0
} netmgr_tc_class_priority_type_t;


typedef struct netmgr_tc_filter_data_s
{
  uint32        flow_id;
  uint8         rule_id;
  unsigned char precedence;
  uint8         ip_version;
} netmgr_tc_filter_data;

/*===========================================================================
                            GLOBAL FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  netmgr_tc_init
===========================================================================*/
/*!
@brief
 Main initialization routine of the traffic control module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
void netmgr_tc_init (int nlink, netmgr_ctl_port_config_type links[]);


/*===========================================================================
  FUNCTION  netmgr_tc_create_delete_dynamic_post_routing_rule
===========================================================================*/
/*!
@brief
 Adds/removes source and interface iptable rules in post routing chain of
mangle table to reset skb->mark to zero if there are no matching rules.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_tc_create_delete_dynamic_post_routing_rule
(
  int link,
  int ip_family,
  netmgr_address_info_t  *addr_info_ptr,
  int create_chain
);

/*===========================================================================
  FUNCTION  netmgr_tc_get_qos_params_by_profile_id
===========================================================================*/
/*!
@brief
  Lookup the datarate and priority QoS parameters based on CDMA profile ID.

@return
  int - NETMGR_SUCCESS on successful operations, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
int netmgr_tc_get_qos_params_by_profile_id
(
  uint16      profile_id,
  uint32    * datarate,
  uint8     * priority
);

/*===========================================================================
  FUNCTION  netmgr_tc_flow_update_data_rate_hdlr
===========================================================================*/
/*!
@brief
 Update the traffic control elements based on a configured throughput
 indication.

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_tc_flow_update_data_rate_hdlr
(
  int       link,
  uint32_t  ul_cfg_tput
);

/*===========================================================================
  FUNCTION netmgr_tc_set_iptables_command_options
===========================================================================*/
/*!
@brief
  Checks whether iptables has support for options ex. -W

@return
  None
*/
/*=========================================================================*/
void
netmgr_tc_set_iptables_command_options(char* opts);

/*===========================================================================
  FUNCTION netmgr_tc_find_qos_modify_cache_flow_info
===========================================================================*/
/*!
@brief
 Find flow info for correspoding link-flow_id combination

@return
  Pointer to flow info object
*/
/*=========================================================================*/
void*
netmgr_tc_find_qos_modify_cache_flow_info
(
  int link,
   uint32 flow_id
);

/*===========================================================================
  FUNCTION netmgr_tc_incr_rcvd_qos_modify_counter
===========================================================================*/
/*!
@brief
  Increment the number of QoS Modify requests pending to be processed in cmdq

@return
  NETMGR_SUCCESS - on sucessful increment
  NETMGR_FAILURE - on unsucessful attempt to increment
*/
/*=========================================================================*/
int
netmgr_tc_incr_rcvd_qos_modify_counter
(
 int link,
 uint32 flow_id
);

/*===========================================================================
  FUNCTION netmgr_tc_match_qos_modify_cache_flows
===========================================================================*/
/*!
@brief
  Compare 2 QoS modify cache flows and returns comparison result

@return
  0 - flows match
  non-zero - flows mismatch
*/
/*=========================================================================*/
long int
netmgr_tc_match_qos_modify_cache_flows
(
   const void * first,
   const void * second
);

/*===========================================================================
  FUNCTION netmgr_tc_decr_rcvd_modify_flow_counter
===========================================================================*/
/*!
@brief
  Decrement the number of QoS Modify requests pending to be processed in cmdq

@return
  NETMGR_SUCCESS - on sucessful decrement
  NETMGR_FAILURE - on unsucessful attempt to decrement
*/
/*=========================================================================*/
int
netmgr_tc_decr_rcvd_modify_flow_counter
(
  int    link,
  uint32 flow_id
);

/*===========================================================================
  FUNCTION netmgr_tc_get_rcvd_modify_flow_counter
===========================================================================*/
/*!
@brief
  Get the number of QoS Modify requests pending to be processed in cmdq

@return
  <num> - counter value
*/
/*=========================================================================*/
int
netmgr_tc_get_rcvd_modify_flow_counter
(
  int    link,
  uint32 flow_id
);
#endif /* __NETMGR_TC_H__ */
