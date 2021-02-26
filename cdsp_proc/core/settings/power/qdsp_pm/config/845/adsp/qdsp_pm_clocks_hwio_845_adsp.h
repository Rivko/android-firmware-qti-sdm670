#ifndef __LPASS_CLOCKS_HWIO_845_H__
#define __LPASS_CLOCKS_HWIO_845_H__
/*
===========================================================================
*/
/**
  @file lpass_clocks_hwio_845.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM845 (Nazgul) [nazgul_v1.0_p3q2r23.1]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_LPASS_CSR
    LPASS_LPASS_CC_REG

  'Include' filters applied: SYSNOC_MPORT[LPASS_LPASS_CC_REG] 
  'Exclude' filters applied: RESERVED DUMMY RESERVED[LPASS_LPASS_CSR]  DUMMY[LPASS_LPASS_CSR] 
*/
/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/845/adsp/qdsp_pm_clocks_hwio_845_adsp.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: LPASS_LPASS_CSR
 *--------------------------------------------------------------------------*/

#define LPASS_LPASS_CSR_REG_BASE                                                       (LPASS_BASE      + 0x000c0000)
#define LPASS_LPASS_CSR_REG_BASE_OFFS                                                  0x000c0000

#define HWIO_LPASS_CSR_GP_CTL_ADDR                                                     (LPASS_LPASS_CSR_REG_BASE      + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_OFFS                                                     (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_RMSK                                                     0xffff801f
#define HWIO_LPASS_CSR_GP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, HWIO_LPASS_CSR_GP_CTL_RMSK)
#define HWIO_LPASS_CSR_GP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, m)
#define HWIO_LPASS_CSR_GP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_GP_CTL_ADDR,v)
#define HWIO_LPASS_CSR_GP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_GP_CTL_ADDR,m,v,HWIO_LPASS_CSR_GP_CTL_IN)
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_BMSK                                         0xfffe0000
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_SHFT                                               0x11
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_BMSK                                              0x10000
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_SHFT                                                 0x10
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBI_ABORT_REQ_BMSK                                  0x8000
#define HWIO_LPASS_CSR_GP_CTL_DISABLE_AHBI_ABORT_REQ_SHFT                                     0xf
#define HWIO_LPASS_CSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_BMSK                                  0x10
#define HWIO_LPASS_CSR_GP_CTL_AHB2AHB_HMEM_FIX_DISABLE_SHFT                                   0x4
#define HWIO_LPASS_CSR_GP_CTL_TLB_PL_HPROT_NB_EN_BMSK                                         0x8
#define HWIO_LPASS_CSR_GP_CTL_TLB_PL_HPROT_NB_EN_SHFT                                         0x3
#define HWIO_LPASS_CSR_GP_CTL_SPDIFTX_HPROT_NB_EN_BMSK                                        0x4
#define HWIO_LPASS_CSR_GP_CTL_SPDIFTX_HPROT_NB_EN_SHFT                                        0x2
#define HWIO_LPASS_CSR_GP_CTL_DML_HPROT_NB_EN_BMSK                                            0x2
#define HWIO_LPASS_CSR_GP_CTL_DML_HPROT_NB_EN_SHFT                                            0x1
#define HWIO_LPASS_CSR_GP_CTL_RESMP_HPROT_NB_EN_BMSK                                          0x1
#define HWIO_LPASS_CSR_GP_CTL_RESMP_HPROT_NB_EN_SHFT                                          0x0

#define HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR                                                (LPASS_LPASS_CSR_REG_BASE      + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OFFS                                                (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK                                                0xf0ffffff
#define HWIO_LPASS_CSR_MEMTYPE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, m)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,v)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,m,v,HWIO_LPASS_CSR_MEMTYPE_CTL_IN)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_BMSK                                   0xe0000000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_SHFT                                         0x1d
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_EN_BMSK                                0x10000000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_CORE_MEMTYPE_EN_SHFT                                      0x1c
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_BMSK                                   0xe00000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_SHFT                                       0x15
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_EN_BMSK                                0x100000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_TLB_PL_MEMTYPE_EN_SHFT                                    0x14
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_BMSK                                   0xe0000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_SHFT                                      0x11
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_EN_BMSK                                0x10000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_SPDIFTX_MEMTYPE_EN_SHFT                                   0x10
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_BMSK                                     0xe000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_SHFT                                        0xd
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_EN_BMSK                                  0x1000
#define HWIO_LPASS_CSR_MEMTYPE_CTL_HDMIRX_MEMTYPE_EN_SHFT                                     0xc
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_BMSK                                       0xe00
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_SHFT                                         0x9
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_EN_BMSK                                    0x100
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RESMP_MEMTYPE_EN_SHFT                                      0x8
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_BMSK                                        0xe0
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_SHFT                                         0x5
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_BMSK                                     0x10
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_SHFT                                      0x4
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_BMSK                                           0xe
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_SHFT                                           0x1
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_EN_BMSK                                        0x1
#define HWIO_LPASS_CSR_MEMTYPE_CTL_DML_MEMTYPE_EN_SHFT                                        0x0

#define HWIO_LPASS_CSR_DEBUG_CTL_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00000010)
#define HWIO_LPASS_CSR_DEBUG_CTL_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000010)
#define HWIO_LPASS_CSR_DEBUG_CTL_RMSK                                                     0x1ff0f
#define HWIO_LPASS_CSR_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_CTL_ADDR, HWIO_LPASS_CSR_DEBUG_CTL_RMSK)
#define HWIO_LPASS_CSR_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_CTL_ADDR, m)
#define HWIO_LPASS_CSR_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_DEBUG_CTL_ADDR,v)
#define HWIO_LPASS_CSR_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_DEBUG_CTL_ADDR,m,v,HWIO_LPASS_CSR_DEBUG_CTL_IN)
#define HWIO_LPASS_CSR_DEBUG_CTL_LPASS_DEBUG_EN_BMSK                                      0x10000
#define HWIO_LPASS_CSR_DEBUG_CTL_LPASS_DEBUG_EN_SHFT                                         0x10
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_ADDR_BMSK                                             0xff00
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_ADDR_SHFT                                                0x8
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_BMSK                                                     0xf
#define HWIO_LPASS_CSR_DEBUG_CTL_SEL_SHFT                                                     0x0

#define HWIO_LPASS_CSR_DEBUG_BUS_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_RMSK                                                  0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_IN          \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, HWIO_LPASS_CSR_DEBUG_BUS_RMSK)
#define HWIO_LPASS_CSR_DEBUG_BUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, m)
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_BMSK                                             0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_SHFT                                                    0x0

