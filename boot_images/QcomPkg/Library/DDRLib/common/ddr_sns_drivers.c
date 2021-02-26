/*
===========================================================================

FILE:         ddr_sns_drivers.c

DESCRIPTION:  
  This is the ddr drivers implementation for Qualcomm memory controller.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
07/18/14   tw      added support for passing highest bank bit information to ram
                   partition table
05/28/14   tw      Renamed lp4_share_data to ddrsns_share_data for code clarity
03/12/14   sr      Initial version

===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_sync.h"
#include "ddr_params.h"
#include "HAL_SNS_DDR.h"
#include "ddr_hwio.h"
#include "ddrss_init_sdi.h"
#include "boothw_target.h"
#include "ddr_target.h"
#include "ddr_sdi_log.h"
#include "boot_fastcall_tz.h"

/*==============================================================================
                                  DATA
==============================================================================*/
ddr_interface_state ddr_status;

extern DDR_SDI_LOGS *ddr_sdi_logs_ptr;

/*Indicates ddr initialization is done or not, FALSE by default*/
boolean ddr_init_done = FALSE;

/* Chip selects for selected ranks by ddr_enter/exit_self_refresh_all() */
static DDR_CHIPSELECT ddr_csel[DDR_MAX_NUM_CH];

ddr_info ddr_physical_size = {{{0}}};
ddr_size_info ddr_system_size = {{0}};

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ============================================================================
**  Function : ddr_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize ddr HAL with the correct ddr parameters
*   This must be done before calling any of the drivers functions
*   
*   @details
*   Initialize ddr HAL with the correct ddr parameters
*   This must be done before calling any of the drivers functions
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_init(void)
{
  DDR_SYNC_INIT();
  
  /* Initialize target specific functions */
  ddr_target_init();

} /* ddr_init */

void Pass3_WA(void)
{  
  ddr_sdi_logs_ptr->pass3_cookie->p_cookie_value = DDR_WENT_INTO_PASS3;
  ddr_sdi_logs_ptr->pass2_cookie->p_cookie_value = UNUSED;
  HWIO_OUTF(AOSS_CC_RESET_DEBUG_ENABLE, ALLOW_SW_BASED_RST_DBG_EN, 0);
  boot_set_pshold_reset_type(BOOT_WARM_RESET_TYPE);
  boot_hw_reset(BOOT_WARM_RESET_TYPE);
}

