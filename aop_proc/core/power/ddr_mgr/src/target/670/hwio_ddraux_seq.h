#ifndef __HWIO_DDRAUX_SEQ__
#define __HWIO_DDRAUX_SEQ__
/*
===========================================================================
*/
/**
  @file hwio_ddraux_seq
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r28.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_DDRAUX_DDR_AUX_SEQ_MEM

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_ddraux_seq.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: DDR_AUX_SEQ_MEM
 *--------------------------------------------------------------------------*/

#define DDR_AUX_SEQ_MEM_REG_BASE                                                          (AOSS_BASE      + 0x00c00200)

#define HWIO_SEQ_BRANCH_EVENT_ADDR                                                        (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000000)
#define HWIO_SEQ_BRANCH_EVENT_RMSK                                                            0xffff
#define HWIO_SEQ_BRANCH_EVENT_ATTR                                                               0x3
#define HWIO_SEQ_BRANCH_EVENT_IN          \
        in_dword_masked(HWIO_SEQ_BRANCH_EVENT_ADDR, HWIO_SEQ_BRANCH_EVENT_RMSK)
#define HWIO_SEQ_BRANCH_EVENT_INM(m)      \
        in_dword_masked(HWIO_SEQ_BRANCH_EVENT_ADDR, m)
#define HWIO_SEQ_BRANCH_EVENT_OUT(v)      \
        out_dword(HWIO_SEQ_BRANCH_EVENT_ADDR,v)
#define HWIO_SEQ_BRANCH_EVENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SEQ_BRANCH_EVENT_ADDR,m,v,HWIO_SEQ_BRANCH_EVENT_IN)
#define HWIO_SEQ_BRANCH_EVENT_BRANCH_EVENT_BMSK                                               0xffff
#define HWIO_SEQ_BRANCH_EVENT_BRANCH_EVENT_SHFT                                                  0x0
#define HWIO_SEQ_BRANCH_EVENT_FREQ_BMSK                                                       0x0007
#define HWIO_SEQ_BRANCH_EVENT_FREQ_SHFT                                                          0x0
#define HWIO_SEQ_BRANCH_EVENT_VOLTAGE_BMSK                                                    0x0070
#define HWIO_SEQ_BRANCH_EVENT_VOLTAGE_SHFT                                                       0x4
#define HWIO_SEQ_BRANCH_EVENT_SHRM_BMSK                                                       0x0700
#define HWIO_SEQ_BRANCH_EVENT_SHRM_SHFT                                                          0x8
#define HWIO_SEQ_BRANCH_EVENT_DCC_BMSK                                                        0x1000
#define HWIO_SEQ_BRANCH_EVENT_DCC_SHFT                                                           0xC
#define HWIO_SEQ_BRANCH_EVENT_SHRM_CLK_EN_BMSK                                                0xC000
#define HWIO_SEQ_BRANCH_EVENT_SHRM_CLK_EN_SHFT                                                   0xe

#define HWIO_SEQ_GCC_CLKPLAN_ADDR                                                         (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000008)
#define HWIO_SEQ_GCC_CLKPLAN_RMSK                                                              0xfff
#define HWIO_SEQ_GCC_CLKPLAN_ATTR                                                                0x3
#define HWIO_SEQ_GCC_CLKPLAN_IN          \
        in_dword_masked(HWIO_SEQ_GCC_CLKPLAN_ADDR, HWIO_SEQ_GCC_CLKPLAN_RMSK)
#define HWIO_SEQ_GCC_CLKPLAN_INM(m)      \
        in_dword_masked(HWIO_SEQ_GCC_CLKPLAN_ADDR, m)
#define HWIO_SEQ_GCC_CLKPLAN_OUT(v)      \
        out_dword(HWIO_SEQ_GCC_CLKPLAN_ADDR,v)
