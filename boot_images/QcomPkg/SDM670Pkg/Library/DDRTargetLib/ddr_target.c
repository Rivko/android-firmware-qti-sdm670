/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/28/15   rp      Added ddr_post_training function to support conditional loading of ddr training
07/20/15   tw      enable one time training
07/01/15   tw      added bimc remapper api call
06/20/14   tw      added ddr_pre_init api to capture any target specific 
                   workarounds that needs to be applied prior to ddr init
05/28/14   tw      cleaned up sbl <-> ddr driver dependencies around ddr training
                   implementation of cx\mx\cpr hash to force retraining
03/12/14   sr      Initial version.
================================================================================
                   Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_sync.h"
#include "ddr_log.h"
#include "ddr_params.h"
#include "ddr_target.h"
#include "HAL_SNS_DDR.h"
#include "ddr_config.h"
#include "ClockBoot.h"
#include "icbcfg.h"
#include <stddef.h>
#include "ddr_external.h"
#include "pm_ldo.h"
#include "CoreVerify.h"
#include "cpr.h"
#include "crc.h"
#include "boot_extern_platforminfo_interface.h"
#include "pm_smps.h"
#include "pm_resources_and_types.h"
#include "PlatformInfo.h"
#include "ddr_hwio.h"
#include "Target_cust.h"
#include "llcc.h"
#include "boot_util.h"
#include "ChipInfoDefs.h"
#include "ChipInfo.h"

#define TRAINING_ENABLED TRUE
#define ONE_TIME_TRAINING TRUE

/*==============================================================================
                                  MACROS
==============================================================================*/
/* Macro for round-up division */
#define div_ceil(n, d)  (((n) + (d) - 1) / (d))

#define EIGHT_SEGMENT_MASK 0xFF
#define FOUR_SEGMENT_MASK  0xF

#define DDR_COLLAPSE_CP    0x2

#ifdef GROOT_ONLY   /* for groot only PMIC configuration for reddragon */
#define CX_ID       2
#define CX_SMPS     PM_SMPS_1
#define VDDA_ID     0
#define VDDA_SMPS   PM_SMPS_2
#define VDDQ_ID     0
#define VDDQ_SMPS   PM_SMPS_3
#else
#define CX_ID       1
#define CX_SMPS     PM_SMPS_3
#define VDDA_ID     1
#define VDDA_SMPS   PM_SMPS_1
#define VDDQ_ID     1
#define VDDQ_SMPS   PM_SMPS_5
#endif

/*==============================================================================
                                  DATA
==============================================================================*/

/* DDR interface status, keeps track of active interfaces and their status */
extern ddr_interface_state ddr_status;

extern ddr_info ddr_physical_size;
extern ddr_size_info ddr_system_size;

uint32 ddr_boot_up_freq = 1017000;
 
static railway_voter_t ebi_pin = NULL;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

void print_ddr_frequency(void);
void ddr_freq_set_min_max(uint8 min, uint8 max);

void ddr_freq_set_min_max(uint8 min, uint8 max)
{
  uint8 i;
  uint8 num_levels = (sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0]));
  if (max > num_levels)
    max = num_levels;
  for (i = 0; i < num_levels; i++)
  {
    if (i >= min && i <= max)
      ddrsns_share_data->aop_params.mc_states[i].freq_enable = TRUE;
    else
      ddrsns_share_data->aop_params.mc_states[i].freq_enable = FALSE;
  }
    
}

