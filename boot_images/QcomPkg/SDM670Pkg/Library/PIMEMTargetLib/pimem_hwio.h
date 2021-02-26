#ifndef __PIMEM_HWIO_H__
#define __PIMEM_HWIO_H__
/*
===========================================================================
*/
/**
  @file pimem_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8998 (Nazgul) [nazgul_v1.0_p3q2r23.1]
 
  This file contains HWIO register definitions for the following modules:
    RAMBLUR_PIMEM_REGS

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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PIMEMTargetLib/pimem_hwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: RAMBLUR_PIMEM_REGS
 *--------------------------------------------------------------------------*/

#define RAMBLUR_PIMEM_REGS_REG_BASE                                                          (RAMBLUR_PIMEM_BASE      + 0x00000000)

#define HWIO_RAMBLUR_PIMEM_VERSION_ADDR                                                      (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000000)
#define HWIO_RAMBLUR_PIMEM_VERSION_RMSK                                                      0xffffffff
#define HWIO_RAMBLUR_PIMEM_VERSION_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VERSION_ADDR, HWIO_RAMBLUR_PIMEM_VERSION_RMSK)
#define HWIO_RAMBLUR_PIMEM_VERSION_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VERSION_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_VERSION_MAJOR_VERSION_BMSK                                        0xff000000
#define HWIO_RAMBLUR_PIMEM_VERSION_MAJOR_VERSION_SHFT                                              0x18
#define HWIO_RAMBLUR_PIMEM_VERSION_MINOR_VERSION_BMSK                                          0xff0000
#define HWIO_RAMBLUR_PIMEM_VERSION_MINOR_VERSION_SHFT                                              0x10
#define HWIO_RAMBLUR_PIMEM_VERSION_STEP_VERSION_BMSK                                             0xffff
#define HWIO_RAMBLUR_PIMEM_VERSION_STEP_VERSION_SHFT                                                0x0

#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_ADDR                                          (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000004)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_RMSK                                                 0xf
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_ADDR, HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_RMSK)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_ADDR,m,v,HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_IN)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_VAULT_START_ADDR_HI_BMSK                             0xf
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_HI_VAULT_START_ADDR_HI_SHFT                             0x0

#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_ADDR                                         (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000008)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_RMSK                                         0xfffff000
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_ADDR, HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_RMSK)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_ADDR,m,v,HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_IN)
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_VAULT_START_ADDR_LOW_BMSK                    0xfffff000
#define HWIO_RAMBLUR_PIMEM_VAULT_START_ADDR_LOW_VAULT_START_ADDR_LOW_SHFT                           0xc

#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_ADDR                                                   (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x0000000c)
#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_RMSK                                                    0x7f00000
#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_SIZE_ADDR, HWIO_RAMBLUR_PIMEM_VAULT_SIZE_RMSK)
#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_VAULT_SIZE_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_VAULT_SIZE_BMSK                                         0x7f00000
#define HWIO_RAMBLUR_PIMEM_VAULT_SIZE_VAULT_SIZE_SHFT                                              0x14

#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_ADDR                                                    (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000010)
#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_RMSK                                                     0x7f00000
#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN0_SIZE_ADDR, HWIO_RAMBLUR_PIMEM_WIN0_SIZE_RMSK)
#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN0_SIZE_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_WIN_SIZE_BMSK                                            0x7f00000
#define HWIO_RAMBLUR_PIMEM_WIN0_SIZE_WIN_SIZE_SHFT                                                 0x14

#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_ADDR(n)                                                 (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000010 + 0x8 * (n))
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_RMSK                                                     0x7f00000
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_MAXn                                                             3
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_INI(n)        \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WINn_SIZE_ADDR(n), HWIO_RAMBLUR_PIMEM_WINn_SIZE_RMSK)
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_INMI(n,mask)    \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WINn_SIZE_ADDR(n), mask)
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_WINn_SIZE_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_WINn_SIZE_ADDR(n),mask,val,HWIO_RAMBLUR_PIMEM_WINn_SIZE_INI(n))
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_WIN_SIZE_BMSK                                            0x7f00000
#define HWIO_RAMBLUR_PIMEM_WINn_SIZE_WIN_SIZE_SHFT                                                 0x14

