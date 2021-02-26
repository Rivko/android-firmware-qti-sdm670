#ifndef __CPR_MPSS_HWIO__
#define __CPR_MPSS_HWIO__
/*
===========================================================================
*/
/**
  @file cpr_mpss_hwio
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    MPSS_PERPH

  'Exclude' filters applied: RESERVED DUMMY[MPSS_PERPH] 

  Generation parameters: 
  { u'filename': u'cpr_mpss_hwio',
    u'module-filter-exclude': { u'MPSS_PERPH': [u'RESERVED DUMMY']},
    u'module-filter-include': { },
    u'modules': [u'MPSS_PERPH']}
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

  $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/target/670/cpr_mpss_hwio.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MPSS_PERPH
 *--------------------------------------------------------------------------*/

#define MPSS_PERPH_REG_BASE                                                         (MODEM_TOP_BASE      + 0x001a8000)

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                                0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                             0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                             0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                                     (MPSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                            0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                                     0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                               0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                           0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                           0x0

#define HWIO_MSS_MSA_CRYPTO_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_MSA_CRYPTO_RMSK                                                           0xf
#define HWIO_MSS_MSA_CRYPTO_IN          \
        in_dword_masked(HWIO_MSS_MSA_CRYPTO_ADDR, HWIO_MSS_MSA_CRYPTO_RMSK)
#define HWIO_MSS_MSA_CRYPTO_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_CRYPTO_ADDR, m)
#define HWIO_MSS_MSA_CRYPTO_OUT(v)      \
        out_dword(HWIO_MSS_MSA_CRYPTO_ADDR,v)
#define HWIO_MSS_MSA_CRYPTO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_CRYPTO_ADDR,m,v,HWIO_MSS_MSA_CRYPTO_IN)
#define HWIO_MSS_MSA_CRYPTO_MSA_CRYPTO_BMSK                                                0xf
#define HWIO_MSS_MSA_CRYPTO_MSA_CRYPTO_SHFT                                                0x0

#define HWIO_MSS_MSA_ADDR                                                           (MPSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_MSA_RMSK                                                                  0xf
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_CONFIG_UNLOCK2_BMSK                                                   0x8
#define HWIO_MSS_MSA_CONFIG_UNLOCK2_SHFT                                                   0x3
#define HWIO_MSS_MSA_FORCE_Q6_MSA_BMSK                                                     0x4
#define HWIO_MSS_MSA_FORCE_Q6_MSA_SHFT                                                     0x2
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                           0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                           0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                                      0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                                      0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                               0xff8007ff
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_BMSK                                 0xe0000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_SHFT                                       0x1d
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_BMSK                                0x1c000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_SHFT                                      0x1a
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_BMSK                             0x3800000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_SHFT                                  0x17
#define HWIO_MSS_DEBUG_CLOCK_CTL_MODEM_MUX_SEL_BMSK                                      0x7c0
#define HWIO_MSS_DEBUG_CLOCK_CTL_MODEM_MUX_SEL_SHFT                                        0x6
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_BMSK                                  0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_SHFT                                   0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                                  0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                                   0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                                   0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                                   0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                                   0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                                   0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                                   0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                                   0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                                   0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                                   0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                                     (MPSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                            0xf
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_SMMU_BYPASS_CHECKER_DISABLE_BMSK                                0x8
#define HWIO_MSS_DEBUG_CTL_SMMU_BYPASS_CHECKER_DISABLE_SHFT                                0x3
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                                      0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                                      0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                                        0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                                        0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                                       0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                                       0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                                   (MPSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                                      0x7ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_FABRIC_BRIDGES_SEL_BMSK                                   0x70000
#define HWIO_MSS_DBG_BUS_CTL_FABRIC_BRIDGES_SEL_SHFT                                      0x10
#define HWIO_MSS_DBG_BUS_CTL_AXI2AXI_DMA_SEL_BMSK                                       0xe000
#define HWIO_MSS_DBG_BUS_CTL_AXI2AXI_DMA_SEL_SHFT                                          0xd
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                                       0x1c00
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                          0xa
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_1X1_CNOC_SEL_BMSK                                   0x300
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_1X1_CNOC_SEL_SHFT                                     0x8
#define HWIO_MSS_DBG_BUS_CTL_MODEM_DBG_BUS_SEL_BMSK                                       0xf0
#define HWIO_MSS_DBG_BUS_CTL_MODEM_DBG_BUS_SEL_SHFT                                        0x4
#define HWIO_MSS_DBG_BUS_CTL_MPSS_DBG_BUS_SEL_BMSK                                         0xf
#define HWIO_MSS_DBG_BUS_CTL_MPSS_DBG_BUS_SEL_SHFT                                         0x0

#define HWIO_MSS_NAV_AXI_HALTREQ_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_NAV_AXI_HALTREQ_RMSK                                                      0x7
#define HWIO_MSS_NAV_AXI_HALTREQ_IN          \
        in_dword_masked(HWIO_MSS_NAV_AXI_HALTREQ_ADDR, HWIO_MSS_NAV_AXI_HALTREQ_RMSK)
#define HWIO_MSS_NAV_AXI_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_AXI_HALTREQ_ADDR, m)
#define HWIO_MSS_NAV_AXI_HALTREQ_OUT(v)      \
        out_dword(HWIO_MSS_NAV_AXI_HALTREQ_ADDR,v)
#define HWIO_MSS_NAV_AXI_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_AXI_HALTREQ_ADDR,m,v,HWIO_MSS_NAV_AXI_HALTREQ_IN)
#define HWIO_MSS_NAV_AXI_HALTREQ_IDLE_BMSK                                                 0x4
#define HWIO_MSS_NAV_AXI_HALTREQ_IDLE_SHFT                                                 0x2
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTACK_BMSK                                              0x2
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTACK_SHFT                                              0x1
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTREQ_BMSK                                              0x1
#define HWIO_MSS_NAV_AXI_HALTREQ_HALTREQ_SHFT                                              0x0

#define HWIO_MSS_BIMC_DANGER_ADDR                                                   (MPSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_BIMC_DANGER_RMSK                                                          0x7
#define HWIO_MSS_BIMC_DANGER_IN          \
        in_dword_masked(HWIO_MSS_BIMC_DANGER_ADDR, HWIO_MSS_BIMC_DANGER_RMSK)
#define HWIO_MSS_BIMC_DANGER_INM(m)      \
        in_dword_masked(HWIO_MSS_BIMC_DANGER_ADDR, m)
#define HWIO_MSS_BIMC_DANGER_OUT(v)      \
        out_dword(HWIO_MSS_BIMC_DANGER_ADDR,v)
#define HWIO_MSS_BIMC_DANGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIMC_DANGER_ADDR,m,v,HWIO_MSS_BIMC_DANGER_IN)
#define HWIO_MSS_BIMC_DANGER_MSS_SAVE_BMSK                                                 0x4
#define HWIO_MSS_BIMC_DANGER_MSS_SAVE_SHFT                                                 0x2
#define HWIO_MSS_BIMC_DANGER_MSS_DANGER_BMSK                                               0x3
#define HWIO_MSS_BIMC_DANGER_MSS_DANGER_SHFT                                               0x0

#define HWIO_MSS_HW_VERSION_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_HW_VERSION_RMSK                                                    0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                              0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                                    0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                               0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                                    0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                                   0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                                      0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000400)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000404)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000408)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000040c)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW4_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000410)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, HWIO_MSS_RELAY_MSG_SHADOW4_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW4_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW4_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW5_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000414)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, HWIO_MSS_RELAY_MSG_SHADOW5_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW5_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW5_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW6_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000418)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, HWIO_MSS_RELAY_MSG_SHADOW6_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW6_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW6_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW7_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000041c)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, HWIO_MSS_RELAY_MSG_SHADOW7_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW7_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW7_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW8_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000420)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, HWIO_MSS_RELAY_MSG_SHADOW8_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW8_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW8_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW9_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00000424)
#define HWIO_MSS_RELAY_MSG_SHADOW9_RMSK                                             0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW9_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR, HWIO_MSS_RELAY_MSG_SHADOW9_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW9_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW9_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW9_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW9_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW9_RELAY_MSG_SHADOW9_BMSK                           0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW9_RELAY_MSG_SHADOW9_SHFT                                  0x0

#define HWIO_MSS_RELAY_MSG_SHADOW10_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00000428)
#define HWIO_MSS_RELAY_MSG_SHADOW10_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW10_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR, HWIO_MSS_RELAY_MSG_SHADOW10_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW10_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW10_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW10_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW10_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW10_RELAY_MSG_SHADOW10_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW10_RELAY_MSG_SHADOW10_SHFT                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW11_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x0000042c)
#define HWIO_MSS_RELAY_MSG_SHADOW11_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW11_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR, HWIO_MSS_RELAY_MSG_SHADOW11_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW11_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW11_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW11_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW11_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW11_RELAY_MSG_SHADOW11_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW11_RELAY_MSG_SHADOW11_SHFT                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW12_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00000430)
#define HWIO_MSS_RELAY_MSG_SHADOW12_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW12_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR, HWIO_MSS_RELAY_MSG_SHADOW12_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW12_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW12_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW12_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW12_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW12_RELAY_MSG_SHADOW12_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW12_RELAY_MSG_SHADOW12_SHFT                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW13_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00000434)
#define HWIO_MSS_RELAY_MSG_SHADOW13_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW13_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR, HWIO_MSS_RELAY_MSG_SHADOW13_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW13_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW13_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW13_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW13_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW13_RELAY_MSG_SHADOW13_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW13_RELAY_MSG_SHADOW13_SHFT                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW14_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00000438)
#define HWIO_MSS_RELAY_MSG_SHADOW14_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW14_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR, HWIO_MSS_RELAY_MSG_SHADOW14_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW14_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW14_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW14_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW14_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW14_RELAY_MSG_SHADOW14_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW14_RELAY_MSG_SHADOW14_SHFT                                0x0

#define HWIO_MSS_RELAY_MSG_SHADOW15_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x0000043c)
#define HWIO_MSS_RELAY_MSG_SHADOW15_RMSK                                            0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW15_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR, HWIO_MSS_RELAY_MSG_SHADOW15_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW15_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW15_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW15_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW15_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW15_RELAY_MSG_SHADOW15_BMSK                         0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW15_RELAY_MSG_SHADOW15_SHFT                                0x0

#define HWIO_MSS_OFFLINE_PLL_MODE_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00001000)
#define HWIO_MSS_OFFLINE_PLL_MODE_RMSK                                              0xffffffff
#define HWIO_MSS_OFFLINE_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, HWIO_MSS_OFFLINE_PLL_MODE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_MODE_IN)
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_BMSK                                 0x80000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_SHFT                                       0x1f
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                              0x40000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                    0x1e
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_BMSK                                0x20000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_SHFT                                      0x1d
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                            0x10000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                  0x1c
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BITS27_24_BMSK                             0xf000000
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BITS27_24_SHFT                                  0x18
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                     0x800000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                         0x17
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_BMSK                                     0x400000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_SHFT                                         0x16
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                             0x200000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                 0x15
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                               0x100000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                   0x14
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_BMSK                                  0xfc000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_SHFT                                      0xe
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_BMSK                                   0x3f00
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_SHFT                                      0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BITS7_3_BMSK                                    0xf8
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BITS7_3_SHFT                                     0x3
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_BMSK                                         0x4
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_SHFT                                         0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT1_BMSK                                        0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_RESERVE_BIT1_SHFT                                        0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_BMSK                                         0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_SHFT                                         0x0

#define HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00001004)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK                                                 0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_L_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_VAL_BMSK                                       0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_VAL_SHFT                                          0x0

#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x00001008)
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_RMSK                                             0xffff
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_PLL_CAL_L_VAL_BMSK                               0xffff
#define HWIO_MSS_OFFLINE_PLL_CAL_L_VAL_PLL_CAL_L_VAL_SHFT                                  0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x0000100c)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK                                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                        0xfff80000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                              0x13
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                               0x70000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                  0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                           0xf000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                              0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                           0xf00
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                             0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                               0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                0x60
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                 0x5
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_TEST_BMSK                                    0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_TEST_SHFT                                     0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS3_BMSK                                   0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS3_SHFT                                   0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_ODD_BMSK                                      0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_ODD_SHFT                                      0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                     0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                     0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                     0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                     0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00001010)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK                                        0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_19_BMSK                      0xfff80000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_19_SHFT                            0x13
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                       0x40000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                          0x12
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                       0x20000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                          0x11
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                        0x10000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                           0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                            0x8000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                               0xf
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                         0x4000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                            0xe
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                             0x2000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                0xd
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                      0x1800
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                         0xb
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                      0x400
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                        0xa
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                             0x3e0
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                               0x5
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                               0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                    0xe
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                    0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                        0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                        0x0

#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00001014)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                     0xfc000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                           0x1a
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                    0x3c00000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                         0x16
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK            0x3c0000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                0x12
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK             0x3c000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                 0xe
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                    0x3800
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                       0xb
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                   0x700
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                     0x8
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                         0xf0
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                          0x4
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                  0xf
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                  0x0

#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_ADDR                                      (MPSS_PERPH_REG_BASE      + 0x00001018)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_RMSK                                      0xffffffff
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                             0xfff00000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                   0x14
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_RESERVE_BITS19_12_BMSK                       0xff000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_RESERVE_BITS19_12_SHFT                           0xc
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                       0xc00
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                         0xa
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                           0x380
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                             0x7
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                0x40
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                 0x6
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                       0x30
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                        0x4
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                       0x8
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                       0x3
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                            0x6
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                            0x1
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                     0x1
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                     0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x0000101c)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK                                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                  0xc0000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                        0x1e
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK              0x30000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                    0x1c
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                     0x8000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                          0x1b
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_BMSK                                  0x7e00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_SHFT                                       0x15
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                          0x100000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                              0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_BMSK                                  0xff000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_SHFT                                      0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                           0x800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                             0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                            0x700
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                              0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                            0x80
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                             0x7
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                   0x40
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                    0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_BMSK                                     0x30
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_SHFT                                      0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_BMSK                                      0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_SHFT                                      0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_BMSK                                       0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_SHFT                                       0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_BMSK                                       0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_SHFT                                       0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK                                        0xffffffff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                    0x80000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                          0x1f
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                       0x40000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                             0x1e
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                        0x20000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                              0x1d
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                       0x10000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                             0x1c
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                  0x8000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                       0x1b
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK               0x6000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                    0x19
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                    0x1c00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                         0x16
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                     0x200000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                         0x15
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                            0x100000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                       0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                          0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                   0x40000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                      0x12
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                               0x30000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                  0x10
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                0x8000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                   0xf
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DIS_LEAK_CMP_BMSK                               0x4000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DIS_LEAK_CMP_SHFT                                  0xe
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                             0x2000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                           0x1000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                              0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                 0xc00
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                   0xa
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                   0x380
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                     0x7
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                 0x40
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                  0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                 0x20
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                  0x5
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                             0x18
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                              0x3
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                 0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                 0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                0x0

#define HWIO_MSS_OFFLINE_PLL_STATUS_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_OFFLINE_PLL_STATUS_RMSK                                            0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, HWIO_MSS_OFFLINE_PLL_STATUS_RMSK)
#define HWIO_MSS_OFFLINE_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_BMSK                                0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_SHFT                                       0x0

#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK                                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_BMSK                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_SHFT                           0x0

#define HWIO_MSS_OFFLINE_PLL_OPMODE_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_OFFLINE_PLL_OPMODE_RMSK                                                   0x7
#define HWIO_MSS_OFFLINE_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_OPMODE_ADDR, HWIO_MSS_OFFLINE_PLL_OPMODE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_OPMODE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_OPMODE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_OPMODE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_OPMODE_IN)
#define HWIO_MSS_OFFLINE_PLL_OPMODE_PLL_OPMODE_BMSK                                        0x7
#define HWIO_MSS_OFFLINE_PLL_OPMODE_PLL_OPMODE_SHFT                                        0x0

#define HWIO_MSS_OFFLINE_PLL_STATE_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_OFFLINE_PLL_STATE_RMSK                                                    0x7
#define HWIO_MSS_OFFLINE_PLL_STATE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATE_ADDR, HWIO_MSS_OFFLINE_PLL_STATE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_STATE_PLL_STATE_BMSK                                          0x7
#define HWIO_MSS_OFFLINE_PLL_STATE_PLL_STATE_SHFT                                          0x0

#define HWIO_MSS_OFFLINE_PLL_DROOP_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_OFFLINE_PLL_DROOP_RMSK                                                   0xff
#define HWIO_MSS_OFFLINE_PLL_DROOP_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_DROOP_ADDR, HWIO_MSS_OFFLINE_PLL_DROOP_RMSK)
#define HWIO_MSS_OFFLINE_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_DROOP_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_DROOP_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_DROOP_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_DROOP_IN)
#define HWIO_MSS_OFFLINE_PLL_DROOP_DROOP_CODE_BMSK                                        0xfe
#define HWIO_MSS_OFFLINE_PLL_DROOP_DROOP_CODE_SHFT                                         0x1
#define HWIO_MSS_OFFLINE_PLL_DROOP_DROOP_EN_BMSK                                           0x1
#define HWIO_MSS_OFFLINE_PLL_DROOP_DROOP_EN_SHFT                                           0x0

#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_RMSK                                              0xffff
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FRAC_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_FRAC_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FRAC_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_FRAC_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_FRAC_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_FRAC_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                 0xffff
#define HWIO_MSS_OFFLINE_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                    0x0

#define HWIO_MSS_OFFLINE_PLL_SPARE_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_OFFLINE_PLL_SPARE_RMSK                                                   0xff
#define HWIO_MSS_OFFLINE_PLL_SPARE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_SPARE_ADDR, HWIO_MSS_OFFLINE_PLL_SPARE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_SPARE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_SPARE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_SPARE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_SPARE_IN)
#define HWIO_MSS_OFFLINE_PLL_SPARE_SPARE_OUTPUTS_BMSK                                     0xf0
#define HWIO_MSS_OFFLINE_PLL_SPARE_SPARE_OUTPUTS_SHFT                                      0x4
#define HWIO_MSS_OFFLINE_PLL_SPARE_SPARE_INPUTS_BMSK                                       0xf
#define HWIO_MSS_OFFLINE_PLL_SPARE_SPARE_INPUTS_SHFT                                       0x0

#define HWIO_MSS_CLAMP_MEM_ADDR                                                     (MPSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                            0xf
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                                0x8
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                                0x3
#define HWIO_MSS_CLAMP_MEM_SPARE_2_TO_1_BMSK                                               0x6
#define HWIO_MSS_CLAMP_MEM_SPARE_2_TO_1_SHFT                                               0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_BMSK                                                  0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_SHFT                                                  0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                                      (MPSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_CLAMP_IO_RMSK                                                            0x3b
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_SPARE_5_TO_4_BMSK                                               0x30
#define HWIO_MSS_CLAMP_IO_SPARE_5_TO_4_SHFT                                                0x4
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                                 0x8
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                                 0x3
#define HWIO_MSS_CLAMP_IO_QLINK_PHY_BMSK                                                   0x2
#define HWIO_MSS_CLAMP_IO_QLINK_PHY_SHFT                                                   0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                                       0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                                       0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK                                          0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS4_DMEM_NRET_N_BMSK                     0x80000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS4_DMEM_NRET_N_SHFT                           0x1f
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS3_DMEM_NRET_N_BMSK                     0x40000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS3_DMEM_NRET_N_SHFT                           0x1e
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC2_DMEM_NRET_N_BMSK                      0x20000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC2_DMEM_NRET_N_SHFT                            0x1d
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH2_DMEM_NRET_N_BMSK                      0x10000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH2_DMEM_NRET_N_SHFT                            0x1c
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD2_DMEM_NRET_N_BMSK                     0x8000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD2_DMEM_NRET_N_SHFT                          0x1b
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS2_DMEM_NRET_N_BMSK                      0x4000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS2_DMEM_NRET_N_SHFT                           0x1a
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS1_DMEM_NRET_N_BMSK                      0x2000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS1_DMEM_NRET_N_SHFT                           0x19
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC1_DMEM_NRET_N_BMSK                       0x1000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC1_DMEM_NRET_N_SHFT                            0x18
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH1_DMEM_NRET_N_BMSK                        0x800000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH1_DMEM_NRET_N_SHFT                            0x17
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD1_DMEM_NRET_N_BMSK                      0x400000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD1_DMEM_NRET_N_SHFT                          0x16
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENG_DMEM_NRET_N_BMSK                 0x200000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENG_DMEM_NRET_N_SHFT                     0x15
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENC_DMEM_NRET_N_BMSK                 0x100000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENC_DMEM_NRET_N_SHFT                     0x14
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_VUSS_DMEM_NRET_N_BMSK                 0x80000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_VUSS_DMEM_NRET_N_SHFT                    0x13
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_MCDMA_DMEM_NRET_N_BMSK                        0x40000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_MCDMA_DMEM_NRET_N_SHFT                           0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_QLBR_DMEM_NRET_N_BMSK                         0x20000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_QLBR_DMEM_NRET_N_SHFT                            0x11
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SPARE_DMEM_NRET_N_BMSK                        0x10000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SPARE_DMEM_NRET_N_SHFT                           0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS4_DMEM_RET_N_BMSK                          0x8000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS4_DMEM_RET_N_SHFT                             0xf
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS3_DMEM_RET_N_BMSK                          0x4000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS3_DMEM_RET_N_SHFT                             0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC2_DMEM_RET_N_BMSK                           0x2000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC2_DMEM_RET_N_SHFT                              0xd
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH2_DMEM_RET_N_BMSK                           0x1000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH2_DMEM_RET_N_SHFT                              0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD2_DMEM_RET_N_BMSK                          0x800
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD2_DMEM_RET_N_SHFT                            0xb
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS2_DMEM_RET_N_BMSK                           0x400
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS2_DMEM_RET_N_SHFT                             0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS1_DMEM_RET_N_BMSK                           0x200
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_VUSS1_DMEM_RET_N_SHFT                             0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC1_DMEM_RET_N_BMSK                            0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEC1_DMEM_RET_N_SHFT                              0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH1_DMEM_RET_N_BMSK                             0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_CCH1_DMEM_RET_N_SHFT                              0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD1_DMEM_RET_N_BMSK                           0x40
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_DEMOD1_DMEM_RET_N_SHFT                            0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENG_DMEM_RET_N_BMSK                      0x20
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENG_DMEM_RET_N_SHFT                       0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENC_DMEM_RET_N_BMSK                      0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_ENC_DMEM_RET_N_SHFT                       0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_VUSS_DMEM_RET_N_BMSK                      0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SYMPROC_VUSS_DMEM_RET_N_SHFT                      0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_MCDMA_DMEM_RET_N_BMSK                             0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_MCDMA_DMEM_RET_N_SHFT                             0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_QLBR_DMEM_RET_N_BMSK                              0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_QLBR_DMEM_RET_N_SHFT                              0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SPARE_DMEM_RET_N_BMSK                             0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IU_SPARE_DMEM_RET_N_SHFT                             0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK                                         0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_29_BMSK                             0xe0000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_29_SHFT                                   0x1d
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_1_SLP_NRET_N_BMSK            0x10000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_1_SLP_NRET_N_SHFT                  0x1c
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_0_SLP_NRET_N_BMSK             0x8000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_0_SLP_NRET_N_SHFT                  0x1b
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_1_SLP_NRET_N_BMSK            0x4000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_1_SLP_NRET_N_SHFT                 0x1a
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_0_SLP_NRET_N_BMSK            0x2000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_0_SLP_NRET_N_SHFT                 0x19
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_1_SLP_NRET_N_BMSK             0x1000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_1_SLP_NRET_N_SHFT                  0x18
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_0_SLP_NRET_N_BMSK              0x800000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_0_SLP_NRET_N_SHFT                  0x17
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_1_SLP_NRET_N_BMSK             0x400000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_1_SLP_NRET_N_SHFT                 0x16
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_0_SLP_NRET_N_BMSK             0x200000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_0_SLP_NRET_N_SHFT                 0x15
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_1_SLP_NRET_N_BMSK              0x100000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_1_SLP_NRET_N_SHFT                  0x14
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_0_SLP_NRET_N_BMSK               0x80000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_0_SLP_NRET_N_SHFT                  0x13
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_SLP_RET_N_BMSK                  0x40000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_D_SLP_RET_N_SHFT                     0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_SLP_RET_N_BMSK                 0x20000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B1_SLP_RET_N_SHFT                    0x11
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_SLP_RET_N_BMSK                  0x10000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_C_SLP_RET_N_SHFT                     0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_SLP_RET_N_BMSK                  0x8000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_B0_SLP_RET_N_SHFT                     0xf
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_SLP_RET_N_BMSK                   0x4000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CLUSTER_SLICE_A_SLP_RET_N_SHFT                      0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_D_SLP_NRET_N_BMSK                     0x2000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_D_SLP_NRET_N_SHFT                        0xd
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B1_SLP_NRET_N_BMSK                    0x1000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B1_SLP_NRET_N_SHFT                       0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_C_SLP_NRET_N_BMSK                      0x800
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_C_SLP_NRET_N_SHFT                        0xb
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B0_SLP_NRET_N_BMSK                     0x400
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B0_SLP_NRET_N_SHFT                       0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_A_SLP_NRET_N_BMSK                      0x200
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_A_SLP_NRET_N_SHFT                        0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_D_SLP_RET_N_BMSK                       0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_D_SLP_RET_N_SHFT                         0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B1_SLP_RET_N_BMSK                       0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B1_SLP_RET_N_SHFT                        0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_C_SLP_RET_N_BMSK                        0x40
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_C_SLP_RET_N_SHFT                         0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B0_SLP_RET_N_BMSK                       0x20
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_B0_SLP_RET_N_SHFT                        0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_A_SLP_RET_N_BMSK                        0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_PMEM_SLICE_A_SLP_RET_N_SHFT                         0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_BMSK                                  0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_SHFT                                  0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_BMSK                                   0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_SHFT                                   0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_1_0_BMSK                                      0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_1_0_SHFT                                      0x0

#define HWIO_MSS_OFFLINE_MEM_SVS_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_OFFLINE_MEM_SVS_RMSK                                               0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SVS_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SVS_ADDR, HWIO_MSS_OFFLINE_MEM_SVS_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SVS_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SVS_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SVS_MSS_RESERVE_03_BMSK                                0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SVS_MSS_RESERVE_03_SHFT                                       0x0

#define HWIO_MSS_MODEM_BCR_ADDR                                                     (MPSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_MODEM_BCR_RMSK                                                     0x80000001
#define HWIO_MSS_MODEM_BCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, HWIO_MSS_MODEM_BCR_RMSK)
#define HWIO_MSS_MODEM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, m)
#define HWIO_MSS_MODEM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MODEM_BCR_ADDR,v)
#define HWIO_MSS_MODEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MODEM_BCR_ADDR,m,v,HWIO_MSS_MODEM_BCR_IN)
#define HWIO_MSS_MODEM_BCR_STATUS_BMSK                                              0x80000000
#define HWIO_MSS_MODEM_BCR_STATUS_SHFT                                                    0x1f
#define HWIO_MSS_MODEM_BCR_BLK_ARES_BMSK                                                   0x1
#define HWIO_MSS_MODEM_BCR_BLK_ARES_SHFT                                                   0x0

#define HWIO_MSS_AXI_MNOC_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_AXI_MNOC_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_AXI_MNOC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_MNOC_CBCR_ADDR, HWIO_MSS_AXI_MNOC_CBCR_RMSK)
#define HWIO_MSS_AXI_MNOC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_MNOC_CBCR_ADDR, m)
#define HWIO_MSS_AXI_MNOC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_MNOC_CBCR_ADDR,v)
#define HWIO_MSS_AXI_MNOC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_MNOC_CBCR_ADDR,m,v,HWIO_MSS_AXI_MNOC_CBCR_IN)
#define HWIO_MSS_AXI_MNOC_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_AXI_MNOC_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_AXI_MNOC_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_AXI_MNOC_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_XO_MDM_CBCR_ADDR                                                   (MPSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_XO_MDM_CBCR_RMSK                                                   0x80000001
#define HWIO_MSS_XO_MDM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MDM_CBCR_ADDR, HWIO_MSS_XO_MDM_CBCR_RMSK)
#define HWIO_MSS_XO_MDM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MDM_CBCR_ADDR, m)
#define HWIO_MSS_XO_MDM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_MDM_CBCR_ADDR,v)
#define HWIO_MSS_XO_MDM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_MDM_CBCR_ADDR,m,v,HWIO_MSS_XO_MDM_CBCR_IN)
#define HWIO_MSS_XO_MDM_CBCR_CLKOFF_BMSK                                            0x80000000
#define HWIO_MSS_XO_MDM_CBCR_CLKOFF_SHFT                                                  0x1f
#define HWIO_MSS_XO_MDM_CBCR_CLKEN_BMSK                                                    0x1
#define HWIO_MSS_XO_MDM_CBCR_CLKEN_SHFT                                                    0x0

#define HWIO_MSS_BUS_CFG_XO_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BUS_CFG_XO_RMSK                                                           0x1
#define HWIO_MSS_BUS_CFG_XO_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_XO_ADDR, HWIO_MSS_BUS_CFG_XO_RMSK)
#define HWIO_MSS_BUS_CFG_XO_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_XO_ADDR, m)
#define HWIO_MSS_BUS_CFG_XO_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_XO_ADDR,v)
#define HWIO_MSS_BUS_CFG_XO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_XO_ADDR,m,v,HWIO_MSS_BUS_CFG_XO_IN)
#define HWIO_MSS_BUS_CFG_XO_DISABLE_SPM_XO_EN_BMSK                                         0x1
#define HWIO_MSS_BUS_CFG_XO_DISABLE_SPM_XO_EN_SHFT                                         0x0

#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_RMSK                                                  0x3
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_IN          \
        in_dword_masked(HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR, HWIO_MSS_FABRIC_AXI2AHB_IDLE_RMSK)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_INM(m)      \
        in_dword_masked(HWIO_MSS_FABRIC_AXI2AHB_IDLE_ADDR, m)
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AHB_S_IDLE_BMSK                                       0x2
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AHB_S_IDLE_SHFT                                       0x1
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AXI_M_IDLE_BMSK                                       0x1
#define HWIO_MSS_FABRIC_AXI2AHB_IDLE_AXI_M_IDLE_SHFT                                       0x0

#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x00001128)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_RMSK                                         0xfff
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_DIVIDER_SEL_BMSK                             0x800
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_DIVIDER_SEL_SHFT                               0xb
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_SERDES_ODIV_MBIST_BMSK                       0x7e0
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_SERDES_ODIV_MBIST_SHFT                         0x5
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_PROC_ODIV_MBIST_BMSK                          0x1f
#define HWIO_MSS_OFFLINE_PLL_ODIV_MBIST_CTL_PROC_ODIV_MBIST_SHFT                           0x0

#define HWIO_MSS_OFFLINE_CLK_CTL_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x0000112c)
#define HWIO_MSS_OFFLINE_CLK_CTL_RMSK                                                      0x3
#define HWIO_MSS_OFFLINE_CLK_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_CLK_CTL_ADDR, HWIO_MSS_OFFLINE_CLK_CTL_RMSK)
#define HWIO_MSS_OFFLINE_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_CLK_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_CLK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_CLK_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_CLK_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_CLK_CTL_IN)
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_SERDES_ENABLE_BMSK                                    0x2
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_SERDES_ENABLE_SHFT                                    0x1
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_PROC_MDM_ENABLE_BMSK                                  0x1
#define HWIO_MSS_OFFLINE_CLK_CTL_CLK_PROC_MDM_ENABLE_SHFT                                  0x0

#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_ADDR                                          (MPSS_PERPH_REG_BASE      + 0x00002000)
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_RMSK                                                 0x1
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_ADDR, HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_RMSK)
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_ADDR,m,v,HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_IN)
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_GPLL0_BMSK                                           0x1
#define HWIO_MSS_Q6_PBL_GPLL_ENA_VOTE_GPLL0_SHFT                                           0x0

