/******************************************************************************

             N M _ M D M P R X Y _ I P T A B L E S . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmprxy_iptables.c
  @brief   Port Resevation plugin

  DESCRIPTION
  Allow the modem to reserve ports on the AP by opening remote sockets

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "ds_util.h"
#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_kif.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_iptables_helper.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static struct route_prov_iptables *route_prov_cmd_tbl;
static struct mark_prov_iptables  *mark_prov_cmd_tbl;

static struct nm_mdmprxy_iiface_rule_info *installed_rules[NM_MDMPRXY_MAXRULES];

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

char iptables_options[IPTABLES_OPTS_MAX_LENGTH];

/*===========================================================================
                     LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  _nm_mdmprxy_uninstall_fwd_rules
===========================================================================*/
/*!
@brief
  Install oiface iptables rules

@arg handle - iiface handle the oiface is bound to
@arg af - AF_INET/AF_INET6
@arg slot - oiface slot number

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_uninstall_fwd_rules
(
  int handle,
  sa_family_t af,
  int slot,
  int start
)
{
  struct nm_mdmprxy_iiface_rule_info *rule;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iface_forward_rule_list);

  if (start >= (int)route_prov_cmd_tbl->per_iface_forward_rule_list_len)
  {
    return NETMGR_FAILURE;
  }

  rule = installed_rules[handle];
  rc = NETMGR_SUCCESS;
  for (i = start; i >= 0; i--)
  {
    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iface_forward_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, af, &(rule->outrules[slot]));
  }

  return rc;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_uninstall_destination_rules
===========================================================================*/
/*!
@brief
  Remove oiface iptables rules

@arg handle - iiface handle the oiface is bound to
@arg af - AF_INET/AF_INET6
@arg slot - oiface slot number
@arg start - rule number to start deleting from

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_uninstall_destination_rules
(
  int handle,
  sa_family_t af,
  int slot,
  int start
)
{
  struct nm_mdmprxy_iiface_rule_info *rule;
  int i, rc = NETMGR_SUCCESS;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_oiface_rule_list);

  if (start >= (int)route_prov_cmd_tbl->per_oiface_rule_list_len)
  {
    return NETMGR_FAILURE;
  }

  rule = installed_rules[handle];
  rc = NETMGR_SUCCESS;
  for (i = start; i >= 0; i--)
  {
    if (!route_prov_cmd_tbl->per_oiface_rule_list[i].rule)
      continue;

    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_oiface_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, af, &(rule->outrules[slot]));
  }

  return rc;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_install_destination_rules
===========================================================================*/
/*!
@brief
  Install oiface iptables rules

@arg handle - iiface handle the oiface is bound to
@arg af - AF_INET/AF_INET6
@arg slot - oiface slot number

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_install_destination_rules
(
  int handle,
  sa_family_t af,
  int slot
)
{
  struct nm_mdmprxy_iiface_rule_info *rule;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_oiface_rule_list);

  rule = installed_rules[handle];
  for (i = 0; i < route_prov_cmd_tbl->per_oiface_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->per_oiface_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_oiface_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, af, &rule->outrules[slot]);
    if (rc != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  _nm_mdmprxy_uninstall_destination_rules(handle, af, slot, i);
  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_install_fwd_rules
===========================================================================*/
/*!
@brief
  Install oiface iptables rules

@arg handle - iiface handle the oiface is bound to
@arg af - AF_INET/AF_INET6
@arg slot - oiface slot number

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_install_fwd_rules
(
  int handle,
  sa_family_t af,
  int slot
)
{
  struct nm_mdmprxy_iiface_rule_info *rule;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iface_forward_rule_list);

  rule = installed_rules[handle];
  for (i = 0; i < route_prov_cmd_tbl->per_iface_forward_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->per_iface_forward_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iface_forward_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, af, &rule->outrules[slot]);
    if (rc != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  _nm_mdmprxy_uninstall_fwd_rules(handle, af, slot, i);
  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_uninstall_iiface_rules
===========================================================================*/
/*!
@brief
  Uninstall iiface iptables rules

@arg handle - iiface handle
@arg af - AF_INET/AF_INET6
@arg start - rule number to start deleting from

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_uninstall_iiface_rules
(
  int handle,
  sa_family_t af,
  int start
)
{
  int i, rc = NETMGR_SUCCESS;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iiface_rule_list);

  if (start >= route_prov_cmd_tbl->per_iiface_rule_list_len)
  {
    rc = NETMGR_FAILURE;
    goto error;
  }

  rc = NETMGR_SUCCESS;
  for (i = start; i >= 0; i--)
  {
    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iiface_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE,
                                  af, installed_rules[handle]);
  }

error:
  return rc;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_install_iiface_rules
===========================================================================*/
/*!
@brief
  Install iiface iptables rules

@arg handle - iiface handle
@arg af - AF_INET/AF_INET6

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_install_iiface_rules
(
  int handle,
  sa_family_t af
)
{
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iiface_rule_list);

  for (i = 0; i < route_prov_cmd_tbl->per_iiface_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->per_iiface_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iiface_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, af, installed_rules[handle]);
    if (rc != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  _nm_mdmprxy_uninstall_iiface_rules(handle, af, i);
  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_uninstall_static_rules
===========================================================================*/
/*!
@brief
  Uninstall static powerup rules

@arg af - AF_INET/AF_INET6
@arg mark - mark number to use once packets have been identified
@arg start - rule number to start deleting from

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
nm_mdmprxy_uninstall_static_rules
(
  sa_family_t af,
  int mark,
  int start
)
{
  struct nm_mdmprxy_iiface_rule_info nfo;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->static_rule_list);

  if (start >= route_prov_cmd_tbl->static_rule_list_len)
  {
    return NETMGR_FAILURE;
  }

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;

  rc = NETMGR_SUCCESS;
  for (i = start; i >= 0; i--)
  {
    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->static_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, af, &nfo);
  }

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_static_rules
===========================================================================*/
/*!
@brief
  Install static powerup rules

@arg af - AF_INET/AF_INET6
@arg mark - mark number to use once packets have been identified

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
nm_mdmprxy_install_static_rules
(
  sa_family_t af,
  int mark
)
{
  struct nm_mdmprxy_iiface_rule_info nfo;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->static_rule_list);

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;
  for (i = 0; i < route_prov_cmd_tbl->static_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->static_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->static_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, af, &nfo);
    if (rc != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_uninstall_static_rules(af, mark, i);
  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_uninstall_udpencap_rules
===========================================================================*/
/*!
@brief
  Uninstall UDP Encapsulation / IKE filter rules

@arg af - AF_INET/AF_INET6
@arg dport - local port
@arg sport - remote port
@arg mark - mark number to use once packets have been identified
@arg start - rule number to start deleting from

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_uninstall_udpencap_rules
(
  sa_family_t af,
  int dport,
  int sport,
  int mark,
  int start
)
{
  struct encap_rule_info nfo;
  int i, rc = NETMGR_SUCCESS;

  NM_MDMPRXY_NULL_CHECK(mark_prov_cmd_tbl->udp_encap_rule_list);

  if (start >= mark_prov_cmd_tbl->udp_encap_rule_list_len)
  {
    return NETMGR_FAILURE;
  }

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;
  nfo.sport = sport;
  nfo.dport = dport;

  rc = NETMGR_SUCCESS;
  for (i = start; i >= 0; i--)
  {
    rc |= nm_mdmprxy_process_rule(&mark_prov_cmd_tbl->udp_encap_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, af, &nfo);
  }

  return rc;
}

/*===========================================================================
  FUNCTION  _nm_mdmprxy_install_udpencap_rules
===========================================================================*/
/*!
@brief
  Uninstall UDP Encapsulation / IKE filter rules

@arg af - AF_INET/AF_INET6
@arg dport - local port
@arg sport - remote port
@arg mark - mark number to use once packets have been identified

@return
  NETMGR_FAILURE / NETMGR_SUCCESS

*/
/*=========================================================================*/
static int
_nm_mdmprxy_install_udpencap_rules
(
  sa_family_t af,
  int dport,
  int sport,
  int mark
)
{
  struct encap_rule_info nfo;
  int i, rc;

  NM_MDMPRXY_NULL_CHECK(mark_prov_cmd_tbl->udp_encap_rule_list);

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;
  nfo.sport = sport;
  nfo.dport = dport;

  for (i = 0; i < mark_prov_cmd_tbl->udp_encap_rule_list_len; i++)
  {
    if (!mark_prov_cmd_tbl->udp_encap_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&mark_prov_cmd_tbl->udp_encap_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, af, &nfo);
    if (rc != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  _nm_mdmprxy_uninstall_udpencap_rules(af, dport, sport, mark, i);
  return NETMGR_FAILURE;
}

/*===========================================================================
                             PUBLIC FUNCTIONS
===========================================================================*/

/*--------------------------------------------------------------------------
                        STRING FORMATTING FUNCTIONS
---------------------------------------------------------------------------*/

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
)
{
  const struct encap_rule_info *nfo = (const struct encap_rule_info*)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->sport, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->sport, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct encap_rule_info *nfo = (const struct encap_rule_info*)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options,
               nfo->sport, nfo->dport, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options,
               nfo->sport, nfo->dport, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct encap_rule_info *nfo = (const struct encap_rule_info*)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->dport, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->dport, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}


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
)
{
  (void) info;

  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}
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
)
{
  (void) info;

  switch(af)
  {
    case AF_INET:
     return NM_MDMPRXY_SKIP;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}


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
int
nm_mdmprxy_markjump_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
)
{
  const struct nm_mdmprxy_iiface_rule_info *nfo = (const struct nm_mdmprxy_iiface_rule_info *)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
int
nm_mdmprxy_ifacejump_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
)
{
  const struct nm_mdmprxy_iiface_rule_info *nfo = (const struct nm_mdmprxy_iiface_rule_info *)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->iif);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->iif);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