#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_ADDR                                        (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000014)
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_RMSK                                               0x7
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_ADDR, HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_RMSK)
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_CONFIDENTIALITY_ENABLE_BMSK                    0x4
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_CONFIDENTIALITY_ENABLE_SHFT                    0x2
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_INTEGRITY_ENABLE_BMSK                          0x2
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_INTEGRITY_ENABLE_SHFT                          0x1
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_ANTIROLLBACK_ENABLE_BMSK                       0x1
#define HWIO_RAMBLUR_PIMEM_WIN0_ALGORITHM_CONFIG_WIN_ANTIROLLBACK_ENABLE_SHFT                       0x0

#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_ADDR(n)                                     (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000014 + 0x8 * (n))
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_RMSK                                               0x7
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_MAXn                                                 3
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_INI(n)        \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_ADDR(n), HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_RMSK)
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_INMI(n,mask)    \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_ADDR(n), mask)
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_ADDR(n),mask,val,HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_INI(n))
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_CONFIDENTIALITY_ENABLE_BMSK                    0x4
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_CONFIDENTIALITY_ENABLE_SHFT                    0x2
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_INTEGRITY_ENABLE_BMSK                          0x2
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_INTEGRITY_ENABLE_SHFT                          0x1
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_ANTIROLLBACK_ENABLE_BMSK                       0x1
#define HWIO_RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG_WIN_ANTIROLLBACK_ENABLE_SHFT                       0x0

#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_ADDR(n)                                     (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000030 + 0x4 * (n))
#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_RMSK                                        0xffffffff
#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_MAXn                                                 3
#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_AES_XTS_DATA_KEY_BMSK                       0xffffffff
#define HWIO_RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn_AES_XTS_DATA_KEY_SHFT                              0x0

#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_ADDR(n)                                    (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_RMSK                                       0xffffffff
#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_MAXn                                                3
#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_AES_XTS_TWEAK_KEY_BMSK                     0xffffffff
#define HWIO_RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn_AES_XTS_TWEAK_KEY_SHFT                            0x0

#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_ADDR(n)                           (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000050 + 0x4 * (n))
#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_RMSK                              0xffffffff
#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_MAXn                                       3
#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_PRI_AUTH_KEY_BMSK                 0xffffffff
#define HWIO_RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn_PRI_AUTH_KEY_SHFT                        0x0

#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_ADDR(n)                                (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000060 + 0x4 * (n))
#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_RMSK                                   0xffffffff
#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_MAXn                                            3
#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_OUTI(n,val)    \
        out_dword(HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_ADDR(n),val)
#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_HKDF_PRK_KEY_BMSK                      0xffffffff
#define HWIO_RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn_HKDF_PRK_KEY_SHFT                             0x0

#define HWIO_RAMBLUR_PIMEM_STATUS_ADDR                                                       (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000070)
#define HWIO_RAMBLUR_PIMEM_STATUS_RMSK                                                          0x3ffff
#define HWIO_RAMBLUR_PIMEM_STATUS_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_STATUS_ADDR, HWIO_RAMBLUR_PIMEM_STATUS_RMSK)
#define HWIO_RAMBLUR_PIMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_STATUS_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_STATUS_NUM_TXN_IN_FLIGHT_BMSK                                        0x3f000
#define HWIO_RAMBLUR_PIMEM_STATUS_NUM_TXN_IN_FLIGHT_SHFT                                            0xc
#define HWIO_RAMBLUR_PIMEM_STATUS_MASTER_OUTSTANDING_BMSK                                         0xfc0
#define HWIO_RAMBLUR_PIMEM_STATUS_MASTER_OUTSTANDING_SHFT                                           0x6
#define HWIO_RAMBLUR_PIMEM_STATUS_SLAVE_OUTSTANDING_BMSK                                           0x3f
#define HWIO_RAMBLUR_PIMEM_STATUS_SLAVE_OUTSTANDING_SHFT                                            0x0

