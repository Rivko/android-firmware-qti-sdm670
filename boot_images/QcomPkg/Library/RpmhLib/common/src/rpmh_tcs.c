/*===========================================================================
                              rpmh_tcs.c

DESCRIPTION:
  Provides tcs management. This includes configuration, triggering AMCs, 
  cleaning up when a TCS has finished, and retrieving read command data. This
  is the main layer which interacts with HAL. Any additional HAL related 
  interaction for RSC/TCS functionality should be put here.

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
#include "rpmh_tcs.h"
#include "comdef.h"
#include "rsc_hal.h"
#include "rpmh_os.h"
#include "rpmh_drv_config.h"
#include <stddef.h>
#include <string.h>

#ifdef RPMH_SENSORS_ISLAND
#include "urpmh.h"
#endif //RPMH_SENSORS_ISLAND

#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
#include "rpmh_cmdq.h"
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
  RscHalMessage g_msg;
  RscHalMessageBody g_msg_body[IMAGE_TCS_SIZE];

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//2D array consisting of rpmh_tcs[DRVs][TCSs]
tcs_t **rpmh_tcs;
uint32 *tcs_mode;

#ifdef RPMH_SENSORS_ISLAND
//---------------------------------------------------------------------------
// Globals, Externs, Statics for Island Mode
//---------------------------------------------------------------------------
uint32 urpmh_cmd_sets_in_use = 0;
static boolean island_mode = FALSE;
extern urpmh_command_set_t urpmh_cmd_sets[NUM_URPMH_CMD_SETS];
uint32 urpmh_num_cmds_allowed = 5;
static RSCSW_DRV_MAPPING island_amc_drv = RSC_DRV_SENSORS;
uint32 sleep_tcs_amc_index = 0;
#endif //RPMH_SENSORS_ISLAND
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
 * <!-- tcs_init -->
 *
 * @brief Initialize internal TCS structures as well as TCS hardware
 */
void tcs_init (void)
{
  uint32 num_tcs=0;
  uint32 num_cmds=0;

  //allocate the DRV portion of TCS lookup table
  rpmh_tcs = (tcs_t **)rpmh_malloc (sizeof (tcs_t *) * DRV_CONFIG_DATA->num_drvs);        
  rpmh_core_verify_ptr (rpmh_tcs);
  memset (rpmh_tcs, 0, sizeof(tcs_t *) * DRV_CONFIG_DATA->num_drvs);

  tcs_mode = (uint32 *)rpmh_malloc (sizeof (uint32) * DRV_CONFIG_DATA->num_drvs);        
  rpmh_core_verify_ptr (tcs_mode);
  memset (rpmh_tcs, 0, sizeof(uint32) * DRV_CONFIG_DATA->num_drvs);


  //loop through all supported DRVs since they each have their own TCS allocation
  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[i]; 
    tcs_mode[i] = 0;
    
    //if its not a virtual DRV...
    if(drv->drv_id < RSC_DRV_VIRTUAL_DRVS)
    {
      //initialize DRV at HAL
      rpmh_core_verify (RscHalRegisterDrv(drv->drv_id) == HAL_STATUS_SUCCESS);

      //verify #TCS and #CMDs in HW is what we have in SW config
      rpmh_core_verify (RscHalReadConfig (drv->drv_id, &num_tcs, &num_cmds) == HAL_STATUS_SUCCESS);

#ifndef RPMH_OFFTARGET_TEST
      rpmh_core_verify (num_tcs >= drv->num_tcs); 
      rpmh_core_verify (num_cmds >= drv->num_cmds);

      //num_amcs is a subset of num_tcs
      rpmh_core_verify (num_tcs >= (drv->modes[0]->num_amcs));
#endif //RPMH_OFFTARGET_TEST

      //ignoring error for these APIs. These are only viable for DRV0 for the given context 
      //  (which we don't know). HAL doesn't handle this for us, so any non-DRV0 calls here 
      //  will return a failure but won't have any functional issue.
      //turn on ePCB timeout for the given DRV
      RscHalUpdateePCBTimeOutThreshold(drv->drv_id, 0xFFFF);
      RscHalToggleePCBTimeOut (drv->drv_id, TRUE);
    }
    
    //allocate the TCS table for this DRV
    rpmh_tcs[i] = (tcs_t *)rpmh_malloc (sizeof (tcs_t) * drv->num_tcs); 
    rpmh_core_verify_ptr (rpmh_tcs[i]);
    memset (rpmh_tcs[i], 0, sizeof(tcs_t) * drv->num_tcs);

    //for now, all DRVs should have the same # cmds per TCS
    rpmh_core_verify(drv->num_cmds == IMAGE_TCS_SIZE);

    //make sure there is room for the sleep/wake TCS if they exist
    if (num_tcs > drv->modes[0]->num_amcs)
    {
      rpmh_core_verify (num_tcs >= (drv->modes[0]->num_amcs + 2));
    }
    
    tcs_update_mode(i, 0);
  }
}

