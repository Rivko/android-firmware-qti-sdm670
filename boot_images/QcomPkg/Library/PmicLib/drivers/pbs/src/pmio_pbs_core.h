#ifndef __PMIO_PBS_CORE_H__
#define __PMIO_PBS_CORE_H__
/*
===========================================================================
*/
/**
  @file pmio_pbs_core.h
  @brief Auto-generated PMIO interface include file.

  This file contains PMIO register definitions for the following modules:
    PBS_CORE

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================


  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * PERIPHERAL: PBS_CORE
 *--------------------------------------------------------------------------*/

#define PBS_CORE_BASE                                                      0x00007000

#define PMIO_PBS_CORE_REVISION1_ADDR                                       (PBS_CORE_BASE      + 0x00000000)
#define PMIO_PBS_CORE_REVISION1_OFFS                                       0x00000000
#define PMIO_PBS_CORE_REVISION1_RMSK                                             0xff
#define PMIO_PBS_CORE_REVISION1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION1_ADDR, PMIO_PBS_CORE_REVISION1_RMSK, 0, val)
#define PMIO_PBS_CORE_REVISION1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_REVISION1_DIG_MINOR_BMSK                                   0xff
#define PMIO_PBS_CORE_REVISION1_DIG_MINOR_SHFT                                    0x0

#define PMIO_PBS_CORE_REVISION2_ADDR                                       (PBS_CORE_BASE      + 0x00000001)
#define PMIO_PBS_CORE_REVISION2_OFFS                                       0x00000001
#define PMIO_PBS_CORE_REVISION2_RMSK                                             0xff
#define PMIO_PBS_CORE_REVISION2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION2_ADDR, PMIO_PBS_CORE_REVISION2_RMSK, 0, val)
#define PMIO_PBS_CORE_REVISION2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_REVISION2_DIG_MAJOR_BMSK                                   0xff
#define PMIO_PBS_CORE_REVISION2_DIG_MAJOR_SHFT                                    0x0

#define PMIO_PBS_CORE_REVISION3_ADDR                                       (PBS_CORE_BASE      + 0x00000002)
#define PMIO_PBS_CORE_REVISION3_OFFS                                       0x00000002
#define PMIO_PBS_CORE_REVISION3_RMSK                                             0xff
#define PMIO_PBS_CORE_REVISION3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION3_ADDR, PMIO_PBS_CORE_REVISION3_RMSK, 0, val)
#define PMIO_PBS_CORE_REVISION3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_REVISION3_ANA_MINOR_BMSK                                   0xff
#define PMIO_PBS_CORE_REVISION3_ANA_MINOR_SHFT                                    0x0

#define PMIO_PBS_CORE_REVISION4_ADDR                                       (PBS_CORE_BASE      + 0x00000003)
#define PMIO_PBS_CORE_REVISION4_OFFS                                       0x00000003
#define PMIO_PBS_CORE_REVISION4_RMSK                                             0xff
#define PMIO_PBS_CORE_REVISION4_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION4_ADDR, PMIO_PBS_CORE_REVISION4_RMSK, 0, val)
#define PMIO_PBS_CORE_REVISION4_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_REVISION4_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_REVISION4_ANA_MAJOR_BMSK                                   0xff
#define PMIO_PBS_CORE_REVISION4_ANA_MAJOR_SHFT                                    0x0

#define PMIO_PBS_CORE_PERPH_TYPE_ADDR                                      (PBS_CORE_BASE      + 0x00000004)
#define PMIO_PBS_CORE_PERPH_TYPE_OFFS                                      0x00000004
#define PMIO_PBS_CORE_PERPH_TYPE_RMSK                                            0xff
#define PMIO_PBS_CORE_PERPH_TYPE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_TYPE_ADDR, PMIO_PBS_CORE_PERPH_TYPE_RMSK, 0, val)
#define PMIO_PBS_CORE_PERPH_TYPE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_TYPE_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_TYPE_TYPE_BMSK                                       0xff
#define PMIO_PBS_CORE_PERPH_TYPE_TYPE_SHFT                                        0x0

