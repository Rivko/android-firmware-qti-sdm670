/*============================================================================
FILE:         urpmh.c

OVERVIEW:     This file provides the master rpm driver functionality for the
uImage on ADSP.

DEPENDENCIES: 

===========================================================================
Copyright (c) 2013 - 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/urpmh/common/urpmh.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/
#include "urpmh.h"
#include "urpmh_rsc_hwio.h"
#include "urpmh_os.h"
#include "HALhwio.h"
#include "pdcTcs.h"
#include "qurt.h"

//command sets populated by the normal context
urpmh_command_set_t urpmh_cmd_sets[NUM_URPMH_CMD_SETS];
qurt_mutex_t g_uRPMhMutex;

boolean island_mode = FALSE;

#ifdef USES_HWSOLVER
#include "unpa.h"
//clients to disable modes at the hwsolver 
//used in normal context, but needs to be accessible in island.
unpa_client rpmh_hwsolver_client[TCS_TOTAL_RESOURCE_NUM];
#endif

//bitmask to track which TCSs are in flight
uint32 tcs_in_flight_bmsk = 0;
uint32 urpmh_num_cmds_allowed;

/**
 * <!-- urpmh_finish_amc -->
 *
 * @brief Finish the specified AMC  
 *
 * @param tcs_index : the tcs index to finish
 * @param read      : whether or not the command is a read
 *
 * @return the read data if it was a read command, 0 if not
 */ 
uint32 urpmh_finish_amc(uint32 tcs_index, boolean read)
{
  uint32 ret_data = 0;	
  uint32 completion_status = 0;
  uint32 reg_val = 0;
  uint32 tcs_bmsk = (1 << tcs_index);
  URPMH_CORE_VERIFY((tcs_in_flight_bmsk & tcs_bmsk) != 0);

  //wait for the TCS to complete
  while (completion_status == 0)
  {
    //read the completion status	  
    reg_val = HWIO_INF(SSC_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0, AMC_COMPLETION_IRQ_STATUS);

    //check for this specific TCS	    
    completion_status = (tcs_bmsk & (reg_val));   
  }

  if(read)
  {
    ret_data = HWIO_INI2(SSC_RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA, tcs_index, 0);
  }

  //clear the IRQ
  HWIO_OUT(SSC_RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0, tcs_bmsk);

  //clear out this tcs as in flight
  tcs_in_flight_bmsk &= (~(tcs_bmsk));

  return ret_data;
}

/**
 * <!-- urpmh_send -->
 *
 * @brief Send the given command set  
 *
 * @param cmd_set   : the command set to send
 * @param tcs_index : the tcs index to use to send the command
 * @param type      : the type of command we are sending
 */ 
static void urpmh_send (rpmh_command_set_internal_t *cmd_set, uint32 tcs_index, urpmh_cmd_set_type_enum type)
{
  uint16 cmd_en_msk = 0;	    

  //make sure the tcs is idle
  URPMH_CORE_VERIFY (1 == HWIO_INFI(SSC_RSCC_TCSm_DRV0_STATUS, tcs_index, CONTROLLER_IDLE));

  //copy cmd set details to TCS
  for (int j=0; j<cmd_set->num_commands; j++)
  {
    rpmh_command_t *cmd = &(cmd_set->commands[j]);      
    //save the command data
    HWIO_OUTI2 (SSC_RSCC_TCSm_CMDn_DRV0_DATA, tcs_index, j, cmd->data);    

    //save the command address
    HWIO_OUTFI2 (SSC_RSCC_TCSm_CMDn_DRV0_ADDR, tcs_index, j, SLV_ID, SLAVE_ID(cmd->address));	    
    HWIO_OUTFI2 (SSC_RSCC_TCSm_CMDn_DRV0_ADDR, tcs_index, j, OFFSET, OFFSET(cmd->address));    

    uint32 write = (type == ISLAND_READ_AMC) ? 0 : 1;
    //save the details - read or write? response required or fire and forget? msg_length?
    HWIO_OUTFI2 (SSC_RSCC_TCSm_CMDn_DRV0_MSGID, tcs_index, j, READ_OR_WRITE, write);    
    HWIO_OUTFI2 (SSC_RSCC_TCSm_CMDn_DRV0_MSGID, tcs_index, j, RES_REQ, cmd->completion);    
    HWIO_OUTFI2 (SSC_RSCC_TCSm_CMDn_DRV0_MSGID, tcs_index, j, MSG_LENGTH, 8);    
    cmd_en_msk |= (1 << j);
  }

  //write the cmd enables
  HWIO_OUTFI (SSC_RSCC_TCSm_DRV0_CMD_ENABLE, tcs_index, INDIVIDUAL_CMD_ENABLE_MASK, cmd_en_msk);    

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
    else if((i - last_dependency_bit) % (urpmh_num_cmds_allowed) == 0)
    {
      cmd->dependency_bmsk |= curr_cmd_bmsk;
    }
  }
