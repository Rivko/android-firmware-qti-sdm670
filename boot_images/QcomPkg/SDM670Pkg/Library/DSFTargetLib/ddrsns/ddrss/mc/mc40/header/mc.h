/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
================================================================================*/

#ifndef _MC_H
#define _MC_H

#include "mc_data.h"
#include "mc_common.h"

#define DDR_BUS_WIDTH_c                                              16
#define NUM_QOS_REGS_c                                               4
#define NUM_INTR_MASTERS_c                                           3
#define NUM_RANKS_c                                                  2
#define NUM_CBUS_c                                                   1
#define NUM_PWRS_c                                                   4
#define GLOBAL_NUM_QOS_REGS_c                                        4
#define GLOBAL_NUM_CBUS_c                                            1
#define ISU_NUM_QOS_REGS_c                                           4
#define ISU_NUM_CBUS_c                                               1
#define ISU_NUM_RANKS_c                                              2
#define CBU_TOP_NUM_QOS_REGS_c                                       4
#define CBU_TOP_NUM_CBUS_c                                           1
#define MPE_NUM_QOS_REGS_c                                           4
#define MPE_NUM_RANKS_c                                              2
#define DTC_NUM_RANKS_c                                              2
#define SHKE_NUM_QOS_REGS_c                                          4
#define NUM_FSW_TASK_c                                               16
#define SHKE_NUM_RANKS_c                                             2
#define NUM_TIMERS_c                                                 5
#define NUM_BYTES_c                                                  2
#define NUM_CMDS_c                                                   24
#define OSU_NUM_QOS_REGS_c                                           4
#define OSU_NUM_RANKS_c                                              2
#define OSU_NUM_PWRS_c                                               4
#define INTERRUPT_NUM_INTR_MASTERS_c                                 3
#define GLOBAL_SEC_NUM_RANKS_c                                       2

// Base address from MC point view before addr remapping, should always start from 0
#define MC_BASE_ADDR                     (0x0) 

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  MC Initialization Functions
** =============================================================================
*/
/**
 * @brief MC one-time settings
 *
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr conifiguration struct      
 * @param[in] DDR_CHANNEL channel:  Channel selection    
 *
 * @retval None
 */
void MC_Config (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);

/**
 * @brief MC Pre_init_1 setup 
 *        Update AC timing parameters from CDT and 
 *        set SHKE_CONFG[RANK_EN]=0x3 for freq switch to proceed to completion
 *
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr configuration struct        
 * @param[in] DDR_CHANNEL channel:  Channel selection    
 *
 * @retval None
 *
 * @note To be called before switching to DDR boot frequency 
 */
void MC_Pre_Init_Setup_1 (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);

/**
 * @brief MC Pre_init_2 setup 
 *        Put CKE low and turn CK off. 
 *        Set SHKE_CONFIG[RANK_EN] back to 0  
 *
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr configuration struct        
 * @param[in] DDR_CHANNEL channel:  Channel selection   
 *
 * @retval None
 *
 * @note To be called after switching to DDR boot frequency 
 */
void MC_Pre_Init_Setup_2 (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);

/** 
 * @brief Device initialization. 
 *        Select LPDDR4 initialization routines for enabling CK and CKE.
 *        Does ZQ calibration and RL/WL programming
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr configuration struct          
 * @param[in] DDR_CHANNEL channel:  Channel selection   
 * 
 * @retval None
 */
void MC_Memory_Device_Init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);

/**
 * @brief This function detects DRAM Geometry. An MR8 read provides the rank density information.
 *        Accordingly, the number of rows and columns is filled out from a Look-Up Table (LUT).
 *
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr configuration struct      
 * @param[in] uint8  ch:            memory controller channel/instance number
 * @param[in] uint8  cs:            rank selection value
 * @param[in] uint32 mr8_value:     JEDEC_MR8 value  
 *
 * @retval Always TRUE
 */
boolean MC_DDR_Geometry_Detection(BOOT_DDR_STRUCT *ddr, uint8 ch,\
                                  uint8 cs, uint32 mr8_value);

/**
 * @brief This function auto-detects the presence of a rank.
 *        If a rank is present, it proceeds to finding the geometry (and subsequently num rank/cols).
 *        If a rank is not present or not requested (a timed-out MRR), it disables the rank and CKE.
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr:        Pointer to ddr configuration struct           
 * @param[in] DDR_CHANNEL channel:         Channel selection   
 * @param[in] DDR_CHIPSELECT chip_select:  Chip select to initialize for
 *
 * @retval None
 */        
