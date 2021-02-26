/*===========================================================================
                              rpmh_client.c

DESCRIPTION: 
  Main client facing functionality. Initialization, sending 
  commands, sending command sets, completing commands/comand sets, invalidating
  commands, entering/exiting sleep, checking if a drv is valid. 

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
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
#include "CoreVerify.h"
#include "drv_lookup.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

//---------------------------------------------------------------------------
// AOP Direct Send Support
//--------------------------------------------------------------------------
#ifdef RPMH_AOP_DIRECT_SEND
#include "HALhwio.h"
#include "arc_vt_hwio.h"
#include "bcm_vt_hwio.h"
#include "vrm_vt_hwio.h"

#define DRV_MULTIPLIER (HWIO_RPMH_ARC_DRVd_RMm_VOTE_ADDR(1, 0) - HWIO_RPMH_ARC_DRVd_RMm_VOTE_ADDR(0, 0))
#define NUM_SLAVES 8
uint32 slv_id_tbl[NUM_SLAVES] = {0, 0, 0, RPMH_ARC_ARC_VT_REG_BASE, RPMH_VRM_VRM_VOTETABLE_REG_BASE, RPMH_BCM_BCM_VOTETABLE_REG_BASE, 0, 0};
#endif //RPMH_AOP_DIRECT_SEND

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
#define RPMH_PREALLOC_RESOURCES 40
#define RC_TABLE_INCREMENT 5
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
  uint32                    rsc_clocks_initialized; //whether or not the rsc_clocks have been requested
  uint32                    num_resource_commands; //the # of unique resource commands
  uint32                    rc_table_size; //the # of unique resource commands
  rpmh_resource_command_t   *resource_commands;    //the resource command table
  //uint32                    num_resource_commands_dirty[RPMH_NUM_SETS]; //number of RCs dirty
  rpmh_cmdq_t               **rpmh_cmdq;           //command queue
  rpmh_clientq_t            *rpmh_clientq;         //client queue
  boolean                   is_standalone;         //used for pre-sil/bringup
  uint32                    aop_global_drv_id;     //global DRV ID for AOP
  uint32                    num_dir_sends;
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
 * <!-- rpmh_create_handle -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands
 *
 * @param drv_id      : The RSC+DRV mapping for the client
 * @param client_name : A unique string to identify the client
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle (RSCSW_DRV_MAPPING drv_id, const char *client_name)
{
  rpmh_lock (FALSE);	
  rpmh_core_verify (rpmh_is_drv_id_valid (drv_id));

  //RPMH_LOG_2 (rpmh_log_create_handle, client_name, drv_id);
  rpmh_client_t *client = rpmh_client_handle_add (rpmh.rpmh_clientq);
  client->name = client_name;
  client->drv_id = drv_id;
  client->req_id = 0;
  rpmh_unlock (FALSE);	
  return (rpmh_client_handle)client;
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

  //RPMH_LOG_2 (rpmh_log_delete_handle, client->name, client->drv_id);
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

  if(rpmh.num_resource_commands > rpmh.rc_table_size)
  {
    rpmh.rc_table_size+=RC_TABLE_INCREMENT;	  
    rpmh.resource_commands = (rpmh_resource_command_t*)rpmh_realloc (rpmh.resource_commands, 
                                    rpmh.rc_table_size * sizeof(rpmh_resource_command_t));
  }

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

  //RPMH_LOG_3 (rpmh_log_churn_all, client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_ALL, TRUE);	
  //RPMH_LOG_0 (rpmh_log_churn_all_complete);
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

  //RPMH_LOG_3 (rpmh_log_churn_single, client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_SINGLE, TRUE);	
  //RPMH_LOG_0 (rpmh_log_churn_single_complete);
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
  //RPMH_LOG_3 (rpmh_log_barrier_all, client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_ALL, FALSE);	
  //RPMH_LOG_3 (rpmh_log_barrier_all_complete, client->name, client->drv_id, req_id);
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
  //RPMH_LOG_3 (rpmh_log_barrier_single, client->name, client->drv_id, req_id);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_BARRIER_SINGLE, FALSE);	
  //RPMH_LOG_3 (rpmh_log_barrier_single_complete, client->name, client->drv_id, req_id);
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
  //RPMH_LOG_3 (rpmh_log_notify_completion_all, client->name, client->drv_id, req_id);
  rpmh_core_verify_ptr(client->completion_cb);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_CALLBACK_ALL, FALSE);	
  //RPMH_LOG_3 (rpmh_log_notify_completion_all_complete, client->name, client->drv_id, req_id);
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
  //RPMH_LOG_3 (rpmh_log_notify_completion_single, client->name, client->drv_id, req_id);
  rpmh_core_verify_ptr(client->completion_cb);
  rpmh_wait_for_cmd (client_handle, req_id, RPMH_CALLBACK_SINGLE, FALSE);	
  //RPMH_LOG_3 (rpmh_log_notify_completion_single_complete, client->name, client->drv_id, req_id);
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

/**
 * <!-- rpmh_cmdqs_empty -->
 *
 * @brief Check if the cmdqs for all DRVs are empty
 *
 * @return Whether or not the cmdqs for all DRVs are empty
 */ 
