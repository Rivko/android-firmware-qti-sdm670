#ifndef __SNS_HWIO_CCD_PPE_H__
#define __SNS_HWIO_CCD_PPE_H__
/*
===========================================================================
*/
/**
  @file sns_hwio_ccd_ppe.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.4_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    CCD
    SSC_SCC_SCC_SCC_REG

  'Include' filters applied: CCD[SSC_SCC_SCC_SCC_REG] 
  'Exclude' filters applied: 
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

  $Header: //components/rel/ssc.slpi/3.2/inc/internal/sns_hwio_ccd_ppe.h#2 $
  $DateTime: 2018/03/27 11:46:46 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: CCD
 *--------------------------------------------------------------------------*/

#define CCD_REG_BASE                                                               (SSC_BASE      + 0x00b80000)

#define HWIO_CCD_HW_VERSION_ADDR                                                   (CCD_REG_BASE      + 0x00000000)
#define HWIO_CCD_HW_VERSION_RMSK                                                   0xffffffff
#define HWIO_CCD_HW_VERSION_IN          \
        in_dword_masked(HWIO_CCD_HW_VERSION_ADDR, HWIO_CCD_HW_VERSION_RMSK)
#define HWIO_CCD_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_CCD_HW_VERSION_ADDR, m)
#define HWIO_CCD_HW_VERSION_CCD_MAJOR_BMSK                                         0xf0000000
#define HWIO_CCD_HW_VERSION_CCD_MAJOR_SHFT                                               0x1c
#define HWIO_CCD_HW_VERSION_CCD_MINOR_BMSK                                          0xfff0000
#define HWIO_CCD_HW_VERSION_CCD_MINOR_SHFT                                               0x10
#define HWIO_CCD_HW_VERSION_CCD_STEP_BMSK                                              0xffff
#define HWIO_CCD_HW_VERSION_CCD_STEP_SHFT                                                 0x0

#define HWIO_CCD_Q6_CONTROL_ADDR                                                   (CCD_REG_BASE      + 0x00000004)
#define HWIO_CCD_Q6_CONTROL_RMSK                                                         0x3f
#define HWIO_CCD_Q6_CONTROL_IN          \
        in_dword_masked(HWIO_CCD_Q6_CONTROL_ADDR, HWIO_CCD_Q6_CONTROL_RMSK)
#define HWIO_CCD_Q6_CONTROL_INM(m)      \
        in_dword_masked(HWIO_CCD_Q6_CONTROL_ADDR, m)
#define HWIO_CCD_Q6_CONTROL_OUT(v)      \
        out_dword(HWIO_CCD_Q6_CONTROL_ADDR,v)
#define HWIO_CCD_Q6_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_Q6_CONTROL_ADDR,m,v,HWIO_CCD_Q6_CONTROL_IN)
#define HWIO_CCD_Q6_CONTROL_CCD_TOP_Q6_ENABLE_BMSK                                       0x20
#define HWIO_CCD_Q6_CONTROL_CCD_TOP_Q6_ENABLE_SHFT                                        0x5
#define HWIO_CCD_Q6_CONTROL_CCD_TILT_Q6_ENABLE_BMSK                                      0x10
#define HWIO_CCD_Q6_CONTROL_CCD_TILT_Q6_ENABLE_SHFT                                       0x4
#define HWIO_CCD_Q6_CONTROL_CCD_WALK_Q6_ENABLE_BMSK                                       0x8
#define HWIO_CCD_Q6_CONTROL_CCD_WALK_Q6_ENABLE_SHFT                                       0x3
#define HWIO_CCD_Q6_CONTROL_CCD_AMD_Q6_ENABLE_BMSK                                        0x4
#define HWIO_CCD_Q6_CONTROL_CCD_AMD_Q6_ENABLE_SHFT                                        0x2
#define HWIO_CCD_Q6_CONTROL_CCD_PPE_Q6_ENABLE_BMSK                                        0x2
#define HWIO_CCD_Q6_CONTROL_CCD_PPE_Q6_ENABLE_SHFT                                        0x1
#define HWIO_CCD_Q6_CONTROL_ALL_Q6_SDC_OVERRIDE_BMSK                                      0x1
#define HWIO_CCD_Q6_CONTROL_ALL_Q6_SDC_OVERRIDE_SHFT                                      0x0

#define HWIO_CCD_SDC_CONTROL_ADDR                                                  (CCD_REG_BASE      + 0x00000008)
#define HWIO_CCD_SDC_CONTROL_RMSK                                                        0x3e
#define HWIO_CCD_SDC_CONTROL_IN          \
        in_dword_masked(HWIO_CCD_SDC_CONTROL_ADDR, HWIO_CCD_SDC_CONTROL_RMSK)
#define HWIO_CCD_SDC_CONTROL_INM(m)      \
        in_dword_masked(HWIO_CCD_SDC_CONTROL_ADDR, m)
#define HWIO_CCD_SDC_CONTROL_OUT(v)      \
        out_dword(HWIO_CCD_SDC_CONTROL_ADDR,v)
#define HWIO_CCD_SDC_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_SDC_CONTROL_ADDR,m,v,HWIO_CCD_SDC_CONTROL_IN)
#define HWIO_CCD_SDC_CONTROL_CCD_TOP_SDC_ENABLE_BMSK                                     0x20
#define HWIO_CCD_SDC_CONTROL_CCD_TOP_SDC_ENABLE_SHFT                                      0x5
#define HWIO_CCD_SDC_CONTROL_CCD_TILT_SDC_ENABLE_BMSK                                    0x10
#define HWIO_CCD_SDC_CONTROL_CCD_TILT_SDC_ENABLE_SHFT                                     0x4
#define HWIO_CCD_SDC_CONTROL_CCD_WALK_SDC_ENABLE_BMSK                                     0x8
#define HWIO_CCD_SDC_CONTROL_CCD_WALK_SDC_ENABLE_SHFT                                     0x3
#define HWIO_CCD_SDC_CONTROL_CCD_AMD_SDC_ENABLE_BMSK                                      0x4
#define HWIO_CCD_SDC_CONTROL_CCD_AMD_SDC_ENABLE_SHFT                                      0x2
#define HWIO_CCD_SDC_CONTROL_CCD_PPE_SDC_ENABLE_BMSK                                      0x2
#define HWIO_CCD_SDC_CONTROL_CCD_PPE_SDC_ENABLE_SHFT                                      0x1

#define HWIO_CCD_Q6_INT_STATUS_ADDR                                                (CCD_REG_BASE      + 0x0000000c)
#define HWIO_CCD_Q6_INT_STATUS_RMSK                                                      0x7f
#define HWIO_CCD_Q6_INT_STATUS_IN          \
        in_dword_masked(HWIO_CCD_Q6_INT_STATUS_ADDR, HWIO_CCD_Q6_INT_STATUS_RMSK)
