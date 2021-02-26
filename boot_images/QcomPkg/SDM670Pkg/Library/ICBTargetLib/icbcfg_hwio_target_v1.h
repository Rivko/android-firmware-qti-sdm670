#ifndef __ICBCFG_HWIO_TARGET_V1_H__
#define __ICBCFG_HWIO_TARGET_V1_H__
/*
===========================================================================
*/
/**
  @file icbcfg_hwio_target_v1.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r36.1.0]
 
  This file contains HWIO register definitions for the following modules:
    MEMNOC_MEM_NOC

  'Include' filters applied: TRANSLATOR_SWID_LOW[MEMNOC_MEM_NOC] 
  'Exclude' filters applied: 
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ICBTargetLib/icbcfg_hwio_target_v1.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MEMNOC_MEM_NOC
 *--------------------------------------------------------------------------*/

#define MEMNOC_MEM_NOC_REG_BASE                                                                             (DDR_SS_BASE      + 0x00280000)
#define MEMNOC_MEM_NOC_REG_BASE_PHYS                                                                        (DDR_SS_BASE_PHYS + 0x00280000)

#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_ADDR                                                     (MEMNOC_MEM_NOC_REG_BASE      + 0x00023000)
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_PHYS                                                     (MEMNOC_MEM_NOC_REG_BASE_PHYS + 0x00023000)
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_RMSK                                                       0xffffff
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_ADDR, HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_ADDR, m)
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_BMSK                                            0xff0000
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_SHFT                                                0x10
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_BMSK                                              0xffff
#define HWIO_MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_SHFT                                                 0x0

#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_ADDR                                                     (MEMNOC_MEM_NOC_REG_BASE      + 0x00023080)
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_PHYS                                                     (MEMNOC_MEM_NOC_REG_BASE_PHYS + 0x00023080)
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_RMSK                                                       0xffffff
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_ADDR, HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_ADDR, m)
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_BMSK                                            0xff0000
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_SHFT                                                0x10
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_BMSK                                              0xffff
#define HWIO_MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_SHFT                                                 0x0

#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_ADDR                                                     (MEMNOC_MEM_NOC_REG_BASE      + 0x00023100)
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_PHYS                                                     (MEMNOC_MEM_NOC_REG_BASE_PHYS + 0x00023100)
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_RMSK                                                       0xffffff
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_ADDR, HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_ADDR, m)
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_BMSK                                            0xff0000
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_SHFT                                                0x10
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_BMSK                                              0xffff
#define HWIO_MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_SHFT                                                 0x0

