#ifndef __HALCLKRENOVOPLL_H__
#define __HALCLKRENOVOPLL_H__
/*
===========================================================================
*/
/**
  @file HALclkRenovoPLL.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8998 (Nazgul) [nazgul_v1.0_p3q3r34]

  This file contains HWIO register definitions for the following modules:
    WCSS_HM_A_PMM_PMM_DEC

  'Include' filters applied: WCSS_HM_A_PMM_WSI_CMD[WCSS_HM_A_PMM_PMM_DEC] WCSS_HM_A_PMM_WSIM_CTL[WCSS_HM_A_PMM_PMM_DEC] WCSS_HM_A_PMM_REG_READ_DATA[WCSS_HM_A_PMM_PMM_DEC]
  'Exclude' filters applied: RESERVED DUMMY
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/hal/hw/v1/HALclkRenovoPLL.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALclkInternal.h"


/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * HWIO base definitions
 */
extern uint32                      HAL_clk_nHWIOBaseWCSS;
#define WCSS_WRAPPER_BASE          HAL_clk_nHWIOBaseWCSS
#define WCSS_WRAPPER_BASE_PHYS     0x18800000
#define WCSS_WRAPPER_BASE_SIZE     0x00800000

/*
 * PLL register offsets.
 */
#define HWIO_PLL_MODE_OFFS                                  0x00000000
#define HWIO_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_PLL_ALPHA_VAL_OFFS                             0x00000008
#define HWIO_PLL_USER_CTL_OFFS                              0x0000000c
#define HWIO_PLL_CONFIG_CTL_OFFS                            0x00000010
#define HWIO_PLL_TEST_CTL_OFFS                              0x00000014
#define HWIO_PLL_STATUS_OFFS                                0x00000018

/*
 * PLL_MODE register definitions.
 */
#define HWIO_PLL_MODE_PLL_LOCK_DET_BMSK                     0x80000000
#define HWIO_PLL_MODE_PLL_LOCK_DET_SHFT                           0x1f
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                  0x40000000
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                        0x1e
#define HWIO_PLL_MODE_PLL_ACK_LATCH_BMSK                    0x20000000
#define HWIO_PLL_MODE_PLL_ACK_LATCH_SHFT                          0x1d
#define HWIO_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK         0x800000
#define HWIO_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT             0x17
#define HWIO_PLL_MODE_PLL_UPDATE_BMSK                         0x400000
#define HWIO_PLL_MODE_PLL_UPDATE_SHFT                             0x16
#define HWIO_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_PLL_USER_CTL_ALPHA_EN_BMSK                      0x1000000
#define HWIO_PLL_USER_CTL_ALPHA_EN_SHFT                           0x18
#define HWIO_PLL_USER_CTL_PDIV_CTL_BMSK                         0x1c00
#define HWIO_PLL_USER_CTL_PDIV_CTL_SHFT                            0xa
#define HWIO_PLL_USER_CTL_POSTDIV_CTL_BMSK                       0x300
#define HWIO_PLL_USER_CTL_POSTDIV_CTL_SHFT                         0x8
#define HWIO_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*----------------------------------------------------------------------------
 * MODULE: WCSS_HM_A_PMM_PMM_DEC
 *--------------------------------------------------------------------------*/

#define WCSS_HM_A_PMM_PMM_DEC_REG_BASE                                                       (WCSS_WRAPPER_BASE      + 0x00080000)
#define WCSS_HM_A_PMM_PMM_DEC_REG_BASE_PHYS                                                  (WCSS_WRAPPER_BASE_PHYS + 0x00080000)
#define WCSS_HM_A_PMM_PMM_DEC_REG_BASE_OFFS                                                  0x00080000