#define PMIO_PBS_CORE_PERPH_SUBTYPE_ADDR                                   (PBS_CORE_BASE      + 0x00000005)
#define PMIO_PBS_CORE_PERPH_SUBTYPE_OFFS                                   0x00000005
#define PMIO_PBS_CORE_PERPH_SUBTYPE_RMSK                                         0xff
#define PMIO_PBS_CORE_PERPH_SUBTYPE_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_SUBTYPE_ADDR, PMIO_PBS_CORE_PERPH_SUBTYPE_RMSK, 0, val)
#define PMIO_PBS_CORE_PERPH_SUBTYPE_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_SUBTYPE_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_SUBTYPE_SUBTYPE_BMSK                                 0xff
#define PMIO_PBS_CORE_PERPH_SUBTYPE_SUBTYPE_SHFT                                  0x0

#define PMIO_PBS_CORE_SEQ_STATUS0_ADDR                                     (PBS_CORE_BASE      + 0x00000008)
#define PMIO_PBS_CORE_SEQ_STATUS0_OFFS                                     0x00000008
#define PMIO_PBS_CORE_SEQ_STATUS0_RMSK                                           0x3f
#define PMIO_PBS_CORE_SEQ_STATUS0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS0_ADDR, PMIO_PBS_CORE_SEQ_STATUS0_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_STATUS0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_STATUS0_LAST_SEQ_COMP_STATUS_BMSK                      0x30
#define PMIO_PBS_CORE_SEQ_STATUS0_LAST_SEQ_COMP_STATUS_SHFT                       0x4
#define PMIO_PBS_CORE_SEQ_STATUS0_LAST_SEQ_COMP_BMSK                              0xf
#define PMIO_PBS_CORE_SEQ_STATUS0_LAST_SEQ_COMP_SHFT                              0x0

#define PMIO_PBS_CORE_SEQ_STATUS3_ADDR                                     (PBS_CORE_BASE      + 0x00000009)
#define PMIO_PBS_CORE_SEQ_STATUS3_OFFS                                     0x00000009
#define PMIO_PBS_CORE_SEQ_STATUS3_RMSK                                            0x1
#define PMIO_PBS_CORE_SEQ_STATUS3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS3_ADDR, PMIO_PBS_CORE_SEQ_STATUS3_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_STATUS3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_STATUS3_PBS_MODE_STATUS_BMSK                            0x1
#define PMIO_PBS_CORE_SEQ_STATUS3_PBS_MODE_STATUS_SHFT                            0x0

#define PMIO_PBS_CORE_SEQ_PC_STATUS0_ADDR                                  (PBS_CORE_BASE      + 0x0000000a)
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_OFFS                                  0x0000000a
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_PC_STATUS0_ADDR, PMIO_PBS_CORE_SEQ_PC_STATUS0_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_PC_STATUS0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_PC_LSB_BMSK                                 0xff
#define PMIO_PBS_CORE_SEQ_PC_STATUS0_PC_LSB_SHFT                                  0x0

#define PMIO_PBS_CORE_SEQ_PC_STATUS1_ADDR                                  (PBS_CORE_BASE      + 0x0000000b)
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_OFFS                                  0x0000000b
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_PC_STATUS1_ADDR, PMIO_PBS_CORE_SEQ_PC_STATUS1_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_PC_STATUS1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_PC_MSB_BMSK                                 0xff
#define PMIO_PBS_CORE_SEQ_PC_STATUS1_PC_MSB_SHFT                                  0x0

#define PMIO_PBS_CORE_SEQ_IR_STATUS0_ADDR                                  (PBS_CORE_BASE      + 0x0000000c)
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_OFFS                                  0x0000000c
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS0_ADDR, PMIO_PBS_CORE_SEQ_IR_STATUS0_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_IR_BYTE0_BMSK                               0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS0_IR_BYTE0_SHFT                                0x0

#define PMIO_PBS_CORE_SEQ_IR_STATUS1_ADDR                                  (PBS_CORE_BASE      + 0x0000000d)
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_OFFS                                  0x0000000d
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS1_ADDR, PMIO_PBS_CORE_SEQ_IR_STATUS1_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_IR_BYTE1_BMSK                               0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS1_IR_BYTE1_SHFT                                0x0

