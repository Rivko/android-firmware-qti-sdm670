#ifndef __QDSS_ETR_REGS_H__
#define __QDSS_ETR_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_etr_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSS_ETR_ETR_CXTMC_R64W32D

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tmc/hal/src/qdss_etr_regs.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"


/*----------------------------------------------------------------------------
 * MODULE: QDSS_ETR_ETR_CXTMC_R64W32D
 *--------------------------------------------------------------------------*/
extern volatile uint32 QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE;


#define HWIO_QDSS_ETR_RSZ_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000004)
#define HWIO_QDSS_ETR_RSZ_RMSK                                      0x7fffffff
#define HWIO_QDSS_ETR_RSZ_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RSZ_ADDR, HWIO_QDSS_ETR_RSZ_RMSK)
#define HWIO_QDSS_ETR_RSZ_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RSZ_ADDR, m)
#define HWIO_QDSS_ETR_RSZ_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RSZ_ADDR,v)
#define HWIO_QDSS_ETR_RSZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_RSZ_ADDR,m,v,HWIO_QDSS_ETR_RSZ_IN)
#define HWIO_QDSS_ETR_RSZ_RSZ_ETR_BMSK                              0x7fffffff
#define HWIO_QDSS_ETR_RSZ_RSZ_ETR_SHFT                                     0x0

#define HWIO_QDSS_ETR_STS_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x0000000c)
#define HWIO_QDSS_ETR_STS_RMSK                                            0x3f
#define HWIO_QDSS_ETR_STS_IN          \
        in_dword_masked(HWIO_QDSS_ETR_STS_ADDR, HWIO_QDSS_ETR_STS_RMSK)
#define HWIO_QDSS_ETR_STS_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_STS_ADDR, m)
#define HWIO_QDSS_ETR_STS_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_STS_ADDR,v)
#define HWIO_QDSS_ETR_STS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_STS_ADDR,m,v,HWIO_QDSS_ETR_STS_IN)
#define HWIO_QDSS_ETR_STS_MEMERR_W_BMSK                                   0x20
#define HWIO_QDSS_ETR_STS_MEMERR_W_SHFT                                    0x5
#define HWIO_QDSS_ETR_STS_MEMERR_R_BMSK                                   0x20
#define HWIO_QDSS_ETR_STS_MEMERR_R_SHFT                                    0x5
#define HWIO_QDSS_ETR_STS_EMPTY_BMSK                                      0x10
#define HWIO_QDSS_ETR_STS_EMPTY_SHFT                                       0x4
#define HWIO_QDSS_ETR_STS_FTEMPTY_BMSK                                     0x8
#define HWIO_QDSS_ETR_STS_FTEMPTY_SHFT                                     0x3
#define HWIO_QDSS_ETR_STS_TMCREADY_BMSK                                    0x4
#define HWIO_QDSS_ETR_STS_TMCREADY_SHFT                                    0x2
#define HWIO_QDSS_ETR_STS_TRIGGERED_BMSK                                   0x2
#define HWIO_QDSS_ETR_STS_TRIGGERED_SHFT                                   0x1
#define HWIO_QDSS_ETR_STS_FULL_BMSK                                        0x1
#define HWIO_QDSS_ETR_STS_FULL_SHFT                                        0x0

#define HWIO_QDSS_ETR_RRD_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000010)
#define HWIO_QDSS_ETR_RRD_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_RRD_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RRD_ADDR, HWIO_QDSS_ETR_RRD_RMSK)
#define HWIO_QDSS_ETR_RRD_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RRD_ADDR, m)
#define HWIO_QDSS_ETR_RRD_RRD_BMSK                                  0xffffffff
#define HWIO_QDSS_ETR_RRD_RRD_SHFT                                         0x0

#define HWIO_QDSS_ETR_RRP_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000014)
#define HWIO_QDSS_ETR_RRP_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_RRP_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RRP_ADDR, HWIO_QDSS_ETR_RRP_RMSK)
#define HWIO_QDSS_ETR_RRP_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RRP_ADDR, m)
#define HWIO_QDSS_ETR_RRP_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RRP_ADDR,v)
#define HWIO_QDSS_ETR_RRP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_RRP_ADDR,m,v,HWIO_QDSS_ETR_RRP_IN)
#define HWIO_QDSS_ETR_RRP_RRP_BMSK                                  0xffffffff
#define HWIO_QDSS_ETR_RRP_RRP_SHFT                                         0x0

#define HWIO_QDSS_ETR_RWP_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000018)
#define HWIO_QDSS_ETR_RWP_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_RWP_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RWP_ADDR, HWIO_QDSS_ETR_RWP_RMSK)
#define HWIO_QDSS_ETR_RWP_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RWP_ADDR, m)
#define HWIO_QDSS_ETR_RWP_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RWP_ADDR,v)
#define HWIO_QDSS_ETR_RWP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_RWP_ADDR,m,v,HWIO_QDSS_ETR_RWP_IN)
#define HWIO_QDSS_ETR_RWP_RWP_BMSK                                  0xffffffff
#define HWIO_QDSS_ETR_RWP_RWP_SHFT                                         0x0

#define HWIO_QDSS_ETR_TRG_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x0000001c)
#define HWIO_QDSS_ETR_TRG_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_TRG_IN          \
        in_dword_masked(HWIO_QDSS_ETR_TRG_ADDR, HWIO_QDSS_ETR_TRG_RMSK)
