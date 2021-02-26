#ifndef __CLOCKNOCSDM670V1_H__
#define __CLOCKNOCSDM670V1_H__
/*
===========================================================================
*/
/**
  @file ClockNOCSDM670v1.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r103_MTO]
 
  This file contains HWIO register definitions for the following modules:
    AGGRE_NOC_A1NOC_SERVICE_NETWORK
    AGGRE_NOC_A2NOC_SERVICE_NETWORK
    CNOC_SERVICE_NETWORK
    MMSS_NOC
    SYSTEM_NOC

  'Include' filters applied: MAXDIV_LOW[SYSTEM_NOC] MAXDIV_LOW[AGGRE_NOC_A1NOC_SERVICE_NETWORK] MAXDIV_LOW[MMSS_NOC] MAXDIV_LOW[CNOC_SERVICE_NETWORK] MAXDIV_LOW[AGGRE_NOC_A2NOC_SERVICE_NETWORK] 

  Generation parameters: 
  { u'filename': u'ClockNOCSDM670v1.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'AGGRE_NOC_A1NOC_SERVICE_NETWORK': [ u'MAXDIV_LOW'],
                                u'AGGRE_NOC_A2NOC_SERVICE_NETWORK': [ u'MAXDIV_LOW'],
                                u'CNOC_SERVICE_NETWORK': [u'MAXDIV_LOW'],
                                u'MMSS_NOC': [u'MAXDIV_LOW'],
                                u'SYSTEM_NOC': [u'MAXDIV_LOW']},
    u'modules': [ u'AGGRE_NOC_A1NOC_SERVICE_NETWORK',
                  u'AGGRE_NOC_A2NOC_SERVICE_NETWORK',
                  u'CNOC_SERVICE_NETWORK',
                  u'MMSS_NOC',
                  u'SYSTEM_NOC']}
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockNOCSDM670.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: AGGRE_NOC_A1NOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE                                                            (AGGRE_NOC_AGGRE_NOC_BASE      + 0x00000000)

#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_ADDR                                                            (AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE      + 0x00013008)
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_RMSK                                                                   0x7
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_ADDR, HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_RMSK)
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_ADDR, m)
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_ADDR,v)
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_ADDR,m,v,HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_IN)
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_MAXDIV_BMSK                                                            0x7
#define HWIO_AGGRE_NOC_A1NOC_DCD_MAXDIV_LOW_MAXDIV_SHFT                                                            0x0

#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_ADDR                                                       (AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE      + 0x0001f008)
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_RMSK                                                              0x7
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_ADDR, HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_RMSK)
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_ADDR, m)
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_ADDR,v)
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_ADDR,m,v,HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_IN)
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_MAXDIV_BMSK                                                       0x7
#define HWIO_AGGRE_NOC_A1NOC_DCD_EAST_MAXDIV_LOW_MAXDIV_SHFT                                                       0x0

/*----------------------------------------------------------------------------
 * MODULE: AGGRE_NOC_A2NOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE                                                      (AGGRE_NOC_AGGRE_NOC_BASE      + 0x00020000)

#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_ADDR                                                (AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE      + 0x0001f088)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_RMSK                                                       0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_ADDR, HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_RMSK)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_ADDR, m)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_ADDR,v)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_ADDR,m,v,HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_IN)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_MAXDIV_BMSK                                                0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_MAXDIV_LOW_MAXDIV_SHFT                                                0x0

#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_ADDR                                                (AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE      + 0x0001f188)
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_RMSK                                                       0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_ADDR, HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_RMSK)
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_ADDR, m)
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_ADDR,v)
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_ADDR,m,v,HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_IN)
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_MAXDIV_BMSK                                                0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_MONAQ_MAXDIV_LOW_MAXDIV_SHFT                                                0x0

#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_ADDR                                             (AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE      + 0x0001f208)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_RMSK                                                    0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_ADDR, HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_RMSK)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_ADDR, m)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_ADDR,v)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_ADDR,m,v,HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_IN)
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_MAXDIV_BMSK                                             0x7
#define HWIO_AGGRE_NOC_A2NOC_DCD_NORTH_SF_MAXDIV_LOW_MAXDIV_SHFT                                             0x0

/*----------------------------------------------------------------------------
 * MODULE: CNOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define CNOC_SERVICE_NETWORK_REG_BASE                                           (CONFIG_NOC_WRAPPER_BASE      + 0x00000000)

#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_ADDR                              (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002008)
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_RMSK                                     0x7
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                              0x7
#define HWIO_CNOC_DCD_CENTER_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                              0x0

#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_ADDR                                (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002088)
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_RMSK                                       0x7
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                                0x7
#define HWIO_CNOC_DCD_EAST_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                                0x0

#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_ADDR                               (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002108)
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_RMSK                                      0x7
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                               0x7
#define HWIO_CNOC_DCD_MMNOC_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                               0x0

#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_ADDR                               (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002188)
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_RMSK                                      0x7
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                               0x7
#define HWIO_CNOC_DCD_MONAQ_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                               0x0

#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_ADDR                               (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002208)
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_RMSK                                      0x7
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                               0x7
#define HWIO_CNOC_DCD_NORTH_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                               0x0

#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_ADDR                               (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002288)
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_RMSK                                      0x7
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                               0x7
#define HWIO_CNOC_DCD_SOUTH_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                               0x0

#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_ADDR                                (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00002308)
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_RMSK                                       0x7
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_ADDR, HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_RMSK)
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_ADDR,m,v,HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_IN)
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_MAXDIV_BMSK                                0x7
#define HWIO_CNOC_DCD_WEST_QHCLK_MAXDIV_LOW_MAXDIV_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: MMSS_NOC
 *--------------------------------------------------------------------------*/

