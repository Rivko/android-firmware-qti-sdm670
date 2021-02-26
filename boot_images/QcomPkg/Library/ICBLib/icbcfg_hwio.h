#ifndef __ICBCFG_HWIO_H__
#define __ICBCFG_HWIO_H__
/*
===========================================================================
*/
/**
  @file icbcfg_query_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r25.1.2_F0_ECO]
 
  This file contains HWIO register definitions for the following modules:
    MEMNOC_MEM_NOC

  'Include' filters applied: ADDR_TRANS[MEMNOC_MEM_NOC] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBLib/icbcfg_hwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MEMNOC_MEM_NOC
 *--------------------------------------------------------------------------*/

#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_ADDR(x)                                                  ((x) + 0x00000000)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_RMSK                                                       0xffffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_IN(x)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_ADDR(x), HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_INM(x, m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_ADDR(x), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_BMSK                                            0xff0000
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_UNITTYPEID_SHFT                                                0x10
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_BMSK                                              0xffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_LOW_UNITCONFID_SHFT                                                 0x0

#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_ADDR(x)                                                 ((x) + 0x00000004)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_RMSK                                                        0xffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_IN(x)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_ADDR(x), HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_INM(x, m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_ADDR(x), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_QNOCID_BMSK                                                 0xffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_SWID_HIGH_QNOCID_SHFT                                                    0x0

#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ADDR(x)                                                ((x) + 0x00000008)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_RMSK                                                          0x1
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_IN(x)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ADDR(x), HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_INM(x, m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ADDR(x), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_OUT(x, v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ADDR(x),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ADDR(x),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_IN(x))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ENABLE_BMSK                                                   0x1
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW_ENABLE_SHFT                                                   0x0

#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ADDR(x)                                                 ((x) + 0x00000010)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_RMSK                                                          0x3f
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_IN(x)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ADDR(x), HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_INM(x, m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ADDR(x), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_OUT(x, v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ADDR(x),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ADDR(x),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_IN(x))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ERROR_BMSK                                                    0x3f
#define HWIO_MEMNOC_ADDR_TRANSLATOR_ERROR_LOW_ERROR_SHFT                                                     0x0

/*----------------------------------------------------------------------------
 * OFFSETn[0..5]
 *--------------------------------------------------------------------------*/
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRI(x,n)                                               ((x) + ((n) * 0x10) + 0x00000018)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_RMSK                                                  0xffffffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_INI(x,n)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRI(x,n), HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_INMI(x,n,m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRI(x,n), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_OUTI(x,n,v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRI(x,n),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRI(x,n),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_INI(x,n))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRH_LSB_BMSK                                        0xf8000000
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_ADDRH_LSB_SHFT                                              0x1b
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_RSV0_BMSK                                              0x7ffffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW_RSV0_SHFT                                                    0x0

#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRI(x,n)                                              ((x) + ((n) * 0x10) + 0x0000001c)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_RMSK                                                        0xf
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_INI(x,n)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRI(x,n), HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_INMI(x,n,m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRI(x,n), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_OUTI(x,n,v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRI(x,n),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRI(x,n),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_INI(x,n))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRH_MSB_BMSK                                              0xf
#define HWIO_MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH_ADDRH_MSB_SHFT                                              0x0

/*----------------------------------------------------------------------------
 * BASEn[1..5]
 *--------------------------------------------------------------------------*/
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRI(x,n)                                                 ((x) + (((n)-1) * 0x10) + 0x00000020)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_RMSK                                                    0xffffffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_INI(x,n)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRI(x,n), HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_INMI(x,n,m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRI(x,n), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_OUTI(x,n,v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRI(x,n),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRI(x,n),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_INI(x,n))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRH_LSB_BMSK                                          0xf8000000
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_ADDRH_LSB_SHFT                                                0x1b
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_RSV0_BMSK                                                0x7ffffff
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_LOW_RSV0_SHFT                                                      0x0

#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRI(x,n)                                                ((x) + (((n)-1) * 0x10) + 0x00000024)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_RMSK                                                          0xf
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_INI(x,n)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRI(x,n), HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_RMSK)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_INMI(x,n,m)      \
        in_dword_masked(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRI(x,n), m)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_OUTI(x,n,v)      \
        out_dword(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRI(x,n),v)
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRI(x,n),m,v,HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_INI(x,n))
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRH_MSB_BMSK                                                0xf
#define HWIO_MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH_ADDRH_MSB_SHFT                                                0x0

/*
===========================================================================
*/
/**
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) v2 [napali_v2.0_p3q1r23.6_F02_ECO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    LLCC0_BEAC

  'Include' filters applied: ADDR[LLCC0_BEAC] 
  'Exclude' filters applied: 
*/

/*----------------------------------------------------------------------------
 * MODULE: LLCC_BEAC_ADDR_TRANSLATOR
 *--------------------------------------------------------------------------*/

#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ADDR(x)                                        ((x) + 0x00000000)
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_RMSK                                              0x3f1
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_IN(x)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ADDR(x), HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_RMSK)
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_INM(x,m)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ADDR(x), m)
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_OUT(x,v)      \
        out_dword(HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ADDR(x),v)
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ADDR(x),m,v,HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_IN(x))
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ERROR_BMSK                                        0x3f0
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ERROR_SHFT                                          0x4
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ENABLE_BMSK                                         0x1
#define HWIO_LLCC_BEAC_ADDR_TRANSLATOR_CFG_ENABLE_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * REGIONn_CFG0[0..5] - OFFSET LOW
 *--------------------------------------------------------------------------*/
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_ADDRI(x,n)                                        ((x) + ((n) * 0x10) + 0x00000004)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_RMSK                                           0xffffffff
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_INI(x,n)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_ADDRI(x,n), HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_RMSK)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_INMI(x,n,m)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_ADDRI(x,n), m)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_OUTI(x,n,v)      \
        out_dword(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_ADDRI(x,n),v)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_ADDRI(x,n),m,v,HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_INI(x,n))
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_OFFSET_LOW_BMSK                                0xffffffff
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG0_OFFSET_LOW_SHFT                                       0x0

