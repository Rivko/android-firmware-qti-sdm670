/******************************************************************************

                        N E T M G R _ K I F . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_kif.c
  @brief   Network manager Kernel Interface Module

  DESCRIPTION
  Implementation of Kernel Interface module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <stdint.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/udp.h>
#include <linux/ethtool.h>
#include <errno.h>
#include <stdint.h>
#include <linux/msm_rmnet.h>

#ifdef FEATURE_DATA_LINUX_EMBMS_ODU
#include <linux/msm_ipa.h>
#endif

#include <assert.h>
#include "stringl.h"

#ifdef NETMGR_OFFTARGET
  #include <netinet/in.h>
  #include <sys/system_properties.h>
  #include <netinet/icmp6.h>
  #include "cutils/properties.h"
  #include "netmgr_stubs.h"
#else
  #ifdef FEATURE_DATA_LINUX_LE
    #include <netinet/icmp6.h>
  #else
    #include <linux/in6.h>
    #include <netinet/icmp6.h>
    #include <netinet/in.h>
    #include <sys/system_properties.h>
    #include "cutils/properties.h"
    #ifdef FEATURE_GB_NET_UTILS
      #include "ifc_utils.h"
    #else
     #include "ifc.h"
    #endif
  #endif
#endif /* NETMGR_OFFTARGET */

#include "ds_cmdq.h"
#include "ds_string.h"
#include "ds_trace.h"
#include "netmgr_defs.h"
#include "netmgr_util.h"
#include "netmgr_platform.h"
#include "netmgr_netlink.h"
#include "netmgr_exec.h"
#include "netmgr_kif.h"
#include "netmgr_main.h"
#include "netmgr_qmi_wda.h"
#include "netmgr_rmnet.h"
#include "netmgr_tc.h"
#include "netmgr_qmi_dfs.h"
#include "netmgr_unix.h"
#include "netmgr_cmdq.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include "netmgr_cb.h"
#include "netmgr_iwlan_client.h"

#include "netmgr_cb.h"
#include "netmgr_main_cb.h"
#include "netmgr_kif_cb.h"

#if defined (NETMGR_OFFTARGET) || defined (FEATURE_DATA_PREFIX_DELEGATION)
#include "netmgr_stubs.h"
#endif

#define IP_PATH "ip"
#define NETMGR_KIF_IPA_DEV_NAME "/dev/ipa"

#define NETMGR_KIF_IPV6_PREFIX_LEN  (8)
#define NETMGR_KIF_IPV6_IID_LEN     (8)
#define NETMGR_KIF_IPV6_MULTICAST_ROUTER_ADDR  "FF02::2"
#define NETMGR_KIF_MAX_COMMAND_LENGTH   200
#define NETMGR_KIF_ARR_SIZE(x)  (sizeof(x)/sizeof(x[0]))

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Constant representing maximum length of interface/device name's prefix,
   i.e. without the numeric instance # (e.g. 'eth' in 'eth0')
---------------------------------------------------------------------------*/
#define NETMGR_KIF_NAME_PR_MAX_LEN (IFNAMSIZ - 3)

/*---------------------------------------------------------------------------
   Constant string representing default interface name
---------------------------------------------------------------------------*/
#define NETMGR_KIF_DEF_NAME "eth"

/*---------------------------------------------------------------------------
   Constant representing global IP address scope
---------------------------------------------------------------------------*/
#define NETMGR_KIF_GLOBAL_SCOPE   RT_SCOPE_UNIVERSE
#define NETMGR_KIF_LOCAL_SCOPE    RT_SCOPE_LINK

/*---------------------------------------------------------------------------
   Constants for Modem port open retry mechanism
---------------------------------------------------------------------------*/
#define NETMGR_KIF_MAX_RETRY_COUNT               120
#define NETMGR_KIF_WAIT_TIME_BEFORE_NEXT_RETRY   500000 /* usec */
#define NETMGR_KIF_USEC_TO_SEC(a)                ((a)/1000000)

/*---------------------------------------------------------------------------
   Constant string representing source address of unix domain socket
---------------------------------------------------------------------------*/
#define NETMGR_KIF_UDS_CLIENT_PATH "/tmp/uds_clnt.1234"

/*---------------------------------------------------------------------------
  Constant strings for kernel driver sysfs files to report transport port
  open status.  This is to supprot power subsystem test automation.
---------------------------------------------------------------------------*/
#define NETMGR_KIF_SYSFILE_OPEN_TIMEOUT "/data/data_test/modem_port_timeout"
#define NETMGR_KIF_SYSFILE_OPEN_STATUS  "/data/data_test/modem_port_status"
#define NETMGR_KIF_OPEN_SUCCESS (1)
#define NETMGR_KIF_OPEN_FAILURE (0)
#define NETMGR_KIF_PORT_OPENED NETMGR_KIF_OPEN_SUCCESS
#define NETMGR_KIF_PORT_CLOSED NETMGR_KIF_OPEN_FAILURE

/*---------------------------------------------------------------------------
   Constant string representing template for network interface flush
---------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_LINUX_LE) || defined(NETMGR_OFFTARGET)
#define NETMGR_KIF_SYSCMD_FLUSHADDR "ip addr flush dev %s"

#define NETMGR_KIF_SYSCMD_V4_DELADDR "ip addr del %s dev %s"

#define NETMGR_KIF_SYSCMD_V6_DELADDR "ip -6 addr del %s/%d dev %s"

#define NETMGR_KIF_SYSCMD_V6_FLUSHADDR "/system/bin/ip -6 addr flush dev %s"

#define NETMGR_KIF_SYSCMD_V6_FLUSH_GLOBAL_ADDR "/system/bin/ip -6 addr flush dev %s scope global"
#else
#define NETMGR_KIF_SYSCMD_FLUSHADDR "/system/bin/ip addr flush dev %s"

#define NETMGR_KIF_SYSCMD_V6_FLUSHADDR "/system/bin/ip-wrapper-1.0 -6 addr flush dev %s"

#define NETMGR_KIF_SYSCMD_V6_FLUSH_GLOBAL_ADDR "/system/bin/ip-wrapper-1.0 -6 addr flush dev %s scope global"
#endif
#define NETMGR_KIF_SYSCMD_SIZ  (256)






#ifdef FEATURE_DS_LINUX_ANDROID
/*---------------------------------------------------------------------------
   Macros for Android DNS property assignment.
---------------------------------------------------------------------------*/
#define ADDR_FMT_V4( val )                  \
        (unsigned char)(val >>  0),         \
        (unsigned char)(val >>  8),         \
        (unsigned char)(val >> 16),         \
        (unsigned char)(val >> 24)

#define ADDR_FMT_V6( val )                  \
        (uint16)(ntohs((uint16)(val[0] >>  0))),      \
        (uint16)(ntohs((uint16)(val[0] >> 16))),      \
        (uint16)(ntohs((uint16)(val[0] >> 32))),      \
        (uint16)(ntohs((uint16)(val[0] >> 48))),      \
        (uint16)(ntohs((uint16)(val[1] >>  0))),      \
        (uint16)(ntohs((uint16)(val[1] >> 16))),      \
        (uint16)(ntohs((uint16)(val[1] >> 32))),      \
        (uint16)(ntohs((uint16)(val[1] >> 48)))

static char name_buf[PROPERTY_KEY_MAX];
static char addr_buf[PROPERTY_VALUE_MAX];

/*---------------------------------------------------------------------------
   Macros for Android network interface MTU override
---------------------------------------------------------------------------*/
#define NETMGR_KIF_PROPERTY_MTU          "persist.data_netmgrd_mtu"
#define NETMGR_KIF_PROPERTY_MTU_SIZE     (4)
#define NETMGR_KIF_PROPERTY_MTU_DEFAULT  NETMGR_MTU_INVALID

#define NETMGR_KIF_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY  (3)

#endif /* FEATURE_DS_LINUX_ANDROID */

/*---------------------------------------------------------------------------
   Miscellaneous definitions
---------------------------------------------------------------------------*/
static char iptables_opts[IPTABLES_OPTS_MAX_LENGTH];

boolean clat_iface_st = IF_DOWN;
boolean softap_iface_st = IF_DOWN;
boolean rndis_iface_st = IF_DOWN;
int clat_iface_indx = -1;


/*===========================================================================
  FUNCTION netmgr_kif_set_ssdp_rule_on_iface
===========================================================================*/
/*!
@brief
  Block SSDP broadcast packets from being transmitted on an interface

@return
  None

@param *dev_name ASCII string representation of network device name

@note
  Underlying ds_system_call() will return 0 even on failure of system command.
  As such, this will return void because there is no way to determine a failure
  condition.
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_set_ssdp_rule_on_iface
(
  const char *dev_name
);

/*===========================================================================
  FUNCTION netmgr_kif_remove_ssdp_rule_on_iface
===========================================================================*/
/*!
@brief
  Remove the SSDP drop rule from the ip table.

@return
  None

@param *dev_name ASCII string representation of network device name

@note
  Underlying ds_system_call() will return 0 even on failure of system command.
  As such, this will return void because there is no way to determine a failure
  condition.
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_remove_ssdp_rule_on_iface
(
  unsigned int link
);

/*===========================================================================
FUNCTION netmgr_kif_set_link_ipv6_config
===========================================================================*/
/*!
@brief
  Wrapper function to configure IPv6 settings on an interface
@note
  - Dependencies
  - None
  - Side Effects
  - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_set_link_ipv6_config
(
  const char *link,
  const char *property,
  const char *value
);

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_dl_checksum_offload_feature
===========================================================================*/
/*!
@brief
  Helper function to set DL checksum offload feature flag on a device.
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_ifioctl_set_dl_checksum_offload_feature
(
  const char * dev
);

LOCAL void
netmgr_kif_update_rmnet_proc_entries
(
  int link
);

LOCAL int
netmgr_kif_configure_dl_gro_feature
(
  const char *dev,
  int enable
);
/*---------------------------------------------------------------------------
   Macros for Android iWLAN
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IWLAN

#define NETMGR_KIF_PROPERTY_REKEY        "persist.data.iwlan.rekey"
#define NETMGR_KIF_PROPERTY_REKEY_SIZE   (10)
#ifndef UINT32_MAX
  #define UINT32_MAX                     (4294967295U)
#endif

#define NETMGR_KIF_GET_XFRM_EXTENED_POLICIES_INSTALLED()                  \
  netmgr_kif_cfg.iwlan.is_v6_extened_xfrm_policies_installed

#define NETMGR_KIF_SET_XFRM_EXTENED_POLICIES_INSTALLED(status)            \
  *(&netmgr_kif_cfg.iwlan.is_v6_extened_xfrm_policies_installed) = status

typedef struct
{
  boolean is_v6_extened_xfrm_policies_installed;
} netmgr_kif_iwlan_cfg_t;

#define TRAFFIC_SEL_WILDCARD_V4  "0.0.0.0/0"
#define TRAFFIC_SEL_WILDCARD_V6  "::/0"

/* Create tables for iwlan iptable rules */
typedef struct iwlan_iptables_cmd_s
{
  char* rule;
} iwlan_iptables_cmd_t;

/* Install protocol specific XFRM policies */
#define NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY_EX(ipsec_dir)            \
do                                                                         \
{                                                                          \
  char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";                    \
  char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                            \
  int cmd_length = 0;                                                      \
                                                                           \
  if (TRUE == NETMGR_KIF_GET_XFRM_EXTENED_POLICIES_INSTALLED())            \
  {                                                                        \
    /* If policies are already installed skip */                           \
    netmgr_log_low("%s(): Extended XFRM policies already installed",       \
                   __func__);                                              \
    break;                                                                 \
  }                                                                        \
                                                                           \
  /* Installs policies to skip encryption of ICMPv6 packets */             \
  /* This rule will be installed at a higher priority than  */             \
  /* the legacy policies */                                                \
  /* 'priority'  - any priority higher than the legacy policies */         \
                                                                           \
  /* If the packet is destined for a link-local address then we       */   \
  /* will not tunnel the packet. ICMPv6 packets such as NS and NA     */   \
  /* have a link-local destination address and should not be tunneled */   \
  /* should not be tunneled */                                             \
  DS_SYSCALL_DEFAULTS(opts);                                               \
  opts.stderr = err_str;                                                   \
  opts.stderr_length = sizeof(err_str);                                    \
  opts.log_options |= DS_EXEC_LOG_EXECTIME;                                \
                                                                           \
  cmd_length = snprintf( xfrm_policy_cmd,                                  \
                         sizeof(xfrm_policy_cmd),                          \
                         "ip xfrm policy add dir "#ipsec_dir" priority 10" \
                           " dst fe80::/64 proto 58");                     \
                                                                           \
  if (DS_EXEC_OK != netmgr_ds_exec(xfrm_policy_cmd, &opts))                \
  {                                                                        \
    netmgr_log_err("Failed to install v6 special policies, err: %s",       \
                   err_str);                                               \
    break;                                                                 \
  }                                                                        \
                                                                           \
  /* Set flag to indicate that the rules were installed */                 \
  NETMGR_KIF_SET_XFRM_EXTENED_POLICIES_INSTALLED(TRUE);                    \
} while (0)

/* Remove protocol specific XFRM policies */
#define NETMGR_KIF_UTIL_REMOVE_IPSEC_XFRM_POLICY_EX(ipsec_dir)       \
do                                                                   \
{                                                                    \
  char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";              \
  char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                      \
  int cmd_length = 0;                                                \
                                                                     \
  if (FALSE == NETMGR_KIF_GET_XFRM_EXTENED_POLICIES_INSTALLED())     \
  {                                                                  \
    /* If policies are already removed skip */                       \
    netmgr_log_low("%s(): Extended XFRM policies already removed",   \
                   __func__);                                        \
    break;                                                           \
  }                                                                  \
                                                                     \
  /* Removes the policies which were added to skip encryption */     \
  /* of ICMPv6 packets */                                            \
  DS_SYSCALL_DEFAULTS(opts);                                         \
  opts.stderr = err_str;                                             \
  opts.stderr_length = sizeof(err_str);                              \
  opts.log_options |= DS_EXEC_LOG_EXECTIME;                          \
                                                                     \
  cmd_length = snprintf( xfrm_policy_cmd,                            \
                         sizeof(xfrm_policy_cmd),                    \
                         "ip xfrm policy delete dir "#ipsec_dir""    \
                          " dst fe80::/64 proto 58");                \
                                                                     \
  if (DS_EXEC_OK != netmgr_ds_exec(xfrm_policy_cmd, &opts))          \
  {                                                                  \
    netmgr_log_err("Failed to install v6 special policies, err: %s", \
                   err_str);                                         \
    break;                                                           \
  }                                                                  \
                                                                     \
  /* Set flag to indicate that the rules were removed */             \
  NETMGR_KIF_SET_XFRM_EXTENED_POLICIES_INSTALLED(FALSE);             \
} while (0)

#endif /* FEATURE_DATA_IWLAN */

/*---------------------------------------------------------------------------
   Type representing collection of configuration info for KIF module
---------------------------------------------------------------------------*/

typedef struct {
  int     nint;                                    /* number of interfaces */
  netmgr_ctl_port_config_type * link_array;          /* link enabled array */
  char    name[NETMGR_IF_NAME_MAX_LEN];                     /* device name */
  int     skip; /* Boolean, which if set, indicates that the device driver
                      module should not be loaded. Used only for debugging */
  char    dirpath[NETMGR_KIF_FILENAME_MAX_LEN];  /* Name of dir containing
                                                        module load script */
  char    modscript[NETMGR_KIF_FILENAME_MAX_LEN];    /* module load script */

#ifdef FEATURE_DATA_IWLAN
  netmgr_kif_iwlan_cfg_t  iwlan;                  /* iWLAN configuration */
#endif /* FEATURE_DATA_IWLAN */
} netmgr_kif_cfg_t;

/*---------------------------------------------------------------------------
   Collection of shared configuration info for KIF module
---------------------------------------------------------------------------*/
LOCAL netmgr_kif_cfg_t netmgr_kif_cfg;

/*---------------------------------------------------------------------------
   Array of state/control info for each kif
---------------------------------------------------------------------------*/
LOCAL netmgr_kif_info_t netmgr_kif_info[NETMGR_MAX_LINK];


/*---------------------------------------------------------------------------
   Collection of control info pertaining to open sockets
---------------------------------------------------------------------------*/
typedef struct {
  netmgr_socklthrd_info_t  sk_thrd_info;    /* Listener thread info        */
  netmgr_socklthrd_fdmap_t sk_thrd_fdmap[1];/* Array of fdmap structs used
                                             ** by listener thread.        */
  netmgr_nl_sk_info_t      rt_sk;           /* Netlink routing socket info */
  netmgr_nl_sk_info_t      ev_sk;           /* Netlink event socket info   */
  netmgr_nl_sk_info_t      xfrm_sk;         /* Netlink xfrm socket info    */
} netmgr_kif_sk_info;

netmgr_kif_sk_info netmgr_kif_sk_route;
netmgr_kif_sk_info netmgr_kif_sk_grp;
netmgr_kif_sk_info netmgr_kif_sk_usersock;
netmgr_kif_sk_info netmgr_kif_sk_xfrm;

#ifdef FEATURE_DATA_IWLAN
  #define NETMGR_KIF_FWD_DEV_PER_MODEM  (11)
  #define NETMGR_KIF_REV_DEV_PER_MODEM  (9)
  #define NETMGR_KIF_DEV_PER_MODEM      (NETMGR_KIF_FWD_DEV_PER_MODEM + \
                                         NETMGR_KIF_REV_DEV_PER_MODEM)
#else
  #define NETMGR_KIF_DEV_PER_MODEM      (11)
#endif /* FEATURE_DATA_IWLAN */

#define TO_XSTR(x) TO_STR(x)
#define TO_STR(x)  #x
#define NETMGR_KIF_NATT_MODEM_PORT   32012
#define NETMGR_KIF_NATT_SERVER_PORT  4500

/*---------------------------------------------------------------------------
   Macros needed for QoS header size
---------------------------------------------------------------------------*/

#define NETMGR_KIF_QOS_HEADER_6_BYTE         (1<<0)
#define NETMGR_KIF_QOS_HEADER_8_BYTE         (1<<1)
#define NETMGR_KIF_QOS_HEADER_DEFAULT        (0)

/*---------------------------------------------------------------------------
   Macros needed for ipv6 duplicate address detection
   Time is in milli seconds.
---------------------------------------------------------------------------*/

#define NETMGR_KIF_IPV6_DAD_RETRANSMIT_TIME  10


/*---------------------------------------------------------------------------
   Macros needed for matching IP family in Newaddr event
---------------------------------------------------------------------------*/

#define NETMGR_KIF_IS_IP_MATCH(link,ifaddr)                                \
  ( ((netmgr_kif_info[link].reconfig_ip_type == NETMGR_IPV4_ADDR           \
            && ifaddr->ifa_family == AF_INET) ||                           \
      (netmgr_kif_info[link].reconfig_ip_type == NETMGR_IPV6_ADDR          \
            && ifaddr->ifa_family == AF_INET6)) ? TRUE:FALSE)


/*---------------------------------------------------------------------------
   Accessor for getting kif state for a given link
---------------------------------------------------------------------------*/
netmgr_kif_state_t
netmgr_kif_get_state (int link)
{
  if (function_debug) {
    netmgr_log_med("netmgr_kif_get_state: link=%d, cur_state=%d\n",
                   link,
                   netmgr_kif_info[link].state);
  }
  return netmgr_kif_info[link].state;
}

/*---------------------------------------------------------------------------
  Inline mutator for setting kif state for a given link
  ---------------------------------------------------------------------------*/
LOCAL __inline__ void
netmgr_kif_set_state (int link, netmgr_kif_state_t state)
{
  if (function_debug) {
    netmgr_log_med("netmgr_kif_set_state: link=%d, cur_state=%d, new_state=%d\n",
                   link,
                   netmgr_kif_info[link].state,
                   state);
  }

  netmgr_kif_info[link].state = state;
}

/*---------------------------------------------------------------------------
   Inline accessor for getting pointer to client callback structure for a
   given link
---------------------------------------------------------------------------*/
LOCAL __inline__ const netmgr_kif_clntcb_t *
netmgr_kif_get_clntcb (int link)
{
  return netmgr_kif_info[link].clntcb;
}

/*---------------------------------------------------------------------------
  Inline mutator for setting pointer to client callback structure for a
  given link
---------------------------------------------------------------------------*/
LOCAL __inline__ void
netmgr_kif_set_clntcb (int link, const netmgr_kif_clntcb_t * clntcb)
{
  netmgr_kif_info[link].clntcb = clntcb;
}

/*---------------------------------------------------------------------------
  Inline accessor for getting client handle ptr for a given link
---------------------------------------------------------------------------*/
LOCAL __inline__ void *
netmgr_kif_get_clnt_hdl (int link)
{
  return netmgr_kif_info[link].clnt_hdl;
}

/*---------------------------------------------------------------------------
  Inline mutator for setting client handle ptr for a given link
---------------------------------------------------------------------------*/
LOCAL __inline__ void
netmgr_kif_set_clnt_hdl (int link, void * clnt_hdl)
{
  netmgr_kif_info[link].clnt_hdl = clnt_hdl;
}

/*===========================================================================
  FUNCTION  netmgr_kif_fill_usercmd_info
===========================================================================*/
/*!
@brief
  Helper function to fill in usercmd_info structure for CB chains
*/
/*=========================================================================*/
static int
netmgr_kif_fill_usercmd_info
(
  struct netmgr_kif_cb_usercmd_info *usercmd_info,
  int                               ip_family,
  boolean                           preference
)
{
  int result = NETMGR_SUCCESS;

  if (!usercmd_info)
  {
    netmgr_log_err("%s(): null usercmd_info", __func__);
    return NETMGR_FAILURE;
  }

  usercmd_info->ip_family  = ip_family;
  usercmd_info->preference = preference;
  usercmd_info->result     = NETMGR_CB_RESULT_FAIL;

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_fill_ifinfo
===========================================================================*/
/*!
@brief
  Helper function to fill in ifinfo structure for CB chains given a link#
*/
/*=========================================================================*/
static int
netmgr_kif_fill_ifinfo
(
  int link,
  struct netmgr_kif_cb_ifinfo *ifinfo
)
{
  int result = NETMGR_SUCCESS;
  const char *ifname;
  wds_technology_name_enum_v01 tech_name = netmgr_qmi_wds_get_tech_name(link);
  ipsec_tunnel_info_t tunnel_info;

  if(!ifinfo)
  {
    netmgr_log_err("%s(): null ifinfo", __func__);
    return NETMGR_FAILURE;
  }

  ifname = netmgr_kif_get_name(link);
  if (ifname == NULL)
  {
     netmgr_log_err("%s(): invalid link [%d]", __func__, link);
     return NETMGR_FAILURE;
  }

  memset(&tunnel_info, 0, sizeof(ipsec_tunnel_info_t));

  ifinfo->link = link;
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    ifinfo->type = NETMGR_KIF_CB_IFTYPE_REVERSE;
    if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech_name)
    {
      ifinfo->tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_LBO;
    }
    else
    {
      ifinfo->tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_EPDG;
    }
  }
  else if (WDS_TECHNOLOGY_NAME_MODEM_LINK_LOCAL_V01 == tech_name)
  {
    ifinfo->type = NETMGR_KIF_CB_IFTYPE_LINK_LOCAL;
  }
  else
  {
    ifinfo->type = NETMGR_KIF_CB_IFTYPE_FORWARD;
  }
  (void)strlcpy(ifinfo->name, ifname, sizeof(ifinfo->name));

  ifinfo->addr.type = NETMGR_KIF_CB_ADDR_INVAL;
  ifinfo->flags = 0;
  ifinfo->ip_family = -1;
  ifinfo->tunnel_info = tunnel_info;

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_fill_link_ssr_cb_ifinfo
===========================================================================*/
/*!
@brief
  Helper function to fill in ifinfo structure for CB chains given a link#
*/
/*=========================================================================*/
static int
netmgr_kif_fill_link_ssr_cb_ifinfo
(
  int link,
  netmgr_kif_cb_link_ssr_info_t *ssr_ifinfo
)
{
  int rc = NETMGR_SUCCESS;
  const char *ifname;
  wds_technology_name_enum_v01 tech_type;
  netmgr_address_info_t* addr_info;

  if ( !ssr_ifinfo )
  {
    netmgr_log_err("%s(): NULL parameter passed", __func__);
    rc = NETMGR_FAILURE;
    goto bail;
  }

  memset(ssr_ifinfo, 0x0, sizeof(netmgr_kif_cb_link_ssr_info_t));

  ifname = netmgr_kif_get_name(link);
  if (ifname == NULL)
  {
     netmgr_log_err("%s(): invalid link [%d]", __func__, link);
     rc = NETMGR_FAILURE;
     goto bail;
  }

  (void)strlcpy(ssr_ifinfo->name, ifname, sizeof(ssr_ifinfo->name));
  ssr_ifinfo->link = link;

  ssr_ifinfo->addr_mask = NETMGR_KIF_IPF_MASK_INVALID;

  addr_info = netmgr_qmi_get_addr_info(link);
  if (addr_info)
  {
    if ( addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4)
    {
      ssr_ifinfo->addr_mask |= NETMGR_KIF_IPF_MASK_IPV4;
      ssr_ifinfo->ipv4.type = NETMGR_KIF_CB_ADDR_IPV4;
      ssr_ifinfo->ipv4._in4.mask = addr_info->ipv4.if_mask;
      ssr_ifinfo->ipv4._in4.addr = addr_info->ipv4.if_addr.addr.v4;
    }

    if ( addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6)
    {
      ssr_ifinfo->addr_mask |= NETMGR_KIF_IPF_MASK_IPV6;
      ssr_ifinfo->ipv6.type = NETMGR_KIF_CB_ADDR_IPV6;
      ssr_ifinfo->ipv6._in6.mask = addr_info->ipv6.if_mask;
      memcpy(&ssr_ifinfo->ipv6._in6.addr, &addr_info->ipv6.if_addr.addr,
             sizeof(ssr_ifinfo->ipv6._in6.addr));
    }
  }
  else
  {
    netmgr_log_err("%s(): unable to get addr info for link [%d]", __func__, link);
  }

  tech_type = netmgr_qmi_wds_get_tech_name(link);
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    ssr_ifinfo->type = NETMGR_KIF_CB_IFTYPE_REVERSE;
    if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech_type)
    {
      ssr_ifinfo->tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_LBO;
    }
    else
    {
      ssr_ifinfo->tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_EPDG;
    }
  }
  else
  {
    ssr_ifinfo->type = NETMGR_KIF_CB_IFTYPE_FORWARD;
  }

  if (WDS_TECHNOLOGY_NAME_MODEM_LINK_LOCAL_V01 == tech_type)
  {
    ssr_ifinfo->type = NETMGR_KIF_CB_IFTYPE_LINK_LOCAL;
  }

  rc = NETMGR_SUCCESS;
bail:
  return rc;
}

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
)
{
  netmgr_cmdq_cmd_t       *cmd_buf = NULL;
  netmgr_cmdq_cmd_info_t  *cmd_info = NULL;
  size_t cmd_str_len = 0;
  int rc = NETMGR_FAILURE;
  struct timeval tv_start, tv_end, tv_elapsed;

  cmd_info = malloc(sizeof(netmgr_cmdq_cmd_info_t));
  if (!cmd_info)
  {
    netmgr_log_err("%s(): failed to allocate command info buffer", __func__);
    goto bail;
  }

  memset(cmd_info, 0x0, sizeof(netmgr_cmdq_cmd_info_t));

  cmd_str_len = strlcpy(cmd_info->cmd_str, command, NETMGR_MAX_COMMAND_LENGTH);
  if (cmd_str_len >= NETMGR_MAX_COMMAND_LENGTH)
  {
    /* cmd_str inside cmd_info struct is atmost NETMGR_MAX_COMMAND_LENGTH long */
    netmgr_log_err("%s(): string was truncated!", __func__);
    goto bail;
  }

  cmd_buf = netmgr_cmdq_get_cmd();
  if (!cmd_buf)
  {
    netmgr_log_err("%s(): failed to get command buffer", __func__);
    goto bail;
  }

  DS_SYSCALL_DEFAULTS(adjusted_opts);

  /* We can't modify the caller's struct so cache a local copy */
  if (opts)
  {
    memcpy(&adjusted_opts, opts, sizeof(adjusted_opts));
  }

  adjusted_opts.log_options |= netmgr_main_cfg.exec_opts.log_options;
  adjusted_opts.log_options |= DS_EXEC_LOG_CMDSTR;

  /* Don't override the caller's outfile if they set one */
  if (!adjusted_opts.outfile && netmgr_main_cfg.exec_opts.outfile[0] != 0)
  {
    adjusted_opts.outfile = netmgr_main_cfg.exec_opts.outfile;
  }

  /* Don't override the caller's cmd_debug if they set one */
  if (!adjusted_opts.cmd_debug && netmgr_main_cfg.exec_opts.cmd_debug != 0)
  {
    adjusted_opts.cmd_debug = netmgr_main_cfg.exec_opts.cmd_debug;
  }

  cmd_info->cmd_type = CMD_TYPE_SYSCALL;

  /* Store a reference to the command options inside the command info struct. This will
     be internally passed into ds_exec function */
  cmd_info->cmd_opts = (void *) &adjusted_opts;

  /* Initialize the semaphore object to synchronize with the command queue */
  if (NETMGR_SUCCESS != sem_init(&cmd_info->cmd_sem, 0, 0))
  {
    netmgr_log_err("%s(): failed to initialize semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
    netmgr_cmdq_release_cmd(cmd_buf);
    goto bail;
  }

  /* Store a reference to the command info structure in the command queue object */
  cmd_buf->userdata = (void *) cmd_info;

  gettimeofday(&tv_start, 0);
  if (NETMGR_SUCCESS != netmgr_cmdq_put_cmd(cmd_buf))
  {
    netmgr_log_err("%s(): failed to queue up command", __func__);
    netmgr_cmdq_release_cmd(cmd_buf);
    goto bail;
  }

  /* Wait on the semphore till the command thread completes execution of the command */
  if (NETMGR_SUCCESS != sem_wait(&cmd_info->cmd_sem))
  {
    netmgr_log_err("%s(): failed to wait on semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
    goto bail;
  }

  /* Since we are context switching between threads there may be some amount of latency wrt
     scheduling between the exec thread and command thread. For debugging purposes we will
     log the amount of time it took to return context back to exec thread after command
     processing */
  gettimeofday(&tv_end, 0);
  timersub(&tv_end, &tv_start, &tv_elapsed);
  netmgr_log_low("%s(): command completed in %u.%06u",
                 __func__, (uint32_t)tv_elapsed.tv_sec,(uint32_t) tv_elapsed.tv_usec);

  rc = cmd_info->ret_code;

bail:
  /* Cleanup the semaphore */
  if (NETMGR_SUCCESS != sem_destroy(&cmd_info->cmd_sem))
  {
    netmgr_log_err("%s(): failed to destroy semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
  }

  if (cmd_info)
  {
    free(cmd_info);
  }

  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_update_system_config
===========================================================================*/
/*!
@brief
  Helper function to configure procfs/sysfs entries with the given value

@return
  NETMGR_SUCCESS on success
  NETMGR_FAILURE on failure

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
netmgr_kif_update_system_config
(
  const char* proc_file,
  netmgr_cmdq_cfg_val_type_t val_type,
  netmgr_cmdq_cfg_val_t val
)
{
  netmgr_cmdq_cmd_t         *cmd_buf = NULL;
  netmgr_cmdq_cmd_info_t    *cmd_info = NULL;
  netmgr_cmdq_procsys_cgf_t procsysfs_cfg;
  size_t cmd_str_len = 0;
  int rc = NETMGR_FAILURE;
  struct timeval tv_start, tv_end, tv_elapsed;

  cmd_info = malloc(sizeof(netmgr_cmdq_cmd_info_t));
  if (!cmd_info)
  {
    netmgr_log_err("%s(): failed to allocate command info buffer", __func__);
    goto bail;
  }

  memset(cmd_info, 0x0, sizeof(netmgr_cmdq_cmd_info_t));

  cmd_str_len = strlcpy(cmd_info->cmd_str, proc_file, NETMGR_MAX_COMMAND_LENGTH);
  if (cmd_str_len >= NETMGR_MAX_COMMAND_LENGTH)
  {
    netmgr_log_err("%s(): string was truncated!", __func__);
    goto bail;
  }

  cmd_buf = netmgr_cmdq_get_cmd();
  if (!cmd_buf)
  {
    netmgr_log_err("%s(): failed to get command buffer", __func__);
    goto bail;
  }

  procsysfs_cfg.config_type = val_type;
  procsysfs_cfg.config_val  = val;

  cmd_info->cmd_type = CMD_TYPE_PROCSYS_CFG;

  /* Store a reference to the command options inside the command info struct. This will
     be internally passed into ds_exec function */
  cmd_info->cmd_opts = (void *) &procsysfs_cfg;

  /* Initialize the semaphore object to synchronize with the command queue */
  if (NETMGR_SUCCESS != sem_init(&cmd_info->cmd_sem, 0, 0))
  {
    netmgr_log_err("%s(): failed to initialize semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
    netmgr_cmdq_release_cmd(cmd_buf);
    goto bail;
  }

  /* Store a reference to the command info structure in the command queue object */
  cmd_buf->userdata = (void *) cmd_info;

  gettimeofday(&tv_start, 0);
  if (NETMGR_SUCCESS != netmgr_cmdq_put_cmd(cmd_buf))
  {
    netmgr_log_err("%s(): failed to queue up command", __func__);
    netmgr_cmdq_release_cmd(cmd_buf);
    goto bail;
  }

  /* Wait on the semphore till the command thread completes execution of the command */
  if (NETMGR_SUCCESS != sem_wait(&cmd_info->cmd_sem))
  {
    netmgr_log_err("%s(): failed to wait on semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
    goto bail;
  }

  /* Since we are context switching between threads there may be some amount of latency wrt
     scheduling between the exec thread and command thread. For debugging purposes we will
     log the amount of time it took to return context back to exec thread after command
     processing */
  gettimeofday(&tv_end, 0);
  timersub(&tv_end, &tv_start, &tv_elapsed);
  netmgr_log_low("%s(): command completed in %u.%06u",
                 __func__,(uint32_t) tv_elapsed.tv_sec, (uint32_t)tv_elapsed.tv_usec);

  rc = cmd_info->ret_code;

bail:
  /* Cleanup the semaphore */
  if (NETMGR_SUCCESS != sem_destroy(&cmd_info->cmd_sem))
  {
    netmgr_log_err("%s(): failed to destroy semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
  }

  if (cmd_info)
  {
    free(cmd_info);
  }

  return rc;
}

/*===========================================================================
                            LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/* Forward declarations */
LOCAL int netmgr_kif_close( int link,
                            netmgr_address_set_t * addr_info_ptr,
                            boolean   teardown_iface );

LOCAL void netmgr_kif_open_cnf (int link);

LOCAL int
netmgr_kif_nl_post_mtu_update_msg( int link, unsigned int mtu );

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
netmgr_kif_verify_link (int link)
{

  /* Range check */
  if( (link < 0) || (link >= NETMGR_MAX_LINK) ) {
    return NETMGR_FAILURE;
  }

  if( !netmgr_kif_cfg.link_array[link].enabled ) {
    return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_flags
 ===========================================================================*/
/*!
@brief
  Helper function to get flags for the given kernel interface

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
LOCAL int
netmgr_kif_ifioctl_get_flags (const char * dev, short * p_flags)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);
  NETMGR_ASSERT(p_flags);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_get: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Get current if flags for the device */
  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_get: SIOCGIFFLAGS ioctl failed:\n");
    close(fd);
    goto error;
  }

  *p_flags = ifr.ifr_flags;

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
 error:
  return rval;

}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_flags
===========================================================================*/
/*!
@brief
  Helper function to change specified SIOCSIFFLAGS on a given device.

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
LOCAL int
netmgr_kif_ifioctl_set_flags( const char * dev, short flags, short fmask )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  /* Get current if flags for the device */
  if( ioctl(fd, SIOCGIFFLAGS, &ifr) < 0 ) {
    netmgr_log_sys_err("ifioctl_get: SIOCGIFFLAGS ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* fmask specifies which flag bits should be changed. flags specifies
  ** the value of those bits. Set bit positions indicated in fmask to the
  ** value specified in flags.
  */
  ifr.ifr_flags &= (short)(~(unsigned short)fmask);
  ifr.ifr_flags |= fmask & flags;

  /* Set if flags for the device */
  if( ioctl( fd, SIOCSIFFLAGS, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: SIOCSIFFLAGS ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_mtu
===========================================================================*/
/*!
@brief
  Helper function to change SIOCSIFMTU on a given device.

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
LOCAL int
netmgr_kif_ifioctl_set_mtu( const char * dev, unsigned int mtu )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  ifr.ifr_mtu = (int)mtu;

  /* Set if MTU for the device */
  if( ioctl( fd, SIOCSIFMTU, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: SIOCSIFMTU ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

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
netmgr_kif_get_mtu(int link)
{
  int fd;
  struct ifreq if_mtu;
  const char *ifname = netmgr_kif_get_name(link);

  if ( ifname == NULL ) {
    netmgr_log_err("netmgr_kif_get_mtu: invalid link [%d]", link);
    return -1;
  }

  fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if ( fd < 0 ) {
    netmgr_log_err("netmgr_kif_get_mtu: socket open failed [%d]", fd);
    return -1;
  }

  strlcpy(if_mtu.ifr_name, ifname, IFNAMSIZ);
  if_mtu.ifr_name[IFNAMSIZ - 1] = '\0';

  if ( ioctl(fd, SIOCGIFMTU, &if_mtu) < 0 ) {
    netmgr_log_err("netmgr_kif_get_mtu: ioctl failed to get mtu");
    close(fd);
    return -1;
  }

  netmgr_log_high("netmgr_kif_get_mtu: mtu=[%d]", if_mtu.ifr_mtu);
  close(fd);
  return if_mtu.ifr_mtu;
}

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
netmgr_kif_set_mtu(int link, boolean post_nl_ev)
{
  /* any "failed" case below is not failing to set mtu because MTU will
     be assigned a default value, BUT we follow the same pattern as other
     netmgr HELPER function to return SUCCESS or FAILURE*/
  int rval = NETMGR_FAILURE;
  unsigned int mtu4, mtu6, mtu = NETMGR_MTU_MAX;

  /* Get Modem MTU for link and assign it to network interface */
  mtu4 = netmgr_qmi_get_mtu(link, QMI_IP_FAMILY_PREF_IPV4);
  mtu6 = netmgr_qmi_get_mtu(link, QMI_IP_FAMILY_PREF_IPV6);
  netmgr_log_low("%s() Got Link MTU %d for V4\n", __func__, mtu4);
  netmgr_log_low("%s() Got Link MTU %d for V6\n", __func__, mtu6);

  if ( (NETMGR_MTU_INVALID != mtu4) && (NETMGR_MTU_INVALID != mtu6) )
  {
    mtu4 = max(mtu4, NETMGR_KIF_MIN_V6_MTU);
    mtu6 = max(mtu6, NETMGR_KIF_MIN_V6_MTU);
    mtu = min(mtu4, mtu6);
  }
  else if(NETMGR_MTU_INVALID != mtu4)
  {
    mtu = max(mtu4, NETMGR_KIF_MIN_V6_MTU);
  }
  else if(NETMGR_MTU_INVALID != mtu6)
  {
    mtu = max(mtu6, NETMGR_KIF_MIN_V6_MTU);
  }
  else if ((NETMGR_MTU_INVALID == mtu4) && (NETMGR_MTU_INVALID == mtu6) )
  {
    mtu = NETMGR_MTU_DEFAULT;
  }
  else
  {
    netmgr_log_err("%s() No valid MTU found!\n", __func__);
    return NETMGR_FAILURE;
  }

  if (mtu > NETMGR_MTU_MAX) {
    netmgr_log_med("%s() selected MTU %d larger than max MTU (%d)\n", __func__, mtu, NETMGR_MTU_MAX);
    mtu = NETMGR_MTU_MAX;
  }

  netmgr_log_low("%s() using final MTU value [%d] for link [%d]\n", __func__, mtu, link);

#ifdef FEATURE_DS_LINUX_ANDROID
  /* Configure Android MTU property value as follows:
   *   0 > value <= NETMGR_MTU_MAX - Override Modem MTU query value
   *   value == 0                  - Use Modem MTU query value
   *   value >  NETMGR_MTU_MAX     - Ignored
   */
  static char  args[PROP_VALUE_MAX];
  char  def[NETMGR_KIF_PROPERTY_MTU_SIZE+1];
  int   ret;

  memset( args, 0x0, sizeof(args) );
  memset( def, 0x0, sizeof(def) );

  /* Query Android proprerty for MTU override */
  snprintf( def, sizeof(def)-1, "%d", NETMGR_KIF_PROPERTY_MTU_DEFAULT );
  ret = property_get( NETMGR_KIF_PROPERTY_MTU, args, def );

  if( (NETMGR_KIF_PROPERTY_MTU_SIZE) < ret ) {
    netmgr_log_err( "System property %s has unexpected size(%d), skippng\n",
                    NETMGR_KIF_PROPERTY_MTU, ret );
  } else {
    ret = ds_atoi( args );
    if( NETMGR_MTU_MAX < (unsigned int)ret ) {
      netmgr_log_err( "System property %s has exceeded limit (%d), skippng\n",
                      NETMGR_KIF_PROPERTY_MTU, NETMGR_MTU_MAX );
    } else {
      if( NETMGR_MTU_INVALID != ret ) {
        /* Update MTU value using property */
        mtu = (unsigned int)ret;
        netmgr_log_high( "MTU overide specified, using value %d\n", mtu );
      }
    }
  }
#endif /* FEATURE_DS_LINUX_ANDROID */

  if( NETMGR_FAILURE ==
      netmgr_kif_ifioctl_set_mtu( netmgr_kif_get_name(link), mtu ) ) {
    netmgr_log_err( "%s() failed to set MTU, using existing MTU\n", __func__ );
  } else {
    netmgr_log_high("%s() assigned MTU %d on %s\n",
                    __func__, mtu, netmgr_kif_get_name(link) ? : "NULL");
  }

#ifdef FEATURE_DS_LINUX_ANDROID
  if (TRUE == post_nl_ev)
  {
    if (NETMGR_SUCCESS != netmgr_kif_nl_post_mtu_update_msg(link, mtu))
    {
      netmgr_log_err( "failed to post MTU update netlink event\n" );
    }
  }
#endif /* FEATURE_DS_LINUX_ANDROID */

  rval = NETMGR_SUCCESS;

 error:
  return rval;
} /* netmgr_kif_set_mtu() */


/*===========================================================================
  FUNCTION  netmgr_kif_call_ioctl_on_dev
===========================================================================*/
/*!
@brief
  Helper function to call a specified IOCTL on a given device. Caller
  is responsible to initializing struct ifreq parameter.

@return
  int - NETMGR_SUCCESS if successful, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_call_ioctl_on_dev (const char * dev, unsigned int req, struct ifreq * ifr)
{
  int fd;
  int rval = NETMGR_FAILURE;

  /* Open a temporary socket of datagram type to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("call_ioctl_on_dev: socket failed:\n");
    goto error;
  }

  /* Set device name in the ioctl req struct */
  (void)strlcpy(ifr->ifr_name, dev, sizeof(ifr->ifr_name));

  /* Issue ioctl on the device */
  if (ioctl(fd, (int)req, ifr) < 0) {
    netmgr_log_sys_err("call_ioctl_on_dev: ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);
  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

#ifndef FEATURE_DS_LINUX_DRIVER_LEGACY

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_supported_qos_modes
 ===========================================================================*/
/*!
@brief
  Helper function to query the rmnet driver to give the supported QoS modes

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
LOCAL int
netmgr_kif_ifioctl_get_supported_qos_modes(const char *dev, uint32_t *qos_modes)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  NETMGR_ASSERT(dev);
  NETMGR_ASSERT(qos_modes);

  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("get_qos_modes: socket failed:\n");
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) netmgr_malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    netmgr_log_sys_err("get_qos_modes: malloc for ifru_data failed:\n");
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_GET_SUPPORTED_QOS_MODES;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_sys_err("get_qos_modes: ioctl RMNET_IOCTL_GET_SUPPORTED_QOS_MODES failed:\n");
    close(fd);
    netmgr_free(ext_ioctl_arg);
    goto error;
  }

  *qos_modes = ext_ioctl_arg->u.data;
  netmgr_log_med("get_qos_modes: ioctl RMNET_IOCTL_GET_SUPPORTED_QOS_MODES as %d \n",
                 ext_ioctl_arg->u.data);


  /* close temporary socket */
  close(fd);
  netmgr_free(ext_ioctl_arg);
  rval = NETMGR_SUCCESS;

error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_qos_version
 ===========================================================================*/
/*!
@brief
  Function to the rmnet driver to set the supported QoS modes

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
LOCAL int
netmgr_kif_ifioctl_set_qos_version(const char *dev, uint32_t qos_modes)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("set_qos_modes: socket failed:\n");
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) netmgr_malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    netmgr_log_sys_err("set_qos_modes: malloc for ifru_data failed:\n");
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_SET_QOS_VERSION;
  ext_ioctl_arg->u.data = qos_modes;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_sys_err("set_qos_modes: ioctl RMNET_IOCTL_SET_QOS_VERSION failed:\n");
    close(fd);
    netmgr_free(ext_ioctl_arg);
    goto error;
  }

  /* close temporary socket */
  close(fd);
  netmgr_free(ext_ioctl_arg);
  rval = NETMGR_SUCCESS;

error:
  return rval;
}


/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_epid
 ===========================================================================*/
/*!
@brief
  Helper function to query the rmnet_ipa driver to give the EPID information

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
LOCAL int
netmgr_kif_ifioctl_get_epid(const char *dev, uint32 *epid)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  NETMGR_ASSERT(dev);
  NETMGR_ASSERT(epid);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("get_epid: socket failed:\n");
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) netmgr_malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    netmgr_log_sys_err("get_epid: malloc for ifru_data failed:\n");
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_GET_EPID;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_sys_err("get_epid: ioctl RMNET_IOCTL_GET_EPID failed:\n");
    close(fd);
    netmgr_free(ext_ioctl_arg);
    goto error;
  }

  *epid = ext_ioctl_arg->u.data;
  netmgr_log_med("get_epid: ioctl RMNET_IOCTL_GET_EPID as %d \n",ext_ioctl_arg->u.data);


  /* close temporary socket */
  close(fd);
  netmgr_free(ext_ioctl_arg);
  rval = NETMGR_SUCCESS;

error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_ep_pair
 ===========================================================================*/
/*!
@brief
  Helper function to query the rmnet_ipa driver to give the EP pair information

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
LOCAL int
netmgr_kif_ifioctl_get_ep_pair(const char *dev,  netmgr_ipa_ep_pair_type *ep_pair)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  NETMGR_ASSERT(dev);
  NETMGR_ASSERT(ep_pair);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ipa_get_ep_pair: socket failed:\n");
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) netmgr_malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    netmgr_log_sys_err("ipa_get_ep_pair: malloc for ifru_data failed:\n");
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_GET_EP_PAIR;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_sys_err("ipa_get_ep_pair: ioctl RMNET_IOCTL_GET_EP_PAIR failed:\n");
    close(fd);
    netmgr_free(ext_ioctl_arg);
    goto error;
  }

  ep_pair->consumer_pipe_num = ext_ioctl_arg->u.ipa_ep_pair.consumer_pipe_num;
  ep_pair->producer_pipe_num = ext_ioctl_arg->u.ipa_ep_pair.producer_pipe_num;

  netmgr_log_med("get_ep_pair: ioctl RMNET_IOCTL_GET_EP_PAIR as c:%d p:%d \n",
      ep_pair->consumer_pipe_num,ep_pair->producer_pipe_num );

  /* close temporary socket */
  close(fd);
  netmgr_free(ext_ioctl_arg);
  rval = NETMGR_SUCCESS;

error:
  return rval;
}


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
netmgr_kif_ipa_ioctl_set_dev_name(const char * dev)
{
  int ipa_fd;
  struct ipa_wan_msg wan_dev;

  if (!dev)
  {
    netmgr_log_err("Bad parameters");
    return NETMGR_FAILURE;
  }

  ipa_fd = open(NETMGR_KIF_IPA_DEV_NAME, O_RDONLY|O_CLOEXEC);

  if (ipa_fd < 0)
  {
    netmgr_log_sys_err("failed to open ipa dev node");
    return NETMGR_FAILURE;
  }

  memset(&wan_dev, 0, sizeof(struct ipa_wan_msg));
  std_strlcpy(&wan_dev.upstream_ifname , dev, IPA_RESOURCE_NAME_MAX);

  if (NETMGR_SUCCESS != (ioctl(ipa_fd,
                               IPA_IOC_NOTIFY_WAN_EMBMS_CONNECTED,
                               &wan_dev)))
  {
    netmgr_log_sys_err("ioctl: IPA_IOC_NOTIFY_WAN_EMBMS_CONNECTED failed");
    close(ipa_fd);
    return NETMGR_FAILURE;
  }

  close(ipa_fd);
  return NETMGR_SUCCESS;
}
#endif

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_get_llpmode
 ===========================================================================*/
/*!
@brief
  Helper function to get driver link-layer protocol mode for the
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
LOCAL int
netmgr_kif_ifioctl_get_llpmode( const char * dev, uint32 * mode )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_data_s ioctl_data;

  NETMGR_ASSERT(dev);
  NETMGR_ASSERT(mode);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_get: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = &ioctl_data;
  memset(&ioctl_data, 0, sizeof(struct rmnet_ioctl_data_s));

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_GET_LLP, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_get: ioctl llp failed:\n");
    close(fd);
    goto error;
  }

  *mode = ioctl_data.u.operation_mode;

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
 error:
  return rval;

}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_qosmode
 ===========================================================================*/
/*!
@brief
  Helper function to set QMI QoS mode for the given kernel interface.

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
LOCAL int
netmgr_kif_ifioctl_set_qosmode (const char * dev, boolean enable)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  int cmd = (enable)? RMNET_IOCTL_SET_QOS_ENABLE :
                      RMNET_IOCTL_SET_QOS_DISABLE;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Get current if flags for the device */
  if (ioctl(fd, cmd, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_set: ioctl qosmode failed:\n");
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
error:
  return rval;

}

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
netmgr_kif_ifioctl_flow_control(const char * dev, int handle, int enable)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_data_s ioctl_data;

  int cmd = (enable)? RMNET_IOCTL_FLOW_ENABLE :
                      RMNET_IOCTL_FLOW_DISABLE;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = &ioctl_data;
  memset(&ioctl_data, 0, sizeof(struct rmnet_ioctl_data_s));

  /* Set tc handle in ioctl req struct */
  ioctl_data.u.tcm_handle = (uint32_t)handle;

  DS_MARK_TRACE_PARAMS("netmgr sending %s IOCTL for device %s, tcm_handle %d\n",
                enable ? "enable" : "disable", dev, handle);

  /* Get current if flags for the device */
  if (ioctl(fd, cmd, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_set: ioctl flow failed:\n");
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
error:
  return rval;

}

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
netmgr_kif_ifioctl_change_sleep_state(const char * dev, int enable)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("change_sleep_state: socket failed:\n");
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) netmgr_malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    netmgr_log_sys_err("change_sleep_state: malloc for ifru_data failed:\n");
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_SET_SLEEP_STATE;
  ext_ioctl_arg->u.data = enable;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_sys_err("change_sleep_state: ioctl RMNET_IOCTL_SET_SLEEP_STATE failed:\n");
    close(fd);
    netmgr_free(ext_ioctl_arg);
    goto error;
  }
  netmgr_log_low("change_sleep_state: set IOCTL RMNET_IOCTL_SET_SLEEP_STATE on dev: %s enable: %d\n",
                 dev,
                 enable);

  /* close temporary socket */
  close(fd);
  netmgr_free(ext_ioctl_arg);
  rval = NETMGR_SUCCESS;

error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_llpmode
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
LOCAL int
netmgr_kif_ifioctl_set_llpmode( const char * dev, uint32 mode )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  int cmd;

  NETMGR_ASSERT(dev);

  if( (RMNET_MODE_LLP_ETH != mode) &&
      (RMNET_MODE_LLP_IP  != mode) ) {
    netmgr_log_err("invalid set llp mode value specified\n");
    goto error;
  }

  cmd = (RMNET_MODE_LLP_ETH == mode)? RMNET_IOCTL_SET_LLP_ETHERNET :
                                      RMNET_IOCTL_SET_LLP_IP;

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Get current if flags for the device */
  if (ioctl(fd, cmd, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_set: ioctl llp failed:\n");
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
 error:
  return rval;

}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_open_port
 ===========================================================================*/
/*!
@brief
  Helper function to open the RmNET driver transport port.

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
LOCAL int
netmgr_kif_ifioctl_open_port (const char * dev)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_OPEN, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl: open port failed:\n");
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  netmgr_log_high("Open port success: %s\n", dev);
  rval = NETMGR_SUCCESS;
 error:
  return rval;

}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_close_port
 ===========================================================================*/
/*!
@brief
  Helper function to close the RmNET driver transport port.

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
LOCAL int
netmgr_kif_ifioctl_close_port (const char * dev)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);

  /* Close a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl: socket failed:\n");
    goto error;
  }

  netmgr_log_med("ifioctl: closing port [%s]\n", dev);

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_CLOSE, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl: close port failed:\n");
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
 error:
  return rval;

}
#endif /* FEATURE_DS_LINUX_DRIVER_LEGACY */

#define NETMGR_IPV6_ENABLED 0
#define NETMGR_IPV6_DISABLED 1
LOCAL inline int
netmgr_kif_set_ipv6_disabled
(
  const char *dev_name,
  int state
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "";
  netmgr_cmdq_cfg_val_t val;

  snprintf(proc_file,
           sizeof(proc_file),
           "/proc/sys/net/ipv6/conf/%s/disable_ipv6",
           dev_name);

  val.intval = state;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to disable ipv6 on device [%s]",
                   __func__, dev_name);
  }
  return rc;
}

static int netmgr_kif_enable_ipv6_on_link
(
  int link
)
{
  const char *link_name;

  link_name = netmgr_kif_get_name(link);
  if (NULL == link_name)
  {
    netmgr_log_err("%s(): unable to determine name for link=%d\n", __func__, link);
    return NETMGR_FAILURE;
  }

  return netmgr_kif_set_ipv6_disabled(link_name, NETMGR_IPV6_ENABLED);
}

LOCAL inline int
netmgr_kif_enable_pkt_transmission_on_dev
(
  const char *dev_name
)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev_name);

  if((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0)
  {
    netmgr_log_sys_err("ifioctl_set: socket failed");
    goto error;
  }

  memset(&ifr, 0, sizeof(ifr));

  if (strlcpy(ifr.ifr_name, dev_name, sizeof(ifr.ifr_name)) >= sizeof(ifr.ifr_name))
  {
    netmgr_log_err("%s(): String truncation occurred", __func__);
    close(fd);
    goto error;
  }

  ifr.ifr_flags |= IFF_UP;

  if(ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
  {
    netmgr_log_sys_err("ifioctl_set: SIOCSIFFLAGS ioctl failed");
    close(fd);
    goto error;
  }

  close(fd);

  rval = NETMGR_SUCCESS;

error:
  netmgr_log_med("%s(): IFF_UP on [%s] : [%s]",
                 __func__, dev_name, rval == NETMGR_SUCCESS ? "succces" : "failure");
  return rval;
}

LOCAL int
netmgr_kif_extended_ioctl
(
  const char * dev,
  struct rmnet_ioctl_extended_s *ext_ioctl
)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  NETMGR_ASSERT(dev);

  /* Close a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_err("%s(): socket failed:\n", __func__);
    goto error;
  }

  netmgr_log_med("%s(): Calling extended IOCTL %04X on [%s]\n",
                 __func__, ext_ioctl->extended_ioctl&0xFFFF, dev);

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    netmgr_log_err("%s(): Extended IOCTL failed\n", __func__);
    close(fd);
    goto error;
  }

  /* close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;
 error:
  return rval;
}

LOCAL int
netmgr_kif_ifioctl_get_features
(
  const char *phys_dev,
  uint32_t *features
)
{
  int rc;
  struct rmnet_ioctl_extended_s ext_ioctl;

  NETMGR_ASSERT(phys_dev);
  NETMGR_ASSERT(features);

  ext_ioctl.extended_ioctl = RMNET_IOCTL_GET_SUPPORTED_FEATURES;
  rc = netmgr_kif_extended_ioctl(phys_dev, &ext_ioctl);
  *features = ext_ioctl.u.data;
  if (rc == NETMGR_SUCCESS)
    netmgr_log_low("%s(%s): returns 0x%08X\n", __func__, phys_dev, *features);
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_notify_mux_channel
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs to set a mux channel if the phys_dev
  supports the RMNET_MUX_NOTIFY feature

@arg phys_dev device to call IOCTL on
@arg vnd_name name of the VND associated with mux id
@arg mux_id the mux id

@return
  int - value of netmgr_kif_extended_ioctl()

*/
/*=========================================================================*/
LOCAL int
netmgr_kif_ifioctl_notify_mux_channel
(
  const char *phys_dev,
  const char *vnd_name,
  uint32_t mux_id
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;
  uint32_t features = 0;
  int rc = NETMGR_FAILURE;

  NETMGR_ASSERT(phys_dev);

  if ((netmgr_kif_ifioctl_get_features(phys_dev, &features)) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): failed to get device features, skipping\n", __func__);
    goto bail;
  }

  if ( features & RMNET_IOCTL_FEAT_NOTIFY_MUX_CHANNEL )
  {
    ext_ioctl.extended_ioctl = RMNET_IOCTL_ADD_MUX_CHANNEL;
    ext_ioctl.u.rmnet_mux_val.mux_id = mux_id;
    std_strlcpy((char*)ext_ioctl.u.rmnet_mux_val.vchannel_name, vnd_name,
                sizeof(ext_ioctl.u.rmnet_mux_val.vchannel_name));
    rc = netmgr_kif_extended_ioctl(phys_dev, &ext_ioctl);

    netmgr_log_low("%s(%s, %s, %d): returns %d\n",
                   __func__, phys_dev, vnd_name, mux_id, rc);
  }

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_egress_data_format
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs to set egress data format

@arg phys_dev device to call IOCTL on
@arg ingress_flags flags to set

@return
  int - value of netmgr_kif_extended_ioctl()

*/
/*=========================================================================*/
LOCAL int
netmgr_kif_ifioctl_set_egress_data_format
(
  const char *phys_dev,
  uint32_t egress_flags
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;
  int rc;

  NETMGR_ASSERT(phys_dev);

  ext_ioctl.extended_ioctl = RMNET_IOCTL_SET_EGRESS_DATA_FORMAT;
  ext_ioctl.u.data = egress_flags;
  rc = netmgr_kif_extended_ioctl(phys_dev, &ext_ioctl);

  netmgr_log_low("%s(%s, 0x%08X): returns %d\n",
                 __func__, phys_dev, egress_flags, rc);
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_ingress_data_format
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs to set ingress data format

@arg phys_dev device to call IOCTL on
@arg ingress_flags flags to set

@return
  int - value of netmgr_kif_extended_ioctl()

*/
/*=========================================================================*/
LOCAL int
netmgr_kif_ifioctl_set_ingress_data_format
(
  const char *phys_dev,
  uint32_t ingress_flags
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;
  netmgr_data_format_t *df;
  int rc = NETMGR_FAILURE;

  NETMGR_ASSERT(phys_dev);
  df = &netmgr_main_cfg.data_format;

  ext_ioctl.extended_ioctl = RMNET_IOCTL_SET_INGRESS_DATA_FORMAT;
  ext_ioctl.u.data = ingress_flags;

  if (ingress_flags & RMNET_IOCTL_INGRESS_FORMAT_DEAGGREGATION)
  {
    ext_ioctl.u.data |= RMNET_IOCTL_INGRESS_FORMAT_AGG_DATA;
    ext_ioctl.u.ingress_format.agg_size = df->dl_agg_size;
    ext_ioctl.u.ingress_format.agg_count = df->dl_agg_cnt;
  }
  rc = netmgr_kif_extended_ioctl(phys_dev, &ext_ioctl);

  if (ingress_flags & RMNET_IOCTL_INGRESS_FORMAT_DEAGGREGATION)
  {
    netmgr_log_low("%s(\"%s\", 0x%08X): with u.data=0x%08X"
                   " u.ingress_format.agg_size=%d"
                   " u.ingress_format.agg_count=%d"
                   " returns %d\n",
                   __func__,
                   phys_dev,
                   ingress_flags,
                   ext_ioctl.u.data,
                   ext_ioctl.u.ingress_format.agg_size,
                   ext_ioctl.u.ingress_format.agg_count,
                   rc);
  }
  else
  {
    netmgr_log_low("%s(%s, 0x%08X): returns %d\n", __func__, phys_dev, ingress_flags, rc);
  }

  return rc;
}

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
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;
  int rc;

  if(NULL == phys_dev || NULL == rmnet_data_dev)
  {
    return NETMGR_FAILURE;
  }

  ext_ioctl.extended_ioctl = RMNET_IOCTL_SET_XLAT_DEV_INFO;
  std_strlcpy((char *)ext_ioctl.u.if_name, rmnet_data_dev, IFNAMSIZ);

  rc = netmgr_kif_extended_ioctl(phys_dev, &ext_ioctl);

  if( NETMGR_FAILURE == rc)
  {
    netmgr_log_err("%s: RMNET_IOCTL_SET_RMNET_DATA_DEV_NAME failed on phys_dev[%s]",
                   __func__,phys_dev);
  }
  return rc;
}

LOCAL int
netmgr_kif_set_mru
(
  const char *dev,
  uint32_t mru
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;

  NETMGR_ASSERT(dev);

  ext_ioctl.extended_ioctl = RMNET_IOCTL_SET_MRU;
  ext_ioctl.u.data = mru;
  return netmgr_kif_extended_ioctl(dev, &ext_ioctl);
}

LOCAL int
netmgr_kif_unreg_dev
(
  const char *dev
)
{
  struct rmnet_ioctl_extended_s ext_ioctl;
  NETMGR_ASSERT(dev);

  ext_ioctl.extended_ioctl = RMNET_IOCTL_DEREGISTER_DEV;
  ext_ioctl.u.data = 0;
  if(netmgr_kif_extended_ioctl(dev, &ext_ioctl) != NETMGR_SUCCESS)
  {
    netmgr_log_err("set_dev_reg failed on %s dev",dev);
    return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

LOCAL inline int
netmgr_kif_set_rps_mask
(
  const char *dev_name,
  int mask
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "";
  netmgr_cmdq_cfg_val_t val;

  snprintf(proc_file,
           sizeof(proc_file),
           "/sys/class/net/%s/queues/rx-0/rps_cpus",
           dev_name);

  val.intval = mask;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_HEX,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to rps_mask on device [%s]",
                   __func__, dev_name);
  }
  return rc;
}


LOCAL inline int
netmgr_kif_set_netdev_budget
(
  int budget
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "/proc/sys/net/core/netdev_budget";
  netmgr_cmdq_cfg_val_t val;

  val.intval = budget;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to netdev_budget", __func__);
  }
  return rc;
}

LOCAL inline int
netmgr_kif_set_netdev_max_backlog
(
  int max_backlog
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "/proc/sys/net/core/netdev_max_backlog";
  netmgr_cmdq_cfg_val_t val;

  val.intval = max_backlog;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to netdev_max_backlog", __func__);
  }
  return rc;
}

LOCAL inline int
netmgr_kif_disable_tcp_hystart_detect
(
  void
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "/sys/module/tcp_cubic/parameters/hystart_detect";
  netmgr_cmdq_cfg_val_t val;
  int config = 0;

  val.intval = config;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to disable tcp_cubic hystart_detect", __func__);
  }
  return rc;
}

LOCAL inline int
netmgr_kif_disable_hystart
(
  void
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "/sys/module/tcp_cubic/parameters/hystart";
  netmgr_cmdq_cfg_val_t val;
  int config = 0;

  val.intval = config;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to disable hystart", __func__);
  }
  return rc;
}

LOCAL inline int
netmgr_kif_set_initial_ssthresh
(
  int ssthresh_val
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "/sys/module/tcp_cubic/parameters/initial_ssthresh";
  netmgr_cmdq_cfg_val_t val;

  val.intval = ssthresh_val;
  rc = netmgr_kif_update_system_config(proc_file,
                                       NETMGR_CMDQ_TYPE_INT,
                                       val);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): failed to set initial_ssthresh", __func__);
  }
  return rc;
}



/*===========================================================================
  FUNCTION  netmgr_kif_set_ip_frag_params
 ===========================================================================*/
/*!
@brief
  Sets the system wide ipv4 low and high fragmentation threshold
  memory limit. Even if any of the params fail to be set, we still try to
  set all other params. Data path will be functional if any of the params
  are failed to be set but might experience out of memory conditions in
  situations like IP fragmentation attacks

@arg frag_low_mem Any amount of packets exceeding the high threshold of memory
will be discarded until low threshold is reached
@arg frag_high_mem maximum memory used to reassemble IP fragments

@return
  int - void

*/
/*=========================================================================*/
LOCAL void
netmgr_kif_set_ip_frag_params
(
  int frag_low_mem,
  int frag_high_mem
)
{
  int rc;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "";
  netmgr_cmdq_cfg_val_t val;

  if (frag_low_mem > 0)
  {
    netmgr_log_low("%s(): Setting ipv4 ipfrag_low_thresh to %d\n",
                   __func__, frag_low_mem);

    memset(proc_file, 0x0, NETMGR_KIF_MAX_COMMAND_LENGTH);
    std_strlprintf(proc_file,
                   sizeof(proc_file),
                   "/proc/sys/net/ipv4/ipfrag_low_thresh");

    val.intval = frag_low_mem;
    if ((rc = netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
          != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to set ipv4 ipfrag_low_thresh!", __func__);
    }
  }

  if (frag_high_mem > 0)
  {
    netmgr_log_low("%s(): Setting ipv4 ipfrag_high_thresh to %d\n",
                   __func__, frag_high_mem);

    memset(proc_file, 0x0, NETMGR_KIF_MAX_COMMAND_LENGTH);
    std_strlprintf(proc_file,
                   sizeof(proc_file),
                   "/proc/sys/net/ipv4/ipfrag_high_thresh");

    val.intval = frag_high_mem;
    if ((rc = netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
          != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to set ipv4 ipfrag_high_thresh!", __func__);
    }
  }
}

int
netmgr_kif_configure_ep_params
(
  const char* dev_name
)
{
  uint32 epid;
  netmgr_ipa_ep_pair_type ep_pair;

  memset(&ep_pair, 0, sizeof(ep_pair));
  if (netmgr_kif_ifioctl_get_ep_pair(dev_name, &ep_pair) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error getting the EP_PAIR information for dev %s\n", __func__, dev_name);
    return NETMGR_FAILURE;
  }
  else
  {
    netmgr_log_low("%s(): Got EP_PAIR c:%d p:%d for device %s\n",
        __func__, ep_pair.consumer_pipe_num, ep_pair.producer_pipe_num, dev_name);
    netmgr_main_cfg.consumer_pipe_num = ep_pair.consumer_pipe_num;
    netmgr_main_cfg.producer_pipe_num = ep_pair.producer_pipe_num;
  }

  if (netmgr_kif_ifioctl_get_epid(dev_name, &epid) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error getting the EPID information for dev %s\n", __func__, dev_name);
    return NETMGR_FAILURE;
  }
  else
  {
    netmgr_log_low("%s(): Got EPID %d for device %s\n", __func__, epid, dev_name);
    netmgr_main_cfg.epid = epid;
  }

  return NETMGR_SUCCESS;
}

int
netmgr_kif_init_physical_transport
(
  const char *dev_name,
  netmgr_data_format_t *data_format
)
{
  uint32 epid;
  netmgr_ipa_ep_pair_type ep_pair;
  if (netmgr_kif_ifioctl_set_llpmode(dev_name, (uint32)data_format->link_prot)  != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s() Failed to set link layer protocol on device (%s)", __func__, dev_name);
    return NETMGR_FAILURE;
  }

  if (netmgr_kif_set_mru(dev_name, (uint32_t)data_format->dl_agg_size) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error setting MRU to %d on the transport (%s)",
                   __func__, data_format->dl_agg_size, dev_name);
  }

  if (netmgr_kif_ifioctl_open_port(dev_name) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error opening the transport (%s)", __func__, dev_name);
    return NETMGR_FAILURE;
  }

  if (netmgr_kif_set_ipv6_disabled(dev_name, NETMGR_IPV6_DISABLED) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Failed to disable IPv6 on transport (%s)", __func__, dev_name);
    return NETMGR_FAILURE;
  }

  if (netmgr_kif_enable_pkt_transmission_on_dev(dev_name) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error putting transport in up state (%s)", __func__, dev_name);
    return NETMGR_FAILURE;
  }

  if (netmgr_kif_ifioctl_get_epid(dev_name, &epid) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): Error getting the EPID information for dev %s\n", __func__, dev_name);
  }
  else
  {
    netmgr_log_low("%s(): Got EPID %d for device %s\n", __func__, epid, dev_name);
    netmgr_main_cfg.epid = epid;
  }

  if (netmgr_main_cfg.pnd_rps_mask > 0)
  {
    netmgr_log_low("%s(): Setting RPS mask on physical net device %s to %d\n",
                   __func__, dev_name, netmgr_main_cfg.pnd_rps_mask);

    if (netmgr_kif_set_rps_mask(dev_name, netmgr_main_cfg.pnd_rps_mask) != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to RPS mask on transport (%s)."
                     " Data path will function but at lower throughput\n",
                      __func__, dev_name);
    }
  }

  if (netmgr_main_cfg.netdev_budget > 0)
  {
    netmgr_log_low("%s(): Setting netdev_budget to %d\n",
                   __func__, netmgr_main_cfg.netdev_budget);

    if (netmgr_kif_set_netdev_budget(netmgr_main_cfg.netdev_budget) != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to netdev budget."
                     " Data path will function but at lower throughput\n",
                     __func__);
    }
  }

  if (netmgr_main_cfg.netdev_max_backlog > 0)
  {
    netmgr_log_low("%s(): Setting netdev_max_backlog to %d\n",
                   __func__, netmgr_main_cfg.netdev_max_backlog);

    if (netmgr_kif_set_netdev_max_backlog(netmgr_main_cfg.netdev_max_backlog) != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to set netdev_max_backlog."
                     " Data path will function but at lower throughput\n",
                     __func__);
    }
  }

  if(netmgr_main_cfg.data_format.ul_gso_enabled == 1){
    netmgr_log_low("%s(): Setting SG on dev %s\n",
                   __func__, netmgr_main_cfg.phys_net_dev);
    if (netmgr_kif_enable_ul_ssg_feature(netmgr_main_cfg.phys_net_dev) != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s(): Failed to set SG on dev %s",
                     __func__, netmgr_main_cfg.phys_net_dev);
    }
  }

  if (netmgr_main_cfg.disable_tcp_hystart_detect == 1)
  {
    netmgr_log_low("%s(): Disabling hystart_detect in tcp_cubic\n",
                   __func__);

    if (netmgr_kif_disable_tcp_hystart_detect() != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to disable hystart_detect in tcp_cubic."
                     " Data path will function but at lower throughput\n",
                     __func__);
    }
  }

  if (netmgr_main_cfg.disable_hystart == 1)
  {
    netmgr_log_low("%s(): Disabling hystart in tcp_cubic\n",
                   __func__);

    if (netmgr_kif_disable_hystart() != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to disable hystart in tcp_cubic."
                     " Data path will function but at lower throughput\n",
                     __func__);
    }
  }

  if (netmgr_main_cfg.initial_ssthresh != -1)
  {
    netmgr_log_low("%s(): Setting initial ssthresh to %d\n",
                   __func__, netmgr_main_cfg.initial_ssthresh);

    if (netmgr_kif_set_initial_ssthresh(netmgr_main_cfg.initial_ssthresh) != NETMGR_SUCCESS)
    {
      netmgr_log_med("%s(): Failed to set initial ssthresh."
                     " Data path will function but at lower throughput\n",
                     __func__);
    }
  }

  netmgr_kif_set_ip_frag_params(netmgr_main_cfg.frag_low_mem, netmgr_main_cfg.frag_high_mem);

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_set_data_format
 ===========================================================================*/
/*!
@brief
  Calls the optional extended IOCTLs on physical data ports

@return
  void

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
)
{
  uint32_t features = 0;

  if (phys_dev == NULL)
  {
    netmgr_log_err("%s(): Invalid device", __func__);
    return NETMGR_FAILURE;
  }

  if ((netmgr_kif_ifioctl_get_features(phys_dev, &features)) != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s(): failed to get device features, skipping\n", __func__);
    return NETMGR_FAILURE;
  }

  if (features & RMNET_IOCTL_FEAT_SET_EGRESS_DATA_FORMAT)
  {
    egress_flags = netmgr_rmnet_convert_rmnet_edf_to_ioctl_edf(egress_flags);
    if (netmgr_kif_ifioctl_set_egress_data_format(phys_dev, egress_flags) != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s(): failed to set egress data format", __func__);
      return NETMGR_FAILURE;
    }
  }

  if (features & RMNET_IOCTL_FEAT_SET_INGRESS_DATA_FORMAT)
  {
    ingress_flags = netmgr_rmnet_convert_rmnet_idf_to_ioctl_idf(ingress_flags);
    if (netmgr_kif_ifioctl_set_ingress_data_format(phys_dev, ingress_flags) != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s(): failed to set ingress data format", __func__);
      return NETMGR_FAILURE;
    }
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_clear_address
===========================================================================*/
/*!
@brief
  Routine to clear the specified address from the kernel network interface.
  This uses the libnetutils functions to reset socket connections using
  address and delete the address from the network interface.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_purge_address( int link, netmgr_ip_address_t * addr_ptr, unsigned int mask )
{
  char buff[INET6_ADDRSTRLEN+1];
  int ret = NETMGR_FAILURE;
  int rc;
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  unsigned int cmdlen = 0;

  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT(addr_ptr);

  memset( buff, 0x0, sizeof(buff) );

  do {

    if( NETMGR_IPV4_ADDR == addr_ptr->type )
    {
      struct in_addr addr;

      /* Convert address to string notation */
      memset( &addr, 0x0, sizeof(addr) );
      memcpy( &addr.s_addr, &addr_ptr->addr.v4, sizeof(addr.s_addr) );
      if( NULL == inet_ntop( AF_INET, &addr, buff, INET_ADDRSTRLEN ) )
      {
        netmgr_log_sys_err("error on inet_ntop():\n");
        break;
      }

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(FEATURE_GB_NET_UTILS) && !defined(NETMGR_OFFTARGET)
      /* Remove address from network interface */
      netmgr_log_med("deleting address [%s] from iface[%s]\n", buff, netmgr_kif_info[ link ].name );
      rc = ifc_del_address( netmgr_kif_info[ link ].name, buff, mask );
      if( 0 > rc )
      {
        netmgr_log_sys_err("error on ifc_del_address():\n");
        break;
      }

#else
      cmdlen = snprintf( cmd, sizeof(cmd), NETMGR_KIF_SYSCMD_V4_DELADDR,buff,
                               netmgr_kif_info[ link ].name );

      rc = netmgr_ds_exec( cmd, NULL);
      if( rc != DS_EXEC_OK ) {
        netmgr_log_sys_err("failed system call:\n");
        ret = NETMGR_FAILURE;
        break;
      }

#endif /* FEATURE_DS_LINUX_ANDROID && !FEATURE_GB_NET_UTILS */

    }
    else
    {
      struct in6_addr addr;
#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(FEATURE_GB_NET_UTILS)
      struct in6_ifreq ifr6;
      int fd;
#endif
      /* Convert address to string notation */
      memset( &addr, 0x0, sizeof(addr) );
      memcpy( addr.s6_addr, addr_ptr->addr.v6_addr8, sizeof(addr.s6_addr) );
      if( NULL == inet_ntop( AF_INET6, &addr, buff, INET6_ADDRSTRLEN ) )
      {
        netmgr_log_sys_err("error on inet_ntop():\n");
        break;
      }

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(FEATURE_GB_NET_UTILS)
      /* Remove address from network interface */
      DS_SYSCALL_DEFAULTS(opts);
      opts.stderr = err_str;
      opts.stderr_length = sizeof(err_str);
      opts.log_options |= DS_EXEC_LOG_EXECTIME;

      cmdlen = snprintf( cmd, sizeof(cmd), NETMGR_KIF_SYSCMD_V6_FLUSH_GLOBAL_ADDR,
                                netmgr_kif_info[ link ].name );

      rc = netmgr_ds_exec(cmd, &opts);

      if( rc != DS_EXEC_OK )
      {
        netmgr_log_err("%s(): cmd failed with err: %s", __func__, err_str);
        ret = NETMGR_FAILURE;

        break;
      }

#else
      DS_SYSCALL_DEFAULTS(opts);
      opts.stderr = err_str;
      opts.stderr_length = sizeof(err_str);
      opts.log_options |= DS_EXEC_LOG_EXECTIME;

      cmdlen = snprintf( cmd, sizeof(cmd), NETMGR_KIF_SYSCMD_V6_DELADDR,buff,mask,
                               netmgr_kif_info[ link ].name );

       rc = netmgr_ds_exec(cmd, &opts);

       if( rc != DS_EXEC_OK )
       {
        netmgr_log_err("%s(): cmd failed with err: %s", __func__, err_str);
        ret = NETMGR_FAILURE;
        break;
       }
#endif /* FEATURE_DS_LINUX_ANDROID && !FEATURE_GB_NET_UTILS */

    }

    ret = NETMGR_SUCCESS;
  } while (0);




  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_clear_iface
===========================================================================*/
/*!
@brief
  Routine to clear the kernel interface configuration using system command.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_clear_iface( int link )
{
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  unsigned int cmdlen = 0;
  int ret = NETMGR_SUCCESS;
  int rc;
  const char *iface_name = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  netmgr_log_high("clearing kernel interface for link [%d]\n", link );

  cmdlen = (unsigned int)snprintf( cmd, sizeof(cmd), NETMGR_KIF_SYSCMD_FLUSHADDR,
                           netmgr_kif_info[ link ].name );

  rc = netmgr_ds_exec( cmd, NULL );
  if( rc != DS_EXEC_OK ) {
    netmgr_log_sys_err("failed system call:\n");
    ret = NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_nl_post_addr_msg
===========================================================================*/
/*!
@brief
  Post address event message to NetLink clients

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_nl_post_addr_msg( int link, const netmgr_nl_msg_t * nlmsg_info_ptr )
{
  netmgr_nl_event_info_t event_info;
  const char *iface_name;

  NETMGR_ASSERT( nlmsg_info_ptr );

  NETMGR_LOG_FUNC_ENTRY;

  /* Post message to clients for kernel address updates */
  if( NETMGR_NLA_PARAM_PREFIXADDR & nlmsg_info_ptr->addr_info.attr_info.param_mask ) {
    memset( &event_info, 0x0, sizeof(event_info) );

    if( RTM_NEWADDR == nlmsg_info_ptr->type ) {
      event_info.event = NET_PLATFORM_NEWADDR_EV;
    } else if( RTM_DELADDR == nlmsg_info_ptr->type ) {
      event_info.event = NET_PLATFORM_DELADDR_EV;
    } else {
      netmgr_log_err("netmgr_kif_nl_post_addr_msg unsupported event type\n" );
      return NETMGR_FAILURE;
    }

    /* Populate attributes */
    event_info.link = link;
    event_info.param_mask |= NETMGR_EVT_PARAM_LINK;

    iface_name = netmgr_kif_get_name(link);
    if(NULL == iface_name)
    {
      netmgr_log_err("netmgr_kif_nl_post_addr_msg unable to determine name for link=%d\n", link);
      return NETMGR_FAILURE;
    }

    strlcpy( event_info.dev_name,
             iface_name,
             sizeof(event_info.dev_name) );
    event_info.param_mask |= NETMGR_EVT_PARAM_DEVNAME;

    memcpy( &event_info.addr_info.addr.ip_addr,
            &nlmsg_info_ptr->addr_info.attr_info.prefix_addr,
            sizeof(event_info.addr_info.addr.ip_addr) );
    event_info.addr_info.addr.mask = (unsigned int)
      ds_get_num_bits_set_count(nlmsg_info_ptr->addr_info.metainfo.ifa_prefixlen);
    event_info.addr_info.flags = nlmsg_info_ptr->addr_info.metainfo.ifa_flags;
    event_info.param_mask |= NETMGR_EVT_PARAM_IPADDR;

    if( NETMGR_NLA_PARAM_CACHEINFO & nlmsg_info_ptr->addr_info.attr_info.param_mask ) {
      memcpy( &event_info.addr_info.cache_info,
              &nlmsg_info_ptr->addr_info.attr_info.cache_info,
              sizeof(event_info.addr_info.cache_info) );
      event_info.param_mask |= NETMGR_EVT_PARAM_CACHE;
    } else {
      netmgr_log_med("netmgr_kif_nl_post_addr_msg no address cacheinfo in NEWADDR indication\n" );
    }

    event_info.mtu = netmgr_kif_get_mtu(link);
    event_info.param_mask |= NETMGR_EVT_PARAM_MTU;

    if( NETMGR_SUCCESS != netmgr_kif_send_event_msg( &event_info ) ) {
      netmgr_log_err("failed on send NET_PLATFORM_xxxADDR_EV\n");
      return NETMGR_FAILURE;
    }
  } else {
    netmgr_log_err("netmgr_kif_nl_post_addr_msg no address in ADDR indication\n" );
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

#ifdef FEATURE_DS_LINUX_ANDROID
/*===========================================================================
  FUNCTION  netmgr_kif_nl_post_mtu_update_msg
===========================================================================*/
/*!
@brief
  Post MTU update event message to NetLink clients

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_nl_post_mtu_update_msg( int link, unsigned int mtu )
{
  netmgr_nl_event_info_t event_info;
  const char *ifname = netmgr_kif_get_name(link);

  NETMGR_LOG_FUNC_ENTRY;

  if ( ifname == NULL ) {
    netmgr_log_err("netmgr_kif_get_mtu: invalid link [%d]", link);
    return NETMGR_FAILURE;
  }

  memset(&event_info, 0, sizeof(event_info));
  event_info.event = NET_PLATFORM_MTU_UPDATE_EV;

  /* Populate attributes */
  event_info.link = link;
  event_info.param_mask |= NETMGR_EVT_PARAM_LINK;

  memcpy( &event_info.dev_name,
          ifname,
          sizeof(event_info.dev_name) );
  event_info.param_mask |= NETMGR_EVT_PARAM_DEVNAME;

  event_info.mtu = mtu;
  event_info.param_mask |= NETMGR_EVT_PARAM_MTU;

  if( NETMGR_SUCCESS != netmgr_kif_send_event_msg( &event_info ) ) {
    netmgr_log_err("failed on send NET_PLATFORM_MTU_UPDATE_EV\n");
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}
#endif /* defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET) */

/*===========================================================================
  FUNCTION  netmgr_kif_open_req
===========================================================================*/
/*!
@brief
  Issues IOCTL to bring up the specified interface

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_open_req( int link, netmgr_address_set_t * addr_info_ptr )
{
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  unsigned int cmdlen = 0;
  char mode;
  int rc;
  short flags;

  NETMGR_ASSERT( addr_info_ptr);

  NETMGR_LOG_FUNC_ENTRY;

  netmgr_log_high("bring up kernel interface for link [%d]  family[%d]\n",
                  link, addr_info_ptr->if_addr.type );

  /*Keep track of IP family to expect Newaddr for from the moment KIF_open_req
    is sent. This won't be applicable for rev links as we don't rely on Newaddr
    events to change their KIF states*/
  netmgr_kif_info[link].reconfig_ip_type = addr_info_ptr->if_addr.type;
  netmgr_log_med("Debug: Waiting for IP %d Newaddr on link %d\n",
                       addr_info_ptr->if_addr.type, link);

  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFUP_PRE, (void *)&link);
  /* Check kernel interface state to see if already UP. Another entity
   * may have already set the state. */
  if (netmgr_kif_ifioctl_get_flags(netmgr_kif_info[link].name, &flags) < 0) {
    NETMGR_ABORT("netmgr_kif_open_req: open req failed, aborting!\n");
  }

  if (flags & IFF_UP) {
    netmgr_log_high("kernel interface %d found to be open, reusing\n", link);
    /* Report interface open */
    netmgr_kif_open_cnf(link);
  }
  else
  {
    /* To bring up interface, issue ioctl to set IFF_UP flag to 1 */
    if( netmgr_kif_ifioctl_set_flags( netmgr_kif_get_name(link), IFF_UP, IFF_UP ) < 0 ) {
      NETMGR_ABORT("netmgr_kif_open_req: open req failed, aborting!\n");
    }
  }
  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFUP_POST, (void *)&link);

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_close_req
===========================================================================*/
/*!
@brief
  Issues IOCTL to bring down the specified interface.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_close_req (int link)
{
  struct netmgr_kif_cb_ifinfo ifinfo;
  int rc;

  NETMGR_LOG_FUNC_ENTRY;

  rc = netmgr_kif_fill_ifinfo(link, &ifinfo);

  if (rc == NETMGR_SUCCESS)
    netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_PRE, (void *)&ifinfo);

  netmgr_log_high("bring down kernel interface for link %d\n", link );
  /* To bring down interface, issue ioctl to set IFF_UP flag to 0 */
  if (netmgr_kif_ifioctl_set_flags(netmgr_kif_get_name(link), ~IFF_UP, IFF_UP) < 0) {
    netmgr_log_err("netmgr_kif_close_req: close req failed, aborting!\n");
  }

  if (rc == NETMGR_SUCCESS)
    netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_POST, (void *)&ifinfo);

  NETMGR_LOG_FUNC_EXIT;
  return;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_get_modem_link_type
===========================================================================*/
/*!
@brief
  Returns the modem and link type for the given link index

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_modem_link_type
(
  int                    link,
  int                    *modem,
  netmgr_main_link_type  *link_type
)
{
  int ret = NETMGR_FAILURE;

  if (!modem || !link_type) {
    netmgr_log_err("netmgr_kif_get_modem_link_type: invalid input\n");
    goto bail;
  }

  if (link >= NETMGR_LINK_RMNET_0 && link <= NETMGR_LINK_RMNET_10) {
    *modem = NETMGR_MODEM_MSM;
    *link_type = NETMGR_FWD_LINK;
  }
  else if (link >= NETMGR_LINK_RMNET_11 && link <= NETMGR_LINK_RMNET_21) {
    *modem = NETMGR_MODEM_MDM;
    *link_type = NETMGR_FWD_LINK;
  }
  else if (link >= NETMGR_LINK_REV_RMNET_0 && link <= NETMGR_LINK_REV_RMNET_8) {
    *modem = NETMGR_MODEM_MSM;
    *link_type = NETMGR_REV_LINK;
  }
  else if (link >= NETMGR_LINK_REV_RMNET_9 && link <= NETMGR_LINK_REV_RMNET_17) {
    *modem = NETMGR_MODEM_MDM;
    *link_type = NETMGR_REV_LINK;
  }
  else {
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_rev_rmnet_cfg_req
===========================================================================*/
/*!
@brief
  Handle the address configuration request for the given reverse rmnet link

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_rev_rmnet_cfg_req
(
  int link,
  netmgr_ip_addr_t addr_type
)
{
  netmgr_address_info_t  *addr_info_ptr = netmgr_qmi_get_addr_info(link);
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char local_addr[NETMGR_MAX_STR_LENGTH] = "";
  netmgr_exec_cmd_t * cmd_buf = NULL;
  const netmgr_kif_clntcb_t * clntcb;
  int result = NETMGR_FAILURE;
  void * clnt_hdl;

  NETMGR_LOG_FUNC_ENTRY;

  if (!addr_info_ptr)
  {
    netmgr_log_err("netmgr_kif_rev_rmnet_cfg_req: invalid input\n");
    goto bail;
  }

  /* Don't assign an address when configuring the WLAN LB interface.
     This interface will share the same address with WLAN and if this
     interface is brought down (with WLAN address assigned) then all
     applications using the WLAN interface will see their sockets
     reset due to the shared address and hence fail. This was observed
     during VPN + iWLAN concurrency
     Similarly, address assignment will be skipped for all Reverse links,
     due to an AOSP change where sockets are being cleared based on IP
     address instead of interface name.
     Forward links, on account of sharing the address with reverse links
     previously, were having their sockets cleared when Reverse Links
     went down. */
  if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == netmgr_qmi_wds_get_tech_name(link))
  {
    netmgr_kif_set_state(link, NETMGR_KIF_OPEN);

    if (NETMGR_IPV4_ADDR == addr_type)
    {
      if (NULL !=
            inet_ntop(AF_INET, &(addr_info_ptr->ipv4.if_addr.addr.v4),
                      local_addr, NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_med("%s(): LBO connected, check if we need to update "
                       "local_addr in IKE table [%s]",
                        __func__, local_addr);
        netmgr_main_cfg_associate_local_addr_with_ike_ports(local_addr);
      }
    }

    result = NETMGR_SUCCESS;
    goto bail;
  }

  result = NETMGR_SUCCESS;

  /* Force the KIF state maching to transition. Let's save some time */
  /*-------------------------------------------------------------------------
    Post message to executive
  -------------------------------------------------------------------------*/
  cmd_buf = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd_buf);

  cmd_buf->data.link = link;
  //cmd_buf->data.info.connect_msg.nlmsg_info = 0;
  /* Silly question: Who frees this? */
  if (NETMGR_ADDRSET_MASK_IPV4 & addr_info_ptr->valid_mask)
  {
    cmd_buf->data.info.connect_msg.addr_info_ptr = &addr_info_ptr->ipv4;
  }
  else
  {
    cmd_buf->data.info.connect_msg.addr_info_ptr = &addr_info_ptr->ipv6;
  }
  cmd_buf->data.info.connect_msg.reconfig_required = FALSE;

  /* Process based on current interface state */
  switch( netmgr_kif_get_state( link ) ) {
    case NETMGR_KIF_OPENING:
      /* Interface is in configuring state. Transition to open state */
      netmgr_kif_set_state( link, NETMGR_KIF_OPEN );
      cmd_buf->data.type = NETMGR_KIF_CONFIGURED_EV;

      (void)netmgr_kif_set_mtu(link, FALSE);

      /* Call registered client handler to indicate that the interface has
      ** come up.
      */
      clntcb = netmgr_kif_get_clntcb(link);
      if( clntcb ) {
        clnt_hdl = netmgr_kif_get_clnt_hdl(link);
        (clntcb->opened_cb)((uint8)link, NETMGR_SUCCESS, clnt_hdl);
      }
      break;

    case NETMGR_KIF_RECONFIGURING:
      netmgr_kif_set_state(link, NETMGR_KIF_OPEN);
      cmd_buf->data.type = NETMGR_KIF_RECONFIGURED_EV;

      (void)netmgr_kif_set_mtu(link, FALSE);

      clntcb = netmgr_kif_get_clntcb(link);
      if( clntcb ) {
        clnt_hdl = netmgr_kif_get_clnt_hdl(link);
        (clntcb->reconfigured_cb)((uint8)link, NETMGR_SUCCESS, clnt_hdl);
      }
      break;

    case NETMGR_KIF_OPEN:
      /* No event to executive in this case */
    default:
      /* Ignore in all other states */
      netmgr_exec_release_cmd(cmd_buf);
      goto bail;
  }

  /* Post command for processing in the command thread context */
  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
    netmgr_exec_release_cmd(cmd_buf);
    NETMGR_ABORT("failed to put commmand");
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return result;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_kif_cfg_req
===========================================================================*/
/*!
@brief
  Executes operations to kick-start interface configuration.
  getruntimesettings() is now used for both ipv4 and ipv6 configuration
  on LE only

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_cfg_req (int link, netmgr_ip_addr_t addr_type, boolean is_reconfig)
{
  netmgr_address_info_t * addr_info_ptr = NULL;
  netmgr_address_set_t  * addr_ptr = NULL;
  struct netmgr_kif_cb_ifinfo ifinfo;
  int result = NETMGR_SUCCESS;
  const char *ifname;
  int sockfd;
  wds_technology_name_enum_v01 tech_name = netmgr_qmi_wds_get_tech_name(link);
  const char *local_addr = NULL, *dest_addr = NULL;
  int tunnel_family;
  int ip_family = AF_INET;

  ifname = netmgr_kif_get_name(link);
  if (ifname == NULL)
  {
     netmgr_log_err("%s() invalid link [%d]", __func__, link);
     return NETMGR_FAILURE;
  }

  addr_info_ptr = netmgr_qmi_get_addr_info( link );
  if(!addr_info_ptr)
  {
    netmgr_log_err("%s() cannot get address info\n", __func__);
    return NETMGR_FAILURE;
  }

  ip_family = (NETMGR_IPV4_ADDR == addr_type) ? AF_INET : AF_INET6;

  ifinfo.link = link;
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    /* Install the SAs here so that the tunnel is setup before we install tunnel end-point
       specific routes */
    /* Find the associated forward rmnet (if any) and install the SA, routing rules */
#ifdef FEATURE_DATA_IWLAN
    (void) netmgr_qmi_iwlan_update_link_assoc(link, NULL);

    if (NETMGR_SUCCESS != netmgr_kif_install_sa_and_routing_rules(link,
                                                                  ip_family))
    {
      netmgr_log_err("%s(): failed installing SAs and routing rules\n", __func__);
    }

    ifinfo.type = NETMGR_KIF_CB_IFTYPE_REVERSE;
    if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech_name)
    {
      ifinfo.tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_LBO;
    }
    else
    {
      ifinfo.tech_type = NETMGR_KIF_CB_IF_TECH_TYPE_EPDG;
    }

    if (NETMGR_SUCCESS != netmgr_qmi_get_ipsec_tunnel_endpoints(link,
                                                                ip_family,
                                                                &local_addr,
                                                                &dest_addr,
                                                                &tunnel_family))
    {
      netmgr_log_err("%s(): failed to get tunnel endpoints for link [%d] ip_family [%d]",
                     __func__, link, ip_family);

      /*LBO calls do not require Tunneling so only fail for EPDG calls*/
      if(ifinfo.tech_type == NETMGR_KIF_CB_IF_TECH_TYPE_EPDG)
        return NETMGR_FAILURE;
    }
#endif /* FEATURE_DATA_IWLAN */

    ifinfo.tunnel_info.local_addr    = local_addr;
    ifinfo.tunnel_info.dest_addr     = dest_addr;
    ifinfo.tunnel_info.tunnel_family = tunnel_family;
  }
  else if (WDS_TECHNOLOGY_NAME_MODEM_LINK_LOCAL_V01 == tech_name)
    ifinfo.type = NETMGR_KIF_CB_IFTYPE_LINK_LOCAL;
  else
    ifinfo.type = NETMGR_KIF_CB_IFTYPE_FORWARD;
  (void)strlcpy(ifinfo.name, ifname, sizeof(ifinfo.name));

  switch(addr_type)
  {
    case NETMGR_IPV4_ADDR:
      ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4;
      ifinfo.addr._in4.addr = addr_info_ptr->ipv4.if_addr.addr.v4;
      ifinfo.addr._in4.mask = addr_info_ptr->ipv4.if_mask;
      break;
    case NETMGR_IPV6_ADDR:
      ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV6;
      memcpy(&ifinfo.addr._in6.addr,
             &addr_info_ptr->ipv6.if_addr.addr,
             sizeof(ifinfo.addr._in6.addr));
      ifinfo.addr._in6.mask = addr_info_ptr->ipv6.if_mask;
      break;
    default:
      netmgr_log_err("%s(): unknown addr_type=%d\n", __func__, addr_type);
      return NETMGR_FAILURE;
  }

  ifinfo.flags = 0;

  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFCFG_PRE, (void *)&ifinfo);

#ifdef FEATURE_DATA_IWLAN
  /* If the given link is a reverse Rmnet link, directly assign the address */
  if (ifinfo.type == NETMGR_KIF_CB_IFTYPE_REVERSE)
  {
    result = netmgr_kif_rev_rmnet_cfg_req(link, addr_type);

    /* Send the success/failure update to the modem */
    netmgr_qmi_send_rev_ip_config_complete(NETMGR_QMI_IWLAN_CALL_BRINGUP,
                                           link,
                                           ip_family,
                                           result);
    goto done;
  }
#endif /* FEATURE_DATA_IWLAN */

  if(NETMGR_IPV4_ADDR == addr_type)
  {
    struct ifreq ifr;
    struct sockaddr_in ipv4_addr;
    char v4_addr[INET_ADDRSTRLEN], v4_netmask[INET_ADDRSTRLEN];
    char cmd[NETMGR_KIF_MAX_COMMAND_LENGTH];

    sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);

    if (sockfd  < 0)
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req: socket failed:");
      return NETMGR_FAILURE;
    }

    (void)strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    memset(&ipv4_addr, 0, sizeof(ipv4_addr));

    /* reset IP address */
    ipv4_addr.sin_family = AF_INET;
    ipv4_addr.sin_port = 0;
    ipv4_addr.sin_addr.s_addr = 0;

    memcpy(&ifr.ifr_addr,
           &ipv4_addr,
           sizeof(struct sockaddr));

    if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0)
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req: Cannot clear ipv4 address:");
      perror(ifname);
      close(sockfd);
      return NETMGR_FAILURE;
    }

    close(sockfd);

    /*configure new IP address*/
    memset(v4_addr, 0, INET_ADDRSTRLEN);
    memset(v4_netmask, 0, INET_ADDRSTRLEN);

    if (!inet_ntop(AF_INET, &(addr_info_ptr->ipv4.if_addr.addr.v4), v4_addr, INET_ADDRSTRLEN))
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req: Cannot cast ipv4 address:");
      perror(ifname);
      return NETMGR_FAILURE;
    }
    if (!inet_ntop(AF_INET, &(addr_info_ptr->ipv4.if_mask), v4_netmask, INET_ADDRSTRLEN))
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req: Cannot cast ipv4 netmask:");
      perror(ifname);
      return NETMGR_FAILURE;
    }

    memset(cmd, 0, NETMGR_KIF_MAX_COMMAND_LENGTH);
    snprintf(cmd,
             sizeof(cmd),
             "%s addr add %s/%s dev %s",
             IP_PATH,
             v4_addr,
             v4_netmask,
             ifname);

    if (netmgr_ds_exec(cmd, NULL) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, unable to assign ipv4 address\n", cmd);
      return NETMGR_FAILURE;
    }
    netmgr_log_med("%s() Assigned ipv4 address: %s netmask: %s dev %s",
                   __func__,
                   v4_addr,
                   v4_netmask,
                   ifname);

  }
  else if (NETMGR_IPV6_ADDR == addr_type)
  {
    netmgr_log_med("%s(): IPv6 global address configuration on link=%d\n", __func__, link);
    struct ifreq ifr;
    struct sockaddr_in6 ipv6_addr;
    struct in6_ifreq ifr6;
    int str_len = -1;
    const char *link_name;

    link_name = netmgr_kif_get_name(link);
    if (NULL == link_name)
    {
      netmgr_log_err("%s(): unable to determine name for link=%d\n", __func__, link);
      return NETMGR_FAILURE;
    }

#ifdef FEATURE_DATA_IWLAN
    if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
#endif
    {
      if (is_reconfig == FALSE)
      {
        /* It's sufficient to enable IPv6 only once per call.
         * Subsequent attempts are NOOP */
        int cmd_rc = NETMGR_FAILURE;

        cmd_rc = netmgr_kif_set_ipv6_disabled(link_name, NETMGR_IPV6_ENABLED);
        netmgr_log_high("%s(): Enabling IPv6 on interface [%s] : status [%s]",
                        __func__, link_name, cmd_rc == NETMGR_SUCCESS ? "SUCCESS" : "FAILURE");

      }
      else
      {
        netmgr_log_high("%s(): Skip enabling IPv6 on interface [%s] due to reconfig event",
                        __func__, link_name);
      }
    }

#ifndef FEATURE_DATA_PREFIX_DELEGATION
    if (TRUE == is_reconfig)
    {
      netmgr_log_med("Initiating V6 router solicitation on link=%d\n", link);
      if (NETMGR_SUCCESS != netmgr_kif_send_icmpv6_router_solicitation( link ))
      {
        netmgr_log_err("netmgr_kif_send_icmpv6_router_solicitation failed on link=%d\n", link);
      }
    }
#else
    sockfd = socket(AF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_IP);
    if (sockfd  < 0)
    {
      netmgr_log_err("%s(): socket failed\n", __func__);
      return NETMGR_FAILURE;
    }

    strlcpy(ifr.ifr_name,
            link_name,
            sizeof(ifr.ifr_name));
    ipv6_addr.sin6_family = AF_INET6;
    ipv6_addr.sin6_port = 0;

    memcpy((char *)&ifr6.ifr6_addr,
           (char *)&addr_info_ptr->ipv6.if_addr.addr.v6_addr8,
           sizeof(struct in6_addr));

    if (ioctl(sockfd, SIOGIFINDEX, &ifr) < 0)
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req(): cannot find ifr_ifindex\n");
      perror(ifr.ifr_name);
      result = NETMGR_FAILURE;
      goto bail;
    }

    ifr6.ifr6_ifindex = ifr.ifr_ifindex;
    ifr6.ifr6_prefixlen = addr_info_ptr->ipv6.if_mask;
    netmgr_log_low("%s(): ifr6_ifindex is %d, ifr6_prefixlen is %d\n",
                   __func__,
                   ifr6.ifr6_ifindex,
                   ifr6.ifr6_prefixlen);
    NETMGR_LOG_IPV6_ADDR(med,
                         "netmgr_kif_cfg_req(): ",
                         addr_info_ptr->ipv6.if_addr.addr.v6_addr64);

    if (ioctl(sockfd, SIOCSIFADDR, &ifr6) < 0)
    {
      netmgr_log_sys_err("netmgr_kif_cfg_req(): cannot set ipv6 address\n");
      perror(ifr.ifr_name);
      result = NETMGR_FAILURE;
      goto bail;
    }

bail:
    close(sockfd);
#endif /* FEATURE_DATA_PREFIX_DELEGATION */
  }

done:
  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFCFG_POST, (void *)&ifinfo);
  return result;
}

/*===========================================================================
  FUNCTION  netmgr_kif_open_cnf
===========================================================================*/
/*!
@brief
  Processes confirmation of interface up event.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_open_cnf (int link)
{
  netmgr_kif_state_t state;
  netmgr_exec_cmd_t * cmd_buf = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify that the specified link id is valid */
  if( netmgr_kif_verify_link(link) == NETMGR_FAILURE ) {
    netmgr_log_err("netmgr_kif_open_cnf called with invalid link %d\n", link);
    return;
  }

  /* Process based on current interface state */
  switch (state = netmgr_kif_get_state(link)) {
    case NETMGR_KIF_OPENING:
      /* Interface is in coming up state. Transition to configuring state */
      /* Allocate command object */
      cmd_buf = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd_buf);
      cmd_buf->data.type = NETMGR_KIF_OPENED_EV;
      cmd_buf->data.link = link;

      /* Post command for processing in the command thread context */
      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
        netmgr_exec_release_cmd(cmd_buf);
        NETMGR_ABORT("netmgr_kif_open_cnf: failed to put commmand");
        return;
      }
      break;

    default:
      /* Ignore in all other states. Effectively we are not supporting the
      ** case where the virtual ethernet devices are being externally
      ** controlled too. We assume that DSC has exclusive control over these
      ** devices.
      */
      netmgr_log_err("netmgr_kif_open_cnf called in state %d\n", state);
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

#if (!defined(FEATURE_DATA_PREFIX_DELEGATION) && !defined(FEATURE_DATA_LINUX_LE))
/*===========================================================================
  FUNCTION  netmgr_kif_is_addr_ll_iid_match
===========================================================================*/
/*!
@brief
  Check if the IID of the global address matches that of the link local
  address of the given link

@return
  int - NETMGR_SUCCESS if successful, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_is_addr_ll_iid_match( int link, const netmgr_nl_msg_t * nlmsg_info_ptr )
{
  int ret = NETMGR_SUCCESS;
  const struct sockaddr_storage *sa_ptr = NULL;
  unsigned char *ll_iid_start = NULL, *global_iid_start = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( nlmsg_info_ptr );

  /* Verify that the specified link id is valid */
  if( netmgr_kif_verify_link( link ) == NETMGR_FAILURE ) {
    netmgr_log_err("%s(): called with invalid link %d\n", __func__, link);
    goto bail;
  }

  sa_ptr = &nlmsg_info_ptr->addr_info.attr_info.prefix_addr;

  if (AF_INET6 == sa_ptr->ss_family) {
    netmgr_address_info_t *addr_info_ptr = netmgr_qmi_get_addr_info(link);
    if (!addr_info_ptr)
    {
      netmgr_log_err("%s(): cannot get address info\n", __func__);
      goto bail;
    }

    NETMGR_LOG_IPV6_ADDR( med, "IID match: Link local address ",
                          addr_info_ptr->ipv6.ll_addr.addr.v6_addr64 );

    NETMGR_NL_REPORT_ADDR( med, "IID match: Global address ",
                          (*sa_ptr) );

    /* Check if the IID of the global address matches that of the link local address */
    ll_iid_start = addr_info_ptr->ipv6.ll_addr.addr.v6_addr8+NETMGR_KIF_IPV6_PREFIX_LEN;
    global_iid_start = ((unsigned char *)SASTORAGE_DATA(*sa_ptr))+NETMGR_KIF_IPV6_PREFIX_LEN;

    if (memcmp( ll_iid_start, global_iid_start, NETMGR_KIF_IPV6_IID_LEN )) {
      netmgr_log_med("%s(): Global and link local IIDs didn't match\n", __func__);
      ret = NETMGR_FAILURE;
    }
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_save_ll_addr
===========================================================================*/
/*!
@brief
  Saves the link local address to the address cache

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_save_ll_addr( int link, const netmgr_nl_msg_t * nlmsg_info_ptr )
{
  const struct sockaddr_storage *sa_ptr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( nlmsg_info_ptr );

  /* Verify that the specified link id is valid */
  if( netmgr_kif_verify_link( link ) == NETMGR_FAILURE ) {
    netmgr_log_err("%s(): called with invalid link %d\n", __func__, link);
    goto bail;
  }

  sa_ptr = &nlmsg_info_ptr->addr_info.attr_info.prefix_addr;

  if (AF_INET6 == sa_ptr->ss_family) {
    /* Update address cache for later use */
    netmgr_address_info_t *addr_info_ptr = netmgr_qmi_get_addr_info(link);
    if (!addr_info_ptr)
    {
      netmgr_log_err("%s(): cannot get address info\n", __func__);
      goto bail;;
    }

    /* Update link local cache address */
    memcpy( &addr_info_ptr->ipv6.ll_addr.addr.v6_addr8,
            SASTORAGE_DATA(*sa_ptr),
            sizeof(addr_info_ptr->ipv6.ll_addr.addr.v6_addr8) );

    NETMGR_LOG_IPV6_ADDR( med, "Updated link local address ",
                          addr_info_ptr->ipv6.ll_addr.addr.v6_addr64 );
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
}
#endif /* (!FEATURE_DATA_PREFIX_DELEGATION && !FEATURE_DATA_LINUX_LE) */

/*===========================================================================
  FUNCTION  netmgr_kif_cfg_cnf
===========================================================================*/
/*!
@brief
  Processes confirmation of event indicating successful configuration of
  the given interface.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_cfg_cnf( int link, const netmgr_nl_msg_t * nlmsg_info_ptr )
{
  const netmgr_kif_clntcb_t * clntcb;
  netmgr_kif_state_t state;
  void * clnt_hdl;
  netmgr_exec_cmd_t * cmd_buf = NULL;
  netmgr_sm_events_t  event = NETMGR_INVALID_EV;
  netmgr_address_info_t * addr_info_ptr = NULL;
  netmgr_address_set_t  * addr_ptr = NULL;
  const struct sockaddr_storage *sa_ptr = NULL;
  unsigned int mtu;
  const char *iface_name = NULL;
  struct ifaddrmsg * ifaddr;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( nlmsg_info_ptr );

  /* Verify that the specified link id is valid */
  if( netmgr_kif_verify_link( link ) == NETMGR_FAILURE ) {
    netmgr_log_err("netmgr_kif_cfg_cnf called with invalid link %d\n", link);
    return;
  }

  if (nlmsg_info_ptr->addr_info.attr_info.param_mask & NETMGR_NLA_PARAM_CACHEINFO)
  {
    const struct ifa_cacheinfo *cache_ptr = &nlmsg_info_ptr->addr_info.attr_info.cache_info;

    if (0 == cache_ptr->ifa_prefered || 0 == cache_ptr->ifa_valid)
    {
      netmgr_log_med("netmgr_kif_cfg_cnf: ignoring NEWADDR event with lifetimes "
                     "prefered=%d valid=%d on link=%d\n",
                     cache_ptr->ifa_prefered, cache_ptr->ifa_valid, link);
      return;
    }
  }

  ifaddr = (struct ifaddrmsg*)&nlmsg_info_ptr->addr_info.metainfo;

  sa_ptr = &nlmsg_info_ptr->addr_info.attr_info.prefix_addr;

#if !defined FEATURE_DATA_PREFIX_DELEGATION
  /* Fetch the runtime settings before the address caches get updated with
     the Kernel provided address below */
  if((AF_INET6 == sa_ptr->ss_family)  &&
        netmgr_kif_info[link].dns_v6_queried == FALSE)
  {
    if (netmgr_kif_info[link].dns_v6_queried == FALSE)
    {
      netmgr_log_high("netmgr_kif_cfg_cnf: fetching runtime settings\n");
      netmgr_qmi_get_modem_link_info((uint8)link, WDS_IP_FAMILY_IPV6_V01);
      netmgr_kif_info[link].dns_v6_queried = TRUE;
    }

    /* Fix IPv6 call, set the mtu after getting the modem link info */
    (void)netmgr_kif_set_mtu(link, FALSE);
  }
#endif /* FEATURE_DATA_PREFIX_DELEGATION */

  /* Update address cache for later use */
  addr_info_ptr = netmgr_qmi_get_addr_info(link);
  if (!addr_info_ptr)
  {
    netmgr_log_err("netmgr_kif_cfg_cnf cannot get address info\n");
    return;
  }

  if ( AF_INET == sa_ptr->ss_family )
  {
    /* Compare the incoming netlink address with the cached address and save the
     * address only if they are different */
    if (0 != memcmp(&addr_info_ptr->ipv4.if_addr.addr.v4,
                    SASTORAGE_DATA(*sa_ptr),
                    sizeof(addr_info_ptr->ipv4.if_addr.addr.v4))
        || !netmgr_kif_cfg.link_array[link].v4_qos_rules_enabled)
    {
#ifdef NETMGR_QOS_ENABLED
       /* Flush any existing rules */
       if (netmgr_main_get_qos_enabled())
       {
         netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                   AF_INET,
                   addr_info_ptr,
                   FALSE);
         netmgr_kif_cfg.link_array[link].v4_qos_rules_enabled = FALSE;
       }
#endif

       /* Update cache address */
       memcpy( &addr_info_ptr->ipv4.if_addr.addr.v4,
               SASTORAGE_DATA(*sa_ptr),
               sizeof(addr_info_ptr->ipv4.if_addr.addr.v4) );
       NETMGR_LOG_IPV4_ADDR( med, "updated cached ",
                             addr_info_ptr->ipv4.if_addr.addr.v4 );

#ifdef NETMGR_QOS_ENABLED
       /* Add iptables rules with the new address on the link */
       if (netmgr_main_get_qos_enabled())
       {
         netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                   AF_INET,
                   addr_info_ptr,
                   TRUE);
         netmgr_kif_cfg.link_array[link].v4_qos_rules_enabled = TRUE;
       }
#endif
    }
  }
  else if( AF_INET6 == sa_ptr->ss_family )
  {
    netmgr_log_err("%s():DEBUG: is_empty[%d], is_ll[%d]",
                   __func__,
                   NETMGR_KIF_IPV6_ADDR_EMPTY(addr_info_ptr->ipv6.if_addr.addr),
                   NETMGR_KIF_IS_LINK_LOCAL_ADDR(SASTORAGE_DATA(*sa_ptr)));
    NETMGR_LOG_IPV6_ADDR(med,"DEBUG: is_empty addr ",
                         addr_info_ptr->ipv6.if_addr.addr.v6_addr64);
    NETMGR_NL_REPORT_ADDR(med, "DEBUG: is_ll addr ", (*sa_ptr));

    /* Compare the incoming netlink address with the cached address and save the
     * address only if they are different */
    if ( (NETMGR_KIF_IPV6_ADDR_EMPTY(addr_info_ptr->ipv6.if_addr.addr) ||
           ! NETMGR_KIF_IS_LINK_LOCAL_ADDR(SASTORAGE_DATA(*sa_ptr)) ) &&
         ((0 != memcmp(&addr_info_ptr->ipv6.if_addr.addr.v6_addr8,
                    SASTORAGE_DATA(*sa_ptr),
                    sizeof(addr_info_ptr->ipv6.if_addr.addr.v6_addr8))
          || !netmgr_kif_cfg.link_array[link].v6_qos_rules_enabled))  )
    {
#ifdef NETMGR_QOS_ENABLED
       /* Flush any existing rules */
       if (netmgr_main_get_qos_enabled())
       {
         netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                   AF_INET6,
                   addr_info_ptr,
                   FALSE);
         netmgr_kif_cfg.link_array[link].v6_qos_rules_enabled = FALSE;
       }
#endif

       /* Update the cache */
       memcpy( addr_info_ptr->ipv6.if_addr.addr.v6_addr8,
               SASTORAGE_DATA(*sa_ptr),
               sizeof(addr_info_ptr->ipv6.if_addr.addr.v6_addr8) );
       NETMGR_LOG_IPV6_ADDR( med, "updated cached ",
                             addr_info_ptr->ipv6.if_addr.addr.v6_addr64 );

#ifdef NETMGR_QOS_ENABLED
       /* Add iptables rules with the new address on the link */
       if (netmgr_main_get_qos_enabled())
       {
         netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                   AF_INET6,
                   addr_info_ptr,
                   TRUE);
         netmgr_kif_cfg.link_array[link].v6_qos_rules_enabled = TRUE;
       }
#endif
    }
  }
  else
  {
    netmgr_log_err("netmgr_kif_cfg_cnf unsupported address family[%d]\n",
                   sa_ptr->ss_family );
    return;
  }

  /* Process based on current interface state */
  switch( state = netmgr_kif_get_state( link ) ) {
    case NETMGR_KIF_OPENING:
      /* Interface is in configuring state. Transition to open state */
      netmgr_kif_set_state( link, NETMGR_KIF_OPEN );
      event = NETMGR_KIF_CONFIGURED_EV;

      (void)netmgr_kif_set_mtu(link, FALSE);

      /* Call registered client handler to indicate that the interface has
      ** come up.
      */
      clntcb = netmgr_kif_get_clntcb(link);
      if( clntcb ) {
        clnt_hdl = netmgr_kif_get_clnt_hdl(link);
        (clntcb->opened_cb)((uint8)link, NETMGR_SUCCESS, clnt_hdl);
      }
      netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;
      netmgr_log_med("Debug: Resetting IP Newaddr tracking on link %d\n", link);
      break;

    case NETMGR_KIF_RECONFIGURING:
      /*Move the state to Open only if we get Newaddr for the IP family we were
        expecting. Else, Notify Newaddr to DSI*/
      if(NETMGR_KIF_IS_IP_MATCH(link,ifaddr))
      {
        netmgr_kif_set_state(link, NETMGR_KIF_OPEN);
        event = NETMGR_KIF_RECONFIGURED_EV;

        (void)netmgr_kif_set_mtu(link, FALSE);

        clntcb = netmgr_kif_get_clntcb(link);
        if( clntcb ) {
          clnt_hdl = netmgr_kif_get_clnt_hdl(link);
          (clntcb->reconfigured_cb)((uint8)link, NETMGR_SUCCESS, clnt_hdl);
        }
        netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;
        netmgr_log_med("Debug: Resetting IP Newaddr tracking on link %d\n", link);
      }
      else
      {
        if( NETMGR_SUCCESS != netmgr_kif_nl_post_addr_msg( link, nlmsg_info_ptr ) )
        {
          netmgr_log_err("netmgr_kif_cfg_cnf failed on posting address event for link %d\n", link);
          return;
        }
      }
      break;

    case NETMGR_KIF_OPEN:
      /* Post event to NetLink clients for local processing */
      if( NETMGR_SUCCESS != netmgr_kif_nl_post_addr_msg( link, nlmsg_info_ptr ) ) {
        netmgr_log_err("netmgr_kif_cfg_cnf failed on posting address event for link %d\n", link);
        return;
      }

      /* No event to executive in this case */
      break;

    default:
      /* Ignore in all other states */
      netmgr_log_high("netmgr_kif_cfg_cnf called in state %d, ignoring!\n", state);
      break;
  }

  /* Check for successful indication processing */
  if( NETMGR_INVALID_EV != event ) {

    /* Query interface address into reported from QMI */
    addr_info_ptr = netmgr_qmi_get_addr_info( link );
    if( !addr_info_ptr ) {
      netmgr_log_err("netmgr_kif_cfg_cnf cannot get address info\n");
      return;
    }
    /* Assign address set for property assignment, giving IPV4 precedence for legacy support. */
    if( NETMGR_ADDRSET_MASK_INVALID == addr_info_ptr->valid_mask )
    {
      netmgr_log_err("netmgr_kif_cfg_cnf cannot get valid address info\n");
      return;
    }
    addr_ptr = (NETMGR_ADDRSET_MASK_IPV4 & addr_info_ptr->valid_mask)?
               &addr_info_ptr->ipv4 : &addr_info_ptr->ipv6;

    /*-------------------------------------------------------------------------
      Post message to executive
    -------------------------------------------------------------------------*/
    cmd_buf = netmgr_exec_get_cmd();
    NETMGR_ASSERT(cmd_buf);

    cmd_buf->data.type = event;
    cmd_buf->data.link = link;
    cmd_buf->data.info.connect_msg.nlmsg_info = *nlmsg_info_ptr;
    cmd_buf->data.info.connect_msg.addr_info_ptr = addr_ptr;
    cmd_buf->data.info.connect_msg.reconfig_required = FALSE;

    /* Post command for processing in the command thread context */
    if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
      netmgr_exec_release_cmd(cmd_buf);
      NETMGR_ABORT("failed to put commmand");
    }
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_cfg_fail
===========================================================================*/
/*!
@brief
  Processes event indicating unsuccessful configuration of given interface.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_cfg_fail (int link, const netmgr_nl_msg_t * nlmsg_info_ptr)
{
  netmgr_kif_state_t state;
  const struct sockaddr_storage *sa_ptr = NULL;
  netmgr_address_info_t *addr_info_ptr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify that the specified link id is valid */
  if (netmgr_kif_verify_link(link) == NETMGR_FAILURE) {
    netmgr_log_err("netmgr_kif_cfg_fail called with invalid link %d\n", link);
    return;
  }

  /* Process based on current interface state */
  switch (state = netmgr_kif_get_state(link)) {
    case NETMGR_KIF_OPEN:
    case NETMGR_KIF_RECONFIGURING:
      /* Update address cache for later use */
      addr_info_ptr = netmgr_qmi_get_addr_info( link );
      if( !addr_info_ptr ) {
        netmgr_log_err("netmgr_kif_cfg_fail cannot get address info\n");
        return;
      }

#ifdef NETMGR_QOS_ENABLED
      /* Flush the iptables created on RTM_DELADDR */
      if (netmgr_main_get_qos_enabled())
      {
        netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                   SASTORAGE_FAMILY(nlmsg_info_ptr->addr_info.attr_info.prefix_addr),
                   addr_info_ptr,
                   FALSE);
        /* Reset QoS flag */
        if (AF_INET == SASTORAGE_FAMILY(nlmsg_info_ptr->addr_info.attr_info.prefix_addr))
        {
          netmgr_kif_cfg.link_array[link].v4_qos_rules_enabled = FALSE;
        }
        else if (AF_INET6 == SASTORAGE_FAMILY(nlmsg_info_ptr->addr_info.attr_info.prefix_addr))
        {
          netmgr_kif_cfg.link_array[link].v6_qos_rules_enabled = FALSE;
        }
      }
#endif

      sa_ptr = &nlmsg_info_ptr->addr_info.attr_info.prefix_addr;
      if( AF_INET == sa_ptr->ss_family )
      {
        memcpy( &addr_info_ptr->ipv4.if_addr.addr.v4,
                SASTORAGE_DATA(*sa_ptr),
                sizeof(addr_info_ptr->ipv4.if_addr.addr.v4) );
        NETMGR_LOG_IPV4_ADDR( med, "updated cached ",
                              addr_info_ptr->ipv4.if_addr.addr.v4 );
      }
      else if( AF_INET6 == sa_ptr->ss_family )
      {
        memcpy( addr_info_ptr->ipv6.if_addr.addr.v6_addr8,
                SASTORAGE_DATA(*sa_ptr),
                sizeof(addr_info_ptr->ipv6.if_addr.addr.v6_addr8) );
        NETMGR_LOG_IPV6_ADDR( med, "updated cached ",
                              addr_info_ptr->ipv6.if_addr.addr.v6_addr64 );
      }
      else
      {
        netmgr_log_err("netmgr_kif_cfg_fail unsupported address family[%d]\n",
                       sa_ptr->ss_family );
        return;
      }

      /* Post event to NetLink clients for local processing */
      if( NETMGR_SUCCESS != netmgr_kif_nl_post_addr_msg( link, nlmsg_info_ptr ) ) {
        netmgr_log_err("netmgr_kif_cfg_fail failed on posting address event for link %d\n", link);
        return;
      }

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
      /* For dual-ip calls if only one address family goes down the main link state machine
         will remain in UP state. Only the address will get deleted. We need to ensure
         that wakelock is properly released at this point */
      netmgr_util_wakelock_release();
#endif
      break;

    case NETMGR_KIF_CLOSING:
        netmgr_log_med("netmgr_kif_cfg_fail called in state %d\n", state);
#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
      /* There is a possible race condition during dual-ip call teardown where WDS_DISCONNECTED
         events are processed back-to-back in quick succession. The second WDS_DISCONNECTED event
         would move the kif state machine to CLOSING state. We need to ensure that wakelock is
         released in this state */
      netmgr_util_wakelock_release();
#endif
      break;

    default:
      /* Ignore in all other states */
      netmgr_log_err("netmgr_kif_cfg_fail called in state %d\n", state);
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_close_cnf
===========================================================================*/
/*!
@brief
  Processes event indicating the given interface has been successfully
  closed.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_close_cnf (int link)
{
  netmgr_kif_state_t state;
  const netmgr_kif_clntcb_t * clntcb;
  void * clnt_hdl;
  netmgr_exec_cmd_t * cmd_buf = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify that the specified link id is valid */
  if (netmgr_kif_verify_link(link) == NETMGR_FAILURE) {
    netmgr_log_err("netmgr_kif_close_cnf called with invalid link %d\n", link);
    return;
  }

  /* Process based on current interface state */
  switch (state = netmgr_kif_get_state(link)) {
    case NETMGR_KIF_RECONFIGURING:
      /* Intentional fall through */
    case NETMGR_KIF_OPEN:
      /* intentional fall through */
    case NETMGR_KIF_OPENING:
    case NETMGR_KIF_CLOSING:
      /* The device has gone down. Transition to closed state and call
      ** registered client handler to notify of this event.
      */
      netmgr_kif_set_state(link, NETMGR_KIF_CLOSED);

      netmgr_kif_info[link].dns_v6_queried = FALSE;
      netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;

      /* Purge address(es) from network interface */
      if( NETMGR_SUCCESS != netmgr_kif_clear_iface( link ) )
      {
        netmgr_log_err("failed to clear net iface on link %d\n", link);
      }

      clntcb = netmgr_kif_get_clntcb(link);
      if( clntcb ) {
        clnt_hdl = netmgr_kif_get_clnt_hdl(link);
        netmgr_kif_set_clntcb(link, NULL);
        netmgr_kif_set_clnt_hdl(link, NULL);
        (clntcb->closed_cb)((uint8)link, NETMGR_SUCCESS, clnt_hdl);
      }

      /* Allocate command object */
      cmd_buf = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd_buf);
      cmd_buf->data.type = NETMGR_KIF_CLOSED_EV;
      cmd_buf->data.link = link;

      /* Post command for processing in the command thread context */
      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
        NETMGR_ABORT("netmgr_kif_close_cnf: failed to put commmand\n");
        netmgr_exec_release_cmd(cmd_buf);
        return;
      }
      break;

    default:
      /* Ignore in all other states */
      netmgr_log_err("netmgr_kif_close_cnf called in state %d\n", state);
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_init_names
===========================================================================*/
/*!
@brief
  Helper function for initializing all device names in the module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_init_names( void )
{
  unsigned int i;
  const char * prefix = NULL;
  unsigned int offset;

  /* Iterate over the array of interfaces, initializing device name for
  ** each. The device name prefix is suffixed with integer values starting
  ** from '0' for the first one and incrementing by one.
  */
  for (i = 0; i < NETMGR_MAX_LINK; i++) {
#ifdef FEATURE_DATA_IWLAN
    int modem;
    netmgr_main_link_type link_type;

    if (NETMGR_SUCCESS != netmgr_kif_get_modem_link_type((int)i, &modem, &link_type)) {
      netmgr_log_high("netmgr_kif_init_names: failed to get modem/link_type for link=%d\n",
                      i);
      return;
    }

    /* Lookup modem device configuration */
    prefix = netmgr_main_dev_prefix_tbl[ modem ][ link_type ].prefix;
    offset = netmgr_main_dev_prefix_tbl[ modem ][ link_type ].inst_min;
#else
    /* Lookup modem device configuration */
    prefix = netmgr_main_dev_prefix_tbl[ (i/NETMGR_KIF_DEV_PER_MODEM) ].prefix;
    offset = netmgr_main_dev_prefix_tbl[ (i/NETMGR_KIF_DEV_PER_MODEM) ].inst_min;
#endif /* FEATURE_DATA_IWLAN */

    /* Make sure prefix is not null or has valid length, otherwise use the
    ** statically defined default device name.
    */
    if ((prefix == NULL) ||
        (std_strlen(prefix) > NETMGR_KIF_NAME_PR_MAX_LEN))
    {
      prefix = NETMGR_KIF_DEF_NAME;
    }

    (void)snprintf( netmgr_kif_info[i].name,
                    sizeof(netmgr_kif_info[i].name),
                    "%s%d", prefix, (i-offset) );
    netmgr_log_high("netmgr_kif named link %d to %s\n",
                    i, netmgr_kif_info[i].name);

  }
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_init_paths
===========================================================================*/
/*!
@brief
  Helper function for initializing all file pathnames in the module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_init_paths
(
    const char * dirpath,
    const char * modscript
)
{
  /* If a directory name was passed on the command line, use it as a prefix
  ** to construct the path name of the module load script.
  */
  if (dirpath) {
    (void)strlcpy( netmgr_kif_cfg.dirpath,
                       dirpath,
                       NETMGR_KIF_FILENAME_MAX_LEN );

    (void)strlcpy( netmgr_kif_cfg.modscript,
                       netmgr_kif_cfg.dirpath,
                       NETMGR_KIF_FILENAME_MAX_LEN );
  }

  /* Set module load script name in configuration blob for later use, if one
  ** was specified on the command line.
  */
  if (modscript) {
    (void)strlcat( netmgr_kif_cfg.modscript,
                       modscript,
                       NETMGR_KIF_FILENAME_MAX_LEN );
  }

  return;
}


/*===========================================================================
  FUNCTION  netmgr_kif_init_link_ifindex
===========================================================================*/
/*!
@brief
  Helper function to initializes the specified link device index.

@return
  int - NETMGR_SUCCESS if successful, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_init_link_ifindex( int link )
{
  struct ifreq ifr;

  /* no need to init ifindex for disabled interface*/
  if (netmgr_kif_cfg.link_array[link].enabled == FALSE) {
    return NETMGR_SUCCESS;
  }

  /* Call ioctl on device to get the if index */
  memset( &ifr, 0, sizeof(struct ifreq) );
  if( NETMGR_SUCCESS !=
      netmgr_kif_call_ioctl_on_dev( netmgr_kif_info[link].name,
                                    SIOCGIFINDEX,
                                    &ifr ) )
  {
    netmgr_log_err("Cannot get ifindex for dev %s!\n",
                   netmgr_kif_info[link].name);
    return NETMGR_FAILURE;
  }

  /* Save if index in the interface info struct */
  netmgr_kif_info[link].ifi_index = ifr.ifr_ifindex;

  netmgr_log_high("netmgr_kif link %d, device %s has ifindex %d\n",
                  link, netmgr_kif_info[link].name, ifr.ifr_ifindex);
  return NETMGR_SUCCESS;
}


/*===========================================================================
  FUNCTION  netmgr_kif_get_link_for_ifindex
===========================================================================*/
/*!
@brief
  Returns the link ID for a specified system device index.

@return
  int - device index if index is valid and recognized, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_link_for_ifindex (uint32 ifindex)
{
  int link = -1;
  int i;

  /* Iterate over the array of interfaces, and return the link id for
  ** the interface matching the specified if index.
  */
  for( i = 0; i < NETMGR_MAX_LINK; i++ ) {
    if( netmgr_kif_cfg.link_array[i].enabled && ((uint32)netmgr_kif_info[i].ifi_index == ifindex) ) {
      link = i;
      break;
    }
  }

  return link;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_name_ifindex
===========================================================================*/
/*!
@brief
  Accessor for getting device name for a given ifindex.

@return
  char* - Pointer to device name

@note

  - Dependencies
    - None

    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_ifioctl_get_ifname(int ifindex, char* dev)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_ifindex = ifindex;

  if( ioctl(fd, SIOCGIFNAME , &ifr) < 0 ) {
    netmgr_log_err("ifioctl_get: SIOCGIFNAME ioctl failed:\n");
    close(fd);
    goto error;
  }

  (void)strlcpy(dev, ifr.ifr_name, NETMGR_IF_NAME_MAX_LEN );

  close(fd);
  rval = NETMGR_SUCCESS;

 error:
  return rval;
}


/*===========================================================================
  FUNCTION  netmgr_kif_load_module
===========================================================================*/
/*!
@brief
  Loads the module providing implementation of virtual Ethernet interfaces.

@return
  int - 0 if module is successfully loaded, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_load_module (void)
{
  char scr_buf[NETMGR_KIF_FILENAME_MAX_LEN];

  NETMGR_LOG_FUNC_ENTRY;

  /* Construct command to load the module. Use module script name if one
  ** was specified on the command line, otherwise use modprobe to load
  ** module assuming the module name is derived from the interface name.
  */
  if (std_strlen(netmgr_kif_cfg.modscript) == 0) {
    (void)snprintf( scr_buf,
                    NETMGR_KIF_FILENAME_MAX_LEN,
                    "modprobe %s",
                    netmgr_kif_cfg.name );
    netmgr_log_high("Loading module %s\n", netmgr_kif_cfg.name);
  } else {
    (void)snprintf( scr_buf,
                    NETMGR_KIF_FILENAME_MAX_LEN,
                    "%s",
                    netmgr_kif_cfg.modscript );
    netmgr_log_high( "Running module load script %s\n",
                     netmgr_kif_cfg.modscript );
  }

  /* Issue command to load module */
  if (netmgr_ds_exec(scr_buf, NULL) != DS_EXEC_OK) {
    return -1;
  }

  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  netmgr_kif_is_ifindex_wlan
===========================================================================*/
/*!
@brief
  Checks if the given ifindex is that of wlan0 interface

@return
  TRUE if the give ifindex is that of wlan0 interface
  FALSE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL boolean
netmgr_kif_is_ifindex_wlan(uint32 ifindex)
{
  struct ifreq ifr;
  boolean ret = FALSE;

  /* Call ioctl on device to get the if index */
  memset( &ifr, 0, sizeof(struct ifreq) );
  if( NETMGR_SUCCESS !=
      netmgr_kif_call_ioctl_on_dev( "wlan0",
                                    SIOCGIFINDEX,
                                    &ifr ) )
  {
    netmgr_log_err("%s(): Cannot get ifindex for dev wlan0!\n",
                   __func__);
    goto bail;
  }

  /* Save if index in the interface info struct */
  ret = (ifindex == (uint32) ifr.ifr_ifindex) ? TRUE : FALSE;

bail:
  netmgr_log_high("%s(): input ifindex=%d, wlan0 ifindex=%d\n",
                  __func__, ifindex, ifr.ifr_ifindex);
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_process_wlan_up_event
===========================================================================*/
/*!
@brief
  Post NETMGR_WLAN_OPENED_EV on all active local breakout and iWLAN links
  when wlan0 interface comes up

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_process_wlan_up_event(void)
{
  netmgr_exec_cmd_t * cmd_buf = NULL;
  wds_technology_name_enum_v01 tech;
  int i;

  NETMGR_LOG_FUNC_ENTRY;

  for (i = 0; i < NETMGR_MAX_LINK; ++i) {
    if (!NETMGR_KIF_IS_REV_RMNET_LINK(i)) {
      continue;
    }

    tech = netmgr_qmi_wds_get_tech_name(i);

    if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech ||
        WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == tech) {

      netmgr_log_med("%s(): posting WLAN opened event on link=%d\n",
                     __func__,
                     i);

      cmd_buf = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd_buf);
      cmd_buf->data.type = NETMGR_WLAN_OPENED_EV;
      cmd_buf->data.link = i;

      /* Post command for processing in the command thread context */
      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
        netmgr_exec_release_cmd(cmd_buf);
        NETMGR_ABORT("%s(): failed to put commmand", __func__);
        return;
      }
    }
  }

  NETMGR_LOG_FUNC_EXIT;
}
/*===========================================================================
  FUNCTION  netmgr_kif_process_wlan_down_event
===========================================================================*/
/*!
@brief
  Post NETMGR_WLAN_CLOSED_EV on all active local breakout and iWLAN links
  when wlan0 interface goes down

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_process_wlan_down_event(void)
{
  netmgr_exec_cmd_t * cmd_buf = NULL;
  wds_technology_name_enum_v01 tech;
  int i;

  NETMGR_LOG_FUNC_ENTRY;

  for (i = 0; i < NETMGR_MAX_LINK; ++i) {
    if (!NETMGR_KIF_IS_REV_RMNET_LINK(i)) {
      continue;
    }

    tech = netmgr_qmi_wds_get_tech_name(i);

    if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech ||
        WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == tech) {

      netmgr_log_med("%s(): posting WLAN closed event on link=%d\n",
                     __func__,
                     i);

      cmd_buf = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd_buf);
      cmd_buf->data.type = NETMGR_WLAN_CLOSED_EV;
      cmd_buf->data.link = i;

      /* Post command for processing in the command thread context */
      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
        netmgr_exec_release_cmd(cmd_buf);
        NETMGR_ABORT("%s(): failed to put commmand", __func__);
        return;
      }
    }
  }

  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_kif_nl_recv_link_msg
===========================================================================*/
/*!
@brief
  Processes incoming NETLINK_ROUTE messages related to link/interface state.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_nl_recv_link_msg (const netmgr_nl_msg_t  * nlmsg_info_ptr)
{
  struct ifinfomsg * ifinfo;
  int link = NETMGR_LINK_NONE;
  boolean is_ifindex_wlan = FALSE;
  char link_name[NETMGR_IF_NAME_MAX_LEN];
  boolean is_gro_update_req = FALSE;
  boolean iface_change = IF_DOWN;

  NETMGR_ASSERT( nlmsg_info_ptr );

  NETMGR_LOG_FUNC_ENTRY;

  if( nlmsg_info_ptr->type == RTM_NEWLINK ) {
    netmgr_log_high("Received RTM_NEWLINK\n");
  } else if( nlmsg_info_ptr->type == RTM_DELLINK ) {
    netmgr_log_high("Received RTM_DELLINK\n");
  } else {
    netmgr_log_err("Unknown Netlink msg type LINK [%d], ignoring!\n",
                    nlmsg_info_ptr->type);
    goto ret;
  }

  /* For a LINK message, data is a ptr to struct ifinfomsg type */
  if( NETMGR_NL_PARAM_LINK & nlmsg_info_ptr->param_mask ) {
    ifinfo = (struct ifinfomsg*)&nlmsg_info_ptr->link_info.metainfo;
  } else {
    netmgr_log_err("Netlink msg missing LINK parmeters [%d], ignoring!\n",
                    nlmsg_info_ptr->type);
    goto ret;
  }

  /* Get link id for the device using the if index indicated in the msg */
  if( ((link = netmgr_kif_get_link_for_ifindex((uint32)ifinfo->ifi_index)) < 0 ) &&
       !(is_ifindex_wlan = netmgr_kif_is_ifindex_wlan(ifinfo->ifi_index))) {
    /* Could not get link id. This is probably an interface that we don't
    ** care about. */

    /* Querying for the interface name using ioctl.*/
    if( NETMGR_FAILURE ==
        netmgr_kif_ifioctl_get_ifname(ifinfo->ifi_index, link_name) ) {
      netmgr_log_err("%s(): Not able to get ifname from ifindex %d",
                        __func__, ifinfo->ifi_index);
      goto ret;
    }
     /* Check if inetrface is getting UP or DOWN */
    if( (ifinfo->ifi_change & IFF_UP) && (ifinfo->ifi_flags & IFF_UP) &&
        (nlmsg_info_ptr->type == RTM_NEWLINK) ) {
      /*Interface Up*/
      iface_change = IF_UP;
    }
    else if( ( (ifinfo->ifi_change & IFF_UP) && (ifinfo->ifi_flags & ~IFF_UP) ) ||
              (nlmsg_info_ptr->type == RTM_DELLINK) ) {
      /*
        It might be possible that DELLINK Event come with or without ifi_change and ifi_flags change
      */
      /*Interface Down*/
      iface_change = IF_DOWN;
    }
    else {
     netmgr_log_err("unrecognized ifindex %d, disable link\n",
                     ifinfo->ifi_index);
     goto ret;
    }

    netmgr_log_low("%s(): iface_changes %d interface %s \n", __func__,
                   iface_change, link_name);

    if( ( iface_change != clat_iface_st ) &&
          !strncmp (link_name,NETMGR_KIF_CLAT_INTF_PREFIX,strlen (NETMGR_KIF_CLAT_INTF_PREFIX)) ) {
          clat_iface_st = iface_change;
          is_gro_update_req = TRUE;
      /* Updating rps mask for clat interface */
      if( iface_change && netmgr_main_cfg.clat_rps_mask > 0 ) {
        clat_iface_indx = ifinfo->ifi_index;
        netmgr_log_high("%s(): Setting RPS on clat interface%s to %d\n", __func__,
                         link_name, netmgr_main_cfg.clat_rps_mask);
        if ( NETMGR_SUCCESS !=
             netmgr_kif_set_rps_mask( link_name,netmgr_main_cfg.clat_rps_mask) ) {
             netmgr_log_err("%s(): Failed to set RPS mask for dev %s", __func__,
                          link_name);
        }
      }
    }
    else if( ( iface_change != softap_iface_st ) &&
            ( !strncmp (link_name,NETMGR_KIF_TETHERING_INTF_SOFTAP,strlen (NETMGR_KIF_TETHERING_INTF_SOFTAP)) ) ) {
            softap_iface_st = iface_change;
            is_gro_update_req = TRUE;
    }
    else if( ( iface_change != rndis_iface_st ) &&
             ( !strncmp (link_name,NETMGR_KIF_TETHERING_INTF_RNDIS,strlen (NETMGR_KIF_TETHERING_INTF_RNDIS)) ) ) {
             rndis_iface_st = iface_change;
             is_gro_update_req = TRUE;
    }
    else {
     netmgr_log_err("unrecognized ifindex %d, disable link\n",
                      ifinfo->ifi_index);
     goto ret;
    }

    netmgr_log_low(" %s(): is_gro_update_req %d clat_iface_st %d softap_iface_st %d rndis_iface_st %d\n",
                    __func__, is_gro_update_req, clat_iface_st, softap_iface_st, rndis_iface_st);

    if( is_gro_update_req && (clat_iface_indx >= 0) &&  netmgr_main_cfg.data_format.dl_gro_enabled ) {

      char *upstream_link_name = NULL;
      int value=GRO_ENABLE;
      memset(link_name, 0, NETMGR_IF_NAME_MAX_LEN);

      if( NETMGR_FAILURE ==  netmgr_kif_ifioctl_get_ifname(clat_iface_indx, link_name) ) {
        netmgr_log_err("%s(): Not able to get ifname from clat_ifi_indx %d",__func__,
                        clat_iface_indx);
        goto ret;
      }

      /* Getting upstream_link_name from Clat intareface name */
      upstream_link_name = link_name + strlen(NETMGR_KIF_CLAT_INTF_PREFIX);

      if( clat_iface_st && ( softap_iface_st || rndis_iface_st ) ) {
        value=GRO_DISABLE;
      }
      else {
        value=GRO_ENABLE;
      }

      netmgr_log_low(" %s(): Update GRO value %d upstream_link_name %s", __func__,
                      value, upstream_link_name);
      if ( NETMGR_SUCCESS != netmgr_kif_configure_dl_gro_feature(upstream_link_name,value) ) {
        netmgr_log_err("%s(): Failed to disable DL GRO feature for dev %s " , __func__, upstream_link_name);
      }
    }

    netmgr_log_err("unrecognized ifindex %d, disable link\n",
                     ifinfo->ifi_index);
    goto ret;
  }

  /* Process message if there is a change in the state of the interface */
  if( ifinfo->ifi_change & IFF_UP ) {
    if( ifinfo->ifi_flags & IFF_UP ) {
      /* Interface came up. Process event based on current state */
      if (is_ifindex_wlan) {
        netmgr_log_high("wlan0 interface up\n");
        netmgr_kif_process_wlan_up_event();
      }
      else {
        netmgr_log_high("link %d interface up\n", link);
        netmgr_kif_open_cnf(link);
      }

      /* Print some debug messages */
      if( nlmsg_info_ptr->type == RTM_NEWLINK ) {
        netmgr_log_high("RTM_NEWLINK rcvd with interface up\n");
      } else {
        netmgr_log_high("not RTM_NEWLINK!\n");
      }
    }
    else {
      /* Interface went down. Process event based on current state */
      if (is_ifindex_wlan) {
        netmgr_log_high("wlan0 interface down\n");
        netmgr_kif_process_wlan_down_event();
      }
      else {
        netmgr_log_high("link %d interface down\n", link);
        netmgr_kif_close_cnf(link);
      }

      /* Print some debug messages */
      if( nlmsg_info_ptr->type == RTM_DELLINK ) {
        netmgr_log_high("RTM_DELLINK rcvd with interface down\n");
      } else {
        netmgr_log_high("not RTM_DELLINK!\n");
      }
    }
  }
  else {
    netmgr_log_high("not IFF_UP change, ignoring\n");
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_nl_recv_addr_msg
===========================================================================*/
/*!
@brief
  Processes incoming NETLINK_ROUTE messages related to address configuration.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_nl_recv_addr_msg( const netmgr_nl_msg_t * nlmsg_info_ptr )
{
  struct ifaddrmsg * ifaddr;
  int link;

  NETMGR_ASSERT( nlmsg_info_ptr );

  NETMGR_LOG_FUNC_ENTRY;

  /* For a ADDR message, data is a ptr to struct ifaddrmsg type */
  if( NETMGR_NL_PARAM_ADDR & nlmsg_info_ptr->param_mask ) {
    ifaddr = (struct ifaddrmsg*)&nlmsg_info_ptr->addr_info.metainfo;
  } else {
    netmgr_log_err("Netlink msg missing ADDR parmeters [%d], ignoring!\n",
                   nlmsg_info_ptr->type);
    goto ret;
  }

  /* Get link id for the device using the if index indicated in the msg */
  if( (link = netmgr_kif_get_link_for_ifindex( ifaddr->ifa_index ) ) < 0) {
    /* Could not get link id. This is probably an interface that we don't
    ** care about. Ignore message.
    */
    netmgr_log_err("unrecognized ifindex %d\n", ifaddr->ifa_index);
    goto ret;
  }

  /* Process based on message type */
  if( nlmsg_info_ptr->type == RTM_NEWADDR ) {
    /* A new address was configured on the interface. Process based on
    ** the current state of the interface.
    */

    if (WDS_TECHNOLOGY_NAME_MODEM_LINK_LOCAL_V01 == netmgr_qmi_wds_get_tech_name(link))
    /* For MODEM_LINK_LOCAL tech, there is no global address assigned, we follow
    ** same condition as SoftAP V6
    */
    {
      if ((ifaddr->ifa_family == AF_INET6) &&
            (ifaddr->ifa_scope == NETMGR_KIF_LOCAL_SCOPE))
      {
        netmgr_log_high("Link local address configured on link:%d\n", link);
        netmgr_kif_cfg_cnf( link, nlmsg_info_ptr );
      }
    }

#if (defined(FEATURE_DATA_PREFIX_DELEGATION) || defined(FEATURE_DATA_LINUX_LE) || defined(NETMGR_OFFTARGET))
    /* Check that the scope is global; ignore local adddresses */
    if( NETMGR_KIF_GLOBAL_SCOPE == ifaddr->ifa_scope ) {
      netmgr_log_high("address configuration update on link %d\n", link);
      netmgr_kif_cfg_cnf( link, nlmsg_info_ptr );
    }
#else
    /* Check that the scope is global; ignore local adddresses */
    if( NETMGR_KIF_GLOBAL_SCOPE == ifaddr->ifa_scope )
    {
      if (NETMGR_KIF_IS_REV_RMNET_LINK(link) ||
          NETMGR_SUCCESS == netmgr_kif_is_addr_ll_iid_match(link, nlmsg_info_ptr))
       {

         if(netmgr_kif_get_state(link) == NETMGR_KIF_OPENING)
         {
           netmgr_log_high("Debug: Link [%d] in state [%d] waiting for IP type [%d] Newaddr",link,
                           netmgr_kif_get_state(link),netmgr_kif_info[link].reconfig_ip_type);

           if( !NETMGR_KIF_IS_IP_MATCH(link,ifaddr))
           {
             netmgr_log_high("Unexpected IP address [%d] Newaddr received for link [%d]",
                                 ifaddr->ifa_family,link);
             goto ret;
           }
         }

        netmgr_log_high("address configuration update on link %d\n", link);
        netmgr_kif_cfg_cnf( link, nlmsg_info_ptr );
      }
    }
    else if (NETMGR_KIF_LOCAL_SCOPE == ifaddr->ifa_scope) {
      netmgr_kif_save_ll_addr( link, nlmsg_info_ptr );
    }
#endif
  }
  else if( nlmsg_info_ptr->type == RTM_DELADDR ) {
    /* An IP address was deleted from the interface. Process based on the
    ** current state of the interface.
    */

#if (defined(FEATURE_DATA_PREFIX_DELEGATION) || defined(FEATURE_DATA_LINUX_LE) || defined(NETMGR_OFFTARGET))
    /* Check that the scope is global; ignore local adddresses */
    if( NETMGR_KIF_GLOBAL_SCOPE == ifaddr->ifa_scope ) {
      netmgr_log_high("address configuration release on link %d\n", link);
      netmgr_kif_cfg_fail( link, nlmsg_info_ptr );
    }
#else
    /* Check that the scope is global; ignore local adddresses */
    if( NETMGR_KIF_GLOBAL_SCOPE == ifaddr->ifa_scope ) {
      if (NETMGR_KIF_IS_REV_RMNET_LINK(link) ||
          NETMGR_SUCCESS == netmgr_kif_is_addr_ll_iid_match(link, nlmsg_info_ptr)) {
        netmgr_log_high("address configuration release on link %d\n", link);
        netmgr_kif_cfg_fail( link, nlmsg_info_ptr );
      }
    }
#endif
    else
    {
      netmgr_log_low("not global address, ignoring on link %d\n", link);
    }
  }
  else {
    netmgr_log_err("Unknown Netlink msg type [%d], ignoring link %d!\n",
                   nlmsg_info_ptr->type, link);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_nl_recv_xfrm_newae_msg
===========================================================================*/
/*!
@brief
  Processes incoming XFRM_MSG_NEWAE messages

@return
  void

@note

  - Dependencies
    - None
  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_nl_recv_xfrm_newae_msg (const netmgr_nl_msg_t  * nlmsg_info_ptr)
{
  int               result = NETMGR_FAILURE, reti = NETMGR_FAILURE;
  int               i = 0;
  unsigned long     spi = 0, seq_num = 0;
  netmgr_address_info_t  *addr_info_ptr = NULL;

  NETMGR_ASSERT( nlmsg_info_ptr );

  NETMGR_LOG_FUNC_ENTRY;
  do
  {
    if (NETMGR_XFRM_PARAM_ESPTHRESH & nlmsg_info_ptr->param_mask)
    {
      spi     = htonl(nlmsg_info_ptr->xfrm_info.xfrm_aevent.sa_id.spi);
      seq_num = htonl(nlmsg_info_ptr->xfrm_info.xfrm_replay.seq);
      netmgr_log_low("SPI: 0x%08lx", spi);
      netmgr_log_low("Seq number: 0x%08lx", seq_num);

      for (i = 0; i < NETMGR_MAX_LINK; i++)
      {
        if (NETMGR_KIF_IS_REV_RMNET_LINK(i))
        {
          /* Check the SPI with the one obtained from the netlink message */
          addr_info_ptr = netmgr_qmi_get_addr_info(i);

          if (NULL == addr_info_ptr)
          {
            netmgr_log_err("Failed to obtain address information for link [%d]",
                           i);
            /* Some rev_rmnet ports maybe inactive on certain devices
             * Continue with other ports if getting addr info fails */
            continue;
          }

          if (spi != addr_info_ptr->ipv4.sa.esp_spi_v4
              && spi != addr_info_ptr->ipv6.sa.esp_spi_v6)
          {
            /* We need to continue searching all rev_rmnet ports */
            continue;
          }
          else if (addr_info_ptr->ipv4.sa.esp_spi_v4 == spi)
          {
            netmgr_log_low("ESP SPI v4: 0x%08x link [%d]",
                           (unsigned int)addr_info_ptr->ipv4.sa.esp_spi_v4, i);
            reti = netmgr_qmi_initiate_esp_rekey(i, NETMGR_QMI_CLIENT_IPV4);
            break;
          }
          else if (addr_info_ptr->ipv6.sa.esp_spi_v6 == spi)
          {
            netmgr_log_low("ESP SPI v6: 0x%08x link [%d]",
                           (unsigned int)addr_info_ptr->ipv4.sa.esp_spi_v6, i);
            reti = netmgr_qmi_initiate_esp_rekey(i, NETMGR_QMI_CLIENT_IPV6);
            break;
          }
        }
      }
    }

    if (reti == NETMGR_FAILURE)
    {
      break;
    }

    result = NETMGR_SUCCESS;
  } while(0);

  if (NETMGR_SUCCESS != result)
  {
    netmgr_log_err("ESP rekey failed!");
  }
  else
  {
    netmgr_log_med("ESP rekey success!");
  }
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_kif_nl_msg_recv_cmd_exec
===========================================================================*/
/*!
@brief
  Virtual function called by the Command Thread to execute KIF command
  to process a received NETLINK message.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_nl_msg_recv_cmd_exec (ds_cmd_t * cmd, void * data)
{
  struct msghdr * msgh;
  struct nlmsghdr * nlmsgh;
  netmgr_nl_msg_t   nlmsg_info;
  (void)cmd;

  NETMGR_LOG_FUNC_ENTRY;

  /* Get netlink message ptr from the message header */
  msgh = (struct msghdr *)((netmgr_msg_t*)data)->msg;
  nlmsgh = (struct nlmsghdr *)(msgh->msg_iov->iov_base);

  /* Decode the message in structure */
  memset( &nlmsg_info, 0x0, sizeof(nlmsg_info) );
  if( NETMGR_SUCCESS !=
      netmgr_nl_decode_nlmsg( (char*)nlmsgh,
                              ((netmgr_msg_t*)data)->msglen, &nlmsg_info ) ) {
    netmgr_log_err( "Error on  netmgr_nl_decode_nlmsg\n" );
    return NETMGR_FAILURE;
  }

  netmgr_log_med( "Rcvd Netlink msg type [%d]\n", nlmsg_info.type );

  /* Process based on netlink message type */
  switch( nlmsg_info.type ) {
    case RTM_NEWLINK:
    case RTM_DELLINK:
      /* Process NETLINK_ROUTE message of type LINK */
      netmgr_kif_nl_recv_link_msg( &nlmsg_info );
      break;
    case RTM_NEWADDR:
    case RTM_DELADDR:
      /* Process NETLINK_ROUTE message of type ADDR */
      netmgr_kif_nl_recv_addr_msg( &nlmsg_info );
      break;
    case RTM_NEWPREFIX:
      netmgr_log_high("Received NEWPREFIX, ignoring\n");
      /* Do nothing */
      break;
    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
      /* Do nothing */
      break;
#ifdef FEATURE_DATA_IWLAN
    case XFRM_MSG_NEWAE:
      netmgr_kif_nl_recv_xfrm_newae_msg ( &nlmsg_info );
      break;
#endif /* FEATURE_DATA_IWLAN */
    default:
      /* Ignore all other message types */
      netmgr_log_low("received unknown nl msg\n");
      break;
  }

  /* NetLink message buffer released in netmgr_kif_nl_msg_recv_cmd_free() */

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_nl_msg_recv_cmd_free
===========================================================================*/
/*!
@brief
  Virtual function called by the Command Thread to free KIF command
  to process a received NETLINK message, after execution of the command is
  complete.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_nl_msg_recv_cmd_free (ds_cmd_t * cmd, void * data)
{
  struct msghdr * msgh;
  struct nlmsghdr * nlmsgh;
  netmgr_exec_cmd_t * cmd_buf = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( cmd );

  cmd_buf = (netmgr_exec_cmd_t *)data;

  /* Get message ptr from the user data ptr */
  msgh = (struct msghdr *)cmd_buf->data.info.kif_msg.msg;

  /* Get netlink message ptr from the message header */
  nlmsgh = (struct nlmsghdr *)(msgh->msg_iov->iov_base);

  /* Deallocate memory for the address structure */
  netmgr_free(msgh->msg_name);

  /* Deallocate memory for the message buffer */
  netmgr_free(nlmsgh);

  /* Deallocate memory for the io vector */
  netmgr_free(msgh->msg_iov);

  /* Deallocate memory for the message header */
  netmgr_free(msgh);

  /* Release NetMgr command buffer */
  cmd_buf = cmd->data;
  NETMGR_ASSERT( cmd_buf );
  netmgr_free( cmd_buf );

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_find_default_link
===========================================================================*/
/*!
@brief
  This function goes throughs the link array finds the first link
  that is enabled.

@return
  int - interface id on successful operation, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static inline int netmgr_kif_find_default_link(void)
{
  int i=0;
  for(i=0; i<NETMGR_MAX_LINK; i++)
  {
    if (TRUE == netmgr_kif_cfg.link_array[i].enabled)
    {
      return i;
    }
  }
  return -1;
}

/*===========================================================================
  FUNCTION  netmgr_kif_nl_recv_routing_msg
===========================================================================*/
/*!
@brief
  Virtual function registered with the socket listener thread to receive
  incoming messages over the NETLINK routing socket.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_nl_recv_routing_msg (int fd)
{
  struct msghdr * msgh = NULL;
  netmgr_exec_cmd_t * cmd_buf = NULL;
  unsigned int  msglen = 0;
  int default_link = -1;

  NETMGR_LOG_FUNC_ENTRY;

  /* Read netlink message from the socket */
  if( NETMGR_SUCCESS != netmgr_nl_recv_msg( fd, &msgh, &msglen ) ) {
    netmgr_log_err( "netmgr_kif_nl_recv_routing_msg: netmgr_nl_recv_msg failed!\n" );
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  /* find an available link where the command can be posted to */
  default_link = netmgr_kif_find_default_link();
  if (-1 == default_link)
  {
    netmgr_log_err( "netmgr_kif_nl_recv_routing_msg: could not find default link" );
    netmgr_nl_release_msg( msgh );
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  /* Allocate command object */
  cmd_buf = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd_buf);

  /* Override cmd buffer free method to release netline message */
  cmd_buf->cmd.free_f = netmgr_kif_nl_msg_recv_cmd_free;

  /* Set data ptr in the command object to the netlink message header ptr */
  cmd_buf->data.type = NETMGR_KIF_MSG_CMD;
  cmd_buf->data.link = default_link;  /* process message through first available link */
  cmd_buf->data.info.kif_msg.msg = msgh;
  cmd_buf->data.info.kif_msg.msglen = msglen;

  /* Post command for processing in the command thread context */
  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ) ) {
    netmgr_log_err("netmgr_kif_nl_recv_routing_msg: failed to put commmand\n");
    netmgr_exec_release_cmd( cmd_buf );
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_nl_recv_xfrm_msg
===========================================================================*/
/*!
@brief
  Virtual function registered with the socket listener thread to receive
  incoming messages over the NETLINK XFRM events socket.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_nl_recv_xfrm_msg (int fd)
{
  struct msghdr * msgh = NULL;
  netmgr_exec_cmd_t * cmd_buf = NULL;
  unsigned int msglen = 0;
  int default_link = -1;

  /* Read the netlink message from the socket */
  if (NETMGR_SUCCESS != netmgr_nl_recv_msg( fd, &msgh, &msglen ) )
  {
    netmgr_log_err("netmgr_kif_nl_recv_xfrm_msg: netmgr_nl_recv_msg failed!\n");
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  /* Find an available link where the command can be posted to */
  default_link = netmgr_kif_find_default_link();
  if (-1 == default_link)
  {
    netmgr_log_err("netmgr_kif_nl_recv_xfrm_msg: could not find default link!\n");
    netmgr_nl_release_msg(msgh);
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  /* Allocate command object */
  cmd_buf = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd_buf);

  /* Override cmd buffer free method to release netline message */
  cmd_buf->cmd.free_f = netmgr_kif_nl_msg_recv_cmd_free;

  /* Set data ptr in the command object to the netlink message header ptr */
  cmd_buf->data.type = NETMGR_KIF_MSG_CMD;
  cmd_buf->data.link = default_link; /* Process message through first available link */
  cmd_buf->data.info.kif_msg.msg = msgh;
  cmd_buf->data.info.kif_msg.msglen = msglen;

  /* Post command for processing in the command thread context */
  if (NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd_buf ))
  {
    netmgr_log_err("netmgr_kif_nl_recv_xfrm_msg: failed to put commmand\n");
    netmgr_exec_release_cmd( cmd_buf );
    NETMGR_LOG_FUNC_EXIT;
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_kif_send_event
===========================================================================*/
/*!
@brief
  Generates an asynchronous event indicaiton messages using NETLINK socket.
  See kernel/include/net/netlink.h for details on message TLV formatting.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_send_event
(
  const netmgr_nl_event_info_t *event_info
)
{
  int result = NETMGR_FAILURE;
  char * buffer = NULL;
  unsigned int buflen = 0;

  NETMGR_LOG_FUNC_ENTRY;

  result = netmgr_nl_encode_netmgr_event(event_info,
                                         &buffer,
                                         &buflen);

  if( NETMGR_SUCCESS == result ) {
    /* Validate buffer */
    if( (buffer && (buflen == 0)) ||
        (!buffer && (buflen != 0)) ){
      netmgr_log_err("invalid buffer allocated\n");
      return NETMGR_FAILURE;
    }

    /* Generate NETLINK message */
    result = netmgr_nl_send_msg( netmgr_kif_sk_grp.ev_sk.sk_fd,
                                 NETMGR_NL_MULTICAST_SEND,
                                 buffer,
                                 (uint16)buflen );
  } else {
    netmgr_log_err("failed on netmgr_nl_encode_netmgr_event\n");
  }

  netmgr_free( buffer );

  NETMGR_LOG_FUNC_EXIT;
  return result;
}

/*===========================================================================
  FUNCTION  netmgr_kif_unix_send_event
===========================================================================*/
/*!
@brief
  Generates an asynchronous response message using UNIX socket.
  See kernel/include/net/netlink.h for details on message TLV formatting.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_unix_send_event
(
  const netmgr_nl_event_info_t  *event_info,
  int                           client_socket
)
{
  int           result = NETMGR_FAILURE;
  char          *buffer = NULL;
  unsigned int  buflen = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if ((result = netmgr_nl_encode_netmgr_event(event_info, &buffer, &buflen)) != NETMGR_SUCCESS)
  {
    netmgr_log_err("failed on netmgr_nl_encode_netmgr_event");
    goto error;
  }

  if (((buffer != NULL) && (buflen == 0)) || (buffer == NULL))
  {
    netmgr_log_err("invalid buffer allocated\n");
    goto error;
  }

  if (send(client_socket, buffer, buflen, MSG_DONTWAIT | MSG_NOSIGNAL ) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Failed to send message to UNIX socket - %s", __func__, strerror(errno));
    goto error;
  }
  netmgr_log_med("%s(): Successfully sent message to UNIX socket", __func__);
  result = NETMGR_SUCCESS;

error:
  if (buffer != NULL)
    netmgr_free(buffer);

  NETMGR_LOG_FUNC_EXIT;
  return result;
}

/*===========================================================================
  FUNCTION  netmgr_kif_process_user_cmd
===========================================================================*/
/*!
@brief
  Processes user provided command

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise
*/
/*=========================================================================*/
static void
netmgr_kif_process_user_cmd
(
  ds_cmd_t *ds_cmd,
  void     *data
)
{
  char cmd[NETMGR_KIF_MAX_COMMAND_LENGTH];
  int result = NETMGR_SUCCESS;
  netmgr_nl_event_info_t *event_info = NULL;
  struct netmgr_kif_cb_usercmd_info usercmd_info;
  netmgr_exec_cmd_t *exec_cmd = NULL;
  netmgr_user_cmd_data_t *cmd_data = NULL;
  int user_sock = 0;

  NETMGR_LOG_FUNC_ENTRY;

  (void) ds_cmd;

  exec_cmd = (netmgr_exec_cmd_t *) data;
  if (!exec_cmd)
  {
    netmgr_log_err("%s(): NULL user command", __func__);
    return;
  }

  if (NETMGR_USER_MSG_CMD != exec_cmd->data.type)
  {
    netmgr_log_err("%s(): invalid command!", __func__);
    return;
  }

  memset(cmd, 0, sizeof(cmd));
  cmd_data  = &(exec_cmd->data.info.usr_cmd);
  user_sock = exec_cmd->data.socket_info;

  netmgr_log_high("%s(): Process user command: %d\n", __func__, cmd_data->cmd_id);

  switch (cmd_data->cmd_id)
  {
    case NETMGR_USER_CMD_SCREEN_OFF:
      netmgr_log_med("Received SCREEN_OFF: Installing rules to drop output rst packets!\n");
      (void) snprintf(cmd,
                      sizeof(cmd),
                      "iptables %s -A OUTPUT -p tcp --tcp-flags RST RST -j DROP",
                      iptables_opts);
      (void) netmgr_ds_exec(cmd, NULL);
      break;

    case NETMGR_USER_CMD_SCREEN_ON:
      netmgr_log_med("Received SCREEN_ON: Deleting rules to drop output rst packets!\n");
      (void) snprintf(cmd,
                      sizeof(cmd),
                      "iptables %s -D OUTPUT -p tcp --tcp-flags RST RST -j DROP",
                      iptables_opts);
      (void) netmgr_ds_exec(cmd, NULL);
      break;

#ifdef FEATURE_DATA_IWLAN
    case NETMGR_USER_CMD_ENABLE_PORT_FORWARDING:
    {
      netmgr_log_med("netmgr_kif_process_user_cmd: Received ENABLE_PORT_FORWARDING cmd for"
                     " ip_family[%d]", cmd_data->data.port_forwarding_data.ip_family);

      event_info = (netmgr_nl_event_info_t*) malloc(sizeof(netmgr_nl_event_info_t));
      if(NULL == event_info)
      {
        netmgr_log_err("netmgr_kif_process_user_cmd: Out of memory error ");
        return;
      }
      memset(event_info, 0x0, sizeof(netmgr_nl_event_info_t));
      event_info->event = NETMGR_USER_CMD;
      event_info->param_mask |= NETMGR_EVT_PARAM_USER_CMD;
      event_info->user_cmd = (int)cmd_data->cmd_id;
      event_info->param_mask |= NETMGR_EVT_PARAM_CMD_DATA;
      memcpy(&event_info->cmd_data, cmd_data, sizeof(netmgr_user_cmd_data_t));

      if (NETMGR_SUCCESS
            != netmgr_kif_fill_usercmd_info(&usercmd_info,
                                            cmd_data->data.port_forwarding_data.ip_family,
                                            TRUE))
      {
        netmgr_log_err("%s(): Failed to initialize usercmd_info struct!", __func__);
        return;
      }

      /* TODO: To support backward compatiability with older PLs we need to live with creating
         a new CB chain specifically for handling port-forwarding rules. Till we figure out a
         better design we will use this approach */
      netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_ENABLE_PORTFWD,
                       (void *) &usercmd_info);

      if(usercmd_info.result != NETMGR_SUCCESS)
      {
        netmgr_log_err("netmgr_kif_process_user_cmd: Failed to install ims iptable_rules");
        event_info->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_FAILED;
      }
      else
      {
        netmgr_log_med("netmgr_kif_process_user_cmd: Successfully installed ims iptable_rules");
        event_info->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_SUCCESS;
      }
      netmgr_kif_unix_send_event(event_info, user_sock);
      break;
    }
    case NETMGR_USER_CMD_DISABLE_PORT_FORWARDING:
    {
      netmgr_log_med("netmgr_kif_process_user_cmd: Received DISABLE_PORT_FORWARDING cmd for"
                     " ip_family[%d]", cmd_data->data.port_forwarding_data.ip_family);

      event_info = (netmgr_nl_event_info_t*) malloc(sizeof(netmgr_nl_event_info_t));
      if(NULL == event_info)
      {
        netmgr_log_err("netmgr_kif_process_user_cmd: Out of memory error ");
        return;
      }
      memset(event_info, 0x0, sizeof(netmgr_nl_event_info_t));
      event_info->event = NETMGR_USER_CMD;
      event_info->param_mask |= NETMGR_EVT_PARAM_USER_CMD;
      event_info->user_cmd = (int)cmd_data->cmd_id;
      event_info->param_mask |= NETMGR_EVT_PARAM_CMD_DATA;
      memcpy(&event_info->cmd_data, cmd_data, sizeof(netmgr_user_cmd_data_t));

      if (NETMGR_SUCCESS
            != netmgr_kif_fill_usercmd_info(&usercmd_info,
                                            cmd_data->data.port_forwarding_data.ip_family,
                                            FALSE))
      {
        netmgr_log_err("%s(): Failed to initialize usercmd_info struct!", __func__);
        return;
      }

      /* TODO: To support backward compatiability with older PLs we need to live with creating
         a new CB chain specifically for handling port-forwarding rules. Till we figure out a
         better design we will use this approach */
      netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_DISABLE_PORTFWD,
                       (void *) &usercmd_info);

      if(usercmd_info.result != NETMGR_SUCCESS)
      {
        netmgr_log_err("netmgr_kif_process_user_cmd: Failed to remove ims iptable_rules");
        event_info->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_FAILED;
      }
      else
      {
        netmgr_log_med("Successfully removed ims iptable_rules");
        event_info->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_SUCCESS;
      }
      netmgr_kif_unix_send_event(event_info, user_sock);
      break;
    }
    case NETMGR_USER_CMD_QUERY_PORT_FORWARDING:
    {
      netmgr_log_med("netmgr_kif_process_user_cmd: Received QUERY_PORT_FORWARDING cmd for"
                     " ip_family[%d]", cmd_data->data.port_forwarding_data.ip_family);

      event_info = (netmgr_nl_event_info_t*) malloc(sizeof(netmgr_nl_event_info_t));
      if(NULL == event_info)
      {
        netmgr_log_err("netmgr_kif_process_user_cmd: Out of memory error ");
        return;
      }
      memset(event_info, 0x0, sizeof(netmgr_nl_event_info_t));
      event_info->event = NETMGR_USER_CMD;
      event_info->param_mask |= NETMGR_EVT_PARAM_USER_CMD;
      event_info->user_cmd = (int)cmd_data->cmd_id;
      event_info->param_mask |= NETMGR_EVT_PARAM_CMD_DATA;
      memcpy(&event_info->cmd_data, cmd_data, sizeof(netmgr_user_cmd_data_t));

      /* reset ip_version */
      event_info->cmd_data.data.port_forwarding_data.ip_family = 0;

      if (NETMGR_SUCCESS
            != netmgr_kif_fill_usercmd_info(&usercmd_info,
                                            cmd_data->data.port_forwarding_data.ip_family,
                                            FALSE))
      {
        netmgr_log_err("%s(): Failed to initialize usercmd_info struct!", __func__);
        return;
      }

      /* TODO: To support backward compatiability with older PLs we need to live with creating
         a new CB chain specifically for handling port-forwarding rules. Till we figure out a
         better design we will use this approach */
      netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_QUERY_PORTFWD,
                       (void *) &usercmd_info);

      /* usercmd_info.preference would have the information of either V4 or V6 install
         preference */
      if (usercmd_info.preference)
      {
        if (AF_INET == cmd_data->data.port_forwarding_data.ip_family)
        {
          event_info->cmd_data.data.port_forwarding_data.ip_family = AF_INET;
          netmgr_log_med("%s: ims AF_INET rules preference [install]", __func__);
        }
        else if (AF_INET6 == cmd_data->data.port_forwarding_data.ip_family)
        {
          event_info->cmd_data.data.port_forwarding_data.ip_family = AF_INET6;
          netmgr_log_med("%s: ims AF_INET6 rules preference  [install]", __func__);
        }
      }

      event_info->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_SUCCESS;

      /* send response back to client */
      netmgr_kif_unix_send_event(event_info, user_sock);
      break;
    }
#endif /* FEATURE_DATA_IWLAN */
  }

  if(NULL != event_info)
  {
    free(event_info);
  }

  /* When processing user commands like enable port-forwarding for iWLAN we will be using the
     exec thread to process the message. Once the processing is done we can release the resources */
  close(user_sock);

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_unix_recv_user_cmd
===========================================================================*/
/*!
@brief
  Receives user provided command and sends a response over UNIX domain
  sockets in a tight loop.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_unix_recv_user_cmd
(
  int server_sock
)
{
  for(;;)
  {
    int                     new_conn_fd = NETMGR_UNIX_API_ERROR;
    int                     bufflen = NETMGR_UNIX_API_ERROR;
    char                    buffer[NETMGR_UNIX_MAX_MESSAGE_SIZE];
    socklen_t               socket_size = NETMGR_UNIX_API_ERROR;
    unsigned int            msglen = 0;
    struct sockaddr_un      client_socket;
    netmgr_nl_event_info_t  *event_info = NULL;
    netmgr_exec_cmd_t       *exec_cmd = NULL;
    boolean                 usercmd_posted = FALSE;
    netmgr_user_cmd_data_t *user_cmd_data = NULL;

    netmgr_log_med("%s(): Waiting for incoming connection", __func__);
    socket_size = sizeof(client_socket);
    if ((new_conn_fd = accept4(server_sock, (struct sockaddr *)&client_socket,
                                                   &socket_size, SOCK_CLOEXEC))
        == NETMGR_UNIX_API_ERROR)
    {
      netmgr_log_err("%s(): Unable to accept connection on UNIX socket - %s",
                     __func__, strerror(errno));
                    continue;
    }

    bufflen = recv(new_conn_fd, buffer, NETMGR_UNIX_MAX_MESSAGE_SIZE, 0);
    if(bufflen == NETMGR_UNIX_API_ERROR)
    {
      netmgr_log_err("%s(): Error in recv", __func__);
      goto free_buffers;
    }

    if(bufflen == 0)
    {
      netmgr_log_err("%s(): Client closed socket", __func__);
      goto free_buffers;
    }

    if((event_info = netmgr_malloc(sizeof(netmgr_nl_event_info_t))) == NULL)
    {
      netmgr_log_err("%s(): failed to allocate message buffer", __func__);
      goto close_client_socket;
    }

    memset(event_info, 0x0, sizeof(netmgr_nl_event_info_t));
    if(netmgr_nl_decode_netmgr_event(buffer, bufflen, event_info) != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s(): netmgr_nl_decode_netmgr_event failed", __func__);
      goto free_buffers;
    }

    if(event_info->event == NETMGR_READY_REQ)
    {
      if (NETMGR_GET_PROCESS_READY_REQ() <= 0)
      {
        netmgr_log_med("%s(): Initialization incomplete, ignoring ping", __func__);
      }
      else
      {
        netmgr_nl_event_info_t *event_info_resp = NULL;

        netmgr_log_high("%s(): Ping message recved", __func__);

        if ((event_info_resp = netmgr_malloc(sizeof(netmgr_nl_event_info_t))) == NULL)
        {
          netmgr_log_err("%s(): failed to allocate event_info_resp buffer", __func__);
          goto free_buffers;
        }

        memset(event_info_resp, 0x0, sizeof(netmgr_nl_event_info_t));
        event_info_resp->event = NETMGR_READY_RESP;

        if(netmgr_kif_unix_send_event(event_info_resp, new_conn_fd) == NETMGR_FAILURE)
        {
          netmgr_log_err("%s(): netmgr_kif_send_event failed", __func__);
        }
        if(event_info_resp)
        {
          netmgr_free(event_info_resp);
        }
      }
    }
    else if (event_info->event == NETMGR_USER_CMD)
    {
      netmgr_log_med("%s(): netmgr_kif_nl_recv_ping_msg: Received user command", __func__);

     user_cmd_data = &(event_info->cmd_data);
     if(user_cmd_data->cmd_id == NETMGR_USER_CMD_QUERY_ROUTE_LOOK_UP)
     {
        netmgr_nl_event_info_t *event_info_resp = NULL;

        netmgr_log_high("%s(): Route look up request recved", __func__);

        if ((event_info_resp = netmgr_malloc(sizeof(netmgr_nl_event_info_t))) == NULL)
        {
          netmgr_log_err("%s(): failed to allocate event_info_resp buffer", __func__);
          goto free_buffers;
        }

        memset(event_info_resp, 0, sizeof(netmgr_nl_event_info_t));

        event_info_resp->event = NETMGR_USER_CMD;
        event_info_resp->user_cmd = NETMGR_USER_CMD_QUERY_ROUTE_LOOK_UP;
        event_info_resp->param_mask |= NETMGR_EVT_PARAM_USER_CMD;
        event_info_resp->param_mask |= NETMGR_EVT_PARAM_CMD_DATA;

        netmgr_log_err("%s DBG  r_lup %d  pmask %d pi %d pi3gpp2 %d "
                       "tpref %d xt_t_pref %d  ippref %d apn %s datagin %d"
                       , __func__,
                       user_cmd_data->rl_params.route_lookup,
                       user_cmd_data->rl_params.params_mask,
                       user_cmd_data->rl_params.profile_index,
                       user_cmd_data->rl_params.profile_index_3gpp2,
                       user_cmd_data->rl_params.tech_pref,
                       user_cmd_data->rl_params.xtended_tech_pref,
                       user_cmd_data->rl_params.ip_family_pref,
                       user_cmd_data->rl_params.apn_name,
                       user_cmd_data->rl_params.data_call_origin);

        if(NETMGR_SUCCESS != netmgr_qmi_route_look_up(&(user_cmd_data->rl_params),
                                                      &(event_info_resp->cmd_data.rl_resp),
                                                       user_cmd_data->modem_subs_id,
                                                       user_cmd_data->app_type))
        {
          event_info_resp->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_FAILED;
          netmgr_log_err("%s(): Route look up failed", __func__);
        }
        else
        {
          event_info_resp->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_SUCCESS;
        }

        if(netmgr_kif_unix_send_event(event_info_resp, new_conn_fd) == NETMGR_FAILURE)
        {
          netmgr_log_err("%s(): netmgr_kif_send_event failed", __func__);
        }
        if(event_info_resp)
        {
          netmgr_free(event_info_resp);
        }
     }
     else if(user_cmd_data->cmd_id ==  NETMGR_USER_CMD_LINK_RELEASE)
     {
        netmgr_nl_event_info_t *event_info_resp = NULL;

        netmgr_log_high("%s(): Link release request received", __func__);

        if ((event_info_resp = netmgr_malloc(sizeof(netmgr_nl_event_info_t))) == NULL)
        {
          netmgr_log_err("%s(): failed to allocate event_info_resp buffer", __func__);
          goto free_buffers;
        }

        memset(event_info_resp, 0, sizeof(netmgr_nl_event_info_t));

        event_info_resp->event = NETMGR_USER_CMD;
        event_info_resp->user_cmd = NETMGR_USER_CMD_LINK_RELEASE;
        event_info_resp->param_mask |= NETMGR_EVT_PARAM_USER_CMD;
        event_info_resp->param_mask |= NETMGR_EVT_PARAM_CMD_DATA;

        if(NETMGR_SUCCESS != netmgr_clear_qmi_iface(user_cmd_data->link))
        {
          event_info_resp->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_FAILED;
          netmgr_log_err("%s(): Link Release failed", __func__);
        }
        else
        {
          event_info_resp->cmd_data.txn.txn_status = NETMGR_USER_CMD_STATUS_SUCCESS;
        }

        if(netmgr_kif_unix_send_event(event_info_resp, new_conn_fd) == NETMGR_FAILURE)
        {
          netmgr_log_err("%s(): netmgr_kif_send_event failed", __func__);
        }
        if(event_info_resp)
        {
          netmgr_free(event_info_resp);
        }

     }
     else
     {
        exec_cmd = netmgr_exec_get_cmd();
        if (!exec_cmd)
        {
          netmgr_log_err("%s(): Out of memory: Skipping user_cmd processing", __func__);
          goto free_buffers;
        }

        /* Override execute function for NETMGR_USR_CMD */
        exec_cmd->cmd.execute_f    = netmgr_kif_process_user_cmd;
        exec_cmd->data.type        = NETMGR_USER_MSG_CMD;
        exec_cmd->data.socket_info = new_conn_fd;

        /* copy netmgr_user_cmd_data to the exec cmd obj */
        memcpy(&(exec_cmd->data.info.usr_cmd), &(event_info->cmd_data),
             sizeof(netmgr_user_cmd_data_t));
        if (NETMGR_SUCCESS != netmgr_exec_put_cmd(exec_cmd))
        {
          netmgr_exec_release_cmd(exec_cmd);
          netmgr_log_err("%s(): Failed to put command [%d]",
                         __func__, event_info->cmd_data.cmd_id);
          goto free_buffers;
        }

        usercmd_posted = TRUE;
        goto ret;
      }
    }
    else
    {
      netmgr_log_low("Unknown event message received %d\n", event_info->event);
    }

ret:
    if (usercmd_posted)
    {
      /* If we have posted a user cmd then we should allow the exec thread
         to cleanup event_info and socket information. We should just continue
         here */
      continue;
    }

free_buffers:
    if(event_info)
    {
      netmgr_free(event_info);
    }
close_client_socket:
    close(new_conn_fd);
  }
}

/*===========================================================================
  FUNCTION  netmgr_kif_configure
===========================================================================*/
/*!
@brief
  API to configure virtual Ethernet interface for the specified link.

@return
  int - NETMGR_SUCCESS if successful,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_configure( int link, netmgr_ip_addr_t addr_type )
{
  int ret = NETMGR_FAILURE;
  netmgr_kif_state_t state;

  NETMGR_LOG_FUNC_ENTRY;

  /* following do..while loop can be used in lieu of
     the infamous goto error approach */
  do
  {
    /* Verify that the specified link id is valid */
    if( netmgr_kif_verify_link(link) == NETMGR_FAILURE ) {
      netmgr_log_err("netmgr_kif_open_cnf called with invalid link %d\n", link);
      break;
    }

    /* Process based on current interface state */
    /*Keep track of IP family for which we are expecting Newaddr only in
      Opening KIF state as Newaddr events are not handled for links whose KIF
     state is Open*/
    switch( state = netmgr_kif_get_state(link) ) {
      case NETMGR_KIF_OPENING:
      case NETMGR_KIF_OPEN:
        netmgr_log_med("Starting address type %d configuration on link %d\n",
                       addr_type, link);
        if( NETMGR_SUCCESS == (ret = netmgr_kif_cfg_req(link, addr_type, FALSE)) ) {
          ret = NETMGR_SUCCESS;
        } else {
          netmgr_log_err("netmgr_kif_cfg_req failed for link %d\n", link);
        }
        break; /* breaks from switch */

      default:
        netmgr_log_err("netmgr_kif_configure not allowed in state %d link %d\n",
                       state, link);
        break;
    }

    /* current assumption is that we always come out of
     * do..while after this switch stmt is executed
     * so, if you plan to add more code here, make sure
     * it is enclosed in if (ret == desired_value) block.*/

  } while(0);

  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_reconfigure
===========================================================================*/
/*!
@brief
  API to reconfigure virtual Ethernet interface for the specified link.
  The address information is the previous address which must be purged.

@return
  int - NETMGR_SUCCESS if successful,
        NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_reconfigure( int link,
                        netmgr_address_set_t * addr_info_ptr )
{
  int ret = NETMGR_FAILURE;
  netmgr_kif_state_t state;

  NETMGR_LOG_FUNC_ENTRY;

  /* following do..while loop can be used in lieu of
     the infamous goto error approach */
  do
  {
    /* Verify that the specified link id is valid */
    if( netmgr_kif_verify_link(link) == NETMGR_FAILURE ) {
      netmgr_log_err("netmgr_kif_open_cnf called with invalid link %d\n", link);
      break;
    }

    /* Process based on current interface state */
    switch( state = netmgr_kif_get_state(link) ) {
      case NETMGR_KIF_OPEN:
      case NETMGR_KIF_RECONFIGURING:
        /* Force reset of socket connections */
        if( addr_info_ptr )
        {
          netmgr_kif_purge_address( link, &addr_info_ptr->if_addr, addr_info_ptr->if_mask );

          netmgr_log_med("Starting address reconfiguration on link=%d, family=%d\n",
                         link, addr_info_ptr->if_addr.type);
          if( NETMGR_SUCCESS == (ret = netmgr_kif_cfg_req(link, addr_info_ptr->if_addr.type, addr_info_ptr->is_reconfig)) ) {
            /* Don't move the local breakout interface to reconfig state (as there's no
               address assignment) */
            if (!NETMGR_KIF_IS_REV_RMNET_LINK(link)) {
              netmgr_kif_set_state(link, NETMGR_KIF_RECONFIGURING);
              netmgr_kif_info[link].reconfig_ip_type = addr_info_ptr->if_addr.type;
              netmgr_log_med("Debug: Waiting for IP %d Newaddr on link %d\n",
                       addr_info_ptr->if_addr.type, link);
            }
            ret = NETMGR_SUCCESS;
          } else {
            netmgr_log_err("netmgr_kif_cfg_req failed for link %d\n", link);
          }
          netmgr_kif_info[link].dns_v6_queried = FALSE;
        }
        else
        {
          netmgr_log_err("No address information provided, ignoring input\n");
        }
        break; /* breaks from switch */
      default:
        netmgr_log_err("netmgr_kif_reconfigure not allowed in state %d\n",
                       state);
        break;
    }

    /* current assumption is that we always come out of
     * do..while after this switch stmt is executed
     * so, if you plan to add more code here, make sure
     * it is enclosed in if (ret == desired_value) block.*/

  } while(0);

  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_link_update
===========================================================================*/
/*!
@brief
  Added a functionality to handle extended ip indication for MTU updates.

@return
  int -  NETMGR_SUCCESS if successful,
          NETMGR_FAILURE otherwise

@note
  Do not call this function or reuse for any other updates other than
  extended ip support.

  - Dependencies
    - None

  - Side Effects
       None
*/
/*=========================================================================*/

LOCAL int netmgr_kif_link_update(uint8 link )
{
  if(NETMGR_SUCCESS == netmgr_kif_set_mtu(link,TRUE))
  {
    return NETMGR_SUCCESS;
  }
  return NETMGR_FAILURE;
}
/*===========================================================================
  FUNCTION  netmgr_kif_open
===========================================================================*/
/*!
@brief
  API to bring up virtual Ethernet interface for the specified link. Once
  interface is up, the associated client callback is called.

@return
  int - NETMGR_SUCCESS if command to bring up interface is successfully
        issued, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_open( int link,
                 netmgr_address_set_t * addr_info_ptr,
                 const netmgr_kif_clntcb_t * clntcb,
                 void * clnt_hdl )
{
  int rval = NETMGR_FAILURE;
  netmgr_kif_state_t state;

  NETMGR_ASSERT( addr_info_ptr);

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify the link id first */
  if (netmgr_kif_verify_link(link) == NETMGR_FAILURE) {
    netmgr_log_err("netmgr_kif_open called with invalid link %d\n", link);
    goto error;
  }

  /* Process based on current interface state */
  switch (state = netmgr_kif_get_state(link)) {
    case NETMGR_KIF_CLOSED:
    case NETMGR_KIF_CLOSING:
      /* Interface is currently closed or closing. Issue command to
      ** bring interface up and transition to the opening state.
      */
      netmgr_kif_set_clntcb(link, clntcb);
      netmgr_kif_set_clnt_hdl(link, clnt_hdl);
      netmgr_kif_set_state(link, NETMGR_KIF_OPENING);
      netmgr_kif_open_req(link, addr_info_ptr);
      break;
    default:
      /* Ignore open request in all other states */
      netmgr_log_err("netmgr_kif_open called in state %d\n", state);
      goto error;
  }

  rval = NETMGR_SUCCESS;

 error:
  NETMGR_LOG_FUNC_EXIT;
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_close
===========================================================================*/
/*!
@brief
  API to bring down virtual Ethernet interface for the specified
  link. Once interface is down, the associated client callback is
  called.  The teardown_iface flag control whether kernel interface
  state is changed.

  For dual-IP calls, it is possible one address family is to be
  removed, but the other lives on.  In this case, the interface does
  not teardown but the specified address is purged, and all socket
  connections using address are reset.


@return
  int - NETMGR_SUCCESS if command to bring down interface is successfully
        issued, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_close( int link,
                  netmgr_address_set_t * addr_info_ptr,
                  boolean   teardown_iface )
{
  int rval = NETMGR_FAILURE;
  netmgr_kif_state_t state;
  netmgr_address_info_t * address_info = netmgr_qmi_get_addr_info(link);
  int ip_family = AF_INET;
  struct netmgr_kif_cb_ifinfo ifinfo;
  const char *local_addr = NULL, *dest_addr = NULL;
  int tunnel_family;

  if( addr_info_ptr != NULL)
  {
    ip_family = (NETMGR_IPV4_ADDR == addr_info_ptr->if_addr.type) ?
                  AF_INET :
                  AF_INET6;
  }

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify the link id first */
  if (netmgr_kif_verify_link(link) == NETMGR_FAILURE) {
    netmgr_log_err("netmgr_kif_close called with invalid link %d\n", link);
    goto error;
  }

  if (NETMGR_SUCCESS != netmgr_kif_fill_ifinfo(link, &ifinfo))
  {
    netmgr_log_err("%s(): Failed to initialize callback info structure!", __func__);
    return NETMGR_FAILURE;
  }

  if (!address_info)
  {
    netmgr_log_err("%s(): Failed to obtain address information for link [%d]",
                   __func__, link);
    return NETMGR_FAILURE;
  }

  switch (ip_family)
  {
  case AF_INET:
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4;
    ifinfo.addr._in4.addr = address_info->ipv4.if_addr.addr.v4;
    ifinfo.addr._in4.mask = address_info->ipv4.if_mask;
    break;
  case AF_INET6:
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV6;
    memcpy(&ifinfo.addr._in6.addr,
           &address_info->ipv6.if_addr.addr,
           sizeof(ifinfo.addr._in6.addr));
    ifinfo.addr._in6.mask = address_info->ipv6.if_mask;
    break;
  default:
    netmgr_log_err("%s(): unknown ip_family [%d]", __func__, ip_family) ;
  }

#ifdef FEATURE_DATA_IWLAN
  /* TODO: Till iWLAN related funtionality is completely moved out of core we will need to live
     with using FEATURE_DATA_IWLAN checks when calling iWLAN functions so that the code can compile
     on LE */
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    if (NETMGR_SUCCESS != netmgr_qmi_get_ipsec_tunnel_endpoints(link,
                                                                ip_family,
                                                                &local_addr,
                                                                &dest_addr,
                                                                &tunnel_family))
    {
      netmgr_log_err("%s(): failed to get tunnel endpoints for link [%d] ip_family [%d]",
                     __func__, link, ip_family);
    }

    ifinfo.tunnel_info.local_addr    = local_addr;
    ifinfo.tunnel_info.dest_addr     = dest_addr;
    ifinfo.tunnel_info.tunnel_family = tunnel_family;
  }
#endif /* FEATURE_DATA_IWLAN */

#ifdef FEATURE_DATA_IWLAN
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link) && NULL != addr_info_ptr)
  {
    int assoc_link;
    int ret;

    /* Attempt to remove the SA and routing rules for a reverse rmnet link */
    ret = netmgr_kif_remove_sa_and_routing_rules(link, ip_family, addr_info_ptr);

    /* TODO: This is a really bad hack! We need to fix the iwlan handling so that it moves
       within the state machine */
    netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_NOSTATE_PRE, (void *)&ifinfo);

    /* If the interface is not being torn down or if an iWLAN iface is being brought down
       but it is associated with a forward rmnet, send the config complete. Otherwise, it
       will be sent when the interface transistions to the INIT state */
    if (FALSE == teardown_iface)
    {
      netmgr_log_med("netmgr_kif_close: link=%d is not being torn down, sending config complete\n",
                     link);

      netmgr_qmi_send_rev_ip_config_complete(NETMGR_QMI_IWLAN_CALL_CLEANUP,
                                             link,
                                             ip_family,
                                             ret);
    }
    else if (WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == netmgr_qmi_wds_get_tech_name(link) &&
             NETMGR_LINK_MAX != (assoc_link = netmgr_qmi_iwlan_get_link_assoc(link)))
    {
      /* Purge address from network interface but state unchanged */
      if (addr_info_ptr)
      {
        netmgr_kif_purge_address(link,
                                 &addr_info_ptr->if_addr,
                                 addr_info_ptr->if_mask);
        addr_info_ptr->is_addr_purge_pend = FALSE;
      }

      netmgr_log_med("netmgr_kif_close: rev_link=%d is associated with fwd_link=%d, delaying teardown\n",
                     link,
                     assoc_link);

      netmgr_qmi_send_rev_ip_config_complete(NETMGR_QMI_IWLAN_CALL_CLEANUP,
                                             link,
                                             ip_family,
                                             ret);

      netmgr_qmi_reset_link_wds_data(link);

      netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;

      return NETMGR_SUCCESS;
    }
    else
    {
      netmgr_log_med("netmgr_kif_close: saving ip_family=%d status=%d for link=%d\n",
                     ip_family,
                     ret,
                     link);

      if (NETMGR_SUCCESS != netmgr_qmi_save_rev_ip_config_status(link, ip_family, ret))
      {
        netmgr_log_err("netmgr_kif_close: failed to save SA removal status link=%d\n",
                       link);
      }
    }

    /* TODO: This is a really bad hack! We need to fix the iwlan handling so that it moves
       within the state machine */
    netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_NOSTATE_POST, (void *)&ifinfo);
  }
#endif /* FEATURE_DATA_IWLAN */

#ifdef NETMGR_QOS_ENABLED
    if (netmgr_main_get_qos_enabled())
    {
      /* Remove iptable rules corresponding to this I/F */
      if (!address_info)
      {
        netmgr_log_err(" invalid addr_info_ptr");
      }
      else
      {
        netmgr_tc_create_delete_dynamic_post_routing_rule(link,
                                                    ip_family,
                                                     address_info,
                                                     FALSE);
        /* Reset Qos flag */
        if (AF_INET == ip_family)
        {
          netmgr_kif_cfg.link_array[link].v4_qos_rules_enabled = FALSE;
        }
        else if (AF_INET6 == ip_family)
        {
          netmgr_kif_cfg.link_array[link].v6_qos_rules_enabled = FALSE;
        }
      }
    }
#endif

  /* Process based on current interface state */
  switch (state = netmgr_kif_get_state(link)) {
    case NETMGR_KIF_OPEN:
    case NETMGR_KIF_RECONFIGURING:
    case NETMGR_KIF_OPENING:
      if( teardown_iface )
      {
        netmgr_kif_close_req(link);
        netmgr_kif_set_state(link, NETMGR_KIF_CLOSING);
        netmgr_kif_set_mtu(link,TRUE);
      }
      else
      {
        /* Purge address from network interface but state unchanged */
        if( addr_info_ptr )
        {
          netmgr_kif_purge_address( link,
                                    &addr_info_ptr->if_addr,
                                    addr_info_ptr->if_mask );
          addr_info_ptr->is_addr_purge_pend = FALSE;
          netmgr_kif_set_mtu(link,TRUE);
        }
      }

#ifdef FEATURE_DATA_IWLAN
      if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
#endif
      {
        if (AF_INET6 == ip_family)
        {
          int cmd_rc = NETMGR_FAILURE;
          const char *link_name;

          link_name = netmgr_kif_get_name(link);
          if (NULL == link_name)
          {
            netmgr_log_err("%s(): unable to determine name for link=%d\n", __func__, link);
            return NETMGR_FAILURE;
          }

          cmd_rc = netmgr_kif_set_ipv6_disabled(link_name, NETMGR_IPV6_DISABLED);
          netmgr_log_high("%s(): Disabling IPv6 on interface [%s] : status [%s]",
                          __func__, link_name, cmd_rc == NETMGR_SUCCESS ? "SUCCESS" : "FAILURE");
        }
      }
      break;

    default:
      /* Ignore close request in all other states */
      netmgr_log_err("netmgr_kif_close called in state %d\n", state);
      goto error;
  }
  netmgr_kif_info[link].dns_v6_queried = FALSE;
  netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;
  rval = NETMGR_SUCCESS;

error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_netlink_init
===========================================================================*/
/*!
@brief
  Initialization routine for the KIF NetLink sockets interface.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_netlink_init( void )
{
  NETMGR_LOG_FUNC_ENTRY;

  memset(&netmgr_kif_sk_route, 0, sizeof(netmgr_kif_sk_route));
  memset(&netmgr_kif_sk_grp, 0, sizeof(netmgr_kif_sk_grp));
  memset(&netmgr_kif_sk_xfrm, 0, sizeof(netmgr_kif_sk_xfrm));

  if( NETMGR_SUCCESS !=
      netmgr_nl_listener_init( &netmgr_kif_sk_route.sk_thrd_info,
                               netmgr_kif_sk_route.sk_thrd_fdmap,
                               ds_arrsize(netmgr_kif_sk_route.sk_thrd_fdmap),
                               &netmgr_kif_sk_route.rt_sk,
                               NETLINK_ROUTE,
                               ( RTMGRP_LINK |
                                 RTMGRP_IPV4_IFADDR |
                                 RTMGRP_IPV6_IFADDR |
                                 RTNLGRP_IPV6_PREFIX ),
                               netmgr_kif_nl_recv_routing_msg ) )
  {
    NETMGR_ABORT("netmgr_kif_netlink_init: Error on netmgr_nl_init\n");
    return;
  }

  /* Open a netlink socket for NETLINK_GENERIC protocol. This socket
  ** is used to receive/generate netlink messaging related to NetMgr
  ** event indications for client multicast group */
  if(NETMGR_SUCCESS !=
     netmgr_nl_listener_init( &netmgr_kif_sk_grp.sk_thrd_info,
                               netmgr_kif_sk_grp.sk_thrd_fdmap,
                              ds_arrsize(netmgr_kif_sk_grp.sk_thrd_fdmap),
                              &netmgr_kif_sk_grp.ev_sk,
                              NETMGR_NL_TYPE,
                              NETMGR_NL_GRP_EVENTS,
                              NULL))
  {
    NETMGR_ABORT("netmgr_kif_netlink_init: Error on netmgr_nl_init for NL_TYPE GRP_EVENTS\n");
    return;
  }

#ifdef FEATURE_DATA_IWLAN
  /* Register for XFRM events */
  if (NETMGR_SUCCESS !=
      netmgr_nl_listener_init( &netmgr_kif_sk_xfrm.sk_thrd_info,
                               netmgr_kif_sk_xfrm.sk_thrd_fdmap,
                               ds_arrsize(netmgr_kif_sk_xfrm.sk_thrd_fdmap),
                               &netmgr_kif_sk_xfrm.xfrm_sk,
                               NETLINK_XFRM,
                               NETMGR_XFRM_GRP_EVENTS,
                               netmgr_kif_nl_recv_xfrm_msg))
  {
    netmgr_log_err("%s", "netmgr_kif_netlink_init: Error on netmgr_nl_init for XFRMNLGRP_AEVENTS");
  }
#endif /* FEATURE_DATA_IWLAN */

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_kif_disable_ipv6_autoconf
===========================================================================*/
/*!
@brief
  Disables IPv6 autoconfiguration on a givem interface

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
#if defined FEATURE_DATA_PREFIX_DELEGATION
LOCAL void
netmgr_kif_disable_ipv6_autoconf(int link_id)
{
  unsigned int i;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH];
  const char *link_name = NULL;
  netmgr_cmdq_cfg_val_t val;

  memset(proc_file, 0, NETMGR_KIF_MAX_COMMAND_LENGTH);

  if (netmgr_kif_cfg.link_array[link_id].enabled)
  {
    link_name = netmgr_kif_get_name(link_id);
    if (NULL == link_name)
    {
      netmgr_log_err("failed to obtain link name for link=%d\n",
                     link_id);
    }
    else
    {
      memset(proc_file, 0, sizeof(proc_file));
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/autoconf",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable ipv6 autoconf", __func__);
      }

      memset(proc_file, 0, sizeof(proc_file));
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/accept_ra",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable ipv6 accept_ra", __func__);
      }

      memset(proc_file, 0, sizeof(proc_file));
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/accept_dad",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable ipv6 accept_dad", __func__);
      }

      memset(proc_file, 0, sizeof(proc_file));
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/dad_transmits",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable ipv6 dad_transmits", __func__);
      }
    }
  }
}
#endif /* FEATURE_DATA_PREFIX_DELEGATION */

/*===========================================================================
  FUNCTION  netmgr_kif_enable_ipv6_optimistic_dad
===========================================================================*/
/*!
@brief
  Enables optimistic duplicate address detection for ipv6 on a given interface

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_enable_ipv6_optimistic_dad(int link_id)
{
  unsigned int i;
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH];
  const char *link_name = NULL;
  netmgr_cmdq_cfg_val_t val;

  memset(proc_file, 0, NETMGR_KIF_MAX_COMMAND_LENGTH);

  if (netmgr_kif_cfg.link_array[link_id].enabled)
  {
    if (NULL == (link_name = netmgr_kif_get_name(link_id)))
    {
      netmgr_log_err("failed to obtain link name for link=%d\n",
                     link_id);
      return;
    }

    netmgr_log_low("%s(): Enabling ipv6 optimistic dad on link %s\n",
                   __func__,
                   link_name);

    snprintf(proc_file,
             sizeof(proc_file),
             "/proc/sys/net/ipv6/conf/%s/optimistic_dad",
             link_name);

    val.intval = 1;
    if (NETMGR_SUCCESS
          != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
    {
      netmgr_log_err("%s(): Failed to enable ipv6 optimistic dad on link %s\n",
                     __func__,
                    link_name);
    }
    else
    {
      netmgr_log_low("%s(): Setting retrans timer %d ms on link %s\n",
                     __func__,
                     NETMGR_KIF_IPV6_DAD_RETRANSMIT_TIME,
                     link_name);

      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/neigh/%s/retrans_time_ms",
               link_name);

      val.intval = NETMGR_KIF_IPV6_DAD_RETRANSMIT_TIME;
      if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): Failed to set retrans timer on link %s\n",
                       __func__,
                      link_name);
      }

    }

  }
}

/*===========================================================================
FUNCTION netmgr_kif_set_link_ipv6_config
===========================================================================*/
/*!
@brief
  Wrapper function to configure IPv6 settings on an interface
@note
  - Dependencies
  - None
  - Side Effects
  - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_set_link_ipv6_config
(
  const char *link,
  const char *property,
  const char *value
)
{
  char proc_file[NETMGR_KIF_MAX_COMMAND_LENGTH] = "";
  netmgr_cmdq_cfg_val_t val;

  if (!link || !property || !value)
    return;

  snprintf(proc_file, sizeof(proc_file), "/proc/sys/net/ipv6/conf/%s/%s",
           link, property);

  strlcpy(val.strval, value, sizeof(val.strval));

  if (NETMGR_SUCCESS
            != netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_STR, val))
  {
    netmgr_log_err("%s(): failed to configure IPv6 property %s for link [%s]",
                   __func__, property, link);
  }
}



/*===========================================================================
FUNCTION netmgr_kif_update_single_link_config
===========================================================================*/
/*!
@brief
  Update link configuration for a single link.
@note
  - Dependencies
  - None
  - Side Effects
  - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_update_single_link_config
(
  int link
)
{
  const char *link_name = NULL;
  int rc = 0;

  netmgr_log_low("%s(): Configuring proc settings on link %d\n",
                 __func__, link);

  if (NULL == (link_name = netmgr_kif_get_name(link)))
  {
    netmgr_log_err("failed to obtain link name for link=%d\n", link);
    return;
  }

  if ( !NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
     netmgr_kif_ifioctl_notify_mux_channel(netmgr_main_cfg.phys_net_dev,
                                           link_name, link + 1);
  }

#ifndef FEATURE_DATA_LINUX_LE
  netmgr_kif_set_ssdp_rule_on_iface( link_name );
#else
  netmgr_kif_set_link_ipv6_config(link_name,"accept_ra_defrtr","0");
#endif /* FEATURE_DATA_LINUX_LE */
  netmgr_kif_set_link_ipv6_config(link_name,"accept_ra","2");
  netmgr_kif_set_link_ipv6_config(link_name,"accept_ra_mtu","0");

  if ((netmgr_main_cfg.data_format.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV4) ||
      (netmgr_main_cfg.data_format.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV3))
  {
    rc = netmgr_kif_ifioctl_set_ul_checksum_offload_feature(link_name);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed setting UL checksum offload feature for dev %s [rc:%d]",
                       __func__, link_name, rc);
    }
  }

  if ((netmgr_main_cfg.data_format.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV4) ||
      (netmgr_main_cfg.data_format.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV3))
  {
    rc = netmgr_kif_ifioctl_set_dl_checksum_offload_feature(link_name);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_low("%s(): Failed setting DL checksum offload feature for dev %s [rc:%d]",
                       __func__, link_name, rc);
    }
  }

  rc = netmgr_kif_configure_dl_gro_feature(link_name, netmgr_main_cfg.data_format.dl_gro_enabled);
  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): Failed to %s DL GRO feature for dev %s [rc:%d]",
                   __func__, netmgr_main_cfg.data_format.dl_gro_enabled ? "enable" : "disable", link_name, rc);
  }
  else
  {
    netmgr_log_low("%s(): %s DL GRO feature for dev %s [rc:%d]",
                   __func__, netmgr_main_cfg.data_format.dl_gro_enabled ? "Enabled" : "Disabled", link_name, rc);
  }

  if (netmgr_main_cfg.data_format.ul_gso_enabled == 1)
  {
    if(!netmgr_kif_enable_ul_ssg_feature(link_name))
    {
      netmgr_log_low("%s(): Successfully settings SSG on link %d\n",
                    __func__, link);
      rc = netmgr_kif_enable_ul_gso_feature(link_name);
      if (NETMGR_SUCCESS != rc)
      {
        netmgr_log_err("%s(): Failed enabling UL GSO feature for dev %s [rc:%d]",
                       __func__, link_name, rc);
      }
    }
  }

  if (netmgr_main_cfg.vnd_rps_mask > 0)
  {
    if (netmgr_kif_set_rps_mask(link_name, netmgr_main_cfg.vnd_rps_mask)
        != NETMGR_RMNET_SUCCESS)
    {
      netmgr_log_err("%s(): failed to set RPS mask on dev %s", __func__, link_name);
    }
  }
}

/*===========================================================================
  FUNCTION  netmgr_kif_init_iface
===========================================================================*/
/*!
@brief
  Initialize kernel interface state

@return
  int -  NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_init_iface( int link )
{
  short flags;
  int ret = NETMGR_FAILURE;
  uint32 mode;
  boolean qos_mode;

  NETMGR_LOG_FUNC_ENTRY

  /* Refresh the kernel ifindex as on some targets it can change */
  if( NETMGR_SUCCESS != netmgr_kif_init_link_ifindex( link ) )
  {
    netmgr_log_err("netmgr_kif_init: netmgr_kif_init_link_ifindex() failed"
                   " for %s\n", netmgr_kif_info[link].name);
    return NETMGR_FAILURE;
  }

  /* close the interface if open; this ensures we are in DOWN state
   * initially. */
  flags = 0;
  if (netmgr_kif_ifioctl_get_flags(netmgr_kif_info[link].name, &flags) < 0) {
    /* Could not get device. This is probably an interface that we
    ** don't care about. */
    netmgr_log_err("netmgr_kif_init: netmgr_kif_ifioctl_get_flags() "
                   "failed for %s, disable link\n",
                   netmgr_kif_info[link].name);
    return NETMGR_FAILURE;
  }

  if (flags & IFF_UP) {
    netmgr_log_high("kernel interface %d found open at init\n", link);
    netmgr_log_high("kernel interface %d will be closed at init\n", link);
    /* close iface */
    netmgr_kif_close_req(link);
    /* blow away iface config */
    netmgr_kif_clear_iface(link);
    /* mark the status */
    netmgr_kif_set_link_powerup_state(link, NETMGR_KIF_LINK_POWERUP_STATE_UP);
  }

  /* Do per link config on the ifaces */
  netmgr_kif_update_single_link_config (link);


#if defined FEATURE_DATA_PREFIX_DELEGATION
  netmgr_kif_disable_ipv6_autoconf(link);
#else
  netmgr_kif_enable_ipv6_optimistic_dad(link);
#endif /* FEATURE_DATA_PREFIX_DELEGATION */

#ifndef FEATURE_DS_LINUX_DRIVER_LEGACY
  /* Set driver link-layer protcol mode */
#ifdef FEATURE_DATA_IWLAN
  /* For reverse rmnet ports always set to IP mode */
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    mode = RMNET_MODE_LLP_IP;
  }
  else
#endif /* FEATURE_DATA_IWLAN */
  {
    mode = (netmgr_main_cfg.runmode & NETMGR_MAIN_RUNMODE_ETHERNET)?
           RMNET_MODE_LLP_ETH : RMNET_MODE_LLP_IP;
  }

  if( NETMGR_SUCCESS !=
      netmgr_kif_ifioctl_set_llpmode( netmgr_kif_info[link].name,
                                      mode ) ) {
    netmgr_log_err("netmgr_kif_init: netmgr_kif_ifioctl_set_llpmode() failed"
                   " for %s\n", netmgr_kif_info[link].name);

    /* Reset runmode to default (IP) */
    netmgr_main_cfg.runmode &= ~NETMGR_MAIN_RUNMODE_ETHERNET;
  }

  /* Set driver QOS header mode */
#ifdef FEATURE_DATA_IWLAN
  /* For reverse rmnet ports always disable QoS */
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    qos_mode = FALSE;
  }
  else
#endif /* FEATURE_DATA_IWLAN */
  {
    qos_mode = (netmgr_main_cfg.runmode & NETMGR_MAIN_RUNMODE_QOSHDR)?
           TRUE : FALSE;
  }

  if( (!netmgr_main_get_rmnet_data_enabled()) && NETMGR_SUCCESS !=
      netmgr_kif_ifioctl_set_qosmode( netmgr_kif_info[link].name,
                                      qos_mode ) ) {
    netmgr_log_err("netmgr_kif_init: netmgr_kif_ifioctl_set_qosmode() failed"
                   " for %s\n", netmgr_kif_info[link].name);
    /* Reset runmode to default (No QoS) */
    netmgr_main_cfg.runmode &= ~NETMGR_MAIN_RUNMODE_QOSHDR;
  }
#endif /* FEATURE_DS_LINUX_DRIVER_LEGACY */

  /* Initialize interface state to closed */
  netmgr_kif_info[link].state = NETMGR_KIF_CLOSED;

  /* Initialize client callback struct ptr to null */
  netmgr_kif_info[link].clntcb = NULL;

  netmgr_kif_info[link].dns_v6_queried = FALSE;

  netmgr_kif_info[link].reconfig_ip_type = NETMGR_IP_ADDR_INVALID;

#ifdef FEATURE_DATA_IWLAN
  if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
#endif
  {
    int cmd_rc = NETMGR_FAILURE;
    const char *link_name;

    link_name = netmgr_kif_get_name(link);
    if (NULL == link_name)
    {
      netmgr_log_err("%s(): unable to determine name for link=%d\n", __func__, link);
      return NETMGR_FAILURE;
    }

    cmd_rc = netmgr_kif_set_ipv6_disabled(link_name, NETMGR_IPV6_DISABLED);
    netmgr_log_high("%s(): Disabling IPv6 on interface [%s] : status [%s]",
                    __func__, link_name, cmd_rc == NETMGR_SUCCESS ? "SUCCESS" : "FAILURE");
  }

  NETMGR_LOG_FUNC_EXIT
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_oos_cleanup
===========================================================================*/
/*!
@brief
  Perform iWLAN clean-up on Modem OOS

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_oos_cleanup
(
  int  link
)
{
  netmgr_kif_cb_link_ssr_info_t ssr_ifinfo;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS !=
          netmgr_kif_fill_link_ssr_cb_ifinfo(link, &ssr_ifinfo))
  {
    netmgr_log_err("%s(): Failed to initialize callback info structure",
                    __func__);
    return;
  }

#ifdef FEATURE_DATA_IWLAN
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    netmgr_address_info_t *addr_info = netmgr_qmi_get_addr_info(link);

    if (!addr_info)
    {
      netmgr_log_err("netmgr_kif_oos_cleanup: invalid addr_info for link=%d\n", link);
      goto bail;
    }
    else
    {
      /* Perform link specific clean-up */
      netmgr_kif_remove_sa_and_routing_rules(link, AF_INET, &addr_info->ipv4);
      netmgr_kif_remove_sa_and_routing_rules(link, AF_INET6, &addr_info->ipv6);

      /* Remove XFRM policies which handle specific use-cases
       * Ex. skip encryption of NS and NA */
      NETMGR_KIF_UTIL_REMOVE_IPSEC_XFRM_POLICY_EX(out);
    }

    if ((NSWO_ONLY == netmgr_main_get_iwlan_enabled() &&
         NETMGR_IS_REV_IP_LOCAL_BRK_OUT_LINK(link))
        ||
        (NETMGR_KIF_IS_MAX_REV_RMNET_LINK(link)))
    {
      if (netmgr_main_cfg.iwlan_client_enable)
      {
        if (NETMGR_SUCCESS != netmgrIwlanClientRelease())
        {
           netmgr_log_err("Error in releasing iwlan client");
           goto bail;
        }
        else
        {
           netmgr_log_high("netmgrIwlanClientRelease success");
        }
      }
    }
  }
#endif /* FEATURE_DATA_IWLAN */

  /* remove ssdp rules as part of cleanup */
  netmgr_kif_remove_ssdp_rule_on_iface(link);

  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_OOS,
                   (void *)&ssr_ifinfo);

bail:
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_kif_reset_link
===========================================================================*/
/*!
@brief
  Reinitialize link data structures on reset command.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_reset_link
(
  int                 link,
  netmgr_sm_events_t  evt
)
{
  netmgr_nl_event_info_t  event_info;
  unsigned int i;

  NETMGR_LOG_FUNC_ENTRY;

  /* Reset kernel interface state */
  if( NETMGR_SUCCESS != netmgr_kif_init_iface(link) ) {
    /* Ignore service initialization failure during SSR */
    if (NETMGR_MODEM_IS_EV == evt || NETMGR_INVALID_EV == evt)
    {
      netmgr_log_high( "Ignoring KIF init failure during NETMGR_MODEM_IS_EV "
                       "on link[%d]\n", link );
      return NETMGR_FAILURE;
    }
    else
    {
      netmgr_log_err("netmgr_kif_reset_link: cannot init iface[%d]"
                     "while processing EV [%d]\n", link, evt);
      return NETMGR_FAILURE;
    }
  }

  netmgr_kif_update_rmnet_proc_entries(link);

  /* Notify clients that state reset occurred */
  memset( &event_info, 0x0, sizeof(event_info) );
  event_info.link = link;
  event_info.event = NET_PLATFORM_RESET_EV;
  event_info.param_mask = NETMGR_EVT_PARAM_NONE;

  if( NETMGR_SUCCESS != netmgr_kif_send_event( &event_info ) ) {
    netmgr_log_err("failed on NET_PLATFORM_RESET_EV\n");
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_cleanup
===========================================================================*/
/*!
@brief
  Performs cleanup of kif module.  Invoked at process termination.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void netmgr_kif_cleanup( void )
{
  int i=0;
  NETMGR_LOG_FUNC_ENTRY;

  for(i=0; i<NETMGR_MAX_LINK; i++)
  {
    (void)netmgr_kif_reset_link(i, NETMGR_INVALID_EV);
  }

  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_kif_update_rmnet_proc_entries
===========================================================================*/
/*!
@brief
  Update the necessary proc entries corresponding to the given rmnet link

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_update_rmnet_proc_entries
(
  int i
)
{
  char proc_file[NETMGR_MAX_COMMAND_LENGTH] = "";
  netmgr_cmdq_cfg_val_t val;

  if (i < 0 || i >= NETMGR_MAX_LINK)
  {
    netmgr_log_err("%s(): invalid link=%d\n",
                   __func__,
                   i);
    goto bail;
  }

  if (!netmgr_kif_cfg.link_array[i].enabled)
  {
    netmgr_log_err("%s(): disabled link=%d\n",
                   __func__,
                   i);
  }
  else
  {
    const char *link_name = NULL;

    if (NULL == (link_name = netmgr_kif_get_name(i)))
    {
      netmgr_log_err("failed to obtain link name for link=%d\n",
                     i);
      goto bail;
    }

    snprintf(proc_file,
             sizeof(proc_file),
             "/proc/sys/net/ipv6/conf/%s/accept_ra",
             link_name);

    val.intval = 2;
    if (NETMGR_SUCCESS !=
            netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
    {
      netmgr_log_err("%s(): failed to set accept_ra on link [%s]",
                     __func__, link_name);
    }

    if (NETMGR_KIF_IS_REV_RMNET_LINK(i))
    {
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv4/conf/%s/accept_local",
               link_name);

      val.intval = 1;
      if (NETMGR_SUCCESS !=
            netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to set accept_local on link [%s]",
                       __func__, link_name);
      }

      /* Disable DAD on reverse Rmnets to speed up iface bring-up */
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/dad_transmits",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS !=
            netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable DAD on link [%s]",
                       __func__, link_name);
      }

      /* Disable autoconf and DAD on reverse Rmnets to speed up iface bring-up */
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv6/conf/%s/autoconf",
               link_name);

      val.intval = 0;
      if (NETMGR_SUCCESS !=
            netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to disable autoconf on link [%s]",
                       __func__, link_name);
      }

      /* Skip IPv4 de-fragmentation for reverse links */
      snprintf(proc_file,
               sizeof(proc_file),
               "/proc/sys/net/ipv4/conf/%s/nf_ipv4_defrag_skip",
               link_name);

      val.intval = 1;
      if (NETMGR_SUCCESS !=
            netmgr_kif_update_system_config(proc_file, NETMGR_CMDQ_TYPE_INT, val))
      {
        netmgr_log_err("%s(): failed to set config to skip IPv4 defrag on link [%s]",
                       __func__, link_name);
      }
    }
  }

bail:
  return;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_update_config_and_routing_rules
===========================================================================*/
/*!
@brief
1. Insert the custom routing table at a higher priority than that of the local
   table
2. Enable the following kernel configs:
    accept_local - to allow packets coming from the modem with the same source
                   IP address as that of AP side rmnets
    ip_forward   - to enable AP to act as a router on modem's behalf
    ip_local_reserved_ports - to mark the modem port range as reserved on the AP side
    ip_local_port_range - to set the port range from which Apps on AP can be
                          allocated ephemeral ports

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_update_config_and_routing_rules(void)
{
  int i;
  int rc = NETMGR_SUCCESS;
  char proc_file[NETMGR_MAX_COMMAND_LENGTH] = "";
  char xfrm_replay_thresh[NETMGR_MAX_STR_LENGTH] = "/proc/sys/net/core/xfrm_aevent_rseqth";
  char xfrm_timer_thresh[NETMGR_MAX_STR_LENGTH] = "/proc/sys/net/core/xfrm_aevent_etime";
  char ipv6_accept_ra_config[NETMGR_MAX_STR_LENGTH] = "/proc/sys/net/ipv6/conf/all/accept_ra";
  char local_port_range[NETMGR_MAX_STR_LENGTH] = "/proc/sys/net/ipv4/ip_local_port_range";
  const char* apps_port_range = "" NETMGR_KIF_APPS_PORT_START " " NETMGR_KIF_APPS_PORT_END "";
  char def[NETMGR_KIF_PROPERTY_REKEY_SIZE+1];
  static char args[PROP_VALUE_MAX];
  int ret;
  unsigned long rekey_val;
  netmgr_cmdq_cfg_val_t val;

  memset( args, 0x0, sizeof(args) );
  memset( def, 0x0, sizeof(def) );

  NETMGR_LOG_FUNC_ENTRY;

  if (IWLAN_DISABLE == netmgr_main_get_iwlan_enabled())
  {
    goto ret;
  }

  snprintf( def, sizeof(def), "%u", UINT32_MAX );
  ret = property_get( NETMGR_KIF_PROPERTY_REKEY, args, def);

  if (NETMGR_KIF_PROPERTY_REKEY_SIZE < ret)
  {
    netmgr_log_err("System property %s has unexpected size(%d), setting default\n",
                   NETMGR_KIF_PROPERTY_REKEY, ret);
    rekey_val = UINT32_MAX;
  }
  else
  {
    rekey_val = strtoul(args, NULL, 10);
    if (UINT32_MAX < rekey_val)
    {
      netmgr_log_err("System property %s has exceeded limit, setting default",
                   NETMGR_KIF_PROPERTY_REKEY);
      rekey_val = UINT32_MAX;
    }
  }

  /* Set the sequence number threshold */
  netmgr_log_med("%s(): Issuing commands to set the xfrm replay configs", __func__);
  val.longval = rekey_val;
  if (NETMGR_SUCCESS
        != netmgr_kif_update_system_config(xfrm_replay_thresh, NETMGR_CMDQ_TYPE_LONG, val))
  {
    netmgr_log_err("%s(): Failed to configure XFRM replay threshold!", __func__);
    rc = NETMGR_FAILURE;
  }

  /* xfrm_aevent_etime controls the delay between indications. By default the kernel
   * sends the indication 1 sec after the event is triggered. We want the indications
   * to be sent immediately and we set the value to zero */
  val.intval = 0;
  if (NETMGR_SUCCESS
        != netmgr_kif_update_system_config(xfrm_timer_thresh, NETMGR_CMDQ_TYPE_INT, val))
  {
    netmgr_log_err("%s(): Failed to configure XFRM timer threshold!", __func__);
    rc = NETMGR_FAILURE;
  }

  netmgr_log_low("%s(): Enabling IPv6 accept_ra for all links", __func__);
  val.intval = 2;
  if (NETMGR_SUCCESS !=
        netmgr_kif_update_system_config(ipv6_accept_ra_config, NETMGR_CMDQ_TYPE_INT, val))
  {
    netmgr_log_err("%s(): failed to enable IPv6 accept_ra flag for all links", __func__);
    rc = NETMGR_FAILURE;
  }

  /* Reserve apps port range */
  netmgr_log_low("%s(): Setting ip_local_port_range", __func__);
  strlcpy(val.strval, apps_port_range, sizeof(val.strval));
  if (NETMGR_SUCCESS !=
        netmgr_kif_update_system_config(local_port_range, NETMGR_CMDQ_TYPE_STR, val))
  {
    netmgr_log_err("%s(): failed to enable IPv6 accept_ra flag for all links", __func__);
    rc = NETMGR_FAILURE;
  }

  for (i = 0; i < NETMGR_MAX_LINK; i++)
  {
    netmgr_kif_update_rmnet_proc_entries(i);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_xfrm_policy_sel_str
===========================================================================*/
/*!
@brief
  Returns the IPSec policy selector string to be used with the ip xfrm policy
  command

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_xfrm_policy_sel_str
(
  netmgr_address_info_t  *addr_info_ptr,
  netmgr_ip_addr_t       ip_type,
  netmgr_ipsec_sa_dir_t  dir,
  char                   *policy_sel_buf,
  unsigned int           policy_sel_buf_len
)
{
  int rc = NETMGR_FAILURE;
  char iface_addr_buf[NETMGR_MAX_STR_LENGTH] = "";
  netmgr_ip_address_t  *ip_addr = NULL;
  unsigned int prefix_len;

  NETMGR_LOG_FUNC_ENTRY;

  if (!addr_info_ptr || !policy_sel_buf || 0 == policy_sel_buf_len)
  {
    netmgr_log_err("netmgr_kif_get_xfrm_policy_sel_str: invalid input\n");
    goto bail;
  }

  if (NETMGR_IPV4_ADDR == ip_type)
  {
    ip_addr = &addr_info_ptr->ipv4.if_addr;
    prefix_len = (unsigned int)ds_get_num_bits_set_count(addr_info_ptr->ipv4.if_mask);
  }
  else if (NETMGR_IPV6_ADDR == ip_type)
  {
    ip_addr = &addr_info_ptr->ipv6.if_addr;
    prefix_len = addr_info_ptr->ipv6.if_mask;
  }
  else
  {
    netmgr_log_err("netmgr_kif_get_xfrm_policy_sel_str: unknown addr type=%d\n",
                   ip_type);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_ip_addr_to_str(ip_addr,
                                                           prefix_len,
                                                           iface_addr_buf,
                                                           sizeof(iface_addr_buf)))
  {
    netmgr_log_err("netmgr_kif_get_xfrm_policy_sel_str: failed to convert iface addr to string\n");
    goto bail;
  }

  if (NETMGR_SA_DIR_TX == dir)
  {
    snprintf(policy_sel_buf,
             policy_sel_buf_len,
             "src %s",
             iface_addr_buf);
  }
  else
  {
    snprintf(policy_sel_buf,
             policy_sel_buf_len,
             "dst %s",
             iface_addr_buf);
  }

  netmgr_log_med("netmgr_kif_get_xfrm_policy_sel_str: policy_sel_buf=%s\n", policy_sel_buf);

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_xfrm_state_encryption_str
===========================================================================*/
/*!
@brief
  Stores the IPSec encryption string to be used with the "ip xfrm state"
  command

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_xfrm_state_encryption_str
(
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config,
  netmgr_ipsec_sa_dir_t                        dir,
  char                                         *enc_buf,
  unsigned int                                 enc_buf_len
)
{
  int rc = NETMGR_FAILURE;
  const char *ealgo = NULL;
  char ekey_str[NETMGR_MAX_STR_LENGTH] = "";
  uint8_t   *ekey = NULL;
  uint32_t  ekey_len;

  NETMGR_LOG_FUNC_ENTRY;

  if (!sa_config || !enc_buf || 0 == enc_buf_len)
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_encryption_str: invalid input\n");
    goto bail;
  }

  if (!sa_config->crypto_algo_valid)
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_encryption_str: crypto mask not set\n");
    goto bail;
  }

  if ((NETMGR_SA_DIR_TX == dir && !(sa_config->crypto_key_tx_valid)) ||
      (NETMGR_SA_DIR_RX == dir && !(sa_config->crypto_key_rx_valid)))
  {
    netmgr_log_err("netmgr_kif_install_sa_rules: crypto key not present in dir=%d", dir);
    goto bail;
  }

  if (!(ealgo = netmgr_util_get_ipsec_algo_str(NETMGR_IPSEC_ALGO_CRYPTO,
                                               sa_config)))
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_encryption_str: failed to convert crypto algo=%d to string",
                   sa_config->crypto_algo);
    goto bail;
  }

  if (NETMGR_SA_DIR_TX == dir)
  {
    ekey = sa_config->crypto_key_tx;
    ekey_len = sa_config->crypto_key_tx_len;
  }
  else
  {
    ekey = sa_config->crypto_key_rx;
    ekey_len = sa_config->crypto_key_rx_len;
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ipsec_key_to_str(ekey,
                                                                 ekey_len,
                                                                 ekey_str,
                                                                 sizeof(ekey_str)))
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_encryption_str: failed to convert crypto key to string");
    goto bail;
  }

  snprintf(enc_buf,
           enc_buf_len,
           "enc %s %s",
           ealgo,
           ekey_str);

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_xfrm_state_authentication_str
===========================================================================*/
/*!
@brief
  Stores the IPSec authentication string to be used with the "ip xfrm state"
  command

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_xfrm_state_authentication_str
(
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config,
  netmgr_ipsec_sa_dir_t                        dir,
  char                                         *auth_buf,
  unsigned int                                 auth_buf_len
)
{
  int rc = NETMGR_FAILURE;
  const char *halgo = NULL;
  char hkey_str[NETMGR_MAX_STR_LENGTH] = "";
  uint8_t   *hkey = NULL;
  uint32_t  hkey_len;
  const int sha2_key_truncation_len = 128;

  NETMGR_LOG_FUNC_ENTRY;

  if (!sa_config || !auth_buf || 0 == auth_buf_len)
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_authentication_str: invalid input");
    goto bail;
  }

  if (!sa_config->hash_algo_valid)
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_authentication_str: hash mask not set");
    goto bail;
  }

  if (( NETMGR_SA_DIR_TX == dir
          && !((sa_config->hash_key_tx_valid) || (sa_config->ext_hash_key_tx_valid)) )
      ||
      ( NETMGR_SA_DIR_RX == dir
          && !((sa_config->hash_key_rx_valid) || (sa_config->ext_hash_key_rx_valid)) ))
  {
    netmgr_log_err("netmgr_kif_install_sa_rules: hash key not present in dir=%d", dir);
    goto bail;
  }

  if (!(halgo = netmgr_util_get_ipsec_algo_str(NETMGR_IPSEC_ALGO_HASH,
                                               sa_config)))
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_authentication_str: failed to convert hash algo=%d to string",
                   sa_config->hash_algo);
    goto bail;
  }

  if (NETMGR_SA_DIR_TX == dir)
  {
    /* hash_key_tx and hash_key_tx_len are deprecated. We need
       to use the new TLVs going forward */
    if (sa_config->ext_hash_key_tx_valid)
    {
      hkey = sa_config->ext_hash_key_tx;
      hkey_len = sa_config->ext_hash_key_tx_len;
    }
    else
    {
      hkey = sa_config->hash_key_tx;
      hkey_len = sa_config->hash_key_tx_len;
    }
  }
  else
  {
    if (sa_config->ext_hash_key_rx_valid)
    {
      hkey = sa_config->ext_hash_key_rx;
      hkey_len = sa_config->ext_hash_key_rx_len;
    }
    else
    {
      hkey = sa_config->hash_key_rx;
      hkey_len = sa_config->hash_key_rx_len;
    }
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ipsec_key_to_str(hkey,
                                                                 hkey_len,
                                                                 hkey_str,
                                                                 sizeof(hkey_str)))
  {
    netmgr_log_err("netmgr_kif_get_xfrm_state_authentication_str: failed to convert hash key to string");
    goto bail;
  }

  if (WDS_IPSEC_CRYPTO_ALGO_SHA256_V01 == sa_config->hash_algo)
  {
    /* SHA256 hash algorithm requires that the key length to be atleast 128 bits. By
       default the kernel truncates key length to 96 bits which is insufficient for
       this algorithm. For now we are using a hard-coded value for truncation length
       because there is no TLV to query this information */
    snprintf(auth_buf,
           auth_buf_len,
           "auth-trunc %s %s %d",
           halgo,
           hkey_str,
           sha2_key_truncation_len);
  }
  else
  {
    /* For other algorithms we will let kernel default to 96 bit truncation */
    snprintf(auth_buf,
             auth_buf_len,
             "auth %s %s",
             halgo,
             hkey_str);
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_xfrm_state_encap_str
===========================================================================*/
/*!
@brief
  Stores the IPSec encapsulation string to be used with the "ip xfrm state"
  command

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_xfrm_state_encap_str
(
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config,
  netmgr_ipsec_sa_dir_t                        dir,
  char                                         *encap_buf,
  unsigned int                                 encap_buf_len
)
{
  int rc = NETMGR_FAILURE;
  int modem_port = NETMGR_MAIN_IKE_PORT_INVALID;
  char local_addr[NETMGR_MAX_STR_LENGTH];
  char dest_addr[NETMGR_MAX_STR_LENGTH];

  NETMGR_LOG_FUNC_ENTRY;

  if (!sa_config || !encap_buf || 0 == encap_buf_len)
  {
    netmgr_log_err("%s(): invalid input", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ip_addr_to_str(&sa_config->local_addr,
                                                               local_addr,
                                                               sizeof(local_addr)))
  {
    netmgr_log_err("%s(): failed to convert SA local address to string!", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ip_addr_to_str(&sa_config->dest_addr,
                                                               dest_addr,
                                                               sizeof(dest_addr)))
  {
    netmgr_log_err("%s(): failed to convert SA dest address to string!", __func__);
    goto bail;
  }
  if (sa_config->udp_encap_src_port_valid)
  {
    /* Update ikeport with TLV's port network to host order*/
    modem_port = netmgr_main_cfg_update_ike_port(ntohs(sa_config->udp_encap_src_port),
                                                 local_addr,
                                                 dest_addr);
  }
  else
  {
    modem_port = netmgr_main_cfg_get_ike_port(local_addr, dest_addr);
  }
  if (sa_config->is_udp_encaps_valid &&
      TRUE == sa_config->is_udp_encaps)
  {
    if (NETMGR_MAIN_IKE_PORT_INVALID == modem_port)
    {
      netmgr_log_med("%s(): failed to setup NATT port", __func__);
      goto bail;
    }

    snprintf(encap_buf,
             encap_buf_len,
             "encap espinudp %u %u 0.0.0.0",
             (dir == NETMGR_SA_DIR_TX) ? modem_port : NETMGR_KIF_NATT_SERVER_PORT,
             (dir == NETMGR_SA_DIR_TX) ? NETMGR_KIF_NATT_SERVER_PORT : modem_port);
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

#define NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_STATE(DIR,mode,state_id,enc,auth,encap)     \
do                                                                                     \
{                                                                                      \
  if (*state_id[NETMGR_SA_DIR_##DIR] != '\0')                                          \
  {                                                                                    \
    char xfrm_state_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";                               \
    char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                                      \
    DS_SYSCALL_DEFAULTS(opts);                                                         \
    opts.stderr = err_str;                                                             \
    opts.stderr_length = sizeof(err_str);                                              \
    opts.log_options |= DS_EXEC_LOG_EXECTIME;                                          \
                                                                                       \
    snprintf(xfrm_state_cmd,                                                           \
             sizeof(xfrm_state_cmd),                                                   \
             "ip xfrm state add %s mode %s %s %s flag af-unspec %s",                   \
             state_id[NETMGR_SA_DIR_##DIR],                                            \
             mode,                                                                     \
             enc,                                                                      \
             auth,                                                                     \
             encap[NETMGR_SA_DIR_##DIR]);                                              \
                                                                                       \
    if (netmgr_ds_exec(xfrm_state_cmd, &opts) != DS_EXEC_OK)                           \
    {                                                                                  \
      netmgr_log_err("%s(): ip xfrm state cmd failed, err: %s",                        \
                      __func__, err_str);                                              \
      goto bail;                                                                       \
    }                                                                                  \
  }                                                                                    \
  else                                                                                 \
  {                                                                                    \
    netmgr_log_err("ip xfrm state cmd failed, invalid state\n");                       \
    goto bail;                                                                         \
  }                                                                                    \
}                                                                                      \
while (0)

#define NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(DIR,ipsec_dir,mode,state_id,policy_sel)  \
do                                                                                         \
{                                                                                          \
  if (*policy_sel[NETMGR_SA_DIR_##DIR] != '\0' && *state_id[NETMGR_SA_DIR_##DIR] != '\0')  \
  {                                                                                        \
    char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";                                  \
    char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                                          \
    DS_SYSCALL_DEFAULTS(opts);                                                             \
    opts.stderr = err_str;                                                                 \
    opts.stderr_length = sizeof(err_str);                                                  \
    opts.log_options |= DS_EXEC_LOG_EXECTIME;                                              \
                                                                                           \
    /* 'priority' - Specify priority option for legacy policies */                         \
    /* The priority is required in order to install other specific policies which */       \
    /* should be enforced before the legacy rules. Higher the value lower the priority */  \
    snprintf(xfrm_policy_cmd,                                                              \
             sizeof(xfrm_policy_cmd),                                                      \
             "ip xfrm policy add dir "#ipsec_dir" priority 100 %s tmpl %s mode %s",        \
             policy_sel[NETMGR_SA_DIR_##DIR],                                              \
             state_id[NETMGR_SA_DIR_##DIR],                                                \
             mode);                                                                        \
                                                                                           \
    if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)                              \
    {                                                                                      \
      netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",                          \
                     __func__, xfrm_policy_cmd, err_str);                                  \
      goto bail;                                                                           \
    }                                                                                      \
  }                                                                                        \
  else                                                                                     \
  {                                                                                        \
    netmgr_log_err("ip xfrm policy cmd failed, invalid policy/state\n");                   \
    goto bail;                                                                             \
  }                                                                                        \
}                                                                                          \
while (0)

/* Macro to get the traffic selector mode */
#define NETMGR_KIF_GET_TS_MODE(ts_mode, num_tsi, num_tsr) \
/* If both TSi and TSr lengths are zero then we did */    \
/* receive any traffic selector TLVs and can set the */   \
/* ts mode to DISABLED. Otherwise we need to check */     \
/* the list lengths to determine the mode */              \
if (FALSE == netmgr_main_get_iwlan_traffic_sel_enabled()) \
{                                                         \
  /* If traffic sel property is disabled */               \
  /* Set the modem to disabled */                         \
  ts_mode = NETMGR_TS_MODE_DISABLED;                      \
}                                                         \
else if (num_tsi == 0 && num_tsr == 0)                    \
{                                                         \
  ts_mode = NETMGR_TS_MODE_DISABLED;                      \
}                                                         \
else if (num_tsi > 0 && num_tsr == 0)                     \
{                                                         \
  ts_mode = NETMGR_TS_MODE_TSI_ONLY;                      \
}                                                         \
else if (num_tsi == 0 && num_tsr > 0)                     \
{                                                         \
  ts_mode = NETMGR_TS_MODE_TSR_ONLY;                      \
}                                                         \
else if (num_tsi > 0 && num_tsr > 0)                      \
{                                                         \
  ts_mode = NETMGR_TS_MODE_BOTH_ENABLED;                  \
}

#define NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(ipsec_dir,mode,state_id,policy_sel)      \
do                                                                                            \
{                                                                                             \
  if (policy_sel[0] != '\0' && state_id[0] != '\0')                                           \
  {                                                                                           \
    char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";                                     \
    char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                                             \
    DS_SYSCALL_DEFAULTS(opts);                                                                \
    opts.stderr = err_str;                                                                    \
    opts.stderr_length = sizeof(err_str);                                                     \
    opts.log_options |= DS_EXEC_LOG_EXECTIME;                                                 \
                                                                                              \
    /* 'priority' - Specify priority option for legacy policies */                            \
    /* The priority is required in order to install other specific policies which */          \
    /* should be enforced before the legacy rules. Higher the value lower the priority */     \
    snprintf(xfrm_policy_cmd,                                                                 \
             sizeof(xfrm_policy_cmd),                                                         \
             "ip xfrm policy add dir "#ipsec_dir" priority 100 %s tmpl %s mode %s",           \
             policy_sel,                                                                      \
             state_id,                                                                        \
             mode);                                                                           \
                                                                                              \
    if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)                                 \
    {                                                                                         \
      netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",                             \
                     __func__, xfrm_policy_cmd, err_str);                                     \
      goto bail;                                                                              \
    }                                                                                         \
  }                                                                                           \
  else                                                                                        \
  {                                                                                           \
    netmgr_log_err("ip xfrm policy cmd failed, invalid policy/state\n");                      \
    goto bail;                                                                                \
  }                                                                                           \
}                                                                                             \
while (0)

#define NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(ipsec_dir,policy_sel)     \
do                                                                            \
{                                                                             \
  if (policy_sel[0] != '\0')                                                  \
  {                                                                           \
    char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";                     \
    char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";                             \
    DS_SYSCALL_DEFAULTS(opts);                                                \
    opts.stderr = err_str;                                                    \
    opts.stderr_length = sizeof(err_str);                                     \
    opts.log_options |= DS_EXEC_LOG_EXECTIME;                                 \
                                                                              \
    snprintf(xfrm_policy_cmd,                                                 \
             sizeof(xfrm_policy_cmd),                                         \
             "ip xfrm policy delete dir "#ipsec_dir" %s ",                    \
             policy_sel);                                                     \
                                                                              \
    if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)                 \
    {                                                                         \
      netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",             \
                     __func__, xfrm_policy_cmd, err_str);                     \
      goto bail;                                                              \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    netmgr_log_err("ip xfrm policy cmd failed, invalid policy/state\n");      \
    goto bail;                                                                \
  }                                                                           \
}                                                                             \
while (0)

/*===========================================================================
  FUNCTION  netmgr_kif_store_ts_params
===========================================================================*/
/*!
@brief
  Reads the traffic selector params and stores it within the SA structure

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
   - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_store_ts_params
(
  netmgr_address_info_t          *addr_info_ptr,
  wds_traffic_selector_type_v01  *ts_list,
  netmgr_ts_list_t               *netmgr_ts,
  int                            num_ts,
  boolean                        check_wildcard
)
{
  unsigned char        proto;
  char                 addr_range[NETMGR_MAX_STR_LENGTH] = "";
  int                  rc = NETMGR_FAILURE, i = 0;
  char                 iface_addr_buf[NETMGR_MAX_STR_LENGTH] = "";
  netmgr_ip_address_t  *ip_addr = NULL;
  unsigned int         prefix_len = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if (NULL == ts_list || NULL == netmgr_ts
      || num_ts == 0 || num_ts > NETMGR_MAX_TRAFFIC_SELS)
  {
    netmgr_log_err("%s(): Invalid params~\n", __func__);
    goto bail;
  }

  for (i = 0; i < num_ts; i++)
  {
    /* Read protocol information */
    proto = ts_list[i].protocol;
    if (proto == 0)
    {
      /* If protocol value is zero then as per RFC we need to allow all types
       * of protocols */
      netmgr_log_low("%s(): Protocol is unspecified!\n", __func__);
    }
    else
    {
      snprintf(netmgr_ts[i].proto,
               sizeof(netmgr_ts[i].proto),
               "%u",
               proto);
    }

    /* Currently port-range for traffic selectors is not supported. Continue processing
     * address range payload */

    /* Save the IP type of this payload */
    if (WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 == ts_list[i].start_address.addr_family
        || WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 == ts_list[i].end_address.addr_family)
    {
      netmgr_ts[i].ip_type = AF_INET;
    }
    else if (WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01 == ts_list[i].start_address.addr_family
             || WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01 == ts_list[i].end_address.addr_family)
    {
      netmgr_ts[i].ip_type = AF_INET6;
    }
    else
    {
      netmgr_log_err("%s(): Cannot determine addr family!\n", __func__);
      goto bail;
    }

    /* Get the address range */
    if (NETMGR_SUCCESS !=
         netmgr_util_convert_qmi_ip_addr_range_to_str_range( &ts_list[i].start_address,
                                                             &ts_list[i].end_address,
                                                             addr_range,
                                                             sizeof(addr_range) ))
    {
      netmgr_log_err("%s(): Failed to parse address range!\n", __func__);
      goto bail;
    }
    else if (std_strlen(addr_range) == 0)
    {
      netmgr_log_low("%s(): Address range not provided in TS payload!\n",
                     __func__);
    }
    else
    {
      /* Copy the address range into SA structure */
      netmgr_log_med("%s(): Address range: %s",
                     __func__, addr_range);

      /* TSi should usually have a valid address range however sometimes the
       * server can be configured to have wildcards ex. 0.0.0.0/0 which means
       * the entire range. However, this configuration is not expected.
       * In this case we will use rmnet address in the policy. */
      if ( TRUE == check_wildcard
             &&
            ( !strcmp(addr_range, TRAFFIC_SEL_WILDCARD_V4)
               || !strcmp(addr_range, TRAFFIC_SEL_WILDCARD_V6) )
         )
      {
        if (!addr_info_ptr)
        {
          netmgr_log_err("%s(): Cannot extract interface address from invalid addr_info_ptr!\n",
                         __func__);
          goto bail;
        }

        /* Check address type to retrieve the proper address from structure
         * either AF_INET or AF_INET6 */
        if (AF_INET == netmgr_ts[i].ip_type)
        {
          ip_addr = &addr_info_ptr->ipv4.if_addr;
          prefix_len = (unsigned int)ds_get_num_bits_set_count(addr_info_ptr->ipv4.if_mask);
        }
        else
        {
          ip_addr = &addr_info_ptr->ipv6.if_addr;
          prefix_len = addr_info_ptr->ipv6.if_mask;
        }

        /* Convert ip_addr to string. This is the rmnet interface address */
        if ( NETMGR_SUCCESS !=
               netmgr_util_convert_ip_addr_to_str( ip_addr,
                                                   prefix_len,
                                                   iface_addr_buf,
                                                   sizeof(iface_addr_buf) ) )
        {
          netmgr_log_err("%s(): Failed to convert iface addr to string!\n", __func__);
          goto bail;
        }

        /* Save address in TSi structure */
        strlcpy(netmgr_ts[i].addr_range,
                iface_addr_buf,
                sizeof(netmgr_ts[i].addr_range));
      }
      else
      {
        /* Address is not a wildcard or is a TSr addr */
        strlcpy(netmgr_ts[i].addr_range,
                addr_range,
                sizeof(netmgr_ts[i].addr_range));
      }
    }
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
===========================================================================*/
/*!
@brief
  Returns the IPSec policy selector string to be used with the ip xfrm
  policy command

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
(
  char         *src_addr,
  char         *dst_addr,
  char         *proto,
  char         *policy_sel_buf,
  unsigned int policy_sel_buf_len
)
{
  int rc = NETMGR_FAILURE;
  int str_length = 0;

  NETMGR_LOG_FUNC_ENTRY;

  /* If the buffer is not correct or both src and dst
   * addresses provided are NULL then we cannot prepare
   * the policy selector. Protocol is optional */
  if (!policy_sel_buf || 0 == policy_sel_buf_len
      || (!src_addr && !dst_addr))
  {
    netmgr_log_err("%s(): Invalid input!\n", __func__);
    goto bail;
  }

  if (NULL != src_addr && NULL != dst_addr)
  {
    /* Both src and dst ranges are valid */
    str_length = snprintf(policy_sel_buf,
                          policy_sel_buf_len,
                          "src %s dst %s",
                          src_addr,
                          dst_addr);
  }
  else if (NULL != dst_addr)
  {
    /* Only dst address is valid */
    str_length = snprintf(policy_sel_buf,
                          policy_sel_buf_len,
                          "dst %s",
                          dst_addr);
  }
  else if (NULL != src_addr)
  {
    /* Only src addr is valid */
    str_length = snprintf(policy_sel_buf,
                          policy_sel_buf_len,
                          "src %s",
                          src_addr);
  }

  /* Check if protocol information is provided, if yes append it
   * to the selector */
  if (NULL != proto && std_strlen(proto) != 0)
  {
    snprintf((policy_sel_buf + str_length),
             (policy_sel_buf_len - str_length),
             " proto %s",
             proto);
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_install_traffic_selector_policies
===========================================================================*/
/*!
@brief
  Installs the XFRM policies based on traffic selector criteria

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_install_traffic_selector_policies
(
  netmgr_sa_ts_t         *netmgr_tsi,
  netmgr_sa_ts_t         *netmgr_tsr,
  netmgr_ts_mode_t       ts_mode,
  netmgr_ipsec_sa_dir_t  sa_dir,
  const char             *mode,
  char                   *sa_state_id,
  int                    ip_family
)
{
  int   i = 0, j = 0;
  int   rc = NETMGR_FAILURE, reti = NETMGR_SUCCESS;
  char  sa_policy_sel[NETMGR_MAX_STR_LENGTH] = "";

  NETMGR_LOG_FUNC_ENTRY;

  if ( (NULL == netmgr_tsi && NULL == netmgr_tsr)
       || (NULL == sa_state_id))
  {
    netmgr_log_err("%s(): Invalid params!\n", __func__);
    goto bail;
  }

  netmgr_log_med("%s(): Installing traffic selector policies", __func__);

  switch (ts_mode)
  {
  case NETMGR_TS_MODE_BOTH_ENABLED:
    {
      /* In this mode neither TSi nor TSr can be NULL */
      if (NULL == netmgr_tsi || NULL == netmgr_tsr)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If the traffic is outgoing, we need to use the source addresses from the TSi
         * list and the dest addresses from TSr list */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          for (j = 0; j < netmgr_tsr->num_ts; j++)
          {
            /* Check the ip type of TSi and TSr */
            /* If the types don't match, skip intallation */
            if (netmgr_tsi->ts_list[i].ip_type
                 != netmgr_tsr->ts_list[j].ip_type)
            {
              continue;
            }

             /* If they match but don't equal ip_family, skip installation */
            if ( (netmgr_tsi->ts_list[i].ip_type
                   == netmgr_tsr->ts_list[j].ip_type)
                  && (netmgr_tsi->ts_list[i].ip_type != ip_family) )
            {
              continue;
            }

            if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[j].addr_range,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
            {
              netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
              reti = NETMGR_FAILURE;
              break;
            }

            /* Install the xfrm policy */
            NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(out, mode, sa_state_id, sa_policy_sel);
          } /* 'j' loop*/
        } /* 'i' loop */
      }
      else
      {
        /* If the traffic is incoming, we need to use the dest addresses from the TSi
         * list and the src addresses from TSr list */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          for (j = 0; j < netmgr_tsr->num_ts; j++)
          {
            /* Check the ip type of TSi and TSr */
            /* If the types don't match, skip intallation */
            if (netmgr_tsi->ts_list[i].ip_type
                 != netmgr_tsr->ts_list[j].ip_type)
            {
              continue;
            }

             /* If they match but don't equal ip_family, skip installation */
            if ( (netmgr_tsi->ts_list[i].ip_type
                   == netmgr_tsr->ts_list[j].ip_type)
                  && (netmgr_tsi->ts_list[i].ip_type != ip_family) )
            {
              continue;
            }

            if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsr->ts_list[j].addr_range,
                                                  netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[j].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
            {
              netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
              reti = NETMGR_FAILURE;
              break;
            }

            /* Install the xfrm policy */
            NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(in, mode, sa_state_id, sa_policy_sel);
            NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(fwd, mode, sa_state_id, sa_policy_sel);
          } /* 'j' loop */
        } /* 'i' loop */
      } /* if (NETMGR_SA_DIR_TX == sa_dir) */
    } /* NETMGR_TS_MODE_BOTH_ENABLED */
    break;

  case NETMGR_TS_MODE_TSI_ONLY:
    {
      /* In this mode TSi cannot be NULL */
      if (NULL == netmgr_tsi)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If only TSi is enabled, then for outgoing direction we will install
         * such that the policy selector will use the src address of TSi */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsi->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsi->ts_list[i].addr_range,
                                                  NULL,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Install the xfrm policy */
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(out, mode, sa_state_id, sa_policy_sel);
        }
      }
      else
      {
        /* For incoming traffic, the destination criteria will be TSi */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsi->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (NULL,
                                                  netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Install the xfrm policy */
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(in, mode, sa_state_id, sa_policy_sel);
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(fwd, mode, sa_state_id, sa_policy_sel);
        }
      }
    }
    break;

  case NETMGR_TS_MODE_TSR_ONLY:
    {
      /* In this mode TSr cannot be NULL */
      if (NULL == netmgr_tsr)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If only TSr is enabled, then for outgoing direction we will install
         * policies such that the policy selector will use the dst address of TSr */
        for (i = 0; i < netmgr_tsr->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsr->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (NULL,
                                                  netmgr_tsr->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Install the xfrm policy */
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(out, mode, sa_state_id, sa_policy_sel);
        }
      }
      else
      {
        /* For incoming traffic, the src criteria will be TSr */
        for (i = 0; i < netmgr_tsr->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsr->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsr->ts_list[i].addr_range,
                                                  NULL,
                                                  netmgr_tsr->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Install the xfrm policy */
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(in, mode, sa_state_id, sa_policy_sel);
          NETMGR_KIF_UTIL_INSTALL_TS_IPSEC_XFRM_POLICY(fwd, mode, sa_state_id, sa_policy_sel);
        }
      }
    }
    break;

  case NETMGR_TS_MODE_DISABLED:
  default:
    netmgr_log_err("%s(): Traffic selector mode is set to disabled!\n", __func__);
    break;
  }

  if (NETMGR_SUCCESS == reti)
  {
    rc = NETMGR_SUCCESS;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_remove_traffic_selector_policies
===========================================================================*/
/*!
@brief
  Removes the XFRM policies for traffic selectors

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_remove_traffic_selector_policies
(
  netmgr_sa_ts_t         *netmgr_tsi,
  netmgr_sa_ts_t         *netmgr_tsr,
  netmgr_ts_mode_t       ts_mode,
  netmgr_ipsec_sa_dir_t  sa_dir,
  int                    ip_family
)
{
  int   i = 0, j = 0;
  int   rc = NETMGR_FAILURE, reti = NETMGR_SUCCESS;
  char  sa_policy_sel[NETMGR_MAX_STR_LENGTH] = "";

  NETMGR_LOG_FUNC_ENTRY;

  if ( (NULL == netmgr_tsi && NULL == netmgr_tsr) )
  {
    netmgr_log_err("%s(): Invalid params!\n", __func__);
    goto bail;
  }

  netmgr_log_med("%s(): Removing traffic selector policies", __func__);

  switch (ts_mode)
  {
  case NETMGR_TS_MODE_BOTH_ENABLED:
    {
      /* In this mode neither TSi nor TSr can be NULL */
      if (NULL == netmgr_tsi || NULL == netmgr_tsr)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If the traffic is outgoing, we need to use the source addresses from the TSi
         * list and the dest addresses from TSr list */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          for (j = 0; j < netmgr_tsr->num_ts; j++)
          {
            /* Check the ip type of TSi and TSr */
            /* If the types don't match, skip intallation */
            if (netmgr_tsi->ts_list[i].ip_type
                 != netmgr_tsr->ts_list[j].ip_type)
            {
              continue;
            }

             /* If they match but don't equal ip_family, skip installation */
            if ( (netmgr_tsi->ts_list[i].ip_type
                   == netmgr_tsr->ts_list[j].ip_type)
                  && (netmgr_tsi->ts_list[i].ip_type != ip_family) )
            {
              continue;
            }

            if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[j].addr_range,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
            {
              netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
              reti = NETMGR_FAILURE;
              break;
            }

            /* Remove the xfrm policy */
            NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(out, sa_policy_sel);
          } /* 'j' loop*/
        } /* 'i' loop */
      }
      else
      {
        /* If the traffic is incoming, we need to use the dest addresses from the TSi
         * list and the src addresses from TSr list */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          for (j = 0; j < netmgr_tsr->num_ts; j++)
          {
            /* Check the ip type of TSi and TSr */
            /* If the types don't match, skip intallation */
            if (netmgr_tsi->ts_list[i].ip_type
                 != netmgr_tsr->ts_list[j].ip_type)
            {
              continue;
            }

            /* If they match but don't equal ip_family, skip installation */
            if ( (netmgr_tsi->ts_list[i].ip_type
                   == netmgr_tsr->ts_list[j].ip_type)
                  && (netmgr_tsi->ts_list[i].ip_type != ip_family) )
            {
              continue;
            }

            if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsr->ts_list[j].addr_range,
                                                  netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[j].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
            {
              netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
              reti = NETMGR_FAILURE;
              break;
            }

            /* Remove the xfrm policy */
            NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(in, sa_policy_sel);
            NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(fwd, sa_policy_sel);
          } /* 'j' loop */
        } /* 'i' loop */
      } /* if (NETMGR_SA_DIR_TX == sa_dir) */
    } /* NETMGR_TS_MODE_BOTH_ENABLED */
    break;

  case NETMGR_TS_MODE_TSI_ONLY:
    {
      /* In this mode TSi cannot be NULL */
      if (NULL == netmgr_tsi)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If only TSi is enabled, then for outgoing direction we will install
         * such that the policy selector will use the src address of TSi */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsi->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsi->ts_list[i].addr_range,
                                                  NULL,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Remove the xfrm policy */
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(out, sa_policy_sel);
        }
      }
      else
      {
        /* For incoming traffic, the destination criteria will be TSi */
        for (i = 0; i < netmgr_tsi->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsi->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (NULL,
                                                  netmgr_tsi->ts_list[i].addr_range,
                                                  netmgr_tsi->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Remove the xfrm policy */
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(in, sa_policy_sel);
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(fwd, sa_policy_sel);
        }
      }
    }
    break;

  case NETMGR_TS_MODE_TSR_ONLY:
    {
      /* In this mode TSr cannot be NULL */
      if (NULL == netmgr_tsr)
      {
        netmgr_log_err("%s(): Invalid parameters received!\n", __func__);
        reti = NETMGR_FAILURE;
        break;
      }

      if (NETMGR_SA_DIR_TX == sa_dir)
      {
        /* If only TSr is enabled, then for outgoing direction we will install
         * policies such that the policy selector will use the dst address of TSr */
        for (i = 0; i < netmgr_tsr->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsr->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (NULL,
                                                  netmgr_tsr->ts_list[i].addr_range,
                                                  netmgr_tsr->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Remove the xfrm policy */
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(out, sa_policy_sel);
        }
      }
      else
      {
        /* For incoming traffic, the src criteria will be TSr */
        for (i = 0; i < netmgr_tsr->num_ts; i++)
        {
          /* If the ip types don't match, skip installation */
          if ( netmgr_tsr->ts_list[i].ip_type != ip_family )
          {
            continue;
          }

          if (NETMGR_SUCCESS
                 != netmgr_kif_get_traffic_selector_xfrm_policy_sel_str
                                                 (netmgr_tsr->ts_list[i].addr_range,
                                                  NULL,
                                                  netmgr_tsr->ts_list[i].proto,
                                                  sa_policy_sel,
                                                  NETMGR_MAX_STR_LENGTH))
          {
            netmgr_log_err("%s(): Failed to get traffic selector policy sel!\n", __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Remove the xfrm policy */
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(in, sa_policy_sel);
          NETMGR_KIF_UTIL_REMOVE_TS_IPSEC_XFRM_POLICY(fwd, sa_policy_sel);
        }
      }
    }
    break;

  case NETMGR_TS_MODE_DISABLED:
  default:
    netmgr_log_err("%s(): Traffic selector mode is set to disabled!\n", __func__);
    break;
  }

  if (NETMGR_SUCCESS == reti)
  {
    rc = NETMGR_SUCCESS;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_kif_install_sa_rules
===========================================================================*/
/*!
@brief
  Installs the SAs to the SAD (Security Association Database) and
  SPD (Security Policy Database) for the given family and link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_install_sa_rules
(
  int link,
  int ip_family
)
{
  char encryption[NETMGR_MAX_STR_LENGTH]     = "";
  char authentication[NETMGR_MAX_STR_LENGTH] = "";
  char encap[NETMGR_SA_DIR_MAX][NETMGR_MAX_STR_LENGTH] = {"",""};
  const char *proto = NULL;
  const char *mode  = NULL;
  netmgr_ipsec_sa_t  *sa = NULL;
  netmgr_ip_addr_t   ip_type;
  netmgr_address_info_t  *addr_info_ptr = netmgr_qmi_get_addr_info(link);
  int rc = NETMGR_FAILURE;
  unsigned int prefix_len = 0;
  wds_get_ipsec_static_sa_config_resp_msg_v01  sa_config;

  NETMGR_LOG_FUNC_ENTRY;

  if (!addr_info_ptr)
  {
    netmgr_log_err("netmgr_kif_install_sa_rules: invalid input\n");
    goto bail;
  }

  if (AF_INET != ip_family && AF_INET6 != ip_family)
  {
    netmgr_log_err("%s(): invalid ip_famliy=%d\n",
                   __func__, ip_family);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_query_ipsec_sa_config(ip_family, link, &sa_config))
  {
    netmgr_log_err("%s(): failed to query SAs for ip_family=%d, link=%d",
                   __func__,
                   ip_family,
                   link);
    goto bail;
  }

  /* Make sure that the basic parameters are present */
  if (!sa_config.ipsec_sa_protocol_valid  ||
      !sa_config.encapsulation_mode_valid ||
      !sa_config.dest_addr_valid          ||
      !sa_config.local_addr_valid         ||
      !sa_config.spi_rx_valid             ||
      !sa_config.spi_tx_valid             ||
      !sa_config.hash_algo_valid          ||
      !sa_config.crypto_algo_valid)
  {
    netmgr_log_err("%s(): SA missing key parameters", __func__);
    goto bail;
  }

  if (AF_INET == ip_family)
  {
    ip_type = NETMGR_IPV4_ADDR;
    sa = &addr_info_ptr->ipv4.sa;
  }
  else
  {
    ip_type = NETMGR_IPV6_ADDR;
    sa = &addr_info_ptr->ipv6.sa;
  }

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ip_addr_to_str(&sa_config.dest_addr,
                                                               sa->tunnel_ep.dest_addr,
                                                               sizeof(sa->tunnel_ep.dest_addr)))
  {
    netmgr_log_err("%s(): failed to convert dest IP addr to string", __func__);
    goto bail;
  }

  netmgr_log_med("%s(): dest_addr=%s", __func__, sa->tunnel_ep.dest_addr);

  if (NETMGR_SUCCESS != netmgr_util_convert_qmi_ip_addr_to_str(&sa_config.local_addr,
                                                               sa->tunnel_ep.local_addr,
                                                               sizeof(sa->tunnel_ep.local_addr)))
  {
    netmgr_log_err("%s(): failed to convert local IP addr to string", __func__);
    goto bail;
  }

  netmgr_log_med("%s(): local_addr=%s", __func__, sa->tunnel_ep.local_addr);

  sa->tunnel_ep.ip_family = (WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 ==
                             sa_config.local_addr.addr_family) ? AF_INET :
                                                                 AF_INET6;
  sa->tunnel_ep.is_valid = TRUE;

  /* Get the protocol string */
  if (!(proto = netmgr_util_get_ipsec_proto_str(sa_config.ipsec_sa_protocol)))
  {
    netmgr_log_err("%s(): failed to get proto string", __func__);
    goto bail;
  }

  /* Get the encapsulation mode string */
  if (!(mode = netmgr_util_get_ipsec_mode_str(sa_config.encapsulation_mode)))
  {
    netmgr_log_err("%s(): failed to get mode string", __func__);
    goto bail;
  }

  /* Get the ESP in UDP encapsulation string (if any) in TX and RX directions */
  if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_encap_str(&sa_config,
                                                            NETMGR_SA_DIR_TX,
                                                            encap[NETMGR_SA_DIR_TX],
                                                            sizeof(encap[NETMGR_SA_DIR_TX])))
  {
    netmgr_log_err("%s(): failed to get encap string in TX dir", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_encap_str(&sa_config,
                                                            NETMGR_SA_DIR_RX,
                                                            encap[NETMGR_SA_DIR_RX],
                                                            sizeof(encap[NETMGR_SA_DIR_RX])))
  {
    netmgr_log_err("%s(): failed to get encap string in RX dir", __func__);
    goto bail;
  }

  /* Save the encapsulation mode */
  sa->mode = mode;

  /* Check if traffic selector TLVs are enabled */
  if (sa_config.traffic_selector_list_valid)
  {
    /* Get the number of TSi selectors */
    sa->sa_tsi.num_ts = sa_config.traffic_selector_list_len;
  }

  if (sa_config.traffic_selector_responder_list_valid)
  {
    /* Get the number of TSr selectors */
    sa->sa_tsr.num_ts = sa_config.traffic_selector_responder_list_len;
  }

  /* Get the traffic selector mode */
  NETMGR_KIF_GET_TS_MODE(sa->ts_mode, sa->sa_tsi.num_ts, sa->sa_tsr.num_ts);

  /* Parse the traffic selector information and store
   * for processing */
  if (NETMGR_TS_MODE_TSI_ONLY == sa->ts_mode ||
      NETMGR_TS_MODE_BOTH_ENABLED == sa->ts_mode)
  {
    /* Store TSi */
    /* If TSi addresses are wildcards we need to use rmnet address instead */
    netmgr_log_low("%s(): Processing TSi information", __func__);
    if (NETMGR_SUCCESS != netmgr_kif_store_ts_params(addr_info_ptr,
                                                     sa_config.traffic_selector_list,
                                                     sa->sa_tsi.ts_list,
                                                     sa->sa_tsi.num_ts,
                                                     TRUE))
    {
      netmgr_log_err("%s(): Failed to process TSi information!", __func__);
    }
  }

  if (NETMGR_TS_MODE_TSR_ONLY == sa->ts_mode ||
      NETMGR_TS_MODE_BOTH_ENABLED == sa->ts_mode)
  {
    /* Store TSr */
    /* Wildcard addresses for TSr is acceptable */
    netmgr_log_low("%s(): Processing TSr information", __func__);
    if (NETMGR_SUCCESS != netmgr_kif_store_ts_params(addr_info_ptr,
                                                     sa_config.traffic_selector_responder_list,
                                                     sa->sa_tsr.ts_list,
                                                     sa->sa_tsr.num_ts,
                                                     FALSE))
    {
      netmgr_log_err("%s(): Failed to process TSr information!", __func__);
    }
  }

  /* Install the SA in the 'out' direction */
  if (sa_config.spi_tx_valid)
  {
    netmgr_ipsec_sa_dir_t sa_dir = NETMGR_SA_DIR_TX;

    snprintf(sa->sa_state_id[sa_dir],
             NETMGR_MAX_STR_LENGTH,
             "src %s dst %s proto %s spi 0x%x",
             sa->tunnel_ep.local_addr,
             sa->tunnel_ep.dest_addr,
             proto,
             sa_config.spi_tx);

    /* Save the SPI value. This would be used to compare against
     * the SPI value returned by the kernel xfrm netlink message
     * when sequence number rolls over */
    if (AF_INET == ip_family)
    {
      addr_info_ptr->ipv4.sa.esp_spi_v4 = sa_config.spi_tx;
    }
    else
    {
      addr_info_ptr->ipv6.sa.esp_spi_v6 = sa_config.spi_tx;
    }

    if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_encryption_str(&sa_config,
                                                                   sa_dir,
                                                                   encryption,
                                                                   sizeof(encryption)))
    {
      netmgr_log_err("%s(): failed to get auth string", __func__);
      goto bail;
    }

    if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_authentication_str(&sa_config,
                                                                       sa_dir,
                                                                       authentication,
                                                                       sizeof(authentication)))
    {
      netmgr_log_err("%s(): failed to get auth string", __func__);
      goto bail;
    }

    /* Install the 'state' and 'policy' in the "out" direction */
    NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_STATE(TX,mode,sa->sa_state_id,encryption,authentication,encap);

    if (NETMGR_TS_MODE_DISABLED == sa->ts_mode)
    {
      /* Install the default policies if traffic selector is disabled */
      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               ip_type,
                                                               sa_dir,
                                                               sa->sa_policy_sel[sa_dir],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("%s(): failed to get xfrm policy sel string", __func__);
        goto bail;
      }

      NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(TX, out, mode, sa->sa_state_id, sa->sa_policy_sel);
    }
    else
    {
      /* Install traffic selector specific policies in 'out' direction */
      if (NETMGR_SUCCESS != netmgr_kif_install_traffic_selector_policies(&sa->sa_tsi,
                                                                         &sa->sa_tsr,
                                                                         sa->ts_mode,
                                                                         sa_dir,
                                                                         sa->mode,
                                                                         sa->sa_state_id[sa_dir],
                                                                         ip_family))
      {
        netmgr_log_err("%s(): Failed to install traffic selector policies in 'out' direction!\n",
                       __func__);
        goto bail;
      }
    }
  }

  /* Install the SA in the 'in' direction */
  if (sa_config.spi_rx_valid)
  {
    netmgr_ipsec_sa_dir_t sa_dir = NETMGR_SA_DIR_RX;

    snprintf(sa->sa_state_id[sa_dir],
             NETMGR_MAX_STR_LENGTH,
             "src %s dst %s proto %s spi 0x%x",
             sa->tunnel_ep.dest_addr,
             sa->tunnel_ep.local_addr,
             proto,
             sa_config.spi_rx);

    if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_encryption_str(&sa_config,
                                                                   sa_dir,
                                                                   encryption,
                                                                   sizeof(encryption)))
    {
      netmgr_log_err("%s(): failed to get encryption string", __func__);
      goto bail;
    }

    if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_state_authentication_str(&sa_config,
                                                                       sa_dir,
                                                                       authentication,
                                                                       sizeof(authentication)))
    {
      netmgr_log_err("%s(): failed to get auth string", __func__);
      goto bail;
    }

    /* Install the 'state' and 'policy' in the "in" direction and also allow forwarding */
    NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_STATE(RX,mode,sa->sa_state_id,encryption,authentication,encap);

    if (NETMGR_TS_MODE_DISABLED == sa->ts_mode)
    {
      /* Get the default policy selector */
      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               ip_type,
                                                               sa_dir,
                                                               sa->sa_policy_sel[sa_dir],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("%s(): failed to get xfrm policy sel string", __func__);
        goto bail;
      }

      /* Install the default policies */
      NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX, in, mode, sa->sa_state_id, sa->sa_policy_sel);
      NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX,fwd,mode,sa->sa_state_id,sa->sa_policy_sel);
    }
    else
    {
      if (NETMGR_SUCCESS != netmgr_kif_install_traffic_selector_policies(&sa->sa_tsi,
                                                                         &sa->sa_tsr,
                                                                         sa->ts_mode,
                                                                         sa_dir,
                                                                         sa->mode,
                                                                         sa->sa_state_id[sa_dir],
                                                                         ip_family))
      {
        netmgr_log_err("%s(): Failed to install traffic selector policies in"
                       " 'in' and 'fwd' directions!\n",
                       __func__);
        goto bail;
      }
    }
  }

  sa->is_sa_valid = TRUE;

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}


/*===========================================================================
  FUNCTION  netmgr_kif_remove_sa_rules
===========================================================================*/
/*!
@brief
  Removes the SAs from the SAD (Security Association Database) and
  SPD (Security Policy Database) for the given link and family

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_kif_remove_sa_rules
(
  int link,
  int ip_family
)
{
  char xfrm_state_cmd[NETMGR_MAX_COMMAND_LENGTH]  = "";
  char xfrm_policy_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";
  netmgr_ip_address_t  *ip_addr = NULL;
  netmgr_address_info_t  *addr_info_ptr = netmgr_qmi_get_addr_info(link);
  int rc = NETMGR_FAILURE;
  char (*sa_state_id)[NETMGR_MAX_STR_LENGTH]   = NULL;
  char (*sa_policy_sel)[NETMGR_MAX_STR_LENGTH] = NULL;
  netmgr_ipsec_sa_t  *sa = NULL;
  netmgr_ipsec_sa_t  *v4_sa = NULL;
  netmgr_ipsec_sa_t  *v6_sa = NULL;
  boolean remove_sa = TRUE;

  NETMGR_LOG_FUNC_ENTRY;

  if (!addr_info_ptr)
  {
    netmgr_log_err("%s(): invalid input", __func__);
    goto bail;
  }

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  v4_sa = &addr_info_ptr->ipv4.sa;
  v6_sa = &addr_info_ptr->ipv6.sa;

  if (AF_INET == ip_family)
  {
    sa = v4_sa;
    ip_addr = &addr_info_ptr->ipv4.if_addr;
    sa_state_id   = sa->sa_state_id;
    sa_policy_sel = sa->sa_policy_sel;

    /* If the SA is shared and the other family is still UP, don't remove the SA */
    if (sa->is_sa_shared && v6_sa->is_sa_valid)
    {
      remove_sa = FALSE;
    }
  }
  else
  {
    sa = v6_sa;
    ip_addr = &addr_info_ptr->ipv6.if_addr;
    sa_state_id   = sa->sa_state_id;
    sa_policy_sel = sa->sa_policy_sel;

    /* If the SA is shared and the other family is still UP, don't remove the SA */
    if (sa->is_sa_shared && v4_sa->is_sa_valid)
    {
      netmgr_log_med("%s(): SA shared, not removing SA state link=%d, family=%d",
                     __func__, link, ip_family);
      remove_sa = FALSE;
    }
  }

  if (TRUE != sa->is_sa_valid)
  {
    netmgr_log_err("%s(): SA is invalid for ip_family=%d, link=%d",
                   __func__,
                   ip_family,
                   link);
    goto bail;
  }

  if (TRUE == remove_sa && *sa_state_id[NETMGR_SA_DIR_TX] != '\0')
  {
    snprintf(xfrm_state_cmd,
             sizeof(xfrm_state_cmd),
             "ip xfrm state deleteall %s",
             sa_state_id[NETMGR_SA_DIR_TX]);

    netmgr_log_med("%s(): removing TX SA state for link=%d, family=%d",
                   __func__, link, ip_family);

    if (netmgr_ds_exec(xfrm_state_cmd, &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("%s(): xfrm_state_cmd: %s failed, err: %s",
                     __func__, xfrm_state_cmd, err_str);
      goto bail;
    }

    *sa_state_id[NETMGR_SA_DIR_TX] = '\0';
  }
  else
  {
    *sa_state_id[NETMGR_SA_DIR_TX] = '\0';
  }

  if (TRUE == remove_sa && *sa_state_id[NETMGR_SA_DIR_RX] != '\0')
  {
    snprintf(xfrm_state_cmd,
             sizeof(xfrm_state_cmd),
             "ip xfrm state deleteall %s",
             sa_state_id[NETMGR_SA_DIR_RX]);

    netmgr_log_med("%s(): removing RX SA state for link=%d, family=%d",
                   __func__, link, ip_family);

    if (netmgr_ds_exec(xfrm_state_cmd, &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("%s(): xfrm_state_cmd: %s failed, err: %s",
                     __func__, xfrm_state_cmd, err_str);
      goto bail;
    }

    *sa_state_id[NETMGR_SA_DIR_RX] = '\0';
  }
  else
  {
    *sa_state_id[NETMGR_SA_DIR_RX] = '\0';
  }

  /* Check if traffic selector mode is enabled */
  if (FALSE == netmgr_main_get_iwlan_traffic_sel_enabled()
        && NETMGR_TS_MODE_DISABLED == sa->ts_mode)
  {
    /* Traffic selectors are disabled, follow default flow */
    if (*sa_policy_sel[NETMGR_SA_DIR_TX] != '\0')
    {
      snprintf(xfrm_policy_cmd,
               sizeof(xfrm_policy_cmd),
               "ip xfrm policy delete dir out %s",
               sa_policy_sel[NETMGR_SA_DIR_TX]);

      netmgr_log_med("%s(): removing TX SA policy for link=%d, family=%d",
                     __func__, link, ip_family);

      if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)
      {
        netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",
                       __func__, xfrm_policy_cmd, err_str);
        goto bail;
      }

      *sa_policy_sel[NETMGR_SA_DIR_TX] = '\0';
    }

    if (*sa_policy_sel[NETMGR_SA_DIR_RX] != '\0')
    {
      snprintf(xfrm_policy_cmd,
               sizeof(xfrm_policy_cmd),
               "ip xfrm policy delete dir in %s",
               sa_policy_sel[NETMGR_SA_DIR_RX]);

      netmgr_log_med("%s(): removing RX SA policy for link=%d, family=%d",
                     __func__, link, ip_family);

      if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)
      {
        netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",
                       __func__, xfrm_policy_cmd, err_str);
        goto bail;
      }

      snprintf(xfrm_policy_cmd,
               sizeof(xfrm_policy_cmd),
               "ip xfrm policy delete dir fwd %s",
               sa_policy_sel[NETMGR_SA_DIR_RX]);

      netmgr_log_med("%s(): removing FWD SA policy for link=%d, family=%d",
                     __func__, link, ip_family);

      if (netmgr_ds_exec(xfrm_policy_cmd, &opts) != DS_EXEC_OK)
      {
        netmgr_log_err("%s(): xfrm_policy_cmd: %s failed, err: %s",
                       __func__, xfrm_policy_cmd, err_str);
        goto bail;
      }

      *sa_policy_sel[NETMGR_SA_DIR_RX] = '\0';
    }
  }
  else
  {
    /* Remove the traffic selector specific policies */
    if (NETMGR_SUCCESS
         != netmgr_kif_remove_traffic_selector_policies(&sa->sa_tsi,
                                                        &sa->sa_tsr,
                                                        sa->ts_mode,
                                                        NETMGR_SA_DIR_TX,
                                                        ip_family))
    {
      netmgr_log_err("%s(): Failed to remove policies in TX dir!\n", __func__);
      goto bail;
    }

    if (NETMGR_SUCCESS
         != netmgr_kif_remove_traffic_selector_policies(&sa->sa_tsi,
                                                        &sa->sa_tsr,
                                                        sa->ts_mode,
                                                        NETMGR_SA_DIR_RX,
                                                        ip_family))
    {
      netmgr_log_err("%s(): Failed to remove policies in RX dir!\n", __func__);
      goto bail;
    }
  }

  sa->is_sa_valid = FALSE;
  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION netmgr_kif_set_ssdp_rule_on_iface
===========================================================================*/
/*!
@brief
  Block SSDP broadcast packets from being trandmitten on an interface

@return
  None

@param *dev_name ASCII string representation of network device name

@note
  Underlying ds_system_call() will return 0 even on failure of system command.
  As such, this will return void because there is no way to determine a failure
  condition.
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_set_ssdp_rule_on_iface
(
  const char *dev_name
)
{
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  int cmdlen, rc;

  if (!netmgr_main_cfg.dropssdp)
    return;

  memset(cmd, 0, sizeof(cmd));
  cmdlen = snprintf(cmd,
                    sizeof(cmd),
                    "iptables %s -I OUTPUT -o %s -p udp --dport 1900 -j DROP",
                    iptables_opts,
                    dev_name);
  if (cmdlen == NETMGR_KIF_SYSCMD_SIZ)
  {
    netmgr_log_err("%s(): Command buffer overflow", __func__);
    return;
  }

  rc = netmgr_ds_exec( cmd, NULL );
  netmgr_log_high("%s(): Command returns %d. cmd: %s", __func__, rc, cmd);
}

/*===========================================================================
  FUNCTION netmgr_kif_remove_ssdp_rule_on_iface
===========================================================================*/
/*!
@brief
  Remove the SSD drop rule from the ip table.

@return
  None

@param *dev_name ASCII string representation of network device name

@note
  Underlying ds_system_call() will return 0 even on failure of system command.
  As such, this will return void because there is no way to determine a failure
  condition.
*/
/*=========================================================================*/
LOCAL void
netmgr_kif_remove_ssdp_rule_on_iface
(
  unsigned int link
)
{
  const char* link_name = NULL;
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  int cmdlen, rc;

  if (!netmgr_main_cfg.dropssdp)
    return;

  link_name = netmgr_kif_get_name(link);
  if (NULL == link_name)
  {
    netmgr_log_err("%s: invalid params passed",__func__);
    return;
  }

  memset(cmd, 0, sizeof(cmd));
  cmdlen = snprintf(cmd,
                    sizeof(cmd),
                    "iptables %s -D OUTPUT -o %s -p udp --dport 1900 -j DROP",
                    iptables_opts,
                    link_name);
  if (cmdlen == NETMGR_KIF_SYSCMD_SIZ)
  {
    netmgr_log_err("%s(): Command buffer overflow", __func__);
    return;
  }
  rc = netmgr_ds_exec( cmd, NULL );
  netmgr_log_high("%s(): Command returns %d. cmd: %s", __func__, rc, cmd);
}


/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_kif_get_qos_header_format
===========================================================================*/
/*!
@brief
  Gets the value of the WDA data format property qos_header_format to be
  sent to the modem. qos_header_format is applicable only for when
  baseband is not mdm2 and netmgr_kif_ifioctl_set_qosmode() is implemented.
  This function checks if QoS is enabled.

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
)
{
  int retval = NETMGR_FAILURE;
  uint32_t qos_modes = 0;
  char dev_name[16];

  if ((phys_dev == NULL) || (data_format == NULL))
  {
    netmgr_log_err("Input parameters cannot be NULL");
    goto error;
  }

  data_format->qos_header_format_valid = FALSE;
  data_format->qos_header_format       = NETMGR_KIF_QOS_HEADER_DEFAULT;
  memset(dev_name, 0, sizeof(dev_name));

  if (!netmgr_main_get_qos_enabled())
  {
    netmgr_log_low("%s() There is no qos_header_format needed since qos is disabled", __func__);
    goto error;
  }
  /* If RMNET_IOCTL_SET_QOS_ENABLE succeeds, then 6 byte QoS header is set. This IOCTL will
   * fail on all Bolt PL's and beyond */
  if (netmgr_kif_ifioctl_set_qosmode(phys_dev,
                                     TRUE) != NETMGR_SUCCESS)
  {
    netmgr_log_high("netmgr_kif_ifioctl_set_qosmode() fails, we do not need to set QoS header");
    goto error;
  }

  if (!netmgr_main_get_rmnet_data_enabled())
  {
    netmgr_log_high("netmgr_main_get_rmnet_data_enabled() is disabled, using 6 byte QoS header");
    goto error;
  }

  /* We dont need the 6 byte QoS header set if RmNet data is enabled. */
  if (netmgr_kif_ifioctl_set_qosmode(phys_dev,
                                     FALSE) != NETMGR_SUCCESS)
  {
    netmgr_log_high("netmgr_kif_ifioctl_set_qosmode() fails");
    goto error;
  }

  if (netmgr_rmnet_query_vnd_name(vnd,
                                  dev_name,
                                  sizeof(dev_name)) != NETMGR_RMNET_SUCCESS)
  {
    netmgr_log_high("Unable to find device name for vnd %d",vnd);
    goto error;
  }
  if (netmgr_kif_ifioctl_get_supported_qos_modes(dev_name,
                                                 &qos_modes) != NETMGR_SUCCESS)
  {
    netmgr_log_err("netmgr_kif_ifioctl_get_supported_qos_modes() failed");
    goto error;
  }

  retval = NETMGR_SUCCESS;
  data_format->qos_header_format_valid = TRUE;
  if (qos_modes & NETMGR_KIF_QOS_HEADER_8_BYTE)
  {
    data_format->qos_header_format = NETMGR_KIF_QOS_HEADER_8_BYTE;
    netmgr_l2s_msg(netmgr_l2s_invalid_link,"8 byte QOS header");
  }
  else
  if (qos_modes & NETMGR_KIF_QOS_HEADER_6_BYTE)
  {
    data_format->qos_header_format = NETMGR_KIF_QOS_HEADER_6_BYTE;
    netmgr_l2s_msg(netmgr_l2s_invalid_link,"6 byte QOS header");
  }
  else
  {
    retval = NETMGR_FAILURE;
    data_format->qos_header_format_valid = FALSE;
  }

error:
  if (NETMGR_FAILURE == retval)
  {
    netmgr_l2s_msg(netmgr_l2s_invalid_link,"Set QOS hdr fmt failed");
  }
  return retval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_set_qos_header_format
===========================================================================*/
/*!
@brief
  Sets the qos_header_format for every virtual network device.
  This function checks if QoS is enabled.

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
)
{
  uint32_t vnd = 0;
  char vnd_name[16];
  int retval = NETMGR_FAILURE;
  if (!netmgr_main_get_qos_enabled())
  {
    netmgr_log_low("%s() QoS is disabled, there is no qos_header_format needed", __func__);
    goto success;
  }

  memset(vnd_name, 0, sizeof(vnd_name));

  if (netmgr_main_get_rmnet_data_enabled())
  {
    for (vnd = vnd_id; vnd < (uint32_t)data_format->num_mux_channels; vnd++)
    {
      if (netmgr_rmnet_query_vnd_name(vnd,
                                      vnd_name,
                                      sizeof(vnd_name)) != NETMGR_RMNET_SUCCESS)
      {
        netmgr_log_err("%s() Unable to find device name for vnd %d", __func__, vnd);
        goto error;
      }
      if (netmgr_kif_ifioctl_set_qos_version(vnd_name,
                                             (uint32_t)data_format->qos_header_format)!= NETMGR_SUCCESS)
      {
        netmgr_log_err("%s() %s dev_name %s wda_response.qos_header_format %u",
                       __func__,
                       "netmgr_kif_ifioctl_set_qos_version() failed",
                       vnd_name,
                       data_format->qos_header_format);
        goto error;
      }
      netmgr_log_low("%s() Set qos_header_mode for vnd %d vnd_name %s qos_header_format %u",
                     __func__,
                     vnd,
                     vnd_name,
                     data_format->qos_header_format);
    }
  }
  netmgr_log_low("%s() Successfully configured QoS", __func__);
success:
  retval = NETMGR_SUCCESS;
error:
  return retval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_powerup_init
===========================================================================*/
/*!
@brief
  Perform power-up initialization.

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
netmgr_kif_powerup_init
(
  char * iname
)
{
  (void)iname;
  ds_target_t target = ds_get_target();

  /* Initialize device names */
  netmgr_kif_init_names();

#ifndef FEATURE_DS_LINUX_DRIVER_LEGACY
  char cmd[NETMGR_KIF_SYSCMD_SIZ];
  int i, j;
  int modem_status = NETMGR_KIF_OPEN_SUCCESS;
  int port_opened = NETMGR_KIF_PORT_CLOSED;
  uint32 mode = RMNET_MODE_LLP_ETH;

  /* Test the RMNET driver QMI link-layer protocol mode to see if
  * already in IP mode (driver uses Ethernet default).  This is an
  * indirect approach to determine if NetMgr has reset without device
  * powercyle.  Goal is to avoid closing ports as this toggle DTR on
  * Modem, which will clear QMI client registrations.  NetMgr may
  * restart after other processes have already registered with Modem
  * QMI, so this must be avoided. */
  if( NETMGR_LINK_MAX > netmgr_main_cfg.def_link ) {
    netmgr_link_id_t link = netmgr_main_cfg.def_link;
    if( NETMGR_SUCCESS !=
        netmgr_kif_ifioctl_get_llpmode( netmgr_kif_info[link].name,
                                        &mode ) ) {
      netmgr_log_err("netmgr_kif_ifioctl_get_llpmode() failed"
                     " for %s\n", netmgr_kif_info[link].name);
    }
  } else {
     netmgr_log_err("Invalid default link[%d]", netmgr_main_cfg.def_link);
  }

  if( RMNET_MODE_LLP_IP != mode ) {
    /* Loop over all links to close ports.  Do this in separate step to
     * ensure Modem will have all ports closed when later attempt to
     * open is done. */
    for (i = 0; i < NETMGR_MAX_LINK; i++) {
      (void)netmgr_kif_ifioctl_close_port( netmgr_kif_info[i].name );
    }
  }
  else
  {
    netmgr_log_med("Driver in IP mode, skipping port close\n");
  }

  /* infinite loop till at least one port is open */
  do
  {
    /* Loop over all links to open ports */
    for (i = 0; i < NETMGR_MAX_LINK; i++)
    {
      /* open driver transport port.  Doing this here to avoid race
       * condition where DTR toggle clears QMI/RmNET configuration
       * settings */

      /* Retry mechanism for first transport port to Modem */
      if( netmgr_kif_cfg.link_array[i].modem_wait )
      {
        j = 0;
        do
        {
          if( NETMGR_SUCCESS !=
            netmgr_kif_ifioctl_open_port( netmgr_kif_info[i].name ) )
          {
            /* Failed to open port */
            netmgr_log_err("netmgr_kif_ifioctl_open_port() failed for %s on attempt %d\n",
                           netmgr_kif_info[i].name, j);
            usleep(NETMGR_KIF_WAIT_TIME_BEFORE_NEXT_RETRY);
          }
          else
          {
            /* Even if we are able to open port successfully, wait a bit
             * before trying to open the next one to give enough time to
             * modem to open them all */
            usleep(NETMGR_KIF_WAIT_TIME_BEFORE_NEXT_RETRY);
            break;
          }
        } while ( NETMGR_KIF_MAX_RETRY_COUNT > j++ );

        /* Disable link if port not opened */
        if( NETMGR_KIF_MAX_RETRY_COUNT < j )
        {
          netmgr_log_err("netmgr_kif_ifioctl_open_port() failed for %s\n",
                         netmgr_kif_info[i].name);
          modem_status = NETMGR_KIF_OPEN_FAILURE;
        }
        else
        {
          port_opened= NETMGR_KIF_PORT_OPENED;
        }
      }
      else if ( netmgr_kif_cfg.link_array[i].enabled )
      {
        /* No retry mechanism for subsequent ports */
        if( NETMGR_SUCCESS !=
            netmgr_kif_ifioctl_open_port( netmgr_kif_info[i].name ) )
        {
          netmgr_log_err("netmgr_kif_ifioctl_open_port() failed for %s\n",
                         netmgr_kif_info[i].name);
        }
        else
        {
          port_opened= NETMGR_KIF_PORT_OPENED;
        }
      }
      else
      {
        netmgr_log_high("kif link [%d] is disabled\n", i);
      }
    }

    /* get out of infinite loop at least one port opened */
    if(port_opened == NETMGR_KIF_PORT_OPENED )
    {
      break;
    }
    else
    {
      netmgr_log_high("No port opened yet, retry .....\n");

      /* reset modem_status */
      modem_status = NETMGR_KIF_OPEN_SUCCESS;
    }
  }while (1);

#ifdef FEATURE_DATA_IWLAN
  if (NETMGR_SUCCESS != netmgr_kif_update_config_and_routing_rules())
  {
    netmgr_log_err( "failed to update config and routing rules\n" );
  }
#endif /* FEATURE_DATA_IWLAN */

 memset(cmd, 0, NETMGR_KIF_SYSCMD_SIZ);

 /* Deregister other phy devices in case QMAP is supported*/
 if(target == DS_TARGET_DPM_2_0)
 {
   /* Skip calling this IOCTL for rmnet0 */
   for(i = NETMGR_LINK_RMNET_1; i <= NETMGR_LINK_RMNET_7; i++)
   {
     snprintf(cmd,sizeof(cmd),"rmnet%d",i);

     if (netmgr_kif_unreg_dev(cmd) != NETMGR_SUCCESS)
     {

      netmgr_log_err("%s(): Error setting netmgr_kif_unreg_dev on the transport (%s)",
                    __func__, netmgr_kif_info[i].name);
     }
   }
 }
#endif /* FEATURE_DS_LINUX_DRIVER_LEGACY */
}


/*===========================================================================
  FUNCTION  netmgr_kif_get_name
===========================================================================*/
/*!
@brief
  Accessor for getting device name for a given link. Returns  NULL on empty string

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
netmgr_kif_get_name (int link)
{
  if( NETMGR_SUCCESS == netmgr_kif_verify_link( link ) && strncmp(netmgr_kif_info[link].name, "", 1))
    return netmgr_kif_info[link].name;
  else
    return NULL;
}

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
netmgr_kif_get_link_powerup_state(int link)
{
   return netmgr_kif_info[link].pwrup_status;
}

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
)
{
   netmgr_kif_info[link].pwrup_status = pwrup_state;
}

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
netmgr_kif_send_icmpv6_router_solicitation (int link)
{
#if defined FEATURE_DS_LINUX_ANDROID || defined FEATURE_DATA_LINUX_LE
  int sock_fd = -1;
  struct icmp6_hdr router_solicit;
  struct sockaddr_in6 dest6;
  struct sockaddr_in6* __attribute__((__may_alias__)) dest6_ptr;
  int ret = NETMGR_FAILURE;
  int hop_limit = 255;
  const char *link_name = netmgr_kif_get_name(link);

  if (NULL == link_name)
  {
    netmgr_log_err("router solicitation unable to determine name for link=%d\n",
                    link);
    goto bail;
  }

  if ((sock_fd = socket(AF_INET6, SOCK_RAW | SOCK_CLOEXEC, IPPROTO_ICMPV6)) < 0)
  {
    netmgr_log_sys_err("router solicitation socket() failed:");
    goto bail;
  }

  netmgr_log_med("router solicitation setting hoplimit[%d] interface[%s]",
                 hop_limit, link_name );

  /* Set the multicast hop limit */
  if( -1 == setsockopt( sock_fd,
                        IPPROTO_IPV6,
                        IPV6_MULTICAST_HOPS,
                        (char *) &hop_limit,
                        sizeof(hop_limit)) )
  {
    netmgr_log_sys_err("router solicitation setsockopt() failed to set hop limit:");
    goto bail;
  }

  /* Bind to the specific link interface */
  if( -1 == setsockopt( sock_fd,
                        SOL_SOCKET,
                        SO_BINDTODEVICE,
                        link_name,
                        (socklen_t)(strlen(link_name)+1) ) )
  {
    netmgr_log_sys_err("router solicitation setsockopt() failed on iface bind:");
    goto bail;
  }

  router_solicit.icmp6_type = ND_ROUTER_SOLICIT;
  router_solicit.icmp6_code = 0;

  memset(&dest6, 0, sizeof(dest6));
  inet_pton(AF_INET6, NETMGR_KIF_IPV6_MULTICAST_ROUTER_ADDR, &dest6.sin6_addr);
  dest6.sin6_family = AF_INET6;
  dest6_ptr = &dest6;

  netmgr_log_med("sending router solicitation");

  if (sendto(sock_fd,
             &router_solicit,
             sizeof(router_solicit),
             0,
             (struct sockaddr*)dest6_ptr,
             sizeof(dest6)) < 0)
  {
    netmgr_log_sys_err("router solication sendto() failed:");
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  if (-1 != sock_fd)
  {
    close(sock_fd);
  }

  return ret;
#else
  netmgr_log_err("sending router solicitation not supported in this build");
  return NETMGR_FAILURE;
#endif
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_kif_get_active_iwlan_calls_by_tech
===========================================================================*/
/*!
@brief
  Returns the number of active iWLAN calls for the given family and technology

@return
  The number of active iWLAN calls

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static unsigned int
netmgr_kif_get_active_iwlan_calls_by_tech
(
  unsigned int addrset_mask,
  wds_technology_name_enum_v01 tech
)
{
  int i;
  unsigned int active_calls = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 != tech
        && WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 != tech)
  {
    netmgr_log_err("%s(): Unsupported tech=0x%x\n", __func__, tech);
    goto bail;
  }

  for (i = 0; i < NETMGR_MAX_LINK; i++)
  {
    if (TRUE == netmgr_kif_cfg.link_array[i].enabled &&
        NETMGR_KIF_IS_REV_RMNET_LINK(i))
    {
      netmgr_address_info_t *addr_info = netmgr_qmi_get_addr_info(i);

      if (NULL != addr_info &&
          0 != (addr_info->valid_mask & addrset_mask) &&
          tech == netmgr_qmi_wds_get_tech_name(i))
      {
        ++active_calls;
      }
    }
  }

bail:
  NETMGR_LOG_FUNC_EXIT;

  return active_calls;
}

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
)
{
  int ret = NETMGR_FAILURE;
  netmgr_address_info_t  *addr_info_ptr = NULL;
  netmgr_ipsec_sa_t  *v4_sa = NULL;
  netmgr_ipsec_sa_t  *v6_sa = NULL;
  netmgr_ip_address_t  *ip_addr = NULL;
  netmgr_qmi_client_type_t  clnt;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_kif_verify_link(link))
  {
    netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: invalid link=%d",
                   link);
    return NETMGR_FAILURE;
  }
  else if (AF_INET  != ip_family &&
           AF_INET6 != ip_family)
  {
    netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: invalid family=%d",
                   ip_family);
    return NETMGR_FAILURE;
  }

  /* Return SUCESS if it is not a reverse Rmnet link */
  if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    return NETMGR_SUCCESS;
  }

  if (!(addr_info_ptr = netmgr_qmi_get_addr_info(link)))
  {
    netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: invalid addr_info_ptr");
    goto bail;
  }

  /* Install the SAs only for an ePDG call */
  if (WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == netmgr_qmi_wds_get_tech_name(link))
  {
    if (NETMGR_ADDRSET_MASK_IPV4 & addr_info_ptr->valid_mask)
    {
      v4_sa = &addr_info_ptr->ipv4.sa;
    }
    if (NETMGR_ADDRSET_MASK_IPV6 & addr_info_ptr->valid_mask)
    {
      v6_sa = &addr_info_ptr->ipv6.sa;
    }

    if ((AF_INET  == ip_family && !v4_sa) ||
        (AF_INET6 == ip_family && !v6_sa))
    {
      netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: invalid SA for ip_family=%d",
                     ip_family);
      goto bail;
    }

    /* If the SA is already installed, return SUCCESS */
    if ((AF_INET  == ip_family && TRUE == v4_sa->is_sa_valid) ||
        (AF_INET6 == ip_family && TRUE == v6_sa->is_sa_valid))
    {
      netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: SA already installed for ip_family=%d",
                     ip_family);
      return NETMGR_SUCCESS;
    }

    /* Install extended xfrm rules for V6 */
    if (AF_INET6 == ip_family)
    {
      NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY_EX(out);
    }

    /* If the SA is shared and already installed in the other address family, return SUCCESS */
    if (AF_INET == ip_family && v4_sa->is_sa_shared && v6_sa && v6_sa->is_sa_valid)
    {
      netmgr_log_med("netmgr_kif_install_sa_and_routing_rules: V6 SA already installed, only installing V4 policy");

      /* We only need to install the 'policy' in both directions for the new address family */
      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               NETMGR_IPV4_ADDR,
                                                               NETMGR_SA_DIR_TX,
                                                               v4_sa->sa_policy_sel[NETMGR_SA_DIR_TX],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: failed to get xfrm policy sel string");
        goto bail;
      }

      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               NETMGR_IPV4_ADDR,
                                                               NETMGR_SA_DIR_RX,
                                                               v4_sa->sa_policy_sel[NETMGR_SA_DIR_RX],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: failed to get xfrm policy sel string");
        goto bail;
      }

      v4_sa->mode = v6_sa->mode;
      strlcpy(v4_sa->sa_state_id[NETMGR_SA_DIR_TX], v6_sa->sa_state_id[NETMGR_SA_DIR_TX], NETMGR_MAX_STR_LENGTH);
      strlcpy(v4_sa->sa_state_id[NETMGR_SA_DIR_RX], v6_sa->sa_state_id[NETMGR_SA_DIR_RX], NETMGR_MAX_STR_LENGTH);

      if (TRUE == netmgr_main_get_iwlan_traffic_sel_enabled()
           && v6_sa->ts_mode != NETMGR_TS_MODE_DISABLED)
      {
        /* If traffic selectors are enabled, process and install the necessary policies */
        /* We would have already processed the TSi and TSr information in the previous
         * reverse WDS_CONNECTED_EV for V6 so we can directly use that same information
         * for installing the remaining policies */
        v4_sa->ts_mode = v6_sa->ts_mode;
        memcpy(&v4_sa->sa_tsi, &v6_sa->sa_tsi, sizeof(v4_sa->sa_tsi));
        memcpy(&v4_sa->sa_tsr, &v6_sa->sa_tsr, sizeof(v4_sa->sa_tsr));

        /* Install traffic selector policy in the 'out' direction */
        if (NETMGR_SUCCESS
             != netmgr_kif_install_traffic_selector_policies(&v4_sa->sa_tsi,
                                                             &v4_sa->sa_tsr,
                                                             v4_sa->ts_mode,
                                                             NETMGR_SA_DIR_TX,
                                                             v4_sa->mode,
                                                             v4_sa->sa_state_id[NETMGR_SA_DIR_TX],
                                                             AF_INET))
        {
          netmgr_log_err("%s(): Failed to install traffic selector"
                         " policies in 'out' direction for V4!\n",
                         __func__);
          goto bail;
        }

        /* Install traffic selector policy in the 'in' and 'fwd' direction */
        if (NETMGR_SUCCESS
             != netmgr_kif_install_traffic_selector_policies(&v4_sa->sa_tsi,
                                                             &v4_sa->sa_tsr,
                                                             v4_sa->ts_mode,
                                                             NETMGR_SA_DIR_RX,
                                                             v4_sa->mode,
                                                             v4_sa->sa_state_id[NETMGR_SA_DIR_RX],
                                                             AF_INET))
        {
          netmgr_log_err("%s(): Failed to install traffic selector"
                         " policies in 'in' direction for V4!\n",
                         __func__);
          goto bail;
        }
      }
      else
      {
        /* Install the default policies */
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(TX,out,v4_sa->mode,
                                                  v4_sa->sa_state_id,v4_sa->sa_policy_sel);
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX,in,v4_sa->mode,
                                                  v4_sa->sa_state_id,v4_sa->sa_policy_sel);
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX,fwd,v4_sa->mode,
                                                  v4_sa->sa_state_id,v4_sa->sa_policy_sel);
      }

      memcpy(&v4_sa->tunnel_ep, &v6_sa->tunnel_ep, sizeof(v4_sa->tunnel_ep));
      v4_sa->is_sa_valid = TRUE;
      ret = NETMGR_SUCCESS;

      goto bail;
    }
    else if (AF_INET6 == ip_family && v6_sa->is_sa_shared && v4_sa && v4_sa->is_sa_valid)
    {
      netmgr_log_med("netmgr_kif_install_sa_and_routing_rules: V4 SA already installed, only installing V6 policy");

      /* We only need to install the 'policy' in both directions for the new address family */
      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               NETMGR_IPV6_ADDR,
                                                               NETMGR_SA_DIR_TX,
                                                               v6_sa->sa_policy_sel[NETMGR_SA_DIR_TX],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: failed to get xfrm policy sel string");
        goto bail;
      }

      if (NETMGR_SUCCESS != netmgr_kif_get_xfrm_policy_sel_str(addr_info_ptr,
                                                               NETMGR_IPV6_ADDR,
                                                               NETMGR_SA_DIR_RX,
                                                               v6_sa->sa_policy_sel[NETMGR_SA_DIR_RX],
                                                               NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: failed to get xfrm policy sel string");
        goto bail;
      }

      v6_sa->mode = v4_sa->mode;
      strlcpy(v6_sa->sa_state_id[NETMGR_SA_DIR_TX], v4_sa->sa_state_id[NETMGR_SA_DIR_TX], NETMGR_MAX_STR_LENGTH);
      strlcpy(v6_sa->sa_state_id[NETMGR_SA_DIR_RX], v4_sa->sa_state_id[NETMGR_SA_DIR_RX], NETMGR_MAX_STR_LENGTH);

      if (TRUE == netmgr_main_get_iwlan_traffic_sel_enabled()
           && v4_sa->ts_mode != NETMGR_TS_MODE_DISABLED)
      {
        /* If traffic selectors are enabled, the necessary policies */
        /* We would have already processed the TSi and TSr information in the previous
         * reverse WDS_CONNECTED_EV for V4 so we can directly use that same information
         * for installing the remaining policies */
        v6_sa->ts_mode = v4_sa->ts_mode;
        memcpy(&v6_sa->sa_tsi, &v4_sa->sa_tsi, sizeof(v6_sa->sa_tsi));
        memcpy(&v6_sa->sa_tsr, &v4_sa->sa_tsr, sizeof(v6_sa->sa_tsr));

        /* Install traffic selector policy in the 'out' direction */
        if (NETMGR_SUCCESS
             != netmgr_kif_install_traffic_selector_policies(&v6_sa->sa_tsi,
                                                             &v6_sa->sa_tsr,
                                                             v6_sa->ts_mode,
                                                             NETMGR_SA_DIR_TX,
                                                             v6_sa->mode,
                                                             v6_sa->sa_state_id[NETMGR_SA_DIR_TX],
                                                             AF_INET6))
        {
          netmgr_log_err("%s(): Failed to install traffic selector"
                         " policies in 'out' direction for V6!\n",
                         __func__);
          goto bail;
        }

        /* Install traffic selector policy in the 'in' and 'fwd' direction */
        if (NETMGR_SUCCESS
             != netmgr_kif_install_traffic_selector_policies(&v6_sa->sa_tsi,
                                                             &v6_sa->sa_tsr,
                                                             v6_sa->ts_mode,
                                                             NETMGR_SA_DIR_RX,
                                                             v6_sa->mode,
                                                             v6_sa->sa_state_id[NETMGR_SA_DIR_RX],
                                                             AF_INET6))
        {
          netmgr_log_err("%s(): Failed to install traffic selector"
                         " policies in 'in' direction for V6!\n",
                         __func__);
          goto bail;
        }
      }
      else
      {
        /* Install the default policies */
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(TX,out,v6_sa->mode,
                                                  v6_sa->sa_state_id,v6_sa->sa_policy_sel);
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX,in,v6_sa->mode,
                                                  v6_sa->sa_state_id,v6_sa->sa_policy_sel);
        NETMGR_KIF_UTIL_INSTALL_IPSEC_XFRM_POLICY(RX,fwd,v6_sa->mode,
                                                  v6_sa->sa_state_id,v6_sa->sa_policy_sel);
      }

      memcpy(&v6_sa->tunnel_ep, &v4_sa->tunnel_ep, sizeof(v6_sa->tunnel_ep));
      v6_sa->is_sa_valid = TRUE;
      ret = NETMGR_SUCCESS;

      goto bail;
    }

    if (NETMGR_SUCCESS != netmgr_kif_install_sa_rules(link, ip_family))
    {
      netmgr_log_err("netmgr_kif_install_sa_and_routing_rules: failed to install SA rules, "
                     "link=%d, ip_family=%d",
                     link,
                     ip_family);
      goto bail;
    }
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}


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
)
{
  int ret = NETMGR_FAILURE;
  netmgr_address_info_t  *addr_info = NULL;
  netmgr_ipsec_sa_t  *v4_sa = NULL;
  netmgr_ipsec_sa_t  *v6_sa = NULL;
  char local_addr[NETMGR_MAX_STR_LENGTH];

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_kif_verify_link(link))
  {
    netmgr_log_err("netmgr_kif_remove_sa_and_routing_rules: invalid link=%d",
                   link);
    return NETMGR_FAILURE;
  }

  if (!addr_info_ptr)
  {
    netmgr_log_err("netmgr_kif_remove_sa_and_routing_rules: invalid addr_info_ptr");
    return NETMGR_FAILURE;
  }

  if (NETMGR_IPV4_ADDR != addr_info_ptr->if_addr.type &&
      NETMGR_IPV6_ADDR != addr_info_ptr->if_addr.type)
  {
    netmgr_log_err("netmgr_kif_remove_sa_and_routing_rules: invalid family_type=%d",
                   addr_info_ptr->if_addr.type);
    return NETMGR_FAILURE;
  }

  /* Return SUCESS if it is not a reverse Rmnet link */
  if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    return NETMGR_SUCCESS;
  }

  /* Remove the SAs only for an ePDG call */
  if (WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == netmgr_qmi_wds_get_tech_name(link))
  {
    if (NETMGR_SUCCESS != netmgr_kif_remove_sa_rules(link, ip_family))
    {
      netmgr_log_err("netmgr_kif_remove_sa_and_routing_rules: failed to remove SA rules");
      goto bail;
    }

    /* Remove extended xfrm rules for V6 */
    if (AF_INET6 == ip_family
          && 0 == netmgr_kif_get_active_iwlan_calls_by_tech(NETMGR_ADDRSET_MASK_IPV6,
                                                            WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01))
    {
      NETMGR_KIF_UTIL_REMOVE_IPSEC_XFRM_POLICY_EX(out);
    }
  }
  else if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == netmgr_qmi_wds_get_tech_name(link))
  {
    /* When LBO interface disconnected we need to cleanup the association between the
       corresponding tunnel end-point addresses and the UDP encap port
       TODO: What if we have a IPv6 tunnel? */
    if (ip_family == AF_INET)
    {
      if (NULL !=
            inet_ntop(AF_INET, &(addr_info_ptr->if_addr.addr.v4),
                      local_addr, NETMGR_MAX_STR_LENGTH))
      {
        netmgr_log_med("%s(): LBO disconnected, local_addr to be removed [%s]",
                       __func__, local_addr);
        netmgr_main_cfg_cleanup_local_addr_for_ike_ports(local_addr);
      }
    }
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_kif_install_wlan_rules
===========================================================================*/
/*!
@brief
  Install default routing rule in our custom table to enable forwarding
  of Modem originated traffic over WLAN

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
netmgr_kif_install_wlan_rules(int link)
{
  struct netmgr_kif_cb_ifinfo ifinfo;
  netmgr_address_info_t * address_info = NULL;
  int ip_family;
  int tunnel_family = AF_INET;//Assuming V4 tunnel by default
  const char *local_addr = NULL, *dest_addr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  memset(&ifinfo, 0, sizeof(ifinfo));

  if (NETMGR_SUCCESS != netmgr_kif_fill_ifinfo(link, &ifinfo))
  {
    netmgr_log_err("%s(): Failed to initialize callback info structure", __func__);
    return NETMGR_FAILURE;
  }

  address_info = netmgr_qmi_get_addr_info(link);

  if (!address_info)
  {
    netmgr_log_err("%s(): Failed to obtain address information for link [%d]",
                   __func__, link);
    return NETMGR_FAILURE;
  }

  if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4 &&
       address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4V6;
    ifinfo.addr._in4.addr = address_info->ipv4.if_addr.addr.v4;
    ifinfo.addr._in4.mask = address_info->ipv4.if_mask;

    memcpy(&ifinfo.addr._in6.addr,
           &address_info->ipv6.if_addr.addr,
           sizeof(ifinfo.addr._in6.addr));
    ifinfo.addr._in6.mask = address_info->ipv6.if_mask;

    ip_family = AF_UNSPEC;
  }
  else if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4;
    ifinfo.addr._in4.addr = address_info->ipv4.if_addr.addr.v4;
    ifinfo.addr._in4.mask = address_info->ipv4.if_mask;
    ip_family = AF_INET;
  }
  else if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV6;
    memcpy(&ifinfo.addr._in6.addr,
           &address_info->ipv6.if_addr.addr,
           sizeof(ifinfo.addr._in6.addr));
    ifinfo.addr._in6.mask = address_info->ipv6.if_mask;
    ip_family = AF_INET6;
  }
  else
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    return NETMGR_FAILURE;
  }

#ifdef FEATURE_DATA_IWLAN
  /* TODO: Remove compiler macro */
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    if (NETMGR_SUCCESS != netmgr_qmi_get_ipsec_tunnel_endpoints(link,
                                                                ip_family,
                                                                &local_addr,
                                                                &dest_addr,
                                                                &tunnel_family))
    {
      netmgr_log_err("%s(): failed to get tunnel endpoints for link [%d] ip_family [%d]",
                     __func__, link, ip_family);
    }
  }
#endif /* FEATURE_DATA_IWLAN */

  ifinfo.tunnel_info.local_addr    = local_addr;
  ifinfo.tunnel_info.dest_addr     = dest_addr;
  ifinfo.tunnel_info.tunnel_family = tunnel_family;

  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_WLAN_UP, (void *) &ifinfo);

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_kif_remove_wlan_rules
===========================================================================*/
/*!
@brief
  Cleanup (previously installed) default routing rule in our custom table

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
netmgr_kif_remove_wlan_rules(int link)
{
  struct netmgr_kif_cb_ifinfo ifinfo;
  netmgr_address_info_t * address_info = NULL;
  int ip_family;
  int tunnel_family = AF_INET;//Assuming V4 tunnel by default
  const char *local_addr = NULL, *dest_addr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  memset(&ifinfo, 0, sizeof(ifinfo));

  if (NETMGR_SUCCESS != netmgr_kif_fill_ifinfo(link, &ifinfo))
  {
    netmgr_log_err("%s(): Failed to initialize callback info structure", __func__);
    return NETMGR_FAILURE;
  }

  address_info = netmgr_qmi_get_addr_info(link);

  if (!address_info)
  {
    netmgr_log_err("%s(): Failed to obtain address information for link [%d]",
                   __func__, link);
    return NETMGR_FAILURE;
  }

    if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4 &&
       address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4V6;
    ifinfo.addr._in4.addr = address_info->ipv4.if_addr.addr.v4;
    ifinfo.addr._in4.mask = address_info->ipv4.if_mask;

    memcpy(&ifinfo.addr._in6.addr,
           &address_info->ipv6.if_addr.addr,
           sizeof(ifinfo.addr._in6.addr));
    ifinfo.addr._in6.mask = address_info->ipv6.if_mask;

    ip_family = AF_UNSPEC;
  }
  else if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV4;
    ifinfo.addr._in4.addr = address_info->ipv4.if_addr.addr.v4;
    ifinfo.addr._in4.mask = address_info->ipv4.if_mask;
    ip_family = AF_INET;
  }
  else if (address_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6)
  {
    ifinfo.addr.type = NETMGR_KIF_CB_ADDR_IPV6;
    memcpy(&ifinfo.addr._in6.addr,
           &address_info->ipv6.if_addr.addr,
           sizeof(ifinfo.addr._in6.addr));
    ifinfo.addr._in6.mask = address_info->ipv6.if_mask;
    ip_family = AF_INET6;
  }
  else
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    return NETMGR_FAILURE;
  }

#ifdef FEATURE_DATA_IWLAN
  /* TODO: Remove compiler macro */
  if (NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    if (NETMGR_SUCCESS != netmgr_qmi_get_ipsec_tunnel_endpoints(link,
                                                                ip_family,
                                                                &local_addr,
                                                                &dest_addr,
                                                                &tunnel_family))
    {
      netmgr_log_err("%s(): failed to get tunnel endpoints for link [%d] ip_family [%d]",
                     __func__, link, ip_family);
    }
  }
#endif /* FEATURE_DATA_IWLAN */

  ifinfo.tunnel_info.local_addr    = local_addr;
  ifinfo.tunnel_info.dest_addr     = dest_addr;
  ifinfo.tunnel_info.tunnel_family = tunnel_family;

  netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_WLAN_DOWN, (void *) &ifinfo);

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}

#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_kif_get_kif_state_str()
 ===========================================================================*/
/*!
@brief
  Helper routine to string constant for netmgr kif state.
*/
/*=========================================================================*/
static char *netmgr_kif_get_kif_state_str
(
  netmgr_kif_state_t state
)
{
  static char *netmgr_kif_state_str[] = {
      "NETMGR_KIF_CLOSED",
      "NETMGR_KIF_OPENING",
      "NETMGR_KIF_CLOSING",
      "NETMGR_KIF_OPEN",
      "NETMGR_KIF_RECONFIGURING"
  };

  if (state >= NETMGR_KIF_CLOSED && state <= NETMGR_KIF_RECONFIGURING)
  {
    return netmgr_kif_state_str[state];
  }
  else
  {
    return "NETMGR_KIF_STATE_INVALID";
  }
}

/*===========================================================================
  FUNCTION  netmgr_kif_get_pwrup_state_str()
 ===========================================================================*/
/*!
@brief
  Helper routine to string constant for powerup state.
*/
/*=========================================================================*/
static char *netmgr_kif_get_pwrup_state_str
(
  netmgr_kif_link_pwrup_state_t pwrup_state
)
{
  static char *netmgr_kif_pwr_up_state_str[] = {
    "NETMGR_KIF_LINK_POWERUP_STATE_DOWN",
    "NETMGR_KIF_LINK_POWERUP_STATE_UP"
  };

  if (pwrup_state >= NETMGR_KIF_LINK_POWERUP_STATE_DOWN && pwrup_state <= NETMGR_KIF_LINK_POWERUP_STATE_UP)
  {
    return netmgr_kif_pwr_up_state_str[pwrup_state];
  }
  else
  {
    return "NETMGR_KIF_LINK_POWERUP_STATE_INVAL";
  }
}

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
netmgr_kif_ifioctl_set_ul_checksum_offload_feature( const char * dev )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct ethtool_value value;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  value.cmd = ETHTOOL_STXCSUM;
  value.data = 1;

  ifr.ifr_data = (caddr_t)&value;

  /* Set features for the device */
  if( ioctl( fd, SIOCETHTOOL, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_ifioctl_set_dl_checksum_offload_feature
===========================================================================*/
/*!
@brief
  Helper function to set DL checksum offload feature flag on a device.

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
static int
netmgr_kif_ifioctl_set_dl_checksum_offload_feature(const char * dev)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct ethtool_value value;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  value.cmd = ETHTOOL_SRXCSUM;
  value.data = 1;

  ifr.ifr_data = (caddr_t)&value;

  /* Set features for the device */
  if(ioctl(fd, SIOCETHTOOL, &ifr) < 0) {
    netmgr_log_sys_err("ifioctl_set: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

/*===========================================================================
  FUNCTION  netmgr_kif_configure_dl_gro_feature
===========================================================================*/
/*!
@brief
  Helper function to configure DL GRO feature on a device.

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
LOCAL int
netmgr_kif_configure_dl_gro_feature(const char * dev, int enable)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct ethtool_value value;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  value.cmd = ETHTOOL_SGRO;
  value.data = enable;

  ifr.ifr_data = (caddr_t)&value;

  /* Set features for the device */
  if( ioctl( fd, SIOCETHTOOL, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

/*===========================================================================
  FUNCTION netmgr_kif_enable_ul_gso_feature
===========================================================================*/
/*!
@brief
  Helper function to enable UL GSO feature on a device.

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
netmgr_kif_enable_ul_gso_feature( const char * dev )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct ethtool_value value;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  value.cmd = ETHTOOL_SGSO;
  value.data = 1;

  ifr.ifr_data = (caddr_t)&value;

  /* Set features for the device */
  if( ioctl( fd, SIOCETHTOOL, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_se_: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}

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
netmgr_kif_get_ul_sg_feature( const char * dev )
{
  int fd;
  int rval = -1;
  struct ifreq ifr;
  struct ethtool_value value;

  if(!dev){
    netmgr_log_sys_err("get_ul_sg: null dev\n");
    goto error;
  }

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("get_ul_sg: socket failed:\n");
    goto error;
  }

  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  value.cmd = ETHTOOL_GSG;
  value.data = -1;

  ifr.ifr_data = (caddr_t)&value;

  /* Get device features */
  if( ioctl( fd, SIOCETHTOOL, &ifr ) < 0 ) {
    netmgr_log_sys_err("get_ul_sg: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  close(fd);

  rval = value.data;

 error:
  return rval;
}

/*===========================================================================
  FUNCTION netmgr_kif_enable_ul_ssg_feature
===========================================================================*/
/*!
@brief
  Helper function to enable UL SSG feature on a device.

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
netmgr_kif_enable_ul_ssg_feature( const char * dev )
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct ethtool_value value;

  NETMGR_ASSERT(dev);

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0 )) < 0 ) {
    netmgr_log_sys_err("ifioctl_set: socket failed:\n");
    goto error;
  }

  memset(&ifr, 0, sizeof(ifr));
  memset(&value, 0, sizeof(struct ethtool_value));

  /* Set device name in ioctl req struct */
  (void)strlcpy( ifr.ifr_name, dev, sizeof(ifr.ifr_name) );

  value.cmd = ETHTOOL_SSG;
  value.data = 1;

  ifr.ifr_data = (caddr_t)&value;

  /* Set features for the device */
  if( ioctl( fd, SIOCETHTOOL, &ifr ) < 0 ) {
    netmgr_log_sys_err("ifioctl_se_: SIOCETHTOOL ioctl failed:\n");
    close(fd);
    goto error;
  }

  close(fd);

  rval = NETMGR_SUCCESS;

 error:
  return rval;
}


/*===========================================================================
  FUNCTION  netmgr_kif_print_state()
 ===========================================================================*/
/*!
@brief
  Helper routine to log KIF states.


@return
  None.
*/
/*=========================================================================*/
LOCAL void *
netmgr_kif_print_state()
{
  int i;

  for (i = 0; i < NETMGR_MAX_LINK; i++)
  {
    netmgr_log_med("Link[%d] name[%s] ifi_index[%d] state[%s] pwrup_status[%s] dns_v6_queried[%d] reconfig_ip_type [%d]\n",
        i, netmgr_kif_info[i].name, netmgr_kif_info[i].ifi_index,
        netmgr_kif_get_kif_state_str(netmgr_kif_info[i].state),
        netmgr_kif_get_pwrup_state_str(netmgr_kif_info[i].pwrup_status),
        netmgr_kif_info[i].dns_v6_queried,
        netmgr_kif_info[i].reconfig_ip_type);
  }

  return 0;
}

#ifdef FEATURE_DS_LINUX_ANDROID
/*===========================================================================
  FUNCTION  netmgr_kif_process_netd_restart
 ===========================================================================*/
/*!
@brief
  Process netd restart event
*/
/*=========================================================================*/
static void netmgr_kif_process_netd_restart
(
  ds_cmd_t *ds_cmd,
  void     *data
)
{
  short                       flags = 0;
  int                         link = 0;
  const char                  *ifname;
  struct netmgr_kif_cb_ifinfo ifinfo;
  netmgr_exec_cmd_t           *exec_cmd = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  (void) ds_cmd;

  exec_cmd = (netmgr_exec_cmd_t *) data;
  if (!exec_cmd)
  {
    netmgr_log_err("%s(): invalid param!", __func__);
    goto bail;
  }

  link = exec_cmd->data.link;

  switch (exec_cmd->data.type)
  {
  case NETMGR_NETD_RESTART_EV:
    {
      netmgr_log_med("%s(): ID=NETMGR_NETD_RESTART_EV, link=%d", __func__, link);
      netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_NETD_IS, NULL);
    }

    break; /* NETMGR_NETD_GLOBAL_IN_SERVICE */

  case NETMGR_NETD_LINK_RESTART_EV:
    {
      netmgr_log_med("%s(): ID=NETMGR_NETD_LINK_RESTART_EV, link=%d", __func__, link);
      if (NETMGR_SUCCESS != netmgr_kif_fill_ifinfo(link, &ifinfo))
      {
        goto bail;
      }

      /* Get the state of the interface */
      if (netmgr_kif_ifioctl_get_flags(netmgr_kif_info[link].name, &flags) < 0)
      {
        netmgr_log_err("%s: ioctl_get failed for link %d\n", __func__, link);
        goto bail;
      }

      /* Update the fields as per requirement */
      ifinfo.flags = flags;

      netmgr_cb_runall(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_NETD_IS,
                       (void *)&ifinfo);
    }

    break; /* NETMGR_NETD_PER_LINK_IN_SERVICE */

  default:
    netmgr_log_err("%s(): unknown event_type [%d]", __func__, exec_cmd->data.type);
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_kif_handle_netd_event
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
netmgr_kif_handle_netd_event(void)
{
  int                          i = 0;
  wds_technology_name_enum_v01 tech_type = WDS_TECHNOLOGY_NAME_ENUM_MIN_ENUM_VAL_V01;
  netmgr_exec_cmd_t            *cmd = NULL;

  if (!netmgr_kif_cfg.link_array)
  {
    netmgr_log_med("%s(): KIF is not initialized, nothing to do", __func__);
    return;
  }

  /* Post global netd restarted event */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

  cmd->cmd.execute_f = netmgr_kif_process_netd_restart;
  cmd->data.type = NETMGR_NETD_RESTART_EV;

  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) )
  {
    /* We failed to post global in-service event, exit the function */
    netmgr_log_err("%s: failed to put commmand\n", __func__);
    netmgr_exec_release_cmd( cmd );
    return;
  }

  for(i = 0; i < NETMGR_MAX_LINK; i++)
  {
    if (!netmgr_kif_cfg.link_array[i].enabled)
    {
      /* Skip disabled links */
      continue;
    }

    /* Get link tech-type */
    tech_type = netmgr_qmi_wds_get_tech_name(i);

    /* If the link is of type link-local or iwlan then post per link netd restart event */
    if (WDS_TECHNOLOGY_NAME_MODEM_LINK_LOCAL_V01 == tech_type
          || WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 == tech_type
          || WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 == tech_type)
    {
      cmd = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd);

      cmd->cmd.execute_f = netmgr_kif_process_netd_restart;
      cmd->data.type = NETMGR_NETD_LINK_RESTART_EV;
      cmd->data.link = i;

      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) )
      {
        netmgr_log_err("%s: failed to put commmand\n", __func__);
        netmgr_exec_release_cmd( cmd );
      }
    }
  }
}
#endif /* FEATURE_DS_LINUX_ANDROID */

/*===========================================================================
  FUNCTION  netmgr_unix_listener_init
===========================================================================*/
/*!
@brief
  Initialization routine for UNIX domain socket interface message listener.
  This sets up the UNIX listener socket for receiving netmgr commands.

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
netmgr_unix_listener_init
(
  void
)
{
  int                 server_sock_fd = NETMGR_UNIX_API_ERROR;
  int                 len = NETMGR_UNIX_API_ERROR;
  int                 str_len = NETMGR_UNIX_API_ERROR;
  struct sockaddr_un  server_socket;

  if ((server_sock_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Unable to create UNIX socket - %s", __func__, strerror(errno));
    goto exit;
  }
  netmgr_log_med("%s(): Created UNIX server socket", __func__);

  server_socket.sun_family = AF_UNIX;
  str_len = strlcpy(server_socket.sun_path, NETMGR_UNIX_CLIENT_CONN_SOCK_PATH, UNIX_PATH_MAX);
  if ((str_len <= NETMGR_UNIX_API_ERROR) || (str_len > UNIX_PATH_MAX))
  {
    netmgr_log_err("%s(): String truncation occurred", __func__);
    goto close_server_socket;
  }
  unlink(server_socket.sun_path);
  len = strlen(server_socket.sun_path) + sizeof(server_socket.sun_family);
  if (bind(server_sock_fd, (struct sockaddr *)&server_socket, len) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Unable to bind to UNIX server socket - %s",
                   __func__,
                   strerror(errno));
    goto close_server_socket;
  }
  netmgr_log_med("%s(): Bound to UNIX server socket", __func__);

  /* Allow RW permissions only for user and group */
  if (chmod(server_socket.sun_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)
    == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): unable to chmod the UNIX server socket %s - %s",
                   __func__,
                   server_socket.sun_path,
                   strerror(errno));
    goto close_server_socket;
  }
  netmgr_log_med("%s(): chmod the UNIX server socket", __func__);

  if (listen(server_sock_fd, NETMGR_UNIX_MAX_CLIENTS) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Unable to listen on UNIX socket - %s", __func__, strerror(errno));
    goto close_server_socket;
  }
  netmgr_log_med("%s(): Listening to UNIX server socket", __func__);

  netmgr_kif_unix_recv_user_cmd(server_sock_fd);

close_server_socket:
  close(server_sock_fd);
exit:
  netmgr_log_err("netmgr_unix_listener_init failed");
  NETMGR_ABORT("netmgr_unix_listener_init failed");
}

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
netmgr_kif_set_iptables_command_options(char* opts)
{
  int rc = 0;

  if (!opts)
  {
    netmgr_log_err("%s(): invalid param!", __func__);
    memset(iptables_opts, 0, sizeof(iptables_opts));
    return;
  }

  rc = strlcpy(iptables_opts, opts, sizeof(iptables_opts));

  if (rc >= IPTABLES_OPTS_MAX_LENGTH)
  {
    netmgr_log_err("%s(): string was truncated!", __func__);
    memset(iptables_opts, 0, sizeof(iptables_opts));
  }
}

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
  int    nint,
  netmgr_ctl_port_config_type links[],
  int    skip,
  char * dirpath,
  char * modscript
)
{
  struct kif_vtbl vtable;
  pthread_mutexattr_t attr;

  NETMGR_LOG_FUNC_ENTRY;

  /* Set number of interfaces in the configuration blob */
  netmgr_log_high("netmgr_kif_init: set netmgr_kif_cfg.nint to %d\n", nint);
  netmgr_kif_cfg.nint = nint;
  netmgr_kif_cfg.link_array = links;

  /* Set 'skip' flag. This indicates if the ethernet driver module should
  ** be loaded or not. This functionality is strictly for debugging purposes.
  */
  netmgr_kif_cfg.skip = skip;

  /* Initialize module load script name and dhcp script names */
  netmgr_kif_init_paths(dirpath, modscript);

  /* Register with Platform layer */
  vtable.out_of_service    = netmgr_kif_oos_cleanup;
  vtable.reset             = netmgr_kif_reset_link;
  vtable.dispatch          = netmgr_kif_nl_msg_recv_cmd_exec;
  vtable.send_event        = netmgr_kif_send_event;
  vtable.iface_open        = netmgr_kif_open;
  vtable.iface_close       = netmgr_kif_close;
  vtable.iface_configure   = netmgr_kif_configure;
  vtable.iface_reconfigure = netmgr_kif_reconfigure;
  vtable.flow_control      = NULL;
  vtable.iface_link_update = netmgr_kif_link_update;
  vtable.enable_ipv6      = netmgr_kif_enable_ipv6_on_link;
#ifdef FEATURE_DATA_IWLAN
  vtable.wlan_open         = netmgr_kif_install_wlan_rules;
  vtable.wlan_close        = netmgr_kif_remove_wlan_rules;
#endif /* FEATURE_DATA_IWLAN */

  if( NETMGR_SUCCESS !=
      netmgr_platform_register_vtbl( NETMGR_PLATFORM_VTBL_KIF,
                                     (void*)&vtable ) )
  {
    NETMGR_ABORT("%s","netmgr_kif_init: cannot register vtable with platform layer\n");
    return;
  }

  /* Create plugin table entries for KIF */
  netmgr_cb_create_table(NETMGR_KIF_CB_TABLE);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFUP_PRE);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFUP_POST);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_PRE);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_POST);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFCFG_PRE);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFCFG_POST);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_NOSTATE_PRE);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_IFDN_NOSTATE_POST);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_NETD_OOS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_NETD_IS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_NETD_OOS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_NETD_IS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_OOS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_LINK_IS);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_ENABLE_PORTFWD);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_DISABLE_PORTFWD);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_USER_CMD_QUERY_PORTFWD);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_WLAN_UP);
  netmgr_cb_create_chain(NETMGR_KIF_CB_TABLE, NETMGR_KIF_CB_WLAN_DOWN);

  /* Register callbacks with the MAIN table */
  netmgr_cb_insert_cb(NETMGR_MAIN_CB_TABLE,
                      NETMGR_MAIN_CB_SIGUSR1,
                      NETMGR_KIF_CB_MODNAME,
                      &netmgr_kif_print_state);

  /* Initialize NetLink socket interface and listening thread */
  netmgr_kif_netlink_init();

  /* Load the kernel module, if skip flag is not set */
  if ((skip != NETMGR_KIF_SKIP) && (netmgr_kif_load_module() < 0)) {
    NETMGR_ABORT("%s","netmgr_kif_init: cannot load kernel module\n");
    return;
  }

  /* Register process termination cleanup handler */
  atexit( netmgr_kif_cleanup );

  NETMGR_LOG_FUNC_EXIT;
  return;
}

