#ifndef __HWIO_QDSS_TPDM_H__
#define __HWIO_QDSS_TPDM_H__

/*
===========================================================================
*/
/**
  @file hwio_qdss_tpdm.h
  @brief Auto-generated HWIO interface include file.

 
  This file contains HWIO register definitions for the following modules:
    QDSS_TPDM

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

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tpdm/hal/src/hwio_qdss_tpdm.h#1 $
  ===========================================================================
*/



#define QDSS_TPDM_BASE (QDSS_TPDM_BLOCK_BASE)
#define QDSS_TPDM_BASE_PHYS (QDSS_TPDM_BLOCK_BASE)
#define QDSS_TPDM_BASE_OFFS 0x0

#define HWIO_TPDM_GPRn_ADDR(n)                                               (QDSS_TPDM_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_TPDM_GPRn_PHYS(n)                                               (QDSS_TPDM_BASE_PHYS + 0x00000000 + 0x4 * (n))
#define HWIO_TPDM_GPRn_OFFS(n)                                               (QDSS_TPDM_BASE_OFFS + 0x00000000 + 0x4 * (n))
#define HWIO_TPDM_GPRn_RMSK                                                  0xffffffff
#define HWIO_TPDM_GPRn_MAXn                                                  159
#define HWIO_TPDM_GPRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_GPRn_ADDR(n), HWIO_TPDM_GPRn_RMSK)
#define HWIO_TPDM_GPRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_GPRn_ADDR(n), mask)
#define HWIO_TPDM_GPRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_GPRn_ADDR(n),val)
#define HWIO_TPDM_GPRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_GPRn_ADDR(n),mask,val,HWIO_TPDM_GPRn_INI(n))
#define HWIO_TPDM_GPRn_VAL_BMSK                                              0xffffffff
#define HWIO_TPDM_GPRn_VAL_SHFT                                                     0x0

#define HWIO_TPDM_BC_CR_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000280)
#define HWIO_TPDM_BC_CR_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000280)
#define HWIO_TPDM_BC_CR_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000280)
#define HWIO_TPDM_BC_CR_RMSK                                                        0xf
#define HWIO_TPDM_BC_CR_IN          \
        in_dword_masked(HWIO_TPDM_BC_CR_ADDR, HWIO_TPDM_BC_CR_RMSK)
#define HWIO_TPDM_BC_CR_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_CR_ADDR, m)
#define HWIO_TPDM_BC_CR_OUT(v)      \
        out_dword(HWIO_TPDM_BC_CR_ADDR,v)
#define HWIO_TPDM_BC_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_CR_ADDR,m,v,HWIO_TPDM_BC_CR_IN)
#define HWIO_TPDM_BC_CR_CAPTURE_BMSK                                                0x8
#define HWIO_TPDM_BC_CR_CAPTURE_SHFT                                                0x3
#define HWIO_TPDM_BC_CR_RETRIEVAL_MODE_BMSK                                         0x4
#define HWIO_TPDM_BC_CR_RETRIEVAL_MODE_SHFT                                         0x2
#define HWIO_TPDM_BC_CR_P_BMSK                                                      0x2
#define HWIO_TPDM_BC_CR_P_SHFT                                                      0x1
#define HWIO_TPDM_BC_CR_E_BMSK                                                      0x1
#define HWIO_TPDM_BC_CR_E_SHFT                                                      0x0

#define HWIO_TPDM_BC_SATROLL_ADDR                                            (QDSS_TPDM_BASE      + 0x00000284)
#define HWIO_TPDM_BC_SATROLL_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x00000284)
#define HWIO_TPDM_BC_SATROLL_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x00000284)
#define HWIO_TPDM_BC_SATROLL_RMSK                                                  0xff
#define HWIO_TPDM_BC_SATROLL_IN          \
        in_dword_masked(HWIO_TPDM_BC_SATROLL_ADDR, HWIO_TPDM_BC_SATROLL_RMSK)
#define HWIO_TPDM_BC_SATROLL_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_SATROLL_ADDR, m)
#define HWIO_TPDM_BC_SATROLL_OUT(v)      \
        out_dword(HWIO_TPDM_BC_SATROLL_ADDR,v)
#define HWIO_TPDM_BC_SATROLL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_SATROLL_ADDR,m,v,HWIO_TPDM_BC_SATROLL_IN)
#define HWIO_TPDM_BC_SATROLL_SO_BMSK                                               0xff
#define HWIO_TPDM_BC_SATROLL_SO_SHFT                                                0x0

#define HWIO_TPDM_BC_CNTENSET_ADDR                                           (QDSS_TPDM_BASE      + 0x00000288)
#define HWIO_TPDM_BC_CNTENSET_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000288)
#define HWIO_TPDM_BC_CNTENSET_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000288)
#define HWIO_TPDM_BC_CNTENSET_RMSK                                                 0xff
#define HWIO_TPDM_BC_CNTENSET_IN          \
        in_dword_masked(HWIO_TPDM_BC_CNTENSET_ADDR, HWIO_TPDM_BC_CNTENSET_RMSK)
#define HWIO_TPDM_BC_CNTENSET_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_CNTENSET_ADDR, m)
#define HWIO_TPDM_BC_CNTENSET_OUT(v)      \
        out_dword(HWIO_TPDM_BC_CNTENSET_ADDR,v)
#define HWIO_TPDM_BC_CNTENSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_CNTENSET_ADDR,m,v,HWIO_TPDM_BC_CNTENSET_IN)
#define HWIO_TPDM_BC_CNTENSET_P_BMSK                                               0xff
#define HWIO_TPDM_BC_CNTENSET_P_SHFT                                                0x0

#define HWIO_TPDM_BC_CNTENCLR_ADDR                                           (QDSS_TPDM_BASE      + 0x0000028c)
#define HWIO_TPDM_BC_CNTENCLR_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x0000028c)
#define HWIO_TPDM_BC_CNTENCLR_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x0000028c)
#define HWIO_TPDM_BC_CNTENCLR_RMSK                                                 0xff
#define HWIO_TPDM_BC_CNTENCLR_IN          \
        in_dword_masked(HWIO_TPDM_BC_CNTENCLR_ADDR, HWIO_TPDM_BC_CNTENCLR_RMSK)
#define HWIO_TPDM_BC_CNTENCLR_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_CNTENCLR_ADDR, m)
#define HWIO_TPDM_BC_CNTENCLR_OUT(v)      \
        out_dword(HWIO_TPDM_BC_CNTENCLR_ADDR,v)
#define HWIO_TPDM_BC_CNTENCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_CNTENCLR_ADDR,m,v,HWIO_TPDM_BC_CNTENCLR_IN)
#define HWIO_TPDM_BC_CNTENCLR_P_BMSK                                               0xff
#define HWIO_TPDM_BC_CNTENCLR_P_SHFT                                                0x0

#define HWIO_TPDM_BC_INTENSET_ADDR                                           (QDSS_TPDM_BASE      + 0x00000290)
#define HWIO_TPDM_BC_INTENSET_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000290)
#define HWIO_TPDM_BC_INTENSET_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000290)
#define HWIO_TPDM_BC_INTENSET_RMSK                                                 0xff
#define HWIO_TPDM_BC_INTENSET_IN          \
        in_dword_masked(HWIO_TPDM_BC_INTENSET_ADDR, HWIO_TPDM_BC_INTENSET_RMSK)
#define HWIO_TPDM_BC_INTENSET_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_INTENSET_ADDR, m)
#define HWIO_TPDM_BC_INTENSET_OUT(v)      \
        out_dword(HWIO_TPDM_BC_INTENSET_ADDR,v)
#define HWIO_TPDM_BC_INTENSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_INTENSET_ADDR,m,v,HWIO_TPDM_BC_INTENSET_IN)
#define HWIO_TPDM_BC_INTENSET_P_BMSK                                               0xff
#define HWIO_TPDM_BC_INTENSET_P_SHFT                                                0x0

#define HWIO_TPDM_BC_INTENCLR_ADDR                                           (QDSS_TPDM_BASE      + 0x00000294)
#define HWIO_TPDM_BC_INTENCLR_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000294)
#define HWIO_TPDM_BC_INTENCLR_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000294)
#define HWIO_TPDM_BC_INTENCLR_RMSK                                                 0xff
#define HWIO_TPDM_BC_INTENCLR_IN          \
        in_dword_masked(HWIO_TPDM_BC_INTENCLR_ADDR, HWIO_TPDM_BC_INTENCLR_RMSK)
#define HWIO_TPDM_BC_INTENCLR_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_INTENCLR_ADDR, m)
#define HWIO_TPDM_BC_INTENCLR_OUT(v)      \
        out_dword(HWIO_TPDM_BC_INTENCLR_ADDR,v)
#define HWIO_TPDM_BC_INTENCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_INTENCLR_ADDR,m,v,HWIO_TPDM_BC_INTENCLR_IN)
#define HWIO_TPDM_BC_INTENCLR_P_BMSK                                               0xff
#define HWIO_TPDM_BC_INTENCLR_P_SHFT                                                0x0

#define HWIO_TPDM_BC_TRIGn_LO_ADDR(n)                                        (QDSS_TPDM_BASE      + 0x00000298 + 0x4 * (n))
#define HWIO_TPDM_BC_TRIGn_LO_PHYS(n)                                        (QDSS_TPDM_BASE_PHYS + 0x00000298 + 0x4 * (n))
#define HWIO_TPDM_BC_TRIGn_LO_OFFS(n)                                        (QDSS_TPDM_BASE_OFFS + 0x00000298 + 0x4 * (n))
#define HWIO_TPDM_BC_TRIGn_LO_RMSK                                           0xffffffff
#define HWIO_TPDM_BC_TRIGn_LO_MAXn                                                    7
#define HWIO_TPDM_BC_TRIGn_LO_INI(n)        \
        in_dword_masked(HWIO_TPDM_BC_TRIGn_LO_ADDR(n), HWIO_TPDM_BC_TRIGn_LO_RMSK)
#define HWIO_TPDM_BC_TRIGn_LO_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_BC_TRIGn_LO_ADDR(n), mask)
#define HWIO_TPDM_BC_TRIGn_LO_OUTI(n,val)    \
        out_dword(HWIO_TPDM_BC_TRIGn_LO_ADDR(n),val)
#define HWIO_TPDM_BC_TRIGn_LO_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_BC_TRIGn_LO_ADDR(n),mask,val,HWIO_TPDM_BC_TRIGn_LO_INI(n))
#define HWIO_TPDM_BC_TRIGn_LO_VAL_BMSK                                       0xffffffff
#define HWIO_TPDM_BC_TRIGn_LO_VAL_SHFT                                              0x0

#define HWIO_TPDM_BC_OVSR_ADDR                                               (QDSS_TPDM_BASE      + 0x000003c0)
#define HWIO_TPDM_BC_OVSR_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x000003c0)
#define HWIO_TPDM_BC_OVSR_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x000003c0)
#define HWIO_TPDM_BC_OVSR_RMSK                                                     0xff
#define HWIO_TPDM_BC_OVSR_IN          \
        in_dword_masked(HWIO_TPDM_BC_OVSR_ADDR, HWIO_TPDM_BC_OVSR_RMSK)
#define HWIO_TPDM_BC_OVSR_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_OVSR_ADDR, m)
#define HWIO_TPDM_BC_OVSR_OUT(v)      \
        out_dword(HWIO_TPDM_BC_OVSR_ADDR,v)
#define HWIO_TPDM_BC_OVSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_OVSR_ADDR,m,v,HWIO_TPDM_BC_OVSR_IN)
#define HWIO_TPDM_BC_OVSR_P_BMSK                                                   0xff
#define HWIO_TPDM_BC_OVSR_P_SHFT                                                    0x0

