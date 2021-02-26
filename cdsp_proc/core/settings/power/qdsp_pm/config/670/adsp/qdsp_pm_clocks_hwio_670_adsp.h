#ifndef __QDSP_PM_CLOCKS_HWIO_670_ADSP_H__
#define __QDSP_PM_CLOCKS_HWIO_670_ADSP_H__
/*
===========================================================================
*/
/**
  @file qdsp_pm_clocks_hwio_670_adsp.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r97]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_CSR
    LPASS_CC

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

  $Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/670/adsp/qdsp_pm_clocks_hwio_670_adsp.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: LPASS_CSR
 *--------------------------------------------------------------------------*/

#define LPASS_CSR_REG_BASE                                                                              (LPASS_BASE      + 0x00cc0000)
#define LPASS_CSR_REG_BASE_OFFS                                                                         0x00cc0000

#define HWIO_LPASS_CSR_GP_CTL_ADDR                                                                      (LPASS_CSR_REG_BASE      + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_OFFS                                                                      (LPASS_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_RMSK                                                                      0xffffc01f
#define HWIO_LPASS_CSR_GP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, HWIO_LPASS_CSR_GP_CTL_RMSK)
#define HWIO_LPASS_CSR_GP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, m)
#define HWIO_LPASS_CSR_GP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_GP_CTL_ADDR,v)
#define HWIO_LPASS_CSR_GP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_GP_CTL_ADDR,m,v,HWIO_LPASS_CSR_GP_CTL_IN)
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_BMSK                                                          0xfffe0000
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_SHFT                                                                0x11
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_BMSK                                                               0x10000
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_SHFT                                                                  0x10
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBI_ABORT_REQ_BMSK                                                   0x8000
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBI_ABORT_REQ_SHFT                                                      0xf
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBL_ABORT_REQ_BMSK                                                   0x4000
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBL_ABORT_REQ_SHFT                                                      0xe
#define HWIO_LPASS_CSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_BMSK                                                   0x10
#define HWIO_LPASS_CSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_SHFT                                                    0x4
#define HWIO_LPASS_CSR_GP_CTL_TLB_PL_HPROT_NB_EN_BMSK                                                          0x8
#define HWIO_LPASS_CSR_GP_CTL_TLB_PL_HPROT_NB_EN_SHFT                                                          0x3
#define HWIO_LPASS_CSR_GP_CTL_SPDIFTX_HPROT_NB_EN_BMSK                                                         0x4
#define HWIO_LPASS_CSR_GP_CTL_SPDIFTX_HPROT_NB_EN_SHFT                                                         0x2
#define HWIO_LPASS_CSR_GP_CTL_DML_HPROT_NB_EN_BMSK                                                             0x2
#define HWIO_LPASS_CSR_GP_CTL_DML_HPROT_NB_EN_SHFT                                                             0x1
#define HWIO_LPASS_CSR_GP_CTL_RESMP_HPROT_NB_EN_BMSK                                                           0x1
#define HWIO_LPASS_CSR_GP_CTL_RESMP_HPROT_NB_EN_SHFT                                                           0x0

#define HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR                                                                 (LPASS_CSR_REG_BASE      + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OFFS                                                                 (LPASS_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK                                                                 0xf0ffffff
#define HWIO_LPASS_CSR_MEMTYPE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, m)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,v)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,m,v,HWIO_LPASS_CSR_MEMTYPE_CTL_IN)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_BMSK                                                    0xe0000000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_SHFT                                                          0x1d
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_EN_BMSK                                                 0x10000000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_EN_SHFT                                                       0x1c
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_BMSK                                                    0xe00000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_SHFT                                                        0x15
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_EN_BMSK                                                 0x100000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_EN_SHFT                                                     0x14
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_BMSK                                                    0xe0000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_SHFT                                                       0x11
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_EN_BMSK                                                 0x10000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_EN_SHFT                                                    0x10
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_BMSK                                                      0xe000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_SHFT                                                         0xd
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_EN_BMSK                                                   0x1000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_EN_SHFT                                                      0xc
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_BMSK                                                        0xe00
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_SHFT                                                          0x9
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_EN_BMSK                                                     0x100
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_EN_SHFT                                                       0x8
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_BMSK                                                         0xe0
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_SHFT                                                          0x5
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_BMSK                                                      0x10
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_SHFT                                                       0x4
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_BMSK                                                            0xe
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_SHFT                                                            0x1
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_EN_BMSK                                                         0x1
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_EN_SHFT                                                         0x0

#define HWIO_LPASS_CSR_DEBUG_CTL_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00000010)
#define HWIO_LPASS_CSR_DEBUG_CTL_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00000010)
#define HWIO_LPASS_CSR_DEBUG_CTL_RMSK                                                                      0x1ff0f
#define HWIO_LPASS_CSR_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_CTL_ADDR, HWIO_LPASS_CSR_DEBUG_CTL_RMSK)
#define HWIO_LPASS_CSR_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_CTL_ADDR, m)
#define HWIO_LPASS_CSR_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_DEBUG_CTL_ADDR,v)
#define HWIO_LPASS_CSR_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_DEBUG_CTL_ADDR,m,v,HWIO_LPASS_CSR_DEBUG_CTL_IN)
#define HWIO_LPASS_CSR_DEBUG_CTL_LPASS_DEBUG_EN_BMSK                                                       0x10000
#define HWIO_LPASS_CSR_DEBUG_CTL_LPASS_DEBUG_EN_SHFT                                                          0x10
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_ADDR_BMSK                                                              0xff00
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_ADDR_SHFT                                                                 0x8
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_BMSK                                                                      0xf
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_SHFT                                                                      0x0

#define HWIO_LPASS_CSR_DEBUG_BUS_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_RMSK                                                                   0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_IN          \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, HWIO_LPASS_CSR_DEBUG_BUS_RMSK)
#define HWIO_LPASS_CSR_DEBUG_BUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, m)
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_BMSK                                                              0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_SHFT                                                                     0x0

#define HWIO_LPASS_CSR_MAXLIM_CTL_ADDR                                                                  (LPASS_CSR_REG_BASE      + 0x00000018)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OFFS                                                                  (LPASS_CSR_REG_BASE_OFFS + 0x00000018)
#define HWIO_LPASS_CSR_MAXLIM_CTL_RMSK                                                                        0x7f
#define HWIO_LPASS_CSR_MAXLIM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR, HWIO_LPASS_CSR_MAXLIM_CTL_RMSK)
#define HWIO_LPASS_CSR_MAXLIM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR, m)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR,v)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR,m,v,HWIO_LPASS_CSR_MAXLIM_CTL_IN)
#define HWIO_LPASS_CSR_MAXLIM_CTL_BYPASS_BMSK                                                                 0x40
#define HWIO_LPASS_CSR_MAXLIM_CTL_BYPASS_SHFT                                                                  0x6
#define HWIO_LPASS_CSR_MAXLIM_CTL_MAX_WRITES_BMSK                                                             0x3f
#define HWIO_LPASS_CSR_MAXLIM_CTL_MAX_WRITES_SHFT                                                              0x0

#define HWIO_LPASS_AHBI_CFG_ADDR                                                                        (LPASS_CSR_REG_BASE      + 0x00001004)
#define HWIO_LPASS_AHBI_CFG_OFFS                                                                        (LPASS_CSR_REG_BASE_OFFS + 0x00001004)
#define HWIO_LPASS_AHBI_CFG_RMSK                                                                             0xff1
#define HWIO_LPASS_AHBI_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, HWIO_LPASS_AHBI_CFG_RMSK)
#define HWIO_LPASS_AHBI_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, m)
#define HWIO_LPASS_AHBI_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_CFG_ADDR,v)
#define HWIO_LPASS_AHBI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_CFG_ADDR,m,v,HWIO_LPASS_AHBI_CFG_IN)
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_BMSK                                                               0xf00
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_SHFT                                                                 0x8
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_BMSK                                                              0xf0
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_SHFT                                                               0x4
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_BMSK                                                                0x1
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_SHFT                                                                0x0

#define HWIO_LPASS_AHBI_LOCK_CTL_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00001008)
#define HWIO_LPASS_AHBI_LOCK_CTL_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00001008)
#define HWIO_LPASS_AHBI_LOCK_CTL_RMSK                                                                          0x3
#define HWIO_LPASS_AHBI_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, HWIO_LPASS_AHBI_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBI_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBI_LOCK_CTL_IN)
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_BMSK                                                             0x2
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_SHFT                                                             0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_BMSK                                                           0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_SHFT                                                           0x0

#define HWIO_LPASS_AHBX_CFG_ADDR                                                                        (LPASS_CSR_REG_BASE      + 0x00001020)
#define HWIO_LPASS_AHBX_CFG_OFFS                                                                        (LPASS_CSR_REG_BASE_OFFS + 0x00001020)
#define HWIO_LPASS_AHBX_CFG_RMSK                                                                        0x7703f77f
#define HWIO_LPASS_AHBX_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, HWIO_LPASS_AHBX_CFG_RMSK)
#define HWIO_LPASS_AHBX_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, m)
#define HWIO_LPASS_AHBX_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_CFG_ADDR,v)
#define HWIO_LPASS_AHBX_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_CFG_ADDR,m,v,HWIO_LPASS_AHBX_CFG_IN)
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_BMSK                                                     0x70000000
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_SHFT                                                           0x1c
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_BMSK                                                        0x7000000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_SHFT                                                             0x18
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_BMSK                                                         0x38000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_SHFT                                                             0xf
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_BMSK                                                                0x7000
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_SHFT                                                                   0xc
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_BMSK                                                               0x700
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_SHFT                                                                 0x8
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_BMSK                                                            0x70
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_SHFT                                                             0x4
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_BMSK                                                              0xe
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_SHFT                                                              0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_BMSK                                                                0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_SHFT                                                                0x0

#define HWIO_LPASS_AHBX_LOCK_CTL_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00001024)
#define HWIO_LPASS_AHBX_LOCK_CTL_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00001024)
#define HWIO_LPASS_AHBX_LOCK_CTL_RMSK                                                                        0x7e5
#define HWIO_LPASS_AHBX_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, HWIO_LPASS_AHBX_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBX_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBX_LOCK_CTL_IN)
#define HWIO_LPASS_AHBX_LOCK_CTL_TLB_PL_LOCK_EN_BMSK                                                         0x400
#define HWIO_LPASS_AHBX_LOCK_CTL_TLB_PL_LOCK_EN_SHFT                                                           0xa
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                                       0x200
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                                         0x9
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_BMSK                                                           0x100
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_SHFT                                                             0x8
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                                         0x80
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                                          0x7
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                                       0x40
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                                        0x6
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                                         0x20
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                                          0x5
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_BMSK                                                               0x4
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_SHFT                                                               0x2
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                                         0x1
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                                         0x0

#define HWIO_LPASS_AHBX_CFG_EXT_ADDR                                                                    (LPASS_CSR_REG_BASE      + 0x00001028)
#define HWIO_LPASS_AHBX_CFG_EXT_OFFS                                                                    (LPASS_CSR_REG_BASE_OFFS + 0x00001028)
#define HWIO_LPASS_AHBX_CFG_EXT_RMSK                                                                           0x7
#define HWIO_LPASS_AHBX_CFG_EXT_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_EXT_ADDR, HWIO_LPASS_AHBX_CFG_EXT_RMSK)
#define HWIO_LPASS_AHBX_CFG_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_EXT_ADDR, m)
#define HWIO_LPASS_AHBX_CFG_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_CFG_EXT_ADDR,v)
#define HWIO_LPASS_AHBX_CFG_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_CFG_EXT_ADDR,m,v,HWIO_LPASS_AHBX_CFG_EXT_IN)
#define HWIO_LPASS_AHBX_CFG_EXT_TLB_PL_PRIORITY_BMSK                                                           0x7
#define HWIO_LPASS_AHBX_CFG_EXT_TLB_PL_PRIORITY_SHFT                                                           0x0

#define HWIO_LPASS_AHBIX_STATUS_ADDR                                                                    (LPASS_CSR_REG_BASE      + 0x00001030)
#define HWIO_LPASS_AHBIX_STATUS_OFFS                                                                    (LPASS_CSR_REG_BASE_OFFS + 0x00001030)
#define HWIO_LPASS_AHBIX_STATUS_RMSK                                                                         0x1ff
#define HWIO_LPASS_AHBIX_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, HWIO_LPASS_AHBIX_STATUS_RMSK)
#define HWIO_LPASS_AHBIX_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, m)
#define HWIO_LPASS_AHBIX_STATUS_AHBL_DECODE_BOUNDARY_ERROR_BMSK                                              0x100
#define HWIO_LPASS_AHBIX_STATUS_AHBL_DECODE_BOUNDARY_ERROR_SHFT                                                0x8
#define HWIO_LPASS_AHBIX_STATUS_AHBL_DECODE_ERROR_BMSK                                                        0x80
#define HWIO_LPASS_AHBIX_STATUS_AHBL_DECODE_ERROR_SHFT                                                         0x7
#define HWIO_LPASS_AHBIX_STATUS_AHBL_MISALIGNED_BMSK                                                          0x40
#define HWIO_LPASS_AHBIX_STATUS_AHBL_MISALIGNED_SHFT                                                           0x6
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                               0x20
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                                0x5
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_BMSK                                                        0x10
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_SHFT                                                         0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_BMSK                                                           0x8
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_SHFT                                                           0x3
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                                0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                                0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_BMSK                                                         0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_SHFT                                                         0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_BMSK                                                           0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_SHFT                                                           0x0

#define HWIO_LPASS_AHBIX_ACK_ADDR                                                                       (LPASS_CSR_REG_BASE      + 0x00001034)
#define HWIO_LPASS_AHBIX_ACK_OFFS                                                                       (LPASS_CSR_REG_BASE_OFFS + 0x00001034)
#define HWIO_LPASS_AHBIX_ACK_RMSK                                                                            0x1ff
#define HWIO_LPASS_AHBIX_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_ACK_ADDR,v)
#define HWIO_LPASS_AHBIX_ACK_AHBL_DECODE_BOUNDARY_ERROR_BMSK                                                 0x100
#define HWIO_LPASS_AHBIX_ACK_AHBL_DECODE_BOUNDARY_ERROR_SHFT                                                   0x8
#define HWIO_LPASS_AHBIX_ACK_AHBL_DECODE_ERROR_BMSK                                                           0x80
#define HWIO_LPASS_AHBIX_ACK_AHBL_DECODE_ERROR_SHFT                                                            0x7
#define HWIO_LPASS_AHBIX_ACK_AHBL_MISALIGNED_BMSK                                                             0x40
#define HWIO_LPASS_AHBIX_ACK_AHBL_MISALIGNED_SHFT                                                              0x6
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                                  0x20
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                                   0x5
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_BMSK                                                           0x10
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_SHFT                                                            0x4
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_BMSK                                                              0x8
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_SHFT                                                              0x3
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                                   0x4
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                                   0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_BMSK                                                            0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_SHFT                                                            0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_BMSK                                                              0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_SHFT                                                              0x0

#define HWIO_LPASS_AHBIX_EN_ADDR                                                                        (LPASS_CSR_REG_BASE      + 0x00001038)
#define HWIO_LPASS_AHBIX_EN_OFFS                                                                        (LPASS_CSR_REG_BASE_OFFS + 0x00001038)
#define HWIO_LPASS_AHBIX_EN_RMSK                                                                             0x1ff
#define HWIO_LPASS_AHBIX_EN_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, HWIO_LPASS_AHBIX_EN_RMSK)
#define HWIO_LPASS_AHBIX_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, m)
#define HWIO_LPASS_AHBIX_EN_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_EN_ADDR,v)
#define HWIO_LPASS_AHBIX_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBIX_EN_ADDR,m,v,HWIO_LPASS_AHBIX_EN_IN)
#define HWIO_LPASS_AHBIX_EN_AHBL_DECODE_BOUNDARY_ERROR_BMSK                                                  0x100
#define HWIO_LPASS_AHBIX_EN_AHBL_DECODE_BOUNDARY_ERROR_SHFT                                                    0x8
#define HWIO_LPASS_AHBIX_EN_AHBL_DECODE_ERROR_BMSK                                                            0x80
#define HWIO_LPASS_AHBIX_EN_AHBL_DECODE_ERROR_SHFT                                                             0x7
#define HWIO_LPASS_AHBIX_EN_AHBL_MISALIGNED_BMSK                                                              0x40
#define HWIO_LPASS_AHBIX_EN_AHBL_MISALIGNED_SHFT                                                               0x6
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                                   0x20
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                                    0x5
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_BMSK                                                            0x10
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_SHFT                                                             0x4
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_BMSK                                                               0x8
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_SHFT                                                               0x3
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                                    0x4
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                                    0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_BMSK                                                             0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_SHFT                                                             0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_BMSK                                                               0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_SHFT                                                               0x0

#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_ADDR                                                            (LPASS_CSR_REG_BASE      + 0x0000103c)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_OFFS                                                            (LPASS_CSR_REG_BASE_OFFS + 0x0000103c)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_RMSK                                                                   0x2
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_EXT_MASTER_CTL_ADDR, HWIO_LPASS_AHBIX_EXT_MASTER_CTL_RMSK)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_EXT_MASTER_CTL_ADDR, m)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_EXT_MASTER_CTL_ADDR,v)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBIX_EXT_MASTER_CTL_ADDR,m,v,HWIO_LPASS_AHBIX_EXT_MASTER_CTL_IN)
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_USE_INTERNAL_PATH_FOR_EXT_MASTER_BMSK                                  0x2
#define HWIO_LPASS_AHBIX_EXT_MASTER_CTL_USE_INTERNAL_PATH_FOR_EXT_MASTER_SHFT                                  0x1

#define HWIO_LPASS_AHBL_CFG_ADDR                                                                        (LPASS_CSR_REG_BASE      + 0x00001040)
#define HWIO_LPASS_AHBL_CFG_OFFS                                                                        (LPASS_CSR_REG_BASE_OFFS + 0x00001040)
#define HWIO_LPASS_AHBL_CFG_RMSK                                                                         0xffffff1
#define HWIO_LPASS_AHBL_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBL_CFG_ADDR, HWIO_LPASS_AHBL_CFG_RMSK)
#define HWIO_LPASS_AHBL_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBL_CFG_ADDR, m)
#define HWIO_LPASS_AHBL_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBL_CFG_ADDR,v)
#define HWIO_LPASS_AHBL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBL_CFG_ADDR,m,v,HWIO_LPASS_AHBL_CFG_IN)
#define HWIO_LPASS_AHBL_CFG_RESAMPLER_PRIORITY_BMSK                                                      0xf000000
#define HWIO_LPASS_AHBL_CFG_RESAMPLER_PRIORITY_SHFT                                                           0x18
#define HWIO_LPASS_AHBL_CFG_SLIMBUS_PRIORITY_BMSK                                                         0xf00000
#define HWIO_LPASS_AHBL_CFG_SLIMBUS_PRIORITY_SHFT                                                             0x14
#define HWIO_LPASS_AHBL_CFG_FABRIC_PRIORITY_BMSK                                                           0xf0000
#define HWIO_LPASS_AHBL_CFG_FABRIC_PRIORITY_SHFT                                                              0x10
#define HWIO_LPASS_AHBL_CFG_DM_PRIORITY_BMSK                                                                0xf000
#define HWIO_LPASS_AHBL_CFG_DM_PRIORITY_SHFT                                                                   0xc
#define HWIO_LPASS_AHBL_CFG_Q6SS_PRIORITY_BMSK                                                               0xf00
#define HWIO_LPASS_AHBL_CFG_Q6SS_PRIORITY_SHFT                                                                 0x8
#define HWIO_LPASS_AHBL_CFG_AUDIO_IF_PRIORITY_BMSK                                                            0xf0
#define HWIO_LPASS_AHBL_CFG_AUDIO_IF_PRIORITY_SHFT                                                             0x4
#define HWIO_LPASS_AHBL_CFG_ROUND_ROBIN_EN_BMSK                                                                0x1
#define HWIO_LPASS_AHBL_CFG_ROUND_ROBIN_EN_SHFT                                                                0x0

#define HWIO_LPASS_AHBL_LOCK_CTL_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00001044)
#define HWIO_LPASS_AHBL_LOCK_CTL_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00001044)
#define HWIO_LPASS_AHBL_LOCK_CTL_RMSK                                                                        0x1ff
#define HWIO_LPASS_AHBL_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBL_LOCK_CTL_ADDR, HWIO_LPASS_AHBL_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBL_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBL_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBL_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBL_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBL_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBL_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBL_LOCK_CTL_IN)
#define HWIO_LPASS_AHBL_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                                       0x100
#define HWIO_LPASS_AHBL_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                                         0x8
#define HWIO_LPASS_AHBL_LOCK_CTL_HDMI_LOCK_EN_BMSK                                                            0x80
#define HWIO_LPASS_AHBL_LOCK_CTL_HDMI_LOCK_EN_SHFT                                                             0x7
#define HWIO_LPASS_AHBL_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                                         0x40
#define HWIO_LPASS_AHBL_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                                          0x6
#define HWIO_LPASS_AHBL_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                                       0x20
#define HWIO_LPASS_AHBL_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                                        0x5
#define HWIO_LPASS_AHBL_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                                         0x10
#define HWIO_LPASS_AHBL_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                                          0x4
#define HWIO_LPASS_AHBL_LOCK_CTL_FABRIC_LOCK_EN_BMSK                                                           0x8
#define HWIO_LPASS_AHBL_LOCK_CTL_FABRIC_LOCK_EN_SHFT                                                           0x3
#define HWIO_LPASS_AHBL_LOCK_CTL_DM_LOCK_EN_BMSK                                                               0x4
#define HWIO_LPASS_AHBL_LOCK_CTL_DM_LOCK_EN_SHFT                                                               0x2
#define HWIO_LPASS_AHBL_LOCK_CTL_Q6SS_LOCK_EN_BMSK                                                             0x2
#define HWIO_LPASS_AHBL_LOCK_CTL_Q6SS_LOCK_EN_SHFT                                                             0x1
#define HWIO_LPASS_AHBL_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                                         0x1
#define HWIO_LPASS_AHBL_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                                         0x0

#define HWIO_LPASS_AHBL_CFG_EXT_ADDR                                                                    (LPASS_CSR_REG_BASE      + 0x00001048)
#define HWIO_LPASS_AHBL_CFG_EXT_OFFS                                                                    (LPASS_CSR_REG_BASE_OFFS + 0x00001048)
#define HWIO_LPASS_AHBL_CFG_EXT_RMSK                                                                         0xfff
#define HWIO_LPASS_AHBL_CFG_EXT_IN          \
        in_dword_masked(HWIO_LPASS_AHBL_CFG_EXT_ADDR, HWIO_LPASS_AHBL_CFG_EXT_RMSK)
