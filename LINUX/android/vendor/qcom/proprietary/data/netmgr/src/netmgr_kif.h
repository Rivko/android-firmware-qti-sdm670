/******************************************************************************

                        N E T M G R _ K I F . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_kif.h
  @brief   Network Manager Kernel Interface Module header file

  DESCRIPTION
  Header file for Kernel Interface module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_KIF_H__
#define __NETMGR_KIF_H__

#include <sys/socket.h>
#ifndef FEATURE_DATA_WITH_UPDATED_HEADERS
 #ifndef NETMGR_OFFTARGET
  #include <netinet/in.h>
 #endif
#endif

#include "ds_util.h"
#include "netmgr.h"
#include "netmgr_main.h"

#ifdef NETMGR_OFFTARGET
#include "netmgr_stubs.h"
#endif

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define NETMGR_KIF_MIN_V4_MTU    68  /* RFC791  Section 3.2 - Fragmentation and Reassembly */
#define NETMGR_KIF_MIN_V6_MTU  1280  /* RFC2460 Section 5 - Packet Size Issues */

/*---------------------------------------------------------------------------
   Constant representing integer value used to skip module load during init
---------------------------------------------------------------------------*/
#define NETMGR_KIF_SKIP 0
#define NETMGR_KIF_LOAD 1

/*---------------------------------------------------------------------------
   Constant representing maximum length of filename
---------------------------------------------------------------------------*/
#define NETMGR_KIF_FILENAME_MAX_LEN 128

/*---------------------------------------------------------------------------
   Constants representing physical transport sleep states
---------------------------------------------------------------------------*/
#define NETMGR_KIF_RESET_SLEEP_STATE   0
#define NETMGR_KIF_SET_SLEEP_STATE     1

/*---------------------------------------------------------------------------
   Constant representing clat interface prefix
---------------------------------------------------------------------------*/
#define NETMGR_KIF_CLAT_INTF_PREFIX "v4-"

/*---------------------------------------------------------------------------
   Constant representing  tethering interface
---------------------------------------------------------------------------*/
#define NETMGR_KIF_TETHERING_INTF_SOFTAP "softap0"
#define NETMGR_KIF_TETHERING_INTF_RNDIS  "rndis0"

#define IF_DOWN 0
#define IF_UP 1
#define GRO_DISABLE 0
#define GRO_ENABLE 1

#ifdef FEATURE_DATA_IWLAN
  #define NETMGR_KIF_IS_REV_RMNET_LINK(link)                                     \
      (((link) >= NETMGR_LINK_REV_RMNET_0 && (link) <= NETMGR_LINK_REV_RMNET_8) || \
       ((link) >= NETMGR_LINK_REV_RMNET_9 && (link) <= NETMGR_LINK_REV_RMNET_17))

  #define NETMGR_KIF_IS_MAX_REV_RMNET_LINK(link)                                 \
       ((link) == NETMGR_LINK_REV_RMNET_8 || (link) == NETMGR_LINK_REV_RMNET_17)

  #define NETMGR_IS_REV_IP_LOCAL_BRK_OUT_LINK(link)                              \
       ((link) == NETMGR_LINK_REV_RMNET_0 || (link) == NETMGR_LINK_REV_RMNET_9)
#else
  #define NETMGR_KIF_IS_REV_RMNET_LINK(link) FALSE

  #define NETMGR_KIF_IS_MAX_REV_RMNET_LINK(link) FALSE

  #define NETMGR_IS_REV_IP_LOCAL_BRK_OUT_LINK(link) FALSE
#endif /* FEATURE_DATA_IWLAN */

