#ifndef __HAL_SNS_DDR_H__
#define __HAL_SNS_DDR_H__

/**---------------------------------------------------------------------------
* @file     HAL_sns_DDR.h
*
* @brief    DDR HW abstraction layer
*
* Copyright (c) 2014-2017, Qualcomm Technologies Incorporated. All rights reserved.
*--------------------------------------------------------------------------*/

/*=============================================================================

                                   DDR HAL
                            H e a d e r  F i l e
GENERAL DESCRIPTION
This is the header file for ddr HAL. This file is shared across targets

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/common/sns_api/HAL_SNS_DDR.h#7 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/015/14  tw      Modified clockplan structure to add new fields to support
                   dynamic clockplan sharing
12/01/13   dp      Initial revision.
=============================================================================*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "HALhwio.h"

#include "ddr_cores.h"
#include "ddr_common.h"
#include "dev_cdt_params.h"
#include "ddr_runtime.h"
#include "dev_flash_params.h"
#include "ddr_aop_params.h"
#include "ddr_external.h"
#include "ddr_status_reg.h"
#include "ddr_ioctl.h"
#include "ddr_log.h"
#include "ddrss_training.h"

// Modes for Power Collapse.
typedef enum {
    DDR_POWER_CLPS_MODE_BIMC_ONLY = 0,
    DDR_POWER_CLPS_MODE_PHY_ONLY,
    DDR_POWER_CLPS_MODE_MAX = 0x7F
} DDR_POWER_CLPS_MODE;

typedef enum
{
    PHY_POWER_CLPS_SAVE         = 0x0,
    PHY_POWER_CLPS_RESTORE      = 0x1,
    PHY_POWER_CLPS_SAVE_RESTORE = 0x2
} PCC_MODE;

typedef enum {
    MSM_VERSION_NAPALI      = 0X6000,
    MSM_VERSION_WARLOCK     = 0X6004,
    MSM_VERSION_REDDRAGON   = 0X6005,
    MSM_VERSION_REDDRAGONLC = 0X6006,
	MSM_VERSION_KINGSMAN 	= 0X6009,
	MSM_VERSION_KINGSMANLC 	= 0X600A
} MSM_VERSION;

/** Encodes vendor-specific pre-standard support for LPDDR4Y features. */
typedef enum
{
    LPDDR4Y_LOW_SPEED_CA_BUFFER = 1 << 0,
    LPDDR4Y_SINGLE_ENDED_CLOCK = 1 << 1
} LPDDR4Y_SUPPORT_TYPE;

/**************************/
/*** Miscellaneous Info ***/
/**************************/
/**
 * ddr_misc: Miscellaneous Information Data Structure.
 */
struct ddr_misc
{
  uint32 training_freq_in_khz[MAX_TRAINING_FREQ_NUM ];
  uint32 misc_cfg[10];                      /**< Index 0: Small Eye Detect Flag             */
                                            /**< Index 1: TBD                               */
                                            /**< Index 2: TBD                               */
                                            /**< Index 3: TBD                               */
                                            /**< Index 4: TBD                               */
                                            /**< Index 5-9: TBD                             */
  uint32 platform_id;
  uint32 chip_version;
  uint32 bus_width_pch;
  uint32 num_channel;
  uint32 broadcast_ch_en_mask;
  uint32 all_pcb_mask;
  uint32 all_way_mask;
  uint32 ram_status0_mask;
  uint32 ram_status1_mask;
  uint32 pcb_status_mask;
  uint32 num_pcb;
  uint32 current_clk_in_kHz;
  uint32 new_clk_in_kHz;
  uint32 current_shub_clk_in_kHz;
  uint32 current_ddrss_powerstate;
  uint32 target_silicon;
  uint32 package_type;
  uint32 profiling_struct_addr;
  uint32 boot_init_completed;
  uint32 boot_training_status;
  uint32 rsi_cmd_logging[16];
  uint32 dcb_crc;
};

/**********************/
/*** DDR Debug ***/
/**********************/
/**
 * ddr_dbg: DDR Debug Data Structure.
 * If this element is zero, it means that there is no debug
 * structure that the DDR System Firmware needs to process.
 * A non-zero value in this element will be used by the DDR System Firmware as a
 * pointer to a debug structure whose contents will be determined by debug requirements
 * as they come up.
 */