#define HWIO_LPASS_CSR_MAXLIM_CTL_ADDR                                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00000018)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OFFS                                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000018)
#define HWIO_LPASS_CSR_MAXLIM_CTL_RMSK                                                       0x7f
#define HWIO_LPASS_CSR_MAXLIM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR, HWIO_LPASS_CSR_MAXLIM_CTL_RMSK)
#define HWIO_LPASS_CSR_MAXLIM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR, m)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR,v)
#define HWIO_LPASS_CSR_MAXLIM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_MAXLIM_CTL_ADDR,m,v,HWIO_LPASS_CSR_MAXLIM_CTL_IN)
#define HWIO_LPASS_CSR_MAXLIM_CTL_BYPASS_BMSK                                                0x40
#define HWIO_LPASS_CSR_MAXLIM_CTL_BYPASS_SHFT                                                 0x6
#define HWIO_LPASS_CSR_MAXLIM_CTL_MAX_WRITES_BMSK                                            0x3f
#define HWIO_LPASS_CSR_MAXLIM_CTL_MAX_WRITES_SHFT                                             0x0

#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ADDR                                           (LPASS_LPASS_CSR_REG_BASE      + 0x0000001c)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_OFFS                                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000001c)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_RMSK                                                  0x1
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ADDR, HWIO_LPASS_CSR_AXI_SPLITTER_CTL_RMSK)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ADDR, m)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ADDR,v)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ADDR,m,v,HWIO_LPASS_CSR_AXI_SPLITTER_CTL_IN)
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ENABLE_ONE_WAY_SPLIT_BMSK                             0x1
#define HWIO_LPASS_CSR_AXI_SPLITTER_CTL_ENABLE_ONE_WAY_SPLIT_SHFT                             0x0

#define HWIO_LPASS_AHBI_CFG_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x00001004)
#define HWIO_LPASS_AHBI_CFG_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001004)
#define HWIO_LPASS_AHBI_CFG_RMSK                                                        0xffffff1
#define HWIO_LPASS_AHBI_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, HWIO_LPASS_AHBI_CFG_RMSK)
#define HWIO_LPASS_AHBI_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, m)
#define HWIO_LPASS_AHBI_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_CFG_ADDR,v)
#define HWIO_LPASS_AHBI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_CFG_ADDR,m,v,HWIO_LPASS_AHBI_CFG_IN)
#define HWIO_LPASS_AHBI_CFG_RESAMPLER_PRIORITY_BMSK                                     0xf000000
#define HWIO_LPASS_AHBI_CFG_RESAMPLER_PRIORITY_SHFT                                          0x18
#define HWIO_LPASS_AHBI_CFG_SLIMBUS_PRIORITY_BMSK                                        0xf00000
#define HWIO_LPASS_AHBI_CFG_SLIMBUS_PRIORITY_SHFT                                            0x14
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_BMSK                                          0xf0000
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_SHFT                                             0x10
#define HWIO_LPASS_AHBI_CFG_DM_PRIORITY_BMSK                                               0xf000
#define HWIO_LPASS_AHBI_CFG_DM_PRIORITY_SHFT                                                  0xc
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_BMSK                                              0xf00
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_SHFT                                                0x8
#define HWIO_LPASS_AHBI_CFG_AUDIO_IF_PRIORITY_BMSK                                           0xf0
#define HWIO_LPASS_AHBI_CFG_AUDIO_IF_PRIORITY_SHFT                                            0x4
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_BMSK                                               0x1
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_SHFT                                               0x0

#define HWIO_LPASS_AHBI_LOCK_CTL_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00001008)
#define HWIO_LPASS_AHBI_LOCK_CTL_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001008)
#define HWIO_LPASS_AHBI_LOCK_CTL_RMSK                                                       0x3ff
#define HWIO_LPASS_AHBI_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, HWIO_LPASS_AHBI_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBI_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBI_LOCK_CTL_IN)
#define HWIO_LPASS_AHBI_LOCK_CTL_AHBX_LPM_LOCK_EN_BMSK                                      0x200
#define HWIO_LPASS_AHBI_LOCK_CTL_AHBX_LPM_LOCK_EN_SHFT                                        0x9
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                      0x100
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                        0x8
#define HWIO_LPASS_AHBI_LOCK_CTL_HDMI_LOCK_EN_BMSK                                           0x80
#define HWIO_LPASS_AHBI_LOCK_CTL_HDMI_LOCK_EN_SHFT                                            0x7
#define HWIO_LPASS_AHBI_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                        0x40
#define HWIO_LPASS_AHBI_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                         0x6
#define HWIO_LPASS_AHBI_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                      0x20
#define HWIO_LPASS_AHBI_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                       0x5
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                        0x10
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                         0x4
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_BMSK                                          0x8
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_SHFT                                          0x3
#define HWIO_LPASS_AHBI_LOCK_CTL_DM_LOCK_EN_BMSK                                              0x4
#define HWIO_LPASS_AHBI_LOCK_CTL_DM_LOCK_EN_SHFT                                              0x2
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_BMSK                                            0x2
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_SHFT                                            0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                        0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                        0x0

#define HWIO_LPASS_AHBI_CFG_EXT_ADDR                                                   (LPASS_LPASS_CSR_REG_BASE      + 0x0000100c)
#define HWIO_LPASS_AHBI_CFG_EXT_OFFS                                                   (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000100c)
#define HWIO_LPASS_AHBI_CFG_EXT_RMSK                                                       0xffff
#define HWIO_LPASS_AHBI_CFG_EXT_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_EXT_ADDR, HWIO_LPASS_AHBI_CFG_EXT_RMSK)
#define HWIO_LPASS_AHBI_CFG_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_EXT_ADDR, m)
#define HWIO_LPASS_AHBI_CFG_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_CFG_EXT_ADDR,v)
#define HWIO_LPASS_AHBI_CFG_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_CFG_EXT_ADDR,m,v,HWIO_LPASS_AHBI_CFG_EXT_IN)
#define HWIO_LPASS_AHBI_CFG_EXT_AHBX_LPM_PRIORITY_BMSK                                     0xf000
#define HWIO_LPASS_AHBI_CFG_EXT_AHBX_LPM_PRIORITY_SHFT                                        0xc
#define HWIO_LPASS_AHBI_CFG_EXT_SLIMBUS1_PRIORITY_BMSK                                      0xf00
#define HWIO_LPASS_AHBI_CFG_EXT_SLIMBUS1_PRIORITY_SHFT                                        0x8
#define HWIO_LPASS_AHBI_CFG_EXT_HDMI_PRIORITY_BMSK                                           0xf0
#define HWIO_LPASS_AHBI_CFG_EXT_HDMI_PRIORITY_SHFT                                            0x4
#define HWIO_LPASS_AHBI_CFG_EXT_SPDIFTX_PRIORITY_BMSK                                         0xf
#define HWIO_LPASS_AHBI_CFG_EXT_SPDIFTX_PRIORITY_SHFT                                         0x0

