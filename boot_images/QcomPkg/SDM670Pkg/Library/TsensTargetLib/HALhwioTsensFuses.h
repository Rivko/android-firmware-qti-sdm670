#ifndef __HALHWIOTSENSFUSES_H__
#define __HALHWIOTSENSFUSES_H__
/*
===========================================================================
*/
/**
  @file HALhwioTsensFuse.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: QFPROM_CORR*[SECURITY_CONTROL_CORE] 

  Generation parameters: 
  { u'filename': u'HALhwioTsensFuse.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'SECURITY_CONTROL_CORE': [u'QFPROM_CORR*']},
    u'modules': [u'SECURITY_CONTROL_CORE']}
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/TsensTargetLib/HALhwioTsensFuses.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                          (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_MAXn                                                           71
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_SHFT                                           0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004120)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RSVD0_BMSK                                            0xfffffff0
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RSVD0_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_BMSK                               0xf
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_SHFT                               0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004124)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RSVD1_BMSK                                            0xfffffffe
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RSVD1_SHFT                                                   0x1
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_BMSK                                     0x1
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_SHFT                                     0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004128)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RSVD0_BMSK                                            0xfffffff0
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RSVD0_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_BMSK                               0xf
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_SHFT                               0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000412c)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RSVD0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RSVD0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004130)
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
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
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
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004144)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RSVD0_BMSK                                                 0x80000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RSVD0_SHFT                                                       0x1f
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG30_BMSK                                       0x40000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG30_SHFT                                             0x1e
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG29_BMSK                                       0x20000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG29_SHFT                                             0x1d
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG28_BMSK                                       0x10000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG28_SHFT                                             0x1c
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_BMSK                                        0x8000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_SHFT                                             0x1b
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_BMSK                                        0x4000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_SHFT                                             0x1a
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH1_BMSK                                               0x2000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH1_SHFT                                                    0x19
#define HWIO_QFPROM_CORR_RD_PERM_LSB_IMAGE_ENCR_KEY1_BMSK                                        0x1000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_IMAGE_ENCR_KEY1_SHFT                                             0x18
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_SHFT                                              0x17
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                  0x400000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                      0x16
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x200000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                      0x15
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_SHFT                                                0x14
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                       0x80000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                          0x13
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG18_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG18_SHFT                                              0x12
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_SHFT                                              0x11
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_SHFT                                              0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_SHFT                                               0xf
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_BMSK                                                0x4000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_SHFT                                                   0xe
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_BMSK                                                     0x2000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_SHFT                                                        0xd
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_BMSK                                                  0x1000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_SHFT                                                     0xc
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_BMSK                                                0x800
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_BMSK                                                 0x400
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_SHFT                                                   0xa
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                            0x200
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                              0x9
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                            0x100
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                              0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                             0x80
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                              0x7
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                             0x40
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                              0x6
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_BMSK                                                      0x20
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_SHFT                                                       0x5
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_BMSK                                                     0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_SHFT                                                      0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_BMSK                                                      0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_SHFT                                                      0x3
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_BMSK                                                          0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_SHFT                                                          0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_BMSK                                                      0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_SHFT                                                      0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_BMSK                                               0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_BMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004158)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RSVD0_BMSK                                                 0x80000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RSVD0_SHFT                                                       0x1f
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG30_BMSK                                       0x40000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG30_SHFT                                             0x1e
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG29_BMSK                                       0x20000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG29_SHFT                                             0x1d
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG28_BMSK                                       0x10000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG28_SHFT                                             0x1c
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_BMSK                                        0x8000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_SHFT                                             0x1b
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_BMSK                                        0x4000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_SHFT                                             0x1a
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH1_BMSK                                               0x2000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH1_SHFT                                                    0x19
#define HWIO_QFPROM_CORR_WR_PERM_LSB_IMAGE_ENCR_KEY1_BMSK                                        0x1000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_IMAGE_ENCR_KEY1_SHFT                                             0x18
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                              0x17
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                  0x400000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                      0x16
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x200000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                      0x15
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                0x14
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                       0x80000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                          0x13
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG18_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG18_SHFT                                              0x12
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_SHFT                                              0x11
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_SHFT                                              0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_SHFT                                               0xf
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_BMSK                                                0x4000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_SHFT                                                   0xe
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_BMSK                                                     0x2000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_SHFT                                                        0xd
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_BMSK                                                  0x1000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_SHFT                                                     0xc
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                0x800
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_BMSK                                                 0x400
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_SHFT                                                   0xa
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                            0x200
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                              0x9
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                            0x100
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                              0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                             0x80
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                              0x7
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                             0x40
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                              0x6
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_BMSK                                                      0x20
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_SHFT                                                       0x5
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_BMSK                                                     0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_SHFT                                                      0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_BMSK                                                      0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_SHFT                                                      0x3
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_BMSK                                                          0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_SHFT                                                          0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_BMSK                                                      0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_SHFT                                                      0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                               0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000415c)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_BMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004160)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_BMSK                                                  0x80000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_SHFT                                                        0x1f
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                              0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                              0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                              0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                              0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                              0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                              0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                              0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                              0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                              0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                              0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                               0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                               0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                               0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                            0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                               0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                             0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                               0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                             0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                               0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                              0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                              0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                               0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                               0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                               0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                               0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                0x0

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004164)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004168)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000416c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004170)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_SHFT                                   0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004174)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_XBL_SEC_BMSK                                       0xfe000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_XBL_SEC_SHFT                                             0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                            0x1fe0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_BMSK                                               0x1ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004178)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD1_BMSK                                         0xc0000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD1_SHFT                                               0x1e
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_BMSK                               0x3e000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_SHFT                                     0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_BMSK                           0x1000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_SHFT                                0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_BMSK                             0xffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_SHFT                                  0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_SAFESWITCH_BMSK                                          0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_SAFESWITCH_SHFT                                           0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000417c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RSVD1_BMSK                                         0xf0000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RSVD1_SHFT                                               0x1c
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_BMSK                                     0xffe0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_SHFT                                          0x11
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_BMSK                                     0x1f000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_SHFT                                         0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_BMSK                                         0xfff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_SHFT                                           0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004180)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_BMSK                                           0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MBA_BMSK                                               0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MBA_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004184)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_SIMLOCK_BMSK                                       0x80000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_SIMLOCK_SHFT                                             0x1f
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_BMSK                                         0x7fffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DCC_DEBUG_DISABLE_BMSK                             0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DCC_DEBUG_DISABLE_SHFT                                   0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_APB_DFD_DISABLE_BMSK                          0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_APB_DFD_DISABLE_SHFT                                0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                             0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                   0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_BMSK                          0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_SHFT                                0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SP_DISABLE_BMSK                                     0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SP_DISABLE_SHFT                                          0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD2_BMSK                                          0x6000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD2_SHFT                                               0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_ENABLE_BMSK                0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_ENABLE_SHFT                     0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK               0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                   0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_BMSK                                  0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_SHFT                                      0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_BMSK                           0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_SHFT                               0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_BMSK                            0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_SHFT                                0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                          0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                             0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                              0x60000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                 0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_USB_TYPE_C_DISABLE_BMSK                           0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_USB_TYPE_C_DISABLE_SHFT                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                   0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                      0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                           0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                              0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                  0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                     0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPI_CLK_BOOT_FREQ_BMSK                                  0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPI_CLK_BOOT_FREQ_SHFT                                    0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                              0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                                0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                          0x3e0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                            0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                               0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                               0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                  0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                         0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LLCC_DSRW_DISABLE_BMSK                             0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LLCC_DSRW_DISABLE_SHFT                                   0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_BMSK                                0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_SHFT                                      0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                            0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                  0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                           0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                 0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_BMSK                                 0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_SHFT                                      0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_BMSK                                 0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_SHFT                                      0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_BMSK                                 0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_SHFT                                      0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                            0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                 0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                            0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                               0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                   0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_BMSK                               0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_SHFT                                   0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CAM_ICP_NIDEN_DISABLE_BMSK                           0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CAM_ICP_NIDEN_DISABLE_SHFT                               0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_NIDEN_DISABLE_BMSK                           0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_NIDEN_DISABLE_SHFT                              0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_BMSK                               0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_SHFT                                  0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_TURING_NIDEN_DISABLE_BMSK                       0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_TURING_NIDEN_DISABLE_SHFT                          0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                   0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                   0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                                 0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                    0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_BMSK                            0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_SHFT                               0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_BMSK                                     0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_SHFT                                       0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_BMSK                                  0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_SHFT                                    0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CAM_ICP_DBGEN_DISABLE_BMSK                              0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CAM_ICP_DBGEN_DISABLE_SHFT                                0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_DBGEN_DISABLE_BMSK                             0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_DBGEN_DISABLE_SHFT                               0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                                  0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                   0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_TURING_DBGEN_DISABLE_BMSK                          0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_TURING_DBGEN_DISABLE_SHFT                           0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                   0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                    0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                  0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                   0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_EUD_DISABLE_BMSK                                          0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_EUD_DISABLE_SHFT                                          0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_DFD_DISABLE_BMSK                                 0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_AOSS_AOP_DFD_DISABLE_SHFT                                 0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DEBUG_BUS_DISABLE_BMSK                                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DEBUG_BUS_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD1_BMSK                                         0xffff8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD1_SHFT                                                0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_BMSK                            0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_SHFT                               0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_BMSK                            0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_SHFT                               0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG28_SECURE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG28_SECURE_SHFT                               0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_BMSK                             0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_SHFT                               0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_BMSK                             0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_SHFT                               0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_BMSK                                              0x3ff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                      0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_BMSK                                         0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_SHFT                                            0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004198)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_BMSK                                    0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_SHFT                                          0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_BMSK                                        0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_SHFT                                           0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000419c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_BMSK                                         0xffffff00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_SHFT                                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                            0xff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                             0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_1_0_BMSK                      0x180000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_1_0_SHFT                          0x13
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_WARLOCK_AR50_FUSE_BMSK                             0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_WARLOCK_AR50_FUSE_SHFT                                  0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_BMSK                                   0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_SHFT                                        0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_BMSK                0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_SHFT                     0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_BMSK                    0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_SHFT                         0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_CAM_8BPP_IF_BMSK                              0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_CAM_8BPP_IF_SHFT                                  0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_WRLK_EN_FUSE_BMSK                                   0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_WRLK_EN_FUSE_SHFT                                       0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_BMSK                               0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_SHFT                                   0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_RESOLUTION_LIMIT_2_BMSK                         0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_RESOLUTION_LIMIT_2_SHFT                            0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_BMSK                               0x7c000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_SHFT                                   0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_BMSK                            0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_SHFT                               0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_BMSK                               0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_SHFT                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_BMSK                                        0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_SHFT                                          0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_FORCE_HW_KEY0_BMSK                            0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_FORCE_HW_KEY0_SHFT                              0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_DISABLE_BMSK                                  0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_DISABLE_SHFT                                    0x9
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SYS_CFG_APC0PLL_LVAL_2_0_BMSK                        0x38000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SYS_CFG_APC0PLL_LVAL_2_0_SHFT                            0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_BMSK                      0x7fe0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_SHFT                         0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_BMSK                        0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_SHFT                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_CFG_APC0PLL_LVAL_7_3_BMSK                       0x1f0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SYS_CFG_APC0PLL_LVAL_7_3_SHFT                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                               0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_DOLBY_BIT_BMSK                                        0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_DOLBY_BIT_SHFT                                           0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SSC_SDC_CCD_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SSC_SDC_CCD_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_NQ_WRLK_RD_FUSE_BMSK                                   0xc00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_NQ_WRLK_RD_FUSE_SHFT                                     0xa
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_6_BMSK                    0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_6_SHFT                       0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_BMSK                               0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_SHFT                                  0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_5_BMSK                    0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_5_SHFT                       0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_BMSK                               0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_4_BMSK                     0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_EFUSE_TURING_Q6SS_PLL_L_MAX_4_SHFT                       0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_BMSK                                 0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_SHFT                                   0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_BMSK                              0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_SHFT                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_BMSK                                       0xff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_SHFT                                        0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c4)
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EMMC_ICE_FORCE_HW_KEY1_BMSK                        0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EMMC_ICE_FORCE_HW_KEY1_SHFT                             0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EFUSE_TURING_Q6SS_PLL_L_MAX_7_BMSK                 0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EFUSE_TURING_Q6SS_PLL_L_MAX_7_SHFT                      0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PERIPH_DRV_STRENGTH_SETTING_BMSK                   0x3800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PERIPH_DRV_STRENGTH_SETTING_SHFT                        0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EFUSE_TURING_Q6SS_PLL_L_MAX_3_0_BMSK                0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_EFUSE_TURING_Q6SS_PLL_L_MAX_3_0_SHFT                    0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_BMSK                                      0x78000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_SHFT                                          0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_BMSK                                          0x7ff0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_SHFT                                             0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_BMSK                                   0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_SHFT                                   0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041cc + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e8)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT1_HSTX_TRIM_LSB_1_0_BMSK                       0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT1_HSTX_TRIM_LSB_1_0_SHFT                             0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HS_REFCLK_SEL_BMSK                           0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HS_REFCLK_SEL_SHFT                                 0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_BMSK                           0x1e000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_SHFT                                 0x19
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SPARE0_BMSK                                              0x1ffe000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SPARE0_SHFT                                                    0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_HVX_LDO_ENABLE_BMSK                             0x1000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_HVX_LDO_ENABLE_SHFT                                0xc
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_LDO_VREF_TRIM_BMSK                               0xf80
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_LDO_VREF_TRIM_SHFT                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_LDO_ENABLE_BMSK                                   0x40
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_LPASS_Q6SS1_LDO_ENABLE_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_MSS_Q6SS0_LDO_VREF_TRIM_BMSK                                  0x3e
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_MSS_Q6SS0_LDO_VREF_TRIM_SHFT                                   0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_MSS_Q6SS0_LDO_ENABLE_BMSK                                      0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_MSS_Q6SS0_LDO_ENABLE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ec)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE1_BMSK                                             0xfffffff8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE1_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_QUSB_PORT1_HS_REFCLK_SEL_BMSK                                  0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_QUSB_PORT1_HS_REFCLK_SEL_SHFT                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_QUSB_PORT1_HSTX_TRIM_LSB_3_2_BMSK                              0x3
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_QUSB_PORT1_HSTX_TRIM_LSB_3_2_SHFT                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE2_BMSK                                             0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE2_SHFT                                                   0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_REFGEN_NORTH_BGV_TRIM_BMSK                              0x7f800000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_REFGEN_NORTH_BGV_TRIM_SHFT                                    0x17
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_REFGEN_SOUTH_BGV_TRIM_BMSK                                0x7f8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_REFGEN_SOUTH_BGV_TRIM_SHFT                                     0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_BANDGAP_TRIM_BMSK                                           0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_BANDGAP_TRIM_SHFT                                              0x9
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE1_BMSK                                                  0x1ff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE1_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BMSK                          0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_SHFT                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_NOM_BMSK                           0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_NOM_SHFT                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BMSK                            0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_TUR_SHFT                                0x14
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_BMSK                                   0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_SHFT                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_BMSK                                     0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_SHFT                                        0xa
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_BMSK                                      0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_SHFT                                        0x5
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_BMSK                                       0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_BMSK                                0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_SHFT                                      0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_BMSK                                  0x7c00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_SHFT                                       0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_NOM_BMSK                                   0x3e0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_NOM_SHFT                                       0x11
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_TUR_BMSK                                    0x1f000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_TUR_SHFT                                        0xc
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR0_AGING_BMSK                                              0xff0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR0_AGING_SHFT                                                0x4
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR0_TARG_VOLT_OFFSET_SVS2_BMSK                                0xf
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR0_TARG_VOLT_OFFSET_SVS2_SHFT                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR2_TARG_VOLT_NOM_2_0_BMSK                             0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR2_TARG_VOLT_NOM_2_0_SHFT                                   0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR2_TARG_VOLT_TUR_BMSK                                 0x1f000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR2_TARG_VOLT_TUR_SHFT                                       0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_AGING_BMSK                                           0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_AGING_SHFT                                               0x10
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                             0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                                0xc
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_SVS_BMSK                               0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_SVS_SHFT                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_NOM_BMSK                                0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_NOM_SHFT                                 0x4
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_TUR_BMSK                                 0xf
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR1_TARG_VOLT_OFFSET_TUR_SHFT                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_AGING_3_0_BMSK                                     0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_AGING_3_0_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_SVS2_BMSK                          0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_SVS2_SHFT                               0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_SVS_BMSK                            0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_SVS_SHFT                                0x14
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_NOM_BMSK                             0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_NOM_SHFT                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_BMSK                              0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_SHFT                                 0xc
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_SVS2_BMSK                                     0xf80
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_SVS2_SHFT                                       0x7
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_SVS_BMSK                                       0x7c
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_SVS_SHFT                                        0x2
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_NOM_4_3_BMSK                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_NOM_4_3_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR4_TARG_VOLT_NOM_2_0_BMSK                             0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR4_TARG_VOLT_NOM_2_0_SHFT                                   0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR4_TARG_VOLT_TUR_BMSK                                 0x1f000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR4_TARG_VOLT_TUR_SHFT                                       0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_SVS2_BMSK                                  0xf80000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_SVS2_SHFT                                      0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_SVS_BMSK                                    0x7c000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_SVS_SHFT                                        0xe
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_TUR_BMSK                                     0x3e00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_TUR_SHFT                                        0x9
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_NOM_BMSK                                      0x1f0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR3_TARG_VOLT_NOM_SHFT                                        0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR2_AGING_7_4_BMSK                                            0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR2_AGING_7_4_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_SVS_BMSK                                 0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_SVS_SHFT                                       0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_TUR_BMSK                                  0x7c00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_TUR_SHFT                                       0x16
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_NOM_BMSK                                   0x3e0000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR5_TARG_VOLT_NOM_SHFT                                       0x11
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_OFFSET_NOM_BMSK                             0x1f000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_OFFSET_NOM_SHFT                                 0xc
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_OFFSET_TUR_BMSK                               0xf80
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_OFFSET_TUR_SHFT                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_SVS_BMSK                                       0x7c
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_SVS_SHFT                                        0x2
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_NOM_4_3_BMSK                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR4_TARG_VOLT_NOM_4_3_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS_1_0_BMSK                  0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS_1_0_SHFT                        0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_LDO_TARG_VOLT_OFFSET_SVS2_BMSK                     0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_LDO_TARG_VOLT_OFFSET_SVS2_SHFT                           0x19
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_LDO_TARG_VOLT_OFFSET_SVS_BMSK                       0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_LDO_TARG_VOLT_OFFSET_SVS_SHFT                            0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_SVS2_BMSK                                   0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_SVS2_SHFT                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_SVS_BMSK                                     0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_SVS_SHFT                                        0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_NOM_BMSK                                      0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_NOM_SHFT                                        0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_TUR_BMSK                                       0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TARG_VOLT_TUR_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TARG_VOLT_NOM_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TARG_VOLT_NOM_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TARG_VOLT_TUR_BMSK                                0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TARG_VOLT_TUR_SHFT                                      0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_SVS2_ROSEL_BMSK                                     0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_SVS2_ROSEL_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_SVS_ROSEL_BMSK                                       0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_SVS_ROSEL_SHFT                                          0x10
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_NOMINAL_ROSEL_BMSK                                    0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_NOMINAL_ROSEL_SHFT                                       0xc
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TURBO_ROSEL_BMSK                                       0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR10_TURBO_ROSEL_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS2_BMSK                           0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS2_SHFT                            0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS_4_2_BMSK                         0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR8_LDO_TARG_VOLT_OFFSET_SVS_4_2_SHFT                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_NOMINAL_QUOT_VMIN_3_0_BMSK                        0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_NOMINAL_QUOT_VMIN_3_0_SHFT                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TURBO_QUOT_VMIN_BMSK                               0xfff0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TURBO_QUOT_VMIN_SHFT                                    0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_SVS2_BMSK                                   0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_SVS2_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_SVS_BMSK                                     0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_SVS_SHFT                                       0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_NOM_5_2_BMSK                                   0xf
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR10_TARG_VOLT_NOM_5_2_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_SVS2_QUOT_VMIN_BMSK                               0xfff00000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_SVS2_QUOT_VMIN_SHFT                                     0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_SVS_QUOT_VMIN_BMSK                                   0xfff00
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_SVS_QUOT_VMIN_SHFT                                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_NOMINAL_QUOT_VMIN_11_4_BMSK                             0xff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR10_NOMINAL_QUOT_VMIN_11_4_SHFT                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR11_TARG_VOLT_TUR_2_0_BMSK                            0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR11_TARG_VOLT_TUR_2_0_SHFT                                  0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_AGING_BMSK                                        0x1fe00000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_AGING_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_SVS_BMSK                                0x1fc000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_SVS_SHFT                                     0xe
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_NOM_BMSK                                  0x3f80
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_NOM_SHFT                                     0x7
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_TUR_BMSK                                    0x7f
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR10_QUOT_OFFSET_TUR_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TURBO_QUOT_VMIN_10_0_BMSK                         0xffe00000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TURBO_QUOT_VMIN_10_0_SHFT                               0x15
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_SVS2_BMSK                                 0x1f8000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_SVS2_SHFT                                      0xf
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_SVS_BMSK                                    0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_SVS_SHFT                                       0x9
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_NOM_BMSK                                     0x1f8
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_NOM_SHFT                                       0x3
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_TUR_5_3_BMSK                                   0x7
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR11_TARG_VOLT_TUR_5_3_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_SVS2_QUOT_VMIN_6_0_BMSK                           0xfe000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_SVS2_QUOT_VMIN_6_0_SHFT                                 0x19
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_SVS_QUOT_VMIN_BMSK                                 0x1ffe000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_SVS_QUOT_VMIN_SHFT                                       0xd
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_NOMINAL_QUOT_VMIN_BMSK                                0x1ffe
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_NOMINAL_QUOT_VMIN_SHFT                                   0x1
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_TURBO_QUOT_VMIN_11_BMSK                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR11_TURBO_QUOT_VMIN_11_SHFT                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR12_TURBO_ROSEL_1_0_BMSK                              0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR12_TURBO_ROSEL_1_0_SHFT                                    0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR12_BOOST_ROSEL_BMSK                                  0x3c000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR12_BOOST_ROSEL_SHFT                                        0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_SVS_BMSK                               0x3f80000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_SVS_SHFT                                    0x13
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_NOM_BMSK                                 0x7f000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_NOM_SHFT                                     0xc
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_TUR_BMSK                                   0xfe0
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_QUOT_OFFSET_TUR_SHFT                                     0x5
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_SVS2_QUOT_VMIN_11_7_BMSK                                0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR11_SVS2_QUOT_VMIN_11_7_SHFT                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_SVS_3_0_BMSK                            0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_SVS_3_0_SHFT                                  0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_NOM_BMSK                                 0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_NOM_SHFT                                      0x16
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_TUR_BMSK                                  0x3f0000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_TUR_SHFT                                      0x10
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_BOOST_BMSK                                  0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TARG_VOLT_BOOST_SHFT                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_SVS_ROSEL_BMSK                                         0x3c0
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_SVS_ROSEL_SHFT                                           0x6
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_NOMINAL_ROSEL_BMSK                                      0x3c
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_NOMINAL_ROSEL_SHFT                                       0x2
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TURBO_ROSEL_3_2_BMSK                                     0x3
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR12_TURBO_ROSEL_3_2_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_NOMINAL_QUOT_VMIN_5_0_BMSK                        0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_NOMINAL_QUOT_VMIN_5_0_SHFT                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_TURBO_QUOT_VMIN_BMSK                               0x3ffc000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_TURBO_QUOT_VMIN_SHFT                                     0xe
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_BOOST_QUOT_VMIN_BMSK                                  0x3ffc
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_BOOST_QUOT_VMIN_SHFT                                     0x2
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_TARG_VOLT_SVS_5_4_BMSK                                   0x3
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR12_TARG_VOLT_SVS_5_4_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_QUOT_OFFSET_TUR_BMSK                             0xfe000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_QUOT_OFFSET_TUR_SHFT                                   0x19
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_QUOT_OFFSET_BOOST_BMSK                            0x1fc0000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_QUOT_OFFSET_BOOST_SHFT                                 0x12
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_SVS_QUOT_VMIN_BMSK                                  0x3ffc0
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_SVS_QUOT_VMIN_SHFT                                      0x6
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_NOMINAL_QUOT_VMIN_11_6_BMSK                            0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR12_NOMINAL_QUOT_VMIN_11_6_SHFT                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_LDO_RC_1_0_BMSK                                    0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_LDO_RC_1_0_SHFT                                          0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_SSC_CX_MODE_DISABLE_BMSK                           0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_SSC_CX_MODE_DISABLE_SHFT                                 0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_DDR_MODE_DISABLE_BMSK                               0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_DDR_MODE_DISABLE_SHFT                                    0x18
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_GFX_MODE_DISABLE_BMSK                                0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_GFX_MODE_DISABLE_SHFT                                    0x15
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_MSS_MODE_DISABLE_BMSK                                0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_MSS_MODE_DISABLE_SHFT                                    0x12
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_CX_MODE_DISABLE_BMSK                                  0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_CX_MODE_DISABLE_SHFT                                      0xf
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR12_AGING_BMSK                                           0x7f80
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR12_AGING_SHFT                                              0x7
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR12_QUOT_OFFSET_NOM_BMSK                                   0x7f
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR12_QUOT_OFFSET_NOM_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR1_TARG_VOLT_OFFSET_SVSP_0_BMSK                      0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR1_TARG_VOLT_OFFSET_SVSP_0_SHFT                            0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR0_TARG_VOLT_OFFSET_SVSP_BMSK                        0x78000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR0_TARG_VOLT_OFFSET_SVSP_SHFT                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR_RESERVED_BMSK                                       0x7fffffe
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR_RESERVED_SHFT                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR_LDO_RC_2_BMSK                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_CPR_LDO_RC_2_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RSVD0_BMSK                                             0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RSVD0_SHFT                                                   0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR9_TARG_VOLT_NOM_BMSK                                 0xf800000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR9_TARG_VOLT_NOM_SHFT                                      0x17
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_AON_AGING_BMSK                                           0x7f8000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_AON_AGING_SHFT                                                0xf
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_NOMP_BMSK                            0x7800
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_NOMP_SHFT                               0xb
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_SUT_BMSK                              0x780
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_SUT_SHFT                                0x7
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_SVSP_BMSK                              0x78
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR2_TARG_VOLT_OFFSET_SVSP_SHFT                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR1_TARG_VOLT_OFFSET_SVSP_3_1_BMSK                           0x7
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_CPR1_TARG_VOLT_OFFSET_SVSP_3_1_SHFT                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW12_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ISENSE_PMOS_COMP_BMSK                                  0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ISENSE_PMOS_COMP_SHFT                                        0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SPARE1_BMSK                                             0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SPARE1_SHFT                                                  0x19
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SRAM_AGING_SENSOR_1_BMSK                                0x1fe0000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SRAM_AGING_SENSOR_1_SHFT                                     0x11
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SRAM_AGING_SENSOR_0_BMSK                                  0x1fe00
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SRAM_AGING_SENSOR_0_SHFT                                      0x9
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SPARE0_BMSK                                                 0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_SPARE0_SHFT                                                   0x6
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_CPR_LOCAL_RC_BMSK                                            0x38
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_CPR_LOCAL_RC_SHFT                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_CPR_GLOBAL_RC_BMSK                                            0x7
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_CPR_GLOBAL_RC_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW12_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS1_BASE1_1_0_BMSK                                  0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS1_BASE1_1_0_SHFT                                        0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS1_BASE0_BMSK                                      0x3ff00000
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS1_BASE0_SHFT                                            0x14
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS0_BASE1_BMSK                                         0xffc00
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS0_BASE1_SHFT                                             0xa
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS0_BASE0_BMSK                                           0x3ff
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TSENS0_BASE0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004250)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW13_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS5_OFFSET_BMSK                                     0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS5_OFFSET_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS4_OFFSET_BMSK                                      0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS4_OFFSET_SHFT                                           0x18
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS3_OFFSET_BMSK                                       0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS3_OFFSET_SHFT                                           0x14
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS2_OFFSET_BMSK                                        0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS2_OFFSET_SHFT                                           0x10
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS1_OFFSET_BMSK                                         0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS1_OFFSET_SHFT                                            0xc
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS0_OFFSET_BMSK                                          0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS0_OFFSET_SHFT                                            0x8
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS1_BASE1_9_2_BMSK                                        0xff
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_TSENS1_BASE1_9_2_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004254)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW13_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS13_OFFSET_BMSK                                    0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS13_OFFSET_SHFT                                          0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS12_OFFSET_BMSK                                     0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS12_OFFSET_SHFT                                          0x18
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS11_OFFSET_BMSK                                      0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS11_OFFSET_SHFT                                          0x14
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS10_OFFSET_BMSK                                       0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS10_OFFSET_SHFT                                          0x10
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS9_OFFSET_BMSK                                         0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS9_OFFSET_SHFT                                            0xc
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS8_OFFSET_BMSK                                          0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS8_OFFSET_SHFT                                            0x8
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS7_OFFSET_BMSK                                           0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS7_OFFSET_SHFT                                            0x4
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS6_OFFSET_BMSK                                            0xf
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_TSENS6_OFFSET_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004258)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW14_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS_CAL_SEL_BMSK                                     0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS_CAL_SEL_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS20_OFFSET_BMSK                                     0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS20_OFFSET_SHFT                                          0x18
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS19_OFFSET_BMSK                                      0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS19_OFFSET_SHFT                                          0x14
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS18_OFFSET_BMSK                                       0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS18_OFFSET_SHFT                                          0x10
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS17_OFFSET_BMSK                                        0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS17_OFFSET_SHFT                                           0xc
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS16_OFFSET_BMSK                                         0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS16_OFFSET_SHFT                                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS15_OFFSET_BMSK                                          0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS15_OFFSET_SHFT                                           0x4
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS14_OFFSET_BMSK                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_TSENS14_OFFSET_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000425c)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW14_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_SPARE1_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004260)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW15_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S5_BMSK                                   0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S5_SHFT                                         0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S4_BMSK                                   0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S4_SHFT                                         0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S3_BMSK                                    0x3800000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S3_SHFT                                         0x17
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S2_BMSK                                     0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S2_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S1_BMSK                                      0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S1_SHFT                                         0x11
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S0_BMSK                                      0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_S0_SHFT                                          0xe
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE2_BMSK                                          0x3f00
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE2_SHFT                                             0x8
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_BMSK                                            0xff
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_VSENSE_FUSE1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004264)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW15_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S6_BMSK                                   0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S6_SHFT                                         0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S5_BMSK                                   0x30000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S5_SHFT                                         0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S4_BMSK                                    0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S4_SHFT                                         0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S3_BMSK                                    0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S3_SHFT                                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S2_BMSK                                     0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S2_SHFT                                         0x16
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S1_BMSK                                     0x300000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S1_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S0_BMSK                                      0xc0000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE2_S0_SHFT                                         0x12
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S11_BMSK                                     0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S11_SHFT                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S10_BMSK                                      0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S10_SHFT                                         0xc
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S9_BMSK                                        0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S9_SHFT                                          0x9
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S8_BMSK                                        0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S8_SHFT                                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S7_BMSK                                         0x38
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S7_SHFT                                          0x3
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S6_BMSK                                          0x7
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_VSENSE_FUSE1_S6_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004268)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW16_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_RESERVED_21_0_BMSK                              0xfffffc00
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_RESERVED_21_0_SHFT                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S11_BMSK                                       0x300
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S11_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S10_BMSK                                        0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S10_SHFT                                         0x6
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S9_BMSK                                         0x30
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S9_SHFT                                          0x4
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S8_BMSK                                          0xc
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S8_SHFT                                          0x2
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S7_BMSK                                          0x3
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_VSENSE_FUSE2_S7_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000426c)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW16_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_SPARE0_BMSK                                            0xffffff00
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_SPARE0_SHFT                                                   0x8
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_VSENSE_RESERVED_29_22_BMSK                                   0xff
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_VSENSE_RESERVED_29_22_SHFT                                    0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004270 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                       18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                           0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004274 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                       18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004308)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_PHY_DISABLE_UPPER_LANE_BMSK               0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_PHY_DISABLE_UPPER_LANE_SHFT                     0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                   0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                         0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_QC_SP_DISABLE_BMSK                             0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_QC_SP_DISABLE_SHFT                                   0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_GFX3D_FREQ_LIMIT_VAL_BMSK                      0x1fe00000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_GFX3D_FREQ_LIMIT_VAL_SHFT                            0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_1_0_BMSK                   0x180000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_MDSS_RESOLUTION_LIMIT_1_0_SHFT                       0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_BMSK                    0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_SHFT                       0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_BMSK                    0x20000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_SHFT                       0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_BMSK                             0x10000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_SHFT                                0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_CORE1_BMSK                           0x8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_CORE1_SHFT                              0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_VPX_BMSK                             0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_VPX_SHFT                                0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DP_DISABLE_BMSK                                    0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DP_DISABLE_SHFT                                       0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                  0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                     0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_BMSK                         0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_SHFT                           0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDCP_GLOBAL_KEY_SPLIT2_DISABLE_BMSK                 0x400
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDCP_GLOBAL_KEY_SPLIT2_DISABLE_SHFT                   0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                  0x200
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                    0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                  0x100
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                    0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_BMSK                                      0x80
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_SHFT                                       0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_BMSK                            0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_SHFT                             0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CAM_IPE_1_DISABLE_IF_BMSK                            0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CAM_IPE_1_DISABLE_IF_SHFT                             0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EUD_PORT1_SEL_BMSK                                   0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EUD_PORT1_SEL_SHFT                                    0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EUD_IGNR_CSR_BMSK                                     0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EUD_IGNR_CSR_SHFT                                     0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                           0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                           0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000430c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY1_BMSK                         0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY1_SHFT                               0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY0_BMSK                         0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_FORCE_HW_KEY0_SHFT                               0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_DISABLE_BMSK                               0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ICE_DISABLE_SHFT                                     0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_LLCC_FUSE_CACHE_SIZE_ZERO_BMSK                 0x10000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_LLCC_FUSE_CACHE_SIZE_ZERO_SHFT                       0x1c
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_WARLOCK_AR50_FUSE_BMSK                          0x8000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_WARLOCK_AR50_FUSE_SHFT                               0x1b
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_BMSK                                0x4000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_DISABLE_SHFT                                     0x1a
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_BMSK             0x2000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_ISLAND_MODE_Q6_CLK_DISABLE_SHFT                  0x19
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_BMSK                 0x1000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SSC_SW_ISLAND_MODE_DISABLE_SHFT                      0x18
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EFUSE_CAM_8BPP_IF_BMSK                           0x800000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EFUSE_CAM_8BPP_IF_SHFT                               0x17
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_BMSK                           0x400000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_SHFT                               0x16
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_WLRK_EN_FUSE_BMSK                                0x200000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_WLRK_EN_FUSE_SHFT                                    0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_BMSK                            0x100000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APB2JTAG_DISABLE_SHFT                                0x14
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MDSS_RESOLUTION_LIMIT_2_BMSK                      0x80000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MDSS_RESOLUTION_LIMIT_2_SHFT                         0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_BMSK                            0x7c000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_SHFT                                0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_BMSK                         0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_SHFT                            0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MDSS_Q_CONFIG_FUSE_SHFT                               0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_BMSK                                     0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_SHFT                                       0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_FORCE_HW_KEY0_BMSK                         0x400
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_FORCE_HW_KEY0_SHFT                           0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_DISABLE_BMSK                               0x200
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_EMMC_ICE_DISABLE_SHFT                                 0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                    0x1e0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                      0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                          0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                           0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_VFE_RESOLUTION_LIMIT_BMSK                             0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_VFE_RESOLUTION_LIMIT_SHFT                             0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004310)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MDSP_FW_DISABLE_BMSK                           0xfff00000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MDSP_FW_DISABLE_SHFT                                 0x14
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_TCM_BOOT_DISABLE_BMSK                       0x80000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_TCM_BOOT_DISABLE_SHFT                          0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_NAV_DISABLE_BMSK                                  0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_NAV_DISABLE_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SYS_CFG_APC0PLL_LVAL_2_0_BMSK                     0x38000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SYS_CFG_APC0PLL_LVAL_2_0_SHFT                         0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_BMSK                   0x7fe0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SPARE_SHFT                      0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_BMSK                     0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_MODEM_FEATURE_DISABLE_SLICE_SHFT                      0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004314)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_APSS_PDXWRPNTRDELAY_BMSK                       0xc0000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_APSS_PDXWRPNTRDELAY_SHFT                             0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_APCSCFGAPMBOOTONMX_BMSK                    0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_APCSCFGAPMBOOTONMX_SHFT                          0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_APCCCFGCPUPRESENT_N_BMSK                   0x1fe00000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_APCCCFGCPUPRESENT_N_SHFT                         0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_CFG_APC0PLL_LVAL_7_3_BMSK                    0x1f0000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SYS_CFG_APC0PLL_LVAL_7_3_SHFT                        0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_BMSK                              0xffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_MODEM_VU_DISABLE_SHFT                                 0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004318)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                   0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                         0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_MSS_NIDEN_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_MSS_NIDEN_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SSC_NIDEN_DISABLE_BMSK                      0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SSC_NIDEN_DISABLE_SHFT                            0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_NIDEN_DISABLE_BMSK                  0x10000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_NIDEN_DISABLE_SHFT                        0x1c
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_NIDEN_DISABLE_BMSK                  0x8000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_NIDEN_DISABLE_SHFT                       0x1b
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_WCSS_NIDEN_DISABLE_BMSK                      0x4000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_WCSS_NIDEN_DISABLE_SHFT                           0x1a
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_NIDEN_DISABLE_BMSK              0x2000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_NIDEN_DISABLE_SHFT                   0x19
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_BMSK                       0x1000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_SHFT                            0x18
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_NIDEN_DISABLE_BMSK                       0x800000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_NIDEN_DISABLE_SHFT                           0x17
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_BMSK                        0x400000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_SHFT                            0x16
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                   0x200000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                       0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                    0x100000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                        0x14
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_BMSK                            0x80000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_SHFT                               0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_DBGEN_DISABLE_BMSK                     0x20000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_CAM_ICP_DBGEN_DISABLE_SHFT                        0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DBGEN_DISABLE_BMSK                    0x10000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DBGEN_DISABLE_SHFT                       0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                         0x8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                            0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_DBGEN_DISABLE_BMSK                 0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_LPASS_TURING_DBGEN_DISABLE_SHFT                    0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_DBGEN_DISABLE_BMSK                         0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_DBGEN_DISABLE_SHFT                            0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                        0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                          0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_EUD_DISABLE_BMSK                                 0x400
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_EUD_DISABLE_SHFT                                   0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DFD_DISABLE_BMSK                        0x200
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_AOSS_AOP_DFD_DISABLE_SHFT                          0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DEBUG_BUS_DISABLE_BMSK                           0x100
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DEBUG_BUS_DISABLE_SHFT                             0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DCC_DEBUG_DISABLE_BMSK                            0x80
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_DCC_DEBUG_DISABLE_SHFT                             0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_APB_DFD_DISABLE_BMSK                         0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_APPS_APB_DFD_DISABLE_SHFT                          0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_NIDEN_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_NIDEN_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_DBGEN_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_DBGEN_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_DAPEN_DISABLE_BMSK                             0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QC_GPU_DAPEN_DISABLE_SHFT                             0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_BMSK                                 0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_SHFT                                 0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_BMSK                                  0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_SHFT                                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000431c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SYS_CFG_APC1PLL_LVAL_BMSK                      0xff000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SYS_CFG_APC1PLL_LVAL_SHFT                            0x18
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                       0x800000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                           0x17
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SYS_CFG_L3_SIZE_RED_BMSK                         0x400000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SYS_CFG_L3_SIZE_RED_SHFT                             0x16
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_AUTO_CCI_RCG_CFG_DISABLE_BMSK                    0x200000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_AUTO_CCI_RCG_CFG_DISABLE_SHFT                        0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_FUSE_BMSK                          0x1f8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_FUSE_SHFT                               0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                         0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                            0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_DOLBY_BIT_BMSK                                     0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_DOLBY_BIT_SHFT                                        0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SSC_SDC_CCD_DISABLE_BMSK                           0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SSC_SDC_CCD_DISABLE_SHFT                              0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_NQ_WRLK_RD_FUSE_BMSK                                0xc00
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_NQ_WRLK_RD_FUSE_SHFT                                  0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_A5X_ISDB_SECURE_DBGEN_DISABLE_BMSK               0x200
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_A5X_ISDB_SECURE_DBGEN_DISABLE_SHFT                 0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE0_DISABLE_BMSK                              0x100
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE0_DISABLE_SHFT                                0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_LLCC_DSRW_DISABLE_BMSK                            0x80
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_LLCC_DSRW_DISABLE_SHFT                             0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE5_DISABLE_BMSK                               0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE5_DISABLE_SHFT                                0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_BMSK                           0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_APPS_SPIDEN_DISABLE_BMSK                          0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_APPS_SPIDEN_DISABLE_SHFT                           0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE4_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE4_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE3_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE3_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE2_DISABLE_BMSK                                0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_SPARE2_DISABLE_SHFT                                0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_BMSK                           0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_SHFT                           0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042a0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_MAXn                                                         18
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042a4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_MAXn                                                         18
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004338 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                                1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000433c + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                         0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                                1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                               0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004348)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                 0xffff0000
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000434c)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                         0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                   0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004350 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                      1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                   0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                         0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                     0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                         0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                       0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                          0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                         0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004354 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                      1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                     0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                         0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                       0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                          0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                         0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004360 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                            3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004364 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                     0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                            3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                           0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004380)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004384)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                     0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                               0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004388 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                            3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000438c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                     0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                            3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                           0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a8)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043ac)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                     0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                               0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                        55
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                        55
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_BMSK                                            0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_SHFT                                                0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                        0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                            0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                              0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                              0x0

#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004570 + 0x8 * (n))
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_MAXn                                                   1
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_KEY_DATA0_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_LSB_KEY_DATA0_SHFT                                       0x0

#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004574 + 0x8 * (n))
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_RMSK                                            0xffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_MAXn                                                   1
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_KEY_DATA1_BMSK                                  0xffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROWn_MSB_KEY_DATA1_SHFT                                       0x0

#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004580)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_RSVD0_BMSK                                    0xffff0000
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_RSVD0_SHFT                                          0x10
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_KEY_DATA0_BMSK                                    0xffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_LSB_KEY_DATA0_SHFT                                       0x0

#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004584)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_RMSK                                            0xffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_RSVD0_BMSK                                      0xffffff
#define HWIO_QFPROM_CORR_IMAGE_ENCR_KEY1_ROW2_MSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004588 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000458c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_HASH_DATA1_BMSK                                        0xffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROWn_MSB_HASH_DATA1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045a8)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045ac)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_RSVD0_BMSK                                             0xfffffc
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_RSVD0_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_CURRENT_SW_ROT_MODEM_BMSK                                   0x2
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_CURRENT_SW_ROT_MODEM_SHFT                                   0x1
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_CURRENT_SW_ROT_APPS_BMSK                                    0x1
#define HWIO_QFPROM_CORR_PK_HASH1_ROW4_MSB_CURRENT_SW_ROT_APPS_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004410 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_MAXn                                                   30
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004414 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_MAXn                                                   30
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004418 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_MAXn                                                   30
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000441c + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_MAXn                                                   30
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_SHFT                                        0x0


#endif /* __HALHWIOTSENSFUSE_H__ */