#define HWIO_MSS_GPLL0_MODE_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002004)
#define HWIO_MSS_GPLL0_MODE_RMSK                                                    0xc0000000
#define HWIO_MSS_GPLL0_MODE_IN          \
        in_dword_masked(HWIO_MSS_GPLL0_MODE_ADDR, HWIO_MSS_GPLL0_MODE_RMSK)
#define HWIO_MSS_GPLL0_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_GPLL0_MODE_ADDR, m)
#define HWIO_MSS_GPLL0_MODE_PLL_LOCK_DET_BMSK                                       0x80000000
#define HWIO_MSS_GPLL0_MODE_PLL_LOCK_DET_SHFT                                             0x1f
#define HWIO_MSS_GPLL0_MODE_PLL_ACTIVE_FLAG_BMSK                                    0x40000000
#define HWIO_MSS_GPLL0_MODE_PLL_ACTIVE_FLAG_SHFT                                          0x1e

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x00002008)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                                  0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                         0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                               0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                                  0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                                  0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x0000200c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                                       0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                               0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                                 0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                                0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                                 0x0

#define HWIO_MSS_UIM0_SRC_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002010)
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_SRC_CMD_RCGR_ADDR, HWIO_MSS_UIM0_SRC_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_SRC_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_SRC_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_SRC_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_SRC_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM0_SRC_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM0_SRC_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002014)
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_RMSK                                                   0x1f
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_SRC_CFG_RCGR_ADDR, HWIO_MSS_UIM0_SRC_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_SRC_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_SRC_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_SRC_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_SRC_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_SRC_DIV_BMSK                                           0x1f
#define HWIO_MSS_UIM0_SRC_CFG_RCGR_SRC_DIV_SHFT                                            0x0

