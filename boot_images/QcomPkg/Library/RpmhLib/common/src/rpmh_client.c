/*===========================================================================
                              rpmh_client.c

DESCRIPTION: 
  Main client facing functionality. Initialization, sending 
  commands, sending command sets, completing commands/comand sets, invalidating
  commands, entering/exiting sleep, checking if a drv is valid. 

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rpmh_client.h"
#include "rpmh_os.h"
#include "rpmh_resource_commands.h"
#include "rpmh_cmdq.h"
#include "rpmh_tcs.h"
#include "rpmh_client_internal.h"
#include "rpmh_drv_config.h"
//#include "sleepActive.h"
#ifdef RPMH_SENSORS_ISLAND
#include "uSleep_mode_trans.h"
#endif //RPMH_SENSORS_ISLAND

#include <stddef.h>
#include <string.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
boolean RPMH_STANDALONE = FALSE;
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
rpmh_command_set_internal_t g_cmd_set;

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

/*
 * rpmh driver global structure
 */
struct rpmh_s
{
  uint32                    initialized;           //whether or not the driver is initialized
  uint32                    num_resource_commands; //the # of unique resource commands
  rpmh_resource_command_t   *resource_commands;    //the resource command table
  //uint32                    num_resource_commands_dirty[RPMH_NUM_SETS]; //number of RCs dirty
  rpmh_cmdq_t               **rpmh_cmdq;           //command queue
  rpmh_clientq_t            *rpmh_clientq;         //client queue
  boolean                   is_standalone;         //used for pre-sil/bringup
} rpmh;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

//GENERAL TODOs:
//1 - write CppUTest for public APIs
//2 - Look at work queue processing for WP
//3 - sleep mode enable/disable based on sleep votes
//4 - if no TCS support? short circuit sleep/wake cmds?
//5 - FEATURIZE (uImage? offtarget test? command_set? sleep?)
//
//done - 
//    AMC send (2.4.7 from RSC HPG)
//    DRV handling
//    tcs global structure multi-DRV
//    integrate HAL layer
//    make cmds_in_progress dynamic
//    barrier semantics + event handling -> based on req_id?
//    error check DRV ID
//    API for ICB to check if DRV_ID is valid
//    command set support
//    figure out how to define the command_set structure given different DRVs have different tcs sizes
//    sleep/wake tcs configuration
//    support multiple sleep/wake TCS
//    TCS clearing
//    sleep/wake AMC handling
//    invalidates for wake/sleep
//    read requests
//    logging
//    tcs stuck
//
//optimizations
//    issue_command path we look up each resource twice. No need to do that
//    issue_command/issue_command_set and the underlying cmdq functionality could be combined
//    configure_sleep_wake loops through resource list twice. This could be done only once by checking 
//    read and completion could be combined (in cmd)




/**
 * <!-- rpmh_is_drv_id_valid -->
 *
 * @breif Checks whether or not the given RSC+DRV is valid for the context
 *
 * @param drv_id : The RSC+DRV mapping to check
 *
 * @return whether or not the given ID is supported in the given context
 */
boolean rpmh_is_drv_id_valid (RSCSW_DRV_MAPPING drv_id)
{
  return drv_valid (drv_id);
}

/**
 * <!-- rpmh_tcs_num_cmds -->
 *
 * @brief Return the # commands per TCS for this DRV
 *
 * @param drv_id : RSC+DRV id for num cmds check
 *
 * @return number of commands in the TCSs for this DRV
 */
uint32 rpmh_tcs_num_cmds (RSCSW_DRV_MAPPING drv_id)
{
  rpmh_core_verify (rpmh_is_drv_id_valid (drv_id));
  return tcs_size (drv_id);
}

/**
 * <!-- rpmh_create_handle_internal -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 * @param explicit_client : Tells us if this client is explicit or not
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle_internal (RSCSW_DRV_MAPPING drv_id, const char *client_name, boolean explicit_client)
{
  rpmh_lock (FALSE);	
  rpmh_core_verify (rpmh_is_drv_id_valid (drv_id));

  RPMH_LOG_2 ("rpmh_create_handle: (CLIENT: %s) (DRV_ID: 0x%08x)", client_name, drv_id);
  rpmh_client_t *client = rpmh_client_handle_add (rpmh.rpmh_clientq);
  client->name = client_name;
  client->drv_id = drv_id;
  client->req_id = 0;
  if(explicit_client)
  {
    client->explicit_cmds = (rpmh_explicit_cmd_set_t*)rpmh_malloc (sizeof (rpmh_explicit_cmd_set_t));
    rpmh_core_verify_ptr(client->explicit_cmds);
    memset(client->explicit_cmds, 0, sizeof (rpmh_explicit_cmd_set_t));
  }

  rpmh_unlock (FALSE);	
  return (rpmh_client_handle)client;
}

/**
 * <!-- rpmh_create_handle_explicit -->
 *
 * @brief Create an explicit client handle. 
 *
 *  Explicit client means for sleep/wake requests, the client will take
 *  care of telling the RPMh driver exactly what is required via rpmh_issue_command_set. The RPMh
 *  driver will not fill in any wake/sleep values based on active requests.
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands.
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle_explicit (RSCSW_DRV_MAPPING drv_id, const char *client_name)
{
  return rpmh_create_handle_internal(drv_id, client_name, TRUE);
}

/**
 * <!-- rpmh_create_handle -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 * @param explicit_client : Tells us if this client is explicit or not
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle (RSCSW_DRV_MAPPING drv_id, const char *client_name)
{
  return rpmh_create_handle_internal(drv_id, client_name, FALSE);
}

/**
 * <!-- rpmh_explicit_client_invalidate -->
 *
 * @brief Invalidate requests for the given explicit client 
 *
 * @param drv_id : The explicit client on which to invalidate votes
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
void rpmh_invalidate_explicit_cmds(rpmh_client_handle client_handle)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  rpmh_core_verify_ptr(client->explicit_cmds);
  rpmh_explicit_cmd_set_t *explicit_cmds = client->explicit_cmds;
  explicit_cmds->cmd_sets_in_use = 0;
  memset(explicit_cmds->cmd_sets, 0, 
         (sizeof(rpmh_command_set_internal_t) * explicit_cmds->cmd_sets_total));
}

/**
 * <!-- rpmh_client_completion_callback -->
 *
 * @brief Each client is allowed a single callback for completion requests. 
 *        This API registers that callback 
 *
 * @param client_handle : The client associated with the callback being registered
 * @param callback      : The callback function
 */
