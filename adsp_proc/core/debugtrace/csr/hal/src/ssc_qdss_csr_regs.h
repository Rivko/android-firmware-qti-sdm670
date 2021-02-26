#ifndef __SSC_QDSS_CSR_REGS_H__
#define __SSC_QDSS_CSR_REGS_H__
/*
===========================================================================
*/
/**
  @file ssc_qdss_csr_regs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8996 (Istari) [istari_v1.0_p2q1r8.1.1_MTO]
 
  This file contains HWIO register definitions for the following modules:
    SSC_MCC_REGS

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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
*/

#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_MCC_REGS
 *--------------------------------------------------------------------------*/

#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR(x)                                      ((x) + 0x00005000)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_RMSK                                         0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR(x), HWIO_SSC_SSC_TSGEN_PRELOAD_LO_RMSK)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR(x), m)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR(x),v)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR(x),m,v,HWIO_SSC_SSC_TSGEN_PRELOAD_LO_IN(x))
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_BMSK                        0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_SHFT                               0x0

#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR(x)                                      ((x) + 0x00005004)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_RMSK                                         0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR(x), HWIO_SSC_SSC_TSGEN_PRELOAD_HI_RMSK)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR(x), m)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR(x),v)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR(x),m,v,HWIO_SSC_SSC_TSGEN_PRELOAD_HI_IN(x))
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_BMSK                        0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_SHFT                               0x0

#define HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR(x)                                          ((x) + 0x00005008)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_RMSK                                             0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_LO_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR(x), HWIO_SSC_SSC_TSGEN_CTR_LO_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR(x), m)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_BMSK                                0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_SHFT                                       0x0

#define HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR(x)                                          ((x) + 0x0000500c)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_RMSK                                             0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_HI_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR(x), HWIO_SSC_SSC_TSGEN_CTR_HI_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR(x), m)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_BMSK                                0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_SHFT                                       0x0



#define HWIO_SSC_SSC_TSGEN_CTL_ADDR(x)                                             ((x) + 0x00005010)
#define HWIO_SSC_SSC_TSGEN_CTL_RMSK                                                       0x3
#define HWIO_SSC_SSC_TSGEN_CTL_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTL_ADDR(x), HWIO_SSC_SSC_TSGEN_CTL_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTL_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTL_ADDR(x), m)
#define HWIO_SSC_SSC_TSGEN_CTL_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_CTL_ADDR(x),v)
#define HWIO_SSC_SSC_TSGEN_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_CTL_ADDR(x),m,v,HWIO_SSC_SSC_TSGEN_CTL_IN(x))
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_BMSK                                      0x2
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_SHFT                                      0x1
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_BMSK                                     0x1
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_SHFT                                     0x0


#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(x)                               ((x) + 0x00005014)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK                                         0x7
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(x), HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(x), m)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(x),v)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(x),m,v,HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_IN(x))
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                                 0x4
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                                 0x2
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                  0x2
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                  0x1
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_BMSK                                     0x1
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_SHFT                                     0x0

#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(x)                               ((x) + 0x00005018)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK                                         0x7
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(x), HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(x), m)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(x),v)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(x),m,v,HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_IN(x))
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                                 0x4
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                                 0x2
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                  0x2
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                  0x1
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_BMSK                                     0x1
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_SHFT                                     0x0


#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(x)                             ((x) + 0x0000501c)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(x), HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(x), m)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(x),v)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(x),m,v,HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_IN(x))
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(x)                             ((x) + 0x00005020)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(x), HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(x), m)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(x),v)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(x),m,v,HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_IN(x))
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR(x)						((x) + 0x00005024)


#define HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR(x)                                        ((x) + 0x00006000)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_RMSK                                               0xffff
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR(x), HWIO_SSC_SSC_HM_TESTBUS_SEL_RMSK)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR(x), m)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR(x),v)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR(x),m,v,HWIO_SSC_SSC_HM_TESTBUS_SEL_IN(x))
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_BMSK                                 0xffff
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_SHFT                                    0x0


#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR(x)                         ((x) + 0x00007000)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK                            0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR(x),v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR(x),m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN(x))
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_BMSK                        0xfffffffc
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_SHFT                               0x2
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_BMSK                   0x2
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_SHFT                   0x1
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_BMSK                           0x1
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_SHFT                           0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR(x)                            ((x) + 0x00007004)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK                               0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_BMSK                       0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_SHFT                              0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR(x)                            ((x) + 0x00007008)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK                               0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_BMSK                       0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_SHFT                              0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR(x)                          ((x) + 0x0000700c)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK                             0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_SHFT                            0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR(x)                          ((x) + 0x00007010)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK                             0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_SHFT                            0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR(x)                         ((x) + 0x00007014)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK                            0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR(x),v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR(x),m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN(x))
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_BMSK                   0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_SHFT                          0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR(x)                         ((x) + 0x00007018)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK                            0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN(x)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR(x), HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_INM(x, m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR(x), m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUT(x, v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR(x),v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR(x),m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN(x))
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_BMSK                   0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_SHFT                          0x0

#define SSC_ATB_LOW_POWER_HANDSHAKE_ADDR   (HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR(qdss_ssc_csr_base))
#define SSC_NTS_LOW_POWER_HANDSHAKE_ADDR   (HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR(qdss_ssc_csr_base))
#define SSC_APB_LOW_POWER_HANDSHAKE_ADDR   (HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR(qdss_ssc_csr_base))
#define SSC_DAP_LOW_POWER_HANDSHAKE_ADDR   (HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR(qdss_ssc_csr_base))
#define SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE   (HWIO_SSC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR(qdss_ssc_csr_base))


#endif /* __SSC_QDSS_CSR_REGS_H__ */
