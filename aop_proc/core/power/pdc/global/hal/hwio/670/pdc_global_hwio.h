#ifndef __PDC_GLOBAL_HWIO_H__
#define __PDC_GLOBAL_HWIO_H__
/*
===========================================================================
*/
/**
  @file pdc_global_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_PDC_PDC_GLOBAL


  Generation parameters: 
  { u'filename': u'pdc_global_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'RPMH_PDC_PDC_GLOBAL'],
    u'output-fvals': True}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/pdc/global/hal/hwio/670/pdc_global_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RPMH_PDC_PDC_GLOBAL
 *--------------------------------------------------------------------------*/

#define RPMH_PDC_PDC_GLOBAL_REG_BASE                                                                (AOSS_BASE      + 0x002e0000)

#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_ADDR(d)                                                  (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000020 + 0x4 * (d))
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_RMSK                                                           0x1f
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_MAXd                                                              2
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_APPS_DRV_ID_BMSK                                               0x1f
#define HWIO_RPMH_PDC_APPS_PDC_RSC_DRV_IDd_APPS_DRV_ID_SHFT                                                0x0

#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_ADDR(d)                                                    (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000050 + 0x4 * (d))
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_RMSK                                                             0x1f
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_MAXd                                                                0
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_SP_DRV_ID_BMSK                                                   0x1f
#define HWIO_RPMH_PDC_SP_PDC_RSC_DRV_IDd_SP_DRV_ID_SHFT                                                    0x0

#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_ADDR(d)                                                 (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000060 + 0x4 * (d))
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_RMSK                                                          0x1f
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_MAXd                                                             0
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_AUDIO_DRV_ID_BMSK                                             0x1f
#define HWIO_RPMH_PDC_AUDIO_PDC_RSC_DRV_IDd_AUDIO_DRV_ID_SHFT                                              0x0

#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_ADDR(d)                                               (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000070 + 0x4 * (d))
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_RMSK                                                        0x1f
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_MAXd                                                           0
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_SENSORS_DRV_ID_BMSK                                         0x1f
#define HWIO_RPMH_PDC_SENSORS_PDC_RSC_DRV_IDd_SENSORS_DRV_ID_SHFT                                          0x0

#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_ADDR(d)                                                   (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000080 + 0x4 * (d))
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_RMSK                                                            0x1f
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_MAXd                                                               0
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_AOP_DRV_ID_BMSK                                                 0x1f
#define HWIO_RPMH_PDC_AOP_PDC_RSC_DRV_IDd_AOP_DRV_ID_SHFT                                                  0x0

#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_ADDR(d)                                                 (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000090 + 0x4 * (d))
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_RMSK                                                          0x1f
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_MAXd                                                             0
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_DEBUG_DRV_ID_BMSK                                             0x1f
#define HWIO_RPMH_PDC_DEBUG_PDC_RSC_DRV_IDd_DEBUG_DRV_ID_SHFT                                              0x0

#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_ADDR(d)                                                   (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x000000a0 + 0x4 * (d))
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_RMSK                                                            0x1f
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_MAXd                                                               0
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_GPU_DRV_ID_BMSK                                                 0x1f
#define HWIO_RPMH_PDC_GPU_PDC_RSC_DRV_IDd_GPU_DRV_ID_SHFT                                                  0x0

#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_ADDR(d)                                               (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x000000b0 + 0x4 * (d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_RMSK                                                        0x1f
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_MAXd                                                           0
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_DISPLAY_DRV_ID_BMSK                                         0x1f
#define HWIO_RPMH_PDC_DISPLAY_PDC_RSC_DRV_IDd_DISPLAY_DRV_ID_SHFT                                          0x0

#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_ADDR(d)                                               (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x000000c0 + 0x4 * (d))
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_RMSK                                                        0x1f
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_MAXd                                                           0
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_COMPUTE_DRV_ID_BMSK                                         0x1f
#define HWIO_RPMH_PDC_COMPUTE_PDC_RSC_DRV_IDd_COMPUTE_DRV_ID_SHFT                                          0x0

#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_ADDR(d)                                                 (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x000000d0 + 0x4 * (d))
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_RMSK                                                          0x1f
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_MAXd                                                             1
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_INI(d)        \
        in_dword_masked(HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_ADDR(d), HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_RMSK)
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_INMI(d,mask)    \
        in_dword_masked(HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_ADDR(d), mask)
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_OUTI(d,val)    \
        out_dword(HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_ADDR(d),val)
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_ADDR(d),mask,val,HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_INI(d))
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_MODEM_DRV_ID_BMSK                                             0x1f
#define HWIO_RPMH_PDC_MODEM_PDC_RSC_DRV_IDd_MODEM_DRV_ID_SHFT                                              0x0

#define HWIO_RPMH_PDC_SYNC_RESET_ADDR                                                               (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000100)
#define HWIO_RPMH_PDC_SYNC_RESET_RMSK                                                                    0x3ff
#define HWIO_RPMH_PDC_SYNC_RESET_IN          \
        in_dword_masked(HWIO_RPMH_PDC_SYNC_RESET_ADDR, HWIO_RPMH_PDC_SYNC_RESET_RMSK)