/* ============================================================================
**  Function : ddr_initialize_device
** ============================================================================
*/
/*!
*   @brief
*   This function will setup the ddr parameter in HAl and initialize the ddr
*   
*   @details
*   This function will first pass the currently used ddr device parameter
*   to the HAL. Then it will initialize the ddr as per specified speed
*   
*   @param clkspeed   -  [IN] clk speed to initialize for
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_initialize_device(uint32 clk_speed, boolean dload_mode)
{
  struct detected_ddr_device_params *ddr_detected_param_interface[DDR_MAX_NUM_CH];
  uint8 i;
  boolean freeze_io_set, freeze_io_not_set;
  char ddr_log_string[200];

  if(!ddr_init_done)
  {    
    /* Pass the ddr parameters to HAL */
    ddr_init();

    /* Update the ddr clock speed variable to the initialization clock */
    ddr_status.clk_speed = clk_speed;
    
    /* To-do: What are the possible cases for channel and chip select
    Here are the possible ones but need to sync up with DSF to see how they're handling them
    
    1. Channel: CH0, Rank: CS0
    2. Channel: CH0, Rank: CS_BOTH
    3. Channel: CH0 and CH1, Rank: CS0
    4. Channel: CH0 and CH1, Rank: CS_BOTH
    5. Channel: CH0, CH1 and CH2, Rank: CS0
    6. Channel: CH0, CH1 and CH2, Rank: CS_BOTH
    7. Channel: CH0, CH1, CH2 and CH3, Rank: CS0
    8. Channel: CH0, CH1, CH2 and CH3, Rank: CS_BOTH */

    /* If download mode is not set, i.e. cold boot path or if we have entered pass 3, then we need to fully initialize the DDR driver */
    if ((!dload_mode) || (ddr_sdi_logs_ptr->pass3_cookie->p_cookie_value == DDR_WENT_INTO_PASS3))
    {
      HAL_DDR_Init(ddrsns_share_data, DDR_CH_ALL, DDR_CS_BOTH, clk_speed);
    }
    else
    {
      freeze_io_set = HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_0) && HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_1);
                         
      freeze_io_not_set = !(HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_0) || HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_1));
                          
      snprintf(ddr_log_string, 200, "Freeze IOs = 0x%x, 0x%x, DBG PDC Tr = 0x%x, First Pass expire = 0x%x, First Pass complete = 0x%x",
               (HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_0)), (HWIO_INF(DDRCC_CH01_DDRCC_PCC_PC_STA, PC_FREEZE_IO_1)), 
               (HWIO_INF(AOSS_CC_RESET_FSM_STATUS, DBG_PDC_TRIGGER)), (HWIO_INF(AOSS_CC_RESET_FSM_STATUS, FIRST_PASS_EXPIRE)), (HWIO_INF(AOSS_CC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE)));
               
      boot_log_message(ddr_log_string);
                         
      if (HWIO_INF(AOSS_CC_RESET_FSM_STATUS, FIRST_PASS_EXPIRE) && freeze_io_not_set)
      {
        Pass3_WA();
      }      
      else if ((HWIO_INF(AOSS_CC_RESET_FSM_STATUS, DBG_PDC_TRIGGER)) && freeze_io_not_set)
      {
        ddr_sdi_logs_ptr->pass2_cookie->p_cookie_value = DDR_FULLY_INIT_PASS2_START;
        ddr_sdi_logs_ptr->pass3_cookie->p_cookie_value = UNUSED;
        HAL_DDR_Init(ddrsns_share_data, DDR_CH_ALL, DDR_CS_BOTH, clk_speed);
        ddr_sdi_logs_ptr->pass2_cookie->p_cookie_value = DDR_FULLY_INIT_PASS2_COMPLETE;
      }
      else if ((HWIO_INF(AOSS_CC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE) || 
                HWIO_INF(AOSS_CC_RESET_FSM_STATUS, FIRST_PASS_EXPIRE)) && 
                freeze_io_set)
      {
        ddr_sdi_logs_ptr->pass2_cookie->p_cookie_value = DDR_RECOVER_PASS2_START;
        ddr_sdi_logs_ptr->pass3_cookie->p_cookie_value = UNUSED;
        HAL_DDR_SDI_Recover(ddrsns_share_data->shrm_ddr_ptr, DDR_CH_ALL, DDR_CS_BOTH, clk_speed);
        ddr_sdi_logs_ptr->pass2_cookie->p_cookie_value = DDR_RECOVER_PASS2_COMPLETE;
      }
      else
      {
        Pass3_WA();
      }
    }

    /* Get DDR device parameters after detection during initialization and update the DDR status */
    for ( i = 0; i < DDR_MAX_NUM_CH; i++)
    {
      ddr_detected_param_interface[i] = (ddr_get_detected_params(1<<i));
      
			//To do: Add a comment for num_rows 0 and 1
      ddr_status.ddr_cs0[i] = (ddr_detected_param_interface[i]->num_rows[0] != 0) ? DDR_ACTIVE : DDR_UNAVAILABLE;
      ddr_status.ddr_cs1[i] = (ddr_detected_param_interface[i]->num_rows[1] != 0) ? DDR_ACTIVE : DDR_UNAVAILABLE;
    }                          
    
    ddr_update_info();
    ddr_update_size();

    ddr_init_done = TRUE;
  }
  /* Else, already initialized ddr, do nothing */
} /* ddr_initialize_device */

