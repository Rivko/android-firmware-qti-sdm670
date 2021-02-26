#ifndef __VRM_STATUS_HWIO_H__
#define __VRM_STATUS_HWIO_H__
/*
===========================================================================
*/
/**
  @file vrm_status_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q2r35_MTO_with_AOSS_USB_PHY_ECOs_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_VRM_VRM_STATUS


  Generation parameters: 
  { u'filename': u'vrm_status_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_VRM_VRM_STATUS']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/670/hwio/vrm_status_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_VRM_VRM_STATUS
 *--------------------------------------------------------------------------*/

#define RPMH_VRM_VRM_STATUS_REG_BASE                                     (AOSS_BASE      + 0x009f2000)

#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_ADDR(s)                      (RPMH_VRM_VRM_STATUS_REG_BASE      + 0x00000000 + 0x4 * (s))
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_RMSK                         0x800003ff
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_MAXs                                 31
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_INI(s)        \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_ADDR(s), HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_RMSK)
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_INMI(s,mask)    \
        in_dword_masked(HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_ADDR(s), mask)
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_SEQUENCER_BUSY_BMSK          0x80000000
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_SEQUENCER_BUSY_SHFT                0x1f
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_SEQUENCER_PC_BMSK                 0x3ff
#define HWIO_RPMH_VRM_SEQUENCERs_STATUS_CSR_SEQUENCER_PC_SHFT                   0x0

#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_ADDR                           (RPMH_VRM_VRM_STATUS_REG_BASE      + 0x00000100)
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_RMSK                              0x10111
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_IN          \
        in_dword_masked(HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_ADDR, HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_RMSK)
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_INM(m)      \
        in_dword_masked(HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_ADDR, m)
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_COMP_IN_IDLE_BMSK                 0x10000
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_COMP_IN_IDLE_SHFT                    0x10
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_BE_IN_IDLE_BMSK                     0x100
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_BE_IN_IDLE_SHFT                       0x8
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_CMD_IN_IDLE_BMSK                     0x10
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_CMD_IN_IDLE_SHFT                      0x4
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_VRM_IN_IDLE_BMSK                      0x1
#define HWIO_RPMH_VRM_VRM_IDLE_STATUS_CSR_VRM_IN_IDLE_SHFT                      0x0


#endif /* __VRM_STATUS_HWIO_H__ */
