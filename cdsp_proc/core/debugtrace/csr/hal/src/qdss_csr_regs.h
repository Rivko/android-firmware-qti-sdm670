#ifndef __QDSS_CSR_REGS_H__
#define __QDSS_CSR_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_csr_regs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8996 (Istari) [istari_v1.0_p2q1r8.1.4_MTO]
 
  This file contains HWIO register definitions for the following modules:
    QDSS_QDSSCSR

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

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/csr/hal/src/qdss_csr_regs.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "HALhwio.h"


/*----------------------------------------------------------------------------
 * MODULE: QDSS_QDSSCSR
 *--------------------------------------------------------------------------*/

extern volatile uint32 QDSS_QDSSCSR_REG_BASE;


#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000000)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_RFU_BMSK                                       0xfffffffe
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_RFU_SHFT                                              0x1
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_SWDBGPWRUPREQ_BMSK                                    0x1
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRCTRL_SWDBGPWRUPREQ_SHFT                                    0x0

#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000004)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_ADDR, HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_RFU_BMSK                                        0xffff0000
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_RFU_SHFT                                              0x10
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_SWDBGPWRACK_BMSK                                    0xffff
#define HWIO_QDSS_CS_QDSSCSR_SWDBGPWRACK_SWDBGPWRACK_SHFT                                       0x0

#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000008)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_ADDR, HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_IN)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_SWSPADRW_BMSK                                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG0_SWSPADRW_SHFT                                           0x0

#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x0000000c)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_ADDR, HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_IN)
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_SWSPADRW_BMSK                                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_SWSPADREG1_SWSPADRW_SHFT                                           0x0

#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000010)
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_RFU_BMSK                                       0xfffffff0
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_RFU_SHFT                                              0x4
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB3_BMSK                                     0x8
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB3_SHFT                                     0x3
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB2_BMSK                                     0x4
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB2_SHFT                                     0x2
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB1_BMSK                                     0x2
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB1_SHFT                                     0x1
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB0_BMSK                                     0x1
#define HWIO_QDSS_CS_QDSSCSR_STMTRANSCTRL_STMTRANENAB0_SHFT                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000014)
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID3_BMSK                                   0xff000000
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID3_SHFT                                         0x18
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID2_BMSK                                     0xff0000
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID2_SHFT                                         0x10
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID1_BMSK                                       0xff00
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID1_SHFT                                          0x8
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID0_BMSK                                         0xff
#define HWIO_QDSS_CS_QDSSCSR_STMAWIDCTRL_STMAWID0_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000018)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_ADDR, HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_RFU_BMSK                                        0xf8000000
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_RFU_SHFT                                              0x1b
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST2_BMSK                                 0x7fc0000
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST2_SHFT                                      0x12
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST1_BMSK                                   0x3fe00
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST1_SHFT                                       0x9
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST0_BMSK                                     0x1ff
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST0_STMCHNOFST0_SHFT                                       0x0

#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x0000001c)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_ADDR, HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_RFU_BMSK                                        0xfffffe00
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_RFU_SHFT                                               0x9
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_STMCHNOFST3_BMSK                                     0x1ff
#define HWIO_QDSS_CS_QDSSCSR_STMCHNOFST1_STMCHNOFST3_SHFT                                       0x0

#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x00000020)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_ADDR, HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS7_BMSK                                    0xf0000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS7_SHFT                                          0x1c
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS6_BMSK                                     0xf000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS6_SHFT                                          0x18
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS5_BMSK                                      0xf00000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS5_SHFT                                          0x14
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS4_BMSK                                       0xf0000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS4_SHFT                                          0x10
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS3_BMSK                                        0xf000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS3_SHFT                                           0xc
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS2_BMSK                                         0xf00
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS2_SHFT                                           0x8
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS1_BMSK                                          0xf0
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS1_SHFT                                           0x4
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS0_BMSK                                           0xf
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL0_HEBS0_SHFT                                           0x0

