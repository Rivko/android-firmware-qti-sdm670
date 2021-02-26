#ifndef __HWIO_LPASS_QOS_GENERIC_H__
#define __HWIO_LPASS_QOS_GENERIC_H__
/*
===========================================================================
*/
/**
  @file hwio_lpass_qos_generic.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.9_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_QOS_QOS_GENERIC

  'Exclude' filters applied: DUMMY RESERVED 

  Generation parameters: 
  { u'exclude-reserved': True,
    u'filename': u'hwio_lpass_qos_generic.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'LPASS_QOS_QOS_GENERIC'],
    u'output-offsets': True}
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

  $Header: //components/rel/core.qdsp6/2.1.c4/power/dangergen/src/target/845/hwio_lpass_qos_generic.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: LPASS_QOS_QOS_GENERIC
 *--------------------------------------------------------------------------*/

#define LPASS_QOS_QOS_GENERIC_REG_BASE                (LPASS_BASE      + 0x000a0000)
#define LPASS_QOS_QOS_GENERIC_REG_BASE_OFFS           0x000a0000

#define HWIO_LPASS_QOS_HW_VERSION_ADDR                (LPASS_QOS_QOS_GENERIC_REG_BASE      + 0x00000000)
#define HWIO_LPASS_QOS_HW_VERSION_OFFS                (LPASS_QOS_QOS_GENERIC_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_QOS_HW_VERSION_RMSK                0xffffffff
#define HWIO_LPASS_QOS_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_QOS_HW_VERSION_ADDR, HWIO_LPASS_QOS_HW_VERSION_RMSK)
#define HWIO_LPASS_QOS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_QOS_HW_VERSION_ADDR, m)
#define HWIO_LPASS_QOS_HW_VERSION_MAJOR_BMSK          0xf0000000
#define HWIO_LPASS_QOS_HW_VERSION_MAJOR_SHFT                0x1c
#define HWIO_LPASS_QOS_HW_VERSION_MINOR_BMSK           0xfff0000
#define HWIO_LPASS_QOS_HW_VERSION_MINOR_SHFT                0x10
#define HWIO_LPASS_QOS_HW_VERSION_STEP_BMSK               0xffff
#define HWIO_LPASS_QOS_HW_VERSION_STEP_SHFT                  0x0

#define HWIO_LPASS_QOS_NOC_CGC_ADDR                   (LPASS_QOS_QOS_GENERIC_REG_BASE      + 0x00000004)
#define HWIO_LPASS_QOS_NOC_CGC_OFFS                   (LPASS_QOS_QOS_GENERIC_REG_BASE_OFFS + 0x00000004)
#define HWIO_LPASS_QOS_NOC_CGC_RMSK                        0x110
#define HWIO_LPASS_QOS_NOC_CGC_IN          \
        in_dword_masked(HWIO_LPASS_QOS_NOC_CGC_ADDR, HWIO_LPASS_QOS_NOC_CGC_RMSK)
#define HWIO_LPASS_QOS_NOC_CGC_INM(m)      \
        in_dword_masked(HWIO_LPASS_QOS_NOC_CGC_ADDR, m)
#define HWIO_LPASS_QOS_NOC_CGC_OUT(v)      \
        out_dword(HWIO_LPASS_QOS_NOC_CGC_ADDR,v)
#define HWIO_LPASS_QOS_NOC_CGC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QOS_NOC_CGC_ADDR,m,v,HWIO_LPASS_QOS_NOC_CGC_IN)
#define HWIO_LPASS_QOS_NOC_CGC_DANGER_BMSK                 0x100
#define HWIO_LPASS_QOS_NOC_CGC_DANGER_SHFT                   0x8
#define HWIO_LPASS_QOS_NOC_CGC_DEBUG_BMSK                   0x10
#define HWIO_LPASS_QOS_NOC_CGC_DEBUG_SHFT                    0x4


#endif /* __HWIO_LPASS_QOS_GENERIC_H__ */
