#ifndef __ABTIMEOUT_HWIO_CDSP_H__
#define __ABTIMEOUT_HWIO_CDSP_H__
/*
===========================================================================
*/
/**
  @file ABTimeout_HWIO_CDSP.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TURING_AHBE_TIME
    TCSR_TCSR_REGS

  'Include' filters applied: TIMEOUT_SLAVE_GLB_EN[TCSR_TCSR_REGS] TCSR_TIMEOUT_INTR_STATUS[TCSR_TCSR_REGS] TCSR_TIMEOUT_INTR_TURING_ENABLE[TCSR_TCSR_REGS] BASE[TURING_AHBE_TIME] 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/cdsp/sdm845/ABTimeout_HWIO_CDSP.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"


/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                      (CORE_TOP_CSR_BASE      + 0x000c0000)

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00000800)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK                                                                                 0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, m)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,v)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,m,v,HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_BMSK                                                            0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_SHFT                                                            0x0

#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00008020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK                                                                           0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_31_TIMEOUT_SLAVE_INTR_BMSK                                             0x80000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_31_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1f
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_30_TIMEOUT_SLAVE_INTR_BMSK                                             0x40000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_30_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1e
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_29_TIMEOUT_SLAVE_INTR_BMSK                                             0x20000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_29_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1d
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_28_TIMEOUT_SLAVE_INTR_BMSK                                             0x10000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_28_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1c
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_27_TIMEOUT_SLAVE_INTR_BMSK                                              0x8000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_27_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1b
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_26_TIMEOUT_SLAVE_INTR_BMSK                                              0x4000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_26_TIMEOUT_SLAVE_INTR_SHFT                                                   0x1a
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_25_TIMEOUT_SLAVE_INTR_BMSK                                              0x2000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_25_TIMEOUT_SLAVE_INTR_SHFT                                                   0x19
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_ECAHB_TSLV_INTR_BMSK                                                       0x1000000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_ECAHB_TSLV_INTR_SHFT                                                            0x18
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT2_BMSK                                                  0x800000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT2_SHFT                                                      0x17
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT1_BMSK                                                  0x400000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT1_SHFT                                                      0x16
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT0_BMSK                                                  0x200000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT0_SHFT                                                      0x15
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_WAHB_TSLV_INTR_BMSK                                                    0x100000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_WAHB_TSLV_INTR_SHFT                                                        0x14
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_19_TIMEOUT_SLAVE_INTR_BMSK                                                0x80000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_19_TIMEOUT_SLAVE_INTR_SHFT                                                   0x13
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_TURING_IRQ_OUT_AHB_TIMEOUT0_BMSK                                                  0x40000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_TURING_IRQ_OUT_AHB_TIMEOUT0_SHFT                                                     0x12
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_17_TIMEOUT_SLAVE_INTR_BMSK                                                0x20000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_17_TIMEOUT_SLAVE_INTR_SHFT                                                   0x11
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_16_TIMEOUT_SLAVE_INTR_BMSK                                                0x10000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_16_TIMEOUT_SLAVE_INTR_SHFT                                                   0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_15_TIMEOUT_SLAVE_INTR_BMSK                                                 0x8000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_15_TIMEOUT_SLAVE_INTR_SHFT                                                    0xf
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_14_TIMEOUT_SLAVE_INTR_BMSK                                                 0x4000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_14_TIMEOUT_SLAVE_INTR_SHFT                                                    0xe
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_13_TIMEOUT_SLAVE_INTR_BMSK                                                 0x2000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_13_TIMEOUT_SLAVE_INTR_SHFT                                                    0xd
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_12_TIMEOUT_SLAVE_INTR_BMSK                                                 0x1000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_12_TIMEOUT_SLAVE_INTR_SHFT                                                    0xc
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_WAPB_TSLV_IRQ_BMSK                                                        0x800
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_WAPB_TSLV_IRQ_SHFT                                                          0xb
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_BMSK                                                   0x400
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_SHFT                                                     0xa
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_9_TIMEOUT_SLAVE_INTR_BMSK                                                   0x200
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_9_TIMEOUT_SLAVE_INTR_SHFT                                                     0x9
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_8_TIMEOUT_SLAVE_INTR_BMSK                                                   0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_8_TIMEOUT_SLAVE_INTR_SHFT                                                     0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_7_TIMEOUT_SLAVE_INTR_BMSK                                                    0x80
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_7_TIMEOUT_SLAVE_INTR_SHFT                                                     0x7
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_NOC_INTR_BMSK                                                              0x40
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_WCSS_TCSR_NOC_INTR_SHFT                                                               0x6
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_5_TIMEOUT_SLAVE_INTR_BMSK                                                    0x20
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_5_TIMEOUT_SLAVE_INTR_SHFT                                                     0x5
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_4_TIMEOUT_SLAVE_INTR_BMSK                                                    0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_4_TIMEOUT_SLAVE_INTR_SHFT                                                     0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_3_TIMEOUT_SLAVE_INTR_BMSK                                                     0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_3_TIMEOUT_SLAVE_INTR_SHFT                                                     0x3
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_2_TIMEOUT_SLAVE_INTR_BMSK                                                     0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_2_TIMEOUT_SLAVE_INTR_SHFT                                                     0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_1_TIMEOUT_SLAVE_INTR_BMSK                                                     0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_1_TIMEOUT_SLAVE_INTR_SHFT                                                     0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_0_TIMEOUT_SLAVE_INTR_BMSK                                                     0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RESERVE_0_TIMEOUT_SLAVE_INTR_SHFT                                                     0x0

#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x0002b000)
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RMSK                                                                    0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_31_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                               0x80000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_31_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1f
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_30_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                               0x40000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_30_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1e
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_29_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                               0x20000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_29_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1d
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_28_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                               0x10000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_28_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1c
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_27_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                0x8000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_27_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1b
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_26_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                0x4000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_26_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x1a
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_25_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                0x2000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_25_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x19
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_ECAHB_TSLV_INTR_ENABLE_BMSK                                         0x1000000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_ECAHB_TSLV_INTR_ENABLE_SHFT                                              0x18
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT2_ENABLE_BMSK                                    0x800000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT2_ENABLE_SHFT                                        0x17
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT1_ENABLE_BMSK                                    0x400000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT1_ENABLE_SHFT                                        0x16
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT0_ENABLE_BMSK                                    0x200000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_LPASS_IRQ_OUT_AHB_TIMEOUT0_ENABLE_SHFT                                        0x15
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_WAHB_TSLV_INTR_ENABLE_BMSK                                      0x100000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_WAHB_TSLV_INTR_ENABLE_SHFT                                          0x14
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_19_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                  0x80000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_19_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x13
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_TURING_IRQ_OUT_AHB_TIMEOUT0_ENABLE_BMSK                                    0x40000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_TURING_IRQ_OUT_AHB_TIMEOUT0_ENABLE_SHFT                                       0x12
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_17_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                  0x20000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_17_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x11
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_16_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                  0x10000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_16_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                     0x10
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_15_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                   0x8000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_15_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                      0xf
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_14_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                   0x4000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_14_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                      0xe
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_13_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                   0x2000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_13_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                      0xd
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_12_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                   0x1000
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_12_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                      0xc
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_WAPB_TSLV_IRQ_ENABLE_BMSK                                          0x800
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_WAPB_TSLV_IRQ_ENABLE_SHFT                                            0xb
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_ENABLE_BMSK                                     0x400
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_ENABLE_SHFT                                       0xa
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_9_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                     0x200
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_9_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x9
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_8_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                     0x100
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_8_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x8
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_7_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                      0x80
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_7_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x7
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_NOC_INTR_ENABLE_BMSK                                                0x40
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_WCSS_TCSR_NOC_INTR_ENABLE_SHFT                                                 0x6
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_5_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                      0x20
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_5_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x5
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_4_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                      0x10
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_4_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x4
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_3_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                       0x8
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_3_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x3
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_2_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                       0x4
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_2_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x2
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_1_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                       0x2
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_1_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x1
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_0_TIMEOUT_SLAVE_INTR_ENABLE_BMSK                                       0x1
#define HWIO_TCSR_TIMEOUT_INTR_TURING_ENABLE_RESERVE_0_TIMEOUT_SLAVE_INTR_ENABLE_SHFT                                       0x0

/*----------------------------------------------------------------------------
 * MODULE: TURING_AHBE_TIME
 *--------------------------------------------------------------------------*/

#define TURING_AHBE_TIME_REG_BASE                                 (TURING_BASE      + 0x000b7000)


#endif /* __ABTIMEOUT_HWIO_CDSP_H__ */
