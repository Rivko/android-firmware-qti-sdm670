#ifndef __PDCHAL_HWIO_H__
#define __PDCHAL_HWIO_H__
/*
===========================================================================
*/
/**
  @file pdcHal_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_PDC_DISPLAY_DISPLAY_PDC
    SDE_RSCC_RSCC_RSC


  Generation parameters: 
  { u'filename': u'pdcHal_hwio_670.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_PDC_DISPLAY_DISPLAY_PDC', u'SDE_RSCC_RSCC_RSC']}
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PdcTargetLib/inc/hwio/670/display/pdcHal_hwio.h#1 $
  $DateTime: 2017/09/20 22:09:48 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "msmhwiobase.h"
/*----------------------------------------------------------------------------
 * MODULE: RPMH_PDC_DISPLAY_DISPLAY_PDC
 *--------------------------------------------------------------------------*/

#define RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE                                                       (AOSS_BASE      + 0x00290000)

#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_ADDR(d)                                            (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00000000 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_RMSK                                               0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_MAXd                                                        0
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_ADDR(d),val)
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_ADDR(d),mask,val,HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_INI(d))
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_VOTE_CMP_DATA_BMSK                                 0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_VOTE_CMP_DATA_DRVd_VOTE_CMP_DATA_SHFT                                        0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_ADDR(d)                                              (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001000 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_RMSK                                                   0xffffff
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_MAXd                                                          0
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_MAJOR_VER_BMSK                                         0xff0000
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_MAJOR_VER_SHFT                                             0x10
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_MINOR_VER_BMSK                                           0xff00
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_MINOR_VER_SHFT                                              0x8
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_STEP_VER_BMSK                                              0xff
#define HWIO_RPMH_PDC_DISPLAY_PDC_VERSION_DRVd_STEP_VER_SHFT                                               0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_ADDR(d)                                       (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001004 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_RMSK                                            0xffffff
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_MAXd                                                   0
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TS_UNIT_BMSK                            0xff0000
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TS_UNIT_SHFT                                0x10
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_PROFILING_UNIT_BMSK                       0xf000
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_PROFILING_UNIT_SHFT                          0xc
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_BMSK                                   0xf00
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_SHFT                                     0x8
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_CMDS_BMSK                               0xe0
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_CMDS_SHFT                                0x5
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_DRV_BMSK                                    0x1f
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_DRV_SHFT                                     0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(d)                                     (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001008 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_RMSK                                          0xff0fff
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_MAXd                                                 0
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_SEQ_CMD_WORDS_BMSK                    0xff0000
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_SEQ_CMD_WORDS_SHFT                        0x10
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_AOP_IRQ_BMSK                             0xf00
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_AOP_IRQ_SHFT                               0x8
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_PWR_CTRL_BMSK                    0xf0
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_PWR_CTRL_SHFT                     0x4
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_WAIT_EVNT_BMSK                    0xf
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_WAIT_EVNT_SHFT                    0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(d)                                     (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x0000100c + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_RMSK                                         0x1ffffff
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_MAXd                                                 0
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_SEL_GP_IRQ_BMSK                      0x1ff0000
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_SEL_GP_IRQ_SHFT                           0x10
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_GP_IRQ_BMSK                             0xff00
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_GP_IRQ_SHFT                                0x8
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_IRQ_BMSK                                  0xff
#define HWIO_RPMH_PDC_DISPLAY_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_IRQ_SHFT                                   0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_ADDR(d)                                                 (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001020 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_RMSK                                                           0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_MAXd                                                             0
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_BUSY_STAT_BMSK                                                 0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BUSY_DRVd_BUSY_STAT_SHFT                                                 0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(d)                                  (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001024 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_RMSK                                           0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_MAXd                                              0
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_BMSK                        0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_ADDR(d)                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001028 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_RMSK                                                       0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_MAXd                                                         0
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_DBG_STEPPING_STATUS_BMSK                                   0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STAT_DRVd_DBG_STEPPING_STATUS_SHFT                                   0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(d)                                        (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x0000102c + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_RMSK                                           0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_MAXd                                                    0
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_SEQ_DBG_WAIT_EVNT_BMSK                         0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_WAIT_EVNT_DRVd_SEQ_DBG_WAIT_EVNT_SHFT                                0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_ADDR(d)                                          (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001030 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_RMSK                                                    0x1
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_MAXd                                                      0
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_PDC_MODE_STATUS_BMSK                                    0x1
#define HWIO_RPMH_PDC_DISPLAY_PDC_MODE_STATUS_DRVd_PDC_MODE_STATUS_SHFT                                    0x0

#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_ADDR(d)                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001034 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_RMSK                                                0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_MAXd                                                         0
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_EPCB_DEBUG_RX_ADDR_BMSK                             0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_ADDRd_EPCB_DEBUG_RX_ADDR_SHFT                                    0x0

#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_ADDR(d)                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001038 + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_RMSK                                                0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_MAXd                                                         0
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_EPCB_DEBUG_RX_DATA_BMSK                             0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_DBG_EPCB_RX_DATAd_EPCB_DEBUG_RX_DATA_SHFT                                    0x0

#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(d,pf)                               (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001200 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_RMSK                                          0x101
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_MAXd                                              0
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_MAXpf                                             4
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_INI2(d,pf)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(d,pf), HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_INMI2(d,pf,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(d,pf), mask)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_OVERFLOW_BMSK                       0x100
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_OVERFLOW_SHFT                         0x8
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_TS_VALID_BMSK                         0x1
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_TS_VALID_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(d,pf)                         (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001204 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_RMSK                               0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_MAXd                                        0
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_MAXpf                                       4
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_INI2(d,pf)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(d,pf), HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_INMI2(d,pf,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(d,pf), mask)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_PROFILING_TS_DATA_LO_BMSK          0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_PROFILING_TS_DATA_LO_SHFT                 0x0

#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(d,pf)                         (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001208 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_RMSK                                 0xffffff
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_MAXd                                        0
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_MAXpf                                       4
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_INI2(d,pf)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(d,pf), HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_INMI2(d,pf,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(d,pf), mask)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_PROFILING_TS_DATA_HI_BMSK            0xffffff
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_PROFILING_TS_DATA_HI_SHFT                 0x0

#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_ADDR(d,t)                                            (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001300 + 0xC8 * (t) + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_RMSK                                                        0x1
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_MAXd                                                          0
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_MAXt                                                          2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_INI2(d,t)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_ADDR(d,t), HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_INMI2(d,t,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_ADDR(d,t), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_CONTROLLER_IDLE_BMSK                                        0x1
#define HWIO_RPMH_PDC_DISPLAY_TCSt_DRVd_STATUS_CONTROLLER_IDLE_SHFT                                        0x0

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_ADDR(d,t,n)                                     (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00001304 + 0xC8 * (t) + 0x8 * (n) + 0x10000 * (d))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_RMSK                                               0x10101
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_MAXd                                                     0
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_MAXt                                                     2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_MAXn                                                     3
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_INI3(d,t,n)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_ADDR(d,t,n), HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_INMI3(d,t,n,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_ADDR(d,t,n), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_RESPONSE_RECD_BMSK                         0x10000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_RESPONSE_RECD_SHFT                            0x10
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_ISSUED_BMSK                                  0x100
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_ISSUED_SHFT                                    0x8
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_TRIGGERED_BMSK                                 0x1
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DRVd_STATUS_TCS_CMD_TRIGGERED_SHFT                                 0x0

#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ADDR                                                       (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004500)
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_RMSK                                                       0x80000001
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ADDR, HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_IN)
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_DISABLE_CLK_GATE_BMSK                                      0x80000000
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_DISABLE_CLK_GATE_SHFT                                            0x1f
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ENABLE_PDC_BMSK                                                   0x1
#define HWIO_RPMH_PDC_DISPLAY_ENABLE_PDC_ENABLE_PDC_SHFT                                                   0x0

#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_ADDR                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004510)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_RMSK                                             0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_ADDR, HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_IN)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_DATA_LO_BMSK                                     0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_LO_DATA_LO_SHFT                                            0x0

#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_ADDR                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004514)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_RMSK                                               0xffffff
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_ADDR, HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_IN)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_DATA_HI_BMSK                                       0xffffff
#define HWIO_RPMH_PDC_DISPLAY_TIMER_MATCH_VALUE_HI_DATA_HI_SHFT                                            0x0

#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_ADDR                                           (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004518)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_RMSK                                                  0x1
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_ADDR, HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_IN)
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_TIMER_DISABLED_BMSK                                   0x1
#define HWIO_RPMH_PDC_DISPLAY_TIMER_DISABLED_FOR_ARC_TIMER_DISABLED_SHFT                                   0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_ADDR                                                   (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004520)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_RMSK                                                         0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_SEQ_START_ADDR_BMSK                                          0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_START_ADDR_SEQ_START_ADDR_SHFT                                           0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_ADDR                                          (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004524)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_RMSK                                          0x8000007f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_VALID_BMSK                       0x80000000
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_VALID_SHFT                             0x1f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_START_ADDR_BMSK                        0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_START_ADDR_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_ADDR                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x0000452c)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_RMSK                                                    0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_SEQ_OVERRIDE_TRIGGER_BMSK                               0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_OVERRIDE_TRIGGER_SEQ_OVERRIDE_TRIGGER_SHFT                               0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR                                       (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004530)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_RMSK                                       0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_PWR_CTRL_OVERRIDE_MASK_BMSK                0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_MASK_PWR_CTRL_OVERRIDE_MASK_SHFT                       0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR                                        (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004534)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_RMSK                                        0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_PWR_CTRL_OVERRIDE_VAL_BMSK                  0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_PWR_CTRL_OVERRIDE_VAL_PWR_CTRL_OVERRIDE_VAL_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR                                     (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004540)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_RMSK                                     0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_WAIT_EVENT_OVERRIDE_MASK_BMSK            0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_MASK_WAIT_EVENT_OVERRIDE_MASK_SHFT                   0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR                                      (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004544)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_RMSK                                      0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_WAIT_EVENT_OVERRIDE_VAL_BMSK              0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_WAIT_EVENT_OVERRIDE_VAL_WAIT_EVENT_OVERRIDE_VAL_SHFT                     0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR                                       (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004550)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_RMSK                                       0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_BR_EVENT_OVERRIDE_MASK_BMSK                0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_MASK_BR_EVENT_OVERRIDE_MASK_SHFT                       0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR                                        (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004554)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_RMSK                                        0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_BR_EVENT_OVERRIDE_VAL_BMSK                  0xffffffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_BR_EVENT_OVERRIDE_VAL_BR_EVENT_OVERRIDE_VAL_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_ADDR(b)                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00004590 + 0x4 * (b))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_RMSK                                                      0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_MAXb                                                         3
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_INI(b)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_ADDR(b), HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_INMI(b,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_ADDR(b), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_OUTI(b,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_ADDR(b),val)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_OUTMI(b,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_ADDR(b),mask,val,HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_INI(b))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_BR_ADDR_BMSK                                              0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_BR_ADDR_b_BR_ADDR_SHFT                                               0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_ADDR(v)                                           (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x000045a0 + 0x4 * (v))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_RMSK                                                 0xfffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_MAXv                                                      15
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_INI(v)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_ADDR(v), HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_INMI(v,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_ADDR(v), mask)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_OUTI(v,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_ADDR(v),val)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_OUTMI(v,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_ADDR(v),mask,val,HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_INI(v))
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_DELAY_VAL_BMSK                                       0xfffff
#define HWIO_RPMH_PDC_DISPLAY_SEQ_CFG_DELAY_VAL_v_DELAY_VAL_SHFT                                           0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_ADDR                                          (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x000045e0)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_RMSK                                          0x8000007f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_BMSK                     0x80000000
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_SHFT                           0x1f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_BMSK                            0x7f
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_SHFT                             0x0

#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_ADDR                                                     (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x000045e4)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_RMSK                                                        0x10001
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_ADDR, HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_ADDR, m)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_ADDR,v)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_ADDR,m,v,HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_IN)
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_SEQ_DBG_STEP_TRIGGER_BMSK                                   0x10000
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_SEQ_DBG_STEP_TRIGGER_SHFT                                      0x10
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_SEQ_DBG_STEP_CONTINUE_BMSK                                      0x1
#define HWIO_RPMH_PDC_DISPLAY_SEQ_DBG_STEP_SEQ_DBG_STEP_CONTINUE_SHFT                                      0x0

#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_ADDR(pf)                                          (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005000 + 0x14 * (pf))
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_RMSK                                                     0x1
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_MAXpf                                                      4
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_INI(pf)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_ADDR(pf), HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_INMI(pf,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_ADDR(pf), mask)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_OUTI(pf,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_ADDR(pf),val)
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_OUTMI(pf,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_ADDR(pf),mask,val,HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_INI(pf))
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_PROFILING_UNIT_ENABLE_BMSK                               0x1
#define HWIO_RPMH_PDC_DISPLAY_PROFILING_UNITpf_EN_PROFILING_UNIT_ENABLE_SHFT                               0x0

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_ADDR(t)                                                  (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005500 + 0xC8 * (t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_RMSK                                                      0x1010000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_MAXt                                                              2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_INI(t)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_ADDR(t), HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_INMI(t,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_ADDR(t), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_OUTI(t,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_ADDR(t),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_ADDR(t),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_INI(t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_AMC_MODE_TRIGGER_BMSK                                     0x1000000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_AMC_MODE_TRIGGER_SHFT                                          0x18
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_AMC_MODE_EN_BMSK                                            0x10000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CONTROL_AMC_MODE_EN_SHFT                                               0x10

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_ADDR(t)                                          (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005504 + 0xC8 * (t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_RMSK                                                    0xf
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_MAXt                                                      2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_INI(t)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_ADDR(t), HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_INMI(t,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_ADDR(t), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_OUTI(t,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_ADDR(t),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_ADDR(t),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_INI(t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                         0xf
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_ENABLE_BANK_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                         0x0

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(t)                                   (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005508 + 0xC8 * (t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_RMSK                                             0xf
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_MAXt                                               2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_INI(t)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(t), HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_INMI(t,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(t), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_OUTI(t,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(t),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_OUTMI(t,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(t),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_INI(t))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_CMD_WAIT_FOR_CMPL_BANK_BMSK                      0xf
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMD_WAIT_FOR_CMPL_BANK_CMD_WAIT_FOR_CMPL_BANK_SHFT                      0x0

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_ADDR(t,n)                                             (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x0000550c + 0xC8 * (t) + 0x10 * (n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_RMSK                                                     0x10100
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_MAXt                                                           2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_MAXn                                                           3
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_INI2(t,n)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_ADDR(t,n), HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_INMI2(t,n,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_ADDR(t,n), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_OUTI2(t,n,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_ADDR(t,n),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_OUTMI2(t,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_ADDR(t,n),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_INI2(t,n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_READ_OR_WRITE_BMSK                                       0x10000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_READ_OR_WRITE_SHFT                                          0x10
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_RES_REQ_BMSK                                               0x100
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_MSGID_RES_REQ_SHFT                                                 0x8

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_ADDR(t,n)                                              (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005510 + 0xC8 * (t) + 0x10 * (n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_RMSK                                                      0x7fffc
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_MAXt                                                            2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_MAXn                                                            3
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_INI2(t,n)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_ADDR(t,n), HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_INMI2(t,n,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_ADDR(t,n), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_OUTI2(t,n,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_ADDR(t,n),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_OUTMI2(t,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_ADDR(t,n),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_INI2(t,n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_SLV_ID_BMSK                                               0x70000
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_SLV_ID_SHFT                                                  0x10
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_OFFSET_BMSK                                                0xfffc
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_ADDR_OFFSET_SHFT                                                   0x2

#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_ADDR(t,n)                                              (RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE      + 0x00005514 + 0xC8 * (t) + 0x10 * (n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_RMSK                                                       0x1fff
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_MAXt                                                            2
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_MAXn                                                            3
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_INI2(t,n)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_ADDR(t,n), HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_INMI2(t,n,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_ADDR(t,n), mask)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_OUTI2(t,n,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_ADDR(t,n),val)
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_OUTMI2(t,n,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_ADDR(t,n),mask,val,HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_INI2(t,n))
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_TCS_CMD_DATA_BMSK                                          0x1fff
#define HWIO_RPMH_PDC_DISPLAY_TCSt_CMDn_DATA_TCS_CMD_DATA_SHFT                                             0x0

/*----------------------------------------------------------------------------
 * MODULE: SDE_RSCC_RSCC_RSC
 *--------------------------------------------------------------------------*/

#define SDE_RSCC_RSCC_RSC_REG_BASE                                                                              (MDSS_BASE      + 0x00120000)


#endif /* __PDCHAL_HWIO_H__ */
