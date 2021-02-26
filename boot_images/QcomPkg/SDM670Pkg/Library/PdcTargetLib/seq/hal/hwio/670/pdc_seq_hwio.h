#ifndef __PDC_SEQ_HWIO_H__
#define __PDC_SEQ_HWIO_H__
/*
===========================================================================
*/
/**
  @file pdc_seq_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_PDC_MODEM_MODEM_PDC
    RPMH_PDC_MODEM_MODEM_PDC_SEQ_MEM
    MSS_RSCC_RSCC_RSC


  Generation parameters: 
  { u'explicit-addressing': True,
    u'filename': u'pdc_seq_hwio.h',
    u'ignore-prefixes': True,
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [ u'RPMH_PDC_MODEM_MODEM_PDC',
                  u'RPMH_PDC_MODEM_MODEM_PDC_SEQ_MEM',
                  u'MSS_RSCC_RSCC_RSC']}
				  
   **************************------------------------*****************************				  
   For "HWIO_SEQ_MEM_m_ADDR" register offset is manually changed from 0x0000000 to 
   0x00200000 as the base address is passed from other module.   
   **************************------------------------*****************************		
	
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PdcTargetLib/seq/hal/hwio/670/pdc_seq_hwio.h#1 $
  $DateTime: 2017/09/20 22:09:48 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MODEM_PDC
 *--------------------------------------------------------------------------*/

#define MODEM_PDC_REG_BASE                                                                        (AOSS_BASE      + 0x002b0000)

#define HWIO_VOTE_CMP_DATA_DRVd_ADDR(base,d)                                                      ((base) + 0x00000000 + 0x10000 * (d))
#define HWIO_VOTE_CMP_DATA_DRVd_RMSK                                                              0xffffffff
#define HWIO_VOTE_CMP_DATA_DRVd_MAXd                                                                       1
#define HWIO_VOTE_CMP_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_VOTE_CMP_DATA_DRVd_ADDR(base,d), HWIO_VOTE_CMP_DATA_DRVd_RMSK)
#define HWIO_VOTE_CMP_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_VOTE_CMP_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_VOTE_CMP_DATA_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_VOTE_CMP_DATA_DRVd_ADDR(base,d),val)
#define HWIO_VOTE_CMP_DATA_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_VOTE_CMP_DATA_DRVd_ADDR(base,d),mask,val,HWIO_VOTE_CMP_DATA_DRVd_INI(base,d))
#define HWIO_VOTE_CMP_DATA_DRVd_VOTE_CMP_DATA_BMSK                                                0xffffffff
#define HWIO_VOTE_CMP_DATA_DRVd_VOTE_CMP_DATA_SHFT                                                       0x0

#define HWIO_IRQ_ENABLE_BANKb_DRVd_ADDR(base,d,b)                                                 ((base) + 0x00000010 + 0x10000 * (d) + 0x4 * (b))
#define HWIO_IRQ_ENABLE_BANKb_DRVd_RMSK                                                           0xffffffff
#define HWIO_IRQ_ENABLE_BANKb_DRVd_MAXd                                                                    1
#define HWIO_IRQ_ENABLE_BANKb_DRVd_MAXb                                                                    1
#define HWIO_IRQ_ENABLE_BANKb_DRVd_INI2(base,d,b)        \
        in_dword_masked(HWIO_IRQ_ENABLE_BANKb_DRVd_ADDR(base,d,b), HWIO_IRQ_ENABLE_BANKb_DRVd_RMSK)
#define HWIO_IRQ_ENABLE_BANKb_DRVd_INMI2(base,d,b,mask)    \
        in_dword_masked(HWIO_IRQ_ENABLE_BANKb_DRVd_ADDR(base,d,b), mask)
#define HWIO_IRQ_ENABLE_BANKb_DRVd_OUTI2(base,d,b,val)    \
        out_dword(HWIO_IRQ_ENABLE_BANKb_DRVd_ADDR(base,d,b),val)
#define HWIO_IRQ_ENABLE_BANKb_DRVd_OUTMI2(base,d,b,mask,val) \
        out_dword_masked_ns(HWIO_IRQ_ENABLE_BANKb_DRVd_ADDR(base,d,b),mask,val,HWIO_IRQ_ENABLE_BANKb_DRVd_INI2(base,d,b))
#define HWIO_IRQ_ENABLE_BANKb_DRVd_IRQ_ENABLE_BANK_BMSK                                           0xffffffff
#define HWIO_IRQ_ENABLE_BANKb_DRVd_IRQ_ENABLE_BANK_SHFT                                                  0x0

#define HWIO_IRQ_i_CFG_DRVd_ADDR(base,d,i)                                                        ((base) + 0x00000110 + 0x10000 * (d) + 0x4 * (i))
#define HWIO_IRQ_i_CFG_DRVd_RMSK                                                                         0x7
#define HWIO_IRQ_i_CFG_DRVd_MAXd                                                                           1
#define HWIO_IRQ_i_CFG_DRVd_MAXi                                                                          44
#define HWIO_IRQ_i_CFG_DRVd_INI2(base,d,i)        \
        in_dword_masked(HWIO_IRQ_i_CFG_DRVd_ADDR(base,d,i), HWIO_IRQ_i_CFG_DRVd_RMSK)
#define HWIO_IRQ_i_CFG_DRVd_INMI2(base,d,i,mask)    \
        in_dword_masked(HWIO_IRQ_i_CFG_DRVd_ADDR(base,d,i), mask)
#define HWIO_IRQ_i_CFG_DRVd_OUTI2(base,d,i,val)    \
        out_dword(HWIO_IRQ_i_CFG_DRVd_ADDR(base,d,i),val)
#define HWIO_IRQ_i_CFG_DRVd_OUTMI2(base,d,i,mask,val) \
        out_dword_masked_ns(HWIO_IRQ_i_CFG_DRVd_ADDR(base,d,i),mask,val,HWIO_IRQ_i_CFG_DRVd_INI2(base,d,i))
#define HWIO_IRQ_i_CFG_DRVd_IRQ_CFG_BMSK                                                                 0x7
#define HWIO_IRQ_i_CFG_DRVd_IRQ_CFG_SHFT                                                                 0x0

#define HWIO_IRQ_STATUS_BANKb_DRVd_ADDR(base,d,b)                                                 ((base) + 0x00000900 + 0x10000 * (d) + 0x4 * (b))
#define HWIO_IRQ_STATUS_BANKb_DRVd_RMSK                                                           0xffffffff
#define HWIO_IRQ_STATUS_BANKb_DRVd_MAXd                                                                    1
#define HWIO_IRQ_STATUS_BANKb_DRVd_MAXb                                                                    1
#define HWIO_IRQ_STATUS_BANKb_DRVd_INI2(base,d,b)        \
        in_dword_masked(HWIO_IRQ_STATUS_BANKb_DRVd_ADDR(base,d,b), HWIO_IRQ_STATUS_BANKb_DRVd_RMSK)
#define HWIO_IRQ_STATUS_BANKb_DRVd_INMI2(base,d,b,mask)    \
        in_dword_masked(HWIO_IRQ_STATUS_BANKb_DRVd_ADDR(base,d,b), mask)
#define HWIO_IRQ_STATUS_BANKb_DRVd_IRQ_STATUS_BANK_BMSK                                           0xffffffff
#define HWIO_IRQ_STATUS_BANKb_DRVd_IRQ_STATUS_BANK_SHFT                                                  0x0

#define HWIO_PDC_VERSION_DRVd_ADDR(base,d)                                                        ((base) + 0x00001000 + 0x10000 * (d))
#define HWIO_PDC_VERSION_DRVd_RMSK                                                                  0xffffff
#define HWIO_PDC_VERSION_DRVd_MAXd                                                                         1
#define HWIO_PDC_VERSION_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_VERSION_DRVd_ADDR(base,d), HWIO_PDC_VERSION_DRVd_RMSK)
#define HWIO_PDC_VERSION_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_VERSION_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_VERSION_DRVd_MAJOR_VER_BMSK                                                        0xff0000
#define HWIO_PDC_VERSION_DRVd_MAJOR_VER_SHFT                                                            0x10
#define HWIO_PDC_VERSION_DRVd_MINOR_VER_BMSK                                                          0xff00
#define HWIO_PDC_VERSION_DRVd_MINOR_VER_SHFT                                                             0x8
#define HWIO_PDC_VERSION_DRVd_STEP_VER_BMSK                                                             0xff
#define HWIO_PDC_VERSION_DRVd_STEP_VER_SHFT                                                              0x0

#define HWIO_PDC_PARAM_RESOURCE_DRVd_ADDR(base,d)                                                 ((base) + 0x00001004 + 0x10000 * (d))
#define HWIO_PDC_PARAM_RESOURCE_DRVd_RMSK                                                           0xffffff
#define HWIO_PDC_PARAM_RESOURCE_DRVd_MAXd                                                                  1
#define HWIO_PDC_PARAM_RESOURCE_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_PARAM_RESOURCE_DRVd_ADDR(base,d), HWIO_PDC_PARAM_RESOURCE_DRVd_RMSK)
#define HWIO_PDC_PARAM_RESOURCE_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_PARAM_RESOURCE_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TS_UNIT_BMSK                                           0xff0000
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TS_UNIT_SHFT                                               0x10
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_PROFILING_UNIT_BMSK                                      0xf000
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_PROFILING_UNIT_SHFT                                         0xc
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_BMSK                                                  0xf00
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_SHFT                                                    0x8
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_CMDS_BMSK                                              0xe0
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_TCS_CMDS_SHFT                                               0x5
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_DRV_BMSK                                                   0x1f
#define HWIO_PDC_PARAM_RESOURCE_DRVd_BLK_NUM_DRV_SHFT                                                    0x0

#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(base,d)                                               ((base) + 0x00001008 + 0x10000 * (d))
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_RMSK                                                         0xff0fff
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_MAXd                                                                1
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(base,d), HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_RMSK)
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_SEQ_CMD_WORDS_BMSK                                   0xff0000
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_SEQ_CMD_WORDS_SHFT                                       0x10
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_AOP_IRQ_BMSK                                            0xf00
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_AOP_IRQ_SHFT                                              0x8
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_PWR_CTRL_BMSK                                   0xf0
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_PWR_CTRL_SHFT                                    0x4
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_WAIT_EVNT_BMSK                                   0xf
#define HWIO_PDC_PARAM_SEQ_CONFIG_DRVd_BLK_NUM_ADDL_SEQ_WAIT_EVNT_SHFT                                   0x0

#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(base,d)                                               ((base) + 0x0000100c + 0x10000 * (d))
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_RMSK                                                        0x1ffffff
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_MAXd                                                                1
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(base,d), HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_RMSK)
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_SEL_GP_IRQ_BMSK                                     0x1ff0000
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_SEL_GP_IRQ_SHFT                                          0x10
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_GP_IRQ_BMSK                                            0xff00
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_GP_IRQ_SHFT                                               0x8
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_IRQ_BMSK                                                 0xff
#define HWIO_PDC_PARAM_IRQ_CONFIG_DRVd_BLK_NUM_IRQ_SHFT                                                  0x0

#define HWIO_SEQ_BUSY_DRVd_ADDR(base,d)                                                           ((base) + 0x00001020 + 0x10000 * (d))
#define HWIO_SEQ_BUSY_DRVd_RMSK                                                                          0x1
#define HWIO_SEQ_BUSY_DRVd_MAXd                                                                            1
#define HWIO_SEQ_BUSY_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_SEQ_BUSY_DRVd_ADDR(base,d), HWIO_SEQ_BUSY_DRVd_RMSK)
#define HWIO_SEQ_BUSY_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_SEQ_BUSY_DRVd_ADDR(base,d), mask)
#define HWIO_SEQ_BUSY_DRVd_BUSY_STAT_BMSK                                                                0x1
#define HWIO_SEQ_BUSY_DRVd_BUSY_STAT_SHFT                                                                0x0

