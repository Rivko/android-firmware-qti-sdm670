/*===========================================================================
                              rpmh_tcs.c

DESCRIPTION:
  Provides tcs management. This includes configuration, triggering AMCs, 
  cleaning up when a TCS has finished, and retrieving read command data. This
  is the main layer which interacts with HAL. Any additional HAL related 
  interaction for RSC/TCS functionality should be put here.

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
#include "rpmh_tcs.h"
#include "comdef.h"
#include "rsc_hal.h"
#include "rpmh_os.h"
#include "rpmh_drv_config.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "CoreVerify.h"

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

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//2D array consisting of rpmh_tcs[DRVs][TCSs]
tcs_t **rpmh_tcs;
tcs_drv_cfg_t *tcs_drv_cfg;
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
void tcs_init (rpmh_cmdq_t **cmdq)
{
  uint32 num_tcs = 0;
  uint32 num_cmds = 0;

  //allocate the DRV portion of TCS lookup table
  rpmh_tcs = (tcs_t **)rpmh_malloc (sizeof (tcs_t *) * DRV_CONFIG_DATA->num_drvs);        
  rpmh_core_verify_ptr (rpmh_tcs);
  memset (rpmh_tcs, 0, sizeof(tcs_t *) * DRV_CONFIG_DATA->num_drvs);

  tcs_drv_cfg = (tcs_drv_cfg_t *)rpmh_malloc (sizeof (tcs_drv_cfg_t) * DRV_CONFIG_DATA->num_drvs);        
  rpmh_core_verify_ptr (tcs_drv_cfg);
  memset (tcs_drv_cfg, 0, sizeof(tcs_drv_cfg_t) * DRV_CONFIG_DATA->num_drvs);


  //loop through all supported DRVs since they each have their own TCS allocation
  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[i]; 
    tcs_drv_cfg[i].mode = 0;
    uint32 num_amcs = drv->modes[0]->num_amcs;
    
    //if its not a virtual DRV...
    if(drv->drv_id < RSC_DRV_VIRTUAL_DRVS)
    {
      //initialize DRV at HAL

      //verify #TCS and #CMDs in HW is what we have in SW config
      num_cmds = TCS_HWIO_INXFI(drv->hwio_base, RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd, DRV, NUM_CMDS_PER_TCS);
      num_tcs = TCS_HWIO_INXFI(drv->hwio_base, RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd, DRV, NUM_TCS_DRV0);

#ifndef RPMH_OFFTARGET_TEST
      rpmh_core_verify (num_tcs >= drv->num_tcs); 
      rpmh_core_verify (num_cmds >= drv->num_cmds);

      //num_amcs is a subset of num_tcs
      rpmh_core_verify (num_tcs >= (num_amcs));
#endif //RPMH_OFFTARGET_TEST


      //turn on ePCB timeout - only applicable to DRV 0
      if(drv->hw_drv == 0)
      {
        TCS_HWIO_OUTXF(drv->hwio_base, RSC_ERROR_IRQ_ENABLE_DRV0, EPCB_TIMEOUT_THRESHOLD, 0xFFFF);
        TCS_HWIO_OUTXF(drv->hwio_base, RSC_ERROR_IRQ_ENABLE_DRV0, EPCB_TIMEOUT_IRQ_EN, 1);
      }
    }
    
    //allocate the TCS table for this DRV
    rpmh_tcs[i] = (tcs_t *)rpmh_malloc (sizeof (tcs_t) * drv->num_tcs); 
    rpmh_core_verify_ptr (rpmh_tcs[i]);
    memset (rpmh_tcs[i], 0, sizeof(tcs_t) * drv->num_tcs);

    //for now, all DRVs should have the same # cmds per TCS
    rpmh_core_verify(drv->num_cmds == IMAGE_TCS_SIZE);

    //make sure there is room for the sleep/wake TCS if they exist
    if (num_tcs > num_amcs)
    {
      rpmh_core_verify (num_tcs >= (num_amcs + 2));
    }
    
    tcs_update_mode(i, 0, cmdq[i]);
  }
}

/**
 * <!-- tcs_update_mode -->
 *
 * @brief Update the TCS mode in use for the given DRV
 *
 * @param drv_index : The DRV (index into the DRV table) on which to switch TCS configuration
 * @param new_mode : The new mode to switch to 
 */