#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_ADDR                                                     (MEMNOC_MEM_NOC_REG_BASE      + 0x00023180)
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_PHYS                                                     (MEMNOC_MEM_NOC_REG_BASE_PHYS + 0x00023180)
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_RMSK                                                       0xffffff
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_ADDR, HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_ADDR, m)
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_BMSK                                            0xff0000
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_SHFT                                                0x10
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_BMSK                                              0xffff
#define HWIO_MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_SHFT                                                 0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_ADDR                                                              (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f000)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_RMSK                                                                0xffffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_UNITTYPEID_BMSK                                                     0xff0000
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_UNITTYPEID_SHFT                                                         0x10
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_UNITCONFID_BMSK                                                       0xffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_LOW_UNITCONFID_SHFT                                                          0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_ADDR                                                             (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f004)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_RMSK                                                                 0xffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_QNOCID_BMSK                                                          0xffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_SWID_HIGH_QNOCID_SHFT                                                             0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ADDR                                                           (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f008)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RMSK                                                             0x1fffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CACHEINDEXOV_BMSK                                                0x100000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CACHEINDEXOV_SHFT                                                    0x14
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV3_BMSK                                                         0x80000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV3_SHFT                                                            0x13
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCCTLOV_BMSK                                                   0x40000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCCTLOV_SHFT                                                      0x12
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCWROV_BMSK                                                    0x20000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCWROV_SHFT                                                       0x11
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCRDOV_BMSK                                                    0x10000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_ALLOCRDOV_SHFT                                                       0x10
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPWRVAL_BMSK                                                     0x8000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPWRVAL_SHFT                                                        0xf
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPWROV_BMSK                                                      0x4000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPWROV_SHFT                                                         0xe
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPRDVAL_BMSK                                                     0x2000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPRDVAL_SHFT                                                        0xd
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPRDOV_BMSK                                                      0x1000
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_CMFPRDOV_SHFT                                                         0xc
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV2_BMSK                                                           0xc00
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV2_SHFT                                                             0xa
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_DEVICEVAL_BMSK                                                      0x200
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_DEVICEVAL_SHFT                                                        0x9
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_DEVICEOV_BMSK                                                       0x100
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_DEVICEOV_SHFT                                                         0x8
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV1_BMSK                                                            0xe0
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV1_SHFT                                                             0x5
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_QOSDISABLE_BMSK                                                      0x10
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_QOSDISABLE_SHFT                                                       0x4
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV0_BMSK                                                             0xc
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_RSV0_SHFT                                                             0x2
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_MODE_BMSK                                                             0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW_MODE_SHFT                                                             0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_ADDR                                                        (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f010)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_RMSK                                                               0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_URGPENDING_BMSK                                                    0x2
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_URGPENDING_SHFT                                                    0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_TRPENDING_BMSK                                                     0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_MAINSTATUS_LOW_TRPENDING_SHFT                                                     0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_ADDR                                                         (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f018)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_RMSK                                                                0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_REQUSEROV_BMSK                                                      0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSEROV_LOW_REQUSEROV_SHFT                                                      0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_ADDR                                                        (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f020)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_RMSK                                                               0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_REQUSERVAL_BMSK                                                    0x1
#define HWIO_MEMNOC_TCU_DYN_ATTR_REQUSERVAL_LOW_REQUSERVAL_SHFT                                                    0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_ADDR                                                         (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f028)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_RMSK                                                              0x7ff
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_LOGUSEROV_BMSK                                                    0x7ff
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSEROV_LOW_LOGUSEROV_SHFT                                                      0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_ADDR                                                        (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f030)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_RMSK                                                             0x7ff
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_LOGUSERVAL_BMSK                                                  0x7ff
#define HWIO_MEMNOC_TCU_DYN_ATTR_LOGUSERVAL_LOW_LOGUSERVAL_SHFT                                                    0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDR                                                         (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f038)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_RMSK                                                                0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDRMSBOV_BMSK                                                      0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBOV_LOW_ADDRMSBOV_SHFT                                                      0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDR                                                        (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f040)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_RMSK                                                               0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDRMSBVAL_BMSK                                                    0x3
#define HWIO_MEMNOC_TCU_DYN_ATTR_ADDRMSBVAL_LOW_ADDRMSBVAL_SHFT                                                    0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_ADDR                                                             (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f048)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RMSK                                                                0xfffff
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_CTLVAL_BMSK                                                         0xf0000
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_CTLVAL_SHFT                                                            0x10
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RSV1_BMSK                                                            0xf000
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RSV1_SHFT                                                               0xc
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_WRVAL_BMSK                                                            0xf00
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_WRVAL_SHFT                                                              0x8
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RSV0_BMSK                                                              0xf0
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RSV0_SHFT                                                               0x4
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RDVAL_BMSK                                                              0xf
#define HWIO_MEMNOC_TCU_DYN_ATTR_ALLOC_LOW_RDVAL_SHFT                                                              0x0

#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_ADDR                                                     (MEMNOC_MEM_NOC_REG_BASE      + 0x0001f050)
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_RMSK                                                           0x1f
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_IN          \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_ADDR, HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_RMSK)
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_INM(m)      \
        in_dword_masked(HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_ADDR, m)
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_OUT(v)      \
        out_dword(HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_ADDR,v)
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_ADDR,m,v,HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_IN)
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_CACHEINDEXVAL_BMSK                                             0x1f
#define HWIO_MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW_CACHEINDEXVAL_SHFT                                              0x0

#endif /* __ICBCFG_HWIO_TARGET_V1_H__ */