#define HWIO_CCD_Q6_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_CCD_Q6_INT_STATUS_ADDR, m)
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_IN_FIFO_FULL_INT_STATUS_BMSK                   0x40
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_IN_FIFO_FULL_INT_STATUS_SHFT                    0x6
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_OUT_FIFO_FULL_INT_STATUS_BMSK                  0x20
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_OUT_FIFO_FULL_INT_STATUS_SHFT                   0x5
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_TILT_INT_STATUS_BMSK                               0x10
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_TILT_INT_STATUS_SHFT                                0x4
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_WALK_INT_STATUS_BMSK                                0x8
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_WALK_INT_STATUS_SHFT                                0x3
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_AMD_INT2_STATUS_BMSK                                0x4
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_AMD_INT2_STATUS_SHFT                                0x2
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_AMD_INT1_STATUS_BMSK                                0x2
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_AMD_INT1_STATUS_SHFT                                0x1
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_INT_STATUS_BMSK                                 0x1
#define HWIO_CCD_Q6_INT_STATUS_CCD_Q6_PPE_INT_STATUS_SHFT                                 0x0

#define HWIO_CCD_Q6_INT_CLEAR_ADDR                                                 (CCD_REG_BASE      + 0x00000010)
#define HWIO_CCD_Q6_INT_CLEAR_RMSK                                                       0x7f
#define HWIO_CCD_Q6_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_CCD_Q6_INT_CLEAR_ADDR,v)
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_IN_FIFO_FULL_INT_CLEAR_BMSK                     0x40
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_IN_FIFO_FULL_INT_CLEAR_SHFT                      0x6
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_OUT_FIFO_FULL_INT_CLEAR_BMSK                    0x20
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_OUT_FIFO_FULL_INT_CLEAR_SHFT                     0x5
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_TILT_INT_CLEAR_BMSK                                 0x10
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_TILT_INT_CLEAR_SHFT                                  0x4
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_WALK_INT_CLEAR_BMSK                                  0x8
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_WALK_INT_CLEAR_SHFT                                  0x3
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_AMD_INT2_CLEAR_BMSK                                  0x4
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_AMD_INT2_CLEAR_SHFT                                  0x2
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_AMD_INT1_CLEAR_BMSK                                  0x2
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_AMD_INT1_CLEAR_SHFT                                  0x1
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_INT_CLEAR_BMSK                                   0x1
#define HWIO_CCD_Q6_INT_CLEAR_CCD_Q6_PPE_INT_CLEAR_SHFT                                   0x0

#define HWIO_CCD_Q6_INT_MASK_ADDR                                                  (CCD_REG_BASE      + 0x00000014)
#define HWIO_CCD_Q6_INT_MASK_RMSK                                                        0x7f
#define HWIO_CCD_Q6_INT_MASK_IN          \
        in_dword_masked(HWIO_CCD_Q6_INT_MASK_ADDR, HWIO_CCD_Q6_INT_MASK_RMSK)
#define HWIO_CCD_Q6_INT_MASK_INM(m)      \
        in_dword_masked(HWIO_CCD_Q6_INT_MASK_ADDR, m)
#define HWIO_CCD_Q6_INT_MASK_OUT(v)      \
        out_dword(HWIO_CCD_Q6_INT_MASK_ADDR,v)
#define HWIO_CCD_Q6_INT_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_Q6_INT_MASK_ADDR,m,v,HWIO_CCD_Q6_INT_MASK_IN)
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_IN_FIFO_FULL_INT_MASK_BMSK                       0x40
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_IN_FIFO_FULL_INT_MASK_SHFT                        0x6
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_OUT_FIFO_FULL_INT_MASK_BMSK                      0x20
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_OUT_FIFO_FULL_INT_MASK_SHFT                       0x5
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_TILT_INT_MASK_BMSK                                   0x10
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_TILT_INT_MASK_SHFT                                    0x4
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_WALK_INT_MASK_BMSK                                    0x8
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_WALK_INT_MASK_SHFT                                    0x3
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_AMD_INT2_MASK_BMSK                                    0x4
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_AMD_INT2_MASK_SHFT                                    0x2
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_AMD_INT1_MASK_BMSK                                    0x2
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_AMD_INT1_MASK_SHFT                                    0x1
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_INT_MASK_BMSK                                     0x1
#define HWIO_CCD_Q6_INT_MASK_CCD_Q6_PPE_INT_MASK_SHFT                                     0x0

#define HWIO_CCD_SDC_INT_STATUS_ADDR                                               (CCD_REG_BASE      + 0x00000018)
#define HWIO_CCD_SDC_INT_STATUS_RMSK                                                     0x1f
#define HWIO_CCD_SDC_INT_STATUS_IN          \
        in_dword_masked(HWIO_CCD_SDC_INT_STATUS_ADDR, HWIO_CCD_SDC_INT_STATUS_RMSK)
#define HWIO_CCD_SDC_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_CCD_SDC_INT_STATUS_ADDR, m)
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_IN_FIFO_FULL_INT_STATUS_BMSK                 0x10
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_IN_FIFO_FULL_INT_STATUS_SHFT                  0x4
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_OUT_FIFO_FULL_INT_STATUS_BMSK                 0x8
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_OUT_FIFO_FULL_INT_STATUS_SHFT                 0x3
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_AMD_INT2_STATUS_BMSK                              0x4
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_AMD_INT2_STATUS_SHFT                              0x2
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_AMD_INT1_STATUS_BMSK                              0x2
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_AMD_INT1_STATUS_SHFT                              0x1
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_INT_STATUS_BMSK                               0x1
#define HWIO_CCD_SDC_INT_STATUS_CCD_SDC_PPE_INT_STATUS_SHFT                               0x0

#define HWIO_CCD_SDC_INT_CLEAR_ADDR                                                (CCD_REG_BASE      + 0x0000001c)
#define HWIO_CCD_SDC_INT_CLEAR_RMSK                                                      0x1f
#define HWIO_CCD_SDC_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_CCD_SDC_INT_CLEAR_ADDR,v)
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_IN_FIFO_FULL_INT_CLEAR_BMSK                   0x10
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_IN_FIFO_FULL_INT_CLEAR_SHFT                    0x4
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_OUT_FIFO_FULL_INT_CLEAR_BMSK                   0x8
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_OUT_FIFO_FULL_INT_CLEAR_SHFT                   0x3
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_AMD_INT2_CLEAR_BMSK                                0x4
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_AMD_INT2_CLEAR_SHFT                                0x2
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_AMD_INT1_CLEAR_BMSK                                0x2
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_AMD_INT1_CLEAR_SHFT                                0x1
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_INT_CLEAR_BMSK                                 0x1
#define HWIO_CCD_SDC_INT_CLEAR_CCD_SDC_PPE_INT_CLEAR_SHFT                                 0x0

#define HWIO_CCD_SDC_INT_MASK_ADDR                                                 (CCD_REG_BASE      + 0x00000020)
#define HWIO_CCD_SDC_INT_MASK_RMSK                                                       0x1f
#define HWIO_CCD_SDC_INT_MASK_IN          \
        in_dword_masked(HWIO_CCD_SDC_INT_MASK_ADDR, HWIO_CCD_SDC_INT_MASK_RMSK)