#define HWIO_MSS_UIM1_SRC_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002018)
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_SRC_CMD_RCGR_ADDR, HWIO_MSS_UIM1_SRC_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_SRC_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_SRC_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_SRC_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_SRC_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM1_SRC_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM1_SRC_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000201c)
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_RMSK                                                   0x1f
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_SRC_CFG_RCGR_ADDR, HWIO_MSS_UIM1_SRC_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_SRC_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_SRC_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_SRC_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_SRC_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_SRC_DIV_BMSK                                           0x1f
#define HWIO_MSS_UIM1_SRC_CFG_RCGR_SRC_DIV_SHFT                                            0x0

#define HWIO_MSS_UIM2_SRC_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002020)
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_SRC_CMD_RCGR_ADDR, HWIO_MSS_UIM2_SRC_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_SRC_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_SRC_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_SRC_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_SRC_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM2_SRC_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM2_SRC_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002024)
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_RMSK                                                   0x1f
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_SRC_CFG_RCGR_ADDR, HWIO_MSS_UIM2_SRC_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_SRC_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_SRC_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_SRC_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_SRC_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_SRC_DIV_BMSK                                           0x1f
#define HWIO_MSS_UIM2_SRC_CFG_RCGR_SRC_DIV_SHFT                                            0x0

#define HWIO_MSS_UIM3_SRC_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002028)
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_SRC_CMD_RCGR_ADDR, HWIO_MSS_UIM3_SRC_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_SRC_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_SRC_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_SRC_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM3_SRC_CMD_RCGR_IN)
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM3_SRC_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM3_SRC_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000202c)
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_RMSK                                                   0x1f
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_SRC_CFG_RCGR_ADDR, HWIO_MSS_UIM3_SRC_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_SRC_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_SRC_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_SRC_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM3_SRC_CFG_RCGR_IN)
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_SRC_DIV_BMSK                                           0x1f
#define HWIO_MSS_UIM3_SRC_CFG_RCGR_SRC_DIV_SHFT                                            0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002030)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002034)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                                 0x3007
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                            0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                               0xc
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC1_ACTIVE_BMSK                                        0x4
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC1_ACTIVE_SHFT                                        0x2
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC0_ACTIVE_BMSK                                        0x2
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC0_ACTIVE_SHFT                                        0x1
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC_SEL_BMSK                                            0x1
#define HWIO_MSS_UIM0_MND_CFG_RCGR_SRC_SEL_SHFT                                            0x0

