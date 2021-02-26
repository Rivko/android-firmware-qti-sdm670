#ifndef __HALDWCHWIO_H__
#define __HALDWCHWIO_H__
/*
===========================================================================
*/
/**
  @file HALdwcHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    HWIO_USB3
    USB30_QSCRATCH

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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


  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * Manually added to allow driver code independence from specific version
 * of Hwio header.
 *--------------------------------------------------------------------------*/
#define HWIO_DEVTEN_CMDCMPLTEN_BMSK          0
#define HWIO_DEVTEN_EVNTOVERFLOWEN_BMSK      0

/*----------------------------------------------------------------------------
 * MODULE: HWIO_USB3
 *--------------------------------------------------------------------------*/

#define HWIO_GUSB2PHYCFG_ADDR(base,p)                                      ((base) + 0x0000c200 + 0x4 * (p))
#define HWIO_GUSB2PHYCFG_OFFS(base,p)                                      (0x0000c200 + 0x4 * (p))
#define HWIO_GUSB2PHYCFG_RMSK                                              0xe007bfff
#define HWIO_GUSB2PHYCFG_MAXp                                                       0
#define HWIO_GUSB2PHYCFG_INI(base,p)        \
        in_dword_masked(HWIO_GUSB2PHYCFG_ADDR(base,p), HWIO_GUSB2PHYCFG_RMSK)
#define HWIO_GUSB2PHYCFG_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GUSB2PHYCFG_ADDR(base,p), mask)
#define HWIO_GUSB2PHYCFG_OUTI(base,p,val)    \
        out_dword(HWIO_GUSB2PHYCFG_ADDR(base,p),val)
#define HWIO_GUSB2PHYCFG_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GUSB2PHYCFG_ADDR(base,p),mask,val,HWIO_GUSB2PHYCFG_INI(base,p))
#define HWIO_GUSB2PHYCFG_PHYSOFTRST_BMSK                                   0x80000000
#define HWIO_GUSB2PHYCFG_PHYSOFTRST_SHFT                                         0x1f
#define HWIO_GUSB2PHYCFG_U2_FREECLK_EXISTS_BMSK                            0x40000000
#define HWIO_GUSB2PHYCFG_U2_FREECLK_EXISTS_SHFT                                  0x1e
#define HWIO_GUSB2PHYCFG_ULPI_LPM_WITH_OPMODE_CHK_BMSK                     0x20000000
#define HWIO_GUSB2PHYCFG_ULPI_LPM_WITH_OPMODE_CHK_SHFT                           0x1d
#define HWIO_GUSB2PHYCFG_ULPIEXTVBUSINDIACTOR_BMSK                            0x40000
#define HWIO_GUSB2PHYCFG_ULPIEXTVBUSINDIACTOR_SHFT                               0x12
#define HWIO_GUSB2PHYCFG_ULPIEXTVBUSDRV_BMSK                                  0x20000
#define HWIO_GUSB2PHYCFG_ULPIEXTVBUSDRV_SHFT                                     0x11
#define HWIO_GUSB2PHYCFG_ULPICLKSUSM_BMSK                                     0x10000
#define HWIO_GUSB2PHYCFG_ULPICLKSUSM_SHFT                                        0x10
#define HWIO_GUSB2PHYCFG_ULPIAUTORES_BMSK                                      0x8000
#define HWIO_GUSB2PHYCFG_ULPIAUTORES_SHFT                                         0xf
#define HWIO_GUSB2PHYCFG_USBTRDTIM_BMSK                                        0x3c00
#define HWIO_GUSB2PHYCFG_USBTRDTIM_SHFT                                           0xa
#define HWIO_GUSB2PHYCFG_XCVRDLY_BMSK                                           0x200
#define HWIO_GUSB2PHYCFG_XCVRDLY_SHFT                                             0x9
#define HWIO_GUSB2PHYCFG_ENBLSLPM_BMSK                                          0x100
#define HWIO_GUSB2PHYCFG_ENBLSLPM_SHFT                                            0x8
#define HWIO_GUSB2PHYCFG_PHYSEL_BMSK                                             0x80
#define HWIO_GUSB2PHYCFG_PHYSEL_SHFT                                              0x7
#define HWIO_GUSB2PHYCFG_SUSPENDUSB20_BMSK                                       0x40
#define HWIO_GUSB2PHYCFG_SUSPENDUSB20_SHFT                                        0x6
#define HWIO_GUSB2PHYCFG_FSINTF_BMSK                                             0x20
#define HWIO_GUSB2PHYCFG_FSINTF_SHFT                                              0x5
#define HWIO_GUSB2PHYCFG_ULPI_UTMI_SEL_BMSK                                      0x10
#define HWIO_GUSB2PHYCFG_ULPI_UTMI_SEL_SHFT                                       0x4
#define HWIO_GUSB2PHYCFG_PHYIF_BMSK                                               0x8
#define HWIO_GUSB2PHYCFG_PHYIF_SHFT                                               0x3
#define HWIO_GUSB2PHYCFG_B1L_BMSK                                                 0x7
#define HWIO_GUSB2PHYCFG_B1L_SHFT                                                 0x0

#define HWIO_GUSB2I2CCTL_ADDR(base,p)                                      ((base) + 0x0000c240 + 0x4 * (p))
#define HWIO_GUSB2I2CCTL_OFFS(base,p)                                      (0x0000c240 + 0x4 * (p))
#define HWIO_GUSB2I2CCTL_RMSK                                              0xffffffff
#define HWIO_GUSB2I2CCTL_MAXp                                                       0
#define HWIO_GUSB2I2CCTL_INI(base,p)        \
        in_dword_masked(HWIO_GUSB2I2CCTL_ADDR(base,p), HWIO_GUSB2I2CCTL_RMSK)
#define HWIO_GUSB2I2CCTL_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GUSB2I2CCTL_ADDR(base,p), mask)
#define HWIO_GUSB2I2CCTL_RSVD32_0_BMSK                                     0xffffffff
#define HWIO_GUSB2I2CCTL_RSVD32_0_SHFT                                            0x0

#define HWIO_GUSB2PHYACC_ULPI_ADDR(base,p)                                 ((base) + 0x0000c280 + 0x4 * (p))
#define HWIO_GUSB2PHYACC_ULPI_OFFS(base,p)                                 (0x0000c280 + 0x4 * (p))
#define HWIO_GUSB2PHYACC_ULPI_RMSK                                          0x7ff1fff
#define HWIO_GUSB2PHYACC_ULPI_MAXp                                                  0
#define HWIO_GUSB2PHYACC_ULPI_INI(base,p)        \
        in_dword_masked(HWIO_GUSB2PHYACC_ULPI_ADDR(base,p), HWIO_GUSB2PHYACC_ULPI_RMSK)
#define HWIO_GUSB2PHYACC_ULPI_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GUSB2PHYACC_ULPI_ADDR(base,p), mask)
#define HWIO_GUSB2PHYACC_ULPI_DISUIPIDRVR_BMSK                              0x4000000
#define HWIO_GUSB2PHYACC_ULPI_DISUIPIDRVR_SHFT                                   0x1a
#define HWIO_GUSB2PHYACC_ULPI_NEWREGREQ_BMSK                                0x2000000
#define HWIO_GUSB2PHYACC_ULPI_NEWREGREQ_SHFT                                     0x19
#define HWIO_GUSB2PHYACC_ULPI_VSTSDONE_BMSK                                 0x1000000
#define HWIO_GUSB2PHYACC_ULPI_VSTSDONE_SHFT                                      0x18
#define HWIO_GUSB2PHYACC_ULPI_VSTSBSY_BMSK                                   0x800000
#define HWIO_GUSB2PHYACC_ULPI_VSTSBSY_SHFT                                       0x17
#define HWIO_GUSB2PHYACC_ULPI_REGWR_BMSK                                     0x400000
#define HWIO_GUSB2PHYACC_ULPI_REGWR_SHFT                                         0x16
#define HWIO_GUSB2PHYACC_ULPI_REGADDR_BMSK                                   0x3f0000
#define HWIO_GUSB2PHYACC_ULPI_REGADDR_SHFT                                       0x10
#define HWIO_GUSB2PHYACC_ULPI_EXTREGADDR_BMSK                                  0x1f00
#define HWIO_GUSB2PHYACC_ULPI_EXTREGADDR_SHFT                                     0x8
#define HWIO_GUSB2PHYACC_ULPI_REGDATA_BMSK                                       0xff
#define HWIO_GUSB2PHYACC_ULPI_REGDATA_SHFT                                        0x0

#define HWIO_GUSB3PIPECTL_ADDR(base,p)                                    ((base) + 0x0000c2c0 + 0x4 * (p))
#define HWIO_GUSB3PIPECTL_OFFS(base,p)                                    (0x0000c2c0 + 0x4 * (p))
#define HWIO_GUSB3PIPECTL_RMSK                                            0xffffff7f
#define HWIO_GUSB3PIPECTL_MAXp                                                     0
#define HWIO_GUSB3PIPECTL_INI(base,p)        \
        in_dword_masked(HWIO_GUSB3PIPECTL_ADDR(base,p), HWIO_GUSB3PIPECTL_RMSK)
#define HWIO_GUSB3PIPECTL_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GUSB3PIPECTL_ADDR(base,p), mask)
#define HWIO_GUSB3PIPECTL_OUTI(base,p,val)    \
        out_dword(HWIO_GUSB3PIPECTL_ADDR(base,p),val)
#define HWIO_GUSB3PIPECTL_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GUSB3PIPECTL_ADDR(base,p),mask,val,HWIO_GUSB3PIPECTL_INI(base,p))
#define HWIO_GUSB3PIPECTL_PHYSOFTRST_BMSK                                 0x80000000
#define HWIO_GUSB3PIPECTL_PHYSOFTRST_SHFT                                       0x1f
#define HWIO_GUSB3PIPECTL_HSTPRTCMPL_BMSK                                 0x40000000
#define HWIO_GUSB3PIPECTL_HSTPRTCMPL_SHFT                                       0x1e
#define HWIO_GUSB3PIPECTL_U2SSINACTP3OK_BMSK                              0x20000000
#define HWIO_GUSB3PIPECTL_U2SSINACTP3OK_SHFT                                    0x1d
#define HWIO_GUSB3PIPECTL_DISRXDETP3_BMSK                                 0x10000000
#define HWIO_GUSB3PIPECTL_DISRXDETP3_SHFT                                       0x1c
#define HWIO_GUSB3PIPECTL_UX_EXIT_IN_PX_BMSK                               0x8000000
#define HWIO_GUSB3PIPECTL_UX_EXIT_IN_PX_SHFT                                    0x1b
#define HWIO_GUSB3PIPECTL_PING_ENHANCEMENT_EN_BMSK                         0x4000000
#define HWIO_GUSB3PIPECTL_PING_ENHANCEMENT_EN_SHFT                              0x1a
#define HWIO_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV_BMSK                       0x2000000
#define HWIO_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV_SHFT                            0x19
#define HWIO_GUSB3PIPECTL_REQUEST_P1P2P3_BMSK                              0x1000000
#define HWIO_GUSB3PIPECTL_REQUEST_P1P2P3_SHFT                                   0x18
#define HWIO_GUSB3PIPECTL_STARTRXDETU3RXDET_BMSK                            0x800000
#define HWIO_GUSB3PIPECTL_STARTRXDETU3RXDET_SHFT                                0x17
#define HWIO_GUSB3PIPECTL_DISRXDETU3RXDET_BMSK                              0x400000
#define HWIO_GUSB3PIPECTL_DISRXDETU3RXDET_SHFT                                  0x16
#define HWIO_GUSB3PIPECTL_DELAYP1P2P3_BMSK                                  0x380000
#define HWIO_GUSB3PIPECTL_DELAYP1P2P3_SHFT                                      0x13
#define HWIO_GUSB3PIPECTL_DELAYP1TRANS_BMSK                                  0x40000
#define HWIO_GUSB3PIPECTL_DELAYP1TRANS_SHFT                                     0x12
#define HWIO_GUSB3PIPECTL_SUSPENDENABLE_BMSK                                 0x20000
#define HWIO_GUSB3PIPECTL_SUSPENDENABLE_SHFT                                    0x11
#define HWIO_GUSB3PIPECTL_DATWIDTH_BMSK                                      0x18000
#define HWIO_GUSB3PIPECTL_DATWIDTH_SHFT                                          0xf
#define HWIO_GUSB3PIPECTL_ABORTRXDETINU2_BMSK                                 0x4000
#define HWIO_GUSB3PIPECTL_ABORTRXDETINU2_SHFT                                    0xe
#define HWIO_GUSB3PIPECTL_SKIPRXDET_BMSK                                      0x2000
#define HWIO_GUSB3PIPECTL_SKIPRXDET_SHFT                                         0xd
#define HWIO_GUSB3PIPECTL_LFPSP0ALGN_BMSK                                     0x1000
#define HWIO_GUSB3PIPECTL_LFPSP0ALGN_SHFT                                        0xc
#define HWIO_GUSB3PIPECTL_P3P2TRANOK_BMSK                                      0x800
#define HWIO_GUSB3PIPECTL_P3P2TRANOK_SHFT                                        0xb
#define HWIO_GUSB3PIPECTL_P3EXSIGP2_BMSK                                       0x400
#define HWIO_GUSB3PIPECTL_P3EXSIGP2_SHFT                                         0xa
#define HWIO_GUSB3PIPECTL_LFPSFILTER_BMSK                                      0x200
#define HWIO_GUSB3PIPECTL_LFPSFILTER_SHFT                                        0x9
#define HWIO_GUSB3PIPECTL_RX_DETECT_TO_POLLING_LFPS_CONTROL_BMSK               0x100
#define HWIO_GUSB3PIPECTL_RX_DETECT_TO_POLLING_LFPS_CONTROL_SHFT                 0x8
#define HWIO_GUSB3PIPECTL_TX_SWING_BMSK                                         0x40
#define HWIO_GUSB3PIPECTL_TX_SWING_SHFT                                          0x6
#define HWIO_GUSB3PIPECTL_TX_MARGIN_BMSK                                        0x38
#define HWIO_GUSB3PIPECTL_TX_MARGIN_SHFT                                         0x3
#define HWIO_GUSB3PIPECTL_TX_DE_EPPHASIS_BMSK                                    0x6
#define HWIO_GUSB3PIPECTL_TX_DE_EPPHASIS_SHFT                                    0x1
#define HWIO_GUSB3PIPECTL_ELASTIC_BUFFER_MODE_BMSK                               0x1
#define HWIO_GUSB3PIPECTL_ELASTIC_BUFFER_MODE_SHFT                               0x0

#define HWIO_GTXFIFOSIZ_ADDR(base,p)                                       ((base) + 0x0000c300 + 0x4 * (p))
#define HWIO_GTXFIFOSIZ_OFFS(base,p)                                       (0x0000c300 + 0x4 * (p))
#define HWIO_GTXFIFOSIZ_RMSK                                               0xffffffff
#define HWIO_GTXFIFOSIZ_MAXp                                                        0
#define HWIO_GTXFIFOSIZ_INI(base,p)        \
        in_dword_masked(HWIO_GTXFIFOSIZ_ADDR(base,p), HWIO_GTXFIFOSIZ_RMSK)
#define HWIO_GTXFIFOSIZ_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GTXFIFOSIZ_ADDR(base,p), mask)
#define HWIO_GTXFIFOSIZ_OUTI(base,p,val)    \
        out_dword(HWIO_GTXFIFOSIZ_ADDR(base,p),val)
#define HWIO_GTXFIFOSIZ_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GTXFIFOSIZ_ADDR(base,p),mask,val,HWIO_GTXFIFOSIZ_INI(base,p))
#define HWIO_GTXFIFOSIZ_TXFSTADDR_N_BMSK                                   0xffff0000
#define HWIO_GTXFIFOSIZ_TXFSTADDR_N_SHFT                                         0x10
#define HWIO_GTXFIFOSIZ_TXFDEP_N_BMSK                                          0xffff
#define HWIO_GTXFIFOSIZ_TXFDEP_N_SHFT                                             0x0

#define HWIO_GRXFIFOSIZ_ADDR(base,p)                                       ((base) + 0x0000c380 + 0x4 * (p))
#define HWIO_GRXFIFOSIZ_OFFS(base,p)                                       (0x0000c380 + 0x4 * (p))
#define HWIO_GRXFIFOSIZ_RMSK                                               0xffffffff
#define HWIO_GRXFIFOSIZ_MAXp                                                        0
#define HWIO_GRXFIFOSIZ_INI(base,p)        \
        in_dword_masked(HWIO_GRXFIFOSIZ_ADDR(base,p), HWIO_GRXFIFOSIZ_RMSK)
#define HWIO_GRXFIFOSIZ_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GRXFIFOSIZ_ADDR(base,p), mask)
#define HWIO_GRXFIFOSIZ_OUTI(base,p,val)    \
        out_dword(HWIO_GRXFIFOSIZ_ADDR(base,p),val)
#define HWIO_GRXFIFOSIZ_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GRXFIFOSIZ_ADDR(base,p),mask,val,HWIO_GRXFIFOSIZ_INI(base,p))
#define HWIO_GRXFIFOSIZ_RXFSTADDR_N_BMSK                                   0xffff0000
#define HWIO_GRXFIFOSIZ_RXFSTADDR_N_SHFT                                         0x10
#define HWIO_GRXFIFOSIZ_RXFDEP_N_BMSK                                          0xffff
#define HWIO_GRXFIFOSIZ_RXFDEP_N_SHFT                                             0x0

#define HWIO_GEVNTADRLO_ADDR(base,p)                                        ((base) + 0x0000c400 + 0x10 * (p))
#define HWIO_GEVNTADRLO_OFFS(base,p)                                        (0x0000c400 + 0x10 * (p))
#define HWIO_GEVNTADRLO_RMSK                                                0xffffffff
#define HWIO_GEVNTADRLO_MAXp                                                         1
#define HWIO_GEVNTADRLO_INI(base,p)        \
        in_dword_masked(HWIO_GEVNTADRLO_ADDR(base,p), HWIO_GEVNTADRLO_RMSK)
#define HWIO_GEVNTADRLO_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GEVNTADRLO_ADDR(base,p), mask)
#define HWIO_GEVNTADRLO_OUTI(base,p,val)    \
        out_dword(HWIO_GEVNTADRLO_ADDR(base,p),val)
#define HWIO_GEVNTADRLO_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GEVNTADRLO_ADDR(base,p),mask,val,HWIO_GEVNTADRLO_INI(base,p))
#define HWIO_GEVNTADRLO_EVNTADRLO_BMSK                                      0xffffffff
#define HWIO_GEVNTADRLO_EVNTADRLO_SHFT                                             0x0

#define HWIO_GEVNTADRHI_ADDR(base,p)                                        ((base) + 0x0000c404 + 0x10 * (p))
#define HWIO_GEVNTADRHI_OFFS(base,p)                                        (0x0000c404 + 0x10 * (p))
#define HWIO_GEVNTADRHI_RMSK                                                0xffffffff
#define HWIO_GEVNTADRHI_MAXp                                                         1
#define HWIO_GEVNTADRHI_INI(base,p)        \
        in_dword_masked(HWIO_GEVNTADRHI_ADDR(base,p), HWIO_GEVNTADRHI_RMSK)
#define HWIO_GEVNTADRHI_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GEVNTADRHI_ADDR(base,p), mask)
#define HWIO_GEVNTADRHI_OUTI(base,p,val)    \
        out_dword(HWIO_GEVNTADRHI_ADDR(base,p),val)
#define HWIO_GEVNTADRHI_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GEVNTADRHI_ADDR(base,p),mask,val,HWIO_GEVNTADRHI_INI(base,p))
#define HWIO_GEVNTADRHI_EVNTADRHI_BMSK                                      0xffffffff
#define HWIO_GEVNTADRHI_EVNTADRHI_SHFT                                             0x0

#define HWIO_GEVNTSIZ_ADDR(base,p)                                          ((base) + 0x0000c408 + 0x10 * (p))
#define HWIO_GEVNTSIZ_OFFS(base,p)                                          (0x0000c408 + 0x10 * (p))
#define HWIO_GEVNTSIZ_RMSK                                                  0x8000ffff
#define HWIO_GEVNTSIZ_MAXp                                                           1
#define HWIO_GEVNTSIZ_INI(base,p)        \
        in_dword_masked(HWIO_GEVNTSIZ_ADDR(base,p), HWIO_GEVNTSIZ_RMSK)
#define HWIO_GEVNTSIZ_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GEVNTSIZ_ADDR(base,p), mask)
#define HWIO_GEVNTSIZ_OUTI(base,p,val)    \
        out_dword(HWIO_GEVNTSIZ_ADDR(base,p),val)
#define HWIO_GEVNTSIZ_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GEVNTSIZ_ADDR(base,p),mask,val,HWIO_GEVNTSIZ_INI(base,p))
#define HWIO_GEVNTSIZ_EVNTINTRPTMASK_BMSK                                   0x80000000
#define HWIO_GEVNTSIZ_EVNTINTRPTMASK_SHFT                                         0x1f
#define HWIO_GEVNTSIZ_EVENTSIZ_BMSK                                             0xffff
#define HWIO_GEVNTSIZ_EVENTSIZ_SHFT                                                0x0

#define HWIO_GEVNTCOUNT_ADDR(base,p)                                        ((base) + 0x0000c40c + 0x10 * (p))
#define HWIO_GEVNTCOUNT_OFFS(base,p)                                        (0x0000c40c + 0x10 * (p))
#define HWIO_GEVNTCOUNT_RMSK                                                    0xffff
#define HWIO_GEVNTCOUNT_MAXp                                                         1
#define HWIO_GEVNTCOUNT_INI(base,p)        \
        in_dword_masked(HWIO_GEVNTCOUNT_ADDR(base,p), HWIO_GEVNTCOUNT_RMSK)
#define HWIO_GEVNTCOUNT_INMI(base,p,mask)    \
        in_dword_masked(HWIO_GEVNTCOUNT_ADDR(base,p), mask)
#define HWIO_GEVNTCOUNT_OUTI(base,p,val)    \
        out_dword(HWIO_GEVNTCOUNT_ADDR(base,p),val)
#define HWIO_GEVNTCOUNT_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_GEVNTCOUNT_ADDR(base,p),mask,val,HWIO_GEVNTCOUNT_INI(base,p))
#define HWIO_GEVNTCOUNT_EVNTCOUNT_BMSK                                          0xffff
#define HWIO_GEVNTCOUNT_EVNTCOUNT_SHFT                                             0x0

#define HWIO_GSBUSCFG0_ADDR(x)                                                                ((x) + 0x0000c100)
#define HWIO_GSBUSCFG0_OFFS                                                                   (0x0000c100)
#define HWIO_GSBUSCFG0_RMSK                                                                   0xffff5cff
#define HWIO_GSBUSCFG0_IN(x)      \
        in_dword_masked(HWIO_GSBUSCFG0_ADDR(x), HWIO_GSBUSCFG0_RMSK)
#define HWIO_GSBUSCFG0_INM(x, m)      \
        in_dword_masked(HWIO_GSBUSCFG0_ADDR(x), m)
#define HWIO_GSBUSCFG0_OUT(x, v)      \
        out_dword(HWIO_GSBUSCFG0_ADDR(x),v)
#define HWIO_GSBUSCFG0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GSBUSCFG0_ADDR(x),m,v,HWIO_GSBUSCFG0_IN(x))
#define HWIO_GSBUSCFG0_DATRDREQINFO_BMSK                                                      0xf0000000
#define HWIO_GSBUSCFG0_DATRDREQINFO_SHFT                                                            0x1c
#define HWIO_GSBUSCFG0_DESRDREQINFO_BMSK                                                       0xf000000
#define HWIO_GSBUSCFG0_DESRDREQINFO_SHFT                                                            0x18
#define HWIO_GSBUSCFG0_DATWRREQINFO_BMSK                                                        0xf00000
#define HWIO_GSBUSCFG0_DATWRREQINFO_SHFT                                                            0x14
#define HWIO_GSBUSCFG0_DESWRREQINFO_BMSK                                                         0xf0000
#define HWIO_GSBUSCFG0_DESWRREQINFO_SHFT                                                            0x10
#define HWIO_GSBUSCFG0_SINGREQ_BMSK                                                               0x4000
#define HWIO_GSBUSCFG0_SINGREQ_SHFT                                                                  0xe
#define HWIO_GSBUSCFG0_STOREANDFORWARD_BMSK                                                       0x1000
#define HWIO_GSBUSCFG0_STOREANDFORWARD_SHFT                                                          0xc
#define HWIO_GSBUSCFG0_DATBIGEND_BMSK                                                              0x800
#define HWIO_GSBUSCFG0_DATBIGEND_SHFT                                                                0xb
#define HWIO_GSBUSCFG0_DESBIGEND_BMSK                                                              0x400
#define HWIO_GSBUSCFG0_DESBIGEND_SHFT                                                                0xa
#define HWIO_GSBUSCFG0_INCR256BRSTENA_BMSK                                                          0x80
#define HWIO_GSBUSCFG0_INCR256BRSTENA_SHFT                                                           0x7
#define HWIO_GSBUSCFG0_INCR128BRSTENA_BMSK                                                          0x40
#define HWIO_GSBUSCFG0_INCR128BRSTENA_SHFT                                                           0x6
#define HWIO_GSBUSCFG0_INCR64BRSTENA_BMSK                                                           0x20
#define HWIO_GSBUSCFG0_INCR64BRSTENA_SHFT                                                            0x5
#define HWIO_GSBUSCFG0_INCR32BRSTENA_BMSK                                                           0x10
#define HWIO_GSBUSCFG0_INCR32BRSTENA_SHFT                                                            0x4
#define HWIO_GSBUSCFG0_INCR16BRSTENA_BMSK                                                            0x8
#define HWIO_GSBUSCFG0_INCR16BRSTENA_SHFT                                                            0x3
#define HWIO_GSBUSCFG0_INCR8BRSTENA_BMSK                                                             0x4
#define HWIO_GSBUSCFG0_INCR8BRSTENA_SHFT                                                             0x2
#define HWIO_GSBUSCFG0_INCR4BRSTENA_BMSK                                                             0x2
#define HWIO_GSBUSCFG0_INCR4BRSTENA_SHFT                                                             0x1
#define HWIO_GSBUSCFG0_INCRBRSTENA_BMSK                                                              0x1
#define HWIO_GSBUSCFG0_INCRBRSTENA_SHFT                                                              0x0

#define HWIO_GSBUSCFG1_ADDR(x)                                                                ((x) + 0x0000c104)
#define HWIO_GSBUSCFG1_OFFS                                                                   (0x0000c104)
#define HWIO_GSBUSCFG1_RMSK                                                                       0x1f00
#define HWIO_GSBUSCFG1_IN(x)      \
        in_dword_masked(HWIO_GSBUSCFG1_ADDR(x), HWIO_GSBUSCFG1_RMSK)
#define HWIO_GSBUSCFG1_INM(x, m)      \
        in_dword_masked(HWIO_GSBUSCFG1_ADDR(x), m)
#define HWIO_GSBUSCFG1_OUT(x, v)      \
        out_dword(HWIO_GSBUSCFG1_ADDR(x),v)
#define HWIO_GSBUSCFG1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GSBUSCFG1_ADDR(x),m,v,HWIO_GSBUSCFG1_IN(x))
#define HWIO_GSBUSCFG1_EN1KPAGE_BMSK                                                              0x1000
#define HWIO_GSBUSCFG1_EN1KPAGE_SHFT                                                                 0xc
#define HWIO_GSBUSCFG1_PIPETRANSLIMIT_BMSK                                                         0xf00
#define HWIO_GSBUSCFG1_PIPETRANSLIMIT_SHFT                                                           0x8

#define HWIO_GTXTHRCFG_ADDR(x)                                                                ((x) + 0x0000c108)
#define HWIO_GTXTHRCFG_OFFS                                                                   (0x0000c108)
#define HWIO_GTXTHRCFG_RMSK                                                                   0xafff8000
#define HWIO_GTXTHRCFG_IN(x)      \
        in_dword_masked(HWIO_GTXTHRCFG_ADDR(x), HWIO_GTXTHRCFG_RMSK)
#define HWIO_GTXTHRCFG_INM(x, m)      \
        in_dword_masked(HWIO_GTXTHRCFG_ADDR(x), m)
#define HWIO_GTXTHRCFG_OUT(x, v)      \
        out_dword(HWIO_GTXTHRCFG_ADDR(x),v)
#define HWIO_GTXTHRCFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GTXTHRCFG_ADDR(x),m,v,HWIO_GTXTHRCFG_IN(x))
#define HWIO_GTXTHRCFG_USBISOTHREN_BMSK                                                       0x80000000
#define HWIO_GTXTHRCFG_USBISOTHREN_SHFT                                                             0x1f
#define HWIO_GTXTHRCFG_USBTXPKTCNTSEL_BMSK                                                    0x20000000
#define HWIO_GTXTHRCFG_USBTXPKTCNTSEL_SHFT                                                          0x1d
#define HWIO_GTXTHRCFG_USBTXPKTCNT_BMSK                                                        0xf000000
#define HWIO_GTXTHRCFG_USBTXPKTCNT_SHFT                                                             0x18
#define HWIO_GTXTHRCFG_USBMAXTXBURSTSIZE_BMSK                                                   0xff0000
#define HWIO_GTXTHRCFG_USBMAXTXBURSTSIZE_SHFT                                                       0x10
#define HWIO_GTXTHRCFG_SBUSISOTHREN_BMSK                                                          0x8000
#define HWIO_GTXTHRCFG_SBUSISOTHREN_SHFT                                                             0xf

#define HWIO_GRXTHRCFG_ADDR(x)                                                                ((x) + 0x0000c10c)
#define HWIO_GRXTHRCFG_OFFS                                                                   (0x0000c10c)
#define HWIO_GRXTHRCFG_RMSK                                                                   0x2ff80000
#define HWIO_GRXTHRCFG_IN(x)      \
        in_dword_masked(HWIO_GRXTHRCFG_ADDR(x), HWIO_GRXTHRCFG_RMSK)
#define HWIO_GRXTHRCFG_INM(x, m)      \
        in_dword_masked(HWIO_GRXTHRCFG_ADDR(x), m)
#define HWIO_GRXTHRCFG_OUT(x, v)      \
        out_dword(HWIO_GRXTHRCFG_ADDR(x),v)
#define HWIO_GRXTHRCFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GRXTHRCFG_ADDR(x),m,v,HWIO_GRXTHRCFG_IN(x))
#define HWIO_GRXTHRCFG_USBRXPKTCNTSEL_BMSK                                                    0x20000000
#define HWIO_GRXTHRCFG_USBRXPKTCNTSEL_SHFT                                                          0x1d
#define HWIO_GRXTHRCFG_USBRXPKTCNT_BMSK                                                        0xf000000
#define HWIO_GRXTHRCFG_USBRXPKTCNT_SHFT                                                             0x18
#define HWIO_GRXTHRCFG_USBMAXRXBURSTSIZE_BMSK                                                   0xf80000
#define HWIO_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT                                                       0x13

#define HWIO_GCTL_ADDR(x)                                                                     ((x) + 0x0000c110)
#define HWIO_GCTL_OFFS                                                                        (0x0000c110)
#define HWIO_GCTL_RMSK                                                                        0xffffffff
#define HWIO_GCTL_IN(x)      \
        in_dword_masked(HWIO_GCTL_ADDR(x), HWIO_GCTL_RMSK)
#define HWIO_GCTL_INM(x, m)      \
        in_dword_masked(HWIO_GCTL_ADDR(x), m)
#define HWIO_GCTL_OUT(x, v)      \
        out_dword(HWIO_GCTL_ADDR(x),v)
#define HWIO_GCTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GCTL_ADDR(x),m,v,HWIO_GCTL_IN(x))
#define HWIO_GCTL_PWRDNSCALE_BMSK                                                             0xfff80000
#define HWIO_GCTL_PWRDNSCALE_SHFT                                                                   0x13
#define HWIO_GCTL_MASTERFILTBYPASS_BMSK                                                          0x40000
#define HWIO_GCTL_MASTERFILTBYPASS_SHFT                                                             0x12
#define HWIO_GCTL_BYPSSETADDR_BMSK                                                               0x20000
#define HWIO_GCTL_BYPSSETADDR_SHFT                                                                  0x11
#define HWIO_GCTL_U2RSTECN_BMSK                                                                  0x10000
#define HWIO_GCTL_U2RSTECN_SHFT                                                                     0x10
#define HWIO_GCTL_FRMSCLDWN_BMSK                                                                  0xc000
#define HWIO_GCTL_FRMSCLDWN_SHFT                                                                     0xe
#define HWIO_GCTL_PRTCAPDIR_BMSK                                                                  0x3000
#define HWIO_GCTL_PRTCAPDIR_SHFT                                                                     0xc
#define HWIO_GCTL_CORESOFTRESET_BMSK                                                               0x800
#define HWIO_GCTL_CORESOFTRESET_SHFT                                                                 0xb
#define HWIO_GCTL_SOFITPSYNC_BMSK                                                                  0x400
#define HWIO_GCTL_SOFITPSYNC_SHFT                                                                    0xa
#define HWIO_GCTL_U1U2TIMERSCALE_BMSK                                                              0x200
#define HWIO_GCTL_U1U2TIMERSCALE_SHFT                                                                0x9
#define HWIO_GCTL_DEBUGATTACH_BMSK                                                                 0x100
#define HWIO_GCTL_DEBUGATTACH_SHFT                                                                   0x8
#define HWIO_GCTL_RAMCLKSEL_BMSK                                                                    0xc0
#define HWIO_GCTL_RAMCLKSEL_SHFT                                                                     0x6
#define HWIO_GCTL_SCALEDOWN_BMSK                                                                    0x30
#define HWIO_GCTL_SCALEDOWN_SHFT                                                                     0x4
#define HWIO_GCTL_DISSCRAMBLE_BMSK                                                                   0x8
#define HWIO_GCTL_DISSCRAMBLE_SHFT                                                                   0x3
#define HWIO_GCTL_U2EXIT_LFPS_BMSK                                                                   0x4
#define HWIO_GCTL_U2EXIT_LFPS_SHFT                                                                   0x2
#define HWIO_GCTL_GBLHIBERNATIONEN_BMSK                                                              0x2
#define HWIO_GCTL_GBLHIBERNATIONEN_SHFT                                                              0x1
#define HWIO_GCTL_DSBLCLKGTNG_BMSK                                                                   0x1
#define HWIO_GCTL_DSBLCLKGTNG_SHFT                                                                   0x0

#define HWIO_GPMSTS_ADDR(x)                                                                   ((x) + 0x0000c114)
#define HWIO_GPMSTS_OFFS                                                                      (0x0000c114)
#define HWIO_GPMSTS_RMSK                                                                      0xf000f1ff
#define HWIO_GPMSTS_IN(x)      \
        in_dword_masked(HWIO_GPMSTS_ADDR(x), HWIO_GPMSTS_RMSK)
#define HWIO_GPMSTS_INM(x, m)      \
        in_dword_masked(HWIO_GPMSTS_ADDR(x), m)
#define HWIO_GPMSTS_OUT(x, v)      \
        out_dword(HWIO_GPMSTS_ADDR(x),v)
#define HWIO_GPMSTS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GPMSTS_ADDR(x),m,v,HWIO_GPMSTS_IN(x))
#define HWIO_GPMSTS_PORTSEL_BMSK                                                              0xf0000000
#define HWIO_GPMSTS_PORTSEL_SHFT                                                                    0x1c
#define HWIO_GPMSTS_U3WAKEUP_BMSK                                                                 0xf000
#define HWIO_GPMSTS_U3WAKEUP_SHFT                                                                    0xc
#define HWIO_GPMSTS_U2WAKEUP_BMSK                                                                  0x1ff
#define HWIO_GPMSTS_U2WAKEUP_SHFT                                                                    0x0