/* ============================================================================
**  Function : ddr_target_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called at the end of ddr init to initialize any
*   target specific configurations
*
*   @details
*   Target specific configurations such as override of timing registers and
*   calling target specific init functions will be captured in this api
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_target_init()
{
  // uint8 ddr_min_freq_idx = 0;
  // uint8 ddr_max_freq_idx = 4;
  uint8 i;
  // char ddr_max_freq[50];
  uint32 chip_plat_ver_info;
  pm_on_off_type  on_off = PM_INVALID;
  uint32 err;
  DalPlatformInfoPlatformType platform = DalPlatformInfo_Platform();
  
 
  if (platform != DALPLATFORMINFO_TYPE_RUMI)
  {
    ddrsns_share_data->shrm_ddr_ptr->misc.target_silicon = 1;

    if (PcdGet32 (PcdBuildType) == 0)
    {
      int rail = rail_id("vddmx");
      CORE_VERIFY(rail!=RAIL_NOT_SUPPORTED_BY_RAILWAY);    
      ebi_pin = railway_create_voter(rail, RAILWAY_DDR_TRAINING_VOTER_ID);
      railway_corner_vote(ebi_pin, RAILWAY_TURBO);
      railway_transition_rails();
      err = pm_smps_sw_mode(CX_ID, CX_SMPS, PM_SW_MODE_NPM); // CX SMPS3B to PWM
      CORE_VERIFY(err == PM_ERR_FLAG__SUCCESS);
      err = pm_smps_sw_mode(VDDA_ID, VDDA_SMPS, PM_SW_MODE_NPM); // VDDA_EBI SMPS1B (MX) to PWM
      CORE_VERIFY(err == PM_ERR_FLAG__SUCCESS);    
    }
  }
  ddrsns_share_data->jtag_connected = HWIO_INF(AOSS_CC_CDBGPWRUPREQ, STATUS);
  
  chip_plat_ver_info =  HWIO_IN(TCSR_SOC_HW_VERSION);

  /* extract upper 16 bit and store it in platform field of ddr_misc */
  ddrsns_share_data->shrm_ddr_ptr->misc.platform_id  = chip_plat_ver_info >> 16 /*& 0xFFFF0000*/ ;

  #ifdef GROOT_ONLY 
  if (ddrsns_share_data->shrm_ddr_ptr->misc.platform_id == 0x6005) 
  {
    ddrsns_share_data->shrm_ddr_ptr->misc.platform_id = 0x6006;  // FOR RED-DRAGON LC
  }
  else if (ddrsns_share_data->shrm_ddr_ptr->misc.platform_id == 0x6009)
  {
    ddrsns_share_data->shrm_ddr_ptr->misc.platform_id = 0x600A;  // FOR KINGSMAN LC
  }	  
  #endif

  /* extract lower 16 bits and store it in version field of ddr_misc */
  ddrsns_share_data->shrm_ddr_ptr->misc.chip_version  = chip_plat_ver_info & 0x0000FFFF ;
  
  /* Extract the MSM_PKG_TYPE from QFPROM register */
  ddrsns_share_data->shrm_ddr_ptr->misc.package_type = HWIO_INF(QFPROM_RAW_FEAT_CONFIG_ROW4_MSB , MSM_PKG_TYPE);
  
  ddrsns_share_data->shrm_ddr_ptr->misc.misc_cfg[0] = 1;
  
  /* This call is made in order to know the voltage level , 
   * for determining whether we have LP4 or LP4x on the chip */

  if (platform != DALPLATFORMINFO_TYPE_RUMI)
  {
    pm_smps_sw_enable_status (VDDQ_ID, VDDQ_SMPS,&on_off);

    if(on_off == PM_OFF){
      for (i = 0; i < DDR_MAX_NUM_CH; i++)
      {
        ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[i].device_type = DDR_TYPE_LPDDR4;
      }
    }
    else if(on_off == PM_ON) {
      err = pm_smps_sw_mode(VDDQ_ID, VDDQ_SMPS, PM_SW_MODE_NPM); // VDDQ SMPS_5B
      CORE_VERIFY(err == PM_ERR_FLAG__SUCCESS);
      for (i = 0; i < DDR_MAX_NUM_CH; i++)
      {
        ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[i].device_type = DDR_TYPE_LPDDR4X;
      }
    }
  }
}