#define HWIO_SEQ_GCC_CLKPLAN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SEQ_GCC_CLKPLAN_ADDR,m,v,HWIO_SEQ_GCC_CLKPLAN_IN)
#define HWIO_SEQ_GCC_CLKPLAN_SHRM_CP_BMSK                                                      0xf00
#define HWIO_SEQ_GCC_CLKPLAN_SHRM_CP_SHFT                                                        0x8
#define HWIO_SEQ_GCC_CLKPLAN_SHUB_CP_BMSK                                                       0xf0
#define HWIO_SEQ_GCC_CLKPLAN_SHUB_CP_SHFT                                                        0x4
#define HWIO_SEQ_GCC_CLKPLAN_MC_CP_BMSK                                                          0xf
#define HWIO_SEQ_GCC_CLKPLAN_MC_CP_SHFT                                                          0x0

#define HWIO_SEQn_DBG_STEP_ADDR(n)                                                        (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000000c + 0x4 * (n))

#define HWIO_SEQ_GCC_CLKPLAN_ADDR                                                         (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000008)
#define HWIO_SEQ_GCC_CLKPLAN_RMSK                                                              0xfff
#define HWIO_SEQ_GCC_CLKPLAN_ATTR                                                                0x3
#define HWIO_SEQ_GCC_CLKPLAN_IN          \
        in_dword_masked(HWIO_SEQ_GCC_CLKPLAN_ADDR, HWIO_SEQ_GCC_CLKPLAN_RMSK)
#define HWIO_SEQ_GCC_CLKPLAN_INM(m)      \
        in_dword_masked(HWIO_SEQ_GCC_CLKPLAN_ADDR, m)
#define HWIO_SEQ_GCC_CLKPLAN_OUT(v)      \
        out_dword(HWIO_SEQ_GCC_CLKPLAN_ADDR,v)
#define HWIO_SEQ_GCC_CLKPLAN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SEQ_GCC_CLKPLAN_ADDR,m,v,HWIO_SEQ_GCC_CLKPLAN_IN)
#define HWIO_SEQ_GCC_CLKPLAN_SHRM_CP_BMSK                                                      0xf00
#define HWIO_SEQ_GCC_CLKPLAN_SHRM_CP_SHFT                                                        0x8
#define HWIO_SEQ_GCC_CLKPLAN_SHUB_CP_BMSK                                                       0xf0
#define HWIO_SEQ_GCC_CLKPLAN_SHUB_CP_SHFT                                                        0x4
#define HWIO_SEQ_GCC_CLKPLAN_MC_CP_BMSK                                                          0xf
#define HWIO_SEQ_GCC_CLKPLAN_MC_CP_SHFT                                                          0x0

#define HWIO_SEQn_DBG_STEP_ADDR(n)                                                        (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000000c + 0x4 * (n))
#define HWIO_SEQn_DBG_STEP_RMSK                                                                  0x1
#define HWIO_SEQn_DBG_STEP_MAXn                                                                    1
#define HWIO_SEQn_DBG_STEP_ATTR                                                                  0x0
#define HWIO_SEQn_DBG_STEP_OUTI(n,val)    \
        out_dword(HWIO_SEQn_DBG_STEP_ADDR(n),val)
#define HWIO_SEQn_DBG_STEP_DBG_STEP_TRIGGER_BMSK                                                 0x1
#define HWIO_SEQn_DBG_STEP_DBG_STEP_TRIGGER_SHFT                                                 0x0

#define HWIO_SEQn_DBG_CONTINUE_ADDR(n)                                                    (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000014 + 0x4 * (n))
#define HWIO_SEQn_DBG_CONTINUE_RMSK                                                              0x1
#define HWIO_SEQn_DBG_CONTINUE_MAXn                                                                1
#define HWIO_SEQn_DBG_CONTINUE_ATTR                                                              0x0
#define HWIO_SEQn_DBG_CONTINUE_OUTI(n,val)    \
        out_dword(HWIO_SEQn_DBG_CONTINUE_ADDR(n),val)
