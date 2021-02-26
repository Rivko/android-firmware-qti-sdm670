#ifndef __MMUSIDHWIO_3XX_H__
#define __MMUSIDHWIO_3XX_H__
/*
===========================================================================
*/
/**
  @file MMUSIDhwio_3xx.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG
    MMSS_BIMC_SMMU_SMMU_GLOBAL0
    MMSS_BIMC_SMMU_SMMU_GLOBAL1
    MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK

  'Include' filters applied: CBN_SCTLR[MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK] CBARn[MMSS_BIMC_SMMU_SMMU_GLOBAL1] S[M2C]*Rn[MMSS_BIMC_SMMU_SMMU_GLOBAL0] HLOS2_VOTE_ALL_MMSS_SMMU_[GCC_CLK_CTL_REG] 
  'Exclude' filters applied: RESERVED DUMMY 

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

#define MMSS_BASE                                                   0x0c800000
#define CLK_CTL_BASE                                                0x00100000

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                 (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                            0x00000000

#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e01c)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_OFFS                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0007e01c)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_RMSK                                               0x80000001
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_ADDR, HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_RMSK)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_IN)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_CLK_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e040)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_OFFS                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0007e040)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_RMSK                                               0x80000001
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_ADDR, HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_RMSK)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_IN)
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_PWR_ON_BMSK                                        0x80000000
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_PWR_ON_SHFT                                              0x1f
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_SW_COLLAPSE_BMSK                                          0x1
#define HWIO_GCC_HLOS2_VOTE_ALL_MMSS_SMMU_GDS_SW_COLLAPSE_SHFT                                          0x0

/*----------------------------------------------------------------------------
 * MODULE: MMSS_BIMC_SMMU_SMMU_GLOBAL0
 *--------------------------------------------------------------------------*/

#define MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE                                         (MMSS_BASE      + 0x00500000)
#define MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE_OFFS                                    0x00500000

#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_ADDR(n)                                        (MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_OFFS(n)                                        (MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE_OFFS + 0x00000800 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_RMSK                                           0xffffffff
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_MAXn                                                   63
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_INI(n)        \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_ADDR(n), HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_RMSK)
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_ADDR(n), mask)
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_OUTI(n,val)    \
        out_dword(HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_ADDR(n),val)
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_ADDR(n),mask,val,HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_INI(n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_FIELD_BMSK                                     0xffffffff
#define HWIO_MMSS_BIMC_SMMU_SMMU_SMRn_FIELD_SHFT                                            0x0

#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_ADDR(n)                                       (MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_OFFS(n)                                       (MMSS_BIMC_SMMU_SMMU_GLOBAL0_REG_BASE_OFFS + 0x00000c00 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_RMSK                                          0x3ffffbff
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_MAXn                                                  63
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_INI(n)        \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_ADDR(n), HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_RMSK)
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_INMI(n,mask)    \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_ADDR(n), mask)
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_OUTI(n,val)    \
        out_dword(HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_ADDR(n),val)
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_ADDR(n),mask,val,HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_INI(n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_TRANSIENTCFG_BMSK                             0x30000000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_TRANSIENTCFG_SHFT                                   0x1c
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_INSTCFG_BMSK                                   0xc000000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_INSTCFG_SHFT                                        0x1a
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_PRIVCFG_BMSK                                   0x3000000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_PRIVCFG_SHFT                                        0x18
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_WACFG_BMSK                                      0xc00000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_WACFG_SHFT                                          0x16
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_RACFG_BMSK                                      0x300000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_RACFG_SHFT                                          0x14
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_NSCFG_BMSK                                       0xc0000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_NSCFG_SHFT                                          0x12
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_TYPE_BMSK                                        0x30000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_TYPE_SHFT                                           0x10
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_MEMATTR_BMSK                                      0xf000
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_MEMATTR_SHFT                                         0xc
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_MTCFG_BMSK                                         0x800
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_MTCFG_SHFT                                           0xb
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_SHCFG_BMSK                                         0x300
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_SHCFG_SHFT                                           0x8
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_FIELD_BMSK                                          0xff
#define HWIO_MMSS_BIMC_SMMU_SMMU_S2CRn_FIELD_SHFT                                           0x0

/*----------------------------------------------------------------------------
 * MODULE: MMSS_BIMC_SMMU_SMMU_GLOBAL1
 *--------------------------------------------------------------------------*/

#define MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE                        (MMSS_BASE      + 0x00501000)
#define MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE_OFFS                   0x00501000

#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_ADDR(n)                      (MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_OFFS(n)                      (MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE_OFFS + 0x00000000 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_RMSK                         0xffffffff
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_MAXn                                 23
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_INI(n)        \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_ADDR(n), HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_RMSK)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_INMI(n,mask)    \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_ADDR(n), mask)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_OUTI(n,val)    \
        out_dword(HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_ADDR(n),val)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_ADDR(n),mask,val,HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_INI(n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_IRPTNDX_BMSK                 0xff000000
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_IRPTNDX_SHFT                       0x18
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_WACFG_BMSK                     0xc00000
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_WACFG_SHFT                         0x16
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_RACFG_BMSK                     0x300000
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_RACFG_SHFT                         0x14
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_BSU_BMSK                        0xc0000
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_BSU_SHFT                           0x12
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_TYPE_BMSK                       0x30000
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_TYPE_SHFT                          0x10
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_FIELD_BMSK                       0xff00
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_FIELD_SHFT                          0x8
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_VMID_BMSK                          0xff
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBARn_VMID_SHFT                           0x0

#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_ADDR(n)                     (MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_OFFS(n)                     (MMSS_BIMC_SMMU_SMMU_GLOBAL1_REG_BASE_OFFS + 0x00000800 + 0x4 * (n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_RMSK                               0x3
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_MAXn                                23
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_INI(n)        \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_ADDR(n), HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_RMSK)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_INMI(n,mask)    \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_ADDR(n), mask)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_OUTI(n,val)    \
        out_dword(HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_ADDR(n),val)
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_ADDR(n),mask,val,HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_INI(n))
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_MONC_BMSK                          0x2
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_MONC_SHFT                          0x1
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_RW64_BMSK                          0x1
#define HWIO_MMSS_BIMC_SMMU_SMMU_CBA2Rn_RW64_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK
 *--------------------------------------------------------------------------*/

