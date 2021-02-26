#ifndef __QDSS_ETFETB_REGS_H__
#define __QDSS_ETFETB_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_etfetb_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSS_ETFETB_ETFETB_CXTMC_F128W64K

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tmc/hal/src/qdss_etfetb_regs.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: QDSS_ETFETB_ETFETB_CXTMC_F128W64K
 *--------------------------------------------------------------------------*/

extern volatile uint32 QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE;



#define HWIO_QDSS_ETFETB_RSZ_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000004)
#define HWIO_QDSS_ETFETB_RSZ_RMSK                                      0x7fffffff
#define HWIO_QDSS_ETFETB_RSZ_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_RSZ_ADDR, HWIO_QDSS_ETFETB_RSZ_RMSK)
#define HWIO_QDSS_ETFETB_RSZ_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_RSZ_ADDR, m)
#define HWIO_QDSS_ETFETB_RSZ_RSZ_BMSK                                  0x7fffffff
#define HWIO_QDSS_ETFETB_RSZ_RSZ_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_STS_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x0000000c)
#define HWIO_QDSS_ETFETB_STS_RMSK                                            0x1f
#define HWIO_QDSS_ETFETB_STS_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_STS_ADDR, HWIO_QDSS_ETFETB_STS_RMSK)
#define HWIO_QDSS_ETFETB_STS_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_STS_ADDR, m)
#define HWIO_QDSS_ETFETB_STS_EMPTY_BMSK                                      0x10
#define HWIO_QDSS_ETFETB_STS_EMPTY_SHFT                                       0x4
#define HWIO_QDSS_ETFETB_STS_FTEMPTY_BMSK                                     0x8
#define HWIO_QDSS_ETFETB_STS_FTEMPTY_SHFT                                     0x3
#define HWIO_QDSS_ETFETB_STS_TMCREADY_BMSK                                    0x4
#define HWIO_QDSS_ETFETB_STS_TMCREADY_SHFT                                    0x2
#define HWIO_QDSS_ETFETB_STS_TRIGGERED_BMSK                                   0x2
#define HWIO_QDSS_ETFETB_STS_TRIGGERED_SHFT                                   0x1
#define HWIO_QDSS_ETFETB_STS_FULL_BMSK                                        0x1
#define HWIO_QDSS_ETFETB_STS_FULL_SHFT                                        0x0

#define HWIO_QDSS_ETFETB_RRD_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000010)
#define HWIO_QDSS_ETFETB_RRD_RMSK                                      0xffffffff
#define HWIO_QDSS_ETFETB_RRD_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_RRD_ADDR, HWIO_QDSS_ETFETB_RRD_RMSK)
#define HWIO_QDSS_ETFETB_RRD_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_RRD_ADDR, m)
#define HWIO_QDSS_ETFETB_RRD_RRD_BMSK                                  0xffffffff
#define HWIO_QDSS_ETFETB_RRD_RRD_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_RRP_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000014)
#define HWIO_QDSS_ETFETB_RRP_RMSK                                          0xffff
#define HWIO_QDSS_ETFETB_RRP_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_RRP_ADDR, HWIO_QDSS_ETFETB_RRP_RMSK)
#define HWIO_QDSS_ETFETB_RRP_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_RRP_ADDR, m)
#define HWIO_QDSS_ETFETB_RRP_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_RRP_ADDR,v)
#define HWIO_QDSS_ETFETB_RRP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_RRP_ADDR,m,v,HWIO_QDSS_ETFETB_RRP_IN)
#define HWIO_QDSS_ETFETB_RRP_RRP_BMSK                                      0xffff
#define HWIO_QDSS_ETFETB_RRP_RRP_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_RWP_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000018)
#define HWIO_QDSS_ETFETB_RWP_RMSK                                          0xffff
#define HWIO_QDSS_ETFETB_RWP_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_RWP_ADDR, HWIO_QDSS_ETFETB_RWP_RMSK)
#define HWIO_QDSS_ETFETB_RWP_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_RWP_ADDR, m)
#define HWIO_QDSS_ETFETB_RWP_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_RWP_ADDR,v)
#define HWIO_QDSS_ETFETB_RWP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_RWP_ADDR,m,v,HWIO_QDSS_ETFETB_RWP_IN)
#define HWIO_QDSS_ETFETB_RWP_RWP_BMSK                                      0xffff
#define HWIO_QDSS_ETFETB_RWP_RWP_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_TRG_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x0000001c)
#define HWIO_QDSS_ETFETB_TRG_RMSK                                          0x3fff
#define HWIO_QDSS_ETFETB_TRG_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_TRG_ADDR, HWIO_QDSS_ETFETB_TRG_RMSK)
#define HWIO_QDSS_ETFETB_TRG_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_TRG_ADDR, m)
#define HWIO_QDSS_ETFETB_TRG_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_TRG_ADDR,v)
#define HWIO_QDSS_ETFETB_TRG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_TRG_ADDR,m,v,HWIO_QDSS_ETFETB_TRG_IN)
#define HWIO_QDSS_ETFETB_TRG_TRG_BMSK                                      0x3fff
#define HWIO_QDSS_ETFETB_TRG_TRG_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_CTL_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000020)
#define HWIO_QDSS_ETFETB_CTL_RMSK                                             0x1
#define HWIO_QDSS_ETFETB_CTL_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_CTL_ADDR, HWIO_QDSS_ETFETB_CTL_RMSK)
#define HWIO_QDSS_ETFETB_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_CTL_ADDR, m)
#define HWIO_QDSS_ETFETB_CTL_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_CTL_ADDR,v)
#define HWIO_QDSS_ETFETB_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_CTL_ADDR,m,v,HWIO_QDSS_ETFETB_CTL_IN)
#define HWIO_QDSS_ETFETB_CTL_TRACECAPTEN_BMSK                                 0x1
#define HWIO_QDSS_ETFETB_CTL_TRACECAPTEN_SHFT                                 0x0

