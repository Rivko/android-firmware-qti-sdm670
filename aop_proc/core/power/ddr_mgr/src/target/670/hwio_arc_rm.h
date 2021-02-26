#ifndef __HWIO_ARC_RM_H__
#define __HWIO_ARC_RM_H__
/*
===========================================================================
*/
/**
  @file hwio_arc_rm.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r28.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_ARC_ARC_RM

  'Include' filters applied: 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_arc_rm.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: ARC_RM
 *--------------------------------------------------------------------------*/

#define ARC_RM_REG_BASE                                                                          (AOSS_BASE      + 0x007e0000)

#define HWIO_RMm_CTRL_ADDR(m)                                                                    (ARC_RM_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_RMm_CTRL_RMSK                                                                          0xf0f01
#define HWIO_RMm_CTRL_MAXm                                                                                8
#define HWIO_RMm_CTRL_INI(m)        \
        in_dword_masked(HWIO_RMm_CTRL_ADDR(m), HWIO_RMm_CTRL_RMSK)
#define HWIO_RMm_CTRL_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_CTRL_ADDR(m), mask)
#define HWIO_RMm_CTRL_OUTI(m,val)    \
        out_dword(HWIO_RMm_CTRL_ADDR(m),val)
#define HWIO_RMm_CTRL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_CTRL_ADDR(m),mask,val,HWIO_RMm_CTRL_INI(m))
#define HWIO_RMm_CTRL_MOL_BMSK                                                                      0xf0000
#define HWIO_RMm_CTRL_MOL_SHFT                                                                         0x10
#define HWIO_RMm_CTRL_RET_OL_BMSK                                                                     0xf00
#define HWIO_RMm_CTRL_RET_OL_SHFT                                                                       0x8
#define HWIO_RMm_CTRL_RM_EN_BMSK                                                                        0x1
#define HWIO_RMm_CTRL_RM_EN_SHFT                                                                        0x0

#define HWIO_RMm_AGGR_BLOCK_EN_ADDR(m)                                                           (ARC_RM_REG_BASE      + 0x00000040 + 0x4 * (m))
#define HWIO_RMm_AGGR_BLOCK_EN_RMSK                                                                     0x1
#define HWIO_RMm_AGGR_BLOCK_EN_MAXm                                                                       8
#define HWIO_RMm_AGGR_BLOCK_EN_INI(m)        \
        in_dword_masked(HWIO_RMm_AGGR_BLOCK_EN_ADDR(m), HWIO_RMm_AGGR_BLOCK_EN_RMSK)
#define HWIO_RMm_AGGR_BLOCK_EN_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_AGGR_BLOCK_EN_ADDR(m), mask)
#define HWIO_RMm_AGGR_BLOCK_EN_OUTI(m,val)    \
        out_dword(HWIO_RMm_AGGR_BLOCK_EN_ADDR(m),val)
#define HWIO_RMm_AGGR_BLOCK_EN_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_AGGR_BLOCK_EN_ADDR(m),mask,val,HWIO_RMm_AGGR_BLOCK_EN_INI(m))
#define HWIO_RMm_AGGR_BLOCK_EN_AGGR_BLOCK_EN_BMSK                                                       0x1
#define HWIO_RMm_AGGR_BLOCK_EN_AGGR_BLOCK_EN_SHFT                                                       0x0

#define HWIO_RMm_AGGR_UNBLOCK_ADDR(m)                                                            (ARC_RM_REG_BASE      + 0x00000080 + 0x4 * (m))
#define HWIO_RMm_AGGR_UNBLOCK_RMSK                                                                      0xf
#define HWIO_RMm_AGGR_UNBLOCK_MAXm                                                                        8
#define HWIO_RMm_AGGR_UNBLOCK_INI(m)        \
        in_dword_masked(HWIO_RMm_AGGR_UNBLOCK_ADDR(m), HWIO_RMm_AGGR_UNBLOCK_RMSK)