#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(base,d)                                            ((base) + 0x00001024 + 0x10000 * (d))
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_RMSK                                                          0x7f
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_MAXd                                                             1
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(base,d), HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_RMSK)
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_ADDR(base,d), mask)
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_BMSK                                       0x7f
#define HWIO_SEQ_DBG_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_SHFT                                        0x0

#define HWIO_SEQ_DBG_STAT_DRVd_ADDR(base,d)                                                       ((base) + 0x00001028 + 0x10000 * (d))
#define HWIO_SEQ_DBG_STAT_DRVd_RMSK                                                                      0x1
#define HWIO_SEQ_DBG_STAT_DRVd_MAXd                                                                        1
#define HWIO_SEQ_DBG_STAT_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_SEQ_DBG_STAT_DRVd_ADDR(base,d), HWIO_SEQ_DBG_STAT_DRVd_RMSK)
#define HWIO_SEQ_DBG_STAT_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_SEQ_DBG_STAT_DRVd_ADDR(base,d), mask)
#define HWIO_SEQ_DBG_STAT_DRVd_DBG_STEPPING_STATUS_BMSK                                                  0x1
#define HWIO_SEQ_DBG_STAT_DRVd_DBG_STEPPING_STATUS_SHFT                                                  0x0

#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(base,d)                                                  ((base) + 0x0000102c + 0x10000 * (d))
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_RMSK                                                          0xffffffff
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_MAXd                                                                   1
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(base,d), HWIO_SEQ_DBG_WAIT_EVNT_DRVd_RMSK)
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_SEQ_DBG_WAIT_EVNT_DRVd_ADDR(base,d), mask)
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_SEQ_DBG_WAIT_EVNT_BMSK                                        0xffffffff
#define HWIO_SEQ_DBG_WAIT_EVNT_DRVd_SEQ_DBG_WAIT_EVNT_SHFT                                               0x0

#define HWIO_PDC_MODE_STATUS_DRVd_ADDR(base,d)                                                    ((base) + 0x00001030 + 0x10000 * (d))
#define HWIO_PDC_MODE_STATUS_DRVd_RMSK                                                                   0x1
#define HWIO_PDC_MODE_STATUS_DRVd_MAXd                                                                     1
#define HWIO_PDC_MODE_STATUS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_MODE_STATUS_DRVd_ADDR(base,d), HWIO_PDC_MODE_STATUS_DRVd_RMSK)
#define HWIO_PDC_MODE_STATUS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_MODE_STATUS_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_MODE_STATUS_DRVd_PDC_MODE_STATUS_BMSK                                                   0x1
#define HWIO_PDC_MODE_STATUS_DRVd_PDC_MODE_STATUS_SHFT                                                   0x0

#define HWIO_DBG_EPCB_RX_ADDRd_ADDR(base,d)                                                       ((base) + 0x00001034 + 0x10000 * (d))
#define HWIO_DBG_EPCB_RX_ADDRd_RMSK                                                               0xffffffff
#define HWIO_DBG_EPCB_RX_ADDRd_MAXd                                                                        1
#define HWIO_DBG_EPCB_RX_ADDRd_INI(base,d)        \
        in_dword_masked(HWIO_DBG_EPCB_RX_ADDRd_ADDR(base,d), HWIO_DBG_EPCB_RX_ADDRd_RMSK)
#define HWIO_DBG_EPCB_RX_ADDRd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_DBG_EPCB_RX_ADDRd_ADDR(base,d), mask)
#define HWIO_DBG_EPCB_RX_ADDRd_EPCB_DEBUG_RX_ADDR_BMSK                                            0xffffffff
#define HWIO_DBG_EPCB_RX_ADDRd_EPCB_DEBUG_RX_ADDR_SHFT                                                   0x0

#define HWIO_DBG_EPCB_RX_DATAd_ADDR(base,d)                                                       ((base) + 0x00001038 + 0x10000 * (d))
#define HWIO_DBG_EPCB_RX_DATAd_RMSK                                                               0xffffffff
#define HWIO_DBG_EPCB_RX_DATAd_MAXd                                                                        1
#define HWIO_DBG_EPCB_RX_DATAd_INI(base,d)        \
        in_dword_masked(HWIO_DBG_EPCB_RX_DATAd_ADDR(base,d), HWIO_DBG_EPCB_RX_DATAd_RMSK)
#define HWIO_DBG_EPCB_RX_DATAd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_DBG_EPCB_RX_DATAd_ADDR(base,d), mask)
#define HWIO_DBG_EPCB_RX_DATAd_EPCB_DEBUG_RX_DATA_BMSK                                            0xffffffff
#define HWIO_DBG_EPCB_RX_DATAd_EPCB_DEBUG_RX_DATA_SHFT                                                   0x0

#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(base,d,pf)                                         ((base) + 0x00001200 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_RMSK                                                         0x101
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_MAXd                                                             1
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_MAXpf                                                            4
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_INI2(base,d,pf)        \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(base,d,pf), HWIO_PROFILING_UNITpf_DRVd_OUTPUT_RMSK)
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_INMI2(base,d,pf,mask)    \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_OUTPUT_ADDR(base,d,pf), mask)
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_OVERFLOW_BMSK                                      0x100
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_OVERFLOW_SHFT                                        0x8
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_TS_VALID_BMSK                                        0x1
#define HWIO_PROFILING_UNITpf_DRVd_OUTPUT_PROFILING_TS_VALID_SHFT                                        0x0

#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(base,d,pf)                                   ((base) + 0x00001204 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_RMSK                                              0xffffffff
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_MAXd                                                       1
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_MAXpf                                                      4
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_INI2(base,d,pf)        \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(base,d,pf), HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_RMSK)
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_INMI2(base,d,pf,mask)    \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_ADDR(base,d,pf), mask)
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_PROFILING_TS_DATA_LO_BMSK                         0xffffffff
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_LO_PROFILING_TS_DATA_LO_SHFT                                0x0

#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(base,d,pf)                                   ((base) + 0x00001208 + 0x14 * (pf) + 0x10000 * (d))
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_RMSK                                                0xffffff
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_MAXd                                                       1
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_MAXpf                                                      4
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_INI2(base,d,pf)        \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(base,d,pf), HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_RMSK)
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_INMI2(base,d,pf,mask)    \
        in_dword_masked(HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_ADDR(base,d,pf), mask)
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_PROFILING_TS_DATA_HI_BMSK                           0xffffff
#define HWIO_PROFILING_UNITpf_DRVd_TIMESTAMP_HI_PROFILING_TS_DATA_HI_SHFT                                0x0

#define HWIO_TCSt_DRVd_STATUS_ADDR(base,d,t)                                                      ((base) + 0x00001300 + 0xC8 * (t) + 0x10000 * (d))
#define HWIO_TCSt_DRVd_STATUS_RMSK                                                                       0x1
#define HWIO_TCSt_DRVd_STATUS_MAXd                                                                         1
#define HWIO_TCSt_DRVd_STATUS_MAXt                                                                         2
#define HWIO_TCSt_DRVd_STATUS_INI2(base,d,t)        \
        in_dword_masked(HWIO_TCSt_DRVd_STATUS_ADDR(base,d,t), HWIO_TCSt_DRVd_STATUS_RMSK)
#define HWIO_TCSt_DRVd_STATUS_INMI2(base,d,t,mask)    \
        in_dword_masked(HWIO_TCSt_DRVd_STATUS_ADDR(base,d,t), mask)
#define HWIO_TCSt_DRVd_STATUS_CONTROLLER_IDLE_BMSK                                                       0x1
#define HWIO_TCSt_DRVd_STATUS_CONTROLLER_IDLE_SHFT                                                       0x0

#define HWIO_TCSt_CMDn_DRVd_STATUS_ADDR(base,d,t,n)                                               ((base) + 0x00001304 + 0xC8 * (t) + 0x8 * (n) + 0x10000 * (d))
#define HWIO_TCSt_CMDn_DRVd_STATUS_RMSK                                                              0x10101
#define HWIO_TCSt_CMDn_DRVd_STATUS_MAXd                                                                    1
#define HWIO_TCSt_CMDn_DRVd_STATUS_MAXt                                                                    2
#define HWIO_TCSt_CMDn_DRVd_STATUS_MAXn                                                                    3
#define HWIO_TCSt_CMDn_DRVd_STATUS_INI3(base,d,t,n)        \
        in_dword_masked(HWIO_TCSt_CMDn_DRVd_STATUS_ADDR(base,d,t,n), HWIO_TCSt_CMDn_DRVd_STATUS_RMSK)
#define HWIO_TCSt_CMDn_DRVd_STATUS_INMI3(base,d,t,n,mask)    \
        in_dword_masked(HWIO_TCSt_CMDn_DRVd_STATUS_ADDR(base,d,t,n), mask)
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_RESPONSE_RECD_BMSK                                        0x10000
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_RESPONSE_RECD_SHFT                                           0x10
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_ISSUED_BMSK                                                 0x100
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_ISSUED_SHFT                                                   0x8
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_TRIGGERED_BMSK                                                0x1
#define HWIO_TCSt_CMDn_DRVd_STATUS_TCS_CMD_TRIGGERED_SHFT                                                0x0

#define HWIO_ENABLE_PDC_ADDR(x)                                                                   ((x) + 0x00004500)
#define HWIO_ENABLE_PDC_RMSK                                                                      0x80000001
#define HWIO_ENABLE_PDC_IN(x)      \
        in_dword_masked(HWIO_ENABLE_PDC_ADDR(x), HWIO_ENABLE_PDC_RMSK)
#define HWIO_ENABLE_PDC_INM(x, m)      \
        in_dword_masked(HWIO_ENABLE_PDC_ADDR(x), m)
#define HWIO_ENABLE_PDC_OUT(x, v)      \
        out_dword(HWIO_ENABLE_PDC_ADDR(x),v)
#define HWIO_ENABLE_PDC_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ENABLE_PDC_ADDR(x),m,v,HWIO_ENABLE_PDC_IN(x))
#define HWIO_ENABLE_PDC_DISABLE_CLK_GATE_BMSK                                                     0x80000000
#define HWIO_ENABLE_PDC_DISABLE_CLK_GATE_SHFT                                                           0x1f
#define HWIO_ENABLE_PDC_ENABLE_PDC_BMSK                                                                  0x1
#define HWIO_ENABLE_PDC_ENABLE_PDC_SHFT                                                                  0x0

#define HWIO_TIMER_MATCH_VALUE_LO_ADDR(x)                                                         ((x) + 0x00004510)
#define HWIO_TIMER_MATCH_VALUE_LO_RMSK                                                            0xffffffff
#define HWIO_TIMER_MATCH_VALUE_LO_IN(x)      \
        in_dword_masked(HWIO_TIMER_MATCH_VALUE_LO_ADDR(x), HWIO_TIMER_MATCH_VALUE_LO_RMSK)
#define HWIO_TIMER_MATCH_VALUE_LO_INM(x, m)      \
        in_dword_masked(HWIO_TIMER_MATCH_VALUE_LO_ADDR(x), m)
#define HWIO_TIMER_MATCH_VALUE_LO_OUT(x, v)      \
        out_dword(HWIO_TIMER_MATCH_VALUE_LO_ADDR(x),v)
#define HWIO_TIMER_MATCH_VALUE_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TIMER_MATCH_VALUE_LO_ADDR(x),m,v,HWIO_TIMER_MATCH_VALUE_LO_IN(x))
#define HWIO_TIMER_MATCH_VALUE_LO_DATA_LO_BMSK                                                    0xffffffff
#define HWIO_TIMER_MATCH_VALUE_LO_DATA_LO_SHFT                                                           0x0

#define HWIO_TIMER_MATCH_VALUE_HI_ADDR(x)                                                         ((x) + 0x00004514)
#define HWIO_TIMER_MATCH_VALUE_HI_RMSK                                                              0xffffff
#define HWIO_TIMER_MATCH_VALUE_HI_IN(x)      \
        in_dword_masked(HWIO_TIMER_MATCH_VALUE_HI_ADDR(x), HWIO_TIMER_MATCH_VALUE_HI_RMSK)