#define HWIO_QDSS_ETR_TRG_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_TRG_ADDR, m)
#define HWIO_QDSS_ETR_TRG_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_TRG_ADDR,v)
#define HWIO_QDSS_ETR_TRG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_TRG_ADDR,m,v,HWIO_QDSS_ETR_TRG_IN)
#define HWIO_QDSS_ETR_TRG_TRG_BMSK                                  0xffffffff
#define HWIO_QDSS_ETR_TRG_TRG_SHFT                                         0x0

#define HWIO_QDSS_ETR_CTL_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000020)
#define HWIO_QDSS_ETR_CTL_RMSK                                             0x1
#define HWIO_QDSS_ETR_CTL_IN          \
        in_dword_masked(HWIO_QDSS_ETR_CTL_ADDR, HWIO_QDSS_ETR_CTL_RMSK)
#define HWIO_QDSS_ETR_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_CTL_ADDR, m)
#define HWIO_QDSS_ETR_CTL_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_CTL_ADDR,v)
#define HWIO_QDSS_ETR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_CTL_ADDR,m,v,HWIO_QDSS_ETR_CTL_IN)
#define HWIO_QDSS_ETR_CTL_TRACECAPTEN_BMSK                                 0x1
#define HWIO_QDSS_ETR_CTL_TRACECAPTEN_SHFT                                 0x0

#define HWIO_QDSS_ETR_RWD_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000024)
#define HWIO_QDSS_ETR_RWD_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_RWD_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RWD_ADDR,v)
#define HWIO_QDSS_ETR_RWD_RWD_BMSK                                  0xffffffff
#define HWIO_QDSS_ETR_RWD_RWD_SHFT                                         0x0

#define HWIO_QDSS_ETR_MODE_ADDR                                     (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000028)
#define HWIO_QDSS_ETR_MODE_RMSK                                            0x3
#define HWIO_QDSS_ETR_MODE_IN          \
        in_dword_masked(HWIO_QDSS_ETR_MODE_ADDR, HWIO_QDSS_ETR_MODE_RMSK)
#define HWIO_QDSS_ETR_MODE_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_MODE_ADDR, m)
#define HWIO_QDSS_ETR_MODE_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_MODE_ADDR,v)
#define HWIO_QDSS_ETR_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_MODE_ADDR,m,v,HWIO_QDSS_ETR_MODE_IN)
#define HWIO_QDSS_ETR_MODE_MODE_BMSK                                       0x3
#define HWIO_QDSS_ETR_MODE_MODE_SHFT                                       0x0

#define HWIO_QDSS_ETR_LBUFLEVEL_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x0000002c)
#define HWIO_QDSS_ETR_LBUFLEVEL_RMSK                                0x7fffffff
#define HWIO_QDSS_ETR_LBUFLEVEL_IN          \
        in_dword_masked(HWIO_QDSS_ETR_LBUFLEVEL_ADDR, HWIO_QDSS_ETR_LBUFLEVEL_RMSK)
#define HWIO_QDSS_ETR_LBUFLEVEL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_LBUFLEVEL_ADDR, m)
#define HWIO_QDSS_ETR_LBUFLEVEL_LBUFLEVEL_BMSK                      0x7fffffff
#define HWIO_QDSS_ETR_LBUFLEVEL_LBUFLEVEL_SHFT                             0x0

#define HWIO_QDSS_ETR_CBUFLEVEL_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000030)
#define HWIO_QDSS_ETR_CBUFLEVEL_RMSK                                0x7fffffff
#define HWIO_QDSS_ETR_CBUFLEVEL_IN          \
        in_dword_masked(HWIO_QDSS_ETR_CBUFLEVEL_ADDR, HWIO_QDSS_ETR_CBUFLEVEL_RMSK)
#define HWIO_QDSS_ETR_CBUFLEVEL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_CBUFLEVEL_ADDR, m)
#define HWIO_QDSS_ETR_CBUFLEVEL_CBUFLEVEL_BMSK                      0x7fffffff
#define HWIO_QDSS_ETR_CBUFLEVEL_CBUFLEVEL_SHFT                             0x0

#define HWIO_QDSS_ETR_BUFWM_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000034)
#define HWIO_QDSS_ETR_BUFWM_RMSK                                    0x3fffffff
#define HWIO_QDSS_ETR_BUFWM_IN          \
        in_dword_masked(HWIO_QDSS_ETR_BUFWM_ADDR, HWIO_QDSS_ETR_BUFWM_RMSK)
#define HWIO_QDSS_ETR_BUFWM_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_BUFWM_ADDR, m)
#define HWIO_QDSS_ETR_BUFWM_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_BUFWM_ADDR,v)
#define HWIO_QDSS_ETR_BUFWM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_BUFWM_ADDR,m,v,HWIO_QDSS_ETR_BUFWM_IN)
#define HWIO_QDSS_ETR_BUFWM_BUFWM_BMSK                              0x3fffffff
#define HWIO_QDSS_ETR_BUFWM_BUFWM_SHFT                                     0x0

