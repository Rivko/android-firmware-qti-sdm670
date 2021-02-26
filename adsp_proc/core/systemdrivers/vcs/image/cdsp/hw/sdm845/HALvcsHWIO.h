#ifndef __HALVCSHWIO_H__
#define __HALVCSHWIO_H__
/*
===========================================================================
*/
/**
  @file HALvcsHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TURING_QDSP6V65SS_PUB

  'Include' filters applied: TURING_QDSP6SS_VERSION 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/image/cdsp/hw/sdm845/HALvcsHWIO.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"



/*----------------------------------------------------------------------------
 * MODULE: TURING_QDSP6V65SS_PUB
 *--------------------------------------------------------------------------*/

#define TURING_QDSP6V65SS_PUB_REG_BASE                                                 (TURING_BASE      + 0x00300000)
#define TURING_QDSP6V65SS_PUB_REG_BASE_OFFS                                            0x00300000

#define HWIO_TURING_QDSP6SS_VERSION_ADDR                                               (TURING_QDSP6V65SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_TURING_QDSP6SS_VERSION_OFFS                                               (TURING_QDSP6V65SS_PUB_REG_BASE_OFFS + 0x00000000)
#define HWIO_TURING_QDSP6SS_VERSION_RMSK                                               0xffffffff
#define HWIO_TURING_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_VERSION_ADDR, HWIO_TURING_QDSP6SS_VERSION_RMSK)
#define HWIO_TURING_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_VERSION_ADDR, m)
#define HWIO_TURING_QDSP6SS_VERSION_MAJOR_BMSK                                         0xf0000000
#define HWIO_TURING_QDSP6SS_VERSION_MAJOR_SHFT                                               0x1c
#define HWIO_TURING_QDSP6SS_VERSION_MINOR_BMSK                                          0xfff0000
#define HWIO_TURING_QDSP6SS_VERSION_MINOR_SHFT                                               0x10
#define HWIO_TURING_QDSP6SS_VERSION_STEP_BMSK                                              0xffff
#define HWIO_TURING_QDSP6SS_VERSION_STEP_SHFT                                                 0x0


#endif /* __HALVCSHWIO_H__ */
