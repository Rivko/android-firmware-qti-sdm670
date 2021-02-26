#ifndef __PMIO_PON_H__
#define __PMIO_PON_H__
/*
===========================================================================
*/
/**
  @file pmio_pon.h
  @brief Auto-generated PMIO interface include file.

  This file contains PMIO register definitions for the following modules:
    PON

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 - 2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================


  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * PERIPHERAL: PON
 *--------------------------------------------------------------------------*/

#define PON_BASE                                                          0x00000800

#define PMIO_PON_REVISION1_ADDR                                           (PON_BASE      + 0x00000000)
#define PMIO_PON_REVISION1_OFFS                                           0x00000000
#define PMIO_PON_REVISION1_RMSK                                                 0xff
#define PMIO_PON_REVISION1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION1_ADDR, PMIO_PON_REVISION1_RMSK, 0, val)
#define PMIO_PON_REVISION1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION1_ADDR, mask, shift, val)
#define PMIO_PON_REVISION1_DIG_MINOR_BMSK                                       0xff
#define PMIO_PON_REVISION1_DIG_MINOR_SHFT                                        0x0

#define PMIO_PON_REVISION2_ADDR                                           (PON_BASE      + 0x00000001)
#define PMIO_PON_REVISION2_OFFS                                           0x00000001
#define PMIO_PON_REVISION2_RMSK                                                 0xff
#define PMIO_PON_REVISION2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION2_ADDR, PMIO_PON_REVISION2_RMSK, 0, val)
#define PMIO_PON_REVISION2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION2_ADDR, mask, shift, val)
#define PMIO_PON_REVISION2_DIG_MAJOR_BMSK                                       0xff
#define PMIO_PON_REVISION2_DIG_MAJOR_SHFT                                        0x0

#define PMIO_PON_REVISION3_ADDR                                           (PON_BASE      + 0x00000002)
#define PMIO_PON_REVISION3_OFFS                                           0x00000002
#define PMIO_PON_REVISION3_RMSK                                                 0xff
#define PMIO_PON_REVISION3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION3_ADDR, PMIO_PON_REVISION3_RMSK, 0, val)
#define PMIO_PON_REVISION3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION3_ADDR, mask, shift, val)
#define PMIO_PON_REVISION3_ANA_MINOR_BMSK                                       0xff
#define PMIO_PON_REVISION3_ANA_MINOR_SHFT                                        0x0

#define PMIO_PON_REVISION4_ADDR                                           (PON_BASE      + 0x00000003)
#define PMIO_PON_REVISION4_OFFS                                           0x00000003
#define PMIO_PON_REVISION4_RMSK                                                 0xff
#define PMIO_PON_REVISION4_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION4_ADDR, PMIO_PON_REVISION4_RMSK, 0, val)
#define PMIO_PON_REVISION4_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_REVISION4_ADDR, mask, shift, val)
#define PMIO_PON_REVISION4_ANA_MAJOR_BMSK                                       0xff
#define PMIO_PON_REVISION4_ANA_MAJOR_SHFT                                        0x0

#define PMIO_PON_PERPH_TYPE_ADDR                                          (PON_BASE      + 0x00000004)
#define PMIO_PON_PERPH_TYPE_OFFS                                          0x00000004
#define PMIO_PON_PERPH_TYPE_RMSK                                                0xff
#define PMIO_PON_PERPH_TYPE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_TYPE_ADDR, PMIO_PON_PERPH_TYPE_RMSK, 0, val)
#define PMIO_PON_PERPH_TYPE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_TYPE_ADDR, mask, shift, val)
#define PMIO_PON_PERPH_TYPE_TYPE_BMSK                                           0xff
#define PMIO_PON_PERPH_TYPE_TYPE_SHFT                                            0x0

#define PMIO_PON_PERPH_SUBTYPE_ADDR                                       (PON_BASE      + 0x00000005)
#define PMIO_PON_PERPH_SUBTYPE_OFFS                                       0x00000005
#define PMIO_PON_PERPH_SUBTYPE_RMSK                                             0xff
#define PMIO_PON_PERPH_SUBTYPE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_SUBTYPE_ADDR, PMIO_PON_PERPH_SUBTYPE_RMSK, 0, val)
#define PMIO_PON_PERPH_SUBTYPE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_SUBTYPE_ADDR, mask, shift, val)
#define PMIO_PON_PERPH_SUBTYPE_SUBTYPE_BMSK                                     0xff
#define PMIO_PON_PERPH_SUBTYPE_SUBTYPE_SHFT                                      0x0

#define PMIO_PON_PON_PBL_STATUS_ADDR                                      (PON_BASE      + 0x00000007)
#define PMIO_PON_PON_PBL_STATUS_OFFS                                      0x00000007
#define PMIO_PON_PON_PBL_STATUS_RMSK                                            0xf8
#define PMIO_PON_PON_PBL_STATUS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_PBL_STATUS_ADDR, PMIO_PON_PON_PBL_STATUS_RMSK, 0, val)
#define PMIO_PON_PON_PBL_STATUS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_PBL_STATUS_ADDR, mask, shift, val)
#define PMIO_PON_PON_PBL_STATUS_DVDD_RB_OCCURRED_BMSK                           0x80
#define PMIO_PON_PON_PBL_STATUS_DVDD_RB_OCCURRED_SHFT                            0x7
#define PMIO_PON_PON_PBL_STATUS_XVDD_RB_OCCURRED_BMSK                           0x40
#define PMIO_PON_PON_PBL_STATUS_XVDD_RB_OCCURRED_SHFT                            0x6
#define PMIO_PON_PON_PBL_STATUS_REG_WRITE_ERROR_BMSK                            0x20
#define PMIO_PON_PON_PBL_STATUS_REG_WRITE_ERROR_SHFT                             0x5
#define PMIO_PON_PON_PBL_STATUS_REG_RESET_ERROR_BMSK                            0x10
#define PMIO_PON_PON_PBL_STATUS_REG_RESET_ERROR_SHFT                             0x4
#define PMIO_PON_PON_PBL_STATUS_REG_SYNC_ERROR_BMSK                              0x8
#define PMIO_PON_PON_PBL_STATUS_REG_SYNC_ERROR_SHFT                              0x3

#define PMIO_PON_PON_REASON1_ADDR                                         (PON_BASE      + 0x00000008)
#define PMIO_PON_PON_REASON1_OFFS                                         0x00000008
#define PMIO_PON_PON_REASON1_RMSK                                               0xff
#define PMIO_PON_PON_REASON1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_REASON1_ADDR, PMIO_PON_PON_REASON1_RMSK, 0, val)
#define PMIO_PON_PON_REASON1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_REASON1_ADDR, mask, shift, val)
#define PMIO_PON_PON_REASON1_KPDPWR_N_BMSK                                      0x80
#define PMIO_PON_PON_REASON1_KPDPWR_N_SHFT                                       0x7
#define PMIO_PON_PON_REASON1_CBLPWR_N_BMSK                                      0x40
#define PMIO_PON_PON_REASON1_CBLPWR_N_SHFT                                       0x6
#define PMIO_PON_PON_REASON1_PON1_BMSK                                          0x20
#define PMIO_PON_PON_REASON1_PON1_SHFT                                           0x5
#define PMIO_PON_PON_REASON1_USB_CHG_BMSK                                       0x10
#define PMIO_PON_PON_REASON1_USB_CHG_SHFT                                        0x4
#define PMIO_PON_PON_REASON1_DC_CHG_BMSK                                         0x8
#define PMIO_PON_PON_REASON1_DC_CHG_SHFT                                         0x3
#define PMIO_PON_PON_REASON1_RTC_BMSK                                            0x4
#define PMIO_PON_PON_REASON1_RTC_SHFT                                            0x2
#define PMIO_PON_PON_REASON1_SMPL_BMSK                                           0x2
#define PMIO_PON_PON_REASON1_SMPL_SHFT                                           0x1
#define PMIO_PON_PON_REASON1_HARD_RESET_BMSK                                     0x1
#define PMIO_PON_PON_REASON1_HARD_RESET_SHFT                                     0x0

#define PMIO_PON_WARM_RESET_REASON1_ADDR                                  (PON_BASE      + 0x0000000a)
#define PMIO_PON_WARM_RESET_REASON1_OFFS                                  0x0000000a
#define PMIO_PON_WARM_RESET_REASON1_RMSK                                        0xff
#define PMIO_PON_WARM_RESET_REASON1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WARM_RESET_REASON1_ADDR, PMIO_PON_WARM_RESET_REASON1_RMSK, 0, val)
#define PMIO_PON_WARM_RESET_REASON1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WARM_RESET_REASON1_ADDR, mask, shift, val)
#define PMIO_PON_WARM_RESET_REASON1_KPDPWR_N_BMSK                               0x80
#define PMIO_PON_WARM_RESET_REASON1_KPDPWR_N_SHFT                                0x7
#define PMIO_PON_WARM_RESET_REASON1_RESIN_N_BMSK                                0x40
#define PMIO_PON_WARM_RESET_REASON1_RESIN_N_SHFT                                 0x6
#define PMIO_PON_WARM_RESET_REASON1_KPDPWR_AND_RESIN_BMSK                       0x20
#define PMIO_PON_WARM_RESET_REASON1_KPDPWR_AND_RESIN_SHFT                        0x5
#define PMIO_PON_WARM_RESET_REASON1_GP2_BMSK                                    0x10
#define PMIO_PON_WARM_RESET_REASON1_GP2_SHFT                                     0x4
#define PMIO_PON_WARM_RESET_REASON1_GP1_BMSK                                     0x8
#define PMIO_PON_WARM_RESET_REASON1_GP1_SHFT                                     0x3
#define PMIO_PON_WARM_RESET_REASON1_PMIC_WD_BMSK                                 0x4
#define PMIO_PON_WARM_RESET_REASON1_PMIC_WD_SHFT                                 0x2
#define PMIO_PON_WARM_RESET_REASON1_PS_HOLD_BMSK                                 0x2
#define PMIO_PON_WARM_RESET_REASON1_PS_HOLD_SHFT                                 0x1
#define PMIO_PON_WARM_RESET_REASON1_SOFT_BMSK                                    0x1
#define PMIO_PON_WARM_RESET_REASON1_SOFT_SHFT                                    0x0

#define PMIO_PON_WARM_RESET_REASON2_ADDR                                  (PON_BASE      + 0x0000000b)
#define PMIO_PON_WARM_RESET_REASON2_OFFS                                  0x0000000b
#define PMIO_PON_WARM_RESET_REASON2_RMSK                                        0x10
#define PMIO_PON_WARM_RESET_REASON2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WARM_RESET_REASON2_ADDR, PMIO_PON_WARM_RESET_REASON2_RMSK, 0, val)
#define PMIO_PON_WARM_RESET_REASON2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WARM_RESET_REASON2_ADDR, mask, shift, val)
#define PMIO_PON_WARM_RESET_REASON2_TFT_BMSK                                    0x10
#define PMIO_PON_WARM_RESET_REASON2_TFT_SHFT                                     0x4

