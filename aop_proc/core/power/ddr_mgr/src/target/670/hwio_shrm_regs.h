#ifndef __HWIO_SHRM_REGS_H__
#define __HWIO_SHRM_REGS_H__
/*
===========================================================================
*/
/**
  @file hwio_shrm_regs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    SHRM_CSR_SHRM_CSR


  Generation parameters: 
  { u'filename': u'hwio_shrm_regs.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'SHRM_CSR_SHRM_CSR'],
    u'output-fvals': True,
    u'output-offsets': True}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_shrm_regs.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SHRM_CSR_SHRM_CSR
 *--------------------------------------------------------------------------*/

#define SHRM_CSR_SHRM_CSR_REG_BASE                                                              (DDR_SS_BASE      + 0x002d0000)
#define SHRM_CSR_SHRM_CSR_REG_BASE_OFFS                                                         0x002d0000

#define HWIO_SHRM_CSR_HW_INFO_ADDR                                                              (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000000)
#define HWIO_SHRM_CSR_HW_INFO_OFFS                                                              (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_SHRM_CSR_HW_INFO_RMSK                                                              0xffffffff
#define HWIO_SHRM_CSR_HW_INFO_IN          \
        in_dword_masked(HWIO_SHRM_CSR_HW_INFO_ADDR, HWIO_SHRM_CSR_HW_INFO_RMSK)
#define HWIO_SHRM_CSR_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_HW_INFO_ADDR, m)
#define HWIO_SHRM_CSR_HW_INFO_MAJOR_REVISION_BMSK                                               0xff000000
#define HWIO_SHRM_CSR_HW_INFO_MAJOR_REVISION_SHFT                                                     0x18
#define HWIO_SHRM_CSR_HW_INFO_BRANCH_REVISION_BMSK                                                0xff0000
#define HWIO_SHRM_CSR_HW_INFO_BRANCH_REVISION_SHFT                                                    0x10
#define HWIO_SHRM_CSR_HW_INFO_MINOR_REVISION_BMSK                                                   0xff00
#define HWIO_SHRM_CSR_HW_INFO_MINOR_REVISION_SHFT                                                      0x8
#define HWIO_SHRM_CSR_HW_INFO_ECO_REVISION_BMSK                                                       0xff
#define HWIO_SHRM_CSR_HW_INFO_ECO_REVISION_SHFT                                                        0x0

#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000004)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RMSK                                                  0x11111
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_ADDR, HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RMSK)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_ADDR,m,v,HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IN)
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_GLOBAL_CGC_BMSK                                       0x10000
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_GLOBAL_CGC_SHFT                                          0x10
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_GLOBAL_CGC_DISABLE_FVAL                                   0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_GLOBAL_CGC_ENABLE_FVAL                                    0x1
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IRAM_BMSK                                              0x1000
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IRAM_SHFT                                                 0xc
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IRAM_DISABLE_FVAL                                         0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_IRAM_ENABLE_FVAL                                          0x1
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_DRAM_BMSK                                               0x100
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_DRAM_SHFT                                                 0x8
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_DRAM_DISABLE_FVAL                                         0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_DRAM_ENABLE_FVAL                                          0x1
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_SPROC_BMSK                                               0x10
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_SPROC_SHFT                                                0x4
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_SPROC_DISABLE_FVAL                                        0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_SPROC_ENABLE_FVAL                                         0x1
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RCI_BMSK                                                  0x1
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RCI_SHFT                                                  0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RCI_DISABLE_FVAL                                          0x0
#define HWIO_SHRM_CSR_SHRM_CGC_OVERRIDE_CTRL_RCI_ENABLE_FVAL                                           0x1

#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_ADDR                                                    (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000008)
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_OFFS                                                    (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000008)
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_RMSK                                                           0x1
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SPROC_STATUS_ADDR, HWIO_SHRM_CSR_SHRM_SPROC_STATUS_RMSK)
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SPROC_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_STATE_BMSK                                                     0x1
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_STATE_SHFT                                                     0x0
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_STATE_RUN_STATE_FVAL                                           0x0
#define HWIO_SHRM_CSR_SHRM_SPROC_STATUS_STATE_WAITI_STATE_FVAL                                         0x1

