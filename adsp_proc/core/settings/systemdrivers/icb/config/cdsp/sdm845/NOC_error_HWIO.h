#ifndef __NOC_ERROR_HWIO_H__
#define __NOC_ERROR_HWIO_H__
/*
===========================================================================
*/
/**
  @file NOC_error_HWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.4_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    TURING_CDSP_NOC
    TURING_VAPSS_NOC

  'Include' filters applied: ERRLOG_SWID_LOW[TURING_CDSP_NOC] SB_SWID_LOW[TURING_CDSP_NOC] ERRLOG_SWID_LOW[TURING_VAPSS_NOC] SB_SWID_LOW[TURING_VAPSS_NOC] 
  'Exclude' filters applied: 
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

  $Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/cdsp/sdm845/NOC_error_HWIO.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: TURING_CDSP_NOC
 *--------------------------------------------------------------------------*/

#define TURING_CDSP_NOC_REG_BASE                                             (TURING_BASE      + 0x000a8000)

#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_ADDR                                (TURING_CDSP_NOC_REG_BASE      + 0x00001000)
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_RMSK                                  0xffffff
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_IN          \
        in_dword_masked(HWIO_TURING_CDSP_ERRLOG_SWID_LOW_ADDR, HWIO_TURING_CDSP_ERRLOG_SWID_LOW_RMSK)
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_TURING_CDSP_ERRLOG_SWID_LOW_ADDR, m)
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_UNITTYPEID_BMSK                       0xff0000
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_UNITTYPEID_SHFT                           0x10
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_UNITCONFID_BMSK                         0xffff
#define HWIO_TURING_CDSP_ERRLOG_SWID_LOW_UNITCONFID_SHFT                            0x0

#define HWIO_TURING_CDSP_SB_SWID_LOW_ADDR                                    (TURING_CDSP_NOC_REG_BASE      + 0x00001200)
#define HWIO_TURING_CDSP_SB_SWID_LOW_RMSK                                      0xffffff
#define HWIO_TURING_CDSP_SB_SWID_LOW_IN          \
        in_dword_masked(HWIO_TURING_CDSP_SB_SWID_LOW_ADDR, HWIO_TURING_CDSP_SB_SWID_LOW_RMSK)
#define HWIO_TURING_CDSP_SB_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_TURING_CDSP_SB_SWID_LOW_ADDR, m)
#define HWIO_TURING_CDSP_SB_SWID_LOW_UNITTYPEID_BMSK                           0xff0000
#define HWIO_TURING_CDSP_SB_SWID_LOW_UNITTYPEID_SHFT                               0x10
#define HWIO_TURING_CDSP_SB_SWID_LOW_UNITCONFID_BMSK                             0xffff
#define HWIO_TURING_CDSP_SB_SWID_LOW_UNITCONFID_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: TURING_VAPSS_NOC
 *--------------------------------------------------------------------------*/

#define TURING_VAPSS_NOC_REG_BASE                                                   (TURING_BASE      + 0x002d0000)

#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_ADDR                            (TURING_VAPSS_NOC_REG_BASE      + 0x00001000)
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_RMSK                              0xffffff
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_ADDR, HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_RMSK)
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_ADDR, m)
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_UNITTYPEID_BMSK                   0xff0000
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_UNITTYPEID_SHFT                       0x10
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_UNITCONFID_BMSK                     0xffff
#define HWIO_TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW_UNITCONFID_SHFT                        0x0

#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_ADDR                                (TURING_VAPSS_NOC_REG_BASE      + 0x00001400)
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_RMSK                                  0xffffff
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_IN          \
        in_dword_masked(HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_ADDR, HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_RMSK)
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_INM(m)      \
        in_dword_masked(HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_ADDR, m)
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_UNITTYPEID_BMSK                       0xff0000
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_UNITTYPEID_SHFT                           0x10
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_UNITCONFID_BMSK                         0xffff
#define HWIO_TURING_VAPSS_NOC_VAPSS_SB_SWID_LOW_UNITCONFID_SHFT                            0x0


#endif /* __NOC_ERROR_HWIO_H__ */