void rpmh_client_register_completion_callback(rpmh_client_handle client_handle, rpmh_completion_cb callback)
{
  rpmh_lock (FALSE);
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  client->completion_cb = callback;
  rpmh_unlock (FALSE);  
}

/**
 * <!-- rpmh_delete_handle -->
 *
 * @breif Delete a client handle.
 *
 * @param handle : The handle to delete
 */ 
void rpmh_delete_handle (rpmh_client_handle client_handle)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;

  RPMH_LOG_2 ("rpmh_delete_handle: (CLIENT: %s) (DRV_ID: 0x%08x)", client->name, client->drv_id);
  rpmh_client_handle_remove (rpmh.rpmh_clientq, client);
}

/**
 * <!-- rpmh_find_resource_command -->
 *
 * @brief Lookup the resource command for the corresponding address
 *
 * @param address : The address of the corresponding resource command to find
 *
 * @return a pointer to the resource command
 */ 
rpmh_resource_command_t *rpmh_find_resource_command (uint32 address)
{
  uint32 low = 0, high = rpmh.num_resource_commands - 1;

  if (rpmh.num_resource_commands == 0)
  {	    
    return NULL;
  }	

  while (high < rpmh.num_resource_commands && high >= low)
  {
    uint32 mid = (low + high) / 2;
    uint32 current = rpmh.resource_commands[mid].address;

    if (current < address)
    {		
      low = mid + 1;
    }	    
    else if (current > address)
    {		
      high = mid - 1;
    }	    
    else
    {		
      return &rpmh.resource_commands[mid];
    }	    
  }

  return NULL;
}

/**
 * <!-- rpmh_add_resource_command -->
 *
 * @brief Add a resource command to our list with the corresponding address
 *
 * @param address : The address of the corresponding resource command to add
 *
 * @return a pointer to the resource command
 */ 
static rpmh_resource_command_t *rpmh_add_resource_command (uint32 address)
{
  uint32 i, old_num_resource_commands;

  old_num_resource_commands = rpmh.num_resource_commands;
  rpmh.num_resource_commands++;
  rpmh.resource_commands = rpmh_realloc (rpmh.resource_commands, 
                                    rpmh.num_resource_commands * sizeof(rpmh_resource_command_t));
  rpmh_core_verify_ptr (rpmh.resource_commands);

  for (i = 0; i < old_num_resource_commands; ++i)
  {
    if (rpmh.resource_commands[i].address > address)
    {
      // Found entries that belong after us.  Move them and insert here.
      memmove (&rpmh.resource_commands[i+1], &rpmh.resource_commands[i], 
               (old_num_resource_commands - i) * sizeof(rpmh_resource_command_t));
      break;
    }
  }

  // Must be largest in the list; insert at the end.
  rpmh_resource_command_init (&rpmh.resource_commands[i], address);

  return &rpmh.resource_commands[i];
}

/**
 * <!-- rpmh_wait_for_cmd -->
 *
 * @brief Wait for a command to complete
 *
 * @param handle       : The handle the command is associated with
 * @param req_id       : The command ID to wait on
 * @param barrier_type : The type of wait (all vs single)
 * @param polling      : TRUE for polling mode, FALSE for event based handling
 */ 
static void rpmh_wait_for_cmd (rpmh_client_handle client_handle, uint32 req_id, rpmh_barrier_enum barrier_type, boolean polling)
{
  rpmh_lock (FALSE);	
  rpmh_client_t *handle = (rpmh_client_t *)client_handle;
  rpmh_core_verify_ptr(handle);
  switch(barrier_type)
  {
    case RPMH_CALLBACK_ALL:
    case RPMH_CALLBACK_SINGLE:
      rpmh_client_register_req_notification (handle, req_id, barrier_type);
      break;

    case RPMH_BARRIER_ALL:
    case RPMH_BARRIER_SINGLE:
      rpmh_client_wait_on_cmd(handle, req_id, barrier_type, polling);
      break;
    default :
      rpmh_core_verify(FALSE);
      break;
  }	      

  rpmh_unlock (FALSE);
}

