/******************************************************************************

                          N E T M G R _ T C _ I . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_tc_i.h
  @brief   Network Manager traffic control internal header file

  DESCRIPTION
  Header file for NetMgr Linux traffic control interface.

******************************************************************************/
/*===========================================================================
 *
 * Copyright (c) 2010, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/23/10   ar         Initial version

******************************************************************************/

#ifndef __NETMGR_TC_I_H__
#define __NETMGR_TC_I_H__

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "netmgr_tc.h"

/*===========================================================================
                        DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Type representing TC handle information
---------------------------------------------------------------------------*/
typedef struct netmgr_tc_handle_info_s {
  int major;
  int minor;
} netmgr_tc_handle_info_t;

/*---------------------------------------------------------------------------
   Type representing collection of a flow's state information
---------------------------------------------------------------------------*/
typedef struct netmgr_tc_flow_info_s {
  netmgr_qmi_qos_flow_info_t  qos_flow; /* Modem flow attributes */
  netmgr_tc_handle_info_t* class_handle; /* class handle */
} netmgr_tc_flow_info_t;

/*---------------------------------------------------------------------------
   Type representing collection of a flow's state information
---------------------------------------------------------------------------*/
typedef struct netmgr_tc_link_info_s {
  netmgr_tc_handle_info_t *root_qdisc;    /* root qdisc handle             */
  netmgr_tc_handle_info_t *root_class;    /* root class handle             */
  netmgr_tc_handle_info_t *default_class; /* default class handle          */
  netmgr_tc_handle_info_t *tcp_ack_class; /* tcp ack class handle          */
  ds_dll_el_t *flow_list;                 /* Flow objects list head        */
  ds_dll_el_t *filter_list;                /* Filter precedence list head */
  int next_class_minor;                   /* Class minor ID sequence       */
} netmgr_tc_link_info_t;

/*---------------------------------------------------------------------------
   Collection of configuration information for the module
---------------------------------------------------------------------------*/
struct netmgr_tc_cfg_s {
  int                   nlink;                     /* number of qmi links  */
  netmgr_ctl_port_config_type *link_array;         /* link enabled array   */
  netmgr_tc_link_info_t links[NETMGR_MAX_LINK];    /* link state info      */
  boolean               is_initialized;            /* Flag init completed  */
  boolean               postrouting_chain_available;  /* chain to only mark qcom qos*/
};

/*-----------------------------------------------------------------------------
 * Keep latest info for a partiluar flow from QMI for ignorning redundant
 * QoS info
 *---------------------------------------------------------------------------*/
typedef struct netmgr_qos_modify_cache_link_info_s {
  ds_dll_el_t *flow_list;                 /* Flow objects list head        */
} netmgr_tc_link_qos_modify_info_t;

struct netmgr_qos_modify_cache_s {
  netmgr_tc_link_qos_modify_info_t links[NETMGR_MAX_LINK];    /* link state info      */
  pthread_mutex_t modify_mutex;
};

extern struct netmgr_qos_modify_cache_s netmgr_qos_modify_cache;
/*===========================================================================
                            FUNCTION DECLARATIONS
===========================================================================*/

#endif /* __NETMGR_TC_I_H__ */
