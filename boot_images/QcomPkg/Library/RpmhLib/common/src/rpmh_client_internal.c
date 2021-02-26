/*===========================================================================
                              rpmh_client_internal.c

DESCRIPTION:
  Provides client handle related functionality and event funcitonality
  for command completion. The main structure here is a queue of client handles.
  Each client tracks the commands in progress and the events (barriers) 
  currently outstanding

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
#include "comdef.h"
#include "rpmh_resource_commands.h"
#include "rpmh_client_internal.h"
#include "rpmh_os.h"
#include "rpmh_tcs.h"
#include "rpmh_drv_config.h"

#include <stddef.h>
#include <string.h>


#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
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
 * <!-- get_cmd_in_progress -->
 *
 * @brief Find if a given req_id is in progress for the given handle 
 *
 * @param handle : The handle associated with the command ID
 * @param cmd    : A cmd pointer to fill in with the cmd if found
 * @param req_id : The cmd id to check for
 *
 * @return whether or not the given req_id is in progres
 */ 
static boolean get_cmd_in_progress(rpmh_client_t *handle, cmds_in_progress_t **cmd, uint32 req_id)
{
  inq_iterator_t it;
  boolean req_id_found = FALSE;

  inq_it_begin (handle->cmds_in_progress, &it);

  //check if the command is in the queue
  while ((*cmd = (cmds_in_progress_t *)inq_it_deref (&it)) != NULL)
  {
    if ((*cmd)->req_id == req_id)
    {
      req_id_found = TRUE;		
      break;
    }
    else
    {
      inq_it_next (&it);
    }
  }
  return req_id_found;
}

/**
 * <!-- pull_from_client_queue -->
 *
 * @brief Remove the given handle from the given queue if it exists 
 *
 * @param q      : The queue to remove from
 * @param handle : The handle to remove
 */ 
static void pull_from_client_queue (inq_t *q, rpmh_client_t *handle)
{
  inq_iterator_t it;
  rpmh_client_t *client;

  inq_it_begin (q, &it);

  while ((client = (rpmh_client_t *)inq_it_deref (&it)) != NULL)
  {
    if (client == handle)
    {
      inq_it_delete (&it);
      return;    
    }
    inq_it_next (&it);
  }

  // We shouldn't ever make it this far; if the command isn't in the queue,
  // something has gone horribly wrong.
  rpmh_err_fatal ("Inconsistent clientq state.");
  while(1);
}

/**
 * <!-- pull_from_cmds_in_progress_queue -->
 *
 * @brief Remove the given cmd from the given queue if it exists 
 *
 * @param q      : The queue to remove from
 * @param handle : The req_id to remove
 * @param cmd    : A cmd pointer to fill in with the cmd if found
 */ 
static void pull_from_cmds_in_progress_queue (inq_t *q, uint32 req_id, cmds_in_progress_t **cmd)
{
  inq_iterator_t it;

  inq_it_begin(q, &it);

  while ((*cmd = (cmds_in_progress_t *)inq_it_deref (&it)) != NULL)
  {
    if ((*cmd)->req_id == req_id)
    {
      inq_it_delete (&it);
      return;    
    }
    inq_it_next (&it);
  }

  // We shouldn't ever make it this far; if the command isn't in the queue,
  // something has gone horribly wrong.
  rpmh_err_fatal ("Inconsistent client cmds_in_progress state.");
  while (1);
}

/**
 * <!-- rpmh_clientq_create -->
 *
 * @brief Create a client queue 
 *
 * @return A pointer to the client queue
 */ 
rpmh_clientq_t *rpmh_clientq_create (void)
{
  rpmh_clientq_t *self = (rpmh_clientq_t *)rpmh_malloc (sizeof (rpmh_clientq_t));
  rpmh_core_verify_ptr (self);
  memset (self, 0, sizeof(rpmh_clientq_t));

  self->handles      = inq_create (offsetof (rpmh_client_t, link));
  self->free_handles = inq_create (offsetof (rpmh_client_t, link));

  return self;
}

/**
 * <!-- rpmh_client_handle_add -->
 *
 * @brief Add a client handle to the given queue 
 *
 * @param self : The queue to add to
 *
 * @return A pointer to the new client handle
 */ 
rpmh_client_t *rpmh_client_handle_add (rpmh_clientq_t *self)
{
  rpmh_client_t *handle = (rpmh_client_t *)inq_pop (self->free_handles);
  if (handle != NULL)
  {
    inq_append (self->handles, handle);
    return handle;
  }

  handle = (rpmh_client_t *)rpmh_malloc (sizeof (rpmh_client_t));
  rpmh_core_verify_ptr (handle);
  memset (handle, 0, sizeof (rpmh_client_t));
  handle->oldest_req_id = CMD_EMPTY;
  inq_append (self->handles, handle);

  handle->barriers              = inq_create (offsetof (active_events_t, link));
  handle->free_barriers         = inq_create (offsetof (active_events_t, link));
  handle->cmds_in_progress      = inq_create (offsetof (cmds_in_progress_t, link));
  handle->free_cmds_in_progress = inq_create (offsetof (cmds_in_progress_t, link));

  return handle;
}

