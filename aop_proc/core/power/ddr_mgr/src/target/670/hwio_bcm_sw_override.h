#ifndef __HWIO_BCM_SW_OVERRIDE_H__
#define __HWIO_BCM_SW_OVERRIDE_H__
/*
===========================================================================
*/
/**
  @file hwio_bcm_sw_override.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r44.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_BCM_BCM_SW_OVERRIDE

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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_bcm_sw_override.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: BCM_SW_OVERRIDE
 *--------------------------------------------------------------------------*/

#define BCM_SW_OVERRIDE_REG_BASE                                                            (AOSS_BASE      + 0x00bf2600)
#define BCM_SW_OVERRIDE_REG_BASE_OFFS                                                       0x00bf2600

#define HWIO_SW_CP_CDm_ADDR(m)                                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_SW_CP_CDm_OFFS(m)                                                              (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000000 + 0x4 * (m))
#define HWIO_SW_CP_CDm_RMSK                                                                       0x1f
#define HWIO_SW_CP_CDm_MAXm                                                                         15
#define HWIO_SW_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_CP_CDm_ADDR(m), HWIO_SW_CP_CDm_RMSK)
#define HWIO_SW_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_CP_CDm_ADDR(m), mask)
#define HWIO_SW_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_CP_CDm_ADDR(m),val)
#define HWIO_SW_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_CP_CDm_ADDR(m),mask,val,HWIO_SW_CP_CDm_INI(m))
#define HWIO_SW_CP_CDm_VALID_BMSK                                                                 0x10
#define HWIO_SW_CP_CDm_VALID_SHFT                                                                  0x4
#define HWIO_SW_CP_CDm_SW_CP_BMSK                                                                  0xf
#define HWIO_SW_CP_CDm_SW_CP_SHFT                                                                  0x0

#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m)                                               (BCM_SW_OVERRIDE_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_OFFS(m)                                               (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000100 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_RMSK                                                         0x1
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_MAXm                                                          15
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m), HWIO_SW_OVERRIDE_STOP_AGG_CDm_RMSK)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_STOP_AGG_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_STOP_AGG_CDm_INI(m))
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_SW_STOP_AGG_BMSK                                             0x1
#define HWIO_SW_OVERRIDE_STOP_AGG_CDm_SW_STOP_AGG_SHFT                                             0x0

#define HWIO_SW_PERMIT_ONE_AGG_CDm_ADDR(m)                                                  (BCM_SW_OVERRIDE_REG_BASE      + 0x00000180 + 0x4 * (m))
#define HWIO_SW_PERMIT_ONE_AGG_CDm_OFFS(m)                                                  (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000180 + 0x4 * (m))
#define HWIO_SW_PERMIT_ONE_AGG_CDm_RMSK                                                            0x1
#define HWIO_SW_PERMIT_ONE_AGG_CDm_MAXm                                                             15
#define HWIO_SW_PERMIT_ONE_AGG_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_PERMIT_ONE_AGG_CDm_ADDR(m),val)
#define HWIO_SW_PERMIT_ONE_AGG_CDm_PERMIT_BMSK                                                     0x1
#define HWIO_SW_PERMIT_ONE_AGG_CDm_PERMIT_SHFT                                                     0x0

#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m)                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000200 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_OFFS(m)                                              (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000200 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_RMSK                                                        0xf
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_MAXm                                                         15
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_FINAL_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_FINAL_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_FINAL_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_FINAL_CP_BMSK                                               0xf
#define HWIO_SW_OVERWRITE_FINAL_CP_CDm_FINAL_CP_SHFT                                               0x0

#define HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m)                                                  (BCM_SW_OVERRIDE_REG_BASE      + 0x00000280 + 0x4 * (m))
#define HWIO_SW_BLOCK_FINAL_CP_CDm_OFFS(m)                                                  (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000280 + 0x4 * (m))
#define HWIO_SW_BLOCK_FINAL_CP_CDm_RMSK                                                            0x1
#define HWIO_SW_BLOCK_FINAL_CP_CDm_MAXm                                                             15
#define HWIO_SW_BLOCK_FINAL_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m), HWIO_SW_BLOCK_FINAL_CP_CDm_RMSK)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m), mask)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m),val)
#define HWIO_SW_BLOCK_FINAL_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_BLOCK_FINAL_CP_CDm_ADDR(m),mask,val,HWIO_SW_BLOCK_FINAL_CP_CDm_INI(m))
#define HWIO_SW_BLOCK_FINAL_CP_CDm_BLOCK_BMSK                                                      0x1
#define HWIO_SW_BLOCK_FINAL_CP_CDm_BLOCK_SHFT                                                      0x0

