#ifndef __MPM_CTRL_HWIO_H__
#define __MPM_CTRL_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpm_ctrl_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r81]
 
  This file contains HWIO register definitions for the following modules:
    G_CTRL_CNTR

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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/target/670/mpm_ctrl_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "msmhwiobase.h"
/*----------------------------------------------------------------------------
 * MODULE: G_CTRL_CNTR
 *--------------------------------------------------------------------------*/

#define G_CTRL_CNTR_REG_BASE                             (AOSS_BASE      + 0x01210000)
#define G_CTRL_CNTR_REG_BASE_PHYS                        (AOSS_BASE_PHYS + 0x01210000)
#define G_CTRL_CNTR_REG_BASE_OFFS                        0x01210000

#define HWIO_MPM_CONTROL_CNTCR_ADDR                      (G_CTRL_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM_CONTROL_CNTCR_PHYS                      (G_CTRL_CNTR_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM_CONTROL_CNTCR_OFFS                      (G_CTRL_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM_CONTROL_CNTCR_RMSK                           0x103
#define HWIO_MPM_CONTROL_CNTCR_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCR_ADDR, HWIO_MPM_CONTROL_CNTCR_RMSK)
#define HWIO_MPM_CONTROL_CNTCR_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCR_ADDR, m)
#define HWIO_MPM_CONTROL_CNTCR_OUT(v)      \
        out_dword(HWIO_MPM_CONTROL_CNTCR_ADDR,v)
#define HWIO_MPM_CONTROL_CNTCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CONTROL_CNTCR_ADDR,m,v,HWIO_MPM_CONTROL_CNTCR_IN)
#define HWIO_MPM_CONTROL_CNTCR_FCREQ_BMSK                     0x100
#define HWIO_MPM_CONTROL_CNTCR_FCREQ_SHFT                       0x8
#define HWIO_MPM_CONTROL_CNTCR_HDBG_BMSK                        0x2
#define HWIO_MPM_CONTROL_CNTCR_HDBG_SHFT                        0x1
#define HWIO_MPM_CONTROL_CNTCR_EN_BMSK                          0x1
#define HWIO_MPM_CONTROL_CNTCR_EN_SHFT                          0x0

#define HWIO_MPM_CONTROL_CNTSR_ADDR                      (G_CTRL_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM_CONTROL_CNTSR_PHYS                      (G_CTRL_CNTR_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM_CONTROL_CNTSR_OFFS                      (G_CTRL_CNTR_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM_CONTROL_CNTSR_RMSK                           0x102
#define HWIO_MPM_CONTROL_CNTSR_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_CNTSR_ADDR, HWIO_MPM_CONTROL_CNTSR_RMSK)
#define HWIO_MPM_CONTROL_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_CNTSR_ADDR, m)
#define HWIO_MPM_CONTROL_CNTSR_FCACK_BMSK                     0x100
#define HWIO_MPM_CONTROL_CNTSR_FCACK_SHFT                       0x8
#define HWIO_MPM_CONTROL_CNTSR_HDBG_BMSK                        0x2
#define HWIO_MPM_CONTROL_CNTSR_HDBG_SHFT                        0x1

#define HWIO_MPM_CONTROL_CNTCV_LO_ADDR                   (G_CTRL_CNTR_REG_BASE      + 0x00000008)
#define HWIO_MPM_CONTROL_CNTCV_LO_PHYS                   (G_CTRL_CNTR_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM_CONTROL_CNTCV_LO_OFFS                   (G_CTRL_CNTR_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM_CONTROL_CNTCV_LO_RMSK                   0xffffffff
#define HWIO_MPM_CONTROL_CNTCV_LO_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCV_LO_ADDR, HWIO_MPM_CONTROL_CNTCV_LO_RMSK)
#define HWIO_MPM_CONTROL_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCV_LO_ADDR, m)
#define HWIO_MPM_CONTROL_CNTCV_LO_OUT(v)      \
        out_dword(HWIO_MPM_CONTROL_CNTCV_LO_ADDR,v)
