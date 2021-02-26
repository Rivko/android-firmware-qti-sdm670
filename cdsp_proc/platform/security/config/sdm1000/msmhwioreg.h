#ifndef __HWIO_H__
#define __HWIO_H__
/*
===========================================================================
*/
/**
  @file hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM1000 (Poipu) [poipu_v1.0_p3q3r65_IPCAT]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Exclude' filters applied: DUMMY RESERVED 

  Generation parameters: 
  { u'exclude-reserved': True,
    u'filename': 'hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'SECURITY_CONTROL_CORE']}
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

  $Header: //components/rel/platform.dsp/1.0.c3/security/config/sdm1000/msmhwioreg.h#1 $
  $DateTime: 2018/07/30 03:09:14 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                          (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_JTAG_ID_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006130)
#define HWIO_JTAG_ID_RMSK                                                                       0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                               0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                      0x0

#define HWIO_SERIAL_NUM_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006134)
#define HWIO_SERIAL_NUM_RMSK                                                                    0xffffffff
#define HWIO_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, HWIO_SERIAL_NUM_RMSK)
#define HWIO_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, m)
#define HWIO_SERIAL_NUM_SERIAL_NUM_BMSK                                                         0xffffffff
#define HWIO_SERIAL_NUM_SERIAL_NUM_SHFT                                                                0x0

#define HWIO_OEM_ID_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006138)
#define HWIO_OEM_ID_RMSK                                                                        0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                 0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                       0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                             0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                                0x0



#endif /* __HWIO_H__ */
