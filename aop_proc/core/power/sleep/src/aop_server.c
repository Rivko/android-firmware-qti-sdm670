/*===========================================================================
                              aop_server.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "unpa_resource.h"
#include "unpa.h"
#include "pwr_utils_lvl.h"
#include "rpmh_client.h"
#include "cmd_db.h"
#include "CoreVerify.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
enum
{
  RAIL_MX = 0,
  RAIL_CX = 1,
  RAIL_EBI = 2,
  RAIL_GFX = 3,
#if !defined(SDX24_STUBS)
  RAIL_LMX = 4,
  // Temporary
  RAIL_VRM_CX = 5,
  
  RAIL_NUM_RAILS
#else
// Temporary
  RAIL_VRM_CX = 4,
  
  RAIL_NUM_RAILS
#endif
};

typedef struct rail_info
{
  unpa_resource_definition definition;
  unpa_resource *resource;
  uint32 voting_addr;
} rail_info;

static unpa_resource_state rail_apply (struct unpa_resource *resource, unpa_client *client, 
                                       unpa_resource_state state);
static unpa_resource_state voltage_apply (struct unpa_resource *resource, unpa_client *client, 
                                          unpa_resource_state state);
                                          
extern bool g_zero_deg_assert;
extern uint32 g_temp_nom_volt;
extern bool is_cx_pc_path (unpa_resource_state cx_state, unpa_resource_state mx_state);

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static rail_info rails[] =
{
  {
    .definition =
    {
      .name = "mx.lvl",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = rail_apply,
      .max = RAIL_VOLTAGE_LEVEL_MAX,
      .attributes = UNPA_RESOURCE_ASYNC_REQUESTS,
    },
    .resource = NULL,
    .voting_addr = 0
  },
  {
    .definition =
    {
      .name = "cx.lvl",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = rail_apply,
      .max = RAIL_VOLTAGE_LEVEL_MAX,
      .attributes = UNPA_RESOURCE_ASYNC_REQUESTS,
    },
    .resource = NULL,
    .voting_addr = 0
  },
  {
    .definition =
    {
      .name = "ebi.lvl",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = rail_apply,
      .max = RAIL_VOLTAGE_LEVEL_MAX,
      .attributes = UNPA_RESOURCE_ASYNC_REQUESTS,
    },
    .resource = NULL,
    .voting_addr = 0
  },
  {
    .definition =
    {
      .name = "gfx.lvl",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = rail_apply,
      .max = RAIL_VOLTAGE_LEVEL_MAX
    },
    .resource = NULL,
    .voting_addr = 0
  },
#if !defined(SDX24_STUBS)
  {
    .definition =
    {
      .name = "lmx.lvl",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = rail_apply,
      .max = RAIL_VOLTAGE_LEVEL_MAX
    },
    .resource = NULL,
    .voting_addr = 0
  },
#endif
  {
    .definition =
    {
      .name = "vrm.cx",
      .update_fcn = unpa_max_update_fcn,
      .driver_fcn = voltage_apply,
      .max = 1082
    },
    .resource = NULL,
    .voting_addr = 0
  },
};
    
static rpmh_client_handle rsc_handle;

#define RAIL_NUM_ASYNC_CBS 10
static struct rail_async_callback {
  uint32 xid;
  unpa_request_done_cb cb;
} rail_async_cbs[RAIL_NUM_ASYNC_CBS];

static rex_crit_sect_type rail_async_lock;


//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
void unpa_send_vote(uint32_t rm, unsigned int level);

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/**
 * <!-- rail_async_completion -->
 */
void rail_async_completion( uint32 xid )
{
  int i;
  for (i = 0; i < RAIL_NUM_ASYNC_CBS; i++)
  {
    if (rail_async_cbs[i].xid == xid)
    {
      unpa_request_done_cb cb = rail_async_cbs[i].cb;
      rail_async_cbs[i].xid = 0;
      
      cb (xid);
      break;
    }
  }
  CORE_VERIFY (i != RAIL_NUM_ASYNC_CBS);
}

/**
 * <!-- aop_server_init -->
 *
 * @brief Initializes the AOP server component
 *
 * @return N/A
 */