#define HWIO_LPASS_AHBL_CFG_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBL_CFG_EXT_ADDR, m)
#define HWIO_LPASS_AHBL_CFG_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_AHBL_CFG_EXT_ADDR,v)
#define HWIO_LPASS_AHBL_CFG_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBL_CFG_EXT_ADDR,m,v,HWIO_LPASS_AHBL_CFG_EXT_IN)
#define HWIO_LPASS_AHBL_CFG_EXT_SLIMBUS1_PRIORITY_BMSK                                                       0xf00
#define HWIO_LPASS_AHBL_CFG_EXT_SLIMBUS1_PRIORITY_SHFT                                                         0x8
#define HWIO_LPASS_AHBL_CFG_EXT_HDMI_PRIORITY_BMSK                                                            0xf0
#define HWIO_LPASS_AHBL_CFG_EXT_HDMI_PRIORITY_SHFT                                                             0x4
#define HWIO_LPASS_AHBL_CFG_EXT_SPDIFTX_PRIORITY_BMSK                                                          0xf
#define HWIO_LPASS_AHBL_CFG_EXT_SPDIFTX_PRIORITY_SHFT                                                          0x0

#define HWIO_LPASS_DML_CTL_ADDR                                                                         (LPASS_CSR_REG_BASE      + 0x00002000)
#define HWIO_LPASS_DML_CTL_OFFS                                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00002000)
#define HWIO_LPASS_DML_CTL_RMSK                                                                         0x8000001f
#define HWIO_LPASS_DML_CTL_IN          \
        in_dword_masked(HWIO_LPASS_DML_CTL_ADDR, HWIO_LPASS_DML_CTL_RMSK)
#define HWIO_LPASS_DML_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_CTL_ADDR, m)
#define HWIO_LPASS_DML_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_DML_CTL_ADDR,v)
#define HWIO_LPASS_DML_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_CTL_ADDR,m,v,HWIO_LPASS_DML_CTL_IN)
#define HWIO_LPASS_DML_CTL_SOFT_RESET_BMSK                                                              0x80000000
#define HWIO_LPASS_DML_CTL_SOFT_RESET_SHFT                                                                    0x1f
#define HWIO_LPASS_DML_CTL_CRYPTO_EN_BMSK                                                                     0x10
#define HWIO_LPASS_DML_CTL_CRYPTO_EN_SHFT                                                                      0x4
#define HWIO_LPASS_DML_CTL_SWAP_BMSK                                                                           0xc
#define HWIO_LPASS_DML_CTL_SWAP_SHFT                                                                           0x2
#define HWIO_LPASS_DML_CTL_CMD_BMSK                                                                            0x3
#define HWIO_LPASS_DML_CTL_CMD_SHFT                                                                            0x0

#define HWIO_LPASS_DML_SRC_ADDR                                                                         (LPASS_CSR_REG_BASE      + 0x00002004)
#define HWIO_LPASS_DML_SRC_OFFS                                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00002004)
#define HWIO_LPASS_DML_SRC_RMSK                                                                         0xfffffffc
#define HWIO_LPASS_DML_SRC_IN          \
        in_dword_masked(HWIO_LPASS_DML_SRC_ADDR, HWIO_LPASS_DML_SRC_RMSK)
#define HWIO_LPASS_DML_SRC_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_SRC_ADDR, m)
#define HWIO_LPASS_DML_SRC_OUT(v)      \
        out_dword(HWIO_LPASS_DML_SRC_ADDR,v)
#define HWIO_LPASS_DML_SRC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_SRC_ADDR,m,v,HWIO_LPASS_DML_SRC_IN)
#define HWIO_LPASS_DML_SRC_ADDR_BMSK                                                                    0xfffffffc
#define HWIO_LPASS_DML_SRC_ADDR_SHFT                                                                           0x2

#define HWIO_LPASS_DML_DST_ADDR                                                                         (LPASS_CSR_REG_BASE      + 0x00002008)
#define HWIO_LPASS_DML_DST_OFFS                                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00002008)
#define HWIO_LPASS_DML_DST_RMSK                                                                         0xfffffffc
#define HWIO_LPASS_DML_DST_IN          \
        in_dword_masked(HWIO_LPASS_DML_DST_ADDR, HWIO_LPASS_DML_DST_RMSK)
#define HWIO_LPASS_DML_DST_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_DST_ADDR, m)
#define HWIO_LPASS_DML_DST_OUT(v)      \
        out_dword(HWIO_LPASS_DML_DST_ADDR,v)
#define HWIO_LPASS_DML_DST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_DST_ADDR,m,v,HWIO_LPASS_DML_DST_IN)
#define HWIO_LPASS_DML_DST_ADDR_BMSK                                                                    0xfffffffc
#define HWIO_LPASS_DML_DST_ADDR_SHFT                                                                           0x2

#define HWIO_LPASS_DML_TRAN_ADDR                                                                        (LPASS_CSR_REG_BASE      + 0x0000200c)
#define HWIO_LPASS_DML_TRAN_OFFS                                                                        (LPASS_CSR_REG_BASE_OFFS + 0x0000200c)
#define HWIO_LPASS_DML_TRAN_RMSK                                                                           0xffffc
#define HWIO_LPASS_DML_TRAN_IN          \
        in_dword_masked(HWIO_LPASS_DML_TRAN_ADDR, HWIO_LPASS_DML_TRAN_RMSK)
#define HWIO_LPASS_DML_TRAN_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_TRAN_ADDR, m)
#define HWIO_LPASS_DML_TRAN_OUT(v)      \
        out_dword(HWIO_LPASS_DML_TRAN_ADDR,v)
#define HWIO_LPASS_DML_TRAN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_TRAN_ADDR,m,v,HWIO_LPASS_DML_TRAN_IN)
#define HWIO_LPASS_DML_TRAN_LEN_BMSK                                                                       0xffffc
#define HWIO_LPASS_DML_TRAN_LEN_SHFT                                                                           0x2

#define HWIO_LPASS_DML_STATUS_ADDR                                                                      (LPASS_CSR_REG_BASE      + 0x00002010)
#define HWIO_LPASS_DML_STATUS_OFFS                                                                      (LPASS_CSR_REG_BASE_OFFS + 0x00002010)
#define HWIO_LPASS_DML_STATUS_RMSK                                                                        0xffffc3
#define HWIO_LPASS_DML_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_DML_STATUS_ADDR, HWIO_LPASS_DML_STATUS_RMSK)
#define HWIO_LPASS_DML_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_STATUS_ADDR, m)
#define HWIO_LPASS_DML_STATUS_WORDS_BMSK                                                                  0xffffc0
#define HWIO_LPASS_DML_STATUS_WORDS_SHFT                                                                       0x6
#define HWIO_LPASS_DML_STATUS_ERROR_BMSK                                                                       0x2
#define HWIO_LPASS_DML_STATUS_ERROR_SHFT                                                                       0x1
#define HWIO_LPASS_DML_STATUS_DONE_BMSK                                                                        0x1
#define HWIO_LPASS_DML_STATUS_DONE_SHFT                                                                        0x0

#define HWIO_LPASS_DML_ACK_ADDR                                                                         (LPASS_CSR_REG_BASE      + 0x00002014)
#define HWIO_LPASS_DML_ACK_OFFS                                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00002014)
#define HWIO_LPASS_DML_ACK_RMSK                                                                                0x3
#define HWIO_LPASS_DML_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_DML_ACK_ADDR,v)
#define HWIO_LPASS_DML_ACK_ERROR_BMSK                                                                          0x2
#define HWIO_LPASS_DML_ACK_ERROR_SHFT                                                                          0x1
#define HWIO_LPASS_DML_ACK_DONE_BMSK                                                                           0x1
#define HWIO_LPASS_DML_ACK_DONE_SHFT                                                                           0x0

#define HWIO_LPASS_DML_HW_VERSION_ADDR                                                                  (LPASS_CSR_REG_BASE      + 0x00002018)
#define HWIO_LPASS_DML_HW_VERSION_OFFS                                                                  (LPASS_CSR_REG_BASE_OFFS + 0x00002018)
#define HWIO_LPASS_DML_HW_VERSION_RMSK                                                                  0xffffffff
#define HWIO_LPASS_DML_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_DML_HW_VERSION_ADDR, HWIO_LPASS_DML_HW_VERSION_RMSK)
#define HWIO_LPASS_DML_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_HW_VERSION_ADDR, m)
#define HWIO_LPASS_DML_HW_VERSION_MAJOR_BMSK                                                            0xf0000000
#define HWIO_LPASS_DML_HW_VERSION_MAJOR_SHFT                                                                  0x1c
#define HWIO_LPASS_DML_HW_VERSION_MINOR_BMSK                                                             0xfff0000
#define HWIO_LPASS_DML_HW_VERSION_MINOR_SHFT                                                                  0x10
#define HWIO_LPASS_DML_HW_VERSION_STEP_BMSK                                                                 0xffff
#define HWIO_LPASS_DML_HW_VERSION_STEP_SHFT                                                                    0x0

#define HWIO_LPASS_DML_SRC_EXT_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x0000201c)
#define HWIO_LPASS_DML_SRC_EXT_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x0000201c)
#define HWIO_LPASS_DML_SRC_EXT_RMSK                                                                            0xf
#define HWIO_LPASS_DML_SRC_EXT_IN          \
        in_dword_masked(HWIO_LPASS_DML_SRC_EXT_ADDR, HWIO_LPASS_DML_SRC_EXT_RMSK)
#define HWIO_LPASS_DML_SRC_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_SRC_EXT_ADDR, m)
#define HWIO_LPASS_DML_SRC_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_DML_SRC_EXT_ADDR,v)
#define HWIO_LPASS_DML_SRC_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_SRC_EXT_ADDR,m,v,HWIO_LPASS_DML_SRC_EXT_IN)
#define HWIO_LPASS_DML_SRC_EXT_ADDR_BMSK                                                                       0xf
#define HWIO_LPASS_DML_SRC_EXT_ADDR_SHFT                                                                       0x0

#define HWIO_LPASS_DML_DST_EXT_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x00002020)
#define HWIO_LPASS_DML_DST_EXT_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00002020)
#define HWIO_LPASS_DML_DST_EXT_RMSK                                                                            0xf
#define HWIO_LPASS_DML_DST_EXT_IN          \
        in_dword_masked(HWIO_LPASS_DML_DST_EXT_ADDR, HWIO_LPASS_DML_DST_EXT_RMSK)
#define HWIO_LPASS_DML_DST_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_DST_EXT_ADDR, m)
#define HWIO_LPASS_DML_DST_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_DML_DST_EXT_ADDR,v)
#define HWIO_LPASS_DML_DST_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_DST_EXT_ADDR,m,v,HWIO_LPASS_DML_DST_EXT_IN)
#define HWIO_LPASS_DML_DST_EXT_ADDR_BMSK                                                                       0xf
#define HWIO_LPASS_DML_DST_EXT_ADDR_SHFT                                                                       0x0

#define HWIO_LPASS_LPM_CTL_ADDR                                                                         (LPASS_CSR_REG_BASE      + 0x00003000)
#define HWIO_LPASS_LPM_CTL_OFFS                                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00003000)
#define HWIO_LPASS_LPM_CTL_RMSK                                                                         0x80000000
#define HWIO_LPASS_LPM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, HWIO_LPASS_LPM_CTL_RMSK)
#define HWIO_LPASS_LPM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, m)
#define HWIO_LPASS_LPM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPM_CTL_ADDR,v)
#define HWIO_LPASS_LPM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPM_CTL_ADDR,m,v,HWIO_LPASS_LPM_CTL_IN)
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_BMSK                                                              0x80000000
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_SHFT                                                                    0x1f

#define HWIO_LPASS_LPM_HW_VERSION_ADDR                                                                  (LPASS_CSR_REG_BASE      + 0x00003004)
#define HWIO_LPASS_LPM_HW_VERSION_OFFS                                                                  (LPASS_CSR_REG_BASE_OFFS + 0x00003004)
#define HWIO_LPASS_LPM_HW_VERSION_RMSK                                                                  0xffffffff
#define HWIO_LPASS_LPM_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_LPM_HW_VERSION_ADDR, HWIO_LPASS_LPM_HW_VERSION_RMSK)
#define HWIO_LPASS_LPM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPM_HW_VERSION_ADDR, m)
#define HWIO_LPASS_LPM_HW_VERSION_MAJOR_BMSK                                                            0xf0000000
#define HWIO_LPASS_LPM_HW_VERSION_MAJOR_SHFT                                                                  0x1c
#define HWIO_LPASS_LPM_HW_VERSION_MINOR_BMSK                                                             0xfff0000
#define HWIO_LPASS_LPM_HW_VERSION_MINOR_SHFT                                                                  0x10
#define HWIO_LPASS_LPM_HW_VERSION_STEP_BMSK                                                                 0xffff
#define HWIO_LPASS_LPM_HW_VERSION_STEP_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_CTL_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00004000)
#define HWIO_LPASS_RESAMPLER_CTL_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00004000)
#define HWIO_LPASS_RESAMPLER_CTL_RMSK                                                                   0xf0000000
#define HWIO_LPASS_RESAMPLER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CTL_ADDR, HWIO_LPASS_RESAMPLER_CTL_RMSK)
#define HWIO_LPASS_RESAMPLER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CTL_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CTL_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CTL_ADDR,m,v,HWIO_LPASS_RESAMPLER_CTL_IN)
#define HWIO_LPASS_RESAMPLER_CTL_SOFT_RESET_BMSK                                                        0x80000000
#define HWIO_LPASS_RESAMPLER_CTL_SOFT_RESET_SHFT                                                              0x1f
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_RESET_BMSK                                                       0x40000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_RESET_SHFT                                                             0x1e
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_CORE_CGC_EN_BMSK                                                 0x20000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_CORE_CGC_EN_SHFT                                                       0x1d
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_AHB_CGC_EN_BMSK                                                  0x10000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_AHB_CGC_EN_SHFT                                                        0x1c

#define HWIO_LPASS_RESAMPLER_STAT_ADDR                                                                  (LPASS_CSR_REG_BASE      + 0x00004004)
#define HWIO_LPASS_RESAMPLER_STAT_OFFS                                                                  (LPASS_CSR_REG_BASE_OFFS + 0x00004004)
#define HWIO_LPASS_RESAMPLER_STAT_RMSK                                                                         0x1
#define HWIO_LPASS_RESAMPLER_STAT_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_STAT_ADDR, HWIO_LPASS_RESAMPLER_STAT_RMSK)
#define HWIO_LPASS_RESAMPLER_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_STAT_ADDR, m)
#define HWIO_LPASS_RESAMPLER_STAT_SOFT_RESET_STAT_BMSK                                                         0x1
#define HWIO_LPASS_RESAMPLER_STAT_SOFT_RESET_STAT_SHFT                                                         0x0

#define HWIO_LPASS_LPAIF_CTL_ADDR                                                                       (LPASS_CSR_REG_BASE      + 0x00005000)
#define HWIO_LPASS_LPAIF_CTL_OFFS                                                                       (LPASS_CSR_REG_BASE_OFFS + 0x00005000)
#define HWIO_LPASS_LPAIF_CTL_RMSK                                                                       0x80000000
#define HWIO_LPASS_LPAIF_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, HWIO_LPASS_LPAIF_CTL_RMSK)
#define HWIO_LPASS_LPAIF_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, m)
#define HWIO_LPASS_LPAIF_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_CTL_ADDR,v)
#define HWIO_LPASS_LPAIF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_CTL_ADDR,m,v,HWIO_LPASS_LPAIF_CTL_IN)
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_BMSK                                                            0x80000000
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_SHFT                                                                  0x1f

#define HWIO_LPASS_SPDIFTX_CTL_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x00006000)
#define HWIO_LPASS_SPDIFTX_CTL_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00006000)
#define HWIO_LPASS_SPDIFTX_CTL_RMSK                                                                     0xc0000000
#define HWIO_LPASS_SPDIFTX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SPDIFTX_CTL_ADDR, HWIO_LPASS_SPDIFTX_CTL_RMSK)
#define HWIO_LPASS_SPDIFTX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SPDIFTX_CTL_ADDR, m)
#define HWIO_LPASS_SPDIFTX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SPDIFTX_CTL_ADDR,v)
#define HWIO_LPASS_SPDIFTX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SPDIFTX_CTL_ADDR,m,v,HWIO_LPASS_SPDIFTX_CTL_IN)
#define HWIO_LPASS_SPDIFTX_CTL_SOFT_RESET_BMSK                                                          0x80000000
#define HWIO_LPASS_SPDIFTX_CTL_SOFT_RESET_SHFT                                                                0x1f
#define HWIO_LPASS_SPDIFTX_CTL_FORCE_RESET_BMSK                                                         0x40000000
#define HWIO_LPASS_SPDIFTX_CTL_FORCE_RESET_SHFT                                                               0x1e

#define HWIO_LPASS_SPDIFTX_STAT_ADDR                                                                    (LPASS_CSR_REG_BASE      + 0x00006004)
#define HWIO_LPASS_SPDIFTX_STAT_OFFS                                                                    (LPASS_CSR_REG_BASE_OFFS + 0x00006004)
#define HWIO_LPASS_SPDIFTX_STAT_RMSK                                                                           0x1
#define HWIO_LPASS_SPDIFTX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_SPDIFTX_STAT_ADDR, HWIO_LPASS_SPDIFTX_STAT_RMSK)
#define HWIO_LPASS_SPDIFTX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_SPDIFTX_STAT_ADDR, m)
#define HWIO_LPASS_SPDIFTX_STAT_SOFT_RESET_STAT_BMSK                                                           0x1
#define HWIO_LPASS_SPDIFTX_STAT_SOFT_RESET_STAT_SHFT                                                           0x0

#define HWIO_LPASS_HDMIRX_CTL_ADDR                                                                      (LPASS_CSR_REG_BASE      + 0x00007000)
#define HWIO_LPASS_HDMIRX_CTL_OFFS                                                                      (LPASS_CSR_REG_BASE_OFFS + 0x00007000)
#define HWIO_LPASS_HDMIRX_CTL_RMSK                                                                      0xc0000000
#define HWIO_LPASS_HDMIRX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_HDMIRX_CTL_ADDR, HWIO_LPASS_HDMIRX_CTL_RMSK)
#define HWIO_LPASS_HDMIRX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMIRX_CTL_ADDR, m)
#define HWIO_LPASS_HDMIRX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_HDMIRX_CTL_ADDR,v)
#define HWIO_LPASS_HDMIRX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMIRX_CTL_ADDR,m,v,HWIO_LPASS_HDMIRX_CTL_IN)
#define HWIO_LPASS_HDMIRX_CTL_SOFT_RESET_BMSK                                                           0x80000000
#define HWIO_LPASS_HDMIRX_CTL_SOFT_RESET_SHFT                                                                 0x1f
#define HWIO_LPASS_HDMIRX_CTL_FORCE_RESET_BMSK                                                          0x40000000
#define HWIO_LPASS_HDMIRX_CTL_FORCE_RESET_SHFT                                                                0x1e

#define HWIO_LPASS_HDMIRX_STAT_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x00007004)
#define HWIO_LPASS_HDMIRX_STAT_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00007004)
#define HWIO_LPASS_HDMIRX_STAT_RMSK                                                                            0x1
#define HWIO_LPASS_HDMIRX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_HDMIRX_STAT_ADDR, HWIO_LPASS_HDMIRX_STAT_RMSK)
#define HWIO_LPASS_HDMIRX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMIRX_STAT_ADDR, m)
#define HWIO_LPASS_HDMIRX_STAT_SOFT_RESET_STAT_BMSK                                                            0x1
#define HWIO_LPASS_HDMIRX_STAT_SOFT_RESET_STAT_SHFT                                                            0x0

#define HWIO_LPASS_HDMITX_CTL_ADDR                                                                      (LPASS_CSR_REG_BASE      + 0x00008000)
#define HWIO_LPASS_HDMITX_CTL_OFFS                                                                      (LPASS_CSR_REG_BASE_OFFS + 0x00008000)
#define HWIO_LPASS_HDMITX_CTL_RMSK                                                                      0xc0000000
#define HWIO_LPASS_HDMITX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_CTL_ADDR, HWIO_LPASS_HDMITX_CTL_RMSK)
#define HWIO_LPASS_HDMITX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_CTL_ADDR, m)
#define HWIO_LPASS_HDMITX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_HDMITX_CTL_ADDR,v)
#define HWIO_LPASS_HDMITX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMITX_CTL_ADDR,m,v,HWIO_LPASS_HDMITX_CTL_IN)
#define HWIO_LPASS_HDMITX_CTL_SOFT_RESET_BMSK                                                           0x80000000
#define HWIO_LPASS_HDMITX_CTL_SOFT_RESET_SHFT                                                                 0x1f
#define HWIO_LPASS_HDMITX_CTL_FORCE_RESET_BMSK                                                          0x40000000
#define HWIO_LPASS_HDMITX_CTL_FORCE_RESET_SHFT                                                                0x1e

#define HWIO_LPASS_HDMITX_STAT_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x00008004)
#define HWIO_LPASS_HDMITX_STAT_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00008004)
#define HWIO_LPASS_HDMITX_STAT_RMSK                                                                            0x1
#define HWIO_LPASS_HDMITX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_STAT_ADDR, HWIO_LPASS_HDMITX_STAT_RMSK)
#define HWIO_LPASS_HDMITX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_STAT_ADDR, m)
#define HWIO_LPASS_HDMITX_STAT_SOFT_RESET_STAT_BMSK                                                            0x1
#define HWIO_LPASS_HDMITX_STAT_SOFT_RESET_STAT_SHFT                                                            0x0

#define HWIO_LPASS_HDMITX_LEGACY_ADDR                                                                   (LPASS_CSR_REG_BASE      + 0x00008008)
#define HWIO_LPASS_HDMITX_LEGACY_OFFS                                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00008008)
#define HWIO_LPASS_HDMITX_LEGACY_RMSK                                                                          0x1
#define HWIO_LPASS_HDMITX_LEGACY_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_LEGACY_ADDR, HWIO_LPASS_HDMITX_LEGACY_RMSK)
#define HWIO_LPASS_HDMITX_LEGACY_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_LEGACY_ADDR, m)
#define HWIO_LPASS_HDMITX_LEGACY_OUT(v)      \
        out_dword(HWIO_LPASS_HDMITX_LEGACY_ADDR,v)
#define HWIO_LPASS_HDMITX_LEGACY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMITX_LEGACY_ADDR,m,v,HWIO_LPASS_HDMITX_LEGACY_IN)
#define HWIO_LPASS_HDMITX_LEGACY_LEGACY_EN_BMSK                                                                0x1
#define HWIO_LPASS_HDMITX_LEGACY_LEGACY_EN_SHFT                                                                0x0

