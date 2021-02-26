/*===========================================================================
                              rpmh_resource_commands.c

DESCRIPTION:
  Provides resource command tracking, caching, and updating. 
  
  A resource command is treated as unique RPMh address on which a client has 
  issued a command. This layer only understands the address and corresponding 
  command details (it does not understand what the address corresponds to in 
  human readable form).

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
#include "rpmh_resource_commands.h"
#include "rpmh_os.h"
#include "rpmh_drv_config.h"
#include "rpmh_client.h"
#include "rpmh_tcs.h"
#include "cmd_db.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
#endif

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
 * <!-- rpmh_resource_command_init -->
 *
 * @brief Initialize a resource command
 *
 * @param self    : The resource command pointer to initialize
 * @param address : The address to associate with the resource command
 */
void rpmh_resource_command_init (rpmh_resource_command_t *self, uint32 address)
{
  memset (self, 0, sizeof (*self));
  self->address = address;
}

/**
 * <!-- rpmh_resource_command_get_voter -->
 *
 * @brief Retrieve a voter for the resource command
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 *
 * @return A pointer to the voter for this rc+drv
 */
drv_votes_t *rpmh_resource_command_get_voter (rpmh_resource_command_t *self, RSCSW_DRV_MAPPING drv_id)
{
  drv_votes_t *curr_drv = self->drv_votes;
  drv_votes_t *prev_drv = NULL;

  while ((curr_drv) && (curr_drv->drv_id != drv_id))
  {
    prev_drv = curr_drv;		
    curr_drv = curr_drv->next;
  }
  if (curr_drv == NULL)
  {
    curr_drv = (drv_votes_t *)rpmh_malloc (sizeof (drv_votes_t));
    rpmh_core_verify_ptr(curr_drv);
    memset (curr_drv, 0, sizeof (drv_votes_t));
    curr_drv->drv_id = drv_id;
	
    //priority needs the HW_DRV
    uint32 drv_index = lookup_drv_index (drv_id);
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
    curr_drv->priority = RPMH_RC_PRIORITY_USE_CASE;
    if (self->drv_votes == NULL)
    {
      self->drv_votes = curr_drv;
    }
    else
    {
      rpmh_core_verify_ptr(prev_drv);	    
      prev_drv->next = curr_drv;
    }
  }
  return curr_drv;
}

/**
 * <!-- rpmh_resource_command_get_priority -->
 *
 * @brief Retrieve this voters priority for the given resource command
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 *
 * @return The priority value associated with this rc+drv
 */
rpmh_rc_priority_enum rpmh_resource_command_get_priority(rpmh_resource_command_t *self, RSCSW_DRV_MAPPING drv_id)
{
  return (rpmh_resource_command_get_voter (self, drv_id))->priority;
}

/**
 * <!-- rpmh_resource_command_get_data -->
 *
 * @brief Retrieve the data for a given resource command, voter, and set 
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 * @param set    : The set on which to retrieve the data
 * @param data   : A pointer to be filled in with the data if its valid
 *
 * @return TRUE if the data is valid, FALSE if not
 */
static boolean resource_command_get_data (rpmh_resource_command_t *self, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set, uint32 *data)
{
  boolean valid = FALSE;	
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);
  if (curr_drv)
  {
    rpmh_vote_t *vote = & (curr_drv->local_votes[set]);
    rpmh_vote_t *active_vote = & (curr_drv->local_votes[RPMH_SET_ACTIVE]);
    switch (vote->dirty)
    {
      //We might have a situation where a new request came in that caused it to be clean. However,
      //  the request is already issued and has an ID, and it might be part of a command set.
      //  Due to this we send it anyway, though it will short circuit at the RPMh HW level.
      //  Additionally, if we have an explicit vote, we store the value in the RC still, 
      //  so it needs to be retrieved here
      case RPMH_RC_CLEAN: 
      case RPMH_RC_DIRTY:
        rpmh_core_verify (vote->valid);
        *data = vote->data;
        valid = TRUE;
        break;

      case RPMH_RC_DIRTY_USE_ACTIVE:
        rpmh_core_verify (active_vote->valid);
        *data = active_vote->data;
        valid = TRUE;
        break;
      default:
        rpmh_core_verify (FALSE);
    }
  }
  return valid;
}

/**
 * <!-- rpmh_resource_command_dirty_update -->
 *
 * @brief Update the dirtiness bit for a given voter
 *
 * @param self       : The resource command associated with the voter.
 * @param curr_drv   : The voter. This is already resource command specific so no more details needed
 * @param completion : Whether or not this new vote is completion based 
 * @param set        : The set associated with the new vote
 */
static void resource_command_dirty_update (rpmh_resource_command_t *self, drv_votes_t *curr_drv, boolean completion, rpmh_set_enum set)
{
  if(curr_drv)
  {
    rpmh_vote_t *active_vote = &(curr_drv->local_votes[RPMH_SET_ACTIVE]);

    //if active vote is valid, and ((vote at rpmh is not) or (vote at rpmh != active vote) or (vote is for active RR, and prior vote was FF))
    if ((active_vote->valid) && 
		    ((!curr_drv->vote_at_rpmh.valid) || 
                     ((active_vote->data) != curr_drv->vote_at_rpmh.data) || 
		     ((set == RPMH_SET_ACTIVE) && completion && (!(active_vote->completion)))))
    {
      //need to send active vote
      active_vote->dirty = RPMH_RC_DIRTY;
    }
    else
    {
      active_vote->dirty = RPMH_RC_CLEAN;
    }
  }
}

