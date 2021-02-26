#ifndef __HALQUPV3COMMONHWIO_H__
#define __HALQUPV3COMMONHWIO_H__
/*
===========================================================================
*/
/**
  @file HALqupv3CommonHwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r47.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    QUPV3_0_QUPV3_COMMON

  'Include' filters applied: 
  'Exclude' filters applied: 
*/
/*
  ===========================================================================

  Copyright (c) 2017-2020 Qualcomm Technologies Incorporated.
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


  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: QUPV3_QUPV3_COMMON
 *--------------------------------------------------------------------------*/

#define QUPV3_QUPV3_COMMON_REG_BASE                                                (QUPV3_QUPV3_ID_1_BASE      + 0x000c0000)
#define QUPV3_QUPV3_COMMON_REG_BASE_OFFS                                           0x000c0000

#define HWIO_QUPV3_QUPV3_HW_PARAM_ADDR(x)                                          ((x) + 0x00000000)
#define HWIO_QUPV3_QUPV3_HW_PARAM_OFFS                                             (0x00000000)
#define HWIO_QUPV3_QUPV3_HW_PARAM_RMSK                                             0xffffff01
#define HWIO_QUPV3_QUPV3_HW_PARAM_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_PARAM_ADDR(x), HWIO_QUPV3_QUPV3_HW_PARAM_RMSK)
#define HWIO_QUPV3_QUPV3_HW_PARAM_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_PARAM_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_AHB_M_ADDR_W_BMSK                          0xff000000
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_AHB_M_ADDR_W_SHFT                                0x18
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_NUM_GPIIS_BMSK                               0xff0000
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_NUM_GPIIS_SHFT                                   0x10
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_NUM_SES_BMSK                                   0xff00
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_NUM_SES_SHFT                                      0x8
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_WRAPPER_ID_BMSK                                   0x1
#define HWIO_QUPV3_QUPV3_HW_PARAM_QUPV3_WRAPPER_ID_SHFT                                   0x0

#define HWIO_QUPV3_QUPV3_HW_VERSION_ADDR(x)                                        ((x) + 0x00000004)
#define HWIO_QUPV3_QUPV3_HW_VERSION_OFFS                                           (0x00000004)
#define HWIO_QUPV3_QUPV3_HW_VERSION_RMSK                                           0xffffffff
#define HWIO_QUPV3_QUPV3_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_VERSION_ADDR(x), HWIO_QUPV3_QUPV3_HW_VERSION_RMSK)
#define HWIO_QUPV3_QUPV3_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_VERSION_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_HW_VERSION_MAJOR_BMSK                                     0xf0000000
#define HWIO_QUPV3_QUPV3_HW_VERSION_MAJOR_SHFT                                           0x1c
#define HWIO_QUPV3_QUPV3_HW_VERSION_MINOR_BMSK                                      0xfff0000
#define HWIO_QUPV3_QUPV3_HW_VERSION_MINOR_SHFT                                           0x10
#define HWIO_QUPV3_QUPV3_HW_VERSION_STEP_BMSK                                          0xffff
#define HWIO_QUPV3_QUPV3_HW_VERSION_STEP_SHFT                                             0x0

#define HWIO_QUPV3_QUPV3_STATUS_ADDR(x)                                            ((x) + 0x00000008)
#define HWIO_QUPV3_QUPV3_STATUS_OFFS                                               (0x00000008)
#define HWIO_QUPV3_QUPV3_STATUS_RMSK                                                      0x1
#define HWIO_QUPV3_QUPV3_STATUS_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_STATUS_ADDR(x), HWIO_QUPV3_QUPV3_STATUS_RMSK)
#define HWIO_QUPV3_QUPV3_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_STATUS_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_STATUS_GSI_BUSY_BMSK                                             0x1
#define HWIO_QUPV3_QUPV3_STATUS_GSI_BUSY_SHFT                                             0x0

#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ADDR(x)                                   ((x) + 0x00000100)
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_OFFS                                      (0x00000100)
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_RMSK                                             0x1
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                            0x1
#define HWIO_QUPV3_QUPV3_EXT_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                            0x0

