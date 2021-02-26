#ifndef __HALTLMMSSCHWIO_H__
#define __HALTLMMSSCHWIO_H__
/*
===========================================================================
*/
/**
  @file HALtlmmSSCHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM830 (NapaliQ) [napaliq_v1.0_p2q1r81]
 
  This file contains HWIO register definitions for the following modules:
    SSC_LPASS_TLMM_CSR

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

  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/tlmm/hw/slpi/sdm670/HALtlmmSSCHWIO.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_LPASS_TLMM_CSR
 *--------------------------------------------------------------------------*/

#define SSC_LPASS_TLMM_CSR_REG_BASE                                 (LPASS_BASE      + 0x00b40000)
#define SSC_LPASS_TLMM_CSR_REG_BASE_OFFS                            0x00b40000

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00000000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_0_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00000000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00000004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_0_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00000004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00001000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_1_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00001000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00001004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_1_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00001004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_2_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00002000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_2_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00002004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_3_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00003000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_3_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00003004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00004000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_4_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00004000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00004004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_4_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00004004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00005000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_5_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00005000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00005004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_5_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00005004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00006000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_6_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00006000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00006004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_6_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00006004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00007000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_7_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00007000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00007004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_7_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00007004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00008000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_8_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00008000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00008004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_8_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00008004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_ADDR                          (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00009000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_9_OFFS                          (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00009000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_ADDR                       (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00009004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_9_OFFS                       (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00009004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000a000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_10_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000a000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000a004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_10_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000a004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000b000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_11_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000b000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000b004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_11_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000b004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000c000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_12_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000c000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000c004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_12_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000c004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000d000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_13_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000d000)
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

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000d004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_13_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000d004)
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

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000e000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000e000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_14_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_14_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_14_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_14_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_14_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_14_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_14_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000e004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000e004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_14_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000f000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000f000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_15_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_15_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_15_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_15_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_15_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_15_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_15_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0000f004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0000f004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_15_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00010000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00010000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_16_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_16_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_16_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_16_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_16_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_16_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_16_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00010004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00010004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_16_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00011000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00011000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_17_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_17_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_17_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_17_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_17_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_17_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_17_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00011004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00011004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_17_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00012000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00012000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_18_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_18_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_18_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_18_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_18_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_18_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_18_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00012004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00012004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_18_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00013000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00013000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_19_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_19_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_19_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_19_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_19_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_19_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_19_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00013004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00013004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_19_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00014000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00014000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_20_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_20_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_20_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_20_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_20_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_20_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_20_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00014004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00014004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_20_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00015000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00015000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_21_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_21_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_21_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_21_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_21_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_21_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_21_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00015004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00015004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_21_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00016000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00016000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_22_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_22_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_22_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_22_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_22_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_22_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_22_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00016004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00016004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_22_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00017000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00017000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_23_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_23_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_23_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_23_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_23_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_23_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_23_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00017004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00017004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_23_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00018000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00018000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_24_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_24_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_24_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_24_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_24_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_24_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_24_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00018004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00018004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_24_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00019000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00019000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_25_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_25_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_25_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_25_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_25_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_25_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_25_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x00019004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x00019004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_25_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001a000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001a000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_26_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_26_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_26_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_26_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_26_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_26_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_26_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001a004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001a004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_26_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001b000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001b000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_27_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_27_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_27_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_27_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_27_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_27_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_27_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001b004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001b004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_27_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001c000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001c000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_28_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_28_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_28_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_28_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_28_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_28_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_28_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001c004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001c004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_28_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001d000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001d000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_29_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_29_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_29_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_29_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_29_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_29_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_29_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001d004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001d004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_29_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001e000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001e000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_30_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_30_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_30_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_30_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_30_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_30_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_30_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001e004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001e004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_30_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_ADDR                         (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001f000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_OFFS                         (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001f000)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_RMSK                             0xffff
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_31_ADDR, HWIO_SSC_TLMM_LITE_GPIO_CFG_31_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_CFG_31_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_CFG_31_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_CFG_31_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_CFG_31_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_LITE_GPIO_CFG_31_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_ADDR                      (SSC_LPASS_TLMM_CSR_REG_BASE      + 0x0001f004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_OFFS                      (SSC_LPASS_TLMM_CSR_REG_BASE_OFFS + 0x0001f004)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_RMSK                             0x3
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_IN          \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_ADDR, HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_RMSK)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_ADDR, m)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_ADDR,v)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_ADDR,m,v,HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_IN)
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_LITE_GPIO_IN_OUT_31_GPIO_IN_SHFT                     0x0


#endif /* __HALTLMMSSCHWIO_H__ */