#define MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK_REG_BASE                                       (MMSS_BASE      + 0x00520000)
#define MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK_REG_BASE_OFFS                                  0x00520000

#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ADDR                                             (MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK_REG_BASE      + 0x00000000)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_OFFS                                             (MMSS_BIMC_SMMU_A_CB0_CB0_SMMU_CONTEXT_BANK_REG_BASE_OFFS + 0x00000000)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_RMSK                                             0x7ffff7ff
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_IN          \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ADDR, HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_RMSK)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_INM(m)      \
        in_dword_masked(HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ADDR, m)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_OUT(v)      \
        out_dword(HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ADDR,v)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ADDR,m,v,HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_IN)
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_UCI_BMSK                                         0x40000000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_UCI_SHFT                                               0x1e
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_NSCFG_BMSK                                       0x30000000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_NSCFG_SHFT                                             0x1c
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_WACFG_BMSK                                        0xc000000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_WACFG_SHFT                                             0x1a
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_RACFG_BMSK                                        0x3000000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_RACFG_SHFT                                             0x18
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_SHCFG_BMSK                                         0xc00000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_SHCFG_SHFT                                             0x16
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_FB_BMSK                                            0x200000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_FB_SHFT                                                0x15
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_MTCFG_BMSK                                         0x100000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_MTCFG_SHFT                                             0x14
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_MEMATTR_BMSK                                        0xf0000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_MEMATTR_SHFT                                           0x10
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_TRANSIENTCFG_BMSK                                    0xc000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_TRANSIENTCFG_SHFT                                       0xe
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_PTW_BMSK                                             0x2000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_PTW_SHFT                                                0xd
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ASIDPNE_BMSK                                         0x1000
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_ASIDPNE_SHFT                                            0xc
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_UWXN_BMSK                                             0x400
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_UWXN_SHFT                                               0xa
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_WXN_BMSK                                              0x200
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_WXN_SHFT                                                0x9
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_HUPCF_BMSK                                            0x100
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_HUPCF_SHFT                                              0x8
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFCFG_BMSK                                             0x80
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFCFG_SHFT                                              0x7
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFIE_BMSK                                              0x40
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFIE_SHFT                                               0x6
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFRE_BMSK                                              0x20
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_CFRE_SHFT                                               0x5
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_E_BMSK                                                 0x10
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_E_SHFT                                                  0x4
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_AFFD_BMSK                                               0x8
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_AFFD_SHFT                                               0x3
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_AFE_BMSK                                                0x4
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_AFE_SHFT                                                0x2
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_TRE_BMSK                                                0x2
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_TRE_SHFT                                                0x1
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_M_BMSK                                                  0x1
#define HWIO_MMSS_BIMC_SMMU_A_CB0_SMMU_CBN_SCTLR_M_SHFT                                                  0x0


#endif /* __MMUSIDHWIO_3XX_H__ */
