/******************************************************************************

        N M _ M D M P R X Y _ I P T A B L E S _ H E L P E R . H

******************************************************************************/

/******************************************************************************

  @file    nm_mdmprxy_iptables_helper.h
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy plugin module is responsible for managing routing rules
  and packet marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <inttypes.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#ifndef NETMGR_OFFTARGET
#include <netinet/in6.h>
#endif

#include "netmgr.h"
#include "netmgr_kif_cb.h"

#ifdef FEATURE_DS_LINUX_ANDROID
#define NM_MDMPRXY_FLAGS " -w"
#else
#define NM_MDMPRXY_FLAGS ""
#endif

#define IPTABLES  "iptables"
#define IP6TABLES "ip6tables"

#define IPRULE  "ip -4 rule"
#define IP6RULE "ip -6 rule"

#define NM_MDMPRXY_ICMP_PROTO  "icmp"
#define NM_MDMPRXY_ICMP6_PROTO "ipv6-icmp"

#define NM_MDMPRXY_MAXPROTOSTR        10
#define NM_MDMPRXY_MAXDEVSTR          32
#define NM_MDMPRXY_MAXCMDSTR          256
#define NM_MDMPRXY_MAXRULES           2
#define NM_MDMPRXY_MAXOUTRULES        32

#define NM_MDMPRXY_OK   0
#define NM_MDMPRXY_ERR  1
#define NM_MDMPRXY_SKIP 2

#define NM_MDMPRXY_RUN_RULE     0
#define NM_MDMPRXY_RUN_ANTIRULE 1

#define NM_MDMPRXY_V4ADDR_INVALID 0

struct nm_mdmprxy_oiface_rule_info {
  int parent_handle;

  char oif[NM_MDMPRXY_MAXDEVSTR];

  int            valid4;
  int            valid6;

  struct in6_addr gw6;
  uint32_t        gw4;

  struct in6_addr daddr6;
  uint32_t        mask6;
  uint32_t        daddr4;
};

struct nm_mdmprxy_iiface_rule_info {
  int handle;
  uint32_t mark;
  char iif[NM_MDMPRXY_MAXDEVSTR];
  struct nm_mdmprxy_oiface_rule_info outrules[NM_MDMPRXY_MAXOUTRULES];
};

struct nm_mdmprxy_cmd {
  const char *rule;
  const char *antirule;
  int (*formatter)(char *, int, sa_family_t, const char*, const void *);
};

/* Information for forwarding rules in filter table */
struct iface_fwd_rule_info
{
  char iiface[NM_MDMPRXY_MAXDEVSTR];    /* Ex. Incoming iface would be r_rmnet_data0 */
  char oiface[NM_MDMPRXY_MAXDEVSTR];    /* Ex. outgoing iface would be wlan0 */
};

/* Information for UDP encap rules */
struct encap_rule_info
{
  unsigned int sport;
  unsigned int dport;
  unsigned int mark;
};

/* Information for modem ping connection tracking rules */
struct conntrack_rule_info
{
  char  proto[NM_MDMPRXY_MAXPROTOSTR];
  char  iif[NM_MDMPRXY_MAXDEVSTR];
  int   mark;
};

/* Information for SPI based filter rules */
struct spi_rule_info
{
  unsigned int spi;
  int mark;
};

/* Information for audio port-forwarding rules */
struct port_forwarding
{
  int start_port;
  int end_port;
  int mark;
};

struct lbo_rule_info
{
  char            iif[NM_MDMPRXY_MAXDEVSTR];
  char            oif[NM_MDMPRXY_MAXDEVSTR];
  struct in6_addr gw6;
  uint32_t        gw4;
};

struct tcp_mss_rule_info
{
  struct in6_addr saddr6;
  uint32_t        mask6;
  int             mss;
};

struct doze_rule_per_iface_info
{
  char link_name[NM_MDMPRXY_MAXDEVSTR];
};

struct doze_rule_tunnel_info
{
  ipsec_tunnel_info_t tunnel;
};

typedef struct doze_rule_tunnel_info vpn_skip_tunnel_info;

