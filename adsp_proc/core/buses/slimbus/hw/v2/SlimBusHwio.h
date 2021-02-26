#ifndef __SLIMBUSHWIO_H__
#define __SLIMBUSHWIO_H__
/*
===========================================================================
*/
/**
  @file SlimBusHwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8998 (Nazgul) [nazgul_v1.0_p1q1r18.1]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_AUD_SLIMBUS

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

  $Header: //components/rel/core.qdsp6/2.1/buses/slimbus/hw/v2/SlimBusHwio.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SLIMBUS
 *--------------------------------------------------------------------------*/

#define HWIO_SB_COMP_HW_VERSION_ADDR(x)                                                        ((x) + 0x00000000)
#define HWIO_SB_COMP_HW_VERSION_RMSK                                                           0xffffffff
#define HWIO_SB_COMP_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_HW_VERSION_ADDR(x), HWIO_SB_COMP_HW_VERSION_RMSK)
#define HWIO_SB_COMP_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_HW_VERSION_ADDR(x), m)
#define HWIO_SB_COMP_HW_VERSION_MAJOR_BMSK                                                     0xff000000
#define HWIO_SB_COMP_HW_VERSION_MAJOR_SHFT                                                           0x18
#define HWIO_SB_COMP_HW_VERSION_MINOR_BMSK                                                       0xff0000
#define HWIO_SB_COMP_HW_VERSION_MINOR_SHFT                                                           0x10
#define HWIO_SB_COMP_HW_VERSION_STEP_BMSK                                                          0xffff
#define HWIO_SB_COMP_HW_VERSION_STEP_SHFT                                                             0x0

#define HWIO_SB_COMP_CFG_ADDR(x)                                                               ((x) + 0x00000004)
#define HWIO_SB_COMP_CFG_RMSK                                                                        0x3f
#define HWIO_SB_COMP_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_CFG_ADDR(x), HWIO_SB_COMP_CFG_RMSK)
#define HWIO_SB_COMP_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_CFG_ADDR(x), m)
#define HWIO_SB_COMP_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_CFG_ADDR(x),v)
#define HWIO_SB_COMP_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_CFG_ADDR(x),m,v,HWIO_SB_COMP_CFG_IN(x))
#define HWIO_SB_COMP_CFG_NO_NEGEDGE_RET_SUPPORT_BMSK                                                 0x20
#define HWIO_SB_COMP_CFG_NO_NEGEDGE_RET_SUPPORT_SHFT                                                  0x5
#define HWIO_SB_COMP_CFG_ALLOW_RET_TO_NEGEDGE_NO_CLK_PAUSE_BMSK                                      0x10
#define HWIO_SB_COMP_CFG_ALLOW_RET_TO_NEGEDGE_NO_CLK_PAUSE_SHFT                                       0x4
#define HWIO_SB_COMP_CFG_SELECT_RET_TO_NEGEDGE_BMSK                                                   0x8
#define HWIO_SB_COMP_CFG_SELECT_RET_TO_NEGEDGE_SHFT                                                   0x3
#define HWIO_SB_COMP_CFG_INT_FRM_CGC_IDLE_LOW_BMSK                                                    0x4
#define HWIO_SB_COMP_CFG_INT_FRM_CGC_IDLE_LOW_SHFT                                                    0x2
#define HWIO_SB_COMP_CFG_RET_EN_BMSK                                                                  0x2
#define HWIO_SB_COMP_CFG_RET_EN_SHFT                                                                  0x1
#define HWIO_SB_COMP_CFG_ENABLE_BMSK                                                                  0x1
#define HWIO_SB_COMP_CFG_ENABLE_SHFT                                                                  0x0

#define HWIO_SB_SW_RESET_ADDR(x)                                                               ((x) + 0x00000008)
#define HWIO_SB_SW_RESET_RMSK                                                                         0x3
#define HWIO_SB_SW_RESET_IN(x)      \
        in_dword_masked(HWIO_SB_SW_RESET_ADDR(x), HWIO_SB_SW_RESET_RMSK)
#define HWIO_SB_SW_RESET_INM(x, m)      \
        in_dword_masked(HWIO_SB_SW_RESET_ADDR(x), m)
#define HWIO_SB_SW_RESET_OUT(x, v)      \
        out_dword(HWIO_SB_SW_RESET_ADDR(x),v)
#define HWIO_SB_SW_RESET_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_SW_RESET_ADDR(x),m,v,HWIO_SB_SW_RESET_IN(x))
#define HWIO_SB_SW_RESET_SW_RESET_ACK_BMSK                                                            0x2
#define HWIO_SB_SW_RESET_SW_RESET_ACK_SHFT                                                            0x1
#define HWIO_SB_SW_RESET_SW_RESET_BMSK                                                                0x1
#define HWIO_SB_SW_RESET_SW_RESET_SHFT                                                                0x0

#define HWIO_SB_COMP_POWER_CFG_ADDR(x)                                                         ((x) + 0x0000000c)
#define HWIO_SB_COMP_POWER_CFG_RMSK                                                            0xffffff3f
#define HWIO_SB_COMP_POWER_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_POWER_CFG_ADDR(x), HWIO_SB_COMP_POWER_CFG_RMSK)
#define HWIO_SB_COMP_POWER_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_POWER_CFG_ADDR(x), m)
#define HWIO_SB_COMP_POWER_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_POWER_CFG_ADDR(x),v)
#define HWIO_SB_COMP_POWER_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_POWER_CFG_ADDR(x),m,v,HWIO_SB_COMP_POWER_CFG_IN(x))
#define HWIO_SB_COMP_POWER_CFG_ENABLE_PGD_PORTS_CGC_BMSK                                       0xffffff00
#define HWIO_SB_COMP_POWER_CFG_ENABLE_PGD_PORTS_CGC_SHFT                                              0x8
#define HWIO_SB_COMP_POWER_CFG_ENABLE_PGD_CGC_BMSK                                                   0x20
#define HWIO_SB_COMP_POWER_CFG_ENABLE_PGD_CGC_SHFT                                                    0x5
#define HWIO_SB_COMP_POWER_CFG_ENABLE_NGD2_CGC_BMSK                                                  0x10
#define HWIO_SB_COMP_POWER_CFG_ENABLE_NGD2_CGC_SHFT                                                   0x4
#define HWIO_SB_COMP_POWER_CFG_ENABLE_NGD1_CGC_BMSK                                                   0x8
#define HWIO_SB_COMP_POWER_CFG_ENABLE_NGD1_CGC_SHFT                                                   0x3
#define HWIO_SB_COMP_POWER_CFG_ENABLE_INTF_CGC_BMSK                                                   0x4
#define HWIO_SB_COMP_POWER_CFG_ENABLE_INTF_CGC_SHFT                                                   0x2
#define HWIO_SB_COMP_POWER_CFG_ENABLE_FRM_CGC_BMSK                                                    0x2
#define HWIO_SB_COMP_POWER_CFG_ENABLE_FRM_CGC_SHFT                                                    0x1
#define HWIO_SB_COMP_POWER_CFG_ENABLE_MGR_CGC_BMSK                                                    0x1
#define HWIO_SB_COMP_POWER_CFG_ENABLE_MGR_CGC_SHFT                                                    0x0

#define HWIO_SB_COMP_FEATURE_CFG_ADDR(x)                                                       ((x) + 0x00000010)
#define HWIO_SB_COMP_FEATURE_CFG_RMSK                                                          0xffffffff
#define HWIO_SB_COMP_FEATURE_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_FEATURE_CFG_ADDR(x), HWIO_SB_COMP_FEATURE_CFG_RMSK)
#define HWIO_SB_COMP_FEATURE_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_FEATURE_CFG_ADDR(x), m)
#define HWIO_SB_COMP_FEATURE_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_FEATURE_CFG_ADDR(x),v)
#define HWIO_SB_COMP_FEATURE_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_FEATURE_CFG_ADDR(x),m,v,HWIO_SB_COMP_FEATURE_CFG_IN(x))
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_PHASE_SHIFT_BMSK                                             0x20
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_PHASE_SHIFT_SHFT                                              0x5
#define HWIO_SB_COMP_FEATURE_CFG_DISABLE_FRAMER_CLK_WHEN_COMP_DISABLED_BMSK                          0x10
#define HWIO_SB_COMP_FEATURE_CFG_DISABLE_FRAMER_CLK_WHEN_COMP_DISABLED_SHFT                           0x4
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_ALWAYS_LOGICAL_OR_BMSK                                        0x8
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_ALWAYS_LOGICAL_OR_SHFT                                        0x3
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_LEGACY_FRAMER_HANDOVER_BMSK                                   0x4
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_LEGACY_FRAMER_HANDOVER_SHFT                                   0x2
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_FEATURE_B_BMSK                                                0x2
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_FEATURE_B_SHFT                                                0x1
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_FEATURE_A_BMSK                                                0x1
#define HWIO_SB_COMP_FEATURE_CFG_ENABLE_FEATURE_A_SHFT                                                0x0

#define HWIO_SB_COMP_STATUS_ADDR(x)                                                            ((x) + 0x00000020)
#define HWIO_SB_COMP_STATUS_RMSK                                                                      0x7
#define HWIO_SB_COMP_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_STATUS_ADDR(x), HWIO_SB_COMP_STATUS_RMSK)
#define HWIO_SB_COMP_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_STATUS_ADDR(x), m)
#define HWIO_SB_COMP_STATUS_NUM_OF_LANES_PARAM_BMSK                                                   0x7
#define HWIO_SB_COMP_STATUS_NUM_OF_LANES_PARAM_SHFT                                                   0x0

#define HWIO_SB_COMP_PARAMS_ADDR(x)                                                            ((x) + 0x00000024)
#define HWIO_SB_COMP_PARAMS_RMSK                                                               0xffff03ff
#define HWIO_SB_COMP_PARAMS_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_PARAMS_ADDR(x), HWIO_SB_COMP_PARAMS_RMSK)
#define HWIO_SB_COMP_PARAMS_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_PARAMS_ADDR(x), m)
#define HWIO_SB_COMP_PARAMS_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_PARAMS_ADDR(x),v)
#define HWIO_SB_COMP_PARAMS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_PARAMS_ADDR(x),m,v,HWIO_SB_COMP_PARAMS_IN(x))
#define HWIO_SB_COMP_PARAMS_SW_CFG_EA_PC_BMSK                                                  0xffff0000
#define HWIO_SB_COMP_PARAMS_SW_CFG_EA_PC_SHFT                                                        0x10
#define HWIO_SB_COMP_PARAMS_NUM_OF_NACKS_RETRANS_BMSK                                               0x3c0
#define HWIO_SB_COMP_PARAMS_NUM_OF_NACKS_RETRANS_SHFT                                                 0x6
#define HWIO_SB_COMP_PARAMS_EN_ENDLESS_NACK_RETRANS_BMSK                                             0x20
#define HWIO_SB_COMP_PARAMS_EN_ENDLESS_NACK_RETRANS_SHFT                                              0x5
#define HWIO_SB_COMP_PARAMS_USE_PARTIAL_CLR_INFO_MASK_BMSK                                           0x10
#define HWIO_SB_COMP_PARAMS_USE_PARTIAL_CLR_INFO_MASK_SHFT                                            0x4
#define HWIO_SB_COMP_PARAMS_USE_CLK_PAUSE_HCGC_BMSK                                                   0x8
#define HWIO_SB_COMP_PARAMS_USE_CLK_PAUSE_HCGC_SHFT                                                   0x3
#define HWIO_SB_COMP_PARAMS_USE_SW_CFG_EA_PC_BMSK                                                     0x4
#define HWIO_SB_COMP_PARAMS_USE_SW_CFG_EA_PC_SHFT                                                     0x2
#define HWIO_SB_COMP_PARAMS_DT_RSVD_REPLY_VAL_BMSK                                                    0x2
#define HWIO_SB_COMP_PARAMS_DT_RSVD_REPLY_VAL_SHFT                                                    0x1
#define HWIO_SB_COMP_PARAMS_DT_RSVD_REPLY_BMSK                                                        0x1
#define HWIO_SB_COMP_PARAMS_DT_RSVD_REPLY_SHFT                                                        0x0

#define HWIO_SB_COMP_CHAR_CFG_ADDR(x)                                                          ((x) + 0x00000028)
#define HWIO_SB_COMP_CHAR_CFG_RMSK                                                                0x7ff11
#define HWIO_SB_COMP_CHAR_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_CHAR_CFG_ADDR(x), HWIO_SB_COMP_CHAR_CFG_RMSK)
#define HWIO_SB_COMP_CHAR_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_CHAR_CFG_ADDR(x), m)
#define HWIO_SB_COMP_CHAR_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_CHAR_CFG_ADDR(x),v)
#define HWIO_SB_COMP_CHAR_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_CHAR_CFG_ADDR(x),m,v,HWIO_SB_COMP_CHAR_CFG_IN(x))
#define HWIO_SB_COMP_CHAR_CFG_DATA_IN_LANE_SEL_BMSK                                               0x70000
#define HWIO_SB_COMP_CHAR_CFG_DATA_IN_LANE_SEL_SHFT                                                  0x10
#define HWIO_SB_COMP_CHAR_CFG_CHAR_MODE_BMSK                                                       0xf000
#define HWIO_SB_COMP_CHAR_CFG_CHAR_MODE_SHFT                                                          0xc
#define HWIO_SB_COMP_CHAR_CFG_CHAR_STATUS_BMSK                                                      0xf00
#define HWIO_SB_COMP_CHAR_CFG_CHAR_STATUS_SHFT                                                        0x8
#define HWIO_SB_COMP_CHAR_CFG_DIRECTION_BMSK                                                         0x10
#define HWIO_SB_COMP_CHAR_CFG_DIRECTION_SHFT                                                          0x4
#define HWIO_SB_COMP_CHAR_CFG_ENABLE_BMSK                                                             0x1
#define HWIO_SB_COMP_CHAR_CFG_ENABLE_SHFT                                                             0x0

#define HWIO_SB_COMP_CHAR_DATA_n_ADDR(base,n)                                                  ((base) + 0x0000002c + 0x4 * (n))
#define HWIO_SB_COMP_CHAR_DATA_n_RMSK                                                              0xffff
#define HWIO_SB_COMP_CHAR_DATA_n_MAXn                                                                   1
#define HWIO_SB_COMP_CHAR_DATA_n_INI(base,n)        \
        in_dword_masked(HWIO_SB_COMP_CHAR_DATA_n_ADDR(base,n), HWIO_SB_COMP_CHAR_DATA_n_RMSK)
#define HWIO_SB_COMP_CHAR_DATA_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_COMP_CHAR_DATA_n_ADDR(base,n), mask)
#define HWIO_SB_COMP_CHAR_DATA_n_OUTI(base,n,val)    \
        out_dword(HWIO_SB_COMP_CHAR_DATA_n_ADDR(base,n),val)
#define HWIO_SB_COMP_CHAR_DATA_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_COMP_CHAR_DATA_n_ADDR(base,n),mask,val,HWIO_SB_COMP_CHAR_DATA_n_INI(base,n))
#define HWIO_SB_COMP_CHAR_DATA_n_DIN_ACTUAL_BMSK                                                   0xff00
#define HWIO_SB_COMP_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                      0x8
#define HWIO_SB_COMP_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                              0xff
#define HWIO_SB_COMP_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                               0x0