/**
 * <!-- rpmh_churn_all -->
 *
 * @brief Poll for all commands up to and including the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_churn_all (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;

  RPMH_LOG_3 ("rpmh_churn_all: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_ALL, TRUE);	
  RPMH_LOG_0 ("rpmh_churn_all_complete");
}

/**
 * <!-- rpmh_churn_single -->
 *
 * @brief Poll for the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_churn_single (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;

  RPMH_LOG_3 ("rpmh_churn_single: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_SINGLE, TRUE);	
  RPMH_LOG_0 ("rpmh_churn_single_complete");
}

/**
 * <!-- rpmh_barrier_all -->
 *
 * @brief Event based wait for all commands up to and including the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_barrier_all (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  RPMH_LOG_3 ("rpmh_barrier_all: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_ALL, FALSE);	
  RPMH_LOG_3 ("rpmh_barrier_all_complete: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
}

/**
 * <!-- rpmh_barrier_single -->
 *
 * @brief Event based wait for the given req_id to complete
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_barrier_single (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  RPMH_LOG_3 ("rpmh_barrier_single: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_SINGLE, FALSE);	
  RPMH_LOG_3 ("rpmh_barrier_single_complete: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
}

/**
 * <!-- rpmh_notify_completion_all -->
 *
 * @brief Trigger the client's callback with the corresponding req_id when all commands up to and 
 *   including the given req_id have completed
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_notify_completion_all (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  RPMH_LOG_3 ("rpmh_notify_completion_all: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_core_verify_ptr(client->completion_cb);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_CALLBACK_ALL, FALSE);	
  RPMH_LOG_3 ("rpmh_notify_completion_all_complete: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
}

/**
 * <!-- rpmh_notify_completion_single -->
 *
 * @brief Trigger the client's callback with the corresponding req_id when this request has finished
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_notify_completion_single (rpmh_client_handle client_handle, uint32 req_id)
{
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  RPMH_LOG_3 ("rpmh_notify_completion_single: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
  rpmh_core_verify_ptr(client->completion_cb);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_CALLBACK_SINGLE, FALSE);	
  RPMH_LOG_3 ("rpmh_notify_completion_single_complete: (HANDLE: %s) (DRV_ID: 0x%08x) (REQ_ID: %d)", client->name, client->drv_id, req_id);
}


/**
 * <!-- rpmh_wait_for_read -->
 *
 * @brief Wait for the given read cmd to complete
 *
 * @param handle  : The handle the command is associated with
 * @param req_id  : The command ID to wait on
 * @param polling : Whether or not we are polling for the command to complete
 *
 * @return The data from the read command
 */ 
uint32 rpmh_wait_for_read (rpmh_client_t *handle, uint32 req_id, boolean polling)
{
  uint32 data = 0;	
  rpmh_lock (FALSE);	
  data = rpmh_client_wait_on_read (handle, req_id, polling);
  rpmh_unlock (FALSE);
  return data;
}

//TODO register this with interrupt handler
/**
 * <!-- rpmh_finish_amc -->
 *
 * @brief Handle a finished AMC
 *
 * @param hw_drv : The RSC+DRV mapping for the AMC
 */ 
void rpmh_finish_amc (RSCSW_DRV_MAPPING hw_drv)
{
  uint32 req_id;
  boolean send_at_head = TRUE;
  rpmh_client_t *finished_client = NULL;
  rpmh_lock (TRUE);

  RSCSW_DRV_MAPPING drv_id = get_finished_drv(hw_drv);
  if (drv_id != RSC_DRV_VIRTUAL_MAX)
  {
    RPMH_LOG_2 ("rpmh_finish_amc (HW_DRV: 0x%08x) (SW_DRV: 0x%08x)", hw_drv, drv_id);
    rpmh_core_verify (rpmh_is_drv_id_valid(drv_id));

    uint32 drv_index = lookup_drv_index (drv_id);

    //clean up at the TCS level. This also cleans up at the resource and queue level	
    req_id = tcs_finish_active_amc (&finished_client, rpmh.rpmh_cmdq[drv_index], drv_id); 

    //when we retrieved the DRV we checked there was an outstanding AMC, so req_id must be non-zero
    rpmh_core_verify (req_id > 0);

    RPMH_LOG_2 ("\tfinished req: (CLIENT: %s)(REQ_ID: %d)", finished_client->name, req_id);
    //clean up at the client level
    rpmh_client_command_finish (finished_client, req_id);

    while(send_at_head)
    {
      //send a queued command
      send_at_head = rpmh_cmdq_send_cmd_at_head (rpmh.rpmh_cmdq[drv_index]);
    }
  }
  else
  {
    RPMH_LOG_0 ("\tNO_AMC_COMPLETED");
  }

  RPMH_LOG_0 ("rpmh_finish_amc_complete");
  rpmh_unlock (TRUE);
}

/**
 * <!-- rpmh_finsh_sleep_wake_amc -->
 *
 * @brief Handle a finished sleep or wake AMC. 
 *
 * @param drv_id : The RSC+DRV mapping for the AMC
 * @param set    : The set associated with the finished AMC
 */ 
void rpmh_finish_sleep_wake_amc (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
  rpmh_lock (FALSE);	
  rpmh_core_verify(rpmh_is_drv_id_valid (drv_id));

  uint32 drv_index = lookup_drv_index (drv_id);

  rpmh_core_verify((set == RPMH_SET_SLEEP) || (set == RPMH_SET_WAKE));

  //clean up at the TCS level. This also cleans up at the resource and queue level	
  tcs_finish_amc (rpmh.rpmh_cmdq[drv_index], drv_id, set); 
  rpmh_unlock (FALSE);
}