boolean rpmh_cmdqs_empty(void)
{
  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    if(!cmdq_empty(rpmh.rpmh_cmdq[i]))
    {
      return FALSE;
    }
  }
  return TRUE;
}

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
  uint32 tcs_index = 0;
  boolean send_at_head = TRUE;
  rpmh_client_t *finished_client = NULL;
  rpmh_lock (TRUE);
  rpmh_enter_isr();

  RSCSW_DRV_MAPPING drv_id = get_finished_tcs(hw_drv, lookup_drv_index(hw_drv), &tcs_index);
  if (drv_id != RSC_DRV_VIRTUAL_MAX)
  {
    //RPMH_LOG_2 (rpmh_log_finish_amc, hw_drv, drv_id);
    rpmh_core_verify (rpmh_is_drv_id_valid(drv_id));

    uint32 drv_index = lookup_drv_index (drv_id);

    //clean up at the TCS level. This also cleans up at the resource and queue level	
    req_id = tcs_finish_active_amc (&finished_client, rpmh.rpmh_cmdq[drv_index], drv_id, drv_index, tcs_index); 

    //when we retrieved the DRV we checked there was an outstanding AMC, so req_id must be non-zero
    rpmh_core_verify (req_id > 0);

    //RPMH_LOG_2 (rpmh_log_finished_req, finished_client->name, req_id);
    //clean up at the client level
    rpmh_client_command_finish (finished_client, req_id);

    while(send_at_head)
    {
      //send a queued command
      send_at_head = rpmh_cmdq_send_cmd_at_head (rpmh.rpmh_cmdq[drv_index], drv_index);
    }
  }
  else
  {
    //RPMH_LOG_0 (rpmh_log_no_amc_completed);
  }

  //RPMH_LOG_0 (rpmh_log_finish_amc_complete);

  rpmh_exit_isr();
  rpmh_unlock (TRUE);
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

#ifdef RPMH_AOP_DIRECT_SEND
/**
 * <!-- rpmh_direct_send -->
 *
 * @brief Update the vote table for the given resource and DRV directly 
 *
 * @param address : The address of the resource on which to vote
 * @param drv_id  : The DRV the vote is coming from
 */ 
static void rpmh_direct_send(uint32 address, RSCSW_DRV_MAPPING drv_id)
{
  uint16 slv_id = SLAVE_ID(address);
  uint16 offset = OFFSET(address);

  //get the data to send
  uint32 data = rpmh_resource_command_val_at_rpmh (address, drv_id, RPMH_SET_ACTIVE, FALSE);

  //get base
  CORE_VERIFY(slv_id < NUM_SLAVES);
  uint32 base = slv_id_tbl[slv_id];
  rpmh_core_verify(base != 0);

  //calculate the address. Base is the accelerator VT base. Add the DRV offset and the resource offset
  uint32 addr = base + (rpmh.aop_global_drv_id * DRV_MULTIPLIER) + offset;

  //write to the vote table
  out_dword(addr, data);
}