#define HWIO_SB_COMP_PRT_LOGGER_CFG_ADDR(x)                                                    ((x) + 0x00000034)
#define HWIO_SB_COMP_PRT_LOGGER_CFG_RMSK                                                        0x1ffffff
#define HWIO_SB_COMP_PRT_LOGGER_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_PRT_LOGGER_CFG_ADDR(x), HWIO_SB_COMP_PRT_LOGGER_CFG_RMSK)
#define HWIO_SB_COMP_PRT_LOGGER_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_PRT_LOGGER_CFG_ADDR(x), m)
#define HWIO_SB_COMP_PRT_LOGGER_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_PRT_LOGGER_CFG_ADDR(x),v)
#define HWIO_SB_COMP_PRT_LOGGER_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_PRT_LOGGER_CFG_ADDR(x),m,v,HWIO_SB_COMP_PRT_LOGGER_CFG_IN(x))
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER_SF_RES_EN_BMSK                                   0x1000000
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER_SF_RES_EN_SHFT                                        0x18
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_SEL_BMSK                                         0xf80000
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_SEL_SHFT                                             0x13
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_CONT_SAMP_EN_BMSK                                 0x40000
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_CONT_SAMP_EN_SHFT                                    0x12
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_TB_EN_BMSK                                        0x20000
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_TB_EN_SHFT                                           0x11
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_EN_BMSK                                           0x10000
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER2_EN_SHFT                                              0x10
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_SEL_BMSK                                           0xf800
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_SEL_SHFT                                              0xb
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_CONT_SAMP_EN_BMSK                                   0x400
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_CONT_SAMP_EN_SHFT                                     0xa
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_TB_EN_BMSK                                          0x200
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_TB_EN_SHFT                                            0x9
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_EN_BMSK                                             0x100
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER1_EN_SHFT                                               0x8
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_SEL_BMSK                                             0xf8
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_SEL_SHFT                                              0x3
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_CONT_SAMP_EN_BMSK                                     0x4
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_CONT_SAMP_EN_SHFT                                     0x2
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_TB_EN_BMSK                                            0x2
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_TB_EN_SHFT                                            0x1
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_EN_BMSK                                               0x1
#define HWIO_SB_COMP_PRT_LOGGER_CFG_PRT_LOGGER0_EN_SHFT                                               0x0

#define HWIO_SB_MGR_CFG_ADDR(x)                                                                ((x) + 0x00000200)
#define HWIO_SB_MGR_CFG_RMSK                                                                          0xf
#define HWIO_SB_MGR_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_CFG_ADDR(x), HWIO_SB_MGR_CFG_RMSK)
#define HWIO_SB_MGR_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_CFG_ADDR(x), m)
#define HWIO_SB_MGR_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_MGR_CFG_ADDR(x),v)
#define HWIO_SB_MGR_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_MGR_CFG_ADDR(x),m,v,HWIO_SB_MGR_CFG_IN(x))
#define HWIO_SB_MGR_CFG_TX_MSGQ_EN_LOW_BMSK                                                           0x8
#define HWIO_SB_MGR_CFG_TX_MSGQ_EN_LOW_SHFT                                                           0x3
#define HWIO_SB_MGR_CFG_TX_MSGQ_EN_HIGH_BMSK                                                          0x4
#define HWIO_SB_MGR_CFG_TX_MSGQ_EN_HIGH_SHFT                                                          0x2
#define HWIO_SB_MGR_CFG_RX_MSGQ_EN_BMSK                                                               0x2
#define HWIO_SB_MGR_CFG_RX_MSGQ_EN_SHFT                                                               0x1
#define HWIO_SB_MGR_CFG_ENABLE_BMSK                                                                   0x1
#define HWIO_SB_MGR_CFG_ENABLE_SHFT                                                                   0x0

#define HWIO_SB_MGR_STATUS_ADDR(x)                                                             ((x) + 0x00000204)
#define HWIO_SB_MGR_STATUS_RMSK                                                                  0xffffff
#define HWIO_SB_MGR_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_STATUS_ADDR(x), HWIO_SB_MGR_STATUS_RMSK)
#define HWIO_SB_MGR_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_STATUS_ADDR(x), m)
#define HWIO_SB_MGR_STATUS_NACKED_TX_MSG_MC_BMSK                                                 0xfe0000
#define HWIO_SB_MGR_STATUS_NACKED_TX_MSG_MC_SHFT                                                     0x11
#define HWIO_SB_MGR_STATUS_ACKED_TX_MSG_MC_BMSK                                                   0x1fc00
#define HWIO_SB_MGR_STATUS_ACKED_TX_MSG_MC_SHFT                                                       0xa
#define HWIO_SB_MGR_STATUS_ERROR_BMSK                                                               0x300
#define HWIO_SB_MGR_STATUS_ERROR_SHFT                                                                 0x8
#define HWIO_SB_MGR_STATUS_PIPE_NUM_OFFSET_BMSK                                                      0xfc
#define HWIO_SB_MGR_STATUS_PIPE_NUM_OFFSET_SHFT                                                       0x2
#define HWIO_SB_MGR_STATUS_ENUMERATED_BMSK                                                            0x2
#define HWIO_SB_MGR_STATUS_ENUMERATED_SHFT                                                            0x1
#define HWIO_SB_MGR_STATUS_TX_MSG_BUFFER_BUSY_BMSK                                                    0x1
#define HWIO_SB_MGR_STATUS_TX_MSG_BUFFER_BUSY_SHFT                                                    0x0

#define HWIO_SB_MGR_RX_MSGQ_CFG_ADDR(x)                                                        ((x) + 0x00000208)
#define HWIO_SB_MGR_RX_MSGQ_CFG_RMSK                                                           0xffffffff
#define HWIO_SB_MGR_RX_MSGQ_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_RX_MSGQ_CFG_ADDR(x), HWIO_SB_MGR_RX_MSGQ_CFG_RMSK)
#define HWIO_SB_MGR_RX_MSGQ_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_RX_MSGQ_CFG_ADDR(x), m)
#define HWIO_SB_MGR_RX_MSGQ_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_MGR_RX_MSGQ_CFG_ADDR(x),v)
#define HWIO_SB_MGR_RX_MSGQ_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_MGR_RX_MSGQ_CFG_ADDR(x),m,v,HWIO_SB_MGR_RX_MSGQ_CFG_IN(x))
#define HWIO_SB_MGR_RX_MSGQ_CFG_TIME_OUT_VAL_BMSK                                              0xffff0000
#define HWIO_SB_MGR_RX_MSGQ_CFG_TIME_OUT_VAL_SHFT                                                    0x10
#define HWIO_SB_MGR_RX_MSGQ_CFG_TRANS_SIZE_BMSK                                                    0xff00
#define HWIO_SB_MGR_RX_MSGQ_CFG_TRANS_SIZE_SHFT                                                       0x8
#define HWIO_SB_MGR_RX_MSGQ_CFG_BLOCK_SIZE_BMSK                                                      0xff
#define HWIO_SB_MGR_RX_MSGQ_CFG_BLOCK_SIZE_SHFT                                                       0x0

#define HWIO_SB_MGR_INT_EN_ADDR(x)                                                             ((x) + 0x00000210)
#define HWIO_SB_MGR_INT_EN_RMSK                                                                0xff000000
#define HWIO_SB_MGR_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_INT_EN_ADDR(x), HWIO_SB_MGR_INT_EN_RMSK)
#define HWIO_SB_MGR_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_INT_EN_ADDR(x), m)
#define HWIO_SB_MGR_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_MGR_INT_EN_ADDR(x),v)
#define HWIO_SB_MGR_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_MGR_INT_EN_ADDR(x),m,v,HWIO_SB_MGR_INT_EN_IN(x))
#define HWIO_SB_MGR_INT_EN_TX_MSG_SENT_BMSK                                                    0x80000000
#define HWIO_SB_MGR_INT_EN_TX_MSG_SENT_SHFT                                                          0x1f
#define HWIO_SB_MGR_INT_EN_RX_MSG_RCVD_BMSK                                                    0x40000000
#define HWIO_SB_MGR_INT_EN_RX_MSG_RCVD_SHFT                                                          0x1e
#define HWIO_SB_MGR_INT_EN_DEV_ERR_BMSK                                                        0x20000000
#define HWIO_SB_MGR_INT_EN_DEV_ERR_SHFT                                                              0x1d
#define HWIO_SB_MGR_INT_EN_IE_VE_CHANGE_BMSK                                                   0x10000000
#define HWIO_SB_MGR_INT_EN_IE_VE_CHANGE_SHFT                                                         0x1c
#define HWIO_SB_MGR_INT_EN_INVALID_TX_MSG_BMSK                                                  0x8000000
#define HWIO_SB_MGR_INT_EN_INVALID_TX_MSG_SHFT                                                       0x1b
#define HWIO_SB_MGR_INT_EN_TX_MSG_BUF_CONTENTION_BMSK                                           0x4000000
#define HWIO_SB_MGR_INT_EN_TX_MSG_BUF_CONTENTION_SHFT                                                0x1a
#define HWIO_SB_MGR_INT_EN_TX_MSG_NACKED_BMSK                                                   0x2000000
#define HWIO_SB_MGR_INT_EN_TX_MSG_NACKED_SHFT                                                        0x19
#define HWIO_SB_MGR_INT_EN_RECONFIG_DONE_BMSK                                                   0x1000000
#define HWIO_SB_MGR_INT_EN_RECONFIG_DONE_SHFT                                                        0x18

#define HWIO_SB_MGR_INT_STATUS_ADDR(x)                                                         ((x) + 0x00000214)
#define HWIO_SB_MGR_INT_STATUS_RMSK                                                            0xff000000
#define HWIO_SB_MGR_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_INT_STATUS_ADDR(x), HWIO_SB_MGR_INT_STATUS_RMSK)
#define HWIO_SB_MGR_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_INT_STATUS_ADDR(x), m)
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_SENT_BMSK                                                0x80000000
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_SENT_SHFT                                                      0x1f
#define HWIO_SB_MGR_INT_STATUS_RX_MSG_RCVD_BMSK                                                0x40000000
#define HWIO_SB_MGR_INT_STATUS_RX_MSG_RCVD_SHFT                                                      0x1e
#define HWIO_SB_MGR_INT_STATUS_DEV_ERR_BMSK                                                    0x20000000
#define HWIO_SB_MGR_INT_STATUS_DEV_ERR_SHFT                                                          0x1d
#define HWIO_SB_MGR_INT_STATUS_IE_VE_CHANGE_BMSK                                               0x10000000
#define HWIO_SB_MGR_INT_STATUS_IE_VE_CHANGE_SHFT                                                     0x1c
#define HWIO_SB_MGR_INT_STATUS_INVALID_TX_MSG_BMSK                                              0x8000000
#define HWIO_SB_MGR_INT_STATUS_INVALID_TX_MSG_SHFT                                                   0x1b
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_BUF_CONTENTION_BMSK                                       0x4000000
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_BUF_CONTENTION_SHFT                                            0x1a
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_NACKED_BMSK                                               0x2000000
#define HWIO_SB_MGR_INT_STATUS_TX_MSG_NACKED_SHFT                                                    0x19
#define HWIO_SB_MGR_INT_STATUS_RECONFIG_DONE_BMSK                                               0x1000000
#define HWIO_SB_MGR_INT_STATUS_RECONFIG_DONE_SHFT                                                    0x18

#define HWIO_SB_MGR_INT_CLR_ADDR(x)                                                            ((x) + 0x00000218)
#define HWIO_SB_MGR_INT_CLR_RMSK                                                               0xff000000
#define HWIO_SB_MGR_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_MGR_INT_CLR_ADDR(x),v)
#define HWIO_SB_MGR_INT_CLR_TX_MSG_SENT_BMSK                                                   0x80000000
#define HWIO_SB_MGR_INT_CLR_TX_MSG_SENT_SHFT                                                         0x1f
#define HWIO_SB_MGR_INT_CLR_RX_MSG_RCVD_BMSK                                                   0x40000000
#define HWIO_SB_MGR_INT_CLR_RX_MSG_RCVD_SHFT                                                         0x1e
#define HWIO_SB_MGR_INT_CLR_DEV_ERR_BMSK                                                       0x20000000
#define HWIO_SB_MGR_INT_CLR_DEV_ERR_SHFT                                                             0x1d
#define HWIO_SB_MGR_INT_CLR_IE_VE_CHANGE_BMSK                                                  0x10000000
#define HWIO_SB_MGR_INT_CLR_IE_VE_CHANGE_SHFT                                                        0x1c
#define HWIO_SB_MGR_INT_CLR_INVALID_TX_MSG_BMSK                                                 0x8000000
#define HWIO_SB_MGR_INT_CLR_INVALID_TX_MSG_SHFT                                                      0x1b
#define HWIO_SB_MGR_INT_CLR_TX_MSG_BUF_CONTENTION_BMSK                                          0x4000000
#define HWIO_SB_MGR_INT_CLR_TX_MSG_BUF_CONTENTION_SHFT                                               0x1a
#define HWIO_SB_MGR_INT_CLR_TX_MSG_NACKED_BMSK                                                  0x2000000
#define HWIO_SB_MGR_INT_CLR_TX_MSG_NACKED_SHFT                                                       0x19
#define HWIO_SB_MGR_INT_CLR_RECONFIG_DONE_BMSK                                                  0x1000000
#define HWIO_SB_MGR_INT_CLR_RECONFIG_DONE_SHFT                                                       0x18

#define HWIO_SB_MGR_TX_MSGn_ADDR(base,n)                                                       ((base) + 0x00000230 + 0x4 * (n))
#define HWIO_SB_MGR_TX_MSGn_RMSK                                                               0xffffffff
#define HWIO_SB_MGR_TX_MSGn_MAXn                                                                        9
#define HWIO_SB_MGR_TX_MSGn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_MGR_TX_MSGn_ADDR(base,n),val)
#define HWIO_SB_MGR_TX_MSGn_MSGVAL_BMSK                                                        0xffffffff
#define HWIO_SB_MGR_TX_MSGn_MSGVAL_SHFT                                                               0x0

#define HWIO_SB_MGR_RX_MSGn_ADDR(base,n)                                                       ((base) + 0x00000270 + 0x4 * (n))
#define HWIO_SB_MGR_RX_MSGn_RMSK                                                               0xffffffff
#define HWIO_SB_MGR_RX_MSGn_MAXn                                                                        9
#define HWIO_SB_MGR_RX_MSGn_INI(base,n)        \
        in_dword_masked(HWIO_SB_MGR_RX_MSGn_ADDR(base,n), HWIO_SB_MGR_RX_MSGn_RMSK)
#define HWIO_SB_MGR_RX_MSGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_MGR_RX_MSGn_ADDR(base,n), mask)
#define HWIO_SB_MGR_RX_MSGn_MSGVAL_BMSK                                                        0xffffffff
#define HWIO_SB_MGR_RX_MSGn_MSGVAL_SHFT                                                               0x0

#define HWIO_SB_MGR_IE_STATUS_ADDR(x)                                                          ((x) + 0x000002f0)
#define HWIO_SB_MGR_IE_STATUS_RMSK                                                             0xffff000d
#define HWIO_SB_MGR_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_IE_STATUS_ADDR(x), HWIO_SB_MGR_IE_STATUS_RMSK)
#define HWIO_SB_MGR_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_IE_STATUS_ADDR(x), m)
#define HWIO_SB_MGR_IE_STATUS_DEV_CLASS_CODE_BMSK                                              0xff000000
#define HWIO_SB_MGR_IE_STATUS_DEV_CLASS_CODE_SHFT                                                    0x18
#define HWIO_SB_MGR_IE_STATUS_DEV_CLASS_VER_BMSK                                                 0xff0000
#define HWIO_SB_MGR_IE_STATUS_DEV_CLASS_VER_SHFT                                                     0x10
#define HWIO_SB_MGR_IE_STATUS_EX_ERROR_BMSK                                                           0x8
#define HWIO_SB_MGR_IE_STATUS_EX_ERROR_SHFT                                                           0x3
#define HWIO_SB_MGR_IE_STATUS_RECONFIG_OBJ_BMSK                                                       0x4
#define HWIO_SB_MGR_IE_STATUS_RECONFIG_OBJ_SHFT                                                       0x2
#define HWIO_SB_MGR_IE_STATUS_UNSPRTD_MSG_BMSK                                                        0x1
#define HWIO_SB_MGR_IE_STATUS_UNSPRTD_MSG_SHFT                                                        0x0

#define HWIO_SB_MGR_VE_STATUS_ADDR(x)                                                          ((x) + 0x00000300)
#define HWIO_SB_MGR_VE_STATUS_RMSK                                                             0xffffffff
#define HWIO_SB_MGR_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_MGR_VE_STATUS_ADDR(x), HWIO_SB_MGR_VE_STATUS_RMSK)
#define HWIO_SB_MGR_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_MGR_VE_STATUS_ADDR(x), m)
#define HWIO_SB_MGR_VE_STATUS_VE_VAL_BMSK                                                      0xffffffff
#define HWIO_SB_MGR_VE_STATUS_VE_VAL_SHFT                                                             0x0