#define HWIO_LPASS_TLB_PL_CTL_ADDR                                                                      (LPASS_CSR_REG_BASE      + 0x00009000)
#define HWIO_LPASS_TLB_PL_CTL_OFFS                                                                      (LPASS_CSR_REG_BASE_OFFS + 0x00009000)
#define HWIO_LPASS_TLB_PL_CTL_RMSK                                                                      0xc0000000
#define HWIO_LPASS_TLB_PL_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TLB_PL_CTL_ADDR, HWIO_LPASS_TLB_PL_CTL_RMSK)
#define HWIO_LPASS_TLB_PL_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TLB_PL_CTL_ADDR, m)
#define HWIO_LPASS_TLB_PL_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TLB_PL_CTL_ADDR,v)
#define HWIO_LPASS_TLB_PL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TLB_PL_CTL_ADDR,m,v,HWIO_LPASS_TLB_PL_CTL_IN)
#define HWIO_LPASS_TLB_PL_CTL_SOFT_RESET_BMSK                                                           0x80000000
#define HWIO_LPASS_TLB_PL_CTL_SOFT_RESET_SHFT                                                                 0x1f
#define HWIO_LPASS_TLB_PL_CTL_FORCE_RESET_BMSK                                                          0x40000000
#define HWIO_LPASS_TLB_PL_CTL_FORCE_RESET_SHFT                                                                0x1e

#define HWIO_LPASS_TLB_PL_STAT_ADDR                                                                     (LPASS_CSR_REG_BASE      + 0x00009004)
#define HWIO_LPASS_TLB_PL_STAT_OFFS                                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00009004)
#define HWIO_LPASS_TLB_PL_STAT_RMSK                                                                            0x1
#define HWIO_LPASS_TLB_PL_STAT_IN          \
        in_dword_masked(HWIO_LPASS_TLB_PL_STAT_ADDR, HWIO_LPASS_TLB_PL_STAT_RMSK)
#define HWIO_LPASS_TLB_PL_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_TLB_PL_STAT_ADDR, m)
#define HWIO_LPASS_TLB_PL_STAT_SOFT_RESET_STAT_BMSK                                                            0x1
#define HWIO_LPASS_TLB_PL_STAT_SOFT_RESET_STAT_SHFT                                                            0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR                                                   (LPASS_CSR_REG_BASE      + 0x0000a000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OFFS                                                   (LPASS_CSR_REG_BASE_OFFS + 0x0000a000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_RMSK                                                          0x3
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_HW_CTL_BMSK                                                   0x2
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_HW_CTL_SHFT                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR                                             (LPASS_CSR_REG_BASE      + 0x0000a004)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OFFS                                             (LPASS_CSR_REG_BASE_OFFS + 0x0000a004)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_RMSK                                                    0x7
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                     0x7
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                     0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR                                                        (LPASS_CSR_REG_BASE      + 0x0000b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OFFS                                                        (LPASS_CSR_REG_BASE_OFFS + 0x0000b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0000b004)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0000b004)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_RMSK                                                         0x7
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                          0x7
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                          0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x0000c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x0000c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR                                               (LPASS_CSR_REG_BASE      + 0x0000c004)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x0000c004)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR                                                        (LPASS_CSR_REG_BASE      + 0x0000d000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OFFS                                                        (LPASS_CSR_REG_BASE_OFFS + 0x0000d000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0000d004)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0000d004)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_RMSK                                                         0x7
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                          0x7
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0000e000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0000e000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_RMSK                                                         0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_HW_CTL_BMSK                                                  0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_HW_CTL_SHFT                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR                                            (LPASS_CSR_REG_BASE      + 0x0000e004)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OFFS                                            (LPASS_CSR_REG_BASE_OFFS + 0x0000e004)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_RMSK                                                   0x7
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                    0x7
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                    0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0000f000)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0000f000)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_RMSK                                                         0x3
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_HW_CTL_BMSK                                                  0x2
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_HW_CTL_SHFT                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR                                            (LPASS_CSR_REG_BASE      + 0x0000f004)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OFFS                                            (LPASS_CSR_REG_BASE_OFFS + 0x0000f004)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_RMSK                                                   0x7
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                    0x7
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                    0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00010000)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00010000)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR                                          (LPASS_CSR_REG_BASE      + 0x00010004)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OFFS                                          (LPASS_CSR_REG_BASE_OFFS + 0x00010004)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_RMSK                                                 0x7
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00011000)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00011000)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR                                          (LPASS_CSR_REG_BASE      + 0x00011004)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OFFS                                          (LPASS_CSR_REG_BASE_OFFS + 0x00011004)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_RMSK                                                 0x7
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x00012000)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00012000)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR                                               (LPASS_CSR_REG_BASE      + 0x00012004)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x00012004)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x00013000)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00013000)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR                                               (LPASS_CSR_REG_BASE      + 0x00013004)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x00013004)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00014000)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00014000)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR                                          (LPASS_CSR_REG_BASE      + 0x00014004)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OFFS                                          (LPASS_CSR_REG_BASE_OFFS + 0x00014004)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_RMSK                                                 0x7
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x00015000)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x00015000)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR                                               (LPASS_CSR_REG_BASE      + 0x00015004)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x00015004)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR                                                        (LPASS_CSR_REG_BASE      + 0x00016000)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OFFS                                                        (LPASS_CSR_REG_BASE_OFFS + 0x00016000)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x00016004)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x00016004)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_RMSK                                                         0x7
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                          0x7
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                          0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00017000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00017000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR                                          (LPASS_CSR_REG_BASE      + 0x00017004)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OFFS                                          (LPASS_CSR_REG_BASE_OFFS + 0x00017004)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_RMSK                                                 0x7
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR                                                    (LPASS_CSR_REG_BASE      + 0x00019000)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OFFS                                                    (LPASS_CSR_REG_BASE_OFFS + 0x00019000)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_RMSK                                                           0x3
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_HW_CTL_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_HW_CTL_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR                                              (LPASS_CSR_REG_BASE      + 0x00019004)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OFFS                                              (LPASS_CSR_REG_BASE_OFFS + 0x00019004)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_RMSK                                                     0x7
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                      0x7
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                      0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR                                                (LPASS_CSR_REG_BASE      + 0x0001a000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x0001a000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR                                          (LPASS_CSR_REG_BASE      + 0x0001a004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OFFS                                          (LPASS_CSR_REG_BASE_OFFS + 0x0001a004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_RMSK                                                 0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR                                             (LPASS_CSR_REG_BASE      + 0x0001a008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OFFS                                             (LPASS_CSR_REG_BASE_OFFS + 0x0001a008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_RMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_TIMEBASE_EN_BMSK                                        0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_TIMEBASE_EN_SHFT                                        0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR                                                              (LPASS_CSR_REG_BASE      + 0x0001b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OFFS                                                              (LPASS_CSR_REG_BASE_OFFS + 0x0001b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_RMSK                                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_QOS_ENABLE_BMSK                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_QOS_ENABLE_SHFT                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_ADDR                                           (LPASS_CSR_REG_BASE      + 0x0001c000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_OFFS                                           (LPASS_CSR_REG_BASE_OFFS + 0x0001c000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_RMSK                                                  0x3
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_HW_CTL_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_HW_CTL_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_CGCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ADDR                                     (LPASS_CSR_REG_BASE      + 0x0001c004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_OFFS                                     (LPASS_CSR_REG_BASE_OFFS + 0x0001c004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_RMSK                                            0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                             0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                             0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_ADDR                                        (LPASS_CSR_REG_BASE      + 0x0001c008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_OFFS                                        (LPASS_CSR_REG_BASE_OFFS + 0x0001c008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_RMSK                                               0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_TIMEBASE_EN_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_AHBL_TIMEBASE_CTL_TIMEBASE_EN_SHFT                                   0x0

#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x0001d000)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x0001d000)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_RMSK                                                            0x3
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_CGCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ADDR                                               (LPASS_CSR_REG_BASE      + 0x0001d004)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x0001d004)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_RMSK                                                      0x7
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                       0x7
#define HWIO_LPASS_AUDIO_CORE_AUDIO_CODEC_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                       0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_ADDR                                               (LPASS_CSR_REG_BASE      + 0x0001d008)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x0001d008)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_RMSK                                                    0x1ff
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_MST_SLV_CLK_SEL_BMSK                         0x100
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_MST_SLV_CLK_SEL_SHFT                           0x8
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIG_CODEC_INT_CLOCK_OBS_EN_BMSK                          0x80
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIG_CODEC_INT_CLOCK_OBS_EN_SHFT                           0x7
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_TLMM_TEST_SD1_OUT_OE_BMSK                                0x40
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_TLMM_TEST_SD1_OUT_OE_SHFT                                 0x6
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_TLMM_TEST_SD0_OUT_OE_BMSK                                0x20
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_TLMM_TEST_SD0_OUT_OE_SHFT                                 0x5
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S1_WS_IN_SEL_BMSK                                0x10
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S1_WS_IN_SEL_SHFT                                 0x4
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIGITAL_CLOCK_SD1_IN_SEL_BMSK                             0x8
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIGITAL_CLOCK_SD1_IN_SEL_SHFT                             0x3
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIGITAL_CODEC_RX0_RX1_WS_IN_SEL_BMSK                      0x4
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_DIGITAL_CODEC_RX0_RX1_WS_IN_SEL_SHFT                      0x2
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_WS_IN_SEL_BMSK                                 0x2
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_WS_IN_SEL_SHFT                                 0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_SCLK_SEL_BMSK                                  0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_SPKR_CTL_LPAIF_I2S0_SCLK_SEL_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_ADDR                                                (LPASS_CSR_REG_BASE      + 0x0001d00c)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x0001d00c)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_RMSK                                                      0xff
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S2_MST_SLV_CLK_SEL_BMSK                           0x80
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S2_MST_SLV_CLK_SEL_SHFT                            0x7
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S3_MST_SLV_CLK_SEL_BMSK                           0x40
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S3_MST_SLV_CLK_SEL_SHFT                            0x6
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_DIG_CODEC_INT_CLOCK_TX2_OBS_EN_BMSK                       0x20
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_DIG_CODEC_INT_CLOCK_TX2_OBS_EN_SHFT                        0x5
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_DIG_CODEC_INT_CLOCK_TX_OBS_EN_BMSK                        0x10
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_DIG_CODEC_INT_CLOCK_TX_OBS_EN_SHFT                         0x4
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_TLMM_TEST_SD1_OUT_OE_BMSK                                  0x8
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_TLMM_TEST_SD1_OUT_OE_SHFT                                  0x3
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_TLMM_TEST_SD0_OUT_OE_BMSK                                  0x4
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_TLMM_TEST_SD0_OUT_OE_SHFT                                  0x2
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S2_SCLK_SEL_BMSK                                   0x2
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S2_SCLK_SEL_SHFT                                   0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S3_SCLK_SEL_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_MUX_MIC_CTL_LPAIF_I2S3_SCLK_SEL_SHFT                                   0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0001d010)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0001d010)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_RMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S0_TEST_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0001d014)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0001d014)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_RMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S1_TEST_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ADDR                                                 (LPASS_CSR_REG_BASE      + 0x0001d018)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_OFFS                                                 (LPASS_CSR_REG_BASE_OFFS + 0x0001d018)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_RMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_RX0_1_TEST_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0001d01c)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0001d01c)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_RMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S2_TEST_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ADDR                                                  (LPASS_CSR_REG_BASE      + 0x0001d020)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_OFFS                                                  (LPASS_CSR_REG_BASE_OFFS + 0x0001d020)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_RMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_I2S3_TEST_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ADDR                                                 (LPASS_CSR_REG_BASE      + 0x0001d024)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_OFFS                                                 (LPASS_CSR_REG_BASE_OFFS + 0x0001d024)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_RMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX0_1_TEST_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ADDR                                                   (LPASS_CSR_REG_BASE      + 0x0001d028)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_OFFS                                                   (LPASS_CSR_REG_BASE_OFFS + 0x0001d028)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_RMSK                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_DCODEC_GP_TX2_TEST_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_ADDR                                                       (LPASS_CSR_REG_BASE      + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_OFFS                                                       (LPASS_CSR_REG_BASE_OFFS + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_RMSK                                                              0x3
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_CGCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ADDR                                                 (LPASS_CSR_REG_BASE      + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_OFFS                                                 (LPASS_CSR_REG_BASE_OFFS + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_RMSK                                                        0x7
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                         0x7
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                         0x0

#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_ADDR                                        (LPASS_CSR_REG_BASE      + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_OFFS                                        (LPASS_CSR_REG_BASE_OFFS + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_RMSK                                            0x3fff
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_MST_SLV_CLK_SEL_BMSK                 0x2000
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_MST_SLV_CLK_SEL_SHFT                    0xd
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S4_MST_SLV_CLK_SEL_BMSK                 0x1000
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S4_MST_SLV_CLK_SEL_SHFT                    0xc
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_DIG_CODEC_INT_CLOCK_RX_OBS_EN_BMSK               0x800
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_DIG_CODEC_INT_CLOCK_RX_OBS_EN_SHFT                 0xb
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_DIG_CODEC_INT_CLOCK_TX_OBS_EN_BMSK               0x400
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_DIG_CODEC_INT_CLOCK_TX_OBS_EN_SHFT                 0xa
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SLKC_OUT_RX_TX_SEL_BMSK                          0x200
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SLKC_OUT_RX_TX_SEL_SHFT                            0x9
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_TLMM_TEST_SD1_OUT_OE_BMSK                        0x100
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_TLMM_TEST_SD1_OUT_OE_SHFT                          0x8
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_TLMM_TEST_SD0_OUT_OE_BMSK                         0x80
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_TLMM_TEST_SD0_OUT_OE_SHFT                          0x7
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S6_WS_IN_SEL_BMSK                         0x40
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S6_WS_IN_SEL_SHFT                          0x6
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_TX_CLOCK_MASTER_SEL_BMSK                     0x20
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_TX_CLOCK_MASTER_SEL_SHFT                      0x5
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_TX_WS_IN_SEL_BMSK                            0x10
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_TX_WS_IN_SEL_SHFT                             0x4
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_WS_IN_SEL_BMSK                          0x8
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_WS_IN_SEL_SHFT                          0x3
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_SCLK_SEL_BMSK                           0x4
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S5_SCLK_SEL_SHFT                           0x2
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_RX_CLOCK_MASTER_SEL_BMSK                      0x2
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_SWRM_RX_CLOCK_MASTER_SEL_SHFT                      0x1
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S4_SCLK_SEL_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_SOUNDWIRE_GP_SPKR_MIC_MUX_CTL_LPAIF_I2S4_SCLK_SEL_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ADDR                                                    (LPASS_CSR_REG_BASE      + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_OFFS                                                    (LPASS_CSR_REG_BASE_OFFS + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S4_TEST_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ADDR                                                    (LPASS_CSR_REG_BASE      + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_OFFS                                                    (LPASS_CSR_REG_BASE_OFFS + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S5_TEST_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ADDR                                                     (LPASS_CSR_REG_BASE      + 0x0001e014)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_OFFS                                                     (LPASS_CSR_REG_BASE_OFFS + 0x0001e014)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_RMSK                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ENABLE_BMSK                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_RX0_TEST_ENABLE_SHFT                                                     0x0

#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ADDR                                                    (LPASS_CSR_REG_BASE      + 0x0001e018)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_OFFS                                                    (LPASS_CSR_REG_BASE_OFFS + 0x0001e018)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_I2S6_TEST_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ADDR                                                   (LPASS_CSR_REG_BASE      + 0x0001e01c)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_OFFS                                                   (LPASS_CSR_REG_BASE_OFFS + 0x0001e01c)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_RMSK                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ADDR, HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_IN)
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_SWRM_GP_TX0_1_TEST_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR                                         (LPASS_CSR_REG_BASE      + 0x0001f000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_OFFS                                         (LPASS_CSR_REG_BASE_OFFS + 0x0001f000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_RMSK                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_LPAIF_NPL_BYP_EN_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_LPAIF_NPL_BYP_EN_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_SEL_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL_SEL_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00020000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00020000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_LPAIF_NPL_BYP_EN_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_LPAIF_NPL_BYP_EN_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_SEL_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL_SEL_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00021000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00021000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_LPAIF_NPL_BYP_EN_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_LPAIF_NPL_BYP_EN_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_SEL_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL_SEL_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR                                                (LPASS_CSR_REG_BASE      + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_OFFS                                                (LPASS_CSR_REG_BASE_OFFS + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_LPAIF_NPL_BYP_EN_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_LPAIF_NPL_BYP_EN_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_SEL_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL_SEL_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR                                               (LPASS_CSR_REG_BASE      + 0x00024000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_OFFS                                               (LPASS_CSR_REG_BASE_OFFS + 0x00024000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_RMSK                                                      0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_LPAIF_NPL_BYP_EN_BMSK                                     0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_LPAIF_NPL_BYP_EN_SHFT                                     0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_SEL_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL_SEL_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_ADDR                                                         (LPASS_CSR_REG_BASE      + 0x00025000)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_OFFS                                                         (LPASS_CSR_REG_BASE_OFFS + 0x00025000)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_HW_CTL_BMSK                                                         0x2
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_HW_CTL_SHFT                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_CGCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ADDR                                                   (LPASS_CSR_REG_BASE      + 0x00025004)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_OFFS                                                   (LPASS_CSR_REG_BASE_OFFS + 0x00025004)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_RMSK                                                          0x7
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                                           0x7
#define HWIO_LPASS_AUDIO_CORE_LCC_CSR_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                                           0x0

/*----------------------------------------------------------------------------
 * MODULE: LPASS_CC
 *--------------------------------------------------------------------------*/

#define LPASS_CC_REG_BASE                                                                                    (LPASS_BASE      + 0x00c00000)
#define LPASS_CC_REG_BASE_OFFS                                                                               0x00c00000

#define HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00000000)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RMSK                                                                    0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR, HWIO_LPASS_LPAAUDIO_PLL_MODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_MODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_MODE_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_DET_BMSK                                                       0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_DET_SHFT                                                             0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                                    0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                                          0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS29_22_BMSK                                                  0x3fc00000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS29_22_SHFT                                                        0x16
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                                   0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                       0x15
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                                     0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                         0x14
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                        0xfc000
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                            0xe
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                         0x3f00
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                            0x8
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS7_4_BMSK                                                          0xf0
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_RESERVE_BITS7_4_SHFT                                                           0x4
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_PLLTEST_BMSK                                                               0x8
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_PLLTEST_SHFT                                                               0x3
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_RESET_N_BMSK                                                               0x4
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_RESET_N_SHFT                                                               0x2
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BYPASSNL_BMSK                                                              0x2
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_BYPASSNL_SHFT                                                              0x1
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_OUTCTRL_BMSK                                                               0x1
#define HWIO_LPASS_LPAAUDIO_PLL_MODE_PLL_OUTCTRL_SHFT                                                               0x0

#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00000004)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00000004)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RMSK                                                                   0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RESERVE_BITS_31_8_BMSK                                                 0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_RESERVE_BITS_31_8_SHFT                                                        0x8
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_PLL_L_BMSK                                                                   0xff
#define HWIO_LPASS_LPAAUDIO_PLL_L_VAL_PLL_L_SHFT                                                                    0x0

#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00000008)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00000008)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RMSK                                                               0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RESERVE_BITS_31_16_BMSK                                            0xffff0000
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_RESERVE_BITS_31_16_SHFT                                                  0x10
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_PLL_ALPHA_BMSK                                                         0xffff
#define HWIO_LPASS_LPAAUDIO_PLL_ALPHA_VAL_PLL_ALPHA_SHFT                                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x0000000c)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OFFS                                                                (LPASS_CC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BITS31_28_BMSK                                              0xf0000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BITS31_28_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_SSC_EN_BMSK                                                          0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_SSC_EN_SHFT                                                               0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT26_BMSK                                                   0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT26_SHFT                                                        0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_MODE_BMSK                                                      0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_MODE_SHFT                                                           0x19
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_EN_BMSK                                                        0x1000000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ALPHA_EN_SHFT                                                             0x18
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_FREQ_STEP_BMSK                                                    0x800000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_FREQ_STEP_SHFT                                                        0x17
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_CONV_TIME_BUDGET_BMSK                                             0x400000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_CONV_TIME_BUDGET_SHFT                                                 0x16
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_DATA_LATCH_EDGE_BMSK                                              0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_DATA_LATCH_EDGE_SHFT                                                  0x15
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_POST_DIV_EN_BMSK                                                  0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DCO_POST_DIV_EN_SHFT                                                      0x14
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT19_BMSK                                                     0x80000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT19_SHFT                                                        0x13
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_L_VAL_CHANGE_TM_EN_BMSK                                                0x40000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_L_VAL_CHANGE_TM_EN_SHFT                                                   0x12
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OLC_EXIT_COUNT_BMSK                                                    0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OLC_EXIT_COUNT_SHFT                                                       0x11
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_STATE_SYNC_DIS_BMSK                                                0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_STATE_SYNC_DIS_SHFT                                                   0x10
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_LOCK_SYNC_DIS_BMSK                                                  0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ADC_LOCK_SYNC_DIS_SHFT                                                     0xf
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_BYPASS_CFA_FFA_BMSK                                               0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_BYPASS_CFA_FFA_SHFT                                                  0xe
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_CFA_MIN_TIME_DIS_BMSK                                                   0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_CFA_MIN_TIME_DIS_SHFT                                                      0xd
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PRE_DIV2_EN_BMSK                                                        0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PRE_DIV2_EN_SHFT                                                           0xc
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_BIT6_BMSK                                                      0x800
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_BIT6_SHFT                                                        0xb
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_2_3_BMSK                                                       0x400
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_DROOP_DIV_2_3_SHFT                                                         0xa
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_POSTDIV_CTL_BMSK                                                         0x300
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_POSTDIV_CTL_SHFT                                                           0x8
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                                     0x80
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                                      0x7
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ANALOG_DROOP_ENABLE_BMSK                                                  0x40
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_ANALOG_DROOP_ENABLE_SHFT                                                   0x6
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT_5_BMSK                                                        0x20
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_RESERVE_BIT_5_SHFT                                                         0x5
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                                                       0x10
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                                                        0x4
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                                                       0x8
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                                                       0x3
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                                                        0x4
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                                                        0x2
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                                                         0x1
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                                                        0x1
#define HWIO_LPASS_LPAAUDIO_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                                                        0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00000010)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x00000010)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_COMM_VOL_ADJ_BMSK                                             0xc0000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_COMM_VOL_ADJ_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_REF_BMSK                                                      0x30000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_REF_SHFT                                                            0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_DEL_BMSK                                                       0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_SAR_DEL_SHFT                                                            0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_CHP_BS_DEL_BMSK                                                    0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_CHP_BS_DEL_SHFT                                                         0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_REF_TRIM_BMSK                                                      0x3800000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_REF_TRIM_SHFT                                                           0x17
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_DCO_ADD_EN_BMSK                                                     0x400000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_DCO_ADD_EN_SHFT                                                         0x16
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_FBC_ALPHA_CAL_SEL_BMSK                                              0x300000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_FBC_ALPHA_CAL_SEL_SHFT                                                  0x14
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_TDL_CONTROL_BMSK                                                     0xfc000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_TDL_CONTROL_SHFT                                                         0xe
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFA_MSB_BMSK                                                          0x3c00
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFA_MSB_SHFT                                                             0xa
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_TM_SEL_BMSK                                                        0x300
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_TM_SEL_SHFT                                                          0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_TM_SEL_BMSK                                                         0xc0
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_TM_SEL_SHFT                                                          0x6
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_FFA_SEL_BMSK                                                        0x30
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_P_FFA_SEL_SHFT                                                         0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_FFA_SEL_BMSK                                                         0xc
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_K_I_FFA_SEL_SHFT                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFD_DZSEL_BMSK                                                           0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_PFD_DZSEL_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00000014)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00000014)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_FORCE_CFA_MODE_BMSK                                             0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_FORCE_CFA_MODE_SHFT                                                   0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_BYPASS_DIS_BMSK                                             0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_BYPASS_DIS_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_EN_EDGE_ANA_EN_BMSK                                       0x20000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_EN_EDGE_ANA_EN_SHFT                                             0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_DIS_BMSK                                              0x10000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_DIS_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CAL_DIV4_BMSK                                                0x8000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CAL_DIV4_SHFT                                                     0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CONFIG_CTL_BMSK                                              0x7000000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_CONFIG_CTL_SHFT                                                   0x18
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_EN_MAIN_AUX_BMSK                                              0x800000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TDL_EN_MAIN_AUX_SHFT                                                  0x17
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TIE_CORRECTION_SETTING_BMSK                                       0x780000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_TIE_CORRECTION_SETTING_SHFT                                           0x13
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_CONT_BMSK                                                0x7e000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DROOP_DIV_CONT_SHFT                                                    0xd
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_CTL_OVRD_BMSK                                              0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_CTL_OVRD_SHFT                                                 0xc
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CFA_GLITCH_DET_BYP_BMSK                                              0x800
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CFA_GLITCH_DET_BYP_SHFT                                                0xb
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_ADJ_BMSK                                                    0x7c0
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_DUTY_CYC_ADJ_SHFT                                                      0x6
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTP_DUTY_CYC_CTL_BMSK                                                0x20
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_OUTP_DUTY_CYC_CTL_SHFT                                                 0x5
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_DMET_BMSK                                                  0x10
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_DMET_SHFT                                                   0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_SINGLE_DMET_BMSK                                            0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_COUNTER_EN_SINGLE_DMET_SHFT                                            0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_LOW_JIT_MODE_EN_BMSK                                                   0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_LOW_JIT_MODE_EN_SHFT                                                   0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CHR_PUMP_REF_CUR_ADJ_BMSK                                              0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_CHR_PUMP_REF_CUR_ADJ_SHFT                                              0x0

#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000018)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_OFFS                                                                (LPASS_CC_REG_BASE_OFFS + 0x00000018)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DFINE_OVER_VAL_BMSK                                                 0xe0000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DFINE_OVER_VAL_SHFT                                                       0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_THR_DFINE_OVR_BMSK                                             0x18000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_THR_DFINE_OVR_SHFT                                                   0x1b
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_TIM_DFINE_OVR_BMSK                                              0x4000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_LOCK_TIM_DFINE_OVR_SHFT                                                   0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_D_FINE_OVERRIDE_BMSK                                                 0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_D_FINE_OVERRIDE_SHFT                                                      0x19
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_VALUE_BMSK                                            0x1fc0000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_VALUE_SHFT                                                 0x12
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_BMSK                                                    0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_G_BWC_OVERRIDE_SHFT                                                       0x11
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_STAY_IN_CFA_BMSK                                                       0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_STAY_IN_CFA_SHFT                                                          0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_FFA_BMSK                                                           0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_FFA_SHFT                                                              0xf
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_CFA_BMSK                                                           0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_SKIP_CFA_SHFT                                                              0xe
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_FFA_TM_ACCUM_BMSK                                                 0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_FFA_TM_ACCUM_SHFT                                                    0xd
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_CFA_ACCUM_BMSK                                                    0x1000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_RESET_CFA_ACCUM_SHFT                                                       0xc
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INP_REF_CLK_SEL_BMSK                                                     0x800
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_INP_REF_CLK_SEL_SHFT                                                       0xb
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                                        0x400
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                                          0xa
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_PLL_STATUS_SEL_BMSK                                                      0x3c0
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_PLL_STATUS_SEL_SHFT                                                        0x6
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DTEST_SEL_BMSK                                                            0x20
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_DTEST_SEL_SHFT                                                             0x5
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_AMP_BYPASS_BMSK                                                     0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_AMP_BYPASS_SHFT                                                      0x4
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_NMO_SEL_BMSK                                                         0xc
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST_NMO_SEL_SHFT                                                         0x2
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST1_EN_BMSK                                                             0x2
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST1_EN_SHFT                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST0_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_ATEST0_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0000001c)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_LV_TEST_SEL_BMSK                                                  0x80000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_LV_TEST_SEL_SHFT                                                        0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_PWR_DN_BMSK                                             0x40000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_PWR_DN_SHFT                                                   0x1e
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENABLE_FBC_BMSK                                                   0x20000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENABLE_FBC_SHFT                                                         0x1d
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_VAL_BMSK                                           0x1c000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_VAL_SHFT                                                 0x1a
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_BMSK                                                0x2000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_PTUNE_OVERRIDE_SHFT                                                     0x19
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DIV_DC_SEL_BMSK                                                    0x1000000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DIV_DC_SEL_SHFT                                                         0x18
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_CFG_BMSK                                                       0xc00000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_CFG_SHFT                                                           0x16
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_EN_BMSK                                                        0x200000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NGEN_EN_SHFT                                                            0x15
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_BIAS_BYP_BMSK                                             0x100000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CHRG_PUMP_BIAS_BYP_SHFT                                                 0x14
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DEM_DCO_DAC_EN_BMSK                                                  0x80000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DEM_DCO_DAC_EN_SHFT                                                     0x13
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NMOSC_EN_BMSK                                                        0x40000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_NMOSC_EN_SHFT                                                           0x12
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RESERVE_BIT49_BMSK                                                   0x20000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_RESERVE_BIT49_SHFT                                                      0x11
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_STATUS_OUT_TRAIN_SEQ_BMSK                                            0x10000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_STATUS_OUT_TRAIN_SEQ_SHFT                                               0x10
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADC_TEST_CLK_SEL_BMSK                                                 0x8000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ADC_TEST_CLK_SEL_SHFT                                                    0xf
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CTUNE_ADD_GLITCH_FILT_BMSK                                            0x4000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_CTUNE_ADD_GLITCH_FILT_SHFT                                               0xe
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_SEL_BMSK                                                        0x2000
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_SEL_SHFT                                                           0xd
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_VAL_BMSK                                                        0x1ff0
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_D_EXT_VAL_SHFT                                                           0x4
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENTER_OLC_MODE_SEL_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_ENTER_OLC_MODE_SEL_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DFINE_OVER_VAL_BMSK                                                      0x7
#define HWIO_LPASS_LPAAUDIO_PLL_TEST_CTL_U_DFINE_OVER_VAL_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00000020)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00000020)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RMSK                                                         0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RESERVE_BITS_31_8_BMSK                                       0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_RESERVE_BITS_31_8_SHFT                                              0x8
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_PLL_SCC_DELTA_ALPHA_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_DELTA_ALPHA_PLL_SCC_DELTA_ALPHA_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00000024)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00000024)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RMSK                                                         0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR, HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RESERVE_BITS_31_8_BMSK                                       0xffffff00
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_RESERVE_BITS_31_8_SHFT                                              0x8
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_PLL_SCC_UPDATE_RATE_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_PLL_SSC_UPDATE_RATE_PLL_SCC_UPDATE_RATE_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00000028)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00000028)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RMSK                                                               0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_31_15_BMSK                                            0xffff8000
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_31_15_SHFT                                                   0xf
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_CODE_BMSK                                                        0x7f00
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_CODE_SHFT                                                           0x8
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_7_1_BMSK                                                    0xfe
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_RESERVE_BITS_7_1_SHFT                                                     0x1
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_PLL_DROOP_CTL_DROOP_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000002c)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0000002c)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_RMSK                                                                  0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR, HWIO_LPASS_LPAAUDIO_PLL_STATUS_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_STATUS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_STATUS_31_0_BMSK                                                      0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_STATUS_STATUS_31_0_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000030)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_OFFS                                                                (LPASS_CC_REG_BASE_OFFS + 0x00000030)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                                             0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                                                    0x0

#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00000034)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_OFFS                                                                (LPASS_CC_REG_BASE_OFFS + 0x00000034)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RESERVE_BITS_31_12_BMSK                                             0xfffff000
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_RESERVE_BITS_31_12_SHFT                                                    0xc
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_BIST_CTL_BMSK                                                            0xfff
#define HWIO_LPASS_LPAAUDIO_PLL_BIST_CTL_BIST_CTL_SHFT                                                              0x0

#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00000038)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x00000038)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_RMSK                                                                    0x1f
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR, HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX2_CGC_EN_BMSK                                                    0x10
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX2_CGC_EN_SHFT                                                     0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_SVS_CGC_EN_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_SVS_CGC_EN_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_CGC_EN_BMSK                                                      0x4
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_AUX_CGC_EN_SHFT                                                      0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_BMSK                                                 0x2
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_SHFT                                                 0x1
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_BMSK                                                     0x1
#define HWIO_LPASS_LPAAUDIO_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_SHFT                                                     0x0

#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0000003c)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x0000003c)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_RMSK                                                             0x3
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_CLK_DIV_BMSK                                                     0x3
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_MAIN_CDIVR_CLK_DIV_SHFT                                                     0x0

#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00000040)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x00000040)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_RMSK                                                              0xf
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_CLK_DIV_BMSK                                                      0xf
#define HWIO_LPASS_LCC_LPAAUDIO_PLL_OUT_AUX_CDIVR_CLK_DIV_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001004)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00001004)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_RMSK                                                                   0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_PLL_L_BMSK                                                             0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_L_VAL_PLL_L_SHFT                                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR                                                           (LPASS_CC_REG_BASE      + 0x00001008)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_OFFS                                                           (LPASS_CC_REG_BASE_OFFS + 0x00001008)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_RMSK                                                               0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                                     0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                                        0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0000100c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x0000100c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                                          0xfff80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                                                0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                                                 0x70000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                                    0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                                             0xf000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                                                0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                                             0xf00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                                               0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                                 0x80
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                                  0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                                  0x60
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                                   0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_TEST_BMSK                                                      0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_TEST_SHFT                                                       0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS3_BMSK                                                     0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_RESERVE_BITS3_SHFT                                                     0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_ODD_BMSK                                                        0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_ODD_SHFT                                                        0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                                       0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                                       0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001010)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00001010)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_BMSK                                        0xffc00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_RESERVE_BITS31_22_SHFT                                              0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_BMSK                              0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_SHFT                                  0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_BMSK                                          0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_SHFT                                              0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_BMSK                                         0x80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_SHFT                                            0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                                         0x40000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                                            0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                                         0x20000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                                            0x11
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                                          0x10000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                                             0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                                              0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                                                 0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                                           0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                                              0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                                               0x2000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                                  0xd
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                                        0x1800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                                           0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                                        0x400
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                                          0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                                               0x3e0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                                                 0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                                                 0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                                  0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                                      0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                                      0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                                          0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                                          0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001014)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00001014)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                                       0xfc000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                                             0x1a
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                                      0x3c00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK                              0x3c0000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                                  0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK                               0x3c000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                                   0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                                      0x3800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                                         0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                                     0x700
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                                       0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                                           0xf0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                                            0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                                    0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                                    0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00001018)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00001018)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RMSK                                                        0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                                               0xfff00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                                     0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_BMSK                                        0xc0000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_RESERVE_BITS_19_18_SHFT                                           0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                                              0x30000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                                 0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_BMSK                                         0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_SHFT                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_BMSK                                           0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BYPASS_DROOP_DIV_SHFT                                              0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                                          0x3000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                                             0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                                         0xc00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                                           0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                                             0x380
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                                               0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                                  0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                                   0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                                         0x30
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                                          0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                                         0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                                         0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                                              0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                                              0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                                       0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                                       0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0000101c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x0000101c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                                    0xc0000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                                          0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK                                0x30000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                                      0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                                       0x8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                                            0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_FINE_FCW_BMSK                                                    0x7e00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_FINE_FCW_SHFT                                                         0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                                            0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                                                0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_COARSE_FCW_BMSK                                                    0xff000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_COARSE_FCW_SHFT                                                        0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                                             0x800
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                                               0xb
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                                              0x700
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                                                0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                                              0x80
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                                               0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                                     0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                                      0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VREF_TEST_BMSK                                                     0x20
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VREF_TEST_SHFT                                                      0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VDCO_TEST_BMSK                                                     0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_VDCO_TEST_SHFT                                                      0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                                               0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                                               0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_IVCO_TEST_BMSK                                                      0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_EN_IVCO_TEST_SHFT                                                      0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST1_EN_BMSK                                                         0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST1_EN_SHFT                                                         0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST0_EN_BMSK                                                         0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_ATEST0_EN_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00001020)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00001020)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_RMSK                                                          0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                                      0x80000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                                            0x1f
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                                         0x40000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                                               0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                                          0x20000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                                                0x1d
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                                         0x10000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                                               0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                                    0x8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                                         0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK                                 0x6000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                                      0x19
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                                      0x1c00000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                                       0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                                           0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                                              0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                                  0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                                         0x80000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                                            0x13
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                                     0x40000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                                        0x12
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                                                 0x30000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                                    0x10
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                                  0x8000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                                     0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIS_AMP_STARTUP_BMSK                                              0x4000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DIS_AMP_STARTUP_SHFT                                                 0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                                               0x2000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                                  0xd
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                                             0x1000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                                                0xc
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                                   0xc00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                                     0xa
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                                     0x380
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                                       0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                                   0x40
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                                    0x6
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                                   0x20
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                                    0x5
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                                               0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                                                0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                                   0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                                   0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                                  0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                                  0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                                  0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                                  0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR                                                                (LPASS_CC_REG_BASE      + 0x00001000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_OFFS                                                                (LPASS_CC_REG_BASE_OFFS + 0x00001000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RMSK                                                                0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_BMSK                                                   0x80000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_SHFT                                                         0x1f
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                                0x40000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                                      0x1e
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACK_LATCH_BMSK                                                  0x20000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_ACK_LATCH_SHFT                                                        0x1d
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                                              0x10000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                                    0x1c
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS27_25_BMSK                                               0xe000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS27_25_SHFT                                                    0x19
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_FSM_LEGACY_MODE_BMSK                                                 0x1000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_FSM_LEGACY_MODE_SHFT                                                      0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                                       0x800000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                                           0x17
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_UPDATE_BMSK                                                       0x400000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_UPDATE_SHFT                                                           0x16
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                               0x200000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                                   0x15
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                                 0x100000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                                     0x14
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_BIAS_COUNT_BMSK                                                    0xfc000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_BIAS_COUNT_SHFT                                                        0xe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_COUNT_BMSK                                                     0x3f00
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_LOCK_COUNT_SHFT                                                        0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS7_3_BMSK                                                      0xf8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BITS7_3_SHFT                                                       0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_RESET_N_BMSK                                                           0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_RESET_N_SHFT                                                           0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BIT1_BMSK                                                          0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_RESERVE_BIT1_SHFT                                                          0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_OUTCTRL_BMSK                                                           0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_MODE_PLL_OUTCTRL_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR                                                              (LPASS_CC_REG_BASE      + 0x00001024)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x00001024)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_RMSK                                                              0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_STATUS_31_0_BMSK                                                  0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATUS_STATUS_31_0_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00001028)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00001028)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RMSK                                                            0xffffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_BMSK                                          0xf8000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_RESERVE_BITS31_27_SHFT                                                0x1b
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_PCAL_BMSK                                                    0x7000000
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_PCAL_SHFT                                                         0x18
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_ACC_BMSK                                                      0xffffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FREQ_CTL_FCW_ACC_SHFT                                                           0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0000102c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x0000102c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_RMSK                                                                     0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_PLL_OPMODE_BMSK                                                          0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_OPMODE_PLL_OPMODE_SHFT                                                          0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001030)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00001030)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_RMSK                                                                      0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_PLL_STATE_BMSK                                                            0x7
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_STATE_PLL_STATE_SHFT                                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00001034)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00001034)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_RMSK                                                                     0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_CODE_BMSK                                                          0xfe
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_CODE_SHFT                                                           0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_EN_BMSK                                                             0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_DROOP_DROOP_EN_SHFT                                                             0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00001038)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00001038)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_RMSK                                                                0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                                   0xffff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                                      0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0000103c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0000103c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_RMSK                                                                     0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_OUTPUTS_BMSK                                                       0xf0
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_OUTPUTS_SHFT                                                        0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_INPUTS_BMSK                                                         0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SPARE_SPARE_INPUTS_SHFT                                                         0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00001040)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00001040)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_RMSK                                                           0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                                           0xff
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00001048)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x00001048)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_RMSK                                                              0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                                                0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00001044)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00001044)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_RMSK                                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                                            0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                                            0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR                                                          (LPASS_CC_REG_BASE      + 0x0000104c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x0000104c)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_RMSK                                                                 0xf
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_BMSK                                                  0x8
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_ODD_CGC_EN_SHFT                                                  0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_BMSK                                                 0x4
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_EVEN_CGC_EN_SHFT                                                 0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_BMSK                                             0x2
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_SVS_CGC_EN_SHFT                                             0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_BMSK                                                 0x1
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_CLK_CGC_EN_CLK_MAIN_CGC_EN_SHFT                                                 0x0

