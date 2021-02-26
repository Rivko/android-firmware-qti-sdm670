/******************************************************************************

      N M _ M D M P R X Y _ L E G A C Y _ R O U T E _ P R O V I D E R . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmproxy_legacy_route_provider.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy module is responsible for managing routing rules and packet
  marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <sys/socket.h>
#include <linux/if.h>
#include <string.h>

#include "ds_string.h"
#include "netmgr.h"
#include "netmgr_kif.h"
#include "netmgr_util.h"

#include "netmgr_kif_cb.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_route_provider_factory.h"
#include "nm_mdmprxy_iptables_helper.h"
#include "nm_mdmprxy_common.h"

#define NM_MDMPRXY_ARR_SIZE(x)  (sizeof(x)/sizeof(x[0]))

#define NM_MDMPRXY_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY  (3)

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static int iiface_handle;
static int iiface_lbo_refcount;
static int iiface_epdg_refcount;

static boolean iptables_inited = FALSE;

typedef struct
{
  boolean is_v4_policy_routing_installed;
  boolean is_v6_policy_routing_installed;
  boolean is_v4_default_routing_installed;
  boolean is_v6_default_routing_installed;
  boolean is_v4_lb_policy_routing_installed;
  boolean is_v6_lb_policy_routing_installed;
  boolean is_v4_s2b_policy_routing_installed;
  boolean is_v6_s2b_policy_routing_installed;
  boolean is_v4_forwarding_rules_installed;
  boolean is_v6_forwarding_rules_installed;
} nm_mdmprxy_cfg_t;

static nm_mdmprxy_cfg_t nm_mdmprxy_global_cfg;

#define NM_MDMPRXY_GET_POLICY_ROUTING_INSTALLED(family)                         \
  ((AF_INET == family) ?                                                        \
   nm_mdmprxy_global_cfg.is_v4_policy_routing_installed :                       \
   nm_mdmprxy_global_cfg.is_v6_policy_routing_installed)

#define NM_MDMPRXY_GET_DEFAULT_ROUTING_INSTALLED(family)                        \
  ((AF_INET == family) ?                                                        \
   nm_mdmprxy_global_cfg.is_v4_default_routing_installed :                      \
   nm_mdmprxy_global_cfg.is_v6_default_routing_installed)

#define NM_MDMPRXY_SET_POLICY_ROUTING_INSTALLED(family, status)                 \
  *(((AF_INET == family) ?                                                      \
    &nm_mdmprxy_global_cfg.is_v4_policy_routing_installed :                     \
    &nm_mdmprxy_global_cfg.is_v6_policy_routing_installed)) = status

#define NM_MDMPRXY_SET_DEFAULT_ROUTING_INSTALLED(family, status)                \
  *(((AF_INET == family) ?                                                      \
    &nm_mdmprxy_global_cfg.is_v4_default_routing_installed :                    \
    &nm_mdmprxy_global_cfg.is_v6_default_routing_installed)) = status

#define NM_MDMPRXY_GET_LB_POLICY_ROUTING_INSTALLED(family)                      \
  ((AF_INET == family) ?                                                        \
   nm_mdmprxy_global_cfg.is_v4_lb_policy_routing_installed :                    \
   nm_mdmprxy_global_cfg.is_v6_lb_policy_routing_installed)

#define NM_MDMPRXY_SET_LB_POLICY_ROUTING_INSTALLED(family, status)              \
  *(((AF_INET == family) ?                                                      \
    &nm_mdmprxy_global_cfg.is_v4_lb_policy_routing_installed :                  \
    &nm_mdmprxy_global_cfg.is_v6_lb_policy_routing_installed)) = status

#define NM_MDMPRXY_GET_S2B_POLICY_ROUTING_INSTALLED(family)                     \
  ((AF_INET == family) ?                                                        \
   nm_mdmprxy_global_cfg.is_v4_s2b_policy_routing_installed :                   \
   nm_mdmprxy_global_cfg.is_v6_s2b_policy_routing_installed)

#define NM_MDMPRXY_SET_S2B_POLICY_ROUTING_INSTALLED(family, status)             \
  *(((AF_INET == family) ?                                                      \
    &nm_mdmprxy_global_cfg.is_v4_s2b_policy_routing_installed :                 \
    &nm_mdmprxy_global_cfg.is_v6_s2b_policy_routing_installed)) = status

#define NM_MDMPRXY_GET_FORWARDING_RULES_INSTALLED(family)                       \
  ((AF_INET == family) ?                                                        \
   nm_mdmprxy_global_cfg.is_v4_forwarding_rules_installed :                     \
   nm_mdmprxy_global_cfg.is_v6_forwarding_rules_installed)

#define NM_MDMPRXY_SET_FORWARDING_RULES_INSTALLED(family, status)               \
  *(((AF_INET == family) ?                                                      \
    &nm_mdmprxy_global_cfg.is_v4_forwarding_rules_installed :                   \
    &nm_mdmprxy_global_cfg.is_v6_forwarding_rules_installed)) = status

static void
nm_mdmprxy_legacy_prov_oos_hndlr(void);

static void
nm_mdmprxy_legacy_prov_is_hndlr(void);

static void
nm_mdmprxy_legacy_prov_init(void);

static int
nm_mdmprxy_legacy_prov_link_oos_hndlr(void*);

static int
nm_mdmprxy_legacy_prov_link_is_hndlr(void*);

static int
nm_mdmprxy_legacy_prov_install_routes
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
);

static int
nm_mdmprxy_legacy_prov_uninstall_routes
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
);

static int
nm_mdmprxy_legacy_prov_install_iiface_rules
(
  int                  link,
  char*                link_name,
  int                  tech_type,
  int                  valid4,
  uint32_t             daddr4,
  int                  valid6,
  struct in6_addr      *daddr6,
  uint32_t             mask6,
  ipsec_tunnel_info_t  *tunnel
);

static int
nm_mdmprxy_legacy_prov_remove_iiface_rules
(
  int                           link,
  char                          *link_name,
  int                           tech_type,
  int                           ip_family,
  ipsec_tunnel_info_t           *tunnel,
  struct netmgr_kif_addrstorage *link_addr
);

static int
nm_mdmprxy_legacy_prov_netd_is_hdlr(void);

static int
nm_mdmprxy_legacy_prov_netd_link_is_hdlr(void*);

nm_mdmprxy_route_prov_t nm_mdmprxy_legacy_route_provider =
{
  /* Initialize the function table for
   * the legacy route provider */
  .add_route               = nm_mdmprxy_legacy_prov_install_routes,
  .remove_route            = nm_mdmprxy_legacy_prov_uninstall_routes,
  .install_rule            = nm_mdmprxy_legacy_prov_install_iiface_rules,
  .remove_rule             = nm_mdmprxy_legacy_prov_remove_iiface_rules,
  .init                    = nm_mdmprxy_legacy_prov_init,
  .is_hdlr                 = nm_mdmprxy_legacy_prov_is_hndlr,
  .oos_hdlr                = nm_mdmprxy_legacy_prov_oos_hndlr,
  .is_hdlr_per_link        = nm_mdmprxy_legacy_prov_link_is_hndlr,
  .oos_hdlr_per_link       = nm_mdmprxy_legacy_prov_link_oos_hndlr,
  .netd_is_hdlr            = nm_mdmprxy_legacy_prov_netd_is_hdlr,
  .netd_is_hdlr_per_link   = nm_mdmprxy_legacy_prov_netd_link_is_hdlr
};

