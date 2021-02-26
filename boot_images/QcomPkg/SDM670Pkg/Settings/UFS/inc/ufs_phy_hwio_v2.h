#ifndef __UFS_PHY_HWIO_V2_H__
#define __UFS_PHY_HWIO_V2_H__
/*
===========================================================================
*/
/**
  @file ufs_phy_hwio_v2.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS
    UFS_MEM_MPHY_UFS
    UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL
    UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX
    UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX


  Generation parameters: 
  { u'filename': u'ufs_phy_hwio_v2.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [ u'UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS',
                  u'UFS_MEM_MPHY_UFS',
                  u'UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL',
                  u'UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX',
                  u'UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX']}
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/UFS/inc/ufs_phy_hwio_v2.h#2 $
  $DateTime: 2017/10/22 23:08:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "hwio_ufs_base.h"

/*----------------------------------------------------------------------------
 * MODULE: UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS
 *--------------------------------------------------------------------------*/

#define UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE                                                                             (UFS_MEM_UFS_1LANE_REGS_BASE      + 0x00007c00)

#define HWIO_UFS_MEM_UFS_PHY_PHY_START_ADDR                                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000000)
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_RMSK                                                                                         0x1
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PHY_START_ADDR, HWIO_UFS_MEM_UFS_PHY_PHY_START_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PHY_START_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_PHY_START_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_PHY_START_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_PHY_START_IN)
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_SERDES_START_BMSK                                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_PHY_START_SERDES_START_SHFT                                                                            0x0

#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000004)
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_RMSK                                                                                0x1
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_ADDR, HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_IN)
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_PWRDN_B_BMSK                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_PWRDN_B_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000008)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_RMSK                                                                     0x3f
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR, HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_IN)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_TIMER_20US_CORECLK_STEPS_MSB_BMSK                                        0x3f
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_TIMER_20US_CORECLK_STEPS_MSB_SHFT                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000000c)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR, HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_IN)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_TIMER_20US_CORECLK_STEPS_LSB_BMSK                                        0xff
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_TIMER_20US_CORECLK_STEPS_LSB_SHFT                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000010)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_RMSK                                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_ADDR, HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_LINE_RESET_TIME_BMSK                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_TIME_LINE_RESET_TIME_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_ADDR                                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000014)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_RMSK                                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_ADDR, HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_LINE_RESET_GRANULARITY_BMSK                                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_LINE_RESET_GRANULARITY_LINE_RESET_GRANULARITY_SHFT                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_ADDR                                                                 (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000018)
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_RMSK                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_ADDR, HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_IN)
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_CONTROLSYM_ONE_HOT_DISABLE_BMSK                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_CONTROLSYM_ONE_HOT_DISABLE_CONTROLSYM_ONE_HOT_DISABLE_SHFT                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_ADDR                                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000001c)
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_RMSK                                                                                         0xf
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_ADDR, HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_IN)
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_SOFT_RESET_BMSK                                                                              0x8
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_SOFT_RESET_SHFT                                                                              0x3
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_RUN_IDAC_CAL_BMSK                                                                            0x4
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_RUN_IDAC_CAL_SHFT                                                                            0x2
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_PLL_SERIES_UPDATE_BMSK                                                                       0x2
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_PLL_SERIES_UPDATE_SHFT                                                                       0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_PLL_SERIES_BMSK                                                                              0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_CTRL1_PLL_SERIES_SHFT                                                                              0x0

#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000020)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_RMSK                                                                                  0xf
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_ADDR, HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_FORCE_RX_CFGCHANGE_SEQ_BMSK                                                           0x8
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_FORCE_RX_CFGCHANGE_SEQ_SHFT                                                           0x3
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_FORCE_TX_CFGCHANGE_SEQ_BMSK                                                           0x4
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_FORCE_TX_CFGCHANGE_SEQ_SHFT                                                           0x2
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_RSYNC_EN_BMSK                                                                         0x2
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_RSYNC_EN_SHFT                                                                         0x1
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_TSYNC_EN_BMSK                                                                         0x1
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_RSYNC_CNTL_TSYNC_EN_SHFT                                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000024)
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_RMSK                                                                                  0x7
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_ADDR, HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_IN)
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_PLACEHOLDER_CHAIN_CLKEN_BMSK                                                          0x4
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_PLACEHOLDER_CHAIN_CLKEN_SHFT                                                          0x2
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_PLACEHOLDER_CHAIN_INPUT_BMSK                                                          0x2
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_PLACEHOLDER_CHAIN_INPUT_SHFT                                                          0x1
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_RETIME_BUFFER_EN_BMSK                                                                 0x1
#define HWIO_UFS_MEM_UFS_PHY_RETIME_BUFFER_EN_RETIME_BUFFER_EN_SHFT                                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_ADDR                                                                                   (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000028)
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_RMSK                                                                                          0x7
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_ADDR, HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_IN)
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_STALL_STATE_RX_LANE_EN_BMSK                                                                   0x4
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_STALL_STATE_RX_LANE_EN_SHFT                                                                   0x2
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_TX_CFGRDYN_WAIT_ON_PLL_BMSK                                                                   0x2
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_TX_CFGRDYN_WAIT_ON_PLL_SHFT                                                                   0x1
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_PLL_SHUTOFF_EN_BMSK                                                                           0x1
#define HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_PLL_SHUTOFF_EN_SHFT                                                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000002c)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_RMSK                                                                             0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_TX_LARGE_DRV_LVL_BMSK                                                            0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_TX_LARGE_DRV_LVL_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_ADDR                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000030)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_RMSK                                                                        0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_TX_LARGE_EMP_POST1_LVL_BMSK                                                 0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_POST_EMP_LVL_TX_LARGE_EMP_POST1_LVL_SHFT                                                  0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000034)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_RMSK                                                                             0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_TX_SMALL_DRV_LVL_BMSK                                                            0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_TX_SMALL_DRV_LVL_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_ADDR                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000038)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_RMSK                                                                        0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_TX_SMALL_EMP_POST1_LVL_BMSK                                                 0x1f
#define HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_POST_EMP_LVL_TX_SMALL_EMP_POST1_LVL_SHFT                                                  0x0

#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000003c)
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_ADDR, HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_IN)
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_CFGCHANGE_CNT_VAL_BMSK                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_CFG_CHANGE_CNT_VAL_CFGCHANGE_CNT_VAL_SHFT                                                              0x0

#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000040)
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_ADDR, HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_IN)
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_OMC_STATUS_RDVAL_BMSK                                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_OMC_STATUS_RDVAL_OMC_STATUS_RDVAL_SHFT                                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_ADDR                                                                          (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000044)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_RMSK                                                                                0x3f
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_RX_SYNC_WAIT_TIME_BMSK                                                              0x3f
#define HWIO_UFS_MEM_UFS_PHY_RX_SYNC_WAIT_TIME_RX_SYNC_WAIT_TIME_SHFT                                                               0x0