#define HWIO_QDSS_ETFETB_RWD_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000024)
#define HWIO_QDSS_ETFETB_RWD_RMSK                                      0xffffffff
#define HWIO_QDSS_ETFETB_RWD_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_RWD_ADDR,v)
#define HWIO_QDSS_ETFETB_RWD_RWD_BMSK                                  0xffffffff
#define HWIO_QDSS_ETFETB_RWD_RWD_SHFT                                         0x0

#define HWIO_QDSS_ETFETB_MODE_ADDR                                     (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000028)
#define HWIO_QDSS_ETFETB_MODE_RMSK                                            0x3
#define HWIO_QDSS_ETFETB_MODE_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_MODE_ADDR, HWIO_QDSS_ETFETB_MODE_RMSK)
#define HWIO_QDSS_ETFETB_MODE_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_MODE_ADDR, m)
#define HWIO_QDSS_ETFETB_MODE_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_MODE_ADDR,v)
#define HWIO_QDSS_ETFETB_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_MODE_ADDR,m,v,HWIO_QDSS_ETFETB_MODE_IN)
#define HWIO_QDSS_ETFETB_MODE_MODE_BMSK                                       0x3
#define HWIO_QDSS_ETFETB_MODE_MODE_SHFT                                       0x0

#define HWIO_QDSS_ETFETB_LBUFLEVEL_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x0000002c)
#define HWIO_QDSS_ETFETB_LBUFLEVEL_RMSK                                    0x7fff
#define HWIO_QDSS_ETFETB_LBUFLEVEL_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_LBUFLEVEL_ADDR, HWIO_QDSS_ETFETB_LBUFLEVEL_RMSK)
#define HWIO_QDSS_ETFETB_LBUFLEVEL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_LBUFLEVEL_ADDR, m)
#define HWIO_QDSS_ETFETB_LBUFLEVEL_LBUFLEVEL_BMSK                          0x7fff
#define HWIO_QDSS_ETFETB_LBUFLEVEL_LBUFLEVEL_SHFT                             0x0

#define HWIO_QDSS_ETFETB_CBUFLEVEL_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000030)
#define HWIO_QDSS_ETFETB_CBUFLEVEL_RMSK                                    0x7fff
#define HWIO_QDSS_ETFETB_CBUFLEVEL_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_CBUFLEVEL_ADDR, HWIO_QDSS_ETFETB_CBUFLEVEL_RMSK)
#define HWIO_QDSS_ETFETB_CBUFLEVEL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_CBUFLEVEL_ADDR, m)
#define HWIO_QDSS_ETFETB_CBUFLEVEL_CBUFLEVEL_BMSK                          0x7fff
#define HWIO_QDSS_ETFETB_CBUFLEVEL_CBUFLEVEL_SHFT                             0x0

#define HWIO_QDSS_ETFETB_BUFWM_ADDR                                    (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000034)
#define HWIO_QDSS_ETFETB_BUFWM_RMSK                                        0x3fff
#define HWIO_QDSS_ETFETB_BUFWM_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_BUFWM_ADDR, HWIO_QDSS_ETFETB_BUFWM_RMSK)
#define HWIO_QDSS_ETFETB_BUFWM_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_BUFWM_ADDR, m)
#define HWIO_QDSS_ETFETB_BUFWM_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_BUFWM_ADDR,v)
#define HWIO_QDSS_ETFETB_BUFWM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_BUFWM_ADDR,m,v,HWIO_QDSS_ETFETB_BUFWM_IN)
#define HWIO_QDSS_ETFETB_BUFWM_BUFWM_BMSK                                  0x3fff
#define HWIO_QDSS_ETFETB_BUFWM_BUFWM_SHFT                                     0x0

#define HWIO_QDSS_ETFETB_FFSR_ADDR                                     (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000300)
#define HWIO_QDSS_ETFETB_FFSR_RMSK                                            0x3
#define HWIO_QDSS_ETFETB_FFSR_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_FFSR_ADDR, HWIO_QDSS_ETFETB_FFSR_RMSK)
#define HWIO_QDSS_ETFETB_FFSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_FFSR_ADDR, m)
#define HWIO_QDSS_ETFETB_FFSR_FTSTOPPED_BMSK                                  0x2
#define HWIO_QDSS_ETFETB_FFSR_FTSTOPPED_SHFT                                  0x1
#define HWIO_QDSS_ETFETB_FFSR_FLINPROG_BMSK                                   0x1
#define HWIO_QDSS_ETFETB_FFSR_FLINPROG_SHFT                                   0x0