void tcs_update_mode(uint32 drv_index, uint32 new_mode, rpmh_cmdq_t *cmdq)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify(new_mode < drv->num_modes);	
  tcs_drv_cfg[drv_index].mode = new_mode;

  //loop through and update each TCS
  for(int i = 0; i < drv->num_tcs; i++)
  {
    tcs_t *tcs = &(rpmh_tcs[drv_index][i]);
    tcs->id = i + drv->tcs_offset;

    //clear out IRQ
    TCS_HWIO_OUTX(drv->hwio_base, TCS_AMC_MODE_IRQ_CLEAR_DRV0, (1 << tcs->id));

    tcs->time_last_sent = 0;
    if (i < drv->modes[new_mode]->num_amcs)
    {
      if(tcs->cmds)
      {
        delete_cmd(cmdq, tcs->cmds);
      }
      //mark AMCs as AMCs in hardware
      tcs->state = TCS_AMC_IDLE;
      //convert to AMC	      
      TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs->id, AMC_MODE_TRIGGER, 0);
      TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs->id, AMC_MODE_EN, 1);

      //enable IRQ	      
      uint32 val = TCS_HWIO_INX(drv->hwio_base, TCS_AMC_MODE_IRQ_ENABLE_DRV0);
      val |= (1 << tcs->id);
      TCS_HWIO_OUTXF(drv->hwio_base, TCS_AMC_MODE_IRQ_ENABLE_DRV0, AMC_COMPLETION_IRQ_ENABLE, val);
    }
    else
    {
      if(tcs->state != TCS_NON_AMC)
      {
        tcs->cmds = new_cmd(cmdq);
      }	      
      //mark TCSs as TCSs in hardware
      tcs->state = TCS_NON_AMC;

      //disable IRQ	      
      uint32 val = TCS_HWIO_INX(drv->hwio_base, TCS_AMC_MODE_IRQ_ENABLE_DRV0);
      val &= ~(1 << tcs->id);
      TCS_HWIO_OUTXF(drv->hwio_base, TCS_AMC_MODE_IRQ_ENABLE_DRV0, AMC_COMPLETION_IRQ_ENABLE, val);

      //convert to TCS	      
      TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs->id, AMC_MODE_TRIGGER, 0);
      TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs->id, AMC_MODE_EN, 0);
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
 * @param drv_index : The DRV (index into the DRV table) to check for a stuck TCS 
 *
 * @return TRUE if a tcs is stuck, FALSE if not
 */
boolean is_tcs_stuck (uint32 drv_index)
{
  uint32 cntr_idle;
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  uint32 bmsk = tcs_drv_cfg[drv_index].busy_bmsk;
  while(bmsk != 0)
  {
    uint32 tcs_index = (31 - rpmh_clz((uint32)bmsk));    
    uint32 tcs_id = tcs_index + drv->tcs_offset;

    //hpg says read twice
    cntr_idle = TCS_HWIO_INXI(drv->hwio_base, TCSm_DRV0_STATUS, tcs_id);
    cntr_idle = TCS_HWIO_INXI(drv->hwio_base, TCSm_DRV0_STATUS, tcs_id);

    uint64 current_time = rpmh_get_timestamp ();   
    if ((1 & cntr_idle) && (current_time > (rpmh_tcs[drv_index][tcs_index].time_last_sent + TCS_TIMEOUT_THRESHOLD)))
    {
      RPMH_LOG_2 (rpmh_log_tcs_stuck, drv->drv_id, tcs_id);      
      return TRUE;
    }

    bmsk &= ~(1 << tcs_index);
  }
  return FALSE;
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
 * <!-- is_amc_free -->
 *
 * @brief let the caller know if an AMC is available for the given DRV
 *
 * @param drv_index : The DRV (index into the DRV table) to check for a free AMC 
 *
 * @return TRUE if an AMC is free
 */
boolean is_amc_free (uint32 drv_index)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  uint32 num_amc_bmsk = ((1 << drv->modes[tcs_drv_cfg[drv_index].mode]->num_amcs) - 1); 

  if(num_amc_bmsk == tcs_drv_cfg[drv_index].busy_bmsk)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
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
  status = TCS_HWIO_INXF(drv->hwio_base, TCS_AMC_MODE_IRQ_STATUS_DRV0, AMC_COMPLETION_IRQ_STATUS);

  return (status != 0);
}

/**
 * <!-- get_finished_tcs -->
 *
 * @brief Checks if any DRV has finished for the given hw_drv. This is used for virtual DRVs to
 *        determine if a virtual DRV has completed or a normal DRV has completed, because virtual
 *        DRVs are a software concept only and won't show up in the RSC registers
 *
 * @param hw_drv : The HW DRV to check 
 * @param drv_index     : The DRV (index into the DRV table) corresponding to the HW DRV 
 * @param ret_tcs_index : The TCS index that finished 
 *
 * @return The DRV ID that has finished an AMC 
 */