#define HWIO_TPDM_BC_SELR_ADDR                                               (QDSS_TPDM_BASE      + 0x000003c4)
#define HWIO_TPDM_BC_SELR_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x000003c4)
#define HWIO_TPDM_BC_SELR_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x000003c4)
#define HWIO_TPDM_BC_SELR_RMSK                                                     0x1f
#define HWIO_TPDM_BC_SELR_IN          \
        in_dword_masked(HWIO_TPDM_BC_SELR_ADDR, HWIO_TPDM_BC_SELR_RMSK)
#define HWIO_TPDM_BC_SELR_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_SELR_ADDR, m)
#define HWIO_TPDM_BC_SELR_OUT(v)      \
        out_dword(HWIO_TPDM_BC_SELR_ADDR,v)
#define HWIO_TPDM_BC_SELR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_SELR_ADDR,m,v,HWIO_TPDM_BC_SELR_IN)
#define HWIO_TPDM_BC_SELR_SEL_BMSK                                                 0x1f
#define HWIO_TPDM_BC_SELR_SEL_SHFT                                                  0x0

#define HWIO_TPDM_BC_CNTR_LO_ADDR                                            (QDSS_TPDM_BASE      + 0x000003c8)
#define HWIO_TPDM_BC_CNTR_LO_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x000003c8)
#define HWIO_TPDM_BC_CNTR_LO_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x000003c8)
#define HWIO_TPDM_BC_CNTR_LO_RMSK                                                  0xff
#define HWIO_TPDM_BC_CNTR_LO_IN          \
        in_dword_masked(HWIO_TPDM_BC_CNTR_LO_ADDR, HWIO_TPDM_BC_CNTR_LO_RMSK)
#define HWIO_TPDM_BC_CNTR_LO_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_CNTR_LO_ADDR, m)
#define HWIO_TPDM_BC_CNTR_LO_OUT(v)      \
        out_dword(HWIO_TPDM_BC_CNTR_LO_ADDR,v)
#define HWIO_TPDM_BC_CNTR_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_CNTR_LO_ADDR,m,v,HWIO_TPDM_BC_CNTR_LO_IN)
#define HWIO_TPDM_BC_CNTR_LO_PMN_BMSK                                              0xff
#define HWIO_TPDM_BC_CNTR_LO_PMN_SHFT                                               0x0

#define HWIO_TPDM_BC_SHADOWn_LO_ADDR(n)                                      (QDSS_TPDM_BASE      + 0x000003d0 + 0x4 * (n))
#define HWIO_TPDM_BC_SHADOWn_LO_PHYS(n)                                      (QDSS_TPDM_BASE_PHYS + 0x000003d0 + 0x4 * (n))
#define HWIO_TPDM_BC_SHADOWn_LO_OFFS(n)                                      (QDSS_TPDM_BASE_OFFS + 0x000003d0 + 0x4 * (n))
#define HWIO_TPDM_BC_SHADOWn_LO_RMSK                                         0xffffffff
#define HWIO_TPDM_BC_SHADOWn_LO_MAXn                                                  7
#define HWIO_TPDM_BC_SHADOWn_LO_INI(n)        \
        in_dword_masked(HWIO_TPDM_BC_SHADOWn_LO_ADDR(n), HWIO_TPDM_BC_SHADOWn_LO_RMSK)
#define HWIO_TPDM_BC_SHADOWn_LO_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_BC_SHADOWn_LO_ADDR(n), mask)
#define HWIO_TPDM_BC_SHADOWn_LO_VAL_BMSK                                     0xffffffff
#define HWIO_TPDM_BC_SHADOWn_LO_VAL_SHFT                                            0x0

#define HWIO_TPDM_BC_SWINC_ADDR                                              (QDSS_TPDM_BASE      + 0x000004d0)
#define HWIO_TPDM_BC_SWINC_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x000004d0)
#define HWIO_TPDM_BC_SWINC_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x000004d0)
#define HWIO_TPDM_BC_SWINC_RMSK                                                    0xff
#define HWIO_TPDM_BC_SWINC_IN          \
        in_dword_masked(HWIO_TPDM_BC_SWINC_ADDR, HWIO_TPDM_BC_SWINC_RMSK)
#define HWIO_TPDM_BC_SWINC_INM(m)      \
        in_dword_masked(HWIO_TPDM_BC_SWINC_ADDR, m)
#define HWIO_TPDM_BC_SWINC_OUT(v)      \
        out_dword(HWIO_TPDM_BC_SWINC_ADDR,v)
#define HWIO_TPDM_BC_SWINC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_BC_SWINC_ADDR,m,v,HWIO_TPDM_BC_SWINC_IN)
#define HWIO_TPDM_BC_SWINC_P_BMSK                                                  0xff
#define HWIO_TPDM_BC_SWINC_P_SHFT                                                   0x0

#define HWIO_TPDM_TC_CR_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000500)
#define HWIO_TPDM_TC_CR_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000500)
#define HWIO_TPDM_TC_CR_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000500)
#define HWIO_TPDM_TC_CR_RMSK                                                       0x1f
#define HWIO_TPDM_TC_CR_IN          \
        in_dword_masked(HWIO_TPDM_TC_CR_ADDR, HWIO_TPDM_TC_CR_RMSK)
#define HWIO_TPDM_TC_CR_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_CR_ADDR, m)
#define HWIO_TPDM_TC_CR_OUT(v)      \
        out_dword(HWIO_TPDM_TC_CR_ADDR,v)
#define HWIO_TPDM_TC_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_CR_ADDR,m,v,HWIO_TPDM_TC_CR_IN)
#define HWIO_TPDM_TC_CR_SO_BMSK                                                    0x10
#define HWIO_TPDM_TC_CR_SO_SHFT                                                     0x4
#define HWIO_TPDM_TC_CR_CAPTURE_BMSK                                                0x8
#define HWIO_TPDM_TC_CR_CAPTURE_SHFT                                                0x3
#define HWIO_TPDM_TC_CR_RETRIEVAL_MODE_BMSK                                         0x4
#define HWIO_TPDM_TC_CR_RETRIEVAL_MODE_SHFT                                         0x2
#define HWIO_TPDM_TC_CR_P_BMSK                                                      0x2
#define HWIO_TPDM_TC_CR_P_SHFT                                                      0x1
#define HWIO_TPDM_TC_CR_E_BMSK                                                      0x1
#define HWIO_TPDM_TC_CR_E_SHFT                                                      0x0

#define HWIO_TPDM_TC_CNTENSET_ADDR                                           (QDSS_TPDM_BASE      + 0x00000504)
#define HWIO_TPDM_TC_CNTENSET_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000504)
#define HWIO_TPDM_TC_CNTENSET_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000504)
#define HWIO_TPDM_TC_CNTENSET_RMSK                                                  0x3
#define HWIO_TPDM_TC_CNTENSET_IN          \
        in_dword_masked(HWIO_TPDM_TC_CNTENSET_ADDR, HWIO_TPDM_TC_CNTENSET_RMSK)
#define HWIO_TPDM_TC_CNTENSET_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_CNTENSET_ADDR, m)
#define HWIO_TPDM_TC_CNTENSET_OUT(v)      \
        out_dword(HWIO_TPDM_TC_CNTENSET_ADDR,v)
#define HWIO_TPDM_TC_CNTENSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_CNTENSET_ADDR,m,v,HWIO_TPDM_TC_CNTENSET_IN)
#define HWIO_TPDM_TC_CNTENSET_P_BMSK                                                0x3
#define HWIO_TPDM_TC_CNTENSET_P_SHFT                                                0x0

#define HWIO_TPDM_TC_CNTENCLR_ADDR                                           (QDSS_TPDM_BASE      + 0x00000508)
#define HWIO_TPDM_TC_CNTENCLR_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000508)
#define HWIO_TPDM_TC_CNTENCLR_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000508)
#define HWIO_TPDM_TC_CNTENCLR_RMSK                                                  0x3
#define HWIO_TPDM_TC_CNTENCLR_IN          \
        in_dword_masked(HWIO_TPDM_TC_CNTENCLR_ADDR, HWIO_TPDM_TC_CNTENCLR_RMSK)
#define HWIO_TPDM_TC_CNTENCLR_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_CNTENCLR_ADDR, m)
#define HWIO_TPDM_TC_CNTENCLR_OUT(v)      \
        out_dword(HWIO_TPDM_TC_CNTENCLR_ADDR,v)
#define HWIO_TPDM_TC_CNTENCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_CNTENCLR_ADDR,m,v,HWIO_TPDM_TC_CNTENCLR_IN)
#define HWIO_TPDM_TC_CNTENCLR_P_BMSK                                                0x3
#define HWIO_TPDM_TC_CNTENCLR_P_SHFT                                                0x0

#define HWIO_TPDM_TC_INTENSET_ADDR                                           (QDSS_TPDM_BASE      + 0x0000050c)
#define HWIO_TPDM_TC_INTENSET_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x0000050c)
#define HWIO_TPDM_TC_INTENSET_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x0000050c)
#define HWIO_TPDM_TC_INTENSET_RMSK                                                 0x13
#define HWIO_TPDM_TC_INTENSET_IN          \
        in_dword_masked(HWIO_TPDM_TC_INTENSET_ADDR, HWIO_TPDM_TC_INTENSET_RMSK)
#define HWIO_TPDM_TC_INTENSET_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_INTENSET_ADDR, m)
#define HWIO_TPDM_TC_INTENSET_OUT(v)      \
        out_dword(HWIO_TPDM_TC_INTENSET_ADDR,v)
#define HWIO_TPDM_TC_INTENSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_INTENSET_ADDR,m,v,HWIO_TPDM_TC_INTENSET_IN)
#define HWIO_TPDM_TC_INTENSET_PSP_BMSK                                             0x10
#define HWIO_TPDM_TC_INTENSET_PSP_SHFT                                              0x4
#define HWIO_TPDM_TC_INTENSET_PTC_BMSK                                              0x3
#define HWIO_TPDM_TC_INTENSET_PTC_SHFT                                              0x0

#define HWIO_TPDM_TC_INTENCLR_ADDR                                           (QDSS_TPDM_BASE      + 0x00000510)
#define HWIO_TPDM_TC_INTENCLR_PHYS                                           (QDSS_TPDM_BASE_PHYS + 0x00000510)
#define HWIO_TPDM_TC_INTENCLR_OFFS                                           (QDSS_TPDM_BASE_OFFS + 0x00000510)
#define HWIO_TPDM_TC_INTENCLR_RMSK                                                 0x13
#define HWIO_TPDM_TC_INTENCLR_IN          \
        in_dword_masked(HWIO_TPDM_TC_INTENCLR_ADDR, HWIO_TPDM_TC_INTENCLR_RMSK)
#define HWIO_TPDM_TC_INTENCLR_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_INTENCLR_ADDR, m)
#define HWIO_TPDM_TC_INTENCLR_OUT(v)      \
        out_dword(HWIO_TPDM_TC_INTENCLR_ADDR,v)
#define HWIO_TPDM_TC_INTENCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_INTENCLR_ADDR,m,v,HWIO_TPDM_TC_INTENCLR_IN)
#define HWIO_TPDM_TC_INTENCLR_PSP_BMSK                                             0x10
#define HWIO_TPDM_TC_INTENCLR_PSP_SHFT                                              0x4
#define HWIO_TPDM_TC_INTENCLR_PTC_BMSK                                              0x3
#define HWIO_TPDM_TC_INTENCLR_PTC_SHFT                                              0x0