#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00001050)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_OFFS                                                  (LPASS_CC_REG_BASE_OFFS + 0x00001050)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_RMSK                                                         0x3
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR, HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_RMSK)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR, m)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR,v)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_ADDR,m,v,HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_IN)
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_CLK_DIV_BMSK                                                 0x3
#define HWIO_LPASS_LCC_LPAAUDIO_DIG_PLL_OUT_MAIN_CDIVR_CLK_DIV_SHFT                                                 0x0

#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00002000)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00002000)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_RMSK                                                                0x3
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                                0x3
#define HWIO_LPASS_LPAAUDIO_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                                0x0

#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00003000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00003000)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_RMSK                                                            0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                            0x3
#define HWIO_LPASS_LPAAUDIO_DIG_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                            0x0

#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00004000)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00004000)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_RMSK                                                                 0x1
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR, HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_RMSK)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR, m)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR,v)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_ADDR,m,v,HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_IN)
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_BMSK                                                 0x1
#define HWIO_LPASS_QDSP6SS_PLL_REF_CLK_SRC_SEL_PLL_CLK_SRC_SEL_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0000b008)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_OFFS                                                             (LPASS_CC_REG_BASE_OFFS + 0x0000b008)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_RMSK                                                             0x80000003
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_SAMPLING_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000f000)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0000f000)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_LPAIF_SPKR_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0000f004)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0000f004)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_LPAIF_SPKR_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_SPKR_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f008)
#define HWIO_LPASS_LPAIF_SPKR_M_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0000f008)
#define HWIO_LPASS_LPAIF_SPKR_M_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_M_ADDR, HWIO_LPASS_LPAIF_SPKR_M_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_M_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_M_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_M_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_M_IN)
#define HWIO_LPASS_LPAIF_SPKR_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_LPAIF_SPKR_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_LPAIF_SPKR_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f00c)
#define HWIO_LPASS_LPAIF_SPKR_N_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0000f00c)
#define HWIO_LPASS_LPAIF_SPKR_N_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_N_ADDR, HWIO_LPASS_LPAIF_SPKR_N_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_N_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_N_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_N_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_N_IN)
#define HWIO_LPASS_LPAIF_SPKR_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_LPAIF_SPKR_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SPKR_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0000f010)
#define HWIO_LPASS_LPAIF_SPKR_D_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0000f010)
#define HWIO_LPASS_LPAIF_SPKR_D_RMSK                                                                               0xff
#define HWIO_LPASS_LPAIF_SPKR_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_D_ADDR, HWIO_LPASS_LPAIF_SPKR_D_RMSK)
#define HWIO_LPASS_LPAIF_SPKR_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SPKR_D_ADDR, m)
#define HWIO_LPASS_LPAIF_SPKR_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SPKR_D_ADDR,v)
#define HWIO_LPASS_LPAIF_SPKR_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SPKR_D_ADDR,m,v,HWIO_LPASS_LPAIF_SPKR_D_IN)
#define HWIO_LPASS_LPAIF_SPKR_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_LPAIF_SPKR_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0000f014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_OFFS                                                 (LPASS_CC_REG_BASE_OFFS + 0x0000f014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_RMSK                                                 0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_HW_CTL_BMSK                                                 0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_HW_CTL_SHFT                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR                                                (LPASS_CC_REG_BASE      + 0x0000f018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_OFFS                                                (LPASS_CC_REG_BASE_OFFS + 0x0000f018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_RMSK                                                0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_DIV_BMSK                                         0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_DIV_SHFT                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR                                                (LPASS_CC_REG_BASE      + 0x0000f01c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_OFFS                                                (LPASS_CC_REG_BASE_OFFS + 0x0000f01c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_RMSK                                                0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR                                           (LPASS_CC_REG_BASE      + 0x0000f020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_OFFS                                           (LPASS_CC_REG_BASE_OFFS + 0x0000f020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_RMSK                                           0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                          0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                             0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                       0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                        0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                   0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_EN_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EXT_CLK_DETECT_IRQ_EN_SHFT                                           0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR                                                  (LPASS_CC_REG_BASE      + 0x0000f024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_OFFS                                                  (LPASS_CC_REG_BASE_OFFS + 0x0000f024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_RMSK                                                         0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_EXT_CLK_BMSK                                             0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_EXT_CLK_SHFT                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_INT_CLK_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_CLK_INV_INV_INT_CLK_SHFT                                             0x0

