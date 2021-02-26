#ifndef __HWIO_LPASS_TCSR_H__
#define __HWIO_LPASS_TCSR_H__
/*
===========================================================================
*/
/**
  @file hwio_lpass_tcsr.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.9_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_TCSR

  'Exclude' filters applied: DUMMY RESERVED 

  Generation parameters: 
  { u'exclude-reserved': True,
    u'filename': u'hwio_lpass_tcsr.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'LPASS_TCSR']}
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

  $Header: //components/rel/core.qdsp6/2.1/power/dangergen/src/target/845/hwio_lpass_tcsr.h#1 $
  $DateTime: 2017/08/07 12:56:54 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: LPASS_TCSR
 *--------------------------------------------------------------------------*/

#define LPASS_TCSR_REG_BASE                                                                              (LPASS_BASE      + 0x00080000)

#define HWIO_LPASS_HW_VERSION_ADDR                                                                       (LPASS_TCSR_REG_BASE      + 0x00000000)
#define HWIO_LPASS_HW_VERSION_RMSK                                                                       0xffffffff
#define HWIO_LPASS_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_HW_VERSION_ADDR, HWIO_LPASS_HW_VERSION_RMSK)
#define HWIO_LPASS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_HW_VERSION_ADDR, m)
#define HWIO_LPASS_HW_VERSION_MAJOR_BMSK                                                                 0xf0000000
#define HWIO_LPASS_HW_VERSION_MAJOR_SHFT                                                                       0x1c
#define HWIO_LPASS_HW_VERSION_MINOR_BMSK                                                                  0xfff0000
#define HWIO_LPASS_HW_VERSION_MINOR_SHFT                                                                       0x10
#define HWIO_LPASS_HW_VERSION_STEP_BMSK                                                                      0xffff
#define HWIO_LPASS_HW_VERSION_STEP_SHFT                                                                         0x0

#define HWIO_LPASS_TCSR_DEBUG_CTL_ADDR                                                                   (LPASS_TCSR_REG_BASE      + 0x00000004)
#define HWIO_LPASS_TCSR_DEBUG_CTL_RMSK                                                                      0x11fff
#define HWIO_LPASS_TCSR_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TCSR_DEBUG_CTL_ADDR, HWIO_LPASS_TCSR_DEBUG_CTL_RMSK)
#define HWIO_LPASS_TCSR_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TCSR_DEBUG_CTL_ADDR, m)
#define HWIO_LPASS_TCSR_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TCSR_DEBUG_CTL_ADDR,v)
#define HWIO_LPASS_TCSR_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TCSR_DEBUG_CTL_ADDR,m,v,HWIO_LPASS_TCSR_DEBUG_CTL_IN)
#define HWIO_LPASS_TCSR_DEBUG_CTL_LPASS_DEBUG_EN_BMSK                                                       0x10000
#define HWIO_LPASS_TCSR_DEBUG_CTL_LPASS_DEBUG_EN_SHFT                                                          0x10
#define HWIO_LPASS_TCSR_DEBUG_CTL_SEL_ADDR_BMSK                                                              0x1f00
#define HWIO_LPASS_TCSR_DEBUG_CTL_SEL_ADDR_SHFT                                                                 0x8
#define HWIO_LPASS_TCSR_DEBUG_CTL_SEL_BMSK                                                                     0xff
#define HWIO_LPASS_TCSR_DEBUG_CTL_SEL_SHFT                                                                      0x0

#define HWIO_LPASS_TCSR_GP_CTL_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x00000008)
#define HWIO_LPASS_TCSR_GP_CTL_RMSK                                                                             0x1
#define HWIO_LPASS_TCSR_GP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TCSR_GP_CTL_ADDR, HWIO_LPASS_TCSR_GP_CTL_RMSK)
#define HWIO_LPASS_TCSR_GP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TCSR_GP_CTL_ADDR, m)
#define HWIO_LPASS_TCSR_GP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TCSR_GP_CTL_ADDR,v)
#define HWIO_LPASS_TCSR_GP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TCSR_GP_CTL_ADDR,m,v,HWIO_LPASS_TCSR_GP_CTL_IN)
#define HWIO_LPASS_TCSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_BMSK                                                    0x1
#define HWIO_LPASS_TCSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_SHFT                                                    0x0

#define HWIO_LPASS_AHBE_CFG_ADDR                                                                         (LPASS_TCSR_REG_BASE      + 0x0000002c)
#define HWIO_LPASS_AHBE_CFG_RMSK                                                                              0x111
#define HWIO_LPASS_AHBE_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBE_CFG_ADDR, HWIO_LPASS_AHBE_CFG_RMSK)
#define HWIO_LPASS_AHBE_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBE_CFG_ADDR, m)
#define HWIO_LPASS_AHBE_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBE_CFG_ADDR,v)
#define HWIO_LPASS_AHBE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBE_CFG_ADDR,m,v,HWIO_LPASS_AHBE_CFG_IN)
#define HWIO_LPASS_AHBE_CFG_Q6SS_PRIORITY_BMSK                                                                0x100
#define HWIO_LPASS_AHBE_CFG_Q6SS_PRIORITY_SHFT                                                                  0x8
#define HWIO_LPASS_AHBE_CFG_FABRIC_PRIORITY_BMSK                                                               0x10
#define HWIO_LPASS_AHBE_CFG_FABRIC_PRIORITY_SHFT                                                                0x4
#define HWIO_LPASS_AHBE_CFG_ROUND_ROBIN_EN_BMSK                                                                 0x1
#define HWIO_LPASS_AHBE_CFG_ROUND_ROBIN_EN_SHFT                                                                 0x0