#define HWIO_MSS_UIM0_MND_M_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002038)
#define HWIO_MSS_UIM0_MND_M_RMSK                                                        0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_BMSK                                                      0xffff
#define HWIO_MSS_UIM0_MND_M_M_SHFT                                                         0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x0000203c)
#define HWIO_MSS_UIM0_MND_N_RMSK                                                        0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_BMSK                                          0xffff
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_SHFT                                             0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002040)
#define HWIO_MSS_UIM0_MND_D_RMSK                                                        0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_NOT_2D_BMSK                                                 0xffff
#define HWIO_MSS_UIM0_MND_D_NOT_2D_SHFT                                                    0x0

#define HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002044)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM0_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002048)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_UART_BIT_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM0_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x0000204c)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                                 0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                                   0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                                      0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                 0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                    0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                   0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                               0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                                 0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                                 0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                                  0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002050)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002054)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                                 0x3007
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                            0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                               0xc
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC1_ACTIVE_BMSK                                        0x4
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC1_ACTIVE_SHFT                                        0x2
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC0_ACTIVE_BMSK                                        0x2
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC0_ACTIVE_SHFT                                        0x1
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC_SEL_BMSK                                            0x1
#define HWIO_MSS_UIM1_MND_CFG_RCGR_SRC_SEL_SHFT                                            0x0