#define HWIO_CCD_SDC_INT_MASK_INM(m)      \
        in_dword_masked(HWIO_CCD_SDC_INT_MASK_ADDR, m)
#define HWIO_CCD_SDC_INT_MASK_OUT(v)      \
        out_dword(HWIO_CCD_SDC_INT_MASK_ADDR,v)
#define HWIO_CCD_SDC_INT_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_SDC_INT_MASK_ADDR,m,v,HWIO_CCD_SDC_INT_MASK_IN)
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_IN_FIFO_FULL_INT_MASK_BMSK                     0x10
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_IN_FIFO_FULL_INT_MASK_SHFT                      0x4
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_OUT_FIFO_FULL_INT_MASK_BMSK                     0x8
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_OUT_FIFO_FULL_INT_MASK_SHFT                     0x3
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_AMD_INT2_MASK_BMSK                                  0x4
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_AMD_INT2_MASK_SHFT                                  0x2
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_AMD_INT1_MASK_BMSK                                  0x2
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_AMD_INT1_MASK_SHFT                                  0x1
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_INT_MASK_BMSK                                   0x1
#define HWIO_CCD_SDC_INT_MASK_CCD_SDC_PPE_INT_MASK_SHFT                                   0x0

#define HWIO_CCD_BLOCK_STATE_ADDR                                                  (CCD_REG_BASE      + 0x00000024)
#define HWIO_CCD_BLOCK_STATE_RMSK                                                        0x7f
#define HWIO_CCD_BLOCK_STATE_IN          \
        in_dword_masked(HWIO_CCD_BLOCK_STATE_ADDR, HWIO_CCD_BLOCK_STATE_RMSK)
#define HWIO_CCD_BLOCK_STATE_INM(m)      \
        in_dword_masked(HWIO_CCD_BLOCK_STATE_ADDR, m)
#define HWIO_CCD_BLOCK_STATE_CCD_AMD_STATE_CFG2_BMSK                                     0x60
#define HWIO_CCD_BLOCK_STATE_CCD_AMD_STATE_CFG2_SHFT                                      0x5
#define HWIO_CCD_BLOCK_STATE_CCD_AMD_STATE_CFG1_BMSK                                     0x18
#define HWIO_CCD_BLOCK_STATE_CCD_AMD_STATE_CFG1_SHFT                                      0x3
#define HWIO_CCD_BLOCK_STATE_CCD_PPE_ERROR_CODE_BMSK                                      0x7
#define HWIO_CCD_BLOCK_STATE_CCD_PPE_ERROR_CODE_SHFT                                      0x0

#define HWIO_CCD_DEBUG_CONTROL_ADDR                                                (CCD_REG_BASE      + 0x00000028)
#define HWIO_CCD_DEBUG_CONTROL_RMSK                                                     0xfff
#define HWIO_CCD_DEBUG_CONTROL_IN          \
        in_dword_masked(HWIO_CCD_DEBUG_CONTROL_ADDR, HWIO_CCD_DEBUG_CONTROL_RMSK)
#define HWIO_CCD_DEBUG_CONTROL_INM(m)      \
        in_dword_masked(HWIO_CCD_DEBUG_CONTROL_ADDR, m)
#define HWIO_CCD_DEBUG_CONTROL_OUT(v)      \
        out_dword(HWIO_CCD_DEBUG_CONTROL_ADDR,v)
#define HWIO_CCD_DEBUG_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_DEBUG_CONTROL_ADDR,m,v,HWIO_CCD_DEBUG_CONTROL_IN)
#define HWIO_CCD_DEBUG_CONTROL_CCD_DEBUG_BLOCK_SELECT_BMSK                              0xf00
#define HWIO_CCD_DEBUG_CONTROL_CCD_DEBUG_BLOCK_SELECT_SHFT                                0x8
#define HWIO_CCD_DEBUG_CONTROL_CCD_DEBUG_SIGNAL_SELECT_BMSK                              0xff
#define HWIO_CCD_DEBUG_CONTROL_CCD_DEBUG_SIGNAL_SELECT_SHFT                               0x0

#define HWIO_CCD_QDSS_CONTROL_ADDR                                                 (CCD_REG_BASE      + 0x0000002c)
#define HWIO_CCD_QDSS_CONTROL_RMSK                                                      0x103
#define HWIO_CCD_QDSS_CONTROL_IN          \
        in_dword_masked(HWIO_CCD_QDSS_CONTROL_ADDR, HWIO_CCD_QDSS_CONTROL_RMSK)
#define HWIO_CCD_QDSS_CONTROL_INM(m)      \
        in_dword_masked(HWIO_CCD_QDSS_CONTROL_ADDR, m)
#define HWIO_CCD_QDSS_CONTROL_OUT(v)      \
        out_dword(HWIO_CCD_QDSS_CONTROL_ADDR,v)
#define HWIO_CCD_QDSS_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_QDSS_CONTROL_ADDR,m,v,HWIO_CCD_QDSS_CONTROL_IN)
#define HWIO_CCD_QDSS_CONTROL_CCD_QDSS_ENABLE_BMSK                                      0x100
#define HWIO_CCD_QDSS_CONTROL_CCD_QDSS_ENABLE_SHFT                                        0x8
#define HWIO_CCD_QDSS_CONTROL_CCD_QDSS_SIGNAL_SET_SELECT_BMSK                             0x3
#define HWIO_CCD_QDSS_CONTROL_CCD_QDSS_SIGNAL_SET_SELECT_SHFT                             0x0

#define HWIO_CCD_SPARE_ADDR                                                        (CCD_REG_BASE      + 0x00000030)
#define HWIO_CCD_SPARE_RMSK                                                        0xffffffff
#define HWIO_CCD_SPARE_IN          \
        in_dword_masked(HWIO_CCD_SPARE_ADDR, HWIO_CCD_SPARE_RMSK)
#define HWIO_CCD_SPARE_INM(m)      \
        in_dword_masked(HWIO_CCD_SPARE_ADDR, m)
#define HWIO_CCD_SPARE_OUT(v)      \
        out_dword(HWIO_CCD_SPARE_ADDR,v)
#define HWIO_CCD_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CCD_SPARE_ADDR,m,v,HWIO_CCD_SPARE_IN)
#define HWIO_CCD_SPARE_CCD_SPARE_BMSK                                              0xffffffff
#define HWIO_CCD_SPARE_CCD_SPARE_SHFT                                                     0x0

#define HWIO_PPE_HW_VERSION_ADDR                                                   (CCD_REG_BASE      + 0x00001000)
#define HWIO_PPE_HW_VERSION_RMSK                                                   0xffffffff
#define HWIO_PPE_HW_VERSION_IN          \
        in_dword_masked(HWIO_PPE_HW_VERSION_ADDR, HWIO_PPE_HW_VERSION_RMSK)