#define HWIO_QDSS_ETR_RRPHI_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000038)
#define HWIO_QDSS_ETR_RRPHI_RMSK                                          0xff
#define HWIO_QDSS_ETR_RRPHI_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RRPHI_ADDR, HWIO_QDSS_ETR_RRPHI_RMSK)
#define HWIO_QDSS_ETR_RRPHI_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RRPHI_ADDR, m)
#define HWIO_QDSS_ETR_RRPHI_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RRPHI_ADDR,v)
#define HWIO_QDSS_ETR_RRPHI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_RRPHI_ADDR,m,v,HWIO_QDSS_ETR_RRPHI_IN)
#define HWIO_QDSS_ETR_RRPHI_RRPHI_BMSK                                    0xff
#define HWIO_QDSS_ETR_RRPHI_RRPHI_SHFT                                     0x0

#define HWIO_QDSS_ETR_RWPHI_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x0000003c)
#define HWIO_QDSS_ETR_RWPHI_RMSK                                          0xff
#define HWIO_QDSS_ETR_RWPHI_IN          \
        in_dword_masked(HWIO_QDSS_ETR_RWPHI_ADDR, HWIO_QDSS_ETR_RWPHI_RMSK)
#define HWIO_QDSS_ETR_RWPHI_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_RWPHI_ADDR, m)
#define HWIO_QDSS_ETR_RWPHI_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_RWPHI_ADDR,v)
#define HWIO_QDSS_ETR_RWPHI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_RWPHI_ADDR,m,v,HWIO_QDSS_ETR_RWPHI_IN)
#define HWIO_QDSS_ETR_RWPHI_RWPHI_BMSK                                    0xff
#define HWIO_QDSS_ETR_RWPHI_RWPHI_SHFT                                     0x0

#define HWIO_QDSS_ETR_AXICTL_ADDR                                   (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000110)
#define HWIO_QDSS_ETR_AXICTL_RMSK                                        0xfbf
#define HWIO_QDSS_ETR_AXICTL_IN          \
        in_dword_masked(HWIO_QDSS_ETR_AXICTL_ADDR, HWIO_QDSS_ETR_AXICTL_RMSK)
#define HWIO_QDSS_ETR_AXICTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_AXICTL_ADDR, m)
#define HWIO_QDSS_ETR_AXICTL_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_AXICTL_ADDR,v)
#define HWIO_QDSS_ETR_AXICTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_AXICTL_ADDR,m,v,HWIO_QDSS_ETR_AXICTL_IN)
#define HWIO_QDSS_ETR_AXICTL_WRBURSTLEN_BMSK                             0xf00
#define HWIO_QDSS_ETR_AXICTL_WRBURSTLEN_SHFT                               0x8
#define HWIO_QDSS_ETR_AXICTL_SCATTERGATHERMODE_BMSK                       0x80
#define HWIO_QDSS_ETR_AXICTL_SCATTERGATHERMODE_SHFT                        0x7
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT3_BMSK                           0x20
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT3_SHFT                            0x5
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT2_BMSK                           0x10
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT2_SHFT                            0x4
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT1_BMSK                            0x8
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT1_SHFT                            0x3
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT0_BMSK                            0x4
#define HWIO_QDSS_ETR_AXICTL_CACHECTRLBIT0_SHFT                            0x2
#define HWIO_QDSS_ETR_AXICTL_PROTCTRLBIT1_BMSK                             0x2
#define HWIO_QDSS_ETR_AXICTL_PROTCTRLBIT1_SHFT                             0x1
#define HWIO_QDSS_ETR_AXICTL_PROTCTRLBIT0_BMSK                             0x1
#define HWIO_QDSS_ETR_AXICTL_PROTCTRLBIT0_SHFT                             0x0

#define HWIO_QDSS_ETR_DBALO_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000118)
#define HWIO_QDSS_ETR_DBALO_RMSK                                    0xffffffff
#define HWIO_QDSS_ETR_DBALO_IN          \
        in_dword_masked(HWIO_QDSS_ETR_DBALO_ADDR, HWIO_QDSS_ETR_DBALO_RMSK)
#define HWIO_QDSS_ETR_DBALO_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_DBALO_ADDR, m)
#define HWIO_QDSS_ETR_DBALO_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_DBALO_ADDR,v)
#define HWIO_QDSS_ETR_DBALO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_DBALO_ADDR,m,v,HWIO_QDSS_ETR_DBALO_IN)
#define HWIO_QDSS_ETR_DBALO_BUFADDRLO_BMSK                          0xffffffff
#define HWIO_QDSS_ETR_DBALO_BUFADDRLO_SHFT                                 0x0

#define HWIO_QDSS_ETR_DBAHI_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x0000011c)
#define HWIO_QDSS_ETR_DBAHI_RMSK                                          0xff
#define HWIO_QDSS_ETR_DBAHI_IN          \
        in_dword_masked(HWIO_QDSS_ETR_DBAHI_ADDR, HWIO_QDSS_ETR_DBAHI_RMSK)
#define HWIO_QDSS_ETR_DBAHI_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_DBAHI_ADDR, m)
#define HWIO_QDSS_ETR_DBAHI_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_DBAHI_ADDR,v)
#define HWIO_QDSS_ETR_DBAHI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_DBAHI_ADDR,m,v,HWIO_QDSS_ETR_DBAHI_IN)
#define HWIO_QDSS_ETR_DBAHI_BUFADDRHI_BMSK                                0xff
#define HWIO_QDSS_ETR_DBAHI_BUFADDRHI_SHFT                                 0x0

#define HWIO_QDSS_ETR_FFSR_ADDR                                     (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000300)
#define HWIO_QDSS_ETR_FFSR_RMSK                                            0x3
#define HWIO_QDSS_ETR_FFSR_IN          \
        in_dword_masked(HWIO_QDSS_ETR_FFSR_ADDR, HWIO_QDSS_ETR_FFSR_RMSK)