#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000048)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_PWM_MODE_BMSK                                                                    0x80
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_PWM_MODE_SHFT                                                                     0x7
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_CHK_ERR_CNT_CLR_BMSK                                                             0x40
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_CHK_ERR_CNT_CLR_SHFT                                                              0x6
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_RX_ERR_EN_BMSK                                                                   0x20
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_RX_ERR_EN_SHFT                                                                    0x5
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_GEN_SHORT_PATTERN_BMSK                                                           0x10
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_GEN_SHORT_PATTERN_SHFT                                                            0x4
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_PAT_SEL_BMSK                                                                      0x8
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_PAT_SEL_SHFT                                                                      0x3
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_TXRX_EN_BMSK                                                                      0x4
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_TXRX_EN_SHFT                                                                      0x2
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_TX_EN_BMSK                                                                        0x2
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_TX_EN_SHFT                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_MODE_BMSK                                                                         0x1
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CTRL_L0_BIST_MODE_SHFT                                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000004c)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_PRBS_POLY0_BMSK                                                                       0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY0_PRBS_POLY0_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000050)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_PRBS_POLY1_BMSK                                                                       0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_POLY1_PRBS_POLY1_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000054)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_PRBS_SEED0_BMSK                                                                       0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED0_PRBS_SEED0_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000058)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_PRBS_SEED1_BMSK                                                                       0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_PRBS_SEED1_PRBS_SEED1_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_ADDR                                                                        (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000005c)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_RMSK                                                                               0xf
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT0_K_BMSK                                                                  0x8
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT0_K_SHFT                                                                  0x3
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT1_K_BMSK                                                                  0x4
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT1_K_SHFT                                                                  0x2
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT2_K_BMSK                                                                  0x2
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT2_K_SHFT                                                                  0x1
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT3_K_BMSK                                                                  0x1
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_FIXED_PAT3_K_SHFT                                                                  0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000060)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_RMSK                                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_FIXED_PAT0_BMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT0_DATA_FIXED_PAT0_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000064)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_RMSK                                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_FIXED_PAT1_BMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT1_DATA_FIXED_PAT1_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000068)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_RMSK                                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_FIXED_PAT2_BMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT2_DATA_FIXED_PAT2_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000006c)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_RMSK                                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_ADDR, HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_IN)
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_FIXED_PAT3_BMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT3_DATA_FIXED_PAT3_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000070)
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_RMSK                                                                              0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_TX_HSGEAR_CAPABILITY_BMSK                                                         0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_TX_HSGEAR_CAPABILITY_SHFT                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_ADDR                                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000074)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_RMSK                                                                             0x7
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_TX_PWMGEAR_CAPABILITY_BMSK                                                       0x7
#define HWIO_UFS_MEM_UFS_PHY_TX_PWMGEAR_CAPABILITY_TX_PWMGEAR_CAPABILITY_SHFT                                                       0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_ADDR                                                                    (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000078)
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_RMSK                                                                           0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_TX_AMPLITUDE_CAPABILITY_BMSK                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_AMPLITUDE_CAPABILITY_TX_AMPLITUDE_CAPABILITY_SHFT                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_ADDR                                                                 (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000007c)
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_RMSK                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_TX_EXTERNALSYNC_CAPABILITY_BMSK                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_EXTERNALSYNC_CAPABILITY_TX_EXTERNALSYNC_CAPABILITY_SHFT                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_ADDR                                                   (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000080)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_RMSK                                                          0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_BMSK                 0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_TX_HS_UNTERMINATED_LINE_DRIVE_CAPABILITY_SHFT                 0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_ADDR                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000084)
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_RMSK                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_BMSK                     0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_TX_LS_TERMINATED_LINE_DRIVE_CAPABILITY_SHFT                     0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000088)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RMSK                                                             0xf
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_BMSK                       0xf
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_TX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_SHFT                       0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000008c)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_BMSK                      0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_TX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_SHFT                       0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000090)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RMSK                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_BMSK                            0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_TX_MIN_SAVE_CONFIG_TIME_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_ADDR                                                             (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000094)
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_RMSK                                                                    0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_TX_REF_CLOCK_SHARED_CAPABILITY_BMSK                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_REF_CLOCK_SHARED_CAPABILITY_TX_REF_CLOCK_SHARED_CAPABILITY_SHFT                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000098)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RMSK                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_TX_PHY_MAJOR_RELEASE_CAPABILITY_BMSK                             0xf0
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_TX_PHY_MAJOR_RELEASE_CAPABILITY_SHFT                              0x4
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_TX_PHY_MINOR_RELEASE_CAPABILITY_BMSK                              0xf
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_MAJORMINOR_RELEASE_CAPABILITY_TX_PHY_MINOR_RELEASE_CAPABILITY_SHFT                              0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR                                                        (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000009c)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_RMSK                                                              0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_BMSK                          0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_TX_PHY_EDITORIAL_RELEASE_CAPABILITY_SHFT                           0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_ADDR                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000a0)
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_RMSK                                                                        0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_TX_HIBERN8TIME_CAPABILITY_BMSK                                              0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_HIBERN8TIME_CAPABILITY_TX_HIBERN8TIME_CAPABILITY_SHFT                                               0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000a4)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_RMSK                                                                0x7
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_TX_ADVANCED_GRANULARITY_STEP_CAPABILITY_BMSK                        0x6
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_TX_ADVANCED_GRANULARITY_STEP_CAPABILITY_SHFT                        0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_TX_ADVANCED_GRANULARITY_CAPABILITY_BMSK                             0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_GRANULARITY_CAPABILITY_TX_ADVANCED_GRANULARITY_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000a8)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_RMSK                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_BMSK                            0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_TX_ADVANCED_HIBERN8TIME_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000ac)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_RMSK                                                                0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_TX_HS_EQUALIZER_SETTING_CAPABILITY_BMSK                             0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_HS_EQUALIZER_SETTING_CAPABILITY_TX_HS_EQUALIZER_SETTING_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000b0)
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_RMSK                                                                              0x3
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_RX_HSGEAR_CAPABILITY_BMSK                                                         0x3
#define HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_RX_HSGEAR_CAPABILITY_SHFT                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_ADDR                                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000b4)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_RMSK                                                                             0x7
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_RX_PWMGEAR_CAPABILITY_BMSK                                                       0x7
#define HWIO_UFS_MEM_UFS_PHY_RX_PWMGEAR_CAPABILITY_RX_PWMGEAR_CAPABILITY_SHFT                                                       0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_ADDR                                                              (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000b8)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_RMSK                                                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_RX_HS_UNTERMINATED_CAPABILITY_BMSK                                       0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_UNTERMINATED_CAPABILITY_RX_HS_UNTERMINATED_CAPABILITY_SHFT                                       0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_ADDR                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000bc)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_RMSK                                                                       0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_RX_LS_TERMINATED_CAPABILITY_BMSK                                           0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_TERMINATED_CAPABILITY_RX_LS_TERMINATED_CAPABILITY_SHFT                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000c0)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RMSK                                                             0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_BMSK                       0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_RX_MIN_SLEEP_NOCONFIG_TIME_CAPABILITY_SHFT                       0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000c4)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_BMSK                      0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_RX_MIN_STALL_NOCONFIG_TIME_CAPABILITY_SHFT                       0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000c8)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RMSK                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_BMSK                            0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_RX_MIN_SAVE_CONFIG_TIME_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_ADDR                                                             (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000cc)
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_RMSK                                                                    0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_RX_REF_CLOCK_SHARED_CAPABILITY_BMSK                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_REF_CLOCK_SHARED_CAPABILITY_RX_REF_CLOCK_SHARED_CAPABILITY_SHFT                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_ADDR                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000d0)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RX_HS_G1_SYNC_RANGE_CAPABILITY_BMSK                                   0xc0
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RX_HS_G1_SYNC_RANGE_CAPABILITY_SHFT                                    0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_BMSK                                  0x3f
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_RX_HS_G1_SYNC_LENGTH_CAPABILITY_SHFT                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000d4)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_RMSK                                                                0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_BMSK                             0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_RX_HS_G1_PREPARE_LENGTH_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_ADDR                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000d8)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_RMSK                                                                   0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_RX_LS_PREPARE_LENGTH_CAPABILITY_BMSK                                   0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_LS_PREPARE_LENGTH_CAPABILITY_RX_LS_PREPARE_LENGTH_CAPABILITY_SHFT                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_ADDR                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000dc)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_RMSK                                                           0x1f
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_BMSK                    0x1f
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_RX_PWM_BURST_CLOSURE_LENGTH_CAPABILITY_SHFT                     0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_ADDR                                                             (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000e0)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_RMSK                                                                    0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_RX_MIN_ACTIVATETIME_CAPABILITY_BMSK                                     0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_ACTIVATETIME_CAPABILITY_RX_MIN_ACTIVATETIME_CAPABILITY_SHFT                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000e4)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RMSK                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RX_PHY_MAJOR_RELEASE_CAPABILITY_BMSK                             0xf0
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RX_PHY_MAJOR_RELEASE_CAPABILITY_SHFT                              0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RX_PHY_MINOR_RELEASE_CAPABILITY_BMSK                              0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_MAJORMINOR_RELEASE_CAPABILITY_RX_PHY_MINOR_RELEASE_CAPABILITY_SHFT                              0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR                                                        (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000e8)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_RMSK                                                              0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_BMSK                          0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_RX_PHY_EDITORIAL_RELEASE_CAPABILITY_SHFT                           0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_ADDR                                                                  (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000ec)
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_RMSK                                                                        0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_RX_HIBERN8TIME_CAPABILITY_BMSK                                              0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_HIBERN8TIME_CAPABILITY_RX_HIBERN8TIME_CAPABILITY_SHFT                                               0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_ADDR                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000f0)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RX_HS_G2_SYNC_RANGE_CAPABILITY_BMSK                                   0xc0
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RX_HS_G2_SYNC_RANGE_CAPABILITY_SHFT                                    0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_BMSK                                  0x3f
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_RX_HS_G2_SYNC_LENGTH_CAPABILITY_SHFT                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_ADDR                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000f4)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RMSK                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RX_HS_G3_SYNC_RANGE_CAPABILITY_BMSK                                   0xc0
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RX_HS_G3_SYNC_RANGE_CAPABILITY_SHFT                                    0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_BMSK                                  0x3f
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_RX_HS_G3_SYNC_LENGTH_CAPABILITY_SHFT                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000f8)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_RMSK                                                                0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_BMSK                             0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_RX_HS_G2_PREPARE_LENGTH_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000000fc)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_RMSK                                                                0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_BMSK                             0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_RX_HS_G3_PREPARE_LENGTH_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000100)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RMSK                                                                0x7
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RX_ADVANCED_GRANULARITY_STEP_CAPABILITY_BMSK                        0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RX_ADVANCED_GRANULARITY_STEP_CAPABILITY_SHFT                        0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RX_ADVANCED_GRANULARITY_CAPABILITY_BMSK                             0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_GRANULARITY_CAPABILITY_RX_ADVANCED_GRANULARITY_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000104)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_RMSK                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_BMSK                            0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_RX_ADVANCED_HIBERN8TIME_CAPABILITY_SHFT                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_ADDR                                                    (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000108)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_RMSK                                                           0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_BMSK                   0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_RX_ADVANCED_MIN_ACTIVATETIME_CAPABILITY_SHFT                   0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000010c)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX3_BMSK                                                                  0x80
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX3_SHFT                                                                   0x7
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX2_BMSK                                                                  0x40
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX2_SHFT                                                                   0x6
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX1_BMSK                                                                  0x20
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX1_SHFT                                                                   0x5
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX0_BMSK                                                                  0x10
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_DEBUG_CLK_IDX0_SHFT                                                                   0x4
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS3_BMSK                                                               0x8
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS3_SHFT                                                               0x3
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS2_BMSK                                                               0x4
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS2_SHFT                                                               0x2
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS1_BMSK                                                               0x2
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS1_SHFT                                                               0x1
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS0_BMSK                                                               0x1
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_USE_CLK_DEBUG_BUS0_SHFT                                                               0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000110)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_RMSK                                                                                 0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_DEBUG_BUS_IDX0_BMSK                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_CTRL_DEBUG_BUS_IDX0_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000114)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_RMSK                                                                                 0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_DEBUG_BUS_IDX1_BMSK                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_CTRL_DEBUG_BUS_IDX1_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000118)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_RMSK                                                                                 0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_DEBUG_BUS_IDX2_BMSK                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_CTRL_DEBUG_BUS_IDX2_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000011c)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_RMSK                                                                                 0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_DEBUG_BUS_IDX3_BMSK                                                                  0x7f
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_CTRL_DEBUG_BUS_IDX3_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_ADDR                                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000120)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_RMSK                                                                           0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_DEBUG_BUS_0_STATUS_CHK_BMSK                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_CHK_DEBUG_BUS_0_STATUS_CHK_SHFT                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_ADDR                                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000124)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_RMSK                                                                           0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_DEBUG_BUS_1_STATUS_CHK_BMSK                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_CHK_DEBUG_BUS_1_STATUS_CHK_SHFT                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_ADDR                                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000128)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_RMSK                                                                           0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_DEBUG_BUS_2_STATUS_CHK_BMSK                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_CHK_DEBUG_BUS_2_STATUS_CHK_SHFT                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_ADDR                                                                     (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000012c)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_RMSK                                                                           0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_IN)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_DEBUG_BUS_3_STATUS_CHK_BMSK                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_CHK_DEBUG_BUS_3_STATUS_CHK_SHFT                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000130)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_RMSK                                                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_RX_LINECFG_DISABLE_BMSK                                                                0x2
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_RX_LINECFG_DISABLE_SHFT                                                                0x1
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_TX_LINECFG_DISABLE_BMSK                                                                0x1
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_TX_LINECFG_DISABLE_SHFT                                                                0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000134)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_RMSK                                                                               0x1f
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_KLOCK_ENABLE_MUX_BMSK                                                              0x10
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_KLOCK_ENABLE_MUX_SHFT                                                               0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_KLOCK_ENABLE_BMSK                                                                   0x8
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_KLOCK_ENABLE_SHFT                                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_FILLER_RESYNC_BMSK                                                                  0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_FILLER_RESYNC_SHFT                                                                  0x2
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_RESET_DISPARITY_BMSK                                                                0x2
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_RESET_DISPARITY_SHFT                                                                0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_FLAG_SYMBOLERR_BMSK                                                                 0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_SYM_RESYNC_CTRL_FLAG_SYMBOLERR_SHFT                                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_ADDR                                                                        (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000138)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_RMSK                                                                              0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_MIN_HIBERN8_TIME_BMSK                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_MIN_HIBERN8_TIME_SHFT                                                              0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000013c)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RMSK                                                                                  0xcf
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_DC_COUPLE_HS_BURST_BMSK                                                            0x80
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_DC_COUPLE_HS_BURST_SHFT                                                             0x7
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_HIGHZ_COUNT_RANGE_BMSK                                                             0x40
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_HIGHZ_COUNT_RANGE_SHFT                                                              0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_L1_SIGDET_EN_BMSK                                                                   0x8
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_L1_SIGDET_EN_SHFT                                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_SIGDET_EN_MODE_BMSK                                                                 0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_SIGDET_EN_MODE_SHFT                                                                 0x2
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_L0_SIGDET_EN_BMSK                                                                   0x2
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_L0_SIGDET_EN_SHFT                                                                   0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_SIGDET_USE_LP_EN_BMSK                                                               0x1
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL1_RX_SIGDET_USE_LP_EN_SHFT                                                               0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000140)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RX_SIGDET_ACT_LVL_BMSK                                                                0xf0
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RX_SIGDET_ACT_LVL_SHFT                                                                 0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RX_SIGDET_SQX_LVL_BMSK                                                                 0xf
#define HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_RX_SIGDET_SQX_LVL_SHFT                                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_ADDR                                                                      (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000144)
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_RMSK                                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_ADDR, HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_TCLK_SYM_CNTR_WAIT_FOR_RX_BMSK                                                  0x80
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_TCLK_SYM_CNTR_WAIT_FOR_RX_SHFT                                                   0x7
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_TCLK_SYM_CNTR_INITVAL_BMSK                                                      0x7f
#define HWIO_UFS_MEM_UFS_PHY_TCLK_SYM_CNTR_INITVAL_TCLK_SYM_CNTR_INITVAL_SHFT                                                       0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000148)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G4_BAND_BMSK                                                                  0xc0
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G4_BAND_SHFT                                                                   0x6
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G3_BAND_BMSK                                                                  0x30
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G3_BAND_SHFT                                                                   0x4
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G2_BAND_BMSK                                                                   0xc
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G2_BAND_SHFT                                                                   0x2
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G1_BAND_BMSK                                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_TX_PWM_G1_BAND_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000014c)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_ADDR, HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_IN)
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G4_BAND_BMSK                                                                  0xc0
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G4_BAND_SHFT                                                                   0x6
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G3_BAND_BMSK                                                                  0x30
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G3_BAND_SHFT                                                                   0x4
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G2_BAND_BMSK                                                                   0xc
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G2_BAND_SHFT                                                                   0x2
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G1_BAND_BMSK                                                                   0x3
#define HWIO_UFS_MEM_UFS_PHY_RX_PWM_GEAR_BAND_RX_PWM_G1_BAND_SHFT                                                                   0x0

#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_ADDR                                                                             (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000150)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMSK                                                                                   0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_RX_CFGUPDT_L1_BMSK                                                                0x80
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_RX_CFGUPDT_L1_SHFT                                                                 0x7
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_TX_CFGUPDT_L1_BMSK                                                                0x40
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_TX_CFGUPDT_L1_SHFT                                                                 0x6
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGWR_L1_BMSK                                                                     0x20
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGWR_L1_SHFT                                                                      0x5
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGRD_L1_BMSK                                                                     0x10
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGRD_L1_SHFT                                                                      0x4
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_RX_CFGUPDT_L0_BMSK                                                                 0x8
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_RX_CFGUPDT_L0_SHFT                                                                 0x3
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_TX_CFGUPDT_L0_BMSK                                                                 0x4
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_TX_CFGUPDT_L0_SHFT                                                                 0x2
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGWR_L0_BMSK                                                                      0x2
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGWR_L0_SHFT                                                                      0x1
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGRD_L0_BMSK                                                                      0x1
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTR_CTRL_RMMI_CFGRD_L0_SHFT                                                                      0x0

#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_ADDR                                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000154)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_RMSK                                                                                      0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_ADDR, HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_IN)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_RMMI_ATTRID_BMSK                                                                          0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRID_RMMI_ATTRID_SHFT                                                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_ADDR                                                                             (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000158)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_RMSK                                                                                   0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_ADDR, HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_IN)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_RMMI_ATTRWRVAL_BMSK                                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRWRVAL_RMMI_ATTRWRVAL_SHFT                                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_ADDR                                                                   (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000015c)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_RMSK                                                                         0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_RMMI_ATTRRDVAL_L0_STATUS_BMSK                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L0_STATUS_RMMI_ATTRRDVAL_L0_STATUS_SHFT                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000160)
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RSVD_STATUS_BMSK                                                                     0x80
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RSVD_STATUS_SHFT                                                                      0x7
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_REFGEN_REQ_BMSK                                                                      0x40
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_REFGEN_REQ_SHFT                                                                       0x6
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RUN_IDAC_CAL_DONE_BMSK                                                               0x20
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_RUN_IDAC_CAL_DONE_SHFT                                                                0x5
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PLL_SERIES_UPDATE_DONE_BMSK                                                          0x10
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PLL_SERIES_UPDATE_DONE_SHFT                                                           0x4
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_CMN_PSMCLK_SWITCH_DONE_BMSK                                                           0x8
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_CMN_PSMCLK_SWITCH_DONE_SHFT                                                           0x3
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_POWER_STATE_IDLE_BMSK                                                                 0x4
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_POWER_STATE_IDLE_SHFT                                                                 0x2
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PLL_ACTIVATE_DONE_BMSK                                                                0x2
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PLL_ACTIVATE_DONE_SHFT                                                                0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PCS_READY_BMSK                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_READY_STATUS_PCS_READY_SHFT                                                                        0x0

#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_ADDR                                                                            (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000164)
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_RMSK                                                                                  0xff
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_RSVD_STATUS_BMSK                                                                      0xfe
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_RSVD_STATUS_SHFT                                                                       0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_PLACEHOLDER_CHAIN_OUTPUT_BMSK                                                          0x1
#define HWIO_UFS_MEM_UFS_PHY_PCS_MISC_STATUS_PLACEHOLDER_CHAIN_OUTPUT_SHFT                                                          0x0