#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00010000)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00010000)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_PRI_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_PRI_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00010004)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00010004)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_PRI_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_PRI_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_PRI_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00010008)
#define HWIO_LPASS_LPAIF_PRI_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00010008)
#define HWIO_LPASS_LPAIF_PRI_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_M_ADDR, HWIO_LPASS_LPAIF_PRI_M_RMSK)
#define HWIO_LPASS_LPAIF_PRI_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_M_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_M_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_M_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_M_IN)
#define HWIO_LPASS_LPAIF_PRI_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_PRI_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_PRI_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001000c)
#define HWIO_LPASS_LPAIF_PRI_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001000c)
#define HWIO_LPASS_LPAIF_PRI_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_N_ADDR, HWIO_LPASS_LPAIF_PRI_N_RMSK)
#define HWIO_LPASS_LPAIF_PRI_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_N_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_N_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_N_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_N_IN)
#define HWIO_LPASS_LPAIF_PRI_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_PRI_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_PRI_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00010010)
#define HWIO_LPASS_LPAIF_PRI_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00010010)
#define HWIO_LPASS_LPAIF_PRI_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_PRI_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_D_ADDR, HWIO_LPASS_LPAIF_PRI_D_RMSK)
#define HWIO_LPASS_LPAIF_PRI_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PRI_D_ADDR, m)
#define HWIO_LPASS_LPAIF_PRI_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PRI_D_ADDR,v)
#define HWIO_LPASS_LPAIF_PRI_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PRI_D_ADDR,m,v,HWIO_LPASS_LPAIF_PRI_D_IN)
#define HWIO_LPASS_LPAIF_PRI_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_PRI_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00010018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x00010018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001001c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x0001001c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00010020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00010020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00010024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_OFFS                                                  (LPASS_CC_REG_BASE_OFFS + 0x00010024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PRI_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00011000)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00011000)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_SEC_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_SEC_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00011004)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00011004)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_SEC_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_SEC_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_SEC_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00011008)
#define HWIO_LPASS_LPAIF_SEC_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00011008)
#define HWIO_LPASS_LPAIF_SEC_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_M_ADDR, HWIO_LPASS_LPAIF_SEC_M_RMSK)
#define HWIO_LPASS_LPAIF_SEC_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_M_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_M_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_M_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_M_IN)
#define HWIO_LPASS_LPAIF_SEC_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_SEC_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_SEC_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001100c)
#define HWIO_LPASS_LPAIF_SEC_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001100c)
#define HWIO_LPASS_LPAIF_SEC_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_N_ADDR, HWIO_LPASS_LPAIF_SEC_N_RMSK)
#define HWIO_LPASS_LPAIF_SEC_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_N_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_N_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_N_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_N_IN)
#define HWIO_LPASS_LPAIF_SEC_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_SEC_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_SEC_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00011010)
#define HWIO_LPASS_LPAIF_SEC_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00011010)
#define HWIO_LPASS_LPAIF_SEC_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_SEC_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_D_ADDR, HWIO_LPASS_LPAIF_SEC_D_RMSK)
#define HWIO_LPASS_LPAIF_SEC_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_SEC_D_ADDR, m)
#define HWIO_LPASS_LPAIF_SEC_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_SEC_D_ADDR,v)
#define HWIO_LPASS_LPAIF_SEC_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_SEC_D_ADDR,m,v,HWIO_LPASS_LPAIF_SEC_D_IN)
#define HWIO_LPASS_LPAIF_SEC_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_SEC_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00011018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x00011018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001101c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x0001101c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00011020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00011020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00011024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_OFFS                                                  (LPASS_CC_REG_BASE_OFFS + 0x00011024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_SEC_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00012000)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00012000)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_TER_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_TER_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_LPAIF_TER_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00012004)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00012004)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_TER_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_TER_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_LPAIF_TER_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_TER_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00012008)
#define HWIO_LPASS_LPAIF_TER_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00012008)
#define HWIO_LPASS_LPAIF_TER_M_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_M_ADDR, HWIO_LPASS_LPAIF_TER_M_RMSK)
#define HWIO_LPASS_LPAIF_TER_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_M_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_M_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_M_ADDR,m,v,HWIO_LPASS_LPAIF_TER_M_IN)
#define HWIO_LPASS_LPAIF_TER_M_M_BMSK                                                                            0xffff
#define HWIO_LPASS_LPAIF_TER_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_LPAIF_TER_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001200c)
#define HWIO_LPASS_LPAIF_TER_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001200c)
#define HWIO_LPASS_LPAIF_TER_N_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_N_ADDR, HWIO_LPASS_LPAIF_TER_N_RMSK)
#define HWIO_LPASS_LPAIF_TER_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_N_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_N_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_N_ADDR,m,v,HWIO_LPASS_LPAIF_TER_N_IN)
#define HWIO_LPASS_LPAIF_TER_N_NOT_N_MINUS_M_BMSK                                                                0xffff
#define HWIO_LPASS_LPAIF_TER_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_LPAIF_TER_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00012010)
#define HWIO_LPASS_LPAIF_TER_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00012010)
#define HWIO_LPASS_LPAIF_TER_D_RMSK                                                                              0xffff
#define HWIO_LPASS_LPAIF_TER_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_D_ADDR, HWIO_LPASS_LPAIF_TER_D_RMSK)
#define HWIO_LPASS_LPAIF_TER_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_TER_D_ADDR, m)
#define HWIO_LPASS_LPAIF_TER_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_TER_D_ADDR,v)
#define HWIO_LPASS_LPAIF_TER_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_TER_D_ADDR,m,v,HWIO_LPASS_LPAIF_TER_D_IN)
#define HWIO_LPASS_LPAIF_TER_D_NOT_2D_BMSK                                                                       0xffff
#define HWIO_LPASS_LPAIF_TER_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x00012018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x00012018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_RMSK                                                       0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_DIV_BMSK                                                0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_DIV_SHFT                                                     0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR                                                       (LPASS_CC_REG_BASE      + 0x0001201c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_OFFS                                                       (LPASS_CC_REG_BASE_OFFS + 0x0001201c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_RMSK                                                       0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_HW_CTL_BMSK                                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_HW_CTL_SHFT                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00012020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00012020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_RMSK                                                                0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_EXT_CLK_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_EXT_CLK_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_INT_CLK_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_CLK_INV_INV_INT_CLK_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR                                                  (LPASS_CC_REG_BASE      + 0x00012024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_OFFS                                                  (LPASS_CC_REG_BASE_OFFS + 0x00012024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_RMSK                                                  0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                    0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                       0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                              0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                               0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_EN_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_TER_EXT_CLK_DETECT_IRQ_EN_SHFT                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00013000)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x00013000)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_LPAIF_QUAD_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00013004)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x00013004)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_LPAIF_QUAD_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_QUAD_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00013008)
#define HWIO_LPASS_LPAIF_QUAD_M_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x00013008)
#define HWIO_LPASS_LPAIF_QUAD_M_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_M_ADDR, HWIO_LPASS_LPAIF_QUAD_M_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_M_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_M_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_M_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_M_IN)
#define HWIO_LPASS_LPAIF_QUAD_M_M_BMSK                                                                           0xffff
#define HWIO_LPASS_LPAIF_QUAD_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_LPAIF_QUAD_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0001300c)
#define HWIO_LPASS_LPAIF_QUAD_N_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0001300c)
#define HWIO_LPASS_LPAIF_QUAD_N_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_N_ADDR, HWIO_LPASS_LPAIF_QUAD_N_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_N_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_N_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_N_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_N_IN)
#define HWIO_LPASS_LPAIF_QUAD_N_NOT_N_MINUS_M_BMSK                                                               0xffff
#define HWIO_LPASS_LPAIF_QUAD_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_LPAIF_QUAD_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00013010)
#define HWIO_LPASS_LPAIF_QUAD_D_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x00013010)
#define HWIO_LPASS_LPAIF_QUAD_D_RMSK                                                                             0xffff
#define HWIO_LPASS_LPAIF_QUAD_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_D_ADDR, HWIO_LPASS_LPAIF_QUAD_D_RMSK)
#define HWIO_LPASS_LPAIF_QUAD_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_QUAD_D_ADDR, m)
#define HWIO_LPASS_LPAIF_QUAD_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_QUAD_D_ADDR,v)
#define HWIO_LPASS_LPAIF_QUAD_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_QUAD_D_ADDR,m,v,HWIO_LPASS_LPAIF_QUAD_D_IN)
#define HWIO_LPASS_LPAIF_QUAD_D_NOT_2D_BMSK                                                                      0xffff
#define HWIO_LPASS_LPAIF_QUAD_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00013018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00013018)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_RMSK                                                      0x81ff0003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_DIV_BMSK                                               0x1ff0000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_DIV_SHFT                                                    0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0001301c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001301c)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00013020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00013020)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_RMSK                                                               0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_EXT_CLK_BMSK                                                   0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_EXT_CLK_SHFT                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_INT_CLK_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_CLK_INV_INV_INT_CLK_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x00013024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_OFFS                                                 (LPASS_CC_REG_BASE_OFFS + 0x00013024)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_QUAD_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_ATIME_CMD_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00015000)
#define HWIO_LPASS_ATIME_CMD_RCGR_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x00015000)
#define HWIO_LPASS_ATIME_CMD_RCGR_RMSK                                                                       0x800000f3
#define HWIO_LPASS_ATIME_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_CMD_RCGR_ADDR, HWIO_LPASS_ATIME_CMD_RCGR_RMSK)
#define HWIO_LPASS_ATIME_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_ATIME_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_ATIME_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_CMD_RCGR_ADDR,m,v,HWIO_LPASS_ATIME_CMD_RCGR_IN)
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_OFF_BMSK                                                              0x80000000
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_OFF_SHFT                                                                    0x1f
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_D_BMSK                                                                     0x80
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_D_SHFT                                                                      0x7
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_N_BMSK                                                                     0x40
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_N_SHFT                                                                      0x6
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_M_BMSK                                                                     0x20
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_M_SHFT                                                                      0x5
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                              0x10
#define HWIO_LPASS_ATIME_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                               0x4
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_EN_BMSK                                                                      0x2
#define HWIO_LPASS_ATIME_CMD_RCGR_ROOT_EN_SHFT                                                                      0x1
#define HWIO_LPASS_ATIME_CMD_RCGR_UPDATE_BMSK                                                                       0x1
#define HWIO_LPASS_ATIME_CMD_RCGR_UPDATE_SHFT                                                                       0x0

#define HWIO_LPASS_ATIME_CFG_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00015004)
#define HWIO_LPASS_ATIME_CFG_RCGR_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x00015004)
#define HWIO_LPASS_ATIME_CFG_RCGR_RMSK                                                                           0x771f
#define HWIO_LPASS_ATIME_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_CFG_RCGR_ADDR, HWIO_LPASS_ATIME_CFG_RCGR_RMSK)
#define HWIO_LPASS_ATIME_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_ATIME_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_ATIME_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_CFG_RCGR_ADDR,m,v,HWIO_LPASS_ATIME_CFG_RCGR_IN)
#define HWIO_LPASS_ATIME_CFG_RCGR_ALT_SRC_SEL_BMSK                                                               0x4000
#define HWIO_LPASS_ATIME_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                  0xe
#define HWIO_LPASS_ATIME_CFG_RCGR_MODE_BMSK                                                                      0x3000
#define HWIO_LPASS_ATIME_CFG_RCGR_MODE_SHFT                                                                         0xc
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_SEL_BMSK                                                                    0x700
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_SEL_SHFT                                                                      0x8
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_DIV_BMSK                                                                     0x1f
#define HWIO_LPASS_ATIME_CFG_RCGR_SRC_DIV_SHFT                                                                      0x0

#define HWIO_LPASS_ATIME_M_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x00015008)
#define HWIO_LPASS_ATIME_M_OFFS                                                                              (LPASS_CC_REG_BASE_OFFS + 0x00015008)
#define HWIO_LPASS_ATIME_M_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_M_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_M_ADDR, HWIO_LPASS_ATIME_M_RMSK)
#define HWIO_LPASS_ATIME_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_M_ADDR, m)
#define HWIO_LPASS_ATIME_M_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_M_ADDR,v)
#define HWIO_LPASS_ATIME_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_M_ADDR,m,v,HWIO_LPASS_ATIME_M_IN)
#define HWIO_LPASS_ATIME_M_M_BMSK                                                                                  0xff
#define HWIO_LPASS_ATIME_M_M_SHFT                                                                                   0x0

#define HWIO_LPASS_ATIME_N_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x0001500c)
#define HWIO_LPASS_ATIME_N_OFFS                                                                              (LPASS_CC_REG_BASE_OFFS + 0x0001500c)
#define HWIO_LPASS_ATIME_N_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_N_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_N_ADDR, HWIO_LPASS_ATIME_N_RMSK)
#define HWIO_LPASS_ATIME_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_N_ADDR, m)
#define HWIO_LPASS_ATIME_N_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_N_ADDR,v)
#define HWIO_LPASS_ATIME_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_N_ADDR,m,v,HWIO_LPASS_ATIME_N_IN)
#define HWIO_LPASS_ATIME_N_NOT_N_MINUS_M_BMSK                                                                      0xff
#define HWIO_LPASS_ATIME_N_NOT_N_MINUS_M_SHFT                                                                       0x0

#define HWIO_LPASS_ATIME_D_ADDR                                                                              (LPASS_CC_REG_BASE      + 0x00015010)
#define HWIO_LPASS_ATIME_D_OFFS                                                                              (LPASS_CC_REG_BASE_OFFS + 0x00015010)
#define HWIO_LPASS_ATIME_D_RMSK                                                                                    0xff
#define HWIO_LPASS_ATIME_D_IN          \
        in_dword_masked(HWIO_LPASS_ATIME_D_ADDR, HWIO_LPASS_ATIME_D_RMSK)
#define HWIO_LPASS_ATIME_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_ATIME_D_ADDR, m)
#define HWIO_LPASS_ATIME_D_OUT(v)      \
        out_dword(HWIO_LPASS_ATIME_D_ADDR,v)
#define HWIO_LPASS_ATIME_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_ATIME_D_ADDR,m,v,HWIO_LPASS_ATIME_D_IN)
#define HWIO_LPASS_ATIME_D_NOT_2D_BMSK                                                                             0xff
#define HWIO_LPASS_ATIME_D_NOT_2D_SHFT                                                                              0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR                                                         (LPASS_CC_REG_BASE      + 0x00015014)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_OFFS                                                         (LPASS_CC_REG_BASE_OFFS + 0x00015014)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_RMSK                                                         0x80000003
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_HW_CTL_BMSK                                                         0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_HW_CTL_SHFT                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016000)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00016000)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR, HWIO_LPASS_RESAMPLER_CMD_RCGR_RMSK)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CMD_RCGR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CMD_RCGR_IN)
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_RESAMPLER_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016004)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00016004)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR, HWIO_LPASS_RESAMPLER_CFG_RCGR_RMSK)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CFG_RCGR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CFG_RCGR_IN)
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_RESAMPLER_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_RESAMPLER_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00016008)
#define HWIO_LPASS_RESAMPLER_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00016008)
#define HWIO_LPASS_RESAMPLER_M_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_M_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_M_ADDR, HWIO_LPASS_RESAMPLER_M_RMSK)
#define HWIO_LPASS_RESAMPLER_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_M_ADDR, m)
#define HWIO_LPASS_RESAMPLER_M_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_M_ADDR,v)
#define HWIO_LPASS_RESAMPLER_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_M_ADDR,m,v,HWIO_LPASS_RESAMPLER_M_IN)
#define HWIO_LPASS_RESAMPLER_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_RESAMPLER_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_RESAMPLER_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001600c)
#define HWIO_LPASS_RESAMPLER_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001600c)
#define HWIO_LPASS_RESAMPLER_N_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_N_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_N_ADDR, HWIO_LPASS_RESAMPLER_N_RMSK)
#define HWIO_LPASS_RESAMPLER_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_N_ADDR, m)
#define HWIO_LPASS_RESAMPLER_N_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_N_ADDR,v)
#define HWIO_LPASS_RESAMPLER_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_N_ADDR,m,v,HWIO_LPASS_RESAMPLER_N_IN)
#define HWIO_LPASS_RESAMPLER_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_RESAMPLER_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00016010)
#define HWIO_LPASS_RESAMPLER_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00016010)
#define HWIO_LPASS_RESAMPLER_D_RMSK                                                                                0xff
#define HWIO_LPASS_RESAMPLER_D_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_D_ADDR, HWIO_LPASS_RESAMPLER_D_RMSK)
#define HWIO_LPASS_RESAMPLER_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_D_ADDR, m)
#define HWIO_LPASS_RESAMPLER_D_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_D_ADDR,v)
#define HWIO_LPASS_RESAMPLER_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_D_ADDR,m,v,HWIO_LPASS_RESAMPLER_D_IN)
#define HWIO_LPASS_RESAMPLER_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_RESAMPLER_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00016098)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00016098)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_RMSK                                                            0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00016014)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00016014)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RMSK                                                                       0xbfef
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR, HWIO_LPASS_RESAMPLER_CMD_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CMD_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_CMD_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RCG_SW_CTRL_BMSK                                                           0x8000
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_RCG_SW_CTRL_SHFT                                                              0xf
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_PERF_STATE_BMSK                                                         0x3800
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_PERF_STATE_SHFT                                                            0xb
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_OVERRIDE_BMSK                                                            0x400
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_SW_OVERRIDE_SHFT                                                              0xa
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                               0x200
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                                 0x9
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_FSM_STATE_BMSK                                                          0x1c0
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_FSM_STATE_SHFT                                                            0x6
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                          0x20
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                           0x5
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_CURR_PERF_STATE_BMSK                                                          0xe
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_CURR_PERF_STATE_SHFT                                                          0x1
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_EN_BMSK                                                                   0x1
#define HWIO_LPASS_RESAMPLER_CMD_DFSR_DFS_EN_SHFT                                                                   0x0

#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016018)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016018)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF0_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001601c)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x0001601c)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF1_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016020)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016020)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF2_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016024)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016024)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF3_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016028)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016028)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF4_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001602c)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x0001602c)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF5_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016030)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016030)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF6_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00016034)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00016034)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_RMSK                                                                     0x371f
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_RESAMPLER_PERF7_DFSR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016038)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016038)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF0_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001603c)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001603c)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF1_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016040)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016040)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF2_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016044)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016044)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF3_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016048)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016048)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF4_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001604c)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001604c)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF5_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016050)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016050)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF6_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016054)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016054)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_M_BMSK                                                                   0xff
#define HWIO_LPASS_RESAMPLER_PERF7_M_DFSR_M_SHFT                                                                    0x0

#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016058)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016058)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001605c)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001605c)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016060)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016060)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016064)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016064)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016068)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016068)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001606c)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001606c)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016070)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016070)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016074)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016074)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                                                       0xff
#define HWIO_LPASS_RESAMPLER_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                                                        0x0

#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016078)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016078)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF0_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001607c)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001607c)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF1_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016080)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016080)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF2_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016084)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016084)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF3_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016088)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016088)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF4_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0001608c)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001608c)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF5_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016090)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016090)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF6_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x00016094)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x00016094)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_RMSK                                                                     0xff
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR, HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_RMSK)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR, m)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR,v)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_ADDR,m,v,HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_IN)
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_NOT_2D_BMSK                                                              0xff
#define HWIO_LPASS_RESAMPLER_PERF7_D_DFSR_NOT_2D_SHFT                                                               0x0

#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00017000)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00017000)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR, HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_IN)
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_AUD_SLIMBUS_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00017004)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00017004)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR, HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_IN)
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_AUD_SLIMBUS_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_AUD_SLIMBUS_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00017008)
#define HWIO_LPASS_AUD_SLIMBUS_M_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00017008)
#define HWIO_LPASS_AUD_SLIMBUS_M_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_M_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_M_ADDR, HWIO_LPASS_AUD_SLIMBUS_M_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_M_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_M_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_M_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_M_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_M_IN)
#define HWIO_LPASS_AUD_SLIMBUS_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_AUD_SLIMBUS_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_AUD_SLIMBUS_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001700c)
#define HWIO_LPASS_AUD_SLIMBUS_N_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001700c)
#define HWIO_LPASS_AUD_SLIMBUS_N_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_N_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_N_ADDR, HWIO_LPASS_AUD_SLIMBUS_N_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_N_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_N_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_N_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_N_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_N_IN)
#define HWIO_LPASS_AUD_SLIMBUS_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_AUD_SLIMBUS_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_AUD_SLIMBUS_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00017010)
#define HWIO_LPASS_AUD_SLIMBUS_D_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00017010)
#define HWIO_LPASS_AUD_SLIMBUS_D_RMSK                                                                              0xff
#define HWIO_LPASS_AUD_SLIMBUS_D_IN          \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_D_ADDR, HWIO_LPASS_AUD_SLIMBUS_D_RMSK)
#define HWIO_LPASS_AUD_SLIMBUS_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUD_SLIMBUS_D_ADDR, m)
#define HWIO_LPASS_AUD_SLIMBUS_D_OUT(v)      \
        out_dword(HWIO_LPASS_AUD_SLIMBUS_D_ADDR,v)
#define HWIO_LPASS_AUD_SLIMBUS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUD_SLIMBUS_D_ADDR,m,v,HWIO_LPASS_AUD_SLIMBUS_D_IN)
#define HWIO_LPASS_AUD_SLIMBUS_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_AUD_SLIMBUS_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00017014)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00017014)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_RMSK                                                          0x80000003
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_HW_CTL_BMSK                                                          0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_HW_CTL_SHFT                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00017018)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00017018)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_RMSK                                                     0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                       0x4000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                          0xe
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                     0x2000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                        0xd
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                    0x1000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                       0xc
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_WAKEUP_BMSK                                                   0xf00
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_WAKEUP_SHFT                                                     0x8
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_SLEEP_BMSK                                                     0xf0
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_SLEEP_SHFT                                                      0x4
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0001701c)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001701c)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00018000)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00018000)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR, HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_IN)
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_QCA_SLIMBUS_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00018004)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00018004)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR, HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_IN)
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_QCA_SLIMBUS_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_QCA_SLIMBUS_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00018008)
#define HWIO_LPASS_QCA_SLIMBUS_M_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00018008)
#define HWIO_LPASS_QCA_SLIMBUS_M_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_M_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_M_ADDR, HWIO_LPASS_QCA_SLIMBUS_M_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_M_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_M_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_M_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_M_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_M_IN)
#define HWIO_LPASS_QCA_SLIMBUS_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_QCA_SLIMBUS_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_QCA_SLIMBUS_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001800c)
#define HWIO_LPASS_QCA_SLIMBUS_N_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001800c)
#define HWIO_LPASS_QCA_SLIMBUS_N_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_N_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_N_ADDR, HWIO_LPASS_QCA_SLIMBUS_N_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_N_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_N_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_N_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_N_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_N_IN)
#define HWIO_LPASS_QCA_SLIMBUS_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_QCA_SLIMBUS_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_QCA_SLIMBUS_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00018010)
#define HWIO_LPASS_QCA_SLIMBUS_D_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00018010)
#define HWIO_LPASS_QCA_SLIMBUS_D_RMSK                                                                              0xff
#define HWIO_LPASS_QCA_SLIMBUS_D_IN          \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_D_ADDR, HWIO_LPASS_QCA_SLIMBUS_D_RMSK)
#define HWIO_LPASS_QCA_SLIMBUS_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_QCA_SLIMBUS_D_ADDR, m)
#define HWIO_LPASS_QCA_SLIMBUS_D_OUT(v)      \
        out_dword(HWIO_LPASS_QCA_SLIMBUS_D_ADDR,v)