/**
 * <!-- rpmh_resource_command_update -->
 *
 * @brief Updated the data for a given command 
 *
 * @param self   : The resource command pointer
 * @param set    : The set associated with the data
 * @param data   : The new data
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 *
 * @return TRUE if the given resource and set (for the given drv_id) is dirty after the update
 */
boolean rpmh_resource_command_update (rpmh_resource_command_t *self, rpmh_set_enum set, uint32 data, boolean completion,
                                      RSCSW_DRV_MAPPING drv_id)
{
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);	

  //establish the data and mark it as valid for the given set
  curr_drv->local_votes[set].data = data;
  curr_drv->local_votes[set].valid = TRUE;
  
  resource_command_dirty_update (self, curr_drv, completion, set);

  curr_drv->local_votes[set].completion = completion;

  return (curr_drv->local_votes[set].dirty > RPMH_RC_LAST_CLEAN);
}

/**
 * <!-- rpmh_resource_command_val_at_rpmh -->
 *
 * @brief Sets the "val_at_rpmh" for the given rc. Updates the resource state to in progress 
 * for an AMC. This is called when a new vote has been sent to the HW. 
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 * @param set    : The set associated with the command 
 * @param amc    : whether or not the command is an AMC
 *
 * @return The data corresponding to val_at_rpmh
 */
uint32 rpmh_resource_command_val_at_rpmh (uint32 address, RSCSW_DRV_MAPPING drv_id, 
                                          rpmh_set_enum set, boolean amc)
{
  uint32 data = 0;
  rpmh_resource_command_t *rc = rpmh_find_resource_command (address);
  rpmh_core_verify_ptr(rc);
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, drv_id);

  //this should only be called after the commands have been updated. 
  //Abort if there is no drv id for this rc already	
  rpmh_core_verify_ptr (curr_drv);

  //the resource must be idle (no AMCs in progress)
  rpmh_core_verify (curr_drv->state == RPMH_RC_STATE_IDLE);

  rpmh_core_verify (resource_command_get_data(rc, drv_id, set, &data));

  curr_drv->vote_at_rpmh.data = data;
  curr_drv->vote_at_rpmh.valid = TRUE;
  if (amc)
  {	    
    curr_drv->state = RPMH_RC_STATE_IN_PROGRESS;
  }

  return data;
}

/**
 * <!-- rpmh_resource_command_is_idle -->
 *
 * @brief Check if the given RC is idle 
 *
 * @param address : The address associated with the resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 */
boolean rpmh_resource_command_is_idle (rpmh_resource_command_t *rc, RSCSW_DRV_MAPPING drv_id)
{
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, drv_id);
  rpmh_core_verify_ptr (curr_drv);
  return (curr_drv->state == RPMH_RC_STATE_IDLE);
}


/**
 * <!-- rpmh_resource_command_finish -->
 *
 * @brief A command has finished, clean up at the resource level 
 *
 * @param address : The address associated with the resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 */
void rpmh_resource_command_finish (uint32 address, RSCSW_DRV_MAPPING drv_id)
{
  rpmh_resource_command_t *rc = rpmh_find_resource_command (address);
  rpmh_core_verify_ptr(rc);
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, drv_id);
  rpmh_core_verify_ptr (curr_drv);
  curr_drv->state = RPMH_RC_STATE_IDLE;
  resource_command_dirty_update (rc, curr_drv, FALSE, RPMH_SET_ACTIVE);
}

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_resource_votes -->
 *
 * @brief Prints current votes from all DRVs on the given resource command 
 *
 * @param self     : list of resource commands
 */
void print_resource_votes (rpmh_resource_command_t *self)
{
  int i;	
  drv_votes_t *votes = self->drv_votes;
  while (votes)
  {
    printf ("\n\t\t\t VOTES FOR DRV_ID: %d", votes->drv_id);
    printf ("\n\t\t\t VOTE AT RPM DATA: %d  VALID: %d", votes->vote_at_rpmh.data, 
           votes->vote_at_rpmh.valid);
    printf ("\n\t\t\t STATE: %d", votes->state);
    printf ("\n\t\t\t LOCAL VOTES");
    for (i = 0; i < RPMH_NUM_SETS; i++)
    {
      printf ("\n\t\t\t\t SET: %d VOTE: %d VALID: %d DIRTY %d", i, votes->local_votes[i].data, 
              votes->local_votes[i].valid, votes->local_votes[i].dirty);
    }
    votes = votes->next;
  }

}

/**
 * <!-- print_resource_commands -->
 *
 * @brief Prints current resource command list and associated details
 *
 * @param self     : list of resource commands
 * @param num_cmds : # of resource commands in the list
 */
void print_resource_commands (rpmh_resource_command_t *self, uint32 num_cmds)
{
  int i;
  printf ("\nPRINTING_CMDS: \n");
  printf ("\tNUM_CMDS: %d\n", num_cmds);
  for (i = 0; i < num_cmds; i++)
  {
    printf ("\n\tRC_INDEX: %d\n", i);
    printf ("\tRC_ADDRESS: 0x%08x\n", self[i].address);
    print_resource_votes (&self[i]);
  }
}
#endif //RPMH_OFFTARGET_TEST