#define HWIO_SEQn_DBG_CONTINUE_DBG_STEP_CONTINUE_BMSK                                            0x1
#define HWIO_SEQn_DBG_CONTINUE_DBG_STEP_CONTINUE_SHFT                                            0x0

#define HWIO_SEQn_DBG_STAT_ADDR(n)                                                        (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000001c + 0x4 * (n))
#define HWIO_SEQn_DBG_STAT_RMSK                                                                  0x1
#define HWIO_SEQn_DBG_STAT_MAXn                                                                    1
#define HWIO_SEQn_DBG_STAT_ATTR                                                                  0x1
#define HWIO_SEQn_DBG_STAT_INI(n)        \
        in_dword_masked(HWIO_SEQn_DBG_STAT_ADDR(n), HWIO_SEQn_DBG_STAT_RMSK)
#define HWIO_SEQn_DBG_STAT_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_DBG_STAT_ADDR(n), mask)
#define HWIO_SEQn_DBG_STAT_DBG_STEPPING_STATUS_BMSK                                              0x1
#define HWIO_SEQn_DBG_STAT_DBG_STEPPING_STATUS_SHFT                                              0x0

#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_ADDR(n)                                             (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000024 + 0x4 * (n))
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_RMSK                                                0x800001ff
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_MAXn                                                         1
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_ATTR                                                       0x3
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_INI(n)        \
        in_dword_masked(HWIO_SEQn_DBG_BREAKPOINT_ADDR_ADDR(n), HWIO_SEQn_DBG_BREAKPOINT_ADDR_RMSK)
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_DBG_BREAKPOINT_ADDR_ADDR(n), mask)
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_OUTI(n,val)    \
        out_dword(HWIO_SEQn_DBG_BREAKPOINT_ADDR_ADDR(n),val)
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_DBG_BREAKPOINT_ADDR_ADDR(n),mask,val,HWIO_SEQn_DBG_BREAKPOINT_ADDR_INI(n))
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_BMSK                           0x80000000
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_VALID_SHFT                                 0x1f
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_BMSK                                 0x1ff
#define HWIO_SEQn_DBG_BREAKPOINT_ADDR_DBG_BREAKPOINT_ADDR_SHFT                                   0x0

#define HWIO_SEQn_CFG_ADDR(n)                                                             (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000002c + 0x4 * (n))
#define HWIO_SEQn_CFG_RMSK                                                                     0x3ff
#define HWIO_SEQn_CFG_MAXn                                                                         1
#define HWIO_SEQn_CFG_ATTR                                                                       0x0
#define HWIO_SEQn_CFG_INI(n)        \
        in_dword_masked(HWIO_SEQn_CFG_ADDR(n), HWIO_SEQn_CFG_RMSK)
#define HWIO_SEQn_CFG_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_CFG_ADDR(n), mask)
#define HWIO_SEQn_CFG_OUTI(n,val)    \
        out_dword(HWIO_SEQn_CFG_ADDR(n),val)
#define HWIO_SEQn_CFG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_CFG_ADDR(n),mask,val,HWIO_SEQn_CFG_INI(n))
#define HWIO_SEQn_CFG_SEQ_START_BMSK                                                           0x200
#define HWIO_SEQn_CFG_SEQ_START_SHFT                                                             0x9
#define HWIO_SEQn_CFG_SEQ_START_ADDR_BMSK                                                      0x1ff
#define HWIO_SEQn_CFG_SEQ_START_ADDR_SHFT                                                        0x0

#define HWIO_SEQn_STAT_ADDR(n)                                                            (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000034 + 0x4 * (n))
#define HWIO_SEQn_STAT_RMSK                                                                    0x3ff
#define HWIO_SEQn_STAT_MAXn                                                                        1
#define HWIO_SEQn_STAT_ATTR                                                                      0x1
#define HWIO_SEQn_STAT_INI(n)        \
        in_dword_masked(HWIO_SEQn_STAT_ADDR(n), HWIO_SEQn_STAT_RMSK)
