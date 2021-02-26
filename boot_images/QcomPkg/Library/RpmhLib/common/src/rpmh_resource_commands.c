/*===========================================================================
                              rpmh_resource_commands.c

DESCRIPTION:
  Provides resource command tracking, caching, and updating. 
  
  A resource command is treated as unique RPMh address on which a client has 
  issued a command. This layer only understands the address and corresponding 
  command details (it does not understand what the address corresponds to in 
  human readable form).

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
#include "rpmh_resource_commands.h"
#include "rpmh_os.h"
#include "rpmh_drv_config.h"
#include "rpmh_client.h"
#include "rpmh_tcs.h"
#include "cmd_db.h"
//#include "sleep_npa.h"
#include "npa.h"
//#include "pdcTcs.h"
#include <stddef.h>
#include <string.h>

#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
#endif

#define TCS_NUM_WAKE0 0
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
 * <!-- rpmh_pdc_lpr_avail_cb -->
 *
 * @brief Callback function when PDC LPR node has been created
 */
#if 0
static void rpmh_pdc_lpr_avail_cb(void *context, unsigned int event_type, void *data, 
                                  unsigned int size)
{
  pdc_votes_t *pdc_votes = (pdc_votes_t *)context;
  rpmh_core_verify_ptr(pdc_votes);

  pdc_votes->sleep_voter = npa_create_sync_client(SLEEP_PDC_LPR_NODE_NAME, pdc_votes->voter_name, 
                                             NPA_CLIENT_REQUIRED);
  rpmh_core_verify_ptr(pdc_votes->sleep_voter);

  return;
}
/**
 * <!-- rpmh_resource_command_init_pdc_votes -->
 *
 * @brief Initialize a resource command
 *
 * @param self    : The resource command pointer to initialize pdc votes
 */
static void rpmh_resource_command_init_pdc_votes (rpmh_resource_command_t *self, char *rc_name)
{
  self->pdc_votes = (pdc_votes_t *)malloc (sizeof (pdc_votes_t));
  rpmh_core_verify_ptr(self->pdc_votes);
  memset (self->pdc_votes, 0, sizeof (pdc_votes_t));

  self->pdc_votes->votes = (uint32 *)malloc (sizeof (uint32) * TCS_NUM_TOTAL);	
  rpmh_core_verify_ptr(self->pdc_votes->votes);
  memset (self->pdc_votes->votes, 0, sizeof (uint32) * TCS_NUM_TOTAL);

  self->pdc_votes->size = TCS_NUM_TOTAL;

  self->pdc_votes->voter_name = rc_name;

  //loop through the TCSs	
  for (int i = 0; i < TCS_NUM_TOTAL; i++)
  {
    RPMH_LOG_2 ("rpmh_pdc_init: (TCS NUM: %d) (ADDRESS: %d)", i, self->address);
    const pdc_tcs_config *tcs = pdcTcs_getResources(i);
    //loop through the commands in each TCS to find self
    for(int j = 0; j < NUM_COMMANDS_PER_TCS; j++)
    {
      if((tcs[j].cmd.resourceP != NULL) && 
         (tcs[j].data.addr_offset + tcs[j].cmd.resourceP->base_addr) == self->address)
      {
        RPMH_LOG_2 ("\trpmh_pdc_init_val: (CMD NUM: %d) (VALUE: %d)", j, tcs[j].data.res_val);
        //save the data for this resource in this tcs     
        self->pdc_votes->votes[i] = tcs[j].data.res_val;
      }
    }
  }

  /* Only register a client if the node is available */
  npa_resource_available_cb(SLEEP_PDC_LPR_NODE_NAME, rpmh_pdc_lpr_avail_cb, (void*)self->pdc_votes);
}
#endif
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
  self->pdc_votes = NULL;
#if 0
  //see if this resource is a PDC resource (and needs PDC TCS initialization)
  const pdc_tcs_resource *pdc_rc = pdcTcs_getAllResourceList();
  for(int i = 0; i < TCS_TOTAL_RESOURCE_NUM; i ++)
  {
    uint32 addr = cmd_db_query_addr(pdc_rc[i].name);
    if(addr == address)
    {
      //found the resource in the PDC. Save the PDC contents and create a voter to sleep
      rpmh_resource_command_init_pdc_votes (self, pdc_rc[i].name);
      return;
    }
  } 
#endif
}