#define HWIO_GSTS_ADDR(x)                                                                     ((x) + 0x0000c118)
#define HWIO_GSTS_OFFS                                                                        (0x0000c118)
#define HWIO_GSTS_RMSK                                                                        0xfff007f3
#define HWIO_GSTS_IN(x)      \
        in_dword_masked(HWIO_GSTS_ADDR(x), HWIO_GSTS_RMSK)
#define HWIO_GSTS_INM(x, m)      \
        in_dword_masked(HWIO_GSTS_ADDR(x), m)
#define HWIO_GSTS_CBELT_BMSK                                                                  0xfff00000
#define HWIO_GSTS_CBELT_SHFT                                                                        0x14
#define HWIO_GSTS_OTG_IP_BMSK                                                                      0x400
#define HWIO_GSTS_OTG_IP_SHFT                                                                        0xa
#define HWIO_GSTS_BC_IP_BMSK                                                                       0x200
#define HWIO_GSTS_BC_IP_SHFT                                                                         0x9
#define HWIO_GSTS_ADP_IP_BMSK                                                                      0x100
#define HWIO_GSTS_ADP_IP_SHFT                                                                        0x8
#define HWIO_GSTS_HOST_IP_BMSK                                                                      0x80
#define HWIO_GSTS_HOST_IP_SHFT                                                                       0x7
#define HWIO_GSTS_DEVICE_IP_BMSK                                                                    0x40
#define HWIO_GSTS_DEVICE_IP_SHFT                                                                     0x6
#define HWIO_GSTS_CSRTIMEOUT_BMSK                                                                   0x20
#define HWIO_GSTS_CSRTIMEOUT_SHFT                                                                    0x5
#define HWIO_GSTS_BUSERRADDRVLD_BMSK                                                                0x10
#define HWIO_GSTS_BUSERRADDRVLD_SHFT                                                                 0x4
#define HWIO_GSTS_CURMOD_BMSK                                                                        0x3
#define HWIO_GSTS_CURMOD_SHFT                                                                        0x0

#define HWIO_GUCTL1_ADDR(x)                                                                   ((x) + 0x0000c11c)
#define HWIO_GUCTL1_OFFS                                                                      (0x0000c11c)
#define HWIO_GUCTL1_RMSK                                                                             0x3
#define HWIO_GUCTL1_IN(x)      \
        in_dword_masked(HWIO_GUCTL1_ADDR(x), HWIO_GUCTL1_RMSK)
#define HWIO_GUCTL1_INM(x, m)      \
        in_dword_masked(HWIO_GUCTL1_ADDR(x), m)
#define HWIO_GUCTL1_OUT(x, v)      \
        out_dword(HWIO_GUCTL1_ADDR(x),v)
#define HWIO_GUCTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GUCTL1_ADDR(x),m,v,HWIO_GUCTL1_IN(x))
#define HWIO_GUCTL1_OVRLD_L1_SUSP_COM_BMSK                                                           0x2
#define HWIO_GUCTL1_OVRLD_L1_SUSP_COM_SHFT                                                           0x1
#define HWIO_GUCTL1_LOA_FILTER_EN_BMSK                                                               0x1
#define HWIO_GUCTL1_LOA_FILTER_EN_SHFT                                                               0x0

#define HWIO_GSNPSID_ADDR(x)                                                                  ((x) + 0x0000c120)
#define HWIO_GSNPSID_OFFS                                                                     (0x0000c120)
#define HWIO_GSNPSID_RMSK                                                                     0xffffffff
#define HWIO_GSNPSID_IN(x)      \
        in_dword_masked(HWIO_GSNPSID_ADDR(x), HWIO_GSNPSID_RMSK)
#define HWIO_GSNPSID_INM(x, m)      \
        in_dword_masked(HWIO_GSNPSID_ADDR(x), m)
#define HWIO_GSNPSID_SYNOPSYSID_BMSK                                                          0xffffffff
#define HWIO_GSNPSID_SYNOPSYSID_SHFT                                                                 0x0

#define HWIO_GGPIO_ADDR(x)                                                                    ((x) + 0x0000c124)
#define HWIO_GGPIO_OFFS                                                                       (0x0000c124)
#define HWIO_GGPIO_RMSK                                                                       0xffffffff
#define HWIO_GGPIO_IN(x)      \
        in_dword_masked(HWIO_GGPIO_ADDR(x), HWIO_GGPIO_RMSK)
#define HWIO_GGPIO_INM(x, m)      \
        in_dword_masked(HWIO_GGPIO_ADDR(x), m)
#define HWIO_GGPIO_OUT(x, v)      \
        out_dword(HWIO_GGPIO_ADDR(x),v)
#define HWIO_GGPIO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GGPIO_ADDR(x),m,v,HWIO_GGPIO_IN(x))
#define HWIO_GGPIO_GPO_BMSK                                                                   0xffff0000
#define HWIO_GGPIO_GPO_SHFT                                                                         0x10
#define HWIO_GGPIO_GPI_BMSK                                                                       0xffff
#define HWIO_GGPIO_GPI_SHFT                                                                          0x0

#define HWIO_GUID_ADDR(x)                                                                     ((x) + 0x0000c128)
#define HWIO_GUID_OFFS                                                                        (0x0000c128)
#define HWIO_GUID_RMSK                                                                        0xffffffff
#define HWIO_GUID_IN(x)      \
        in_dword_masked(HWIO_GUID_ADDR(x), HWIO_GUID_RMSK)
#define HWIO_GUID_INM(x, m)      \
        in_dword_masked(HWIO_GUID_ADDR(x), m)
#define HWIO_GUID_OUT(x, v)      \
        out_dword(HWIO_GUID_ADDR(x),v)
#define HWIO_GUID_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GUID_ADDR(x),m,v,HWIO_GUID_IN(x))
#define HWIO_GUID_USERID_BMSK                                                                 0xffffffff
#define HWIO_GUID_USERID_SHFT                                                                        0x0

#define HWIO_GUCTL_ADDR(x)                                                                    ((x) + 0x0000c12c)
#define HWIO_GUCTL_OFFS                                                                       (0x0000c12c)
#define HWIO_GUCTL_RMSK                                                                       0xffffdfff
#define HWIO_GUCTL_IN(x)      \
        in_dword_masked(HWIO_GUCTL_ADDR(x), HWIO_GUCTL_RMSK)
#define HWIO_GUCTL_INM(x, m)      \
        in_dword_masked(HWIO_GUCTL_ADDR(x), m)
#define HWIO_GUCTL_OUT(x, v)      \
        out_dword(HWIO_GUCTL_ADDR(x),v)
#define HWIO_GUCTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GUCTL_ADDR(x),m,v,HWIO_GUCTL_IN(x))
#define HWIO_GUCTL_REFCLKPER_BMSK                                                             0xffc00000
#define HWIO_GUCTL_REFCLKPER_SHFT                                                                   0x16
#define HWIO_GUCTL_NOEXTRDL_BMSK                                                                0x200000
#define HWIO_GUCTL_NOEXTRDL_SHFT                                                                    0x15
#define HWIO_GUCTL_PSQEXTRRESSP_BMSK                                                            0x1c0000
#define HWIO_GUCTL_PSQEXTRRESSP_SHFT                                                                0x12
#define HWIO_GUCTL_SPRSCTRLTRANSEN_BMSK                                                          0x20000
#define HWIO_GUCTL_SPRSCTRLTRANSEN_SHFT                                                             0x11
#define HWIO_GUCTL_RESBWHSEPS_BMSK                                                               0x10000
#define HWIO_GUCTL_RESBWHSEPS_SHFT                                                                  0x10
#define HWIO_GUCTL_CMDEVADDR_BMSK                                                                 0x8000
#define HWIO_GUCTL_CMDEVADDR_SHFT                                                                    0xf
#define HWIO_GUCTL_USBHSTINAUTORETRYEN_BMSK                                                       0x4000
#define HWIO_GUCTL_USBHSTINAUTORETRYEN_SHFT                                                          0xe
#define HWIO_GUCTL_EXTCAPSUPPTEN_BMSK                                                             0x1000
#define HWIO_GUCTL_EXTCAPSUPPTEN_SHFT                                                                0xc
#define HWIO_GUCTL_INSRTEXTRFSBODI_BMSK                                                            0x800
#define HWIO_GUCTL_INSRTEXTRFSBODI_SHFT                                                              0xb
#define HWIO_GUCTL_DTCT_BMSK                                                                       0x600
#define HWIO_GUCTL_DTCT_SHFT                                                                         0x9
#define HWIO_GUCTL_DTFT_BMSK                                                                       0x1ff
#define HWIO_GUCTL_DTFT_SHFT                                                                         0x0

#define HWIO_GBUSERRADDRLO_ADDR(x)                                                            ((x) + 0x0000c130)
#define HWIO_GBUSERRADDRLO_OFFS                                                               (0x0000c130)
#define HWIO_GBUSERRADDRLO_RMSK                                                               0xffffffff
#define HWIO_GBUSERRADDRLO_IN(x)      \
        in_dword_masked(HWIO_GBUSERRADDRLO_ADDR(x), HWIO_GBUSERRADDRLO_RMSK)
#define HWIO_GBUSERRADDRLO_INM(x, m)      \
        in_dword_masked(HWIO_GBUSERRADDRLO_ADDR(x), m)
#define HWIO_GBUSERRADDRLO_BUSERRADDR_BMSK                                                    0xffffffff
#define HWIO_GBUSERRADDRLO_BUSERRADDR_SHFT                                                           0x0

#define HWIO_GBUSERRADDRHI_ADDR(x)                                                            ((x) + 0x0000c134)
#define HWIO_GBUSERRADDRHI_OFFS                                                               (0x0000c134)
#define HWIO_GBUSERRADDRHI_RMSK                                                               0xffffffff
#define HWIO_GBUSERRADDRHI_IN(x)      \
        in_dword_masked(HWIO_GBUSERRADDRHI_ADDR(x), HWIO_GBUSERRADDRHI_RMSK)
#define HWIO_GBUSERRADDRHI_INM(x, m)      \
        in_dword_masked(HWIO_GBUSERRADDRHI_ADDR(x), m)
#define HWIO_GBUSERRADDRHI_BUSERRADDR_BMSK                                                    0xffffffff
#define HWIO_GBUSERRADDRHI_BUSERRADDR_SHFT                                                           0x0

#define HWIO_GPRTBIMAPLO_ADDR(x)                                                              ((x) + 0x0000c138)
#define HWIO_GPRTBIMAPLO_OFFS                                                                 (0x0000c138)
#define HWIO_GPRTBIMAPLO_RMSK                                                                 0xffffffff
#define HWIO_GPRTBIMAPLO_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAPLO_ADDR(x), HWIO_GPRTBIMAPLO_RMSK)
#define HWIO_GPRTBIMAPLO_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAPLO_ADDR(x), m)
#define HWIO_GPRTBIMAPLO_OUT(x, v)      \
        out_dword(HWIO_GPRTBIMAPLO_ADDR(x),v)
#define HWIO_GPRTBIMAPLO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GPRTBIMAPLO_ADDR(x),m,v,HWIO_GPRTBIMAPLO_IN(x))
#define HWIO_GPRTBIMAPLO_BINUM8_BMSK                                                          0xf0000000
#define HWIO_GPRTBIMAPLO_BINUM8_SHFT                                                                0x1c
#define HWIO_GPRTBIMAPLO_BINUM7_BMSK                                                           0xf000000
#define HWIO_GPRTBIMAPLO_BINUM7_SHFT                                                                0x18
#define HWIO_GPRTBIMAPLO_BINUM6_BMSK                                                            0xf00000
#define HWIO_GPRTBIMAPLO_BINUM6_SHFT                                                                0x14
#define HWIO_GPRTBIMAPLO_BINUM5_BMSK                                                             0xf0000
#define HWIO_GPRTBIMAPLO_BINUM5_SHFT                                                                0x10
#define HWIO_GPRTBIMAPLO_BINUM4_BMSK                                                              0xf000
#define HWIO_GPRTBIMAPLO_BINUM4_SHFT                                                                 0xc
#define HWIO_GPRTBIMAPLO_BINUM3_BMSK                                                               0xf00
#define HWIO_GPRTBIMAPLO_BINUM3_SHFT                                                                 0x8
#define HWIO_GPRTBIMAPLO_BINUM2_BMSK                                                                0xf0
#define HWIO_GPRTBIMAPLO_BINUM2_SHFT                                                                 0x4
#define HWIO_GPRTBIMAPLO_BINUM1_BMSK                                                                 0xf
#define HWIO_GPRTBIMAPLO_BINUM1_SHFT                                                                 0x0

#define HWIO_GPRTBIMAPHI_ADDR(x)                                                              ((x) + 0x0000c13c)
#define HWIO_GPRTBIMAPHI_OFFS                                                                 (0x0000c13c)
#define HWIO_GPRTBIMAPHI_RMSK                                                                  0xfffffff
#define HWIO_GPRTBIMAPHI_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAPHI_ADDR(x), HWIO_GPRTBIMAPHI_RMSK)
#define HWIO_GPRTBIMAPHI_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAPHI_ADDR(x), m)
#define HWIO_GPRTBIMAPHI_BINUM15_BMSK                                                          0xf000000
#define HWIO_GPRTBIMAPHI_BINUM15_SHFT                                                               0x18
#define HWIO_GPRTBIMAPHI_BINUM14_BMSK                                                           0xf00000
#define HWIO_GPRTBIMAPHI_BINUM14_SHFT                                                               0x14
#define HWIO_GPRTBIMAPHI_BINUM13_BMSK                                                            0xf0000
#define HWIO_GPRTBIMAPHI_BINUM13_SHFT                                                               0x10
#define HWIO_GPRTBIMAPHI_BINUM12_BMSK                                                             0xf000
#define HWIO_GPRTBIMAPHI_BINUM12_SHFT                                                                0xc
#define HWIO_GPRTBIMAPHI_BINUM11_BMSK                                                              0xf00
#define HWIO_GPRTBIMAPHI_BINUM11_SHFT                                                                0x8
#define HWIO_GPRTBIMAPHI_BINUM10_BMSK                                                               0xf0
#define HWIO_GPRTBIMAPHI_BINUM10_SHFT                                                                0x4
#define HWIO_GPRTBIMAPHI_BINUM9_BMSK                                                                 0xf
#define HWIO_GPRTBIMAPHI_BINUM9_SHFT                                                                 0x0

#define HWIO_GHWPARAMS0_ADDR(x)                                                               ((x) + 0x0000c140)
#define HWIO_GHWPARAMS0_OFFS                                                                  (0x0000c140)
#define HWIO_GHWPARAMS0_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS0_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS0_ADDR(x), HWIO_GHWPARAMS0_RMSK)
#define HWIO_GHWPARAMS0_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS0_ADDR(x), m)
#define HWIO_GHWPARAMS0_GHWPARAMS0_31_24_BMSK                                                 0xff000000
#define HWIO_GHWPARAMS0_GHWPARAMS0_31_24_SHFT                                                       0x18
#define HWIO_GHWPARAMS0_GHWPARAMS0_23_16_BMSK                                                   0xff0000
#define HWIO_GHWPARAMS0_GHWPARAMS0_23_16_SHFT                                                       0x10
#define HWIO_GHWPARAMS0_GHWPARAMS0_15_8_BMSK                                                      0xff00
#define HWIO_GHWPARAMS0_GHWPARAMS0_15_8_SHFT                                                         0x8
#define HWIO_GHWPARAMS0_GHWPARAMS0_7_6_BMSK                                                         0xc0
#define HWIO_GHWPARAMS0_GHWPARAMS0_7_6_SHFT                                                          0x6
#define HWIO_GHWPARAMS0_GHWPARAMS0_5_3_BMSK                                                         0x38
#define HWIO_GHWPARAMS0_GHWPARAMS0_5_3_SHFT                                                          0x3
#define HWIO_GHWPARAMS0_GHWPARAMS0_2_0_BMSK                                                          0x7
#define HWIO_GHWPARAMS0_GHWPARAMS0_2_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS1_ADDR(x)                                                               ((x) + 0x0000c144)
#define HWIO_GHWPARAMS1_OFFS                                                                  (0x0000c144)
#define HWIO_GHWPARAMS1_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS1_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS1_ADDR(x), HWIO_GHWPARAMS1_RMSK)
#define HWIO_GHWPARAMS1_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS1_ADDR(x), m)
#define HWIO_GHWPARAMS1_GHWPARAMS1_31_BMSK                                                    0x80000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_31_SHFT                                                          0x1f
#define HWIO_GHWPARAMS1_GHWPARAMS1_30_BMSK                                                    0x40000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_30_SHFT                                                          0x1e
#define HWIO_GHWPARAMS1_GHWPARAMS1_29_BMSK                                                    0x20000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_29_SHFT                                                          0x1d
#define HWIO_GHWPARAMS1_GHWPARAMS1_28_BMSK                                                    0x10000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_28_SHFT                                                          0x1c
#define HWIO_GHWPARAMS1_GHWPARAMS1_27_BMSK                                                     0x8000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_27_SHFT                                                          0x1b
#define HWIO_GHWPARAMS1_GHWPARAMS1_26_BMSK                                                     0x4000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_26_SHFT                                                          0x1a
#define HWIO_GHWPARAMS1_GHWPARAMS1_25_24_BMSK                                                  0x3000000
#define HWIO_GHWPARAMS1_GHWPARAMS1_25_24_SHFT                                                       0x18
#define HWIO_GHWPARAMS1_GHWPARAMS1_23_BMSK                                                      0x800000
#define HWIO_GHWPARAMS1_GHWPARAMS1_23_SHFT                                                          0x17
#define HWIO_GHWPARAMS1_GHWPARAMS1_22_21_BMSK                                                   0x600000
#define HWIO_GHWPARAMS1_GHWPARAMS1_22_21_SHFT                                                       0x15
#define HWIO_GHWPARAMS1_GHWPARAMS1_20_15_BMSK                                                   0x1f8000
#define HWIO_GHWPARAMS1_GHWPARAMS1_20_15_SHFT                                                        0xf
#define HWIO_GHWPARAMS1_GHWPARAMS1_14_12_BMSK                                                     0x7000
#define HWIO_GHWPARAMS1_GHWPARAMS1_14_12_SHFT                                                        0xc
#define HWIO_GHWPARAMS1_GHWPARAMS1_11_9_BMSK                                                       0xe00
#define HWIO_GHWPARAMS1_GHWPARAMS1_11_9_SHFT                                                         0x9
#define HWIO_GHWPARAMS1_GHWPARAMS1_8_6_BMSK                                                        0x1c0
#define HWIO_GHWPARAMS1_GHWPARAMS1_8_6_SHFT                                                          0x6
#define HWIO_GHWPARAMS1_GHWPARAMS1_5_3_BMSK                                                         0x38
#define HWIO_GHWPARAMS1_GHWPARAMS1_5_3_SHFT                                                          0x3
#define HWIO_GHWPARAMS1_GHWPARAMS1_2_0_BMSK                                                          0x7
#define HWIO_GHWPARAMS1_GHWPARAMS1_2_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS2_ADDR(x)                                                               ((x) + 0x0000c148)
#define HWIO_GHWPARAMS2_OFFS                                                                  (0x0000c148)
#define HWIO_GHWPARAMS2_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS2_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS2_ADDR(x), HWIO_GHWPARAMS2_RMSK)
#define HWIO_GHWPARAMS2_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS2_ADDR(x), m)
#define HWIO_GHWPARAMS2_GHWPARAMS2_31_0_BMSK                                                  0xffffffff
#define HWIO_GHWPARAMS2_GHWPARAMS2_31_0_SHFT                                                         0x0

#define HWIO_GHWPARAMS3_ADDR(x)                                                               ((x) + 0x0000c14c)
#define HWIO_GHWPARAMS3_OFFS                                                                  (0x0000c14c)
#define HWIO_GHWPARAMS3_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS3_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS3_ADDR(x), HWIO_GHWPARAMS3_RMSK)
#define HWIO_GHWPARAMS3_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS3_ADDR(x), m)
#define HWIO_GHWPARAMS3_GHWPARAMS3_31_BMSK                                                    0x80000000
#define HWIO_GHWPARAMS3_GHWPARAMS3_31_SHFT                                                          0x1f
#define HWIO_GHWPARAMS3_GHWPARAMS3_30_23_BMSK                                                 0x7f800000
#define HWIO_GHWPARAMS3_GHWPARAMS3_30_23_SHFT                                                       0x17
#define HWIO_GHWPARAMS3_GHWPARAMS3_22_18_BMSK                                                   0x7c0000
#define HWIO_GHWPARAMS3_GHWPARAMS3_22_18_SHFT                                                       0x12
#define HWIO_GHWPARAMS3_GHWPARAMS3_17_12_BMSK                                                    0x3f000
#define HWIO_GHWPARAMS3_GHWPARAMS3_17_12_SHFT                                                        0xc
#define HWIO_GHWPARAMS3_GHWPARAMS3_11_BMSK                                                         0x800
#define HWIO_GHWPARAMS3_GHWPARAMS3_11_SHFT                                                           0xb
#define HWIO_GHWPARAMS3_GHWPARAMS3_10_BMSK                                                         0x400
#define HWIO_GHWPARAMS3_GHWPARAMS3_10_SHFT                                                           0xa
#define HWIO_GHWPARAMS3_GHWPARAMS3_9_8_BMSK                                                        0x300
#define HWIO_GHWPARAMS3_GHWPARAMS3_9_8_SHFT                                                          0x8
#define HWIO_GHWPARAMS3_GHWPARAMS3_7_6_BMSK                                                         0xc0
#define HWIO_GHWPARAMS3_GHWPARAMS3_7_6_SHFT                                                          0x6
#define HWIO_GHWPARAMS3_GHWPARAMS3_5_4_BMSK                                                         0x30
#define HWIO_GHWPARAMS3_GHWPARAMS3_5_4_SHFT                                                          0x4
#define HWIO_GHWPARAMS3_GHWPARAMS3_3_2_BMSK                                                          0xc
#define HWIO_GHWPARAMS3_GHWPARAMS3_3_2_SHFT                                                          0x2
#define HWIO_GHWPARAMS3_GHWPARAMS3_1_0_BMSK                                                          0x3
#define HWIO_GHWPARAMS3_GHWPARAMS3_1_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS4_ADDR(x)                                                               ((x) + 0x0000c150)
#define HWIO_GHWPARAMS4_OFFS                                                                  (0x0000c150)
#define HWIO_GHWPARAMS4_RMSK                                                                  0xffffe07f
#define HWIO_GHWPARAMS4_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS4_ADDR(x), HWIO_GHWPARAMS4_RMSK)
#define HWIO_GHWPARAMS4_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS4_ADDR(x), m)
#define HWIO_GHWPARAMS4_GHWPARAMS4_31_28_BMSK                                                 0xf0000000
#define HWIO_GHWPARAMS4_GHWPARAMS4_31_28_SHFT                                                       0x1c
#define HWIO_GHWPARAMS4_GHWPARAMS4_27_24_BMSK                                                  0xf000000
#define HWIO_GHWPARAMS4_GHWPARAMS4_27_24_SHFT                                                       0x18
#define HWIO_GHWPARAMS4_GHWPARAMS4_23_BMSK                                                      0x800000
#define HWIO_GHWPARAMS4_GHWPARAMS4_23_SHFT                                                          0x17
#define HWIO_GHWPARAMS4_GHWPARAMS4_22_BMSK                                                      0x400000
#define HWIO_GHWPARAMS4_GHWPARAMS4_22_SHFT                                                          0x16
#define HWIO_GHWPARAMS4_GHWPARAMS4_21_BMSK                                                      0x200000
#define HWIO_GHWPARAMS4_GHWPARAMS4_21_SHFT                                                          0x15
#define HWIO_GHWPARAMS4_GHWPARAMS4_20_17_BMSK                                                   0x1e0000
#define HWIO_GHWPARAMS4_GHWPARAMS4_20_17_SHFT                                                       0x11
#define HWIO_GHWPARAMS4_GHWPARAMS4_16_13_BMSK                                                    0x1e000
#define HWIO_GHWPARAMS4_GHWPARAMS4_16_13_SHFT                                                        0xd
#define HWIO_GHWPARAMS4_GHWPARAMS4_6_BMSK                                                           0x40
#define HWIO_GHWPARAMS4_GHWPARAMS4_6_SHFT                                                            0x6
#define HWIO_GHWPARAMS4_GHWPARAMS4_5_0_BMSK                                                         0x3f
#define HWIO_GHWPARAMS4_GHWPARAMS4_5_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS5_ADDR(x)                                                               ((x) + 0x0000c154)
#define HWIO_GHWPARAMS5_OFFS                                                                  (0x0000c154)
#define HWIO_GHWPARAMS5_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS5_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS5_ADDR(x), HWIO_GHWPARAMS5_RMSK)
#define HWIO_GHWPARAMS5_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS5_ADDR(x), m)
#define HWIO_GHWPARAMS5_GHWPARAMS5_31_28_BMSK                                                 0xf0000000
#define HWIO_GHWPARAMS5_GHWPARAMS5_31_28_SHFT                                                       0x1c
#define HWIO_GHWPARAMS5_GHWPARAMS5_27_22_BMSK                                                  0xfc00000
#define HWIO_GHWPARAMS5_GHWPARAMS5_27_22_SHFT                                                       0x16
#define HWIO_GHWPARAMS5_GHWPARAMS5_21_16_BMSK                                                   0x3f0000
#define HWIO_GHWPARAMS5_GHWPARAMS5_21_16_SHFT                                                       0x10
#define HWIO_GHWPARAMS5_GHWPARAMS5_15_10_BMSK                                                     0xfc00
#define HWIO_GHWPARAMS5_GHWPARAMS5_15_10_SHFT                                                        0xa
#define HWIO_GHWPARAMS5_GHWPARAMS5_9_4_BMSK                                                        0x3f0
#define HWIO_GHWPARAMS5_GHWPARAMS5_9_4_SHFT                                                          0x4
#define HWIO_GHWPARAMS5_GHWPARAMS5_3_0_BMSK                                                          0xf
#define HWIO_GHWPARAMS5_GHWPARAMS5_3_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS6_ADDR(x)                                                               ((x) + 0x0000c158)
#define HWIO_GHWPARAMS6_OFFS                                                                  (0x0000c158)
#define HWIO_GHWPARAMS6_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS6_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS6_ADDR(x), HWIO_GHWPARAMS6_RMSK)
#define HWIO_GHWPARAMS6_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS6_ADDR(x), m)
#define HWIO_GHWPARAMS6_GHWPARAMS6_31_16_BMSK                                                 0xffff0000
#define HWIO_GHWPARAMS6_GHWPARAMS6_31_16_SHFT                                                       0x10
#define HWIO_GHWPARAMS6_BUSFLTRSSUPPORT_BMSK                                                      0x8000
#define HWIO_GHWPARAMS6_BUSFLTRSSUPPORT_SHFT                                                         0xf
#define HWIO_GHWPARAMS6_BCSUPPORT_BMSK                                                            0x4000
#define HWIO_GHWPARAMS6_BCSUPPORT_SHFT                                                               0xe
#define HWIO_GHWPARAMS6_OTG_SS_SUPPORT_BMSK                                                       0x2000
#define HWIO_GHWPARAMS6_OTG_SS_SUPPORT_SHFT                                                          0xd
#define HWIO_GHWPARAMS6_ADPSUPPORT_BMSK                                                           0x1000
#define HWIO_GHWPARAMS6_ADPSUPPORT_SHFT                                                              0xc
#define HWIO_GHWPARAMS6_HNPSUPPORT_BMSK                                                            0x800
#define HWIO_GHWPARAMS6_HNPSUPPORT_SHFT                                                              0xb
#define HWIO_GHWPARAMS6_SRPSUPPORT_BMSK                                                            0x400
#define HWIO_GHWPARAMS6_SRPSUPPORT_SHFT                                                              0xa
#define HWIO_GHWPARAMS6_GHWPARAMS6_9_8_BMSK                                                        0x300
#define HWIO_GHWPARAMS6_GHWPARAMS6_9_8_SHFT                                                          0x8
#define HWIO_GHWPARAMS6_GHWPARAMS6_7_BMSK                                                           0x80
#define HWIO_GHWPARAMS6_GHWPARAMS6_7_SHFT                                                            0x7
#define HWIO_GHWPARAMS6_GHWPARAMS6_6_BMSK                                                           0x40
#define HWIO_GHWPARAMS6_GHWPARAMS6_6_SHFT                                                            0x6
#define HWIO_GHWPARAMS6_GHWPARAMS6_5_0_BMSK                                                         0x3f
#define HWIO_GHWPARAMS6_GHWPARAMS6_5_0_SHFT                                                          0x0

#define HWIO_GHWPARAMS7_ADDR(x)                                                               ((x) + 0x0000c15c)
#define HWIO_GHWPARAMS7_OFFS                                                                  (0x0000c15c)
#define HWIO_GHWPARAMS7_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS7_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS7_ADDR(x), HWIO_GHWPARAMS7_RMSK)
#define HWIO_GHWPARAMS7_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS7_ADDR(x), m)
#define HWIO_GHWPARAMS7_GHWPARAMS7_31_16_BMSK                                                 0xffff0000
#define HWIO_GHWPARAMS7_GHWPARAMS7_31_16_SHFT                                                       0x10
#define HWIO_GHWPARAMS7_GHWPARAMS7_15_0_BMSK                                                      0xffff
#define HWIO_GHWPARAMS7_GHWPARAMS7_15_0_SHFT                                                         0x0

#define HWIO_GDBGFIFOSPACE_ADDR(x)                                                            ((x) + 0x0000c160)
#define HWIO_GDBGFIFOSPACE_OFFS                                                               (0x0000c160)
#define HWIO_GDBGFIFOSPACE_RMSK                                                               0xffff00ff
#define HWIO_GDBGFIFOSPACE_IN(x)      \
        in_dword_masked(HWIO_GDBGFIFOSPACE_ADDR(x), HWIO_GDBGFIFOSPACE_RMSK)
#define HWIO_GDBGFIFOSPACE_INM(x, m)      \
        in_dword_masked(HWIO_GDBGFIFOSPACE_ADDR(x), m)
#define HWIO_GDBGFIFOSPACE_OUT(x, v)      \
        out_dword(HWIO_GDBGFIFOSPACE_ADDR(x),v)
#define HWIO_GDBGFIFOSPACE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GDBGFIFOSPACE_ADDR(x),m,v,HWIO_GDBGFIFOSPACE_IN(x))
#define HWIO_GDBGFIFOSPACE_SPACE_AVAILABLE_BMSK                                               0xffff0000
#define HWIO_GDBGFIFOSPACE_SPACE_AVAILABLE_SHFT                                                     0x10
#define HWIO_GDBGFIFOSPACE_FIFO_QUEUE_SELECT_BMSK                                                   0xff
#define HWIO_GDBGFIFOSPACE_FIFO_QUEUE_SELECT_SHFT                                                    0x0

#define HWIO_GDBGLTSSM_ADDR(x)                                                                ((x) + 0x0000c164)
#define HWIO_GDBGLTSSM_OFFS                                                                   (0x0000c164)
#define HWIO_GDBGLTSSM_RMSK                                                                   0x3fffffff
#define HWIO_GDBGLTSSM_IN(x)      \
        in_dword_masked(HWIO_GDBGLTSSM_ADDR(x), HWIO_GDBGLTSSM_RMSK)
#define HWIO_GDBGLTSSM_INM(x, m)      \
        in_dword_masked(HWIO_GDBGLTSSM_ADDR(x), m)
#define HWIO_GDBGLTSSM_X3_XS_SWAPPING_BMSK                                                    0x20000000
#define HWIO_GDBGLTSSM_X3_XS_SWAPPING_SHFT                                                          0x1d
#define HWIO_GDBGLTSSM_X3_DS_HOST_SHUTDOWN_BMSK                                               0x10000000
#define HWIO_GDBGLTSSM_X3_DS_HOST_SHUTDOWN_SHFT                                                     0x1c
#define HWIO_GDBGLTSSM_PRTDIRECTION_BMSK                                                       0x8000000
#define HWIO_GDBGLTSSM_PRTDIRECTION_SHFT                                                            0x1b
#define HWIO_GDBGLTSSM_LTDBTIMEOUT_BMSK                                                        0x4000000
#define HWIO_GDBGLTSSM_LTDBTIMEOUT_SHFT                                                             0x1a
#define HWIO_GDBGLTSSM_LTDBLINKSTATE_BMSK                                                      0x3c00000
#define HWIO_GDBGLTSSM_LTDBLINKSTATE_SHFT                                                           0x16
#define HWIO_GDBGLTSSM_LTDBSUBSTATE_BMSK                                                        0x3c0000
#define HWIO_GDBGLTSSM_LTDBSUBSTATE_SHFT                                                            0x12
#define HWIO_GDBGLTSSM_ELASTICBUFFERMODE_BMSK                                                    0x20000
#define HWIO_GDBGLTSSM_ELASTICBUFFERMODE_SHFT                                                       0x11
#define HWIO_GDBGLTSSM_TXELECLDLE_BMSK                                                           0x10000
#define HWIO_GDBGLTSSM_TXELECLDLE_SHFT                                                              0x10
#define HWIO_GDBGLTSSM_RXPOLARITY_BMSK                                                            0x8000
#define HWIO_GDBGLTSSM_RXPOLARITY_SHFT                                                               0xf
#define HWIO_GDBGLTSSM_TXDETRXLOOPBACK_BMSK                                                       0x4000
#define HWIO_GDBGLTSSM_TXDETRXLOOPBACK_SHFT                                                          0xe
#define HWIO_GDBGLTSSM_LTDBPHYCMDSTATE_BMSK                                                       0x3800
#define HWIO_GDBGLTSSM_LTDBPHYCMDSTATE_SHFT                                                          0xb
#define HWIO_GDBGLTSSM_POWERDOWN_BMSK                                                              0x600
#define HWIO_GDBGLTSSM_POWERDOWN_SHFT                                                                0x9
#define HWIO_GDBGLTSSM_RXEQTRAIN_BMSK                                                              0x100
#define HWIO_GDBGLTSSM_RXEQTRAIN_SHFT                                                                0x8
#define HWIO_GDBGLTSSM_TXDEEMPHASIS_BMSK                                                            0xc0
#define HWIO_GDBGLTSSM_TXDEEMPHASIS_SHFT                                                             0x6
#define HWIO_GDBGLTSSM_LTDBCLKSTATE_BMSK                                                            0x38
#define HWIO_GDBGLTSSM_LTDBCLKSTATE_SHFT                                                             0x3
#define HWIO_GDBGLTSSM_TXSWING_BMSK                                                                  0x4
#define HWIO_GDBGLTSSM_TXSWING_SHFT                                                                  0x2
#define HWIO_GDBGLTSSM_RXTERMINATION_BMSK                                                            0x2
#define HWIO_GDBGLTSSM_RXTERMINATION_SHFT                                                            0x1
#define HWIO_GDBGLTSSM_TXONESZEROS_BMSK                                                              0x1
#define HWIO_GDBGLTSSM_TXONESZEROS_SHFT                                                              0x0

#define HWIO_GDBGLNMCC_ADDR(x)                                                                ((x) + 0x0000c168)
#define HWIO_GDBGLNMCC_OFFS                                                                   (0x0000c168)
#define HWIO_GDBGLNMCC_RMSK                                                                        0x1ff
#define HWIO_GDBGLNMCC_IN(x)      \
        in_dword_masked(HWIO_GDBGLNMCC_ADDR(x), HWIO_GDBGLNMCC_RMSK)
#define HWIO_GDBGLNMCC_INM(x, m)      \
        in_dword_masked(HWIO_GDBGLNMCC_ADDR(x), m)
#define HWIO_GDBGLNMCC_LNMCC_BERC_BMSK                                                             0x1ff
#define HWIO_GDBGLNMCC_LNMCC_BERC_SHFT                                                               0x0

