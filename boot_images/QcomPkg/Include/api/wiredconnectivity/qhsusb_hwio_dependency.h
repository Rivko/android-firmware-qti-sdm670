#ifndef __QHSUSB_HWIO_DEPENDENCY_H__
#define __QHSUSB_HWIO_DEPENDENCY_H__
/*
===========================================================================
*/
/**
  @file qhsusb_hwio_dependency.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PERIPH_SS_USB1_HS_USB_OTG_HS_BASE
    PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE
    GCC_CLK_CTL_REG

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================


  ===========================================================================
*/

/******************************************************************************/
/*#includes from msm.h                                                  */
/******************************************************************************/
/*
 * Common types.
 */
#ifndef _ARM_ASM_
#include "comdef.h"
#endif

/*
 * HWIO access macros.
 */
#include "msmhwio.h"

/*
 * For now include private definitions here until drivers move to
 * include msmp.h directly.
 */
#ifdef FEATURE_LIBRARY_ONLY
#include "msmp.h"
#endif
/*******************************************************************************/
/* # includes which are needed by qhsusb register mapping                      */
/*******************************************************************************/
#include "msmhwiobase.h"

/*****************************************************************************/
/* #defines from msmhwioreg.h                                                */
/*****************************************************************************/

/*----------------------------------------------------------------------------
 * MODULE: PERIPH_SS_USB1_HS_USB_OTG_HS_BASE
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE                                                       (PERIPH_SS_BASE      + 0x00255000)
#define PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS                                                  (PERIPH_SS_BASE_PHYS + 0x00255000)
#define PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS                                                  0x00255000

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_ADDR                                                        (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_PHYS                                                        (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000000)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_OFFS                                                        (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000000)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_RMSK                                                          0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_POR                                                         0x0042fa05
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_REVISON_BMSK                                                  0xff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_REVISON_SHFT                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_NID_BMSK                                                        0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_NID_SHFT                                                           0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_ID_BMSK                                                           0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ID_ID_SHFT                                                            0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000004)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000004)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000004)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_RMSK                                                      0x3ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_POR                                                  0x000001c2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_SM_BMSK                                                   0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_SM_SHFT                                                     0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_PHYM_BMSK                                                 0x1c0
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_PHYM_SHFT                                                   0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_PHYW_BMSK                                                  0x30
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_PHYW_SHFT                                                   0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_BWT_BMSK                                                    0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_BWT_SHFT                                                    0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_CLCK_BMSK                                                   0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_CLCK_SHFT                                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_RT_BMSK                                                     0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWGENERAL_RT_SHFT                                                     0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000008)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000008)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_RMSK                                                    0xffff000f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_POR                                                     0x10020001
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_TTPER_BMSK                                              0xff000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_TTPER_SHFT                                                    0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_TTASY_BMSK                                                0xff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_TTASY_SHFT                                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_NPORT_BMSK                                                     0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_NPORT_SHFT                                                     0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_HC_BMSK                                                        0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWHOST_HC_SHFT                                                        0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000000c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000000c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000000c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_RMSK                                                        0x3f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_POR                                                   0x00000021
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_DEVEP_BMSK                                                  0x3e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_DEVEP_SHFT                                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_DC_BMSK                                                      0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWDEVICE_DC_SHFT                                                      0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_ADDR                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_PHYS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000010)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_OFFS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000010)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_RMSK                                                   0x80ffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_POR                                                    0x80070b08
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXLCR_BMSK                                             0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXLCR_SHFT                                                   0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK                                           0xff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT                                               0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXADD_BMSK                                                 0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXADD_SHFT                                                    0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXBURST_BMSK                                                 0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWTXBUF_TXBURST_SHFT                                                  0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_ADDR                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000014)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_PHYS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000014)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_OFFS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000014)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RMSK                                                       0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_POR                                                    0x00000808
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RX_ADD_BMSK                                                0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RX_ADD_SHFT                                                   0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RX_BURST_BMSK                                                0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HWRXBUF_RX_BURST_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(n)                                           (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_PHYS(n)                                           (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_OFFS(n)                                           (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_RMSK                                              0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_MAXn                                                      15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_POR                                               0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), mask)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),val)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),mask,val,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_INI(n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_BMSK                             0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_SHFT                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000080)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000080)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000080)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_RMSK                                                  0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK                                            0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000084)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_PHYS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000084)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_OFFS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000084)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_RMSK                                              0xc1ffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_POR                                               0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK                                       0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT                                             0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK                                       0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT                                             0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK                                       0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT                                            0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK                                         0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT                                              0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000088)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000088)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000088)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_RMSK                                                  0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK                                            0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000008c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_PHYS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000008c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_OFFS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000008c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_RMSK                                              0xc1ffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_POR                                               0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK                                       0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT                                             0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK                                       0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT                                             0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK                                       0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT                                            0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK                                         0xffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT                                              0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000090)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000090)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_RMSK                                                        0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_POR                                                  0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK                                              0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT                                              0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000094)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000094)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000094)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_RMSK                                                         0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_POR                                                   0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK                                            0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT                                            0x1

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000098)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000098)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000098)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_RMSK                                                  0x8000001f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_POR                                                   0x00000008
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK                             0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_SHFT                                   0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_BMSK                                          0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_SHFT                                           0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK                                              0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT                                              0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK                                      0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT                                      0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK                                             0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT                                             0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000009c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000009c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000009c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_RMSK                                                0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_POR                                                 0x00000830
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_BMSK                         0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_SHFT                               0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_BMSK                                      0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_SHFT                                         0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_BMSK                                      0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_SHFT                                         0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_BMSK                                    0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_SHFT                                       0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_BMSK                                     0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_SHFT                                        0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_BMSK                                          0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_SHFT                                            0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_BMSK                                       0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_SHFT                                         0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK                             0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT                               0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK                              0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT                                0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_BMSK                                 0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_SHFT                                  0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK                              0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_SHFT                               0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_BMSK                                       0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_SHFT                                        0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_BMSK                              0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_SHFT                               0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_BMSK                                       0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000000a0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_PHYS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000000a0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_OFFS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000000a0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RMSK                                                 0xfffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_POR                                               0x000aff60
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_BMSK                          0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_SHFT                             0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_BMSK                    0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_SHFT                       0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_BMSK                                        0x20000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_SHFT                                           0x11
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_BMSK                                 0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_SHFT                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_BMSK                             0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_SHFT                                0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_BMSK                          0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_SHFT                             0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_BMSK                         0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_SHFT                            0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_BMSK                         0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_SHFT                            0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_BMSK                                  0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_SHFT                                    0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_BMSK                                    0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_SHFT                                      0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_BMSK                                     0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_SHFT                                       0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_BMSK                                    0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_SHFT                                      0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_BMSK                                   0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_SHFT                                    0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_BMSK                                 0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_SHFT                                  0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_BMSK                                        0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_SHFT                                         0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_BMSK                                           0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_SHFT                                            0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_BMSK                                           0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_SHFT                                           0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_BMSK                                 0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_SHFT                                 0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_BMSK                                   0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_SHFT                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_BMSK                                    0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_SHFT                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000100)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000100)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000100)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_RMSK                                                 0xffff00ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_POR                                                  0x01000040
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_HCIVERSION_BMSK                                      0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_HCIVERSION_SHFT                                            0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK                                             0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT                                              0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000104)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000104)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000104)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_RMSK                                                  0xff1ff1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_POR                                                  0x00010011
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_TT_BMSK                                             0xf000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_TT_SHFT                                                  0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PTT_BMSK                                             0xf00000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PTT_SHFT                                                 0x14
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_PI_BMSK                                                 0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_PI_SHFT                                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_CC_BMSK                                                0xf000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_CC_SHFT                                                   0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PCC_BMSK                                                0xf00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PCC_SHFT                                                  0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_PPC_BMSK                                                   0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_PPC_SHFT                                                    0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK                                                0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT                                                0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000108)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000108)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000108)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_RMSK                                                     0xfff7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_POR                                                  0x00000006
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_EECP_BMSK                                                0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_EECP_SHFT                                                   0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_IST_BMSK                                                   0xf0
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_IST_SHFT                                                    0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ASP_BMSK                                                    0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ASP_SHFT                                                    0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_PFL_BMSK                                                    0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_PFL_SHFT                                                    0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ADC_BMSK                                                    0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HCCPARAMS_ADC_SHFT                                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000120)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000120)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000120)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_RMSK                                                    0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_POR                                                 0x00000001
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK                                         0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT                                            0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000124)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000124)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000124)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_RMSK                                                      0x19f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_POR                                                  0x00000190
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_HC_BMSK                                                   0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_HC_SHFT                                                     0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_DC_BMSK                                                    0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_DC_SHFT                                                     0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_DEN_BMSK                                                   0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DCCPARAMS_DEN_SHFT                                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000140)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000140)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000140)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RMSK                                                    0xfeffebff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_POR                                                     0x00080000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RST_CTRL_BMSK                                           0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RST_CTRL_SHFT                                                 0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK                                      0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT                                            0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK                                    0x20000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT                                          0x1d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK                                0x10000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT                                      0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK                             0x8000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_SHFT                                  0x1b
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_BMSK                                  0x4000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_SHFT                                       0x1a
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK                                       0x2000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT                                            0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ITC_BMSK                                                  0xff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ITC_SHFT                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS2_BMSK                                                    0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS2_SHFT                                                       0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ATDTW_BMSK                                                  0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ATDTW_SHFT                                                     0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SUTW_BMSK                                                   0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_SUTW_SHFT                                                      0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASPE_BMSK                                                    0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASPE_SHFT                                                      0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASP_BMSK                                                     0x300
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASP_SHFT                                                       0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_LR_BMSK                                                       0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_LR_SHFT                                                        0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_IAA_BMSK                                                      0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_IAA_SHFT                                                       0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASE_BMSK                                                      0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_ASE_SHFT                                                       0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_PSE_BMSK                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_PSE_SHFT                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS_BMSK                                                        0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_FS_SHFT                                                        0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RST_BMSK                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RST_SHFT                                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RS_BMSK                                                        0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBCMD_RS_SHFT                                                        0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000144)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000144)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000144)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_RMSK                                                    0xfb0df5ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_POR                                                     0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ULPI_INTR_BMSK                                          0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ULPI_INTR_SHFT                                                0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_BMSK                                   0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_SHFT                                         0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_SESS_VLD_BMSK                                       0x20000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_SESS_VLD_SHFT                                             0x1d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_ALT_INT_BMSK                                        0x10000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PHY_ALT_INT_SHFT                                              0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_BMSK                                   0x8000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_SHFT                                        0x1b
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_TI1_BMSK                                                 0x2000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_TI1_SHFT                                                      0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_TI0_BMSK                                                 0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_TI0_SHFT                                                      0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UPI_BMSK                                                   0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UPI_SHFT                                                      0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UAI_BMSK                                                   0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UAI_SHFT                                                      0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_NAKI_BMSK                                                  0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_NAKI_SHFT                                                     0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_AS_BMSK                                                     0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_AS_SHFT                                                        0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PS_BMSK                                                     0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PS_SHFT                                                        0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_RCL_BMSK                                                    0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_RCL_SHFT                                                       0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_HCH_BMSK                                                    0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_HCH_SHFT                                                       0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ULPII_BMSK                                                   0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_ULPII_SHFT                                                     0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SLI_BMSK                                                     0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SLI_SHFT                                                       0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SRI_BMSK                                                      0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SRI_SHFT                                                       0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_URI_BMSK                                                      0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_URI_SHFT                                                       0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_AAI_BMSK                                                      0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_AAI_SHFT                                                       0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SEI_BMSK                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_SEI_SHFT                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_FRI_BMSK                                                       0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_FRI_SHFT                                                       0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PCI_BMSK                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_PCI_SHFT                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UEI_BMSK                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UEI_SHFT                                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UI_BMSK                                                        0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBSTS_UI_SHFT                                                        0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ADDR                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000148)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_PHYS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000148)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_OFFS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000148)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_RMSK                                                   0xc30d05ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_POR                                                    0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK                                      0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT                                            0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_BMSK                               0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_SHFT                                     0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_TIE1_BMSK                                               0x2000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_TIE1_SHFT                                                    0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_TIE0_BMSK                                               0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_TIE0_SHFT                                                    0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UPIE_BMSK                                                 0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UPIE_SHFT                                                    0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UAIE_BMSK                                                 0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UAIE_SHFT                                                    0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_NAKE_BMSK                                                 0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_NAKE_SHFT                                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ULPIE_BMSK                                                  0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_ULPIE_SHFT                                                    0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SLE_BMSK                                                    0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SLE_SHFT                                                      0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SRE_BMSK                                                     0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SRE_SHFT                                                      0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_URE_BMSK                                                     0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_URE_SHFT                                                      0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_AAE_BMSK                                                     0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_AAE_SHFT                                                      0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SEE_BMSK                                                     0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_SEE_SHFT                                                      0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_FRE_BMSK                                                      0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_FRE_SHFT                                                      0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_PCE_BMSK                                                      0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_PCE_SHFT                                                      0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UEE_BMSK                                                      0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UEE_SHFT                                                      0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UE_BMSK                                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBINTR_UE_SHFT                                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_ADDR                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000014c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_PHYS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000014c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_OFFS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000014c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_RMSK                                                       0x3fff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_POR                                                    0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_FRINDEX_BMSK                                               0x3fff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_FRINDEX_FRINDEX_SHFT                                                  0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_PHYS                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_OFFS                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_RMSK                                          0xfffff000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_POR                                           0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK                                  0xfffff000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT                                         0xc

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_RMSK                                                0xff000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_USBADR_BMSK                                         0xfe000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_USBADR_SHFT                                               0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_USBADRA_BMSK                                         0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_DEVICEADDR_USBADRA_SHFT                                              0x18

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_PHYS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_OFFS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_RMSK                                             0xffffffe0
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_POR                                              0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK                                     0xffffffe0
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT                                            0x5

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_PHYS                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_OFFS                                          (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_RMSK                                          0xfffff800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_POR                                           0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK                                   0xfffff800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT                                          0xb

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000015c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000015c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000015c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_RMSK                                                    0x7f000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_POR                                                     0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_TTHA_BMSK                                               0x7f000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TTCTRL_TTHA_SHFT                                                     0x18

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000160)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000160)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000160)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_RMSK                                                     0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_POR                                                  0x00001010
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK                                            0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT                                               0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK                                              0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT                                               0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_ADDR                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000164)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_PHYS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000164)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_OFFS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000164)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_RMSK                                                0x3f1fff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_POR                                               0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK                                    0x3f0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT                                        0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK                                      0x1f00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT                                         0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK                                            0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT                                             0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000170)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_PHYS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000170)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_OFFS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000170)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_RMSK                                             0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_POR                                              0x08000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK                                      0x80000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT                                            0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK                                     0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT                                           0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK                                      0x20000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT                                            0x1d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_BMSK                                   0x10000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_SHFT                                         0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK                                       0x8000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT                                            0x1b
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK                                     0x7000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT                                          0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK                                      0xff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT                                          0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK                                       0xff00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT                                          0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK                                         0xff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT                                          0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000178)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000178)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000178)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_RMSK                                                  0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_POR                                                   0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_EPTN_BMSK                                             0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_EPTN_SHFT                                                   0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_EPRN_BMSK                                                 0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAK_EPRN_SHFT                                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x0000017c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000017c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000017c)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_RMSK                                                0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK                                          0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT                                                0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK                                              0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTNAKEN_EPRNE_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000184)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000184)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000184)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_RMSK                                                    0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_POR                                                     0xcc000004
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTS_BMSK                                                0xc0000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTS_SHFT                                                      0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_STS_BMSK                                                0x20000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_STS_SHFT                                                      0x1d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTW_BMSK                                                0x10000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTW_SHFT                                                      0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PSPD_BMSK                                                0xc000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PSPD_SHFT                                                     0x1a
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_SPRT_BMSK                                                0x2000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_SPRT_SHFT                                                     0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PFSC_BMSK                                                0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PFSC_SHFT                                                     0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PHCD_BMSK                                                 0x800000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PHCD_SHFT                                                     0x17
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKOC_BMSK                                                 0x400000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKOC_SHFT                                                     0x16
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKDS_BMSK                                                 0x200000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKDS_SHFT                                                     0x15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKCN_BMSK                                                 0x100000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_WKCN_SHFT                                                     0x14
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTC_BMSK                                                   0xf0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PTC_SHFT                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PIC_BMSK                                                    0xc000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PIC_SHFT                                                       0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PO_BMSK                                                     0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PO_SHFT                                                        0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PP_BMSK                                                     0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PP_SHFT                                                        0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_LS_BMSK                                                      0xc00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_LS_SHFT                                                        0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_HSP_BMSK                                                     0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_HSP_SHFT                                                       0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PR_BMSK                                                      0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PR_SHFT                                                        0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_SUSP_BMSK                                                     0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_SUSP_SHFT                                                      0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_FPR_BMSK                                                      0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_FPR_SHFT                                                       0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OCC_BMSK                                                      0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OCC_SHFT                                                       0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OCA_BMSK                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_OCA_SHFT                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PEC_BMSK                                                       0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PEC_SHFT                                                       0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PE_BMSK                                                        0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_PE_SHFT                                                        0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_CSC_BMSK                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_CSC_SHFT                                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_CCS_BMSK                                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PORTSC_CCS_SHFT                                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ADDR                                                     (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001a4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_PHYS                                                     (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_OFFS                                                     (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_RMSK                                                     0x7f7f7fff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_POR                                                      0x00000e20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPIE_BMSK                                                0x40000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPIE_SHFT                                                      0x1e
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MSE_BMSK                                          0x20000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MSE_SHFT                                                0x1d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSEIE_BMSK                                               0x10000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSEIE_SHFT                                                     0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSVIE_BMSK                                                0x8000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSVIE_SHFT                                                     0x1b
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASVIE_BMSK                                                0x4000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASVIE_SHFT                                                     0x1a
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVVIE_BMSK                                                0x2000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVVIE_SHFT                                                     0x19
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDIE_BMSK                                                 0x1000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDIE_SHFT                                                      0x18
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPIS_BMSK                                                  0x400000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPIS_SHFT                                                      0x16
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MSS_BMSK                                            0x200000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MSS_SHFT                                                0x15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSEIS_BMSK                                                 0x100000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSEIS_SHFT                                                     0x14
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSVIS_BMSK                                                  0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSVIS_SHFT                                                     0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASVIS_BMSK                                                  0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASVIS_SHFT                                                     0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVVIS_BMSK                                                  0x20000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVVIS_SHFT                                                     0x11
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDIS_BMSK                                                   0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDIS_SHFT                                                      0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPS_BMSK                                                     0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DPS_SHFT                                                        0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MST_BMSK                                              0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_FIELD_1MST_SHFT                                                 0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSE_BMSK                                                     0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSE_SHFT                                                        0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSV_BMSK                                                      0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_BSV_SHFT                                                        0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASV_BMSK                                                      0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ASV_SHFT                                                        0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVV_BMSK                                                      0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_AVV_SHFT                                                        0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ID_BMSK                                                       0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_ID_SHFT                                                         0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HABA_BMSK                                                      0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HABA_SHFT                                                       0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HADP_BMSK                                                      0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HADP_SHFT                                                       0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDPU_BMSK                                                      0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_IDPU_SHFT                                                       0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DP_BMSK                                                        0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_DP_SHFT                                                         0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_OT_BMSK                                                         0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_OT_SHFT                                                         0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HAAR_BMSK                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_HAAR_SHFT                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_VC_BMSK                                                         0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_VC_SHFT                                                         0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_VD_BMSK                                                         0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_OTGSC_VD_SHFT                                                         0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ADDR                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001a8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_PHYS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_OFFS                                                   (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_RMSK                                                         0x3f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_POR                                                    0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_VBPS_BMSK                                                    0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_VBPS_SHFT                                                     0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_SDIS_BMSK                                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_SDIS_SHFT                                                     0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_SLOM_BMSK                                                     0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_SLOM_SHFT                                                     0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ES_BMSK                                                       0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_ES_SHFT                                                       0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_CM_BMSK                                                       0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_USBMODE_CM_SHFT                                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001ac)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_PHYS                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001ac)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_OFFS                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001ac)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_RMSK                                                0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_POR                                             0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_BMSK                                 0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHFT                                    0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001b0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_RMSK                                                0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_PETB_BMSK                                           0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_PETB_SHFT                                                 0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_PERB_BMSK                                               0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTPRIME_PERB_SHFT                                                  0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001b4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b4)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_RMSK                                                0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_FETB_BMSK                                           0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_FETB_SHFT                                                 0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_FERB_BMSK                                               0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTFLUSH_FERB_SHFT                                                  0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ADDR                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001b8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_PHYS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_OFFS                                                 (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b8)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_RMSK                                                 0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_POR                                                  0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ETBR_BMSK                                            0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ETBR_SHFT                                                  0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ERBR_BMSK                                                0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTSTAT_ERBR_SHFT                                                   0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001bc)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_PHYS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001bc)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_OFFS                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001bc)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_RMSK                                             0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_POR                                              0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK                                        0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT                                              0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK                                            0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT                                               0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RMSK                                                  0x8d008d
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_POR                                                 0x00800080
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXE_BMSK                                              0x800000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXE_SHFT                                                  0x17
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXT_BMSK                                               0xc0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXT_SHFT                                                  0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXS_BMSK                                               0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_TXS_SHFT                                                  0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXE_BMSK                                                  0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXE_SHFT                                                   0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXT_BMSK                                                   0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXT_SHFT                                                   0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXS_BMSK                                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRL0_RXS_SHFT                                                   0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(n)                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_PHYS(n)                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_OFFS(n)                                             (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RMSK                                                  0xef00ef
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_MAXn                                                        15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_POR                                                 0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(n), HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(n), mask)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(n),val)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(n),mask,val,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_INI(n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXE_BMSK                                              0x800000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXE_SHFT                                                  0x17
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXR_BMSK                                              0x400000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXR_SHFT                                                  0x16
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXI_BMSK                                              0x200000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXI_SHFT                                                  0x15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXT_BMSK                                               0xc0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXT_SHFT                                                  0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXD_BMSK                                               0x20000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXD_SHFT                                                  0x11
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXS_BMSK                                               0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_TXS_SHFT                                                  0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXE_BMSK                                                  0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXE_SHFT                                                   0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXR_BMSK                                                  0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXR_SHFT                                                   0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXI_BMSK                                                  0x20
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXI_SHFT                                                   0x5
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXT_BMSK                                                   0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXT_SHFT                                                   0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXD_BMSK                                                   0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXD_SHFT                                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXS_BMSK                                                   0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPTCTRLn_RXS_SHFT                                                   0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n)                                         (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x000001fc + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_PHYS(n)                                         (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001fc + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_OFFS(n)                                         (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001fc + 0x4 * (n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_RMSK                                              0x1f001f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_MAXn                                                    15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_POR                                             0x001f001f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n), HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n), mask)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n),val)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n),mask,val,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_INI(n))
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_TX_PIPE_ID_BMSK                                   0x1f0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_TX_PIPE_ID_SHFT                                       0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_RX_PIPE_ID_BMSK                                       0x1f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_ENDPT_PIPE_IDn_RX_PIPE_ID_SHFT                                        0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000240)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000240)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000240)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_RMSK                                                    0x3effff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_POR                                                   0x000c3c3a
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                  0x200000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                      0x15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_BMSK                                0x100000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_SHFT                                    0x14
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_BMSK                            0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_SHFT                               0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_CLK_EN_BMSK                                0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_CLK_EN_SHFT                                   0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_REFCLKOUT_EN_BMSK                               0x20000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_REFCLKOUT_EN_SHFT                                  0x11
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_USE_CLKCORE_BMSK                                 0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_USE_CLKCORE_SHFT                                    0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_BMSK                                  0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_SHFT                                     0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_BMSK                             0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_SHFT                                0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_CLAMP_EN_BMSK                               0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_CLAMP_EN_SHFT                                  0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_BMSK                        0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_SHFT                          0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_PHY_MPM_HV_CLAMP_EN_BMSK                                   0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_PHY_MPM_HV_CLAMP_EN_SHFT                                     0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_BMSK                           0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_SHFT                             0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_INTEN_BMSK                                   0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_INTEN_SHFT                                     0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_POR_BMSK                                      0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_POR_SHFT                                       0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_FSEL_BMSK                                          0x70
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_FSEL_SHFT                                           0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_HOST_PORTCTRL_FORCE_SUSEN_BMSK                               0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_HOST_PORTCTRL_FORCE_SUSEN_SHFT                               0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_SIDDQ_BMSK                                          0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_SIDDQ_SHFT                                          0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_RETEN_BMSK                                          0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_RETEN_SHFT                                          0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_POR_BMSK                                            0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_CTRL_USB2_PHY_POR_SHFT                                            0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000244)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000244)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000244)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_RMSK                                                  0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_POR                                                   0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_BMSK                                  0xffff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_SHFT                                        0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_BMSK                                      0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_SHFT                                         0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000248)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000248)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000248)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_RMSK                                                     0xfffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_POR                                                   0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_HBM_BMSK                                             0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_HBM_SHFT                                                0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_BMSK                                        0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_SHFT                                           0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SECURITY_BMSK                                        0x20000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SECURITY_SHFT                                           0x11
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_FS_SIE_BMSK                                          0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_FS_SIE_SHFT                                             0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_BMSK                                      0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_SHFT                                         0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_LPM_SUPPORT_BMSK                                          0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_LPM_SUPPORT_SHFT                                             0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_BMSK                                        0x3e00
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_SHFT                                           0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_MAX_PIPES_BMSK                                             0x1f8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_MAX_PIPES_SHFT                                               0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SPS_BMSK                                                 0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_SPS_SHFT                                                 0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_HSIC_BMSK                                                0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_USE_HSIC_SHFT                                                0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_BMSK                                       0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_ADDR                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000270)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_PHYS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000270)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_OFFS                                                    (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000270)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_RMSK                                                    0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_POR                                                     0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_MAJOR_BMSK                                              0xf0000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_MAJOR_SHFT                                                    0x1c
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_MINOR_BMSK                                               0xfff0000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_MINOR_SHFT                                                    0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_STEP_BMSK                                                   0xffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_HW_VER_STEP_SHFT                                                      0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_ADDR                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000274)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_PHYS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000274)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_OFFS                                                  (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000274)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_RMSK                                                  0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_POR                                                   0x00000000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_BMSK                                    0xffffffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_SHFT                                           0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_ADDR                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000278)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_PHYS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000278)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_OFFS                                              (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000278)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_RMSK                                                0x3dfff7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_POR                                               0x000c3c32
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_2_BMSK            0x200000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_2_SHFT                0x15
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSE_INTEN_2_BMSK                          0x100000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSE_INTEN_2_SHFT                              0x14
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_2_BMSK                      0x80000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_2_SHFT                         0x13
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_CLK_EN_2_BMSK                          0x40000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_CLK_EN_2_SHFT                             0x12
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_HS2_DIG_CLAMP_N_2_BMSK                      0x10000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_HS2_DIG_CLAMP_N_2_SHFT                         0x10
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_USE_CLKCORE_2_BMSK                           0x8000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_USE_CLKCORE_2_SHFT                              0xf
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSE_INTEN_2_BMSK                            0x4000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSE_INTEN_2_SHFT                               0xe
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_2_BMSK                       0x2000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_2_SHFT                          0xd
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_CLAMP_EN_2_BMSK                         0x1000
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_CLAMP_EN_2_SHFT                            0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_2_BMSK                  0x800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_2_SHFT                    0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_PHY_MPM_HV_CLAMP_EN_2_BMSK                             0x400
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_PHY_MPM_HV_CLAMP_EN_2_SHFT                               0xa
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_2_BMSK                     0x200
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_2_SHFT                       0x9
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_INTEN_2_BMSK                             0x100
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_INTEN_2_SHFT                               0x8
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_POR_2_BMSK                                0x80
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_POR_2_SHFT                                 0x7
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_FSEL_2_BMSK                                    0x70
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_FSEL_2_SHFT                                     0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_SIDDQ_2_BMSK                                    0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_SIDDQ_2_SHFT                                    0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_RETEN_2_BMSK                                    0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_RETEN_2_SHFT                                    0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_POR_2_BMSK                                      0x1
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_POR_2_SHFT                                      0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_ADDR                                               (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000340)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_PHYS                                               (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000340)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_OFFS                                               (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000340)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_RMSK                                                 0x3fffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_POR                                                0x000b40a2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_BMSK                                 0x3ff800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_SHFT                                      0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_BMSK                                    0x7ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_SHFT                                      0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_ADDR                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000344)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_PHYS                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000344)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_OFFS                                            (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000344)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_RMSK                                              0x3fffff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_POR                                             0x002b20cc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_BMSK                                  0x3ff800
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_SHFT                                       0xb
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_BMSK                                     0x7ff
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_ADDR                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE      + 0x00000348)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_PHYS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000348)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_OFFS                                                (PERIPH_SS_USB1_HS_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000348)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_RMSK                                                      0x7f
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_POR                                                 0x00000028
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_ADDR, HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_RMSK)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_ADDR, m)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_ADDR,v)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_ADDR,m,v,HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_IN)
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_BMSK                                     0x40
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_SHFT                                      0x6
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_BMSK                                    0x30
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_SHFT                                     0x4
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_BMSK                                     0xc
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_SHFT                                     0x2
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_BMSK                           0x3
#define HWIO_PERIPH_SS_USB1_HS_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE                                                (PERIPH_SS_BASE      + 0x00200000)
#define PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS                                           (PERIPH_SS_BASE_PHYS + 0x00200000)
#define PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS                                           0x00200000

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_ADDR                                                 (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000000)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_PHYS                                                 (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000000)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_OFFS                                                 (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000000)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_RMSK                                                   0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_POR                                                  0x0042fa05
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_REVISON_BMSK                                           0xff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_REVISON_SHFT                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_NID_BMSK                                                 0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_NID_SHFT                                                    0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_ID_BMSK                                                    0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ID_ID_SHFT                                                     0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000004)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000004)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000004)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_RMSK                                               0x3ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_POR                                           0x000001c2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_BMSK                                            0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_SHFT                                              0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_BMSK                                          0x1c0
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_SHFT                                            0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_BMSK                                           0x30
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_SHFT                                            0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_BMSK                                             0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_SHFT                                             0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_BMSK                                            0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_SHFT                                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_BMSK                                              0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_SHFT                                              0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000008)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000008)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000008)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_RMSK                                             0xffff000f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_POR                                              0x10020001
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_BMSK                                       0xff000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_SHFT                                             0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_BMSK                                         0xff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_SHFT                                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_BMSK                                              0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_SHFT                                              0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_HC_BMSK                                                 0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWHOST_HC_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000000c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000000c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000000c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_RMSK                                                 0x3f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_POR                                            0x00000021
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_BMSK                                           0x3e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_SHFT                                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_BMSK                                               0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_SHFT                                               0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000010)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_PHYS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000010)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_OFFS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000010)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_RMSK                                            0x80ffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_POR                                             0x80070b08
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_BMSK                                      0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_SHFT                                            0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK                                    0xff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT                                        0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_BMSK                                          0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_SHFT                                             0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_BMSK                                          0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_SHFT                                           0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000014)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_PHYS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000014)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_OFFS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000014)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RMSK                                                0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_POR                                             0x00000808
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_BMSK                                         0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_SHFT                                            0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_BMSK                                         0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_SHFT                                          0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n)                                    (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_PHYS(n)                                    (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OFFS(n)                                    (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_RMSK                                       0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_MAXn                                               15
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_POR                                        0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), mask)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),val)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),mask,val,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INI(n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_BMSK                      0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_SHFT                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000080)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000080)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000080)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_RMSK                                           0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK                                     0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT                                          0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000084)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_PHYS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000084)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OFFS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000084)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_RMSK                                       0xc1ffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_POR                                        0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK                                0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT                                      0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK                                0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT                                      0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK                                0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT                                     0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK                                  0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000088)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000088)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000088)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_RMSK                                           0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK                                     0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT                                          0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000008c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_PHYS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000008c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OFFS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000008c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_RMSK                                       0xc1ffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_POR                                        0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK                                0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT                                      0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK                                0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT                                      0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK                                0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT                                     0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK                                  0xffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000090)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000090)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000090)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_RMSK                                                 0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_POR                                           0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK                                       0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000094)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000094)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000094)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_RMSK                                                  0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_POR                                            0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK                                     0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT                                     0x1

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000098)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000098)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000098)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_RMSK                                           0x8000001f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_POR                                            0x00000008
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK                      0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_SHFT                            0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_BMSK                                   0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_SHFT                                    0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK                                       0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT                                       0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK                               0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT                               0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK                                      0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT                                      0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000009c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000009c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000009c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_RMSK                                         0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_POR                                          0x00000830
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_BMSK                  0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_SHFT                        0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_BMSK                               0x8000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_SHFT                                  0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_BMSK                               0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_SHFT                                  0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_BMSK                             0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_SHFT                                0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_BMSK                              0x1000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_SHFT                                 0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_BMSK                                   0x800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_SHFT                                     0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_BMSK                                0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_SHFT                                  0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK                      0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT                        0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK                       0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT                         0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_BMSK                          0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_SHFT                           0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK                       0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_SHFT                        0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_BMSK                                0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_SHFT                                 0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_BMSK                       0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_SHFT                        0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_BMSK                                0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_SHFT                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000000a0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_PHYS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000000a0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OFFS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000000a0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RMSK                                          0xfffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_POR                                        0x000aff60
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_BMSK                   0x80000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_SHFT                      0x13
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_BMSK             0x40000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_SHFT                0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_BMSK                                 0x20000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_SHFT                                    0x11
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_BMSK                          0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_SHFT                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_BMSK                      0x8000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_SHFT                         0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_BMSK                   0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_SHFT                      0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_BMSK                  0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_SHFT                     0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_BMSK                  0x1000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_SHFT                     0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_BMSK                           0x800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_SHFT                             0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_BMSK                             0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_SHFT                               0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_BMSK                              0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_SHFT                                0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_BMSK                             0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_SHFT                               0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_BMSK                            0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_SHFT                             0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_BMSK                          0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_SHFT                           0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_BMSK                                 0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_SHFT                                  0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_BMSK                                    0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_SHFT                                     0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_BMSK                                    0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_SHFT                                    0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_BMSK                          0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_SHFT                          0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_BMSK                            0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_SHFT                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_BMSK                             0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_SHFT                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000100)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000100)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000100)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_RMSK                                          0xffff00ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_POR                                           0x01000040
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_BMSK                               0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_SHFT                                     0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK                                      0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000104)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000104)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000104)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_RMSK                                           0xff1ff1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_POR                                           0x00010011
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_BMSK                                      0xf000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_SHFT                                           0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_BMSK                                      0xf00000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_SHFT                                          0x14
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_BMSK                                          0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_SHFT                                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_BMSK                                         0xf000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_SHFT                                            0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_BMSK                                         0xf00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_SHFT                                           0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_BMSK                                            0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_SHFT                                             0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK                                         0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT                                         0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000108)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000108)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000108)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_RMSK                                              0xfff7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_POR                                           0x00000006
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_BMSK                                         0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_SHFT                                            0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_BMSK                                            0xf0
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_SHFT                                             0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_BMSK                                             0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_SHFT                                             0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_BMSK                                             0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_SHFT                                             0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_BMSK                                             0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_SHFT                                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000120)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000120)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000120)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_RMSK                                             0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_POR                                          0x00000001
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK                                  0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT                                     0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000124)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000124)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000124)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_RMSK                                               0x19f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_POR                                           0x00000190
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_BMSK                                            0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_SHFT                                              0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_BMSK                                             0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_SHFT                                              0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_BMSK                                            0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_SHFT                                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000140)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000140)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000140)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RMSK                                             0xfeffebff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_POR                                              0x00080000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RST_CTRL_BMSK                                    0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RST_CTRL_SHFT                                          0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK                               0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT                                     0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK                             0x20000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT                                   0x1d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK                         0x10000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT                               0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK                      0x8000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_SHFT                           0x1b
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_BMSK                           0x4000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_SHFT                                0x1a
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK                                0x2000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT                                     0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_BMSK                                           0xff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_SHFT                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_BMSK                                             0x8000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_SHFT                                                0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_BMSK                                           0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_SHFT                                              0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_BMSK                                            0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_SHFT                                               0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_BMSK                                             0x800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_SHFT                                               0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_BMSK                                              0x300
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_SHFT                                                0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_LR_BMSK                                                0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_LR_SHFT                                                 0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_BMSK                                               0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_SHFT                                                0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_BMSK                                               0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_SHFT                                                0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_BMSK                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_SHFT                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS_BMSK                                                 0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_FS_SHFT                                                 0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RST_BMSK                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RST_SHFT                                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RS_BMSK                                                 0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBCMD_RS_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000144)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000144)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000144)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_RMSK                                             0xfb0df5ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_POR                                              0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_BMSK                                   0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_SHFT                                         0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_BMSK                            0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_SHFT                                  0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_BMSK                                0x20000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_SHFT                                      0x1d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_ALT_INT_BMSK                                 0x10000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_ALT_INT_SHFT                                       0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_BMSK                            0x8000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_SHFT                                 0x1b
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_BMSK                                          0x2000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_SHFT                                               0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_BMSK                                          0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_SHFT                                               0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_BMSK                                            0x80000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_SHFT                                               0x13
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_BMSK                                            0x40000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_SHFT                                               0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_BMSK                                           0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_SHFT                                              0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_AS_BMSK                                              0x8000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_AS_SHFT                                                 0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PS_BMSK                                              0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PS_SHFT                                                 0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_BMSK                                             0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_SHFT                                                0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_BMSK                                             0x1000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_SHFT                                                0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_BMSK                                            0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_SHFT                                              0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_BMSK                                              0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_SHFT                                                0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_BMSK                                               0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_SHFT                                                0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_URI_BMSK                                               0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_URI_SHFT                                                0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_BMSK                                               0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_SHFT                                                0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_BMSK                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_SHFT                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_BMSK                                                0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_SHFT                                                0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_BMSK                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_SHFT                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_BMSK                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_SHFT                                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UI_BMSK                                                 0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBSTS_UI_SHFT                                                 0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000148)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_PHYS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000148)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_OFFS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000148)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_RMSK                                            0xc30d05ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_POR                                             0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK                               0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT                                     0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_BMSK                        0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_SHFT                              0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_BMSK                                        0x2000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_SHFT                                             0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_BMSK                                        0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_SHFT                                             0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_BMSK                                          0x80000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_SHFT                                             0x13
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_BMSK                                          0x40000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_SHFT                                             0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_BMSK                                          0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_SHFT                                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_BMSK                                           0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_SHFT                                             0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_BMSK                                             0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_SHFT                                               0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_BMSK                                              0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_SHFT                                               0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_URE_BMSK                                              0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_URE_SHFT                                               0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_BMSK                                              0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_SHFT                                               0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_BMSK                                              0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_SHFT                                               0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_BMSK                                               0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_SHFT                                               0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_BMSK                                               0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_SHFT                                               0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_BMSK                                               0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_SHFT                                               0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UE_BMSK                                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBINTR_UE_SHFT                                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000014c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_PHYS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000014c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_OFFS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000014c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_RMSK                                                0x3fff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_POR                                             0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_BMSK                                        0x3fff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_SHFT                                           0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PHYS                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OFFS                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_RMSK                                   0xfffff000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_POR                                    0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK                           0xfffff000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT                                  0xc

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_RMSK                                         0xff000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_BMSK                                  0xfe000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_SHFT                                        0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_BMSK                                  0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_SHFT                                       0x18

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_PHYS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OFFS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_RMSK                                      0xffffffe0
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_POR                                       0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK                              0xffffffe0
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT                                     0x5

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_PHYS                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OFFS                                   (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_RMSK                                   0xfffff800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_POR                                    0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK                            0xfffff800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT                                   0xb

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000015c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000015c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000015c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_RMSK                                             0x7f000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_POR                                              0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_BMSK                                        0x7f000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_SHFT                                              0x18

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000160)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000160)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000160)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RMSK                                              0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_POR                                           0x00001010
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK                                     0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT                                        0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK                                       0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT                                        0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000164)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_PHYS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000164)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OFFS                                       (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000164)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_RMSK                                         0x3f1fff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_POR                                        0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK                             0x3f0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT                                 0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK                               0x1f00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT                                  0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK                                     0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT                                      0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000170)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_PHYS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000170)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OFFS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000170)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_RMSK                                      0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_POR                                       0x08000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK                               0x80000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT                                     0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK                              0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT                                    0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK                               0x20000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT                                     0x1d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_BMSK                            0x10000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_SHFT                                  0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK                                0x8000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT                                     0x1b
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK                              0x7000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT                                   0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK                               0xff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT                                   0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK                                0xff00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT                                   0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK                                  0xff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT                                   0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000178)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000178)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000178)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_RMSK                                           0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_POR                                            0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_BMSK                                      0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_SHFT                                            0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_BMSK                                          0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_SHFT                                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000017c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000017c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000017c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_RMSK                                         0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK                                   0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT                                         0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK                                       0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_SHFT                                          0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000184)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000184)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000184)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_RMSK                                             0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_POR                                              0xcc000004
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_BMSK                                         0xc0000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_SHFT                                               0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_STS_BMSK                                         0x20000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_STS_SHFT                                               0x1d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_BMSK                                         0x10000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_SHFT                                               0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_BMSK                                         0xc000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_SHFT                                              0x1a
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_SPRT_BMSK                                         0x2000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_SPRT_SHFT                                              0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_BMSK                                         0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_SHFT                                              0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_BMSK                                          0x800000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_SHFT                                              0x17
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_BMSK                                          0x400000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_SHFT                                              0x16
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_BMSK                                          0x200000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_SHFT                                              0x15
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_BMSK                                          0x100000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_SHFT                                              0x14
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_BMSK                                            0xf0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_SHFT                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_BMSK                                             0xc000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_SHFT                                                0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PO_BMSK                                              0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PO_SHFT                                                 0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PP_BMSK                                              0x1000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PP_SHFT                                                 0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_LS_BMSK                                               0xc00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_LS_SHFT                                                 0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_BMSK                                              0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_SHFT                                                0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PR_BMSK                                               0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PR_SHFT                                                 0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_BMSK                                              0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_SHFT                                               0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_BMSK                                               0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_SHFT                                                0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_BMSK                                               0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_SHFT                                                0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_BMSK                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_SHFT                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_BMSK                                                0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_SHFT                                                0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PE_BMSK                                                 0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_PE_SHFT                                                 0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_BMSK                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_SHFT                                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_BMSK                                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_SHFT                                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR                                              (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001a4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_PHYS                                              (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_OFFS                                              (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_RMSK                                              0x7f7f7fff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_POR                                               0x00000e20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_BMSK                                         0x40000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_SHFT                                               0x1e
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_BMSK                                   0x20000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_SHFT                                         0x1d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_BMSK                                        0x10000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_SHFT                                              0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_BMSK                                         0x8000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_SHFT                                              0x1b
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_BMSK                                         0x4000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_SHFT                                              0x1a
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_BMSK                                         0x2000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_SHFT                                              0x19
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_BMSK                                          0x1000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_SHFT                                               0x18
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_BMSK                                           0x400000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_SHFT                                               0x16
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_BMSK                                     0x200000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_SHFT                                         0x15
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_BMSK                                          0x100000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_SHFT                                              0x14
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_BMSK                                           0x80000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_SHFT                                              0x13
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_BMSK                                           0x40000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_SHFT                                              0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_BMSK                                           0x20000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_SHFT                                              0x11
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_BMSK                                            0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_SHFT                                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_BMSK                                              0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_SHFT                                                 0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_BMSK                                       0x2000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_SHFT                                          0xd
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_BMSK                                              0x1000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_SHFT                                                 0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_BMSK                                               0x800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_SHFT                                                 0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_BMSK                                               0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_SHFT                                                 0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_BMSK                                               0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_SHFT                                                 0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ID_BMSK                                                0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_ID_SHFT                                                  0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_BMSK                                               0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_SHFT                                                0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_BMSK                                               0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_SHFT                                                0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_BMSK                                               0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_SHFT                                                0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DP_BMSK                                                 0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_DP_SHFT                                                  0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_OT_BMSK                                                  0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_OT_SHFT                                                  0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_BMSK                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_SHFT                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_VC_BMSK                                                  0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_VC_SHFT                                                  0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_VD_BMSK                                                  0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_OTGSC_VD_SHFT                                                  0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001a8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_PHYS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_OFFS                                            (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_RMSK                                                  0x3f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_POR                                             0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_BMSK                                             0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_SHFT                                              0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_BMSK                                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_SHFT                                              0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_BMSK                                              0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_SHFT                                              0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ES_BMSK                                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_ES_SHFT                                                0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_CM_BMSK                                                0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_USBMODE_CM_SHFT                                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001ac)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_PHYS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001ac)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OFFS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001ac)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_RMSK                                         0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_POR                                      0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_BMSK                          0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHFT                             0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_RMSK                                         0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_BMSK                                    0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_SHFT                                          0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_BMSK                                        0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_SHFT                                           0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b4)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_RMSK                                         0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_BMSK                                    0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_SHFT                                          0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_BMSK                                        0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_SHFT                                           0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b8)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_RMSK                                          0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_POR                                           0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_BMSK                                     0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_SHFT                                           0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_BMSK                                         0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_SHFT                                            0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001bc)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_PHYS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001bc)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OFFS                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001bc)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_RMSK                                      0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_POR                                       0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK                                 0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT                                       0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK                                     0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT                                        0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RMSK                                           0x8d008d
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_POR                                          0x00800080
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_BMSK                                       0x800000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_SHFT                                           0x17
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_BMSK                                        0xc0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_SHFT                                           0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_BMSK                                        0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_SHFT                                           0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_BMSK                                           0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_SHFT                                            0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_BMSK                                            0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_SHFT                                            0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_BMSK                                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_SHFT                                            0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n)                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_PHYS(n)                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OFFS(n)                                      (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0 + 0x4 * (n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RMSK                                           0xef00ef
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_MAXn                                                 15
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_POR                                          0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INI(n)        \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n), HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INMI(n,mask)    \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n), mask)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OUTI(n,val)    \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n),val)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n),mask,val,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INI(n))
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_BMSK                                       0x800000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_SHFT                                           0x17
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_BMSK                                       0x400000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_SHFT                                           0x16
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_BMSK                                       0x200000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_SHFT                                           0x15
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_BMSK                                        0xc0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_SHFT                                           0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_BMSK                                        0x20000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_SHFT                                           0x11
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_BMSK                                        0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_SHFT                                           0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_BMSK                                           0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_SHFT                                            0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_BMSK                                           0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_SHFT                                            0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_BMSK                                           0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_SHFT                                            0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_BMSK                                            0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_SHFT                                            0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_BMSK                                            0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_SHFT                                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_BMSK                                            0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_SHFT                                            0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000244)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000244)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000244)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_RMSK                                           0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_POR                                            0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_BMSK                           0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_SHFT                                 0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_BMSK                               0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_SHFT                                  0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000248)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000248)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000248)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_RMSK                                              0xfffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_POR                                            0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HBM_BMSK                                      0x80000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HBM_SHFT                                         0x13
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_BMSK                                 0x40000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_SHFT                                    0x12
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SECURITY_BMSK                                 0x20000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SECURITY_SHFT                                    0x11
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_FS_SIE_BMSK                                   0x10000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_FS_SIE_SHFT                                      0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_BMSK                               0x8000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_SHFT                                  0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_LPM_SUPPORT_BMSK                                   0x4000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_LPM_SUPPORT_SHFT                                      0xe
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_BMSK                                 0x3e00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_SHFT                                    0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_MAX_PIPES_BMSK                                      0x1f8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_MAX_PIPES_SHFT                                        0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_BMSK                                          0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_SHFT                                          0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HSIC_BMSK                                         0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HSIC_SHFT                                         0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_BMSK                                0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_SHFT                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000250)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000250)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000250)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_RMSK                                         0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_POR                                          0x0000ffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_BMSK                     0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_SHFT                           0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EN_BMSK                           0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EN_SHFT                              0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000254)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHYS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000254)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OFFS                                          (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000254)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_RMSK                                               0xfff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_POR                                           0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_PWR_DWN_EN_BMSK                                0x800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_PWR_DWN_EN_SHFT                                  0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHY_LPM_EN_BMSK                                    0x400
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHY_LPM_EN_SHFT                                      0xa
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_AHB_CLK_EN_BMSK                               0x200
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_AHB_CLK_EN_SHFT                                 0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK                           0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_FS_XCVR_CLK_EN_SHFT                             0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_SYS_CLK_EN_BMSK                                0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_SYS_CLK_EN_SHFT                                 0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK                               0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_XCVR_CLK_EN_SHFT                                0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK                            0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_REMOTE_WAKEUP_EN_SHFT                             0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_LPM_EN_BMSK                                         0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_LPM_EN_SHFT                                          0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_BMSK                            0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_SHFT                            0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000258)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_PHYS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000258)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_OFFS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000258)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_RMSK                                        0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_POR                                         0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_BMSK                  0xffff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_SHFT                        0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_BMSK                     0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_SHFT                        0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000025c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_PHYS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000025c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OFFS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000025c)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_RMSK                                            0x1fff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_POR                                         0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_RMT_WKUP_TIME_BMSK                           0x1e00
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_RMT_WKUP_TIME_SHFT                              0x9
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_FPR_BMSK                                      0x100
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_FPR_SHFT                                        0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_BMSK                        0x80
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_SHFT                         0x7
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_BMSK                           0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_SHFT                            0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_GATE_BMSK                                0x20
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_GATE_SHFT                                 0x5
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_FS_XCVR_CLK_GATE_BMSK                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_FS_XCVR_CLK_GATE_SHFT                              0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_SYS_CLK_GATE_BMSK                                  0x8
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_SYS_CLK_GATE_SHFT                                  0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_AHB_CLK_GATE_BMSK                                  0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_AHB_CLK_GATE_SHFT                                  0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_STATE_BMSK                                      0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_STATE_SHFT                                      0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_DEBUG_L1_EN_BMSK                                   0x1
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_DEBUG_L1_EN_SHFT                                   0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000260)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_PHYS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000260)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_OFFS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000260)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_RMSK                                           0x1f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_POR                                      0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK                               0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_BREMOTEWAKE_SHFT                                0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_HIRD_BMSK                                       0xf
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_HIRD_SHFT                                       0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000270)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_PHYS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000270)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_OFFS                                             (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000270)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_RMSK                                             0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_POR                                              0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_MAJOR_BMSK                                       0xf0000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_MAJOR_SHFT                                             0x1c
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_MINOR_BMSK                                        0xfff0000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_MINOR_SHFT                                             0x10
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_STEP_BMSK                                            0xffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_HW_VER_STEP_SHFT                                               0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000274)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_PHYS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000274)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_OFFS                                           (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000274)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_RMSK                                           0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_POR                                            0x00000000
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_BMSK                             0xffffffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_SHFT                                    0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000340)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_PHYS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000340)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OFFS                                        (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000340)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_RMSK                                          0x3fffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_POR                                         0x000b40a2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_BMSK                          0x3ff800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_SHFT                               0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_BMSK                             0x7ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_SHFT                               0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000344)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_PHYS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000344)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OFFS                                     (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000344)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_RMSK                                       0x3fffff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_POR                                      0x002b20cc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_BMSK                           0x3ff800
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_SHFT                                0xb
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_BMSK                              0x7ff
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_SHFT                                0x0

#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000348)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_PHYS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000348)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OFFS                                         (PERIPH_SS_USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000348)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_RMSK                                               0x7f
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_POR                                          0x00000028
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_IN          \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR, HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_RMSK)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_INM(m)      \
        in_dword_masked(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR, m)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OUT(v)      \
        out_dword(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR,v)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR,m,v,HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_IN)
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_BMSK                              0x40
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_SHFT                               0x6
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_BMSK                             0x30
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_SHFT                              0x4
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_BMSK                              0xc
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_SHFT                              0x2
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_BMSK                    0x3
#define HWIO_PERIPH_SS_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_SHFT                    0x0

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                      (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_PHYS                                                                 (CLK_CTL_BASE_PHYS + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                                 0x00000000

#define HWIO_GCC_USB_HS_HSIC_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00000400)
#define HWIO_GCC_USB_HS_HSIC_BCR_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000400)
#define HWIO_GCC_USB_HS_HSIC_BCR_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000400)
#define HWIO_GCC_USB_HS_HSIC_BCR_RMSK                                                                        0x1
#define HWIO_GCC_USB_HS_HSIC_BCR_POR                                                                  0x00000000
#define HWIO_GCC_USB_HS_HSIC_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_HSIC_BCR_ADDR, HWIO_GCC_USB_HS_HSIC_BCR_RMSK)
#define HWIO_GCC_USB_HS_HSIC_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_HSIC_BCR_ADDR, m)
#define HWIO_GCC_USB_HS_HSIC_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_HSIC_BCR_ADDR,v)
#define HWIO_GCC_USB_HS_HSIC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_HSIC_BCR_ADDR,m,v,HWIO_GCC_USB_HS_HSIC_BCR_IN)
#define HWIO_GCC_USB_HS_HSIC_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_USB_HS_HSIC_BCR_BLK_ARES_SHFT                                                               0x0

#define HWIO_GCC_USB_HS_HSIC_GDSCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00000404)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000404)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000404)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RMSK                                                               0xf8ffffff
#define HWIO_GCC_USB_HS_HSIC_GDSCR_POR                                                                0x00222001
#define HWIO_GCC_USB_HS_HSIC_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_HSIC_GDSCR_ADDR, HWIO_GCC_USB_HS_HSIC_GDSCR_RMSK)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_HSIC_GDSCR_ADDR, m)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_HSIC_GDSCR_ADDR,v)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_HSIC_GDSCR_ADDR,m,v,HWIO_GCC_USB_HS_HSIC_GDSCR_IN)
#define HWIO_GCC_USB_HS_HSIC_GDSCR_PWR_ON_BMSK                                                        0x80000000
#define HWIO_GCC_USB_HS_HSIC_GDSCR_PWR_ON_SHFT                                                              0x1f
#define HWIO_GCC_USB_HS_HSIC_GDSCR_GDSC_STATE_BMSK                                                    0x78000000
#define HWIO_GCC_USB_HS_HSIC_GDSCR_GDSC_STATE_SHFT                                                          0x1b
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_REST_WAIT_BMSK                                                    0xf00000
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_REST_WAIT_SHFT                                                        0x14
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_FEW_WAIT_BMSK                                                      0xf0000
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_FEW_WAIT_SHFT                                                         0x10
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLK_DIS_WAIT_BMSK                                                      0xf000
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLK_DIS_WAIT_SHFT                                                         0xc
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RETAIN_FF_ENABLE_BMSK                                                   0x800
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RETAIN_FF_ENABLE_SHFT                                                     0xb
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RESTORE_BMSK                                                            0x400
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RESTORE_SHFT                                                              0xa
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SAVE_BMSK                                                               0x200
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SAVE_SHFT                                                                 0x9
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RETAIN_BMSK                                                             0x100
#define HWIO_GCC_USB_HS_HSIC_GDSCR_RETAIN_SHFT                                                               0x8
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_REST_BMSK                                                             0x80
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_REST_SHFT                                                              0x7
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_FEW_BMSK                                                              0x40
#define HWIO_GCC_USB_HS_HSIC_GDSCR_EN_FEW_SHFT                                                               0x6
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLAMP_IO_BMSK                                                            0x20
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLAMP_IO_SHFT                                                             0x5
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLK_DISABLE_BMSK                                                         0x10
#define HWIO_GCC_USB_HS_HSIC_GDSCR_CLK_DISABLE_SHFT                                                          0x4
#define HWIO_GCC_USB_HS_HSIC_GDSCR_PD_ARES_BMSK                                                              0x8
#define HWIO_GCC_USB_HS_HSIC_GDSCR_PD_ARES_SHFT                                                              0x3
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SW_OVERRIDE_BMSK                                                          0x4
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SW_OVERRIDE_SHFT                                                          0x2
#define HWIO_GCC_USB_HS_HSIC_GDSCR_HW_CONTROL_BMSK                                                           0x2
#define HWIO_GCC_USB_HS_HSIC_GDSCR_HW_CONTROL_SHFT                                                           0x1
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SW_COLLAPSE_BMSK                                                          0x1
#define HWIO_GCC_USB_HS_HSIC_GDSCR_SW_COLLAPSE_SHFT                                                          0x0

#define HWIO_GCC_USB_HSIC_AHB_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00000408)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000408)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000408)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_RMSK                                                               0xf000fff1
#define HWIO_GCC_USB_HSIC_AHB_CBCR_POR                                                                0x8000cff0
#define HWIO_GCC_USB_HSIC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_AHB_CBCR_ADDR, HWIO_GCC_USB_HSIC_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_HSIC_AHB_CBCR_IN)
#define HWIO_GCC_USB_HSIC_AHB_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_USB_HSIC_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                      0x70000000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                            0x1c
#define HWIO_GCC_USB_HSIC_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                              0x8000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                 0xf
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                                 0x4000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                    0xe
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                               0x2000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                  0xd
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                              0x1000
#define HWIO_GCC_USB_HSIC_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                 0xc
#define HWIO_GCC_USB_HSIC_AHB_CBCR_WAKEUP_BMSK                                                             0xf00
#define HWIO_GCC_USB_HSIC_AHB_CBCR_WAKEUP_SHFT                                                               0x8
#define HWIO_GCC_USB_HSIC_AHB_CBCR_SLEEP_BMSK                                                               0xf0
#define HWIO_GCC_USB_HSIC_AHB_CBCR_SLEEP_SHFT                                                                0x4
#define HWIO_GCC_USB_HSIC_AHB_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_USB_HSIC_AHB_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0000041c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0000041c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0000041c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_RMSK                                                        0x80000013
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_POR                                                         0x80000000
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ADDR, HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_IN)
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ROOT_OFF_BMSK                                               0x80000000
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ROOT_OFF_SHFT                                                     0x1f
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                               0x10
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                0x4
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ROOT_EN_BMSK                                                       0x2
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_ROOT_EN_SHFT                                                       0x1
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_UPDATE_BMSK                                                        0x1
#define HWIO_GCC_USB_HSIC_SYSTEM_CMD_RCGR_UPDATE_SHFT                                                        0x0

#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00000420)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000420)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000420)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_RMSK                                                             0x71f
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_POR                                                         0x00000000
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_ADDR, HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_IN)
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_SRC_SEL_BMSK                                                     0x700
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_SRC_SEL_SHFT                                                       0x8
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_SRC_DIV_BMSK                                                      0x1f
#define HWIO_GCC_USB_HSIC_SYSTEM_CFG_RCGR_SRC_DIV_SHFT                                                       0x0

#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000040c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_PHYS                                                            (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0000040c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_OFFS                                                            (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0000040c)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_RMSK                                                            0x80007ff1
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_POR                                                             0x80004ff0
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CBCR_ADDR, HWIO_GCC_USB_HSIC_SYSTEM_CBCR_RMSK)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_SYSTEM_CBCR_ADDR, m)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_SYSTEM_CBCR_ADDR,v)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_SYSTEM_CBCR_ADDR,m,v,HWIO_GCC_USB_HSIC_SYSTEM_CBCR_IN)
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_USB_HSIC_SYSTEM_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_USB_HSIC_CMD_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00000440)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000440)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000440)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_RMSK                                                               0x80000013
#define HWIO_GCC_USB_HSIC_CMD_RCGR_POR                                                                0x80000000
#define HWIO_GCC_USB_HSIC_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_CMD_RCGR_ADDR, HWIO_GCC_USB_HSIC_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_CMD_RCGR_IN)
#define HWIO_GCC_USB_HSIC_CMD_RCGR_ROOT_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_USB_HSIC_CMD_RCGR_ROOT_OFF_SHFT                                                            0x1f
#define HWIO_GCC_USB_HSIC_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                      0x10
#define HWIO_GCC_USB_HSIC_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                       0x4
#define HWIO_GCC_USB_HSIC_CMD_RCGR_ROOT_EN_BMSK                                                              0x2
#define HWIO_GCC_USB_HSIC_CMD_RCGR_ROOT_EN_SHFT                                                              0x1
#define HWIO_GCC_USB_HSIC_CMD_RCGR_UPDATE_BMSK                                                               0x1
#define HWIO_GCC_USB_HSIC_CMD_RCGR_UPDATE_SHFT                                                               0x0

#define HWIO_GCC_USB_HSIC_CFG_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00000444)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000444)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000444)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_RMSK                                                                    0x71f
#define HWIO_GCC_USB_HSIC_CFG_RCGR_POR                                                                0x00000000
#define HWIO_GCC_USB_HSIC_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_CFG_RCGR_ADDR, HWIO_GCC_USB_HSIC_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_CFG_RCGR_IN)
#define HWIO_GCC_USB_HSIC_CFG_RCGR_SRC_SEL_BMSK                                                            0x700
#define HWIO_GCC_USB_HSIC_CFG_RCGR_SRC_SEL_SHFT                                                              0x8
#define HWIO_GCC_USB_HSIC_CFG_RCGR_SRC_DIV_BMSK                                                             0x1f
#define HWIO_GCC_USB_HSIC_CFG_RCGR_SRC_DIV_SHFT                                                              0x0

#define HWIO_GCC_USB_HSIC_CBCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00000410)
#define HWIO_GCC_USB_HSIC_CBCR_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000410)
#define HWIO_GCC_USB_HSIC_CBCR_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000410)
#define HWIO_GCC_USB_HSIC_CBCR_RMSK                                                                   0x80000001
#define HWIO_GCC_USB_HSIC_CBCR_POR                                                                    0x80000000
#define HWIO_GCC_USB_HSIC_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_CBCR_ADDR, HWIO_GCC_USB_HSIC_CBCR_RMSK)
#define HWIO_GCC_USB_HSIC_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_CBCR_ADDR, m)
#define HWIO_GCC_USB_HSIC_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_CBCR_ADDR,v)
#define HWIO_GCC_USB_HSIC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_CBCR_ADDR,m,v,HWIO_GCC_USB_HSIC_CBCR_IN)
#define HWIO_GCC_USB_HSIC_CBCR_CLK_OFF_BMSK                                                           0x80000000
#define HWIO_GCC_USB_HSIC_CBCR_CLK_OFF_SHFT                                                                 0x1f
#define HWIO_GCC_USB_HSIC_CBCR_CLK_ENABLE_BMSK                                                               0x1
#define HWIO_GCC_USB_HSIC_CBCR_CLK_ENABLE_SHFT                                                               0x0

#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00000458)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000458)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000458)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_RMSK                                                        0x80000013
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_POR                                                         0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ADDR, HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_IN)
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ROOT_OFF_BMSK                                               0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ROOT_OFF_SHFT                                                     0x1f
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                               0x10
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                0x4
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ROOT_EN_BMSK                                                       0x2
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_ROOT_EN_SHFT                                                       0x1
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_UPDATE_BMSK                                                        0x1
#define HWIO_GCC_USB_HSIC_IO_CAL_CMD_RCGR_UPDATE_SHFT                                                        0x0

#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0000045c)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0000045c)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0000045c)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_RMSK                                                              0x1f
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_POR                                                         0x00000000
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_ADDR, HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_IN)
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_SRC_DIV_BMSK                                                      0x1f
#define HWIO_GCC_USB_HSIC_IO_CAL_CFG_RCGR_SRC_DIV_SHFT                                                       0x0

#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00000414)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_PHYS                                                            (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000414)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_OFFS                                                            (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000414)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_POR                                                             0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CBCR_ADDR, HWIO_GCC_USB_HSIC_IO_CAL_CBCR_RMSK)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_CBCR_ADDR, m)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_IO_CAL_CBCR_ADDR,v)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_IO_CAL_CBCR_ADDR,m,v,HWIO_GCC_USB_HSIC_IO_CAL_CBCR_IN)
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_USB_HSIC_IO_CAL_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00000418)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_PHYS                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000418)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_OFFS                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000418)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_POR                                                       0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_ADDR, HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_RMSK)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_IN)
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_USB_HSIC_IO_CAL_SLEEP_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_USB_HS_BCR_ADDR                                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00000480)
#define HWIO_GCC_USB_HS_BCR_PHYS                                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000480)
#define HWIO_GCC_USB_HS_BCR_OFFS                                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000480)
#define HWIO_GCC_USB_HS_BCR_RMSK                                                                             0x1
#define HWIO_GCC_USB_HS_BCR_POR                                                                       0x00000000
#define HWIO_GCC_USB_HS_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, HWIO_GCC_USB_HS_BCR_RMSK)
#define HWIO_GCC_USB_HS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, m)
#define HWIO_GCC_USB_HS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_BCR_ADDR,v)
#define HWIO_GCC_USB_HS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_BCR_ADDR,m,v,HWIO_GCC_USB_HS_BCR_IN)
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK                                                                    0x1
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT                                                                    0x0

#define HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00000484)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_PHYS                                                              (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000484)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OFFS                                                              (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000484)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK                                                              0x80007ff1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_POR                                                               0x80004ff0
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CBCR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_SHFT                                                               0x4
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_USB_HS_AHB_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00000488)
#define HWIO_GCC_USB_HS_AHB_CBCR_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000488)
#define HWIO_GCC_USB_HS_AHB_CBCR_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000488)
#define HWIO_GCC_USB_HS_AHB_CBCR_RMSK                                                                 0xf000fff1
#define HWIO_GCC_USB_HS_AHB_CBCR_POR                                                                  0x8000cff0
#define HWIO_GCC_USB_HS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, HWIO_GCC_USB_HS_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_HS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_AHB_CBCR_IN)
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                        0x70000000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                              0x1c
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                                0x8000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                   0xf
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                                   0x4000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                      0xe
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                 0x2000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                    0xd
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                0x1000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                   0xc
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_BMSK                                                               0xf00
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_SHFT                                                                 0x8
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_BMSK                                                                 0xf0
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_SHFT                                                                  0x4
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_SHFT                                                             0x0

#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0000048c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_PHYS                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0000048c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OFFS                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0000048c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK                                                   0x80000001
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_POR                                                    0x80000000
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00000490)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_PHYS                                                          (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000490)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OFFS                                                          (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000490)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK                                                          0x80000013
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_POR                                                           0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_SHFT                                                       0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                 0x10
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                  0x4
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_BMSK                                                         0x2
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_SHFT                                                         0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_BMSK                                                          0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_SHFT                                                          0x0

#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00000494)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_PHYS                                                          (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000494)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OFFS                                                          (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000494)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK                                                               0x71f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_POR                                                           0x00000000
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_BMSK                                                       0x700
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_SHFT                                                         0x8
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_BMSK                                                        0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_SHFT                                                         0x0

#define HWIO_GCC_USB2A_PHY_BCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x000004a8)
#define HWIO_GCC_USB2A_PHY_BCR_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x000004a8)
#define HWIO_GCC_USB2A_PHY_BCR_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x000004a8)
#define HWIO_GCC_USB2A_PHY_BCR_RMSK                                                                          0x1
#define HWIO_GCC_USB2A_PHY_BCR_POR                                                                    0x00000000
#define HWIO_GCC_USB2A_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, HWIO_GCC_USB2A_PHY_BCR_RMSK)
#define HWIO_GCC_USB2A_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_BCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_BCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_BCR_IN)
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_BMSK                                                                 0x1
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_SHFT                                                                 0x0

#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x000004ac)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_PHYS                                                            (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x000004ac)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OFFS                                                            (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x000004ac)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_POR                                                             0x80000000
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_USB2B_PHY_BCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x000004b0)
#define HWIO_GCC_USB2B_PHY_BCR_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x000004b0)
#define HWIO_GCC_USB2B_PHY_BCR_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x000004b0)
#define HWIO_GCC_USB2B_PHY_BCR_RMSK                                                                          0x1
#define HWIO_GCC_USB2B_PHY_BCR_POR                                                                    0x00000000
#define HWIO_GCC_USB2B_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB2B_PHY_BCR_ADDR, HWIO_GCC_USB2B_PHY_BCR_RMSK)
#define HWIO_GCC_USB2B_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2B_PHY_BCR_ADDR, m)
#define HWIO_GCC_USB2B_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2B_PHY_BCR_ADDR,v)
#define HWIO_GCC_USB2B_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2B_PHY_BCR_ADDR,m,v,HWIO_GCC_USB2B_PHY_BCR_IN)
#define HWIO_GCC_USB2B_PHY_BCR_BLK_ARES_BMSK                                                                 0x1
#define HWIO_GCC_USB2B_PHY_BCR_BLK_ARES_SHFT                                                                 0x0

#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x000004b4)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_PHYS                                                            (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x000004b4)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_OFFS                                                            (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x000004b4)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_RMSK                                                            0x80000001
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_POR                                                             0x80000000
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB2B_PHY_SLEEP_CBCR_ADDR, HWIO_GCC_USB2B_PHY_SLEEP_CBCR_RMSK)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2B_PHY_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2B_PHY_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2B_PHY_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_USB2B_PHY_SLEEP_CBCR_IN)
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_USB2B_PHY_SLEEP_CBCR_CLK_ENABLE_SHFT                                                        0x0

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/
 /*No need of the whole module added only those which are needed by USB*/