#define HWIO_MPM_CONTROL_CNTCV_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CONTROL_CNTCV_LO_ADDR,m,v,HWIO_MPM_CONTROL_CNTCV_LO_IN)
#define HWIO_MPM_CONTROL_CNTCV_LO_LOAD_VAL_BMSK          0xffffffff
#define HWIO_MPM_CONTROL_CNTCV_LO_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM_CONTROL_CNTCV_HI_ADDR                   (G_CTRL_CNTR_REG_BASE      + 0x0000000c)
#define HWIO_MPM_CONTROL_CNTCV_HI_PHYS                   (G_CTRL_CNTR_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM_CONTROL_CNTCV_HI_OFFS                   (G_CTRL_CNTR_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM_CONTROL_CNTCV_HI_RMSK                     0xffffff
#define HWIO_MPM_CONTROL_CNTCV_HI_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCV_HI_ADDR, HWIO_MPM_CONTROL_CNTCV_HI_RMSK)
#define HWIO_MPM_CONTROL_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_CNTCV_HI_ADDR, m)
#define HWIO_MPM_CONTROL_CNTCV_HI_OUT(v)      \
        out_dword(HWIO_MPM_CONTROL_CNTCV_HI_ADDR,v)
#define HWIO_MPM_CONTROL_CNTCV_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CONTROL_CNTCV_HI_ADDR,m,v,HWIO_MPM_CONTROL_CNTCV_HI_IN)
#define HWIO_MPM_CONTROL_CNTCV_HI_LOAD_VAL_BMSK            0xffffff
#define HWIO_MPM_CONTROL_CNTCV_HI_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM_CONTROL_CNTFID0_ADDR                    (G_CTRL_CNTR_REG_BASE      + 0x00000020)
#define HWIO_MPM_CONTROL_CNTFID0_PHYS                    (G_CTRL_CNTR_REG_BASE_PHYS + 0x00000020)
#define HWIO_MPM_CONTROL_CNTFID0_OFFS                    (G_CTRL_CNTR_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM_CONTROL_CNTFID0_RMSK                    0xffffffff
#define HWIO_MPM_CONTROL_CNTFID0_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_CNTFID0_ADDR, HWIO_MPM_CONTROL_CNTFID0_RMSK)
#define HWIO_MPM_CONTROL_CNTFID0_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_CNTFID0_ADDR, m)
#define HWIO_MPM_CONTROL_CNTFID0_OUT(v)      \
        out_dword(HWIO_MPM_CONTROL_CNTFID0_ADDR,v)
#define HWIO_MPM_CONTROL_CNTFID0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CONTROL_CNTFID0_ADDR,m,v,HWIO_MPM_CONTROL_CNTFID0_IN)
#define HWIO_MPM_CONTROL_CNTFID0_FREQ_BMSK               0xffffffff
#define HWIO_MPM_CONTROL_CNTFID0_FREQ_SHFT                      0x0

#define HWIO_MPM_CONTROL_ID_ADDR                         (G_CTRL_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM_CONTROL_ID_PHYS                         (G_CTRL_CNTR_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_MPM_CONTROL_ID_OFFS                         (G_CTRL_CNTR_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_MPM_CONTROL_ID_RMSK                         0xffffffff
#define HWIO_MPM_CONTROL_ID_IN          \
        in_dword_masked(HWIO_MPM_CONTROL_ID_ADDR, HWIO_MPM_CONTROL_ID_RMSK)
#define HWIO_MPM_CONTROL_ID_INM(m)      \
        in_dword_masked(HWIO_MPM_CONTROL_ID_ADDR, m)
#define HWIO_MPM_CONTROL_ID_MAJOR_BMSK                   0xf0000000
#define HWIO_MPM_CONTROL_ID_MAJOR_SHFT                         0x1c
#define HWIO_MPM_CONTROL_ID_MINOR_BMSK                    0xfff0000
#define HWIO_MPM_CONTROL_ID_MINOR_SHFT                         0x10
#define HWIO_MPM_CONTROL_ID_STEP_BMSK                        0xffff
#define HWIO_MPM_CONTROL_ID_STEP_SHFT                           0x0


#endif /* __MPM_CTRL_HWIO_H__ */