static const struct nm_mdmprxy_cmd static_rule_list[] =
{
  { "%s %s -t mangle -A nm_mdmprxy_pkt_marker -m mark --mark %u"
                          " -j nm_mdmprxy_pkt_forwarder",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -m mark --mark %u"
                          " -j nm_mdmprxy_pkt_forwarder",
    nm_mdmprxy_markjump_formatter },

  { "%s %s -t mangle -A nm_mdmprxy_pkt_forwarder -j ACCEPT",
    "%s %s -t mangle -D nm_mdmprxy_pkt_forwarder -j ACCEPT",
    nm_mdmprxy_basic_formatter
  }
};

static const struct nm_mdmprxy_cmd per_iiface_rule_list[] =
{
  { "%s %s -t mangle -I PREROUTING -i %s -j nm_mdmprxy_pkt_marker",
    "%s %s -t mangle -D PREROUTING -i %s -j nm_mdmprxy_pkt_marker",
    nm_mdmprxy_ifacejump_formatter },

  /* nm_mdmprxy_icmp_pkt_marker chain hosts marking rules for icmp ping packets
     originating from the modem. The mark provider will install the marking rules. The
     route provider just needs to install the jump rule in PREROUTING chain to the
     nm_mdmprxy_icmp_pkt_marker chain */
  { "%s %s -t mangle -A PREROUTING -j nm_mdmprxy_icmp_pkt_marker",
    "%s %s -t mangle -D PREROUTING -j nm_mdmprxy_icmp_pkt_marker",
    nm_mdmprxy_basic_formatter }
};

static const struct nm_mdmprxy_cmd per_oiface_rule_list[] = {{0,0,0}};