#define HWIO_TPDM_TC_TRIGSELn_LO_ADDR(n)                                     (QDSS_TPDM_BASE      + 0x00000514 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGSELn_LO_PHYS(n)                                     (QDSS_TPDM_BASE_PHYS + 0x00000514 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGSELn_LO_OFFS(n)                                     (QDSS_TPDM_BASE_OFFS + 0x00000514 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGSELn_LO_RMSK                                              0x3f
#define HWIO_TPDM_TC_TRIGSELn_LO_MAXn                                                 7
#define HWIO_TPDM_TC_TRIGSELn_LO_INI(n)        \
        in_dword_masked(HWIO_TPDM_TC_TRIGSELn_LO_ADDR(n), HWIO_TPDM_TC_TRIGSELn_LO_RMSK)
#define HWIO_TPDM_TC_TRIGSELn_LO_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_TC_TRIGSELn_LO_ADDR(n), mask)
#define HWIO_TPDM_TC_TRIGSELn_LO_OUTI(n,val)    \
        out_dword(HWIO_TPDM_TC_TRIGSELn_LO_ADDR(n),val)
#define HWIO_TPDM_TC_TRIGSELn_LO_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_TC_TRIGSELn_LO_ADDR(n),mask,val,HWIO_TPDM_TC_TRIGSELn_LO_INI(n))
#define HWIO_TPDM_TC_TRIGSELn_LO_ELMNTSEL_BMSK                                     0x3c
#define HWIO_TPDM_TC_TRIGSELn_LO_ELMNTSEL_SHFT                                      0x2
#define HWIO_TPDM_TC_TRIGSELn_LO_TCSEL_BMSK                                         0x3
#define HWIO_TPDM_TC_TRIGSELn_LO_TCSEL_SHFT                                         0x0

#define HWIO_TPDM_TC_TRIGn_LO_ADDR(n)                                        (QDSS_TPDM_BASE      + 0x00000534 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGn_LO_PHYS(n)                                        (QDSS_TPDM_BASE_PHYS + 0x00000534 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGn_LO_OFFS(n)                                        (QDSS_TPDM_BASE_OFFS + 0x00000534 + 0x4 * (n))
#define HWIO_TPDM_TC_TRIGn_LO_RMSK                                               0x3fff
#define HWIO_TPDM_TC_TRIGn_LO_MAXn                                                    7
#define HWIO_TPDM_TC_TRIGn_LO_INI(n)        \
        in_dword_masked(HWIO_TPDM_TC_TRIGn_LO_ADDR(n), HWIO_TPDM_TC_TRIGn_LO_RMSK)
#define HWIO_TPDM_TC_TRIGn_LO_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_TC_TRIGn_LO_ADDR(n), mask)
#define HWIO_TPDM_TC_TRIGn_LO_OUTI(n,val)    \
        out_dword(HWIO_TPDM_TC_TRIGn_LO_ADDR(n),val)
#define HWIO_TPDM_TC_TRIGn_LO_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_TC_TRIGn_LO_ADDR(n),mask,val,HWIO_TPDM_TC_TRIGn_LO_INI(n))
#define HWIO_TPDM_TC_TRIGn_LO_VAL_BMSK                                           0x3fff
#define HWIO_TPDM_TC_TRIGn_LO_VAL_SHFT                                              0x0

#define HWIO_TPDM_TC_OVSR_GP_ADDR                                            (QDSS_TPDM_BASE      + 0x00000580)
#define HWIO_TPDM_TC_OVSR_GP_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x00000580)
#define HWIO_TPDM_TC_OVSR_GP_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x00000580)
#define HWIO_TPDM_TC_OVSR_GP_RMSK                                                0xffff
#define HWIO_TPDM_TC_OVSR_GP_IN          \
        in_dword_masked(HWIO_TPDM_TC_OVSR_GP_ADDR, HWIO_TPDM_TC_OVSR_GP_RMSK)
#define HWIO_TPDM_TC_OVSR_GP_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_OVSR_GP_ADDR, m)
#define HWIO_TPDM_TC_OVSR_GP_OUT(v)      \
        out_dword(HWIO_TPDM_TC_OVSR_GP_ADDR,v)
#define HWIO_TPDM_TC_OVSR_GP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_OVSR_GP_ADDR,m,v,HWIO_TPDM_TC_OVSR_GP_IN)
#define HWIO_TPDM_TC_OVSR_GP_P1_BMSK                                             0xff00
#define HWIO_TPDM_TC_OVSR_GP_P1_SHFT                                                0x8
#define HWIO_TPDM_TC_OVSR_GP_P0_BMSK                                               0xff
#define HWIO_TPDM_TC_OVSR_GP_P0_SHFT                                                0x0

#define HWIO_TPDM_TC_OVSR_IMPL_ADDR                                          (QDSS_TPDM_BASE      + 0x00000584)
#define HWIO_TPDM_TC_OVSR_IMPL_PHYS                                          (QDSS_TPDM_BASE_PHYS + 0x00000584)
#define HWIO_TPDM_TC_OVSR_IMPL_OFFS                                          (QDSS_TPDM_BASE_OFFS + 0x00000584)
#define HWIO_TPDM_TC_OVSR_IMPL_RMSK                                          0x80000404
#define HWIO_TPDM_TC_OVSR_IMPL_IN          \
        in_dword_masked(HWIO_TPDM_TC_OVSR_IMPL_ADDR, HWIO_TPDM_TC_OVSR_IMPL_RMSK)
#define HWIO_TPDM_TC_OVSR_IMPL_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_OVSR_IMPL_ADDR, m)
#define HWIO_TPDM_TC_OVSR_IMPL_OUT(v)      \
        out_dword(HWIO_TPDM_TC_OVSR_IMPL_ADDR,v)
#define HWIO_TPDM_TC_OVSR_IMPL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_OVSR_IMPL_ADDR,m,v,HWIO_TPDM_TC_OVSR_IMPL_IN)
#define HWIO_TPDM_TC_OVSR_IMPL_P_SP_BMSK                                     0x80000000
#define HWIO_TPDM_TC_OVSR_IMPL_P_SP_SHFT                                           0x1f
#define HWIO_TPDM_TC_OVSR_IMPL_P1_XTRA_GP2_TAT_BMSK                               0x400
#define HWIO_TPDM_TC_OVSR_IMPL_P1_XTRA_GP2_TAT_SHFT                                 0xa
#define HWIO_TPDM_TC_OVSR_IMPL_P0_XTRA_GP2_TAT_BMSK                                 0x4
#define HWIO_TPDM_TC_OVSR_IMPL_P0_XTRA_GP2_TAT_SHFT                                 0x2

#define HWIO_TPDM_TC_SELR_ADDR                                               (QDSS_TPDM_BASE      + 0x00000588)
#define HWIO_TPDM_TC_SELR_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x00000588)
#define HWIO_TPDM_TC_SELR_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x00000588)
#define HWIO_TPDM_TC_SELR_RMSK                                                  0x1ffff
#define HWIO_TPDM_TC_SELR_IN          \
        in_dword_masked(HWIO_TPDM_TC_SELR_ADDR, HWIO_TPDM_TC_SELR_RMSK)
#define HWIO_TPDM_TC_SELR_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_SELR_ADDR, m)
#define HWIO_TPDM_TC_SELR_OUT(v)      \
        out_dword(HWIO_TPDM_TC_SELR_ADDR,v)
#define HWIO_TPDM_TC_SELR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_SELR_ADDR,m,v,HWIO_TPDM_TC_SELR_IN)
#define HWIO_TPDM_TC_SELR_GPSEL_BMSK                                            0x1e000
#define HWIO_TPDM_TC_SELR_GPSEL_SHFT                                                0xd
#define HWIO_TPDM_TC_SELR_TCSEL_BMSK                                             0x1800
#define HWIO_TPDM_TC_SELR_TCSEL_SHFT                                                0xb
#define HWIO_TPDM_TC_SELR_SPSEL_BMSK                                              0x7fe
#define HWIO_TPDM_TC_SELR_SPSEL_SHFT                                                0x1
#define HWIO_TPDM_TC_SELR_SEL_BMSK                                                  0x1
#define HWIO_TPDM_TC_SELR_SEL_SHFT                                                  0x0

#define HWIO_TPDM_TC_CNTR_LO_ADDR                                            (QDSS_TPDM_BASE      + 0x0000058c)
#define HWIO_TPDM_TC_CNTR_LO_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x0000058c)
#define HWIO_TPDM_TC_CNTR_LO_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x0000058c)
#define HWIO_TPDM_TC_CNTR_LO_RMSK                                                0x3fff
#define HWIO_TPDM_TC_CNTR_LO_IN          \
        in_dword_masked(HWIO_TPDM_TC_CNTR_LO_ADDR, HWIO_TPDM_TC_CNTR_LO_RMSK)
#define HWIO_TPDM_TC_CNTR_LO_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_CNTR_LO_ADDR, m)
#define HWIO_TPDM_TC_CNTR_LO_OUT(v)      \
        out_dword(HWIO_TPDM_TC_CNTR_LO_ADDR,v)
#define HWIO_TPDM_TC_CNTR_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_CNTR_LO_ADDR,m,v,HWIO_TPDM_TC_CNTR_LO_IN)
#define HWIO_TPDM_TC_CNTR_LO_PMN_BMSK                                            0x3fff
#define HWIO_TPDM_TC_CNTR_LO_PMN_SHFT                                               0x0

#define HWIO_TPDM_TC_SHADOWn_LO_ADDR(n)                                      (QDSS_TPDM_BASE      + 0x00000594 + 0x4 * (n))
#define HWIO_TPDM_TC_SHADOWn_LO_PHYS(n)                                      (QDSS_TPDM_BASE_PHYS + 0x00000594 + 0x4 * (n))
#define HWIO_TPDM_TC_SHADOWn_LO_OFFS(n)                                      (QDSS_TPDM_BASE_OFFS + 0x00000594 + 0x4 * (n))
#define HWIO_TPDM_TC_SHADOWn_LO_RMSK                                             0x3fff
#define HWIO_TPDM_TC_SHADOWn_LO_MAXn                                                 21
#define HWIO_TPDM_TC_SHADOWn_LO_INI(n)        \
        in_dword_masked(HWIO_TPDM_TC_SHADOWn_LO_ADDR(n), HWIO_TPDM_TC_SHADOWn_LO_RMSK)
#define HWIO_TPDM_TC_SHADOWn_LO_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_TC_SHADOWn_LO_ADDR(n), mask)
#define HWIO_TPDM_TC_SHADOWn_LO_VAL_BMSK                                         0x3fff
#define HWIO_TPDM_TC_SHADOWn_LO_VAL_SHFT                                            0x0

#define HWIO_TPDM_TC_SWINC_ADDR                                              (QDSS_TPDM_BASE      + 0x00000700)
#define HWIO_TPDM_TC_SWINC_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x00000700)
#define HWIO_TPDM_TC_SWINC_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x00000700)
#define HWIO_TPDM_TC_SWINC_RMSK                                                     0x3
#define HWIO_TPDM_TC_SWINC_IN          \
        in_dword_masked(HWIO_TPDM_TC_SWINC_ADDR, HWIO_TPDM_TC_SWINC_RMSK)
#define HWIO_TPDM_TC_SWINC_INM(m)      \
        in_dword_masked(HWIO_TPDM_TC_SWINC_ADDR, m)
#define HWIO_TPDM_TC_SWINC_OUT(v)      \
        out_dword(HWIO_TPDM_TC_SWINC_ADDR,v)
#define HWIO_TPDM_TC_SWINC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_TC_SWINC_ADDR,m,v,HWIO_TPDM_TC_SWINC_IN)
#define HWIO_TPDM_TC_SWINC_P_BMSK                                                   0x3
#define HWIO_TPDM_TC_SWINC_P_SHFT                                                   0x0

