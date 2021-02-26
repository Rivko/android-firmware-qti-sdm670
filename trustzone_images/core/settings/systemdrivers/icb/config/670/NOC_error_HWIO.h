#ifndef __NOC_ERROR_HWIO_H__
#define __NOC_ERROR_HWIO_H__
/*
===========================================================================
*/
/**
  @file NOC_error_HWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r89]
 
  This file contains HWIO register definitions for the following modules:
    AGGRE_NOC_A1NOC_SERVICE_NETWORK
    AGGRE_NOC_A2NOC_SERVICE_NETWORK
    CNOC_SERVICE_NETWORK
    DC_NOC_DC_NOC
    MEMNOC_MEM_NOC
    MMSS_NOC
    SYSTEM_NOC
    GCC_CLK_CTL_REG

  'Include' filters applied: SBM_SWID_LOW[MEMNOC_MEM_NOC] ERRORLOGGER_SWID_LOW[MEMNOC_MEM_NOC] SBM_FLAGOUTSET0[MEMNOC_MEM_NOC] SBM_SWID_LOW[MMSS_NOC] ERRLOG_SWID_LOW[MMSS_NOC] SBM_FLAGOUTSET0[MMSS_NOC] SIDEBANDMANAGER_SWID_LOW[DC_NOC_DC_NOC] ERRORLOGGER_SWID_LOW[DC_NOC_DC_NOC] SBM_FLAGOUTSET0[DC_NOC_DC_NOC] SIDEBANDMANAGER_FLAGOUTSET0_LOW[DC_NOC_DC_NOC] SBM_SWID_LOW[CNOC_SERVICE_NETWORK] OBS_SWID_LOW[CNOC_SERVICE_NETWORK] SBM_FLAGOUTSET0[CNOC_SERVICE_NETWORK] SBM_SWID_LOW[SYSTEM_NOC] ERRLOG_SWID_LOW[SYSTEM_NOC] SBM_FLAGOUTSET0[SYSTEM_NOC] OBS_SWID_LOW[SYSTEM_NOC] SBM_SWID_LOW[AGGRE_NOC_A1NOC_SERVICE_NETWORK] ERRLOG_SWID_LOW[AGGRE_NOC_A1NOC_SERVICE_NETWORK] SBM_FLAGOUTSET0[AGGRE_NOC_A1NOC_SERVICE_NETWORK] GCC_NOC_BUS_TIMEOUT_EXTREF_CBCR[GCC_CLK_CTL_REG] GCC_NOC_BUS_TIMEOUT_EXTREF_CDIVR[GCC_CLK_CTL_REG] SBM_FLAGOUTSET0[GCC_CLK_CTL_REG] SBM_SWID_LOW[AGGRE_NOC_A2NOC_SERVICE_NETWORK] ERRLOG_SWID_LOW[AGGRE_NOC_A2NOC_SERVICE_NETWORK] SBM_FLAGOUTSET0[AGGRE_NOC_A2NOC_SERVICE_NETWORK] 
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

  $Header: //components/rel/core.tz/2.0.2/settings/systemdrivers/icb/config/670/NOC_error_HWIO.h#1 $
  $DateTime: 2018/02/06 02:57:17 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: AGGRE_NOC_A1NOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE                                                            (AGGRE_NOC_AGGRE_NOC_BASE      + 0x00000000)

#define HWIO_AGGRE_NOC_A1NOC_SBM_SWID_LOW_ADDR                                                              (AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE      + 0x00000200)
#define HWIO_AGGRE_NOC_A1NOC_ERRLOG_SWID_LOW_ADDR                                                           (AGGRE_NOC_A1NOC_SERVICE_NETWORK_REG_BASE      + 0x00000400)

/*----------------------------------------------------------------------------
 * MODULE: AGGRE_NOC_A2NOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE                                                      (AGGRE_NOC_AGGRE_NOC_BASE      + 0x00020000)

#define HWIO_AGGRE_NOC_A2NOC_SBM_SWID_LOW_ADDR                                                        (AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE      + 0x00000200)
#define HWIO_AGGRE_NOC_A2NOC_ERRLOG_SWID_LOW_ADDR                                                     (AGGRE_NOC_A2NOC_SERVICE_NETWORK_REG_BASE      + 0x00000c00)

/*----------------------------------------------------------------------------
 * MODULE: CNOC_SERVICE_NETWORK
 *--------------------------------------------------------------------------*/