#define PMIO_PON_POFF_REASON1_ADDR                                        (PON_BASE      + 0x0000000c)
#define PMIO_PON_POFF_REASON1_OFFS                                        0x0000000c
#define PMIO_PON_POFF_REASON1_RMSK                                              0xff
#define PMIO_PON_POFF_REASON1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_POFF_REASON1_ADDR, PMIO_PON_POFF_REASON1_RMSK, 0, val)
#define PMIO_PON_POFF_REASON1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_POFF_REASON1_ADDR, mask, shift, val)
#define PMIO_PON_POFF_REASON1_KPDPWR_N_BMSK                                     0x80
#define PMIO_PON_POFF_REASON1_KPDPWR_N_SHFT                                      0x7
#define PMIO_PON_POFF_REASON1_RESIN_N_BMSK                                      0x40
#define PMIO_PON_POFF_REASON1_RESIN_N_SHFT                                       0x6
#define PMIO_PON_POFF_REASON1_KPDPWR_AND_RESIN_BMSK                             0x20
#define PMIO_PON_POFF_REASON1_KPDPWR_AND_RESIN_SHFT                              0x5
#define PMIO_PON_POFF_REASON1_GP2_BMSK                                          0x10
#define PMIO_PON_POFF_REASON1_GP2_SHFT                                           0x4
#define PMIO_PON_POFF_REASON1_GP1_BMSK                                           0x8
#define PMIO_PON_POFF_REASON1_GP1_SHFT                                           0x3
#define PMIO_PON_POFF_REASON1_PMIC_WD_BMSK                                       0x4
#define PMIO_PON_POFF_REASON1_PMIC_WD_SHFT                                       0x2
#define PMIO_PON_POFF_REASON1_PS_HOLD_BMSK                                       0x2
#define PMIO_PON_POFF_REASON1_PS_HOLD_SHFT                                       0x1
#define PMIO_PON_POFF_REASON1_SOFT_BMSK                                          0x1
#define PMIO_PON_POFF_REASON1_SOFT_SHFT                                          0x0

#define PMIO_PON_POFF_REASON2_ADDR                                        (PON_BASE      + 0x0000000d)
#define PMIO_PON_POFF_REASON2_OFFS                                        0x0000000d
#define PMIO_PON_POFF_REASON2_RMSK                                              0xf8
#define PMIO_PON_POFF_REASON2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_POFF_REASON2_ADDR, PMIO_PON_POFF_REASON2_RMSK, 0, val)
#define PMIO_PON_POFF_REASON2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_POFF_REASON2_ADDR, mask, shift, val)
#define PMIO_PON_POFF_REASON2_STAGE3_BMSK                                       0x80
#define PMIO_PON_POFF_REASON2_STAGE3_SHFT                                        0x7
#define PMIO_PON_POFF_REASON2_OTST3_BMSK                                        0x40
#define PMIO_PON_POFF_REASON2_OTST3_SHFT                                         0x6
#define PMIO_PON_POFF_REASON2_UVLO_BMSK                                         0x20
#define PMIO_PON_POFF_REASON2_UVLO_SHFT                                          0x5
#define PMIO_PON_POFF_REASON2_TFT_BMSK                                          0x10
#define PMIO_PON_POFF_REASON2_TFT_SHFT                                           0x4
#define PMIO_PON_POFF_REASON2_CHARGER_BMSK                                       0x8
#define PMIO_PON_POFF_REASON2_CHARGER_SHFT                                       0x3

#define PMIO_PON_SOFT_RESET_REASON1_ADDR                                  (PON_BASE      + 0x0000000e)
#define PMIO_PON_SOFT_RESET_REASON1_OFFS                                  0x0000000e
#define PMIO_PON_SOFT_RESET_REASON1_RMSK                                        0xff
#define PMIO_PON_SOFT_RESET_REASON1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RESET_REASON1_ADDR, PMIO_PON_SOFT_RESET_REASON1_RMSK, 0, val)
#define PMIO_PON_SOFT_RESET_REASON1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RESET_REASON1_ADDR, mask, shift, val)
#define PMIO_PON_SOFT_RESET_REASON1_KPDPWR_N_BMSK                               0x80
#define PMIO_PON_SOFT_RESET_REASON1_KPDPWR_N_SHFT                                0x7
#define PMIO_PON_SOFT_RESET_REASON1_RESIN_N_BMSK                                0x40
#define PMIO_PON_SOFT_RESET_REASON1_RESIN_N_SHFT                                 0x6
#define PMIO_PON_SOFT_RESET_REASON1_KPDPWR_AND_RESIN_BMSK                       0x20
#define PMIO_PON_SOFT_RESET_REASON1_KPDPWR_AND_RESIN_SHFT                        0x5
#define PMIO_PON_SOFT_RESET_REASON1_GP2_BMSK                                    0x10
#define PMIO_PON_SOFT_RESET_REASON1_GP2_SHFT                                     0x4
#define PMIO_PON_SOFT_RESET_REASON1_GP1_BMSK                                     0x8
#define PMIO_PON_SOFT_RESET_REASON1_GP1_SHFT                                     0x3
#define PMIO_PON_SOFT_RESET_REASON1_PMIC_WD_BMSK                                 0x4
#define PMIO_PON_SOFT_RESET_REASON1_PMIC_WD_SHFT                                 0x2
#define PMIO_PON_SOFT_RESET_REASON1_PS_HOLD_BMSK                                 0x2
#define PMIO_PON_SOFT_RESET_REASON1_PS_HOLD_SHFT                                 0x1
#define PMIO_PON_SOFT_RESET_REASON1_SOFT_BMSK                                    0x1
#define PMIO_PON_SOFT_RESET_REASON1_SOFT_SHFT                                    0x0

#define PMIO_PON_SOFT_RESET_REASON2_ADDR                                  (PON_BASE      + 0x0000000f)
#define PMIO_PON_SOFT_RESET_REASON2_OFFS                                  0x0000000f
#define PMIO_PON_SOFT_RESET_REASON2_RMSK                                        0x10
#define PMIO_PON_SOFT_RESET_REASON2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RESET_REASON2_ADDR, PMIO_PON_SOFT_RESET_REASON2_RMSK, 0, val)
#define PMIO_PON_SOFT_RESET_REASON2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RESET_REASON2_ADDR, mask, shift, val)
#define PMIO_PON_SOFT_RESET_REASON2_TFT_BMSK                                    0x10
#define PMIO_PON_SOFT_RESET_REASON2_TFT_SHFT                                     0x4

#define PMIO_PON_INT_RT_STS_ADDR                                          (PON_BASE      + 0x00000010)
#define PMIO_PON_INT_RT_STS_OFFS                                          0x00000010
#define PMIO_PON_INT_RT_STS_RMSK                                                0xff
#define PMIO_PON_INT_RT_STS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_RT_STS_ADDR, PMIO_PON_INT_RT_STS_RMSK, 0, val)
#define PMIO_PON_INT_RT_STS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_RT_STS_ADDR, mask, shift, val)
#define PMIO_PON_INT_RT_STS_SOFT_RESET_OCCURED_BMSK                             0x80
#define PMIO_PON_INT_RT_STS_SOFT_RESET_OCCURED_SHFT                              0x7
#define PMIO_PON_INT_RT_STS_PMIC_WD_BARK_BMSK                                   0x40
#define PMIO_PON_INT_RT_STS_PMIC_WD_BARK_SHFT                                    0x6
#define PMIO_PON_INT_RT_STS_K_R_BARK_BMSK                                       0x20
#define PMIO_PON_INT_RT_STS_K_R_BARK_SHFT                                        0x5
#define PMIO_PON_INT_RT_STS_RESIN_BARK_BMSK                                     0x10
#define PMIO_PON_INT_RT_STS_RESIN_BARK_SHFT                                      0x4
#define PMIO_PON_INT_RT_STS_KPDPWR_BARK_BMSK                                     0x8
#define PMIO_PON_INT_RT_STS_KPDPWR_BARK_SHFT                                     0x3
#define PMIO_PON_INT_RT_STS_CBLPWR_ON_BMSK                                       0x4
#define PMIO_PON_INT_RT_STS_CBLPWR_ON_SHFT                                       0x2
#define PMIO_PON_INT_RT_STS_RESIN_ON_BMSK                                        0x2
#define PMIO_PON_INT_RT_STS_RESIN_ON_SHFT                                        0x1
#define PMIO_PON_INT_RT_STS_KPDPWR_ON_BMSK                                       0x1
#define PMIO_PON_INT_RT_STS_KPDPWR_ON_SHFT                                       0x0

#define PMIO_PON_INT_SET_TYPE_ADDR                                        (PON_BASE      + 0x00000011)
#define PMIO_PON_INT_SET_TYPE_OFFS                                        0x00000011
#define PMIO_PON_INT_SET_TYPE_RMSK                                              0xff
#define PMIO_PON_INT_SET_TYPE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_SET_TYPE_ADDR, PMIO_PON_INT_SET_TYPE_RMSK, 0, val)
#define PMIO_PON_INT_SET_TYPE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_SET_TYPE_ADDR, mask, shift, val)
#define PMIO_PON_INT_SET_TYPE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_SET_TYPE_ADDR, val)
#define PMIO_PON_INT_SET_TYPE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_SET_TYPE_ADDR, mask, shift, val)
#define PMIO_PON_INT_SET_TYPE_SOFT_RESET_OCCURED_BMSK                           0x80
#define PMIO_PON_INT_SET_TYPE_SOFT_RESET_OCCURED_SHFT                            0x7
#define PMIO_PON_INT_SET_TYPE_PMIC_WD_BARK_BMSK                                 0x40
#define PMIO_PON_INT_SET_TYPE_PMIC_WD_BARK_SHFT                                  0x6
#define PMIO_PON_INT_SET_TYPE_K_R_BARK_BMSK                                     0x20
#define PMIO_PON_INT_SET_TYPE_K_R_BARK_SHFT                                      0x5
#define PMIO_PON_INT_SET_TYPE_RESIN_BARK_BMSK                                   0x10
#define PMIO_PON_INT_SET_TYPE_RESIN_BARK_SHFT                                    0x4
#define PMIO_PON_INT_SET_TYPE_KPDPWR_BARK_BMSK                                   0x8
#define PMIO_PON_INT_SET_TYPE_KPDPWR_BARK_SHFT                                   0x3
#define PMIO_PON_INT_SET_TYPE_CBLPWR_ON_BMSK                                     0x4
#define PMIO_PON_INT_SET_TYPE_CBLPWR_ON_SHFT                                     0x2
#define PMIO_PON_INT_SET_TYPE_RESIN_ON_BMSK                                      0x2
#define PMIO_PON_INT_SET_TYPE_RESIN_ON_SHFT                                      0x1
#define PMIO_PON_INT_SET_TYPE_KPDPWR_ON_BMSK                                     0x1
#define PMIO_PON_INT_SET_TYPE_KPDPWR_ON_SHFT                                     0x0