#ifdef FEATURE_DATA_IWLAN
  /*---------------------------------------------------------------------------
     Macro to determine if the given link corresponds to reverse Rmnet
  ---------------------------------------------------------------------------*/

  #define NETMGR_KIF_IMS_SIP_PORT      "5060"

  #define NETMGR_KIF_MODEM_PORT_START  "32000"
  #define NETMGR_KIF_MODEM_PORT_END    "36999"

  #define NETMGR_KIF_APPS_PORT_START   "37000" /* MODEM_PORT_END + 1 */
  #define NETMGR_KIF_APPS_PORT_END     "50000" /* DEFAULT end port */

  #define NETMGR_KIF_IMS_AUDIO_PORT_START      "50010"
  #define NETMGR_KIF_IMS_AUDIO_PORT_END        "50060"

  #define NETMGR_KIF_FWMARK            "9"
  #define NETMGR_KIF_DEFAULT_FLOW      "0"
#else
  /*---------------------------------------------------------------------------
    Macro to determine if the given link corresponds to reverse Rmnet
  ---------------------------------------------------------------------------*/
  #ifndef NETMGR_KIF_IS_REV_RMNET_LINK
  #define NETMGR_KIF_IS_REV_RMNET_LINK(link)  (FALSE)
  #endif
#endif /*FEATURE_DATA_IWLAN */

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

#ifndef max
#define max(a,b)  ((a) < (b) ? (b):(a))
#endif
/*---------------------------------------------------------------------------
   Type representing callback function registered by upper layer to receive
   notification of interface up event
---------------------------------------------------------------------------*/
typedef void (* netmgr_kif_opened_f)
(
    uint8  link,
    uint8  status,
    void * clnt_hdl
);

/*---------------------------------------------------------------------------
   Type representing callback function registered by upper layer to receive
   notification of interface down event
---------------------------------------------------------------------------*/
typedef void (* netmgr_kif_closed_f)
(
    uint8  link,
    uint8  status,
    void * clnt_hdl
);

/*---------------------------------------------------------------------------
   Type representing callback function registered by upper layer to receive
   notification of interface reconfigured event
---------------------------------------------------------------------------*/
typedef void (* netmgr_kif_reconfigured_f)
(
    uint8  link,
    uint8  status,
    void * clnt_hdl
);

/*---------------------------------------------------------------------------
   Type representing collection of callback functions registered by upper
   layer to receive event notifications/confirmations
---------------------------------------------------------------------------*/
typedef struct {
    netmgr_kif_opened_f        opened_cb;
    netmgr_kif_closed_f        closed_cb;
    netmgr_kif_reconfigured_f  reconfigured_cb;
} netmgr_kif_clntcb_t;

/*---------------------------------------------------------------------------
   Type representing collection of worker thread attributes
---------------------------------------------------------------------------*/
typedef enum
{
  NETMGR_KIF_WTHREAD_DHCP,
  NETMGR_KIF_WTHREAD_MAX
} netmgr_kif_wthread_name_t;

typedef enum
{
  NETMGR_KIF_WTHREAD_STATE_IDLE,
  NETMGR_KIF_WTHREAD_STATE_RUNNING,
  NETMGR_KIF_WTHREAD_STATE_ABORTED,
  NETMGR_KIF_WTHREAD_STATE_MAX
} netmgr_kif_wthread_state_t;

typedef struct netmgr_kif_thread_s {
  netmgr_kif_wthread_state_t state;      /* Thread state */
  pthread_t                  thrd_id;    /* Thread ID */
  pthread_cond_t             cond;       /* Sychronization condition variable */
  boolean                    cond_flag;  /* Condition variable predicate */
  pthread_mutex_t            mutx;       /* Sychronization mutex  */
} netmgr_kif_thread_t;

typedef struct netmgr_kif_wtlist_s {
  netmgr_kif_thread_t   workers[NETMGR_KIF_WTHREAD_MAX];  /* Thread list */
  pthread_mutex_t       mutx;               /* Mutex for list operations */
} netmgr_kif_wtlist_t;

#define NETMGR_KIF_THRDID_NULL  (0)
#define NETMGR_KIF_WTHREAD_WAIT (30)  /* Timeout [secs] for cond_var wait */