#define HWIO_LPASS_AHBE_LOCK_CTL_ADDR                                                                    (LPASS_TCSR_REG_BASE      + 0x00000030)
#define HWIO_LPASS_AHBE_LOCK_CTL_RMSK                                                                           0x3
#define HWIO_LPASS_AHBE_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBE_LOCK_CTL_ADDR, HWIO_LPASS_AHBE_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBE_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBE_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBE_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBE_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBE_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBE_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBE_LOCK_CTL_IN)
#define HWIO_LPASS_AHBE_LOCK_CTL_Q6SS_LOCK_EN_BMSK                                                              0x2
#define HWIO_LPASS_AHBE_LOCK_CTL_Q6SS_LOCK_EN_SHFT                                                              0x1
#define HWIO_LPASS_AHBE_LOCK_CTL_FABRIC_LOCK_EN_BMSK                                                            0x1
#define HWIO_LPASS_AHBE_LOCK_CTL_FABRIC_LOCK_EN_SHFT                                                            0x0

#define HWIO_LPASS_AHBE_STATUS_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x00000034)
#define HWIO_LPASS_AHBE_STATUS_RMSK                                                                             0x7
#define HWIO_LPASS_AHBE_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_AHBE_STATUS_ADDR, HWIO_LPASS_AHBE_STATUS_RMSK)
#define HWIO_LPASS_AHBE_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBE_STATUS_ADDR, m)
#define HWIO_LPASS_AHBE_STATUS_AHBE_DECODE_BOUNDARY_ERROR_BMSK                                                  0x4
#define HWIO_LPASS_AHBE_STATUS_AHBE_DECODE_BOUNDARY_ERROR_SHFT                                                  0x2
#define HWIO_LPASS_AHBE_STATUS_AHBE_DECODE_ERROR_BMSK                                                           0x2
#define HWIO_LPASS_AHBE_STATUS_AHBE_DECODE_ERROR_SHFT                                                           0x1
#define HWIO_LPASS_AHBE_STATUS_AHBE_MISALIGNED_BMSK                                                             0x1
#define HWIO_LPASS_AHBE_STATUS_AHBE_MISALIGNED_SHFT                                                             0x0

#define HWIO_LPASS_AHBE_ACK_ADDR                                                                         (LPASS_TCSR_REG_BASE      + 0x00000038)
#define HWIO_LPASS_AHBE_ACK_RMSK                                                                                0x7
#define HWIO_LPASS_AHBE_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_AHBE_ACK_ADDR,v)
#define HWIO_LPASS_AHBE_ACK_AHBE_DECODE_BOUNDARY_ERROR_BMSK                                                     0x4
#define HWIO_LPASS_AHBE_ACK_AHBE_DECODE_BOUNDARY_ERROR_SHFT                                                     0x2
#define HWIO_LPASS_AHBE_ACK_AHBE_DECODE_ERROR_BMSK                                                              0x2
#define HWIO_LPASS_AHBE_ACK_AHBE_DECODE_ERROR_SHFT                                                              0x1
#define HWIO_LPASS_AHBE_ACK_AHBE_MISALIGNED_BMSK                                                                0x1
#define HWIO_LPASS_AHBE_ACK_AHBE_MISALIGNED_SHFT                                                                0x0

#define HWIO_LPASS_AHBE_INT_EN_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x0000003c)
#define HWIO_LPASS_AHBE_INT_EN_RMSK                                                                             0x7
#define HWIO_LPASS_AHBE_INT_EN_IN          \
        in_dword_masked(HWIO_LPASS_AHBE_INT_EN_ADDR, HWIO_LPASS_AHBE_INT_EN_RMSK)
#define HWIO_LPASS_AHBE_INT_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBE_INT_EN_ADDR, m)
#define HWIO_LPASS_AHBE_INT_EN_OUT(v)      \
        out_dword(HWIO_LPASS_AHBE_INT_EN_ADDR,v)
#define HWIO_LPASS_AHBE_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBE_INT_EN_ADDR,m,v,HWIO_LPASS_AHBE_INT_EN_IN)
#define HWIO_LPASS_AHBE_INT_EN_AHBE_DECODE_BOUNDARY_ERROR_BMSK                                                  0x4
#define HWIO_LPASS_AHBE_INT_EN_AHBE_DECODE_BOUNDARY_ERROR_SHFT                                                  0x2
#define HWIO_LPASS_AHBE_INT_EN_AHBE_DECODE_ERROR_BMSK                                                           0x2
#define HWIO_LPASS_AHBE_INT_EN_AHBE_DECODE_ERROR_SHFT                                                           0x1
#define HWIO_LPASS_AHBE_INT_EN_AHBE_MISALIGNED_BMSK                                                             0x1
#define HWIO_LPASS_AHBE_INT_EN_AHBE_MISALIGNED_SHFT                                                             0x0

#define HWIO_LPASS_AHBE_SPARE_ADDR                                                                       (LPASS_TCSR_REG_BASE      + 0x00000040)
#define HWIO_LPASS_AHBE_SPARE_RMSK                                                                       0xffffffff
#define HWIO_LPASS_AHBE_SPARE_IN          \
        in_dword_masked(HWIO_LPASS_AHBE_SPARE_ADDR, HWIO_LPASS_AHBE_SPARE_RMSK)
#define HWIO_LPASS_AHBE_SPARE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBE_SPARE_ADDR, m)
#define HWIO_LPASS_AHBE_SPARE_OUT(v)      \
        out_dword(HWIO_LPASS_AHBE_SPARE_ADDR,v)
