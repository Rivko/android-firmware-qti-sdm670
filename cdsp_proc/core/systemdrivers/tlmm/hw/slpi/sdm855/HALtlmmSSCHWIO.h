#ifndef __HALTLMMSSCHWIO_H__
#define __HALTLMMSSCHWIO_H__
/*
===========================================================================
*/
/**
  @file HALtlmmSSCHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM855 (Hana) [hana_v1.0_p1q0r10]
 
  This file contains HWIO register definitions for the following modules:
    SSC_TLMM_CSR

  'Include' filters applied: 
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

  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/tlmm/hw/slpi/sdm855/HALtlmmSSCHWIO.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SSC_TLMM_CSR
 *--------------------------------------------------------------------------*/

#define SSC_TLMM_CSR_REG_BASE                                       (SSC_BASE      + 0x00b40000)
#define SSC_TLMM_CSR_REG_BASE_OFFS                                  0x00b40000

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00000000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_0_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_0_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00000004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00001000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00001000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_1_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_1_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00001004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00001004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00002000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_2_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_2_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00002004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00003000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_3_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_3_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00003004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00004000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00004000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_4_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_4_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00004004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00004004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00005000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00005000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_5_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_5_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00005004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00005004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00006000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00006000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_6_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_6_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00006004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00006004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00007000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00007000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_7_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_7_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00007004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00007004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00008000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00008000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_8_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_8_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00008004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00008004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00009000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00009000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_RMSK                              0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_9_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_9_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00009004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00009004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_RMSK                              0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000a000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000a000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_10_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_10_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000a004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000a004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000b000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000b000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_11_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_11_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000b004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000b004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000c000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000c000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_12_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_12_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000c004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000c004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000d000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000d000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_13_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_13_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000d004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000d004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_GPIO_IN_SHFT                     0x0


#endif /* __HALTLMMSSCHWIO_H__ */