#define NETMGR_KIF_WTHREAD_LIST_LOCK( link, ret )                        \
    if (pthread_mutex_lock(&netmgr_kif_info[link].wtlist.mutx) != 0) {   \
      netmgr_log_sys_err("pthread_mutex_lock failed:");                  \
      return (ret);                                                      \
    }

#define NETMGR_KIF_WTHREAD_LIST_UNLOCK( link, ret )                      \
    if (pthread_mutex_unlock(&netmgr_kif_info[link].wtlist.mutx) != 0) { \
      netmgr_log_sys_err("pthread_mutex_unlock failed:");                \
      return (ret);                                                      \
    }


/*---------------------------------------------------------------------------
   Type representing enumeration of kif (kernel interface) states
---------------------------------------------------------------------------*/
typedef enum netmgr_kif_state_e {
    NETMGR_KIF_UNKNOWN         = -1,/* Uknown interface state */
    NETMGR_KIF_CLOSED          = 0, /* Interface 'down' state */
    NETMGR_KIF_OPENING         = 1, /* Interface coming up */
    NETMGR_KIF_CLOSING         = 2, /* Interface deconfigured and going down */
    NETMGR_KIF_OPEN            = 3, /* Interface up and configured */
    NETMGR_KIF_RECONFIGURING   = 4  /* Interface up and being reconfigured */
} netmgr_kif_state_t;


typedef enum netmgr_kif_link_pwrup_state_e {
    NETMGR_KIF_LINK_POWERUP_STATE_UNKNOWN = -1,
    NETMGR_KIF_LINK_POWERUP_STATE_DOWN = 0,
    NETMGR_KIF_LINK_POWERUP_STATE_UP   = 1
}netmgr_kif_link_pwrup_state_t;

/*---------------------------------------------------------------------------
   Type representing collection of state and control information of a kif
---------------------------------------------------------------------------*/
typedef struct netmgr_kif_info_s {
  char                            name[NETMGR_IF_NAME_MAX_LEN];    /* dev name */
  int                             ifi_index;         /* system assigned unique
                                                              device index */
  netmgr_kif_state_t              state;                    /* interface state */
  const netmgr_kif_clntcb_t     * clntcb;     /* client's registered callbacks */
  void                          * clnt_hdl;             /* Client's handle ptr */
  netmgr_kif_link_pwrup_state_t   pwrup_status;              /* powerup status */
  int                             dns_v6_queried; /*flag to check if the DNS
                                                    address's are already queried */
  netmgr_ip_addr_t reconfig_ip_type; /*Keep track of the IP family for which Newaddr
                                       is expected in Reconfig state*/
} netmgr_kif_info_t;


/*===========================================================================
                            GLOBAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_ds_exec
===========================================================================*/
/*!
@brief
  Wrapper around ds_exec(). Injects netmgr-wide logging options

*/
/*=========================================================================*/
int netmgr_ds_exec
(
  const char *command,
  const struct ds_exec_opts *opts
);

/*===========================================================================
  FUNCTION  netmgr_kif_create_link_network
===========================================================================*/
/*!
@brief
  Creates a new network type to handle specific use-cases ex. routing over
  link-local interface

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_kif_create_link_network(int link);

/*===========================================================================
  FUNCTION  netmgr_kif_remove_link_network
===========================================================================*/
/*!
@brief
  Removes network type for specified link

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_kif_remove_link_network(int link);

/*===========================================================================
  FUNCTION  netmgr_kif_get_name
===========================================================================*/
/*!
@brief
  Accessor for getting device name for a given link.

@return
  char* - Pointer to device name

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *
netmgr_kif_get_name (int link);

/*===========================================================================
  FUNCTION  netmgr_kif_get_state
===========================================================================*/
/*!
@brief
  Accessor for getting kif state for a given link.

@return
  netmgr_kif_state_t - State of kif on given link

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

netmgr_kif_state_t
netmgr_kif_get_state (int link);

/*===========================================================================
  FUNCTION  netmgr_kif_get_link_powerup_state
===========================================================================*/
/*!
@brief
  Accessor for getting netmgr power-up state for a given link.

@return
  netmgr_kif_link_pwrup_state_t

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
netmgr_kif_link_pwrup_state_t
netmgr_kif_get_link_powerup_state(int link);

/*===========================================================================
  FUNCTION  netmgr_kif_set_link_powerup_state
===========================================================================*/
/*!
@brief
  Assign netmgr power-up state for a given link.

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_kif_set_link_powerup_state
(
   int link,
   netmgr_kif_link_pwrup_state_t pwrup_state
);

/*===========================================================================
  FUNCTION  netmgr_kif_powerup_init
 ===========================================================================*/