#define HWIO_SB_FRM_CFG_ADDR(x)                                                                ((x) + 0x00000400)
#define HWIO_SB_FRM_CFG_RMSK                                                                   0xffffffff
#define HWIO_SB_FRM_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_CFG_ADDR(x), HWIO_SB_FRM_CFG_RMSK)
#define HWIO_SB_FRM_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_CFG_ADDR(x), m)
#define HWIO_SB_FRM_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_FRM_CFG_ADDR(x),v)
#define HWIO_SB_FRM_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_FRM_CFG_ADDR(x),m,v,HWIO_SB_FRM_CFG_IN(x))
#define HWIO_SB_FRM_CFG_CNF_BIT_0_BMSK                                                         0x80000000
#define HWIO_SB_FRM_CFG_CNF_BIT_0_SHFT                                                               0x1f
#define HWIO_SB_FRM_CFG_CNF_BIT_1_BMSK                                                         0x40000000
#define HWIO_SB_FRM_CFG_CNF_BIT_1_SHFT                                                               0x1e
#define HWIO_SB_FRM_CFG_CNF_BIT_2_BMSK                                                         0x20000000
#define HWIO_SB_FRM_CFG_CNF_BIT_2_SHFT                                                               0x1d
#define HWIO_SB_FRM_CFG_CNF_BIT_3_BMSK                                                         0x10000000
#define HWIO_SB_FRM_CFG_CNF_BIT_3_SHFT                                                               0x1c
#define HWIO_SB_FRM_CFG_CNF_BIT_4_BMSK                                                          0x8000000
#define HWIO_SB_FRM_CFG_CNF_BIT_4_SHFT                                                               0x1b
#define HWIO_SB_FRM_CFG_DRIFTFREE_CDIV_EN_BMSK                                                  0x4000000
#define HWIO_SB_FRM_CFG_DRIFTFREE_CDIV_EN_SHFT                                                       0x1a
#define HWIO_SB_FRM_CFG_SEL_INTFB_CAPTURE_CLK_BMSK                                              0x2000000
#define HWIO_SB_FRM_CFG_SEL_INTFB_CAPTURE_CLK_SHFT                                                   0x19
#define HWIO_SB_FRM_CFG_CDL_TIMEOUT_BMSK                                                        0x1c00000
#define HWIO_SB_FRM_CFG_CDL_TIMEOUT_SHFT                                                             0x16
#define HWIO_SB_FRM_CFG_CLK_QUALITY_BMSK                                                         0x300000
#define HWIO_SB_FRM_CFG_CLK_QUALITY_SHFT                                                             0x14
#define HWIO_SB_FRM_CFG_INTERNAL_WAKEUP_EN_BMSK                                                   0x80000
#define HWIO_SB_FRM_CFG_INTERNAL_WAKEUP_EN_SHFT                                                      0x13
#define HWIO_SB_FRM_CFG_REFERENCE_CLK_CG_BMSK                                                     0x78000
#define HWIO_SB_FRM_CFG_REFERENCE_CLK_CG_SHFT                                                         0xf
#define HWIO_SB_FRM_CFG_BOOT_ROOT_FREQ_BMSK                                                        0x7800
#define HWIO_SB_FRM_CFG_BOOT_ROOT_FREQ_SHFT                                                           0xb
#define HWIO_SB_FRM_CFG_BOOT_CLOCK_GEAR_BMSK                                                        0x780
#define HWIO_SB_FRM_CFG_BOOT_CLOCK_GEAR_SHFT                                                          0x7
#define HWIO_SB_FRM_CFG_BOOT_SUBFRAME_MODE_BMSK                                                      0x7c
#define HWIO_SB_FRM_CFG_BOOT_SUBFRAME_MODE_SHFT                                                       0x2
#define HWIO_SB_FRM_CFG_BOOT_AS_ACTIVE_FRAMER_BMSK                                                    0x2
#define HWIO_SB_FRM_CFG_BOOT_AS_ACTIVE_FRAMER_SHFT                                                    0x1
#define HWIO_SB_FRM_CFG_ENABLE_BMSK                                                                   0x1
#define HWIO_SB_FRM_CFG_ENABLE_SHFT                                                                   0x0

#define HWIO_SB_FRM_STATUS_ADDR(x)                                                             ((x) + 0x00000404)
#define HWIO_SB_FRM_STATUS_RMSK                                                                 0x7ffffff
#define HWIO_SB_FRM_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_STATUS_ADDR(x), HWIO_SB_FRM_STATUS_RMSK)
#define HWIO_SB_FRM_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_STATUS_ADDR(x), m)
#define HWIO_SB_FRM_STATUS_DEV_LA_BMSK                                                          0x7f80000
#define HWIO_SB_FRM_STATUS_DEV_LA_SHFT                                                               0x13
#define HWIO_SB_FRM_STATUS_ENUMERATED_BMSK                                                        0x40000
#define HWIO_SB_FRM_STATUS_ENUMERATED_SHFT                                                           0x12
#define HWIO_SB_FRM_STATUS_IDLE_MODE_BMSK                                                         0x20000
#define HWIO_SB_FRM_STATUS_IDLE_MODE_SHFT                                                            0x11
#define HWIO_SB_FRM_STATUS_ACTIVE_FRAMER_BMSK                                                     0x10000
#define HWIO_SB_FRM_STATUS_ACTIVE_FRAMER_SHFT                                                        0x10
#define HWIO_SB_FRM_STATUS_CUR_ROOT_FREQUENCY_BMSK                                                 0xf000
#define HWIO_SB_FRM_STATUS_CUR_ROOT_FREQUENCY_SHFT                                                    0xc
#define HWIO_SB_FRM_STATUS_CUR_CLOCK_GEAR_BMSK                                                      0xf00
#define HWIO_SB_FRM_STATUS_CUR_CLOCK_GEAR_SHFT                                                        0x8
#define HWIO_SB_FRM_STATUS_ERROR_BMSK                                                                0xc0
#define HWIO_SB_FRM_STATUS_ERROR_SHFT                                                                 0x6
#define HWIO_SB_FRM_STATUS_CUR_SUBFRAME_MODE_BMSK                                                    0x3e
#define HWIO_SB_FRM_STATUS_CUR_SUBFRAME_MODE_SHFT                                                     0x1
#define HWIO_SB_FRM_STATUS_FRAMER_BOOTED_BMSK                                                         0x1
#define HWIO_SB_FRM_STATUS_FRAMER_BOOTED_SHFT                                                         0x0

#define HWIO_SB_FRM_INT_EN_ADDR(x)                                                             ((x) + 0x00000410)
#define HWIO_SB_FRM_INT_EN_RMSK                                                                      0x7f
#define HWIO_SB_FRM_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_INT_EN_ADDR(x), HWIO_SB_FRM_INT_EN_RMSK)
#define HWIO_SB_FRM_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_INT_EN_ADDR(x), m)
#define HWIO_SB_FRM_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_FRM_INT_EN_ADDR(x),v)
#define HWIO_SB_FRM_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_FRM_INT_EN_ADDR(x),m,v,HWIO_SB_FRM_INT_EN_IN(x))
#define HWIO_SB_FRM_INT_EN_CHANGED_ROOT_FREQUENCY_BMSK                                               0x40
#define HWIO_SB_FRM_INT_EN_CHANGED_ROOT_FREQUENCY_SHFT                                                0x6
#define HWIO_SB_FRM_INT_EN_DATA_LINE_TOGGLE_BMSK                                                     0x20
#define HWIO_SB_FRM_INT_EN_DATA_LINE_TOGGLE_SHFT                                                      0x5
#define HWIO_SB_FRM_INT_EN_CDL_TIMEDOUT_BMSK                                                         0x10
#define HWIO_SB_FRM_INT_EN_CDL_TIMEDOUT_SHFT                                                          0x4
#define HWIO_SB_FRM_INT_EN_IDLE_MODE_EXITED_BMSK                                                      0x8
#define HWIO_SB_FRM_INT_EN_IDLE_MODE_EXITED_SHFT                                                      0x3
#define HWIO_SB_FRM_INT_EN_FRAMER_BOOT_COMPLETE_BMSK                                                  0x4
#define HWIO_SB_FRM_INT_EN_FRAMER_BOOT_COMPLETE_SHFT                                                  0x2
#define HWIO_SB_FRM_INT_EN_IE_VE_CHANGE_BMSK                                                          0x2
#define HWIO_SB_FRM_INT_EN_IE_VE_CHANGE_SHFT                                                          0x1
#define HWIO_SB_FRM_INT_EN_DEV_ERR_BMSK                                                               0x1
#define HWIO_SB_FRM_INT_EN_DEV_ERR_SHFT                                                               0x0

#define HWIO_SB_FRM_INT_STATUS_ADDR(x)                                                         ((x) + 0x00000414)
#define HWIO_SB_FRM_INT_STATUS_RMSK                                                                  0x7f
#define HWIO_SB_FRM_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_INT_STATUS_ADDR(x), HWIO_SB_FRM_INT_STATUS_RMSK)
#define HWIO_SB_FRM_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_INT_STATUS_ADDR(x), m)
#define HWIO_SB_FRM_INT_STATUS_CHANGED_ROOT_FREQUENCY_BMSK                                           0x40
#define HWIO_SB_FRM_INT_STATUS_CHANGED_ROOT_FREQUENCY_SHFT                                            0x6
#define HWIO_SB_FRM_INT_STATUS_DATA_LINE_TOGGLE_BMSK                                                 0x20
#define HWIO_SB_FRM_INT_STATUS_DATA_LINE_TOGGLE_SHFT                                                  0x5
#define HWIO_SB_FRM_INT_STATUS_CDL_TIMEDOUT_BMSK                                                     0x10
#define HWIO_SB_FRM_INT_STATUS_CDL_TIMEDOUT_SHFT                                                      0x4
#define HWIO_SB_FRM_INT_STATUS_IDLE_MODE_EXITED_BMSK                                                  0x8
#define HWIO_SB_FRM_INT_STATUS_IDLE_MODE_EXITED_SHFT                                                  0x3
#define HWIO_SB_FRM_INT_STATUS_FRAMER_BOOT_COMPLETE_BMSK                                              0x4
#define HWIO_SB_FRM_INT_STATUS_FRAMER_BOOT_COMPLETE_SHFT                                              0x2
#define HWIO_SB_FRM_INT_STATUS_IE_VE_CHANGE_BMSK                                                      0x2
#define HWIO_SB_FRM_INT_STATUS_IE_VE_CHANGE_SHFT                                                      0x1
#define HWIO_SB_FRM_INT_STATUS_DEV_ERR_BMSK                                                           0x1
#define HWIO_SB_FRM_INT_STATUS_DEV_ERR_SHFT                                                           0x0

#define HWIO_SB_FRM_INT_CLR_ADDR(x)                                                            ((x) + 0x00000418)
#define HWIO_SB_FRM_INT_CLR_RMSK                                                                     0x7f
#define HWIO_SB_FRM_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_FRM_INT_CLR_ADDR(x),v)
#define HWIO_SB_FRM_INT_CLR_CHANGED_ROOT_FREQUENCY_BMSK                                              0x40
#define HWIO_SB_FRM_INT_CLR_CHANGED_ROOT_FREQUENCY_SHFT                                               0x6
#define HWIO_SB_FRM_INT_CLR_DATA_LINE_TOGGLE_BMSK                                                    0x20
#define HWIO_SB_FRM_INT_CLR_DATA_LINE_TOGGLE_SHFT                                                     0x5
#define HWIO_SB_FRM_INT_CLR_CDL_TIMEDOUT_BMSK                                                        0x10
#define HWIO_SB_FRM_INT_CLR_CDL_TIMEDOUT_SHFT                                                         0x4
#define HWIO_SB_FRM_INT_CLR_IDLE_MODE_EXITED_BMSK                                                     0x8
#define HWIO_SB_FRM_INT_CLR_IDLE_MODE_EXITED_SHFT                                                     0x3
#define HWIO_SB_FRM_INT_CLR_FRAMER_BOOT_COMPLETE_BMSK                                                 0x4
#define HWIO_SB_FRM_INT_CLR_FRAMER_BOOT_COMPLETE_SHFT                                                 0x2
#define HWIO_SB_FRM_INT_CLR_IE_VE_CHANGE_BMSK                                                         0x2
#define HWIO_SB_FRM_INT_CLR_IE_VE_CHANGE_SHFT                                                         0x1
#define HWIO_SB_FRM_INT_CLR_DEV_ERR_BMSK                                                              0x1
#define HWIO_SB_FRM_INT_CLR_DEV_ERR_SHFT                                                              0x0

#define HWIO_SB_FRM_WAKEUP_ADDR(x)                                                             ((x) + 0x0000041c)
#define HWIO_SB_FRM_WAKEUP_RMSK                                                                       0x1
#define HWIO_SB_FRM_WAKEUP_OUT(x, v)      \
        out_dword(HWIO_SB_FRM_WAKEUP_ADDR(x),v)
#define HWIO_SB_FRM_WAKEUP_WAKEUP_NOW_BMSK                                                            0x1
#define HWIO_SB_FRM_WAKEUP_WAKEUP_NOW_SHFT                                                            0x0

#define HWIO_SB_FRM_CLKCTL_DONE_ADDR(x)                                                        ((x) + 0x00000420)
#define HWIO_SB_FRM_CLKCTL_DONE_RMSK                                                                  0x1
#define HWIO_SB_FRM_CLKCTL_DONE_OUT(x, v)      \
        out_dword(HWIO_SB_FRM_CLKCTL_DONE_ADDR(x),v)
#define HWIO_SB_FRM_CLKCTL_DONE_CLKCTL_DONE_BMSK                                                      0x1
#define HWIO_SB_FRM_CLKCTL_DONE_CLKCTL_DONE_SHFT                                                      0x0

#define HWIO_SB_FRM_IE_STATUS_ADDR(x)                                                          ((x) + 0x00000430)
#define HWIO_SB_FRM_IE_STATUS_RMSK                                                             0xffff03fd
#define HWIO_SB_FRM_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_IE_STATUS_ADDR(x), HWIO_SB_FRM_IE_STATUS_RMSK)
#define HWIO_SB_FRM_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_IE_STATUS_ADDR(x), m)
#define HWIO_SB_FRM_IE_STATUS_DEV_CLASS_CODE_BMSK                                              0xff000000
#define HWIO_SB_FRM_IE_STATUS_DEV_CLASS_CODE_SHFT                                                    0x18
#define HWIO_SB_FRM_IE_STATUS_DEV_CLASS_VER_BMSK                                                 0xff0000
#define HWIO_SB_FRM_IE_STATUS_DEV_CLASS_VER_SHFT                                                     0x10
#define HWIO_SB_FRM_IE_STATUS_QUAL_BMSK                                                             0x300
#define HWIO_SB_FRM_IE_STATUS_QUAL_SHFT                                                               0x8
#define HWIO_SB_FRM_IE_STATUS_GC_TX_COL_BMSK                                                         0x80
#define HWIO_SB_FRM_IE_STATUS_GC_TX_COL_SHFT                                                          0x7
#define HWIO_SB_FRM_IE_STATUS_FI_TX_COL_BMSK                                                         0x40
#define HWIO_SB_FRM_IE_STATUS_FI_TX_COL_SHFT                                                          0x6
#define HWIO_SB_FRM_IE_STATUS_FS_TX_COL_BMSK                                                         0x20
#define HWIO_SB_FRM_IE_STATUS_FS_TX_COL_SHFT                                                          0x5
#define HWIO_SB_FRM_IE_STATUS_ACTIVE_FRAMER_BMSK                                                     0x10
#define HWIO_SB_FRM_IE_STATUS_ACTIVE_FRAMER_SHFT                                                      0x4
#define HWIO_SB_FRM_IE_STATUS_EX_ERROR_BMSK                                                           0x8
#define HWIO_SB_FRM_IE_STATUS_EX_ERROR_SHFT                                                           0x3
#define HWIO_SB_FRM_IE_STATUS_RECONFIG_OBJ_BMSK                                                       0x4
#define HWIO_SB_FRM_IE_STATUS_RECONFIG_OBJ_SHFT                                                       0x2
#define HWIO_SB_FRM_IE_STATUS_UNSPRTD_MSG_BMSK                                                        0x1
#define HWIO_SB_FRM_IE_STATUS_UNSPRTD_MSG_SHFT                                                        0x0