#define HWIO_LPASS_QCA_SLIMBUS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QCA_SLIMBUS_D_ADDR,m,v,HWIO_LPASS_QCA_SLIMBUS_D_IN)
#define HWIO_LPASS_QCA_SLIMBUS_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_QCA_SLIMBUS_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x00018014)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x00018014)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_RMSK                                                          0x80000003
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_HW_CTL_BMSK                                                          0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_HW_CTL_SHFT                                                          0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00018018)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00018018)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_RMSK                                                     0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                       0x4000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                          0xe
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                     0x2000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                        0xd
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                    0x1000
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                       0xc
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_WAKEUP_BMSK                                                   0xf00
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_WAKEUP_SHFT                                                     0x8
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_SLEEP_BMSK                                                     0xf0
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_SLEEP_SHFT                                                      0x4
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00019000)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00019000)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_RMSK                                                                 0x800000f3
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR, HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_IN)
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_OFF_BMSK                                                        0x80000000
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_OFF_SHFT                                                              0x1f
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_D_BMSK                                                               0x80
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_D_SHFT                                                                0x7
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_N_BMSK                                                               0x40
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_N_SHFT                                                                0x6
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_M_BMSK                                                               0x20
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_M_SHFT                                                                0x5
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                        0x10
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                         0x4
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_EN_BMSK                                                                0x2
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_ROOT_EN_SHFT                                                                0x1
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_UPDATE_BMSK                                                                 0x1
#define HWIO_LPASS_LPAIF_PCMOE_CMD_RCGR_UPDATE_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x00019004)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x00019004)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_RMSK                                                                     0x771f
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR, HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_IN)
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ALT_SRC_SEL_BMSK                                                         0x4000
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_ALT_SRC_SEL_SHFT                                                            0xe
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_MODE_BMSK                                                                0x3000
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_MODE_SHFT                                                                   0xc
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_SEL_BMSK                                                              0x700
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_SEL_SHFT                                                                0x8
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_DIV_BMSK                                                               0x1f
#define HWIO_LPASS_LPAIF_PCMOE_CFG_RCGR_SRC_DIV_SHFT                                                                0x0

#define HWIO_LPASS_LPAIF_PCMOE_M_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00019008)
#define HWIO_LPASS_LPAIF_PCMOE_M_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00019008)
#define HWIO_LPASS_LPAIF_PCMOE_M_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_M_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_M_ADDR, HWIO_LPASS_LPAIF_PCMOE_M_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_M_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_M_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_M_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_M_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_M_IN)
#define HWIO_LPASS_LPAIF_PCMOE_M_M_BMSK                                                                            0xff
#define HWIO_LPASS_LPAIF_PCMOE_M_M_SHFT                                                                             0x0

#define HWIO_LPASS_LPAIF_PCMOE_N_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001900c)
#define HWIO_LPASS_LPAIF_PCMOE_N_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001900c)
#define HWIO_LPASS_LPAIF_PCMOE_N_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_N_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_N_ADDR, HWIO_LPASS_LPAIF_PCMOE_N_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_N_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_N_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_N_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_N_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_N_IN)
#define HWIO_LPASS_LPAIF_PCMOE_N_NOT_N_MINUS_M_BMSK                                                                0xff
#define HWIO_LPASS_LPAIF_PCMOE_N_NOT_N_MINUS_M_SHFT                                                                 0x0

#define HWIO_LPASS_LPAIF_PCMOE_D_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x00019010)
#define HWIO_LPASS_LPAIF_PCMOE_D_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x00019010)
#define HWIO_LPASS_LPAIF_PCMOE_D_RMSK                                                                              0xff
#define HWIO_LPASS_LPAIF_PCMOE_D_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_D_ADDR, HWIO_LPASS_LPAIF_PCMOE_D_RMSK)
#define HWIO_LPASS_LPAIF_PCMOE_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_PCMOE_D_ADDR, m)
#define HWIO_LPASS_LPAIF_PCMOE_D_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_PCMOE_D_ADDR,v)
#define HWIO_LPASS_LPAIF_PCMOE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_PCMOE_D_ADDR,m,v,HWIO_LPASS_LPAIF_PCMOE_D_IN)
#define HWIO_LPASS_LPAIF_PCMOE_D_NOT_2D_BMSK                                                                       0xff
#define HWIO_LPASS_LPAIF_PCMOE_D_NOT_2D_SHFT                                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00019014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_OFFS                                                    (LPASS_CC_REG_BASE_OFFS + 0x00019014)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_RMSK                                                    0x80000003
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_HW_CTL_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_HW_CTL_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_LPASS_SLEEP_CMD_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0001a000)
#define HWIO_LPASS_SLEEP_CMD_RCGR_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x0001a000)
#define HWIO_LPASS_SLEEP_CMD_RCGR_RMSK                                                                       0x800000f3
#define HWIO_LPASS_SLEEP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR, HWIO_LPASS_SLEEP_CMD_RCGR_RMSK)
#define HWIO_LPASS_SLEEP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_SLEEP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_SLEEP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR,m,v,HWIO_LPASS_SLEEP_CMD_RCGR_IN)
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_OFF_BMSK                                                              0x80000000
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_OFF_SHFT                                                                    0x1f
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_D_BMSK                                                                     0x80
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_D_SHFT                                                                      0x7
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_N_BMSK                                                                     0x40
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_N_SHFT                                                                      0x6
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_M_BMSK                                                                     0x20
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_M_SHFT                                                                      0x5
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                              0x10
#define HWIO_LPASS_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                               0x4
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_EN_BMSK                                                                      0x2
#define HWIO_LPASS_SLEEP_CMD_RCGR_ROOT_EN_SHFT                                                                      0x1
#define HWIO_LPASS_SLEEP_CMD_RCGR_UPDATE_BMSK                                                                       0x1
#define HWIO_LPASS_SLEEP_CMD_RCGR_UPDATE_SHFT                                                                       0x0

#define HWIO_LPASS_SLEEP_CFG_RCGR_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0001a004)
#define HWIO_LPASS_SLEEP_CFG_RCGR_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x0001a004)
#define HWIO_LPASS_SLEEP_CFG_RCGR_RMSK                                                                          0x1371f
#define HWIO_LPASS_SLEEP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR, HWIO_LPASS_SLEEP_CFG_RCGR_RMSK)
#define HWIO_LPASS_SLEEP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_SLEEP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_SLEEP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SLEEP_CFG_RCGR_ADDR,m,v,HWIO_LPASS_SLEEP_CFG_RCGR_IN)
#define HWIO_LPASS_SLEEP_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                          0x10000
#define HWIO_LPASS_SLEEP_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                             0x10
#define HWIO_LPASS_SLEEP_CFG_RCGR_MODE_BMSK                                                                      0x3000
#define HWIO_LPASS_SLEEP_CFG_RCGR_MODE_SHFT                                                                         0xc
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_SEL_BMSK                                                                    0x700
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_SEL_SHFT                                                                      0x8
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_DIV_BMSK                                                                     0x1f
#define HWIO_LPASS_SLEEP_CFG_RCGR_SRC_DIV_SHFT                                                                      0x0

#define HWIO_LPASS_XO_CMD_RCGR_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001b000)
#define HWIO_LPASS_XO_CMD_RCGR_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001b000)
#define HWIO_LPASS_XO_CMD_RCGR_RMSK                                                                          0x800000f3
#define HWIO_LPASS_XO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_XO_CMD_RCGR_ADDR, HWIO_LPASS_XO_CMD_RCGR_RMSK)
#define HWIO_LPASS_XO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_XO_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_XO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_XO_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_XO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_XO_CMD_RCGR_ADDR,m,v,HWIO_LPASS_XO_CMD_RCGR_IN)
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_OFF_BMSK                                                                 0x80000000
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_OFF_SHFT                                                                       0x1f
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_D_BMSK                                                                        0x80
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_D_SHFT                                                                         0x7
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_N_BMSK                                                                        0x40
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_N_SHFT                                                                         0x6
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_M_BMSK                                                                        0x20
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_M_SHFT                                                                         0x5
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                                 0x10
#define HWIO_LPASS_XO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                  0x4
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_EN_BMSK                                                                         0x2
#define HWIO_LPASS_XO_CMD_RCGR_ROOT_EN_SHFT                                                                         0x1
#define HWIO_LPASS_XO_CMD_RCGR_UPDATE_BMSK                                                                          0x1
#define HWIO_LPASS_XO_CMD_RCGR_UPDATE_SHFT                                                                          0x0

#define HWIO_LPASS_XO_CFG_RCGR_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0001b004)
#define HWIO_LPASS_XO_CFG_RCGR_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0001b004)
#define HWIO_LPASS_XO_CFG_RCGR_RMSK                                                                             0x1771f
#define HWIO_LPASS_XO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_XO_CFG_RCGR_ADDR, HWIO_LPASS_XO_CFG_RCGR_RMSK)
#define HWIO_LPASS_XO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_XO_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_XO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_XO_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_XO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_XO_CFG_RCGR_ADDR,m,v,HWIO_LPASS_XO_CFG_RCGR_IN)
#define HWIO_LPASS_XO_CFG_RCGR_RCGLITE_DISABLE_BMSK                                                             0x10000
#define HWIO_LPASS_XO_CFG_RCGR_RCGLITE_DISABLE_SHFT                                                                0x10
#define HWIO_LPASS_XO_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                  0x4000
#define HWIO_LPASS_XO_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                     0xe
#define HWIO_LPASS_XO_CFG_RCGR_MODE_BMSK                                                                         0x3000
#define HWIO_LPASS_XO_CFG_RCGR_MODE_SHFT                                                                            0xc
#define HWIO_LPASS_XO_CFG_RCGR_SRC_SEL_BMSK                                                                       0x700
#define HWIO_LPASS_XO_CFG_RCGR_SRC_SEL_SHFT                                                                         0x8
#define HWIO_LPASS_XO_CFG_RCGR_SRC_DIV_BMSK                                                                        0x1f
#define HWIO_LPASS_XO_CFG_RCGR_SRC_DIV_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0001c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_OFFS                                                           (LPASS_CC_REG_BASE_OFFS + 0x0001c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_RMSK                                                           0x80000003
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_HW_CTL_BMSK                                                           0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_HW_CTL_SHFT                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_STC_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_LPASS_CORE_CMD_RCGR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d000)
#define HWIO_LPASS_CORE_CMD_RCGR_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001d000)
#define HWIO_LPASS_CORE_CMD_RCGR_RMSK                                                                        0x800000f3
#define HWIO_LPASS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CMD_RCGR_ADDR, HWIO_LPASS_CORE_CMD_RCGR_RMSK)
#define HWIO_LPASS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CMD_RCGR_ADDR,m,v,HWIO_LPASS_CORE_CMD_RCGR_IN)
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_OFF_BMSK                                                               0x80000000
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                                                     0x1f
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_D_BMSK                                                                      0x80
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_D_SHFT                                                                       0x7
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_N_BMSK                                                                      0x40
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_N_SHFT                                                                       0x6
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_M_BMSK                                                                      0x20
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_M_SHFT                                                                       0x5
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                               0x10
#define HWIO_LPASS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                                0x4
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_EN_BMSK                                                                       0x2
#define HWIO_LPASS_CORE_CMD_RCGR_ROOT_EN_SHFT                                                                       0x1
#define HWIO_LPASS_CORE_CMD_RCGR_UPDATE_BMSK                                                                        0x1
#define HWIO_LPASS_CORE_CMD_RCGR_UPDATE_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_CFG_RCGR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d004)
#define HWIO_LPASS_CORE_CFG_RCGR_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001d004)
#define HWIO_LPASS_CORE_CFG_RCGR_RMSK                                                                            0x771f
#define HWIO_LPASS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CFG_RCGR_ADDR, HWIO_LPASS_CORE_CFG_RCGR_RMSK)
#define HWIO_LPASS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CFG_RCGR_ADDR,m,v,HWIO_LPASS_CORE_CFG_RCGR_IN)
#define HWIO_LPASS_CORE_CFG_RCGR_ALT_SRC_SEL_BMSK                                                                0x4000
#define HWIO_LPASS_CORE_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                   0xe
#define HWIO_LPASS_CORE_CFG_RCGR_MODE_BMSK                                                                       0x3000
#define HWIO_LPASS_CORE_CFG_RCGR_MODE_SHFT                                                                          0xc
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_SEL_BMSK                                                                     0x700
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_SEL_SHFT                                                                       0x8
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_DIV_BMSK                                                                      0x1f
#define HWIO_LPASS_CORE_CFG_RCGR_SRC_DIV_SHFT                                                                       0x0

#define HWIO_LPASS_CORE_M_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d008)
#define HWIO_LPASS_CORE_M_OFFS                                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001d008)
#define HWIO_LPASS_CORE_M_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_M_IN          \
        in_dword_masked(HWIO_LPASS_CORE_M_ADDR, HWIO_LPASS_CORE_M_RMSK)
#define HWIO_LPASS_CORE_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_M_ADDR, m)
#define HWIO_LPASS_CORE_M_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_M_ADDR,v)
#define HWIO_LPASS_CORE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_M_ADDR,m,v,HWIO_LPASS_CORE_M_IN)
#define HWIO_LPASS_CORE_M_M_BMSK                                                                                   0xff
#define HWIO_LPASS_CORE_M_M_SHFT                                                                                    0x0

#define HWIO_LPASS_CORE_N_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d00c)
#define HWIO_LPASS_CORE_N_OFFS                                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001d00c)
#define HWIO_LPASS_CORE_N_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_N_IN          \
        in_dword_masked(HWIO_LPASS_CORE_N_ADDR, HWIO_LPASS_CORE_N_RMSK)
#define HWIO_LPASS_CORE_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_N_ADDR, m)
#define HWIO_LPASS_CORE_N_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_N_ADDR,v)
#define HWIO_LPASS_CORE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_N_ADDR,m,v,HWIO_LPASS_CORE_N_IN)
#define HWIO_LPASS_CORE_N_NOT_N_MINUS_M_BMSK                                                                       0xff
#define HWIO_LPASS_CORE_N_NOT_N_MINUS_M_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_D_ADDR                                                                               (LPASS_CC_REG_BASE      + 0x0001d010)
#define HWIO_LPASS_CORE_D_OFFS                                                                               (LPASS_CC_REG_BASE_OFFS + 0x0001d010)
#define HWIO_LPASS_CORE_D_RMSK                                                                                     0xff
#define HWIO_LPASS_CORE_D_IN          \
        in_dword_masked(HWIO_LPASS_CORE_D_ADDR, HWIO_LPASS_CORE_D_RMSK)
#define HWIO_LPASS_CORE_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_D_ADDR, m)
#define HWIO_LPASS_CORE_D_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_D_ADDR,v)
#define HWIO_LPASS_CORE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_D_ADDR,m,v,HWIO_LPASS_CORE_D_IN)
#define HWIO_LPASS_CORE_D_NOT_2D_BMSK                                                                              0xff
#define HWIO_LPASS_CORE_D_NOT_2D_SHFT                                                                               0x0

#define HWIO_LPASS_CORE_CMD_DFSR_ADDR                                                                        (LPASS_CC_REG_BASE      + 0x0001d014)
#define HWIO_LPASS_CORE_CMD_DFSR_OFFS                                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001d014)
#define HWIO_LPASS_CORE_CMD_DFSR_RMSK                                                                            0xbfef
#define HWIO_LPASS_CORE_CMD_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_CMD_DFSR_ADDR, HWIO_LPASS_CORE_CMD_DFSR_RMSK)
#define HWIO_LPASS_CORE_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_CMD_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_CMD_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_CMD_DFSR_ADDR,m,v,HWIO_LPASS_CORE_CMD_DFSR_IN)
#define HWIO_LPASS_CORE_CMD_DFSR_RCG_SW_CTRL_BMSK                                                                0x8000
#define HWIO_LPASS_CORE_CMD_DFSR_RCG_SW_CTRL_SHFT                                                                   0xf
#define HWIO_LPASS_CORE_CMD_DFSR_SW_PERF_STATE_BMSK                                                              0x3800
#define HWIO_LPASS_CORE_CMD_DFSR_SW_PERF_STATE_SHFT                                                                 0xb
#define HWIO_LPASS_CORE_CMD_DFSR_SW_OVERRIDE_BMSK                                                                 0x400
#define HWIO_LPASS_CORE_CMD_DFSR_SW_OVERRIDE_SHFT                                                                   0xa
#define HWIO_LPASS_CORE_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                                    0x200
#define HWIO_LPASS_CORE_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                                      0x9
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_FSM_STATE_BMSK                                                               0x1c0
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_FSM_STATE_SHFT                                                                 0x6
#define HWIO_LPASS_CORE_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                               0x20
#define HWIO_LPASS_CORE_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                                0x5
#define HWIO_LPASS_CORE_CMD_DFSR_CURR_PERF_STATE_BMSK                                                               0xe
#define HWIO_LPASS_CORE_CMD_DFSR_CURR_PERF_STATE_SHFT                                                               0x1
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_EN_BMSK                                                                        0x1
#define HWIO_LPASS_CORE_CMD_DFSR_DFS_EN_SHFT                                                                        0x0

#define HWIO_LPASS_CORE_PERF0_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d018)
#define HWIO_LPASS_CORE_PERF0_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d018)
#define HWIO_LPASS_CORE_PERF0_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF0_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF0_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF1_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d01c)
#define HWIO_LPASS_CORE_PERF1_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d01c)
#define HWIO_LPASS_CORE_PERF1_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF1_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF1_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF2_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d020)
#define HWIO_LPASS_CORE_PERF2_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d020)
#define HWIO_LPASS_CORE_PERF2_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF2_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF2_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF3_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d024)
#define HWIO_LPASS_CORE_PERF3_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d024)
#define HWIO_LPASS_CORE_PERF3_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF3_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF3_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF4_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d028)
#define HWIO_LPASS_CORE_PERF4_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d028)
#define HWIO_LPASS_CORE_PERF4_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF4_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF4_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF5_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d02c)
#define HWIO_LPASS_CORE_PERF5_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d02c)
#define HWIO_LPASS_CORE_PERF5_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF5_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF5_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF6_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d030)
#define HWIO_LPASS_CORE_PERF6_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d030)
#define HWIO_LPASS_CORE_PERF6_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF6_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF6_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF7_DFSR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0001d034)
#define HWIO_LPASS_CORE_PERF7_DFSR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0001d034)
#define HWIO_LPASS_CORE_PERF7_DFSR_RMSK                                                                          0x371f
#define HWIO_LPASS_CORE_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_DFSR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_CORE_PERF7_DFSR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_CORE_PERF7_DFSR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d038)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d038)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF0_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d03c)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d03c)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF1_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d040)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d040)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF2_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d044)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d044)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF3_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d048)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d048)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF4_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d04c)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d04c)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF5_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d050)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d050)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF6_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d054)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d054)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_M_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_M_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_M_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_M_DFSR_M_BMSK                                                                        0xff
#define HWIO_LPASS_CORE_PERF7_M_DFSR_M_SHFT                                                                         0x0

#define HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d058)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d058)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d05c)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d05c)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d060)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d060)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d064)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d064)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d068)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d068)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d06c)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d06c)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d070)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d070)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d074)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d074)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_N_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_N_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_N_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                                                            0xff
#define HWIO_LPASS_CORE_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                                                             0x0

#define HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d078)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d078)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF0_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF0_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF0_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF0_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF0_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d07c)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d07c)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF1_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF1_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF1_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF1_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF1_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d080)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d080)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF2_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF2_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF2_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF2_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF2_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d084)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d084)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF3_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF3_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF3_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF3_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF3_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d088)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d088)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF4_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF4_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF4_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF4_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF4_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d08c)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d08c)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF5_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF5_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF5_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF5_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF5_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d090)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d090)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF6_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF6_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF6_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF6_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF6_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0001d094)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0001d094)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_RMSK                                                                          0xff
#define HWIO_LPASS_CORE_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR, HWIO_LPASS_CORE_PERF7_D_DFSR_RMSK)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR, m)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR,v)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CORE_PERF7_D_DFSR_ADDR,m,v,HWIO_LPASS_CORE_PERF7_D_DFSR_IN)
#define HWIO_LPASS_CORE_PERF7_D_DFSR_NOT_2D_BMSK                                                                   0xff
#define HWIO_LPASS_CORE_PERF7_D_DFSR_NOT_2D_SHFT                                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_OFFS                                                             (LPASS_CC_REG_BASE_OFFS + 0x0001e000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_RMSK                                                             0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                               0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                                  0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                             0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                            0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                               0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_WAKEUP_BMSK                                                           0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_WAKEUP_SHFT                                                             0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_SLEEP_BMSK                                                             0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_SLEEP_SHFT                                                              0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001e004)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001e008)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001e00c)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x0001e010)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_RMSK                                                        0x80007ff3
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR                                                                 (LPASS_CC_REG_BASE      + 0x0001f000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_OFFS                                                                 (LPASS_CC_REG_BASE_OFFS + 0x0001f000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_RMSK                                                                 0x80000003
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_HW_CTL_BMSK                                                                 0x2
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_HW_CTL_SHFT                                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00020000)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00020000)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK0_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK0_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00020004)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00020004)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK0_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK0_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK0_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00020008)
#define HWIO_LPASS_EXT_MCLK0_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00020008)
#define HWIO_LPASS_EXT_MCLK0_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_M_ADDR, HWIO_LPASS_EXT_MCLK0_M_RMSK)
#define HWIO_LPASS_EXT_MCLK0_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_M_IN)
#define HWIO_LPASS_EXT_MCLK0_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK0_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK0_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002000c)
#define HWIO_LPASS_EXT_MCLK0_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0002000c)
#define HWIO_LPASS_EXT_MCLK0_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_N_ADDR, HWIO_LPASS_EXT_MCLK0_N_RMSK)
#define HWIO_LPASS_EXT_MCLK0_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_N_IN)
#define HWIO_LPASS_EXT_MCLK0_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK0_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK0_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00020010)
#define HWIO_LPASS_EXT_MCLK0_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00020010)
#define HWIO_LPASS_EXT_MCLK0_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK0_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_D_ADDR, HWIO_LPASS_EXT_MCLK0_D_RMSK)
#define HWIO_LPASS_EXT_MCLK0_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK0_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK0_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK0_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK0_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK0_D_IN)
#define HWIO_LPASS_EXT_MCLK0_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK0_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00020014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00020014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00021000)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00021000)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK1_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK1_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00021004)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00021004)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK1_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK1_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK1_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00021008)
#define HWIO_LPASS_EXT_MCLK1_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00021008)
#define HWIO_LPASS_EXT_MCLK1_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_M_ADDR, HWIO_LPASS_EXT_MCLK1_M_RMSK)
#define HWIO_LPASS_EXT_MCLK1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_M_IN)
#define HWIO_LPASS_EXT_MCLK1_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK1_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK1_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002100c)
#define HWIO_LPASS_EXT_MCLK1_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0002100c)
#define HWIO_LPASS_EXT_MCLK1_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_N_ADDR, HWIO_LPASS_EXT_MCLK1_N_RMSK)
#define HWIO_LPASS_EXT_MCLK1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_N_IN)
#define HWIO_LPASS_EXT_MCLK1_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK1_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK1_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00021010)
#define HWIO_LPASS_EXT_MCLK1_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00021010)
#define HWIO_LPASS_EXT_MCLK1_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK1_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_D_ADDR, HWIO_LPASS_EXT_MCLK1_D_RMSK)
#define HWIO_LPASS_EXT_MCLK1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK1_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK1_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK1_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK1_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK1_D_IN)
#define HWIO_LPASS_EXT_MCLK1_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK1_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00021014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00021014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK1_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00022000)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00022000)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR, HWIO_LPASS_EXT_MCLK2_CMD_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_CMD_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_EXT_MCLK2_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x00022004)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x00022004)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR, HWIO_LPASS_EXT_MCLK2_CFG_RCGR_RMSK)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_CFG_RCGR_IN)
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_EXT_MCLK2_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_EXT_MCLK2_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00022008)
#define HWIO_LPASS_EXT_MCLK2_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00022008)
#define HWIO_LPASS_EXT_MCLK2_M_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_M_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_M_ADDR, HWIO_LPASS_EXT_MCLK2_M_RMSK)
#define HWIO_LPASS_EXT_MCLK2_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_M_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_M_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_M_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_M_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_M_IN)
#define HWIO_LPASS_EXT_MCLK2_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_EXT_MCLK2_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_EXT_MCLK2_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0002200c)
#define HWIO_LPASS_EXT_MCLK2_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0002200c)
#define HWIO_LPASS_EXT_MCLK2_N_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_N_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_N_ADDR, HWIO_LPASS_EXT_MCLK2_N_RMSK)
#define HWIO_LPASS_EXT_MCLK2_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_N_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_N_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_N_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_N_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_N_IN)
#define HWIO_LPASS_EXT_MCLK2_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_EXT_MCLK2_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_EXT_MCLK2_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x00022010)
#define HWIO_LPASS_EXT_MCLK2_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x00022010)
#define HWIO_LPASS_EXT_MCLK2_D_RMSK                                                                                0xff
#define HWIO_LPASS_EXT_MCLK2_D_IN          \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_D_ADDR, HWIO_LPASS_EXT_MCLK2_D_RMSK)
#define HWIO_LPASS_EXT_MCLK2_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_EXT_MCLK2_D_ADDR, m)
#define HWIO_LPASS_EXT_MCLK2_D_OUT(v)      \
        out_dword(HWIO_LPASS_EXT_MCLK2_D_ADDR,v)