/**
 * <!-- get_rc -->
 *
 * @brief Retrieve the resource command pointer for the given addres 
 *
 * @param address : The address of the corresponding resource command to find
 *
 * @return the resource command pointer
 */ 
static inline rpmh_resource_command_t *get_rc (uint32 address)
{
  rpmh_resource_command_t *rc = rpmh_find_resource_command (address);
  if (rc == NULL)
  {	    
    rc = rpmh_add_resource_command (address);
  }
  return rc;
}

/**
 * <!-- rpmh_issue_command_set -->
 *
 * @brief Issue a command set (multiple commands in parallel). If none of the resource commands
 *          are dirty, the command set is not sent. However if at least one resource command is
 *          dirty, the command set will be sent. The not dirty commands will finish quickly as
 *          the HW aggregation won't update, so the dirty commands will be the long pole.
 *
 * @param handle      : The client handle associated with the command set
 * @param command_set : The structure containing all the details of the command set request
 *
 * @return ID for command set on this handle. Used in barrier APIs to track completion
 */ 
uint32 rpmh_issue_command_set (rpmh_client_handle client_handle, rpmh_command_set_t *cmd_set)
{
  rpmh_lock (FALSE);
  uint32 req_id = 0;
  boolean rcs_dirty = FALSE;
  rpmh_client_t *handle = (rpmh_client_t *)client_handle;
  RPMH_LOG_4 ("rpmh_issue_command_set (CLIENT: %s)(DRV_ID: 0x%08x)(SET: %d)(NUM_CMDS: %d)", 
               handle->name, handle->drv_id, cmd_set->set, cmd_set->num_commands);
  //cast to internal command set type to hide dependency bmsk for now
  rpmh_command_set_internal_t *command_set = (rpmh_command_set_internal_t*)cmd_set;

  //remove this if we support dependencies at some point
  command_set->dependency_bmsk = 0;

  rpmh_core_verify (command_set->num_commands >=0 && command_set->num_commands <= IMAGE_TCS_SIZE &&
  	                command_set->num_commands <= 
                    DRV_CONFIG_DATA->drvs[lookup_drv_index (handle->drv_id)].num_cmds);

  boolean is_request_explicit = (handle->explicit_cmds != NULL);

  //update the command set resources
  for (int i = 0; i < command_set->num_commands; i++)
  {
    rpmh_command_t command = command_set->commands[i];	    
    rpmh_resource_command_t *rc = get_rc (command.address);
    RPMH_LOG_2 ("\tresource_command (ADDRESS: 0x%08x)(DATA: 0x%08x)", command.address, command.data);

    //update the resource
    if(rpmh_resource_command_update (rc, command_set->set, command.data, handle->drv_id, is_request_explicit))
    {
      rcs_dirty = TRUE;
    }
  }

  //send if its active
  if(command_set->set == RPMH_SET_ACTIVE)
  {	    
    if(!rcs_dirty)
    {
      RPMH_LOG_0 ("\tredundant commands, short circuiting");
    }
    else if(rpmh.is_standalone)
    {
      RPMH_LOG_0 ("\tstandalone mode, short circuiting");
    }
    else
    {
        //make sure no TCSs are "stuck"
        rpmh_core_verify (!is_tcs_stuck (handle->drv_id));

        //configure client for new request
        rpmh_client_command_add (handle); 

        //send the request
        req_id = rpmh_cmdq_send_cmd_set (rpmh.rpmh_cmdq[lookup_drv_index(handle->drv_id)], handle,
                                         command_set);
        RPMH_LOG_1 ("\treq_id (REQ_ID: %d)", req_id);
    }
  }
  else if(is_request_explicit)
  {
    //if its not active and its an explicit client, save to the client	  
    rpmh_explicit_cmd_set_t *explicit_cmds = handle->explicit_cmds;

    explicit_cmds->cmd_sets_in_use++;

    //make sure we have enough room for a new command set
    if(explicit_cmds->cmd_sets_in_use > explicit_cmds->cmd_sets_total)
    {
      explicit_cmds->cmd_sets = rpmh_realloc (explicit_cmds->cmd_sets, 
                                         explicit_cmds->cmd_sets_in_use * sizeof(rpmh_command_set_internal_t));
      explicit_cmds->cmd_sets_total = explicit_cmds->cmd_sets_in_use;
    }

    //memscpy(explicit_cmds->cmd_sets[explicit_cmds->cmd_sets_in_use - 1], 
    //        sizeof(rpmh_command_set_internal_t), command_set, 
    //        sizeof(rpmh_command_set_internal_t));

    //save the command set
    memcpy(&(explicit_cmds->cmd_sets[explicit_cmds->cmd_sets_in_use - 1]), command_set, 
           sizeof(rpmh_command_set_internal_t));
  }

  rpmh_unlock (FALSE);

  //call Churn queue for UEFI as thsi is in single thread mode and pollign mode.
  rpmh_churn_all(client_handle, req_id);
  return req_id;
}

/**
 * <!-- rpmh_invalidate_command -->
 * @brief Remove the command for a given address (resource command).
 *
 * @param handle  : The client handle associated with the invalidate
 * @param handle  : The set to invalidate
 * @param address : Concatenation of slaveID and offset
 */
