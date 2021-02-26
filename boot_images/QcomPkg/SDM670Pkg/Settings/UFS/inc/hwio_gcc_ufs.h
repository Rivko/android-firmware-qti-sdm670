#ifndef __HWIO_GCC_UFS_H__
#define __HWIO_GCC_UFS_H__
/*
===========================================================================
*/
/**
  @file hwio_gcc_ufs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.3_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG

  'Include' filters applied: 
  'Exclude' filters applied: 
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/UFS/inc/hwio_gcc_ufs.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                   (CLK_CTL_BASE      + 0x00000000)

#define HWIO_GCC_UFS_PHY_BCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00077000)
#define HWIO_GCC_UFS_PHY_BCR_RMSK                                                                         0x1
#define HWIO_GCC_UFS_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_UFS_PHY_BCR_ADDR, HWIO_GCC_UFS_PHY_BCR_RMSK)
#define HWIO_GCC_UFS_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_UFS_PHY_BCR_ADDR, m)
#define HWIO_GCC_UFS_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_UFS_PHY_BCR_ADDR,v)
#define HWIO_GCC_UFS_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_UFS_PHY_BCR_ADDR,m,v,HWIO_GCC_UFS_PHY_BCR_IN)
#define HWIO_GCC_UFS_PHY_BCR_BLK_ARES_BMSK                                                                0x1
#define HWIO_GCC_UFS_PHY_BCR_BLK_ARES_SHFT                                                                0x0

#endif /* __HWIO_GCC_UFS_H__ */