#define HWIO_RMm_AGGR_UNBLOCK_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_AGGR_UNBLOCK_ADDR(m), mask)
#define HWIO_RMm_AGGR_UNBLOCK_OUTI(m,val)    \
        out_dword(HWIO_RMm_AGGR_UNBLOCK_ADDR(m),val)
#define HWIO_RMm_AGGR_UNBLOCK_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_AGGR_UNBLOCK_ADDR(m),mask,val,HWIO_RMm_AGGR_UNBLOCK_INI(m))
#define HWIO_RMm_AGGR_UNBLOCK_RM_AGGR_UNBLOCK_DATA_BMSK                                                 0xf
#define HWIO_RMm_AGGR_UNBLOCK_RM_AGGR_UNBLOCK_DATA_SHFT                                                 0x0

#define HWIO_RMm_OPERATIONAL_LEVEL_ADDR(m)                                                       (ARC_RM_REG_BASE      + 0x000000c0 + 0x4 * (m))
#define HWIO_RMm_OPERATIONAL_LEVEL_RMSK                                                          0xfffff000
#define HWIO_RMm_OPERATIONAL_LEVEL_MAXm                                                                   8
#define HWIO_RMm_OPERATIONAL_LEVEL_INI(m)        \
        in_dword_masked(HWIO_RMm_OPERATIONAL_LEVEL_ADDR(m), HWIO_RMm_OPERATIONAL_LEVEL_RMSK)
#define HWIO_RMm_OPERATIONAL_LEVEL_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_OPERATIONAL_LEVEL_ADDR(m), mask)
#define HWIO_RMm_OPERATIONAL_LEVEL_OUTI(m,val)    \
        out_dword(HWIO_RMm_OPERATIONAL_LEVEL_ADDR(m),val)
#define HWIO_RMm_OPERATIONAL_LEVEL_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_OPERATIONAL_LEVEL_ADDR(m),mask,val,HWIO_RMm_OPERATIONAL_LEVEL_INI(m))
#define HWIO_RMm_OPERATIONAL_LEVEL_DESTINATION_OL_BMSK                                           0xf0000000
#define HWIO_RMm_OPERATIONAL_LEVEL_DESTINATION_OL_SHFT                                                 0x1c
#define HWIO_RMm_OPERATIONAL_LEVEL_SEQUENCE_OL_BMSK                                               0xf000000
#define HWIO_RMm_OPERATIONAL_LEVEL_SEQUENCE_OL_SHFT                                                    0x18
#define HWIO_RMm_OPERATIONAL_LEVEL_AGGREGATED_OL_BMSK                                              0xf00000
#define HWIO_RMm_OPERATIONAL_LEVEL_AGGREGATED_OL_SHFT                                                  0x14
#define HWIO_RMm_OPERATIONAL_LEVEL_SOLVED_OL_BMSK                                                   0xf0000
#define HWIO_RMm_OPERATIONAL_LEVEL_SOLVED_OL_SHFT                                                      0x10
#define HWIO_RMm_OPERATIONAL_LEVEL_CURRENT_OL_BMSK                                                   0xf000
#define HWIO_RMm_OPERATIONAL_LEVEL_CURRENT_OL_SHFT                                                      0xc

#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_ADDR(m)                                               (ARC_RM_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_RMSK                                                  0xffffffff
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_MAXm                                                           8
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_INI(m)        \
        in_dword_masked(HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_ADDR(m), HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_RMSK)
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_ADDR(m), mask)
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_MIN_IDLE_TIME_LOW_BMSK                                0xffffffff
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_LOW_MIN_IDLE_TIME_LOW_SHFT                                       0x0

#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_ADDR(m)                                              (ARC_RM_REG_BASE      + 0x00000140 + 0x4 * (m))
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_RMSK                                                   0xffffff
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_MAXm                                                          8
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_INI(m)        \
        in_dword_masked(HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_ADDR(m), HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_RMSK)
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_ADDR(m), mask)
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_MIN_IDLE_TIME_HIGH_BMSK                                0xffffff
#define HWIO_RMm_MIN_IDLE_TIME_SOLVING_HIGH_MIN_IDLE_TIME_HIGH_SHFT                                     0x0