#define HWIO_SB_FRM_VE_STATUS_ADDR(x)                                                          ((x) + 0x00000440)
#define HWIO_SB_FRM_VE_STATUS_RMSK                                                             0xffffffff
#define HWIO_SB_FRM_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_FRM_VE_STATUS_ADDR(x), HWIO_SB_FRM_VE_STATUS_RMSK)
#define HWIO_SB_FRM_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_FRM_VE_STATUS_ADDR(x), m)
#define HWIO_SB_FRM_VE_STATUS_VE_VAL_BMSK                                                      0xffffffff
#define HWIO_SB_FRM_VE_STATUS_VE_VAL_SHFT                                                             0x0

#define HWIO_SB_INTF_CFG_ADDR(x)                                                               ((x) + 0x00000600)
#define HWIO_SB_INTF_CFG_RMSK                                                                         0x3
#define HWIO_SB_INTF_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_CFG_ADDR(x), HWIO_SB_INTF_CFG_RMSK)
#define HWIO_SB_INTF_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_CFG_ADDR(x), m)
#define HWIO_SB_INTF_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_INTF_CFG_ADDR(x),v)
#define HWIO_SB_INTF_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_INTF_CFG_ADDR(x),m,v,HWIO_SB_INTF_CFG_IN(x))
#define HWIO_SB_INTF_CFG_NEXT_PARAM_FIX_ENABLE_BMSK                                                   0x2
#define HWIO_SB_INTF_CFG_NEXT_PARAM_FIX_ENABLE_SHFT                                                   0x1
#define HWIO_SB_INTF_CFG_ENABLE_BMSK                                                                  0x1
#define HWIO_SB_INTF_CFG_ENABLE_SHFT                                                                  0x0

#define HWIO_SB_INTF_STATUS_ADDR(x)                                                            ((x) + 0x00000604)
#define HWIO_SB_INTF_STATUS_RMSK                                                                   0x3fff
#define HWIO_SB_INTF_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_STATUS_ADDR(x), HWIO_SB_INTF_STATUS_RMSK)
#define HWIO_SB_INTF_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_STATUS_ADDR(x), m)
#define HWIO_SB_INTF_STATUS_MESSAGE_SYNC_ACQUIRED_BMSK                                             0x2000
#define HWIO_SB_INTF_STATUS_MESSAGE_SYNC_ACQUIRED_SHFT                                                0xd
#define HWIO_SB_INTF_STATUS_SUPERFRAME_SYNC_ACQUIRED_BMSK                                          0x1000
#define HWIO_SB_INTF_STATUS_SUPERFRAME_SYNC_ACQUIRED_SHFT                                             0xc
#define HWIO_SB_INTF_STATUS_FRAME_SYNC_ACQUIRED_BMSK                                                0x800
#define HWIO_SB_INTF_STATUS_FRAME_SYNC_ACQUIRED_SHFT                                                  0xb
#define HWIO_SB_INTF_STATUS_DEV_LA_BMSK                                                             0x7f8
#define HWIO_SB_INTF_STATUS_DEV_LA_SHFT                                                               0x3
#define HWIO_SB_INTF_STATUS_ENUMERATED_BMSK                                                           0x4
#define HWIO_SB_INTF_STATUS_ENUMERATED_SHFT                                                           0x2
#define HWIO_SB_INTF_STATUS_ERROR_BMSK                                                                0x3
#define HWIO_SB_INTF_STATUS_ERROR_SHFT                                                                0x0

#define HWIO_SB_INTF_INT_EN_ADDR(x)                                                            ((x) + 0x00000610)
#define HWIO_SB_INTF_INT_EN_RMSK                                                                      0x7
#define HWIO_SB_INTF_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_INT_EN_ADDR(x), HWIO_SB_INTF_INT_EN_RMSK)
#define HWIO_SB_INTF_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_INT_EN_ADDR(x), m)
#define HWIO_SB_INTF_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_INTF_INT_EN_ADDR(x),v)
#define HWIO_SB_INTF_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_INTF_INT_EN_ADDR(x),m,v,HWIO_SB_INTF_INT_EN_IN(x))
#define HWIO_SB_INTF_INT_EN_NO_DATA_LINE_ACTIVITY_BMSK                                                0x4
#define HWIO_SB_INTF_INT_EN_NO_DATA_LINE_ACTIVITY_SHFT                                                0x2
#define HWIO_SB_INTF_INT_EN_IE_VE_CHANGE_BMSK                                                         0x2
#define HWIO_SB_INTF_INT_EN_IE_VE_CHANGE_SHFT                                                         0x1
#define HWIO_SB_INTF_INT_EN_DEV_ERR_BMSK                                                              0x1
#define HWIO_SB_INTF_INT_EN_DEV_ERR_SHFT                                                              0x0

#define HWIO_SB_INTF_INT_STATUS_ADDR(x)                                                        ((x) + 0x00000614)
#define HWIO_SB_INTF_INT_STATUS_RMSK                                                                  0x7
#define HWIO_SB_INTF_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_INT_STATUS_ADDR(x), HWIO_SB_INTF_INT_STATUS_RMSK)
#define HWIO_SB_INTF_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_INT_STATUS_ADDR(x), m)
#define HWIO_SB_INTF_INT_STATUS_NO_DATA_LINE_ACTIVITY_BMSK                                            0x4
#define HWIO_SB_INTF_INT_STATUS_NO_DATA_LINE_ACTIVITY_SHFT                                            0x2
#define HWIO_SB_INTF_INT_STATUS_IE_VE_CHANGE_BMSK                                                     0x2
#define HWIO_SB_INTF_INT_STATUS_IE_VE_CHANGE_SHFT                                                     0x1
#define HWIO_SB_INTF_INT_STATUS_DEV_ERR_BMSK                                                          0x1
#define HWIO_SB_INTF_INT_STATUS_DEV_ERR_SHFT                                                          0x0

#define HWIO_SB_INTF_INT_CLR_ADDR(x)                                                           ((x) + 0x00000618)
#define HWIO_SB_INTF_INT_CLR_RMSK                                                                     0x7
#define HWIO_SB_INTF_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_INTF_INT_CLR_ADDR(x),v)
#define HWIO_SB_INTF_INT_CLR_NO_DATA_LINE_ACTIVITY_BMSK                                               0x4
#define HWIO_SB_INTF_INT_CLR_NO_DATA_LINE_ACTIVITY_SHFT                                               0x2
#define HWIO_SB_INTF_INT_CLR_IE_VE_CHANGE_BMSK                                                        0x2
#define HWIO_SB_INTF_INT_CLR_IE_VE_CHANGE_SHFT                                                        0x1
#define HWIO_SB_INTF_INT_CLR_DEV_ERR_BMSK                                                             0x1
#define HWIO_SB_INTF_INT_CLR_DEV_ERR_SHFT                                                             0x0

#define HWIO_SB_INTF_IE_STATUS_ADDR(x)                                                         ((x) + 0x00000630)
#define HWIO_SB_INTF_IE_STATUS_RMSK                                                            0xffff0fff
#define HWIO_SB_INTF_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_IE_STATUS_ADDR(x), HWIO_SB_INTF_IE_STATUS_RMSK)
#define HWIO_SB_INTF_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_IE_STATUS_ADDR(x), m)
#define HWIO_SB_INTF_IE_STATUS_DEV_CLASS_CODE_BMSK                                             0xff000000
#define HWIO_SB_INTF_IE_STATUS_DEV_CLASS_CODE_SHFT                                                   0x18
#define HWIO_SB_INTF_IE_STATUS_DEV_CLASS_VER_BMSK                                                0xff0000
#define HWIO_SB_INTF_IE_STATUS_DEV_CLASS_VER_SHFT                                                    0x10
#define HWIO_SB_INTF_IE_STATUS_MC_P_BIT_COHERENCE_ERR_BMSK                                          0x800
#define HWIO_SB_INTF_IE_STATUS_MC_P_BIT_COHERENCE_ERR_SHFT                                            0xb
#define HWIO_SB_INTF_IE_STATUS_MC_SRQ_BIT_COHERENCE_ERR_BMSK                                        0x400
#define HWIO_SB_INTF_IE_STATUS_MC_SRQ_BIT_COHERENCE_ERR_SHFT                                          0xa
#define HWIO_SB_INTF_IE_STATUS_SRQ_P_MISMATCH_ERR_BMSK                                              0x200
#define HWIO_SB_INTF_IE_STATUS_SRQ_P_MISMATCH_ERR_SHFT                                                0x9
#define HWIO_SB_INTF_IE_STATUS_DS_OVERLAP_BMSK                                                      0x100
#define HWIO_SB_INTF_IE_STATUS_DS_OVERLAP_SHFT                                                        0x8
#define HWIO_SB_INTF_IE_STATUS_LOST_MS_BMSK                                                          0x80
#define HWIO_SB_INTF_IE_STATUS_LOST_MS_SHFT                                                           0x7
#define HWIO_SB_INTF_IE_STATUS_LOST_SFS_BMSK                                                         0x40
#define HWIO_SB_INTF_IE_STATUS_LOST_SFS_SHFT                                                          0x6
#define HWIO_SB_INTF_IE_STATUS_LOST_FS_BMSK                                                          0x20
#define HWIO_SB_INTF_IE_STATUS_LOST_FS_SHFT                                                           0x5
#define HWIO_SB_INTF_IE_STATUS_MC_TX_COL_BMSK                                                        0x10
#define HWIO_SB_INTF_IE_STATUS_MC_TX_COL_SHFT                                                         0x4
#define HWIO_SB_INTF_IE_STATUS_EX_ERROR_BMSK                                                          0x8
#define HWIO_SB_INTF_IE_STATUS_EX_ERROR_SHFT                                                          0x3
#define HWIO_SB_INTF_IE_STATUS_RECONFIG_OBJ_BMSK                                                      0x4
#define HWIO_SB_INTF_IE_STATUS_RECONFIG_OBJ_SHFT                                                      0x2
#define HWIO_SB_INTF_IE_STATUS_DATA_TX_COL_BMSK                                                       0x2
#define HWIO_SB_INTF_IE_STATUS_DATA_TX_COL_SHFT                                                       0x1
#define HWIO_SB_INTF_IE_STATUS_UNSPRTD_MSG_BMSK                                                       0x1
#define HWIO_SB_INTF_IE_STATUS_UNSPRTD_MSG_SHFT                                                       0x0

#define HWIO_SB_INTF_VE_STATUS_ADDR(x)                                                         ((x) + 0x00000640)
#define HWIO_SB_INTF_VE_STATUS_RMSK                                                            0xffffffff
#define HWIO_SB_INTF_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_INTF_VE_STATUS_ADDR(x), HWIO_SB_INTF_VE_STATUS_RMSK)
#define HWIO_SB_INTF_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_INTF_VE_STATUS_ADDR(x), m)
#define HWIO_SB_INTF_VE_STATUS_VE_VAL_BMSK                                                     0xffffffff
#define HWIO_SB_INTF_VE_STATUS_VE_VAL_SHFT                                                            0x0

#define HWIO_SB_NGD1_CFG_ADDR(x)                                                               ((x) + 0x00001000)
#define HWIO_SB_NGD1_CFG_RMSK                                                                         0x7
#define HWIO_SB_NGD1_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_CFG_ADDR(x), HWIO_SB_NGD1_CFG_RMSK)
#define HWIO_SB_NGD1_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_CFG_ADDR(x), m)
#define HWIO_SB_NGD1_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_NGD1_CFG_ADDR(x),v)
#define HWIO_SB_NGD1_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD1_CFG_ADDR(x),m,v,HWIO_SB_NGD1_CFG_IN(x))
#define HWIO_SB_NGD1_CFG_TX_MSGQ_EN_BMSK                                                              0x4
#define HWIO_SB_NGD1_CFG_TX_MSGQ_EN_SHFT                                                              0x2
#define HWIO_SB_NGD1_CFG_RX_MSGQ_EN_BMSK                                                              0x2
#define HWIO_SB_NGD1_CFG_RX_MSGQ_EN_SHFT                                                              0x1
#define HWIO_SB_NGD1_CFG_ENABLE_BMSK                                                                  0x1
#define HWIO_SB_NGD1_CFG_ENABLE_SHFT                                                                  0x0

#define HWIO_SB_NGD1_STATUS_ADDR(x)                                                            ((x) + 0x00001004)
#define HWIO_SB_NGD1_STATUS_RMSK                                                                0x7ffffff
#define HWIO_SB_NGD1_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_STATUS_ADDR(x), HWIO_SB_NGD1_STATUS_RMSK)
#define HWIO_SB_NGD1_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_STATUS_ADDR(x), m)
#define HWIO_SB_NGD1_STATUS_NACKED_TX_MSG_MC_BMSK                                               0x7f00000
#define HWIO_SB_NGD1_STATUS_NACKED_TX_MSG_MC_SHFT                                                    0x14
#define HWIO_SB_NGD1_STATUS_ACKED_TX_MSG_MC_BMSK                                                  0xfe000
#define HWIO_SB_NGD1_STATUS_ACKED_TX_MSG_MC_SHFT                                                      0xd
#define HWIO_SB_NGD1_STATUS_ERROR_BMSK                                                             0x1800
#define HWIO_SB_NGD1_STATUS_ERROR_SHFT                                                                0xb
#define HWIO_SB_NGD1_STATUS_MSGQ_SUPPORT_BMSK                                                       0x400
#define HWIO_SB_NGD1_STATUS_MSGQ_SUPPORT_SHFT                                                         0xa
#define HWIO_SB_NGD1_STATUS_PIPE_NUM_OFFSET_BMSK                                                    0x3fc
#define HWIO_SB_NGD1_STATUS_PIPE_NUM_OFFSET_SHFT                                                      0x2
#define HWIO_SB_NGD1_STATUS_ENUMERATED_BMSK                                                           0x2
#define HWIO_SB_NGD1_STATUS_ENUMERATED_SHFT                                                           0x1
#define HWIO_SB_NGD1_STATUS_TX_MSG_BUFFER_BUSY_BMSK                                                   0x1
#define HWIO_SB_NGD1_STATUS_TX_MSG_BUFFER_BUSY_SHFT                                                   0x0

#define HWIO_SB_NGD1_RX_MSGQ_CFG_ADDR(x)                                                       ((x) + 0x00001008)
#define HWIO_SB_NGD1_RX_MSGQ_CFG_RMSK                                                          0xffffffff
#define HWIO_SB_NGD1_RX_MSGQ_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_RX_MSGQ_CFG_ADDR(x), HWIO_SB_NGD1_RX_MSGQ_CFG_RMSK)
#define HWIO_SB_NGD1_RX_MSGQ_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_RX_MSGQ_CFG_ADDR(x), m)
#define HWIO_SB_NGD1_RX_MSGQ_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_NGD1_RX_MSGQ_CFG_ADDR(x),v)
#define HWIO_SB_NGD1_RX_MSGQ_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD1_RX_MSGQ_CFG_ADDR(x),m,v,HWIO_SB_NGD1_RX_MSGQ_CFG_IN(x))
#define HWIO_SB_NGD1_RX_MSGQ_CFG_TIME_OUT_VAL_BMSK                                             0xffff0000
#define HWIO_SB_NGD1_RX_MSGQ_CFG_TIME_OUT_VAL_SHFT                                                   0x10
#define HWIO_SB_NGD1_RX_MSGQ_CFG_TRANS_SIZE_BMSK                                                   0xff00
#define HWIO_SB_NGD1_RX_MSGQ_CFG_TRANS_SIZE_SHFT                                                      0x8
#define HWIO_SB_NGD1_RX_MSGQ_CFG_BLOCK_SIZE_BMSK                                                     0xff
#define HWIO_SB_NGD1_RX_MSGQ_CFG_BLOCK_SIZE_SHFT                                                      0x0

