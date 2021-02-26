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

#ifndef _MC_COMMON_H
#define _MC_COMMON_H

#include "cabo_seq_hwiobase.h"
#include "cabo_seq_hwioreg.h"
#include "HAL_SNS_DDR.h"
#include "ddrss_common.h"


#define REG_OFFSET_MC(uint8)          ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_OFFSET))
									   
#define REG_OFFSET_MC_GLOBAL(uint8)   ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_GLOBAL_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_GLOBAL_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_GLOBAL_OFFSET))
#define REG_OFFSET_MC_ISU(uint8)      ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_ISU_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_ISU_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_ISU_OFFSET)) 
#define REG_OFFSET_MC_CBU_TOP(uint8)  ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_CBU_TOP_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_CBU_TOP_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_CBU_TOP_OFFSET)) 
#define REG_OFFSET_MC_MPE(uint8)      ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_MPE_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_MPE_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_MPE_OFFSET)) 
#define REG_OFFSET_MC_DTC(uint8)      ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_DTC_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_DTC_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_DTC_OFFSET)) 
#define REG_OFFSET_MC_SHKE(uint8)     ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_SHKE_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_SHKE_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_SHKE_OFFSET)) 
#define REG_OFFSET_MC_OSU(uint8)      ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_OSU_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_OSU_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_OSU_OFFSET)) 
#define REG_OFFSET_MC_PXI(uint8)      ((uint8 == 0) ? \
                                       (SEQ_DDR_SS_CABO0_CABO_PXI_OFFSET) : \
                                       (uint8 == 1) ? \
                                       (SEQ_DDR_SS_CABO1_CABO_PXI_OFFSET) : \
                                       (SEQ_DDR_SS_CABO_BROADCAST_CABO_PXI_OFFSET)) 
#define REG_OFFSET_MC_INTERRUPT(uint8) ((uint8 == 0) ? \
                                        (SEQ_DDR_SS_CABO0_CABO_INTERRUPT_OFFSET) : \
                                        (uint8 == 1) ? \
                                        (SEQ_DDR_SS_CABO1_CABO_INTERRUPT_OFFSET) : \
                                        (SEQ_DDR_SS_CABO_BROADCAST_CABO_INTERRUPT_OFFSET)) 
#define REG_OFFSET_MC_PERFMON(uint8)   ((uint8 == 0) ? \
                                        (SEQ_DDR_SS_CABO0_CABO_PERFMON_OFFSET) : \
                                        (uint8 == 1) ? \
                                        (SEQ_DDR_SS_CABO1_CABO_PERFMON_OFFSET) : \
                                        (SEQ_DDR_SS_CABO_BROADCAST_CABO_PERFMON_OFFSET)) 
#define REG_OFFSET_MC_GLOBAL_SEC(uint8) ((uint8 == 0) ? \
                                         (SEQ_DDR_SS_CABO0_CABO_GLOBAL_SEC_OFFSET) : \
                                         (uint8 == 1) ? \
                                         (SEQ_DDR_SS_CABO1_CABO_GLOBAL_SEC_OFFSET) : \
                                         (SEQ_DDR_SS_CABO_BROADCAST_CABO_GLOBAL_SEC_OFFSET)) 
#define REG_OFFSET_MC_PXI_SECURE(uint8) ((uint8 == 0) ? \
                                         (SEQ_DDR_SS_CABO0_CABO_PXI_SECURE_OFFSET) : \
                                         (uint8 == 1) ? \
                                         (SEQ_DDR_SS_CABO1_CABO_PXI_SECURE_OFFSET) : \
                                         (SEQ_DDR_SS_CABO_BROADCAST_CABO_PXI_SECURE_OFFSET)) 										 

#define CH_1HOT(uint8)                  ((uint8 == 0) ? \
                                         DDR_CH0 : \
                                         (uint8 == 1) ? \
                                         DDR_CH1 : \
                                         (uint8 == 2) ? \
                                         DDR_CH2 : \
                                         DDR_CH3)

#define CS_1HOT(uint8)                  ((uint8 == 0) ? \
                                         DDR_CS0 : \
                                         DDR_CS1)

#define MC_CH_INX(DDR_CHANNEL)          ((DDR_CHANNEL == DDR_CH0) ? \
                                         0: \
                                         (DDR_CHANNEL == DDR_CH1) ? \
                                         1: \
                                         (DDR_CHANNEL == DDR_CH2) ? \
                                         2: \
                                         (DDR_CHANNEL == DDR_CH3) ? \
                                         3: \
                                         4)

#define CS_INX(DDR_CHIPSELECT)          ((DDR_CHIPSELECT == DDR_CS0) ? \
                                         0 : \
                                         1)

#define XO_PERIOD_IN_PS                 (52080)   //52.08ns, based on XO = 19.2MHz