#define HWIO_QDSS_ETR_FFSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_FFSR_ADDR, m)
#define HWIO_QDSS_ETR_FFSR_FTSTOPPED_BMSK                                  0x2
#define HWIO_QDSS_ETR_FFSR_FTSTOPPED_SHFT                                  0x1
#define HWIO_QDSS_ETR_FFSR_FLINPROG_BMSK                                   0x1
#define HWIO_QDSS_ETR_FFSR_FLINPROG_SHFT                                   0x0

#define HWIO_QDSS_ETR_FFCR_ADDR                                     (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000304)
#define HWIO_QDSS_ETR_FFCR_RMSK                                         0x3773
#define HWIO_QDSS_ETR_FFCR_IN          \
        in_dword_masked(HWIO_QDSS_ETR_FFCR_ADDR, HWIO_QDSS_ETR_FFCR_RMSK)
#define HWIO_QDSS_ETR_FFCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_FFCR_ADDR, m)
#define HWIO_QDSS_ETR_FFCR_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_FFCR_ADDR,v)
#define HWIO_QDSS_ETR_FFCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_FFCR_ADDR,m,v,HWIO_QDSS_ETR_FFCR_IN)
#define HWIO_QDSS_ETR_FFCR_STOPONTRIGEVT_BMSK                           0x2000
#define HWIO_QDSS_ETR_FFCR_STOPONTRIGEVT_SHFT                              0xd
#define HWIO_QDSS_ETR_FFCR_STOPONFL_BMSK                                0x1000
#define HWIO_QDSS_ETR_FFCR_STOPONFL_SHFT                                   0xc
#define HWIO_QDSS_ETR_FFCR_TRIGONFL_BMSK                                 0x400
#define HWIO_QDSS_ETR_FFCR_TRIGONFL_SHFT                                   0xa
#define HWIO_QDSS_ETR_FFCR_TRIGONTRIGEVT_BMSK                            0x200
#define HWIO_QDSS_ETR_FFCR_TRIGONTRIGEVT_SHFT                              0x9
#define HWIO_QDSS_ETR_FFCR_TRIGONTRIGIN_BMSK                             0x100
#define HWIO_QDSS_ETR_FFCR_TRIGONTRIGIN_SHFT                               0x8
#define HWIO_QDSS_ETR_FFCR_FLUSHMAN_R_BMSK                                0x40
#define HWIO_QDSS_ETR_FFCR_FLUSHMAN_R_SHFT                                 0x6
#define HWIO_QDSS_ETR_FFCR_FLUSHMAN_W_BMSK                                0x40
#define HWIO_QDSS_ETR_FFCR_FLUSHMAN_W_SHFT                                 0x6
#define HWIO_QDSS_ETR_FFCR_FONTRIGEVT_BMSK                                0x20
#define HWIO_QDSS_ETR_FFCR_FONTRIGEVT_SHFT                                 0x5
#define HWIO_QDSS_ETR_FFCR_FONFLIN_BMSK                                   0x10
#define HWIO_QDSS_ETR_FFCR_FONFLIN_SHFT                                    0x4
#define HWIO_QDSS_ETR_FFCR_ENTI_BMSK                                       0x2
#define HWIO_QDSS_ETR_FFCR_ENTI_SHFT                                       0x1
#define HWIO_QDSS_ETR_FFCR_ENFT_BMSK                                       0x1
#define HWIO_QDSS_ETR_FFCR_ENFT_SHFT                                       0x0

#define HWIO_QDSS_ETR_PSCR_ADDR                                     (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000308)
#define HWIO_QDSS_ETR_PSCR_RMSK                                           0x1f
#define HWIO_QDSS_ETR_PSCR_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PSCR_ADDR, HWIO_QDSS_ETR_PSCR_RMSK)
#define HWIO_QDSS_ETR_PSCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PSCR_ADDR, m)
#define HWIO_QDSS_ETR_PSCR_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_PSCR_ADDR,v)
#define HWIO_QDSS_ETR_PSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_PSCR_ADDR,m,v,HWIO_QDSS_ETR_PSCR_IN)
#define HWIO_QDSS_ETR_PSCR_PSCOUNT_BMSK                                   0x1f
#define HWIO_QDSS_ETR_PSCR_PSCOUNT_SHFT                                    0x0

#define HWIO_QDSS_ETR_ITMISCOP0_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ee0)
#define HWIO_QDSS_ETR_ITMISCOP0_RMSK                                       0x3
#define HWIO_QDSS_ETR_ITMISCOP0_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_ITMISCOP0_ADDR,v)
#define HWIO_QDSS_ETR_ITMISCOP0_FULL_BMSK                                  0x2
#define HWIO_QDSS_ETR_ITMISCOP0_FULL_SHFT                                  0x1
#define HWIO_QDSS_ETR_ITMISCOP0_ACQCOMP_BMSK                               0x1
#define HWIO_QDSS_ETR_ITMISCOP0_ACQCOMP_SHFT                               0x0

#define HWIO_QDSS_ETR_ITTRFLIN_ADDR                                 (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ee8)
#define HWIO_QDSS_ETR_ITTRFLIN_RMSK                                        0x3
#define HWIO_QDSS_ETR_ITTRFLIN_IN          \
        in_dword_masked(HWIO_QDSS_ETR_ITTRFLIN_ADDR, HWIO_QDSS_ETR_ITTRFLIN_RMSK)