#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ADDR(x)                                   ((x) + 0x00000104)
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_OFFS                                      (0x00000104)
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_RMSK                                             0x1
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                            0x1
#define HWIO_QUPV3_QUPV3_INT_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                            0x0

#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x)                             ((x) + 0x00000108)
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OFFS                                (0x00000108)
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_RMSK                                       0x7
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_UNLOCK_DEPENDS_HSEL_BMSK                   0x4
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_UNLOCK_DEPENDS_HSEL_SHFT                   0x2
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_USE_DYNAMIC_HSEL_BMSK                      0x2
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_USE_DYNAMIC_HSEL_SHFT                      0x1
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                      0x1
#define HWIO_QUPV3_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                      0x0

#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_ADDR(x)                                  ((x) + 0x00000110)
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_OFFS                                     (0x00000110)
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_RMSK                                     0xffffffff
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_ADDR(x), HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_RMSK)
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_SE_IF_IRQ_MASKED_BMSK                    0xffffffff
#define HWIO_QUPV3_QUPV3_SE_IF_IRQ_MASKED_SE_IF_IRQ_MASKED_SHFT                           0x0

#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_ADDR(x)                                   ((x) + 0x00000114)
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_OFFS                                      (0x00000114)
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_RMSK                                           0x1ff
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_ADDR(x), HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_LAST_BMSK                       0x1c0
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_LAST_SHFT                         0x6
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_MID_TRANS_BMSK                   0x38
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_MID_TRANS_SHFT                    0x3
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_READ_BMSK                               0x7
#define HWIO_QUPV3_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_READ_SHFT                               0x0

#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_ADDR(x)                                      ((x) + 0x00000118)
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_OFFS                                         (0x00000118)
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_RMSK                                                0x1
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_ADDR(x), HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK                               0x1
#define HWIO_QUPV3_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_SHFT                               0x0

#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_ADDR(x)                                  ((x) + 0x0000011c)
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_OFFS                                     (0x0000011c)
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_RMSK                                     0xffffffff
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_ADDR(x), HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_RMSK)
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_QTIMER_BIN_TRUNC_BMSK                    0xffffffff
#define HWIO_QUPV3_QUPV3_QTIMER_BIN_TRUNC_QTIMER_BIN_TRUNC_SHFT                           0x0

#define HWIO_QUPV3_QUPV3_COMMON_CFG_ADDR(x)                                        ((x) + 0x00000120)
#define HWIO_QUPV3_QUPV3_COMMON_CFG_OFFS                                           (0x00000120)
#define HWIO_QUPV3_QUPV3_COMMON_CFG_RMSK                                           0xffff0001
#define HWIO_QUPV3_QUPV3_COMMON_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_COMMON_CFG_ADDR(x), HWIO_QUPV3_QUPV3_COMMON_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_COMMON_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_COMMON_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_COMMON_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_COMMON_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_COMMON_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_COMMON_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_COMMON_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_COMMON_CFG_CFG_FUTURE_USE_BMSK                            0xffff0000
#define HWIO_QUPV3_QUPV3_COMMON_CFG_CFG_FUTURE_USE_SHFT                                  0x10
#define HWIO_QUPV3_QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_BMSK                      0x1
#define HWIO_QUPV3_QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_SHFT                      0x0

#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_ADDR(x)                                      ((x) + 0x00000200)
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_OFFS                                         (0x00000200)
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_RMSK                                            0x77f3f
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_SEL_ADDR(x), HWIO_QUPV3_QUPV3_TEST_BUS_SEL_RMSK)
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_SEL_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_TEST_BUS_SEL_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_TEST_BUS_SEL_ADDR(x),m,v,HWIO_QUPV3_QUPV3_TEST_BUS_SEL_IN(x))
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SE_WRAPPER_TEST_BUS_SEL_BMSK                    0x70000
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SE_WRAPPER_TEST_BUS_SEL_SHFT                       0x10
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SE_WRAPPER_MASTER_TEST_BUS_SEL_BMSK              0x7000
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SE_WRAPPER_MASTER_TEST_BUS_SEL_SHFT                 0xc
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SLAVE_BRIDGE_TEST_BUS_SEL_BMSK                    0xc00
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_SLAVE_BRIDGE_TEST_BUS_SEL_SHFT                      0xa
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_MASTER_BRIDGE_TEST_BUS_SEL_BMSK                   0x300
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_MASTER_BRIDGE_TEST_BUS_SEL_SHFT                     0x8
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_TOP_TEST_BUS_SEL_BMSK                              0x3f
#define HWIO_QUPV3_QUPV3_TEST_BUS_SEL_TOP_TEST_BUS_SEL_SHFT                               0x0