#define HWIO_RPMH_PDC_SYNC_RESET_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_SYNC_RESET_ADDR, m)
#define HWIO_RPMH_PDC_SYNC_RESET_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_SYNC_RESET_ADDR,v)
#define HWIO_RPMH_PDC_SYNC_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_SYNC_RESET_ADDR,m,v,HWIO_RPMH_PDC_SYNC_RESET_IN)
#define HWIO_RPMH_PDC_SYNC_RESET_MODEM_SYNC_RESET_BMSK                                                   0x200
#define HWIO_RPMH_PDC_SYNC_RESET_MODEM_SYNC_RESET_SHFT                                                     0x9
#define HWIO_RPMH_PDC_SYNC_RESET_MODEM_SYNC_RESET_RESET_RELEASE_FVAL                                       0x0
#define HWIO_RPMH_PDC_SYNC_RESET_MODEM_SYNC_RESET_RESET_FVAL                                               0x1
#define HWIO_RPMH_PDC_SYNC_RESET_COMPUTE_SYNC_RESET_BMSK                                                 0x100
#define HWIO_RPMH_PDC_SYNC_RESET_COMPUTE_SYNC_RESET_SHFT                                                   0x8
#define HWIO_RPMH_PDC_SYNC_RESET_COMPUTE_SYNC_RESET_RESET_RELEASE_FVAL                                     0x0
#define HWIO_RPMH_PDC_SYNC_RESET_COMPUTE_SYNC_RESET_RESET_FVAL                                             0x1
#define HWIO_RPMH_PDC_SYNC_RESET_DISPLAY_SYNC_RESET_BMSK                                                  0x80
#define HWIO_RPMH_PDC_SYNC_RESET_DISPLAY_SYNC_RESET_SHFT                                                   0x7
#define HWIO_RPMH_PDC_SYNC_RESET_DISPLAY_SYNC_RESET_RESET_RELEASE_FVAL                                     0x0
#define HWIO_RPMH_PDC_SYNC_RESET_DISPLAY_SYNC_RESET_RESET_FVAL                                             0x1
#define HWIO_RPMH_PDC_SYNC_RESET_GPU_SYNC_RESET_BMSK                                                      0x40
#define HWIO_RPMH_PDC_SYNC_RESET_GPU_SYNC_RESET_SHFT                                                       0x6
#define HWIO_RPMH_PDC_SYNC_RESET_GPU_SYNC_RESET_RESET_RELEASE_FVAL                                         0x0
#define HWIO_RPMH_PDC_SYNC_RESET_GPU_SYNC_RESET_RESET_FVAL                                                 0x1
#define HWIO_RPMH_PDC_SYNC_RESET_DEBUG_SYNC_RESET_BMSK                                                    0x20
#define HWIO_RPMH_PDC_SYNC_RESET_DEBUG_SYNC_RESET_SHFT                                                     0x5
#define HWIO_RPMH_PDC_SYNC_RESET_DEBUG_SYNC_RESET_RESET_RELEASE_FVAL                                       0x0
#define HWIO_RPMH_PDC_SYNC_RESET_DEBUG_SYNC_RESET_RESET_FVAL                                               0x1
#define HWIO_RPMH_PDC_SYNC_RESET_AOP_SYNC_RESET_BMSK                                                      0x10
#define HWIO_RPMH_PDC_SYNC_RESET_AOP_SYNC_RESET_SHFT                                                       0x4
#define HWIO_RPMH_PDC_SYNC_RESET_AOP_SYNC_RESET_RESET_RELEASE_FVAL                                         0x0
#define HWIO_RPMH_PDC_SYNC_RESET_AOP_SYNC_RESET_RESET_FVAL                                                 0x1
#define HWIO_RPMH_PDC_SYNC_RESET_SENSORS_SYNC_RESET_BMSK                                                   0x8
#define HWIO_RPMH_PDC_SYNC_RESET_SENSORS_SYNC_RESET_SHFT                                                   0x3
#define HWIO_RPMH_PDC_SYNC_RESET_SENSORS_SYNC_RESET_RESET_RELEASE_FVAL                                     0x0
#define HWIO_RPMH_PDC_SYNC_RESET_SENSORS_SYNC_RESET_RESET_FVAL                                             0x1
#define HWIO_RPMH_PDC_SYNC_RESET_AUDIO_SYNC_RESET_BMSK                                                     0x4
#define HWIO_RPMH_PDC_SYNC_RESET_AUDIO_SYNC_RESET_SHFT                                                     0x2
#define HWIO_RPMH_PDC_SYNC_RESET_AUDIO_SYNC_RESET_RESET_RELEASE_FVAL                                       0x0
#define HWIO_RPMH_PDC_SYNC_RESET_AUDIO_SYNC_RESET_RESET_FVAL                                               0x1
#define HWIO_RPMH_PDC_SYNC_RESET_SP_SYNC_RESET_BMSK                                                        0x2
#define HWIO_RPMH_PDC_SYNC_RESET_SP_SYNC_RESET_SHFT                                                        0x1
#define HWIO_RPMH_PDC_SYNC_RESET_SP_SYNC_RESET_RESET_RELEASE_FVAL                                          0x0
#define HWIO_RPMH_PDC_SYNC_RESET_SP_SYNC_RESET_RESET_FVAL                                                  0x1
#define HWIO_RPMH_PDC_SYNC_RESET_APPS_SYNC_RESET_BMSK                                                      0x1
#define HWIO_RPMH_PDC_SYNC_RESET_APPS_SYNC_RESET_SHFT                                                      0x0
#define HWIO_RPMH_PDC_SYNC_RESET_APPS_SYNC_RESET_RESET_RELEASE_FVAL                                        0x0
#define HWIO_RPMH_PDC_SYNC_RESET_APPS_SYNC_RESET_RESET_FVAL                                                0x1

#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_ADDR                                                   (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000140)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_RMSK                                                       0xffff
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_IN          \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_ADDR, HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_RMSK)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_ADDR, m)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_ADDR,v)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_ADDR,m,v,HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_IN)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_EPCB_TIMEOUT_THRESHOLD_BMSK                                0xffff
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_THRESHOLD_EPCB_TIMEOUT_THRESHOLD_SHFT                                   0x0