#define HWIO_QDSS_ETR_ITTRFLIN_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_ITTRFLIN_ADDR, m)
#define HWIO_QDSS_ETR_ITTRFLIN_FLUSHIN_BMSK                                0x2
#define HWIO_QDSS_ETR_ITTRFLIN_FLUSHIN_SHFT                                0x1
#define HWIO_QDSS_ETR_ITTRFLIN_TRIGIN_BMSK                                 0x1
#define HWIO_QDSS_ETR_ITTRFLIN_TRIGIN_SHFT                                 0x0

#define HWIO_QDSS_ETR_ITATBDATA0_ADDR                               (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000eec)
#define HWIO_QDSS_ETR_ITATBDATA0_RMSK                                    0x1ff
#define HWIO_QDSS_ETR_ITATBDATA0_IN          \
        in_dword_masked(HWIO_QDSS_ETR_ITATBDATA0_ADDR, HWIO_QDSS_ETR_ITATBDATA0_RMSK)
#define HWIO_QDSS_ETR_ITATBDATA0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_ITATBDATA0_ADDR, m)
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT63_BMSK                       0x100
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT63_SHFT                         0x8
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT55_BMSK                        0x80
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT55_SHFT                         0x7
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT47_BMSK                        0x40
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT47_SHFT                         0x6
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT39_BMSK                        0x20
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT39_SHFT                         0x5
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT31_BMSK                        0x10
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT31_SHFT                         0x4
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT23_BMSK                         0x8
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT23_SHFT                         0x3
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT15_BMSK                         0x4
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT15_SHFT                         0x2
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT7_BMSK                          0x2
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT7_SHFT                          0x1
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT0_BMSK                          0x1
#define HWIO_QDSS_ETR_ITATBDATA0_ATDATASBIT0_SHFT                          0x0

#define HWIO_QDSS_ETR_ITATBCTR2_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ef0)
#define HWIO_QDSS_ETR_ITATBCTR2_RMSK                                       0x7
#define HWIO_QDSS_ETR_ITATBCTR2_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_ITATBCTR2_ADDR,v)
#define HWIO_QDSS_ETR_ITATBCTR2_SYNCREQS_BMSK                              0x4
#define HWIO_QDSS_ETR_ITATBCTR2_SYNCREQS_SHFT                              0x2
#define HWIO_QDSS_ETR_ITATBCTR2_AFVALIDS_BMSK                              0x2
#define HWIO_QDSS_ETR_ITATBCTR2_AFVALIDS_SHFT                              0x1
#define HWIO_QDSS_ETR_ITATBCTR2_ATREADYS_BMSK                              0x1
#define HWIO_QDSS_ETR_ITATBCTR2_ATREADYS_SHFT                              0x0

#define HWIO_QDSS_ETR_ITATBCTR1_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ef4)
#define HWIO_QDSS_ETR_ITATBCTR1_RMSK                                      0x7f
#define HWIO_QDSS_ETR_ITATBCTR1_IN          \
        in_dword_masked(HWIO_QDSS_ETR_ITATBCTR1_ADDR, HWIO_QDSS_ETR_ITATBCTR1_RMSK)
#define HWIO_QDSS_ETR_ITATBCTR1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_ITATBCTR1_ADDR, m)
#define HWIO_QDSS_ETR_ITATBCTR1_ATIDS_BMSK                                0x7f
#define HWIO_QDSS_ETR_ITATBCTR1_ATIDS_SHFT                                 0x0

#define HWIO_QDSS_ETR_ITATBCTR0_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_ETR_ITATBCTR0_RMSK                                     0x703
#define HWIO_QDSS_ETR_ITATBCTR0_IN          \
        in_dword_masked(HWIO_QDSS_ETR_ITATBCTR0_ADDR, HWIO_QDSS_ETR_ITATBCTR0_RMSK)
#define HWIO_QDSS_ETR_ITATBCTR0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_ITATBCTR0_ADDR, m)
#define HWIO_QDSS_ETR_ITATBCTR0_ATBYTESS_BMSK                            0x700
#define HWIO_QDSS_ETR_ITATBCTR0_ATBYTESS_SHFT                              0x8
#define HWIO_QDSS_ETR_ITATBCTR0_AFREADYS_BMSK                              0x2
#define HWIO_QDSS_ETR_ITATBCTR0_AFREADYS_SHFT                              0x1
#define HWIO_QDSS_ETR_ITATBCTR0_ATVALIDS_BMSK                              0x1
#define HWIO_QDSS_ETR_ITATBCTR0_ATVALIDS_SHFT                              0x0

#define HWIO_QDSS_ETR_ITCTRL_ADDR                                   (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_ETR_ITCTRL_RMSK                                          0x1
#define HWIO_QDSS_ETR_ITCTRL_IN          \
        in_dword_masked(HWIO_QDSS_ETR_ITCTRL_ADDR, HWIO_QDSS_ETR_ITCTRL_RMSK)
#define HWIO_QDSS_ETR_ITCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_ITCTRL_ADDR, m)
#define HWIO_QDSS_ETR_ITCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_ITCTRL_ADDR,v)
#define HWIO_QDSS_ETR_ITCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_ITCTRL_ADDR,m,v,HWIO_QDSS_ETR_ITCTRL_IN)
#define HWIO_QDSS_ETR_ITCTRL_INTEGRATION_MODE_BMSK                         0x1
#define HWIO_QDSS_ETR_ITCTRL_INTEGRATION_MODE_SHFT                         0x0