static void rpmh_invalidate_command (rpmh_client_handle client_handle, rpmh_set_enum set, uint32 address)
{
  rpmh_client_t *handle = (rpmh_client_t *)client_handle;
  rpmh_resource_command_t *rc = rpmh_find_resource_command (address);
  if ((rc == NULL) || (set == RPMH_SET_ACTIVE))
  {	    
    return;
  }	

  RPMH_LOG_3 ("rpmh_invlidate_command (CLIENT: %s)(ADDRESS: 0x%08x)(SET: %d)", handle->name, address, set);

  //update the resource
  rpmh_resource_command_invalidate (rc, set, handle->drv_id);
}

/**
 * <!-- rpmh_invalidate_sleep_command -->
 * @breif Remove the sleep command for a given address (resource command).
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_sleep_command (rpmh_client_handle client_handle, uint32 address)
{ 
  rpmh_invalidate_command (client_handle, RPMH_SET_SLEEP, address);
}

/**
 * <!-- rpmh_invalidate_wake_command -->
 *
 * @brief Remove the wake command for a given address (resource command). 
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_wake_command (rpmh_client_handle client_handle, uint32 address)
{ 
  rpmh_invalidate_command (client_handle, RPMH_SET_WAKE, address);
}

//This is disabled for V1. When we get around to V2, re-enable.
#if 0
/**
 * <!-- rpmh_issue_read_command -->
 *
 * @brief Issue a read command and return the data. THIS IS A BLOCKING API! 
 *
 *  This will block the given SW thread until the data is read from HW.
 *
 * @param handle  : The client handle associated with the read command
 * @param address : Concatenation of slaveID and offset
 * @param data    : Data to be sent
 * @param polling : TRUE for polling mode, FALSE for event based handling
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
uint32 rpmh_issue_read_command (rpmh_client_handle client_handle, uint32 address, boolean polling)
{
  rpmh_lock (FALSE);	
  uint32 data = 0;
  uint32 req_id = 0;
  boolean v1_workaround_queue = FALSE;
  rpmh_client_t *client = (rpmh_client_t *)client_handle;
  uint32 drv_index = lookup_drv_index (client->drv_id);

  RPMH_LOG_3 ("rpmh_issue_read_command (CLIENT: %s)(ADDRESS: 0x%08x)(POLLING?: %d)", 
              client->name, address, polling);

  if (!rpmh.is_standalone)
  {
    //configure client for new request
    rpmh_client_command_add (client); 

    //[V1 PDC Workaround]
    //   if the command queue is not empty, we have to wait for it to be empty
    if(!cmdq_empty(rpmh.rpmh_cmdq[drv_index]))
    {    
      if(polling)
      {	      
        //churn until there are no messages
        while(!cmdq_empty(rpmh.rpmh_cmdq[drv_index]))
        {
          rpmh_finish_amc(client->drv_id); 
        }
      }	      
      else
      {
        //queue the message if its an event based read
	    v1_workaround_queue = TRUE;
      }
    }
    //send the request
    req_id = rpmh_cmdq_send_read (rpmh.rpmh_cmdq[lookup_drv_index (client->drv_id)], address, client, v1_workaround_queue);
    data = rpmh_wait_for_read(client, req_id, polling);
    RPMH_LOG_3 ("rpmh_issue_read_command_complete (CLIENT: %s)(ADDRESS: 0x%08x)(DATA: 0x%08x)", 
                client->name, address, data);
  }
  else
  {
      RPMH_LOG_0 ("\tstandalone mode, short circuiting");
  }

  rpmh_unlock (FALSE);
  return data;
}
#endif

static void set_g_cmd_set_single(uint32 address, uint32 data, boolean completion, rpmh_set_enum set)
{
  memset(g_cmd_set.commands, 0, sizeof(rpmh_command_t) * IMAGE_TCS_SIZE);
  g_cmd_set.commands->address = address;
  g_cmd_set.commands->data = data;
  g_cmd_set.commands->completion = completion;
  g_cmd_set.set = set;
  g_cmd_set.num_commands = 1;
  g_cmd_set.dependency_bmsk = 0;
}	

static void set_g_cmd_set(rpmh_command_set_internal_t *cmd_set)
{
  //memscpy(&g_cmd_set, sizeof(rpmh_command_set_internal_t, cmd_set sizeof(rpmh_command_set_internal_t));

  memcpy(&g_cmd_set, cmd_set, sizeof(rpmh_command_set_internal_t));
}

/**
 * <!-- rpmh_issue_command -->
 *
 * @brief Issue a single command
 *
 * @param handle     : The client handle associated with the command
 * @param set        : The set associated with the command
 * @param completion : TRUE for completion requests, FALSE for fire and forget
 * @param address    : Concatenation of slaveID and offset
 * @param data       : Data to be sent
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
uint32 rpmh_issue_command(rpmh_client_handle client_handle, rpmh_set_enum set, boolean completion, 
                          uint32 address, uint32 data)
{
  uint32 req_id;	
  rpmh_lock (FALSE);
  set_g_cmd_set_single(address, data, completion, set);
  req_id = rpmh_issue_command_set(client_handle, (rpmh_command_set_t*)(&g_cmd_set));
  rpmh_unlock (FALSE);
  return req_id;
}

static void add_cmd_set_for_sleep(rpmh_command_set_internal_t *cmd_set, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set, rpmh_rc_priority_enum priority)
{
    uint32 drv_index = lookup_drv_index (drv_id);
    if (tcs_slots_available(drv_id, set, cmd_set->num_commands) != -1)
    {
      RPMH_LOG_1 ("\tadding command set to sleep/wake TCS (SET: %d)", set);
      rpmh_core_verify (tcs_prepare_sleep_add_cmd_set((rpmh_command_set_internal_t*)cmd_set, drv_id));
    }
    else
    {
      rpmh_core_verify (priority != RPMH_RC_PRIORITY_SUBSYSTEM);
      RPMH_LOG_1 ("\tadding command set to sleep/wake AMC (SET: %d)", set);
      rpmh_cmdq_send_cmd_set (rpmh.rpmh_cmdq[drv_index], NULL, 
                              (rpmh_command_set_internal_t*)cmd_set);
    }
    return;
}

/**
 * <!-- add_rc_for_sleep -->
 *
 * @brief If needed, add the given RC to sleep config (TCS or sleep/wake AMCs)
 *
 * @param rc         : The resource command to add to sleep config (if needed)
 * @param drv_id     : The RSC+DRV mapping to configure
 * @param sets_dirty : Bitmask of which sets are dirty for the given RC
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
static void add_rc_for_sleep (rpmh_resource_command_t *rc, RSCSW_DRV_MAPPING drv_id)
{
  uint32 sets_dirty = resource_command_sets_dirty (rc, drv_id);
  rpmh_rc_priority_enum priority = rpmh_resource_command_get_priority(rc, drv_id);
  if ((1 << RPMH_SET_SLEEP) & sets_dirty)
  {
    set_g_cmd_set_single(rc->address, 0 /*data not used*/ , FALSE, RPMH_SET_SLEEP);
    add_cmd_set_for_sleep(&g_cmd_set, drv_id, RPMH_SET_SLEEP, priority);
  }
  if ((1 << RPMH_SET_WAKE) & sets_dirty)
  {
    set_g_cmd_set_single(rc->address, 0 /*data not used*/ , TRUE, RPMH_SET_WAKE);
    add_cmd_set_for_sleep(&g_cmd_set, drv_id, RPMH_SET_WAKE, priority);
  }

}