int
nm_mdmprxy_tee_formatter
(
  char *buff,
  int bufflen,
  sa_family_t af,
  const char* formatstr,
  const void *info
)
{
  const struct nm_mdmprxy_oiface_rule_info *nfo = (const struct nm_mdmprxy_oiface_rule_info *)info;
  char daddr[INET6_ADDRSTRLEN*2];
  char gw[INET6_ADDRSTRLEN];
  char prefix[8];

  switch(af)
  {
    case AF_INET:
      if (!nfo->valid4)
        return NM_MDMPRXY_SKIP;
      inet_ntop(af, &(nfo->gw4), gw, INET6_ADDRSTRLEN);
      inet_ntop(af, &(nfo->daddr4), daddr, INET6_ADDRSTRLEN);
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, daddr, gw, nfo->oif);
      break;

    case AF_INET6:
      if (!nfo->valid6)
        return NM_MDMPRXY_SKIP;
      inet_ntop(af, &(nfo->gw6), gw, INET6_ADDRSTRLEN);
      inet_ntop(af, &(nfo->daddr6), daddr, INET6_ADDRSTRLEN);
      if (nfo->mask6)
      {
        snprintf(prefix,8,"/%u", nfo->mask6);
        strlcat(daddr, prefix, INET6_ADDRSTRLEN*2);
      }
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, daddr, gw, nfo->oif);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct nm_mdmprxy_oiface_rule_info *nfo = (const struct nm_mdmprxy_oiface_rule_info *) info;
  char daddr[INET6_ADDRSTRLEN*2];
  char prefix[8];

  if (!nfo)
  {
    netmgr_log_err("%s(): invalid input!", __func__);
    return NM_MDMPRXY_ERR;
  }

  if (!buff)
  {
    netmgr_log_err("%s(): buff not provided, invalid input!", __func__);
    return NM_MDMPRXY_ERR;
  }

  if (!formatstr)
  {
    netmgr_log_err("%s(): formatstr not provided, invalid input!", __func__);
    return NM_MDMPRXY_ERR;
  }

  switch (af)
  {
  case AF_INET:
    if (!nfo->valid4)
    {
      return NM_MDMPRXY_SKIP;
    }
    inet_ntop(af, &(nfo->daddr4), daddr, INET6_ADDRSTRLEN);
    snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, daddr);
    break;

  case AF_INET6:
    if (!nfo->valid6)
    {
      return NM_MDMPRXY_SKIP;
    }
    inet_ntop(af, &(nfo->daddr6), daddr, INET6_ADDRSTRLEN);
    if (nfo->mask6)
    {
      snprintf(prefix,8,"/%u", nfo->mask6);
      strlcat(daddr, prefix, INET6_ADDRSTRLEN*2);
    }
    snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, daddr);

    break;

  default:
    netmgr_log_err("%s(): Bad AF", __func__);
    return NM_MDMPRXY_ERR;
  }

  return NM_MDMPRXY_OK;
}

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
)
{
  const struct nm_mdmprxy_oiface_rule_info *nfo = (const struct nm_mdmprxy_oiface_rule_info *)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, NM_MDMPRXY_IIFACE, nfo->oif);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, NM_MDMPRXY_IIFACE, nfo->oif);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct nm_mdmprxy_oiface_rule_info *nfo = (const struct nm_mdmprxy_oiface_rule_info *)info;
  switch(af)
  {
  case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->oif, NM_MDMPRXY_IIFACE);
      break;

  case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->oif, NM_MDMPRXY_IIFACE);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct spi_rule_info *nfo = (const struct spi_rule_info *)info;
  switch (af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->spi, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->spi, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }

  return NM_MDMPRXY_OK;
}

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
)
{
  const struct port_forwarding *nfo = (const struct port_forwarding *)info;
  switch (af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options,
               nfo->start_port, nfo->end_port, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options,
               nfo->start_port, nfo->end_port, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }

  return NM_MDMPRXY_OK;
}

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
)
{
  const struct conntrack_rule_info *nfo = (const struct conntrack_rule_info*)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options,
               nfo->proto, nfo->iif, nfo->mark);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options,
               nfo->proto, nfo->iif, nfo->mark);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct lbo_rule_info *nfo = (const struct lbo_rule_info*)info;
  char gw[INET6_ADDRSTRLEN];

  switch(af)
  {
    case AF_INET:
      inet_ntop(af, &(nfo->gw4), gw, sizeof(gw));
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, gw, nfo->oif);
      break;

    case AF_INET6:
      inet_ntop(af, &(nfo->gw6), gw, sizeof(gw));
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, gw, nfo->oif);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct lbo_rule_info *nfo = (const struct lbo_rule_info *)info;
  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->iif);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->iif);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}
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
)
{
  NM_MDMPRXY_VAR_UNUSED(info);

  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, NM_MDMPRXY_LBO_MARK);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, NM_MDMPRXY_LBO_MARK);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct doze_rule_per_iface_info *nfo = (const struct doze_rule_per_iface_info *)info;

  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options, nfo->link_name);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, nfo->link_name);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct doze_rule_tunnel_info *nfo = (const struct doze_rule_tunnel_info *)info;
  int tunnel_af = nfo->tunnel.tunnel_family;

  (void) af;

  switch(tunnel_af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options,
               nfo->tunnel.local_addr, nfo->tunnel.dest_addr);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options,
               nfo->tunnel.local_addr, nfo->tunnel.dest_addr);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct doze_rule_tunnel_info *nfo = (const struct doze_rule_tunnel_info *)info;
  int tunnel_af = nfo->tunnel.tunnel_family;

  (void) af;

  switch(tunnel_af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPTABLES, iptables_options,
               nfo->tunnel.dest_addr, nfo->tunnel.local_addr);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options,
               nfo->tunnel.dest_addr, nfo->tunnel.local_addr);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const vpn_skip_tunnel_info *nfo = (const vpn_skip_tunnel_info *)info;
  int tunnel_af = nfo->tunnel.tunnel_family;

  (void) af;

  switch(tunnel_af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPRULE, nfo->tunnel.local_addr, nfo->tunnel.dest_addr);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6RULE, nfo->tunnel.local_addr, nfo->tunnel.dest_addr);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  (void) info;

  switch(af)
  {
    case AF_INET:
      snprintf(buff, bufflen, formatstr, IPRULE);
      break;

    case AF_INET6:
      snprintf(buff, bufflen, formatstr, IP6RULE);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_dl_ping6_formatter
===========================================================================*/
/*!
@brief
  Formatter for DL ping6 echo request messages

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
)
{

  const struct ping6_rule_info *nfo = (const struct ping6_rule_info *)info;
  char daddr[INET6_ADDRSTRLEN*2];
  char prefix[8];

  switch(af)
  {
    case AF_INET:
      break;

    case AF_INET6:
      inet_ntop(af, &(nfo->daddr6), daddr, INET6_ADDRSTRLEN);
      if (nfo->mask6)
      {
        snprintf(prefix, 8, "/%u", nfo->mask6);
        strlcat(daddr, prefix, INET6_ADDRSTRLEN*2);
      }
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, daddr);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

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
)
{
  const struct tcp_mss_rule_info *nfo = (const struct tcp_mss_rule_info *)info;
  char saddr[INET6_ADDRSTRLEN*2];
  char prefix[8];

  switch(af)
  {
    case AF_INET:
      break;

    case AF_INET6:
      inet_ntop(af, &(nfo->saddr6), saddr, INET6_ADDRSTRLEN);
      if (nfo->mask6)
      {
        snprintf(prefix, 8, "/%u", nfo->mask6);
        strlcat(saddr, prefix, INET6_ADDRSTRLEN*2);
      }
      snprintf(buff, bufflen, formatstr, IP6TABLES, iptables_options, saddr, nfo->mss);
      break;

    default:
      netmgr_log_err("%s(): Bad AF", __func__);
      return NM_MDMPRXY_ERR;
  }
  return NM_MDMPRXY_OK;
}

/*---------------------------------------------------------------------------
                     PUBLIC IPTABLES HELPER FUNCTIONS
---------------------------------------------------------------------------*/

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
)
{
  char buf[NM_MDMPRXY_MAXCMDSTR];
  char result[NM_MDMPRXY_MAXCMDSTR] = "";
  const char *rule;
  int rc;

#ifdef NETMGR_OFFTARGET
  netmgr_log_low("%s(): iptables rule installation currently stubbed out till support"
                 " is added", __func__);
  return NETMGR_SUCCESS;
#endif /* NETMGR_OFFTARGET */

  DS_SYSCALL_DEFAULTS(opts);
  opts.stderr = result;
  opts.stderr_length = sizeof(result);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  memset(buf, 0, sizeof(buf));
  memset(result, 0, sizeof(result));

  if (use_antirule)
    rule = cmd->antirule;
  else
    rule = cmd->rule;

  if(cmd->formatter)
  {
    rc = cmd->formatter(buf, NM_MDMPRXY_MAXCMDSTR, af, rule, nfo);
    if (rc == NM_MDMPRXY_SKIP)
    {
      return NETMGR_SUCCESS;
    }
    else if (rc != NM_MDMPRXY_OK)
    {
      netmgr_log_err("%s(): Failed to format rule: %s", __func__, rule);
      return NETMGR_FAILURE;
    }
  }
  else
  {
    strlcpy(buf, rule, NM_MDMPRXY_MAXCMDSTR);
  }

  rc = netmgr_ds_exec(buf, &opts);

  if (DS_EXEC_OK != rc)
  {
    netmgr_log_err("%s(): command failed with err: %s", __func__, result);
    rc = NETMGR_FAILURE;
  }

  return rc;
}

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
int
nm_mdmprxy_install_iiface_rules
(
  const char *ifname,
  uint32_t mark
)
{
  struct nm_mdmprxy_iiface_rule_info *ruleinfo;
  int i, handle, rc;
  size_t cpyres;

  NM_MDMPRXY_NULL_CHECK(ifname);
  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iiface_rule_list);

  handle = -1;
  for (i = 0; i < NM_MDMPRXY_MAXRULES; i++)
  {
    if (!installed_rules[i])
    {
      handle = i;
      break;
    }
  }
  if (handle == -1)
  {
    return NETMGR_FAILURE;
  }

  installed_rules[handle] = (struct nm_mdmprxy_iiface_rule_info *)
                            malloc(sizeof(struct nm_mdmprxy_iiface_rule_info));

  if (!installed_rules[handle])
  {
    return NETMGR_FAILURE;
  }

  memset(installed_rules[handle], 0, sizeof(struct nm_mdmprxy_iiface_rule_info));

  installed_rules[handle]->handle = handle;
  installed_rules[handle]->mark = mark;
  cpyres = strlcpy(installed_rules[handle]->iif, ifname, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  /* TODO: We need to extend this interface to take in IP family as an
     argument to have better control over installation of iptables rules
     per family */
  rc = _nm_mdmprxy_install_iiface_rules(handle, AF_INET);
  if (rc != NETMGR_SUCCESS)
    goto error;

  rc = _nm_mdmprxy_install_iiface_rules(handle, AF_INET6);
  if (rc != NETMGR_SUCCESS)
  {
    _nm_mdmprxy_uninstall_iiface_rules(handle,
                                       AF_INET,
                                       route_prov_cmd_tbl->per_iiface_rule_list_len - 1);
    goto error;
  }

  return handle;

error:
  free(installed_rules[handle]);
  installed_rules[handle] = NULL;
  return NETMGR_FAILURE;
}

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
int
nm_mdmprxy_uninstall_oiface_rules
(
  int handle,
  int slot,
  int ip_family
)
{
  struct nm_mdmprxy_oiface_rule_info *orule;
  int rc = 0;

  if (handle < 0 || handle >= NM_MDMPRXY_MAXRULES)
  {
    netmgr_log_err("%s(): Bad handle \"%d\"", __func__, handle);
    return NETMGR_FAILURE;
  }

  if (slot < 0 || slot >= NM_MDMPRXY_MAXOUTRULES)
  {
    netmgr_log_err("%s(): Bad destination slot \"%d\"", __func__, slot);
    return NETMGR_FAILURE;
  }

  if (!installed_rules[handle])
  {
    netmgr_log_err("%s(): Nothing on this handle \"%d\"", __func__, handle);
    return NETMGR_FAILURE;
  }

  orule = &(installed_rules[handle]->outrules[slot]);

  if (AF_INET == ip_family)
  {
    if (!orule->valid4)
    {
      netmgr_log_low("%s(): Nothing to do for v4 here, slot is empty. handle:%d slot:%d",
                     __func__, handle, slot);
    }
    else
    {
      /* Uninstall the rule to forward packets between rev rmnet and wlan0 */
      rc = _nm_mdmprxy_uninstall_fwd_rules(handle,
                                           AF_INET,
                                           slot,
                                           route_prov_cmd_tbl->per_iface_forward_rule_list_len - 1);

      rc |= _nm_mdmprxy_uninstall_destination_rules(handle,
                                                    AF_INET,
                                                    slot,
                                                    route_prov_cmd_tbl->per_oiface_rule_list_len - 1);
      orule->valid4 = 0;
    }
  }
  else if (AF_INET6 == ip_family)
  {
    if (!orule->valid6)
    {
      netmgr_log_low("%s(): Nothing to do for v6 here, slot is empty. handle:%d slot:%d",
                     __func__, handle, slot);
    }
    else
    {
      /* Uninstall the rule to forward packets between rev rmnet and wlano */
      rc = _nm_mdmprxy_uninstall_fwd_rules(handle,
                                           AF_INET6,
                                           slot,
                                           route_prov_cmd_tbl->per_iface_forward_rule_list_len - 1);

      rc |= _nm_mdmprxy_uninstall_destination_rules(handle,
                                                    AF_INET6,
                                                    slot,
                                                    route_prov_cmd_tbl->per_oiface_rule_list_len - 1);
      orule->valid6 = 0;
    }
  }
  else
  {
    netmgr_log_err("%s(): unknown ip family [%d]", __func__, ip_family);
  }

  if (!orule->valid4 && !orule->valid6)
  {
    /* Clear the oiface rule struct only when both address family rules
       are cleaned up properly */
    memset(orule, 0x0, sizeof( struct nm_mdmprxy_oiface_rule_info ) );
  }
  return rc;
}

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
int
nm_mdmprxy_uninstall_iiface_rules
(
  int handle
)
{
  int i;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iiface_rule_list);

  if (handle < 0 || handle >= NM_MDMPRXY_MAXRULES)
  {
    netmgr_log_err("%s(): Bad handle \"%d\"", __func__, handle);
    return NETMGR_FAILURE;
  }

  if (!installed_rules[handle])
  {
    return NETMGR_SUCCESS;
  }

  for (i = 0; i < NM_MDMPRXY_MAXDEVSTR; i++)
  {
    nm_mdmprxy_uninstall_oiface_rules(handle, i, AF_INET);
    nm_mdmprxy_uninstall_oiface_rules(handle, i, AF_INET6);
  }

  _nm_mdmprxy_uninstall_iiface_rules(handle,
                                     AF_INET,
                                     route_prov_cmd_tbl->per_iiface_rule_list_len - 1);
  _nm_mdmprxy_uninstall_iiface_rules(handle,
                                     AF_INET6,
                                     route_prov_cmd_tbl->per_iiface_rule_list_len - 1);
  /* Partial retry on this handle can result in bad things. Reset the handle
   * even if the uninstall fails */
  installed_rules[handle] = NULL;
  return NETMGR_SUCCESS;
}

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
)
{
  struct nm_mdmprxy_oiface_rule_info *orule;
  int rc, cpyres;

  NM_MDMPRXY_NULL_CHECK(ifname);

  if (handle < 0 || handle >= NM_MDMPRXY_MAXRULES)
  {
    netmgr_log_err("%s(): Bad handle \"%d\"", __func__, handle);
    return NETMGR_FAILURE;
  }

  if (slot < 0 || slot >= NM_MDMPRXY_MAXOUTRULES)
  {
    netmgr_log_err("%s(): Bad destination slot \"%d\"", __func__, slot);
    return NETMGR_FAILURE;
  }

  if (!installed_rules[handle])
  {
    return NETMGR_FAILURE;
  }

  orule = &(installed_rules[handle]->outrules[slot]);

  orule->parent_handle = handle;
  cpyres = strlcpy(orule->oif, ifname, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  if(valid4)
  {
    if (orule->valid4 == 1)
    {
      netmgr_log_err("%s(): Already have a v4 oiface rule for [%s]",
                      __func__, ifname);
      return NETMGR_FAILURE;
    }
    orule->valid4 = 1;
    netmgr_log_low("%s(): Installing v4 oiface rules for [%s]",
                   __func__, ifname);
    orule->gw4 = gw4;
    orule->daddr4 = daddr4;
    rc = _nm_mdmprxy_install_destination_rules(handle, AF_INET, slot);
    if (rc != NETMGR_SUCCESS) {
      orule->valid4 = 0;
      netmgr_log_err("%s(): Failed to install v4 dest rules for [%s]",
                     __func__, ifname);
      goto error;
    }

    /* Install the rule to forward packets between rev rmnet and the provided iiface */
    rc = _nm_mdmprxy_install_fwd_rules(handle, AF_INET, slot);
    if (rc != NETMGR_SUCCESS)
    {
      orule->valid4 = 0;
      netmgr_log_err("%s(): Failed to install v4 forwarding rules for [%s]",
                     __func__, ifname);
      goto error;
    }
  }

  if(valid6)
  {
    if (orule->valid6 == 1)
    {
      netmgr_log_err("%s(): Already have a v6 oiface rule for [%s]",
                     __func__, ifname);
      return NETMGR_FAILURE;
    }
    orule->valid6 = 1;
    netmgr_log_low("%s(): Installing v6 oiface rules for [%s]",
                   __func__, ifname);

    if (gw6)
    {
      memcpy(&(orule->gw6), gw6, sizeof(*gw6));
    }

    if (daddr6)
    {
      memcpy(&(orule->daddr6), daddr6, sizeof(*daddr6));
    }

    orule->mask6 = mask6;
    rc = _nm_mdmprxy_install_destination_rules(handle, AF_INET6, slot);
    if (rc != NETMGR_SUCCESS)
    {
      orule->valid6 = 0;
      netmgr_log_err("%s(): Failed to install v dest rules for [%s]",
                     __func__, ifname);
      goto error;
    }

    /* Install the rule to forward packets between rev rmnet and the provided iiface */
    rc = _nm_mdmprxy_install_fwd_rules(handle, AF_INET6, slot);
    if (rc != NETMGR_SUCCESS)
    {
      orule->valid6 = 0;
      netmgr_log_err("%s(): Failed to install v6 forwarding rules for [%s]",
                     __func__, ifname);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  return NETMGR_FAILURE;
}

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
/* TODO: take iiface handle instead of mark and grab the mark from there */
int
nm_mdmprxy_uninstall_udpencap_rules
(
  int dport,
  int sport,
  int mark
)
{
  int rc = _nm_mdmprxy_uninstall_udpencap_rules(AF_INET,
                                                dport,
                                                sport,
                                                mark,
                                                mark_prov_cmd_tbl->udp_encap_rule_list_len - 1);
  return rc;
}

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
/* TODO: take iiface handle instead of mark and grab the mark from there */
int
nm_mdmprxy_install_udpencap_rules
(
  int dport,
  int sport,
  int mark
)
{
  int rc = _nm_mdmprxy_install_udpencap_rules(AF_INET, dport, sport, mark);
  return rc;
}

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
int
nm_mdmprxy_uninstall_connmark_rules
(
  sa_family_t  af,
  const char*  proto,
  const char*  ifname,
  int          mark,
  int          start
)
{
  const struct nm_mdmprxy_cmd *cmd_list;
  struct conntrack_rule_info nfo;
  int i;
  int rc = NETMGR_SUCCESS;
  size_t len;

  NM_MDMPRXY_NULL_CHECK(mark_prov_cmd_tbl->per_iface_conntrack_rule_list);

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;

  len = strlcpy(nfo.proto, proto, NM_MDMPRXY_MAXPROTOSTR);
  if (len >= NM_MDMPRXY_MAXPROTOSTR)
  {
    rc = NETMGR_FAILURE;
    goto error;
  }

  len = strlcpy(nfo.iif, ifname, NM_MDMPRXY_MAXDEVSTR);
  if (len >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  cmd_list = mark_prov_cmd_tbl->per_iface_conntrack_rule_list;

  if (start >= mark_prov_cmd_tbl->per_iface_conntrack_rule_list_len)
  {
    rc = NETMGR_FAILURE;
    goto error;
  }

  for (i = start; i >= 0; i--)
  {
    rc |= nm_mdmprxy_process_rule(&cmd_list[i], NM_MDMPRXY_RUN_ANTIRULE, af, &nfo);
  }

error:
  return rc;
}

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
int
nm_mdmprxy_install_connmark_rules
(
  sa_family_t  af,
  const char*  proto,
  const char*  ifname,
  int          mark
)
{
  const struct nm_mdmprxy_cmd *cmd_list;
  struct conntrack_rule_info nfo;
  int i = 0, rc;
  size_t len;

  NM_MDMPRXY_NULL_CHECK(mark_prov_cmd_tbl->per_iface_conntrack_rule_list);

  memset(&nfo, 0, sizeof(nfo));
  nfo.mark = mark;

  len = strlcpy(nfo.proto, proto, NM_MDMPRXY_MAXPROTOSTR);
  if (len >= NM_MDMPRXY_MAXPROTOSTR)
  {
    goto error;
  }

  len = strlcpy(nfo.iif, ifname, NM_MDMPRXY_MAXDEVSTR);
  if (len >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  cmd_list = mark_prov_cmd_tbl->per_iface_conntrack_rule_list;
  for (i = 0; i < mark_prov_cmd_tbl->per_iface_conntrack_rule_list_len; i++)
  {
    if (!cmd_list[i].rule)
    {
      continue;
    }

    rc = nm_mdmprxy_process_rule(&cmd_list[i], NM_MDMPRXY_RUN_RULE, af, &nfo);
    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_uninstall_connmark_rules(af, proto, ifname, mark, i);
  return NETMGR_FAILURE;
}

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
int
nm_mdmprxy_flush_connmark_rules(sa_family_t af)
{
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd flush_conntrack_rules;
  struct conntrack_rule_info nfo;

  memset(&flush_conntrack_rules, 0, sizeof(struct nm_mdmprxy_cmd));
  memset(&nfo, 0, sizeof(nfo));

  flush_conntrack_rules.rule = "%s %s -t mangle -F nm_mdmprxy_icmp_pkt_marker";
  flush_conntrack_rules.antirule = NULL;
  flush_conntrack_rules.formatter = nm_mdmprxy_basic_formatter;

  rc = nm_mdmprxy_process_rule(&flush_conntrack_rules, NM_MDMPRXY_RUN_RULE, af, &nfo);

  return rc;
}

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
nm_mdmprxy_flush_spi_rules(sa_family_t af)
{
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd flush_spi_rules;
  struct spi_rule_info nfo;

  memset(&flush_spi_rules, 0, sizeof(struct nm_mdmprxy_cmd));
  memset(&nfo, 0, sizeof(nfo));

  flush_spi_rules.rule = "%s %s -t mangle -F nm_mdmprxy_mngl_pre_spi";
  flush_spi_rules.antirule = NULL;
  flush_spi_rules.formatter = nm_mdmprxy_basic_formatter;

  rc = nm_mdmprxy_process_rule(&flush_spi_rules, NM_MDMPRXY_RUN_RULE, af, &nfo);

  return rc;
}

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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;
  struct lbo_rule_info lbo_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->local_breakout_rule_list);
  NM_MDMPRXY_NULL_CHECK(iif);
  NM_MDMPRXY_NULL_CHECK(oif);

  memset(&lbo_rule, 0, sizeof(lbo_rule));

  cpyres = strlcpy(lbo_rule.iif, iif, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    rc = NETMGR_FAILURE;
    goto error;
  }

  cpyres = strlcpy(lbo_rule.oif, oif, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    rc = NETMGR_FAILURE;
    goto error;
  }

  lbo_rule.gw4 = gw4;

  if (gw6)
  {
    memcpy(&(lbo_rule.gw6), gw6, sizeof(*gw6));
  }

  for (i = start; i >= 0; i--)
  {
    if (!route_prov_cmd_tbl->local_breakout_rule_list[i].rule)
      continue;

    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->local_breakout_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, ip_family, &lbo_rule);
  }

error:
  return rc;
}

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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;
  struct lbo_rule_info lbo_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->local_breakout_rule_list);
  NM_MDMPRXY_NULL_CHECK(iif);
  NM_MDMPRXY_NULL_CHECK(oif);

  memset(&lbo_rule, 0, sizeof(lbo_rule));

  cpyres = strlcpy(lbo_rule.iif, iif, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  cpyres = strlcpy(lbo_rule.oif, oif, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  lbo_rule.gw4 = gw4;

  if (gw6)
  {
    memcpy(&(lbo_rule.gw6), gw6, sizeof(*gw6));
  }

  for (i = 0; i < route_prov_cmd_tbl->local_breakout_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->local_breakout_rule_list[i].rule)
      continue;

    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->local_breakout_rule_list[i],
                                  NM_MDMPRXY_RUN_RULE, ip_family, &lbo_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_remove_local_breakout_rules(iif, oif, gw4, gw6, ip_family, i);
  return NETMGR_FAILURE;
}

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
nm_mdmprxy_check_iptables_socket_match_support(void)
{
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd check_skt_match =
  { "%s %s -t mangle -A PREROUTING -m socket --nowildcard --restore-skmark"
                          " -j ACCEPT",
    "%s %s -t mangle -D PREROUTING -m socket --nowildcard --restore-skmark"
                          " -j ACCEPT",
    nm_mdmprxy_basic_formatter
  };

  rc |= nm_mdmprxy_process_rule(&check_skt_match, NM_MDMPRXY_RUN_RULE, AF_INET, NULL);
  rc |= nm_mdmprxy_process_rule(&check_skt_match, NM_MDMPRXY_RUN_ANTIRULE, AF_INET, NULL);

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_set_iptables_command_options
===========================================================================*/
/*!
@brief
  Checks whether iptables has support for specific command options ex. -W
*/
/*=========================================================================*/
void
nm_mdmprxy_set_iptables_command_options(void)
{
#ifdef FEATURE_DS_LINUX_ANDROID
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd check_W_option =
  { "%s -w -W 10000 -t mangle -A PREROUTING -m mark --mark 0x20 -j ACCEPT",
    "%s -w -W 10000 -t mangle -D PREROUTING -m mark --mark 0x20 -j ACCEPT",
    nm_mdmprxy_basic_formatter
  };

  rc |= nm_mdmprxy_process_rule(&check_W_option, NM_MDMPRXY_RUN_RULE, AF_INET, NULL);
  rc |= nm_mdmprxy_process_rule(&check_W_option, NM_MDMPRXY_RUN_ANTIRULE, AF_INET, NULL);

  if (NETMGR_SUCCESS == rc)
  {
    (void) strlcpy(iptables_options,
                   " -w -W 10000",
                   sizeof(iptables_options));
  }
  else
  {
    (void) strlcpy(iptables_options,
                   " -w",
                   sizeof(iptables_options));
  }
#else
  memset(iptables_options, 0, sizeof(iptables_options));
#endif
}

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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->local_breakout_doze_rule_list);

  for (i = 0; i < route_prov_cmd_tbl->local_breakout_doze_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->local_breakout_doze_rule_list[i].rule)
      continue;

    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->local_breakout_doze_rule_list[i],
                                  NM_MDMPRXY_RUN_RULE, ip_family, NULL);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_remove_local_breakout_doze_rules(ip_family, i);
  return NETMGR_FAILURE;
}


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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;
  struct doze_rule_tunnel_info doze_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->tunnel_doze_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(tunnel);

  if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG != tech_type)
  {
    goto error;
  }

  memset(&doze_rule, 0, sizeof(doze_rule));

  doze_rule.tunnel = *tunnel;

  for (i = 0; i < route_prov_cmd_tbl->tunnel_doze_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->tunnel_doze_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->tunnel_doze_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, tunnel->tunnel_family, &doze_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_remove_tunnel_doze_rules(link_name, tech_type, tunnel);
  return NETMGR_FAILURE;
}

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
)
{
  int rc = NETMGR_FAILURE;
  int i = 0;
  struct doze_rule_tunnel_info doze_rule;
  int cpyres;

  NM_MDMPRXY_VAR_UNUSED(link_name);

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->tunnel_doze_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(tunnel);

  if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG != tech_type)
  {
    goto error;
  }

  memset(&doze_rule, 0, sizeof(doze_rule));

  doze_rule.tunnel = *tunnel;

  for (i = 0; i < route_prov_cmd_tbl->tunnel_doze_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->tunnel_doze_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->tunnel_doze_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_ANTIRULE, tunnel->tunnel_family, &doze_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  rc = NETMGR_SUCCESS;

error:
  return rc;
}

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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;
  vpn_skip_tunnel_info vpn_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->vpn_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(tunnel);

  if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG != tech_type)
  {
    goto error;
  }

  memset(&vpn_rule, 0, sizeof(vpn_rule));

  vpn_rule.tunnel = *tunnel;

  for (i = 0; i < route_prov_cmd_tbl->vpn_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->vpn_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->vpn_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, tunnel->tunnel_family, &vpn_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d] for link=%s", __func__, i, link_name);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_remove_vpn_skip_rule(link_name, tech_type, tunnel);
  return NETMGR_FAILURE;
}

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
)
{
  int rc = NETMGR_FAILURE;
  int i = 0;
  vpn_skip_tunnel_info vpn_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->vpn_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(tunnel);

  if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG != tech_type)
  {
    goto error;
  }

  memset(&vpn_rule, 0, sizeof(vpn_rule));

  vpn_rule.tunnel = *tunnel;

  for (i = 0; i < route_prov_cmd_tbl->vpn_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->vpn_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->vpn_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_ANTIRULE, tunnel->tunnel_family, &vpn_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d] for link=%s", __func__, i, link_name);
      goto error;
    }
  }

  rc = NETMGR_SUCCESS;

error:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_install_per_iface_doze_rules
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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;
  struct doze_rule_per_iface_info doze_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iface_doze_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(link_name);

  memset(&doze_rule, 0, sizeof(doze_rule));

  cpyres = strlcpy(doze_rule.link_name, link_name, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  for (i = 0; i < route_prov_cmd_tbl->per_iface_doze_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->per_iface_doze_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iface_doze_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_RULE, ip_family, &doze_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to apply rule[%d]", __func__, i);
      goto error;
    }
  }

  return NETMGR_SUCCESS;

error:
  nm_mdmprxy_remove_per_iface_doze_rules(link_name, ip_family);
  return NETMGR_FAILURE;
}

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
)
{
  int rc = NETMGR_FAILURE;
  int i = 0;
  struct doze_rule_per_iface_info doze_rule;
  int cpyres;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->per_iface_doze_skip_rule_list);
  NM_MDMPRXY_NULL_CHECK(link_name);

  memset(&doze_rule, 0, sizeof(doze_rule));

  cpyres = strlcpy(doze_rule.link_name, link_name, NM_MDMPRXY_MAXDEVSTR);
  if (cpyres >= NM_MDMPRXY_MAXDEVSTR)
  {
    goto error;
  }

  for (i = 0; i < route_prov_cmd_tbl->per_iface_doze_skip_rule_list_len; i++)
  {
    if (!route_prov_cmd_tbl->per_iface_doze_skip_rule_list[i].rule)
      continue;

    rc = nm_mdmprxy_process_rule(&route_prov_cmd_tbl->per_iface_doze_skip_rule_list[i],
                                 NM_MDMPRXY_RUN_ANTIRULE, ip_family, &doze_rule);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("%s(): failed to remove rule[%d]", __func__, i);
      goto error;
    }
  }

  rc = NETMGR_SUCCESS;