#define HWIO_TIMER_MATCH_VALUE_HI_INM(x, m)      \
        in_dword_masked(HWIO_TIMER_MATCH_VALUE_HI_ADDR(x), m)
#define HWIO_TIMER_MATCH_VALUE_HI_OUT(x, v)      \
        out_dword(HWIO_TIMER_MATCH_VALUE_HI_ADDR(x),v)
#define HWIO_TIMER_MATCH_VALUE_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TIMER_MATCH_VALUE_HI_ADDR(x),m,v,HWIO_TIMER_MATCH_VALUE_HI_IN(x))
#define HWIO_TIMER_MATCH_VALUE_HI_DATA_HI_BMSK                                                      0xffffff
#define HWIO_TIMER_MATCH_VALUE_HI_DATA_HI_SHFT                                                           0x0

#define HWIO_TIMER_DISABLED_FOR_ARC_ADDR(x)                                                       ((x) + 0x00004518)
#define HWIO_TIMER_DISABLED_FOR_ARC_RMSK                                                                 0x1
#define HWIO_TIMER_DISABLED_FOR_ARC_IN(x)      \
        in_dword_masked(HWIO_TIMER_DISABLED_FOR_ARC_ADDR(x), HWIO_TIMER_DISABLED_FOR_ARC_RMSK)
#define HWIO_TIMER_DISABLED_FOR_ARC_INM(x, m)      \
        in_dword_masked(HWIO_TIMER_DISABLED_FOR_ARC_ADDR(x), m)
#define HWIO_TIMER_DISABLED_FOR_ARC_OUT(x, v)      \
        out_dword(HWIO_TIMER_DISABLED_FOR_ARC_ADDR(x),v)
#define HWIO_TIMER_DISABLED_FOR_ARC_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TIMER_DISABLED_FOR_ARC_ADDR(x),m,v,HWIO_TIMER_DISABLED_FOR_ARC_IN(x))
#define HWIO_TIMER_DISABLED_FOR_ARC_TIMER_DISABLED_BMSK                                                  0x1
#define HWIO_TIMER_DISABLED_FOR_ARC_TIMER_DISABLED_SHFT                                                  0x0

#define HWIO_SEQ_START_ADDR_ADDR(x)                                                               ((x) + 0x00004520)
#define HWIO_SEQ_START_ADDR_RMSK                                                                        0x7f
#define HWIO_SEQ_START_ADDR_IN(x)      \
        in_dword_masked(HWIO_SEQ_START_ADDR_ADDR(x), HWIO_SEQ_START_ADDR_RMSK)
#define HWIO_SEQ_START_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_START_ADDR_ADDR(x), m)
#define HWIO_SEQ_START_ADDR_OUT(x, v)      \
        out_dword(HWIO_SEQ_START_ADDR_ADDR(x),v)
#define HWIO_SEQ_START_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_START_ADDR_ADDR(x),m,v,HWIO_SEQ_START_ADDR_IN(x))
#define HWIO_SEQ_START_ADDR_SEQ_START_ADDR_BMSK                                                         0x7f
#define HWIO_SEQ_START_ADDR_SEQ_START_ADDR_SHFT                                                          0x0

#define HWIO_SEQ_OVERRIDE_START_ADDR_ADDR(x)                                                      ((x) + 0x00004524)
#define HWIO_SEQ_OVERRIDE_START_ADDR_RMSK                                                         0x8000007f
#define HWIO_SEQ_OVERRIDE_START_ADDR_IN(x)      \
        in_dword_masked(HWIO_SEQ_OVERRIDE_START_ADDR_ADDR(x), HWIO_SEQ_OVERRIDE_START_ADDR_RMSK)
#define HWIO_SEQ_OVERRIDE_START_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_OVERRIDE_START_ADDR_ADDR(x), m)
#define HWIO_SEQ_OVERRIDE_START_ADDR_OUT(x, v)      \
        out_dword(HWIO_SEQ_OVERRIDE_START_ADDR_ADDR(x),v)
#define HWIO_SEQ_OVERRIDE_START_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_OVERRIDE_START_ADDR_ADDR(x),m,v,HWIO_SEQ_OVERRIDE_START_ADDR_IN(x))
#define HWIO_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_VALID_BMSK                                      0x80000000
#define HWIO_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_VALID_SHFT                                            0x1f
#define HWIO_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_START_ADDR_BMSK                                       0x7f
#define HWIO_SEQ_OVERRIDE_START_ADDR_SEQ_OVERRIDE_START_ADDR_SHFT                                        0x0

#define HWIO_SEQ_OVERRIDE_TRIGGER_ADDR(x)                                                         ((x) + 0x0000452c)
#define HWIO_SEQ_OVERRIDE_TRIGGER_RMSK                                                                   0x1
#define HWIO_SEQ_OVERRIDE_TRIGGER_IN(x)      \
        in_dword_masked(HWIO_SEQ_OVERRIDE_TRIGGER_ADDR(x), HWIO_SEQ_OVERRIDE_TRIGGER_RMSK)
#define HWIO_SEQ_OVERRIDE_TRIGGER_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_OVERRIDE_TRIGGER_ADDR(x), m)
#define HWIO_SEQ_OVERRIDE_TRIGGER_OUT(x, v)      \
        out_dword(HWIO_SEQ_OVERRIDE_TRIGGER_ADDR(x),v)
#define HWIO_SEQ_OVERRIDE_TRIGGER_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_OVERRIDE_TRIGGER_ADDR(x),m,v,HWIO_SEQ_OVERRIDE_TRIGGER_IN(x))
#define HWIO_SEQ_OVERRIDE_TRIGGER_SEQ_OVERRIDE_TRIGGER_BMSK                                              0x1
#define HWIO_SEQ_OVERRIDE_TRIGGER_SEQ_OVERRIDE_TRIGGER_SHFT                                              0x0

#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR(x)                                                   ((x) + 0x00004530)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_RMSK                                                      0xffffffff
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_IN(x)      \
        in_dword_masked(HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR(x), HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_RMSK)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR(x), m)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_OUT(x, v)      \
        out_dword(HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR(x),v)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_ADDR(x),m,v,HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_IN(x))
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_PWR_CTRL_OVERRIDE_MASK_BMSK                               0xffffffff
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_MASK_PWR_CTRL_OVERRIDE_MASK_SHFT                                      0x0

#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR(x)                                                    ((x) + 0x00004534)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_RMSK                                                       0xffffffff
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_IN(x)      \
        in_dword_masked(HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR(x), HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_RMSK)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR(x), m)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_OUT(x, v)      \
        out_dword(HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR(x),v)
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_ADDR(x),m,v,HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_IN(x))
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_PWR_CTRL_OVERRIDE_VAL_BMSK                                 0xffffffff
#define HWIO_SEQ_PWR_CTRL_OVERRIDE_VAL_PWR_CTRL_OVERRIDE_VAL_SHFT                                        0x0

#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR(x)                                                 ((x) + 0x00004540)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_RMSK                                                    0xffffffff
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_IN(x)      \
        in_dword_masked(HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR(x), HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR(x), m)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_OUT(x, v)      \
        out_dword(HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR(x),v)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_ADDR(x),m,v,HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_IN(x))
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_WAIT_EVENT_OVERRIDE_MASK_BMSK                           0xffffffff
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_MASK_WAIT_EVENT_OVERRIDE_MASK_SHFT                                  0x0

#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR(x)                                                  ((x) + 0x00004544)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_RMSK                                                     0xffffffff
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_IN(x)      \
        in_dword_masked(HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR(x), HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR(x), m)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_OUT(x, v)      \
        out_dword(HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR(x),v)
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_ADDR(x),m,v,HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_IN(x))
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_WAIT_EVENT_OVERRIDE_VAL_BMSK                             0xffffffff
#define HWIO_SEQ_WAIT_EVENT_OVERRIDE_VAL_WAIT_EVENT_OVERRIDE_VAL_SHFT                                    0x0

#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR(x)                                                   ((x) + 0x00004550)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_RMSK                                                      0xffffffff
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_IN(x)      \
        in_dword_masked(HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR(x), HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_RMSK)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR(x), m)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_OUT(x, v)      \
        out_dword(HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR(x),v)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_ADDR(x),m,v,HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_IN(x))
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_BR_EVENT_OVERRIDE_MASK_BMSK                               0xffffffff
#define HWIO_SEQ_BR_EVENT_OVERRIDE_MASK_BR_EVENT_OVERRIDE_MASK_SHFT                                      0x0

#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR(x)                                                    ((x) + 0x00004554)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_RMSK                                                       0xffffffff
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_IN(x)      \
        in_dword_masked(HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR(x), HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_RMSK)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR(x), m)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_OUT(x, v)      \
        out_dword(HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR(x),v)
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_ADDR(x),m,v,HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_IN(x))
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_BR_EVENT_OVERRIDE_VAL_BMSK                                 0xffffffff
#define HWIO_SEQ_BR_EVENT_OVERRIDE_VAL_BR_EVENT_OVERRIDE_VAL_SHFT                                        0x0

#define HWIO_SEQ_CFG_BR_ADDR_b_ADDR(base,b)                                                       ((base) + 0x00004590 + 0x4 * (b))
#define HWIO_SEQ_CFG_BR_ADDR_b_RMSK                                                                     0x7f
#define HWIO_SEQ_CFG_BR_ADDR_b_MAXb                                                                        3
#define HWIO_SEQ_CFG_BR_ADDR_b_INI(base,b)        \
        in_dword_masked(HWIO_SEQ_CFG_BR_ADDR_b_ADDR(base,b), HWIO_SEQ_CFG_BR_ADDR_b_RMSK)
#define HWIO_SEQ_CFG_BR_ADDR_b_INMI(base,b,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_BR_ADDR_b_ADDR(base,b), mask)
#define HWIO_SEQ_CFG_BR_ADDR_b_OUTI(base,b,val)    \
        out_dword(HWIO_SEQ_CFG_BR_ADDR_b_ADDR(base,b),val)
#define HWIO_SEQ_CFG_BR_ADDR_b_OUTMI(base,b,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_BR_ADDR_b_ADDR(base,b),mask,val,HWIO_SEQ_CFG_BR_ADDR_b_INI(base,b))
#define HWIO_SEQ_CFG_BR_ADDR_b_BR_ADDR_BMSK                                                             0x7f
#define HWIO_SEQ_CFG_BR_ADDR_b_BR_ADDR_SHFT                                                              0x0

#define HWIO_SEQ_CFG_DELAY_VAL_v_ADDR(base,v)                                                     ((base) + 0x000045a0 + 0x4 * (v))
#define HWIO_SEQ_CFG_DELAY_VAL_v_RMSK                                                                0xfffff
#define HWIO_SEQ_CFG_DELAY_VAL_v_MAXv                                                                     15
#define HWIO_SEQ_CFG_DELAY_VAL_v_INI(base,v)        \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_VAL_v_ADDR(base,v), HWIO_SEQ_CFG_DELAY_VAL_v_RMSK)
#define HWIO_SEQ_CFG_DELAY_VAL_v_INMI(base,v,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_VAL_v_ADDR(base,v), mask)
#define HWIO_SEQ_CFG_DELAY_VAL_v_OUTI(base,v,val)    \
        out_dword(HWIO_SEQ_CFG_DELAY_VAL_v_ADDR(base,v),val)
#define HWIO_SEQ_CFG_DELAY_VAL_v_OUTMI(base,v,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_DELAY_VAL_v_ADDR(base,v),mask,val,HWIO_SEQ_CFG_DELAY_VAL_v_INI(base,v))
#define HWIO_SEQ_CFG_DELAY_VAL_v_DELAY_VAL_BMSK                                                      0xfffff
#define HWIO_SEQ_CFG_DELAY_VAL_v_DELAY_VAL_SHFT                                                          0x0