#define HWIO_TPDM_DSB_CR_ADDR                                                (QDSS_TPDM_BASE      + 0x00000780)
#define HWIO_TPDM_DSB_CR_PHYS                                                (QDSS_TPDM_BASE_PHYS + 0x00000780)
#define HWIO_TPDM_DSB_CR_OFFS                                                (QDSS_TPDM_BASE_OFFS + 0x00000780)
#define HWIO_TPDM_DSB_CR_RMSK                                                     0x1ff
#define HWIO_TPDM_DSB_CR_IN          \
        in_dword_masked(HWIO_TPDM_DSB_CR_ADDR, HWIO_TPDM_DSB_CR_RMSK)
#define HWIO_TPDM_DSB_CR_INM(m)      \
        in_dword_masked(HWIO_TPDM_DSB_CR_ADDR, m)
#define HWIO_TPDM_DSB_CR_OUT(v)      \
        out_dword(HWIO_TPDM_DSB_CR_ADDR,v)
#define HWIO_TPDM_DSB_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_DSB_CR_ADDR,m,v,HWIO_TPDM_DSB_CR_IN)
#define HWIO_TPDM_DSB_CR_ATBFLOWERR_BMSK                                          0x100
#define HWIO_TPDM_DSB_CR_ATBFLOWERR_SHFT                                            0x8
#define HWIO_TPDM_DSB_CR_EBITSET_BMSK                                              0x80
#define HWIO_TPDM_DSB_CR_EBITSET_SHFT                                               0x7
#define HWIO_TPDM_DSB_CR_HPSEL_BMSK                                                0x7c
#define HWIO_TPDM_DSB_CR_HPSEL_SHFT                                                 0x2
#define HWIO_TPDM_DSB_CR_MODE_BMSK                                                  0x2
#define HWIO_TPDM_DSB_CR_MODE_SHFT                                                  0x1
#define HWIO_TPDM_DSB_CR_E_BMSK                                                     0x1
#define HWIO_TPDM_DSB_CR_E_SHFT                                                     0x0

#define HWIO_TPDM_DSB_TIER_ADDR                                              (QDSS_TPDM_BASE      + 0x00000784)
#define HWIO_TPDM_DSB_TIER_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x00000784)
#define HWIO_TPDM_DSB_TIER_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x00000784)
#define HWIO_TPDM_DSB_TIER_RMSK                                                     0x3
#define HWIO_TPDM_DSB_TIER_IN          \
        in_dword_masked(HWIO_TPDM_DSB_TIER_ADDR, HWIO_TPDM_DSB_TIER_RMSK)
#define HWIO_TPDM_DSB_TIER_INM(m)      \
        in_dword_masked(HWIO_TPDM_DSB_TIER_ADDR, m)
#define HWIO_TPDM_DSB_TIER_OUT(v)      \
        out_dword(HWIO_TPDM_DSB_TIER_ADDR,v)
#define HWIO_TPDM_DSB_TIER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_DSB_TIER_ADDR,m,v,HWIO_TPDM_DSB_TIER_IN)

#define HWIO_TPDM_DSB_TIER_PATT_TYPE_BMSK                                           0x4
#define HWIO_TPDM_DSB_TIER_PATT_TYPE_SHFT                                           0x2
#define HWIO_TPDM_DSB_TIER_XTRIG_TSENAB_BMSK                                        0x2
#define HWIO_TPDM_DSB_TIER_XTRIG_TSENAB_SHFT                                        0x1
#define HWIO_TPDM_DSB_TIER_PATT_TSENAB_BMSK                                         0x1
#define HWIO_TPDM_DSB_TIER_PATT_TSENAB_SHFT                                         0x0

#define HWIO_TPDM_DSB_TPRn_ADDR(n)                                           (QDSS_TPDM_BASE      + 0x00000788 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPRn_PHYS(n)                                           (QDSS_TPDM_BASE_PHYS + 0x00000788 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPRn_OFFS(n)                                           (QDSS_TPDM_BASE_OFFS + 0x00000788 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPRn_RMSK                                              0xffffffff
#define HWIO_TPDM_DSB_TPRn_MAXn                                                       0
#define HWIO_TPDM_DSB_TPRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_TPRn_ADDR(n), HWIO_TPDM_DSB_TPRn_RMSK)
#define HWIO_TPDM_DSB_TPRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_TPRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_TPRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_TPRn_ADDR(n),val)
#define HWIO_TPDM_DSB_TPRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_TPRn_ADDR(n),mask,val,HWIO_TPDM_DSB_TPRn_INI(n))
#define HWIO_TPDM_DSB_TPRn_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_DSB_TPRn_VAL_SHFT                                                 0x0

#define HWIO_TPDM_DSB_TPRm_ADDR(m)                                           (QDSS_TPDM_BASE      + 0x00000788 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPRm_PHYS(m)                                           (QDSS_TPDM_BASE_PHYS + 0x00000788 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPRm_OFFS(m)                                           (QDSS_TPDM_BASE_OFFS + 0x00000788 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPRm_RMSK                                              0xffffffff
#define HWIO_TPDM_DSB_TPRm_MAXm                                                       1
#define HWIO_TPDM_DSB_TPRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_TPRm_ADDR(m), HWIO_TPDM_DSB_TPRm_RMSK)
#define HWIO_TPDM_DSB_TPRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_TPRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_TPRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_TPRm_ADDR(m),val)
#define HWIO_TPDM_DSB_TPRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_TPRm_ADDR(m),mask,val,HWIO_TPDM_DSB_TPRm_INI(m))
#define HWIO_TPDM_DSB_TPRm_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_DSB_TPRm_VAL_SHFT                                                 0x0

#define HWIO_TPDM_DSB_TPMRn_ADDR(n)                                          (QDSS_TPDM_BASE      + 0x000007a8 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPMRn_PHYS(n)                                          (QDSS_TPDM_BASE_PHYS + 0x000007a8 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPMRn_OFFS(n)                                          (QDSS_TPDM_BASE_OFFS + 0x000007a8 + 0x4 * (n))
#define HWIO_TPDM_DSB_TPMRn_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_TPMRn_MAXn                                                      0
#define HWIO_TPDM_DSB_TPMRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_TPMRn_ADDR(n), HWIO_TPDM_DSB_TPMRn_RMSK)
#define HWIO_TPDM_DSB_TPMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_TPMRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_TPMRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_TPMRn_ADDR(n),val)
#define HWIO_TPDM_DSB_TPMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_TPMRn_ADDR(n),mask,val,HWIO_TPDM_DSB_TPMRn_INI(n))
#define HWIO_TPDM_DSB_TPMRn_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_DSB_TPMRn_VAL_SHFT                                                0x0

#define HWIO_TPDM_DSB_TPMRm_ADDR(m)                                          (QDSS_TPDM_BASE      + 0x000007a8 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPMRm_PHYS(m)                                          (QDSS_TPDM_BASE_PHYS + 0x000007a8 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPMRm_OFFS(m)                                          (QDSS_TPDM_BASE_OFFS + 0x000007a8 + 0x4 * (m))
#define HWIO_TPDM_DSB_TPMRm_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_TPMRm_MAXm                                                      1
#define HWIO_TPDM_DSB_TPMRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_TPMRm_ADDR(m), HWIO_TPDM_DSB_TPMRm_RMSK)
#define HWIO_TPDM_DSB_TPMRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_TPMRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_TPMRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_TPMRm_ADDR(m),val)
#define HWIO_TPDM_DSB_TPMRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_TPMRm_ADDR(m),mask,val,HWIO_TPDM_DSB_TPMRm_INI(m))
#define HWIO_TPDM_DSB_TPMRm_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_DSB_TPMRm_VAL_SHFT                                                0x0

#define HWIO_TPDM_DSB_XPRn_ADDR(n)                                           (QDSS_TPDM_BASE      + 0x000007c8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPRn_PHYS(n)                                           (QDSS_TPDM_BASE_PHYS + 0x000007c8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPRn_OFFS(n)                                           (QDSS_TPDM_BASE_OFFS + 0x000007c8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPRn_RMSK                                              0xffffffff
#define HWIO_TPDM_DSB_XPRn_MAXn                                                       0
#define HWIO_TPDM_DSB_XPRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_XPRn_ADDR(n), HWIO_TPDM_DSB_XPRn_RMSK)
#define HWIO_TPDM_DSB_XPRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_XPRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_XPRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_XPRn_ADDR(n),val)
#define HWIO_TPDM_DSB_XPRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_XPRn_ADDR(n),mask,val,HWIO_TPDM_DSB_XPRn_INI(n))
#define HWIO_TPDM_DSB_XPRn_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_DSB_XPRn_VAL_SHFT                                                 0x0

#define HWIO_TPDM_DSB_XPRm_ADDR(m)                                           (QDSS_TPDM_BASE      + 0x000007c8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPRm_PHYS(m)                                           (QDSS_TPDM_BASE_PHYS + 0x000007c8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPRm_OFFS(m)                                           (QDSS_TPDM_BASE_OFFS + 0x000007c8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPRm_RMSK                                              0xffffffff
#define HWIO_TPDM_DSB_XPRm_MAXm                                                       1
#define HWIO_TPDM_DSB_XPRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_XPRm_ADDR(m), HWIO_TPDM_DSB_XPRm_RMSK)
#define HWIO_TPDM_DSB_XPRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_XPRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_XPRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_XPRm_ADDR(m),val)
#define HWIO_TPDM_DSB_XPRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_XPRm_ADDR(m),mask,val,HWIO_TPDM_DSB_XPRm_INI(m))
#define HWIO_TPDM_DSB_XPRm_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_DSB_XPRm_VAL_SHFT                                                 0x0

#define HWIO_TPDM_DSB_XPMRn_ADDR(n)                                          (QDSS_TPDM_BASE      + 0x000007e8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPMRn_PHYS(n)                                          (QDSS_TPDM_BASE_PHYS + 0x000007e8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPMRn_OFFS(n)                                          (QDSS_TPDM_BASE_OFFS + 0x000007e8 + 0x4 * (n))
#define HWIO_TPDM_DSB_XPMRn_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_XPMRn_MAXn                                                      0
#define HWIO_TPDM_DSB_XPMRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_XPMRn_ADDR(n), HWIO_TPDM_DSB_XPMRn_RMSK)
#define HWIO_TPDM_DSB_XPMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_XPMRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_XPMRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_XPMRn_ADDR(n),val)
#define HWIO_TPDM_DSB_XPMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_XPMRn_ADDR(n),mask,val,HWIO_TPDM_DSB_XPMRn_INI(n))
#define HWIO_TPDM_DSB_XPMRn_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_DSB_XPMRn_VAL_SHFT                                                0x0

#define HWIO_TPDM_DSB_XPMRm_ADDR(m)                                          (QDSS_TPDM_BASE      + 0x000007e8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPMRm_PHYS(m)                                          (QDSS_TPDM_BASE_PHYS + 0x000007e8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPMRm_OFFS(m)                                          (QDSS_TPDM_BASE_OFFS + 0x000007e8 + 0x4 * (m))
#define HWIO_TPDM_DSB_XPMRm_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_XPMRm_MAXm                                                      1
#define HWIO_TPDM_DSB_XPMRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_XPMRm_ADDR(m), HWIO_TPDM_DSB_XPMRm_RMSK)
#define HWIO_TPDM_DSB_XPMRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_XPMRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_XPMRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_XPMRm_ADDR(m),val)
#define HWIO_TPDM_DSB_XPMRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_XPMRm_ADDR(m),mask,val,HWIO_TPDM_DSB_XPMRm_INI(m))
#define HWIO_TPDM_DSB_XPMRm_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_DSB_XPMRm_VAL_SHFT                                                0x0

