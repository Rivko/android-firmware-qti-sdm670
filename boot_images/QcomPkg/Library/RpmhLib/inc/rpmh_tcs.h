#ifndef RPMH_TCS_H
#define RPMH_TCS_H

/*===========================================================================
                             rpmh_tcs.h

DESCRIPTION:
  Interface for interacting with TCSs. This includes managing TCS state, 
  configuring TCSs, and handling TCS completion.  

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "rpmh_resource_commands.h"
#include "rpmh_cmdq.h"
#include "rpmh_client_internal.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define SLAVE_ID(address) address >> 0x10  
#define OFFSET(address) address & 0xFFFF 
#define TCS_TIMEOUT_THRESHOLD 0x2EE00 //10mS in qtmr ticks

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/* 
 * tcs states 
 */
typedef enum
{
  TCS_AMC_IDLE = 0,
  TCS_AMC_WAIT_FOR_DONE,
  TCS_AMC_DONE,
  TCS_NON_AMC,
  TCS_NUM_STATES    
} tcs_state_enum;

/* 
 * tcs details 
 */
typedef struct
{
  uint8                   id;    //tcs ID
  tcs_state_enum          state; //tcs state
  uint64                  time_last_sent;
  rpmh_cmd_t              *cmds; //commands for this tcs
} tcs_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- tcs_init -->
 *
 * @brief Initialize internal TCS structures as well as TCS hardware
 */
void tcs_init (void);

#ifdef RPMH_SENSORS_ISLAND
/**
 * <!-- tcs_toggle_island_mode -->
 *
 * @brief Controls whether or not we are about to enter island. If we are about
 *   to enter island mode, the TCS layer sends sleep/wake TCS information to 
 *   island SW instead of directly to the TCS HW. 
 *
 * @param island_en : TRUE if we are preparing for island mode entry
 */
void tcs_toggle_island_mode(boolean island_en);
#endif //RPMH_SENSORS_ISLAND

/**
 * <!-- drv_valid -->
 *
 * @brief Check whether or not a DRV is valid (is supported by this context)
 *
 * @param drv_id : RSC+DRV id for validity check
 *
 * @return whether or not the given DRV is valid
 */
boolean drv_valid (RSCSW_DRV_MAPPING drv_id);


/**
 * <!-- tcs_update_mode -->
 *
 * @brief Update the TCS mode in use for the given DRV
 *
 * @param drv_id   : The DRV on which to switch TCS configuration
 * @param new_mode : The new mode to switch to 
 */
void tcs_update_mode(uint32 drv_index, uint32 new_mode);

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
RSCSW_DRV_MAPPING get_finished_drv(RSCSW_DRV_MAPPING drv_id);

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
boolean is_tcs_stuck (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- is_amc_free -->
 *
 * @brief let the caller know if an AMC is available for the given DRV
 *
 * @param drv_id : DRV to check for free AMCs 
 *
 * @return TRUE if an AMC is free
 */
boolean is_amc_free (RSCSW_DRV_MAPPING drv_id);

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
uint32 tcs_finish_active_amc (rpmh_client_t **client, rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- tcs_finish_amc -->
 *
 * @brief A TCS has completed, clean it up
 *
 * @param cmdq   : cmd queue for the given DRV. This is just a passthrough to the cmdq layer 
 * @param drv_id : RSC+DRV id 
 * @param set    : set associated with the AMC  (could be sleep/wake AMCs)
 */
void tcs_finish_amc (rpmh_cmdq_t *cmdq, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set);

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
boolean tcs_send (rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- lookup_drv_valid -->
 *
 * @brief Get the index into the DRV config table for the given ID
 *
 * @param RSC+DRV id 
 *
 * @return index into DRV config table
 */
int32 lookup_drv_index (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- is_amc_finished -->
 *
 * @brief Checks if an AMC is finished at the RSC level for the given ID
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if an AMC is finished, FALSE if not 
 */
boolean is_amc_finished (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- tcs_prepare_sleep_add_cmd_set -->
 *
 * @brief Add a resource command to a TCS
 *
 * @param cmd_set : command set to add to a tcs
 * @param drv_id  : RSC+DRV id 
 *
 * @return TRUE if we were able to add the command, FALSE if not (no room in TCSs)
 */
boolean tcs_prepare_sleep_add_cmd_set (rpmh_command_set_internal_t *cmd_set, RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- tcs_prepare_sleep_commit -->
 *
 * @brief Ready for sleep, commit the sleep and wake TCS config
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
void tcs_prepare_sleep_commit (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- tcs_clean_sleep_wake -->
 *
 * @brief Clean up the TCSs for the given set. Used for sleep/wake AMCs ONLY
 *
 * @param drv_id : RSC+DRV id 
 *
 * @return TRUE if the TCS was able to be sent, FALSE if not
 */
void tcs_clean_sleep_wake (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- tcs_slot_available -->
 *
 * @brief Retrieve slots in a corresponding TCS for the given ID and set
 *
 * @param drv_id    : RSC+DRV id 
 * @param set       : set on which to check for TCS space
 * @param num_slots : number of sots needed
 *
 * @return tcs index which has room. -1 if there is no room
 */
int tcs_slots_available (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set, uint32 num_slots);

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
uint32 tcs_get_read_data (RSCSW_DRV_MAPPING drv_id, uint8 tcs_id, uint32 req_id);

/**
 * <!-- tcs_size -->
 *
 * @brief Return the # commands per TCS for this DRV
 *
 * @param drv_id : RSC+DRV id for num cmds check
 *
 * @return number of commands in the TCSs for this DRV
 */
uint32 tcs_size (RSCSW_DRV_MAPPING drv_id);

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_drv -->
 *
 * @brief Print a single DRV config and current TCS config
 *
 * @param RSC+DRV id to print
 */
void print_drv (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- print_drvs -->
 *
 * @brief Prints DRV config and current TCS config for each supported DRV
 */
void print_drvs (void);
#endif //RPMH_OFFTARGET_TEST

#endif // RPMH_TCS_H

