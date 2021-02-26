/*===========================================================================
                              rpmh_cmdq.c

DESCRIPTION:
  Provides rpmh command queue tracking and related functionality. This includes
  sending commands, finishing commands, cleaning commands, and moving commands
  between queues when appropriate.

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
#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "rpmh_cmdq.h"
#include "rpmh_os.h"
#include "rpmh_resource_commands.h"
#include "rpmh_tcs.h"
#include "rpmh_client.h"
#include "rpmh_drv_config.h"

#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
#include "rpmh_drv_config.h"
#endif //RPMH_OFFTARGET_TEST


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

/*
 * cmd queue
 */
struct rpmh_cmdq_s
{
  unsigned num_cmds_per_tcs; //number of commands per TCS
  inq_t *in_flight;          //in flight queue
  inq_t *enqueued;           //enqueued queue
  inq_t *free_cmds;          //free cmds queue
  inq_t *sleep_amcs;         //sleep amcs queue
  inq_t *wake_amcs;          //wake amcs queue
};

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
 * <!-- new_cmd -->
 *
 * @brief Get a new command 
 *
 * @param self : The cmdq on which to retrieve a new command
 *
 * @return A pointer to the new command
 */ 
static rpmh_cmd_t *new_cmd (rpmh_cmdq_t *self)
{
  rpmh_cmd_t *cmd = (rpmh_cmd_t *)inq_pop (self->free_cmds);
  if (cmd == NULL)
  {
    cmd = rpmh_malloc (sizeof (rpmh_cmd_t));
    rpmh_core_verify_ptr (cmd);
    memset (cmd, 0, sizeof (rpmh_cmd_t));
  }

  //set up the resource command list as well
  if (cmd->details == NULL)
  {
    cmd->details = (rpmh_cmd_details_t *)rpmh_malloc (sizeof (rpmh_cmd_details_t) * self->num_cmds_per_tcs);	    
    rpmh_core_verify_ptr (cmd->details);
    memset (cmd->details, 0, sizeof (rpmh_cmd_details_t) * self->num_cmds_per_tcs);
  }	

  return cmd;
}

/**
 * <!-- rpmh_clean_cmd -->
 *
 * @brief Clean the given cmd 
 *
 * @param cmd      : The command to clean
 * @param num_cmds : The number of resource commands in the command
 */ 
void rpmh_clean_cmd (rpmh_cmd_t *cmd, uint32 num_cmds)
{
  memset(cmd->details, 0, sizeof(rpmh_cmd_details_t) * num_cmds);
  
  //cmd->num_rcs = 0;
  //cmd->completion_bmsk = 0;
  //cmd->dependency_bmsk = 0;
  //cmd->req_id = 0;
  //cmd->set = 0;
  //cmd->amc = FALSE;
  //cmd->client = NULL;
  //memset everything thats not a list
  uint32 num_rcs_offset = offsetof (rpmh_cmd_t, num_rcs);
  memset (((char*)cmd + num_rcs_offset), 0, (sizeof (rpmh_cmd_t) - num_rcs_offset));
}

/**
 * <!-- rpmh_delete_cmd -->
 *
 * @brief Delete the cmd from the queue 
 *
 * @param slef : The cmdq to delete the command from
 * @param cmd  : The command to delete
 */ 
static void delete_cmd(rpmh_cmdq_t *self, rpmh_cmd_t *cmd)
{
  rpmh_clean_cmd (cmd, self->num_cmds_per_tcs);	
  inq_append (self->free_cmds, cmd);
}

/**
 * <!-- rpmh_cmdq_create -->
 *
 * @brief Create a new command queue 
 *
 * @param drv_index : The index into the DRV config to create the queue
 *
 * @return A pointer to the cmdq 
 */ 