boolean rpmh_resource_command_pdc_compare (rpmh_resource_command_t *self, uint32 vote)
{
#if 0
  //determine sleep mode
  //determine RSC sleep vote for resource
  uint32 sleep_tcs_vote = 0;
  pdc_votes_t *pdc_votes = self->pdc_votes;
  for(int i = 0; i < TCS_NUM_TOTAL_SLEEPS; i++)
  {
    if (vote <= pdc_votes->votes[i])
    {
      sleep_tcs_vote |= (1 << i);	      
    }
  }  

  if(NULL != self->pdc_votes->sleep_voter)
  {
    npa_issue_required_request(self->pdc_votes->sleep_voter, sleep_tcs_vote);
  }
  
  return (vote <= pdc_votes->votes[TCS_NUM_WAKE0]);
#endif
  return 1;
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
    curr_drv->priority = (rpmh_rc_priority_enum)cmd_db_query_priority(self->address, (uint8)drv->hw_drv);
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
 * <!-- rpmh_resource_command_sets_dirty -->
 *
 * @brief Check which sets are dirty for the given resource command and voter
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 *
 * @return A bitmask of which sets are dirty
 */
uint32 resource_command_sets_dirty (rpmh_resource_command_t *self, RSCSW_DRV_MAPPING drv_id)
{
  uint32 sets_dirty = 0;	
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);	
  if (curr_drv)
  {
    rpmh_vote_t *sleep_vote = &(curr_drv->local_votes[RPMH_SET_SLEEP]);
    rpmh_vote_t *wake_vote = &(curr_drv->local_votes[RPMH_SET_WAKE]);
    //take advantage of the enum ordering, if its > the last clean, its dirty
    if (sleep_vote->dirty > RPMH_RC_LAST_CLEAN)
    {
      sets_dirty |= (1 << RPMH_SET_SLEEP);
    }
    if (wake_vote->dirty > RPMH_RC_LAST_CLEAN)
    {
      sets_dirty |= (1 << RPMH_SET_WAKE);
    }
  }
  return sets_dirty;
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
      case RPMH_RC_CLEAN_EXPLICIT_VOTE: 
      case RPMH_RC_DIRTY:
        rpmh_core_verify (vote->valid);
        *data = vote->data;
        valid = TRUE;
        break;
      case RPMH_RC_DIRTY_MOL:
        rpmh_core_verify (vote->valid);
        rpmh_core_verify (set == RPMH_SET_SLEEP);
        rpmh_core_verify_ptr (self->pdc_votes->votes);
        *data = self->pdc_votes->votes[TCS_NUM_WAKE0];
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
 * @param self     : The resource command associated with the voter.
 * @param curr_drv : The voter. This is already resource command specific so no more details needed
 */
static void resource_command_dirty_update (rpmh_resource_command_t *self, drv_votes_t *curr_drv)
{
  if(curr_drv)
  {
    rpmh_vote_t *sleep_vote = &(curr_drv->local_votes[RPMH_SET_SLEEP]);
    rpmh_vote_t *active_vote = &(curr_drv->local_votes[RPMH_SET_ACTIVE]);
    rpmh_vote_t *wake_vote = &(curr_drv->local_votes[RPMH_SET_WAKE]);

    //if active vote is valid, and (vote at rpmh is not or vote at rpmh != active vote)
    if ((active_vote->valid) && 
		    ((!curr_drv->vote_at_rpmh.valid) || 
                     ((active_vote->data) != curr_drv->vote_at_rpmh.data)))
    {
      //need to send active vote
      active_vote->dirty = RPMH_RC_DIRTY;
    }
    else
    {
      active_vote->dirty = RPMH_RC_CLEAN;
    }

    //only update dirtiness if vote is not explicit
    if (sleep_vote->dirty != RPMH_RC_CLEAN_EXPLICIT_VOTE)
    {
      //if sleep vote is valid, and ((active vote is not valid) or (active is valid AND active != sleep)) 
      if ((sleep_vote->valid) &&
          ((!active_vote->valid) || 
          ((active_vote->valid) && (active_vote->data != sleep_vote->data))))
      {
        //need to send a sleep vote		
        sleep_vote->dirty = RPMH_RC_DIRTY;
        if(self->pdc_votes != NULL)
        {
          //sleep vote is dirty. If this is a PDC resource, don't sent < MOL		
          boolean MOL = rpmh_resource_command_pdc_compare (self, sleep_vote->data);
          if(MOL)
          {
            sleep_vote->dirty = RPMH_RC_DIRTY_MOL;
          }  
        }
      }	
      else
      {
        sleep_vote->dirty = RPMH_RC_CLEAN;
      }
    }
  
    //only update dirtiness if vote is not explicit
    if (wake_vote->dirty != RPMH_RC_CLEAN_EXPLICIT_VOTE)
    {
      //if wake is valid and ((wake != active) OR (wake != sleep))
      if ((wake_vote->valid) && 
          (((active_vote->valid) && (wake_vote->data != active_vote->data)) || 
          ((sleep_vote->valid) && (wake_vote->data != sleep_vote->data)))) 
      {
        //need to send a wake vote		
        wake_vote->dirty = RPMH_RC_DIRTY;
      } 
      else if((!wake_vote->valid) && 
           ((active_vote->valid && sleep_vote->valid) && 
           (active_vote->data != sleep_vote->data) && 
           (sleep_vote->dirty != RPMH_RC_CLEAN_EXPLICIT_VOTE)))
      {
        //need to send a wake vote
        //special case. No wake vote, but active and sleep votes that are different (and its not an explicit vote).    
        wake_vote->dirty = RPMH_RC_DIRTY_USE_ACTIVE;
      }
      else
      {
        wake_vote->dirty = RPMH_RC_CLEAN;
      }
    }
  }
}

/**
 * <!-- rpmh_resource_command_invalidate -->
 *
 * @brief Invlidate the vote for a given set. 
 *
 * NOTE: ACTIVE IS NOT ALLOWED. There is no "invalidate" at the HW level, so if a request has 
 *       been sent to RPMh it will persist. Thus we can only invalidate sleep/wake.
 *
 * @param self   : The resource command pointer
 * @param set    : The set on which to invalidate
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 */
void rpmh_resource_command_invalidate (rpmh_resource_command_t *self, rpmh_set_enum set, RSCSW_DRV_MAPPING drv_id)
{
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);
  if (curr_drv)
  {
      curr_drv->local_votes[set].valid = FALSE;
  }

  resource_command_dirty_update(self, curr_drv);
}