/* =============================================================================
**  Function : ddr_enter_self_refresh_all
** =============================================================================
*/
/**
*   @brief
*   Put DDR into self refresh mode for all active ranks.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_enter_self_refresh_all(uint32 clk_speed)
{
  uint8 i;
  DDR_CHANNEL interface_name;
  
  DDR_SYNC_LOCK();
  
  /* Update DDR status and enter self refresh for selected ranks and channels */
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    interface_name = (1 << i);
    ddr_csel[i] = DDR_CS_NONE;
    
    if (ddr_status.ddr_cs0[i] == DDR_ACTIVE)
    {
      ddr_status.ddr_cs0[i] = DDR_SELF_REFRESH;
      ddr_csel[i] |= DDR_CS0;
    }
    if (ddr_status.ddr_cs1[i] == DDR_ACTIVE)
    {
      ddr_status.ddr_cs1[i] = DDR_SELF_REFRESH;
      ddr_csel[i] |= DDR_CS1;
    }
    /* Enter self refresh for selected ranks */
    if (ddr_csel[i] != DDR_CS_NONE)
    {
      HAL_DDR_Enter_Self_Refresh(ddrsns_share_data,interface_name,  ddr_csel[i]);
    }
  }

  DDR_SYNC_UNLOCK();

} /* ddr_enter_self_refresh_all */

/* =============================================================================
**  Function : ddr_exit_self_refresh_all
** =============================================================================
*/
/**
*   @brief
*   Take DDR out of self refresh mode for ranks that are active before
*   ddr_enter_self_refresh_all() is called.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_exit_self_refresh_all(uint32 clk_speed)
{
  uint8 i;
  DDR_CHANNEL interface_name;
  
  DDR_SYNC_LOCK();

  /* Update DDR status and exit self refresh for selected ranks and channels */
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    interface_name = (1 << i);
    ddr_csel[i] = DDR_CS_NONE;
    
    if (ddr_status.ddr_cs0[i] == DDR_ACTIVE)
    {
      ddr_status.ddr_cs0[i] = DDR_SELF_REFRESH;
       ddr_csel[i] |= DDR_CS0;
    }
    if (ddr_status.ddr_cs1[i] == DDR_ACTIVE)
    {
      ddr_status.ddr_cs1[i] = DDR_SELF_REFRESH;
       ddr_csel[i] |= DDR_CS1;
    }
    /* Enter self refresh for selected ranks */
    if ( ddr_csel[i] != DDR_CS_NONE)
    {
      HAL_DDR_Enter_Self_Refresh(ddrsns_share_data,interface_name,  ddr_csel[i]);
    }
  }

  DDR_SYNC_UNLOCK();

} /* ddr_exit_self_refresh_all */

/* ============================================================================
**  Function : ddr_enter_self_refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the a particular ddr into self refresh mode, provided that it is currently active.
*   
*   @details
*   This function will first check to see if the ddr specified by the caller is available and active, then put it into self
*   refresh mode. If it is not active or not available, simply do nothing.
*   
*   @param interface_name   - [IN] The interface to put ddr into self refresh
*   @param chip_select      -     [IN] The chip select to put ddr into self refresh
*   @param clkspeed      -        [IN] The current clock speed

*
*   @par Dependencies
*   This code has to reside in IRAM
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_enter_self_refresh(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
   ddrsns_enter_self_refresh(interface_name,chip_select,clk_speed);
}

void ddrsns_enter_self_refresh(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
  DDR_CHIPSELECT csel = DDR_CS_NONE;
  uint8 i = 0;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();
 
  /* update ddr status */ 
  for(i = 0; i < DDR_MAX_NUM_CH; i++) {
    if( 0==i && 0==(interface_name&DDR_CH0) ) continue;
    if( 1==i && 0==(interface_name&DDR_CH1) ) continue;
    if( 2==i && 0==(interface_name&DDR_CH2) ) continue;
    if( 3==i && 0==(interface_name&DDR_CH3) ) continue;

    /* Mark device status as self refresh for selected channel and rank */
    if ( (chip_select & DDR_CS0) && (ddr_status.ddr_cs0[i] == DDR_ACTIVE) ) {
      csel |= DDR_CS0;
      ddr_status.ddr_cs0[i] = DDR_SELF_REFRESH;
    }
    if ( (chip_select & DDR_CS1) && (ddr_status.ddr_cs1[i] == DDR_ACTIVE) ) {
      csel |= DDR_CS1;
      ddr_status.ddr_cs1[i] = DDR_SELF_REFRESH;
    }
  }

  if ( interface_name!=DDR_CH_NONE && csel!=DDR_CS_NONE )
    HAL_DDR_Enter_Self_Refresh(ddrsns_share_data, interface_name, csel);

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_enter_self_refresh */