#define HWIO_QDSS_ETR_CLAIMSET_ADDR                                 (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_ETR_CLAIMSET_RMSK                                        0xf
#define HWIO_QDSS_ETR_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_ETR_CLAIMSET_ADDR, HWIO_QDSS_ETR_CLAIMSET_RMSK)
#define HWIO_QDSS_ETR_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_CLAIMSET_ADDR, m)
#define HWIO_QDSS_ETR_CLAIMSET_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_CLAIMSET_ADDR,v)
#define HWIO_QDSS_ETR_CLAIMSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_CLAIMSET_ADDR,m,v,HWIO_QDSS_ETR_CLAIMSET_IN)
#define HWIO_QDSS_ETR_CLAIMSET_CLAIMSET_W_BMSK                             0xf
#define HWIO_QDSS_ETR_CLAIMSET_CLAIMSET_W_SHFT                             0x0
#define HWIO_QDSS_ETR_CLAIMSET_CLAIMSET_R_BMSK                             0xf
#define HWIO_QDSS_ETR_CLAIMSET_CLAIMSET_R_SHFT                             0x0

#define HWIO_QDSS_ETR_CLAIMCLR_ADDR                                 (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_ETR_CLAIMCLR_RMSK                                        0xf
#define HWIO_QDSS_ETR_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_ETR_CLAIMCLR_ADDR, HWIO_QDSS_ETR_CLAIMCLR_RMSK)
#define HWIO_QDSS_ETR_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_ETR_CLAIMCLR_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_CLAIMCLR_ADDR,v)
#define HWIO_QDSS_ETR_CLAIMCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_CLAIMCLR_ADDR,m,v,HWIO_QDSS_ETR_CLAIMCLR_IN)
#define HWIO_QDSS_ETR_CLAIMCLR_CLAIMCLR_W_BMSK                             0xf
#define HWIO_QDSS_ETR_CLAIMCLR_CLAIMCLR_W_SHFT                             0x0
#define HWIO_QDSS_ETR_CLAIMCLR_CLAIMCLR_R_BMSK                             0xf
#define HWIO_QDSS_ETR_CLAIMCLR_CLAIMCLR_R_SHFT                             0x0

#define HWIO_QDSS_ETR_LAR_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_ETR_LAR_RMSK                                      0xffffffff
#define HWIO_QDSS_ETR_LAR_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_LAR_ADDR,v)
#define HWIO_QDSS_ETR_LAR_ACCESS_W_BMSK                             0xffffffff
#define HWIO_QDSS_ETR_LAR_ACCESS_W_SHFT                                    0x0

#define HWIO_QDSS_ETR_LSR_ADDR                                      (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_ETR_LSR_RMSK                                             0x7
#define HWIO_QDSS_ETR_LSR_IN          \
        in_dword_masked(HWIO_QDSS_ETR_LSR_ADDR, HWIO_QDSS_ETR_LSR_RMSK)
#define HWIO_QDSS_ETR_LSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_LSR_ADDR, m)
#define HWIO_QDSS_ETR_LSR_LOCKTYPE_BMSK                                    0x4
#define HWIO_QDSS_ETR_LSR_LOCKTYPE_SHFT                                    0x2
#define HWIO_QDSS_ETR_LSR_LOCKGRANT_BMSK                                   0x2
#define HWIO_QDSS_ETR_LSR_LOCKGRANT_SHFT                                   0x1
#define HWIO_QDSS_ETR_LSR_LOCKEXIST_BMSK                                   0x1
#define HWIO_QDSS_ETR_LSR_LOCKEXIST_SHFT                                   0x0

#define HWIO_QDSS_ETR_AUTHSTATUS_ADDR                               (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_ETR_AUTHSTATUS_RMSK                                     0xff
#define HWIO_QDSS_ETR_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_ETR_AUTHSTATUS_ADDR, HWIO_QDSS_ETR_AUTHSTATUS_RMSK)
#define HWIO_QDSS_ETR_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_ETR_AUTHSTATUS_SNID_BMSK                                0xc0
#define HWIO_QDSS_ETR_AUTHSTATUS_SNID_SHFT                                 0x6
#define HWIO_QDSS_ETR_AUTHSTATUS_SID_BMSK                                 0x30
#define HWIO_QDSS_ETR_AUTHSTATUS_SID_SHFT                                  0x4
#define HWIO_QDSS_ETR_AUTHSTATUS_NSNID_BMSK                                0xc
#define HWIO_QDSS_ETR_AUTHSTATUS_NSNID_SHFT                                0x2
#define HWIO_QDSS_ETR_AUTHSTATUS_NSID_BMSK                                 0x3
#define HWIO_QDSS_ETR_AUTHSTATUS_NSID_SHFT                                 0x0

#define HWIO_QDSS_ETR_DEVID_ADDR                                    (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_ETR_DEVID_RMSK                                        0x3fff
#define HWIO_QDSS_ETR_DEVID_IN          \
        in_dword_masked(HWIO_QDSS_ETR_DEVID_ADDR, HWIO_QDSS_ETR_DEVID_RMSK)