#define HWIO_PPE_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_PPE_HW_VERSION_ADDR, m)
#define HWIO_PPE_HW_VERSION_PPE_VERSION_MAJOR_BMSK                                 0xf0000000
#define HWIO_PPE_HW_VERSION_PPE_VERSION_MAJOR_SHFT                                       0x1c
#define HWIO_PPE_HW_VERSION_PPE_VERSION_MINOR_BMSK                                  0xfff0000
#define HWIO_PPE_HW_VERSION_PPE_VERSION_MINOR_SHFT                                       0x10
#define HWIO_PPE_HW_VERSION_PPE_VERSION_STEP_BMSK                                      0xffff
#define HWIO_PPE_HW_VERSION_PPE_VERSION_STEP_SHFT                                         0x0

#define HWIO_PPE_ACCEL_XY_ADDR                                                     (CCD_REG_BASE      + 0x00001004)
#define HWIO_PPE_ACCEL_XY_RMSK                                                     0xffffffff
#define HWIO_PPE_ACCEL_XY_OUT(v)      \
        out_dword(HWIO_PPE_ACCEL_XY_ADDR,v)
#define HWIO_PPE_ACCEL_XY_PPE_ACCEL_Y_BMSK                                         0xffff0000
#define HWIO_PPE_ACCEL_XY_PPE_ACCEL_Y_SHFT                                               0x10
#define HWIO_PPE_ACCEL_XY_PPE_ACCEL_X_BMSK                                             0xffff
#define HWIO_PPE_ACCEL_XY_PPE_ACCEL_X_SHFT                                                0x0

#define HWIO_PPE_ACCEL_Z_ADDR                                                      (CCD_REG_BASE      + 0x00001008)
#define HWIO_PPE_ACCEL_Z_RMSK                                                          0xffff
#define HWIO_PPE_ACCEL_Z_OUT(v)      \
        out_dword(HWIO_PPE_ACCEL_Z_ADDR,v)
#define HWIO_PPE_ACCEL_Z_PPE_ACCEL_Z_BMSK                                              0xffff
#define HWIO_PPE_ACCEL_Z_PPE_ACCEL_Z_SHFT                                                 0x0

#define HWIO_PPE_ACCEL_TS_ADDR                                                     (CCD_REG_BASE      + 0x0000100c)
#define HWIO_PPE_ACCEL_TS_RMSK                                                     0xffffffff
#define HWIO_PPE_ACCEL_TS_OUT(v)      \
        out_dword(HWIO_PPE_ACCEL_TS_ADDR,v)
#define HWIO_PPE_ACCEL_TS_PPE_ACCEL_TIMESTAMP_BMSK                                 0xffffffff
#define HWIO_PPE_ACCEL_TS_PPE_ACCEL_TIMESTAMP_SHFT                                        0x0

#define HWIO_PPE_CONTROL_1_ADDR                                                    (CCD_REG_BASE      + 0x00001010)
#define HWIO_PPE_CONTROL_1_RMSK                                                     0x1ffffff
#define HWIO_PPE_CONTROL_1_IN          \
        in_dword_masked(HWIO_PPE_CONTROL_1_ADDR, HWIO_PPE_CONTROL_1_RMSK)
#define HWIO_PPE_CONTROL_1_INM(m)      \
        in_dword_masked(HWIO_PPE_CONTROL_1_ADDR, m)
#define HWIO_PPE_CONTROL_1_OUT(v)      \
        out_dword(HWIO_PPE_CONTROL_1_ADDR,v)
#define HWIO_PPE_CONTROL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_CONTROL_1_ADDR,m,v,HWIO_PPE_CONTROL_1_IN)
#define HWIO_PPE_CONTROL_1_PPE_IRQ_MASK_BMSK                                        0x1c00000
#define HWIO_PPE_CONTROL_1_PPE_IRQ_MASK_SHFT                                             0x16
#define HWIO_PPE_CONTROL_1_PPE_RAW_ACCEL_SHIFT_BMSK                                  0x380000
#define HWIO_PPE_CONTROL_1_PPE_RAW_ACCEL_SHIFT_SHFT                                      0x13
#define HWIO_PPE_CONTROL_1_PPE_LOG2_CIC_DEC_RATIO_BMSK                                0x70000
#define HWIO_PPE_CONTROL_1_PPE_LOG2_CIC_DEC_RATIO_SHFT                                   0x10
#define HWIO_PPE_CONTROL_1_PPE_INTERP_OUT_PERIOD_TSU_BMSK                              0xffff
#define HWIO_PPE_CONTROL_1_PPE_INTERP_OUT_PERIOD_TSU_SHFT                                 0x0

#define HWIO_PPE_CONTROL_2_ADDR                                                    (CCD_REG_BASE      + 0x00001014)
#define HWIO_PPE_CONTROL_2_RMSK                                                       0xfffff
#define HWIO_PPE_CONTROL_2_IN          \
        in_dword_masked(HWIO_PPE_CONTROL_2_ADDR, HWIO_PPE_CONTROL_2_RMSK)
#define HWIO_PPE_CONTROL_2_INM(m)      \
        in_dword_masked(HWIO_PPE_CONTROL_2_ADDR, m)
#define HWIO_PPE_CONTROL_2_OUT(v)      \
        out_dword(HWIO_PPE_CONTROL_2_ADDR,v)
#define HWIO_PPE_CONTROL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_CONTROL_2_ADDR,m,v,HWIO_PPE_CONTROL_2_IN)
#define HWIO_PPE_CONTROL_2_PPE_TS_GAP_THRESH_TSU_BMSK                                 0xfffff
#define HWIO_PPE_CONTROL_2_PPE_TS_GAP_THRESH_TSU_SHFT                                     0x0

#define HWIO_PPE_ACCEL_OFFSET_CAL_ADDR                                             (CCD_REG_BASE      + 0x00001018)
#define HWIO_PPE_ACCEL_OFFSET_CAL_RMSK                                             0x3fffffff
#define HWIO_PPE_ACCEL_OFFSET_CAL_IN          \
        in_dword_masked(HWIO_PPE_ACCEL_OFFSET_CAL_ADDR, HWIO_PPE_ACCEL_OFFSET_CAL_RMSK)
#define HWIO_PPE_ACCEL_OFFSET_CAL_INM(m)      \
        in_dword_masked(HWIO_PPE_ACCEL_OFFSET_CAL_ADDR, m)
#define HWIO_PPE_ACCEL_OFFSET_CAL_OUT(v)      \
        out_dword(HWIO_PPE_ACCEL_OFFSET_CAL_ADDR,v)
#define HWIO_PPE_ACCEL_OFFSET_CAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_ACCEL_OFFSET_CAL_ADDR,m,v,HWIO_PPE_ACCEL_OFFSET_CAL_IN)
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_Z_BMSK                      0x3ff00000
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_Z_SHFT                            0x14
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_Y_BMSK                         0xffc00
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_Y_SHFT                             0xa
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_X_BMSK                           0x3ff
#define HWIO_PPE_ACCEL_OFFSET_CAL_PPE_ACCEL_OFFSET_CAL_X_SHFT                             0x0

#define HWIO_PPE_ACCEL_SCALE_CAL_ADDR                                              (CCD_REG_BASE      + 0x0000101c)
#define HWIO_PPE_ACCEL_SCALE_CAL_RMSK                                                0xffffff
#define HWIO_PPE_ACCEL_SCALE_CAL_IN          \
        in_dword_masked(HWIO_PPE_ACCEL_SCALE_CAL_ADDR, HWIO_PPE_ACCEL_SCALE_CAL_RMSK)