/**
 * <!-- direct_send_supported -->
 *
 * @brief Check if direct send is available for the given address 
 *
 * @param address : The address of the resource on which to check availability of direct send
 *
 * @return TRUE if direct send is available, false if not
 */ 
static boolean direct_send_supported(uint32 address)
{	
  uint32 slv_id = SLAVE_ID(address);
  CORE_VERIFY(slv_id < NUM_SLAVES);
  return (slv_id_tbl[slv_id] != 0);
}
#endif //RPMH_AOP_DIRECT_SEND

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
  uint32 rcs_dirty = 0;

#ifdef RPMH_AOP_DIRECT_SEND
  uint32 rcs_direct_send = 0;
#endif //RPMH_AOP_DIRECT_SEND
  rpmh_client_t *handle = (rpmh_client_t *)client_handle;
  uint32 drv_index = lookup_drv_index (handle->drv_id);
  //RPMH_LOG_4 (rpmh_log_issue_command_set, 
  //             handle->name, handle->drv_id, cmd_set->set, cmd_set->num_commands);
  //cast to internal command set type to hide dependency bmsk for now
  rpmh_command_set_internal_t *command_set = (rpmh_command_set_internal_t*)cmd_set;

  rpmh_core_verify(command_set->set == RPMH_SET_ACTIVE);

  //remove this if we support dependencies at some point
  command_set->dependency_bmsk = 0;

  rpmh_core_verify (command_set->num_commands <= 
                    DRV_CONFIG_DATA->drvs[drv_index].num_cmds);

  //update the command set resources
  for (int i = 0; i < command_set->num_commands; i++)
  {
    boolean rc_dirty = FALSE;	  
    rpmh_command_t command = command_set->commands[i];	    
    rpmh_resource_command_t *rc = get_rc (command.address);
    //RPMH_LOG_3 (rpmh_log_resource_command, command.address, command.data, command.completion);

    //update the resource
    rc_dirty = rpmh_resource_command_update (rc, command_set->set, command.data, command.completion, handle->drv_id);
    if(rc_dirty)
    {
      rcs_dirty++;
    }

#ifdef RPMH_AOP_DIRECT_SEND
    //if this resource is dirty, and its FF, and the resource isn't idle, write directly to the accelerator
    if((rc_dirty) && (command.completion == 0) && (rpmh_resource_command_is_idle(rc, handle->drv_id)) && (direct_send_supported(command.address)))
    {
      rcs_direct_send++;    
    }
#endif //RPMH_AOP_DIRECT_SEND
  }

  if(!rcs_dirty)
  {
    //RPMH_LOG_0 (rpmh_log_redundant_commands);
  }
  else if(rpmh.is_standalone)
  {
    //RPMH_LOG_0 (rpmh_log_standalone_mode);
  }
#ifdef RPMH_AOP_DIRECT_SEND
  else if(rcs_direct_send == rcs_dirty)
  {
    //RPMH_LOG_0(rpmh_direct_send_log);  
    rpmh.num_dir_sends++;
    for (int i = 0; i < command_set->num_commands; i++)
    {
      rpmh_command_t command = command_set->commands[i];	    
      rpmh_resource_command_t *rc = get_rc (command.address);

      //direct send the resources
      rpmh_direct_send(command.address, handle->drv_id);
      rpmh_resource_command_finish (command.address, handle->drv_id);
    }
  }