#define HWIO_TPDM_DSB_EDCRn_ADDR(n)                                          (QDSS_TPDM_BASE      + 0x00000808 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCRn_PHYS(n)                                          (QDSS_TPDM_BASE_PHYS + 0x00000808 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCRn_OFFS(n)                                          (QDSS_TPDM_BASE_OFFS + 0x00000808 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCRn_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_EDCRn_MAXn                                                      2
#define HWIO_TPDM_DSB_EDCRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_EDCRn_ADDR(n), HWIO_TPDM_DSB_EDCRn_RMSK)
#define HWIO_TPDM_DSB_EDCRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_EDCRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_EDCRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_EDCRn_ADDR(n),val)
#define HWIO_TPDM_DSB_EDCRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_EDCRn_ADDR(n),mask,val,HWIO_TPDM_DSB_EDCRn_INI(n))
#define HWIO_TPDM_DSB_EDCRn_EDGECONTROL_BMSK                                 0xffffffff
#define HWIO_TPDM_DSB_EDCRn_EDGECONTROL_SHFT                                        0x0

#define HWIO_TPDM_DSB_EDCRm_ADDR(m)                                          (QDSS_TPDM_BASE      + 0x00000808 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCRm_PHYS(m)                                          (QDSS_TPDM_BASE_PHYS + 0x00000808 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCRm_OFFS(m)                                          (QDSS_TPDM_BASE_OFFS + 0x00000808 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCRm_RMSK                                             0xffffffff
#define HWIO_TPDM_DSB_EDCRm_MAXm                                                      3
#define HWIO_TPDM_DSB_EDCRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_EDCRm_ADDR(m), HWIO_TPDM_DSB_EDCRm_RMSK)
#define HWIO_TPDM_DSB_EDCRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_EDCRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_EDCRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_EDCRm_ADDR(m),val)
#define HWIO_TPDM_DSB_EDCRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_EDCRm_ADDR(m),mask,val,HWIO_TPDM_DSB_EDCRm_INI(m))
#define HWIO_TPDM_DSB_EDCRm_EDGECONTROL_BMSK                                 0xffffffff
#define HWIO_TPDM_DSB_EDCRm_EDGECONTROL_SHFT                                        0x0

#define HWIO_TPDM_DSB_EDCMRn_ADDR(n)                                         (QDSS_TPDM_BASE      + 0x00000848 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCMRn_PHYS(n)                                         (QDSS_TPDM_BASE_PHYS + 0x00000848 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCMRn_OFFS(n)                                         (QDSS_TPDM_BASE_OFFS + 0x00000848 + 0x4 * (n))
#define HWIO_TPDM_DSB_EDCMRn_RMSK                                            0xffffffff
#define HWIO_TPDM_DSB_EDCMRn_MAXn                                                     0
#define HWIO_TPDM_DSB_EDCMRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_DSB_EDCMRn_ADDR(n), HWIO_TPDM_DSB_EDCMRn_RMSK)
#define HWIO_TPDM_DSB_EDCMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_EDCMRn_ADDR(n), mask)
#define HWIO_TPDM_DSB_EDCMRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_DSB_EDCMRn_ADDR(n),val)
#define HWIO_TPDM_DSB_EDCMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_EDCMRn_ADDR(n),mask,val,HWIO_TPDM_DSB_EDCMRn_INI(n))
#define HWIO_TPDM_DSB_EDCMRn_VAL_BMSK                                        0xffffffff
#define HWIO_TPDM_DSB_EDCMRn_VAL_SHFT                                               0x0

#define HWIO_TPDM_DSB_EDCMRm_ADDR(m)                                         (QDSS_TPDM_BASE      + 0x00000848 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCMRm_PHYS(m)                                         (QDSS_TPDM_BASE_PHYS + 0x00000848 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCMRm_OFFS(m)                                         (QDSS_TPDM_BASE_OFFS + 0x00000848 + 0x4 * (m))
#define HWIO_TPDM_DSB_EDCMRm_RMSK                                            0xffffffff
#define HWIO_TPDM_DSB_EDCMRm_MAXm                                                     1
#define HWIO_TPDM_DSB_EDCMRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_DSB_EDCMRm_ADDR(m), HWIO_TPDM_DSB_EDCMRm_RMSK)
#define HWIO_TPDM_DSB_EDCMRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_DSB_EDCMRm_ADDR(m), mask)
#define HWIO_TPDM_DSB_EDCMRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_DSB_EDCMRm_ADDR(m),val)
#define HWIO_TPDM_DSB_EDCMRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_DSB_EDCMRm_ADDR(m),mask,val,HWIO_TPDM_DSB_EDCMRm_INI(m))
#define HWIO_TPDM_DSB_EDCMRm_VAL_BMSK                                        0xffffffff
#define HWIO_TPDM_DSB_EDCMRm_VAL_SHFT                                               0x0

#define HWIO_TPDM_CMB_CR_ADDR                                                (QDSS_TPDM_BASE      + 0x00000a00)
#define HWIO_TPDM_CMB_CR_PHYS                                                (QDSS_TPDM_BASE_PHYS + 0x00000a00)
#define HWIO_TPDM_CMB_CR_OFFS                                                (QDSS_TPDM_BASE_OFFS + 0x00000a00)
#define HWIO_TPDM_CMB_CR_RMSK                                                      0xc7
#define HWIO_TPDM_CMB_CR_IN          \
        in_dword_masked(HWIO_TPDM_CMB_CR_ADDR, HWIO_TPDM_CMB_CR_RMSK)
#define HWIO_TPDM_CMB_CR_INM(m)      \
        in_dword_masked(HWIO_TPDM_CMB_CR_ADDR, m)
#define HWIO_TPDM_CMB_CR_OUT(v)      \
        out_dword(HWIO_TPDM_CMB_CR_ADDR,v)
#define HWIO_TPDM_CMB_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_CMB_CR_ADDR,m,v,HWIO_TPDM_CMB_CR_IN)
#define HWIO_TPDM_CMB_CR_ATBFLOWERR_BMSK                                           0x80
#define HWIO_TPDM_CMB_CR_ATBFLOWERR_SHFT                                            0x7
#define HWIO_TPDM_CMB_CR_EBITSET_BMSK                                              0x40
#define HWIO_TPDM_CMB_CR_EBITSET_SHFT                                               0x6
#define HWIO_TPDM_CMB_CR_FLOWCTRL_BMSK                                              0x4
#define HWIO_TPDM_CMB_CR_FLOWCTRL_SHFT                                              0x2
#define HWIO_TPDM_CMB_CR_MODE_BMSK                                                  0x2
#define HWIO_TPDM_CMB_CR_MODE_SHFT                                                  0x1
#define HWIO_TPDM_CMB_CR_E_BMSK                                                     0x1
#define HWIO_TPDM_CMB_CR_E_SHFT                                                     0x0

#define HWIO_TPDM_CMB_TIER_ADDR                                              (QDSS_TPDM_BASE      + 0x00000a04)
#define HWIO_TPDM_CMB_TIER_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x00000a04)
#define HWIO_TPDM_CMB_TIER_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x00000a04)
#define HWIO_TPDM_CMB_TIER_RMSK                                                     0x3
#define HWIO_TPDM_CMB_TIER_IN          \
        in_dword_masked(HWIO_TPDM_CMB_TIER_ADDR, HWIO_TPDM_CMB_TIER_RMSK)
#define HWIO_TPDM_CMB_TIER_INM(m)      \
        in_dword_masked(HWIO_TPDM_CMB_TIER_ADDR, m)
#define HWIO_TPDM_CMB_TIER_OUT(v)      \
        out_dword(HWIO_TPDM_CMB_TIER_ADDR,v)
#define HWIO_TPDM_CMB_TIER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_CMB_TIER_ADDR,m,v,HWIO_TPDM_CMB_TIER_IN)
#define HWIO_TPDM_CMB_TIER_XTRIG_TSENAB_BMSK                                        0x2
#define HWIO_TPDM_CMB_TIER_XTRIG_TSENAB_SHFT                                        0x1
#define HWIO_TPDM_CMB_TIER_PATT_TSENAB_BMSK                                         0x1
#define HWIO_TPDM_CMB_TIER_PATT_TSENAB_SHFT                                         0x0

#define HWIO_TPDM_CMB_TPRn_ADDR(n)                                           (QDSS_TPDM_BASE      + 0x00000a08 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPRn_PHYS(n)                                           (QDSS_TPDM_BASE_PHYS + 0x00000a08 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPRn_OFFS(n)                                           (QDSS_TPDM_BASE_OFFS + 0x00000a08 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPRn_RMSK                                              0xffffffff
#define HWIO_TPDM_CMB_TPRn_MAXn                                                       0
#define HWIO_TPDM_CMB_TPRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_CMB_TPRn_ADDR(n), HWIO_TPDM_CMB_TPRn_RMSK)
#define HWIO_TPDM_CMB_TPRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_TPRn_ADDR(n), mask)
#define HWIO_TPDM_CMB_TPRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_CMB_TPRn_ADDR(n),val)
#define HWIO_TPDM_CMB_TPRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_TPRn_ADDR(n),mask,val,HWIO_TPDM_CMB_TPRn_INI(n))
#define HWIO_TPDM_CMB_TPRn_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_CMB_TPRn_VAL_SHFT                                                 0x0

#define HWIO_TPDM_CMB_TPRm_ADDR(m)                                           (QDSS_TPDM_BASE      + 0x00000a08 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPRm_PHYS(m)                                           (QDSS_TPDM_BASE_PHYS + 0x00000a08 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPRm_OFFS(m)                                           (QDSS_TPDM_BASE_OFFS + 0x00000a08 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPRm_RMSK                                              0xffffffff
#define HWIO_TPDM_CMB_TPRm_MAXm                                                       1
#define HWIO_TPDM_CMB_TPRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_CMB_TPRm_ADDR(m), HWIO_TPDM_CMB_TPRm_RMSK)
#define HWIO_TPDM_CMB_TPRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_TPRm_ADDR(m), mask)
#define HWIO_TPDM_CMB_TPRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_CMB_TPRm_ADDR(m),val)
#define HWIO_TPDM_CMB_TPRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_TPRm_ADDR(m),mask,val,HWIO_TPDM_CMB_TPRm_INI(m))
#define HWIO_TPDM_CMB_TPRm_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_CMB_TPRm_VAL_SHFT                                                 0x0

#define HWIO_TPDM_CMB_TPMRn_ADDR(n)                                          (QDSS_TPDM_BASE      + 0x00000a10 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPMRn_PHYS(n)                                          (QDSS_TPDM_BASE_PHYS + 0x00000a10 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPMRn_OFFS(n)                                          (QDSS_TPDM_BASE_OFFS + 0x00000a10 + 0x4 * (n))
#define HWIO_TPDM_CMB_TPMRn_RMSK                                             0xffffffff
#define HWIO_TPDM_CMB_TPMRn_MAXn                                                      0
#define HWIO_TPDM_CMB_TPMRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_CMB_TPMRn_ADDR(n), HWIO_TPDM_CMB_TPMRn_RMSK)
#define HWIO_TPDM_CMB_TPMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_TPMRn_ADDR(n), mask)
#define HWIO_TPDM_CMB_TPMRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_CMB_TPMRn_ADDR(n),val)
#define HWIO_TPDM_CMB_TPMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_TPMRn_ADDR(n),mask,val,HWIO_TPDM_CMB_TPMRn_INI(n))
#define HWIO_TPDM_CMB_TPMRn_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_CMB_TPMRn_VAL_SHFT                                                0x0

