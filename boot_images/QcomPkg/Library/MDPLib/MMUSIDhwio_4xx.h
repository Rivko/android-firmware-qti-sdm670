#ifndef __MMUSIDHWIO_4XX_H__
#define __MMUSIDHWIO_4XX_H__
/*
===========================================================================
*/
/**
  @file MMUSIDhwio_4xx.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r50.0.0_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    APPS_MMU500_SMMU_APP

  'Include' filters applied: CB0[APPS_MMU500_SMMU_APP] 
  'Exclude' filters applied: DUMMY RESERVED 

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: APPS_MMU500_SMMU_APP
 *--------------------------------------------------------------------------*/
#define SMMU_SS_APP_TCU_REG_BASE                                                        0x15000000
#define SMMU_500_APPS_REG_WRAPPER_BASE                                                  SMMU_SS_APP_TCU_REG_BASE

#define APPS_MMU500_SMMU_APP_REG_BASE                                                   (SMMU_500_APPS_REG_WRAPPER_BASE      + 0x00000000)
#define APPS_MMU500_SMMU_APP_REG_BASE_PHYS                                              (SMMU_500_APPS_REG_WRAPPER_BASE_PHYS + 0x00000000)
#define APPS_MMU500_SMMU_APP_REG_BASE_OFFS                                              0x00000000

#define HWIO_APPS_SMMU_SMR0_ADDR                                                        (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00000800)
#define HWIO_APPS_SMMU_SMR0_PHYS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00000800)
#define HWIO_APPS_SMMU_SMR0_OFFS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00000800)
#define HWIO_APPS_SMMU_SMR0_RMSK                                                        0xffff7fff
#define HWIO_APPS_SMMU_SMR0_IN          \
        in_dword_masked(HWIO_APPS_SMMU_SMR0_ADDR, HWIO_APPS_SMMU_SMR0_RMSK)
#define HWIO_APPS_SMMU_SMR0_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_SMR0_ADDR, m)
#define HWIO_APPS_SMMU_SMR0_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_SMR0_ADDR,v)
#define HWIO_APPS_SMMU_SMR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_SMR0_ADDR,m,v,HWIO_APPS_SMMU_SMR0_IN)
#define HWIO_APPS_SMMU_SMR0_VALID_BMSK                                                  0x80000000
#define HWIO_APPS_SMMU_SMR0_VALID_SHFT                                                        0x1f
#define HWIO_APPS_SMMU_SMR0_MASK_BMSK                                                   0x7fff0000
#define HWIO_APPS_SMMU_SMR0_MASK_SHFT                                                         0x10
#define HWIO_APPS_SMMU_SMR0_ID_BMSK                                                         0x7fff
#define HWIO_APPS_SMMU_SMR0_ID_SHFT                                                            0x0


#define HWIO_APPS_SMMU_SMR1_ADDR                                                        (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00000804)
#define HWIO_APPS_SMMU_SMR1_PHYS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00000804)
#define HWIO_APPS_SMMU_SMR1_OFFS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00000804)
#define HWIO_APPS_SMMU_SMR1_RMSK                                                        0xffff7fff
#define HWIO_APPS_SMMU_SMR1_IN          \
        in_dword_masked(HWIO_APPS_SMMU_SMR1_ADDR, HWIO_APPS_SMMU_SMR1_RMSK)
#define HWIO_APPS_SMMU_SMR1_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_SMR1_ADDR, m)
#define HWIO_APPS_SMMU_SMR1_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_SMR1_ADDR,v)
#define HWIO_APPS_SMMU_SMR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_SMR1_ADDR,m,v,HWIO_APPS_SMMU_SMR1_IN)
#define HWIO_APPS_SMMU_SMR1_VALID_BMSK                                                  0x80000000
#define HWIO_APPS_SMMU_SMR1_VALID_SHFT                                                        0x1f
#define HWIO_APPS_SMMU_SMR1_MASK_BMSK                                                   0x7fff0000
#define HWIO_APPS_SMMU_SMR1_MASK_SHFT                                                         0x10
#define HWIO_APPS_SMMU_SMR1_ID_BMSK                                                         0x7fff
#define HWIO_APPS_SMMU_SMR1_ID_SHFT                                                            0x0

#define HWIO_APPS_SMMU_SMR2_ADDR                                                        (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00000808)
#define HWIO_APPS_SMMU_SMR2_PHYS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00000808)
#define HWIO_APPS_SMMU_SMR2_OFFS                                                        (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00000808)
#define HWIO_APPS_SMMU_SMR2_RMSK                                                        0xffff7fff
#define HWIO_APPS_SMMU_SMR2_IN          \
        in_dword_masked(HWIO_APPS_SMMU_SMR2_ADDR, HWIO_APPS_SMMU_SMR2_RMSK)