#define HWIO_PPE_ACCEL_SCALE_CAL_INM(m)      \
        in_dword_masked(HWIO_PPE_ACCEL_SCALE_CAL_ADDR, m)
#define HWIO_PPE_ACCEL_SCALE_CAL_OUT(v)      \
        out_dword(HWIO_PPE_ACCEL_SCALE_CAL_ADDR,v)
#define HWIO_PPE_ACCEL_SCALE_CAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_ACCEL_SCALE_CAL_ADDR,m,v,HWIO_PPE_ACCEL_SCALE_CAL_IN)
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_Z_BMSK                          0xff0000
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_Z_SHFT                              0x10
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_Y_BMSK                            0xff00
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_Y_SHFT                               0x8
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_X_BMSK                              0xff
#define HWIO_PPE_ACCEL_SCALE_CAL_PPE_ACCEL_SCALE_CAL_X_SHFT                               0x0

#define HWIO_PPE_FIFO_OUT_DELAY_ADDR                                               (CCD_REG_BASE      + 0x00001020)
#define HWIO_PPE_FIFO_OUT_DELAY_RMSK                                               0xffffffff
#define HWIO_PPE_FIFO_OUT_DELAY_IN          \
        in_dword_masked(HWIO_PPE_FIFO_OUT_DELAY_ADDR, HWIO_PPE_FIFO_OUT_DELAY_RMSK)
#define HWIO_PPE_FIFO_OUT_DELAY_INM(m)      \
        in_dword_masked(HWIO_PPE_FIFO_OUT_DELAY_ADDR, m)
#define HWIO_PPE_FIFO_OUT_DELAY_OUT(v)      \
        out_dword(HWIO_PPE_FIFO_OUT_DELAY_ADDR,v)
#define HWIO_PPE_FIFO_OUT_DELAY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_FIFO_OUT_DELAY_ADDR,m,v,HWIO_PPE_FIFO_OUT_DELAY_IN)
#define HWIO_PPE_FIFO_OUT_DELAY_PPE_FIFO_OUT_DELAY_BMSK                            0xffffffff
#define HWIO_PPE_FIFO_OUT_DELAY_PPE_FIFO_OUT_DELAY_SHFT                                   0x0

#define HWIO_PPE_SPARE_ADDR                                                        (CCD_REG_BASE      + 0x00001024)
#define HWIO_PPE_SPARE_RMSK                                                        0xffffffff
#define HWIO_PPE_SPARE_IN          \
        in_dword_masked(HWIO_PPE_SPARE_ADDR, HWIO_PPE_SPARE_RMSK)
#define HWIO_PPE_SPARE_INM(m)      \
        in_dword_masked(HWIO_PPE_SPARE_ADDR, m)
#define HWIO_PPE_SPARE_OUT(v)      \
        out_dword(HWIO_PPE_SPARE_ADDR,v)
#define HWIO_PPE_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PPE_SPARE_ADDR,m,v,HWIO_PPE_SPARE_IN)
#define HWIO_PPE_SPARE_PPE_SPARE_BMSK                                              0xffffffff
#define HWIO_PPE_SPARE_PPE_SPARE_SHFT                                                     0x0

#define HWIO_AMD_HW_VERSION_ADDR                                                   (CCD_REG_BASE      + 0x00002000)
#define HWIO_AMD_HW_VERSION_RMSK                                                   0xffffffff
#define HWIO_AMD_HW_VERSION_IN          \
        in_dword_masked(HWIO_AMD_HW_VERSION_ADDR, HWIO_AMD_HW_VERSION_RMSK)
#define HWIO_AMD_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_AMD_HW_VERSION_ADDR, m)
#define HWIO_AMD_HW_VERSION_AMD_VERSION_MAJOR_BMSK                                 0xf0000000
#define HWIO_AMD_HW_VERSION_AMD_VERSION_MAJOR_SHFT                                       0x1c
#define HWIO_AMD_HW_VERSION_AMD_VERSION_MINOR_BMSK                                  0xfff0000
#define HWIO_AMD_HW_VERSION_AMD_VERSION_MINOR_SHFT                                       0x10
#define HWIO_AMD_HW_VERSION_AMD_VERSION_STEP_BMSK                                      0xffff
#define HWIO_AMD_HW_VERSION_AMD_VERSION_STEP_SHFT                                         0x0

#define HWIO_AMD_CONTROL_CFG2_ADDR                                                 (CCD_REG_BASE      + 0x00002004)
#define HWIO_AMD_CONTROL_CFG2_RMSK                                                      0xfff
#define HWIO_AMD_CONTROL_CFG2_IN          \
        in_dword_masked(HWIO_AMD_CONTROL_CFG2_ADDR, HWIO_AMD_CONTROL_CFG2_RMSK)
#define HWIO_AMD_CONTROL_CFG2_INM(m)      \
        in_dword_masked(HWIO_AMD_CONTROL_CFG2_ADDR, m)
#define HWIO_AMD_CONTROL_CFG2_OUT(v)      \
        out_dword(HWIO_AMD_CONTROL_CFG2_ADDR,v)
#define HWIO_AMD_CONTROL_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AMD_CONTROL_CFG2_ADDR,m,v,HWIO_AMD_CONTROL_CFG2_IN)
#define HWIO_AMD_CONTROL_CFG2_AMD_HYSTERESIS_CFG2_BMSK                                  0xfc0
#define HWIO_AMD_CONTROL_CFG2_AMD_HYSTERESIS_CFG2_SHFT                                    0x6
#define HWIO_AMD_CONTROL_CFG2_AMD_THRESHOLD_CFG2_BMSK                                    0x3f
#define HWIO_AMD_CONTROL_CFG2_AMD_THRESHOLD_CFG2_SHFT                                     0x0

#define HWIO_AMD_CONTROL_CFG1_ADDR                                                 (CCD_REG_BASE      + 0x00002008)
#define HWIO_AMD_CONTROL_CFG1_RMSK                                                      0xfff
#define HWIO_AMD_CONTROL_CFG1_IN          \
        in_dword_masked(HWIO_AMD_CONTROL_CFG1_ADDR, HWIO_AMD_CONTROL_CFG1_RMSK)
#define HWIO_AMD_CONTROL_CFG1_INM(m)      \
        in_dword_masked(HWIO_AMD_CONTROL_CFG1_ADDR, m)
#define HWIO_AMD_CONTROL_CFG1_OUT(v)      \
        out_dword(HWIO_AMD_CONTROL_CFG1_ADDR,v)
#define HWIO_AMD_CONTROL_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AMD_CONTROL_CFG1_ADDR,m,v,HWIO_AMD_CONTROL_CFG1_IN)
#define HWIO_AMD_CONTROL_CFG1_AMD_HYSTERESIS_CFG1_BMSK                                  0xfc0
#define HWIO_AMD_CONTROL_CFG1_AMD_HYSTERESIS_CFG1_SHFT                                    0x6
#define HWIO_AMD_CONTROL_CFG1_AMD_THRESHOLD_CFG1_BMSK                                    0x3f
#define HWIO_AMD_CONTROL_CFG1_AMD_THRESHOLD_CFG1_SHFT                                     0x0

