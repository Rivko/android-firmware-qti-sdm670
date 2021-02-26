/*============================================================================
  @file therm_npa_mitigate.c

  Define NPA nodes representing mitigation states.

  Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/src/target/therm_npa_mitigate.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "npa_resource.h"
#include "CoreVerify.h"
#include "therm_log.h"

/*=======================================================================

                  STATIC MEMBER / FUNCTION DECLARATIONS / DEFINITIONS

========================================================================*/

static npa_resource_state therm_node_mitigate_driver_fcn(npa_resource *resource,
                                                         npa_client   *client,
                                                         npa_resource_state state);

/*=======================================================================

                  GLOBAL DEFINITIONS

========================================================================*/
/* Supplies device list for QMI request for mitigation device list.
   Must edit if additional mitigation devices are added. */
const char *therm_mitigation_devices[] =
{
  "cpuv_restriction_cold",
};

const unsigned int therm_mitigation_devices_count =
 ARR_SIZE(therm_mitigation_devices);

/* "/therm/mitigate" node and resource
   Must edit if additional mitigation devices are added. */

static npa_resource_plugin therm_mitigate_plugin;

static npa_resource_definition therm_mitigate_resource[] =
{
  {
    "/therm/mitigate/cpuv_restriction_cold", /* name of resource */
    "State Level",                           /* Units of the resource */
    0x1,                                     /* Maximum value of resource */
    &therm_mitigate_plugin,
    NPA_RESOURCE_DRIVER_UNCONDITIONAL,
    (void *)0x0,
    NULL
  },
};

static npa_resource_state initial_mitigate_state[] = {1};

npa_node_definition therm_mitigate_node =
{
  "/node/therm/mitigate",            /* Node name - info only */
  therm_node_mitigate_driver_fcn,    /* Driver function for temperature */
  NPA_NODE_DEFAULT,                  /* No attributes */
  NULL,                              /* No User Data */
  NPA_EMPTY_ARRAY,                   /* No Dependencies */
  NPA_ARRAY(therm_mitigate_resource) /* Resources */
};

/* Used to limit max mitigation to zero for special test cases */
static boolean limit_max_zero = FALSE;

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/

/**
  @brief therm_node_mitigate_driver_fcn

  Mitigate node driver function.  Nothing to update.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the
            resource.
            state: Update function state.

  @return : Resource state.
*/
static npa_resource_state therm_node_mitigate_driver_fcn(npa_resource *resource,
                                                         npa_client   *client,
                                                         npa_resource_state state)
{

  npa_resource_definition *definition = resource->definition;
  boolean cust_call_supported = (definition->data) ? (1) : (0);

  if (cust_call_supported)
  {
    if (client->type == NPA_CLIENT_CUSTOM1)
    {
      limit_max_zero = TRUE;
      therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                       "Limit MAX to 0 state %s for %s",
                       (limit_max_zero)?"ON":"OFF",
                       therm_mitigation_devices[resource->index]);
    }
    else if (client->type == NPA_CLIENT_CUSTOM2)
    {
      limit_max_zero = FALSE;
      therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                       "Limit MAX to 0 state %s for %s",
                       (limit_max_zero)?"ON":"OFF",
                       therm_mitigation_devices[resource->index]);
    }

    if (limit_max_zero)
    {
      state = 0;
    }
  }

  therm_log_printf(THERM_LOG_LEVEL_STATS, 2,
                   "Mitigation ( %s:%d )",
                   therm_mitigation_devices[resource->index],
                   state);

  return state;
}

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/

/**
  @brief therm_npa_mitigate_init

  Thermal mitigate init function.

  @param  : None

  @return : None.
*/
void therm_npa_mitigate_init( void )
{
  /* Copy all attributes of standard NPA max pluggin */
  memcpy(&therm_mitigate_plugin, &npa_max_plugin, sizeof(npa_resource_plugin));
  therm_mitigate_plugin.supported_clients |= (NPA_CLIENT_CUSTOM1 |
                                              NPA_CLIENT_CUSTOM2);

  /* Define nodes */
  npa_define_node( &therm_mitigate_node, initial_mitigate_state, NULL );
}