#define HWIO_TPDM_CMB_TPMRm_ADDR(m)                                          (QDSS_TPDM_BASE      + 0x00000a10 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPMRm_PHYS(m)                                          (QDSS_TPDM_BASE_PHYS + 0x00000a10 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPMRm_OFFS(m)                                          (QDSS_TPDM_BASE_OFFS + 0x00000a10 + 0x4 * (m))
#define HWIO_TPDM_CMB_TPMRm_RMSK                                             0xffffffff
#define HWIO_TPDM_CMB_TPMRm_MAXm                                                      1
#define HWIO_TPDM_CMB_TPMRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_CMB_TPMRm_ADDR(m), HWIO_TPDM_CMB_TPMRm_RMSK)
#define HWIO_TPDM_CMB_TPMRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_TPMRm_ADDR(m), mask)
#define HWIO_TPDM_CMB_TPMRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_CMB_TPMRm_ADDR(m),val)
#define HWIO_TPDM_CMB_TPMRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_TPMRm_ADDR(m),mask,val,HWIO_TPDM_CMB_TPMRm_INI(m))
#define HWIO_TPDM_CMB_TPMRm_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_CMB_TPMRm_VAL_SHFT                                                0x0

#define HWIO_TPDM_CMB_XPRn_ADDR(n)                                           (QDSS_TPDM_BASE      + 0x00000a18 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPRn_PHYS(n)                                           (QDSS_TPDM_BASE_PHYS + 0x00000a18 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPRn_OFFS(n)                                           (QDSS_TPDM_BASE_OFFS + 0x00000a18 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPRn_RMSK                                              0xffffffff
#define HWIO_TPDM_CMB_XPRn_MAXn                                                       0
#define HWIO_TPDM_CMB_XPRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_CMB_XPRn_ADDR(n), HWIO_TPDM_CMB_XPRn_RMSK)
#define HWIO_TPDM_CMB_XPRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_XPRn_ADDR(n), mask)
#define HWIO_TPDM_CMB_XPRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_CMB_XPRn_ADDR(n),val)
#define HWIO_TPDM_CMB_XPRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_XPRn_ADDR(n),mask,val,HWIO_TPDM_CMB_XPRn_INI(n))
#define HWIO_TPDM_CMB_XPRn_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_CMB_XPRn_VAL_SHFT                                                 0x0

#define HWIO_TPDM_CMB_XPRm_ADDR(m)                                           (QDSS_TPDM_BASE      + 0x00000a18 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPRm_PHYS(m)                                           (QDSS_TPDM_BASE_PHYS + 0x00000a18 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPRm_OFFS(m)                                           (QDSS_TPDM_BASE_OFFS + 0x00000a18 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPRm_RMSK                                              0xffffffff
#define HWIO_TPDM_CMB_XPRm_MAXm                                                       1
#define HWIO_TPDM_CMB_XPRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_CMB_XPRm_ADDR(m), HWIO_TPDM_CMB_XPRm_RMSK)
#define HWIO_TPDM_CMB_XPRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_XPRm_ADDR(m), mask)
#define HWIO_TPDM_CMB_XPRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_CMB_XPRm_ADDR(m),val)
#define HWIO_TPDM_CMB_XPRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_XPRm_ADDR(m),mask,val,HWIO_TPDM_CMB_XPRm_INI(m))
#define HWIO_TPDM_CMB_XPRm_VAL_BMSK                                          0xffffffff
#define HWIO_TPDM_CMB_XPRm_VAL_SHFT                                                 0x0

#define HWIO_TPDM_CMB_XPMRn_ADDR(n)                                          (QDSS_TPDM_BASE      + 0x00000a20 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPMRn_PHYS(n)                                          (QDSS_TPDM_BASE_PHYS + 0x00000a20 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPMRn_OFFS(n)                                          (QDSS_TPDM_BASE_OFFS + 0x00000a20 + 0x4 * (n))
#define HWIO_TPDM_CMB_XPMRn_RMSK                                             0xffffffff
#define HWIO_TPDM_CMB_XPMRn_MAXn                                                      0
#define HWIO_TPDM_CMB_XPMRn_INI(n)        \
        in_dword_masked(HWIO_TPDM_CMB_XPMRn_ADDR(n), HWIO_TPDM_CMB_XPMRn_RMSK)
#define HWIO_TPDM_CMB_XPMRn_INMI(n,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_XPMRn_ADDR(n), mask)
#define HWIO_TPDM_CMB_XPMRn_OUTI(n,val)    \
        out_dword(HWIO_TPDM_CMB_XPMRn_ADDR(n),val)
#define HWIO_TPDM_CMB_XPMRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_XPMRn_ADDR(n),mask,val,HWIO_TPDM_CMB_XPMRn_INI(n))
#define HWIO_TPDM_CMB_XPMRn_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_CMB_XPMRn_VAL_SHFT                                                0x0

#define HWIO_TPDM_CMB_XPMRm_ADDR(m)                                          (QDSS_TPDM_BASE      + 0x00000a20 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPMRm_PHYS(m)                                          (QDSS_TPDM_BASE_PHYS + 0x00000a20 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPMRm_OFFS(m)                                          (QDSS_TPDM_BASE_OFFS + 0x00000a20 + 0x4 * (m))
#define HWIO_TPDM_CMB_XPMRm_RMSK                                             0xffffffff
#define HWIO_TPDM_CMB_XPMRm_MAXm                                                      1
#define HWIO_TPDM_CMB_XPMRm_INI(m)        \
        in_dword_masked(HWIO_TPDM_CMB_XPMRm_ADDR(m), HWIO_TPDM_CMB_XPMRm_RMSK)
#define HWIO_TPDM_CMB_XPMRm_INMI(m,mask)    \
        in_dword_masked(HWIO_TPDM_CMB_XPMRm_ADDR(m), mask)
#define HWIO_TPDM_CMB_XPMRm_OUTI(m,val)    \
        out_dword(HWIO_TPDM_CMB_XPMRm_ADDR(m),val)
#define HWIO_TPDM_CMB_XPMRm_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TPDM_CMB_XPMRm_ADDR(m),mask,val,HWIO_TPDM_CMB_XPMRm_INI(m))
#define HWIO_TPDM_CMB_XPMRm_VAL_BMSK                                         0xffffffff
#define HWIO_TPDM_CMB_XPMRm_VAL_SHFT                                                0x0

#define HWIO_TPDM_ITATBCNTRL_ADDR                                            (QDSS_TPDM_BASE      + 0x00000ef0)
#define HWIO_TPDM_ITATBCNTRL_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x00000ef0)
#define HWIO_TPDM_ITATBCNTRL_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x00000ef0)
#define HWIO_TPDM_ITATBCNTRL_RMSK                                            0xc07fffff
#define HWIO_TPDM_ITATBCNTRL_IN          \
        in_dword_masked(HWIO_TPDM_ITATBCNTRL_ADDR, HWIO_TPDM_ITATBCNTRL_RMSK)
#define HWIO_TPDM_ITATBCNTRL_INM(m)      \
        in_dword_masked(HWIO_TPDM_ITATBCNTRL_ADDR, m)
#define HWIO_TPDM_ITATBCNTRL_OUT(v)      \
        out_dword(HWIO_TPDM_ITATBCNTRL_ADDR,v)
#define HWIO_TPDM_ITATBCNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_ITATBCNTRL_ADDR,m,v,HWIO_TPDM_ITATBCNTRL_IN)
#define HWIO_TPDM_ITATBCNTRL_TSREQ_BMSK                                      0x80000000
#define HWIO_TPDM_ITATBCNTRL_TSREQ_SHFT                                            0x1f
#define HWIO_TPDM_ITATBCNTRL_ATVALID_BMSK                                    0x40000000
#define HWIO_TPDM_ITATBCNTRL_ATVALID_SHFT                                          0x1e
#define HWIO_TPDM_ITATBCNTRL_ATDATAMODE_BMSK                                   0x400000
#define HWIO_TPDM_ITATBCNTRL_ATDATAMODE_SHFT                                       0x16
#define HWIO_TPDM_ITATBCNTRL_ATBYTES_BMSK                                      0x3c0000
#define HWIO_TPDM_ITATBCNTRL_ATBYTES_SHFT                                          0x12
#define HWIO_TPDM_ITATBCNTRL_ATDATA_BMSK                                        0x3fc00
#define HWIO_TPDM_ITATBCNTRL_ATDATA_SHFT                                            0xa
#define HWIO_TPDM_ITATBCNTRL_ATID_BMSK                                            0x3f8
#define HWIO_TPDM_ITATBCNTRL_ATID_SHFT                                              0x3
#define HWIO_TPDM_ITATBCNTRL_TSVAL_BMSK                                             0x7
#define HWIO_TPDM_ITATBCNTRL_TSVAL_SHFT                                             0x0

#define HWIO_TPDM_ITCNTRL_ADDR                                               (QDSS_TPDM_BASE      + 0x00000f00)
#define HWIO_TPDM_ITCNTRL_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x00000f00)
#define HWIO_TPDM_ITCNTRL_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x00000f00)
#define HWIO_TPDM_ITCNTRL_RMSK                                                      0x1
#define HWIO_TPDM_ITCNTRL_IN          \
        in_dword_masked(HWIO_TPDM_ITCNTRL_ADDR, HWIO_TPDM_ITCNTRL_RMSK)
#define HWIO_TPDM_ITCNTRL_INM(m)      \
        in_dword_masked(HWIO_TPDM_ITCNTRL_ADDR, m)
#define HWIO_TPDM_ITCNTRL_OUT(v)      \
        out_dword(HWIO_TPDM_ITCNTRL_ADDR,v)
#define HWIO_TPDM_ITCNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_ITCNTRL_ADDR,m,v,HWIO_TPDM_ITCNTRL_IN)
#define HWIO_TPDM_ITCNTRL_IME_BMSK                                                  0x1
#define HWIO_TPDM_ITCNTRL_IME_SHFT                                                  0x0

#define HWIO_TPDM_CLAIMSET_ADDR                                              (QDSS_TPDM_BASE      + 0x00000fa0)
#define HWIO_TPDM_CLAIMSET_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x00000fa0)
#define HWIO_TPDM_CLAIMSET_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x00000fa0)
#define HWIO_TPDM_CLAIMSET_RMSK                                              0xffffffff
#define HWIO_TPDM_CLAIMSET_IN          \
        in_dword_masked(HWIO_TPDM_CLAIMSET_ADDR, HWIO_TPDM_CLAIMSET_RMSK)
#define HWIO_TPDM_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_TPDM_CLAIMSET_ADDR, m)
#define HWIO_TPDM_CLAIMSET_VAL_SET_BMSK                                      0xffffffff
#define HWIO_TPDM_CLAIMSET_VAL_SET_SHFT                                             0x0

#define HWIO_TPDM_CLAIMCLR_ADDR                                              (QDSS_TPDM_BASE      + 0x00000fa4)
#define HWIO_TPDM_CLAIMCLR_PHYS                                              (QDSS_TPDM_BASE_PHYS + 0x00000fa4)
#define HWIO_TPDM_CLAIMCLR_OFFS                                              (QDSS_TPDM_BASE_OFFS + 0x00000fa4)
#define HWIO_TPDM_CLAIMCLR_RMSK                                              0xffffffff
#define HWIO_TPDM_CLAIMCLR_IN          \
        in_dword_masked(HWIO_TPDM_CLAIMCLR_ADDR, HWIO_TPDM_CLAIMCLR_RMSK)
#define HWIO_TPDM_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_TPDM_CLAIMCLR_ADDR, m)
#define HWIO_TPDM_CLAIMCLR_VAL_CLR_BMSK                                      0xffffffff
#define HWIO_TPDM_CLAIMCLR_VAL_CLR_SHFT                                             0x0