#define PMIO_PON_INT_POLARITY_HIGH_ADDR                                   (PON_BASE      + 0x00000012)
#define PMIO_PON_INT_POLARITY_HIGH_OFFS                                   0x00000012
#define PMIO_PON_INT_POLARITY_HIGH_RMSK                                         0xff
#define PMIO_PON_INT_POLARITY_HIGH_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_HIGH_ADDR, PMIO_PON_INT_POLARITY_HIGH_RMSK, 0, val)
#define PMIO_PON_INT_POLARITY_HIGH_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_HIGH_ADDR, mask, shift, val)
#define PMIO_PON_INT_POLARITY_HIGH_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_POLARITY_HIGH_ADDR, val)
#define PMIO_PON_INT_POLARITY_HIGH_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_HIGH_ADDR, mask, shift, val)
#define PMIO_PON_INT_POLARITY_HIGH_SOFT_RESET_OCCURED_BMSK                      0x80
#define PMIO_PON_INT_POLARITY_HIGH_SOFT_RESET_OCCURED_SHFT                       0x7
#define PMIO_PON_INT_POLARITY_HIGH_PMIC_WD_BARK_BMSK                            0x40
#define PMIO_PON_INT_POLARITY_HIGH_PMIC_WD_BARK_SHFT                             0x6
#define PMIO_PON_INT_POLARITY_HIGH_K_R_BARK_BMSK                                0x20
#define PMIO_PON_INT_POLARITY_HIGH_K_R_BARK_SHFT                                 0x5
#define PMIO_PON_INT_POLARITY_HIGH_RESIN_BARK_BMSK                              0x10
#define PMIO_PON_INT_POLARITY_HIGH_RESIN_BARK_SHFT                               0x4
#define PMIO_PON_INT_POLARITY_HIGH_KPDPWR_BARK_BMSK                              0x8
#define PMIO_PON_INT_POLARITY_HIGH_KPDPWR_BARK_SHFT                              0x3
#define PMIO_PON_INT_POLARITY_HIGH_CBLPWR_ON_BMSK                                0x4
#define PMIO_PON_INT_POLARITY_HIGH_CBLPWR_ON_SHFT                                0x2
#define PMIO_PON_INT_POLARITY_HIGH_RESIN_ON_BMSK                                 0x2
#define PMIO_PON_INT_POLARITY_HIGH_RESIN_ON_SHFT                                 0x1
#define PMIO_PON_INT_POLARITY_HIGH_KPDPWR_ON_BMSK                                0x1
#define PMIO_PON_INT_POLARITY_HIGH_KPDPWR_ON_SHFT                                0x0

#define PMIO_PON_INT_POLARITY_LOW_ADDR                                    (PON_BASE      + 0x00000013)
#define PMIO_PON_INT_POLARITY_LOW_OFFS                                    0x00000013
#define PMIO_PON_INT_POLARITY_LOW_RMSK                                          0xff
#define PMIO_PON_INT_POLARITY_LOW_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_LOW_ADDR, PMIO_PON_INT_POLARITY_LOW_RMSK, 0, val)
#define PMIO_PON_INT_POLARITY_LOW_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_LOW_ADDR, mask, shift, val)
#define PMIO_PON_INT_POLARITY_LOW_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_POLARITY_LOW_ADDR, val)
#define PMIO_PON_INT_POLARITY_LOW_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_POLARITY_LOW_ADDR, mask, shift, val)
#define PMIO_PON_INT_POLARITY_LOW_SOFT_RESET_OCCURED_BMSK                       0x80
#define PMIO_PON_INT_POLARITY_LOW_SOFT_RESET_OCCURED_SHFT                        0x7
#define PMIO_PON_INT_POLARITY_LOW_PMIC_WD_BARK_BMSK                             0x40
#define PMIO_PON_INT_POLARITY_LOW_PMIC_WD_BARK_SHFT                              0x6
#define PMIO_PON_INT_POLARITY_LOW_K_R_BARK_BMSK                                 0x20
#define PMIO_PON_INT_POLARITY_LOW_K_R_BARK_SHFT                                  0x5
#define PMIO_PON_INT_POLARITY_LOW_RESIN_BARK_BMSK                               0x10
#define PMIO_PON_INT_POLARITY_LOW_RESIN_BARK_SHFT                                0x4
#define PMIO_PON_INT_POLARITY_LOW_KPDPWR_BARK_BMSK                               0x8
#define PMIO_PON_INT_POLARITY_LOW_KPDPWR_BARK_SHFT                               0x3
#define PMIO_PON_INT_POLARITY_LOW_CBLPWR_ON_BMSK                                 0x4
#define PMIO_PON_INT_POLARITY_LOW_CBLPWR_ON_SHFT                                 0x2
#define PMIO_PON_INT_POLARITY_LOW_RESIN_ON_BMSK                                  0x2
#define PMIO_PON_INT_POLARITY_LOW_RESIN_ON_SHFT                                  0x1
#define PMIO_PON_INT_POLARITY_LOW_KPDPWR_ON_BMSK                                 0x1
#define PMIO_PON_INT_POLARITY_LOW_KPDPWR_ON_SHFT                                 0x0

#define PMIO_PON_INT_LATCHED_CLR_ADDR                                     (PON_BASE      + 0x00000014)
#define PMIO_PON_INT_LATCHED_CLR_OFFS                                     0x00000014
#define PMIO_PON_INT_LATCHED_CLR_RMSK                                           0xff
#define PMIO_PON_INT_LATCHED_CLR_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_LATCHED_CLR_ADDR, val)
#define PMIO_PON_INT_LATCHED_CLR_SOFT_RESET_OCCURED_BMSK                        0x80
#define PMIO_PON_INT_LATCHED_CLR_SOFT_RESET_OCCURED_SHFT                         0x7
#define PMIO_PON_INT_LATCHED_CLR_PMIC_WD_BARK_BMSK                              0x40
#define PMIO_PON_INT_LATCHED_CLR_PMIC_WD_BARK_SHFT                               0x6
#define PMIO_PON_INT_LATCHED_CLR_K_R_BARK_BMSK                                  0x20
#define PMIO_PON_INT_LATCHED_CLR_K_R_BARK_SHFT                                   0x5
#define PMIO_PON_INT_LATCHED_CLR_RESIN_BARK_BMSK                                0x10
#define PMIO_PON_INT_LATCHED_CLR_RESIN_BARK_SHFT                                 0x4
#define PMIO_PON_INT_LATCHED_CLR_KPDPWR_BARK_BMSK                                0x8
#define PMIO_PON_INT_LATCHED_CLR_KPDPWR_BARK_SHFT                                0x3
#define PMIO_PON_INT_LATCHED_CLR_CBLPWR_ON_BMSK                                  0x4
#define PMIO_PON_INT_LATCHED_CLR_CBLPWR_ON_SHFT                                  0x2
#define PMIO_PON_INT_LATCHED_CLR_RESIN_ON_BMSK                                   0x2
#define PMIO_PON_INT_LATCHED_CLR_RESIN_ON_SHFT                                   0x1
#define PMIO_PON_INT_LATCHED_CLR_KPDPWR_ON_BMSK                                  0x1
#define PMIO_PON_INT_LATCHED_CLR_KPDPWR_ON_SHFT                                  0x0

#define PMIO_PON_INT_EN_SET_ADDR                                          (PON_BASE      + 0x00000015)
#define PMIO_PON_INT_EN_SET_OFFS                                          0x00000015
#define PMIO_PON_INT_EN_SET_RMSK                                                0xff
#define PMIO_PON_INT_EN_SET_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_SET_ADDR, PMIO_PON_INT_EN_SET_RMSK, 0, val)
#define PMIO_PON_INT_EN_SET_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_SET_ADDR, mask, shift, val)
#define PMIO_PON_INT_EN_SET_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_EN_SET_ADDR, val)
#define PMIO_PON_INT_EN_SET_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_SET_ADDR, mask, shift, val)
#define PMIO_PON_INT_EN_SET_SOFT_RESET_OCCURED_BMSK                             0x80
#define PMIO_PON_INT_EN_SET_SOFT_RESET_OCCURED_SHFT                              0x7
#define PMIO_PON_INT_EN_SET_PMIC_WD_BARK_BMSK                                   0x40
#define PMIO_PON_INT_EN_SET_PMIC_WD_BARK_SHFT                                    0x6
#define PMIO_PON_INT_EN_SET_K_R_BARK_BMSK                                       0x20
#define PMIO_PON_INT_EN_SET_K_R_BARK_SHFT                                        0x5
#define PMIO_PON_INT_EN_SET_RESIN_BARK_BMSK                                     0x10
#define PMIO_PON_INT_EN_SET_RESIN_BARK_SHFT                                      0x4
#define PMIO_PON_INT_EN_SET_KPDPWR_BARK_BMSK                                     0x8
#define PMIO_PON_INT_EN_SET_KPDPWR_BARK_SHFT                                     0x3
#define PMIO_PON_INT_EN_SET_CBLPWR_ON_BMSK                                       0x4
#define PMIO_PON_INT_EN_SET_CBLPWR_ON_SHFT                                       0x2
#define PMIO_PON_INT_EN_SET_RESIN_ON_BMSK                                        0x2
#define PMIO_PON_INT_EN_SET_RESIN_ON_SHFT                                        0x1
#define PMIO_PON_INT_EN_SET_KPDPWR_ON_BMSK                                       0x1
#define PMIO_PON_INT_EN_SET_KPDPWR_ON_SHFT                                       0x0