#define HWIO_SB_NGD1_INT_EN_ADDR(x)                                                            ((x) + 0x00001010)
#define HWIO_SB_NGD1_INT_EN_RMSK                                                               0xff000000
#define HWIO_SB_NGD1_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_INT_EN_ADDR(x), HWIO_SB_NGD1_INT_EN_RMSK)
#define HWIO_SB_NGD1_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_INT_EN_ADDR(x), m)
#define HWIO_SB_NGD1_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_NGD1_INT_EN_ADDR(x),v)
#define HWIO_SB_NGD1_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD1_INT_EN_ADDR(x),m,v,HWIO_SB_NGD1_INT_EN_IN(x))
#define HWIO_SB_NGD1_INT_EN_TX_MSG_SENT_BMSK                                                   0x80000000
#define HWIO_SB_NGD1_INT_EN_TX_MSG_SENT_SHFT                                                         0x1f
#define HWIO_SB_NGD1_INT_EN_RX_MSG_RCVD_BMSK                                                   0x40000000
#define HWIO_SB_NGD1_INT_EN_RX_MSG_RCVD_SHFT                                                         0x1e
#define HWIO_SB_NGD1_INT_EN_DEV_ERR_BMSK                                                       0x20000000
#define HWIO_SB_NGD1_INT_EN_DEV_ERR_SHFT                                                             0x1d
#define HWIO_SB_NGD1_INT_EN_IE_VE_CHANGE_BMSK                                                  0x10000000
#define HWIO_SB_NGD1_INT_EN_IE_VE_CHANGE_SHFT                                                        0x1c
#define HWIO_SB_NGD1_INT_EN_INVALID_TX_MSG_BMSK                                                 0x8000000
#define HWIO_SB_NGD1_INT_EN_INVALID_TX_MSG_SHFT                                                      0x1b
#define HWIO_SB_NGD1_INT_EN_TX_MSG_BUF_CONTENTION_BMSK                                          0x4000000
#define HWIO_SB_NGD1_INT_EN_TX_MSG_BUF_CONTENTION_SHFT                                               0x1a
#define HWIO_SB_NGD1_INT_EN_TX_MSG_NACKED_BMSK                                                  0x2000000
#define HWIO_SB_NGD1_INT_EN_TX_MSG_NACKED_SHFT                                                       0x19
#define HWIO_SB_NGD1_INT_EN_RECONFIG_DONE_BMSK                                                  0x1000000
#define HWIO_SB_NGD1_INT_EN_RECONFIG_DONE_SHFT                                                       0x18

#define HWIO_SB_NGD1_INT_STATUS_ADDR(x)                                                        ((x) + 0x00001014)
#define HWIO_SB_NGD1_INT_STATUS_RMSK                                                           0xff000000
#define HWIO_SB_NGD1_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_INT_STATUS_ADDR(x), HWIO_SB_NGD1_INT_STATUS_RMSK)
#define HWIO_SB_NGD1_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_INT_STATUS_ADDR(x), m)
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_SENT_BMSK                                               0x80000000
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_SENT_SHFT                                                     0x1f
#define HWIO_SB_NGD1_INT_STATUS_RX_MSG_RCVD_BMSK                                               0x40000000
#define HWIO_SB_NGD1_INT_STATUS_RX_MSG_RCVD_SHFT                                                     0x1e
#define HWIO_SB_NGD1_INT_STATUS_DEV_ERR_BMSK                                                   0x20000000
#define HWIO_SB_NGD1_INT_STATUS_DEV_ERR_SHFT                                                         0x1d
#define HWIO_SB_NGD1_INT_STATUS_IE_VE_CHANGE_BMSK                                              0x10000000
#define HWIO_SB_NGD1_INT_STATUS_IE_VE_CHANGE_SHFT                                                    0x1c
#define HWIO_SB_NGD1_INT_STATUS_INVALID_TX_MSG_BMSK                                             0x8000000
#define HWIO_SB_NGD1_INT_STATUS_INVALID_TX_MSG_SHFT                                                  0x1b
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_BUF_CONTENTION_BMSK                                      0x4000000
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_BUF_CONTENTION_SHFT                                           0x1a
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_NACKED_BMSK                                              0x2000000
#define HWIO_SB_NGD1_INT_STATUS_TX_MSG_NACKED_SHFT                                                   0x19
#define HWIO_SB_NGD1_INT_STATUS_RECONFIG_DONE_BMSK                                              0x1000000
#define HWIO_SB_NGD1_INT_STATUS_RECONFIG_DONE_SHFT                                                   0x18

#define HWIO_SB_NGD1_INT_CLR_ADDR(x)                                                           ((x) + 0x00001018)
#define HWIO_SB_NGD1_INT_CLR_RMSK                                                              0xff000000
#define HWIO_SB_NGD1_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_NGD1_INT_CLR_ADDR(x),v)
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_SENT_BMSK                                                  0x80000000
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_SENT_SHFT                                                        0x1f
#define HWIO_SB_NGD1_INT_CLR_RX_MSG_RCVD_BMSK                                                  0x40000000
#define HWIO_SB_NGD1_INT_CLR_RX_MSG_RCVD_SHFT                                                        0x1e
#define HWIO_SB_NGD1_INT_CLR_DEV_ERR_BMSK                                                      0x20000000
#define HWIO_SB_NGD1_INT_CLR_DEV_ERR_SHFT                                                            0x1d
#define HWIO_SB_NGD1_INT_CLR_IE_VE_CHANGE_BMSK                                                 0x10000000
#define HWIO_SB_NGD1_INT_CLR_IE_VE_CHANGE_SHFT                                                       0x1c
#define HWIO_SB_NGD1_INT_CLR_INVALID_TX_MSG_BMSK                                                0x8000000
#define HWIO_SB_NGD1_INT_CLR_INVALID_TX_MSG_SHFT                                                     0x1b
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_BUF_CONTENTION_BMSK                                         0x4000000
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_BUF_CONTENTION_SHFT                                              0x1a
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_NACKED_BMSK                                                 0x2000000
#define HWIO_SB_NGD1_INT_CLR_TX_MSG_NACKED_SHFT                                                      0x19
#define HWIO_SB_NGD1_INT_CLR_RECONFIG_DONE_BMSK                                                 0x1000000
#define HWIO_SB_NGD1_INT_CLR_RECONFIG_DONE_SHFT                                                      0x18

#define HWIO_SB_NGD1_TX_MSGn_ADDR(base,n)                                                      ((base) + 0x00001030 + 0x4 * (n))
#define HWIO_SB_NGD1_TX_MSGn_RMSK                                                              0xffffffff
#define HWIO_SB_NGD1_TX_MSGn_MAXn                                                                       9
#define HWIO_SB_NGD1_TX_MSGn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_NGD1_TX_MSGn_ADDR(base,n),val)
#define HWIO_SB_NGD1_TX_MSGn_MSGVAL_BMSK                                                       0xffffffff
#define HWIO_SB_NGD1_TX_MSGn_MSGVAL_SHFT                                                              0x0

#define HWIO_SB_NGD1_RX_MSGn_ADDR(base,n)                                                      ((base) + 0x00001070 + 0x4 * (n))
#define HWIO_SB_NGD1_RX_MSGn_RMSK                                                              0xffffffff
#define HWIO_SB_NGD1_RX_MSGn_MAXn                                                                       9
#define HWIO_SB_NGD1_RX_MSGn_INI(base,n)        \
        in_dword_masked(HWIO_SB_NGD1_RX_MSGn_ADDR(base,n), HWIO_SB_NGD1_RX_MSGn_RMSK)
#define HWIO_SB_NGD1_RX_MSGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_NGD1_RX_MSGn_ADDR(base,n), mask)
#define HWIO_SB_NGD1_RX_MSGn_MSGVAL_BMSK                                                       0xffffffff
#define HWIO_SB_NGD1_RX_MSGn_MSGVAL_SHFT                                                              0x0

#define HWIO_SB_NGD1_IE_STATUS_ADDR(x)                                                         ((x) + 0x000010f0)
#define HWIO_SB_NGD1_IE_STATUS_RMSK                                                            0xffff000f
#define HWIO_SB_NGD1_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_IE_STATUS_ADDR(x), HWIO_SB_NGD1_IE_STATUS_RMSK)
#define HWIO_SB_NGD1_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_IE_STATUS_ADDR(x), m)
#define HWIO_SB_NGD1_IE_STATUS_DEV_CLASS_CODE_BMSK                                             0xff000000
#define HWIO_SB_NGD1_IE_STATUS_DEV_CLASS_CODE_SHFT                                                   0x18
#define HWIO_SB_NGD1_IE_STATUS_DEV_CLASS_VER_BMSK                                                0xff0000
#define HWIO_SB_NGD1_IE_STATUS_DEV_CLASS_VER_SHFT                                                    0x10
#define HWIO_SB_NGD1_IE_STATUS_EX_ERROR_BMSK                                                          0x8
#define HWIO_SB_NGD1_IE_STATUS_EX_ERROR_SHFT                                                          0x3
#define HWIO_SB_NGD1_IE_STATUS_RECONFIG_OBJ_BMSK                                                      0x4
#define HWIO_SB_NGD1_IE_STATUS_RECONFIG_OBJ_SHFT                                                      0x2
#define HWIO_SB_NGD1_IE_STATUS_DATA_TX_COL_BMSK                                                       0x2
#define HWIO_SB_NGD1_IE_STATUS_DATA_TX_COL_SHFT                                                       0x1
#define HWIO_SB_NGD1_IE_STATUS_UNSPRTD_MSG_BMSK                                                       0x1
#define HWIO_SB_NGD1_IE_STATUS_UNSPRTD_MSG_SHFT                                                       0x0

#define HWIO_SB_NGD1_VE_STATUS_ADDR(x)                                                         ((x) + 0x00001100)
#define HWIO_SB_NGD1_VE_STATUS_RMSK                                                            0xffffffff
#define HWIO_SB_NGD1_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD1_VE_STATUS_ADDR(x), HWIO_SB_NGD1_VE_STATUS_RMSK)
#define HWIO_SB_NGD1_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD1_VE_STATUS_ADDR(x), m)
#define HWIO_SB_NGD1_VE_STATUS_VE_VAL_BMSK                                                     0xffffffff
#define HWIO_SB_NGD1_VE_STATUS_VE_VAL_SHFT                                                            0x0

#define HWIO_SB_NGD2_CFG_ADDR(x)                                                               ((x) + 0x00002000)
#define HWIO_SB_NGD2_CFG_RMSK                                                                         0x7
#define HWIO_SB_NGD2_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_CFG_ADDR(x), HWIO_SB_NGD2_CFG_RMSK)
#define HWIO_SB_NGD2_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_CFG_ADDR(x), m)
#define HWIO_SB_NGD2_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_NGD2_CFG_ADDR(x),v)
#define HWIO_SB_NGD2_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD2_CFG_ADDR(x),m,v,HWIO_SB_NGD2_CFG_IN(x))
#define HWIO_SB_NGD2_CFG_TX_MSGQ_EN_BMSK                                                              0x4
#define HWIO_SB_NGD2_CFG_TX_MSGQ_EN_SHFT                                                              0x2
#define HWIO_SB_NGD2_CFG_RX_MSGQ_EN_BMSK                                                              0x2
#define HWIO_SB_NGD2_CFG_RX_MSGQ_EN_SHFT                                                              0x1
#define HWIO_SB_NGD2_CFG_ENABLE_BMSK                                                                  0x1
#define HWIO_SB_NGD2_CFG_ENABLE_SHFT                                                                  0x0

#define HWIO_SB_NGD2_STATUS_ADDR(x)                                                            ((x) + 0x00002004)
#define HWIO_SB_NGD2_STATUS_RMSK                                                                0x7ffffff
#define HWIO_SB_NGD2_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_STATUS_ADDR(x), HWIO_SB_NGD2_STATUS_RMSK)
#define HWIO_SB_NGD2_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_STATUS_ADDR(x), m)
#define HWIO_SB_NGD2_STATUS_NACKED_TX_MSG_MC_BMSK                                               0x7f00000
#define HWIO_SB_NGD2_STATUS_NACKED_TX_MSG_MC_SHFT                                                    0x14
#define HWIO_SB_NGD2_STATUS_ACKED_TX_MSG_MC_BMSK                                                  0xfe000
#define HWIO_SB_NGD2_STATUS_ACKED_TX_MSG_MC_SHFT                                                      0xd
#define HWIO_SB_NGD2_STATUS_ERROR_BMSK                                                             0x1800
#define HWIO_SB_NGD2_STATUS_ERROR_SHFT                                                                0xb
#define HWIO_SB_NGD2_STATUS_MSGQ_SUPPORT_BMSK                                                       0x400
#define HWIO_SB_NGD2_STATUS_MSGQ_SUPPORT_SHFT                                                         0xa
#define HWIO_SB_NGD2_STATUS_PIPE_NUM_OFFSET_BMSK                                                    0x3fc
#define HWIO_SB_NGD2_STATUS_PIPE_NUM_OFFSET_SHFT                                                      0x2
#define HWIO_SB_NGD2_STATUS_ENUMERATED_BMSK                                                           0x2
#define HWIO_SB_NGD2_STATUS_ENUMERATED_SHFT                                                           0x1
#define HWIO_SB_NGD2_STATUS_TX_MSG_BUFFER_BUSY_BMSK                                                   0x1
#define HWIO_SB_NGD2_STATUS_TX_MSG_BUFFER_BUSY_SHFT                                                   0x0

#define HWIO_SB_NGD2_RX_MSGQ_CFG_ADDR(x)                                                       ((x) + 0x00002008)
#define HWIO_SB_NGD2_RX_MSGQ_CFG_RMSK                                                          0xffffffff
#define HWIO_SB_NGD2_RX_MSGQ_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_RX_MSGQ_CFG_ADDR(x), HWIO_SB_NGD2_RX_MSGQ_CFG_RMSK)
#define HWIO_SB_NGD2_RX_MSGQ_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_RX_MSGQ_CFG_ADDR(x), m)
#define HWIO_SB_NGD2_RX_MSGQ_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_NGD2_RX_MSGQ_CFG_ADDR(x),v)
#define HWIO_SB_NGD2_RX_MSGQ_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD2_RX_MSGQ_CFG_ADDR(x),m,v,HWIO_SB_NGD2_RX_MSGQ_CFG_IN(x))
#define HWIO_SB_NGD2_RX_MSGQ_CFG_TIME_OUT_VAL_BMSK                                             0xffff0000
#define HWIO_SB_NGD2_RX_MSGQ_CFG_TIME_OUT_VAL_SHFT                                                   0x10
#define HWIO_SB_NGD2_RX_MSGQ_CFG_TRANS_SIZE_BMSK                                                   0xff00
#define HWIO_SB_NGD2_RX_MSGQ_CFG_TRANS_SIZE_SHFT                                                      0x8
#define HWIO_SB_NGD2_RX_MSGQ_CFG_BLOCK_SIZE_BMSK                                                     0xff
#define HWIO_SB_NGD2_RX_MSGQ_CFG_BLOCK_SIZE_SHFT                                                      0x0

#define HWIO_SB_NGD2_INT_EN_ADDR(x)                                                            ((x) + 0x00002010)
#define HWIO_SB_NGD2_INT_EN_RMSK                                                               0xff000000
#define HWIO_SB_NGD2_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_INT_EN_ADDR(x), HWIO_SB_NGD2_INT_EN_RMSK)
#define HWIO_SB_NGD2_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_INT_EN_ADDR(x), m)
#define HWIO_SB_NGD2_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_NGD2_INT_EN_ADDR(x),v)
#define HWIO_SB_NGD2_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_NGD2_INT_EN_ADDR(x),m,v,HWIO_SB_NGD2_INT_EN_IN(x))
#define HWIO_SB_NGD2_INT_EN_TX_MSG_SENT_BMSK                                                   0x80000000
#define HWIO_SB_NGD2_INT_EN_TX_MSG_SENT_SHFT                                                         0x1f
#define HWIO_SB_NGD2_INT_EN_RX_MSG_RCVD_BMSK                                                   0x40000000
#define HWIO_SB_NGD2_INT_EN_RX_MSG_RCVD_SHFT                                                         0x1e
#define HWIO_SB_NGD2_INT_EN_DEV_ERR_BMSK                                                       0x20000000
#define HWIO_SB_NGD2_INT_EN_DEV_ERR_SHFT                                                             0x1d
#define HWIO_SB_NGD2_INT_EN_IE_VE_CHANGE_BMSK                                                  0x10000000
#define HWIO_SB_NGD2_INT_EN_IE_VE_CHANGE_SHFT                                                        0x1c
#define HWIO_SB_NGD2_INT_EN_INVALID_TX_MSG_BMSK                                                 0x8000000
#define HWIO_SB_NGD2_INT_EN_INVALID_TX_MSG_SHFT                                                      0x1b
#define HWIO_SB_NGD2_INT_EN_TX_MSG_BUF_CONTENTION_BMSK                                          0x4000000
#define HWIO_SB_NGD2_INT_EN_TX_MSG_BUF_CONTENTION_SHFT                                               0x1a
#define HWIO_SB_NGD2_INT_EN_TX_MSG_NACKED_BMSK                                                  0x2000000
#define HWIO_SB_NGD2_INT_EN_TX_MSG_NACKED_SHFT                                                       0x19
#define HWIO_SB_NGD2_INT_EN_RECONFIG_DONE_BMSK                                                  0x1000000
#define HWIO_SB_NGD2_INT_EN_RECONFIG_DONE_SHFT                                                       0x18

