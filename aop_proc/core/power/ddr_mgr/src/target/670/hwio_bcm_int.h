#ifndef __HWIO_BCM_INT_H__
#define __HWIO_BCM_INT_H__
/*
===========================================================================
*/
/**
  @file hwio_bcm_int.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r44.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_BCM_BCM_INTERRUPT

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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_bcm_int.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: BCM_INTERRUPT
 *--------------------------------------------------------------------------*/

#define BCM_INTERRUPT_REG_BASE                                                 (AOSS_BASE      + 0x00bf1f00)
#define BCM_INTERRUPT_REG_BASE_OFFS                                            0x00bf1f00

#define HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR                                    (BCM_INTERRUPT_REG_BASE      + 0x00000000)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_OFFS                                    (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000000)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_RMSK                                        0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR, HWIO_INTERRUPT_AGG_DONE_INT_EN_RMSK)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_AGG_DONE_INT_EN_ADDR,m,v,HWIO_INTERRUPT_AGG_DONE_INT_EN_IN)
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_EN_BMSK                                     0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_EN_EN_SHFT                                        0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR                              (BCM_INTERRUPT_REG_BASE      + 0x00000004)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_OFFS                              (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000004)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_RMSK                                  0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR, HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_RMSK)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_ADDR,m,v,HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_IN)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_EN_BMSK                               0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_EN_EN_SHFT                                  0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR                                    (BCM_INTERRUPT_REG_BASE      + 0x00000008)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_OFFS                                    (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000008)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_RMSK                                        0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR, HWIO_INTERRUPT_CTT_FULL_INT_EN_RMSK)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_CTT_FULL_INT_EN_ADDR,m,v,HWIO_INTERRUPT_CTT_FULL_INT_EN_IN)
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_EN_BMSK                                     0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_EN_EN_SHFT                                        0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR                               (BCM_INTERRUPT_REG_BASE      + 0x0000000c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_OFFS                               (BCM_INTERRUPT_REG_BASE_OFFS + 0x0000000c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_RMSK                                   0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_ADDR,m,v,HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_IN)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_EN_BMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_EN_EN_SHFT                                   0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR                               (BCM_INTERRUPT_REG_BASE      + 0x00000010)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_OFFS                               (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000010)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_RMSK                                   0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_ADDR,m,v,HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_IN)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_EN_BMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_EN_EN_SHFT                                   0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000014)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_OFFS                                (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000014)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_RMSK                                    0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_IN          \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR, HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_RMSK)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR, m)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_OUT(v)      \
        out_dword(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR,v)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_ADDR,m,v,HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_IN)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_EN_BMSK                                 0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_EN_EN_SHFT                                    0x0

#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000040)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_OFFS                                (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000040)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_RMSK                                    0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR, HWIO_INTERRUPT_AGG_DONE_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_AGG_DONE_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_STATUS_BMSK                             0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_STATUS_STATUS_SHFT                                0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR                          (BCM_INTERRUPT_REG_BASE      + 0x00000044)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_OFFS                          (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000044)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_RMSK                              0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR, HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_STATUS_BMSK                       0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_STATUS_STATUS_SHFT                          0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR                                (BCM_INTERRUPT_REG_BASE      + 0x00000048)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_OFFS                                (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000048)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_RMSK                                    0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR, HWIO_INTERRUPT_CTT_FULL_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_CTT_FULL_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_STATUS_BMSK                             0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_STATUS_STATUS_SHFT                                0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x0000004c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_OFFS                           (BCM_INTERRUPT_REG_BASE_OFFS + 0x0000004c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_RMSK                               0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_STATUS_BMSK                        0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_STATUS_STATUS_SHFT                           0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x00000050)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_OFFS                           (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000050)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_RMSK                               0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR, HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_RMSK)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_ADDR, m)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_STATUS_BMSK                        0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_STATUS_STATUS_SHFT                           0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x00000054)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_OFFS                            (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000054)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_RMSK                                0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_IN          \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR, HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_RMSK)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_INM(m)      \
        in_dword_masked(HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_ADDR, m)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_STATUS_BMSK                         0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_STATUS_STATUS_SHFT                            0x0

#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_ADDR                                 (BCM_INTERRUPT_REG_BASE      + 0x00000080)
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_OFFS                                 (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000080)
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_RMSK                                     0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_CLEAR_BMSK                               0xffff
#define HWIO_INTERRUPT_AGG_DONE_INT_CLEAR_CLEAR_SHFT                                  0x0

#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_ADDR                           (BCM_INTERRUPT_REG_BASE      + 0x00000084)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_OFFS                           (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000084)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_RMSK                               0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_CLEAR_BMSK                         0xffff
#define HWIO_INTERRUPT_CP_CHANGE_DONE_INT_CLEAR_CLEAR_SHFT                            0x0

#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_ADDR                                 (BCM_INTERRUPT_REG_BASE      + 0x00000088)
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_OFFS                                 (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000088)
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_RMSK                                     0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_CLEAR_BMSK                               0x1fff
#define HWIO_INTERRUPT_CTT_FULL_INT_CLEAR_CLEAR_SHFT                                  0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x0000008c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_OFFS                            (BCM_INTERRUPT_REG_BASE_OFFS + 0x0000008c)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_RMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_CLEAR_BMSK                          0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_0_CLEAR_CLEAR_SHFT                             0x0

#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_ADDR                            (BCM_INTERRUPT_REG_BASE      + 0x00000090)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_OFFS                            (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000090)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_RMSK                                0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_CLEAR_BMSK                          0xffff
#define HWIO_INTERRUPT_SEQ_GENERIC_INT_1_CLEAR_CLEAR_SHFT                             0x0

#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_ADDR                             (BCM_INTERRUPT_REG_BASE      + 0x00000094)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_OFFS                             (BCM_INTERRUPT_REG_BASE_OFFS + 0x00000094)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_RMSK                                 0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_OUT(v)      \
        out_dword(HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_ADDR,v)
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_CLEAR_BMSK                           0xffff
#define HWIO_INTERRUPT_STOP_AGG_ACK_INT_CLEAR_CLEAR_SHFT                              0x0


#endif /* __HWIO_BCM_INT_H__ */