#define HWIO_LPASS_EXT_MCLK2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_EXT_MCLK2_D_ADDR,m,v,HWIO_LPASS_EXT_MCLK2_D_IN)
#define HWIO_LPASS_EXT_MCLK2_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_EXT_MCLK2_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x00022014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x00022014)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_EXT_MCLK2_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OFFS                                                    (LPASS_CC_REG_BASE_OFFS + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK                                                    0x80000003
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_BMSK                                                    0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_SHFT                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x00024000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x00024000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_RMSK                                                     0x80000003
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR                                             (LPASS_CC_REG_BASE      + 0x00037008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OFFS                                             (LPASS_CC_REG_BASE_OFFS + 0x00037008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK                                                    0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN          \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_RMSK)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_INM(m)      \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR, m)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUT(v)      \
        out_dword(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,v)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_ADDR,m,v,HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_IN)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_BMSK                   0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_Q6_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_SHFT                   0x0

#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR                                           (LPASS_CC_REG_BASE      + 0x00038008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OFFS                                           (LPASS_CC_REG_BASE_OFFS + 0x00038008)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK                                                  0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN          \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_RMSK)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_INM(m)      \
        in_dword_masked(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR, m)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUT(v)      \
        out_dword(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,v)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_ADDR,m,v,HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_IN)
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_BMSK                 0x1
#define HWIO_LPASS_GDS_HW_CTRL_SEQUENCE_ABORT_IRQ_ENABLE_APPS_LPASS_CORE_SEQUENCE_ABORT_IRQ_EN_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR                                                     (LPASS_CC_REG_BASE      + 0x0003b000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_OFFS                                                     (LPASS_CC_REG_BASE_OFFS + 0x0003b000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_RMSK                                                     0x80000003
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_HW_CTL_BMSK                                                     0x2
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_HW_CTL_SHFT                                                     0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x0003e000)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x0003e000)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_RMSK                                                                           0x3
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR, HWIO_LPASS_JBIST_PLL_SRC_SEL_RMSK)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR, m)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR,v)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_PLL_SRC_SEL_ADDR,m,v,HWIO_LPASS_JBIST_PLL_SRC_SEL_IN)
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_BMSK                                                     0x3
#define HWIO_LPASS_JBIST_PLL_SRC_SEL_JBIST_PLL_CLK_SRC_SEL_SHFT                                                     0x0

#define HWIO_LPASS_JBIST_MODE_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0003e004)
#define HWIO_LPASS_JBIST_MODE_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x0003e004)
#define HWIO_LPASS_JBIST_MODE_RMSK                                                                           0xffffffff
#define HWIO_LPASS_JBIST_MODE_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_MODE_ADDR, HWIO_LPASS_JBIST_MODE_RMSK)
#define HWIO_LPASS_JBIST_MODE_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_MODE_ADDR, m)
#define HWIO_LPASS_JBIST_MODE_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_MODE_ADDR,v)
#define HWIO_LPASS_JBIST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_MODE_ADDR,m,v,HWIO_LPASS_JBIST_MODE_IN)
#define HWIO_LPASS_JBIST_MODE_RESERVE_BITS31_4_BMSK                                                          0xfffffff0
#define HWIO_LPASS_JBIST_MODE_RESERVE_BITS31_4_SHFT                                                                 0x4
#define HWIO_LPASS_JBIST_MODE_START_MEAS_BMSK                                                                       0x8
#define HWIO_LPASS_JBIST_MODE_START_MEAS_SHFT                                                                       0x3
#define HWIO_LPASS_JBIST_MODE_JBIST_TEST_BMSK                                                                       0x4
#define HWIO_LPASS_JBIST_MODE_JBIST_TEST_SHFT                                                                       0x2
#define HWIO_LPASS_JBIST_MODE_RESET_N_BMSK                                                                          0x2
#define HWIO_LPASS_JBIST_MODE_RESET_N_SHFT                                                                          0x1
#define HWIO_LPASS_JBIST_MODE_SLEEP_N_BMSK                                                                          0x1
#define HWIO_LPASS_JBIST_MODE_SLEEP_N_SHFT                                                                          0x0

#define HWIO_LPASS_JBIST_CONFIG_CTL_ADDR                                                                     (LPASS_CC_REG_BASE      + 0x0003e008)
#define HWIO_LPASS_JBIST_CONFIG_CTL_OFFS                                                                     (LPASS_CC_REG_BASE_OFFS + 0x0003e008)
#define HWIO_LPASS_JBIST_CONFIG_CTL_RMSK                                                                     0xffffffff
#define HWIO_LPASS_JBIST_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR, HWIO_LPASS_JBIST_CONFIG_CTL_RMSK)
#define HWIO_LPASS_JBIST_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_CONFIG_CTL_ADDR,m,v,HWIO_LPASS_JBIST_CONFIG_CTL_IN)
#define HWIO_LPASS_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_BMSK                                                    0xffffffff
#define HWIO_LPASS_JBIST_CONFIG_CTL_JBIST_CONFIG_CTL_SHFT                                                           0x0

#define HWIO_LPASS_JBIST_USER_CTL_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x0003e00c)
#define HWIO_LPASS_JBIST_USER_CTL_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x0003e00c)
#define HWIO_LPASS_JBIST_USER_CTL_RMSK                                                                       0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_ADDR, HWIO_LPASS_JBIST_USER_CTL_RMSK)
#define HWIO_LPASS_JBIST_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_USER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_USER_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_USER_CTL_ADDR,m,v,HWIO_LPASS_JBIST_USER_CTL_IN)
#define HWIO_LPASS_JBIST_USER_CTL_JBIST_USER_CTL_BMSK                                                        0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_JBIST_USER_CTL_SHFT                                                               0x0

#define HWIO_LPASS_JBIST_USER_CTL_U_ADDR                                                                     (LPASS_CC_REG_BASE      + 0x0003e010)
#define HWIO_LPASS_JBIST_USER_CTL_U_OFFS                                                                     (LPASS_CC_REG_BASE_OFFS + 0x0003e010)
#define HWIO_LPASS_JBIST_USER_CTL_U_RMSK                                                                     0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_U_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_U_ADDR, HWIO_LPASS_JBIST_USER_CTL_U_RMSK)
#define HWIO_LPASS_JBIST_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_USER_CTL_U_ADDR, m)
#define HWIO_LPASS_JBIST_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_USER_CTL_U_ADDR,v)
#define HWIO_LPASS_JBIST_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_USER_CTL_U_ADDR,m,v,HWIO_LPASS_JBIST_USER_CTL_U_IN)
#define HWIO_LPASS_JBIST_USER_CTL_U_JBIST_USER_CTL_U_BMSK                                                    0xffffffff
#define HWIO_LPASS_JBIST_USER_CTL_U_JBIST_USER_CTL_U_SHFT                                                           0x0

#define HWIO_LPASS_JBIST_TEST_CTL_ADDR                                                                       (LPASS_CC_REG_BASE      + 0x00003e14)
#define HWIO_LPASS_JBIST_TEST_CTL_OFFS                                                                       (LPASS_CC_REG_BASE_OFFS + 0x00003e14)
#define HWIO_LPASS_JBIST_TEST_CTL_RMSK                                                                       0xffffffff
#define HWIO_LPASS_JBIST_TEST_CTL_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_TEST_CTL_ADDR, HWIO_LPASS_JBIST_TEST_CTL_RMSK)
#define HWIO_LPASS_JBIST_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_TEST_CTL_ADDR, m)
#define HWIO_LPASS_JBIST_TEST_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_TEST_CTL_ADDR,v)
#define HWIO_LPASS_JBIST_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_TEST_CTL_ADDR,m,v,HWIO_LPASS_JBIST_TEST_CTL_IN)
#define HWIO_LPASS_JBIST_TEST_CTL_JBIST_TEST_CTL_BMSK                                                        0xffffffff
#define HWIO_LPASS_JBIST_TEST_CTL_JBIST_TEST_CTL_SHFT                                                               0x0

#define HWIO_LPASS_JBIST_STATUS_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0003e018)
#define HWIO_LPASS_JBIST_STATUS_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0003e018)
#define HWIO_LPASS_JBIST_STATUS_RMSK                                                                         0xffffffff
#define HWIO_LPASS_JBIST_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_STATUS_ADDR, HWIO_LPASS_JBIST_STATUS_RMSK)
#define HWIO_LPASS_JBIST_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_STATUS_ADDR, m)
#define HWIO_LPASS_JBIST_STATUS_JBIST_STATUS_BMSK                                                            0xffffffff
#define HWIO_LPASS_JBIST_STATUS_JBIST_STATUS_SHFT                                                                   0x0

#define HWIO_LPASS_JBIST_MEAS_DONE_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0003e01c)
#define HWIO_LPASS_JBIST_MEAS_DONE_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0003e01c)
#define HWIO_LPASS_JBIST_MEAS_DONE_RMSK                                                                      0xffffffff
#define HWIO_LPASS_JBIST_MEAS_DONE_IN          \
        in_dword_masked(HWIO_LPASS_JBIST_MEAS_DONE_ADDR, HWIO_LPASS_JBIST_MEAS_DONE_RMSK)
#define HWIO_LPASS_JBIST_MEAS_DONE_INM(m)      \
        in_dword_masked(HWIO_LPASS_JBIST_MEAS_DONE_ADDR, m)
#define HWIO_LPASS_JBIST_MEAS_DONE_OUT(v)      \
        out_dword(HWIO_LPASS_JBIST_MEAS_DONE_ADDR,v)
#define HWIO_LPASS_JBIST_MEAS_DONE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_JBIST_MEAS_DONE_ADDR,m,v,HWIO_LPASS_JBIST_MEAS_DONE_IN)
#define HWIO_LPASS_JBIST_MEAS_DONE_RESERVE_BITS31_1_BMSK                                                     0xfffffffe
#define HWIO_LPASS_JBIST_MEAS_DONE_RESERVE_BITS31_1_SHFT                                                            0x1
#define HWIO_LPASS_JBIST_MEAS_DONE_JBIST_MEAS_DONE_BMSK                                                             0x1
#define HWIO_LPASS_JBIST_MEAS_DONE_JBIST_MEAS_DONE_SHFT                                                             0x0

#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00046000)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x00046000)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S0_1_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_INT_I2S0_1_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x00046004)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x00046004)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S0_1_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_INT_I2S0_1_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_INT_I2S0_1_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00046008)
#define HWIO_LPASS_INT_I2S0_1_M_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x00046008)
#define HWIO_LPASS_INT_I2S0_1_M_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_M_ADDR, HWIO_LPASS_INT_I2S0_1_M_RMSK)
#define HWIO_LPASS_INT_I2S0_1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_M_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_M_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_M_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_M_IN)
#define HWIO_LPASS_INT_I2S0_1_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_INT_I2S0_1_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_INT_I2S0_1_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004600c)
#define HWIO_LPASS_INT_I2S0_1_N_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0004600c)
#define HWIO_LPASS_INT_I2S0_1_N_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_N_ADDR, HWIO_LPASS_INT_I2S0_1_N_RMSK)
#define HWIO_LPASS_INT_I2S0_1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_N_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_N_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_N_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_N_IN)
#define HWIO_LPASS_INT_I2S0_1_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_INT_I2S0_1_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S0_1_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x00046010)
#define HWIO_LPASS_INT_I2S0_1_D_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x00046010)
#define HWIO_LPASS_INT_I2S0_1_D_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S0_1_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_D_ADDR, HWIO_LPASS_INT_I2S0_1_D_RMSK)
#define HWIO_LPASS_INT_I2S0_1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S0_1_D_ADDR, m)
#define HWIO_LPASS_INT_I2S0_1_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S0_1_D_ADDR,v)
#define HWIO_LPASS_INT_I2S0_1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S0_1_D_ADDR,m,v,HWIO_LPASS_INT_I2S0_1_D_IN)
#define HWIO_LPASS_INT_I2S0_1_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_INT_I2S0_1_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00046014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00046014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00046018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00046018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0004601c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_OFFS                                                 (LPASS_CC_REG_BASE_OFFS + 0x0004601c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR                                                    (LPASS_CC_REG_BASE      + 0x00046020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_OFFS                                                    (LPASS_CC_REG_BASE_OFFS + 0x00046020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S0_1_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                           0x0

#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00047000)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00047000)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S2_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S2_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S2_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00047004)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00047004)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S2_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S2_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S2_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S2_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00047008)
#define HWIO_LPASS_INT_I2S2_M_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00047008)
#define HWIO_LPASS_INT_I2S2_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_M_ADDR, HWIO_LPASS_INT_I2S2_M_RMSK)
#define HWIO_LPASS_INT_I2S2_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_M_ADDR, m)
#define HWIO_LPASS_INT_I2S2_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_M_ADDR,v)
#define HWIO_LPASS_INT_I2S2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_M_ADDR,m,v,HWIO_LPASS_INT_I2S2_M_IN)
#define HWIO_LPASS_INT_I2S2_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S2_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S2_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004700c)
#define HWIO_LPASS_INT_I2S2_N_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x0004700c)
#define HWIO_LPASS_INT_I2S2_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_N_ADDR, HWIO_LPASS_INT_I2S2_N_RMSK)
#define HWIO_LPASS_INT_I2S2_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_N_ADDR, m)
#define HWIO_LPASS_INT_I2S2_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_N_ADDR,v)
#define HWIO_LPASS_INT_I2S2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_N_ADDR,m,v,HWIO_LPASS_INT_I2S2_N_IN)
#define HWIO_LPASS_INT_I2S2_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S2_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S2_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00047010)
#define HWIO_LPASS_INT_I2S2_D_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00047010)
#define HWIO_LPASS_INT_I2S2_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S2_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S2_D_ADDR, HWIO_LPASS_INT_I2S2_D_RMSK)
#define HWIO_LPASS_INT_I2S2_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S2_D_ADDR, m)
#define HWIO_LPASS_INT_I2S2_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S2_D_ADDR,v)
#define HWIO_LPASS_INT_I2S2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S2_D_ADDR,m,v,HWIO_LPASS_INT_I2S2_D_IN)
#define HWIO_LPASS_INT_I2S2_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S2_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00047014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00047014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00047018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00047018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004701c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_OFFS                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004701c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00047020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00047020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S2_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00048000)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00048000)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S3_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S3_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S3_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00048004)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00048004)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S3_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S3_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S3_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S3_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00048008)
#define HWIO_LPASS_INT_I2S3_M_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00048008)
#define HWIO_LPASS_INT_I2S3_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_M_ADDR, HWIO_LPASS_INT_I2S3_M_RMSK)
#define HWIO_LPASS_INT_I2S3_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_M_ADDR, m)
#define HWIO_LPASS_INT_I2S3_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_M_ADDR,v)
#define HWIO_LPASS_INT_I2S3_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_M_ADDR,m,v,HWIO_LPASS_INT_I2S3_M_IN)
#define HWIO_LPASS_INT_I2S3_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S3_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S3_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004800c)
#define HWIO_LPASS_INT_I2S3_N_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x0004800c)
#define HWIO_LPASS_INT_I2S3_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_N_ADDR, HWIO_LPASS_INT_I2S3_N_RMSK)
#define HWIO_LPASS_INT_I2S3_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_N_ADDR, m)
#define HWIO_LPASS_INT_I2S3_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_N_ADDR,v)
#define HWIO_LPASS_INT_I2S3_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_N_ADDR,m,v,HWIO_LPASS_INT_I2S3_N_IN)
#define HWIO_LPASS_INT_I2S3_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S3_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S3_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00048010)
#define HWIO_LPASS_INT_I2S3_D_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00048010)
#define HWIO_LPASS_INT_I2S3_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S3_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S3_D_ADDR, HWIO_LPASS_INT_I2S3_D_RMSK)
#define HWIO_LPASS_INT_I2S3_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S3_D_ADDR, m)
#define HWIO_LPASS_INT_I2S3_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S3_D_ADDR,v)
#define HWIO_LPASS_INT_I2S3_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S3_D_ADDR,m,v,HWIO_LPASS_INT_I2S3_D_IN)
#define HWIO_LPASS_INT_I2S3_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S3_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00048014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00048014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00048018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00048018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004801c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_OFFS                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004801c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00048020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00048020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S3_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00049000)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00049000)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_RMSK                                                                    0x800000f3
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S4_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S4_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_OFF_BMSK                                                           0x80000000
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_OFF_SHFT                                                                 0x1f
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_D_BMSK                                                                  0x80
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_D_SHFT                                                                   0x7
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_N_BMSK                                                                  0x40
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_N_SHFT                                                                   0x6
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_M_BMSK                                                                  0x20
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_M_SHFT                                                                   0x5
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                           0x10
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                            0x4
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_EN_BMSK                                                                   0x2
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_ROOT_EN_SHFT                                                                   0x1
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_UPDATE_BMSK                                                                    0x1
#define HWIO_LPASS_INT_I2S4_CMD_RCGR_UPDATE_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR                                                                    (LPASS_CC_REG_BASE      + 0x00049004)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_OFFS                                                                    (LPASS_CC_REG_BASE_OFFS + 0x00049004)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_RMSK                                                                        0x771f
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S4_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S4_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ALT_SRC_SEL_BMSK                                                            0x4000
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_ALT_SRC_SEL_SHFT                                                               0xe
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_MODE_BMSK                                                                   0x3000
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_MODE_SHFT                                                                      0xc
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_SEL_BMSK                                                                 0x700
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_SEL_SHFT                                                                   0x8
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_DIV_BMSK                                                                  0x1f
#define HWIO_LPASS_INT_I2S4_CFG_RCGR_SRC_DIV_SHFT                                                                   0x0

#define HWIO_LPASS_INT_I2S4_M_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00049008)
#define HWIO_LPASS_INT_I2S4_M_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00049008)
#define HWIO_LPASS_INT_I2S4_M_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_M_ADDR, HWIO_LPASS_INT_I2S4_M_RMSK)
#define HWIO_LPASS_INT_I2S4_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_M_ADDR, m)
#define HWIO_LPASS_INT_I2S4_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_M_ADDR,v)
#define HWIO_LPASS_INT_I2S4_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_M_ADDR,m,v,HWIO_LPASS_INT_I2S4_M_IN)
#define HWIO_LPASS_INT_I2S4_M_M_BMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S4_M_M_SHFT                                                                                0x0

#define HWIO_LPASS_INT_I2S4_N_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x0004900c)
#define HWIO_LPASS_INT_I2S4_N_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x0004900c)
#define HWIO_LPASS_INT_I2S4_N_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_N_ADDR, HWIO_LPASS_INT_I2S4_N_RMSK)
#define HWIO_LPASS_INT_I2S4_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_N_ADDR, m)
#define HWIO_LPASS_INT_I2S4_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_N_ADDR,v)
#define HWIO_LPASS_INT_I2S4_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_N_ADDR,m,v,HWIO_LPASS_INT_I2S4_N_IN)
#define HWIO_LPASS_INT_I2S4_N_NOT_N_MINUS_M_BMSK                                                                   0xff
#define HWIO_LPASS_INT_I2S4_N_NOT_N_MINUS_M_SHFT                                                                    0x0

#define HWIO_LPASS_INT_I2S4_D_ADDR                                                                           (LPASS_CC_REG_BASE      + 0x00049010)
#define HWIO_LPASS_INT_I2S4_D_OFFS                                                                           (LPASS_CC_REG_BASE_OFFS + 0x00049010)
#define HWIO_LPASS_INT_I2S4_D_RMSK                                                                                 0xff
#define HWIO_LPASS_INT_I2S4_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S4_D_ADDR, HWIO_LPASS_INT_I2S4_D_RMSK)
#define HWIO_LPASS_INT_I2S4_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S4_D_ADDR, m)
#define HWIO_LPASS_INT_I2S4_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S4_D_ADDR,v)
#define HWIO_LPASS_INT_I2S4_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S4_D_ADDR,m,v,HWIO_LPASS_INT_I2S4_D_IN)
#define HWIO_LPASS_INT_I2S4_D_NOT_2D_BMSK                                                                          0xff
#define HWIO_LPASS_INT_I2S4_D_NOT_2D_SHFT                                                                           0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00049014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00049014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR                                                        (LPASS_CC_REG_BASE      + 0x00049018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_OFFS                                                        (LPASS_CC_REG_BASE_OFFS + 0x00049018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_RMSK                                                        0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_HW_CTL_BMSK                                                        0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_HW_CTL_SHFT                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EBIT_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR                                                   (LPASS_CC_REG_BASE      + 0x0004901c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_OFFS                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004901c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_RMSK                                                   0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                  0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                        0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                     0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                        0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                               0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                                0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                           0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_EN_BMSK                                                   0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_EXT_CLK_DETECT_IRQ_EN_SHFT                                                   0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR                                                      (LPASS_CC_REG_BASE      + 0x00049020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x00049020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_RMSK                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                             0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S4_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                             0x0