static void add_explicit_cmd_set_for_sleep(rpmh_rc_priority_enum priority_level, RSCSW_DRV_MAPPING drv_id)
{
  inq_iterator_t it;
  //TODO make clientq an array... we will only have a couple clients. Then this becomes easy
  rpmh_client_t *client;
  inq_t *q = rpmh.rpmh_clientq->handles;
  inq_it_begin (q, &it);
  while ((client = (rpmh_client_t *)inq_it_deref (&it)) != NULL)
  {
    if(client->explicit_cmds != NULL)
    {
      for(int i = 0; i < client->explicit_cmds->cmd_sets_in_use; i++)
      {
        rpmh_rc_priority_enum max_priority = RPMH_RC_PRIORITY_USE_CASE;	      
        rpmh_command_set_internal_t *cmd_set = &(client->explicit_cmds->cmd_sets[i]);  
        for(int j = 0; j < cmd_set->num_commands; j++)
        {
          //should we store this in the explicit request itself when issued instead of 
	      //looking it up on the way to sleep?		
          max_priority = MAX(max_priority, rpmh_resource_command_get_priority (get_rc (cmd_set->commands[j].address), drv_id));
        }
        if(max_priority == priority_level)
        {		
          set_g_cmd_set(cmd_set);		
          add_cmd_set_for_sleep(&g_cmd_set, drv_id, cmd_set->set, max_priority);
        }
      }
    }
    inq_it_next (&it);
  }
}

#ifdef RPMH_SENSORS_ISLAND
void rpmh_island_cb (uSleep_state_notification state)
{
  rpmh_lock (FALSE);	
  switch (state)
  {
    case USLEEP_STATE_ENTER:
      tcs_toggle_island_mode (TRUE);
      rpmh_enter_sleep (RSC_DRV_SENSORS);
      rpmh_enter_sleep (RSC_DRV_VIRTUAL_SENSORS);
      break;

    case USLEEP_STATE_EXIT:
      //clean up TCS state?
      rpmh_exit_sleep (RSC_DRV_VIRTUAL_SENSORS);
      rpmh_exit_sleep (RSC_DRV_SENSORS);
      tcs_toggle_island_mode (FALSE);
      break;
    default:
      rpmh_core_verify (FALSE);
      break;
  }
  rpmh_unlock (FALSE);	
}
#endif //RPMH_SENSORS_ISLAND

/**
 * <!-- rpmh_enter_sleep -->
 *
 * @breif API to configure RSCSW_DRV for entering sleep. 
 *        Configures RSC TCSs, sends sleep AMCs if needed  
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep
 */