#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x00000024)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_ADDR, HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS15_BMSK                                   0xf0000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS15_SHFT                                         0x1c
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS14_BMSK                                    0xf000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS14_SHFT                                         0x18
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS13_BMSK                                     0xf00000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS13_SHFT                                         0x14
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS12_BMSK                                      0xf0000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS12_SHFT                                         0x10
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS11_BMSK                                       0xf000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS11_SHFT                                          0xc
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS10_BMSK                                        0xf00
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS10_SHFT                                          0x8
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS9_BMSK                                          0xf0
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS9_SHFT                                           0x4
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS8_BMSK                                           0xf
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL1_HEBS8_SHFT                                           0x0

#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x00000028)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_ADDR, HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS23_BMSK                                   0xf0000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS23_SHFT                                         0x1c
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS22_BMSK                                    0xf000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS22_SHFT                                         0x18
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS21_BMSK                                     0xf00000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS21_SHFT                                         0x14
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS20_BMSK                                      0xf0000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS20_SHFT                                         0x10
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS19_BMSK                                       0xf000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS19_SHFT                                          0xc
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS18_BMSK                                        0xf00
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS18_SHFT                                          0x8
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS17_BMSK                                         0xf0
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS17_SHFT                                          0x4
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS16_BMSK                                          0xf
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL2_HEBS16_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x0000002c)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_ADDR, HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_IN)
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS31_BMSK                                   0xf0000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS31_SHFT                                         0x1c
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS30_BMSK                                    0xf000000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS30_SHFT                                         0x18
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS29_BMSK                                     0xf00000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS29_SHFT                                         0x14
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS28_BMSK                                      0xf0000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS28_SHFT                                         0x10
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS27_BMSK                                       0xf000
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS27_SHFT                                          0xc
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS26_BMSK                                        0xf00
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS26_SHFT                                          0x8
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS25_BMSK                                         0xf0
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS25_SHFT                                          0x4
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS24_BMSK                                          0xf
#define HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRL3_HEBS24_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000030)
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_RFU_BMSK                                         0xfffffff8
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_RFU_SHFT                                                0x3
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_USBENAB_BMSK                                            0x4
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_USBENAB_SHFT                                            0x2
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_BLKSIZE_BMSK                                            0x3
#define HWIO_QDSS_CS_QDSSCSR_USBBAMCTRL_BLKSIZE_SHFT                                            0x0

#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000034)
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_RFU_BMSK                                        0xfff00000
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_RFU_SHFT                                              0x14
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_TRIGEOT_BMSK                                       0x80000
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_TRIGEOT_SHFT                                          0x13
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSHEOT_BMSK                                    0x40000
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSHEOT_SHFT                                       0x12
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSHTHRS_BMSK                                   0x3fffc
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSHTHRS_SHFT                                       0x2
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSH_BMSK                                           0x2
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_PERFLSH_SHFT                                           0x1
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_EOT_BMSK                                               0x1
#define HWIO_QDSS_CS_QDSSCSR_USBFLSHCTRL_EOT_SHFT                                               0x0

#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x00000038)
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_BMSK                                      0xfffffff0
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_SHFT                                             0x4
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPSYNCDISABLE_BMSK                            0x8
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPSYNCDISABLE_SHFT                            0x3
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPSYNCON_BMSK                                 0x4
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPSYNCON_SHFT                                 0x2
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_BMSK                                 0x2
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_SHFT                                 0x1
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_BMSK                                         0x1
#define HWIO_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_SHFT                                         0x0

#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x0000003c)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_BMSK                                     0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_SHFT                                            0x0

#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000040)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_RFU_BMSK                                         0xff000000
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_RFU_SHFT                                               0x18
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_BMSK                                       0xffffff
#define HWIO_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_SHFT                                            0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000044)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_BMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000048)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_BMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x0000004c)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_BMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_SHFT                                        0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR                                          (QDSS_QDSSCSR_REG_BASE      + 0x00000050)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_BMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_SHFT                                        0x0

#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000054)
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_ADDR, HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_IN)
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_RFU_BMSK                                          0xfffffffe
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_RFU_SHFT                                                 0x1
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_MSAVAL_BMSK                                              0x1
#define HWIO_QDSS_CS_QDSSCSR_DAPMSAVAL_MSAVAL_SHFT                                              0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000058)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_CLKVOTE_BMSK                                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKVOTE_CLKVOTE_SHFT                                           0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x0000005c)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_RFU_BMSK                                         0xfffffffe
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_RFU_SHFT                                                0x1
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_CLKIPI_BMSK                                             0x1
#define HWIO_QDSS_CS_QDSSCSR_QDSSCLKIPI_CLKIPI_SHFT                                             0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR                                       (QDSS_QDSSCSR_REG_BASE      + 0x00000060)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_RMSK                                       0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_BMSK                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_SHFT                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000064)
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_ADDR, HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_IN)
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_SPARE_BMSK                                        0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_QDSSSPARE_SPARE_SHFT                                               0x0