#define HWIO_RAMBLUR_PIMEM_CTL_ADDR                                                          (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000074)
#define HWIO_RAMBLUR_PIMEM_CTL_RMSK                                                          0x7fffffff
#define HWIO_RAMBLUR_PIMEM_CTL_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_CTL_ADDR, HWIO_RAMBLUR_PIMEM_CTL_RMSK)
#define HWIO_RAMBLUR_PIMEM_CTL_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_CTL_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_CTL_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_CTL_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_CTL_ADDR,m,v,HWIO_RAMBLUR_PIMEM_CTL_IN)
#define HWIO_RAMBLUR_PIMEM_CTL_MEM_SLOT_WMARK_BMSK                                           0x7c000000
#define HWIO_RAMBLUR_PIMEM_CTL_MEM_SLOT_WMARK_SHFT                                                 0x1a
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_SIPHASH_IV_SWAP_SEL_BMSK                                  0x3c00000
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_SIPHASH_IV_SWAP_SEL_SHFT                                       0x16
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_SIPHASH_DATA_SWAP_SEL_BMSK                                 0x3c0000
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_SIPHASH_DATA_SWAP_SEL_SHFT                                     0x12
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_KEY_SWAP_SEL_BMSK                                       0x3f000
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_KEY_SWAP_SEL_SHFT                                           0xc
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_IV_SWAP_SEL_BMSK                                          0xf00
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_IV_SWAP_SEL_SHFT                                            0x8
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_DATA_SWAP_SEL_BMSK                                         0xf0
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_DATA_SWAP_SEL_SHFT                                          0x4
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_FORCE_SINGLE_DP_BMSK                                        0x8
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_AES_FORCE_SINGLE_DP_SHFT                                        0x3
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_CGC_DISABLE_BMSK                                                0x4
#define HWIO_RAMBLUR_PIMEM_CTL_CLIB_CGC_DISABLE_SHFT                                                0x2
#define HWIO_RAMBLUR_PIMEM_CTL_PIMEM_LOCAL_CGC_DISABLE_BMSK                                         0x2
#define HWIO_RAMBLUR_PIMEM_CTL_PIMEM_LOCAL_CGC_DISABLE_SHFT                                         0x1
#define HWIO_RAMBLUR_PIMEM_CTL_PIMEM_DISABLE_BMSK                                                   0x1
#define HWIO_RAMBLUR_PIMEM_CTL_PIMEM_DISABLE_SHFT                                                   0x0

#define HWIO_RAMBLUR_PIMEM_ADV_CTL_ADDR                                                      (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000078)
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_RMSK                                                        0x3fffff
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ADV_CTL_ADDR, HWIO_RAMBLUR_PIMEM_ADV_CTL_RMSK)
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ADV_CTL_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_ADV_CTL_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_ADV_CTL_ADDR,m,v,HWIO_RAMBLUR_PIMEM_ADV_CTL_IN)
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_OPTIMIZE_BW_FOR_AFULL_EQ_0_WRITES_BMSK                      0x200000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_OPTIMIZE_BW_FOR_AFULL_EQ_0_WRITES_SHFT                          0x15
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_DISABLE_ON_AUTH_ERROR_BMSK                                  0x100000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_DISABLE_ON_AUTH_ERROR_SHFT                                      0x14
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_PROTNS_BMSK                                           0x80000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_PROTNS_SHFT                                              0x13
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ACACHEABLE_BMSK                                       0x40000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ACACHEABLE_SHFT                                          0x12
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AWRITETHROUGH_BMSK                                    0x20000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AWRITETHROUGH_SHFT                                       0x11
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINNERCACHEABLE_BMSK                                  0x10000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINNERCACHEABLE_SHFT                                     0x10
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ANOALLOCATE_BMSK                                       0x8000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ANOALLOCATE_SHFT                                          0xf
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_PRIORITYLVL_BMSK                                       0x6000
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_PRIORITYLVL_SHFT                                          0xd
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AREQPRIORITY_BMSK                                      0x1800
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AREQPRIORITY_SHFT                                         0xb
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ADEVICE_BMSK                                            0x400
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ADEVICE_SHFT                                              0xa
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ADEVICETYPE_BMSK                                        0x300
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ADEVICETYPE_SHFT                                          0x8
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ASHARED_BMSK                                             0x80
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_ASHARED_SHFT                                              0x7
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINNERSHARED_BMSK                                        0x40
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINNERSHARED_SHFT                                         0x6
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINST_BMSK                                               0x20
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_AINST_SHFT                                                0x5
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_APRIV_BMSK                                               0x10
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_MASTER_APRIV_SHFT                                                0x4
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_AHB2AHB_BRIDGE_HYSTERESIS_VALUE_BMSK                             0xf
#define HWIO_RAMBLUR_PIMEM_ADV_CTL_AHB2AHB_BRIDGE_HYSTERESIS_VALUE_SHFT                             0x0