#define PMIO_PON_INT_EN_CLR_ADDR                                          (PON_BASE      + 0x00000016)
#define PMIO_PON_INT_EN_CLR_OFFS                                          0x00000016
#define PMIO_PON_INT_EN_CLR_RMSK                                                0xff
#define PMIO_PON_INT_EN_CLR_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_CLR_ADDR, PMIO_PON_INT_EN_CLR_RMSK, 0, val)
#define PMIO_PON_INT_EN_CLR_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_CLR_ADDR, mask, shift, val)
#define PMIO_PON_INT_EN_CLR_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_EN_CLR_ADDR, val)
#define PMIO_PON_INT_EN_CLR_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_EN_CLR_ADDR, mask, shift, val)
#define PMIO_PON_INT_EN_CLR_SOFT_RESET_OCCURED_BMSK                             0x80
#define PMIO_PON_INT_EN_CLR_SOFT_RESET_OCCURED_SHFT                              0x7
#define PMIO_PON_INT_EN_CLR_PMIC_WD_BARK_BMSK                                   0x40
#define PMIO_PON_INT_EN_CLR_PMIC_WD_BARK_SHFT                                    0x6
#define PMIO_PON_INT_EN_CLR_K_R_BARK_BMSK                                       0x20
#define PMIO_PON_INT_EN_CLR_K_R_BARK_SHFT                                        0x5
#define PMIO_PON_INT_EN_CLR_RESIN_BARK_BMSK                                     0x10
#define PMIO_PON_INT_EN_CLR_RESIN_BARK_SHFT                                      0x4
#define PMIO_PON_INT_EN_CLR_KPDPWR_BARK_BMSK                                     0x8
#define PMIO_PON_INT_EN_CLR_KPDPWR_BARK_SHFT                                     0x3
#define PMIO_PON_INT_EN_CLR_CBLPWR_ON_BMSK                                       0x4
#define PMIO_PON_INT_EN_CLR_CBLPWR_ON_SHFT                                       0x2
#define PMIO_PON_INT_EN_CLR_RESIN_ON_BMSK                                        0x2
#define PMIO_PON_INT_EN_CLR_RESIN_ON_SHFT                                        0x1
#define PMIO_PON_INT_EN_CLR_KPDPWR_ON_BMSK                                       0x1
#define PMIO_PON_INT_EN_CLR_KPDPWR_ON_SHFT                                       0x0

#define PMIO_PON_INT_LATCHED_STS_ADDR                                     (PON_BASE      + 0x00000018)
#define PMIO_PON_INT_LATCHED_STS_OFFS                                     0x00000018
#define PMIO_PON_INT_LATCHED_STS_RMSK                                           0xff
#define PMIO_PON_INT_LATCHED_STS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_LATCHED_STS_ADDR, PMIO_PON_INT_LATCHED_STS_RMSK, 0, val)
#define PMIO_PON_INT_LATCHED_STS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_LATCHED_STS_ADDR, mask, shift, val)
#define PMIO_PON_INT_LATCHED_STS_SOFT_RESET_OCCURED_BMSK                        0x80
#define PMIO_PON_INT_LATCHED_STS_SOFT_RESET_OCCURED_SHFT                         0x7
#define PMIO_PON_INT_LATCHED_STS_PMIC_WD_BARK_BMSK                              0x40
#define PMIO_PON_INT_LATCHED_STS_PMIC_WD_BARK_SHFT                               0x6
#define PMIO_PON_INT_LATCHED_STS_K_R_BARK_BMSK                                  0x20
#define PMIO_PON_INT_LATCHED_STS_K_R_BARK_SHFT                                   0x5
#define PMIO_PON_INT_LATCHED_STS_RESIN_BARK_BMSK                                0x10
#define PMIO_PON_INT_LATCHED_STS_RESIN_BARK_SHFT                                 0x4
#define PMIO_PON_INT_LATCHED_STS_KPDPWR_BARK_BMSK                                0x8
#define PMIO_PON_INT_LATCHED_STS_KPDPWR_BARK_SHFT                                0x3
#define PMIO_PON_INT_LATCHED_STS_CBLPWR_ON_BMSK                                  0x4
#define PMIO_PON_INT_LATCHED_STS_CBLPWR_ON_SHFT                                  0x2
#define PMIO_PON_INT_LATCHED_STS_RESIN_ON_BMSK                                   0x2
#define PMIO_PON_INT_LATCHED_STS_RESIN_ON_SHFT                                   0x1
#define PMIO_PON_INT_LATCHED_STS_KPDPWR_ON_BMSK                                  0x1
#define PMIO_PON_INT_LATCHED_STS_KPDPWR_ON_SHFT                                  0x0

#define PMIO_PON_INT_PENDING_STS_ADDR                                     (PON_BASE      + 0x00000019)
#define PMIO_PON_INT_PENDING_STS_OFFS                                     0x00000019
#define PMIO_PON_INT_PENDING_STS_RMSK                                           0xff
#define PMIO_PON_INT_PENDING_STS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_PENDING_STS_ADDR, PMIO_PON_INT_PENDING_STS_RMSK, 0, val)
#define PMIO_PON_INT_PENDING_STS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_PENDING_STS_ADDR, mask, shift, val)
#define PMIO_PON_INT_PENDING_STS_SOFT_RESET_OCCURED_BMSK                        0x80
#define PMIO_PON_INT_PENDING_STS_SOFT_RESET_OCCURED_SHFT                         0x7
#define PMIO_PON_INT_PENDING_STS_PMIC_WD_BARK_BMSK                              0x40
#define PMIO_PON_INT_PENDING_STS_PMIC_WD_BARK_SHFT                               0x6
#define PMIO_PON_INT_PENDING_STS_K_R_BARK_BMSK                                  0x20
#define PMIO_PON_INT_PENDING_STS_K_R_BARK_SHFT                                   0x5
#define PMIO_PON_INT_PENDING_STS_RESIN_BARK_BMSK                                0x10
#define PMIO_PON_INT_PENDING_STS_RESIN_BARK_SHFT                                 0x4
#define PMIO_PON_INT_PENDING_STS_KPDPWR_BARK_BMSK                                0x8
#define PMIO_PON_INT_PENDING_STS_KPDPWR_BARK_SHFT                                0x3
#define PMIO_PON_INT_PENDING_STS_CBLPWR_ON_BMSK                                  0x4
#define PMIO_PON_INT_PENDING_STS_CBLPWR_ON_SHFT                                  0x2
#define PMIO_PON_INT_PENDING_STS_RESIN_ON_BMSK                                   0x2
#define PMIO_PON_INT_PENDING_STS_RESIN_ON_SHFT                                   0x1
#define PMIO_PON_INT_PENDING_STS_KPDPWR_ON_BMSK                                  0x1
#define PMIO_PON_INT_PENDING_STS_KPDPWR_ON_SHFT                                  0x0

#define PMIO_PON_INT_MID_SEL_ADDR                                         (PON_BASE      + 0x0000001a)
#define PMIO_PON_INT_MID_SEL_OFFS                                         0x0000001a
#define PMIO_PON_INT_MID_SEL_RMSK                                                0x3
#define PMIO_PON_INT_MID_SEL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_MID_SEL_ADDR, PMIO_PON_INT_MID_SEL_RMSK, 0, val)
#define PMIO_PON_INT_MID_SEL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_MID_SEL_ADDR, mask, shift, val)
#define PMIO_PON_INT_MID_SEL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_MID_SEL_ADDR, val)
#define PMIO_PON_INT_MID_SEL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_MID_SEL_ADDR, mask, shift, val)
#define PMIO_PON_INT_MID_SEL_INT_MID_SEL_BMSK                                    0x3
#define PMIO_PON_INT_MID_SEL_INT_MID_SEL_SHFT                                    0x0

#define PMIO_PON_INT_PRIORITY_ADDR                                        (PON_BASE      + 0x0000001b)
#define PMIO_PON_INT_PRIORITY_OFFS                                        0x0000001b
#define PMIO_PON_INT_PRIORITY_RMSK                                               0x1
#define PMIO_PON_INT_PRIORITY_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_PRIORITY_ADDR, PMIO_PON_INT_PRIORITY_RMSK, 0, val)
#define PMIO_PON_INT_PRIORITY_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_INT_PRIORITY_ADDR, mask, shift, val)
#define PMIO_PON_INT_PRIORITY_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_INT_PRIORITY_ADDR, val)
#define PMIO_PON_INT_PRIORITY_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_INT_PRIORITY_ADDR, mask, shift, val)
#define PMIO_PON_INT_PRIORITY_INT_PRIORITY_BMSK                                  0x1
#define PMIO_PON_INT_PRIORITY_INT_PRIORITY_SHFT                                  0x0

#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR                             (PON_BASE      + 0x00000040)
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_OFFS                             0x00000040
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_RMSK                                    0xf
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR, PMIO_PON_KPDPWR_N_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_S1_TIMER_BMSK                           0xf
#define PMIO_PON_KPDPWR_N_RESET_S1_TIMER_S1_TIMER_SHFT                           0x0

#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR                             (PON_BASE      + 0x00000041)
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_OFFS                             0x00000041
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_RMSK                                    0x7
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR, PMIO_PON_KPDPWR_N_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_S2_TIMER_BMSK                           0x7
#define PMIO_PON_KPDPWR_N_RESET_S2_TIMER_S2_TIMER_SHFT                           0x0

#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR                               (PON_BASE      + 0x00000042)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_OFFS                               0x00000042
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_RMSK                                      0xf
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR, PMIO_PON_KPDPWR_N_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_RESET_TYPE_BMSK                           0xf
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL_RESET_TYPE_SHFT                           0x0

#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR                              (PON_BASE      + 0x00000043)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_OFFS                              0x00000043
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_RMSK                                    0x80
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR, PMIO_PON_KPDPWR_N_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_S2_RESET_EN_BMSK                        0x80
#define PMIO_PON_KPDPWR_N_RESET_S2_CTL2_S2_RESET_EN_SHFT                         0x7

#define PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR                              (PON_BASE      + 0x00000044)
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_OFFS                              0x00000044
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_RMSK                                     0xf
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR, PMIO_PON_RESIN_N_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_S1_TIMER_BMSK                            0xf
#define PMIO_PON_RESIN_N_RESET_S1_TIMER_S1_TIMER_SHFT                            0x0

#define PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR                              (PON_BASE      + 0x00000045)
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_OFFS                              0x00000045
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_RMSK                                     0x7
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR, PMIO_PON_RESIN_N_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_S2_TIMER_BMSK                            0x7
#define PMIO_PON_RESIN_N_RESET_S2_TIMER_S2_TIMER_SHFT                            0x0

#define PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR                                (PON_BASE      + 0x00000046)
#define PMIO_PON_RESIN_N_RESET_S2_CTL_OFFS                                0x00000046
#define PMIO_PON_RESIN_N_RESET_S2_CTL_RMSK                                       0xf
#define PMIO_PON_RESIN_N_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR, PMIO_PON_RESIN_N_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL_RESET_TYPE_BMSK                            0xf
#define PMIO_PON_RESIN_N_RESET_S2_CTL_RESET_TYPE_SHFT                            0x0

#define PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR                               (PON_BASE      + 0x00000047)
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_OFFS                               0x00000047
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_RMSK                                     0x80
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR, PMIO_PON_RESIN_N_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_S2_RESET_EN_BMSK                         0x80
#define PMIO_PON_RESIN_N_RESET_S2_CTL2_S2_RESET_EN_SHFT                          0x7

#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR                     (PON_BASE      + 0x00000048)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_OFFS                     0x00000048
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_RMSK                            0xf
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR, PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_S1_TIMER_BMSK                   0xf
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_S1_TIMER_SHFT                   0x0