#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000168)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_L0_BIST_CHK_ERR_CNT_L_BMSK                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_L_STATUS_L0_BIST_CHK_ERR_CNT_L_SHFT                                                0x0

#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000016c)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_L0_BIST_CHK_ERR_CNT_H_BMSK                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_ERR_CNT_H_STATUS_L0_BIST_CHK_ERR_CNT_H_SHFT                                                0x0

#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000170)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_RSVD_STATUS_BMSK                                                                   0xf0
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_RSVD_STATUS_SHFT                                                                    0x4
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_CHK_HEADER_SEL_BMSK                                                         0x8
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_CHK_HEADER_SEL_SHFT                                                         0x3
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_HEADER_NOT_FOUND_BMSK                                                       0x4
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_HEADER_NOT_FOUND_SHFT                                                       0x2
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_PAT_DATA_NOT_FOUND_BMSK                                                     0x2
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_PAT_DATA_NOT_FOUND_SHFT                                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_CHECK_DONE_BMSK                                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_L0_BIST_CHK_STATUS_L0_BIST_CHECK_DONE_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000174)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_BMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000178)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_BMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000017c)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_BMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000180)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_BMSK                                                            0xff
#define HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000184)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_ADDR, HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_REVISION_ID_7_0_BMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID0_REVISION_ID_7_0_SHFT                                                                      0x0

#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000188)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_ADDR, HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_REVISION_ID_15_8_BMSK                                                                    0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID1_REVISION_ID_15_8_SHFT                                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000018c)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_ADDR, HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_REVISION_ID_23_16_BMSK                                                                   0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID2_REVISION_ID_23_16_SHFT                                                                    0x0

#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000190)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_ADDR, HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_REVISION_ID_31_24_BMSK                                                                   0xff
#define HWIO_UFS_MEM_UFS_PHY_REVISION_ID3_REVISION_ID_31_24_SHFT                                                                    0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_ADDR                                                                   (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000194)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_RMSK                                                                          0xf
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G4_BMSK                                             0x8
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G4_SHFT                                             0x3
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G3_BMSK                                             0x4
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G3_SHFT                                             0x2
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G2_BMSK                                             0x2
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G2_SHFT                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G1_BMSK                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_CTRL_LINECFG_HIGHZ_COUNT_RANGE_G1_SHFT                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_ADDR                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x00000198)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_RMSK                                                                      0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_LINECFG_HIGHZ_COUNT_INIT_G1_BMSK                                          0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G1_LINECFG_HIGHZ_COUNT_INIT_G1_SHFT                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_ADDR                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x0000019c)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_RMSK                                                                      0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_LINECFG_HIGHZ_COUNT_INIT_G2_BMSK                                          0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G2_LINECFG_HIGHZ_COUNT_INIT_G2_SHFT                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_ADDR                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001a0)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_RMSK                                                                      0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_LINECFG_HIGHZ_COUNT_INIT_G3_BMSK                                          0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G3_LINECFG_HIGHZ_COUNT_INIT_G3_SHFT                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_ADDR                                                                (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001a4)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_RMSK                                                                      0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_ADDR, HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_IN)
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_LINECFG_HIGHZ_COUNT_INIT_G4_BMSK                                          0xff
#define HWIO_UFS_MEM_UFS_PHY_LINECFG_HIGHZ_COUNT_INIT_G4_LINECFG_HIGHZ_COUNT_INIT_G4_SHFT                                           0x0

#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001a8)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_RMSK                                                                                 0xff
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_TSYNC_COUNT_INIT_BMSK                                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_TSYNC_COUNT_CTRL_TSYNC_COUNT_INIT_SHFT                                                                 0x0

#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_ADDR                                                                       (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001ac)
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_RMSK                                                                             0xff
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_SYSCLK_EN_COUNT_INIT_BMSK                                                        0xff
#define HWIO_UFS_MEM_UFS_PHY_SYSCLK_EN_COUNT_CTRL_SYSCLK_EN_COUNT_INIT_SHFT                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_ADDR                                                                          (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001b0)
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_RMSK                                                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_PLL_SHUTDOWN_WAIT_INIT_BMSK                                                         0xff
#define HWIO_UFS_MEM_UFS_PHY_PLL_SHUTDOWN_CTRL_PLL_SHUTDOWN_WAIT_INIT_SHFT                                                          0x0

#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001b4)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_RMSK                                                                     0x3f
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_ADDR, HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_IN)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_TIMER_20US_LANECLK_STEPS_MSB_BMSK                                        0x3f
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_MSB_TIMER_20US_LANECLK_STEPS_MSB_SHFT                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001b8)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_ADDR, HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_IN)
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_TIMER_20US_LANECLK_STEPS_LSB_BMSK                                        0xff
#define HWIO_UFS_MEM_UFS_PHY_TIMER_20US_LANECLK_STEPS_LSB_TIMER_20US_LANECLK_STEPS_LSB_SHFT                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_ADDR                                                                          (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001bc)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_RMSK                                                                                0xf3
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_SLEEP_COUNT_INIT_BMSK                                                   0xf0
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_SLEEP_COUNT_INIT_SHFT                                                    0x4
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_SLEEP_EN_BMSK                                                            0x2
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_SLEEP_EN_SHFT                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_STALL_EN_BMSK                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_TX_MID_TERM_STALL_EN_SHFT                                                            0x0

#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_ADDR                                                                          (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001c0)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_RMSK                                                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_ADDR, HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_IN)
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_TX_MID_TERM_COUNT_INIT_BMSK                                                         0xff
#define HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL2_TX_MID_TERM_COUNT_INIT_SHFT                                                          0x0

#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_ADDR                                                                           (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001c4)
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_RMSK                                                                                  0xf
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_ADDR, HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_IN)
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_LANE_WRITE_BROADCAST_BMSK                                                             0x8
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_LANE_WRITE_BROADCAST_SHFT                                                             0x3
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_SECONDARY_LANE_MODE_GEAR_SRC_BMSK                                                     0x4
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_SECONDARY_LANE_MODE_GEAR_SRC_SHFT                                                     0x2
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_SECONDARY_LANE_ENABLE_BMSK                                                            0x2
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_SECONDARY_LANE_ENABLE_SHFT                                                            0x1
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_PRIMARY_LANE_BMSK                                                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_PRIMARY_LANE_SHFT                                                                     0x0

#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_ADDR                                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001c8)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_RMSK                                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_ADDR, HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_ADDR,v)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_ADDR,m,v,HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_IN)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_PWM_MODE_BMSK                                                                    0x80
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_PWM_MODE_SHFT                                                                     0x7
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_CHK_ERR_CNT_CLR_BMSK                                                             0x40
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_CHK_ERR_CNT_CLR_SHFT                                                              0x6
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_RX_ERR_EN_BMSK                                                                   0x20
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_RX_ERR_EN_SHFT                                                                    0x5
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_GEN_SHORT_PATTERN_BMSK                                                           0x10
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_GEN_SHORT_PATTERN_SHFT                                                            0x4
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_PAT_SEL_BMSK                                                                      0x8
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_PAT_SEL_SHFT                                                                      0x3
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_TXRX_EN_BMSK                                                                      0x4
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_TXRX_EN_SHFT                                                                      0x2
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_TX_EN_BMSK                                                                        0x2
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_TX_EN_SHFT                                                                        0x1
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_MODE_BMSK                                                                         0x1
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CTRL_L1_BIST_MODE_SHFT                                                                         0x0

#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001cc)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_L1_BIST_CHK_ERR_CNT_L_BMSK                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_L_STATUS_L1_BIST_CHK_ERR_CNT_L_SHFT                                                0x0

#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_ADDR                                                               (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001d0)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_RMSK                                                                     0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_L1_BIST_CHK_ERR_CNT_H_BMSK                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_ERR_CNT_H_STATUS_L1_BIST_CHK_ERR_CNT_H_SHFT                                                0x0

#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_ADDR                                                                         (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001d4)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_RMSK                                                                               0xff
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_RSVD_STATUS_BMSK                                                                   0xf0
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_RSVD_STATUS_SHFT                                                                    0x4
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_CHK_HEADER_SEL_BMSK                                                         0x8
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_CHK_HEADER_SEL_SHFT                                                         0x3
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_HEADER_NOT_FOUND_BMSK                                                       0x4
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_HEADER_NOT_FOUND_SHFT                                                       0x2
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_PAT_DATA_NOT_FOUND_BMSK                                                     0x2
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_PAT_DATA_NOT_FOUND_SHFT                                                     0x1
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_CHECK_DONE_BMSK                                                             0x1
#define HWIO_UFS_MEM_UFS_PHY_L1_BIST_CHK_STATUS_L1_BIST_CHECK_DONE_SHFT                                                             0x0

#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_ADDR                                                                   (UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS_REG_BASE      + 0x000001d8)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_RMSK                                                                         0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_ADDR, HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_RMSK)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_ADDR, m)
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_RMMI_ATTRRDVAL_L1_STATUS_BMSK                                                0xff
#define HWIO_UFS_MEM_UFS_PHY_RMMI_ATTRRDVAL_L1_STATUS_RMMI_ATTRRDVAL_L1_STATUS_SHFT                                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: UFS_MEM_MPHY_UFS
 *--------------------------------------------------------------------------*/

#define UFS_MEM_MPHY_UFS_REG_BASE (UFS_MEM_UFS_1LANE_REGS_BASE      + 0x00007000)

/*----------------------------------------------------------------------------
 * MODULE: UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL
 *--------------------------------------------------------------------------*/

#define UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE                                    (UFS_MEM_UFS_1LANE_REGS_BASE      + 0x00007000)

#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000000)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ADDR, HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ATB_SEL_7_0_BMSK                                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL1_ATB_SEL_7_0_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000004)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_RMSK                                                         0x7
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ADDR, HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ATB_SEL_10_8_BMSK                                            0x7
#define HWIO_UFS_MEM_QSERDES_COM_ATB_SEL2_ATB_SEL_10_8_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_ADDR                                               (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000008)
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_RMSK                                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_ADDR, HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_IN)
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_PLL_FREQ_UPDATE_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_FREQ_UPDATE_PLL_FREQ_UPDATE_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000000c)
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_ADDR, HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_IN)
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_SYSCLK_WAKE_BYPASS_BMSK                                     0x80
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_SYSCLK_WAKE_BYPASS_SHFT                                      0x7
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_SYSCLK_WAKE_TIME_BMSK                                       0x70
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_SYSCLK_WAKE_TIME_SHFT                                        0x4
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_REFCLK_FREQ_SEL_BMSK                                         0x8
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_REFCLK_FREQ_SEL_SHFT                                         0x3
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_RSVD_BMSK                                                    0x4
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_RSVD_SHFT                                                    0x2
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_BIAS_WAIT_TIME_BMSK                                          0x3
#define HWIO_UFS_MEM_QSERDES_COM_BG_TIMER_BIAS_WAIT_TIME_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000010)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_RMSK                                                    0x3
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_SSC_CENTER_BMSK                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_SSC_CENTER_SHFT                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_SSC_EN_BMSK                                             0x1
#define HWIO_UFS_MEM_QSERDES_COM_SSC_EN_CENTER_SSC_EN_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000014)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_SSC_ADJPER_7_0_BMSK                                     0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER1_SSC_ADJPER_7_0_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000018)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_RMSK                                                     0x3
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_SSC_ADJPER_9_8_BMSK                                      0x3
#define HWIO_UFS_MEM_QSERDES_COM_SSC_ADJ_PER2_SSC_ADJPER_9_8_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000001c)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_SSC_PER_7_0_BMSK                                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER1_SSC_PER_7_0_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000020)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_SSC_PER_15_8_BMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_PER2_SSC_PER_15_8_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_ADDR                                            (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000024)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_SSC_STEPSIZE_7_0_BMSK                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE1_SSC_STEPSIZE_7_0_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_ADDR                                            (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000028)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_ADDR, HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_SSC_STEPSIZE_15_8_BMSK                                0xff
#define HWIO_UFS_MEM_QSERDES_COM_SSC_STEP_SIZE2_SSC_STEPSIZE_15_8_SHFT                                 0x0