/*!
@brief
  Powerup initialization routine for the KIF module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_kif_powerup_init
(
  char * iname
);

/*===========================================================================
  FUNCTION  netmgr_kif_init
===========================================================================*/
/*!
@brief
  Initialization routine for the KIF module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_kif_init
(
  int nint,
  netmgr_ctl_port_config_type links[],
  int skip,
  char * dirpath,
  char * modscript
);

/*===========================================================================
  FUNCTION  netmgr_kif_set_mtu
===========================================================================*/
/*!
@brief
  Helper function to change MTU on a given link.

@return
  int - NETMGR_SUCCESS if MTU is successfully changed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_set_mtu(int link, boolean post_nl_ev);

/*===========================================================================
  FUNCTION  netmgr_kif_send_icmpv6_router_solicitation
===========================================================================*/
/*!
@brief
  Sends a ICMPV6 router solicitation message

@return
  NETMGR_SUCCESS
  NETMGR_FAILURE

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_send_icmpv6_router_solicitation (int link);

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_flow_control
 ===========================================================================*/
/*!
@brief
  Helper function to enable/disable flow on a given handle.

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_ifioctl_flow_control(const char * dev, int handle, int enable);

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_change_sleep_state
 ===========================================================================*/
/*!
@brief
  Helper function to set the sleep state for the given kernel interface.

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_ifioctl_change_sleep_state(const char * dev, int enable);

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_install_sa_and_routing_rules
===========================================================================*/
/*!
@brief
  Installs forwarding rules and security associations (SA) (if any)
  for given reverse rmnet link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_install_sa_and_routing_rules
(
  int  link,
  int  ip_family
);

/*===========================================================================
  FUNCTION  netmgr_kif_remove_sa_and_routing_rules
===========================================================================*/
/*!
@brief
  Removes the forwarding rules and security associations (SA) (if any)
  for given reverse rmnet link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_remove_sa_and_routing_rules
(
  int                   link,
  int                   ip_family,
  netmgr_address_set_t  *addr_info_ptr
);

#endif /* FEATURE_DATA_IWLAN */

int
netmgr_kif_configure_ep_params
(
  const char * dev_name
);

int
netmgr_kif_init_physical_transport
(
  const char *dev_name,
  netmgr_data_format_t *data_format
);

/*===========================================================================
  FUNCTION  netmgr_kif_set_data_format
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs on physical data ports

@return
  int

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_set_data_format
(
  const char *phys_dev,
  uint32_t ingress_flags,
  uint32_t egress_flags
);

/*===========================================================================
  FUNCTION  netmgr_kif_get_qos_header_format
===========================================================================*/
/*!
@brief
  Gets the value of the WDA data format property qos_header_format to be
  sent to the modem. qos_header_format is applicable only for when
  baseband is not mdm2 and netmgr_kif_ifioctl_set_qosmode() is implemented.

@return
  NETMGR_SUCCESS - if successful
  NETMGR_FAILURE - if any errors occurred while determining the
                   qos_header_format

*/
/*=========================================================================*/
int
netmgr_kif_get_qos_header_format
(
  const char             *phys_dev,
  uint32_t               vnd,
  netmgr_data_format_t   *data_format
);