#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_ADDR                                                      (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000000c)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_OFFS                                                      (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_RMSK                                                      0xffffffff
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SW_LOGGING_ADDR, HWIO_SHRM_CSR_SHRM_SW_LOGGING_RMSK)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SW_LOGGING_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_SW_LOGGING_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_SW_LOGGING_ADDR,m,v,HWIO_SHRM_CSR_SHRM_SW_LOGGING_IN)
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_VALUE_BMSK                                                0xffffffff
#define HWIO_SHRM_CSR_SHRM_SW_LOGGING_VALUE_SHFT                                                       0x0

#define HWIO_SHRM_CSR_SHRM_RSI_PATH_ADDR                                                        (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000010)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_OFFS                                                        (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000010)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_RMSK                                                               0x1
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RSI_PATH_ADDR, HWIO_SHRM_CSR_SHRM_RSI_PATH_RMSK)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RSI_PATH_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_RSI_PATH_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_RSI_PATH_ADDR,m,v,HWIO_SHRM_CSR_SHRM_RSI_PATH_IN)
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_SEL_BMSK                                                           0x1
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_SEL_SHFT                                                           0x0
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_SEL_RSI_OVERRIDE_PATH_FVAL                                         0x0
#define HWIO_SHRM_CSR_SHRM_RSI_PATH_SEL_RSI_PATH_FVAL                                                  0x1

#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_ADDR                                                (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000020)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_OFFS                                                (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000020)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_RMSK                                                       0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_IN          \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_ADDR, HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_RMSK)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_ADDR, m)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_ADDR,v)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_ADDR,m,v,HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_IN)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_EN_BMSK                                                    0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_EN_SHFT                                                    0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_EN_DISABLE_FVAL                                            0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_EN_EN_ENABLE_FVAL                                             0x1

#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_ADDR                                           (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000024)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_OFFS                                           (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000024)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_RMSK                                                  0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_IN          \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_ADDR, HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_RMSK)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_ADDR, m)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_ADDR,v)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_ADDR,m,v,HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_IN)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_TRIGGER_BMSK                                          0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_TRIGGER_SHFT                                          0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_TRIGGER_CLR_FVAL                                      0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_TRIGGER_TRIGGER_SET_FVAL                                      0x1

#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000028)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000028)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_RMSK                                               0xffffffff
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_IN          \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_ADDR, HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_RMSK)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_ADDR, m)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_ADDR,v)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_ADDR,m,v,HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_IN)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_CMD_PAYLOAD_BMSK                                   0xffffffff
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_CMD_CMD_PAYLOAD_SHFT                                          0x0

#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_ADDR                                            (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000001c)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_OFFS                                            (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_RMSK                                                   0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_ADDR, HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_RMSK)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_STATUS_BMSK                                            0x1
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_STATUS_SHFT                                            0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_STATUS_COMPLETED_FVAL                                  0x0
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_STATUS_STATUS_PROCESSING_FVAL                                 0x1

#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_ADDR                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000002c)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_OFFS                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_RMSK                                          0xffffffff
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_ADDR, HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_RMSK)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_ADDR, m)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_ADDR,v)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_ADDR,m,v,HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_IN)
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_RSP_PAYLOAD_BMSK                              0xffffffff
#define HWIO_SHRM_CSR_AOP2SHRM_INTERRUPT_RESPONSE_RSP_PAYLOAD_SHFT                                     0x0

#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_ADDR                                                (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000040)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_OFFS                                                (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000040)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_RMSK                                                       0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_ADDR, HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_RMSK)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_ADDR,m,v,HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_IN)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_EN_BMSK                                                    0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_EN_SHFT                                                    0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_EN_DISABLE_FVAL                                            0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_EN_EN_ENABLE_FVAL                                             0x1

#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_ADDR                                           (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000044)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_OFFS                                           (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000044)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_RMSK                                                  0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_ADDR, HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_RMSK)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_ADDR,m,v,HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_IN)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_TRIGGER_BMSK                                          0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_TRIGGER_SHFT                                          0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_TRIGGER_CLR_FVAL                                      0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_TRIGGER_TRIGGER_SET_FVAL                                      0x1

#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000048)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000048)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_RMSK                                               0xffffffff
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_ADDR, HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_RMSK)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_ADDR,m,v,HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_IN)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_CMD_PAYLOAD_BMSK                                   0xffffffff
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_CMD_CMD_PAYLOAD_SHFT                                          0x0

#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_ADDR                                            (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000050)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_OFFS                                            (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000050)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_RMSK                                                   0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_ADDR, HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_RMSK)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_STATUS_BMSK                                            0x1
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_STATUS_SHFT                                            0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_STATUS_COMPLETED_FVAL                                  0x0
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_STATUS_STATUS_PROCESSING_FVAL                                 0x1