/* ============================================================================
**  Function : ddr_pre_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called before ddr is initialized. It will take care of any
*   pre initialization workarounds.
*
*   @details
*   This function is called before ddr is initialized. It will take care of any
*   pre initialization workarounds.
*
*   @param
*   boolean -
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean ddr_pre_init()
{
  return TRUE;
}

/* ============================================================================
**  Function : ddr_post_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @details
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @param
*   boolean -
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean ddr_post_init()
{
  char ddr_log_string[50];
  uint32 i , size_cs0 = 0, size_cs1 = 0;
  DalPlatformInfoPlatformType platform = DalPlatformInfo_Platform();

  snprintf (ddr_log_string, 50, "DSF version = %d.%d, DSF SHRM version = %d.%d", 
            ((ddrsns_share_data->shrm_ddr_ptr->boot_dsf_version) >> 16),((ddrsns_share_data->shrm_ddr_ptr->boot_dsf_version) & 0x0000FFFF),
             ((ddrsns_share_data->shrm_ddr_ptr->shrm_dsf_version) >> 16),((ddrsns_share_data->shrm_ddr_ptr->shrm_dsf_version) & 0x0000FFFF));
  boot_log_message(ddr_log_string);
  
  snprintf (ddr_log_string, 50, "Manufacturer ID = %x, Device Type = %x", ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[0].manufacturer_id,
            ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[0].device_type);
  boot_log_message(ddr_log_string);
  
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    size_cs0+= ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_mb[i];
    size_cs1+= ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_mb[i];
  }
  snprintf (ddr_log_string, 50, "Rank 0 size = %d MB, Rank 1 size = %d MB", size_cs0, size_cs1);
  boot_log_message(ddr_log_string);
  
  if (platform != DALPLATFORMINFO_TYPE_RUMI)
  {
    if (PcdGet32 (PcdBuildType) == 0)
    {
      railway_corner_vote(ebi_pin, RAILWAY_NO_REQUEST);
      railway_transition_rails();
    }
    // Clock_SetBIMCSpeed(300000);
    // print_ddr_frequency();
  }
  
	return TRUE;
}

void print_ddr_frequency()
{
  uint32 clk_period;
  char ddr_freq[50];
  
  clk_period = HWIO_IN(CABO_BROADCAST_CABO_GLOBAL_DDR_CLK_PERIOD_CFG);
  snprintf (ddr_freq, 50, "Frequency = %d MHz", 1000000/clk_period);
  boot_log_message(ddr_freq);
}

/* ============================================================================
**  Function : ddr_pre_clock_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before clock switching occures.
*   The function will configure the ddr such that no data loss will occur
*
*   @details
*   DDR will be stalled and new timing parameter is loaded into shadow.
*   Depending on bumping up or stepping down clock speed, we will load the
*   shadow register value to actual registers before or after clock switch
*   has occurred.
*
*   @param curr_clk   -   [IN] the current clock speed
*   @param new_clk    -  [IN] the clock speed we are switching to
*   @param new_clk    -  [IN] interface to switch clock for
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_pre_clock_switch(uint32 curr_clk, uint32 new_clk , DDR_CHANNEL interface_name)
{
  HAL_DDR_Pre_Clock_Switch(ddrsns_share_data, interface_name, curr_clk, new_clk);
} /* ddr_pre_clock_switch */


/* ============================================================================
**  Function : ddr_clock_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after pre clock switching function.
*   The function will actually do the real clock switch
*
*   @details
*   DDR will be stalled and new timing parameter is loaded into shadow.
*   Depending on bumping up or stepping down clock speed, we will load the
*   shadow register value to actual registers before or after clock switch
*   has occurred.
*
*   @param curr_clk   -   [IN] the current clock speed
*   @param new_clk    -  [IN] the clock speed we are switching to
*   @param new_clk    -  [IN] interface to switch clock for
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_clock_switch(uint32 curr_clk, uint32 new_clk , DDR_CHANNEL interface_name)
{
  HAL_DDR_Clock_Switch(ddrsns_share_data, interface_name, curr_clk, new_clk);
} /* ddr_clock_switch */


/* ============================================================================
**  Function : ddr_post_clock_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after clock switching occurs.
*   The function will configure the ddr such that no data loss will occur
*
*   @details
*   DDR will be unstalled.
*   Depending on bumping up or stepping down clock speed, we will load the
*   shadow register value to actual registers before or after clock switch
*   has occurred.
*
*   @param curr_clk          -  [IN] the current clock speed
*   @param new_clk           -  [IN] the clock speed we are switching to
*   @param interface_name    -  [IN] interface to switch clock for
*
*   @par Dependencies
*   This code has to be on IRAM because ddr is unavailable during clock switching
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_post_clock_switch(uint32 curr_clk, uint32 new_clk, DDR_CHANNEL interface_name)
{
     HAL_DDR_Post_Clock_Switch(ddrsns_share_data, interface_name , curr_clk, new_clk);
} /* ddr_post_clock_switch */

/* ============================================================================
**  Function : ddr_pre_vddmx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before voltage switch occurs.
*
*   @param vddmx_microvolts - vddmx voltage in microvolts
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_pre_vddmx_switch(uint32 vddmx_microvolts)
{
  /* Stepping Down in VDDCX voltage */
  ddr_status.vddmx_voltage = vddmx_microvolts;
} /* ddr_pre_vddmx_switch */