#define HWIO_AMD_EVENT_TS_CFG2_ADDR                                                (CCD_REG_BASE      + 0x0000200c)
#define HWIO_AMD_EVENT_TS_CFG2_RMSK                                                0xffffffff
#define HWIO_AMD_EVENT_TS_CFG2_IN          \
        in_dword_masked(HWIO_AMD_EVENT_TS_CFG2_ADDR, HWIO_AMD_EVENT_TS_CFG2_RMSK)
#define HWIO_AMD_EVENT_TS_CFG2_INM(m)      \
        in_dword_masked(HWIO_AMD_EVENT_TS_CFG2_ADDR, m)
#define HWIO_AMD_EVENT_TS_CFG2_AMD_EVENT_TS_CFG2_BMSK                              0xffffffff
#define HWIO_AMD_EVENT_TS_CFG2_AMD_EVENT_TS_CFG2_SHFT                                     0x0

#define HWIO_AMD_EVENT_TS_CFG1_ADDR                                                (CCD_REG_BASE      + 0x00002010)
#define HWIO_AMD_EVENT_TS_CFG1_RMSK                                                0xffffffff
#define HWIO_AMD_EVENT_TS_CFG1_IN          \
        in_dword_masked(HWIO_AMD_EVENT_TS_CFG1_ADDR, HWIO_AMD_EVENT_TS_CFG1_RMSK)
#define HWIO_AMD_EVENT_TS_CFG1_INM(m)      \
        in_dword_masked(HWIO_AMD_EVENT_TS_CFG1_ADDR, m)
#define HWIO_AMD_EVENT_TS_CFG1_AMD_EVENT_TS_CFG1_BMSK                              0xffffffff
#define HWIO_AMD_EVENT_TS_CFG1_AMD_EVENT_TS_CFG1_SHFT                                     0x0

#define HWIO_AMD_SPARE_ADDR                                                        (CCD_REG_BASE      + 0x00002014)
#define HWIO_AMD_SPARE_RMSK                                                        0xffffffff
#define HWIO_AMD_SPARE_IN          \
        in_dword_masked(HWIO_AMD_SPARE_ADDR, HWIO_AMD_SPARE_RMSK)
#define HWIO_AMD_SPARE_INM(m)      \
        in_dword_masked(HWIO_AMD_SPARE_ADDR, m)
#define HWIO_AMD_SPARE_OUT(v)      \
        out_dword(HWIO_AMD_SPARE_ADDR,v)
#define HWIO_AMD_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AMD_SPARE_ADDR,m,v,HWIO_AMD_SPARE_IN)
#define HWIO_AMD_SPARE_AMD_SPARE_BMSK                                              0xffffffff
#define HWIO_AMD_SPARE_AMD_SPARE_SHFT                                                     0x0

#define HWIO_WALK_HW_VERSION_ADDR                                                  (CCD_REG_BASE      + 0x00003000)
#define HWIO_WALK_HW_VERSION_RMSK                                                  0xffffffff
#define HWIO_WALK_HW_VERSION_IN          \
        in_dword_masked(HWIO_WALK_HW_VERSION_ADDR, HWIO_WALK_HW_VERSION_RMSK)
#define HWIO_WALK_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_WALK_HW_VERSION_ADDR, m)
#define HWIO_WALK_HW_VERSION_WALK_VERSION_MAJOR_BMSK                               0xf0000000
#define HWIO_WALK_HW_VERSION_WALK_VERSION_MAJOR_SHFT                                     0x1c
#define HWIO_WALK_HW_VERSION_WALK_VERSION_MINOR_BMSK                                0xfff0000
#define HWIO_WALK_HW_VERSION_WALK_VERSION_MINOR_SHFT                                     0x10
#define HWIO_WALK_HW_VERSION_WALK_VERSION_STEP_BMSK                                    0xffff
#define HWIO_WALK_HW_VERSION_WALK_VERSION_STEP_SHFT                                       0x0

#define HWIO_WALK_CONTROL_ADDR                                                     (CCD_REG_BASE      + 0x00003004)
#define HWIO_WALK_CONTROL_RMSK                                                      0x7ffffff
#define HWIO_WALK_CONTROL_IN          \
        in_dword_masked(HWIO_WALK_CONTROL_ADDR, HWIO_WALK_CONTROL_RMSK)
#define HWIO_WALK_CONTROL_INM(m)      \
        in_dword_masked(HWIO_WALK_CONTROL_ADDR, m)
#define HWIO_WALK_CONTROL_OUT(v)      \
        out_dword(HWIO_WALK_CONTROL_ADDR,v)
#define HWIO_WALK_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WALK_CONTROL_ADDR,m,v,HWIO_WALK_CONTROL_IN)
#define HWIO_WALK_CONTROL_WALK_HS_MAX_PEAK_SEP_BMSK                                 0x7000000
#define HWIO_WALK_CONTROL_WALK_HS_MAX_PEAK_SEP_SHFT                                      0x18
#define HWIO_WALK_CONTROL_WALK_MIN_PEAK_HALFSTEP_BMSK                                0xff8000
#define HWIO_WALK_CONTROL_WALK_MIN_PEAK_HALFSTEP_SHFT                                     0xf
#define HWIO_WALK_CONTROL_WALK_XCOR_THRESH_BMSK                                        0x7ff0
#define HWIO_WALK_CONTROL_WALK_XCOR_THRESH_SHFT                                           0x4
#define HWIO_WALK_CONTROL_WALK_MAX_LAG_SEP_BMSK                                           0xf
#define HWIO_WALK_CONTROL_WALK_MAX_LAG_SEP_SHFT                                           0x0

#define HWIO_WALK_EVENT_TS_ADDR                                                    (CCD_REG_BASE      + 0x00003008)
#define HWIO_WALK_EVENT_TS_RMSK                                                    0xffffffff
#define HWIO_WALK_EVENT_TS_IN          \
        in_dword_masked(HWIO_WALK_EVENT_TS_ADDR, HWIO_WALK_EVENT_TS_RMSK)
#define HWIO_WALK_EVENT_TS_INM(m)      \
        in_dword_masked(HWIO_WALK_EVENT_TS_ADDR, m)
#define HWIO_WALK_EVENT_TS_WALK_EVENT_TS_BMSK                                      0xffffffff
#define HWIO_WALK_EVENT_TS_WALK_EVENT_TS_SHFT                                             0x0

#define HWIO_WALK_SPARE_ADDR                                                       (CCD_REG_BASE      + 0x0000300c)
#define HWIO_WALK_SPARE_RMSK                                                       0xffffffff
#define HWIO_WALK_SPARE_IN          \
        in_dword_masked(HWIO_WALK_SPARE_ADDR, HWIO_WALK_SPARE_RMSK)
#define HWIO_WALK_SPARE_INM(m)      \
        in_dword_masked(HWIO_WALK_SPARE_ADDR, m)
