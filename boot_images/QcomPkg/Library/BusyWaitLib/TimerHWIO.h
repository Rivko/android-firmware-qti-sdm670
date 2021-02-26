#ifndef __TIMERHWIO_H__
#define __TIMERHWIO_H__
/*
===========================================================================
*/
/**
  @file TimerHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QTMR_V1

  'Include' filters applied: QTMR[QTMR_V1] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 12 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================


  ===========================================================================
*/



/*----------------------------------------------------------------------------
 * MODULE: QTIMER_AC1
 *--------------------------------------------------------------------------*/

#define HWIO_QTMR_AC_CNTFRQ_ADDR(BASE)                           (BASE      + 0x00000000)
#define HWIO_QTMR_AC_CNTFRQ_RMSK                           0xffffffff
#define HWIO_QTMR_AC_CNTFRQ_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_AC_CNTFRQ_ADDR(BASE), HWIO_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_QTMR_AC_CNTFRQ_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_AC_CNTFRQ_ADDR(BASE), m)
#define HWIO_QTMR_AC_CNTFRQ_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_AC_CNTFRQ_ADDR(BASE),v)
#define HWIO_QTMR_AC_CNTFRQ_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_AC_CNTFRQ_ADDR(BASE),m,v,HWIO_QTMR_AC_CNTFRQ_IN(BASE))
#define HWIO_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                    0xffffffff
#define HWIO_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                           0x0

#define HWIO_QTMR_AC_CNTSR_ADDR(BASE)                            (BASE      + 0x00000004)
#define HWIO_QTMR_AC_CNTSR_RMSK                                  0x7f
#define HWIO_QTMR_AC_CNTSR_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_AC_CNTSR_ADDR(BASE), HWIO_QTMR_AC_CNTSR_RMSK)
#define HWIO_QTMR_AC_CNTSR_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_AC_CNTSR_ADDR(BASE), m)
#define HWIO_QTMR_AC_CNTSR_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_AC_CNTSR_ADDR(BASE),v)
#define HWIO_QTMR_AC_CNTSR_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_AC_CNTSR_ADDR(BASE),m,v,HWIO_QTMR_AC_CNTSR_IN(BASE))
#define HWIO_QTMR_AC_CNTSR_NSN_BMSK                              0x7f
#define HWIO_QTMR_AC_CNTSR_NSN_SHFT                               0x0

#define HWIO_QTMR_AC_CNTTID_ADDR(BASE)                           (BASE      + 0x00000008)
#define HWIO_QTMR_AC_CNTTID_RMSK                           0xffffffff
#define HWIO_QTMR_AC_CNTTID_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_AC_CNTTID_ADDR(BASE), HWIO_QTMR_AC_CNTTID_RMSK)
#define HWIO_QTMR_AC_CNTTID_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_AC_CNTTID_ADDR(BASE), m)
#define HWIO_QTMR_AC_CNTTID_F7_CFG_BMSK                    0xf0000000
#define HWIO_QTMR_AC_CNTTID_F7_CFG_SHFT                          0x1c
#define HWIO_QTMR_AC_CNTTID_F6_CFG_BMSK                     0xf000000
#define HWIO_QTMR_AC_CNTTID_F6_CFG_SHFT                          0x18
#define HWIO_QTMR_AC_CNTTID_F5_CFG_BMSK                      0xf00000
#define HWIO_QTMR_AC_CNTTID_F5_CFG_SHFT                          0x14
#define HWIO_QTMR_AC_CNTTID_F4_CFG_BMSK                       0xf0000
#define HWIO_QTMR_AC_CNTTID_F4_CFG_SHFT                          0x10
#define HWIO_QTMR_AC_CNTTID_F3_CFG_BMSK                        0xf000
#define HWIO_QTMR_AC_CNTTID_F3_CFG_SHFT                           0xc
#define HWIO_QTMR_AC_CNTTID_F2_CFG_BMSK                         0xf00
#define HWIO_QTMR_AC_CNTTID_F2_CFG_SHFT                           0x8
#define HWIO_QTMR_AC_CNTTID_F1_CFG_BMSK                          0xf0
#define HWIO_QTMR_AC_CNTTID_F1_CFG_SHFT                           0x4
#define HWIO_QTMR_AC_CNTTID_F0_CFG_BMSK                           0xf
#define HWIO_QTMR_AC_CNTTID_F0_CFG_SHFT                           0x0