rpmh_cmdq_t *rpmh_cmdq_create (uint32 drv_index)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_cmdq_t *self = rpmh_malloc (sizeof(rpmh_cmdq_t));
  rpmh_core_verify_ptr (self);
  memset (self, 0, sizeof (rpmh_cmdq_t));

  self->num_cmds_per_tcs = drv->num_cmds;
  self->in_flight   = inq_create (offsetof (rpmh_cmd_t, link));
  self->enqueued    = inq_create (offsetof (rpmh_cmd_t, link));
  self->free_cmds   = inq_create (offsetof (rpmh_cmd_t, link));
  self->sleep_amcs  = inq_create (offsetof (rpmh_cmd_t, link));
  self->wake_amcs   = inq_create (offsetof (rpmh_cmd_t, link));

  return self;
}

/**
 * <!-- rpmh_cmdq_destroy -->
 *
 * @brief Delete the given cmdq 
 *
 * @param self : The cmdq to destroy
 */ 
void rpmh_cmdq_destroy (rpmh_cmdq_t *self)
{
  void *msg;

  while ((msg = inq_pop (self->in_flight)) != NULL)
  {
    rpmh_free (msg);
  }	
  inq_destroy (self->in_flight);

  while ((msg = inq_pop (self->enqueued)) != NULL) 
  {	    
    rpmh_free(msg);
  }	
  inq_destroy (self->enqueued);

  while ((msg = inq_pop (self->free_cmds)) != NULL) 
  {	    
    rpmh_free(msg);
  }	
  inq_destroy (self->free_cmds);

  while ((msg = inq_pop (self->free_cmds)) != NULL) 
  {
    rpmh_free (msg);
  } 
  inq_destroy(self->sleep_amcs);

  while ((msg = inq_pop (self->free_cmds)) != NULL) 
  {
    rpmh_free (msg);
  }	    
  inq_destroy (self->wake_amcs);

  rpmh_free (self);
}

/**
 * <!-- rpmh_cmdq_send_read -->
 *
 * @brief Send a new command 
 *
 * @param self    : The cmdq associated with the send
 * @param address : The address to read
 * @param client  : The client who initiated the command
 * @param queue   : V1 workaround - if there are commands in flight we queue the read instead of 
 *                  sending it right away
 *
 * @return The command id for the request 
 */ 
uint32 rpmh_cmdq_send_read (rpmh_cmdq_t *self, uint32 address, rpmh_client_t *client, boolean queue)
{
  //get new command. allocate # of rcs possible for the given DRV
  rpmh_cmd_t *cmd = new_cmd (self);

  cmd->num_rcs = 1;
  cmd->dependency_bmsk = 0;
  cmd->details[0].address = address; 
  cmd->details[0].type = ReadRequest; 
  cmd->client = client;
  cmd->req_id = client->req_id;
  cmd->set = RPMH_SET_ACTIVE;
  cmd->amc = TRUE;

  if (!queue) 
  {
    if (tcs_send(cmd, client->drv_id))
    {
      //command sent    
      inq_append (self->in_flight, cmd);
      return cmd->req_id;
    }
    RPMH_LOG_0("\trpmh_holding_read_request (NO FREE AMCS)");
  }
  else
  {
    RPMH_LOG_0("\trpmh_holding_read_request (PDC READ LIMIT WORKAROUND)");
  }

  inq_append (self->enqueued, cmd);
  return cmd->req_id;
}

/**
 * <!-- get_queue -->
 *
 * @brief Helper function to retrieve the associated queue for the given set 
 *
 * @param self : The cmdq on which to retrieve the queue
 * @param set  : The set to indicate which queue to retrieve
 *
 * @return A queue pointer to the corresponding queue 
 */ 
static inline inq_t *get_queue(rpmh_cmdq_t *self, rpmh_set_enum set)
{
  inq_t *queue = NULL;
  switch(set)
  {
    case RPMH_SET_ACTIVE:
      queue = self->in_flight;
      break;
    case RPMH_SET_SLEEP:
      queue = self->sleep_amcs;
      break;
    case RPMH_SET_WAKE:
      queue = self->wake_amcs;
      break;
    default:
      rpmh_core_verify(FALSE);
      break;
  }	    
  return queue;
}

/**
 * <!-- rpmh_cmdq_flush_sleep_wake_amcs -->
 *
 * @brief Send all of the sleep/wake AMCs. Should be called on the way in/out of sleep 
 *
 * @param self   : The cmdq from which to send the sleep/wake AMCs
 * @param drv_id : The RSC+DRV mapping on which to flush
 * @param set    : The set to indicate which to send (sleep or wake)
 */ 
