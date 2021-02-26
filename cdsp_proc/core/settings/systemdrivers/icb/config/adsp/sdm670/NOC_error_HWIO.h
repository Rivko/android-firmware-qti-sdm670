#ifndef __NOC_ERROR_HWIO_H__
#define __NOC_ERROR_HWIO_H__
/*
===========================================================================
*/
/**
  @file NOC_error_HWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r93]
 
  This file contains HWIO register definitions for the following modules:
    SSC_NOC
    SSC_SCC_SCC_REG

  'Include' filters applied: SCC_QTIMER_TMR[SSC_SCC_SCC_REG] OBS_SWID_LOW[SSC_NOC] SBM_SWID_LOW[SSC_NOC] 
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

  $Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/icb/config/adsp/sdm670/NOC_error_HWIO.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_NOC
 *--------------------------------------------------------------------------*/

#define SSC_NOC_REG_BASE                                                                      (LPASS_BASE      + 0x00be0000)

#define HWIO_SSC_NOC_OBS_SWID_LOW_ADDR                                                        (SSC_NOC_REG_BASE      + 0x00000000)
#define HWIO_SSC_NOC_OBS_SWID_LOW_RMSK                                                          0xffffff
#define HWIO_SSC_NOC_OBS_SWID_LOW_IN          \
        in_dword_masked(HWIO_SSC_NOC_OBS_SWID_LOW_ADDR, HWIO_SSC_NOC_OBS_SWID_LOW_RMSK)
#define HWIO_SSC_NOC_OBS_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_SSC_NOC_OBS_SWID_LOW_ADDR, m)
#define HWIO_SSC_NOC_OBS_SWID_LOW_UNITTYPEID_BMSK                                               0xff0000
#define HWIO_SSC_NOC_OBS_SWID_LOW_UNITTYPEID_SHFT                                                   0x10
#define HWIO_SSC_NOC_OBS_SWID_LOW_UNITCONFID_BMSK                                                 0xffff
#define HWIO_SSC_NOC_OBS_SWID_LOW_UNITCONFID_SHFT                                                    0x0

#define HWIO_SSC_NOC_SBM_SWID_LOW_ADDR                                                        (SSC_NOC_REG_BASE      + 0x00002000)
#define HWIO_SSC_NOC_SBM_SWID_LOW_RMSK                                                          0xffffff
#define HWIO_SSC_NOC_SBM_SWID_LOW_IN          \
        in_dword_masked(HWIO_SSC_NOC_SBM_SWID_LOW_ADDR, HWIO_SSC_NOC_SBM_SWID_LOW_RMSK)
#define HWIO_SSC_NOC_SBM_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_SSC_NOC_SBM_SWID_LOW_ADDR, m)
#define HWIO_SSC_NOC_SBM_SWID_LOW_UNITTYPEID_BMSK                                               0xff0000
#define HWIO_SSC_NOC_SBM_SWID_LOW_UNITTYPEID_SHFT                                                   0x10
#define HWIO_SSC_NOC_SBM_SWID_LOW_UNITCONFID_BMSK                                                 0xffff
#define HWIO_SSC_NOC_SBM_SWID_LOW_UNITCONFID_SHFT                                                    0x0

#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_ADDR                                                 (SSC_NOC_REG_BASE      + 0x00002088)
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_RMSK                                                    0x7ffff
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_OUT(v)      \
        out_dword(HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_ADDR,v)
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT18_BMSK                                             0x40000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT18_SHFT                                                0x12
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT17_BMSK                                             0x20000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT17_SHFT                                                0x11
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT16_BMSK                                             0x10000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT16_SHFT                                                0x10
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT15_BMSK                                              0x8000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT15_SHFT                                                 0xf
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT14_BMSK                                              0x4000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT14_SHFT                                                 0xe
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT13_BMSK                                              0x2000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT13_SHFT                                                 0xd
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT12_BMSK                                              0x1000
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT12_SHFT                                                 0xc
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT11_BMSK                                               0x800
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT11_SHFT                                                 0xb
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT10_BMSK                                               0x400
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT10_SHFT                                                 0xa
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT9_BMSK                                                0x200
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT9_SHFT                                                  0x9
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT8_BMSK                                                0x100
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT8_SHFT                                                  0x8
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT7_BMSK                                                 0x80
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT7_SHFT                                                  0x7
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT6_BMSK                                                 0x40
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT6_SHFT                                                  0x6
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT5_BMSK                                                 0x20
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT5_SHFT                                                  0x5
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT4_BMSK                                                 0x10
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT4_SHFT                                                  0x4
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT3_BMSK                                                  0x8
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT3_SHFT                                                  0x3
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT2_BMSK                                                  0x4
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT2_SHFT                                                  0x2
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT1_BMSK                                                  0x2
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT1_SHFT                                                  0x1
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT0_BMSK                                                  0x1
#define HWIO_SSC_NOC_SBM_FLAGOUTSET0_LOW_PORT0_SHFT                                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_SCC_SCC_REG
 *--------------------------------------------------------------------------*/

#define SSC_SCC_SCC_REG_REG_BASE                                                              (LPASS_BASE      + 0x00b10000)

#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR                                           (SSC_SCC_SCC_REG_REG_BASE      + 0x00009060)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_RMSK                                                0x1ff
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR, HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_CLK_DIV_BMSK                                        0x1ff
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_CLK_DIV_SHFT                                          0x0

#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR                                                     (SSC_SCC_SCC_REG_REG_BASE      + 0x00009064)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_RMSK                                                     0x80000005
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR, HWIO_SSC_SCC_QTIMER_TMR_CBCR_RMSK)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_TMR_CBCR_IN)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ARES_BMSK                                                   0x4
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ARES_SHFT                                                   0x2
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_SHFT                                                 0x0


#endif /* __NOC_ERROR_HWIO_H__ */