#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_ADDR(x)                                                      ((x) + 0x000045e0)
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_RMSK                                                         0x8000007f
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_IN(x)      \
        in_dword_masked(HWIO_SEQ_DBG_BREAKPOINT_ADDR_ADDR(x), HWIO_SEQ_DBG_BREAKPOINT_ADDR_RMSK)
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_DBG_BREAKPOINT_ADDR_ADDR(x), m)
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_OUT(x, v)      \
        out_dword(HWIO_SEQ_DBG_BREAKPOINT_ADDR_ADDR(x),v)
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_DBG_BREAKPOINT_ADDR_ADDR(x),m,v,HWIO_SEQ_DBG_BREAKPOINT_ADDR_IN(x))
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_BMSK                                    0x80000000
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_SHFT                                          0x1f
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_BMSK                                           0x7f
#define HWIO_SEQ_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_SHFT                                            0x0

#define HWIO_SEQ_DBG_STEP_ADDR(x)                                                                 ((x) + 0x000045e4)
#define HWIO_SEQ_DBG_STEP_RMSK                                                                       0x10001
#define HWIO_SEQ_DBG_STEP_IN(x)      \
        in_dword_masked(HWIO_SEQ_DBG_STEP_ADDR(x), HWIO_SEQ_DBG_STEP_RMSK)
#define HWIO_SEQ_DBG_STEP_INM(x, m)      \
        in_dword_masked(HWIO_SEQ_DBG_STEP_ADDR(x), m)
#define HWIO_SEQ_DBG_STEP_OUT(x, v)      \
        out_dword(HWIO_SEQ_DBG_STEP_ADDR(x),v)
#define HWIO_SEQ_DBG_STEP_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SEQ_DBG_STEP_ADDR(x),m,v,HWIO_SEQ_DBG_STEP_IN(x))
#define HWIO_SEQ_DBG_STEP_SEQ_DBG_STEP_TRIGGER_BMSK                                                  0x10000
#define HWIO_SEQ_DBG_STEP_SEQ_DBG_STEP_TRIGGER_SHFT                                                     0x10
#define HWIO_SEQ_DBG_STEP_SEQ_DBG_STEP_CONTINUE_BMSK                                                     0x1
#define HWIO_SEQ_DBG_STEP_SEQ_DBG_STEP_CONTINUE_SHFT                                                     0x0

#define HWIO_IRQ_i_OWNER_ADDR(base,i)                                                             ((base) + 0x00004600 + 0x4 * (i))
#define HWIO_IRQ_i_OWNER_RMSK                                                                            0x1
#define HWIO_IRQ_i_OWNER_MAXi                                                                             44
#define HWIO_IRQ_i_OWNER_INI(base,i)        \
        in_dword_masked(HWIO_IRQ_i_OWNER_ADDR(base,i), HWIO_IRQ_i_OWNER_RMSK)
#define HWIO_IRQ_i_OWNER_INMI(base,i,mask)    \
        in_dword_masked(HWIO_IRQ_i_OWNER_ADDR(base,i), mask)
#define HWIO_IRQ_i_OWNER_OUTI(base,i,val)    \
        out_dword(HWIO_IRQ_i_OWNER_ADDR(base,i),val)
#define HWIO_IRQ_i_OWNER_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_IRQ_i_OWNER_ADDR(base,i),mask,val,HWIO_IRQ_i_OWNER_INI(base,i))
#define HWIO_IRQ_i_OWNER_IRQ_OWNER_BMSK                                                                  0x1
#define HWIO_IRQ_i_OWNER_IRQ_OWNER_SHFT                                                                  0x0

#define HWIO_GP_IRQ_SELECTgp_ADDR(base,gp)                                                        ((base) + 0x00004900 + 0x14 * (gp))
#define HWIO_GP_IRQ_SELECTgp_RMSK                                                                       0x7f
#define HWIO_GP_IRQ_SELECTgp_MAXgp                                                                        29
#define HWIO_GP_IRQ_SELECTgp_INI(base,gp)        \
        in_dword_masked(HWIO_GP_IRQ_SELECTgp_ADDR(base,gp), HWIO_GP_IRQ_SELECTgp_RMSK)
#define HWIO_GP_IRQ_SELECTgp_INMI(base,gp,mask)    \
        in_dword_masked(HWIO_GP_IRQ_SELECTgp_ADDR(base,gp), mask)
#define HWIO_GP_IRQ_SELECTgp_OUTI(base,gp,val)    \
        out_dword(HWIO_GP_IRQ_SELECTgp_ADDR(base,gp),val)
#define HWIO_GP_IRQ_SELECTgp_OUTMI(base,gp,mask,val) \
        out_dword_masked_ns(HWIO_GP_IRQ_SELECTgp_ADDR(base,gp),mask,val,HWIO_GP_IRQ_SELECTgp_INI(base,gp))
#define HWIO_GP_IRQ_SELECTgp_IRQ_SELECT_BMSK                                                            0x7f
#define HWIO_GP_IRQ_SELECTgp_IRQ_SELECT_SHFT                                                             0x0

#define HWIO_PROFILING_UNITpf_EN_ADDR(base,pf)                                                    ((base) + 0x00005000 + 0x14 * (pf))
#define HWIO_PROFILING_UNITpf_EN_RMSK                                                                    0x1
#define HWIO_PROFILING_UNITpf_EN_MAXpf                                                                     4
#define HWIO_PROFILING_UNITpf_EN_INI(base,pf)        \
        in_dword_masked(HWIO_PROFILING_UNITpf_EN_ADDR(base,pf), HWIO_PROFILING_UNITpf_EN_RMSK)
#define HWIO_PROFILING_UNITpf_EN_INMI(base,pf,mask)    \
        in_dword_masked(HWIO_PROFILING_UNITpf_EN_ADDR(base,pf), mask)
#define HWIO_PROFILING_UNITpf_EN_OUTI(base,pf,val)    \
        out_dword(HWIO_PROFILING_UNITpf_EN_ADDR(base,pf),val)
#define HWIO_PROFILING_UNITpf_EN_OUTMI(base,pf,mask,val) \
        out_dword_masked_ns(HWIO_PROFILING_UNITpf_EN_ADDR(base,pf),mask,val,HWIO_PROFILING_UNITpf_EN_INI(base,pf))
#define HWIO_PROFILING_UNITpf_EN_PROFILING_UNIT_ENABLE_BMSK                                              0x1
#define HWIO_PROFILING_UNITpf_EN_PROFILING_UNIT_ENABLE_SHFT                                              0x0

#define HWIO_TCSt_CONTROL_ADDR(base,t)                                                            ((base) + 0x00005500 + 0xC8 * (t))
#define HWIO_TCSt_CONTROL_RMSK                                                                     0x1010000
#define HWIO_TCSt_CONTROL_MAXt                                                                             2
#define HWIO_TCSt_CONTROL_INI(base,t)        \
        in_dword_masked(HWIO_TCSt_CONTROL_ADDR(base,t), HWIO_TCSt_CONTROL_RMSK)
#define HWIO_TCSt_CONTROL_INMI(base,t,mask)    \
        in_dword_masked(HWIO_TCSt_CONTROL_ADDR(base,t), mask)
#define HWIO_TCSt_CONTROL_OUTI(base,t,val)    \
        out_dword(HWIO_TCSt_CONTROL_ADDR(base,t),val)
#define HWIO_TCSt_CONTROL_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CONTROL_ADDR(base,t),mask,val,HWIO_TCSt_CONTROL_INI(base,t))
#define HWIO_TCSt_CONTROL_AMC_MODE_TRIGGER_BMSK                                                    0x1000000
#define HWIO_TCSt_CONTROL_AMC_MODE_TRIGGER_SHFT                                                         0x18
#define HWIO_TCSt_CONTROL_AMC_MODE_EN_BMSK                                                           0x10000
#define HWIO_TCSt_CONTROL_AMC_MODE_EN_SHFT                                                              0x10

#define HWIO_TCSt_CMD_ENABLE_BANK_ADDR(base,t)                                                    ((base) + 0x00005504 + 0xC8 * (t))
#define HWIO_TCSt_CMD_ENABLE_BANK_RMSK                                                                   0xf
#define HWIO_TCSt_CMD_ENABLE_BANK_MAXt                                                                     2
#define HWIO_TCSt_CMD_ENABLE_BANK_INI(base,t)        \
        in_dword_masked(HWIO_TCSt_CMD_ENABLE_BANK_ADDR(base,t), HWIO_TCSt_CMD_ENABLE_BANK_RMSK)
#define HWIO_TCSt_CMD_ENABLE_BANK_INMI(base,t,mask)    \
        in_dword_masked(HWIO_TCSt_CMD_ENABLE_BANK_ADDR(base,t), mask)
#define HWIO_TCSt_CMD_ENABLE_BANK_OUTI(base,t,val)    \
        out_dword(HWIO_TCSt_CMD_ENABLE_BANK_ADDR(base,t),val)
#define HWIO_TCSt_CMD_ENABLE_BANK_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CMD_ENABLE_BANK_ADDR(base,t),mask,val,HWIO_TCSt_CMD_ENABLE_BANK_INI(base,t))
#define HWIO_TCSt_CMD_ENABLE_BANK_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                        0xf
#define HWIO_TCSt_CMD_ENABLE_BANK_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                        0x0

#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(base,t)                                             ((base) + 0x00005508 + 0xC8 * (t))
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_RMSK                                                            0xf
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_MAXt                                                              2
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_INI(base,t)        \
        in_dword_masked(HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(base,t), HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_RMSK)
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_INMI(base,t,mask)    \
        in_dword_masked(HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(base,t), mask)
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_OUTI(base,t,val)    \
        out_dword(HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(base,t),val)
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_ADDR(base,t),mask,val,HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_INI(base,t))
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_CMD_WAIT_FOR_CMPL_BANK_BMSK                                     0xf
#define HWIO_TCSt_CMD_WAIT_FOR_CMPL_BANK_CMD_WAIT_FOR_CMPL_BANK_SHFT                                     0x0

#define HWIO_TCSt_CMDn_MSGID_ADDR(base,t,n)                                                       ((base) + 0x0000550c + 0xC8 * (t) + 0x10 * (n))
#define HWIO_TCSt_CMDn_MSGID_RMSK                                                                    0x10100
#define HWIO_TCSt_CMDn_MSGID_MAXt                                                                          2
#define HWIO_TCSt_CMDn_MSGID_MAXn                                                                          3
#define HWIO_TCSt_CMDn_MSGID_INI2(base,t,n)        \
        in_dword_masked(HWIO_TCSt_CMDn_MSGID_ADDR(base,t,n), HWIO_TCSt_CMDn_MSGID_RMSK)
#define HWIO_TCSt_CMDn_MSGID_INMI2(base,t,n,mask)    \
        in_dword_masked(HWIO_TCSt_CMDn_MSGID_ADDR(base,t,n), mask)
#define HWIO_TCSt_CMDn_MSGID_OUTI2(base,t,n,val)    \
        out_dword(HWIO_TCSt_CMDn_MSGID_ADDR(base,t,n),val)
#define HWIO_TCSt_CMDn_MSGID_OUTMI2(base,t,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CMDn_MSGID_ADDR(base,t,n),mask,val,HWIO_TCSt_CMDn_MSGID_INI2(base,t,n))
#define HWIO_TCSt_CMDn_MSGID_READ_OR_WRITE_BMSK                                                      0x10000
#define HWIO_TCSt_CMDn_MSGID_READ_OR_WRITE_SHFT                                                         0x10
#define HWIO_TCSt_CMDn_MSGID_RES_REQ_BMSK                                                              0x100
#define HWIO_TCSt_CMDn_MSGID_RES_REQ_SHFT                                                                0x8

#define HWIO_TCSt_CMDn_ADDR_ADDR(base,t,n)                                                        ((base) + 0x00005510 + 0xC8 * (t) + 0x10 * (n))
#define HWIO_TCSt_CMDn_ADDR_RMSK                                                                     0x7fffc
#define HWIO_TCSt_CMDn_ADDR_MAXt                                                                           2
#define HWIO_TCSt_CMDn_ADDR_MAXn                                                                           3
#define HWIO_TCSt_CMDn_ADDR_INI2(base,t,n)        \
        in_dword_masked(HWIO_TCSt_CMDn_ADDR_ADDR(base,t,n), HWIO_TCSt_CMDn_ADDR_RMSK)