/**
 * <!-- rpmh_client_handle_remove -->
 *
 * @brief Remove the client handle from the given queue 
 *
 * @param self   : The queue to remove from
 * @param handle : The handle to remove
 *
 * @return A pointer to the new client handle
 */ 
void rpmh_client_handle_remove (rpmh_clientq_t *self, rpmh_client_t *handle)
{
  pull_from_client_queue (self->handles, handle);
  //manually set values - some aren't 0 and this way we keep 
  //the qs (cmds_in_progress and events) for the next client
  handle->oldest_req_id        = CMD_EMPTY;
  handle->req_id               = CMD_EMPTY;
  handle->num_reqs_in_progress = 0;
  handle->name = "";

  inq_append (self->free_handles, handle);
}

/**
 * <!-- rpmh_client_command_add -->
 *
 * @brief Add a new command in progres to the given handle 
 *
 * @param handle : The handle on which to add a new command
 */ 
void rpmh_client_command_add (rpmh_client_t *handle)
{
  cmds_in_progress_t *cmd_in_progress = inq_pop (handle->free_cmds_in_progress);	
  if (cmd_in_progress == NULL)
  {
    cmd_in_progress = (cmds_in_progress_t *)rpmh_malloc (sizeof (cmds_in_progress_t));
    rpmh_core_verify_ptr (cmd_in_progress);
    memset (cmd_in_progress, 0, sizeof (cmds_in_progress_t));
  }

  handle->req_id += 1;
  cmd_in_progress->req_id = handle->req_id;	    
  inq_append (handle->cmds_in_progress, cmd_in_progress);
  handle->num_reqs_in_progress += 1;
  handle->oldest_req_id = MIN (handle->oldest_req_id, handle->req_id);
}

/**
 * <!-- rpmh_check_for_events -->
 *
 * @brief check for any completion events as a result of finishing the given req_id
 *
 * @param handle : The handle on which to check for events
 * @param req_id : The req_id which finished
 */ 
void check_for_events (rpmh_client_t *handle, uint32 req_id)
{
  inq_iterator_t it;
  active_events_t *event;

  inq_it_begin (handle->barriers, &it);

  while((event = (active_events_t *)inq_it_deref (&it)) != NULL)
  {
    //if its an "all" wait and this request is the oldest, trigger the corresponding event/callback
    //  if its a "single" wait and this request is the one we are waiting on, trigger the corresponding event/callback	  
    if ((((event->barrier_type == RPMH_CALLBACK_ALL) || (event->barrier_type == RPMH_BARRIER_ALL)) && (event->req_id < handle->oldest_req_id)) ||
        (((event->barrier_type == RPMH_CALLBACK_SINGLE) || (event->barrier_type == RPMH_BARRIER_SINGLE)) && (handle->req_id == req_id)))
    {
      switch(event->barrier_type)
      {
        case RPMH_CALLBACK_ALL:
        case RPMH_CALLBACK_SINGLE:
          //call the callback	      
          handle->completion_cb(req_id); 
          break;

        case RPMH_BARRIER_ALL:
        case RPMH_BARRIER_SINGLE:
          //trigger the event	      
          rpmh_event_set (event->event);
          break;
        default :
          rpmh_core_verify(FALSE);
          break;
      }	      

      //clean up the event
      inq_it_delete (&it);
      memset (event, 0, sizeof(active_events_t));
      inq_append (handle->free_barriers, event);
      inq_it_begin (handle->barriers, &it);
    }
    else
    {
      inq_it_next (&it);
    }
  }
}

/**
 * <!-- rpmh_client_command_remove -->
 *
 * @brief remove a command and check for any necessary completion events
 *
 * @param handle : The handle to remove from
 * @param req_id : The req_id to remove
 */ 
void rpmh_client_command_remove (rpmh_client_t *handle, uint32 req_id)
{
  cmds_in_progress_t *cmd = NULL;
  //remove cmd from in progress queue
  pull_from_cmds_in_progress_queue (handle->cmds_in_progress, req_id, &cmd);
  memset(cmd, 0, sizeof (cmds_in_progress_t));
  inq_append (handle->free_cmds_in_progress, cmd);

  //update oldest_req_id
  cmds_in_progress_t *head = inq_head (handle->cmds_in_progress);
  if (head != NULL)
  {
    handle->oldest_req_id = head->req_id;
  }	    
  else
  {
    handle->oldest_req_id = CMD_EMPTY;
  }

  //decrement num_reqs_in_progress since one just finished
  handle->num_reqs_in_progress -= 1;
}

