#ifndef __MSMHWIOREG_H__
#define __MSMHWIOREG_H__
/*
===========================================================================
*/
/**
  @file msmhwioreg.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SM7150 (Moorea) [moorea_v1.0_p3q1r47]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: QFPROM_CORR_PTE_ROW1[SECURITY_CONTROL_CORE] OEM_ID[SECURITY_CONTROL_CORE] 
  'Exclude' filters applied: DUMMY RESERVED 

  Generation parameters: 
  { u'exclude-reserved': True,
    u'filename': u'msmhwioreg.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'SECURITY_CONTROL_CORE': [ u'QFPROM_CORR_PTE_ROW1',
                                                            u'OEM_ID']},
    u'modules': [u'SECURITY_CONTROL_CORE'],
    u'output-attrs': True,
    u'output-fvals': True,
    u'output-offsets': True,
    u'output-resets': True}

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
*/
/*
  ===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
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

  $Header: //components/rel/platform.dsp/1.0.c3/security/config/sm7150/msmhwioreg.h#1 $
  $DateTime: 2018/07/30 03:09:14 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                          (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_SIZE                                                     0x7000
#define SECURITY_CONTROL_CORE_REG_BASE_USED                                                     0x6400
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                     0x00000000

#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_POR                                                       0x00000000
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_POR_RMSK                                                  0x00000000
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ATTR                                                             0x1
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_POR                                                       0x00000000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_POR_RMSK                                                  0x00000000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ATTR                                                             0x1
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD0_BMSK                                                0xc0000000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD0_SHFT                                                      0x1e
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_EBI_RETENTION_BMSK                                        0x30000000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_EBI_RETENTION_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PARTIAL_GOOD_BMSK                                          0xf800000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PARTIAL_GOOD_SHFT                                               0x17
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CPU_GLOBAL_VS_LOCAL_BMSK                                    0x400000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CPU_GLOBAL_VS_LOCAL_SHFT                                        0x16
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_SPARE0_BMSK                                                 0x3f0000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_SPARE0_SHFT                                                     0x10
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_BMSK                                                  0xffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_SHFT                                                     0x0

#define HWIO_OEM_ID_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006138)
#define HWIO_OEM_ID_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006138)
#define HWIO_OEM_ID_RMSK                                                                        0xffffffff
#define HWIO_OEM_ID_POR                                                                         0x00000000
#define HWIO_OEM_ID_POR_RMSK                                                                    0x00000000
#define HWIO_OEM_ID_ATTR                                                                               0x1
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                 0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                       0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                             0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                                0x0


#endif /* __MSMHWIOREG_H__ */
