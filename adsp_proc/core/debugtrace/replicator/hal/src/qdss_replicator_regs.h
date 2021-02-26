#ifndef __QDSS_REPLICATOR_REGS_H__
#define __QDSS_REPLICATOR_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_replicator_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/replicator/hal/src/qdss_replicator_regs.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP
 *--------------------------------------------------------------------------*/

extern volatile uint32  QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE;


#define HWIO_QDSS_REPL64_IDFILTER0_ADDR                            (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000000)
#define HWIO_QDSS_REPL64_IDFILTER0_RMSK                                  0xff
#define HWIO_QDSS_REPL64_IDFILTER0_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_IDFILTER0_ADDR, HWIO_QDSS_REPL64_IDFILTER0_RMSK)
#define HWIO_QDSS_REPL64_IDFILTER0_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_IDFILTER0_ADDR, m)
#define HWIO_QDSS_REPL64_IDFILTER0_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_IDFILTER0_ADDR,v)
#define HWIO_QDSS_REPL64_IDFILTER0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_IDFILTER0_ADDR,m,v,HWIO_QDSS_REPL64_IDFILTER0_IN)
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_70_7F_BMSK                        0x80
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_70_7F_SHFT                         0x7
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_60_6F_BMSK                        0x40
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_60_6F_SHFT                         0x6
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_50_5F_BMSK                        0x20
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_50_5F_SHFT                         0x5
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_40_4F_BMSK                        0x10
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_40_4F_SHFT                         0x4
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_30_3F_BMSK                         0x8
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_30_3F_SHFT                         0x3
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_20_2F_BMSK                         0x4
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_20_2F_SHFT                         0x2
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_10_1F_BMSK                         0x2
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_10_1F_SHFT                         0x1
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_0_F_BMSK                           0x1
#define HWIO_QDSS_REPL64_IDFILTER0_ID0_0_F_SHFT                           0x0

#define HWIO_QDSS_REPL64_IDFILTER1_ADDR                            (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000004)
#define HWIO_QDSS_REPL64_IDFILTER1_RMSK                                  0xff
#define HWIO_QDSS_REPL64_IDFILTER1_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_IDFILTER1_ADDR, HWIO_QDSS_REPL64_IDFILTER1_RMSK)
#define HWIO_QDSS_REPL64_IDFILTER1_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_IDFILTER1_ADDR, m)
#define HWIO_QDSS_REPL64_IDFILTER1_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_IDFILTER1_ADDR,v)
#define HWIO_QDSS_REPL64_IDFILTER1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_IDFILTER1_ADDR,m,v,HWIO_QDSS_REPL64_IDFILTER1_IN)
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_70_7F_BMSK                        0x80
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_70_7F_SHFT                         0x7
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_60_6F_BMSK                        0x40
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_60_6F_SHFT                         0x6
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_50_5F_BMSK                        0x20
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_50_5F_SHFT                         0x5
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_40_4F_BMSK                        0x10
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_40_4F_SHFT                         0x4
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_30_3F_BMSK                         0x8
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_30_3F_SHFT                         0x3
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_20_2F_BMSK                         0x4
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_20_2F_SHFT                         0x2
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_10_1F_BMSK                         0x2
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_10_1F_SHFT                         0x1
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_0_F_BMSK                           0x1
#define HWIO_QDSS_REPL64_IDFILTER1_ID1_0_F_SHFT                           0x0

#define HWIO_QDSS_REPL64_ITATBCTR0_ADDR                            (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000efc)
#define HWIO_QDSS_REPL64_ITATBCTR0_RMSK                                  0x15
#define HWIO_QDSS_REPL64_ITATBCTR0_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_ITATBCTR0_ADDR,v)
#define HWIO_QDSS_REPL64_ITATBCTR0_ATREADYS_W_BMSK                       0x10
#define HWIO_QDSS_REPL64_ITATBCTR0_ATREADYS_W_SHFT                        0x4
#define HWIO_QDSS_REPL64_ITATBCTR0_ATVALIDM1_W_BMSK                       0x4
#define HWIO_QDSS_REPL64_ITATBCTR0_ATVALIDM1_W_SHFT                       0x2
#define HWIO_QDSS_REPL64_ITATBCTR0_ATVALIDM0_W_BMSK                       0x1
#define HWIO_QDSS_REPL64_ITATBCTR0_ATVALIDM0_W_SHFT                       0x0

