#ifndef __CHIPINFOHWIO_H__
#define __CHIPINFOHWIO_H__
/*
===========================================================================
*/
/**
  @file ChipInfoHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.4_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    TLMM_NORTH
    TCSR_TCSR_REGS
    SECURITY_CONTROL_CORE

  'Include' filters applied: QFPROM_CORR_FEAT_CONFIG*[SECURITY_CONTROL_CORE] QFPROM_CORR_PTE*[SECURITY_CONTROL_CORE] QFPROM_CORR_QC_SPARE*[SECURITY_CONTROL_CORE] TLMM_HW_REVISION_NUMBER[TLMM_NORTH] TCSR_SOC_HW_VERSION[TCSR_TCSR_REGS] 
  'Exclude' filters applied: DUMMY RESERVED 
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Include/ChipInfoHWIO.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

#define CHIPINFO_TLMM_HW_REVISION_NUMBER  TLMM_HW_REVISION_NUMBER
#define CHIPINFO_TCSR_SOC_HW_VERSION      TCSR_SOC_HW_VERSION

#define CHIPINFO_FOUNDRY_ID_REG           QFPROM_CORR_FEAT_CONFIG_ROW4_MSB
#define CHIPINFO_SERIAL_NUM_REG           QFPROM_CORR_PTE_ROW1_LSB
#define CHIPINFO_QFPROM_CHIP_ID_REG       QFPROM_CORR_PTE_ROW1_MSB

#define CHIPINFO_PARTIALGOOD_REG          QFPROM_CORR_PTE_ROW1_MSB
#define CHIPINFO_PARTIALGOOD_CPU_REG      QFPROM_CORR_FEAT_CONFIG_ROW1_MSB

// TODO: Technically, fusemaster shows only the RAW versions, not the CORR versions.
// RAW (the one associated with the listed address) is at 780324 whereas CORR is at 784324
#define CHIPINFO_PARTIALGOODS_SUBBIN_REG    QFPROM_CORR_QC_SPARE_REG16_MSB  

/*----------------------------------------------------------------------------
 * MODULE: TLMM_NORTH
 *--------------------------------------------------------------------------*/

#define TLMM_NORTH_REG_BASE                                                (TLMM_BASE      + 0x00500000)
#define TLMM_NORTH_REG_BASE_PHYS                                           (TLMM_BASE_PHYS + 0x00500000)

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                  (TLMM_NORTH_REG_BASE      + 0x00098010)
#define HWIO_TLMM_HW_REVISION_NUMBER_PHYS                                  (TLMM_NORTH_REG_BASE_PHYS + 0x00098010)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                  0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                       0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                             0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_BMSK                 0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_SHFT                       0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                       0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                         0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                               0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                               0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                      (CORE_TOP_CSR_BASE      + 0x000c0000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS                                                                                 (CORE_TOP_CSR_BASE_PHYS + 0x000c0000)