/* ============================================================================
**  Function : ddr_post_vddmx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after voltage switch occurs.
*
*   @param vddmx_microvolts - vddmx voltage in microvolts
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_post_vddmx_switch(uint32 vddmx_microvolts)
{

} /* ddr_post_vddmx_switch */

/* ============================================================================
**  Function : ddr_pre_vddcx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before vddcx switch.
*
*   @param settings - contains the VDDCX voltage level we just switched to
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void ddr_pre_vddcx_switch(uint32 vddcx_microvolts)
{
} /* ddr_pre_vddcx_switch */

/* ============================================================================
**  Function : ddr_post_vddcx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after VDDCX is switched
*
*   @param none
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_post_vddcx_switch(uint32 vddcx_microvolts)
{
} /* ddr_post_vddcx_switch */

/* ============================================================================
**  Function : ddr_pre_xo_shutdown
** ============================================================================
*/
/**
*   @brief
*   Called right before XO shutdown. Puts DDR into self refresh mode and
*   disables CDC and I/O calibration.
*
*   @param[in]  clk_speed    Current clock speed
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   ddr_post_xo_shutdown
*/

void ddr_pre_xo_shutdown(uint32 clk_speed)
{
  ddr_enter_self_refresh_all(clk_speed);
} /* ddr_pre_xo_shutdown */

/* ============================================================================
**  Function : ddr_post_xo_shutdown
** ============================================================================
*/
/**
*   @brief
*   Called right after XO wakeup. Takes DDR out of self refresh mode and enables
*   CDC and I/O calibration.
*
*   @param[in]  clk_speed    Current clock speed
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   ddr_pre_xo_shutdown
*/

void ddr_post_xo_shutdown(uint32 clk_speed)
{
  ddr_exit_self_refresh_all(clk_speed);

} /* ddr_post_xo_shutdown */

/* ============================================================================
**  Function : ddr_check_partition
** ============================================================================
*/
/*!
*   @brief
*   A helper function for ddr_dmm_partition to check the given interface and partition
*   whether we can put into dpd or full\partial self refresh
*
*   @details
*   Given the retention and active state, put the corresponding ddr interface
*   into self refresh, or deep power down when possible
*
*   Truth table for active and retention state:
*                        Active State:0         |      Active_state:1
*   Retention_state:0    self refresh/dpd       |      Invalid config, assume to be
*                                               |      Active retention
*   Retention_state:1    self refresh retention |      Active Retention
*
*   @param interface_name   -  [IN] the interface to check for
*   @param chip_sel         -  [IN] the chip select on the interface to check for
*   @param retention_state  -  [IN] the retention state for the partitions given
*   @param active_state     -  [IN] the active state for the partitions given
*   @param num_partitions   -  [IN] the number of partitions on this interface
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
void ddr_check_partition(DDR_CHANNEL interface_name, DDR_CHIPSELECT chip_sel, uint32 retention_state, uint32 active_state, uint8 num_partitions)
{

} /* ddr_check_partition */

/* ============================================================================
**  Function : ddr_dmm_partition
** ============================================================================
*/
/*!
*   @brief
*   Given the retention and active state, put the corresponding ddr interface
*   into self refresh, or deep power down when possible
*
*   @details
*   Given the retention and active state, put the corresponding ddr interface
*   into self refresh, or deep power down when possible
*
*   Truth table for active and retention state:
*                        Active State:0         |      Active_state:1
*   Retention_state:0    self refresh/dpd       |      Invalid config, assume to be
*                                               |      Active retention
*   Retention_state:1    self refresh retention |      Active Retention
*
*   @param retention_state  -  [IN] the retention state for the partitions given
*   @param active_state     -  [IN] the active state for the partitions given
*
*   @par Dependencies
*   Caller of this API has to take care not to put ddr interface that is in use
*   into self refresh or deep power down.
*
*   @par Side Effects
*   None
*
*   @retval
*  None
*
*   @sa None
*/