void rpmh_enter_sleep (RSCSW_DRV_MAPPING drv_id)
{
  rpmh_lock (FALSE);
  if(rpmh.is_standalone)
  {
    RPMH_LOG_0 ("rpmh_enter_sleep short circuit (STANDALONE MODE)");
    rpmh_unlock (FALSE);
    return;
  }

  rpmh_core_verify (rpmh_is_drv_id_valid(drv_id));
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  RPMH_LOG_1 ("rpmh_enter_sleep (DRV_ID: 0x%08x)", drv_id);

#ifdef RPMH_SENSORS_ISLAND
  //for sensors we have to reconfigure one of the TCSs to AMC for active, and a TCS for sleep
  if(drv_id == RSC_DRV_VIRTUAL_SENSORS)
  {
    tcs_update_mode(drv_index, (uint32)SLPI_VIRTUAL_TCS_CONFIG_SLEEP);
  }
#endif //RPMH_SENSORS_ISLAND

  //clean up from the previous sleep before sending again
  tcs_clean_sleep_wake (drv_id);

  if(drv->in_irq_num)
  {
    //disable interrupt
    rpmh_deregister_isr(drv->in_irq_num);
  }
  
  rpmh_core_verify (cmdq_empty(rpmh.rpmh_cmdq[drv_index]));

  //4 categories of requests to check
  //  1) explicit subsystem requests
  //  2) standard subsystem resources
  //  3) explicit use case resources
  //  4) standard use case resources

  //loop through all the "subsystem" explicit command sets
  add_explicit_cmd_set_for_sleep(RPMH_RC_PRIORITY_SUBSYSTEM, drv_id);
  
  //loop through the "subsystem" resources
  for (int i = 0; i < rpmh.num_resource_commands; i++)
  {
    rpmh_resource_command_t *rc = &rpmh.resource_commands[i];
    rpmh_rc_priority_enum priority = rpmh_resource_command_get_priority(rc, drv_id);
    if (priority == RPMH_RC_PRIORITY_SUBSYSTEM) 
    {
      add_rc_for_sleep (rc, drv_id);
    }
  }

  //loop through all the "use case" explicit command sets
  add_explicit_cmd_set_for_sleep(RPMH_RC_PRIORITY_USE_CASE, drv_id);

  //loop through all "use case" resources
  for (int i = 0; i < rpmh.num_resource_commands; i++)
  {
    rpmh_resource_command_t *rc = &rpmh.resource_commands[i];
    rpmh_rc_priority_enum priority = rpmh_resource_command_get_priority(rc, drv_id);
    if (priority != RPMH_RC_PRIORITY_SUBSYSTEM) 
    {   
        add_rc_for_sleep (rc, drv_id);
    }
  }

  //commit changes to the TCSs
  tcs_prepare_sleep_commit (drv_id);

  RPMH_LOG_0 ("\tflushing sleep AMCs");
  rpmh_cmdq_flush_sleep_wake_amcs (rpmh.rpmh_cmdq[drv_index], drv_id, RPMH_SET_SLEEP);

  //disable RSC clocks
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_sleep_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_sleep_clks[i], FALSE);
  }

  RPMH_LOG_0 ("rpmh_enter_sleep_complete");

  rpmh_unlock (FALSE);
}

/**
 * <!-- rpmh_exit_sleep -->
 *
 * @brief API to configure RSCSW_DRV for exiting sleep. 
 *        Sends wake AMCs if needed, cleans up driver context sleep 
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep
 */
void rpmh_exit_sleep (RSCSW_DRV_MAPPING drv_id)
{
  rpmh_lock (FALSE);
  if(rpmh.is_standalone)
  {
    RPMH_LOG_0 ("rpmh_exit_sleep short circuit (STANDALONE MODE)");
    rpmh_unlock (FALSE);
    return;
  }


  //enable RSC clocks
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_sleep_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_sleep_clks[i], TRUE);
  }

  rpmh_core_verify (rpmh_is_drv_id_valid(drv_id));
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

#ifdef RPMH_SENSORS_ISLAND
  //for sensors we have to reconfigure one of the TCSs to AMC for active, and a TCS for sleep
  if(drv_id == RSC_DRV_VIRTUAL_SENSORS)
  {
    tcs_update_mode(drv_index, (uint32)SLPI_VIRTUAL_TCS_CONFIG_ACTIVE);
  }
#endif //RPMH_SENSORS_ISLAND


  RPMH_LOG_1 ("rpmh_exit_sleep, flushing wake AMCs (DRV_ID: 0x%08x)", drv_id);
  rpmh_cmdq_flush_sleep_wake_amcs (rpmh.rpmh_cmdq[drv_index], drv_id, RPMH_SET_WAKE);

  for(int i = 0; i < rpmh.num_resource_commands; i++)
  {
    rpmh_resource_command_t *rc = &rpmh.resource_commands[i];
    rpmh_resource_command_exit_sleep (rc, drv_id);
  }

  if(drv->in_irq_num)
  {
    //enable interrupt
    rpmh_register_isr (drv->in_irq_num, (rpmh_cb_fn)rpmh_finish_amc, (void *)drv->drv_id, RPMH_ISR_COMPLETION);
  }

  RPMH_LOG_0 ("rpmh_exit_sleep_complete");

  rpmh_unlock (FALSE);
}

/**
 * <!-- rpmh_get_wake_latency -->
 *
 * @brief Retrieve the wake latency for the given DRV ID. For now this is a constant value
 *        roughly equivalent to the worst case time for the wake TCS to complete
 *
 * @param drv_id : The DRV associated with the wake set
 *
 * @return Wake TCS latency in 19.2Mhz ticks
 */ 
uint32 rpmh_get_wake_latency(RSCSW_DRV_MAPPING drv_id)
{
  return DRV_CONFIG_DATA->drvs[lookup_drv_index (drv_id)].wake_set_latency;
}

#if 0
void rpmh_test_cb(uint32 req_id)
{
  RPMH_LOG_1 ("test_callback (REQ_ID: %d)\n", req_id);
}