#define HWIO_QDSS_CS_QDSSCSR_IPCAT_ADDR                                                  (QDSS_QDSSCSR_REG_BASE      + 0x00000068)
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_RMSK                                                  0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_IPCAT_ADDR, HWIO_QDSS_CS_QDSSCSR_IPCAT_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_IPCAT_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_MAJ_BMSK                                              0xf0000000
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_MAJ_SHFT                                                    0x1c
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_MIN_BMSK                                               0xfff0000
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_MIN_SHFT                                                    0x10
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_STEP_BMSK                                                 0xffff
#define HWIO_QDSS_CS_QDSSCSR_IPCAT_STEP_SHFT                                                    0x0

#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x0000006c)
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_ADDR, HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_IN)
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_IRQBYTECNTVAL_BMSK                               0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ETRIRQCTRL_IRQBYTECNTVAL_SHFT                                      0x0

#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_ADDR                                            (QDSS_QDSSCSR_REG_BASE      + 0x00000070)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RMSK                                            0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_ADDR, HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RFU_BMSK                                        0xff800000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RFU_SHFT                                              0x17
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_MEMERR_BMSK                                   0x400000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_MEMERR_SHFT                                       0x16
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_AFVALIDS_BMSK                                     0x200000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_AFVALIDS_SHFT                                         0x15
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_EMPTY_BMSK                                    0x100000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_EMPTY_SHFT                                        0x14
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_ACQCOMP_BMSK                                       0x80000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_ACQCOMP_SHFT                                          0x13
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_TMCREADY_BMSK                                  0x40000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_TMCREADY_SHFT                                     0x12
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_TRIGGERED_BMSK                                 0x20000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_STS_TRIGGERED_SHFT                                    0x11
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_FULL_BMSK                                          0x10000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_FULL_SHFT                                             0x10
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RWP_PRESERVE_BMSK                                   0xffff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETF_RWP_PRESERVE_SHFT                                      0x0