#define HWIO_LPASS_AHBX_CFG_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x00001020)
#define HWIO_LPASS_AHBX_CFG_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001020)
#define HWIO_LPASS_AHBX_CFG_RMSK                                                       0x7703f77f
#define HWIO_LPASS_AHBX_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, HWIO_LPASS_AHBX_CFG_RMSK)
#define HWIO_LPASS_AHBX_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, m)
#define HWIO_LPASS_AHBX_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_CFG_ADDR,v)
#define HWIO_LPASS_AHBX_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_CFG_ADDR,m,v,HWIO_LPASS_AHBX_CFG_IN)
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_BMSK                                    0x70000000
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_SHFT                                          0x1c
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_BMSK                                       0x7000000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_SHFT                                            0x18
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_BMSK                                        0x38000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_SHFT                                            0xf
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_BMSK                                               0x7000
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_SHFT                                                  0xc
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_BMSK                                              0x700
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_SHFT                                                0x8
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_BMSK                                           0x70
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_SHFT                                            0x4
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_BMSK                                             0xe
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_SHFT                                             0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_BMSK                                               0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_SHFT                                               0x0

#define HWIO_LPASS_AHBX_LOCK_CTL_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00001024)
#define HWIO_LPASS_AHBX_LOCK_CTL_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001024)
#define HWIO_LPASS_AHBX_LOCK_CTL_RMSK                                                       0x7e5
#define HWIO_LPASS_AHBX_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, HWIO_LPASS_AHBX_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBX_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBX_LOCK_CTL_IN)
#define HWIO_LPASS_AHBX_LOCK_CTL_TLB_PL_LOCK_EN_BMSK                                        0x400
#define HWIO_LPASS_AHBX_LOCK_CTL_TLB_PL_LOCK_EN_SHFT                                          0xa
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                      0x200
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                        0x9
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_BMSK                                          0x100
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_SHFT                                            0x8
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                        0x80
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                         0x7
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                      0x40
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                       0x6
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                        0x20
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                         0x5
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_BMSK                                              0x4
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_SHFT                                              0x2
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                        0x1
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                        0x0

#define HWIO_LPASS_AHBX_CFG_EXT_ADDR                                                   (LPASS_LPASS_CSR_REG_BASE      + 0x00001028)
#define HWIO_LPASS_AHBX_CFG_EXT_OFFS                                                   (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001028)
#define HWIO_LPASS_AHBX_CFG_EXT_RMSK                                                          0x7
#define HWIO_LPASS_AHBX_CFG_EXT_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_EXT_ADDR, HWIO_LPASS_AHBX_CFG_EXT_RMSK)
#define HWIO_LPASS_AHBX_CFG_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_EXT_ADDR, m)
#define HWIO_LPASS_AHBX_CFG_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_CFG_EXT_ADDR,v)
#define HWIO_LPASS_AHBX_CFG_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_CFG_EXT_ADDR,m,v,HWIO_LPASS_AHBX_CFG_EXT_IN)
#define HWIO_LPASS_AHBX_CFG_EXT_TLB_PL_PRIORITY_BMSK                                          0x7
#define HWIO_LPASS_AHBX_CFG_EXT_TLB_PL_PRIORITY_SHFT                                          0x0

#define HWIO_LPASS_AHBIX_STATUS_ADDR                                                   (LPASS_LPASS_CSR_REG_BASE      + 0x00001030)
#define HWIO_LPASS_AHBIX_STATUS_OFFS                                                   (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001030)
#define HWIO_LPASS_AHBIX_STATUS_RMSK                                                         0x3f
#define HWIO_LPASS_AHBIX_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, HWIO_LPASS_AHBIX_STATUS_RMSK)
#define HWIO_LPASS_AHBIX_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, m)
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_BMSK                              0x20
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_SHFT                               0x5
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_BMSK                                       0x10
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_SHFT                                        0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_BMSK                                          0x8
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_SHFT                                          0x3
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_BMSK                               0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_SHFT                               0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_BMSK                                        0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_SHFT                                        0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_BMSK                                          0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_SHFT                                          0x0

#define HWIO_LPASS_AHBIX_ACK_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00001034)
#define HWIO_LPASS_AHBIX_ACK_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001034)
#define HWIO_LPASS_AHBIX_ACK_RMSK                                                            0x3f
#define HWIO_LPASS_AHBIX_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_ACK_ADDR,v)
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                 0x20
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                  0x5
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_BMSK                                          0x10
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_SHFT                                           0x4
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_BMSK                                             0x8
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_SHFT                                             0x3
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                  0x4
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                  0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_BMSK                                           0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_SHFT                                           0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_BMSK                                             0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_SHFT                                             0x0

#define HWIO_LPASS_AHBIX_EN_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x00001038)
#define HWIO_LPASS_AHBIX_EN_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00001038)
#define HWIO_LPASS_AHBIX_EN_RMSK                                                             0x3f
#define HWIO_LPASS_AHBIX_EN_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, HWIO_LPASS_AHBIX_EN_RMSK)
#define HWIO_LPASS_AHBIX_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, m)
#define HWIO_LPASS_AHBIX_EN_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_EN_ADDR,v)
#define HWIO_LPASS_AHBIX_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBIX_EN_ADDR,m,v,HWIO_LPASS_AHBIX_EN_IN)
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                  0x20
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                   0x5
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_BMSK                                           0x10
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_SHFT                                            0x4
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_BMSK                                              0x8
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_SHFT                                              0x3
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                   0x4
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                   0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_BMSK                                            0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_SHFT                                            0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_BMSK                                              0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_SHFT                                              0x0

#define HWIO_LPASS_DML_CTL_ADDR                                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00002000)
#define HWIO_LPASS_DML_CTL_OFFS                                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002000)
#define HWIO_LPASS_DML_CTL_RMSK                                                        0x8000001f
#define HWIO_LPASS_DML_CTL_IN          \
        in_dword_masked(HWIO_LPASS_DML_CTL_ADDR, HWIO_LPASS_DML_CTL_RMSK)