#define HWIO_TCSt_CMDn_ADDR_INMI2(base,t,n,mask)    \
        in_dword_masked(HWIO_TCSt_CMDn_ADDR_ADDR(base,t,n), mask)
#define HWIO_TCSt_CMDn_ADDR_OUTI2(base,t,n,val)    \
        out_dword(HWIO_TCSt_CMDn_ADDR_ADDR(base,t,n),val)
#define HWIO_TCSt_CMDn_ADDR_OUTMI2(base,t,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CMDn_ADDR_ADDR(base,t,n),mask,val,HWIO_TCSt_CMDn_ADDR_INI2(base,t,n))
#define HWIO_TCSt_CMDn_ADDR_SLV_ID_BMSK                                                              0x70000
#define HWIO_TCSt_CMDn_ADDR_SLV_ID_SHFT                                                                 0x10
#define HWIO_TCSt_CMDn_ADDR_OFFSET_BMSK                                                               0xfffc
#define HWIO_TCSt_CMDn_ADDR_OFFSET_SHFT                                                                  0x2

#define HWIO_TCSt_CMDn_DATA_ADDR(base,t,n)                                                        ((base) + 0x00005514 + 0xC8 * (t) + 0x10 * (n))
#define HWIO_TCSt_CMDn_DATA_RMSK                                                                      0x1fff
#define HWIO_TCSt_CMDn_DATA_MAXt                                                                           2
#define HWIO_TCSt_CMDn_DATA_MAXn                                                                           3
#define HWIO_TCSt_CMDn_DATA_INI2(base,t,n)        \
        in_dword_masked(HWIO_TCSt_CMDn_DATA_ADDR(base,t,n), HWIO_TCSt_CMDn_DATA_RMSK)
#define HWIO_TCSt_CMDn_DATA_INMI2(base,t,n,mask)    \
        in_dword_masked(HWIO_TCSt_CMDn_DATA_ADDR(base,t,n), mask)
#define HWIO_TCSt_CMDn_DATA_OUTI2(base,t,n,val)    \
        out_dword(HWIO_TCSt_CMDn_DATA_ADDR(base,t,n),val)
#define HWIO_TCSt_CMDn_DATA_OUTMI2(base,t,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSt_CMDn_DATA_ADDR(base,t,n),mask,val,HWIO_TCSt_CMDn_DATA_INI2(base,t,n))
#define HWIO_TCSt_CMDn_DATA_TCS_CMD_DATA_BMSK                                                         0x1fff
#define HWIO_TCSt_CMDn_DATA_TCS_CMD_DATA_SHFT                                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: MODEM_PDC_SEQ_MEM
 *--------------------------------------------------------------------------*/

#define MODEM_PDC_SEQ_MEM_REG_BASE                                    (AOSS_BASE      + 0x004b0000)

#define HWIO_SEQ_MEM_m_ADDR(base,m)                                   ((base) + 0x00200000 + 0x4 * (m))
#define HWIO_SEQ_MEM_m_RMSK                                           0xffffffff
#define HWIO_SEQ_MEM_m_MAXm                                                   23
#define HWIO_SEQ_MEM_m_INI(base,m)        \
        in_dword_masked(HWIO_SEQ_MEM_m_ADDR(base,m), HWIO_SEQ_MEM_m_RMSK)
#define HWIO_SEQ_MEM_m_INMI(base,m,mask)    \
        in_dword_masked(HWIO_SEQ_MEM_m_ADDR(base,m), mask)
#define HWIO_SEQ_MEM_m_OUTI(base,m,val)    \
        out_dword(HWIO_SEQ_MEM_m_ADDR(base,m),val)
#define HWIO_SEQ_MEM_m_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_MEM_m_ADDR(base,m),mask,val,HWIO_SEQ_MEM_m_INI(base,m))
#define HWIO_SEQ_MEM_m_CMD_3_INSTR_FIELD_BMSK                         0xe0000000
#define HWIO_SEQ_MEM_m_CMD_3_INSTR_FIELD_SHFT                               0x1d
#define HWIO_SEQ_MEM_m_CMD_3_DATA_FIELD_BMSK                          0x1f000000
#define HWIO_SEQ_MEM_m_CMD_3_DATA_FIELD_SHFT                                0x18
#define HWIO_SEQ_MEM_m_CMD_2_INSTR_FIELD_BMSK                           0xe00000
#define HWIO_SEQ_MEM_m_CMD_2_INSTR_FIELD_SHFT                               0x15
#define HWIO_SEQ_MEM_m_CMD_2_DATA_FIELD_BMSK                            0x1f0000
#define HWIO_SEQ_MEM_m_CMD_2_DATA_FIELD_SHFT                                0x10
#define HWIO_SEQ_MEM_m_CMD_1_INSTR_FIELD_BMSK                             0xe000
#define HWIO_SEQ_MEM_m_CMD_1_INSTR_FIELD_SHFT                                0xd
#define HWIO_SEQ_MEM_m_CMD_1_DATA_FIELD_BMSK                              0x1f00
#define HWIO_SEQ_MEM_m_CMD_1_DATA_FIELD_SHFT                                 0x8
#define HWIO_SEQ_MEM_m_CMD_0_INSTR_FIELD_BMSK                               0xe0
#define HWIO_SEQ_MEM_m_CMD_0_INSTR_FIELD_SHFT                                0x5
#define HWIO_SEQ_MEM_m_CMD_0_DATA_FIELD_BMSK                                0x1f
#define HWIO_SEQ_MEM_m_CMD_0_DATA_FIELD_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: RSCC_RSC
 *--------------------------------------------------------------------------*/

#define RSCC_RSC_REG_BASE                                                                                       (MODEM_TOP_BASE      + 0x00200000)

#define HWIO_RSC_ID_DRVd_ADDR(base,d)                                                                           ((base) + 0x00000000 + 0x10000 * (d))
#define HWIO_RSC_ID_DRVd_RMSK                                                                                     0xffffff
#define HWIO_RSC_ID_DRVd_MAXd                                                                                            1
#define HWIO_RSC_ID_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_ID_DRVd_ADDR(base,d), HWIO_RSC_ID_DRVd_RMSK)
#define HWIO_RSC_ID_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_ID_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                           0xff0000
#define HWIO_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                               0x10
#define HWIO_RSC_ID_DRVd_MINOR_VER_BMSK                                                                             0xff00
#define HWIO_RSC_ID_DRVd_MINOR_VER_SHFT                                                                                0x8
#define HWIO_RSC_ID_DRVd_STEP_VER_BMSK                                                                                0xff
#define HWIO_RSC_ID_DRVd_STEP_VER_SHFT                                                                                 0x0

#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d)                                                          ((base) + 0x00000004 + 0x10000 * (d))
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                                   0x71f1f1f
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                           1
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_BMSK                                                      0x4000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_SHFT                                                           0x1a
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                                      0x2000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                           0x19
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                         0x1000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                              0x18
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                                       0x1f0000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                           0x10
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                         0x1f00
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                            0x8
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                             0x1f
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                              0x0

#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d)                                                             ((base) + 0x00000008 + 0x10000 * (d))
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                                      0x7ff0f1f
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                              1
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                              0x7000000
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                                   0x18
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                                     0xff0000
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                         0x10
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                            0xf00
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                              0x8
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                                       0x1f
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                                        0x0

#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d)                                                 ((base) + 0x0000000c + 0x10000 * (d))
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                         0xf8ffffff
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                                  1
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                                        0xf8000000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                              0x1b
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                              0xfc0000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                                  0x12
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                               0x3f000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                                   0xc
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                                 0xfc0
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                                   0x6
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                                  0x3f
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                                   0x0

#define HWIO_RSC_STATUS0_DRVd_ADDR(base,d)                                                                      ((base) + 0x00000010 + 0x10000 * (d))
#define HWIO_RSC_STATUS0_DRVd_RMSK                                                                               0x101ffff
#define HWIO_RSC_STATUS0_DRVd_MAXd                                                                                       1
#define HWIO_RSC_STATUS0_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS0_DRVd_ADDR(base,d), HWIO_RSC_STATUS0_DRVd_RMSK)
#define HWIO_RSC_STATUS0_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS0_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS0_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_RSC_STATUS0_DRVd_ADDR(base,d),val)
#define HWIO_RSC_STATUS0_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_RSC_STATUS0_DRVd_ADDR(base,d),mask,val,HWIO_RSC_STATUS0_DRVd_INI(base,d))
#define HWIO_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_BMSK                                                              0x1000000
#define HWIO_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_SHFT                                                                   0x18
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_BMSK                                                         0x10000
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_SHFT                                                            0x10
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                             0xffff
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                                0x0

#define HWIO_RSC_STATUS1_DRVd_ADDR(base,d)                                                                      ((base) + 0x00000014 + 0x10000 * (d))
#define HWIO_RSC_STATUS1_DRVd_RMSK                                                                              0xffffffff
#define HWIO_RSC_STATUS1_DRVd_MAXd                                                                                       1
#define HWIO_RSC_STATUS1_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS1_DRVd_ADDR(base,d), HWIO_RSC_STATUS1_DRVd_RMSK)
#define HWIO_RSC_STATUS1_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS1_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                           0xffffffff
#define HWIO_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                                  0x0

#define HWIO_RSC_STATUS2_DRVd_ADDR(base,d)                                                                      ((base) + 0x00000018 + 0x10000 * (d))
#define HWIO_RSC_STATUS2_DRVd_RMSK                                                                                0xffffff
#define HWIO_RSC_STATUS2_DRVd_MAXd                                                                                       1
#define HWIO_RSC_STATUS2_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS2_DRVd_ADDR(base,d), HWIO_RSC_STATUS2_DRVd_RMSK)
#define HWIO_RSC_STATUS2_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS2_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                             0xffffff
#define HWIO_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                                  0x0

#define HWIO_HIDDEN_TCS_CTRL_DRV0_ADDR(x)                                                                       ((x) + 0x0000001c)
#define HWIO_HIDDEN_TCS_CTRL_DRV0_RMSK                                                                             0x10003
#define HWIO_HIDDEN_TCS_CTRL_DRV0_IN(x)      \
        in_dword_masked(HWIO_HIDDEN_TCS_CTRL_DRV0_ADDR(x), HWIO_HIDDEN_TCS_CTRL_DRV0_RMSK)
#define HWIO_HIDDEN_TCS_CTRL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_HIDDEN_TCS_CTRL_DRV0_ADDR(x), m)
#define HWIO_HIDDEN_TCS_CTRL_DRV0_OUT(x, v)      \
        out_dword(HWIO_HIDDEN_TCS_CTRL_DRV0_ADDR(x),v)
#define HWIO_HIDDEN_TCS_CTRL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HIDDEN_TCS_CTRL_DRV0_ADDR(x),m,v,HWIO_HIDDEN_TCS_CTRL_DRV0_IN(x))
#define HWIO_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_BMSK                                                          0x10000
#define HWIO_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_TRIGGER_SHFT                                                             0x10
#define HWIO_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                                  0x3
#define HWIO_HIDDEN_TCS_CTRL_DRV0_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                                  0x0

#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x)                                                         ((x) + 0x00000020)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK                                                                0xffff
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x), HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_RMSK)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_IN(x))
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRV0_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x)                                                         ((x) + 0x00000024)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK                                                                0xffff
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x), HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_RMSK)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_IN(x))
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRV0_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x)                                                         ((x) + 0x00000028)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK                                                                0xffff
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN(x)      \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x), HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_RMSK)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x), m)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUT(x, v)      \
        out_dword(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x),v)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_ADDR(x),m,v,HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_IN(x))
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRV0_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_SLAVE_ID_DRV0_ADDR(x)                                                                          ((x) + 0x0000002c)
#define HWIO_PDC_SLAVE_ID_DRV0_RMSK                                                                                    0x7
#define HWIO_PDC_SLAVE_ID_DRV0_IN(x)      \
        in_dword_masked(HWIO_PDC_SLAVE_ID_DRV0_ADDR(x), HWIO_PDC_SLAVE_ID_DRV0_RMSK)