#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_ADDR                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000004c)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_OFFS                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000004c)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_RMSK                                          0xffffffff
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_ADDR, HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_RMSK)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_ADDR,m,v,HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_IN)
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_RSP_PAYLOAD_BMSK                              0xffffffff
#define HWIO_SHRM_CSR_SHRM2AOP_INTERRUPT_RESPONSE_RSP_PAYLOAD_SHFT                                     0x0

#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_ADDR                                            (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000060)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_OFFS                                            (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000060)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_RMSK                                                   0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_ADDR, HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_RMSK)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_ADDR,m,v,HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_IN)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_EN_BMSK                                                0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_EN_SHFT                                                0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_EN_DISABLE_FVAL                                        0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN_EN_ENABLE_FVAL                                         0x1

#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_ADDR                                       (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000064)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_OFFS                                       (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000064)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_RMSK                                              0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_ADDR, HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_RMSK)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_ADDR,m,v,HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_IN)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_TRIGGER_BMSK                                      0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_TRIGGER_SHFT                                      0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_TRIGGER_CLR_FVAL                                  0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER_TRIGGER_SET_FVAL                                  0x1

#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_ADDR                                           (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000068)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_OFFS                                           (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000068)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_RMSK                                           0xffffffff
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_ADDR, HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_RMSK)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_ADDR,m,v,HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_IN)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_CMD_PAYLOAD_BMSK                               0xffffffff
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD_CMD_PAYLOAD_SHFT                                      0x0

#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_ADDR                                      (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000006c)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_OFFS                                      (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000006c)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_RMSK                                      0xffffffff
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_ADDR, HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_RMSK)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_ADDR,m,v,HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_IN)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_RSP_PAYLOAD_BMSK                          0xffffffff
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_RESPONSE_RSP_PAYLOAD_SHFT                                 0x0

#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_ADDR                                        (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000070)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_OFFS                                        (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000070)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_RMSK                                               0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_ADDR, HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_RMSK)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_STATUS_BMSK                                        0x1
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_STATUS_SHFT                                        0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_STATUS_COMPLETED_FVAL                              0x0
#define HWIO_SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_STATUS_STATUS_PROCESSING_FVAL                             0x1

#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_ADDR                                                  (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000074)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_OFFS                                                  (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000074)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_RMSK                                                         0x1
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_ADDR, HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_RMSK)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_ADDR,m,v,HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_IN)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_EN_BMSK                                                      0x1
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_EN_SHFT                                                      0x0
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_EN_DISABLE_FVAL                                              0x0
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_CTRL_EN_ENSABLE_FVAL                                              0x1

#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_ADDR                                                (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000078)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_OFFS                                                (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000078)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_RMSK                                                  0xffffff
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_ADDR, HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_RMSK)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_STATUS_BMSK                                           0xff0000
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_STATUS_SHFT                                               0x10
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_PC_BMSK                                                 0xffff
#define HWIO_SHRM_CSR_SHRM_APB_DEBUG_STATUS_PC_SHFT                                                    0x0

#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_ADDR(n)                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000080 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_OFFS(n)                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000080 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_RMSK                                                  0x101
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_MAXn                                                      1
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_ADDR(n), HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_RMSK)
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_ADDR(n),val)
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_ADDR(n),mask,val,HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_INI(n))
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_CLR_BMSK                                              0x100
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_CLR_SHFT                                                0x8
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_RUN_STOP_BMSK                                           0x1
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTL_RUN_STOP_SHFT                                           0x0

#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_ADDR(n)                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000090 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_OFFS(n)                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000090 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_RMSK                                             0xffffffff
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_MAXn                                                      1
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_ADDR(n), HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_RMSK)
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_VALUE_BMSK                                       0xffffffff
#define HWIO_SHRM_CSR_SHRM_PERF_COUNTER_n_CNTR_VALUE_SHFT                                              0x0

#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ADDR                                                    (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x000000a0)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_OFFS                                                    (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x000000a0)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_RMSK                                                    0xffffffff
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ADDR, HWIO_SHRM_CSR_SHRM_QOS_ENCODING_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ADDR,m,v,HWIO_SHRM_CSR_SHRM_QOS_ENCODING_IN)
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ACV_CTRL_BMSK                                           0xff000000
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ACV_CTRL_SHFT                                                 0x18
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ALC_CTRL_BMSK                                             0xff0000
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ALC_CTRL_SHFT                                                 0x10
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ACV_BMSK                                                    0xff00
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ACV_SHFT                                                       0x8
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ALC_BMSK                                                      0xff
#define HWIO_SHRM_CSR_SHRM_QOS_ENCODING_ALC_SHFT                                                       0x0