#define HWIO_RAMBLUR_PIMEM_ESR_ADDR                                                          (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x0000007c)
#define HWIO_RAMBLUR_PIMEM_ESR_RMSK                                                                0x7f
#define HWIO_RAMBLUR_PIMEM_ESR_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESR_ADDR, HWIO_RAMBLUR_PIMEM_ESR_RMSK)
#define HWIO_RAMBLUR_PIMEM_ESR_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESR_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_ESR_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_ESR_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_ESR_ADDR,m,v,HWIO_RAMBLUR_PIMEM_ESR_IN)
#define HWIO_RAMBLUR_PIMEM_ESR_PIMEM_KERNEL_RACE_CONDITION_BMSK                                    0x40
#define HWIO_RAMBLUR_PIMEM_ESR_PIMEM_KERNEL_RACE_CONDITION_SHFT                                     0x6
#define HWIO_RAMBLUR_PIMEM_ESR_S_AFULL_ERROR_BMSK                                                  0x20
#define HWIO_RAMBLUR_PIMEM_ESR_S_AFULL_ERROR_SHFT                                                   0x5
#define HWIO_RAMBLUR_PIMEM_ESR_VAULT_TRANSACTION_ERROR_BMSK                                        0x10
#define HWIO_RAMBLUR_PIMEM_ESR_VAULT_TRANSACTION_ERROR_SHFT                                         0x4
#define HWIO_RAMBLUR_PIMEM_ESR_AUTH_ERROR_BMSK                                                      0x8
#define HWIO_RAMBLUR_PIMEM_ESR_AUTH_ERROR_SHFT                                                      0x3
#define HWIO_RAMBLUR_PIMEM_ESR_PIMEM_DISABLED_ERROR_BMSK                                            0x4
#define HWIO_RAMBLUR_PIMEM_ESR_PIMEM_DISABLED_ERROR_SHFT                                            0x2
#define HWIO_RAMBLUR_PIMEM_ESR_CLIENT_ADDR_DECODE_ERROR_BMSK                                        0x2
#define HWIO_RAMBLUR_PIMEM_ESR_CLIENT_ADDR_DECODE_ERROR_SHFT                                        0x1
#define HWIO_RAMBLUR_PIMEM_ESR_CFG_ADDR_DECODE_ERROR_BMSK                                           0x1
#define HWIO_RAMBLUR_PIMEM_ESR_CFG_ADDR_DECODE_ERROR_SHFT                                           0x0

#define HWIO_RAMBLUR_PIMEM_EAR0_ADDR                                                         (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000080)
#define HWIO_RAMBLUR_PIMEM_EAR0_RMSK                                                         0xffffffff
#define HWIO_RAMBLUR_PIMEM_EAR0_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_EAR0_ADDR, HWIO_RAMBLUR_PIMEM_EAR0_RMSK)
#define HWIO_RAMBLUR_PIMEM_EAR0_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_EAR0_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_EAR0_PA_BMSK                                                      0xffffffff
#define HWIO_RAMBLUR_PIMEM_EAR0_PA_SHFT                                                             0x0

#define HWIO_RAMBLUR_PIMEM_ESYNR0_ADDR                                                       (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000084)
#define HWIO_RAMBLUR_PIMEM_ESYNR0_RMSK                                                       0xffffffff
#define HWIO_RAMBLUR_PIMEM_ESYNR0_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESYNR0_ADDR, HWIO_RAMBLUR_PIMEM_ESYNR0_RMSK)
#define HWIO_RAMBLUR_PIMEM_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESYNR0_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_ESYNR0_ATID_BMSK                                                  0xff000000
#define HWIO_RAMBLUR_PIMEM_ESYNR0_ATID_SHFT                                                        0x18
#define HWIO_RAMBLUR_PIMEM_ESYNR0_AVMID_BMSK                                                   0xff0000
#define HWIO_RAMBLUR_PIMEM_ESYNR0_AVMID_SHFT                                                       0x10
#define HWIO_RAMBLUR_PIMEM_ESYNR0_ABID_BMSK                                                      0xe000
#define HWIO_RAMBLUR_PIMEM_ESYNR0_ABID_SHFT                                                         0xd
#define HWIO_RAMBLUR_PIMEM_ESYNR0_APID_BMSK                                                      0x1f00
#define HWIO_RAMBLUR_PIMEM_ESYNR0_APID_SHFT                                                         0x8
#define HWIO_RAMBLUR_PIMEM_ESYNR0_AMID_BMSK                                                        0xff
#define HWIO_RAMBLUR_PIMEM_ESYNR0_AMID_SHFT                                                         0x0

