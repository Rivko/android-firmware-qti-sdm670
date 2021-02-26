#ifndef __QDSS_SWAOCSR_REGS_H__
#define __QDSS_SWAOCSR_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_swaocsr_regs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.9_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    SWAOCSR_SWAOCSR


  Generation parameters: 
  { u'filename': u'qdss_swaocsr_regs.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'SWAOCSR_SWAOCSR']}
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/csr/hal/src/qdss_swaocsr_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

extern volatile uint32 QDSS_QDSSCSR_REG_BASE;

/*----------------------------------------------------------------------------
 * MODULE: SWAOCSR_SWAOCSR
 *--------------------------------------------------------------------------*/

#define SWAOCSR_SWAOCSR_REG_BASE                                                            (QDSS_QDSSCSR_REG_BASE)

#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000000)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_RFU_BMSK                                       0xfffffffe
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_RFU_SHFT                                              0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_SWDBGPWRUPREQ_BMSK                                    0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRCTRL_SWDBGPWRUPREQ_SHFT                                    0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_ADDR                                            (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000004)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_RMSK                                            0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_RFU_BMSK                                        0xffff0000
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_RFU_SHFT                                              0x10
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_SWDBGPWRACK_BMSK                                    0xffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWDBGPWRACK_SWDBGPWRACK_SHFT                                       0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000008)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_SWSPADRW_BMSK                                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG0_SWSPADRW_SHFT                                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000000c)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_SWSPADRW_BMSK                                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWSPADREG1_SWSPADRW_SHFT                                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_ADDR                                          (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000038)
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_RMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_RFU_BMSK                                      0xfffffff0
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_RFU_SHFT                                             0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPSYNCDISABLE_BMSK                            0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPSYNCDISABLE_SHFT                            0x3
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPSYNCON_BMSK                                 0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPSYNCON_SHFT                                 0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_BMSK                                 0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_SHFT                                 0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_CAPTURE_BMSK                                         0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_TIMESTAMPCTRL_CAPTURE_SHFT                                         0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000003c)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_TIMEVAL_BMSK                                     0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL0_TIMEVAL_SHFT                                            0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000040)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_RFU_BMSK                                         0xff000000
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_RFU_SHFT                                               0x18
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_TIMEVAL_BMSK                                       0xffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AOTIMEVAL1_TIMEVAL_SHFT                                            0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000044)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_TIMEVAL_BMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL0_TIMEVAL_SHFT                                          0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000048)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_TIMEVAL_BMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMEVAL1_TIMEVAL_SHFT                                          0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_ADDR                                          (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000004c)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_RMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_TIMELOAD_BMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD0_TIMELOAD_SHFT                                        0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_ADDR                                          (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000050)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_RMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_TIMELOAD_BMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSTIMELOAD1_TIMELOAD_SHFT                                        0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000054)
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_RFU_BMSK                                          0xfffffffc
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_RFU_SHFT                                                 0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_SPBIT_BMSK                                               0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_SPBIT_SHFT                                               0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_MSAVAL_BMSK                                              0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_DAPAPATTR_MSAVAL_SHFT                                              0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_ADDR                                            (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000058)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_RMSK                                            0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_CLKVOTE_BMSK                                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKVOTE_CLKVOTE_SHFT                                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000005c)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_RFU_BMSK                                         0xfffffffe
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_RFU_SHFT                                                0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_CLKIPI_BMSK                                             0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCLKIPI_CLKIPI_SHFT                                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_ADDR                                       (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000060)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_RMSK                                       0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_BMSK                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSPWRREQIGNORE_PWRREQIGNORE_SHFT                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000064)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_RFU_BMSK                                       0xfffffffe
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_RFU_SHFT                                              0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_AOPRQ_BMSK                                            0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_AOSSPWRUPREQ_AOPRQ_SHFT                                            0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_ADDR                                                  (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000068)
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_RMSK                                                  0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_MAJ_BMSK                                              0xf0000000
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_MAJ_SHFT                                                    0x1c
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_MIN_BMSK                                               0xfff0000
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_MIN_SHFT                                                    0x10
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_STEP_BMSK                                                 0xffff
#define HWIO_SWAOCSR_CS_SWAOCSR_IPCAT_STEP_SHFT                                                    0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000006c)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_IRQBYTECNTVAL_BMSK                               0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETRIRQCTRL_IRQBYTECNTVAL_SHFT                                      0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_ADDR                                            (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000070)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RMSK                                            0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RFU_BMSK                                        0xff800000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RFU_SHFT                                              0x17
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_MEMERR_BMSK                                   0x400000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_MEMERR_SHFT                                       0x16
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_AFVALIDS_BMSK                                     0x200000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_AFVALIDS_SHFT                                         0x15
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_EMPTY_BMSK                                    0x100000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_EMPTY_SHFT                                        0x14
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_ACQCOMP_BMSK                                       0x80000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_ACQCOMP_SHFT                                          0x13
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_TMCREADY_BMSK                                  0x40000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_TMCREADY_SHFT                                     0x12
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_TRIGGERED_BMSK                                 0x20000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_STS_TRIGGERED_SHFT                                    0x11
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_FULL_BMSK                                          0x10000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_FULL_SHFT                                             0x10
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RWP_PRESERVE_BMSK                                   0xffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETF_RWP_PRESERVE_SHFT                                      0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000074)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_RWP_PRESERVE_BMSK                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR0_RWP_PRESERVE_SHFT                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000078)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RFU_BMSK                                       0xff800000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RFU_SHFT                                             0x17
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_MEMERR_BMSK                                  0x400000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_MEMERR_SHFT                                      0x16
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_AFVALIDS_BMSK                                    0x200000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_AFVALIDS_SHFT                                        0x15
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_EMPTY_BMSK                                   0x100000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_EMPTY_SHFT                                       0x14
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_ACQCOMP_BMSK                                      0x80000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_ACQCOMP_SHFT                                         0x13
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_TMCREADY_BMSK                                 0x40000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_TMCREADY_SHFT                                    0x12
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_TRIGGERED_BMSK                                0x20000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_STS_TRIGGERED_SHFT                                   0x11
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_FULL_BMSK                                         0x10000
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_FULL_SHFT                                            0x10
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RFU15_8_BMSK                                       0xff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RFU15_8_SHFT                                          0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RWP_PRESERVE_BMSK                                    0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_PRESERVEETR1_RWP_PRESERVE_SHFT                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ADDR                                         (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000007c)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_RMSK                                         0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_RFU_BMSK                                     0xff000000
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_RFU_SHFT                                           0x18
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ARADDR_EXT_BMSK                                0xffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_ARADDR_EXT_ARADDR_EXT_SHFT                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_ADDR                                         (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000080)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_RMSK                                         0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_RFU_BMSK                                     0xff000000
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_RFU_SHFT                                           0x18
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_AWADDR_EXT_BMSK                                0xffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ETR_AWADDR_EXT_AWADDR_EXT_SHFT                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_ADDR                                       (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000084)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_RMSK                                       0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_TS_HBEAT_VAL0_LO_BMSK                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_LO_TS_HBEAT_VAL0_LO_SHFT                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_ADDR                                       (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000088)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_RMSK                                       0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_TS_HBEAT_VAL0_HI_BMSK                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL0_HI_TS_HBEAT_VAL0_HI_SHFT                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_ADDR                                       (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000008c)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_RMSK                                       0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_TS_HBEAT_VAL1_LO_BMSK                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_LO_TS_HBEAT_VAL1_LO_SHFT                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_ADDR                                       (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000090)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_RMSK                                       0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_TS_HBEAT_VAL1_HI_BMSK                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_VAL1_HI_TS_HBEAT_VAL1_HI_SHFT                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_ADDR                                      (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000094)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_RMSK                                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_TS_HBEAT_MASK0_LO_BMSK                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_LO_TS_HBEAT_MASK0_LO_SHFT                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_ADDR                                      (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000098)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_RMSK                                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_TS_HBEAT_MASK0_HI_BMSK                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK0_HI_TS_HBEAT_MASK0_HI_SHFT                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_ADDR                                      (SWAOCSR_SWAOCSR_REG_BASE      + 0x0000009c)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_RMSK                                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_TS_HBEAT_MASK1_LO_BMSK                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_LO_TS_HBEAT_MASK1_LO_SHFT                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_ADDR                                      (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000a0)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_RMSK                                      0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_TS_HBEAT_MASK1_HI_BMSK                    0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_HBEAT_MASK1_HI_TS_HBEAT_MASK1_HI_SHFT                           0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_ADDR                                 (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000a4)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_RMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_TS_LEVEL_START_VAL0_LO_BMSK          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_LO_TS_LEVEL_START_VAL0_LO_SHFT                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_ADDR                                 (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000a8)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_RMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_TS_LEVEL_START_VAL0_HI_BMSK          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL0_HI_TS_LEVEL_START_VAL0_HI_SHFT                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_ADDR                                   (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000ac)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_RMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_TS_LEVEL_END_VAL0_LO_BMSK              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_LO_TS_LEVEL_END_VAL0_LO_SHFT                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_ADDR                                   (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000b0)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_RMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_TS_LEVEL_END_VAL0_HI_BMSK              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL0_HI_TS_LEVEL_END_VAL0_HI_SHFT                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_ADDR                                 (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000b4)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_RMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_TS_LEVEL_START_VAL1_LO_BMSK          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_LO_TS_LEVEL_START_VAL1_LO_SHFT                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_ADDR                                 (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000b8)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_RMSK                                 0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_TS_LEVEL_START_VAL1_HI_BMSK          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_START_VAL1_HI_TS_LEVEL_START_VAL1_HI_SHFT                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_ADDR                                   (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000bc)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_RMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_TS_LEVEL_END_VAL1_LO_BMSK              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_LO_TS_LEVEL_END_VAL1_LO_SHFT                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_ADDR                                   (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000c0)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_RMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_TS_LEVEL_END_VAL1_HI_BMSK              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_TS_LEVEL_END_VAL1_HI_TS_LEVEL_END_VAL1_HI_SHFT                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_ADDR                                         (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000d8)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_RMSK                                         0x80000003
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_RFU_BMSK                                     0x80000000
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_RFU_SHFT                                           0x1f
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_QDSS_SRST_STOP_BMSK                                 0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_QDSS_SRST_STOP_SHFT                                 0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_QDSS_SRST_START_BMSK                                0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSS_SRST_CTRL_QDSS_SRST_START_SHFT                                0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_ADDR                                   (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000dc)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_RMSK                                   0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_RFU_BMSK                               0xfffffff0
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_RFU_SHFT                                      0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_PWRREQIGNORE_BMSK                             0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_QDSSCSYSPWRREQIGNORE_PWRREQIGNORE_SHFT                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_ADDR                                          (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000e0)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_RMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_RFU_BMSK                                      0xfffffffe
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_RFU_SHFT                                             0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_SWCSYSPWRUPREQ_BMSK                                  0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRCTRL_SWCSYSPWRUPREQ_SHFT                                  0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_ADDR                                           (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000e4)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_RMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_RFU_BMSK                                       0xfffffff0
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_RFU_SHFT                                              0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_SWCSYSPWRACK_BMSK                                     0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_SWCSYSPWRACK_SWCSYSPWRACK_SHFT                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x000000e8)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_ADDR,m,v,HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_IN)
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_SPARE_BMSK                                        0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_SWAOSPARE_SPARE_SHFT                                               0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_ADDR                                                  (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000f00)
#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_RMSK                                                  0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_RFU_BMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_ITCTL_RFU_SHFT                                                     0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_ADDR                                               (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fa0)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_RMSK                                               0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_RFU_BMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMSET_RFU_SHFT                                                  0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_ADDR                                               (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fa4)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_RMSK                                               0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_RFU_BMSK                                           0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_CLAIMCLR_RFU_SHFT                                                  0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fb0)
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_OUT(v)      \
        out_dword(HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_ADDR,v)
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_LOCKACCESS_BMSK                                  0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKACCESS_LOCKACCESS_SHFT                                         0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fb4)
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_RFU_BMSK                                         0xfffffff8
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_RFU_SHFT                                                0x3
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_BMSK                                   0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_FIELD_8_BIT_LOCK_SHFT                                   0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_ACCESS_BMSK                                             0x2
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_ACCESS_SHFT                                             0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_LOCK_CONTROL_BMSK                                       0x1
#define HWIO_SWAOCSR_CS_SWAOCSR_LOCKSTATUS_LOCK_CONTROL_SHFT                                       0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fb8)
#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_RFU_BMSK                                         0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_AUTHSTATUS_RFU_SHFT                                                0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_ADDR                                               (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fc8)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_RMSK                                               0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_RFU_BMSK                                           0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_RFU_SHFT                                                  0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_DEVID_BMSK                                               0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICEID_DEVID_SHFT                                                0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_ADDR                                             (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fcc)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_RMSK                                             0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_RFU_BMSK                                         0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_RFU_SHFT                                                0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_SUBTYPE_BMSK                                           0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_SUBTYPE_SHFT                                            0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_MAJTYPE_BMSK                                            0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_DEVICETYPE_MAJTYPE_SHFT                                            0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fd0)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_RFU_BMSK                                          0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_RFU_SHFT                                                 0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_FIELD_4KB_COUNT_BMSK                                    0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_FIELD_4KB_COUNT_SHFT                                     0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_JEP106_CONTINUATION_BMSK                                 0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID4_JEP106_CONTINUATION_SHFT                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fd4)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_RFU_BMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID5_RFU_SHFT                                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fd8)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_RFU_BMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID6_RFU_SHFT                                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fdc)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_RFU_BMSK                                          0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID7_RFU_SHFT                                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fe0)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_RFU_BMSK                                          0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_RFU_SHFT                                                 0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_PARTNUM_BMSK                                            0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID0_PARTNUM_SHFT                                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fe4)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_RFU_BMSK                                          0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_RFU_SHFT                                                 0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_JEP106_IDENTITY_3_0_BMSK                                0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_JEP106_IDENTITY_3_0_SHFT                                 0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_PARTNUM_BMSK                                             0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID1_PARTNUM_SHFT                                             0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fe8)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_RFU_BMSK                                          0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_RFU_SHFT                                                 0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_MAJREV_BMSK                                             0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_MAJREV_SHFT                                              0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_JEDEC_BMSK                                               0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_JEDEC_SHFT                                               0x3
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_JEP106_IDENTITY_6_4_BMSK                                 0x7
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID2_JEP106_IDENTITY_6_4_SHFT                                 0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_ADDR                                              (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000fec)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_RMSK                                              0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_RFU_BMSK                                          0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_RFU_SHFT                                                 0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_REV_AND_BMSK                                            0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_REV_AND_SHFT                                             0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_CUSTOMER_MODIFIED_BMSK                                   0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_PERIPHID3_CUSTOMER_MODIFIED_SHFT                                   0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_ADDR                                                (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000ff0)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_RMSK                                                0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_RFU_BMSK                                            0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_RFU_SHFT                                                   0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_PREAMBLE_7_0_BMSK                                         0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID0_PREAMBLE_7_0_SHFT                                          0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_ADDR                                                (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000ff4)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_RMSK                                                0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_RFU_BMSK                                            0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_RFU_SHFT                                                   0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_PREAMBLE_15_12_BMSK                                       0xf0
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_PREAMBLE_15_12_SHFT                                        0x4
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_PREAMBLE_11_8_BMSK                                         0xf
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID1_PREAMBLE_11_8_SHFT                                         0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_ADDR                                                (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000ff8)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_RMSK                                                0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_RFU_BMSK                                            0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_RFU_SHFT                                                   0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_PREAMBLE_23_16_BMSK                                       0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID2_PREAMBLE_23_16_SHFT                                        0x0

#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_ADDR                                                (SWAOCSR_SWAOCSR_REG_BASE      + 0x00000ffc)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_RMSK                                                0xffffffff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_IN          \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_ADDR, HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_RMSK)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_INM(m)      \
        in_dword_masked(HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_ADDR, m)
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_RFU_BMSK                                            0xffffff00
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_RFU_SHFT                                                   0x8
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_PREAMBLE_31_24_BMSK                                       0xff
#define HWIO_SWAOCSR_CS_SWAOCSR_COMPID3_PREAMBLE_31_24_SHFT                                        0x0


#endif /* __QDSS_SWAOCSR_REGS_H__ */