#define PMIO_PBS_CORE_SEQ_IR_STATUS2_ADDR                                  (PBS_CORE_BASE      + 0x0000000e)
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_OFFS                                  0x0000000e
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS2_ADDR, PMIO_PBS_CORE_SEQ_IR_STATUS2_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_IR_BYTE2_BMSK                               0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS2_IR_BYTE2_SHFT                                0x0

#define PMIO_PBS_CORE_SEQ_IR_STATUS3_ADDR                                  (PBS_CORE_BASE      + 0x0000000f)
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_OFFS                                  0x0000000f
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_RMSK                                        0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS3_ADDR, PMIO_PBS_CORE_SEQ_IR_STATUS3_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_IR_STATUS3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_IR_BYTE3_BMSK                               0xff
#define PMIO_PBS_CORE_SEQ_IR_STATUS3_IR_BYTE3_SHFT                                0x0

#define PMIO_PBS_CORE_MEM_INTF_CFG_ADDR                                    (PBS_CORE_BASE      + 0x00000040)
#define PMIO_PBS_CORE_MEM_INTF_CFG_OFFS                                    0x00000040
#define PMIO_PBS_CORE_MEM_INTF_CFG_RMSK                                          0x80
#define PMIO_PBS_CORE_MEM_INTF_CFG_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, PMIO_PBS_CORE_MEM_INTF_CFG_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_CFG_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_CFG_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_CFG_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_CFG_RIF_MEM_ACCESS_EN_BMSK                        0x80
#define PMIO_PBS_CORE_MEM_INTF_CFG_RIF_MEM_ACCESS_EN_SHFT                         0x7

#define PMIO_PBS_CORE_MEM_INTF_CTL_ADDR                                    (PBS_CORE_BASE      + 0x00000041)
#define PMIO_PBS_CORE_MEM_INTF_CTL_OFFS                                    0x00000041
#define PMIO_PBS_CORE_MEM_INTF_CTL_RMSK                                          0xc0
#define PMIO_PBS_CORE_MEM_INTF_CTL_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, PMIO_PBS_CORE_MEM_INTF_CTL_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_CTL_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_CTL_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_CTL_BURST_BMSK                                    0x80
#define PMIO_PBS_CORE_MEM_INTF_CTL_BURST_SHFT                                     0x7
#define PMIO_PBS_CORE_MEM_INTF_CTL_WR_EN_BMSK                                    0x40
#define PMIO_PBS_CORE_MEM_INTF_CTL_WR_EN_SHFT                                     0x6

#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR                               (PBS_CORE_BASE      + 0x00000042)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_OFFS                               0x00000042
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_MEM_INTF_ADDR_LSB_BMSK                   0xff
#define PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_MEM_INTF_ADDR_LSB_SHFT                    0x0

#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_ADDR                               (PBS_CORE_BASE      + 0x00000043)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_OFFS                               0x00000043
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_ADDR, PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_MEM_INTF_ADDR_MSB_BMSK                   0xff
#define PMIO_PBS_CORE_MEM_INTF_ADDR_MSB_MEM_INTF_ADDR_MSB_SHFT                    0x0