#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m)                                            (BCM_SW_OVERRIDE_REG_BASE      + 0x00000300 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_OFFS(m)                                            (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000300 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_RMSK                                                      0xf
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_MAXm                                                       15
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_WRITTEN_CP_BMSK                                           0xf
#define HWIO_SW_OVERWRITE_WRITTEN_CP_CDm_WRITTEN_CP_SHFT                                           0x0

#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m)                                            (BCM_SW_OVERRIDE_REG_BASE      + 0x00000380 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_OFFS(m)                                            (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000380 + 0x4 * (m))
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_RMSK                                                      0xf
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_MAXm                                                       15
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m), HWIO_SW_OVERWRITE_CURRENT_CP_CDm_RMSK)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m), mask)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m),val)
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERWRITE_CURRENT_CP_CDm_ADDR(m),mask,val,HWIO_SW_OVERWRITE_CURRENT_CP_CDm_INI(m))
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_CURRENT_CP_BMSK                                           0xf
#define HWIO_SW_OVERWRITE_CURRENT_CP_CDm_CURRENT_CP_SHFT                                           0x0

#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m)                                             (BCM_SW_OVERRIDE_REG_BASE      + 0x00000400 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_OFFS(m)                                             (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000400 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_RMSK                                                       0xf
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_MAXm                                                        15
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_CP_SEL_BMSK                                                0xf
#define HWIO_SW_OVERRIDE_TCS_CP_SEL_CDm_CP_SEL_SHFT                                                0x0

#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m)                                             (BCM_SW_OVERRIDE_REG_BASE      + 0x00000480 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_OFFS(m)                                             (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000480 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_RMSK                                                       0x1
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_MAXm                                                        15
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ENABLE_BMSK                                                0x1
#define HWIO_SW_OVERRIDE_TCS_ENABLE_CDm_ENABLE_SHFT                                                0x0

#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m)                                              (BCM_SW_OVERRIDE_REG_BASE      + 0x00000500 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_OFFS(m)                                              (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000500 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_RMSK                                                        0x1
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_MAXm                                                         15
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m), HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_RMSK)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_INI(m))
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_FLUSH_BMSK                                                  0x1
#define HWIO_SW_OVERRIDE_TCS_FLUSH_CDm_FLUSH_SHFT                                                  0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000580 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_OFFS(m)                                     (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000580 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_RMSK                                              0x11
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m), HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_RMSK)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_INI(m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_B_BMSK                         0x10
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_B_SHFT                          0x4
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_A_BMSK                          0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_STICKY_CDm_SEQ_UNBLOCK_STICKY_A_SHFT                          0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000600 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_OFFS(m)                                     (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000600 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_RMSK                                               0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_SEQ_UNBLOCK_ONCE_A_BMSK                            0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_A_CDm_SEQ_UNBLOCK_ONCE_A_SHFT                            0x0

#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_ADDR(m)                                     (BCM_SW_OVERRIDE_REG_BASE      + 0x00000680 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_OFFS(m)                                     (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000680 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_RMSK                                               0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_MAXm                                                15
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_SEQ_UNBLOCK_ONCE_B_BMSK                            0x1
#define HWIO_SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm_SEQ_UNBLOCK_ONCE_B_SHFT                            0x0

#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m)                                               (BCM_SW_OVERRIDE_REG_BASE      + 0x00000700 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_OFFS(m)                                               (BCM_SW_OVERRIDE_REG_BASE_OFFS + 0x00000700 + 0x4 * (m))
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_RMSK                                                        0x11
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_MAXm                                                          15
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_INI(m)        \
        in_dword_masked(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m), HWIO_SW_OVERRIDE_CE_INPUT_CDm_RMSK)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_INMI(m,mask)    \
        in_dword_masked(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m), mask)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_OUTI(m,val)    \
        out_dword(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m),val)
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SW_OVERRIDE_CE_INPUT_CDm_ADDR(m),mask,val,HWIO_SW_OVERRIDE_CE_INPUT_CDm_INI(m))
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_VALID_BMSK                                                  0x10
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_VALID_SHFT                                                   0x4
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_AGG_DONE_BMSK                                                0x1
#define HWIO_SW_OVERRIDE_CE_INPUT_CDm_AGG_DONE_SHFT                                                0x0


#endif /* __HWIO_BCM_SW_OVERRIDE_H__ */