union ddr_dbg
{
  uint32 ptr32;   /**< Store pointer to DDR Debug Structure */
  uint64 ptr64;   /**< Store pointer to DDR Debug Structure */
};

/*******************************/
/*** SW ZQCAL Data Structure ***/
/*******************************/
/**
 * sw_zqcal_struct: SW ZQCAL Data Structure.
 */
struct sw_zqcal_struct
{
  uint8  event_enable;
  uint8  event_status;
  uint8  rank_flag;
  uint32 interval;  
};

/*******************************************/
/*** SHRM DDR Parameter datastructure ***/
/*******************************************/
/**
 * SHRM_DDR_STRUCT: Runtime DDR Parameter Data Structure.
 * Inside SHRM code, DDR System Firmware can use a pointer to this struct directly.
 * However, if DSF needs to access the elements of this data structure during boot time,
 * it should do it through the pointer inside BOOT_DDR_STRUCT. 
 */
typedef struct
{
  uint32                                boot_dsf_version;               /**< DDR System Firmware Version */
  uint32                                shrm_dsf_version;               /**< Version of SHRM- only code in DDR System Firmware */
  struct ddr_mc                         mc;                             /**< DDR Controller Information Structure */
  struct ddr_phy                        phy;                            /**< DDR PHY Information Structure */
  struct sc_llcc                        llcc;                           /**< Last level cache controlled Information Structure */
  struct ddr_shrm                       shrm;                           /**< SHRM Information Structure */
  struct detected_ddr_device_params     detected_ddr_device[4];         /**< Detected DDR device parameters for every channel */
  struct ddr_misc                       misc;                           /**< Miscellaneous Info Data Structure */
  ddr_sns_size_info                     ddr_size_info;                  /**< DDR part details , including type,interleave,size & start address per channel & cs */
  struct ddr_runtime_struct             ddr_runtime;                    /**< Extended Configuration Data Table */
  uint32                                periodic_events_enable_flag;    /**< Periodic_events_flag for power_collapse_exit */
  struct sw_zqcal_struct                sw_zqcal;                       /**< SW ZQCAL Data Structure */
} SHRM_DDR_STRUCT;

/*******************************************/
/*** BOOT DDR Parameter datastructure ***/
/*******************************************/
/**
 * DDR_STRUCT: Boot time DDR Parameter Data Structure.
 * Caller of DDR System Firmware will pass a pointer to this data structure
 * with every call made to top-level functions such as HAL_DDR_Init().
 */

#define DCB_HEADER_MAX_NUM_IDX 25
typedef struct
{
  struct ddr_params_partition       training_results;                       /**< DDR Training results to be stored to flash */
  SHRM_DDR_STRUCT                   *shrm_ddr_ptr;                          /**< Pointer to SHRM_DDR_STRUCT */
  uint32                            jtag_connected;                         /**< JTAG connection status */
  uint16                            (*dcb_ptr)[DCB_HEADER_MAX_NUM_IDX][2];  /**< Pointer to DCB */
  DDR_AOP_PARAMS                    aop_params;                             /**< Information copied from DCB to AOP */
  training_params_t                 training_params;                        /**< Training parameters including limits for aborting training */              
  uint32                            dq_phy_regs[NUM_DQ_TRAINING_STACK];     /**< List of register addresses to save and restore for DQ PHY */
  uint32                            ca_phy_regs[NUM_CA_TRAINING_STACK];     /**< List of register addresses to save and restore for CA PHY */
} BOOT_DDR_STRUCT;

/****************************************/
/******* For LLCC Cacheable Modes *******/
/****************************************/
typedef enum
{
    NO_OVERRIDE    = 0x0 ,
    NON_CACHEABLE  = 0x1 ,
    CACHEABLE      = 0x2
} CACHEABLE_MODE;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* =============================================================================
**  Function : HAL_DDR_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR controller, as well as DDR device.
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel      channel to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_freq_khz Clock speed (in KHz) to initialize for
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
// ------------------------------------------------------------------------------------------------------
// The API HAL_DDR_Init() takes a new parameter: EXTENDED_CDT_STRUCT *ecdt. In this MSM8996
// DSF, the Extended CDT is not being processed. It has been put in purely to freeze
// the data structures and APIs. Application of Extended CDT parameters into DDR
// subsystem and boot-time and run-time will be implemented in a future DSF release.
// ------------------------------------------------------------------------------------------------------
boolean HAL_DDR_Init(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select,
                     uint32 clk_freq_khz);