#define HWIO_SEQn_STAT_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_STAT_ADDR(n), mask)
#define HWIO_SEQn_STAT_SEQ_BUSY_BMSK                                                           0x200
#define HWIO_SEQn_STAT_SEQ_BUSY_SHFT                                                             0x9
#define HWIO_SEQn_STAT_SEQ_PC_BMSK                                                             0x1ff
#define HWIO_SEQn_STAT_SEQ_PC_SHFT                                                               0x0

#define HWIO_SEQ_VCD_SEL_ADDR                                                             (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000003c)
#define HWIO_SEQ_VCD_SEL_RMSK                                                                    0x3
#define HWIO_SEQ_VCD_SEL_ATTR                                                                    0x1
#define HWIO_SEQ_VCD_SEL_IN          \
        in_dword_masked(HWIO_SEQ_VCD_SEL_ADDR, HWIO_SEQ_VCD_SEL_RMSK)
#define HWIO_SEQ_VCD_SEL_INM(m)      \
        in_dword_masked(HWIO_SEQ_VCD_SEL_ADDR, m)
#define HWIO_SEQ_VCD_SEL_VCD_BMSK                                                                0x3
#define HWIO_SEQ_VCD_SEL_VCD_SHFT                                                                0x0

#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ADDR(n)                                  (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000044 + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_RMSK                                     0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_MAXn                                              1
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ATTR                                            0x3
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ADDR(n), HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_RMSK)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_OUTI(n,val)    \
        out_dword(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ADDR(n),val)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_ADDR(n),mask,val,HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_INI(n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_MASK_BMSK                                0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_MASK_MASK_SHFT                                       0x0

#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ADDR(n)                                 (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000004c + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_RMSK                                    0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_MAXn                                             1
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ATTR                                           0x3
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ADDR(n), HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_RMSK)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_OUTI(n,val)    \
        out_dword(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ADDR(n),val)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_ADDR(n),mask,val,HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_INI(n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_MASK_BMSK                               0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_MASK_MASK_SHFT                                      0x0

#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ADDR(n)                                   (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000054 + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_RMSK                                      0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_MAXn                                               1
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ATTR                                             0x3
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ADDR(n), HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_RMSK)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_OUTI(n,val)    \
        out_dword(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ADDR(n),val)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_ADDR(n),mask,val,HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_INI(n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_OVERRIDE_VAL_BMSK                         0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_31_TO_0_VAL_OVERRIDE_VAL_SHFT                                0x0

#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ADDR(n)                                  (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000005c + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_RMSK                                     0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_MAXn                                              1
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ATTR                                            0x3
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ADDR(n), HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_RMSK)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_OUTI(n,val)    \
        out_dword(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ADDR(n),val)
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_ADDR(n),mask,val,HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_INI(n))
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_OVERRIDE_VAL_BMSK                        0xffffffff
#define HWIO_SEQn_PWR_CTRL_OVRD_BIT_63_TO_32_VAL_OVERRIDE_VAL_SHFT                               0x0

#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ADDR(n)                                (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000064 + 0x4 * (n))
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_RMSK                                   0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_MAXn                                            1
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ATTR                                          0x3
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_INI(n)        \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ADDR(n), HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_RMSK)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ADDR(n), mask)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_OUTI(n,val)    \
        out_dword(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ADDR(n),val)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_ADDR(n),mask,val,HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_INI(n))
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_MASK_BMSK                              0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_MASK_MASK_SHFT                                     0x0

#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ADDR(n)                               (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000006c + 0x4 * (n))
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_RMSK                                  0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_MAXn                                           1
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ATTR                                         0x3
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_INI(n)        \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ADDR(n), HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_RMSK)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ADDR(n), mask)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_OUTI(n,val)    \
        out_dword(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ADDR(n),val)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_ADDR(n),mask,val,HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_INI(n))
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_MASK_BMSK                             0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_MASK_MASK_SHFT                                    0x0