#define HWIO_LPASS_DML_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_CTL_ADDR, m)
#define HWIO_LPASS_DML_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_DML_CTL_ADDR,v)
#define HWIO_LPASS_DML_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_CTL_ADDR,m,v,HWIO_LPASS_DML_CTL_IN)
#define HWIO_LPASS_DML_CTL_SOFT_RESET_BMSK                                             0x80000000
#define HWIO_LPASS_DML_CTL_SOFT_RESET_SHFT                                                   0x1f
#define HWIO_LPASS_DML_CTL_CRYPTO_EN_BMSK                                                    0x10
#define HWIO_LPASS_DML_CTL_CRYPTO_EN_SHFT                                                     0x4
#define HWIO_LPASS_DML_CTL_SWAP_BMSK                                                          0xc
#define HWIO_LPASS_DML_CTL_SWAP_SHFT                                                          0x2
#define HWIO_LPASS_DML_CTL_CMD_BMSK                                                           0x3
#define HWIO_LPASS_DML_CTL_CMD_SHFT                                                           0x0

#define HWIO_LPASS_DML_SRC_ADDR                                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00002004)
#define HWIO_LPASS_DML_SRC_OFFS                                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002004)
#define HWIO_LPASS_DML_SRC_RMSK                                                        0xfffffffc
#define HWIO_LPASS_DML_SRC_IN          \
        in_dword_masked(HWIO_LPASS_DML_SRC_ADDR, HWIO_LPASS_DML_SRC_RMSK)
#define HWIO_LPASS_DML_SRC_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_SRC_ADDR, m)
#define HWIO_LPASS_DML_SRC_OUT(v)      \
        out_dword(HWIO_LPASS_DML_SRC_ADDR,v)
#define HWIO_LPASS_DML_SRC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_SRC_ADDR,m,v,HWIO_LPASS_DML_SRC_IN)
#define HWIO_LPASS_DML_SRC_ADDR_BMSK                                                   0xfffffffc
#define HWIO_LPASS_DML_SRC_ADDR_SHFT                                                          0x2

#define HWIO_LPASS_DML_DST_ADDR                                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00002008)
#define HWIO_LPASS_DML_DST_OFFS                                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002008)
#define HWIO_LPASS_DML_DST_RMSK                                                        0xfffffffc
#define HWIO_LPASS_DML_DST_IN          \
        in_dword_masked(HWIO_LPASS_DML_DST_ADDR, HWIO_LPASS_DML_DST_RMSK)
#define HWIO_LPASS_DML_DST_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_DST_ADDR, m)
#define HWIO_LPASS_DML_DST_OUT(v)      \
        out_dword(HWIO_LPASS_DML_DST_ADDR,v)
#define HWIO_LPASS_DML_DST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_DST_ADDR,m,v,HWIO_LPASS_DML_DST_IN)
#define HWIO_LPASS_DML_DST_ADDR_BMSK                                                   0xfffffffc
#define HWIO_LPASS_DML_DST_ADDR_SHFT                                                          0x2

#define HWIO_LPASS_DML_TRAN_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x0000200c)
#define HWIO_LPASS_DML_TRAN_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000200c)
#define HWIO_LPASS_DML_TRAN_RMSK                                                          0xffffc
#define HWIO_LPASS_DML_TRAN_IN          \
        in_dword_masked(HWIO_LPASS_DML_TRAN_ADDR, HWIO_LPASS_DML_TRAN_RMSK)
#define HWIO_LPASS_DML_TRAN_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_TRAN_ADDR, m)
#define HWIO_LPASS_DML_TRAN_OUT(v)      \
        out_dword(HWIO_LPASS_DML_TRAN_ADDR,v)
#define HWIO_LPASS_DML_TRAN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_TRAN_ADDR,m,v,HWIO_LPASS_DML_TRAN_IN)
#define HWIO_LPASS_DML_TRAN_LEN_BMSK                                                      0xffffc
#define HWIO_LPASS_DML_TRAN_LEN_SHFT                                                          0x2

#define HWIO_LPASS_DML_STATUS_ADDR                                                     (LPASS_LPASS_CSR_REG_BASE      + 0x00002010)
#define HWIO_LPASS_DML_STATUS_OFFS                                                     (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002010)
#define HWIO_LPASS_DML_STATUS_RMSK                                                       0xffffc3
#define HWIO_LPASS_DML_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_DML_STATUS_ADDR, HWIO_LPASS_DML_STATUS_RMSK)
#define HWIO_LPASS_DML_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_STATUS_ADDR, m)
#define HWIO_LPASS_DML_STATUS_WORDS_BMSK                                                 0xffffc0
#define HWIO_LPASS_DML_STATUS_WORDS_SHFT                                                      0x6
#define HWIO_LPASS_DML_STATUS_ERROR_BMSK                                                      0x2
#define HWIO_LPASS_DML_STATUS_ERROR_SHFT                                                      0x1
#define HWIO_LPASS_DML_STATUS_DONE_BMSK                                                       0x1
#define HWIO_LPASS_DML_STATUS_DONE_SHFT                                                       0x0

#define HWIO_LPASS_DML_ACK_ADDR                                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00002014)
#define HWIO_LPASS_DML_ACK_OFFS                                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002014)
#define HWIO_LPASS_DML_ACK_RMSK                                                               0x3
#define HWIO_LPASS_DML_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_DML_ACK_ADDR,v)
#define HWIO_LPASS_DML_ACK_ERROR_BMSK                                                         0x2
#define HWIO_LPASS_DML_ACK_ERROR_SHFT                                                         0x1
#define HWIO_LPASS_DML_ACK_DONE_BMSK                                                          0x1
#define HWIO_LPASS_DML_ACK_DONE_SHFT                                                          0x0

#define HWIO_LPASS_DML_HW_VERSION_ADDR                                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00002018)
#define HWIO_LPASS_DML_HW_VERSION_OFFS                                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002018)
#define HWIO_LPASS_DML_HW_VERSION_RMSK                                                 0xffffffff
#define HWIO_LPASS_DML_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_DML_HW_VERSION_ADDR, HWIO_LPASS_DML_HW_VERSION_RMSK)
#define HWIO_LPASS_DML_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_HW_VERSION_ADDR, m)
#define HWIO_LPASS_DML_HW_VERSION_MAJOR_BMSK                                           0xf0000000
#define HWIO_LPASS_DML_HW_VERSION_MAJOR_SHFT                                                 0x1c
#define HWIO_LPASS_DML_HW_VERSION_MINOR_BMSK                                            0xfff0000
#define HWIO_LPASS_DML_HW_VERSION_MINOR_SHFT                                                 0x10
#define HWIO_LPASS_DML_HW_VERSION_STEP_BMSK                                                0xffff
#define HWIO_LPASS_DML_HW_VERSION_STEP_SHFT                                                   0x0