#define HWIO_MSS_UIM1_MND_M_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002058)
#define HWIO_MSS_UIM1_MND_M_RMSK                                                        0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_BMSK                                                      0xffff
#define HWIO_MSS_UIM1_MND_M_M_SHFT                                                         0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x0000205c)
#define HWIO_MSS_UIM1_MND_N_RMSK                                                        0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_BMSK                                          0xffff
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_SHFT                                             0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002060)
#define HWIO_MSS_UIM1_MND_D_RMSK                                                        0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_NOT_2D_BMSK                                                 0xffff
#define HWIO_MSS_UIM1_MND_D_NOT_2D_SHFT                                                    0x0

#define HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002064)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM1_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002068)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_UART_BIT_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM1_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x0000206c)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                                 0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                                   0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                                      0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                 0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                    0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                   0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                               0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                                 0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                                 0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                                  0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002070)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002074)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                                 0x3007
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                            0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                               0xc
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC1_ACTIVE_BMSK                                        0x4
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC1_ACTIVE_SHFT                                        0x2
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC0_ACTIVE_BMSK                                        0x2
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC0_ACTIVE_SHFT                                        0x1
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC_SEL_BMSK                                            0x1
#define HWIO_MSS_UIM2_MND_CFG_RCGR_SRC_SEL_SHFT                                            0x0

#define HWIO_MSS_UIM2_MND_M_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002078)
#define HWIO_MSS_UIM2_MND_M_RMSK                                                        0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_BMSK                                                      0xffff
#define HWIO_MSS_UIM2_MND_M_M_SHFT                                                         0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x0000207c)
#define HWIO_MSS_UIM2_MND_N_RMSK                                                        0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_BMSK                                          0xffff
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_SHFT                                             0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002080)
#define HWIO_MSS_UIM2_MND_D_RMSK                                                        0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_NOT_2D_BMSK                                                 0xffff
#define HWIO_MSS_UIM2_MND_D_NOT_2D_SHFT                                                    0x0

#define HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002084)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM2_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x00002088)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_UART_BIT_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM2_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x0000208c)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                                 0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                                   0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                                      0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                 0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                    0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                   0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                               0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                                 0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                                 0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                                  0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002090)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK                                             0x80000003
#define HWIO_MSS_UIM3_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002094)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK                                                 0x3007
#define HWIO_MSS_UIM3_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_BMSK                                            0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_SHFT                                               0xc
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC1_ACTIVE_BMSK                                        0x4
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC1_ACTIVE_SHFT                                        0x2
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC0_ACTIVE_BMSK                                        0x2
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC0_ACTIVE_SHFT                                        0x1
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC_SEL_BMSK                                            0x1
#define HWIO_MSS_UIM3_MND_CFG_RCGR_SRC_SEL_SHFT                                            0x0

#define HWIO_MSS_UIM3_MND_M_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002098)
#define HWIO_MSS_UIM3_MND_M_RMSK                                                        0xffff
#define HWIO_MSS_UIM3_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, HWIO_MSS_UIM3_MND_M_RMSK)
#define HWIO_MSS_UIM3_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, m)
#define HWIO_MSS_UIM3_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_M_ADDR,v)
#define HWIO_MSS_UIM3_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_M_ADDR,m,v,HWIO_MSS_UIM3_MND_M_IN)
#define HWIO_MSS_UIM3_MND_M_M_BMSK                                                      0xffff
#define HWIO_MSS_UIM3_MND_M_M_SHFT                                                         0x0

#define HWIO_MSS_UIM3_MND_N_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x0000209c)
#define HWIO_MSS_UIM3_MND_N_RMSK                                                        0xffff
#define HWIO_MSS_UIM3_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, HWIO_MSS_UIM3_MND_N_RMSK)
#define HWIO_MSS_UIM3_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, m)
#define HWIO_MSS_UIM3_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_N_ADDR,v)
#define HWIO_MSS_UIM3_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_N_ADDR,m,v,HWIO_MSS_UIM3_MND_N_IN)
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_BMSK                                          0xffff
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_SHFT                                             0x0