#define HWIO_QDSS_REPL64_ITATBCTR1_ADDR                            (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_REPL64_ITATBCTR1_RMSK                                   0xb
#define HWIO_QDSS_REPL64_ITATBCTR1_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_ITATBCTR1_ADDR, HWIO_QDSS_REPL64_ITATBCTR1_RMSK)
#define HWIO_QDSS_REPL64_ITATBCTR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_ITATBCTR1_ADDR, m)
#define HWIO_QDSS_REPL64_ITATBCTR1_ATVALIDS_R_BMSK                        0x8
#define HWIO_QDSS_REPL64_ITATBCTR1_ATVALIDS_R_SHFT                        0x3
#define HWIO_QDSS_REPL64_ITATBCTR1_ATREADYM1_R_BMSK                       0x2
#define HWIO_QDSS_REPL64_ITATBCTR1_ATREADYM1_R_SHFT                       0x1
#define HWIO_QDSS_REPL64_ITATBCTR1_ATREADYM0_R_BMSK                       0x1
#define HWIO_QDSS_REPL64_ITATBCTR1_ATREADYM0_R_SHFT                       0x0

#define HWIO_QDSS_REPL64_ITCTRL_ADDR                               (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_REPL64_ITCTRL_RMSK                                      0x1
#define HWIO_QDSS_REPL64_ITCTRL_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_ITCTRL_ADDR, HWIO_QDSS_REPL64_ITCTRL_RMSK)
#define HWIO_QDSS_REPL64_ITCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_ITCTRL_ADDR, m)
#define HWIO_QDSS_REPL64_ITCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_ITCTRL_ADDR,v)
#define HWIO_QDSS_REPL64_ITCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_ITCTRL_ADDR,m,v,HWIO_QDSS_REPL64_ITCTRL_IN)
#define HWIO_QDSS_REPL64_ITCTRL_INTEGRATION_MODE_BMSK                     0x1
#define HWIO_QDSS_REPL64_ITCTRL_INTEGRATION_MODE_SHFT                     0x0

#define HWIO_QDSS_REPL64_CLAIMSET_ADDR                             (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_REPL64_CLAIMSET_RMSK                                    0xf
#define HWIO_QDSS_REPL64_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CLAIMSET_ADDR, HWIO_QDSS_REPL64_CLAIMSET_RMSK)
#define HWIO_QDSS_REPL64_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CLAIMSET_ADDR, m)
#define HWIO_QDSS_REPL64_CLAIMSET_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_CLAIMSET_ADDR,v)
#define HWIO_QDSS_REPL64_CLAIMSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_CLAIMSET_ADDR,m,v,HWIO_QDSS_REPL64_CLAIMSET_IN)
#define HWIO_QDSS_REPL64_CLAIMSET_CLAIMSET_W_BMSK                         0xf
#define HWIO_QDSS_REPL64_CLAIMSET_CLAIMSET_W_SHFT                         0x0
#define HWIO_QDSS_REPL64_CLAIMSET_CLAIMSET_R_BMSK                         0xf
#define HWIO_QDSS_REPL64_CLAIMSET_CLAIMSET_R_SHFT                         0x0

#define HWIO_QDSS_REPL64_CLAIMCLR_ADDR                             (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_REPL64_CLAIMCLR_RMSK                                    0xf
#define HWIO_QDSS_REPL64_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CLAIMCLR_ADDR, HWIO_QDSS_REPL64_CLAIMCLR_RMSK)
#define HWIO_QDSS_REPL64_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_REPL64_CLAIMCLR_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_CLAIMCLR_ADDR,v)
#define HWIO_QDSS_REPL64_CLAIMCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_CLAIMCLR_ADDR,m,v,HWIO_QDSS_REPL64_CLAIMCLR_IN)
#define HWIO_QDSS_REPL64_CLAIMCLR_CLAIMCLR_W_BMSK                         0xf
#define HWIO_QDSS_REPL64_CLAIMCLR_CLAIMCLR_W_SHFT                         0x0
#define HWIO_QDSS_REPL64_CLAIMCLR_CLAIMCLR_R_BMSK                         0xf
#define HWIO_QDSS_REPL64_CLAIMCLR_CLAIMCLR_R_SHFT                         0x0