#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR                               (PBS_CORE_BASE      + 0x00000048)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_OFFS                               0x00000048
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_MEM_INTF_WR_DATA_0_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA0_MEM_INTF_WR_DATA_0_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_ADDR                               (PBS_CORE_BASE      + 0x00000049)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_OFFS                               0x00000049
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA1_ADDR, PMIO_PBS_CORE_MEM_INTF_WR_DATA1_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA1_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_MEM_INTF_WR_DATA_1_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA1_MEM_INTF_WR_DATA_1_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_ADDR                               (PBS_CORE_BASE      + 0x0000004a)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_OFFS                               0x0000004a
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA2_ADDR, PMIO_PBS_CORE_MEM_INTF_WR_DATA2_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA2_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_MEM_INTF_WR_DATA_2_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA2_MEM_INTF_WR_DATA_2_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_ADDR                               (PBS_CORE_BASE      + 0x0000004b)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_OFFS                               0x0000004b
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA3_ADDR, PMIO_PBS_CORE_MEM_INTF_WR_DATA3_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA3_ADDR, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_WR_DATA3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_MEM_INTF_WR_DATA_3_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_WR_DATA3_MEM_INTF_WR_DATA_3_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR                               (PBS_CORE_BASE      + 0x0000004c)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_OFFS                               0x0000004c
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_MEM_INTF_RD_DATA_0_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA0_MEM_INTF_RD_DATA_0_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_ADDR                               (PBS_CORE_BASE      + 0x0000004d)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_OFFS                               0x0000004d
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA1_ADDR, PMIO_PBS_CORE_MEM_INTF_RD_DATA1_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_MEM_INTF_RD_DATA_1_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA1_MEM_INTF_RD_DATA_1_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_ADDR                               (PBS_CORE_BASE      + 0x0000004e)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_OFFS                               0x0000004e
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA2_ADDR, PMIO_PBS_CORE_MEM_INTF_RD_DATA2_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_MEM_INTF_RD_DATA_2_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA2_MEM_INTF_RD_DATA_2_SHFT                   0x0

#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_ADDR                               (PBS_CORE_BASE      + 0x0000004f)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_OFFS                               0x0000004f
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_RMSK                                     0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA3_ADDR, PMIO_PBS_CORE_MEM_INTF_RD_DATA3_RMSK, 0, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_MEM_INTF_RD_DATA3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_MEM_INTF_RD_DATA_3_BMSK                  0xff
#define PMIO_PBS_CORE_MEM_INTF_RD_DATA3_MEM_INTF_RD_DATA_3_SHFT                   0x0

#define PMIO_PBS_CORE_SEQ_STATUS1_ADDR                                     (PBS_CORE_BASE      + 0x00000090)
#define PMIO_PBS_CORE_SEQ_STATUS1_OFFS                                     0x00000090
#define PMIO_PBS_CORE_SEQ_STATUS1_RMSK                                           0xff
#define PMIO_PBS_CORE_SEQ_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS1_ADDR, PMIO_PBS_CORE_SEQ_STATUS1_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_STATUS1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_STATUS1_SEQ_IN_SERVICE_BMSK                            0xf0
#define PMIO_PBS_CORE_SEQ_STATUS1_SEQ_IN_SERVICE_SHFT                             0x4
#define PMIO_PBS_CORE_SEQ_STATUS1_LAST_SEQ_NORMAL_BMSK                            0xf
#define PMIO_PBS_CORE_SEQ_STATUS1_LAST_SEQ_NORMAL_SHFT                            0x0

#define PMIO_PBS_CORE_SEQ_STATUS2_ADDR                                     (PBS_CORE_BASE      + 0x00000091)
#define PMIO_PBS_CORE_SEQ_STATUS2_OFFS                                     0x00000091
#define PMIO_PBS_CORE_SEQ_STATUS2_RMSK                                           0xff
#define PMIO_PBS_CORE_SEQ_STATUS2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS2_ADDR, PMIO_PBS_CORE_SEQ_STATUS2_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_STATUS2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_STATUS2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_STATUS2_LAST_SEQ_ABORTED_BMSK                          0xf0
#define PMIO_PBS_CORE_SEQ_STATUS2_LAST_SEQ_ABORTED_SHFT                           0x4
#define PMIO_PBS_CORE_SEQ_STATUS2_LAST_SEQ_ERROR_BMSK                             0xf
#define PMIO_PBS_CORE_SEQ_STATUS2_LAST_SEQ_ERROR_SHFT                             0x0

#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_ADDR                              (PBS_CORE_BASE      + 0x00000092)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_OFFS                              0x00000092
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_ADDR, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_ERR_PC_LSB_BMSK                         0xff
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS0_ERR_PC_LSB_SHFT                          0x0