#define HWIO_LPASS_DML_SRC_EXT_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x0000201c)
#define HWIO_LPASS_DML_SRC_EXT_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000201c)
#define HWIO_LPASS_DML_SRC_EXT_RMSK                                                           0xf
#define HWIO_LPASS_DML_SRC_EXT_IN          \
        in_dword_masked(HWIO_LPASS_DML_SRC_EXT_ADDR, HWIO_LPASS_DML_SRC_EXT_RMSK)
#define HWIO_LPASS_DML_SRC_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_SRC_EXT_ADDR, m)
#define HWIO_LPASS_DML_SRC_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_DML_SRC_EXT_ADDR,v)
#define HWIO_LPASS_DML_SRC_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_SRC_EXT_ADDR,m,v,HWIO_LPASS_DML_SRC_EXT_IN)
#define HWIO_LPASS_DML_SRC_EXT_ADDR_BMSK                                                      0xf
#define HWIO_LPASS_DML_SRC_EXT_ADDR_SHFT                                                      0x0

#define HWIO_LPASS_DML_DST_EXT_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00002020)
#define HWIO_LPASS_DML_DST_EXT_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00002020)
#define HWIO_LPASS_DML_DST_EXT_RMSK                                                           0xf
#define HWIO_LPASS_DML_DST_EXT_IN          \
        in_dword_masked(HWIO_LPASS_DML_DST_EXT_ADDR, HWIO_LPASS_DML_DST_EXT_RMSK)
#define HWIO_LPASS_DML_DST_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_DML_DST_EXT_ADDR, m)
#define HWIO_LPASS_DML_DST_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_DML_DST_EXT_ADDR,v)
#define HWIO_LPASS_DML_DST_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DML_DST_EXT_ADDR,m,v,HWIO_LPASS_DML_DST_EXT_IN)
#define HWIO_LPASS_DML_DST_EXT_ADDR_BMSK                                                      0xf
#define HWIO_LPASS_DML_DST_EXT_ADDR_SHFT                                                      0x0

#define HWIO_LPASS_LPM_CTL_ADDR                                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00003000)
#define HWIO_LPASS_LPM_CTL_OFFS                                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00003000)
#define HWIO_LPASS_LPM_CTL_RMSK                                                        0x80000000
#define HWIO_LPASS_LPM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, HWIO_LPASS_LPM_CTL_RMSK)
#define HWIO_LPASS_LPM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, m)
#define HWIO_LPASS_LPM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPM_CTL_ADDR,v)
#define HWIO_LPASS_LPM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPM_CTL_ADDR,m,v,HWIO_LPASS_LPM_CTL_IN)
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_BMSK                                             0x80000000
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_SHFT                                                   0x1f

#define HWIO_LPASS_LPM_HW_VERSION_ADDR                                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00003004)
#define HWIO_LPASS_LPM_HW_VERSION_OFFS                                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00003004)
#define HWIO_LPASS_LPM_HW_VERSION_RMSK                                                 0xffffffff
#define HWIO_LPASS_LPM_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_LPM_HW_VERSION_ADDR, HWIO_LPASS_LPM_HW_VERSION_RMSK)
#define HWIO_LPASS_LPM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPM_HW_VERSION_ADDR, m)
#define HWIO_LPASS_LPM_HW_VERSION_MAJOR_BMSK                                           0xf0000000
#define HWIO_LPASS_LPM_HW_VERSION_MAJOR_SHFT                                                 0x1c
#define HWIO_LPASS_LPM_HW_VERSION_MINOR_BMSK                                            0xfff0000
#define HWIO_LPASS_LPM_HW_VERSION_MINOR_SHFT                                                 0x10
#define HWIO_LPASS_LPM_HW_VERSION_STEP_BMSK                                                0xffff
#define HWIO_LPASS_LPM_HW_VERSION_STEP_SHFT                                                   0x0

#define HWIO_LPASS_RESAMPLER_CTL_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00004000)
#define HWIO_LPASS_RESAMPLER_CTL_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00004000)
#define HWIO_LPASS_RESAMPLER_CTL_RMSK                                                  0xf0000000
#define HWIO_LPASS_RESAMPLER_CTL_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CTL_ADDR, HWIO_LPASS_RESAMPLER_CTL_RMSK)
#define HWIO_LPASS_RESAMPLER_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_CTL_ADDR, m)
#define HWIO_LPASS_RESAMPLER_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_RESAMPLER_CTL_ADDR,v)
#define HWIO_LPASS_RESAMPLER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_RESAMPLER_CTL_ADDR,m,v,HWIO_LPASS_RESAMPLER_CTL_IN)
#define HWIO_LPASS_RESAMPLER_CTL_SOFT_RESET_BMSK                                       0x80000000
#define HWIO_LPASS_RESAMPLER_CTL_SOFT_RESET_SHFT                                             0x1f
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_RESET_BMSK                                      0x40000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_RESET_SHFT                                            0x1e
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_CORE_CGC_EN_BMSK                                0x20000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_CORE_CGC_EN_SHFT                                      0x1d
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_AHB_CGC_EN_BMSK                                 0x10000000
#define HWIO_LPASS_RESAMPLER_CTL_FORCE_AHB_CGC_EN_SHFT                                       0x1c

#define HWIO_LPASS_RESAMPLER_STAT_ADDR                                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00004004)
#define HWIO_LPASS_RESAMPLER_STAT_OFFS                                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00004004)
#define HWIO_LPASS_RESAMPLER_STAT_RMSK                                                        0x1
#define HWIO_LPASS_RESAMPLER_STAT_IN          \
        in_dword_masked(HWIO_LPASS_RESAMPLER_STAT_ADDR, HWIO_LPASS_RESAMPLER_STAT_RMSK)
#define HWIO_LPASS_RESAMPLER_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_RESAMPLER_STAT_ADDR, m)
#define HWIO_LPASS_RESAMPLER_STAT_SOFT_RESET_STAT_BMSK                                        0x1
#define HWIO_LPASS_RESAMPLER_STAT_SOFT_RESET_STAT_SHFT                                        0x0