#define HWIO_QDSS_REPL64_LAR_ADDR                                  (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_REPL64_LAR_RMSK                                  0xffffffff
#define HWIO_QDSS_REPL64_LAR_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_LAR_ADDR,v)
#define HWIO_QDSS_REPL64_LAR_ACCESS_W_BMSK                         0xffffffff
#define HWIO_QDSS_REPL64_LAR_ACCESS_W_SHFT                                0x0

#define HWIO_QDSS_REPL64_LSR_ADDR                                  (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_REPL64_LSR_RMSK                                         0x7
#define HWIO_QDSS_REPL64_LSR_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_LSR_ADDR, HWIO_QDSS_REPL64_LSR_RMSK)
#define HWIO_QDSS_REPL64_LSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_LSR_ADDR, m)
#define HWIO_QDSS_REPL64_LSR_LOCKTYPE_BMSK                                0x4
#define HWIO_QDSS_REPL64_LSR_LOCKTYPE_SHFT                                0x2
#define HWIO_QDSS_REPL64_LSR_LOCKGRANT_BMSK                               0x2
#define HWIO_QDSS_REPL64_LSR_LOCKGRANT_SHFT                               0x1
#define HWIO_QDSS_REPL64_LSR_LOCKEXIST_BMSK                               0x1
#define HWIO_QDSS_REPL64_LSR_LOCKEXIST_SHFT                               0x0

#define HWIO_QDSS_REPL64_AUTHSTATUS_ADDR                           (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_REPL64_AUTHSTATUS_RMSK                                 0xff
#define HWIO_QDSS_REPL64_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_AUTHSTATUS_ADDR, HWIO_QDSS_REPL64_AUTHSTATUS_RMSK)
#define HWIO_QDSS_REPL64_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_REPL64_AUTHSTATUS_SNID_BMSK                            0xc0
#define HWIO_QDSS_REPL64_AUTHSTATUS_SNID_SHFT                             0x6
#define HWIO_QDSS_REPL64_AUTHSTATUS_SID_BMSK                             0x30
#define HWIO_QDSS_REPL64_AUTHSTATUS_SID_SHFT                              0x4
#define HWIO_QDSS_REPL64_AUTHSTATUS_NSNID_BMSK                            0xc
#define HWIO_QDSS_REPL64_AUTHSTATUS_NSNID_SHFT                            0x2
#define HWIO_QDSS_REPL64_AUTHSTATUS_NSID_BMSK                             0x3
#define HWIO_QDSS_REPL64_AUTHSTATUS_NSID_SHFT                             0x0

#define HWIO_QDSS_REPL64_DEVID_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_REPL64_DEVID_RMSK                                       0xf
#define HWIO_QDSS_REPL64_DEVID_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_DEVID_ADDR, HWIO_QDSS_REPL64_DEVID_RMSK)
#define HWIO_QDSS_REPL64_DEVID_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_DEVID_ADDR, m)
#define HWIO_QDSS_REPL64_DEVID_PORTNUM_BMSK                               0xf
#define HWIO_QDSS_REPL64_DEVID_PORTNUM_SHFT                               0x0

#define HWIO_QDSS_REPL64_DEVTYPE_ADDR                              (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_REPL64_DEVTYPE_RMSK                                    0xff
#define HWIO_QDSS_REPL64_DEVTYPE_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_DEVTYPE_ADDR, HWIO_QDSS_REPL64_DEVTYPE_RMSK)
#define HWIO_QDSS_REPL64_DEVTYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_DEVTYPE_ADDR, m)
#define HWIO_QDSS_REPL64_DEVTYPE_SUB_TYPE_BMSK                           0xf0
#define HWIO_QDSS_REPL64_DEVTYPE_SUB_TYPE_SHFT                            0x4
#define HWIO_QDSS_REPL64_DEVTYPE_MAJOR_TYPE_BMSK                          0xf
#define HWIO_QDSS_REPL64_DEVTYPE_MAJOR_TYPE_SHFT                          0x0

