#ifndef __PMIO_BUS_H__
#define __PMIO_BUS_H__
/*
===========================================================================
*/
/**
  @file pmio_bus.h
  @brief Auto-generated PMIO interface include file.

  This file contains PMIO register definitions for the following modules:
    BUS

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/pbs/src/pmio_bus.h#2 $
  $DateTime: 2019/03/08 09:58:25 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * PERIPHERAL: BUS
 *--------------------------------------------------------------------------*/

#define BUS_BASE                                                      0x00000400

#define PMIO_BUS_REVISION1_ADDR                                       (BUS_BASE      + 0x00000000)
#define PMIO_BUS_REVISION1_OFFS                                       0x00000000
#define PMIO_BUS_REVISION1_RMSK                                             0xff
#define PMIO_BUS_REVISION1_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_REVISION1_ADDR, PMIO_BUS_REVISION1_RMSK, val)
#define PMIO_BUS_REVISION1_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_REVISION1_ADDR, mask, shift, val)
#define PMIO_BUS_REVISION1_DIG_MINOR_BMSK                                   0xff
#define PMIO_BUS_REVISION1_DIG_MINOR_SHFT                                    0x0

#define PMIO_BUS_REVISION2_ADDR                                       (BUS_BASE      + 0x00000001)
#define PMIO_BUS_REVISION2_OFFS                                       0x00000001
#define PMIO_BUS_REVISION2_RMSK                                             0xff
#define PMIO_BUS_REVISION2_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_REVISION2_ADDR, PMIO_BUS_REVISION2_RMSK, val)
#define PMIO_BUS_REVISION2_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_REVISION2_ADDR, mask, shift, val)
#define PMIO_BUS_REVISION2_DIG_MAJOR_BMSK                                   0xff
#define PMIO_BUS_REVISION2_DIG_MAJOR_SHFT                                    0x0

#define PMIO_BUS_PERPH_TYPE_ADDR                                      (BUS_BASE      + 0x00000004)
#define PMIO_BUS_PERPH_TYPE_OFFS                                      0x00000004
#define PMIO_BUS_PERPH_TYPE_RMSK                                            0xff
#define PMIO_BUS_PERPH_TYPE_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_TYPE_ADDR, PMIO_BUS_PERPH_TYPE_RMSK, val)
#define PMIO_BUS_PERPH_TYPE_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_TYPE_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_TYPE_TYPE_BMSK                                       0xff
#define PMIO_BUS_PERPH_TYPE_TYPE_SHFT                                        0x0

#define PMIO_BUS_PERPH_SUBTYPE_ADDR                                   (BUS_BASE      + 0x00000005)
#define PMIO_BUS_PERPH_SUBTYPE_OFFS                                   0x00000005
#define PMIO_BUS_PERPH_SUBTYPE_RMSK                                         0xff
#define PMIO_BUS_PERPH_SUBTYPE_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_SUBTYPE_ADDR, PMIO_BUS_PERPH_SUBTYPE_RMSK, val)
#define PMIO_BUS_PERPH_SUBTYPE_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_SUBTYPE_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_SUBTYPE_SUBTYPE_BMSK                                 0xff
#define PMIO_BUS_PERPH_SUBTYPE_SUBTYPE_SHFT                                  0x0

#define PMIO_BUS_STATUS1_ADDR                                         (BUS_BASE      + 0x00000008)
#define PMIO_BUS_STATUS1_OFFS                                         0x00000008
#define PMIO_BUS_STATUS1_RMSK                                               0xef
#define PMIO_BUS_STATUS1_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_STATUS1_ADDR, PMIO_BUS_STATUS1_RMSK, val)
#define PMIO_BUS_STATUS1_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_STATUS1_ADDR, mask, shift, val)
#define PMIO_BUS_STATUS1_LOGGER_OK_BMSK                                     0x80
#define PMIO_BUS_STATUS1_LOGGER_OK_SHFT                                      0x7
#define PMIO_BUS_STATUS1_LOGGER_FULL_BMSK                                   0x40
#define PMIO_BUS_STATUS1_LOGGER_FULL_SHFT                                    0x6
#define PMIO_BUS_STATUS1_HALF_ENTRY_BMSK                                    0x20
#define PMIO_BUS_STATUS1_HALF_ENTRY_SHFT                                     0x5
#define PMIO_BUS_STATUS1_INTBUS_ARB_GNT_BMSK                                 0xf
#define PMIO_BUS_STATUS1_INTBUS_ARB_GNT_SHFT                                 0x0

