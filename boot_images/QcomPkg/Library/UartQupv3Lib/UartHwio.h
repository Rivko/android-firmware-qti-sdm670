#ifndef __UART_HWIO_H__
#define __UART_HWIO_H__
/*
===========================================================================
*/
/**
  @file UartHwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r30.0.0_F2_ECO]

  This file contains HWIO register definitions for the following modules:
    QUPV3_0_QUPV3_COMMON
    QUPV3_0_SE0_GENI4_CFG
    QUPV3_0_SE0_GENI4_DATA
    QUPV3_0_SE0_GENI4_IMAGE
    QUPV3_0_SE0_GENI4_IMAGE_REGS
    QUPV3_0_SE0_QUPV3_SE_DMA
    QUPV3_0_SE0_QUPV3_SEC

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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartHwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: QUPV3_COMMON
 *--------------------------------------------------------------------------*/

#define QUPV3_COMMON_REG_BASE                                                        (QUPV3_0_QUPV3_ID_1_BASE      + 0x000c0000)
#define QUPV3_COMMON_REG_BASE_OFFS                                                   0x000c0000

#define HWIO_QUPV3_HW_PARAM_ADDR(x)                                                  ((x) + 0x00000000)
#define HWIO_QUPV3_HW_PARAM_OFFS                                                     (0x00000000)
#define HWIO_QUPV3_HW_PARAM_RMSK                                                     0xffffff01
#define HWIO_QUPV3_HW_PARAM_IN(x)      \
        in_dword_masked(HWIO_QUPV3_HW_PARAM_ADDR(x), HWIO_QUPV3_HW_PARAM_RMSK)
#define HWIO_QUPV3_HW_PARAM_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_HW_PARAM_ADDR(x), m)
#define HWIO_QUPV3_HW_PARAM_QUPV3_AHB_M_ADDR_W_BMSK                                  0xff000000
#define HWIO_QUPV3_HW_PARAM_QUPV3_AHB_M_ADDR_W_SHFT                                        0x18
#define HWIO_QUPV3_HW_PARAM_QUPV3_NUM_GPIIS_BMSK                                       0xff0000
#define HWIO_QUPV3_HW_PARAM_QUPV3_NUM_GPIIS_SHFT                                           0x10
#define HWIO_QUPV3_HW_PARAM_QUPV3_NUM_SES_BMSK                                           0xff00
#define HWIO_QUPV3_HW_PARAM_QUPV3_NUM_SES_SHFT                                              0x8
#define HWIO_QUPV3_HW_PARAM_QUPV3_WRAPPER_ID_BMSK                                           0x1
#define HWIO_QUPV3_HW_PARAM_QUPV3_WRAPPER_ID_SHFT                                           0x0

#define HWIO_QUPV3_HW_VERSION_ADDR(x)                                                ((x) + 0x00000004)
#define HWIO_QUPV3_HW_VERSION_OFFS                                                   (0x00000004)
#define HWIO_QUPV3_HW_VERSION_RMSK                                                   0xffffffff
#define HWIO_QUPV3_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_QUPV3_HW_VERSION_ADDR(x), HWIO_QUPV3_HW_VERSION_RMSK)
#define HWIO_QUPV3_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_HW_VERSION_ADDR(x), m)
#define HWIO_QUPV3_HW_VERSION_MAJOR_BMSK                                             0xf0000000
#define HWIO_QUPV3_HW_VERSION_MAJOR_SHFT                                                   0x1c
#define HWIO_QUPV3_HW_VERSION_MINOR_BMSK                                              0xfff0000
#define HWIO_QUPV3_HW_VERSION_MINOR_SHFT                                                   0x10
#define HWIO_QUPV3_HW_VERSION_STEP_BMSK                                                  0xffff
#define HWIO_QUPV3_HW_VERSION_STEP_SHFT                                                     0x0

#define HWIO_QUPV3_STATUS_ADDR(x)                                                    ((x) + 0x00000008)
#define HWIO_QUPV3_STATUS_OFFS                                                       (0x00000008)
#define HWIO_QUPV3_STATUS_RMSK                                                              0x1
#define HWIO_QUPV3_STATUS_IN(x)      \
        in_dword_masked(HWIO_QUPV3_STATUS_ADDR(x), HWIO_QUPV3_STATUS_RMSK)
#define HWIO_QUPV3_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_STATUS_ADDR(x), m)
#define HWIO_QUPV3_STATUS_GSI_BUSY_BMSK                                                     0x1
#define HWIO_QUPV3_STATUS_GSI_BUSY_SHFT                                                     0x0

#define HWIO_QUPV3_EXT_AHB_ARB_CFG_ADDR(x)                                           ((x) + 0x00000100)
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_OFFS                                              (0x00000100)
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_RMSK                                                     0x1
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_EXT_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_EXT_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_EXT_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_EXT_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_EXT_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_EXT_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                                    0x1
#define HWIO_QUPV3_EXT_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                                    0x0

#define HWIO_QUPV3_INT_AHB_ARB_CFG_ADDR(x)                                           ((x) + 0x00000104)
#define HWIO_QUPV3_INT_AHB_ARB_CFG_OFFS                                              (0x00000104)
#define HWIO_QUPV3_INT_AHB_ARB_CFG_RMSK                                                     0x1
#define HWIO_QUPV3_INT_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_INT_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_INT_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_INT_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_INT_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_INT_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_INT_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_INT_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_INT_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_INT_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_INT_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                                    0x1
#define HWIO_QUPV3_INT_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                                    0x0

#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x)                                     ((x) + 0x00000108)
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OFFS                                        (0x00000108)
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_RMSK                                               0x7
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x), HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_RMSK)
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x), m)
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x),v)
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ADDR(x),m,v,HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_IN(x))
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_UNLOCK_DEPENDS_HSEL_BMSK                           0x4
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_UNLOCK_DEPENDS_HSEL_SHFT                           0x2
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_USE_DYNAMIC_HSEL_BMSK                              0x2
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_USE_DYNAMIC_HSEL_SHFT                              0x1
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ARBITRATION_TYPE_BMSK                              0x1
#define HWIO_QUPV3_GSI_SLAVE_AHB_ARB_CFG_ARBITRATION_TYPE_SHFT                              0x0

#define HWIO_QUPV3_SE_IF_IRQ_MASKED_ADDR(x)                                          ((x) + 0x00000110)
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_OFFS                                             (0x00000110)
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_RMSK                                             0xffffffff
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_IN(x)      \
        in_dword_masked(HWIO_QUPV3_SE_IF_IRQ_MASKED_ADDR(x), HWIO_QUPV3_SE_IF_IRQ_MASKED_RMSK)
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_SE_IF_IRQ_MASKED_ADDR(x), m)
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_SE_IF_IRQ_MASKED_BMSK                            0xffffffff
#define HWIO_QUPV3_SE_IF_IRQ_MASKED_SE_IF_IRQ_MASKED_SHFT                                   0x0

#define HWIO_QUPV3_SE_HMEMTYPE_CFG_ADDR(x)                                           ((x) + 0x00000114)
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_OFFS                                              (0x00000114)
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_RMSK                                                   0x1ff
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_SE_HMEMTYPE_CFG_ADDR(x), HWIO_QUPV3_SE_HMEMTYPE_CFG_RMSK)
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_SE_HMEMTYPE_CFG_ADDR(x), m)
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_SE_HMEMTYPE_CFG_ADDR(x),v)
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_SE_HMEMTYPE_CFG_ADDR(x),m,v,HWIO_QUPV3_SE_HMEMTYPE_CFG_IN(x))
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_LAST_BMSK                               0x1c0
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_LAST_SHFT                                 0x6
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_MID_TRANS_BMSK                           0x38
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_WRITE_MID_TRANS_SHFT                            0x3
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_READ_BMSK                                       0x7
#define HWIO_QUPV3_SE_HMEMTYPE_CFG_HMEMTYPE_READ_SHFT                                       0x0

#define HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x)                                              ((x) + 0x00000118)
#define HWIO_QUPV3_SE_AHB_M_CFG_OFFS                                                 (0x00000118)
#define HWIO_QUPV3_SE_AHB_M_CFG_RMSK                                                        0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x), HWIO_QUPV3_SE_AHB_M_CFG_RMSK)
#define HWIO_QUPV3_SE_AHB_M_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x), m)
#define HWIO_QUPV3_SE_AHB_M_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x),v)
#define HWIO_QUPV3_SE_AHB_M_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_SE_AHB_M_CFG_ADDR(x),m,v,HWIO_QUPV3_SE_AHB_M_CFG_IN(x))
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK                                       0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_SHFT                                       0x0

#define HWIO_QUPV3_QTIMER_BIN_TRUNC_ADDR(x)                                          ((x) + 0x0000011c)
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_OFFS                                             (0x0000011c)
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_RMSK                                             0xffffffff
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_IN(x)      \
        in_dword_masked(HWIO_QUPV3_QTIMER_BIN_TRUNC_ADDR(x), HWIO_QUPV3_QTIMER_BIN_TRUNC_RMSK)
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_QTIMER_BIN_TRUNC_ADDR(x), m)
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_QTIMER_BIN_TRUNC_BMSK                            0xffffffff
#define HWIO_QUPV3_QTIMER_BIN_TRUNC_QTIMER_BIN_TRUNC_SHFT                                   0x0

#define HWIO_QUPV3_COMMON_CFG_ADDR(x)                                                ((x) + 0x00000120)
#define HWIO_QUPV3_COMMON_CFG_OFFS                                                   (0x00000120)
#define HWIO_QUPV3_COMMON_CFG_RMSK                                                   0xffff0001
#define HWIO_QUPV3_COMMON_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_COMMON_CFG_ADDR(x), HWIO_QUPV3_COMMON_CFG_RMSK)
#define HWIO_QUPV3_COMMON_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_COMMON_CFG_ADDR(x), m)
#define HWIO_QUPV3_COMMON_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_COMMON_CFG_ADDR(x),v)
#define HWIO_QUPV3_COMMON_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_COMMON_CFG_ADDR(x),m,v,HWIO_QUPV3_COMMON_CFG_IN(x))
#define HWIO_QUPV3_COMMON_CFG_CFG_FUTURE_USE_BMSK                                    0xffff0000
#define HWIO_QUPV3_COMMON_CFG_CFG_FUTURE_USE_SHFT                                          0x10
#define HWIO_QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_BMSK                              0x1
#define HWIO_QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_SHFT                              0x0

#define HWIO_QUPV3_TEST_BUS_SEL_ADDR(x)                                              ((x) + 0x00000200)
#define HWIO_QUPV3_TEST_BUS_SEL_OFFS                                                 (0x00000200)
#define HWIO_QUPV3_TEST_BUS_SEL_RMSK                                                    0x77f3f
#define HWIO_QUPV3_TEST_BUS_SEL_IN(x)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_SEL_ADDR(x), HWIO_QUPV3_TEST_BUS_SEL_RMSK)
#define HWIO_QUPV3_TEST_BUS_SEL_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_SEL_ADDR(x), m)
#define HWIO_QUPV3_TEST_BUS_SEL_OUT(x, v)      \
        out_dword(HWIO_QUPV3_TEST_BUS_SEL_ADDR(x),v)
#define HWIO_QUPV3_TEST_BUS_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_TEST_BUS_SEL_ADDR(x),m,v,HWIO_QUPV3_TEST_BUS_SEL_IN(x))
#define HWIO_QUPV3_TEST_BUS_SEL_SE_WRAPPER_TEST_BUS_SEL_BMSK                            0x70000
#define HWIO_QUPV3_TEST_BUS_SEL_SE_WRAPPER_TEST_BUS_SEL_SHFT                               0x10
#define HWIO_QUPV3_TEST_BUS_SEL_SE_WRAPPER_MASTER_TEST_BUS_SEL_BMSK                      0x7000
#define HWIO_QUPV3_TEST_BUS_SEL_SE_WRAPPER_MASTER_TEST_BUS_SEL_SHFT                         0xc
#define HWIO_QUPV3_TEST_BUS_SEL_SLAVE_BRIDGE_TEST_BUS_SEL_BMSK                            0xc00
#define HWIO_QUPV3_TEST_BUS_SEL_SLAVE_BRIDGE_TEST_BUS_SEL_SHFT                              0xa
#define HWIO_QUPV3_TEST_BUS_SEL_MASTER_BRIDGE_TEST_BUS_SEL_BMSK                           0x300
#define HWIO_QUPV3_TEST_BUS_SEL_MASTER_BRIDGE_TEST_BUS_SEL_SHFT                             0x8
#define HWIO_QUPV3_TEST_BUS_SEL_TOP_TEST_BUS_SEL_BMSK                                      0x3f
#define HWIO_QUPV3_TEST_BUS_SEL_TOP_TEST_BUS_SEL_SHFT                                       0x0

#define HWIO_QUPV3_TEST_BUS_EN_ADDR(x)                                               ((x) + 0x00000204)
#define HWIO_QUPV3_TEST_BUS_EN_OFFS                                                  (0x00000204)
#define HWIO_QUPV3_TEST_BUS_EN_RMSK                                                       0x111
#define HWIO_QUPV3_TEST_BUS_EN_IN(x)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_EN_ADDR(x), HWIO_QUPV3_TEST_BUS_EN_RMSK)
#define HWIO_QUPV3_TEST_BUS_EN_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_EN_ADDR(x), m)
#define HWIO_QUPV3_TEST_BUS_EN_OUT(x, v)      \
        out_dword(HWIO_QUPV3_TEST_BUS_EN_ADDR(x),v)
#define HWIO_QUPV3_TEST_BUS_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_TEST_BUS_EN_ADDR(x),m,v,HWIO_QUPV3_TEST_BUS_EN_IN(x))
#define HWIO_QUPV3_TEST_BUS_EN_SW_TB_TOGGLE_BMSK                                          0x100
#define HWIO_QUPV3_TEST_BUS_EN_SW_TB_TOGGLE_SHFT                                            0x8
#define HWIO_QUPV3_TEST_BUS_EN_TEST_BUS_EN_BMSK                                            0x10
#define HWIO_QUPV3_TEST_BUS_EN_TEST_BUS_EN_SHFT                                             0x4
#define HWIO_QUPV3_TEST_BUS_EN_TEST_BUS_REG_EN_BMSK                                         0x1
#define HWIO_QUPV3_TEST_BUS_EN_TEST_BUS_REG_EN_SHFT                                         0x0

#define HWIO_QUPV3_TEST_BUS_REG_ADDR(x)                                              ((x) + 0x00000208)
#define HWIO_QUPV3_TEST_BUS_REG_OFFS                                                 (0x00000208)
#define HWIO_QUPV3_TEST_BUS_REG_RMSK                                                 0xffffffff
#define HWIO_QUPV3_TEST_BUS_REG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_REG_ADDR(x), HWIO_QUPV3_TEST_BUS_REG_RMSK)
#define HWIO_QUPV3_TEST_BUS_REG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_TEST_BUS_REG_ADDR(x), m)
#define HWIO_QUPV3_TEST_BUS_REG_TEST_BUS_VALUE_BMSK                                  0xffffffff
#define HWIO_QUPV3_TEST_BUS_REG_TEST_BUS_VALUE_SHFT                                         0x0

#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x)                                         ((x) + 0x0000020c)
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_OFFS                                            (0x0000020c)
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_RMSK                                            0x80000700
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_IN(x)      \
        in_dword_masked(HWIO_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x), HWIO_QUPV3_HW_EVENTS_MUX_CFG_RMSK)
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_INM(x, m)      \
        in_dword_masked(HWIO_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x), m)
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_OUT(x, v)      \
        out_dword(HWIO_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x),v)
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUPV3_HW_EVENTS_MUX_CFG_ADDR(x),m,v,HWIO_QUPV3_HW_EVENTS_MUX_CFG_IN(x))
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_EN_BMSK                               0x80000000
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_EN_SHFT                                     0x1f
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_SEL_BMSK                                   0x700
#define HWIO_QUPV3_HW_EVENTS_MUX_CFG_HW_EVENTS_SEL_SHFT                                     0x8

#define HWIO_QUPV3_USER_DATA_TX_SEn_ADDR(base,n)                                     ((base) + 0x00001000 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_TX_SEn_OFFS(base,n)                                     (0x00001000 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_TX_SEn_RMSK                                             0xffffffff
#define HWIO_QUPV3_USER_DATA_TX_SEn_MAXn                                                      7
#define HWIO_QUPV3_USER_DATA_TX_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_USER_DATA_TX_SEn_ADDR(base,n), HWIO_QUPV3_USER_DATA_TX_SEn_RMSK)
#define HWIO_QUPV3_USER_DATA_TX_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_USER_DATA_TX_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_USER_DATA_TX_SEn_USER_DATA_TX_SE_BMSK                             0xffffffff
#define HWIO_QUPV3_USER_DATA_TX_SEn_USER_DATA_TX_SE_SHFT                                    0x0

#define HWIO_QUPV3_USER_DATA_RX_SEn_ADDR(base,n)                                     ((base) + 0x00001004 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_RX_SEn_OFFS(base,n)                                     (0x00001004 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_RX_SEn_RMSK                                             0xffffffff
#define HWIO_QUPV3_USER_DATA_RX_SEn_MAXn                                                      7
#define HWIO_QUPV3_USER_DATA_RX_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_USER_DATA_RX_SEn_ADDR(base,n), HWIO_QUPV3_USER_DATA_RX_SEn_RMSK)
#define HWIO_QUPV3_USER_DATA_RX_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_USER_DATA_RX_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_USER_DATA_RX_SEn_USER_DATA_RX_SE_BMSK                             0xffffffff
#define HWIO_QUPV3_USER_DATA_RX_SEn_USER_DATA_RX_SE_SHFT                                    0x0

#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n)                                 ((base) + 0x00001008 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_OFFS(base,n)                                 (0x00001008 + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_RMSK                                         0xffffffff
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_MAXn                                                  7
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n), HWIO_QUPV3_USER_DATA_GENI_M_SEn_RMSK)
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_USER_DATA_GENI_M_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_USER_DATA_GENI_M_SE_BMSK                     0xffffffff
#define HWIO_QUPV3_USER_DATA_GENI_M_SEn_USER_DATA_GENI_M_SE_SHFT                            0x0

#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n)                                 ((base) + 0x0000100c + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_OFFS(base,n)                                 (0x0000100c + 0x100 * (n))
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_RMSK                                         0xffffffff
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_MAXn                                                  7
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_INI(base,n)        \
        in_dword_masked(HWIO_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n), HWIO_QUPV3_USER_DATA_GENI_S_SEn_RMSK)
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QUPV3_USER_DATA_GENI_S_SEn_ADDR(base,n), mask)
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_USER_DATA_GENI_S_SE_BMSK                     0xffffffff
#define HWIO_QUPV3_USER_DATA_GENI_S_SEn_USER_DATA_GENI_S_SE_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: GENI4_CFG
 *--------------------------------------------------------------------------*/

#define GENI4_CFG_REG_BASE                                                                  (QUPV3_0_QUPV3_ID_1_BASE      + 0x00080000)
#define GENI4_CFG_REG_BASE_OFFS                                                             0x00080000

#define HWIO_GENI_INIT_CFG_REVISION_ADDR(x)                                                 ((x) + 0x00000000)
#define HWIO_GENI_INIT_CFG_REVISION_OFFS                                                    (0x00000000)
#define HWIO_GENI_INIT_CFG_REVISION_RMSK                                                          0xff
#define HWIO_GENI_INIT_CFG_REVISION_IN(x)      \
        in_dword_masked(HWIO_GENI_INIT_CFG_REVISION_ADDR(x), HWIO_GENI_INIT_CFG_REVISION_RMSK)
#define HWIO_GENI_INIT_CFG_REVISION_INM(x, m)      \
        in_dword_masked(HWIO_GENI_INIT_CFG_REVISION_ADDR(x), m)
#define HWIO_GENI_INIT_CFG_REVISION_OUT(x, v)      \
        out_dword(HWIO_GENI_INIT_CFG_REVISION_ADDR(x),v)
#define HWIO_GENI_INIT_CFG_REVISION_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_INIT_CFG_REVISION_ADDR(x),m,v,HWIO_GENI_INIT_CFG_REVISION_IN(x))
#define HWIO_GENI_INIT_CFG_REVISION_VERSION_BMSK                                                  0xff
#define HWIO_GENI_INIT_CFG_REVISION_VERSION_SHFT                                                   0x0

#define HWIO_GENI_S_INIT_CFG_REVISION_ADDR(x)                                               ((x) + 0x00000004)
#define HWIO_GENI_S_INIT_CFG_REVISION_OFFS                                                  (0x00000004)
#define HWIO_GENI_S_INIT_CFG_REVISION_RMSK                                                        0xff
#define HWIO_GENI_S_INIT_CFG_REVISION_IN(x)      \
        in_dword_masked(HWIO_GENI_S_INIT_CFG_REVISION_ADDR(x), HWIO_GENI_S_INIT_CFG_REVISION_RMSK)
#define HWIO_GENI_S_INIT_CFG_REVISION_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_INIT_CFG_REVISION_ADDR(x), m)
#define HWIO_GENI_S_INIT_CFG_REVISION_OUT(x, v)      \
        out_dword(HWIO_GENI_S_INIT_CFG_REVISION_ADDR(x),v)
#define HWIO_GENI_S_INIT_CFG_REVISION_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_INIT_CFG_REVISION_ADDR(x),m,v,HWIO_GENI_S_INIT_CFG_REVISION_IN(x))
#define HWIO_GENI_S_INIT_CFG_REVISION_VERSION_BMSK                                                0xff
#define HWIO_GENI_S_INIT_CFG_REVISION_VERSION_SHFT                                                 0x0

#define HWIO_GENI_GENERAL_CFG_ADDR(x)                                                       ((x) + 0x00000010)
#define HWIO_GENI_GENERAL_CFG_OFFS                                                          (0x00000010)
#define HWIO_GENI_GENERAL_CFG_RMSK                                                                0x3f
#define HWIO_GENI_GENERAL_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_GENERAL_CFG_ADDR(x), HWIO_GENI_GENERAL_CFG_RMSK)
#define HWIO_GENI_GENERAL_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_GENERAL_CFG_ADDR(x), m)
#define HWIO_GENI_GENERAL_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_GENERAL_CFG_ADDR(x),v)
#define HWIO_GENI_GENERAL_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_GENERAL_CFG_ADDR(x),m,v,HWIO_GENI_GENERAL_CFG_IN(x))
#define HWIO_GENI_GENERAL_CFG_TX_CRC_MSB_LSB_BMSK                                                 0x20
#define HWIO_GENI_GENERAL_CFG_TX_CRC_MSB_LSB_SHFT                                                  0x5
#define HWIO_GENI_GENERAL_CFG_CHAR_HUNT_SR_MSB_LSB_BMSK                                           0x10
#define HWIO_GENI_GENERAL_CFG_CHAR_HUNT_SR_MSB_LSB_SHFT                                            0x4
#define HWIO_GENI_GENERAL_CFG_AUTO_CANCEL_RX_STATUS_SET_BMSK                                       0x8
#define HWIO_GENI_GENERAL_CFG_AUTO_CANCEL_RX_STATUS_SET_SHFT                                       0x3
#define HWIO_GENI_GENERAL_CFG_CLEAR_IRQS_CMD_START_BMSK                                            0x4
#define HWIO_GENI_GENERAL_CFG_CLEAR_IRQS_CMD_START_SHFT                                            0x2
#define HWIO_GENI_GENERAL_CFG_SW_IRQ_IO2_RX_DATA_BMSK                                              0x2
#define HWIO_GENI_GENERAL_CFG_SW_IRQ_IO2_RX_DATA_SHFT                                              0x1
#define HWIO_GENI_GENERAL_CFG_SW_RX_FIFO_FLUSH_NO_FW_BMSK                                          0x1
#define HWIO_GENI_GENERAL_CFG_SW_RX_FIFO_FLUSH_NO_FW_SHFT                                          0x0

#define HWIO_GENI_RX_FIFO_CTRL_ADDR(x)                                                      ((x) + 0x00000014)
#define HWIO_GENI_RX_FIFO_CTRL_OFFS                                                         (0x00000014)
#define HWIO_GENI_RX_FIFO_CTRL_RMSK                                                                0x1
#define HWIO_GENI_RX_FIFO_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_FIFO_CTRL_ADDR(x),v)
#define HWIO_GENI_RX_FIFO_CTRL_SW_RX_FIFO_FLUSH_BMSK                                               0x1
#define HWIO_GENI_RX_FIFO_CTRL_SW_RX_FIFO_FLUSH_SHFT                                               0x0

#define HWIO_GENI_FORCE_DEFAULT_REG_ADDR(x)                                                 ((x) + 0x00000020)
#define HWIO_GENI_FORCE_DEFAULT_REG_OFFS                                                    (0x00000020)
#define HWIO_GENI_FORCE_DEFAULT_REG_RMSK                                                           0x1
#define HWIO_GENI_FORCE_DEFAULT_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_FORCE_DEFAULT_REG_ADDR(x),v)
#define HWIO_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK                                             0x1
#define HWIO_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_SHFT                                             0x0

#define HWIO_GENI_OUTPUT_CTRL_ADDR(x)                                                       ((x) + 0x00000024)
#define HWIO_GENI_OUTPUT_CTRL_OFFS                                                          (0x00000024)
#define HWIO_GENI_OUTPUT_CTRL_RMSK                                                                0x7f
#define HWIO_GENI_OUTPUT_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_OUTPUT_CTRL_ADDR(x), HWIO_GENI_OUTPUT_CTRL_RMSK)
#define HWIO_GENI_OUTPUT_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_OUTPUT_CTRL_ADDR(x), m)
#define HWIO_GENI_OUTPUT_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_OUTPUT_CTRL_ADDR(x),v)
#define HWIO_GENI_OUTPUT_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_OUTPUT_CTRL_ADDR(x),m,v,HWIO_GENI_OUTPUT_CTRL_IN(x))
#define HWIO_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_BMSK                                                 0x7f
#define HWIO_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_SHFT                                                  0x0

#define HWIO_GENI_CGC_CTRL_ADDR(x)                                                          ((x) + 0x00000028)
#define HWIO_GENI_CGC_CTRL_OFFS                                                             (0x00000028)
#define HWIO_GENI_CGC_CTRL_RMSK                                                                  0x37f
#define HWIO_GENI_CGC_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_CGC_CTRL_ADDR(x), HWIO_GENI_CGC_CTRL_RMSK)
#define HWIO_GENI_CGC_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CGC_CTRL_ADDR(x), m)
#define HWIO_GENI_CGC_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_CGC_CTRL_ADDR(x),v)
#define HWIO_GENI_CGC_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CGC_CTRL_ADDR(x),m,v,HWIO_GENI_CGC_CTRL_IN(x))
#define HWIO_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK                                                0x200
#define HWIO_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_SHFT                                                  0x9
#define HWIO_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK                                                0x100
#define HWIO_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_SHFT                                                  0x8
#define HWIO_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK                                                    0x40
#define HWIO_GENI_CGC_CTRL_EXT_CLK_CGC_ON_SHFT                                                     0x6
#define HWIO_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK                                                     0x20
#define HWIO_GENI_CGC_CTRL_RX_CLK_CGC_ON_SHFT                                                      0x5
#define HWIO_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK                                                     0x10
#define HWIO_GENI_CGC_CTRL_TX_CLK_CGC_ON_SHFT                                                      0x4
#define HWIO_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK                                                        0x8
#define HWIO_GENI_CGC_CTRL_SCLK_CGC_ON_SHFT                                                        0x3
#define HWIO_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK                                                0x4
#define HWIO_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_SHFT                                                0x2
#define HWIO_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK                                              0x2
#define HWIO_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_SHFT                                              0x1
#define HWIO_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK                                                 0x1
#define HWIO_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_SHFT                                                 0x0

#define HWIO_GENI_CHAR_CFG_ADDR(x)                                                          ((x) + 0x0000002c)
#define HWIO_GENI_CHAR_CFG_OFFS                                                             (0x0000002c)
#define HWIO_GENI_CHAR_CFG_RMSK                                                                0xfff31
#define HWIO_GENI_CHAR_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_CHAR_CFG_ADDR(x), HWIO_GENI_CHAR_CFG_RMSK)
#define HWIO_GENI_CHAR_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CHAR_CFG_ADDR(x), m)
#define HWIO_GENI_CHAR_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_CHAR_CFG_ADDR(x),v)
#define HWIO_GENI_CHAR_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CHAR_CFG_ADDR(x),m,v,HWIO_GENI_CHAR_CFG_IN(x))
#define HWIO_GENI_CHAR_CFG_CHAR_MODE_BMSK                                                      0xff000
#define HWIO_GENI_CHAR_CFG_CHAR_MODE_SHFT                                                          0xc
#define HWIO_GENI_CHAR_CFG_CHAR_STATUS_BMSK                                                      0xf00
#define HWIO_GENI_CHAR_CFG_CHAR_STATUS_SHFT                                                        0x8
#define HWIO_GENI_CHAR_CFG_ENGINE_SEL_BMSK                                                        0x30
#define HWIO_GENI_CHAR_CFG_ENGINE_SEL_SHFT                                                         0x4
#define HWIO_GENI_CHAR_CFG_ENABLE_BMSK                                                             0x1
#define HWIO_GENI_CHAR_CFG_ENABLE_SHFT                                                             0x0

#define HWIO_GENI_CHAR_DATA_n_ADDR(base,n)                                                  ((base) + 0x00000030 + 0x4 * (n))
#define HWIO_GENI_CHAR_DATA_n_OFFS(base,n)                                                  (0x00000030 + 0x4 * (n))
#define HWIO_GENI_CHAR_DATA_n_RMSK                                                              0xffff
#define HWIO_GENI_CHAR_DATA_n_MAXn                                                                   1
#define HWIO_GENI_CHAR_DATA_n_INI(base,n)        \
        in_dword_masked(HWIO_GENI_CHAR_DATA_n_ADDR(base,n), HWIO_GENI_CHAR_DATA_n_RMSK)
#define HWIO_GENI_CHAR_DATA_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_GENI_CHAR_DATA_n_ADDR(base,n), mask)
#define HWIO_GENI_CHAR_DATA_n_OUTI(base,n,val)    \
        out_dword(HWIO_GENI_CHAR_DATA_n_ADDR(base,n),val)
#define HWIO_GENI_CHAR_DATA_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_GENI_CHAR_DATA_n_ADDR(base,n),mask,val,HWIO_GENI_CHAR_DATA_n_INI(base,n))
#define HWIO_GENI_CHAR_DATA_n_DIN_ACTUAL_BMSK                                                   0xff00
#define HWIO_GENI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                      0x8
#define HWIO_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                              0xff
#define HWIO_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                               0x0

#define HWIO_GENI_STATUS_ADDR(x)                                                            ((x) + 0x00000040)
#define HWIO_GENI_STATUS_OFFS                                                               (0x00000040)
#define HWIO_GENI_STATUS_RMSK                                                                 0x1fffff
#define HWIO_GENI_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_STATUS_ADDR(x), HWIO_GENI_STATUS_RMSK)
#define HWIO_GENI_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_STATUS_ADDR(x), m)
#define HWIO_GENI_STATUS_S_GENI_CMD_FSM_STATE_BMSK                                            0x1f0000
#define HWIO_GENI_STATUS_S_GENI_CMD_FSM_STATE_SHFT                                                0x10
#define HWIO_GENI_STATUS_NOT_USED_BITS_3_BMSK                                                   0xe000
#define HWIO_GENI_STATUS_NOT_USED_BITS_3_SHFT                                                      0xd
#define HWIO_GENI_STATUS_S_GENI_CMD_ACTIVE_BMSK                                                 0x1000
#define HWIO_GENI_STATUS_S_GENI_CMD_ACTIVE_SHFT                                                    0xc
#define HWIO_GENI_STATUS_NOT_USED_BITS_2_BMSK                                                    0xe00
#define HWIO_GENI_STATUS_NOT_USED_BITS_2_SHFT                                                      0x9
#define HWIO_GENI_STATUS_M_GENI_CMD_FSM_STATE_BMSK                                               0x1f0
#define HWIO_GENI_STATUS_M_GENI_CMD_FSM_STATE_SHFT                                                 0x4
#define HWIO_GENI_STATUS_NOT_USED_BITS_1_BMSK                                                      0xe
#define HWIO_GENI_STATUS_NOT_USED_BITS_1_SHFT                                                      0x1
#define HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK                                                    0x1
#define HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_SHFT                                                    0x0

#define HWIO_GENI_TEST_BUS_CTRL_ADDR(x)                                                     ((x) + 0x00000044)
#define HWIO_GENI_TEST_BUS_CTRL_OFFS                                                        (0x00000044)
#define HWIO_GENI_TEST_BUS_CTRL_RMSK                                                              0x3f
#define HWIO_GENI_TEST_BUS_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_TEST_BUS_CTRL_ADDR(x), HWIO_GENI_TEST_BUS_CTRL_RMSK)
#define HWIO_GENI_TEST_BUS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TEST_BUS_CTRL_ADDR(x), m)
#define HWIO_GENI_TEST_BUS_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_TEST_BUS_CTRL_ADDR(x),v)
#define HWIO_GENI_TEST_BUS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TEST_BUS_CTRL_ADDR(x),m,v,HWIO_GENI_TEST_BUS_CTRL_IN(x))
#define HWIO_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                                                 0x3f
#define HWIO_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                                                  0x0

#define HWIO_GENI_SER_M_CLK_CFG_ADDR(x)                                                     ((x) + 0x00000048)
#define HWIO_GENI_SER_M_CLK_CFG_OFFS                                                        (0x00000048)
#define HWIO_GENI_SER_M_CLK_CFG_RMSK                                                            0xfff1
#define HWIO_GENI_SER_M_CLK_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_SER_M_CLK_CFG_ADDR(x), HWIO_GENI_SER_M_CLK_CFG_RMSK)
#define HWIO_GENI_SER_M_CLK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_SER_M_CLK_CFG_ADDR(x), m)
#define HWIO_GENI_SER_M_CLK_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_SER_M_CLK_CFG_ADDR(x),v)
#define HWIO_GENI_SER_M_CLK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_SER_M_CLK_CFG_ADDR(x),m,v,HWIO_GENI_SER_M_CLK_CFG_IN(x))
#define HWIO_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_BMSK                                              0xfff0
#define HWIO_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_SHFT                                                 0x4
#define HWIO_GENI_SER_M_CLK_CFG_SER_CLK_EN_BMSK                                                    0x1
#define HWIO_GENI_SER_M_CLK_CFG_SER_CLK_EN_SHFT                                                    0x0

#define HWIO_GENI_SER_S_CLK_CFG_ADDR(x)                                                     ((x) + 0x0000004c)
#define HWIO_GENI_SER_S_CLK_CFG_OFFS                                                        (0x0000004c)
#define HWIO_GENI_SER_S_CLK_CFG_RMSK                                                            0xfff1
#define HWIO_GENI_SER_S_CLK_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_SER_S_CLK_CFG_ADDR(x), HWIO_GENI_SER_S_CLK_CFG_RMSK)
#define HWIO_GENI_SER_S_CLK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_SER_S_CLK_CFG_ADDR(x), m)
#define HWIO_GENI_SER_S_CLK_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_SER_S_CLK_CFG_ADDR(x),v)
#define HWIO_GENI_SER_S_CLK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_SER_S_CLK_CFG_ADDR(x),m,v,HWIO_GENI_SER_S_CLK_CFG_IN(x))
#define HWIO_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_BMSK                                              0xfff0
#define HWIO_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_SHFT                                                 0x4
#define HWIO_GENI_SER_S_CLK_CFG_SER_CLK_EN_BMSK                                                    0x1
#define HWIO_GENI_SER_S_CLK_CFG_SER_CLK_EN_SHFT                                                    0x0

#define HWIO_GENI_PROG_ROM_CTRL_REG_ADDR(x)                                                 ((x) + 0x00000050)
#define HWIO_GENI_PROG_ROM_CTRL_REG_OFFS                                                    (0x00000050)
#define HWIO_GENI_PROG_ROM_CTRL_REG_RMSK                                                           0x1
#define HWIO_GENI_PROG_ROM_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_PROG_ROM_CTRL_REG_ADDR(x), HWIO_GENI_PROG_ROM_CTRL_REG_RMSK)
#define HWIO_GENI_PROG_ROM_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_PROG_ROM_CTRL_REG_ADDR(x), m)
#define HWIO_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_BMSK                                               0x1
#define HWIO_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_SHFT                                               0x0

#define HWIO_GENI_CLK_CTRL_RO_ADDR(x)                                                       ((x) + 0x00000060)
#define HWIO_GENI_CLK_CTRL_RO_OFFS                                                          (0x00000060)
#define HWIO_GENI_CLK_CTRL_RO_RMSK                                                                 0x1
#define HWIO_GENI_CLK_CTRL_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_CLK_CTRL_RO_ADDR(x), HWIO_GENI_CLK_CTRL_RO_RMSK)
#define HWIO_GENI_CLK_CTRL_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CLK_CTRL_RO_ADDR(x), m)
#define HWIO_GENI_CLK_CTRL_RO_SER_CLK_SEL_BMSK                                                     0x1
#define HWIO_GENI_CLK_CTRL_RO_SER_CLK_SEL_SHFT                                                     0x0

#define HWIO_FIFO_IF_DISABLE_RO_ADDR(x)                                                     ((x) + 0x00000064)
#define HWIO_FIFO_IF_DISABLE_RO_OFFS                                                        (0x00000064)
#define HWIO_FIFO_IF_DISABLE_RO_RMSK                                                               0x1
#define HWIO_FIFO_IF_DISABLE_RO_IN(x)      \
        in_dword_masked(HWIO_FIFO_IF_DISABLE_RO_ADDR(x), HWIO_FIFO_IF_DISABLE_RO_RMSK)
#define HWIO_FIFO_IF_DISABLE_RO_INM(x, m)      \
        in_dword_masked(HWIO_FIFO_IF_DISABLE_RO_ADDR(x), m)
#define HWIO_FIFO_IF_DISABLE_RO_FIFO_IF_DISABLE_BMSK                                               0x1
#define HWIO_FIFO_IF_DISABLE_RO_FIFO_IF_DISABLE_SHFT                                               0x0

#define HWIO_GENI_FW_REVISION_RO_ADDR(x)                                                    ((x) + 0x00000068)
#define HWIO_GENI_FW_REVISION_RO_OFFS                                                       (0x00000068)
#define HWIO_GENI_FW_REVISION_RO_RMSK                                                           0xffff
#define HWIO_GENI_FW_REVISION_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_REVISION_RO_ADDR(x), HWIO_GENI_FW_REVISION_RO_RMSK)
#define HWIO_GENI_FW_REVISION_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_REVISION_RO_ADDR(x), m)
#define HWIO_GENI_FW_REVISION_RO_PROTOCOL_BMSK                                                  0xff00
#define HWIO_GENI_FW_REVISION_RO_PROTOCOL_SHFT                                                     0x8
#define HWIO_GENI_FW_REVISION_RO_VERSION_BMSK                                                     0xff
#define HWIO_GENI_FW_REVISION_RO_VERSION_SHFT                                                      0x0

#define HWIO_GENI_S_FW_REVISION_RO_ADDR(x)                                                  ((x) + 0x0000006c)
#define HWIO_GENI_S_FW_REVISION_RO_OFFS                                                     (0x0000006c)
#define HWIO_GENI_S_FW_REVISION_RO_RMSK                                                         0xffff
#define HWIO_GENI_S_FW_REVISION_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_S_FW_REVISION_RO_ADDR(x), HWIO_GENI_S_FW_REVISION_RO_RMSK)
#define HWIO_GENI_S_FW_REVISION_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_FW_REVISION_RO_ADDR(x), m)
#define HWIO_GENI_S_FW_REVISION_RO_PROTOCOL_BMSK                                                0xff00
#define HWIO_GENI_S_FW_REVISION_RO_PROTOCOL_SHFT                                                   0x8
#define HWIO_GENI_S_FW_REVISION_RO_VERSION_BMSK                                                   0xff
#define HWIO_GENI_S_FW_REVISION_RO_VERSION_SHFT                                                    0x0

#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_ADDR(x)                                            ((x) + 0x00000070)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_OFFS                                               (0x00000070)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_RMSK                                                      0x1
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_PROTNS_RO_ADDR(x), HWIO_GENI_FW_MULTILOCK_PROTNS_RO_RMSK)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_PROTNS_RO_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_GENI_FW_MULTILOCK_PROTNS_BMSK                             0x1
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RO_GENI_FW_MULTILOCK_PROTNS_SHFT                             0x0

#define HWIO_GENI_FW_MULTILOCK_MSA_RO_ADDR(x)                                               ((x) + 0x00000074)
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_OFFS                                                  (0x00000074)
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_RMSK                                                         0x1
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_MSA_RO_ADDR(x), HWIO_GENI_FW_MULTILOCK_MSA_RO_RMSK)
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_MSA_RO_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_GENI_FW_MULTILOCK_MSA_BMSK                                   0x1
#define HWIO_GENI_FW_MULTILOCK_MSA_RO_GENI_FW_MULTILOCK_MSA_SHFT                                   0x0

#define HWIO_GENI_FW_MULTILOCK_SP_RO_ADDR(x)                                                ((x) + 0x00000078)
#define HWIO_GENI_FW_MULTILOCK_SP_RO_OFFS                                                   (0x00000078)
#define HWIO_GENI_FW_MULTILOCK_SP_RO_RMSK                                                          0x1
#define HWIO_GENI_FW_MULTILOCK_SP_RO_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_SP_RO_ADDR(x), HWIO_GENI_FW_MULTILOCK_SP_RO_RMSK)
#define HWIO_GENI_FW_MULTILOCK_SP_RO_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_SP_RO_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_SP_RO_GENI_FW_MULTILOCK_SP_BMSK                                     0x1
#define HWIO_GENI_FW_MULTILOCK_SP_RO_GENI_FW_MULTILOCK_SP_SHFT                                     0x0

#define HWIO_GENI_CLK_SEL_ADDR(x)                                                           ((x) + 0x0000007c)
#define HWIO_GENI_CLK_SEL_OFFS                                                              (0x0000007c)
#define HWIO_GENI_CLK_SEL_RMSK                                                                     0x7
#define HWIO_GENI_CLK_SEL_IN(x)      \
        in_dword_masked(HWIO_GENI_CLK_SEL_ADDR(x), HWIO_GENI_CLK_SEL_RMSK)
#define HWIO_GENI_CLK_SEL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CLK_SEL_ADDR(x), m)
#define HWIO_GENI_CLK_SEL_OUT(x, v)      \
        out_dword(HWIO_GENI_CLK_SEL_ADDR(x),v)
#define HWIO_GENI_CLK_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CLK_SEL_ADDR(x),m,v,HWIO_GENI_CLK_SEL_IN(x))
#define HWIO_GENI_CLK_SEL_CLK_SEL_BMSK                                                             0x7
#define HWIO_GENI_CLK_SEL_CLK_SEL_SHFT                                                             0x0

#define HWIO_GENI_DFS_IF_CFG_ADDR(x)                                                        ((x) + 0x00000080)
#define HWIO_GENI_DFS_IF_CFG_OFFS                                                           (0x00000080)
#define HWIO_GENI_DFS_IF_CFG_RMSK                                                                0x701
#define HWIO_GENI_DFS_IF_CFG_IN(x)      \
        in_dword_masked(HWIO_GENI_DFS_IF_CFG_ADDR(x), HWIO_GENI_DFS_IF_CFG_RMSK)
#define HWIO_GENI_DFS_IF_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_DFS_IF_CFG_ADDR(x), m)
#define HWIO_GENI_DFS_IF_CFG_OUT(x, v)      \
        out_dword(HWIO_GENI_DFS_IF_CFG_ADDR(x),v)
#define HWIO_GENI_DFS_IF_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_DFS_IF_CFG_ADDR(x),m,v,HWIO_GENI_DFS_IF_CFG_IN(x))
#define HWIO_GENI_DFS_IF_CFG_NUM_WAIT_STATES_BMSK                                                0x700
#define HWIO_GENI_DFS_IF_CFG_NUM_WAIT_STATES_SHFT                                                  0x8
#define HWIO_GENI_DFS_IF_CFG_DFS_IF_EN_BMSK                                                        0x1
#define HWIO_GENI_DFS_IF_CFG_DFS_IF_EN_SHFT                                                        0x0

/*----------------------------------------------------------------------------
 * MODULE: GENI4_DATA
 *--------------------------------------------------------------------------*/

#define GENI4_DATA_REG_BASE                                                           (QUPV3_0_QUPV3_ID_1_BASE      + 0x00080600)
#define GENI4_DATA_REG_BASE_OFFS                                                      0x00080600

#define HWIO_GENI_M_CMD0_ADDR(x)                                                      ((x) + 0x00000000)
#define HWIO_GENI_M_CMD0_OFFS                                                         (0x00000000)
#define HWIO_GENI_M_CMD0_RMSK                                                         0xf9ffffff
#define HWIO_GENI_M_CMD0_IN(x)      \
        in_dword_masked(HWIO_GENI_M_CMD0_ADDR(x), HWIO_GENI_M_CMD0_RMSK)
#define HWIO_GENI_M_CMD0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_CMD0_ADDR(x), m)
#define HWIO_GENI_M_CMD0_OUT(x, v)      \
        out_dword(HWIO_GENI_M_CMD0_ADDR(x),v)
#define HWIO_GENI_M_CMD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_M_CMD0_ADDR(x),m,v,HWIO_GENI_M_CMD0_IN(x))
#define HWIO_GENI_M_CMD0_OPCODE_BMSK                                                  0xf8000000
#define HWIO_GENI_M_CMD0_OPCODE_SHFT                                                        0x1b
#define HWIO_GENI_M_CMD0_GNRL_PURP_BMSK                                                0x1000000
#define HWIO_GENI_M_CMD0_GNRL_PURP_SHFT                                                     0x18
#define HWIO_GENI_M_CMD0_PARAM_BMSK                                                     0xffffff
#define HWIO_GENI_M_CMD0_PARAM_SHFT                                                          0x0

#define HWIO_GENI_M_CMD_CTRL_REG_ADDR(x)                                              ((x) + 0x00000004)
#define HWIO_GENI_M_CMD_CTRL_REG_OFFS                                                 (0x00000004)
#define HWIO_GENI_M_CMD_CTRL_REG_RMSK                                                        0x7
#define HWIO_GENI_M_CMD_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_M_CMD_CTRL_REG_ADDR(x), HWIO_GENI_M_CMD_CTRL_REG_RMSK)
#define HWIO_GENI_M_CMD_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_CMD_CTRL_REG_ADDR(x), m)
#define HWIO_GENI_M_CMD_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_M_CMD_CTRL_REG_ADDR(x),v)
#define HWIO_GENI_M_CMD_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_M_CMD_CTRL_REG_ADDR(x),m,v,HWIO_GENI_M_CMD_CTRL_REG_IN(x))
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_CMD_CANCEL_BMSK                                      0x4
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_CMD_CANCEL_SHFT                                      0x2
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_CMD_ABORT_BMSK                                       0x2
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_CMD_ABORT_SHFT                                       0x1
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_DISABLE_BMSK                                         0x1
#define HWIO_GENI_M_CMD_CTRL_REG_M_GENI_DISABLE_SHFT                                         0x0

#define HWIO_GENI_M_IRQ_STATUS_ADDR(x)                                                ((x) + 0x00000010)
#define HWIO_GENI_M_IRQ_STATUS_OFFS                                                   (0x00000010)
#define HWIO_GENI_M_IRQ_STATUS_RMSK                                                   0xffc07fff
#define HWIO_GENI_M_IRQ_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_M_IRQ_STATUS_ADDR(x), HWIO_GENI_M_IRQ_STATUS_RMSK)
#define HWIO_GENI_M_IRQ_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_IRQ_STATUS_ADDR(x), m)
#define HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_BMSK                                           0x80000000
#define HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_SHFT                                                 0x1f
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_BMSK                                 0x40000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_SHFT                                       0x1e
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_BMSK                                    0x20000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_SHFT                                          0x1d
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_BMSK                                    0x10000000
#define HWIO_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_SHFT                                          0x1c
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_LAST_BMSK                                       0x8000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_LAST_SHFT                                            0x1b
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                                  0x4000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                                       0x1a
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                                     0x2000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                                          0x19
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                                     0x1000000
#define HWIO_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                                          0x18
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_BMSK                                      0x800000
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_SHFT                                          0x17
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                                    0x400000
#define HWIO_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                                        0x16
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_5_BMSK                                            0x4000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_5_SHFT                                               0xe
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_4_BMSK                                            0x2000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_4_SHFT                                               0xd
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_3_BMSK                                            0x1000
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_3_SHFT                                               0xc
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_2_BMSK                                             0x800
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_2_SHFT                                               0xb
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_1_BMSK                                             0x400
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_1_SHFT                                               0xa
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_0_BMSK                                             0x200
#define HWIO_GENI_M_IRQ_STATUS_M_GP_IRQ_0_SHFT                                               0x9
#define HWIO_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_BMSK                                        0x100
#define HWIO_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_SHFT                                          0x8
#define HWIO_GENI_M_IRQ_STATUS_M_RX_IRQ_BMSK                                                0x80
#define HWIO_GENI_M_IRQ_STATUS_M_RX_IRQ_SHFT                                                 0x7
#define HWIO_GENI_M_IRQ_STATUS_M_TIMESTAMP_BMSK                                             0x40
#define HWIO_GENI_M_IRQ_STATUS_M_TIMESTAMP_SHFT                                              0x6
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_ABORT_BMSK                                             0x20
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_ABORT_SHFT                                              0x5
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_CANCEL_BMSK                                            0x10
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_CANCEL_SHFT                                             0x4
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_FAILURE_BMSK                                            0x8
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_FAILURE_SHFT                                            0x3
#define HWIO_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_BMSK                                            0x4
#define HWIO_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_SHFT                                            0x2
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_BMSK                                            0x2
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_SHFT                                            0x1
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_BMSK                                               0x1
#define HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_SHFT                                               0x0

#define HWIO_GENI_M_IRQ_ENABLE_ADDR(x)                                                ((x) + 0x00000014)
#define HWIO_GENI_M_IRQ_ENABLE_OFFS                                                   (0x00000014)
#define HWIO_GENI_M_IRQ_ENABLE_RMSK                                                   0xffc07fff
#define HWIO_GENI_M_IRQ_ENABLE_IN(x)      \
        in_dword_masked(HWIO_GENI_M_IRQ_ENABLE_ADDR(x), HWIO_GENI_M_IRQ_ENABLE_RMSK)
#define HWIO_GENI_M_IRQ_ENABLE_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_IRQ_ENABLE_ADDR(x), m)
#define HWIO_GENI_M_IRQ_ENABLE_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_ENABLE_ADDR(x),v)
#define HWIO_GENI_M_IRQ_ENABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_M_IRQ_ENABLE_ADDR(x),m,v,HWIO_GENI_M_IRQ_ENABLE_IN(x))
#define HWIO_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_BMSK                                        0x80000000
#define HWIO_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_SHFT                                              0x1f
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_BMSK                              0x40000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_SHFT                                    0x1e
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_BMSK                                 0x20000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_SHFT                                       0x1d
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_BMSK                                 0x10000000
#define HWIO_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_SHFT                                       0x1c
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                                    0x8000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                                         0x1b
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                               0x4000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                                    0x1a
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                                  0x2000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                                       0x19
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                                  0x1000000
#define HWIO_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                                       0x18
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                                   0x800000
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                                       0x17
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                                 0x400000
#define HWIO_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                                     0x16
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_BMSK                                         0x4000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_SHFT                                            0xe
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_BMSK                                         0x2000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_SHFT                                            0xd
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_BMSK                                         0x1000
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_SHFT                                            0xc
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_BMSK                                          0x800
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_SHFT                                            0xb
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_BMSK                                          0x400
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_SHFT                                            0xa
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_BMSK                                          0x200
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_SHFT                                            0x9
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_BMSK                                     0x100
#define HWIO_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_SHFT                                       0x8
#define HWIO_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_BMSK                                             0x80
#define HWIO_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_SHFT                                              0x7
#define HWIO_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_BMSK                                          0x40
#define HWIO_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_SHFT                                           0x6
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_BMSK                                          0x20
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_SHFT                                           0x5
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_BMSK                                         0x10
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_SHFT                                          0x4
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_BMSK                                         0x8
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_SHFT                                         0x3
#define HWIO_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_BMSK                                         0x4
#define HWIO_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_SHFT                                         0x2
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_BMSK                                         0x2
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_SHFT                                         0x1
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_BMSK                                            0x1
#define HWIO_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_SHFT                                            0x0

#define HWIO_GENI_M_IRQ_CLEAR_ADDR(x)                                                 ((x) + 0x00000018)
#define HWIO_GENI_M_IRQ_CLEAR_OFFS                                                    (0x00000018)
#define HWIO_GENI_M_IRQ_CLEAR_RMSK                                                    0xffc07fff
#define HWIO_GENI_M_IRQ_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_CLEAR_ADDR(x),v)
#define HWIO_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_BMSK                                      0x80000000
#define HWIO_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_SHFT                                            0x1f
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_BMSK                            0x40000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_SHFT                                  0x1e
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_BMSK                               0x20000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_SHFT                                     0x1d
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_BMSK                               0x10000000
#define HWIO_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_SHFT                                     0x1c
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                                  0x8000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                                       0x1b
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                             0x4000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                                  0x1a
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                                0x2000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                                     0x19
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                                0x1000000
#define HWIO_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                                     0x18
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                                 0x800000
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                                     0x17
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                               0x400000
#define HWIO_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                                   0x16
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_BMSK                                       0x4000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_SHFT                                          0xe
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_BMSK                                       0x2000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_SHFT                                          0xd
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_BMSK                                       0x1000
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_SHFT                                          0xc
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_BMSK                                        0x800
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_SHFT                                          0xb
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_BMSK                                        0x400
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_SHFT                                          0xa
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_BMSK                                        0x200
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_SHFT                                          0x9
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_BMSK                                   0x100
#define HWIO_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_SHFT                                     0x8
#define HWIO_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_BMSK                                           0x80
#define HWIO_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_SHFT                                            0x7
#define HWIO_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_BMSK                                        0x40
#define HWIO_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_SHFT                                         0x6
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_BMSK                                        0x20
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_SHFT                                         0x5
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_BMSK                                       0x10
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_SHFT                                        0x4
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_BMSK                                       0x8
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_SHFT                                       0x3
#define HWIO_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_BMSK                                       0x4
#define HWIO_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_SHFT                                       0x2
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_BMSK                                       0x2
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_SHFT                                       0x1
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_BMSK                                          0x1
#define HWIO_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_SHFT                                          0x0

#define HWIO_GENI_M_IRQ_EN_SET_ADDR(x)                                                ((x) + 0x0000001c)
#define HWIO_GENI_M_IRQ_EN_SET_OFFS                                                   (0x0000001c)
#define HWIO_GENI_M_IRQ_EN_SET_RMSK                                                   0xffc07fff
#define HWIO_GENI_M_IRQ_EN_SET_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_EN_SET_ADDR(x),v)
#define HWIO_GENI_M_IRQ_EN_SET_SEC_IRQ_EN_SET_BMSK                                    0x80000000
#define HWIO_GENI_M_IRQ_EN_SET_SEC_IRQ_EN_SET_SHFT                                          0x1f
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_WATERMARK_EN_SET_BMSK                          0x40000000
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_WATERMARK_EN_SET_SHFT                                0x1e
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_WR_ERR_EN_SET_BMSK                             0x20000000
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_WR_ERR_EN_SET_SHFT                                   0x1d
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_RD_ERR_EN_SET_BMSK                             0x10000000
#define HWIO_GENI_M_IRQ_EN_SET_TX_FIFO_RD_ERR_EN_SET_SHFT                                   0x1c
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_BMSK                                0x8000000
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_SHFT                                     0x1b
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_BMSK                           0x4000000
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_SHFT                                0x1a
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_BMSK                              0x2000000
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_SHFT                                   0x19
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_BMSK                              0x1000000
#define HWIO_GENI_M_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_SHFT                                   0x18
#define HWIO_GENI_M_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_BMSK                               0x800000
#define HWIO_GENI_M_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_SHFT                                   0x17
#define HWIO_GENI_M_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_BMSK                             0x400000
#define HWIO_GENI_M_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_SHFT                                 0x16
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_5_EN_SET_BMSK                                     0x4000
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_5_EN_SET_SHFT                                        0xe
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_4_EN_SET_BMSK                                     0x2000
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_4_EN_SET_SHFT                                        0xd
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_3_EN_SET_BMSK                                     0x1000
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_3_EN_SET_SHFT                                        0xc
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_2_EN_SET_BMSK                                      0x800
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_2_EN_SET_SHFT                                        0xb
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_1_EN_SET_BMSK                                      0x400
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_1_EN_SET_SHFT                                        0xa
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_0_EN_SET_BMSK                                      0x200
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_IRQ_0_EN_SET_SHFT                                        0x9
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_SYNC_IRQ_0_EN_SET_BMSK                                 0x100
#define HWIO_GENI_M_IRQ_EN_SET_M_GP_SYNC_IRQ_0_EN_SET_SHFT                                   0x8
#define HWIO_GENI_M_IRQ_EN_SET_M_RX_IRQ_EN_SET_BMSK                                         0x80
#define HWIO_GENI_M_IRQ_EN_SET_M_RX_IRQ_EN_SET_SHFT                                          0x7
#define HWIO_GENI_M_IRQ_EN_SET_M_TIMESTAMP_EN_SET_BMSK                                      0x40
#define HWIO_GENI_M_IRQ_EN_SET_M_TIMESTAMP_EN_SET_SHFT                                       0x6
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_ABORT_EN_SET_BMSK                                      0x20
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_ABORT_EN_SET_SHFT                                       0x5
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_CANCEL_EN_SET_BMSK                                     0x10
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_CANCEL_EN_SET_SHFT                                      0x4
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_FAILURE_EN_SET_BMSK                                     0x8
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_FAILURE_EN_SET_SHFT                                     0x3
#define HWIO_GENI_M_IRQ_EN_SET_M_ILLEGAL_CMD_EN_SET_BMSK                                     0x4
#define HWIO_GENI_M_IRQ_EN_SET_M_ILLEGAL_CMD_EN_SET_SHFT                                     0x2
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_OVERRUN_EN_SET_BMSK                                     0x2
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_OVERRUN_EN_SET_SHFT                                     0x1
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_DONE_EN_SET_BMSK                                        0x1
#define HWIO_GENI_M_IRQ_EN_SET_M_CMD_DONE_EN_SET_SHFT                                        0x0

#define HWIO_GENI_M_IRQ_EN_CLEAR_ADDR(x)                                              ((x) + 0x00000020)
#define HWIO_GENI_M_IRQ_EN_CLEAR_OFFS                                                 (0x00000020)
#define HWIO_GENI_M_IRQ_EN_CLEAR_RMSK                                                 0xffc07fff
#define HWIO_GENI_M_IRQ_EN_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_M_IRQ_EN_CLEAR_ADDR(x),v)
#define HWIO_GENI_M_IRQ_EN_CLEAR_SEC_IRQ_EN_CLEAR_BMSK                                0x80000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_SEC_IRQ_EN_CLEAR_SHFT                                      0x1f
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WATERMARK_EN_CLEAR_BMSK                      0x40000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WATERMARK_EN_CLEAR_SHFT                            0x1e
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WR_ERR_EN_CLEAR_BMSK                         0x20000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WR_ERR_EN_CLEAR_SHFT                               0x1d
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_RD_ERR_EN_CLEAR_BMSK                         0x10000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_TX_FIFO_RD_ERR_EN_CLEAR_SHFT                               0x1c
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_BMSK                            0x8000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_SHFT                                 0x1b
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_BMSK                       0x4000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_SHFT                            0x1a
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_BMSK                          0x2000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_SHFT                               0x19
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_BMSK                          0x1000000
#define HWIO_GENI_M_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_SHFT                               0x18
#define HWIO_GENI_M_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_BMSK                           0x800000
#define HWIO_GENI_M_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_SHFT                               0x17
#define HWIO_GENI_M_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_BMSK                         0x400000
#define HWIO_GENI_M_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_SHFT                             0x16
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_5_EN_CLEAR_BMSK                                 0x4000
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_5_EN_CLEAR_SHFT                                    0xe
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_4_EN_CLEAR_BMSK                                 0x2000
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_4_EN_CLEAR_SHFT                                    0xd
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_3_EN_CLEAR_BMSK                                 0x1000
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_3_EN_CLEAR_SHFT                                    0xc
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_2_EN_CLEAR_BMSK                                  0x800
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_2_EN_CLEAR_SHFT                                    0xb
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_1_EN_CLEAR_BMSK                                  0x400
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_1_EN_CLEAR_SHFT                                    0xa
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_0_EN_CLEAR_BMSK                                  0x200
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_0_EN_CLEAR_SHFT                                    0x9
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_SYNC_IRQ_0_EN_CLEAR_BMSK                             0x100
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_GP_SYNC_IRQ_0_EN_CLEAR_SHFT                               0x8
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_RX_IRQ_EN_CLEAR_BMSK                                     0x80
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_RX_IRQ_EN_CLEAR_SHFT                                      0x7
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_TIMESTAMP_EN_CLEAR_BMSK                                  0x40
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_TIMESTAMP_EN_CLEAR_SHFT                                   0x6
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_ABORT_EN_CLEAR_BMSK                                  0x20
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_ABORT_EN_CLEAR_SHFT                                   0x5
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_CANCEL_EN_CLEAR_BMSK                                 0x10
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_CANCEL_EN_CLEAR_SHFT                                  0x4
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_FAILURE_EN_CLEAR_BMSK                                 0x8
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_FAILURE_EN_CLEAR_SHFT                                 0x3
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_ILLEGAL_CMD_EN_CLEAR_BMSK                                 0x4
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_ILLEGAL_CMD_EN_CLEAR_SHFT                                 0x2
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_OVERRUN_EN_CLEAR_BMSK                                 0x2
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_OVERRUN_EN_CLEAR_SHFT                                 0x1
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_DONE_EN_CLEAR_BMSK                                    0x1
#define HWIO_GENI_M_IRQ_EN_CLEAR_M_CMD_DONE_EN_CLEAR_SHFT                                    0x0

#define HWIO_GENI_S_CMD0_ADDR(x)                                                      ((x) + 0x00000030)
#define HWIO_GENI_S_CMD0_OFFS                                                         (0x00000030)
#define HWIO_GENI_S_CMD0_RMSK                                                         0xf9ffffff
#define HWIO_GENI_S_CMD0_IN(x)      \
        in_dword_masked(HWIO_GENI_S_CMD0_ADDR(x), HWIO_GENI_S_CMD0_RMSK)
#define HWIO_GENI_S_CMD0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_CMD0_ADDR(x), m)
#define HWIO_GENI_S_CMD0_OUT(x, v)      \
        out_dword(HWIO_GENI_S_CMD0_ADDR(x),v)
#define HWIO_GENI_S_CMD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_CMD0_ADDR(x),m,v,HWIO_GENI_S_CMD0_IN(x))
#define HWIO_GENI_S_CMD0_OPCODE_BMSK                                                  0xf8000000
#define HWIO_GENI_S_CMD0_OPCODE_SHFT                                                        0x1b
#define HWIO_GENI_S_CMD0_GNRL_PURP_BMSK                                                0x1000000
#define HWIO_GENI_S_CMD0_GNRL_PURP_SHFT                                                     0x18
#define HWIO_GENI_S_CMD0_PARAM_BMSK                                                     0xffffff
#define HWIO_GENI_S_CMD0_PARAM_SHFT                                                          0x0

#define HWIO_GENI_S_CMD_CTRL_REG_ADDR(x)                                              ((x) + 0x00000034)
#define HWIO_GENI_S_CMD_CTRL_REG_OFFS                                                 (0x00000034)
#define HWIO_GENI_S_CMD_CTRL_REG_RMSK                                                        0x7
#define HWIO_GENI_S_CMD_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_S_CMD_CTRL_REG_ADDR(x), HWIO_GENI_S_CMD_CTRL_REG_RMSK)
#define HWIO_GENI_S_CMD_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_CMD_CTRL_REG_ADDR(x), m)
#define HWIO_GENI_S_CMD_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_S_CMD_CTRL_REG_ADDR(x),v)
#define HWIO_GENI_S_CMD_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_CMD_CTRL_REG_ADDR(x),m,v,HWIO_GENI_S_CMD_CTRL_REG_IN(x))
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_CMD_CANCEL_BMSK                                      0x4
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_CMD_CANCEL_SHFT                                      0x2
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_CMD_ABORT_BMSK                                       0x2
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_CMD_ABORT_SHFT                                       0x1
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_DISABLE_BMSK                                         0x1
#define HWIO_GENI_S_CMD_CTRL_REG_S_GENI_DISABLE_SHFT                                         0x0

#define HWIO_GENI_S_IRQ_STATUS_ADDR(x)                                                ((x) + 0x00000040)
#define HWIO_GENI_S_IRQ_STATUS_OFFS                                                   (0x00000040)
#define HWIO_GENI_S_IRQ_STATUS_RMSK                                                    0xfc07f3f
#define HWIO_GENI_S_IRQ_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_S_IRQ_STATUS_ADDR(x), HWIO_GENI_S_IRQ_STATUS_RMSK)
#define HWIO_GENI_S_IRQ_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_IRQ_STATUS_ADDR(x), m)
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_BMSK                                       0x8000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_SHFT                                            0x1b
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                                  0x4000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                                       0x1a
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                                     0x2000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                                          0x19
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                                     0x1000000
#define HWIO_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                                          0x18
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_BMSK                                      0x800000
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_SHFT                                          0x17
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                                    0x400000
#define HWIO_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                                        0x16
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_5_BMSK                                            0x4000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_5_SHFT                                               0xe
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_4_BMSK                                            0x2000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_4_SHFT                                               0xd
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_BMSK                                            0x1000
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_SHFT                                               0xc
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_BMSK                                             0x800
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_SHFT                                               0xb
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_1_BMSK                                             0x400
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_1_SHFT                                               0xa
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_0_BMSK                                             0x200
#define HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_0_SHFT                                               0x9
#define HWIO_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_BMSK                                        0x100
#define HWIO_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_SHFT                                          0x8
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_ABORT_BMSK                                             0x20
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_ABORT_SHFT                                              0x5
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_CANCEL_BMSK                                            0x10
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_CANCEL_SHFT                                             0x4
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_FAILURE_BMSK                                            0x8
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_FAILURE_SHFT                                            0x3
#define HWIO_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_BMSK                                            0x4
#define HWIO_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_SHFT                                            0x2
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_BMSK                                            0x2
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_SHFT                                            0x1
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_DONE_BMSK                                               0x1
#define HWIO_GENI_S_IRQ_STATUS_S_CMD_DONE_SHFT                                               0x0

#define HWIO_GENI_S_IRQ_ENABLE_ADDR(x)                                                ((x) + 0x00000044)
#define HWIO_GENI_S_IRQ_ENABLE_OFFS                                                   (0x00000044)
#define HWIO_GENI_S_IRQ_ENABLE_RMSK                                                    0xfc07f3f
#define HWIO_GENI_S_IRQ_ENABLE_IN(x)      \
        in_dword_masked(HWIO_GENI_S_IRQ_ENABLE_ADDR(x), HWIO_GENI_S_IRQ_ENABLE_RMSK)
#define HWIO_GENI_S_IRQ_ENABLE_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_IRQ_ENABLE_ADDR(x), m)
#define HWIO_GENI_S_IRQ_ENABLE_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_ENABLE_ADDR(x),v)
#define HWIO_GENI_S_IRQ_ENABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_IRQ_ENABLE_ADDR(x),m,v,HWIO_GENI_S_IRQ_ENABLE_IN(x))
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                                    0x8000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                                         0x1b
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                               0x4000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                                    0x1a
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                                  0x2000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                                       0x19
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                                  0x1000000
#define HWIO_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                                       0x18
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                                   0x800000
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                                       0x17
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                                 0x400000
#define HWIO_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                                     0x16
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_BMSK                                         0x4000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_SHFT                                            0xe
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_BMSK                                         0x2000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_SHFT                                            0xd
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_BMSK                                         0x1000
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_SHFT                                            0xc
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_BMSK                                          0x800
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_SHFT                                            0xb
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_BMSK                                          0x400
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_SHFT                                            0xa
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_BMSK                                          0x200
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_SHFT                                            0x9
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_BMSK                                     0x100
#define HWIO_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_SHFT                                       0x8
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_BMSK                                          0x20
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_SHFT                                           0x5
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_BMSK                                         0x10
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_SHFT                                          0x4
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_BMSK                                         0x8
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_SHFT                                         0x3
#define HWIO_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_BMSK                                         0x4
#define HWIO_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_SHFT                                         0x2
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_BMSK                                         0x2
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_SHFT                                         0x1
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_BMSK                                            0x1
#define HWIO_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_SHFT                                            0x0

#define HWIO_GENI_S_IRQ_CLEAR_ADDR(x)                                                 ((x) + 0x00000048)
#define HWIO_GENI_S_IRQ_CLEAR_OFFS                                                    (0x00000048)
#define HWIO_GENI_S_IRQ_CLEAR_RMSK                                                     0xfc07f3f
#define HWIO_GENI_S_IRQ_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_CLEAR_ADDR(x),v)
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                                  0x8000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                                       0x1b
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                             0x4000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                                  0x1a
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                                0x2000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                                     0x19
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                                0x1000000
#define HWIO_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                                     0x18
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                                 0x800000
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                                     0x17
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                               0x400000
#define HWIO_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                                   0x16
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_BMSK                                       0x4000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_SHFT                                          0xe
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_BMSK                                       0x2000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_SHFT                                          0xd
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_BMSK                                       0x1000
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_SHFT                                          0xc
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_BMSK                                        0x800
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_SHFT                                          0xb
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_BMSK                                        0x400
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_SHFT                                          0xa
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_BMSK                                        0x200
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_SHFT                                          0x9
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_BMSK                                   0x100
#define HWIO_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_SHFT                                     0x8
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_BMSK                                        0x20
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_SHFT                                         0x5
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_BMSK                                       0x10
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_SHFT                                        0x4
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_BMSK                                       0x8
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_SHFT                                       0x3
#define HWIO_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_BMSK                                       0x4
#define HWIO_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_SHFT                                       0x2
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_BMSK                                       0x2
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_SHFT                                       0x1
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_BMSK                                          0x1
#define HWIO_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_SHFT                                          0x0

#define HWIO_GENI_S_IRQ_EN_SET_ADDR(x)                                                ((x) + 0x0000004c)
#define HWIO_GENI_S_IRQ_EN_SET_OFFS                                                   (0x0000004c)
#define HWIO_GENI_S_IRQ_EN_SET_RMSK                                                    0xfc07f3f
#define HWIO_GENI_S_IRQ_EN_SET_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_EN_SET_ADDR(x),v)
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_BMSK                                0x8000000
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_SHFT                                     0x1b
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_BMSK                           0x4000000
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_SHFT                                0x1a
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_BMSK                              0x2000000
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_SHFT                                   0x19
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_BMSK                              0x1000000
#define HWIO_GENI_S_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_SHFT                                   0x18
#define HWIO_GENI_S_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_BMSK                               0x800000
#define HWIO_GENI_S_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_SHFT                                   0x17
#define HWIO_GENI_S_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_BMSK                             0x400000
#define HWIO_GENI_S_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_SHFT                                 0x16
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_5_EN_SET_BMSK                                     0x4000
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_5_EN_SET_SHFT                                        0xe
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_4_EN_SET_BMSK                                     0x2000
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_4_EN_SET_SHFT                                        0xd
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_3_EN_SET_BMSK                                     0x1000
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_3_EN_SET_SHFT                                        0xc
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_2_EN_SET_BMSK                                      0x800
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_2_EN_SET_SHFT                                        0xb
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_1_EN_SET_BMSK                                      0x400
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_1_EN_SET_SHFT                                        0xa
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_0_EN_SET_BMSK                                      0x200
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_IRQ_0_EN_SET_SHFT                                        0x9
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_SYNC_IRQ_0_EN_SET_BMSK                                 0x100
#define HWIO_GENI_S_IRQ_EN_SET_S_GP_SYNC_IRQ_0_EN_SET_SHFT                                   0x8
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_ABORT_EN_SET_BMSK                                      0x20
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_ABORT_EN_SET_SHFT                                       0x5
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_CANCEL_EN_SET_BMSK                                     0x10
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_CANCEL_EN_SET_SHFT                                      0x4
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_FAILURE_EN_SET_BMSK                                     0x8
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_FAILURE_EN_SET_SHFT                                     0x3
#define HWIO_GENI_S_IRQ_EN_SET_S_ILLEGAL_CMD_EN_SET_BMSK                                     0x4
#define HWIO_GENI_S_IRQ_EN_SET_S_ILLEGAL_CMD_EN_SET_SHFT                                     0x2
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_OVERRUN_EN_SET_BMSK                                     0x2
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_OVERRUN_EN_SET_SHFT                                     0x1
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_DONE_EN_SET_BMSK                                        0x1
#define HWIO_GENI_S_IRQ_EN_SET_S_CMD_DONE_EN_SET_SHFT                                        0x0

#define HWIO_GENI_S_IRQ_EN_CLEAR_ADDR(x)                                              ((x) + 0x00000050)
#define HWIO_GENI_S_IRQ_EN_CLEAR_OFFS                                                 (0x00000050)
#define HWIO_GENI_S_IRQ_EN_CLEAR_RMSK                                                  0xfc07f3f
#define HWIO_GENI_S_IRQ_EN_CLEAR_OUT(x, v)      \
        out_dword(HWIO_GENI_S_IRQ_EN_CLEAR_ADDR(x),v)
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_BMSK                            0x8000000
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_SHFT                                 0x1b
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_BMSK                       0x4000000
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_SHFT                            0x1a
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_BMSK                          0x2000000
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_SHFT                               0x19
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_BMSK                          0x1000000
#define HWIO_GENI_S_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_SHFT                               0x18
#define HWIO_GENI_S_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_BMSK                           0x800000
#define HWIO_GENI_S_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_SHFT                               0x17
#define HWIO_GENI_S_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_BMSK                         0x400000
#define HWIO_GENI_S_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_SHFT                             0x16
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_5_EN_CLEAR_BMSK                                 0x4000
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_5_EN_CLEAR_SHFT                                    0xe
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_4_EN_CLEAR_BMSK                                 0x2000
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_4_EN_CLEAR_SHFT                                    0xd
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_3_EN_CLEAR_BMSK                                 0x1000
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_3_EN_CLEAR_SHFT                                    0xc
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_2_EN_CLEAR_BMSK                                  0x800
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_2_EN_CLEAR_SHFT                                    0xb
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_1_EN_CLEAR_BMSK                                  0x400
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_1_EN_CLEAR_SHFT                                    0xa
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_0_EN_CLEAR_BMSK                                  0x200
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_0_EN_CLEAR_SHFT                                    0x9
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_SYNC_IRQ_0_EN_CLEAR_BMSK                             0x100
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_GP_SYNC_IRQ_0_EN_CLEAR_SHFT                               0x8
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_ABORT_EN_CLEAR_BMSK                                  0x20
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_ABORT_EN_CLEAR_SHFT                                   0x5
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_CANCEL_EN_CLEAR_BMSK                                 0x10
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_CANCEL_EN_CLEAR_SHFT                                  0x4
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_FAILURE_EN_CLEAR_BMSK                                 0x8
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_FAILURE_EN_CLEAR_SHFT                                 0x3
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_ILLEGAL_CMD_EN_CLEAR_BMSK                                 0x4
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_ILLEGAL_CMD_EN_CLEAR_SHFT                                 0x2
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_OVERRUN_EN_CLEAR_BMSK                                 0x2
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_OVERRUN_EN_CLEAR_SHFT                                 0x1
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_DONE_EN_CLEAR_BMSK                                    0x1
#define HWIO_GENI_S_IRQ_EN_CLEAR_S_CMD_DONE_EN_CLEAR_SHFT                                    0x0

#define HWIO_GENI_TX_FIFOn_ADDR(base,n)                                               ((base) + 0x00000100 + 0x4 * (n))
#define HWIO_GENI_TX_FIFOn_OFFS(base,n)                                               (0x00000100 + 0x4 * (n))
#define HWIO_GENI_TX_FIFOn_RMSK                                                       0xffffffff
#define HWIO_GENI_TX_FIFOn_MAXn                                                               15
#define HWIO_GENI_TX_FIFOn_OUTI(base,n,val)    \
        out_dword(HWIO_GENI_TX_FIFOn_ADDR(base,n),val)
#define HWIO_GENI_TX_FIFOn_TX_DATA_BMSK                                               0xffffffff
#define HWIO_GENI_TX_FIFOn_TX_DATA_SHFT                                                      0x0

#define HWIO_GENI_RX_FIFOn_ADDR(base,n)                                               ((base) + 0x00000180 + 0x4 * (n))
#define HWIO_GENI_RX_FIFOn_OFFS(base,n)                                               (0x00000180 + 0x4 * (n))
#define HWIO_GENI_RX_FIFOn_RMSK                                                       0xffffffff
#define HWIO_GENI_RX_FIFOn_MAXn                                                               15
#define HWIO_GENI_RX_FIFOn_INI(base,n)        \
        in_dword_masked(HWIO_GENI_RX_FIFOn_ADDR(base,n), HWIO_GENI_RX_FIFOn_RMSK)
#define HWIO_GENI_RX_FIFOn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_GENI_RX_FIFOn_ADDR(base,n), mask)
#define HWIO_GENI_RX_FIFOn_RX_DATA_BMSK                                               0xffffffff
#define HWIO_GENI_RX_FIFOn_RX_DATA_SHFT                                                      0x0

#define HWIO_GENI_TX_FIFO_STATUS_ADDR(x)                                              ((x) + 0x00000200)
#define HWIO_GENI_TX_FIFO_STATUS_OFFS                                                 (0x00000200)
#define HWIO_GENI_TX_FIFO_STATUS_RMSK                                                 0xffffffff
#define HWIO_GENI_TX_FIFO_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_FIFO_STATUS_ADDR(x), HWIO_GENI_TX_FIFO_STATUS_RMSK)
#define HWIO_GENI_TX_FIFO_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_FIFO_STATUS_ADDR(x), m)
#define HWIO_GENI_TX_FIFO_STATUS_TX_AFIFO_WC_BMSK                                     0xf0000000
#define HWIO_GENI_TX_FIFO_STATUS_TX_AFIFO_WC_SHFT                                           0x1c
#define HWIO_GENI_TX_FIFO_STATUS_TX_FIFO_WC_BMSK                                       0xfffffff
#define HWIO_GENI_TX_FIFO_STATUS_TX_FIFO_WC_SHFT                                             0x0

#define HWIO_GENI_RX_FIFO_STATUS_ADDR(x)                                              ((x) + 0x00000204)
#define HWIO_GENI_RX_FIFO_STATUS_OFFS                                                 (0x00000204)
#define HWIO_GENI_RX_FIFO_STATUS_RMSK                                                 0xffffffff
#define HWIO_GENI_RX_FIFO_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_FIFO_STATUS_ADDR(x), HWIO_GENI_RX_FIFO_STATUS_RMSK)
#define HWIO_GENI_RX_FIFO_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_FIFO_STATUS_ADDR(x), m)
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BMSK                                         0x80000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_SHFT                                               0x1f
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_BMSK                              0x70000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_SHFT                                    0x1c
#define HWIO_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_BMSK                                      0xe000000
#define HWIO_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_SHFT                                           0x19
#define HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_BMSK                                       0x1ffffff
#define HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_SHFT                                             0x0

#define HWIO_GENI_TX_FIFO_THRESHOLD_ADDR(x)                                           ((x) + 0x00000208)
#define HWIO_GENI_TX_FIFO_THRESHOLD_OFFS                                              (0x00000208)
#define HWIO_GENI_TX_FIFO_THRESHOLD_RMSK                                                    0x1f
#define HWIO_GENI_TX_FIFO_THRESHOLD_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_FIFO_THRESHOLD_ADDR(x), HWIO_GENI_TX_FIFO_THRESHOLD_RMSK)
#define HWIO_GENI_TX_FIFO_THRESHOLD_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_FIFO_THRESHOLD_ADDR(x), m)
#define HWIO_GENI_TX_FIFO_THRESHOLD_OUT(x, v)      \
        out_dword(HWIO_GENI_TX_FIFO_THRESHOLD_ADDR(x),v)
#define HWIO_GENI_TX_FIFO_THRESHOLD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TX_FIFO_THRESHOLD_ADDR(x),m,v,HWIO_GENI_TX_FIFO_THRESHOLD_IN(x))
#define HWIO_GENI_TX_FIFO_THRESHOLD_TX_FIFO_THRESHOLD_BMSK                                  0x1f
#define HWIO_GENI_TX_FIFO_THRESHOLD_TX_FIFO_THRESHOLD_SHFT                                   0x0

#define HWIO_GENI_TX_WATERMARK_REG_ADDR(x)                                            ((x) + 0x0000020c)
#define HWIO_GENI_TX_WATERMARK_REG_OFFS                                               (0x0000020c)
#define HWIO_GENI_TX_WATERMARK_REG_RMSK                                                     0x3f
#define HWIO_GENI_TX_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_WATERMARK_REG_ADDR(x), HWIO_GENI_TX_WATERMARK_REG_RMSK)
#define HWIO_GENI_TX_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_TX_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_TX_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_TX_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TX_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_TX_WATERMARK_REG_IN(x))
#define HWIO_GENI_TX_WATERMARK_REG_TX_WATERMARK_BMSK                                        0x3f
#define HWIO_GENI_TX_WATERMARK_REG_TX_WATERMARK_SHFT                                         0x0

#define HWIO_GENI_RX_WATERMARK_REG_ADDR(x)                                            ((x) + 0x00000210)
#define HWIO_GENI_RX_WATERMARK_REG_OFFS                                               (0x00000210)
#define HWIO_GENI_RX_WATERMARK_REG_RMSK                                                     0x3f
#define HWIO_GENI_RX_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_WATERMARK_REG_ADDR(x), HWIO_GENI_RX_WATERMARK_REG_RMSK)
#define HWIO_GENI_RX_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_RX_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_RX_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_RX_WATERMARK_REG_IN(x))
#define HWIO_GENI_RX_WATERMARK_REG_RX_WATERMARK_BMSK                                        0x3f
#define HWIO_GENI_RX_WATERMARK_REG_RX_WATERMARK_SHFT                                         0x0

#define HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x)                                        ((x) + 0x00000214)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OFFS                                           (0x00000214)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RMSK                                                 0x3f
#define HWIO_GENI_RX_RFR_WATERMARK_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x), HWIO_GENI_RX_RFR_WATERMARK_REG_RMSK)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x), m)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x),v)
#define HWIO_GENI_RX_RFR_WATERMARK_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_RFR_WATERMARK_REG_ADDR(x),m,v,HWIO_GENI_RX_RFR_WATERMARK_REG_IN(x))
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_BMSK                                0x3f
#define HWIO_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_SHFT                                 0x0

#define HWIO_GENI_GP_OUTPUT_REG_ADDR(x)                                               ((x) + 0x00000300)
#define HWIO_GENI_GP_OUTPUT_REG_OFFS                                                  (0x00000300)
#define HWIO_GENI_GP_OUTPUT_REG_RMSK                                                        0x3f
#define HWIO_GENI_GP_OUTPUT_REG_IN(x)      \
        in_dword_masked(HWIO_GENI_GP_OUTPUT_REG_ADDR(x), HWIO_GENI_GP_OUTPUT_REG_RMSK)
#define HWIO_GENI_GP_OUTPUT_REG_INM(x, m)      \
        in_dword_masked(HWIO_GENI_GP_OUTPUT_REG_ADDR(x), m)
#define HWIO_GENI_GP_OUTPUT_REG_OUT(x, v)      \
        out_dword(HWIO_GENI_GP_OUTPUT_REG_ADDR(x),v)
#define HWIO_GENI_GP_OUTPUT_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_GP_OUTPUT_REG_ADDR(x),m,v,HWIO_GENI_GP_OUTPUT_REG_IN(x))
#define HWIO_GENI_GP_OUTPUT_REG_GP_OUTPUT_BMSK                                              0x3f
#define HWIO_GENI_GP_OUTPUT_REG_GP_OUTPUT_SHFT                                               0x0

#define HWIO_GENI_IOS_ADDR(x)                                                         ((x) + 0x00000308)
#define HWIO_GENI_IOS_OFFS                                                            (0x00000308)
#define HWIO_GENI_IOS_RMSK                                                                   0x3
#define HWIO_GENI_IOS_IN(x)      \
        in_dword_masked(HWIO_GENI_IOS_ADDR(x), HWIO_GENI_IOS_RMSK)
#define HWIO_GENI_IOS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_IOS_ADDR(x), m)
#define HWIO_GENI_IOS_IO2_DATA_IN_BMSK                                                       0x2
#define HWIO_GENI_IOS_IO2_DATA_IN_SHFT                                                       0x1
#define HWIO_GENI_IOS_RX_DATA_IN_BMSK                                                        0x1
#define HWIO_GENI_IOS_RX_DATA_IN_SHFT                                                        0x0

#define HWIO_GENI_TIMESTAMP_ADDR(x)                                                   ((x) + 0x0000030c)
#define HWIO_GENI_TIMESTAMP_OFFS                                                      (0x0000030c)
#define HWIO_GENI_TIMESTAMP_RMSK                                                      0xffffffff
#define HWIO_GENI_TIMESTAMP_IN(x)      \
        in_dword_masked(HWIO_GENI_TIMESTAMP_ADDR(x), HWIO_GENI_TIMESTAMP_RMSK)
#define HWIO_GENI_TIMESTAMP_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TIMESTAMP_ADDR(x), m)
#define HWIO_GENI_TIMESTAMP_TIMESTAMP_BMSK                                            0xffffffff
#define HWIO_GENI_TIMESTAMP_TIMESTAMP_SHFT                                                   0x0

#define HWIO_GENI_M_GP_LENGTH_ADDR(x)                                                 ((x) + 0x00000310)
#define HWIO_GENI_M_GP_LENGTH_OFFS                                                    (0x00000310)
#define HWIO_GENI_M_GP_LENGTH_RMSK                                                    0xffffffff
#define HWIO_GENI_M_GP_LENGTH_IN(x)      \
        in_dword_masked(HWIO_GENI_M_GP_LENGTH_ADDR(x), HWIO_GENI_M_GP_LENGTH_RMSK)
#define HWIO_GENI_M_GP_LENGTH_INM(x, m)      \
        in_dword_masked(HWIO_GENI_M_GP_LENGTH_ADDR(x), m)
#define HWIO_GENI_M_GP_LENGTH_M_GP_LENGTH_BMSK                                        0xffffffff
#define HWIO_GENI_M_GP_LENGTH_M_GP_LENGTH_SHFT                                               0x0

#define HWIO_GENI_S_GP_LENGTH_ADDR(x)                                                 ((x) + 0x00000314)
#define HWIO_GENI_S_GP_LENGTH_OFFS                                                    (0x00000314)
#define HWIO_GENI_S_GP_LENGTH_RMSK                                                    0xffffffff
#define HWIO_GENI_S_GP_LENGTH_IN(x)      \
        in_dword_masked(HWIO_GENI_S_GP_LENGTH_ADDR(x), HWIO_GENI_S_GP_LENGTH_RMSK)
#define HWIO_GENI_S_GP_LENGTH_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_GP_LENGTH_ADDR(x), m)
#define HWIO_GENI_S_GP_LENGTH_S_GP_LENGTH_BMSK                                        0xffffffff
#define HWIO_GENI_S_GP_LENGTH_S_GP_LENGTH_SHFT                                               0x0

#define HWIO_GENI_I3C_IBI_CFG_TABLEn_ADDR(base,n)                                     ((base) + 0x00000400 + 0x4 * (n))
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_OFFS(base,n)                                     (0x00000400 + 0x4 * (n))
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_RMSK                                              0x1ffffff
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_MAXn                                                     15
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_INI(base,n)        \
        in_dword_masked(HWIO_GENI_I3C_IBI_CFG_TABLEn_ADDR(base,n), HWIO_GENI_I3C_IBI_CFG_TABLEn_RMSK)
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_GENI_I3C_IBI_CFG_TABLEn_ADDR(base,n), mask)
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_SCL_LOW_CNT_BMSK                                  0x1fe0000
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_SCL_LOW_CNT_SHFT                                       0x11
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_OWNER_BMSK                                          0x1e000
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_OWNER_SHFT                                              0xd
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_NUM_BYTES_BMSK                                       0x1c00
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_NUM_BYTES_SHFT                                          0xa
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_ACK_NACK_BMSK                                         0x200
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_ACK_NACK_SHFT                                           0x9
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_RD_WR_BMSK                                            0x100
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_RD_WR_SHFT                                              0x8
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_DEVICE_ADDR_BMSK                                       0xfe
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_DEVICE_ADDR_SHFT                                        0x1
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_VALID_BMSK                                              0x1
#define HWIO_GENI_I3C_IBI_CFG_TABLEn_VALID_SHFT                                              0x0

#define HWIO_GENI_I3C_IBI_STATUS_ADDR(x)                                              ((x) + 0x00000480)
#define HWIO_GENI_I3C_IBI_STATUS_OFFS                                                 (0x00000480)
#define HWIO_GENI_I3C_IBI_STATUS_RMSK                                                 0x81ef71ff
#define HWIO_GENI_I3C_IBI_STATUS_IN(x)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_STATUS_ADDR(x), HWIO_GENI_I3C_IBI_STATUS_RMSK)
#define HWIO_GENI_I3C_IBI_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_STATUS_ADDR(x), m)
#define HWIO_GENI_I3C_IBI_STATUS_FOUND_BMSK                                           0x80000000
#define HWIO_GENI_I3C_IBI_STATUS_FOUND_SHFT                                                 0x1f
#define HWIO_GENI_I3C_IBI_STATUS_INDEX_BMSK                                            0x1e00000
#define HWIO_GENI_I3C_IBI_STATUS_INDEX_SHFT                                                 0x15
#define HWIO_GENI_I3C_IBI_STATUS_OWNER_BMSK                                              0xf0000
#define HWIO_GENI_I3C_IBI_STATUS_OWNER_SHFT                                                 0x10
#define HWIO_GENI_I3C_IBI_STATUS_NUM_BYTES_BMSK                                           0x7000
#define HWIO_GENI_I3C_IBI_STATUS_NUM_BYTES_SHFT                                              0xc
#define HWIO_GENI_I3C_IBI_STATUS_ACK_NACK_BMSK                                             0x100
#define HWIO_GENI_I3C_IBI_STATUS_ACK_NACK_SHFT                                               0x8
#define HWIO_GENI_I3C_IBI_STATUS_DEVICE_ADDR_BMSK                                           0xfe
#define HWIO_GENI_I3C_IBI_STATUS_DEVICE_ADDR_SHFT                                            0x1
#define HWIO_GENI_I3C_IBI_STATUS_RD_WR_BMSK                                                  0x1
#define HWIO_GENI_I3C_IBI_STATUS_RD_WR_SHFT                                                  0x0

#define HWIO_GENI_I3C_IBI_RD_DATA_ADDR(x)                                             ((x) + 0x00000484)
#define HWIO_GENI_I3C_IBI_RD_DATA_OFFS                                                (0x00000484)
#define HWIO_GENI_I3C_IBI_RD_DATA_RMSK                                                0xffffffff
#define HWIO_GENI_I3C_IBI_RD_DATA_IN(x)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_RD_DATA_ADDR(x), HWIO_GENI_I3C_IBI_RD_DATA_RMSK)
#define HWIO_GENI_I3C_IBI_RD_DATA_INM(x, m)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_RD_DATA_ADDR(x), m)
#define HWIO_GENI_I3C_IBI_RD_DATA_DATA_BMSK                                           0xffffffff
#define HWIO_GENI_I3C_IBI_RD_DATA_DATA_SHFT                                                  0x0

#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_ADDR(x)                                      ((x) + 0x00000488)
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_OFFS                                         (0x00000488)
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_RMSK                                               0xff
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_IN(x)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_SEARCH_PATTERN_ADDR(x), HWIO_GENI_I3C_IBI_SEARCH_PATTERN_RMSK)
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_INM(x, m)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_SEARCH_PATTERN_ADDR(x), m)
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_DEVICE_ADDR_BMSK                                   0xfe
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_DEVICE_ADDR_SHFT                                    0x1
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_RD_WR_BMSK                                          0x1
#define HWIO_GENI_I3C_IBI_SEARCH_PATTERN_RD_WR_SHFT                                          0x0

#define HWIO_GENI_I3C_IBI_SEARCH_DATA_ADDR(x)                                         ((x) + 0x0000048c)
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_OFFS                                            (0x0000048c)
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_RMSK                                            0x8000000f
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_IN(x)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_SEARCH_DATA_ADDR(x), HWIO_GENI_I3C_IBI_SEARCH_DATA_RMSK)
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_INM(x, m)      \
        in_dword_masked(HWIO_GENI_I3C_IBI_SEARCH_DATA_ADDR(x), m)
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_FOUND_BMSK                                      0x80000000
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_FOUND_SHFT                                            0x1f
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_INDEX_BMSK                                             0xf
#define HWIO_GENI_I3C_IBI_SEARCH_DATA_INDEX_SHFT                                             0x0

#define HWIO_GENI_I3C_SW_IBI_EN_ADDR(x)                                               ((x) + 0x00000490)
#define HWIO_GENI_I3C_SW_IBI_EN_OFFS                                                  (0x00000490)
#define HWIO_GENI_I3C_SW_IBI_EN_RMSK                                                        0x11
#define HWIO_GENI_I3C_SW_IBI_EN_IN(x)      \
        in_dword_masked(HWIO_GENI_I3C_SW_IBI_EN_ADDR(x), HWIO_GENI_I3C_SW_IBI_EN_RMSK)
#define HWIO_GENI_I3C_SW_IBI_EN_INM(x, m)      \
        in_dword_masked(HWIO_GENI_I3C_SW_IBI_EN_ADDR(x), m)
#define HWIO_GENI_I3C_SW_IBI_EN_IBI_EN_STATUS_BMSK                                          0x10
#define HWIO_GENI_I3C_SW_IBI_EN_IBI_EN_STATUS_SHFT                                           0x4
#define HWIO_GENI_I3C_SW_IBI_EN_IBI_EN_BMSK                                                  0x1
#define HWIO_GENI_I3C_SW_IBI_EN_IBI_EN_SHFT                                                  0x0

#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_ADDR(x)                                       ((x) + 0x00000494)
#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_OFFS                                          (0x00000494)
#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_RMSK                                                 0x1
#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_OUT(x, v)      \
        out_dword(HWIO_GENI_I3C_SW_IBI_EN_RECOVER_ADDR(x),v)
#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_IBI_EN_RECOVER_SET_BMSK                              0x1
#define HWIO_GENI_I3C_SW_IBI_EN_RECOVER_IBI_EN_RECOVER_SET_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: GENI4_IMAGE
 *--------------------------------------------------------------------------*/

#define GENI4_IMAGE_REG_BASE                                       (QUPV3_0_QUPV3_ID_1_BASE      + 0x00081000)
#define GENI4_IMAGE_REG_BASE_OFFS                                  0x00081000

#define HWIO_GENI_FW_REVISION_ADDR(x)                              ((x) + 0x00000000)
#define HWIO_GENI_FW_REVISION_OFFS                                 (0x00000000)
#define HWIO_GENI_FW_REVISION_RMSK                                     0xffff
#define HWIO_GENI_FW_REVISION_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_REVISION_ADDR(x), HWIO_GENI_FW_REVISION_RMSK)
#define HWIO_GENI_FW_REVISION_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_REVISION_ADDR(x), m)
#define HWIO_GENI_FW_REVISION_OUT(x, v)      \
        out_dword(HWIO_GENI_FW_REVISION_ADDR(x),v)
#define HWIO_GENI_FW_REVISION_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_FW_REVISION_ADDR(x),m,v,HWIO_GENI_FW_REVISION_IN(x))
#define HWIO_GENI_FW_REVISION_PROTOCOL_BMSK                            0xff00
#define HWIO_GENI_FW_REVISION_PROTOCOL_SHFT                               0x8
#define HWIO_GENI_FW_REVISION_VERSION_BMSK                               0xff
#define HWIO_GENI_FW_REVISION_VERSION_SHFT                                0x0

#define HWIO_GENI_S_FW_REVISION_ADDR(x)                            ((x) + 0x00000004)
#define HWIO_GENI_S_FW_REVISION_OFFS                               (0x00000004)
#define HWIO_GENI_S_FW_REVISION_RMSK                                   0xffff
#define HWIO_GENI_S_FW_REVISION_IN(x)      \
        in_dword_masked(HWIO_GENI_S_FW_REVISION_ADDR(x), HWIO_GENI_S_FW_REVISION_RMSK)
#define HWIO_GENI_S_FW_REVISION_INM(x, m)      \
        in_dword_masked(HWIO_GENI_S_FW_REVISION_ADDR(x), m)
#define HWIO_GENI_S_FW_REVISION_OUT(x, v)      \
        out_dword(HWIO_GENI_S_FW_REVISION_ADDR(x),v)
#define HWIO_GENI_S_FW_REVISION_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_S_FW_REVISION_ADDR(x),m,v,HWIO_GENI_S_FW_REVISION_IN(x))
#define HWIO_GENI_S_FW_REVISION_PROTOCOL_BMSK                          0xff00
#define HWIO_GENI_S_FW_REVISION_PROTOCOL_SHFT                             0x8
#define HWIO_GENI_S_FW_REVISION_VERSION_BMSK                             0xff
#define HWIO_GENI_S_FW_REVISION_VERSION_SHFT                              0x0

#define HWIO_GENI_CFG_RAMn_ADDR(base,n)                            ((base) + 0x00000010 + 0x4 * (n))
#define HWIO_GENI_CFG_RAMn_OFFS(base,n)                            (0x00000010 + 0x4 * (n))
#define HWIO_GENI_CFG_RAMn_RMSK                                      0x7fffff
#define HWIO_GENI_CFG_RAMn_MAXn                                           303
#define HWIO_GENI_CFG_RAMn_INI(base,n)        \
        in_dword_masked(HWIO_GENI_CFG_RAMn_ADDR(base,n), HWIO_GENI_CFG_RAMn_RMSK)
#define HWIO_GENI_CFG_RAMn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_GENI_CFG_RAMn_ADDR(base,n), mask)
#define HWIO_GENI_CFG_RAMn_OUTI(base,n,val)    \
        out_dword(HWIO_GENI_CFG_RAMn_ADDR(base,n),val)
#define HWIO_GENI_CFG_RAMn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_GENI_CFG_RAMn_ADDR(base,n),mask,val,HWIO_GENI_CFG_RAMn_INI(base,n))
#define HWIO_GENI_CFG_RAMn_TBD_BMSK                                  0x7fffff
#define HWIO_GENI_CFG_RAMn_TBD_SHFT                                       0x0

/*----------------------------------------------------------------------------
 * MODULE: GENI4_IMAGE_REGS
 *--------------------------------------------------------------------------*/

#define GENI4_IMAGE_REGS_REG_BASE                                                           (QUPV3_0_QUPV3_ID_1_BASE      + 0x00080100)
#define GENI4_IMAGE_REGS_REG_BASE_OFFS                                                      0x00080100

#define HWIO_GENI_CFG_REG0_ADDR(x)                                                          ((x) + 0x00000000)
#define HWIO_GENI_CFG_REG0_OFFS                                                             (0x00000000)
#define HWIO_GENI_CFG_REG0_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG0_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG0_ADDR(x), HWIO_GENI_CFG_REG0_RMSK)
#define HWIO_GENI_CFG_REG0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG0_ADDR(x), m)
#define HWIO_GENI_CFG_REG0_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG0_ADDR(x),v)
#define HWIO_GENI_CFG_REG0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG0_ADDR(x),m,v,HWIO_GENI_CFG_REG0_IN(x))
#define HWIO_GENI_CFG_REG0_PRIM_11_TX_DURATION_BMSK                                           0xc00000
#define HWIO_GENI_CFG_REG0_PRIM_11_TX_DURATION_SHFT                                               0x16
#define HWIO_GENI_CFG_REG0_PRIM_10_TX_DURATION_BMSK                                           0x300000
#define HWIO_GENI_CFG_REG0_PRIM_10_TX_DURATION_SHFT                                               0x14
#define HWIO_GENI_CFG_REG0_PRIM_9_TX_DURATION_BMSK                                             0xc0000
#define HWIO_GENI_CFG_REG0_PRIM_9_TX_DURATION_SHFT                                                0x12
#define HWIO_GENI_CFG_REG0_PRIM_8_TX_DURATION_BMSK                                             0x30000
#define HWIO_GENI_CFG_REG0_PRIM_8_TX_DURATION_SHFT                                                0x10
#define HWIO_GENI_CFG_REG0_PRIM_7_TX_DURATION_BMSK                                              0xc000
#define HWIO_GENI_CFG_REG0_PRIM_7_TX_DURATION_SHFT                                                 0xe
#define HWIO_GENI_CFG_REG0_PRIM_6_TX_DURATION_BMSK                                              0x3000
#define HWIO_GENI_CFG_REG0_PRIM_6_TX_DURATION_SHFT                                                 0xc
#define HWIO_GENI_CFG_REG0_PRIM_5_TX_DURATION_BMSK                                               0xc00
#define HWIO_GENI_CFG_REG0_PRIM_5_TX_DURATION_SHFT                                                 0xa
#define HWIO_GENI_CFG_REG0_PRIM_4_TX_DURATION_BMSK                                               0x300
#define HWIO_GENI_CFG_REG0_PRIM_4_TX_DURATION_SHFT                                                 0x8
#define HWIO_GENI_CFG_REG0_PRIM_3_TX_DURATION_BMSK                                                0xc0
#define HWIO_GENI_CFG_REG0_PRIM_3_TX_DURATION_SHFT                                                 0x6
#define HWIO_GENI_CFG_REG0_PRIM_2_TX_DURATION_BMSK                                                0x30
#define HWIO_GENI_CFG_REG0_PRIM_2_TX_DURATION_SHFT                                                 0x4
#define HWIO_GENI_CFG_REG0_PRIM_1_TX_DURATION_BMSK                                                 0xc
#define HWIO_GENI_CFG_REG0_PRIM_1_TX_DURATION_SHFT                                                 0x2
#define HWIO_GENI_CFG_REG0_PRIM_0_TX_DURATION_BMSK                                                 0x3
#define HWIO_GENI_CFG_REG0_PRIM_0_TX_DURATION_SHFT                                                 0x0

#define HWIO_GENI_CFG_REG1_ADDR(x)                                                          ((x) + 0x00000004)
#define HWIO_GENI_CFG_REG1_OFFS                                                             (0x00000004)
#define HWIO_GENI_CFG_REG1_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG1_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG1_ADDR(x), HWIO_GENI_CFG_REG1_RMSK)
#define HWIO_GENI_CFG_REG1_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG1_ADDR(x), m)
#define HWIO_GENI_CFG_REG1_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG1_ADDR(x),v)
#define HWIO_GENI_CFG_REG1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG1_ADDR(x),m,v,HWIO_GENI_CFG_REG1_IN(x))
#define HWIO_GENI_CFG_REG1_PRIM_23_TX_DURATION_BMSK                                           0xc00000
#define HWIO_GENI_CFG_REG1_PRIM_23_TX_DURATION_SHFT                                               0x16
#define HWIO_GENI_CFG_REG1_PRIM_22_TX_DURATION_BMSK                                           0x300000
#define HWIO_GENI_CFG_REG1_PRIM_22_TX_DURATION_SHFT                                               0x14
#define HWIO_GENI_CFG_REG1_PRIM_21_TX_DURATION_BMSK                                            0xc0000
#define HWIO_GENI_CFG_REG1_PRIM_21_TX_DURATION_SHFT                                               0x12
#define HWIO_GENI_CFG_REG1_PRIM_20_TX_DURATION_BMSK                                            0x30000
#define HWIO_GENI_CFG_REG1_PRIM_20_TX_DURATION_SHFT                                               0x10
#define HWIO_GENI_CFG_REG1_PRIM_19_TX_DURATION_BMSK                                             0xc000
#define HWIO_GENI_CFG_REG1_PRIM_19_TX_DURATION_SHFT                                                0xe
#define HWIO_GENI_CFG_REG1_PRIM_18_TX_DURATION_BMSK                                             0x3000
#define HWIO_GENI_CFG_REG1_PRIM_18_TX_DURATION_SHFT                                                0xc
#define HWIO_GENI_CFG_REG1_PRIM_17_TX_DURATION_BMSK                                              0xc00
#define HWIO_GENI_CFG_REG1_PRIM_17_TX_DURATION_SHFT                                                0xa
#define HWIO_GENI_CFG_REG1_PRIM_16_TX_DURATION_BMSK                                              0x300
#define HWIO_GENI_CFG_REG1_PRIM_16_TX_DURATION_SHFT                                                0x8
#define HWIO_GENI_CFG_REG1_PRIM_15_TX_DURATION_BMSK                                               0xc0
#define HWIO_GENI_CFG_REG1_PRIM_15_TX_DURATION_SHFT                                                0x6
#define HWIO_GENI_CFG_REG1_PRIM_14_TX_DURATION_BMSK                                               0x30
#define HWIO_GENI_CFG_REG1_PRIM_14_TX_DURATION_SHFT                                                0x4
#define HWIO_GENI_CFG_REG1_PRIM_13_TX_DURATION_BMSK                                                0xc
#define HWIO_GENI_CFG_REG1_PRIM_13_TX_DURATION_SHFT                                                0x2
#define HWIO_GENI_CFG_REG1_PRIM_12_TX_DURATION_BMSK                                                0x3
#define HWIO_GENI_CFG_REG1_PRIM_12_TX_DURATION_SHFT                                                0x0

#define HWIO_GENI_CFG_REG2_ADDR(x)                                                          ((x) + 0x00000008)
#define HWIO_GENI_CFG_REG2_OFFS                                                             (0x00000008)
#define HWIO_GENI_CFG_REG2_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG2_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG2_ADDR(x), HWIO_GENI_CFG_REG2_RMSK)
#define HWIO_GENI_CFG_REG2_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG2_ADDR(x), m)
#define HWIO_GENI_CFG_REG2_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG2_ADDR(x),v)
#define HWIO_GENI_CFG_REG2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG2_ADDR(x),m,v,HWIO_GENI_CFG_REG2_IN(x))
#define HWIO_GENI_CFG_REG2_PRIM_11_RX_DURATION_BMSK                                           0xc00000
#define HWIO_GENI_CFG_REG2_PRIM_11_RX_DURATION_SHFT                                               0x16
#define HWIO_GENI_CFG_REG2_PRIM_10_RX_DURATION_BMSK                                           0x300000
#define HWIO_GENI_CFG_REG2_PRIM_10_RX_DURATION_SHFT                                               0x14
#define HWIO_GENI_CFG_REG2_PRIM_9_RX_DURATION_BMSK                                             0xc0000
#define HWIO_GENI_CFG_REG2_PRIM_9_RX_DURATION_SHFT                                                0x12
#define HWIO_GENI_CFG_REG2_PRIM_8_RX_DURATION_BMSK                                             0x30000
#define HWIO_GENI_CFG_REG2_PRIM_8_RX_DURATION_SHFT                                                0x10
#define HWIO_GENI_CFG_REG2_PRIM_7_RX_DURATION_BMSK                                              0xc000
#define HWIO_GENI_CFG_REG2_PRIM_7_RX_DURATION_SHFT                                                 0xe
#define HWIO_GENI_CFG_REG2_PRIM_6_RX_DURATION_BMSK                                              0x3000
#define HWIO_GENI_CFG_REG2_PRIM_6_RX_DURATION_SHFT                                                 0xc
#define HWIO_GENI_CFG_REG2_PRIM_5_RX_DURATION_BMSK                                               0xc00
#define HWIO_GENI_CFG_REG2_PRIM_5_RX_DURATION_SHFT                                                 0xa
#define HWIO_GENI_CFG_REG2_PRIM_4_RX_DURATION_BMSK                                               0x300
#define HWIO_GENI_CFG_REG2_PRIM_4_RX_DURATION_SHFT                                                 0x8
#define HWIO_GENI_CFG_REG2_PRIM_3_RX_DURATION_BMSK                                                0xc0
#define HWIO_GENI_CFG_REG2_PRIM_3_RX_DURATION_SHFT                                                 0x6
#define HWIO_GENI_CFG_REG2_PRIM_2_RX_DURATION_BMSK                                                0x30
#define HWIO_GENI_CFG_REG2_PRIM_2_RX_DURATION_SHFT                                                 0x4
#define HWIO_GENI_CFG_REG2_PRIM_1_RX_DURATION_BMSK                                                 0xc
#define HWIO_GENI_CFG_REG2_PRIM_1_RX_DURATION_SHFT                                                 0x2
#define HWIO_GENI_CFG_REG2_PRIM_0_RX_DURATION_BMSK                                                 0x3
#define HWIO_GENI_CFG_REG2_PRIM_0_RX_DURATION_SHFT                                                 0x0

#define HWIO_GENI_CFG_REG3_ADDR(x)                                                          ((x) + 0x0000000c)
#define HWIO_GENI_CFG_REG3_OFFS                                                             (0x0000000c)
#define HWIO_GENI_CFG_REG3_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG3_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG3_ADDR(x), HWIO_GENI_CFG_REG3_RMSK)
#define HWIO_GENI_CFG_REG3_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG3_ADDR(x), m)
#define HWIO_GENI_CFG_REG3_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG3_ADDR(x),v)
#define HWIO_GENI_CFG_REG3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG3_ADDR(x),m,v,HWIO_GENI_CFG_REG3_IN(x))
#define HWIO_GENI_CFG_REG3_PRIM_23_RX_DURATION_BMSK                                           0xc00000
#define HWIO_GENI_CFG_REG3_PRIM_23_RX_DURATION_SHFT                                               0x16
#define HWIO_GENI_CFG_REG3_PRIM_22_RX_DURATION_BMSK                                           0x300000
#define HWIO_GENI_CFG_REG3_PRIM_22_RX_DURATION_SHFT                                               0x14
#define HWIO_GENI_CFG_REG3_PRIM_21_RX_DURATION_BMSK                                            0xc0000
#define HWIO_GENI_CFG_REG3_PRIM_21_RX_DURATION_SHFT                                               0x12
#define HWIO_GENI_CFG_REG3_PRIM_20_RX_DURATION_BMSK                                            0x30000
#define HWIO_GENI_CFG_REG3_PRIM_20_RX_DURATION_SHFT                                               0x10
#define HWIO_GENI_CFG_REG3_PRIM_19_RX_DURATION_BMSK                                             0xc000
#define HWIO_GENI_CFG_REG3_PRIM_19_RX_DURATION_SHFT                                                0xe
#define HWIO_GENI_CFG_REG3_PRIM_18_RX_DURATION_BMSK                                             0x3000
#define HWIO_GENI_CFG_REG3_PRIM_18_RX_DURATION_SHFT                                                0xc
#define HWIO_GENI_CFG_REG3_PRIM_17_RX_DURATION_BMSK                                              0xc00
#define HWIO_GENI_CFG_REG3_PRIM_17_RX_DURATION_SHFT                                                0xa
#define HWIO_GENI_CFG_REG3_PRIM_16_RX_DURATION_BMSK                                              0x300
#define HWIO_GENI_CFG_REG3_PRIM_16_RX_DURATION_SHFT                                                0x8
#define HWIO_GENI_CFG_REG3_PRIM_15_RX_DURATION_BMSK                                               0xc0
#define HWIO_GENI_CFG_REG3_PRIM_15_RX_DURATION_SHFT                                                0x6
#define HWIO_GENI_CFG_REG3_PRIM_14_RX_DURATION_BMSK                                               0x30
#define HWIO_GENI_CFG_REG3_PRIM_14_RX_DURATION_SHFT                                                0x4
#define HWIO_GENI_CFG_REG3_PRIM_13_RX_DURATION_BMSK                                                0xc
#define HWIO_GENI_CFG_REG3_PRIM_13_RX_DURATION_SHFT                                                0x2
#define HWIO_GENI_CFG_REG3_PRIM_12_RX_DURATION_BMSK                                                0x3
#define HWIO_GENI_CFG_REG3_PRIM_12_RX_DURATION_SHFT                                                0x0

#define HWIO_GENI_CFG_REG4_ADDR(x)                                                          ((x) + 0x00000010)
#define HWIO_GENI_CFG_REG4_OFFS                                                             (0x00000010)
#define HWIO_GENI_CFG_REG4_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG4_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG4_ADDR(x), HWIO_GENI_CFG_REG4_RMSK)
#define HWIO_GENI_CFG_REG4_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG4_ADDR(x), m)
#define HWIO_GENI_CFG_REG4_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG4_ADDR(x),v)
#define HWIO_GENI_CFG_REG4_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG4_ADDR(x),m,v,HWIO_GENI_CFG_REG4_IN(x))
#define HWIO_GENI_CFG_REG4_PRIM_7_TX_FUNC_SEL_BMSK                                            0xe00000
#define HWIO_GENI_CFG_REG4_PRIM_7_TX_FUNC_SEL_SHFT                                                0x15
#define HWIO_GENI_CFG_REG4_PRIM_6_TX_FUNC_SEL_BMSK                                            0x1c0000
#define HWIO_GENI_CFG_REG4_PRIM_6_TX_FUNC_SEL_SHFT                                                0x12
#define HWIO_GENI_CFG_REG4_PRIM_5_TX_FUNC_SEL_BMSK                                             0x38000
#define HWIO_GENI_CFG_REG4_PRIM_5_TX_FUNC_SEL_SHFT                                                 0xf
#define HWIO_GENI_CFG_REG4_PRIM_4_TX_FUNC_SEL_BMSK                                              0x7000
#define HWIO_GENI_CFG_REG4_PRIM_4_TX_FUNC_SEL_SHFT                                                 0xc
#define HWIO_GENI_CFG_REG4_PRIM_3_TX_FUNC_SEL_BMSK                                               0xe00
#define HWIO_GENI_CFG_REG4_PRIM_3_TX_FUNC_SEL_SHFT                                                 0x9
#define HWIO_GENI_CFG_REG4_PRIM_2_TX_FUNC_SEL_BMSK                                               0x1c0
#define HWIO_GENI_CFG_REG4_PRIM_2_TX_FUNC_SEL_SHFT                                                 0x6
#define HWIO_GENI_CFG_REG4_PRIM_1_TX_FUNC_SEL_BMSK                                                0x38
#define HWIO_GENI_CFG_REG4_PRIM_1_TX_FUNC_SEL_SHFT                                                 0x3
#define HWIO_GENI_CFG_REG4_PRIM_0_TX_FUNC_SEL_BMSK                                                 0x7
#define HWIO_GENI_CFG_REG4_PRIM_0_TX_FUNC_SEL_SHFT                                                 0x0

#define HWIO_GENI_CFG_REG5_ADDR(x)                                                          ((x) + 0x00000014)
#define HWIO_GENI_CFG_REG5_OFFS                                                             (0x00000014)
#define HWIO_GENI_CFG_REG5_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG5_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG5_ADDR(x), HWIO_GENI_CFG_REG5_RMSK)
#define HWIO_GENI_CFG_REG5_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG5_ADDR(x), m)
#define HWIO_GENI_CFG_REG5_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG5_ADDR(x),v)
#define HWIO_GENI_CFG_REG5_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG5_ADDR(x),m,v,HWIO_GENI_CFG_REG5_IN(x))
#define HWIO_GENI_CFG_REG5_PRIM_15_TX_FUNC_SEL_BMSK                                           0xe00000
#define HWIO_GENI_CFG_REG5_PRIM_15_TX_FUNC_SEL_SHFT                                               0x15
#define HWIO_GENI_CFG_REG5_PRIM_14_TX_FUNC_SEL_BMSK                                           0x1c0000
#define HWIO_GENI_CFG_REG5_PRIM_14_TX_FUNC_SEL_SHFT                                               0x12
#define HWIO_GENI_CFG_REG5_PRIM_13_TX_FUNC_SEL_BMSK                                            0x38000
#define HWIO_GENI_CFG_REG5_PRIM_13_TX_FUNC_SEL_SHFT                                                0xf
#define HWIO_GENI_CFG_REG5_PRIM_12_TX_FUNC_SEL_BMSK                                             0x7000
#define HWIO_GENI_CFG_REG5_PRIM_12_TX_FUNC_SEL_SHFT                                                0xc
#define HWIO_GENI_CFG_REG5_PRIM_11_TX_FUNC_SEL_BMSK                                              0xe00
#define HWIO_GENI_CFG_REG5_PRIM_11_TX_FUNC_SEL_SHFT                                                0x9
#define HWIO_GENI_CFG_REG5_PRIM_10_TX_FUNC_SEL_BMSK                                              0x1c0
#define HWIO_GENI_CFG_REG5_PRIM_10_TX_FUNC_SEL_SHFT                                                0x6
#define HWIO_GENI_CFG_REG5_PRIM_9_TX_FUNC_SEL_BMSK                                                0x38
#define HWIO_GENI_CFG_REG5_PRIM_9_TX_FUNC_SEL_SHFT                                                 0x3
#define HWIO_GENI_CFG_REG5_PRIM_8_TX_FUNC_SEL_BMSK                                                 0x7
#define HWIO_GENI_CFG_REG5_PRIM_8_TX_FUNC_SEL_SHFT                                                 0x0

#define HWIO_GENI_CFG_REG6_ADDR(x)                                                          ((x) + 0x00000018)
#define HWIO_GENI_CFG_REG6_OFFS                                                             (0x00000018)
#define HWIO_GENI_CFG_REG6_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG6_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG6_ADDR(x), HWIO_GENI_CFG_REG6_RMSK)
#define HWIO_GENI_CFG_REG6_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG6_ADDR(x), m)
#define HWIO_GENI_CFG_REG6_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG6_ADDR(x),v)
#define HWIO_GENI_CFG_REG6_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG6_ADDR(x),m,v,HWIO_GENI_CFG_REG6_IN(x))
#define HWIO_GENI_CFG_REG6_PRIM_23_TX_FUNC_SEL_BMSK                                           0xe00000
#define HWIO_GENI_CFG_REG6_PRIM_23_TX_FUNC_SEL_SHFT                                               0x15
#define HWIO_GENI_CFG_REG6_PRIM_22_TX_FUNC_SEL_BMSK                                           0x1c0000
#define HWIO_GENI_CFG_REG6_PRIM_22_TX_FUNC_SEL_SHFT                                               0x12
#define HWIO_GENI_CFG_REG6_PRIM_21_TX_FUNC_SEL_BMSK                                            0x38000
#define HWIO_GENI_CFG_REG6_PRIM_21_TX_FUNC_SEL_SHFT                                                0xf
#define HWIO_GENI_CFG_REG6_PRIM_20_TX_FUNC_SEL_BMSK                                             0x7000
#define HWIO_GENI_CFG_REG6_PRIM_20_TX_FUNC_SEL_SHFT                                                0xc
#define HWIO_GENI_CFG_REG6_PRIM_19_TX_FUNC_SEL_BMSK                                              0xe00
#define HWIO_GENI_CFG_REG6_PRIM_19_TX_FUNC_SEL_SHFT                                                0x9
#define HWIO_GENI_CFG_REG6_PRIM_18_TX_FUNC_SEL_BMSK                                              0x1c0
#define HWIO_GENI_CFG_REG6_PRIM_18_TX_FUNC_SEL_SHFT                                                0x6
#define HWIO_GENI_CFG_REG6_PRIM_17_TX_FUNC_SEL_BMSK                                               0x38
#define HWIO_GENI_CFG_REG6_PRIM_17_TX_FUNC_SEL_SHFT                                                0x3
#define HWIO_GENI_CFG_REG6_PRIM_16_TX_FUNC_SEL_BMSK                                                0x7
#define HWIO_GENI_CFG_REG6_PRIM_16_TX_FUNC_SEL_SHFT                                                0x0

#define HWIO_GENI_CFG_REG7_ADDR(x)                                                          ((x) + 0x0000001c)
#define HWIO_GENI_CFG_REG7_OFFS                                                             (0x0000001c)
#define HWIO_GENI_CFG_REG7_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG7_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG7_ADDR(x), HWIO_GENI_CFG_REG7_RMSK)
#define HWIO_GENI_CFG_REG7_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG7_ADDR(x), m)
#define HWIO_GENI_CFG_REG7_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG7_ADDR(x),v)
#define HWIO_GENI_CFG_REG7_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG7_ADDR(x),m,v,HWIO_GENI_CFG_REG7_IN(x))
#define HWIO_GENI_CFG_REG7_PRIM_23_TX_PAR_FIRST_BMSK                                          0x800000
#define HWIO_GENI_CFG_REG7_PRIM_23_TX_PAR_FIRST_SHFT                                              0x17
#define HWIO_GENI_CFG_REG7_PRIM_22_TX_PAR_FIRST_BMSK                                          0x400000
#define HWIO_GENI_CFG_REG7_PRIM_22_TX_PAR_FIRST_SHFT                                              0x16
#define HWIO_GENI_CFG_REG7_PRIM_21_TX_PAR_FIRST_BMSK                                          0x200000
#define HWIO_GENI_CFG_REG7_PRIM_21_TX_PAR_FIRST_SHFT                                              0x15
#define HWIO_GENI_CFG_REG7_PRIM_20_TX_PAR_FIRST_BMSK                                          0x100000
#define HWIO_GENI_CFG_REG7_PRIM_20_TX_PAR_FIRST_SHFT                                              0x14
#define HWIO_GENI_CFG_REG7_PRIM_19_TX_PAR_FIRST_BMSK                                           0x80000
#define HWIO_GENI_CFG_REG7_PRIM_19_TX_PAR_FIRST_SHFT                                              0x13
#define HWIO_GENI_CFG_REG7_PRIM_18_TX_PAR_FIRST_BMSK                                           0x40000
#define HWIO_GENI_CFG_REG7_PRIM_18_TX_PAR_FIRST_SHFT                                              0x12
#define HWIO_GENI_CFG_REG7_PRIM_17_TX_PAR_FIRST_BMSK                                           0x20000
#define HWIO_GENI_CFG_REG7_PRIM_17_TX_PAR_FIRST_SHFT                                              0x11
#define HWIO_GENI_CFG_REG7_PRIM_16_TX_PAR_FIRST_BMSK                                           0x10000
#define HWIO_GENI_CFG_REG7_PRIM_16_TX_PAR_FIRST_SHFT                                              0x10
#define HWIO_GENI_CFG_REG7_PRIM_15_TX_PAR_FIRST_BMSK                                            0x8000
#define HWIO_GENI_CFG_REG7_PRIM_15_TX_PAR_FIRST_SHFT                                               0xf
#define HWIO_GENI_CFG_REG7_PRIM_14_TX_PAR_FIRST_BMSK                                            0x4000
#define HWIO_GENI_CFG_REG7_PRIM_14_TX_PAR_FIRST_SHFT                                               0xe
#define HWIO_GENI_CFG_REG7_PRIM_13_TX_PAR_FIRST_BMSK                                            0x2000
#define HWIO_GENI_CFG_REG7_PRIM_13_TX_PAR_FIRST_SHFT                                               0xd
#define HWIO_GENI_CFG_REG7_PRIM_12_TX_PAR_FIRST_BMSK                                            0x1000
#define HWIO_GENI_CFG_REG7_PRIM_12_TX_PAR_FIRST_SHFT                                               0xc
#define HWIO_GENI_CFG_REG7_PRIM_11_TX_PAR_FIRST_BMSK                                             0x800
#define HWIO_GENI_CFG_REG7_PRIM_11_TX_PAR_FIRST_SHFT                                               0xb
#define HWIO_GENI_CFG_REG7_PRIM_10_TX_PAR_FIRST_BMSK                                             0x400
#define HWIO_GENI_CFG_REG7_PRIM_10_TX_PAR_FIRST_SHFT                                               0xa
#define HWIO_GENI_CFG_REG7_PRIM_9_TX_PAR_FIRST_BMSK                                              0x200
#define HWIO_GENI_CFG_REG7_PRIM_9_TX_PAR_FIRST_SHFT                                                0x9
#define HWIO_GENI_CFG_REG7_PRIM_8_TX_PAR_FIRST_BMSK                                              0x100
#define HWIO_GENI_CFG_REG7_PRIM_8_TX_PAR_FIRST_SHFT                                                0x8
#define HWIO_GENI_CFG_REG7_PRIM_7_TX_PAR_FIRST_BMSK                                               0x80
#define HWIO_GENI_CFG_REG7_PRIM_7_TX_PAR_FIRST_SHFT                                                0x7
#define HWIO_GENI_CFG_REG7_PRIM_6_TX_PAR_FIRST_BMSK                                               0x40
#define HWIO_GENI_CFG_REG7_PRIM_6_TX_PAR_FIRST_SHFT                                                0x6
#define HWIO_GENI_CFG_REG7_PRIM_5_TX_PAR_FIRST_BMSK                                               0x20
#define HWIO_GENI_CFG_REG7_PRIM_5_TX_PAR_FIRST_SHFT                                                0x5
#define HWIO_GENI_CFG_REG7_PRIM_4_TX_PAR_FIRST_BMSK                                               0x10
#define HWIO_GENI_CFG_REG7_PRIM_4_TX_PAR_FIRST_SHFT                                                0x4
#define HWIO_GENI_CFG_REG7_PRIM_3_TX_PAR_FIRST_BMSK                                                0x8
#define HWIO_GENI_CFG_REG7_PRIM_3_TX_PAR_FIRST_SHFT                                                0x3
#define HWIO_GENI_CFG_REG7_PRIM_2_TX_PAR_FIRST_BMSK                                                0x4
#define HWIO_GENI_CFG_REG7_PRIM_2_TX_PAR_FIRST_SHFT                                                0x2
#define HWIO_GENI_CFG_REG7_PRIM_1_TX_PAR_FIRST_BMSK                                                0x2
#define HWIO_GENI_CFG_REG7_PRIM_1_TX_PAR_FIRST_SHFT                                                0x1
#define HWIO_GENI_CFG_REG7_PRIM_0_TX_PAR_FIRST_BMSK                                                0x1
#define HWIO_GENI_CFG_REG7_PRIM_0_TX_PAR_FIRST_SHFT                                                0x0

#define HWIO_GENI_CFG_REG8_ADDR(x)                                                          ((x) + 0x00000020)
#define HWIO_GENI_CFG_REG8_OFFS                                                             (0x00000020)
#define HWIO_GENI_CFG_REG8_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG8_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG8_ADDR(x), HWIO_GENI_CFG_REG8_RMSK)
#define HWIO_GENI_CFG_REG8_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG8_ADDR(x), m)
#define HWIO_GENI_CFG_REG8_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG8_ADDR(x),v)
#define HWIO_GENI_CFG_REG8_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG8_ADDR(x),m,v,HWIO_GENI_CFG_REG8_IN(x))

#define HWIO_GENI_CFG_REG9_ADDR(x)                                                          ((x) + 0x00000024)
#define HWIO_GENI_CFG_REG9_OFFS                                                             (0x00000024)
#define HWIO_GENI_CFG_REG9_RMSK                                                               0xffffff
#define HWIO_GENI_CFG_REG9_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG9_ADDR(x), HWIO_GENI_CFG_REG9_RMSK)
#define HWIO_GENI_CFG_REG9_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG9_ADDR(x), m)
#define HWIO_GENI_CFG_REG9_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG9_ADDR(x),v)
#define HWIO_GENI_CFG_REG9_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG9_ADDR(x),m,v,HWIO_GENI_CFG_REG9_IN(x))

#define HWIO_GENI_CFG_REG10_ADDR(x)                                                         ((x) + 0x00000028)
#define HWIO_GENI_CFG_REG10_OFFS                                                            (0x00000028)
#define HWIO_GENI_CFG_REG10_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG10_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG10_ADDR(x), HWIO_GENI_CFG_REG10_RMSK)
#define HWIO_GENI_CFG_REG10_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG10_ADDR(x), m)
#define HWIO_GENI_CFG_REG10_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG10_ADDR(x),v)
#define HWIO_GENI_CFG_REG10_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG10_ADDR(x),m,v,HWIO_GENI_CFG_REG10_IN(x))
#define HWIO_GENI_CFG_REG10_PRIM_23_RX_SI_EN_BMSK                                             0x800000
#define HWIO_GENI_CFG_REG10_PRIM_23_RX_SI_EN_SHFT                                                 0x17
#define HWIO_GENI_CFG_REG10_PRIM_22_RX_SI_EN_BMSK                                             0x400000
#define HWIO_GENI_CFG_REG10_PRIM_22_RX_SI_EN_SHFT                                                 0x16
#define HWIO_GENI_CFG_REG10_PRIM_21_RX_SI_EN_BMSK                                             0x200000
#define HWIO_GENI_CFG_REG10_PRIM_21_RX_SI_EN_SHFT                                                 0x15
#define HWIO_GENI_CFG_REG10_PRIM_20_RX_SI_EN_BMSK                                             0x100000
#define HWIO_GENI_CFG_REG10_PRIM_20_RX_SI_EN_SHFT                                                 0x14
#define HWIO_GENI_CFG_REG10_PRIM_19_RX_SI_EN_BMSK                                              0x80000
#define HWIO_GENI_CFG_REG10_PRIM_19_RX_SI_EN_SHFT                                                 0x13
#define HWIO_GENI_CFG_REG10_PRIM_18_RX_SI_EN_BMSK                                              0x40000
#define HWIO_GENI_CFG_REG10_PRIM_18_RX_SI_EN_SHFT                                                 0x12
#define HWIO_GENI_CFG_REG10_PRIM_17_RX_SI_EN_BMSK                                              0x20000
#define HWIO_GENI_CFG_REG10_PRIM_17_RX_SI_EN_SHFT                                                 0x11
#define HWIO_GENI_CFG_REG10_PRIM_16_RX_SI_EN_BMSK                                              0x10000
#define HWIO_GENI_CFG_REG10_PRIM_16_RX_SI_EN_SHFT                                                 0x10
#define HWIO_GENI_CFG_REG10_PRIM_15_RX_SI_EN_BMSK                                               0x8000
#define HWIO_GENI_CFG_REG10_PRIM_15_RX_SI_EN_SHFT                                                  0xf
#define HWIO_GENI_CFG_REG10_PRIM_14_RX_SI_EN_BMSK                                               0x4000
#define HWIO_GENI_CFG_REG10_PRIM_14_RX_SI_EN_SHFT                                                  0xe
#define HWIO_GENI_CFG_REG10_PRIM_13_RX_SI_EN_BMSK                                               0x2000
#define HWIO_GENI_CFG_REG10_PRIM_13_RX_SI_EN_SHFT                                                  0xd
#define HWIO_GENI_CFG_REG10_PRIM_12_RX_SI_EN_BMSK                                               0x1000
#define HWIO_GENI_CFG_REG10_PRIM_12_RX_SI_EN_SHFT                                                  0xc
#define HWIO_GENI_CFG_REG10_PRIM_11_RX_SI_EN_BMSK                                                0x800
#define HWIO_GENI_CFG_REG10_PRIM_11_RX_SI_EN_SHFT                                                  0xb
#define HWIO_GENI_CFG_REG10_PRIM_10_RX_SI_EN_BMSK                                                0x400
#define HWIO_GENI_CFG_REG10_PRIM_10_RX_SI_EN_SHFT                                                  0xa
#define HWIO_GENI_CFG_REG10_PRIM_9_RX_SI_EN_BMSK                                                 0x200
#define HWIO_GENI_CFG_REG10_PRIM_9_RX_SI_EN_SHFT                                                   0x9
#define HWIO_GENI_CFG_REG10_PRIM_8_RX_SI_EN_BMSK                                                 0x100
#define HWIO_GENI_CFG_REG10_PRIM_8_RX_SI_EN_SHFT                                                   0x8
#define HWIO_GENI_CFG_REG10_PRIM_7_RX_SI_EN_BMSK                                                  0x80
#define HWIO_GENI_CFG_REG10_PRIM_7_RX_SI_EN_SHFT                                                   0x7
#define HWIO_GENI_CFG_REG10_PRIM_6_RX_SI_EN_BMSK                                                  0x40
#define HWIO_GENI_CFG_REG10_PRIM_6_RX_SI_EN_SHFT                                                   0x6
#define HWIO_GENI_CFG_REG10_PRIM_5_RX_SI_EN_BMSK                                                  0x20
#define HWIO_GENI_CFG_REG10_PRIM_5_RX_SI_EN_SHFT                                                   0x5
#define HWIO_GENI_CFG_REG10_PRIM_4_RX_SI_EN_BMSK                                                  0x10
#define HWIO_GENI_CFG_REG10_PRIM_4_RX_SI_EN_SHFT                                                   0x4
#define HWIO_GENI_CFG_REG10_PRIM_3_RX_SI_EN_BMSK                                                   0x8
#define HWIO_GENI_CFG_REG10_PRIM_3_RX_SI_EN_SHFT                                                   0x3
#define HWIO_GENI_CFG_REG10_PRIM_2_RX_SI_EN_BMSK                                                   0x4
#define HWIO_GENI_CFG_REG10_PRIM_2_RX_SI_EN_SHFT                                                   0x2
#define HWIO_GENI_CFG_REG10_PRIM_1_RX_SI_EN_BMSK                                                   0x2
#define HWIO_GENI_CFG_REG10_PRIM_1_RX_SI_EN_SHFT                                                   0x1
#define HWIO_GENI_CFG_REG10_PRIM_0_RX_SI_EN_BMSK                                                   0x1
#define HWIO_GENI_CFG_REG10_PRIM_0_RX_SI_EN_SHFT                                                   0x0

#define HWIO_GENI_CFG_REG11_ADDR(x)                                                         ((x) + 0x0000002c)
#define HWIO_GENI_CFG_REG11_OFFS                                                            (0x0000002c)
#define HWIO_GENI_CFG_REG11_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG11_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG11_ADDR(x), HWIO_GENI_CFG_REG11_RMSK)
#define HWIO_GENI_CFG_REG11_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG11_ADDR(x), m)
#define HWIO_GENI_CFG_REG11_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG11_ADDR(x),v)
#define HWIO_GENI_CFG_REG11_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG11_ADDR(x),m,v,HWIO_GENI_CFG_REG11_IN(x))
#define HWIO_GENI_CFG_REG11_PRIM_23_RX_PAR_EN_BMSK                                            0x800000
#define HWIO_GENI_CFG_REG11_PRIM_23_RX_PAR_EN_SHFT                                                0x17
#define HWIO_GENI_CFG_REG11_PRIM_22_RX_PAR_EN_BMSK                                            0x400000
#define HWIO_GENI_CFG_REG11_PRIM_22_RX_PAR_EN_SHFT                                                0x16
#define HWIO_GENI_CFG_REG11_PRIM_21_RX_PAR_EN_BMSK                                            0x200000
#define HWIO_GENI_CFG_REG11_PRIM_21_RX_PAR_EN_SHFT                                                0x15
#define HWIO_GENI_CFG_REG11_PRIM_20_RX_PAR_EN_BMSK                                            0x100000
#define HWIO_GENI_CFG_REG11_PRIM_20_RX_PAR_EN_SHFT                                                0x14
#define HWIO_GENI_CFG_REG11_PRIM_19_RX_PAR_EN_BMSK                                             0x80000
#define HWIO_GENI_CFG_REG11_PRIM_19_RX_PAR_EN_SHFT                                                0x13
#define HWIO_GENI_CFG_REG11_PRIM_18_RX_PAR_EN_BMSK                                             0x40000
#define HWIO_GENI_CFG_REG11_PRIM_18_RX_PAR_EN_SHFT                                                0x12
#define HWIO_GENI_CFG_REG11_PRIM_17_RX_PAR_EN_BMSK                                             0x20000
#define HWIO_GENI_CFG_REG11_PRIM_17_RX_PAR_EN_SHFT                                                0x11
#define HWIO_GENI_CFG_REG11_PRIM_16_RX_PAR_EN_BMSK                                             0x10000
#define HWIO_GENI_CFG_REG11_PRIM_16_RX_PAR_EN_SHFT                                                0x10
#define HWIO_GENI_CFG_REG11_PRIM_15_RX_PAR_EN_BMSK                                              0x8000
#define HWIO_GENI_CFG_REG11_PRIM_15_RX_PAR_EN_SHFT                                                 0xf
#define HWIO_GENI_CFG_REG11_PRIM_14_RX_PAR_EN_BMSK                                              0x4000
#define HWIO_GENI_CFG_REG11_PRIM_14_RX_PAR_EN_SHFT                                                 0xe
#define HWIO_GENI_CFG_REG11_PRIM_13_RX_PAR_EN_BMSK                                              0x2000
#define HWIO_GENI_CFG_REG11_PRIM_13_RX_PAR_EN_SHFT                                                 0xd
#define HWIO_GENI_CFG_REG11_PRIM_12_RX_PAR_EN_BMSK                                              0x1000
#define HWIO_GENI_CFG_REG11_PRIM_12_RX_PAR_EN_SHFT                                                 0xc
#define HWIO_GENI_CFG_REG11_PRIM_11_RX_PAR_EN_BMSK                                               0x800
#define HWIO_GENI_CFG_REG11_PRIM_11_RX_PAR_EN_SHFT                                                 0xb
#define HWIO_GENI_CFG_REG11_PRIM_10_RX_PAR_EN_BMSK                                               0x400
#define HWIO_GENI_CFG_REG11_PRIM_10_RX_PAR_EN_SHFT                                                 0xa
#define HWIO_GENI_CFG_REG11_PRIM_9_RX_PAR_EN_BMSK                                                0x200
#define HWIO_GENI_CFG_REG11_PRIM_9_RX_PAR_EN_SHFT                                                  0x9
#define HWIO_GENI_CFG_REG11_PRIM_8_RX_PAR_EN_BMSK                                                0x100
#define HWIO_GENI_CFG_REG11_PRIM_8_RX_PAR_EN_SHFT                                                  0x8
#define HWIO_GENI_CFG_REG11_PRIM_7_RX_PAR_EN_BMSK                                                 0x80
#define HWIO_GENI_CFG_REG11_PRIM_7_RX_PAR_EN_SHFT                                                  0x7
#define HWIO_GENI_CFG_REG11_PRIM_6_RX_PAR_EN_BMSK                                                 0x40
#define HWIO_GENI_CFG_REG11_PRIM_6_RX_PAR_EN_SHFT                                                  0x6
#define HWIO_GENI_CFG_REG11_PRIM_5_RX_PAR_EN_BMSK                                                 0x20
#define HWIO_GENI_CFG_REG11_PRIM_5_RX_PAR_EN_SHFT                                                  0x5
#define HWIO_GENI_CFG_REG11_PRIM_4_RX_PAR_EN_BMSK                                                 0x10
#define HWIO_GENI_CFG_REG11_PRIM_4_RX_PAR_EN_SHFT                                                  0x4
#define HWIO_GENI_CFG_REG11_PRIM_3_RX_PAR_EN_BMSK                                                  0x8
#define HWIO_GENI_CFG_REG11_PRIM_3_RX_PAR_EN_SHFT                                                  0x3
#define HWIO_GENI_CFG_REG11_PRIM_2_RX_PAR_EN_BMSK                                                  0x4
#define HWIO_GENI_CFG_REG11_PRIM_2_RX_PAR_EN_SHFT                                                  0x2
#define HWIO_GENI_CFG_REG11_PRIM_1_RX_PAR_EN_BMSK                                                  0x2
#define HWIO_GENI_CFG_REG11_PRIM_1_RX_PAR_EN_SHFT                                                  0x1
#define HWIO_GENI_CFG_REG11_PRIM_0_RX_PAR_EN_BMSK                                                  0x1
#define HWIO_GENI_CFG_REG11_PRIM_0_RX_PAR_EN_SHFT                                                  0x0

#define HWIO_GENI_CFG_REG12_ADDR(x)                                                         ((x) + 0x00000030)
#define HWIO_GENI_CFG_REG12_OFFS                                                            (0x00000030)
#define HWIO_GENI_CFG_REG12_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG12_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG12_ADDR(x), HWIO_GENI_CFG_REG12_RMSK)
#define HWIO_GENI_CFG_REG12_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG12_ADDR(x), m)
#define HWIO_GENI_CFG_REG12_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG12_ADDR(x),v)
#define HWIO_GENI_CFG_REG12_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG12_ADDR(x),m,v,HWIO_GENI_CFG_REG12_IN(x))
#define HWIO_GENI_CFG_REG12_PRIM_23_RX_PAR_FIRST_BMSK                                         0x800000
#define HWIO_GENI_CFG_REG12_PRIM_23_RX_PAR_FIRST_SHFT                                             0x17
#define HWIO_GENI_CFG_REG12_PRIM_22_RX_PAR_FIRST_BMSK                                         0x400000
#define HWIO_GENI_CFG_REG12_PRIM_22_RX_PAR_FIRST_SHFT                                             0x16
#define HWIO_GENI_CFG_REG12_PRIM_21_RX_PAR_FIRST_BMSK                                         0x200000
#define HWIO_GENI_CFG_REG12_PRIM_21_RX_PAR_FIRST_SHFT                                             0x15
#define HWIO_GENI_CFG_REG12_PRIM_20_RX_PAR_FIRST_BMSK                                         0x100000
#define HWIO_GENI_CFG_REG12_PRIM_20_RX_PAR_FIRST_SHFT                                             0x14
#define HWIO_GENI_CFG_REG12_PRIM_19_RX_PAR_FIRST_BMSK                                          0x80000
#define HWIO_GENI_CFG_REG12_PRIM_19_RX_PAR_FIRST_SHFT                                             0x13
#define HWIO_GENI_CFG_REG12_PRIM_18_RX_PAR_FIRST_BMSK                                          0x40000
#define HWIO_GENI_CFG_REG12_PRIM_18_RX_PAR_FIRST_SHFT                                             0x12
#define HWIO_GENI_CFG_REG12_PRIM_17_RX_PAR_FIRST_BMSK                                          0x20000
#define HWIO_GENI_CFG_REG12_PRIM_17_RX_PAR_FIRST_SHFT                                             0x11
#define HWIO_GENI_CFG_REG12_PRIM_16_RX_PAR_FIRST_BMSK                                          0x10000
#define HWIO_GENI_CFG_REG12_PRIM_16_RX_PAR_FIRST_SHFT                                             0x10
#define HWIO_GENI_CFG_REG12_PRIM_15_RX_PAR_FIRST_BMSK                                           0x8000
#define HWIO_GENI_CFG_REG12_PRIM_15_RX_PAR_FIRST_SHFT                                              0xf
#define HWIO_GENI_CFG_REG12_PRIM_14_RX_PAR_FIRST_BMSK                                           0x4000
#define HWIO_GENI_CFG_REG12_PRIM_14_RX_PAR_FIRST_SHFT                                              0xe
#define HWIO_GENI_CFG_REG12_PRIM_13_RX_PAR_FIRST_BMSK                                           0x2000
#define HWIO_GENI_CFG_REG12_PRIM_13_RX_PAR_FIRST_SHFT                                              0xd
#define HWIO_GENI_CFG_REG12_PRIM_12_RX_PAR_FIRST_BMSK                                           0x1000
#define HWIO_GENI_CFG_REG12_PRIM_12_RX_PAR_FIRST_SHFT                                              0xc
#define HWIO_GENI_CFG_REG12_PRIM_11_RX_PAR_FIRST_BMSK                                            0x800
#define HWIO_GENI_CFG_REG12_PRIM_11_RX_PAR_FIRST_SHFT                                              0xb
#define HWIO_GENI_CFG_REG12_PRIM_10_RX_PAR_FIRST_BMSK                                            0x400
#define HWIO_GENI_CFG_REG12_PRIM_10_RX_PAR_FIRST_SHFT                                              0xa
#define HWIO_GENI_CFG_REG12_PRIM_9_RX_PAR_FIRST_BMSK                                             0x200
#define HWIO_GENI_CFG_REG12_PRIM_9_RX_PAR_FIRST_SHFT                                               0x9
#define HWIO_GENI_CFG_REG12_PRIM_8_RX_PAR_FIRST_BMSK                                             0x100
#define HWIO_GENI_CFG_REG12_PRIM_8_RX_PAR_FIRST_SHFT                                               0x8
#define HWIO_GENI_CFG_REG12_PRIM_7_RX_PAR_FIRST_BMSK                                              0x80
#define HWIO_GENI_CFG_REG12_PRIM_7_RX_PAR_FIRST_SHFT                                               0x7
#define HWIO_GENI_CFG_REG12_PRIM_6_RX_PAR_FIRST_BMSK                                              0x40
#define HWIO_GENI_CFG_REG12_PRIM_6_RX_PAR_FIRST_SHFT                                               0x6
#define HWIO_GENI_CFG_REG12_PRIM_5_RX_PAR_FIRST_BMSK                                              0x20
#define HWIO_GENI_CFG_REG12_PRIM_5_RX_PAR_FIRST_SHFT                                               0x5
#define HWIO_GENI_CFG_REG12_PRIM_4_RX_PAR_FIRST_BMSK                                              0x10
#define HWIO_GENI_CFG_REG12_PRIM_4_RX_PAR_FIRST_SHFT                                               0x4
#define HWIO_GENI_CFG_REG12_PRIM_3_RX_PAR_FIRST_BMSK                                               0x8
#define HWIO_GENI_CFG_REG12_PRIM_3_RX_PAR_FIRST_SHFT                                               0x3
#define HWIO_GENI_CFG_REG12_PRIM_2_RX_PAR_FIRST_BMSK                                               0x4
#define HWIO_GENI_CFG_REG12_PRIM_2_RX_PAR_FIRST_SHFT                                               0x2
#define HWIO_GENI_CFG_REG12_PRIM_1_RX_PAR_FIRST_BMSK                                               0x2
#define HWIO_GENI_CFG_REG12_PRIM_1_RX_PAR_FIRST_SHFT                                               0x1
#define HWIO_GENI_CFG_REG12_PRIM_0_RX_PAR_FIRST_BMSK                                               0x1
#define HWIO_GENI_CFG_REG12_PRIM_0_RX_PAR_FIRST_SHFT                                               0x0

#define HWIO_GENI_CFG_REG13_ADDR(x)                                                         ((x) + 0x00000034)
#define HWIO_GENI_CFG_REG13_OFFS                                                            (0x00000034)
#define HWIO_GENI_CFG_REG13_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG13_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG13_ADDR(x), HWIO_GENI_CFG_REG13_RMSK)
#define HWIO_GENI_CFG_REG13_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG13_ADDR(x), m)
#define HWIO_GENI_CFG_REG13_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG13_ADDR(x),v)
#define HWIO_GENI_CFG_REG13_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG13_ADDR(x),m,v,HWIO_GENI_CFG_REG13_IN(x))
#define HWIO_GENI_CFG_REG13_PRIM_23_ACC_PAR_EN_BMSK                                           0x800000
#define HWIO_GENI_CFG_REG13_PRIM_23_ACC_PAR_EN_SHFT                                               0x17
#define HWIO_GENI_CFG_REG13_PRIM_22_ACC_PAR_EN_BMSK                                           0x400000
#define HWIO_GENI_CFG_REG13_PRIM_22_ACC_PAR_EN_SHFT                                               0x16
#define HWIO_GENI_CFG_REG13_PRIM_21_ACC_PAR_EN_BMSK                                           0x200000
#define HWIO_GENI_CFG_REG13_PRIM_21_ACC_PAR_EN_SHFT                                               0x15
#define HWIO_GENI_CFG_REG13_PRIM_20_ACC_PAR_EN_BMSK                                           0x100000
#define HWIO_GENI_CFG_REG13_PRIM_20_ACC_PAR_EN_SHFT                                               0x14
#define HWIO_GENI_CFG_REG13_PRIM_19_ACC_PAR_EN_BMSK                                            0x80000
#define HWIO_GENI_CFG_REG13_PRIM_19_ACC_PAR_EN_SHFT                                               0x13
#define HWIO_GENI_CFG_REG13_PRIM_18_ACC_PAR_EN_BMSK                                            0x40000
#define HWIO_GENI_CFG_REG13_PRIM_18_ACC_PAR_EN_SHFT                                               0x12
#define HWIO_GENI_CFG_REG13_PRIM_17_ACC_PAR_EN_BMSK                                            0x20000
#define HWIO_GENI_CFG_REG13_PRIM_17_ACC_PAR_EN_SHFT                                               0x11
#define HWIO_GENI_CFG_REG13_PRIM_16_ACC_PAR_EN_BMSK                                            0x10000
#define HWIO_GENI_CFG_REG13_PRIM_16_ACC_PAR_EN_SHFT                                               0x10
#define HWIO_GENI_CFG_REG13_PRIM_15_ACC_PAR_EN_BMSK                                             0x8000
#define HWIO_GENI_CFG_REG13_PRIM_15_ACC_PAR_EN_SHFT                                                0xf
#define HWIO_GENI_CFG_REG13_PRIM_14_ACC_PAR_EN_BMSK                                             0x4000
#define HWIO_GENI_CFG_REG13_PRIM_14_ACC_PAR_EN_SHFT                                                0xe
#define HWIO_GENI_CFG_REG13_PRIM_13_ACC_PAR_EN_BMSK                                             0x2000
#define HWIO_GENI_CFG_REG13_PRIM_13_ACC_PAR_EN_SHFT                                                0xd
#define HWIO_GENI_CFG_REG13_PRIM_12_ACC_PAR_EN_BMSK                                             0x1000
#define HWIO_GENI_CFG_REG13_PRIM_12_ACC_PAR_EN_SHFT                                                0xc
#define HWIO_GENI_CFG_REG13_PRIM_11_ACC_PAR_EN_BMSK                                              0x800
#define HWIO_GENI_CFG_REG13_PRIM_11_ACC_PAR_EN_SHFT                                                0xb
#define HWIO_GENI_CFG_REG13_PRIM_10_ACC_PAR_EN_BMSK                                              0x400
#define HWIO_GENI_CFG_REG13_PRIM_10_ACC_PAR_EN_SHFT                                                0xa
#define HWIO_GENI_CFG_REG13_PRIM_9_ACC_PAR_EN_BMSK                                               0x200
#define HWIO_GENI_CFG_REG13_PRIM_9_ACC_PAR_EN_SHFT                                                 0x9
#define HWIO_GENI_CFG_REG13_PRIM_8_ACC_PAR_EN_BMSK                                               0x100
#define HWIO_GENI_CFG_REG13_PRIM_8_ACC_PAR_EN_SHFT                                                 0x8
#define HWIO_GENI_CFG_REG13_PRIM_7_ACC_PAR_EN_BMSK                                                0x80
#define HWIO_GENI_CFG_REG13_PRIM_7_ACC_PAR_EN_SHFT                                                 0x7
#define HWIO_GENI_CFG_REG13_PRIM_6_ACC_PAR_EN_BMSK                                                0x40
#define HWIO_GENI_CFG_REG13_PRIM_6_ACC_PAR_EN_SHFT                                                 0x6
#define HWIO_GENI_CFG_REG13_PRIM_5_ACC_PAR_EN_BMSK                                                0x20
#define HWIO_GENI_CFG_REG13_PRIM_5_ACC_PAR_EN_SHFT                                                 0x5
#define HWIO_GENI_CFG_REG13_PRIM_4_ACC_PAR_EN_BMSK                                                0x10
#define HWIO_GENI_CFG_REG13_PRIM_4_ACC_PAR_EN_SHFT                                                 0x4
#define HWIO_GENI_CFG_REG13_PRIM_3_ACC_PAR_EN_BMSK                                                 0x8
#define HWIO_GENI_CFG_REG13_PRIM_3_ACC_PAR_EN_SHFT                                                 0x3
#define HWIO_GENI_CFG_REG13_PRIM_2_ACC_PAR_EN_BMSK                                                 0x4
#define HWIO_GENI_CFG_REG13_PRIM_2_ACC_PAR_EN_SHFT                                                 0x2
#define HWIO_GENI_CFG_REG13_PRIM_1_ACC_PAR_EN_BMSK                                                 0x2
#define HWIO_GENI_CFG_REG13_PRIM_1_ACC_PAR_EN_SHFT                                                 0x1
#define HWIO_GENI_CFG_REG13_PRIM_0_ACC_PAR_EN_BMSK                                                 0x1
#define HWIO_GENI_CFG_REG13_PRIM_0_ACC_PAR_EN_SHFT                                                 0x0

#define HWIO_GENI_CFG_REG14_ADDR(x)                                                         ((x) + 0x00000038)
#define HWIO_GENI_CFG_REG14_OFFS                                                            (0x00000038)
#define HWIO_GENI_CFG_REG14_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG14_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG14_ADDR(x), HWIO_GENI_CFG_REG14_RMSK)
#define HWIO_GENI_CFG_REG14_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG14_ADDR(x), m)
#define HWIO_GENI_CFG_REG14_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG14_ADDR(x),v)
#define HWIO_GENI_CFG_REG14_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG14_ADDR(x),m,v,HWIO_GENI_CFG_REG14_IN(x))
#define HWIO_GENI_CFG_REG14_PRIM_11_IO2_FUNC_SEL_BMSK                                         0xc00000
#define HWIO_GENI_CFG_REG14_PRIM_11_IO2_FUNC_SEL_SHFT                                             0x16
#define HWIO_GENI_CFG_REG14_PRIM_10_IO2_FUNC_SEL_BMSK                                         0x300000
#define HWIO_GENI_CFG_REG14_PRIM_10_IO2_FUNC_SEL_SHFT                                             0x14
#define HWIO_GENI_CFG_REG14_PRIM_9_IO2_FUNC_SEL_BMSK                                           0xc0000
#define HWIO_GENI_CFG_REG14_PRIM_9_IO2_FUNC_SEL_SHFT                                              0x12
#define HWIO_GENI_CFG_REG14_PRIM_8_IO2_FUNC_SEL_BMSK                                           0x30000
#define HWIO_GENI_CFG_REG14_PRIM_8_IO2_FUNC_SEL_SHFT                                              0x10
#define HWIO_GENI_CFG_REG14_PRIM_7_IO2_FUNC_SEL_BMSK                                            0xc000
#define HWIO_GENI_CFG_REG14_PRIM_7_IO2_FUNC_SEL_SHFT                                               0xe
#define HWIO_GENI_CFG_REG14_PRIM_6_IO2_FUNC_SEL_BMSK                                            0x3000
#define HWIO_GENI_CFG_REG14_PRIM_6_IO2_FUNC_SEL_SHFT                                               0xc
#define HWIO_GENI_CFG_REG14_PRIM_5_IO2_FUNC_SEL_BMSK                                             0xc00
#define HWIO_GENI_CFG_REG14_PRIM_5_IO2_FUNC_SEL_SHFT                                               0xa
#define HWIO_GENI_CFG_REG14_PRIM_4_IO2_FUNC_SEL_BMSK                                             0x300
#define HWIO_GENI_CFG_REG14_PRIM_4_IO2_FUNC_SEL_SHFT                                               0x8
#define HWIO_GENI_CFG_REG14_PRIM_3_IO2_FUNC_SEL_BMSK                                              0xc0
#define HWIO_GENI_CFG_REG14_PRIM_3_IO2_FUNC_SEL_SHFT                                               0x6
#define HWIO_GENI_CFG_REG14_PRIM_2_IO2_FUNC_SEL_BMSK                                              0x30
#define HWIO_GENI_CFG_REG14_PRIM_2_IO2_FUNC_SEL_SHFT                                               0x4
#define HWIO_GENI_CFG_REG14_PRIM_1_IO2_FUNC_SEL_BMSK                                               0xc
#define HWIO_GENI_CFG_REG14_PRIM_1_IO2_FUNC_SEL_SHFT                                               0x2
#define HWIO_GENI_CFG_REG14_PRIM_0_IO2_FUNC_SEL_BMSK                                               0x3
#define HWIO_GENI_CFG_REG14_PRIM_0_IO2_FUNC_SEL_SHFT                                               0x0

#define HWIO_GENI_CFG_REG15_ADDR(x)                                                         ((x) + 0x0000003c)
#define HWIO_GENI_CFG_REG15_OFFS                                                            (0x0000003c)
#define HWIO_GENI_CFG_REG15_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG15_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG15_ADDR(x), HWIO_GENI_CFG_REG15_RMSK)
#define HWIO_GENI_CFG_REG15_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG15_ADDR(x), m)
#define HWIO_GENI_CFG_REG15_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG15_ADDR(x),v)
#define HWIO_GENI_CFG_REG15_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG15_ADDR(x),m,v,HWIO_GENI_CFG_REG15_IN(x))
#define HWIO_GENI_CFG_REG15_PRIM_23_IO2_FUNC_SEL_BMSK                                         0xc00000
#define HWIO_GENI_CFG_REG15_PRIM_23_IO2_FUNC_SEL_SHFT                                             0x16
#define HWIO_GENI_CFG_REG15_PRIM_22_IO2_FUNC_SEL_BMSK                                         0x300000
#define HWIO_GENI_CFG_REG15_PRIM_22_IO2_FUNC_SEL_SHFT                                             0x14
#define HWIO_GENI_CFG_REG15_PRIM_21_IO2_FUNC_SEL_BMSK                                          0xc0000
#define HWIO_GENI_CFG_REG15_PRIM_21_IO2_FUNC_SEL_SHFT                                             0x12
#define HWIO_GENI_CFG_REG15_PRIM_20_IO2_FUNC_SEL_BMSK                                          0x30000
#define HWIO_GENI_CFG_REG15_PRIM_20_IO2_FUNC_SEL_SHFT                                             0x10
#define HWIO_GENI_CFG_REG15_PRIM_19_IO2_FUNC_SEL_BMSK                                           0xc000
#define HWIO_GENI_CFG_REG15_PRIM_19_IO2_FUNC_SEL_SHFT                                              0xe
#define HWIO_GENI_CFG_REG15_PRIM_18_IO2_FUNC_SEL_BMSK                                           0x3000
#define HWIO_GENI_CFG_REG15_PRIM_18_IO2_FUNC_SEL_SHFT                                              0xc
#define HWIO_GENI_CFG_REG15_PRIM_17_IO2_FUNC_SEL_BMSK                                            0xc00
#define HWIO_GENI_CFG_REG15_PRIM_17_IO2_FUNC_SEL_SHFT                                              0xa
#define HWIO_GENI_CFG_REG15_PRIM_16_IO2_FUNC_SEL_BMSK                                            0x300
#define HWIO_GENI_CFG_REG15_PRIM_16_IO2_FUNC_SEL_SHFT                                              0x8
#define HWIO_GENI_CFG_REG15_PRIM_15_IO2_FUNC_SEL_BMSK                                             0xc0
#define HWIO_GENI_CFG_REG15_PRIM_15_IO2_FUNC_SEL_SHFT                                              0x6
#define HWIO_GENI_CFG_REG15_PRIM_14_IO2_FUNC_SEL_BMSK                                             0x30
#define HWIO_GENI_CFG_REG15_PRIM_14_IO2_FUNC_SEL_SHFT                                              0x4
#define HWIO_GENI_CFG_REG15_PRIM_13_IO2_FUNC_SEL_BMSK                                              0xc
#define HWIO_GENI_CFG_REG15_PRIM_13_IO2_FUNC_SEL_SHFT                                              0x2
#define HWIO_GENI_CFG_REG15_PRIM_12_IO2_FUNC_SEL_BMSK                                              0x3
#define HWIO_GENI_CFG_REG15_PRIM_12_IO2_FUNC_SEL_SHFT                                              0x0

#define HWIO_GENI_CFG_REG16_ADDR(x)                                                         ((x) + 0x00000040)
#define HWIO_GENI_CFG_REG16_OFFS                                                            (0x00000040)
#define HWIO_GENI_CFG_REG16_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG16_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG16_ADDR(x), HWIO_GENI_CFG_REG16_RMSK)
#define HWIO_GENI_CFG_REG16_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG16_ADDR(x), m)
#define HWIO_GENI_CFG_REG16_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG16_ADDR(x),v)
#define HWIO_GENI_CFG_REG16_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG16_ADDR(x),m,v,HWIO_GENI_CFG_REG16_IN(x))
#define HWIO_GENI_CFG_REG16_PRIM_23_TX_TIME_COUNTER_EN_BMSK                                   0x800000
#define HWIO_GENI_CFG_REG16_PRIM_23_TX_TIME_COUNTER_EN_SHFT                                       0x17
#define HWIO_GENI_CFG_REG16_PRIM_22_TX_TIME_COUNTER_EN_BMSK                                   0x400000
#define HWIO_GENI_CFG_REG16_PRIM_22_TX_TIME_COUNTER_EN_SHFT                                       0x16
#define HWIO_GENI_CFG_REG16_PRIM_21_TX_TIME_COUNTER_EN_BMSK                                   0x200000
#define HWIO_GENI_CFG_REG16_PRIM_21_TX_TIME_COUNTER_EN_SHFT                                       0x15
#define HWIO_GENI_CFG_REG16_PRIM_20_TX_TIME_COUNTER_EN_BMSK                                   0x100000
#define HWIO_GENI_CFG_REG16_PRIM_20_TX_TIME_COUNTER_EN_SHFT                                       0x14
#define HWIO_GENI_CFG_REG16_PRIM_19_TX_TIME_COUNTER_EN_BMSK                                    0x80000
#define HWIO_GENI_CFG_REG16_PRIM_19_TX_TIME_COUNTER_EN_SHFT                                       0x13
#define HWIO_GENI_CFG_REG16_PRIM_18_TX_TIME_COUNTER_EN_BMSK                                    0x40000
#define HWIO_GENI_CFG_REG16_PRIM_18_TX_TIME_COUNTER_EN_SHFT                                       0x12
#define HWIO_GENI_CFG_REG16_PRIM_17_TX_TIME_COUNTER_EN_BMSK                                    0x20000
#define HWIO_GENI_CFG_REG16_PRIM_17_TX_TIME_COUNTER_EN_SHFT                                       0x11
#define HWIO_GENI_CFG_REG16_PRIM_16_TX_TIME_COUNTER_EN_BMSK                                    0x10000
#define HWIO_GENI_CFG_REG16_PRIM_16_TX_TIME_COUNTER_EN_SHFT                                       0x10
#define HWIO_GENI_CFG_REG16_PRIM_15_TX_TIME_COUNTER_EN_BMSK                                     0x8000
#define HWIO_GENI_CFG_REG16_PRIM_15_TX_TIME_COUNTER_EN_SHFT                                        0xf
#define HWIO_GENI_CFG_REG16_PRIM_14_TX_TIME_COUNTER_EN_BMSK                                     0x4000
#define HWIO_GENI_CFG_REG16_PRIM_14_TX_TIME_COUNTER_EN_SHFT                                        0xe
#define HWIO_GENI_CFG_REG16_PRIM_13_TX_TIME_COUNTER_EN_BMSK                                     0x2000
#define HWIO_GENI_CFG_REG16_PRIM_13_TX_TIME_COUNTER_EN_SHFT                                        0xd
#define HWIO_GENI_CFG_REG16_PRIM_12_TX_TIME_COUNTER_EN_BMSK                                     0x1000
#define HWIO_GENI_CFG_REG16_PRIM_12_TX_TIME_COUNTER_EN_SHFT                                        0xc
#define HWIO_GENI_CFG_REG16_PRIM_11_TX_TIME_COUNTER_EN_BMSK                                      0x800
#define HWIO_GENI_CFG_REG16_PRIM_11_TX_TIME_COUNTER_EN_SHFT                                        0xb
#define HWIO_GENI_CFG_REG16_PRIM_10_TX_TIME_COUNTER_EN_BMSK                                      0x400
#define HWIO_GENI_CFG_REG16_PRIM_10_TX_TIME_COUNTER_EN_SHFT                                        0xa
#define HWIO_GENI_CFG_REG16_PRIM_9_TX_TIME_COUNTER_EN_BMSK                                       0x200
#define HWIO_GENI_CFG_REG16_PRIM_9_TX_TIME_COUNTER_EN_SHFT                                         0x9
#define HWIO_GENI_CFG_REG16_PRIM_8_TX_TIME_COUNTER_EN_BMSK                                       0x100
#define HWIO_GENI_CFG_REG16_PRIM_8_TX_TIME_COUNTER_EN_SHFT                                         0x8
#define HWIO_GENI_CFG_REG16_PRIM_7_TX_TIME_COUNTER_EN_BMSK                                        0x80
#define HWIO_GENI_CFG_REG16_PRIM_7_TX_TIME_COUNTER_EN_SHFT                                         0x7
#define HWIO_GENI_CFG_REG16_PRIM_6_TX_TIME_COUNTER_EN_BMSK                                        0x40
#define HWIO_GENI_CFG_REG16_PRIM_6_TX_TIME_COUNTER_EN_SHFT                                         0x6
#define HWIO_GENI_CFG_REG16_PRIM_5_TX_TIME_COUNTER_EN_BMSK                                        0x20
#define HWIO_GENI_CFG_REG16_PRIM_5_TX_TIME_COUNTER_EN_SHFT                                         0x5
#define HWIO_GENI_CFG_REG16_PRIM_4_TX_TIME_COUNTER_EN_BMSK                                        0x10
#define HWIO_GENI_CFG_REG16_PRIM_4_TX_TIME_COUNTER_EN_SHFT                                         0x4
#define HWIO_GENI_CFG_REG16_PRIM_3_TX_TIME_COUNTER_EN_BMSK                                         0x8
#define HWIO_GENI_CFG_REG16_PRIM_3_TX_TIME_COUNTER_EN_SHFT                                         0x3
#define HWIO_GENI_CFG_REG16_PRIM_2_TX_TIME_COUNTER_EN_BMSK                                         0x4
#define HWIO_GENI_CFG_REG16_PRIM_2_TX_TIME_COUNTER_EN_SHFT                                         0x2
#define HWIO_GENI_CFG_REG16_PRIM_1_TX_TIME_COUNTER_EN_BMSK                                         0x2
#define HWIO_GENI_CFG_REG16_PRIM_1_TX_TIME_COUNTER_EN_SHFT                                         0x1
#define HWIO_GENI_CFG_REG16_PRIM_0_TX_TIME_COUNTER_EN_BMSK                                         0x1
#define HWIO_GENI_CFG_REG16_PRIM_0_TX_TIME_COUNTER_EN_SHFT                                         0x0

#define HWIO_GENI_CFG_REG17_ADDR(x)                                                         ((x) + 0x00000044)
#define HWIO_GENI_CFG_REG17_OFFS                                                            (0x00000044)
#define HWIO_GENI_CFG_REG17_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG17_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG17_ADDR(x), HWIO_GENI_CFG_REG17_RMSK)
#define HWIO_GENI_CFG_REG17_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG17_ADDR(x), m)
#define HWIO_GENI_CFG_REG17_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG17_ADDR(x),v)
#define HWIO_GENI_CFG_REG17_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG17_ADDR(x),m,v,HWIO_GENI_CFG_REG17_IN(x))
#define HWIO_GENI_CFG_REG17_PRIM_23_RX_TIME_COUNTER_EN_BMSK                                   0x800000
#define HWIO_GENI_CFG_REG17_PRIM_23_RX_TIME_COUNTER_EN_SHFT                                       0x17
#define HWIO_GENI_CFG_REG17_PRIM_22_RX_TIME_COUNTER_EN_BMSK                                   0x400000
#define HWIO_GENI_CFG_REG17_PRIM_22_RX_TIME_COUNTER_EN_SHFT                                       0x16
#define HWIO_GENI_CFG_REG17_PRIM_21_RX_TIME_COUNTER_EN_BMSK                                   0x200000
#define HWIO_GENI_CFG_REG17_PRIM_21_RX_TIME_COUNTER_EN_SHFT                                       0x15
#define HWIO_GENI_CFG_REG17_PRIM_20_RX_TIME_COUNTER_EN_BMSK                                   0x100000
#define HWIO_GENI_CFG_REG17_PRIM_20_RX_TIME_COUNTER_EN_SHFT                                       0x14
#define HWIO_GENI_CFG_REG17_PRIM_19_RX_TIME_COUNTER_EN_BMSK                                    0x80000
#define HWIO_GENI_CFG_REG17_PRIM_19_RX_TIME_COUNTER_EN_SHFT                                       0x13
#define HWIO_GENI_CFG_REG17_PRIM_18_RX_TIME_COUNTER_EN_BMSK                                    0x40000
#define HWIO_GENI_CFG_REG17_PRIM_18_RX_TIME_COUNTER_EN_SHFT                                       0x12
#define HWIO_GENI_CFG_REG17_PRIM_17_RX_TIME_COUNTER_EN_BMSK                                    0x20000
#define HWIO_GENI_CFG_REG17_PRIM_17_RX_TIME_COUNTER_EN_SHFT                                       0x11
#define HWIO_GENI_CFG_REG17_PRIM_16_RX_TIME_COUNTER_EN_BMSK                                    0x10000
#define HWIO_GENI_CFG_REG17_PRIM_16_RX_TIME_COUNTER_EN_SHFT                                       0x10
#define HWIO_GENI_CFG_REG17_PRIM_15_RX_TIME_COUNTER_EN_BMSK                                     0x8000
#define HWIO_GENI_CFG_REG17_PRIM_15_RX_TIME_COUNTER_EN_SHFT                                        0xf
#define HWIO_GENI_CFG_REG17_PRIM_14_RX_TIME_COUNTER_EN_BMSK                                     0x4000
#define HWIO_GENI_CFG_REG17_PRIM_14_RX_TIME_COUNTER_EN_SHFT                                        0xe
#define HWIO_GENI_CFG_REG17_PRIM_13_RX_TIME_COUNTER_EN_BMSK                                     0x2000
#define HWIO_GENI_CFG_REG17_PRIM_13_RX_TIME_COUNTER_EN_SHFT                                        0xd
#define HWIO_GENI_CFG_REG17_PRIM_12_RX_TIME_COUNTER_EN_BMSK                                     0x1000
#define HWIO_GENI_CFG_REG17_PRIM_12_RX_TIME_COUNTER_EN_SHFT                                        0xc
#define HWIO_GENI_CFG_REG17_PRIM_11_RX_TIME_COUNTER_EN_BMSK                                      0x800
#define HWIO_GENI_CFG_REG17_PRIM_11_RX_TIME_COUNTER_EN_SHFT                                        0xb
#define HWIO_GENI_CFG_REG17_PRIM_10_RX_TIME_COUNTER_EN_BMSK                                      0x400
#define HWIO_GENI_CFG_REG17_PRIM_10_RX_TIME_COUNTER_EN_SHFT                                        0xa
#define HWIO_GENI_CFG_REG17_PRIM_9_RX_TIME_COUNTER_EN_BMSK                                       0x200
#define HWIO_GENI_CFG_REG17_PRIM_9_RX_TIME_COUNTER_EN_SHFT                                         0x9
#define HWIO_GENI_CFG_REG17_PRIM_8_RX_TIME_COUNTER_EN_BMSK                                       0x100
#define HWIO_GENI_CFG_REG17_PRIM_8_RX_TIME_COUNTER_EN_SHFT                                         0x8
#define HWIO_GENI_CFG_REG17_PRIM_7_RX_TIME_COUNTER_EN_BMSK                                        0x80
#define HWIO_GENI_CFG_REG17_PRIM_7_RX_TIME_COUNTER_EN_SHFT                                         0x7
#define HWIO_GENI_CFG_REG17_PRIM_6_RX_TIME_COUNTER_EN_BMSK                                        0x40
#define HWIO_GENI_CFG_REG17_PRIM_6_RX_TIME_COUNTER_EN_SHFT                                         0x6
#define HWIO_GENI_CFG_REG17_PRIM_5_RX_TIME_COUNTER_EN_BMSK                                        0x20
#define HWIO_GENI_CFG_REG17_PRIM_5_RX_TIME_COUNTER_EN_SHFT                                         0x5
#define HWIO_GENI_CFG_REG17_PRIM_4_RX_TIME_COUNTER_EN_BMSK                                        0x10
#define HWIO_GENI_CFG_REG17_PRIM_4_RX_TIME_COUNTER_EN_SHFT                                         0x4
#define HWIO_GENI_CFG_REG17_PRIM_3_RX_TIME_COUNTER_EN_BMSK                                         0x8
#define HWIO_GENI_CFG_REG17_PRIM_3_RX_TIME_COUNTER_EN_SHFT                                         0x3
#define HWIO_GENI_CFG_REG17_PRIM_2_RX_TIME_COUNTER_EN_BMSK                                         0x4
#define HWIO_GENI_CFG_REG17_PRIM_2_RX_TIME_COUNTER_EN_SHFT                                         0x2
#define HWIO_GENI_CFG_REG17_PRIM_1_RX_TIME_COUNTER_EN_BMSK                                         0x2
#define HWIO_GENI_CFG_REG17_PRIM_1_RX_TIME_COUNTER_EN_SHFT                                         0x1
#define HWIO_GENI_CFG_REG17_PRIM_0_RX_TIME_COUNTER_EN_BMSK                                         0x1
#define HWIO_GENI_CFG_REG17_PRIM_0_RX_TIME_COUNTER_EN_SHFT                                         0x0

#define HWIO_GENI_CFG_REG18_ADDR(x)                                                         ((x) + 0x00000048)
#define HWIO_GENI_CFG_REG18_OFFS                                                            (0x00000048)
#define HWIO_GENI_CFG_REG18_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG18_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG18_ADDR(x), HWIO_GENI_CFG_REG18_RMSK)
#define HWIO_GENI_CFG_REG18_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG18_ADDR(x), m)
#define HWIO_GENI_CFG_REG18_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG18_ADDR(x),v)
#define HWIO_GENI_CFG_REG18_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG18_ADDR(x),m,v,HWIO_GENI_CFG_REG18_IN(x))
#define HWIO_GENI_CFG_REG18_PRIM_11_IO3_FUNC_SEL_BMSK                                         0xc00000
#define HWIO_GENI_CFG_REG18_PRIM_11_IO3_FUNC_SEL_SHFT                                             0x16
#define HWIO_GENI_CFG_REG18_PRIM_10_IO3_FUNC_SEL_BMSK                                         0x300000
#define HWIO_GENI_CFG_REG18_PRIM_10_IO3_FUNC_SEL_SHFT                                             0x14
#define HWIO_GENI_CFG_REG18_PRIM_9_IO3_FUNC_SEL_BMSK                                           0xc0000
#define HWIO_GENI_CFG_REG18_PRIM_9_IO3_FUNC_SEL_SHFT                                              0x12
#define HWIO_GENI_CFG_REG18_PRIM_8_IO3_FUNC_SEL_BMSK                                           0x30000
#define HWIO_GENI_CFG_REG18_PRIM_8_IO3_FUNC_SEL_SHFT                                              0x10
#define HWIO_GENI_CFG_REG18_PRIM_7_IO3_FUNC_SEL_BMSK                                            0xc000
#define HWIO_GENI_CFG_REG18_PRIM_7_IO3_FUNC_SEL_SHFT                                               0xe
#define HWIO_GENI_CFG_REG18_PRIM_6_IO3_FUNC_SEL_BMSK                                            0x3000
#define HWIO_GENI_CFG_REG18_PRIM_6_IO3_FUNC_SEL_SHFT                                               0xc
#define HWIO_GENI_CFG_REG18_PRIM_5_IO3_FUNC_SEL_BMSK                                             0xc00
#define HWIO_GENI_CFG_REG18_PRIM_5_IO3_FUNC_SEL_SHFT                                               0xa
#define HWIO_GENI_CFG_REG18_PRIM_4_IO3_FUNC_SEL_BMSK                                             0x300
#define HWIO_GENI_CFG_REG18_PRIM_4_IO3_FUNC_SEL_SHFT                                               0x8
#define HWIO_GENI_CFG_REG18_PRIM_3_IO3_FUNC_SEL_BMSK                                              0xc0
#define HWIO_GENI_CFG_REG18_PRIM_3_IO3_FUNC_SEL_SHFT                                               0x6
#define HWIO_GENI_CFG_REG18_PRIM_2_IO3_FUNC_SEL_BMSK                                              0x30
#define HWIO_GENI_CFG_REG18_PRIM_2_IO3_FUNC_SEL_SHFT                                               0x4
#define HWIO_GENI_CFG_REG18_PRIM_1_IO3_FUNC_SEL_BMSK                                               0xc
#define HWIO_GENI_CFG_REG18_PRIM_1_IO3_FUNC_SEL_SHFT                                               0x2
#define HWIO_GENI_CFG_REG18_PRIM_0_IO3_FUNC_SEL_BMSK                                               0x3
#define HWIO_GENI_CFG_REG18_PRIM_0_IO3_FUNC_SEL_SHFT                                               0x0

#define HWIO_GENI_CFG_REG19_ADDR(x)                                                         ((x) + 0x0000004c)
#define HWIO_GENI_CFG_REG19_OFFS                                                            (0x0000004c)
#define HWIO_GENI_CFG_REG19_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG19_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG19_ADDR(x), HWIO_GENI_CFG_REG19_RMSK)
#define HWIO_GENI_CFG_REG19_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG19_ADDR(x), m)
#define HWIO_GENI_CFG_REG19_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG19_ADDR(x),v)
#define HWIO_GENI_CFG_REG19_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG19_ADDR(x),m,v,HWIO_GENI_CFG_REG19_IN(x))
#define HWIO_GENI_CFG_REG19_PRIM_23_IO3_FUNC_SEL_BMSK                                         0xc00000
#define HWIO_GENI_CFG_REG19_PRIM_23_IO3_FUNC_SEL_SHFT                                             0x16
#define HWIO_GENI_CFG_REG19_PRIM_22_IO3_FUNC_SEL_BMSK                                         0x300000
#define HWIO_GENI_CFG_REG19_PRIM_22_IO3_FUNC_SEL_SHFT                                             0x14
#define HWIO_GENI_CFG_REG19_PRIM_21_IO3_FUNC_SEL_BMSK                                          0xc0000
#define HWIO_GENI_CFG_REG19_PRIM_21_IO3_FUNC_SEL_SHFT                                             0x12
#define HWIO_GENI_CFG_REG19_PRIM_20_IO3_FUNC_SEL_BMSK                                          0x30000
#define HWIO_GENI_CFG_REG19_PRIM_20_IO3_FUNC_SEL_SHFT                                             0x10
#define HWIO_GENI_CFG_REG19_PRIM_19_IO3_FUNC_SEL_BMSK                                           0xc000
#define HWIO_GENI_CFG_REG19_PRIM_19_IO3_FUNC_SEL_SHFT                                              0xe
#define HWIO_GENI_CFG_REG19_PRIM_18_IO3_FUNC_SEL_BMSK                                           0x3000
#define HWIO_GENI_CFG_REG19_PRIM_18_IO3_FUNC_SEL_SHFT                                              0xc
#define HWIO_GENI_CFG_REG19_PRIM_17_IO3_FUNC_SEL_BMSK                                            0xc00
#define HWIO_GENI_CFG_REG19_PRIM_17_IO3_FUNC_SEL_SHFT                                              0xa
#define HWIO_GENI_CFG_REG19_PRIM_16_IO3_FUNC_SEL_BMSK                                            0x300
#define HWIO_GENI_CFG_REG19_PRIM_16_IO3_FUNC_SEL_SHFT                                              0x8
#define HWIO_GENI_CFG_REG19_PRIM_15_IO3_FUNC_SEL_BMSK                                             0xc0
#define HWIO_GENI_CFG_REG19_PRIM_15_IO3_FUNC_SEL_SHFT                                              0x6
#define HWIO_GENI_CFG_REG19_PRIM_14_IO3_FUNC_SEL_BMSK                                             0x30
#define HWIO_GENI_CFG_REG19_PRIM_14_IO3_FUNC_SEL_SHFT                                              0x4
#define HWIO_GENI_CFG_REG19_PRIM_13_IO3_FUNC_SEL_BMSK                                              0xc
#define HWIO_GENI_CFG_REG19_PRIM_13_IO3_FUNC_SEL_SHFT                                              0x2
#define HWIO_GENI_CFG_REG19_PRIM_12_IO3_FUNC_SEL_BMSK                                              0x3
#define HWIO_GENI_CFG_REG19_PRIM_12_IO3_FUNC_SEL_SHFT                                              0x0

#define HWIO_GENI_CFG_REG20_ADDR(x)                                                         ((x) + 0x00000050)
#define HWIO_GENI_CFG_REG20_OFFS                                                            (0x00000050)
#define HWIO_GENI_CFG_REG20_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG20_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG20_ADDR(x), HWIO_GENI_CFG_REG20_RMSK)
#define HWIO_GENI_CFG_REG20_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG20_ADDR(x), m)
#define HWIO_GENI_CFG_REG20_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG20_ADDR(x),v)
#define HWIO_GENI_CFG_REG20_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG20_ADDR(x),m,v,HWIO_GENI_CFG_REG20_IN(x))
#define HWIO_GENI_CFG_REG20_PRIM_23_RX_CHAR_HUNT_EN_BMSK                                      0x800000
#define HWIO_GENI_CFG_REG20_PRIM_23_RX_CHAR_HUNT_EN_SHFT                                          0x17
#define HWIO_GENI_CFG_REG20_PRIM_22_RX_CHAR_HUNT_EN_BMSK                                      0x400000
#define HWIO_GENI_CFG_REG20_PRIM_22_RX_CHAR_HUNT_EN_SHFT                                          0x16
#define HWIO_GENI_CFG_REG20_PRIM_21_RX_CHAR_HUNT_EN_BMSK                                      0x200000
#define HWIO_GENI_CFG_REG20_PRIM_21_RX_CHAR_HUNT_EN_SHFT                                          0x15
#define HWIO_GENI_CFG_REG20_PRIM_20_RX_CHAR_HUNT_EN_BMSK                                      0x100000
#define HWIO_GENI_CFG_REG20_PRIM_20_RX_CHAR_HUNT_EN_SHFT                                          0x14
#define HWIO_GENI_CFG_REG20_PRIM_19_RX_CHAR_HUNT_EN_BMSK                                       0x80000
#define HWIO_GENI_CFG_REG20_PRIM_19_RX_CHAR_HUNT_EN_SHFT                                          0x13
#define HWIO_GENI_CFG_REG20_PRIM_18_RX_CHAR_HUNT_EN_BMSK                                       0x40000
#define HWIO_GENI_CFG_REG20_PRIM_18_RX_CHAR_HUNT_EN_SHFT                                          0x12
#define HWIO_GENI_CFG_REG20_PRIM_17_RX_CHAR_HUNT_EN_BMSK                                       0x20000
#define HWIO_GENI_CFG_REG20_PRIM_17_RX_CHAR_HUNT_EN_SHFT                                          0x11
#define HWIO_GENI_CFG_REG20_PRIM_16_RX_CHAR_HUNT_EN_BMSK                                       0x10000
#define HWIO_GENI_CFG_REG20_PRIM_16_RX_CHAR_HUNT_EN_SHFT                                          0x10
#define HWIO_GENI_CFG_REG20_PRIM_15_RX_CHAR_HUNT_EN_BMSK                                        0x8000
#define HWIO_GENI_CFG_REG20_PRIM_15_RX_CHAR_HUNT_EN_SHFT                                           0xf
#define HWIO_GENI_CFG_REG20_PRIM_14_RX_CHAR_HUNT_EN_BMSK                                        0x4000
#define HWIO_GENI_CFG_REG20_PRIM_14_RX_CHAR_HUNT_EN_SHFT                                           0xe
#define HWIO_GENI_CFG_REG20_PRIM_13_RX_CHAR_HUNT_EN_BMSK                                        0x2000
#define HWIO_GENI_CFG_REG20_PRIM_13_RX_CHAR_HUNT_EN_SHFT                                           0xd
#define HWIO_GENI_CFG_REG20_PRIM_12_RX_CHAR_HUNT_EN_BMSK                                        0x1000
#define HWIO_GENI_CFG_REG20_PRIM_12_RX_CHAR_HUNT_EN_SHFT                                           0xc
#define HWIO_GENI_CFG_REG20_PRIM_11_RX_CHAR_HUNT_EN_BMSK                                         0x800
#define HWIO_GENI_CFG_REG20_PRIM_11_RX_CHAR_HUNT_EN_SHFT                                           0xb
#define HWIO_GENI_CFG_REG20_PRIM_10_RX_CHAR_HUNT_EN_BMSK                                         0x400
#define HWIO_GENI_CFG_REG20_PRIM_10_RX_CHAR_HUNT_EN_SHFT                                           0xa
#define HWIO_GENI_CFG_REG20_PRIM_9_RX_CHAR_HUNT_EN_BMSK                                          0x200
#define HWIO_GENI_CFG_REG20_PRIM_9_RX_CHAR_HUNT_EN_SHFT                                            0x9
#define HWIO_GENI_CFG_REG20_PRIM_8_RX_CHAR_HUNT_EN_BMSK                                          0x100
#define HWIO_GENI_CFG_REG20_PRIM_8_RX_CHAR_HUNT_EN_SHFT                                            0x8
#define HWIO_GENI_CFG_REG20_PRIM_7_RX_CHAR_HUNT_EN_BMSK                                           0x80
#define HWIO_GENI_CFG_REG20_PRIM_7_RX_CHAR_HUNT_EN_SHFT                                            0x7
#define HWIO_GENI_CFG_REG20_PRIM_6_RX_CHAR_HUNT_EN_BMSK                                           0x40
#define HWIO_GENI_CFG_REG20_PRIM_6_RX_CHAR_HUNT_EN_SHFT                                            0x6
#define HWIO_GENI_CFG_REG20_PRIM_5_RX_CHAR_HUNT_EN_BMSK                                           0x20
#define HWIO_GENI_CFG_REG20_PRIM_5_RX_CHAR_HUNT_EN_SHFT                                            0x5
#define HWIO_GENI_CFG_REG20_PRIM_4_RX_CHAR_HUNT_EN_BMSK                                           0x10
#define HWIO_GENI_CFG_REG20_PRIM_4_RX_CHAR_HUNT_EN_SHFT                                            0x4
#define HWIO_GENI_CFG_REG20_PRIM_3_RX_CHAR_HUNT_EN_BMSK                                            0x8
#define HWIO_GENI_CFG_REG20_PRIM_3_RX_CHAR_HUNT_EN_SHFT                                            0x3
#define HWIO_GENI_CFG_REG20_PRIM_2_RX_CHAR_HUNT_EN_BMSK                                            0x4
#define HWIO_GENI_CFG_REG20_PRIM_2_RX_CHAR_HUNT_EN_SHFT                                            0x2
#define HWIO_GENI_CFG_REG20_PRIM_1_RX_CHAR_HUNT_EN_BMSK                                            0x2
#define HWIO_GENI_CFG_REG20_PRIM_1_RX_CHAR_HUNT_EN_SHFT                                            0x1
#define HWIO_GENI_CFG_REG20_PRIM_0_RX_CHAR_HUNT_EN_BMSK                                            0x1
#define HWIO_GENI_CFG_REG20_PRIM_0_RX_CHAR_HUNT_EN_SHFT                                            0x0

#define HWIO_GENI_CFG_REG64_ADDR(x)                                                         ((x) + 0x00000100)
#define HWIO_GENI_CFG_REG64_OFFS                                                            (0x00000100)
#define HWIO_GENI_CFG_REG64_RMSK                                                               0x30a03
#define HWIO_GENI_CFG_REG64_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG64_ADDR(x), HWIO_GENI_CFG_REG64_RMSK)
#define HWIO_GENI_CFG_REG64_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG64_ADDR(x), m)
#define HWIO_GENI_CFG_REG64_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG64_ADDR(x),v)
#define HWIO_GENI_CFG_REG64_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG64_ADDR(x),m,v,HWIO_GENI_CFG_REG64_IN(x))
#define HWIO_GENI_CFG_REG64_M_IO_DATA_IRQ_FW_EN_BMSK                                           0x20000
#define HWIO_GENI_CFG_REG64_M_IO_DATA_IRQ_FW_EN_SHFT                                              0x11
#define HWIO_GENI_CFG_REG64_S_IO_DATA_IRQ_FW_EN_BMSK                                           0x10000
#define HWIO_GENI_CFG_REG64_S_IO_DATA_IRQ_FW_EN_SHFT                                              0x10
#define HWIO_GENI_CFG_REG64_GENI_TX_MODE_BMSK                                                    0x800
#define HWIO_GENI_CFG_REG64_GENI_TX_MODE_SHFT                                                      0xb
#define HWIO_GENI_CFG_REG64_DATA_CNT_MODE_BMSK                                                   0x200
#define HWIO_GENI_CFG_REG64_DATA_CNT_MODE_SHFT                                                     0x9
#define HWIO_GENI_CFG_REG64_EXT_ARB_EN_BMSK                                                        0x2
#define HWIO_GENI_CFG_REG64_EXT_ARB_EN_SHFT                                                        0x1
#define HWIO_GENI_CFG_REG64_M_PROG_RAM_SEC_WORD_EN_BMSK                                            0x1
#define HWIO_GENI_CFG_REG64_M_PROG_RAM_SEC_WORD_EN_SHFT                                            0x0

#define HWIO_GENI_CFG_REG65_ADDR(x)                                                         ((x) + 0x00000104)
#define HWIO_GENI_CFG_REG65_OFFS                                                            (0x00000104)
#define HWIO_GENI_CFG_REG65_RMSK                                                               0x3ffff
#define HWIO_GENI_CFG_REG65_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG65_ADDR(x), HWIO_GENI_CFG_REG65_RMSK)
#define HWIO_GENI_CFG_REG65_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG65_ADDR(x), m)
#define HWIO_GENI_CFG_REG65_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG65_ADDR(x),v)
#define HWIO_GENI_CFG_REG65_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG65_ADDR(x),m,v,HWIO_GENI_CFG_REG65_IN(x))
#define HWIO_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_1_BMSK                                          0x3fe00
#define HWIO_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_1_SHFT                                              0x9
#define HWIO_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_0_BMSK                                            0x1ff
#define HWIO_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_0_SHFT                                              0x0

#define HWIO_GENI_CFG_REG66_ADDR(x)                                                         ((x) + 0x00000108)
#define HWIO_GENI_CFG_REG66_OFFS                                                            (0x00000108)
#define HWIO_GENI_CFG_REG66_RMSK                                                               0x3ffff
#define HWIO_GENI_CFG_REG66_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG66_ADDR(x), HWIO_GENI_CFG_REG66_RMSK)
#define HWIO_GENI_CFG_REG66_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG66_ADDR(x), m)
#define HWIO_GENI_CFG_REG66_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG66_ADDR(x),v)
#define HWIO_GENI_CFG_REG66_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG66_ADDR(x),m,v,HWIO_GENI_CFG_REG66_IN(x))
#define HWIO_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_3_BMSK                                          0x3fe00
#define HWIO_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_3_SHFT                                              0x9
#define HWIO_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_2_BMSK                                            0x1ff
#define HWIO_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_2_SHFT                                              0x0

#define HWIO_GENI_CFG_REG67_ADDR(x)                                                         ((x) + 0x0000010c)
#define HWIO_GENI_CFG_REG67_OFFS                                                            (0x0000010c)
#define HWIO_GENI_CFG_REG67_RMSK                                                             0xfdffffc
#define HWIO_GENI_CFG_REG67_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG67_ADDR(x), HWIO_GENI_CFG_REG67_RMSK)
#define HWIO_GENI_CFG_REG67_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG67_ADDR(x), m)
#define HWIO_GENI_CFG_REG67_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG67_ADDR(x),v)
#define HWIO_GENI_CFG_REG67_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG67_ADDR(x),m,v,HWIO_GENI_CFG_REG67_IN(x))
#define HWIO_GENI_CFG_REG67_M_AUTO_OPCODE_CANCEL_BMSK                                        0x8000000
#define HWIO_GENI_CFG_REG67_M_AUTO_OPCODE_CANCEL_SHFT                                             0x1b
#define HWIO_GENI_CFG_REG67_M_OPCODE_CMD_CANCEL_BMSK                                         0x7c00000
#define HWIO_GENI_CFG_REG67_M_OPCODE_CMD_CANCEL_SHFT                                              0x16
#define HWIO_GENI_CFG_REG67_DRIVE_DEFAULT_ON_START_EN_BMSK                                    0x100000
#define HWIO_GENI_CFG_REG67_DRIVE_DEFAULT_ON_START_EN_SHFT                                        0x14
#define HWIO_GENI_CFG_REG67_TX_BP_MODE_BMSK                                                    0x80000
#define HWIO_GENI_CFG_REG67_TX_BP_MODE_SHFT                                                       0x13
#define HWIO_GENI_CFG_REG67_TX_SOE_EDGE_SEL_BMSK                                               0x40000
#define HWIO_GENI_CFG_REG67_TX_SOE_EDGE_SEL_SHFT                                                  0x12
#define HWIO_GENI_CFG_REG67_TX_SOUT_EDGE_SEL_BMSK                                              0x20000
#define HWIO_GENI_CFG_REG67_TX_SOUT_EDGE_SEL_SHFT                                                 0x11
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOE_VALUE_BMSK                                     0x10000
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOE_VALUE_SHFT                                        0x10
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOUT_VALUE_BMSK                                     0x8000
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOUT_VALUE_SHFT                                        0xf
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_SOE_VALUE_BMSK                                           0x4000
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_SOE_VALUE_SHFT                                              0xe
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_SOUT_VALUE_BMSK                                          0x2000
#define HWIO_GENI_CFG_REG67_TX_DEFAULT_SOUT_VALUE_SHFT                                             0xd
#define HWIO_GENI_CFG_REG67_TX_CONST1_EFF_SIZE_BMSK                                             0x1c00
#define HWIO_GENI_CFG_REG67_TX_CONST1_EFF_SIZE_SHFT                                                0xa
#define HWIO_GENI_CFG_REG67_TX_CONST1_REG_BMSK                                                   0x3fc
#define HWIO_GENI_CFG_REG67_TX_CONST1_REG_SHFT                                                     0x2

#define HWIO_GENI_CFG_REG68_ADDR(x)                                                         ((x) + 0x00000110)
#define HWIO_GENI_CFG_REG68_OFFS                                                            (0x00000110)
#define HWIO_GENI_CFG_REG68_RMSK                                                            0xe03bfd3f
#define HWIO_GENI_CFG_REG68_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG68_ADDR(x), HWIO_GENI_CFG_REG68_RMSK)
#define HWIO_GENI_CFG_REG68_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG68_ADDR(x), m)
#define HWIO_GENI_CFG_REG68_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG68_ADDR(x),v)
#define HWIO_GENI_CFG_REG68_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG68_ADDR(x),m,v,HWIO_GENI_CFG_REG68_IN(x))
#define HWIO_GENI_CFG_REG68_M_TIME_CNT_FR_DIV_BMSK                                          0xc0000000
#define HWIO_GENI_CFG_REG68_M_TIME_CNT_FR_DIV_SHFT                                                0x1e
#define HWIO_GENI_CFG_REG68_M_TIME_CNT_EN_FR_SEL_BMSK                                       0x20000000
#define HWIO_GENI_CFG_REG68_M_TIME_CNT_EN_FR_SEL_SHFT                                             0x1d
#define HWIO_GENI_CFG_REG68_M_GP_SYNC_CANCEL_CLEAR_BMSK                                       0x200000
#define HWIO_GENI_CFG_REG68_M_GP_SYNC_CANCEL_CLEAR_SHFT                                           0x15
#define HWIO_GENI_CFG_REG68_M_GP_CNT_DIRECTION_BMSK                                           0x100000
#define HWIO_GENI_CFG_REG68_M_GP_CNT_DIRECTION_SHFT                                               0x14
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_START_VAL_BMSK                                       0x80000
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_START_VAL_SHFT                                          0x13
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_STOP_DUR_BMSK                                        0x30000
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_STOP_DUR_SHFT                                           0x10
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_EN_BMSK                                               0x8000
#define HWIO_GENI_CFG_REG68_TX_SOUT_MODUL_EN_SHFT                                                  0xf
#define HWIO_GENI_CFG_REG68_TX_LOAD_PAR_EN_BMSK                                                 0x4000
#define HWIO_GENI_CFG_REG68_TX_LOAD_PAR_EN_SHFT                                                    0xe
#define HWIO_GENI_CFG_REG68_TX_PARAM_PAR_EN_BMSK                                                0x1000
#define HWIO_GENI_CFG_REG68_TX_PARAM_PAR_EN_SHFT                                                   0xc
#define HWIO_GENI_CFG_REG68_TX_FIFO_PAR_EN_BMSK                                                  0x400
#define HWIO_GENI_CFG_REG68_TX_FIFO_PAR_EN_SHFT                                                    0xa
#define HWIO_GENI_CFG_REG68_IO2_CONST_EFF_SIZE_BMSK                                              0x100
#define HWIO_GENI_CFG_REG68_IO2_CONST_EFF_SIZE_SHFT                                                0x8
#define HWIO_GENI_CFG_REG68_IO2_CONST_REG_BMSK                                                    0x30
#define HWIO_GENI_CFG_REG68_IO2_CONST_REG_SHFT                                                     0x4
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOE_VALUE_BMSK                                        0x8
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOE_VALUE_SHFT                                        0x3
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOUT_VALUE_BMSK                                       0x4
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOUT_VALUE_SHFT                                       0x2
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_SOE_VALUE_BMSK                                             0x2
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_SOE_VALUE_SHFT                                             0x1
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_SOUT_VALUE_BMSK                                            0x1
#define HWIO_GENI_CFG_REG68_IO2_DEFAULT_SOUT_VALUE_SHFT                                            0x0

#define HWIO_GENI_CFG_REG69_ADDR(x)                                                         ((x) + 0x00000114)
#define HWIO_GENI_CFG_REG69_OFFS                                                            (0x00000114)
#define HWIO_GENI_CFG_REG69_RMSK                                                            0x4ec03ff1
#define HWIO_GENI_CFG_REG69_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG69_ADDR(x), HWIO_GENI_CFG_REG69_RMSK)
#define HWIO_GENI_CFG_REG69_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG69_ADDR(x), m)
#define HWIO_GENI_CFG_REG69_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG69_ADDR(x),v)
#define HWIO_GENI_CFG_REG69_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG69_ADDR(x),m,v,HWIO_GENI_CFG_REG69_IN(x))
#define HWIO_GENI_CFG_REG69_GENI_RX_MODE_BMSK                                               0x40000000
#define HWIO_GENI_CFG_REG69_GENI_RX_MODE_SHFT                                                     0x1e
#define HWIO_GENI_CFG_REG69_RX_IO_SIN_SEL_BMSK                                               0xe000000
#define HWIO_GENI_CFG_REG69_RX_IO_SIN_SEL_SHFT                                                    0x19
#define HWIO_GENI_CFG_REG69_RX_SIN_SEL_BMSK                                                   0xc00000
#define HWIO_GENI_CFG_REG69_RX_SIN_SEL_SHFT                                                       0x16
#define HWIO_GENI_CFG_REG69_S_GP_SYNC_CANCEL_CLEAR_BMSK                                         0x2000
#define HWIO_GENI_CFG_REG69_S_GP_SYNC_CANCEL_CLEAR_SHFT                                            0xd
#define HWIO_GENI_CFG_REG69_SW_FLUSH_SYNC_EN_BMSK                                               0x1000
#define HWIO_GENI_CFG_REG69_SW_FLUSH_SYNC_EN_SHFT                                                  0xc
#define HWIO_GENI_CFG_REG69_AUTO_SYNC_FIFO_CLEAR_BMSK                                            0x800
#define HWIO_GENI_CFG_REG69_AUTO_SYNC_FIFO_CLEAR_SHFT                                              0xb
#define HWIO_GENI_CFG_REG69_LAST_IS_DONE_BMSK                                                    0x400
#define HWIO_GENI_CFG_REG69_LAST_IS_DONE_SHFT                                                      0xa
#define HWIO_GENI_CFG_REG69_S_AUTO_OPCODE_CANCEL_BMSK                                            0x200
#define HWIO_GENI_CFG_REG69_S_AUTO_OPCODE_CANCEL_SHFT                                              0x9
#define HWIO_GENI_CFG_REG69_S_OPCODE_CMD_CANCEL_BMSK                                             0x1f0
#define HWIO_GENI_CFG_REG69_S_OPCODE_CMD_CANCEL_SHFT                                               0x4
#define HWIO_GENI_CFG_REG69_S_DATA_CNT_MODE_BMSK                                                   0x1
#define HWIO_GENI_CFG_REG69_S_DATA_CNT_MODE_SHFT                                                   0x0

#define HWIO_GENI_CFG_REG70_ADDR(x)                                                         ((x) + 0x00000118)
#define HWIO_GENI_CFG_REG70_OFFS                                                            (0x00000118)
#define HWIO_GENI_CFG_REG70_RMSK                                                                 0x401
#define HWIO_GENI_CFG_REG70_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG70_ADDR(x), HWIO_GENI_CFG_REG70_RMSK)
#define HWIO_GENI_CFG_REG70_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG70_ADDR(x), m)
#define HWIO_GENI_CFG_REG70_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG70_ADDR(x),v)
#define HWIO_GENI_CFG_REG70_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG70_ADDR(x),m,v,HWIO_GENI_CFG_REG70_IN(x))
#define HWIO_GENI_CFG_REG70_S_GP_CNT_DIRECTION_BMSK                                              0x400
#define HWIO_GENI_CFG_REG70_S_GP_CNT_DIRECTION_SHFT                                                0xa
#define HWIO_GENI_CFG_REG70_S_PROG_RAM_SEC_WORD_EN_BMSK                                            0x1
#define HWIO_GENI_CFG_REG70_S_PROG_RAM_SEC_WORD_EN_SHFT                                            0x0

#define HWIO_GENI_CFG_REG71_ADDR(x)                                                         ((x) + 0x0000011c)
#define HWIO_GENI_CFG_REG71_OFFS                                                            (0x0000011c)
#define HWIO_GENI_CFG_REG71_RMSK                                                            0x3fffffff
#define HWIO_GENI_CFG_REG71_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG71_ADDR(x), HWIO_GENI_CFG_REG71_RMSK)
#define HWIO_GENI_CFG_REG71_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG71_ADDR(x), m)
#define HWIO_GENI_CFG_REG71_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG71_ADDR(x),v)
#define HWIO_GENI_CFG_REG71_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG71_ADDR(x),m,v,HWIO_GENI_CFG_REG71_IN(x))
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_EXT_BMSK                             0x3ff00000
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_EXT_SHFT                                   0x14
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_BMSK                                    0xffc00
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_SHFT                                        0xa
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_VEC_BMSK                                                0x3ff
#define HWIO_GENI_CFG_REG71_COND_COMP_IN_VEC_SHFT                                                  0x0

#define HWIO_GENI_CFG_REG72_ADDR(x)                                                         ((x) + 0x00000120)
#define HWIO_GENI_CFG_REG72_OFFS                                                            (0x00000120)
#define HWIO_GENI_CFG_REG72_RMSK                                                            0x3fffffff
#define HWIO_GENI_CFG_REG72_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG72_ADDR(x), HWIO_GENI_CFG_REG72_RMSK)
#define HWIO_GENI_CFG_REG72_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG72_ADDR(x), m)
#define HWIO_GENI_CFG_REG72_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG72_ADDR(x),v)
#define HWIO_GENI_CFG_REG72_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG72_ADDR(x),m,v,HWIO_GENI_CFG_REG72_IN(x))
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_EXT_BMSK                           0x3ff00000
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_EXT_SHFT                                 0x14
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_BMSK                                  0xffc00
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_SHFT                                      0xa
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_VEC_BMSK                                              0x3ff
#define HWIO_GENI_CFG_REG72_S_COND_COMP_IN_VEC_SHFT                                                0x0

#define HWIO_GENI_CFG_REG73_ADDR(x)                                                         ((x) + 0x00000124)
#define HWIO_GENI_CFG_REG73_OFFS                                                            (0x00000124)
#define HWIO_GENI_CFG_REG73_RMSK                                                                   0xf
#define HWIO_GENI_CFG_REG73_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG73_ADDR(x), HWIO_GENI_CFG_REG73_RMSK)
#define HWIO_GENI_CFG_REG73_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG73_ADDR(x), m)
#define HWIO_GENI_CFG_REG73_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG73_ADDR(x),v)
#define HWIO_GENI_CFG_REG73_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG73_ADDR(x),m,v,HWIO_GENI_CFG_REG73_IN(x))
#define HWIO_GENI_CFG_REG73_SOUT_2D_SEL_BMSK                                                       0xf
#define HWIO_GENI_CFG_REG73_SOUT_2D_SEL_SHFT                                                       0x0

#define HWIO_GENI_CFG_REG74_ADDR(x)                                                         ((x) + 0x00000128)
#define HWIO_GENI_CFG_REG74_OFFS                                                            (0x00000128)
#define HWIO_GENI_CFG_REG74_RMSK                                                                   0x1
#define HWIO_GENI_CFG_REG74_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG74_ADDR(x), HWIO_GENI_CFG_REG74_RMSK)
#define HWIO_GENI_CFG_REG74_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG74_ADDR(x), m)
#define HWIO_GENI_CFG_REG74_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG74_ADDR(x),v)
#define HWIO_GENI_CFG_REG74_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG74_ADDR(x),m,v,HWIO_GENI_CFG_REG74_IN(x))
#define HWIO_GENI_CFG_REG74_NOT_USED_BMSK                                                          0x1
#define HWIO_GENI_CFG_REG74_NOT_USED_SHFT                                                          0x0

#define HWIO_GENI_CFG_REG75_ADDR(x)                                                         ((x) + 0x0000012c)
#define HWIO_GENI_CFG_REG75_OFFS                                                            (0x0000012c)
#define HWIO_GENI_CFG_REG75_RMSK                                                                   0x3
#define HWIO_GENI_CFG_REG75_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG75_ADDR(x), HWIO_GENI_CFG_REG75_RMSK)
#define HWIO_GENI_CFG_REG75_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG75_ADDR(x), m)
#define HWIO_GENI_CFG_REG75_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG75_ADDR(x),v)
#define HWIO_GENI_CFG_REG75_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG75_ADDR(x),m,v,HWIO_GENI_CFG_REG75_IN(x))
#define HWIO_GENI_CFG_REG75_LOOPBACK_MODE_BMSK                                                     0x3
#define HWIO_GENI_CFG_REG75_LOOPBACK_MODE_SHFT                                                     0x0

#define HWIO_GENI_CFG_REG76_ADDR(x)                                                         ((x) + 0x00000130)
#define HWIO_GENI_CFG_REG76_OFFS                                                            (0x00000130)
#define HWIO_GENI_CFG_REG76_RMSK                                                                   0x7
#define HWIO_GENI_CFG_REG76_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG76_ADDR(x), HWIO_GENI_CFG_REG76_RMSK)
#define HWIO_GENI_CFG_REG76_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG76_ADDR(x), m)
#define HWIO_GENI_CFG_REG76_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG76_ADDR(x),v)
#define HWIO_GENI_CFG_REG76_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG76_ADDR(x),m,v,HWIO_GENI_CFG_REG76_IN(x))
#define HWIO_GENI_CFG_REG76_IO2_ENGINE_INV_EN_BMSK                                                 0x4
#define HWIO_GENI_CFG_REG76_IO2_ENGINE_INV_EN_SHFT                                                 0x2
#define HWIO_GENI_CFG_REG76_RX_ENGINE_INV_EN_BMSK                                                  0x2
#define HWIO_GENI_CFG_REG76_RX_ENGINE_INV_EN_SHFT                                                  0x1
#define HWIO_GENI_CFG_REG76_TX_ENGINE_INV_EN_BMSK                                                  0x1
#define HWIO_GENI_CFG_REG76_TX_ENGINE_INV_EN_SHFT                                                  0x0

#define HWIO_GENI_CFG_REG77_ADDR(x)                                                         ((x) + 0x00000134)
#define HWIO_GENI_CFG_REG77_OFFS                                                            (0x00000134)
#define HWIO_GENI_CFG_REG77_RMSK                                                                  0x1f
#define HWIO_GENI_CFG_REG77_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG77_ADDR(x), HWIO_GENI_CFG_REG77_RMSK)
#define HWIO_GENI_CFG_REG77_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG77_ADDR(x), m)
#define HWIO_GENI_CFG_REG77_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG77_ADDR(x),v)
#define HWIO_GENI_CFG_REG77_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG77_ADDR(x),m,v,HWIO_GENI_CFG_REG77_IN(x))
#define HWIO_GENI_CFG_REG77_SDA_NOISE_REJECT_BMSK                                                 0x18
#define HWIO_GENI_CFG_REG77_SDA_NOISE_REJECT_SHFT                                                  0x3
#define HWIO_GENI_CFG_REG77_SCL_NOISE_REJECT_BMSK                                                  0x6
#define HWIO_GENI_CFG_REG77_SCL_NOISE_REJECT_SHFT                                                  0x1
#define HWIO_GENI_CFG_REG77_LOW_PERIOD_NOISE_REJECT_EN_BMSK                                        0x1
#define HWIO_GENI_CFG_REG77_LOW_PERIOD_NOISE_REJECT_EN_SHFT                                        0x0

#define HWIO_GENI_CFG_REG78_ADDR(x)                                                         ((x) + 0x00000138)
#define HWIO_GENI_CFG_REG78_OFFS                                                            (0x00000138)
#define HWIO_GENI_CFG_REG78_RMSK                                                                   0x3
#define HWIO_GENI_CFG_REG78_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG78_ADDR(x), HWIO_GENI_CFG_REG78_RMSK)
#define HWIO_GENI_CFG_REG78_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG78_ADDR(x), m)
#define HWIO_GENI_CFG_REG78_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG78_ADDR(x),v)
#define HWIO_GENI_CFG_REG78_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG78_ADDR(x),m,v,HWIO_GENI_CFG_REG78_IN(x))
#define HWIO_GENI_CFG_REG78_BUSY_INDICATION_SELECT_BMSK                                            0x2
#define HWIO_GENI_CFG_REG78_BUSY_INDICATION_SELECT_SHFT                                            0x1
#define HWIO_GENI_CFG_REG78_SDA_DELAYED_DETECTION_BMSK                                             0x1
#define HWIO_GENI_CFG_REG78_SDA_DELAYED_DETECTION_SHFT                                             0x0

#define HWIO_GENI_CFG_REG79_ADDR(x)                                                         ((x) + 0x0000013c)
#define HWIO_GENI_CFG_REG79_OFFS                                                            (0x0000013c)
#define HWIO_GENI_CFG_REG79_RMSK                                                                   0x1
#define HWIO_GENI_CFG_REG79_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG79_ADDR(x), HWIO_GENI_CFG_REG79_RMSK)
#define HWIO_GENI_CFG_REG79_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG79_ADDR(x), m)
#define HWIO_GENI_CFG_REG79_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG79_ADDR(x),v)
#define HWIO_GENI_CFG_REG79_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG79_ADDR(x),m,v,HWIO_GENI_CFG_REG79_IN(x))
#define HWIO_GENI_CFG_REG79_IO3_S_FE_FORCE_BMSK                                                    0x1
#define HWIO_GENI_CFG_REG79_IO3_S_FE_FORCE_SHFT                                                    0x0

#define HWIO_GENI_CFG_REG80_ADDR(x)                                                         ((x) + 0x00000140)
#define HWIO_GENI_CFG_REG80_OFFS                                                            (0x00000140)
#define HWIO_GENI_CFG_REG80_RMSK                                                                   0xf
#define HWIO_GENI_CFG_REG80_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG80_ADDR(x), HWIO_GENI_CFG_REG80_RMSK)
#define HWIO_GENI_CFG_REG80_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG80_ADDR(x), m)
#define HWIO_GENI_CFG_REG80_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG80_ADDR(x),v)
#define HWIO_GENI_CFG_REG80_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG80_ADDR(x),m,v,HWIO_GENI_CFG_REG80_IN(x))
#define HWIO_GENI_CFG_REG80_IO_MACRO_MUX_SEL_BMSK                                                  0xf
#define HWIO_GENI_CFG_REG80_IO_MACRO_MUX_SEL_SHFT                                                  0x0

#define HWIO_GENI_CFG_REG81_ADDR(x)                                                         ((x) + 0x00000144)
#define HWIO_GENI_CFG_REG81_OFFS                                                            (0x00000144)
#define HWIO_GENI_CFG_REG81_RMSK                                                                   0xf
#define HWIO_GENI_CFG_REG81_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG81_ADDR(x), HWIO_GENI_CFG_REG81_RMSK)
#define HWIO_GENI_CFG_REG81_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG81_ADDR(x), m)
#define HWIO_GENI_CFG_REG81_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG81_ADDR(x),v)
#define HWIO_GENI_CFG_REG81_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG81_ADDR(x),m,v,HWIO_GENI_CFG_REG81_IN(x))
#define HWIO_GENI_CFG_REG81_GENI_IO3_SOUT2_PULL_UP_EN_BMSK                                         0x8
#define HWIO_GENI_CFG_REG81_GENI_IO3_SOUT2_PULL_UP_EN_SHFT                                         0x3
#define HWIO_GENI_CFG_REG81_GENI_IO3_SOUT0_PULL_UP_EN_BMSK                                         0x4
#define HWIO_GENI_CFG_REG81_GENI_IO3_SOUT0_PULL_UP_EN_SHFT                                         0x2
#define HWIO_GENI_CFG_REG81_GENI_SOUT2_PULL_UP_EN_BMSK                                             0x2
#define HWIO_GENI_CFG_REG81_GENI_SOUT2_PULL_UP_EN_SHFT                                             0x1
#define HWIO_GENI_CFG_REG81_GENI_SOUT0_PULL_UP_EN_BMSK                                             0x1
#define HWIO_GENI_CFG_REG81_GENI_SOUT0_PULL_UP_EN_SHFT                                             0x0

#define HWIO_GENI_CFG_REG82_ADDR(x)                                                         ((x) + 0x00000148)
#define HWIO_GENI_CFG_REG82_OFFS                                                            (0x00000148)
#define HWIO_GENI_CFG_REG82_RMSK                                                                   0xf
#define HWIO_GENI_CFG_REG82_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG82_ADDR(x), HWIO_GENI_CFG_REG82_RMSK)
#define HWIO_GENI_CFG_REG82_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG82_ADDR(x), m)
#define HWIO_GENI_CFG_REG82_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG82_ADDR(x),v)
#define HWIO_GENI_CFG_REG82_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG82_ADDR(x),m,v,HWIO_GENI_CFG_REG82_IN(x))
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOE_VALUE_BMSK                                        0x8
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOE_VALUE_SHFT                                        0x3
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOUT_VALUE_BMSK                                       0x4
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOUT_VALUE_SHFT                                       0x2
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_SOE_VALUE_BMSK                                             0x2
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_SOE_VALUE_SHFT                                             0x1
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_SOUT_VALUE_BMSK                                            0x1
#define HWIO_GENI_CFG_REG82_IO3_DEFAULT_SOUT_VALUE_SHFT                                            0x0

#define HWIO_GENI_CFG_REG83_ADDR(x)                                                         ((x) + 0x0000014c)
#define HWIO_GENI_CFG_REG83_OFFS                                                            (0x0000014c)
#define HWIO_GENI_CFG_REG83_RMSK                                                                   0xf
#define HWIO_GENI_CFG_REG83_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG83_ADDR(x), HWIO_GENI_CFG_REG83_RMSK)
#define HWIO_GENI_CFG_REG83_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG83_ADDR(x), m)
#define HWIO_GENI_CFG_REG83_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG83_ADDR(x),v)
#define HWIO_GENI_CFG_REG83_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG83_ADDR(x),m,v,HWIO_GENI_CFG_REG83_IN(x))
#define HWIO_GENI_CFG_REG83_IO3_DEMUX_OUTPUT_INV_BMSK                                              0xf
#define HWIO_GENI_CFG_REG83_IO3_DEMUX_OUTPUT_INV_SHFT                                              0x0

#define HWIO_GENI_CFG_REG84_ADDR(x)                                                         ((x) + 0x00000150)
#define HWIO_GENI_CFG_REG84_OFFS                                                            (0x00000150)
#define HWIO_GENI_CFG_REG84_RMSK                                                                  0x1f
#define HWIO_GENI_CFG_REG84_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG84_ADDR(x), HWIO_GENI_CFG_REG84_RMSK)
#define HWIO_GENI_CFG_REG84_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG84_ADDR(x), m)
#define HWIO_GENI_CFG_REG84_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG84_ADDR(x),v)
#define HWIO_GENI_CFG_REG84_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG84_ADDR(x),m,v,HWIO_GENI_CFG_REG84_IN(x))
#define HWIO_GENI_CFG_REG84_IO3_DEMUX_FW_EN_BMSK                                                  0x10
#define HWIO_GENI_CFG_REG84_IO3_DEMUX_FW_EN_SHFT                                                   0x4
#define HWIO_GENI_CFG_REG84_IO3_DEMUX_SEL_BMSK                                                     0xf
#define HWIO_GENI_CFG_REG84_IO3_DEMUX_SEL_SHFT                                                     0x0

#define HWIO_GENI_CFG_REG85_ADDR(x)                                                         ((x) + 0x00000154)
#define HWIO_GENI_CFG_REG85_OFFS                                                            (0x00000154)
#define HWIO_GENI_CFG_REG85_RMSK                                                                   0x3
#define HWIO_GENI_CFG_REG85_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG85_ADDR(x), HWIO_GENI_CFG_REG85_RMSK)
#define HWIO_GENI_CFG_REG85_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG85_ADDR(x), m)
#define HWIO_GENI_CFG_REG85_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG85_ADDR(x),v)
#define HWIO_GENI_CFG_REG85_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG85_ADDR(x),m,v,HWIO_GENI_CFG_REG85_IN(x))
#define HWIO_GENI_CFG_REG85_GENI_BYTE_GRANULARITY_BMSK                                             0x3
#define HWIO_GENI_CFG_REG85_GENI_BYTE_GRANULARITY_SHFT                                             0x0

#define HWIO_GENI_CFG_REG86_ADDR(x)                                                         ((x) + 0x00000158)
#define HWIO_GENI_CFG_REG86_OFFS                                                            (0x00000158)
#define HWIO_GENI_CFG_REG86_RMSK                                                                   0x1
#define HWIO_GENI_CFG_REG86_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG86_ADDR(x), HWIO_GENI_CFG_REG86_RMSK)
#define HWIO_GENI_CFG_REG86_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG86_ADDR(x), m)
#define HWIO_GENI_CFG_REG86_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG86_ADDR(x),v)
#define HWIO_GENI_CFG_REG86_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG86_ADDR(x),m,v,HWIO_GENI_CFG_REG86_IN(x))
#define HWIO_GENI_CFG_REG86_GENI_DMA_MODE_EN_BMSK                                                  0x1
#define HWIO_GENI_CFG_REG86_GENI_DMA_MODE_EN_SHFT                                                  0x0

#define HWIO_GENI_CFG_REG87_ADDR(x)                                                         ((x) + 0x0000015c)
#define HWIO_GENI_CFG_REG87_OFFS                                                            (0x0000015c)
#define HWIO_GENI_CFG_REG87_RMSK                                                                  0x3f
#define HWIO_GENI_CFG_REG87_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG87_ADDR(x), HWIO_GENI_CFG_REG87_RMSK)
#define HWIO_GENI_CFG_REG87_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG87_ADDR(x), m)
#define HWIO_GENI_CFG_REG87_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG87_ADDR(x),v)
#define HWIO_GENI_CFG_REG87_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG87_ADDR(x),m,v,HWIO_GENI_CFG_REG87_IN(x))
#define HWIO_GENI_CFG_REG87_M_SW_COMP5_BMSK                                                       0x20
#define HWIO_GENI_CFG_REG87_M_SW_COMP5_SHFT                                                        0x5
#define HWIO_GENI_CFG_REG87_M_SW_COMP4_BMSK                                                       0x10
#define HWIO_GENI_CFG_REG87_M_SW_COMP4_SHFT                                                        0x4
#define HWIO_GENI_CFG_REG87_M_SW_COMP3_BMSK                                                        0x8
#define HWIO_GENI_CFG_REG87_M_SW_COMP3_SHFT                                                        0x3
#define HWIO_GENI_CFG_REG87_M_SW_COMP2_BMSK                                                        0x4
#define HWIO_GENI_CFG_REG87_M_SW_COMP2_SHFT                                                        0x2
#define HWIO_GENI_CFG_REG87_M_SW_COMP1_BMSK                                                        0x2
#define HWIO_GENI_CFG_REG87_M_SW_COMP1_SHFT                                                        0x1
#define HWIO_GENI_CFG_REG87_M_SW_COMP0_BMSK                                                        0x1
#define HWIO_GENI_CFG_REG87_M_SW_COMP0_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG88_ADDR(x)                                                         ((x) + 0x00000160)
#define HWIO_GENI_CFG_REG88_OFFS                                                            (0x00000160)
#define HWIO_GENI_CFG_REG88_RMSK                                                               0xfffff
#define HWIO_GENI_CFG_REG88_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG88_ADDR(x), HWIO_GENI_CFG_REG88_RMSK)
#define HWIO_GENI_CFG_REG88_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG88_ADDR(x), m)
#define HWIO_GENI_CFG_REG88_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG88_ADDR(x),v)
#define HWIO_GENI_CFG_REG88_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG88_ADDR(x),m,v,HWIO_GENI_CFG_REG88_IN(x))
#define HWIO_GENI_CFG_REG88_TX_DATA_TABLE_VEC_1_BMSK                                           0xffc00
#define HWIO_GENI_CFG_REG88_TX_DATA_TABLE_VEC_1_SHFT                                               0xa
#define HWIO_GENI_CFG_REG88_TX_DATA_TABLE_VEC_0_BMSK                                             0x3ff
#define HWIO_GENI_CFG_REG88_TX_DATA_TABLE_VEC_0_SHFT                                               0x0

#define HWIO_GENI_CFG_REG89_ADDR(x)                                                         ((x) + 0x00000164)
#define HWIO_GENI_CFG_REG89_OFFS                                                            (0x00000164)
#define HWIO_GENI_CFG_REG89_RMSK                                                               0xfffff
#define HWIO_GENI_CFG_REG89_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG89_ADDR(x), HWIO_GENI_CFG_REG89_RMSK)
#define HWIO_GENI_CFG_REG89_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG89_ADDR(x), m)
#define HWIO_GENI_CFG_REG89_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG89_ADDR(x),v)
#define HWIO_GENI_CFG_REG89_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG89_ADDR(x),m,v,HWIO_GENI_CFG_REG89_IN(x))
#define HWIO_GENI_CFG_REG89_TX_DATA_TABLE_VEC_3_BMSK                                           0xffc00
#define HWIO_GENI_CFG_REG89_TX_DATA_TABLE_VEC_3_SHFT                                               0xa
#define HWIO_GENI_CFG_REG89_TX_DATA_TABLE_VEC_2_BMSK                                             0x3ff
#define HWIO_GENI_CFG_REG89_TX_DATA_TABLE_VEC_2_SHFT                                               0x0

#define HWIO_GENI_CFG_REG90_ADDR(x)                                                         ((x) + 0x00000168)
#define HWIO_GENI_CFG_REG90_OFFS                                                            (0x00000168)
#define HWIO_GENI_CFG_REG90_RMSK                                                                 0x3ff
#define HWIO_GENI_CFG_REG90_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG90_ADDR(x), HWIO_GENI_CFG_REG90_RMSK)
#define HWIO_GENI_CFG_REG90_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG90_ADDR(x), m)
#define HWIO_GENI_CFG_REG90_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG90_ADDR(x),v)
#define HWIO_GENI_CFG_REG90_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG90_ADDR(x),m,v,HWIO_GENI_CFG_REG90_IN(x))
#define HWIO_GENI_CFG_REG90_M_GP_CNT0_BMSK                                                       0x3ff
#define HWIO_GENI_CFG_REG90_M_GP_CNT0_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG91_ADDR(x)                                                         ((x) + 0x0000016c)
#define HWIO_GENI_CFG_REG91_OFFS                                                            (0x0000016c)
#define HWIO_GENI_CFG_REG91_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG91_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG91_ADDR(x), HWIO_GENI_CFG_REG91_RMSK)
#define HWIO_GENI_CFG_REG91_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG91_ADDR(x), m)
#define HWIO_GENI_CFG_REG91_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG91_ADDR(x),v)
#define HWIO_GENI_CFG_REG91_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG91_ADDR(x),m,v,HWIO_GENI_CFG_REG91_IN(x))
#define HWIO_GENI_CFG_REG91_M_GP_CNT1_BMSK                                                    0xffffff
#define HWIO_GENI_CFG_REG91_M_GP_CNT1_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG92_ADDR(x)                                                         ((x) + 0x00000170)
#define HWIO_GENI_CFG_REG92_OFFS                                                            (0x00000170)
#define HWIO_GENI_CFG_REG92_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG92_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG92_ADDR(x), HWIO_GENI_CFG_REG92_RMSK)
#define HWIO_GENI_CFG_REG92_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG92_ADDR(x), m)
#define HWIO_GENI_CFG_REG92_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG92_ADDR(x),v)
#define HWIO_GENI_CFG_REG92_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG92_ADDR(x),m,v,HWIO_GENI_CFG_REG92_IN(x))
#define HWIO_GENI_CFG_REG92_M_GP_CNT2_BMSK                                                    0xffffff
#define HWIO_GENI_CFG_REG92_M_GP_CNT2_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG93_ADDR(x)                                                         ((x) + 0x00000174)
#define HWIO_GENI_CFG_REG93_OFFS                                                            (0x00000174)
#define HWIO_GENI_CFG_REG93_RMSK                                                              0xffffff
#define HWIO_GENI_CFG_REG93_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG93_ADDR(x), HWIO_GENI_CFG_REG93_RMSK)
#define HWIO_GENI_CFG_REG93_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG93_ADDR(x), m)
#define HWIO_GENI_CFG_REG93_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG93_ADDR(x),v)
#define HWIO_GENI_CFG_REG93_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG93_ADDR(x),m,v,HWIO_GENI_CFG_REG93_IN(x))
#define HWIO_GENI_CFG_REG93_M_GP_CNT3_BMSK                                                    0xffffff
#define HWIO_GENI_CFG_REG93_M_GP_CNT3_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG94_ADDR(x)                                                         ((x) + 0x00000178)
#define HWIO_GENI_CFG_REG94_OFFS                                                            (0x00000178)
#define HWIO_GENI_CFG_REG94_RMSK                                                            0x3fffffff
#define HWIO_GENI_CFG_REG94_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG94_ADDR(x), HWIO_GENI_CFG_REG94_RMSK)
#define HWIO_GENI_CFG_REG94_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG94_ADDR(x), m)
#define HWIO_GENI_CFG_REG94_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG94_ADDR(x),v)
#define HWIO_GENI_CFG_REG94_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG94_ADDR(x),m,v,HWIO_GENI_CFG_REG94_IN(x))
#define HWIO_GENI_CFG_REG94_M_GP_CNT6_BMSK                                                  0x3ff00000
#define HWIO_GENI_CFG_REG94_M_GP_CNT6_SHFT                                                        0x14
#define HWIO_GENI_CFG_REG94_M_GP_CNT5_BMSK                                                     0xffc00
#define HWIO_GENI_CFG_REG94_M_GP_CNT5_SHFT                                                         0xa
#define HWIO_GENI_CFG_REG94_M_GP_CNT4_BMSK                                                       0x3ff
#define HWIO_GENI_CFG_REG94_M_GP_CNT4_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG95_ADDR(x)                                                         ((x) + 0x0000017c)
#define HWIO_GENI_CFG_REG95_OFFS                                                            (0x0000017c)
#define HWIO_GENI_CFG_REG95_RMSK                                                                 0x3ff
#define HWIO_GENI_CFG_REG95_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG95_ADDR(x), HWIO_GENI_CFG_REG95_RMSK)
#define HWIO_GENI_CFG_REG95_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG95_ADDR(x), m)
#define HWIO_GENI_CFG_REG95_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG95_ADDR(x),v)
#define HWIO_GENI_CFG_REG95_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG95_ADDR(x),m,v,HWIO_GENI_CFG_REG95_IN(x))
#define HWIO_GENI_CFG_REG95_M_GP_CNT7_BMSK                                                       0x3ff
#define HWIO_GENI_CFG_REG95_M_GP_CNT7_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG96_ADDR(x)                                                         ((x) + 0x00000180)
#define HWIO_GENI_CFG_REG96_OFFS                                                            (0x00000180)
#define HWIO_GENI_CFG_REG96_RMSK                                                                  0x3f
#define HWIO_GENI_CFG_REG96_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG96_ADDR(x), HWIO_GENI_CFG_REG96_RMSK)
#define HWIO_GENI_CFG_REG96_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG96_ADDR(x), m)
#define HWIO_GENI_CFG_REG96_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG96_ADDR(x),v)
#define HWIO_GENI_CFG_REG96_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG96_ADDR(x),m,v,HWIO_GENI_CFG_REG96_IN(x))
#define HWIO_GENI_CFG_REG96_S_SW_COMP5_BMSK                                                       0x20
#define HWIO_GENI_CFG_REG96_S_SW_COMP5_SHFT                                                        0x5
#define HWIO_GENI_CFG_REG96_S_SW_COMP4_BMSK                                                       0x10
#define HWIO_GENI_CFG_REG96_S_SW_COMP4_SHFT                                                        0x4
#define HWIO_GENI_CFG_REG96_S_SW_COMP3_BMSK                                                        0x8
#define HWIO_GENI_CFG_REG96_S_SW_COMP3_SHFT                                                        0x3
#define HWIO_GENI_CFG_REG96_S_SW_COMP2_BMSK                                                        0x4
#define HWIO_GENI_CFG_REG96_S_SW_COMP2_SHFT                                                        0x2
#define HWIO_GENI_CFG_REG96_S_SW_COMP1_BMSK                                                        0x2
#define HWIO_GENI_CFG_REG96_S_SW_COMP1_SHFT                                                        0x1
#define HWIO_GENI_CFG_REG96_S_SW_COMP0_BMSK                                                        0x1
#define HWIO_GENI_CFG_REG96_S_SW_COMP0_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG97_ADDR(x)                                                         ((x) + 0x00000184)
#define HWIO_GENI_CFG_REG97_OFFS                                                            (0x00000184)
#define HWIO_GENI_CFG_REG97_RMSK                                                               0xfffff
#define HWIO_GENI_CFG_REG97_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG97_ADDR(x), HWIO_GENI_CFG_REG97_RMSK)
#define HWIO_GENI_CFG_REG97_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG97_ADDR(x), m)
#define HWIO_GENI_CFG_REG97_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG97_ADDR(x),v)
#define HWIO_GENI_CFG_REG97_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG97_ADDR(x),m,v,HWIO_GENI_CFG_REG97_IN(x))
#define HWIO_GENI_CFG_REG97_RX_DATA_TABLE_VEC_1_BMSK                                           0xffc00
#define HWIO_GENI_CFG_REG97_RX_DATA_TABLE_VEC_1_SHFT                                               0xa
#define HWIO_GENI_CFG_REG97_RX_DATA_TABLE_VEC_0_BMSK                                             0x3ff
#define HWIO_GENI_CFG_REG97_RX_DATA_TABLE_VEC_0_SHFT                                               0x0

#define HWIO_GENI_CFG_REG98_ADDR(x)                                                         ((x) + 0x00000188)
#define HWIO_GENI_CFG_REG98_OFFS                                                            (0x00000188)
#define HWIO_GENI_CFG_REG98_RMSK                                                               0xfffff
#define HWIO_GENI_CFG_REG98_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG98_ADDR(x), HWIO_GENI_CFG_REG98_RMSK)
#define HWIO_GENI_CFG_REG98_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG98_ADDR(x), m)
#define HWIO_GENI_CFG_REG98_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG98_ADDR(x),v)
#define HWIO_GENI_CFG_REG98_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG98_ADDR(x),m,v,HWIO_GENI_CFG_REG98_IN(x))
#define HWIO_GENI_CFG_REG98_RX_DATA_TABLE_VEC_3_BMSK                                           0xffc00
#define HWIO_GENI_CFG_REG98_RX_DATA_TABLE_VEC_3_SHFT                                               0xa
#define HWIO_GENI_CFG_REG98_RX_DATA_TABLE_VEC_2_BMSK                                             0x3ff
#define HWIO_GENI_CFG_REG98_RX_DATA_TABLE_VEC_2_SHFT                                               0x0

#define HWIO_GENI_CFG_REG99_ADDR(x)                                                         ((x) + 0x0000018c)
#define HWIO_GENI_CFG_REG99_OFFS                                                            (0x0000018c)
#define HWIO_GENI_CFG_REG99_RMSK                                                                 0x3ff
#define HWIO_GENI_CFG_REG99_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG99_ADDR(x), HWIO_GENI_CFG_REG99_RMSK)
#define HWIO_GENI_CFG_REG99_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG99_ADDR(x), m)
#define HWIO_GENI_CFG_REG99_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG99_ADDR(x),v)
#define HWIO_GENI_CFG_REG99_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG99_ADDR(x),m,v,HWIO_GENI_CFG_REG99_IN(x))
#define HWIO_GENI_CFG_REG99_S_GP_CNT0_BMSK                                                       0x3ff
#define HWIO_GENI_CFG_REG99_S_GP_CNT0_SHFT                                                         0x0

#define HWIO_GENI_CFG_REG100_ADDR(x)                                                        ((x) + 0x00000190)
#define HWIO_GENI_CFG_REG100_OFFS                                                           (0x00000190)
#define HWIO_GENI_CFG_REG100_RMSK                                                             0xffffff
#define HWIO_GENI_CFG_REG100_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG100_ADDR(x), HWIO_GENI_CFG_REG100_RMSK)
#define HWIO_GENI_CFG_REG100_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG100_ADDR(x), m)
#define HWIO_GENI_CFG_REG100_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG100_ADDR(x),v)
#define HWIO_GENI_CFG_REG100_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG100_ADDR(x),m,v,HWIO_GENI_CFG_REG100_IN(x))
#define HWIO_GENI_CFG_REG100_S_GP_CNT1_BMSK                                                   0xffffff
#define HWIO_GENI_CFG_REG100_S_GP_CNT1_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG101_ADDR(x)                                                        ((x) + 0x00000194)
#define HWIO_GENI_CFG_REG101_OFFS                                                           (0x00000194)
#define HWIO_GENI_CFG_REG101_RMSK                                                             0xffffff
#define HWIO_GENI_CFG_REG101_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG101_ADDR(x), HWIO_GENI_CFG_REG101_RMSK)
#define HWIO_GENI_CFG_REG101_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG101_ADDR(x), m)
#define HWIO_GENI_CFG_REG101_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG101_ADDR(x),v)
#define HWIO_GENI_CFG_REG101_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG101_ADDR(x),m,v,HWIO_GENI_CFG_REG101_IN(x))
#define HWIO_GENI_CFG_REG101_S_GP_CNT2_BMSK                                                   0xffffff
#define HWIO_GENI_CFG_REG101_S_GP_CNT2_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG102_ADDR(x)                                                        ((x) + 0x00000198)
#define HWIO_GENI_CFG_REG102_OFFS                                                           (0x00000198)
#define HWIO_GENI_CFG_REG102_RMSK                                                             0xffffff
#define HWIO_GENI_CFG_REG102_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG102_ADDR(x), HWIO_GENI_CFG_REG102_RMSK)
#define HWIO_GENI_CFG_REG102_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG102_ADDR(x), m)
#define HWIO_GENI_CFG_REG102_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG102_ADDR(x),v)
#define HWIO_GENI_CFG_REG102_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG102_ADDR(x),m,v,HWIO_GENI_CFG_REG102_IN(x))
#define HWIO_GENI_CFG_REG102_S_GP_CNT3_BMSK                                                   0xffffff
#define HWIO_GENI_CFG_REG102_S_GP_CNT3_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG103_ADDR(x)                                                        ((x) + 0x0000019c)
#define HWIO_GENI_CFG_REG103_OFFS                                                           (0x0000019c)
#define HWIO_GENI_CFG_REG103_RMSK                                                           0x3fffffff
#define HWIO_GENI_CFG_REG103_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG103_ADDR(x), HWIO_GENI_CFG_REG103_RMSK)
#define HWIO_GENI_CFG_REG103_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG103_ADDR(x), m)
#define HWIO_GENI_CFG_REG103_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG103_ADDR(x),v)
#define HWIO_GENI_CFG_REG103_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG103_ADDR(x),m,v,HWIO_GENI_CFG_REG103_IN(x))
#define HWIO_GENI_CFG_REG103_S_GP_CNT6_BMSK                                                 0x3ff00000
#define HWIO_GENI_CFG_REG103_S_GP_CNT6_SHFT                                                       0x14
#define HWIO_GENI_CFG_REG103_S_GP_CNT5_BMSK                                                    0xffc00
#define HWIO_GENI_CFG_REG103_S_GP_CNT5_SHFT                                                        0xa
#define HWIO_GENI_CFG_REG103_S_GP_CNT4_BMSK                                                      0x3ff
#define HWIO_GENI_CFG_REG103_S_GP_CNT4_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG104_ADDR(x)                                                        ((x) + 0x000001a0)
#define HWIO_GENI_CFG_REG104_OFFS                                                           (0x000001a0)
#define HWIO_GENI_CFG_REG104_RMSK                                                                0x3ff
#define HWIO_GENI_CFG_REG104_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG104_ADDR(x), HWIO_GENI_CFG_REG104_RMSK)
#define HWIO_GENI_CFG_REG104_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG104_ADDR(x), m)
#define HWIO_GENI_CFG_REG104_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG104_ADDR(x),v)
#define HWIO_GENI_CFG_REG104_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG104_ADDR(x),m,v,HWIO_GENI_CFG_REG104_IN(x))
#define HWIO_GENI_CFG_REG104_S_GP_CNT7_BMSK                                                      0x3ff
#define HWIO_GENI_CFG_REG104_S_GP_CNT7_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG105_ADDR(x)                                                        ((x) + 0x000001a4)
#define HWIO_GENI_CFG_REG105_OFFS                                                           (0x000001a4)
#define HWIO_GENI_CFG_REG105_RMSK                                                                 0x1f
#define HWIO_GENI_CFG_REG105_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG105_ADDR(x), HWIO_GENI_CFG_REG105_RMSK)
#define HWIO_GENI_CFG_REG105_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG105_ADDR(x), m)
#define HWIO_GENI_CFG_REG105_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG105_ADDR(x),v)
#define HWIO_GENI_CFG_REG105_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG105_ADDR(x),m,v,HWIO_GENI_CFG_REG105_IN(x))
#define HWIO_GENI_CFG_REG105_TX_PARITY_TOGGLE_EN_BMSK                                             0x10
#define HWIO_GENI_CFG_REG105_TX_PARITY_TOGGLE_EN_SHFT                                              0x4
#define HWIO_GENI_CFG_REG105_TX_FW_PAR_SEL_BMSK                                                    0x8
#define HWIO_GENI_CFG_REG105_TX_FW_PAR_SEL_SHFT                                                    0x3
#define HWIO_GENI_CFG_REG105_TX_PAR_MODE_BMSK                                                      0x6
#define HWIO_GENI_CFG_REG105_TX_PAR_MODE_SHFT                                                      0x1
#define HWIO_GENI_CFG_REG105_TX_PAR_CALC_EN_BMSK                                                   0x1
#define HWIO_GENI_CFG_REG105_TX_PAR_CALC_EN_SHFT                                                   0x0

#define HWIO_GENI_CFG_REG106_ADDR(x)                                                        ((x) + 0x000001a8)
#define HWIO_GENI_CFG_REG106_OFFS                                                           (0x000001a8)
#define HWIO_GENI_CFG_REG106_RMSK                                                                 0x1f
#define HWIO_GENI_CFG_REG106_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG106_ADDR(x), HWIO_GENI_CFG_REG106_RMSK)
#define HWIO_GENI_CFG_REG106_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG106_ADDR(x), m)
#define HWIO_GENI_CFG_REG106_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG106_ADDR(x),v)
#define HWIO_GENI_CFG_REG106_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG106_ADDR(x),m,v,HWIO_GENI_CFG_REG106_IN(x))
#define HWIO_GENI_CFG_REG106_RX_PARITY_TOGGLE_EN_BMSK                                             0x10
#define HWIO_GENI_CFG_REG106_RX_PARITY_TOGGLE_EN_SHFT                                              0x4
#define HWIO_GENI_CFG_REG106_RX_FW_PAR_SEL_BMSK                                                    0x8
#define HWIO_GENI_CFG_REG106_RX_FW_PAR_SEL_SHFT                                                    0x3
#define HWIO_GENI_CFG_REG106_RX_PAR_MODE_BMSK                                                      0x6
#define HWIO_GENI_CFG_REG106_RX_PAR_MODE_SHFT                                                      0x1
#define HWIO_GENI_CFG_REG106_RX_PAR_CALC_EN_BMSK                                                   0x1
#define HWIO_GENI_CFG_REG106_RX_PAR_CALC_EN_SHFT                                                   0x0

#define HWIO_GENI_CFG_REG107_ADDR(x)                                                        ((x) + 0x000001ac)
#define HWIO_GENI_CFG_REG107_OFFS                                                           (0x000001ac)
#define HWIO_GENI_CFG_REG107_RMSK                                                           0x80000003
#define HWIO_GENI_CFG_REG107_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG107_ADDR(x), HWIO_GENI_CFG_REG107_RMSK)
#define HWIO_GENI_CFG_REG107_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG107_ADDR(x), m)
#define HWIO_GENI_CFG_REG107_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG107_ADDR(x),v)
#define HWIO_GENI_CFG_REG107_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG107_ADDR(x),m,v,HWIO_GENI_CFG_REG107_IN(x))
#define HWIO_GENI_CFG_REG107_IO3_SW_CONTROL_BMSK                                            0x80000000
#define HWIO_GENI_CFG_REG107_IO3_SW_CONTROL_SHFT                                                  0x1f
#define HWIO_GENI_CFG_REG107_IO3_FORCE_HIGH_BMSK                                                   0x2
#define HWIO_GENI_CFG_REG107_IO3_FORCE_HIGH_SHFT                                                   0x1
#define HWIO_GENI_CFG_REG107_IO3_FORCE_LOW_BMSK                                                    0x1
#define HWIO_GENI_CFG_REG107_IO3_FORCE_LOW_SHFT                                                    0x0

#define HWIO_GENI_CFG_REG108_ADDR(x)                                                        ((x) + 0x000001b0)
#define HWIO_GENI_CFG_REG108_OFFS                                                           (0x000001b0)
#define HWIO_GENI_CFG_REG108_RMSK                                                              0x7773f
#define HWIO_GENI_CFG_REG108_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG108_ADDR(x), HWIO_GENI_CFG_REG108_RMSK)
#define HWIO_GENI_CFG_REG108_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG108_ADDR(x), m)
#define HWIO_GENI_CFG_REG108_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG108_ADDR(x),v)
#define HWIO_GENI_CFG_REG108_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG108_ADDR(x),m,v,HWIO_GENI_CFG_REG108_IN(x))
#define HWIO_GENI_CFG_REG108_TX_CORE_TO_IO_SAMP_EN_BMSK                                        0x40000
#define HWIO_GENI_CFG_REG108_TX_CORE_TO_IO_SAMP_EN_SHFT                                           0x12
#define HWIO_GENI_CFG_REG108_IO2_CORE_TO_IO_SAMP_EN_BMSK                                       0x20000
#define HWIO_GENI_CFG_REG108_IO2_CORE_TO_IO_SAMP_EN_SHFT                                          0x11
#define HWIO_GENI_CFG_REG108_IO3_CORE_TO_IO_SAMP_EN_BMSK                                       0x10000
#define HWIO_GENI_CFG_REG108_IO3_CORE_TO_IO_SAMP_EN_SHFT                                          0x10
#define HWIO_GENI_CFG_REG108_RX_SI_EN2IO_DEALAY_BMSK                                            0x7000
#define HWIO_GENI_CFG_REG108_RX_SI_EN2IO_DEALAY_SHFT                                               0xc
#define HWIO_GENI_CFG_REG108_RX_IO_EN2CORE_EN_DELAY_BMSK                                         0x700
#define HWIO_GENI_CFG_REG108_RX_IO_EN2CORE_EN_DELAY_SHFT                                           0x8
#define HWIO_GENI_CFG_REG108_RX_POS_FF_EN_SEL_BMSK                                                0x20
#define HWIO_GENI_CFG_REG108_RX_POS_FF_EN_SEL_SHFT                                                 0x5
#define HWIO_GENI_CFG_REG108_RX_IO_POS_FF_EN_SEL_BMSK                                             0x10
#define HWIO_GENI_CFG_REG108_RX_IO_POS_FF_EN_SEL_SHFT                                              0x4
#define HWIO_GENI_CFG_REG108_RX_IO_INT_CLK_SAMP_EN_BMSK                                            0x8
#define HWIO_GENI_CFG_REG108_RX_IO_INT_CLK_SAMP_EN_SHFT                                            0x3
#define HWIO_GENI_CFG_REG108_RX_IO_PIPE_STAGE_SEL_BMSK                                             0x4
#define HWIO_GENI_CFG_REG108_RX_IO_PIPE_STAGE_SEL_SHFT                                             0x2
#define HWIO_GENI_CFG_REG108_CPOL_CTRL_BMSK                                                        0x2
#define HWIO_GENI_CFG_REG108_CPOL_CTRL_SHFT                                                        0x1
#define HWIO_GENI_CFG_REG108_CPHA_CTRL_BMSK                                                        0x1
#define HWIO_GENI_CFG_REG108_CPHA_CTRL_SHFT                                                        0x0

#define HWIO_GENI_CFG_REG109_ADDR(x)                                                        ((x) + 0x000001b4)
#define HWIO_GENI_CFG_REG109_OFFS                                                           (0x000001b4)
#define HWIO_GENI_CFG_REG109_RMSK                                                                  0x1
#define HWIO_GENI_CFG_REG109_IN(x)      \
        in_dword_masked(HWIO_GENI_CFG_REG109_ADDR(x), HWIO_GENI_CFG_REG109_RMSK)
#define HWIO_GENI_CFG_REG109_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CFG_REG109_ADDR(x), m)
#define HWIO_GENI_CFG_REG109_OUT(x, v)      \
        out_dword(HWIO_GENI_CFG_REG109_ADDR(x),v)
#define HWIO_GENI_CFG_REG109_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CFG_REG109_ADDR(x),m,v,HWIO_GENI_CFG_REG109_IN(x))
#define HWIO_GENI_CFG_REG109_RX_CPOL_BMSK                                                          0x1
#define HWIO_GENI_CFG_REG109_RX_CPOL_SHFT                                                          0x0

#define HWIO_UART_LOOPBACK_CFG_ADDR(x)                                                      ((x) + 0x0000012c)
#define HWIO_UART_LOOPBACK_CFG_OFFS                                                         (0x0000012c)
#define HWIO_UART_LOOPBACK_CFG_RMSK                                                                0x3
#define HWIO_UART_LOOPBACK_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_LOOPBACK_CFG_ADDR(x), HWIO_UART_LOOPBACK_CFG_RMSK)
#define HWIO_UART_LOOPBACK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_LOOPBACK_CFG_ADDR(x), m)
#define HWIO_UART_LOOPBACK_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_LOOPBACK_CFG_ADDR(x),v)
#define HWIO_UART_LOOPBACK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_LOOPBACK_CFG_ADDR(x),m,v,HWIO_UART_LOOPBACK_CFG_IN(x))
#define HWIO_UART_LOOPBACK_CFG_LOOPBACK_MODE_BMSK                                                  0x3
#define HWIO_UART_LOOPBACK_CFG_LOOPBACK_MODE_SHFT                                                  0x0

#define HWIO_UART_TX_TRANS_CFG_ADDR(x)                                                      ((x) + 0x0000015c)
#define HWIO_UART_TX_TRANS_CFG_OFFS                                                         (0x0000015c)
#define HWIO_UART_TX_TRANS_CFG_RMSK                                                               0x3f
#define HWIO_UART_TX_TRANS_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_TX_TRANS_CFG_ADDR(x), HWIO_UART_TX_TRANS_CFG_RMSK)
#define HWIO_UART_TX_TRANS_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_TRANS_CFG_ADDR(x), m)
#define HWIO_UART_TX_TRANS_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_TX_TRANS_CFG_ADDR(x),v)
#define HWIO_UART_TX_TRANS_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_TRANS_CFG_ADDR(x),m,v,HWIO_UART_TX_TRANS_CFG_IN(x))
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG3_BMSK                                            0x20
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG3_SHFT                                             0x5
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG2_BMSK                                            0x10
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG2_SHFT                                             0x4
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG1_BMSK                                             0x8
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG1_SHFT                                             0x3
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG_BMSK                                              0x4
#define HWIO_UART_TX_TRANS_CFG_UART_NOT_USED_CFG_SHFT                                              0x2
#define HWIO_UART_TX_TRANS_CFG_UART_CTS_MASK_BMSK                                                  0x2
#define HWIO_UART_TX_TRANS_CFG_UART_CTS_MASK_SHFT                                                  0x1
#define HWIO_UART_TX_TRANS_CFG_UART_PARITY_EN_BMSK                                                 0x1
#define HWIO_UART_TX_TRANS_CFG_UART_PARITY_EN_SHFT                                                 0x0

#define HWIO_UART_TX_WORD_LEN_ADDR(x)                                                       ((x) + 0x00000168)
#define HWIO_UART_TX_WORD_LEN_OFFS                                                          (0x00000168)
#define HWIO_UART_TX_WORD_LEN_RMSK                                                               0x3ff
#define HWIO_UART_TX_WORD_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_WORD_LEN_ADDR(x), HWIO_UART_TX_WORD_LEN_RMSK)
#define HWIO_UART_TX_WORD_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_WORD_LEN_ADDR(x), m)
#define HWIO_UART_TX_WORD_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_WORD_LEN_ADDR(x),v)
#define HWIO_UART_TX_WORD_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_WORD_LEN_ADDR(x),m,v,HWIO_UART_TX_WORD_LEN_IN(x))
#define HWIO_UART_TX_WORD_LEN_UART_TX_WORD_LEN_BMSK                                              0x3ff
#define HWIO_UART_TX_WORD_LEN_UART_TX_WORD_LEN_SHFT                                                0x0

#define HWIO_UART_TX_STOP_BIT_LEN_ADDR(x)                                                   ((x) + 0x0000016c)
#define HWIO_UART_TX_STOP_BIT_LEN_OFFS                                                      (0x0000016c)
#define HWIO_UART_TX_STOP_BIT_LEN_RMSK                                                        0xffffff
#define HWIO_UART_TX_STOP_BIT_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x), HWIO_UART_TX_STOP_BIT_LEN_RMSK)
#define HWIO_UART_TX_STOP_BIT_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x), m)
#define HWIO_UART_TX_STOP_BIT_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x),v)
#define HWIO_UART_TX_STOP_BIT_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_STOP_BIT_LEN_ADDR(x),m,v,HWIO_UART_TX_STOP_BIT_LEN_IN(x))
#define HWIO_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_BMSK                                   0xffffff
#define HWIO_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_SHFT                                        0x0

#define HWIO_UART_TX_TRANS_LEN_ADDR(x)                                                      ((x) + 0x00000170)
#define HWIO_UART_TX_TRANS_LEN_OFFS                                                         (0x00000170)
#define HWIO_UART_TX_TRANS_LEN_RMSK                                                           0xffffff
#define HWIO_UART_TX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_TX_TRANS_LEN_ADDR(x), HWIO_UART_TX_TRANS_LEN_RMSK)
#define HWIO_UART_TX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_TRANS_LEN_ADDR(x), m)
#define HWIO_UART_TX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_TX_TRANS_LEN_ADDR(x),v)
#define HWIO_UART_TX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_TRANS_LEN_ADDR(x),m,v,HWIO_UART_TX_TRANS_LEN_IN(x))
#define HWIO_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_BMSK                                         0xffffff
#define HWIO_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_SHFT                                              0x0

#define HWIO_UART_RX_TRANS_CFG_ADDR(x)                                                      ((x) + 0x00000180)
#define HWIO_UART_RX_TRANS_CFG_OFFS                                                         (0x00000180)
#define HWIO_UART_RX_TRANS_CFG_RMSK                                                               0x3f
#define HWIO_UART_RX_TRANS_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_RX_TRANS_CFG_ADDR(x), HWIO_UART_RX_TRANS_CFG_RMSK)
#define HWIO_UART_RX_TRANS_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_TRANS_CFG_ADDR(x), m)
#define HWIO_UART_RX_TRANS_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_RX_TRANS_CFG_ADDR(x),v)
#define HWIO_UART_RX_TRANS_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_TRANS_CFG_ADDR(x),m,v,HWIO_UART_RX_TRANS_CFG_IN(x))
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG3_BMSK                                            0x20
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG3_SHFT                                             0x5
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG2_BMSK                                            0x10
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG2_SHFT                                             0x4
#define HWIO_UART_RX_TRANS_CFG_UART_PARITY_EN_BMSK                                                 0x8
#define HWIO_UART_RX_TRANS_CFG_UART_PARITY_EN_SHFT                                                 0x3
#define HWIO_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_BMSK                                      0x4
#define HWIO_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_SHFT                                      0x2
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG1_BMSK                                             0x2
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG1_SHFT                                             0x1
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG0_BMSK                                             0x1
#define HWIO_UART_RX_TRANS_CFG_UART_NOT_USED_CFG0_SHFT                                             0x0

#define HWIO_UART_RX_WORD_LEN_ADDR(x)                                                       ((x) + 0x0000018c)
#define HWIO_UART_RX_WORD_LEN_OFFS                                                          (0x0000018c)
#define HWIO_UART_RX_WORD_LEN_RMSK                                                               0x3ff
#define HWIO_UART_RX_WORD_LEN_IN(x)      \
        in_dword_masked(HWIO_UART_RX_WORD_LEN_ADDR(x), HWIO_UART_RX_WORD_LEN_RMSK)
#define HWIO_UART_RX_WORD_LEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_WORD_LEN_ADDR(x), m)
#define HWIO_UART_RX_WORD_LEN_OUT(x, v)      \
        out_dword(HWIO_UART_RX_WORD_LEN_ADDR(x),v)
#define HWIO_UART_RX_WORD_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_WORD_LEN_ADDR(x),m,v,HWIO_UART_RX_WORD_LEN_IN(x))
#define HWIO_UART_RX_WORD_LEN_UART_RX_WORD_LEN_BMSK                                              0x3ff
#define HWIO_UART_RX_WORD_LEN_UART_RX_WORD_LEN_SHFT                                                0x0

#define HWIO_UART_RX_STALE_CNT_ADDR(x)                                                      ((x) + 0x00000194)
#define HWIO_UART_RX_STALE_CNT_OFFS                                                         (0x00000194)
#define HWIO_UART_RX_STALE_CNT_RMSK                                                           0xffffff
#define HWIO_UART_RX_STALE_CNT_IN(x)      \
        in_dword_masked(HWIO_UART_RX_STALE_CNT_ADDR(x), HWIO_UART_RX_STALE_CNT_RMSK)
#define HWIO_UART_RX_STALE_CNT_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_STALE_CNT_ADDR(x), m)
#define HWIO_UART_RX_STALE_CNT_OUT(x, v)      \
        out_dword(HWIO_UART_RX_STALE_CNT_ADDR(x),v)
#define HWIO_UART_RX_STALE_CNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_STALE_CNT_ADDR(x),m,v,HWIO_UART_RX_STALE_CNT_IN(x))
#define HWIO_UART_RX_STALE_CNT_UART_RX_STALE_CNT_BMSK                                         0xffffff
#define HWIO_UART_RX_STALE_CNT_UART_RX_STALE_CNT_SHFT                                              0x0

#define HWIO_UART_TX_PARITY_CFG_ADDR(x)                                                     ((x) + 0x000001a4)
#define HWIO_UART_TX_PARITY_CFG_OFFS                                                        (0x000001a4)
#define HWIO_UART_TX_PARITY_CFG_RMSK                                                              0x1f
#define HWIO_UART_TX_PARITY_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_TX_PARITY_CFG_ADDR(x), HWIO_UART_TX_PARITY_CFG_RMSK)
#define HWIO_UART_TX_PARITY_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_TX_PARITY_CFG_ADDR(x), m)
#define HWIO_UART_TX_PARITY_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_TX_PARITY_CFG_ADDR(x),v)
#define HWIO_UART_TX_PARITY_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_TX_PARITY_CFG_ADDR(x),m,v,HWIO_UART_TX_PARITY_CFG_IN(x))
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG1_BMSK                                           0x10
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG1_SHFT                                            0x4
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG0_BMSK                                            0x8
#define HWIO_UART_TX_PARITY_CFG_UART_NOT_USED_CFG0_SHFT                                            0x3
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_MODE_BMSK                                                   0x6
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_MODE_SHFT                                                   0x1
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_BMSK                                                0x1
#define HWIO_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_SHFT                                                0x0

#define HWIO_UART_RX_PARITY_CFG_ADDR(x)                                                     ((x) + 0x000001a8)
#define HWIO_UART_RX_PARITY_CFG_OFFS                                                        (0x000001a8)
#define HWIO_UART_RX_PARITY_CFG_RMSK                                                              0x1f
#define HWIO_UART_RX_PARITY_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_RX_PARITY_CFG_ADDR(x), HWIO_UART_RX_PARITY_CFG_RMSK)
#define HWIO_UART_RX_PARITY_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_RX_PARITY_CFG_ADDR(x), m)
#define HWIO_UART_RX_PARITY_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_RX_PARITY_CFG_ADDR(x),v)
#define HWIO_UART_RX_PARITY_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_RX_PARITY_CFG_ADDR(x),m,v,HWIO_UART_RX_PARITY_CFG_IN(x))
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG1_BMSK                                           0x10
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG1_SHFT                                            0x4
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG0_BMSK                                            0x8
#define HWIO_UART_RX_PARITY_CFG_UART_NOT_USED_CFG0_SHFT                                            0x3
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_MODE_BMSK                                                   0x6
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_MODE_SHFT                                                   0x1
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_BMSK                                                0x1
#define HWIO_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_SHFT                                                0x0

#define HWIO_UART_MANUAL_RFR_ADDR(x)                                                        ((x) + 0x000001ac)
#define HWIO_UART_MANUAL_RFR_OFFS                                                           (0x000001ac)
#define HWIO_UART_MANUAL_RFR_RMSK                                                           0x80000003
#define HWIO_UART_MANUAL_RFR_IN(x)      \
        in_dword_masked(HWIO_UART_MANUAL_RFR_ADDR(x), HWIO_UART_MANUAL_RFR_RMSK)
#define HWIO_UART_MANUAL_RFR_INM(x, m)      \
        in_dword_masked(HWIO_UART_MANUAL_RFR_ADDR(x), m)
#define HWIO_UART_MANUAL_RFR_OUT(x, v)      \
        out_dword(HWIO_UART_MANUAL_RFR_ADDR(x),v)
#define HWIO_UART_MANUAL_RFR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_MANUAL_RFR_ADDR(x),m,v,HWIO_UART_MANUAL_RFR_IN(x))
#define HWIO_UART_MANUAL_RFR_UART_MANUAL_RFR_EN_BMSK                                        0x80000000
#define HWIO_UART_MANUAL_RFR_UART_MANUAL_RFR_EN_SHFT                                              0x1f
#define HWIO_UART_MANUAL_RFR_UART_RFR_NOT_READY_BMSK                                               0x2
#define HWIO_UART_MANUAL_RFR_UART_RFR_NOT_READY_SHFT                                               0x1
#define HWIO_UART_MANUAL_RFR_UART_RFR_READY_BMSK                                                   0x1
#define HWIO_UART_MANUAL_RFR_UART_RFR_READY_SHFT                                                   0x0

#define HWIO_GENI_BYTE_GRANULARITY_ADDR(x)                                                  ((x) + 0x00000154)
#define HWIO_GENI_BYTE_GRANULARITY_OFFS                                                     (0x00000154)
#define HWIO_GENI_BYTE_GRANULARITY_RMSK                                                            0x3
#define HWIO_GENI_BYTE_GRANULARITY_IN(x)      \
        in_dword_masked(HWIO_GENI_BYTE_GRANULARITY_ADDR(x), HWIO_GENI_BYTE_GRANULARITY_RMSK)
#define HWIO_GENI_BYTE_GRANULARITY_INM(x, m)      \
        in_dword_masked(HWIO_GENI_BYTE_GRANULARITY_ADDR(x), m)
#define HWIO_GENI_BYTE_GRANULARITY_OUT(x, v)      \
        out_dword(HWIO_GENI_BYTE_GRANULARITY_ADDR(x),v)
#define HWIO_GENI_BYTE_GRANULARITY_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_BYTE_GRANULARITY_ADDR(x),m,v,HWIO_GENI_BYTE_GRANULARITY_IN(x))
#define HWIO_GENI_BYTE_GRANULARITY_GENI_BYTE_GRANULARITY_BMSK                                      0x3
#define HWIO_GENI_BYTE_GRANULARITY_GENI_BYTE_GRANULARITY_SHFT                                      0x0

#define HWIO_GENI_DMA_MODE_EN_ADDR(x)                                                       ((x) + 0x00000158)
#define HWIO_GENI_DMA_MODE_EN_OFFS                                                          (0x00000158)
#define HWIO_GENI_DMA_MODE_EN_RMSK                                                                 0x1
#define HWIO_GENI_DMA_MODE_EN_IN(x)      \
        in_dword_masked(HWIO_GENI_DMA_MODE_EN_ADDR(x), HWIO_GENI_DMA_MODE_EN_RMSK)
#define HWIO_GENI_DMA_MODE_EN_INM(x, m)      \
        in_dword_masked(HWIO_GENI_DMA_MODE_EN_ADDR(x), m)
#define HWIO_GENI_DMA_MODE_EN_OUT(x, v)      \
        out_dword(HWIO_GENI_DMA_MODE_EN_ADDR(x),v)
#define HWIO_GENI_DMA_MODE_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_DMA_MODE_EN_ADDR(x),m,v,HWIO_GENI_DMA_MODE_EN_IN(x))
#define HWIO_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK                                                0x1
#define HWIO_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_SHFT                                                0x0

#define HWIO_GENI_TX_PACKING_CFG0_ADDR(x)                                                   ((x) + 0x00000160)
#define HWIO_GENI_TX_PACKING_CFG0_OFFS                                                      (0x00000160)
#define HWIO_GENI_TX_PACKING_CFG0_RMSK                                                         0xfffff
#define HWIO_GENI_TX_PACKING_CFG0_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_PACKING_CFG0_ADDR(x), HWIO_GENI_TX_PACKING_CFG0_RMSK)
#define HWIO_GENI_TX_PACKING_CFG0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_PACKING_CFG0_ADDR(x), m)
#define HWIO_GENI_TX_PACKING_CFG0_OUT(x, v)      \
        out_dword(HWIO_GENI_TX_PACKING_CFG0_ADDR(x),v)
#define HWIO_GENI_TX_PACKING_CFG0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TX_PACKING_CFG0_ADDR(x),m,v,HWIO_GENI_TX_PACKING_CFG0_IN(x))
#define HWIO_GENI_TX_PACKING_CFG0_TX_DATA_TABLE_VEC_1_BMSK                                     0xffc00
#define HWIO_GENI_TX_PACKING_CFG0_TX_DATA_TABLE_VEC_1_SHFT                                         0xa
#define HWIO_GENI_TX_PACKING_CFG0_TX_DATA_TABLE_VEC_0_BMSK                                       0x3ff
#define HWIO_GENI_TX_PACKING_CFG0_TX_DATA_TABLE_VEC_0_SHFT                                         0x0

#define HWIO_GENI_TX_PACKING_CFG1_ADDR(x)                                                   ((x) + 0x00000164)
#define HWIO_GENI_TX_PACKING_CFG1_OFFS                                                      (0x00000164)
#define HWIO_GENI_TX_PACKING_CFG1_RMSK                                                         0xfffff
#define HWIO_GENI_TX_PACKING_CFG1_IN(x)      \
        in_dword_masked(HWIO_GENI_TX_PACKING_CFG1_ADDR(x), HWIO_GENI_TX_PACKING_CFG1_RMSK)
#define HWIO_GENI_TX_PACKING_CFG1_INM(x, m)      \
        in_dword_masked(HWIO_GENI_TX_PACKING_CFG1_ADDR(x), m)
#define HWIO_GENI_TX_PACKING_CFG1_OUT(x, v)      \
        out_dword(HWIO_GENI_TX_PACKING_CFG1_ADDR(x),v)
#define HWIO_GENI_TX_PACKING_CFG1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_TX_PACKING_CFG1_ADDR(x),m,v,HWIO_GENI_TX_PACKING_CFG1_IN(x))
#define HWIO_GENI_TX_PACKING_CFG1_TX_DATA_TABLE_VEC_3_BMSK                                     0xffc00
#define HWIO_GENI_TX_PACKING_CFG1_TX_DATA_TABLE_VEC_3_SHFT                                         0xa
#define HWIO_GENI_TX_PACKING_CFG1_TX_DATA_TABLE_VEC_2_BMSK                                       0x3ff
#define HWIO_GENI_TX_PACKING_CFG1_TX_DATA_TABLE_VEC_2_SHFT                                         0x0

#define HWIO_GENI_RX_PACKING_CFG0_ADDR(x)                                                   ((x) + 0x00000184)
#define HWIO_GENI_RX_PACKING_CFG0_OFFS                                                      (0x00000184)
#define HWIO_GENI_RX_PACKING_CFG0_RMSK                                                         0xfffff
#define HWIO_GENI_RX_PACKING_CFG0_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_PACKING_CFG0_ADDR(x), HWIO_GENI_RX_PACKING_CFG0_RMSK)
#define HWIO_GENI_RX_PACKING_CFG0_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_PACKING_CFG0_ADDR(x), m)
#define HWIO_GENI_RX_PACKING_CFG0_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_PACKING_CFG0_ADDR(x),v)
#define HWIO_GENI_RX_PACKING_CFG0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_PACKING_CFG0_ADDR(x),m,v,HWIO_GENI_RX_PACKING_CFG0_IN(x))
#define HWIO_GENI_RX_PACKING_CFG0_RX_DATA_TABLE_VEC_1_BMSK                                     0xffc00
#define HWIO_GENI_RX_PACKING_CFG0_RX_DATA_TABLE_VEC_1_SHFT                                         0xa
#define HWIO_GENI_RX_PACKING_CFG0_RX_DATA_TABLE_VEC_0_BMSK                                       0x3ff
#define HWIO_GENI_RX_PACKING_CFG0_RX_DATA_TABLE_VEC_0_SHFT                                         0x0

#define HWIO_GENI_RX_PACKING_CFG1_ADDR(x)                                                   ((x) + 0x00000188)
#define HWIO_GENI_RX_PACKING_CFG1_OFFS                                                      (0x00000188)
#define HWIO_GENI_RX_PACKING_CFG1_RMSK                                                         0xfffff
#define HWIO_GENI_RX_PACKING_CFG1_IN(x)      \
        in_dword_masked(HWIO_GENI_RX_PACKING_CFG1_ADDR(x), HWIO_GENI_RX_PACKING_CFG1_RMSK)
#define HWIO_GENI_RX_PACKING_CFG1_INM(x, m)      \
        in_dword_masked(HWIO_GENI_RX_PACKING_CFG1_ADDR(x), m)
#define HWIO_GENI_RX_PACKING_CFG1_OUT(x, v)      \
        out_dword(HWIO_GENI_RX_PACKING_CFG1_ADDR(x),v)
#define HWIO_GENI_RX_PACKING_CFG1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_RX_PACKING_CFG1_ADDR(x),m,v,HWIO_GENI_RX_PACKING_CFG1_IN(x))
#define HWIO_GENI_RX_PACKING_CFG1_RX_DATA_TABLE_VEC_3_BMSK                                     0xffc00
#define HWIO_GENI_RX_PACKING_CFG1_RX_DATA_TABLE_VEC_3_SHFT                                         0xa
#define HWIO_GENI_RX_PACKING_CFG1_RX_DATA_TABLE_VEC_2_BMSK                                       0x3ff
#define HWIO_GENI_RX_PACKING_CFG1_RX_DATA_TABLE_VEC_2_SHFT                                         0x0

#define HWIO_I2C_NOISE_CANCELATION_CTL_ADDR(x)                                              ((x) + 0x00000134)
#define HWIO_I2C_NOISE_CANCELATION_CTL_OFFS                                                 (0x00000134)
#define HWIO_I2C_NOISE_CANCELATION_CTL_RMSK                                                       0x1f
#define HWIO_I2C_NOISE_CANCELATION_CTL_IN(x)      \
        in_dword_masked(HWIO_I2C_NOISE_CANCELATION_CTL_ADDR(x), HWIO_I2C_NOISE_CANCELATION_CTL_RMSK)
#define HWIO_I2C_NOISE_CANCELATION_CTL_INM(x, m)      \
        in_dword_masked(HWIO_I2C_NOISE_CANCELATION_CTL_ADDR(x), m)
#define HWIO_I2C_NOISE_CANCELATION_CTL_OUT(x, v)      \
        out_dword(HWIO_I2C_NOISE_CANCELATION_CTL_ADDR(x),v)
#define HWIO_I2C_NOISE_CANCELATION_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_NOISE_CANCELATION_CTL_ADDR(x),m,v,HWIO_I2C_NOISE_CANCELATION_CTL_IN(x))
#define HWIO_I2C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_BMSK                                      0x18
#define HWIO_I2C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_SHFT                                       0x3
#define HWIO_I2C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_BMSK                                       0x6
#define HWIO_I2C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_SHFT                                       0x1
#define HWIO_I2C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_BMSK                             0x1
#define HWIO_I2C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_SHFT                             0x0

#define HWIO_I2C_MONITOR_CTL_ADDR(x)                                                        ((x) + 0x00000138)
#define HWIO_I2C_MONITOR_CTL_OFFS                                                           (0x00000138)
#define HWIO_I2C_MONITOR_CTL_RMSK                                                                  0x3
#define HWIO_I2C_MONITOR_CTL_IN(x)      \
        in_dword_masked(HWIO_I2C_MONITOR_CTL_ADDR(x), HWIO_I2C_MONITOR_CTL_RMSK)
#define HWIO_I2C_MONITOR_CTL_INM(x, m)      \
        in_dword_masked(HWIO_I2C_MONITOR_CTL_ADDR(x), m)
#define HWIO_I2C_MONITOR_CTL_OUT(x, v)      \
        out_dword(HWIO_I2C_MONITOR_CTL_ADDR(x),v)
#define HWIO_I2C_MONITOR_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_MONITOR_CTL_ADDR(x),m,v,HWIO_I2C_MONITOR_CTL_IN(x))
#define HWIO_I2C_MONITOR_CTL_BUSY_INDICATION_SELECT_BMSK                                           0x2
#define HWIO_I2C_MONITOR_CTL_BUSY_INDICATION_SELECT_SHFT                                           0x1
#define HWIO_I2C_MONITOR_CTL_SDA_DELAYED_DETECTION_BMSK                                            0x1
#define HWIO_I2C_MONITOR_CTL_SDA_DELAYED_DETECTION_SHFT                                            0x0

#define HWIO_I2C_TX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x0000016c)
#define HWIO_I2C_TX_TRANS_LEN_OFFS                                                          (0x0000016c)
#define HWIO_I2C_TX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_I2C_TX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_I2C_TX_TRANS_LEN_ADDR(x), HWIO_I2C_TX_TRANS_LEN_RMSK)
#define HWIO_I2C_TX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_I2C_TX_TRANS_LEN_ADDR(x), m)
#define HWIO_I2C_TX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_I2C_TX_TRANS_LEN_ADDR(x),v)
#define HWIO_I2C_TX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_TX_TRANS_LEN_ADDR(x),m,v,HWIO_I2C_TX_TRANS_LEN_IN(x))
#define HWIO_I2C_TX_TRANS_LEN_I2C_TX_TRANS_LEN_BMSK                                           0xffffff
#define HWIO_I2C_TX_TRANS_LEN_I2C_TX_TRANS_LEN_SHFT                                                0x0

#define HWIO_I2C_RX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x00000170)
#define HWIO_I2C_RX_TRANS_LEN_OFFS                                                          (0x00000170)
#define HWIO_I2C_RX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_I2C_RX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_I2C_RX_TRANS_LEN_ADDR(x), HWIO_I2C_RX_TRANS_LEN_RMSK)
#define HWIO_I2C_RX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_I2C_RX_TRANS_LEN_ADDR(x), m)
#define HWIO_I2C_RX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_I2C_RX_TRANS_LEN_ADDR(x),v)
#define HWIO_I2C_RX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_RX_TRANS_LEN_ADDR(x),m,v,HWIO_I2C_RX_TRANS_LEN_IN(x))
#define HWIO_I2C_RX_TRANS_LEN_I2C_RX_TRANS_LEN_BMSK                                           0xffffff
#define HWIO_I2C_RX_TRANS_LEN_I2C_RX_TRANS_LEN_SHFT                                                0x0

#define HWIO_I2C_DELAY_COUNTER_ADDR(x)                                                      ((x) + 0x00000174)
#define HWIO_I2C_DELAY_COUNTER_OFFS                                                         (0x00000174)
#define HWIO_I2C_DELAY_COUNTER_RMSK                                                           0xffffff
#define HWIO_I2C_DELAY_COUNTER_IN(x)      \
        in_dword_masked(HWIO_I2C_DELAY_COUNTER_ADDR(x), HWIO_I2C_DELAY_COUNTER_RMSK)
#define HWIO_I2C_DELAY_COUNTER_INM(x, m)      \
        in_dword_masked(HWIO_I2C_DELAY_COUNTER_ADDR(x), m)
#define HWIO_I2C_DELAY_COUNTER_OUT(x, v)      \
        out_dword(HWIO_I2C_DELAY_COUNTER_ADDR(x),v)
#define HWIO_I2C_DELAY_COUNTER_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_DELAY_COUNTER_ADDR(x),m,v,HWIO_I2C_DELAY_COUNTER_IN(x))
#define HWIO_I2C_DELAY_COUNTER_I2C_DELAY_COUNTER_BMSK                                         0xffffff
#define HWIO_I2C_DELAY_COUNTER_I2C_DELAY_COUNTER_SHFT                                              0x0

#define HWIO_I2C_SCL_COUNTERS_ADDR(x)                                                       ((x) + 0x00000178)
#define HWIO_I2C_SCL_COUNTERS_OFFS                                                          (0x00000178)
#define HWIO_I2C_SCL_COUNTERS_RMSK                                                          0x3fffffff
#define HWIO_I2C_SCL_COUNTERS_IN(x)      \
        in_dword_masked(HWIO_I2C_SCL_COUNTERS_ADDR(x), HWIO_I2C_SCL_COUNTERS_RMSK)
#define HWIO_I2C_SCL_COUNTERS_INM(x, m)      \
        in_dword_masked(HWIO_I2C_SCL_COUNTERS_ADDR(x), m)
#define HWIO_I2C_SCL_COUNTERS_OUT(x, v)      \
        out_dword(HWIO_I2C_SCL_COUNTERS_ADDR(x),v)
#define HWIO_I2C_SCL_COUNTERS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_SCL_COUNTERS_ADDR(x),m,v,HWIO_I2C_SCL_COUNTERS_IN(x))
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_BMSK                                     0x3ff00000
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_SHFT                                           0x14
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_BMSK                                         0xffc00
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_SHFT                                             0xa
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_BMSK                                         0x3ff
#define HWIO_I2C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_SHFT                                           0x0

#define HWIO_SPI_CPHA_ADDR(x)                                                               ((x) + 0x00000124)
#define HWIO_SPI_CPHA_OFFS                                                                  (0x00000124)
#define HWIO_SPI_CPHA_RMSK                                                                         0xf
#define HWIO_SPI_CPHA_IN(x)      \
        in_dword_masked(HWIO_SPI_CPHA_ADDR(x), HWIO_SPI_CPHA_RMSK)
#define HWIO_SPI_CPHA_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CPHA_ADDR(x), m)
#define HWIO_SPI_CPHA_OUT(x, v)      \
        out_dword(HWIO_SPI_CPHA_ADDR(x),v)
#define HWIO_SPI_CPHA_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CPHA_ADDR(x),m,v,HWIO_SPI_CPHA_IN(x))
#define HWIO_SPI_CPHA_CPHA_BMSK                                                                    0xf
#define HWIO_SPI_CPHA_CPHA_SHFT                                                                    0x0

#define HWIO_SPI_LOOPBACK_CFG_ADDR(x)                                                       ((x) + 0x0000012c)
#define HWIO_SPI_LOOPBACK_CFG_OFFS                                                          (0x0000012c)
#define HWIO_SPI_LOOPBACK_CFG_RMSK                                                                 0x3
#define HWIO_SPI_LOOPBACK_CFG_IN(x)      \
        in_dword_masked(HWIO_SPI_LOOPBACK_CFG_ADDR(x), HWIO_SPI_LOOPBACK_CFG_RMSK)
#define HWIO_SPI_LOOPBACK_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SPI_LOOPBACK_CFG_ADDR(x), m)
#define HWIO_SPI_LOOPBACK_CFG_OUT(x, v)      \
        out_dword(HWIO_SPI_LOOPBACK_CFG_ADDR(x),v)
#define HWIO_SPI_LOOPBACK_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_LOOPBACK_CFG_ADDR(x),m,v,HWIO_SPI_LOOPBACK_CFG_IN(x))
#define HWIO_SPI_LOOPBACK_CFG_LOOPBACK_MODE_BMSK                                                   0x3
#define HWIO_SPI_LOOPBACK_CFG_LOOPBACK_MODE_SHFT                                                   0x0

#define HWIO_SPI_CPOL_ADDR(x)                                                               ((x) + 0x00000130)
#define HWIO_SPI_CPOL_OFFS                                                                  (0x00000130)
#define HWIO_SPI_CPOL_RMSK                                                                         0x7
#define HWIO_SPI_CPOL_IN(x)      \
        in_dword_masked(HWIO_SPI_CPOL_ADDR(x), HWIO_SPI_CPOL_RMSK)
#define HWIO_SPI_CPOL_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CPOL_ADDR(x), m)
#define HWIO_SPI_CPOL_OUT(x, v)      \
        out_dword(HWIO_SPI_CPOL_ADDR(x),v)
#define HWIO_SPI_CPOL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CPOL_ADDR(x),m,v,HWIO_SPI_CPOL_IN(x))
#define HWIO_SPI_CPOL_CPOL_BMSK                                                                    0x4
#define HWIO_SPI_CPOL_CPOL_SHFT                                                                    0x2
#define HWIO_SPI_CPOL_SPI_NOT_USED_1_BMSK                                                          0x2
#define HWIO_SPI_CPOL_SPI_NOT_USED_1_SHFT                                                          0x1
#define HWIO_SPI_CPOL_SPI_NOT_USED_0_BMSK                                                          0x1
#define HWIO_SPI_CPOL_SPI_NOT_USED_0_SHFT                                                          0x0

#define HWIO_SPI_DEMUX_OUTPUT_INV_ADDR(x)                                                   ((x) + 0x0000014c)
#define HWIO_SPI_DEMUX_OUTPUT_INV_OFFS                                                      (0x0000014c)
#define HWIO_SPI_DEMUX_OUTPUT_INV_RMSK                                                             0xf
#define HWIO_SPI_DEMUX_OUTPUT_INV_IN(x)      \
        in_dword_masked(HWIO_SPI_DEMUX_OUTPUT_INV_ADDR(x), HWIO_SPI_DEMUX_OUTPUT_INV_RMSK)
#define HWIO_SPI_DEMUX_OUTPUT_INV_INM(x, m)      \
        in_dword_masked(HWIO_SPI_DEMUX_OUTPUT_INV_ADDR(x), m)
#define HWIO_SPI_DEMUX_OUTPUT_INV_OUT(x, v)      \
        out_dword(HWIO_SPI_DEMUX_OUTPUT_INV_ADDR(x),v)
#define HWIO_SPI_DEMUX_OUTPUT_INV_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_DEMUX_OUTPUT_INV_ADDR(x),m,v,HWIO_SPI_DEMUX_OUTPUT_INV_IN(x))
#define HWIO_SPI_DEMUX_OUTPUT_INV_CS_DEMUX_OUTPUT_INV_BMSK                                         0xf
#define HWIO_SPI_DEMUX_OUTPUT_INV_CS_DEMUX_OUTPUT_INV_SHFT                                         0x0

#define HWIO_SPI_DEMUX_SEL_ADDR(x)                                                          ((x) + 0x00000150)
#define HWIO_SPI_DEMUX_SEL_OFFS                                                             (0x00000150)
#define HWIO_SPI_DEMUX_SEL_RMSK                                                                   0x1f
#define HWIO_SPI_DEMUX_SEL_IN(x)      \
        in_dword_masked(HWIO_SPI_DEMUX_SEL_ADDR(x), HWIO_SPI_DEMUX_SEL_RMSK)
#define HWIO_SPI_DEMUX_SEL_INM(x, m)      \
        in_dword_masked(HWIO_SPI_DEMUX_SEL_ADDR(x), m)
#define HWIO_SPI_DEMUX_SEL_OUT(x, v)      \
        out_dword(HWIO_SPI_DEMUX_SEL_ADDR(x),v)
#define HWIO_SPI_DEMUX_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_DEMUX_SEL_ADDR(x),m,v,HWIO_SPI_DEMUX_SEL_IN(x))
#define HWIO_SPI_DEMUX_SEL_CS_DEMUX_FW_EN_BMSK                                                    0x10
#define HWIO_SPI_DEMUX_SEL_CS_DEMUX_FW_EN_SHFT                                                     0x4
#define HWIO_SPI_DEMUX_SEL_CS_DEMUX_SEL_BMSK                                                       0xf
#define HWIO_SPI_DEMUX_SEL_CS_DEMUX_SEL_SHFT                                                       0x0

#define HWIO_SPI_TRANS_CFG_ADDR(x)                                                          ((x) + 0x0000015c)
#define HWIO_SPI_TRANS_CFG_OFFS                                                             (0x0000015c)
#define HWIO_SPI_TRANS_CFG_RMSK                                                                   0x3f
#define HWIO_SPI_TRANS_CFG_IN(x)      \
        in_dword_masked(HWIO_SPI_TRANS_CFG_ADDR(x), HWIO_SPI_TRANS_CFG_RMSK)
#define HWIO_SPI_TRANS_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SPI_TRANS_CFG_ADDR(x), m)
#define HWIO_SPI_TRANS_CFG_OUT(x, v)      \
        out_dword(HWIO_SPI_TRANS_CFG_ADDR(x),v)
#define HWIO_SPI_TRANS_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_TRANS_CFG_ADDR(x),m,v,HWIO_SPI_TRANS_CFG_IN(x))
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG4_BMSK                                                 0x20
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG4_SHFT                                                  0x5
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG3_BMSK                                                 0x10
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG3_SHFT                                                  0x4
#define HWIO_SPI_TRANS_CFG_SPI_CS_TOGGLE_BMSK                                                      0x8
#define HWIO_SPI_TRANS_CFG_SPI_CS_TOGGLE_SHFT                                                      0x3
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG2_BMSK                                                  0x4
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG2_SHFT                                                  0x2
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG1_BMSK                                                  0x2
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG1_SHFT                                                  0x1
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG0_BMSK                                                  0x1
#define HWIO_SPI_TRANS_CFG_SPI_NOT_USED_CFG0_SHFT                                                  0x0

#define HWIO_SPI_WORD_LEN_ADDR(x)                                                           ((x) + 0x00000168)
#define HWIO_SPI_WORD_LEN_OFFS                                                              (0x00000168)
#define HWIO_SPI_WORD_LEN_RMSK                                                                   0x3ff
#define HWIO_SPI_WORD_LEN_IN(x)      \
        in_dword_masked(HWIO_SPI_WORD_LEN_ADDR(x), HWIO_SPI_WORD_LEN_RMSK)
#define HWIO_SPI_WORD_LEN_INM(x, m)      \
        in_dword_masked(HWIO_SPI_WORD_LEN_ADDR(x), m)
#define HWIO_SPI_WORD_LEN_OUT(x, v)      \
        out_dword(HWIO_SPI_WORD_LEN_ADDR(x),v)
#define HWIO_SPI_WORD_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_WORD_LEN_ADDR(x),m,v,HWIO_SPI_WORD_LEN_IN(x))
#define HWIO_SPI_WORD_LEN_SPI_WORD_LEN_BMSK                                                      0x3ff
#define HWIO_SPI_WORD_LEN_SPI_WORD_LEN_SHFT                                                        0x0

#define HWIO_SPI_TX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x0000016c)
#define HWIO_SPI_TX_TRANS_LEN_OFFS                                                          (0x0000016c)
#define HWIO_SPI_TX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_SPI_TX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_SPI_TX_TRANS_LEN_ADDR(x), HWIO_SPI_TX_TRANS_LEN_RMSK)
#define HWIO_SPI_TX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_SPI_TX_TRANS_LEN_ADDR(x), m)
#define HWIO_SPI_TX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_SPI_TX_TRANS_LEN_ADDR(x),v)
#define HWIO_SPI_TX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_TX_TRANS_LEN_ADDR(x),m,v,HWIO_SPI_TX_TRANS_LEN_IN(x))
#define HWIO_SPI_TX_TRANS_LEN_SPI_TX_TRANS_LEN_BMSK                                           0xffffff
#define HWIO_SPI_TX_TRANS_LEN_SPI_TX_TRANS_LEN_SHFT                                                0x0

#define HWIO_SPI_RX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x00000170)
#define HWIO_SPI_RX_TRANS_LEN_OFFS                                                          (0x00000170)
#define HWIO_SPI_RX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_SPI_RX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_SPI_RX_TRANS_LEN_ADDR(x), HWIO_SPI_RX_TRANS_LEN_RMSK)
#define HWIO_SPI_RX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_SPI_RX_TRANS_LEN_ADDR(x), m)
#define HWIO_SPI_RX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_SPI_RX_TRANS_LEN_ADDR(x),v)
#define HWIO_SPI_RX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_RX_TRANS_LEN_ADDR(x),m,v,HWIO_SPI_RX_TRANS_LEN_IN(x))
#define HWIO_SPI_RX_TRANS_LEN_SPI_RX_TRANS_LEN_BMSK                                           0xffffff
#define HWIO_SPI_RX_TRANS_LEN_SPI_RX_TRANS_LEN_SHFT                                                0x0

#define HWIO_SPI_PRE_POST_CMD_DLY_ADDR(x)                                                   ((x) + 0x00000174)
#define HWIO_SPI_PRE_POST_CMD_DLY_OFFS                                                      (0x00000174)
#define HWIO_SPI_PRE_POST_CMD_DLY_RMSK                                                        0xffffff
#define HWIO_SPI_PRE_POST_CMD_DLY_IN(x)      \
        in_dword_masked(HWIO_SPI_PRE_POST_CMD_DLY_ADDR(x), HWIO_SPI_PRE_POST_CMD_DLY_RMSK)
#define HWIO_SPI_PRE_POST_CMD_DLY_INM(x, m)      \
        in_dword_masked(HWIO_SPI_PRE_POST_CMD_DLY_ADDR(x), m)
#define HWIO_SPI_PRE_POST_CMD_DLY_OUT(x, v)      \
        out_dword(HWIO_SPI_PRE_POST_CMD_DLY_ADDR(x),v)
#define HWIO_SPI_PRE_POST_CMD_DLY_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_PRE_POST_CMD_DLY_ADDR(x),m,v,HWIO_SPI_PRE_POST_CMD_DLY_IN(x))
#define HWIO_SPI_PRE_POST_CMD_DLY_SPI_PRE_POST_CMD_DLY_BMSK                                   0xffffff
#define HWIO_SPI_PRE_POST_CMD_DLY_SPI_PRE_POST_CMD_DLY_SHFT                                        0x0

#define HWIO_SPI_DELAYS_COUNTERS_ADDR(x)                                                    ((x) + 0x00000178)
#define HWIO_SPI_DELAYS_COUNTERS_OFFS                                                       (0x00000178)
#define HWIO_SPI_DELAYS_COUNTERS_RMSK                                                       0x3fffffff
#define HWIO_SPI_DELAYS_COUNTERS_IN(x)      \
        in_dword_masked(HWIO_SPI_DELAYS_COUNTERS_ADDR(x), HWIO_SPI_DELAYS_COUNTERS_RMSK)
#define HWIO_SPI_DELAYS_COUNTERS_INM(x, m)      \
        in_dword_masked(HWIO_SPI_DELAYS_COUNTERS_ADDR(x), m)
#define HWIO_SPI_DELAYS_COUNTERS_OUT(x, v)      \
        out_dword(HWIO_SPI_DELAYS_COUNTERS_ADDR(x),v)
#define HWIO_SPI_DELAYS_COUNTERS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_DELAYS_COUNTERS_ADDR(x),m,v,HWIO_SPI_DELAYS_COUNTERS_IN(x))
#define HWIO_SPI_DELAYS_COUNTERS_SPI_NOT_USED_BMSK                                          0x3ff00000
#define HWIO_SPI_DELAYS_COUNTERS_SPI_NOT_USED_SHFT                                                0x14
#define HWIO_SPI_DELAYS_COUNTERS_SPI_CS_CLK_DLY_BMSK                                           0xffc00
#define HWIO_SPI_DELAYS_COUNTERS_SPI_CS_CLK_DLY_SHFT                                               0xa
#define HWIO_SPI_DELAYS_COUNTERS_SPI_INTER_WORDS_DLY_BMSK                                        0x3ff
#define HWIO_SPI_DELAYS_COUNTERS_SPI_INTER_WORDS_DLY_SHFT                                          0x0

#define HWIO_I3C_NOISE_CANCELATION_CTL_ADDR(x)                                              ((x) + 0x00000134)
#define HWIO_I3C_NOISE_CANCELATION_CTL_OFFS                                                 (0x00000134)
#define HWIO_I3C_NOISE_CANCELATION_CTL_RMSK                                                       0x1f
#define HWIO_I3C_NOISE_CANCELATION_CTL_IN(x)      \
        in_dword_masked(HWIO_I3C_NOISE_CANCELATION_CTL_ADDR(x), HWIO_I3C_NOISE_CANCELATION_CTL_RMSK)
#define HWIO_I3C_NOISE_CANCELATION_CTL_INM(x, m)      \
        in_dword_masked(HWIO_I3C_NOISE_CANCELATION_CTL_ADDR(x), m)
#define HWIO_I3C_NOISE_CANCELATION_CTL_OUT(x, v)      \
        out_dword(HWIO_I3C_NOISE_CANCELATION_CTL_ADDR(x),v)
#define HWIO_I3C_NOISE_CANCELATION_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_NOISE_CANCELATION_CTL_ADDR(x),m,v,HWIO_I3C_NOISE_CANCELATION_CTL_IN(x))
#define HWIO_I3C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_BMSK                                      0x18
#define HWIO_I3C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_SHFT                                       0x3
#define HWIO_I3C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_BMSK                                       0x6
#define HWIO_I3C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_SHFT                                       0x1
#define HWIO_I3C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_BMSK                             0x1
#define HWIO_I3C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_SHFT                             0x0

#define HWIO_I3C_MONITOR_CTL_ADDR(x)                                                        ((x) + 0x00000138)
#define HWIO_I3C_MONITOR_CTL_OFFS                                                           (0x00000138)
#define HWIO_I3C_MONITOR_CTL_RMSK                                                                  0x3
#define HWIO_I3C_MONITOR_CTL_IN(x)      \
        in_dword_masked(HWIO_I3C_MONITOR_CTL_ADDR(x), HWIO_I3C_MONITOR_CTL_RMSK)
#define HWIO_I3C_MONITOR_CTL_INM(x, m)      \
        in_dword_masked(HWIO_I3C_MONITOR_CTL_ADDR(x), m)
#define HWIO_I3C_MONITOR_CTL_OUT(x, v)      \
        out_dword(HWIO_I3C_MONITOR_CTL_ADDR(x),v)
#define HWIO_I3C_MONITOR_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_MONITOR_CTL_ADDR(x),m,v,HWIO_I3C_MONITOR_CTL_IN(x))
#define HWIO_I3C_MONITOR_CTL_BUSY_INDICATION_SELECT_BMSK                                           0x2
#define HWIO_I3C_MONITOR_CTL_BUSY_INDICATION_SELECT_SHFT                                           0x1
#define HWIO_I3C_MONITOR_CTL_SDA_DELAYED_DETECTION_BMSK                                            0x1
#define HWIO_I3C_MONITOR_CTL_SDA_DELAYED_DETECTION_SHFT                                            0x0

#define HWIO_I3C_SCL_HIGH_ADDR(x)                                                           ((x) + 0x00000168)
#define HWIO_I3C_SCL_HIGH_OFFS                                                              (0x00000168)
#define HWIO_I3C_SCL_HIGH_RMSK                                                                   0x3ff
#define HWIO_I3C_SCL_HIGH_IN(x)      \
        in_dword_masked(HWIO_I3C_SCL_HIGH_ADDR(x), HWIO_I3C_SCL_HIGH_RMSK)
#define HWIO_I3C_SCL_HIGH_INM(x, m)      \
        in_dword_masked(HWIO_I3C_SCL_HIGH_ADDR(x), m)
#define HWIO_I3C_SCL_HIGH_OUT(x, v)      \
        out_dword(HWIO_I3C_SCL_HIGH_ADDR(x),v)
#define HWIO_I3C_SCL_HIGH_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_SCL_HIGH_ADDR(x),m,v,HWIO_I3C_SCL_HIGH_IN(x))
#define HWIO_I3C_SCL_HIGH_I3C_SCL_HIGH_COUNTER_BMSK                                              0x3ff
#define HWIO_I3C_SCL_HIGH_I3C_SCL_HIGH_COUNTER_SHFT                                                0x0

#define HWIO_I3C_TX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x0000016c)
#define HWIO_I3C_TX_TRANS_LEN_OFFS                                                          (0x0000016c)
#define HWIO_I3C_TX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_I3C_TX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_I3C_TX_TRANS_LEN_ADDR(x), HWIO_I3C_TX_TRANS_LEN_RMSK)
#define HWIO_I3C_TX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_I3C_TX_TRANS_LEN_ADDR(x), m)
#define HWIO_I3C_TX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_I3C_TX_TRANS_LEN_ADDR(x),v)
#define HWIO_I3C_TX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_TX_TRANS_LEN_ADDR(x),m,v,HWIO_I3C_TX_TRANS_LEN_IN(x))
#define HWIO_I3C_TX_TRANS_LEN_TX_TRANS_LEN_BMSK                                               0xffffff
#define HWIO_I3C_TX_TRANS_LEN_TX_TRANS_LEN_SHFT                                                    0x0

#define HWIO_I3C_RX_TRANS_LEN_ADDR(x)                                                       ((x) + 0x00000170)
#define HWIO_I3C_RX_TRANS_LEN_OFFS                                                          (0x00000170)
#define HWIO_I3C_RX_TRANS_LEN_RMSK                                                            0xffffff
#define HWIO_I3C_RX_TRANS_LEN_IN(x)      \
        in_dword_masked(HWIO_I3C_RX_TRANS_LEN_ADDR(x), HWIO_I3C_RX_TRANS_LEN_RMSK)
#define HWIO_I3C_RX_TRANS_LEN_INM(x, m)      \
        in_dword_masked(HWIO_I3C_RX_TRANS_LEN_ADDR(x), m)
#define HWIO_I3C_RX_TRANS_LEN_OUT(x, v)      \
        out_dword(HWIO_I3C_RX_TRANS_LEN_ADDR(x),v)
#define HWIO_I3C_RX_TRANS_LEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_RX_TRANS_LEN_ADDR(x),m,v,HWIO_I3C_RX_TRANS_LEN_IN(x))
#define HWIO_I3C_RX_TRANS_LEN_RX_TRANS_LEN_BMSK                                               0xffffff
#define HWIO_I3C_RX_TRANS_LEN_RX_TRANS_LEN_SHFT                                                    0x0

#define HWIO_I3C_DELAY_COUNTER_ADDR(x)                                                      ((x) + 0x00000174)
#define HWIO_I3C_DELAY_COUNTER_OFFS                                                         (0x00000174)
#define HWIO_I3C_DELAY_COUNTER_RMSK                                                           0xffffff
#define HWIO_I3C_DELAY_COUNTER_IN(x)      \
        in_dword_masked(HWIO_I3C_DELAY_COUNTER_ADDR(x), HWIO_I3C_DELAY_COUNTER_RMSK)
#define HWIO_I3C_DELAY_COUNTER_INM(x, m)      \
        in_dword_masked(HWIO_I3C_DELAY_COUNTER_ADDR(x), m)
#define HWIO_I3C_DELAY_COUNTER_OUT(x, v)      \
        out_dword(HWIO_I3C_DELAY_COUNTER_ADDR(x),v)
#define HWIO_I3C_DELAY_COUNTER_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_DELAY_COUNTER_ADDR(x),m,v,HWIO_I3C_DELAY_COUNTER_IN(x))
#define HWIO_I3C_DELAY_COUNTER_DELAY_COUNTER_BMSK                                             0xffffff
#define HWIO_I3C_DELAY_COUNTER_DELAY_COUNTER_SHFT                                                  0x0

#define HWIO_I3C_SCL_COUNTERS_ADDR(x)                                                       ((x) + 0x00000178)
#define HWIO_I3C_SCL_COUNTERS_OFFS                                                          (0x00000178)
#define HWIO_I3C_SCL_COUNTERS_RMSK                                                          0x3fffffff
#define HWIO_I3C_SCL_COUNTERS_IN(x)      \
        in_dword_masked(HWIO_I3C_SCL_COUNTERS_ADDR(x), HWIO_I3C_SCL_COUNTERS_RMSK)
#define HWIO_I3C_SCL_COUNTERS_INM(x, m)      \
        in_dword_masked(HWIO_I3C_SCL_COUNTERS_ADDR(x), m)
#define HWIO_I3C_SCL_COUNTERS_OUT(x, v)      \
        out_dword(HWIO_I3C_SCL_COUNTERS_ADDR(x),v)
#define HWIO_I3C_SCL_COUNTERS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_SCL_COUNTERS_ADDR(x),m,v,HWIO_I3C_SCL_COUNTERS_IN(x))
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_BMSK                                     0x3ff00000
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_SHFT                                           0x14
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_BMSK                                         0xffc00
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_SHFT                                             0xa
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_BMSK                                         0x3ff
#define HWIO_I3C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_SHFT                                           0x0

#define HWIO_I3C_SCL_LOW_ADDR(x)                                                            ((x) + 0x0000017c)
#define HWIO_I3C_SCL_LOW_OFFS                                                               (0x0000017c)
#define HWIO_I3C_SCL_LOW_RMSK                                                                    0x3ff
#define HWIO_I3C_SCL_LOW_IN(x)      \
        in_dword_masked(HWIO_I3C_SCL_LOW_ADDR(x), HWIO_I3C_SCL_LOW_RMSK)
#define HWIO_I3C_SCL_LOW_INM(x, m)      \
        in_dword_masked(HWIO_I3C_SCL_LOW_ADDR(x), m)
#define HWIO_I3C_SCL_LOW_OUT(x, v)      \
        out_dword(HWIO_I3C_SCL_LOW_ADDR(x),v)
#define HWIO_I3C_SCL_LOW_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I3C_SCL_LOW_ADDR(x),m,v,HWIO_I3C_SCL_LOW_IN(x))
#define HWIO_I3C_SCL_LOW_I3C_SCL_LOW_COUNTER_BMSK                                                0x3ff
#define HWIO_I3C_SCL_LOW_I3C_SCL_LOW_COUNTER_SHFT                                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: QUPV3_SE_DMA
 *--------------------------------------------------------------------------*/

#define QUPV3_SE_DMA_REG_BASE                                                        (QUPV3_0_QUPV3_ID_1_BASE      + 0x00080c00)
#define QUPV3_SE_DMA_REG_BASE_OFFS                                                   0x00080c00

#define HWIO_DMA_TX_PTR_L_ADDR(x)                                                    ((x) + 0x00000030)
#define HWIO_DMA_TX_PTR_L_OFFS                                                       (0x00000030)
#define HWIO_DMA_TX_PTR_L_RMSK                                                       0xffffffff
#define HWIO_DMA_TX_PTR_L_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_PTR_L_ADDR(x), HWIO_DMA_TX_PTR_L_RMSK)
#define HWIO_DMA_TX_PTR_L_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_PTR_L_ADDR(x), m)
#define HWIO_DMA_TX_PTR_L_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_PTR_L_ADDR(x),v)
#define HWIO_DMA_TX_PTR_L_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_PTR_L_ADDR(x),m,v,HWIO_DMA_TX_PTR_L_IN(x))
#define HWIO_DMA_TX_PTR_L_TX_PTR_L_BMSK                                              0xffffffff
#define HWIO_DMA_TX_PTR_L_TX_PTR_L_SHFT                                                     0x0

#define HWIO_DMA_TX_PTR_H_ADDR(x)                                                    ((x) + 0x00000034)
#define HWIO_DMA_TX_PTR_H_OFFS                                                       (0x00000034)
#define HWIO_DMA_TX_PTR_H_RMSK                                                       0xffffffff
#define HWIO_DMA_TX_PTR_H_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_PTR_H_ADDR(x), HWIO_DMA_TX_PTR_H_RMSK)
#define HWIO_DMA_TX_PTR_H_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_PTR_H_ADDR(x), m)
#define HWIO_DMA_TX_PTR_H_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_PTR_H_ADDR(x),v)
#define HWIO_DMA_TX_PTR_H_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_PTR_H_ADDR(x),m,v,HWIO_DMA_TX_PTR_H_IN(x))
#define HWIO_DMA_TX_PTR_H_TX_PTR_H_BMSK                                              0xffffffff
#define HWIO_DMA_TX_PTR_H_TX_PTR_H_SHFT                                                     0x0

#define HWIO_DMA_TX_ATTR_ADDR(x)                                                     ((x) + 0x00000038)
#define HWIO_DMA_TX_ATTR_OFFS                                                        (0x00000038)
#define HWIO_DMA_TX_ATTR_RMSK                                                             0xf0f
#define HWIO_DMA_TX_ATTR_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_ATTR_ADDR(x), HWIO_DMA_TX_ATTR_RMSK)
#define HWIO_DMA_TX_ATTR_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_ATTR_ADDR(x), m)
#define HWIO_DMA_TX_ATTR_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_ATTR_ADDR(x),v)
#define HWIO_DMA_TX_ATTR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_ATTR_ADDR(x),m,v,HWIO_DMA_TX_ATTR_IN(x))
#define HWIO_DMA_TX_ATTR_GPII_INDEX_BMSK                                                  0xf00
#define HWIO_DMA_TX_ATTR_GPII_INDEX_SHFT                                                    0x8
#define HWIO_DMA_TX_ATTR_REQ_PRIORITY_BMSK                                                  0x8
#define HWIO_DMA_TX_ATTR_REQ_PRIORITY_SHFT                                                  0x3
#define HWIO_DMA_TX_ATTR_DMA_OPERATION_BMSK                                                 0x6
#define HWIO_DMA_TX_ATTR_DMA_OPERATION_SHFT                                                 0x1
#define HWIO_DMA_TX_ATTR_EOT_BMSK                                                           0x1
#define HWIO_DMA_TX_ATTR_EOT_SHFT                                                           0x0

#define HWIO_DMA_TX_LENGTH_ADDR(x)                                                   ((x) + 0x0000003c)
#define HWIO_DMA_TX_LENGTH_OFFS                                                      (0x0000003c)
#define HWIO_DMA_TX_LENGTH_RMSK                                                        0xffffff
#define HWIO_DMA_TX_LENGTH_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_LENGTH_ADDR(x), HWIO_DMA_TX_LENGTH_RMSK)
#define HWIO_DMA_TX_LENGTH_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_LENGTH_ADDR(x), m)
#define HWIO_DMA_TX_LENGTH_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_LENGTH_ADDR(x),v)
#define HWIO_DMA_TX_LENGTH_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_LENGTH_ADDR(x),m,v,HWIO_DMA_TX_LENGTH_IN(x))
#define HWIO_DMA_TX_LENGTH_TX_LENGTH_BMSK                                              0xffffff
#define HWIO_DMA_TX_LENGTH_TX_LENGTH_SHFT                                                   0x0

#define HWIO_DMA_TX_IRQ_STAT_ADDR(x)                                                 ((x) + 0x00000040)
#define HWIO_DMA_TX_IRQ_STAT_OFFS                                                    (0x00000040)
#define HWIO_DMA_TX_IRQ_STAT_RMSK                                                           0xf
#define HWIO_DMA_TX_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_IRQ_STAT_ADDR(x), HWIO_DMA_TX_IRQ_STAT_RMSK)
#define HWIO_DMA_TX_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_IRQ_STAT_ADDR(x), m)
#define HWIO_DMA_TX_IRQ_STAT_RESET_DONE_BMSK                                                0x8
#define HWIO_DMA_TX_IRQ_STAT_RESET_DONE_SHFT                                                0x3
#define HWIO_DMA_TX_IRQ_STAT_SBE_BMSK                                                       0x4
#define HWIO_DMA_TX_IRQ_STAT_SBE_SHFT                                                       0x2
#define HWIO_DMA_TX_IRQ_STAT_EOT_BMSK                                                       0x2
#define HWIO_DMA_TX_IRQ_STAT_EOT_SHFT                                                       0x1
#define HWIO_DMA_TX_IRQ_STAT_DMA_DONE_BMSK                                                  0x1
#define HWIO_DMA_TX_IRQ_STAT_DMA_DONE_SHFT                                                  0x0

#define HWIO_DMA_TX_IRQ_CLR_ADDR(x)                                                  ((x) + 0x00000044)
#define HWIO_DMA_TX_IRQ_CLR_OFFS                                                     (0x00000044)
#define HWIO_DMA_TX_IRQ_CLR_RMSK                                                            0xf
#define HWIO_DMA_TX_IRQ_CLR_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_IRQ_CLR_ADDR(x),v)
#define HWIO_DMA_TX_IRQ_CLR_RESET_DONE_CLR_BMSK                                             0x8
#define HWIO_DMA_TX_IRQ_CLR_RESET_DONE_CLR_SHFT                                             0x3
#define HWIO_DMA_TX_IRQ_CLR_SBE_CLR_BMSK                                                    0x4
#define HWIO_DMA_TX_IRQ_CLR_SBE_CLR_SHFT                                                    0x2
#define HWIO_DMA_TX_IRQ_CLR_EOT_CLR_BMSK                                                    0x2
#define HWIO_DMA_TX_IRQ_CLR_EOT_CLR_SHFT                                                    0x1
#define HWIO_DMA_TX_IRQ_CLR_DMA_DONE_CLR_BMSK                                               0x1
#define HWIO_DMA_TX_IRQ_CLR_DMA_DONE_CLR_SHFT                                               0x0

#define HWIO_DMA_TX_IRQ_EN_ADDR(x)                                                   ((x) + 0x00000048)
#define HWIO_DMA_TX_IRQ_EN_OFFS                                                      (0x00000048)
#define HWIO_DMA_TX_IRQ_EN_RMSK                                                             0xf
#define HWIO_DMA_TX_IRQ_EN_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_IRQ_EN_ADDR(x), HWIO_DMA_TX_IRQ_EN_RMSK)
#define HWIO_DMA_TX_IRQ_EN_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_IRQ_EN_ADDR(x), m)
#define HWIO_DMA_TX_IRQ_EN_RESET_DONE_EN_BMSK                                               0x8
#define HWIO_DMA_TX_IRQ_EN_RESET_DONE_EN_SHFT                                               0x3
#define HWIO_DMA_TX_IRQ_EN_SBE_EN_BMSK                                                      0x4
#define HWIO_DMA_TX_IRQ_EN_SBE_EN_SHFT                                                      0x2
#define HWIO_DMA_TX_IRQ_EN_EOT_EN_BMSK                                                      0x2
#define HWIO_DMA_TX_IRQ_EN_EOT_EN_SHFT                                                      0x1
#define HWIO_DMA_TX_IRQ_EN_DMA_DONE_EN_BMSK                                                 0x1
#define HWIO_DMA_TX_IRQ_EN_DMA_DONE_EN_SHFT                                                 0x0

#define HWIO_DMA_TX_IRQ_EN_SET_ADDR(x)                                               ((x) + 0x0000004c)
#define HWIO_DMA_TX_IRQ_EN_SET_OFFS                                                  (0x0000004c)
#define HWIO_DMA_TX_IRQ_EN_SET_RMSK                                                         0xf
#define HWIO_DMA_TX_IRQ_EN_SET_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_IRQ_EN_SET_ADDR(x),v)
#define HWIO_DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK                                       0x8
#define HWIO_DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_SHFT                                       0x3
#define HWIO_DMA_TX_IRQ_EN_SET_SBE_EN_SET_BMSK                                              0x4
#define HWIO_DMA_TX_IRQ_EN_SET_SBE_EN_SET_SHFT                                              0x2
#define HWIO_DMA_TX_IRQ_EN_SET_EOT_EN_SET_BMSK                                              0x2
#define HWIO_DMA_TX_IRQ_EN_SET_EOT_EN_SET_SHFT                                              0x1
#define HWIO_DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK                                         0x1
#define HWIO_DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_SHFT                                         0x0

#define HWIO_DMA_TX_IRQ_EN_CLR_ADDR(x)                                               ((x) + 0x00000050)
#define HWIO_DMA_TX_IRQ_EN_CLR_OFFS                                                  (0x00000050)
#define HWIO_DMA_TX_IRQ_EN_CLR_RMSK                                                         0xf
#define HWIO_DMA_TX_IRQ_EN_CLR_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_IRQ_EN_CLR_ADDR(x),v)
#define HWIO_DMA_TX_IRQ_EN_CLR_RESET_DONE_EN_CLR_BMSK                                       0x8
#define HWIO_DMA_TX_IRQ_EN_CLR_RESET_DONE_EN_CLR_SHFT                                       0x3
#define HWIO_DMA_TX_IRQ_EN_CLR_SBE_EN_CLR_BMSK                                              0x4
#define HWIO_DMA_TX_IRQ_EN_CLR_SBE_EN_CLR_SHFT                                              0x2
#define HWIO_DMA_TX_IRQ_EN_CLR_EOT_EN_CLR_BMSK                                              0x2
#define HWIO_DMA_TX_IRQ_EN_CLR_EOT_EN_CLR_SHFT                                              0x1
#define HWIO_DMA_TX_IRQ_EN_CLR_DMA_DONE_EN_CLR_BMSK                                         0x1
#define HWIO_DMA_TX_IRQ_EN_CLR_DMA_DONE_EN_CLR_SHFT                                         0x0

#define HWIO_DMA_TX_LENGTH_IN_ADDR(x)                                                ((x) + 0x00000054)
#define HWIO_DMA_TX_LENGTH_IN_OFFS                                                   (0x00000054)
#define HWIO_DMA_TX_LENGTH_IN_RMSK                                                     0xffffff
#define HWIO_DMA_TX_LENGTH_IN_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_LENGTH_IN_ADDR(x), HWIO_DMA_TX_LENGTH_IN_RMSK)
#define HWIO_DMA_TX_LENGTH_IN_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_LENGTH_IN_ADDR(x), m)
#define HWIO_DMA_TX_LENGTH_IN_TX_LENGTH_IN_BMSK                                        0xffffff
#define HWIO_DMA_TX_LENGTH_IN_TX_LENGTH_IN_SHFT                                             0x0

#define HWIO_DMA_TX_FSM_RST_ADDR(x)                                                  ((x) + 0x00000058)
#define HWIO_DMA_TX_FSM_RST_OFFS                                                     (0x00000058)
#define HWIO_DMA_TX_FSM_RST_RMSK                                                            0x1
#define HWIO_DMA_TX_FSM_RST_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_FSM_RST_ADDR(x), HWIO_DMA_TX_FSM_RST_RMSK)
#define HWIO_DMA_TX_FSM_RST_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_FSM_RST_ADDR(x), m)
#define HWIO_DMA_TX_FSM_RST_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_FSM_RST_ADDR(x),v)
#define HWIO_DMA_TX_FSM_RST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_FSM_RST_ADDR(x),m,v,HWIO_DMA_TX_FSM_RST_IN(x))
#define HWIO_DMA_TX_FSM_RST_TX_FSM_RST_BMSK                                                 0x1
#define HWIO_DMA_TX_FSM_RST_TX_FSM_RST_SHFT                                                 0x0

#define HWIO_DMA_TX_MAX_BURST_SIZE_ADDR(x)                                           ((x) + 0x0000005c)
#define HWIO_DMA_TX_MAX_BURST_SIZE_OFFS                                              (0x0000005c)
#define HWIO_DMA_TX_MAX_BURST_SIZE_RMSK                                                     0x3
#define HWIO_DMA_TX_MAX_BURST_SIZE_IN(x)      \
        in_dword_masked(HWIO_DMA_TX_MAX_BURST_SIZE_ADDR(x), HWIO_DMA_TX_MAX_BURST_SIZE_RMSK)
#define HWIO_DMA_TX_MAX_BURST_SIZE_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TX_MAX_BURST_SIZE_ADDR(x), m)
#define HWIO_DMA_TX_MAX_BURST_SIZE_OUT(x, v)      \
        out_dword(HWIO_DMA_TX_MAX_BURST_SIZE_ADDR(x),v)
#define HWIO_DMA_TX_MAX_BURST_SIZE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TX_MAX_BURST_SIZE_ADDR(x),m,v,HWIO_DMA_TX_MAX_BURST_SIZE_IN(x))
#define HWIO_DMA_TX_MAX_BURST_SIZE_TX_MAX_BURST_SIZE_BMSK                                   0x3
#define HWIO_DMA_TX_MAX_BURST_SIZE_TX_MAX_BURST_SIZE_SHFT                                   0x0

#define HWIO_DMA_RX_PTR_L_ADDR(x)                                                    ((x) + 0x00000130)
#define HWIO_DMA_RX_PTR_L_OFFS                                                       (0x00000130)
#define HWIO_DMA_RX_PTR_L_RMSK                                                       0xffffffff
#define HWIO_DMA_RX_PTR_L_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_PTR_L_ADDR(x), HWIO_DMA_RX_PTR_L_RMSK)
#define HWIO_DMA_RX_PTR_L_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_PTR_L_ADDR(x), m)
#define HWIO_DMA_RX_PTR_L_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_PTR_L_ADDR(x),v)
#define HWIO_DMA_RX_PTR_L_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_PTR_L_ADDR(x),m,v,HWIO_DMA_RX_PTR_L_IN(x))
#define HWIO_DMA_RX_PTR_L_RX_PTR_L_BMSK                                              0xffffffff
#define HWIO_DMA_RX_PTR_L_RX_PTR_L_SHFT                                                     0x0

#define HWIO_DMA_RX_PTR_H_ADDR(x)                                                    ((x) + 0x00000134)
#define HWIO_DMA_RX_PTR_H_OFFS                                                       (0x00000134)
#define HWIO_DMA_RX_PTR_H_RMSK                                                       0xffffffff
#define HWIO_DMA_RX_PTR_H_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_PTR_H_ADDR(x), HWIO_DMA_RX_PTR_H_RMSK)
#define HWIO_DMA_RX_PTR_H_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_PTR_H_ADDR(x), m)
#define HWIO_DMA_RX_PTR_H_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_PTR_H_ADDR(x),v)
#define HWIO_DMA_RX_PTR_H_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_PTR_H_ADDR(x),m,v,HWIO_DMA_RX_PTR_H_IN(x))
#define HWIO_DMA_RX_PTR_H_RX_PTR_H_BMSK                                              0xffffffff
#define HWIO_DMA_RX_PTR_H_RX_PTR_H_SHFT                                                     0x0

#define HWIO_DMA_RX_ATTR_ADDR(x)                                                     ((x) + 0x00000138)
#define HWIO_DMA_RX_ATTR_OFFS                                                        (0x00000138)
#define HWIO_DMA_RX_ATTR_RMSK                                                             0xf0e
#define HWIO_DMA_RX_ATTR_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_ATTR_ADDR(x), HWIO_DMA_RX_ATTR_RMSK)
#define HWIO_DMA_RX_ATTR_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_ATTR_ADDR(x), m)
#define HWIO_DMA_RX_ATTR_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_ATTR_ADDR(x),v)
#define HWIO_DMA_RX_ATTR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_ATTR_ADDR(x),m,v,HWIO_DMA_RX_ATTR_IN(x))
#define HWIO_DMA_RX_ATTR_GPII_INDEX_BMSK                                                  0xf00
#define HWIO_DMA_RX_ATTR_GPII_INDEX_SHFT                                                    0x8
#define HWIO_DMA_RX_ATTR_REQ_PRIORITY_BMSK                                                  0x8
#define HWIO_DMA_RX_ATTR_REQ_PRIORITY_SHFT                                                  0x3
#define HWIO_DMA_RX_ATTR_DMA_OPERATION_BMSK                                                 0x6
#define HWIO_DMA_RX_ATTR_DMA_OPERATION_SHFT                                                 0x1

#define HWIO_DMA_RX_LENGTH_ADDR(x)                                                   ((x) + 0x0000013c)
#define HWIO_DMA_RX_LENGTH_OFFS                                                      (0x0000013c)
#define HWIO_DMA_RX_LENGTH_RMSK                                                        0xffffff
#define HWIO_DMA_RX_LENGTH_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_LENGTH_ADDR(x), HWIO_DMA_RX_LENGTH_RMSK)
#define HWIO_DMA_RX_LENGTH_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_LENGTH_ADDR(x), m)
#define HWIO_DMA_RX_LENGTH_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_LENGTH_ADDR(x),v)
#define HWIO_DMA_RX_LENGTH_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_LENGTH_ADDR(x),m,v,HWIO_DMA_RX_LENGTH_IN(x))
#define HWIO_DMA_RX_LENGTH_RX_LENGTH_BMSK                                              0xffffff
#define HWIO_DMA_RX_LENGTH_RX_LENGTH_SHFT                                                   0x0

#define HWIO_DMA_RX_IRQ_STAT_ADDR(x)                                                 ((x) + 0x00000140)
#define HWIO_DMA_RX_IRQ_STAT_OFFS                                                    (0x00000140)
#define HWIO_DMA_RX_IRQ_STAT_RMSK                                                         0xfff
#define HWIO_DMA_RX_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_IRQ_STAT_ADDR(x), HWIO_DMA_RX_IRQ_STAT_RMSK)
#define HWIO_DMA_RX_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_IRQ_STAT_ADDR(x), m)
#define HWIO_DMA_RX_IRQ_STAT_GENI_CANCEL_IRQ_BMSK                                         0x800
#define HWIO_DMA_RX_IRQ_STAT_GENI_CANCEL_IRQ_SHFT                                           0xb
#define HWIO_DMA_RX_IRQ_STAT_GENI_GP_IRQ_BMSK                                             0x7e0
#define HWIO_DMA_RX_IRQ_STAT_GENI_GP_IRQ_SHFT                                               0x5
#define HWIO_DMA_RX_IRQ_STAT_FLUSH_DONE_BMSK                                               0x10
#define HWIO_DMA_RX_IRQ_STAT_FLUSH_DONE_SHFT                                                0x4
#define HWIO_DMA_RX_IRQ_STAT_RESET_DONE_BMSK                                                0x8
#define HWIO_DMA_RX_IRQ_STAT_RESET_DONE_SHFT                                                0x3
#define HWIO_DMA_RX_IRQ_STAT_SBE_BMSK                                                       0x4
#define HWIO_DMA_RX_IRQ_STAT_SBE_SHFT                                                       0x2
#define HWIO_DMA_RX_IRQ_STAT_EOT_BMSK                                                       0x2
#define HWIO_DMA_RX_IRQ_STAT_EOT_SHFT                                                       0x1
#define HWIO_DMA_RX_IRQ_STAT_DMA_DONE_BMSK                                                  0x1
#define HWIO_DMA_RX_IRQ_STAT_DMA_DONE_SHFT                                                  0x0

#define HWIO_DMA_RX_IRQ_CLR_ADDR(x)                                                  ((x) + 0x00000144)
#define HWIO_DMA_RX_IRQ_CLR_OFFS                                                     (0x00000144)
#define HWIO_DMA_RX_IRQ_CLR_RMSK                                                          0xfff
#define HWIO_DMA_RX_IRQ_CLR_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_IRQ_CLR_ADDR(x),v)
#define HWIO_DMA_RX_IRQ_CLR_GENI_CANCEL_IRQ_CLR_BMSK                                      0x800
#define HWIO_DMA_RX_IRQ_CLR_GENI_CANCEL_IRQ_CLR_SHFT                                        0xb
#define HWIO_DMA_RX_IRQ_CLR_GENI_GP_IRQ_CLR_BMSK                                          0x7e0
#define HWIO_DMA_RX_IRQ_CLR_GENI_GP_IRQ_CLR_SHFT                                            0x5
#define HWIO_DMA_RX_IRQ_CLR_FLUSH_DONE_CLR_BMSK                                            0x10
#define HWIO_DMA_RX_IRQ_CLR_FLUSH_DONE_CLR_SHFT                                             0x4
#define HWIO_DMA_RX_IRQ_CLR_RESET_DONE_CLR_BMSK                                             0x8
#define HWIO_DMA_RX_IRQ_CLR_RESET_DONE_CLR_SHFT                                             0x3
#define HWIO_DMA_RX_IRQ_CLR_SBE_CLR_BMSK                                                    0x4
#define HWIO_DMA_RX_IRQ_CLR_SBE_CLR_SHFT                                                    0x2
#define HWIO_DMA_RX_IRQ_CLR_EOT_CLR_BMSK                                                    0x2
#define HWIO_DMA_RX_IRQ_CLR_EOT_CLR_SHFT                                                    0x1
#define HWIO_DMA_RX_IRQ_CLR_DMA_DONE_CLR_BMSK                                               0x1
#define HWIO_DMA_RX_IRQ_CLR_DMA_DONE_CLR_SHFT                                               0x0

#define HWIO_DMA_RX_IRQ_EN_ADDR(x)                                                   ((x) + 0x00000148)
#define HWIO_DMA_RX_IRQ_EN_OFFS                                                      (0x00000148)
#define HWIO_DMA_RX_IRQ_EN_RMSK                                                           0xfff
#define HWIO_DMA_RX_IRQ_EN_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_IRQ_EN_ADDR(x), HWIO_DMA_RX_IRQ_EN_RMSK)
#define HWIO_DMA_RX_IRQ_EN_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_IRQ_EN_ADDR(x), m)
#define HWIO_DMA_RX_IRQ_EN_GENI_CANCEL_IRQ_EN_BMSK                                        0x800
#define HWIO_DMA_RX_IRQ_EN_GENI_CANCEL_IRQ_EN_SHFT                                          0xb
#define HWIO_DMA_RX_IRQ_EN_GENI_GP_IRQ_EN_BMSK                                            0x7e0
#define HWIO_DMA_RX_IRQ_EN_GENI_GP_IRQ_EN_SHFT                                              0x5
#define HWIO_DMA_RX_IRQ_EN_FLUSH_DONE_EN_BMSK                                              0x10
#define HWIO_DMA_RX_IRQ_EN_FLUSH_DONE_EN_SHFT                                               0x4
#define HWIO_DMA_RX_IRQ_EN_RESET_DONE_EN_BMSK                                               0x8
#define HWIO_DMA_RX_IRQ_EN_RESET_DONE_EN_SHFT                                               0x3
#define HWIO_DMA_RX_IRQ_EN_SBE_EN_BMSK                                                      0x4
#define HWIO_DMA_RX_IRQ_EN_SBE_EN_SHFT                                                      0x2
#define HWIO_DMA_RX_IRQ_EN_EOT_EN_BMSK                                                      0x2
#define HWIO_DMA_RX_IRQ_EN_EOT_EN_SHFT                                                      0x1
#define HWIO_DMA_RX_IRQ_EN_DMA_DONE_EN_BMSK                                                 0x1
#define HWIO_DMA_RX_IRQ_EN_DMA_DONE_EN_SHFT                                                 0x0

#define HWIO_DMA_RX_IRQ_EN_SET_ADDR(x)                                               ((x) + 0x0000014c)
#define HWIO_DMA_RX_IRQ_EN_SET_OFFS                                                  (0x0000014c)
#define HWIO_DMA_RX_IRQ_EN_SET_RMSK                                                       0xfff
#define HWIO_DMA_RX_IRQ_EN_SET_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_IRQ_EN_SET_ADDR(x),v)
#define HWIO_DMA_RX_IRQ_EN_SET_GENI_CANCEL_IRQ_EN_SET_BMSK                                0x800
#define HWIO_DMA_RX_IRQ_EN_SET_GENI_CANCEL_IRQ_EN_SET_SHFT                                  0xb
#define HWIO_DMA_RX_IRQ_EN_SET_GENI_GP_IRQ_EN_SET_BMSK                                    0x7e0
#define HWIO_DMA_RX_IRQ_EN_SET_GENI_GP_IRQ_EN_SET_SHFT                                      0x5
#define HWIO_DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_BMSK                                      0x10
#define HWIO_DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_SHFT                                       0x4
#define HWIO_DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK                                       0x8
#define HWIO_DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_SHFT                                       0x3
#define HWIO_DMA_RX_IRQ_EN_SET_SBE_EN_SET_BMSK                                              0x4
#define HWIO_DMA_RX_IRQ_EN_SET_SBE_EN_SET_SHFT                                              0x2
#define HWIO_DMA_RX_IRQ_EN_SET_EOT_EN_SET_BMSK                                              0x2
#define HWIO_DMA_RX_IRQ_EN_SET_EOT_EN_SET_SHFT                                              0x1
#define HWIO_DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK                                         0x1
#define HWIO_DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_SHFT                                         0x0

#define HWIO_DMA_RX_IRQ_EN_CLR_ADDR(x)                                               ((x) + 0x00000150)
#define HWIO_DMA_RX_IRQ_EN_CLR_OFFS                                                  (0x00000150)
#define HWIO_DMA_RX_IRQ_EN_CLR_RMSK                                                       0xfff
#define HWIO_DMA_RX_IRQ_EN_CLR_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_IRQ_EN_CLR_ADDR(x),v)
#define HWIO_DMA_RX_IRQ_EN_CLR_GENI_CANCEL_IRQ_EN_CLR_BMSK                                0x800
#define HWIO_DMA_RX_IRQ_EN_CLR_GENI_CANCEL_IRQ_EN_CLR_SHFT                                  0xb
#define HWIO_DMA_RX_IRQ_EN_CLR_GENI_GP_IRQ_EN_CLR_BMSK                                    0x7e0
#define HWIO_DMA_RX_IRQ_EN_CLR_GENI_GP_IRQ_EN_CLR_SHFT                                      0x5
#define HWIO_DMA_RX_IRQ_EN_CLR_FLUSH_DONE_EN_CLR_BMSK                                      0x10
#define HWIO_DMA_RX_IRQ_EN_CLR_FLUSH_DONE_EN_CLR_SHFT                                       0x4
#define HWIO_DMA_RX_IRQ_EN_CLR_RESET_DONE_EN_CLR_BMSK                                       0x8
#define HWIO_DMA_RX_IRQ_EN_CLR_RESET_DONE_EN_CLR_SHFT                                       0x3
#define HWIO_DMA_RX_IRQ_EN_CLR_SBE_EN_CLR_BMSK                                              0x4
#define HWIO_DMA_RX_IRQ_EN_CLR_SBE_EN_CLR_SHFT                                              0x2
#define HWIO_DMA_RX_IRQ_EN_CLR_EOT_EN_CLR_BMSK                                              0x2
#define HWIO_DMA_RX_IRQ_EN_CLR_EOT_EN_CLR_SHFT                                              0x1
#define HWIO_DMA_RX_IRQ_EN_CLR_DMA_DONE_EN_CLR_BMSK                                         0x1
#define HWIO_DMA_RX_IRQ_EN_CLR_DMA_DONE_EN_CLR_SHFT                                         0x0

#define HWIO_DMA_RX_LENGTH_IN_ADDR(x)                                                ((x) + 0x00000154)
#define HWIO_DMA_RX_LENGTH_IN_OFFS                                                   (0x00000154)
#define HWIO_DMA_RX_LENGTH_IN_RMSK                                                     0xffffff
#define HWIO_DMA_RX_LENGTH_IN_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_LENGTH_IN_ADDR(x), HWIO_DMA_RX_LENGTH_IN_RMSK)
#define HWIO_DMA_RX_LENGTH_IN_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_LENGTH_IN_ADDR(x), m)
#define HWIO_DMA_RX_LENGTH_IN_RX_LENGTH_IN_BMSK                                        0xffffff
#define HWIO_DMA_RX_LENGTH_IN_RX_LENGTH_IN_SHFT                                             0x0

#define HWIO_DMA_RX_FSM_RST_ADDR(x)                                                  ((x) + 0x00000158)
#define HWIO_DMA_RX_FSM_RST_OFFS                                                     (0x00000158)
#define HWIO_DMA_RX_FSM_RST_RMSK                                                            0x1
#define HWIO_DMA_RX_FSM_RST_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_FSM_RST_ADDR(x), HWIO_DMA_RX_FSM_RST_RMSK)
#define HWIO_DMA_RX_FSM_RST_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_FSM_RST_ADDR(x), m)
#define HWIO_DMA_RX_FSM_RST_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_FSM_RST_ADDR(x),v)
#define HWIO_DMA_RX_FSM_RST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_FSM_RST_ADDR(x),m,v,HWIO_DMA_RX_FSM_RST_IN(x))
#define HWIO_DMA_RX_FSM_RST_RX_FSM_RST_BMSK                                                 0x1
#define HWIO_DMA_RX_FSM_RST_RX_FSM_RST_SHFT                                                 0x0

#define HWIO_DMA_RX_MAX_BURST_SIZE_ADDR(x)                                           ((x) + 0x0000015c)
#define HWIO_DMA_RX_MAX_BURST_SIZE_OFFS                                              (0x0000015c)
#define HWIO_DMA_RX_MAX_BURST_SIZE_RMSK                                                     0x3
#define HWIO_DMA_RX_MAX_BURST_SIZE_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_MAX_BURST_SIZE_ADDR(x), HWIO_DMA_RX_MAX_BURST_SIZE_RMSK)
#define HWIO_DMA_RX_MAX_BURST_SIZE_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_MAX_BURST_SIZE_ADDR(x), m)
#define HWIO_DMA_RX_MAX_BURST_SIZE_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_MAX_BURST_SIZE_ADDR(x),v)
#define HWIO_DMA_RX_MAX_BURST_SIZE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_MAX_BURST_SIZE_ADDR(x),m,v,HWIO_DMA_RX_MAX_BURST_SIZE_IN(x))
#define HWIO_DMA_RX_MAX_BURST_SIZE_RX_MAX_BURST_SIZE_BMSK                                   0x3
#define HWIO_DMA_RX_MAX_BURST_SIZE_RX_MAX_BURST_SIZE_SHFT                                   0x0

#define HWIO_DMA_RX_FLUSH_ADDR(x)                                                    ((x) + 0x00000160)
#define HWIO_DMA_RX_FLUSH_OFFS                                                       (0x00000160)
#define HWIO_DMA_RX_FLUSH_RMSK                                                              0x1
#define HWIO_DMA_RX_FLUSH_IN(x)      \
        in_dword_masked(HWIO_DMA_RX_FLUSH_ADDR(x), HWIO_DMA_RX_FLUSH_RMSK)
#define HWIO_DMA_RX_FLUSH_INM(x, m)      \
        in_dword_masked(HWIO_DMA_RX_FLUSH_ADDR(x), m)
#define HWIO_DMA_RX_FLUSH_OUT(x, v)      \
        out_dword(HWIO_DMA_RX_FLUSH_ADDR(x),v)
#define HWIO_DMA_RX_FLUSH_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_RX_FLUSH_ADDR(x),m,v,HWIO_DMA_RX_FLUSH_IN(x))
#define HWIO_DMA_RX_FLUSH_RX_FLUSH_BMSK                                                     0x1
#define HWIO_DMA_RX_FLUSH_RX_FLUSH_SHFT                                                     0x0

#define HWIO_SE_IRQ_HIGH_PRIORITY_ADDR(x)                                            ((x) + 0x00000214)
#define HWIO_SE_IRQ_HIGH_PRIORITY_OFFS                                               (0x00000214)
#define HWIO_SE_IRQ_HIGH_PRIORITY_RMSK                                                      0xf
#define HWIO_SE_IRQ_HIGH_PRIORITY_IN(x)      \
        in_dword_masked(HWIO_SE_IRQ_HIGH_PRIORITY_ADDR(x), HWIO_SE_IRQ_HIGH_PRIORITY_RMSK)
#define HWIO_SE_IRQ_HIGH_PRIORITY_INM(x, m)      \
        in_dword_masked(HWIO_SE_IRQ_HIGH_PRIORITY_ADDR(x), m)
#define HWIO_SE_IRQ_HIGH_PRIORITY_OUT(x, v)      \
        out_dword(HWIO_SE_IRQ_HIGH_PRIORITY_ADDR(x),v)
#define HWIO_SE_IRQ_HIGH_PRIORITY_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_IRQ_HIGH_PRIORITY_ADDR(x),m,v,HWIO_SE_IRQ_HIGH_PRIORITY_IN(x))
#define HWIO_SE_IRQ_HIGH_PRIORITY_GENI_S_IRQ_HIGH_PRIORITY_BMSK                             0x8
#define HWIO_SE_IRQ_HIGH_PRIORITY_GENI_S_IRQ_HIGH_PRIORITY_SHFT                             0x3
#define HWIO_SE_IRQ_HIGH_PRIORITY_GENI_M_IRQ_HIGH_PRIORITY_BMSK                             0x4
#define HWIO_SE_IRQ_HIGH_PRIORITY_GENI_M_IRQ_HIGH_PRIORITY_SHFT                             0x2
#define HWIO_SE_IRQ_HIGH_PRIORITY_RX_IRQ_HIGH_PRIORITY_BMSK                                 0x2
#define HWIO_SE_IRQ_HIGH_PRIORITY_RX_IRQ_HIGH_PRIORITY_SHFT                                 0x1
#define HWIO_SE_IRQ_HIGH_PRIORITY_TX_IRQ_HIGH_PRIORITY_BMSK                                 0x1
#define HWIO_SE_IRQ_HIGH_PRIORITY_TX_IRQ_HIGH_PRIORITY_SHFT                                 0x0

#define HWIO_SE_GSI_EVENT_EN_ADDR(x)                                                 ((x) + 0x00000218)
#define HWIO_SE_GSI_EVENT_EN_OFFS                                                    (0x00000218)
#define HWIO_SE_GSI_EVENT_EN_RMSK                                                           0xf
#define HWIO_SE_GSI_EVENT_EN_IN(x)      \
        in_dword_masked(HWIO_SE_GSI_EVENT_EN_ADDR(x), HWIO_SE_GSI_EVENT_EN_RMSK)
#define HWIO_SE_GSI_EVENT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SE_GSI_EVENT_EN_ADDR(x), m)
#define HWIO_SE_GSI_EVENT_EN_OUT(x, v)      \
        out_dword(HWIO_SE_GSI_EVENT_EN_ADDR(x),v)
#define HWIO_SE_GSI_EVENT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_GSI_EVENT_EN_ADDR(x),m,v,HWIO_SE_GSI_EVENT_EN_IN(x))
#define HWIO_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_BMSK                                           0x8
#define HWIO_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_SHFT                                           0x3
#define HWIO_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_BMSK                                           0x4
#define HWIO_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_SHFT                                           0x2
#define HWIO_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_BMSK                                           0x2
#define HWIO_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_SHFT                                           0x1
#define HWIO_SE_GSI_EVENT_EN_DMA_RX_EVENT_EN_BMSK                                           0x1
#define HWIO_SE_GSI_EVENT_EN_DMA_RX_EVENT_EN_SHFT                                           0x0

#define HWIO_SE_IRQ_EN_ADDR(x)                                                       ((x) + 0x0000021c)
#define HWIO_SE_IRQ_EN_OFFS                                                          (0x0000021c)
#define HWIO_SE_IRQ_EN_RMSK                                                                 0xf
#define HWIO_SE_IRQ_EN_IN(x)      \
        in_dword_masked(HWIO_SE_IRQ_EN_ADDR(x), HWIO_SE_IRQ_EN_RMSK)
#define HWIO_SE_IRQ_EN_INM(x, m)      \
        in_dword_masked(HWIO_SE_IRQ_EN_ADDR(x), m)
#define HWIO_SE_IRQ_EN_OUT(x, v)      \
        out_dword(HWIO_SE_IRQ_EN_ADDR(x),v)
#define HWIO_SE_IRQ_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_IRQ_EN_ADDR(x),m,v,HWIO_SE_IRQ_EN_IN(x))
#define HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_BMSK                                                   0x8
#define HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_SHFT                                                   0x3
#define HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_BMSK                                                   0x4
#define HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_SHFT                                                   0x2
#define HWIO_SE_IRQ_EN_DMA_TX_IRQ_EN_BMSK                                                   0x2
#define HWIO_SE_IRQ_EN_DMA_TX_IRQ_EN_SHFT                                                   0x1
#define HWIO_SE_IRQ_EN_DMA_RX_IRQ_EN_BMSK                                                   0x1
#define HWIO_SE_IRQ_EN_DMA_RX_IRQ_EN_SHFT                                                   0x0

#define HWIO_DMA_IF_EN_RO_ADDR(x)                                                    ((x) + 0x00000220)
#define HWIO_DMA_IF_EN_RO_OFFS                                                       (0x00000220)
#define HWIO_DMA_IF_EN_RO_RMSK                                                              0x1
#define HWIO_DMA_IF_EN_RO_IN(x)      \
        in_dword_masked(HWIO_DMA_IF_EN_RO_ADDR(x), HWIO_DMA_IF_EN_RO_RMSK)
#define HWIO_DMA_IF_EN_RO_INM(x, m)      \
        in_dword_masked(HWIO_DMA_IF_EN_RO_ADDR(x), m)
#define HWIO_DMA_IF_EN_RO_DMA_IF_EN_BMSK                                                    0x1
#define HWIO_DMA_IF_EN_RO_DMA_IF_EN_SHFT                                                    0x0

#define HWIO_SE_HW_PARAM_0_ADDR(x)                                                   ((x) + 0x00000224)
#define HWIO_SE_HW_PARAM_0_OFFS                                                      (0x00000224)
#define HWIO_SE_HW_PARAM_0_RMSK                                                      0x3f3f79ff
#define HWIO_SE_HW_PARAM_0_IN(x)      \
        in_dword_masked(HWIO_SE_HW_PARAM_0_ADDR(x), HWIO_SE_HW_PARAM_0_RMSK)
#define HWIO_SE_HW_PARAM_0_INM(x, m)      \
        in_dword_masked(HWIO_SE_HW_PARAM_0_ADDR(x), m)
#define HWIO_SE_HW_PARAM_0_TX_FIFO_WIDTH_BMSK                                        0x3f000000
#define HWIO_SE_HW_PARAM_0_TX_FIFO_WIDTH_SHFT                                              0x18
#define HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK                                          0x3f0000
#define HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT                                              0x10
#define HWIO_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_BMSK                                      0x7000
#define HWIO_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_SHFT                                         0xc
#define HWIO_SE_HW_PARAM_0_TX_FIFO_EN_BMSK                                                0x800
#define HWIO_SE_HW_PARAM_0_TX_FIFO_EN_SHFT                                                  0xb
#define HWIO_SE_HW_PARAM_0_GEN_I3C_BMSK                                                   0x100
#define HWIO_SE_HW_PARAM_0_GEN_I3C_SHFT                                                     0x8
#define HWIO_SE_HW_PARAM_0_GEN_PROG_ROM_BMSK                                               0x80
#define HWIO_SE_HW_PARAM_0_GEN_PROG_ROM_SHFT                                                0x7
#define HWIO_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_BMSK                                         0x40
#define HWIO_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_SHFT                                          0x6
#define HWIO_SE_HW_PARAM_0_AHB_M_ADDR_W_BMSK                                               0x3f
#define HWIO_SE_HW_PARAM_0_AHB_M_ADDR_W_SHFT                                                0x0

#define HWIO_SE_HW_PARAM_1_ADDR(x)                                                   ((x) + 0x00000228)
#define HWIO_SE_HW_PARAM_1_OFFS                                                      (0x00000228)
#define HWIO_SE_HW_PARAM_1_RMSK                                                      0x3f3f7fff
#define HWIO_SE_HW_PARAM_1_IN(x)      \
        in_dword_masked(HWIO_SE_HW_PARAM_1_ADDR(x), HWIO_SE_HW_PARAM_1_RMSK)
#define HWIO_SE_HW_PARAM_1_INM(x, m)      \
        in_dword_masked(HWIO_SE_HW_PARAM_1_ADDR(x), m)
#define HWIO_SE_HW_PARAM_1_RX_FIFO_WIDTH_BMSK                                        0x3f000000
#define HWIO_SE_HW_PARAM_1_RX_FIFO_WIDTH_SHFT                                              0x18
#define HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK                                          0x3f0000
#define HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT                                              0x10
#define HWIO_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_BMSK                                      0x7000
#define HWIO_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_SHFT                                         0xc
#define HWIO_SE_HW_PARAM_1_RX_FIFO_EN_BMSK                                                0x800
#define HWIO_SE_HW_PARAM_1_RX_FIFO_EN_SHFT                                                  0xb
#define HWIO_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_BMSK                                         0x600
#define HWIO_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_SHFT                                           0x9
#define HWIO_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_BMSK                                      0x1ff
#define HWIO_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_SHFT                                        0x0

#define HWIO_DMA_GENERAL_CFG_ADDR(x)                                                 ((x) + 0x00000230)
#define HWIO_DMA_GENERAL_CFG_OFFS                                                    (0x00000230)
#define HWIO_DMA_GENERAL_CFG_RMSK                                                         0x1ff
#define HWIO_DMA_GENERAL_CFG_IN(x)      \
        in_dword_masked(HWIO_DMA_GENERAL_CFG_ADDR(x), HWIO_DMA_GENERAL_CFG_RMSK)
#define HWIO_DMA_GENERAL_CFG_INM(x, m)      \
        in_dword_masked(HWIO_DMA_GENERAL_CFG_ADDR(x), m)
#define HWIO_DMA_GENERAL_CFG_OUT(x, v)      \
        out_dword(HWIO_DMA_GENERAL_CFG_ADDR(x),v)
#define HWIO_DMA_GENERAL_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_GENERAL_CFG_ADDR(x),m,v,HWIO_DMA_GENERAL_CFG_IN(x))
#define HWIO_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_BMSK                                        0x1c0
#define HWIO_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_SHFT                                          0x6
#define HWIO_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_BMSK                                   0x20
#define HWIO_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_SHFT                                    0x5
#define HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK                                    0x8
#define HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_SHFT                                    0x3
#define HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK                                    0x4
#define HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_SHFT                                    0x2
#define HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK                                         0x2
#define HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_SHFT                                         0x1
#define HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK                                         0x1
#define HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_SHFT                                         0x0

#define HWIO_DMA_DEBUG_REG0_ADDR(x)                                                  ((x) + 0x00000240)
#define HWIO_DMA_DEBUG_REG0_OFFS                                                     (0x00000240)
#define HWIO_DMA_DEBUG_REG0_RMSK                                                          0xff3
#define HWIO_DMA_DEBUG_REG0_IN(x)      \
        in_dword_masked(HWIO_DMA_DEBUG_REG0_ADDR(x), HWIO_DMA_DEBUG_REG0_RMSK)
#define HWIO_DMA_DEBUG_REG0_INM(x, m)      \
        in_dword_masked(HWIO_DMA_DEBUG_REG0_ADDR(x), m)
#define HWIO_DMA_DEBUG_REG0_DMA_RX_STATE_BMSK                                             0xf00
#define HWIO_DMA_DEBUG_REG0_DMA_RX_STATE_SHFT                                               0x8
#define HWIO_DMA_DEBUG_REG0_DMA_TX_STATE_BMSK                                              0xf0
#define HWIO_DMA_DEBUG_REG0_DMA_TX_STATE_SHFT                                               0x4
#define HWIO_DMA_DEBUG_REG0_DMA_RX_ACTIVE_BMSK                                              0x2
#define HWIO_DMA_DEBUG_REG0_DMA_RX_ACTIVE_SHFT                                              0x1
#define HWIO_DMA_DEBUG_REG0_DMA_TX_ACTIVE_BMSK                                              0x1
#define HWIO_DMA_DEBUG_REG0_DMA_TX_ACTIVE_SHFT                                              0x0

#define HWIO_DMA_TEST_BUS_CTRL_ADDR(x)                                               ((x) + 0x00000244)
#define HWIO_DMA_TEST_BUS_CTRL_OFFS                                                  (0x00000244)
#define HWIO_DMA_TEST_BUS_CTRL_RMSK                                                         0x7
#define HWIO_DMA_TEST_BUS_CTRL_IN(x)      \
        in_dword_masked(HWIO_DMA_TEST_BUS_CTRL_ADDR(x), HWIO_DMA_TEST_BUS_CTRL_RMSK)
#define HWIO_DMA_TEST_BUS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_DMA_TEST_BUS_CTRL_ADDR(x), m)
#define HWIO_DMA_TEST_BUS_CTRL_OUT(x, v)      \
        out_dword(HWIO_DMA_TEST_BUS_CTRL_ADDR(x),v)
#define HWIO_DMA_TEST_BUS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_TEST_BUS_CTRL_ADDR(x),m,v,HWIO_DMA_TEST_BUS_CTRL_IN(x))
#define HWIO_DMA_TEST_BUS_CTRL_DMA_TEST_BUS_SEL_BMSK                                        0x7
#define HWIO_DMA_TEST_BUS_CTRL_DMA_TEST_BUS_SEL_SHFT                                        0x0

#define HWIO_SE_TOP_TEST_BUS_CTRL_ADDR(x)                                            ((x) + 0x00000248)
#define HWIO_SE_TOP_TEST_BUS_CTRL_OFFS                                               (0x00000248)
#define HWIO_SE_TOP_TEST_BUS_CTRL_RMSK                                                      0x3
#define HWIO_SE_TOP_TEST_BUS_CTRL_IN(x)      \
        in_dword_masked(HWIO_SE_TOP_TEST_BUS_CTRL_ADDR(x), HWIO_SE_TOP_TEST_BUS_CTRL_RMSK)
#define HWIO_SE_TOP_TEST_BUS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_SE_TOP_TEST_BUS_CTRL_ADDR(x), m)
#define HWIO_SE_TOP_TEST_BUS_CTRL_OUT(x, v)      \
        out_dword(HWIO_SE_TOP_TEST_BUS_CTRL_ADDR(x),v)
#define HWIO_SE_TOP_TEST_BUS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SE_TOP_TEST_BUS_CTRL_ADDR(x),m,v,HWIO_SE_TOP_TEST_BUS_CTRL_IN(x))
#define HWIO_SE_TOP_TEST_BUS_CTRL_SE_TOP_TEST_BUS_SEL_BMSK                                  0x3
#define HWIO_SE_TOP_TEST_BUS_CTRL_SE_TOP_TEST_BUS_SEL_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: QUPV3_SEC
 *--------------------------------------------------------------------------*/

#define QUPV3_SEC_REG_BASE                                                               (QUPV3_0_QUPV3_ID_1_BASE      + 0x00082000)
#define QUPV3_SEC_REG_BASE_OFFS                                                          0x00082000

#define HWIO_GENI_CLK_CTRL_ADDR(x)                                                       ((x) + 0x00000000)
#define HWIO_GENI_CLK_CTRL_OFFS                                                          (0x00000000)
#define HWIO_GENI_CLK_CTRL_RMSK                                                                 0x1
#define HWIO_GENI_CLK_CTRL_IN(x)      \
        in_dword_masked(HWIO_GENI_CLK_CTRL_ADDR(x), HWIO_GENI_CLK_CTRL_RMSK)
#define HWIO_GENI_CLK_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_GENI_CLK_CTRL_ADDR(x), m)
#define HWIO_GENI_CLK_CTRL_OUT(x, v)      \
        out_dword(HWIO_GENI_CLK_CTRL_ADDR(x),v)
#define HWIO_GENI_CLK_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_CLK_CTRL_ADDR(x),m,v,HWIO_GENI_CLK_CTRL_IN(x))
#define HWIO_GENI_CLK_CTRL_SER_CLK_SEL_BMSK                                                     0x1
#define HWIO_GENI_CLK_CTRL_SER_CLK_SEL_SHFT                                                     0x0

#define HWIO_DMA_IF_EN_ADDR(x)                                                           ((x) + 0x00000004)
#define HWIO_DMA_IF_EN_OFFS                                                              (0x00000004)
#define HWIO_DMA_IF_EN_RMSK                                                                     0x1
#define HWIO_DMA_IF_EN_IN(x)      \
        in_dword_masked(HWIO_DMA_IF_EN_ADDR(x), HWIO_DMA_IF_EN_RMSK)
#define HWIO_DMA_IF_EN_INM(x, m)      \
        in_dword_masked(HWIO_DMA_IF_EN_ADDR(x), m)
#define HWIO_DMA_IF_EN_OUT(x, v)      \
        out_dword(HWIO_DMA_IF_EN_ADDR(x),v)
#define HWIO_DMA_IF_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DMA_IF_EN_ADDR(x),m,v,HWIO_DMA_IF_EN_IN(x))
#define HWIO_DMA_IF_EN_DMA_IF_EN_BMSK                                                           0x1
#define HWIO_DMA_IF_EN_DMA_IF_EN_SHFT                                                           0x0

#define HWIO_FIFO_IF_DISABLE_ADDR(x)                                                     ((x) + 0x00000008)
#define HWIO_FIFO_IF_DISABLE_OFFS                                                        (0x00000008)
#define HWIO_FIFO_IF_DISABLE_RMSK                                                               0x1
#define HWIO_FIFO_IF_DISABLE_IN(x)      \
        in_dword_masked(HWIO_FIFO_IF_DISABLE_ADDR(x), HWIO_FIFO_IF_DISABLE_RMSK)
#define HWIO_FIFO_IF_DISABLE_INM(x, m)      \
        in_dword_masked(HWIO_FIFO_IF_DISABLE_ADDR(x), m)
#define HWIO_FIFO_IF_DISABLE_OUT(x, v)      \
        out_dword(HWIO_FIFO_IF_DISABLE_ADDR(x),v)
#define HWIO_FIFO_IF_DISABLE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_FIFO_IF_DISABLE_ADDR(x),m,v,HWIO_FIFO_IF_DISABLE_IN(x))
#define HWIO_FIFO_IF_DISABLE_FIFO_IF_DISABLE_BMSK                                               0x1
#define HWIO_FIFO_IF_DISABLE_FIFO_IF_DISABLE_SHFT                                               0x0

#define HWIO_GENI_FW_MULTILOCK_PROTNS_ADDR(x)                                            ((x) + 0x0000000c)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_OFFS                                               (0x0000000c)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_RMSK                                                      0x1
#define HWIO_GENI_FW_MULTILOCK_PROTNS_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_PROTNS_ADDR(x), HWIO_GENI_FW_MULTILOCK_PROTNS_RMSK)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_PROTNS_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_OUT(x, v)      \
        out_dword(HWIO_GENI_FW_MULTILOCK_PROTNS_ADDR(x),v)
#define HWIO_GENI_FW_MULTILOCK_PROTNS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_FW_MULTILOCK_PROTNS_ADDR(x),m,v,HWIO_GENI_FW_MULTILOCK_PROTNS_IN(x))
#define HWIO_GENI_FW_MULTILOCK_PROTNS_GENI_FW_MULTILOCK_PROTNS_BMSK                             0x1
#define HWIO_GENI_FW_MULTILOCK_PROTNS_GENI_FW_MULTILOCK_PROTNS_SHFT                             0x0

#define HWIO_GENI_FW_MULTILOCK_MSA_ADDR(x)                                               ((x) + 0x00000010)
#define HWIO_GENI_FW_MULTILOCK_MSA_OFFS                                                  (0x00000010)
#define HWIO_GENI_FW_MULTILOCK_MSA_RMSK                                                         0x1
#define HWIO_GENI_FW_MULTILOCK_MSA_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_MSA_ADDR(x), HWIO_GENI_FW_MULTILOCK_MSA_RMSK)
#define HWIO_GENI_FW_MULTILOCK_MSA_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_MSA_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_MSA_OUT(x, v)      \
        out_dword(HWIO_GENI_FW_MULTILOCK_MSA_ADDR(x),v)
#define HWIO_GENI_FW_MULTILOCK_MSA_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_FW_MULTILOCK_MSA_ADDR(x),m,v,HWIO_GENI_FW_MULTILOCK_MSA_IN(x))
#define HWIO_GENI_FW_MULTILOCK_MSA_GENI_FW_MULTILOCK_MSA_BMSK                                   0x1
#define HWIO_GENI_FW_MULTILOCK_MSA_GENI_FW_MULTILOCK_MSA_SHFT                                   0x0

#define HWIO_GENI_FW_MULTILOCK_SP_ADDR(x)                                                ((x) + 0x00000014)
#define HWIO_GENI_FW_MULTILOCK_SP_OFFS                                                   (0x00000014)
#define HWIO_GENI_FW_MULTILOCK_SP_RMSK                                                          0x1
#define HWIO_GENI_FW_MULTILOCK_SP_IN(x)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_SP_ADDR(x), HWIO_GENI_FW_MULTILOCK_SP_RMSK)
#define HWIO_GENI_FW_MULTILOCK_SP_INM(x, m)      \
        in_dword_masked(HWIO_GENI_FW_MULTILOCK_SP_ADDR(x), m)
#define HWIO_GENI_FW_MULTILOCK_SP_OUT(x, v)      \
        out_dword(HWIO_GENI_FW_MULTILOCK_SP_ADDR(x),v)
#define HWIO_GENI_FW_MULTILOCK_SP_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENI_FW_MULTILOCK_SP_ADDR(x),m,v,HWIO_GENI_FW_MULTILOCK_SP_IN(x))
#define HWIO_GENI_FW_MULTILOCK_SP_GENI_FW_MULTILOCK_SP_BMSK                                     0x1
#define HWIO_GENI_FW_MULTILOCK_SP_GENI_FW_MULTILOCK_SP_SHFT                                     0x0


#endif /* __UART_HWIO_H__ */