typedef enum
{
    WAIT_XO_CLOCK    = 0x0,
    WAIT_SLEEP_CLOCK = 0x1,
    WAIT_CORE_CLOCK  = 0x2
} MC_Wait_Timer_Domain;

typedef enum
{
    RL_DBI_OFF = 0x0 ,
    RL_DBI_ON  = 0x1 ,
    WL         = 0x2 ,
    MR2_WR_VAL = 0x3
} RL_WL_Table_Sel;

typedef enum
{
    DEVICE_WIDTH_IS_x16 = 0x0 ,
    DEVICE_WIDTH_IS_x8  = 0x1 
} Device_Io_Width_sel;

// #defines for all valid MRs
#define JEDEC_MR_0   0x0
#define JEDEC_MR_1   0x1
#define JEDEC_MR_2   0x2
#define JEDEC_MR_3   0x3
#define JEDEC_MR_4   0x4
#define JEDEC_MR_5   0x5
#define JEDEC_MR_6   0x6
#define JEDEC_MR_7   0x7
#define JEDEC_MR_8   0x8
#define JEDEC_MR_9   0x9
#define JEDEC_MR_11  0xB
#define JEDEC_MR_12  0xC
#define JEDEC_MR_13  0xD
#define JEDEC_MR_14  0xE
#define JEDEC_MR_15  0xF
#define JEDEC_MR_16  0x10
#define JEDEC_MR_17  0x11
#define JEDEC_MR_18  0x12
#define JEDEC_MR_19  0x13
#define JEDEC_MR_20  0x14
#define JEDEC_MR_22  0x16
#define JEDEC_MR_23  0x17
#define JEDEC_MR_24  0x18
#define JEDEC_MR_32  0x20
#define JEDEC_MR_39  0x27
#define JEDEC_MR_40  0x28

#define MR13_BASE (1 << 5 | 1 << 4) // Set OP[4]=1 (RRO : refresh rate option) and OP[5] =1 (DMD : data mask disable)
typedef enum
{
   CMD_REFRESH_AB            = 0x40,
   CMD_SELF_REFRESH_SRE      = 0x50,
   CMD_SELF_REFRESH_SRE_ASAP = 0x51,
   CMD_SELF_REFRESH_SRX      = 0x54,
   CMD_POWER_DOWN_PDE        = 0x60,
   CMD_POWER_DOWN_PDX        = 0x64,
   CMD_POWER_DOWN_CSPDE      = 0x68,
   CMD_POWER_DOWN_CSPDX      = 0x6C,
   CMD_MRR                   = 0x70,
   CMD_MRR_MR4               = 0x71,
   CMD_MRW                   = 0x74,
   CMD_MRW_FSP_OP            = 0x75,
   CMD_MRW_VRCG_EN           = 0x76,
   CMD_MRW_VRCG_DIS          = 0x77,
   CMD_MPC_WRFIFO            = 0x80,
   CMD_MPC_RDFIFO            = 0x84,
   CMD_MPC_RDDQCAL           = 0x88, 
   CMD_MPC_ZQCAL_LATCH       = 0x8C,
   CMD_MPC_ZQCAL_START       = 0x90,
   CMD_MPC_DQS_OSC_START     = 0x94,
   CMD_MPC_DQS_OSC_STOP      = 0x98,  
   CMD_INIT_CK_ON            = 0xA0,
   CMD_INIT_CK_OFF           = 0xA4,
   CMD_CUSTOM_CA             = 0xB0,
   CMD_CUSTOM_DQ             = 0xB4,
   CMD_MCPHY_FCS_STALL       = 0xC0,
   CMD_MCPHY_FCS_STALL_EXIT  = 0xC1,
   CMD_MC_SW_STALL           = 0xC4,
   CMD_MC_SW_STALL_EXIT      = 0xC5,
   CMD_NOP                   = 0x00
} MC_SHKE_CMD_EVENT;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  MC Common Functions
** =============================================================================
*/
/**
 * @brief Configure SHKE_CMD_SET_CFG_n register
 * 
 * @param[in] uint8  ch:          memory controller channel/instance number
 * @param[in] uint32 chip_select: rank selection value  (SHKE_CMD_SET_CFG_n[13:12], RANK_SEL) 
 * @param[in] uint8  reg_num:     n value in SHKE_CMD_SET_CFG_n
 * @param[in] uint32 event_sel:   event selection value (SHKE_CMD_SET_CFG_n[9:0], EVENT_SEL) 
 * @param[in] uint32 mr_addr:     MR address value      (SHKE_CMD_SET_CFG_n[23:16], MR_ADDR) 
 * @param[in] uint32 mr_data:     MR data value         (SHKE_CMD_SET_CFG_n[31:24], MR_DATA) 
 * 
 * @retval None
 */   
