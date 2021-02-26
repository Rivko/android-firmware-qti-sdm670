#ifndef __TCSR_HWIO_H__
#define __TCSR_HWIO_H__
/*
===========================================================================
*/
/**
  @file tcsr_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    TCSR_TCSR_REGS

  'Include' filters applied: TCSR_SOC_HW_VERSION[TCSR_TCSR_REGS] 

  Generation parameters: 
  { u'filename': u'tcsr_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'TCSR_TCSR_REGS': [u'TCSR_SOC_HW_VERSION']},
    u'modules': [u'TCSR_TCSR_REGS'],
    u'output-offsets': True,
    u'output-phys': True}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/target/670/hwio/tcsr_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"
#include "msmhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                      (CORE_TOP_CSR_BASE      + 0x000c0000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS                                                                                 (CORE_TOP_CSR_BASE_PHYS + 0x000c0000)
#define TCSR_TCSR_REGS_REG_BASE_OFFS                                                                                 0x000c0000

#define HWIO_TCSR_SOC_HW_VERSION_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00008000)
#define HWIO_TCSR_SOC_HW_VERSION_PHYS                                                                                (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x00008000)
#define HWIO_TCSR_SOC_HW_VERSION_OFFS                                                                                (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00008000)
#define HWIO_TCSR_SOC_HW_VERSION_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SOC_HW_VERSION_IN          \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, HWIO_TCSR_SOC_HW_VERSION_RMSK)
#define HWIO_TCSR_SOC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, m)
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_BMSK                                                                  0xf0000000
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_SHFT                                                                        0x1c
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_BMSK                                                                   0xfff0000
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_SHFT                                                                        0x10
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_BMSK                                                                      0xff00
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_SHFT                                                                         0x8
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_BMSK                                                                        0xff
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_SHFT                                                                         0x0


#endif /* __TCSR_HWIO_H__ */