#define HWIO_LPASS_LPAIF_CTL_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00005000)
#define HWIO_LPASS_LPAIF_CTL_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00005000)
#define HWIO_LPASS_LPAIF_CTL_RMSK                                                      0x80000000
#define HWIO_LPASS_LPAIF_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, HWIO_LPASS_LPAIF_CTL_RMSK)
#define HWIO_LPASS_LPAIF_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, m)
#define HWIO_LPASS_LPAIF_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_CTL_ADDR,v)
#define HWIO_LPASS_LPAIF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_CTL_ADDR,m,v,HWIO_LPASS_LPAIF_CTL_IN)
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_BMSK                                           0x80000000
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_SHFT                                                 0x1f

#define HWIO_LPASS_SPDIFTX_CTL_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00006000)
#define HWIO_LPASS_SPDIFTX_CTL_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00006000)
#define HWIO_LPASS_SPDIFTX_CTL_RMSK                                                    0xc0000000
#define HWIO_LPASS_SPDIFTX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SPDIFTX_CTL_ADDR, HWIO_LPASS_SPDIFTX_CTL_RMSK)
#define HWIO_LPASS_SPDIFTX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SPDIFTX_CTL_ADDR, m)
#define HWIO_LPASS_SPDIFTX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SPDIFTX_CTL_ADDR,v)
#define HWIO_LPASS_SPDIFTX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SPDIFTX_CTL_ADDR,m,v,HWIO_LPASS_SPDIFTX_CTL_IN)
#define HWIO_LPASS_SPDIFTX_CTL_SOFT_RESET_BMSK                                         0x80000000
#define HWIO_LPASS_SPDIFTX_CTL_SOFT_RESET_SHFT                                               0x1f
#define HWIO_LPASS_SPDIFTX_CTL_FORCE_RESET_BMSK                                        0x40000000
#define HWIO_LPASS_SPDIFTX_CTL_FORCE_RESET_SHFT                                              0x1e

#define HWIO_LPASS_SPDIFTX_STAT_ADDR                                                   (LPASS_LPASS_CSR_REG_BASE      + 0x00006004)
#define HWIO_LPASS_SPDIFTX_STAT_OFFS                                                   (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00006004)
#define HWIO_LPASS_SPDIFTX_STAT_RMSK                                                          0x1
#define HWIO_LPASS_SPDIFTX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_SPDIFTX_STAT_ADDR, HWIO_LPASS_SPDIFTX_STAT_RMSK)
#define HWIO_LPASS_SPDIFTX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_SPDIFTX_STAT_ADDR, m)
#define HWIO_LPASS_SPDIFTX_STAT_SOFT_RESET_STAT_BMSK                                          0x1
#define HWIO_LPASS_SPDIFTX_STAT_SOFT_RESET_STAT_SHFT                                          0x0

#define HWIO_LPASS_HDMIRX_CTL_ADDR                                                     (LPASS_LPASS_CSR_REG_BASE      + 0x00007000)
#define HWIO_LPASS_HDMIRX_CTL_OFFS                                                     (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00007000)
#define HWIO_LPASS_HDMIRX_CTL_RMSK                                                     0xc0000000
#define HWIO_LPASS_HDMIRX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_HDMIRX_CTL_ADDR, HWIO_LPASS_HDMIRX_CTL_RMSK)
#define HWIO_LPASS_HDMIRX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMIRX_CTL_ADDR, m)
#define HWIO_LPASS_HDMIRX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_HDMIRX_CTL_ADDR,v)
#define HWIO_LPASS_HDMIRX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMIRX_CTL_ADDR,m,v,HWIO_LPASS_HDMIRX_CTL_IN)
#define HWIO_LPASS_HDMIRX_CTL_SOFT_RESET_BMSK                                          0x80000000
#define HWIO_LPASS_HDMIRX_CTL_SOFT_RESET_SHFT                                                0x1f
#define HWIO_LPASS_HDMIRX_CTL_FORCE_RESET_BMSK                                         0x40000000
#define HWIO_LPASS_HDMIRX_CTL_FORCE_RESET_SHFT                                               0x1e

#define HWIO_LPASS_HDMIRX_STAT_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00007004)
#define HWIO_LPASS_HDMIRX_STAT_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00007004)
#define HWIO_LPASS_HDMIRX_STAT_RMSK                                                           0x1
#define HWIO_LPASS_HDMIRX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_HDMIRX_STAT_ADDR, HWIO_LPASS_HDMIRX_STAT_RMSK)
#define HWIO_LPASS_HDMIRX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMIRX_STAT_ADDR, m)
#define HWIO_LPASS_HDMIRX_STAT_SOFT_RESET_STAT_BMSK                                           0x1
#define HWIO_LPASS_HDMIRX_STAT_SOFT_RESET_STAT_SHFT                                           0x0

#define HWIO_LPASS_HDMITX_CTL_ADDR                                                     (LPASS_LPASS_CSR_REG_BASE      + 0x00008000)
#define HWIO_LPASS_HDMITX_CTL_OFFS                                                     (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00008000)
#define HWIO_LPASS_HDMITX_CTL_RMSK                                                     0xc0000000
#define HWIO_LPASS_HDMITX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_CTL_ADDR, HWIO_LPASS_HDMITX_CTL_RMSK)
#define HWIO_LPASS_HDMITX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_CTL_ADDR, m)
#define HWIO_LPASS_HDMITX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_HDMITX_CTL_ADDR,v)
#define HWIO_LPASS_HDMITX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMITX_CTL_ADDR,m,v,HWIO_LPASS_HDMITX_CTL_IN)
#define HWIO_LPASS_HDMITX_CTL_SOFT_RESET_BMSK                                          0x80000000
#define HWIO_LPASS_HDMITX_CTL_SOFT_RESET_SHFT                                                0x1f
#define HWIO_LPASS_HDMITX_CTL_FORCE_RESET_BMSK                                         0x40000000
#define HWIO_LPASS_HDMITX_CTL_FORCE_RESET_SHFT                                               0x1e

#define HWIO_LPASS_HDMITX_STAT_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00008004)
#define HWIO_LPASS_HDMITX_STAT_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00008004)
#define HWIO_LPASS_HDMITX_STAT_RMSK                                                           0x1
#define HWIO_LPASS_HDMITX_STAT_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_STAT_ADDR, HWIO_LPASS_HDMITX_STAT_RMSK)
#define HWIO_LPASS_HDMITX_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_STAT_ADDR, m)
#define HWIO_LPASS_HDMITX_STAT_SOFT_RESET_STAT_BMSK                                           0x1
#define HWIO_LPASS_HDMITX_STAT_SOFT_RESET_STAT_SHFT                                           0x0