#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000002c)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_ADDR, HWIO_UFS_MEM_QSERDES_COM_POST_DIV_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_POST_DIV_IN)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_PLL_POSTDIV_7_0_BMSK                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_PLL_POSTDIV_7_0_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000030)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_RMSK                                                    0x11
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_ADDR, HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_IN)
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_MUX_BMSK                                    0x10
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_MUX_SHFT                                     0x4
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_8_8_BMSK                                     0x1
#define HWIO_UFS_MEM_QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_8_8_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000034)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_RMSK                                             0x7f
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR, HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_IN)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_R_BMSK                           0x40
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_R_SHFT                            0x6
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_L_BMSK                           0x20
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_L_SHFT                            0x5
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_EN_SYSCLK_TX_SEL_BMSK                            0x10
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_EN_SYSCLK_TX_SEL_SHFT                             0x4
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_L_EN_BMSK                                  0x8
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_L_EN_SHFT                                  0x3
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_R_EN_BMSK                                  0x4
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_R_EN_SHFT                                  0x2
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_MUX_BMSK                                  0x2
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_MUX_SHFT                                  0x1
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_ADDR                                               (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000038)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_CLKDRVIDLE_EN_BMSK                                       0x80
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_CLKDRVIDLE_EN_SHFT                                        0x7
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS3PS4_BMSK                                 0x40
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS3PS4_SHFT                                  0x6
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS0PS1PS2_BMSK                              0x20
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS0PS1PS2_SHFT                               0x5
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_CLK_EP_EN_BMSK                                           0x10
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_CLK_EP_EN_SHFT                                            0x4
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_MUX_BMSK                                     0x8
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_MUX_SHFT                                     0x3
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_BMSK                                         0x4
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_SHFT                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_MUX_BMSK                                     0x2
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_MUX_SHFT                                     0x1
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000003c)
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_RMSK                                                     0xf
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_BMSK                                     0x8
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_SHFT                                     0x3
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_CM_BMSK                                           0x4
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_CM_SHFT                                           0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_AC_COUPLE_BMSK                                    0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_AC_COUPLE_SHFT                                    0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_GNDTERM_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYS_CLK_CTRL_SYSCLK_GNDTERM_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000040)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_RMSK                                                0xf
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR, HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_CTRL_BY_PSM_BMSK                          0x8
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_CTRL_BY_PSM_SHFT                          0x3
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS3PS4_BMSK                               0x4
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS3PS4_SHFT                               0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_BUF_EN_MUX_BMSK                              0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_BUF_EN_MUX_SHFT                              0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SW_SYSCLK_BUF_EN_BMSK                               0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_BUF_ENABLE_SW_SYSCLK_BUF_EN_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_ADDR                                                    (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000044)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_RMSK                                                           0x3
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_EN_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_EN_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_PLL_EN_MUX_BMSK                                                0x2
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_PLL_EN_MUX_SHFT                                                0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_PLL_EN_BMSK                                                    0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_EN_PLL_EN_SHFT                                                    0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000048)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_PLL_IVCO_BMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_PLL_IVCO_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000004c)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_RMSK                                                      0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_IETRIM_BMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IETRIM_IETRIM_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000050)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_RMSK                                                      0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_IPTRIM_BMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_IPTRIM_IPTRIM_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_ADDR                                      (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000054)
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_RMSK                                            0x3f
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_MUX_BMSK                   0x20
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_MUX_SHFT                    0x5
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_BMSK                       0x18
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_SHFT                        0x3
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_FORCE_BMSK                         0x4
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_FORCE_SHFT                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_MUX_BMSK                        0x2
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_MUX_SHFT                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_BMSK                            0x1
#define HWIO_UFS_MEM_QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_ADDR                                    (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000058)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_RMSK                                           0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_SYSCLK_CLKDET_COMP_BMSK                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_DET_COMP_STATUS_SYSCLK_CLKDET_COMP_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000005c)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_ADDR, HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_CLK_EP_DIV_BMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_CLK_EP_DIV_CLK_EP_DIV_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000060)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_RMSK                                                   0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_PLL_CPCTRL_MODE0_BMSK                                  0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_PLL_CPCTRL_MODE0_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000064)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_RMSK                                                   0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_PLL_CPCTRL_MODE1_BMSK                                  0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_PLL_CPCTRL_MODE1_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000068)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_RMSK                                                 0x1f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_PLL_RCTRL_MODE0_BMSK                                 0x1f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_PLL_RCTRL_MODE0_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000006c)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_RMSK                                                 0x1f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_PLL_RCTRL_MODE1_BMSK                                 0x1f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_PLL_RCTRL_MODE1_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000070)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_RMSK                                                 0x3f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_PLL_CCTRL_MODE0_BMSK                                 0x3f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_PLL_CCTRL_MODE0_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000074)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_RMSK                                                 0x3f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_PLL_CCTRL_MODE1_BMSK                                 0x3f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_PLL_CCTRL_MODE1_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_ADDR                                                 (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000078)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_RMSK                                                       0x7f
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_STABLE_TIME_BMSK                                       0x60
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_STABLE_TIME_SHFT                                        0x5
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_MASH_RESET_MUX_BMSK                                        0x10
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_MASH_RESET_MUX_SHFT                                         0x4
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_MASH_RESET_BMSK                                             0x8
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_MASH_RESET_SHFT                                             0x3
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_CPLIN_BMSK                                              0x4
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_CPLIN_SHFT                                              0x2
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_DIV_FFEN_BMSK                                           0x2
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_DIV_FFEN_SHFT                                           0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_DIV_ORD_BMSK                                            0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_CNTRL_PLL_DIV_ORD_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000007c)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_RMSK                                              0x1
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_ADDR, HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_IN)
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_BIAS_EN_CTRL_BY_PSM_BMSK                          0x1
#define HWIO_UFS_MEM_QSERDES_COM_BIAS_EN_CTRL_BY_PSM_BIAS_EN_CTRL_BY_PSM_SHFT                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000080)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_CML_SYSCLK_SEL_BMSK                                    0xe0
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_CML_SYSCLK_SEL_SHFT                                     0x5
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_TERM_SEL_BMSK                                   0x10
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_TERM_SEL_SHFT                                    0x4
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_SEL_BMSK                                         0xc
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_SEL_SHFT                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_MUX_BMSK                                      0x2
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_MUX_SHFT                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_BMSK                                          0x1
#define HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_ADDR                                            (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000084)
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_RMSK                                                   0x1
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_CML_SYSCLK_SEL_MUX_BMSK                                0x1
#define HWIO_UFS_MEM_QSERDES_COM_CML_SYSCLK_SEL_CML_SYSCLK_SEL_MUX_SHFT                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000088)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RMSK                                                   0x7f
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_FORCE_C_READY_BMSK                                     0x40
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_FORCE_C_READY_SHFT                                      0x6
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_TSYNC_SEL_BMSK                                         0x20
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_TSYNC_SEL_SHFT                                          0x5
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_DISABLE_CLKGATE_BMSK                                   0x10
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_DISABLE_CLKGATE_SHFT                                    0x4
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_PLL_UNLOCK_DISABLE_BMSK                                 0x8
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_PLL_UNLOCK_DISABLE_SHFT                                 0x3
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RSM_START_MUX_BMSK                                      0x4
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RSM_START_MUX_SHFT                                      0x2
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RSM_START_BMSK                                          0x2
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_RSM_START_SHFT                                          0x1
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_FORCE_PLLLOCK_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL_FORCE_PLLLOCK_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_ADDR                                            (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000008c)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_ADDR, HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_CORE_PLL_EN_MUX_BMSK                                  0x80
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_CORE_PLL_EN_MUX_SHFT                                   0x7
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_CORE_PLL_EN_BMSK                                      0x40
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_CORE_PLL_EN_SHFT                                       0x6
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_BIAS_BMSK                                  0x20
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_BIAS_SHFT                                   0x5
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_INTEGLOOP_CAL_BMSK                         0x10
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_INTEGLOOP_CAL_SHFT                          0x4
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_PULSE_WIDTH_BMSK                                 0xc
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_PULSE_WIDTH_SHFT                                 0x2
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_MUX_BMSK                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_MUX_SHFT                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_BMSK                                             0x1
#define HWIO_UFS_MEM_QSERDES_COM_RESETSM_CNTRL2_TSYNC_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_ADDR                                               (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000090)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_2_BMSK                                       0x80
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_2_SHFT                                        0x7
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_2_BMSK                                       0x40
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_2_SHFT                                        0x6
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_MUX_BMSK                                      0x20
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_MUX_SHFT                                       0x5
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_BMSK                                          0x10
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_SHFT                                           0x4
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_BMSK                                          0xc
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_SHFT                                          0x2
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_BMSK                                          0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000094)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_RMSK                                                     0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_PLLLOCK_STOP_COUNT_BMSK                                  0x2
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_PLLLOCK_STOP_COUNT_SHFT                                  0x1
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_BYP_PLLLOCK_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_CFG_BYP_PLLLOCK_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000098)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_PLLLOCK_CMP_MODE0_7_0_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_PLLLOCK_CMP_MODE0_7_0_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000009c)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_PLLLOCK_CMP_MODE0_15_8_BMSK                          0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_PLLLOCK_CMP_MODE0_15_8_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000a0)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_PLLLOCK_CMP_MODE0_17_16_BMSK                          0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE0_PLLLOCK_CMP_MODE0_17_16_SHFT                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000a4)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_PLLLOCK_CMP_MODE1_7_0_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_PLLLOCK_CMP_MODE1_7_0_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000a8)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_PLLLOCK_CMP_MODE1_15_8_BMSK                          0xff
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_PLLLOCK_CMP_MODE1_15_8_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000ac)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_PLLLOCK_CMP_MODE1_17_16_BMSK                          0x3
#define HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP3_MODE1_PLLLOCK_CMP_MODE1_17_16_SHFT                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000b0)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_DEC_START_MODE0_7_0_BMSK                             0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_DEC_START_MODE0_7_0_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000b4)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_DEC_START_MODE1_7_0_BMSK                             0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_DEC_START_MODE1_7_0_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000b8)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_DIV_FRAC_START_MODE0_7_0_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE0_DIV_FRAC_START_MODE0_7_0_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000bc)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_DIV_FRAC_START_MODE0_15_8_BMSK                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE0_DIV_FRAC_START_MODE0_15_8_SHFT                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000c0)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_RMSK                                            0xf
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_DIV_FRAC_START_MODE0_19_16_BMSK                 0xf
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE0_DIV_FRAC_START_MODE0_19_16_SHFT                 0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000c4)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_DIV_FRAC_START_MODE1_7_0_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START1_MODE1_DIV_FRAC_START_MODE1_7_0_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000c8)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_DIV_FRAC_START_MODE1_15_8_BMSK                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START2_MODE1_DIV_FRAC_START_MODE1_15_8_SHFT                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000cc)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_RMSK                                            0xf
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_DIV_FRAC_START_MODE1_19_16_BMSK                 0xf
#define HWIO_UFS_MEM_QSERDES_COM_DIV_FRAC_START3_MODE1_DIV_FRAC_START_MODE1_19_16_SHFT                 0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000d0)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_INTEGLOOP_INITVAL_BMSK                             0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_INITVAL_INTEGLOOP_INITVAL_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000d4)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_RMSK                                                     0x7
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_CODE_BYPASS_BMSK                               0x4
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_CODE_BYPASS_SHFT                               0x2
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_MUX_BMSK                                    0x2
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_MUX_SHFT                                    0x1
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_BMSK                                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_SHFT                                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000d8)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_INTEGLOOP_GAIN_MODE0_7_0_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_INTEGLOOP_GAIN_MODE0_7_0_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000dc)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_RMSK                                            0xf
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_INTEGLOOP_GAIN_MODE0_11_8_BMSK                  0xf
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_INTEGLOOP_GAIN_MODE0_11_8_SHFT                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000e0)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_INTEGLOOP_GAIN_MODE1_7_0_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN0_MODE1_INTEGLOOP_GAIN_MODE1_7_0_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_ADDR                                     (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000e4)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_RMSK                                            0xf
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_INTEGLOOP_GAIN_MODE1_11_8_BMSK                  0xf
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_GAIN1_MODE1_INTEGLOOP_GAIN_MODE1_11_8_SHFT                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000e8)
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_RMSK                                             0x1f
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_TOO_SLOW_CNTSEL_BMSK                       0x18
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_TOO_SLOW_CNTSEL_SHFT                        0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FORCE_CORRECT_POL_BMSK                            0x4
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FORCE_CORRECT_POL_SHFT                            0x2
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_CORRECT_EN_BMSK                        0x2
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_CORRECT_EN_SHFT                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_TIME_BMSK                              0x1
#define HWIO_UFS_MEM_QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_TIME_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000ec)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_RMSK                                                   0x1f
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCOCAL_POS_SLOPE_BMSK                                  0x10
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCOCAL_POS_SLOPE_SHFT                                   0x4
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE1_BMSK                                   0x8
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE1_SHFT                                   0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE0_BMSK                                   0x4
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE0_SHFT                                   0x2
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_MUX_BMSK                                    0x2
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_MUX_SHFT                                    0x1
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_BMSK                                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_SHFT                                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000f0)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_RMSK                                                    0x45
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_VCO_DEFAULT_FREQ_BMSK                                   0x40
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_VCO_DEFAULT_FREQ_SHFT                                    0x6
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0B_BMSK                                       0x4
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0B_SHFT                                       0x2
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0A_BMSK                                       0x1
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0A_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000f4)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_PLL_VCOTUNE_MODE0_7_0_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE0_PLL_VCOTUNE_MODE0_7_0_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000f8)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_PLL_VCOTUNE_MODE0_9_8_BMSK                            0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE0_PLL_VCOTUNE_MODE0_9_8_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x000000fc)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_PLL_VCOTUNE_MODE1_7_0_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE1_MODE1_PLL_VCOTUNE_MODE1_7_0_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000100)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_PLL_VCOTUNE_MODE1_9_8_BMSK                            0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE2_MODE1_PLL_VCOTUNE_MODE1_9_8_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000104)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_VCO_TUNE_INITVAL_7_0_BMSK                          0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL1_VCO_TUNE_INITVAL_7_0_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000108)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_RMSK                                                0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_VCO_TUNE_INITVAL_9_8_BMSK                           0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_VCO_TUNE_INITVAL_9_8_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_ADDR                                          (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000010c)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_RMSK                                                0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_VCO_TUNE_MINVAL_7_0_BMSK                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL1_VCO_TUNE_MINVAL_7_0_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_ADDR                                          (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000110)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_RMSK                                                 0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_VCO_TUNE_MINVAL_9_8_BMSK                             0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MINVAL2_VCO_TUNE_MINVAL_9_8_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_ADDR                                          (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000114)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_RMSK                                                0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_VCO_TUNE_MAXVAL_7_0_BMSK                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL1_VCO_TUNE_MAXVAL_7_0_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_ADDR                                          (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000118)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_RMSK                                                 0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_VCO_TUNE_MAXVAL_9_8_BMSK                             0x3
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAXVAL2_VCO_TUNE_MAXVAL_9_8_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000011c)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_PLL_VCOTUNE_TIMER_7_0_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER1_PLL_VCOTUNE_TIMER_7_0_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000120)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_ADDR, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_PLL_VCOTUNE_TIMER_15_8_BMSK                          0xff
#define HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_TIMER2_PLL_VCOTUNE_TIMER_15_8_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000124)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_REFGEN_READY_BMSK                                         0x80
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_REFGEN_READY_SHFT                                          0x7
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_FREEZEIO_D_BMSK                                           0x40
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_FREEZEIO_D_SHFT                                            0x6
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_REFCLK_DONE_BMSK                                0x20
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_REFCLK_DONE_SHFT                                 0x5
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_CORECLK_DONE_BMSK                               0x10
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_CORECLK_DONE_SHFT                                0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_AUXCLK_DONE_BMSK                                 0x8
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_SWITCH_TO_AUXCLK_DONE_SHFT                                 0x3
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_UNLOCKED_BMSK                                          0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_UNLOCKED_SHFT                                          0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_LOCKED_BMSK                                            0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_LOCKED_SHFT                                            0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_FREQ_DONE_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_STATUS_PLL_FREQ_DONE_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_ADDR                                           (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000128)
#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_RMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_RESETSM_BMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_COM_RESET_SM_STATUS_RESETSM_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000012c)
#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_RMSK                                             0x7f
#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_RESTRIM_CODE_BMSK                                0x7f
#define HWIO_UFS_MEM_QSERDES_COM_RESTRIM_CODE_STATUS_RESTRIM_CODE_SHFT                                 0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000130)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_RMSK                                             0xff
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_VCO_CTUNE_BINCODE_7_0_BMSK                       0xff
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE1_STATUS_VCO_CTUNE_BINCODE_7_0_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_ADDR                                       (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000134)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_RMSK                                              0x3
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_VCO_CTUNE_BINCODE_9_8_BMSK                        0x3
#define HWIO_UFS_MEM_QSERDES_COM_PLLCAL_CODE2_STATUS_VCO_CTUNE_BINCODE_9_8_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000138)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_RMSK                                                      0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_ADDR, HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_TSYNC_EN_MUX_BMSK                                         0x20
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_TSYNC_EN_MUX_SHFT                                          0x5
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_TSYNC_EN_BMSK                                             0x10
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_TSYNC_EN_SHFT                                              0x4
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_CMOS_PAD_SEL_BMSK                                   0x8
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_CMOS_PAD_SEL_SHFT                                   0x3
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_TX_SWINGSEL_BMSK                                    0x6
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_TX_SWINGSEL_SHFT                                    0x1
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_TX_SYSCLKSEL_BMSK                                   0x1
#define HWIO_UFS_MEM_QSERDES_COM_CLK_SELECT_SYSCLK_TX_SYSCLKSEL_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_ADDR                                                 (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000013c)
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_RMSK                                                       0xff
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE1_BMSK                                    0xf0
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE1_SHFT                                     0x4
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE0_BMSK                                     0xf
#define HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE0_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_ADDR                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000140)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_RMSK                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_INTEGLOOP_BINCODE_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_INTEGLOOP_BINCODE_STATUS_INTEGLOOP_BINCODE_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000144)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_RMSK                                                       0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_ADDR, HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_IN)
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_PLL_ANALOG_BMSK                                            0x1
#define HWIO_UFS_MEM_QSERDES_COM_PLL_ANALOG_PLL_ANALOG_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000148)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_ADDR, HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_CORE_CLK_DIV_MODE0_BMSK                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE0_CORE_CLK_DIV_MODE0_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000014c)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_ADDR, HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_CORE_CLK_DIV_MODE1_BMSK                            0xff
#define HWIO_UFS_MEM_QSERDES_COM_CORECLK_DIV_MODE1_CORE_CLK_DIV_MODE1_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000150)
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_RMSK                                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SW_RESET_ADDR, HWIO_UFS_MEM_QSERDES_COM_SW_RESET_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SW_RESET_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SW_RESET_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SW_RESET_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SW_RESET_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_SW_RESET_BMSK                                                0x1
#define HWIO_UFS_MEM_QSERDES_COM_SW_RESET_SW_RESET_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_ADDR                                               (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000154)
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_RMSK                                                     0x3f
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_ADDR, HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE1_BMSK                               0x20
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE1_SHFT                                0x5
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE0_BMSK                               0x10
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE0_SHFT                                0x4
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CORECLK_EN_MUX_BMSK                                       0x8
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CORECLK_EN_MUX_SHFT                                       0x3
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CORECLK_EN_BMSK                                           0x4
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_CORECLK_EN_SHFT                                           0x2
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_MUX_BMSK                                        0x2
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_MUX_SHFT                                        0x1
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_BMSK                                            0x1
#define HWIO_UFS_MEM_QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_ADDR                                            (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000158)
#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_RMSK                                                   0x1
#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_C_READY_BMSK                                           0x1
#define HWIO_UFS_MEM_QSERDES_COM_C_READY_STATUS_C_READY_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000015c)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_RMSK                                                      0x7f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_SSC_STEP_DIV2_LSB_BMSK                                    0x40
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_SSC_STEP_DIV2_LSB_SHFT                                     0x6
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_FRAC_START_DIV2_LSB_MODE1_BMSK                            0x20
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_FRAC_START_DIV2_LSB_MODE1_SHFT                             0x5
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_FRAC_START_DIV2_LSB_MODE0_BMSK                            0x10
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_FRAC_START_DIV2_LSB_MODE0_SHFT                             0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PSM_SWITCH_CORECLK_BMSK                                    0x8
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PSM_SWITCH_CORECLK_SHFT                                    0x3
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PSM_SWITCH_AUXCLK_BMSK                                     0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PSM_SWITCH_AUXCLK_SHFT                                     0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PLL_FBCLK_SEL_BMSK                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_PLL_FBCLK_SEL_SHFT                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_CLK_BIAS_REFSEL_BMSK                                       0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_CONFIG_CLK_BIAS_REFSEL_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_ADDR                                         (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000160)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_RMSK                                                0x7
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_UPDATE_BMSK                             0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_UPDATE_SHFT                             0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_SEL_BMSK                                0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_SEL_SHFT                                0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_CODE_BMSK                                  0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_CODE_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR                                          (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000164)
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_RMSK                                                 0xf
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE1_BMSK                         0xc
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE1_SHFT                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE0_BMSK                         0x3
#define HWIO_UFS_MEM_QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE0_SHFT                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000168)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_DEBUG_BUS_7_0_BMSK                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS0_DEBUG_BUS_7_0_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000016c)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_DEBUG_BUS_15_8_BMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS1_DEBUG_BUS_15_8_SHFT                                        0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000170)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_DEBUG_BUS_23_16_BMSK                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS2_DEBUG_BUS_23_16_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_ADDR                                                (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000174)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_RMSK                                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_DEBUG_BUS_31_24_BMSK                                      0xff
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS3_DEBUG_BUS_31_24_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_ADDR                                             (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000178)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_RMSK                                                    0xf
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_DEBUG_BUS_SEL_BMSK                                      0xf
#define HWIO_UFS_MEM_QSERDES_COM_DEBUG_BUS_SEL_DEBUG_BUS_SEL_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_ADDR                                                 (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x0000017c)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_RMSK                                                       0x7f
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DIS_CG_RATE_CHANGE_BMSK                                    0x40
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DIS_CG_RATE_CHANGE_SHFT                                     0x6
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DIS_CG_SYSGLITCH_BMSK                                      0x20
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DIS_CG_SYSGLITCH_SHFT                                       0x5
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_RESET_MUX_BMSK                                         0x10
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_RESET_MUX_SHFT                                          0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_RESET_BMSK                                              0x8
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_RESET_SHFT                                              0x3
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_VREG_READY_MUX_BMSK                                     0x4
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_VREG_READY_MUX_SHFT                                     0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_VREG_READY_BMSK                                         0x2
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_PLL_VREG_READY_SHFT                                         0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DISABLE_B2T_INTEGLOOP_CLKGATE_BMSK                          0x1
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC1_DISABLE_B2T_INTEGLOOP_CLKGATE_SHFT                          0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_ADDR                                                 (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000180)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_RMSK                                                       0xff
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_RSVD_BMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MISC2_RSVD_SHFT                                                   0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_ADDR                                                  (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000184)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_CMN_MODE_BMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_COM_CMN_MODE_CMN_MODE_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_ADDR                                              (UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL_REG_BASE      + 0x00000188)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_RMSK                                                     0x7
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_ADDR, HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_CMN_VREG_SEL_BMSK                                        0x7
#define HWIO_UFS_MEM_QSERDES_COM_CMN_VREG_SEL_CMN_VREG_SEL_SHFT                                        0x0