#define HWIO_WALK_SPARE_OUT(v)      \
        out_dword(HWIO_WALK_SPARE_ADDR,v)
#define HWIO_WALK_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WALK_SPARE_ADDR,m,v,HWIO_WALK_SPARE_IN)
#define HWIO_WALK_SPARE_WALK_SPARE_BMSK                                            0xffffffff
#define HWIO_WALK_SPARE_WALK_SPARE_SHFT                                                   0x0

#define HWIO_TILT_HW_VERSION_ADDR                                                  (CCD_REG_BASE      + 0x00004000)
#define HWIO_TILT_HW_VERSION_RMSK                                                  0xffffffff
#define HWIO_TILT_HW_VERSION_IN          \
        in_dword_masked(HWIO_TILT_HW_VERSION_ADDR, HWIO_TILT_HW_VERSION_RMSK)
#define HWIO_TILT_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_TILT_HW_VERSION_ADDR, m)
#define HWIO_TILT_HW_VERSION_TILT_VERSION_MAJOR_BMSK                               0xf0000000
#define HWIO_TILT_HW_VERSION_TILT_VERSION_MAJOR_SHFT                                     0x1c
#define HWIO_TILT_HW_VERSION_TILT_VERSION_MINOR_BMSK                                0xfff0000
#define HWIO_TILT_HW_VERSION_TILT_VERSION_MINOR_SHFT                                     0x10
#define HWIO_TILT_HW_VERSION_TILT_VERSION_STEP_BMSK                                    0xffff
#define HWIO_TILT_HW_VERSION_TILT_VERSION_STEP_SHFT                                       0x0

#define HWIO_TILT_CONTROL_ADDR                                                     (CCD_REG_BASE      + 0x00004004)
#define HWIO_TILT_CONTROL_RMSK                                                       0x3fffff
#define HWIO_TILT_CONTROL_IN          \
        in_dword_masked(HWIO_TILT_CONTROL_ADDR, HWIO_TILT_CONTROL_RMSK)
#define HWIO_TILT_CONTROL_INM(m)      \
        in_dword_masked(HWIO_TILT_CONTROL_ADDR, m)
#define HWIO_TILT_CONTROL_OUT(v)      \
        out_dword(HWIO_TILT_CONTROL_ADDR,v)
#define HWIO_TILT_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TILT_CONTROL_ADDR,m,v,HWIO_TILT_CONTROL_IN)
#define HWIO_TILT_CONTROL_TILT_ANGLE_BMSK                                            0x3ff000
#define HWIO_TILT_CONTROL_TILT_ANGLE_SHFT                                                 0xc
#define HWIO_TILT_CONTROL_TILT_INIT_WIN_LEN_SAMP_BMSK                                   0xfc0
#define HWIO_TILT_CONTROL_TILT_INIT_WIN_LEN_SAMP_SHFT                                     0x6
#define HWIO_TILT_CONTROL_TILT_WIN_LEN_SAMP_BMSK                                         0x3f
#define HWIO_TILT_CONTROL_TILT_WIN_LEN_SAMP_SHFT                                          0x0

#define HWIO_TILT_EVENT_TS_ADDR                                                    (CCD_REG_BASE      + 0x00004008)
#define HWIO_TILT_EVENT_TS_RMSK                                                    0xffffffff
#define HWIO_TILT_EVENT_TS_IN          \
        in_dword_masked(HWIO_TILT_EVENT_TS_ADDR, HWIO_TILT_EVENT_TS_RMSK)
#define HWIO_TILT_EVENT_TS_INM(m)      \
        in_dword_masked(HWIO_TILT_EVENT_TS_ADDR, m)
#define HWIO_TILT_EVENT_TS_TILT_EVENT_TS_BMSK                                      0xffffffff
#define HWIO_TILT_EVENT_TS_TILT_EVENT_TS_SHFT                                             0x0

#define HWIO_TILT_SPARE_ADDR                                                       (CCD_REG_BASE      + 0x0000400c)
#define HWIO_TILT_SPARE_RMSK                                                       0xffffffff
#define HWIO_TILT_SPARE_IN          \
        in_dword_masked(HWIO_TILT_SPARE_ADDR, HWIO_TILT_SPARE_RMSK)
#define HWIO_TILT_SPARE_INM(m)      \
        in_dword_masked(HWIO_TILT_SPARE_ADDR, m)
#define HWIO_TILT_SPARE_OUT(v)      \
        out_dword(HWIO_TILT_SPARE_ADDR,v)
#define HWIO_TILT_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TILT_SPARE_ADDR,m,v,HWIO_TILT_SPARE_IN)
#define HWIO_TILT_SPARE_TILT_SPARE_BMSK                                            0xffffffff
#define HWIO_TILT_SPARE_TILT_SPARE_SHFT                                                   0x0

#define HWIO_HW_VERSION_ADDR                                                       (CCD_REG_BASE      + 0x0000f000)
#define HWIO_HW_VERSION_RMSK                                                       0xffffffff
#define HWIO_HW_VERSION_IN          \
        in_dword_masked(HWIO_HW_VERSION_ADDR, HWIO_HW_VERSION_RMSK)
#define HWIO_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_HW_VERSION_ADDR, m)
#define HWIO_HW_VERSION_MAJOR_BMSK                                                 0xf0000000
#define HWIO_HW_VERSION_MAJOR_SHFT                                                       0x1c
#define HWIO_HW_VERSION_MINOR_BMSK                                                  0xfff0000
#define HWIO_HW_VERSION_MINOR_SHFT                                                       0x10
#define HWIO_HW_VERSION_STEP_BMSK                                                      0xffff
#define HWIO_HW_VERSION_STEP_SHFT                                                         0x0

#define HWIO_HW_CONTROL_1_ADDR                                                     (CCD_REG_BASE      + 0x0000f004)
#define HWIO_HW_CONTROL_1_RMSK                                                     0xffffffff
#define HWIO_HW_CONTROL_1_IN          \
        in_dword_masked(HWIO_HW_CONTROL_1_ADDR, HWIO_HW_CONTROL_1_RMSK)
#define HWIO_HW_CONTROL_1_INM(m)      \
        in_dword_masked(HWIO_HW_CONTROL_1_ADDR, m)
#define HWIO_HW_CONTROL_1_OUT(v)      \
        out_dword(HWIO_HW_CONTROL_1_ADDR,v)
#define HWIO_HW_CONTROL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_CONTROL_1_ADDR,m,v,HWIO_HW_CONTROL_1_IN)
#define HWIO_HW_CONTROL_1_HW_CONTROL_1_BMSK                                        0xffffffff
#define HWIO_HW_CONTROL_1_HW_CONTROL_1_SHFT                                               0x0

#define HWIO_HW_CONTROL_2_ADDR                                                     (CCD_REG_BASE      + 0x0000f008)
#define HWIO_HW_CONTROL_2_RMSK                                                     0xffffffff
#define HWIO_HW_CONTROL_2_IN          \
        in_dword_masked(HWIO_HW_CONTROL_2_ADDR, HWIO_HW_CONTROL_2_RMSK)