struct ping6_rule_info
{
  struct in6_addr daddr6;
  uint32_t        mask6;
};

struct route_prov_iptables
{
  const struct nm_mdmprxy_cmd *static_rule_list;
  const struct nm_mdmprxy_cmd *per_iiface_rule_list;
  const struct nm_mdmprxy_cmd *per_oiface_rule_list;
  const struct nm_mdmprxy_cmd *per_iface_forward_rule_list;
  const struct nm_mdmprxy_cmd *local_breakout_rule_list;
  const struct nm_mdmprxy_cmd *local_breakout_doze_rule_list;
  const struct nm_mdmprxy_cmd *per_iface_doze_skip_rule_list;
  const struct nm_mdmprxy_cmd *tunnel_doze_skip_rule_list;
  const struct nm_mdmprxy_cmd *vpn_skip_rule_list;
  int                         static_rule_list_len;
  int                         per_iiface_rule_list_len;
  int                         per_oiface_rule_list_len;
  int                         per_iface_forward_rule_list_len;
  int                         local_breakout_rule_list_len;
  int                         local_breakout_doze_rule_list_len;
  int                         per_iface_doze_skip_rule_list_len;
  int                         tunnel_doze_skip_rule_list_len;
  int                         vpn_skip_rule_list_len;
};

struct mark_prov_iptables
{
  const struct nm_mdmprxy_cmd  *udp_encap_rule_list;
  const struct nm_mdmprxy_cmd  *per_iface_conntrack_rule_list;
  int                          udp_encap_rule_list_len;
  int                          per_iface_conntrack_rule_list_len;
};

#define nm_mdmprxy_rules_len(X) (int) (sizeof(X)/sizeof(struct nm_mdmprxy_cmd))