/**
 * <!-- rpmh_resource_command_update -->
 *
 * @brief Updated the data for a given command 
 *
 * @param self         : The resource command pointer
 * @param set          : The set associated with the data
 * @param data         : The new data
 * @param drv_id       : The RSCSW_DRV_MAPPING associated with the voter
 * @param explicit_cmd : If this is a command from an explicit client. If so, we ignore sleep/wake details
 *
 * @return TRUE if the given resource and set (for the given drv_id) is dirty after the update
 */
boolean rpmh_resource_command_update (rpmh_resource_command_t *self, rpmh_set_enum set, uint32 data, 
                                      RSCSW_DRV_MAPPING drv_id, boolean explicit_cmd)
{
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);	

  //establish the data and mark it as valid for the given set
  curr_drv->local_votes[set].data = data;
  curr_drv->local_votes[set].valid = TRUE;
  
  if ((explicit_cmd) && (set != RPMH_SET_ACTIVE))
  {
    //if we have an explicit voter, mark sleep/wake such that they don't update	  
    curr_drv->local_votes[set].dirty = RPMH_RC_CLEAN_EXPLICIT_VOTE;	  
  }

  resource_command_dirty_update (self, curr_drv);

  return (curr_drv->local_votes[set].dirty > RPMH_RC_LAST_CLEAN);
}

/**
 * <!-- rpmh_resource_command_exit_sleep -->
 *
 * @brief Exit sleep processing for the given rc/drv: 
 *             -set active vote as wake value
 *             -clear out wake value
 *             -update val at rpmh 
 *
 * @param self   : The resource command pointer
 * @param drv_id : The RSCSW_DRV_MAPPING associated with the voter
 */
void rpmh_resource_command_exit_sleep (rpmh_resource_command_t *self, RSCSW_DRV_MAPPING drv_id)
{
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (self, drv_id);

  rpmh_vote_t *wake_vote = &(curr_drv->local_votes[RPMH_SET_WAKE]);

  if((curr_drv) && (wake_vote->valid))
  {
    rpmh_vote_t *active_vote = &(curr_drv->local_votes[RPMH_SET_ACTIVE]);

    //we just woke up, set active to wake values
    active_vote->data = wake_vote->data;
    active_vote->valid = TRUE;

    //only clear the wake contents if the wake vote is not an explicit vote
    if(wake_vote->dirty != RPMH_RC_CLEAN_EXPLICIT_VOTE)
    {
      //clear out wake values
      wake_vote->data = 0;
      wake_vote->valid = FALSE;
    }

    //set val at rpm to the new active values
    curr_drv->vote_at_rpmh.data = active_vote->data;
    curr_drv->vote_at_rpmh.valid = TRUE;
  }

  resource_command_dirty_update (self, curr_drv);
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
  rpmh_core_verify_ptr (rc);

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
  rpmh_core_verify_ptr (rc);
  drv_votes_t *curr_drv = rpmh_resource_command_get_voter (rc, drv_id);
  rpmh_core_verify_ptr (curr_drv);
  curr_drv->state = RPMH_RC_STATE_IDLE;
  resource_command_dirty_update (rc, curr_drv);
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