/*----------------------------------------------------------------------------
 * MODULE: UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX
 *--------------------------------------------------------------------------*/

#define UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE                                        (UFS_MEM_UFS_1LANE_REGS_BASE      + 0x00007600)

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_ADDR                                            (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000000)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_RMSK                                                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_UCDR_FO_GAIN_HALF_BMSK                                0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_HALF_UCDR_FO_GAIN_HALF_SHFT                                 0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000004)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_RMSK                                               0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_UCDR_FO_GAIN_QUARTER_BMSK                          0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_QUARTER_UCDR_FO_GAIN_QUARTER_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000008)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_RMSK                                                       0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_UCDR_FO_GAIN_BMSK                                          0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_UCDR_FO_GAIN_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_ADDR                                            (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000000c)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_RMSK                                                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_UCDR_SO_GAIN_HALF_BMSK                                0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_HALF_UCDR_SO_GAIN_HALF_SHFT                                 0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000010)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_RMSK                                               0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_UCDR_SO_GAIN_QUARTER_BMSK                          0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_QUARTER_UCDR_SO_GAIN_QUARTER_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000014)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_RMSK                                                       0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_UCDR_SO_GAIN_BMSK                                          0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_UCDR_SO_GAIN_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000018)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_RMSK                                              0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_UCDR_SVS_FO_GAIN_HALF_BMSK                        0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_HALF_UCDR_SVS_FO_GAIN_HALF_SHFT                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_ADDR                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000001c)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_RMSK                                           0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_UCDR_SVS_FO_GAIN_QUARTER_BMSK                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_QUARTER_UCDR_SVS_FO_GAIN_QUARTER_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000020)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_RMSK                                                   0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_UCDR_SVS_FO_GAIN_BMSK                                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_FO_GAIN_UCDR_SVS_FO_GAIN_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000024)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_RMSK                                              0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_UCDR_SVS_SO_GAIN_HALF_BMSK                        0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_HALF_UCDR_SVS_SO_GAIN_HALF_SHFT                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_ADDR                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000028)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_RMSK                                           0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_UCDR_SVS_SO_GAIN_QUARTER_BMSK                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_QUARTER_UCDR_SVS_SO_GAIN_QUARTER_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000002c)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_RMSK                                                   0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_UCDR_SVS_SO_GAIN_BMSK                                  0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SVS_SO_GAIN_UCDR_SVS_SO_GAIN_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000030)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_RMSK                                              0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_UCDR_FASTLOCK_FO_GAIN_BMSK                        0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_UCDR_FASTLOCK_FO_GAIN_SHFT                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_ADDR                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000034)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_RMSK                                      0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_UCDR_ENABLE_BMSK                          0x40
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_UCDR_ENABLE_SHFT                           0x6
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_UCDR_SO_SATURATION_BMSK                   0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_UCDR_SO_SATURATION_SHFT                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_ADDR                                          (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000038)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_RMSK                                                0xff
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_UCDR_FO_TO_SO_DELAY_BMSK                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_TO_SO_DELAY_UCDR_FO_TO_SO_DELAY_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_ADDR                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000003c)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_RMSK                                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_UCDR_FASTLOCK_COUNTER_LOW_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_UCDR_FASTLOCK_COUNTER_LOW_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_ADDR                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000040)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_RMSK                                            0xf
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_UCDR_FASTLOCK_COUNTER_HIGH_BMSK                 0xf
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_HIGH_UCDR_FASTLOCK_COUNTER_HIGH_SHFT                 0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000044)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_SB2_EN_BMSK                                            0x80
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_SB2_EN_SHFT                                             0x7
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_10_BMSK                           0x40
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_10_SHFT                            0x6
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_01_BMSK                           0x20
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_01_SHFT                            0x5
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_00_BMSK                           0x10
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_TRAN_CDRCLK_DIV6_SEL_00_SHFT                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_BLOCK_STEP_BY_TWO_BMSK                             0x8
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_BLOCK_STEP_BY_TWO_SHFT                             0x3
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_MUX_BMSK                               0x4
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_MUX_SHFT                               0x2
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_BMSK                                   0x2
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_SHFT                                   0x1
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_FASTLOCK_ENABLE_BMSK                               0x1
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_UCDR_FASTLOCK_ENABLE_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000048)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_RMSK                                                   0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_DIV6PD_THRESH1_BMSK                                    0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH1_DIV6PD_THRESH1_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000004c)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_RMSK                                                   0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_DIV6PD_THRESH2_BMSK                                    0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_THRESH2_DIV6PD_THRESH2_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000050)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_RMSK                                                     0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_DIV6PD_GAIN1_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN1_DIV6PD_GAIN1_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000054)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_RMSK                                                     0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_DIV6PD_GAIN2_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_UCDR_SB2_GAIN2_DIV6PD_GAIN2_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_ADDR                                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000058)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_RMSK                                                        0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_AUX_OFFSET_BMSK                                             0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_CONTROL_AUX_OFFSET_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_ADDR                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000005c)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_RMSK                                             0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_AUXDATA_TCOARSE_BMSK                             0x70
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_AUXDATA_TCOARSE_SHFT                              0x4
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_AUXDATA_TFINE_BMSK                                0xf
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA_TCOARSE_TFINE_AUXDATA_TFINE_SHFT                                0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000060)
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_EN_BMSK                                        0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_EN_SHFT                                         0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_EN_MUX_BMSK                                    0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_EN_MUX_SHFT                                     0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXCLK_EN_BMSK                                         0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXCLK_EN_SHFT                                          0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXCLK_EN_MUX_BMSK                                     0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXCLK_EN_MUX_SHFT                                      0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IACOMPARE_CLEAR_BMSK                                    0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IACOMPARE_CLEAR_SHFT                                    0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IACOMPARE_ENABLE_BMSK                                   0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_IACOMPARE_ENABLE_SHFT                                   0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_SEL_BMSK                                        0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_AUXDATA_SEL_SHFT                                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_RCLK_SEL_BMSK                                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RCLK_AUXDATA_SEL_RCLK_SEL_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000064)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_RMSK                                                      0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_AC_JTAG_EN_BMSK                                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_ENABLE_AC_JTAG_EN_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000068)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_RMSK                                                       0x3
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_AC_JTAG_INITP_MUX_BMSK                                     0x2
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_AC_JTAG_INITP_MUX_SHFT                                     0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_AC_JTAG_INITP_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITP_AC_JTAG_INITP_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000006c)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_RMSK                                                       0x3
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_AC_JTAG_INITN_MUX_BMSK                                     0x2
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_AC_JTAG_INITN_MUX_SHFT                                     0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_AC_JTAG_INITN_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_INITN_AC_JTAG_INITN_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_ADDR                                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000070)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_RMSK                                                         0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_AC_JTAG_LVL_BMSK                                             0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_LVL_AC_JTAG_LVL_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000074)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_RMSK                                                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_AC_JTAG_MODE_BMSK                                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_MODE_AC_JTAG_MODE_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000078)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_RMSK                                                       0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_AC_JTAG_RESET_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_RESET_AC_JTAG_RESET_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000007c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RMSK                                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND3_BMSK                                        0xc0
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND3_SHFT                                         0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND2_BMSK                                        0x30
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND2_SHFT                                         0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND1_BMSK                                         0xc
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND1_SHFT                                         0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND0_BMSK                                         0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_RX_TERM_BW_BAND0_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000080)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RMSK                                                      0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_POL_INV_BMSK                                           0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_POL_INV_SHFT                                            0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_Q_EN_MUX_BMSK                                          0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_Q_EN_MUX_SHFT                                           0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_Q_EN_BMSK                                              0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_Q_EN_SHFT                                               0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_I_EN_MUX_BMSK                                           0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_I_EN_MUX_SHFT                                           0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_I_EN_BMSK                                               0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RX_I_EN_SHFT                                               0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RCVR_EN_MUX_BMSK                                           0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RCVR_EN_MUX_SHFT                                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RCVR_EN_BMSK                                               0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_RCVR_IQ_EN_RCVR_EN_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000084)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_RMSK                                               0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_IDAC_I_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_I_DC_OFFSETS_IDAC_I_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_ADDR                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000088)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_RMSK                                            0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_IDAC_IB_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_IBAR_DC_OFFSETS_IDAC_IB_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000008c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_RMSK                                               0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_IDAC_Q_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_Q_DC_OFFSETS_IDAC_Q_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_ADDR                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000090)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_RMSK                                            0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_IDAC_QB_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_QBAR_DC_OFFSETS_IDAC_QB_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000094)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_RMSK                                               0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_IDAC_A_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_A_DC_OFFSETS_IDAC_A_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_ADDR                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000098)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_RMSK                                            0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_IDAC_AB_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ABAR_DC_OFFSETS_IDAC_AB_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000009c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_RMSK                                                         0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_IDAC_FORCE_MASK_BMSK                                    0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_IDAC_FORCE_MASK_SHFT                                     0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_MUX_BMSK                                                0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_MUX_SHFT                                                 0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_BYPASS_BMSK                                         0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_BYPASS_SHFT                                          0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_EN_MUX_BMSK                                          0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_EN_MUX_SHFT                                          0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_EN_BMSK                                              0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_IDAC_CAL_EN_SHFT                                              0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_RX_IDAC_CAL_EN_MUX_BMSK                                  0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_RX_IDAC_CAL_EN_MUX_SHFT                                  0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_RX_IDAC_CAL_EN_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_EN_CORE_RX_IDAC_CAL_EN_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_ADDR                                              (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000a0)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_RMSK                                                    0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_IDAC_ENABLE_BMSK                                        0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ENABLES_IDAC_ENABLE_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000a4)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_RMSK                                                       0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_IDAC_SIGN_BMSK                                             0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_SIGN_IDAC_SIGN_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_ADDR                                            (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000a8)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RMSK                                                   0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RX_HIGHZ_MUX_BMSK                                      0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RX_HIGHZ_MUX_SHFT                                      0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RX_HIGHZ_BMSK                                          0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_HIGHZ_HIGHRATE_RX_HIGHZ_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_ADDR                           (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000ac)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RMSK                                 0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_MUX_BMSK                0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_MUX_SHFT                 0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_BMSK                    0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_SHFT                     0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_MUX_BMSK                 0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_MUX_SHFT                 0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_BMSK                     0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_SHFT                     0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_MUX_BMSK                 0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_MUX_SHFT                 0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_BMSK                     0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_SHFT                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_ADDR                                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000b0)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_RMSK                                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_DFE_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_DFE_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_DFE_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_DFE_1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_TAP1CODE_MAN_VAL_BMSK                                             0xf8
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_TAP1CODE_MAN_VAL_SHFT                                              0x3
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_KTAP1_BMSK                                                         0x7
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_1_KTAP1_SHFT                                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_ADDR                                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000b4)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_RMSK                                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_DFE_2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_DFE_2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_DFE_2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_DFE_2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_TAP2CODE_MAN_VAL_BMSK                                             0xf8
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_TAP2CODE_MAN_VAL_SHFT                                              0x3
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_KTAP2_BMSK                                                         0x7
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_2_KTAP2_SHFT                                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_ADDR                                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000b8)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_RMSK                                                              0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_3_ADDR, HWIO_UFS_MEM_QSERDES_RX0_DFE_3_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_DFE_3_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_DFE_3_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_DFE_3_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_MODE_BMSK                                                 0x20
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_MODE_SHFT                                                  0x5
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_EN_BMSK                                                   0x10
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_EN_SHFT                                                    0x4
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_FLIP_SIGN_BMSK                                             0x8
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP2ADP_FLIP_SIGN_SHFT                                             0x3
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_MODE_BMSK                                                  0x4
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_MODE_SHFT                                                  0x2
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_EN_BMSK                                                    0x2
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_EN_SHFT                                                    0x1
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_FLIP_SIGN_BMSK                                             0x1
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_3_TAP1ADP_FLIP_SIGN_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000bc)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_RMSK                                                     0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGA_UNROLL_BMSK                                          0x40
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGA_UNROLL_SHFT                                           0x6
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_MODE_BMSK                                         0x20
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_MODE_SHFT                                          0x5
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_EN_BMSK                                           0x10
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_EN_SHFT                                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_FLIP_SIGN_BMSK                                     0x8
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_VGAADP_FLIP_SIGN_SHFT                                     0x3
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_KVGA_BMSK                                                 0x7
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL1_KVGA_SHFT                                                 0x0