#define HWIO_PDC_SLAVE_ID_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_PDC_SLAVE_ID_DRV0_ADDR(x), m)
#define HWIO_PDC_SLAVE_ID_DRV0_OUT(x, v)      \
        out_dword(HWIO_PDC_SLAVE_ID_DRV0_ADDR(x),v)
#define HWIO_PDC_SLAVE_ID_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PDC_SLAVE_ID_DRV0_ADDR(x),m,v,HWIO_PDC_SLAVE_ID_DRV0_IN(x))
#define HWIO_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_BMSK                                                                       0x7
#define HWIO_PDC_SLAVE_ID_DRV0_PDC_SLAVE_ID_SHFT                                                                       0x0

#define HWIO_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d)                                                                  ((base) + 0x0000001c + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CTRL_DRVd_RMSK                                                                                 0x3
#define HWIO_HIDDEN_TCS_CTRL_DRVd_MAXd                                                                                   1
#define HWIO_HIDDEN_TCS_CTRL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CTRL_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CTRL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CTRL_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CTRL_DRVd_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_BMSK                                                  0x3
#define HWIO_HIDDEN_TCS_CTRL_DRVd_HIDDEN_TCS_PDC_TIMER_DRV_OWNER_SHFT                                                  0x0

#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d)                                                    ((base) + 0x00000020 + 0x10000 * (d))
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_RMSK                                                                0xffff
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_MAXd                                                                     1
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d), HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_RMSK)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_PDC_SEQ_START_ADDR_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_SEQ_START_ADDR_REG_OFFSET_DRVd_PDC_SEQ_START_ADDR_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d)                                                    ((base) + 0x00000024 + 0x10000 * (d))
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_RMSK                                                                0xffff
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_MAXd                                                                     1
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d), HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_RMSK)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_PDC_MATCH_VALUE_LO_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_MATCH_VALUE_LO_REG_OFFSET_DRVd_PDC_MATCH_VALUE_LO_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d)                                                    ((base) + 0x00000028 + 0x10000 * (d))
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_RMSK                                                                0xffff
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_MAXd                                                                     1
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d), HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_RMSK)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_PDC_MATCH_VALUE_HI_REG_OFFSET_BMSK                                  0xffff
#define HWIO_PDC_MATCH_VALUE_HI_REG_OFFSET_DRVd_PDC_MATCH_VALUE_HI_REG_OFFSET_SHFT                                     0x0

#define HWIO_PDC_SLAVE_ID_DRVd_ADDR(base,d)                                                                     ((base) + 0x0000002c + 0x10000 * (d))
#define HWIO_PDC_SLAVE_ID_DRVd_RMSK                                                                                    0x7
#define HWIO_PDC_SLAVE_ID_DRVd_MAXd                                                                                      1
#define HWIO_PDC_SLAVE_ID_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_PDC_SLAVE_ID_DRVd_ADDR(base,d), HWIO_PDC_SLAVE_ID_DRVd_RMSK)
#define HWIO_PDC_SLAVE_ID_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_PDC_SLAVE_ID_DRVd_ADDR(base,d), mask)
#define HWIO_PDC_SLAVE_ID_DRVd_PDC_SLAVE_ID_BMSK                                                                       0x7
#define HWIO_PDC_SLAVE_ID_DRVd_PDC_SLAVE_ID_SHFT                                                                       0x0

#define HWIO_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d)                                                                ((base) + 0x00000030 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_STATUS_DRVd_RMSK                                                                           0x70707
#define HWIO_HIDDEN_TCS_STATUS_DRVd_MAXd                                                                                 1
#define HWIO_HIDDEN_TCS_STATUS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_STATUS_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_STATUS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_STATUS_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_STATUS_DRVd_COMPLETED_BMSK                                                                 0x70000
#define HWIO_HIDDEN_TCS_STATUS_DRVd_COMPLETED_SHFT                                                                    0x10
#define HWIO_HIDDEN_TCS_STATUS_DRVd_ISSUED_BMSK                                                                      0x700
#define HWIO_HIDDEN_TCS_STATUS_DRVd_ISSUED_SHFT                                                                        0x8
#define HWIO_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_BMSK                                                                     0x7
#define HWIO_HIDDEN_TCS_STATUS_DRVd_TRIGGERED_SHFT                                                                     0x0

#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d)                                                             ((base) + 0x00000034 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK                                                                     0xffffffff
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_BMSK                                                                0xffffffff
#define HWIO_HIDDEN_TCS_CMD0_ADDR_DRVd_ADDR_SHFT                                                                       0x0

#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d)                                                             ((base) + 0x00000038 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK                                                                     0x80ffffff
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d),val)
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_ADDR(base,d),mask,val,HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_INI(base,d))
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_BMSK                                               0x80000000
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_VALID_SHFT                                                     0x1f
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                    0xffffff
#define HWIO_HIDDEN_TCS_CMD0_DATA_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                         0x0

#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d)                                                             ((base) + 0x0000003c + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK                                                                     0xffffffff
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_BMSK                                                                0xffffffff
#define HWIO_HIDDEN_TCS_CMD1_ADDR_DRVd_ADDR_SHFT                                                                       0x0

#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d)                                                             ((base) + 0x00000040 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK                                                                     0xffffffff
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d),val)
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_ADDR(base,d),mask,val,HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_INI(base,d))
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                  0xffffffff
#define HWIO_HIDDEN_TCS_CMD1_DATA_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                         0x0

#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d)                                                             ((base) + 0x00000044 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK                                                                     0xffffffff
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_BMSK                                                                0xffffffff
#define HWIO_HIDDEN_TCS_CMD2_ADDR_DRVd_ADDR_SHFT                                                                       0x0

#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x)                                                                  ((x) + 0x00000048)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK                                                                     0x8000ffff
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_IN(x)      \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x), HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_RMSK)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x), m)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_OUT(x, v)      \
        out_dword(HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x),v)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_ADDR(x),m,v,HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_IN(x))
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_BMSK                                            0x80000000
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_VALID_SHFT                                                  0x1f
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_BMSK                                                      0xffff
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRV0_PDC_SEQ_START_ADDR_SHFT                                                         0x0

#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d)                                                             ((base) + 0x00000048 + 0x10000 * (d))
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_RMSK                                                                     0x8000ffff
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_MAXd                                                                              1
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d), HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_RMSK)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_ADDR(base,d), mask)
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_VALID_BMSK                                            0x80000000
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_VALID_SHFT                                                  0x1f
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_BMSK                                                      0xffff
#define HWIO_HIDDEN_TCS_CMD2_DATA_DRVd_PDC_SEQ_START_ADDR_SHFT                                                         0x0

#define HWIO_HW_EVENT_OWNER_DRV0_ADDR(x)                                                                        ((x) + 0x0000004c)
#define HWIO_HW_EVENT_OWNER_DRV0_RMSK                                                                                  0x3
#define HWIO_HW_EVENT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x), HWIO_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_HW_EVENT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x), m)
#define HWIO_HW_EVENT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x),v)
#define HWIO_HW_EVENT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x),m,v,HWIO_HW_EVENT_OWNER_DRV0_IN(x))
#define HWIO_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                            0x3
#define HWIO_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                            0x0

#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m)                                                           ((base) + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                                     0x80000007
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                              1
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                             31
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), HWIO_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), mask)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),val)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),mask,val,HWIO_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m))
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                              0x80000000
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                                    0x1f
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                               0x7
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                               0x0

#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x)                                                                  ((x) + 0x000000d0)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                            0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), HWIO_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), m)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                           0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                           0x0

#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x)                                                                   ((x) + 0x000000d4)
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                             0x1
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x),v)
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                                        0x1
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                                        0x0

#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x)                                                                  ((x) + 0x000000d8)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                                       0x10ffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), m)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),v)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),m,v,HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x))
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                                   0x100000
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                                       0x14
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                                  0xffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                                     0x0

#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d)                                                             ((base) + 0x000000d0 + 0x10000 * (d))
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_RMSK                                                                            0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_MAXd                                                                              1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d), HWIO_RSC_ERROR_IRQ_STATUS_DRVd_RMSK)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_EPCB_TIMEOUT_IRQ_BMSK                                                           0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRVd_EPCB_TIMEOUT_IRQ_SHFT                                                           0x0

#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d)                                                             ((base) + 0x000000d8 + 0x10000 * (d))
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_RMSK                                                                       0x10ffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_MAXd                                                                              1
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d), HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_RMSK)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_BMSK                                                   0x100000
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_IRQ_EN_SHFT                                                       0x14
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_THRESHOLD_BMSK                                                  0xffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRVd_EPCB_TIMEOUT_THRESHOLD_SHFT                                                     0x0

#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x)                                                              ((x) + 0x00000200)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                                        0x3
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), m)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),v)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),m,v,HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x))
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                                  0x3
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                                  0x0

#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m)                                                         ((base) + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                          0x1
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                            1
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                            5
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),val)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),mask,val,HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m))
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                                   0x1
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                                   0x0

#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m)                                                ((base) + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                          0xffffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                                   1
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                                   5
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                                0xffffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                                       0x0

#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m)                                                ((base) + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                            0xffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                                   1
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                                   5
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                                  0xffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                                       0x0

#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m)                                                     ((base) + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                                    0x101
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                                        1
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                                        5
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                                        0x100
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                          0x8
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                             0x1
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                             0x0

#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x)                                                           ((x) + 0x00000400)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                              0x800003ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                                        0x80000000
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                              0x1f
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                              0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                                0x0

#define HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x)                                                                          ((x) + 0x00000404)
#define HWIO_RSC_SEQ_BUSY_DRV0_RMSK                                                                                    0x1
#define HWIO_RSC_SEQ_BUSY_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x), HWIO_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_RSC_SEQ_BUSY_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                          0x1
#define HWIO_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                          0x0

#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x)                                                               ((x) + 0x00000408)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                                       0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                                    0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                                      0x0

#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i)                                                            ((base) + 0x00000410 + 0x4 * (i))
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                         0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                             3
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), mask)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),val)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),mask,val,HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i))
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                                    0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                                      0x0

#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i)                                                          ((base) + 0x00000450 + 0x4 * (i))
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                                     0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                           3
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), mask)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),val)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),mask,val,HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i))
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                           0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                               0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x)                                                              ((x) + 0x00000460)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                                        0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                                   0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                                   0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x)                                                ((x) + 0x00000464)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                                        0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK                        0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                          0x0

#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d)                                                      ((base) + 0x00000400 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_RMSK                                                              0x800003ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_MAXd                                                                       1
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_VALID_BMSK                                                        0x80000000
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_VALID_SHFT                                                              0x1f
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR_BMSK                                                              0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRVd_ADDR_SHFT                                                                0x0

#define HWIO_RSC_SEQ_BUSY_DRVd_ADDR(base,d)                                                                     ((base) + 0x00000404 + 0x10000 * (d))
#define HWIO_RSC_SEQ_BUSY_DRVd_RMSK                                                                                    0x1
#define HWIO_RSC_SEQ_BUSY_DRVd_MAXd                                                                                      1
#define HWIO_RSC_SEQ_BUSY_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRVd_ADDR(base,d), HWIO_RSC_SEQ_BUSY_DRVd_RMSK)
#define HWIO_RSC_SEQ_BUSY_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_BUSY_DRVd_BUSY_STAT_BMSK                                                                          0x1
#define HWIO_RSC_SEQ_BUSY_DRVd_BUSY_STAT_SHFT                                                                          0x0

#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d)                                                          ((base) + 0x00000408 + 0x10000 * (d))
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_RMSK                                                                       0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_MAXd                                                                           1
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d), HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_RMSK)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_BMSK                                                    0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRVd_CURRENT_INSTR_ADDR_SHFT                                                      0x0