#define MMSS_NOC_REG_BASE                                                                                (MMSS_NOC_BASE      + 0x00000000)

#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_ADDR                                                           (MMSS_NOC_REG_BASE      + 0x0001c008)
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_RMSK                                                                  0x7
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_ADDR, HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_RMSK)
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_ADDR,m,v,HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_IN)
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_MAXDIV_BMSK                                                           0x7
#define HWIO_MNOC_DCD_HF_QXCLK_MAXDIV_LOW_MAXDIV_SHFT                                                           0x0

#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_ADDR                                                           (MMSS_NOC_REG_BASE      + 0x0001c088)
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_RMSK                                                                  0x7
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_ADDR, HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_RMSK)
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_ADDR, m)
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_ADDR,v)
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_ADDR,m,v,HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_IN)
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_MAXDIV_BMSK                                                           0x7
#define HWIO_MNOC_DCD_SF_QXCLK_MAXDIV_LOW_MAXDIV_SHFT                                                           0x0

/*----------------------------------------------------------------------------
 * MODULE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_REG_BASE                                                                   (SYSTEM_NOC_BASE      + 0x00000000)

#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_ADDR                                                  (SYSTEM_NOC_REG_BASE      + 0x0000d008)
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_RMSK                                                         0x7
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_MAXDIV_BMSK                                                  0x7
#define HWIO_SNOC_DCD_QX_CLK_MAXDIV_LOW_MAXDIV_SHFT                                                  0x0

#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_ADDR                                               (SYSTEM_NOC_REG_BASE      + 0x0000d088)
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_RMSK                                                      0x7
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_MAXDIV_BMSK                                               0x7
#define HWIO_SNOC_DCD_HS_QX_CLK_MAXDIV_LOW_MAXDIV_SHFT                                               0x0

#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_ADDR                                               (SYSTEM_NOC_REG_BASE      + 0x0000d108)
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_RMSK                                                      0x7
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_MAXDIV_BMSK                                               0x7
#define HWIO_SNOC_DCD_PIMEM_CLK_MAXDIV_LOW_MAXDIV_SHFT                                               0x0

#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_ADDR                                            (SYSTEM_NOC_REG_BASE      + 0x0000d188)
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_RMSK                                                   0x7
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_MAXDIV_BMSK                                            0x7
#define HWIO_SNOC_DCD_SOUTH_QX_CLK_MAXDIV_LOW_MAXDIV_SHFT                                            0x0

#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_ADDR                                            (SYSTEM_NOC_REG_BASE      + 0x0000d288)
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_RMSK                                                   0x7
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_MAXDIV_BMSK                                            0x7
#define HWIO_SNOC_DCD_NORTH_QX_CLK_MAXDIV_LOW_MAXDIV_SHFT                                            0x0

#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_ADDR                                            (SYSTEM_NOC_REG_BASE      + 0x0000d308)
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_RMSK                                                   0x7
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_IN          \
        in_dword_masked(HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_ADDR, HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_RMSK)
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_INM(m)      \
        in_dword_masked(HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_ADDR, m)
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_OUT(v)      \
        out_dword(HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_ADDR,v)
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_ADDR,m,v,HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_IN)
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_MAXDIV_BMSK                                            0x7
#define HWIO_SNOC_DCD_MONAQ_QX_CLK_MAXDIV_LOW_MAXDIV_SHFT                                            0x0


#endif /* __CLOCKNOCSDM670V1_H__ */