/**
 * <!-- tcs_update_mode -->
 *
 * @brief Update the TCS mode in use for the given DRV
 *
 * @param drv_id   : The DRV on which to switch TCS configuration
 * @param new_mode : The new mode to switch to 
 */
void tcs_update_mode(uint32 drv_index, uint32 new_mode)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify(new_mode < drv->num_modes);	
  tcs_mode[drv_index] = new_mode;

  //loop through and update each TCS
  for(int i = 0; i < drv->num_tcs; i++)
  {
    tcs_t *tcs = &(rpmh_tcs[drv_index][i]);
    tcs->id = i + drv->tcs_offset;
    rpmh_core_verify (RscHalClearAMCFinishedIRQ(drv->hw_drv, tcs->id) == HAL_STATUS_SUCCESS);
    tcs->time_last_sent = 0;
    if (i < drv->modes[new_mode]->num_amcs)
    {
      //mark AMCs as AMCs in hardware
      tcs->state = TCS_AMC_IDLE;
      rpmh_core_verify (RscHalConvertTCStoAMC(drv->hw_drv, tcs->id) == HAL_STATUS_SUCCESS);
    }
    else
    {
      //mark TCSs as TCSs in hardware
      tcs->state = TCS_NON_AMC;
      rpmh_core_verify (RscHalConvertAMCtoTCS(drv->hw_drv, tcs->id) == HAL_STATUS_SUCCESS);
    }	
  }
}

/**
 * <!-- drv_valid -->
 *
 * @brief Check whether or not a DRV is valid (is supported by this context)
 *
 * @param drv_id : RSC+DRV id for validity check
 *
 * @return whether or not the given DRV is valid
 */