#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_ADDR(m)                                                 (ARC_RM_REG_BASE      + 0x00000180 + 0x4 * (m))
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_RMSK                                                    0xffffffff
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_MAXm                                                             8
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_INI(m)        \
        in_dword_masked(HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_ADDR(m), HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_RMSK)
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_ADDR(m), mask)
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_WAKEUP_TIMER_VALUE_LOW_BMSK                             0xffffffff
#define HWIO_RMm_WAKEUP_TIMER_STATUS_LOW_WAKEUP_TIMER_VALUE_LOW_SHFT                                    0x0

#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_ADDR(m)                                                (ARC_RM_REG_BASE      + 0x000001c0 + 0x4 * (m))
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_RMSK                                                   0xe0ffffff
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_MAXm                                                            8
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_INI(m)        \
        in_dword_masked(HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_ADDR(m), HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_RMSK)
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_ADDR(m), mask)
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_ENABLE_BMSK                               0x80000000
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_ENABLE_SHFT                                     0x1f
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_EXPIRE_BMSK                               0x40000000
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_EXPIRE_SHFT                                     0x1e
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_CONSIDERED_BMSK                           0x20000000
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_CONSIDERED_SHFT                                 0x1d
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_VALUE_HIGH_BMSK                             0xffffff
#define HWIO_RMm_WAKEUP_TIMER_STATUS_HIGH_WAKEUP_TIMER_VALUE_HIGH_SHFT                                  0x0

#define HWIO_RMm_SEQ_UP_BR_ADDRi_ADDR(m,i)                                                       (ARC_RM_REG_BASE      + 0x00001240 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SEQ_UP_BR_ADDRi_RMSK                                                                 0xfff
#define HWIO_RMm_SEQ_UP_BR_ADDRi_MAXm                                                                     8
#define HWIO_RMm_SEQ_UP_BR_ADDRi_MAXi                                                                    15
#define HWIO_RMm_SEQ_UP_BR_ADDRi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SEQ_UP_BR_ADDRi_ADDR(m,i), HWIO_RMm_SEQ_UP_BR_ADDRi_RMSK)
#define HWIO_RMm_SEQ_UP_BR_ADDRi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_UP_BR_ADDRi_ADDR(m,i), mask)
#define HWIO_RMm_SEQ_UP_BR_ADDRi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SEQ_UP_BR_ADDRi_ADDR(m,i),val)
#define HWIO_RMm_SEQ_UP_BR_ADDRi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_UP_BR_ADDRi_ADDR(m,i),mask,val,HWIO_RMm_SEQ_UP_BR_ADDRi_INI2(m,i))
#define HWIO_RMm_SEQ_UP_BR_ADDRi_HW_UP_BR_ADDR_BMSK                                                   0xfff
#define HWIO_RMm_SEQ_UP_BR_ADDRi_HW_UP_BR_ADDR_SHFT                                                     0x0

#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_ADDR(m,i)                                                     (ARC_RM_REG_BASE      + 0x00001640 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_RMSK                                                               0xfff
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_MAXm                                                                   8
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_MAXi                                                                  15
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_BR_ADDRi_ADDR(m,i), HWIO_RMm_SEQ_DOWN_BR_ADDRi_RMSK)
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_BR_ADDRi_ADDR(m,i), mask)
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SEQ_DOWN_BR_ADDRi_ADDR(m,i),val)
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_DOWN_BR_ADDRi_ADDR(m,i),mask,val,HWIO_RMm_SEQ_DOWN_BR_ADDRi_INI2(m,i))
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_HW_DOWN_BR_ADDR_BMSK                                               0xfff
#define HWIO_RMm_SEQ_DOWN_BR_ADDRi_HW_DOWN_BR_ADDR_SHFT                                                 0x0