/**
 * <!-- churn_cmd -->
 *
 * @brief polling based wait for a cmd to finished
 *
 * @param handle       : The handle on which to wait the next command to finished
 * @param barrier_type : The type of wait (all vs single)
 */ 
static void churn_cmd(rpmh_client_t *handle, rpmh_barrier_enum barrier_type)
{
  //wait until a command has finished
  while(!is_amc_finished (handle->drv_id)){}
  
  //now that a command has finished, handle it
  rpmh_finish_amc (handle->drv_id);
}

static active_events_t *new_event(rpmh_client_t *handle)
{
  active_events_t *event = inq_pop (handle->free_barriers);
  if (event == NULL)
  {
    event = (active_events_t *)rpmh_malloc (sizeof (active_events_t));
    rpmh_core_verify_ptr (event);
    memset (event, 0, sizeof (active_events_t));
  }
  return event;
}

/**
 * <!-- wait_cmd -->
 *
 * @brief event based wait for a cmd to finished
 *
 * @param handle       : The handle on which to wait for the given cmd to finish
 * @param req_id       : The command we are waiting for
 * @param barrier_type : The type of wait (all vs single)
 */ 
static void wait_cmd (rpmh_client_t *handle, uint32 req_id, rpmh_barrier_enum barrier_type)
{
  rpmh_event_t *finish_event;	
  active_events_t *event = new_event(handle);

  event->event = rpmh_event_create();
  event->barrier_type = barrier_type;
  event->req_id = req_id;
  finish_event = event->event;
  inq_append (handle->barriers, event);

  rpmh_unlock (FALSE); 
  rpmh_event_wait (finish_event);
  rpmh_lock (FALSE);		    
  rpmh_event_destroy (finish_event);
}

/**
 * <!-- rpmh_client_save_read_data -->
 *
 * @brief Used to store data from a read command
 *
 * @param handle : The handle that initiated the read request
 * @param req_id : The read command id
 * @param read   : The data from the read
 */ 
void rpmh_client_save_read_data (rpmh_client_t *handle, uint32 req_id, uint32 data)
{
  cmds_in_progress_t *cmd = NULL;
  boolean req_id_found = get_cmd_in_progress (handle, &cmd, req_id);

  rpmh_core_verify (req_id_found);

  cmd->data_valid = TRUE;	  
  cmd->data = data;
}

/**
 * <!-- rpmh_client_command_finish -->
 *
 * @brief A command has finished, clean it up at the client level
 *
 * @param handle : The handle associated with the finished command
 * @param req_id : The command id that has finished
 */ 
void rpmh_client_command_finish(rpmh_client_t *handle, uint32 req_id)
{
  cmds_in_progress_t *cmd = NULL;
  boolean req_id_found = get_cmd_in_progress(handle, &cmd, req_id);

  //if we found a non-read requests, handle it
  if(req_id_found && (!cmd->data_valid))
  {
    //read requests are not removed yet because the client might still need to retrieve the data	  
    if(!cmd->data_valid)
    {  	    
      rpmh_client_command_remove(handle, req_id);
    }      

    //check if any client events need to be triggered
    check_for_events (handle, req_id);
  }
}


/**
 * <!-- rpmh_client_command_wait_on_read -->
 *
 * @brief Wait until a given read command is finished
 *
 * @param handle  : The handle associated with the read command
 * @param req_id  : The read command id we are waiting for
 * @param polling : TRUE for polling mode, FALSE for event based handling
 */ 
uint32 rpmh_client_wait_on_read (rpmh_client_t *handle, uint32 req_id, boolean polling)
{
  uint32 data;	
  cmds_in_progress_t *cmd = NULL;
  boolean req_id_found = get_cmd_in_progress (handle, &cmd, req_id);

  //if the data is there the read command has finished. Clean up and return the data
  if ((req_id_found == TRUE) && (cmd->data_valid))
  {
    data = cmd->data;
    //now that the data has been saved, clean up
    rpmh_client_command_remove (handle, req_id);
  }    
  else
  {
    if (polling)
    {
      churn_cmd (handle, RPMH_BARRIER_SINGLE);
    }
    else
    {
      wait_cmd (handle, req_id, RPMH_BARRIER_SINGLE);
    }
    //recursive call - we handled a command, now check if we are done
    data = rpmh_client_wait_on_read (handle, req_id, polling);
  }    

  return data;
}

/**
 * <!-- rpmh_client_wait_on_cmd -->
 *
 * @brief Wait until the given command is finished
 *
 * @param handle       : The handle associated with the command
 * @param req_id       : The command id we are waiting for
 * @param barrier_type : The type of wait (all vs single)
 * @param polling      : TRUE for polling mode, FALSE for event based handling
 */ 
