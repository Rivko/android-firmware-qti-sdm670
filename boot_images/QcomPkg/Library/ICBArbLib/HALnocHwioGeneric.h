#ifndef __HALNOCHWIOGENERIC_H__
#define __HALNOCHWIOGENERIC_H__
/*
===========================================================================
*/
/**
  @file HALnocHwioGeneric.h
  @brief Generic hwio for the QNoC4 QOS block.

*/
/*
  ===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/HALnocHwioGeneric.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: NOC_QOS (generic)
 *--------------------------------------------------------------------------*/
#define NOC_QOS_REG_BASE(b,o)                              ((b)      + (o))

/*------------------------------ NOC_QOS_<ID>_SWID_LOWn -----------------------------*/
#define NOC_QOS_SWID_LOW_ADDR(b,o,s,n)                                                      (NOC_QOS_REG_BASE(b,o) + (s) * (n))
#define NOC_QOS_SWID_LOW_RMSK                                                               0xffffff
#define NOC_QOS_SWID_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_SWID_LOW_ADDR(b,o,s,n), NOC_QOS_SWID_LOW_RMSK)
#define NOC_QOS_SWID_LOW_INMI(b,o,s,n,m) \
        in_dword_masked(NOC_QOS_SWID_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_SWID_LOW_UNITTYPEID_BMSK                                                    0xff0000
#define NOC_QOS_SWID_LOW_UNITTYPEID_SHFT                                                        0x10
#define NOC_QOS_SWID_LOW_UNITCONFID_BMSK                                                      0xffff
#define NOC_QOS_SWID_LOW_UNITCONFID_SHFT                                                         0x0

/*------------------------------ NOC_QOS_<ID>_SWID_HIGHn -----------------------------*/
#define NOC_QOS_SWID_HIGH_ADDR(b,o,s,n)                                                   (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x4)
#define NOC_QOS_SWID_HIGH_RMSK                                                                0xffff
#define NOC_QOS_SWID_HIGH_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_SWID_HIGH_ADDR(b,o,s,n), NOC_QOS_SWID_HIGH_RMSK)
#define NOC_QOS_SWID_HIGH_INMI(b,o,s,n,m) \
        in_dword_masked(NOC_QOS_SWID_HIGH_ADDR(b,o,s,n), m)
#define NOC_QOS_SWID_HIGH_QNOCID_BMSK                                                         0xffff
#define NOC_QOS_SWID_HIGH_QNOCID_SHFT                                                            0x0

/*------------------------------ NOC_QOS_<ID>_MAINCTL_LOWn -----------------------------*/
#define NOC_QOS_MAINCTL_LOW_ADDR(b,o,s,n)                                                 (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x8)
#define NOC_QOS_MAINCTL_LOW_RMSK                                                            0x3fffff
#define NOC_QOS_MAINCTL_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_MAINCTL_LOW_ADDR(b,o,s,n), NOC_QOS_MAINCTL_LOW_RMSK)
#define NOC_QOS_MAINCTL_LOW_INMI(b,o,s,n,m) \
        in_dword_masked(NOC_QOS_MAINCTL_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_MAINCTL_LOW_OUTI(b,o,s,n,v) \
        out_dword(NOC_QOS_MAINCTL_LOW_ADDR(b,o,s,n),v)
#define NOC_QOS_MAINCTL_LOW_OUTMI(b,o,s,n,m,v) \
        out_dword_masked_ns(NOC_QOS_MAINCTL_LOW_ADDR(b,o,s,n),m,v,NOC_QOS_MAINCTL_LOW_INI(b,o,s,n))
#define NOC_QOS_MAINCTL_LOW_SAFESHAPING_BMSK                                                0x3f0000
#define NOC_QOS_MAINCTL_LOW_SAFESHAPING_SHFT                                                    0x10
#define NOC_QOS_MAINCTL_LOW_URGDELAY_BMSK                                                     0x3f00
#define NOC_QOS_MAINCTL_LOW_URGDELAY_SHFT                                                        0x8
#define NOC_QOS_MAINCTL_LOW_DFLTPRIORITY_BMSK                                                   0x70
#define NOC_QOS_MAINCTL_LOW_DFLTPRIORITY_SHFT                                                    0x4
#define NOC_QOS_MAINCTL_LOW_SLVURGMSGEN_BMSK                                                     0x8
#define NOC_QOS_MAINCTL_LOW_SLVURGMSGEN_SHFT                                                     0x3
#define NOC_QOS_MAINCTL_LOW_STOP_BMSK                                                            0x4
#define NOC_QOS_MAINCTL_LOW_STOP_SHFT                                                            0x2
#define NOC_QOS_MAINCTL_LOW_SHAPEREN_BMSK                                                        0x2
#define NOC_QOS_MAINCTL_LOW_SHAPEREN_SHFT                                                        0x1
#define NOC_QOS_MAINCTL_LOW_BWLIMITEN_BMSK                                                       0x1
#define NOC_QOS_MAINCTL_LOW_BWLIMITEN_SHFT                                                       0x0

/*------------------------------ NOC_QOS_<ID>_MAINSTATUS_LOWn -----------------------------*/
#define NOC_QOS_MAINSTATUS_LOW_ADDR(b,o,s,n)                                              (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x10)
#define NOC_QOS_MAINSTATUS_LOW_RMSK                                                        0xfffffff
#define NOC_QOS_MAINSTATUS_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_MAINSTATUS_LOW_ADDR(b,o,s,n), NOC_QOS_MAINSTATUS_LOW_RMSK)
#define NOC_QOS_MAINSTATUS_LOW_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_MAINSTATUS_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_MAINSTATUS_LOW_NOMINALFREQ_BMSK                                            0xfff0000
#define NOC_QOS_MAINSTATUS_LOW_NOMINALFREQ_SHFT                                                 0x10
#define NOC_QOS_MAINSTATUS_LOW_PENDING_BMSK                                                     0x7f
#define NOC_QOS_MAINSTATUS_LOW_PENDING_SHFT                                                      0x0

/*------------------------------ NOC_QOS_<ID>_LIMITBW_LOWn -----------------------------*/
#define NOC_QOS_LIMITBW_LOW_ADDR(b,o,s,n)                                                 (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x18)
#define NOC_QOS_LIMITBW_LOW_RMSK                                                           0x3ffffff
#define NOC_QOS_LIMITBW_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_LIMITBW_LOW_ADDR(b,o,s,n), NOC_QOS_LIMITBW_LOW_RMSK)
#define NOC_QOS_LIMITBW_LOW_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_LIMITBW_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_LIMITBW_LOW_OUTI(b,o,s,n,v)\
        out_dword(NOC_QOS_LIMITBW_LOW_ADDR(b,o,s,n),v)
#define NOC_QOS_LIMITBW_LOW_OUTMI(b,o,s,n,m,v)\
        out_dword_masked_ns(NOC_QOS_LIMITBW_LOW_ADDR(b,o,s,n),m,v,NOC_QOS_LIMITBW_LOW_INI(b,o,s,n))
#define NOC_QOS_LIMITBW_LOW_SATURATION_BMSK                                                0x3ff0000
#define NOC_QOS_LIMITBW_LOW_SATURATION_SHFT                                                     0x10
#define NOC_QOS_LIMITBW_LOW_BANDWIDTH_BMSK                                                    0x1fff
#define NOC_QOS_LIMITBW_LOW_BANDWIDTH_SHFT                                                       0x0

/*------------------------------ NOC_QOS_<ID>_SHAPING_LOWn -----------------------------*/
#define NOC_QOS_SHAPING_LOW_ADDR(b,o,s,n)                                                 (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x20)
#define NOC_QOS_SHAPING_LOW_RMSK                                                          0xffffffff
#define NOC_QOS_SHAPING_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_SHAPING_LOW_ADDR(b,o,s,n), NOC_QOS_SHAPING_LOW_RMSK)
#define NOC_QOS_SHAPING_LOW_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_SHAPING_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_SHAPING_LOW_OUTI(b,o,s,n,v)\
        out_dword(NOC_QOS_SHAPING_LOW_ADDR(b,o,s,n),v)
#define NOC_QOS_SHAPING_LOW_OUTMI(b,o,s,n,m,v)\
        out_dword_masked_ns(NOC_QOS_SHAPING_LOW_ADDR(b,o,s,n),m,v,NOC_QOS_SHAPING_LOW_INI(b,o,s,n))
#define NOC_QOS_SHAPING_LOW_LVL3_BMSK                                                     0x3f000000
#define NOC_QOS_SHAPING_LOW_LVL3_SHFT                                                           0x18
#define NOC_QOS_SHAPING_LOW_LVL2_BMSK                                                       0x3f0000
#define NOC_QOS_SHAPING_LOW_LVL2_SHFT                                                           0x10
#define NOC_QOS_SHAPING_LOW_LVL1_BMSK                                                         0x3f00
#define NOC_QOS_SHAPING_LOW_LVL1_SHFT                                                            0x8
#define NOC_QOS_SHAPING_LOW_LVL0_BMSK                                                           0x3f
#define NOC_QOS_SHAPING_LOW_LVL0_SHFT                                                            0x0

/*------------------------------ NOC_QOS_<ID>_SHAPING_HIGHn -----------------------------*/
#define NOC_QOS_SHAPING_HIGH_ADDR(b,o,s,n)                                                (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x24)
#define NOC_QOS_SHAPING_HIGH_RMSK                                                         0xffffffff
#define NOC_QOS_SHAPING_HIGH_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_SHAPING_HIGH_ADDR(b,o,s,n), NOC_QOS_SHAPING_HIGH_RMSK)
#define NOC_QOS_SHAPING_HIGH_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_SHAPING_HIGH_ADDR(b,o,s,n), m)
#define NOC_QOS_SHAPING_HIGH_OUTI(b,o,s,n,v)\
        out_dword(NOC_QOS_SHAPING_HIGH_ADDR(b,o,s,n),v)
#define NOC_QOS_SHAPING_HIGH_OUTMI(b,o,s,n,m,v)\
        out_dword_masked_ns(NOC_QOS_SHAPING_HIGH_ADDR(b,o,s,n),m,v,NOC_QOS_SHAPING_HIGH_INI(b,o,s,n))
#define NOC_QOS_SHAPING_HIGH_LVL7_BMSK                                                    0x3f000000
#define NOC_QOS_SHAPING_HIGH_LVL7_SHFT                                                          0x18
#define NOC_QOS_SHAPING_HIGH_LVL6_BMSK                                                      0x3f0000
#define NOC_QOS_SHAPING_HIGH_LVL6_SHFT                                                          0x10
#define NOC_QOS_SHAPING_HIGH_LVL5_BMSK                                                        0x3f00
#define NOC_QOS_SHAPING_HIGH_LVL5_SHFT                                                           0x8
#define NOC_QOS_SHAPING_HIGH_LVL4_BMSK                                                          0x3f
#define NOC_QOS_SHAPING_HIGH_LVL4_SHFT                                                           0x0

/*------------------------------ NOC_QOS_<ID>_REGUL0CTL_LOWn -----------------------------*/
#define NOC_QOS_REGUL0CTL_LOW_ADDR(b,o,s,n)                                               (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x40)
#define NOC_QOS_REGUL0CTL_LOW_RMSK                                                            0x7fff
#define NOC_QOS_REGUL0CTL_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_REGUL0CTL_LOW_ADDR(b,o,s,n), NOC_QOS_REGUL0CTL_LOW_RMSK)
#define NOC_QOS_REGUL0CTL_LOW_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_REGUL0CTL_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_REGUL0CTL_LOW_OUTI(b,o,s,n,v)\
        out_dword(NOC_QOS_REGUL0CTL_LOW_ADDR(b,o,s,n),v)
#define NOC_QOS_REGUL0CTL_LOW_OUTMI(b,o,s,n,m,v)\
        out_dword_masked_ns(NOC_QOS_REGUL0CTL_LOW_ADDR(b,o,s,n),m,v,NOC_QOS_REGUL0CTL_LOW_INI(b,o,s,n))
#define NOC_QOS_REGUL0CTL_LOW_HIGHPRIORITY_BMSK                                               0x7000
#define NOC_QOS_REGUL0CTL_LOW_HIGHPRIORITY_SHFT                                                  0xc
#define NOC_QOS_REGUL0CTL_LOW_LOWPRIORITY_BMSK                                                 0x700
#define NOC_QOS_REGUL0CTL_LOW_LOWPRIORITY_SHFT                                                   0x8
#define NOC_QOS_REGUL0CTL_LOW_WREN_BMSK                                                          0x2
#define NOC_QOS_REGUL0CTL_LOW_WREN_SHFT                                                          0x1
#define NOC_QOS_REGUL0CTL_LOW_RDEN_BMSK                                                          0x1
#define NOC_QOS_REGUL0CTL_LOW_RDEN_SHFT                                                          0x0

/*------------------------------ NOC_QOS_<ID>_REGUL0BW_LOWn -----------------------------*/
#define NOC_QOS_REGUL0BW_LOW_ADDR(b,o,s,n)                                                (NOC_QOS_REG_BASE(b,o) + (s) * (n) + 0x48)
#define NOC_QOS_REGUL0BW_LOW_RMSK                                                          0x3ffffff
#define NOC_QOS_REGUL0BW_LOW_INI(b,o,s,n) \
        in_dword_masked(NOC_QOS_REGUL0BW_LOW_ADDR(b,o,s,n), NOC_QOS_REGUL0BW_LOW_RMSK)
#define NOC_QOS_REGUL0BW_LOW_INMI(b,o,s,n,m)\
        in_dword_masked(NOC_QOS_REGUL0BW_LOW_ADDR(b,o,s,n), m)
#define NOC_QOS_REGUL0BW_LOW_OUTI(b,o,s,n,v)\
        out_dword(NOC_QOS_REGUL0BW_LOW_ADDR(b,o,s,n),v)
#define NOC_QOS_REGUL0BW_LOW_OUTMI(b,o,s,n,m,v)\
        out_dword_masked_ns(NOC_QOS_REGUL0BW_LOW_ADDR(b,o,s,n),m,v,NOC_QOS_REGUL0BW_LOW_INI(b,o,s,n))
#define NOC_QOS_REGUL0BW_LOW_SATURATION_BMSK                                               0x3ff0000
#define NOC_QOS_REGUL0BW_LOW_SATURATION_SHFT                                                    0x10
#define NOC_QOS_REGUL0BW_LOW_BANDWIDTH_BMSK                                                   0x1fff
#define NOC_QOS_REGUL0BW_LOW_BANDWIDTH_SHFT                                                      0x0

#endif /* __HALNOCHWIOGENERIC_H__ */