#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ADDR(n)                                 (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000074 + 0x4 * (n))
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_RMSK                                    0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_MAXn                                             1
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ATTR                                           0x3
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_INI(n)        \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ADDR(n), HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_RMSK)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ADDR(n), mask)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_OUTI(n,val)    \
        out_dword(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ADDR(n),val)
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_ADDR(n),mask,val,HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_INI(n))
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_OVERRIDE_VAL_BMSK                       0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_31_TO_0_OVRD_VAL_OVERRIDE_VAL_SHFT                              0x0

#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ADDR(n)                                (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000007c + 0x4 * (n))
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_RMSK                                   0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_MAXn                                            1
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ATTR                                          0x3
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_INI(n)        \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ADDR(n), HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_RMSK)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ADDR(n), mask)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_OUTI(n,val)    \
        out_dword(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ADDR(n),val)
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_ADDR(n),mask,val,HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_INI(n))
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_OVERRIDE_VAL_BMSK                      0xffffffff
#define HWIO_SEQn_WAIT_EVENT_BIT_63_TO_32_OVRD_VAL_OVERRIDE_VAL_SHFT                             0x0

#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ADDR(n)                                          (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000084 + 0x4 * (n))
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_RMSK                                                 0xffff
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_MAXn                                                      1
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ATTR                                                    0x3
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_INI(n)        \
        in_dword_masked(HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ADDR(n), HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_RMSK)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ADDR(n), mask)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_OUTI(n,val)    \
        out_dword(HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ADDR(n),val)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_ADDR(n),mask,val,HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_INI(n))
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_MASK_BMSK                                            0xffff
#define HWIO_SEQn_BRANCH_EVENT_OVRD_MASK_MASK_SHFT                                               0x0

#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ADDR(n)                                           (DDR_AUX_SEQ_MEM_REG_BASE      + 0x0000008c + 0x4 * (n))
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_RMSK                                                  0xffff
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_MAXn                                                       1
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ATTR                                                     0x3
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_INI(n)        \
        in_dword_masked(HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ADDR(n), HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_RMSK)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ADDR(n), mask)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_OUTI(n,val)    \
        out_dword(HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ADDR(n),val)
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_ADDR(n),mask,val,HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_INI(n))
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_OVERRIDE_VAL_BMSK                                     0xffff
#define HWIO_SEQn_BRANCH_EVENT_OVRD_VAL_OVERRIDE_VAL_SHFT                                        0x0

#define HWIO_SEQ_CFG_DELAY_VAL_q_ADDR(q)                                                  (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000098 + 0x4 * (q))
#define HWIO_SEQ_CFG_DELAY_VAL_q_RMSK                                                        0xfffff
#define HWIO_SEQ_CFG_DELAY_VAL_q_MAXq                                                              3
#define HWIO_SEQ_CFG_DELAY_VAL_q_ATTR                                                            0x3
#define HWIO_SEQ_CFG_DELAY_VAL_q_INI(q)        \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_VAL_q_ADDR(q), HWIO_SEQ_CFG_DELAY_VAL_q_RMSK)
#define HWIO_SEQ_CFG_DELAY_VAL_q_INMI(q,mask)    \
        in_dword_masked(HWIO_SEQ_CFG_DELAY_VAL_q_ADDR(q), mask)
#define HWIO_SEQ_CFG_DELAY_VAL_q_OUTI(q,val)    \
        out_dword(HWIO_SEQ_CFG_DELAY_VAL_q_ADDR(q),val)