void rpmh_client_wait_on_cmd (rpmh_client_t *handle, uint32 req_id, rpmh_barrier_enum barrier_type,
                              boolean polling)
{
  cmds_in_progress_t *cmd = NULL;
  boolean req_id_found = get_cmd_in_progress(handle, &cmd, req_id);
  //if it was in the queue, or if the barrier is waiting for ALL and 
  //there are older commands to wait for, wait for the event
  if ((req_id_found == TRUE) || ((barrier_type == RPMH_BARRIER_ALL) && (req_id > handle->oldest_req_id)))
  {
    if (polling)
    {
      //churn a command	    
      churn_cmd (handle, barrier_type);
      //see if we still need to wait	    
      rpmh_client_wait_on_cmd (handle, req_id, barrier_type, TRUE);
    }		
    else
    {
      wait_cmd (handle, req_id, barrier_type);
    }
  }	    
}

/**
 * <!-- rpmh_client_register_req_notification -->
 *
 * @brief Register to callback to the given client when the given req_id has completed
 *
 * @param handle       : The handle associated with request
 * @param req_id       : The command id we are waiting for
 * @param barrier_type : The type of wait (all vs single)
 *
 * @return TRUE if the request was not yet completed
 */ 
void rpmh_client_register_req_notification (rpmh_client_t *handle, uint32 req_id, rpmh_barrier_enum barrier_type)
{
  cmds_in_progress_t *cmd = NULL;
  rpmh_core_verify((barrier_type == RPMH_CALLBACK_SINGLE) || (barrier_type == RPMH_CALLBACK_ALL));
  boolean req_id_found = get_cmd_in_progress(handle, &cmd, req_id);
  //if it was in the queue, or if the barrier is waiting for ALL and 
  //there are older commands to wait for, wait for the event
  if ((req_id_found == TRUE) || ((barrier_type == RPMH_CALLBACK_ALL) && (req_id > handle->oldest_req_id)))
  {
    active_events_t *event = new_event(handle);
    event->barrier_type = barrier_type;
    event->req_id = req_id;
    inq_append (handle->barriers, event);
  }    
  else
  {
    //its not in the queue, call the callback right away
    handle->completion_cb(req_id); 
  }
}


#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_client_barriers -->
 *
 * @brief Prints the current barrier calls from a given client
 *
 * @param handle : The handle on which to print the barriers 
 */
static void print_client_barriers (rpmh_client_t *handle)
{
  inq_iterator_t it;
  active_events_t *event;
  inq_it_begin (handle->barriers, &it);

  printf ("\t\tBARRIERS:\n");
  while ((event = (active_events_t *)inq_it_deref(&it)) != NULL)
  {
    printf ("\t\tBARRIER_TYPE %d  BARRIER_REQ_ID: %d\n", event->barrier_type, event->req_id);
    inq_it_next (&it);
  }
}

/**
 * <!-- print_cmds_in_progress -->
 *
 * @brief Prints the commands in progress for the given handle
 *
 * @param handle : The handle on which to print the commands in progress 
 */
static void print_cmds_in_progress (rpmh_client_t *handle)
{
  inq_iterator_t it;
  cmds_in_progress_t *cmd;
  inq_it_begin(handle->cmds_in_progress, &it);

  printf ("\t\tCMDS_IN_PROGRESS:\n");
  while ((cmd = (cmds_in_progress_t *)inq_it_deref (&it)) != NULL)
  {
    printf ("\t\tREQ_ID: %d\n", cmd->req_id);
    inq_it_next (&it);
  }
  printf ("\t\tCMDS_IN_PROGRESS_DONE\n");
}

/**
 * <!-- print_clients -->
 *
 * @brief Prints current client list and associated client details
 */
void print_clients(rpmh_clientq_t *self)
{
  inq_iterator_t it;
  rpmh_client_t *client;

  inq_it_begin (self->handles, &it);

  printf ("CLIENT_LIST_START\n");
  while ((client = (rpmh_client_t *)inq_it_deref(&it)) != NULL)
  {
    printf ("\tCLIENT NAME: \"%s\" \n", client->name);	    
    printf ("\t\tCLIENT DRV_ID: %d\n", client->drv_id);	    
    printf ("\t\tCLIENT LAST_REQ_ID: %d\n", client->req_id);	    
    printf ("\t\tCLIENT OLDEST_REQ_ID: %d\n", client->oldest_req_id);	    
    print_cmds_in_progress (client);
    print_client_barriers (client);
    inq_it_next (&it);
  }
  printf ("CLIENT_LIST_END");
}
#endif //RPMH_OFFTARGET_TEST

