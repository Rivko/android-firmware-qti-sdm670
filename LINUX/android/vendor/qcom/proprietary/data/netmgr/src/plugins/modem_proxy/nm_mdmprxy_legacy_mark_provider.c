/******************************************************************************

      N M _ M D M P R X Y _ L E G A C Y _ M A R K _ P R O V I D E R . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmproxy_legacy_mark_provider.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Responsible for installing packet marking rules required for forwarding
  packets to the modem

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <sys/socket.h>
#include <linux/udp.h>

#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_config.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_iptables_helper.h"
#include "nm_mdmprxy_mark_provider_factory.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef struct
{
  boolean is_v4_marking_rule_installed;
  boolean is_v6_marking_rule_installed;
  boolean is_v4_apps_marking_rule_installed;
  boolean is_v6_apps_marking_rule_installed;
} nm_mdmprxy_cfg_t;

static nm_mdmprxy_cfg_t global_config;

#define NM_MDMPRXY_GET_MARKING_RULES_INSTALLED(family)                 \
  ((AF_INET == family) ?                                               \
   global_config.is_v4_marking_rule_installed :                        \
   global_config.is_v6_marking_rule_installed)

#define NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(family, status)         \
  *(((AF_INET == family) ?                                             \
    &global_config.is_v4_marking_rule_installed :                      \
    &global_config.is_v6_marking_rule_installed)) = status

#define NM_MDMPRXY_GET_APPS_MARKING_RULES_INSTALLED(family)            \
  ((AF_INET == family) ?                                               \
   global_config.is_v4_apps_marking_rule_installed :                   \
   global_config.is_v6_apps_marking_rule_installed)

#define NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(family, status)    \
  *(((AF_INET == family) ?                                             \
    &global_config.is_v4_apps_marking_rule_installed :                 \
    &global_config.is_v6_apps_marking_rule_installed)) = status

static int
nm_mdmprxy_legacy_mark_prov_install_rules(sa_family_t);

static int
nm_mdmprxy_legacy_mark_prov_remove_rules(sa_family_t);

extern int
nm_mdmprxy_common_mark_prov_install_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_remove_spi_rule(sa_family_t,unsigned int);

extern int
nm_mdmprxy_common_mark_prov_install_portfwd_rule(sa_family_t, boolean);

extern int
nm_mdmprxy_common_mark_prov_remove_portfwd_rule(sa_family_t, boolean);

extern boolean
nm_mdmprxy_common_mark_prov_query_portfwd_status(sa_family_t);

extern int
nm_mdmprxy_common_mark_prov_install_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_remove_icmp_rules(sa_family_t, char*);

extern int
nm_mdmprxy_common_mark_prov_oos_cleanup(void);

static int
nm_mdmprxy_legacy_mark_prov_init(void);

static int
nm_mdmprxy_legacy_mark_prov_is_hdlr(void);

static int
nm_mdmprxy_legacy_mark_prov_oos_hdlr(void);

extern void
nm_mdmprxy_common_mark_prov_setup_iptables(struct mark_prov_iptables *);

nm_mdmprxy_mark_prov_t nm_mdmprxy_legacy_mark_provider =
{
  .install_rules        = nm_mdmprxy_legacy_mark_prov_install_rules,
  .remove_rules         = nm_mdmprxy_legacy_mark_prov_remove_rules,

  .install_spi_rule     = nm_mdmprxy_common_mark_prov_install_spi_rule,
  .remove_spi_rule      = nm_mdmprxy_common_mark_prov_remove_spi_rule,
  .install_portfwd_rule = nm_mdmprxy_common_mark_prov_install_portfwd_rule,
  .remove_portfwd_rule  = nm_mdmprxy_common_mark_prov_remove_portfwd_rule,
  .query_portfwd_status = nm_mdmprxy_common_mark_prov_query_portfwd_status,
  .install_icmp_rule    = nm_mdmprxy_common_mark_prov_install_icmp_rules,
  .remove_icmp_rule     = nm_mdmprxy_common_mark_prov_remove_icmp_rules,

  .init                 = nm_mdmprxy_legacy_mark_prov_init,
  .is_hdlr              = nm_mdmprxy_legacy_mark_prov_is_hdlr,
  .oos_hdlr             = nm_mdmprxy_legacy_mark_prov_oos_hdlr
};

static const struct nm_mdmprxy_cmd static_rule_list[] =
{
  /* Insert a jump rule to IMS audio port-forwarding based rules */
  /* TODO: Figure out how to handle dynamic addition and deletion of audio
           port-forwarding based rules */
  /* save any packet mark on to the connection (for handling fragmented IPv6 pkts) */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --save-mark",
    nm_mdmprxy_connmark_save_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_ex",
    nm_mdmprxy_basic_formatter },

  /* Mark IMS SIP packets arriving on port 5060 */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p udp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p udp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p tcp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p tcp"
      " --dport " strstr(NM_MDMPRXY_IMS_SIP_PORT) " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
    nm_mdmprxy_basic_formatter },

  /* Mark packets belonging to the modem range 32000 - 36999 */
  { "%s %s -t mangle -I nm_mdmprxy_mark_prov_chain -p udp"
        " --dport " strstr(NM_MDMPRXY_MODEM_PORT_START) ":" strstr(NM_MDMPRXY_MODEM_PORT_END) " "
        "-j MARK --set-mark 0x9",
    "%s %s -t mangle -D nm_mdmprxy_mark_prov_chain -p udp"
        " --dport " strstr(NM_MDMPRXY_MODEM_PORT_START) ":" strstr(NM_MDMPRXY_MODEM_PORT_END) " "
        "-j MARK --set-mark 0x9",
    nm_mdmprxy_basic_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_mark_prov_chain -p tcp"
        " --dport " strstr(NM_MDMPRXY_MODEM_PORT_START) ":" strstr(NM_MDMPRXY_MODEM_PORT_END) " "
        "-j MARK --set-mark 0x9",
    "%s %s -t mangle -D nm_mdmprxy_mark_prov_chain -p tcp"
        " --dport " strstr(NM_MDMPRXY_MODEM_PORT_START) ":" strstr(NM_MDMPRXY_MODEM_PORT_END) " "
        "-j MARK --set-mark 0x9",
    nm_mdmprxy_basic_formatter },

  /* Insert a jump rule to the SPI matching table from pkt_marker table */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mngl_pre_spi",
    nm_mdmprxy_basic_formatter },

  /* Jump to mark prov chain which hosts the modem port-range rules */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j nm_mdmprxy_mark_prov_chain",
    nm_mdmprxy_basic_formatter },

  /* Restore mark on icmp packets originating from modem using connection tracking
     framework to enable pings originiating from modem to be successful */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -j CONNMARK --restore-mark",
    nm_mdmprxy_basic_formatter }
};