#define HWIO_SEQ_CFG_DELAY_VAL_q_OUTMI(q,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_CFG_DELAY_VAL_q_ADDR(q),mask,val,HWIO_SEQ_CFG_DELAY_VAL_q_INI(q))
#define HWIO_SEQ_CFG_DELAY_VAL_q_DELAY_VAL_BMSK                                              0xfffff
#define HWIO_SEQ_CFG_DELAY_VAL_q_DELAY_VAL_SHFT                                                  0x0

#define HWIO_BRANCH_ADDRq_ADDR(q)                                                         (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000108 + 0x4 * (q))
#define HWIO_BRANCH_ADDRq_RMSK                                                                 0x1ff
#define HWIO_BRANCH_ADDRq_MAXq                                                                    63
#define HWIO_BRANCH_ADDRq_ATTR                                                                   0x3
#define HWIO_BRANCH_ADDRq_INI(q)        \
        in_dword_masked(HWIO_BRANCH_ADDRq_ADDR(q), HWIO_BRANCH_ADDRq_RMSK)
#define HWIO_BRANCH_ADDRq_INMI(q,mask)    \
        in_dword_masked(HWIO_BRANCH_ADDRq_ADDR(q), mask)
#define HWIO_BRANCH_ADDRq_OUTI(q,val)    \
        out_dword(HWIO_BRANCH_ADDRq_ADDR(q),val)
#define HWIO_BRANCH_ADDRq_OUTMI(q,mask,val) \
        out_dword_masked_ns(HWIO_BRANCH_ADDRq_ADDR(q),mask,val,HWIO_BRANCH_ADDRq_INI(q))
#define HWIO_BRANCH_ADDRq_BRANCH_ADDR_BMSK                                                     0x1ff
#define HWIO_BRANCH_ADDRq_BRANCH_ADDR_SHFT                                                       0x0

#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_ADDR(n)                                       (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000208 + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_RMSK                                          0xffffffff
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_MAXn                                                   1
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_ATTR                                                 0x1
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_ADDR(n), HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_RMSK)
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_PWR_CTRL_31_TO_0_BMSK                         0xffffffff
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_31_TO_0_PWR_CTRL_31_TO_0_SHFT                                0x0

#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_ADDR(n)                                      (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000210 + 0x4 * (n))
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_RMSK                                         0xffffffff
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_MAXn                                                  1
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_ATTR                                                0x1
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_INI(n)        \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_ADDR(n), HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_RMSK)
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_INMI(n,mask)    \
        in_dword_masked(HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_ADDR(n), mask)
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_PWR_CTRL_63_TO_32_BMSK                       0xffffffff
#define HWIO_SEQn_PWR_CTRL_STAT_BIT_63_TO_32_PWR_CTRL_63_TO_32_SHFT                              0x0

#define HWIO_SEQ_MEMq_ADDR(q)                                                             (DDR_AUX_SEQ_MEM_REG_BASE      + 0x00000218 + 0x4 * (q))
#define HWIO_SEQ_MEMq_RMSK                                                                0xffffffff
#define HWIO_SEQ_MEMq_MAXq                                                                       127
#define HWIO_SEQ_MEMq_ATTR                                                                       0x3
#define HWIO_SEQ_MEMq_INI(q)        \
        in_dword_masked(HWIO_SEQ_MEMq_ADDR(q), HWIO_SEQ_MEMq_RMSK)
#define HWIO_SEQ_MEMq_INMI(q,mask)    \
        in_dword_masked(HWIO_SEQ_MEMq_ADDR(q), mask)
#define HWIO_SEQ_MEMq_OUTI(q,val)    \
        out_dword(HWIO_SEQ_MEMq_ADDR(q),val)
#define HWIO_SEQ_MEMq_OUTMI(q,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_MEMq_ADDR(q),mask,val,HWIO_SEQ_MEMq_INI(q))
#define HWIO_SEQ_MEMq_WORD_BMSK                                                           0xffffffff
#define HWIO_SEQ_MEMq_WORD_SHFT                                                                  0x0


#endif /* __HWIO_DDRAUX_SEQ__ */