#define HWIO_GDBGBMU_ADDR(x)                                                                  ((x) + 0x0000c16c)
#define HWIO_GDBGBMU_OFFS                                                                     (0x0000c16c)
#define HWIO_GDBGBMU_RMSK                                                                     0xffffffff
#define HWIO_GDBGBMU_IN(x)      \
        in_dword_masked(HWIO_GDBGBMU_ADDR(x), HWIO_GDBGBMU_RMSK)
#define HWIO_GDBGBMU_INM(x, m)      \
        in_dword_masked(HWIO_GDBGBMU_ADDR(x), m)
#define HWIO_GDBGBMU_BMU_BCU_BMSK                                                             0xffffff00
#define HWIO_GDBGBMU_BMU_BCU_SHFT                                                                    0x8
#define HWIO_GDBGBMU_BMU_DCU_BMSK                                                                   0xf0
#define HWIO_GDBGBMU_BMU_DCU_SHFT                                                                    0x4
#define HWIO_GDBGBMU_BMU_CCU_BMSK                                                                    0xf
#define HWIO_GDBGBMU_BMU_CCU_SHFT                                                                    0x0

#define HWIO_GDBGLSPMUX_HST_ADDR(x)                                                           ((x) + 0x0000c170)
#define HWIO_GDBGLSPMUX_HST_OFFS                                                              (0x0000c170)
#define HWIO_GDBGLSPMUX_HST_RMSK                                                                0xff3fff
#define HWIO_GDBGLSPMUX_HST_IN(x)      \
        in_dword_masked(HWIO_GDBGLSPMUX_HST_ADDR(x), HWIO_GDBGLSPMUX_HST_RMSK)
#define HWIO_GDBGLSPMUX_HST_INM(x, m)      \
        in_dword_masked(HWIO_GDBGLSPMUX_HST_ADDR(x), m)
#define HWIO_GDBGLSPMUX_HST_OUT(x, v)      \
        out_dword(HWIO_GDBGLSPMUX_HST_ADDR(x),v)
#define HWIO_GDBGLSPMUX_HST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GDBGLSPMUX_HST_ADDR(x),m,v,HWIO_GDBGLSPMUX_HST_IN(x))
#define HWIO_GDBGLSPMUX_HST_LOGIC_ANALYZER_TRACE_BMSK                                           0xff0000
#define HWIO_GDBGLSPMUX_HST_LOGIC_ANALYZER_TRACE_SHFT                                               0x10
#define HWIO_GDBGLSPMUX_HST_HOSTSELECT_BMSK                                                       0x3fff
#define HWIO_GDBGLSPMUX_HST_HOSTSELECT_SHFT                                                          0x0

#define HWIO_GDBGLSP_ADDR(x)                                                                  ((x) + 0x0000c174)
#define HWIO_GDBGLSP_OFFS                                                                     (0x0000c174)
#define HWIO_GDBGLSP_RMSK                                                                     0xffffffff
#define HWIO_GDBGLSP_IN(x)      \
        in_dword_masked(HWIO_GDBGLSP_ADDR(x), HWIO_GDBGLSP_RMSK)
#define HWIO_GDBGLSP_INM(x, m)      \
        in_dword_masked(HWIO_GDBGLSP_ADDR(x), m)
#define HWIO_GDBGLSP_LSPDEBUG_BMSK                                                            0xffffffff
#define HWIO_GDBGLSP_LSPDEBUG_SHFT                                                                   0x0

#define HWIO_GDBGEPINFO0_ADDR(x)                                                              ((x) + 0x0000c178)
#define HWIO_GDBGEPINFO0_OFFS                                                                 (0x0000c178)
#define HWIO_GDBGEPINFO0_RMSK                                                                 0xffffffff
#define HWIO_GDBGEPINFO0_IN(x)      \
        in_dword_masked(HWIO_GDBGEPINFO0_ADDR(x), HWIO_GDBGEPINFO0_RMSK)
#define HWIO_GDBGEPINFO0_INM(x, m)      \
        in_dword_masked(HWIO_GDBGEPINFO0_ADDR(x), m)
#define HWIO_GDBGEPINFO0_EPDEBUG_BMSK                                                         0xffffffff
#define HWIO_GDBGEPINFO0_EPDEBUG_SHFT                                                                0x0

#define HWIO_GDBGEPINFO1_ADDR(x)                                                              ((x) + 0x0000c17c)
#define HWIO_GDBGEPINFO1_OFFS                                                                 (0x0000c17c)
#define HWIO_GDBGEPINFO1_RMSK                                                                 0xffffffff
#define HWIO_GDBGEPINFO1_IN(x)      \
        in_dword_masked(HWIO_GDBGEPINFO1_ADDR(x), HWIO_GDBGEPINFO1_RMSK)
#define HWIO_GDBGEPINFO1_INM(x, m)      \
        in_dword_masked(HWIO_GDBGEPINFO1_ADDR(x), m)
#define HWIO_GDBGEPINFO1_EPDEBUG_BMSK                                                         0xffffffff
#define HWIO_GDBGEPINFO1_EPDEBUG_SHFT                                                                0x0

#define HWIO_GPRTBIMAP_HSLO_ADDR(x)                                                           ((x) + 0x0000c180)
#define HWIO_GPRTBIMAP_HSLO_OFFS                                                              (0x0000c180)
#define HWIO_GPRTBIMAP_HSLO_RMSK                                                              0xffffffff
#define HWIO_GPRTBIMAP_HSLO_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAP_HSLO_ADDR(x), HWIO_GPRTBIMAP_HSLO_RMSK)
#define HWIO_GPRTBIMAP_HSLO_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAP_HSLO_ADDR(x), m)
#define HWIO_GPRTBIMAP_HSLO_OUT(x, v)      \
        out_dword(HWIO_GPRTBIMAP_HSLO_ADDR(x),v)
#define HWIO_GPRTBIMAP_HSLO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GPRTBIMAP_HSLO_ADDR(x),m,v,HWIO_GPRTBIMAP_HSLO_IN(x))
#define HWIO_GPRTBIMAP_HSLO_BINUM8_BMSK                                                       0xf0000000
#define HWIO_GPRTBIMAP_HSLO_BINUM8_SHFT                                                             0x1c
#define HWIO_GPRTBIMAP_HSLO_BINUM7_BMSK                                                        0xf000000
#define HWIO_GPRTBIMAP_HSLO_BINUM7_SHFT                                                             0x18
#define HWIO_GPRTBIMAP_HSLO_BINUM6_BMSK                                                         0xf00000
#define HWIO_GPRTBIMAP_HSLO_BINUM6_SHFT                                                             0x14
#define HWIO_GPRTBIMAP_HSLO_BINUM5_BMSK                                                          0xf0000
#define HWIO_GPRTBIMAP_HSLO_BINUM5_SHFT                                                             0x10
#define HWIO_GPRTBIMAP_HSLO_BINUM4_BMSK                                                           0xf000
#define HWIO_GPRTBIMAP_HSLO_BINUM4_SHFT                                                              0xc
#define HWIO_GPRTBIMAP_HSLO_BINUM3_BMSK                                                            0xf00
#define HWIO_GPRTBIMAP_HSLO_BINUM3_SHFT                                                              0x8
#define HWIO_GPRTBIMAP_HSLO_BINUM2_BMSK                                                             0xf0
#define HWIO_GPRTBIMAP_HSLO_BINUM2_SHFT                                                              0x4
#define HWIO_GPRTBIMAP_HSLO_BINUM1_BMSK                                                              0xf
#define HWIO_GPRTBIMAP_HSLO_BINUM1_SHFT                                                              0x0

#define HWIO_GPRTBIMAP_HSHI_ADDR(x)                                                           ((x) + 0x0000c184)
#define HWIO_GPRTBIMAP_HSHI_OFFS                                                              (0x0000c184)
#define HWIO_GPRTBIMAP_HSHI_RMSK                                                               0xfffffff
#define HWIO_GPRTBIMAP_HSHI_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAP_HSHI_ADDR(x), HWIO_GPRTBIMAP_HSHI_RMSK)
#define HWIO_GPRTBIMAP_HSHI_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAP_HSHI_ADDR(x), m)
#define HWIO_GPRTBIMAP_HSHI_BINUM15_BMSK                                                       0xf000000
#define HWIO_GPRTBIMAP_HSHI_BINUM15_SHFT                                                            0x18
#define HWIO_GPRTBIMAP_HSHI_BINUM14_BMSK                                                        0xf00000
#define HWIO_GPRTBIMAP_HSHI_BINUM14_SHFT                                                            0x14
#define HWIO_GPRTBIMAP_HSHI_BINUM13_BMSK                                                         0xf0000
#define HWIO_GPRTBIMAP_HSHI_BINUM13_SHFT                                                            0x10
#define HWIO_GPRTBIMAP_HSHI_BINUM12_BMSK                                                          0xf000
#define HWIO_GPRTBIMAP_HSHI_BINUM12_SHFT                                                             0xc
#define HWIO_GPRTBIMAP_HSHI_BINUM11_BMSK                                                           0xf00
#define HWIO_GPRTBIMAP_HSHI_BINUM11_SHFT                                                             0x8
#define HWIO_GPRTBIMAP_HSHI_BINUM10_BMSK                                                            0xf0
#define HWIO_GPRTBIMAP_HSHI_BINUM10_SHFT                                                             0x4
#define HWIO_GPRTBIMAP_HSHI_BINUM9_BMSK                                                              0xf
#define HWIO_GPRTBIMAP_HSHI_BINUM9_SHFT                                                              0x0

#define HWIO_GPRTBIMAP_FSLO_ADDR(x)                                                           ((x) + 0x0000c188)
#define HWIO_GPRTBIMAP_FSLO_OFFS                                                              (0x0000c188)
#define HWIO_GPRTBIMAP_FSLO_RMSK                                                              0xffffffff
#define HWIO_GPRTBIMAP_FSLO_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAP_FSLO_ADDR(x), HWIO_GPRTBIMAP_FSLO_RMSK)
#define HWIO_GPRTBIMAP_FSLO_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAP_FSLO_ADDR(x), m)
#define HWIO_GPRTBIMAP_FSLO_OUT(x, v)      \
        out_dword(HWIO_GPRTBIMAP_FSLO_ADDR(x),v)
#define HWIO_GPRTBIMAP_FSLO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GPRTBIMAP_FSLO_ADDR(x),m,v,HWIO_GPRTBIMAP_FSLO_IN(x))
#define HWIO_GPRTBIMAP_FSLO_BINUM8_BMSK                                                       0xf0000000
#define HWIO_GPRTBIMAP_FSLO_BINUM8_SHFT                                                             0x1c
#define HWIO_GPRTBIMAP_FSLO_BINUM7_BMSK                                                        0xf000000
#define HWIO_GPRTBIMAP_FSLO_BINUM7_SHFT                                                             0x18
#define HWIO_GPRTBIMAP_FSLO_BINUM6_BMSK                                                         0xf00000
#define HWIO_GPRTBIMAP_FSLO_BINUM6_SHFT                                                             0x14
#define HWIO_GPRTBIMAP_FSLO_BINUM5_BMSK                                                          0xf0000
#define HWIO_GPRTBIMAP_FSLO_BINUM5_SHFT                                                             0x10
#define HWIO_GPRTBIMAP_FSLO_BINUM4_BMSK                                                           0xf000
#define HWIO_GPRTBIMAP_FSLO_BINUM4_SHFT                                                              0xc
#define HWIO_GPRTBIMAP_FSLO_BINUM3_BMSK                                                            0xf00
#define HWIO_GPRTBIMAP_FSLO_BINUM3_SHFT                                                              0x8
#define HWIO_GPRTBIMAP_FSLO_BINUM2_BMSK                                                             0xf0
#define HWIO_GPRTBIMAP_FSLO_BINUM2_SHFT                                                              0x4
#define HWIO_GPRTBIMAP_FSLO_BINUM1_BMSK                                                              0xf
#define HWIO_GPRTBIMAP_FSLO_BINUM1_SHFT                                                              0x0

#define HWIO_GPRTBIMAP_FSHI_ADDR(x)                                                           ((x) + 0x0000c18c)
#define HWIO_GPRTBIMAP_FSHI_OFFS                                                              (0x0000c18c)
#define HWIO_GPRTBIMAP_FSHI_RMSK                                                               0xfffffff
#define HWIO_GPRTBIMAP_FSHI_IN(x)      \
        in_dword_masked(HWIO_GPRTBIMAP_FSHI_ADDR(x), HWIO_GPRTBIMAP_FSHI_RMSK)
#define HWIO_GPRTBIMAP_FSHI_INM(x, m)      \
        in_dword_masked(HWIO_GPRTBIMAP_FSHI_ADDR(x), m)
#define HWIO_GPRTBIMAP_FSHI_BINUM15_BMSK                                                       0xf000000
#define HWIO_GPRTBIMAP_FSHI_BINUM15_SHFT                                                            0x18
#define HWIO_GPRTBIMAP_FSHI_BINUM14_BMSK                                                        0xf00000
#define HWIO_GPRTBIMAP_FSHI_BINUM14_SHFT                                                            0x14
#define HWIO_GPRTBIMAP_FSHI_BINUM13_BMSK                                                         0xf0000
#define HWIO_GPRTBIMAP_FSHI_BINUM13_SHFT                                                            0x10
#define HWIO_GPRTBIMAP_FSHI_BINUM12_BMSK                                                          0xf000
#define HWIO_GPRTBIMAP_FSHI_BINUM12_SHFT                                                             0xc
#define HWIO_GPRTBIMAP_FSHI_BINUM11_BMSK                                                           0xf00
#define HWIO_GPRTBIMAP_FSHI_BINUM11_SHFT                                                             0x8
#define HWIO_GPRTBIMAP_FSHI_BINUM10_BMSK                                                            0xf0
#define HWIO_GPRTBIMAP_FSHI_BINUM10_SHFT                                                             0x4
#define HWIO_GPRTBIMAP_FSHI_BINUM9_BMSK                                                              0xf
#define HWIO_GPRTBIMAP_FSHI_BINUM9_SHFT                                                              0x0

#define HWIO_GHWPARAMS8_ADDR(x)                                                               ((x) + 0x0000c600)
#define HWIO_GHWPARAMS8_OFFS                                                                  (0x0000c600)
#define HWIO_GHWPARAMS8_RMSK                                                                  0xffffffff
#define HWIO_GHWPARAMS8_IN(x)      \
        in_dword_masked(HWIO_GHWPARAMS8_ADDR(x), HWIO_GHWPARAMS8_RMSK)
#define HWIO_GHWPARAMS8_INM(x, m)      \
        in_dword_masked(HWIO_GHWPARAMS8_ADDR(x), m)
#define HWIO_GHWPARAMS8_GHWPARAMS8_32_0_BMSK                                                  0xffffffff
#define HWIO_GHWPARAMS8_GHWPARAMS8_32_0_SHFT                                                         0x0

#define HWIO_GTXFIFOPRIDEV_ADDR(x)                                                            ((x) + 0x0000c610)
#define HWIO_GTXFIFOPRIDEV_OFFS                                                               (0x0000c610)
#define HWIO_GTXFIFOPRIDEV_RMSK                                                                   0xffff
#define HWIO_GTXFIFOPRIDEV_IN(x)      \
        in_dword_masked(HWIO_GTXFIFOPRIDEV_ADDR(x), HWIO_GTXFIFOPRIDEV_RMSK)
#define HWIO_GTXFIFOPRIDEV_INM(x, m)      \
        in_dword_masked(HWIO_GTXFIFOPRIDEV_ADDR(x), m)
#define HWIO_GTXFIFOPRIDEV_OUT(x, v)      \
        out_dword(HWIO_GTXFIFOPRIDEV_ADDR(x),v)
#define HWIO_GTXFIFOPRIDEV_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GTXFIFOPRIDEV_ADDR(x),m,v,HWIO_GTXFIFOPRIDEV_IN(x))
#define HWIO_GTXFIFOPRIDEV_GTXFIFOPRIDEV_BMSK                                                     0xffff
#define HWIO_GTXFIFOPRIDEV_GTXFIFOPRIDEV_SHFT                                                        0x0

#define HWIO_GTXFIFOPRIHST_ADDR(x)                                                            ((x) + 0x0000c618)
#define HWIO_GTXFIFOPRIHST_OFFS                                                               (0x0000c618)
#define HWIO_GTXFIFOPRIHST_RMSK                                                                      0xf
#define HWIO_GTXFIFOPRIHST_IN(x)      \
        in_dword_masked(HWIO_GTXFIFOPRIHST_ADDR(x), HWIO_GTXFIFOPRIHST_RMSK)
#define HWIO_GTXFIFOPRIHST_INM(x, m)      \
        in_dword_masked(HWIO_GTXFIFOPRIHST_ADDR(x), m)
#define HWIO_GTXFIFOPRIHST_OUT(x, v)      \
        out_dword(HWIO_GTXFIFOPRIHST_ADDR(x),v)
#define HWIO_GTXFIFOPRIHST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GTXFIFOPRIHST_ADDR(x),m,v,HWIO_GTXFIFOPRIHST_IN(x))
#define HWIO_GTXFIFOPRIHST_GTXFIFOPRIHST_BMSK                                                        0xf
#define HWIO_GTXFIFOPRIHST_GTXFIFOPRIHST_SHFT                                                        0x0

#define HWIO_GRXFIFOPRIHST_ADDR(x)                                                            ((x) + 0x0000c61c)
#define HWIO_GRXFIFOPRIHST_OFFS                                                               (0x0000c61c)
#define HWIO_GRXFIFOPRIHST_RMSK                                                                      0x7
#define HWIO_GRXFIFOPRIHST_IN(x)      \
        in_dword_masked(HWIO_GRXFIFOPRIHST_ADDR(x), HWIO_GRXFIFOPRIHST_RMSK)
#define HWIO_GRXFIFOPRIHST_INM(x, m)      \
        in_dword_masked(HWIO_GRXFIFOPRIHST_ADDR(x), m)
#define HWIO_GRXFIFOPRIHST_OUT(x, v)      \
        out_dword(HWIO_GRXFIFOPRIHST_ADDR(x),v)
#define HWIO_GRXFIFOPRIHST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GRXFIFOPRIHST_ADDR(x),m,v,HWIO_GRXFIFOPRIHST_IN(x))
#define HWIO_GRXFIFOPRIHST_GRXFIFOPRIHST_BMSK                                                        0x7
#define HWIO_GRXFIFOPRIHST_GRXFIFOPRIHST_SHFT                                                        0x0

#define HWIO_GFIFOPRIDBC_ADDR(x)                                                              ((x) + 0x0000c620)
#define HWIO_GFIFOPRIDBC_OFFS                                                                 (0x0000c620)
#define HWIO_GFIFOPRIDBC_RMSK                                                                        0x3
#define HWIO_GFIFOPRIDBC_IN(x)      \
        in_dword_masked(HWIO_GFIFOPRIDBC_ADDR(x), HWIO_GFIFOPRIDBC_RMSK)
#define HWIO_GFIFOPRIDBC_INM(x, m)      \
        in_dword_masked(HWIO_GFIFOPRIDBC_ADDR(x), m)
#define HWIO_GFIFOPRIDBC_OUT(x, v)      \
        out_dword(HWIO_GFIFOPRIDBC_ADDR(x),v)
#define HWIO_GFIFOPRIDBC_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GFIFOPRIDBC_ADDR(x),m,v,HWIO_GFIFOPRIDBC_IN(x))
#define HWIO_GFIFOPRIDBC_GFIFOPRIDBC_BMSK                                                            0x3
#define HWIO_GFIFOPRIDBC_GFIFOPRIDBC_SHFT                                                            0x0

#define HWIO_GDMAHLRATIO_ADDR(x)                                                              ((x) + 0x0000c624)
#define HWIO_GDMAHLRATIO_OFFS                                                                 (0x0000c624)
#define HWIO_GDMAHLRATIO_RMSK                                                                     0x1f1f
#define HWIO_GDMAHLRATIO_IN(x)      \
        in_dword_masked(HWIO_GDMAHLRATIO_ADDR(x), HWIO_GDMAHLRATIO_RMSK)
#define HWIO_GDMAHLRATIO_INM(x, m)      \
        in_dword_masked(HWIO_GDMAHLRATIO_ADDR(x), m)
#define HWIO_GDMAHLRATIO_OUT(x, v)      \
        out_dword(HWIO_GDMAHLRATIO_ADDR(x),v)
#define HWIO_GDMAHLRATIO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GDMAHLRATIO_ADDR(x),m,v,HWIO_GDMAHLRATIO_IN(x))
#define HWIO_GDMAHLRATIO_HSTRXFIFO_BMSK                                                           0x1f00
#define HWIO_GDMAHLRATIO_HSTRXFIFO_SHFT                                                              0x8
#define HWIO_GDMAHLRATIO_HSTTXFIFO_BMSK                                                             0x1f
#define HWIO_GDMAHLRATIO_HSTTXFIFO_SHFT                                                              0x0

#define HWIO_GFLADJ_ADDR(x)                                                                   ((x) + 0x0000c630)
#define HWIO_GFLADJ_OFFS                                                                      (0x0000c630)
#define HWIO_GFLADJ_RMSK                                                                      0xffbfffbf
#define HWIO_GFLADJ_IN(x)      \
        in_dword_masked(HWIO_GFLADJ_ADDR(x), HWIO_GFLADJ_RMSK)
#define HWIO_GFLADJ_INM(x, m)      \
        in_dword_masked(HWIO_GFLADJ_ADDR(x), m)
#define HWIO_GFLADJ_OUT(x, v)      \
        out_dword(HWIO_GFLADJ_ADDR(x),v)
#define HWIO_GFLADJ_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GFLADJ_ADDR(x),m,v,HWIO_GFLADJ_IN(x))
#define HWIO_GFLADJ_GFLADJ_REFCLK_240MHZDECR_PLS1_BMSK                                        0x80000000
#define HWIO_GFLADJ_GFLADJ_REFCLK_240MHZDECR_PLS1_SHFT                                              0x1f
#define HWIO_GFLADJ_GFLADJ_REFCLK_240MHZ_DECR_BMSK                                            0x7f000000
#define HWIO_GFLADJ_GFLADJ_REFCLK_240MHZ_DECR_SHFT                                                  0x18
#define HWIO_GFLADJ_GFLADJ_REFCLK_LPM_SEL_BMSK                                                  0x800000
#define HWIO_GFLADJ_GFLADJ_REFCLK_LPM_SEL_SHFT                                                      0x17
#define HWIO_GFLADJ_GFLADJ_REFCLK_FLADJ_BMSK                                                    0x3fff00
#define HWIO_GFLADJ_GFLADJ_REFCLK_FLADJ_SHFT                                                         0x8
#define HWIO_GFLADJ_GFLADJ_30MHZ_SDBND_SEL_BMSK                                                     0x80
#define HWIO_GFLADJ_GFLADJ_30MHZ_SDBND_SEL_SHFT                                                      0x7
#define HWIO_GFLADJ_GFLADJ_30MHZ_BMSK                                                               0x3f
#define HWIO_GFLADJ_GFLADJ_30MHZ_SHFT                                                                0x0

#define HWIO_RSVD_ADDR(base,p)                                                    ((base) + 0x0000c780 + 0x4 * (p))
#define HWIO_RSVD_OFFS(base,p)                                                    (0x0000c780 + 0x4 * (p))
#define HWIO_RSVD_RMSK                                                            0xffffffff
#define HWIO_RSVD_MAXp                                                                    31
#define HWIO_RSVD_INI(base,p)        \
        in_dword_masked(HWIO_RSVD_ADDR(base,p), HWIO_RSVD_RMSK)
#define HWIO_RSVD_INMI(base,p,mask)    \
        in_dword_masked(HWIO_RSVD_ADDR(base,p), mask)
#define HWIO_RSVD_RSVD32_0_BMSK                                                   0xffffffff
#define HWIO_RSVD_RSVD32_0_SHFT                                                          0x0

#define HWIO_DEPCMDPAR2_ADDR(base,p)                                        ((base) + 0x0000c800 + 0x10 * (p))
#define HWIO_DEPCMDPAR2_OFFS(base,p)                                        (0x0000c800 + 0x10 * (p))
#define HWIO_DEPCMDPAR2_RMSK                                                0xffffffff
#define HWIO_DEPCMDPAR2_MAXp                                                        31
#define HWIO_DEPCMDPAR2_INI(base,p)        \
        in_dword_masked(HWIO_DEPCMDPAR2_ADDR(base,p), HWIO_DEPCMDPAR2_RMSK)
#define HWIO_DEPCMDPAR2_INMI(base,p,mask)    \
        in_dword_masked(HWIO_DEPCMDPAR2_ADDR(base,p), mask)
#define HWIO_DEPCMDPAR2_OUTI(base,p,val)    \
        out_dword(HWIO_DEPCMDPAR2_ADDR(base,p),val)
#define HWIO_DEPCMDPAR2_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_DEPCMDPAR2_ADDR(base,p),mask,val,HWIO_DEPCMDPAR2_INI(base,p))
#define HWIO_DEPCMDPAR2_PARAMETER_BMSK                                      0xffffffff
#define HWIO_DEPCMDPAR2_PARAMETER_SHFT                                             0x0

#define HWIO_DEPCMDPAR1_ADDR(base,p)                                        ((base) + 0x0000c804 + 0x10 * (p))
#define HWIO_DEPCMDPAR1_OFFS(base,p)                                        (0x0000c804 + 0x10 * (p))
#define HWIO_DEPCMDPAR1_RMSK                                                0xffffffff
#define HWIO_DEPCMDPAR1_MAXp                                                        31
#define HWIO_DEPCMDPAR1_INI(base,p)        \
        in_dword_masked(HWIO_DEPCMDPAR1_ADDR(base,p), HWIO_DEPCMDPAR1_RMSK)
#define HWIO_DEPCMDPAR1_INMI(base,p,mask)    \
        in_dword_masked(HWIO_DEPCMDPAR1_ADDR(base,p), mask)
#define HWIO_DEPCMDPAR1_OUTI(base,p,val)    \
        out_dword(HWIO_DEPCMDPAR1_ADDR(base,p),val)
#define HWIO_DEPCMDPAR1_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_DEPCMDPAR1_ADDR(base,p),mask,val,HWIO_DEPCMDPAR1_INI(base,p))
#define HWIO_DEPCMDPAR1_PARAMETER_BMSK                                      0xffffffff
#define HWIO_DEPCMDPAR1_PARAMETER_SHFT                                             0x0

#define HWIO_DEPCMDPAR0_ADDR(base,p)                                        ((base) + 0x0000c808 + 0x10 * (p))
#define HWIO_DEPCMDPAR0_OFFS(base,p)                                        (0x0000c808 + 0x10 * (p))
#define HWIO_DEPCMDPAR0_RMSK                                                0xffffffff
#define HWIO_DEPCMDPAR0_MAXp                                                        31
#define HWIO_DEPCMDPAR0_INI(base,p)        \
        in_dword_masked(HWIO_DEPCMDPAR0_ADDR(base,p), HWIO_DEPCMDPAR0_RMSK)
#define HWIO_DEPCMDPAR0_INMI(base,p,mask)    \
        in_dword_masked(HWIO_DEPCMDPAR0_ADDR(base,p), mask)
#define HWIO_DEPCMDPAR0_OUTI(base,p,val)    \
        out_dword(HWIO_DEPCMDPAR0_ADDR(base,p),val)
#define HWIO_DEPCMDPAR0_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_DEPCMDPAR0_ADDR(base,p),mask,val,HWIO_DEPCMDPAR0_INI(base,p))
#define HWIO_DEPCMDPAR0_PARAMETER_BMSK                                      0xffffffff
#define HWIO_DEPCMDPAR0_PARAMETER_SHFT                                             0x0

#define HWIO_DEPCMD_ADDR(base,p)                                            ((base) + 0x0000c80c + 0x10 * (p))
#define HWIO_DEPCMD_OFFS(base,p)                                            (0x0000c80c + 0x10 * (p))
#define HWIO_DEPCMD_RMSK                                                    0xffffff0f
#define HWIO_DEPCMD_MAXp                                                            31
#define HWIO_DEPCMD_INI(base,p)        \
        in_dword_masked(HWIO_DEPCMD_ADDR(base,p), HWIO_DEPCMD_RMSK)
#define HWIO_DEPCMD_INMI(base,p,mask)    \
        in_dword_masked(HWIO_DEPCMD_ADDR(base,p), mask)
#define HWIO_DEPCMD_OUTI(base,p,val)    \
        out_dword(HWIO_DEPCMD_ADDR(base,p),val)
#define HWIO_DEPCMD_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_DEPCMD_ADDR(base,p),mask,val,HWIO_DEPCMD_INI(base,p))
#define HWIO_DEPCMD_COMMANDPARAM_BMSK                                       0xffff0000
#define HWIO_DEPCMD_COMMANDPARAM_SHFT                                             0x10
#define HWIO_DEPCMD_CMDSTATUS_BMSK                                              0xf000
#define HWIO_DEPCMD_CMDSTATUS_SHFT                                                 0xc
#define HWIO_DEPCMD_HIPRI_FORCERM_BMSK                                           0x800
#define HWIO_DEPCMD_HIPRI_FORCERM_SHFT                                             0xb
#define HWIO_DEPCMD_CMDACT_BMSK                                                  0x400
#define HWIO_DEPCMD_CMDACT_SHFT                                                    0xa
#define HWIO_DEPCMD_R39_BMSK                                                     0x200
#define HWIO_DEPCMD_R39_SHFT                                                       0x9
#define HWIO_DEPCMD_CMDIOC_BMSK                                                  0x100
#define HWIO_DEPCMD_CMDIOC_SHFT                                                    0x8
#define HWIO_DEPCMD_CMDTYP_BMSK                                                    0xf
#define HWIO_DEPCMD_CMDTYP_SHFT                                                    0x0

#define HWIO_DCFG_ADDR(x)                                                                     ((x) + 0x0000c700)
#define HWIO_DCFG_OFFS                                                                        (0x0000c700)
#define HWIO_DCFG_RMSK                                                                          0xffffff
#define HWIO_DCFG_IN(x)      \
        in_dword_masked(HWIO_DCFG_ADDR(x), HWIO_DCFG_RMSK)
#define HWIO_DCFG_INM(x, m)      \
        in_dword_masked(HWIO_DCFG_ADDR(x), m)
#define HWIO_DCFG_OUT(x, v)      \
        out_dword(HWIO_DCFG_ADDR(x),v)
#define HWIO_DCFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCFG_ADDR(x),m,v,HWIO_DCFG_IN(x))
#define HWIO_DCFG_IGNSTRMPP_BMSK                                                                0x800000
#define HWIO_DCFG_IGNSTRMPP_SHFT                                                                    0x17
#define HWIO_DCFG_LPMCAP_BMSK                                                                   0x400000
#define HWIO_DCFG_LPMCAP_SHFT                                                                       0x16
#define HWIO_DCFG_NUMP_BMSK                                                                     0x3e0000
#define HWIO_DCFG_NUMP_SHFT                                                                         0x11
#define HWIO_DCFG_INTRNUM_BMSK                                                                   0x1f000
#define HWIO_DCFG_INTRNUM_SHFT                                                                       0xc
#define HWIO_DCFG_PERFRINT_BMSK                                                                    0xc00
#define HWIO_DCFG_PERFRINT_SHFT                                                                      0xa
#define HWIO_DCFG_DEVADDR_BMSK                                                                     0x3f8
#define HWIO_DCFG_DEVADDR_SHFT                                                                       0x3
#define HWIO_DCFG_DEVSPD_BMSK                                                                        0x7
#define HWIO_DCFG_DEVSPD_SHFT                                                                        0x0

#define HWIO_DCTL_ADDR(x)                                                                     ((x) + 0x0000c704)
#define HWIO_DCTL_OFFS                                                                        (0x0000c704)
#define HWIO_DCTL_RMSK                                                                        0xdff31ffe
#define HWIO_DCTL_IN(x)      \
        in_dword_masked(HWIO_DCTL_ADDR(x), HWIO_DCTL_RMSK)
#define HWIO_DCTL_INM(x, m)      \
        in_dword_masked(HWIO_DCTL_ADDR(x), m)
#define HWIO_DCTL_OUT(x, v)      \
        out_dword(HWIO_DCTL_ADDR(x),v)
#define HWIO_DCTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCTL_ADDR(x),m,v,HWIO_DCTL_IN(x))
#define HWIO_DCTL_RUN_STOP_BMSK                                                               0x80000000
#define HWIO_DCTL_RUN_STOP_SHFT                                                                     0x1f
#define HWIO_DCTL_CSFTRST_BMSK                                                                0x40000000
#define HWIO_DCTL_CSFTRST_SHFT                                                                      0x1e
#define HWIO_DCTL_HIRDTHRES_BMSK                                                              0x1f000000
#define HWIO_DCTL_HIRDTHRES_SHFT                                                                    0x18
#define HWIO_DCTL_LPM_NYET_THRES_BMSK                                                           0xf00000
#define HWIO_DCTL_LPM_NYET_THRES_SHFT                                                               0x14
#define HWIO_DCTL_CRS_BMSK                                                                       0x20000
#define HWIO_DCTL_CRS_SHFT                                                                          0x11
#define HWIO_DCTL_CSS_BMSK                                                                       0x10000
#define HWIO_DCTL_CSS_SHFT                                                                          0x10
#define HWIO_DCTL_INITU2ENA_BMSK                                                                  0x1000
#define HWIO_DCTL_INITU2ENA_SHFT                                                                     0xc
#define HWIO_DCTL_ACCEPTU2ENA_BMSK                                                                 0x800
#define HWIO_DCTL_ACCEPTU2ENA_SHFT                                                                   0xb
#define HWIO_DCTL_INITU1ENA_BMSK                                                                   0x400
#define HWIO_DCTL_INITU1ENA_SHFT                                                                     0xa
#define HWIO_DCTL_ACCEPTU1ENA_BMSK                                                                 0x200
#define HWIO_DCTL_ACCEPTU1ENA_SHFT                                                                   0x9
#define HWIO_DCTL_ULSTCHNGREQ_BMSK                                                                 0x1e0
#define HWIO_DCTL_ULSTCHNGREQ_SHFT                                                                   0x5
#define HWIO_DCTL_TSTCTL_BMSK                                                                       0x1e
#define HWIO_DCTL_TSTCTL_SHFT                                                                        0x1

#define HWIO_DEVTEN_ADDR(x)                                                                   ((x) + 0x0000c708)
#define HWIO_DEVTEN_OFFS                                                                      (0x0000c708)
#define HWIO_DEVTEN_RMSK                                                                          0x12ff
#define HWIO_DEVTEN_IN(x)      \
        in_dword_masked(HWIO_DEVTEN_ADDR(x), HWIO_DEVTEN_RMSK)
#define HWIO_DEVTEN_INM(x, m)      \
        in_dword_masked(HWIO_DEVTEN_ADDR(x), m)
#define HWIO_DEVTEN_OUT(x, v)      \
        out_dword(HWIO_DEVTEN_ADDR(x),v)
#define HWIO_DEVTEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DEVTEN_ADDR(x),m,v,HWIO_DEVTEN_IN(x))
#define HWIO_DEVTEN_VENDEVTSTRCVDEN_BMSK                                                          0x1000
#define HWIO_DEVTEN_VENDEVTSTRCVDEN_SHFT                                                             0xc
#define HWIO_DEVTEN_ERRTICERREVTEN_BMSK                                                            0x200
#define HWIO_DEVTEN_ERRTICERREVTEN_SHFT                                                              0x9
#define HWIO_DEVTEN_SOFTEVTEN_BMSK                                                                  0x80
#define HWIO_DEVTEN_SOFTEVTEN_SHFT                                                                   0x7
#define HWIO_DEVTEN_U3L2L1SUSPEN_BMSK                                                               0x40
#define HWIO_DEVTEN_U3L2L1SUSPEN_SHFT                                                                0x6
#define HWIO_DEVTEN_HIBERNATIONREQEVTEN_BMSK                                                        0x20
#define HWIO_DEVTEN_HIBERNATIONREQEVTEN_SHFT                                                         0x5
#define HWIO_DEVTEN_WKUPEVTEN_BMSK                                                                  0x10
#define HWIO_DEVTEN_WKUPEVTEN_SHFT                                                                   0x4
#define HWIO_DEVTEN_ULSTCNGEN_BMSK                                                                   0x8
#define HWIO_DEVTEN_ULSTCNGEN_SHFT                                                                   0x3
#define HWIO_DEVTEN_CONNECTDONEEVTEN_BMSK                                                            0x4
#define HWIO_DEVTEN_CONNECTDONEEVTEN_SHFT                                                            0x2
#define HWIO_DEVTEN_USBRSTEVTEN_BMSK                                                                 0x2
#define HWIO_DEVTEN_USBRSTEVTEN_SHFT                                                                 0x1
#define HWIO_DEVTEN_DISSCONNEVTEN_BMSK                                                               0x1
#define HWIO_DEVTEN_DISSCONNEVTEN_SHFT                                                               0x0

