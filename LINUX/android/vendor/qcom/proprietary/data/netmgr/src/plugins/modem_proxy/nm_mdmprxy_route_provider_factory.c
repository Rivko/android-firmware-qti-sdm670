/**************************************************************************************

      N M _ M D M P R X Y _ R O U T E _ P R O V I D E R _ F A C T O R Y . C

***************************************************************************************/

/**************************************************************************************

  @file    nm_mdmprxy_route_provider_factory.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Returns the platform specific route provider instance

**************************************************************************************/
/*====================================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================================================*/

/*====================================================================================
                              INCLUDE FILES
====================================================================================*/

#include <string.h>
#include <cutils/properties.h>

#include "netmgr.h"
#include "netmgr_util.h"

#include "nm_mdmprxy_route_provider_factory.h"

/*===================================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===================================================================================*/

#define PLATFORM_VERSION_PROP "ro.build.version.release"

nm_mdmprxy_route_prov_t *iwlan_route_provider;

typedef enum
{
  PLATFORM_VERSION_INVALID = -1,
  PLATFORM_VERSION_LE,
  PLATFORM_VERSION_L,
  PLATFORM_VERSION_LMR1,
  PLATFORM_VERSION_MAX
} platform_version_t;

extern nm_mdmprxy_route_prov_t nm_mdmprxy_ndc_route_provider;
extern nm_mdmprxy_route_prov_t nm_mdmprxy_legacy_route_provider;

/*===================================================================================
  FUNCTION  nm_mdmprxy_get_platform_version
===================================================================================*/
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
/*=================================================================================*/
static platform_version_t
nm_mdmprxy_get_platform_version(void)
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

  if (NULL != strstr(platform_property, "5."))
  {
    /* Android version will be in the form 5.* for L-release */
    platform_version = PLATFORM_VERSION_L;
  }
  else
  {
    /* TODO: Once the version number is fixed we need to check against
     * the proper number.For now we default to LMR1 if the property is not L */
    platform_version = PLATFORM_VERSION_LMR1;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return platform_version;
}

/*===================================================================================
  FUNCTION  nm_mdmprxy_get_route_provider
===================================================================================*/
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
/*=================================================================================*/
nm_mdmprxy_route_prov_t *nm_mdmprxy_get_route_provider(void)
{
  platform_version_t platform_version = nm_mdmprxy_get_platform_version();

  NETMGR_LOG_FUNC_ENTRY;

  if (PLATFORM_VERSION_LMR1 == platform_version)
  {
    /* Return ndc provider */
    iwlan_route_provider = &nm_mdmprxy_ndc_route_provider;
  }
  else if (PLATFORM_VERSION_L == platform_version)
  {
    /* Return legacy provider */
    iwlan_route_provider = &nm_mdmprxy_legacy_route_provider;
  }
  else
  {
    return NULL;
  }

  NETMGR_LOG_FUNC_EXIT;
  return iwlan_route_provider;
}

