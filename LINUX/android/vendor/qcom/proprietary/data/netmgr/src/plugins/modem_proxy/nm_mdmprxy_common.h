/******************************************************************************

                   N M _ M D M P R X Y _ C O M M O N _ . H

******************************************************************************/

/******************************************************************************

  @file    nm_mdmprxy_common.h
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy plugin module is responsible for managing routing rules
  and packet marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <sys/socket.h>
#include <inttypes.h>

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_uninstall_jump_rules
===========================================================================*/
/*!
@brief
  In the main iptables chains, remove jump rule to custom chains

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_uninstall_jump_rules
(
  sa_family_t  af
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_install_jump_rules
===========================================================================*/
/*!
@brief
  In the main iptables chains, install jump rule to custom chains

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_install_jump_rules
(
  sa_family_t  af
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_clear_tcp6_mss
===========================================================================*/
/*!
@brief
  Remove iptables TCP V6 MSS clamping rule.

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
nm_mdmprxy_common_clear_tcp6_mss
(
  const char*     ifname,
  int             tech_type,
  struct in6_addr *addr6,
  uint32_t        mask6
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_set_tcp6_mss
===========================================================================*/
/*!
@brief
  Install iptables rules to set the TCP6 MSS to NM_MDMPRXY_TCP_MSS to avoid
  fragmentation of incoming ESP tunneled packets as such fragments might be
  discarded on some networks due to security concerns.

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
nm_mdmprxy_common_set_tcp6_mss
(
  const char*     ifname,
  int             tech_type,
  struct in6_addr *addr6,
  uint32_t        mask6
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_enable_forwarding
===========================================================================*/
/*!
@brief
  Enable IP forwarding by calling 'ndc ipfwd enable iwlan'. Netd will always
  enabled both IPv6 and IPv4 forwarding at the same time.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_enable_forwarding(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_disable_forwarding
===========================================================================*/
/*!
@brief
  Disable IP forwarding

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_disable_forwarding(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_oos_hdlr
===========================================================================*/
/*!
@brief
  Global OOS handler for common module

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_oos_hdlr(boolean force);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_is_hdlr
===========================================================================*/
/*!
@brief
  Global IS handler for common module

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_init(void* arg);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_install_dl_ping6_fwmark_rules
===========================================================================*/
/*!
@brief
  Installs policy routing rule to route packets with custom ICMPv6 echo
  request mark

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_common_install_dl_ping6_fwmark_rules
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_remove_dl_ping6_fwmark_rules
===========================================================================*/
/*!
@brief
  Removes policy routing rule to route packets with custom ICMPv6 echo
  request mark

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg tunnel     - Tunnel parameters (if any)

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_common_remove_dl_ping6_fwmark_rules
(
  const char          *link_name,
  int                 tech_type,
  ipsec_tunnel_info_t *tunnel
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_install_per_iface_dl_ping6_rules
===========================================================================*/
/*!
@brief
  Installs rules to mark incoming ICMPv6 echo requests with a custom mark and
  not with the WLAN interface fwmark

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg daddr6     - Destination Address
@arg mask6      - Prefix length

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_common_install_per_iface_dl_ping6_rules
(
  const char          *link_name,
  int                 tech_type,
  struct in6_addr     *daddr6,
  uint32_t            mask6
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_remove_per_iface_dl_ping6_rules
===========================================================================*/
/*!
@brief
  Removes previously installed custom marking rules for incoming ICMPv6 echo
  request messages

@arg link_name  - Interface name
@arg tech_type  - Local breakout or S2B
@arg daddr6     - Destination Address
@arg mask6      - Prefix length

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_common_remove_per_iface_dl_ping6_rules
(
  const char          *link_name,
  int                 tech_type,
  struct in6_addr     *daddr6,
  uint32_t            mask6
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_flush_ping6_rules
===========================================================================*/
/*!
@brief
  Flushes the DL ping6 rules when SSR is detected

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_common_flush_dl_ping6_rules(void);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_install_jump_rules_netd_restart
===========================================================================*/
/*!
@brief
  In the main iptables chains, install jump rule to custom chains
  on netd restart

@return
  NETMGR_SUCCESS on successful command execution
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_install_jump_rules_netd_restart
(
  sa_family_t  af
);

/*===========================================================================
  FUNCTION  nm_mdmprxy_common_enable_forwarding_netd_restart
===========================================================================*/
/*!
@brief
  Enable IP forwarding by calling 'ndc ipfwd enable iwlan'. Netd will always
  enabled both IPv6 and IPv4 forwarding at the same time.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
nm_mdmprxy_common_enable_forwarding_netd_restart(void);