#define HWIO_LPASS_AHBE_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBE_SPARE_ADDR,m,v,HWIO_LPASS_AHBE_SPARE_IN)
#define HWIO_LPASS_AHBE_SPARE_AHBE_SPARE_BMSK                                                            0xffffffff
#define HWIO_LPASS_AHBE_SPARE_AHBE_SPARE_SHFT                                                                   0x0

#define HWIO_LPASS_FP_BASE_ADDR                                                                          (LPASS_TCSR_REG_BASE      + 0x00001000)
#define HWIO_LPASS_FP_BASE_RMSK                                                                            0xffffff
#define HWIO_LPASS_FP_BASE_IN          \
        in_dword_masked(HWIO_LPASS_FP_BASE_ADDR, HWIO_LPASS_FP_BASE_RMSK)
#define HWIO_LPASS_FP_BASE_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_BASE_ADDR, m)
#define HWIO_LPASS_FP_BASE_FP_BASE_ADDR_35_12_BMSK                                                         0xffffff
#define HWIO_LPASS_FP_BASE_FP_BASE_ADDR_35_12_SHFT                                                              0x0

#define HWIO_LPASS_FP_OFFSET_A_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x00001004)
#define HWIO_LPASS_FP_OFFSET_A_RMSK                                                                          0x3fff
#define HWIO_LPASS_FP_OFFSET_A_IN          \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_A_ADDR, HWIO_LPASS_FP_OFFSET_A_RMSK)
#define HWIO_LPASS_FP_OFFSET_A_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_A_ADDR, m)
#define HWIO_LPASS_FP_OFFSET_A_OUT(v)      \
        out_dword(HWIO_LPASS_FP_OFFSET_A_ADDR,v)
#define HWIO_LPASS_FP_OFFSET_A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_OFFSET_A_ADDR,m,v,HWIO_LPASS_FP_OFFSET_A_IN)
#define HWIO_LPASS_FP_OFFSET_A_FP_OFFSET_A_MAX_BMSK                                                          0x3f80
#define HWIO_LPASS_FP_OFFSET_A_FP_OFFSET_A_MAX_SHFT                                                             0x7
#define HWIO_LPASS_FP_OFFSET_A_FP_OFFSET_A_MIN_BMSK                                                            0x7f
#define HWIO_LPASS_FP_OFFSET_A_FP_OFFSET_A_MIN_SHFT                                                             0x0

#define HWIO_LPASS_FP_OFFSET_B_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x00001008)
#define HWIO_LPASS_FP_OFFSET_B_RMSK                                                                          0x3fff
#define HWIO_LPASS_FP_OFFSET_B_IN          \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_B_ADDR, HWIO_LPASS_FP_OFFSET_B_RMSK)
#define HWIO_LPASS_FP_OFFSET_B_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_B_ADDR, m)
#define HWIO_LPASS_FP_OFFSET_B_OUT(v)      \
        out_dword(HWIO_LPASS_FP_OFFSET_B_ADDR,v)
#define HWIO_LPASS_FP_OFFSET_B_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_OFFSET_B_ADDR,m,v,HWIO_LPASS_FP_OFFSET_B_IN)
#define HWIO_LPASS_FP_OFFSET_B_FP_OFFSET_B_MAX_BMSK                                                          0x3f80
#define HWIO_LPASS_FP_OFFSET_B_FP_OFFSET_B_MAX_SHFT                                                             0x7
#define HWIO_LPASS_FP_OFFSET_B_FP_OFFSET_B_MIN_BMSK                                                            0x7f
#define HWIO_LPASS_FP_OFFSET_B_FP_OFFSET_B_MIN_SHFT                                                             0x0

#define HWIO_LPASS_FP_OFFSET_C_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x0000100c)
#define HWIO_LPASS_FP_OFFSET_C_RMSK                                                                          0x3fff
#define HWIO_LPASS_FP_OFFSET_C_IN          \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_C_ADDR, HWIO_LPASS_FP_OFFSET_C_RMSK)
#define HWIO_LPASS_FP_OFFSET_C_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_OFFSET_C_ADDR, m)
#define HWIO_LPASS_FP_OFFSET_C_OUT(v)      \
        out_dword(HWIO_LPASS_FP_OFFSET_C_ADDR,v)
#define HWIO_LPASS_FP_OFFSET_C_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_OFFSET_C_ADDR,m,v,HWIO_LPASS_FP_OFFSET_C_IN)
#define HWIO_LPASS_FP_OFFSET_C_FP_OFFSET_C_MAX_BMSK                                                          0x3f80
#define HWIO_LPASS_FP_OFFSET_C_FP_OFFSET_C_MAX_SHFT                                                             0x7
#define HWIO_LPASS_FP_OFFSET_C_FP_OFFSET_C_MIN_BMSK                                                            0x7f
#define HWIO_LPASS_FP_OFFSET_C_FP_OFFSET_C_MIN_SHFT                                                             0x0