#define HWIO_QDSS_ETFETB_FFCR_ADDR                                     (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000304)
#define HWIO_QDSS_ETFETB_FFCR_RMSK                                         0x7773
#define HWIO_QDSS_ETFETB_FFCR_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_FFCR_ADDR, HWIO_QDSS_ETFETB_FFCR_RMSK)
#define HWIO_QDSS_ETFETB_FFCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_FFCR_ADDR, m)
#define HWIO_QDSS_ETFETB_FFCR_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_FFCR_ADDR,v)
#define HWIO_QDSS_ETFETB_FFCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_FFCR_ADDR,m,v,HWIO_QDSS_ETFETB_FFCR_IN)
#define HWIO_QDSS_ETFETB_FFCR_DRAINBUFFER_BMSK                             0x4000
#define HWIO_QDSS_ETFETB_FFCR_DRAINBUFFER_SHFT                                0xe
#define HWIO_QDSS_ETFETB_FFCR_STOPONTRIGEVT_BMSK                           0x2000
#define HWIO_QDSS_ETFETB_FFCR_STOPONTRIGEVT_SHFT                              0xd
#define HWIO_QDSS_ETFETB_FFCR_STOPONFL_BMSK                                0x1000
#define HWIO_QDSS_ETFETB_FFCR_STOPONFL_SHFT                                   0xc
#define HWIO_QDSS_ETFETB_FFCR_TRIGONFL_BMSK                                 0x400
#define HWIO_QDSS_ETFETB_FFCR_TRIGONFL_SHFT                                   0xa
#define HWIO_QDSS_ETFETB_FFCR_TRIGONTRIGEVT_BMSK                            0x200
#define HWIO_QDSS_ETFETB_FFCR_TRIGONTRIGEVT_SHFT                              0x9
#define HWIO_QDSS_ETFETB_FFCR_TRIGONTRIGIN_BMSK                             0x100
#define HWIO_QDSS_ETFETB_FFCR_TRIGONTRIGIN_SHFT                               0x8
#define HWIO_QDSS_ETFETB_FFCR_FLUSHMAN_R_BMSK                                0x40
#define HWIO_QDSS_ETFETB_FFCR_FLUSHMAN_R_SHFT                                 0x6
#define HWIO_QDSS_ETFETB_FFCR_FLUSHMAN_W_BMSK                                0x40
#define HWIO_QDSS_ETFETB_FFCR_FLUSHMAN_W_SHFT                                 0x6
#define HWIO_QDSS_ETFETB_FFCR_FONTRIGEVT_BMSK                                0x20
#define HWIO_QDSS_ETFETB_FFCR_FONTRIGEVT_SHFT                                 0x5
#define HWIO_QDSS_ETFETB_FFCR_FONFLIN_BMSK                                   0x10
#define HWIO_QDSS_ETFETB_FFCR_FONFLIN_SHFT                                    0x4
#define HWIO_QDSS_ETFETB_FFCR_ENTI_BMSK                                       0x2
#define HWIO_QDSS_ETFETB_FFCR_ENTI_SHFT                                       0x1
#define HWIO_QDSS_ETFETB_FFCR_ENFT_BMSK                                       0x1
#define HWIO_QDSS_ETFETB_FFCR_ENFT_SHFT                                       0x0

#define HWIO_QDSS_ETFETB_PSCR_ADDR                                     (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000308)
#define HWIO_QDSS_ETFETB_PSCR_RMSK                                           0x1f
#define HWIO_QDSS_ETFETB_PSCR_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PSCR_ADDR, HWIO_QDSS_ETFETB_PSCR_RMSK)
#define HWIO_QDSS_ETFETB_PSCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PSCR_ADDR, m)
#define HWIO_QDSS_ETFETB_PSCR_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_PSCR_ADDR,v)
#define HWIO_QDSS_ETFETB_PSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_PSCR_ADDR,m,v,HWIO_QDSS_ETFETB_PSCR_IN)
#define HWIO_QDSS_ETFETB_PSCR_PSCOUNT_BMSK                                   0x1f
#define HWIO_QDSS_ETFETB_PSCR_PSCOUNT_SHFT                                    0x0

#define HWIO_QDSS_ETFETB_ITATBMDATA0_ADDR                              (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ed0)
#define HWIO_QDSS_ETFETB_ITATBMDATA0_RMSK                                 0x1ffff
#define HWIO_QDSS_ETFETB_ITATBMDATA0_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITATBMDATA0_ADDR,v)
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT127_BMSK                   0x10000
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT127_SHFT                      0x10
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT119_BMSK                    0x8000
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT119_SHFT                       0xf
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT111_BMSK                    0x4000
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT111_SHFT                       0xe
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT103_BMSK                    0x2000
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT103_SHFT                       0xd
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT95_BMSK                     0x1000
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT95_SHFT                        0xc
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT87_BMSK                      0x800
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT87_SHFT                        0xb
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT79_BMSK                      0x400
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT79_SHFT                        0xa
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT71_BMSK                      0x200
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT71_SHFT                        0x9
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT63_BMSK                      0x100
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT63_SHFT                        0x8
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT55_BMSK                       0x80
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT55_SHFT                        0x7
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT47_BMSK                       0x40
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT47_SHFT                        0x6
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT39_BMSK                       0x20
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT39_SHFT                        0x5
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT31_BMSK                       0x10
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT31_SHFT                        0x4
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT23_BMSK                        0x8
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT23_SHFT                        0x3
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT15_BMSK                        0x4
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT15_SHFT                        0x2
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT7_BMSK                         0x2
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT7_SHFT                         0x1
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT0_BMSK                         0x1
#define HWIO_QDSS_ETFETB_ITATBMDATA0_ATDATAMBIT0_SHFT                         0x0