#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000c0)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_RMSK                                                      0xf
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_VGACODE_MAN_VAL_BMSK                                      0xf
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_CAL_CNTRL2_VGACODE_MAN_VAL_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_ADDR                                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000c4)
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_RMSK                                                             0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_TRAN_RCVR_GMCAL_RES_BMSK                                         0x1e
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_TRAN_RCVR_GMCAL_RES_SHFT                                          0x1
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_TRAN_RX_GMCAL_EN_BMSK                                             0x1
#define HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_TRAN_RX_GMCAL_EN_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_ADDR                                              (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000c8)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_RX_EQ_GAIN2_LSB_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_LSB_RX_EQ_GAIN2_LSB_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_ADDR                                              (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000cc)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_RX_EQ_GAIN2_MSB_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_GAIN2_MSB_RX_EQ_GAIN2_MSB_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000d0)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_RMSK                                               0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_OFFSETADP_MODE_MUX_BMSK                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_OFFSETADP_MODE_MUX_SHFT                            0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_MUX_BMSK                               0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_MUX_SHFT                               0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_BMSK                                   0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000d4)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_RMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_CORE_EQADP_MASK_BMSK                              0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_CORE_EQADP_MASK_SHFT                               0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_MUX_BMSK                                 0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_MUX_SHFT                                  0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_BMSK                                     0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_SHFT                                      0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN2_MUX_BMSK                                 0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN2_MUX_SHFT                                  0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQDEGCAP_BMSK                                      0xf
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_EQDEGCAP_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000d8)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_RMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_CORE_EQADP_FORCE_MASK_BMSK                        0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_CORE_EQADP_FORCE_MASK_SHFT                         0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_KEQ_BMSK                                          0x70
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_KEQ_SHFT                                           0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_FLIP_SIGN_BMSK                               0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_FLIP_SIGN_SHFT                               0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_MUX_BMSK                                0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_MUX_SHFT                                0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_BMSK                                    0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000dc)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_RMSK                                              0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_MUX_BMSK                           0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_MUX_SHFT                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_BMSK                                0xf
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_SHFT                                0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_ADDR                                          (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000e0)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_RMSK                                                0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_TSETTLE_LOW_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_TSETTLE_LOW_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000e4)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_RMSK                                                0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_TSETTLE_HIGH_BMSK                                   0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_TSETTLE_HIGH_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000e8)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_RMSK                                               0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_IDAC_MEASURE_TIME_BMSK                             0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_IDAC_MEASURE_TIME_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_ADDR                                          (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000ec)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_RMSK                                                0x7f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IDAC_SKIP_ACCUM_BMSK                                0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IDAC_SKIP_ACCUM_SHFT                                 0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IDAC_ACCUM_SAT_VALUE_BMSK                           0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_ACCUMULATOR_IDAC_ACCUM_SAT_VALUE_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000f0)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_RX_EQ_OFFSET_LSB_BMSK                                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_LSB_RX_EQ_OFFSET_LSB_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000f4)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_RX_EQ_OFFSET_MSB_BMSK                                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_MSB_RX_EQ_OFFSET_MSB_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000f8)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RMSK                                        0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RX_EQ_OFFSET_MUX_BMSK                       0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RX_EQ_OFFSET_MUX_SHFT                        0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_KOFFSET_BMSK                                0x70
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_KOFFSET_SHFT                                 0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_FLIP_SIGN_BMSK                     0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_FLIP_SIGN_SHFT                     0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_MODE_BMSK                          0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_MODE_SHFT                          0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_MUX_BMSK                        0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_MUX_SHFT                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_BMSK                            0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_ADDR                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000000fc)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_FORCE_MASK_BMSK                  0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_FORCE_MASK_SHFT                   0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_MASK_BMSK                        0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_MASK_SHFT                         0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_MUX_BMSK                    0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_MUX_SHFT                     0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_BMSK                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_SHFT                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000100)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_CORE_RX_SIGDET_MUX_BMSK                                  0x80
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_CORE_RX_SIGDET_MUX_SHFT                                   0x7
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_CORE_RX_SIGDET_BMSK                                      0x60
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_CORE_RX_SIGDET_SHFT                                       0x5
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_FLT_BYP_BMSK                                      0x10
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_FLT_BYP_SHFT                                       0x4
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_LP_EN_MUX_BMSK                                     0x8
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_LP_EN_MUX_SHFT                                     0x3
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_LP_EN_BMSK                                         0x4
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_LP_EN_SHFT                                         0x2
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_EN_MUX_BMSK                                        0x2
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_EN_MUX_SHFT                                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_EN_BMSK                                            0x1
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_ENABLES_SIGDET_EN_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000104)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_RMSK                                                       0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_SIGDET_CAP_CTRL_BMSK                                       0xf0
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_SIGDET_CAP_CTRL_SHFT                                        0x4
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_SIGDET_BW_CTRL_BMSK                                         0xf
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_SIGDET_BW_CTRL_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000108)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_RMSK                                                         0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_UFS_MODE_BMSK                                         0x20
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_UFS_MODE_SHFT                                          0x5
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_LVL_MUX_BMSK                                          0x10
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_LVL_MUX_SHFT                                           0x4
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_LVL_BMSK                                               0xf
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_SIGDET_LVL_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000010c)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_RMSK                                              0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_CTRL_BMSK                         0x1e
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_CTRL_SHFT                          0x1
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_BYP_BMSK                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_BYP_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000110)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RMSK                                                            0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_UNMODIFIED_MUX_BMSK                                     0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_UNMODIFIED_MUX_SHFT                                      0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_10_HALF_BMSK                                            0x10
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_10_HALF_SHFT                                             0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_01_HALF_BMSK                                             0x8
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_01_HALF_SHFT                                             0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_MUX_BMSK                                                 0x4
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_MUX_SHFT                                                 0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_BMSK                                                     0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_RX_BAND_SHFT                                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000114)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_RMSK                                                    0x7
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_DN_BMSK                                             0x4
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_DN_SHFT                                             0x2
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_UP_BMSK                                             0x2
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_UP_SHFT                                             0x1
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_FREEZE_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_FREEZE_UP_DN_CDR_FREEZE_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_ADDR                                           (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000118)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_MUX_BMSK                           0x2
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_MUX_SHFT                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_BMSK                               0x1
#define HWIO_UFS_MEM_QSERDES_RX0_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_ADDR                                            (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000011c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_SVS_MODE_MUX_BMSK                                     0x80
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_SVS_MODE_MUX_SHFT                                      0x7
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_SVS_MODE_BMSK                                         0x40
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_SVS_MODE_SHFT                                          0x6
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RX_CLOCK_EDGE_BMSK                                    0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RX_CLOCK_EDGE_SHFT                                     0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RXINTERFACE_MODE_BMSK                                 0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_INTERFACE_MODE_RXINTERFACE_MODE_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_ADDR                                              (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000120)
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_JITTERGENMODE_BMSK                                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_JITTER_GEN_MODE_JITTERGENMODE_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000124)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_RMSK                                                            0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_ADDR, HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_BUJ_AMPLITUDE_BMSK                                              0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_AMP_BUJ_AMPLITUDE_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000128)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_RMSK                                                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_SJ_AMPLITUDE1_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP1_SJ_AMPLITUDE1_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000012c)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_RMSK                                                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_SJ_AMPLITUDE2_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_AMP2_SJ_AMPLITUDE2_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000130)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_RMSK                                                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_SJ_PERIOD1_BMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER1_SJ_PERIOD1_SHFT                                                  0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000134)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_RMSK                                                            0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_SJ_PERIOD2_BMSK                                                 0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SJ_PER2_SJ_PERIOD2_SHFT                                                  0x0