#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000074)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_ADDR, HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_RWP_PRESERVE_BMSK                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR0_RWP_PRESERVE_SHFT                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_ADDR                                           (QDSS_QDSSCSR_REG_BASE      + 0x00000078)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_ADDR, HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RFU_BMSK                                       0xff800000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RFU_SHFT                                             0x17
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_MEMERR_BMSK                                  0x400000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_MEMERR_SHFT                                      0x16
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_AFVALIDS_BMSK                                    0x200000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_AFVALIDS_SHFT                                        0x15
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_EMPTY_BMSK                                   0x100000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_EMPTY_SHFT                                       0x14
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_ACQCOMP_BMSK                                      0x80000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_ACQCOMP_SHFT                                         0x13
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_TMCREADY_BMSK                                 0x40000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_TMCREADY_SHFT                                    0x12
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_TRIGGERED_BMSK                                0x20000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_STS_TRIGGERED_SHFT                                   0x11
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_FULL_BMSK                                         0x10000
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_FULL_SHFT                                            0x10
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RFU15_8_BMSK                                       0xff00
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RFU15_8_SHFT                                          0x8
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RWP_PRESERVE_BMSK                                    0xff
#define HWIO_QDSS_CS_QDSSCSR_PRESERVEETR1_RWP_PRESERVE_SHFT                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ADDR                                         (QDSS_QDSSCSR_REG_BASE      + 0x0000007c)
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_RMSK                                         0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ADDR, HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_IN)
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_RFU_BMSK                                     0xff000000
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_RFU_SHFT                                           0x18
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ARADDR_EXT_BMSK                                0xffffff
#define HWIO_QDSS_CS_QDSSCSR_ETR_ARADDR_EXT_ARADDR_EXT_SHFT                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_ADDR                                         (QDSS_QDSSCSR_REG_BASE      + 0x00000080)
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_RMSK                                         0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_ADDR, HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_IN)
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_RFU_BMSK                                     0xff000000
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_RFU_SHFT                                           0x18
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_AWADDR_EXT_BMSK                                0xffffff
#define HWIO_QDSS_CS_QDSSCSR_ETR_AWADDR_EXT_AWADDR_EXT_SHFT                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_ADDR                                       (QDSS_QDSSCSR_REG_BASE      + 0x00000084)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_RMSK                                       0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_TS_HBEAT_VAL0_LO_BMSK                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_LO_TS_HBEAT_VAL0_LO_SHFT                             0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_ADDR                                       (QDSS_QDSSCSR_REG_BASE      + 0x00000088)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_RMSK                                       0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_TS_HBEAT_VAL0_HI_BMSK                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL0_HI_TS_HBEAT_VAL0_HI_SHFT                             0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_ADDR                                       (QDSS_QDSSCSR_REG_BASE      + 0x0000008c)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_RMSK                                       0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_TS_HBEAT_VAL1_LO_BMSK                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_LO_TS_HBEAT_VAL1_LO_SHFT                             0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_ADDR                                       (QDSS_QDSSCSR_REG_BASE      + 0x00000090)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_RMSK                                       0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_TS_HBEAT_VAL1_HI_BMSK                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_VAL1_HI_TS_HBEAT_VAL1_HI_SHFT                             0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_ADDR                                      (QDSS_QDSSCSR_REG_BASE      + 0x00000094)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_RMSK                                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_TS_HBEAT_MASK0_LO_BMSK                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_LO_TS_HBEAT_MASK0_LO_SHFT                           0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_ADDR                                      (QDSS_QDSSCSR_REG_BASE      + 0x00000098)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_RMSK                                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_TS_HBEAT_MASK0_HI_BMSK                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK0_HI_TS_HBEAT_MASK0_HI_SHFT                           0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_ADDR                                      (QDSS_QDSSCSR_REG_BASE      + 0x0000009c)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_RMSK                                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_TS_HBEAT_MASK1_LO_BMSK                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_LO_TS_HBEAT_MASK1_LO_SHFT                           0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_ADDR                                      (QDSS_QDSSCSR_REG_BASE      + 0x000000a0)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_RMSK                                      0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_TS_HBEAT_MASK1_HI_BMSK                    0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_HBEAT_MASK1_HI_TS_HBEAT_MASK1_HI_SHFT                           0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_ADDR                                 (QDSS_QDSSCSR_REG_BASE      + 0x000000a4)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_RMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_TS_LEVEL_START_VAL0_LO_BMSK          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_LO_TS_LEVEL_START_VAL0_LO_SHFT                 0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_ADDR                                 (QDSS_QDSSCSR_REG_BASE      + 0x000000a8)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_RMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_TS_LEVEL_START_VAL0_HI_BMSK          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL0_HI_TS_LEVEL_START_VAL0_HI_SHFT                 0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000ac)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_TS_LEVEL_END_VAL0_LO_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_LO_TS_LEVEL_END_VAL0_LO_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000b0)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_TS_LEVEL_END_VAL0_HI_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL0_HI_TS_LEVEL_END_VAL0_HI_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_ADDR                                 (QDSS_QDSSCSR_REG_BASE      + 0x000000b4)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_RMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_TS_LEVEL_START_VAL1_LO_BMSK          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_LO_TS_LEVEL_START_VAL1_LO_SHFT                 0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_ADDR                                 (QDSS_QDSSCSR_REG_BASE      + 0x000000b8)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_RMSK                                 0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_TS_LEVEL_START_VAL1_HI_BMSK          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_START_VAL1_HI_TS_LEVEL_START_VAL1_HI_SHFT                 0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000bc)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_TS_LEVEL_END_VAL1_LO_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_LO_TS_LEVEL_END_VAL1_LO_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000c0)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_TS_LEVEL_END_VAL1_HI_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_TS_LEVEL_END_VAL1_HI_TS_LEVEL_END_VAL1_HI_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000c4)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_HWE_TIMER_A_B_VAL_LO_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_LO_HWE_TIMER_A_B_VAL_LO_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000c8)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_HWE_TIMER_A_B_VAL_HI_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_A_B_VAL_HI_HWE_TIMER_A_B_VAL_HI_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000cc)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_ADDR, HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_IN)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_HWE_TIMER_B_C_VAL_LO_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_LO_HWE_TIMER_B_C_VAL_LO_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000d0)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_ADDR, HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_IN)
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_HWE_TIMER_B_C_VAL_HI_BMSK              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_TIMER_B_C_VAL_HI_HWE_TIMER_B_C_VAL_HI_SHFT                     0x0