/*===========================================================================
  FUNCTION  nm_mdmprxy_encaprulelocal_formatter
===========================================================================*/
/*!
@brief
  Formatter for IKE / ESP packet forwarding rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_encaprulelocal_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_encaprule_formatter
===========================================================================*/
/*!
@brief
  Formatter for IKE / ESP packet forwarding rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_encaprule_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_encaprule_dport_formatter
===========================================================================*/
/*!
@brief
  Formatter for IKE / ESP packet forwarding rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_encaprule_dport_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_basic_formatter
===========================================================================*/
/*!
@brief
  Formatter basic static forwarding rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_basic_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_connmark_save_formatter
===========================================================================*/
/*!
@brief
  Formatter for Connmark save rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK / NM_MDMPRXY_SKIP 

*/
/*=========================================================================*/
int
nm_mdmprxy_connmark_save_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_markjump_formatter
===========================================================================*/
/*!
@brief
  Formatter for static jump if marked rule

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int nm_mdmprxy_markjump_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_ifacejump_formatter
===========================================================================*/
/*!
@brief
  Formatter for individual iface jump rule

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int nm_mdmprxy_ifacejump_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_tee_formatter
===========================================================================*/
/*!
@brief
  Formatter TEE target rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int nm_mdmprxy_tee_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_dst_drop_formatter
===========================================================================*/
/*!
@brief
  Formatter for dropping packets after TEE

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int nm_mdmprxy_dst_drop_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void* info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_ifaceforward_incoming_formatter
===========================================================================*/
/*!
@brief
  Formatter for iface forward rule between wlan0 and r_rmnet_data*

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_ifaceforward_incoming_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_ifaceforward_outgoing_formatter
===========================================================================*/
/*!
@brief
  Formatter for iface forward rule r_rmnet_data* and wlan0

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_ifaceforward_outgoing_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_spi_formatter
===========================================================================*/
/*!
@brief
  Formatter for spi marking rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_spi_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_conntrack_formatter
===========================================================================*/
/*!
@brief
  Formatter protocol based conntrack rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_conntrack_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_port_fwd_formatter
===========================================================================*/
/*!
@brief
  Formatter for port forwarding rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_port_fwd_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_lbo_formatter
===========================================================================*/
/*!
@brief
  Formatter for local breakout rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_lbo_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_lbo_ifacejump_formatter
===========================================================================*/
/*!
@brief
  Formatter for local-brakout iface jump rule

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct nm_mdmprxy_iiface_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_lbo_ifacejump_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);
/*===========================================================================
  FUNCTION  nm_mdmprxy_doze_lbo_formatter
===========================================================================*/
/*!
@brief
  Formatter for doze lbo rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct doze_rule_per_iface_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_doze_lbo_formatter

(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_doze_oif_formatter
===========================================================================*/
/*!
@brief
  Formatter for doze oif rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct doze_rule_per_iface_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_doze_oif_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_doze_out_tunnel_formatter
===========================================================================*/
/*!
@brief
  Formatter for doze tunnel rules to protect outgoing tunneled packets

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct doze_rule_tunnel_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_doze_out_tunnel_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_doze_in_tunnel_formatter
===========================================================================*/
/*!
@brief
  Formatter for doze tunnel rules to protect incoming tunneled packets

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct doze_rule_tunnel_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_doze_in_tunnel_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_vpn_skip_formatter
===========================================================================*/
/*!
@brief
  Formatter for VPN skip rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - vpn_skip_tunnel_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_vpn_skip_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_dl_ping6_formatter
===========================================================================*/
/*!
@brief
  Formatter for DL ping6 echo request

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct doze_rule_tunnel_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_dl_ping6_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_ip_rule_formatter
===========================================================================*/
/*!
@brief
  Formatter for ip policy routing rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - vpn_skip_tunnel_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_ip_rule_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_tcp_mss_formatter
===========================================================================*/
/*!
@brief
  Formatter for TCP MSS rules

@arg buff - char buffer to store the resulting string
@arg bufflen - length of buff in bytes
@arg af - AF_INET/AF_INET6
@arg formatstr - format string; you know, the thing with the %s and %d
@arg info - struct encap_rule_info

@return
  NM_MDMPRXY_ERR / NM_MDMPRXY_OK

*/
/*=========================================================================*/
int
nm_mdmprxy_tcp_mss_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_process_rule
===========================================================================*/
/*!
@brief
  Install / remove an iptable rule (really just runs the cmd specified,
  does not have to be an iptables rule)

@arg cmd - rule chain entry to process
@arg use_antirule - TRUE/FALSE; whether or not the anti-rule should be used
@arg af - AF_INET/AF_INET6
@arg nfo - the info structure the formatter for the rule. See the individual
           formatters for the actual arguments

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
int
nm_mdmprxy_process_rule
(
  const struct nm_mdmprxy_cmd *cmd,
  int use_antirule,
  sa_family_t af,
  const void *nfo
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_register_route_prov_cmd_tbl
===========================================================================*/
/*!
@brief
  Initialize all the iptables command arrays used by route providers

*/
/*=========================================================================*/
int
nm_mdmprxy_register_route_prov_cmd_tbl
(
  struct route_prov_iptables* rule_tbl
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_register_mark_prov_cmd_tbl
===========================================================================*/
/*!
@brief
  Initialize all the iptables command arrays used by mark providers

*/
/*=========================================================================*/
int
nm_mdmprxy_register_mark_prov_cmd_tbl
(
  struct mark_prov_iptables* rule_tbl
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_iptables_init
===========================================================================*/
/*!
@brief
  Reset the iiface handles and install the static fules for AF_INET
  and AF_INET6

*/
/*=========================================================================*/
void nm_mdmprxy_iptables_init();

/*===========================================================================
  FUNCTION  nm_mdmprxy_iptables_uninit
===========================================================================*/
/*!
@brief
  Clean up all iptables rules

*/
/*=========================================================================*/
void nm_mdmprxy_iptables_uninit();

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_iiface_rules
===========================================================================*/
/*!
@brief
  Install the iiface jump rules for all AFs

@arg handle - iiface handle
@arg mark - mark to be used by subsequent rule installs

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_install_iiface_rules
(
  const char *ifname,
  uint32_t mark
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_uninstall_iiface_rules
===========================================================================*/
/*!
@brief
  Remove the iiface jump rules for all AFs

@arg handle - iiface handle

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_uninstall_iiface_rules
(
  int handle
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_uninstall_oiface_rules
===========================================================================*/
/*!
@brief
  Remove the TEE rules and such for an egress port. Removes all AFs

@arg handle - iiface handle
@arg slot - destination slot (there are one of these per RMNET interface)

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_uninstall_oiface_rules
(
  int handle,
  int slot,
  int ip_family
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_oiface_rules
===========================================================================*/
/*!
@brief
  Install the TEE rules and such for an egress port

@arg handle - iiface handle
@arg slot - destination slot (there are one of these per RMNET interface)
@arg ifname - egress iface name (e.g.. "r_rmnet_data0")
@arg valid4 - Whether or not to configure v4 rules
@arg gw4 - IPv4 gateway to use when constructing the TEE target
@arg daddr4 - IPv4 destination address to use when constructing the TEE target
@arg valid6 - Whether or not to configure v4 rules
@arg gw6 - IPv6 gateway to use when constructing the TEE target
@arg daddr6 - IPv6 destination address to use when constructing the TEE target
@arg mask6 - IPv6 prefix length

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_install_oiface_rules
(
  int             handle,
  int             slot,
  const char      *ifname,
  int             valid4,
  uint32_t        gw4,
  uint32_t        daddr4,
  int             valid6,
  struct in6_addr *gw6,
  struct in6_addr *daddr6,
  uint32_t        mask6
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_udpencap_rules
===========================================================================*/
/*!
@brief
  Wrapper around _nm_mdmprxy_install_udpencap_rules

@arg dport - the modem ephemeral port used for IKE
@arg sport - 4500
@arg mark - 9

@return
  whatever _nm_mdmprxy_install_udpencap_rules() returns

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_install_udpencap_rules
(
  int dport,
  int sport,
  int mark
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_uninstall_udpencap_rules
===========================================================================*/
/*!
@brief
  Wrapper around _nm_mdmprxy_uninstall_udpencap_rules

@arg dport - the modem ephemeral port used for IKE
@arg sport - 4500
@arg mark - 9

@return
  whatever _nm_mdmprxy_uninstall_udpencap_rules() returns

*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_uninstall_udpencap_rules
(
  int dport,
  int sport,
  int mark
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_uninstall_connmark_rules
===========================================================================*/
/*!
@brief
  Removes connection marking rule for features ex. modem pings over iwlan

@arg af     - IP family
@arg proto  - Protocol for matching
@arg iiface - incoming interface to match
@arg mark -  9

@return
  NETMGR_SUCCESS/NETMGR_FAILURE

*/
/*=========================================================================*/
/* TODO: take iiface handle instead of mark and grab the mark from there */
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_uninstall_connmark_rules
(
  sa_family_t  af,
  const char*  proto,
  const char*  iiface,
  int          mark,
  int          start
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_connmark_rules
===========================================================================*/
/*!
@brief
  Installs connection marking rule for features ex. modem pings over iwlan

@arg af     - IP family
@arg proto  - Protocol for matching
@arg ifname - incoming interface name
@arg mark -  9

@return
  NETMGR_SUCCESS/NETMGR_FAILURE

*/
/*=========================================================================*/
/* TODO: take iiface handle instead of mark and grab the mark from there */
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_install_connmark_rules
(
  sa_family_t  af,
  const char*  proto,
  const char*  ifname,
  int          mark
);

/* TODO: Till we can fix the global vs per-link handling of SSR within
   netmgr core we need to flush the connmark rules for reverse rmnet
   inside the icmp marking chain completely instead of handling it per
   link. Once the core is fixed we can rely on per-link SSR hanlding within
   the mark provider */
/*===========================================================================
  FUNCTION  nm_mdmprxy_flush_connmark_rules
===========================================================================*/
/*!
@brief
  Flushes the icmp connection marking chain when SSR is detected

@arg af     - IP family
@arg proto  - Protocol for matching
@arg ifname - incoming interface name
@arg mark -  9

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
/* TODO: This is not generic and does not belong in iptables helper */
int
nm_mdmprxy_flush_connmark_rules(sa_family_t af);

/*===========================================================================
  FUNCTION  nm_mdmprxy_flush_spi_rules
===========================================================================*/
/*!
@brief
  Flushes the SPI based packet marking chain when SSR is detected

@arg af     - IP family
@arg proto  - Protocol for matching
@arg ifname - incoming interface name
@arg mark   - 9

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_flush_spi_rules(sa_family_t af);

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_local_breakout_rules
===========================================================================*/
/*!
@brief
  Removes iptables rules specific to local breakout interface

@arg iif  - Input interface
@arg oif  - Output interface
@arg gw4  - V4 gateway address to be used to route packets
@arg gw6  - V6 gateway address to be used to route packets

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_remove_local_breakout_rules
(
  const char*     iif,
  const char*     oif,
  uint32_t        gw4,
  struct in6_addr *gw6,
  int             ip_family,
  int             start
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_local_breakout_rules
===========================================================================*/
/*!
@brief
  Installs iptables rules specific to local breakout interface

@arg iif  - Input interface
@arg oif  - Output interface
@arg gw4  - V4 gateway address to be used to route packets
@arg gw6  - V6 gateway address to be used to route packets
@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_local_breakout_rules
(
  const char*     iif,
  const char*     oif,
  uint32_t        gw4,
  struct in6_addr *gw6,
  int             ip_family
);


/*===========================================================================
  FUNCTION  nm_mdmprxy_install_local_breakout_doze_rules
===========================================================================*/
/*!
@brief
  Installs iptables rules specific to local breakout interface

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_local_breakout_doze_rules
(
  int ip_family
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_local_breakout_doze_rules
===========================================================================*/
/*!
@brief
  Removes iptables rules specific to local breakout interface

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_remove_local_breakout_doze_rules
(
  int ip_family,
  int start
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_check_iptables_socket_match_support
===========================================================================*/
/*!
@brief
  Checks whether the iptables binary supports advanced options to do
  socket-based matching

@return
  NETMGR_SUCCESS/NETMGR_FAILURE

*/
/*=========================================================================*/
int
nm_mdmprxy_check_iptables_socket_match_support(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_set_iptables_command_options
===========================================================================*/
/*!
@brief
  Checks whether iptables has support for specific command options ex. -W
*/
/*=========================================================================*/
void
nm_mdmprxy_set_iptables_command_options(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_doze_mode_skip_rules
===========================================================================*/
/*!
@brief
  Installs iptables rules (out interface) to avoid dropping of iWLAN packets
  during doze mode

@arg link_name  - Interface name
@arg ip_family  - AF_INET or AF_INET6

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_per_iface_doze_rules
(
  const char          *link_name,
  int                 ip_family
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_tunnel_doze_rules
===========================================================================*/
/*!
@brief
  Installs iptables rules (tunnel) to avoid dropping of iWLAN packets during
  doze mode

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_tunnel_doze_rules
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_per_iface_doze_rules
===========================================================================*/
/*!
@brief
  Removes iptables rules (out interface) to avoid dropping of iWLAN packets
  during doze mode

@arg link_name  - Interface name
@arg ip_family  - AF_INET or AF_INET6

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_remove_per_iface_doze_rules
(
  const char          *link_name,
  int                 ip_family
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_tunnel_doze_rules
===========================================================================*/
/*!
@brief
  Removes iptables rules (tunnel) to avoid dropping of iWLAN packets during
  doze mode

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_remove_tunnel_doze_rules
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_flush_doze_rules
===========================================================================*/
/*!
@brief
  Flushes the doze mode rules when SSR is detected

@arg af     - IP family
@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_flush_doze_rules(sa_family_t af);

/*===========================================================================
  FUNCTION  nm_mdmprxy_flush_vpn_skip_rules
===========================================================================*/
/*!
@brief
  Flushes the vpn skip rules when SSR is detected

@arg af     - IP family
@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_flush_vpn_skip_rules(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_vpn_skip_rule
===========================================================================*/
/*!
@brief
  Installs ip rule to skip VPN rules for iWLAN packets

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_install_vpn_skip_rule
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_remove_vpn_skip_rule
===========================================================================*/
/*!
@brief
  Removes ip rule to skip VPN rules for iWLAN traffic

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_remove_vpn_skip_rule
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