#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_ADDR                                                (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000150)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_RMSK                                                     0x3ff
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_ADDR, HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_RMSK)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_ADDR,m,v,HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_IN)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_MODEM_EPCB_TIMEOUT_IRQ_EN_BMSK                           0x200
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_MODEM_EPCB_TIMEOUT_IRQ_EN_SHFT                             0x9
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_MODEM_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL           0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_MODEM_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL            0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_EN_BMSK                         0x100
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_EN_SHFT                           0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL         0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL          0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_EN_BMSK                          0x80
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_EN_SHFT                           0x7
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL         0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL          0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_GPU_EPCB_TIMEOUT_IRQ_EN_BMSK                              0x40
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_GPU_EPCB_TIMEOUT_IRQ_EN_SHFT                               0x6
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_GPU_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL             0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_GPU_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL              0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_EN_BMSK                            0x20
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_EN_SHFT                             0x5
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL           0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL            0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AOP_EPCB_TIMEOUT_IRQ_EN_BMSK                              0x10
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AOP_EPCB_TIMEOUT_IRQ_EN_SHFT                               0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AOP_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL             0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AOP_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL              0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_EN_BMSK                           0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_EN_SHFT                           0x3
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL         0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL          0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_EN_BMSK                             0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_EN_SHFT                             0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL           0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL            0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SP_EPCB_TIMEOUT_IRQ_EN_BMSK                                0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SP_EPCB_TIMEOUT_IRQ_EN_SHFT                                0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SP_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_SP_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_APPS_EPCB_TIMEOUT_IRQ_EN_BMSK                              0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_APPS_EPCB_TIMEOUT_IRQ_EN_SHFT                              0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_APPS_EPCB_TIMEOUT_IRQ_EN_DISABLE_INTERRUPT_FVAL            0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_EN_BANK0_APPS_EPCB_TIMEOUT_IRQ_EN_ENABLE_INTERRUPT_FVAL             0x1

#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_ADDR                                            (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000160)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_RMSK                                                 0x3ff
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_MODEM_EPCB_TIMEOUT_IRQ_STATUS_BMSK                   0x200
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_MODEM_EPCB_TIMEOUT_IRQ_STATUS_SHFT                     0x9
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_MODEM_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_MODEM_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_STATUS_BMSK                 0x100
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_STATUS_SHFT                   0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_STATUS_BMSK                  0x80
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_STATUS_SHFT                   0x7
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_GPU_EPCB_TIMEOUT_IRQ_STATUS_BMSK                      0x40
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_GPU_EPCB_TIMEOUT_IRQ_STATUS_SHFT                       0x6
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_GPU_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_GPU_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_STATUS_BMSK                    0x20
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_STATUS_SHFT                     0x5
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AOP_EPCB_TIMEOUT_IRQ_STATUS_BMSK                      0x10
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AOP_EPCB_TIMEOUT_IRQ_STATUS_SHFT                       0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AOP_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AOP_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_STATUS_BMSK                   0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_STATUS_SHFT                   0x3
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_STATUS_BMSK                     0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_STATUS_SHFT                     0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SP_EPCB_TIMEOUT_IRQ_STATUS_BMSK                        0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SP_EPCB_TIMEOUT_IRQ_STATUS_SHFT                        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SP_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_SP_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_APPS_EPCB_TIMEOUT_IRQ_STATUS_BMSK                      0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_APPS_EPCB_TIMEOUT_IRQ_STATUS_SHFT                      0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_APPS_EPCB_TIMEOUT_IRQ_STATUS_NO_INTERRUPT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_STATUS_BANK0_APPS_EPCB_TIMEOUT_IRQ_STATUS_INTERRUPT_OCCURED_FVAL        0x1

#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_ADDR                                             (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000170)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_RMSK                                                  0x3ff
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_ADDR, HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_RMSK)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_ADDR,m,v,HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_IN)
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_MODEM_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                     0x200
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_MODEM_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                       0x9
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                   0x100
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_COMPUTE_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                     0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                    0x80
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_DISPLAY_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                     0x7
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_GPU_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                        0x40
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_GPU_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                         0x6
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                      0x20
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_DEBUG_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                       0x5
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_AOP_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                        0x10
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_AOP_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                         0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                     0x8
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_SENSORS_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                     0x3
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                       0x4
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_AUDIO_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                       0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_SP_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                          0x2
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_SP_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                          0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_APPS_EPCB_TIMEOUT_IRQ_CLEAR_BMSK                        0x1
#define HWIO_RPMH_PDC_EPCB_TIMEOUT_IRQ_CLEAR_BANK0_APPS_EPCB_TIMEOUT_IRQ_CLEAR_SHFT                        0x0

