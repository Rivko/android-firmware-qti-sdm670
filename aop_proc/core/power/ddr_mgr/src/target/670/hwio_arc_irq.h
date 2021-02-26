#ifndef __HWIO_ARC_IRQ__
#define __HWIO_ARC_IRQ__
/*
===========================================================================
*/
/**
  @file hwio_arc_irq
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r32.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_ARC_ARC_IRQ

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_arc_irq.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: ARC_IRQ
 *--------------------------------------------------------------------------*/

#define ARC_IRQ_REG_BASE                                         (AOSS_BASE      + 0x007ed800)
#define ARC_IRQ_REG_BASE_OFFS                                    0x007ed800

#define HWIO_RMm_IRQ_EN_ADDR(m)                                  (ARC_IRQ_REG_BASE      + 0x0000000c + 0x4 * (m))
#define HWIO_RMm_IRQ_EN_OFFS(m)                                  (ARC_IRQ_REG_BASE_OFFS + 0x0000000c + 0x4 * (m))
#define HWIO_RMm_IRQ_EN_RMSK                                          0x1ff
#define HWIO_RMm_IRQ_EN_MAXm                                              8
#define HWIO_RMm_IRQ_EN_INI(m)        \
        in_dword_masked(HWIO_RMm_IRQ_EN_ADDR(m), HWIO_RMm_IRQ_EN_RMSK)
#define HWIO_RMm_IRQ_EN_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_IRQ_EN_ADDR(m), mask)
#define HWIO_RMm_IRQ_EN_OUTI(m,val)    \
        out_dword(HWIO_RMm_IRQ_EN_ADDR(m),val)
#define HWIO_RMm_IRQ_EN_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RMm_IRQ_EN_ADDR(m),mask,val,HWIO_RMm_IRQ_EN_INI(m))
#define HWIO_RMm_IRQ_EN_RM_IRQ_EN_BMSK                                0x1ff
#define HWIO_RMm_IRQ_EN_RM_IRQ_EN_SHFT                                  0x0

#define HWIO_RMm_IRQ_STATUS_ADDR(m)                              (ARC_IRQ_REG_BASE      + 0x00000100 + 0x4 * (m))
#define HWIO_RMm_IRQ_STATUS_OFFS(m)                              (ARC_IRQ_REG_BASE_OFFS + 0x00000100 + 0x4 * (m))
#define HWIO_RMm_IRQ_STATUS_RMSK                                      0x1ff
#define HWIO_RMm_IRQ_STATUS_MAXm                                          8
#define HWIO_RMm_IRQ_STATUS_INI(m)        \
        in_dword_masked(HWIO_RMm_IRQ_STATUS_ADDR(m), HWIO_RMm_IRQ_STATUS_RMSK)
#define HWIO_RMm_IRQ_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_RMm_IRQ_STATUS_ADDR(m), mask)
#define HWIO_RMm_IRQ_STATUS_RM_IRQ_STATUS_BMSK                        0x1ff
#define HWIO_RMm_IRQ_STATUS_RM_IRQ_STATUS_SHFT                          0x0

#define HWIO_RMm_IRQ_CLR_ADDR(m)                                 (ARC_IRQ_REG_BASE      + 0x00000200 + 0x4 * (m))
#define HWIO_RMm_IRQ_CLR_OFFS(m)                                 (ARC_IRQ_REG_BASE_OFFS + 0x00000200 + 0x4 * (m))
#define HWIO_RMm_IRQ_CLR_RMSK                                         0x1ff
#define HWIO_RMm_IRQ_CLR_MAXm                                             8
#define HWIO_RMm_IRQ_CLR_OUTI(m,val)    \
        out_dword(HWIO_RMm_IRQ_CLR_ADDR(m),val)
#define HWIO_RMm_IRQ_CLR_RM_IRQ_CLR_BMSK                              0x1ff
#define HWIO_RMm_IRQ_CLR_RM_IRQ_CLR_SHFT                                0x0


#endif /* __HWIO_ARC_IRQ__ */