#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_ADDR                                                             (LPASS_TCSR_REG_BASE      + 0x00002000)
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_RMSK                                                                0x3ffff
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_AIF_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_AIF_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_AIF_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_AIF_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_AIF_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_AIF_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_8_BMSK                                         0x30000
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_8_SHFT                                            0x10
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_7_BMSK                                          0xc000
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_7_SHFT                                             0xe
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_6_BMSK                                          0x3000
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_6_SHFT                                             0xc
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_5_BMSK                                           0xc00
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_5_SHFT                                             0xa
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_4_BMSK                                           0x300
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_4_SHFT                                             0x8
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_3_BMSK                                            0xc0
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_3_SHFT                                             0x6
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_2_BMSK                                            0x30
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_2_SHFT                                             0x4
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_1_BMSK                                             0xc
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_1_SHFT                                             0x2
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_0_BMSK                                             0x3
#define HWIO_LPASS_FP_EE_OWNER_AIF_LMID_FP_EE_OWNER_AIF_LMID_0_SHFT                                             0x0

#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_ADDR                                                          (LPASS_TCSR_REG_BASE      + 0x00003000)
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_RMSK                                                          0x3fffffff
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_14_BMSK                               0x30000000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_14_SHFT                                     0x1c
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_13_BMSK                                0xc000000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_13_SHFT                                     0x1a
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_12_BMSK                                0x3000000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_12_SHFT                                     0x18
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_11_BMSK                                 0xc00000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_11_SHFT                                     0x16
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_10_BMSK                                 0x300000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_10_SHFT                                     0x14
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_9_BMSK                                   0xc0000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_9_SHFT                                      0x12
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_8_BMSK                                   0x30000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_8_SHFT                                      0x10
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_7_BMSK                                    0xc000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_7_SHFT                                       0xe
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_6_BMSK                                    0x3000
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_6_SHFT                                       0xc
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_5_BMSK                                     0xc00
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_5_SHFT                                       0xa
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_4_BMSK                                     0x300
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_4_SHFT                                       0x8
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_3_BMSK                                      0xc0
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_3_SHFT                                       0x6
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_2_BMSK                                      0x30
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_2_SHFT                                       0x4
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_1_BMSK                                       0xc
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_1_SHFT                                       0x2
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_0_BMSK                                       0x3
#define HWIO_LPASS_FP_EE_OWNER_AUD_SB_LMID_FP_EE_OWNER_AUD_SB_LMID_0_SHFT                                       0x0

#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_ADDR                                                          (LPASS_TCSR_REG_BASE      + 0x00004000)
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_RMSK                                                             0x3ffff
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_8_BMSK                                   0x30000
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_8_SHFT                                      0x10
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_7_BMSK                                    0xc000
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_7_SHFT                                       0xe
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_6_BMSK                                    0x3000
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_6_SHFT                                       0xc
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_5_BMSK                                     0xc00
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_5_SHFT                                       0xa
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_4_BMSK                                     0x300
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_4_SHFT                                       0x8
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_3_BMSK                                      0xc0
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_3_SHFT                                       0x6
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_2_BMSK                                      0x30
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_2_SHFT                                       0x4
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_1_BMSK                                       0xc
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_1_SHFT                                       0x2
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_0_BMSK                                       0x3
#define HWIO_LPASS_FP_EE_OWNER_QCA_SB_LMID_FP_EE_OWNER_QCA_SB_LMID_0_SHFT                                       0x0

#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_ADDR                                                             (LPASS_TCSR_REG_BASE      + 0x00005000)
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_RMSK                                                                    0x3
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_DML_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_DML_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_DML_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_DML_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_DML_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_DML_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_FP_EE_OWNER_DML_LMID_0_BMSK                                             0x3
#define HWIO_LPASS_FP_EE_OWNER_DML_LMID_FP_EE_OWNER_DML_LMID_0_SHFT                                             0x0

#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_ADDR                                                            (LPASS_TCSR_REG_BASE      + 0x00006000)
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_RMSK                                                                   0x3
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_FP_EE_OWNER_RSMP_LMID_0_BMSK                                           0x3
#define HWIO_LPASS_FP_EE_OWNER_RSMP_LMID_FP_EE_OWNER_RSMP_LMID_0_SHFT                                           0x0

#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_ADDR                                                            (LPASS_TCSR_REG_BASE      + 0x00007000)
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_RMSK                                                                  0xff
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_3_BMSK                                          0xc0
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_3_SHFT                                           0x6
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_2_BMSK                                          0x30
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_2_SHFT                                           0x4
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_1_BMSK                                           0xc
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_1_SHFT                                           0x2
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_0_BMSK                                           0x3
#define HWIO_LPASS_FP_EE_OWNER_HDMI_LMID_FP_EE_OWNER_HDMI_LMID_0_SHFT                                           0x0

#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_ADDR                                                           (LPASS_TCSR_REG_BASE      + 0x00008000)
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_RMSK                                                                  0x3
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_IN          \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_ADDR, HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_RMSK)
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_INM(m)      \
        in_dword_masked(HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_ADDR, m)
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_OUT(v)      \
        out_dword(HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_ADDR,v)
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_ADDR,m,v,HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_IN)
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_FP_EE_OWNER_SPDIF_LMID_0_BMSK                                         0x3
#define HWIO_LPASS_FP_EE_OWNER_SPDIF_LMID_FP_EE_OWNER_SPDIF_LMID_0_SHFT                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR                                          (LPASS_TCSR_REG_BASE      + 0x00009000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_RMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_SEL_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_SEL_SHFT                                             0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR                                                 (LPASS_TCSR_REG_BASE      + 0x0000a000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_RMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_SEL_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_SEL_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR                                                 (LPASS_TCSR_REG_BASE      + 0x0000b000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_RMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_SEL_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_SEL_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR                                                 (LPASS_TCSR_REG_BASE      + 0x0000c000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_RMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_SEL_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_SEL_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR                                                (LPASS_TCSR_REG_BASE      + 0x0000d000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_RMSK                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_SEL_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_SEL_SHFT                                                   0x0