void aop_server_init (void)
{
  // Get a handle for voting via RSC
  rsc_handle = rpmh_create_handle (RSC_DRV_AOP, "aop_server");

  // Register a catch-all completion callback for async requests to rails
  rpmh_client_register_completion_callback (rsc_handle, 
                                            (rpmh_completion_cb)rail_async_completion);
                                           
  rex_init_crit_sect (&rail_async_lock);

  for (int i = 0; i < RAIL_NUM_RAILS; i++)
  {
    rails[i].resource = unpa_create_resource (&rails[i].definition, RAIL_VOLTAGE_LEVEL_OFF);
    CORE_VERIFY (rails[i].resource != NULL);
    rails[i].resource->user_data = (void *)i;
    rails[i].voting_addr = cmd_db_query_addr ( rails[i].resource->definition->name);
  }
}

/**
 * <!-- rail_apply -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
unpa_resource_state cx_state = 0;
static unpa_resource_state rail_apply (struct unpa_resource *resource, unpa_client *client, 
                                       unpa_resource_state state)
{
  CORE_VERIFY (resource != NULL);
  CORE_VERIFY (client != NULL);

#ifdef SDM855_TARGET_CONFIG
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR_L1;
#else
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR;
#endif

  if (client->type == UNPA_CLIENT_INITIALIZE)
  {
    // Don't write through the initial value
    return state;
  }
  
  if (client->type != UNPA_CLIENT_SLEEP)
  {
    int rail_idx = (int)resource->user_data;
    CORE_VERIFY ((rail_idx >= 0) && (rail_idx < RAIL_NUM_RAILS));
    
    /* Override aggregated state for all rails in zero degree conditions,
       if rail is operational */
    if (g_zero_deg_assert && state >= RAIL_VOLTAGE_LEVEL_MIN_SVS &&
        state < g_temp_nom_volt)
    {
      state = g_temp_nom_volt;
    }

    if (rail_idx == RAIL_CX)
    {
      cx_state = state;
    }
    else if (rail_idx == RAIL_MX)
    {
      /* Override aggregated state for MX in cx collapse/restore */
      if ((state != mx_max_vlvl) && (is_cx_pc_path(cx_state, state)))
      {
        state = mx_max_vlvl;
      }
    }

    bool completion = ! (client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET);
    int res_idx = pwr_utils_lvl_resource_idx (resource->definition->name);
    CORE_VERIFY (res_idx >= 0);
    int lvl = pwr_utils_hlvl (res_idx, state);
    CORE_VERIFY (lvl >= 0);
    
    uint32 xid = rpmh_issue_command (rsc_handle, RPMH_SET_ACTIVE, completion,
                                     rails[rail_idx].voting_addr, lvl);

    if (client->request_attr & UNPA_REQUEST_ASYNC)
    {
      client->async_data = xid;
      if (xid)
      {
        int i;
        
        rex_enter_crit_sect (&rail_async_lock);
        for (i = 0; i < RAIL_NUM_ASYNC_CBS; i++)
        {
          if (rail_async_cbs[i].xid == 0)
          {
            rail_async_cbs[i].xid = xid;
            rail_async_cbs[i].cb = client->done_cb;
            break;
          }
        }
        rex_leave_crit_sect (&rail_async_lock);
        
        CORE_VERIFY (i != RAIL_NUM_ASYNC_CBS);

        // This must happen AFTER the (xid, cb) tuple is entered into the array
        rpmh_notify_completion_single (rsc_handle, xid);
      }
    }
  }

  return state;
}

/**
 * <!-- voltage_apply -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static unpa_resource_state voltage_apply (struct unpa_resource *resource, unpa_client *client, 
                                          unpa_resource_state state)
{
  CORE_VERIFY (resource != NULL);
  CORE_VERIFY (client != NULL);

  if (client->type == UNPA_CLIENT_INITIALIZE)
  {
    // Don't write through the initial value
    return state;
  }
  
  int user_data = (int)resource->user_data;
  CORE_VERIFY ((user_data >= 0) && (user_data < RAIL_NUM_RAILS));

  bool completion = ! (client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET);
  
  uint32 xid = rpmh_issue_command (rsc_handle, RPMH_SET_ACTIVE, completion,
                                   rails[user_data].voting_addr, state);

  return state;
}