#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR                     (PON_BASE      + 0x00000049)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_OFFS                     0x00000049
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_RMSK                            0x7
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_S2_TIMER_BMSK                   0x7
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_S2_TIMER_SHFT                   0x0

#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR                       (PON_BASE      + 0x0000004a)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_OFFS                       0x0000004a
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_RMSK                              0xf
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_RESET_TYPE_BMSK                   0xf
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_RESET_TYPE_SHFT                   0x0

#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR                      (PON_BASE      + 0x0000004b)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_OFFS                      0x0000004b
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_RMSK                            0x80
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_S2_RESET_EN_BMSK                0x80
#define PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_S2_RESET_EN_SHFT                 0x7

#define PMIO_PON_GP2_RESET_S1_TIMER_ADDR                                  (PON_BASE      + 0x0000004c)
#define PMIO_PON_GP2_RESET_S1_TIMER_OFFS                                  0x0000004c
#define PMIO_PON_GP2_RESET_S1_TIMER_RMSK                                         0xf
#define PMIO_PON_GP2_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S1_TIMER_ADDR, PMIO_PON_GP2_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_GP2_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP2_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_GP2_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S1_TIMER_S1_TIMER_BMSK                                0xf
#define PMIO_PON_GP2_RESET_S1_TIMER_S1_TIMER_SHFT                                0x0

#define PMIO_PON_GP2_RESET_S2_TIMER_ADDR                                  (PON_BASE      + 0x0000004d)
#define PMIO_PON_GP2_RESET_S2_TIMER_OFFS                                  0x0000004d
#define PMIO_PON_GP2_RESET_S2_TIMER_RMSK                                         0x7
#define PMIO_PON_GP2_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_TIMER_ADDR, PMIO_PON_GP2_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_GP2_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP2_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_GP2_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_TIMER_S2_TIMER_BMSK                                0x7
#define PMIO_PON_GP2_RESET_S2_TIMER_S2_TIMER_SHFT                                0x0

#define PMIO_PON_GP2_RESET_S2_CTL_ADDR                                    (PON_BASE      + 0x0000004e)
#define PMIO_PON_GP2_RESET_S2_CTL_OFFS                                    0x0000004e
#define PMIO_PON_GP2_RESET_S2_CTL_RMSK                                           0xf
#define PMIO_PON_GP2_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL_ADDR, PMIO_PON_GP2_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_GP2_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_GP2_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_CTL_RESET_TYPE_BMSK                                0xf
#define PMIO_PON_GP2_RESET_S2_CTL_RESET_TYPE_SHFT                                0x0

#define PMIO_PON_GP2_RESET_S2_CTL2_ADDR                                   (PON_BASE      + 0x0000004f)
#define PMIO_PON_GP2_RESET_S2_CTL2_OFFS                                   0x0000004f
#define PMIO_PON_GP2_RESET_S2_CTL2_RMSK                                         0x80
#define PMIO_PON_GP2_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL2_ADDR, PMIO_PON_GP2_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_GP2_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_GP2_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP2_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_GP2_RESET_S2_CTL2_S2_RESET_EN_BMSK                             0x80
#define PMIO_PON_GP2_RESET_S2_CTL2_S2_RESET_EN_SHFT                              0x7

#define PMIO_PON_GP1_RESET_S1_TIMER_ADDR                                  (PON_BASE      + 0x00000050)
#define PMIO_PON_GP1_RESET_S1_TIMER_OFFS                                  0x00000050
#define PMIO_PON_GP1_RESET_S1_TIMER_RMSK                                         0xf
#define PMIO_PON_GP1_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S1_TIMER_ADDR, PMIO_PON_GP1_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_GP1_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP1_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_GP1_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S1_TIMER_S1_TIMER_BMSK                                0xf
#define PMIO_PON_GP1_RESET_S1_TIMER_S1_TIMER_SHFT                                0x0

#define PMIO_PON_GP1_RESET_S2_TIMER_ADDR                                  (PON_BASE      + 0x00000051)
#define PMIO_PON_GP1_RESET_S2_TIMER_OFFS                                  0x00000051
#define PMIO_PON_GP1_RESET_S2_TIMER_RMSK                                         0x7
#define PMIO_PON_GP1_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_TIMER_ADDR, PMIO_PON_GP1_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_GP1_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP1_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_GP1_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_TIMER_S2_TIMER_BMSK                                0x7
#define PMIO_PON_GP1_RESET_S2_TIMER_S2_TIMER_SHFT                                0x0

#define PMIO_PON_GP1_RESET_S2_CTL_ADDR                                    (PON_BASE      + 0x00000052)
#define PMIO_PON_GP1_RESET_S2_CTL_OFFS                                    0x00000052
#define PMIO_PON_GP1_RESET_S2_CTL_RMSK                                           0xf
#define PMIO_PON_GP1_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL_ADDR, PMIO_PON_GP1_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_GP1_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_GP1_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_CTL_RESET_TYPE_BMSK                                0xf
#define PMIO_PON_GP1_RESET_S2_CTL_RESET_TYPE_SHFT                                0x0

#define PMIO_PON_GP1_RESET_S2_CTL2_ADDR                                   (PON_BASE      + 0x00000053)
#define PMIO_PON_GP1_RESET_S2_CTL2_OFFS                                   0x00000053
#define PMIO_PON_GP1_RESET_S2_CTL2_RMSK                                         0x80
#define PMIO_PON_GP1_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL2_ADDR, PMIO_PON_GP1_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_GP1_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_GP1_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_GP1_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_GP1_RESET_S2_CTL2_S2_RESET_EN_BMSK                             0x80
#define PMIO_PON_GP1_RESET_S2_CTL2_S2_RESET_EN_SHFT                              0x7

#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR                              (PON_BASE      + 0x00000054)
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_OFFS                              0x00000054
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_RMSK                                    0x7f
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR, PMIO_PON_PMIC_WD_RESET_S1_TIMER_RMSK, 0, val)
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR, val)
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_S1_TIMER_BMSK                           0x7f
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_S1_TIMER_SHFT                            0x0

#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR                              (PON_BASE      + 0x00000055)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_OFFS                              0x00000055
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_RMSK                                    0x7f
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR, PMIO_PON_PMIC_WD_RESET_S2_TIMER_RMSK, 0, val)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR, val)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_S2_TIMER_BMSK                           0x7f
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_S2_TIMER_SHFT                            0x0

#define PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR                                (PON_BASE      + 0x00000056)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_OFFS                                0x00000056
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_RMSK                                       0xf
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR, PMIO_PON_PMIC_WD_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_RESET_TYPE_BMSK                            0xf
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_RESET_TYPE_SHFT                            0x0

#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR                               (PON_BASE      + 0x00000057)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_OFFS                               0x00000057
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_RMSK                                     0x80
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, PMIO_PON_PMIC_WD_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_S2_RESET_EN_BMSK                         0x80
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_S2_RESET_EN_SHFT                          0x7

#define PMIO_PON_PMIC_WD_RESET_PET_ADDR                                   (PON_BASE      + 0x00000058)
#define PMIO_PON_PMIC_WD_RESET_PET_OFFS                                   0x00000058
#define PMIO_PON_PMIC_WD_RESET_PET_RMSK                                          0x1
#define PMIO_PON_PMIC_WD_RESET_PET_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PMIC_WD_RESET_PET_ADDR, val)
#define PMIO_PON_PMIC_WD_RESET_PET_WATCHDOG_PET_BMSK                             0x1
#define PMIO_PON_PMIC_WD_RESET_PET_WATCHDOG_PET_SHFT                             0x0

#define PMIO_PON_PS_HOLD_RESET_CTL_ADDR                                   (PON_BASE      + 0x0000005a)
#define PMIO_PON_PS_HOLD_RESET_CTL_OFFS                                   0x0000005a
#define PMIO_PON_PS_HOLD_RESET_CTL_RMSK                                          0xf
#define PMIO_PON_PS_HOLD_RESET_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL_ADDR, PMIO_PON_PS_HOLD_RESET_CTL_RMSK, 0, val)
#define PMIO_PON_PS_HOLD_RESET_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PS_HOLD_RESET_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL_ADDR, val)
#define PMIO_PON_PS_HOLD_RESET_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PS_HOLD_RESET_CTL_RESET_TYPE_BMSK                               0xf
#define PMIO_PON_PS_HOLD_RESET_CTL_RESET_TYPE_SHFT                               0x0

#define PMIO_PON_PS_HOLD_RESET_CTL2_ADDR                                  (PON_BASE      + 0x0000005b)
#define PMIO_PON_PS_HOLD_RESET_CTL2_OFFS                                  0x0000005b
#define PMIO_PON_PS_HOLD_RESET_CTL2_RMSK                                        0x80
#define PMIO_PON_PS_HOLD_RESET_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL2_ADDR, PMIO_PON_PS_HOLD_RESET_CTL2_RMSK, 0, val)
#define PMIO_PON_PS_HOLD_RESET_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_PS_HOLD_RESET_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL2_ADDR, val)
#define PMIO_PON_PS_HOLD_RESET_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PS_HOLD_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_PS_HOLD_RESET_CTL2_S2_RESET_EN_BMSK                            0x80
#define PMIO_PON_PS_HOLD_RESET_CTL2_S2_RESET_EN_SHFT                             0x7

#define PMIO_PON_SW_RESET_S2_CTL_ADDR                                     (PON_BASE      + 0x00000062)
#define PMIO_PON_SW_RESET_S2_CTL_OFFS                                     0x00000062
#define PMIO_PON_SW_RESET_S2_CTL_RMSK                                            0xf
#define PMIO_PON_SW_RESET_S2_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL_ADDR, PMIO_PON_SW_RESET_S2_CTL_RMSK, 0, val)
#define PMIO_PON_SW_RESET_S2_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_SW_RESET_S2_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL_ADDR, val)
#define PMIO_PON_SW_RESET_S2_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL_ADDR, mask, shift, val)
#define PMIO_PON_SW_RESET_S2_CTL_RESET_TYPE_BMSK                                 0xf
#define PMIO_PON_SW_RESET_S2_CTL_RESET_TYPE_SHFT                                 0x0

#define PMIO_PON_SW_RESET_S2_CTL2_ADDR                                    (PON_BASE      + 0x00000063)
#define PMIO_PON_SW_RESET_S2_CTL2_OFFS                                    0x00000063
#define PMIO_PON_SW_RESET_S2_CTL2_RMSK                                          0x80
#define PMIO_PON_SW_RESET_S2_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL2_ADDR, PMIO_PON_SW_RESET_S2_CTL2_RMSK, 0, val)
#define PMIO_PON_SW_RESET_S2_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_SW_RESET_S2_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL2_ADDR, val)
#define PMIO_PON_SW_RESET_S2_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_SW_RESET_S2_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_SW_RESET_S2_CTL2_SW_RESET_EN_BMSK                              0x80
#define PMIO_PON_SW_RESET_S2_CTL2_SW_RESET_EN_SHFT                               0x7