#define HWIO_QDSS_ETFETB_ITATBMCTR2_ADDR                               (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ed4)
#define HWIO_QDSS_ETFETB_ITATBMCTR2_RMSK                                      0x7
#define HWIO_QDSS_ETFETB_ITATBMCTR2_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBMCTR2_ADDR, HWIO_QDSS_ETFETB_ITATBMCTR2_RMSK)
#define HWIO_QDSS_ETFETB_ITATBMCTR2_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBMCTR2_ADDR, m)
#define HWIO_QDSS_ETFETB_ITATBMCTR2_SYNCREQM_BMSK                             0x4
#define HWIO_QDSS_ETFETB_ITATBMCTR2_SYNCREQM_SHFT                             0x2
#define HWIO_QDSS_ETFETB_ITATBMCTR2_AFVALIDM_BMSK                             0x2
#define HWIO_QDSS_ETFETB_ITATBMCTR2_AFVALIDM_SHFT                             0x1
#define HWIO_QDSS_ETFETB_ITATBMCTR2_ATREADYM_BMSK                             0x1
#define HWIO_QDSS_ETFETB_ITATBMCTR2_ATREADYM_SHFT                             0x0

#define HWIO_QDSS_ETFETB_ITATBMCTR1_ADDR                               (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ed8)
#define HWIO_QDSS_ETFETB_ITATBMCTR1_RMSK                                     0x7f
#define HWIO_QDSS_ETFETB_ITATBMCTR1_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITATBMCTR1_ADDR,v)
#define HWIO_QDSS_ETFETB_ITATBMCTR1_ATIDM_BMSK                               0x7f
#define HWIO_QDSS_ETFETB_ITATBMCTR1_ATIDM_SHFT                                0x0

#define HWIO_QDSS_ETFETB_ITATBMCTR0_ADDR                               (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000edc)
#define HWIO_QDSS_ETFETB_ITATBMCTR0_RMSK                                    0xf03
#define HWIO_QDSS_ETFETB_ITATBMCTR0_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITATBMCTR0_ADDR,v)
#define HWIO_QDSS_ETFETB_ITATBMCTR0_ATBYTESM_BMSK                           0xf00
#define HWIO_QDSS_ETFETB_ITATBMCTR0_ATBYTESM_SHFT                             0x8
#define HWIO_QDSS_ETFETB_ITATBMCTR0_AFREADYM_BMSK                             0x2
#define HWIO_QDSS_ETFETB_ITATBMCTR0_AFREADYM_SHFT                             0x1
#define HWIO_QDSS_ETFETB_ITATBMCTR0_ATVALIDM_BMSK                             0x1
#define HWIO_QDSS_ETFETB_ITATBMCTR0_ATVALIDM_SHFT                             0x0

#define HWIO_QDSS_ETFETB_ITMISCOP0_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ee0)
#define HWIO_QDSS_ETFETB_ITMISCOP0_RMSK                                       0x3
#define HWIO_QDSS_ETFETB_ITMISCOP0_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITMISCOP0_ADDR,v)
#define HWIO_QDSS_ETFETB_ITMISCOP0_FULL_BMSK                                  0x2
#define HWIO_QDSS_ETFETB_ITMISCOP0_FULL_SHFT                                  0x1
#define HWIO_QDSS_ETFETB_ITMISCOP0_ACQCOMP_BMSK                               0x1
#define HWIO_QDSS_ETFETB_ITMISCOP0_ACQCOMP_SHFT                               0x0

#define HWIO_QDSS_ETFETB_ITTRFLIN_ADDR                                 (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ee8)
#define HWIO_QDSS_ETFETB_ITTRFLIN_RMSK                                        0x3
#define HWIO_QDSS_ETFETB_ITTRFLIN_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITTRFLIN_ADDR, HWIO_QDSS_ETFETB_ITTRFLIN_RMSK)
#define HWIO_QDSS_ETFETB_ITTRFLIN_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITTRFLIN_ADDR, m)
#define HWIO_QDSS_ETFETB_ITTRFLIN_FLUSHIN_BMSK                                0x2
#define HWIO_QDSS_ETFETB_ITTRFLIN_FLUSHIN_SHFT                                0x1
#define HWIO_QDSS_ETFETB_ITTRFLIN_TRIGIN_BMSK                                 0x1
#define HWIO_QDSS_ETFETB_ITTRFLIN_TRIGIN_SHFT                                 0x0

