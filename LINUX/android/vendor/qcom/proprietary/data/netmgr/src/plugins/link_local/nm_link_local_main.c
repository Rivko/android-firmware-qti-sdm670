/******************************************************************************

                  N M _ L I N K _ L O C A L _ M A I N . C

******************************************************************************/

/******************************************************************************

  @file    nm_link_local_main.c
  @brief   Link-local routing plugin

  DESCRIPTION
  Manage the routing rules required for link-local data calls

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <sys/socket.h>
#include <linux/if.h>
#include <string.h>

#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_cb.h"
#include "netmgr_main_cb.h"
#include "netmgr_kif_cb.h"

#include "nm_link_local_route_provider_factory.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* Macro to suppress unused variable compiler warnings */
#define NM_PROVIDER_VAR_UNUSED(var)    (var = var)

#define NETMGR_LL_ROUTING_CB_MODNAME    "LINK_LOCAL_ROUTING"

static nm_ll_route_provider_t *route_provider_inst;

/*===========================================================================
  FUNCTION  nm_link_local_routing_add_route
===========================================================================*/
/*!
@brief
  Adds route for link-local call

@note

  - Dependencies
    - Either nm_link_local_routing_init() or nm_link_local_routing_is_handler()
      has been called prior to this function

  - Side Effects
    - None
*/
/*=========================================================================*/
static void*
nm_link_local_routing_add_route(void* arg)
{

  struct netmgr_kif_cb_ifinfo *ifinfo = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  if (!arg)
  {
    netmgr_log_err("%s(): Invalid parameter!", __func__);
    goto bail;
  }

  if (!route_provider_inst)
  {
    netmgr_log_err("%s(): Route provider not inited!", __func__);
    goto bail;
  }

  ifinfo = (struct netmgr_kif_cb_ifinfo *) arg;

  if (ifinfo->type != NETMGR_KIF_CB_IFTYPE_LINK_LOCAL)
  {
    /* Nothing to do, return success */
    return NETMGR_SUCCESS;
  }

  /* Add route */
  if (NETMGR_SUCCESS != route_provider_inst->add_route(ifinfo->link,
                                                       ifinfo->name))
  {
    netmgr_log_err("%s(): failed to setup route for link-local call!", __func__);
    goto bail;
  }

  netmgr_log_low("%s(): Successfully added route for link-local call", __func__);

bail:
  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  nm_link_local_routing_remove_route
===========================================================================*/
/*!
@brief
  Removes route for link-local call

@note

  - Dependencies
    - Either nm_link_local_routing_init() or nm_link_local_routing_is_handler()
      has been called prior to this function

  - Side Effects
    - None
*/
/*=========================================================================*/
static void*
nm_link_local_routing_remove_route(void* arg)
{
  struct netmgr_kif_cb_ifinfo *ifinfo = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  if (!arg)
  {
    netmgr_log_err("%s(): Invalid parameter!", __func__);
    goto bail;
  }

  if (!route_provider_inst)
  {
    netmgr_log_err("%s(): Route provider not inited!", __func__);
    goto bail;
  }

  ifinfo = (struct netmgr_kif_cb_ifinfo *) arg;

  if (ifinfo->type != NETMGR_KIF_CB_IFTYPE_LINK_LOCAL)
  {
    /* Nothing to do, return success */
    return NETMGR_SUCCESS;
  }

  /* Remove route */
  if (NETMGR_SUCCESS != route_provider_inst->remove_route(ifinfo->link, ifinfo->name))
  {
    netmgr_log_err("%s(): failed to remove route for link-local call!", __func__);
    goto bail;
  }

  netmgr_log_low("%s(): Successfully removed route for link-local call", __func__);

bail:
  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  nm_link_local_routing_ssr_per_link_oos_handler
===========================================================================*/
/*!
@brief
  Handler for NETMGR_KIF_CB_LINK_OOS.

@note

  - Dependencies
    - Either nm_link_local_routing_init() or nm_link_local_routing_is_handler()
      has been called prior to this function

  - Side Effects
    - None
*/
/*=========================================================================*/
static void*
nm_link_local_routing_ssr_per_link_oos_handler(void *arg)
{
  netmgr_kif_cb_link_ssr_info_t *ifinfo = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  if (!arg)
  {
    netmgr_log_err("%s(): Invalid parameter!", __func__);
    goto bail;
  }

  if (!route_provider_inst)
  {
    netmgr_log_err("%s(): Route provider not inited!", __func__);
    goto bail;
  }

  ifinfo = (netmgr_kif_cb_link_ssr_info_t *) arg;

  if (ifinfo->type != NETMGR_KIF_CB_IFTYPE_LINK_LOCAL)
  {
    /* Nothing to do, return success */
    return NETMGR_SUCCESS;
  }

  /* Remove route */
  if (NETMGR_SUCCESS != route_provider_inst->remove_route(ifinfo->link, ifinfo->name))
  {
    netmgr_log_err("%s(): failed to remove route for link-local call!", __func__);
    goto bail;
  }

  netmgr_log_low("%s(): Successfully removed route for link-local call", __func__);

bail:
  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  nm_link_local_routing_netd_is_handler
===========================================================================*/
/*!
@brief
  Global IS handler for netd
*/
/*=========================================================================*/
static void*
nm_link_local_routing_netd_is_handler(void *arg)
{
  NETMGR_LOG_FUNC_ENTRY;

  (void) arg;

  memset(&route_provider_inst, 0x0, sizeof(route_provider_inst));

  /* Get route provider from factory */
  route_provider_inst = nm_link_local_get_route_provider();

  if (!route_provider_inst)
  {
    netmgr_log_err("%s(): Platform version does not support "
                   "link-local route provider", __func__);
    goto bail;
  }

  /* Initialize route provider */
  if (NETMGR_SUCCESS != route_provider_inst->init(TRUE))
  {
    netmgr_log_err("%s(): Failed to initialize route provider!", __func__);
    goto bail;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  nm_link_local_routing_netd_link_is_handler
===========================================================================*/
/*!
@brief
  Per link IS handler for netd restart
*/
/*=========================================================================*/
static void*
nm_link_local_routing_netd_link_is_handler(void *arg)
{
  struct netmgr_kif_cb_ifinfo *ifinfo = NULL;

  if (arg)
  {
    ifinfo = (struct netmgr_kif_cb_ifinfo *) arg;

    /* If interface was up, re-install routing rules */
    if (ifinfo->type == NETMGR_KIF_CB_IFTYPE_LINK_LOCAL
          && ifinfo->flags & IFF_UP)
    {
      nm_link_local_routing_add_route((void *) ifinfo);
    }
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return 0;
}

/*===========================================================================
  FUNCTION  nm_link_local_routing_init
===========================================================================*/
/*!
@brief
  Initializes the link-local routing module

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
nm_link_local_routing_main_init(void)
{
  NETMGR_LOG_FUNC_ENTRY;

  /* Call in-service handler */
  nm_link_local_routing_netd_is_handler(NULL);

  /* Register callbacks with the MAIN table */
  netmgr_cb_insert_cb(NETMGR_KIF_CB_TABLE,
                      NETMGR_KIF_CB_IFCFG_PRE,
                      NETMGR_LL_ROUTING_CB_MODNAME,
                      &nm_link_local_routing_add_route);

  netmgr_cb_insert_cb(NETMGR_KIF_CB_TABLE,
                      NETMGR_KIF_CB_IFDN_PRE,
                      NETMGR_LL_ROUTING_CB_MODNAME,
                      &nm_link_local_routing_remove_route);

  netmgr_cb_insert_cb(NETMGR_KIF_CB_TABLE,
                      NETMGR_KIF_CB_LINK_OOS,
                      NETMGR_LL_ROUTING_CB_MODNAME,
                      &nm_link_local_routing_ssr_per_link_oos_handler);

  netmgr_cb_insert_cb(NETMGR_KIF_CB_TABLE,
                      NETMGR_KIF_CB_NETD_IS,
                      NETMGR_LL_ROUTING_CB_MODNAME,
                      &nm_link_local_routing_netd_is_handler);

  netmgr_cb_insert_cb(NETMGR_KIF_CB_TABLE,
                      NETMGR_KIF_CB_LINK_NETD_IS,
                      NETMGR_LL_ROUTING_CB_MODNAME,
                      &nm_link_local_routing_netd_link_is_handler);

  netmgr_log_low("%s(): complete", __func__);

  NETMGR_LOG_FUNC_EXIT;
}