#define PMIO_BUS_FIFO_SIZE_ADDR                                       (BUS_BASE      + 0x0000000b)
#define PMIO_BUS_FIFO_SIZE_OFFS                                       0x0000000b
#define PMIO_BUS_FIFO_SIZE_RMSK                                              0x3
#define PMIO_BUS_FIFO_SIZE_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_FIFO_SIZE_ADDR, PMIO_BUS_FIFO_SIZE_RMSK, val)
#define PMIO_BUS_FIFO_SIZE_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_FIFO_SIZE_ADDR, mask, shift, val)
#define PMIO_BUS_FIFO_SIZE_FIFO_SIZE_BMSK                                    0x3
#define PMIO_BUS_FIFO_SIZE_FIFO_SIZE_SHFT                                    0x0

#define PMIO_BUS_TIMEOUT_ADDR                                         (BUS_BASE      + 0x00000044)
#define PMIO_BUS_TIMEOUT_OFFS                                         0x00000044
#define PMIO_BUS_TIMEOUT_RMSK                                               0xff
#define PMIO_BUS_TIMEOUT_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_TIMEOUT_ADDR, PMIO_BUS_TIMEOUT_RMSK, val)
#define PMIO_BUS_TIMEOUT_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_TIMEOUT_ADDR, mask, shift, val)
#define PMIO_BUS_TIMEOUT_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_TIMEOUT_ADDR, val)
#define PMIO_BUS_TIMEOUT_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_TIMEOUT_ADDR, mask, val)
#define PMIO_BUS_TIMEOUT_TIMEOUT_MANT_BMSK                                  0xf0
#define PMIO_BUS_TIMEOUT_TIMEOUT_MANT_SHFT                                   0x4
#define PMIO_BUS_TIMEOUT_TIMEOUT_EXP_BMSK                                    0xf
#define PMIO_BUS_TIMEOUT_TIMEOUT_EXP_SHFT                                    0x0

#define PMIO_BUS_LOGGER_EN_ADDR                                       (BUS_BASE      + 0x00000046)
#define PMIO_BUS_LOGGER_EN_OFFS                                       0x00000046
#define PMIO_BUS_LOGGER_EN_RMSK                                             0x80
#define PMIO_BUS_LOGGER_EN_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_EN_ADDR, PMIO_BUS_LOGGER_EN_RMSK, val)
#define PMIO_BUS_LOGGER_EN_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_LOGGER_EN_ADDR, mask, shift, val)
#define PMIO_BUS_LOGGER_EN_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_LOGGER_EN_ADDR, val)
#define PMIO_BUS_LOGGER_EN_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_EN_ADDR, mask, val)
#define PMIO_BUS_LOGGER_EN_LOGGER_EN_BMSK                                   0x80
#define PMIO_BUS_LOGGER_EN_LOGGER_EN_SHFT                                    0x7

#define PMIO_BUS_LOGGER_CTL_ADDR                                      (BUS_BASE      + 0x00000050)
#define PMIO_BUS_LOGGER_CTL_OFFS                                      0x00000050
#define PMIO_BUS_LOGGER_CTL_RMSK                                             0xf
#define PMIO_BUS_LOGGER_CTL_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_CTL_ADDR, PMIO_BUS_LOGGER_CTL_RMSK, val)
#define PMIO_BUS_LOGGER_CTL_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_LOGGER_CTL_ADDR, mask, shift, val)
#define PMIO_BUS_LOGGER_CTL_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_LOGGER_CTL_ADDR, val)
#define PMIO_BUS_LOGGER_CTL_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_CTL_ADDR, mask, val)
#define PMIO_BUS_LOGGER_CTL_PERPH_FILTER_BMSK                                0x8
#define PMIO_BUS_LOGGER_CTL_PERPH_FILTER_SHFT                                0x3
#define PMIO_BUS_LOGGER_CTL_IGNORE_TIMESTAMP_BMSK                            0x4
#define PMIO_BUS_LOGGER_CTL_IGNORE_TIMESTAMP_SHFT                            0x2
#define PMIO_BUS_LOGGER_CTL_IGNORE_READS_BMSK                                0x2
#define PMIO_BUS_LOGGER_CTL_IGNORE_READS_SHFT                                0x1
#define PMIO_BUS_LOGGER_CTL_WRAP_BMSK                                        0x1
#define PMIO_BUS_LOGGER_CTL_WRAP_SHFT                                        0x0