#define HWIO_APPS_SMMU_SMR2_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_SMR2_ADDR, m)
#define HWIO_APPS_SMMU_SMR2_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_SMR2_ADDR,v)
#define HWIO_APPS_SMMU_SMR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_SMR2_ADDR,m,v,HWIO_APPS_SMMU_SMR2_IN)
#define HWIO_APPS_SMMU_SMR2_VALID_BMSK                                                  0x80000000
#define HWIO_APPS_SMMU_SMR2_VALID_SHFT                                                        0x1f
#define HWIO_APPS_SMMU_SMR2_MASK_BMSK                                                   0x7fff0000
#define HWIO_APPS_SMMU_SMR2_MASK_SHFT                                                         0x10
#define HWIO_APPS_SMMU_SMR2_ID_BMSK                                                         0x7fff
#define HWIO_APPS_SMMU_SMR2_ID_SHFT                                                            0x0

 



#define HWIO_APPS_SMMU_S2CR0_ADDR                                                       (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00000c00)
#define HWIO_APPS_SMMU_S2CR0_PHYS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00000c00)
#define HWIO_APPS_SMMU_S2CR0_OFFS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00000c00)
#define HWIO_APPS_SMMU_S2CR0_RMSK                                                       0x3ffffbff
#define HWIO_APPS_SMMU_S2CR0_IN          \
        in_dword_masked(HWIO_APPS_SMMU_S2CR0_ADDR, HWIO_APPS_SMMU_S2CR0_RMSK)
#define HWIO_APPS_SMMU_S2CR0_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_S2CR0_ADDR, m)
#define HWIO_APPS_SMMU_S2CR0_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_S2CR0_ADDR,v)
#define HWIO_APPS_SMMU_S2CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_S2CR0_ADDR,m,v,HWIO_APPS_SMMU_S2CR0_IN)
#define HWIO_APPS_SMMU_S2CR0_TRANSIENTCFG_BMSK                                          0x30000000
#define HWIO_APPS_SMMU_S2CR0_TRANSIENTCFG_SHFT                                                0x1c
#define HWIO_APPS_SMMU_S2CR0_INSTCFG_1_BMSK                                              0x8000000
#define HWIO_APPS_SMMU_S2CR0_INSTCFG_1_SHFT                                                   0x1b
#define HWIO_APPS_SMMU_S2CR0_INSTCFG_0_FB_BMSK                                           0x4000000
#define HWIO_APPS_SMMU_S2CR0_INSTCFG_0_FB_SHFT                                                0x1a
#define HWIO_APPS_SMMU_S2CR0_PRIVCFG_BSU_BMSK                                            0x3000000
#define HWIO_APPS_SMMU_S2CR0_PRIVCFG_BSU_SHFT                                                 0x18
#define HWIO_APPS_SMMU_S2CR0_WACFG_BMSK                                                   0xc00000
#define HWIO_APPS_SMMU_S2CR0_WACFG_SHFT                                                       0x16
#define HWIO_APPS_SMMU_S2CR0_RACFG_BMSK                                                   0x300000
#define HWIO_APPS_SMMU_S2CR0_RACFG_SHFT                                                       0x14
#define HWIO_APPS_SMMU_S2CR0_NSCFG_BMSK                                                    0xc0000
#define HWIO_APPS_SMMU_S2CR0_NSCFG_SHFT                                                       0x12
#define HWIO_APPS_SMMU_S2CR0_TYPE_BMSK                                                     0x30000
#define HWIO_APPS_SMMU_S2CR0_TYPE_SHFT                                                        0x10
#define HWIO_APPS_SMMU_S2CR0_MEM_ATTR_BMSK                                                  0xf000
#define HWIO_APPS_SMMU_S2CR0_MEM_ATTR_SHFT                                                     0xc
#define HWIO_APPS_SMMU_S2CR0_MTCFG_BMSK                                                      0x800
#define HWIO_APPS_SMMU_S2CR0_MTCFG_SHFT                                                        0xb
#define HWIO_APPS_SMMU_S2CR0_SHCFG_BMSK                                                      0x300
#define HWIO_APPS_SMMU_S2CR0_SHCFG_SHFT                                                        0x8
#define HWIO_APPS_SMMU_S2CR0_CBNDX_VMID_BMSK                                                  0xff
#define HWIO_APPS_SMMU_S2CR0_CBNDX_VMID_SHFT                                                   0x0