#define HWIO_MSS_UIM3_MND_D_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x000020a0)
#define HWIO_MSS_UIM3_MND_D_RMSK                                                        0xffff
#define HWIO_MSS_UIM3_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, HWIO_MSS_UIM3_MND_D_RMSK)
#define HWIO_MSS_UIM3_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, m)
#define HWIO_MSS_UIM3_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_D_ADDR,v)
#define HWIO_MSS_UIM3_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_D_ADDR,m,v,HWIO_MSS_UIM3_MND_D_IN)
#define HWIO_MSS_UIM3_MND_D_NOT_2D_BMSK                                                 0xffff
#define HWIO_MSS_UIM3_MND_D_NOT_2D_SHFT                                                    0x0

#define HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020a4)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM3_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020a8)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_UART_BIT_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM3_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_UIM3_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x000020ac)
#define HWIO_MSS_BUS_UIM3_CBCR_RMSK                                                 0x80007ff1
#define HWIO_MSS_BUS_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, HWIO_MSS_BUS_UIM3_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM3_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM3_CBCR_IN)
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_BMSK                                   0x4000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_SHFT                                      0xe
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                 0x2000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                    0xd
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                0x1000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                   0xc
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_BMSK                                               0xf00
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_SHFT                                                 0x8
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_BMSK                                                 0xf0
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_SHFT                                                  0x4
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_BUS_Q6SS_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x000020b0)
#define HWIO_MSS_BUS_Q6SS_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_BUS_Q6SS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, HWIO_MSS_BUS_Q6SS_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6SS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6SS_CBCR_IN)
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_NAV_BCR_ADDR                                                       (MPSS_PERPH_REG_BASE      + 0x000020b4)
#define HWIO_MSS_NAV_BCR_RMSK                                                       0x80000001
#define HWIO_MSS_NAV_BCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, HWIO_MSS_NAV_BCR_RMSK)
#define HWIO_MSS_NAV_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, m)
#define HWIO_MSS_NAV_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_BCR_ADDR,v)
#define HWIO_MSS_NAV_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_BCR_ADDR,m,v,HWIO_MSS_NAV_BCR_IN)
#define HWIO_MSS_NAV_BCR_STATUS_BMSK                                                0x80000000
#define HWIO_MSS_NAV_BCR_STATUS_SHFT                                                      0x1f
#define HWIO_MSS_NAV_BCR_BLK_ARES_BMSK                                                     0x1
#define HWIO_MSS_NAV_BCR_BLK_ARES_SHFT                                                     0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x000020b8)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_AXI_NAV_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x000020bc)
#define HWIO_MSS_AXI_NAV_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_AXI_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_NAV_CBCR_ADDR, HWIO_MSS_AXI_NAV_CBCR_RMSK)
#define HWIO_MSS_AXI_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_NAV_CBCR_ADDR, m)
#define HWIO_MSS_AXI_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_NAV_CBCR_ADDR,v)
#define HWIO_MSS_AXI_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_NAV_CBCR_ADDR,m,v,HWIO_MSS_AXI_NAV_CBCR_IN)
#define HWIO_MSS_AXI_NAV_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_AXI_NAV_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_AXI_NAV_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_AXI_NAV_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_ADDR                                           (MPSS_PERPH_REG_BASE      + 0x000020c0)
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_NAV_SAMPLE_CBCR_ADDR, HWIO_MSS_AXI_NAV_SAMPLE_CBCR_RMSK)
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_NAV_SAMPLE_CBCR_ADDR, m)
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_NAV_SAMPLE_CBCR_ADDR,v)
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_NAV_SAMPLE_CBCR_ADDR,m,v,HWIO_MSS_AXI_NAV_SAMPLE_CBCR_IN)
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_AXI_NAV_SAMPLE_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_NAV_CBCR_ADDR                                                   (MPSS_PERPH_REG_BASE      + 0x000020c4)
#define HWIO_MSS_XO_NAV_CBCR_RMSK                                                   0x80000001
#define HWIO_MSS_XO_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, HWIO_MSS_XO_NAV_CBCR_RMSK)
#define HWIO_MSS_XO_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, m)
#define HWIO_MSS_XO_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_NAV_CBCR_ADDR,v)
#define HWIO_MSS_XO_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_NAV_CBCR_ADDR,m,v,HWIO_MSS_XO_NAV_CBCR_IN)
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_BMSK                                            0x80000000
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_SHFT                                                  0x1f
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_BMSK                                                    0x1
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_SHFT                                                    0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x000020c8)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                               0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                        0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                              0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                                0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                                0x0

#define HWIO_MSS_AXI_CRYPTO_CBCR_ADDR                                               (MPSS_PERPH_REG_BASE      + 0x000020cc)
#define HWIO_MSS_AXI_CRYPTO_CBCR_RMSK                                               0x80000001
#define HWIO_MSS_AXI_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR, HWIO_MSS_AXI_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_AXI_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_AXI_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_AXI_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_AXI_CRYPTO_CBCR_IN)
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKOFF_BMSK                                        0x80000000
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKOFF_SHFT                                              0x1f
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKEN_BMSK                                                0x1
#define HWIO_MSS_AXI_CRYPTO_CBCR_CLKEN_SHFT                                                0x0

#define HWIO_MSS_AXI_DMA_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x000020d0)
#define HWIO_MSS_AXI_DMA_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_AXI_DMA_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_DMA_CBCR_ADDR, HWIO_MSS_AXI_DMA_CBCR_RMSK)
#define HWIO_MSS_AXI_DMA_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_DMA_CBCR_ADDR, m)
#define HWIO_MSS_AXI_DMA_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_DMA_CBCR_ADDR,v)
#define HWIO_MSS_AXI_DMA_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_DMA_CBCR_ADDR,m,v,HWIO_MSS_AXI_DMA_CBCR_IN)
#define HWIO_MSS_AXI_DMA_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_AXI_DMA_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_AXI_DMA_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_AXI_DMA_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x000020d4)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK                                         0x80000003
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR                                         (MPSS_PERPH_REG_BASE      + 0x000020d8)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK                                             0x371f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_BMSK                                        0x3000
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_SHFT                                           0xc
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_MSS_BIT_COXM_MND_M_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x000020dc)
#define HWIO_MSS_BIT_COXM_MND_M_RMSK                                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_M_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, HWIO_MSS_BIT_COXM_MND_M_RMSK)
#define HWIO_MSS_BIT_COXM_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_M_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_M_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_M_IN)
#define HWIO_MSS_BIT_COXM_MND_M_M_BMSK                                                  0xffff
#define HWIO_MSS_BIT_COXM_MND_M_M_SHFT                                                     0x0

#define HWIO_MSS_BIT_COXM_MND_N_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x000020e0)
#define HWIO_MSS_BIT_COXM_MND_N_RMSK                                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_N_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, HWIO_MSS_BIT_COXM_MND_N_RMSK)
#define HWIO_MSS_BIT_COXM_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_N_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_N_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_N_IN)
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_BMSK                                      0xffff
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_SHFT                                         0x0

#define HWIO_MSS_BIT_COXM_MND_D_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x000020e4)
#define HWIO_MSS_BIT_COXM_MND_D_RMSK                                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_D_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, HWIO_MSS_BIT_COXM_MND_D_RMSK)
#define HWIO_MSS_BIT_COXM_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_D_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_D_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_D_IN)
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_BMSK                                             0xffff
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_SHFT                                                0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x000020e8)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                                 0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                                   0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                                      0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                 0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                    0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                   0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                               0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                                 0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                                 0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                                  0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x000020ec)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020f0)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK                                            0x80000003
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CMD_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_BMSK                                           0x2
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_SHFT                                           0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR                                            (MPSS_PERPH_REG_BASE      + 0x000020f4)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK                                                 0x71f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CFG_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_BMSK                                         0x700
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_SHFT                                           0x8
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x000020f8)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                                0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                                         0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                               0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                                 0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                                 0x0

#define HWIO_MSS_RBCPR_REF_CBCR_ADDR                                                (MPSS_PERPH_REG_BASE      + 0x000020fc)
#define HWIO_MSS_RBCPR_REF_CBCR_RMSK                                                0x80000001
#define HWIO_MSS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_BMSK                                         0x80000000
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_SHFT                                               0x1f
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_BMSK                                                 0x1
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_SHFT                                                 0x0

#define HWIO_MSS_BUS_MGPI_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x00002100)
#define HWIO_MSS_BUS_MGPI_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_BUS_MGPI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, HWIO_MSS_BUS_MGPI_CBCR_RMSK)
#define HWIO_MSS_BUS_MGPI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MGPI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MGPI_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MGPI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MGPI_CBCR_ADDR,m,v,HWIO_MSS_BUS_MGPI_CBCR_IN)
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                                        (MPSS_PERPH_REG_BASE      + 0x00002104)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_ADDR                                    (MPSS_PERPH_REG_BASE      + 0x00002108)
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_RMSK                                    0x80000001
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_ADDR, HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_RMSK)
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_ADDR,m,v,HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_IN)
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_CLKOFF_BMSK                             0x80000000
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_CLKOFF_SHFT                                   0x1f
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_CLKEN_BMSK                                     0x1
#define HWIO_MSS_BUS_GERAN_ENCRYPT_ACC_CBCR_CLKEN_SHFT                                     0x0