void ddr_dmm_partition(uint32 retention_state, uint32 active_state)
{
}
/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. LPDDR3 will always require training syncronization
*   to be done between rpm and sbl
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_required( void )
{
  DalPlatformInfoPlatformType platform;
  platform = boot_DalPlatformInfo_Platform();
  boolean training_required;
  
  if ((platform != DALPLATFORMINFO_TYPE_RUMI) &&
      (platform != DALPLATFORMINFO_TYPE_VIRTIO))
  {
    training_required = TRAINING_ENABLED;
  }
  else
  {
    training_required = FALSE;
  }
  return training_required;
} /* ddr_params_is_training_required */


/* ============================================================================
**  Function : ddr_is_training_required
** ============================================================================
*/
/*!
*   @brief
*   This function will parse the crc hash and determine if training is required
*   based on serial number, mx\cx\rbcpr hash
*
*   @details
*   This function will parse the crc hash and determine if training is required
*   based on serial number, mx\cx\rbcpr hash
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  boolean - Training required/Not required
*
*   @sa None
*/
boolean ddr_is_training_required(void)
{
  uint32 serial_number;

  /* cx/mx/vdda hash variables */
  uint32 combined_checksum = cpr_cx_mx_settings_checksum();

  /* compare checksum for training data in our partition with DDR_STRUCT to see if training is required */
  serial_number = BOOT_HWIO_IN(QFPROM_RAW_PTE_ROW1_LSB , 0);

  ddr_printf ( DDR_NORMAL, "DDR: The serial number is %d", serial_number);

  /* combine serial number with voltage checksum for a new seed */
  combined_checksum = serial_number ^ combined_checksum;

  ddr_printf (DDR_NORMAL, "DDR: Checksum on flash is %d", ddrsns_share_data->training_results.checksum);
  ddr_printf (DDR_NORMAL, "DDR: Recomputed checksum is %d", ddr_get_training_checksum(combined_checksum));


  if (ddrsns_share_data->training_results.checksum  != ddr_get_training_checksum(combined_checksum))
  {
    ddr_printf (DDR_NORMAL, "DDR: Training is required");

    return TRUE;
  }

  ddr_printf (DDR_NORMAL, "DDR: Training is not required");
  return FALSE;
} /* ddr_is_training_required */

/* =============================================================================
**  Function : ddr_do_phy_training_init
** =============================================================================
*/
/**
*   @brief
*   Indicates that PHY training needs to be done in SBL1.
*
*   @param  None
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
void ddr_do_phy_training_init( void )
{
  ddr_printf (DDR_NORMAL, "DDR: Start of HAL DDR Boot Training");
  HAL_DDR_Boot_Training_Init(ddrsns_share_data, DDR_CH_ALL, DDR_CS_BOTH);	
	Clock_SetBIMCSpeed(ddr_boot_up_freq);
  print_ddr_frequency();
	ddr_printf (DDR_NORMAL, "DDR: End of HAL DDR Boot Training");
} /* ddr_do_phy_training */

/* =============================================================================
**  Function : ddr_do_phy_training_restore
** =============================================================================
*/
/**
*   @brief
*   Restore training data results, if phy training is already done
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_do_phy_training_restore( void )
{
#if ONE_TIME_TRAINING  
  if (ddr_is_training_required())
  {
#endif
    memset(&ddrsns_share_data->training_results, 0x0, sizeof(struct ddr_params_partition));
    return TRUE;
#if ONE_TIME_TRAINING
  }
  else if (!HAL_DDR_Boot_Training_Restore(ddrsns_share_data, DDR_CH_ALL, DDR_CS_BOTH))
  {
    memset(&ddrsns_share_data->training_results, 0x0, sizeof(struct ddr_params_partition));
    return TRUE;
  }
  else
  {
    Clock_SetBIMCSpeed(ddr_boot_up_freq);
    print_ddr_frequency();

    return FALSE;
  }
#endif

} /* ddr_do_phy_training */