#define HWIO_TPDM_DEVAFF0_ADDR                                               (QDSS_TPDM_BASE      + 0x00000fa8)
#define HWIO_TPDM_DEVAFF0_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x00000fa8)
#define HWIO_TPDM_DEVAFF0_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x00000fa8)
#define HWIO_TPDM_DEVAFF0_RMSK                                               0xffffffff
#define HWIO_TPDM_DEVAFF0_IN          \
        in_dword_masked(HWIO_TPDM_DEVAFF0_ADDR, HWIO_TPDM_DEVAFF0_RMSK)
#define HWIO_TPDM_DEVAFF0_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVAFF0_ADDR, m)
#define HWIO_TPDM_DEVAFF0_VAL_BMSK                                           0xffffffff
#define HWIO_TPDM_DEVAFF0_VAL_SHFT                                                  0x0

#define HWIO_TPDM_DEVAFF1_ADDR                                               (QDSS_TPDM_BASE      + 0x00000fac)
#define HWIO_TPDM_DEVAFF1_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x00000fac)
#define HWIO_TPDM_DEVAFF1_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x00000fac)
#define HWIO_TPDM_DEVAFF1_RMSK                                               0xffffffff
#define HWIO_TPDM_DEVAFF1_IN          \
        in_dword_masked(HWIO_TPDM_DEVAFF1_ADDR, HWIO_TPDM_DEVAFF1_RMSK)
#define HWIO_TPDM_DEVAFF1_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVAFF1_ADDR, m)
#define HWIO_TPDM_DEVAFF1_VAL_BMSK                                           0xffffffff
#define HWIO_TPDM_DEVAFF1_VAL_SHFT                                                  0x0

#define HWIO_TPDM_LAR_ADDR                                                   (QDSS_TPDM_BASE      + 0x00000fb0)
#define HWIO_TPDM_LAR_PHYS                                                   (QDSS_TPDM_BASE_PHYS + 0x00000fb0)
#define HWIO_TPDM_LAR_OFFS                                                   (QDSS_TPDM_BASE_OFFS + 0x00000fb0)
#define HWIO_TPDM_LAR_RMSK                                                   0xffffffff
#define HWIO_TPDM_LAR_IN          \
        in_dword_masked(HWIO_TPDM_LAR_ADDR, HWIO_TPDM_LAR_RMSK)
#define HWIO_TPDM_LAR_INM(m)      \
        in_dword_masked(HWIO_TPDM_LAR_ADDR, m)
#define HWIO_TPDM_LAR_OUT(v)      \
        out_dword(HWIO_TPDM_LAR_ADDR,v)
#define HWIO_TPDM_LAR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TPDM_LAR_ADDR,m,v,HWIO_TPDM_LAR_IN)
#define HWIO_TPDM_LAR_KEY_BMSK                                               0xffffffff
#define HWIO_TPDM_LAR_KEY_SHFT                                                      0x0

#define HWIO_TPDM_LSR_ADDR                                                   (QDSS_TPDM_BASE      + 0x00000fb4)
#define HWIO_TPDM_LSR_PHYS                                                   (QDSS_TPDM_BASE_PHYS + 0x00000fb4)
#define HWIO_TPDM_LSR_OFFS                                                   (QDSS_TPDM_BASE_OFFS + 0x00000fb4)
#define HWIO_TPDM_LSR_RMSK                                                          0x7
#define HWIO_TPDM_LSR_IN          \
        in_dword_masked(HWIO_TPDM_LSR_ADDR, HWIO_TPDM_LSR_RMSK)
#define HWIO_TPDM_LSR_INM(m)      \
        in_dword_masked(HWIO_TPDM_LSR_ADDR, m)
#define HWIO_TPDM_LSR_NTT_BMSK                                                      0x4
#define HWIO_TPDM_LSR_NTT_SHFT                                                      0x2
#define HWIO_TPDM_LSR_SLK_BMSK                                                      0x2
#define HWIO_TPDM_LSR_SLK_SHFT                                                      0x1
#define HWIO_TPDM_LSR_SLI_BMSK                                                      0x1
#define HWIO_TPDM_LSR_SLI_SHFT                                                      0x0

#define HWIO_TPDM_AUTHSTATUS_ADDR                                            (QDSS_TPDM_BASE      + 0x00000fb8)
#define HWIO_TPDM_AUTHSTATUS_PHYS                                            (QDSS_TPDM_BASE_PHYS + 0x00000fb8)
#define HWIO_TPDM_AUTHSTATUS_OFFS                                            (QDSS_TPDM_BASE_OFFS + 0x00000fb8)
#define HWIO_TPDM_AUTHSTATUS_RMSK                                                  0xff
#define HWIO_TPDM_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_TPDM_AUTHSTATUS_ADDR, HWIO_TPDM_AUTHSTATUS_RMSK)
#define HWIO_TPDM_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_TPDM_AUTHSTATUS_ADDR, m)
#define HWIO_TPDM_AUTHSTATUS_SNID_BMSK                                             0xc0
#define HWIO_TPDM_AUTHSTATUS_SNID_SHFT                                              0x6
#define HWIO_TPDM_AUTHSTATUS_SID_BMSK                                              0x30
#define HWIO_TPDM_AUTHSTATUS_SID_SHFT                                               0x4
#define HWIO_TPDM_AUTHSTATUS_NSNID_BMSK                                             0xc
#define HWIO_TPDM_AUTHSTATUS_NSNID_SHFT                                             0x2
#define HWIO_TPDM_AUTHSTATUS_NSID_BMSK                                              0x3
#define HWIO_TPDM_AUTHSTATUS_NSID_SHFT                                              0x0

#define HWIO_TPDM_DEVARCH_ADDR                                               (QDSS_TPDM_BASE      + 0x00000fbc)
#define HWIO_TPDM_DEVARCH_PHYS                                               (QDSS_TPDM_BASE_PHYS + 0x00000fbc)
#define HWIO_TPDM_DEVARCH_OFFS                                               (QDSS_TPDM_BASE_OFFS + 0x00000fbc)
#define HWIO_TPDM_DEVARCH_RMSK                                               0xffffffff
#define HWIO_TPDM_DEVARCH_IN          \
        in_dword_masked(HWIO_TPDM_DEVARCH_ADDR, HWIO_TPDM_DEVARCH_RMSK)
#define HWIO_TPDM_DEVARCH_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVARCH_ADDR, m)
#define HWIO_TPDM_DEVARCH_ARCHITECT_BMSK                                     0xffe00000
#define HWIO_TPDM_DEVARCH_ARCHITECT_SHFT                                           0x15
#define HWIO_TPDM_DEVARCH_PRESENT_BMSK                                         0x100000
#define HWIO_TPDM_DEVARCH_PRESENT_SHFT                                             0x14
#define HWIO_TPDM_DEVARCH_REVISION_BMSK                                         0xf0000
#define HWIO_TPDM_DEVARCH_REVISION_SHFT                                            0x10
#define HWIO_TPDM_DEVARCH_ARCHID_BMSK                                            0xffff
#define HWIO_TPDM_DEVARCH_ARCHID_SHFT                                               0x0

#define HWIO_TPDM_DEVID2_ADDR                                                (QDSS_TPDM_BASE      + 0x00000fc0)
#define HWIO_TPDM_DEVID2_PHYS                                                (QDSS_TPDM_BASE_PHYS + 0x00000fc0)
#define HWIO_TPDM_DEVID2_OFFS                                                (QDSS_TPDM_BASE_OFFS + 0x00000fc0)
#define HWIO_TPDM_DEVID2_RMSK                                                   0x3ffc0
#define HWIO_TPDM_DEVID2_IN          \
        in_dword_masked(HWIO_TPDM_DEVID2_ADDR, HWIO_TPDM_DEVID2_RMSK)
#define HWIO_TPDM_DEVID2_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVID2_ADDR, m)
#define HWIO_TPDM_DEVID2_TC1_WIDTH_BMSK                                         0x3f000
#define HWIO_TPDM_DEVID2_TC1_WIDTH_SHFT                                             0xc
#define HWIO_TPDM_DEVID2_TC0_WIDTH_BMSK                                           0xfc0
#define HWIO_TPDM_DEVID2_TC0_WIDTH_SHFT                                             0x6

#define HWIO_TPDM_DEVID1_ADDR                                                (QDSS_TPDM_BASE      + 0x00000fc4)
#define HWIO_TPDM_DEVID1_PHYS                                                (QDSS_TPDM_BASE_PHYS + 0x00000fc4)
#define HWIO_TPDM_DEVID1_OFFS                                                (QDSS_TPDM_BASE_OFFS + 0x00000fc4)
#define HWIO_TPDM_DEVID1_RMSK                                                  0xfff03f
#define HWIO_TPDM_DEVID1_IN          \
        in_dword_masked(HWIO_TPDM_DEVID1_ADDR, HWIO_TPDM_DEVID1_RMSK)
#define HWIO_TPDM_DEVID1_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVID1_ADDR, m)
#define HWIO_TPDM_DEVID1_TC1GP2_BMSK                                           0xc00000
#define HWIO_TPDM_DEVID1_TC1GP2_SHFT                                               0x16
#define HWIO_TPDM_DEVID1_TC1GP1_BMSK                                           0x300000
#define HWIO_TPDM_DEVID1_TC1GP1_SHFT                                               0x14
#define HWIO_TPDM_DEVID1_TC1GP0_BMSK                                            0xc0000
#define HWIO_TPDM_DEVID1_TC1GP0_SHFT                                               0x12
#define HWIO_TPDM_DEVID1_TC0GP2_BMSK                                            0x30000
#define HWIO_TPDM_DEVID1_TC0GP2_SHFT                                               0x10
#define HWIO_TPDM_DEVID1_TC0GP1_BMSK                                             0xc000
#define HWIO_TPDM_DEVID1_TC0GP1_SHFT                                                0xe
#define HWIO_TPDM_DEVID1_TC0GP0_BMSK                                             0x3000
#define HWIO_TPDM_DEVID1_TC0GP0_SHFT                                                0xc
#define HWIO_TPDM_DEVID1_TC1GPCNT_BMSK                                             0x38
#define HWIO_TPDM_DEVID1_TC1GPCNT_SHFT                                              0x3
#define HWIO_TPDM_DEVID1_TC0GPCNT_BMSK                                              0x7
#define HWIO_TPDM_DEVID1_TC0GPCNT_SHFT                                              0x0

#define HWIO_TPDM_DEVID_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fc8)
#define HWIO_TPDM_DEVID_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fc8)
#define HWIO_TPDM_DEVID_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fc8)
#define HWIO_TPDM_DEVID_RMSK                                                  0x1fffffe
#define HWIO_TPDM_DEVID_IN          \
        in_dword_masked(HWIO_TPDM_DEVID_ADDR, HWIO_TPDM_DEVID_RMSK)
#define HWIO_TPDM_DEVID_INM(m)      \
        in_dword_masked(HWIO_TPDM_DEVID_ADDR, m)
#define HWIO_TPDM_DEVID_TC_SP_WIDTH_BMSK                                      0x1f80000
#define HWIO_TPDM_DEVID_TC_SP_WIDTH_SHFT                                           0x13
#define HWIO_TPDM_DEVID_BC_WIDTH_BMSK                                           0x7e000
#define HWIO_TPDM_DEVID_BC_WIDTH_SHFT                                               0xd
#define HWIO_TPDM_DEVID_BC_CNT_BMSK                                              0x1f00
#define HWIO_TPDM_DEVID_BC_CNT_SHFT                                                 0x8
#define HWIO_TPDM_DEVID_TC_CNT_BMSK                                                0xc0
#define HWIO_TPDM_DEVID_TC_CNT_SHFT                                                 0x6
#define HWIO_TPDM_DEVID_CMB_IN_BMSK                                                0x30
#define HWIO_TPDM_DEVID_CMB_IN_SHFT                                                 0x4
#define HWIO_TPDM_DEVID_DSB_ELEM_BMSK                                               0x8
#define HWIO_TPDM_DEVID_DSB_ELEM_SHFT                                               0x3
#define HWIO_TPDM_DEVID_DSB_IN_BMSK                                                 0x6
#define HWIO_TPDM_DEVID_DSB_IN_SHFT                                                 0x1