#define HWIO_HW_CONTROL_2_INM(m)      \
        in_dword_masked(HWIO_HW_CONTROL_2_ADDR, m)
#define HWIO_HW_CONTROL_2_OUT(v)      \
        out_dword(HWIO_HW_CONTROL_2_ADDR,v)
#define HWIO_HW_CONTROL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_CONTROL_2_ADDR,m,v,HWIO_HW_CONTROL_2_IN)
#define HWIO_HW_CONTROL_2_HW_CONTROL_2_BMSK                                        0xffffffff
#define HWIO_HW_CONTROL_2_HW_CONTROL_2_SHFT                                               0x0

#define HWIO_HW_CONTROL_3_ADDR                                                     (CCD_REG_BASE      + 0x0000f00c)
#define HWIO_HW_CONTROL_3_RMSK                                                     0xffffffff
#define HWIO_HW_CONTROL_3_IN          \
        in_dword_masked(HWIO_HW_CONTROL_3_ADDR, HWIO_HW_CONTROL_3_RMSK)
#define HWIO_HW_CONTROL_3_INM(m)      \
        in_dword_masked(HWIO_HW_CONTROL_3_ADDR, m)
#define HWIO_HW_CONTROL_3_OUT(v)      \
        out_dword(HWIO_HW_CONTROL_3_ADDR,v)
#define HWIO_HW_CONTROL_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_CONTROL_3_ADDR,m,v,HWIO_HW_CONTROL_3_IN)
#define HWIO_HW_CONTROL_3_HW_CONTROL_3_BMSK                                        0xffffffff
#define HWIO_HW_CONTROL_3_HW_CONTROL_3_SHFT                                               0x0

#define HWIO_HW_CONTROL_4_ADDR                                                     (CCD_REG_BASE      + 0x0000f010)
#define HWIO_HW_CONTROL_4_RMSK                                                     0xffffffff
#define HWIO_HW_CONTROL_4_IN          \
        in_dword_masked(HWIO_HW_CONTROL_4_ADDR, HWIO_HW_CONTROL_4_RMSK)
#define HWIO_HW_CONTROL_4_INM(m)      \
        in_dword_masked(HWIO_HW_CONTROL_4_ADDR, m)
#define HWIO_HW_CONTROL_4_OUT(v)      \
        out_dword(HWIO_HW_CONTROL_4_ADDR,v)
#define HWIO_HW_CONTROL_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_CONTROL_4_ADDR,m,v,HWIO_HW_CONTROL_4_IN)
#define HWIO_HW_CONTROL_4_HW_CONTROL_4_BMSK                                        0xffffffff
#define HWIO_HW_CONTROL_4_HW_CONTROL_4_SHFT                                               0x0

#define HWIO_HW_CONTROL_5_ADDR                                                     (CCD_REG_BASE      + 0x0000f014)
#define HWIO_HW_CONTROL_5_RMSK                                                     0xffffffff
#define HWIO_HW_CONTROL_5_IN          \
        in_dword_masked(HWIO_HW_CONTROL_5_ADDR, HWIO_HW_CONTROL_5_RMSK)
#define HWIO_HW_CONTROL_5_INM(m)      \
        in_dword_masked(HWIO_HW_CONTROL_5_ADDR, m)
#define HWIO_HW_CONTROL_5_OUT(v)      \
        out_dword(HWIO_HW_CONTROL_5_ADDR,v)
#define HWIO_HW_CONTROL_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_CONTROL_5_ADDR,m,v,HWIO_HW_CONTROL_5_IN)
#define HWIO_HW_CONTROL_5_HW_CONTROL_5_BMSK                                        0xffffffff
#define HWIO_HW_CONTROL_5_HW_CONTROL_5_SHFT                                               0x0

#define HWIO_HW_STATUS_1_ADDR                                                      (CCD_REG_BASE      + 0x0000f100)
#define HWIO_HW_STATUS_1_RMSK                                                      0xffffffff
#define HWIO_HW_STATUS_1_IN          \
        in_dword_masked(HWIO_HW_STATUS_1_ADDR, HWIO_HW_STATUS_1_RMSK)
#define HWIO_HW_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_HW_STATUS_1_ADDR, m)
#define HWIO_HW_STATUS_1_HW_STATUS_1_BMSK                                          0xffffffff
#define HWIO_HW_STATUS_1_HW_STATUS_1_SHFT                                                 0x0

#define HWIO_HW_SPARE_ADDR                                                         (CCD_REG_BASE      + 0x0000f104)
#define HWIO_HW_SPARE_RMSK                                                         0xffffffff
#define HWIO_HW_SPARE_IN          \
        in_dword_masked(HWIO_HW_SPARE_ADDR, HWIO_HW_SPARE_RMSK)
#define HWIO_HW_SPARE_INM(m)      \
        in_dword_masked(HWIO_HW_SPARE_ADDR, m)
#define HWIO_HW_SPARE_OUT(v)      \
        out_dword(HWIO_HW_SPARE_ADDR,v)
#define HWIO_HW_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HW_SPARE_ADDR,m,v,HWIO_HW_SPARE_IN)
#define HWIO_HW_SPARE_HW_SPARE_BMSK                                                0xffffffff
#define HWIO_HW_SPARE_HW_SPARE_SHFT                                                       0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_SCC_SCC_SCC_REG
 *--------------------------------------------------------------------------*/

#define SSC_SCC_SCC_SCC_REG_REG_BASE                                               (SSC_BASE      + 0x00b10000)

#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR                                     (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e000)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_RMSK                                            0x1
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR, HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_IN)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_BLK_ARES_BMSK                                   0x1
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_BLK_ARES_SHFT                                   0x0

#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e004)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_RMSK                                            0x1ff
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR, HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_CLK_DIV_BMSK                                    0x1ff
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_CLK_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_CCD_CBCR_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e008)
#define HWIO_SSC_SCC_CCD_CBCR_RMSK                                                 0x80000007
#define HWIO_SSC_SCC_CCD_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_CBCR_ADDR, HWIO_SSC_SCC_CCD_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_CBCR_IN)
#define HWIO_SSC_SCC_CCD_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_SSC_SCC_CCD_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ARES_BMSK                                               0x4
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ARES_SHFT                                               0x2
#define HWIO_SSC_SCC_CCD_CBCR_HW_CTL_BMSK                                                 0x2
#define HWIO_SSC_SCC_CCD_CBCR_HW_CTL_SHFT                                                 0x1
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e00c)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_RMSK                                            0x80000007
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR, HWIO_SSC_SCC_CCD_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_HW_CTL_BMSK                                            0x2
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_HW_CTL_SHFT                                            0x1
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e010)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_RMSK                            0x80000005
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR, HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_IN)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_OFF_BMSK                    0x80000000
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_OFF_SHFT                          0x1f
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ARES_BMSK                          0x4
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ARES_SHFT                          0x2
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ENABLE_BMSK                        0x1
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ENABLE_SHFT                        0x0


#endif /* __SNS_HWIO_CCD_PPE_H__ */