RSCSW_DRV_MAPPING get_finished_tcs(RSCSW_DRV_MAPPING hw_drv, uint32 drv_index, uint32 *ret_tcs_index)
{
  uint32 status;
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  status = TCS_HWIO_INXF(drv->hwio_base, TCS_AMC_MODE_IRQ_STATUS_DRV0, AMC_COMPLETION_IRQ_STATUS);

  if (status != 0)
  {
    uint32 tcs_id = (31 - rpmh_clz((uint32)status));    
    for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
    {
      drv = &DRV_CONFIG_DATA->drvs[i]; 
      int tcs_index = tcs_id - drv->tcs_offset;
      if((drv->hw_drv == hw_drv) && (tcs_index >= 0) && (tcs_index < drv->modes[tcs_drv_cfg[drv_index].mode]->num_amcs)) 
      {
        *ret_tcs_index = tcs_index;	      
        return drv->drv_id;
      }
    }
  }
  return RSC_DRV_VIRTUAL_MAX;
}

/**
 * <!-- get_amc -->
 *
 * @brief retrieves the given TCS index for the given DRV ID
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return pointer to the finished AMC if one finished, NULL if not 
 */
static tcs_t *get_amc (uint32 drv_index, uint32 tcs_index)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  rpmh_core_verify (tcs_index <= drv->num_tcs);
  rpmh_core_verify (rpmh_tcs[drv_index][tcs_index].state == TCS_AMC_WAIT_FOR_DONE);
  return &rpmh_tcs[drv_index][(tcs_index)];
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
static inline void tcs_clean_up (rpmh_cmdq_t *cmdq, tcs_t *finished_amc, RSCSW_DRV_MAPPING drv_id, uint32 drv_index, rpmh_set_enum set)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  //clear out IRQ
  TCS_HWIO_OUTX(drv->hwio_base, TCS_AMC_MODE_IRQ_CLEAR_DRV0, (1 << finished_amc->id));

  //clean up at the queue level
  cmdq_finish_cmd (cmdq, finished_amc->cmds, drv_id, finished_amc->id, set);

  //mark the AMC as idle and clear out its contents
  finished_amc->state = TCS_AMC_IDLE;
  tcs_drv_cfg[drv_index].busy_bmsk &= ~(1 << (finished_amc->id - drv->tcs_offset));
  finished_amc->cmds = NULL;
  //RPMH_LOG_1(rpmh_log_tcs_idle, finished_amc->id);
}

/**
 * <!-- tcs_finish_amc -->
 *
 * @brief A TCS has completed, clean it up
 *
 * @param cmdq   : cmd queue for the given DRV. This is just a passthrough to the cmdq layer 
 * @param drv_id : RSC+DRV id 
 * @param drv_index : The DRV (index into the DRV table) corresponding to the drv_id 
 * @param set    : set associated with the AMC  (could be sleep/wake AMCs)
 */