#define HWIO_WCSS_HM_A_PMM_WSI_CMD_ADDR                                                      (WCSS_HM_A_PMM_PMM_DEC_REG_BASE      + 0x000000e0)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_PHYS                                                      (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_PHYS + 0x000000e0)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_OFFS                                                      (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_OFFS + 0x000000e0)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_RMSK                                                      0xf1ff07ff
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_IN          \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_WSI_CMD_ADDR, HWIO_WCSS_HM_A_PMM_WSI_CMD_RMSK)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_INM(m)      \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_WSI_CMD_ADDR, m)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_OUT(v)      \
        out_dword(HWIO_WCSS_HM_A_PMM_WSI_CMD_ADDR,v)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WCSS_HM_A_PMM_WSI_CMD_ADDR,m,v,HWIO_WCSS_HM_A_PMM_WSI_CMD_IN)
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_RF_CMD_IP_BMSK                                            0x80000000
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_RF_CMD_IP_SHFT                                                  0x1f
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_WSI_ERR_STATUS_BMSK                                       0x70000000
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_WSI_ERR_STATUS_SHFT                                             0x1c
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_PMM_WSI_SM_BMSK                                            0x1ff0000
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_PMM_WSI_SM_SHFT                                                 0x10
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_DISABLE_WSI_ERROR_CHECKING_BMSK                                0x400
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_DISABLE_WSI_ERROR_CHECKING_SHFT                                  0xa
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_FORCE_IDLE_BMSK                                             0x200
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_FORCE_IDLE_SHFT                                               0x9
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_XO_DIS_BMSK                                                 0x100
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_XO_DIS_SHFT                                                   0x8
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_XO_EN_BMSK                                                   0x80
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_XO_EN_SHFT                                                    0x7
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_REG_WRITE_BMSK                                               0x40
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_REG_WRITE_SHFT                                                0x6
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_REG_READ_BMSK                                                0x20
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_REG_READ_SHFT                                                 0x5
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_RF_RESET_BMSK                                                0x10
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_RF_RESET_SHFT                                                 0x4
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_BUS_SYNC_BMSK                                                 0x8
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_BUS_SYNC_SHFT                                                 0x3
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_USE_PMM_WSI_BMSK                                              0x4
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_SW_USE_PMM_WSI_SHFT                                              0x2
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_USE_WLAN2_WSI_BMSK                                               0x2
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_USE_WLAN2_WSI_SHFT                                               0x1
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_USE_WLAN1_WSI_BMSK                                               0x1
#define HWIO_WCSS_HM_A_PMM_WSI_CMD_USE_WLAN1_WSI_SHFT                                               0x0