#define PMIO_PON_SW_RESET_GO_ADDR                                         (PON_BASE      + 0x00000064)
#define PMIO_PON_SW_RESET_GO_OFFS                                         0x00000064
#define PMIO_PON_SW_RESET_GO_RMSK                                               0xff
#define PMIO_PON_SW_RESET_GO_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_SW_RESET_GO_ADDR, val)
#define PMIO_PON_SW_RESET_GO_SW_RESET_GO_BMSK                                   0xff
#define PMIO_PON_SW_RESET_GO_SW_RESET_GO_SHFT                                    0x0

#define PMIO_PON_OVERTEMP_RESET_CTL_ADDR                                  (PON_BASE      + 0x00000066)
#define PMIO_PON_OVERTEMP_RESET_CTL_OFFS                                  0x00000066
#define PMIO_PON_OVERTEMP_RESET_CTL_RMSK                                         0xf
#define PMIO_PON_OVERTEMP_RESET_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL_ADDR, PMIO_PON_OVERTEMP_RESET_CTL_RMSK, 0, val)
#define PMIO_PON_OVERTEMP_RESET_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_OVERTEMP_RESET_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL_ADDR, val)
#define PMIO_PON_OVERTEMP_RESET_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_OVERTEMP_RESET_CTL_RESET_TYPE_BMSK                              0xf
#define PMIO_PON_OVERTEMP_RESET_CTL_RESET_TYPE_SHFT                              0x0

#define PMIO_PON_OVERTEMP_RESET_CTL2_ADDR                                 (PON_BASE      + 0x00000067)
#define PMIO_PON_OVERTEMP_RESET_CTL2_OFFS                                 0x00000067
#define PMIO_PON_OVERTEMP_RESET_CTL2_RMSK                                       0x80
#define PMIO_PON_OVERTEMP_RESET_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL2_ADDR, PMIO_PON_OVERTEMP_RESET_CTL2_RMSK, 0, val)
#define PMIO_PON_OVERTEMP_RESET_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_OVERTEMP_RESET_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL2_ADDR, val)
#define PMIO_PON_OVERTEMP_RESET_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_OVERTEMP_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_OVERTEMP_RESET_CTL2_S2_RESET_EN_BMSK                           0x80
#define PMIO_PON_OVERTEMP_RESET_CTL2_S2_RESET_EN_SHFT                            0x7

#define PMIO_PON_TFT_RESET_CTL_ADDR                                       (PON_BASE      + 0x0000006a)
#define PMIO_PON_TFT_RESET_CTL_OFFS                                       0x0000006a
#define PMIO_PON_TFT_RESET_CTL_RMSK                                              0xf
#define PMIO_PON_TFT_RESET_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL_ADDR, PMIO_PON_TFT_RESET_CTL_RMSK, 0, val)
#define PMIO_PON_TFT_RESET_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_TFT_RESET_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_TFT_RESET_CTL_ADDR, val)
#define PMIO_PON_TFT_RESET_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL_ADDR, mask, shift, val)
#define PMIO_PON_TFT_RESET_CTL_RESET_TYPE_BMSK                                   0xf
#define PMIO_PON_TFT_RESET_CTL_RESET_TYPE_SHFT                                   0x0

#define PMIO_PON_TFT_RESET_CTL2_ADDR                                      (PON_BASE      + 0x0000006b)
#define PMIO_PON_TFT_RESET_CTL2_OFFS                                      0x0000006b
#define PMIO_PON_TFT_RESET_CTL2_RMSK                                            0x80
#define PMIO_PON_TFT_RESET_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL2_ADDR, PMIO_PON_TFT_RESET_CTL2_RMSK, 0, val)
#define PMIO_PON_TFT_RESET_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_TFT_RESET_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_TFT_RESET_CTL2_ADDR, val)
#define PMIO_PON_TFT_RESET_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_TFT_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PON_TFT_RESET_CTL2_S2_RESET_EN_BMSK                                0x80
#define PMIO_PON_TFT_RESET_CTL2_S2_RESET_EN_SHFT                                 0x7

#define PMIO_PON_PULL_CTL_ADDR                                            (PON_BASE      + 0x00000070)
#define PMIO_PON_PULL_CTL_OFFS                                            0x00000070
#define PMIO_PON_PULL_CTL_RMSK                                                   0xf
#define PMIO_PON_PULL_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PULL_CTL_ADDR, PMIO_PON_PULL_CTL_RMSK, 0, val)
#define PMIO_PON_PULL_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PULL_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PULL_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PULL_CTL_ADDR, val)
#define PMIO_PON_PULL_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PULL_CTL_ADDR, mask, shift, val)
#define PMIO_PON_PULL_CTL_PON1_PD_EN_BMSK                                        0x8
#define PMIO_PON_PULL_CTL_PON1_PD_EN_SHFT                                        0x3
#define PMIO_PON_PULL_CTL_CBLPWR_N_PU_EN_BMSK                                    0x4
#define PMIO_PON_PULL_CTL_CBLPWR_N_PU_EN_SHFT                                    0x2
#define PMIO_PON_PULL_CTL_KPDPWR_N_PU_EN_BMSK                                    0x2
#define PMIO_PON_PULL_CTL_KPDPWR_N_PU_EN_SHFT                                    0x1
#define PMIO_PON_PULL_CTL_RESIN_N_PU_EN_BMSK                                     0x1
#define PMIO_PON_PULL_CTL_RESIN_N_PU_EN_SHFT                                     0x0

#define PMIO_PON_DEBOUNCE_CTL_ADDR                                        (PON_BASE      + 0x00000071)
#define PMIO_PON_DEBOUNCE_CTL_OFFS                                        0x00000071
#define PMIO_PON_DEBOUNCE_CTL_RMSK                                               0x7
#define PMIO_PON_DEBOUNCE_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_DEBOUNCE_CTL_ADDR, PMIO_PON_DEBOUNCE_CTL_RMSK, 0, val)
#define PMIO_PON_DEBOUNCE_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_DEBOUNCE_CTL_ADDR, mask, shift, val)
#define PMIO_PON_DEBOUNCE_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_DEBOUNCE_CTL_ADDR, val)
#define PMIO_PON_DEBOUNCE_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_DEBOUNCE_CTL_ADDR, mask, shift, val)
#define PMIO_PON_DEBOUNCE_CTL_DEBOUNCE_BMSK                                      0xF
#define PMIO_PON_DEBOUNCE_CTL_DEBOUNCE_SHFT                                      0x0

#define PMIO_PON_RESET_S3_SRC_ADDR                                      (PON_BASE      + 0x00000074)
#define PMIO_PON_RESET_S3_SRC_OFFS                                      0x00000074
#define PMIO_PON_RESET_S3_SRC_RMSK                                             0x3
#define PMIO_PON_RESET_S3_SRC_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_SRC_ADDR, PMIO_PON_RESET_S3_SRC_RMSK, 0, val)
#define PMIO_PON_RESET_S3_SRC_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_SRC_ADDR, mask, shift, val)
#define PMIO_PON_RESET_S3_SRC_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESET_S3_SRC_ADDR, val)
#define PMIO_PON_RESET_S3_SRC_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_SRC_ADDR, mask, shift, val)
#define PMIO_PON_RESET_S3_SRC_S3_SRC_BMSK                                    0x3
#define PMIO_PON_RESET_S3_SRC_S3_SRC_SHFT                                    0x0

#define PMIO_PON_RESET_S3_TIMER_ADDR                                      (PON_BASE      + 0x00000075)
#define PMIO_PON_RESET_S3_TIMER_OFFS                                      0x00000075
#define PMIO_PON_RESET_S3_TIMER_RMSK                                             0x7
#define PMIO_PON_RESET_S3_TIMER_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_TIMER_ADDR, PMIO_PON_RESET_S3_TIMER_RMSK, 0, val)
#define PMIO_PON_RESET_S3_TIMER_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESET_S3_TIMER_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_RESET_S3_TIMER_ADDR, val)
#define PMIO_PON_RESET_S3_TIMER_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_RESET_S3_TIMER_ADDR, mask, shift, val)
#define PMIO_PON_RESET_S3_TIMER_S3_TIMER_BMSK                                    0x7
#define PMIO_PON_RESET_S3_TIMER_S3_TIMER_SHFT                                    0x0

#define PMIO_PON_PON_TRIGGER_EN_ADDR                                      (PON_BASE      + 0x00000080)
#define PMIO_PON_PON_TRIGGER_EN_OFFS                                      0x00000080
#define PMIO_PON_PON_TRIGGER_EN_RMSK                                            0xfe
#define PMIO_PON_PON_TRIGGER_EN_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_TRIGGER_EN_ADDR, PMIO_PON_PON_TRIGGER_EN_RMSK, 0, val)
#define PMIO_PON_PON_TRIGGER_EN_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON_TRIGGER_EN_ADDR, mask, shift, val)
#define PMIO_PON_PON_TRIGGER_EN_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PON_TRIGGER_EN_ADDR, val)
#define PMIO_PON_PON_TRIGGER_EN_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PON_TRIGGER_EN_ADDR, mask, shift, val)
#define PMIO_PON_PON_TRIGGER_EN_KPDPWR_N_BMSK                                   0x80
#define PMIO_PON_PON_TRIGGER_EN_KPDPWR_N_SHFT                                    0x7
#define PMIO_PON_PON_TRIGGER_EN_CBLPWR_N_BMSK                                   0x40
#define PMIO_PON_PON_TRIGGER_EN_CBLPWR_N_SHFT                                    0x6
#define PMIO_PON_PON_TRIGGER_EN_PON1_BMSK                                       0x20
#define PMIO_PON_PON_TRIGGER_EN_PON1_SHFT                                        0x5
#define PMIO_PON_PON_TRIGGER_EN_USB_CHG_BMSK                                    0x10
#define PMIO_PON_PON_TRIGGER_EN_USB_CHG_SHFT                                     0x4
#define PMIO_PON_PON_TRIGGER_EN_DC_CHG_BMSK                                      0x8
#define PMIO_PON_PON_TRIGGER_EN_DC_CHG_SHFT                                      0x3
#define PMIO_PON_PON_TRIGGER_EN_RTC_BMSK                                         0x4
#define PMIO_PON_PON_TRIGGER_EN_RTC_SHFT                                         0x2
#define PMIO_PON_PON_TRIGGER_EN_SMPL_BMSK                                        0x2
#define PMIO_PON_PON_TRIGGER_EN_SMPL_SHFT                                        0x1