/* ============================================================================
**  Function : ddr_exit_self_refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the particular ddr into active mode, provided that it is currently in self refresh.
*   
*   @details
*   This function will first check to see if the ddr specified by the caller is available and in self refresh, then put it into active
*   mode. If it is not in self refresh, simply do nothing.
*   
*   @param interface_name   -  [IN] The interface to put ddr into self refresh
*   @param chip_select      -  [IN] The chip select to put ddr into self refresh
*   @param clkspeed      -        [IN] The current clock speed
*
*   @par Dependencies
*   This code has to reside in IRAM
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_exit_self_refresh(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
   ddrsns_exit_self_refresh(interface_name,chip_select,clk_speed);
}

void ddrsns_exit_self_refresh(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
  DDR_CHIPSELECT csel = DDR_CS_NONE;
  uint8 i = 0;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();
  
  /* update ddr status */ 
  for(i = 0; i < DDR_MAX_NUM_CH; i++) {
    if( 0==i && 0==(interface_name&DDR_CH0) ) continue;
    if( 1==i && 0==(interface_name&DDR_CH1) ) continue;
    if( 2==i && 0==(interface_name&DDR_CH2) ) continue;
    if( 3==i && 0==(interface_name&DDR_CH3) ) continue;

    /* Mark device status as self refresh for selected channel and rank */
    if ( (chip_select & DDR_CS0) && (ddr_status.ddr_cs0[i] == DDR_SELF_REFRESH) ) {
      csel |= DDR_CS0;
      ddr_status.ddr_cs0[i] = DDR_ACTIVE;
    }
    if ( (chip_select & DDR_CS1) && (ddr_status.ddr_cs1[i] == DDR_SELF_REFRESH) ) {
      csel |= DDR_CS1;
      ddr_status.ddr_cs1[i] = DDR_ACTIVE;
    }
  }

  if ( interface_name!=DDR_CH_NONE && csel!=DDR_CS_NONE )
    HAL_DDR_Exit_Self_Refresh(ddrsns_share_data, interface_name, csel);

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();
} /* ddr_exit_self_refresh */

