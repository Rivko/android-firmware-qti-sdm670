#ifndef __VRM_SEQMEM_HWIO_H__
#define __VRM_SEQMEM_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_seqmem_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_SEQMEM


  Generation parameters: 
  { u'filename': u'vrm_seqmem_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_SEQMEM']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/hwio/vrm_seqmem_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_SEQMEM
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_SEQMEM_REG_BASE                                  (AOSS_BASE      + 0x009f3000)

#define HWIO_RPMH_VRM_SEQ_START_ADDRr_ADDR(r)                         (RPMH_VRM_VRM_SEQMEM_REG_BASE      + 0x00000190 + 0x4 * (r))
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_RMSK                                 0x3ff
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_MAXr                                     7
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_START_ADDRr_ADDR(r), HWIO_RPMH_VRM_SEQ_START_ADDRr_RMSK)
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_START_ADDRr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_SEQ_START_ADDRr_ADDR(r),val)
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SEQ_START_ADDRr_ADDR(r),mask,val,HWIO_RPMH_VRM_SEQ_START_ADDRr_INI(r))
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_SEQ_START_ADDR_BMSK                  0x3ff
#define HWIO_RPMH_VRM_SEQ_START_ADDRr_SEQ_START_ADDR_SHFT                    0x0

#define HWIO_RPMH_VRM_SEQ_WORDr_ADDR(r)                               (RPMH_VRM_VRM_SEQMEM_REG_BASE      + 0x000001f4 + 0x4 * (r))
#define HWIO_RPMH_VRM_SEQ_WORDr_RMSK                                  0xffffffff
#define HWIO_RPMH_VRM_SEQ_WORDr_MAXr                                          31
#define HWIO_RPMH_VRM_SEQ_WORDr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_WORDr_ADDR(r), HWIO_RPMH_VRM_SEQ_WORDr_RMSK)
#define HWIO_RPMH_VRM_SEQ_WORDr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_WORDr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SEQ_WORDr_OUTI(r,val)    \
        out_dword(HWIO_RPMH_VRM_SEQ_WORDr_ADDR(r),val)
#define HWIO_RPMH_VRM_SEQ_WORDr_OUTMI(r,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_VRM_SEQ_WORDr_ADDR(r),mask,val,HWIO_RPMH_VRM_SEQ_WORDr_INI(r))
#define HWIO_RPMH_VRM_SEQ_WORDr_SEQ_WORD_BMSK                         0xffffffff
#define HWIO_RPMH_VRM_SEQ_WORDr_SEQ_WORD_SHFT                                0x0

#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_ADDR(r)                     (RPMH_VRM_VRM_SEQMEM_REG_BASE      + 0x000005f4 + 0xC * (r))
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_RMSK                         0x7ffffff
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_MAXr                                31
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_ADDR(r), HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_RMSK)
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_SETTLING_TIME_BMSK           0x7fff800
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_SETTLING_TIME_SHFT                 0xb
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_TASK_ID_BMSK                     0x700
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_TASK_ID_SHFT                       0x8
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_RESOURCE_ID_BMSK                  0xff
#define HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_RESOURCE_ID_SHFT                   0x0

#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_ADDR(r)                     (RPMH_VRM_VRM_SEQMEM_REG_BASE      + 0x000005f8 + 0xC * (r))
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RMSK                        0xffffffff
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_MAXr                                31
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_ADDR(r), HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RMSK)
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RESOURCE_ADDR_BMSK          0xffff0000
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RESOURCE_ADDR_SHFT                0x10
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RESOURCE_DATA_BMSK              0xffff
#define HWIO_RPMH_VRM_SEQ_DATA_1_SEQCERSr_RESOURCE_DATA_SHFT                 0x0

#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_ADDR(r)                     (RPMH_VRM_VRM_SEQMEM_REG_BASE      + 0x000005fc + 0xC * (r))
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RMSK                        0xffffffff
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_MAXr                                31
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_INI(r)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_ADDR(r), HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RMSK)
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_INMI(r,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_ADDR(r), mask)
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RESOURCE_ADDR_BMSK          0xffff0000
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RESOURCE_ADDR_SHFT                0x10
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RESOURCE_DATA_BMSK              0xffff
#define HWIO_RPMH_VRM_SEQ_DATA_2_SEQCERSr_RESOURCE_DATA_SHFT                 0x0


#endif /* __VRM_SEQMEM_HWIO_H__ */