#define HWIO_SB_NGD2_INT_STATUS_ADDR(x)                                                        ((x) + 0x00002014)
#define HWIO_SB_NGD2_INT_STATUS_RMSK                                                           0xff000000
#define HWIO_SB_NGD2_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_INT_STATUS_ADDR(x), HWIO_SB_NGD2_INT_STATUS_RMSK)
#define HWIO_SB_NGD2_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_INT_STATUS_ADDR(x), m)
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_SENT_BMSK                                               0x80000000
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_SENT_SHFT                                                     0x1f
#define HWIO_SB_NGD2_INT_STATUS_RX_MSG_RCVD_BMSK                                               0x40000000
#define HWIO_SB_NGD2_INT_STATUS_RX_MSG_RCVD_SHFT                                                     0x1e
#define HWIO_SB_NGD2_INT_STATUS_DEV_ERR_BMSK                                                   0x20000000
#define HWIO_SB_NGD2_INT_STATUS_DEV_ERR_SHFT                                                         0x1d
#define HWIO_SB_NGD2_INT_STATUS_IE_VE_CHANGE_BMSK                                              0x10000000
#define HWIO_SB_NGD2_INT_STATUS_IE_VE_CHANGE_SHFT                                                    0x1c
#define HWIO_SB_NGD2_INT_STATUS_INVALID_TX_MSG_BMSK                                             0x8000000
#define HWIO_SB_NGD2_INT_STATUS_INVALID_TX_MSG_SHFT                                                  0x1b
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_BUF_CONTENTION_BMSK                                      0x4000000
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_BUF_CONTENTION_SHFT                                           0x1a
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_NACKED_BMSK                                              0x2000000
#define HWIO_SB_NGD2_INT_STATUS_TX_MSG_NACKED_SHFT                                                   0x19
#define HWIO_SB_NGD2_INT_STATUS_RECONFIG_DONE_BMSK                                              0x1000000
#define HWIO_SB_NGD2_INT_STATUS_RECONFIG_DONE_SHFT                                                   0x18

#define HWIO_SB_NGD2_INT_CLR_ADDR(x)                                                           ((x) + 0x00002018)
#define HWIO_SB_NGD2_INT_CLR_RMSK                                                              0xff000000
#define HWIO_SB_NGD2_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_NGD2_INT_CLR_ADDR(x),v)
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_SENT_BMSK                                                  0x80000000
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_SENT_SHFT                                                        0x1f
#define HWIO_SB_NGD2_INT_CLR_RX_MSG_RCVD_BMSK                                                  0x40000000
#define HWIO_SB_NGD2_INT_CLR_RX_MSG_RCVD_SHFT                                                        0x1e
#define HWIO_SB_NGD2_INT_CLR_DEV_ERR_BMSK                                                      0x20000000
#define HWIO_SB_NGD2_INT_CLR_DEV_ERR_SHFT                                                            0x1d
#define HWIO_SB_NGD2_INT_CLR_IE_VE_CHANGE_BMSK                                                 0x10000000
#define HWIO_SB_NGD2_INT_CLR_IE_VE_CHANGE_SHFT                                                       0x1c
#define HWIO_SB_NGD2_INT_CLR_INVALID_TX_MSG_BMSK                                                0x8000000
#define HWIO_SB_NGD2_INT_CLR_INVALID_TX_MSG_SHFT                                                     0x1b
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_BUF_CONTENTION_BMSK                                         0x4000000
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_BUF_CONTENTION_SHFT                                              0x1a
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_NACKED_BMSK                                                 0x2000000
#define HWIO_SB_NGD2_INT_CLR_TX_MSG_NACKED_SHFT                                                      0x19
#define HWIO_SB_NGD2_INT_CLR_RECONFIG_DONE_BMSK                                                 0x1000000
#define HWIO_SB_NGD2_INT_CLR_RECONFIG_DONE_SHFT                                                      0x18

#define HWIO_SB_NGD2_TX_MSGn_ADDR(base,n)                                                      ((base) + 0x00002030 + 0x4 * (n))
#define HWIO_SB_NGD2_TX_MSGn_RMSK                                                              0xffffffff
#define HWIO_SB_NGD2_TX_MSGn_MAXn                                                                       9
#define HWIO_SB_NGD2_TX_MSGn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_NGD2_TX_MSGn_ADDR(base,n),val)
#define HWIO_SB_NGD2_TX_MSGn_MSGVAL_BMSK                                                       0xffffffff
#define HWIO_SB_NGD2_TX_MSGn_MSGVAL_SHFT                                                              0x0

#define HWIO_SB_NGD2_RX_MSGn_ADDR(base,n)                                                      ((base) + 0x00002070 + 0x4 * (n))
#define HWIO_SB_NGD2_RX_MSGn_RMSK                                                              0xffffffff
#define HWIO_SB_NGD2_RX_MSGn_MAXn                                                                       9
#define HWIO_SB_NGD2_RX_MSGn_INI(base,n)        \
        in_dword_masked(HWIO_SB_NGD2_RX_MSGn_ADDR(base,n), HWIO_SB_NGD2_RX_MSGn_RMSK)
#define HWIO_SB_NGD2_RX_MSGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_NGD2_RX_MSGn_ADDR(base,n), mask)
#define HWIO_SB_NGD2_RX_MSGn_MSGVAL_BMSK                                                       0xffffffff
#define HWIO_SB_NGD2_RX_MSGn_MSGVAL_SHFT                                                              0x0

#define HWIO_SB_NGD2_IE_STATUS_ADDR(x)                                                         ((x) + 0x000020f0)
#define HWIO_SB_NGD2_IE_STATUS_RMSK                                                            0xffff000f
#define HWIO_SB_NGD2_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_IE_STATUS_ADDR(x), HWIO_SB_NGD2_IE_STATUS_RMSK)
#define HWIO_SB_NGD2_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_IE_STATUS_ADDR(x), m)
#define HWIO_SB_NGD2_IE_STATUS_DEV_CLASS_CODE_BMSK                                             0xff000000
#define HWIO_SB_NGD2_IE_STATUS_DEV_CLASS_CODE_SHFT                                                   0x18
#define HWIO_SB_NGD2_IE_STATUS_DEV_CLASS_VER_BMSK                                                0xff0000
#define HWIO_SB_NGD2_IE_STATUS_DEV_CLASS_VER_SHFT                                                    0x10
#define HWIO_SB_NGD2_IE_STATUS_EX_ERROR_BMSK                                                          0x8
#define HWIO_SB_NGD2_IE_STATUS_EX_ERROR_SHFT                                                          0x3
#define HWIO_SB_NGD2_IE_STATUS_RECONFIG_OBJ_BMSK                                                      0x4
#define HWIO_SB_NGD2_IE_STATUS_RECONFIG_OBJ_SHFT                                                      0x2
#define HWIO_SB_NGD2_IE_STATUS_DATA_TX_COL_BMSK                                                       0x2
#define HWIO_SB_NGD2_IE_STATUS_DATA_TX_COL_SHFT                                                       0x1
#define HWIO_SB_NGD2_IE_STATUS_UNSPRTD_MSG_BMSK                                                       0x1
#define HWIO_SB_NGD2_IE_STATUS_UNSPRTD_MSG_SHFT                                                       0x0

#define HWIO_SB_NGD2_VE_STATUS_ADDR(x)                                                         ((x) + 0x00002100)
#define HWIO_SB_NGD2_VE_STATUS_RMSK                                                            0xffffffff
#define HWIO_SB_NGD2_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_NGD2_VE_STATUS_ADDR(x), HWIO_SB_NGD2_VE_STATUS_RMSK)
#define HWIO_SB_NGD2_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_NGD2_VE_STATUS_ADDR(x), m)
#define HWIO_SB_NGD2_VE_STATUS_VE_VAL_BMSK                                                     0xffffffff
#define HWIO_SB_NGD2_VE_STATUS_VE_VAL_SHFT                                                            0x0

#define HWIO_SB_PGD_CFG_ADDR(x)                                                                ((x) + 0x00000800)
#define HWIO_SB_PGD_CFG_RMSK                                                                          0x7
#define HWIO_SB_PGD_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_CFG_ADDR(x), HWIO_SB_PGD_CFG_RMSK)
#define HWIO_SB_PGD_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_CFG_ADDR(x), m)
#define HWIO_SB_PGD_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_CFG_ADDR(x),v)
#define HWIO_SB_PGD_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_PGD_CFG_ADDR(x),m,v,HWIO_SB_PGD_CFG_IN(x))
#define HWIO_SB_PGD_CFG_SI_CNT_EOSF_RST_DIS_BMSK                                                      0x4
#define HWIO_SB_PGD_CFG_SI_CNT_EOSF_RST_DIS_SHFT                                                      0x2
#define HWIO_SB_PGD_CFG_DRIVE_RX_MSG_CONSUMED_BMSK                                                    0x2
#define HWIO_SB_PGD_CFG_DRIVE_RX_MSG_CONSUMED_SHFT                                                    0x1
#define HWIO_SB_PGD_CFG_ENABLE_BMSK                                                                   0x1
#define HWIO_SB_PGD_CFG_ENABLE_SHFT                                                                   0x0

#define HWIO_SB_PGD_STATUS_ADDR(x)                                                             ((x) + 0x00000804)
#define HWIO_SB_PGD_STATUS_RMSK                                                                  0xffffff
#define HWIO_SB_PGD_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_STATUS_ADDR(x), HWIO_SB_PGD_STATUS_RMSK)
#define HWIO_SB_PGD_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_STATUS_ADDR(x), m)
#define HWIO_SB_PGD_STATUS_ENUMERATED_BMSK                                                       0x800000
#define HWIO_SB_PGD_STATUS_ENUMERATED_SHFT                                                           0x17
#define HWIO_SB_PGD_STATUS_NUM_PC_VFR_BLKS_BMSK                                                  0x7c0000
#define HWIO_SB_PGD_STATUS_NUM_PC_VFR_BLKS_SHFT                                                      0x12
#define HWIO_SB_PGD_STATUS_ERROR_BMSK                                                             0x30000
#define HWIO_SB_PGD_STATUS_ERROR_SHFT                                                                0x10
#define HWIO_SB_PGD_STATUS_PIPE_NUM_OFFSET_BMSK                                                    0xff00
#define HWIO_SB_PGD_STATUS_PIPE_NUM_OFFSET_SHFT                                                       0x8
#define HWIO_SB_PGD_STATUS_NUM_PORTS_BMSK                                                            0xff
#define HWIO_SB_PGD_STATUS_NUM_PORTS_SHFT                                                             0x0

#define HWIO_SB_PGD_DEV_INT_EN_ADDR(x)                                                         ((x) + 0x00000810)
#define HWIO_SB_PGD_DEV_INT_EN_RMSK                                                                   0x3
#define HWIO_SB_PGD_DEV_INT_EN_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_DEV_INT_EN_ADDR(x), HWIO_SB_PGD_DEV_INT_EN_RMSK)
#define HWIO_SB_PGD_DEV_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_DEV_INT_EN_ADDR(x), m)
#define HWIO_SB_PGD_DEV_INT_EN_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_DEV_INT_EN_ADDR(x),v)
#define HWIO_SB_PGD_DEV_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_PGD_DEV_INT_EN_ADDR(x),m,v,HWIO_SB_PGD_DEV_INT_EN_IN(x))
#define HWIO_SB_PGD_DEV_INT_EN_IE_VE_CHANGE_BMSK                                                      0x2
#define HWIO_SB_PGD_DEV_INT_EN_IE_VE_CHANGE_SHFT                                                      0x1
#define HWIO_SB_PGD_DEV_INT_EN_DEV_ERR_BMSK                                                           0x1
#define HWIO_SB_PGD_DEV_INT_EN_DEV_ERR_SHFT                                                           0x0

#define HWIO_SB_PGD_DEV_INT_STATUS_ADDR(x)                                                     ((x) + 0x00000814)
#define HWIO_SB_PGD_DEV_INT_STATUS_RMSK                                                               0x3
#define HWIO_SB_PGD_DEV_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_DEV_INT_STATUS_ADDR(x), HWIO_SB_PGD_DEV_INT_STATUS_RMSK)
#define HWIO_SB_PGD_DEV_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_DEV_INT_STATUS_ADDR(x), m)
#define HWIO_SB_PGD_DEV_INT_STATUS_IE_VE_CHANGE_BMSK                                                  0x2
#define HWIO_SB_PGD_DEV_INT_STATUS_IE_VE_CHANGE_SHFT                                                  0x1
#define HWIO_SB_PGD_DEV_INT_STATUS_DEV_ERR_BMSK                                                       0x1
#define HWIO_SB_PGD_DEV_INT_STATUS_DEV_ERR_SHFT                                                       0x0

#define HWIO_SB_PGD_DEV_INT_CLR_ADDR(x)                                                        ((x) + 0x00000818)
#define HWIO_SB_PGD_DEV_INT_CLR_RMSK                                                                  0x3
#define HWIO_SB_PGD_DEV_INT_CLR_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_DEV_INT_CLR_ADDR(x),v)
#define HWIO_SB_PGD_DEV_INT_CLR_IE_VE_CHANGE_BMSK                                                     0x2
#define HWIO_SB_PGD_DEV_INT_CLR_IE_VE_CHANGE_SHFT                                                     0x1
#define HWIO_SB_PGD_DEV_INT_CLR_DEV_ERR_BMSK                                                          0x1
#define HWIO_SB_PGD_DEV_INT_CLR_DEV_ERR_SHFT                                                          0x0

#define HWIO_SB_COMP_TRUST_CFG_ADDR(x)                                                         ((x) + 0x00003000)
#define HWIO_SB_COMP_TRUST_CFG_RMSK                                                                 0xfff
#define HWIO_SB_COMP_TRUST_CFG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_CFG_ADDR(x), HWIO_SB_COMP_TRUST_CFG_RMSK)
#define HWIO_SB_COMP_TRUST_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_CFG_ADDR(x), m)
#define HWIO_SB_COMP_TRUST_CFG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_TRUST_CFG_ADDR(x),v)
#define HWIO_SB_COMP_TRUST_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_TRUST_CFG_ADDR(x),m,v,HWIO_SB_COMP_TRUST_CFG_IN(x))
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_MGR_RSRC_GRP_BMSK                                             0xc00
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_MGR_RSRC_GRP_SHFT                                               0xa
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_NGD1_BMSK                                                     0x300
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_NGD1_SHFT                                                       0x8
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_NGD2_BMSK                                                      0xc0
#define HWIO_SB_COMP_TRUST_CFG_EE_FOR_NGD2_SHFT                                                       0x6
#define HWIO_SB_COMP_TRUST_CFG_HALD_MGR_MSG_TX_BMSK                                                  0x20
#define HWIO_SB_COMP_TRUST_CFG_HALD_MGR_MSG_TX_SHFT                                                   0x5
#define HWIO_SB_COMP_TRUST_CFG_HALT_NGD2_MSG_TX_BMSK                                                 0x10
#define HWIO_SB_COMP_TRUST_CFG_HALT_NGD2_MSG_TX_SHFT                                                  0x4
#define HWIO_SB_COMP_TRUST_CFG_HALT_NGD1_MSG_TX_BMSK                                                  0x8
#define HWIO_SB_COMP_TRUST_CFG_HALT_NGD1_MSG_TX_SHFT                                                  0x3
#define HWIO_SB_COMP_TRUST_CFG_HALT_EN_CHANGE_BMSK                                                    0x4
#define HWIO_SB_COMP_TRUST_CFG_HALT_EN_CHANGE_SHFT                                                    0x2
#define HWIO_SB_COMP_TRUST_CFG_HALT_SW_RESET_BMSK                                                     0x2
#define HWIO_SB_COMP_TRUST_CFG_HALT_SW_RESET_SHFT                                                     0x1
#define HWIO_SB_COMP_TRUST_CFG_HALT_RECONFIG_BMSK                                                     0x1
#define HWIO_SB_COMP_TRUST_CFG_HALT_RECONFIG_SHFT                                                     0x0