#define HWIO_RAMBLUR_PIMEM_ESYNR1_ADDR                                                       (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000088)
#define HWIO_RAMBLUR_PIMEM_ESYNR1_RMSK                                                       0x3fffffff
#define HWIO_RAMBLUR_PIMEM_ESYNR1_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESYNR1_ADDR, HWIO_RAMBLUR_PIMEM_ESYNR1_RMSK)
#define HWIO_RAMBLUR_PIMEM_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESYNR1_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_ESYNR1_BURSTLEN_BMSK                                              0x20000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_BURSTLEN_SHFT                                                    0x1d
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ARDALLOCATE_BMSK                                           0x10000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ARDALLOCATE_SHFT                                                 0x1c
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ABURST_BMSK                                                 0x8000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ABURST_SHFT                                                      0x1b
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AEXCLUSIVE_BMSK                                             0x4000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AEXCLUSIVE_SHFT                                                  0x1a
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AWRITE_BMSK                                                 0x2000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AWRITE_SHFT                                                      0x19
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AFULL_BMSK                                                  0x1000000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AFULL_SHFT                                                       0x18
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ARDBEADNDXEN_BMSK                                            0x800000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ARDBEADNDXEN_SHFT                                                0x17
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AOOO_BMSK                                                    0x400000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AOOO_SHFT                                                        0x16
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APREQPRIORITY_BMSK                                           0x380000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APREQPRIORITY_SHFT                                               0x13
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ASIZE_BMSK                                                    0x70000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ASIZE_SHFT                                                       0x10
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AMSSSELFAUTH_BMSK                                              0x8000
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AMSSSELFAUTH_SHFT                                                 0xf
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ALEN_BMSK                                                      0x7f00
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ALEN_SHFT                                                         0x8
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AINST_BMSK                                                       0x80
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AINST_SHFT                                                        0x7
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APROTNS_BMSK                                                     0x40
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APROTNS_SHFT                                                      0x6
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APRIV_BMSK                                                       0x20
#define HWIO_RAMBLUR_PIMEM_ESYNR1_APRIV_SHFT                                                        0x5
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AINNERSHARED_BMSK                                                0x10
#define HWIO_RAMBLUR_PIMEM_ESYNR1_AINNERSHARED_SHFT                                                 0x4
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ASHARED_BMSK                                                      0x8
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ASHARED_SHFT                                                      0x3
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ADEVICETYPE_BMSK                                                  0x6
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ADEVICETYPE_SHFT                                                  0x1
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ADEVICE_BMSK                                                      0x1
#define HWIO_RAMBLUR_PIMEM_ESYNR1_ADEVICE_SHFT                                                      0x0

#define HWIO_RAMBLUR_PIMEM_DEBUG_ADDR                                                        (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x0000008c)
#define HWIO_RAMBLUR_PIMEM_DEBUG_RMSK                                                         0x3ffffff
#define HWIO_RAMBLUR_PIMEM_DEBUG_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_DEBUG_ADDR, HWIO_RAMBLUR_PIMEM_DEBUG_RMSK)
#define HWIO_RAMBLUR_PIMEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_DEBUG_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_DEBUG_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_DEBUG_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_DEBUG_ADDR,m,v,HWIO_RAMBLUR_PIMEM_DEBUG_IN)
#define HWIO_RAMBLUR_PIMEM_DEBUG_MAX_MEM_SLOTS_LIMIT_MINUS_1_BMSK                             0x3c00000
#define HWIO_RAMBLUR_PIMEM_DEBUG_MAX_MEM_SLOTS_LIMIT_MINUS_1_SHFT                                  0x16
#define HWIO_RAMBLUR_PIMEM_DEBUG_S_AFULL_CHECKER_EN_BMSK                                       0x200000
#define HWIO_RAMBLUR_PIMEM_DEBUG_S_AFULL_CHECKER_EN_SHFT                                           0x15
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSUME_S_AFULL_EQUALS_0_BMSK                            0x100000
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSUME_S_AFULL_EQUALS_0_SHFT                                0x14
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSUME_S_AFULL_EQUALS_1_BMSK                             0x80000
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSUME_S_AFULL_EQUALS_1_SHFT                                0x13
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSERT_NON_FATAL_IRQ_LINE_BMSK                           0x40000
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSERT_NON_FATAL_IRQ_LINE_SHFT                              0x12
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSERT_FATAL_IRQ_LINE_BMSK                               0x20000
#define HWIO_RAMBLUR_PIMEM_DEBUG_FORCE_ASSERT_FATAL_IRQ_LINE_SHFT                                  0x11
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_REORDERING_FOR_ADEVICE_BMSK                             0x1000
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_REORDERING_FOR_ADEVICE_SHFT                                0xc
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_OUT_OF_ORDER_PROCESSING_BMSK                             0x800
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_OUT_OF_ORDER_PROCESSING_SHFT                               0xb
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_POSTED_WRITES_BMSK                                       0x400
#define HWIO_RAMBLUR_PIMEM_DEBUG_DISABLE_POSTED_WRITES_SHFT                                         0xa
#define HWIO_RAMBLUR_PIMEM_DEBUG_IGNORE_ALL_AUTH_ERRORS_AFTER_FIRST_AUTH_ERROR_BMSK               0x200
#define HWIO_RAMBLUR_PIMEM_DEBUG_IGNORE_ALL_AUTH_ERRORS_AFTER_FIRST_AUTH_ERROR_SHFT                 0x9
#define HWIO_RAMBLUR_PIMEM_DEBUG_DBG_BUS_SEL_BMSK                                                 0x1ff
#define HWIO_RAMBLUR_PIMEM_DEBUG_DBG_BUS_SEL_SHFT                                                   0x0