#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004a000)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0004a000)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_RMSK                                                                  0x800000f3
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR, HWIO_LPASS_INT_I2S5_6_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_CMD_RCGR_IN)
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_OFF_BMSK                                                         0x80000000
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_OFF_SHFT                                                               0x1f
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_D_BMSK                                                                0x80
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_D_SHFT                                                                 0x7
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_N_BMSK                                                                0x40
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_N_SHFT                                                                 0x6
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_M_BMSK                                                                0x20
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_M_SHFT                                                                 0x5
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                         0x10
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                          0x4
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_EN_BMSK                                                                 0x2
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_ROOT_EN_SHFT                                                                 0x1
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_UPDATE_BMSK                                                                  0x1
#define HWIO_LPASS_INT_I2S5_6_CMD_RCGR_UPDATE_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004a004)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0004a004)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_RMSK                                                                      0x771f
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR, HWIO_LPASS_INT_I2S5_6_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_CFG_RCGR_IN)
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ALT_SRC_SEL_BMSK                                                          0x4000
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_ALT_SRC_SEL_SHFT                                                             0xe
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_MODE_BMSK                                                                 0x3000
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_MODE_SHFT                                                                    0xc
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_SEL_BMSK                                                               0x700
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_SEL_SHFT                                                                 0x8
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_DIV_BMSK                                                                0x1f
#define HWIO_LPASS_INT_I2S5_6_CFG_RCGR_SRC_DIV_SHFT                                                                 0x0

#define HWIO_LPASS_INT_I2S5_6_M_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a008)
#define HWIO_LPASS_INT_I2S5_6_M_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0004a008)
#define HWIO_LPASS_INT_I2S5_6_M_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_M_ADDR, HWIO_LPASS_INT_I2S5_6_M_RMSK)
#define HWIO_LPASS_INT_I2S5_6_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_M_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_M_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_M_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_M_IN)
#define HWIO_LPASS_INT_I2S5_6_M_M_BMSK                                                                             0xff
#define HWIO_LPASS_INT_I2S5_6_M_M_SHFT                                                                              0x0

#define HWIO_LPASS_INT_I2S5_6_N_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a00c)
#define HWIO_LPASS_INT_I2S5_6_N_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0004a00c)
#define HWIO_LPASS_INT_I2S5_6_N_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_N_ADDR, HWIO_LPASS_INT_I2S5_6_N_RMSK)
#define HWIO_LPASS_INT_I2S5_6_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_N_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_N_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_N_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_N_IN)
#define HWIO_LPASS_INT_I2S5_6_N_NOT_N_MINUS_M_BMSK                                                                 0xff
#define HWIO_LPASS_INT_I2S5_6_N_NOT_N_MINUS_M_SHFT                                                                  0x0

#define HWIO_LPASS_INT_I2S5_6_D_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004a010)
#define HWIO_LPASS_INT_I2S5_6_D_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0004a010)
#define HWIO_LPASS_INT_I2S5_6_D_RMSK                                                                               0xff
#define HWIO_LPASS_INT_I2S5_6_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_D_ADDR, HWIO_LPASS_INT_I2S5_6_D_RMSK)
#define HWIO_LPASS_INT_I2S5_6_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_I2S5_6_D_ADDR, m)
#define HWIO_LPASS_INT_I2S5_6_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_I2S5_6_D_ADDR,v)
#define HWIO_LPASS_INT_I2S5_6_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_I2S5_6_D_ADDR,m,v,HWIO_LPASS_INT_I2S5_6_D_IN)
#define HWIO_LPASS_INT_I2S5_6_D_NOT_2D_BMSK                                                                        0xff
#define HWIO_LPASS_INT_I2S5_6_D_NOT_2D_SHFT                                                                         0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0004a014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x0004a014)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR                                                      (LPASS_CC_REG_BASE      + 0x0004a018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_OFFS                                                      (LPASS_CC_REG_BASE_OFFS + 0x0004a018)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_RMSK                                                      0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_HW_CTL_BMSK                                                      0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_HW_CTL_SHFT                                                      0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EBIT_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR                                                 (LPASS_CC_REG_BASE      + 0x0004a01c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_OFFS                                                 (LPASS_CC_REG_BASE_OFFS + 0x0004a01c)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_RMSK                                                 0x80007f13
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_BMSK                                0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_INACTIVE_IRQ_SHFT                                      0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_VALUE_BMSK                                   0x7f00
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_VALUE_SHFT                                      0x8
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_CLEAR_BMSK                                             0x10
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_CLEAR_SHFT                                              0x4
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_EN_BMSK                                         0x2
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_CLK_COUNTER_EN_SHFT                                         0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_EN_BMSK                                                 0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_EXT_CLK_DETECT_IRQ_EN_SHFT                                                 0x0

#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR                                                    (LPASS_CC_REG_BASE      + 0x0004a020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_OFFS                                                    (LPASS_CC_REG_BASE_OFFS + 0x0004a020)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_RMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR, HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_TEST_CLK_ENABLE_BMSK                                           0x1
#define HWIO_LPASS_AUDIO_CORE_INT_I2S5_6_TEST_ENABLE_TEST_CLK_ENABLE_SHFT                                           0x0

#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004b000)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004b000)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR, HWIO_LPASS_INT_MCLK0_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK0_CMD_RCGR_IN)
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_INT_MCLK0_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004b004)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004b004)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR, HWIO_LPASS_INT_MCLK0_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK0_CFG_RCGR_IN)
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_INT_MCLK0_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_INT_MCLK0_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b008)
#define HWIO_LPASS_INT_MCLK0_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004b008)
#define HWIO_LPASS_INT_MCLK0_M_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_M_ADDR, HWIO_LPASS_INT_MCLK0_M_RMSK)
#define HWIO_LPASS_INT_MCLK0_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_M_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_M_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_M_ADDR,m,v,HWIO_LPASS_INT_MCLK0_M_IN)
#define HWIO_LPASS_INT_MCLK0_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_INT_MCLK0_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_INT_MCLK0_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b00c)
#define HWIO_LPASS_INT_MCLK0_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004b00c)
#define HWIO_LPASS_INT_MCLK0_N_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_N_ADDR, HWIO_LPASS_INT_MCLK0_N_RMSK)
#define HWIO_LPASS_INT_MCLK0_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_N_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_N_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_N_ADDR,m,v,HWIO_LPASS_INT_MCLK0_N_IN)
#define HWIO_LPASS_INT_MCLK0_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_INT_MCLK0_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK0_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004b010)
#define HWIO_LPASS_INT_MCLK0_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004b010)
#define HWIO_LPASS_INT_MCLK0_D_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK0_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_D_ADDR, HWIO_LPASS_INT_MCLK0_D_RMSK)
#define HWIO_LPASS_INT_MCLK0_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK0_D_ADDR, m)
#define HWIO_LPASS_INT_MCLK0_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK0_D_ADDR,v)
#define HWIO_LPASS_INT_MCLK0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK0_D_ADDR,m,v,HWIO_LPASS_INT_MCLK0_D_IN)
#define HWIO_LPASS_INT_MCLK0_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_INT_MCLK0_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004b014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x0004b014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK0_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004c000)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004c000)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_RMSK                                                                   0x800000f3
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR, HWIO_LPASS_INT_MCLK1_CMD_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_CMD_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK1_CMD_RCGR_IN)
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_D_BMSK                                                                 0x80
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_D_SHFT                                                                  0x7
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_N_BMSK                                                                 0x40
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_N_SHFT                                                                  0x6
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_M_BMSK                                                                 0x20
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_M_SHFT                                                                  0x5
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                          0x10
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                           0x4
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_EN_BMSK                                                                  0x2
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_ROOT_EN_SHFT                                                                  0x1
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_UPDATE_BMSK                                                                   0x1
#define HWIO_LPASS_INT_MCLK1_CMD_RCGR_UPDATE_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004c004)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004c004)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_RMSK                                                                       0x771f
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR, HWIO_LPASS_INT_MCLK1_CFG_RCGR_RMSK)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_CFG_RCGR_ADDR,m,v,HWIO_LPASS_INT_MCLK1_CFG_RCGR_IN)
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ALT_SRC_SEL_BMSK                                                           0x4000
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_ALT_SRC_SEL_SHFT                                                              0xe
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_MODE_BMSK                                                                  0x3000
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_MODE_SHFT                                                                     0xc
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_SEL_BMSK                                                                0x700
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_SEL_SHFT                                                                  0x8
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_DIV_BMSK                                                                 0x1f
#define HWIO_LPASS_INT_MCLK1_CFG_RCGR_SRC_DIV_SHFT                                                                  0x0

#define HWIO_LPASS_INT_MCLK1_M_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c008)
#define HWIO_LPASS_INT_MCLK1_M_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004c008)
#define HWIO_LPASS_INT_MCLK1_M_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_M_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_M_ADDR, HWIO_LPASS_INT_MCLK1_M_RMSK)
#define HWIO_LPASS_INT_MCLK1_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_M_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_M_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_M_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_M_ADDR,m,v,HWIO_LPASS_INT_MCLK1_M_IN)
#define HWIO_LPASS_INT_MCLK1_M_M_BMSK                                                                              0xff
#define HWIO_LPASS_INT_MCLK1_M_M_SHFT                                                                               0x0

#define HWIO_LPASS_INT_MCLK1_N_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c00c)
#define HWIO_LPASS_INT_MCLK1_N_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004c00c)
#define HWIO_LPASS_INT_MCLK1_N_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_N_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_N_ADDR, HWIO_LPASS_INT_MCLK1_N_RMSK)
#define HWIO_LPASS_INT_MCLK1_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_N_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_N_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_N_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_N_ADDR,m,v,HWIO_LPASS_INT_MCLK1_N_IN)
#define HWIO_LPASS_INT_MCLK1_N_NOT_N_MINUS_M_BMSK                                                                  0xff
#define HWIO_LPASS_INT_MCLK1_N_NOT_N_MINUS_M_SHFT                                                                   0x0

#define HWIO_LPASS_INT_MCLK1_D_ADDR                                                                          (LPASS_CC_REG_BASE      + 0x0004c010)
#define HWIO_LPASS_INT_MCLK1_D_OFFS                                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004c010)
#define HWIO_LPASS_INT_MCLK1_D_RMSK                                                                                0xff
#define HWIO_LPASS_INT_MCLK1_D_IN          \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_D_ADDR, HWIO_LPASS_INT_MCLK1_D_RMSK)
#define HWIO_LPASS_INT_MCLK1_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_INT_MCLK1_D_ADDR, m)
#define HWIO_LPASS_INT_MCLK1_D_OUT(v)      \
        out_dword(HWIO_LPASS_INT_MCLK1_D_ADDR,v)
#define HWIO_LPASS_INT_MCLK1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_INT_MCLK1_D_ADDR,m,v,HWIO_LPASS_INT_MCLK1_D_IN)
#define HWIO_LPASS_INT_MCLK1_D_NOT_2D_BMSK                                                                         0xff
#define HWIO_LPASS_INT_MCLK1_D_NOT_2D_SHFT                                                                          0x0

#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004c014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x0004c014)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_RMSK                                                            0x80000003
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_HW_CTL_BMSK                                                            0x2
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_HW_CTL_SHFT                                                            0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_LPASS_AUDIO_CORE_INT_MCLK1_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0004d000)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0004d000)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_RMSK                                                                      0x800000f3
#define HWIO_LPASS_SW_NPL_CMD_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR, HWIO_LPASS_SW_NPL_CMD_RCGR_RMSK)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR, m)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR,v)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_CMD_RCGR_ADDR,m,v,HWIO_LPASS_SW_NPL_CMD_RCGR_IN)
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_OFF_BMSK                                                             0x80000000
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_OFF_SHFT                                                                   0x1f
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_D_BMSK                                                                    0x80
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_D_SHFT                                                                     0x7
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_N_BMSK                                                                    0x40
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_N_SHFT                                                                     0x6
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_M_BMSK                                                                    0x20
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_M_SHFT                                                                     0x5
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                             0x10
#define HWIO_LPASS_SW_NPL_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                              0x4
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_EN_BMSK                                                                     0x2
#define HWIO_LPASS_SW_NPL_CMD_RCGR_ROOT_EN_SHFT                                                                     0x1
#define HWIO_LPASS_SW_NPL_CMD_RCGR_UPDATE_BMSK                                                                      0x1
#define HWIO_LPASS_SW_NPL_CMD_RCGR_UPDATE_SHFT                                                                      0x0

#define HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR                                                                      (LPASS_CC_REG_BASE      + 0x0004d004)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_OFFS                                                                      (LPASS_CC_REG_BASE_OFFS + 0x0004d004)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_RMSK                                                                          0x771f
#define HWIO_LPASS_SW_NPL_CFG_RCGR_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR, HWIO_LPASS_SW_NPL_CFG_RCGR_RMSK)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR, m)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR,v)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_CFG_RCGR_ADDR,m,v,HWIO_LPASS_SW_NPL_CFG_RCGR_IN)
#define HWIO_LPASS_SW_NPL_CFG_RCGR_ALT_SRC_SEL_BMSK                                                              0x4000
#define HWIO_LPASS_SW_NPL_CFG_RCGR_ALT_SRC_SEL_SHFT                                                                 0xe
#define HWIO_LPASS_SW_NPL_CFG_RCGR_MODE_BMSK                                                                     0x3000
#define HWIO_LPASS_SW_NPL_CFG_RCGR_MODE_SHFT                                                                        0xc
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_SEL_BMSK                                                                   0x700
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_SEL_SHFT                                                                     0x8
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_DIV_BMSK                                                                    0x1f
#define HWIO_LPASS_SW_NPL_CFG_RCGR_SRC_DIV_SHFT                                                                     0x0

#define HWIO_LPASS_SW_NPL_M_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d008)
#define HWIO_LPASS_SW_NPL_M_OFFS                                                                             (LPASS_CC_REG_BASE_OFFS + 0x0004d008)
#define HWIO_LPASS_SW_NPL_M_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_M_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_M_ADDR, HWIO_LPASS_SW_NPL_M_RMSK)
#define HWIO_LPASS_SW_NPL_M_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_M_ADDR, m)
#define HWIO_LPASS_SW_NPL_M_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_M_ADDR,v)
#define HWIO_LPASS_SW_NPL_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_M_ADDR,m,v,HWIO_LPASS_SW_NPL_M_IN)
#define HWIO_LPASS_SW_NPL_M_M_BMSK                                                                                 0xff
#define HWIO_LPASS_SW_NPL_M_M_SHFT                                                                                  0x0

#define HWIO_LPASS_SW_NPL_N_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d00c)
#define HWIO_LPASS_SW_NPL_N_OFFS                                                                             (LPASS_CC_REG_BASE_OFFS + 0x0004d00c)
#define HWIO_LPASS_SW_NPL_N_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_N_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_N_ADDR, HWIO_LPASS_SW_NPL_N_RMSK)
#define HWIO_LPASS_SW_NPL_N_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_N_ADDR, m)
#define HWIO_LPASS_SW_NPL_N_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_N_ADDR,v)
#define HWIO_LPASS_SW_NPL_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_N_ADDR,m,v,HWIO_LPASS_SW_NPL_N_IN)
#define HWIO_LPASS_SW_NPL_N_NOT_N_MINUS_M_BMSK                                                                     0xff
#define HWIO_LPASS_SW_NPL_N_NOT_N_MINUS_M_SHFT                                                                      0x0

#define HWIO_LPASS_SW_NPL_D_ADDR                                                                             (LPASS_CC_REG_BASE      + 0x0004d010)
#define HWIO_LPASS_SW_NPL_D_OFFS                                                                             (LPASS_CC_REG_BASE_OFFS + 0x0004d010)
#define HWIO_LPASS_SW_NPL_D_RMSK                                                                                   0xff
#define HWIO_LPASS_SW_NPL_D_IN          \
        in_dword_masked(HWIO_LPASS_SW_NPL_D_ADDR, HWIO_LPASS_SW_NPL_D_RMSK)
#define HWIO_LPASS_SW_NPL_D_INM(m)      \
        in_dword_masked(HWIO_LPASS_SW_NPL_D_ADDR, m)
#define HWIO_LPASS_SW_NPL_D_OUT(v)      \
        out_dword(HWIO_LPASS_SW_NPL_D_ADDR,v)
#define HWIO_LPASS_SW_NPL_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SW_NPL_D_ADDR,m,v,HWIO_LPASS_SW_NPL_D_IN)
#define HWIO_LPASS_SW_NPL_D_NOT_2D_BMSK                                                                            0xff
#define HWIO_LPASS_SW_NPL_D_NOT_2D_SHFT                                                                             0x0

#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0004d014)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0004d014)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_RMSK                                                               0x80000003
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_HW_CTL_BMSK                                                               0x2
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_HW_CTL_SHFT                                                               0x1
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_LPASS_AUDIO_CORE_SW_NPL_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR                                                              (LPASS_CC_REG_BASE      + 0x0004e000)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_OFFS                                                              (LPASS_CC_REG_BASE_OFFS + 0x0004e000)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_RMSK                                                                  0xffff
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_MUX_SEL_BMSK                                                          0xffff
#define HWIO_LPASS_LPASS_CC_DEBUG_MUX_MUXR_MUX_SEL_SHFT                                                             0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR                                                             (LPASS_CC_REG_BASE      + 0x0004e004)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_OFFS                                                             (LPASS_CC_REG_BASE_OFFS + 0x0004e004)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_RMSK                                                                    0x3
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_CLK_DIV_BMSK                                                            0x3
#define HWIO_LPASS_LPASS_CC_DEBUG_DIV_CDIVR_CLK_DIV_SHFT                                                            0x0

#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004e008)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0004e008)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_RMSK                                                                  0x80000001
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR, HWIO_LPASS_LPASS_CC_DEBUG_CBCR_RMSK)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_DEBUG_CBCR_ADDR,m,v,HWIO_LPASS_LPASS_CC_DEBUG_CBCR_IN)
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_LPASS_LPASS_CC_DEBUG_CBCR_CLK_ENABLE_SHFT                                                              0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0004e00c)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_OFFS                                                           (LPASS_CC_REG_BASE_OFFS + 0x0004e00c)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_RMSK                                                                  0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_BMSK                                                          0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_SHFT                                                          0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR                                                          (LPASS_CC_REG_BASE      + 0x0004e010)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_OFFS                                                          (LPASS_CC_REG_BASE_OFFS + 0x0004e010)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_RMSK                                                                 0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_BMSK                                                         0x3
#define HWIO_LPASS_LPASS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_SHFT                                                         0x0

#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR                                                               (LPASS_CC_REG_BASE      + 0x0004e014)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_OFFS                                                               (LPASS_CC_REG_BASE_OFFS + 0x0004e014)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_RMSK                                                               0x80000001
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR, HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_LPASS_LPASS_CC_PLL_TEST_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR                                                            (LPASS_CC_REG_BASE      + 0x0004e018)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_OFFS                                                            (LPASS_CC_REG_BASE_OFFS + 0x0004e018)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_RMSK                                                                   0x7
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_MUX_SEL_BMSK                                                           0x7
#define HWIO_LPASS_LPASS_CC_PLL_RESET_N_MUXR_MUX_SEL_SHFT                                                           0x0

#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR                                                           (LPASS_CC_REG_BASE      + 0x0004e020)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_OFFS                                                           (LPASS_CC_REG_BASE_OFFS + 0x0004e020)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_RMSK                                                                  0x7
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR, HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_RMSK)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR, m)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR,v)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_ADDR,m,v,HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_IN)
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_MUX_SEL_BMSK                                                          0x7
#define HWIO_LPASS_LPASS_CC_PLL_BYPASSNL_MUXR_MUX_SEL_SHFT                                                          0x0

#define HWIO_LPASS_TEST_BUS_SEL_ADDR                                                                         (LPASS_CC_REG_BASE      + 0x0004e024)
#define HWIO_LPASS_TEST_BUS_SEL_OFFS                                                                         (LPASS_CC_REG_BASE_OFFS + 0x0004e024)
#define HWIO_LPASS_TEST_BUS_SEL_RMSK                                                                                0x3
#define HWIO_LPASS_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_LPASS_TEST_BUS_SEL_ADDR, HWIO_LPASS_TEST_BUS_SEL_RMSK)
#define HWIO_LPASS_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TEST_BUS_SEL_ADDR, m)
#define HWIO_LPASS_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_LPASS_TEST_BUS_SEL_ADDR,v)
#define HWIO_LPASS_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TEST_BUS_SEL_ADDR,m,v,HWIO_LPASS_TEST_BUS_SEL_IN)
#define HWIO_LPASS_TEST_BUS_SEL_SEL_BMSK                                                                            0x3
#define HWIO_LPASS_TEST_BUS_SEL_SEL_SHFT                                                                            0x0

#define HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR                                                                   (LPASS_CC_REG_BASE      + 0x0004f000)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_OFFS                                                                   (LPASS_CC_REG_BASE_OFFS + 0x0004f000)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_RMSK                                                                   0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE_REG_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR, HWIO_LPASS_LPASS_CC_SPARE_REG_RMSK)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR, m)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR,v)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_SPARE_REG_ADDR,m,v,HWIO_LPASS_LPASS_CC_SPARE_REG_IN)
#define HWIO_LPASS_LPASS_CC_SPARE_REG_SPARE_BMSK                                                             0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE_REG_SPARE_SHFT                                                                    0x0

#define HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR                                                                  (LPASS_CC_REG_BASE      + 0x0004f004)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_OFFS                                                                  (LPASS_CC_REG_BASE_OFFS + 0x0004f004)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_RMSK                                                                  0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR, HWIO_LPASS_LPASS_CC_SPARE1_REG_RMSK)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR, m)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR,v)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CC_SPARE1_REG_ADDR,m,v,HWIO_LPASS_LPASS_CC_SPARE1_REG_IN)
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_SPARE1_BMSK                                                           0xffffffff
#define HWIO_LPASS_LPASS_CC_SPARE1_REG_SPARE1_SHFT                                                                  0x0


#endif /* __QDSP_PM_CLOCKS_HWIO_670_ADSP_H__ */