#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_ADDR                                                         (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000200)
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_RMSK                                                            0xfffff
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_ADDR, HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_ADDR,m,v,HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_IN)
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_1_BMSK                                     0x80000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_1_SHFT                                        0x13
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_0_BMSK                                     0x40000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_0_SHFT                                        0x12
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_MODEM_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_1_BMSK                                   0x20000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_1_SHFT                                      0x11
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_0_BMSK                                   0x10000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_0_SHFT                                      0x10
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL            0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_COMPUTE_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_1_BMSK                                    0x8000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_1_SHFT                                       0xf
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_0_BMSK                                    0x4000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_0_SHFT                                       0xe
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL            0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DISPLAY_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_1_BMSK                                        0x2000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_1_SHFT                                           0xd
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                  0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                   0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_0_BMSK                                        0x1000
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_0_SHFT                                           0xc
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL                0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_GPU_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_1_BMSK                                       0x800
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_1_SHFT                                         0xb
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_0_BMSK                                       0x400
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_0_SHFT                                         0xa
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_DEBUG_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_1_BMSK                                         0x200
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_1_SHFT                                           0x9
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                  0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                   0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_0_BMSK                                         0x100
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_0_SHFT                                           0x8
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL                0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AOP_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_1_BMSK                                      0x80
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_1_SHFT                                       0x7
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_0_BMSK                                      0x40
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_0_SHFT                                       0x6
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL            0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SENSORS_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_1_BMSK                                        0x20
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_1_SHFT                                         0x5
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_0_BMSK                                        0x10
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_0_SHFT                                         0x4
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL              0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_AUDIO_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_1_BMSK                                            0x8
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_1_SHFT                                            0x3
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                    0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_0_BMSK                                            0x4
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_0_SHFT                                            0x2
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_SP_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL                  0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_1_BMSK                                          0x2
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_1_SHFT                                          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_1_DISABLE_WAKEUP_INTERRUPT_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_1_ENABLE_WAKEUP_INTERRUPT_FVAL                  0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_0_BMSK                                          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_0_SHFT                                          0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_0_DISABLE_SHUTDOWN_INTERRUPT_FVAL               0x0
#define HWIO_RPMH_PDC_AOP_IRQ_EN_BANK0_APPS_PDC_AOP_IRQ_EN_0_ENABLE_SHUTDOWN_INTERRUPT_FVAL                0x1

#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_ADDR                                                     (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000210)
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_RMSK                                                        0xfffff
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_1_BMSK                             0x80000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_1_SHFT                                0x13
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_0_BMSK                             0x40000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_0_SHFT                                0x12
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_MODEM_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_1_BMSK                           0x20000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_1_SHFT                              0x11
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_0_BMSK                           0x10000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_0_SHFT                              0x10
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_1_BMSK                            0x8000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_1_SHFT                               0xf
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_0_BMSK                            0x4000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_0_SHFT                               0xe
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_1_BMSK                                0x2000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_1_SHFT                                   0xd
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_0_BMSK                                0x1000
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_0_SHFT                                   0xc
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_GPU_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_1_BMSK                               0x800
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_1_SHFT                                 0xb
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_0_BMSK                               0x400
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_0_SHFT                                 0xa
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_1_BMSK                                 0x200
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_1_SHFT                                   0x9
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_0_BMSK                                 0x100
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_0_SHFT                                   0x8
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AOP_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_1_BMSK                              0x80
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_1_SHFT                               0x7
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_0_BMSK                              0x40
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_0_SHFT                               0x6
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_1_BMSK                                0x20
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_1_SHFT                                 0x5
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_0_BMSK                                0x10
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_0_SHFT                                 0x4
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_1_BMSK                                    0x8
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_1_SHFT                                    0x3
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL           0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_0_BMSK                                    0x4
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_0_SHFT                                    0x2
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_SP_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL         0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_1_BMSK                                  0x2
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_1_SHFT                                  0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL         0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_0_BMSK                                  0x1
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_0_SHFT                                  0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_STATUS_BANK0_APPS_PDC_AOP_IRQ_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1

#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_ADDR                                                 (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000214)
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_RMSK                                                    0xfffff
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                     0x80000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                        0x13
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                     0x40000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                        0x12
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_MODEM_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                   0x20000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                      0x11
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                   0x10000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                      0x10
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_COMPUTE_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                    0x8000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                       0xf
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                    0x4000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                       0xe
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DISPLAY_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                        0x2000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                           0xd
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                        0x1000
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                           0xc
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_GPU_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                       0x800
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                         0xb
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                       0x400
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                         0xa
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_DEBUG_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                         0x200
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                           0x9
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                         0x100
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                           0x8
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AOP_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                      0x80
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                       0x7
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                      0x40
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                       0x6
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SENSORS_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                        0x20
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                         0x5
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                        0x10
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                         0x4
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_AUDIO_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                            0x8
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                            0x3
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                            0x4
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                            0x2
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_SP_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_1_BMSK                          0x2
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_1_SHFT                          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_1_WAKEUP_INTERRUPT_OCCURED_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_0_BMSK                          0x1
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_0_SHFT                          0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_NOT_OCCURED_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_IRQ_RAW_STATUS_BANK0_APPS_PDC_AOP_IRQ_RAW_STATUS_0_SHUTDOWN_INTERRUPT_OCCURED_FVAL        0x1