#define HWIO_APPS_SMMU_S2CR1_ADDR                                                       (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00000c04)
#define HWIO_APPS_SMMU_S2CR1_PHYS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00000c04)
#define HWIO_APPS_SMMU_S2CR1_OFFS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00000c04)
#define HWIO_APPS_SMMU_S2CR1_RMSK                                                       0x3ffffbff
#define HWIO_APPS_SMMU_S2CR1_IN          \
        in_dword_masked(HWIO_APPS_SMMU_S2CR1_ADDR, HWIO_APPS_SMMU_S2CR1_RMSK)
#define HWIO_APPS_SMMU_S2CR1_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_S2CR1_ADDR, m)
#define HWIO_APPS_SMMU_S2CR1_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_S2CR1_ADDR,v)
#define HWIO_APPS_SMMU_S2CR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_S2CR1_ADDR,m,v,HWIO_APPS_SMMU_S2CR1_IN)
#define HWIO_APPS_SMMU_S2CR1_TRANSIENTCFG_BMSK                                          0x30000000
#define HWIO_APPS_SMMU_S2CR1_TRANSIENTCFG_SHFT                                                0x1c
#define HWIO_APPS_SMMU_S2CR1_INSTCFG_1_BMSK                                              0x8000000
#define HWIO_APPS_SMMU_S2CR1_INSTCFG_1_SHFT                                                   0x1b
#define HWIO_APPS_SMMU_S2CR1_INSTCFG_0_FB_BMSK                                           0x4000000
#define HWIO_APPS_SMMU_S2CR1_INSTCFG_0_FB_SHFT                                                0x1a
#define HWIO_APPS_SMMU_S2CR1_PRIVCFG_BSU_BMSK                                            0x3000000
#define HWIO_APPS_SMMU_S2CR1_PRIVCFG_BSU_SHFT                                                 0x18
#define HWIO_APPS_SMMU_S2CR1_WACFG_BMSK                                                   0xc00000
#define HWIO_APPS_SMMU_S2CR1_WACFG_SHFT                                                       0x16
#define HWIO_APPS_SMMU_S2CR1_RACFG_BMSK                                                   0x300000
#define HWIO_APPS_SMMU_S2CR1_RACFG_SHFT                                                       0x14
#define HWIO_APPS_SMMU_S2CR1_NSCFG_BMSK                                                    0xc0000
#define HWIO_APPS_SMMU_S2CR1_NSCFG_SHFT                                                       0x12
#define HWIO_APPS_SMMU_S2CR1_TYPE_BMSK                                                     0x30000
#define HWIO_APPS_SMMU_S2CR1_TYPE_SHFT                                                        0x10
#define HWIO_APPS_SMMU_S2CR1_MEM_ATTR_BMSK                                                  0xf000
#define HWIO_APPS_SMMU_S2CR1_MEM_ATTR_SHFT                                                     0xc
#define HWIO_APPS_SMMU_S2CR1_MTCFG_BMSK                                                      0x800
#define HWIO_APPS_SMMU_S2CR1_MTCFG_SHFT                                                        0xb
#define HWIO_APPS_SMMU_S2CR1_SHCFG_BMSK                                                      0x300
#define HWIO_APPS_SMMU_S2CR1_SHCFG_SHFT                                                        0x8
#define HWIO_APPS_SMMU_S2CR1_CBNDX_VMID_BMSK                                                  0xff
#define HWIO_APPS_SMMU_S2CR1_CBNDX_VMID_SHFT                                                   0x0

#define HWIO_APPS_SMMU_CBAR0_ADDR                                                       (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00001000)
#define HWIO_APPS_SMMU_CBAR0_PHYS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00001000)
#define HWIO_APPS_SMMU_CBAR0_OFFS                                                       (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00001000)
#define HWIO_APPS_SMMU_CBAR0_RMSK                                                       0xffffffff
#define HWIO_APPS_SMMU_CBAR0_IN          \
        in_dword_masked(HWIO_APPS_SMMU_CBAR0_ADDR, HWIO_APPS_SMMU_CBAR0_RMSK)