#define HWIO_LPASS_Q6SS_RST_EVB_SEL_ADDR                                                                 (LPASS_TCSR_REG_BASE      + 0x0000e000)
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_RMSK                                                                        0x1
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_IN          \
        in_dword_masked(HWIO_LPASS_Q6SS_RST_EVB_SEL_ADDR, HWIO_LPASS_Q6SS_RST_EVB_SEL_RMSK)
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_Q6SS_RST_EVB_SEL_ADDR, m)
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_Q6SS_RST_EVB_SEL_ADDR,v)
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_Q6SS_RST_EVB_SEL_ADDR,m,v,HWIO_LPASS_Q6SS_RST_EVB_SEL_IN)
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_SEL_BMSK                                                                    0x1
#define HWIO_LPASS_Q6SS_RST_EVB_SEL_SEL_SHFT                                                                    0x0

#define HWIO_LPASS_Q6SS_RST_EVB_ADDR                                                                     (LPASS_TCSR_REG_BASE      + 0x0000e004)
#define HWIO_LPASS_Q6SS_RST_EVB_RMSK                                                                      0xffffff0
#define HWIO_LPASS_Q6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_LPASS_Q6SS_RST_EVB_ADDR, HWIO_LPASS_Q6SS_RST_EVB_RMSK)
#define HWIO_LPASS_Q6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_LPASS_Q6SS_RST_EVB_ADDR, m)
#define HWIO_LPASS_Q6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_LPASS_Q6SS_RST_EVB_ADDR,v)
#define HWIO_LPASS_Q6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_Q6SS_RST_EVB_ADDR,m,v,HWIO_LPASS_Q6SS_RST_EVB_IN)
#define HWIO_LPASS_Q6SS_RST_EVB_EVB_BMSK                                                                  0xffffff0
#define HWIO_LPASS_Q6SS_RST_EVB_EVB_SHFT                                                                        0x4

#define HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR                                                                  (LPASS_TCSR_REG_BASE      + 0x0000f000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_RMSK                                                                       0x11f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR, HWIO_LPASS_VFR_IRQ_MUX_CTL_RMSK)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR, m)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR,v)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR,m,v,HWIO_LPASS_VFR_IRQ_MUX_CTL_IN)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_VFR_BMSK                                                                   0x100
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_VFR_SHFT                                                                     0x8
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_SEL_BMSK                                                                    0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_SEL_SHFT                                                                     0x0

#define HWIO_LPASS_VFR_STC_MSB_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x0000f004)
#define HWIO_LPASS_VFR_STC_MSB_RMSK                                                                        0xffffff
#define HWIO_LPASS_VFR_STC_MSB_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_ADDR, HWIO_LPASS_VFR_STC_MSB_RMSK)
#define HWIO_LPASS_VFR_STC_MSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_ADDR, m)
#define HWIO_LPASS_VFR_STC_MSB_MSB_BMSK                                                                    0xffffff
#define HWIO_LPASS_VFR_STC_MSB_MSB_SHFT                                                                         0x0

#define HWIO_LPASS_VFR_STC_LSB_ADDR                                                                      (LPASS_TCSR_REG_BASE      + 0x0000f008)
#define HWIO_LPASS_VFR_STC_LSB_RMSK                                                                      0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_ADDR, HWIO_LPASS_VFR_STC_LSB_RMSK)
#define HWIO_LPASS_VFR_STC_LSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_ADDR, m)
#define HWIO_LPASS_VFR_STC_LSB_LSB_BMSK                                                                  0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_LSB_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR                                                           (LPASS_TCSR_REG_BASE      + 0x00010100)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_RMSK                                                                 0xff
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR, HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_IN)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_LPASS_CORE_CLAMP_ZERO_DELAY_BMSK                                     0xff
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_LPASS_CORE_CLAMP_ZERO_DELAY_SHFT                                      0x0

#define HWIO_LPASS_STM_MUXSEL_n_ADDR(n)                                                                  (LPASS_TCSR_REG_BASE      + 0x00011000 + 0x4 * (n))
#define HWIO_LPASS_STM_MUXSEL_n_RMSK                                                                     0x8000000f
#define HWIO_LPASS_STM_MUXSEL_n_MAXn                                                                             31
#define HWIO_LPASS_STM_MUXSEL_n_INI(n)        \
        in_dword_masked(HWIO_LPASS_STM_MUXSEL_n_ADDR(n), HWIO_LPASS_STM_MUXSEL_n_RMSK)
#define HWIO_LPASS_STM_MUXSEL_n_INMI(n,mask)    \
        in_dword_masked(HWIO_LPASS_STM_MUXSEL_n_ADDR(n), mask)
#define HWIO_LPASS_STM_MUXSEL_n_OUTI(n,val)    \
        out_dword(HWIO_LPASS_STM_MUXSEL_n_ADDR(n),val)
#define HWIO_LPASS_STM_MUXSEL_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_LPASS_STM_MUXSEL_n_ADDR(n),mask,val,HWIO_LPASS_STM_MUXSEL_n_INI(n))
#define HWIO_LPASS_STM_MUXSEL_n_ENABLE_BMSK                                                              0x80000000
#define HWIO_LPASS_STM_MUXSEL_n_ENABLE_SHFT                                                                    0x1f
#define HWIO_LPASS_STM_MUXSEL_n_SEL_BMSK                                                                        0xf
#define HWIO_LPASS_STM_MUXSEL_n_SEL_SHFT                                                                        0x0