#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000138)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_BUJ_STEPFREQ1_BMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ1_BUJ_STEPFREQ1_SHFT                                        0x0

#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_ADDR                                               (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000013c)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_BUJ_STEPFREQ2_BMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_RX0_BUJ_STEP_FREQ2_BUJ_STEPFREQ2_SHFT                                        0x0

#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_ADDR                                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000140)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_PPMOFFSET1_BMSK                                             0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET1_PPMOFFSET1_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_ADDR                                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000144)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_RMSK                                                        0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_PPMOFFSET2_BMSK                                             0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PPM_OFFSET2_PPMOFFSET2_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000148)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_SIGN_PPM_PERIOD1_BMSK                                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD1_SIGN_PPM_PERIOD1_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000014c)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_SIGN_PPM_PERIOD2_BMSK                                  0xff
#define HWIO_UFS_MEM_QSERDES_RX0_SIGN_PPM_PERIOD2_SIGN_PPM_PERIOD2_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_ADDR                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000150)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RMSK                                             0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_MUX_BMSK                             0x20
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_MUX_SHFT                              0x5
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_BMSK                                 0x1c
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_SHFT                                  0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_MUX_BMSK                                0x2
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_MUX_SHFT                                0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_BMSK                                    0x1
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_ADDR                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000154)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_RX_PWM_GEAR1_COUNT_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR1_TIMEOUT_COUNT_RX_PWM_GEAR1_COUNT_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_ADDR                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000158)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_RX_PWM_GEAR2_COUNT_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR2_TIMEOUT_COUNT_RX_PWM_GEAR2_COUNT_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_ADDR                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000015c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_RX_PWM_GEAR3_COUNT_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR3_TIMEOUT_COUNT_RX_PWM_GEAR3_COUNT_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_ADDR                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000160)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_RX_PWM_GEAR4_COUNT_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_PWM_GEAR4_TIMEOUT_COUNT_RX_PWM_GEAR4_COUNT_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000164)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_RMSK                                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_RX_MODE_00_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_RX_MODE_00_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000168)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_RMSK                                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_RX_MODE_01_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_RX_MODE_01_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_ADDR                                                   (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000016c)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_RMSK                                                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_RX_MODE_10_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_RX_MODE_10_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000170)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_IN)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ALOG_OBSV_BUS_CTRL_1_SEL_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_CTRL_1_ALOG_OBSV_BUS_CTRL_1_SEL_SHFT                       0x0

#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_ADDR                                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000174)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_RMSK                                                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_PI_CTRL1_STATUS_BMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL1_PI_CTRL1_STATUS_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_ADDR                                                     (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000178)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_RMSK                                                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_PI_CTRL2_STATUS_BMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_PI_CTRL2_PI_CTRL2_STATUS_SHFT                                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_ADDR                                                      (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000017c)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_RMSK                                                             0x3
#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_ADDR, HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_PI_QUAD_STATUS_BMSK                                              0x3
#define HWIO_UFS_MEM_QSERDES_RX0_PI_QUAD_PI_QUAD_STATUS_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_ADDR                                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000180)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_RMSK                                                             0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDATA1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDATA1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDATA1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_IDATA1_STATUS_BMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA1_IDATA1_STATUS_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_ADDR                                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000184)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_RMSK                                                             0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDATA2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDATA2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDATA2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_IDATA2_STATUS_BMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IDATA2_IDATA2_STATUS_SHFT                                                0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_ADDR                                                    (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000188)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_RMSK                                                          0xff
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_AUXDATA1_STATUS_BMSK                                          0xff
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA1_AUXDATA1_STATUS_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_ADDR                                                    (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000018c)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_RMSK                                                           0x3
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_AUXDATA2_STATUS_BMSK                                           0x3
#define HWIO_UFS_MEM_QSERDES_RX0_AUX_DATA2_AUXDATA2_STATUS_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000190)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_RMSK                                                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_BMSK                                    0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_ADDR                                                 (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000194)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_RMSK                                                        0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_ADDR, HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_BMSK                                    0x1
#define HWIO_UFS_MEM_QSERDES_RX0_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_ADDR                                                    (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x00000198)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_RMSK                                                           0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_ADDR, HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_RX_SIGDET_OUT_STATUS_BMSK                                      0x3
#define HWIO_UFS_MEM_QSERDES_RX0_RX_SIGDET_RX_SIGDET_OUT_STATUS_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x0000019c)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_RMSK                                                      0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_IDAC_I_STATUS_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_I_IDAC_I_STATUS_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001a0)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_RMSK                                                   0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_IDAC_IB_STATUS_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_IBAR_IDAC_IB_STATUS_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001a4)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_RMSK                                                      0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_IDAC_Q_STATUS_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_Q_IDAC_Q_STATUS_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001a8)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_RMSK                                                   0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_IDAC_QB_STATUS_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_QBAR_IDAC_QB_STATUS_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001ac)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_RMSK                                                      0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_IDAC_A_STATUS_BMSK                                        0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_A_IDAC_A_STATUS_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_ADDR                                             (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001b0)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_RMSK                                                   0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_IDAC_AB_STATUS_BMSK                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_ABAR_IDAC_AB_STATUS_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_ADDR                                            (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001b4)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_RMSK                                                  0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_CAL_DONE_STATUS_BMSK                             0x10
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_CAL_DONE_STATUS_SHFT                              0x4
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_CAL_ON_STATUS_BMSK                                0x8
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_CAL_ON_STATUS_SHFT                                0x3
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_SM_STATUS_BMSK                                    0x7
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SM_ON_IDAC_SM_STATUS_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001b8)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_RMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_AB_BMSK                              0x20
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_AB_SHFT                               0x5
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_A_BMSK                               0x10
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_A_SHFT                                0x4
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_QB_BMSK                               0x8
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_QB_SHFT                               0x3
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_Q_BMSK                                0x4
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_Q_SHFT                                0x2
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_IB_BMSK                               0x2
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_IB_SHFT                               0x1
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_I_BMSK                                0x1
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_CAL_DONE_IDAC_CAL_DONE_I_SHFT                                0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001bc)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_RMSK                                              0x3f
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_AB_BMSK                          0x20
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_AB_SHFT                           0x5
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_A_BMSK                           0x10
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_A_SHFT                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_QB_BMSK                           0x8
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_QB_SHFT                           0x3
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_Q_BMSK                            0x4
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_Q_SHFT                            0x2
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_IB_BMSK                           0x2
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_IB_SHFT                           0x1
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_I_BMSK                            0x1
#define HWIO_UFS_MEM_QSERDES_RX0_IDAC_STATUS_SIGNERROR_SIGNERROR_STATUS_I_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_ADDR                                                  (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001c0)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_RMSK                                                         0xf
#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_READ_EQCODE_STATUS_BMSK                                      0xf
#define HWIO_UFS_MEM_QSERDES_RX0_READ_EQCODE_READ_EQCODE_STATUS_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_ADDR                                              (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001c4)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_RMSK                                                    0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_READ_OFFSETCODE_STATUS_BMSK                             0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_READ_OFFSETCODE_READ_OFFSETCODE_STATUS_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_ADDR                                         (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001c8)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_IACOMPARE_COUNT_LOW_BMSK                           0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_LOW_IACOMPARE_COUNT_LOW_SHFT                            0x0

#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_ADDR                                        (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001cc)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_RMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_ADDR, HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_IACOMPARE_COUNT_HIGH_BMSK                         0xff
#define HWIO_UFS_MEM_QSERDES_RX0_IA_ERROR_COUNTER_HIGH_IACOMPARE_COUNT_HIGH_SHFT                          0x0

#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_ADDR                                                (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001d0)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_RMSK                                                       0xf
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_READ_VGACODE_BMSK                                          0xf
#define HWIO_UFS_MEM_QSERDES_RX0_VGA_READ_CODE_READ_VGACODE_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_ADDR                                           (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001d4)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_RMSK                                                 0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_READ_TAP1CODE_BMSK                                   0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP1_READ_CODE_READ_TAP1CODE_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_ADDR                                           (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001d8)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_RMSK                                                 0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_ADDR, HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_READ_TAP2CODE_BMSK                                   0x1f
#define HWIO_UFS_MEM_QSERDES_RX0_DFE_TAP2_READ_CODE_READ_TAP2CODE_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_ADDR                                       (UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX_REG_BASE      + 0x000001dc)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_RMSK                                             0xff
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_ADDR, HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_ALOG_OBSV_BUS_STATUS_1_BMSK                      0xff
#define HWIO_UFS_MEM_QSERDES_RX0_ALOG_OBSV_BUS_STATUS_1_ALOG_OBSV_BUS_STATUS_1_SHFT                       0x0

/*----------------------------------------------------------------------------
 * MODULE: UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX
 *--------------------------------------------------------------------------*/

#define UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE                                    (UFS_MEM_UFS_1LANE_REGS_BASE      + 0x00007400)

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000000)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BIST_ERROR_INJECT_BMSK                             0x80
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BIST_ERROR_INJECT_SHFT                              0x7
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BIST_LANE_NUMBER_BMSK                              0x60
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BIST_LANE_NUMBER_SHFT                               0x5
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BISTMODE_BMSK                                      0x1f
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_MODE_LANENO_BISTMODE_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000004)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_RMSK                                                     0x3
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_BIST_INVERT_RX_BMSK                                      0x2
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_BIST_INVERT_RX_SHFT                                      0x1
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_BIST_INVERT_TX_BMSK                                      0x1
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_INVERT_BIST_INVERT_TX_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000008)
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_RMSK                                                  0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_ADDR, HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_TXONLY_CLOCK_EDGE_BMSK                                0x20
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_TXONLY_CLOCK_EDGE_SHFT                                 0x5
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_GATED_CLK_EN_BMSK                                     0x10
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_GATED_CLK_EN_SHFT                                      0x4
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE1_MUX_BMSK                               0x8
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE1_MUX_SHFT                               0x3
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE1_BMSK                                   0x4
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE1_SHFT                                   0x2
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE0_MUX_BMSK                               0x2
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE0_MUX_SHFT                               0x1
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE0_BMSK                                   0x1
#define HWIO_UFS_MEM_QSERDES_TX0_CLKBUF_ENABLE_CLKBUF_EN_LANE0_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000000c)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_RMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_MUX_BMSK                          0x20
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_MUX_SHFT                           0x5
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_BMSK                              0x1f
#define HWIO_UFS_MEM_QSERDES_TX0_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000010)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_RMSK                                                   0x7
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_TX_POST2_EMPH_BMSK                                     0x7
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POST2_EMPH_TX_POST2_EMPH_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_ADDR                                       (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000014)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_RMSK                                             0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_DN_BMSK                             0x38
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_DN_SHFT                              0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_UP_BMSK                              0x7
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_UP_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_ADDR                                    (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000018)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_RMSK                                           0x7
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_MUX_BMSK                           0x4
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_MUX_SHFT                           0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_BMSK                               0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000001c)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_RMSK                                                     0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_TX_DRV_LVL_MUX_BMSK                                      0x20
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_TX_DRV_LVL_MUX_SHFT                                       0x5
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_TX_DRV_LVL_BMSK                                          0x1f
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_TX_DRV_LVL_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_ADDR                                        (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000020)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_RMSK                                               0xf
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_TX_DRV_LVL_OFFSET_BMSK                             0xf
#define HWIO_UFS_MEM_QSERDES_TX0_TX_DRV_LVL_OFFSET_TX_DRV_LVL_OFFSET_SHFT                             0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_ADDR                                           (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000024)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RMSK                                                  0x3
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RESET_TSYNC_EN_MUX_BMSK                               0x2
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RESET_TSYNC_EN_MUX_SHFT                               0x1
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RESET_TSYNC_EN_BMSK                                   0x1
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_TSYNC_EN_RESET_TSYNC_EN_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_ADDR                                   (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000028)
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_BOOST_EN_MUX_BMSK                            0x80
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_BOOST_EN_MUX_SHFT                             0x7
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_BOOST_EN_BMSK                                0x40
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_BOOST_EN_SHFT                                 0x6
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_LDO_EN_MUX_BMSK                              0x20
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_LDO_EN_MUX_SHFT                               0x5
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_LDO_EN_BMSK                                  0x10
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_LDO_EN_SHFT                                   0x4
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_STALL_EN_MUX_BMSK                             0x8
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_STALL_EN_MUX_SHFT                             0x3
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_STALL_EN_BMSK                                 0x4
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_STALL_EN_SHFT                                 0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_PRE_EN_MUX_BMSK                               0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_PRE_EN_MUX_SHFT                               0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_PRE_EN_BMSK                                   0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PRE_STALL_LDO_BOOST_EN_PRE_EN_SHFT                                   0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_ADDR                                                  (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000002c)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_RMSK                                                         0x7
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_TX_BAND_MUX_BMSK                                             0x4
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_TX_BAND_MUX_SHFT                                             0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_TX_BAND_BMSK                                                 0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TX_BAND_TX_BAND_SHFT                                                 0x0