#define HWIO_APPS_SMMU_CBAR0_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_CBAR0_ADDR, m)
#define HWIO_APPS_SMMU_CBAR0_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_CBAR0_ADDR,v)
#define HWIO_APPS_SMMU_CBAR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_CBAR0_ADDR,m,v,HWIO_APPS_SMMU_CBAR0_IN)
#define HWIO_APPS_SMMU_CBAR0_IRPTNDX_BMSK                                               0xff000000
#define HWIO_APPS_SMMU_CBAR0_IRPTNDX_SHFT                                                     0x18
#define HWIO_APPS_SMMU_CBAR0_WACFG_BMSK                                                   0xc00000
#define HWIO_APPS_SMMU_CBAR0_WACFG_SHFT                                                       0x16
#define HWIO_APPS_SMMU_CBAR0_RACFG_BMSK                                                   0x300000
#define HWIO_APPS_SMMU_CBAR0_RACFG_SHFT                                                       0x14
#define HWIO_APPS_SMMU_CBAR0_BSU_BMSK                                                      0xc0000
#define HWIO_APPS_SMMU_CBAR0_BSU_SHFT                                                         0x12
#define HWIO_APPS_SMMU_CBAR0_TYPE_BMSK                                                     0x30000
#define HWIO_APPS_SMMU_CBAR0_TYPE_SHFT                                                        0x10
#define HWIO_APPS_SMMU_CBAR0_MEMATTR_CBNDX_7_4_BMSK                                         0xf000
#define HWIO_APPS_SMMU_CBAR0_MEMATTR_CBNDX_7_4_SHFT                                            0xc
#define HWIO_APPS_SMMU_CBAR0_FB_CBNDX_3_BMSK                                                 0x800
#define HWIO_APPS_SMMU_CBAR0_FB_CBNDX_3_SHFT                                                   0xb
#define HWIO_APPS_SMMU_CBAR0_HYPC_CBNDX_2_BMSK                                               0x400
#define HWIO_APPS_SMMU_CBAR0_HYPC_CBNDX_2_SHFT                                                 0xa
#define HWIO_APPS_SMMU_CBAR0_BPSHCFG_CBNDX_1_0_BMSK                                          0x300
#define HWIO_APPS_SMMU_CBAR0_BPSHCFG_CBNDX_1_0_SHFT                                            0x8
#define HWIO_APPS_SMMU_CBAR0_VMID_BMSK                                                        0xff
#define HWIO_APPS_SMMU_CBAR0_VMID_SHFT                                                         0x0


#define HWIO_APPS_SMMU_CBA2R0_ADDR                                                      (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00001800)
#define HWIO_APPS_SMMU_CBA2R0_OFFS                                                      (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00001800)
#define HWIO_APPS_SMMU_CBA2R0_RMSK                                                             0x3
#define HWIO_APPS_SMMU_CBA2R0_IN          \
        in_dword_masked(HWIO_APPS_SMMU_CBA2R0_ADDR, HWIO_APPS_SMMU_CBA2R0_RMSK)
#define HWIO_APPS_SMMU_CBA2R0_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_CBA2R0_ADDR, m)
#define HWIO_APPS_SMMU_CBA2R0_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_CBA2R0_ADDR,v)
#define HWIO_APPS_SMMU_CBA2R0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_CBA2R0_ADDR,m,v,HWIO_APPS_SMMU_CBA2R0_IN)
#define HWIO_APPS_SMMU_CBA2R0_MONC_BMSK                                                        0x2
#define HWIO_APPS_SMMU_CBA2R0_MONC_SHFT                                                        0x1
#define HWIO_APPS_SMMU_CBA2R0_VA64_BMSK                                                        0x1
#define HWIO_APPS_SMMU_CBA2R0_VA64_SHFT                                                        0x0


#define HWIO_APPS_SMMU_CB0_SCTLR_ADDR                                                   (APPS_MMU500_SMMU_APP_REG_BASE      + 0x00040000)
#define HWIO_APPS_SMMU_CB0_SCTLR_PHYS                                                   (APPS_MMU500_SMMU_APP_REG_BASE_PHYS + 0x00040000)
#define HWIO_APPS_SMMU_CB0_SCTLR_OFFS                                                   (APPS_MMU500_SMMU_APP_REG_BASE_OFFS + 0x00040000)
#define HWIO_APPS_SMMU_CB0_SCTLR_RMSK                                                   0x3ffff7ff
#define HWIO_APPS_SMMU_CB0_SCTLR_IN          \
        in_dword_masked(HWIO_APPS_SMMU_CB0_SCTLR_ADDR, HWIO_APPS_SMMU_CB0_SCTLR_RMSK)
#define HWIO_APPS_SMMU_CB0_SCTLR_INM(m)      \
        in_dword_masked(HWIO_APPS_SMMU_CB0_SCTLR_ADDR, m)
#define HWIO_APPS_SMMU_CB0_SCTLR_OUT(v)      \
        out_dword(HWIO_APPS_SMMU_CB0_SCTLR_ADDR,v)