#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR                                                                (LPASS_TCSR_REG_BASE      + 0x00012000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_RMSK                                                                     0x11f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR, HWIO_LPASS_VFR_IRQ_MUX_CTL_1_RMSK)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR, m)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_OUT(v)      \
        out_dword(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR,v)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR,m,v,HWIO_LPASS_VFR_IRQ_MUX_CTL_1_IN)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_VFR_BMSK                                                                 0x100
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_VFR_SHFT                                                                   0x8
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_SEL_BMSK                                                                  0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_SEL_SHFT                                                                   0x0

#define HWIO_LPASS_VFR_STC_MSB_1_ADDR                                                                    (LPASS_TCSR_REG_BASE      + 0x00012004)
#define HWIO_LPASS_VFR_STC_MSB_1_RMSK                                                                      0xffffff
#define HWIO_LPASS_VFR_STC_MSB_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_1_ADDR, HWIO_LPASS_VFR_STC_MSB_1_RMSK)
#define HWIO_LPASS_VFR_STC_MSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_1_ADDR, m)
#define HWIO_LPASS_VFR_STC_MSB_1_MSB_BMSK                                                                  0xffffff
#define HWIO_LPASS_VFR_STC_MSB_1_MSB_SHFT                                                                       0x0

#define HWIO_LPASS_VFR_STC_LSB_1_ADDR                                                                    (LPASS_TCSR_REG_BASE      + 0x00012008)
#define HWIO_LPASS_VFR_STC_LSB_1_RMSK                                                                    0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_1_ADDR, HWIO_LPASS_VFR_STC_LSB_1_RMSK)
#define HWIO_LPASS_VFR_STC_LSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_1_ADDR, m)
#define HWIO_LPASS_VFR_STC_LSB_1_LSB_BMSK                                                                0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_1_LSB_SHFT                                                                       0x0

#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR                                                            (LPASS_TCSR_REG_BASE      + 0x00013000)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_RMSK                                                                 0x1ff
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR, HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR,v)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR,m,v,HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_IN)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_VFR_BMSK                                                             0x100
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_VFR_SHFT                                                               0x8
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_SEL_BMSK                                                              0xff
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_SEL_SHFT                                                               0x0

#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR                                                               (LPASS_TCSR_REG_BASE      + 0x00013004)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_RMSK                                                                 0xffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR, HWIO_LPASS_SENSOR_IRQ_STC_MSB_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_MSB_BMSK                                                             0xffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_MSB_SHFT                                                                  0x0

#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR                                                               (LPASS_TCSR_REG_BASE      + 0x00013008)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_RMSK                                                               0xffffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR, HWIO_LPASS_SENSOR_IRQ_STC_LSB_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_LSB_BMSK                                                           0xffffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_LSB_SHFT                                                                  0x0

#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR                                                              (LPASS_TCSR_REG_BASE      + 0x00014000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_RMSK                                                                   0x1ff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR, HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_RMSK)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_OUT(v)      \
        out_dword(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR,v)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR,m,v,HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_IN)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_VFR_BMSK                                                               0x100
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_VFR_SHFT                                                                 0x8
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_SEL_BMSK                                                                0xff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_SEL_SHFT                                                                 0x0

#define HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR                                                                 (LPASS_TCSR_REG_BASE      + 0x00014004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_RMSK                                                                   0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR, HWIO_LPASS_GP_IRQ_STC_MSB_0_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_MSB_BMSK                                                               0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_MSB_SHFT                                                                    0x0

#define HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR                                                                 (LPASS_TCSR_REG_BASE      + 0x00014008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_RMSK                                                                 0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR, HWIO_LPASS_GP_IRQ_STC_LSB_0_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_LSB_BMSK                                                             0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_LSB_SHFT                                                                    0x0

#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR                                                              (LPASS_TCSR_REG_BASE      + 0x00015000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_RMSK                                                                   0x1ff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR, HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_RMSK)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_OUT(v)      \
        out_dword(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR,v)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR,m,v,HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_IN)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_VFR_BMSK                                                               0x100
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_VFR_SHFT                                                                 0x8
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_SEL_BMSK                                                                0xff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_SEL_SHFT                                                                 0x0

#define HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR                                                                 (LPASS_TCSR_REG_BASE      + 0x00015004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_RMSK                                                                   0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR, HWIO_LPASS_GP_IRQ_STC_MSB_1_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_MSB_BMSK                                                               0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_MSB_SHFT                                                                    0x0

#define HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR                                                                 (LPASS_TCSR_REG_BASE      + 0x00015008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_RMSK                                                                 0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR, HWIO_LPASS_GP_IRQ_STC_LSB_1_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_LSB_BMSK                                                             0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_LSB_SHFT                                                                    0x0