#define HWIO_QDSS_ETFETB_ITATBDATA0_ADDR                               (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000eec)
#define HWIO_QDSS_ETFETB_ITATBDATA0_RMSK                                  0x1ffff
#define HWIO_QDSS_ETFETB_ITATBDATA0_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBDATA0_ADDR, HWIO_QDSS_ETFETB_ITATBDATA0_RMSK)
#define HWIO_QDSS_ETFETB_ITATBDATA0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBDATA0_ADDR, m)
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT127_BMSK                    0x10000
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT127_SHFT                       0x10
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT119_BMSK                     0x8000
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT119_SHFT                        0xf
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT111_BMSK                     0x4000
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT111_SHFT                        0xe
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT103_BMSK                     0x2000
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT103_SHFT                        0xd
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT95_BMSK                      0x1000
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT95_SHFT                         0xc
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT87_BMSK                       0x800
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT87_SHFT                         0xb
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT79_BMSK                       0x400
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT79_SHFT                         0xa
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT71_BMSK                       0x200
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT71_SHFT                         0x9
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT63_BMSK                       0x100
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT63_SHFT                         0x8
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT55_BMSK                        0x80
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT55_SHFT                         0x7
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT47_BMSK                        0x40
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT47_SHFT                         0x6
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT39_BMSK                        0x20
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT39_SHFT                         0x5
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT31_BMSK                        0x10
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT31_SHFT                         0x4
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT23_BMSK                         0x8
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT23_SHFT                         0x3
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT15_BMSK                         0x4
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT15_SHFT                         0x2
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT7_BMSK                          0x2
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT7_SHFT                          0x1
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT0_BMSK                          0x1
#define HWIO_QDSS_ETFETB_ITATBDATA0_ATDATASBIT0_SHFT                          0x0

#define HWIO_QDSS_ETFETB_ITATBCTR2_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ef0)
#define HWIO_QDSS_ETFETB_ITATBCTR2_RMSK                                       0x7
#define HWIO_QDSS_ETFETB_ITATBCTR2_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITATBCTR2_ADDR,v)
#define HWIO_QDSS_ETFETB_ITATBCTR2_SYNCREQS_BMSK                              0x4
#define HWIO_QDSS_ETFETB_ITATBCTR2_SYNCREQS_SHFT                              0x2
#define HWIO_QDSS_ETFETB_ITATBCTR2_AFVALIDS_BMSK                              0x2
#define HWIO_QDSS_ETFETB_ITATBCTR2_AFVALIDS_SHFT                              0x1
#define HWIO_QDSS_ETFETB_ITATBCTR2_ATREADYS_BMSK                              0x1
#define HWIO_QDSS_ETFETB_ITATBCTR2_ATREADYS_SHFT                              0x0

#define HWIO_QDSS_ETFETB_ITATBCTR1_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ef4)
#define HWIO_QDSS_ETFETB_ITATBCTR1_RMSK                                      0x7f
#define HWIO_QDSS_ETFETB_ITATBCTR1_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBCTR1_ADDR, HWIO_QDSS_ETFETB_ITATBCTR1_RMSK)
#define HWIO_QDSS_ETFETB_ITATBCTR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBCTR1_ADDR, m)
#define HWIO_QDSS_ETFETB_ITATBCTR1_ATIDS_BMSK                                0x7f
#define HWIO_QDSS_ETFETB_ITATBCTR1_ATIDS_SHFT                                 0x0

#define HWIO_QDSS_ETFETB_ITATBCTR0_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_ETFETB_ITATBCTR0_RMSK                                     0xf03
#define HWIO_QDSS_ETFETB_ITATBCTR0_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBCTR0_ADDR, HWIO_QDSS_ETFETB_ITATBCTR0_RMSK)
#define HWIO_QDSS_ETFETB_ITATBCTR0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITATBCTR0_ADDR, m)
#define HWIO_QDSS_ETFETB_ITATBCTR0_ATBYTESS_BMSK                            0xf00
#define HWIO_QDSS_ETFETB_ITATBCTR0_ATBYTESS_SHFT                              0x8
#define HWIO_QDSS_ETFETB_ITATBCTR0_AFREADYS_BMSK                              0x2
#define HWIO_QDSS_ETFETB_ITATBCTR0_AFREADYS_SHFT                              0x1
#define HWIO_QDSS_ETFETB_ITATBCTR0_ATVALIDS_BMSK                              0x1
#define HWIO_QDSS_ETFETB_ITATBCTR0_ATVALIDS_SHFT                              0x0

#define HWIO_QDSS_ETFETB_ITCTRL_ADDR                                   (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_ETFETB_ITCTRL_RMSK                                          0x1
#define HWIO_QDSS_ETFETB_ITCTRL_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_ITCTRL_ADDR, HWIO_QDSS_ETFETB_ITCTRL_RMSK)
#define HWIO_QDSS_ETFETB_ITCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_ITCTRL_ADDR, m)
#define HWIO_QDSS_ETFETB_ITCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_ITCTRL_ADDR,v)
#define HWIO_QDSS_ETFETB_ITCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_ITCTRL_ADDR,m,v,HWIO_QDSS_ETFETB_ITCTRL_IN)
#define HWIO_QDSS_ETFETB_ITCTRL_INTEGRATION_MODE_BMSK                         0x1
#define HWIO_QDSS_ETFETB_ITCTRL_INTEGRATION_MODE_SHFT                         0x0