#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_ADDR                              (PBS_CORE_BASE      + 0x00000093)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_OFFS                              0x00000093
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_ADDR, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_ERR_PC_MSB_BMSK                         0xff
#define PMIO_PBS_CORE_SEQ_ERR_PC_STATUS1_ERR_PC_MSB_SHFT                          0x0

#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_ADDR                              (PBS_CORE_BASE      + 0x00000094)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_OFFS                              0x00000094
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_ADDR, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_ERR_IR_BYTE0_BMSK                       0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS0_ERR_IR_BYTE0_SHFT                        0x0

#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_ADDR                              (PBS_CORE_BASE      + 0x00000095)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_OFFS                              0x00000095
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_ADDR, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_ERR_IR_BYTE1_BMSK                       0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS1_ERR_IR_BYTE1_SHFT                        0x0

#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_ADDR                              (PBS_CORE_BASE      + 0x00000096)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_OFFS                              0x00000096
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_ADDR, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_ERR_IR_BYTE2_BMSK                       0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS2_ERR_IR_BYTE2_SHFT                        0x0

#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_ADDR                              (PBS_CORE_BASE      + 0x00000097)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_OFFS                              0x00000097
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_RMSK                                    0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_ADDR, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_RMSK, 0, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_ERR_IR_BYTE3_BMSK                       0xff
#define PMIO_PBS_CORE_SEQ_ERR_IR_STATUS3_ERR_IR_BYTE3_SHFT                        0x0

#define PMIO_PBS_CORE_SEC_ACCESS_ADDR                                      (PBS_CORE_BASE      + 0x000000d0)
#define PMIO_PBS_CORE_SEC_ACCESS_OFFS                                      0x000000d0
#define PMIO_PBS_CORE_SEC_ACCESS_RMSK                                            0xff
#define PMIO_PBS_CORE_SEC_ACCESS_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEC_ACCESS_ADDR, PMIO_PBS_CORE_SEC_ACCESS_RMSK, 0, val)
#define PMIO_PBS_CORE_SEC_ACCESS_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEC_ACCESS_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEC_ACCESS_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_SEC_ACCESS_ADDR, val)
#define PMIO_PBS_CORE_SEC_ACCESS_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_SEC_ACCESS_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_SEC_ACCESS_SEC_UNLOCK_BMSK                                 0xff
#define PMIO_PBS_CORE_SEC_ACCESS_SEC_UNLOCK_SHFT                                  0x0

#define PMIO_PBS_CORE_PERPH_RESET_CTL2_ADDR                                (PBS_CORE_BASE      + 0x000000d9)
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_OFFS                                0x000000d9
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_RMSK                                       0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL2_ADDR, PMIO_PBS_CORE_PERPH_RESET_CTL2_RMSK, 0, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL2_ADDR, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_BMSK                 0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_SHFT                 0x0

#define PMIO_PBS_CORE_PERPH_RESET_CTL3_ADDR                                (PBS_CORE_BASE      + 0x000000da)
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_OFFS                                0x000000da
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_RMSK                                       0xf
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL3_ADDR, PMIO_PBS_CORE_PERPH_RESET_CTL3_RMSK, 0, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL3_ADDR, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL3_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_BMSK                       0x8
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_SHFT                       0x3
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_WARM_RB_BMSK                        0x4
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_WARM_RB_SHFT                        0x2
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_BMSK                   0x2
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_SHFT                   0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_BMSK                   0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_SHFT                   0x0

#define PMIO_PBS_CORE_PERPH_RESET_CTL4_ADDR                                (PBS_CORE_BASE      + 0x000000db)
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_OFFS                                0x000000db
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_RMSK                                       0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_IN(ctxt, pmic, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL4_ADDR, PMIO_PBS_CORE_PERPH_RESET_CTL4_RMSK, 0, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_INF(ctxt, pmic, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL4_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL4_ADDR, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_OUTF(ctxt, pmic, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PBS_CORE_PERPH_RESET_CTL4_ADDR, mask, shift, val)
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_BMSK                      0x1
#define PMIO_PBS_CORE_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_SHFT                      0x0


#endif /* __PMIO_PBS_CORE_H__ */