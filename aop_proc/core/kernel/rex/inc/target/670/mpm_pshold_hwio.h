#ifndef __MPM_PSHOLD_HWIO_H__
#define __MPM_PSHOLD_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpm_pshold_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r81]
 
  This file contains HWIO register definitions for the following modules:
    PSHOLD

  'Include' filters applied: 
  'Exclude' filters applied: 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/target/670/mpm_pshold_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: PSHOLD
 *--------------------------------------------------------------------------*/

#define PSHOLD_REG_BASE                       (AOSS_BASE      + 0x01264000)
#define PSHOLD_REG_BASE_OFFS                  0x01264000

#define HWIO_MPM_PS_HOLD_ADDR                 (PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM_PS_HOLD_OFFS                 (PSHOLD_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM_PS_HOLD_ADDR, HWIO_MPM_PS_HOLD_RMSK)
#define HWIO_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM_PS_HOLD_IN)
#define HWIO_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM_PS_HOLD_PSHOLD_SHFT                 0x0


#endif /* __MPM_PSHOLD_HWIO_H__ */