/* Hosts all the application specific port-based packet marking rules
   Currently IMS application is the only one which has specific port requirements

   40100 - 40150 : Additional ports used for operator specific requirements
   50010 - 50060 : Audio ports

   nm_mdmprxy_mngl_pre_ex chain will be used to host all these rules */
static const struct nm_mdmprxy_cmd apps_ports_rule_list[] =
{
  {"%s %s -t mangle -A nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   "%s %s -t mangle -D nm_mdmprxy_mngl_pre_ex -p tcp"
       " --dport " strstr(NM_MDMPRXY_EX_IMS_PORT_START) ":" strstr(NM_MDMPRXY_EX_IMS_PORT_END)
       " -j MARK --set-mark " strstr(NM_MDMPRXY_MARK),
   nm_mdmprxy_basic_formatter }
};

/* All IKE in UDP packets destined to the Modem NATT port must be sent to the
   Modem and other IKE/ESP in UDP to the AP network stack for decryption and
   subsequent forwarding based on innner packet contents. This is how we can
   differentiate between the two: the first 4 bytes of the UDP payload of
   IKE in UDP is always 0 and non-zero for ESP in UDP packets (contains SPI) */

/* Start at the IP header, extract IHL and add the offset to get to the
   start of UDP header and then offset 8 (UDP header) bytes to get to the
   payload. If the 4 bytes are 0 then mark the packet so that it gets
   forwarded to the Modem */

/* Send the rest of the ESP in UDP packets originating from NATT server port
   up the networking stack and also to avoid further processing within the mangle table
   which can cause incorrect marking to happen. Currently ESP packets use port combination
   of 32012 and 4500 which will clash with the packet marking rules for modem range packets */