/* ============================================================================
**  Function : ddr_enter_deep_power_down
** ============================================================================
*/
/*!
*   @brief
*   This function will put ddr into deep power down mode.
*   
*   @details
*   This function will put ddr into deep power down mode.
*   
*   @param interface_name   -  [IN] The interface to put ddr into DPD
*   @param chip_select      -  [IN] The chip select to put ddr into DPD
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   After a device has entered Deep Power Down, it will be unavailable for use.
*   When it is necessary to bring device out of DPD, a full HW initialization must be
*   executed.
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_enter_deep_power_down(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select)
{
   ddrsns_enter_deep_power_down(interface_name, chip_select);
}

void ddrsns_enter_deep_power_down(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select)
{
  DDR_CHIPSELECT csel = DDR_CS_NONE;
  uint8 i = 0;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();
  
  // i = ((interface_name == DDR_CH0) ? 0 : (interface_name == DDR_CH1) ? 1 : (interface_name == DDR_CH2) ? 2 : (interface_name == DDR_CH3) ? 3 : 0);

  while (interface_name != 1)
  {
    interface_name >>= 1;
    i++;
  } 

  /* Mark device status as DPD for selected channel and rank */
  if ( (chip_select & DDR_CS0) && (ddr_status.ddr_cs0[i] == DDR_ACTIVE) )
  {
    csel |= DDR_CS0;
    ddr_status.ddr_cs0[i] = DDR_DPD;
  }
  if ( (chip_select & DDR_CS1) && (ddr_status.ddr_cs1[i] == DDR_ACTIVE) )
  {
    csel |= DDR_CS1;
    ddr_status.ddr_cs1[i] = DDR_DPD;
  }
  
  if (csel != DDR_CS_NONE)
  {
    HAL_DDR_Enter_Deep_Power_Down(ddrsns_share_data,interface_name, csel);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_enter_deep_power_down */

/* ============================================================================
**  Function : ddr_exit_deep_power_down
** ============================================================================
*/
/*!
*   @brief
*   This function will take the ddr out of deep power down mode.
*   
*   @details
*   This function will take the ddr out of deep power down mode.
*   
*   @param interface_name  -  [IN] The interface to take ddr out of DPD
*   @param chip_select         -  [IN] The chip select to take ddr out of DPD
*   @param clk_speed          -  [IN] The ddr speed to reinitialize for
*
*   @par Dependencies
*   ddr clock must be returned to an applicable initialization frequency
*   LPDDR1 - no restrictions
*   LPDDR2 - < 50Mhz
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_exit_deep_power_down(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
   ddrsns_exit_deep_power_down(interface_name,chip_select,clk_speed);
}

void ddrsns_exit_deep_power_down(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_select, uint32 clk_speed)
{
  DDR_CHIPSELECT csel = DDR_CS_NONE;
  uint8 i = 0;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();
  
	// To-do:Get power of 2 in a better way
  // i = ((interface_name == DDR_CH0) ? 0 : (interface_name == DDR_CH1) ? 1 : (interface_name == DDR_CH2) ? 2 : (interface_name == DDR_CH3) ? 3 : 0);

  while (interface_name != 1)
  {
    interface_name >>= 1;
    i++;
  } 

  /* Mark device status as active for selected channel and rank */
  if ( (chip_select & DDR_CS0) && (ddr_status.ddr_cs0[i] == DDR_DPD) )
  {
    csel |= DDR_CS0;
    ddr_status.ddr_cs0[i] = DDR_ACTIVE;
  }
  if ( (chip_select & DDR_CS1) && (ddr_status.ddr_cs1[i] == DDR_DPD) )
  {
    csel |= DDR_CS1;
    ddr_status.ddr_cs1[i] = DDR_ACTIVE;
  }

  if (csel != DDR_CS_NONE)
  {
    HAL_DDR_Exit_Deep_Power_Down(ddrsns_share_data,interface_name, csel, clk_speed);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_exit_deep_power_down */

/* ============================================================================
**  Function : ddr_get_status
** ============================================================================
*/
/*!
*   @brief
*   Returns the status of DDR.
*   
*   @details
*   Returns the ddr_status internal structure so that caller of this API can
*   see which devices are available and what state they are in.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns the status of the devices on all available interface
*   and chip select
*   
*   @sa None
*/

ddr_interface_state ddr_get_status(void)
{
  return ddr_status;
} /* ddr_get_status */

/* ============================================================================
**  Function : ddr_update_info
** ============================================================================
*/
/*!
*   @brief
*   update the internal ddr info variable with the latest detected settings
*
*   @details
*   update the internal ddr info variable with the latest detected settings
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval
*   None
*
*   @sa None
*/

void ddr_update_info(void)
{
  ddr_info ddr_physical_memory_size = {{{0}}};
  IOCTL_ARG data = {0};
  ddr_size_info *ddr_size = &(ddr_physical_memory_size.ddr_size);
  uint8 i;
  
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    ddr_size->ddr_cs0[i] = ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_mb[i];
    ddr_size->ddr_cs0_addr[i] = ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_addr[i];
    
    ddr_size->ddr_cs1[i] = ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_mb[i];
    ddr_size->ddr_cs1_addr[i] = ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_addr[i];
  }

  /* Check to see if interleave is turned on */
  ddr_physical_memory_size.interleaved_memory = ddr_param_interleaved();

  /*
    Parse for ddr type from ddr parameters
    it is assumed that interface 0 and interface 1 has to have
    the same ddr type
  */
  ddr_physical_memory_size.ddr_type = (DDR_TYPE)(ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[0].device_type);

  /* update for highest banked bit */
  /* populate highest bank bit field */
  HAL_DDR_IOCTL(ddrsns_share_data, IOCTL_CMD_GET_HIGHEST_BANK_BIT, &data);
  ddr_size->highest_bank_bit = data.results;
    
  ddr_physical_size = ddr_physical_memory_size;
}/* ddr_update_info */

/* ============================================================================
**  Function : ddr_get_info
** ============================================================================
*/
/*!
*   @brief
*   Returns the physical configuration of ddr on target
*
*   @details
*   This function will return the physical configuration of the ddr on target
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval ddr_size_infor contains the ddr size and base address
*
*   @sa None
*/

ddr_info ddr_get_info(void)
{
  struct detected_ddr_device_params *ddr_detected_param_interface_0 = (ddr_get_detected_params(DDR_CH0));
  /* to workaround drivers calling ddr_get_info prior to ddr init, always update
   * with correct ddr type from ddr params. 
   */
  ddr_physical_size.ddr_type = (DDR_TYPE)(ddr_detected_param_interface_0->device_type);   
  
  return ddr_physical_size;
}/* ddr_get_info */

/* ============================================================================
**  Function : ddr_update_size
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr size and base address
*   
*   @details
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_infor contains the ddr size and base address
*   
*   @sa None
*/

void ddr_update_size(void)
{
  uint8 i;
  ddr_info ddr_physical_info = ddr_get_info();

  /* Check to see if interleave is turned on for CS0 */
  if(ddr_physical_info.interleaved_memory & DDR_CS0)
  {
    /* Interleave mode is on for CS0 , add all the sizes of CS0, assign it to interface 0 and do not populate remaining interfaces */
    for (i = 1; i < DDR_MAX_NUM_CH; i++)
    {
      ddr_physical_info.ddr_size.ddr_cs0[0] += ddr_physical_info.ddr_size.ddr_cs0[i];
      ddr_physical_info.ddr_size.ddr_cs0[i] = 0;    
    }
  }

  if(ddr_physical_info.interleaved_memory & DDR_CS1)
  {
    /* Interleave mode is on for CS1 , add all the sizes of CS1, assign it to interface 0 and do not populate remaining interfaces */
    for (i = 1; i < DDR_MAX_NUM_CH; i++)
    {
      ddr_physical_info.ddr_size.ddr_cs1[0] += ddr_physical_info.ddr_size.ddr_cs1[i];
      ddr_physical_info.ddr_size.ddr_cs1[i] = 0;
    }
  }
 
  ddr_system_size = ddr_physical_info.ddr_size;  
} /* ddr_update_size */

/* ============================================================================
**  Function : ddr_get_size
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr size and base address
*   
*   @details
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_infor contains the ddr size and base address
*   
*   @sa None
*/

ddr_size_info ddr_get_size(void)
{
  return ddr_system_size;
} /* ddr_get_size */

/* ============================================================================
**  Function : ddr_get_partition
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr partitions and base address
*   
*   @details
*	This API will return the number of partitions on each interface along with the 
*	base address
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_partition contains the ddr partitions and base address
*   
*   @sa None
*/

ddr_size_partition ddr_get_partition(void)
{
  ddr_size_partition ddr_partitions = {{{0}}};
  uint8 i;
  
  /* 
    because we only support segment masking due to bank interleaving,
    we always have 8 partitions per device
  */  
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    if (ddr_system_size.ddr_cs0[i])
    {
      ddr_partitions.ddr_cs0[i].ddr_addr = ddr_system_size.ddr_cs0_addr[i];
      ddr_partitions.ddr_cs0[i].num_partitions = 8;
    }
    if (ddr_system_size.ddr_cs1[i])
    {
      ddr_partitions.ddr_cs1[i].ddr_addr = ddr_system_size.ddr_cs1_addr[i];
      ddr_partitions.ddr_cs1[i].num_partitions = 8;
    }
  } 
  return ddr_partitions;
}

/*===========================================================================

**  Function :  ddr_unlock_xpu_protected_mem_region

** ==========================================================================
*/
/*!
* 
* @brief
*    Function unlock the DDR region locked(XPU protected) by XBL_SEC 
* 
* @par Dependencies
*   None
*   
* @retval. 
*    None
* @par Side Effects
*   None
*/


void ddr_unlock_xpu_protected_mem_region()
{
  
  /* Unlock DDR XPU regions using SMC call */
  boot_fastcall_tz_no_rsp (TZ_UNLOCK_CMD,
                           TZ_UNLOCK_CMD_PARAM_ID,
                           2,0,0,0); 
}

