/******************************************************************************

                          N E T M G R _ S M . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_sm.h
  @brief   Network Manager state machine header file

  DESCRIPTION
  Header file containing definition of NetMgr's state machine.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010,2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/08/10   ar         Initial version

******************************************************************************/

#ifndef __NETMGR_SM_H__
#define __NETMGR_SM_H__

#include "netmgr_defs.h"
#include "netmgr_sm_int.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef enum netmgr_sm_events_e {
  NETMGR_IS_COMPLETE_MSG,        /* Message to initialize vars post init */
  NETMGR_KIF_MSG_CMD,            /* Kernel socket interface message      */
  NETMGR_QMI_MSG_CMD,            /* QMI library message                  */
  NETMGR_RESET_MSG_CMD,          /* netmgr reset message                 */
  NETMGR_USER_MSG_CMD,           /* User command msg */

  NETMGR_INITED_EV,              /* netmgr initialized event             */
  NETMGR_MODEM_IS_EV,            /* Modem in service event               */
  NETMGR_MODEM_OOS_EV,           /* Modem out of service event           */
  NETMGR_WDS_CONNECTED_EV,       /* Modem interface connected            */
  NETMGR_WDS_DISCONNECTED_EV,    /* Modem interface disconnected         */
  NETMGR_KIF_OPENED_EV,          /* Kernel network interface up          */
  NETMGR_KIF_CLOSED_EV,          /* Kernel network interface down        */
  NETMGR_KIF_CONFIGURED_EV,      /* Kernel network interface configured  */
  NETMGR_KIF_RECONFIGURED_EV,    /* Kernel network interface reconfigured*/
  NETMGR_QOS_ACTIVATE_EV,        /* Modem QoS flow activated             */
  NETMGR_QOS_MODIFY_EV,          /* Modem QoS flow modified              */
  NETMGR_QOS_DELETE_EV,          /* Modem QoS flow deleted               */
  NETMGR_QOS_SUSPEND_EV,         /* modem QoS flow suspended             */
  NETMGR_QOS_FLOCNTRL_EV,        /* modem QoS flow control change        */
#ifdef FEATURE_DS_LINUX_ANDROID
  NETMGR_NETD_RESTART_EV,        /* netd restart event                   */
  NETMGR_NETD_LINK_RESTART_EV,   /* Per link netd restart event          */
#endif /* FEATURE_DS_LINUX_ANDROID */
  NETMGR_WLAN_OPENED_EV,         /* WLAN interface is up                 */
  NETMGR_WLAN_CLOSED_EV,         /* WLAN interface is down               */
  NETMGR_OOS_COMPLETE_MSG,       /* Global OOS msg post per link OOS msg */

  NETMGR_INVALID_EV              /* Internal value                       */
} netmgr_sm_events_t;


/*===========================================================================
                            GLOBAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

  STATE MACHINE ERROR HOOK FUNCTION: netmgr_sm_error_hook

===========================================================================*/
void netmgr_sm_error_hook
(
  stm_status_t     error,
  const char      *filename,
  uint32           line,
  struct stm_state_machine_s *sm
);


/*===========================================================================

  STATE MACHINE DEBUG HOOK FUNCTION:  netmgr_sm_debug_hook

===========================================================================*/
void netmgr_sm_debug_hook
(
  stm_debug_event_t debug_event,
  struct stm_state_machine_s *sm,
  stm_state_t state_info,
  void *payload
);



#endif /* __NETMGR_SM_H__ */