#define HWIO_QDSS_REPL64_PIDR0_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_REPL64_PIDR0_RMSK                                      0xff
#define HWIO_QDSS_REPL64_PIDR0_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR0_ADDR, HWIO_QDSS_REPL64_PIDR0_RMSK)
#define HWIO_QDSS_REPL64_PIDR0_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR0_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR0_PART_NUMBER_BITS7TO0_BMSK                 0xff
#define HWIO_QDSS_REPL64_PIDR0_PART_NUMBER_BITS7TO0_SHFT                  0x0

#define HWIO_QDSS_REPL64_PIDR1_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_REPL64_PIDR1_RMSK                                      0xff
#define HWIO_QDSS_REPL64_PIDR1_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR1_ADDR, HWIO_QDSS_REPL64_PIDR1_RMSK)
#define HWIO_QDSS_REPL64_PIDR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR1_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR1_JEP106_BITS3TO0_BMSK                      0xf0
#define HWIO_QDSS_REPL64_PIDR1_JEP106_BITS3TO0_SHFT                       0x4
#define HWIO_QDSS_REPL64_PIDR1_PART_NUMBER_BITS11TO8_BMSK                 0xf
#define HWIO_QDSS_REPL64_PIDR1_PART_NUMBER_BITS11TO8_SHFT                 0x0

#define HWIO_QDSS_REPL64_PIDR2_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_REPL64_PIDR2_RMSK                                      0xff
#define HWIO_QDSS_REPL64_PIDR2_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR2_ADDR, HWIO_QDSS_REPL64_PIDR2_RMSK)
#define HWIO_QDSS_REPL64_PIDR2_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR2_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR2_REVISION_BMSK                             0xf0
#define HWIO_QDSS_REPL64_PIDR2_REVISION_SHFT                              0x4
#define HWIO_QDSS_REPL64_PIDR2_JEDEC_BMSK                                 0x8
#define HWIO_QDSS_REPL64_PIDR2_JEDEC_SHFT                                 0x3
#define HWIO_QDSS_REPL64_PIDR2_JEP106_BITS6TO4_BMSK                       0x7
#define HWIO_QDSS_REPL64_PIDR2_JEP106_BITS6TO4_SHFT                       0x0

#define HWIO_QDSS_REPL64_PIDR3_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_REPL64_PIDR3_RMSK                                      0xff
#define HWIO_QDSS_REPL64_PIDR3_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR3_ADDR, HWIO_QDSS_REPL64_PIDR3_RMSK)
#define HWIO_QDSS_REPL64_PIDR3_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR3_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR3_REVAND_BMSK                               0xf0
#define HWIO_QDSS_REPL64_PIDR3_REVAND_SHFT                                0x4
#define HWIO_QDSS_REPL64_PIDR3_CUSTOMER_MODIFIED_BMSK                     0xf
#define HWIO_QDSS_REPL64_PIDR3_CUSTOMER_MODIFIED_SHFT                     0x0

#define HWIO_QDSS_REPL64_PIDR4_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_REPL64_PIDR4_RMSK                                      0xff
#define HWIO_QDSS_REPL64_PIDR4_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR4_ADDR, HWIO_QDSS_REPL64_PIDR4_RMSK)
#define HWIO_QDSS_REPL64_PIDR4_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR4_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR4_FOURKB_COUNT_BMSK                         0xf0
#define HWIO_QDSS_REPL64_PIDR4_FOURKB_COUNT_SHFT                          0x4
#define HWIO_QDSS_REPL64_PIDR4_JEP106_CONT_BMSK                           0xf
#define HWIO_QDSS_REPL64_PIDR4_JEP106_CONT_SHFT                           0x0

#define HWIO_QDSS_REPL64_PIDR5_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_REPL64_PIDR5_RMSK                                0xffffffff
#define HWIO_QDSS_REPL64_PIDR5_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR5_ADDR, HWIO_QDSS_REPL64_PIDR5_RMSK)
#define HWIO_QDSS_REPL64_PIDR5_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR5_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR5_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_PIDR5_ADDR,v)
#define HWIO_QDSS_REPL64_PIDR5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_PIDR5_ADDR,m,v,HWIO_QDSS_REPL64_PIDR5_IN)
#define HWIO_QDSS_REPL64_PIDR5_PIDR5_BMSK                          0xffffffff
#define HWIO_QDSS_REPL64_PIDR5_PIDR5_SHFT                                 0x0

