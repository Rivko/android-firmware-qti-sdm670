#ifndef __SNS_HWIO_SDC_H__
#define __SNS_HWIO_SDC_H__
/*
===========================================================================
*/
/**
  @file sns_hwio_sdc.h
  @brief HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.8_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    SSC_SCC_SCC_SCC_REG

  'Include' filters applied: Necessary registers for SDC reset
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

  $Header: //components/rel/ssc.slpi/3.2/inc/internal/sns_hwio_sdc.h#2 $
  $DateTime: 2018/03/27 11:46:46 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_SCC_SCC_SCC_REG
 *--------------------------------------------------------------------------*/

#define SSC_SCC_SCC_SCC_REG_REG_BASE                                               (SSC_BASE      + 0x00b10000)
#define SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS                                          0x00b10000

#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f008)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f008)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_RMSK                                       0x80000005
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f00c)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f00c)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_RMSK                                       0x80000007
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_HW_CTL_BMSK                                       0x2
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_HW_CTL_SHFT                                       0x1
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f010)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f010)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_RMSK                                        0x80007ff7
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_CORE_ON_BMSK                          0x4000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_CORE_ON_SHFT                             0xe
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_ON_BMSK                        0x2000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_ON_SHFT                           0xd
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                       0x1000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                          0xc
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_WAKEUP_BMSK                                      0xf00
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_WAKEUP_SHFT                                        0x8
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_SLEEP_BMSK                                        0xf0
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_SLEEP_SHFT                                         0x4
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_HW_CTL_BMSK                                        0x2
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_HW_CTL_SHFT                                        0x1
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_BUS_BCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009000)
#define HWIO_SSC_SCC_BUS_BCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009000)
#define HWIO_SSC_SCC_BUS_BCR_RMSK                                                         0x1
#define HWIO_SSC_SCC_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_BUS_BCR_ADDR, HWIO_SSC_SCC_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_BUS_BCR_IN)
#define HWIO_SSC_SCC_BUS_BCR_BLK_ARES_BMSK                                                0x1
#define HWIO_SSC_SCC_BUS_BCR_BLK_ARES_SHFT                                                0x0

#define HWIO_SSC_SCC_SDC_BCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f000)
#define HWIO_SSC_SCC_SDC_BCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f000)
#define HWIO_SSC_SCC_SDC_BCR_RMSK                                                         0x1
#define HWIO_SSC_SCC_SDC_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_BCR_ADDR, HWIO_SSC_SCC_SDC_BCR_RMSK)
#define HWIO_SSC_SCC_SDC_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_BCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_BCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_BCR_ADDR,m,v,HWIO_SSC_SCC_SDC_BCR_IN)
#define HWIO_SSC_SCC_SDC_BCR_BLK_ARES_BMSK                                                0x1
#define HWIO_SSC_SCC_SDC_BCR_BLK_ARES_SHFT                                                0x0

#define HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a060)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a060)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_SDC_DAP_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR, HWIO_SSC_SCC_SDC_DAP_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_DAP_CBCR_IN)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ENABLE_SHFT                                         0x0


#endif /* __SNS_HWIO_SDC_H__ */
