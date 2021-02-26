#ifndef __MSMHWIOREG_H__
#define __MSMHWIOREG_H__
/*
===========================================================================
*/
/**
  @file msmhwioreg.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8952 (Sahi) [sahi_v1.0_p3q3r139_BTO]

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied:
  'Exclude' filters applied: RESERVED DUMMY
*/
/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                         (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                    0x00000000



#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_RMSK                                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, HWIO_QFPROM_CORR_PTE2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, m)

#define HWIO_QFPROM_RAW_PTE2_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_RMSK                                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, HWIO_QFPROM_RAW_PTE2_RMSK)
#define HWIO_QFPROM_RAW_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, m)
#define HWIO_QFPROM_RAW_PTE2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE2_ADDR,v)
#define HWIO_QFPROM_RAW_PTE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE2_ADDR,m,v,HWIO_QFPROM_RAW_PTE2_IN)

#define HWIO_OEM_ID_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006080)
#define HWIO_OEM_ID_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006080)
#define HWIO_OEM_ID_RMSK                                                                       0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                      0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                            0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                               0x0


#endif /* __MSMHWIOREG_H__ */