#define HWIO_QDSS_ETR_DEVID_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_DEVID_ADDR, m)
#define HWIO_QDSS_ETR_DEVID_WBUF_DEPTH_BMSK                             0x3800
#define HWIO_QDSS_ETR_DEVID_WBUF_DEPTH_SHFT                                0xb
#define HWIO_QDSS_ETR_DEVID_MEMWIDTH_BMSK                                0x700
#define HWIO_QDSS_ETR_DEVID_MEMWIDTH_SHFT                                  0x8
#define HWIO_QDSS_ETR_DEVID_CONFIGTYPE_BMSK                               0xc0
#define HWIO_QDSS_ETR_DEVID_CONFIGTYPE_SHFT                                0x6
#define HWIO_QDSS_ETR_DEVID_CLKSCHEME_BMSK                                0x20
#define HWIO_QDSS_ETR_DEVID_CLKSCHEME_SHFT                                 0x5
#define HWIO_QDSS_ETR_DEVID_ATBINPORTCOUNT_BMSK                           0x1f
#define HWIO_QDSS_ETR_DEVID_ATBINPORTCOUNT_SHFT                            0x0

#define HWIO_QDSS_ETR_DEVTYPE_ADDR                                  (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_ETR_DEVTYPE_RMSK                                        0xff
#define HWIO_QDSS_ETR_DEVTYPE_IN          \
        in_dword_masked(HWIO_QDSS_ETR_DEVTYPE_ADDR, HWIO_QDSS_ETR_DEVTYPE_RMSK)
#define HWIO_QDSS_ETR_DEVTYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_DEVTYPE_ADDR, m)
#define HWIO_QDSS_ETR_DEVTYPE_SUB_TYPE_BMSK                               0xf0
#define HWIO_QDSS_ETR_DEVTYPE_SUB_TYPE_SHFT                                0x4
#define HWIO_QDSS_ETR_DEVTYPE_MAJOR_TYPE_BMSK                              0xf
#define HWIO_QDSS_ETR_DEVTYPE_MAJOR_TYPE_SHFT                              0x0

#define HWIO_QDSS_ETR_PERIPHID0_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_ETR_PERIPHID0_RMSK                                      0xff
#define HWIO_QDSS_ETR_PERIPHID0_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID0_ADDR, HWIO_QDSS_ETR_PERIPHID0_RMSK)
#define HWIO_QDSS_ETR_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID0_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID0_PART_NUMBER_BITS7TO0_BMSK                 0xff
#define HWIO_QDSS_ETR_PERIPHID0_PART_NUMBER_BITS7TO0_SHFT                  0x0

#define HWIO_QDSS_ETR_PERIPHID1_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_ETR_PERIPHID1_RMSK                                      0xff
#define HWIO_QDSS_ETR_PERIPHID1_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID1_ADDR, HWIO_QDSS_ETR_PERIPHID1_RMSK)
#define HWIO_QDSS_ETR_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID1_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID1_JEP106_BITS3TO0_BMSK                      0xf0
#define HWIO_QDSS_ETR_PERIPHID1_JEP106_BITS3TO0_SHFT                       0x4
#define HWIO_QDSS_ETR_PERIPHID1_PART_NUMBER_BITS11TO8_BMSK                 0xf
#define HWIO_QDSS_ETR_PERIPHID1_PART_NUMBER_BITS11TO8_SHFT                 0x0

#define HWIO_QDSS_ETR_PERIPHID2_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_ETR_PERIPHID2_RMSK                                      0xff
#define HWIO_QDSS_ETR_PERIPHID2_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID2_ADDR, HWIO_QDSS_ETR_PERIPHID2_RMSK)
#define HWIO_QDSS_ETR_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID2_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID2_REVISION_BMSK                             0xf0
#define HWIO_QDSS_ETR_PERIPHID2_REVISION_SHFT                              0x4
#define HWIO_QDSS_ETR_PERIPHID2_JEDEC_BMSK                                 0x8
#define HWIO_QDSS_ETR_PERIPHID2_JEDEC_SHFT                                 0x3
#define HWIO_QDSS_ETR_PERIPHID2_JEP106_BITS6TO4_BMSK                       0x7
#define HWIO_QDSS_ETR_PERIPHID2_JEP106_BITS6TO4_SHFT                       0x0

#define HWIO_QDSS_ETR_PERIPHID3_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_ETR_PERIPHID3_RMSK                                      0xff
#define HWIO_QDSS_ETR_PERIPHID3_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID3_ADDR, HWIO_QDSS_ETR_PERIPHID3_RMSK)
#define HWIO_QDSS_ETR_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID3_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID3_REVAND_BMSK                               0xf0
#define HWIO_QDSS_ETR_PERIPHID3_REVAND_SHFT                                0x4
#define HWIO_QDSS_ETR_PERIPHID3_CUSTOMER_MODIFIED_BMSK                     0xf
#define HWIO_QDSS_ETR_PERIPHID3_CUSTOMER_MODIFIED_SHFT                     0x0

#define HWIO_QDSS_ETR_PERIPHID4_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_ETR_PERIPHID4_RMSK                                      0xff
#define HWIO_QDSS_ETR_PERIPHID4_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID4_ADDR, HWIO_QDSS_ETR_PERIPHID4_RMSK)
#define HWIO_QDSS_ETR_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID4_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID4_FOURKB_COUNT_BMSK                         0xf0
#define HWIO_QDSS_ETR_PERIPHID4_FOURKB_COUNT_SHFT                          0x4
#define HWIO_QDSS_ETR_PERIPHID4_JEP106_CONT_BMSK                           0xf
#define HWIO_QDSS_ETR_PERIPHID4_JEP106_CONT_SHFT                           0x0