/*===========================================================================
  FUNCTION  netmgr_kif_set_qos_header_format
===========================================================================*/
/*!
@brief
  Sets the qos_header_format for every virtual network device.

@return
  NETMGR_SUCCESS - if successful
  NETMGR_FAILURE - if any errors occurred while determining the
                   qos_header_format

*/
/*=========================================================================*/
int
netmgr_kif_set_qos_header_format
(
  uint32_t               vnd_id,
  netmgr_data_format_t   *data_format
);

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_sg_support
 ===========================================================================*/
/*!
@brief
  Function to the rmnet driver to query the Scatter gather support from IPA driver

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
          NETMGR_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_ifioctl_get_sg_support(const char *dev);

/*===========================================================================
  FUNCTION  netmgr_kif_verify_link
===========================================================================*/
/*!
@brief
  Helper function to verify validity of a link ID.

@return
  int - NETMGR_SUCCESS if link ID is valid, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_verify_link (int link);

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_ul_checksum_offload_feature
===========================================================================*/
/*!
@brief
  Helper function to set UL checksum offload feature flag on a device.

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_ifioctl_set_ul_checksum_offload_feature( const char * dev );

#if defined(FEATURE_DATA_LINUX_LE) && defined(FEATURE_DATA_LINUX_EMBMS_ODU)
/*===========================================================================
  FUNCTION  netmgr_kif_ipa_ioctl_set_dev_name
 ===========================================================================*/
/*!
@brief
  Helper function to set driver link-layer protocol mode for the
  given kernel interface.  Driver mode values are from msm_rmnet.h.


@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_ipa_ioctl_set_dev_name(const char * dev);
#endif

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_xlat_dev_info
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs to set rmnet data dev name

@arg phys_dev device to call IOCTL on
@arg rmnet_data_dev rmnet data device name

@return
  int - value of netmgr_kif_extended_ioctl()

*/
/*=========================================================================*/
int
netmgr_kif_ifioctl_set_xlat_dev_info
(
  const char *phys_dev,
  const char *rmnet_data_dev
);

/*===========================================================================
  FUNCTION  netmgr_kif_enable_ul_gso_feature
===========================================================================*/
/*!
@brief
  Helper function to enable UL GSO feature on device.

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_enable_ul_gso_feature (const char * dev);

/*===========================================================================
  FUNCTION netmgr_kif_get_ul_sg_feature
===========================================================================*/
/*!
@brief
  Helper function to get UL SG feature on a device.

@return
  int - returns the status of SG 0 or 1. -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_get_ul_sg_feature( const char * dev );

/*===========================================================================
  FUNCTION  netmgr_kif_enable_ul_ssg_feature
===========================================================================*/
/*!
@brief
  Helper function to enable UL SSG feature on device.

@return
  int - NETMGR_SUCCESS if IOCTL is successfully executed,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_enable_ul_ssg_feature (const char * dev);


/*===========================================================================
  FUNCTION  netmgr_kif_get_mtu
===========================================================================*/
/*!
@brief
  Helper function to retrieve MTU on a given link.

@return
  int - link MTU if successfully queried,
        -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_kif_get_mtu(int link);

#ifdef FEATURE_DS_LINUX_ANDROID
/*===========================================================================
  FUNCTION  netmgr_kif_handle_netd_event()
 ===========================================================================*/
/*!
@brief
  netd crashes are handled through registration events from
  Netd HAL via HIDL.
@return
  None.
*/
/*=========================================================================*/
void
netmgr_kif_handle_netd_event(void);
#endif /* FEATURE_DS_LINUX_ANDROID */

/*===========================================================================
  FUNCTION  netmgr_unix_listener_init
===========================================================================*/
/*!
@brief
  Initialization routine for UNIX domain socket interface message listener.
  listener.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_unix_listener_init(void);

/*===========================================================================
  FUNCTION netmgr_kif_set_iptables_command_options
===========================================================================*/
/*!
@brief
  Checks whether iptables has support for options ex. -W

@return
  None
*/
/*=========================================================================*/
void
netmgr_kif_set_iptables_command_options(char* opts);

#endif /* __NETMGR_KIF_H__ */