#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_ADDR                                   (QDSS_QDSSCSR_REG_BASE      + 0x000000d4)
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_RMSK                                   0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_ADDR, HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_ADDR,m,v,HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_IN)
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_RFU_BMSK                               0x80000000
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_RFU_SHFT                                     0x1f
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_C_BMSK                       0x7fc00000
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_C_SHFT                             0x16
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_B_BMSK                         0x3fe000
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_B_SHFT                              0xd
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_A_BMSK                           0x1ff0
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEL_EVENT_A_SHFT                              0x4
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_DETECT_EN_BMSK                                0x8
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_DETECT_EN_SHFT                                0x3
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEQ_TO_DETECT_BMSK                            0x7
#define HWIO_QDSS_CS_QDSSCSR_HWE_SEQ_DETECT_CTRL0_SEQ_TO_DETECT_SHFT                            0x0

#define HWIO_QDSS_CS_QDSSCSR_ITCTL_ADDR                                                  (QDSS_QDSSCSR_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_CS_QDSSCSR_ITCTL_RMSK                                                  0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ITCTL_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ITCTL_ADDR, HWIO_QDSS_CS_QDSSCSR_ITCTL_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_ITCTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_ITCTL_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_ITCTL_RFU_BMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_ITCTL_RFU_SHFT                                                     0x0

#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_ADDR                                               (QDSS_QDSSCSR_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_RMSK                                               0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_CLAIMSET_ADDR, HWIO_QDSS_CS_QDSSCSR_CLAIMSET_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_CLAIMSET_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_RFU_BMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_CLAIMSET_RFU_SHFT                                                  0x0

#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_ADDR                                               (QDSS_QDSSCSR_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_RMSK                                               0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_ADDR, HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_RFU_BMSK                                           0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_CLAIMCLR_RFU_SHFT                                                  0x0

#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_OUT(v)      \
        out_dword(HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_ADDR,v)
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_LOCKACCESS_BMSK                                  0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_LOCKACCESS_SHFT                                         0x0

#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR, HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RFU_BMSK                                         0xfffffff8
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_RFU_SHFT                                                0x3
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_BMSK                                   0x4
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_SHFT                                   0x2
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ACCESS_BMSK                                             0x2
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_ACCESS_SHFT                                             0x1
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_LOCK_CONTROL_BMSK                                       0x1
#define HWIO_QDSS_CS_QDSSCSR_LOCKSTATUS_LOCK_CONTROL_SHFT                                       0x0

#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_ADDR, HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_RFU_BMSK                                         0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_AUTHSTATUS_RFU_SHFT                                                0x0

#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_ADDR                                               (QDSS_QDSSCSR_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_RMSK                                               0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DEVICEID_ADDR, HWIO_QDSS_CS_QDSSCSR_DEVICEID_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DEVICEID_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_RFU_BMSK                                           0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_RFU_SHFT                                                  0x8
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_DEVID_BMSK                                               0xff
#define HWIO_QDSS_CS_QDSSCSR_DEVICEID_DEVID_SHFT                                                0x0