void rpmh_test_task(void)
{
  RPMH_LOG_0 ("rpmh_test_task\n");
  rpmh_client_handle a = rpmh_create_handle(RSC_DRV_AOP, "TEST_CLIENTA");
  rpmh_client_register_completion_callback(a, (rpmh_completion_cb)rpmh_test_cb);

  uint32 a_req_id = rpmh_issue_command(a, RPMH_SET_ACTIVE, 0, 0xabcdabcd, 444);
  a_req_id = rpmh_issue_command(a, RPMH_SET_ACTIVE, 0, 0xabcdabcd, 555);
  rpmh_churn_all(a, a_req_id);
  RPMH_LOG_0 ("rpmh_test_task_complete\n");
}
#endif

void rpmh_epcb_timeout(void *unused)
{
  RPMH_LOG_0 ("EPCB TIMEOUT");
  rpmh_core_verify(0);
}

/**
 * <!-- rpmh_client_init -->
 *
 * @brief Call before using any other rpm API 
 */
void rpmh_client_init (void)
{
  if (rpmh.initialized == 1)
  {	    
    return;
  }	
  rpmh.initialized = 1;
  rpmh.is_standalone = RPMH_STANDALONE;

  rpmh_os_init (); //initialize locks, logging, etc.

  RPMH_LOG_0 ("rpmh_client_init");

  //turn on the necessary RSC clocks
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_init_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_init_clks[i], TRUE);
  }
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_sleep_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_sleep_clks[i], TRUE);
  }

  if(!rpmh.is_standalone)
  {
    tcs_init (); //initialize tcs/drv structures   
  }

  rpmh.rpmh_cmdq = (rpmh_cmdq_t **)rpmh_malloc ((sizeof(rpmh_cmdq_t *) * DRV_CONFIG_DATA->num_drvs));
  rpmh_core_verify_ptr(rpmh.rpmh_cmdq);

  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[i]; 
    if(drv->in_irq_num != 0)
    {
      rpmh_register_isr (drv->in_irq_num, (rpmh_cb_fn)rpmh_finish_amc, (void *)drv->drv_id, RPMH_ISR_COMPLETION);
    }
    if(drv->epcb_timeout_irq_num != 0)
    {
      rpmh_register_isr (drv->epcb_timeout_irq_num, (rpmh_cb_fn)rpmh_epcb_timeout, (void *)drv->drv_id, RPMH_ISR_EPCB_TIMEOUT);
    }
    rpmh.rpmh_cmdq[i] = rpmh_cmdq_create (i); //initialize the command queues per DRV
  }

  rpmh.rpmh_clientq = rpmh_clientq_create (); //initialize the client handle queues

  /* Inform sleep of the wake set latency.  When the latency is dynmically updated, this
   * will need to move to that point so sleep is always aware of the latest value. */
 // sleepActive_SetSignal(SLEEP_SIGNAL_WAKE_LATENCY_CHANGED);

  RPMH_LOG_0 ("rpmh_client_init_complete");
}

#ifdef RPMH_SENSORS_ISLAND
void rpmh_island_init(void)
{
  RPMH_LOG_0 ("rpmh_island_init");
  //TODO profile how long the latency actually takes
  uSleep_registerNotificationCallback(20, 20, (uSleep_notification_cb_type)rpmh_island_cb);
  RPMH_LOG_0 ("rpmh_island_init_complete");
}
#endif //RPMH_SENSORS_ISLAND

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- rpmh_offtarget_test_print_drvs -->
 *
 * @brief Prints DRV config and current TCS config for each supported DRV
 */
void rpmh_offtarget_test_print_drvs ()
{
  print_drvs();
}

/**
 * <!-- rpmh_offtarget_test_print_drv -->
 *
 * @brief Prints DRV config and current TCS config for the given drv
 *
 * @param drv_id : The RSCSW_DRV_MAPPING to print config
 */
void rpmh_offtarget_test_print_drv (RSCSW_DRV_MAPPING drv_id)
{
  print_drv (drv_id);
}

/**
 * <!-- rpmh_offtarget_test_print_clients -->
 *
 * @brief Prints current client list and associated client details
 */
void rpmh_offtarget_test_print_clients ()
{
  print_clients (rpmh.rpmh_clientq);
}

/**
 * <!-- rpmh_offtarget_test_print_resource_commands -->
 *
 * @brief Prints current resource command list and associated details
 */
void rpmh_offtarget_test_print_resource_commands ()
{
  print_resource_commands (rpmh.resource_commands, rpmh.num_resource_commands);
}

/**
 * <!-- rpmh_offtarget_test_print_cmdqs -->
 *
 * @brief Prints current cmd queues (queued, in flight, sleep/wake amcs)
 */
void rpmh_offtarget_test_print_cmdqs ()
{
  print_cmdqs (rpmh.rpmh_cmdq);
}

/**
 * <!-- rpmh_get_core_verify_failures -->
 * @brief Used to track the # of core_verify failures for offtarget testing.  
 *
 * @return The # of core_verify failures so far in this run 
 */
uint32 rpmh_get_core_verify_failures ()
{
  return get_core_verify_failures ();
}

#endif //RPMH_OFFTARGET_TEST

/**
 * <!-- rpmh_exit -->
 * @brief Clean all TCS cmds configuration while Exiting UEFI services.  
 *
 */

void rpmh_exit(void)
{	
	for( uint32 drv_index = 0; drv_index < DRV_CONFIG_DATA->num_drvs; drv_index++)
	{
		const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index];
		
		for(uint8 tcs_id = 0; tcs_id < drv->num_tcs; tcs_id++)
		{
            RscHalClearTcsControl(drv->drv_id, tcs_id);
            RscHalClearTcsCmdEnable(drv->drv_id, tcs_id);
		}
	}
	return;
}