#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_ADDR(n)                                  (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x00000090 + 0x4 * (n))
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_RMSK                                     0xffffffff
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_MAXn                                             15
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_INI(n)        \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_ADDR(n), HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_RMSK)
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_ADDR(n), mask)
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_RACE_COND_STATUS_BITS_BMSK               0xffffffff
#define HWIO_RAMBLUR_PIMEM_KERNEL_RACE_COND_STATUSn_RACE_COND_STATUS_BITS_SHFT                      0x0

#define HWIO_RAMBLUR_PIMEM_ESR_MASK_ADDR                                                     (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x000000d0)
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_RMSK                                                           0x7f
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESR_MASK_ADDR, HWIO_RAMBLUR_PIMEM_ESR_MASK_RMSK)
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_ESR_MASK_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_ESR_MASK_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_ESR_MASK_ADDR,m,v,HWIO_RAMBLUR_PIMEM_ESR_MASK_IN)
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_PIMEM_KERNEL_RACE_CONDITION_MASK_BMSK                          0x40
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_PIMEM_KERNEL_RACE_CONDITION_MASK_SHFT                           0x6
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_S_AFULL_ERROR_MASK_BMSK                                        0x20
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_S_AFULL_ERROR_MASK_SHFT                                         0x5
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_VAULT_TRANSACTION_ERROR_MASK_BMSK                              0x10
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_VAULT_TRANSACTION_ERROR_MASK_SHFT                               0x4
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_AUTH_ERROR_MASK_BMSK                                            0x8
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_AUTH_ERROR_MASK_SHFT                                            0x3
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_PIMEM_DISABLED_ERROR_MASK_BMSK                                  0x4
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_PIMEM_DISABLED_ERROR_MASK_SHFT                                  0x2
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_CLIENT_ADDR_DECODE_ERROR_MASK_BMSK                              0x2
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_CLIENT_ADDR_DECODE_ERROR_MASK_SHFT                              0x1
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_CFG_ADDR_DECODE_ERROR_MASK_BMSK                                 0x1
#define HWIO_RAMBLUR_PIMEM_ESR_MASK_CFG_ADDR_DECODE_ERROR_MASK_SHFT                                 0x0

#define HWIO_RAMBLUR_PIMEM_WIN_INIT_ADDR                                                     (RAMBLUR_PIMEM_REGS_REG_BASE      + 0x000000d4)
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_RMSK                                                            0xf
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_IN          \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN_INIT_ADDR, HWIO_RAMBLUR_PIMEM_WIN_INIT_RMSK)
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_INM(m)      \
        in_dword_masked(HWIO_RAMBLUR_PIMEM_WIN_INIT_ADDR, m)
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_OUT(v)      \
        out_dword(HWIO_RAMBLUR_PIMEM_WIN_INIT_ADDR,v)
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RAMBLUR_PIMEM_WIN_INIT_ADDR,m,v,HWIO_RAMBLUR_PIMEM_WIN_INIT_IN)
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN3_INIT_MODE_BMSK                                             0x8
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN3_INIT_MODE_SHFT                                             0x3
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN2_INIT_MODE_BMSK                                             0x4
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN2_INIT_MODE_SHFT                                             0x2
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN1_INIT_MODE_BMSK                                             0x2
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN1_INIT_MODE_SHFT                                             0x1
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN0_INIT_MODE_BMSK                                             0x1
#define HWIO_RAMBLUR_PIMEM_WIN_INIT_WIN0_INIT_MODE_SHFT                                             0x0


#endif /* __PIMEM_HWIO_H__ */
