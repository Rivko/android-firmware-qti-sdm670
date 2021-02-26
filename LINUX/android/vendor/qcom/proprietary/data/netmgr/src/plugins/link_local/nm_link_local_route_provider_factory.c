/******************************************************************************

  N M _ L I N K _ L O C A L _ R O U T E _ P R O V I D E R _ F A C T O R Y . C

******************************************************************************/

/******************************************************************************

  @file    nm_link_local_route_provider_factory.c
  @brief   Link-local routing plugin

  DESCRIPTION
  Manage the routing rules required for link-local data calls

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <cutils/properties.h>

#include "netmgr.h"
#include "netmgr_util.h"

#include "nm_link_local_route_provider_factory.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define PLATFORM_VERSION_PROP "ro.build.version.release"

nm_ll_route_provider_t *ll_route_provider;

typedef enum
{
  PLATFORM_VERSION_INVALID = -1,
  PLATFORM_VERSION_LE,
  PLATFORM_VERSION_L,
  PLATFORM_VERSION_LMR1,
  PLATFORM_VERSION_MAX
} platform_version_t;

extern nm_ll_route_provider_t ll_nm_link_local_ndc_prov;

/*===========================================================================
  FUNCTION  nm_link_local_get_platform_version
===========================================================================*/
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
/*=========================================================================*/
static platform_version_t
nm_link_local_get_platform_version(void)
{
  static ds_target_t target                  = DS_TARGET_INVALID;
  char platform_property[PROPERTY_VALUE_MAX] = "";
  platform_version_t platform_version        = PLATFORM_VERSION_INVALID;

  NETMGR_LOG_FUNC_ENTRY;

  /* Check whether the target is LE */
  if (target == DS_TARGET_LE_LEGACY
        || target == DS_TARGET_LE_MDM9X15
        || target == DS_TARGET_LE_MDM9X25
        || target == DS_TARGET_LE_MDM9X35
        || target == DS_TARGET_LE_TESLA)
  {
    platform_version = PLATFORM_VERSION_LE;
    goto bail;
  }

  /* If not LE, check for Android version */
  (void) property_get(PLATFORM_VERSION_PROP, platform_property, "");

  if (NULL == strstr(platform_property, "5."))
  {
    /* Android version will be in the form 5.* for L-release */
    platform_version = PLATFORM_VERSION_L;
  }
  else
  {
    /* TODO: Once the version number is fixed we need to check against wuth the proper number.
     * For now we default to LMR1 if the property is not L */
    platform_version = PLATFORM_VERSION_LMR1;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return platform_version;
}

/*===========================================================================
  FUNCTION  nm_link_local_get_route_provider
===========================================================================*/
/*!
@brief
  Returns the route provider instance based on the platform

@return
  Address of a route_provider instance

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
nm_ll_route_provider_t *nm_link_local_get_route_provider(void)
{
  platform_version_t platform_version = nm_link_local_get_platform_version();

  NETMGR_LOG_FUNC_ENTRY;

  if (PLATFORM_VERSION_L == platform_version
      || PLATFORM_VERSION_LMR1 == platform_version)
  {
    /* For now just return ndc provider */
    ll_route_provider = &ll_nm_link_local_ndc_prov;
  }
  else
  {
    return NULL;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ll_route_provider;
}