#define HWIO_SB_COMP_TRUST_DEV1_ADDR(x)                                                        ((x) + 0x00003004)
#define HWIO_SB_COMP_TRUST_DEV1_RMSK                                                           0xffffff01
#define HWIO_SB_COMP_TRUST_DEV1_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_DEV1_ADDR(x), HWIO_SB_COMP_TRUST_DEV1_RMSK)
#define HWIO_SB_COMP_TRUST_DEV1_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_DEV1_ADDR(x), m)
#define HWIO_SB_COMP_TRUST_DEV1_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_TRUST_DEV1_ADDR(x),v)
#define HWIO_SB_COMP_TRUST_DEV1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_TRUST_DEV1_ADDR(x),m,v,HWIO_SB_COMP_TRUST_DEV1_IN(x))
#define HWIO_SB_COMP_TRUST_DEV1_TRUSTED_EA_MSB_BMSK                                            0xffff0000
#define HWIO_SB_COMP_TRUST_DEV1_TRUSTED_EA_MSB_SHFT                                                  0x10
#define HWIO_SB_COMP_TRUST_DEV1_TRUSTED_LA_BMSK                                                    0xff00
#define HWIO_SB_COMP_TRUST_DEV1_TRUSTED_LA_SHFT                                                       0x8
#define HWIO_SB_COMP_TRUST_DEV1_TRUST_DEV_EN_BMSK                                                     0x1
#define HWIO_SB_COMP_TRUST_DEV1_TRUST_DEV_EN_SHFT                                                     0x0

#define HWIO_SB_COMP_TRUST_DEV2_ADDR(x)                                                        ((x) + 0x00003008)
#define HWIO_SB_COMP_TRUST_DEV2_RMSK                                                           0xffffffff
#define HWIO_SB_COMP_TRUST_DEV2_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_DEV2_ADDR(x), HWIO_SB_COMP_TRUST_DEV2_RMSK)
#define HWIO_SB_COMP_TRUST_DEV2_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_TRUST_DEV2_ADDR(x), m)
#define HWIO_SB_COMP_TRUST_DEV2_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_TRUST_DEV2_ADDR(x),v)
#define HWIO_SB_COMP_TRUST_DEV2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_TRUST_DEV2_ADDR(x),m,v,HWIO_SB_COMP_TRUST_DEV2_IN(x))
#define HWIO_SB_COMP_TRUST_DEV2_TRUSTED_EA_LSB_BMSK                                            0xffffffff
#define HWIO_SB_COMP_TRUST_DEV2_TRUSTED_EA_LSB_SHFT                                                   0x0

#define HWIO_SB_PGD_TRUST_OWN_EE0_ADDR(x)                                                      ((x) + 0x0000300c)
#define HWIO_SB_PGD_TRUST_OWN_EE0_RMSK                                                         0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE0_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE0_ADDR(x), HWIO_SB_PGD_TRUST_OWN_EE0_RMSK)
#define HWIO_SB_PGD_TRUST_OWN_EE0_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE0_ADDR(x), m)
#define HWIO_SB_PGD_TRUST_OWN_EE0_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_TRUST_OWN_EE0_ADDR(x),v)
#define HWIO_SB_PGD_TRUST_OWN_EE0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_PGD_TRUST_OWN_EE0_ADDR(x),m,v,HWIO_SB_PGD_TRUST_OWN_EE0_IN(x))
#define HWIO_SB_PGD_TRUST_OWN_EE0_PORT_OWN_BMSK                                                0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE0_PORT_OWN_SHFT                                                       0x0

#define HWIO_SB_PGD_TRUST_OWN_EE1_ADDR(x)                                                      ((x) + 0x00003010)
#define HWIO_SB_PGD_TRUST_OWN_EE1_RMSK                                                         0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE1_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE1_ADDR(x), HWIO_SB_PGD_TRUST_OWN_EE1_RMSK)
#define HWIO_SB_PGD_TRUST_OWN_EE1_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE1_ADDR(x), m)
#define HWIO_SB_PGD_TRUST_OWN_EE1_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_TRUST_OWN_EE1_ADDR(x),v)
#define HWIO_SB_PGD_TRUST_OWN_EE1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_PGD_TRUST_OWN_EE1_ADDR(x),m,v,HWIO_SB_PGD_TRUST_OWN_EE1_IN(x))
#define HWIO_SB_PGD_TRUST_OWN_EE1_PORT_OWN_BMSK                                                0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE1_PORT_OWN_SHFT                                                       0x0

#define HWIO_SB_PGD_TRUST_OWN_EE2_ADDR(x)                                                      ((x) + 0x00003014)
#define HWIO_SB_PGD_TRUST_OWN_EE2_RMSK                                                         0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE2_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE2_ADDR(x), HWIO_SB_PGD_TRUST_OWN_EE2_RMSK)
#define HWIO_SB_PGD_TRUST_OWN_EE2_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_TRUST_OWN_EE2_ADDR(x), m)
#define HWIO_SB_PGD_TRUST_OWN_EE2_OUT(x, v)      \
        out_dword(HWIO_SB_PGD_TRUST_OWN_EE2_ADDR(x),v)
#define HWIO_SB_PGD_TRUST_OWN_EE2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_PGD_TRUST_OWN_EE2_ADDR(x),m,v,HWIO_SB_PGD_TRUST_OWN_EE2_IN(x))
#define HWIO_SB_PGD_TRUST_OWN_EE2_PORT_OWN_BMSK                                                0xffffffff
#define HWIO_SB_PGD_TRUST_OWN_EE2_PORT_OWN_SHFT                                                       0x0

#define HWIO_SB_COMP_TEST_ADDR(x)                                                              ((x) + 0x00004000)
#define HWIO_SB_COMP_TEST_RMSK                                                                     0xffff
#define HWIO_SB_COMP_TEST_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_TEST_ADDR(x), HWIO_SB_COMP_TEST_RMSK)
#define HWIO_SB_COMP_TEST_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_TEST_ADDR(x), m)
#define HWIO_SB_COMP_TEST_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_TEST_ADDR(x),v)
#define HWIO_SB_COMP_TEST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_TEST_ADDR(x),m,v,HWIO_SB_COMP_TEST_IN(x))
#define HWIO_SB_COMP_TEST_SB_PGD_PORT_TESTBUS_SEL_BMSK                                             0xf000
#define HWIO_SB_COMP_TEST_SB_PGD_PORT_TESTBUS_SEL_SHFT                                                0xc
#define HWIO_SB_COMP_TEST_SB_PGD_TESTBUS_SEL_BMSK                                                   0xf80
#define HWIO_SB_COMP_TEST_SB_PGD_TESTBUS_SEL_SHFT                                                     0x7
#define HWIO_SB_COMP_TEST_SB_TESTBUS_SEL_BMSK                                                        0x70
#define HWIO_SB_COMP_TEST_SB_TESTBUS_SEL_SHFT                                                         0x4
#define HWIO_SB_COMP_TEST_SB_TESTBUS_EN_BMSK                                                          0x8
#define HWIO_SB_COMP_TEST_SB_TESTBUS_EN_SHFT                                                          0x3
#define HWIO_SB_COMP_TEST_INT_EE2_BMSK                                                                0x4
#define HWIO_SB_COMP_TEST_INT_EE2_SHFT                                                                0x2
#define HWIO_SB_COMP_TEST_INT_EE1_BMSK                                                                0x2
#define HWIO_SB_COMP_TEST_INT_EE1_SHFT                                                                0x1
#define HWIO_SB_COMP_TEST_INT_EE0_BMSK                                                                0x1
#define HWIO_SB_COMP_TEST_INT_EE0_SHFT                                                                0x0

#define HWIO_SB_COMP_DEBUG_ADDR(x)                                                             ((x) + 0x00004004)
#define HWIO_SB_COMP_DEBUG_RMSK                                                                       0x1
#define HWIO_SB_COMP_DEBUG_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_DEBUG_ADDR(x), HWIO_SB_COMP_DEBUG_RMSK)
#define HWIO_SB_COMP_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_DEBUG_ADDR(x), m)
#define HWIO_SB_COMP_DEBUG_OUT(x, v)      \
        out_dword(HWIO_SB_COMP_DEBUG_ADDR(x),v)
#define HWIO_SB_COMP_DEBUG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SB_COMP_DEBUG_ADDR(x),m,v,HWIO_SB_COMP_DEBUG_IN(x))
#define HWIO_SB_COMP_DEBUG_EN_CONTROL_ON_ADDITIONAL_DATA_LANE_BMSK                                    0x1
#define HWIO_SB_COMP_DEBUG_EN_CONTROL_ON_ADDITIONAL_DATA_LANE_SHFT                                    0x0

#define HWIO_SB_COMP_DEBUG_BUS_VALUE_ADDR(x)                                                   ((x) + 0x00004008)
#define HWIO_SB_COMP_DEBUG_BUS_VALUE_RMSK                                                      0xffffffff
#define HWIO_SB_COMP_DEBUG_BUS_VALUE_IN(x)      \
        in_dword_masked(HWIO_SB_COMP_DEBUG_BUS_VALUE_ADDR(x), HWIO_SB_COMP_DEBUG_BUS_VALUE_RMSK)
#define HWIO_SB_COMP_DEBUG_BUS_VALUE_INM(x, m)      \
        in_dword_masked(HWIO_SB_COMP_DEBUG_BUS_VALUE_ADDR(x), m)
#define HWIO_SB_COMP_DEBUG_BUS_VALUE_DEBUG_BUS_VALUE_BMSK                                      0xffffffff
#define HWIO_SB_COMP_DEBUG_BUS_VALUE_DEBUG_BUS_VALUE_SHFT                                             0x0

#define HWIO_SB_PGD_PORT_INT_EN_EEn_ADDR(base,n)                                               ((base) + 0x00005000 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_INT_EN_EEn_RMSK                                                       0xffffffff
#define HWIO_SB_PGD_PORT_INT_EN_EEn_MAXn                                                                2
#define HWIO_SB_PGD_PORT_INT_EN_EEn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_INT_EN_EEn_ADDR(base,n), HWIO_SB_PGD_PORT_INT_EN_EEn_RMSK)
#define HWIO_SB_PGD_PORT_INT_EN_EEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_INT_EN_EEn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_INT_EN_EEn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_INT_EN_EEn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_INT_EN_EEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_INT_EN_EEn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_INT_EN_EEn_INI(base,n))
#define HWIO_SB_PGD_PORT_INT_EN_EEn_PORT_INT_BMSK                                              0xffffffff
#define HWIO_SB_PGD_PORT_INT_EN_EEn_PORT_INT_SHFT                                                     0x0

#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_ADDR(base,n)                                           ((base) + 0x00005004 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_RMSK                                                   0xffffffff
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_MAXn                                                            2
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_INT_STATUS_EEn_ADDR(base,n), HWIO_SB_PGD_PORT_INT_STATUS_EEn_RMSK)
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_INT_STATUS_EEn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_PORT_INT_BMSK                                          0xffffffff
#define HWIO_SB_PGD_PORT_INT_STATUS_EEn_PORT_INT_SHFT                                                 0x0

#define HWIO_SB_PGD_PORT_INT_CLR_EEn_ADDR(base,n)                                              ((base) + 0x00005008 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_INT_CLR_EEn_RMSK                                                      0xffffffff
#define HWIO_SB_PGD_PORT_INT_CLR_EEn_MAXn                                                               2
#define HWIO_SB_PGD_PORT_INT_CLR_EEn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_INT_CLR_EEn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_INT_CLR_EEn_PORT_INT_BMSK                                             0xffffffff
#define HWIO_SB_PGD_PORT_INT_CLR_EEn_PORT_INT_SHFT                                                    0x0

#define HWIO_SB_PGD_PORT_CFGn_ADDR(base,n)                                                     ((base) + 0x00014000 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_CFGn_RMSK                                                                 0xffff
#define HWIO_SB_PGD_PORT_CFGn_MAXn                                                                     23
#define HWIO_SB_PGD_PORT_CFGn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_CFGn_ADDR(base,n), HWIO_SB_PGD_PORT_CFGn_RMSK)
#define HWIO_SB_PGD_PORT_CFGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_CFGn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_CFGn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_CFGn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_CFGn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_CFGn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_CFGn_INI(base,n))
#define HWIO_SB_PGD_PORT_CFGn_SAMPLE_SHADOW_ON_MLTI_CHNL_DMA_IRQ_BMSK                              0x8000
#define HWIO_SB_PGD_PORT_CFGn_SAMPLE_SHADOW_ON_MLTI_CHNL_DMA_IRQ_SHFT                                 0xf
#define HWIO_SB_PGD_PORT_CFGn_DMA_IRQ_EN_BMSK                                                      0x4000
#define HWIO_SB_PGD_PORT_CFGn_DMA_IRQ_EN_SHFT                                                         0xe
#define HWIO_SB_PGD_PORT_CFGn_TX_WORD_COUNT_CALC_ADJUST_BMSK                                       0x2000
#define HWIO_SB_PGD_PORT_CFGn_TX_WORD_COUNT_CALC_ADJUST_SHFT                                          0xd
#define HWIO_SB_PGD_PORT_CFGn_UPPER_WATERMARK_BMSK                                                 0x1e00
#define HWIO_SB_PGD_PORT_CFGn_UPPER_WATERMARK_SHFT                                                    0x9
#define HWIO_SB_PGD_PORT_CFGn_USE_UPPER_WATERMARK_BMSK                                              0x100
#define HWIO_SB_PGD_PORT_CFGn_USE_UPPER_WATERMARK_SHFT                                                0x8
#define HWIO_SB_PGD_PORT_CFGn_ALIGN_MSB_BMSK                                                         0x80
#define HWIO_SB_PGD_PORT_CFGn_ALIGN_MSB_SHFT                                                          0x7
#define HWIO_SB_PGD_PORT_CFGn_PACK_BMSK                                                              0x40
#define HWIO_SB_PGD_PORT_CFGn_PACK_SHFT                                                               0x6
#define HWIO_SB_PGD_PORT_CFGn_ALLOW_DT_LPCM_BMSK                                                     0x20
#define HWIO_SB_PGD_PORT_CFGn_ALLOW_DT_LPCM_SHFT                                                      0x5
#define HWIO_SB_PGD_PORT_CFGn_WATERMARK_BMSK                                                         0x1e
#define HWIO_SB_PGD_PORT_CFGn_WATERMARK_SHFT                                                          0x1
#define HWIO_SB_PGD_PORT_CFGn_ENABLE_BMSK                                                             0x1
#define HWIO_SB_PGD_PORT_CFGn_ENABLE_SHFT                                                             0x0