#define HWIO_DSTS_ADDR(x)                                                                     ((x) + 0x0000c70c)
#define HWIO_DSTS_OFFS                                                                        (0x0000c70c)
#define HWIO_DSTS_RMSK                                                                        0x2fffffff
#define HWIO_DSTS_IN(x)      \
        in_dword_masked(HWIO_DSTS_ADDR(x), HWIO_DSTS_RMSK)
#define HWIO_DSTS_INM(x, m)      \
        in_dword_masked(HWIO_DSTS_ADDR(x), m)
#define HWIO_DSTS_OUT(x, v)      \
        out_dword(HWIO_DSTS_ADDR(x),v)
#define HWIO_DSTS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DSTS_ADDR(x),m,v,HWIO_DSTS_IN(x))
#define HWIO_DSTS_DCNRD_BMSK                                                                  0x20000000
#define HWIO_DSTS_DCNRD_SHFT                                                                        0x1d
#define HWIO_DSTS_PLC_BMSK                                                                     0x8000000
#define HWIO_DSTS_PLC_SHFT                                                                          0x1b
#define HWIO_DSTS_CSC_BMSK                                                                     0x4000000
#define HWIO_DSTS_CSC_SHFT                                                                          0x1a
#define HWIO_DSTS_RSS_BMSK                                                                     0x2000000
#define HWIO_DSTS_RSS_SHFT                                                                          0x19
#define HWIO_DSTS_SSS_BMSK                                                                     0x1000000
#define HWIO_DSTS_SSS_SHFT                                                                          0x18
#define HWIO_DSTS_COREIDLE_BMSK                                                                 0x800000
#define HWIO_DSTS_COREIDLE_SHFT                                                                     0x17
#define HWIO_DSTS_DEVCTRLHLT_BMSK                                                               0x400000
#define HWIO_DSTS_DEVCTRLHLT_SHFT                                                                   0x16
#define HWIO_DSTS_USBLNKST_BMSK                                                                 0x3c0000
#define HWIO_DSTS_USBLNKST_SHFT                                                                     0x12
#define HWIO_DSTS_RXFIFOEMPTY_BMSK                                                               0x20000
#define HWIO_DSTS_RXFIFOEMPTY_SHFT                                                                  0x11
#define HWIO_DSTS_SOFFN_BMSK                                                                     0x1fff8
#define HWIO_DSTS_SOFFN_SHFT                                                                         0x3
#define HWIO_DSTS_CONNECTSPD_BMSK                                                                    0x7
#define HWIO_DSTS_CONNECTSPD_SHFT                                                                    0x0

#define HWIO_DGCMDPAR_ADDR(x)                                                                 ((x) + 0x0000c710)
#define HWIO_DGCMDPAR_OFFS                                                                    (0x0000c710)
#define HWIO_DGCMDPAR_RMSK                                                                    0xffffffff
#define HWIO_DGCMDPAR_IN(x)      \
        in_dword_masked(HWIO_DGCMDPAR_ADDR(x), HWIO_DGCMDPAR_RMSK)
#define HWIO_DGCMDPAR_INM(x, m)      \
        in_dword_masked(HWIO_DGCMDPAR_ADDR(x), m)
#define HWIO_DGCMDPAR_OUT(x, v)      \
        out_dword(HWIO_DGCMDPAR_ADDR(x),v)
#define HWIO_DGCMDPAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DGCMDPAR_ADDR(x),m,v,HWIO_DGCMDPAR_IN(x))
#define HWIO_DGCMDPAR_PARAMETER_BMSK                                                          0xffffffff
#define HWIO_DGCMDPAR_PARAMETER_SHFT                                                                 0x0

#define HWIO_DGCMD_ADDR(x)                                                                    ((x) + 0x0000c714)
#define HWIO_DGCMD_OFFS                                                                       (0x0000c714)
#define HWIO_DGCMD_RMSK                                                                           0xf5ff
#define HWIO_DGCMD_IN(x)      \
        in_dword_masked(HWIO_DGCMD_ADDR(x), HWIO_DGCMD_RMSK)
#define HWIO_DGCMD_INM(x, m)      \
        in_dword_masked(HWIO_DGCMD_ADDR(x), m)
#define HWIO_DGCMD_OUT(x, v)      \
        out_dword(HWIO_DGCMD_ADDR(x),v)
#define HWIO_DGCMD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DGCMD_ADDR(x),m,v,HWIO_DGCMD_IN(x))
#define HWIO_DGCMD_CMDSTATUS_BMSK                                                                 0xf000
#define HWIO_DGCMD_CMDSTATUS_SHFT                                                                    0xc
#define HWIO_DGCMD_CMDACT_BMSK                                                                     0x400
#define HWIO_DGCMD_CMDACT_SHFT                                                                       0xa
#define HWIO_DGCMD_CMDIOC_BMSK                                                                     0x100
#define HWIO_DGCMD_CMDIOC_SHFT                                                                       0x8
#define HWIO_DGCMD_CMDTYP_BMSK                                                                      0xff
#define HWIO_DGCMD_CMDTYP_SHFT                                                                       0x0

#define HWIO_DALEPENA_ADDR(x)                                                                 ((x) + 0x0000c720)
#define HWIO_DALEPENA_OFFS                                                                    (0x0000c720)
#define HWIO_DALEPENA_RMSK                                                                    0xffffffff
#define HWIO_DALEPENA_IN(x)      \
        in_dword_masked(HWIO_DALEPENA_ADDR(x), HWIO_DALEPENA_RMSK)
#define HWIO_DALEPENA_INM(x, m)      \
        in_dword_masked(HWIO_DALEPENA_ADDR(x), m)
#define HWIO_DALEPENA_OUT(x, v)      \
        out_dword(HWIO_DALEPENA_ADDR(x),v)
#define HWIO_DALEPENA_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DALEPENA_ADDR(x),m,v,HWIO_DALEPENA_IN(x))
#define HWIO_DALEPENA_USBACTEP_BMSK                                                           0xffffffff
#define HWIO_DALEPENA_USBACTEP_SHFT                                                                  0x0

#define HWIO_OCFG_ADDR(x)                                                                     ((x) + 0x0000cc00)
#define HWIO_OCFG_OFFS                                                                        (0x0000cc00)
#define HWIO_OCFG_RMSK                                                                              0x3f
#define HWIO_OCFG_IN(x)      \
        in_dword_masked(HWIO_OCFG_ADDR(x), HWIO_OCFG_RMSK)
#define HWIO_OCFG_INM(x, m)      \
        in_dword_masked(HWIO_OCFG_ADDR(x), m)
#define HWIO_OCFG_OUT(x, v)      \
        out_dword(HWIO_OCFG_ADDR(x),v)
#define HWIO_OCFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_OCFG_ADDR(x),m,v,HWIO_OCFG_IN(x))
#define HWIO_OCFG_DISPRTPWRCUTOFF_BMSK                                                              0x20
#define HWIO_OCFG_DISPRTPWRCUTOFF_SHFT                                                               0x5
#define HWIO_OCFG_OTGHIBDISMASK_BMSK                                                                0x10
#define HWIO_OCFG_OTGHIBDISMASK_SHFT                                                                 0x4
#define HWIO_OCFG_OTGSFTRSTMSK_BMSK                                                                  0x8
#define HWIO_OCFG_OTGSFTRSTMSK_SHFT                                                                  0x3
#define HWIO_OCFG_OTG_VERSION_BMSK                                                                   0x4
#define HWIO_OCFG_OTG_VERSION_SHFT                                                                   0x2
#define HWIO_OCFG_HNPCAP_BMSK                                                                        0x2
#define HWIO_OCFG_HNPCAP_SHFT                                                                        0x1
#define HWIO_OCFG_SRPCAP_BMSK                                                                        0x1
#define HWIO_OCFG_SRPCAP_SHFT                                                                        0x0

#define HWIO_OCTL_ADDR(x)                                                                     ((x) + 0x0000cc04)
#define HWIO_OCTL_OFFS                                                                        (0x0000cc04)
#define HWIO_OCTL_RMSK                                                                              0xff
#define HWIO_OCTL_IN(x)      \
        in_dword_masked(HWIO_OCTL_ADDR(x), HWIO_OCTL_RMSK)
#define HWIO_OCTL_INM(x, m)      \
        in_dword_masked(HWIO_OCTL_ADDR(x), m)
#define HWIO_OCTL_OUT(x, v)      \
        out_dword(HWIO_OCTL_ADDR(x),v)
#define HWIO_OCTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_OCTL_ADDR(x),m,v,HWIO_OCTL_IN(x))
#define HWIO_OCTL_OTG3_GOERR_BMSK                                                                   0x80
#define HWIO_OCTL_OTG3_GOERR_SHFT                                                                    0x7
#define HWIO_OCTL_PERIMODE_BMSK                                                                     0x40
#define HWIO_OCTL_PERIMODE_SHFT                                                                      0x6
#define HWIO_OCTL_PRTPWRCTL_BMSK                                                                    0x20
#define HWIO_OCTL_PRTPWRCTL_SHFT                                                                     0x5
#define HWIO_OCTL_HNPREQ_BMSK                                                                       0x10
#define HWIO_OCTL_HNPREQ_SHFT                                                                        0x4
#define HWIO_OCTL_SESREQ_BMSK                                                                        0x8
#define HWIO_OCTL_SESREQ_SHFT                                                                        0x3
#define HWIO_OCTL_TERMSELDLPULSE_BMSK                                                                0x4
#define HWIO_OCTL_TERMSELDLPULSE_SHFT                                                                0x2
#define HWIO_OCTL_DEVSETHNPEN_BMSK                                                                   0x2
#define HWIO_OCTL_DEVSETHNPEN_SHFT                                                                   0x1
#define HWIO_OCTL_HSTSETHNPEN_BMSK                                                                   0x1
#define HWIO_OCTL_HSTSETHNPEN_SHFT                                                                   0x0

#define HWIO_OEVT_ADDR(x)                                                                     ((x) + 0x0000cc08)
#define HWIO_OEVT_OFFS                                                                        (0x0000cc08)
#define HWIO_OEVT_RMSK                                                                        0x8fff0f0f
#define HWIO_OEVT_IN(x)      \
        in_dword_masked(HWIO_OEVT_ADDR(x), HWIO_OEVT_RMSK)
#define HWIO_OEVT_INM(x, m)      \
        in_dword_masked(HWIO_OEVT_ADDR(x), m)
#define HWIO_OEVT_OUT(x, v)      \
        out_dword(HWIO_OEVT_ADDR(x),v)
#define HWIO_OEVT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_OEVT_ADDR(x),m,v,HWIO_OEVT_IN(x))
#define HWIO_OEVT_DEVICEMODE_BMSK                                                             0x80000000
#define HWIO_OEVT_DEVICEMODE_SHFT                                                                   0x1f
#define HWIO_OEVT_OTGXHCIRUNSTPSETEVNT_BMSK                                                    0x8000000
#define HWIO_OEVT_OTGXHCIRUNSTPSETEVNT_SHFT                                                         0x1b
#define HWIO_OEVT_OTGDEVRUNSTPSETEVNT_BMSK                                                     0x4000000
#define HWIO_OEVT_OTGDEVRUNSTPSETEVNT_SHFT                                                          0x1a
#define HWIO_OEVT_OTGHIBENTRYEVNT_BMSK                                                         0x2000000
#define HWIO_OEVT_OTGHIBENTRYEVNT_SHFT                                                              0x19
#define HWIO_OEVT_OTGCONIDSTSCHNGEVNT_BMSK                                                     0x1000000
#define HWIO_OEVT_OTGCONIDSTSCHNGEVNT_SHFT                                                          0x18
#define HWIO_OEVT_HRRCONFNOTIFEVNT_BMSK                                                         0x800000
#define HWIO_OEVT_HRRCONFNOTIFEVNT_SHFT                                                             0x17
#define HWIO_OEVT_HRRINITNOTIFEVNT_BMSK                                                         0x400000
#define HWIO_OEVT_HRRINITNOTIFEVNT_SHFT                                                             0x16
#define HWIO_OEVT_OTGADEVIDLEEVNT_BMSK                                                          0x200000
#define HWIO_OEVT_OTGADEVIDLEEVNT_SHFT                                                              0x15
#define HWIO_OEVT_OTGADEVBHOSTENDEVNT_BMSK                                                      0x100000
#define HWIO_OEVT_OTGADEVBHOSTENDEVNT_SHFT                                                          0x14
#define HWIO_OEVT_OTGADEVHOSTEVNT_BMSK                                                           0x80000
#define HWIO_OEVT_OTGADEVHOSTEVNT_SHFT                                                              0x13
#define HWIO_OEVT_OTGADEVHNPCHNGEVNT_BMSK                                                        0x40000
#define HWIO_OEVT_OTGADEVHNPCHNGEVNT_SHFT                                                           0x12
#define HWIO_OEVT_OTGADEVSRPDETEVNT_BMSK                                                         0x20000
#define HWIO_OEVT_OTGADEVSRPDETEVNT_SHFT                                                            0x11
#define HWIO_OEVT_OTGADEVSESSENDDETEVNT_BMSK                                                     0x10000
#define HWIO_OEVT_OTGADEVSESSENDDETEVNT_SHFT                                                        0x10
#define HWIO_OEVT_OTGBDEVBHOSTENDEVNT_BMSK                                                         0x800
#define HWIO_OEVT_OTGBDEVBHOSTENDEVNT_SHFT                                                           0xb
#define HWIO_OEVT_OTGBDEVHNPCHNGEVNT_BMSK                                                          0x400
#define HWIO_OEVT_OTGBDEVHNPCHNGEVNT_SHFT                                                            0xa
#define HWIO_OEVT_OTGBDEVSESSVLDDETEVNT_BMSK                                                       0x200
#define HWIO_OEVT_OTGBDEVSESSVLDDETEVNT_SHFT                                                         0x9
#define HWIO_OEVT_OTGBDEVVBUSCHNGEVNT_BMSK                                                         0x100
#define HWIO_OEVT_OTGBDEVVBUSCHNGEVNT_SHFT                                                           0x8
#define HWIO_OEVT_BSESVLD_BMSK                                                                       0x8
#define HWIO_OEVT_BSESVLD_SHFT                                                                       0x3
#define HWIO_OEVT_HSTNEGSTS_BMSK                                                                     0x4
#define HWIO_OEVT_HSTNEGSTS_SHFT                                                                     0x2
#define HWIO_OEVT_SESREQSTS_BMSK                                                                     0x2
#define HWIO_OEVT_SESREQSTS_SHFT                                                                     0x1
#define HWIO_OEVT_OEVTERROR_BMSK                                                                     0x1
#define HWIO_OEVT_OEVTERROR_SHFT                                                                     0x0

#define HWIO_OEVTEN_ADDR(x)                                                                   ((x) + 0x0000cc0c)
#define HWIO_OEVTEN_OFFS                                                                      (0x0000cc0c)
#define HWIO_OEVTEN_RMSK                                                                       0xfff0f00
#define HWIO_OEVTEN_IN(x)      \
        in_dword_masked(HWIO_OEVTEN_ADDR(x), HWIO_OEVTEN_RMSK)
#define HWIO_OEVTEN_INM(x, m)      \
        in_dword_masked(HWIO_OEVTEN_ADDR(x), m)
#define HWIO_OEVTEN_OUT(x, v)      \
        out_dword(HWIO_OEVTEN_ADDR(x),v)
#define HWIO_OEVTEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_OEVTEN_ADDR(x),m,v,HWIO_OEVTEN_IN(x))
#define HWIO_OEVTEN_OTGXHCIRUNSTPSETEVNTEN_BMSK                                                0x8000000
#define HWIO_OEVTEN_OTGXHCIRUNSTPSETEVNTEN_SHFT                                                     0x1b
#define HWIO_OEVTEN_OTGDEVRUNSTPSETEVNTEN_BMSK                                                 0x4000000
#define HWIO_OEVTEN_OTGDEVRUNSTPSETEVNTEN_SHFT                                                      0x1a
#define HWIO_OEVTEN_OTGHIBENTRYEVNTEN_BMSK                                                     0x2000000
#define HWIO_OEVTEN_OTGHIBENTRYEVNTEN_SHFT                                                          0x19
#define HWIO_OEVTEN_OTGCONIDSTSCHNGEVNTEN_BMSK                                                 0x1000000
#define HWIO_OEVTEN_OTGCONIDSTSCHNGEVNTEN_SHFT                                                      0x18
#define HWIO_OEVTEN_HRRCONFNOTIFEVNTEN_BMSK                                                     0x800000
#define HWIO_OEVTEN_HRRCONFNOTIFEVNTEN_SHFT                                                         0x17
#define HWIO_OEVTEN_HRRINITNOTIFEVNTEN_BMSK                                                     0x400000
#define HWIO_OEVTEN_HRRINITNOTIFEVNTEN_SHFT                                                         0x16
#define HWIO_OEVTEN_OTGADEVIDLEEVNTEN_BMSK                                                      0x200000
#define HWIO_OEVTEN_OTGADEVIDLEEVNTEN_SHFT                                                          0x15
#define HWIO_OEVTEN_OTGADEVBHOSTENDEVNTEN_BMSK                                                  0x100000
#define HWIO_OEVTEN_OTGADEVBHOSTENDEVNTEN_SHFT                                                      0x14
#define HWIO_OEVTEN_OTGADEVHOSTEVNTEN_BMSK                                                       0x80000
#define HWIO_OEVTEN_OTGADEVHOSTEVNTEN_SHFT                                                          0x13
#define HWIO_OEVTEN_OTGADEVHNPCHNGEVNTEN_BMSK                                                    0x40000
#define HWIO_OEVTEN_OTGADEVHNPCHNGEVNTEN_SHFT                                                       0x12
#define HWIO_OEVTEN_OTGADEVSRPDETEVNTEN_BMSK                                                     0x20000
#define HWIO_OEVTEN_OTGADEVSRPDETEVNTEN_SHFT                                                        0x11
#define HWIO_OEVTEN_OTGADEVSESSENDDETEVNTEN_BMSK                                                 0x10000
#define HWIO_OEVTEN_OTGADEVSESSENDDETEVNTEN_SHFT                                                    0x10
#define HWIO_OEVTEN_OTGBDEVBHOSTENDEVNTEN_BMSK                                                     0x800
#define HWIO_OEVTEN_OTGBDEVBHOSTENDEVNTEN_SHFT                                                       0xb
#define HWIO_OEVTEN_OTGBDEVHNPCHNGEVNTEN_BMSK                                                      0x400
#define HWIO_OEVTEN_OTGBDEVHNPCHNGEVNTEN_SHFT                                                        0xa
#define HWIO_OEVTEN_OTGBDEVSESSVLDDETEVNTEN_BMSK                                                   0x200
#define HWIO_OEVTEN_OTGBDEVSESSVLDDETEVNTEN_SHFT                                                     0x9
#define HWIO_OEVTEN_OTGBDEVVBUSCHNGEVNTEN_BMSK                                                     0x100
#define HWIO_OEVTEN_OTGBDEVVBUSCHNGEVNTEN_SHFT                                                       0x8

#define HWIO_OSTS_ADDR(x)                                                                     ((x) + 0x0000cc10)
#define HWIO_OSTS_OFFS                                                                        (0x0000cc10)
#define HWIO_OSTS_RMSK                                                                            0x3f1f
#define HWIO_OSTS_IN(x)      \
        in_dword_masked(HWIO_OSTS_ADDR(x), HWIO_OSTS_RMSK)
#define HWIO_OSTS_INM(x, m)      \
        in_dword_masked(HWIO_OSTS_ADDR(x), m)
#define HWIO_OSTS_DEVRUNSTP_BMSK                                                                  0x2000
#define HWIO_OSTS_DEVRUNSTP_SHFT                                                                     0xd
#define HWIO_OSTS_XHCIRUNSTP_BMSK                                                                 0x1000
#define HWIO_OSTS_XHCIRUNSTP_SHFT                                                                    0xc
#define HWIO_OSTS_OTGSTATE_BMSK                                                                    0xf00
#define HWIO_OSTS_OTGSTATE_SHFT                                                                      0x8
#define HWIO_OSTS_PERIPHERALSTATE_BMSK                                                              0x10
#define HWIO_OSTS_PERIPHERALSTATE_SHFT                                                               0x4
#define HWIO_OSTS_XHCIPRTPOWER_BMSK                                                                  0x8
#define HWIO_OSTS_XHCIPRTPOWER_SHFT                                                                  0x3
#define HWIO_OSTS_BSESVLD_BMSK                                                                       0x4
#define HWIO_OSTS_BSESVLD_SHFT                                                                       0x2
#define HWIO_OSTS_ASESVLD_BMSK                                                                       0x2
#define HWIO_OSTS_ASESVLD_SHFT                                                                       0x1
#define HWIO_OSTS_CONIDSTS_BMSK                                                                      0x1
#define HWIO_OSTS_CONIDSTS_SHFT                                                                      0x0

#define HWIO_ADPEVT_ADDR(x)                                                                   ((x) + 0x0000cc28)
#define HWIO_ADPEVT_OFFS                                                                      (0x0000cc28)
#define HWIO_ADPEVT_RMSK                                                                      0x1e00ffff
#define HWIO_ADPEVT_IN(x)      \
        in_dword_masked(HWIO_ADPEVT_ADDR(x), HWIO_ADPEVT_RMSK)
#define HWIO_ADPEVT_INM(x, m)      \
        in_dword_masked(HWIO_ADPEVT_ADDR(x), m)
#define HWIO_ADPEVT_OUT(x, v)      \
        out_dword(HWIO_ADPEVT_ADDR(x),v)
#define HWIO_ADPEVT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ADPEVT_ADDR(x),m,v,HWIO_ADPEVT_IN(x))
#define HWIO_ADPEVT_ADPPRBEVNT_BMSK                                                           0x10000000
#define HWIO_ADPEVT_ADPPRBEVNT_SHFT                                                                 0x1c
#define HWIO_ADPEVT_ADPSNSEVNT_BMSK                                                            0x8000000
#define HWIO_ADPEVT_ADPSNSEVNT_SHFT                                                                 0x1b
#define HWIO_ADPEVT_ADPTMOUTEVNT_BMSK                                                          0x4000000
#define HWIO_ADPEVT_ADPTMOUTEVNT_SHFT                                                               0x1a
#define HWIO_ADPEVT_ADPRSTCMPLTEVNT_BMSK                                                       0x2000000
#define HWIO_ADPEVT_ADPRSTCMPLTEVNT_SHFT                                                            0x19
#define HWIO_ADPEVT_RTIM_BMSK                                                                     0xffff
#define HWIO_ADPEVT_RTIM_SHFT                                                                        0x0

#define HWIO_RSVD_HC_ADDR(base,p)                                              ((base) + 0x0000001c + 0x4 * (p))
#define HWIO_RSVD_HC_OFFS(base,p)                                              (0x0000001c + 0x4 * (p))
#define HWIO_RSVD_HC_RMSK                                                      0xffffffff
#define HWIO_RSVD_HC_MAXp                                                               0
#define HWIO_RSVD_HC_INI(base,p)        \
        in_dword_masked(HWIO_RSVD_HC_ADDR(base,p), HWIO_RSVD_HC_RMSK)
#define HWIO_RSVD_HC_INMI(base,p,mask)    \
        in_dword_masked(HWIO_RSVD_HC_ADDR(base,p), mask)
#define HWIO_RSVD_HC_RSVD32_0_BMSK                                             0xffffffff
#define HWIO_RSVD_HC_RSVD32_0_SHFT                                                    0x0

#define HWIO_CAPLENGTH_ADDR(x)                                                                ((x) + 0x00000000)
#define HWIO_CAPLENGTH_OFFS                                                                   (0x00000000)
#define HWIO_CAPLENGTH_RMSK                                                                   0xffff00ff
#define HWIO_CAPLENGTH_IN(x)      \
        in_dword_masked(HWIO_CAPLENGTH_ADDR(x), HWIO_CAPLENGTH_RMSK)
#define HWIO_CAPLENGTH_INM(x, m)      \
        in_dword_masked(HWIO_CAPLENGTH_ADDR(x), m)
#define HWIO_CAPLENGTH_HCIVERSION_BMSK                                                        0xffff0000
#define HWIO_CAPLENGTH_HCIVERSION_SHFT                                                              0x10
#define HWIO_CAPLENGTH_CAPLENGTH_BMSK                                                               0xff
#define HWIO_CAPLENGTH_CAPLENGTH_SHFT                                                                0x0

#define HWIO_HCSPARAMS1_ADDR(x)                                                               ((x) + 0x00000004)
#define HWIO_HCSPARAMS1_OFFS                                                                  (0x00000004)
#define HWIO_HCSPARAMS1_RMSK                                                                  0xff07ffff
#define HWIO_HCSPARAMS1_IN(x)      \
        in_dword_masked(HWIO_HCSPARAMS1_ADDR(x), HWIO_HCSPARAMS1_RMSK)
#define HWIO_HCSPARAMS1_INM(x, m)      \
        in_dword_masked(HWIO_HCSPARAMS1_ADDR(x), m)
#define HWIO_HCSPARAMS1_MAXPORTS_BMSK                                                         0xff000000
#define HWIO_HCSPARAMS1_MAXPORTS_SHFT                                                               0x18
#define HWIO_HCSPARAMS1_MAXINTRS_BMSK                                                            0x7ff00
#define HWIO_HCSPARAMS1_MAXINTRS_SHFT                                                                0x8
#define HWIO_HCSPARAMS1_MAXSLOTS_BMSK                                                               0xff
#define HWIO_HCSPARAMS1_MAXSLOTS_SHFT                                                                0x0

#define HWIO_HCSPARAMS2_ADDR(x)                                                               ((x) + 0x00000008)
#define HWIO_HCSPARAMS2_OFFS                                                                  (0x00000008)
#define HWIO_HCSPARAMS2_RMSK                                                                  0xffe000ff
#define HWIO_HCSPARAMS2_IN(x)      \
        in_dword_masked(HWIO_HCSPARAMS2_ADDR(x), HWIO_HCSPARAMS2_RMSK)
#define HWIO_HCSPARAMS2_INM(x, m)      \
        in_dword_masked(HWIO_HCSPARAMS2_ADDR(x), m)
#define HWIO_HCSPARAMS2_MAXSCRATCHPADBUFS_BMSK                                                0xf8000000
#define HWIO_HCSPARAMS2_MAXSCRATCHPADBUFS_SHFT                                                      0x1b
#define HWIO_HCSPARAMS2_SPR_BMSK                                                               0x4000000
#define HWIO_HCSPARAMS2_SPR_SHFT                                                                    0x1a
#define HWIO_HCSPARAMS2_MAXSCRATCHPADBUFS_HI_BMSK                                              0x3e00000
#define HWIO_HCSPARAMS2_MAXSCRATCHPADBUFS_HI_SHFT                                                   0x15
#define HWIO_HCSPARAMS2_ERSTMAX_BMSK                                                                0xf0
#define HWIO_HCSPARAMS2_ERSTMAX_SHFT                                                                 0x4
#define HWIO_HCSPARAMS2_IST_BMSK                                                                     0xf
#define HWIO_HCSPARAMS2_IST_SHFT                                                                     0x0

#define HWIO_HCSPARAMS3_ADDR(x)                                                               ((x) + 0x0000000c)
#define HWIO_HCSPARAMS3_OFFS                                                                  (0x0000000c)
#define HWIO_HCSPARAMS3_RMSK                                                                  0xffff00ff
#define HWIO_HCSPARAMS3_IN(x)      \
        in_dword_masked(HWIO_HCSPARAMS3_ADDR(x), HWIO_HCSPARAMS3_RMSK)
#define HWIO_HCSPARAMS3_INM(x, m)      \
        in_dword_masked(HWIO_HCSPARAMS3_ADDR(x), m)
#define HWIO_HCSPARAMS3_U2_DEVICE_EXIT_LAT_BMSK                                               0xffff0000
#define HWIO_HCSPARAMS3_U2_DEVICE_EXIT_LAT_SHFT                                                     0x10
#define HWIO_HCSPARAMS3_U1_DEVICE_EXIT_LAT_BMSK                                                     0xff
#define HWIO_HCSPARAMS3_U1_DEVICE_EXIT_LAT_SHFT                                                      0x0

#define HWIO_HCCPARAMS_ADDR(x)                                                                ((x) + 0x00000010)
#define HWIO_HCCPARAMS_OFFS                                                                   (0x00000010)
#define HWIO_HCCPARAMS_RMSK                                                                   0xfffff1ff
#define HWIO_HCCPARAMS_IN(x)      \
        in_dword_masked(HWIO_HCCPARAMS_ADDR(x), HWIO_HCCPARAMS_RMSK)
#define HWIO_HCCPARAMS_INM(x, m)      \
        in_dword_masked(HWIO_HCCPARAMS_ADDR(x), m)
#define HWIO_HCCPARAMS_XECP_BMSK                                                              0xffff0000
#define HWIO_HCCPARAMS_XECP_SHFT                                                                    0x10
#define HWIO_HCCPARAMS_MAXPSASIZE_BMSK                                                            0xf000
#define HWIO_HCCPARAMS_MAXPSASIZE_SHFT                                                               0xc
#define HWIO_HCCPARAMS_PAE_BMSK                                                                    0x100
#define HWIO_HCCPARAMS_PAE_SHFT                                                                      0x8
#define HWIO_HCCPARAMS_NSS_BMSK                                                                     0x80
#define HWIO_HCCPARAMS_NSS_SHFT                                                                      0x7
#define HWIO_HCCPARAMS_LTC_BMSK                                                                     0x40
#define HWIO_HCCPARAMS_LTC_SHFT                                                                      0x6
#define HWIO_HCCPARAMS_LHRC_BMSK                                                                    0x20
#define HWIO_HCCPARAMS_LHRC_SHFT                                                                     0x5
#define HWIO_HCCPARAMS_PIND_BMSK                                                                    0x10
#define HWIO_HCCPARAMS_PIND_SHFT                                                                     0x4
#define HWIO_HCCPARAMS_PPC_BMSK                                                                      0x8
#define HWIO_HCCPARAMS_PPC_SHFT                                                                      0x3
#define HWIO_HCCPARAMS_CSZ_BMSK                                                                      0x4
#define HWIO_HCCPARAMS_CSZ_SHFT                                                                      0x2
#define HWIO_HCCPARAMS_BNC_BMSK                                                                      0x2
#define HWIO_HCCPARAMS_BNC_SHFT                                                                      0x1
#define HWIO_HCCPARAMS_AC64_BMSK                                                                     0x1
#define HWIO_HCCPARAMS_AC64_SHFT                                                                     0x0

#define HWIO_DBOFF_ADDR(x)                                                                    ((x) + 0x00000014)
#define HWIO_DBOFF_OFFS                                                                       (0x00000014)
#define HWIO_DBOFF_RMSK                                                                       0xfffffffc
#define HWIO_DBOFF_IN(x)      \
        in_dword_masked(HWIO_DBOFF_ADDR(x), HWIO_DBOFF_RMSK)
#define HWIO_DBOFF_INM(x, m)      \
        in_dword_masked(HWIO_DBOFF_ADDR(x), m)
#define HWIO_DBOFF_DOORBELL_ARRAY_OFFSET_BMSK                                                 0xfffffffc
#define HWIO_DBOFF_DOORBELL_ARRAY_OFFSET_SHFT                                                        0x2

#define HWIO_RTSOFF_ADDR(x)                                                                   ((x) + 0x00000018)
#define HWIO_RTSOFF_OFFS                                                                      (0x00000018)
#define HWIO_RTSOFF_RMSK                                                                      0xffffffe0
#define HWIO_RTSOFF_IN(x)      \
        in_dword_masked(HWIO_RTSOFF_ADDR(x), HWIO_RTSOFF_RMSK)
#define HWIO_RTSOFF_INM(x, m)      \
        in_dword_masked(HWIO_RTSOFF_ADDR(x), m)
#define HWIO_RTSOFF_RUNTIME_REG_SPACE_OFFSET_BMSK                                             0xffffffe0
#define HWIO_RTSOFF_RUNTIME_REG_SPACE_OFFSET_SHFT                                                    0x5

#define HWIO_USBCMD_ADDR(x)                                                                   ((x) + 0x00000020)
#define HWIO_USBCMD_OFFS                                                                      (0x00000020)
#define HWIO_USBCMD_RMSK                                                                           0xf8f
#define HWIO_USBCMD_IN(x)      \
        in_dword_masked(HWIO_USBCMD_ADDR(x), HWIO_USBCMD_RMSK)
#define HWIO_USBCMD_INM(x, m)      \
        in_dword_masked(HWIO_USBCMD_ADDR(x), m)
#define HWIO_USBCMD_OUT(x, v)      \
        out_dword(HWIO_USBCMD_ADDR(x),v)
#define HWIO_USBCMD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_USBCMD_ADDR(x),m,v,HWIO_USBCMD_IN(x))
#define HWIO_USBCMD_EU3S_BMSK                                                                      0x800
#define HWIO_USBCMD_EU3S_SHFT                                                                        0xb
#define HWIO_USBCMD_EWE_BMSK                                                                       0x400
#define HWIO_USBCMD_EWE_SHFT                                                                         0xa
#define HWIO_USBCMD_CRS_BMSK                                                                       0x200
#define HWIO_USBCMD_CRS_SHFT                                                                         0x9
#define HWIO_USBCMD_CSS_BMSK                                                                       0x100
#define HWIO_USBCMD_CSS_SHFT                                                                         0x8
#define HWIO_USBCMD_LHCRST_BMSK                                                                     0x80
#define HWIO_USBCMD_LHCRST_SHFT                                                                      0x7
#define HWIO_USBCMD_HSEE_BMSK                                                                        0x8
#define HWIO_USBCMD_HSEE_SHFT                                                                        0x3
#define HWIO_USBCMD_INTE_BMSK                                                                        0x4
#define HWIO_USBCMD_INTE_SHFT                                                                        0x2
#define HWIO_USBCMD_HCRST_BMSK                                                                       0x2
#define HWIO_USBCMD_HCRST_SHFT                                                                       0x1
#define HWIO_USBCMD_R_S_BMSK                                                                         0x1
#define HWIO_USBCMD_R_S_SHFT                                                                         0x0

#define HWIO_USBSTS_ADDR(x)                                                                   ((x) + 0x00000024)
#define HWIO_USBSTS_OFFS                                                                      (0x00000024)
#define HWIO_USBSTS_RMSK                                                                          0x1f1d
#define HWIO_USBSTS_IN(x)      \
        in_dword_masked(HWIO_USBSTS_ADDR(x), HWIO_USBSTS_RMSK)
#define HWIO_USBSTS_INM(x, m)      \
        in_dword_masked(HWIO_USBSTS_ADDR(x), m)
#define HWIO_USBSTS_OUT(x, v)      \
        out_dword(HWIO_USBSTS_ADDR(x),v)