#define PMIO_BUS_LOGGER_PID_ADDR                                      (BUS_BASE      + 0x00000052)
#define PMIO_BUS_LOGGER_PID_OFFS                                      0x00000052
#define PMIO_BUS_LOGGER_PID_RMSK                                            0xff
#define PMIO_BUS_LOGGER_PID_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_PID_ADDR, PMIO_BUS_LOGGER_PID_RMSK, val)
#define PMIO_BUS_LOGGER_PID_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_LOGGER_PID_ADDR, mask, shift, val)
#define PMIO_BUS_LOGGER_PID_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_LOGGER_PID_ADDR, val)
#define PMIO_BUS_LOGGER_PID_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_PID_ADDR, mask, val)
#define PMIO_BUS_LOGGER_PID_LOGGER_PID_BMSK                                 0xff
#define PMIO_BUS_LOGGER_PID_LOGGER_PID_SHFT                                  0x0

#define PMIO_BUS_LOGGER_SID_ADDR                                      (BUS_BASE      + 0x00000053)
#define PMIO_BUS_LOGGER_SID_OFFS                                      0x00000053
#define PMIO_BUS_LOGGER_SID_RMSK                                             0x3
#define PMIO_BUS_LOGGER_SID_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_SID_ADDR, PMIO_BUS_LOGGER_SID_RMSK, val)
#define PMIO_BUS_LOGGER_SID_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_LOGGER_SID_ADDR, mask, shift, val)
#define PMIO_BUS_LOGGER_SID_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_LOGGER_SID_ADDR, val)
#define PMIO_BUS_LOGGER_SID_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_LOGGER_SID_ADDR, mask, val)
#define PMIO_BUS_LOGGER_SID_LOGGER_SID_BMSK                                  0x3
#define PMIO_BUS_LOGGER_SID_LOGGER_SID_SHFT                                  0x0

#define PMIO_BUS_MEM_INTF_ADDR_ADDR                                   (BUS_BASE      + 0x00000054)
#define PMIO_BUS_MEM_INTF_ADDR_OFFS                                   0x00000054
#define PMIO_BUS_MEM_INTF_ADDR_RMSK                                         0xff
#define PMIO_BUS_MEM_INTF_ADDR_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_ADDR_ADDR, PMIO_BUS_MEM_INTF_ADDR_RMSK, val)
#define PMIO_BUS_MEM_INTF_ADDR_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_ADDR_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_ADDR_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_ADDR_ADDR, val)
#define PMIO_BUS_MEM_INTF_ADDR_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_ADDR_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_ADDR_MEM_INTF_ADDR_BMSK                           0xff
#define PMIO_BUS_MEM_INTF_ADDR_MEM_INTF_ADDR_SHFT                            0x0

#define PMIO_BUS_CAPTURE_INC_ADDR                                     (BUS_BASE      + 0x00000056)
#define PMIO_BUS_CAPTURE_INC_OFFS                                     0x00000056
#define PMIO_BUS_CAPTURE_INC_RMSK                                            0x1
#define PMIO_BUS_CAPTURE_INC_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_CAPTURE_INC_ADDR, val)
#define PMIO_BUS_CAPTURE_INC_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_CAPTURE_INC_ADDR, mask, val)
#define PMIO_BUS_CAPTURE_INC_CAPTURE_INC_BMSK                                0x1
#define PMIO_BUS_CAPTURE_INC_CAPTURE_INC_SHFT                                0x0