#define HWIO_QDSS_ETFETB_CLAIMSET_ADDR                                 (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_ETFETB_CLAIMSET_RMSK                                        0xf
#define HWIO_QDSS_ETFETB_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_CLAIMSET_ADDR, HWIO_QDSS_ETFETB_CLAIMSET_RMSK)
#define HWIO_QDSS_ETFETB_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_CLAIMSET_ADDR, m)
#define HWIO_QDSS_ETFETB_CLAIMSET_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_CLAIMSET_ADDR,v)
#define HWIO_QDSS_ETFETB_CLAIMSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_CLAIMSET_ADDR,m,v,HWIO_QDSS_ETFETB_CLAIMSET_IN)
#define HWIO_QDSS_ETFETB_CLAIMSET_CLAIMSET_W_BMSK                             0xf
#define HWIO_QDSS_ETFETB_CLAIMSET_CLAIMSET_W_SHFT                             0x0
#define HWIO_QDSS_ETFETB_CLAIMSET_CLAIMSET_R_BMSK                             0xf
#define HWIO_QDSS_ETFETB_CLAIMSET_CLAIMSET_R_SHFT                             0x0

#define HWIO_QDSS_ETFETB_CLAIMCLR_ADDR                                 (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_ETFETB_CLAIMCLR_RMSK                                        0xf
#define HWIO_QDSS_ETFETB_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_CLAIMCLR_ADDR, HWIO_QDSS_ETFETB_CLAIMCLR_RMSK)
#define HWIO_QDSS_ETFETB_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_ETFETB_CLAIMCLR_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_CLAIMCLR_ADDR,v)
#define HWIO_QDSS_ETFETB_CLAIMCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_CLAIMCLR_ADDR,m,v,HWIO_QDSS_ETFETB_CLAIMCLR_IN)
#define HWIO_QDSS_ETFETB_CLAIMCLR_CLAIMCLR_W_BMSK                             0xf
#define HWIO_QDSS_ETFETB_CLAIMCLR_CLAIMCLR_W_SHFT                             0x0
#define HWIO_QDSS_ETFETB_CLAIMCLR_CLAIMCLR_R_BMSK                             0xf
#define HWIO_QDSS_ETFETB_CLAIMCLR_CLAIMCLR_R_SHFT                             0x0

#define HWIO_QDSS_ETFETB_LAR_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_ETFETB_LAR_RMSK                                      0xffffffff
#define HWIO_QDSS_ETFETB_LAR_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_LAR_ADDR,v)
#define HWIO_QDSS_ETFETB_LAR_ACCESS_W_BMSK                             0xffffffff
#define HWIO_QDSS_ETFETB_LAR_ACCESS_W_SHFT                                    0x0

#define HWIO_QDSS_ETFETB_LSR_ADDR                                      (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_ETFETB_LSR_RMSK                                             0x7
#define HWIO_QDSS_ETFETB_LSR_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_LSR_ADDR, HWIO_QDSS_ETFETB_LSR_RMSK)
#define HWIO_QDSS_ETFETB_LSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_LSR_ADDR, m)
#define HWIO_QDSS_ETFETB_LSR_LOCKTYPE_BMSK                                    0x4
#define HWIO_QDSS_ETFETB_LSR_LOCKTYPE_SHFT                                    0x2
#define HWIO_QDSS_ETFETB_LSR_LOCKGRANT_BMSK                                   0x2
#define HWIO_QDSS_ETFETB_LSR_LOCKGRANT_SHFT                                   0x1
#define HWIO_QDSS_ETFETB_LSR_LOCKEXIST_BMSK                                   0x1
#define HWIO_QDSS_ETFETB_LSR_LOCKEXIST_SHFT                                   0x0

#define HWIO_QDSS_ETFETB_AUTHSTATUS_ADDR                               (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_ETFETB_AUTHSTATUS_RMSK                                     0xff
#define HWIO_QDSS_ETFETB_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_AUTHSTATUS_ADDR, HWIO_QDSS_ETFETB_AUTHSTATUS_RMSK)
#define HWIO_QDSS_ETFETB_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_ETFETB_AUTHSTATUS_SNID_BMSK                                0xc0
#define HWIO_QDSS_ETFETB_AUTHSTATUS_SNID_SHFT                                 0x6
#define HWIO_QDSS_ETFETB_AUTHSTATUS_SID_BMSK                                 0x30
#define HWIO_QDSS_ETFETB_AUTHSTATUS_SID_SHFT                                  0x4
#define HWIO_QDSS_ETFETB_AUTHSTATUS_NSNID_BMSK                                0xc
#define HWIO_QDSS_ETFETB_AUTHSTATUS_NSNID_SHFT                                0x2
#define HWIO_QDSS_ETFETB_AUTHSTATUS_NSID_BMSK                                 0x3
#define HWIO_QDSS_ETFETB_AUTHSTATUS_NSID_SHFT                                 0x0