#define HWIO_RMm_SEQ_SW_CONF_ADDRi_ADDR(m,i)                                                     (ARC_RM_REG_BASE      + 0x00001a40 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_RMSK                                                               0xfff
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_MAXm                                                                   8
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_MAXi                                                                  15
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SEQ_SW_CONF_ADDRi_ADDR(m,i), HWIO_RMm_SEQ_SW_CONF_ADDRi_RMSK)
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_SW_CONF_ADDRi_ADDR(m,i), mask)
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SEQ_SW_CONF_ADDRi_ADDR(m,i),val)
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_SW_CONF_ADDRi_ADDR(m,i),mask,val,HWIO_RMm_SEQ_SW_CONF_ADDRi_INI2(m,i))
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_SW_CONF_BR_ADDR_BMSK                                               0xfff
#define HWIO_RMm_SEQ_SW_CONF_ADDRi_SW_CONF_BR_ADDR_SHFT                                                 0x0

#define HWIO_RMm_SEQ_UP_BR_EVENT_ADDR(m)                                                         (ARC_RM_REG_BASE      + 0x00002240 + 0x4 * (m))
#define HWIO_RMm_SEQ_UP_BR_EVENT_RMSK                                                                0xffff
#define HWIO_RMm_SEQ_UP_BR_EVENT_MAXm                                                                     8
#define HWIO_RMm_SEQ_UP_BR_EVENT_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQ_UP_BR_EVENT_ADDR(m), HWIO_RMm_SEQ_UP_BR_EVENT_RMSK)
#define HWIO_RMm_SEQ_UP_BR_EVENT_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_UP_BR_EVENT_ADDR(m), mask)
#define HWIO_RMm_SEQ_UP_BR_EVENT_HW_UP_BR_EVENT_BMSK                                                 0xffff
#define HWIO_RMm_SEQ_UP_BR_EVENT_HW_UP_BR_EVENT_SHFT                                                    0x0

#define HWIO_RMm_SEQ_DOWN_BR_EVENT_ADDR(m)                                                       (ARC_RM_REG_BASE      + 0x000022c0 + 0x4 * (m))
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_RMSK                                                              0xffff
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_MAXm                                                                   8
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_BR_EVENT_ADDR(m), HWIO_RMm_SEQ_DOWN_BR_EVENT_RMSK)
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_BR_EVENT_ADDR(m), mask)
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_HW_DOWN_BR_EVENT_BMSK                                             0xffff
#define HWIO_RMm_SEQ_DOWN_BR_EVENT_HW_DOWN_BR_EVENT_SHFT                                                0x0

#define HWIO_RMm_SEQ_SW_CONF_EVENT_ADDR(m)                                                       (ARC_RM_REG_BASE      + 0x00002340 + 0x4 * (m))
#define HWIO_RMm_SEQ_SW_CONF_EVENT_RMSK                                                              0xffff
#define HWIO_RMm_SEQ_SW_CONF_EVENT_MAXm                                                                   8
#define HWIO_RMm_SEQ_SW_CONF_EVENT_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQ_SW_CONF_EVENT_ADDR(m), HWIO_RMm_SEQ_SW_CONF_EVENT_RMSK)
#define HWIO_RMm_SEQ_SW_CONF_EVENT_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_SW_CONF_EVENT_ADDR(m), mask)
#define HWIO_RMm_SEQ_SW_CONF_EVENT_OUTI(m,val)    \
        out_dword(HWIO_RMm_SEQ_SW_CONF_EVENT_ADDR(m),val)
#define HWIO_RMm_SEQ_SW_CONF_EVENT_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_SW_CONF_EVENT_ADDR(m),mask,val,HWIO_RMm_SEQ_SW_CONF_EVENT_INI(m))
#define HWIO_RMm_SEQ_SW_CONF_EVENT_SW_CONF_EVENT_BMSK                                                0xffff
#define HWIO_RMm_SEQ_SW_CONF_EVENT_SW_CONF_EVENT_SHFT                                                   0x0