#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_ADDR                                                      (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000218)
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_RMSK                                                         0xfffff
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_ADDR, HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_ADDR,m,v,HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_IN)
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_MODEM_PDC_AOP_IRQ_CLEAR_1_BMSK                               0x80000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_MODEM_PDC_AOP_IRQ_CLEAR_1_SHFT                                  0x13
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_MODEM_PDC_AOP_IRQ_CLEAR_0_BMSK                               0x40000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_MODEM_PDC_AOP_IRQ_CLEAR_0_SHFT                                  0x12
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_COMPUTE_PDC_AOP_IRQ_CLEAR_1_BMSK                             0x20000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_COMPUTE_PDC_AOP_IRQ_CLEAR_1_SHFT                                0x11
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_COMPUTE_PDC_AOP_IRQ_CLEAR_0_BMSK                             0x10000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_COMPUTE_PDC_AOP_IRQ_CLEAR_0_SHFT                                0x10
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DISPLAY_PDC_AOP_IRQ_CLEAR_1_BMSK                              0x8000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DISPLAY_PDC_AOP_IRQ_CLEAR_1_SHFT                                 0xf
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DISPLAY_PDC_AOP_IRQ_CLEAR_0_BMSK                              0x4000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DISPLAY_PDC_AOP_IRQ_CLEAR_0_SHFT                                 0xe
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_GPU_PDC_AOP_IRQ_CLEAR_1_BMSK                                  0x2000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_GPU_PDC_AOP_IRQ_CLEAR_1_SHFT                                     0xd
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_GPU_PDC_AOP_IRQ_CLEAR_0_BMSK                                  0x1000
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_GPU_PDC_AOP_IRQ_CLEAR_0_SHFT                                     0xc
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DEBUG_PDC_AOP_IRQ_CLEAR_1_BMSK                                 0x800
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DEBUG_PDC_AOP_IRQ_CLEAR_1_SHFT                                   0xb
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DEBUG_PDC_AOP_IRQ_CLEAR_0_BMSK                                 0x400
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_DEBUG_PDC_AOP_IRQ_CLEAR_0_SHFT                                   0xa
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AOP_PDC_AOP_IRQ_CLEAR_1_BMSK                                   0x200
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AOP_PDC_AOP_IRQ_CLEAR_1_SHFT                                     0x9
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AOP_PDC_AOP_IRQ_CLEAR_0_BMSK                                   0x100
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AOP_PDC_AOP_IRQ_CLEAR_0_SHFT                                     0x8
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SENSORS_PDC_AOP_IRQ_CLEAR_1_BMSK                                0x80
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SENSORS_PDC_AOP_IRQ_CLEAR_1_SHFT                                 0x7
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SENSORS_PDC_AOP_IRQ_CLEAR_0_BMSK                                0x40
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SENSORS_PDC_AOP_IRQ_CLEAR_0_SHFT                                 0x6
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AUDIO_PDC_AOP_IRQ_CLEAR_1_BMSK                                  0x20
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AUDIO_PDC_AOP_IRQ_CLEAR_1_SHFT                                   0x5
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AUDIO_PDC_AOP_IRQ_CLEAR_0_BMSK                                  0x10
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_AUDIO_PDC_AOP_IRQ_CLEAR_0_SHFT                                   0x4
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SP_PDC_AOP_IRQ_CLEAR_1_BMSK                                      0x8
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SP_PDC_AOP_IRQ_CLEAR_1_SHFT                                      0x3
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SP_PDC_AOP_IRQ_CLEAR_0_BMSK                                      0x4
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_SP_PDC_AOP_IRQ_CLEAR_0_SHFT                                      0x2
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_APPS_PDC_AOP_IRQ_CLEAR_1_BMSK                                    0x2
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_APPS_PDC_AOP_IRQ_CLEAR_1_SHFT                                    0x1
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_APPS_PDC_AOP_IRQ_CLEAR_0_BMSK                                    0x1
#define HWIO_RPMH_PDC_AOP_IRQ_CLEAR_BANK0_APPS_PDC_AOP_IRQ_CLEAR_0_SHFT                                    0x0

#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_ADDR                                                     (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000220)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_RMSK                                                        0xfffff
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_ADDR, HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_ADDR,m,v,HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_IN)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_1_BMSK                             0x80000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_1_SHFT                                0x13
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_0_BMSK                             0x40000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_0_SHFT                                0x12
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_MODEM_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_1_BMSK                           0x20000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_1_SHFT                              0x11
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_0_BMSK                           0x10000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_0_SHFT                              0x10
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL           0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_1_BMSK                            0x8000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_1_SHFT                               0xf
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_0_BMSK                            0x4000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_0_SHFT                               0xe
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL           0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_1_BMSK                                0x2000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_1_SHFT                                   0xd
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                     0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_0_BMSK                                0x1000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_0_SHFT                                   0xc
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                     0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_GPU_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_1_BMSK                               0x800
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_1_SHFT                                 0xb
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_0_BMSK                               0x400
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_0_SHFT                                 0xa
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_1_BMSK                                 0x200
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_1_SHFT                                   0x9
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                     0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL                 0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_0_BMSK                                 0x100
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_0_SHFT                                   0x8
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                     0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AOP_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_1_BMSK                              0x80
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_1_SHFT                               0x7
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_0_BMSK                              0x40
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_0_SHFT                               0x6
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                 0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL           0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_1_BMSK                                0x20
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_1_SHFT                                 0x5
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL               0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_0_BMSK                                0x10
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_0_SHFT                                 0x4
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                   0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL             0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_1_BMSK                                    0x8
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_1_SHFT                                    0x3
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                      0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL                  0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_0_BMSK                                    0x4
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_0_SHFT                                    0x2
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                      0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_SP_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL                0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_1_BMSK                                  0x2
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_1_SHFT                                  0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_1_DO_NOT_RESUME_FVAL                    0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_1_RESUME_WAKEUP_SEQ_FVAL                0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_0_BMSK                                  0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_0_SHFT                                  0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_0_DO_NOT_RESUME_FVAL                    0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_BANK0_APPS_PDC_AOP_SEQ_RESUME_0_RESUME_SHUTDOWN_SEQ_FVAL              0x1

