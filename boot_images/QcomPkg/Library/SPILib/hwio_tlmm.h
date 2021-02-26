#ifndef __TLMMTILE_H__
#define __TLMMTILE_H__
/*
===========================================================================
*/
/**
  @file tlmmtile.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8998 (Nazgul) [nazgul_v1.0_p3q3r33]
 
  This file contains HWIO register definitions for the following modules:
    TLMM_EAST

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016-2020 Qualcomm Technologies Incorporated.
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


  ===========================================================================
*/

#include "msmhwiobase.h"
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: TLMM_NORTH, TLMM_SOUTH
 *--------------------------------------------------------------------------*/

#define TLMM_NORTH_REG_BASE                                                (TLMM_BASE      + 0x00500000)
#define TLMM_NORTH_REG_BASE_OFFS                                           0x00500000
#define TLMM_SOUTH_REG_BASE                                                (TLMM_BASE      + 0x00900000)
#define TLMM_SOUTH_REG_BASE_OFFS                                           0x00900000

#define HWIO_TLMM_GPIO_CFG_ADDR(x,n)                                      ((x) + ( n * 0x1000 ) + 0x00000000)
#define HWIO_TLMM_GPIO_CFG_OFFS                                         (0x00000000)
#define HWIO_TLMM_GPIO_CFG_RMSK                                              0x7ff
#define HWIO_TLMM_GPIO_CFG_IN(x, n)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG_ADDR(x,n), HWIO_TLMM_GPIO_CFG_RMSK)

#define HWIO_TLMM_GPIO_CFG_INM(x, n, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG_ADDR(x,n), m)
		
#define HWIO_TLMM_GPIO_CFG_OUT(x, n, v)      \
        out_dword(HWIO_TLMM_GPIO_CFG_ADDR(x,n),v)
		
#define HWIO_TLMM_GPIO_CFG_OUTM(x,n,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG_ADDR(x,n),m,v,HWIO_TLMM_GPIO_CFG_IN(x, n))

#define HWIO_TLMM_GPIO_CFG_GPIO_HIHYS_EN_BMSK                                0x400
#define HWIO_TLMM_GPIO_CFG_GPIO_HIHYS_EN_SHFT                                  0xa
#define HWIO_TLMM_GPIO_CFG_GPIO_OE_BMSK                                      0x200
#define HWIO_TLMM_GPIO_CFG_GPIO_OE_SHFT                                        0x9
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_BMSK                                 0x1c0
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_SHFT                                   0x6
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_2_MA_FVAL                          0x0
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_4_MA_FVAL                          0x1
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_6_MA_FVAL                          0x2
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_8_MA_FVAL                          0x3
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_10_MA_FVAL                         0x4
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_12_MA_FVAL                         0x5
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_14_MA_FVAL                         0x6
#define HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_DRV_16_MA_FVAL                         0x7
#define HWIO_TLMM_GPIO_CFG_FUNC_SEL_BMSK                                      0x3c
#define HWIO_TLMM_GPIO_CFG_FUNC_SEL_SHFT                                       0x2
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_BMSK                                      0x3
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_SHFT                                      0x0
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_NO_PULL_FVAL                              0x0
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_PULL_DOWN_FVAL                            0x1
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_KEEPER_FVAL                               0x2
#define HWIO_TLMM_GPIO_CFG_GPIO_PULL_PULL_UP_FVAL                              0x3

#define HWIO_TLMM_GPIO_IN_OUT_ADDR(x,n)                                   ((x) + ( 0x1000 * n ) + 0x00000004)
#define HWIO_TLMM_GPIO_IN_OUT_OFFS                                      (0x00000004)
#define HWIO_TLMM_GPIO_IN_OUT_RMSK                                             0x3
#define HWIO_TLMM_GPIO_IN_OUT_IN(x,n)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT_ADDR(x,n), HWIO_TLMM_GPIO_IN_OUT_RMSK)
#define HWIO_TLMM_GPIO_IN_OUT_INM(x,n, m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT_ADDR(x,n), m)


#define HWIO_TLMM_GPIO_IN_OUT_OUT(x, n, v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT_ADDR(x,n),v)


#define HWIO_TLMM_GPIO_IN_OUT_OUTM(x,n,m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT_ADDR(x,n),m,v,HWIO_TLMM_GPIO_IN_OUT_IN(x,n))
#define HWIO_TLMM_GPIO_IN_OUT_GPIO_OUT_BMSK                                    0x2
#define HWIO_TLMM_GPIO_IN_OUT_GPIO_OUT_SHFT                                    0x1
#define HWIO_TLMM_GPIO_IN_OUT_GPIO_IN_BMSK                                     0x1
#define HWIO_TLMM_GPIO_IN_OUT_GPIO_IN_SHFT                                     0x0


#endif /* __TLMMTILE_H__ */