#define PMIO_PON_OPTION_BIT_ADDR                                         (PON_BASE      + 0x00000082)
#define PMIO_PON_OPTION_BIT_OFFS                                          0x00000082
#define PMIO_PON_OPTION_BIT_RMSK                                                0x3e
#define PMIO_PON_OPTION_BIT_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OPTION_BIT_ADDR, PMIO_PON_OPTION_BIT_RMSK, 0, val)
#define PMIO_PON_OPTION_BIT_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_OPTION_BIT_ADDR, mask, shift, val)
#define PMIO_PON_OPTION_BIT_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_OPTION_BIT_ADDR, val)
#define PMIO_PON_OPTION_BIT_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_OPTION_BIT_ADDR, mask, shift, val)
#define PMIO_PON_OPTION_BIT_KPDPWR_FEDGE_PON_BMSK                                0x20
#define PMIO_PON_OPTION_BIT_KPDPWR_FEDGE_PON_SHFT                                 0x5
#define PMIO_PON_OPTION_BIT_PON1_REDGE_PON_BMSK                                  0x10
#define PMIO_PON_OPTION_BIT_PON1_REDGE_PON_SHFT                                   0x4
#define PMIO_PON_OPTION_BIT_CBLPWR_FEDGE_PON_BMSK                                 0x8
#define PMIO_PON_OPTION_BIT_CBLPWR_FEDGE_PON_SHFT                                 0x3
#define PMIO_PON_OPTION_BIT_USB_CHG_REDGE_PON_BMSK                                0x4
#define PMIO_PON_OPTION_BIT_USB_CHG_REDGE_PON_SHFT                                0x2
#define PMIO_PON_OPTION_BIT_DC_CHG_REDGE_PON_BMSK                                 0x2
#define PMIO_PON_OPTION_BIT_DC_CHG_REDGE_PON_SHFT                                 0x1


#define PMIO_PON_WATCHDOG_LOCK_ADDR                                       (PON_BASE      + 0x00000083)
#define PMIO_PON_WATCHDOG_LOCK_OFFS                                       0x00000083
#define PMIO_PON_WATCHDOG_LOCK_RMSK                                             0x80
#define PMIO_PON_WATCHDOG_LOCK_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WATCHDOG_LOCK_ADDR, PMIO_PON_WATCHDOG_LOCK_RMSK, 0, val)
#define PMIO_PON_WATCHDOG_LOCK_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_WATCHDOG_LOCK_ADDR, mask, shift, val)
#define PMIO_PON_WATCHDOG_LOCK_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_WATCHDOG_LOCK_ADDR, val)
#define PMIO_PON_WATCHDOG_LOCK_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_WATCHDOG_LOCK_ADDR, mask, shift, val)
#define PMIO_PON_WATCHDOG_LOCK_PMIC_WD_LOCK_BMSK                                0x80
#define PMIO_PON_WATCHDOG_LOCK_PMIC_WD_LOCK_SHFT                                 0x7

#define PMIO_PON_UVLO_ADDR                                                (PON_BASE      + 0x00000088)
#define PMIO_PON_UVLO_OFFS                                                0x00000088
#define PMIO_PON_UVLO_RMSK                                                       0x7
#define PMIO_PON_UVLO_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_UVLO_ADDR, PMIO_PON_UVLO_RMSK, 0, val)
#define PMIO_PON_UVLO_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_UVLO_ADDR, mask, shift, val)
#define PMIO_PON_UVLO_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_UVLO_ADDR, val)
#define PMIO_PON_UVLO_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_UVLO_ADDR, mask, shift, val)
#define PMIO_PON_UVLO_UVLO_DLY_BMSK                                              0x7
#define PMIO_PON_UVLO_UVLO_DLY_SHFT                                              0x0

#define PMIO_PON_AVDD_VPH_ADDR                                            (PON_BASE      + 0x0000008a)
#define PMIO_PON_AVDD_VPH_OFFS                                            0x0000008a
#define PMIO_PON_AVDD_VPH_RMSK                                                  0x30
#define PMIO_PON_AVDD_VPH_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_AVDD_VPH_ADDR, PMIO_PON_AVDD_VPH_RMSK, 0, val)
#define PMIO_PON_AVDD_VPH_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_AVDD_VPH_ADDR, mask, shift, val)
#define PMIO_PON_AVDD_VPH_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_AVDD_VPH_ADDR, val)
#define PMIO_PON_AVDD_VPH_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_AVDD_VPH_ADDR, mask, shift, val)
#define PMIO_PON_AVDD_VPH_AVDD_HPM_EN_BMSK                                      0x20
#define PMIO_PON_AVDD_VPH_AVDD_HPM_EN_SHFT                                       0x5
#define PMIO_PON_AVDD_VPH_AVDD_REF_OVR_BMSK                                     0x10
#define PMIO_PON_AVDD_VPH_AVDD_REF_OVR_SHFT                                      0x4

#define PMIO_PON_PERPH_RB_SPARE_ADDR                                      (PON_BASE      + 0x0000008c)
#define PMIO_PON_PERPH_RB_SPARE_OFFS                                      0x0000008c
#define PMIO_PON_PERPH_RB_SPARE_RMSK                                            0xff
#define PMIO_PON_PERPH_RB_SPARE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_RB_SPARE_ADDR, PMIO_PON_PERPH_RB_SPARE_RMSK, 0, val)
#define PMIO_PON_PERPH_RB_SPARE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PERPH_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_PERPH_RB_SPARE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PERPH_RB_SPARE_ADDR, val)
#define PMIO_PON_PERPH_RB_SPARE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PERPH_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_PERPH_RB_SPARE_SPARE_BMSK                                      0xff
#define PMIO_PON_PERPH_RB_SPARE_SPARE_SHFT                                       0x0

#define PMIO_PON_DVDD_RB_SPARE_ADDR                                       (PON_BASE      + 0x0000008d)
#define PMIO_PON_DVDD_RB_SPARE_OFFS                                       0x0000008d
#define PMIO_PON_DVDD_RB_SPARE_RMSK                                             0xff
#define PMIO_PON_DVDD_RB_SPARE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_DVDD_RB_SPARE_ADDR, PMIO_PON_DVDD_RB_SPARE_RMSK, 0, val)
#define PMIO_PON_DVDD_RB_SPARE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_DVDD_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_DVDD_RB_SPARE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_DVDD_RB_SPARE_ADDR, val)
#define PMIO_PON_DVDD_RB_SPARE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_DVDD_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_DVDD_RB_SPARE_SPARE_BMSK                                       0xff
#define PMIO_PON_DVDD_RB_SPARE_SPARE_SHFT                                        0x0

#define PMIO_PON_XVDD_RB_SPARE_ADDR                                       (PON_BASE      + 0x0000008e)
#define PMIO_PON_XVDD_RB_SPARE_OFFS                                       0x0000008e
#define PMIO_PON_XVDD_RB_SPARE_RMSK                                             0xff
#define PMIO_PON_XVDD_RB_SPARE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_XVDD_RB_SPARE_ADDR, PMIO_PON_XVDD_RB_SPARE_RMSK, 0, val)
#define PMIO_PON_XVDD_RB_SPARE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_XVDD_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_XVDD_RB_SPARE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_XVDD_RB_SPARE_ADDR, val)
#define PMIO_PON_XVDD_RB_SPARE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_XVDD_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_XVDD_RB_SPARE_SPARE_BMSK                                       0xff
#define PMIO_PON_XVDD_RB_SPARE_SPARE_SHFT                                        0x0

#define PMIO_PON_SOFT_RB_SPARE_ADDR                                       (PON_BASE      + 0x0000008f)
#define PMIO_PON_SOFT_RB_SPARE_OFFS                                       0x0000008f
#define PMIO_PON_SOFT_RB_SPARE_RMSK                                             0xff
#define PMIO_PON_SOFT_RB_SPARE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RB_SPARE_ADDR, PMIO_PON_SOFT_RB_SPARE_RMSK, 0, val)
#define PMIO_PON_SOFT_RB_SPARE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_SOFT_RB_SPARE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_SOFT_RB_SPARE_ADDR, val)
#define PMIO_PON_SOFT_RB_SPARE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_SOFT_RB_SPARE_ADDR, mask, shift, val)
#define PMIO_PON_SOFT_RB_SPARE_SPARE_BMSK                                       0xff
#define PMIO_PON_SOFT_RB_SPARE_SPARE_SHFT                                        0x0

#define PMIO_PON_PON1_INTERFACE_ADDR                                      (PON_BASE      + 0x00000090)
#define PMIO_PON_PON1_INTERFACE_OFFS                                      0x00000090
#define PMIO_PON_PON1_INTERFACE_RMSK                                            0x80
#define PMIO_PON_PON1_INTERFACE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON1_INTERFACE_ADDR, PMIO_PON_PON1_INTERFACE_RMSK, 0, val)
#define PMIO_PON_PON1_INTERFACE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PON1_INTERFACE_ADDR, mask, shift, val)
#define PMIO_PON_PON1_INTERFACE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PON1_INTERFACE_ADDR, val)
#define PMIO_PON_PON1_INTERFACE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PON1_INTERFACE_ADDR, mask, shift, val)
#define PMIO_PON_PON1_INTERFACE_PON_OUT_BMSK                                    0x80
#define PMIO_PON_PON1_INTERFACE_PON_OUT_SHFT                                     0x7

#define PMIO_PON_PBS_INTERFACE_ADDR                                       (PON_BASE      + 0x00000091)
#define PMIO_PON_PBS_INTERFACE_OFFS                                       0x00000091
#define PMIO_PON_PBS_INTERFACE_RMSK                                             0x40
#define PMIO_PON_PBS_INTERFACE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PBS_INTERFACE_ADDR, PMIO_PON_PBS_INTERFACE_RMSK, 0, val)
#define PMIO_PON_PBS_INTERFACE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_PBS_INTERFACE_ADDR, mask, shift, val)
#define PMIO_PON_PBS_INTERFACE_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_PBS_INTERFACE_ADDR, val)
#define PMIO_PON_PBS_INTERFACE_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_PBS_INTERFACE_ADDR, mask, shift, val)
#define PMIO_PON_PBS_INTERFACE_ACK_NACK_BMSK                                    0x40
#define PMIO_PON_PBS_INTERFACE_ACK_NACK_SHFT                                     0x6

#define PMIO_PON_FSM_CTL_ADDR                                             (PON_BASE      + 0x00000094)
#define PMIO_PON_FSM_CTL_OFFS                                             0x00000094
#define PMIO_PON_FSM_CTL_RMSK                                                    0xf
#define PMIO_PON_FSM_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_FSM_CTL_ADDR, PMIO_PON_FSM_CTL_RMSK, 0, val)
#define PMIO_PON_FSM_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_FSM_CTL_ADDR, mask, shift, val)
#define PMIO_PON_FSM_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PON_FSM_CTL_ADDR, val)
#define PMIO_PON_FSM_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PON_FSM_CTL_ADDR, mask, shift, val)
#define PMIO_PON_FSM_CTL_SEL_FSM_BMSK                                            0xf
#define PMIO_PON_FSM_CTL_SEL_FSM_SHFT                                            0x0