void rpmh_cmdq_flush_sleep_wake_amcs (rpmh_cmdq_t *self, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
  inq_iterator_t it;
  rpmh_cmd_t *cmd;

  inq_it_begin(get_queue(self, set), &it);

  //loop through the AMCs and send them all 
  while ((cmd = (rpmh_cmd_t *)inq_it_deref (&it)) != NULL)
  {
    //send the command. If there isn't room, churn a command		
    while (!tcs_send (cmd, drv_id))
    {
      if (is_amc_finished(drv_id))
      { 
        rpmh_finish_sleep_wake_amc (drv_id, set);    
      }
    }
    inq_it_next (&it);
  }

  inq_t *queue = get_queue(self, set);
  //churning all commands on sleep enter and exit. Churn on enter because there can't be outstanding
  //AMCs when we enter sleep.  Churn on exit to guarantee state when other software starts
  while (inq_head (queue))
  {
    if (is_amc_finished (drv_id))
    {
      rpmh_finish_sleep_wake_amc (drv_id, set);
    }	
  }
}

/**
 * <!-- set_new_req -->
 *
 * @brief Send a new command  set
 *
 * @param self        : The cmdq associated with the send
 * @param client      : The client who initiated the command set
 * @param command_set : The command set with all the request details
 *
 * @return The command id for the request 
 */ 
static rpmh_cmd_t *set_new_req(rpmh_cmdq_t *self, rpmh_client_t *client, 
                               rpmh_command_set_internal_t *command_set, boolean *rc_idle)
{
  *rc_idle = TRUE;
  //get new command. allocate # of rcs possible for the given DRV
  rpmh_cmd_t *cmd = new_cmd (self);

  cmd->num_rcs = command_set->num_commands;
  //this can also be called from enter/exit sleep (with no client), so check for client validity
  cmd->client = client;
  cmd->req_id = (client != NULL) ? client->req_id : 0;

  cmd->dependency_bmsk = command_set->dependency_bmsk;
  cmd->set = command_set->set;
  cmd->amc = TRUE;

  //TODO
  //we already have this check
  rpmh_core_verify (command_set->num_commands <= (uint32)16);
  for (int i = 0; i < cmd->num_rcs; i++)
  {
    rpmh_command_t command = command_set->commands[i];
    rpmh_resource_command_t *rc = rpmh_find_resource_command(command.address);    
    rpmh_core_verify_ptr (rc);
    cmd->details[i].type = command.completion ? WriteCompletion : WriteFirenForget; 
    cmd->details[i].address = command.address;

    //if this is a client vote, check if the resource is idle
    if(client)
    {	    
      drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, client->drv_id);
      rpmh_core_verify_ptr(curr_drv);

      if (curr_drv->state != RPMH_RC_STATE_IDLE)
      {
        *rc_idle = FALSE;
      }
    }      
  }
  return cmd;
}

/**
 * <!-- rpmh_cmdq_send_cmd_set -->
 *
 * @brief Send a new command  set
 *
 * @param self        : The cmdq associated with the send
 * @param client      : The client who initiated the command set
 * @param command_set : The command set with all the request details
 *
 * @return The command id for the request 
 */ 
uint32 rpmh_cmdq_send_cmd_set(rpmh_cmdq_t *self, rpmh_client_t *client, 
                              rpmh_command_set_internal_t *command_set)
{
  uint32 ret_req_id = 0;
  boolean rc_idle = TRUE;	
  inq_t *queue;    
  rpmh_cmd_t *cmd = set_new_req(self, client, command_set, &rc_idle);

  switch (command_set->set)
  {
    //sleep/wake AMC  
    case RPMH_SET_SLEEP:
    case RPMH_SET_WAKE:
      queue = get_queue(self, cmd->set);    
      inq_append(queue, cmd);
      break;
    //standard active request	  
    case RPMH_SET_ACTIVE:
      rpmh_core_verify_ptr(client);      
      ret_req_id = cmd->req_id;
      //make sure the resources are idle
      if ((rc_idle == TRUE) && is_amc_free(client->drv_id))
      {    
        //request sent    
        inq_append (self->in_flight, cmd);
        tcs_send(cmd, client->drv_id);
      }
      else
      {
        //resource wasn't idle or there weren't any free AMCs
        RPMH_LOG_1("\trpmh_holding_request (RCS_IDLE?: %d)", rc_idle);
        inq_append (self->enqueued, cmd);
      }
      break;
    default:
      rpmh_core_verify(FALSE);      
      break;
  }
  return ret_req_id;
}