#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_ADDR(x)                                       ((x) + 0x00000204)
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_OFFS                                          (0x00000204)
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_RMSK                                               0x111
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_EN_ADDR(x), HWIO_QUPV3_QUPV3_TEST_BUS_EN_RMSK)
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_EN_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_TEST_BUS_EN_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_TEST_BUS_EN_ADDR(x),m,v,HWIO_QUPV3_QUPV3_TEST_BUS_EN_IN(x))
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_SW_TB_TOGGLE_BMSK                                  0x100
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_SW_TB_TOGGLE_SHFT                                    0x8
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_TEST_BUS_EN_BMSK                                    0x10
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_TEST_BUS_EN_SHFT                                     0x4
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_TEST_BUS_REG_EN_BMSK                                 0x1
#define HWIO_QUPV3_QUPV3_TEST_BUS_EN_TEST_BUS_REG_EN_SHFT                                 0x0

#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_ADDR(x)                                      ((x) + 0x00000208)
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_OFFS                                         (0x00000208)
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_RMSK                                         0xffffffff
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_REG_ADDR(x), HWIO_QUPV3_QUPV3_TEST_BUS_REG_RMSK)
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_TEST_BUS_REG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_TEST_BUS_VALUE_BMSK                          0xffffffff
#define HWIO_QUPV3_QUPV3_TEST_BUS_REG_TEST_BUS_VALUE_SHFT                                 0x0

#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x)                                 ((x) + 0x0000020c)
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_OFFS                                    (0x0000020c)
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_RMSK                                    0x80000700
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x), HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_RMSK)
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x), m)
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x),v)
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x),m,v,HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_IN(x))
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_EN_BMSK                       0x80000000
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_EN_SHFT                             0x1f
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_SEL_BMSK                           0x700
#define HWIO_QUPV3_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_SEL_SHFT                             0x8

#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_ADDR(base,n)                             ((base) + 0x00001000 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_OFFS(base,n)                             (0x00001000 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_RMSK                                     0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_MAXn                                              7
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_ADDR(base,n), HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_RMSK)
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_USER_DATA_TX_SE_BMSK                     0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_TX_SEn_USER_DATA_TX_SE_SHFT                            0x0

#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_ADDR(base,n)                             ((base) + 0x00001004 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_OFFS(base,n)                             (0x00001004 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_RMSK                                     0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_MAXn                                              7
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_ADDR(base,n), HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_RMSK)
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_USER_DATA_RX_SE_BMSK                     0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_RX_SEn_USER_DATA_RX_SE_SHFT                            0x0

#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n)                         ((base) + 0x00001008 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_OFFS(base,n)                         (0x00001008 + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_RMSK                                 0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_MAXn                                          7
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n), HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_RMSK)
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_USER_DATA_GENI_M_SE_BMSK             0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_M_SEn_USER_DATA_GENI_M_SE_SHFT                    0x0

#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n)                         ((base) + 0x0000100c + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_OFFS(base,n)                         (0x0000100c + 0x100 * (n))
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_RMSK                                 0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_MAXn                                          7
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n), HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_RMSK)
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_USER_DATA_GENI_S_SE_BMSK             0xffffffff
#define HWIO_QUPV3_QUPV3_USER_DATA_GENI_S_SEn_USER_DATA_GENI_S_SE_SHFT                    0x0


#endif /* __HALQUPV3COMMONHWIO_H__ */