#define HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR                                                                   (LPASS_TCSR_REG_BASE      + 0x00016000)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_RMSK                                                                     0xffffff
#define HWIO_LPASS_SB_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR, HWIO_LPASS_SB_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_SB_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_23_BMSK                                               0x800000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_23_SHFT                                                   0x17
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_22_BMSK                                               0x400000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_22_SHFT                                                   0x16
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_21_BMSK                                               0x200000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_21_SHFT                                                   0x15
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_20_BMSK                                               0x100000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_20_SHFT                                                   0x14
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_19_BMSK                                                0x80000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_19_SHFT                                                   0x13
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_18_BMSK                                                0x40000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_18_SHFT                                                   0x12
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_17_BMSK                                                0x20000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_17_SHFT                                                   0x11
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_16_BMSK                                                0x10000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_16_SHFT                                                   0x10
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_15_BMSK                                                 0x8000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_15_SHFT                                                    0xf
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_14_BMSK                                                 0x4000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_14_SHFT                                                    0xe
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_13_BMSK                                                 0x2000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_13_SHFT                                                    0xd
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_12_BMSK                                                 0x1000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_12_SHFT                                                    0xc
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_11_BMSK                                                  0x800
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_11_SHFT                                                    0xb
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_10_BMSK                                                  0x400
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_10_SHFT                                                    0xa
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_9_BMSK                                                   0x200
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_9_SHFT                                                     0x9
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_8_BMSK                                                   0x100
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_8_SHFT                                                     0x8
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_7_BMSK                                                    0x80
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_7_SHFT                                                     0x7
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_6_BMSK                                                    0x40
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_6_SHFT                                                     0x6
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_5_BMSK                                                    0x20
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_5_SHFT                                                     0x5
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_4_BMSK                                                    0x10
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_4_SHFT                                                     0x4
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_3_BMSK                                                     0x8
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_3_SHFT                                                     0x3
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_2_BMSK                                                     0x4
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_2_SHFT                                                     0x2
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_1_BMSK                                                     0x2
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_1_SHFT                                                     0x1
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_0_BMSK                                                     0x1
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_0_SHFT                                                     0x0

#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR                                                                (LPASS_TCSR_REG_BASE      + 0x00016004)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_RMSK                                                                     0x1ff
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR, HWIO_LPASS_LPAIF_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_LPAIF_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_3_BMSK                                       0x100
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_3_SHFT                                         0x8
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_2_BMSK                                        0x80
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_2_SHFT                                         0x7
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_1_BMSK                                        0x40
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_1_SHFT                                         0x6
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_0_BMSK                                        0x20
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_0_SHFT                                         0x5
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_4_BMSK                                         0x10
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_4_SHFT                                          0x4
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_3_BMSK                                          0x8
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_3_SHFT                                          0x3
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_2_BMSK                                          0x4
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_2_SHFT                                          0x2
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_1_BMSK                                          0x2
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_1_SHFT                                          0x1
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_0_BMSK                                          0x1
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_0_SHFT                                          0x0

#define HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR                                                                  (LPASS_TCSR_REG_BASE      + 0x00016008)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_RMSK                                                                         0x1
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR, HWIO_LPASS_WCN_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_WCN_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_SEL_BMSK                                                                     0x1
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_SEL_SHFT                                                                     0x0

#define HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR                                                                  (LPASS_TCSR_REG_BASE      + 0x0001600c)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_RMSK                                                                         0x1
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR, HWIO_LPASS_AVT_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_AVT_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_SEL_BMSK                                                                     0x1
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_SEL_SHFT                                                                     0x0

#define HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR                                                                  (LPASS_TCSR_REG_BASE      + 0x00016010)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_RMSK                                                                    0xffffff
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR, HWIO_LPASS_SB1_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_SB1_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_23_BMSK                                             0x800000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_23_SHFT                                                 0x17
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_22_BMSK                                             0x400000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_22_SHFT                                                 0x16
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_21_BMSK                                             0x200000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_21_SHFT                                                 0x15
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_20_BMSK                                             0x100000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_20_SHFT                                                 0x14
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_19_BMSK                                              0x80000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_19_SHFT                                                 0x13
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_18_BMSK                                              0x40000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_18_SHFT                                                 0x12
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_17_BMSK                                              0x20000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_17_SHFT                                                 0x11
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_16_BMSK                                              0x10000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_16_SHFT                                                 0x10
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_15_BMSK                                               0x8000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_15_SHFT                                                  0xf
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_14_BMSK                                               0x4000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_14_SHFT                                                  0xe
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_13_BMSK                                               0x2000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_13_SHFT                                                  0xd
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_12_BMSK                                               0x1000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_12_SHFT                                                  0xc
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_11_BMSK                                                0x800
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_11_SHFT                                                  0xb
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_10_BMSK                                                0x400
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_10_SHFT                                                  0xa
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_9_BMSK                                                 0x200
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_9_SHFT                                                   0x9
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_8_BMSK                                                 0x100
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_8_SHFT                                                   0x8
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_7_BMSK                                                  0x80
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_7_SHFT                                                   0x7
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_6_BMSK                                                  0x40
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_6_SHFT                                                   0x6
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_5_BMSK                                                  0x20
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_5_SHFT                                                   0x5
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_4_BMSK                                                  0x10
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_4_SHFT                                                   0x4
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_3_BMSK                                                   0x8
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_3_SHFT                                                   0x3
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_2_BMSK                                                   0x4
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_2_SHFT                                                   0x2
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_1_BMSK                                                   0x2
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_1_SHFT                                                   0x1
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_0_BMSK                                                   0x1
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_0_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_ADDR                                                   (LPASS_TCSR_REG_BASE      + 0x00017000)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_RMSK                                                          0x3
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_ADDR, HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_HW_CTL_BMSK                                                   0x2
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_HW_CTL_SHFT                                                   0x1
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_CGCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ADDR                                             (LPASS_TCSR_REG_BASE      + 0x00017004)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_RMSK                                                    0x7
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                     0x7
#define HWIO_LPASS_AUDIO_WRAPPER_LCC_CSR_AON_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                     0x0

#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_ADDR                                                      (LPASS_TCSR_REG_BASE      + 0x00018000)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_RMSK                                                             0x3
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_ADDR, HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_CGCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ADDR                                                (LPASS_TCSR_REG_BASE      + 0x00018004)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_RMSK                                                       0x7
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                        0x7
#define HWIO_LPASS_AUDIO_WRAPPER_TCSR_AON_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                        0x0