void tcs_finish_amc (rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id, uint32 drv_index, rpmh_set_enum set)
{
  uint32 status;
  tcs_t *finished_amc; 
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  status = TCS_HWIO_INXF(drv->hwio_base, TCS_AMC_MODE_IRQ_STATUS_DRV0, AMC_COMPLETION_IRQ_STATUS);

  if (status != 0)    
  {    
    uint32 tcs_index = (31 - rpmh_clz((uint32)status)) - (drv->tcs_offset);    

    finished_amc = get_amc(drv_index, tcs_index);

    //make sure we found a valid AMC
    rpmh_core_verify_ptr (finished_amc->cmds);
    
    tcs_clean_up (cmdq, finished_amc, drv_id, drv_index, set);
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
  data = TCS_HWIO_INXI2(drv->hwio_base, TCSm_CMDn_DRV0_READ_RESPONSE_DATA, rpmh_tcs[drv_index][tcs_id].id, req_id);
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
 * @param drv_index : The DRV (index into the DRV table) corresponding to the drv_id 
 * @param tcs_index : index of the finished TCS 
 *
 * @return the command ID of the finished amc
 */
uint32 tcs_finish_active_amc (rpmh_client_t **client, rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id, uint32 drv_index, uint32 tcs_index)
{
  uint32 req_id = 0;
  tcs_t *finished_amc = get_amc (drv_index, tcs_index); 

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

  tcs_clean_up (cmdq, finished_amc, drv_id, drv_index, RPMH_SET_ACTIVE);

  //return the msg id of the completed request
  return req_id;
}

/**
 * <!-- tcs_send_internal -->
 *
 * @brief The guts of sending a TCS (AMC or non-AMC)
 *
 * @param cmd    : cmd to send 
 * @param drv_id : RSC+DRV id 
 * @param tcs_id : index into the TCS list to indicate which TCS to use
 */
static void tcs_send_internal (rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id, uint32 drv_index, uint32 tcs_index)
{
  int i;
  uint32 cmd_en_bmsk = 0;
  uint32 controller_idle = 0;
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 

  //make sure # commands is valid
  rpmh_core_verify (cmd->num_rcs <= drv->num_cmds);

  //save a pointer to the command for this AMC
  rpmh_tcs[drv_index][tcs_index].cmds = cmd;

  //set up the AMC header
  uint32 tcs_id = rpmh_tcs[drv_index][tcs_index].id;

  //make sure TCS is idle (it should be per our internal SW state)
  controller_idle = TCS_HWIO_INXI(drv->hwio_base, TCSm_DRV0_STATUS, tcs_id);
  controller_idle = TCS_HWIO_INXI(drv->hwio_base, TCSm_DRV0_STATUS, tcs_id);
  rpmh_core_verify(controller_idle & 1);

  CORE_VERIFY(cmd->num_rcs <= IMAGE_TCS_SIZE);

  //configure commands
  for (i = 0; i < cmd->num_rcs; i++)
  {
    cmd_en_bmsk |= (1 << i);

    //MSGID
    TCS_HWIO_OUTXI2(drv->hwio_base, TCSm_CMDn_DRV0_MSGID, tcs_id, i, MSG_ID(AMC_WRITE, cmd->details[i].type, MSG_SIZE));
    //ADDR
    TCS_HWIO_OUTXI2(drv->hwio_base, TCSm_CMDn_DRV0_ADDR,  tcs_id, i, cmd->details[i].address);
    //DATA
    TCS_HWIO_OUTXI2(drv->hwio_base, TCSm_CMDn_DRV0_DATA,  tcs_id, i,
                rpmh_resource_command_val_at_rpmh (cmd->details[i].address, drv_id, cmd->set, cmd->amc));
  }

  //if there's a dependency listed, notify the HAL layer
  TCS_HWIO_OUTXI(drv->hwio_base, TCSm_DRV0_CMD_WAIT_FOR_CMPL, tcs_id, cmd->dependency_bmsk);

  //enable commands
  TCS_HWIO_OUTXI(drv->hwio_base, TCSm_DRV0_CMD_ENABLE, tcs_id, cmd_en_bmsk);

  //If its an AMC we have a few extra steps
  if (cmd->amc == TRUE)
  {
    //clear out IRQ
    TCS_HWIO_OUTX(drv->hwio_base, TCS_AMC_MODE_IRQ_CLEAR_DRV0,  (1 << tcs_id));

    //IRQ should be enabled, but confirm
    rpmh_core_verify(((1 << tcs_id) & TCS_HWIO_INX(drv->hwio_base, TCS_AMC_MODE_IRQ_ENABLE_DRV0)) != 0);

    //trigger AMC
    //set 0
    TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs_id, AMC_MODE_TRIGGER, 0);

    //set 1
    TCS_HWIO_OUTXFI(drv->hwio_base, TCSm_DRV0_CONTROL,  tcs_id, AMC_MODE_TRIGGER, 1);
    
    //save the time
    rpmh_tcs[drv_index][tcs_index].time_last_sent = rpmh_get_timestamp();

    //RPMH_LOG_2(rpmh_log_tcs_sent, tcs_index, tcs_id);
  }
}

/**
 * <!-- tcs_send -->
 *
 * @brief Consume an AMC and send it
 *
 * @param cmd       : cmd to send if possible
 * @param drv_id    : RSC+DRV id 
 * @param drv_index : The DRV (index into the DRV table) corresponding to the drv_id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
boolean tcs_send (rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id, uint32 drv_index)
{
  const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[drv_index]; 
  uint32 num_amc_bmsk = ((1 << drv->modes[tcs_drv_cfg[drv_index].mode]->num_amcs) - 1); 
  uint32 idle_bmsk = ((~(tcs_drv_cfg[drv_index].busy_bmsk)) & num_amc_bmsk);

  if(idle_bmsk != 0)
  {
    uint32 tcs_index = (31 - rpmh_clz((uint32)idle_bmsk));    

    rpmh_core_verify (rpmh_tcs[drv_index][tcs_index].state == TCS_AMC_IDLE);
    rpmh_tcs[drv_index][tcs_index].state = TCS_AMC_WAIT_FOR_DONE;    
    tcs_drv_cfg[drv_index].busy_bmsk |= (1 << tcs_index);
    tcs_send_internal (cmd, drv_id, drv_index, tcs_index);
    return TRUE;
  }


  return FALSE;
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