#define HWIO_LPASS_HDMITX_LEGACY_ADDR                                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00008008)
#define HWIO_LPASS_HDMITX_LEGACY_OFFS                                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00008008)
#define HWIO_LPASS_HDMITX_LEGACY_RMSK                                                         0x1
#define HWIO_LPASS_HDMITX_LEGACY_IN          \
        in_dword_masked(HWIO_LPASS_HDMITX_LEGACY_ADDR, HWIO_LPASS_HDMITX_LEGACY_RMSK)
#define HWIO_LPASS_HDMITX_LEGACY_INM(m)      \
        in_dword_masked(HWIO_LPASS_HDMITX_LEGACY_ADDR, m)
#define HWIO_LPASS_HDMITX_LEGACY_OUT(v)      \
        out_dword(HWIO_LPASS_HDMITX_LEGACY_ADDR,v)
#define HWIO_LPASS_HDMITX_LEGACY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_HDMITX_LEGACY_ADDR,m,v,HWIO_LPASS_HDMITX_LEGACY_IN)
#define HWIO_LPASS_HDMITX_LEGACY_LEGACY_EN_BMSK                                               0x1
#define HWIO_LPASS_HDMITX_LEGACY_LEGACY_EN_SHFT                                               0x0

#define HWIO_LPASS_TLB_PL_CTL_ADDR                                                     (LPASS_LPASS_CSR_REG_BASE      + 0x00009000)
#define HWIO_LPASS_TLB_PL_CTL_OFFS                                                     (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00009000)
#define HWIO_LPASS_TLB_PL_CTL_RMSK                                                     0xc0000000
#define HWIO_LPASS_TLB_PL_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TLB_PL_CTL_ADDR, HWIO_LPASS_TLB_PL_CTL_RMSK)
#define HWIO_LPASS_TLB_PL_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TLB_PL_CTL_ADDR, m)
#define HWIO_LPASS_TLB_PL_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TLB_PL_CTL_ADDR,v)
#define HWIO_LPASS_TLB_PL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TLB_PL_CTL_ADDR,m,v,HWIO_LPASS_TLB_PL_CTL_IN)
#define HWIO_LPASS_TLB_PL_CTL_SOFT_RESET_BMSK                                          0x80000000
#define HWIO_LPASS_TLB_PL_CTL_SOFT_RESET_SHFT                                                0x1f
#define HWIO_LPASS_TLB_PL_CTL_FORCE_RESET_BMSK                                         0x40000000
#define HWIO_LPASS_TLB_PL_CTL_FORCE_RESET_SHFT                                               0x1e

#define HWIO_LPASS_TLB_PL_STAT_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00009004)
#define HWIO_LPASS_TLB_PL_STAT_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00009004)
#define HWIO_LPASS_TLB_PL_STAT_RMSK                                                           0x1
#define HWIO_LPASS_TLB_PL_STAT_IN          \
        in_dword_masked(HWIO_LPASS_TLB_PL_STAT_ADDR, HWIO_LPASS_TLB_PL_STAT_RMSK)
#define HWIO_LPASS_TLB_PL_STAT_INM(m)      \
        in_dword_masked(HWIO_LPASS_TLB_PL_STAT_ADDR, m)
#define HWIO_LPASS_TLB_PL_STAT_SOFT_RESET_STAT_BMSK                                           0x1
#define HWIO_LPASS_TLB_PL_STAT_SOFT_RESET_STAT_SHFT                                           0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR                                  (LPASS_LPASS_CSR_REG_BASE      + 0x0000a000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OFFS                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000a000)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_RMSK                                         0x3
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_HW_CTL_BMSK                                  0x2
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_HW_CTL_SHFT                                  0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_CLK_ENABLE_BMSK                              0x1
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_CLK_ENABLE_SHFT                              0x0

#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR                            (LPASS_LPASS_CSR_REG_BASE      + 0x0000a004)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OFFS                            (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000a004)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_RMSK                                   0x7
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                    0x7
#define HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                    0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR                                       (LPASS_LPASS_CSR_REG_BASE      + 0x0000b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OFFS                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_RMSK                                              0x3
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_HW_CTL_BMSK                                       0x2
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_HW_CTL_SHFT                                       0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x0000b004)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000b004)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_RMSK                                        0x7
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                         0x7
#define HWIO_LPASS_AUDIO_CORE_QOS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                         0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR                                    (LPASS_LPASS_CSR_REG_BASE      + 0x0000c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OFFS                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000c000)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_RMSK                                           0x3
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_HW_CTL_BMSK                                    0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_HW_CTL_SHFT                                    0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_CLK_ENABLE_BMSK                                0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_CLK_ENABLE_SHFT                                0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR                              (LPASS_LPASS_CSR_REG_BASE      + 0x0000c004)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OFFS                              (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000c004)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_RMSK                                     0x7
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                      0x7
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                      0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR                                       (LPASS_LPASS_CSR_REG_BASE      + 0x0000d000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OFFS                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000d000)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_RMSK                                              0x3
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_HW_CTL_BMSK                                       0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_HW_CTL_SHFT                                       0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x0000d004)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000d004)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_RMSK                                        0x7
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                         0x7
#define HWIO_LPASS_AUDIO_CORE_LPM_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                         0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x0000e000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000e000)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_RMSK                                        0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_HW_CTL_BMSK                                 0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_HW_CTL_SHFT                                 0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_CLK_ENABLE_BMSK                             0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_CLK_ENABLE_SHFT                             0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR                           (LPASS_LPASS_CSR_REG_BASE      + 0x0000e004)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OFFS                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000e004)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_RMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                   0x7
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                   0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x0000f000)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000f000)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_RMSK                                        0x3
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_HW_CTL_BMSK                                 0x2
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_HW_CTL_SHFT                                 0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_CLK_ENABLE_BMSK                             0x1
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_CLK_ENABLE_SHFT                             0x0

