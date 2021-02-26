#ifndef __ARC_SW_OVERRIDE_HWIO_H__
#define __ARC_SW_OVERRIDE_HWIO_H__
/*
===========================================================================
*/
/**
  @file arc_sw_override_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r48.1.0_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_ARC_ARC_SW_OVERRIDE

  'Include' filters applied: 
  'Exclude' filters applied: 
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/arc/hal/hwio/670/arc_sw_override_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: RPMH_ARC_ARC_SW_OVERRIDE
 *--------------------------------------------------------------------------*/

#define RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE                                                    (AOSS_BASE      + 0x007eea00)
#define RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS                                               0x007eea00

#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_ADDR(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000040 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_OFFS(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000040 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_RMSK                                      0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_MAXm                                               8
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_OVERRIDE_PWR_CTRL_VAL_BMSK                0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_VAL_RMm_OVERRIDE_PWR_CTRL_VAL_SHFT                       0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_ADDR(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000080 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_OFFS(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000080 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_RMSK                                     0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_MAXm                                              8
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_OVERRIDE_PWR_CTRL2_VAL_BMSK              0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_VAL_RMm_OVERRIDE_PWR_CTRL2_VAL_SHFT                     0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_ADDR(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x000000c0 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_OFFS(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x000000c0 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_RMSK                                     0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_MAXm                                              8
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_OVERRIDE_PWR_CTRL_MASK_BMSK              0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL_MASK_RMm_OVERRIDE_PWR_CTRL_MASK_SHFT                     0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_ADDR(m)                                 (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_OFFS(m)                                 (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000100 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_RMSK                                    0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_MAXm                                             8
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_OVERRIDE_PWR_CTRL2_MASK_BMSK            0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_PWR_CTRL2_MASK_RMm_OVERRIDE_PWR_CTRL2_MASK_SHFT                   0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_ADDR(m)                                 (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000140 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_OFFS(m)                                 (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000140 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_RMSK                                    0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_MAXm                                             8
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_OVERRIDE_WAIT_EVENT_VAL_BMSK            0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_VAL_RMm_OVERRIDE_WAIT_EVENT_VAL_SHFT                   0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_ADDR(m)                                (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000180 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_OFFS(m)                                (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000180 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_RMSK                                   0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_MAXm                                            8
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_OVERRIDE_WAIT_EVENT_MASK_BMSK          0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_WAIT_EVENT_MASK_RMm_OVERRIDE_WAIT_EVENT_MASK_SHFT                 0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_ADDR(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x000001c0 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_OFFS(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x000001c0 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_RMSK                                      0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_MAXm                                               8
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_OVERRIDE_BR_EVENT_VAL_BMSK                0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_VAL_RMm_OVERRIDE_BR_EVENT_VAL_SHFT                       0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_ADDR(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000200 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_OFFS(m)                                  (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000200 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_RMSK                                     0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_MAXm                                              8
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_OVERRIDE_BR_EVENT_MASK_BMSK              0xffffffff
#define HWIO_RPMH_ARC_SW_OVERRIDE_BR_EVENT_MASK_RMm_OVERRIDE_BR_EVENT_MASK_SHFT                     0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_ADDR(m)                                    (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000240 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_OFFS(m)                                    (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000240 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_RMSK                                              0x3
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_MAXm                                                8
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_OVERRIDE_MEM_ACC_VAL_BMSK                         0x3
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_VAL_RMm_OVERRIDE_MEM_ACC_VAL_SHFT                         0x0

#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_ADDR(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000280 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_OFFS(m)                                   (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000280 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_RMSK                                             0x3
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_MAXm                                               8
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_ADDR(m), HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_OVERRIDE_MEM_ACC_MASK_BMSK                       0x3
#define HWIO_RPMH_ARC_SW_OVERRIDE_MEM_ACC_MASK_RMm_OVERRIDE_MEM_ACC_MASK_SHFT                       0x0

#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_ADDR(m)                                         (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000300 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_OFFS(m)                                         (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000300 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_RMSK                                                   0x1
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_MAXm                                                     8
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_ADDR(m), HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_UNBLOCK_SEQ_PRE_BMSK                                   0x1
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm_UNBLOCK_SEQ_PRE_SHFT                                   0x0

#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_ADDR(m)                                        (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE      + 0x00000340 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_OFFS(m)                                        (RPMH_ARC_ARC_SW_OVERRIDE_REG_BASE_OFFS + 0x00000340 + 0x4 * (m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_RMSK                                                  0x1
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_MAXm                                                    8
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_INI(m)        \
        in_dword_masked(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_ADDR(m), HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_RMSK)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_INMI(m,mask)    \
        in_dword_masked(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_ADDR(m), mask)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_OUTI(m,val)    \
        out_dword(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_ADDR(m),val)
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_ADDR(m),mask,val,HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_INI(m))
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_UNBLOCK_SEQ_POST_BMSK                                 0x1
#define HWIO_RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm_UNBLOCK_SEQ_POST_SHFT                                 0x0


#endif /* __ARC_SW_OVERRIDE_HWIO_H__ */
