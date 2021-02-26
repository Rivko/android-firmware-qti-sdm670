#ifndef __HWIO_ARC_ARC_VT_H__
#define __HWIO_ARC_ARC_VT_H__
/*
===========================================================================
*/
/**
  @file hwio_arc_arc_vt.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r32.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_ARC_ARC_VT

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_arc_vt.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: ARC_VT
 *--------------------------------------------------------------------------*/

#define ARC_VT_REG_BASE                                                     (AOSS_BASE      + 0x00600000)

#define HWIO_DRVd_RMm_VOTE_ADDR(d,m)                                        (ARC_VT_REG_BASE      + 0x00000000 + 0x10000 * (d) + 0x10 * (m))
#define HWIO_DRVd_RMm_VOTE_RMSK                                             0x8000000f
#define HWIO_DRVd_RMm_VOTE_MAXd                                                     29
#define HWIO_DRVd_RMm_VOTE_MAXm                                                      8
#define HWIO_DRVd_RMm_VOTE_ATTR                                                    0x3
#define HWIO_DRVd_RMm_VOTE_INI2(d,m)        \
        in_dword_masked(HWIO_DRVd_RMm_VOTE_ADDR(d,m), HWIO_DRVd_RMm_VOTE_RMSK)
#define HWIO_DRVd_RMm_VOTE_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_DRVd_RMm_VOTE_ADDR(d,m), mask)
#define HWIO_DRVd_RMm_VOTE_OUTI2(d,m,val)    \
        out_dword(HWIO_DRVd_RMm_VOTE_ADDR(d,m),val)
#define HWIO_DRVd_RMm_VOTE_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_DRVd_RMm_VOTE_ADDR(d,m),mask,val,HWIO_DRVd_RMm_VOTE_INI2(d,m))
#define HWIO_DRVd_RMm_VOTE_RR_BMSK                                          0x80000000
#define HWIO_DRVd_RMm_VOTE_RR_SHFT                                                0x1f
#define HWIO_DRVd_RMm_VOTE_OL_BMSK                                                 0xf
#define HWIO_DRVd_RMm_VOTE_OL_SHFT                                                 0x0

#define HWIO_DRVd_RMm_TIMER_ADDR(d,m)                                       (ARC_VT_REG_BASE      + 0x00000004 + 0x10000 * (d) + 0x10 * (m))
#define HWIO_DRVd_RMm_TIMER_RMSK                                                   0x1
#define HWIO_DRVd_RMm_TIMER_MAXd                                                    29
#define HWIO_DRVd_RMm_TIMER_MAXm                                                     8
#define HWIO_DRVd_RMm_TIMER_ATTR                                                   0x3
#define HWIO_DRVd_RMm_TIMER_INI2(d,m)        \
        in_dword_masked(HWIO_DRVd_RMm_TIMER_ADDR(d,m), HWIO_DRVd_RMm_TIMER_RMSK)
#define HWIO_DRVd_RMm_TIMER_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_DRVd_RMm_TIMER_ADDR(d,m), mask)
#define HWIO_DRVd_RMm_TIMER_OUTI2(d,m,val)    \
        out_dword(HWIO_DRVd_RMm_TIMER_ADDR(d,m),val)
#define HWIO_DRVd_RMm_TIMER_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_DRVd_RMm_TIMER_ADDR(d,m),mask,val,HWIO_DRVd_RMm_TIMER_INI2(d,m))
#define HWIO_DRVd_RMm_TIMER_TIMER_CONSIDERED_BMSK                                  0x1
#define HWIO_DRVd_RMm_TIMER_TIMER_CONSIDERED_SHFT                                  0x0

#define HWIO_DRVd_RMm_SHADOW_STATUS_ADDR(d,m)                               (ARC_VT_REG_BASE      + 0x00000008 + 0x10000 * (d) + 0x10 * (m))
#define HWIO_DRVd_RMm_SHADOW_STATUS_RMSK                                    0xfffff001
#define HWIO_DRVd_RMm_SHADOW_STATUS_MAXd                                            29
#define HWIO_DRVd_RMm_SHADOW_STATUS_MAXm                                             8
#define HWIO_DRVd_RMm_SHADOW_STATUS_ATTR                                           0x1
#define HWIO_DRVd_RMm_SHADOW_STATUS_INI2(d,m)        \
        in_dword_masked(HWIO_DRVd_RMm_SHADOW_STATUS_ADDR(d,m), HWIO_DRVd_RMm_SHADOW_STATUS_RMSK)
#define HWIO_DRVd_RMm_SHADOW_STATUS_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_DRVd_RMm_SHADOW_STATUS_ADDR(d,m), mask)
#define HWIO_DRVd_RMm_SHADOW_STATUS_DESTINATION_OL_BMSK                     0xf0000000
#define HWIO_DRVd_RMm_SHADOW_STATUS_DESTINATION_OL_SHFT                           0x1c
#define HWIO_DRVd_RMm_SHADOW_STATUS_SEQUENCE_OL_BMSK                         0xf000000
#define HWIO_DRVd_RMm_SHADOW_STATUS_SEQUENCE_OL_SHFT                              0x18
#define HWIO_DRVd_RMm_SHADOW_STATUS_AGGREGATED_OL_BMSK                        0xf00000
#define HWIO_DRVd_RMm_SHADOW_STATUS_AGGREGATED_OL_SHFT                            0x14
#define HWIO_DRVd_RMm_SHADOW_STATUS_SOLVED_OL_BMSK                             0xf0000
#define HWIO_DRVd_RMm_SHADOW_STATUS_SOLVED_OL_SHFT                                0x10
#define HWIO_DRVd_RMm_SHADOW_STATUS_CURRENT_OL_BMSK                             0xf000
#define HWIO_DRVd_RMm_SHADOW_STATUS_CURRENT_OL_SHFT                                0xc
#define HWIO_DRVd_RMm_SHADOW_STATUS_RESPONSE_PENDING_BMSK                          0x1
#define HWIO_DRVd_RMm_SHADOW_STATUS_RESPONSE_PENDING_SHFT                          0x0


#endif /* __HWIO_ARC_ARC_VT_H__ */