#define HWIO_QDSS_REPL64_PIDR6_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_REPL64_PIDR6_RMSK                                0xffffffff
#define HWIO_QDSS_REPL64_PIDR6_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR6_ADDR, HWIO_QDSS_REPL64_PIDR6_RMSK)
#define HWIO_QDSS_REPL64_PIDR6_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR6_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR6_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_PIDR6_ADDR,v)
#define HWIO_QDSS_REPL64_PIDR6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_PIDR6_ADDR,m,v,HWIO_QDSS_REPL64_PIDR6_IN)
#define HWIO_QDSS_REPL64_PIDR6_PIDR6_BMSK                          0xffffffff
#define HWIO_QDSS_REPL64_PIDR6_PIDR6_SHFT                                 0x0

#define HWIO_QDSS_REPL64_PIDR7_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_REPL64_PIDR7_RMSK                                0xffffffff
#define HWIO_QDSS_REPL64_PIDR7_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR7_ADDR, HWIO_QDSS_REPL64_PIDR7_RMSK)
#define HWIO_QDSS_REPL64_PIDR7_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_PIDR7_ADDR, m)
#define HWIO_QDSS_REPL64_PIDR7_OUT(v)      \
        out_dword(HWIO_QDSS_REPL64_PIDR7_ADDR,v)
#define HWIO_QDSS_REPL64_PIDR7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_REPL64_PIDR7_ADDR,m,v,HWIO_QDSS_REPL64_PIDR7_IN)
#define HWIO_QDSS_REPL64_PIDR7_PIDR7_BMSK                          0xffffffff
#define HWIO_QDSS_REPL64_PIDR7_PIDR7_SHFT                                 0x0

#define HWIO_QDSS_REPL64_CIDR0_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_REPL64_CIDR0_RMSK                                      0xff
#define HWIO_QDSS_REPL64_CIDR0_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR0_ADDR, HWIO_QDSS_REPL64_CIDR0_RMSK)
#define HWIO_QDSS_REPL64_CIDR0_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR0_ADDR, m)
#define HWIO_QDSS_REPL64_CIDR0_PREAMBLE_BMSK                             0xff
#define HWIO_QDSS_REPL64_CIDR0_PREAMBLE_SHFT                              0x0

#define HWIO_QDSS_REPL64_CIDR1_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_REPL64_CIDR1_RMSK                                      0xff
#define HWIO_QDSS_REPL64_CIDR1_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR1_ADDR, HWIO_QDSS_REPL64_CIDR1_RMSK)
#define HWIO_QDSS_REPL64_CIDR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR1_ADDR, m)
#define HWIO_QDSS_REPL64_CIDR1_CLASS_BMSK                                0xf0
#define HWIO_QDSS_REPL64_CIDR1_CLASS_SHFT                                 0x4
#define HWIO_QDSS_REPL64_CIDR1_PREAMBLE_BMSK                              0xf
#define HWIO_QDSS_REPL64_CIDR1_PREAMBLE_SHFT                              0x0

#define HWIO_QDSS_REPL64_CIDR2_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_REPL64_CIDR2_RMSK                                      0xff
#define HWIO_QDSS_REPL64_CIDR2_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR2_ADDR, HWIO_QDSS_REPL64_CIDR2_RMSK)
#define HWIO_QDSS_REPL64_CIDR2_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR2_ADDR, m)
#define HWIO_QDSS_REPL64_CIDR2_PREAMBLE_BMSK                             0xff
#define HWIO_QDSS_REPL64_CIDR2_PREAMBLE_SHFT                              0x0

#define HWIO_QDSS_REPL64_CIDR3_ADDR                                (QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_REPL64_CIDR3_RMSK                                      0xff
#define HWIO_QDSS_REPL64_CIDR3_IN          \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR3_ADDR, HWIO_QDSS_REPL64_CIDR3_RMSK)
#define HWIO_QDSS_REPL64_CIDR3_INM(m)      \
        in_dword_masked(HWIO_QDSS_REPL64_CIDR3_ADDR, m)
#define HWIO_QDSS_REPL64_CIDR3_PREAMBLE_BMSK                             0xff
#define HWIO_QDSS_REPL64_CIDR3_PREAMBLE_SHFT                              0x0


#endif /* __QDSS_REPLICATOR_REGS_H__ */