#define HWIO_QDSS_ETFETB_DEVID_ADDR                                    (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_ETFETB_DEVID_RMSK                                         0x7ff
#define HWIO_QDSS_ETFETB_DEVID_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_DEVID_ADDR, HWIO_QDSS_ETFETB_DEVID_RMSK)
#define HWIO_QDSS_ETFETB_DEVID_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_DEVID_ADDR, m)
#define HWIO_QDSS_ETFETB_DEVID_MEMWIDTH_BMSK                                0x700
#define HWIO_QDSS_ETFETB_DEVID_MEMWIDTH_SHFT                                  0x8
#define HWIO_QDSS_ETFETB_DEVID_CONFIGTYPE_BMSK                               0xc0
#define HWIO_QDSS_ETFETB_DEVID_CONFIGTYPE_SHFT                                0x6
#define HWIO_QDSS_ETFETB_DEVID_CLKSCHEME_BMSK                                0x20
#define HWIO_QDSS_ETFETB_DEVID_CLKSCHEME_SHFT                                 0x5
#define HWIO_QDSS_ETFETB_DEVID_ATBINPORTCOUNT_BMSK                           0x1f
#define HWIO_QDSS_ETFETB_DEVID_ATBINPORTCOUNT_SHFT                            0x0

#define HWIO_QDSS_ETFETB_DEVTYPE_ADDR                                  (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_ETFETB_DEVTYPE_RMSK                                        0xff
#define HWIO_QDSS_ETFETB_DEVTYPE_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_DEVTYPE_ADDR, HWIO_QDSS_ETFETB_DEVTYPE_RMSK)
#define HWIO_QDSS_ETFETB_DEVTYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_DEVTYPE_ADDR, m)
#define HWIO_QDSS_ETFETB_DEVTYPE_SUB_TYPE_BMSK                               0xf0
#define HWIO_QDSS_ETFETB_DEVTYPE_SUB_TYPE_SHFT                                0x4
#define HWIO_QDSS_ETFETB_DEVTYPE_MAJOR_TYPE_BMSK                              0xf
#define HWIO_QDSS_ETFETB_DEVTYPE_MAJOR_TYPE_SHFT                              0x0

#define HWIO_QDSS_ETFETB_PERIPHID0_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_ETFETB_PERIPHID0_RMSK                                      0xff
#define HWIO_QDSS_ETFETB_PERIPHID0_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID0_ADDR, HWIO_QDSS_ETFETB_PERIPHID0_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID0_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID0_PART_NUMBER_BITS7TO0_BMSK                 0xff
#define HWIO_QDSS_ETFETB_PERIPHID0_PART_NUMBER_BITS7TO0_SHFT                  0x0

#define HWIO_QDSS_ETFETB_PERIPHID1_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_ETFETB_PERIPHID1_RMSK                                      0xff
#define HWIO_QDSS_ETFETB_PERIPHID1_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID1_ADDR, HWIO_QDSS_ETFETB_PERIPHID1_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID1_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID1_JEP106_BITS3TO0_BMSK                      0xf0
#define HWIO_QDSS_ETFETB_PERIPHID1_JEP106_BITS3TO0_SHFT                       0x4
#define HWIO_QDSS_ETFETB_PERIPHID1_PART_NUMBER_BITS11TO8_BMSK                 0xf
#define HWIO_QDSS_ETFETB_PERIPHID1_PART_NUMBER_BITS11TO8_SHFT                 0x0

#define HWIO_QDSS_ETFETB_PERIPHID2_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_ETFETB_PERIPHID2_RMSK                                      0xff
#define HWIO_QDSS_ETFETB_PERIPHID2_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID2_ADDR, HWIO_QDSS_ETFETB_PERIPHID2_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID2_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID2_REVISION_BMSK                             0xf0
#define HWIO_QDSS_ETFETB_PERIPHID2_REVISION_SHFT                              0x4
#define HWIO_QDSS_ETFETB_PERIPHID2_JEDEC_BMSK                                 0x8
#define HWIO_QDSS_ETFETB_PERIPHID2_JEDEC_SHFT                                 0x3
#define HWIO_QDSS_ETFETB_PERIPHID2_JEP106_BITS6TO4_BMSK                       0x7
#define HWIO_QDSS_ETFETB_PERIPHID2_JEP106_BITS6TO4_SHFT                       0x0

#define HWIO_QDSS_ETFETB_PERIPHID3_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_ETFETB_PERIPHID3_RMSK                                      0xff
#define HWIO_QDSS_ETFETB_PERIPHID3_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID3_ADDR, HWIO_QDSS_ETFETB_PERIPHID3_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID3_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID3_REVAND_BMSK                               0xf0
#define HWIO_QDSS_ETFETB_PERIPHID3_REVAND_SHFT                                0x4
#define HWIO_QDSS_ETFETB_PERIPHID3_CUSTOMER_MODIFIED_BMSK                     0xf
#define HWIO_QDSS_ETFETB_PERIPHID3_CUSTOMER_MODIFIED_SHFT                     0x0