#define HWIO_MSS_BUS_STMR_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x0000210c)
#define HWIO_MSS_BUS_STMR_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_BUS_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, HWIO_MSS_BUS_STMR_CBCR_RMSK)
#define HWIO_MSS_BUS_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_STMR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_STMR_CBCR_ADDR,m,v,HWIO_MSS_BUS_STMR_CBCR_IN)
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_XO_CX_CBCR_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002110)
#define HWIO_MSS_XO_CX_CBCR_RMSK                                                    0x80000001
#define HWIO_MSS_XO_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_CX_CBCR_ADDR, HWIO_MSS_XO_CX_CBCR_RMSK)
#define HWIO_MSS_XO_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_CX_CBCR_ADDR, m)
#define HWIO_MSS_XO_CX_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_CX_CBCR_ADDR,v)
#define HWIO_MSS_XO_CX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_CX_CBCR_ADDR,m,v,HWIO_MSS_XO_CX_CBCR_IN)
#define HWIO_MSS_XO_CX_CBCR_CLKOFF_BMSK                                             0x80000000
#define HWIO_MSS_XO_CX_CBCR_CLKOFF_SHFT                                                   0x1f
#define HWIO_MSS_XO_CX_CBCR_CLKEN_BMSK                                                     0x1
#define HWIO_MSS_XO_CX_CBCR_CLKEN_SHFT                                                     0x0

#define HWIO_MSS_2XO_RFFE_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x00002114)
#define HWIO_MSS_2XO_RFFE_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_2XO_RFFE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_2XO_RFFE_CBCR_ADDR, HWIO_MSS_2XO_RFFE_CBCR_RMSK)
#define HWIO_MSS_2XO_RFFE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_2XO_RFFE_CBCR_ADDR, m)
#define HWIO_MSS_2XO_RFFE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_2XO_RFFE_CBCR_ADDR,v)
#define HWIO_MSS_2XO_RFFE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_2XO_RFFE_CBCR_ADDR,m,v,HWIO_MSS_2XO_RFFE_CBCR_IN)
#define HWIO_MSS_2XO_RFFE_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_2XO_RFFE_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_2XO_RFFE_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_2XO_RFFE_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_XO_RFFE_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x00002118)
#define HWIO_MSS_XO_RFFE_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_XO_RFFE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_RFFE_CBCR_ADDR, HWIO_MSS_XO_RFFE_CBCR_RMSK)
#define HWIO_MSS_XO_RFFE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_RFFE_CBCR_ADDR, m)
#define HWIO_MSS_XO_RFFE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_RFFE_CBCR_ADDR,v)
#define HWIO_MSS_XO_RFFE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_RFFE_CBCR_ADDR,m,v,HWIO_MSS_XO_RFFE_CBCR_IN)
#define HWIO_MSS_XO_RFFE_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_XO_RFFE_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_XO_RFFE_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_XO_RFFE_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_BUS_RFC_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x0000211c)
#define HWIO_MSS_BUS_RFC_CBCR_RMSK                                                  0x80007ff1
#define HWIO_MSS_BUS_RFC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RFC_CBCR_ADDR, HWIO_MSS_BUS_RFC_CBCR_RMSK)
#define HWIO_MSS_BUS_RFC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RFC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RFC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RFC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RFC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RFC_CBCR_ADDR,m,v,HWIO_MSS_BUS_RFC_CBCR_IN)
#define HWIO_MSS_BUS_RFC_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_BUS_RFC_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_CORE_ON_BMSK                                    0x4000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_CORE_ON_SHFT                                       0xe
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                  0x2000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                     0xd
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                 0x1000
#define HWIO_MSS_BUS_RFC_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                    0xc
#define HWIO_MSS_BUS_RFC_CBCR_WAKEUP_BMSK                                                0xf00
#define HWIO_MSS_BUS_RFC_CBCR_WAKEUP_SHFT                                                  0x8
#define HWIO_MSS_BUS_RFC_CBCR_SLEEP_BMSK                                                  0xf0
#define HWIO_MSS_BUS_RFC_CBCR_SLEEP_SHFT                                                   0x4
#define HWIO_MSS_BUS_RFC_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_BUS_RFC_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_SW_FEATURE_FUSES_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002120)
#define HWIO_MSS_SW_FEATURE_FUSES_RMSK                                              0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, HWIO_MSS_SW_FEATURE_FUSES_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES_MSS_SW_FEATURE_FUSES_31_0_BMSK                    0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_MSS_SW_FEATURE_FUSES_31_0_SHFT                           0x0

#define HWIO_MSS_SW_FEATURE_FUSES2_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x00002124)
#define HWIO_MSS_SW_FEATURE_FUSES2_RMSK                                             0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES2_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES2_ADDR, HWIO_MSS_SW_FEATURE_FUSES2_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES2_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES2_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES2_MSS_SW_FEATURE_FUSES2_31_0_BMSK                  0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES2_MSS_SW_FEATURE_FUSES2_31_0_SHFT                         0x0

#define HWIO_MSS_RESERVE_01_ADDR                                                    (MPSS_PERPH_REG_BASE      + 0x00002128)
#define HWIO_MSS_RESERVE_01_RMSK                                                    0xffffffff
#define HWIO_MSS_RESERVE_01_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, HWIO_MSS_RESERVE_01_RMSK)
#define HWIO_MSS_RESERVE_01_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, m)
#define HWIO_MSS_RESERVE_01_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_01_ADDR,v)
#define HWIO_MSS_RESERVE_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_01_ADDR,m,v,HWIO_MSS_RESERVE_01_IN)
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_BMSK                                     0xffffffff
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_SHFT                                            0x0

#define HWIO_MSS_QLPHY_REFGEN_VOTE_ADDR                                             (MPSS_PERPH_REG_BASE      + 0x0000212c)
#define HWIO_MSS_QLPHY_REFGEN_VOTE_RMSK                                                    0x1
#define HWIO_MSS_QLPHY_REFGEN_VOTE_IN          \
        in_dword_masked(HWIO_MSS_QLPHY_REFGEN_VOTE_ADDR, HWIO_MSS_QLPHY_REFGEN_VOTE_RMSK)
#define HWIO_MSS_QLPHY_REFGEN_VOTE_INM(m)      \
        in_dword_masked(HWIO_MSS_QLPHY_REFGEN_VOTE_ADDR, m)
#define HWIO_MSS_QLPHY_REFGEN_VOTE_OUT(v)      \
        out_dword(HWIO_MSS_QLPHY_REFGEN_VOTE_ADDR,v)
#define HWIO_MSS_QLPHY_REFGEN_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QLPHY_REFGEN_VOTE_ADDR,m,v,HWIO_MSS_QLPHY_REFGEN_VOTE_IN)
#define HWIO_MSS_QLPHY_REFGEN_VOTE_MSS_QLPHY_REFGEN_VOTE_BMSK                              0x1
#define HWIO_MSS_QLPHY_REFGEN_VOTE_MSS_QLPHY_REFGEN_VOTE_SHFT                              0x0

#define HWIO_MSS_XO_RSCC_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x00002130)
#define HWIO_MSS_XO_RSCC_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_XO_RSCC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_RSCC_CBCR_ADDR, HWIO_MSS_XO_RSCC_CBCR_RMSK)
#define HWIO_MSS_XO_RSCC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_RSCC_CBCR_ADDR, m)
#define HWIO_MSS_XO_RSCC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_RSCC_CBCR_ADDR,v)
#define HWIO_MSS_XO_RSCC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_RSCC_CBCR_ADDR,m,v,HWIO_MSS_XO_RSCC_CBCR_IN)
#define HWIO_MSS_XO_RSCC_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_XO_RSCC_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_XO_RSCC_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_XO_RSCC_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_BUS_RSCC_CBCR_ADDR                                                 (MPSS_PERPH_REG_BASE      + 0x00002134)
#define HWIO_MSS_BUS_RSCC_CBCR_RMSK                                                 0x80000001
#define HWIO_MSS_BUS_RSCC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RSCC_CBCR_ADDR, HWIO_MSS_BUS_RSCC_CBCR_RMSK)
#define HWIO_MSS_BUS_RSCC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RSCC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RSCC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RSCC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RSCC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RSCC_CBCR_ADDR,m,v,HWIO_MSS_BUS_RSCC_CBCR_IN)
#define HWIO_MSS_BUS_RSCC_CBCR_CLKOFF_BMSK                                          0x80000000
#define HWIO_MSS_BUS_RSCC_CBCR_CLKOFF_SHFT                                                0x1f
#define HWIO_MSS_BUS_RSCC_CBCR_CLKEN_BMSK                                                  0x1
#define HWIO_MSS_BUS_RSCC_CBCR_CLKEN_SHFT                                                  0x0