#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_ADDR                                               (LPASS_TCSR_REG_BASE      + 0x00019000)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_RMSK                                                      0x3
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_ADDR, HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_HW_CTL_BMSK                                               0x2
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_HW_CTL_SHFT                                               0x1
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CGCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ADDR                                         (LPASS_TCSR_REG_BASE      + 0x00019004)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_RMSK                                                0x7
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                 0x7
#define HWIO_LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                 0x0

#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_ADDR                                                     (LPASS_TCSR_REG_BASE      + 0x0001a000)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_ADDR, HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ADDR                                               (LPASS_TCSR_REG_BASE      + 0x0001a004)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_WRAPPER_Q6AHB_AON_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_ADDR                                                  (LPASS_TCSR_REG_BASE      + 0x0001b000)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_RMSK                                                         0x3
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_ADDR, HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_HW_CTL_BMSK                                                  0x2
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_HW_CTL_SHFT                                                  0x1
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CGCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ADDR                                            (LPASS_TCSR_REG_BASE      + 0x0001b004)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_RMSK                                                   0x7
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                    0x7
#define HWIO_LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                    0x0

#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_ADDR                                                             (LPASS_TCSR_REG_BASE      + 0x0001c000)
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_RMSK                                                                    0x3
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_IN          \
        in_dword_masked(HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_ADDR, HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_RMSK)
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_INM(m)      \
        in_dword_masked(HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_ADDR, m)
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_OUT(v)      \
        out_dword(HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_ADDR,v)
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_ADDR,m,v,HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_IN)
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_REQ_PRIORITY_BMSK                                                       0x3
#define HWIO_LPASS_Q6_SMMU_REQ_PRIORITY_REQ_PRIORITY_SHFT                                                       0x0

#define HWIO_LPASS_TCSR_QOS_CTL_ADDR                                                                     (LPASS_TCSR_REG_BASE      + 0x0001d000)
#define HWIO_LPASS_TCSR_QOS_CTL_RMSK                                                                            0x1
#define HWIO_LPASS_TCSR_QOS_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TCSR_QOS_CTL_ADDR, HWIO_LPASS_TCSR_QOS_CTL_RMSK)
#define HWIO_LPASS_TCSR_QOS_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TCSR_QOS_CTL_ADDR, m)
#define HWIO_LPASS_TCSR_QOS_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TCSR_QOS_CTL_ADDR,v)
#define HWIO_LPASS_TCSR_QOS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TCSR_QOS_CTL_ADDR,m,v,HWIO_LPASS_TCSR_QOS_CTL_IN)
#define HWIO_LPASS_TCSR_QOS_CTL_QOS_ENABLE_BMSK                                                                 0x1
#define HWIO_LPASS_TCSR_QOS_CTL_QOS_ENABLE_SHFT                                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_ADDR                                                             (LPASS_TCSR_REG_BASE      + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_RMSK                                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ASCID_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ASCID_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ASCID_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ASCID_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ASCID_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ASCID_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_AUDIO_CORE_AXI_ASCID_BMSK                                              0x1f
#define HWIO_LPASS_AUDIO_CORE_AXI_ASCID_AUDIO_CORE_AXI_ASCID_SHFT                                               0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_ADDR                                                      (LPASS_TCSR_REG_BASE      + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_RMSK                                                             0xf
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_AUDIO_CORE_AXI_ACACHEOPTYPE_BMSK                                 0xf
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEOPTYPE_AUDIO_CORE_AXI_ACACHEOPTYPE_SHFT                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_ADDR                                              (LPASS_TCSR_REG_BASE      + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_RMSK                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_AUDIO_CORE_AXI_ACGRANULETRANSACTION_BMSK                 0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ACGRANULETRANSACTION_AUDIO_CORE_AXI_ACGRANULETRANSACTION_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_ADDR                                                        (LPASS_TCSR_REG_BASE      + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_TCSR_AUDIO_CORE_AXI_ATRANSIENT_ENABLE_BMSK                         0x2
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_TCSR_AUDIO_CORE_AXI_ATRANSIENT_ENABLE_SHFT                         0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_AUDIO_CORE_AXI_ATRANSIENT_BMSK                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ATRANSIENT_AUDIO_CORE_AXI_ATRANSIENT_SHFT                                     0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_ADDR                                                        (LPASS_TCSR_REG_BASE      + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_TCSR_AUDIO_CORE_AXI_ACACHEABLE_ENABLE_BMSK                         0x2
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_TCSR_AUDIO_CORE_AXI_ACACHEABLE_ENABLE_SHFT                         0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_AUDIO_CORE_AXI_ACACHEABLE_BMSK                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ACACHEABLE_AUDIO_CORE_AXI_ACACHEABLE_SHFT                                     0x0

#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_ADDR                                                       (LPASS_TCSR_REG_BASE      + 0x0001e014)
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_RMSK                                                              0x3
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_ADDR, HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_IN)
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_TCSR_AUDIO_CORE_AXI_ANOALLOCATE_ENABLE_BMSK                       0x2
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_TCSR_AUDIO_CORE_AXI_ANOALLOCATE_ENABLE_SHFT                       0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_AUDIO_CORE_AXI_ANOALLOCATE_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_AXI_ANOALLOCATE_AUDIO_CORE_AXI_ANOALLOCATE_SHFT                                   0x0


#endif /* __HWIO_LPASS_TCSR_H__ */
