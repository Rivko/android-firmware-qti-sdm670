#ifndef __HALTLMMSSCHWIO_H__
#define __HALTLMMSSCHWIO_H__
/*
===========================================================================
*/
/**
  @file HALtlmmSSCHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r20.3.6]
 
  This file contains HWIO register definitions for the following modules:
    SSC_TLMM_CSR

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

  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/tlmm/hw/slpi/sdm845/HALtlmmSSCHWIO.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SSC_TLMM_CSR
 *--------------------------------------------------------------------------*/

#define SSC_TLMM_CSR_REG_BASE                                        (SSC_BASE      + 0x00b40000)
#define SSC_TLMM_CSR_REG_BASE_OFFS                                   0x00b40000

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00000000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG0_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00000004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT0_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00001000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00001000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG1_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00001004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00001004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT1_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00002000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG2_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00002004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT2_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00003000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG3_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00003004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT3_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00004000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00004000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG4_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00004004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00004004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT4_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00005000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00005000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG5_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00005004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00005004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT5_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00006000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00006000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG6_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00006004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00006004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT6_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00007000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00007000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG7_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00007004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00007004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT7_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00008000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00008000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG8_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00008004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00008004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT8_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_ADDR                          (SSC_TLMM_CSR_REG_BASE      + 0x00009000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_OFFS                          (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00009000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_RMSK                              0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_MISC_CFG_BMSK                0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_MISC_CFG_SHFT                   0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_HIHYS_CTL_BMSK                0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_HIHYS_CTL_SHFT                  0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_OE_BMSK                       0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_OE_SHFT                         0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_HDRIVE_BMSK                   0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_HDRIVE_SHFT                     0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_FUNC_SEL_BMSK                  0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_FUNC_SEL_SHFT                   0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_PULL_BMSK                       0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG9_GPIO_PULL_SHFT                       0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_ADDR                       (SSC_TLMM_CSR_REG_BASE      + 0x00009004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_OFFS                       (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00009004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_RMSK                              0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_GPIO_OUT_BMSK                     0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_GPIO_OUT_SHFT                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_GPIO_IN_BMSK                      0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT9_GPIO_IN_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000a000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000a000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG10_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000a004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000a004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT10_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000b000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000b000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG11_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000b004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000b004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT11_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000c000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000c000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG12_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000c004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000c004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT12_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000d000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000d000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG13_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000d004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000d004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT13_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000e000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000e000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG14_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000e004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000e004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT14_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x0000f000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000f000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG15_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x0000f004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x0000f004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT15_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x00010000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00010000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG16_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x00010004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00010004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT16_GPIO_IN_SHFT                     0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_ADDR                         (SSC_TLMM_CSR_REG_BASE      + 0x00011000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_OFFS                         (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00011000)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_RMSK                             0xffff
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_MISC_CFG_BMSK               0xf800
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_MISC_CFG_SHFT                  0xb
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_HIHYS_CTL_BMSK               0x400
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_HIHYS_CTL_SHFT                 0xa
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_OE_BMSK                      0x200
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_OE_SHFT                        0x9
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_HDRIVE_BMSK                  0x1c0
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_HDRIVE_SHFT                    0x6
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_FUNC_SEL_BMSK                 0x3c
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_FUNC_SEL_SHFT                  0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_PULL_BMSK                      0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_CFG17_GPIO_PULL_SHFT                      0x0

#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_ADDR                      (SSC_TLMM_CSR_REG_BASE      + 0x00011004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_OFFS                      (SSC_TLMM_CSR_REG_BASE_OFFS + 0x00011004)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_RMSK                             0x3
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_IN          \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_ADDR, HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_RMSK)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_INM(m)      \
        in_dword_masked(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_ADDR, m)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_OUT(v)      \
        out_dword(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_ADDR,v)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_ADDR,m,v,HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_IN)
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_GPIO_OUT_BMSK                    0x2
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_GPIO_OUT_SHFT                    0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_GPIO_IN_BMSK                     0x1
#define HWIO_SSC_TLMM_GPIO_ISLAND_IN_OUT17_GPIO_IN_SHFT                     0x0


#endif /* __HALTLMMSSCHWIO_H__ */