#define HWIO_RMm_SEQ_UP_START_ADDR_ADDR(m)                                                       (ARC_RM_REG_BASE      + 0x00002440 + 0x40 * (m))
#define HWIO_RMm_SEQ_UP_START_ADDR_RMSK                                                               0xfff
#define HWIO_RMm_SEQ_UP_START_ADDR_MAXm                                                                   8
#define HWIO_RMm_SEQ_UP_START_ADDR_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQ_UP_START_ADDR_ADDR(m), HWIO_RMm_SEQ_UP_START_ADDR_RMSK)
#define HWIO_RMm_SEQ_UP_START_ADDR_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_UP_START_ADDR_ADDR(m), mask)
#define HWIO_RMm_SEQ_UP_START_ADDR_OUTI(m,val)    \
        out_dword(HWIO_RMm_SEQ_UP_START_ADDR_ADDR(m),val)
#define HWIO_RMm_SEQ_UP_START_ADDR_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_UP_START_ADDR_ADDR(m),mask,val,HWIO_RMm_SEQ_UP_START_ADDR_INI(m))
#define HWIO_RMm_SEQ_UP_START_ADDR_UP_START_ADDR_BMSK                                                 0xfff
#define HWIO_RMm_SEQ_UP_START_ADDR_UP_START_ADDR_SHFT                                                   0x0

#define HWIO_RMm_SEQ_DOWN_START_ADDR_ADDR(m)                                                     (ARC_RM_REG_BASE      + 0x00002700 + 0x40 * (m))
#define HWIO_RMm_SEQ_DOWN_START_ADDR_RMSK                                                             0xfff
#define HWIO_RMm_SEQ_DOWN_START_ADDR_MAXm                                                                 8
#define HWIO_RMm_SEQ_DOWN_START_ADDR_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_START_ADDR_ADDR(m), HWIO_RMm_SEQ_DOWN_START_ADDR_RMSK)
#define HWIO_RMm_SEQ_DOWN_START_ADDR_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_DOWN_START_ADDR_ADDR(m), mask)
#define HWIO_RMm_SEQ_DOWN_START_ADDR_OUTI(m,val)    \
        out_dword(HWIO_RMm_SEQ_DOWN_START_ADDR_ADDR(m),val)
#define HWIO_RMm_SEQ_DOWN_START_ADDR_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_DOWN_START_ADDR_ADDR(m),mask,val,HWIO_RMm_SEQ_DOWN_START_ADDR_INI(m))
#define HWIO_RMm_SEQ_DOWN_START_ADDR_DOWN_START_ADDR_BMSK                                             0xfff
#define HWIO_RMm_SEQ_DOWN_START_ADDR_DOWN_START_ADDR_SHFT                                               0x0

#define HWIO_RMm_SEQ_DELAYi_ADDR(m,i)                                                            (ARC_RM_REG_BASE      + 0x00002a00 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SEQ_DELAYi_RMSK                                                                    0xfffff
#define HWIO_RMm_SEQ_DELAYi_MAXm                                                                          8
#define HWIO_RMm_SEQ_DELAYi_MAXi                                                                          3
#define HWIO_RMm_SEQ_DELAYi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SEQ_DELAYi_ADDR(m,i), HWIO_RMm_SEQ_DELAYi_RMSK)
#define HWIO_RMm_SEQ_DELAYi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SEQ_DELAYi_ADDR(m,i), mask)
#define HWIO_RMm_SEQ_DELAYi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SEQ_DELAYi_ADDR(m,i),val)
#define HWIO_RMm_SEQ_DELAYi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SEQ_DELAYi_ADDR(m,i),mask,val,HWIO_RMm_SEQ_DELAYi_INI2(m,i))
#define HWIO_RMm_SEQ_DELAYi_DELAY_CNT_BMSK                                                          0xfffff
#define HWIO_RMm_SEQ_DELAYi_DELAY_CNT_SHFT                                                              0x0