/* =============================================================================
**  Function : HAL_DDR_Boot_Training_Init
** =============================================================================
*/
/**
*   @brief
*   Execute all one time DDR trainings
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel      channel to initialize for
*   @param[in]  chip_select  Chip select to initialize for

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
boolean HAL_DDR_Boot_Training_Init(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);
                              
/* =============================================================================
**  Function : HAL_DDR_Boot_Training_Restore
** =============================================================================
*/
/**
*   @brief
*   Execute DDR Training Restore.
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel      channel to initialize for
*   @param[in]  chip_select  Chip select to initialize for

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
boolean HAL_DDR_Boot_Training_Restore(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);
                              
/* =============================================================================
**  Function : HAL_DDR_Periodic_Training
** =============================================================================
*/
/**
*   @brief
*   Setup state for any DDR perioidic trianings
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel      channel to initialize for
*   @param[in]  chip_select  Chip select to initialize for
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
boolean HAL_DDR_Periodic_Training(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/* =============================================================================
**  Function : HAL_DDR_Periodic_Training_Ctrl
** =============================================================================
*/
/**
*   @brief
*   Enable or Disable Periodic Training after Boot Training is completed
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  enable       enable or disable control
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
boolean HAL_DDR_Periodic_Training_Ctrl(BOOT_DDR_STRUCT *ddr, uint8 enable);

/* =============================================================================
**  Function : HAL_DDR_Pre_Clock_Switch
** =============================================================================
*/
/**
*   @brief
*   Init state before DDR clock switch
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel    channel to initialize for
*   @param[in]  curr_clk_khz Current CLK freq in KHZ before clock switch
*   @param[in]  new_clk_khz  New CLK freq in KHZ after clock switch
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
boolean HAL_DDR_Pre_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz,
                                 uint32 new_clk_khz);

/* =============================================================================
**  Function : HAL_DDR_Clock_Switch
** =============================================================================
*/
/**
*   @brief
*   Init state before DDR clock switch
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel    channel to initialize for
*   @param[in]  curr_clk_khz Current CLK freq in KHZ before clock switch
*   @param[in]  new_clk_khz  New CLK freq in KHZ after clock switch
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
boolean HAL_DDR_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz,
                                 uint32 new_clk_khz);



/* =============================================================================
**  Function : HAL_DDR_Post_Clock_Switch
** =============================================================================
*/
/**
*   @brief
*   Init state after DDR clock switch
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel    channel to initialize for
*   @param[in]  curr_clk_khz Current CLK freq in KHZ before clock switch
*   @param[in]  new_clk_khz  New CLK freq in KHZ after clock switch
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
boolean HAL_DDR_Post_Clock_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_clk_khz,
                                  uint32 new_clk_khz);

/* =============================================================================
**  Function : HAL_DDR_Pre_Voltage_Switch
** =============================================================================
*/
/**
*   @brief
*   Init state before DDR voltage switch
*
*   @param[in]  ddr             Pointer to ddr conifiguration struct
*   @param[in]  channel       channel to initialize for
*   @param[in]  curr_volt_level Current voltage level before voltage switch
*   @param[in]  new_volt_level  New voltage level after voltage switch
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
boolean HAL_DDR_Pre_Voltage_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_volt_level,
                                   uint32 new_volt_level);

/* =============================================================================
**  Function : HAL_DDR_Post_Voltage_Switch
** =============================================================================
*/
/**
*   @brief
*   Init state after DDR voltage switch
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel    channel to initialize for
*   @param[in]  curr_volt_level Current voltage level before voltage switch
*   @param[in]  new_volt_level  New voltage level after voltage switch
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
boolean HAL_DDR_Post_Voltage_Switch(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint32 curr_volt_level,
                                    uint32 new_volt_level);

/* ============================================================================
**  Function : HAL_DDR_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function triggers the ddr to enter self refresh mode for channel EBI0/1
*
*   @details
*   This function triggers the ddr to enter self refresh mode for channel EBI0/1
*
*
*   @par Dependencies
*   This code must not reside on the ddr that is being put to self refresh.
*
*   @par Side Effects
*   DDR is in self refresh mode and if both CS is selected, or only 1 CS is physically
*   present, then the ddr clock is turned off, at this point, any writes to ddr will
*   fail since the controller is not going to respond.
*
*   @retval  None
*
*   @sa None
*/
boolean HAL_DDR_Enter_Self_Refresh(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                   DDR_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_DDR_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function triggers the ddr to exit self refresh mode for channel EBI0/1
*
*   @details
*   This function triggers the ddr to exit self refresh mode for channel EBI0/1
*
*   @par Dependencies
*
*
*   @par Side Effects
*
*   @retval  None
*
*   @sa None
*/
boolean HAL_DDR_Exit_Self_Refresh(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                  DDR_CHIPSELECT chip_select);

/* =============================================================================
**  Function : HAL_DDR_Enter_Power_Collapse
** =============================================================================
*/
/**
*   @brief
*   Init state before entry of power collapse
*
*   @param[in]  ddr          Pointer to ddr conifiguration struct
*   @param[in]  channel    channel to initialize for
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
boolean HAL_DDR_Enter_Power_Collapse(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_POWER_CLPS_MODE collapse_mode, 
                                     uint32 clk_freq_khz);
/* =============================================================================
**  Function : HAL_DDR_Exit_Power_Collapse
** =============================================================================
*/
/**
*   @brief
*   Init state after exit of power collapse
*
*   @param[in]  ddr          Pointer to ddr configuration struct
*   @param[in]  channel    channel to initialize for
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
boolean HAL_DDR_Exit_Power_Collapse(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_POWER_CLPS_MODE collapse_mode, 
                                    uint32 clk_freq_khz);
/* ============================================================================
**  Function : HAL_DDR_Enter_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr into DPD mode
*
*   @details
*   This function will trigger the ddr to be put into deep power down mode.
*   In the case where we only have 1 device on the channel (CS0 only), or both
*   chip select are chosen, we will also turn off ddr clock.
*
*   @param channel   -  [IN] channel to configure DPD for
*
*   @par Dependencies
*   The desired CS to be configured for Deep Power Down Must be configured first by
*   calling HAL_DDR_Chipselect().
*
*   @par Side Effects
*   After ddr has gone into Deep Power Down, all the data stored will be lost.
*   Also, when returning from deep power down, we must reinitialize the DDR.
*
*   @retval  None
*
*   @sa None
*/
boolean HAL_DDR_Enter_Deep_Power_Down(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                      DDR_CHIPSELECT);

/* ============================================================================
**  Function : HAL_DDR_Exit_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   Brief_Description_Goes_here
*
*   @details
*   Detailed_Description_Goes_here
*
*   @param clkspeed   -  [IN] Clock speed to reinitialize ddr for
*
*   @par Dependencies
*   Clock must be setup for initialization speed.
*   LPDDR1 - no limitation
*   LPDDR2 - < 50Mhz
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean HAL_DDR_Exit_Deep_Power_Down(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                     DDR_CHIPSELECT, uint32 clkspeed);

/* =============================================================================
**  Function : HAL_DDR_Read_Mode_Register
** =============================================================================
*/
/**
*   @brief
*   Read DDR mode register.
*
*   @param[in]  channel    channel to read mode register for
*   @param[in]  chip_select  Chip select to read mode register for
*   @param[in]  mr_addr      Mode register address
*
*   @return
*   Mode register data that is read back.
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
uint32 HAL_DDR_Read_Mode_Register(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                  DDR_CHIPSELECT chip_select, uint32 mr_addr);

/* =============================================================================
**  Function : HAL_DDR_Write_Mode_Register
** =============================================================================
*/
/**
*   @brief
*   Write DDR mode register.
*
*   @param[in]  channel    channel to write mode register for
*   @param[in]  chip_select  Chip select to write mode register for
*   @param[in]  mr_addr      Mode register address
*   @param[in]  mr_data      Mode register data to write
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
boolean HAL_DDR_Write_Mode_Register(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel,
                                    DDR_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data);

/* =============================================================================
**  Function : HAL_DDR_ZQ_Calibration
** =============================================================================
*/
/**
*   @brief
*   Perform manual ZQ calibration.
*
*   @param[in]  channel    Channel to perform ZQ calibration on.
*   @param[in]  chip_select  Chip select to perform ZQ calibration on.
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
boolean HAL_DDR_ZQ_Calibration (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/* =============================================================================
**  Function : HAL_DDR_IOCTL
** =============================================================================
*/
/**
*   @brief
*   Standard IOCTL API for various controls, debug, get/set APIs.
*
*   @param[in]  ddr         Pointer to ddr configuration struct.
*   @param[in]  cmd         Indicates the type of IOCTL commands.
*   @param[in]  arg         Pointer to structure/union of all possible arguments.
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
boolean HAL_DDR_IOCTL (BOOT_DDR_STRUCT *ddr, IOCTL_CMD ioctl_cmd, IOCTL_ARG *ioctl_arg);

/* =============================================================================
**  Function : HAL_DDR_LLCC_cacheable_mode_override_ctrl
** =============================================================================
*/
/**
*   @brief
*   Set the override mode for LLCC
*
*   @param[in]  ddr         Pointer to ddr configuration struct.
*   @param[in]  channel     DDR Channel
*   @param[in]  cachable    Cache Mode
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
boolean HAL_DDR_LLCC_cacheable_mode_override_ctrl(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, CACHEABLE_MODE cachable_mode);

/* =============================================================================
**  Function : HAL_DDR_MC_All_Periodic_Ctrl
** =============================================================================
*/
/**
*   @brief
*   Set the override mode for LLCC
*
*   @param[in]  ddr         Pointer to ddr configuration struct.
*   @param[in]  channel     DDR Channel
*   @param[in]  chip_select DDR chipselect
*   @param[in]  enable      enable/disable
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
boolean HAL_DDR_MC_All_Periodic_Ctrl(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

/* =============================================================================
**  Function : HAL_DDR_MC_Power_Saving_Ctrl
** =============================================================================
*/
/**
*   @brief
*   Set the override mode for LLCC
*
*   @param[in]  ddr         Pointer to ddr configuration struct.
*   @param[in]  channel     DDR Channel
*   @param[in]  chip_select DDR chipselect
*   @param[in]  enable      enable/disable
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
boolean HAL_DDR_MC_Power_Saving_Ctrl(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

/* =============================================================================
**  Function : HAL_DDR_rd_dqdqs_coarse_fine_plot_pivot
** =============================================================================
*/
/**
*   @brief
*   Plot read coarse/fine eye at a specific frequency
*
*   @param[in]  ddr                 Pointer to ddr configuration struct.
*   @param[in]  clk_freq_khz        Frequency in KHz for which to plot.
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
boolean HAL_DDR_rd_dqdqs_coarse_fine_plot_pivot(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, uint64* output_iter);

/* =============================================================================
**  Function : HAL_DDR_wr_dqdqs_coarse_fine_plot_pivot
** =============================================================================
*/
/**
*   @brief
*   Plot write coarse/fine eye at a specific frequency
*
*   @param[in]  ddr                 Pointer to ddr configuration struct.
*   @param[in]  clk_freq_khz        Frequency in KHz for which to plot.
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
boolean HAL_DDR_wr_dqdqs_coarse_fine_plot_pivot(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, uint64* output_iter);

/* =============================================================================
**  Function : Enable/Disable clock gating for DDRSS/LLCC/SHRM/MC/MCCC
** =============================================================================
*/
/**
*   @brief
*   Enable Disable Clock Gating for DDRSS LLCC SHRM MC MCCC
*
*   @param[in]  ddr                 Pointer to ddr configuration struct.
*   @param[in]  enable              To enable/disable clock gating enable=1/enable=0
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
void HAL_DDR_Clock_Gating_Ctrl(BOOT_DDR_STRUCT *ddr, uint8 enable);

/* =============================================================================
**  Function : Read eCDT parameter values
** =============================================================================
*/
/**
*   @brief
*   Read eCDT parameter values
*
*   @param[in]  ddr                 Pointer to ddr configuration struct.
*   @param[in]  clk_freq_khz        Frequency in KHz for which to read values.
*   @param[in]  buffer_ptr          Pointer to output buffer.
*   @param[in]  buffer_size         Size of buffer in bytes.
*
*   @retval     Number of bytes used in buffer or -1 if an error occurred.
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
int HAL_DDR_Read_eCDT_Parameter_Values(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, void* buffer_ptr, size_t buffer_size);
#endif /* __HAL_SNS_DDR_H__ */