#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_ADDR                                      (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x000000a4)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_OFFS                                      (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x000000a4)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_RMSK                                          0x1111
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_ADDR, HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_ADDR,m,v,HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_IN)
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_THROTTLE_POLICY_OVERRIDE_EN_BMSK              0x1000
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_THROTTLE_POLICY_OVERRIDE_EN_SHFT                 0xc
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_MEMNOC_POLICY_OVERRIDE_EN_BMSK                 0x100
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_MEMNOC_POLICY_OVERRIDE_EN_SHFT                   0x8
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_MC_POLICY_OVERRIDE_EN_BMSK                      0x10
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_MC_POLICY_OVERRIDE_EN_SHFT                       0x4
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_LLCC_POLICY_OVERRIDE_EN_BMSK                     0x1
#define HWIO_SHRM_CSR_SHRM_QOS_OUTPUT_POLICY_OVERRIDE_LLCC_POLICY_OVERRIDE_EN_SHFT                     0x0

#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_ADDR                                              (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x000000b0)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_OFFS                                              (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x000000b0)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_RMSK                                                0xff00ff
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_ADDR, HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_RMSK)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_ADDR,m,v,HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_IN)
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_SHRM_BMSK                                           0xff0000
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_SHRM_SHFT                                               0x10
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_CFG_BMSK                                                0xff
#define HWIO_SHRM_CSR_SHRM_CGC_THRESHOLD_CTRL_CFG_SHFT                                                 0x0

#define HWIO_SHRM_CSR_SCRATCH_n_REG_ADDR(n)                                                     (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000100 + 0x4 * (n))
#define HWIO_SHRM_CSR_SCRATCH_n_REG_OFFS(n)                                                     (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000100 + 0x4 * (n))
#define HWIO_SHRM_CSR_SCRATCH_n_REG_RMSK                                                        0xffffffff
#define HWIO_SHRM_CSR_SCRATCH_n_REG_MAXn                                                                15
#define HWIO_SHRM_CSR_SCRATCH_n_REG_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SCRATCH_n_REG_ADDR(n), HWIO_SHRM_CSR_SCRATCH_n_REG_RMSK)
#define HWIO_SHRM_CSR_SCRATCH_n_REG_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SCRATCH_n_REG_ADDR(n), mask)
#define HWIO_SHRM_CSR_SCRATCH_n_REG_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SCRATCH_n_REG_ADDR(n),val)
#define HWIO_SHRM_CSR_SCRATCH_n_REG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SCRATCH_n_REG_ADDR(n),mask,val,HWIO_SHRM_CSR_SCRATCH_n_REG_INI(n))
#define HWIO_SHRM_CSR_SCRATCH_n_REG_DATA_BMSK                                                   0xffffffff
#define HWIO_SHRM_CSR_SCRATCH_n_REG_DATA_SHFT                                                          0x0

#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_ADDR(n,m)                                       (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000400 + 0x20 * (n) + 0x4 * (m))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_OFFS(n,m)                                       (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000400 + 0x20 * (n) + 0x4 * (m))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_RMSK                                            0x70000f0f
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MAXn                                                    15
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MAXm                                                     3
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_INI2(n,m)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_ADDR(n,m), HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_ADDR(n,m), mask)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_OUTI2(n,m,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_ADDR(n,m),val)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_ADDR(n,m),mask,val,HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_INI2(n,m))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_OPERATION_BMSK                                  0x70000000
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_OPERATION_SHFT                                        0x1c
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MASK_BMSK                                            0xf00
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MASK_SHFT                                              0x8
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MATCH_BMSK                                             0xf
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_LUTm_INPUT_MATCH_SHFT                                             0x0