#define HWIO_TPDM_PIDR4_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fd0)
#define HWIO_TPDM_PIDR4_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fd0)
#define HWIO_TPDM_PIDR4_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fd0)
#define HWIO_TPDM_PIDR4_RMSK                                                       0xff
#define HWIO_TPDM_PIDR4_IN          \
        in_dword_masked(HWIO_TPDM_PIDR4_ADDR, HWIO_TPDM_PIDR4_RMSK)
#define HWIO_TPDM_PIDR4_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR4_ADDR, m)
#define HWIO_TPDM_PIDR4_SIZE_BMSK                                                  0xf0
#define HWIO_TPDM_PIDR4_SIZE_SHFT                                                   0x4
#define HWIO_TPDM_PIDR4_DES_2_BMSK                                                  0xf
#define HWIO_TPDM_PIDR4_DES_2_SHFT                                                  0x0

#define HWIO_TPDM_PIDR5_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fd4)
#define HWIO_TPDM_PIDR5_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fd4)
#define HWIO_TPDM_PIDR5_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fd4)
#define HWIO_TPDM_PIDR5_RMSK                                                 0xffffffff
#define HWIO_TPDM_PIDR5_IN          \
        in_dword_masked(HWIO_TPDM_PIDR5_ADDR, HWIO_TPDM_PIDR5_RMSK)
#define HWIO_TPDM_PIDR5_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR5_ADDR, m)
#define HWIO_TPDM_PIDR5_EMPTY_BMSK                                           0xffffffff
#define HWIO_TPDM_PIDR5_EMPTY_SHFT                                                  0x0

#define HWIO_TPDM_PIDR6_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fd8)
#define HWIO_TPDM_PIDR6_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fd8)
#define HWIO_TPDM_PIDR6_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fd8)
#define HWIO_TPDM_PIDR6_RMSK                                                 0xffffffff
#define HWIO_TPDM_PIDR6_IN          \
        in_dword_masked(HWIO_TPDM_PIDR6_ADDR, HWIO_TPDM_PIDR6_RMSK)
#define HWIO_TPDM_PIDR6_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR6_ADDR, m)
#define HWIO_TPDM_PIDR6_EMPTY_BMSK                                           0xffffffff
#define HWIO_TPDM_PIDR6_EMPTY_SHFT                                                  0x0

#define HWIO_TPDM_PIDR7_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fdc)
#define HWIO_TPDM_PIDR7_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fdc)
#define HWIO_TPDM_PIDR7_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fdc)
#define HWIO_TPDM_PIDR7_RMSK                                                 0xffffffff
#define HWIO_TPDM_PIDR7_IN          \
        in_dword_masked(HWIO_TPDM_PIDR7_ADDR, HWIO_TPDM_PIDR7_RMSK)
#define HWIO_TPDM_PIDR7_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR7_ADDR, m)
#define HWIO_TPDM_PIDR7_EMPTY_BMSK                                           0xffffffff
#define HWIO_TPDM_PIDR7_EMPTY_SHFT                                                  0x0

#define HWIO_TPDM_PIDR0_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fe0)
#define HWIO_TPDM_PIDR0_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fe0)
#define HWIO_TPDM_PIDR0_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fe0)
#define HWIO_TPDM_PIDR0_RMSK                                                       0xff
#define HWIO_TPDM_PIDR0_IN          \
        in_dword_masked(HWIO_TPDM_PIDR0_ADDR, HWIO_TPDM_PIDR0_RMSK)
#define HWIO_TPDM_PIDR0_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR0_ADDR, m)
#define HWIO_TPDM_PIDR0_PART_7_6_BMSK                                              0xc0
#define HWIO_TPDM_PIDR0_PART_7_6_SHFT                                               0x6
#define HWIO_TPDM_PIDR0_PART_5_BMSK                                                0x20
#define HWIO_TPDM_PIDR0_PART_5_SHFT                                                 0x5
#define HWIO_TPDM_PIDR0_PART_4_BMSK                                                0x10
#define HWIO_TPDM_PIDR0_PART_4_SHFT                                                 0x4
#define HWIO_TPDM_PIDR0_PART_3_BMSK                                                 0x8
#define HWIO_TPDM_PIDR0_PART_3_SHFT                                                 0x3
#define HWIO_TPDM_PIDR0_PART_2_BMSK                                                 0x4
#define HWIO_TPDM_PIDR0_PART_2_SHFT                                                 0x2
#define HWIO_TPDM_PIDR0_PART_1_BMSK                                                 0x2
#define HWIO_TPDM_PIDR0_PART_1_SHFT                                                 0x1
#define HWIO_TPDM_PIDR0_PART_0_BMSK                                                 0x1
#define HWIO_TPDM_PIDR0_PART_0_SHFT                                                 0x0

#define HWIO_TPDM_PIDR1_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fe4)
#define HWIO_TPDM_PIDR1_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fe4)
#define HWIO_TPDM_PIDR1_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fe4)
#define HWIO_TPDM_PIDR1_RMSK                                                       0xff
#define HWIO_TPDM_PIDR1_IN          \
        in_dword_masked(HWIO_TPDM_PIDR1_ADDR, HWIO_TPDM_PIDR1_RMSK)
#define HWIO_TPDM_PIDR1_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR1_ADDR, m)
#define HWIO_TPDM_PIDR1_DES_0_BMSK                                                 0xf0
#define HWIO_TPDM_PIDR1_DES_0_SHFT                                                  0x4
#define HWIO_TPDM_PIDR1_PART_1_BMSK                                                 0xf
#define HWIO_TPDM_PIDR1_PART_1_SHFT                                                 0x0

#define HWIO_TPDM_PIDR2_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fe8)
#define HWIO_TPDM_PIDR2_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fe8)
#define HWIO_TPDM_PIDR2_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fe8)
#define HWIO_TPDM_PIDR2_RMSK                                                       0xff
#define HWIO_TPDM_PIDR2_IN          \
        in_dword_masked(HWIO_TPDM_PIDR2_ADDR, HWIO_TPDM_PIDR2_RMSK)
#define HWIO_TPDM_PIDR2_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR2_ADDR, m)
#define HWIO_TPDM_PIDR2_REVISION_BMSK                                              0xf0
#define HWIO_TPDM_PIDR2_REVISION_SHFT                                               0x4
#define HWIO_TPDM_PIDR2_JEDEC_BMSK                                                  0x8
#define HWIO_TPDM_PIDR2_JEDEC_SHFT                                                  0x3
#define HWIO_TPDM_PIDR2_DES_1_BMSK                                                  0x7
#define HWIO_TPDM_PIDR2_DES_1_SHFT                                                  0x0

#define HWIO_TPDM_PIDR3_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000fec)
#define HWIO_TPDM_PIDR3_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000fec)
#define HWIO_TPDM_PIDR3_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000fec)
#define HWIO_TPDM_PIDR3_RMSK                                                       0xff
#define HWIO_TPDM_PIDR3_IN          \
        in_dword_masked(HWIO_TPDM_PIDR3_ADDR, HWIO_TPDM_PIDR3_RMSK)
#define HWIO_TPDM_PIDR3_INM(m)      \
        in_dword_masked(HWIO_TPDM_PIDR3_ADDR, m)
#define HWIO_TPDM_PIDR3_REVAND_BMSK                                                0xf0
#define HWIO_TPDM_PIDR3_REVAND_SHFT                                                 0x4
#define HWIO_TPDM_PIDR3_CMOD_BMSK                                                   0xf
#define HWIO_TPDM_PIDR3_CMOD_SHFT                                                   0x0

#define HWIO_TPDM_CIDR0_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000ff0)
#define HWIO_TPDM_CIDR0_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000ff0)
#define HWIO_TPDM_CIDR0_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000ff0)
#define HWIO_TPDM_CIDR0_RMSK                                                       0xff
#define HWIO_TPDM_CIDR0_IN          \
        in_dword_masked(HWIO_TPDM_CIDR0_ADDR, HWIO_TPDM_CIDR0_RMSK)
#define HWIO_TPDM_CIDR0_INM(m)      \
        in_dword_masked(HWIO_TPDM_CIDR0_ADDR, m)
#define HWIO_TPDM_CIDR0_PRMBL_0_BMSK                                               0xff
#define HWIO_TPDM_CIDR0_PRMBL_0_SHFT                                                0x0

#define HWIO_TPDM_CIDR1_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000ff4)
#define HWIO_TPDM_CIDR1_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000ff4)
#define HWIO_TPDM_CIDR1_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000ff4)
#define HWIO_TPDM_CIDR1_RMSK                                                       0xff
#define HWIO_TPDM_CIDR1_IN          \
        in_dword_masked(HWIO_TPDM_CIDR1_ADDR, HWIO_TPDM_CIDR1_RMSK)
#define HWIO_TPDM_CIDR1_INM(m)      \
        in_dword_masked(HWIO_TPDM_CIDR1_ADDR, m)
#define HWIO_TPDM_CIDR1_CLASS_BMSK                                                 0xf0
#define HWIO_TPDM_CIDR1_CLASS_SHFT                                                  0x4
#define HWIO_TPDM_CIDR1_PRMBL_1_BMSK                                                0xf
#define HWIO_TPDM_CIDR1_PRMBL_1_SHFT                                                0x0

#define HWIO_TPDM_CIDR2_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000ff8)
#define HWIO_TPDM_CIDR2_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000ff8)
#define HWIO_TPDM_CIDR2_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000ff8)
#define HWIO_TPDM_CIDR2_RMSK                                                       0xff
#define HWIO_TPDM_CIDR2_IN          \
        in_dword_masked(HWIO_TPDM_CIDR2_ADDR, HWIO_TPDM_CIDR2_RMSK)
#define HWIO_TPDM_CIDR2_INM(m)      \
        in_dword_masked(HWIO_TPDM_CIDR2_ADDR, m)
#define HWIO_TPDM_CIDR2_PRMBL_2_BMSK                                               0xff
#define HWIO_TPDM_CIDR2_PRMBL_2_SHFT                                                0x0

#define HWIO_TPDM_CIDR3_ADDR                                                 (QDSS_TPDM_BASE      + 0x00000ffc)
#define HWIO_TPDM_CIDR3_PHYS                                                 (QDSS_TPDM_BASE_PHYS + 0x00000ffc)
#define HWIO_TPDM_CIDR3_OFFS                                                 (QDSS_TPDM_BASE_OFFS + 0x00000ffc)
#define HWIO_TPDM_CIDR3_RMSK                                                       0xff
#define HWIO_TPDM_CIDR3_IN          \
        in_dword_masked(HWIO_TPDM_CIDR3_ADDR, HWIO_TPDM_CIDR3_RMSK)
#define HWIO_TPDM_CIDR3_INM(m)      \
        in_dword_masked(HWIO_TPDM_CIDR3_ADDR, m)
#define HWIO_TPDM_CIDR3_PRMBL_3_BMSK                                               0xff
#define HWIO_TPDM_CIDR3_PRMBL_3_SHFT                                                0x0


#endif /* __HWIO_QDSS_TPDM_H__ */