#define HWIO_TCSR_SOC_HW_VERSION_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00008000)
#define HWIO_TCSR_SOC_HW_VERSION_PHYS                                                                                (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x00008000)
#define HWIO_TCSR_SOC_HW_VERSION_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SOC_HW_VERSION_IN          \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, HWIO_TCSR_SOC_HW_VERSION_RMSK)
#define HWIO_TCSR_SOC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, m)
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_BMSK                                                                  0xf0000000
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_SHFT                                                                        0x1c
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_BMSK                                                                   0xfff0000
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_SHFT                                                                        0x10
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_BMSK                                                                      0xff00
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_SHFT                                                                         0x8
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_BMSK                                                                        0xff
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_SHFT                                                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                          (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_PHYS                                                     (SECURITY_CONTROL_BASE_PHYS + 0x00000000)

#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004130)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004130)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_PTE_DATA0_BMSK                                            0xe0000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_PTE_DATA0_SHFT                                                  0x1d
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_BMSK                                         0x10000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_SHFT                                               0x1c
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_BMSK                                            0xff00000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_BMSK                                                 0xfffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004134)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004134)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PTE_DATA1_BMSK                                            0xffff0000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PTE_DATA1_SHFT                                                  0x10
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_BMSK                                                  0xffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004140)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004140)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004144)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004144)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004148)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000414c)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041a0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_PHY_DISABLE_UPPER_LANE_BMSK                  0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_PHY_DISABLE_UPPER_LANE_SHFT                        0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_QC_SP_DISABLE_BMSK                                0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_QC_SP_DISABLE_SHFT                                      0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_FREQ_LIMIT_VAL_BMSK                         0x1fe00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_FREQ_LIMIT_VAL_SHFT                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_BMSK                          0x180000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_SHFT                              0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_BMSK                       0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_SHFT                          0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_BMSK                       0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_SHFT                          0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_BMSK                                0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_SHFT                                   0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_CORE1_BMSK                              0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_CORE1_SHFT                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_VPX_BMSK                                0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_VPX_SHFT                                   0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DP_DISABLE_BMSK                                       0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DP_DISABLE_SHFT                                          0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                     0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                        0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_BMSK                            0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_SHFT                              0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_GLOBAL_KEY_SPLIT2_DISABLE_BMSK                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_GLOBAL_KEY_SPLIT2_DISABLE_SHFT                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                     0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                       0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                     0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_BMSK                                         0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_SHFT                                          0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_BMSK                               0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_SHFT                                0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CAM_IPE_1_DISABLE_IF_BMSK                               0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CAM_IPE_1_DISABLE_IF_SHFT                                0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EUD_PORT1_SEL_BMSK                                      0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EUD_PORT1_SEL_SHFT                                       0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EUD_IGNR_CSR_BMSK                                        0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EUD_IGNR_CSR_SHFT                                        0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                              0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                              0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041a4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY1_BMSK                            0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY1_SHFT                                  0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY0_BMSK                            0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY0_SHFT                                  0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_DISABLE_BMSK                                  0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ICE_DISABLE_SHFT                                        0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_LLCC_FUSE_CACHE_SIZE_ZERO_BMSK                    0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_LLCC_FUSE_CACHE_SIZE_ZERO_SHFT                          0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD3_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD3_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_BMSK                                   0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_SHFT                                        0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_BMSK                0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_SHFT                     0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_BMSK                    0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_SHFT                         0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD2_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD2_SHFT                                              0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD1_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD1_SHFT                                              0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_BMSK                               0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_SHFT                                   0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                              0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_BMSK                               0x7c000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_SHFT                                   0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_BMSK                            0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_SHFT                               0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_BMSK                               0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_SHFT                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_BMSK                                        0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_SHFT                                          0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_BMSK                                     0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_SHFT                                       0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_BMSK                                      0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_SHFT                                        0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                       0x1e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                         0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_RESOLUTION_LIMIT_BMSK                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_RESOLUTION_LIMIT_SHFT                                0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_BMSK                                      0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_SHFT                                      0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_BMSK                                      0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_SHFT                                      0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041a8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MDSP_FW_DISABLE_BMSK                              0xfff00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MDSP_FW_DISABLE_SHFT                                    0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_TCM_BOOT_DISABLE_BMSK                          0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_TCM_BOOT_DISABLE_SHFT                             0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NAV_DISABLE_BMSK                                     0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NAV_DISABLE_SHFT                                        0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                           0x38000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                               0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_BMSK                      0x7fe0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_SHFT                         0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_BMSK                        0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_SHFT                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041ac)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APSS_PDXWRPNTRDELAY_BMSK                          0xc0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APSS_PDXWRPNTRDELAY_SHFT                                0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_APCSCFGAPMBOOTONMX_BMSK                       0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_APCSCFGAPMBOOTONMX_SHFT                             0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_APCCCFGCPUPRESENT_N_BMSK                      0x1fe00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_APCCCFGCPUPRESENT_N_SHFT                            0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD1_BMSK                                          0x1f0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD1_SHFT                                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                      0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                            0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_NIDEN_DISABLE_BMSK                         0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_NIDEN_DISABLE_SHFT                               0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_NIDEN_DISABLE_BMSK                         0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_NIDEN_DISABLE_SHFT                               0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_NIDEN_DISABLE_BMSK                     0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_NIDEN_DISABLE_SHFT                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_NIDEN_DISABLE_BMSK                     0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_NIDEN_DISABLE_SHFT                          0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_NIDEN_DISABLE_BMSK                         0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_NIDEN_DISABLE_SHFT                              0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_NIDEN_DISABLE_BMSK                 0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_NIDEN_DISABLE_SHFT                      0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_NIDEN_DISABLE_BMSK                          0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_NIDEN_DISABLE_SHFT                              0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_BMSK                           0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_SHFT                               0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                      0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                          0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                       0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                           0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_BMSK                               0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_SHFT                                  0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_BMSK                            0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_SHFT                               0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_DBGEN_DISABLE_BMSK                        0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_DBGEN_DISABLE_SHFT                           0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DBGEN_DISABLE_BMSK                       0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DBGEN_DISABLE_SHFT                          0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                            0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                               0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_DBGEN_DISABLE_BMSK                    0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_DBGEN_DISABLE_SHFT                       0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_BMSK                             0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_SHFT                                0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_DBGEN_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_DBGEN_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                           0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                             0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_EUD_DISABLE_BMSK                                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_EUD_DISABLE_SHFT                                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DFD_DISABLE_BMSK                           0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DFD_DISABLE_SHFT                             0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DEBUG_BUS_DISABLE_BMSK                              0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DEBUG_BUS_DISABLE_SHFT                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DCC_DEBUG_DISABLE_BMSK                               0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DCC_DEBUG_DISABLE_SHFT                                0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_APB_DFD_DISABLE_BMSK                            0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS_APB_DFD_DISABLE_SHFT                             0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_NIDEN_DISABLE_BMSK                               0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_NIDEN_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_DBGEN_DISABLE_BMSK                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_DBGEN_DISABLE_SHFT                                0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_DAPEN_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPU_DAPEN_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_BMSK                                    0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_SHFT                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_BMSK                                     0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_SHFT                                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_BMSK                                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_CFG_APC1PLL_LVAL_BMSK                         0xff000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_CFG_APC1PLL_LVAL_SHFT                               0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                              0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_CFG_L3_SIZE_RED_BMSK                            0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_CFG_L3_SIZE_RED_SHFT                                0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_AUTO_CCI_RCG_CFG_DISABLE_BMSK                       0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_AUTO_CCI_RCG_CFG_DISABLE_SHFT                           0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_FUSE_BMSK                             0x1f8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_FUSE_SHFT                                  0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                            0x7c00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                               0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_A5X_ISDB_SECURE_DBGEN_DISABLE_BMSK                  0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_A5X_ISDB_SECURE_DBGEN_DISABLE_SHFT                    0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE0_DISABLE_BMSK                                 0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE0_DISABLE_SHFT                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_LLCC_DSRW_DISABLE_BMSK                               0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_LLCC_DSRW_DISABLE_SHFT                                0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE5_DISABLE_BMSK                                  0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE5_DISABLE_SHFT                                   0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS_SPIDEN_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS_SPIDEN_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE4_DISABLE_BMSK                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE4_DISABLE_SHFT                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE3_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE3_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE2_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SPARE2_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_BMSK                              0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_SHFT                              0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_BMSK             0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_SHFT                   0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_BMSK                               0x3ffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041bc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041bc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                       0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_BMSK               0x3ffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_SHFT                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041c0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_BMSK                      0xfe000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_SHFT                            0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_BMSK                        0x1fc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_SHFT                             0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_BMSK                             0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_SHFT                                0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_BOOT_BMSK                                   0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_BOOT_SHFT                                      0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD2_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD2_SHFT                                               0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_BMSK                               0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_SHFT                                  0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD1_BMSK                                            0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD1_SHFT                                               0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_BMSK                               0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD0_BMSK                                             0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD0_SHFT                                               0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_BMSK                                 0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_SHFT                                   0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_BMSK                              0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_SHFT                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_BMSK                                       0xff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_SHFT                                        0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041c4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                    0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                          0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                   0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                         0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_XBL_SEC_AUTH_DISABLE_BMSK                         0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_XBL_SEC_AUTH_DISABLE_SHFT                               0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_MSM_PKG_TYPE_BMSK                                 0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_MSM_PKG_TYPE_SHFT                                       0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_SPSS_CONFIG_MODE_BMSK                              0xc000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_SPSS_CONFIG_MODE_SHFT                                   0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RSVD3_BMSK                                         0x3f80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RSVD3_SHFT                                              0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_BMSK                                      0x78000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_SHFT                                          0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_BMSK                                          0x7ff0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_SHFT                                             0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_BMSK                                   0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_SHFT                                   0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042a0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_PHYS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000042a0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_MAXn                                                         18
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042a4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_PHYS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000042a4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_MAXn                                                         18
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                               0x0


#endif /* __CHIPINFOHWIO_H__ */