error:
  return rc;
}
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
)
{
  int rc = NETMGR_SUCCESS;
  int i = 0;

  NM_MDMPRXY_NULL_CHECK(route_prov_cmd_tbl->local_breakout_doze_rule_list);

  for (i = start; i >= 0; i--)
  {
    if (!route_prov_cmd_tbl->local_breakout_doze_rule_list[i].rule)
      continue;

    rc |= nm_mdmprxy_process_rule(&route_prov_cmd_tbl->local_breakout_doze_rule_list[i],
                                  NM_MDMPRXY_RUN_ANTIRULE, ip_family, NULL);
  }

error:
  return rc;
}

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
nm_mdmprxy_flush_doze_rules(sa_family_t af)
{
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd flush_doze_rules;
  struct doze_rule_per_iface_info nfo;

  memset(&flush_doze_rules, 0, sizeof(struct nm_mdmprxy_cmd));
  memset(&nfo, 0, sizeof(nfo));

  flush_doze_rules.rule = "%s %s -t filter -F nm_mdmprxy_doze_mode_skip";
  flush_doze_rules.antirule = NULL;
  flush_doze_rules.formatter = nm_mdmprxy_basic_formatter;

  rc = nm_mdmprxy_process_rule(&flush_doze_rules, NM_MDMPRXY_RUN_RULE, af, &nfo);

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_flush_vpn_skip_rules
===========================================================================*/
/*!
@brief
  Flushes the VPN skip rules when SSR is detected

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
int
nm_mdmprxy_flush_vpn_skip_rules(void)
{
  int rc = NETMGR_SUCCESS;
  struct nm_mdmprxy_cmd flush_vpn_rules;

  memset(&flush_vpn_rules, 0, sizeof(struct nm_mdmprxy_cmd));

  flush_vpn_rules.rule = "%s del prio 11999";
  flush_vpn_rules.antirule = NULL;
  flush_vpn_rules.formatter = nm_mdmprxy_ip_rule_formatter;

  rc = nm_mdmprxy_process_rule(&flush_vpn_rules,
                               NM_MDMPRXY_RUN_RULE,
                               AF_INET,
                               NULL);

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_iptables_init
===========================================================================*/
/*!
@brief
  Reset the iiface handles and install the static fules for AF_INET
  and AF_INET6

*/
/*=========================================================================*/
void
nm_mdmprxy_iptables_init()
{
  memset(installed_rules, 0, sizeof(struct nm_mdmprxy_iiface_rule_info *)*NM_MDMPRXY_MAXRULES);
  nm_mdmprxy_install_static_rules(AF_INET, NM_MDMPRXY_MARK);
  nm_mdmprxy_install_static_rules(AF_INET6, NM_MDMPRXY_MARK);
  netmgr_log_low("%s(): Initialization finished", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_iptables_uninit
===========================================================================*/
/*!
@brief
  Clean up all iptables rules

*/
/*=========================================================================*/
void
nm_mdmprxy_iptables_uninit()
{
  int i = 0, j;

  for (i = 0; i < NM_MDMPRXY_MAXRULES; i++) {
    nm_mdmprxy_uninstall_iiface_rules(i);
  }
}

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
)
{
  int rc = NETMGR_FAILURE;

  if (!rule_tbl)
  {
    netmgr_log_err("%s(): Failed to register route provider command table!", __func__);
    goto bail;
  }

  memset(&route_prov_cmd_tbl, 0, sizeof(route_prov_cmd_tbl));
  route_prov_cmd_tbl = rule_tbl;

  rc = NETMGR_FAILURE;

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_register_mark_prov_cmd_tbl
===========================================================================*/
/*!
@brief
  Initialize all the iptables command arrays used by marking providers

*/
/*=========================================================================*/
int
nm_mdmprxy_register_mark_prov_cmd_tbl
(
  struct mark_prov_iptables* rule_tbl
)
{
  int rc = NETMGR_FAILURE;

  if (!rule_tbl)
  {
    netmgr_log_err("%s(): Failed to register route provider command table!", __func__);
    goto bail;
  }

  memset(&mark_prov_cmd_tbl, 0, sizeof(mark_prov_cmd_tbl));
  mark_prov_cmd_tbl = rule_tbl;

  rc = NETMGR_FAILURE;

bail:
  return rc;
}