#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_ADDR                                                (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000230)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_RMSK                                                   0xfffff
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_ADDR, HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_RMSK)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_ADDR,m,v,HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_IN)
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                   0x80000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                      0x13
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                   0x40000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                      0x12
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_MODEM_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                 0x20000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                    0x11
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                 0x10000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                    0x10
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_COMPUTE_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                  0x8000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                     0xf
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                  0x4000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                     0xe
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DISPLAY_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                      0x2000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                         0xd
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                      0x1000
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                         0xc
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_GPU_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                     0x800
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                       0xb
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                     0x400
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                       0xa
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_DEBUG_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                       0x200
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                         0x9
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                       0x100
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                         0x8
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AOP_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                    0x80
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                     0x7
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                    0x40
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                     0x6
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SENSORS_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                      0x20
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                       0x5
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                      0x10
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                       0x4
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_AUDIO_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                          0x8
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                          0x3
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                          0x4
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                          0x2
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_SP_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_1_BMSK                        0x2
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_1_SHFT                        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_1_DO_NOT_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_1_RESUME_WAKEUP_SEQ_ONCE_FVAL        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_0_BMSK                        0x1
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_0_SHFT                        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_0_DO_NOT_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x0
#define HWIO_RPMH_PDC_AOP_SEQ_RESUME_ONCE_BANK0_APPS_PDC_AOP_SEQ_RESUME_ONCE_0_RESUME_SHUTDOWN_SEQ_ONCE_FVAL        0x1

#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_ADDR                                                (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000300)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_RMSK                                                     0x3ff
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_ADDR, HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_RMSK)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_ADDR,m,v,HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_IN)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_MODEM_SOC_SLEEP_LOCK_MASK_BMSK                           0x200
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_MODEM_SOC_SLEEP_LOCK_MASK_SHFT                             0x9
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_MODEM_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_MODEM_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_COMPUTE_SOC_SLEEP_LOCK_MASK_BMSK                         0x100
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_COMPUTE_SOC_SLEEP_LOCK_MASK_SHFT                           0x8
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_COMPUTE_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_COMPUTE_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DISPLAY_SOC_SLEEP_LOCK_MASK_BMSK                          0x80
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DISPLAY_SOC_SLEEP_LOCK_MASK_SHFT                           0x7
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DISPLAY_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DISPLAY_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_GPU_SOC_SLEEP_LOCK_MASK_BMSK                              0x40
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_GPU_SOC_SLEEP_LOCK_MASK_SHFT                               0x6
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_GPU_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_GPU_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DEBUG_SOC_SLEEP_LOCK_MASK_BMSK                            0x20
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DEBUG_SOC_SLEEP_LOCK_MASK_SHFT                             0x5
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DEBUG_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_DEBUG_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AOP_SOC_SLEEP_LOCK_MASK_BMSK                              0x10
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AOP_SOC_SLEEP_LOCK_MASK_SHFT                               0x4
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AOP_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AOP_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SENSORS_SOC_SLEEP_LOCK_MASK_BMSK                           0x8
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SENSORS_SOC_SLEEP_LOCK_MASK_SHFT                           0x3
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SENSORS_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SENSORS_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AUDIO_SOC_SLEEP_LOCK_MASK_BMSK                             0x4
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AUDIO_SOC_SLEEP_LOCK_MASK_SHFT                             0x2
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AUDIO_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_AUDIO_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SP_SOC_SLEEP_LOCK_MASK_BMSK                                0x2
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SP_SOC_SLEEP_LOCK_MASK_SHFT                                0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SP_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_SP_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_APPS_SOC_SLEEP_LOCK_MASK_BMSK                              0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_APPS_SOC_SLEEP_LOCK_MASK_SHFT                              0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_APPS_SOC_SLEEP_LOCK_MASK_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0_APPS_SOC_SLEEP_LOCK_MASK_DO_NOT_CONSIDER_FOR_SLEEP_LOCK_FVAL        0x1

#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_ADDR                                              (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000310)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_RMSK                                                   0x3ff
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_MODEM_SOC_SLEEP_LOCK_STATUS_BMSK                       0x200
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_MODEM_SOC_SLEEP_LOCK_STATUS_SHFT                         0x9
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_MODEM_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_MODEM_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_COMPUTE_SOC_SLEEP_LOCK_STATUS_BMSK                     0x100
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_COMPUTE_SOC_SLEEP_LOCK_STATUS_SHFT                       0x8
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_COMPUTE_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_COMPUTE_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DISPLAY_SOC_SLEEP_LOCK_STATUS_BMSK                      0x80
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DISPLAY_SOC_SLEEP_LOCK_STATUS_SHFT                       0x7
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DISPLAY_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DISPLAY_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_GPU_SOC_SLEEP_LOCK_STATUS_BMSK                          0x40
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_GPU_SOC_SLEEP_LOCK_STATUS_SHFT                           0x6
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_GPU_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_GPU_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DEBUG_SOC_SLEEP_LOCK_STATUS_BMSK                        0x20
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DEBUG_SOC_SLEEP_LOCK_STATUS_SHFT                         0x5
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DEBUG_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_DEBUG_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AOP_SOC_SLEEP_LOCK_STATUS_BMSK                          0x10
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AOP_SOC_SLEEP_LOCK_STATUS_SHFT                           0x4
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AOP_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AOP_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SENSORS_SOC_SLEEP_LOCK_STATUS_BMSK                       0x8
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SENSORS_SOC_SLEEP_LOCK_STATUS_SHFT                       0x3
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SENSORS_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SENSORS_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AUDIO_SOC_SLEEP_LOCK_STATUS_BMSK                         0x4
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AUDIO_SOC_SLEEP_LOCK_STATUS_SHFT                         0x2
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AUDIO_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_AUDIO_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SP_SOC_SLEEP_LOCK_STATUS_BMSK                            0x2
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SP_SOC_SLEEP_LOCK_STATUS_SHFT                            0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SP_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_SP_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_APPS_SOC_SLEEP_LOCK_STATUS_BMSK                          0x1
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_APPS_SOC_SLEEP_LOCK_STATUS_SHFT                          0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_APPS_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_SOC_SLEEP_LOCK_STATUS_BANK0_APPS_SOC_SLEEP_LOCK_STATUS_SUBSYSTEM_SLEEP_LOCKED_FVAL        0x1