#define PMIO_PON_FSM_STATUS_ADDR                                          (PON_BASE      + 0x00000095)
#define PMIO_PON_FSM_STATUS_OFFS                                          0x00000095
#define PMIO_PON_FSM_STATUS_RMSK                                                 0xf
#define PMIO_PON_FSM_STATUS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_FSM_STATUS_ADDR, PMIO_PON_FSM_STATUS_RMSK, 0, val)
#define PMIO_PON_FSM_STATUS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PON_FSM_STATUS_ADDR, mask, shift, val)
#define PMIO_PON_FSM_STATUS_FSM_STATE_BMSK                                       0xf
#define PMIO_PON_FSM_STATUS_FSM_STATE_SHFT                                       0x0

#define PMIO_PON_SEC_ACCESS_ADDR                                           (PON_BASE      + 0x000000D0)
#define PMIO_PON_SEC_ACCESS_RMSK                                           0xFF
#define PMIO_PON_UVLO_THRESH_ADDR                                          (PON_BASE      + 0x000000F2)
#define PMIO_PON_UVLO_THRESH_RMSK                                           0xF8
#define PMIO_PON_UVLO_VOLT_HYST_ADDR                                       (PON_BASE      + 0x000000F2)
#define PMIO_PON_UVLO_VOLT_HYST_RMSK                                        0x06 
#define PMIO_PON_PERPH_RESET_CTL3                                          (PON_BASE      + 0x000000DA)
#define PMIO_PON_FOLLOW_WARM_RB_RMSK                                        0x4

//PON GEN2
#define PMIO_PON_GEN2_DEBOUNCE_CTL_DEBOUNCE_BMSK                               0x0F
#define PMIO_PON_GEN2_FSM_STATUS_ADDR                                         (PON_BASE      + 0x00000008)
#define PMIO_PON_GEN2_FSM_STATUS_BMSK                                          0x07

#define PMIO_PON_UVLO_CTL                                                    (PON_BASE      + 0x00000084)
#define PMIO_PON_UVLO_ENABLE_PON_SHIFT                                        7
#define PMIO_PON_UVLO_HYST_DATA_PON_SHIFT                                     5

#define PMIO_PON_GEN2_PON_REASON1_ADDR                                        (PON_BASE      + 0x000000C0)
#define PMIO_PON_GEN2_WARM_RESET_REASON1_ADDR                                 (PON_BASE      + 0x000000C2)
#define PMIO_PON_GEN2_ON_REASON_ADDR                                          (PON_BASE      + 0x000000C4)
#define PMIO_PON_GEN2_POFF_REASON1_ADDR                                       (PON_BASE      + 0x000000C5)
#define PMIO_PON_GEN2_OFF_REASON_ADDR                                         (PON_BASE      + 0x000000C7)
#define PMIO_PON_GEN2_FAULT_REASON1_ADDR                                      (PON_BASE      + 0x000000C8)
#define PMIO_PON_GEN2_FAULT_REASON2_ADDR                                      (PON_BASE      + 0x000000C9)
#define PMIO_PON_GEN2_S3_RESET_REASON_ADDR                                    (PON_BASE      + 0x000000CA)
#define PMIO_PON_GEN2_SOFT_RESET_REASON1_ADDR                                 (PON_BASE      + 0x000000CB)


//#define PMIO_PON_GEN2_ON_REASON_ADDR              (PON_BASE      + 0x000000C4)
#define PMIO_PON_GEN2_ON_REASON_PON_SEQ_BMSK        0x80
#define PMIO_PON_GEN2_ON_REASON_PON_SEQ_SHFT        0x7
#define PMIO_PON_GEN2_ON_REASON_WARM_SEQ_BMSK       0x40
#define PMIO_PON_GEN2_ON_REASON_WARM_SEQ_SHFT       0x6


//#define PMIO_PON_GEN2_POFF_REASON1_ADDR                     (PON_BASE      + 0x000000C5)
#define PMIO_PON_GEN2_POFF_REASON1_KPDPWR_N_BMSK              0x80
#define PMIO_PON_GEN2_POFF_REASON1_KPDPWR_N_SHFT              0x7
#define PMIO_PON_GEN2_POFF_REASON1_RESIN_N_BMSK               0x40
#define PMIO_PON_GEN2_POFF_REASON1_RESIN_N_SHFT               0x6
#define PMIO_PON_GEN2_POFF_REASON1_KPDPWR_AND_RESIN_BMSK      0x20
#define PMIO_PON_GEN2_POFF_REASON1_KPDPWR_AND_RESIN_SHFT      0x5
#define PMIO_PON_GEN2_POFF_REASON1_GP2_BMSK                   0x10
#define PMIO_PON_GEN2_POFF_REASON1_GP2_SHFT                   0x4
#define PMIO_PON_GEN2_POFF_REASON1_GP1_BMSK                   0x8
#define PMIO_PON_GEN2_POFF_REASON1_GP1_SHFT                   0x3
#define PMIO_PON_GEN2_POFF_REASON1_PMIC_WD_BMSK               0x4
#define PMIO_PON_GEN2_POFF_REASON1_PMIC_WD_SHFT               0x2
#define PMIO_PON_GEN2_POFF_REASON1_PS_HOLD_BMSK               0x2
#define PMIO_PON_GEN2_POFF_REASON1_PS_HOLD_SHFT               0x1
#define PMIO_PON_GEN2_POFF_REASON1_SOFT_BMSK                  0x1
#define PMIO_PON_GEN2_POFF_REASON1_SOFT_SHFT                  0x0



//#define PMIO_PON_GEN2_OFF_REASON_ADDR                       (PON_BASE      + 0x000000C7)
#define PMIO_PON_GEN2_OFF_REASON_POFF_SEQ_BMSK                0x80
#define PMIO_PON_GEN2_OFF_REASON_POFF_SEQ_SHFT                0x7
#define PMIO_PON_GEN2_OFF_REASON_FAULT_SEQ_BMSK               0x40
#define PMIO_PON_GEN2_OFF_REASON_FAULT_SEQ_SHFT               0x6
#define PMIO_PON_GEN2_OFF_REASON_S3_RESET_BMSK                0x20
#define PMIO_PON_GEN2_OFF_REASON_S3_RESET_SHFT                0x5
#define PMIO_PON_GEN2_OFF_REASON_IMM_XVDD_SD_BMSK             0x10
#define PMIO_PON_GEN2_OFF_REASON_IMM_XVDD_SD_SHFT             0x4
#define PMIO_PON_GEN2_OFF_REASON_RAW_DVDD_SHD_BMSK            0x8
#define PMIO_PON_GEN2_OFF_REASON_RAW_DVDD_SHD_SHFT            0x3
#define PMIO_PON_GEN2_OFF_REASON_RAW_XVDD_SHD_BMSK            0x4
#define PMIO_PON_GEN2_OFF_REASON_RAW_XVDD_SHD_SHFT            0x2



//PON_FAULT_REASON1 : 0x000008C8
//#define PMIO_PON_GEN2_FAULT_REASON1_ADDR              (PON_BASE      + 0x000000C8)
#define PMIO_PON_GEN2_FAULT_REASON1_AVDD_RB_BMSK        0x80
#define PMIO_PON_GEN2_FAULT_REASON1_AVDD_RB_SHFT        0x7
#define PMIO_PON_GEN2_FAULT_REASON1_UVLO_BMSK           0x40
#define PMIO_PON_GEN2_FAULT_REASON1_UVLO_SHFT           0x6
#define PMIO_PON_GEN2_FAULT_REASON1_OVLO_BMSK           0x20
#define PMIO_PON_GEN2_FAULT_REASON1_OVLO_SHFT           0x5
#define PMIO_PON_GEN2_FAULT_REASON1_MBG_FAULT_BMSK      0x10
#define PMIO_PON_GEN2_FAULT_REASON1_MBG_FAULT_SHFT      0x4
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT3_BMSK      0x8
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT3_SHFT      0x3
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT2_BMSK      0x4
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT2_SHFT      0x2
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT1_BMSK      0x2
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT1_SHFT      0x1
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT0_BMSK      0x1
#define PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT0_SHFT      0x0


//#define PMIO_PON_GEN2_FAULT_REASON2_ADDR              (PON_BASE      + 0x000000C9)
#define PMIO_PON_GEN2_FAULT_REASON2_OTST3_BMSK          0x80
#define PMIO_PON_GEN2_FAULT_REASON2_OTST3_SHFT          0x7
#define PMIO_PON_GEN2_FAULT_REASON2_RESTART_PON_BMSK    0x40
#define PMIO_PON_GEN2_FAULT_REASON2_RESTART_PON_SHFT    0x6
#define PMIO_PON_GEN2_FAULT_REASON2_PBS_NACK_BMSK       0x20
#define PMIO_PON_GEN2_FAULT_REASON2_PBS_NACK_SHFT       0x5
#define PMIO_PON_GEN2_FAULT_REASON2_PBS_WD_TO_BMSK      0x10
#define PMIO_PON_GEN2_FAULT_REASON2_PBS_WD_TO_SHFT      0x4
#define PMIO_PON_GEN2_FAULT_REASON2_FAULT_N_BMSK        0x8
#define PMIO_PON_GEN2_FAULT_REASON2_FAULT_N_SHFT        0x3



//#define PMIO_PON_GEN2_S3_RESET_REASON_ADDR                        (PON_BASE      + 0x000000CA)
#define PMIO_PON_GEN2_S3_RESET_REASON_KPDPWR_ANDOR_RESIN_BMSK       0x80
#define PMIO_PON_GEN2_S3_RESET_REASON_KPDPWR_ANDOR_RESIN_SHFT       0x7
#define PMIO_PON_GEN2_S3_RESET_REASON_PBS_NACK_BMSK                 0x40
#define PMIO_PON_GEN2_S3_RESET_REASON_PBS_NACK_SHFT                 0x6
#define PMIO_PON_GEN2_S3_RESET_REASON_PBS_WD_TO_BMSK                0x20
#define PMIO_PON_GEN2_S3_RESET_REASON_PBS_WD_TO_SHFT                0x5
#define PMIO_PON_GEN2_S3_RESET_REASON_FAULT_N_BMSK                  0x10
#define PMIO_PON_GEN2_S3_RESET_REASON_FAULT_N_SHFT                  0x4


//#define PMIO_PON_GEN2_SOFT_RESET_REASON1_ADDR            (PON_BASE      + 0x000000CB)
#define PMIO_PON_GEN2_SOFT_RESET_REASON1_SOFT_BMSK         0x1
#define PMIO_PON_GEN2_SOFT_RESET_REASON1_SOFT_SHFT         0x0


#endif //__PMIO_PON_H__