#define HWIO_SB_PGD_PORT_STATUSn_ADDR(base,n)                                                  ((base) + 0x00014004 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_STATUSn_RMSK                                                            0x3fffff
#define HWIO_SB_PGD_PORT_STATUSn_MAXn                                                                  23
#define HWIO_SB_PGD_PORT_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_STATUSn_ADDR(base,n), HWIO_SB_PGD_PORT_STATUSn_RMSK)
#define HWIO_SB_PGD_PORT_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_STATUSn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_STATUSn_DMA_IRQ_BMSK                                                    0x200000
#define HWIO_SB_PGD_PORT_STATUSn_DMA_IRQ_SHFT                                                        0x15
#define HWIO_SB_PGD_PORT_STATUSn_REMOVE_CHANNEL_BMSK                                             0x100000
#define HWIO_SB_PGD_PORT_STATUSn_REMOVE_CHANNEL_SHFT                                                 0x14
#define HWIO_SB_PGD_PORT_STATUSn_PRT_DISCONN_BMSK                                                 0x80000
#define HWIO_SB_PGD_PORT_STATUSn_PRT_DISCONN_SHFT                                                    0x13
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_FILL_LEVEL_BMSK                                             0x40000
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_FILL_LEVEL_SHFT                                                0x12
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_WORD_CNT_BMSK                                               0x3f000
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_WORD_CNT_SHFT                                                   0xc
#define HWIO_SB_PGD_PORT_STATUSn_PIPE_NUM_BMSK                                                      0xff0
#define HWIO_SB_PGD_PORT_STATUSn_PIPE_NUM_SHFT                                                        0x4
#define HWIO_SB_PGD_PORT_STATUSn_UNDERFLOW_BMSK                                                       0x8
#define HWIO_SB_PGD_PORT_STATUSn_UNDERFLOW_SHFT                                                       0x3
#define HWIO_SB_PGD_PORT_STATUSn_OVERFLOW_BMSK                                                        0x4
#define HWIO_SB_PGD_PORT_STATUSn_OVERFLOW_SHFT                                                        0x2
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_SIZE_BMSK                                                       0x3
#define HWIO_SB_PGD_PORT_STATUSn_FIFO_SIZE_SHFT                                                       0x0

#define HWIO_SB_PGD_PORT_PARAMn_ADDR(base,n)                                                   ((base) + 0x00014008 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_PARAMn_RMSK                                                           0x3fffffff
#define HWIO_SB_PGD_PORT_PARAMn_MAXn                                                                   23
#define HWIO_SB_PGD_PORT_PARAMn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_PARAMn_ADDR(base,n), HWIO_SB_PGD_PORT_PARAMn_RMSK)
#define HWIO_SB_PGD_PORT_PARAMn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_PARAMn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_PARAMn_SEG_LENGTH_BMSK                                                0x3e000000
#define HWIO_SB_PGD_PORT_PARAMn_SEG_LENGTH_SHFT                                                      0x19
#define HWIO_SB_PGD_PORT_PARAMn_TRANSPORT_PROTOCOL_BMSK                                         0x1e00000
#define HWIO_SB_PGD_PORT_PARAMn_TRANSPORT_PROTOCOL_SHFT                                              0x15
#define HWIO_SB_PGD_PORT_PARAMn_SEG_DISTN_BMSK                                                   0x1ffe00
#define HWIO_SB_PGD_PORT_PARAMn_SEG_DISTN_SHFT                                                        0x9
#define HWIO_SB_PGD_PORT_PARAMn_CHANNEL_NUMBER_BMSK                                                 0x1fe
#define HWIO_SB_PGD_PORT_PARAMn_CHANNEL_NUMBER_SHFT                                                   0x1
#define HWIO_SB_PGD_PORT_PARAMn_CHANNEL_ACTIVE_BMSK                                                   0x1
#define HWIO_SB_PGD_PORT_PARAMn_CHANNEL_ACTIVE_SHFT                                                   0x0

#define HWIO_SB_PGD_PORT_BLK_SIZEn_ADDR(base,n)                                                ((base) + 0x0001400c + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_BLK_SIZEn_RMSK                                                        0xffffffff
#define HWIO_SB_PGD_PORT_BLK_SIZEn_MAXn                                                                23
#define HWIO_SB_PGD_PORT_BLK_SIZEn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_BLK_SIZEn_ADDR(base,n), HWIO_SB_PGD_PORT_BLK_SIZEn_RMSK)
#define HWIO_SB_PGD_PORT_BLK_SIZEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_BLK_SIZEn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_BLK_SIZEn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_BLK_SIZEn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_BLK_SIZEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_BLK_SIZEn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_BLK_SIZEn_INI(base,n))
#define HWIO_SB_PGD_PORT_BLK_SIZEn_BLK_SIZE_BMSK                                               0xffffffff
#define HWIO_SB_PGD_PORT_BLK_SIZEn_BLK_SIZE_SHFT                                                      0x0

#define HWIO_SB_PGD_PORT_TRANS_SIZEn_ADDR(base,n)                                              ((base) + 0x00014010 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_RMSK                                                      0xffffffff
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_MAXn                                                              23
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_TRANS_SIZEn_ADDR(base,n), HWIO_SB_PGD_PORT_TRANS_SIZEn_RMSK)
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_TRANS_SIZEn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_TRANS_SIZEn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_TRANS_SIZEn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_TRANS_SIZEn_INI(base,n))
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_TRANS_SIZE_BMSK                                           0xffffffff
#define HWIO_SB_PGD_PORT_TRANS_SIZEn_TRANS_SIZE_SHFT                                                  0x0

#define HWIO_SB_PGD_PORT_MULTI_CHNLn_ADDR(base,n)                                              ((base) + 0x00014014 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_RMSK                                                      0xffffffff
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_MAXn                                                              23
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_MULTI_CHNLn_ADDR(base,n), HWIO_SB_PGD_PORT_MULTI_CHNLn_RMSK)
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_MULTI_CHNLn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_MULTI_CHNLn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_MULTI_CHNLn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_MULTI_CHNLn_INI(base,n))
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_PORTS_BMSK                                                0xffffffff
#define HWIO_SB_PGD_PORT_MULTI_CHNLn_PORTS_SHFT                                                       0x0

#define HWIO_SB_PGD_PORT_PUSHPULLn_ADDR(base,n)                                                ((base) + 0x00014018 + 0x1000 * (n))
#define HWIO_SB_PGD_PORT_PUSHPULLn_RMSK                                                        0xffffffff
#define HWIO_SB_PGD_PORT_PUSHPULLn_MAXn                                                                23
#define HWIO_SB_PGD_PORT_PUSHPULLn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PORT_PUSHPULLn_ADDR(base,n), HWIO_SB_PGD_PORT_PUSHPULLn_RMSK)
#define HWIO_SB_PGD_PORT_PUSHPULLn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PORT_PUSHPULLn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PORT_PUSHPULLn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PORT_PUSHPULLn_ADDR(base,n),val)
#define HWIO_SB_PGD_PORT_PUSHPULLn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PORT_PUSHPULLn_ADDR(base,n),mask,val,HWIO_SB_PGD_PORT_PUSHPULLn_INI(base,n))
#define HWIO_SB_PGD_PORT_PUSHPULLn_NUM_SAMPLES_BMSK                                            0xffff0000
#define HWIO_SB_PGD_PORT_PUSHPULLn_NUM_SAMPLES_SHFT                                                  0x10
#define HWIO_SB_PGD_PORT_PUSHPULLn_RPT_PERIOD_BMSK                                                 0xffff
#define HWIO_SB_PGD_PORT_PUSHPULLn_RPT_PERIOD_SHFT                                                    0x0

#define HWIO_SB_PGD_IE_STATUS_ADDR(x)                                                          ((x) + 0x00000820)
#define HWIO_SB_PGD_IE_STATUS_RMSK                                                             0xffff000f
#define HWIO_SB_PGD_IE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_IE_STATUS_ADDR(x), HWIO_SB_PGD_IE_STATUS_RMSK)
#define HWIO_SB_PGD_IE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_IE_STATUS_ADDR(x), m)
#define HWIO_SB_PGD_IE_STATUS_DEV_CLASS_CODE_BMSK                                              0xff000000
#define HWIO_SB_PGD_IE_STATUS_DEV_CLASS_CODE_SHFT                                                    0x18
#define HWIO_SB_PGD_IE_STATUS_DEV_CLASS_VER_BMSK                                                 0xff0000
#define HWIO_SB_PGD_IE_STATUS_DEV_CLASS_VER_SHFT                                                     0x10
#define HWIO_SB_PGD_IE_STATUS_EX_ERROR_BMSK                                                           0x8
#define HWIO_SB_PGD_IE_STATUS_EX_ERROR_SHFT                                                           0x3
#define HWIO_SB_PGD_IE_STATUS_RECONFIG_OBJ_BMSK                                                       0x4
#define HWIO_SB_PGD_IE_STATUS_RECONFIG_OBJ_SHFT                                                       0x2
#define HWIO_SB_PGD_IE_STATUS_DATA_TX_COL_BMSK                                                        0x2
#define HWIO_SB_PGD_IE_STATUS_DATA_TX_COL_SHFT                                                        0x1
#define HWIO_SB_PGD_IE_STATUS_UNSPRTD_MSG_BMSK                                                        0x1
#define HWIO_SB_PGD_IE_STATUS_UNSPRTD_MSG_SHFT                                                        0x0

#define HWIO_SB_PGD_VE_STATUS_ADDR(x)                                                          ((x) + 0x00000830)
#define HWIO_SB_PGD_VE_STATUS_RMSK                                                             0xffffffff
#define HWIO_SB_PGD_VE_STATUS_IN(x)      \
        in_dword_masked(HWIO_SB_PGD_VE_STATUS_ADDR(x), HWIO_SB_PGD_VE_STATUS_RMSK)
#define HWIO_SB_PGD_VE_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_SB_PGD_VE_STATUS_ADDR(x), m)
#define HWIO_SB_PGD_VE_STATUS_VE_VAL_BMSK                                                      0xffffffff
#define HWIO_SB_PGD_VE_STATUS_VE_VAL_SHFT                                                             0x0

#define HWIO_SB_PGD_PC_CFGn_ADDR(base,n)                                                       ((base) + 0x00008000 + 0x800 * (n))
#define HWIO_SB_PGD_PC_CFGn_RMSK                                                               0x9fffffff
#define HWIO_SB_PGD_PC_CFGn_MAXn                                                                       23
#define HWIO_SB_PGD_PC_CFGn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_CFGn_ADDR(base,n), HWIO_SB_PGD_PC_CFGn_RMSK)
#define HWIO_SB_PGD_PC_CFGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_CFGn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_CFGn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PC_CFGn_ADDR(base,n),val)
#define HWIO_SB_PGD_PC_CFGn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SB_PGD_PC_CFGn_ADDR(base,n),mask,val,HWIO_SB_PGD_PC_CFGn_INI(base,n))
#define HWIO_SB_PGD_PC_CFGn_INIT_BMSK                                                          0x80000000
#define HWIO_SB_PGD_PC_CFGn_INIT_SHFT                                                                0x1f
#define HWIO_SB_PGD_PC_CFGn_INIT_VAL_BMSK                                                        0xffffff
#define HWIO_SB_PGD_PC_CFGn_INIT_VAL_SHFT                                                             0x0
#define HWIO_SB_PGD_PC_CFGn__V1_PORT_SEL_BMSK                                                  0x1f000000
#define HWIO_SB_PGD_PC_CFGn__V1_PORT_SEL_SHFT                                                        0x18

#define HWIO_SB_PGD_PC_VALn_ADDR(base,n)                                                       ((base) + 0x00008004 + 0x800 * (n))
#define HWIO_SB_PGD_PC_VALn_RMSK                                                               0xffffffff
#define HWIO_SB_PGD_PC_VALn_MAXn                                                                       23
#define HWIO_SB_PGD_PC_VALn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_VALn_ADDR(base,n), HWIO_SB_PGD_PC_VALn_RMSK)
#define HWIO_SB_PGD_PC_VALn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_VALn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_VALn_PC_FIFO_SAMPLES_BMSK                                               0xff000000
#define HWIO_SB_PGD_PC_VALn_PC_FIFO_SAMPLES_SHFT                                                     0x18
#define HWIO_SB_PGD_PC_VALn_PC_DMA_BMSK                                                          0xffffff
#define HWIO_SB_PGD_PC_VALn_PC_DMA_SHFT                                                               0x0

#define HWIO_SB_PGD_PC_VFR_TSn_ADDR(base,n)                                                    ((base) + 0x00008008 + 0x800 * (n))
#define HWIO_SB_PGD_PC_VFR_TSn_RMSK                                                            0xffffffff
#define HWIO_SB_PGD_PC_VFR_TSn_MAXn                                                                    23
#define HWIO_SB_PGD_PC_VFR_TSn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_VFR_TSn_ADDR(base,n), HWIO_SB_PGD_PC_VFR_TSn_RMSK)
#define HWIO_SB_PGD_PC_VFR_TSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_VFR_TSn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_VFR_TSn_VAL_BMSK                                                        0xffffffff
#define HWIO_SB_PGD_PC_VFR_TSn_VAL_SHFT                                                               0x0

#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_ADDR(base,n)                                             ((base) + 0x0000800c + 0x800 * (n))
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_RMSK                                                            0x1
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_MAXn                                                             23
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_VFR_TS_STATUSn_ADDR(base,n), HWIO_SB_PGD_PC_VFR_TS_STATUSn_RMSK)
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_VFR_TS_STATUSn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_VFR_INT_BMSK                                                    0x1
#define HWIO_SB_PGD_PC_VFR_TS_STATUSn_VFR_INT_SHFT                                                    0x0

#define HWIO_SB_PGD_PC_VFR_TS_CLRn_ADDR(base,n)                                                ((base) + 0x00008010 + 0x800 * (n))
#define HWIO_SB_PGD_PC_VFR_TS_CLRn_RMSK                                                               0x1
#define HWIO_SB_PGD_PC_VFR_TS_CLRn_MAXn                                                                23
#define HWIO_SB_PGD_PC_VFR_TS_CLRn_OUTI(base,n,val)    \
        out_dword(HWIO_SB_PGD_PC_VFR_TS_CLRn_ADDR(base,n),val)
#define HWIO_SB_PGD_PC_VFR_TS_CLRn_VFR_INT_BMSK                                                       0x1
#define HWIO_SB_PGD_PC_VFR_TS_CLRn_VFR_INT_SHFT                                                       0x0

#define HWIO_SB_PGD_PC_VAL_SHADOWn_ADDR(base,n)                                                ((base) + 0x00008014 + 0x800 * (n))
#define HWIO_SB_PGD_PC_VAL_SHADOWn_RMSK                                                        0xffffffff
#define HWIO_SB_PGD_PC_VAL_SHADOWn_MAXn                                                                23
#define HWIO_SB_PGD_PC_VAL_SHADOWn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_VAL_SHADOWn_ADDR(base,n), HWIO_SB_PGD_PC_VAL_SHADOWn_RMSK)
#define HWIO_SB_PGD_PC_VAL_SHADOWn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_VAL_SHADOWn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_VAL_SHADOWn_PC_FIFO_SAMPLES_BMSK                                        0xff000000
#define HWIO_SB_PGD_PC_VAL_SHADOWn_PC_FIFO_SAMPLES_SHFT                                              0x18
#define HWIO_SB_PGD_PC_VAL_SHADOWn_PC_DMA_BMSK                                                   0xffffff
#define HWIO_SB_PGD_PC_VAL_SHADOWn_PC_DMA_SHFT                                                        0x0

#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_ADDR(base,n)                                             ((base) + 0x00008018 + 0x800 * (n))
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_RMSK                                                     0xffffffff
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_MAXn                                                             23
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_TIMESTAMP_LSBn_ADDR(base,n), HWIO_SB_PGD_PC_TIMESTAMP_LSBn_RMSK)
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_TIMESTAMP_LSBn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_PC_TIMESTAMP_LSB_BMSK                                    0xffffffff
#define HWIO_SB_PGD_PC_TIMESTAMP_LSBn_PC_TIMESTAMP_LSB_SHFT                                           0x0

#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_ADDR(base,n)                                             ((base) + 0x0000801c + 0x800 * (n))
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_RMSK                                                      0x3ffffff
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_MAXn                                                             23
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_INI(base,n)        \
        in_dword_masked(HWIO_SB_PGD_PC_TIMESTAMP_MSBn_ADDR(base,n), HWIO_SB_PGD_PC_TIMESTAMP_MSBn_RMSK)
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SB_PGD_PC_TIMESTAMP_MSBn_ADDR(base,n), mask)
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_SAMPLE_VALID_BMSK                            0x2000000
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_SAMPLE_VALID_SHFT                                 0x19
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_SAMPLE_MISSED_BMSK                           0x1000000
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_SAMPLE_MISSED_SHFT                                0x18
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_MSB_BMSK                                      0xffffff
#define HWIO_SB_PGD_PC_TIMESTAMP_MSBn_PC_TIMESTAMP_MSB_SHFT                                           0x0


#endif /* __SLIMBUSHWIO_H__ */