/*----------------------------------------------------------------------------
 * REGIONn_CFG1[0..5] - OFFSET HIGH
 *--------------------------------------------------------------------------*/
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_ADDRI(x,n)                                        ((x) + ((n) * 0x10) + 0x00000008)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_RMSK                                                  0xf
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_INI(x,n)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_ADDRI(x,n), HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_RMSK)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_INMI(x,n,m)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_ADDRI(x,n), m)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_OUTI(x,n,v)      \
        out_dword(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_ADDRI(x,n),v)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_ADDRI(x,n),m,v,HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_INI(x,n))
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_OFFSET_HIGH_BMSK                                      0xf
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG1_OFFSET_HIGH_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * REGIONn_CFG2[1..5] - ADDR LOW
 *--------------------------------------------------------------------------*/
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_ADDRI(x,n)                                        ((x) + (((n)-1) * 0x10) + 0x0000000c)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_RMSK                                           0xffffffff
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_INI(x,n)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_ADDRI(x,n), HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_RMSK)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_INMI(x,n,m)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_ADDRI(x,n), m)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_OUTI(x,n,v)      \
        out_dword(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_ADDRI(x,n),v)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_ADDRI(x,n),m,v,HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_INI(x,n))
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_BASE_LOW_BMSK                                  0xffffffff
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG2_BASE_LOW_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * REGIONn_CFG3[1..5] - ADDR LOW
 *--------------------------------------------------------------------------*/
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_ADDRI(x,n)                                        ((x) + (((n)-1) * 0x10) + 0x00000010)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_RMSK                                                  0xf
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_INI(x,n)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_ADDRI(x,n), HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_RMSK)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_INMI(x,n,m)      \
        in_dword_masked(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_ADDRI(x,n), m)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_OUTI(x,n,v)      \
        out_dword(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_ADDRI(x,n),v)
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_OUTMI(x,n,m,v) \
        out_dword_masked_ns(HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_ADDRI(x,n),m,v,HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_INI(x,n))
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_BASE_HIGH_BMSK                                        0xf
#define HWIO_LLCC_BEAC_ADDR_REGIONn_CFG3_BASE_HIGH_SHFT                                        0x0

#endif /* __ICBCFG_HWIO_H__ */