#define PMIO_BUS_MEM_INTF_DATA0_ADDR                                  (BUS_BASE      + 0x00000058)
#define PMIO_BUS_MEM_INTF_DATA0_OFFS                                  0x00000058
#define PMIO_BUS_MEM_INTF_DATA0_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA0_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA0_ADDR, PMIO_BUS_MEM_INTF_DATA0_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA0_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA0_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA0_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA0_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA0_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA0_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA0_MEM_INTF_DATA0_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA0_MEM_INTF_DATA0_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA1_ADDR                                  (BUS_BASE      + 0x00000059)
#define PMIO_BUS_MEM_INTF_DATA1_OFFS                                  0x00000059
#define PMIO_BUS_MEM_INTF_DATA1_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA1_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA1_ADDR, PMIO_BUS_MEM_INTF_DATA1_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA1_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA1_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA1_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA1_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA1_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA1_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA1_MEM_INTF_DATA1_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA1_MEM_INTF_DATA1_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA2_ADDR                                  (BUS_BASE      + 0x0000005a)
#define PMIO_BUS_MEM_INTF_DATA2_OFFS                                  0x0000005a
#define PMIO_BUS_MEM_INTF_DATA2_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA2_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA2_ADDR, PMIO_BUS_MEM_INTF_DATA2_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA2_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA2_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA2_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA2_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA2_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA2_MEM_INTF_DATA2_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA2_MEM_INTF_DATA2_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA3_ADDR                                  (BUS_BASE      + 0x0000005b)
#define PMIO_BUS_MEM_INTF_DATA3_OFFS                                  0x0000005b
#define PMIO_BUS_MEM_INTF_DATA3_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA3_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA3_ADDR, PMIO_BUS_MEM_INTF_DATA3_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA3_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA3_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA3_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA3_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA3_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA3_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA3_MEM_INTF_DATA3_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA3_MEM_INTF_DATA3_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA4_ADDR                                  (BUS_BASE      + 0x0000005c)
#define PMIO_BUS_MEM_INTF_DATA4_OFFS                                  0x0000005c
#define PMIO_BUS_MEM_INTF_DATA4_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA4_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA4_ADDR, PMIO_BUS_MEM_INTF_DATA4_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA4_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA4_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA4_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA4_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA4_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA4_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA4_MEM_INTF_DATA4_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA4_MEM_INTF_DATA4_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA5_ADDR                                  (BUS_BASE      + 0x0000005d)
#define PMIO_BUS_MEM_INTF_DATA5_OFFS                                  0x0000005d
#define PMIO_BUS_MEM_INTF_DATA5_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA5_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA5_ADDR, PMIO_BUS_MEM_INTF_DATA5_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA5_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA5_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA5_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA5_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA5_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA5_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA5_MEM_INTF_DATA5_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA5_MEM_INTF_DATA5_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA6_ADDR                                  (BUS_BASE      + 0x0000005e)
#define PMIO_BUS_MEM_INTF_DATA6_OFFS                                  0x0000005e
#define PMIO_BUS_MEM_INTF_DATA6_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA6_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA6_ADDR, PMIO_BUS_MEM_INTF_DATA6_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA6_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA6_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA6_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA6_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA6_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA6_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA6_MEM_INTF_DATA6_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA6_MEM_INTF_DATA6_SHFT                          0x0

#define PMIO_BUS_MEM_INTF_DATA7_ADDR                                  (BUS_BASE      + 0x0000005f)
#define PMIO_BUS_MEM_INTF_DATA7_OFFS                                  0x0000005f
#define PMIO_BUS_MEM_INTF_DATA7_RMSK                                        0xff
#define PMIO_BUS_MEM_INTF_DATA7_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA7_ADDR, PMIO_BUS_MEM_INTF_DATA7_RMSK, val)
#define PMIO_BUS_MEM_INTF_DATA7_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA7_ADDR, mask, shift, val)
#define PMIO_BUS_MEM_INTF_DATA7_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA7_ADDR, val)
#define PMIO_BUS_MEM_INTF_DATA7_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_MEM_INTF_DATA7_ADDR, mask, val)
#define PMIO_BUS_MEM_INTF_DATA7_MEM_INTF_DATA7_BMSK                         0xff
#define PMIO_BUS_MEM_INTF_DATA7_MEM_INTF_DATA7_SHFT                          0x0

#define PMIO_BUS_SEC_ACCESS_ADDR                                      (BUS_BASE      + 0x000000d0)
#define PMIO_BUS_SEC_ACCESS_OFFS                                      0x000000d0
#define PMIO_BUS_SEC_ACCESS_RMSK                                            0xff
#define PMIO_BUS_SEC_ACCESS_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_SEC_ACCESS_ADDR, PMIO_BUS_SEC_ACCESS_RMSK, val)
#define PMIO_BUS_SEC_ACCESS_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_SEC_ACCESS_ADDR, mask, shift, val)
#define PMIO_BUS_SEC_ACCESS_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_SEC_ACCESS_ADDR, val)
#define PMIO_BUS_SEC_ACCESS_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_SEC_ACCESS_ADDR, mask, val)
#define PMIO_BUS_SEC_ACCESS_SEC_UNLOCK_BMSK                                 0xff
#define PMIO_BUS_SEC_ACCESS_SEC_UNLOCK_SHFT                                  0x0

#define PMIO_BUS_PERPH_RESET_CTL1_ADDR                                (BUS_BASE      + 0x000000d8)
#define PMIO_BUS_PERPH_RESET_CTL1_OFFS                                0x000000d8
#define PMIO_BUS_PERPH_RESET_CTL1_RMSK                                       0x1
#define PMIO_BUS_PERPH_RESET_CTL1_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL1_ADDR, PMIO_BUS_PERPH_RESET_CTL1_RMSK, val)
#define PMIO_BUS_PERPH_RESET_CTL1_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL1_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_RESET_CTL1_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL1_ADDR, val)
#define PMIO_BUS_PERPH_RESET_CTL1_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL1_ADDR, mask, val)
#define PMIO_BUS_PERPH_RESET_CTL1_ENABLE_PRESET_BMSK                         0x1
#define PMIO_BUS_PERPH_RESET_CTL1_ENABLE_PRESET_SHFT                         0x0