#define HWIO_QTMR_AC_CNTACR_n_ADDR(BASE,n)                      (BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_QTMR_AC_CNTACR_n_MAXn                                  6
#define HWIO_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_QTMR_AC_CNTACR_n_ADDR(BASE,n), HWIO_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_QTMR_AC_CNTACR_n_INMI(BASE,n,mask)    \
        in_dword_masked(HWIO_QTMR_AC_CNTACR_n_ADDR(BASE,n), (mask))
#define HWIO_QTMR_AC_CNTACR_n_OUTI(BASE,n,val)    \
        out_dword(HWIO_QTMR_AC_CNTACR_n_ADDR(BASE,n),val)
#define HWIO_QTMR_AC_CNTACR_n_OUTMI(BASE,n,mask,val) \
        out_dword_masked_ns(HWIO_QTMR_AC_CNTACR_n_ADDR(BASE,n),mask,val,HWIO_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0

#define HWIO_QTMR_AC_CNTVOFF_LO_n_ADDR(BASE,n)                  (BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_QTMR_AC_CNTVOFF_LO_n_RMSK                     0xffffffff
#define HWIO_QTMR_AC_CNTVOFF_LO_n_MAXn                              6
#define HWIO_QTMR_AC_CNTVOFF_LO_n_INI(n)        \
        in_dword_masked(HWIO_QTMR_AC_CNTVOFF_LO_n_ADDR(BASE,n), HWIO_QTMR_AC_CNTVOFF_LO_n_RMSK)
#define HWIO_QTMR_AC_CNTVOFF_LO_n_INMI(BASE,n,mask)    \
        in_dword_masked(HWIO_QTMR_AC_CNTVOFF_LO_n_ADDR(BASE,n), (mask))
#define HWIO_QTMR_AC_CNTVOFF_LO_n_OUTI(BASE,n,val)    \
        out_dword(HWIO_QTMR_AC_CNTVOFF_LO_n_ADDR(n),val)
#define HWIO_QTMR_AC_CNTVOFF_LO_n_OUTMI(BASE,n,mask,val) \
        out_dword_masked_ns(HWIO_QTMR_AC_CNTVOFF_LO_n_ADDR(BASE,n),mask,val,HWIO_QTMR_AC_CNTVOFF_LO_n_INI(BASE,n))
#define HWIO_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_BMSK          0xffffffff
#define HWIO_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_SHFT                 0x0

#define HWIO_QTMR_AC_CNTVOFF_HI_n_ADDR(BASE,n)                  (BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_QTMR_AC_CNTVOFF_HI_n_RMSK                     0xffffffff
#define HWIO_QTMR_AC_CNTVOFF_HI_n_MAXn                              6
#define HWIO_QTMR_AC_CNTVOFF_HI_n_INI(n)        \
        in_dword_masked(HWIO_QTMR_AC_CNTVOFF_HI_n_ADDR(BASE,n), HWIO_QTMR_AC_CNTVOFF_HI_n_RMSK)
#define HWIO_QTMR_AC_CNTVOFF_HI_n_INMI(BASE,n,mask)    \
        in_dword_masked(HWIO_QTMR_AC_CNTVOFF_HI_n_ADDR(BASE,n), (mask))
#define HWIO_QTMR_AC_CNTVOFF_HI_n_OUTI(BASE,n,val)    \
        out_dword(HWIO_QTMR_AC_CNTVOFF_HI_n_ADDR(BASE,n),val)
#define HWIO_QTMR_AC_CNTVOFF_HI_n_OUTMI(BASE,n,mask,val) \
        out_dword_masked_ns(HWIO_QTMR_AC_CNTVOFF_HI_n_ADDR(BASE,n),mask,val,HWIO_QTMR_AC_CNTVOFF_HI_n_INI(BASE,n))
#define HWIO_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_BMSK          0xffffffff
#define HWIO_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_SHFT                 0x0

#define HWIO_QTMR_VERSION_ADDR(BASE)                             (BASE      + 0x00000fd0)
#define HWIO_QTMR_VERSION_RMSK                             0xffffffff
#define HWIO_QTMR_VERSION_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_VERSION_ADDR(BASE), HWIO_QTMR_VERSION_RMSK)
#define HWIO_QTMR_VERSION_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_VERSION_ADDR(BASE), m)
#define HWIO_QTMR_VERSION_MAJOR_BMSK                       0xf0000000
#define HWIO_QTMR_VERSION_MAJOR_SHFT                             0x1c
#define HWIO_QTMR_VERSION_MINOR_BMSK                        0xfff0000
#define HWIO_QTMR_VERSION_MINOR_SHFT                             0x10
#define HWIO_QTMR_VERSION_STEP_BMSK                            0xffff
#define HWIO_QTMR_VERSION_STEP_SHFT                               0x0


/*----------------------------------------------------------------------------
 * MODULE: QTMR_V1
 *--------------------------------------------------------------------------*/

#define HWIO_QTMR_CNTPCT_LO_ADDR(BASE)                          (BASE      + 0x00000000)
#define HWIO_QTMR_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_QTMR_CNTPCT_LO_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTPCT_LO_ADDR(BASE), HWIO_QTMR_CNTPCT_LO_RMSK)
#define HWIO_QTMR_CNTPCT_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTPCT_LO_ADDR(BASE), m)
#define HWIO_QTMR_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_QTMR_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_QTMR_CNTPCT_HI_ADDR(BASE)                          (BASE      + 0x00000004)
#define HWIO_QTMR_CNTPCT_HI_RMSK                          0xffffffff
#define HWIO_QTMR_CNTPCT_HI_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTPCT_HI_ADDR(BASE), HWIO_QTMR_CNTPCT_HI_RMSK)
#define HWIO_QTMR_CNTPCT_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTPCT_HI_ADDR(BASE), m)
#define HWIO_QTMR_CNTPCT_HI_CNTPCT_HI_BMSK                0xffffffff
#define HWIO_QTMR_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_QTMR_CNTVCT_LO_ADDR(BASE)                          (BASE      + 0x00000008)
#define HWIO_QTMR_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_QTMR_CNTVCT_LO_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTVCT_LO_ADDR(BASE), HWIO_QTMR_CNTVCT_LO_RMSK)
#define HWIO_QTMR_CNTVCT_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVCT_LO_ADDR(BASE), m)
#define HWIO_QTMR_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_QTMR_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_QTMR_CNTVCT_HI_ADDR(BASE)                          (BASE      + 0x0000000c)
#define HWIO_QTMR_CNTVCT_HI_RMSK                          0xffffffff
#define HWIO_QTMR_CNTVCT_HI_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTVCT_HI_ADDR(BASE), HWIO_QTMR_CNTVCT_HI_RMSK)
#define HWIO_QTMR_CNTVCT_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVCT_HI_ADDR(BASE), m)
#define HWIO_QTMR_CNTVCT_HI_CNTVCT_HI_BMSK                0xffffffff
#define HWIO_QTMR_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_QTMR_CNTFRQ_ADDR(BASE)                             (BASE      + 0x00000010)
#define HWIO_QTMR_CNTFRQ_RMSK                             0xffffffff
#define HWIO_QTMR_CNTFRQ_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTFRQ_ADDR(BASE), HWIO_QTMR_CNTFRQ_RMSK)
#define HWIO_QTMR_CNTFRQ_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTFRQ_ADDR(BASE), m)
#define HWIO_QTMR_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_QTMR_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_QTMR_CNTPL0ACR_ADDR(BASE)                               ((BASE) + 0x00000014)
#define HWIO_QTMR_CNTPL0ACR_RMSK                                       0x303
#define HWIO_QTMR_CNTPL0ACR_IN(BASE)      \
        in_dword_masked(HWIO_QTMR_CNTPL0ACR_ADDR(BASE), HWIO_QTMR_CNTPL0ACR_RMSK)
#define HWIO_QTMR_CNTPL0ACR_INM(BASE, m)      \
        in_dword_masked(HWIO_QTMR_CNTPL0ACR_ADDR(BASE), m)
#define HWIO_QTMR_CNTPL0ACR_OUT(BASE, v)      \
        out_dword(HWIO_QTMR_CNTPL0ACR_ADDR(BASE),v)
#define HWIO_QTMR_CNTPL0ACR_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTPL0ACR_ADDR(BASE),m,v,HWIO_QTMR_CNTPL0ACR_IN(BASE))
#define HWIO_QTMR_CNTPL0ACR_PL0CTEN_BMSK                               0x200
#define HWIO_QTMR_CNTPL0ACR_PL0CTEN_SHFT                                 0x9
#define HWIO_QTMR_CNTPL0ACR_PL0VTEN_BMSK                               0x100
#define HWIO_QTMR_CNTPL0ACR_PL0VTEN_SHFT                                 0x8
#define HWIO_QTMR_CNTPL0ACR_PL0VCTEN_BMSK                                0x2
#define HWIO_QTMR_CNTPL0ACR_PL0VCTEN_SHFT                                0x1
#define HWIO_QTMR_CNTPL0ACR_PL0PCTEN_BMSK                                0x1
#define HWIO_QTMR_CNTPL0ACR_PL0PCTEN_SHFT                                0x0

#define HWIO_QTMR_CNTVOFF_LO_ADDR(BASE)                         (BASE      + 0x00000018)
#define HWIO_QTMR_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_QTMR_CNTVOFF_LO_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTVOFF_LO_ADDR(BASE), HWIO_QTMR_CNTVOFF_LO_RMSK)
#define HWIO_QTMR_CNTVOFF_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVOFF_LO_ADDR(BASE), m)
#define HWIO_QTMR_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_QTMR_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_QTMR_CNTVOFF_HI_ADDR(BASE)                         (BASE      + 0x0000001c)
#define HWIO_QTMR_CNTVOFF_HI_RMSK                         0xffffffff
#define HWIO_QTMR_CNTVOFF_HI_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTVOFF_HI_ADDR(BASE), HWIO_QTMR_CNTVOFF_HI_RMSK)
#define HWIO_QTMR_CNTVOFF_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVOFF_HI_ADDR(BASE), m)
#define HWIO_QTMR_CNTVOFF_HI_CNTVOFF_HI_BMSK              0xffffffff
#define HWIO_QTMR_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_QTMR_CNTP_CVAL_LO_ADDR(BASE)                       (BASE      + 0x00000020)
#define HWIO_QTMR_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_QTMR_CNTP_CVAL_LO_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_LO_ADDR(BASE), HWIO_QTMR_CNTP_CVAL_LO_RMSK)
#define HWIO_QTMR_CNTP_CVAL_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_LO_ADDR(BASE), m)
#define HWIO_QTMR_CNTP_CVAL_LO_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CVAL_LO_ADDR(BASE),v)
#define HWIO_QTMR_CNTP_CVAL_LO_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CVAL_LO_ADDR(BASE),m,v,HWIO_QTMR_CNTP_CVAL_LO_IN(BASE))
#define HWIO_QTMR_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_QTMR_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_QTMR_CNTP_CVAL_HI_ADDR(BASE)                       (BASE      + 0x00000024)
#define HWIO_QTMR_CNTP_CVAL_HI_RMSK                       0xffffffff
#define HWIO_QTMR_CNTP_CVAL_HI_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_HI_ADDR(BASE), HWIO_QTMR_CNTP_CVAL_HI_RMSK)
#define HWIO_QTMR_CNTP_CVAL_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_HI_ADDR(BASE), m)
#define HWIO_QTMR_CNTP_CVAL_HI_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CVAL_HI_ADDR(BASE),v)
#define HWIO_QTMR_CNTP_CVAL_HI_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CVAL_HI_ADDR(BASE),m,v,HWIO_QTMR_CNTP_CVAL_HI_IN(BASE))
#define HWIO_QTMR_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK          0xffffffff
#define HWIO_QTMR_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_QTMR_CNTP_TVAL_ADDR(BASE)                          (BASE      + 0x00000028)
#define HWIO_QTMR_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_QTMR_CNTP_TVAL_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTP_TVAL_ADDR(BASE), HWIO_QTMR_CNTP_TVAL_RMSK)
#define HWIO_QTMR_CNTP_TVAL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_TVAL_ADDR(BASE), m)
#define HWIO_QTMR_CNTP_TVAL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_TVAL_ADDR(BASE),v)
#define HWIO_QTMR_CNTP_TVAL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_TVAL_ADDR(BASE),m,v,HWIO_QTMR_CNTP_TVAL_IN(BASE))
#define HWIO_QTMR_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_QTMR_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_QTMR_CNTP_CTL_ADDR(BASE)                           (BASE      + 0x0000002c)
#define HWIO_QTMR_CNTP_CTL_RMSK                                  0x7
#define HWIO_QTMR_CNTP_CTL_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTP_CTL_ADDR(BASE), HWIO_QTMR_CNTP_CTL_RMSK)
#define HWIO_QTMR_CNTP_CTL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CTL_ADDR(BASE), m)
#define HWIO_QTMR_CNTP_CTL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CTL_ADDR(BASE),v)
#define HWIO_QTMR_CNTP_CTL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CTL_ADDR(BASE),m,v,HWIO_QTMR_CNTP_CTL_IN(BASE))
#define HWIO_QTMR_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_QTMR_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_QTMR_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_QTMR_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_QTMR_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_QTMR_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_QTMR_CNTV_CVAL_LO_ADDR(BASE)                       (BASE      + 0x00000030)
#define HWIO_QTMR_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_QTMR_CNTV_CVAL_LO_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_LO_ADDR(BASE), HWIO_QTMR_CNTV_CVAL_LO_RMSK)
#define HWIO_QTMR_CNTV_CVAL_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_LO_ADDR(BASE), m)
#define HWIO_QTMR_CNTV_CVAL_LO_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CVAL_LO_ADDR(BASE),v)
#define HWIO_QTMR_CNTV_CVAL_LO_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CVAL_LO_ADDR(BASE),m,v,HWIO_QTMR_CNTV_CVAL_LO_IN(BASE))
#define HWIO_QTMR_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_QTMR_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_QTMR_CNTV_CVAL_HI_ADDR(BASE)                       (BASE      + 0x00000034)
#define HWIO_QTMR_CNTV_CVAL_HI_RMSK                       0xffffffff
#define HWIO_QTMR_CNTV_CVAL_HI_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_HI_ADDR(BASE), HWIO_QTMR_CNTV_CVAL_HI_RMSK)
#define HWIO_QTMR_CNTV_CVAL_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_HI_ADDR(BASE), m)
#define HWIO_QTMR_CNTV_CVAL_HI_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CVAL_HI_ADDR(BASE),v)
#define HWIO_QTMR_CNTV_CVAL_HI_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CVAL_HI_ADDR(BASE),m,v,HWIO_QTMR_CNTV_CVAL_HI_IN(BASE))
#define HWIO_QTMR_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK          0xffffffff
#define HWIO_QTMR_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_QTMR_CNTV_TVAL_ADDR(BASE)                          (BASE      + 0x00000038)
#define HWIO_QTMR_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_QTMR_CNTV_TVAL_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTV_TVAL_ADDR(BASE), HWIO_QTMR_CNTV_TVAL_RMSK)
#define HWIO_QTMR_CNTV_TVAL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_TVAL_ADDR(BASE), m)
#define HWIO_QTMR_CNTV_TVAL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_TVAL_ADDR(BASE),v)
#define HWIO_QTMR_CNTV_TVAL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_TVAL_ADDR(BASE),m,v,HWIO_QTMR_CNTV_TVAL_IN(BASE))
#define HWIO_QTMR_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_QTMR_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_QTMR_CNTV_CTL_ADDR(BASE)                           (BASE      + 0x0000003c)
#define HWIO_QTMR_CNTV_CTL_RMSK                                  0x7
#define HWIO_QTMR_CNTV_CTL_IN(BASE)          \
        in_dword_masked(HWIO_QTMR_CNTV_CTL_ADDR(BASE), HWIO_QTMR_CNTV_CTL_RMSK)
#define HWIO_QTMR_CNTV_CTL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CTL_ADDR(BASE), m)
#define HWIO_QTMR_CNTV_CTL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CTL_ADDR(BASE),v)
#define HWIO_QTMR_CNTV_CTL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CTL_ADDR(BASE),m,v,HWIO_QTMR_CNTV_CTL_IN(BASE))
#define HWIO_QTMR_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_QTMR_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_QTMR_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_QTMR_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_QTMR_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_QTMR_CNTV_CTL_EN_SHFT                               0x0


#endif /* __TIMERHWIO_H__ */