#define CNOC_SERVICE_NETWORK_REG_BASE                                           (CONFIG_NOC_WRAPPER_BASE      + 0x00000000)

#define HWIO_CNOC_OBS_SWID_LOW_ADDR                                             (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00000000)
#define HWIO_CNOC_SBM_SWID_LOW_ADDR                                             (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00000200)
#define HWIO_CNOC_SBM_FLAGOUTSET0_LOW_ADDR                                      (CNOC_SERVICE_NETWORK_REG_BASE      + 0x00000288)

/*----------------------------------------------------------------------------
 * MODULE: DC_NOC_DC_NOC
 *--------------------------------------------------------------------------*/

#define DC_NOC_DC_NOC_REG_BASE                                                               (DDR_SS_BASE      + 0x003e0000)

#define HWIO_DC_NOC_ERL_ERRORLOGGER_MAIN_ERRORLOGGER_SWID_LOW_ADDR                           (DC_NOC_DC_NOC_REG_BASE      + 0x00000000)
#define HWIO_DC_NOC_DC_NOC_SBM_MAIN_SIDEBANDMANAGER_SWID_LOW_ADDR                            (DC_NOC_DC_NOC_REG_BASE      + 0x00000200)
#define HWIO_DC_NOC_DC_NOC_SBM_MAIN_SIDEBANDMANAGER_FLAGOUTSET0_LOW_ADDR                     (DC_NOC_DC_NOC_REG_BASE      + 0x00000288)

/*----------------------------------------------------------------------------
 * MODULE: MEMNOC_MEM_NOC
 *--------------------------------------------------------------------------*/

#define MEMNOC_MEM_NOC_REG_BASE                                                                             (DDR_SS_BASE      + 0x00280000)

#define HWIO_MEMNOC_MEM_NOC_ERRORLOGGER_SWID_LOW_ADDR                                                       (MEMNOC_MEM_NOC_REG_BASE      + 0x00000000)
#define HWIO_MEMNOC_MEM_NOC_SBM_SWID_LOW_ADDR                                                               (MEMNOC_MEM_NOC_REG_BASE      + 0x00000400)

/*----------------------------------------------------------------------------
 * MODULE: MMSS_NOC
 *--------------------------------------------------------------------------*/

#define MMSS_NOC_REG_BASE                                                                                (MMSS_NOC_BASE      + 0x00000000)

#define HWIO_MNOC_ERRLOG_SWID_LOW_ADDR                                                                   (MMSS_NOC_REG_BASE      + 0x00000000)
#define HWIO_MNOC_SBM_SWID_LOW_ADDR                                                                      (MMSS_NOC_REG_BASE      + 0x00000200)

/*----------------------------------------------------------------------------
 * MODULE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_REG_BASE                                                                   (SYSTEM_NOC_BASE      + 0x00000000)

#define HWIO_SNOC_OBS_SWID_LOW_ADDR                                                           (SYSTEM_NOC_REG_BASE      + 0x00000000)
#define HWIO_SNOC_CENTER_SBM_SWID_LOW_ADDR                                                    (SYSTEM_NOC_REG_BASE      + 0x00000200)
#define HWIO_SNOC_CENTER_SBM_FLAGOUTSET0_LOW_ADDR                                             (SYSTEM_NOC_REG_BASE      + 0x00000288)

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                   (CLK_CTL_BASE      + 0x00000000)

#define HWIO_GCC_NOC_BUS_TIMEOUT_EXTREF_CBCR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00049004)
#define HWIO_GCC_NOC_BUS_TIMEOUT_EXTREF_CDIVR_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00049008)

#endif /* __NOC_ERROR_HWIO_H__ */