#define HWIO_USBSTS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_USBSTS_ADDR(x),m,v,HWIO_USBSTS_IN(x))
#define HWIO_USBSTS_HCE_BMSK                                                                      0x1000
#define HWIO_USBSTS_HCE_SHFT                                                                         0xc
#define HWIO_USBSTS_CNR_BMSK                                                                       0x800
#define HWIO_USBSTS_CNR_SHFT                                                                         0xb
#define HWIO_USBSTS_SRE_BMSK                                                                       0x400
#define HWIO_USBSTS_SRE_SHFT                                                                         0xa
#define HWIO_USBSTS_RSS_BMSK                                                                       0x200
#define HWIO_USBSTS_RSS_SHFT                                                                         0x9
#define HWIO_USBSTS_SSS_BMSK                                                                       0x100
#define HWIO_USBSTS_SSS_SHFT                                                                         0x8
#define HWIO_USBSTS_PCD_BMSK                                                                        0x10
#define HWIO_USBSTS_PCD_SHFT                                                                         0x4
#define HWIO_USBSTS_EINT_BMSK                                                                        0x8
#define HWIO_USBSTS_EINT_SHFT                                                                        0x3
#define HWIO_USBSTS_HSE_BMSK                                                                         0x4
#define HWIO_USBSTS_HSE_SHFT                                                                         0x2
#define HWIO_USBSTS_HCH_BMSK                                                                         0x1
#define HWIO_USBSTS_HCH_SHFT                                                                         0x0

#define HWIO_PAGESIZE_ADDR(x)                                                                 ((x) + 0x00000028)
#define HWIO_PAGESIZE_OFFS                                                                    (0x00000028)
#define HWIO_PAGESIZE_RMSK                                                                        0xffff
#define HWIO_PAGESIZE_IN(x)      \
        in_dword_masked(HWIO_PAGESIZE_ADDR(x), HWIO_PAGESIZE_RMSK)
#define HWIO_PAGESIZE_INM(x, m)      \
        in_dword_masked(HWIO_PAGESIZE_ADDR(x), m)
#define HWIO_PAGESIZE_PAGE_SIZE_BMSK                                                              0xffff
#define HWIO_PAGESIZE_PAGE_SIZE_SHFT                                                                 0x0

#define HWIO_DNCTRL_ADDR(x)                                                                   ((x) + 0x00000034)
#define HWIO_DNCTRL_OFFS                                                                      (0x00000034)
#define HWIO_DNCTRL_RMSK                                                                          0xffff
#define HWIO_DNCTRL_IN(x)      \
        in_dword_masked(HWIO_DNCTRL_ADDR(x), HWIO_DNCTRL_RMSK)
#define HWIO_DNCTRL_INM(x, m)      \
        in_dword_masked(HWIO_DNCTRL_ADDR(x), m)
#define HWIO_DNCTRL_OUT(x, v)      \
        out_dword(HWIO_DNCTRL_ADDR(x),v)
#define HWIO_DNCTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DNCTRL_ADDR(x),m,v,HWIO_DNCTRL_IN(x))
#define HWIO_DNCTRL_N0_N15_BMSK                                                                   0xffff
#define HWIO_DNCTRL_N0_N15_SHFT                                                                      0x0

#define HWIO_CRCR_LO_ADDR(x)                                                                  ((x) + 0x00000038)
#define HWIO_CRCR_LO_OFFS                                                                     (0x00000038)
#define HWIO_CRCR_LO_RMSK                                                                     0xffffffcf
#define HWIO_CRCR_LO_IN(x)      \
        in_dword_masked(HWIO_CRCR_LO_ADDR(x), HWIO_CRCR_LO_RMSK)
#define HWIO_CRCR_LO_INM(x, m)      \
        in_dword_masked(HWIO_CRCR_LO_ADDR(x), m)
#define HWIO_CRCR_LO_OUT(x, v)      \
        out_dword(HWIO_CRCR_LO_ADDR(x),v)
#define HWIO_CRCR_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CRCR_LO_ADDR(x),m,v,HWIO_CRCR_LO_IN(x))
#define HWIO_CRCR_LO_CMD_RING_PNTR_BMSK                                                       0xffffffc0
#define HWIO_CRCR_LO_CMD_RING_PNTR_SHFT                                                              0x6
#define HWIO_CRCR_LO_CRR_BMSK                                                                        0x8
#define HWIO_CRCR_LO_CRR_SHFT                                                                        0x3
#define HWIO_CRCR_LO_CA_BMSK                                                                         0x4
#define HWIO_CRCR_LO_CA_SHFT                                                                         0x2
#define HWIO_CRCR_LO_CS_BMSK                                                                         0x2
#define HWIO_CRCR_LO_CS_SHFT                                                                         0x1
#define HWIO_CRCR_LO_RCS_BMSK                                                                        0x1
#define HWIO_CRCR_LO_RCS_SHFT                                                                        0x0

#define HWIO_CRCR_HI_ADDR(x)                                                                  ((x) + 0x0000003c)
#define HWIO_CRCR_HI_OFFS                                                                     (0x0000003c)
#define HWIO_CRCR_HI_RMSK                                                                     0xffffffff
#define HWIO_CRCR_HI_IN(x)      \
        in_dword_masked(HWIO_CRCR_HI_ADDR(x), HWIO_CRCR_HI_RMSK)
#define HWIO_CRCR_HI_INM(x, m)      \
        in_dword_masked(HWIO_CRCR_HI_ADDR(x), m)
#define HWIO_CRCR_HI_OUT(x, v)      \
        out_dword(HWIO_CRCR_HI_ADDR(x),v)
#define HWIO_CRCR_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CRCR_HI_ADDR(x),m,v,HWIO_CRCR_HI_IN(x))
#define HWIO_CRCR_HI_CMD_RING_PNTR_BMSK                                                       0xffffffff
#define HWIO_CRCR_HI_CMD_RING_PNTR_SHFT                                                              0x0

#define HWIO_DCBAAP_LO_ADDR(x)                                                                ((x) + 0x00000050)
#define HWIO_DCBAAP_LO_OFFS                                                                   (0x00000050)
#define HWIO_DCBAAP_LO_RMSK                                                                   0xffffffc0
#define HWIO_DCBAAP_LO_IN(x)      \
        in_dword_masked(HWIO_DCBAAP_LO_ADDR(x), HWIO_DCBAAP_LO_RMSK)
#define HWIO_DCBAAP_LO_INM(x, m)      \
        in_dword_masked(HWIO_DCBAAP_LO_ADDR(x), m)
#define HWIO_DCBAAP_LO_OUT(x, v)      \
        out_dword(HWIO_DCBAAP_LO_ADDR(x),v)
#define HWIO_DCBAAP_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCBAAP_LO_ADDR(x),m,v,HWIO_DCBAAP_LO_IN(x))
#define HWIO_DCBAAP_LO_DEVICE_CONTEXT_BAAP_BMSK                                               0xffffffc0
#define HWIO_DCBAAP_LO_DEVICE_CONTEXT_BAAP_SHFT                                                      0x6

#define HWIO_DCBAAP_HI_ADDR(x)                                                                ((x) + 0x00000054)
#define HWIO_DCBAAP_HI_OFFS                                                                   (0x00000054)
#define HWIO_DCBAAP_HI_RMSK                                                                   0xffffffff
#define HWIO_DCBAAP_HI_IN(x)      \
        in_dword_masked(HWIO_DCBAAP_HI_ADDR(x), HWIO_DCBAAP_HI_RMSK)
#define HWIO_DCBAAP_HI_INM(x, m)      \
        in_dword_masked(HWIO_DCBAAP_HI_ADDR(x), m)
#define HWIO_DCBAAP_HI_OUT(x, v)      \
        out_dword(HWIO_DCBAAP_HI_ADDR(x),v)
#define HWIO_DCBAAP_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCBAAP_HI_ADDR(x),m,v,HWIO_DCBAAP_HI_IN(x))
#define HWIO_DCBAAP_HI_DEVICE_CONTEXT_BAAP_BMSK                                               0xffffffff
#define HWIO_DCBAAP_HI_DEVICE_CONTEXT_BAAP_SHFT                                                      0x0

#define HWIO_CONFIG_ADDR(x)                                                                   ((x) + 0x00000058)
#define HWIO_CONFIG_OFFS                                                                      (0x00000058)
#define HWIO_CONFIG_RMSK                                                                            0xff
#define HWIO_CONFIG_IN(x)      \
        in_dword_masked(HWIO_CONFIG_ADDR(x), HWIO_CONFIG_RMSK)
#define HWIO_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_CONFIG_ADDR(x), m)
#define HWIO_CONFIG_OUT(x, v)      \
        out_dword(HWIO_CONFIG_ADDR(x),v)
#define HWIO_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CONFIG_ADDR(x),m,v,HWIO_CONFIG_IN(x))
#define HWIO_CONFIG_MAXSLOTSEN_BMSK                                                                 0xff
#define HWIO_CONFIG_MAXSLOTSEN_SHFT                                                                  0x0

#define HWIO_PORTSC_ADDR(base,p)                                                ((base) + 0x00000420 + 0x10 * (p))
#define HWIO_PORTSC_OFFS(base,p)                                                (0x00000420 + 0x10 * (p))
#define HWIO_PORTSC_RMSK                                                        0xcffffffb
#define HWIO_PORTSC_MAXp                                                                 1
#define HWIO_PORTSC_INI(base,p)        \
        in_dword_masked(HWIO_PORTSC_ADDR(base,p), HWIO_PORTSC_RMSK)
#define HWIO_PORTSC_INMI(base,p,mask)    \
        in_dword_masked(HWIO_PORTSC_ADDR(base,p), mask)
#define HWIO_PORTSC_OUTI(base,p,val)    \
        out_dword(HWIO_PORTSC_ADDR(base,p),val)
#define HWIO_PORTSC_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_PORTSC_ADDR(base,p),mask,val,HWIO_PORTSC_INI(base,p))
#define HWIO_PORTSC_WPR_BMSK                                                    0x80000000
#define HWIO_PORTSC_WPR_SHFT                                                          0x1f
#define HWIO_PORTSC_DR_BMSK                                                     0x40000000
#define HWIO_PORTSC_DR_SHFT                                                           0x1e
#define HWIO_PORTSC_WOE_BMSK                                                     0x8000000
#define HWIO_PORTSC_WOE_SHFT                                                          0x1b
#define HWIO_PORTSC_WDE_BMSK                                                     0x4000000
#define HWIO_PORTSC_WDE_SHFT                                                          0x1a
#define HWIO_PORTSC_WCE_BMSK                                                     0x2000000
#define HWIO_PORTSC_WCE_SHFT                                                          0x19
#define HWIO_PORTSC_CAS_BMSK                                                     0x1000000
#define HWIO_PORTSC_CAS_SHFT                                                          0x18
#define HWIO_PORTSC_CEC_BMSK                                                      0x800000
#define HWIO_PORTSC_CEC_SHFT                                                          0x17
#define HWIO_PORTSC_PLC_BMSK                                                      0x400000
#define HWIO_PORTSC_PLC_SHFT                                                          0x16
#define HWIO_PORTSC_PRC_BMSK                                                      0x200000
#define HWIO_PORTSC_PRC_SHFT                                                          0x15
#define HWIO_PORTSC_OCC_BMSK                                                      0x100000
#define HWIO_PORTSC_OCC_SHFT                                                          0x14
#define HWIO_PORTSC_WRC_BMSK                                                       0x80000
#define HWIO_PORTSC_WRC_SHFT                                                          0x13
#define HWIO_PORTSC_PEC_BMSK                                                       0x40000
#define HWIO_PORTSC_PEC_SHFT                                                          0x12
#define HWIO_PORTSC_CSC_BMSK                                                       0x20000
#define HWIO_PORTSC_CSC_SHFT                                                          0x11
#define HWIO_PORTSC_LWS_BMSK                                                       0x10000
#define HWIO_PORTSC_LWS_SHFT                                                          0x10
#define HWIO_PORTSC_PIC_BMSK                                                        0xc000
#define HWIO_PORTSC_PIC_SHFT                                                           0xe
#define HWIO_PORTSC_PORTSPEED_BMSK                                                  0x3c00
#define HWIO_PORTSC_PORTSPEED_SHFT                                                     0xa
#define HWIO_PORTSC_PP_BMSK                                                          0x200
#define HWIO_PORTSC_PP_SHFT                                                            0x9
#define HWIO_PORTSC_PLS_BMSK                                                         0x1e0
#define HWIO_PORTSC_PLS_SHFT                                                           0x5
#define HWIO_PORTSC_PR_BMSK                                                           0x10
#define HWIO_PORTSC_PR_SHFT                                                            0x4
#define HWIO_PORTSC_OCA_BMSK                                                           0x8
#define HWIO_PORTSC_OCA_SHFT                                                           0x3
#define HWIO_PORTSC_PED_BMSK                                                           0x2
#define HWIO_PORTSC_PED_SHFT                                                           0x1
#define HWIO_PORTSC_CCS_BMSK                                                           0x1
#define HWIO_PORTSC_CCS_SHFT                                                           0x0

#define HWIO_PORTPMSC_ADDR(base,p)                                              ((base) + 0x00000424 + 0x10 * (p))
#define HWIO_PORTPMSC_OFFS(base,p)                                              (0x00000424 + 0x10 * (p))
#define HWIO_PORTPMSC_RMSK                                                         0x1ffff
#define HWIO_PORTPMSC_MAXp                                                               1
#define HWIO_PORTPMSC_INI(base,p)        \
        in_dword_masked(HWIO_PORTPMSC_ADDR(base,p), HWIO_PORTPMSC_RMSK)
#define HWIO_PORTPMSC_INMI(base,p,mask)    \
        in_dword_masked(HWIO_PORTPMSC_ADDR(base,p), mask)
#define HWIO_PORTPMSC_OUTI(base,p,val)    \
        out_dword(HWIO_PORTPMSC_ADDR(base,p),val)
#define HWIO_PORTPMSC_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_PORTPMSC_ADDR(base,p),mask,val,HWIO_PORTPMSC_INI(base,p))
#define HWIO_PORTPMSC_FLA_BMSK                                                     0x10000
#define HWIO_PORTPMSC_FLA_SHFT                                                        0x10
#define HWIO_PORTPMSC_U2_TIMEOUT_BMSK                                               0xff00
#define HWIO_PORTPMSC_U2_TIMEOUT_SHFT                                                  0x8
#define HWIO_PORTPMSC_U1_TIMEOUT_BMSK                                                 0xff
#define HWIO_PORTPMSC_U1_TIMEOUT_SHFT                                                  0x0

#define HWIO_PORTLI_ADDR(base,p)                                                ((base) + 0x00000428 + 0x10 * (p))
#define HWIO_PORTLI_OFFS(base,p)                                                (0x00000428 + 0x10 * (p))
#define HWIO_PORTLI_RMSK                                                            0xffff
#define HWIO_PORTLI_MAXp                                                                 1
#define HWIO_PORTLI_INI(base,p)        \
        in_dword_masked(HWIO_PORTLI_ADDR(base,p), HWIO_PORTLI_RMSK)
#define HWIO_PORTLI_INMI(base,p,mask)    \
        in_dword_masked(HWIO_PORTLI_ADDR(base,p), mask)
#define HWIO_PORTLI_LINK_ERROR_COUNT_BMSK                                           0xffff
#define HWIO_PORTLI_LINK_ERROR_COUNT_SHFT                                              0x0

#define HWIO_PORTHLPMC_ADDR(base,p)                                             ((base) + 0x0000042c + 0x10 * (p))
#define HWIO_PORTHLPMC_OFFS(base,p)                                             (0x0000042c + 0x10 * (p))
#define HWIO_PORTHLPMC_RMSK                                                         0x3fff
#define HWIO_PORTHLPMC_MAXp                                                              1
#define HWIO_PORTHLPMC_INI(base,p)        \
        in_dword_masked(HWIO_PORTHLPMC_ADDR(base,p), HWIO_PORTHLPMC_RMSK)
#define HWIO_PORTHLPMC_INMI(base,p,mask)    \
        in_dword_masked(HWIO_PORTHLPMC_ADDR(base,p), mask)
#define HWIO_PORTHLPMC_OUTI(base,p,val)    \
        out_dword(HWIO_PORTHLPMC_ADDR(base,p),val)
#define HWIO_PORTHLPMC_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_PORTHLPMC_ADDR(base,p),mask,val,HWIO_PORTHLPMC_INI(base,p))
#define HWIO_PORTHLPMC_HIRDD_BMSK                                                   0x3c00
#define HWIO_PORTHLPMC_HIRDD_SHFT                                                      0xa
#define HWIO_PORTHLPMC_L1_TIMEOUT_BMSK                                               0x3fc
#define HWIO_PORTHLPMC_L1_TIMEOUT_SHFT                                                 0x2
#define HWIO_PORTHLPMC_HIRDM_BMSK                                                      0x3
#define HWIO_PORTHLPMC_HIRDM_SHFT                                                      0x0

#define HWIO_MFINDEX_ADDR(x)                                                                  ((x) + 0x00000440)
#define HWIO_MFINDEX_OFFS                                                                     (0x00000440)
#define HWIO_MFINDEX_RMSK                                                                         0x3fff
#define HWIO_MFINDEX_IN(x)      \
        in_dword_masked(HWIO_MFINDEX_ADDR(x), HWIO_MFINDEX_RMSK)
#define HWIO_MFINDEX_INM(x, m)      \
        in_dword_masked(HWIO_MFINDEX_ADDR(x), m)
#define HWIO_MFINDEX_MICROFRAME_INDEX_BMSK                                                        0x3fff
#define HWIO_MFINDEX_MICROFRAME_INDEX_SHFT                                                           0x0

#define HWIO_RSVDZ_ADDR(x)                                                                    ((x) + 0x00000444)
#define HWIO_RSVDZ_OFFS                                                                       (0x00000444)
#define HWIO_RSVDZ_RMSK                                                                       0xffffffff
#define HWIO_RSVDZ_IN(x)      \
        in_dword_masked(HWIO_RSVDZ_ADDR(x), HWIO_RSVDZ_RMSK)
#define HWIO_RSVDZ_INM(x, m)      \
        in_dword_masked(HWIO_RSVDZ_ADDR(x), m)
#define HWIO_RSVDZ_RSVD32_0_BMSK                                                              0xffffffff
#define HWIO_RSVDZ_RSVD32_0_SHFT                                                                     0x0

#define HWIO_IMAN_ADDR(base,p)                                                    ((base) + 0x00000460 + 0x20 * (p))
#define HWIO_IMAN_OFFS(base,p)                                                    (0x00000460 + 0x20 * (p))
#define HWIO_IMAN_RMSK                                                                   0x3
#define HWIO_IMAN_MAXp                                                                     1
#define HWIO_IMAN_INI(base,p)        \
        in_dword_masked(HWIO_IMAN_ADDR(base,p), HWIO_IMAN_RMSK)
#define HWIO_IMAN_INMI(base,p,mask)    \
        in_dword_masked(HWIO_IMAN_ADDR(base,p), mask)
#define HWIO_IMAN_OUTI(base,p,val)    \
        out_dword(HWIO_IMAN_ADDR(base,p),val)
#define HWIO_IMAN_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_IMAN_ADDR(base,p),mask,val,HWIO_IMAN_INI(base,p))
#define HWIO_IMAN_IE_BMSK                                                                0x2
#define HWIO_IMAN_IE_SHFT                                                                0x1
#define HWIO_IMAN_IP_BMSK                                                                0x1
#define HWIO_IMAN_IP_SHFT                                                                0x0

#define HWIO_IMOD_ADDR(base,p)                                                    ((base) + 0x00000464 + 0x20 * (p))
#define HWIO_IMOD_OFFS(base,p)                                                    (0x00000464 + 0x20 * (p))
#define HWIO_IMOD_RMSK                                                            0xffffffff
#define HWIO_IMOD_MAXp                                                                     1
#define HWIO_IMOD_INI(base,p)        \
        in_dword_masked(HWIO_IMOD_ADDR(base,p), HWIO_IMOD_RMSK)
#define HWIO_IMOD_INMI(base,p,mask)    \
        in_dword_masked(HWIO_IMOD_ADDR(base,p), mask)
#define HWIO_IMOD_OUTI(base,p,val)    \
        out_dword(HWIO_IMOD_ADDR(base,p),val)
#define HWIO_IMOD_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_IMOD_ADDR(base,p),mask,val,HWIO_IMOD_INI(base,p))
#define HWIO_IMOD_IMODC_BMSK                                                      0xffff0000
#define HWIO_IMOD_IMODC_SHFT                                                            0x10
#define HWIO_IMOD_IMODI_BMSK                                                          0xffff
#define HWIO_IMOD_IMODI_SHFT                                                             0x0

#define HWIO_ERSTSZ_ADDR(base,p)                                                  ((base) + 0x00000468 + 0x20 * (p))
#define HWIO_ERSTSZ_OFFS(base,p)                                                  (0x00000468 + 0x20 * (p))
#define HWIO_ERSTSZ_RMSK                                                              0xffff
#define HWIO_ERSTSZ_MAXp                                                                   1
#define HWIO_ERSTSZ_INI(base,p)        \
        in_dword_masked(HWIO_ERSTSZ_ADDR(base,p), HWIO_ERSTSZ_RMSK)
#define HWIO_ERSTSZ_INMI(base,p,mask)    \
        in_dword_masked(HWIO_ERSTSZ_ADDR(base,p), mask)
#define HWIO_ERSTSZ_OUTI(base,p,val)    \
        out_dword(HWIO_ERSTSZ_ADDR(base,p),val)
#define HWIO_ERSTSZ_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_ERSTSZ_ADDR(base,p),mask,val,HWIO_ERSTSZ_INI(base,p))
#define HWIO_ERSTSZ_ERS_TABLE_SIZE_BMSK                                               0xffff
#define HWIO_ERSTSZ_ERS_TABLE_SIZE_SHFT                                                  0x0

#define HWIO_RSVDP_ADDR(base,p)                                                   ((base) + 0x0000046c + 0x20 * (p))
#define HWIO_RSVDP_OFFS(base,p)                                                   (0x0000046c + 0x20 * (p))
#define HWIO_RSVDP_RMSK                                                           0xffffffff
#define HWIO_RSVDP_MAXp                                                                    1
#define HWIO_RSVDP_INI(base,p)        \
        in_dword_masked(HWIO_RSVDP_ADDR(base,p), HWIO_RSVDP_RMSK)
#define HWIO_RSVDP_INMI(base,p,mask)    \
        in_dword_masked(HWIO_RSVDP_ADDR(base,p), mask)
#define HWIO_RSVDP_RSVD32_0_BMSK                                                  0xffffffff
#define HWIO_RSVDP_RSVD32_0_SHFT                                                         0x0

#define HWIO_ERSTBA_LO_ADDR(base,p)                                               ((base) + 0x00000470 + 0x20 * (p))
#define HWIO_ERSTBA_LO_OFFS(base,p)                                               (0x00000470 + 0x20 * (p))
#define HWIO_ERSTBA_LO_RMSK                                                       0xffffffc0
#define HWIO_ERSTBA_LO_MAXp                                                                1
#define HWIO_ERSTBA_LO_INI(base,p)        \
        in_dword_masked(HWIO_ERSTBA_LO_ADDR(base,p), HWIO_ERSTBA_LO_RMSK)
#define HWIO_ERSTBA_LO_INMI(base,p,mask)    \
        in_dword_masked(HWIO_ERSTBA_LO_ADDR(base,p), mask)
#define HWIO_ERSTBA_LO_OUTI(base,p,val)    \
        out_dword(HWIO_ERSTBA_LO_ADDR(base,p),val)
#define HWIO_ERSTBA_LO_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_ERSTBA_LO_ADDR(base,p),mask,val,HWIO_ERSTBA_LO_INI(base,p))
#define HWIO_ERSTBA_LO_ERS_TABLE_BAR_BMSK                                         0xffffffc0
#define HWIO_ERSTBA_LO_ERS_TABLE_BAR_SHFT                                                0x6

#define HWIO_ERSTBA_HI_ADDR(base,p)                                               ((base) + 0x00000474 + 0x20 * (p))
#define HWIO_ERSTBA_HI_OFFS(base,p)                                               (0x00000474 + 0x20 * (p))
#define HWIO_ERSTBA_HI_RMSK                                                       0xffffffff
#define HWIO_ERSTBA_HI_MAXp                                                                1
#define HWIO_ERSTBA_HI_INI(base,p)        \
        in_dword_masked(HWIO_ERSTBA_HI_ADDR(base,p), HWIO_ERSTBA_HI_RMSK)
#define HWIO_ERSTBA_HI_INMI(base,p,mask)    \
        in_dword_masked(HWIO_ERSTBA_HI_ADDR(base,p), mask)
#define HWIO_ERSTBA_HI_OUTI(base,p,val)    \
        out_dword(HWIO_ERSTBA_HI_ADDR(base,p),val)
#define HWIO_ERSTBA_HI_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_ERSTBA_HI_ADDR(base,p),mask,val,HWIO_ERSTBA_HI_INI(base,p))
#define HWIO_ERSTBA_HI_ERS_TABLE_BAR_BMSK                                         0xffffffff
#define HWIO_ERSTBA_HI_ERS_TABLE_BAR_SHFT                                                0x0

#define HWIO_ERDP_LO_ADDR(base,p)                                                 ((base) + 0x00000478 + 0x20 * (p))
#define HWIO_ERDP_LO_OFFS(base,p)                                                 (0x00000478 + 0x20 * (p))
#define HWIO_ERDP_LO_RMSK                                                         0xffffffff
#define HWIO_ERDP_LO_MAXp                                                                  1
#define HWIO_ERDP_LO_INI(base,p)        \
        in_dword_masked(HWIO_ERDP_LO_ADDR(base,p), HWIO_ERDP_LO_RMSK)
#define HWIO_ERDP_LO_INMI(base,p,mask)    \
        in_dword_masked(HWIO_ERDP_LO_ADDR(base,p), mask)
#define HWIO_ERDP_LO_OUTI(base,p,val)    \
        out_dword(HWIO_ERDP_LO_ADDR(base,p),val)
#define HWIO_ERDP_LO_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_ERDP_LO_ADDR(base,p),mask,val,HWIO_ERDP_LO_INI(base,p))
#define HWIO_ERDP_LO_ERD_PNTR_BMSK                                                0xfffffff0
#define HWIO_ERDP_LO_ERD_PNTR_SHFT                                                       0x4
#define HWIO_ERDP_LO_EHB_BMSK                                                            0x8
#define HWIO_ERDP_LO_EHB_SHFT                                                            0x3
#define HWIO_ERDP_LO_DESI_BMSK                                                           0x7
#define HWIO_ERDP_LO_DESI_SHFT                                                           0x0

#define HWIO_ERDP_HI_ADDR(base,p)                                                 ((base) + 0x0000047c + 0x20 * (p))
#define HWIO_ERDP_HI_OFFS(base,p)                                                 (0x0000047c + 0x20 * (p))
#define HWIO_ERDP_HI_RMSK                                                         0xffffffff
#define HWIO_ERDP_HI_MAXp                                                                  1
#define HWIO_ERDP_HI_INI(base,p)        \
        in_dword_masked(HWIO_ERDP_HI_ADDR(base,p), HWIO_ERDP_HI_RMSK)
#define HWIO_ERDP_HI_INMI(base,p,mask)    \
        in_dword_masked(HWIO_ERDP_HI_ADDR(base,p), mask)
#define HWIO_ERDP_HI_OUTI(base,p,val)    \
        out_dword(HWIO_ERDP_HI_ADDR(base,p),val)
#define HWIO_ERDP_HI_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_ERDP_HI_ADDR(base,p),mask,val,HWIO_ERDP_HI_INI(base,p))
#define HWIO_ERDP_HI_ERD_PNTR_BMSK                                                0xffffffff
#define HWIO_ERDP_HI_ERD_PNTR_SHFT                                                       0x0

#define HWIO_DB_ADDR(base,p)                                                        ((base) + 0x000004a0 + 0x4 * (p))
#define HWIO_DB_OFFS(base,p)                                                        (0x000004a0 + 0x4 * (p))
#define HWIO_DB_RMSK                                                                0xffff00ff
#define HWIO_DB_MAXp                                                                        63
#define HWIO_DB_INI(base,p)        \
        in_dword_masked(HWIO_DB_ADDR(base,p), HWIO_DB_RMSK)
#define HWIO_DB_INMI(base,p,mask)    \
        in_dword_masked(HWIO_DB_ADDR(base,p), mask)
#define HWIO_DB_OUTI(base,p,val)    \
        out_dword(HWIO_DB_ADDR(base,p),val)
#define HWIO_DB_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_DB_ADDR(base,p),mask,val,HWIO_DB_INI(base,p))
#define HWIO_DB_DB_STREAM_ID_BMSK                                                   0xffff0000
#define HWIO_DB_DB_STREAM_ID_SHFT                                                         0x10
#define HWIO_DB_DB_TARGET_BMSK                                                            0xff
#define HWIO_DB_DB_TARGET_SHFT                                                             0x0

#define HWIO_USBLEGSUP_ADDR(x)                                                                ((x) + 0x000008a0)
#define HWIO_USBLEGSUP_OFFS                                                                   (0x000008a0)
#define HWIO_USBLEGSUP_RMSK                                                                    0x101ffff
#define HWIO_USBLEGSUP_IN(x)      \
        in_dword_masked(HWIO_USBLEGSUP_ADDR(x), HWIO_USBLEGSUP_RMSK)
#define HWIO_USBLEGSUP_INM(x, m)      \
        in_dword_masked(HWIO_USBLEGSUP_ADDR(x), m)
#define HWIO_USBLEGSUP_OUT(x, v)      \
        out_dword(HWIO_USBLEGSUP_ADDR(x),v)
#define HWIO_USBLEGSUP_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_USBLEGSUP_ADDR(x),m,v,HWIO_USBLEGSUP_IN(x))
#define HWIO_USBLEGSUP_HC_OS_OWNED_BMSK                                                        0x1000000
#define HWIO_USBLEGSUP_HC_OS_OWNED_SHFT                                                             0x18
#define HWIO_USBLEGSUP_HC_BIOS_OWNED_BMSK                                                        0x10000
#define HWIO_USBLEGSUP_HC_BIOS_OWNED_SHFT                                                           0x10
#define HWIO_USBLEGSUP_NEXT_CAPABILITY_POINTER_BMSK                                               0xff00
#define HWIO_USBLEGSUP_NEXT_CAPABILITY_POINTER_SHFT                                                  0x8
#define HWIO_USBLEGSUP_CAPABILITY_ID_BMSK                                                           0xff
#define HWIO_USBLEGSUP_CAPABILITY_ID_SHFT                                                            0x0

#define HWIO_USBLEGCTLSTS_ADDR(x)                                                             ((x) + 0x000008a4)
#define HWIO_USBLEGCTLSTS_OFFS                                                                (0x000008a4)
#define HWIO_USBLEGCTLSTS_RMSK                                                                0xe011e011
#define HWIO_USBLEGCTLSTS_IN(x)      \
        in_dword_masked(HWIO_USBLEGCTLSTS_ADDR(x), HWIO_USBLEGCTLSTS_RMSK)
#define HWIO_USBLEGCTLSTS_INM(x, m)      \
        in_dword_masked(HWIO_USBLEGCTLSTS_ADDR(x), m)
#define HWIO_USBLEGCTLSTS_OUT(x, v)      \
        out_dword(HWIO_USBLEGCTLSTS_ADDR(x),v)
#define HWIO_USBLEGCTLSTS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_USBLEGCTLSTS_ADDR(x),m,v,HWIO_USBLEGCTLSTS_IN(x))
#define HWIO_USBLEGCTLSTS_SMI_ON_BAR_BMSK                                                     0x80000000
#define HWIO_USBLEGCTLSTS_SMI_ON_BAR_SHFT                                                           0x1f
#define HWIO_USBLEGCTLSTS_SMI_ON_PCI_BMSK                                                     0x40000000
#define HWIO_USBLEGCTLSTS_SMI_ON_PCI_SHFT                                                           0x1e
#define HWIO_USBLEGCTLSTS_SMI_ON_OS_BMSK                                                      0x20000000
#define HWIO_USBLEGCTLSTS_SMI_ON_OS_SHFT                                                            0x1d
#define HWIO_USBLEGCTLSTS_SMI_ON_HOST_BMSK                                                      0x100000
#define HWIO_USBLEGCTLSTS_SMI_ON_HOST_SHFT                                                          0x14
#define HWIO_USBLEGCTLSTS_SMI_ON_EVENT_BMSK                                                      0x10000
#define HWIO_USBLEGCTLSTS_SMI_ON_EVENT_SHFT                                                         0x10
#define HWIO_USBLEGCTLSTS_SMI_ON_BAR_E_BMSK                                                       0x8000
#define HWIO_USBLEGCTLSTS_SMI_ON_BAR_E_SHFT                                                          0xf
#define HWIO_USBLEGCTLSTS_SMI_ON_PCI_E_BMSK                                                       0x4000
#define HWIO_USBLEGCTLSTS_SMI_ON_PCI_E_SHFT                                                          0xe
#define HWIO_USBLEGCTLSTS_SMI_ON_OS_E_BMSK                                                        0x2000
#define HWIO_USBLEGCTLSTS_SMI_ON_OS_E_SHFT                                                           0xd
#define HWIO_USBLEGCTLSTS_SMI_ON_HOST_E_BMSK                                                        0x10
#define HWIO_USBLEGCTLSTS_SMI_ON_HOST_E_SHFT                                                         0x4
#define HWIO_USBLEGCTLSTS_USB_SMI_ENABLE_BMSK                                                        0x1
#define HWIO_USBLEGCTLSTS_USB_SMI_ENABLE_SHFT                                                        0x0

#define HWIO_SUPTPRT2_DW0_ADDR(x)                                                             ((x) + 0x000008b0)
#define HWIO_SUPTPRT2_DW0_OFFS                                                                (0x000008b0)
#define HWIO_SUPTPRT2_DW0_RMSK                                                                0xffffffff
#define HWIO_SUPTPRT2_DW0_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT2_DW0_ADDR(x), HWIO_SUPTPRT2_DW0_RMSK)
#define HWIO_SUPTPRT2_DW0_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT2_DW0_ADDR(x), m)
#define HWIO_SUPTPRT2_DW0_MAJOR_REVISION_BMSK                                                 0xff000000
#define HWIO_SUPTPRT2_DW0_MAJOR_REVISION_SHFT                                                       0x18
#define HWIO_SUPTPRT2_DW0_MINOR_REVISION_BMSK                                                   0xff0000
#define HWIO_SUPTPRT2_DW0_MINOR_REVISION_SHFT                                                       0x10
#define HWIO_SUPTPRT2_DW0_NEXT_CAPABILITY_POINTER_BMSK                                            0xff00
#define HWIO_SUPTPRT2_DW0_NEXT_CAPABILITY_POINTER_SHFT                                               0x8
#define HWIO_SUPTPRT2_DW0_CAPABILITY_ID_BMSK                                                        0xff
#define HWIO_SUPTPRT2_DW0_CAPABILITY_ID_SHFT                                                         0x0

#define HWIO_SUPTPRT2_DW1_ADDR(x)                                                             ((x) + 0x000008b4)
#define HWIO_SUPTPRT2_DW1_OFFS                                                                (0x000008b4)
#define HWIO_SUPTPRT2_DW1_RMSK                                                                0xffffffff
#define HWIO_SUPTPRT2_DW1_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT2_DW1_ADDR(x), HWIO_SUPTPRT2_DW1_RMSK)
#define HWIO_SUPTPRT2_DW1_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT2_DW1_ADDR(x), m)
#define HWIO_SUPTPRT2_DW1_NAME_STRING_BMSK                                                    0xffffffff
#define HWIO_SUPTPRT2_DW1_NAME_STRING_SHFT                                                           0x0