#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR                           (LPASS_LPASS_CSR_REG_BASE      + 0x0000f004)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OFFS                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000f004)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_RMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                   0x7
#define HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                   0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR                               (LPASS_LPASS_CSR_REG_BASE      + 0x00010000)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OFFS                               (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00010000)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_RMSK                                      0x3
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_HW_CTL_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_HW_CTL_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_CLK_ENABLE_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR                         (LPASS_LPASS_CSR_REG_BASE      + 0x00010004)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OFFS                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00010004)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_RMSK                                0x7
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                 0x7
#define HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR                               (LPASS_LPASS_CSR_REG_BASE      + 0x00011000)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OFFS                               (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00011000)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_RMSK                                      0x3
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_HW_CTL_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_HW_CTL_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_CLK_ENABLE_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR                         (LPASS_LPASS_CSR_REG_BASE      + 0x00011004)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OFFS                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00011004)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_RMSK                                0x7
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                 0x7
#define HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00012000)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OFFS                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00012000)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_RMSK                                           0x3
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_HW_CTL_BMSK                                    0x2
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_HW_CTL_SHFT                                    0x1
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_CLK_ENABLE_BMSK                                0x1
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_CLK_ENABLE_SHFT                                0x0

#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR                              (LPASS_LPASS_CSR_REG_BASE      + 0x00012004)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OFFS                              (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00012004)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_RMSK                                     0x7
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                      0x7
#define HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                      0x0

#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00013000)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OFFS                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00013000)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_RMSK                                           0x3
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_HW_CTL_BMSK                                    0x2
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_HW_CTL_SHFT                                    0x1
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_CLK_ENABLE_BMSK                                0x1
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_CGCR_CLK_ENABLE_SHFT                                0x0

#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR                              (LPASS_LPASS_CSR_REG_BASE      + 0x00013004)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OFFS                              (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00013004)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_RMSK                                     0x7
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                      0x7
#define HWIO_LPASS_AUDIO_CORE_HDMIRX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                      0x0

#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR                               (LPASS_LPASS_CSR_REG_BASE      + 0x00014000)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OFFS                               (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00014000)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_RMSK                                      0x3
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_HW_CTL_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_HW_CTL_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_CLK_ENABLE_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_CLK_ENABLE_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR                         (LPASS_LPASS_CSR_REG_BASE      + 0x00014004)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OFFS                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00014004)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_RMSK                                0x7
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                 0x7
#define HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00015000)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OFFS                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00015000)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_RMSK                                           0x3
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_HW_CTL_BMSK                                    0x2
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_HW_CTL_SHFT                                    0x1
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_CLK_ENABLE_BMSK                                0x1
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_CLK_ENABLE_SHFT                                0x0

#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR                              (LPASS_LPASS_CSR_REG_BASE      + 0x00015004)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OFFS                              (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00015004)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_RMSK                                     0x7
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                      0x7
#define HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                      0x0

#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR                                       (LPASS_LPASS_CSR_REG_BASE      + 0x00016000)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OFFS                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00016000)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_RMSK                                              0x3
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_HW_CTL_BMSK                                       0x2
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_HW_CTL_SHFT                                       0x1
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00016004)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00016004)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_RMSK                                        0x7
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                         0x7
#define HWIO_LPASS_AUDIO_CORE_DML_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                         0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR                               (LPASS_LPASS_CSR_REG_BASE      + 0x00017000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OFFS                               (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00017000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_RMSK                                      0x3
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_HW_CTL_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_HW_CTL_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_CLK_ENABLE_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_CLK_ENABLE_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR                         (LPASS_LPASS_CSR_REG_BASE      + 0x00017004)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OFFS                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00017004)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_RMSK                                0x7
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                 0x7
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_ADDR                                 (LPASS_LPASS_CSR_REG_BASE      + 0x00018000)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_OFFS                                 (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00018000)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_RMSK                                        0x3
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_HW_CTL_BMSK                                 0x2
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_HW_CTL_SHFT                                 0x1
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_CLK_ENABLE_BMSK                             0x1
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_CLK_ENABLE_SHFT                             0x0

#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ADDR                           (LPASS_LPASS_CSR_REG_BASE      + 0x00018004)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_OFFS                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00018004)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_RMSK                                  0x7
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                   0x7
#define HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                   0x0

#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR                                   (LPASS_LPASS_CSR_REG_BASE      + 0x00019000)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OFFS                                   (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00019000)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_RMSK                                          0x3
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_HW_CTL_BMSK                                   0x2
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_HW_CTL_SHFT                                   0x1
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_CLK_ENABLE_BMSK                               0x1
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_CGCR_CLK_ENABLE_SHFT                               0x0

#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR                             (LPASS_LPASS_CSR_REG_BASE      + 0x00019004)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OFFS                             (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00019004)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_RMSK                                    0x7
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                     0x7
#define HWIO_LPASS_AUDIO_CORE_SPDIFTX_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                     0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR                               (LPASS_LPASS_CSR_REG_BASE      + 0x0001a000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OFFS                               (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0001a000)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_RMSK                                      0x3
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_HW_CTL_BMSK                               0x2
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_HW_CTL_SHFT                               0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_CLK_ENABLE_BMSK                           0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_CLK_ENABLE_SHFT                           0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR                         (LPASS_LPASS_CSR_REG_BASE      + 0x0001a004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OFFS                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0001a004)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_RMSK                                0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ACTIVE_HYSTSEL_BMSK                 0x7
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_HYSTERESIS_ACTIVE_HYSTSEL_SHFT                 0x0

#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR                            (LPASS_LPASS_CSR_REG_BASE      + 0x0001a008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OFFS                            (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0001a008)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_RMSK                                   0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_TIMEBASE_EN_BMSK                       0x1
#define HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_TIMEBASE_CTL_TIMEBASE_EN_SHFT                       0x0

#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR                                             (LPASS_LPASS_CSR_REG_BASE      + 0x0001b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OFFS                                             (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0001b000)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_RMSK                                                    0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR, HWIO_LPASS_AUDIO_CORE_QOS_CTL_RMSK)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_QOS_CTL_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_QOS_CTL_IN)
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_QOS_ENABLE_BMSK                                         0x1
#define HWIO_LPASS_AUDIO_CORE_QOS_CTL_QOS_ENABLE_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: LPASS_LPASS_CC_REG
 *--------------------------------------------------------------------------*/

#define LPASS_LPASS_CC_REG_REG_BASE                                                                                   (LPASS_BASE      + 0x00000000)
#define LPASS_LPASS_CC_REG_REG_BASE_OFFS                                                                              0x00000000

#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR                                                             (LPASS_LPASS_CC_REG_REG_BASE      + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OFFS                                                             (LPASS_LPASS_CC_REG_REG_BASE_OFFS + 0x00023000)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK                                                             0x80000003
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_BMSK                                                             0x2
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_HW_CTL_SHFT                                                             0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR_CLK_ENABLE_SHFT                                                         0x0


#endif /* __LPASS_CLOCKS_HWIO_845_H__ */