#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_ADDR(n)                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_OFFS(n)                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000800 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_RMSK                                     0xf030f
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_MAXn                                          15
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_ADDR(n), HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_ADDR(n),val)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_ADDR(n),mask,val,HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_INI(n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_POLICY_BMSK                              0xf0000
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_POLICY_SHFT                                 0x10
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_MODE_BMSK                                  0x300
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_MODE_SHFT                                    0x8
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_VALUE_BMSK                                   0xf
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_THROTTLE_CNTL_VALUE_SHFT                                   0x0

#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_ADDR(n)                                 (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000840 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_OFFS(n)                                 (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000840 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_RMSK                                       0xf030f
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_MAXn                                            15
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_ADDR(n), HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_ADDR(n),val)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_ADDR(n),mask,val,HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_INI(n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_POLICY_BMSK                                0xf0000
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_POLICY_SHFT                                   0x10
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_MODE_BMSK                                    0x300
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_MODE_SHFT                                      0x8
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_VALUE_BMSK                                     0xf
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MEMNOC_CNTL_VALUE_SHFT                                     0x0

#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_ADDR(n)                                     (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000880 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_OFFS(n)                                     (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000880 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_RMSK                                           0xf030f
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_MAXn                                                15
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_ADDR(n), HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_ADDR(n),val)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_ADDR(n),mask,val,HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_INI(n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_POLICY_BMSK                                    0xf0000
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_POLICY_SHFT                                       0x10
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_MODE_BMSK                                        0x300
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_MODE_SHFT                                          0x8
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_VALUE_BMSK                                         0xf
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_MC_CNTL_VALUE_SHFT                                         0x0

#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_ADDR(n)                                   (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x000008c0 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_OFFS(n)                                   (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x000008c0 + 0x4 * (n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_RMSK                                         0xf030f
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_MAXn                                              15
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_INI(n)        \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_ADDR(n), HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_RMSK)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_INMI(n,mask)    \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_ADDR(n), mask)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_OUTI(n,val)    \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_ADDR(n),val)
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_ADDR(n),mask,val,HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_INI(n))
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_POLICY_BMSK                                  0xf0000
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_POLICY_SHFT                                     0x10
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_MODE_BMSK                                      0x300
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_MODE_SHFT                                        0x8
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_VALUE_BMSK                                       0xf
#define HWIO_SHRM_CSR_SHRM_QOS_INSTn_OUTPUT_LLCC_CNTL_VALUE_SHFT                                       0x0

#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_ADDR                                                        (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000900)
#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_OFFS                                                        (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000900)
#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_RMSK                                                               0x1
#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_QOS_CTRL_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_LOAD_PARAMETERS_BMSK                                               0x1
#define HWIO_SHRM_CSR_SHRM_QOS_CTRL_LOAD_PARAMETERS_SHFT                                               0x0

#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_ADDR                                                      (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001000)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_OFFS                                                      (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001000)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_RMSK                                                           0x101
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SPROC_CTRL_ADDR, HWIO_SHRM_CSR_SHRM_SPROC_CTRL_RMSK)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SPROC_CTRL_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_SPROC_CTRL_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_SPROC_CTRL_ADDR,m,v,HWIO_SHRM_CSR_SHRM_SPROC_CTRL_IN)
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_START_VEC_SEL_BMSK                                             0x100
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_START_VEC_SEL_SHFT                                               0x8
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_RUN_STALL_BMSK                                                   0x1
#define HWIO_SHRM_CSR_SHRM_SPROC_CTRL_RUN_STALL_SHFT                                                   0x0

#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001020)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001020)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_RMSK                                                      0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_IN          \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_ADDR, HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_RMSK)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_ADDR, m)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_ADDR,v)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_ADDR,m,v,HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_IN)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_EN_BMSK                                                   0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_EN_SHFT                                                   0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_EN_DISABLE_FVAL                                           0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_EN_EN_ENABLE_FVAL                                            0x1

#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_ADDR                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001024)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_OFFS                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001024)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_RMSK                                                 0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_IN          \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_ADDR, HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_RMSK)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_ADDR, m)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_ADDR,v)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_ADDR,m,v,HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_IN)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_TRIGGER_BMSK                                         0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_TRIGGER_SHFT                                         0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_TRIGGER_CLR_FVAL                                     0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_TRIGGER_TRIGGER_SET_FVAL                                     0x1

#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_ADDR                                              (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001028)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_OFFS                                              (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001028)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_RMSK                                              0xffffffff
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_IN          \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_ADDR, HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_RMSK)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_ADDR, m)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_ADDR,v)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_ADDR,m,v,HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_IN)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_CMD_PAYLOAD_BMSK                                  0xffffffff
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_CMD_CMD_PAYLOAD_SHFT                                         0x0