#define HWIO_SUPTPRT2_DW2_ADDR(x)                                                             ((x) + 0x000008b8)
#define HWIO_SUPTPRT2_DW2_OFFS                                                                (0x000008b8)
#define HWIO_SUPTPRT2_DW2_RMSK                                                                0xf00effff
#define HWIO_SUPTPRT2_DW2_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT2_DW2_ADDR(x), HWIO_SUPTPRT2_DW2_RMSK)
#define HWIO_SUPTPRT2_DW2_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT2_DW2_ADDR(x), m)
#define HWIO_SUPTPRT2_DW2_PSIC_BMSK                                                           0xf0000000
#define HWIO_SUPTPRT2_DW2_PSIC_SHFT                                                                 0x1c
#define HWIO_SUPTPRT2_DW2_HLC_BMSK                                                               0x80000
#define HWIO_SUPTPRT2_DW2_HLC_SHFT                                                                  0x13
#define HWIO_SUPTPRT2_DW2_IHI_BMSK                                                               0x40000
#define HWIO_SUPTPRT2_DW2_IHI_SHFT                                                                  0x12
#define HWIO_SUPTPRT2_DW2_HSO_BMSK                                                               0x20000
#define HWIO_SUPTPRT2_DW2_HSO_SHFT                                                                  0x11
#define HWIO_SUPTPRT2_DW2_COMPATIBLE_PORT_COUNT_BMSK                                              0xff00
#define HWIO_SUPTPRT2_DW2_COMPATIBLE_PORT_COUNT_SHFT                                                 0x8
#define HWIO_SUPTPRT2_DW2_COMPATIBLE_PORT_OFFSET_BMSK                                               0xff
#define HWIO_SUPTPRT2_DW2_COMPATIBLE_PORT_OFFSET_SHFT                                                0x0

#define HWIO_SUPTPRT2_DW3_ADDR(x)                                                             ((x) + 0x000008bc)
#define HWIO_SUPTPRT2_DW3_OFFS                                                                (0x000008bc)
#define HWIO_SUPTPRT2_DW3_RMSK                                                                      0x1f
#define HWIO_SUPTPRT2_DW3_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT2_DW3_ADDR(x), HWIO_SUPTPRT2_DW3_RMSK)
#define HWIO_SUPTPRT2_DW3_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT2_DW3_ADDR(x), m)
#define HWIO_SUPTPRT2_DW3_PROTCL_SLT_TY_BMSK                                                        0x1f
#define HWIO_SUPTPRT2_DW3_PROTCL_SLT_TY_SHFT                                                         0x0

#define HWIO_SUPTPRT3_DW0_ADDR(x)                                                             ((x) + 0x000008c0)
#define HWIO_SUPTPRT3_DW0_OFFS                                                                (0x000008c0)
#define HWIO_SUPTPRT3_DW0_RMSK                                                                0xffffffff
#define HWIO_SUPTPRT3_DW0_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT3_DW0_ADDR(x), HWIO_SUPTPRT3_DW0_RMSK)
#define HWIO_SUPTPRT3_DW0_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT3_DW0_ADDR(x), m)
#define HWIO_SUPTPRT3_DW0_MAJOR_REVISION_BMSK                                                 0xff000000
#define HWIO_SUPTPRT3_DW0_MAJOR_REVISION_SHFT                                                       0x18
#define HWIO_SUPTPRT3_DW0_MINOR_REVISION_BMSK                                                   0xff0000
#define HWIO_SUPTPRT3_DW0_MINOR_REVISION_SHFT                                                       0x10
#define HWIO_SUPTPRT3_DW0_NEXT_CAPABILITY_POINTER_BMSK                                            0xff00
#define HWIO_SUPTPRT3_DW0_NEXT_CAPABILITY_POINTER_SHFT                                               0x8
#define HWIO_SUPTPRT3_DW0_CAPABILITY_ID_BMSK                                                        0xff
#define HWIO_SUPTPRT3_DW0_CAPABILITY_ID_SHFT                                                         0x0

#define HWIO_SUPTPRT3_DW1_ADDR(x)                                                             ((x) + 0x000008c4)
#define HWIO_SUPTPRT3_DW1_OFFS                                                                (0x000008c4)
#define HWIO_SUPTPRT3_DW1_RMSK                                                                0xffffffff
#define HWIO_SUPTPRT3_DW1_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT3_DW1_ADDR(x), HWIO_SUPTPRT3_DW1_RMSK)
#define HWIO_SUPTPRT3_DW1_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT3_DW1_ADDR(x), m)
#define HWIO_SUPTPRT3_DW1_NAME_STRING_BMSK                                                    0xffffffff
#define HWIO_SUPTPRT3_DW1_NAME_STRING_SHFT                                                           0x0

#define HWIO_SUPTPRT3_DW2_ADDR(x)                                                             ((x) + 0x000008c8)
#define HWIO_SUPTPRT3_DW2_OFFS                                                                (0x000008c8)
#define HWIO_SUPTPRT3_DW2_RMSK                                                                0xf000ffff
#define HWIO_SUPTPRT3_DW2_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT3_DW2_ADDR(x), HWIO_SUPTPRT3_DW2_RMSK)
#define HWIO_SUPTPRT3_DW2_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT3_DW2_ADDR(x), m)
#define HWIO_SUPTPRT3_DW2_PSIC_BMSK                                                           0xf0000000
#define HWIO_SUPTPRT3_DW2_PSIC_SHFT                                                                 0x1c
#define HWIO_SUPTPRT3_DW2_COMPATIBLE_PORT_COUNT_BMSK                                              0xff00
#define HWIO_SUPTPRT3_DW2_COMPATIBLE_PORT_COUNT_SHFT                                                 0x8
#define HWIO_SUPTPRT3_DW2_COMPATIBLE_PORT_OFFSET_BMSK                                               0xff
#define HWIO_SUPTPRT3_DW2_COMPATIBLE_PORT_OFFSET_SHFT                                                0x0

#define HWIO_SUPTPRT3_DW3_ADDR(x)                                                             ((x) + 0x000008cc)
#define HWIO_SUPTPRT3_DW3_OFFS                                                                (0x000008cc)
#define HWIO_SUPTPRT3_DW3_RMSK                                                                      0x1f
#define HWIO_SUPTPRT3_DW3_IN(x)      \
        in_dword_masked(HWIO_SUPTPRT3_DW3_ADDR(x), HWIO_SUPTPRT3_DW3_RMSK)
#define HWIO_SUPTPRT3_DW3_INM(x, m)      \
        in_dword_masked(HWIO_SUPTPRT3_DW3_ADDR(x), m)
#define HWIO_SUPTPRT3_DW3_PROTCL_SLT_TY_BMSK                                                        0x1f
#define HWIO_SUPTPRT3_DW3_PROTCL_SLT_TY_SHFT                                                         0x0

#define HWIO_DCID_ADDR(x)                                                                     ((x) + 0x000008d0)
#define HWIO_DCID_OFFS                                                                        (0x000008d0)
#define HWIO_DCID_RMSK                                                                          0x1fffff
#define HWIO_DCID_IN(x)      \
        in_dword_masked(HWIO_DCID_ADDR(x), HWIO_DCID_RMSK)
#define HWIO_DCID_INM(x, m)      \
        in_dword_masked(HWIO_DCID_ADDR(x), m)
#define HWIO_DCID_DCERSTMAX_BMSK                                                                0x1f0000
#define HWIO_DCID_DCERSTMAX_SHFT                                                                    0x10
#define HWIO_DCID_NEXT_CAPABILITY_POINTER_BMSK                                                    0xff00
#define HWIO_DCID_NEXT_CAPABILITY_POINTER_SHFT                                                       0x8
#define HWIO_DCID_CAPABILITY_ID_BMSK                                                                0xff
#define HWIO_DCID_CAPABILITY_ID_SHFT                                                                 0x0

#define HWIO_DCDB_ADDR(x)                                                                     ((x) + 0x000008d4)
#define HWIO_DCDB_OFFS                                                                        (0x000008d4)
#define HWIO_DCDB_RMSK                                                                            0xff00
#define HWIO_DCDB_IN(x)      \
        in_dword_masked(HWIO_DCDB_ADDR(x), HWIO_DCDB_RMSK)
#define HWIO_DCDB_INM(x, m)      \
        in_dword_masked(HWIO_DCDB_ADDR(x), m)
#define HWIO_DCDB_OUT(x, v)      \
        out_dword(HWIO_DCDB_ADDR(x),v)
#define HWIO_DCDB_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCDB_ADDR(x),m,v,HWIO_DCDB_IN(x))
#define HWIO_DCDB_DBTARGET_BMSK                                                                   0xff00
#define HWIO_DCDB_DBTARGET_SHFT                                                                      0x8

#define HWIO_DCERSTSZ_ADDR(x)                                                                 ((x) + 0x000008d8)
#define HWIO_DCERSTSZ_OFFS                                                                    (0x000008d8)
#define HWIO_DCERSTSZ_RMSK                                                                        0xffff
#define HWIO_DCERSTSZ_IN(x)      \
        in_dword_masked(HWIO_DCERSTSZ_ADDR(x), HWIO_DCERSTSZ_RMSK)
#define HWIO_DCERSTSZ_INM(x, m)      \
        in_dword_masked(HWIO_DCERSTSZ_ADDR(x), m)
#define HWIO_DCERSTSZ_OUT(x, v)      \
        out_dword(HWIO_DCERSTSZ_ADDR(x),v)
#define HWIO_DCERSTSZ_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCERSTSZ_ADDR(x),m,v,HWIO_DCERSTSZ_IN(x))
#define HWIO_DCERSTSZ_ERS_TABLE_SIZE_BMSK                                                         0xffff
#define HWIO_DCERSTSZ_ERS_TABLE_SIZE_SHFT                                                            0x0

#define HWIO_DCERSTBA_LO_ADDR(x)                                                              ((x) + 0x000008e0)
#define HWIO_DCERSTBA_LO_OFFS                                                                 (0x000008e0)
#define HWIO_DCERSTBA_LO_RMSK                                                                 0xfffffff0
#define HWIO_DCERSTBA_LO_IN(x)      \
        in_dword_masked(HWIO_DCERSTBA_LO_ADDR(x), HWIO_DCERSTBA_LO_RMSK)
#define HWIO_DCERSTBA_LO_INM(x, m)      \
        in_dword_masked(HWIO_DCERSTBA_LO_ADDR(x), m)
#define HWIO_DCERSTBA_LO_OUT(x, v)      \
        out_dword(HWIO_DCERSTBA_LO_ADDR(x),v)
#define HWIO_DCERSTBA_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCERSTBA_LO_ADDR(x),m,v,HWIO_DCERSTBA_LO_IN(x))
#define HWIO_DCERSTBA_LO_ERS_TABLE_BAR_BMSK                                                   0xfffffff0
#define HWIO_DCERSTBA_LO_ERS_TABLE_BAR_SHFT                                                          0x4

#define HWIO_DCERSTBA_HI_ADDR(x)                                                              ((x) + 0x000008e4)
#define HWIO_DCERSTBA_HI_OFFS                                                                 (0x000008e4)
#define HWIO_DCERSTBA_HI_RMSK                                                                 0xffffffff
#define HWIO_DCERSTBA_HI_IN(x)      \
        in_dword_masked(HWIO_DCERSTBA_HI_ADDR(x), HWIO_DCERSTBA_HI_RMSK)
#define HWIO_DCERSTBA_HI_INM(x, m)      \
        in_dword_masked(HWIO_DCERSTBA_HI_ADDR(x), m)
#define HWIO_DCERSTBA_HI_OUT(x, v)      \
        out_dword(HWIO_DCERSTBA_HI_ADDR(x),v)
#define HWIO_DCERSTBA_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCERSTBA_HI_ADDR(x),m,v,HWIO_DCERSTBA_HI_IN(x))
#define HWIO_DCERSTBA_HI_ERS_TABLE_BAR_BMSK                                                   0xffffffff
#define HWIO_DCERSTBA_HI_ERS_TABLE_BAR_SHFT                                                          0x0

#define HWIO_DCERDP_LO_ADDR(x)                                                                ((x) + 0x000008e8)
#define HWIO_DCERDP_LO_OFFS                                                                   (0x000008e8)
#define HWIO_DCERDP_LO_RMSK                                                                   0xfffffff7
#define HWIO_DCERDP_LO_IN(x)      \
        in_dword_masked(HWIO_DCERDP_LO_ADDR(x), HWIO_DCERDP_LO_RMSK)
#define HWIO_DCERDP_LO_INM(x, m)      \
        in_dword_masked(HWIO_DCERDP_LO_ADDR(x), m)
#define HWIO_DCERDP_LO_OUT(x, v)      \
        out_dword(HWIO_DCERDP_LO_ADDR(x),v)
#define HWIO_DCERDP_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCERDP_LO_ADDR(x),m,v,HWIO_DCERDP_LO_IN(x))
#define HWIO_DCERDP_LO_DEQUEUE_POINTER_BMSK                                                   0xfffffff0
#define HWIO_DCERDP_LO_DEQUEUE_POINTER_SHFT                                                          0x4
#define HWIO_DCERDP_LO_DESI_BMSK                                                                     0x7
#define HWIO_DCERDP_LO_DESI_SHFT                                                                     0x0

#define HWIO_DCERDP_HI_ADDR(x)                                                                ((x) + 0x000008ec)
#define HWIO_DCERDP_HI_OFFS                                                                   (0x000008ec)
#define HWIO_DCERDP_HI_RMSK                                                                   0xffffffff
#define HWIO_DCERDP_HI_IN(x)      \
        in_dword_masked(HWIO_DCERDP_HI_ADDR(x), HWIO_DCERDP_HI_RMSK)
#define HWIO_DCERDP_HI_INM(x, m)      \
        in_dword_masked(HWIO_DCERDP_HI_ADDR(x), m)
#define HWIO_DCERDP_HI_OUT(x, v)      \
        out_dword(HWIO_DCERDP_HI_ADDR(x),v)
#define HWIO_DCERDP_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCERDP_HI_ADDR(x),m,v,HWIO_DCERDP_HI_IN(x))
#define HWIO_DCERDP_HI_DEQUEUE_POINTER_BMSK                                                   0xffffffff
#define HWIO_DCERDP_HI_DEQUEUE_POINTER_SHFT                                                          0x0

#define HWIO_DCCTRL_ADDR(x)                                                                   ((x) + 0x000008f0)
#define HWIO_DCCTRL_OFFS                                                                      (0x000008f0)
#define HWIO_DCCTRL_RMSK                                                                      0xffff001f
#define HWIO_DCCTRL_IN(x)      \
        in_dword_masked(HWIO_DCCTRL_ADDR(x), HWIO_DCCTRL_RMSK)
#define HWIO_DCCTRL_INM(x, m)      \
        in_dword_masked(HWIO_DCCTRL_ADDR(x), m)
#define HWIO_DCCTRL_OUT(x, v)      \
        out_dword(HWIO_DCCTRL_ADDR(x),v)
#define HWIO_DCCTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCCTRL_ADDR(x),m,v,HWIO_DCCTRL_IN(x))
#define HWIO_DCCTRL_DCE_BMSK                                                                  0x80000000
#define HWIO_DCCTRL_DCE_SHFT                                                                        0x1f
#define HWIO_DCCTRL_DEVICE_ADDRESS_BMSK                                                       0x7f000000
#define HWIO_DCCTRL_DEVICE_ADDRESS_SHFT                                                             0x18
#define HWIO_DCCTRL_DEBUG_MAX_BURST_SIZE_BMSK                                                   0xff0000
#define HWIO_DCCTRL_DEBUG_MAX_BURST_SIZE_SHFT                                                       0x10
#define HWIO_DCCTRL_DRC_BMSK                                                                        0x10
#define HWIO_DCCTRL_DRC_SHFT                                                                         0x4
#define HWIO_DCCTRL_HIT_BMSK                                                                         0x8
#define HWIO_DCCTRL_HIT_SHFT                                                                         0x3
#define HWIO_DCCTRL_HOT_BMSK                                                                         0x4
#define HWIO_DCCTRL_HOT_SHFT                                                                         0x2
#define HWIO_DCCTRL_LSE_BMSK                                                                         0x2
#define HWIO_DCCTRL_LSE_SHFT                                                                         0x1
#define HWIO_DCCTRL_DCR_BMSK                                                                         0x1
#define HWIO_DCCTRL_DCR_SHFT                                                                         0x0

#define HWIO_DCST_ADDR(x)                                                                     ((x) + 0x000008f4)
#define HWIO_DCST_OFFS                                                                        (0x000008f4)
#define HWIO_DCST_RMSK                                                                        0xff000001
#define HWIO_DCST_IN(x)      \
        in_dword_masked(HWIO_DCST_ADDR(x), HWIO_DCST_RMSK)
#define HWIO_DCST_INM(x, m)      \
        in_dword_masked(HWIO_DCST_ADDR(x), m)
#define HWIO_DCST_DEBUG_PORT_NUMBER_BMSK                                                      0xff000000
#define HWIO_DCST_DEBUG_PORT_NUMBER_SHFT                                                            0x18
#define HWIO_DCST_ER_BMSK                                                                            0x1
#define HWIO_DCST_ER_SHFT                                                                            0x0

#define HWIO_DCPORTSC_ADDR(x)                                                                 ((x) + 0x000008f8)
#define HWIO_DCPORTSC_OFFS                                                                    (0x000008f8)
#define HWIO_DCPORTSC_RMSK                                                                      0xe23df3
#define HWIO_DCPORTSC_IN(x)      \
        in_dword_masked(HWIO_DCPORTSC_ADDR(x), HWIO_DCPORTSC_RMSK)
#define HWIO_DCPORTSC_INM(x, m)      \
        in_dword_masked(HWIO_DCPORTSC_ADDR(x), m)
#define HWIO_DCPORTSC_OUT(x, v)      \
        out_dword(HWIO_DCPORTSC_ADDR(x),v)
#define HWIO_DCPORTSC_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCPORTSC_ADDR(x),m,v,HWIO_DCPORTSC_IN(x))
#define HWIO_DCPORTSC_CEC_BMSK                                                                  0x800000
#define HWIO_DCPORTSC_CEC_SHFT                                                                      0x17
#define HWIO_DCPORTSC_PLC_BMSK                                                                  0x400000
#define HWIO_DCPORTSC_PLC_SHFT                                                                      0x16
#define HWIO_DCPORTSC_PRC_BMSK                                                                  0x200000
#define HWIO_DCPORTSC_PRC_SHFT                                                                      0x15
#define HWIO_DCPORTSC_CSC_BMSK                                                                   0x20000
#define HWIO_DCPORTSC_CSC_SHFT                                                                      0x11
#define HWIO_DCPORTSC_PORTSPEED_BMSK                                                              0x3c00
#define HWIO_DCPORTSC_PORTSPEED_SHFT                                                                 0xa
#define HWIO_DCPORTSC_PLS_BMSK                                                                     0x1e0
#define HWIO_DCPORTSC_PLS_SHFT                                                                       0x5
#define HWIO_DCPORTSC_PR_BMSK                                                                       0x10
#define HWIO_DCPORTSC_PR_SHFT                                                                        0x4
#define HWIO_DCPORTSC_PED_BMSK                                                                       0x2
#define HWIO_DCPORTSC_PED_SHFT                                                                       0x1
#define HWIO_DCPORTSC_CCS_BMSK                                                                       0x1
#define HWIO_DCPORTSC_CCS_SHFT                                                                       0x0

#define HWIO_DCCP_LO_ADDR(x)                                                                  ((x) + 0x00000900)
#define HWIO_DCCP_LO_OFFS                                                                     (0x00000900)
#define HWIO_DCCP_LO_RMSK                                                                     0xfffffff0
#define HWIO_DCCP_LO_IN(x)      \
        in_dword_masked(HWIO_DCCP_LO_ADDR(x), HWIO_DCCP_LO_RMSK)
#define HWIO_DCCP_LO_INM(x, m)      \
        in_dword_masked(HWIO_DCCP_LO_ADDR(x), m)
#define HWIO_DCCP_LO_OUT(x, v)      \
        out_dword(HWIO_DCCP_LO_ADDR(x),v)
#define HWIO_DCCP_LO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCCP_LO_ADDR(x),m,v,HWIO_DCCP_LO_IN(x))
#define HWIO_DCCP_LO_DCCPR_BMSK                                                               0xfffffff0
#define HWIO_DCCP_LO_DCCPR_SHFT                                                                      0x4

#define HWIO_DCCP_HI_ADDR(x)                                                                  ((x) + 0x00000904)
#define HWIO_DCCP_HI_OFFS                                                                     (0x00000904)
#define HWIO_DCCP_HI_RMSK                                                                     0xffffffff
#define HWIO_DCCP_HI_IN(x)      \
        in_dword_masked(HWIO_DCCP_HI_ADDR(x), HWIO_DCCP_HI_RMSK)
#define HWIO_DCCP_HI_INM(x, m)      \
        in_dword_masked(HWIO_DCCP_HI_ADDR(x), m)
#define HWIO_DCCP_HI_OUT(x, v)      \
        out_dword(HWIO_DCCP_HI_ADDR(x),v)
#define HWIO_DCCP_HI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCCP_HI_ADDR(x),m,v,HWIO_DCCP_HI_IN(x))
#define HWIO_DCCP_HI_DCCPR_BMSK                                                               0xffffffff
#define HWIO_DCCP_HI_DCCPR_SHFT                                                                      0x0

#define HWIO_DCDDI1_ADDR(x)                                                                   ((x) + 0x00000908)
#define HWIO_DCDDI1_OFFS                                                                      (0x00000908)
#define HWIO_DCDDI1_RMSK                                                                      0xffff00ff
#define HWIO_DCDDI1_IN(x)      \
        in_dword_masked(HWIO_DCDDI1_ADDR(x), HWIO_DCDDI1_RMSK)
#define HWIO_DCDDI1_INM(x, m)      \
        in_dword_masked(HWIO_DCDDI1_ADDR(x), m)
#define HWIO_DCDDI1_OUT(x, v)      \
        out_dword(HWIO_DCDDI1_ADDR(x),v)
#define HWIO_DCDDI1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCDDI1_ADDR(x),m,v,HWIO_DCDDI1_IN(x))
#define HWIO_DCDDI1_VENDORID_BMSK                                                             0xffff0000
#define HWIO_DCDDI1_VENDORID_SHFT                                                                   0x10
#define HWIO_DCDDI1_DBCPROTOCOL_BMSK                                                                0xff
#define HWIO_DCDDI1_DBCPROTOCOL_SHFT                                                                 0x0

#define HWIO_DCDDI2_ADDR(x)                                                                   ((x) + 0x0000090c)
#define HWIO_DCDDI2_OFFS                                                                      (0x0000090c)
#define HWIO_DCDDI2_RMSK                                                                      0xffffffff
#define HWIO_DCDDI2_IN(x)      \
        in_dword_masked(HWIO_DCDDI2_ADDR(x), HWIO_DCDDI2_RMSK)
#define HWIO_DCDDI2_INM(x, m)      \
        in_dword_masked(HWIO_DCDDI2_ADDR(x), m)
#define HWIO_DCDDI2_OUT(x, v)      \
        out_dword(HWIO_DCDDI2_ADDR(x),v)
#define HWIO_DCDDI2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DCDDI2_ADDR(x),m,v,HWIO_DCDDI2_IN(x))
#define HWIO_DCDDI2_DEVICEREV_BMSK                                                            0xffff0000
#define HWIO_DCDDI2_DEVICEREV_SHFT                                                                  0x10
#define HWIO_DCDDI2_PRODUCTID_BMSK                                                                0xffff
#define HWIO_DCDDI2_PRODUCTID_SHFT                                                                   0x0

/*----------------------------------------------------------------------------
 * MODULE: USB30_QSCRATCH
 *--------------------------------------------------------------------------*/

#define USB30_QSCRATCH_REG_BASE                                             (USB30_PRIM_BASE      + 0x000f8800)
#define USB30_QSCRATCH_REG_BASE_OFFS                                        0x000f8800

#define HWIO_IPCAT_REG_ADDR(x)                                              ((x) + 0x00000000)
#define HWIO_IPCAT_REG_OFFS                                                 (0x00000000)
#define HWIO_IPCAT_REG_RMSK                                                 0xffffffff
#define HWIO_IPCAT_REG_IN(x)      \
        in_dword_masked(HWIO_IPCAT_REG_ADDR(x), HWIO_IPCAT_REG_RMSK)
#define HWIO_IPCAT_REG_INM(x, m)      \
        in_dword_masked(HWIO_IPCAT_REG_ADDR(x), m)
#define HWIO_IPCAT_REG_MAJOR_BMSK                                           0xf0000000
#define HWIO_IPCAT_REG_MAJOR_SHFT                                                 0x1c
#define HWIO_IPCAT_REG_MINOR_BMSK                                            0xfff0000
#define HWIO_IPCAT_REG_MINOR_SHFT                                                 0x10
#define HWIO_IPCAT_REG_STEP_BMSK                                                0xffff
#define HWIO_IPCAT_REG_STEP_SHFT                                                   0x0

#define HWIO_CTRL_REG_ADDR(x)                                               ((x) + 0x00000004)
#define HWIO_CTRL_REG_OFFS                                                  (0x00000004)
#define HWIO_CTRL_REG_RMSK                                                  0xffdff7ff
#define HWIO_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_CTRL_REG_ADDR(x), HWIO_CTRL_REG_RMSK)
#define HWIO_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_CTRL_REG_ADDR(x), m)
#define HWIO_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_CTRL_REG_ADDR(x),v)
#define HWIO_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CTRL_REG_ADDR(x),m,v,HWIO_CTRL_REG_IN(x))
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SUSPEND_BMSK                            0x80000000
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SUSPEND_SHFT                                  0x1f
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SLEEP_BMSK                              0x40000000
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SLEEP_SHFT                                    0x1e
#define HWIO_CTRL_REG_BC_XCVR_SELECT_3_BMSK                                 0x30000000
#define HWIO_CTRL_REG_BC_XCVR_SELECT_3_SHFT                                       0x1c
#define HWIO_CTRL_REG_BC_TERM_SELECT_3_BMSK                                  0x8000000
#define HWIO_CTRL_REG_BC_TERM_SELECT_3_SHFT                                       0x1b
#define HWIO_CTRL_REG_BC_TX_VALID_3_BMSK                                     0x4000000
#define HWIO_CTRL_REG_BC_TX_VALID_3_SHFT                                          0x1a
#define HWIO_CTRL_REG_BC_OPMODE_3_BMSK                                       0x3000000
#define HWIO_CTRL_REG_BC_OPMODE_3_SHFT                                            0x18
#define HWIO_CTRL_REG_BC_DMPULLDOWN_3_BMSK                                    0x800000
#define HWIO_CTRL_REG_BC_DMPULLDOWN_3_SHFT                                        0x17
#define HWIO_CTRL_REG_BC_DPPULLDOWN_3_BMSK                                    0x400000
#define HWIO_CTRL_REG_BC_DPPULLDOWN_3_SHFT                                        0x16
#define HWIO_CTRL_REG_BC_SEL_3_BMSK                                           0x100000
#define HWIO_CTRL_REG_BC_SEL_3_SHFT                                               0x14
#define HWIO_CTRL_REG_BC_XCVR_SELECT_2_BMSK                                    0xc0000
#define HWIO_CTRL_REG_BC_XCVR_SELECT_2_SHFT                                       0x12
#define HWIO_CTRL_REG_BC_TERM_SELECT_2_BMSK                                    0x20000
#define HWIO_CTRL_REG_BC_TERM_SELECT_2_SHFT                                       0x11
#define HWIO_CTRL_REG_BC_TX_VALID_2_BMSK                                       0x10000
#define HWIO_CTRL_REG_BC_TX_VALID_2_SHFT                                          0x10
#define HWIO_CTRL_REG_BC_OPMODE_2_BMSK                                          0xc000
#define HWIO_CTRL_REG_BC_OPMODE_2_SHFT                                             0xe
#define HWIO_CTRL_REG_BC_DMPULLDOWN_2_BMSK                                      0x2000
#define HWIO_CTRL_REG_BC_DMPULLDOWN_2_SHFT                                         0xd
#define HWIO_CTRL_REG_BC_DPPULLDOWN_2_BMSK                                      0x1000
#define HWIO_CTRL_REG_BC_DPPULLDOWN_2_SHFT                                         0xc
#define HWIO_CTRL_REG_BC_SEL_2_BMSK                                              0x400
#define HWIO_CTRL_REG_BC_SEL_2_SHFT                                                0xa
#define HWIO_CTRL_REG_BC_XCVR_SELECT_BMSK                                        0x300
#define HWIO_CTRL_REG_BC_XCVR_SELECT_SHFT                                          0x8
#define HWIO_CTRL_REG_BC_TERM_SELECT_BMSK                                         0x80
#define HWIO_CTRL_REG_BC_TERM_SELECT_SHFT                                          0x7
#define HWIO_CTRL_REG_BC_TX_VALID_BMSK                                            0x40
#define HWIO_CTRL_REG_BC_TX_VALID_SHFT                                             0x6
#define HWIO_CTRL_REG_BC_OPMODE_BMSK                                              0x30
#define HWIO_CTRL_REG_BC_OPMODE_SHFT                                               0x4
#define HWIO_CTRL_REG_BC_DMPULLDOWN_BMSK                                           0x8
#define HWIO_CTRL_REG_BC_DMPULLDOWN_SHFT                                           0x3
#define HWIO_CTRL_REG_BC_DPPULLDOWN_BMSK                                           0x4
#define HWIO_CTRL_REG_BC_DPPULLDOWN_SHFT                                           0x2
#define HWIO_CTRL_REG_BC_IDPULLUP_BMSK                                             0x2
#define HWIO_CTRL_REG_BC_IDPULLUP_SHFT                                             0x1
#define HWIO_CTRL_REG_BC_SEL_BMSK                                                  0x1
#define HWIO_CTRL_REG_BC_SEL_SHFT                                                  0x0

#define HWIO_GENERAL_CFG_ADDR(x)                                            ((x) + 0x00000008)
#define HWIO_GENERAL_CFG_OFFS                                               (0x00000008)
#define HWIO_GENERAL_CFG_RMSK                                                     0xf3
#define HWIO_GENERAL_CFG_IN(x)      \
        in_dword_masked(HWIO_GENERAL_CFG_ADDR(x), HWIO_GENERAL_CFG_RMSK)
#define HWIO_GENERAL_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENERAL_CFG_ADDR(x), m)
#define HWIO_GENERAL_CFG_OUT(x, v)      \
        out_dword(HWIO_GENERAL_CFG_ADDR(x),v)
#define HWIO_GENERAL_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENERAL_CFG_ADDR(x),m,v,HWIO_GENERAL_CFG_IN(x))
#define HWIO_GENERAL_CFG_SEC_UTMI_FREE_CLK_GFM_SEL1_BMSK                          0x80
#define HWIO_GENERAL_CFG_SEC_UTMI_FREE_CLK_GFM_SEL1_SHFT                           0x7
#define HWIO_GENERAL_CFG_IDDIG_BMSK                                               0x40
#define HWIO_GENERAL_CFG_IDDIG_SHFT                                                0x6
#define HWIO_GENERAL_CFG_IDDIG_SW_SEL_BMSK                                        0x20
#define HWIO_GENERAL_CFG_IDDIG_SW_SEL_SHFT                                         0x5
#define HWIO_GENERAL_CFG_IDDIG_SW_BMSK                                            0x10
#define HWIO_GENERAL_CFG_IDDIG_SW_SHFT                                             0x4
#define HWIO_GENERAL_CFG_DBM_EN_BMSK                                               0x2
#define HWIO_GENERAL_CFG_DBM_EN_SHFT                                               0x1
#define HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK                                    0x1
#define HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_SHFT                                    0x0

#define HWIO_RAM1_REG_ADDR(x)                                               ((x) + 0x0000000c)
#define HWIO_RAM1_REG_OFFS                                                  (0x0000000c)
#define HWIO_RAM1_REG_RMSK                                                         0x7
#define HWIO_RAM1_REG_IN(x)      \
        in_dword_masked(HWIO_RAM1_REG_ADDR(x), HWIO_RAM1_REG_RMSK)
#define HWIO_RAM1_REG_INM(x, m)      \
        in_dword_masked(HWIO_RAM1_REG_ADDR(x), m)
#define HWIO_RAM1_REG_OUT(x, v)      \
        out_dword(HWIO_RAM1_REG_ADDR(x),v)
#define HWIO_RAM1_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RAM1_REG_ADDR(x),m,v,HWIO_RAM1_REG_IN(x))
#define HWIO_RAM1_REG_RAM13_EN_BMSK                                                0x4
#define HWIO_RAM1_REG_RAM13_EN_SHFT                                                0x2
#define HWIO_RAM1_REG_RAM12_EN_BMSK                                                0x2
#define HWIO_RAM1_REG_RAM12_EN_SHFT                                                0x1
#define HWIO_RAM1_REG_RAM11_EN_BMSK                                                0x1
#define HWIO_RAM1_REG_RAM11_EN_SHFT                                                0x0

#define HWIO_HS_PHY_CTRL_ADDR(x)                                            ((x) + 0x00000010)
#define HWIO_HS_PHY_CTRL_OFFS                                               (0x00000010)
#define HWIO_HS_PHY_CTRL_RMSK                                               0x3cf82601
#define HWIO_HS_PHY_CTRL_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_ADDR(x), HWIO_HS_PHY_CTRL_RMSK)
#define HWIO_HS_PHY_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_ADDR(x),m,v,HWIO_HS_PHY_CTRL_IN(x))
#define HWIO_HS_PHY_CTRL_FREECLOCK_SEL_BMSK                                 0x20000000
#define HWIO_HS_PHY_CTRL_FREECLOCK_SEL_SHFT                                       0x1d
#define HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK                                0x10000000
#define HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT                                      0x1c
#define HWIO_HS_PHY_CTRL_FREECLK_DIS_WHEN_SUSP_BMSK                          0x8000000
#define HWIO_HS_PHY_CTRL_FREECLK_DIS_WHEN_SUSP_SHFT                               0x1b
#define HWIO_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                       0x4000000
#define HWIO_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                            0x1a
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_BMSK                              0x800000
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_SHFT                                  0x17
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_BMSK                                  0x400000
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SHFT                                      0x16
#define HWIO_HS_PHY_CTRL_USB2_UTMI_CLK_EN_BMSK                                0x200000
#define HWIO_HS_PHY_CTRL_USB2_UTMI_CLK_EN_SHFT                                    0x15
#define HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK                             0x100000
#define HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT                                 0x14
#define HWIO_HS_PHY_CTRL_AUTORESUME_BMSK                                       0x80000
#define HWIO_HS_PHY_CTRL_AUTORESUME_SHFT                                          0x13
#define HWIO_HS_PHY_CTRL_VBUSVLDEXT0_BMSK                                       0x2000
#define HWIO_HS_PHY_CTRL_VBUSVLDEXT0_SHFT                                          0xd
#define HWIO_HS_PHY_CTRL_ULPIPOR_BMSK                                            0x400
#define HWIO_HS_PHY_CTRL_ULPIPOR_SHFT                                              0xa
#define HWIO_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_BMSK                                   0x200
#define HWIO_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_SHFT                                     0x9
#define HWIO_HS_PHY_CTRL_AUTORSMENB0_BMSK                                          0x1
#define HWIO_HS_PHY_CTRL_AUTORSMENB0_SHFT                                          0x0

#define HWIO_PARAMETER_OVERRIDE_X_ADDR(x)                                   ((x) + 0x00000014)
#define HWIO_PARAMETER_OVERRIDE_X_OFFS                                      (0x00000014)
#define HWIO_PARAMETER_OVERRIDE_X_RMSK                                       0x3ffffc7
#define HWIO_PARAMETER_OVERRIDE_X_IN(x)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_ADDR(x), HWIO_PARAMETER_OVERRIDE_X_RMSK)
#define HWIO_PARAMETER_OVERRIDE_X_INM(x, m)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_ADDR(x), m)
#define HWIO_PARAMETER_OVERRIDE_X_OUT(x, v)      \
        out_dword(HWIO_PARAMETER_OVERRIDE_X_ADDR(x),v)