#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_ADDR                                              (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000320)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_RMSK                                                   0x3ff
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_ADDR, HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_RMSK)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_ADDR,m,v,HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_IN)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_MODEM_CX_COLLAPSE_LOCK_MASK_BMSK                       0x200
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_MODEM_CX_COLLAPSE_LOCK_MASK_SHFT                         0x9
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_MODEM_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_MODEM_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_COMPUTE_CX_COLLAPSE_LOCK_MASK_BMSK                     0x100
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_COMPUTE_CX_COLLAPSE_LOCK_MASK_SHFT                       0x8
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_COMPUTE_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_COMPUTE_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DISPLAY_CX_COLLAPSE_LOCK_MASK_BMSK                      0x80
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DISPLAY_CX_COLLAPSE_LOCK_MASK_SHFT                       0x7
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DISPLAY_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DISPLAY_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_GPU_CX_COLLAPSE_LOCK_MASK_BMSK                          0x40
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_GPU_CX_COLLAPSE_LOCK_MASK_SHFT                           0x6
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_GPU_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_GPU_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DEBUG_CX_COLLAPSE_LOCK_MASK_BMSK                        0x20
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DEBUG_CX_COLLAPSE_LOCK_MASK_SHFT                         0x5
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DEBUG_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_DEBUG_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AOP_CX_COLLAPSE_LOCK_MASK_BMSK                          0x10
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AOP_CX_COLLAPSE_LOCK_MASK_SHFT                           0x4
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AOP_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AOP_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SENSORS_CX_COLLAPSE_LOCK_MASK_BMSK                       0x8
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SENSORS_CX_COLLAPSE_LOCK_MASK_SHFT                       0x3
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SENSORS_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SENSORS_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AUDIO_CX_COLLAPSE_LOCK_MASK_BMSK                         0x4
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AUDIO_CX_COLLAPSE_LOCK_MASK_SHFT                         0x2
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AUDIO_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_AUDIO_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SP_CX_COLLAPSE_LOCK_MASK_BMSK                            0x2
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SP_CX_COLLAPSE_LOCK_MASK_SHFT                            0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SP_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_SP_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_APPS_CX_COLLAPSE_LOCK_MASK_BMSK                          0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_APPS_CX_COLLAPSE_LOCK_MASK_SHFT                          0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_APPS_CX_COLLAPSE_LOCK_MASK_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0_APPS_CX_COLLAPSE_LOCK_MASK_DO_NOT_CONSIDER_FOR_CX_COLLAPSE_LOCK_FVAL        0x1

#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_ADDR                                            (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000330)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_RMSK                                                 0x3ff
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_MODEM_CX_COLLAPSE_LOCK_STATUS_BMSK                   0x200
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_MODEM_CX_COLLAPSE_LOCK_STATUS_SHFT                     0x9
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_MODEM_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_MODEM_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_COMPUTE_CX_COLLAPSE_LOCK_STATUS_BMSK                 0x100
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_COMPUTE_CX_COLLAPSE_LOCK_STATUS_SHFT                   0x8
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_COMPUTE_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_COMPUTE_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DISPLAY_CX_COLLAPSE_LOCK_STATUS_BMSK                  0x80
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DISPLAY_CX_COLLAPSE_LOCK_STATUS_SHFT                   0x7
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DISPLAY_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DISPLAY_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_GPU_CX_COLLAPSE_LOCK_STATUS_BMSK                      0x40
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_GPU_CX_COLLAPSE_LOCK_STATUS_SHFT                       0x6
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_GPU_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_GPU_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DEBUG_CX_COLLAPSE_LOCK_STATUS_BMSK                    0x20
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DEBUG_CX_COLLAPSE_LOCK_STATUS_SHFT                     0x5
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DEBUG_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_DEBUG_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AOP_CX_COLLAPSE_LOCK_STATUS_BMSK                      0x10
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AOP_CX_COLLAPSE_LOCK_STATUS_SHFT                       0x4
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AOP_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AOP_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SENSORS_CX_COLLAPSE_LOCK_STATUS_BMSK                   0x8
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SENSORS_CX_COLLAPSE_LOCK_STATUS_SHFT                   0x3
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SENSORS_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SENSORS_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AUDIO_CX_COLLAPSE_LOCK_STATUS_BMSK                     0x4
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AUDIO_CX_COLLAPSE_LOCK_STATUS_SHFT                     0x2
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AUDIO_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_AUDIO_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SP_CX_COLLAPSE_LOCK_STATUS_BMSK                        0x2
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SP_CX_COLLAPSE_LOCK_STATUS_SHFT                        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SP_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_SP_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_APPS_CX_COLLAPSE_LOCK_STATUS_BMSK                      0x1
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_APPS_CX_COLLAPSE_LOCK_STATUS_SHFT                      0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_APPS_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_CX_COLLAPSE_LOCK_STATUS_BANK0_APPS_CX_COLLAPSE_LOCK_STATUS_SUBSYSTEM_CX_COLLAPSE_LOCKED_FVAL        0x1