void MC_DDR_Topology_Detection(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/**
 * @brief MC DRAM Address Setting up
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr:       Pointer to ddr configuration struct         
 * @param[in] DDR_CHANNEL channel:        Channel selection     
 * @param[in] DDR_CHIPSELECT chip_select: rank selection    
 * 
 * @retval None
 */
void MC_DDR_Addr_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/**
 * @brief Post initialization sequence
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr configuration struct 
 * @param[in] DDR_CHANNEL channel:  Channel selection   
 * 
 * @retval None
 */  							
void MC_Post_Init_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);


/* =============================================================================
**  MC Internal Functions
** =============================================================================
*/
/**
 * @brief Mode Register write can use for all channels and both ranks
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr:       Pointer to DDR configuration struct
 * @param[in] DDR_CHANNEL channel:        channel selection
 * @param[in] DDR_CHIPSELECT chip_select: rank selection
 * @param[in] uint32 mr_addr:             MR address value
 * @param[in] uint32 mr_data:             MR data value
 * 
 * @retval None
 */
void MC_MR_Write (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data);

/**
 * @brief Mode register read function only returns a single MR value, it reads on per channel per rank basis
 * 
 * @param[in] DDR_CHANNEL channel:        channel selection
 * @param[in] DDR_CHIPSELECT chip_select: rank selection
 * @param[in] uint32 mr_addr:             MR address value
 * 
 * @retval uint32 MR read back value
 * 
 * @note Both rank option (DDR_CS_BOTH) is not supported for DDR_CHIPSELECT
 */
uint32 MC_MR_Read (DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint32 mr_addr);

/**
 * @brief Kick off timing parameter calculation and update all timing values
 *
 * @param[in] uint8 ch: memory controller channel/instance number 
 *
 * @retval None
 */ 
void MC_Timing_Recalc_Update (uint8 ch);

/**
 * @brief Enable/Disable periodic auto refresh
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr:       Pointer to ddr configuration struct   
 * @param[in] DDR_CHANNEL channel:        Channel selection    
 * @param[in] DDR_CHIPSELECT chip_select: rank selection    
 * @param[in] uint8  enable:              0x0->disable, 0x1->enable
 * 
 * @retval None
 */
void MC_Periodic_Auto_Refresh_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

/**
 * @brief Periodic events controlled through this function are
 *        1. Periodic refresh
 *        2. Periodic MR4 read
 *        3. Periodic ZQ calibration
 *        4. Periodic DIT
 *
 * @param[in] DDR_CHANNEL channel:        Channel selection   
 * @param[in] DDR_CHIPSELECT chip_select: rank selection       
 * @param[in] uint8 enable:               0x0->disable, 0x1->enable       
 *
 * @retval None 
 */
void MC_All_Periodic_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

/**
 * @brief Power saving features controlled through this function are
 *        1. CSPD
 *        2. HW activity based self-refresh
 *        3. Root clock gating
 *        4. Power state transitions
 *
 * @param[in] BOOT_DDR_STRUCT *ddr:       Pointer to ddr configuration struct       
 * @param[in] DDR_CHANNEL channel:        Channel selection   
 * @param[in] DDR_CHIPSELECT chip_select: rank selection       
 * @param[in] uint8 enable:               0x0->disable, 0x1->enable       
 *
 * @retval None 
 */
void MC_Power_Saving_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

/**
 * @brief Manual ZQ calibration for LPDDR4
 *        This function supports ZQ Cal per rank and both ranks. While using for both ranks, ZQCAL_START
 *        is issued for one rank followed by next rank. ZQCAL_LATCH for both ranks are issued together.
 * @param[in] BOOT_DR_STRUCT *ddr:        ddr configuration pointer
 * @param[in] DDR_CHANNEL channel:        Channel selection   
 * @param[in] DDR_CHIPSELECT chip_select: rank selection  
 *
 * @retval None
 */
void MC_ZQ_Calibration (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);


/* =============================================================================
**  MC Watchdog Sequence
** =============================================================================
*/
/**
 * @brief Watchdog reset setup
 *
 * @param[in] BOOT_DDR_STRUCT *ddr:       Pointer to ddr configuration struct         
 * @param[in] DDR_CHANNEL channel:        Channel selection 
 * @param[in] DDR_CHIPSELECT chip_select: rank selection  
 *
 * @retval None
 */ 
void MC_Watchdog_Reset_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);


#endif