#define HWIO_QDSS_ETFETB_PERIPHID4_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_ETFETB_PERIPHID4_RMSK                                      0xff
#define HWIO_QDSS_ETFETB_PERIPHID4_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID4_ADDR, HWIO_QDSS_ETFETB_PERIPHID4_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID4_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID4_FOURKB_COUNT_BMSK                         0xf0
#define HWIO_QDSS_ETFETB_PERIPHID4_FOURKB_COUNT_SHFT                          0x4
#define HWIO_QDSS_ETFETB_PERIPHID4_JEP106_CONT_BMSK                           0xf
#define HWIO_QDSS_ETFETB_PERIPHID4_JEP106_CONT_SHFT                           0x0

#define HWIO_QDSS_ETFETB_PERIPHID5_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_ETFETB_PERIPHID5_RMSK                                0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID5_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID5_ADDR, HWIO_QDSS_ETFETB_PERIPHID5_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID5_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID5_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_PERIPHID5_ADDR,v)
#define HWIO_QDSS_ETFETB_PERIPHID5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_PERIPHID5_ADDR,m,v,HWIO_QDSS_ETFETB_PERIPHID5_IN)
#define HWIO_QDSS_ETFETB_PERIPHID5_PERIPHID5_BMSK                      0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID5_PERIPHID5_SHFT                             0x0

#define HWIO_QDSS_ETFETB_PERIPHID6_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_ETFETB_PERIPHID6_RMSK                                0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID6_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID6_ADDR, HWIO_QDSS_ETFETB_PERIPHID6_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID6_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID6_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_PERIPHID6_ADDR,v)
#define HWIO_QDSS_ETFETB_PERIPHID6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_PERIPHID6_ADDR,m,v,HWIO_QDSS_ETFETB_PERIPHID6_IN)
#define HWIO_QDSS_ETFETB_PERIPHID6_PERIPHID6_BMSK                      0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID6_PERIPHID6_SHFT                             0x0

#define HWIO_QDSS_ETFETB_PERIPHID7_ADDR                                (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_ETFETB_PERIPHID7_RMSK                                0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID7_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID7_ADDR, HWIO_QDSS_ETFETB_PERIPHID7_RMSK)
#define HWIO_QDSS_ETFETB_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_PERIPHID7_ADDR, m)
#define HWIO_QDSS_ETFETB_PERIPHID7_OUT(v)      \
        out_dword(HWIO_QDSS_ETFETB_PERIPHID7_ADDR,v)
#define HWIO_QDSS_ETFETB_PERIPHID7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETFETB_PERIPHID7_ADDR,m,v,HWIO_QDSS_ETFETB_PERIPHID7_IN)
#define HWIO_QDSS_ETFETB_PERIPHID7_PERIPHID7_BMSK                      0xffffffff
#define HWIO_QDSS_ETFETB_PERIPHID7_PERIPHID7_SHFT                             0x0

#define HWIO_QDSS_ETFETB_COMPID0_ADDR                                  (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_ETFETB_COMPID0_RMSK                                        0xff
#define HWIO_QDSS_ETFETB_COMPID0_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID0_ADDR, HWIO_QDSS_ETFETB_COMPID0_RMSK)
#define HWIO_QDSS_ETFETB_COMPID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID0_ADDR, m)
#define HWIO_QDSS_ETFETB_COMPID0_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETFETB_COMPID0_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETFETB_COMPID1_ADDR                                  (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_ETFETB_COMPID1_RMSK                                        0xff
#define HWIO_QDSS_ETFETB_COMPID1_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID1_ADDR, HWIO_QDSS_ETFETB_COMPID1_RMSK)
#define HWIO_QDSS_ETFETB_COMPID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID1_ADDR, m)
#define HWIO_QDSS_ETFETB_COMPID1_CLASS_BMSK                                  0xf0
#define HWIO_QDSS_ETFETB_COMPID1_CLASS_SHFT                                   0x4
#define HWIO_QDSS_ETFETB_COMPID1_PREAMBLE_BMSK                                0xf
#define HWIO_QDSS_ETFETB_COMPID1_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETFETB_COMPID2_ADDR                                  (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_ETFETB_COMPID2_RMSK                                        0xff
#define HWIO_QDSS_ETFETB_COMPID2_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID2_ADDR, HWIO_QDSS_ETFETB_COMPID2_RMSK)
#define HWIO_QDSS_ETFETB_COMPID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID2_ADDR, m)
#define HWIO_QDSS_ETFETB_COMPID2_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETFETB_COMPID2_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETFETB_COMPID3_ADDR                                  (QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_ETFETB_COMPID3_RMSK                                        0xff
#define HWIO_QDSS_ETFETB_COMPID3_IN          \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID3_ADDR, HWIO_QDSS_ETFETB_COMPID3_RMSK)
#define HWIO_QDSS_ETFETB_COMPID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETFETB_COMPID3_ADDR, m)
#define HWIO_QDSS_ETFETB_COMPID3_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETFETB_COMPID3_PREAMBLE_SHFT                                0x0


#endif /* __QDSS_ETFETB_REGS_H__ */