static const struct nm_mdmprxy_cmd udp_encap_rule_list[] =
{
  /* --sport 4500 */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p udp --sport %u"
                          " -m mark ! --mark %u -j ACCEPT",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p udp --sport %u"
                          " -m mark ! --mark %u -j ACCEPT",
    nm_mdmprxy_encaprulelocal_formatter },

  /* --sport 4500 --dport 32012 */
  { "%s %s -t mangle -I nm_mdmprxy_pkt_marker -p udp --sport %d --dport %d -m u32"
                    " --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -p udp --sport %d --dport %d -m u32"
                    " --u32 0>>22&0x3C@8=0 -j MARK --set-mark %u",
    nm_mdmprxy_encaprule_formatter },
};

static struct mark_prov_iptables legacy_mark_prov_cmd_tbl;
static int natt_fd = -1;

/*===========================================================================
                              LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_set_reserved_ports
===========================================================================*/
/*!
@brief
  Sets the 'ip_local_reserved_port_range' proc entry to block kernel from
  choosing an ephemeral port from the given range
*/
/*=========================================================================*/
static void
nm_mdmprxy_legacy_mark_set_reserved_ports(void)
{
  FILE* fp = NULL;
  const char* port_range =
      strstr(NM_MDMPRXY_MODEM_PORT_START) "-" strstr(NM_MDMPRXY_MODEM_PORT_END) ","
      strstr(NM_MDMPRXY_IMS_SIP_PORT) ","
      strstr(NM_MDMPRXY_EX_IMS_PORT_START) "-" strstr(NM_MDMPRXY_EX_IMS_PORT_END) ","
      strstr(NM_MDMPRXY_IMS_AUDIO_PORT_START) "-" strstr(NM_MDMPRXY_IMS_AUDIO_PORT_END);

  /* Check if we can access the reserved_port_bind */
  if (access(FILEPATH_RESERVED_PORT_BIND, F_OK))
  {
    /* We are unable to access the file */
    netmgr_log_err("%s(): unable to access ip_local_reserved_ports /proc entry [%s]",
                   __func__, strerror(errno));
    return;
  }

  fp = fopen(FILEPATH_IP_LOCAL_RESERVED_PORTS, "we");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open ip_local_reserved_ports /proc file [%s]",
                   __func__, strerror(errno));
    return;
  }

  fprintf(fp, "%s", port_range);
  fclose(fp);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_remove_rules