#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_ADDR                                                (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000030)
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_RMSK                                                       0xf
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_SLEW_CNTL_MUX_BMSK                                         0x8
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_SLEW_CNTL_MUX_SHFT                                         0x3
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_SLEW_CNTL_BMSK                                             0x7
#define HWIO_UFS_MEM_QSERDES_TX0_SLEW_CNTL_SLEW_CNTL_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000034)
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_RMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_ADDR, HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_TX_CLKDIV_PHASE_BMSK                               0x30
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_TX_CLKDIV_PHASE_SHFT                                0x4
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_TX_PWM_PHASE_BMSK                                   0xc
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_TX_PWM_PHASE_SHFT                                   0x2
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_SEL_20B_10B_BMSK                                    0x2
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_SEL_20B_10B_SHFT                                    0x1
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_SEL_10B_8B_BMSK                                     0x1
#define HWIO_UFS_MEM_QSERDES_TX0_INTERFACE_SELECT_SEL_10B_8B_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_ADDR                                                   (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000038)
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_RMSK                                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_PLACEHOLDER_CHAIN_INPUT_BMSK                                 0x80
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_PLACEHOLDER_CHAIN_INPUT_SHFT                                  0x7
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_LINE_LPB_EN_MUX_BMSK                                         0x40
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_LINE_LPB_EN_MUX_SHFT                                          0x6
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_LINE_LPB_EN_BMSK                                             0x20
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_LINE_LPB_EN_SHFT                                              0x5
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_TX_RCLK_LPB_EN_BMSK                                          0x10
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_TX_RCLK_LPB_EN_SHFT                                           0x4
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_SER_LPB_EN_MUX_BMSK                                           0x8
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_SER_LPB_EN_MUX_SHFT                                           0x3
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_SER_LPB_EN_BMSK                                               0x4
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_SER_LPB_EN_SHFT                                               0x2
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_PAR_LPB_EN_BMSK                                               0x2
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_PAR_LPB_EN_SHFT                                               0x1
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_RCLK_LPB_EN_BMSK                                              0x1
#define HWIO_UFS_MEM_QSERDES_TX0_LPB_EN_RCLK_LPB_EN_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000003c)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RES_CODE_LANE_TX_MUX_BMSK                          0x80
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RES_CODE_LANE_TX_MUX_SHFT                           0x7
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RES_CODE_LANE_TX_BMSK                              0x7f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_TX_RES_CODE_LANE_TX_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000040)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RMSK                                               0xff
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RES_CODE_LANE_RX_MUX_BMSK                          0x80
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RES_CODE_LANE_RX_MUX_SHFT                           0x7
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RES_CODE_LANE_RX_BMSK                              0x7f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_RX_RES_CODE_LANE_RX_SHFT                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_ADDR                                  (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000044)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_RMSK                                        0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_RES_CODE_LANE_OFFSET_TX_BMSK                0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX_RES_CODE_LANE_OFFSET_TX_SHFT                 0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_ADDR                                  (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000048)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_RMSK                                        0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_RES_CODE_LANE_OFFSET_RX_BMSK                0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_RES_CODE_LANE_OFFSET_RX_RES_CODE_LANE_OFFSET_RX_SHFT                 0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_ADDR                                             (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000004c)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_PERL_LENGTH1_BMSK                                      0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH1_PERL_LENGTH1_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_ADDR                                             (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000050)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_RMSK                                                   0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_PERL_LENGTH2_BMSK                                      0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PERL_LENGTH2_PERL_LENGTH2_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_ADDR                                        (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000054)
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_RMSK                                              0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_ADDR, HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_MUX_BMSK                           0x20
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_MUX_SHFT                            0x5
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_BMSK                               0x10
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_SHFT                                0x4
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_MUX_BMSK                         0x8
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_MUX_SHFT                         0x3
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_BMSK                             0x4
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_SHFT                             0x2
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_MUX_BMSK                          0x2
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_MUX_SHFT                          0x1
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_BMSK                              0x1
#define HWIO_UFS_MEM_QSERDES_TX0_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000058)
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_RMSK                                                   0xf
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_DEBUGBUS_SEL_BMSK                                      0xf
#define HWIO_UFS_MEM_QSERDES_TX0_DEBUG_BUS_SEL_DEBUGBUS_SEL_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_ADDR                                      (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000005c)
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_RMSK                                            0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TRANSCEIVER_BIAS_EN_MUX_BMSK                    0x20
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TRANSCEIVER_BIAS_EN_MUX_SHFT                     0x5
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TRANSCEIVER_BIAS_EN_BMSK                        0x10
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TRANSCEIVER_BIAS_EN_SHFT                         0x4
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE1_MUX_BMSK                             0x8
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE1_MUX_SHFT                             0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE1_BMSK                                 0x4
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE1_SHFT                                 0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE0_MUX_BMSK                             0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE0_MUX_SHFT                             0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE0_BMSK                                 0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TRANSCEIVER_BIAS_EN_TX_EN_LANE0_SHFT                                 0x0

#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000060)
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_RMSK                                                  0x1f
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_DRVR_EN_BMSK                                          0x10
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_DRVR_EN_SHFT                                           0x4
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE1_MUX_BMSK                                0x8
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE1_MUX_SHFT                                0x3
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE1_BMSK                                    0x4
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE1_SHFT                                    0x2
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE0_MUX_BMSK                                0x2
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE0_MUX_SHFT                                0x1
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE0_BMSK                                    0x1
#define HWIO_UFS_MEM_QSERDES_TX0_HIGHZ_DRVR_EN_TX_HIGHZ_LANE0_SHFT                                    0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000064)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_RMSK                                                      0xf
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE1_MUX_BMSK                                 0x8
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE1_MUX_SHFT                                 0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE1_BMSK                                     0x4
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE1_SHFT                                     0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE0_MUX_BMSK                                 0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE0_MUX_SHFT                                 0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE0_BMSK                                     0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TX_POL_INV_TX_POL_INV_LANE0_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_ADDR                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000068)
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RMSK                                     0x7f
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_MUX_BMSK                        0x40
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_MUX_SHFT                         0x6
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_BMSK                            0x30
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_SHFT                             0x4
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_MUX_BMSK                          0x8
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_MUX_SHFT                          0x3
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_BMSK                              0x4
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_SHFT                              0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_MUX_BMSK                       0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_MUX_SHFT                       0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_BMSK                           0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_SHFT                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000006c)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_BIST_PATTERN1_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN1_BIST_PATTERN1_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000070)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_BIST_PATTERN2_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN2_BIST_PATTERN2_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000074)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_BIST_PATTERN3_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN3_BIST_PATTERN3_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000078)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_BIST_PATTERN4_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN4_BIST_PATTERN4_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000007c)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_BIST_PATTERN5_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN5_BIST_PATTERN5_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000080)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_BIST_PATTERN6_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN6_BIST_PATTERN6_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000084)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_BIST_PATTERN7_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN7_BIST_PATTERN7_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_ADDR                                            (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000088)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_RMSK                                                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_BIST_PATTERN8_BMSK                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_PATTERN8_BIST_PATTERN8_SHFT                                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000008c)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_LANE_MODE1_BMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_LANE_MODE1_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000090)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_LANE_MODE2_BMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_2_LANE_MODE2_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000094)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_ADDR, HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_LANE_MODE3_BMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_3_LANE_MODE3_SHFT                                          0x0

#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ADDR                                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000098)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_RMSK                                                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ATB_SEL1_BMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL1_ATB_SEL1_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ADDR                                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x0000009c)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_RMSK                                                        0x1
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ATB_SEL2_BMSK                                               0x1
#define HWIO_UFS_MEM_QSERDES_TX0_ATB_SEL2_ATB_SEL2_SHFT                                               0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_ADDR                                           (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000a0)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RMSK                                                  0xf
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RCV_DET_LVL_MUX_BMSK                                  0x8
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RCV_DET_LVL_MUX_SHFT                                  0x3
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RCV_DET_LVL_BMSK                                      0x7
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_RCV_DET_LVL_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000a4)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RMSK                                               0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RCV_DET_LVL_2_BMSK                                 0x38
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RCV_DET_LVL_2_SHFT                                  0x3
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RCV_DET_LVL_1_BMSK                                  0x7
#define HWIO_UFS_MEM_QSERDES_TX0_RCV_DETECT_LVL_2_RCV_DET_LVL_1_SHFT                                  0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000a8)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_PRBS_SEED1_BMSK                                          0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED1_PRBS_SEED1_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000ac)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_PRBS_SEED2_BMSK                                          0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED2_PRBS_SEED2_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000b0)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_PRBS_SEED3_BMSK                                          0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED3_PRBS_SEED3_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_ADDR                                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000b4)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_RMSK                                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_PRBS_SEED4_BMSK                                          0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PRBS_SEED4_PRBS_SEED4_SHFT                                           0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_ADDR                                                (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000b8)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_RMSK                                                      0x7f
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_RESET_GEN_BMSK                                            0x7f
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_RESET_GEN_SHFT                                             0x0

#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_ADDR                                          (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000bc)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_RMSK                                                 0x3
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_ADDR, HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_TSYNC_MUX_BMSK                                       0x2
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_TSYNC_MUX_SHFT                                       0x1
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_RSYNC_MUX_BMSK                                       0x1
#define HWIO_UFS_MEM_QSERDES_TX0_RESET_GEN_MUXES_RSYNC_MUX_SHFT                                       0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_ADDR                                         (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000c0)
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_RMSK                                                0x3
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_EMP_EN_MUX_BMSK                                     0x2
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_EMP_EN_MUX_SHFT                                     0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_EMP_EN_BMSK                                         0x1
#define HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_EMP_EN_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_ADDR                                        (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000c4)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_RMSK                                               0xf
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_ADDR, HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_TXINTERFACE_MODE_BMSK                              0xf
#define HWIO_UFS_MEM_QSERDES_TX0_TX_INTERFACE_MODE_TXINTERFACE_MODE_SHFT                              0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_ADDR                                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000c8)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_RMSK                                                       0x3f
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_GEAR_MUX_BMSK                                       0x20
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_GEAR_MUX_SHFT                                        0x5
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_GEAR_BMSK                                           0x1c
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_GEAR_SHFT                                            0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_EN_MUX_BMSK                                          0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_EN_MUX_SHFT                                          0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_EN_BMSK                                              0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_CTRL_TX_PWM_EN_SHFT                                              0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_ADDR                                      (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000cc)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_RMSK                                             0x3
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_MUX_BMSK                            0x2
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_MUX_SHFT                            0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_BMSK                                0x1
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_SHFT                                0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_ADDR                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000d0)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_RMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_TX_PWM_DIVGEAR1_BMSK                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND2_TX_PWM_DIVGEAR1_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_ADDR                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000d4)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_RMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_TX_PWM_DIVGEAR2_BMSK                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND2_TX_PWM_DIVGEAR2_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_ADDR                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000d8)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_RMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_TX_PWM_DIVGEAR3_BMSK                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND2_TX_PWM_DIVGEAR3_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_ADDR                                 (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000dc)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_RMSK                                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_TX_PWM_DIVGEAR4_BMSK                       0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND2_TX_PWM_DIVGEAR4_SHFT                        0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_ADDR                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000e0)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_RMSK                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_TX_PWM_DIVGEAR1A_BMSK                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_TX_PWM_DIVGEAR1A_SHFT                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_ADDR                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000e4)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_RMSK                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_TX_PWM_DIVGEAR2A_BMSK                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_TX_PWM_DIVGEAR2A_SHFT                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_ADDR                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000e8)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_RMSK                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_TX_PWM_DIVGEAR3A_BMSK                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_TX_PWM_DIVGEAR3A_SHFT                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_ADDR                               (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000ec)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_RMSK                                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_TX_PWM_DIVGEAR4A_BMSK                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_TX_PWM_DIVGEAR4A_SHFT                     0x0

#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000f0)
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_RMSK                                                    0xff
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_VMODE_CTRL1_BMSK                                        0xff
#define HWIO_UFS_MEM_QSERDES_TX0_VMODE_CTRL1_VMODE_CTRL1_SHFT                                         0x0

#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ADDR                                     (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000f4)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_RMSK                                           0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_OUT(v)      \
        out_dword(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ADDR,v)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ADDR,m,v,HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_IN)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ALOG_OBSV_BUS_CTRL_1_SEL_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_CTRL_1_ALOG_OBSV_BUS_CTRL_1_SEL_SHFT                   0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_ADDR                                              (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000f8)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_RMSK                                                     0xf
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_PLACEHOLDER_CHAIN_OUTPUT_BMSK                            0x8
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_PLACEHOLDER_CHAIN_OUTPUT_SHFT                            0x3
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_BIST_STATUS_RO_BMSK                                      0x7
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_STATUS_BIST_STATUS_RO_SHFT                                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_ADDR                                        (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x000000fc)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_RMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_BIST_ERROR_COUNT1_STATUS_BMSK                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT1_BIST_ERROR_COUNT1_STATUS_SHFT                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_ADDR                                        (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000100)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_RMSK                                              0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_ADDR, HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_BIST_ERROR_COUNT2_STATUS_BMSK                     0xff
#define HWIO_UFS_MEM_QSERDES_TX0_BIST_ERROR_COUNT2_BIST_ERROR_COUNT2_STATUS_SHFT                      0x0

#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_ADDR                                   (UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX_REG_BASE      + 0x00000104)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_RMSK                                         0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_IN          \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_ADDR, HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_RMSK)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_ADDR, m)
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_ALOG_OBSV_BUS_STATUS_1_BMSK                  0xff
#define HWIO_UFS_MEM_QSERDES_TX0_ALOG_OBSV_BUS_STATUS_1_ALOG_OBSV_BUS_STATUS_1_SHFT                   0x0


#endif /* __UFS_PHY_HWIO_V2_H__ */