#define PMIO_BUS_PERPH_RESET_CTL2_ADDR                                (BUS_BASE      + 0x000000d9)
#define PMIO_BUS_PERPH_RESET_CTL2_OFFS                                0x000000d9
#define PMIO_BUS_PERPH_RESET_CTL2_RMSK                                       0x1
#define PMIO_BUS_PERPH_RESET_CTL2_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL2_ADDR, PMIO_BUS_PERPH_RESET_CTL2_RMSK, val)
#define PMIO_BUS_PERPH_RESET_CTL2_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL2_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_RESET_CTL2_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL2_ADDR, val)
#define PMIO_BUS_PERPH_RESET_CTL2_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL2_ADDR, mask, val)
#define PMIO_BUS_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_BMSK                 0x1
#define PMIO_BUS_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_SHFT                 0x0

#define PMIO_BUS_PERPH_RESET_CTL3_ADDR                                (BUS_BASE      + 0x000000da)
#define PMIO_BUS_PERPH_RESET_CTL3_OFFS                                0x000000da
#define PMIO_BUS_PERPH_RESET_CTL3_RMSK                                       0xf
#define PMIO_BUS_PERPH_RESET_CTL3_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL3_ADDR, PMIO_BUS_PERPH_RESET_CTL3_RMSK, val)
#define PMIO_BUS_PERPH_RESET_CTL3_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL3_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_RESET_CTL3_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL3_ADDR, val)
#define PMIO_BUS_PERPH_RESET_CTL3_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL3_ADDR, mask, val)
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_BMSK                       0x8
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_SHFT                       0x3
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_WARM_RB_BMSK                        0x4
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_WARM_RB_SHFT                        0x2
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_BMSK                   0x2
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_SHFT                   0x1
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_BMSK                   0x1
#define PMIO_BUS_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_SHFT                   0x0

#define PMIO_BUS_PERPH_RESET_CTL4_ADDR                                (BUS_BASE      + 0x000000db)
#define PMIO_BUS_PERPH_RESET_CTL4_OFFS                                0x000000db
#define PMIO_BUS_PERPH_RESET_CTL4_RMSK                                       0x1
#define PMIO_BUS_PERPH_RESET_CTL4_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL4_ADDR, PMIO_BUS_PERPH_RESET_CTL4_RMSK, val)
#define PMIO_BUS_PERPH_RESET_CTL4_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL4_ADDR, mask, shift, val)
#define PMIO_BUS_PERPH_RESET_CTL4_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL4_ADDR, val)
#define PMIO_BUS_PERPH_RESET_CTL4_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_PERPH_RESET_CTL4_ADDR, mask, val)
#define PMIO_BUS_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_BMSK                      0x1
#define PMIO_BUS_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_SHFT                      0x0

#define PMIO_BUS_TEST1_ADDR                                           (BUS_BASE      + 0x000000e2)
#define PMIO_BUS_TEST1_OFFS                                           0x000000e2
#define PMIO_BUS_TEST1_RMSK                                                 0xff
#define PMIO_BUS_TEST1_IN(ctxt, pmic, val)\
        in_pmio_masked(ctxt, pmic, PMIO_BUS_TEST1_ADDR, PMIO_BUS_TEST1_RMSK, val)
#define PMIO_BUS_TEST1_INMS(ctxt, pmic, mask, shift, val)\
        in_pmio_masked_shift(ctxt, pmic, PMIO_BUS_TEST1_ADDR, mask, shift, val)
#define PMIO_BUS_TEST1_OUT(ctxt, pmic, val)\
        out_pmio(ctxt, pmic, PMIO_BUS_TEST1_ADDR, val)
#define PMIO_BUS_TEST1_OUTM(ctxt, pmic, mask, val)\
        out_pmio_masked(ctxt, pmic, PMIO_BUS_TEST1_ADDR, mask, val)
#define PMIO_BUS_TEST1_DTEST_EN_BMSK                                        0xf0
#define PMIO_BUS_TEST1_DTEST_EN_SHFT                                         0x4
#define PMIO_BUS_TEST1_DTEST_BMSK                                            0xf
#define PMIO_BUS_TEST1_DTEST_SHFT                                            0x0


#endif /* __PMIO_BUS_H__ */