#define TLMM_CSR_REG_BASE                                                (TLMM_BASE      + 0x00010000)
#define TLMM_CSR_REG_BASE_PHYS                                           (TLMM_BASE_PHYS + 0x00010000)
#define TLMM_CSR_REG_BASE_OFFS                                           0x00010000

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                      (TLMM_CSR_REG_BASE      + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_PHYS(n)                                      (TLMM_CSR_REG_BASE_PHYS + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_OFFS(n)                                      (TLMM_CSR_REG_BASE_OFFS + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                              0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                145
#define HWIO_TLMM_GPIO_CFGn_POR                                          0x00000001
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PHYS                                (TLMM_CSR_REG_BASE_PHYS + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OFFS                                (TLMM_CSR_REG_BASE_OFFS + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                   0x3ffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_POR                                 0x00035800
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
		
/*USB Based Definitions*/
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                 0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                   0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                      0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                       0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                      0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                      0x0       
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                         0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                           0x9

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE      + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_PHYS                  (MPM2_MPM_BASE_PHYS + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_OFFS                  0x0000b000

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_PHYS                 (MPM2_PSHOLD_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_OFFS                 (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00020000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS                                                                                      (CORE_TOP_CSR_BASE_PHYS + 0x00020000)
#define TCSR_TCSR_REGS_REG_BASE_OFFS                                                                                       0x00020000

#define HWIO_TCSR_SYS_POWER_CTRL_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0000a000)
#define HWIO_TCSR_SYS_POWER_CTRL_PHYS                                                                                     (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x0000a000)
#define HWIO_TCSR_SYS_POWER_CTRL_OFFS                                                                                     (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x0000a000)
#define HWIO_TCSR_SYS_POWER_CTRL_RMSK                                                                                      0xffff
#define HWIO_TCSR_SYS_POWER_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_SYS_POWER_CTRL_ADDR, HWIO_TCSR_SYS_POWER_CTRL_RMSK)
#define HWIO_TCSR_SYS_POWER_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SYS_POWER_CTRL_ADDR, m)
#define HWIO_TCSR_SYS_POWER_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_SYS_POWER_CTRL_ADDR,v)
#define HWIO_TCSR_SYS_POWER_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SYS_POWER_CTRL_ADDR,m,v,HWIO_TCSR_SYS_POWER_CTRL_IN)
#define HWIO_TCSR_SYS_POWER_CTRL_SYS_POWER_CTRL_BMSK                                                                          0xffff
#define HWIO_TCSR_SYS_POWER_CTRL_SYS_POWER_CTRL_SHFT                                                                          0x0

#define HWIO_TCSR_SYS_POWER_CTRL_USB_PORT_TYPE_BMSK    																		  0xF00
#define HWIO_TCSR_SYS_POWER_CTRL_USB_PORT_TYPE_SHFT      																	  0x8

/*----------------------------------------------------------------------------
 * MODULE: JTAG_ID
 *--------------------------------------------------------------------------*/

#define HWIO_JTAG_ID_ADDR                                                                                          			(SECURITY_CONTROL_BASE      + 0x000060e0)
#define HWIO_JTAG_ID_RMSK                                                                                          			 0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                                                  			 0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                                         	 0x0
#define HWIO_JTAG_ID_VERSION_ID_BMSK																						 0xf0000000
#define HWIO_JTAG_ID_VERSION_ID_SHIFT																						 0x1C

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
typedef struct
{
  uint32 dummy;  /* Ensure at least one element */
} hwio_shadow_type;

extern hwio_shadow_type *hwio_shadow_ptr;

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

#endif /* __QHSUSB_HWIO_DEPENDENCY_H__ */