===========================================================================*/
/*!
@brief
  Remove iptables marking rules

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
static int
nm_mdmprxy_legacy_mark_prov_remove_rules
(
  sa_family_t af
)
{
  struct nm_mdmprxy_iiface_rule_info nfo;
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  if (FALSE == NM_MDMPRXY_GET_MARKING_RULES_INSTALLED(af))
  {
    netmgr_log_med("%s(): marking rules already removed for ip_family [%d]", __func__, af);
    rc = NETMGR_SUCCESS;
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(static_rule_list);

  memset(&nfo, 0, sizeof(nfo));

  for (i = 0; i < num_rules; i++)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, &nfo);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  /* Uninstall udpencap rules (currently only for V4) */
  if (AF_INET == af)
  {
    rc = nm_mdmprxy_uninstall_udpencap_rules(NM_MDMPRXY_MODEM_NATT_PORT,  /* dport */
                                             NM_MDMPRXY_IKE_REMOTE_PORT,  /* sport */
                                             NM_MDMPRXY_MARK);            /* mark */
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to remove UDP encap rules", __func__);
      goto bail;
    }
  }

  rc = NETMGR_SUCCESS;

  netmgr_log_low("%s(): Successfully removed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, FALSE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_install_rules
===========================================================================*/
/*!
@brief
  Install iptables marking rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_install_rules
(
  sa_family_t af
)
{
  struct nm_mdmprxy_iiface_rule_info nfo;
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;


  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  if (TRUE == NM_MDMPRXY_GET_MARKING_RULES_INSTALLED(af))
  {
    netmgr_log_med("%s(): marking rules already installed for ip_family [%d]", __func__, af);
    rc = NETMGR_SUCCESS;
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(static_rule_list);

  memset(&nfo, 0, sizeof(nfo));

  for (i = 0; i < num_rules; i++)
  {
    if (!static_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&static_rule_list[i], NM_MDMPRXY_RUN_RULE, af, &nfo);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  /* Install udpencap rules (currently only for V4) */
  if (AF_INET == af)
  {
    rc = nm_mdmprxy_install_udpencap_rules(NM_MDMPRXY_MODEM_NATT_PORT,  /* dport */
                                           NM_MDMPRXY_IKE_REMOTE_PORT,  /* sport */
                                           NM_MDMPRXY_MARK);            /* mark */
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to install UDP encap rules", __func__);
      goto bail;
    }
  }


  netmgr_log_low("%s(): Successfully installed marking rules for ip_family [%d]", __func__, af);
  NM_MDMPRXY_SET_MARKING_RULES_INSTALLED(af, TRUE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_uninstall_apps_rules
===========================================================================*/
/*!
@brief
  Remove application specific marking rules

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
static int
nm_mdmprxy_legacy_mark_prov_uninstall_apps_rules
(
  sa_family_t af,
  int start
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (DFS_REMOTE_SOCKET_HANDLING_V1_V01 == nm_mdmprxy_get_dfs_service_version())
  {
    netmgr_log_low("%s(): application specific port-based rules are not installed"
                   " in this configuration", __func__);
    return NETMGR_SUCCESS;
  }

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  for (i = start; i >= 0; i--)
  {
    if (!apps_ports_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&apps_ports_rule_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully removed application specific marking rules for ip_family [%d]",
                 __func__, af);
  NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(af, FALSE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_install_apps_rules
===========================================================================*/
/*!
@brief
  Install application specific iptables packet marking rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_install_apps_rules
(
  sa_family_t af
)
{
  int num_rules = 0, i = 0;
  int rc = NETMGR_FAILURE;

  if (DFS_REMOTE_SOCKET_HANDLING_V1_V01 == nm_mdmprxy_get_dfs_service_version())
  {
    netmgr_log_low("%s(): application specific port-based rules are not installed"
                   " in this configuration", __func__);
    return NETMGR_SUCCESS;
  }

  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown IP family!", __func__);
    goto bail;
  }

  num_rules = nm_mdmprxy_rules_len(apps_ports_rule_list);

  for (i = 0; i < num_rules; i++)
  {
    if (!apps_ports_rule_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&apps_ports_rule_list[i], NM_MDMPRXY_RUN_RULE, af, 0);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): Failed to apply rule [%d]", __func__, i);
      nm_mdmprxy_legacy_mark_prov_uninstall_apps_rules(af, i);
      goto bail;
    }
  }

  netmgr_log_low("%s(): Successfully installed apps specific marking rules for ip_family [%d]",
                 __func__, af);
  NM_MDMPRXY_SET_APPS_MARKING_RULES_INSTALLED(af, TRUE);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_oos_hdlr
===========================================================================*/
/*!
@brief
  Handler for modem OOS event

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_oos_hdlr(void)
{
  int rc = NETMGR_FAILURE;

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_uninstall_apps_rules(AF_INET,
                                                                         nm_mdmprxy_rules_len(apps_ports_rule_list) - 1))
  {
    netmgr_log_err("%s(): failed to uninstall application specific marking rules for V4", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_uninstall_apps_rules(AF_INET6,
                                                                         nm_mdmprxy_rules_len(apps_ports_rule_list) - 1))
  {
    netmgr_log_err("%s(): failed to uninstall application specific marking rules for V6", __func__);
    goto bail;
  }

  /* Remove common rules */
  if (NETMGR_SUCCESS != nm_mdmprxy_common_mark_prov_oos_cleanup())
  {
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_setup_natt_port
===========================================================================*/
/*!
@brief
  Initializes the UDP port to be used for UDP encapsulation to support
  NAT-Traversal

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_setup_natt_port(void)
{
  int rc = NETMGR_FAILURE;

  /* For NATT to work, we need to have at least one UDP socket open on the
     Apps side which is bound to the NATT Modem port along with the ESP in UDP
     socket option set. */
  /* Set the UDP_ENCAP socket option */
  if (natt_fd < 0)
  {
    int type = UDP_ENCAP_ESPINUDP;
    struct sockaddr_in saddr;
    struct sockaddr_in* __attribute__((__may_alias__)) saddr_ptr;

    netmgr_log_med("%s(): setting ESPINUDP sockopt",__func__);

    /* Open a datagram socket to use for issuing the ioctl */
    if ((natt_fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
      netmgr_log_sys_err("netmgr_kif_setup_natt_port: socket() failed:\n");
      goto bail;
    }

    if (setsockopt(natt_fd, SOL_UDP, UDP_ENCAP, &type, sizeof(type)) < 0)
    {
      netmgr_log_sys_err("netmgr_kif_setup_natt_port: failed to set ESPINUDP option:\n");
      goto bail;
    }

    memset((char *) &saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = (unsigned short)htons(NM_MDMPRXY_MODEM_NATT_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr_ptr = &saddr;

    if (bind(natt_fd, (struct sockaddr*)saddr_ptr, sizeof(saddr)) < 0)
    {
      netmgr_log_sys_err("netmgr_kif_setup_natt_port: failed to bind to port\n");
      goto bail;
    }

    rc = NETMGR_SUCCESS;
  }
  else
  {
    netmgr_log_med("%s(): natt port already initialized\n",__func__);
    rc = NETMGR_SUCCESS;
  }

bail:
  if (NETMGR_SUCCESS != rc)
  {
    if (natt_fd >= 0)
      close(natt_fd);
    natt_fd = -1;
  }

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_is_hdlr
===========================================================================*/
/*!
@brief
  Handler for modem IS event

@return
  NETMGR_SUCCESS/NETMGR_FAILURE

@note

  - Dependencies
    - Static iptables module has been initialized
    - Routing provider module has been initialized
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_is_hdlr(void)
{
  int ret = NETMGR_SUCCESS;

  /* Install IMS audio port-forwarding rules, set the preference to TRUE indicating that we
     want to install the rules since this is an SSR in-service scenario */
  if (NETMGR_SUCCESS != nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET, TRUE))
  {
    netmgr_log_err("%s(): Failed to install audio port-forwarding rules for ip_family [%d]",
                   __func__, AF_INET);
    ret = NETMGR_FAILURE;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_common_mark_prov_install_portfwd_rule(AF_INET6, TRUE))
  {
    netmgr_log_err("%s(): Failed to install audio port-forwarding rules for ip_family [%d]",
                   __func__, AF_INET6);
    ret = NETMGR_FAILURE;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_legacy_mark_prov_init
===========================================================================*/
/*!
@brief
  Do legacy mark provider init

@return
  NETMGR_SUCCESS/NETMGR_FAILURE

@note

  - Dependencies
    - Static iptables module has been initialized
    - Routing provider module has been initialized
*/
/*=========================================================================*/
static int
nm_mdmprxy_legacy_mark_prov_init(void)
{
  int rc = NETMGR_FAILURE;

  memset(&legacy_mark_prov_cmd_tbl, 0, sizeof(legacy_mark_prov_cmd_tbl));

  legacy_mark_prov_cmd_tbl.udp_encap_rule_list       = udp_encap_rule_list;
  legacy_mark_prov_cmd_tbl.udp_encap_rule_list_len   = (int) nm_mdmprxy_rules_len(udp_encap_rule_list);

  nm_mdmprxy_common_mark_prov_setup_iptables(&legacy_mark_prov_cmd_tbl);
  (void) nm_mdmprxy_register_mark_prov_cmd_tbl(&legacy_mark_prov_cmd_tbl);

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_install_rules(AF_INET))
  {
    netmgr_log_err("%s(): failed to install marking rules for V4", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_install_rules(AF_INET6))
  {
    netmgr_log_err("%s(): failed to install marking rules for V4", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_install_apps_rules(AF_INET))
  {
    netmgr_log_err("%s(): failed to install application specific marking rules for V4", __func__);
    goto bail;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_install_apps_rules(AF_INET6))
  {
    netmgr_log_err("%s(): failed to install application specific marking rules for V6", __func__);
    goto bail;
  }

  /* Setup NAT-T port */
  if (NETMGR_SUCCESS != nm_mdmprxy_legacy_mark_prov_setup_natt_port())
  {
    netmgr_log_err("%s(): failed to setup NATT port", __func__);
    goto bail;
  }

  netmgr_main_cfg_set_ike_port(NM_MDMPRXY_MODEM_NATT_PORT);
  nm_mdmprxy_legacy_mark_set_reserved_ports();

  rc = NETMGR_SUCCESS;

bail:
  return rc;
}