#endif //RPMH_AOP_DIRECT_SEND
  else
  {
      //make sure no TCSs are "stuck"
      //rpmh_core_verify (!is_tcs_stuck (handle->drv_id));

    //configure client for new request
    rpmh_client_command_add (handle); 

    CORE_VERIFY(command_set->num_commands <= IMAGE_TCS_SIZE);
    //send the request
      req_id = rpmh_cmdq_send_cmd_set (rpmh.rpmh_cmdq[drv_index], handle,
                                         command_set, handle->drv_id, drv_index);
  }

  //RPMH_LOG_1 (rpmh_log_issue_command_set_complete, req_id);
  rpmh_unlock (FALSE);
  return req_id;
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

  RPMH_LOG_3 (rpmh_log_issue_read_command, client->name, address, polling);

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
    req_id = rpmh_cmdq_send_read (rpmh.rpmh_cmdq[drv_index], address, client, v1_workaround_queue);
    data = rpmh_wait_for_read(client, req_id, polling);
    RPMH_LOG_3 (rpmh_log_issue_read_command_complete, client->name, address, data);
  }
  else
  {
      RPMH_LOG_0 (rpmh_log_standalone_mode");
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

//Not needed on AOP for now - keep around in case we need it later
//static void set_g_cmd_set(rpmh_command_set_internal_t *cmd_set)
//{
//  //memscpy(&g_cmd_set, sizeof(rpmh_command_set_internal_t, cmd_set sizeof(rpmh_command_set_internal_t));
//
//  memcpy(&g_cmd_set, cmd_set, sizeof(rpmh_command_set_internal_t));
//}

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

  uint32 a_req_id = rpmh_issue_command(a, RPMH_SET_ACTIVE, 1, 0x41000, 0x710);
  a_req_id = rpmh_issue_command(a, RPMH_SET_ACTIVE, 1, 0x41200, 0x708);
  rpmh_churn_all(a, a_req_id);
  RPMH_LOG_0 ("rpmh_test_task_complete\n");
}
#endif

void rpmh_epcb_timeout(void *unused)
{
  RPMH_LOG_0 (rpmh_log_epcb_timeout);
  rpmh_core_verify(0);
}

void rpmh_rsc_clocks_init(void)
{
  if (rpmh.rsc_clocks_initialized == 1)
  {     
    return;
  } 

  rpmh.rsc_clocks_initialized = 1;

  rpmh_os_init (); //initialize locks, logging, etc.

  //RPMH_LOG_0 (rpmh_log_rsc_clocks_init);

  //turn on the necessary RSC clocks
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_init_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_init_clks[i], TRUE);
  }
  for (int i = 0; i < DRV_CONFIG_DATA->num_rsc_sleep_clks; i++)
  {
    rpmh_clock_toggle (DRV_CONFIG_DATA->rsc_sleep_clks[i], TRUE);
  }

  //RPMH_LOG_0 (rpmh_log_rsc_clocks_init_complete);
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

  if (rpmh.rsc_clocks_initialized == 0)
  {     
    rpmh_rsc_clocks_init();
  } 

  rpmh.initialized = 1;
  rpmh.is_standalone = RPMH_STANDALONE;

  //RPMH_LOG_0 (rpmh_log_client_init);

  rpmh.rpmh_cmdq = (rpmh_cmdq_t **)rpmh_malloc ((sizeof(rpmh_cmdq_t *) * DRV_CONFIG_DATA->num_drvs));

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

  if(!rpmh.is_standalone)
  {
    tcs_init (rpmh.rpmh_cmdq); //initialize tcs/drv structures   
  }

  rpmh.rpmh_clientq = rpmh_clientq_create (); //initialize the client handle queues

  rpmh.rc_table_size = RPMH_PREALLOC_RESOURCES;
  rpmh.resource_commands = (rpmh_resource_command_t*)rpmh_malloc (rpmh.rc_table_size * sizeof(rpmh_resource_command_t));

  rpmh_core_verify(get_version_drv_id(RSC_DRV_GLOBAL_AOP, &(rpmh.aop_global_drv_id)) == DRV_ID_LOOKUP_SUCCESS);

  //RPMH_LOG_0 (rpmh_log_client_init_complete);
}

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