#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_ADDR                                                     (WCSS_HM_A_PMM_PMM_DEC_REG_BASE      + 0x000000e4)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_PHYS                                                     (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_PHYS + 0x000000e4)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_OFFS                                                     (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_OFFS + 0x000000e4)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_RMSK                                                     0x7fffff7f
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_IN          \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_WSIM_CTL_ADDR, HWIO_WCSS_HM_A_PMM_WSIM_CTL_RMSK)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_INM(m)      \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_WSIM_CTL_ADDR, m)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_OUT(v)      \
        out_dword(HWIO_WCSS_HM_A_PMM_WSIM_CTL_ADDR,v)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WCSS_HM_A_PMM_WSIM_CTL_ADDR,m,v,HWIO_WCSS_HM_A_PMM_WSIM_CTL_IN)
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_WSI2M_STATE_BMSK                             0x7c000000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_WSI2M_STATE_SHFT                                   0x1a
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SYNCH_STATE_BMSK                                    0x2000000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SYNCH_STATE_SHFT                                         0x19
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SL_ASSERT_ASYNC_BMSK                                0x1000000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SL_ASSERT_ASYNC_SHFT                                     0x18
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SL_ASSERT_BMSK                                       0x800000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_SL_ASSERT_SHFT                                           0x17
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_RX_DVAL_BMSK                                   0x400000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_RX_DVAL_SHFT                                       0x16
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_RX_DATA_BMSK                                   0x200000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_RX_DATA_SHFT                                       0x15
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_RESYNCH_REQ_STB_BMSK                                 0x100000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_RESYNCH_REQ_STB_SHFT                                     0x14
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_RD_ACT_BMSK                                       0xc0000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_RD_ACT_SHFT                                          0x12
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_PREAMBLE_DLY_BMSK                                 0x30000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_PREAMBLE_DLY_SHFT                                    0x10
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_RD_DLY_BMSK                                        0xf000
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_RD_DLY_SHFT                                           0xc
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_BP_LIMIT_BMSK                                       0xf00
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_BP_LIMIT_SHFT                                         0x8
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_IGNORE_ACK_BMSK                                    0x40
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_IGNORE_ACK_SHFT                                     0x6
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_FORCE_ERROR_BMSK                                   0x20
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_DEBUG_FORCE_ERROR_SHFT                                    0x5
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_SC_BEFORE_OTHER_CMDS_BMSK                            0x10
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_SC_BEFORE_OTHER_CMDS_SHFT                             0x4
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_SC_BEFORE_ALL_CMDS_BMSK                               0x8
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_SC_BEFORE_ALL_CMDS_SHFT                               0x3
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_MONITOR_CMD_BMSK                                      0x4
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_MONITOR_CMD_SHFT                                      0x2
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_DISABLE_WSI_BMSK                                      0x2
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_DISABLE_WSI_SHFT                                      0x1
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_AUTO_SYNCH_MODE_BMSK                                  0x1
#define HWIO_WCSS_HM_A_PMM_WSIM_CTL_WSI2M_CFG_AUTO_SYNCH_MODE_SHFT                                  0x0

#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_ADDR                                                  (WCSS_HM_A_PMM_PMM_DEC_REG_BASE      + 0x000000f0)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_PHYS                                                  (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_PHYS + 0x000000f0)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_OFFS                                                  (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_OFFS + 0x000000f0)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_RMSK                                                      0xffff
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_IN          \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_ADDR, HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_RMSK)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_INM(m)      \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_ADDR, m)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_OUT(v)      \
        out_dword(HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_ADDR,v)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_ADDR,m,v,HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_IN)
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_SW_REG_RW_ADDR_BMSK                                       0xffff
#define HWIO_WCSS_HM_A_PMM_REG_RW_ADDR_SW_REG_RW_ADDR_SHFT                                          0x0

#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_ADDR                                                (WCSS_HM_A_PMM_PMM_DEC_REG_BASE      + 0x000000f8)
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_PHYS                                                (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_PHYS + 0x000000f8)
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_OFFS                                                (WCSS_HM_A_PMM_PMM_DEC_REG_BASE_OFFS + 0x000000f8)
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_RMSK                                                0xffffffff
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_IN          \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_REG_READ_DATA_ADDR, HWIO_WCSS_HM_A_PMM_REG_READ_DATA_RMSK)
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_INM(m)      \
        in_dword_masked(HWIO_WCSS_HM_A_PMM_REG_READ_DATA_ADDR, m)
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_SW_REG_RD_DATA_BMSK                                 0xffffffff
#define HWIO_WCSS_HM_A_PMM_REG_READ_DATA_SW_REG_RD_DATA_SHFT                                        0x0


/* ============================================================================
**    Functions
** ==========================================================================*/

/*
 * Source control functions.  These can be used if some of the control
 * needs to be overridden by a specific PLL.
 */
static boolean HAL_clk_RenovoPLLEnable           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static void    HAL_clk_RenovoPLLDisable          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static boolean HAL_clk_RenovoPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_RenovoPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
static boolean HAL_clk_RenovoPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode);
static void    HAL_clk_RenovoPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
static boolean HAL_clk_RenovoPLLIsPLLConfigEqual ( const HAL_clk_PLLConfigType *pConfigA, const HAL_clk_PLLConfigType *pConfigB);

#endif  /* __HALCLKRENOVOPLL_H__ */