#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i)                                                          ((base) + 0x00000410 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_RMSK                                                                         0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_MAXd                                                                             1
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_MAXi                                                                             3
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_INI2(base,d,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i), HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_RMSK)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_INMI2(base,d,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR(base,d,i), mask)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR_BMSK                                                                    0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRVd_ADDR_SHFT                                                                      0x0

#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i)                                                        ((base) + 0x00000450 + 0x4 * (i) + 0x10000 * (d))
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_RMSK                                                                     0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_MAXd                                                                           1
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_MAXi                                                                           3
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_INI2(base,d,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i), HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_RMSK)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_INMI2(base,d,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_ADDR(base,d,i), mask)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_DELAY_VAL_BMSK                                                           0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRVd_DELAY_VAL_SHFT                                                               0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d)                                                         ((base) + 0x00000460 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_RMSK                                                                        0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_MAXd                                                                          1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_SEQ_OVERRIDE_TRIGGER_BMSK                                                   0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRVd_SEQ_OVERRIDE_TRIGGER_SHFT                                                   0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d)                                           ((base) + 0x00000464 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_RMSK                                                        0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_MAXd                                                            1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK                        0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRVd_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                          0x0

#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x)                                                           ((x) + 0x00000490)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                              0x800003ff
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                         0x80000000
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                               0x1f
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                               0x3ff
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                                 0x0

#define HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x)                                                                      ((x) + 0x00000494)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                                0x1
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_STEP_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                               0x1
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                               0x0

#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x)                                                                  ((x) + 0x00000498)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                            0x1
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                          0x1
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                          0x0

#define HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x)                                                                      ((x) + 0x0000049c)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                                0x1
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                            0x1
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                            0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x)                                                       ((x) + 0x000004a0)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                                0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                                    0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                                     0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x)                                                        ((x) + 0x000004a4)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                                 0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                                      0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                                       0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x)                                                      ((x) + 0x000004a8)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                               0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                                  0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                                   0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x)                                                       ((x) + 0x000004ac)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                                0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                                    0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                                     0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d)                                                  ((base) + 0x000004a0 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_RMSK                                                                0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_MAXd                                                                   1
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                                    0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRVd_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                                     0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d)                                                   ((base) + 0x000004a4 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_RMSK                                                                 0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_MAXd                                                                    1
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                                      0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRVd_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                                       0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d)                                                 ((base) + 0x000004a8 + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_RMSK                                                               0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_MAXd                                                                  1
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                                  0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRVd_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                                   0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d)                                                  ((base) + 0x000004ac + 0x10000 * (d))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_RMSK                                                                0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_MAXd                                                                   1
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                                    0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRVd_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                                     0x0

#define HWIO_SEQ_MEM_m_DRV0_ADDR(base,m)                                                                        ((base) + 0x00000600 + 0x4 * (m))
#define HWIO_SEQ_MEM_m_DRV0_RMSK                                                                                0xffffffff
#define HWIO_SEQ_MEM_m_DRV0_MAXm                                                                                        79
#define HWIO_SEQ_MEM_m_DRV0_INI(base,m)        \
        in_dword_masked(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m), HWIO_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_SEQ_MEM_m_DRV0_INMI(base,m,mask)    \
        in_dword_masked(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m), mask)
#define HWIO_SEQ_MEM_m_DRV0_OUTI(base,m,val)    \
        out_dword(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m),val)
#define HWIO_SEQ_MEM_m_DRV0_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m),mask,val,HWIO_SEQ_MEM_m_DRV0_INI(base,m))
#define HWIO_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                                       0xffffffff
#define HWIO_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                              0x0

#define HWIO_SEQ_MEM_m_DRVd_ADDR(base,d,m)                                                                      ((base) + 0x00000600 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_SEQ_MEM_m_DRVd_RMSK                                                                                0xffffffff
#define HWIO_SEQ_MEM_m_DRVd_MAXd                                                                                         1
#define HWIO_SEQ_MEM_m_DRVd_MAXm                                                                                        79
#define HWIO_SEQ_MEM_m_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_SEQ_MEM_m_DRVd_ADDR(base,d,m), HWIO_SEQ_MEM_m_DRVd_RMSK)
#define HWIO_SEQ_MEM_m_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_SEQ_MEM_m_DRVd_ADDR(base,d,m), mask)
#define HWIO_SEQ_MEM_m_DRVd_CMD_WORD_BMSK                                                                       0xffffffff
#define HWIO_SEQ_MEM_m_DRVd_CMD_WORD_SHFT                                                                              0x0

#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x)                                                               ((x) + 0x00000d00)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK                                                                        0x3f
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN(x)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x), HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x), m)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUT(x, v)      \
        out_dword(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x),v)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_ADDR(x),m,v,HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_IN(x))
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_BMSK                                              0x3f
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV0_AMC_COMPLETION_IRQ_ENABLE_SHFT                                               0x0

#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x)                                                               ((x) + 0x00000d04)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK                                                                        0x3f
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_IN(x)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x), HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_RMSK)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_ADDR(x), m)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_BMSK                                              0x3f
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV0_AMC_COMPLETION_IRQ_STATUS_SHFT                                               0x0

#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR(x)                                                                ((x) + 0x00000d08)
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_RMSK                                                                         0x3f
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_OUT(x, v)      \
        out_dword(HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_ADDR(x),v)
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_BMSK                                                0x3f
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV0_AMC_COMPLETION_IRQ_CLEAR_SHFT                                                 0x0

#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m)                                                           ((base) + 0x00000d10 + 0x2A0 * (m))
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK                                                                       0xffff
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_MAXm                                                                            5
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m), HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m), mask)
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m),val)
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_ADDR(base,m),mask,val,HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_INI(base,m))
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                                     0xffff
#define HWIO_TCSm_DRV0_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                                        0x0

#define HWIO_TCSm_DRV0_CONTROL_ADDR(base,m)                                                                     ((base) + 0x00000d14 + 0x2A0 * (m))
#define HWIO_TCSm_DRV0_CONTROL_RMSK                                                                              0x1010000
#define HWIO_TCSm_DRV0_CONTROL_MAXm                                                                                      5
#define HWIO_TCSm_DRV0_CONTROL_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV0_CONTROL_ADDR(base,m), HWIO_TCSm_DRV0_CONTROL_RMSK)
#define HWIO_TCSm_DRV0_CONTROL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV0_CONTROL_ADDR(base,m), mask)
#define HWIO_TCSm_DRV0_CONTROL_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV0_CONTROL_ADDR(base,m),val)
#define HWIO_TCSm_DRV0_CONTROL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV0_CONTROL_ADDR(base,m),mask,val,HWIO_TCSm_DRV0_CONTROL_INI(base,m))
#define HWIO_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_BMSK                                                             0x1000000
#define HWIO_TCSm_DRV0_CONTROL_AMC_MODE_TRIGGER_SHFT                                                                  0x18
#define HWIO_TCSm_DRV0_CONTROL_AMC_MODE_EN_BMSK                                                                    0x10000
#define HWIO_TCSm_DRV0_CONTROL_AMC_MODE_EN_SHFT                                                                       0x10

#define HWIO_TCSm_DRV0_STATUS_ADDR(base,m)                                                                      ((base) + 0x00000d18 + 0x2A0 * (m))
#define HWIO_TCSm_DRV0_STATUS_RMSK                                                                                     0x1
#define HWIO_TCSm_DRV0_STATUS_MAXm                                                                                       5
#define HWIO_TCSm_DRV0_STATUS_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV0_STATUS_ADDR(base,m), HWIO_TCSm_DRV0_STATUS_RMSK)
#define HWIO_TCSm_DRV0_STATUS_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV0_STATUS_ADDR(base,m), mask)
#define HWIO_TCSm_DRV0_STATUS_CONTROLLER_IDLE_BMSK                                                                     0x1
#define HWIO_TCSm_DRV0_STATUS_CONTROLLER_IDLE_SHFT                                                                     0x0

#define HWIO_TCSm_DRV0_CMD_ENABLE_ADDR(base,m)                                                                  ((base) + 0x00000d1c + 0x2A0 * (m))
#define HWIO_TCSm_DRV0_CMD_ENABLE_RMSK                                                                              0xffff
#define HWIO_TCSm_DRV0_CMD_ENABLE_MAXm                                                                                   5
#define HWIO_TCSm_DRV0_CMD_ENABLE_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV0_CMD_ENABLE_ADDR(base,m), HWIO_TCSm_DRV0_CMD_ENABLE_RMSK)
#define HWIO_TCSm_DRV0_CMD_ENABLE_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV0_CMD_ENABLE_ADDR(base,m), mask)
#define HWIO_TCSm_DRV0_CMD_ENABLE_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV0_CMD_ENABLE_ADDR(base,m),val)
#define HWIO_TCSm_DRV0_CMD_ENABLE_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV0_CMD_ENABLE_ADDR(base,m),mask,val,HWIO_TCSm_DRV0_CMD_ENABLE_INI(base,m))
#define HWIO_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                                   0xffff
#define HWIO_TCSm_DRV0_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                                      0x0

#define HWIO_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n)                                                                ((base) + 0x00000d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV0_MSGID_RMSK                                                                             0x1010f
#define HWIO_TCSm_CMDn_DRV0_MSGID_MAXm                                                                                   5
#define HWIO_TCSm_CMDn_DRV0_MSGID_MAXn                                                                                  15
#define HWIO_TCSm_CMDn_DRV0_MSGID_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV0_MSGID_RMSK)
#define HWIO_TCSm_CMDn_DRV0_MSGID_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV0_MSGID_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV0_MSGID_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV0_MSGID_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV0_MSGID_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_BMSK                                                               0x10000
#define HWIO_TCSm_CMDn_DRV0_MSGID_READ_OR_WRITE_SHFT                                                                  0x10
#define HWIO_TCSm_CMDn_DRV0_MSGID_RES_REQ_BMSK                                                                       0x100
#define HWIO_TCSm_CMDn_DRV0_MSGID_RES_REQ_SHFT                                                                         0x8
#define HWIO_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_BMSK                                                                      0xf
#define HWIO_TCSm_CMDn_DRV0_MSGID_MSG_LENGTH_SHFT                                                                      0x0

#define HWIO_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n)                                                                 ((base) + 0x00000d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV0_ADDR_RMSK                                                                              0x7ffff
#define HWIO_TCSm_CMDn_DRV0_ADDR_MAXm                                                                                    5
#define HWIO_TCSm_CMDn_DRV0_ADDR_MAXn                                                                                   15
#define HWIO_TCSm_CMDn_DRV0_ADDR_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV0_ADDR_RMSK)
#define HWIO_TCSm_CMDn_DRV0_ADDR_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV0_ADDR_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV0_ADDR_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV0_ADDR_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV0_ADDR_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV0_ADDR_SLV_ID_BMSK                                                                       0x70000
#define HWIO_TCSm_CMDn_DRV0_ADDR_SLV_ID_SHFT                                                                          0x10
#define HWIO_TCSm_CMDn_DRV0_ADDR_OFFSET_BMSK                                                                        0xffff
#define HWIO_TCSm_CMDn_DRV0_ADDR_OFFSET_SHFT                                                                           0x0

#define HWIO_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n)                                                                 ((base) + 0x00000d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV0_DATA_RMSK                                                                           0xffffffff
#define HWIO_TCSm_CMDn_DRV0_DATA_MAXm                                                                                    5
#define HWIO_TCSm_CMDn_DRV0_DATA_MAXn                                                                                   15
#define HWIO_TCSm_CMDn_DRV0_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV0_DATA_RMSK)
#define HWIO_TCSm_CMDn_DRV0_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV0_DATA_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV0_DATA_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV0_DATA_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV0_DATA_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV0_DATA_DATA_BMSK                                                                      0xffffffff
#define HWIO_TCSm_CMDn_DRV0_DATA_DATA_SHFT                                                                             0x0