/**
 * <!-- rpmh_cmdq_send_cmd_at_head -->
 *
 * @brief Send a new command from the head of the enqueued queue
 *
 * @param self : The cmdq associated with the send
 *
 * @return TRUE if sent, false if not because the resource was busy or there were no AMCs
 */ 
boolean rpmh_cmdq_send_cmd_at_head (rpmh_cmdq_t *self)
{
  int i;	
  boolean idle = TRUE;
  inq_iterator_t it;
  rpmh_cmd_t *cmd;

  inq_it_begin (self->enqueued, &it);
  //check the head of the queue for a request and try to send if so. 
  //  If there is one, make sure the resource is idle. 
  //  If it's not, leave it at the head of the queue
  if ((cmd = (rpmh_cmd_t *)inq_it_deref (&it)) != NULL)
  {
    for (i = 0; i < cmd->num_rcs; i++)
    {
      rpmh_resource_command_t *rc = rpmh_find_resource_command (cmd->details[i].address);     
        rpmh_core_verify_ptr (rc);

      if(!cmdq_empty(self))
      {	    
        drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, cmd->client->drv_id);
        rpmh_core_verify_ptr (curr_drv);
        //if the resource is idle OR [V1 PDC Workaround] its a read and there are commands in flight
        if ((curr_drv->state != RPMH_RC_STATE_IDLE) || 
            ((cmd->details[0].type == ReadRequest) && (cmdq_num_in_flight(self) > 0)))
        {
          idle = FALSE;
        }
      }
    }
    if(idle == TRUE)
    {
      rpmh_core_verify (cmd->num_rcs <= (uint32)16);
      //resource is idle, try to send the request
      if (tcs_send (cmd, cmd->client->drv_id) == TRUE)
      {
        //command sent, delete from enqueued and add to inflight   
        inq_it_delete (&it);	
        inq_append (self->in_flight, cmd);
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * <!-- pull_from_queue -->
 *
 * @brief Remove the given cmd from the given queue if it exists 
 *
 * @param q           : The queue to remove from
 * @param looking_for : The cmd we are looking for
 * @param cmd         : A pointer to fill in with the command if found
 */ 
static void pull_from_queue (inq_t *q, rpmh_cmd_t *looking_for, rpmh_cmd_t **cmd)
{
  inq_iterator_t it;

  inq_it_begin (q, &it);

  while ((*cmd = (rpmh_cmd_t *)inq_it_deref (&it)) != NULL)
  {
    if (looking_for == *cmd)
    {
        inq_it_delete (&it);
        return;
    }
    inq_it_next (&it);
  }

  // We shouldn't ever make it this far; if the command isn't in the queue,
  // something has gone horribly wrong.
  rpmh_err_fatal ("Inconsistent cmdq state.");
  while (1);
}

/**
 * <!-- cmdq_finish_cmd -->
 *
 * @brief A command has completed, finish at the queue level 
 *
 * @param self   : The cmdq associated with the command
 * @param cmd    : The command to finish
 * @param drv_id : The RSC+DRV mapping that we are finishing the command for
 * @param tcs_id : The TCS ID that the command finished on. 
 *                 This is a passthrough for the TCS layer to retrieve read data for read commands
 * @param set    : The set associated with the command completion
 */ 
void cmdq_finish_cmd (rpmh_cmdq_t *self, rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id, uint8 tcs_id, rpmh_set_enum set)
{
  int i;	
  rpmh_cmd_t *completed;

  //remove the cmd from the queue
  pull_from_queue (get_queue (self, set), cmd, &completed);

  for (i = 0; i < completed->num_rcs; i++)
  {
    if (completed->details[i].type == ReadRequest)
    {
      //save the read data to the client	    
      rpmh_client_save_read_data (cmd->client, cmd->req_id, tcs_get_read_data (drv_id, tcs_id, i));
    }
    else
    {	    
      //finish at the resource level
      rpmh_resource_command_finish (completed->details[i].address, drv_id);
    }      
  }

  //finally delete the command
  delete_cmd (self, completed);
}


/**
 * <!-- cmdq_empty -->
 *
 * @brief Checks if the corresponding cmdq is empty or not 
 *
 * @param self   : cmdq on which we check if any commands are pending
 *
 * @return TRUE if command is empty, FALSE if any commands are in the cmdq
 */ 
boolean cmdq_empty(rpmh_cmdq_t *self)
{
    return((inq_head (self->enqueued) == NULL) && (inq_head (self->in_flight) == NULL) &&
           (inq_head (self->sleep_amcs) == NULL) && (inq_head (self->wake_amcs) == NULL));
}

/**
 * <!-- cmdq_num_in_flight -->
 *
 * @brief Checks how many requests are in flight 
 *
 * @param self : cmdq on which we check if any commands are pending
 *
 * @return number of commands in flight
 */ 
uint32 cmdq_num_in_flight(rpmh_cmdq_t *self)
{
    return inq_size(self->in_flight);
}

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_cmd_list -->
 *
 * @brief Prints a list of commands
 *
 * @param cmds : list of commands to print
 */
void print_cmd_list (rpmh_cmd_t *cmds)
{
  int i;	
  if (cmds == NULL)
  {
    printf ("\t\tNO CMD\n");
  }
  else
  {
    printf ("\t\tREQ_ID: %d\n", cmds->req_id);
    if (cmds->client)
    {
      printf ("\t\tCLIENT_NAME: %s\n", cmds->client->name);
    }    
    printf ("\t\tNUM_RESOURCE_COMMANDS: %d\n", cmds->num_rcs);
    for (i = 0; i < cmds->num_rcs; i++)
    {
      printf ("\t\t\tCMD #: %d\n", i);
      printf ("\t\t\tCMD ADDRESS: 0x%08x\n", cmds->details[i].address);
      printf ("\t\t\tCMD TYPE: 0x%08x\n", cmds->details[i].type);
      //print_resource_votes(*rc);
    }
  }	
}

/**
 * <!-- print_cmdq -->
 *
 * @brief Prints a single cmdq (queued, in flight, sleep/wake amcs)
 *
 * @param cmdq : cmdq to print
 */
static void print_cmdq(inq_t *cmdq)
{
  inq_iterator_t it;
  rpmh_cmd_t *cmd;

  inq_it_begin(cmdq, &it);

  while ((cmd = (rpmh_cmd_t *)inq_it_deref (&it)) != NULL)
  {
    printf ("\tCMDS:\n");
    print_cmd_list (cmd);
    //TODO print cmds_in_progress
    inq_it_next (&it);
  }
}

/**
 * <!-- print_cmdqs -->
 *
 * @brief Prints current cmd queues (queued, in flight, sleep/wake amcs)
 *
 * @param self : list of cmdqs to print
 */
void print_cmdqs(rpmh_cmdq_t **self)
{
  int i;	
  for (i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    printf ("DRV: %d CMD_QUEUED_LIST_START\n", i);
    print_cmdq (self[i]->enqueued);
    printf ("\tCMD_QUEUED_LIST_END\n");

    printf ("\tCMD_IN_FLIGHT_LIST_START\n");
    print_cmdq (self[i]->in_flight);
    printf ("\tCMD_IN_FLIGHT_LIST_END\n");

    printf ("\tCMD_SLEEP_AMCS_LIST_START\n");
    print_cmdq (self[i]->sleep_amcs);
    printf ("\tCMD_SLEEP_AMCS_LIST_END\n");

    printf ("\tCMD_WAKE_AMCS_LIST_START\n");
    print_cmdq (self[i]->wake_amcs);
    printf ("\tCMD_WAKE_AMCS_LIST_END\n");

  }

}

#endif //RPMH_OFFTARGET_TEST