#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_ADDR                                             (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000340)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_RMSK                                                  0x3ff
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_ADDR, HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_RMSK)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_ADDR,v)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_ADDR,m,v,HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_IN)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_MODEM_DBG_RSRC_REL_LOCK_MASK_BMSK                     0x200
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_MODEM_DBG_RSRC_REL_LOCK_MASK_SHFT                       0x9
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_MODEM_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_MODEM_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_MASK_BMSK                   0x100
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_MASK_SHFT                     0x8
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_MASK_BMSK                    0x80
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_MASK_SHFT                     0x7
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_GPU_DBG_RSRC_REL_LOCK_MASK_BMSK                        0x40
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_GPU_DBG_RSRC_REL_LOCK_MASK_SHFT                         0x6
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_GPU_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_GPU_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DEBUG_DBG_RSRC_REL_LOCK_MASK_BMSK                      0x20
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DEBUG_DBG_RSRC_REL_LOCK_MASK_SHFT                       0x5
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DEBUG_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_DEBUG_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AOP_DBG_RSRC_REL_LOCK_MASK_BMSK                        0x10
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AOP_DBG_RSRC_REL_LOCK_MASK_SHFT                         0x4
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AOP_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AOP_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SENSORS_DBG_RSRC_REL_LOCK_MASK_BMSK                     0x8
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SENSORS_DBG_RSRC_REL_LOCK_MASK_SHFT                     0x3
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SENSORS_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SENSORS_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AUDIO_DBG_RSRC_REL_LOCK_MASK_BMSK                       0x4
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AUDIO_DBG_RSRC_REL_LOCK_MASK_SHFT                       0x2
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AUDIO_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AUDIO_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SP_DBG_RSRC_REL_LOCK_MASK_BMSK                          0x2
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SP_DBG_RSRC_REL_LOCK_MASK_SHFT                          0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SP_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SP_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_APPS_DBG_RSRC_REL_LOCK_MASK_BMSK                        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_APPS_DBG_RSRC_REL_LOCK_MASK_SHFT                        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_APPS_DBG_RSRC_REL_LOCK_MASK_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_APPS_DBG_RSRC_REL_LOCK_MASK_DO_NOT_CONSIDER_FOR_DEBUG_RESOURCE_RELEASE_LOCK_FVAL        0x1

#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_ADDR                                           (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000350)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_RMSK                                                0x3ff
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_ADDR, HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_RMSK)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_ADDR, m)
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_MODEM_DBG_RSRC_REL_LOCK_STATUS_BMSK                 0x200
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_MODEM_DBG_RSRC_REL_LOCK_STATUS_SHFT                   0x9
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_MODEM_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_MODEM_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_STATUS_BMSK               0x100
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_STATUS_SHFT                 0x8
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_STATUS_BMSK                0x80
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_STATUS_SHFT                 0x7
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DISPLAY_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_GPU_DBG_RSRC_REL_LOCK_STATUS_BMSK                    0x40
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_GPU_DBG_RSRC_REL_LOCK_STATUS_SHFT                     0x6
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_GPU_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_GPU_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DEBUG_DBG_RSRC_REL_LOCK_STATUS_BMSK                  0x20
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DEBUG_DBG_RSRC_REL_LOCK_STATUS_SHFT                   0x5
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DEBUG_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_DEBUG_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AOP_DBG_RSRC_REL_LOCK_STATUS_BMSK                    0x10
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AOP_DBG_RSRC_REL_LOCK_STATUS_SHFT                     0x4
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AOP_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AOP_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SENSORS_DBG_RSRC_REL_LOCK_STATUS_BMSK                 0x8
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SENSORS_DBG_RSRC_REL_LOCK_STATUS_SHFT                 0x3
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SENSORS_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SENSORS_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AUDIO_DBG_RSRC_REL_LOCK_STATUS_BMSK                   0x4
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AUDIO_DBG_RSRC_REL_LOCK_STATUS_SHFT                   0x2
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AUDIO_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_AUDIO_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SP_DBG_RSRC_REL_LOCK_STATUS_BMSK                      0x2
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SP_DBG_RSRC_REL_LOCK_STATUS_SHFT                      0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SP_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_SP_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_APPS_DBG_RSRC_REL_LOCK_STATUS_BMSK                    0x1
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_APPS_DBG_RSRC_REL_LOCK_STATUS_SHFT                    0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_APPS_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_UNLOCKED_FVAL        0x0
#define HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_STATUS_BANK0_APPS_DBG_RSRC_REL_LOCK_STATUS_SUBSYSTEM_DEBUG_RESOURCE_RELEASE_LOCKED_FVAL        0x1

#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_ADDR                                                          (RPMH_PDC_PDC_GLOBAL_REG_BASE      + 0x00000400)
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_RMSK                                                                0x1f
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_IN          \
        in_dword_masked(HWIO_RPMH_PDC_DBG_BUS_CONTROL_ADDR, HWIO_RPMH_PDC_DBG_BUS_CONTROL_RMSK)
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_INM(m)      \
        in_dword_masked(HWIO_RPMH_PDC_DBG_BUS_CONTROL_ADDR, m)
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_OUT(v)      \
        out_dword(HWIO_RPMH_PDC_DBG_BUS_CONTROL_ADDR,v)
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_PDC_DBG_BUS_CONTROL_ADDR,m,v,HWIO_RPMH_PDC_DBG_BUS_CONTROL_IN)
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_DBG_BUS_CONTROL_BMSK                                                0x1e
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_DBG_BUS_CONTROL_SHFT                                                 0x1
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_DBG_BUS_ENABLE_BMSK                                                  0x1
#define HWIO_RPMH_PDC_DBG_BUS_CONTROL_DBG_BUS_ENABLE_SHFT                                                  0x0


#endif /* __PDC_GLOBAL_HWIO_H__ */