/* =============================================================================
**  Function : ddr_post_training
** =============================================================================
*/
/**
*   @brief
*   This function is called after ddr training. It will take care of all post
*   training activities such as computing checksum over training data.
*
*   @param  None
*
*   @retval  TRUE If one time training is enabled
*   @retval  FALSE If one time training is disabled
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
boolean ddr_post_training(void)
{
  uint32 serial_number;
  // uint8 min_ddr_freq = 0;
  // uint8 max_ddr_freq = 8;
  uint32 combined_checksum = cpr_cx_mx_settings_checksum();

  serial_number = BOOT_HWIO_IN(QFPROM_RAW_PTE_ROW1_LSB , 0);

  /* combine serial number with voltage checksum for a new seed */
  combined_checksum = serial_number ^ combined_checksum;

  /* update training data checksum */
  ddr_set_training_checksum(combined_checksum);

  /* update training data and log size */
  ddrsns_share_data->training_results.training_data_size = sizeof(ddrsns_share_data->training_results.training_data);
  ddrsns_share_data->training_results.training_log_size = ddr_external_get_training_log_size();
  
  // ddr_freq_set_min_max(min_ddr_freq, max_ddr_freq);

  return ONE_TIME_TRAINING;
}

/*===========================================================================

**  Function :  ddr_post_setup

** ==========================================================================
*/
/*!
* 
* @brief
*    Function which gets executed after DDR setup operations are complete
* 
* @par Dependencies
*   None
*   
* @retval. 
*    None
* @par Side Effects
*   None
*/

void ddr_post_setup()
{
   uint8 min_ddr_freq = 2;
   uint8 max_ddr_freq = 0;
   uint8 index=0;
   uint8 num_levels = (sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0]));
   for (index = num_levels-1; index >= 0; index--)
   {
     if (ddrsns_share_data->aop_params.mc_states[index].freq_enable == TRUE)
       break;
   } 
   max_ddr_freq = index;


   ddr_freq_set_min_max(min_ddr_freq, max_ddr_freq);

   /* Put LLCC in cacheable mode. Do this WA in DDR driver till DSF moves it inside their code */
  HAL_DDR_LLCC_cacheable_mode_override_ctrl(ddrsns_share_data, DDR_CH_ALL, CACHEABLE);
}




/*  Remapper Table 
    This table and code will need to be updated based on newer DDR configurations 
*/

RemapperTable RemapTable[] =
{
/* remapping configurations */
  
/*             SCMO_CS0  SCMO_CS1                 rank_detected  cs0_size cs1_size cs0_remap_size cs1_remap_size  BIMC_Remap_CS0 BIMC_Remap_CS1 */
/* 1GB Single Rank    { 0x0   N/A   } */          {DDR_CS0 ,      0x400,  0x0,     0x400,          0x0,           0x80000000,              0},
/* 1.5GB Single Rank    { 0x0   N/A   } */        {DDR_CS0 ,      0x600,  0x0,     0x600,          0x0,           0x80000000,              0},
/* 2GB Single Rank    { 0x0   N/A         } */    {DDR_CS0,       0x800,  0x0,     0x800,          0x0,           0x80000000,              0},
/* 2GB Dual Rank    { 0x0,  0x80000000  }*/       {DDR_CS_BOTH,   0x400,  0x400,   0x400,          0x0,           0xC0000000,              0},
/* 3GB Single Rank    { 0x0   N/A         } */    {DDR_CS0 ,      0xC00,  0x0,     0xC00,          0x0,           0x80000000,              0},
/* 3GB Dual Rank(1+2) { 0x0   0x80000000  } */    {DDR_CS_BOTH,   0x400,  0x800,   0x400,          0x0,           0x100000000,             0},
/* 3GB Dual Rank(1.5+1.5){ 0x0  0x80000000  } */  {DDR_CS_BOTH,   0x600,  0x600,   0x600,          0x0,           0xE0000000,              0},
/* 3GB Dual Rank(2+1) { 0x0   0x80000000  } */    {DDR_CS_BOTH,   0x800,  0x400,   0x800,          0x0,           0xC0000000,              0},
/* 4GB Single Rank    { 0x0   N/A         } */    {DDR_CS0,       0x1000, 0x0,     0x1000,         0x0,           0x80000000,              0},
/* 4GB Dual Rank(1+3) { 0x0   0x80000000 } */     {DDR_CS_BOTH,   0x400,  0xC00,   0x400,          0x0,           0x140000000,             0},
/* 4GB Dual Rank(2+2) { 0x0   0x80000000 } */     {DDR_CS_BOTH,   0x800,  0x800,   0x800,          0x800,         0x80000000,    0x100000000},
/* 4GB Dual Rank(3+1) { 0x0   0x100000000 } */    {DDR_CS_BOTH,   0xC00,  0x400,   0xC00,          0x400,         0x80000000,    0x140000000},
/* 6GB Dual Rank(2+4) { 0x0   0x80000000 } */     {DDR_CS_BOTH,   0x800,  0x1000,  0x800,          0x0,           0x80000000,              0},
/* 6GB Dual Rank(3+3) { 0x0   0x100000000 } */    {DDR_CS_BOTH,   0xC00,  0xC00,   0xC00,          0xC00,         0x80000000,    0x140000000},
/* 6GB Dual Rank(4+2) { 0x0   0x100000000 } */    {DDR_CS_BOTH,   0x1000, 0x800,   0x800,          0x0,           0x180000000,             0},
/* 8GB Dual Rank(4+4) { 0x0,  0x100000000 } */    {DDR_CS_BOTH,   0x1000, 0x1000,  0x1000,         0x1000,        0x80000000,    0x180000000}

};