boolean drv_valid (RSCSW_DRV_MAPPING drv_id)
{
  int i;	
  for (i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    if (drv_id == DRV_CONFIG_DATA->drvs[i].drv_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * <!-- lookup_drv_valid -->
 *
 * @brief Get the index into the DRV config table for the given ID
 *
 * @param RSC+DRV id 
 *
 * @return index into DRV config table
 */
int32 lookup_drv_index (RSCSW_DRV_MAPPING drv_id)
{
  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    if (drv_id == DRV_CONFIG_DATA->drvs[i].drv_id)
    {
      return i;
    }
  }

  //we don't have a config for the requested DRV. This should never happen
  rpmh_core_verify (FALSE);
  return -1;
}

/**
 * <!-- is_tcs_stuck -->
 *
 * @brief Check to see if a TCS is stuck. Uses TCS_TIMEOUT_THRESHOLD as defined
 *        in rpmh_tcs.h to determine if a tcs is stuck
 *
 * @param RSC+DRV id 
 *
 * @return TRUE if a tcs is stuck, FALSE if not
 */
boolean is_tcs_stuck (RSCSW_DRV_MAPPING drv_id)
{
  boolean retval = FALSE;

  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  for (int i = 0; i < drv->modes[tcs_mode[drv_index]]->num_amcs; i++)
  {
    boolean is_idle;
    rpmh_core_verify (RscHalIsTCSIdle (drv->hw_drv, i + drv->tcs_offset, &is_idle) == HAL_STATUS_SUCCESS);
    if (!is_idle)
    {
      uint64 current_time = rpmh_get_timestamp ();   
      if (current_time > (rpmh_tcs[drv_index][i].time_last_sent + TCS_TIMEOUT_THRESHOLD))
      {
        RPMH_LOG_2 ("!!!!TCS STUCK!!!!", drv_id, i);      
	retval = TRUE;
      }
    }
  }    
  return retval;
}

/**
 * <!-- tcs_size -->
 *
 * @brief Return the # commands per TCS for this DRV
 *
 * @param drv_id : RSC+DRV id for num cmds check
 *
 * @return number of commands in the TCSs for this DRV
 */
uint32 tcs_size (RSCSW_DRV_MAPPING drv_id)
{
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  return drv->num_cmds;
}

/**
 * <!-- tcs_slots_available -->
 *
 * @brief Retrieve a slot in corresponding TCSs for the given ID and set
 *
 * @param drv_id : RSC+DRV id 
 * @param set    : set on which to check for TCS space
 *
 * @return tcs index which has room. -1 if there is no room
 */
int tcs_slots_available (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set, uint32 num_slots)
{
  uint32 start = 0;
  uint32 end = 0;
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  tcs_t *tcs = NULL;
  rpmh_core_verify(num_slots <= drv->num_cmds);
  switch (set)
  {
    case RPMH_SET_SLEEP:
      start = drv->modes[tcs_mode[drv_index]]->sleep_start;
      end = drv->modes[tcs_mode[drv_index]]->wake_start;
      break;

    case RPMH_SET_WAKE:
      start = drv->modes[tcs_mode[drv_index]]->wake_start;
      end = drv->num_tcs;
      break;
    default:
      rpmh_core_verify (FALSE);
      break;
  }	    

  for (int i = start; i < end; i++)
  {
    tcs = &(rpmh_tcs[drv_index][i]);
    if ((!tcs->cmds) || (num_slots <= (drv->num_cmds - tcs->cmds->num_rcs)))
    {
      //if there are no commands yet or the # of commands left is < num_slots_needed 
      return i;                
    }
  }		
  return -1;
}

/**
 * <!-- tcs_prepare_sleep_add_cmd_set -->
 *
 * @brief Add a resource command to a TCS
 *
 * @param address : address for the resource command 
 * @param drv_id  : RSC+DRV id 
 * @param set     : set for the command to add
 *
 * @return TRUE if we were able to add the command, FALSE if not (no room in TCSs)
 */
boolean tcs_prepare_sleep_add_cmd_set (rpmh_command_set_internal_t *cmd_set, RSCSW_DRV_MAPPING drv_id)
{
  uint32 drv_index = lookup_drv_index(drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  tcs_t *tcs = NULL;

  //find proper tcs pointer
  int tcs_slot = tcs_slots_available (drv_id, cmd_set->set, cmd_set->num_commands);

  //if there was no TCS with room, notify the caller
  if(tcs_slot== -1)
  {
      return FALSE;
  }

  tcs = &(rpmh_tcs[drv_index][tcs_slot]);

  //verify the tcs is valid and its not an AMC
  rpmh_core_verify_ptr (tcs);
  rpmh_core_verify (tcs->state == TCS_NON_AMC);

  //if this is our first sleep cycle, create the cmds structure for the sleep/wake
  if (!tcs->cmds)
  {
    tcs->cmds = rpmh_malloc (sizeof(rpmh_cmd_t));
    rpmh_core_verify_ptr (tcs->cmds);
    memset (tcs->cmds, 0, sizeof(rpmh_cmd_t));

    tcs->cmds->details = (rpmh_cmd_details_t *)rpmh_malloc (sizeof (rpmh_cmd_details_t) * drv->num_cmds);    
    rpmh_core_verify_ptr (tcs->cmds->details);
    memset (tcs->cmds->details, 0, sizeof (rpmh_cmd_details_t) * drv->num_cmds);
    tcs->cmds->set = cmd_set->set;
  }

  rpmh_cmd_t *cmds = tcs->cmds; 

  for(int i = 0; i < cmd_set->num_commands; i++)
  {
    uint32 cmd_index = (cmds->num_rcs + i);	  
    cmds->details[cmd_index].address = cmd_set->commands[i].address;
    cmds->details[cmd_index].type = cmd_set->commands[i].completion ? WriteCompletion : WriteFirenForget;
  }
  cmds->amc = FALSE;
  cmds->num_rcs += cmd_set->num_commands;
  cmds->dependency_bmsk = cmd_set->dependency_bmsk;
  rpmh_core_verify(cmds->num_rcs <= drv->num_cmds);

  return TRUE;
}

/**
 * <!-- is_amc_free -->
 *
 * @brief let the caller know if an AMC is available for the given DRV
 *
 * @param drv_id : DRV to check for free AMCs 
 *
 * @return TRUE if an AMC is free
 */
boolean is_amc_free (RSCSW_DRV_MAPPING drv_id)
{
  int i;
  uint32 drv_index = lookup_drv_index(drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (drv->modes[tcs_mode[drv_index]]->num_amcs > 0);
  for (i = 0; i < drv->modes[tcs_mode[drv_index]]->num_amcs; i++)
  {
    if (rpmh_tcs[drv_index][i].state == TCS_AMC_IDLE)
    {
      return TRUE;
    }
  }    
  return FALSE;
}

/**
 * <!-- is_amc_finished -->
 *
 * @brief Checks if an AMC is finished at the RSC level for the given ID
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if an AMC is finished, FALSE if not 
 */
boolean is_amc_finished (RSCSW_DRV_MAPPING drv_id)
{
  uint32 status;
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (RscHalCheckAMCFinishedIRQ (drv->hw_drv, &status) == HAL_STATUS_SUCCESS);
  return (status != 0);
}

/**
 * <!-- get_finished_drv -->
 *
 * @brief Checks if any DRV has finisehd for the given hw_drv. This is used for virtual DRVs to
 *        determine if a virtual DRV has completed or a normal DRV has completed, because virtual
 *        DRVs are a software concept only and won't show up in the RSC registers
 *
 * @param hw_drv : The HW DRV to check 
 *
 * @return The DRV ID that has finished an AMC 
 */
RSCSW_DRV_MAPPING get_finished_drv(RSCSW_DRV_MAPPING hw_drv)
{
  uint32 status;
  uint32 drv_index = lookup_drv_index (hw_drv);
  drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (RscHalCheckAMCFinishedIRQ(drv->hw_drv, &status) == HAL_STATUS_SUCCESS);
  if (status != 0)
  {
    uint32 tcs_id = (31 - rpmh_clz((uint32)status));    
    for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
    {
      drv = &DRV_CONFIG_DATA->drvs[i]; 
      int tcs_index = tcs_id - drv->tcs_offset;
      if((tcs_index >= 0) && (tcs_index < drv->modes[tcs_mode[drv_index]]->num_amcs)) 
      {
        return drv->drv_id;
      }
    }
  }
  return RSC_DRV_VIRTUAL_MAX;
}

/**
 * <!-- get_finished_amc -->
 *
 * @brief retrieves a finished AMC for the given ID
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return pointer to the finished AMC if one finished, NULL if not 
 */
static tcs_t *get_finished_amc (RSCSW_DRV_MAPPING drv_id)
{
  uint32 status;
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (RscHalCheckAMCFinishedIRQ(drv->hw_drv, &status) == HAL_STATUS_SUCCESS);
  if (status != 0)    
  {    
    uint32 tcs_index = (31 - rpmh_clz((uint32)status)) - (drv->tcs_offset);    
    rpmh_core_verify (tcs_index <= drv->num_tcs);
    rpmh_core_verify (rpmh_tcs[drv_index][tcs_index].state == TCS_AMC_WAIT_FOR_DONE);
    return &rpmh_tcs[drv_index][(tcs_index)];
  }	
  return NULL;
}

/**
 * <!-- tcs_clean_up -->
 *
 * @brief A TCS has completed, clean it up
 *
 * @param cmdq         : cmd queue for the given DRV. This is just a passthrough to the cmdq layer 
 * @param finished_amc : pointer to the amc that is completed 
 * @param drv_id       : RSC+DRV id 
 * @param set          : set associated with the AMC (could be sleep/wake AMCs)
 */
static inline void tcs_clean_up (rpmh_cmdq_t *cmdq, tcs_t *finished_amc, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  //clear out IRQ
  rpmh_core_verify (RscHalClearAMCFinishedIRQ(drv->hw_drv, finished_amc->id) == HAL_STATUS_SUCCESS);

  //clean up at the queue level
  cmdq_finish_cmd (cmdq, finished_amc->cmds, drv_id, finished_amc->id, set);

  //mark the AMC as idle and clear out its contents
  finished_amc->state = TCS_AMC_IDLE;
  finished_amc->cmds = NULL;
  RPMH_LOG_1("\tTCS IDLE (TCS_ID: %d)", finished_amc->id);
}

/**
 * <!-- tcs_finish_amc -->
 *
 * @brief A TCS has completed, clean it up
 *
 * @param cmdq   : cmd queue for the given DRV. This is just a passthrough to the cmdq layer 
 * @param drv_id : RSC+DRV id 
 * @param set    : set associated with the AMC  (could be sleep/wake AMCs)
 */
void tcs_finish_amc (rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
  tcs_t *finished_amc = get_finished_amc (drv_id); 

  if (finished_amc)
  {
    //make sure we found a valid AMC
    rpmh_core_verify_ptr (finished_amc->cmds);
    
    tcs_clean_up (cmdq, finished_amc, drv_id, set);
  }
}

/**
 * <!-- tcs_get_read_data -->
 *
 * @brief A TCS read command has completed. Get the read data
 *
 * @param drv_id : RSC+DRV id 
 * @param tcs_id : index into the TCS list to indicate which TCS was used 
 * @param req_id : command ID for the completed read command 
 *
 * @return the read data
 */
uint32 tcs_get_read_data (RSCSW_DRV_MAPPING drv_id, uint8 tcs_id, uint32 req_id)
{
  uint32 data = 0;	
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (RscHalReadResponseData (drv->hw_drv, rpmh_tcs[drv_index][tcs_id].id, req_id, &data) == HAL_STATUS_SUCCESS);
  return data;
}

/**
 * <!-- tcs_finish_active_amc -->
 *
 * @brief An active AMC has finished, clean it up
 *
 * @param client : a client pointer to save the client corresponding to the finished AMC 
 * @param cmdq   : cmd queue for the given DRV. This is just a passthrough to the cmdq layer 
 * @param drv_id : RSC+DRV id 
 *
 * @return the command ID of the finished amc
 */
uint32 tcs_finish_active_amc (rpmh_client_t **client, rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id)
{
  uint32 req_id = 0;
  tcs_t *finished_amc = get_finished_amc (drv_id); 

  //nothing finished... we got called in error
  if(!finished_amc)
  {
    return 0;
  }

  //make sure we found a valid AMC
  rpmh_cmd_t *finished_cmds = finished_amc->cmds;
  rpmh_core_verify_ptr (finished_cmds);
  //save client
  *client = finished_cmds->client;

  //save req_id
  req_id = finished_cmds->req_id;

  tcs_clean_up (cmdq, finished_amc, drv_id, RPMH_SET_ACTIVE);

  //return the msg id of the completed request
  return req_id;
}

#ifdef RPMH_SENSORS_ISLAND

/**
 * <!-- get_island_tcs_index -->
 *
 * @brief Finds an island AMC not currently being used for a given set 
 *
 * @param drv_id : The DRV we are using for island AMCs
 * @param set    : The set on which to check island AMC usage
 *
 * @return the tcs index to use
 */
static uint32 get_island_tcs_index(RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set)
{
  boolean use_this_tcs = TRUE;	
  uint32 drv_index = lookup_drv_index(drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  //loop through AMCs to find an index for this island TCS
  for(int i = drv->tcs_offset; i < (drv->modes[tcs_mode[drv_index]]->num_amcs + drv->tcs_offset); i++)
  {
    use_this_tcs = TRUE;

    //check if this AMC has already been used by an island TCS of the same set
    for(int j = 0; j < urpmh_cmd_sets_in_use; j++)
    {
      if((urpmh_cmd_sets[j].tcs_num == i) && (urpmh_cmd_sets[j].cmd_set.set == set))
      {
        //if its been used by an island tcs of the same set, we can't reuse it	      
        use_this_tcs = FALSE;
      }
    }   
    //if this TCS wasn't already being used, lets use it!
    if(use_this_tcs)
    {
      return i;
    }
  }
  return 0xFFFFFFFF;
}

/**
 * <!-- tcs_toggle_island_mode -->
 *
 * @brief Controls whether or not we are about to enter island. If we are about
 *   to enter island mode, the TCS layer sends sleep/wake TCS information to 
 *   island SW instead of directly to the TCS HW. On island exit, clean up the
 *   island TCS SW content
 *
 * @param island_en : TRUE if we are preparing for island mode entry
 */
void tcs_toggle_island_mode(boolean island_en)
{
  island_mode = island_en;

  //if we are exiting - clean up
  if(!island_en)
  {
    for(int i = 0; i < urpmh_cmd_sets_in_use; i++)
    {
      urpmh_cmd_sets[i].type = ISLAND_EMPTY;
    }
    urpmh_cmd_sets_in_use = 0;
  }
}

/**
 * <!-- configure_island_tcs -->
 *
 * @brief Configures the given command for use in island mode
 *
 * @param cmd    : The RPMh command details to configure for island. Note this contains a full command set
 * @param drv_id : The DRV who owns the TCSs we are using in island mode
 */
static void configure_island_tcs(rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id)
{
  urpmh_command_set_t *island_cmd = &(urpmh_cmd_sets[urpmh_cmd_sets_in_use]);
  urpmh_cmd_sets_in_use++;	    
  rpmh_core_verify(urpmh_cmd_sets_in_use <= NUM_URPMH_CMD_SETS);

  switch(cmd->set)
  {
    case RPMH_SET_SLEEP:
      island_cmd->type = ISLAND_ENTER_AMC;    
      break;
    case RPMH_SET_WAKE:
      island_cmd->type = ISLAND_EXIT_AMC;    
      break;
    //case RPMH_SET_ISLAND_SLEEP/WAKE - this will be SSC_CX/MX if we introduce this set
    //
    //only sleep/wake sets for island
    case RPMH_SET_ACTIVE:
    default:
      rpmh_core_verify(FALSE);	    
      break;
  }

  island_cmd->tcs_num = get_island_tcs_index(drv_id, cmd->set);
  //make sure we have a valid tcs returned
  rpmh_core_verify(island_cmd->tcs_num != 0xFFFFFFFF);
  rpmh_command_set_internal_t *cmd_set = &(island_cmd->cmd_set); 
  cmd_set->set = cmd->set;
  cmd_set->num_commands = cmd->num_rcs;
  cmd_set->dependency_bmsk = cmd->dependency_bmsk;
  for(int i = 0; i < cmd_set->num_commands; i++)
  {
    cmd_set->commands[i].address = cmd->details[i].address;
    //don't allow preconfigured read requests in island
    rpmh_core_verify(cmd->details[i].type != ReadRequest);
    cmd_set->commands[i].completion = (cmd->details[i].type == WriteFirenForget) ? FALSE : TRUE ;
    rpmh_core_verify(!cmd->amc);
    cmd_set->commands[i].data = rpmh_resource_command_val_at_rpmh (cmd->details[i].address, drv_id, cmd->set, cmd->amc);
  }
}

static boolean check_island_drvs(RSCSW_DRV_MAPPING drv_id)
{
  return (drv_id == island_amc_drv);
}

#endif //RPMH_SENSORS_ISLAND

/**
 * <!-- tcs_send_internal -->
 *
 * @brief The guts of sending a TCS (AMC or non-AMC)
 *
 * @param cmd    : cmd to send 
 * @param drv_id : RSC+DRV id 
 * @param tcs_id : index into the TCS list to indicate which TCS to use
 */
static void tcs_send_internal (rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id, uint32 tcs_index)
{
  int i;
  uint32 cmd_en_bmsk = 0;
#ifdef RPMH_SENSORS_ISLAND
  //If we are preparing for island mode, send the TCS configuration to the island context instead of the TCS HW
  //Note that we still send sleep/wake AMCs if needed - these are resources not required for subsystem SW to operate AND
  //  we ran out of room in the sleep/wake TCSs
  if((!cmd->amc) && (island_mode) && check_island_drvs(drv_id))
  {
    configure_island_tcs(cmd, drv_id);
    return;
  }
#endif //RPMH_SENSORS_ISLAND
  uint32 drv_index = lookup_drv_index(drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  RPMH_LOG_3("\tconfiguring TCS (DRV_ID: %d)(TCS_ID: %d)(AMC?: %d)", drv_id, tcs_index, cmd->amc);
  if (cmd->client && cmd->amc)
  {
    RPMH_LOG_2("\t\t(CLIENT: %s)(REQ_ID: %d)", cmd->client->name, cmd->req_id);
  }

  //make sure # commands is valid
  rpmh_core_verify (cmd->num_rcs <= drv->num_cmds);

  //save a pointer to the command for this AMC
  rpmh_tcs[drv_index][tcs_index].cmds = cmd;

  //set up the AMC header
  g_msg.TcsId = rpmh_tcs[drv_index][tcs_index].id;
  g_msg.RscSwDrvMapId = drv->hw_drv;  
  g_msg.TotalCommands = cmd->num_rcs;
  g_msg.Start = g_msg_body;

  boolean is_idle = FALSE;
  //make sure TCS is idle (it should be per our internal SW state)
  rpmh_core_verify (RscHalIsTCSIdle (drv->hw_drv, g_msg.TcsId, &is_idle) == HAL_STATUS_SUCCESS);
#ifndef RPMH_OFFTARGET_TEST
  rpmh_core_verify (is_idle);
#endif //RPMH_OFFTARGET_TEST


  //clear out the prior contents
  memset(g_msg_body, 0, sizeof(RscHalMessageBody) * IMAGE_TCS_SIZE);

  //set up the AMC commands
  for (i = 0; i < cmd->num_rcs; i++)
  {
    g_msg_body[i].CmdId = i;    
    g_msg_body[i].SlaveId = SLAVE_ID(cmd->details[i].address);    
    g_msg_body[i].Offset = OFFSET(cmd->details[i].address);    
    g_msg_body[i].RequestType = cmd->details[i].type;   
    g_msg_body[i].DataSize = 8;    
    cmd_en_bmsk |= (1 << i);

    //If it's not a read, get data from rc and mark it as sent to rpmh
    if (cmd->details[i].type != ReadRequest)
    {    
      uint32 data = rpmh_resource_command_val_at_rpmh (cmd->details[i].address, drv_id, cmd->set, cmd->amc);
      g_msg_body[i].Data = data;    
    }      
  }

#ifdef PDC_READ_WORKAROUND_V1
  uint32 last_dependency_bit = 0;
  //V1 workaround
  for(int i = 1; i < cmd->num_rcs; i++)
  {
    uint32 curr_cmd_bmsk = 1 << (i - 1);  
    if((cmd->dependency_bmsk & curr_cmd_bmsk) != 0)  
    {
      last_dependency_bit = i;
    }
    else if((i - last_dependency_bit) % (drv->num_cmds_allowed) == 0)
    {
      cmd->dependency_bmsk |= curr_cmd_bmsk;
    }
  }
#endif //PDC_READ_WORKAROUND_V1      

  //if there's a dependency listed, notify the HAL layer
  rpmh_core_verify (RscHalConfigureCmdCompletion (drv->hw_drv, g_msg.TcsId, cmd->dependency_bmsk) == 
                    HAL_STATUS_SUCCESS);

  //enable commands
  rpmh_core_verify (RscHalToggleTCSCmd (drv->hw_drv, g_msg.TcsId, cmd_en_bmsk) == HAL_STATUS_SUCCESS);

  //configure commands
  rpmh_core_verify (RscHalSetupTCS (&g_msg) == HAL_STATUS_SUCCESS);

  //If its an AMC we have a few extra steps
  if (cmd->amc == TRUE)
  {
    //clear out IRQ
    rpmh_core_verify (RscHalClearAMCFinishedIRQ (drv->hw_drv, g_msg.TcsId) == HAL_STATUS_SUCCESS);

    //enable IRQ
    rpmh_core_verify (RscHalEnableAMCFinishedIRQ (drv->hw_drv, g_msg.TcsId) == HAL_STATUS_SUCCESS);

    //trigger AMC
    rpmh_core_verify (RscHalTriggerTCS (drv->hw_drv, g_msg.TcsId) == HAL_STATUS_SUCCESS); 
    
    //save the time
    rpmh_tcs[drv_index][tcs_index].time_last_sent = rpmh_get_timestamp();

    RPMH_LOG_2("\tTCS SENT (TCS INDEX: %d) (TCS_ID: %d)", tcs_index, g_msg.TcsId);
  }
}

/**
 * <!-- tcs_send -->
 *
 * @brief Consume an AMC and send it
 *
 * @param cmd    : cmd to send if possible
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
boolean tcs_send (rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id)
{
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (drv->modes[tcs_mode[drv_index]]->num_amcs > 0);
  for (int i = 0; i < drv->modes[tcs_mode[drv_index]]->num_amcs; i++)
  {
    if (rpmh_tcs[drv_index][i].state == TCS_AMC_IDLE)
    {
      rpmh_tcs[drv_index][i].state = TCS_AMC_WAIT_FOR_DONE;    
      tcs_send_internal (cmd, drv_id, i);
      return TRUE;
    }
  }    

  return FALSE;
}

/**
 * <!-- tcs_clean_sleep_wake -->
 *
 * @brief Clean up the TCSs for the given set. Used for sleep/wake AMCs ONLY
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
void tcs_clean_sleep_wake (RSCSW_DRV_MAPPING drv_id)
{
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  for (int i = drv->modes[tcs_mode[drv_index]]->sleep_start; i < drv->num_tcs; i++)
  { 
    tcs_t *tcs = &(rpmh_tcs[drv_index][i]);
    if ((tcs->cmds != NULL) && (tcs->cmds->num_rcs > 0))
    {
      rpmh_core_verify((tcs->cmds->set == RPMH_SET_SLEEP) || (tcs->cmds->set == RPMH_SET_WAKE));	   
      rpmh_clean_cmd (tcs->cmds, drv->num_cmds);
    }
  }
}

/**
 * <!-- tcs_prepare_sleep_commit -->
 *
 * @brief Ready for sleep, commit the sleep and wake TCS config
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
void tcs_prepare_sleep_commit (RSCSW_DRV_MAPPING drv_id)
{
  int i;	
  uint32 drv_index = lookup_drv_index(drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  for (i = drv->modes[tcs_mode[drv_index]]->sleep_start; i < drv->num_tcs; i++)
  { 
    tcs_t *tcs = &(rpmh_tcs[drv_index][i]);
    if ((tcs->cmds != NULL) && (tcs->cmds->num_rcs > 0))
    {
      tcs_send_internal (tcs->cmds, drv_id, i);
    }
  }
}

#ifdef RPMH_OFFTARGET_TEST

/**
 * <!-- print_drv -->
 *
 * @brief Print a single DRV config and current TCS config
 *
 * @param RSC+DRV id to print
 */
void print_drv (RSCSW_DRV_MAPPING drv_id)
{
  int i;	
  uint32 drv_index = lookup_drv_index (drv_id);
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  printf ("\tDRV_TABLE_INDEX: %d\n", drv_index);      
  printf ("\tDRV_ID: %d\n", drv->drv_id);      
  printf ("\tDRV_NUM_TCS: %d\n", drv->num_tcs);      
  printf ("\tDRV_NUM_AMCS: %d\n", drv->num_amcs);      
  printf ("\tDRV_NUM_CMDS: %d\n", drv->num_cmds);      
  for (i = 0; i < drv->num_tcs; i++)
  {
    printf ("\t\tTCS_TABLE_INDEX: %d\n", i);      
    uint32 id = rpmh_tcs[drv_index][i].id;  
    uint32 state = rpmh_tcs[drv_index][i].state;  
    printf ("\t\tTCS ID: %d\n", id);      
    printf ("\t\tTCS STATE: %d\n", state);      
    print_cmd_list (rpmh_tcs[drv_index][i].cmds);
  }
}

/**
 * <!-- print_drvs -->
 *
 * @brief Prints DRV config and current TCS config for each supported DRV
 */
void print_drvs(void)
{
  int i;	
  for (i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[i]; 
    print_drv (drv->drv_id);	    
  }
}
#endif //RPMH_OFFTARGET_TEST