#define HWIO_PARAMETER_OVERRIDE_X_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PARAMETER_OVERRIDE_X_ADDR(x),m,v,HWIO_PARAMETER_OVERRIDE_X_IN(x))
#define HWIO_PARAMETER_OVERRIDE_X_TXFSLSTUNE0_BMSK                           0x3c00000
#define HWIO_PARAMETER_OVERRIDE_X_TXFSLSTUNE0_SHFT                                0x16
#define HWIO_PARAMETER_OVERRIDE_X_TXRESTUNE0_BMSK                             0x300000
#define HWIO_PARAMETER_OVERRIDE_X_TXRESTUNE0_SHFT                                 0x14
#define HWIO_PARAMETER_OVERRIDE_X_TXHSXVTUNE0_BMSK                             0xc0000
#define HWIO_PARAMETER_OVERRIDE_X_TXHSXVTUNE0_SHFT                                0x12
#define HWIO_PARAMETER_OVERRIDE_X_TXRISETUNE0_BMSK                             0x30000
#define HWIO_PARAMETER_OVERRIDE_X_TXRISETUNE0_SHFT                                0x10
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPAMPTUNE0_BMSK                         0xc000
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPAMPTUNE0_SHFT                            0xe
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPPULSETUNE0_BMSK                       0x2000
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPPULSETUNE0_SHFT                          0xd
#define HWIO_PARAMETER_OVERRIDE_X_TXVREFTUNE0_BMSK                              0x1e00
#define HWIO_PARAMETER_OVERRIDE_X_TXVREFTUNE0_SHFT                                 0x9
#define HWIO_PARAMETER_OVERRIDE_X_SQRXTUNE0_BMSK                                 0x1c0
#define HWIO_PARAMETER_OVERRIDE_X_SQRXTUNE0_SHFT                                   0x6
#define HWIO_PARAMETER_OVERRIDE_X_COMPDISTUNE0_BMSK                                0x7
#define HWIO_PARAMETER_OVERRIDE_X_COMPDISTUNE0_SHFT                                0x0

#define HWIO_CHARGING_DET_CTRL_ADDR(x)                                      ((x) + 0x00000018)
#define HWIO_CHARGING_DET_CTRL_OFFS                                         (0x00000018)
#define HWIO_CHARGING_DET_CTRL_RMSK                                               0x36
#define HWIO_CHARGING_DET_CTRL_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_ADDR(x), HWIO_CHARGING_DET_CTRL_RMSK)
#define HWIO_CHARGING_DET_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_ADDR(x), m)
#define HWIO_CHARGING_DET_CTRL_OUT(x, v)      \
        out_dword(HWIO_CHARGING_DET_CTRL_ADDR(x),v)
#define HWIO_CHARGING_DET_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CHARGING_DET_CTRL_ADDR(x),m,v,HWIO_CHARGING_DET_CTRL_IN(x))
#define HWIO_CHARGING_DET_CTRL_VDATDETENB0_BMSK                                   0x20
#define HWIO_CHARGING_DET_CTRL_VDATDETENB0_SHFT                                    0x5
#define HWIO_CHARGING_DET_CTRL_VDATSRCENB0_BMSK                                   0x10
#define HWIO_CHARGING_DET_CTRL_VDATSRCENB0_SHFT                                    0x4
//#define HWIO_CHARGING_DET_CTRL_VDMSRCAUTO_BMSK                                   0x8
//#define HWIO_CHARGING_DET_CTRL_VDMSRCAUTO_SHFT                                   0x3
#define HWIO_CHARGING_DET_CTRL_CHRGSEL0_BMSK                                       0x4
#define HWIO_CHARGING_DET_CTRL_CHRGSEL0_SHFT                                       0x2
#define HWIO_CHARGING_DET_CTRL_DCDENB0_BMSK                                        0x2
#define HWIO_CHARGING_DET_CTRL_DCDENB0_SHFT                                        0x1
//#define HWIO_CHARGING_DET_CTRL_ACAENB0_BMSK                                      0x1
//#define HWIO_CHARGING_DET_CTRL_ACAENB0_SHFT                                      0x0

#define HWIO_CHARGING_DET_OUTPUT_ADDR(x)                                    ((x) + 0x0000001c)
#define HWIO_CHARGING_DET_OUTPUT_OFFS                                       (0x0000001c)
#define HWIO_CHARGING_DET_OUTPUT_RMSK                                            0x303
#define HWIO_CHARGING_DET_OUTPUT_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_ADDR(x), HWIO_CHARGING_DET_OUTPUT_RMSK)
#define HWIO_CHARGING_DET_OUTPUT_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_ADDR(x), m)
#define HWIO_CHARGING_DET_OUTPUT_LINESTATE_BMSK                                  0x300
#define HWIO_CHARGING_DET_OUTPUT_LINESTATE_SHFT                                    0x8
#define HWIO_CHARGING_DET_OUTPUT_DCDOUT_BMSK                                       0x2
#define HWIO_CHARGING_DET_OUTPUT_DCDOUT_SHFT                                       0x1
#define HWIO_CHARGING_DET_OUTPUT_CHGDET_BMSK                                       0x1
#define HWIO_CHARGING_DET_OUTPUT_CHGDET_SHFT                                       0x0

#define HWIO_ALT_INTERRUPT_EN_ADDR(x)                                       ((x) + 0x00000020)
#define HWIO_ALT_INTERRUPT_EN_OFFS                                          (0x00000020)
#define HWIO_ALT_INTERRUPT_EN_RMSK                                              0x301e
#define HWIO_ALT_INTERRUPT_EN_IN(x)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_ADDR(x), HWIO_ALT_INTERRUPT_EN_RMSK)
#define HWIO_ALT_INTERRUPT_EN_INM(x, m)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_ADDR(x), m)
#define HWIO_ALT_INTERRUPT_EN_OUT(x, v)      \
        out_dword(HWIO_ALT_INTERRUPT_EN_ADDR(x),v)
#define HWIO_ALT_INTERRUPT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ALT_INTERRUPT_EN_ADDR(x),m,v,HWIO_ALT_INTERRUPT_EN_IN(x))
#define HWIO_ALT_INTERRUPT_EN_IDDIG_INTEN_BMSK                                  0x2000
#define HWIO_ALT_INTERRUPT_EN_IDDIG_INTEN_SHFT                                     0xd
#define HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_BMSK                              0x1000
#define HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_SHFT                                 0xc
#define HWIO_ALT_INTERRUPT_EN_CHGDETINTEN_BMSK                                    0x10
#define HWIO_ALT_INTERRUPT_EN_CHGDETINTEN_SHFT                                     0x4
#define HWIO_ALT_INTERRUPT_EN_DPINTEN_BMSK                                         0x8
#define HWIO_ALT_INTERRUPT_EN_DPINTEN_SHFT                                         0x3
#define HWIO_ALT_INTERRUPT_EN_DCDINTEN_BMSK                                        0x4
#define HWIO_ALT_INTERRUPT_EN_DCDINTEN_SHFT                                        0x2
#define HWIO_ALT_INTERRUPT_EN_DMINTEN_BMSK                                         0x2
#define HWIO_ALT_INTERRUPT_EN_DMINTEN_SHFT                                         0x1

#define HWIO_HS_PHY_IRQ_STAT_ADDR(x)                                        ((x) + 0x00000024)
#define HWIO_HS_PHY_IRQ_STAT_OFFS                                           (0x00000024)
#define HWIO_HS_PHY_IRQ_STAT_RMSK                                               0x101e
#define HWIO_HS_PHY_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_ADDR(x), HWIO_HS_PHY_IRQ_STAT_RMSK)
#define HWIO_HS_PHY_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_ADDR(x), m)
#define HWIO_HS_PHY_IRQ_STAT_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_IRQ_STAT_ADDR(x),v)
#define HWIO_HS_PHY_IRQ_STAT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_IRQ_STAT_ADDR(x),m,v,HWIO_HS_PHY_IRQ_STAT_IN(x))
#define HWIO_HS_PHY_IRQ_STAT_LINESTATE_INTLCH_BMSK                              0x1000
#define HWIO_HS_PHY_IRQ_STAT_LINESTATE_INTLCH_SHFT                                 0xc
#define HWIO_HS_PHY_IRQ_STAT_CHGDETINTLCH_BMSK                                    0x10
#define HWIO_HS_PHY_IRQ_STAT_CHGDETINTLCH_SHFT                                     0x4
#define HWIO_HS_PHY_IRQ_STAT_DPINTLCH_BMSK                                         0x8
#define HWIO_HS_PHY_IRQ_STAT_DPINTLCH_SHFT                                         0x3
#define HWIO_HS_PHY_IRQ_STAT_DCDINTLCH_BMSK                                        0x4
#define HWIO_HS_PHY_IRQ_STAT_DCDINTLCH_SHFT                                        0x2
#define HWIO_HS_PHY_IRQ_STAT_DMINTLCH_BMSK                                         0x2
#define HWIO_HS_PHY_IRQ_STAT_DMINTLCH_SHFT                                         0x1

#define HWIO_CGCTL_REG_ADDR(x)                                              ((x) + 0x00000028)
#define HWIO_CGCTL_REG_OFFS                                                 (0x00000028)
#define HWIO_CGCTL_REG_RMSK                                                       0x1e
#define HWIO_CGCTL_REG_IN(x)      \
        in_dword_masked(HWIO_CGCTL_REG_ADDR(x), HWIO_CGCTL_REG_RMSK)
#define HWIO_CGCTL_REG_INM(x, m)      \
        in_dword_masked(HWIO_CGCTL_REG_ADDR(x), m)
#define HWIO_CGCTL_REG_OUT(x, v)      \
        out_dword(HWIO_CGCTL_REG_ADDR(x),v)
#define HWIO_CGCTL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CGCTL_REG_ADDR(x),m,v,HWIO_CGCTL_REG_IN(x))
#define HWIO_CGCTL_REG_RAM13_EN_BMSK                                              0x10
#define HWIO_CGCTL_REG_RAM13_EN_SHFT                                               0x4
#define HWIO_CGCTL_REG_RAM1112_EN_BMSK                                             0x8
#define HWIO_CGCTL_REG_RAM1112_EN_SHFT                                             0x3
#define HWIO_CGCTL_REG_BAM_NDP_EN_BMSK                                             0x4
#define HWIO_CGCTL_REG_BAM_NDP_EN_SHFT                                             0x2
#define HWIO_CGCTL_REG_DBM_FSM_EN_BMSK                                             0x2
#define HWIO_CGCTL_REG_DBM_FSM_EN_SHFT                                             0x1

#define HWIO_DBG_BUS_REG_ADDR(x)                                            ((x) + 0x0000002c)
#define HWIO_DBG_BUS_REG_OFFS                                               (0x0000002c)
#define HWIO_DBG_BUS_REG_RMSK                                                0xf1ff001
#define HWIO_DBG_BUS_REG_IN(x)      \
        in_dword_masked(HWIO_DBG_BUS_REG_ADDR(x), HWIO_DBG_BUS_REG_RMSK)
#define HWIO_DBG_BUS_REG_INM(x, m)      \
        in_dword_masked(HWIO_DBG_BUS_REG_ADDR(x), m)
#define HWIO_DBG_BUS_REG_OUT(x, v)      \
        out_dword(HWIO_DBG_BUS_REG_ADDR(x),v)
#define HWIO_DBG_BUS_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DBG_BUS_REG_ADDR(x),m,v,HWIO_DBG_BUS_REG_IN(x))
#define HWIO_DBG_BUS_REG_GENERAL_DBG_SEL_BMSK                                0xf000000
#define HWIO_DBG_BUS_REG_GENERAL_DBG_SEL_SHFT                                     0x18
#define HWIO_DBG_BUS_REG_DBM_DBG_EN_BMSK                                      0x100000
#define HWIO_DBG_BUS_REG_DBM_DBG_EN_SHFT                                          0x14
#define HWIO_DBG_BUS_REG_DBM_DBG_SEL_BMSK                                      0xff000
#define HWIO_DBG_BUS_REG_DBM_DBG_SEL_SHFT                                          0xc
#define HWIO_DBG_BUS_REG_CTRL_DBG_SEL_BMSK                                         0x1
#define HWIO_DBG_BUS_REG_CTRL_DBG_SEL_SHFT                                         0x0

#define HWIO_SS_PHY_CTRL_ADDR(x)                                            ((x) + 0x00000030)
#define HWIO_SS_PHY_CTRL_OFFS                                               (0x00000030)
#define HWIO_SS_PHY_CTRL_RMSK                                               0x7f3fff7f
#define HWIO_SS_PHY_CTRL_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_ADDR(x), HWIO_SS_PHY_CTRL_RMSK)
#define HWIO_SS_PHY_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_ADDR(x), m)
#define HWIO_SS_PHY_CTRL_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_CTRL_ADDR(x),v)
#define HWIO_SS_PHY_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_CTRL_ADDR(x),m,v,HWIO_SS_PHY_CTRL_IN(x))
#define HWIO_SS_PHY_CTRL_VREG_BYPASS_BMSK                                   0x40000000
#define HWIO_SS_PHY_CTRL_VREG_BYPASS_SHFT                                         0x1e
#define HWIO_SS_PHY_CTRL_SSC_EN_BMSK                                        0x20000000
#define HWIO_SS_PHY_CTRL_SSC_EN_SHFT                                              0x1d
#define HWIO_SS_PHY_CTRL_REF_USE_PAD_BMSK                                   0x10000000
#define HWIO_SS_PHY_CTRL_REF_USE_PAD_SHFT                                         0x1c
#define HWIO_SS_PHY_CTRL_TEST_BURNIN_BMSK                                    0x8000000
#define HWIO_SS_PHY_CTRL_TEST_BURNIN_SHFT                                         0x1b
#define HWIO_SS_PHY_CTRL_TEST_POWERDOWN_BMSK                                 0x4000000
#define HWIO_SS_PHY_CTRL_TEST_POWERDOWN_SHFT                                      0x1a
#define HWIO_SS_PHY_CTRL_RTUNE_REQ_BMSK                                      0x2000000
#define HWIO_SS_PHY_CTRL_RTUNE_REQ_SHFT                                           0x19
#define HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK                              0x1000000
#define HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT                                   0x18
#define HWIO_SS_PHY_CTRL_SSC_REF_CLK_SEL_BMSK                                 0x3fe000
#define HWIO_SS_PHY_CTRL_SSC_REF_CLK_SEL_SHFT                                      0xd
#define HWIO_SS_PHY_CTRL_SSC_RANGE_BMSK                                         0x1c00
#define HWIO_SS_PHY_CTRL_SSC_RANGE_SHFT                                            0xa
#define HWIO_SS_PHY_CTRL_REF_USB2_EN_BMSK                                        0x200
#define HWIO_SS_PHY_CTRL_REF_USB2_EN_SHFT                                          0x9
#define HWIO_SS_PHY_CTRL_REF_SS_PHY_EN_BMSK                                      0x100
#define HWIO_SS_PHY_CTRL_REF_SS_PHY_EN_SHFT                                        0x8
#define HWIO_SS_PHY_CTRL_MPLL_MULTI_BMSK                                          0x7f
#define HWIO_SS_PHY_CTRL_MPLL_MULTI_SHFT                                           0x0

#define HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x)                                    ((x) + 0x00000034)
#define HWIO_SS_PHY_PARAM_CTRL_1_OFFS                                       (0x00000034)
#define HWIO_SS_PHY_PARAM_CTRL_1_RMSK                                       0xffffffff
#define HWIO_SS_PHY_PARAM_CTRL_1_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x), HWIO_SS_PHY_PARAM_CTRL_1_RMSK)
#define HWIO_SS_PHY_PARAM_CTRL_1_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x), m)
#define HWIO_SS_PHY_PARAM_CTRL_1_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x),v)
#define HWIO_SS_PHY_PARAM_CTRL_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x),m,v,HWIO_SS_PHY_PARAM_CTRL_1_IN(x))
#define HWIO_SS_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_BMSK                  0xf8000000
#define HWIO_SS_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_SHFT                        0x1b
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_SWING_FULL_BMSK                          0x7f00000
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_SWING_FULL_SHFT                               0x14
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_BMSK                            0xfc000
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_SHFT                                0xe
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_BMSK                           0x3f00
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_SHFT                              0x8
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_LEVEL_BMSK                                   0xf8
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_LEVEL_SHFT                                    0x3
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_BIAS_BMSK                                     0x7
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_BIAS_SHFT                                     0x0

#define HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x)                                    ((x) + 0x00000038)
#define HWIO_SS_PHY_PARAM_CTRL_2_OFFS                                       (0x00000038)
#define HWIO_SS_PHY_PARAM_CTRL_2_RMSK                                             0x37
#define HWIO_SS_PHY_PARAM_CTRL_2_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x), HWIO_SS_PHY_PARAM_CTRL_2_RMSK)
#define HWIO_SS_PHY_PARAM_CTRL_2_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x), m)
#define HWIO_SS_PHY_PARAM_CTRL_2_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x),v)
#define HWIO_SS_PHY_PARAM_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x),m,v,HWIO_SS_PHY_PARAM_CTRL_2_IN(x))
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_BMSK                        0x20
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_SHFT                         0x5
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_BMSK                          0x10
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_SHFT                           0x4
#define HWIO_SS_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_BMSK                              0x7
#define HWIO_SS_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_SHFT                              0x0

#define HWIO_SS_STATUS_READ_ONLY_ADDR(x)                                    ((x) + 0x00000054)
#define HWIO_SS_STATUS_READ_ONLY_OFFS                                       (0x00000054)
#define HWIO_SS_STATUS_READ_ONLY_RMSK                                              0x3
#define HWIO_SS_STATUS_READ_ONLY_IN(x)      \
        in_dword_masked(HWIO_SS_STATUS_READ_ONLY_ADDR(x), HWIO_SS_STATUS_READ_ONLY_RMSK)
#define HWIO_SS_STATUS_READ_ONLY_INM(x, m)      \
        in_dword_masked(HWIO_SS_STATUS_READ_ONLY_ADDR(x), m)
#define HWIO_SS_STATUS_READ_ONLY_REF_CLKREQ_N_BMSK                                 0x2
#define HWIO_SS_STATUS_READ_ONLY_REF_CLKREQ_N_SHFT                                 0x1
#define HWIO_SS_STATUS_READ_ONLY_RTUNE_ACK_BMSK                                    0x1
#define HWIO_SS_STATUS_READ_ONLY_RTUNE_ACK_SHFT                                    0x0

#define HWIO_PWR_EVNT_IRQ_STAT_ADDR(x)                                      ((x) + 0x00000058)
#define HWIO_PWR_EVNT_IRQ_STAT_OFFS                                         (0x00000058)
#define HWIO_PWR_EVNT_IRQ_STAT_RMSK                                             0x7fff
#define HWIO_PWR_EVNT_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x), HWIO_PWR_EVNT_IRQ_STAT_RMSK)
#define HWIO_PWR_EVNT_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x), m)
#define HWIO_PWR_EVNT_IRQ_STAT_OUT(x, v)      \
        out_dword(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x),v)
#define HWIO_PWR_EVNT_IRQ_STAT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x),m,v,HWIO_PWR_EVNT_IRQ_STAT_IN(x))
#define HWIO_PWR_EVNT_IRQ_STAT_PME_IRQ_STAT_BMSK                                0x4000
#define HWIO_PWR_EVNT_IRQ_STAT_PME_IRQ_STAT_SHFT                                   0xe
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L1_IRQ_STAT_BMSK                         0x2000
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L1_IRQ_STAT_SHFT                            0xd
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_RX_ELECIDLE_IRQ_STAT_BMSK                0x1000
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_RX_ELECIDLE_IRQ_STAT_SHFT                   0xc
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_2_IRQ_STAT_BMSK               0x800
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_2_IRQ_STAT_SHFT                 0xb
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_2_IRQ_STAT_BMSK                0x400
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_2_IRQ_STAT_SHFT                  0xa
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_1_IRQ_STAT_BMSK               0x200
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_1_IRQ_STAT_SHFT                 0x9
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_1_IRQ_STAT_BMSK                0x100
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_1_IRQ_STAT_SHFT                  0x8
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_0_IRQ_STAT_BMSK                0x80
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_PER_PORT_0_IRQ_STAT_SHFT                 0x7
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_0_IRQ_STAT_BMSK                 0x40
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_PER_PORT_0_IRQ_STAT_SHFT                  0x6
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_IRQ_STAT_BMSK                           0x20
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_IRQ_STAT_SHFT                            0x5
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_IRQ_STAT_BMSK                            0x10
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_IRQ_STAT_SHFT                             0x4
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_OUT_P3_IRQ_STAT_BMSK                      0x8
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_OUT_P3_IRQ_STAT_SHFT                      0x3
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_IN_P3_IRQ_STAT_BMSK                       0x4
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_IN_P3_IRQ_STAT_SHFT                       0x2
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_REQ_IN_P3_IRQ_STAT_BMSK                         0x2
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_REQ_IN_P3_IRQ_STAT_SHFT                         0x1
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_GATE_IN_P3_IRQ_STAT_BMSK                        0x1
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_GATE_IN_P3_IRQ_STAT_SHFT                        0x0

#define HWIO_PWR_EVNT_IRQ_MASK_ADDR(x)                                      ((x) + 0x0000005c)
#define HWIO_PWR_EVNT_IRQ_MASK_OFFS                                         (0x0000005c)
#define HWIO_PWR_EVNT_IRQ_MASK_RMSK                                             0x7fff
#define HWIO_PWR_EVNT_IRQ_MASK_IN(x)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x), HWIO_PWR_EVNT_IRQ_MASK_RMSK)
#define HWIO_PWR_EVNT_IRQ_MASK_INM(x, m)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x), m)
#define HWIO_PWR_EVNT_IRQ_MASK_OUT(x, v)      \
        out_dword(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x),v)
#define HWIO_PWR_EVNT_IRQ_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x),m,v,HWIO_PWR_EVNT_IRQ_MASK_IN(x))
#define HWIO_PWR_EVNT_IRQ_MASK_PME_IRQ_MASK_BMSK                                0x4000
#define HWIO_PWR_EVNT_IRQ_MASK_PME_IRQ_MASK_SHFT                                   0xe
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L1_IRQ_MASK_BMSK                         0x2000
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L1_IRQ_MASK_SHFT                            0xd
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_RX_ELECIDLE_IRQ_MASK_BMSK                0x1000
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_RX_ELECIDLE_IRQ_MASK_SHFT                   0xc
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_2_IRQ_MASK_BMSK               0x800
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_2_IRQ_MASK_SHFT                 0xb
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_2_IRQ_MASK_BMSK                0x400
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_2_IRQ_MASK_SHFT                  0xa
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_1_IRQ_MASK_BMSK               0x200
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_1_IRQ_MASK_SHFT                 0x9
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_1_IRQ_MASK_BMSK                0x100
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_1_IRQ_MASK_SHFT                  0x8
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_0_IRQ_MASK_BMSK                0x80
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_PER_PORT_0_IRQ_MASK_SHFT                 0x7
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_0_IRQ_MASK_BMSK                 0x40
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_PER_PORT_0_IRQ_MASK_SHFT                  0x6
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_BMSK                           0x20
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_SHFT                            0x5
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_BMSK                            0x10
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_SHFT                             0x4
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_OUT_P3_IRQ_MASK_BMSK                      0x8
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_OUT_P3_IRQ_MASK_SHFT                      0x3
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_IN_P3_IRQ_MASK_BMSK                       0x4
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_IN_P3_IRQ_MASK_SHFT                       0x2
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_REQ_IN_P3_IRQ_MASK_BMSK                         0x2
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_REQ_IN_P3_IRQ_MASK_SHFT                         0x1
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_GATE_IN_P3_IRQ_MASK_BMSK                        0x1
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_GATE_IN_P3_IRQ_MASK_SHFT                        0x0

#define HWIO_HW_SW_EVT_CTRL_REG_ADDR(x)                                     ((x) + 0x00000060)
#define HWIO_HW_SW_EVT_CTRL_REG_OFFS                                        (0x00000060)
#define HWIO_HW_SW_EVT_CTRL_REG_RMSK                                             0x131
#define HWIO_HW_SW_EVT_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x), HWIO_HW_SW_EVT_CTRL_REG_RMSK)
#define HWIO_HW_SW_EVT_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x), m)
#define HWIO_HW_SW_EVT_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x),v)
#define HWIO_HW_SW_EVT_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x),m,v,HWIO_HW_SW_EVT_CTRL_REG_IN(x))
#define HWIO_HW_SW_EVT_CTRL_REG_SW_EVT_MUX_SEL_BMSK                              0x100
#define HWIO_HW_SW_EVT_CTRL_REG_SW_EVT_MUX_SEL_SHFT                                0x8
#define HWIO_HW_SW_EVT_CTRL_REG_HW_EVT_MUX_CTRL_BMSK                              0x30
#define HWIO_HW_SW_EVT_CTRL_REG_HW_EVT_MUX_CTRL_SHFT                               0x4
#define HWIO_HW_SW_EVT_CTRL_REG_EVENT_BUS_HALT_BMSK                                0x1
#define HWIO_HW_SW_EVT_CTRL_REG_EVENT_BUS_HALT_SHFT                                0x0

#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x)                               ((x) + 0x00000064)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_OFFS                                  (0x00000064)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_RMSK                                         0x7
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x), HWIO_VMIDMT_AMEMTYPE_CTRL_REG_RMSK)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x), m)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x),v)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x),m,v,HWIO_VMIDMT_AMEMTYPE_CTRL_REG_IN(x))
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_VMIDMT_AMEMTYPE_VALUE_BMSK                   0x7
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_VMIDMT_AMEMTYPE_VALUE_SHFT                   0x0

#define HWIO_FLADJ_30MHZ_REG_ADDR(x)                                        ((x) + 0x00000068)
#define HWIO_FLADJ_30MHZ_REG_OFFS                                           (0x00000068)
#define HWIO_FLADJ_30MHZ_REG_RMSK                                                 0x3f
#define HWIO_FLADJ_30MHZ_REG_IN(x)      \
        in_dword_masked(HWIO_FLADJ_30MHZ_REG_ADDR(x), HWIO_FLADJ_30MHZ_REG_RMSK)
#define HWIO_FLADJ_30MHZ_REG_INM(x, m)      \
        in_dword_masked(HWIO_FLADJ_30MHZ_REG_ADDR(x), m)
#define HWIO_FLADJ_30MHZ_REG_OUT(x, v)      \
        out_dword(HWIO_FLADJ_30MHZ_REG_ADDR(x),v)
#define HWIO_FLADJ_30MHZ_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_FLADJ_30MHZ_REG_ADDR(x),m,v,HWIO_FLADJ_30MHZ_REG_IN(x))
#define HWIO_FLADJ_30MHZ_REG_FLADJ_30MHZ_VALUE_BMSK                               0x3f
#define HWIO_FLADJ_30MHZ_REG_FLADJ_30MHZ_VALUE_SHFT                                0x0

#define HWIO_M_AW_USER_REG_ADDR(x)                                          ((x) + 0x0000006c)
#define HWIO_M_AW_USER_REG_OFFS                                             (0x0000006c)
#define HWIO_M_AW_USER_REG_RMSK                                                  0x97f
#define HWIO_M_AW_USER_REG_IN(x)      \
        in_dword_masked(HWIO_M_AW_USER_REG_ADDR(x), HWIO_M_AW_USER_REG_RMSK)
#define HWIO_M_AW_USER_REG_INM(x, m)      \
        in_dword_masked(HWIO_M_AW_USER_REG_ADDR(x), m)
#define HWIO_M_AW_USER_REG_OUT(x, v)      \
        out_dword(HWIO_M_AW_USER_REG_ADDR(x),v)
#define HWIO_M_AW_USER_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_M_AW_USER_REG_ADDR(x),m,v,HWIO_M_AW_USER_REG_IN(x))
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_1_SEL_BMSK                                 0x800
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_1_SEL_SHFT                                   0xb
#define HWIO_M_AW_USER_REG_AW_NOALLOACATE_BMSK                                   0x100
#define HWIO_M_AW_USER_REG_AW_NOALLOACATE_SHFT                                     0x8
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_BMSK                                        0x70
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_SHFT                                         0x4
#define HWIO_M_AW_USER_REG_AW_CACHE_BMSK                                           0xf
#define HWIO_M_AW_USER_REG_AW_CACHE_SHFT                                           0x0

#define HWIO_M_AR_USER_REG_ADDR(x)                                          ((x) + 0x00000070)
#define HWIO_M_AR_USER_REG_OFFS                                             (0x00000070)
#define HWIO_M_AR_USER_REG_RMSK                                                  0x97f
#define HWIO_M_AR_USER_REG_IN(x)      \
        in_dword_masked(HWIO_M_AR_USER_REG_ADDR(x), HWIO_M_AR_USER_REG_RMSK)
#define HWIO_M_AR_USER_REG_INM(x, m)      \
        in_dword_masked(HWIO_M_AR_USER_REG_ADDR(x), m)
#define HWIO_M_AR_USER_REG_OUT(x, v)      \
        out_dword(HWIO_M_AR_USER_REG_ADDR(x),v)
#define HWIO_M_AR_USER_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_M_AR_USER_REG_ADDR(x),m,v,HWIO_M_AR_USER_REG_IN(x))
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_1_SEL_BMSK                                 0x800
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_1_SEL_SHFT                                   0xb
#define HWIO_M_AR_USER_REG_AR_NOALLOACATE_BMSK                                   0x100
#define HWIO_M_AR_USER_REG_AR_NOALLOACATE_SHFT                                     0x8
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_BMSK                                        0x70
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_SHFT                                         0x4
#define HWIO_M_AR_USER_REG_AR_CACHE_BMSK                                           0xf
#define HWIO_M_AR_USER_REG_AR_CACHE_SHFT                                           0x0

#define HWIO_HS_RX_TESTER_1_REG_ADDR(x)                                     ((x) + 0x00000074)
#define HWIO_HS_RX_TESTER_1_REG_OFFS                                        (0x00000074)
#define HWIO_HS_RX_TESTER_1_REG_RMSK                                         0x1ffffff
#define HWIO_HS_RX_TESTER_1_REG_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_1_REG_ADDR(x), HWIO_HS_RX_TESTER_1_REG_RMSK)
#define HWIO_HS_RX_TESTER_1_REG_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_1_REG_ADDR(x), m)
#define HWIO_HS_RX_TESTER_1_REG_OUT(x, v)      \
        out_dword(HWIO_HS_RX_TESTER_1_REG_ADDR(x),v)
#define HWIO_HS_RX_TESTER_1_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_RX_TESTER_1_REG_ADDR(x),m,v,HWIO_HS_RX_TESTER_1_REG_IN(x))
#define HWIO_HS_RX_TESTER_1_REG_USB_HS_RX_TEST_EN_BMSK                       0x1000000
#define HWIO_HS_RX_TESTER_1_REG_USB_HS_RX_TEST_EN_SHFT                            0x18
#define HWIO_HS_RX_TESTER_1_REG_RX_PKT_VALUE_BMSK                             0xffffff
#define HWIO_HS_RX_TESTER_1_REG_RX_PKT_VALUE_SHFT                                  0x0

#define HWIO_HS_PHY_CTRL_2_ADDR(x)                                          ((x) + 0x0000007c)
#define HWIO_HS_PHY_CTRL_2_OFFS                                             (0x0000007c)
#define HWIO_HS_PHY_CTRL_2_RMSK                                              0x4f82400
#define HWIO_HS_PHY_CTRL_2_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_2_ADDR(x), HWIO_HS_PHY_CTRL_2_RMSK)
#define HWIO_HS_PHY_CTRL_2_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_2_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_2_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_2_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_2_ADDR(x),m,v,HWIO_HS_PHY_CTRL_2_IN(x))
#define HWIO_HS_PHY_CTRL_2_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                     0x4000000
#define HWIO_HS_PHY_CTRL_2_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                          0x1a
#define HWIO_HS_PHY_CTRL_2_USB2_SUSPEND_N_SEL_BMSK                            0x800000
#define HWIO_HS_PHY_CTRL_2_USB2_SUSPEND_N_SEL_SHFT                                0x17
#define HWIO_HS_PHY_CTRL_2_USB2_SUSPEND_N_BMSK                                0x400000
#define HWIO_HS_PHY_CTRL_2_USB2_SUSPEND_N_SHFT                                    0x16
#define HWIO_HS_PHY_CTRL_2_USB2_UTMI_CLK_EN_BMSK                              0x200000
#define HWIO_HS_PHY_CTRL_2_USB2_UTMI_CLK_EN_SHFT                                  0x15
#define HWIO_HS_PHY_CTRL_2_UTMI_OTG_VBUS_VALID_BMSK                           0x100000
#define HWIO_HS_PHY_CTRL_2_UTMI_OTG_VBUS_VALID_SHFT                               0x14
#define HWIO_HS_PHY_CTRL_2_AUTORESUME_BMSK                                     0x80000
#define HWIO_HS_PHY_CTRL_2_AUTORESUME_SHFT                                        0x13
#define HWIO_HS_PHY_CTRL_2_VBUSVLDEXT0_BMSK                                     0x2000
#define HWIO_HS_PHY_CTRL_2_VBUSVLDEXT0_SHFT                                        0xd
#define HWIO_HS_PHY_CTRL_2_ULPIPOR_BMSK                                          0x400
#define HWIO_HS_PHY_CTRL_2_ULPIPOR_SHFT                                            0xa

#define HWIO_PARAMETER_OVERRIDE_X_2_ADDR(x)                                 ((x) + 0x00000080)
#define HWIO_PARAMETER_OVERRIDE_X_2_OFFS                                    (0x00000080)
#define HWIO_PARAMETER_OVERRIDE_X_2_RMSK                                     0x3ffffc7
#define HWIO_PARAMETER_OVERRIDE_X_2_IN(x)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_2_ADDR(x), HWIO_PARAMETER_OVERRIDE_X_2_RMSK)
#define HWIO_PARAMETER_OVERRIDE_X_2_INM(x, m)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_2_ADDR(x), m)
#define HWIO_PARAMETER_OVERRIDE_X_2_OUT(x, v)      \
        out_dword(HWIO_PARAMETER_OVERRIDE_X_2_ADDR(x),v)
#define HWIO_PARAMETER_OVERRIDE_X_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PARAMETER_OVERRIDE_X_2_ADDR(x),m,v,HWIO_PARAMETER_OVERRIDE_X_2_IN(x))
#define HWIO_PARAMETER_OVERRIDE_X_2_TXFSLSTUNE0_BMSK                         0x3c00000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXFSLSTUNE0_SHFT                              0x16
#define HWIO_PARAMETER_OVERRIDE_X_2_TXRESTUNE0_BMSK                           0x300000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXRESTUNE0_SHFT                               0x14
#define HWIO_PARAMETER_OVERRIDE_X_2_TXHSXVTUNE0_BMSK                           0xc0000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXHSXVTUNE0_SHFT                              0x12
#define HWIO_PARAMETER_OVERRIDE_X_2_TXRISETUNE0_BMSK                           0x30000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXRISETUNE0_SHFT                              0x10
#define HWIO_PARAMETER_OVERRIDE_X_2_TXPREEMPAMPTUNE0_BMSK                       0xc000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXPREEMPAMPTUNE0_SHFT                          0xe
#define HWIO_PARAMETER_OVERRIDE_X_2_TXPREEMPPULSETUNE0_BMSK                     0x2000
#define HWIO_PARAMETER_OVERRIDE_X_2_TXPREEMPPULSETUNE0_SHFT                        0xd
#define HWIO_PARAMETER_OVERRIDE_X_2_TXVREFTUNE0_BMSK                            0x1e00
#define HWIO_PARAMETER_OVERRIDE_X_2_TXVREFTUNE0_SHFT                               0x9
#define HWIO_PARAMETER_OVERRIDE_X_2_SQRXTUNE0_BMSK                               0x1c0
#define HWIO_PARAMETER_OVERRIDE_X_2_SQRXTUNE0_SHFT                                 0x6
#define HWIO_PARAMETER_OVERRIDE_X_2_COMPDISTUNE0_BMSK                              0x7
#define HWIO_PARAMETER_OVERRIDE_X_2_COMPDISTUNE0_SHFT                              0x0