#define HWIO_MSS_SYSTEM_CACHE_CTL_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002138)
#define HWIO_MSS_SYSTEM_CACHE_CTL_RMSK                                              0xffffffff
#define HWIO_MSS_SYSTEM_CACHE_CTL_IN          \
        in_dword_masked(HWIO_MSS_SYSTEM_CACHE_CTL_ADDR, HWIO_MSS_SYSTEM_CACHE_CTL_RMSK)
#define HWIO_MSS_SYSTEM_CACHE_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_SYSTEM_CACHE_CTL_ADDR, m)
#define HWIO_MSS_SYSTEM_CACHE_CTL_OUT(v)      \
        out_dword(HWIO_MSS_SYSTEM_CACHE_CTL_ADDR,v)
#define HWIO_MSS_SYSTEM_CACHE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_SYSTEM_CACHE_CTL_ADDR,m,v,HWIO_MSS_SYSTEM_CACHE_CTL_IN)
#define HWIO_MSS_SYSTEM_CACHE_CTL_SPARE_31_19_BMSK                                  0xfff80000
#define HWIO_MSS_SYSTEM_CACHE_CTL_SPARE_31_19_SHFT                                        0x13
#define HWIO_MSS_SYSTEM_CACHE_CTL_AINNERCACHEABLE_BMSK                                 0x40000
#define HWIO_MSS_SYSTEM_CACHE_CTL_AINNERCACHEABLE_SHFT                                    0x12
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACGRANULETRANSACTION_BMSK                            0x20000
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACGRANULETRANSACTION_SHFT                               0x11
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACACHEABLE_BMSK                                      0x10000
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACACHEABLE_SHFT                                         0x10
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACACHEOPTYPE_BMSK                                     0xf000
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACACHEOPTYPE_SHFT                                        0xc
#define HWIO_MSS_SYSTEM_CACHE_CTL_ADEVICETYPE_BMSK                                       0x800
#define HWIO_MSS_SYSTEM_CACHE_CTL_ADEVICETYPE_SHFT                                         0xb
#define HWIO_MSS_SYSTEM_CACHE_CTL_ADEVICE_BMSK                                           0x400
#define HWIO_MSS_SYSTEM_CACHE_CTL_ADEVICE_SHFT                                             0xa
#define HWIO_MSS_SYSTEM_CACHE_CTL_AWRITETHROUGH_BMSK                                     0x200
#define HWIO_MSS_SYSTEM_CACHE_CTL_AWRITETHROUGH_SHFT                                       0x9
#define HWIO_MSS_SYSTEM_CACHE_CTL_AINNERSHARED_BMSK                                      0x100
#define HWIO_MSS_SYSTEM_CACHE_CTL_AINNERSHARED_SHFT                                        0x8
#define HWIO_MSS_SYSTEM_CACHE_CTL_ASHARED_BMSK                                            0x80
#define HWIO_MSS_SYSTEM_CACHE_CTL_ASHARED_SHFT                                             0x7
#define HWIO_MSS_SYSTEM_CACHE_CTL_ANOALLOCATE_BMSK                                        0x40
#define HWIO_MSS_SYSTEM_CACHE_CTL_ANOALLOCATE_SHFT                                         0x6
#define HWIO_MSS_SYSTEM_CACHE_CTL_ATRANSIENT_BMSK                                         0x20
#define HWIO_MSS_SYSTEM_CACHE_CTL_ATRANSIENT_SHFT                                          0x5
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACID_BMSK                                               0x1f
#define HWIO_MSS_SYSTEM_CACHE_CTL_ACID_SHFT                                                0x0

#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_ADDR                                       (MPSS_PERPH_REG_BASE      + 0x0000213c)
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_ADDR, HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_RMSK)
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_ADDR,m,v,HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_IN)
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_ALT_RESET_Q6SS_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_RFFE_TP_CBCR_ADDR                                                  (MPSS_PERPH_REG_BASE      + 0x00002140)
#define HWIO_MSS_RFFE_TP_CBCR_RMSK                                                  0x80000001
#define HWIO_MSS_RFFE_TP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_RFFE_TP_CBCR_ADDR, HWIO_MSS_RFFE_TP_CBCR_RMSK)
#define HWIO_MSS_RFFE_TP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RFFE_TP_CBCR_ADDR, m)
#define HWIO_MSS_RFFE_TP_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_RFFE_TP_CBCR_ADDR,v)
#define HWIO_MSS_RFFE_TP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RFFE_TP_CBCR_ADDR,m,v,HWIO_MSS_RFFE_TP_CBCR_IN)
#define HWIO_MSS_RFFE_TP_CBCR_CLKOFF_BMSK                                           0x80000000
#define HWIO_MSS_RFFE_TP_CBCR_CLKOFF_SHFT                                                 0x1f
#define HWIO_MSS_RFFE_TP_CBCR_CLKEN_BMSK                                                   0x1
#define HWIO_MSS_RFFE_TP_CBCR_CLKEN_SHFT                                                   0x0

#define HWIO_MSS_RFFE_TP_CMD_RCGR_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002144)
#define HWIO_MSS_RFFE_TP_CMD_RCGR_RMSK                                              0x80000003
#define HWIO_MSS_RFFE_TP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RFFE_TP_CMD_RCGR_ADDR, HWIO_MSS_RFFE_TP_CMD_RCGR_RMSK)
#define HWIO_MSS_RFFE_TP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RFFE_TP_CMD_RCGR_ADDR, m)
#define HWIO_MSS_RFFE_TP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RFFE_TP_CMD_RCGR_ADDR,v)
#define HWIO_MSS_RFFE_TP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RFFE_TP_CMD_RCGR_ADDR,m,v,HWIO_MSS_RFFE_TP_CMD_RCGR_IN)
#define HWIO_MSS_RFFE_TP_CMD_RCGR_ROOT_OFF_BMSK                                     0x80000000
#define HWIO_MSS_RFFE_TP_CMD_RCGR_ROOT_OFF_SHFT                                           0x1f
#define HWIO_MSS_RFFE_TP_CMD_RCGR_ROOT_EN_BMSK                                             0x2
#define HWIO_MSS_RFFE_TP_CMD_RCGR_ROOT_EN_SHFT                                             0x1
#define HWIO_MSS_RFFE_TP_CMD_RCGR_UPDATE_BMSK                                              0x1
#define HWIO_MSS_RFFE_TP_CMD_RCGR_UPDATE_SHFT                                              0x0

#define HWIO_MSS_RFFE_TP_CFG_RCGR_ADDR                                              (MPSS_PERPH_REG_BASE      + 0x00002148)
#define HWIO_MSS_RFFE_TP_CFG_RCGR_RMSK                                                   0x71f
#define HWIO_MSS_RFFE_TP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RFFE_TP_CFG_RCGR_ADDR, HWIO_MSS_RFFE_TP_CFG_RCGR_RMSK)
#define HWIO_MSS_RFFE_TP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RFFE_TP_CFG_RCGR_ADDR, m)
#define HWIO_MSS_RFFE_TP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RFFE_TP_CFG_RCGR_ADDR,v)
#define HWIO_MSS_RFFE_TP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RFFE_TP_CFG_RCGR_ADDR,m,v,HWIO_MSS_RFFE_TP_CFG_RCGR_IN)
#define HWIO_MSS_RFFE_TP_CFG_RCGR_SRC_SEL_BMSK                                           0x700
#define HWIO_MSS_RFFE_TP_CFG_RCGR_SRC_SEL_SHFT                                             0x8
#define HWIO_MSS_RFFE_TP_CFG_RCGR_SRC_DIV_BMSK                                            0x1f
#define HWIO_MSS_RFFE_TP_CFG_RCGR_SRC_DIV_SHFT                                             0x0


#endif /* __CPR_MPSS_HWIO__ */
