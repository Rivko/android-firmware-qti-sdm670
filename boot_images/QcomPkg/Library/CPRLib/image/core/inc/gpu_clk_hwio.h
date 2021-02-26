#ifndef __GPU_CLK_HWIO_H__
#define __GPU_CLK_HWIO_H__
/*
===========================================================================
*/
/**
  @file gpu_clk_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r105_MTO]
 
  This file contains HWIO register definitions for the following modules:
    GPUCC_GPU_CC_GPU_CC_GPU_CC_REG

  'Include' filters applied: GPUCC_GPU_CC_GX_BCR[GPUCC_GPU_CC_GPU_CC_GPU_CC_REG] GPUCC_GPU_CC_GX_DOMAIN_MISC[GPUCC_GPU_CC_GPU_CC_GPU_CC_REG] 

  Generation parameters: 
  { u'filename': u'gpu_clk_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'GPUCC_GPU_CC_GPU_CC_GPU_CC_REG': [ u'GPUCC_GPU_CC_GX_BCR',
                                                                     u'GPUCC_GPU_CC_GX_DOMAIN_MISC']},
    u'modules': [u'GPUCC_GPU_CC_GPU_CC_GPU_CC_REG']}
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/core/inc/gpu_clk_hwio.h#1 $
  $DateTime: 2017/12/19 03:28:31 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: GPUCC_GPU_CC_GPU_CC_GPU_CC_REG
 *--------------------------------------------------------------------------*/

#define GPUCC_GPU_CC_GPU_CC_GPU_CC_REG_REG_BASE                                             (GFX_BASE      + 0x00090000)

#define HWIO_GPUCC_GPU_CC_GX_BCR_ADDR                                                       (GPUCC_GPU_CC_GPU_CC_GPU_CC_REG_REG_BASE      + 0x00001008)
#define HWIO_GPUCC_GPU_CC_GX_BCR_RMSK                                                              0x1
#define HWIO_GPUCC_GPU_CC_GX_BCR_IN          \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_BCR_ADDR, HWIO_GPUCC_GPU_CC_GX_BCR_RMSK)
#define HWIO_GPUCC_GPU_CC_GX_BCR_INM(m)      \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_BCR_ADDR, m)
#define HWIO_GPUCC_GPU_CC_GX_BCR_OUT(v)      \
        out_dword(HWIO_GPUCC_GPU_CC_GX_BCR_ADDR,v)
#define HWIO_GPUCC_GPU_CC_GX_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GPUCC_GPU_CC_GX_BCR_ADDR,m,v,HWIO_GPUCC_GPU_CC_GX_BCR_IN)
#define HWIO_GPUCC_GPU_CC_GX_BCR_BLK_ARES_BMSK                                                     0x1
#define HWIO_GPUCC_GPU_CC_GX_BCR_BLK_ARES_SHFT                                                     0x0

#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_ADDR                                               (GPUCC_GPU_CC_GPU_CC_GPU_CC_REG_REG_BASE      + 0x00001508)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RMSK                                                 0x3c0011
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_IN          \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_ADDR, HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RMSK)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_INM(m)      \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_ADDR, m)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_OUT(v)      \
        out_dword(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_ADDR,v)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_ADDR,m,v,HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_IN)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RAC_ENR_STATUS_BMSK                                  0x200000
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RAC_ENR_STATUS_SHFT                                      0x15
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RAC_ENF_STATUS_BMSK                                  0x100000
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_RAC_ENF_STATUS_SHFT                                      0x14
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_SPTP_ENR_STATUS_BMSK                                  0x80000
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_SPTP_ENR_STATUS_SHFT                                     0x13
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_SPTP_ENF_STATUS_BMSK                                  0x40000
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_SPTP_ENF_STATUS_SHFT                                     0x12
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_GPU_GX_GMEM_RESET_BMSK                                   0x10
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_GPU_GX_GMEM_RESET_SHFT                                    0x4
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_GPU_GX_GMEM_CLAMP_IO_BMSK                                 0x1
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC_GPU_GX_GMEM_CLAMP_IO_SHFT                                 0x0

#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_ADDR                                              (GPUCC_GPU_CC_GPU_CC_GPU_CC_REG_REG_BASE      + 0x0000150c)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_RMSK                                                     0x1
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_IN          \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_ADDR, HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_RMSK)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_INM(m)      \
        in_dword_masked(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_ADDR, m)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_OUT(v)      \
        out_dword(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_ADDR,v)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_ADDR,m,v,HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_IN)
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_GFX_CLAMP_MEM_BMSK                                       0x1
#define HWIO_GPUCC_GPU_CC_GX_DOMAIN_MISC2_GFX_CLAMP_MEM_SHFT                                       0x0


#endif /* __GPU_CLK_HWIO_H__ */