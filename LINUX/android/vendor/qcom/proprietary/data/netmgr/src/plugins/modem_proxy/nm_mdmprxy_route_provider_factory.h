/**************************************************************************************

       N M _ M D M P R X Y _ R O U T E _ P R O V I D E R _ F A C T O R Y . H

***************************************************************************************/

/**************************************************************************************

  @file    nm_mdmprxy_route_provider_factory.h
  @brief   Modem proxy plugin

  DESCRIPTION
  Header file for route provider factory

**************************************************************************************/
/*====================================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================================================*/

#ifndef __MDM_PROXY_ROUTE_FACTORY_H__
#define __MDM_PROXY_ROUTE_FACTORY_H__
#endif /* __MDM_PROXY_ROUTE_FACTORY_H__ */

#include <sys/socket.h>
#include <inttypes.h>
#ifndef NETMGR_OFFTARGET
#include <netinet/in6.h>
#endif

#include "netmgr_kif_cb.h"

typedef struct route_provider_s
{
  /* Install and remove routing rules */
  int (*add_route)    (int, char*, int, sa_family_t, ipsec_tunnel_info_t*,
                       struct netmgr_kif_addrstorage*);
  int (*remove_route) (int, char*, int, sa_family_t, ipsec_tunnel_info_t*,
                       struct netmgr_kif_addrstorage*);

  /* Install and remove the corresponding iptables rules */
  int (*install_rule) (int, char*, int, int, uint32_t, int, struct in6_addr *daddr6, uint32_t, ipsec_tunnel_info_t*);
  int (*remove_rule)  (int, char*, int, int, ipsec_tunnel_info_t*,struct netmgr_kif_addrstorage*);

  /* Global OOS and IS handlers for the provider */
  void (*init)                  (void);
  void (*is_hdlr)               (void);
  void (*oos_hdlr)              (void);

  /* Per link OOS and IS handlers for the provider */
  int (*is_hdlr_per_link)      (void*);
  int (*oos_hdlr_per_link)     (void*);

  /* IS handlers for netd restart. Currently we don't have support for netd OOS */
  int (*netd_is_hdlr)          (void);
  int (*netd_is_hdlr_per_link) (void*);

  void (*sigusr1_hdlr) (void);
} nm_mdmprxy_route_prov_t;

/*====================================================================================
  FUNCTION  nm_mdmprxy_get_route_provider
====================================================================================*/
/*!
@brief
  Returns the route provider instance based on the platform

@return
  struct route_provider instance

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*==================================================================================*/
nm_mdmprxy_route_prov_t *nm_mdmprxy_get_route_provider(void);