void MC_Cmd_Set_Cfg (uint8 ch, uint32 chip_select, uint8 reg_num,\
                     uint32 event_sel, uint32 mr_addr, uint32 mr_data);

/**
 * @brief Configure SHKE_CMD_SET_STAGGER_CFG_n register
 * 
 * @param[in] uint8  ch:         memory controller channel/instance number
 * @param[in] uint8  reg_num:    n value in SHKE_CMD_SET_STAGGER_CFG_n
 * @param[in] uint32 stagger_en: 0->concurrent issue, 1->two separate commands to Rank0 and Rank1
 *                                           (SHKE_CMD_SET_STAGGER_CFG_n[20]: STAGGER_RANKS_EN)
 * @param[in] uint32 clk_sel:    0->XO clock (19.2MHz), 1->Sleep clock (32KHz), 2->Core clock 
 *                                           (SHKE_CMD_SET_STAGGER_CFG_n[17:16], CLK_SELECT) 
 * @param[in] uint32 interval:   timer value (SHKE_CMD_SET_STAGGER_CFG_n[15:0], INTERVAL) 
 * 
 * @retval None
 */  
void MC_Cmd_Stagger_Cfg (uint8 ch, uint8 reg_num, uint32 stagger_en,\
                         uint32 clk_sel, uint32 interval);

/** 
 * @brief Trigger SHKE Command and wait for completion
 * 
 * @param[in] uint8  ch:      memory controller channel/instance number
 * @param[in] uint32 cmd_sel: command select (SHKE_SW_CMD_SEQ[23:0], CMD_SELECT)
 * 
 * @retval None
 */						 
void MC_Cmd_Trigger (uint8 ch, uint32 cmd_sel);

/**
 * @brief Create MR1 value from fields in RL_WL_lpddr_struct and mc_freq_switch_params_struct 
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm_ddr configuration struct       
 * @param[in] uint8 ch:                      memory controller channel/instance number 
 * @param[in] uint8 RL_WL_idx:               index number of RL_WL_lpddr_struct
 * @param[in] uint8 params_idx:              index number of mc_freq_switch_params_struct
 *
 * @retval uint8 MR1 value
 */ 
uint8 MC_MR1_Value (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint8 ch, uint8 RL_WL_idx, uint8 params_idx);

/**
 * @brief Based on device RL/WL/ODTLon frequency band, 
          get an index for selecting in RL/WL/ODTLon table
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm_ddr configuration struct         
 * @param[in] uint32 clk_in_ps:              Current clock frequency in pico second
 *
 * @retval uint8 Index for selecting in RL/WL/ODTLon table
 */
uint8 MC_DRAM_Latency_Index (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint32 clk_in_ps);

/**
 * @brief Based on Freq_Switch_Params band, 
 *        get an index for selecting in Freq_Switch_Params table
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to ddr configuration struct         
 * @param[in] uint32 clk_in_ps:              Current clock frequency in pico second
 *
 * @retval uint8 Index for selecting in Freq_Switch_Params table
 */
uint8 MC_Freq_Switch_Params_Index (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint32 clk_in_ps);

/**
 * @brief Enter SW self refresh
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm ddr configuration struct
 * @param[in] DDR_CHANNEL channel:           MC channel selection   
 * @param[in] DDR_CHIPSELECT chip_select:    rank selection  
 *
 * @retval None
 */  
void MC_Enter_Self_Refresh (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/**
 * @brief Exit SW self refresh
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm ddr configuration struct 
 * @param[in] DDR_CHANNEL channel:           MC channel selection   
 * @param[in] DDR_CHIPSELECT chip_select:    rank selection  
 *
 * @retval None
 *
 * @note  When execute this function, if HW self refresh is enabled, 
 *        MC will still be in self refresh until traffic comes        
 */ 
void MC_Exit_Self_Refresh (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

/**
 * @brief Enable/Disable HW activity based self refresh
 * 
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm ddr configuration struct 
 * @param[in] DDR_CHANNEL channel:        Channel selection    
 * @param[in] DDR_CHIPSELECT chip_select: rank selection    
 * @param[in] uint8  enable:              0x0->disable, 0x1->enable
 * 
 * @retval None
 */
void MC_HW_Self_Refresh_Ctrl (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel,\
                              DDR_CHIPSELECT chip_select, uint8 enable);

/**
 * @brief QFI clock gating control function
 *
 * @param[in] SHRM_DDR_STRUCT *shrm_ddr_ptr: Pointer to shrm ddr configuration struct 
 * @param[in] DDR_CHANNEL channel:           Channel selection   
 * @param[in] DDR_CHIPSELECT chip_select:    rank selection       
 * @param[in] uint8 enable:                  0x0->disable, 0x1->enable       
 *
 * @retval None 
 */
void MC_QFI_Clock_Gating_Ctrl (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable);

#endif