#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_ADDR                                           (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00000030)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_OFFS                                           (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00000030)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_RMSK                                                  0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_ADDR, HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_RMSK)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_STATUS_BMSK                                           0x1
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_STATUS_SHFT                                           0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_STATUS_COMPLETED_FVAL                                 0x0
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_STATUS_STATUS_PROCESSING_FVAL                                0x1

#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_ADDR                                         (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000102c)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_OFFS                                         (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000102c)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_RMSK                                         0xffffffff
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_ADDR, HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_RMSK)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_ADDR, m)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_ADDR,v)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_ADDR,m,v,HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_IN)
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_RSP_PAYLOAD_BMSK                             0xffffffff
#define HWIO_SHRM_CSR_APPS2SHRM_INTERRUPT_RESPONSE_RSP_PAYLOAD_SHFT                                    0x0

#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001040)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001040)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_RMSK                                                      0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_ADDR, HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_RMSK)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_ADDR,m,v,HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_IN)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_EN_BMSK                                                   0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_EN_SHFT                                                   0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_EN_DISABLE_FVAL                                           0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_EN_EN_ENABLE_FVAL                                            0x1

#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_ADDR                                          (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001044)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_OFFS                                          (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001044)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_RMSK                                                 0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_ADDR, HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_RMSK)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_ADDR,m,v,HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_IN)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_TRIGGER_BMSK                                         0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_TRIGGER_SHFT                                         0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_TRIGGER_CLR_FVAL                                     0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_TRIGGER_TRIGGER_SET_FVAL                                     0x1

#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_ADDR                                              (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001048)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_OFFS                                              (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001048)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_RMSK                                              0xffffffff
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_ADDR, HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_RMSK)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_ADDR,m,v,HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_IN)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_CMD_PAYLOAD_BMSK                                  0xffffffff
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_CMD_CMD_PAYLOAD_SHFT                                         0x0

#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_ADDR                                           (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x0000104c)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_OFFS                                           (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x0000104c)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_RMSK                                                  0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_ADDR, HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_RMSK)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_STATUS_BMSK                                           0x1
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_STATUS_SHFT                                           0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_STATUS_COMPLETED_FVAL                                 0x0
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_STATUS_STATUS_PROCESSING_FVAL                                0x1

#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_ADDR                                         (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001050)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_OFFS                                         (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001050)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_RMSK                                         0xffffffff
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_ADDR, HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_RMSK)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_ADDR, m)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_ADDR,v)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_ADDR,m,v,HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_IN)
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_RSP_PAYLOAD_BMSK                             0xffffffff
#define HWIO_SHRM_CSR_SHRM2APPS_INTERRUPT_RESPONSE_RSP_PAYLOAD_SHFT                                    0x0

#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_ADDR                                                      (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001080)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_OFFS                                                      (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001080)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_RMSK                                                             0x1
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_TRIG_ADDR, HWIO_SHRM_CSR_SHRM_SWRST_TRIG_RMSK)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_TRIG_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_SWRST_TRIG_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_SWRST_TRIG_ADDR,m,v,HWIO_SHRM_CSR_SHRM_SWRST_TRIG_IN)
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_SWRESET_ENA_BMSK                                                 0x1
#define HWIO_SHRM_CSR_SHRM_SWRST_TRIG_SWRESET_ENA_SHFT                                                 0x0

#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_ADDR                                               (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001084)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_OFFS                                               (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001084)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_RMSK                                               0xffffffff
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_ADDR, HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_RMSK)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_ADDR,m,v,HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_IN)
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_SWRESET_CLK_EN_BMSK                                0xffffffff
#define HWIO_SHRM_CSR_SHRM_SWRST_CLKGATE_SEQ_SWRESET_CLK_EN_SHFT                                       0x0

#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_ADDR                                                   (SHRM_CSR_SHRM_CSR_REG_BASE      + 0x00001088)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_OFFS                                                   (SHRM_CSR_SHRM_CSR_REG_BASE_OFFS + 0x00001088)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_RMSK                                                   0xffffffff
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_IN          \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_ADDR, HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_RMSK)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_INM(m)      \
        in_dword_masked(HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_ADDR, m)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_OUT(v)      \
        out_dword(HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_ADDR,v)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_ADDR,m,v,HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_IN)
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_SWRESET_ARES_SET_BMSK                                  0xffffffff
#define HWIO_SHRM_CSR_SHRM_SWRST_RES_SEQ_SWRESET_ARES_SET_SHFT                                         0x0


#endif /* __HWIO_SHRM_REGS_H__ */