#define HWIO_RMm_SOLVER_TABLE_MODEi_ADDR(m,i)                                                    (ARC_RM_REG_BASE      + 0x00003a00 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SOLVER_TABLE_MODEi_RMSK                                                            0xfffff
#define HWIO_RMm_SOLVER_TABLE_MODEi_MAXm                                                                  8
#define HWIO_RMm_SOLVER_TABLE_MODEi_MAXi                                                                  5
#define HWIO_RMm_SOLVER_TABLE_MODEi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SOLVER_TABLE_MODEi_ADDR(m,i), HWIO_RMm_SOLVER_TABLE_MODEi_RMSK)
#define HWIO_RMm_SOLVER_TABLE_MODEi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SOLVER_TABLE_MODEi_ADDR(m,i), mask)
#define HWIO_RMm_SOLVER_TABLE_MODEi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SOLVER_TABLE_MODEi_ADDR(m,i),val)
#define HWIO_RMm_SOLVER_TABLE_MODEi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SOLVER_TABLE_MODEi_ADDR(m,i),mask,val,HWIO_RMm_SOLVER_TABLE_MODEi_INI2(m,i))
#define HWIO_RMm_SOLVER_TABLE_MODEi_SOLVER_TABLE_SOLVER_MODE_BMSK                                   0xf0000
#define HWIO_RMm_SOLVER_TABLE_MODEi_SOLVER_TABLE_SOLVER_MODE_SHFT                                      0x10
#define HWIO_RMm_SOLVER_TABLE_MODEi_SOLVER_TABLE_BACKOFF_TIME_BMSK                                   0xffff
#define HWIO_RMm_SOLVER_TABLE_MODEi_SOLVER_TABLE_BACKOFF_TIME_SHFT                                      0x0

#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_ADDR(m,i)                                          (ARC_RM_REG_BASE      + 0x00004100 + 0x40 * (m) + 0x4 * (i))
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_RMSK                                               0xffffffff
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_MAXm                                                        8
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_MAXi                                                        5
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_INI2(m,i)        \
        in_dword_masked(HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_ADDR(m,i), HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_RMSK)
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_INMI2(m,i,mask)    \
        in_dword_masked(HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_ADDR(m,i), mask)
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_OUTI2(m,i,val)    \
        out_dword(HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_ADDR(m,i),val)
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_OUTMI2(m,i,mask,val) \
        out_dword_masked_ns(HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_ADDR(m,i),mask,val,HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_INI2(m,i))
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_SOLVER_TABLE_MIN_SLEEP_TIME_BMSK                   0xffffffff
#define HWIO_RMm_SOLVER_TABLE_MIN_SLEEP_TIMEi_SOLVER_TABLE_MIN_SLEEP_TIME_SHFT                          0x0

#define HWIO_RMm_SEQUENCER_STATUS_CSR_ADDR(m)                                                    (ARC_RM_REG_BASE      + 0x00004500 + 0x40 * (m))
#define HWIO_RMm_SEQUENCER_STATUS_CSR_RMSK                                                       0x80000fff
#define HWIO_RMm_SEQUENCER_STATUS_CSR_MAXm                                                                8
#define HWIO_RMm_SEQUENCER_STATUS_CSR_INI(m)        \
        in_dword_masked(HWIO_RMm_SEQUENCER_STATUS_CSR_ADDR(m), HWIO_RMm_SEQUENCER_STATUS_CSR_RMSK)
#define HWIO_RMm_SEQUENCER_STATUS_CSR_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_SEQUENCER_STATUS_CSR_ADDR(m), mask)
#define HWIO_RMm_SEQUENCER_STATUS_CSR_SEQUENCER_BUSY_BMSK                                        0x80000000
#define HWIO_RMm_SEQUENCER_STATUS_CSR_SEQUENCER_BUSY_SHFT                                              0x1f
#define HWIO_RMm_SEQUENCER_STATUS_CSR_SEQUENCER_PC_BMSK                                               0xfff
#define HWIO_RMm_SEQUENCER_STATUS_CSR_SEQUENCER_PC_SHFT                                                 0x0


#endif /* __HWIO_ARC_RM_H__ */