#endif //PDC_READ_WORKAROUND_V1 
  
  //write the dependency mask
  HWIO_OUTFI (SSC_RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL, tcs_index, CMD_WAIT_FOR_CMPL, cmd_set->dependency_bmsk);    

  //trigger AMC
  
  //enable the irq so that IRQ_STATUS will update in the RSC (and we can poll on this register).
  //Note that this interrupt is disabled at the subsystem interrupt controller, so it won't trigger at the subsystem level.
  uint32 enable_mask = HWIO_INF (SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0, AMC_COMPLETION_IRQ_ENABLE);    
  uint32 enable_tcs = (enable_mask | (1 << tcs_index));
  HWIO_OUTF (SSC_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0, AMC_COMPLETION_IRQ_ENABLE, enable_tcs);    

  tcs_in_flight_bmsk |= (1 << tcs_index);

  //Trigger the TCS. HPG says write to 0, then write to 1
  HWIO_OUTFI (SSC_RSCC_TCSm_DRV0_CONTROL, tcs_index, AMC_MODE_TRIGGER, 0);
  HWIO_OUTFI (SSC_RSCC_TCSm_DRV0_CONTROL, tcs_index, AMC_MODE_EN, 1);  
  HWIO_OUTFI (SSC_RSCC_TCSm_DRV0_CONTROL, tcs_index, AMC_MODE_TRIGGER, 1);    
}

/**
 * <!-- urpmh_issue_read -->
 *
 * @brief Issue a read AMC  
 *
 * @param address : the address to read
 * @param read_val : the value read at the hardware
 *
 * @return URPMH_READ_SUCCESS if completed the read successfully
 */ 
urpmh_read_enum urpmh_issue_read(uint32 address, uint32 *read_val)
{
  qurt_mutex_lock(&g_uRPMhMutex);
  if(!island_mode)
  {
    qurt_mutex_unlock(&g_uRPMhMutex);
    return URPMH_READ_FAIL_NOT_IN_ISLAND;
  }
  for (int i = 0; i < NUM_URPMH_CMD_SETS; i++)
  {
    if ((urpmh_cmd_sets[i].type == ISLAND_READ_AMC) || 
        (urpmh_cmd_sets[i].type == ISLAND_ENTER_AMC))
    {
      rpmh_command_set_internal_t *cmd_set = &(urpmh_cmd_sets[i].cmd_set);   
      urpmh_cmd_sets[i].type = ISLAND_READ_AMC;
      cmd_set->num_commands = 1;
      cmd_set->dependency_bmsk = 0;
      cmd_set->commands[0].address = address;
      cmd_set->commands[0].completion = TRUE;
      cmd_set->commands[0].data = 0;
      urpmh_send(cmd_set, urpmh_cmd_sets[i].tcs_num, ISLAND_READ_AMC);      
      *read_val = urpmh_finish_amc(urpmh_cmd_sets[i].tcs_num, TRUE);
      qurt_mutex_unlock(&g_uRPMhMutex);
      return URPMH_READ_SUCCESS;
    }
  }
  //should never get here... this means we didn't have an AMC to use
  URPMH_CORE_VERIFY(0);
  qurt_mutex_unlock(&g_uRPMhMutex);
  return URPMH_READ_FAIL_NO_CMD_SET;
}

/**
 * <!-- urpmh_send_requests -->
 *
 * @param type : The type of commands to send
 *
 * @brief Send all requests of the given type  
 */ 
static void urpmh_send_requests (urpmh_cmd_set_type_enum type)
{
  for (int i = 0; i < NUM_URPMH_CMD_SETS; i++)
  {
    if (urpmh_cmd_sets[i].type == type)
    {
      rpmh_command_set_internal_t *cmd_set = &(urpmh_cmd_sets[i].cmd_set);   
      uint32 tcs_index = urpmh_cmd_sets[i].tcs_num;
      urpmh_send(cmd_set, tcs_index, type);
    }
  }
}


/**
 * <!-- urpmh_finish_outstanding_amcs -->
 *
 * @brief Finish any pending AMCs  
 */ 
static void urpmh_finish_outstanding_amcs(void)
{
  while(tcs_in_flight_bmsk)
  {
    uint32 tcs_index = (31 - urpmh_clz((uint32)tcs_in_flight_bmsk));    
    urpmh_finish_amc(tcs_index, FALSE);
  }
}

/**
 * <!-- urpmh_island_enter -->
 *
 * @brief Called on island entry. Sends the proper island enter AMCs  
 */ 
void urpmh_island_enter()
{
  qurt_mutex_lock(&g_uRPMhMutex);
  urpmh_send_requests (ISLAND_ENTER_AMC);
  urpmh_finish_outstanding_amcs();
  qurt_mutex_unlock(&g_uRPMhMutex);
}

/**
 * <!-- urpmh_island_exit -->
 *
 * @brief Called on island exit. Sends the proper island exit AMCs  
 */ 
void urpmh_island_exit()
{
  qurt_mutex_lock(&g_uRPMhMutex);
  urpmh_finish_outstanding_amcs();
  urpmh_send_requests (ISLAND_EXIT_AMC);
  urpmh_finish_outstanding_amcs();
  qurt_mutex_unlock(&g_uRPMhMutex);
}