#define HWIO_QDSS_ETR_PERIPHID5_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_ETR_PERIPHID5_RMSK                                0xffffffff
#define HWIO_QDSS_ETR_PERIPHID5_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID5_ADDR, HWIO_QDSS_ETR_PERIPHID5_RMSK)
#define HWIO_QDSS_ETR_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID5_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID5_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_PERIPHID5_ADDR,v)
#define HWIO_QDSS_ETR_PERIPHID5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_PERIPHID5_ADDR,m,v,HWIO_QDSS_ETR_PERIPHID5_IN)
#define HWIO_QDSS_ETR_PERIPHID5_PERIPHID5_BMSK                      0xffffffff
#define HWIO_QDSS_ETR_PERIPHID5_PERIPHID5_SHFT                             0x0

#define HWIO_QDSS_ETR_PERIPHID6_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_ETR_PERIPHID6_RMSK                                0xffffffff
#define HWIO_QDSS_ETR_PERIPHID6_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID6_ADDR, HWIO_QDSS_ETR_PERIPHID6_RMSK)
#define HWIO_QDSS_ETR_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID6_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID6_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_PERIPHID6_ADDR,v)
#define HWIO_QDSS_ETR_PERIPHID6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_PERIPHID6_ADDR,m,v,HWIO_QDSS_ETR_PERIPHID6_IN)
#define HWIO_QDSS_ETR_PERIPHID6_PERIPHID6_BMSK                      0xffffffff
#define HWIO_QDSS_ETR_PERIPHID6_PERIPHID6_SHFT                             0x0

#define HWIO_QDSS_ETR_PERIPHID7_ADDR                                (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_ETR_PERIPHID7_RMSK                                0xffffffff
#define HWIO_QDSS_ETR_PERIPHID7_IN          \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID7_ADDR, HWIO_QDSS_ETR_PERIPHID7_RMSK)
#define HWIO_QDSS_ETR_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_PERIPHID7_ADDR, m)
#define HWIO_QDSS_ETR_PERIPHID7_OUT(v)      \
        out_dword(HWIO_QDSS_ETR_PERIPHID7_ADDR,v)
#define HWIO_QDSS_ETR_PERIPHID7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_ETR_PERIPHID7_ADDR,m,v,HWIO_QDSS_ETR_PERIPHID7_IN)
#define HWIO_QDSS_ETR_PERIPHID7_PERIPHID7_BMSK                      0xffffffff
#define HWIO_QDSS_ETR_PERIPHID7_PERIPHID7_SHFT                             0x0

#define HWIO_QDSS_ETR_COMPID0_ADDR                                  (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_ETR_COMPID0_RMSK                                        0xff
#define HWIO_QDSS_ETR_COMPID0_IN          \
        in_dword_masked(HWIO_QDSS_ETR_COMPID0_ADDR, HWIO_QDSS_ETR_COMPID0_RMSK)
#define HWIO_QDSS_ETR_COMPID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_COMPID0_ADDR, m)
#define HWIO_QDSS_ETR_COMPID0_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETR_COMPID0_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETR_COMPID1_ADDR                                  (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_ETR_COMPID1_RMSK                                        0xff
#define HWIO_QDSS_ETR_COMPID1_IN          \
        in_dword_masked(HWIO_QDSS_ETR_COMPID1_ADDR, HWIO_QDSS_ETR_COMPID1_RMSK)
#define HWIO_QDSS_ETR_COMPID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_COMPID1_ADDR, m)
#define HWIO_QDSS_ETR_COMPID1_CLASS_BMSK                                  0xf0
#define HWIO_QDSS_ETR_COMPID1_CLASS_SHFT                                   0x4
#define HWIO_QDSS_ETR_COMPID1_PREAMBLE_BMSK                                0xf
#define HWIO_QDSS_ETR_COMPID1_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETR_COMPID2_ADDR                                  (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_ETR_COMPID2_RMSK                                        0xff
#define HWIO_QDSS_ETR_COMPID2_IN          \
        in_dword_masked(HWIO_QDSS_ETR_COMPID2_ADDR, HWIO_QDSS_ETR_COMPID2_RMSK)
#define HWIO_QDSS_ETR_COMPID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_COMPID2_ADDR, m)
#define HWIO_QDSS_ETR_COMPID2_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETR_COMPID2_PREAMBLE_SHFT                                0x0

#define HWIO_QDSS_ETR_COMPID3_ADDR                                  (QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_ETR_COMPID3_RMSK                                        0xff
#define HWIO_QDSS_ETR_COMPID3_IN          \
        in_dword_masked(HWIO_QDSS_ETR_COMPID3_ADDR, HWIO_QDSS_ETR_COMPID3_RMSK)
#define HWIO_QDSS_ETR_COMPID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_ETR_COMPID3_ADDR, m)
#define HWIO_QDSS_ETR_COMPID3_PREAMBLE_BMSK                               0xff
#define HWIO_QDSS_ETR_COMPID3_PREAMBLE_SHFT                                0x0


#endif /* __QDSS_ETR_REGS_H__ */