#define HWIO_CHARGING_DET_CTRL_2_ADDR(x)                                    ((x) + 0x00000084)
#define HWIO_CHARGING_DET_CTRL_2_OFFS                                       (0x00000084)
#define HWIO_CHARGING_DET_CTRL_2_RMSK                                             0x36
#define HWIO_CHARGING_DET_CTRL_2_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_2_ADDR(x), HWIO_CHARGING_DET_CTRL_2_RMSK)
#define HWIO_CHARGING_DET_CTRL_2_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_2_ADDR(x), m)
#define HWIO_CHARGING_DET_CTRL_2_OUT(x, v)      \
        out_dword(HWIO_CHARGING_DET_CTRL_2_ADDR(x),v)
#define HWIO_CHARGING_DET_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CHARGING_DET_CTRL_2_ADDR(x),m,v,HWIO_CHARGING_DET_CTRL_2_IN(x))
#define HWIO_CHARGING_DET_CTRL_2_VDATDETENB0_BMSK                                 0x20
#define HWIO_CHARGING_DET_CTRL_2_VDATDETENB0_SHFT                                  0x5
#define HWIO_CHARGING_DET_CTRL_2_VDATSRCENB0_BMSK                                 0x10
#define HWIO_CHARGING_DET_CTRL_2_VDATSRCENB0_SHFT                                  0x4
#define HWIO_CHARGING_DET_CTRL_2_CHRGSEL0_BMSK                                     0x4
#define HWIO_CHARGING_DET_CTRL_2_CHRGSEL0_SHFT                                     0x2
#define HWIO_CHARGING_DET_CTRL_2_DCDENB0_BMSK                                      0x2
#define HWIO_CHARGING_DET_CTRL_2_DCDENB0_SHFT                                      0x1

#define HWIO_CHARGING_DET_OUTPUT_2_ADDR(x)                                  ((x) + 0x00000088)
#define HWIO_CHARGING_DET_OUTPUT_2_OFFS                                     (0x00000088)
#define HWIO_CHARGING_DET_OUTPUT_2_RMSK                                          0x303
#define HWIO_CHARGING_DET_OUTPUT_2_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_2_ADDR(x), HWIO_CHARGING_DET_OUTPUT_2_RMSK)
#define HWIO_CHARGING_DET_OUTPUT_2_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_2_ADDR(x), m)
#define HWIO_CHARGING_DET_OUTPUT_2_LINESTATE_BMSK                                0x300
#define HWIO_CHARGING_DET_OUTPUT_2_LINESTATE_SHFT                                  0x8
#define HWIO_CHARGING_DET_OUTPUT_2_DCDOUT_BMSK                                     0x2
#define HWIO_CHARGING_DET_OUTPUT_2_DCDOUT_SHFT                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_2_CHGDET_BMSK                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_2_CHGDET_SHFT                                     0x0

#define HWIO_ALT_INTERRUPT_EN_2_ADDR(x)                                     ((x) + 0x0000008c)
#define HWIO_ALT_INTERRUPT_EN_2_OFFS                                        (0x0000008c)
#define HWIO_ALT_INTERRUPT_EN_2_RMSK                                            0x101e
#define HWIO_ALT_INTERRUPT_EN_2_IN(x)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_2_ADDR(x), HWIO_ALT_INTERRUPT_EN_2_RMSK)
#define HWIO_ALT_INTERRUPT_EN_2_INM(x, m)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_2_ADDR(x), m)
#define HWIO_ALT_INTERRUPT_EN_2_OUT(x, v)      \
        out_dword(HWIO_ALT_INTERRUPT_EN_2_ADDR(x),v)
#define HWIO_ALT_INTERRUPT_EN_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ALT_INTERRUPT_EN_2_ADDR(x),m,v,HWIO_ALT_INTERRUPT_EN_2_IN(x))
#define HWIO_ALT_INTERRUPT_EN_2_LINESTATE_INTEN_BMSK                            0x1000
#define HWIO_ALT_INTERRUPT_EN_2_LINESTATE_INTEN_SHFT                               0xc
#define HWIO_ALT_INTERRUPT_EN_2_CHGDETINTEN_BMSK                                  0x10
#define HWIO_ALT_INTERRUPT_EN_2_CHGDETINTEN_SHFT                                   0x4
#define HWIO_ALT_INTERRUPT_EN_2_DPINTEN_BMSK                                       0x8
#define HWIO_ALT_INTERRUPT_EN_2_DPINTEN_SHFT                                       0x3
#define HWIO_ALT_INTERRUPT_EN_2_DCDINTEN_BMSK                                      0x4
#define HWIO_ALT_INTERRUPT_EN_2_DCDINTEN_SHFT                                      0x2
#define HWIO_ALT_INTERRUPT_EN_2_DMINTEN_BMSK                                       0x2
#define HWIO_ALT_INTERRUPT_EN_2_DMINTEN_SHFT                                       0x1

#define HWIO_HS_PHY_IRQ_STAT_2_ADDR(x)                                      ((x) + 0x00000090)
#define HWIO_HS_PHY_IRQ_STAT_2_OFFS                                         (0x00000090)
#define HWIO_HS_PHY_IRQ_STAT_2_RMSK                                             0x101e
#define HWIO_HS_PHY_IRQ_STAT_2_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_2_ADDR(x), HWIO_HS_PHY_IRQ_STAT_2_RMSK)
#define HWIO_HS_PHY_IRQ_STAT_2_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_2_ADDR(x), m)
#define HWIO_HS_PHY_IRQ_STAT_2_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_IRQ_STAT_2_ADDR(x),v)
#define HWIO_HS_PHY_IRQ_STAT_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_IRQ_STAT_2_ADDR(x),m,v,HWIO_HS_PHY_IRQ_STAT_2_IN(x))
#define HWIO_HS_PHY_IRQ_STAT_2_LINESTATE_INTLCH_BMSK                            0x1000
#define HWIO_HS_PHY_IRQ_STAT_2_LINESTATE_INTLCH_SHFT                               0xc
#define HWIO_HS_PHY_IRQ_STAT_2_CHGDETINTLCH_BMSK                                  0x10
#define HWIO_HS_PHY_IRQ_STAT_2_CHGDETINTLCH_SHFT                                   0x4
#define HWIO_HS_PHY_IRQ_STAT_2_DPINTLCH_BMSK                                       0x8
#define HWIO_HS_PHY_IRQ_STAT_2_DPINTLCH_SHFT                                       0x3
#define HWIO_HS_PHY_IRQ_STAT_2_DCDINTLCH_BMSK                                      0x4
#define HWIO_HS_PHY_IRQ_STAT_2_DCDINTLCH_SHFT                                      0x2
#define HWIO_HS_PHY_IRQ_STAT_2_DMINTLCH_BMSK                                       0x2
#define HWIO_HS_PHY_IRQ_STAT_2_DMINTLCH_SHFT                                       0x1

#define HWIO_HS_PHY_CTRL_3_ADDR(x)                                          ((x) + 0x00000098)
#define HWIO_HS_PHY_CTRL_3_OFFS                                             (0x00000098)
#define HWIO_HS_PHY_CTRL_3_RMSK                                              0x4f82400
#define HWIO_HS_PHY_CTRL_3_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_3_ADDR(x), HWIO_HS_PHY_CTRL_3_RMSK)
#define HWIO_HS_PHY_CTRL_3_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_3_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_3_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_3_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_3_ADDR(x),m,v,HWIO_HS_PHY_CTRL_3_IN(x))
#define HWIO_HS_PHY_CTRL_3_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                     0x4000000
#define HWIO_HS_PHY_CTRL_3_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                          0x1a
#define HWIO_HS_PHY_CTRL_3_USB2_SUSPEND_N_SEL_BMSK                            0x800000
#define HWIO_HS_PHY_CTRL_3_USB2_SUSPEND_N_SEL_SHFT                                0x17
#define HWIO_HS_PHY_CTRL_3_USB2_SUSPEND_N_BMSK                                0x400000
#define HWIO_HS_PHY_CTRL_3_USB2_SUSPEND_N_SHFT                                    0x16
#define HWIO_HS_PHY_CTRL_3_USB2_UTMI_CLK_EN_BMSK                              0x200000
#define HWIO_HS_PHY_CTRL_3_USB2_UTMI_CLK_EN_SHFT                                  0x15
#define HWIO_HS_PHY_CTRL_3_UTMI_OTG_VBUS_VALID_BMSK                           0x100000
#define HWIO_HS_PHY_CTRL_3_UTMI_OTG_VBUS_VALID_SHFT                               0x14
#define HWIO_HS_PHY_CTRL_3_AUTORESUME_BMSK                                     0x80000
#define HWIO_HS_PHY_CTRL_3_AUTORESUME_SHFT                                        0x13
#define HWIO_HS_PHY_CTRL_3_VBUSVLDEXT0_BMSK                                     0x2000
#define HWIO_HS_PHY_CTRL_3_VBUSVLDEXT0_SHFT                                        0xd
#define HWIO_HS_PHY_CTRL_3_ULPIPOR_BMSK                                          0x400
#define HWIO_HS_PHY_CTRL_3_ULPIPOR_SHFT                                            0xa

#define HWIO_PARAMETER_OVERRIDE_X_3_ADDR(x)                                 ((x) + 0x0000009c)
#define HWIO_PARAMETER_OVERRIDE_X_3_OFFS                                    (0x0000009c)
#define HWIO_PARAMETER_OVERRIDE_X_3_RMSK                                     0x3ffffc7
#define HWIO_PARAMETER_OVERRIDE_X_3_IN(x)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_3_ADDR(x), HWIO_PARAMETER_OVERRIDE_X_3_RMSK)
#define HWIO_PARAMETER_OVERRIDE_X_3_INM(x, m)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_3_ADDR(x), m)
#define HWIO_PARAMETER_OVERRIDE_X_3_OUT(x, v)      \
        out_dword(HWIO_PARAMETER_OVERRIDE_X_3_ADDR(x),v)
#define HWIO_PARAMETER_OVERRIDE_X_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PARAMETER_OVERRIDE_X_3_ADDR(x),m,v,HWIO_PARAMETER_OVERRIDE_X_3_IN(x))
#define HWIO_PARAMETER_OVERRIDE_X_3_TXFSLSTUNE0_BMSK                         0x3c00000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXFSLSTUNE0_SHFT                              0x16
#define HWIO_PARAMETER_OVERRIDE_X_3_TXRESTUNE0_BMSK                           0x300000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXRESTUNE0_SHFT                               0x14
#define HWIO_PARAMETER_OVERRIDE_X_3_TXHSXVTUNE0_BMSK                           0xc0000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXHSXVTUNE0_SHFT                              0x12
#define HWIO_PARAMETER_OVERRIDE_X_3_TXRISETUNE0_BMSK                           0x30000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXRISETUNE0_SHFT                              0x10
#define HWIO_PARAMETER_OVERRIDE_X_3_TXPREEMPAMPTUNE0_BMSK                       0xc000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXPREEMPAMPTUNE0_SHFT                          0xe
#define HWIO_PARAMETER_OVERRIDE_X_3_TXPREEMPPULSETUNE0_BMSK                     0x2000
#define HWIO_PARAMETER_OVERRIDE_X_3_TXPREEMPPULSETUNE0_SHFT                        0xd
#define HWIO_PARAMETER_OVERRIDE_X_3_TXVREFTUNE0_BMSK                            0x1e00
#define HWIO_PARAMETER_OVERRIDE_X_3_TXVREFTUNE0_SHFT                               0x9
#define HWIO_PARAMETER_OVERRIDE_X_3_SQRXTUNE0_BMSK                               0x1c0
#define HWIO_PARAMETER_OVERRIDE_X_3_SQRXTUNE0_SHFT                                 0x6
#define HWIO_PARAMETER_OVERRIDE_X_3_COMPDISTUNE0_BMSK                              0x7
#define HWIO_PARAMETER_OVERRIDE_X_3_COMPDISTUNE0_SHFT                              0x0

#define HWIO_CHARGING_DET_CTRL_3_ADDR(x)                                    ((x) + 0x000000a0)
#define HWIO_CHARGING_DET_CTRL_3_OFFS                                       (0x000000a0)
#define HWIO_CHARGING_DET_CTRL_3_RMSK                                             0x36
#define HWIO_CHARGING_DET_CTRL_3_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_3_ADDR(x), HWIO_CHARGING_DET_CTRL_3_RMSK)
#define HWIO_CHARGING_DET_CTRL_3_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_3_ADDR(x), m)
#define HWIO_CHARGING_DET_CTRL_3_OUT(x, v)      \
        out_dword(HWIO_CHARGING_DET_CTRL_3_ADDR(x),v)
#define HWIO_CHARGING_DET_CTRL_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CHARGING_DET_CTRL_3_ADDR(x),m,v,HWIO_CHARGING_DET_CTRL_3_IN(x))
#define HWIO_CHARGING_DET_CTRL_3_VDATDETENB0_BMSK                                 0x20
#define HWIO_CHARGING_DET_CTRL_3_VDATDETENB0_SHFT                                  0x5
#define HWIO_CHARGING_DET_CTRL_3_VDATSRCENB0_BMSK                                 0x10
#define HWIO_CHARGING_DET_CTRL_3_VDATSRCENB0_SHFT                                  0x4
#define HWIO_CHARGING_DET_CTRL_3_CHRGSEL0_BMSK                                     0x4
#define HWIO_CHARGING_DET_CTRL_3_CHRGSEL0_SHFT                                     0x2
#define HWIO_CHARGING_DET_CTRL_3_DCDENB0_BMSK                                      0x2
#define HWIO_CHARGING_DET_CTRL_3_DCDENB0_SHFT                                      0x1

#define HWIO_CHARGING_DET_OUTPUT_3_ADDR(x)                                  ((x) + 0x000000a4)
#define HWIO_CHARGING_DET_OUTPUT_3_OFFS                                     (0x000000a4)
#define HWIO_CHARGING_DET_OUTPUT_3_RMSK                                          0x303
#define HWIO_CHARGING_DET_OUTPUT_3_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_3_ADDR(x), HWIO_CHARGING_DET_OUTPUT_3_RMSK)
#define HWIO_CHARGING_DET_OUTPUT_3_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_3_ADDR(x), m)
#define HWIO_CHARGING_DET_OUTPUT_3_LINESTATE_BMSK                                0x300
#define HWIO_CHARGING_DET_OUTPUT_3_LINESTATE_SHFT                                  0x8
#define HWIO_CHARGING_DET_OUTPUT_3_DCDOUT_BMSK                                     0x2
#define HWIO_CHARGING_DET_OUTPUT_3_DCDOUT_SHFT                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_3_CHGDET_BMSK                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_3_CHGDET_SHFT                                     0x0

#define HWIO_ALT_INTERRUPT_EN_3_ADDR(x)                                     ((x) + 0x000000a8)
#define HWIO_ALT_INTERRUPT_EN_3_OFFS                                        (0x000000a8)
#define HWIO_ALT_INTERRUPT_EN_3_RMSK                                            0x101e
#define HWIO_ALT_INTERRUPT_EN_3_IN(x)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_3_ADDR(x), HWIO_ALT_INTERRUPT_EN_3_RMSK)
#define HWIO_ALT_INTERRUPT_EN_3_INM(x, m)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_3_ADDR(x), m)
#define HWIO_ALT_INTERRUPT_EN_3_OUT(x, v)      \
        out_dword(HWIO_ALT_INTERRUPT_EN_3_ADDR(x),v)
#define HWIO_ALT_INTERRUPT_EN_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ALT_INTERRUPT_EN_3_ADDR(x),m,v,HWIO_ALT_INTERRUPT_EN_3_IN(x))
#define HWIO_ALT_INTERRUPT_EN_3_LINESTATE_INTEN_BMSK                            0x1000
#define HWIO_ALT_INTERRUPT_EN_3_LINESTATE_INTEN_SHFT                               0xc
#define HWIO_ALT_INTERRUPT_EN_3_CHGDETINTEN_BMSK                                  0x10
#define HWIO_ALT_INTERRUPT_EN_3_CHGDETINTEN_SHFT                                   0x4
#define HWIO_ALT_INTERRUPT_EN_3_DPINTEN_BMSK                                       0x8
#define HWIO_ALT_INTERRUPT_EN_3_DPINTEN_SHFT                                       0x3
#define HWIO_ALT_INTERRUPT_EN_3_DCDINTEN_BMSK                                      0x4
#define HWIO_ALT_INTERRUPT_EN_3_DCDINTEN_SHFT                                      0x2
#define HWIO_ALT_INTERRUPT_EN_3_DMINTEN_BMSK                                       0x2
#define HWIO_ALT_INTERRUPT_EN_3_DMINTEN_SHFT                                       0x1

#define HWIO_HS_PHY_IRQ_STAT_3_ADDR(x)                                      ((x) + 0x000000ac)
#define HWIO_HS_PHY_IRQ_STAT_3_OFFS                                         (0x000000ac)
#define HWIO_HS_PHY_IRQ_STAT_3_RMSK                                             0x101e
#define HWIO_HS_PHY_IRQ_STAT_3_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_3_ADDR(x), HWIO_HS_PHY_IRQ_STAT_3_RMSK)
#define HWIO_HS_PHY_IRQ_STAT_3_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_3_ADDR(x), m)
#define HWIO_HS_PHY_IRQ_STAT_3_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_IRQ_STAT_3_ADDR(x),v)
#define HWIO_HS_PHY_IRQ_STAT_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_IRQ_STAT_3_ADDR(x),m,v,HWIO_HS_PHY_IRQ_STAT_3_IN(x))
#define HWIO_HS_PHY_IRQ_STAT_3_LINESTATE_INTLCH_BMSK                            0x1000
#define HWIO_HS_PHY_IRQ_STAT_3_LINESTATE_INTLCH_SHFT                               0xc
#define HWIO_HS_PHY_IRQ_STAT_3_CHGDETINTLCH_BMSK                                  0x10
#define HWIO_HS_PHY_IRQ_STAT_3_CHGDETINTLCH_SHFT                                   0x4
#define HWIO_HS_PHY_IRQ_STAT_3_DPINTLCH_BMSK                                       0x8
#define HWIO_HS_PHY_IRQ_STAT_3_DPINTLCH_SHFT                                       0x3
#define HWIO_HS_PHY_IRQ_STAT_3_DCDINTLCH_BMSK                                      0x4
#define HWIO_HS_PHY_IRQ_STAT_3_DCDINTLCH_SHFT                                      0x2
#define HWIO_HS_PHY_IRQ_STAT_3_DMINTLCH_BMSK                                       0x2
#define HWIO_HS_PHY_IRQ_STAT_3_DMINTLCH_SHFT                                       0x1

#define HWIO_QSCRTCH_REG_n_ADDR(base,n)                                     ((base) + 0x000000b4 + 0x4 * (n))
#define HWIO_QSCRTCH_REG_n_OFFS(base,n)                                     (0x000000b4 + 0x4 * (n))
#define HWIO_QSCRTCH_REG_n_RMSK                                             0xffffffff
#define HWIO_QSCRTCH_REG_n_MAXn                                                      4
#define HWIO_QSCRTCH_REG_n_INI(base,n)        \
        in_dword_masked(HWIO_QSCRTCH_REG_n_ADDR(base,n), HWIO_QSCRTCH_REG_n_RMSK)
#define HWIO_QSCRTCH_REG_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QSCRTCH_REG_n_ADDR(base,n), mask)
#define HWIO_QSCRTCH_REG_n_OUTI(base,n,val)    \
        out_dword(HWIO_QSCRTCH_REG_n_ADDR(base,n),val)
#define HWIO_QSCRTCH_REG_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_QSCRTCH_REG_n_ADDR(base,n),mask,val,HWIO_QSCRTCH_REG_n_INI(base,n))
#define HWIO_QSCRTCH_REG_n_QSCRTCH_REG_BMSK                                 0xffffffff
#define HWIO_QSCRTCH_REG_n_QSCRTCH_REG_SHFT                                        0x0

#define HWIO_SS_PHY_CTRL_2_ADDR(x)                                          ((x) + 0x000000c8)
#define HWIO_SS_PHY_CTRL_2_OFFS                                             (0x000000c8)
#define HWIO_SS_PHY_CTRL_2_RMSK                                                  0x3ff
#define HWIO_SS_PHY_CTRL_2_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_2_ADDR(x), HWIO_SS_PHY_CTRL_2_RMSK)
#define HWIO_SS_PHY_CTRL_2_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_2_ADDR(x), m)
#define HWIO_SS_PHY_CTRL_2_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_CTRL_2_ADDR(x),v)
#define HWIO_SS_PHY_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_CTRL_2_ADDR(x),m,v,HWIO_SS_PHY_CTRL_2_IN(x))
#define HWIO_SS_PHY_CTRL_2_PCS_RX_LOS_MASK_VAL_BMSK                              0x3ff
#define HWIO_SS_PHY_CTRL_2_PCS_RX_LOS_MASK_VAL_SHFT                                0x0

#define HWIO_HS_PHY_TEST_OUT_1_ADDR(x)                                      ((x) + 0x000000cc)
#define HWIO_HS_PHY_TEST_OUT_1_OFFS                                         (0x000000cc)
#define HWIO_HS_PHY_TEST_OUT_1_RMSK                                                0xf
#define HWIO_HS_PHY_TEST_OUT_1_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_1_ADDR(x), HWIO_HS_PHY_TEST_OUT_1_RMSK)
#define HWIO_HS_PHY_TEST_OUT_1_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_1_ADDR(x), m)
#define HWIO_HS_PHY_TEST_OUT_1_TESTDATAOUT_BMSK                                    0xf
#define HWIO_HS_PHY_TEST_OUT_1_TESTDATAOUT_SHFT                                    0x0

#define HWIO_HS_PHY_TEST_OUT_2_ADDR(x)                                      ((x) + 0x000000d0)
#define HWIO_HS_PHY_TEST_OUT_2_OFFS                                         (0x000000d0)
#define HWIO_HS_PHY_TEST_OUT_2_RMSK                                                0xf
#define HWIO_HS_PHY_TEST_OUT_2_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_2_ADDR(x), HWIO_HS_PHY_TEST_OUT_2_RMSK)
#define HWIO_HS_PHY_TEST_OUT_2_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_2_ADDR(x), m)
#define HWIO_HS_PHY_TEST_OUT_2_TESTDATAOUT_BMSK                                    0xf
#define HWIO_HS_PHY_TEST_OUT_2_TESTDATAOUT_SHFT                                    0x0

#define HWIO_HS_PHY_TEST_OUT_3_ADDR(x)                                      ((x) + 0x000000d4)
#define HWIO_HS_PHY_TEST_OUT_3_OFFS                                         (0x000000d4)
#define HWIO_HS_PHY_TEST_OUT_3_RMSK                                                0xf
#define HWIO_HS_PHY_TEST_OUT_3_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_3_ADDR(x), HWIO_HS_PHY_TEST_OUT_3_RMSK)
#define HWIO_HS_PHY_TEST_OUT_3_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_TEST_OUT_3_ADDR(x), m)
#define HWIO_HS_PHY_TEST_OUT_3_TESTDATAOUT_BMSK                                    0xf
#define HWIO_HS_PHY_TEST_OUT_3_TESTDATAOUT_SHFT                                    0x0

#define HWIO_HS_RX_TESTER_2_ADDR(x)                                         ((x) + 0x000000d8)
#define HWIO_HS_RX_TESTER_2_OFFS                                            (0x000000d8)
#define HWIO_HS_RX_TESTER_2_RMSK                                             0x1ffffff
#define HWIO_HS_RX_TESTER_2_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_2_ADDR(x), HWIO_HS_RX_TESTER_2_RMSK)
#define HWIO_HS_RX_TESTER_2_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_2_ADDR(x), m)
#define HWIO_HS_RX_TESTER_2_OUT(x, v)      \
        out_dword(HWIO_HS_RX_TESTER_2_ADDR(x),v)
#define HWIO_HS_RX_TESTER_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_RX_TESTER_2_ADDR(x),m,v,HWIO_HS_RX_TESTER_2_IN(x))
#define HWIO_HS_RX_TESTER_2_USB_HS_RX_TEST_EN_BMSK                           0x1000000
#define HWIO_HS_RX_TESTER_2_USB_HS_RX_TEST_EN_SHFT                                0x18
#define HWIO_HS_RX_TESTER_2_RX_PKT_VALUE_BMSK                                 0xffffff
#define HWIO_HS_RX_TESTER_2_RX_PKT_VALUE_SHFT                                      0x0

#define HWIO_HS_RX_TESTER_3_ADDR(x)                                         ((x) + 0x000000dc)
#define HWIO_HS_RX_TESTER_3_OFFS                                            (0x000000dc)
#define HWIO_HS_RX_TESTER_3_RMSK                                             0x1ffffff
#define HWIO_HS_RX_TESTER_3_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_3_ADDR(x), HWIO_HS_RX_TESTER_3_RMSK)
#define HWIO_HS_RX_TESTER_3_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_3_ADDR(x), m)
#define HWIO_HS_RX_TESTER_3_OUT(x, v)      \
        out_dword(HWIO_HS_RX_TESTER_3_ADDR(x),v)
#define HWIO_HS_RX_TESTER_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_RX_TESTER_3_ADDR(x),m,v,HWIO_HS_RX_TESTER_3_IN(x))
#define HWIO_HS_RX_TESTER_3_USB_HS_RX_TEST_EN_BMSK                           0x1000000
#define HWIO_HS_RX_TESTER_3_USB_HS_RX_TEST_EN_SHFT                                0x18
#define HWIO_HS_RX_TESTER_3_RX_PKT_VALUE_BMSK                                 0xffffff
#define HWIO_HS_RX_TESTER_3_RX_PKT_VALUE_SHFT                                      0x0

#define HWIO_HS_RX_TESTER_OUT_1_ADDR(x)                                     ((x) + 0x000000e0)
#define HWIO_HS_RX_TESTER_OUT_1_OFFS                                        (0x000000e0)
#define HWIO_HS_RX_TESTER_OUT_1_RMSK                                              0x3f
#define HWIO_HS_RX_TESTER_OUT_1_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_1_ADDR(x), HWIO_HS_RX_TESTER_OUT_1_RMSK)
#define HWIO_HS_RX_TESTER_OUT_1_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_1_ADDR(x), m)
#define HWIO_HS_RX_TESTER_OUT_1_PHY_RX_TESTER_STATUS_BMSK                         0x3c
#define HWIO_HS_RX_TESTER_OUT_1_PHY_RX_TESTER_STATUS_SHFT                          0x2
#define HWIO_HS_RX_TESTER_OUT_1_RX_ALL_BAD_BMSK                                    0x2
#define HWIO_HS_RX_TESTER_OUT_1_RX_ALL_BAD_SHFT                                    0x1
#define HWIO_HS_RX_TESTER_OUT_1_RX_ALL_GOOD_BMSK                                   0x1
#define HWIO_HS_RX_TESTER_OUT_1_RX_ALL_GOOD_SHFT                                   0x0

#define HWIO_HS_RX_TESTER_OUT_2_ADDR(x)                                     ((x) + 0x000000e4)
#define HWIO_HS_RX_TESTER_OUT_2_OFFS                                        (0x000000e4)
#define HWIO_HS_RX_TESTER_OUT_2_RMSK                                              0x3f
#define HWIO_HS_RX_TESTER_OUT_2_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_2_ADDR(x), HWIO_HS_RX_TESTER_OUT_2_RMSK)
#define HWIO_HS_RX_TESTER_OUT_2_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_2_ADDR(x), m)
#define HWIO_HS_RX_TESTER_OUT_2_PHY_RX_TESTER_STATUS_BMSK                         0x3c
#define HWIO_HS_RX_TESTER_OUT_2_PHY_RX_TESTER_STATUS_SHFT                          0x2
#define HWIO_HS_RX_TESTER_OUT_2_RX_ALL_BAD_BMSK                                    0x2
#define HWIO_HS_RX_TESTER_OUT_2_RX_ALL_BAD_SHFT                                    0x1
#define HWIO_HS_RX_TESTER_OUT_2_RX_ALL_GOOD_BMSK                                   0x1
#define HWIO_HS_RX_TESTER_OUT_2_RX_ALL_GOOD_SHFT                                   0x0

#define HWIO_HS_RX_TESTER_OUT_3_ADDR(x)                                     ((x) + 0x000000e8)
#define HWIO_HS_RX_TESTER_OUT_3_OFFS                                        (0x000000e8)
#define HWIO_HS_RX_TESTER_OUT_3_RMSK                                              0x3f
#define HWIO_HS_RX_TESTER_OUT_3_IN(x)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_3_ADDR(x), HWIO_HS_RX_TESTER_OUT_3_RMSK)
#define HWIO_HS_RX_TESTER_OUT_3_INM(x, m)      \
        in_dword_masked(HWIO_HS_RX_TESTER_OUT_3_ADDR(x), m)
#define HWIO_HS_RX_TESTER_OUT_3_PHY_RX_TESTER_STATUS_BMSK                         0x3c
#define HWIO_HS_RX_TESTER_OUT_3_PHY_RX_TESTER_STATUS_SHFT                          0x2
#define HWIO_HS_RX_TESTER_OUT_3_RX_ALL_BAD_BMSK                                    0x2
#define HWIO_HS_RX_TESTER_OUT_3_RX_ALL_BAD_SHFT                                    0x1
#define HWIO_HS_RX_TESTER_OUT_3_RX_ALL_GOOD_BMSK                                   0x1
#define HWIO_HS_RX_TESTER_OUT_3_RX_ALL_GOOD_SHFT                                   0x0

#define HWIO_HS_PHY_CTRL_COMMON_ADDR(x)                                     ((x) + 0x000000ec)
#define HWIO_HS_PHY_CTRL_COMMON_OFFS                                        (0x000000ec)
#define HWIO_HS_PHY_CTRL_COMMON_RMSK                                          0xffffff
#define HWIO_HS_PHY_CTRL_COMMON_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_COMMON_ADDR(x), HWIO_HS_PHY_CTRL_COMMON_RMSK)
#define HWIO_HS_PHY_CTRL_COMMON_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_COMMON_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_COMMON_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_COMMON_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_COMMON_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_COMMON_ADDR(x),m,v,HWIO_HS_PHY_CTRL_COMMON_IN(x))
#define HWIO_HS_PHY_CTRL_COMMON_CLK_REF_DIV2_BMSK                             0x800000
#define HWIO_HS_PHY_CTRL_COMMON_CLK_REF_DIV2_SHFT                                 0x17
#define HWIO_HS_PHY_CTRL_COMMON_PLLPTUNE_BMSK                                 0x780000
#define HWIO_HS_PHY_CTRL_COMMON_PLLPTUNE_SHFT                                     0x13
#define HWIO_HS_PHY_CTRL_COMMON_PLLITUNE_BMSK                                  0x60000
#define HWIO_HS_PHY_CTRL_COMMON_PLLITUNE_SHFT                                     0x11
#define HWIO_HS_PHY_CTRL_COMMON_VREGBYPASS_BMSK                                0x10000
#define HWIO_HS_PHY_CTRL_COMMON_VREGBYPASS_SHFT                                   0x10
#define HWIO_HS_PHY_CTRL_COMMON_PLLBTUNE_BMSK                                   0x8000
#define HWIO_HS_PHY_CTRL_COMMON_PLLBTUNE_SHFT                                      0xf
#define HWIO_HS_PHY_CTRL_COMMON_USE_CLKCORE_BMSK                                0x4000
#define HWIO_HS_PHY_CTRL_COMMON_USE_CLKCORE_SHFT                                   0xe
#define HWIO_HS_PHY_CTRL_COMMON_ACAENB0_BMSK                                    0x2000
#define HWIO_HS_PHY_CTRL_COMMON_ACAENB0_SHFT                                       0xd
#define HWIO_HS_PHY_CTRL_COMMON_VBUSVLDEXTSEL0_BMSK                             0x1000
#define HWIO_HS_PHY_CTRL_COMMON_VBUSVLDEXTSEL0_SHFT                                0xc
#define HWIO_HS_PHY_CTRL_COMMON_OTGDISABLE0_BMSK                                 0x800
#define HWIO_HS_PHY_CTRL_COMMON_OTGDISABLE0_SHFT                                   0xb
#define HWIO_HS_PHY_CTRL_COMMON_OTGTUNE0_BMSK                                    0x700
#define HWIO_HS_PHY_CTRL_COMMON_OTGTUNE0_SHFT                                      0x8
#define HWIO_HS_PHY_CTRL_COMMON_COMMONONN_BMSK                                    0x80
#define HWIO_HS_PHY_CTRL_COMMON_COMMONONN_SHFT                                     0x7
#define HWIO_HS_PHY_CTRL_COMMON_FSEL_BMSK                                         0x70
#define HWIO_HS_PHY_CTRL_COMMON_FSEL_SHFT                                          0x4
#define HWIO_HS_PHY_CTRL_COMMON_RETENABLEN_BMSK                                    0x8
#define HWIO_HS_PHY_CTRL_COMMON_RETENABLEN_SHFT                                    0x3
#define HWIO_HS_PHY_CTRL_COMMON_SIDDQ_BMSK                                         0x4
#define HWIO_HS_PHY_CTRL_COMMON_SIDDQ_SHFT                                         0x2
#define HWIO_HS_PHY_CTRL_COMMON_VATESTENB_BMSK                                     0x3
#define HWIO_HS_PHY_CTRL_COMMON_VATESTENB_SHFT                                     0x0

#define HWIO_SS_QMP_PHY_CTRL_ADDR(x)                                        ((x) + 0x000000f0)
#define HWIO_SS_QMP_PHY_CTRL_OFFS                                           (0x000000f0)
#define HWIO_SS_QMP_PHY_CTRL_RMSK                                                  0x1
#define HWIO_SS_QMP_PHY_CTRL_IN(x)      \
        in_dword_masked(HWIO_SS_QMP_PHY_CTRL_ADDR(x), HWIO_SS_QMP_PHY_CTRL_RMSK)
#define HWIO_SS_QMP_PHY_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_SS_QMP_PHY_CTRL_ADDR(x), m)
#define HWIO_SS_QMP_PHY_CTRL_OUT(x, v)      \
        out_dword(HWIO_SS_QMP_PHY_CTRL_ADDR(x),v)
#define HWIO_SS_QMP_PHY_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_QMP_PHY_CTRL_ADDR(x),m,v,HWIO_SS_QMP_PHY_CTRL_IN(x))
#define HWIO_SS_QMP_PHY_CTRL_USB3QMP_PHY_RESET_EN_BMSK                             0x1
#define HWIO_SS_QMP_PHY_CTRL_USB3QMP_PHY_RESET_EN_SHFT                             0x0

#define HWIO_SNPS_CORE_CFG_ADDR(x)                                          ((x) + 0x000000f4)
#define HWIO_SNPS_CORE_CFG_OFFS                                             (0x000000f4)
#define HWIO_SNPS_CORE_CFG_RMSK                                                    0x1
#define HWIO_SNPS_CORE_CFG_IN(x)      \
        in_dword_masked(HWIO_SNPS_CORE_CFG_ADDR(x), HWIO_SNPS_CORE_CFG_RMSK)
#define HWIO_SNPS_CORE_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SNPS_CORE_CFG_ADDR(x), m)
#define HWIO_SNPS_CORE_CFG_OUT(x, v)      \
        out_dword(HWIO_SNPS_CORE_CFG_ADDR(x),v)
#define HWIO_SNPS_CORE_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SNPS_CORE_CFG_ADDR(x),m,v,HWIO_SNPS_CORE_CFG_IN(x))
#define HWIO_SNPS_CORE_CFG_SNPS_CORE_PME_EN_BMSK                                   0x1
#define HWIO_SNPS_CORE_CFG_SNPS_CORE_PME_EN_SHFT                                   0x0


#endif /* __HALDWCHWIO_H__ */