#define HWIO_APPS_SMMU_CB0_SCTLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APPS_SMMU_CB0_SCTLR_ADDR,m,v,HWIO_APPS_SMMU_CB0_SCTLR_IN)
#define HWIO_APPS_SMMU_CB0_SCTLR_NSCFG_BMSK                                             0x30000000
#define HWIO_APPS_SMMU_CB0_SCTLR_NSCFG_SHFT                                                   0x1c
#define HWIO_APPS_SMMU_CB0_SCTLR_WACFG_BMSK                                              0xc000000
#define HWIO_APPS_SMMU_CB0_SCTLR_WACFG_SHFT                                                   0x1a
#define HWIO_APPS_SMMU_CB0_SCTLR_RACFG_BMSK                                              0x3000000
#define HWIO_APPS_SMMU_CB0_SCTLR_RACFG_SHFT                                                   0x18
#define HWIO_APPS_SMMU_CB0_SCTLR_SHCFG_BMSK                                               0xc00000
#define HWIO_APPS_SMMU_CB0_SCTLR_SHCFG_SHFT                                                   0x16
#define HWIO_APPS_SMMU_CB0_SCTLR_FB_BMSK                                                  0x200000
#define HWIO_APPS_SMMU_CB0_SCTLR_FB_SHFT                                                      0x15
#define HWIO_APPS_SMMU_CB0_SCTLR_MTCFG_BMSK                                               0x100000
#define HWIO_APPS_SMMU_CB0_SCTLR_MTCFG_SHFT                                                   0x14
#define HWIO_APPS_SMMU_CB0_SCTLR_MEMATTR_BMSK                                              0xf0000
#define HWIO_APPS_SMMU_CB0_SCTLR_MEMATTR_SHFT                                                 0x10
#define HWIO_APPS_SMMU_CB0_SCTLR_TRANSIENTCFG_BMSK                                          0xc000
#define HWIO_APPS_SMMU_CB0_SCTLR_TRANSIENTCFG_SHFT                                             0xe
#define HWIO_APPS_SMMU_CB0_SCTLR_PTW_BMSK                                                   0x2000
#define HWIO_APPS_SMMU_CB0_SCTLR_PTW_SHFT                                                      0xd
#define HWIO_APPS_SMMU_CB0_SCTLR_ASIDPNE_BMSK                                               0x1000
#define HWIO_APPS_SMMU_CB0_SCTLR_ASIDPNE_SHFT                                                  0xc
#define HWIO_APPS_SMMU_CB0_SCTLR_UWXN_BMSK                                                   0x400
#define HWIO_APPS_SMMU_CB0_SCTLR_UWXN_SHFT                                                     0xa
#define HWIO_APPS_SMMU_CB0_SCTLR_WXN_BMSK                                                    0x200
#define HWIO_APPS_SMMU_CB0_SCTLR_WXN_SHFT                                                      0x9
#define HWIO_APPS_SMMU_CB0_SCTLR_HUPCF_BMSK                                                  0x100
#define HWIO_APPS_SMMU_CB0_SCTLR_HUPCF_SHFT                                                    0x8
#define HWIO_APPS_SMMU_CB0_SCTLR_CFCFG_BMSK                                                   0x80
#define HWIO_APPS_SMMU_CB0_SCTLR_CFCFG_SHFT                                                    0x7
#define HWIO_APPS_SMMU_CB0_SCTLR_CFIE_BMSK                                                    0x40
#define HWIO_APPS_SMMU_CB0_SCTLR_CFIE_SHFT                                                     0x6
#define HWIO_APPS_SMMU_CB0_SCTLR_CFRE_BMSK                                                    0x20
#define HWIO_APPS_SMMU_CB0_SCTLR_CFRE_SHFT                                                     0x5
#define HWIO_APPS_SMMU_CB0_SCTLR_E_BMSK                                                       0x10
#define HWIO_APPS_SMMU_CB0_SCTLR_E_SHFT                                                        0x4
#define HWIO_APPS_SMMU_CB0_SCTLR_AFFD_BMSK                                                     0x8
#define HWIO_APPS_SMMU_CB0_SCTLR_AFFD_SHFT                                                     0x3
#define HWIO_APPS_SMMU_CB0_SCTLR_AFE_BMSK                                                      0x4
#define HWIO_APPS_SMMU_CB0_SCTLR_AFE_SHFT                                                      0x2
#define HWIO_APPS_SMMU_CB0_SCTLR_TRE_BMSK                                                      0x2
#define HWIO_APPS_SMMU_CB0_SCTLR_TRE_SHFT                                                      0x1
#define HWIO_APPS_SMMU_CB0_SCTLR_M_BMSK                                                        0x1
#define HWIO_APPS_SMMU_CB0_SCTLR_M_SHFT                                                        0x0




#endif /* __MMUSIDHWIO_4XX_H__ */