/* =============================================================================
**  Function : ddr_remapper
** =============================================================================
*/
/**
*   @brief
*   configures ddr to be remapped based on density
*
*   @param void
*
*   @retval  void
*
*   @dependencies
*   ddr initialization has already finished
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_remapper(void)
{
  icbcfg_remap_info_type remap_info;
  uint64 remap_size = 0;

  uint8 cs=0, index;
  RemapperTable *Remap_Tbl_ptr = RemapTable;
  uint8 remap_table_size = (sizeof(RemapTable)/sizeof(RemapTable[0]));
  DDR_CHIPSELECT chip_select = ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
  uint8 cs_loop_inx = 0;
  uint8 i = 0;
  uint32 ddr_cs0_size = 0, ddr_cs1_size = 0;
  
  for(i = 0; i < DDR_MAX_NUM_CH; i++)
  {
	  ddr_cs0_size += ddr_system_size.ddr_cs0[i];
	  ddr_cs1_size += ddr_system_size.ddr_cs1[i];
  }
  
  for (index = 0; index < remap_table_size; index++)
  {

    if((ddr_cs0_size == Remap_Tbl_ptr[index].cs0_size) && 
      (ddr_cs1_size == Remap_Tbl_ptr[index].cs1_size) && 
      (Remap_Tbl_ptr[index].rank_detected == chip_select)) 
	 	break;
  }

  if(index >= remap_table_size)
    ddr_printf (DDR_ERROR, "**ERROR Can't find a matching entry in the DDR remapper table**");
  else
  {
    for(cs = DDR_CS1; cs >= DDR_CS0; cs--) {

      remap_info.dest_addr = (cs == DDR_CS0)? Remap_Tbl_ptr[index].bimc_remapper_cs0 : Remap_Tbl_ptr[index].bimc_remapper_cs1;
      remap_info.src_addr = (cs == DDR_CS0)?ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_addr[0]: ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_addr[0];
  
      remap_size = (cs == DDR_CS0)? Remap_Tbl_ptr[index].cs0_remap_size : Remap_Tbl_ptr[index].cs1_remap_size;
      remap_size *= 1024 * 1024; /* ram size is in byte */
	  
	  remap_info.size = remap_size;
      remap_info.interleaved = ICBCFG_REMAP_INTERLEAVE_DEFAULT;
      remap_info.deinterleave = FALSE;
	  
	  /* if dest addr is 0, will skip mapping */
      if(0 == remap_info.dest_addr) {
         if(cs == DDR_CS1) {
           for (i = 0; i < DDR_MAX_NUM_CH; i++){
                ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[i] = ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_addr[i];
           }
        }
        continue;
      }

      ICB_RemapEx("/dev/icbcfg/boot", &ddr_physical_size, cs_loop_inx, &remap_info);

      if(cs == DDR_CS0) {
        /* update internal variable to track new mapping */
        for (i = 0; i < DDR_MAX_NUM_CH; i++) {
          ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[i] = ddr_system_size.ddr_cs0_addr[i] = remap_info.dest_addr;
        }
      }
      else if(cs == DDR_CS1) {
        for (i = 0; i < DDR_MAX_NUM_CH; i++) {
          ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[i] = ddr_system_size.ddr_cs1_addr[i] = remap_info.dest_addr;
        }
      }
      cs_loop_inx ++;
    }
     ddr_printf (DDR_NORMAL, "Remapped addresses ddr_cs0 is %X, ddr_cs1 is %X", 
	  	                              ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0],
	  	                              ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0]);
  }

} /* ddr_remapper */