#define HWIO_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n)                                                               ((base) + 0x00000d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV0_STATUS_RMSK                                                                            0x10101
#define HWIO_TCSm_CMDn_DRV0_STATUS_MAXm                                                                                  5
#define HWIO_TCSm_CMDn_DRV0_STATUS_MAXn                                                                                 15
#define HWIO_TCSm_CMDn_DRV0_STATUS_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV0_STATUS_RMSK)
#define HWIO_TCSm_CMDn_DRV0_STATUS_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_STATUS_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV0_STATUS_COMPLETED_BMSK                                                                  0x10000
#define HWIO_TCSm_CMDn_DRV0_STATUS_COMPLETED_SHFT                                                                     0x10
#define HWIO_TCSm_CMDn_DRV0_STATUS_ISSUED_BMSK                                                                       0x100
#define HWIO_TCSm_CMDn_DRV0_STATUS_ISSUED_SHFT                                                                         0x8
#define HWIO_TCSm_CMDn_DRV0_STATUS_TRIGGERED_BMSK                                                                      0x1
#define HWIO_TCSm_CMDn_DRV0_STATUS_TRIGGERED_SHFT                                                                      0x0

#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n)                                                   ((base) + 0x00000d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK                                                             0xffffffff
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXm                                                                      5
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_MAXn                                                                     15
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_RMSK)
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                          0xffffffff
#define HWIO_TCSm_CMDn_DRV0_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                                 0x0

#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x)                                                               ((x) + 0x00010d00)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_RMSK                                                                         0x3
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_IN(x)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x), HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_RMSK)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_INM(x, m)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x), m)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_OUT(x, v)      \
        out_dword(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x),v)
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_ADDR(x),m,v,HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_IN(x))
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_AMC_COMPLETION_IRQ_ENABLE_BMSK                                               0x3
#define HWIO_TCS_AMC_MODE_IRQ_ENABLE_DRV1_AMC_COMPLETION_IRQ_ENABLE_SHFT                                               0x0

#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x)                                                               ((x) + 0x00010d04)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_RMSK                                                                         0x3
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_IN(x)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x), HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_RMSK)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_INM(x, m)      \
        in_dword_masked(HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_ADDR(x), m)
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_AMC_COMPLETION_IRQ_STATUS_BMSK                                               0x3
#define HWIO_TCS_AMC_MODE_IRQ_STATUS_DRV1_AMC_COMPLETION_IRQ_STATUS_SHFT                                               0x0

#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_ADDR(x)                                                                ((x) + 0x00010d08)
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_RMSK                                                                          0x3
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_OUT(x, v)      \
        out_dword(HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_ADDR(x),v)
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_AMC_COMPLETION_IRQ_CLEAR_BMSK                                                 0x3
#define HWIO_TCS_AMC_MODE_IRQ_CLEAR_DRV1_AMC_COMPLETION_IRQ_CLEAR_SHFT                                                 0x0

#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m)                                                           ((base) + 0x00010d10 + 0x2A0 * (m))
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_RMSK                                                                       0xffff
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_MAXm                                                                            1
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m), HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_RMSK)
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m), mask)
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m),val)
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_ADDR(base,m),mask,val,HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_INI(base,m))
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK                                                     0xffff
#define HWIO_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT                                                        0x0

#define HWIO_TCSm_DRV1_CONTROL_ADDR(base,m)                                                                     ((base) + 0x00010d14 + 0x2A0 * (m))
#define HWIO_TCSm_DRV1_CONTROL_RMSK                                                                              0x1010000
#define HWIO_TCSm_DRV1_CONTROL_MAXm                                                                                      1
#define HWIO_TCSm_DRV1_CONTROL_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV1_CONTROL_ADDR(base,m), HWIO_TCSm_DRV1_CONTROL_RMSK)
#define HWIO_TCSm_DRV1_CONTROL_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV1_CONTROL_ADDR(base,m), mask)
#define HWIO_TCSm_DRV1_CONTROL_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV1_CONTROL_ADDR(base,m),val)
#define HWIO_TCSm_DRV1_CONTROL_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV1_CONTROL_ADDR(base,m),mask,val,HWIO_TCSm_DRV1_CONTROL_INI(base,m))
#define HWIO_TCSm_DRV1_CONTROL_AMC_MODE_TRIGGER_BMSK                                                             0x1000000
#define HWIO_TCSm_DRV1_CONTROL_AMC_MODE_TRIGGER_SHFT                                                                  0x18
#define HWIO_TCSm_DRV1_CONTROL_AMC_MODE_EN_BMSK                                                                    0x10000
#define HWIO_TCSm_DRV1_CONTROL_AMC_MODE_EN_SHFT                                                                       0x10

#define HWIO_TCSm_DRV1_STATUS_ADDR(base,m)                                                                      ((base) + 0x00010d18 + 0x2A0 * (m))
#define HWIO_TCSm_DRV1_STATUS_RMSK                                                                                     0x1
#define HWIO_TCSm_DRV1_STATUS_MAXm                                                                                       1
#define HWIO_TCSm_DRV1_STATUS_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV1_STATUS_ADDR(base,m), HWIO_TCSm_DRV1_STATUS_RMSK)
#define HWIO_TCSm_DRV1_STATUS_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV1_STATUS_ADDR(base,m), mask)
#define HWIO_TCSm_DRV1_STATUS_CONTROLLER_IDLE_BMSK                                                                     0x1
#define HWIO_TCSm_DRV1_STATUS_CONTROLLER_IDLE_SHFT                                                                     0x0

#define HWIO_TCSm_DRV1_CMD_ENABLE_ADDR(base,m)                                                                  ((base) + 0x00010d1c + 0x2A0 * (m))
#define HWIO_TCSm_DRV1_CMD_ENABLE_RMSK                                                                              0xffff
#define HWIO_TCSm_DRV1_CMD_ENABLE_MAXm                                                                                   1
#define HWIO_TCSm_DRV1_CMD_ENABLE_INI(base,m)        \
        in_dword_masked(HWIO_TCSm_DRV1_CMD_ENABLE_ADDR(base,m), HWIO_TCSm_DRV1_CMD_ENABLE_RMSK)
#define HWIO_TCSm_DRV1_CMD_ENABLE_INMI(base,m,mask)    \
        in_dword_masked(HWIO_TCSm_DRV1_CMD_ENABLE_ADDR(base,m), mask)
#define HWIO_TCSm_DRV1_CMD_ENABLE_OUTI(base,m,val)    \
        out_dword(HWIO_TCSm_DRV1_CMD_ENABLE_ADDR(base,m),val)
#define HWIO_TCSm_DRV1_CMD_ENABLE_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_DRV1_CMD_ENABLE_ADDR(base,m),mask,val,HWIO_TCSm_DRV1_CMD_ENABLE_INI(base,m))
#define HWIO_TCSm_DRV1_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_BMSK                                                   0xffff
#define HWIO_TCSm_DRV1_CMD_ENABLE_INDIVIDUAL_CMD_ENABLE_MASK_SHFT                                                      0x0

#define HWIO_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n)                                                                ((base) + 0x00010d30 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV1_MSGID_RMSK                                                                             0x1010f
#define HWIO_TCSm_CMDn_DRV1_MSGID_MAXm                                                                                   1
#define HWIO_TCSm_CMDn_DRV1_MSGID_MAXn                                                                                  15
#define HWIO_TCSm_CMDn_DRV1_MSGID_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV1_MSGID_RMSK)
#define HWIO_TCSm_CMDn_DRV1_MSGID_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV1_MSGID_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV1_MSGID_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV1_MSGID_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV1_MSGID_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_BMSK                                                               0x10000
#define HWIO_TCSm_CMDn_DRV1_MSGID_READ_OR_WRITE_SHFT                                                                  0x10
#define HWIO_TCSm_CMDn_DRV1_MSGID_RES_REQ_BMSK                                                                       0x100
#define HWIO_TCSm_CMDn_DRV1_MSGID_RES_REQ_SHFT                                                                         0x8
#define HWIO_TCSm_CMDn_DRV1_MSGID_MSG_LENGTH_BMSK                                                                      0xf
#define HWIO_TCSm_CMDn_DRV1_MSGID_MSG_LENGTH_SHFT                                                                      0x0

#define HWIO_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n)                                                                 ((base) + 0x00010d34 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV1_ADDR_RMSK                                                                              0x7ffff
#define HWIO_TCSm_CMDn_DRV1_ADDR_MAXm                                                                                    1
#define HWIO_TCSm_CMDn_DRV1_ADDR_MAXn                                                                                   15
#define HWIO_TCSm_CMDn_DRV1_ADDR_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV1_ADDR_RMSK)
#define HWIO_TCSm_CMDn_DRV1_ADDR_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV1_ADDR_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV1_ADDR_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV1_ADDR_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV1_ADDR_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV1_ADDR_SLV_ID_BMSK                                                                       0x70000
#define HWIO_TCSm_CMDn_DRV1_ADDR_SLV_ID_SHFT                                                                          0x10
#define HWIO_TCSm_CMDn_DRV1_ADDR_OFFSET_BMSK                                                                        0xffff
#define HWIO_TCSm_CMDn_DRV1_ADDR_OFFSET_SHFT                                                                           0x0

#define HWIO_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n)                                                                 ((base) + 0x00010d38 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV1_DATA_RMSK                                                                           0xffffffff
#define HWIO_TCSm_CMDn_DRV1_DATA_MAXm                                                                                    1
#define HWIO_TCSm_CMDn_DRV1_DATA_MAXn                                                                                   15
#define HWIO_TCSm_CMDn_DRV1_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV1_DATA_RMSK)
#define HWIO_TCSm_CMDn_DRV1_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV1_DATA_OUTI2(base,m,n,val)    \
        out_dword(HWIO_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n),val)
#define HWIO_TCSm_CMDn_DRV1_DATA_OUTMI2(base,m,n,mask,val) \
        out_dword_masked_ns(HWIO_TCSm_CMDn_DRV1_DATA_ADDR(base,m,n),mask,val,HWIO_TCSm_CMDn_DRV1_DATA_INI2(base,m,n))
#define HWIO_TCSm_CMDn_DRV1_DATA_DATA_BMSK                                                                      0xffffffff
#define HWIO_TCSm_CMDn_DRV1_DATA_DATA_SHFT                                                                             0x0

#define HWIO_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n)                                                               ((base) + 0x00010d3c + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV1_STATUS_RMSK                                                                            0x10101
#define HWIO_TCSm_CMDn_DRV1_STATUS_MAXm                                                                                  1
#define HWIO_TCSm_CMDn_DRV1_STATUS_MAXn                                                                                 15
#define HWIO_TCSm_CMDn_DRV1_STATUS_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV1_STATUS_RMSK)
#define HWIO_TCSm_CMDn_DRV1_STATUS_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_STATUS_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV1_STATUS_COMPLETED_BMSK                                                                  0x10000
#define HWIO_TCSm_CMDn_DRV1_STATUS_COMPLETED_SHFT                                                                     0x10
#define HWIO_TCSm_CMDn_DRV1_STATUS_ISSUED_BMSK                                                                       0x100
#define HWIO_TCSm_CMDn_DRV1_STATUS_ISSUED_SHFT                                                                         0x8
#define HWIO_TCSm_CMDn_DRV1_STATUS_TRIGGERED_BMSK                                                                      0x1
#define HWIO_TCSm_CMDn_DRV1_STATUS_TRIGGERED_SHFT                                                                      0x0

#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n)                                                   ((base) + 0x00010d40 + 0x2A0 * (m) + 0x14 * (n))
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_RMSK                                                             0xffffffff
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_MAXm                                                                      1
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_MAXn                                                                     15
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_INI2(base,m,n)        \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n), HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_RMSK)
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_INMI2(base,m,n,mask)    \
        in_dword_masked(HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_ADDR(base,m,n), mask)
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_READ_RESPONSE_DATA_BMSK                                          0xffffffff
#define HWIO_TCSm_CMDn_DRV1_READ_RESPONSE_DATA_READ_RESPONSE_DATA_SHFT                                                 0x0


#endif /* __PDC_SEQ_HWIO_H__ */