static const struct nm_mdmprxy_cmd per_iface_forward_rule_list[] =
{
  { "%s %s -t filter -I nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    nm_mdmprxy_ifaceforward_incoming_formatter },

  { "%s %s -t filter -I nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    nm_mdmprxy_ifaceforward_outgoing_formatter },
};

/* It is possible to have wlan0 address on the UE and DNS address on the epdg server to share
     the same subnet. In such situations packets originating on from the modem will be routed
     up the AP stack since the subnets are same. To prevent such situations we will TEE the
     incoming packet from the modem arriving from local-breakout interface and force the packet
     over wlan0 using a dummy gateway */
static const struct nm_mdmprxy_cmd local_breakout_rule_list[] =
{
  { "%s %s -t mangle -A PREROUTING -p tcp -i %s -j nm_mdmprxy_mngl_pre_tee",
    "%s %s -t mangle -D PREROUTING -p tcp -i %s -j nm_mdmprxy_mngl_pre_tee",
    nm_mdmprxy_lbo_ifacejump_formatter },

  { "%s %s -t mangle -A PREROUTING -p udp -i %s -j nm_mdmprxy_mngl_pre_tee",
    "%s %s -t mangle -D PREROUTING -p udp -i %s -j nm_mdmprxy_mngl_pre_tee",
    nm_mdmprxy_lbo_ifacejump_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_mngl_pre_tee -j TEE --gateway %s --oif %s",
    "%s %s -t mangle -D nm_mdmprxy_mngl_pre_tee -j TEE --gateway %s --oif %s",
    nm_mdmprxy_lbo_formatter },

  { "%s %s -t mangle -A nm_mdmprxy_mngl_pre_tee -j DROP",
    "%s %s -t mangle -D nm_mdmprxy_mngl_pre_tee -j DROP",
    nm_mdmprxy_basic_formatter }
};

static const struct nm_mdmprxy_cmd vpn_skip_rule_list[] =
{
  { "%s add from %s to %s iif lo fwmark 0x0/0x20000 goto 13000 prio 11999",
    "%s del from %s to %s iif lo fwmark 0x0/0x20000 goto 13000 prio 11999",
    nm_mdmprxy_vpn_skip_formatter }
};

struct route_prov_iptables legacy_prov_iptables_cmd_tbl;

static int
nm_mdmprxy_cleanup_policy_routing_rules
(
  sa_family_t  ip_family,
  boolean      force
);

static int
nm_mdmprxy_cleanup_default_routing_rule
(
  sa_family_t    ip_family,
  boolean        force
);

static int
nm_mdmprxy_cleanup_rev_rmnet_policy_routing_rule
(
  sa_family_t    af,
  int            link,
  const char*    link_name
);

static int
nm_mdmprxy_cleanup_lb_policy_routing_rule
(
  sa_family_t     af,
  int             link,
  const char*     link_name,
  boolean         force
);

static int
nm_mdmprxy_cleanup_s2b_policy_routing_rule
(
  sa_family_t ip_family,
  int link,
  ipsec_tunnel_info_t *tunnel_info,
  boolean force
);

/*---------------------------------------------------------------------------
                            IPTABLES FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_install_iiface_rules
===========================================================================*/
/*!
@brief
  Installs iptables rules for routing marked packets

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_install_iiface_rules
(
  int                  link,
  char*                link_name,
  int                  tech_type,
  int                  valid4,
  uint32_t             daddr4,
  int                  valid6,
  struct in6_addr      *daddr6,
  uint32_t             mask6,
  ipsec_tunnel_info_t  *tunnel
)
{
  int ip_family;
  struct in6_addr gwaddr6;
  inet_pton(AF_INET6, NM_MDMPRXY_GATEWAY_V6_ADDR, &gwaddr6);

  NM_MDMPRXY_VAR_UNUSED(tunnel);

  netmgr_log_med("%s(): link_name [%s]", __func__, link_name);

  /* Install TEE rule for local breakout */
  if (NETMGR_KIF_CB_IF_TECH_TYPE_LBO == tech_type)
  {
    if (valid4)
    {
      ip_family = AF_INET;
    }
    else if (valid6)
    {
      ip_family = AF_INET6;
    }
    else
    {
      netmgr_log_err("%s(): bad ip_family!", __func__);
      return NETMGR_FAILURE;
    }

    if (NETMGR_SUCCESS
          != nm_mdmprxy_install_local_breakout_rules(link_name,
                                                     NM_MDMPRXY_IIFACE,
                                                     NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                     &gwaddr6,
                                                     ip_family))
    {
      netmgr_log_err("%s(): Failed to install TEE rule for local-breakout iface!", __func__);
    }
  }
  else if ((NETMGR_KIF_CB_IF_TECH_TYPE_EPDG == tech_type) &&
           (0 == iiface_epdg_refcount))
  {
    if (NETMGR_SUCCESS != nm_mdmprxy_install_vpn_skip_rule(link_name,
                                                           tech_type,
                                                           tunnel))
    {
      netmgr_log_err("%s(): Failed to install vpn skip rule for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IP[%d] vpn skip rule for link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }

    if (NETMGR_SUCCESS != nm_mdmprxy_common_install_dl_ping6_fwmark_rules(link_name,
                                                                          tech_type,
                                                                          tunnel))
    {
      netmgr_log_err("%s(): Failed to install DL ping6 fwmark rules for tunnel=%d, link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed DL ping6 fwmark rules for tunnel=%d, link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }
  }

  /* Install the destination based rules, if any */
  if (valid4)
  {
    nm_mdmprxy_install_oiface_rules(iiface_handle,
                                    link,
                                    (const char*) link_name,
                                    TRUE,
                                    NM_MDMPRXY_V4ADDR_INVALID,
                                    daddr4,
                                    FALSE,
                                    NULL,
                                    NULL,
                                    0);

    netmgr_log_med("%s(): Installed IPv4 oif rules for link [%s]",
                   __func__, link_name);
  }
  else if (valid6)
  {
    nm_mdmprxy_install_oiface_rules(iiface_handle,
                                    link,
                                    (const char*) link_name,
                                    FALSE,
                                    NM_MDMPRXY_V4ADDR_INVALID,
                                    NM_MDMPRXY_V4ADDR_INVALID,
                                    TRUE,
                                    NULL,
                                    daddr6,
                                    mask6);

    netmgr_log_med("%s(): Installed IPv6 oif rules for link [%s]",
                   __func__, link_name);

    if (NETMGR_SUCCESS != nm_mdmprxy_common_install_per_iface_dl_ping6_rules(link_name,
                                                                             tech_type,
                                                                             daddr6,
                                                                             mask6))
    {
      netmgr_log_err("%s(): Failed to install IPv6 DL ping6 rules for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IPv6 DL ping6 rules for link=%s!",
                     __func__,
                     link_name);
    }
  }
  else
  {
    netmgr_log_err("%s(): Unhandled AF; skipping config", __func__);
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_remove_iiface_rules
===========================================================================*/
/*!
@brief
  Removes iptables rules for routing marked packets

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_remove_iiface_rules
(
  int                           link,
  char                          *link_name,
  int                           tech_type,
  int                           ip_family,
  ipsec_tunnel_info_t           *tunnel,
  struct netmgr_kif_addrstorage *link_addr
)
{
  struct in6_addr gwaddr6;
  inet_pton(AF_INET6, NM_MDMPRXY_GATEWAY_V6_ADDR, &gwaddr6);

  NM_MDMPRXY_VAR_UNUSED(ip_family);
  NM_MDMPRXY_VAR_UNUSED(tunnel);

  /* Remove destination based rules, if any */
  nm_mdmprxy_uninstall_oiface_rules(iiface_handle, link, ip_family);

  /* Remove TEE rule for local breakout */
  if (NETMGR_KIF_CB_IF_TECH_TYPE_LBO == tech_type)
  {
    if (NETMGR_SUCCESS
          != nm_mdmprxy_remove_local_breakout_rules(link_name,
                                                    NM_MDMPRXY_IIFACE,
                                                    NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                    &gwaddr6,
                                                    ip_family,
                                                    (int) nm_mdmprxy_rules_len(local_breakout_rule_list) - 1))
    {
      netmgr_log_err("%s(): Failed to install TEE rule for local-breakout iface!", __func__);
    }
  }
  else if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG == tech_type)
  {
    if (0 == iiface_epdg_refcount)
    {
      if (NETMGR_SUCCESS != nm_mdmprxy_remove_vpn_skip_rule(link_name,
                                                            tech_type,
                                                            tunnel))
      {
        netmgr_log_err("%s(): Failed to remove tunnel[%d] vpn skip rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed tunnel[%d] vpn skip rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }

      if (NETMGR_SUCCESS != nm_mdmprxy_common_remove_dl_ping6_fwmark_rules(link_name,
                                                                           tech_type,
                                                                           tunnel))
      {
        netmgr_log_err("%s(): Failed to remove DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
    }

    if (AF_INET6 == ip_family)
    {
      if (NETMGR_SUCCESS != nm_mdmprxy_common_remove_per_iface_dl_ping6_rules(link_name,
                                                                              tech_type,
                                                                              &link_addr->_in6.addr,
                                                                              link_addr->_in6.mask))
      {
        netmgr_log_err("%s(): Failed to remove DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
    }
  }

  /* TODO: The original design was assuming that the refcount being = 1 means that the iface
     being cleaned up was an iiface. However, there is no gurantee that the last iface being
     disconnected is the iiface. If there are other oifaces still up and we get a disconnect
     on the iiface earlier to getting disconnect on oiface then we  are not cleaning up  the
     associated oiface rules */
  if (iiface_lbo_refcount == 0)
  {
    /* If this is the last rev rmnet iface then remove the jump rule from PREROUTING */
    nm_mdmprxy_uninstall_iiface_rules(iiface_handle);
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_iptables_init
===========================================================================*/
/*!
@brief
  Initializes the iptables component of the route provider

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_prov_iptables_init()
{
  nm_mdmprxy_iptables_init();

  iptables_inited = TRUE;

  netmgr_log_low("%s(): Initialization finished\n", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_iptables_deinit
===========================================================================*/
/*!
@brief
  De-nitializes the iptables component of the route provider

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_prov_iptables_deinit()
{
  /* This removes both static and iiface + oiface rules. This mechanism will be
     used till we fix the SSR handling within core */
  nm_mdmprxy_iptables_uninit();

  netmgr_log_low("%s(): De-initialization finished\n", __func__);
}

/*---------------------------------------------------------------------------
                            ROUTING FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_policy_routing_rules
===========================================================================*/
/*!
@brief
  Install routing policy rules for forwarding Modem destined traffic

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_install_policy_routing_rules
(
  sa_family_t ip_family
)
{
  unsigned int i, num_rules = 0;
  int rc = NETMGR_SUCCESS;
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char **ip_rules = NULL;
  const char *ip4_rules[] =
  {
    /* IPv4 rules */
    "ip rule add from all lookup local prio 1" ";"
    "ip rule del from all lookup local prio 0" ";"
    "ip rule add fwmark " strstr(NM_MDMPRXY_MARK) " table " strstr(NM_MDMPRXY_MARK) " prio 0" ";"
    "ip rule add from all lookup local prio 0" ";"
    "ip rule del from all lookup local prio 1" ";"
  };
  const char *ip6_rules[] =
  {
    /* IPv6 rules */
    "ip -6 rule add from all lookup local prio 1" ";"
    "ip -6 rule del from all lookup local prio 0" ";"
    "ip -6 rule add fwmark " strstr(NM_MDMPRXY_MARK) " table " strstr(NM_MDMPRXY_MARK) " prio 0" ";"
    "ip -6 rule add from all lookup local prio 0" ";"
    "ip -6 rule del from all lookup local prio 1" ";"
  };

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (TRUE == NM_MDMPRXY_GET_POLICY_ROUTING_INSTALLED(ip_family))
  {
    netmgr_log_med("nm_mdmprxy_install_policy_routing_rules: already installed for ip_family=%d!\n",
                   ip_family);
    goto ret;
  }

  /* Attempt to clean-up any dangling rules */
  (void) nm_mdmprxy_cleanup_policy_routing_rules(ip_family, TRUE);

  if (AF_INET == ip_family)
  {
    ip_rules = ip4_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip4_rules);
  }
  else
  {
    ip_rules = ip6_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip6_rules);
  }

  for (i = 0; i < num_rules; ++i)
  {
    memset(err_str, 0, sizeof(err_str));

    if (netmgr_ds_exec(ip_rules[i], &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, err: %s\n", ip_rules[i], err_str);
      rc = NETMGR_FAILURE;
      break;
    }
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_POLICY_ROUTING_INSTALLED(ip_family, TRUE);
    netmgr_log_med("%s(): installation complete for ip_family=%d\n",
                   __func__, ip_family);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_forwarding_rules
===========================================================================*/
/*!
@brief
  Installs the forwarding rules for the given reverse rmnet link to enable
  forwarding the downlink traffic to the modem

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
LOCAL int
nm_mdmprxy_install_forwarding_rules
(
  sa_family_t ip_family,
  int link,
  const char* link_name,
  const char* addr
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  char *ip_type = NULL;
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (AF_INET == ip_family)
  {
    ip_type = "-4";
  }
  else if (AF_INET6 == ip_family)
  {
    ip_type = "-6";
  }
  else
  {
    netmgr_log_err("%s(): unknown ip_family=%d\n", __func__, ip_family);
    goto bail;
  }

  if (!link_name)
  {
    netmgr_log_err("%s(): failed to obtain link name for link=%d\n",
                   __func__, link);
    goto bail;
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s route add table " strstr(NM_MDMPRXY_MARK) " %s dev %s",
           ip_type,
           addr,
           link_name);

  netmgr_log_low("%s(): installing ip forwarding rule for link=%s type=%s\n",
                 __func__, link_name, ip_type);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("%s(): cmd: %s failed, err: %s\n", __func__, cmd, err_str);
    netmgr_l2s_format_msg(link, "install_fwdg_rules\\nV%d failed",
                          (ip_family == AF_INET ? 4 : 6));
    goto bail;
  }

  netmgr_l2s_format_msg(link, "install_fwdg_rules\\nV%d complete",
                        (ip_family == AF_INET ? 4 : 6));
  NM_MDMPRXY_SET_FORWARDING_RULES_INSTALLED(ip_family, TRUE);
  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_default_routing_rule
===========================================================================*/
/*!
@brief
  Install default routing rule in our custom table to enable forwarding
  of Modem originated traffic over WLAN

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_default_routing_rule
(
  sa_family_t ip_family
)
{
  unsigned int i, num_rules = 0;
  int rc = NETMGR_SUCCESS;
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char **ip_rules = NULL;
  const char *ip4_rules[] =
  {
    /* IPv4 default routing rule */
    "ip route add default dev wlan0 table " strstr(NM_MDMPRXY_MARK)
  };
  const char *ip6_rules[] =
  {
    /* IPv6 default routing rule */
    "ip -6 route add default dev wlan0 table " strstr(NM_MDMPRXY_MARK)
  };

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (TRUE == NM_MDMPRXY_GET_DEFAULT_ROUTING_INSTALLED(ip_family))
  {
    netmgr_log_med("%s(): already installed for ip_family=%d!\n",
                   __func__, ip_family);
    goto ret;
  }

  /* Attempt to clean-up any dangling rules */
  (void) nm_mdmprxy_cleanup_default_routing_rule(ip_family, TRUE);

  if (AF_INET == ip_family)
  {
    ip_rules = ip4_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip4_rules);
  }
  else if (AF_INET6 == ip_family)
  {
    ip_rules = ip6_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip6_rules);
  }
  else
  {
    netmgr_log_err("%s(): Unknown family!", __func__);
    rc = NETMGR_FAILURE;
    goto ret;
  }

  for (i = 0; i < num_rules; ++i)
  {
    memset(err_str, 0, sizeof(err_str));

    if (netmgr_ds_exec(ip_rules[i], &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, err: %s\n", ip_rules[i], err_str);
      rc = NETMGR_FAILURE;
      break;
    }
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_DEFAULT_ROUTING_INSTALLED(ip_family, TRUE);
    netmgr_log_med("%s(): installation complete for ip_family=%d\n",
                   __func__, ip_family);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_rev_rmnet_policy_routing_rule
===========================================================================*/
/*!
@brief
  Install routing policy rules for reverse rmnet interfaces to use custom
  table for modem originated traffic on those interfaces

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_install_rev_rmnet_policy_routing_rule
(
  sa_family_t ip_family,
  int link,
  const char* link_name
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  /* Attempt to clean-up any dangling rules */
  (void) nm_mdmprxy_cleanup_rev_rmnet_policy_routing_rule(ip_family, link, link_name);

  if (AF_INET == ip_family)
  {
    ip_type = "-4";
  }
  else if (AF_INET6 == ip_family)
  {
    ip_type = "-6";
  }
  else
  {
    netmgr_log_err("%s(): Unknown family!", __func__);
    rc = NETMGR_FAILURE;
    goto ret;
  }

  if (!link_name)
  {
    netmgr_log_err("failed to obtain link name for link=%d\n",
                   link);
    goto ret;
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule add from all iif %s lookup " strstr(NM_MDMPRXY_MARK) " prio 0",
           ip_type,
           link_name);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("cmd: %s failed with err=%s for link=%d\n",
                   cmd,
                   err_str,
                   link);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    netmgr_log_med("%s(): installation complete for link=%d ip_family=%d\n",
                   __func__, link, ip_family);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_lb_policy_routing_rule
===========================================================================*/
/*!
@brief
  Install routing policy rules for local breakout interface to always use
  wlan0 table for local breakout traffic

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_install_lb_policy_routing_rule
(
  sa_family_t  ip_family,
  int          link,
  const char*  link_name
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;
  int retry_cnt = 0;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (TRUE == NM_MDMPRXY_GET_LB_POLICY_ROUTING_INSTALLED(ip_family))
  {
    netmgr_log_med("%s(): already installed for ip_family=%d!\n",
                   __func__, ip_family);
    goto ret;
  }

  /* Attempt to clean-up any dangling rules */
  (void) nm_mdmprxy_cleanup_lb_policy_routing_rule(ip_family, link, link_name, TRUE);

  if (AF_INET == ip_family)
  {
    ip_type = "-4";
  }
  else
  {
    ip_type = "-6";
  }

  if (!link_name)
  {
    netmgr_log_err("failed to obtain link name for link=%d\n",
                   link);
    goto ret;
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule add from all iif %s lookup wlan0 prio 0",
           ip_type,
           link_name);

  while (NM_MDMPRXY_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY != retry_cnt)
  {
    if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
    {
      if (std_strlen(err_str) != 0)
      {
        /* There might be a delay in "wlan0" table being available at the time
           of rule installation but it gets created shortly afterwards. To handle
           such a scenario we will attempt to retry rule installation a few times
           before giving up */
        retry_cnt++;

        memset(err_str, 0, sizeof(err_str));
        sleep(1);
        netmgr_log_low("%s(): Failed to setup lb policy routing...retrying", __func__);
        continue;
      }
    }

    /* If there is nothing in the error buffer then the command succeeded */
    break;
  }

  if (NM_MDMPRXY_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY == retry_cnt)
  {
    netmgr_log_err("%s(): Failed to install local-breakout routing rules!", __func__);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_LB_POLICY_ROUTING_INSTALLED(ip_family, TRUE);
    netmgr_log_med("%s(): installation complete\n", __func__);
  }

ret:
  netmgr_log_med("%s(): status for link=%d ip_family=%d lb_policy_routing=%d\n",
                 __func__, link, ip_family,
                 NM_MDMPRXY_GET_LB_POLICY_ROUTING_INSTALLED(ip_family));

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_s2b_policy_routing_rule
===========================================================================*/
/*!
@brief
  Install routing policy rules for iWLAN S2B interface to always use
  wlan0 table for tunneled traffic

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_install_s2b_policy_routing_rule
(
  sa_family_t ip_family,
  int link,
  ipsec_tunnel_info_t *tunnel_info
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;
  char *local_addr = NULL, *dest_addr = NULL;
  int tunnel_family = 0;
  int retry_cnt = 0;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (!tunnel_info)
  {
    netmgr_log_err("%s(): Invalid params!", __func__);
    goto ret;
  }

  if (!tunnel_info->local_addr)
  {
    netmgr_log_med("%s(): Tunnel endpoint addresses are invalid for link [%d] ip_family [%d],"
                   " skipping rule installation",
                   __func__, link, ip_family);
    goto ret;
  }

  if (!tunnel_info->dest_addr)
  {
    netmgr_log_med("%s(): Tunnel endpoint addresses are invalid for link [%d] ip_family [%d],"
                   " skipping rule installation",
                   __func__, link, ip_family);
    goto ret;
  }

  local_addr = malloc(NETMGR_MAX_STR_LENGTH);

  if (!local_addr)
  {
    netmgr_log_err("%s(): Out of memory!", __func__);
    goto ret;
  }

  dest_addr  = malloc(NETMGR_MAX_STR_LENGTH);

  if (!dest_addr)
  {
    netmgr_log_err("%s(): Out of memory!", __func__);
    goto ret;
  }

  strlcpy(local_addr, tunnel_info->local_addr, (ssize_t)(NETMGR_MAX_STR_LENGTH));
  strlcpy(dest_addr, tunnel_info->dest_addr, (ssize_t)(NETMGR_MAX_STR_LENGTH));
  tunnel_family = tunnel_info->tunnel_family;

  netmgr_log_med("%s(): tunnel endpoints for link=%d tunnel_family=%d local=%s dest=%s!\n",
                 __func__, link, tunnel_family, local_addr, dest_addr);

  if (TRUE == NM_MDMPRXY_GET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family))
  {
    netmgr_log_med("%s(): already installed for ip_family=%d!\n",
                   __func__, ip_family);
    goto ret;
  }

  /* Attempt to clean-up any dangling rules */
  (void) nm_mdmprxy_cleanup_s2b_policy_routing_rule(ip_family, link, tunnel_info, TRUE);

  if (AF_INET == tunnel_family)
  {
    ip_type = "-4";
  }
  else
  {
    ip_type = "-6";
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule add from %s to %s lookup wlan0 prio 0",
           ip_type,
           local_addr,
           dest_addr);

  while (NM_MDMPRXY_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY != retry_cnt)
  {
    if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
    {
      if (std_strlen(err_str) != 0)
      {
        /* There might be a delay in "wlan0" table being available at the time
           of rule installation but it gets created shortly afterwards. To handle
           such a scenario we will attempt to retry rule installation a few times
           before giving up */
        retry_cnt++;

        memset(err_str, 0, sizeof(err_str));
        sleep(1);
        netmgr_log_low("%s(): Failed to setup s2b policy routing...retrying", __func__);
        continue;
      }
    }

    break;
  }

  if (NM_MDMPRXY_WLAN_DEFAULT_ROUTE_ADD_MAX_RETRY == retry_cnt)
  {
    netmgr_log_err("%s(): Failed to install s2b routing rules!", __func__);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family, TRUE);
    netmgr_log_med("%s(): installation complete\n", __func__);
  }

ret:
  netmgr_log_med("%s(): status for link=%d tunnel_family=%d s2b_policy_routing=%d\n",
                 __func__, link, tunnel_family,
                 NM_MDMPRXY_GET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family));

  if (local_addr)
  {
    free(local_addr);
  }

  if (dest_addr)
  {
    free(dest_addr);
  }

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_forwarding_rules
===========================================================================*/
/*!
@brief
  Cleanup routing rules in the forwarding table

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_forwarding_rules
(
  int      ip_family,
  boolean  force
)
{
  unsigned int i, num_rules = 0;
  int rc = NETMGR_SUCCESS;
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char **ip_rules = NULL;
  const char *ip4_rules[] =
  {
    /* Flush the IPv4 forwarding table */
    "ip -4 route flush table " strstr(NM_MDMPRXY_MARK)
  };
  const char *ip6_rules[] =
  {
    /* Flush the IPv6 forwarding table */
    "ip -6 route flush table " strstr(NM_MDMPRXY_MARK)
  };

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (FALSE == force && FALSE == NM_MDMPRXY_GET_FORWARDING_RULES_INSTALLED(ip_family))
  {
    netmgr_log_med("nm_mdmprxy_cleanup_forwarding_rules: already cleaned-up for ip_family=%d!\n",
                   ip_family);
    goto ret;
  }

  if (AF_INET == ip_family)
  {
    ip_rules = ip4_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip4_rules);
  }
  else
  {
    ip_rules = ip6_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip6_rules);
  }

  for (i = 0; i < NM_MDMPRXY_ARR_SIZE(ip_rules); ++i)
  {
    memset(err_str, 0, sizeof(err_str));

    if (netmgr_ds_exec(ip_rules[i], &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, err: %s\n", ip_rules[i], err_str);
      rc = NETMGR_FAILURE;
      break;
    }
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_FORWARDING_RULES_INSTALLED(ip_family, FALSE);
    netmgr_log_med("nm_mdmprxy_cleanup_forwarding_rules: clean-up complete for ip_family=%d\n",
                   ip_family);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_policy_routing_rules
===========================================================================*/
/*!
@brief
  Cleanup (previously installed) custom routing policy rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_policy_routing_rules
(
  sa_family_t  ip_family,
  boolean      force
)
{
  unsigned int i, num_rules = 0;
  int rc = NETMGR_SUCCESS;
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char **ip_rules = NULL;
  const char *ip4_rules[] =
  {
    /* IPv4 rules */
    "ip rule del fwmark " strstr(NM_MDMPRXY_MARK) " table " strstr(NM_MDMPRXY_MARK) " prio 0"
  };
  const char *ip6_rules[] =
  {
    /* IPv6 rules */
    "ip -6 rule del fwmark " strstr(NM_MDMPRXY_MARK) " table " strstr(NM_MDMPRXY_MARK) " prio 0"
  };

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (FALSE == force && FALSE == NM_MDMPRXY_GET_POLICY_ROUTING_INSTALLED(ip_family))
  {
    netmgr_log_med("nm_mdmprxy_cleanup_policy_routing_rules: already cleaned-up for ip_family=%d!\n",
                   ip_family);
    goto ret;
  }

  if (AF_INET == ip_family)
  {
    ip_rules = ip4_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip4_rules);
  }
  else
  {
    ip_rules = ip6_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip6_rules);
  }

  for (i = 0; i < NM_MDMPRXY_ARR_SIZE(ip_rules); ++i)
  {
    memset(err_str, 0, sizeof(err_str));
    if (netmgr_ds_exec(ip_rules[i], &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, err: %s\n", ip_rules[i], err_str);
      rc = NETMGR_FAILURE;
      break;
    }
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_POLICY_ROUTING_INSTALLED(ip_family, FALSE);
    netmgr_log_med("nm_mdmprxy_cleanup_policy_routing_rules: clean-up complete for ip_family=%d\n",
                   ip_family);
  }

ret:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_forwarding_rules
===========================================================================*/
/*!
@brief
  Removes the forwarding rules for the given reverse rmnet link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
LOCAL int
nm_mdmprxy_remove_forwarding_rules
(
  sa_family_t    ip_family,
  int            link,
  const char     *link_name,
  const char*    addr
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  char *ip_type = NULL;
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (AF_INET == ip_family)
  {
    ip_type = "-4";
  }
  else if (AF_INET6 == ip_family)
  {
    ip_type = "-6";
  }
  else
  {
    netmgr_log_err("%s(): unknown ip_family=%d\n",
                   __func__, ip_family);
    goto bail;
  }

  if (!link_name)
  {
    netmgr_log_err("%s(): failed to obtain link name for link=%d\n",
                   __func__, link);
    goto bail;
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s route del table " strstr(NM_MDMPRXY_MARK) " %s dev %s",
           ip_type,addr,link_name);

  netmgr_log_med("%s(): removing ip forwarding rule for link=%s, type=%s\n",
                 __func__, link_name, ip_type);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("%s(): cmd: %s failed, err: %s\n", __func__, cmd, err_str);
    netmgr_l2s_format_msg(link, "remove_fwdg_rules\\nV%d failed",
                          (AF_INET == ip_family ? 4 : 6));
    goto bail;
  }

  netmgr_l2s_format_msg(link, "remove_fwdg_rules\\nV%d complete",
                        (AF_INET == ip_family ? 4 : 6));
  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_default_routing_rule
===========================================================================*/
/*!
@brief
  Cleanup (previously installed) default routing rule in our custom table

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_default_routing_rule
(
  sa_family_t    ip_family,
  boolean        force
)
{
  unsigned int i, num_rules = 0;
  int rc = NETMGR_SUCCESS;
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char **ip_rules = NULL;
  const char *ip4_rules[] =
  {
    /* IPv4 default routing rule */
    "ip route del default dev wlan0 table " strstr(NM_MDMPRXY_MARK)
  };
  const char *ip6_rules[] =
  {
    /* IPv6 default routing rule */
    "ip -6 route del default dev wlan0 table " strstr(NM_MDMPRXY_MARK)
  };

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (FALSE == force && FALSE == NM_MDMPRXY_GET_DEFAULT_ROUTING_INSTALLED(ip_family))
  {
    netmgr_log_med("%s(): already cleaned-up for ip_family=%d!\n",
                   __func__, ip_family);
    goto bail;
  }

  if (AF_INET == ip_family)
  {
    ip_rules = ip4_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip4_rules);
  }
  else if(AF_INET6 == ip_family)
  {
    ip_rules = ip6_rules;
    num_rules = NM_MDMPRXY_ARR_SIZE(ip6_rules);
  }
  else
  {
    netmgr_log_err("%s(): Unknown family!", __func__);
    rc = NETMGR_FAILURE;
    goto bail;
  }

  for (i = 0; i < NM_MDMPRXY_ARR_SIZE(ip_rules); ++i)
  {
    memset(err_str, 0, sizeof(err_str));

    if (netmgr_ds_exec(ip_rules[i], &opts) != DS_EXEC_OK)
    {
      netmgr_log_err("cmd: %s failed, err: %s\n", ip_rules[i], err_str);
      rc = NETMGR_FAILURE;
      break;
    }
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_DEFAULT_ROUTING_INSTALLED(ip_family, FALSE);
    netmgr_log_med("%s(): clean-up complete for ip_family=%d\n",
                   __func__, ip_family);
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_rev_rmnet_policy_routing_rule
===========================================================================*/
/*!
@brief
  Cleanup routing policy rules for reverse rmnet traffic to always use
  custom table for routing

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_rev_rmnet_policy_routing_rule
(
  sa_family_t    af,
  int            link,
  const char*    link_name
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (AF_INET == af)
  {
    ip_type = "-4";
  }
  else if (AF_INET6 == af)
  {
    ip_type = "-6";
  }
  else
  {
    netmgr_log_err("%s(): Unknown family!", __func__);
    rc = NETMGR_FAILURE;
    goto bail;
  }

  if (!link_name)
  {
    netmgr_log_err("failed to obtain link name for link=%d\n",
                   link);
    goto bail;
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule del from all iif %s prio 0",
           ip_type,
           link_name);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("cmd: %s failed with err=%s for link=%d\n",
                   cmd,
                   err_str,
                   link);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    netmgr_log_med("%s(): clean-up complete for link=%d ip_family=%d\n",
                   __func__, link, af);
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_lb_policy_routing_rule
===========================================================================*/
/*!
@brief
  Cleanup routing policy rules for local breakout interface to always use
  wlan0 table for local breakout traffic

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_lb_policy_routing_rule
(
  sa_family_t     af,
  int             link,
  const char*     link_name,
  boolean         force
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (FALSE == force && FALSE == NM_MDMPRXY_GET_LB_POLICY_ROUTING_INSTALLED(af))
  {
    netmgr_log_med("%s(): already cleaned-up for ip_family=%d!\n",
                   __func__, af);
    goto ret;
  }

  if (AF_INET == af)
  {
    ip_type = "-4";
  }
  else
  {
    ip_type = "-6";
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule del from all iif %s prio 0",
           ip_type,
           link_name);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("cmd: %s failed with err=%s for link=%d\n",
                   cmd,
                   err_str,
                   link);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_LB_POLICY_ROUTING_INSTALLED(af, FALSE);
    netmgr_log_med("%s(): clean-up complete\n", __func__);
  }

ret:
  netmgr_log_med("%s(): status for link=%d ip_family=%d lb_policy_routing=%d\n",
                 __func__, link, af,
                 NM_MDMPRXY_GET_LB_POLICY_ROUTING_INSTALLED(af));

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_cleanup_s2b_policy_routing_rule
===========================================================================*/
/*!
@brief
  Cleanup routing policy rules for iWLAN S2B interface to always use
  wlan0 table for tunneled traffic

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_cleanup_s2b_policy_routing_rule
(
  sa_family_t ip_family,
  int link,
  ipsec_tunnel_info_t *tunnel_info,
  boolean force
)
{
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  char err_str[NETMGR_MAX_COMMAND_LENGTH * 3] = "";
  const char *ip_type = NULL;
  int rc = NETMGR_SUCCESS;
  char *local_addr = NULL, *dest_addr = NULL;
  int tunnel_family = 0;

  NETMGR_LOG_FUNC_ENTRY;

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = err_str;
  opts.stderr_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  NM_MDMPRXY_VAR_UNUSED(ip_family);

  if (!tunnel_info)
  {
    netmgr_log_err("%s(): Invalid params!", __func__);
    goto ret;
  }

  if (!tunnel_info->local_addr)
  {
    netmgr_log_med("%s(): Tunnel endpoint addresses are invalid for link [%d] ip_family [%d],"
                   " skipping rule deletion",
                   __func__, link, ip_family);
    goto ret;
  }

  if (!tunnel_info->dest_addr)
  {
    netmgr_log_med("%s(): Tunnel endpoint addresses are invalid for link [%d] ip_family [%d],"
                   " skipping rule deletion",
                   __func__, link, ip_family);
    goto ret;
  }

  local_addr = malloc(NETMGR_MAX_STR_LENGTH);

  if (!local_addr)
  {
    netmgr_log_err("%s(): Out of memory!", __func__);
    goto ret;
  }

  dest_addr  = malloc(NETMGR_MAX_STR_LENGTH);

  if (!dest_addr)
  {
    netmgr_log_err("%s(): Out of memory!", __func__);
    goto ret;
  }

  strlcpy(local_addr, tunnel_info->local_addr, (ssize_t)(NETMGR_MAX_STR_LENGTH));
  strlcpy(dest_addr, tunnel_info->dest_addr, (ssize_t)(NETMGR_MAX_STR_LENGTH));
  tunnel_family = tunnel_info->tunnel_family;

  netmgr_log_med("%s(): tunnel endpoints for link=%d tunnel_family=%d local=%s dest=%s!\n",
                 __func__, link, tunnel_family, local_addr, dest_addr);

  if (FALSE == force && FALSE == NM_MDMPRXY_GET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family))
  {
    netmgr_log_med("%s(): already cleaned-up for tunnel_family=%d!\n",
                   __func__, tunnel_family);
    goto ret;
  }

  if (AF_INET == tunnel_family)
  {
    ip_type = "-4";
  }
  else
  {
    ip_type = "-6";
  }

  snprintf(cmd,
           sizeof(cmd),
           "ip %s rule del from %s to %s prio 0",
           ip_type,
           local_addr,
           dest_addr);

  if (netmgr_ds_exec(cmd, &opts) != DS_EXEC_OK)
  {
    netmgr_log_err("cmd: %s failed with err=%s for link=%d\n",
                   cmd,
                   err_str,
                   link);
    rc = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family, FALSE);
    netmgr_log_med("%s(): clean-up complete\n", __func__);
  }

ret:
  netmgr_log_med("%s(): status for link=%d tunnel_family=%d s2b_policy_routing=%d\n",
                 __func__, link, tunnel_family,
                 NM_MDMPRXY_GET_S2B_POLICY_ROUTING_INSTALLED(tunnel_family));

  if (local_addr)
  {
    free(local_addr);
  }

  if (dest_addr)
  {
    free(dest_addr);
  }

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_install_routes
===========================================================================*/
/*!
@brief
  Setup routing rule for the provided link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_install_routes
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
)
{
  int rc = NETMGR_FAILURE;

  char link_address[INET6_ADDRSTRLEN*2];
  char prefix[8];

  if (!link_name || !link_addr)
  {
    netmgr_log_err("%s(): Invalid params received!", __func__);
    goto error;
  }

  /* Convert the address to string */
  if (AF_INET == af)
  {
    if (link_addr->type != NETMGR_KIF_CB_ADDR_IPV4)
    {
      netmgr_log_err("%s(): Address type mismatch!", __func__);
      goto error;
    }

    inet_ntop(af, &link_addr->_in4.addr, link_address, INET6_ADDRSTRLEN);
  }
  else if (AF_INET6 == af)
  {
    if (link_addr->type != NETMGR_KIF_CB_ADDR_IPV6)
    {
      netmgr_log_err("%s(): Address type mismatch!", __func__);
      goto error;
    }

    inet_ntop(af, &link_addr->_in6.addr, link_address, INET6_ADDRSTRLEN);
    if (link_addr->_in6.mask)
    {
      snprintf(prefix, 8, "/%u", link_addr->_in6.mask);
      strlcat(link_address, prefix, INET6_ADDRSTRLEN);
    }
  }

  /* There are two callflows where we install policy routing rules:
     1. For modem only embedded calls, when WLAN is connected
     2. For apps call when the first iWLAN call (based on tech-type) comes up
   */

  if (iiface_lbo_refcount == 0
      || iiface_epdg_refcount == 0)
  {
    /* Install prio 0 rule in ip rules to forward packets marked with modem mark over table 9 */
    if ((iiface_lbo_refcount + iiface_epdg_refcount) == 0)
    {
      /* Install rthe main prio 0 routing rules only for the first iWLAN call */
      if (NETMGR_SUCCESS != nm_mdmprxy_install_policy_routing_rules(af))
      {
        netmgr_log_err("%s(): failed to install policy routing rules!", __func__);
        goto error;
      }
    }

    switch (tech_type)
    {
    case NETMGR_KIF_CB_IF_TECH_TYPE_LBO:
      /* For local-breakout we need to install lbo policy rules at prio 0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_install_lb_policy_routing_rule(af, link, link_name))
      {
        netmgr_log_err("%s(): failed to install lb policy routing rules for link [%s]",
                       __func__, link_name);
        goto error;
      }

      /* Increment LBO refcount */
      iiface_lbo_refcount++;

      break;

    case NETMGR_KIF_CB_IF_TECH_TYPE_EPDG:
      /* Install s2b policy rules for other reverse links at prio 0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_install_s2b_policy_routing_rule(af, link, tunnel_info))
      {
        netmgr_log_err("%s(): failed to install lb policy routing rules for link [%s]",
                       __func__, link_name);
        goto error;
      }

      /* Install default route for wlan0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_install_default_routing_rule(af))
      {
         netmgr_log_err("%s(): failed to install default routing rule link=%s ip_family=%d\n",
                        __func__, link_name, af);
         goto error;
      }

      /* Increment the epdg refcount */
      iiface_epdg_refcount++;

      break;

    default:
      netmgr_log_low("%s(): Unknown tech-type, skipping", __func__);
      break;
    }
  }

  /* Install prio 0 rule for all incoming packets on rev rmnets to lookup table 9 */
  if (NETMGR_SUCCESS != nm_mdmprxy_install_rev_rmnet_policy_routing_rule(af, link, link_name))
  {
    netmgr_log_err("%s(): Failed to install rev rmnet policy routing rules for link [%s]",
                   __func__, link_name);
    goto error;
  }

  /* Install routing rule in table 9 for the given link */
  if (NETMGR_SUCCESS != nm_mdmprxy_install_forwarding_rules(af, link, link_name, link_address))
  {
    netmgr_log_err("%s(): Failed to install forwarding rules for link [%s]",
                   __func__, link_name);
    goto error;
  }

  rc = NETMGR_SUCCESS;

error:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_uninstall_routes
===========================================================================*/
/*!
@brief
  Remove routing rule for the provided link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_uninstall_routes
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
)
{
  int rc = NETMGR_FAILURE;

  char link_address[INET6_ADDRSTRLEN*2];
  char prefix[8];

  if (!link_name || !link_addr)
  {
    netmgr_log_err("%s(): Invalid params received!", __func__);
    goto error;
  }

  /* Convert the address to string */
  if (AF_INET == af)
  {
    if (link_addr->type != NETMGR_KIF_CB_ADDR_IPV4)
    {
      netmgr_log_err("%s(): Address type mismatch!", __func__);
      goto error;
    }

    inet_ntop(af, &link_addr->_in4.addr, link_address, INET6_ADDRSTRLEN);
  }
  else if (AF_INET6 == af)
  {
    if (link_addr->type != NETMGR_KIF_CB_ADDR_IPV6)
    {
      netmgr_log_err("%s(): Address type mismatch!", __func__);
      goto error;
    }

    inet_ntop(af, &link_addr->_in6.addr, link_address, INET6_ADDRSTRLEN);
    if (link_addr->_in6.mask)
    {
      snprintf(prefix, 8, "/%u", link_addr->_in6.mask);
      strlcat(link_address, prefix, INET6_ADDRSTRLEN);
    }
  }

  /* Remove routing rule in table 9 for the given link */
  if (NETMGR_SUCCESS != nm_mdmprxy_remove_forwarding_rules(af, link, link_name, link_address))
  {
    netmgr_log_err("%s(): Failed to remove forwarding rules for link [%s]",
                   __func__, link_name);
    goto error;
  }

  if (iiface_lbo_refcount == 1
      || iiface_epdg_refcount == 1)
  {
    /* uninnstall prio 0 rule in ip rules to forward packets marked with modem mark over table 9 */
    if ((iiface_lbo_refcount + iiface_epdg_refcount) == 1)
    {
      /* We need to remove the prio 0 rules *only* when the last iWLAN call
         goes down */
      if (NETMGR_SUCCESS != nm_mdmprxy_cleanup_policy_routing_rules(af, FALSE))
      {
        netmgr_log_err("%s(): failed to remove policy routing rules!", __func__);
        goto error;
      }
    }

    switch (tech_type)
    {
    case NETMGR_KIF_CB_IF_TECH_TYPE_LBO:
      /* For local-breakout we need to install lbo policy rules at prio 0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_cleanup_lb_policy_routing_rule(af, link, link_name, FALSE))
      {
        netmgr_log_err("%s(): failed to remove lb policy routing rules for link [%s]",
                       __func__, link_name);
        goto error;
      }

      /* Decrement LBO refcount */
      iiface_lbo_refcount--;

      break;

    case NETMGR_KIF_CB_IF_TECH_TYPE_EPDG:
      /* Remove s2b policy rules for other reverse links at prio 0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_cleanup_s2b_policy_routing_rule(link,
                                                                       af,
                                                                       tunnel_info,
                                                                       FALSE))
      {
        netmgr_log_err("%s(): failed to remove s2b policy routing rules for link [%s]",
                       __func__, link_name);
        goto error;
      }

      /* Remove default route for wlan0 */
      if (NETMGR_SUCCESS != nm_mdmprxy_cleanup_default_routing_rule(af, FALSE))
      {
         netmgr_log_err("%s(): failed to remove default routing rule link=%s ip_family=%d\n",
                        __func__, link_name, af);
         goto error;
      }

      /* Decrement the epdg refcount */
      iiface_epdg_refcount--;

      break;

    default:
      netmgr_log_low("%s(): Unknown tech-type, skipping", __func__);
      break;
    }
  }

  /* Cleanup prio 0 rule for all incoming packets on rev rmnets to lookup table 9 */
  if (NETMGR_SUCCESS != nm_mdmprxy_cleanup_rev_rmnet_policy_routing_rule(af, link, link_name))
  {
    netmgr_log_err("%s(): Failed to remove rev rmnet policy routing rules for link [%s]",
                   __func__, link_name);
    goto error;
  }

  rc = NETMGR_SUCCESS;

error:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_netd_is_hdlr
===========================================================================*/
/*!
@brief
  Global handler for netd in-service

@return
  NETMGR_SUCCESS since legacy provider does not use netd
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_netd_is_hdlr(void)
{
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_netd_link_is_hdlr
===========================================================================*/
/*!
@brief
  Per link handler for netd in service

@return
  NETMGR_SUCCESS since legacy provider does not use netd
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_netd_link_is_hdlr(void* arg)
{
  NM_MDMPRXY_VAR_UNUSED(arg);
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_link_oos_hndlr
===========================================================================*/
/*!
@brief
  Per link OOS handler

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_link_oos_hndlr(void* arg)
{
  struct netmgr_kif_cb_ifinfo *ifinfo = (struct netmgr_kif_cb_ifinfo *) arg;
  int rc = NETMGR_FAILURE;

  if (!ifinfo)
  {
    netmgr_log_err("%s(): Invalid params...skipping deinit!", __func__);
    goto bail;
  }

  /* In modem SSR scenario where the kernel interface would have gone down
     when modem goes down, we need to ensure that the corresponding route is cleaned
     up as well */
  rc = nm_mdmprxy_legacy_prov_uninstall_routes(ifinfo->link,
                                               ifinfo->name,
                                               ifinfo->tech_type,
                                               AF_INET,
                                               &ifinfo->tunnel_info,
                                               &ifinfo->addr);

  rc |= nm_mdmprxy_legacy_prov_uninstall_routes(ifinfo->link,
                                                ifinfo->name,
                                                ifinfo->tech_type,
                                                AF_INET6,
                                                &ifinfo->tunnel_info,
                                                &ifinfo->addr);

  netmgr_log_low("%s(): complete!", __func__);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_link_is_hndlr
===========================================================================*/
/*!
@brief
  Per link IS handler

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_prov_link_is_hndlr(void* arg)
{
  /* Currently we don't have anything to handle in per link modem IS handler
     Eventually we want to move iiface + oiface rule + any other link specific
     rules to this handler */
  NM_MDMPRXY_VAR_UNUSED(arg);
  return NETMGR_SUCCESS;
}


/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_oos_hndlr
===========================================================================*/
/*!
@brief
  Global OOS handler
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_prov_oos_hndlr(void)
{
  /* Un-initialize iptables */
  if (iptables_inited)
  {
    nm_mdmprxy_legacy_prov_iptables_deinit();
  }

  nm_mdmprxy_flush_vpn_skip_rules();

  nm_mdmprxy_common_flush_dl_ping6_rules();
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_is_hndlr
===========================================================================*/
/*!
@brief
  Global IS handler

@note
  - Dependencies
    - Common module has been initialized
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_prov_is_hndlr(void)
{
  iiface_lbo_refcount  = 0;
  iiface_epdg_refcount = 0;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_prov_init
===========================================================================*/
/*!
@brief
  Init function

@note
  - Dependencies
    - Global nm_mdmprxy_common_init
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_prov_init(void)
{
  /* Reset interface counters */
  iiface_lbo_refcount  = 0;
  iiface_epdg_refcount = 0;

  memset(&legacy_prov_iptables_cmd_tbl, 0, sizeof(legacy_prov_iptables_cmd_tbl));

  /* Copy all the command arrays to the iptables helper */
  legacy_prov_iptables_cmd_tbl.static_rule_list     = static_rule_list;
  legacy_prov_iptables_cmd_tbl.static_rule_list_len = (int) nm_mdmprxy_rules_len(static_rule_list);

  legacy_prov_iptables_cmd_tbl.per_iiface_rule_list     = per_iiface_rule_list;
  legacy_prov_iptables_cmd_tbl.per_iiface_rule_list_len = (int) nm_mdmprxy_rules_len(per_iiface_rule_list);

  legacy_prov_iptables_cmd_tbl.per_oiface_rule_list     = per_oiface_rule_list;
  legacy_prov_iptables_cmd_tbl.per_oiface_rule_list_len = (int) nm_mdmprxy_rules_len(per_oiface_rule_list);

  legacy_prov_iptables_cmd_tbl.per_iface_forward_rule_list     = per_iface_forward_rule_list;
  legacy_prov_iptables_cmd_tbl.per_iface_forward_rule_list_len = (int) nm_mdmprxy_rules_len(per_iface_forward_rule_list);

  legacy_prov_iptables_cmd_tbl.local_breakout_rule_list     = local_breakout_rule_list;
  legacy_prov_iptables_cmd_tbl.local_breakout_rule_list_len = (int) nm_mdmprxy_rules_len(local_breakout_rule_list);

  legacy_prov_iptables_cmd_tbl.vpn_skip_rule_list     = vpn_skip_rule_list;
  legacy_prov_iptables_cmd_tbl.vpn_skip_rule_list_len = (int) nm_mdmprxy_rules_len(vpn_skip_rule_list);

  nm_mdmprxy_register_route_prov_cmd_tbl(&legacy_prov_iptables_cmd_tbl);

  /* Install static iptables rules */
  if (!iptables_inited)
  {
      nm_mdmprxy_legacy_prov_iptables_init();
      /* iiface rule list is nothing but the jump rules.
         The jump rules are static and can be installed at bootup.*/
      iiface_handle = nm_mdmprxy_install_iiface_rules(NM_MDMPRXY_IIFACE, NM_MDMPRXY_MARK);
  }
}
