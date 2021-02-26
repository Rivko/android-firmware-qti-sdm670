/******************************************************************************

                        N E T M G R _ K I F _ C B . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_kif_cb.h
  @brief   Network Manager Kernel Interface Module header file

  DESCRIPTION
  Header file for Kernel Interface module plugin interface

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_KIF_CB_H__
#define __NETMGR_KIF_CB_H__

#include <sys/un.h>
#include "netmgr_cb_in.h"
#include "netmgr_defs.h"

#define NETMGR_KIF_CB_MODNAME "KIF"
#define NETMGR_KIF_CB_TABLE   "KIF"

#define NETMGR_KIF_CB_IFUP_PRE             "IF_UP_PRE"
#define NETMGR_KIF_CB_IFUP_POST            "IF_UP_POST"
#define NETMGR_KIF_CB_IFDN_PRE             "IF_DOWN_PRE"
#define NETMGR_KIF_CB_IFDN_POST            "IF_DOWN_POST"
#define NETMGR_KIF_CB_IFDN_NOSTATE_PRE     "IF_DOWN_NOSTATE_PRE"
#define NETMGR_KIF_CB_IFDN_NOSTATE_POST    "IF_DOWN_NOSTATE_POST"

/* Takes (struct netmgr_kif_cb_ifinfo* ifinfo) as an argument */
#define NETMGR_KIF_CB_IFCFG_PRE  "IF_CFG_PRE"
#define NETMGR_KIF_CB_IFCFG_POST "IF_CFG_POST"

#define NETMGR_KIF_CB_WLAN_UP    "WLAN_UP"
#define NETMGR_KIF_CB_WLAN_DOWN  "WLAN_DOWN"

/* TODO: This is not a kif feature and needs to be moved out eventually
   when a separate module for netd restart is created */
#define NETMGR_KIF_CB_NETD_OOS   "NETD_OOS"    /* Global netd restart cleanup */
#define NETMGR_KIF_CB_NETD_IS    "NETD_IS"     /* Global netd restart recovery */

/* TODO: Eventually modules will be receiving only global netd
   restart events. At that time we need to remove the per link
   chains */
#define NETMGR_KIF_CB_LINK_NETD_OOS    "LINK_NETD_OOS"  /* Per-link netd restart cleanup */
#define NETMGR_KIF_CB_LINK_NETD_IS     "LINK_NETD_IS"   /* Per-link netd restart recovery */

/* Used for handling modem SSR where we need to cleanup and recover routes per link */
#define NETMGR_KIF_CB_LINK_OOS        "IF_LINK_OSS"     /* Per-link SSR cleanup */
#define NETMGR_KIF_CB_LINK_IS         "IF_LINK_IS"      /* Per-link SSR recovery */

/* Used for handling dynamic audio port-forwarding rule installation and removal
   This chain is used ONLY for handling IMS port-forwarding feature, no other CB client
   should use this */

/* Takes (struct netmgr_kif_cb_usercmd_info* usercmd_info) as an argument */
#define NETMGR_KIF_USER_CMD_ENABLE_PORTFWD       "ENABLE_PORTFWD"
#define NETMGR_KIF_USER_CMD_DISABLE_PORTFWD      "DISABLE_PORTFWD"
#define NETMGR_KIF_USER_CMD_QUERY_PORTFWD        "QUERY_PORTFWD"

#define NETMGR_CB_RESULT_SUCCESS 0
#define NETMGR_CB_RESULT_FAIL    1

/* Initialize result to RESULT_FAIL befoer invoking CB chain. CB needs to update the result
   in case of success. CB client will assume failure otherwise */
struct netmgr_kif_cb_usercmd_info
{
  int      ip_family;
  boolean  preference;
  int      result;
};

#define NETMGR_KIF_CB_IFNAMELEN 24 /* Kernel limits this to 16 */

#define NETMGR_KIF_CB_IFTYPE_FORWARD     1
#define NETMGR_KIF_CB_IFTYPE_REVERSE     2
#define NETMGR_KIF_CB_IFTYPE_LINK_LOCAL  3

#define NETMGR_KIF_CB_IF_TECH_TYPE_DONT_CARE    0
#define NETMGR_KIF_CB_IF_TECH_TYPE_LBO          1
#define NETMGR_KIF_CB_IF_TECH_TYPE_EPDG         2

typedef struct netmgr_kif_cb_ipsec_tunnel_info
{
  const char* local_addr;
  const char* dest_addr;
  sa_family_t tunnel_family;
} ipsec_tunnel_info_t;

struct netmgr_kif_cb_ifinfo {
  int ip_family;                            /* IP family */
  int link;                                 /* Netmgr internal link number */
  char name[NETMGR_KIF_CB_IFNAMELEN];       /* Iface name (as the kernel sees it) */
  int type;                                 /* Forward/Reverse */
  int tech_type;                            /* Tech-type of the call */
  short flags;                              /* Interface state flags */

  struct netmgr_kif_addrstorage addr;

  /* Ipsec tunnel information */
  ipsec_tunnel_info_t tunnel_info;
};

#define NETMGR_KIF_IPF_MASK_INVALID (0x00)
#define NETMGR_KIF_IPF_MASK_IPV4    (0x01)
#define NETMGR_KIF_IPF_MASK_IPV6    (0x02)
#define NETMGR_KIF_IPF_MASK_IPV4V6  (NETMGR_KIF_IPF_MASK_IPV4|NETMGR_KIF_IPF_MASK_IPV6)

typedef struct netmgr_kif_cb_link_ssr_info_s {
  int  link;                               /* Link on which SSR event is received */
  char name[NETMGR_KIF_CB_IFNAMELEN];
  int  tech_type;
  int  type;

  int  addr_mask;
  struct netmgr_kif_addrstorage   ipv4;
  struct netmgr_kif_addrstorage   ipv6;
}netmgr_kif_cb_link_ssr_info_t;

#endif /* __NETMGR_KIF_CB_H__ */