#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_ADDR                                             (QDSS_QDSSCSR_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_RMSK                                             0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_ADDR, HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_RFU_BMSK                                         0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_RFU_SHFT                                                0x8
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_SUBTYPE_BMSK                                           0xf0
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_SUBTYPE_SHFT                                            0x4
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_MAJTYPE_BMSK                                            0xf
#define HWIO_QDSS_CS_QDSSCSR_DEVICETYPE_MAJTYPE_SHFT                                            0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID4_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID4_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID4_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_RFU_BMSK                                          0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_RFU_SHFT                                                 0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_FIELD_4KB_COUNT_BMSK                                    0xf0
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_FIELD_4KB_COUNT_SHFT                                     0x4
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_JEP106_CONTINUATION_BMSK                                 0xf
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID4_JEP106_CONTINUATION_SHFT                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID5_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID5_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID5_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_RFU_BMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID5_RFU_SHFT                                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID6_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID6_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID6_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_RFU_BMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID6_RFU_SHFT                                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID7_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID7_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID7_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_RFU_BMSK                                          0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID7_RFU_SHFT                                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID0_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_RFU_BMSK                                          0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_RFU_SHFT                                                 0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_PARTNUM_BMSK                                            0xff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID0_PARTNUM_SHFT                                             0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID1_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_RFU_BMSK                                          0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_RFU_SHFT                                                 0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_JEP106_IDENTITY_3_0_BMSK                                0xf0
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_JEP106_IDENTITY_3_0_SHFT                                 0x4
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_PARTNUM_BMSK                                             0xf
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID1_PARTNUM_SHFT                                             0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID2_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID2_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID2_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_RFU_BMSK                                          0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_RFU_SHFT                                                 0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_MAJREV_BMSK                                             0xf0
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_MAJREV_SHFT                                              0x4
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_JEDEC_BMSK                                               0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_JEDEC_SHFT                                               0x3
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_JEP106_IDENTITY_6_4_BMSK                                 0x7
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID2_JEP106_IDENTITY_6_4_SHFT                                 0x0

#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_ADDR                                              (QDSS_QDSSCSR_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_RMSK                                              0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID3_ADDR, HWIO_QDSS_CS_QDSSCSR_PERIPHID3_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_PERIPHID3_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_RFU_BMSK                                          0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_RFU_SHFT                                                 0x8
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_REV_AND_BMSK                                            0xf0
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_REV_AND_SHFT                                             0x4
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_CUSTOMER_MODIFIED_BMSK                                   0xf
#define HWIO_QDSS_CS_QDSSCSR_PERIPHID3_CUSTOMER_MODIFIED_SHFT                                   0x0

#define HWIO_QDSS_CS_QDSSCSR_COMPID0_ADDR                                                (QDSS_QDSSCSR_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_RMSK                                                0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID0_ADDR, HWIO_QDSS_CS_QDSSCSR_COMPID0_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID0_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_RFU_BMSK                                            0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_RFU_SHFT                                                   0x8
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_PREAMBLE_7_0_BMSK                                         0xff
#define HWIO_QDSS_CS_QDSSCSR_COMPID0_PREAMBLE_7_0_SHFT                                          0x0

#define HWIO_QDSS_CS_QDSSCSR_COMPID1_ADDR                                                (QDSS_QDSSCSR_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_RMSK                                                0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID1_ADDR, HWIO_QDSS_CS_QDSSCSR_COMPID1_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID1_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_RFU_BMSK                                            0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_RFU_SHFT                                                   0x8
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_PREAMBLE_15_12_BMSK                                       0xf0
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_PREAMBLE_15_12_SHFT                                        0x4
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_PREAMBLE_11_8_BMSK                                         0xf
#define HWIO_QDSS_CS_QDSSCSR_COMPID1_PREAMBLE_11_8_SHFT                                         0x0

#define HWIO_QDSS_CS_QDSSCSR_COMPID2_ADDR                                                (QDSS_QDSSCSR_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_RMSK                                                0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID2_ADDR, HWIO_QDSS_CS_QDSSCSR_COMPID2_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID2_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_RFU_BMSK                                            0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_RFU_SHFT                                                   0x8
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_PREAMBLE_23_16_BMSK                                       0xff
#define HWIO_QDSS_CS_QDSSCSR_COMPID2_PREAMBLE_23_16_SHFT                                        0x0

#define HWIO_QDSS_CS_QDSSCSR_COMPID3_ADDR                                                (QDSS_QDSSCSR_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_RMSK                                                0xffffffff
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_IN          \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID3_ADDR, HWIO_QDSS_CS_QDSSCSR_COMPID3_RMSK)
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CS_QDSSCSR_COMPID3_ADDR, m)
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_RFU_BMSK                                            0xffffff00
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_RFU_SHFT                                                   0x8
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_PREAMBLE_31_24_BMSK                                       0xff
#define HWIO_QDSS_CS_QDSSCSR_COMPID3_PREAMBLE_31_24_SHFT                                        0x0


#endif /* __QDSS_CSR_REGS_H__ */
