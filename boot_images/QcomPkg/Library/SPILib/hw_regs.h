#ifndef __HW_REGS_H__
#define __HW_REGS_H__
/*
===========================================================================
*/
/**
  @file hw_regs.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p2q1r34.1.0_F03_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    SE
    SE_GENI4_CFG
    SE_GENI4_DATA
    SE_GENI4_IMAGE
    SE_GENI4_IMAGE_REGS
    SE_QUPV3_SE_DMA
    SE_QUPV3_SEC

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2017-2020 Qualcomm Technologies Incorporated.
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
 * MODULE: SE_GENI4_CFG
 *--------------------------------------------------------------------------*/

#define SE_GENI4_CFG_REG_BASE 0x0

#define HWIO_SE_GENI_INIT_CFG_REVISION_OFFS                                        (SE_GENI4_CFG_REG_BASE      + 0x00000000)
#define HWIO_SE_GENI_INIT_CFG_REVISION_RMSK                                              0xff
#define HWIO_SE_GENI_INIT_CFG_REVISION_VERSION_BMSK                                      0xff
#define HWIO_SE_GENI_INIT_CFG_REVISION_VERSION_SHFT                                       0x0

#define HWIO_SE_GENI_S_INIT_CFG_REVISION_OFFS                                      (SE_GENI4_CFG_REG_BASE      + 0x00000004)
#define HWIO_SE_GENI_S_INIT_CFG_REVISION_RMSK                                            0xff
#define HWIO_SE_GENI_S_INIT_CFG_REVISION_VERSION_BMSK                                    0xff
#define HWIO_SE_GENI_S_INIT_CFG_REVISION_VERSION_SHFT                                     0x0

#define HWIO_SE_GENI_GENERAL_CFG_OFFS                                              (SE_GENI4_CFG_REG_BASE      + 0x00000010)
#define HWIO_SE_GENI_GENERAL_CFG_RMSK                                                    0x3f
#define HWIO_SE_GENI_GENERAL_CFG_TX_CRC_MSB_LSB_BMSK                                     0x20
#define HWIO_SE_GENI_GENERAL_CFG_TX_CRC_MSB_LSB_SHFT                                      0x5
#define HWIO_SE_GENI_GENERAL_CFG_CHAR_HUNT_SR_MSB_LSB_BMSK                               0x10
#define HWIO_SE_GENI_GENERAL_CFG_CHAR_HUNT_SR_MSB_LSB_SHFT                                0x4
#define HWIO_SE_GENI_GENERAL_CFG_AUTO_CANCEL_RX_STATUS_SET_BMSK                           0x8
#define HWIO_SE_GENI_GENERAL_CFG_AUTO_CANCEL_RX_STATUS_SET_SHFT                           0x3
#define HWIO_SE_GENI_GENERAL_CFG_CLEAR_IRQS_CMD_START_BMSK                                0x4
#define HWIO_SE_GENI_GENERAL_CFG_CLEAR_IRQS_CMD_START_SHFT                                0x2
#define HWIO_SE_GENI_GENERAL_CFG_SW_IRQ_IO2_RX_DATA_BMSK                                  0x2
#define HWIO_SE_GENI_GENERAL_CFG_SW_IRQ_IO2_RX_DATA_SHFT                                  0x1
#define HWIO_SE_GENI_GENERAL_CFG_SW_RX_FIFO_FLUSH_NO_FW_BMSK                              0x1
#define HWIO_SE_GENI_GENERAL_CFG_SW_RX_FIFO_FLUSH_NO_FW_SHFT                              0x0

#define HWIO_SE_GENI_RX_FIFO_CTRL_OFFS                                             (SE_GENI4_CFG_REG_BASE      + 0x00000014)
#define HWIO_SE_GENI_RX_FIFO_CTRL_RMSK                                                    0x1
#define HWIO_SE_GENI_RX_FIFO_CTRL_SW_RX_FIFO_FLUSH_BMSK                                   0x1
#define HWIO_SE_GENI_RX_FIFO_CTRL_SW_RX_FIFO_FLUSH_SHFT                                   0x0

#define HWIO_SE_GENI_FORCE_DEFAULT_REG_OFFS                                        (SE_GENI4_CFG_REG_BASE      + 0x00000020)
#define HWIO_SE_GENI_FORCE_DEFAULT_REG_RMSK                                               0x1
#define HWIO_SE_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK                                 0x1
#define HWIO_SE_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_SHFT                                 0x0

#define HWIO_SE_GENI_OUTPUT_CTRL_OFFS                                              (SE_GENI4_CFG_REG_BASE      + 0x00000024)
#define HWIO_SE_GENI_OUTPUT_CTRL_RMSK                                                    0x7f
#define HWIO_SE_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_BMSK                                     0x7f
#define HWIO_SE_GENI_OUTPUT_CTRL_IO_OUTPUT_CTRL_SHFT                                      0x0

#define HWIO_SE_GENI_CGC_CTRL_OFFS                                                 (SE_GENI4_CFG_REG_BASE      + 0x00000028)
#define HWIO_SE_GENI_CGC_CTRL_RMSK                                                      0x37f
#define HWIO_SE_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK                                    0x200
#define HWIO_SE_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_SHFT                                      0x9
#define HWIO_SE_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK                                    0x100
#define HWIO_SE_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_SHFT                                      0x8
#define HWIO_SE_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK                                        0x40
#define HWIO_SE_GENI_CGC_CTRL_EXT_CLK_CGC_ON_SHFT                                         0x6
#define HWIO_SE_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK                                         0x20
#define HWIO_SE_GENI_CGC_CTRL_RX_CLK_CGC_ON_SHFT                                          0x5
#define HWIO_SE_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK                                         0x10
#define HWIO_SE_GENI_CGC_CTRL_TX_CLK_CGC_ON_SHFT                                          0x4
#define HWIO_SE_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK                                            0x8
#define HWIO_SE_GENI_CGC_CTRL_SCLK_CGC_ON_SHFT                                            0x3
#define HWIO_SE_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK                                    0x4
#define HWIO_SE_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_SHFT                                    0x2
#define HWIO_SE_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK                                  0x2
#define HWIO_SE_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_SHFT                                  0x1
#define HWIO_SE_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK                                     0x1
#define HWIO_SE_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_SHFT                                     0x0

#define HWIO_SE_GENI_CHAR_CFG_OFFS                                                 (SE_GENI4_CFG_REG_BASE      + 0x0000002c)
#define HWIO_SE_GENI_CHAR_CFG_RMSK                                                    0xfff31
#define HWIO_SE_GENI_CHAR_CFG_CHAR_MODE_BMSK                                          0xff000
#define HWIO_SE_GENI_CHAR_CFG_CHAR_MODE_SHFT                                              0xc
#define HWIO_SE_GENI_CHAR_CFG_CHAR_STATUS_BMSK                                          0xf00
#define HWIO_SE_GENI_CHAR_CFG_CHAR_STATUS_SHFT                                            0x8
#define HWIO_SE_GENI_CHAR_CFG_ENGINE_SEL_BMSK                                            0x30
#define HWIO_SE_GENI_CHAR_CFG_ENGINE_SEL_SHFT                                             0x4
#define HWIO_SE_GENI_CHAR_CFG_ENABLE_BMSK                                                 0x1
#define HWIO_SE_GENI_CHAR_CFG_ENABLE_SHFT                                                 0x0

#define HWIO_SE_GENI_CHAR_DATA_n_OFFS(n)                                           (SE_GENI4_CFG_REG_BASE      + 0x00000030 + 0x4 * (n))
#define HWIO_SE_GENI_CHAR_DATA_n_RMSK                                                  0xffff
#define HWIO_SE_GENI_CHAR_DATA_n_MAXn                                                       1
#define HWIO_SE_GENI_CHAR_DATA_n_DIN_ACTUAL_BMSK                                       0xff00
#define HWIO_SE_GENI_CHAR_DATA_n_DIN_ACTUAL_SHFT                                          0x8
#define HWIO_SE_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                  0xff
#define HWIO_SE_GENI_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                   0x0

#define HWIO_SE_GENI_STATUS_OFFS                                                   (SE_GENI4_CFG_REG_BASE      + 0x00000040)
#define HWIO_SE_GENI_STATUS_RMSK                                                     0x1fffff
#define HWIO_SE_GENI_STATUS_S_GENI_CMD_FSM_STATE_BMSK                                0x1f0000
#define HWIO_SE_GENI_STATUS_S_GENI_CMD_FSM_STATE_SHFT                                    0x10
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_3_BMSK                                       0xe000
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_3_SHFT                                          0xd
#define HWIO_SE_GENI_STATUS_S_GENI_CMD_ACTIVE_BMSK                                     0x1000
#define HWIO_SE_GENI_STATUS_S_GENI_CMD_ACTIVE_SHFT                                        0xc
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_2_BMSK                                        0xe00
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_2_SHFT                                          0x9
#define HWIO_SE_GENI_STATUS_M_GENI_CMD_FSM_STATE_BMSK                                   0x1f0
#define HWIO_SE_GENI_STATUS_M_GENI_CMD_FSM_STATE_SHFT                                     0x4
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_1_BMSK                                          0xe
#define HWIO_SE_GENI_STATUS_NOT_USED_BITS_1_SHFT                                          0x1
#define HWIO_SE_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK                                        0x1
#define HWIO_SE_GENI_STATUS_M_GENI_CMD_ACTIVE_SHFT                                        0x0

#define HWIO_SE_GENI_TEST_BUS_CTRL_OFFS                                            (SE_GENI4_CFG_REG_BASE      + 0x00000044)
#define HWIO_SE_GENI_TEST_BUS_CTRL_RMSK                                                  0x3f
#define HWIO_SE_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_BMSK                                     0x3f
#define HWIO_SE_GENI_TEST_BUS_CTRL_TEST_BUS_SEL_SHFT                                      0x0

#define HWIO_SE_GENI_SER_M_CLK_CFG_OFFS                                            (SE_GENI4_CFG_REG_BASE      + 0x00000048)
#define HWIO_SE_GENI_SER_M_CLK_CFG_RMSK                                                0xfff1
#define HWIO_SE_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_BMSK                                  0xfff0
#define HWIO_SE_GENI_SER_M_CLK_CFG_CLK_DIV_VALUE_SHFT                                     0x4
#define HWIO_SE_GENI_SER_M_CLK_CFG_SER_CLK_EN_BMSK                                        0x1
#define HWIO_SE_GENI_SER_M_CLK_CFG_SER_CLK_EN_SHFT                                        0x0

#define HWIO_SE_GENI_SER_S_CLK_CFG_OFFS                                            (SE_GENI4_CFG_REG_BASE      + 0x0000004c)
#define HWIO_SE_GENI_SER_S_CLK_CFG_RMSK                                                0xfff1
#define HWIO_SE_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_BMSK                                  0xfff0
#define HWIO_SE_GENI_SER_S_CLK_CFG_CLK_DIV_VALUE_SHFT                                     0x4
#define HWIO_SE_GENI_SER_S_CLK_CFG_SER_CLK_EN_BMSK                                        0x1
#define HWIO_SE_GENI_SER_S_CLK_CFG_SER_CLK_EN_SHFT                                        0x0

#define HWIO_SE_GENI_PROG_ROM_CTRL_REG_OFFS                                        (SE_GENI4_CFG_REG_BASE      + 0x00000050)
#define HWIO_SE_GENI_PROG_ROM_CTRL_REG_RMSK                                               0x1
#define HWIO_SE_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_BMSK                                   0x1
#define HWIO_SE_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_SHFT                                   0x0

#define HWIO_SE_GENI_CLK_CTRL_RO_OFFS                                              (SE_GENI4_CFG_REG_BASE      + 0x00000060)
#define HWIO_SE_GENI_CLK_CTRL_RO_RMSK                                                     0x1
#define HWIO_SE_GENI_CLK_CTRL_RO_SER_CLK_SEL_BMSK                                         0x1
#define HWIO_SE_GENI_CLK_CTRL_RO_SER_CLK_SEL_SHFT                                         0x0

#define HWIO_SE_FIFO_IF_DISABLE_RO_OFFS                                            (SE_GENI4_CFG_REG_BASE      + 0x00000064)
#define HWIO_SE_FIFO_IF_DISABLE_RO_RMSK                                                   0x1
#define HWIO_SE_FIFO_IF_DISABLE_RO_FIFO_IF_DISABLE_BMSK                                   0x1
#define HWIO_SE_FIFO_IF_DISABLE_RO_FIFO_IF_DISABLE_SHFT                                   0x0

#define HWIO_SE_GENI_FW_REVISION_RO_OFFS                                           (SE_GENI4_CFG_REG_BASE      + 0x00000068)
#define HWIO_SE_GENI_FW_REVISION_RO_RMSK                                               0xffff
#define HWIO_SE_GENI_FW_REVISION_RO_PROTOCOL_BMSK                                      0xff00
#define HWIO_SE_GENI_FW_REVISION_RO_PROTOCOL_SHFT                                         0x8
#define HWIO_SE_GENI_FW_REVISION_RO_VERSION_BMSK                                         0xff
#define HWIO_SE_GENI_FW_REVISION_RO_VERSION_SHFT                                          0x0

#define HWIO_SE_GENI_S_FW_REVISION_RO_OFFS                                         (SE_GENI4_CFG_REG_BASE      + 0x0000006c)
#define HWIO_SE_GENI_S_FW_REVISION_RO_RMSK                                             0xffff
#define HWIO_SE_GENI_S_FW_REVISION_RO_PROTOCOL_BMSK                                    0xff00
#define HWIO_SE_GENI_S_FW_REVISION_RO_PROTOCOL_SHFT                                       0x8
#define HWIO_SE_GENI_S_FW_REVISION_RO_VERSION_BMSK                                       0xff
#define HWIO_SE_GENI_S_FW_REVISION_RO_VERSION_SHFT                                        0x0

#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_RO_OFFS                                   (SE_GENI4_CFG_REG_BASE      + 0x00000070)
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_RO_RMSK                                          0x1
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_RO_GENI_FW_MULTILOCK_PROTNS_BMSK                 0x1
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_RO_GENI_FW_MULTILOCK_PROTNS_SHFT                 0x0

#define HWIO_SE_GENI_FW_MULTILOCK_MSA_RO_OFFS                                      (SE_GENI4_CFG_REG_BASE      + 0x00000074)
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_RO_RMSK                                             0x1
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_RO_GENI_FW_MULTILOCK_MSA_BMSK                       0x1
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_RO_GENI_FW_MULTILOCK_MSA_SHFT                       0x0

#define HWIO_SE_GENI_FW_MULTILOCK_SP_RO_OFFS                                       (SE_GENI4_CFG_REG_BASE      + 0x00000078)
#define HWIO_SE_GENI_FW_MULTILOCK_SP_RO_RMSK                                              0x1
#define HWIO_SE_GENI_FW_MULTILOCK_SP_RO_GENI_FW_MULTILOCK_SP_BMSK                         0x1
#define HWIO_SE_GENI_FW_MULTILOCK_SP_RO_GENI_FW_MULTILOCK_SP_SHFT                         0x0

#define HWIO_SE_GENI_CLK_SEL_OFFS                                                  (SE_GENI4_CFG_REG_BASE      + 0x0000007c)
#define HWIO_SE_GENI_CLK_SEL_RMSK                                                         0x7
#define HWIO_SE_GENI_CLK_SEL_CLK_SEL_BMSK                                                 0x7
#define HWIO_SE_GENI_CLK_SEL_CLK_SEL_SHFT                                                 0x0

#define HWIO_SE_GENI_DFS_IF_CFG_OFFS                                               (SE_GENI4_CFG_REG_BASE      + 0x00000080)
#define HWIO_SE_GENI_DFS_IF_CFG_RMSK                                                    0x701
#define HWIO_SE_GENI_DFS_IF_CFG_NUM_WAIT_STATES_BMSK                                    0x700
#define HWIO_SE_GENI_DFS_IF_CFG_NUM_WAIT_STATES_SHFT                                      0x8
#define HWIO_SE_GENI_DFS_IF_CFG_DFS_IF_EN_BMSK                                            0x1
#define HWIO_SE_GENI_DFS_IF_CFG_DFS_IF_EN_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_GENI4_DATA
 *--------------------------------------------------------------------------*/

#define SE_GENI4_DATA_REG_BASE                                               (SE_GENI4_CFG_REG_BASE + 0x00000600)

#define HWIO_SE_GENI_M_CMD0_OFFS                                             (SE_GENI4_DATA_REG_BASE      + 0x00000000)
#define HWIO_SE_GENI_M_CMD0_RMSK                                             0xf9ffffff
#define HWIO_SE_GENI_M_CMD0_OPCODE_BMSK                                      0xf8000000
#define HWIO_SE_GENI_M_CMD0_OPCODE_SHFT                                            0x1b
#define HWIO_SE_GENI_M_CMD0_GNRL_PURP_BMSK                                    0x1000000
#define HWIO_SE_GENI_M_CMD0_GNRL_PURP_SHFT                                         0x18
#define HWIO_SE_GENI_M_CMD0_PARAM_BMSK                                         0xffffff
#define HWIO_SE_GENI_M_CMD0_PARAM_SHFT                                              0x0

#define HWIO_SE_GENI_M_CMD0_I2C_SLAVE_ADDRESS_BMSK                               0xFE00
#define HWIO_SE_GENI_M_CMD0_I2C_SLAVE_ADDRESS_SHFT                                  0x9

#define HWIO_SE_GENI_M_CMD0_I2C_PRE_CMD_DELAY_BMSK                           0x00000001
#define HWIO_SE_GENI_M_CMD0_I2C_PRE_CMD_DELAY_SHFT                                  0x0

#define HWIO_SE_GENI_M_CMD0_I2C_TIMESTAMP_BEFORE_BMSK                        0x00000002
#define HWIO_SE_GENI_M_CMD0_I2C_TIMESTAMP_BEFORE_SHFT                               0x1

#define HWIO_SE_GENI_M_CMD0_I2C_STRETCH_BMSK                                 0x00000004
#define HWIO_SE_GENI_M_CMD0_I2C_STRETCH_SHFT                                        0x2

#define HWIO_SE_GENI_M_CMD0_I2C_TIMESTAMP_AFTER_BMSK                         0x00000008
#define HWIO_SE_GENI_M_CMD0_I2C_TIMESTAMP_AFTER_SHFT                                0x3

#define HWIO_SE_GENI_M_CMD0_POST_CMD_DELAY_BMSK                              0x00000010
#define HWIO_SE_GENI_M_CMD0_POST_CMD_DELAY_SHFT                                     0x4

#define HWIO_SE_GENI_M_CMD0_I2C_IGNORE_ADDR_NACK_BMSK                        0x00000040
#define HWIO_SE_GENI_M_CMD0_I2C_IGNORE_ADDR_NACK_SHFT                               0x6

#define HWIO_SE_GENI_M_CMD0_I2C_READ_FINISH_WITH_ACK_BMSK                    0x00000080
#define HWIO_SE_GENI_M_CMD0_I2C_READ_FINISH_WITH_ACK_SHFT                           0x7

#define HWIO_SE_GENI_M_CMD0_I2C_BYPASS_ADDRESS_PHASE_BMSK                    0x00000100
#define HWIO_SE_GENI_M_CMD0_I2C_BYPASS_ADDRESS_PHASE_SHFT                           0x8

#define HWIO_SE_GENI_M_CMD_CTRL_REG_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000004)
#define HWIO_SE_GENI_M_CMD_CTRL_REG_RMSK                                            0x7
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_CMD_CANCEL_BMSK                          0x4
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_CMD_CANCEL_SHFT                          0x2
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_CMD_ABORT_BMSK                           0x2
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_CMD_ABORT_SHFT                           0x1
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_DISABLE_BMSK                             0x1
#define HWIO_SE_GENI_M_CMD_CTRL_REG_M_GENI_DISABLE_SHFT                             0x0

#define HWIO_SE_GENI_M_IRQ_STATUS_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x00000010)
#define HWIO_SE_GENI_M_IRQ_STATUS_RMSK                                       0xffc07fff
#define HWIO_SE_GENI_M_IRQ_STATUS_SEC_IRQ_BMSK                               0x80000000
#define HWIO_SE_GENI_M_IRQ_STATUS_SEC_IRQ_SHFT                                     0x1f
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_BMSK                     0x40000000
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_SHFT                           0x1e
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_BMSK                        0x20000000
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_WR_ERR_SHFT                              0x1d
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_BMSK                        0x10000000
#define HWIO_SE_GENI_M_IRQ_STATUS_TX_FIFO_RD_ERR_SHFT                              0x1c
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_LAST_BMSK                           0x8000000
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_LAST_SHFT                                0x1b
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                      0x4000000
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                           0x1a
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                         0x2000000
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                              0x19
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                         0x1000000
#define HWIO_SE_GENI_M_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                              0x18
#define HWIO_SE_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_BMSK                          0x800000
#define HWIO_SE_GENI_M_IRQ_STATUS_IO_DATA_ASSERT_SHFT                              0x17
#define HWIO_SE_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                        0x400000
#define HWIO_SE_GENI_M_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                            0x16
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_5_BMSK                                0x4000
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_5_SHFT                                   0xe
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_4_BMSK                                0x2000
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_4_SHFT                                   0xd
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_3_BMSK                                0x1000
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_3_SHFT                                   0xc
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_2_BMSK                                 0x800
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_2_SHFT                                   0xb
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_1_BMSK                                 0x400
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_1_SHFT                                   0xa
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_0_BMSK                                 0x200
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_IRQ_0_SHFT                                   0x9
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_BMSK                            0x100
#define HWIO_SE_GENI_M_IRQ_STATUS_M_GP_SYNC_IRQ_0_SHFT                              0x8
#define HWIO_SE_GENI_M_IRQ_STATUS_M_RX_IRQ_BMSK                                    0x80
#define HWIO_SE_GENI_M_IRQ_STATUS_M_RX_IRQ_SHFT                                     0x7
#define HWIO_SE_GENI_M_IRQ_STATUS_M_TIMESTAMP_BMSK                                 0x40
#define HWIO_SE_GENI_M_IRQ_STATUS_M_TIMESTAMP_SHFT                                  0x6
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_ABORT_BMSK                                 0x20
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_ABORT_SHFT                                  0x5
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_CANCEL_BMSK                                0x10
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_CANCEL_SHFT                                 0x4
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_FAILURE_BMSK                                0x8
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_FAILURE_SHFT                                0x3
#define HWIO_SE_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_BMSK                                0x4
#define HWIO_SE_GENI_M_IRQ_STATUS_M_ILLEGAL_CMD_SHFT                                0x2
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_BMSK                                0x2
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_OVERRUN_SHFT                                0x1
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_DONE_BMSK                                   0x1
#define HWIO_SE_GENI_M_IRQ_STATUS_M_CMD_DONE_SHFT                                   0x0

#define HWIO_SE_GENI_M_IRQ_ENABLE_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x00000014)
#define HWIO_SE_GENI_M_IRQ_ENABLE_RMSK                                       0xffc07fff
#define HWIO_SE_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_BMSK                            0x80000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_SEC_IRQ_EN_SHFT                                  0x1f
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_BMSK                  0x40000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_WATERMARK_EN_SHFT                        0x1e
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_BMSK                     0x20000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_WR_ERR_EN_SHFT                           0x1d
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_BMSK                     0x10000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_TX_FIFO_RD_ERR_EN_SHFT                           0x1c
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                        0x8000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                             0x1b
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                   0x4000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                        0x1a
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                      0x2000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                           0x19
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                      0x1000000
#define HWIO_SE_GENI_M_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                           0x18
#define HWIO_SE_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                       0x800000
#define HWIO_SE_GENI_M_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                           0x17
#define HWIO_SE_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                     0x400000
#define HWIO_SE_GENI_M_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                         0x16
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_BMSK                             0x4000
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_5_EN_SHFT                                0xe
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_BMSK                             0x2000
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_4_EN_SHFT                                0xd
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_BMSK                             0x1000
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_3_EN_SHFT                                0xc
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_BMSK                              0x800
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_2_EN_SHFT                                0xb
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_BMSK                              0x400
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_1_EN_SHFT                                0xa
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_BMSK                              0x200
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_IRQ_0_EN_SHFT                                0x9
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_BMSK                         0x100
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_GP_SYNC_IRQ_0_EN_SHFT                           0x8
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_BMSK                                 0x80
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_RX_IRQ_EN_SHFT                                  0x7
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_BMSK                              0x40
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_TIMESTAMP_EN_SHFT                               0x6
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_BMSK                              0x20
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_ABORT_EN_SHFT                               0x5
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_BMSK                             0x10
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_CANCEL_EN_SHFT                              0x4
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_BMSK                             0x8
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_FAILURE_EN_SHFT                             0x3
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_BMSK                             0x4
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_ILLEGAL_CMD_EN_SHFT                             0x2
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_BMSK                             0x2
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_OVERRUN_EN_SHFT                             0x1
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_BMSK                                0x1
#define HWIO_SE_GENI_M_IRQ_ENABLE_M_CMD_DONE_EN_SHFT                                0x0

#define HWIO_SE_GENI_M_IRQ_CLEAR_OFFS                                        (SE_GENI4_DATA_REG_BASE      + 0x00000018)
#define HWIO_SE_GENI_M_IRQ_CLEAR_RMSK                                        0xffc07fff
#define HWIO_SE_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_BMSK                          0x80000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_SEC_IRQ_CLEAR_SHFT                                0x1f
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_BMSK                0x40000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WATERMARK_CLEAR_SHFT                      0x1e
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_BMSK                   0x20000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_WR_ERR_CLEAR_SHFT                         0x1d
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_BMSK                   0x10000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_TX_FIFO_RD_ERR_CLEAR_SHFT                         0x1c
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                      0x8000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                           0x1b
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                 0x4000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                      0x1a
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                    0x2000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                         0x19
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                    0x1000000
#define HWIO_SE_GENI_M_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                         0x18
#define HWIO_SE_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                     0x800000
#define HWIO_SE_GENI_M_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                         0x17
#define HWIO_SE_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                   0x400000
#define HWIO_SE_GENI_M_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                       0x16
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_BMSK                           0x4000
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_5_CLEAR_SHFT                              0xe
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_BMSK                           0x2000
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_4_CLEAR_SHFT                              0xd
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_BMSK                           0x1000
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_3_CLEAR_SHFT                              0xc
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_BMSK                            0x800
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_2_CLEAR_SHFT                              0xb
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_BMSK                            0x400
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_1_CLEAR_SHFT                              0xa
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_BMSK                            0x200
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_IRQ_0_CLEAR_SHFT                              0x9
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_BMSK                       0x100
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_GP_SYNC_IRQ_0_CLEAR_SHFT                         0x8
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_BMSK                               0x80
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_RX_IRQ_CLEAR_SHFT                                0x7
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_BMSK                            0x40
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_TIMESTAMP_CLEAR_SHFT                             0x6
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_BMSK                            0x20
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_ABORT_CLEAR_SHFT                             0x5
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_BMSK                           0x10
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_CANCEL_CLEAR_SHFT                            0x4
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_BMSK                           0x8
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_FAILURE_CLEAR_SHFT                           0x3
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_BMSK                           0x4
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_ILLEGAL_CMD_CLEAR_SHFT                           0x2
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_BMSK                           0x2
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_OVERRUN_CLEAR_SHFT                           0x1
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_BMSK                              0x1
#define HWIO_SE_GENI_M_IRQ_CLEAR_M_CMD_DONE_CLEAR_SHFT                              0x0

#define HWIO_SE_GENI_M_IRQ_EN_SET_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x0000001c)
#define HWIO_SE_GENI_M_IRQ_EN_SET_RMSK                                       0xffc07fff
#define HWIO_SE_GENI_M_IRQ_EN_SET_SEC_IRQ_EN_SET_BMSK                        0x80000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_SEC_IRQ_EN_SET_SHFT                              0x1f
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_WATERMARK_EN_SET_BMSK              0x40000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_WATERMARK_EN_SET_SHFT                    0x1e
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_WR_ERR_EN_SET_BMSK                 0x20000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_WR_ERR_EN_SET_SHFT                       0x1d
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_RD_ERR_EN_SET_BMSK                 0x10000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_TX_FIFO_RD_ERR_EN_SET_SHFT                       0x1c
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_BMSK                    0x8000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_SHFT                         0x1b
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_BMSK               0x4000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_SHFT                    0x1a
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_BMSK                  0x2000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_SHFT                       0x19
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_BMSK                  0x1000000
#define HWIO_SE_GENI_M_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_SHFT                       0x18
#define HWIO_SE_GENI_M_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_BMSK                   0x800000
#define HWIO_SE_GENI_M_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_SHFT                       0x17
#define HWIO_SE_GENI_M_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_BMSK                 0x400000
#define HWIO_SE_GENI_M_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_SHFT                     0x16
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_5_EN_SET_BMSK                         0x4000
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_5_EN_SET_SHFT                            0xe
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_4_EN_SET_BMSK                         0x2000
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_4_EN_SET_SHFT                            0xd
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_3_EN_SET_BMSK                         0x1000
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_3_EN_SET_SHFT                            0xc
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_2_EN_SET_BMSK                          0x800
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_2_EN_SET_SHFT                            0xb
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_1_EN_SET_BMSK                          0x400
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_1_EN_SET_SHFT                            0xa
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_0_EN_SET_BMSK                          0x200
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_IRQ_0_EN_SET_SHFT                            0x9
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_SYNC_IRQ_0_EN_SET_BMSK                     0x100
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_GP_SYNC_IRQ_0_EN_SET_SHFT                       0x8
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_RX_IRQ_EN_SET_BMSK                             0x80
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_RX_IRQ_EN_SET_SHFT                              0x7
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_TIMESTAMP_EN_SET_BMSK                          0x40
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_TIMESTAMP_EN_SET_SHFT                           0x6
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_ABORT_EN_SET_BMSK                          0x20
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_ABORT_EN_SET_SHFT                           0x5
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_CANCEL_EN_SET_BMSK                         0x10
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_CANCEL_EN_SET_SHFT                          0x4
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_FAILURE_EN_SET_BMSK                         0x8
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_FAILURE_EN_SET_SHFT                         0x3
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_ILLEGAL_CMD_EN_SET_BMSK                         0x4
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_ILLEGAL_CMD_EN_SET_SHFT                         0x2
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_OVERRUN_EN_SET_BMSK                         0x2
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_OVERRUN_EN_SET_SHFT                         0x1
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_DONE_EN_SET_BMSK                            0x1
#define HWIO_SE_GENI_M_IRQ_EN_SET_M_CMD_DONE_EN_SET_SHFT                            0x0

#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000020)
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RMSK                                     0xffc07fff
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_SEC_IRQ_EN_CLEAR_BMSK                    0x80000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_SEC_IRQ_EN_CLEAR_SHFT                          0x1f
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WATERMARK_EN_CLEAR_BMSK          0x40000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WATERMARK_EN_CLEAR_SHFT                0x1e
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WR_ERR_EN_CLEAR_BMSK             0x20000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_WR_ERR_EN_CLEAR_SHFT                   0x1d
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_RD_ERR_EN_CLEAR_BMSK             0x10000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_TX_FIFO_RD_ERR_EN_CLEAR_SHFT                   0x1c
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_BMSK                0x8000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_SHFT                     0x1b
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_BMSK           0x4000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_SHFT                0x1a
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_BMSK              0x2000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_SHFT                   0x19
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_BMSK              0x1000000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_SHFT                   0x18
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_BMSK               0x800000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_SHFT                   0x17
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_BMSK             0x400000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_SHFT                 0x16
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_5_EN_CLEAR_BMSK                     0x4000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_5_EN_CLEAR_SHFT                        0xe
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_4_EN_CLEAR_BMSK                     0x2000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_4_EN_CLEAR_SHFT                        0xd
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_3_EN_CLEAR_BMSK                     0x1000
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_3_EN_CLEAR_SHFT                        0xc
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_2_EN_CLEAR_BMSK                      0x800
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_2_EN_CLEAR_SHFT                        0xb
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_1_EN_CLEAR_BMSK                      0x400
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_1_EN_CLEAR_SHFT                        0xa
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_0_EN_CLEAR_BMSK                      0x200
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_IRQ_0_EN_CLEAR_SHFT                        0x9
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_SYNC_IRQ_0_EN_CLEAR_BMSK                 0x100
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_GP_SYNC_IRQ_0_EN_CLEAR_SHFT                   0x8
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_RX_IRQ_EN_CLEAR_BMSK                         0x80
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_RX_IRQ_EN_CLEAR_SHFT                          0x7
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_TIMESTAMP_EN_CLEAR_BMSK                      0x40
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_TIMESTAMP_EN_CLEAR_SHFT                       0x6
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_ABORT_EN_CLEAR_BMSK                      0x20
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_ABORT_EN_CLEAR_SHFT                       0x5
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_CANCEL_EN_CLEAR_BMSK                     0x10
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_CANCEL_EN_CLEAR_SHFT                      0x4
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_FAILURE_EN_CLEAR_BMSK                     0x8
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_FAILURE_EN_CLEAR_SHFT                     0x3
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_ILLEGAL_CMD_EN_CLEAR_BMSK                     0x4
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_ILLEGAL_CMD_EN_CLEAR_SHFT                     0x2
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_OVERRUN_EN_CLEAR_BMSK                     0x2
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_OVERRUN_EN_CLEAR_SHFT                     0x1
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_DONE_EN_CLEAR_BMSK                        0x1
#define HWIO_SE_GENI_M_IRQ_EN_CLEAR_M_CMD_DONE_EN_CLEAR_SHFT                        0x0

#define HWIO_SE_GENI_S_CMD0_OFFS                                             (SE_GENI4_DATA_REG_BASE      + 0x00000030)
#define HWIO_SE_GENI_S_CMD0_RMSK                                             0xf9ffffff
#define HWIO_SE_GENI_S_CMD0_OPCODE_BMSK                                      0xf8000000
#define HWIO_SE_GENI_S_CMD0_OPCODE_SHFT                                            0x1b
#define HWIO_SE_GENI_S_CMD0_GNRL_PURP_BMSK                                    0x1000000
#define HWIO_SE_GENI_S_CMD0_GNRL_PURP_SHFT                                         0x18
#define HWIO_SE_GENI_S_CMD0_PARAM_BMSK                                         0xffffff
#define HWIO_SE_GENI_S_CMD0_PARAM_SHFT                                              0x0

#define HWIO_SE_GENI_S_CMD_CTRL_REG_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000034)
#define HWIO_SE_GENI_S_CMD_CTRL_REG_RMSK                                            0x7
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_CMD_CANCEL_BMSK                          0x4
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_CMD_CANCEL_SHFT                          0x2
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_CMD_ABORT_BMSK                           0x2
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_CMD_ABORT_SHFT                           0x1
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_DISABLE_BMSK                             0x1
#define HWIO_SE_GENI_S_CMD_CTRL_REG_S_GENI_DISABLE_SHFT                             0x0

#define HWIO_SE_GENI_S_IRQ_STATUS_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x00000040)
#define HWIO_SE_GENI_S_IRQ_STATUS_RMSK                                        0xfc07f3f
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_LAST_BMSK                           0x8000000
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_LAST_SHFT                                0x1b
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK                      0x4000000
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_SHFT                           0x1a
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_BMSK                         0x2000000
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_WR_ERR_SHFT                              0x19
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_BMSK                         0x1000000
#define HWIO_SE_GENI_S_IRQ_STATUS_RX_FIFO_RD_ERR_SHFT                              0x18
#define HWIO_SE_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_BMSK                          0x800000
#define HWIO_SE_GENI_S_IRQ_STATUS_IO_DATA_ASSERT_SHFT                              0x17
#define HWIO_SE_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_BMSK                        0x400000
#define HWIO_SE_GENI_S_IRQ_STATUS_IO_DATA_DEASSERT_SHFT                            0x16
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_5_BMSK                                0x4000
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_5_SHFT                                   0xe
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_4_BMSK                                0x2000
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_4_SHFT                                   0xd
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_3_BMSK                                0x1000
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_3_SHFT                                   0xc
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_2_BMSK                                 0x800
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_2_SHFT                                   0xb
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_1_BMSK                                 0x400
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_1_SHFT                                   0xa
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_0_BMSK                                 0x200
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_IRQ_0_SHFT                                   0x9
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_BMSK                            0x100
#define HWIO_SE_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_SHFT                              0x8
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_ABORT_BMSK                                 0x20
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_ABORT_SHFT                                  0x5
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_CANCEL_BMSK                                0x10
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_CANCEL_SHFT                                 0x4
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_FAILURE_BMSK                                0x8
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_FAILURE_SHFT                                0x3
#define HWIO_SE_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_BMSK                                0x4
#define HWIO_SE_GENI_S_IRQ_STATUS_S_ILLEGAL_CMD_SHFT                                0x2
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_BMSK                                0x2
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_OVERRUN_SHFT                                0x1
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_DONE_BMSK                                   0x1
#define HWIO_SE_GENI_S_IRQ_STATUS_S_CMD_DONE_SHFT                                   0x0

#define HWIO_SE_GENI_S_IRQ_ENABLE_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x00000044)
#define HWIO_SE_GENI_S_IRQ_ENABLE_RMSK                                        0xfc07f3f
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_BMSK                        0x8000000
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_LAST_EN_SHFT                             0x1b
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_BMSK                   0x4000000
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_WATERMARK_EN_SHFT                        0x1a
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_BMSK                      0x2000000
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_WR_ERR_EN_SHFT                           0x19
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_BMSK                      0x1000000
#define HWIO_SE_GENI_S_IRQ_ENABLE_RX_FIFO_RD_ERR_EN_SHFT                           0x18
#define HWIO_SE_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_BMSK                       0x800000
#define HWIO_SE_GENI_S_IRQ_ENABLE_IO_DATA_ASSERT_EN_SHFT                           0x17
#define HWIO_SE_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_BMSK                     0x400000
#define HWIO_SE_GENI_S_IRQ_ENABLE_IO_DATA_DEASSERT_EN_SHFT                         0x16
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_BMSK                             0x4000
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_5_EN_SHFT                                0xe
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_BMSK                             0x2000
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_4_EN_SHFT                                0xd
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_BMSK                             0x1000
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_3_EN_SHFT                                0xc
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_BMSK                              0x800
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_2_EN_SHFT                                0xb
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_BMSK                              0x400
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_1_EN_SHFT                                0xa
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_BMSK                              0x200
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_IRQ_0_EN_SHFT                                0x9
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_BMSK                         0x100
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_GP_SYNC_IRQ_0_EN_SHFT                           0x8
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_BMSK                              0x20
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_ABORT_EN_SHFT                               0x5
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_BMSK                             0x10
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_CANCEL_EN_SHFT                              0x4
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_BMSK                             0x8
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_FAILURE_EN_SHFT                             0x3
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_BMSK                             0x4
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_ILLEGAL_CMD_EN_SHFT                             0x2
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_BMSK                             0x2
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_OVERRUN_EN_SHFT                             0x1
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_BMSK                                0x1
#define HWIO_SE_GENI_S_IRQ_ENABLE_S_CMD_DONE_EN_SHFT                                0x0

#define HWIO_SE_GENI_S_IRQ_CLEAR_OFFS                                        (SE_GENI4_DATA_REG_BASE      + 0x00000048)
#define HWIO_SE_GENI_S_IRQ_CLEAR_RMSK                                         0xfc07f3f
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_BMSK                      0x8000000
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_LAST_CLEAR_SHFT                           0x1b
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_BMSK                 0x4000000
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_WATERMARK_CLEAR_SHFT                      0x1a
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_BMSK                    0x2000000
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_WR_ERR_CLEAR_SHFT                         0x19
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_BMSK                    0x1000000
#define HWIO_SE_GENI_S_IRQ_CLEAR_RX_FIFO_RD_ERR_CLEAR_SHFT                         0x18
#define HWIO_SE_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_BMSK                     0x800000
#define HWIO_SE_GENI_S_IRQ_CLEAR_IO_DATA_ASSERT_CLEAR_SHFT                         0x17
#define HWIO_SE_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_BMSK                   0x400000
#define HWIO_SE_GENI_S_IRQ_CLEAR_IO_DATA_DEASSERT_CLEAR_SHFT                       0x16
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_BMSK                           0x4000
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_5_CLEAR_SHFT                              0xe
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_BMSK                           0x2000
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_4_CLEAR_SHFT                              0xd
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_BMSK                           0x1000
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_3_CLEAR_SHFT                              0xc
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_BMSK                            0x800
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_2_CLEAR_SHFT                              0xb
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_BMSK                            0x400
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_1_CLEAR_SHFT                              0xa
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_BMSK                            0x200
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_IRQ_0_CLEAR_SHFT                              0x9
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_BMSK                       0x100
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_GP_SYNC_IRQ_0_CLEAR_SHFT                         0x8
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_BMSK                            0x20
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_ABORT_CLEAR_SHFT                             0x5
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_BMSK                           0x10
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_CANCEL_CLEAR_SHFT                            0x4
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_BMSK                           0x8
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_FAILURE_CLEAR_SHFT                           0x3
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_BMSK                           0x4
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_ILLEGAL_CMD_CLEAR_SHFT                           0x2
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_BMSK                           0x2
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_OVERRUN_CLEAR_SHFT                           0x1
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_BMSK                              0x1
#define HWIO_SE_GENI_S_IRQ_CLEAR_S_CMD_DONE_CLEAR_SHFT                              0x0

#define HWIO_SE_GENI_S_IRQ_EN_SET_OFFS                                       (SE_GENI4_DATA_REG_BASE      + 0x0000004c)
#define HWIO_SE_GENI_S_IRQ_EN_SET_RMSK                                        0xfc07f3f
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_BMSK                    0x8000000
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_LAST_EN_SET_SHFT                         0x1b
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_BMSK               0x4000000
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_WATERMARK_EN_SET_SHFT                    0x1a
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_BMSK                  0x2000000
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_WR_ERR_EN_SET_SHFT                       0x19
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_BMSK                  0x1000000
#define HWIO_SE_GENI_S_IRQ_EN_SET_RX_FIFO_RD_ERR_EN_SET_SHFT                       0x18
#define HWIO_SE_GENI_S_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_BMSK                   0x800000
#define HWIO_SE_GENI_S_IRQ_EN_SET_IO_DATA_ASSERT_EN_SET_SHFT                       0x17
#define HWIO_SE_GENI_S_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_BMSK                 0x400000
#define HWIO_SE_GENI_S_IRQ_EN_SET_IO_DATA_DEASSERT_EN_SET_SHFT                     0x16
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_5_EN_SET_BMSK                         0x4000
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_5_EN_SET_SHFT                            0xe
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_4_EN_SET_BMSK                         0x2000
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_4_EN_SET_SHFT                            0xd
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_3_EN_SET_BMSK                         0x1000
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_3_EN_SET_SHFT                            0xc
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_2_EN_SET_BMSK                          0x800
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_2_EN_SET_SHFT                            0xb
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_1_EN_SET_BMSK                          0x400
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_1_EN_SET_SHFT                            0xa
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_0_EN_SET_BMSK                          0x200
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_IRQ_0_EN_SET_SHFT                            0x9
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_SYNC_IRQ_0_EN_SET_BMSK                     0x100
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_GP_SYNC_IRQ_0_EN_SET_SHFT                       0x8
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_ABORT_EN_SET_BMSK                          0x20
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_ABORT_EN_SET_SHFT                           0x5
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_CANCEL_EN_SET_BMSK                         0x10
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_CANCEL_EN_SET_SHFT                          0x4
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_FAILURE_EN_SET_BMSK                         0x8
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_FAILURE_EN_SET_SHFT                         0x3
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_ILLEGAL_CMD_EN_SET_BMSK                         0x4
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_ILLEGAL_CMD_EN_SET_SHFT                         0x2
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_OVERRUN_EN_SET_BMSK                         0x2
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_OVERRUN_EN_SET_SHFT                         0x1
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_DONE_EN_SET_BMSK                            0x1
#define HWIO_SE_GENI_S_IRQ_EN_SET_S_CMD_DONE_EN_SET_SHFT                            0x0

#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000050)
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RMSK                                      0xfc07f3f
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_BMSK                0x8000000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_LAST_EN_CLEAR_SHFT                     0x1b
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_BMSK           0x4000000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WATERMARK_EN_CLEAR_SHFT                0x1a
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_BMSK              0x2000000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_WR_ERR_EN_CLEAR_SHFT                   0x19
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_BMSK              0x1000000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_RX_FIFO_RD_ERR_EN_CLEAR_SHFT                   0x18
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_BMSK               0x800000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_IO_DATA_ASSERT_EN_CLEAR_SHFT                   0x17
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_BMSK             0x400000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_IO_DATA_DEASSERT_EN_CLEAR_SHFT                 0x16
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_5_EN_CLEAR_BMSK                     0x4000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_5_EN_CLEAR_SHFT                        0xe
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_4_EN_CLEAR_BMSK                     0x2000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_4_EN_CLEAR_SHFT                        0xd
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_3_EN_CLEAR_BMSK                     0x1000
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_3_EN_CLEAR_SHFT                        0xc
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_2_EN_CLEAR_BMSK                      0x800
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_2_EN_CLEAR_SHFT                        0xb
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_1_EN_CLEAR_BMSK                      0x400
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_1_EN_CLEAR_SHFT                        0xa
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_0_EN_CLEAR_BMSK                      0x200
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_IRQ_0_EN_CLEAR_SHFT                        0x9
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_SYNC_IRQ_0_EN_CLEAR_BMSK                 0x100
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_GP_SYNC_IRQ_0_EN_CLEAR_SHFT                   0x8
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_ABORT_EN_CLEAR_BMSK                      0x20
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_ABORT_EN_CLEAR_SHFT                       0x5
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_CANCEL_EN_CLEAR_BMSK                     0x10
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_CANCEL_EN_CLEAR_SHFT                      0x4
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_FAILURE_EN_CLEAR_BMSK                     0x8
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_FAILURE_EN_CLEAR_SHFT                     0x3
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_ILLEGAL_CMD_EN_CLEAR_BMSK                     0x4
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_ILLEGAL_CMD_EN_CLEAR_SHFT                     0x2
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_OVERRUN_EN_CLEAR_BMSK                     0x2
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_OVERRUN_EN_CLEAR_SHFT                     0x1
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_DONE_EN_CLEAR_BMSK                        0x1
#define HWIO_SE_GENI_S_IRQ_EN_CLEAR_S_CMD_DONE_EN_CLEAR_SHFT                        0x0

#define HWIO_SE_GENI_TX_FIFOn_OFFS                                           (SE_GENI4_DATA_REG_BASE      + 0x00000100)
#define HWIO_SE_GENI_TX_FIFOn_RMSK                                           0xffffffff
#define HWIO_SE_GENI_TX_FIFOn_MAXn                                                   15
#define HWIO_SE_GENI_TX_FIFOn_TX_DATA_BMSK                                   0xffffffff
#define HWIO_SE_GENI_TX_FIFOn_TX_DATA_SHFT                                          0x0

#define HWIO_SE_GENI_RX_FIFOn_OFFS                                           (SE_GENI4_DATA_REG_BASE      + 0x00000180)
#define HWIO_SE_GENI_RX_FIFOn_RMSK                                           0xffffffff
#define HWIO_SE_GENI_RX_FIFOn_MAXn                                                   15
#define HWIO_SE_GENI_RX_FIFOn_RX_DATA_BMSK                                   0xffffffff
#define HWIO_SE_GENI_RX_FIFOn_RX_DATA_SHFT                                          0x0

#define HWIO_SE_GENI_TX_FIFO_STATUS_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000200)
#define HWIO_SE_GENI_TX_FIFO_STATUS_RMSK                                     0xffffffff
#define HWIO_SE_GENI_TX_FIFO_STATUS_TX_AFIFO_WC_BMSK                         0xf0000000
#define HWIO_SE_GENI_TX_FIFO_STATUS_TX_AFIFO_WC_SHFT                               0x1c
#define HWIO_SE_GENI_TX_FIFO_STATUS_TX_FIFO_WC_BMSK                           0xfffffff
#define HWIO_SE_GENI_TX_FIFO_STATUS_TX_FIFO_WC_SHFT                                 0x0

#define HWIO_SE_GENI_RX_FIFO_STATUS_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000204)
#define HWIO_SE_GENI_RX_FIFO_STATUS_RMSK                                     0xffffffff
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_LAST_BMSK                             0x80000000
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_LAST_SHFT                                   0x1f
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_BMSK                  0x70000000
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_SHFT                        0x1c
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_BMSK                          0xe000000
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_AFIFO_WC_SHFT                               0x19
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_FIFO_WC_BMSK                           0x1ffffff
#define HWIO_SE_GENI_RX_FIFO_STATUS_RX_FIFO_WC_SHFT                                 0x0

#define HWIO_SE_GENI_TX_FIFO_THRESHOLD_OFFS                                  (SE_GENI4_DATA_REG_BASE      + 0x00000208)
#define HWIO_SE_GENI_TX_FIFO_THRESHOLD_RMSK                                        0x1f
#define HWIO_SE_GENI_TX_FIFO_THRESHOLD_TX_FIFO_THRESHOLD_BMSK                      0x1f
#define HWIO_SE_GENI_TX_FIFO_THRESHOLD_TX_FIFO_THRESHOLD_SHFT                       0x0

#define HWIO_SE_GENI_TX_WATERMARK_REG_OFFS                                   (SE_GENI4_DATA_REG_BASE      + 0x0000020c)
#define HWIO_SE_GENI_TX_WATERMARK_REG_RMSK                                         0x3f
#define HWIO_SE_GENI_TX_WATERMARK_REG_TX_WATERMARK_BMSK                            0x3f
#define HWIO_SE_GENI_TX_WATERMARK_REG_TX_WATERMARK_SHFT                             0x0

#define HWIO_SE_GENI_RX_WATERMARK_REG_OFFS                                   (SE_GENI4_DATA_REG_BASE      + 0x00000210)
#define HWIO_SE_GENI_RX_WATERMARK_REG_RMSK                                         0x3f
#define HWIO_SE_GENI_RX_WATERMARK_REG_RX_WATERMARK_BMSK                            0x3f
#define HWIO_SE_GENI_RX_WATERMARK_REG_RX_WATERMARK_SHFT                             0x0

#define HWIO_SE_GENI_RX_RFR_WATERMARK_REG_OFFS                               (SE_GENI4_DATA_REG_BASE      + 0x00000214)
#define HWIO_SE_GENI_RX_RFR_WATERMARK_REG_RMSK                                     0x3f
#define HWIO_SE_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_BMSK                    0x3f
#define HWIO_SE_GENI_RX_RFR_WATERMARK_REG_RX_RFR_WATERMARK_SHFT                     0x0

#define HWIO_SE_GENI_GP_OUTPUT_REG_OFFS                                      (SE_GENI4_DATA_REG_BASE      + 0x00000300)
#define HWIO_SE_GENI_GP_OUTPUT_REG_RMSK                                            0x3f
#define HWIO_SE_GENI_GP_OUTPUT_REG_GP_OUTPUT_BMSK                                  0x3f
#define HWIO_SE_GENI_GP_OUTPUT_REG_GP_OUTPUT_SHFT                                   0x0

#define HWIO_SE_GENI_IOS_OFFS                                                (SE_GENI4_DATA_REG_BASE      + 0x00000308)
#define HWIO_SE_GENI_IOS_RMSK                                                       0x3
#define HWIO_SE_GENI_IOS_IO2_DATA_IN_BMSK                                           0x2
#define HWIO_SE_GENI_IOS_IO2_DATA_IN_SHFT                                           0x1
#define HWIO_SE_GENI_IOS_RX_DATA_IN_BMSK                                            0x1
#define HWIO_SE_GENI_IOS_RX_DATA_IN_SHFT                                            0x0

#define HWIO_SE_GENI_TIMESTAMP_OFFS                                          (SE_GENI4_DATA_REG_BASE      + 0x0000030c)
#define HWIO_SE_GENI_TIMESTAMP_RMSK                                          0xffffffff
#define HWIO_SE_GENI_TIMESTAMP_TIMESTAMP_BMSK                                0xffffffff
#define HWIO_SE_GENI_TIMESTAMP_TIMESTAMP_SHFT                                       0x0

#define HWIO_SE_GENI_M_GP_LENGTH_OFFS                                        (SE_GENI4_DATA_REG_BASE      + 0x00000310)
#define HWIO_SE_GENI_M_GP_LENGTH_RMSK                                        0xffffffff
#define HWIO_SE_GENI_M_GP_LENGTH_M_GP_LENGTH_BMSK                            0xffffffff
#define HWIO_SE_GENI_M_GP_LENGTH_M_GP_LENGTH_SHFT                                   0x0

#define HWIO_SE_GENI_S_GP_LENGTH_OFFS                                        (SE_GENI4_DATA_REG_BASE      + 0x00000314)
#define HWIO_SE_GENI_S_GP_LENGTH_RMSK                                        0xffffffff
#define HWIO_SE_GENI_S_GP_LENGTH_S_GP_LENGTH_BMSK                            0xffffffff
#define HWIO_SE_GENI_S_GP_LENGTH_S_GP_LENGTH_SHFT                                   0x0

#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_OFFS(n)                              (SE_GENI4_DATA_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_RMSK                                  0x1ffffff
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_MAXn                                         15
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_SCL_LOW_CNT_BMSK                      0x1fe0000
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_SCL_LOW_CNT_SHFT                           0x11
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_OWNER_BMSK                              0x1e000
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_OWNER_SHFT                                  0xd
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_NUM_BYTES_BMSK                           0x1c00
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_NUM_BYTES_SHFT                              0xa
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_ACK_NACK_BMSK                             0x200
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_ACK_NACK_SHFT                               0x9
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_RD_WR_BMSK                                0x100
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_RD_WR_SHFT                                  0x8
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_DEVICE_OFFS_BMSK                           0xfe
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_DEVICE_OFFS_SHFT                            0x1
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_VALID_BMSK                                  0x1
#define HWIO_SE_GENI_I3C_IBI_CFG_TABLEn_VALID_SHFT                                  0x0

#define HWIO_SE_GENI_I3C_IBI_STATUS_OFFS                                     (SE_GENI4_DATA_REG_BASE      + 0x00000480)
#define HWIO_SE_GENI_I3C_IBI_STATUS_RMSK                                     0x81ef71ff
#define HWIO_SE_GENI_I3C_IBI_STATUS_FOUND_BMSK                               0x80000000
#define HWIO_SE_GENI_I3C_IBI_STATUS_FOUND_SHFT                                     0x1f
#define HWIO_SE_GENI_I3C_IBI_STATUS_INDEX_BMSK                                0x1e00000
#define HWIO_SE_GENI_I3C_IBI_STATUS_INDEX_SHFT                                     0x15
#define HWIO_SE_GENI_I3C_IBI_STATUS_OWNER_BMSK                                  0xf0000
#define HWIO_SE_GENI_I3C_IBI_STATUS_OWNER_SHFT                                     0x10
#define HWIO_SE_GENI_I3C_IBI_STATUS_NUM_BYTES_BMSK                               0x7000
#define HWIO_SE_GENI_I3C_IBI_STATUS_NUM_BYTES_SHFT                                  0xc
#define HWIO_SE_GENI_I3C_IBI_STATUS_ACK_NACK_BMSK                                 0x100
#define HWIO_SE_GENI_I3C_IBI_STATUS_ACK_NACK_SHFT                                   0x8
#define HWIO_SE_GENI_I3C_IBI_STATUS_DEVICE_OFFS_BMSK                               0xfe
#define HWIO_SE_GENI_I3C_IBI_STATUS_DEVICE_OFFS_SHFT                                0x1
#define HWIO_SE_GENI_I3C_IBI_STATUS_RD_WR_BMSK                                      0x1
#define HWIO_SE_GENI_I3C_IBI_STATUS_RD_WR_SHFT                                      0x0

#define HWIO_SE_GENI_I3C_IBI_RD_DATA_OFFS                                    (SE_GENI4_DATA_REG_BASE      + 0x00000484)
#define HWIO_SE_GENI_I3C_IBI_RD_DATA_RMSK                                    0xffffffff
#define HWIO_SE_GENI_I3C_IBI_RD_DATA_DATA_BMSK                               0xffffffff
#define HWIO_SE_GENI_I3C_IBI_RD_DATA_DATA_SHFT                                      0x0

#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_OFFS                             (SE_GENI4_DATA_REG_BASE      + 0x00000488)
#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_RMSK                                   0xff
#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_DEVICE_OFFS_BMSK                       0xfe
#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_DEVICE_OFFS_SHFT                        0x1
#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_RD_WR_BMSK                              0x1
#define HWIO_SE_GENI_I3C_IBI_SEARCH_PATTERN_RD_WR_SHFT                              0x0

#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_OFFS                                (SE_GENI4_DATA_REG_BASE      + 0x0000048c)
#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_RMSK                                0x8000000f
#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_FOUND_BMSK                          0x80000000
#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_FOUND_SHFT                                0x1f
#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_INDEX_BMSK                                 0xf
#define HWIO_SE_GENI_I3C_IBI_SEARCH_DATA_INDEX_SHFT                                 0x0

#define HWIO_SE_GENI_I3C_SW_IBI_EN_OFFS                                      (SE_GENI4_DATA_REG_BASE      + 0x00000490)
#define HWIO_SE_GENI_I3C_SW_IBI_EN_RMSK                                            0x11
#define HWIO_SE_GENI_I3C_SW_IBI_EN_IBI_EN_STATUS_BMSK                              0x10
#define HWIO_SE_GENI_I3C_SW_IBI_EN_IBI_EN_STATUS_SHFT                               0x4
#define HWIO_SE_GENI_I3C_SW_IBI_EN_IBI_EN_BMSK                                      0x1
#define HWIO_SE_GENI_I3C_SW_IBI_EN_IBI_EN_SHFT                                      0x0

#define HWIO_SE_GENI_I3C_SW_IBI_EN_RECOVER_OFFS                              (SE_GENI4_DATA_REG_BASE      + 0x00000494)
#define HWIO_SE_GENI_I3C_SW_IBI_EN_RECOVER_RMSK                                     0x1
#define HWIO_SE_GENI_I3C_SW_IBI_EN_RECOVER_IBI_EN_RECOVER_SET_BMSK                  0x1
#define HWIO_SE_GENI_I3C_SW_IBI_EN_RECOVER_IBI_EN_RECOVER_SET_SHFT                  0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_GENI4_IMAGE
 *--------------------------------------------------------------------------*/

#define SE_GENI4_IMAGE_REG_BASE                           (SE_GENI4_CFG_REG_BASE      + 0x00001000)

#define HWIO_SE_GENI_FW_REVISION_OFFS                     (SE_GENI4_IMAGE_REG_BASE      + 0x00000000)
#define HWIO_SE_GENI_FW_REVISION_RMSK                         0xffff
#define HWIO_SE_GENI_FW_REVISION_PROTOCOL_BMSK                0xff00
#define HWIO_SE_GENI_FW_REVISION_PROTOCOL_SHFT                   0x8
#define HWIO_SE_GENI_FW_REVISION_VERSION_BMSK                   0xff
#define HWIO_SE_GENI_FW_REVISION_VERSION_SHFT                    0x0

#define HWIO_SE_GENI_S_FW_REVISION_OFFS                   (SE_GENI4_IMAGE_REG_BASE      + 0x00000004)
#define HWIO_SE_GENI_S_FW_REVISION_RMSK                       0xffff
#define HWIO_SE_GENI_S_FW_REVISION_PROTOCOL_BMSK              0xff00
#define HWIO_SE_GENI_S_FW_REVISION_PROTOCOL_SHFT                 0x8
#define HWIO_SE_GENI_S_FW_REVISION_VERSION_BMSK                 0xff
#define HWIO_SE_GENI_S_FW_REVISION_VERSION_SHFT                  0x0

#define HWIO_SE_GENI_CFG_RAMn_OFFS(n)                     (SE_GENI4_IMAGE_REG_BASE      + 0x00000010 + 0x4 * (n))
#define HWIO_SE_GENI_CFG_RAMn_RMSK                          0x7fffff
#define HWIO_SE_GENI_CFG_RAMn_MAXn                               303
#define HWIO_SE_GENI_CFG_RAMn_TBD_BMSK                      0x7fffff
#define HWIO_SE_GENI_CFG_RAMn_TBD_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_GENI4_IMAGE_REGS
 *--------------------------------------------------------------------------*/

#define SE_GENI4_IMAGE_REGS_REG_BASE                                               (SE_GENI4_CFG_REG_BASE      + 0x00000100)

#define HWIO_SE_GENI_CFG_REG0_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000000)
#define HWIO_SE_GENI_CFG_REG0_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG0_PRIM_11_TX_DURATION_BMSK                               0xc00000
#define HWIO_SE_GENI_CFG_REG0_PRIM_11_TX_DURATION_SHFT                                   0x16
#define HWIO_SE_GENI_CFG_REG0_PRIM_10_TX_DURATION_BMSK                               0x300000
#define HWIO_SE_GENI_CFG_REG0_PRIM_10_TX_DURATION_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG0_PRIM_9_TX_DURATION_BMSK                                 0xc0000
#define HWIO_SE_GENI_CFG_REG0_PRIM_9_TX_DURATION_SHFT                                    0x12
#define HWIO_SE_GENI_CFG_REG0_PRIM_8_TX_DURATION_BMSK                                 0x30000
#define HWIO_SE_GENI_CFG_REG0_PRIM_8_TX_DURATION_SHFT                                    0x10
#define HWIO_SE_GENI_CFG_REG0_PRIM_7_TX_DURATION_BMSK                                  0xc000
#define HWIO_SE_GENI_CFG_REG0_PRIM_7_TX_DURATION_SHFT                                     0xe
#define HWIO_SE_GENI_CFG_REG0_PRIM_6_TX_DURATION_BMSK                                  0x3000
#define HWIO_SE_GENI_CFG_REG0_PRIM_6_TX_DURATION_SHFT                                     0xc
#define HWIO_SE_GENI_CFG_REG0_PRIM_5_TX_DURATION_BMSK                                   0xc00
#define HWIO_SE_GENI_CFG_REG0_PRIM_5_TX_DURATION_SHFT                                     0xa
#define HWIO_SE_GENI_CFG_REG0_PRIM_4_TX_DURATION_BMSK                                   0x300
#define HWIO_SE_GENI_CFG_REG0_PRIM_4_TX_DURATION_SHFT                                     0x8
#define HWIO_SE_GENI_CFG_REG0_PRIM_3_TX_DURATION_BMSK                                    0xc0
#define HWIO_SE_GENI_CFG_REG0_PRIM_3_TX_DURATION_SHFT                                     0x6
#define HWIO_SE_GENI_CFG_REG0_PRIM_2_TX_DURATION_BMSK                                    0x30
#define HWIO_SE_GENI_CFG_REG0_PRIM_2_TX_DURATION_SHFT                                     0x4
#define HWIO_SE_GENI_CFG_REG0_PRIM_1_TX_DURATION_BMSK                                     0xc
#define HWIO_SE_GENI_CFG_REG0_PRIM_1_TX_DURATION_SHFT                                     0x2
#define HWIO_SE_GENI_CFG_REG0_PRIM_0_TX_DURATION_BMSK                                     0x3
#define HWIO_SE_GENI_CFG_REG0_PRIM_0_TX_DURATION_SHFT                                     0x0

#define HWIO_SE_GENI_CFG_REG1_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000004)
#define HWIO_SE_GENI_CFG_REG1_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG1_PRIM_23_TX_DURATION_BMSK                               0xc00000
#define HWIO_SE_GENI_CFG_REG1_PRIM_23_TX_DURATION_SHFT                                   0x16
#define HWIO_SE_GENI_CFG_REG1_PRIM_22_TX_DURATION_BMSK                               0x300000
#define HWIO_SE_GENI_CFG_REG1_PRIM_22_TX_DURATION_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG1_PRIM_21_TX_DURATION_BMSK                                0xc0000
#define HWIO_SE_GENI_CFG_REG1_PRIM_21_TX_DURATION_SHFT                                   0x12
#define HWIO_SE_GENI_CFG_REG1_PRIM_20_TX_DURATION_BMSK                                0x30000
#define HWIO_SE_GENI_CFG_REG1_PRIM_20_TX_DURATION_SHFT                                   0x10
#define HWIO_SE_GENI_CFG_REG1_PRIM_19_TX_DURATION_BMSK                                 0xc000
#define HWIO_SE_GENI_CFG_REG1_PRIM_19_TX_DURATION_SHFT                                    0xe
#define HWIO_SE_GENI_CFG_REG1_PRIM_18_TX_DURATION_BMSK                                 0x3000
#define HWIO_SE_GENI_CFG_REG1_PRIM_18_TX_DURATION_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG1_PRIM_17_TX_DURATION_BMSK                                  0xc00
#define HWIO_SE_GENI_CFG_REG1_PRIM_17_TX_DURATION_SHFT                                    0xa
#define HWIO_SE_GENI_CFG_REG1_PRIM_16_TX_DURATION_BMSK                                  0x300
#define HWIO_SE_GENI_CFG_REG1_PRIM_16_TX_DURATION_SHFT                                    0x8
#define HWIO_SE_GENI_CFG_REG1_PRIM_15_TX_DURATION_BMSK                                   0xc0
#define HWIO_SE_GENI_CFG_REG1_PRIM_15_TX_DURATION_SHFT                                    0x6
#define HWIO_SE_GENI_CFG_REG1_PRIM_14_TX_DURATION_BMSK                                   0x30
#define HWIO_SE_GENI_CFG_REG1_PRIM_14_TX_DURATION_SHFT                                    0x4
#define HWIO_SE_GENI_CFG_REG1_PRIM_13_TX_DURATION_BMSK                                    0xc
#define HWIO_SE_GENI_CFG_REG1_PRIM_13_TX_DURATION_SHFT                                    0x2
#define HWIO_SE_GENI_CFG_REG1_PRIM_12_TX_DURATION_BMSK                                    0x3
#define HWIO_SE_GENI_CFG_REG1_PRIM_12_TX_DURATION_SHFT                                    0x0

#define HWIO_SE_GENI_CFG_REG2_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000008)
#define HWIO_SE_GENI_CFG_REG2_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG2_PRIM_11_RX_DURATION_BMSK                               0xc00000
#define HWIO_SE_GENI_CFG_REG2_PRIM_11_RX_DURATION_SHFT                                   0x16
#define HWIO_SE_GENI_CFG_REG2_PRIM_10_RX_DURATION_BMSK                               0x300000
#define HWIO_SE_GENI_CFG_REG2_PRIM_10_RX_DURATION_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG2_PRIM_9_RX_DURATION_BMSK                                 0xc0000
#define HWIO_SE_GENI_CFG_REG2_PRIM_9_RX_DURATION_SHFT                                    0x12
#define HWIO_SE_GENI_CFG_REG2_PRIM_8_RX_DURATION_BMSK                                 0x30000
#define HWIO_SE_GENI_CFG_REG2_PRIM_8_RX_DURATION_SHFT                                    0x10
#define HWIO_SE_GENI_CFG_REG2_PRIM_7_RX_DURATION_BMSK                                  0xc000
#define HWIO_SE_GENI_CFG_REG2_PRIM_7_RX_DURATION_SHFT                                     0xe
#define HWIO_SE_GENI_CFG_REG2_PRIM_6_RX_DURATION_BMSK                                  0x3000
#define HWIO_SE_GENI_CFG_REG2_PRIM_6_RX_DURATION_SHFT                                     0xc
#define HWIO_SE_GENI_CFG_REG2_PRIM_5_RX_DURATION_BMSK                                   0xc00
#define HWIO_SE_GENI_CFG_REG2_PRIM_5_RX_DURATION_SHFT                                     0xa
#define HWIO_SE_GENI_CFG_REG2_PRIM_4_RX_DURATION_BMSK                                   0x300
#define HWIO_SE_GENI_CFG_REG2_PRIM_4_RX_DURATION_SHFT                                     0x8
#define HWIO_SE_GENI_CFG_REG2_PRIM_3_RX_DURATION_BMSK                                    0xc0
#define HWIO_SE_GENI_CFG_REG2_PRIM_3_RX_DURATION_SHFT                                     0x6
#define HWIO_SE_GENI_CFG_REG2_PRIM_2_RX_DURATION_BMSK                                    0x30
#define HWIO_SE_GENI_CFG_REG2_PRIM_2_RX_DURATION_SHFT                                     0x4
#define HWIO_SE_GENI_CFG_REG2_PRIM_1_RX_DURATION_BMSK                                     0xc
#define HWIO_SE_GENI_CFG_REG2_PRIM_1_RX_DURATION_SHFT                                     0x2
#define HWIO_SE_GENI_CFG_REG2_PRIM_0_RX_DURATION_BMSK                                     0x3
#define HWIO_SE_GENI_CFG_REG2_PRIM_0_RX_DURATION_SHFT                                     0x0

#define HWIO_SE_GENI_CFG_REG3_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000000c)
#define HWIO_SE_GENI_CFG_REG3_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG3_PRIM_23_RX_DURATION_BMSK                               0xc00000
#define HWIO_SE_GENI_CFG_REG3_PRIM_23_RX_DURATION_SHFT                                   0x16
#define HWIO_SE_GENI_CFG_REG3_PRIM_22_RX_DURATION_BMSK                               0x300000
#define HWIO_SE_GENI_CFG_REG3_PRIM_22_RX_DURATION_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG3_PRIM_21_RX_DURATION_BMSK                                0xc0000
#define HWIO_SE_GENI_CFG_REG3_PRIM_21_RX_DURATION_SHFT                                   0x12
#define HWIO_SE_GENI_CFG_REG3_PRIM_20_RX_DURATION_BMSK                                0x30000
#define HWIO_SE_GENI_CFG_REG3_PRIM_20_RX_DURATION_SHFT                                   0x10
#define HWIO_SE_GENI_CFG_REG3_PRIM_19_RX_DURATION_BMSK                                 0xc000
#define HWIO_SE_GENI_CFG_REG3_PRIM_19_RX_DURATION_SHFT                                    0xe
#define HWIO_SE_GENI_CFG_REG3_PRIM_18_RX_DURATION_BMSK                                 0x3000
#define HWIO_SE_GENI_CFG_REG3_PRIM_18_RX_DURATION_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG3_PRIM_17_RX_DURATION_BMSK                                  0xc00
#define HWIO_SE_GENI_CFG_REG3_PRIM_17_RX_DURATION_SHFT                                    0xa
#define HWIO_SE_GENI_CFG_REG3_PRIM_16_RX_DURATION_BMSK                                  0x300
#define HWIO_SE_GENI_CFG_REG3_PRIM_16_RX_DURATION_SHFT                                    0x8
#define HWIO_SE_GENI_CFG_REG3_PRIM_15_RX_DURATION_BMSK                                   0xc0
#define HWIO_SE_GENI_CFG_REG3_PRIM_15_RX_DURATION_SHFT                                    0x6
#define HWIO_SE_GENI_CFG_REG3_PRIM_14_RX_DURATION_BMSK                                   0x30
#define HWIO_SE_GENI_CFG_REG3_PRIM_14_RX_DURATION_SHFT                                    0x4
#define HWIO_SE_GENI_CFG_REG3_PRIM_13_RX_DURATION_BMSK                                    0xc
#define HWIO_SE_GENI_CFG_REG3_PRIM_13_RX_DURATION_SHFT                                    0x2
#define HWIO_SE_GENI_CFG_REG3_PRIM_12_RX_DURATION_BMSK                                    0x3
#define HWIO_SE_GENI_CFG_REG3_PRIM_12_RX_DURATION_SHFT                                    0x0

#define HWIO_SE_GENI_CFG_REG4_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000010)
#define HWIO_SE_GENI_CFG_REG4_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG4_PRIM_7_TX_FUNC_SEL_BMSK                                0xe00000
#define HWIO_SE_GENI_CFG_REG4_PRIM_7_TX_FUNC_SEL_SHFT                                    0x15
#define HWIO_SE_GENI_CFG_REG4_PRIM_6_TX_FUNC_SEL_BMSK                                0x1c0000
#define HWIO_SE_GENI_CFG_REG4_PRIM_6_TX_FUNC_SEL_SHFT                                    0x12
#define HWIO_SE_GENI_CFG_REG4_PRIM_5_TX_FUNC_SEL_BMSK                                 0x38000
#define HWIO_SE_GENI_CFG_REG4_PRIM_5_TX_FUNC_SEL_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG4_PRIM_4_TX_FUNC_SEL_BMSK                                  0x7000
#define HWIO_SE_GENI_CFG_REG4_PRIM_4_TX_FUNC_SEL_SHFT                                     0xc
#define HWIO_SE_GENI_CFG_REG4_PRIM_3_TX_FUNC_SEL_BMSK                                   0xe00
#define HWIO_SE_GENI_CFG_REG4_PRIM_3_TX_FUNC_SEL_SHFT                                     0x9
#define HWIO_SE_GENI_CFG_REG4_PRIM_2_TX_FUNC_SEL_BMSK                                   0x1c0
#define HWIO_SE_GENI_CFG_REG4_PRIM_2_TX_FUNC_SEL_SHFT                                     0x6
#define HWIO_SE_GENI_CFG_REG4_PRIM_1_TX_FUNC_SEL_BMSK                                    0x38
#define HWIO_SE_GENI_CFG_REG4_PRIM_1_TX_FUNC_SEL_SHFT                                     0x3
#define HWIO_SE_GENI_CFG_REG4_PRIM_0_TX_FUNC_SEL_BMSK                                     0x7
#define HWIO_SE_GENI_CFG_REG4_PRIM_0_TX_FUNC_SEL_SHFT                                     0x0

#define HWIO_SE_GENI_CFG_REG5_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000014)
#define HWIO_SE_GENI_CFG_REG5_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG5_PRIM_15_TX_FUNC_SEL_BMSK                               0xe00000
#define HWIO_SE_GENI_CFG_REG5_PRIM_15_TX_FUNC_SEL_SHFT                                   0x15
#define HWIO_SE_GENI_CFG_REG5_PRIM_14_TX_FUNC_SEL_BMSK                               0x1c0000
#define HWIO_SE_GENI_CFG_REG5_PRIM_14_TX_FUNC_SEL_SHFT                                   0x12
#define HWIO_SE_GENI_CFG_REG5_PRIM_13_TX_FUNC_SEL_BMSK                                0x38000
#define HWIO_SE_GENI_CFG_REG5_PRIM_13_TX_FUNC_SEL_SHFT                                    0xf
#define HWIO_SE_GENI_CFG_REG5_PRIM_12_TX_FUNC_SEL_BMSK                                 0x7000
#define HWIO_SE_GENI_CFG_REG5_PRIM_12_TX_FUNC_SEL_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG5_PRIM_11_TX_FUNC_SEL_BMSK                                  0xe00
#define HWIO_SE_GENI_CFG_REG5_PRIM_11_TX_FUNC_SEL_SHFT                                    0x9
#define HWIO_SE_GENI_CFG_REG5_PRIM_10_TX_FUNC_SEL_BMSK                                  0x1c0
#define HWIO_SE_GENI_CFG_REG5_PRIM_10_TX_FUNC_SEL_SHFT                                    0x6
#define HWIO_SE_GENI_CFG_REG5_PRIM_9_TX_FUNC_SEL_BMSK                                    0x38
#define HWIO_SE_GENI_CFG_REG5_PRIM_9_TX_FUNC_SEL_SHFT                                     0x3
#define HWIO_SE_GENI_CFG_REG5_PRIM_8_TX_FUNC_SEL_BMSK                                     0x7
#define HWIO_SE_GENI_CFG_REG5_PRIM_8_TX_FUNC_SEL_SHFT                                     0x0

#define HWIO_SE_GENI_CFG_REG6_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000018)
#define HWIO_SE_GENI_CFG_REG6_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG6_PRIM_23_TX_FUNC_SEL_BMSK                               0xe00000
#define HWIO_SE_GENI_CFG_REG6_PRIM_23_TX_FUNC_SEL_SHFT                                   0x15
#define HWIO_SE_GENI_CFG_REG6_PRIM_22_TX_FUNC_SEL_BMSK                               0x1c0000
#define HWIO_SE_GENI_CFG_REG6_PRIM_22_TX_FUNC_SEL_SHFT                                   0x12
#define HWIO_SE_GENI_CFG_REG6_PRIM_21_TX_FUNC_SEL_BMSK                                0x38000
#define HWIO_SE_GENI_CFG_REG6_PRIM_21_TX_FUNC_SEL_SHFT                                    0xf
#define HWIO_SE_GENI_CFG_REG6_PRIM_20_TX_FUNC_SEL_BMSK                                 0x7000
#define HWIO_SE_GENI_CFG_REG6_PRIM_20_TX_FUNC_SEL_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG6_PRIM_19_TX_FUNC_SEL_BMSK                                  0xe00
#define HWIO_SE_GENI_CFG_REG6_PRIM_19_TX_FUNC_SEL_SHFT                                    0x9
#define HWIO_SE_GENI_CFG_REG6_PRIM_18_TX_FUNC_SEL_BMSK                                  0x1c0
#define HWIO_SE_GENI_CFG_REG6_PRIM_18_TX_FUNC_SEL_SHFT                                    0x6
#define HWIO_SE_GENI_CFG_REG6_PRIM_17_TX_FUNC_SEL_BMSK                                   0x38
#define HWIO_SE_GENI_CFG_REG6_PRIM_17_TX_FUNC_SEL_SHFT                                    0x3
#define HWIO_SE_GENI_CFG_REG6_PRIM_16_TX_FUNC_SEL_BMSK                                    0x7
#define HWIO_SE_GENI_CFG_REG6_PRIM_16_TX_FUNC_SEL_SHFT                                    0x0

#define HWIO_SE_GENI_CFG_REG7_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000001c)
#define HWIO_SE_GENI_CFG_REG7_RMSK                                                   0xffffff
#define HWIO_SE_GENI_CFG_REG7_PRIM_23_TX_PAR_FIRST_BMSK                              0x800000
#define HWIO_SE_GENI_CFG_REG7_PRIM_23_TX_PAR_FIRST_SHFT                                  0x17
#define HWIO_SE_GENI_CFG_REG7_PRIM_22_TX_PAR_FIRST_BMSK                              0x400000
#define HWIO_SE_GENI_CFG_REG7_PRIM_22_TX_PAR_FIRST_SHFT                                  0x16
#define HWIO_SE_GENI_CFG_REG7_PRIM_21_TX_PAR_FIRST_BMSK                              0x200000
#define HWIO_SE_GENI_CFG_REG7_PRIM_21_TX_PAR_FIRST_SHFT                                  0x15
#define HWIO_SE_GENI_CFG_REG7_PRIM_20_TX_PAR_FIRST_BMSK                              0x100000
#define HWIO_SE_GENI_CFG_REG7_PRIM_20_TX_PAR_FIRST_SHFT                                  0x14
#define HWIO_SE_GENI_CFG_REG7_PRIM_19_TX_PAR_FIRST_BMSK                               0x80000
#define HWIO_SE_GENI_CFG_REG7_PRIM_19_TX_PAR_FIRST_SHFT                                  0x13
#define HWIO_SE_GENI_CFG_REG7_PRIM_18_TX_PAR_FIRST_BMSK                               0x40000
#define HWIO_SE_GENI_CFG_REG7_PRIM_18_TX_PAR_FIRST_SHFT                                  0x12
#define HWIO_SE_GENI_CFG_REG7_PRIM_17_TX_PAR_FIRST_BMSK                               0x20000
#define HWIO_SE_GENI_CFG_REG7_PRIM_17_TX_PAR_FIRST_SHFT                                  0x11
#define HWIO_SE_GENI_CFG_REG7_PRIM_16_TX_PAR_FIRST_BMSK                               0x10000
#define HWIO_SE_GENI_CFG_REG7_PRIM_16_TX_PAR_FIRST_SHFT                                  0x10
#define HWIO_SE_GENI_CFG_REG7_PRIM_15_TX_PAR_FIRST_BMSK                                0x8000
#define HWIO_SE_GENI_CFG_REG7_PRIM_15_TX_PAR_FIRST_SHFT                                   0xf
#define HWIO_SE_GENI_CFG_REG7_PRIM_14_TX_PAR_FIRST_BMSK                                0x4000
#define HWIO_SE_GENI_CFG_REG7_PRIM_14_TX_PAR_FIRST_SHFT                                   0xe
#define HWIO_SE_GENI_CFG_REG7_PRIM_13_TX_PAR_FIRST_BMSK                                0x2000
#define HWIO_SE_GENI_CFG_REG7_PRIM_13_TX_PAR_FIRST_SHFT                                   0xd
#define HWIO_SE_GENI_CFG_REG7_PRIM_12_TX_PAR_FIRST_BMSK                                0x1000
#define HWIO_SE_GENI_CFG_REG7_PRIM_12_TX_PAR_FIRST_SHFT                                   0xc
#define HWIO_SE_GENI_CFG_REG7_PRIM_11_TX_PAR_FIRST_BMSK                                 0x800
#define HWIO_SE_GENI_CFG_REG7_PRIM_11_TX_PAR_FIRST_SHFT                                   0xb
#define HWIO_SE_GENI_CFG_REG7_PRIM_10_TX_PAR_FIRST_BMSK                                 0x400
#define HWIO_SE_GENI_CFG_REG7_PRIM_10_TX_PAR_FIRST_SHFT                                   0xa
#define HWIO_SE_GENI_CFG_REG7_PRIM_9_TX_PAR_FIRST_BMSK                                  0x200
#define HWIO_SE_GENI_CFG_REG7_PRIM_9_TX_PAR_FIRST_SHFT                                    0x9
#define HWIO_SE_GENI_CFG_REG7_PRIM_8_TX_PAR_FIRST_BMSK                                  0x100
#define HWIO_SE_GENI_CFG_REG7_PRIM_8_TX_PAR_FIRST_SHFT                                    0x8
#define HWIO_SE_GENI_CFG_REG7_PRIM_7_TX_PAR_FIRST_BMSK                                   0x80
#define HWIO_SE_GENI_CFG_REG7_PRIM_7_TX_PAR_FIRST_SHFT                                    0x7
#define HWIO_SE_GENI_CFG_REG7_PRIM_6_TX_PAR_FIRST_BMSK                                   0x40
#define HWIO_SE_GENI_CFG_REG7_PRIM_6_TX_PAR_FIRST_SHFT                                    0x6
#define HWIO_SE_GENI_CFG_REG7_PRIM_5_TX_PAR_FIRST_BMSK                                   0x20
#define HWIO_SE_GENI_CFG_REG7_PRIM_5_TX_PAR_FIRST_SHFT                                    0x5
#define HWIO_SE_GENI_CFG_REG7_PRIM_4_TX_PAR_FIRST_BMSK                                   0x10
#define HWIO_SE_GENI_CFG_REG7_PRIM_4_TX_PAR_FIRST_SHFT                                    0x4
#define HWIO_SE_GENI_CFG_REG7_PRIM_3_TX_PAR_FIRST_BMSK                                    0x8
#define HWIO_SE_GENI_CFG_REG7_PRIM_3_TX_PAR_FIRST_SHFT                                    0x3
#define HWIO_SE_GENI_CFG_REG7_PRIM_2_TX_PAR_FIRST_BMSK                                    0x4
#define HWIO_SE_GENI_CFG_REG7_PRIM_2_TX_PAR_FIRST_SHFT                                    0x2
#define HWIO_SE_GENI_CFG_REG7_PRIM_1_TX_PAR_FIRST_BMSK                                    0x2
#define HWIO_SE_GENI_CFG_REG7_PRIM_1_TX_PAR_FIRST_SHFT                                    0x1
#define HWIO_SE_GENI_CFG_REG7_PRIM_0_TX_PAR_FIRST_BMSK                                    0x1
#define HWIO_SE_GENI_CFG_REG7_PRIM_0_TX_PAR_FIRST_SHFT                                    0x0

#define HWIO_SE_GENI_CFG_REG8_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000020)
#define HWIO_SE_GENI_CFG_REG8_RMSK                                                   0xffffff

#define HWIO_SE_GENI_CFG_REG9_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000024)
#define HWIO_SE_GENI_CFG_REG9_RMSK                                                   0xffffff

#define HWIO_SE_GENI_CFG_REG10_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000028)
#define HWIO_SE_GENI_CFG_REG10_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG10_PRIM_23_RX_SI_EN_BMSK                                 0x800000
#define HWIO_SE_GENI_CFG_REG10_PRIM_23_RX_SI_EN_SHFT                                     0x17
#define HWIO_SE_GENI_CFG_REG10_PRIM_22_RX_SI_EN_BMSK                                 0x400000
#define HWIO_SE_GENI_CFG_REG10_PRIM_22_RX_SI_EN_SHFT                                     0x16
#define HWIO_SE_GENI_CFG_REG10_PRIM_21_RX_SI_EN_BMSK                                 0x200000
#define HWIO_SE_GENI_CFG_REG10_PRIM_21_RX_SI_EN_SHFT                                     0x15
#define HWIO_SE_GENI_CFG_REG10_PRIM_20_RX_SI_EN_BMSK                                 0x100000
#define HWIO_SE_GENI_CFG_REG10_PRIM_20_RX_SI_EN_SHFT                                     0x14
#define HWIO_SE_GENI_CFG_REG10_PRIM_19_RX_SI_EN_BMSK                                  0x80000
#define HWIO_SE_GENI_CFG_REG10_PRIM_19_RX_SI_EN_SHFT                                     0x13
#define HWIO_SE_GENI_CFG_REG10_PRIM_18_RX_SI_EN_BMSK                                  0x40000
#define HWIO_SE_GENI_CFG_REG10_PRIM_18_RX_SI_EN_SHFT                                     0x12
#define HWIO_SE_GENI_CFG_REG10_PRIM_17_RX_SI_EN_BMSK                                  0x20000
#define HWIO_SE_GENI_CFG_REG10_PRIM_17_RX_SI_EN_SHFT                                     0x11
#define HWIO_SE_GENI_CFG_REG10_PRIM_16_RX_SI_EN_BMSK                                  0x10000
#define HWIO_SE_GENI_CFG_REG10_PRIM_16_RX_SI_EN_SHFT                                     0x10
#define HWIO_SE_GENI_CFG_REG10_PRIM_15_RX_SI_EN_BMSK                                   0x8000
#define HWIO_SE_GENI_CFG_REG10_PRIM_15_RX_SI_EN_SHFT                                      0xf
#define HWIO_SE_GENI_CFG_REG10_PRIM_14_RX_SI_EN_BMSK                                   0x4000
#define HWIO_SE_GENI_CFG_REG10_PRIM_14_RX_SI_EN_SHFT                                      0xe
#define HWIO_SE_GENI_CFG_REG10_PRIM_13_RX_SI_EN_BMSK                                   0x2000
#define HWIO_SE_GENI_CFG_REG10_PRIM_13_RX_SI_EN_SHFT                                      0xd
#define HWIO_SE_GENI_CFG_REG10_PRIM_12_RX_SI_EN_BMSK                                   0x1000
#define HWIO_SE_GENI_CFG_REG10_PRIM_12_RX_SI_EN_SHFT                                      0xc
#define HWIO_SE_GENI_CFG_REG10_PRIM_11_RX_SI_EN_BMSK                                    0x800
#define HWIO_SE_GENI_CFG_REG10_PRIM_11_RX_SI_EN_SHFT                                      0xb
#define HWIO_SE_GENI_CFG_REG10_PRIM_10_RX_SI_EN_BMSK                                    0x400
#define HWIO_SE_GENI_CFG_REG10_PRIM_10_RX_SI_EN_SHFT                                      0xa
#define HWIO_SE_GENI_CFG_REG10_PRIM_9_RX_SI_EN_BMSK                                     0x200
#define HWIO_SE_GENI_CFG_REG10_PRIM_9_RX_SI_EN_SHFT                                       0x9
#define HWIO_SE_GENI_CFG_REG10_PRIM_8_RX_SI_EN_BMSK                                     0x100
#define HWIO_SE_GENI_CFG_REG10_PRIM_8_RX_SI_EN_SHFT                                       0x8
#define HWIO_SE_GENI_CFG_REG10_PRIM_7_RX_SI_EN_BMSK                                      0x80
#define HWIO_SE_GENI_CFG_REG10_PRIM_7_RX_SI_EN_SHFT                                       0x7
#define HWIO_SE_GENI_CFG_REG10_PRIM_6_RX_SI_EN_BMSK                                      0x40
#define HWIO_SE_GENI_CFG_REG10_PRIM_6_RX_SI_EN_SHFT                                       0x6
#define HWIO_SE_GENI_CFG_REG10_PRIM_5_RX_SI_EN_BMSK                                      0x20
#define HWIO_SE_GENI_CFG_REG10_PRIM_5_RX_SI_EN_SHFT                                       0x5
#define HWIO_SE_GENI_CFG_REG10_PRIM_4_RX_SI_EN_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG10_PRIM_4_RX_SI_EN_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG10_PRIM_3_RX_SI_EN_BMSK                                       0x8
#define HWIO_SE_GENI_CFG_REG10_PRIM_3_RX_SI_EN_SHFT                                       0x3
#define HWIO_SE_GENI_CFG_REG10_PRIM_2_RX_SI_EN_BMSK                                       0x4
#define HWIO_SE_GENI_CFG_REG10_PRIM_2_RX_SI_EN_SHFT                                       0x2
#define HWIO_SE_GENI_CFG_REG10_PRIM_1_RX_SI_EN_BMSK                                       0x2
#define HWIO_SE_GENI_CFG_REG10_PRIM_1_RX_SI_EN_SHFT                                       0x1
#define HWIO_SE_GENI_CFG_REG10_PRIM_0_RX_SI_EN_BMSK                                       0x1
#define HWIO_SE_GENI_CFG_REG10_PRIM_0_RX_SI_EN_SHFT                                       0x0

#define HWIO_SE_GENI_CFG_REG11_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000002c)
#define HWIO_SE_GENI_CFG_REG11_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG11_PRIM_23_RX_PAR_EN_BMSK                                0x800000
#define HWIO_SE_GENI_CFG_REG11_PRIM_23_RX_PAR_EN_SHFT                                    0x17
#define HWIO_SE_GENI_CFG_REG11_PRIM_22_RX_PAR_EN_BMSK                                0x400000
#define HWIO_SE_GENI_CFG_REG11_PRIM_22_RX_PAR_EN_SHFT                                    0x16
#define HWIO_SE_GENI_CFG_REG11_PRIM_21_RX_PAR_EN_BMSK                                0x200000
#define HWIO_SE_GENI_CFG_REG11_PRIM_21_RX_PAR_EN_SHFT                                    0x15
#define HWIO_SE_GENI_CFG_REG11_PRIM_20_RX_PAR_EN_BMSK                                0x100000
#define HWIO_SE_GENI_CFG_REG11_PRIM_20_RX_PAR_EN_SHFT                                    0x14
#define HWIO_SE_GENI_CFG_REG11_PRIM_19_RX_PAR_EN_BMSK                                 0x80000
#define HWIO_SE_GENI_CFG_REG11_PRIM_19_RX_PAR_EN_SHFT                                    0x13
#define HWIO_SE_GENI_CFG_REG11_PRIM_18_RX_PAR_EN_BMSK                                 0x40000
#define HWIO_SE_GENI_CFG_REG11_PRIM_18_RX_PAR_EN_SHFT                                    0x12
#define HWIO_SE_GENI_CFG_REG11_PRIM_17_RX_PAR_EN_BMSK                                 0x20000
#define HWIO_SE_GENI_CFG_REG11_PRIM_17_RX_PAR_EN_SHFT                                    0x11
#define HWIO_SE_GENI_CFG_REG11_PRIM_16_RX_PAR_EN_BMSK                                 0x10000
#define HWIO_SE_GENI_CFG_REG11_PRIM_16_RX_PAR_EN_SHFT                                    0x10
#define HWIO_SE_GENI_CFG_REG11_PRIM_15_RX_PAR_EN_BMSK                                  0x8000
#define HWIO_SE_GENI_CFG_REG11_PRIM_15_RX_PAR_EN_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG11_PRIM_14_RX_PAR_EN_BMSK                                  0x4000
#define HWIO_SE_GENI_CFG_REG11_PRIM_14_RX_PAR_EN_SHFT                                     0xe
#define HWIO_SE_GENI_CFG_REG11_PRIM_13_RX_PAR_EN_BMSK                                  0x2000
#define HWIO_SE_GENI_CFG_REG11_PRIM_13_RX_PAR_EN_SHFT                                     0xd
#define HWIO_SE_GENI_CFG_REG11_PRIM_12_RX_PAR_EN_BMSK                                  0x1000
#define HWIO_SE_GENI_CFG_REG11_PRIM_12_RX_PAR_EN_SHFT                                     0xc
#define HWIO_SE_GENI_CFG_REG11_PRIM_11_RX_PAR_EN_BMSK                                   0x800
#define HWIO_SE_GENI_CFG_REG11_PRIM_11_RX_PAR_EN_SHFT                                     0xb
#define HWIO_SE_GENI_CFG_REG11_PRIM_10_RX_PAR_EN_BMSK                                   0x400
#define HWIO_SE_GENI_CFG_REG11_PRIM_10_RX_PAR_EN_SHFT                                     0xa
#define HWIO_SE_GENI_CFG_REG11_PRIM_9_RX_PAR_EN_BMSK                                    0x200
#define HWIO_SE_GENI_CFG_REG11_PRIM_9_RX_PAR_EN_SHFT                                      0x9
#define HWIO_SE_GENI_CFG_REG11_PRIM_8_RX_PAR_EN_BMSK                                    0x100
#define HWIO_SE_GENI_CFG_REG11_PRIM_8_RX_PAR_EN_SHFT                                      0x8
#define HWIO_SE_GENI_CFG_REG11_PRIM_7_RX_PAR_EN_BMSK                                     0x80
#define HWIO_SE_GENI_CFG_REG11_PRIM_7_RX_PAR_EN_SHFT                                      0x7
#define HWIO_SE_GENI_CFG_REG11_PRIM_6_RX_PAR_EN_BMSK                                     0x40
#define HWIO_SE_GENI_CFG_REG11_PRIM_6_RX_PAR_EN_SHFT                                      0x6
#define HWIO_SE_GENI_CFG_REG11_PRIM_5_RX_PAR_EN_BMSK                                     0x20
#define HWIO_SE_GENI_CFG_REG11_PRIM_5_RX_PAR_EN_SHFT                                      0x5
#define HWIO_SE_GENI_CFG_REG11_PRIM_4_RX_PAR_EN_BMSK                                     0x10
#define HWIO_SE_GENI_CFG_REG11_PRIM_4_RX_PAR_EN_SHFT                                      0x4
#define HWIO_SE_GENI_CFG_REG11_PRIM_3_RX_PAR_EN_BMSK                                      0x8
#define HWIO_SE_GENI_CFG_REG11_PRIM_3_RX_PAR_EN_SHFT                                      0x3
#define HWIO_SE_GENI_CFG_REG11_PRIM_2_RX_PAR_EN_BMSK                                      0x4
#define HWIO_SE_GENI_CFG_REG11_PRIM_2_RX_PAR_EN_SHFT                                      0x2
#define HWIO_SE_GENI_CFG_REG11_PRIM_1_RX_PAR_EN_BMSK                                      0x2
#define HWIO_SE_GENI_CFG_REG11_PRIM_1_RX_PAR_EN_SHFT                                      0x1
#define HWIO_SE_GENI_CFG_REG11_PRIM_0_RX_PAR_EN_BMSK                                      0x1
#define HWIO_SE_GENI_CFG_REG11_PRIM_0_RX_PAR_EN_SHFT                                      0x0

#define HWIO_SE_GENI_CFG_REG12_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000030)
#define HWIO_SE_GENI_CFG_REG12_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG12_PRIM_23_RX_PAR_FIRST_BMSK                             0x800000
#define HWIO_SE_GENI_CFG_REG12_PRIM_23_RX_PAR_FIRST_SHFT                                 0x17
#define HWIO_SE_GENI_CFG_REG12_PRIM_22_RX_PAR_FIRST_BMSK                             0x400000
#define HWIO_SE_GENI_CFG_REG12_PRIM_22_RX_PAR_FIRST_SHFT                                 0x16
#define HWIO_SE_GENI_CFG_REG12_PRIM_21_RX_PAR_FIRST_BMSK                             0x200000
#define HWIO_SE_GENI_CFG_REG12_PRIM_21_RX_PAR_FIRST_SHFT                                 0x15
#define HWIO_SE_GENI_CFG_REG12_PRIM_20_RX_PAR_FIRST_BMSK                             0x100000
#define HWIO_SE_GENI_CFG_REG12_PRIM_20_RX_PAR_FIRST_SHFT                                 0x14
#define HWIO_SE_GENI_CFG_REG12_PRIM_19_RX_PAR_FIRST_BMSK                              0x80000
#define HWIO_SE_GENI_CFG_REG12_PRIM_19_RX_PAR_FIRST_SHFT                                 0x13
#define HWIO_SE_GENI_CFG_REG12_PRIM_18_RX_PAR_FIRST_BMSK                              0x40000
#define HWIO_SE_GENI_CFG_REG12_PRIM_18_RX_PAR_FIRST_SHFT                                 0x12
#define HWIO_SE_GENI_CFG_REG12_PRIM_17_RX_PAR_FIRST_BMSK                              0x20000
#define HWIO_SE_GENI_CFG_REG12_PRIM_17_RX_PAR_FIRST_SHFT                                 0x11
#define HWIO_SE_GENI_CFG_REG12_PRIM_16_RX_PAR_FIRST_BMSK                              0x10000
#define HWIO_SE_GENI_CFG_REG12_PRIM_16_RX_PAR_FIRST_SHFT                                 0x10
#define HWIO_SE_GENI_CFG_REG12_PRIM_15_RX_PAR_FIRST_BMSK                               0x8000
#define HWIO_SE_GENI_CFG_REG12_PRIM_15_RX_PAR_FIRST_SHFT                                  0xf
#define HWIO_SE_GENI_CFG_REG12_PRIM_14_RX_PAR_FIRST_BMSK                               0x4000
#define HWIO_SE_GENI_CFG_REG12_PRIM_14_RX_PAR_FIRST_SHFT                                  0xe
#define HWIO_SE_GENI_CFG_REG12_PRIM_13_RX_PAR_FIRST_BMSK                               0x2000
#define HWIO_SE_GENI_CFG_REG12_PRIM_13_RX_PAR_FIRST_SHFT                                  0xd
#define HWIO_SE_GENI_CFG_REG12_PRIM_12_RX_PAR_FIRST_BMSK                               0x1000
#define HWIO_SE_GENI_CFG_REG12_PRIM_12_RX_PAR_FIRST_SHFT                                  0xc
#define HWIO_SE_GENI_CFG_REG12_PRIM_11_RX_PAR_FIRST_BMSK                                0x800
#define HWIO_SE_GENI_CFG_REG12_PRIM_11_RX_PAR_FIRST_SHFT                                  0xb
#define HWIO_SE_GENI_CFG_REG12_PRIM_10_RX_PAR_FIRST_BMSK                                0x400
#define HWIO_SE_GENI_CFG_REG12_PRIM_10_RX_PAR_FIRST_SHFT                                  0xa
#define HWIO_SE_GENI_CFG_REG12_PRIM_9_RX_PAR_FIRST_BMSK                                 0x200
#define HWIO_SE_GENI_CFG_REG12_PRIM_9_RX_PAR_FIRST_SHFT                                   0x9
#define HWIO_SE_GENI_CFG_REG12_PRIM_8_RX_PAR_FIRST_BMSK                                 0x100
#define HWIO_SE_GENI_CFG_REG12_PRIM_8_RX_PAR_FIRST_SHFT                                   0x8
#define HWIO_SE_GENI_CFG_REG12_PRIM_7_RX_PAR_FIRST_BMSK                                  0x80
#define HWIO_SE_GENI_CFG_REG12_PRIM_7_RX_PAR_FIRST_SHFT                                   0x7
#define HWIO_SE_GENI_CFG_REG12_PRIM_6_RX_PAR_FIRST_BMSK                                  0x40
#define HWIO_SE_GENI_CFG_REG12_PRIM_6_RX_PAR_FIRST_SHFT                                   0x6
#define HWIO_SE_GENI_CFG_REG12_PRIM_5_RX_PAR_FIRST_BMSK                                  0x20
#define HWIO_SE_GENI_CFG_REG12_PRIM_5_RX_PAR_FIRST_SHFT                                   0x5
#define HWIO_SE_GENI_CFG_REG12_PRIM_4_RX_PAR_FIRST_BMSK                                  0x10
#define HWIO_SE_GENI_CFG_REG12_PRIM_4_RX_PAR_FIRST_SHFT                                   0x4
#define HWIO_SE_GENI_CFG_REG12_PRIM_3_RX_PAR_FIRST_BMSK                                   0x8
#define HWIO_SE_GENI_CFG_REG12_PRIM_3_RX_PAR_FIRST_SHFT                                   0x3
#define HWIO_SE_GENI_CFG_REG12_PRIM_2_RX_PAR_FIRST_BMSK                                   0x4
#define HWIO_SE_GENI_CFG_REG12_PRIM_2_RX_PAR_FIRST_SHFT                                   0x2
#define HWIO_SE_GENI_CFG_REG12_PRIM_1_RX_PAR_FIRST_BMSK                                   0x2
#define HWIO_SE_GENI_CFG_REG12_PRIM_1_RX_PAR_FIRST_SHFT                                   0x1
#define HWIO_SE_GENI_CFG_REG12_PRIM_0_RX_PAR_FIRST_BMSK                                   0x1
#define HWIO_SE_GENI_CFG_REG12_PRIM_0_RX_PAR_FIRST_SHFT                                   0x0

#define HWIO_SE_GENI_CFG_REG13_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000034)
#define HWIO_SE_GENI_CFG_REG13_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG13_PRIM_23_ACC_PAR_EN_BMSK                               0x800000
#define HWIO_SE_GENI_CFG_REG13_PRIM_23_ACC_PAR_EN_SHFT                                   0x17
#define HWIO_SE_GENI_CFG_REG13_PRIM_22_ACC_PAR_EN_BMSK                               0x400000
#define HWIO_SE_GENI_CFG_REG13_PRIM_22_ACC_PAR_EN_SHFT                                   0x16
#define HWIO_SE_GENI_CFG_REG13_PRIM_21_ACC_PAR_EN_BMSK                               0x200000
#define HWIO_SE_GENI_CFG_REG13_PRIM_21_ACC_PAR_EN_SHFT                                   0x15
#define HWIO_SE_GENI_CFG_REG13_PRIM_20_ACC_PAR_EN_BMSK                               0x100000
#define HWIO_SE_GENI_CFG_REG13_PRIM_20_ACC_PAR_EN_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG13_PRIM_19_ACC_PAR_EN_BMSK                                0x80000
#define HWIO_SE_GENI_CFG_REG13_PRIM_19_ACC_PAR_EN_SHFT                                   0x13
#define HWIO_SE_GENI_CFG_REG13_PRIM_18_ACC_PAR_EN_BMSK                                0x40000
#define HWIO_SE_GENI_CFG_REG13_PRIM_18_ACC_PAR_EN_SHFT                                   0x12
#define HWIO_SE_GENI_CFG_REG13_PRIM_17_ACC_PAR_EN_BMSK                                0x20000
#define HWIO_SE_GENI_CFG_REG13_PRIM_17_ACC_PAR_EN_SHFT                                   0x11
#define HWIO_SE_GENI_CFG_REG13_PRIM_16_ACC_PAR_EN_BMSK                                0x10000
#define HWIO_SE_GENI_CFG_REG13_PRIM_16_ACC_PAR_EN_SHFT                                   0x10
#define HWIO_SE_GENI_CFG_REG13_PRIM_15_ACC_PAR_EN_BMSK                                 0x8000
#define HWIO_SE_GENI_CFG_REG13_PRIM_15_ACC_PAR_EN_SHFT                                    0xf
#define HWIO_SE_GENI_CFG_REG13_PRIM_14_ACC_PAR_EN_BMSK                                 0x4000
#define HWIO_SE_GENI_CFG_REG13_PRIM_14_ACC_PAR_EN_SHFT                                    0xe
#define HWIO_SE_GENI_CFG_REG13_PRIM_13_ACC_PAR_EN_BMSK                                 0x2000
#define HWIO_SE_GENI_CFG_REG13_PRIM_13_ACC_PAR_EN_SHFT                                    0xd
#define HWIO_SE_GENI_CFG_REG13_PRIM_12_ACC_PAR_EN_BMSK                                 0x1000
#define HWIO_SE_GENI_CFG_REG13_PRIM_12_ACC_PAR_EN_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG13_PRIM_11_ACC_PAR_EN_BMSK                                  0x800
#define HWIO_SE_GENI_CFG_REG13_PRIM_11_ACC_PAR_EN_SHFT                                    0xb
#define HWIO_SE_GENI_CFG_REG13_PRIM_10_ACC_PAR_EN_BMSK                                  0x400
#define HWIO_SE_GENI_CFG_REG13_PRIM_10_ACC_PAR_EN_SHFT                                    0xa
#define HWIO_SE_GENI_CFG_REG13_PRIM_9_ACC_PAR_EN_BMSK                                   0x200
#define HWIO_SE_GENI_CFG_REG13_PRIM_9_ACC_PAR_EN_SHFT                                     0x9
#define HWIO_SE_GENI_CFG_REG13_PRIM_8_ACC_PAR_EN_BMSK                                   0x100
#define HWIO_SE_GENI_CFG_REG13_PRIM_8_ACC_PAR_EN_SHFT                                     0x8
#define HWIO_SE_GENI_CFG_REG13_PRIM_7_ACC_PAR_EN_BMSK                                    0x80
#define HWIO_SE_GENI_CFG_REG13_PRIM_7_ACC_PAR_EN_SHFT                                     0x7
#define HWIO_SE_GENI_CFG_REG13_PRIM_6_ACC_PAR_EN_BMSK                                    0x40
#define HWIO_SE_GENI_CFG_REG13_PRIM_6_ACC_PAR_EN_SHFT                                     0x6
#define HWIO_SE_GENI_CFG_REG13_PRIM_5_ACC_PAR_EN_BMSK                                    0x20
#define HWIO_SE_GENI_CFG_REG13_PRIM_5_ACC_PAR_EN_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG13_PRIM_4_ACC_PAR_EN_BMSK                                    0x10
#define HWIO_SE_GENI_CFG_REG13_PRIM_4_ACC_PAR_EN_SHFT                                     0x4
#define HWIO_SE_GENI_CFG_REG13_PRIM_3_ACC_PAR_EN_BMSK                                     0x8
#define HWIO_SE_GENI_CFG_REG13_PRIM_3_ACC_PAR_EN_SHFT                                     0x3
#define HWIO_SE_GENI_CFG_REG13_PRIM_2_ACC_PAR_EN_BMSK                                     0x4
#define HWIO_SE_GENI_CFG_REG13_PRIM_2_ACC_PAR_EN_SHFT                                     0x2
#define HWIO_SE_GENI_CFG_REG13_PRIM_1_ACC_PAR_EN_BMSK                                     0x2
#define HWIO_SE_GENI_CFG_REG13_PRIM_1_ACC_PAR_EN_SHFT                                     0x1
#define HWIO_SE_GENI_CFG_REG13_PRIM_0_ACC_PAR_EN_BMSK                                     0x1
#define HWIO_SE_GENI_CFG_REG13_PRIM_0_ACC_PAR_EN_SHFT                                     0x0

#define HWIO_SE_GENI_CFG_REG14_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000038)
#define HWIO_SE_GENI_CFG_REG14_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG14_PRIM_11_IO2_FUNC_SEL_BMSK                             0xc00000
#define HWIO_SE_GENI_CFG_REG14_PRIM_11_IO2_FUNC_SEL_SHFT                                 0x16
#define HWIO_SE_GENI_CFG_REG14_PRIM_10_IO2_FUNC_SEL_BMSK                             0x300000
#define HWIO_SE_GENI_CFG_REG14_PRIM_10_IO2_FUNC_SEL_SHFT                                 0x14
#define HWIO_SE_GENI_CFG_REG14_PRIM_9_IO2_FUNC_SEL_BMSK                               0xc0000
#define HWIO_SE_GENI_CFG_REG14_PRIM_9_IO2_FUNC_SEL_SHFT                                  0x12
#define HWIO_SE_GENI_CFG_REG14_PRIM_8_IO2_FUNC_SEL_BMSK                               0x30000
#define HWIO_SE_GENI_CFG_REG14_PRIM_8_IO2_FUNC_SEL_SHFT                                  0x10
#define HWIO_SE_GENI_CFG_REG14_PRIM_7_IO2_FUNC_SEL_BMSK                                0xc000
#define HWIO_SE_GENI_CFG_REG14_PRIM_7_IO2_FUNC_SEL_SHFT                                   0xe
#define HWIO_SE_GENI_CFG_REG14_PRIM_6_IO2_FUNC_SEL_BMSK                                0x3000
#define HWIO_SE_GENI_CFG_REG14_PRIM_6_IO2_FUNC_SEL_SHFT                                   0xc
#define HWIO_SE_GENI_CFG_REG14_PRIM_5_IO2_FUNC_SEL_BMSK                                 0xc00
#define HWIO_SE_GENI_CFG_REG14_PRIM_5_IO2_FUNC_SEL_SHFT                                   0xa
#define HWIO_SE_GENI_CFG_REG14_PRIM_4_IO2_FUNC_SEL_BMSK                                 0x300
#define HWIO_SE_GENI_CFG_REG14_PRIM_4_IO2_FUNC_SEL_SHFT                                   0x8
#define HWIO_SE_GENI_CFG_REG14_PRIM_3_IO2_FUNC_SEL_BMSK                                  0xc0
#define HWIO_SE_GENI_CFG_REG14_PRIM_3_IO2_FUNC_SEL_SHFT                                   0x6
#define HWIO_SE_GENI_CFG_REG14_PRIM_2_IO2_FUNC_SEL_BMSK                                  0x30
#define HWIO_SE_GENI_CFG_REG14_PRIM_2_IO2_FUNC_SEL_SHFT                                   0x4
#define HWIO_SE_GENI_CFG_REG14_PRIM_1_IO2_FUNC_SEL_BMSK                                   0xc
#define HWIO_SE_GENI_CFG_REG14_PRIM_1_IO2_FUNC_SEL_SHFT                                   0x2
#define HWIO_SE_GENI_CFG_REG14_PRIM_0_IO2_FUNC_SEL_BMSK                                   0x3
#define HWIO_SE_GENI_CFG_REG14_PRIM_0_IO2_FUNC_SEL_SHFT                                   0x0

#define HWIO_SE_GENI_CFG_REG15_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000003c)
#define HWIO_SE_GENI_CFG_REG15_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG15_PRIM_23_IO2_FUNC_SEL_BMSK                             0xc00000
#define HWIO_SE_GENI_CFG_REG15_PRIM_23_IO2_FUNC_SEL_SHFT                                 0x16
#define HWIO_SE_GENI_CFG_REG15_PRIM_22_IO2_FUNC_SEL_BMSK                             0x300000
#define HWIO_SE_GENI_CFG_REG15_PRIM_22_IO2_FUNC_SEL_SHFT                                 0x14
#define HWIO_SE_GENI_CFG_REG15_PRIM_21_IO2_FUNC_SEL_BMSK                              0xc0000
#define HWIO_SE_GENI_CFG_REG15_PRIM_21_IO2_FUNC_SEL_SHFT                                 0x12
#define HWIO_SE_GENI_CFG_REG15_PRIM_20_IO2_FUNC_SEL_BMSK                              0x30000
#define HWIO_SE_GENI_CFG_REG15_PRIM_20_IO2_FUNC_SEL_SHFT                                 0x10
#define HWIO_SE_GENI_CFG_REG15_PRIM_19_IO2_FUNC_SEL_BMSK                               0xc000
#define HWIO_SE_GENI_CFG_REG15_PRIM_19_IO2_FUNC_SEL_SHFT                                  0xe
#define HWIO_SE_GENI_CFG_REG15_PRIM_18_IO2_FUNC_SEL_BMSK                               0x3000
#define HWIO_SE_GENI_CFG_REG15_PRIM_18_IO2_FUNC_SEL_SHFT                                  0xc
#define HWIO_SE_GENI_CFG_REG15_PRIM_17_IO2_FUNC_SEL_BMSK                                0xc00
#define HWIO_SE_GENI_CFG_REG15_PRIM_17_IO2_FUNC_SEL_SHFT                                  0xa
#define HWIO_SE_GENI_CFG_REG15_PRIM_16_IO2_FUNC_SEL_BMSK                                0x300
#define HWIO_SE_GENI_CFG_REG15_PRIM_16_IO2_FUNC_SEL_SHFT                                  0x8
#define HWIO_SE_GENI_CFG_REG15_PRIM_15_IO2_FUNC_SEL_BMSK                                 0xc0
#define HWIO_SE_GENI_CFG_REG15_PRIM_15_IO2_FUNC_SEL_SHFT                                  0x6
#define HWIO_SE_GENI_CFG_REG15_PRIM_14_IO2_FUNC_SEL_BMSK                                 0x30
#define HWIO_SE_GENI_CFG_REG15_PRIM_14_IO2_FUNC_SEL_SHFT                                  0x4
#define HWIO_SE_GENI_CFG_REG15_PRIM_13_IO2_FUNC_SEL_BMSK                                  0xc
#define HWIO_SE_GENI_CFG_REG15_PRIM_13_IO2_FUNC_SEL_SHFT                                  0x2
#define HWIO_SE_GENI_CFG_REG15_PRIM_12_IO2_FUNC_SEL_BMSK                                  0x3
#define HWIO_SE_GENI_CFG_REG15_PRIM_12_IO2_FUNC_SEL_SHFT                                  0x0

#define HWIO_SE_GENI_CFG_REG16_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000040)
#define HWIO_SE_GENI_CFG_REG16_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG16_PRIM_23_TX_TIME_COUNTER_EN_BMSK                       0x800000
#define HWIO_SE_GENI_CFG_REG16_PRIM_23_TX_TIME_COUNTER_EN_SHFT                           0x17
#define HWIO_SE_GENI_CFG_REG16_PRIM_22_TX_TIME_COUNTER_EN_BMSK                       0x400000
#define HWIO_SE_GENI_CFG_REG16_PRIM_22_TX_TIME_COUNTER_EN_SHFT                           0x16
#define HWIO_SE_GENI_CFG_REG16_PRIM_21_TX_TIME_COUNTER_EN_BMSK                       0x200000
#define HWIO_SE_GENI_CFG_REG16_PRIM_21_TX_TIME_COUNTER_EN_SHFT                           0x15
#define HWIO_SE_GENI_CFG_REG16_PRIM_20_TX_TIME_COUNTER_EN_BMSK                       0x100000
#define HWIO_SE_GENI_CFG_REG16_PRIM_20_TX_TIME_COUNTER_EN_SHFT                           0x14
#define HWIO_SE_GENI_CFG_REG16_PRIM_19_TX_TIME_COUNTER_EN_BMSK                        0x80000
#define HWIO_SE_GENI_CFG_REG16_PRIM_19_TX_TIME_COUNTER_EN_SHFT                           0x13
#define HWIO_SE_GENI_CFG_REG16_PRIM_18_TX_TIME_COUNTER_EN_BMSK                        0x40000
#define HWIO_SE_GENI_CFG_REG16_PRIM_18_TX_TIME_COUNTER_EN_SHFT                           0x12
#define HWIO_SE_GENI_CFG_REG16_PRIM_17_TX_TIME_COUNTER_EN_BMSK                        0x20000
#define HWIO_SE_GENI_CFG_REG16_PRIM_17_TX_TIME_COUNTER_EN_SHFT                           0x11
#define HWIO_SE_GENI_CFG_REG16_PRIM_16_TX_TIME_COUNTER_EN_BMSK                        0x10000
#define HWIO_SE_GENI_CFG_REG16_PRIM_16_TX_TIME_COUNTER_EN_SHFT                           0x10
#define HWIO_SE_GENI_CFG_REG16_PRIM_15_TX_TIME_COUNTER_EN_BMSK                         0x8000
#define HWIO_SE_GENI_CFG_REG16_PRIM_15_TX_TIME_COUNTER_EN_SHFT                            0xf
#define HWIO_SE_GENI_CFG_REG16_PRIM_14_TX_TIME_COUNTER_EN_BMSK                         0x4000
#define HWIO_SE_GENI_CFG_REG16_PRIM_14_TX_TIME_COUNTER_EN_SHFT                            0xe
#define HWIO_SE_GENI_CFG_REG16_PRIM_13_TX_TIME_COUNTER_EN_BMSK                         0x2000
#define HWIO_SE_GENI_CFG_REG16_PRIM_13_TX_TIME_COUNTER_EN_SHFT                            0xd
#define HWIO_SE_GENI_CFG_REG16_PRIM_12_TX_TIME_COUNTER_EN_BMSK                         0x1000
#define HWIO_SE_GENI_CFG_REG16_PRIM_12_TX_TIME_COUNTER_EN_SHFT                            0xc
#define HWIO_SE_GENI_CFG_REG16_PRIM_11_TX_TIME_COUNTER_EN_BMSK                          0x800
#define HWIO_SE_GENI_CFG_REG16_PRIM_11_TX_TIME_COUNTER_EN_SHFT                            0xb
#define HWIO_SE_GENI_CFG_REG16_PRIM_10_TX_TIME_COUNTER_EN_BMSK                          0x400
#define HWIO_SE_GENI_CFG_REG16_PRIM_10_TX_TIME_COUNTER_EN_SHFT                            0xa
#define HWIO_SE_GENI_CFG_REG16_PRIM_9_TX_TIME_COUNTER_EN_BMSK                           0x200
#define HWIO_SE_GENI_CFG_REG16_PRIM_9_TX_TIME_COUNTER_EN_SHFT                             0x9
#define HWIO_SE_GENI_CFG_REG16_PRIM_8_TX_TIME_COUNTER_EN_BMSK                           0x100
#define HWIO_SE_GENI_CFG_REG16_PRIM_8_TX_TIME_COUNTER_EN_SHFT                             0x8
#define HWIO_SE_GENI_CFG_REG16_PRIM_7_TX_TIME_COUNTER_EN_BMSK                            0x80
#define HWIO_SE_GENI_CFG_REG16_PRIM_7_TX_TIME_COUNTER_EN_SHFT                             0x7
#define HWIO_SE_GENI_CFG_REG16_PRIM_6_TX_TIME_COUNTER_EN_BMSK                            0x40
#define HWIO_SE_GENI_CFG_REG16_PRIM_6_TX_TIME_COUNTER_EN_SHFT                             0x6
#define HWIO_SE_GENI_CFG_REG16_PRIM_5_TX_TIME_COUNTER_EN_BMSK                            0x20
#define HWIO_SE_GENI_CFG_REG16_PRIM_5_TX_TIME_COUNTER_EN_SHFT                             0x5
#define HWIO_SE_GENI_CFG_REG16_PRIM_4_TX_TIME_COUNTER_EN_BMSK                            0x10
#define HWIO_SE_GENI_CFG_REG16_PRIM_4_TX_TIME_COUNTER_EN_SHFT                             0x4
#define HWIO_SE_GENI_CFG_REG16_PRIM_3_TX_TIME_COUNTER_EN_BMSK                             0x8
#define HWIO_SE_GENI_CFG_REG16_PRIM_3_TX_TIME_COUNTER_EN_SHFT                             0x3
#define HWIO_SE_GENI_CFG_REG16_PRIM_2_TX_TIME_COUNTER_EN_BMSK                             0x4
#define HWIO_SE_GENI_CFG_REG16_PRIM_2_TX_TIME_COUNTER_EN_SHFT                             0x2
#define HWIO_SE_GENI_CFG_REG16_PRIM_1_TX_TIME_COUNTER_EN_BMSK                             0x2
#define HWIO_SE_GENI_CFG_REG16_PRIM_1_TX_TIME_COUNTER_EN_SHFT                             0x1
#define HWIO_SE_GENI_CFG_REG16_PRIM_0_TX_TIME_COUNTER_EN_BMSK                             0x1
#define HWIO_SE_GENI_CFG_REG16_PRIM_0_TX_TIME_COUNTER_EN_SHFT                             0x0

#define HWIO_SE_GENI_CFG_REG17_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000044)
#define HWIO_SE_GENI_CFG_REG17_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG17_PRIM_23_RX_TIME_COUNTER_EN_BMSK                       0x800000
#define HWIO_SE_GENI_CFG_REG17_PRIM_23_RX_TIME_COUNTER_EN_SHFT                           0x17
#define HWIO_SE_GENI_CFG_REG17_PRIM_22_RX_TIME_COUNTER_EN_BMSK                       0x400000
#define HWIO_SE_GENI_CFG_REG17_PRIM_22_RX_TIME_COUNTER_EN_SHFT                           0x16
#define HWIO_SE_GENI_CFG_REG17_PRIM_21_RX_TIME_COUNTER_EN_BMSK                       0x200000
#define HWIO_SE_GENI_CFG_REG17_PRIM_21_RX_TIME_COUNTER_EN_SHFT                           0x15
#define HWIO_SE_GENI_CFG_REG17_PRIM_20_RX_TIME_COUNTER_EN_BMSK                       0x100000
#define HWIO_SE_GENI_CFG_REG17_PRIM_20_RX_TIME_COUNTER_EN_SHFT                           0x14
#define HWIO_SE_GENI_CFG_REG17_PRIM_19_RX_TIME_COUNTER_EN_BMSK                        0x80000
#define HWIO_SE_GENI_CFG_REG17_PRIM_19_RX_TIME_COUNTER_EN_SHFT                           0x13
#define HWIO_SE_GENI_CFG_REG17_PRIM_18_RX_TIME_COUNTER_EN_BMSK                        0x40000
#define HWIO_SE_GENI_CFG_REG17_PRIM_18_RX_TIME_COUNTER_EN_SHFT                           0x12
#define HWIO_SE_GENI_CFG_REG17_PRIM_17_RX_TIME_COUNTER_EN_BMSK                        0x20000
#define HWIO_SE_GENI_CFG_REG17_PRIM_17_RX_TIME_COUNTER_EN_SHFT                           0x11
#define HWIO_SE_GENI_CFG_REG17_PRIM_16_RX_TIME_COUNTER_EN_BMSK                        0x10000
#define HWIO_SE_GENI_CFG_REG17_PRIM_16_RX_TIME_COUNTER_EN_SHFT                           0x10
#define HWIO_SE_GENI_CFG_REG17_PRIM_15_RX_TIME_COUNTER_EN_BMSK                         0x8000
#define HWIO_SE_GENI_CFG_REG17_PRIM_15_RX_TIME_COUNTER_EN_SHFT                            0xf
#define HWIO_SE_GENI_CFG_REG17_PRIM_14_RX_TIME_COUNTER_EN_BMSK                         0x4000
#define HWIO_SE_GENI_CFG_REG17_PRIM_14_RX_TIME_COUNTER_EN_SHFT                            0xe
#define HWIO_SE_GENI_CFG_REG17_PRIM_13_RX_TIME_COUNTER_EN_BMSK                         0x2000
#define HWIO_SE_GENI_CFG_REG17_PRIM_13_RX_TIME_COUNTER_EN_SHFT                            0xd
#define HWIO_SE_GENI_CFG_REG17_PRIM_12_RX_TIME_COUNTER_EN_BMSK                         0x1000
#define HWIO_SE_GENI_CFG_REG17_PRIM_12_RX_TIME_COUNTER_EN_SHFT                            0xc
#define HWIO_SE_GENI_CFG_REG17_PRIM_11_RX_TIME_COUNTER_EN_BMSK                          0x800
#define HWIO_SE_GENI_CFG_REG17_PRIM_11_RX_TIME_COUNTER_EN_SHFT                            0xb
#define HWIO_SE_GENI_CFG_REG17_PRIM_10_RX_TIME_COUNTER_EN_BMSK                          0x400
#define HWIO_SE_GENI_CFG_REG17_PRIM_10_RX_TIME_COUNTER_EN_SHFT                            0xa
#define HWIO_SE_GENI_CFG_REG17_PRIM_9_RX_TIME_COUNTER_EN_BMSK                           0x200
#define HWIO_SE_GENI_CFG_REG17_PRIM_9_RX_TIME_COUNTER_EN_SHFT                             0x9
#define HWIO_SE_GENI_CFG_REG17_PRIM_8_RX_TIME_COUNTER_EN_BMSK                           0x100
#define HWIO_SE_GENI_CFG_REG17_PRIM_8_RX_TIME_COUNTER_EN_SHFT                             0x8
#define HWIO_SE_GENI_CFG_REG17_PRIM_7_RX_TIME_COUNTER_EN_BMSK                            0x80
#define HWIO_SE_GENI_CFG_REG17_PRIM_7_RX_TIME_COUNTER_EN_SHFT                             0x7
#define HWIO_SE_GENI_CFG_REG17_PRIM_6_RX_TIME_COUNTER_EN_BMSK                            0x40
#define HWIO_SE_GENI_CFG_REG17_PRIM_6_RX_TIME_COUNTER_EN_SHFT                             0x6
#define HWIO_SE_GENI_CFG_REG17_PRIM_5_RX_TIME_COUNTER_EN_BMSK                            0x20
#define HWIO_SE_GENI_CFG_REG17_PRIM_5_RX_TIME_COUNTER_EN_SHFT                             0x5
#define HWIO_SE_GENI_CFG_REG17_PRIM_4_RX_TIME_COUNTER_EN_BMSK                            0x10
#define HWIO_SE_GENI_CFG_REG17_PRIM_4_RX_TIME_COUNTER_EN_SHFT                             0x4
#define HWIO_SE_GENI_CFG_REG17_PRIM_3_RX_TIME_COUNTER_EN_BMSK                             0x8
#define HWIO_SE_GENI_CFG_REG17_PRIM_3_RX_TIME_COUNTER_EN_SHFT                             0x3
#define HWIO_SE_GENI_CFG_REG17_PRIM_2_RX_TIME_COUNTER_EN_BMSK                             0x4
#define HWIO_SE_GENI_CFG_REG17_PRIM_2_RX_TIME_COUNTER_EN_SHFT                             0x2
#define HWIO_SE_GENI_CFG_REG17_PRIM_1_RX_TIME_COUNTER_EN_BMSK                             0x2
#define HWIO_SE_GENI_CFG_REG17_PRIM_1_RX_TIME_COUNTER_EN_SHFT                             0x1
#define HWIO_SE_GENI_CFG_REG17_PRIM_0_RX_TIME_COUNTER_EN_BMSK                             0x1
#define HWIO_SE_GENI_CFG_REG17_PRIM_0_RX_TIME_COUNTER_EN_SHFT                             0x0

#define HWIO_SE_GENI_CFG_REG18_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000048)
#define HWIO_SE_GENI_CFG_REG18_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG18_PRIM_11_IO3_FUNC_SEL_BMSK                             0xc00000
#define HWIO_SE_GENI_CFG_REG18_PRIM_11_IO3_FUNC_SEL_SHFT                                 0x16
#define HWIO_SE_GENI_CFG_REG18_PRIM_10_IO3_FUNC_SEL_BMSK                             0x300000
#define HWIO_SE_GENI_CFG_REG18_PRIM_10_IO3_FUNC_SEL_SHFT                                 0x14
#define HWIO_SE_GENI_CFG_REG18_PRIM_9_IO3_FUNC_SEL_BMSK                               0xc0000
#define HWIO_SE_GENI_CFG_REG18_PRIM_9_IO3_FUNC_SEL_SHFT                                  0x12
#define HWIO_SE_GENI_CFG_REG18_PRIM_8_IO3_FUNC_SEL_BMSK                               0x30000
#define HWIO_SE_GENI_CFG_REG18_PRIM_8_IO3_FUNC_SEL_SHFT                                  0x10
#define HWIO_SE_GENI_CFG_REG18_PRIM_7_IO3_FUNC_SEL_BMSK                                0xc000
#define HWIO_SE_GENI_CFG_REG18_PRIM_7_IO3_FUNC_SEL_SHFT                                   0xe
#define HWIO_SE_GENI_CFG_REG18_PRIM_6_IO3_FUNC_SEL_BMSK                                0x3000
#define HWIO_SE_GENI_CFG_REG18_PRIM_6_IO3_FUNC_SEL_SHFT                                   0xc
#define HWIO_SE_GENI_CFG_REG18_PRIM_5_IO3_FUNC_SEL_BMSK                                 0xc00
#define HWIO_SE_GENI_CFG_REG18_PRIM_5_IO3_FUNC_SEL_SHFT                                   0xa
#define HWIO_SE_GENI_CFG_REG18_PRIM_4_IO3_FUNC_SEL_BMSK                                 0x300
#define HWIO_SE_GENI_CFG_REG18_PRIM_4_IO3_FUNC_SEL_SHFT                                   0x8
#define HWIO_SE_GENI_CFG_REG18_PRIM_3_IO3_FUNC_SEL_BMSK                                  0xc0
#define HWIO_SE_GENI_CFG_REG18_PRIM_3_IO3_FUNC_SEL_SHFT                                   0x6
#define HWIO_SE_GENI_CFG_REG18_PRIM_2_IO3_FUNC_SEL_BMSK                                  0x30
#define HWIO_SE_GENI_CFG_REG18_PRIM_2_IO3_FUNC_SEL_SHFT                                   0x4
#define HWIO_SE_GENI_CFG_REG18_PRIM_1_IO3_FUNC_SEL_BMSK                                   0xc
#define HWIO_SE_GENI_CFG_REG18_PRIM_1_IO3_FUNC_SEL_SHFT                                   0x2
#define HWIO_SE_GENI_CFG_REG18_PRIM_0_IO3_FUNC_SEL_BMSK                                   0x3
#define HWIO_SE_GENI_CFG_REG18_PRIM_0_IO3_FUNC_SEL_SHFT                                   0x0

#define HWIO_SE_GENI_CFG_REG19_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000004c)
#define HWIO_SE_GENI_CFG_REG19_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG19_PRIM_23_IO3_FUNC_SEL_BMSK                             0xc00000
#define HWIO_SE_GENI_CFG_REG19_PRIM_23_IO3_FUNC_SEL_SHFT                                 0x16
#define HWIO_SE_GENI_CFG_REG19_PRIM_22_IO3_FUNC_SEL_BMSK                             0x300000
#define HWIO_SE_GENI_CFG_REG19_PRIM_22_IO3_FUNC_SEL_SHFT                                 0x14
#define HWIO_SE_GENI_CFG_REG19_PRIM_21_IO3_FUNC_SEL_BMSK                              0xc0000
#define HWIO_SE_GENI_CFG_REG19_PRIM_21_IO3_FUNC_SEL_SHFT                                 0x12
#define HWIO_SE_GENI_CFG_REG19_PRIM_20_IO3_FUNC_SEL_BMSK                              0x30000
#define HWIO_SE_GENI_CFG_REG19_PRIM_20_IO3_FUNC_SEL_SHFT                                 0x10
#define HWIO_SE_GENI_CFG_REG19_PRIM_19_IO3_FUNC_SEL_BMSK                               0xc000
#define HWIO_SE_GENI_CFG_REG19_PRIM_19_IO3_FUNC_SEL_SHFT                                  0xe
#define HWIO_SE_GENI_CFG_REG19_PRIM_18_IO3_FUNC_SEL_BMSK                               0x3000
#define HWIO_SE_GENI_CFG_REG19_PRIM_18_IO3_FUNC_SEL_SHFT                                  0xc
#define HWIO_SE_GENI_CFG_REG19_PRIM_17_IO3_FUNC_SEL_BMSK                                0xc00
#define HWIO_SE_GENI_CFG_REG19_PRIM_17_IO3_FUNC_SEL_SHFT                                  0xa
#define HWIO_SE_GENI_CFG_REG19_PRIM_16_IO3_FUNC_SEL_BMSK                                0x300
#define HWIO_SE_GENI_CFG_REG19_PRIM_16_IO3_FUNC_SEL_SHFT                                  0x8
#define HWIO_SE_GENI_CFG_REG19_PRIM_15_IO3_FUNC_SEL_BMSK                                 0xc0
#define HWIO_SE_GENI_CFG_REG19_PRIM_15_IO3_FUNC_SEL_SHFT                                  0x6
#define HWIO_SE_GENI_CFG_REG19_PRIM_14_IO3_FUNC_SEL_BMSK                                 0x30
#define HWIO_SE_GENI_CFG_REG19_PRIM_14_IO3_FUNC_SEL_SHFT                                  0x4
#define HWIO_SE_GENI_CFG_REG19_PRIM_13_IO3_FUNC_SEL_BMSK                                  0xc
#define HWIO_SE_GENI_CFG_REG19_PRIM_13_IO3_FUNC_SEL_SHFT                                  0x2
#define HWIO_SE_GENI_CFG_REG19_PRIM_12_IO3_FUNC_SEL_BMSK                                  0x3
#define HWIO_SE_GENI_CFG_REG19_PRIM_12_IO3_FUNC_SEL_SHFT                                  0x0

#define HWIO_SE_GENI_CFG_REG20_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000050)
#define HWIO_SE_GENI_CFG_REG20_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG20_PRIM_23_RX_CHAR_HUNT_EN_BMSK                          0x800000
#define HWIO_SE_GENI_CFG_REG20_PRIM_23_RX_CHAR_HUNT_EN_SHFT                              0x17
#define HWIO_SE_GENI_CFG_REG20_PRIM_22_RX_CHAR_HUNT_EN_BMSK                          0x400000
#define HWIO_SE_GENI_CFG_REG20_PRIM_22_RX_CHAR_HUNT_EN_SHFT                              0x16
#define HWIO_SE_GENI_CFG_REG20_PRIM_21_RX_CHAR_HUNT_EN_BMSK                          0x200000
#define HWIO_SE_GENI_CFG_REG20_PRIM_21_RX_CHAR_HUNT_EN_SHFT                              0x15
#define HWIO_SE_GENI_CFG_REG20_PRIM_20_RX_CHAR_HUNT_EN_BMSK                          0x100000
#define HWIO_SE_GENI_CFG_REG20_PRIM_20_RX_CHAR_HUNT_EN_SHFT                              0x14
#define HWIO_SE_GENI_CFG_REG20_PRIM_19_RX_CHAR_HUNT_EN_BMSK                           0x80000
#define HWIO_SE_GENI_CFG_REG20_PRIM_19_RX_CHAR_HUNT_EN_SHFT                              0x13
#define HWIO_SE_GENI_CFG_REG20_PRIM_18_RX_CHAR_HUNT_EN_BMSK                           0x40000
#define HWIO_SE_GENI_CFG_REG20_PRIM_18_RX_CHAR_HUNT_EN_SHFT                              0x12
#define HWIO_SE_GENI_CFG_REG20_PRIM_17_RX_CHAR_HUNT_EN_BMSK                           0x20000
#define HWIO_SE_GENI_CFG_REG20_PRIM_17_RX_CHAR_HUNT_EN_SHFT                              0x11
#define HWIO_SE_GENI_CFG_REG20_PRIM_16_RX_CHAR_HUNT_EN_BMSK                           0x10000
#define HWIO_SE_GENI_CFG_REG20_PRIM_16_RX_CHAR_HUNT_EN_SHFT                              0x10
#define HWIO_SE_GENI_CFG_REG20_PRIM_15_RX_CHAR_HUNT_EN_BMSK                            0x8000
#define HWIO_SE_GENI_CFG_REG20_PRIM_15_RX_CHAR_HUNT_EN_SHFT                               0xf
#define HWIO_SE_GENI_CFG_REG20_PRIM_14_RX_CHAR_HUNT_EN_BMSK                            0x4000
#define HWIO_SE_GENI_CFG_REG20_PRIM_14_RX_CHAR_HUNT_EN_SHFT                               0xe
#define HWIO_SE_GENI_CFG_REG20_PRIM_13_RX_CHAR_HUNT_EN_BMSK                            0x2000
#define HWIO_SE_GENI_CFG_REG20_PRIM_13_RX_CHAR_HUNT_EN_SHFT                               0xd
#define HWIO_SE_GENI_CFG_REG20_PRIM_12_RX_CHAR_HUNT_EN_BMSK                            0x1000
#define HWIO_SE_GENI_CFG_REG20_PRIM_12_RX_CHAR_HUNT_EN_SHFT                               0xc
#define HWIO_SE_GENI_CFG_REG20_PRIM_11_RX_CHAR_HUNT_EN_BMSK                             0x800
#define HWIO_SE_GENI_CFG_REG20_PRIM_11_RX_CHAR_HUNT_EN_SHFT                               0xb
#define HWIO_SE_GENI_CFG_REG20_PRIM_10_RX_CHAR_HUNT_EN_BMSK                             0x400
#define HWIO_SE_GENI_CFG_REG20_PRIM_10_RX_CHAR_HUNT_EN_SHFT                               0xa
#define HWIO_SE_GENI_CFG_REG20_PRIM_9_RX_CHAR_HUNT_EN_BMSK                              0x200
#define HWIO_SE_GENI_CFG_REG20_PRIM_9_RX_CHAR_HUNT_EN_SHFT                                0x9
#define HWIO_SE_GENI_CFG_REG20_PRIM_8_RX_CHAR_HUNT_EN_BMSK                              0x100
#define HWIO_SE_GENI_CFG_REG20_PRIM_8_RX_CHAR_HUNT_EN_SHFT                                0x8
#define HWIO_SE_GENI_CFG_REG20_PRIM_7_RX_CHAR_HUNT_EN_BMSK                               0x80
#define HWIO_SE_GENI_CFG_REG20_PRIM_7_RX_CHAR_HUNT_EN_SHFT                                0x7
#define HWIO_SE_GENI_CFG_REG20_PRIM_6_RX_CHAR_HUNT_EN_BMSK                               0x40
#define HWIO_SE_GENI_CFG_REG20_PRIM_6_RX_CHAR_HUNT_EN_SHFT                                0x6
#define HWIO_SE_GENI_CFG_REG20_PRIM_5_RX_CHAR_HUNT_EN_BMSK                               0x20
#define HWIO_SE_GENI_CFG_REG20_PRIM_5_RX_CHAR_HUNT_EN_SHFT                                0x5
#define HWIO_SE_GENI_CFG_REG20_PRIM_4_RX_CHAR_HUNT_EN_BMSK                               0x10
#define HWIO_SE_GENI_CFG_REG20_PRIM_4_RX_CHAR_HUNT_EN_SHFT                                0x4
#define HWIO_SE_GENI_CFG_REG20_PRIM_3_RX_CHAR_HUNT_EN_BMSK                                0x8
#define HWIO_SE_GENI_CFG_REG20_PRIM_3_RX_CHAR_HUNT_EN_SHFT                                0x3
#define HWIO_SE_GENI_CFG_REG20_PRIM_2_RX_CHAR_HUNT_EN_BMSK                                0x4
#define HWIO_SE_GENI_CFG_REG20_PRIM_2_RX_CHAR_HUNT_EN_SHFT                                0x2
#define HWIO_SE_GENI_CFG_REG20_PRIM_1_RX_CHAR_HUNT_EN_BMSK                                0x2
#define HWIO_SE_GENI_CFG_REG20_PRIM_1_RX_CHAR_HUNT_EN_SHFT                                0x1
#define HWIO_SE_GENI_CFG_REG20_PRIM_0_RX_CHAR_HUNT_EN_BMSK                                0x1
#define HWIO_SE_GENI_CFG_REG20_PRIM_0_RX_CHAR_HUNT_EN_SHFT                                0x0

#define HWIO_SE_GENI_CFG_REG64_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000100)
#define HWIO_SE_GENI_CFG_REG64_RMSK                                                   0x30a03
#define HWIO_SE_GENI_CFG_REG64_M_IO_DATA_IRQ_FW_EN_BMSK                               0x20000
#define HWIO_SE_GENI_CFG_REG64_M_IO_DATA_IRQ_FW_EN_SHFT                                  0x11
#define HWIO_SE_GENI_CFG_REG64_S_IO_DATA_IRQ_FW_EN_BMSK                               0x10000
#define HWIO_SE_GENI_CFG_REG64_S_IO_DATA_IRQ_FW_EN_SHFT                                  0x10
#define HWIO_SE_GENI_CFG_REG64_GENI_TX_MODE_BMSK                                        0x800
#define HWIO_SE_GENI_CFG_REG64_GENI_TX_MODE_SHFT                                          0xb
#define HWIO_SE_GENI_CFG_REG64_DATA_CNT_MODE_BMSK                                       0x200
#define HWIO_SE_GENI_CFG_REG64_DATA_CNT_MODE_SHFT                                         0x9
#define HWIO_SE_GENI_CFG_REG64_EXT_ARB_EN_BMSK                                            0x2
#define HWIO_SE_GENI_CFG_REG64_EXT_ARB_EN_SHFT                                            0x1
#define HWIO_SE_GENI_CFG_REG64_M_PROG_RAM_SEC_WORD_EN_BMSK                                0x1
#define HWIO_SE_GENI_CFG_REG64_M_PROG_RAM_SEC_WORD_EN_SHFT                                0x0

#define HWIO_SE_GENI_CFG_REG65_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000104)
#define HWIO_SE_GENI_CFG_REG65_RMSK                                                   0x3ffff
#define HWIO_SE_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_1_BMSK                              0x3fe00
#define HWIO_SE_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_1_SHFT                                  0x9
#define HWIO_SE_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_0_BMSK                                0x1ff
#define HWIO_SE_GENI_CFG_REG65_TX_PARAM_TABLE_VEC_0_SHFT                                  0x0

#define HWIO_SE_GENI_CFG_REG66_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000108)
#define HWIO_SE_GENI_CFG_REG66_RMSK                                                   0x3ffff
#define HWIO_SE_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_3_BMSK                              0x3fe00
#define HWIO_SE_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_3_SHFT                                  0x9
#define HWIO_SE_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_2_BMSK                                0x1ff
#define HWIO_SE_GENI_CFG_REG66_TX_PARAM_TABLE_VEC_2_SHFT                                  0x0

#define HWIO_SE_GENI_CFG_REG67_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000010c)
#define HWIO_SE_GENI_CFG_REG67_RMSK                                                 0xfdffffc
#define HWIO_SE_GENI_CFG_REG67_M_AUTO_OPCODE_CANCEL_BMSK                            0x8000000
#define HWIO_SE_GENI_CFG_REG67_M_AUTO_OPCODE_CANCEL_SHFT                                 0x1b
#define HWIO_SE_GENI_CFG_REG67_M_OPCODE_CMD_CANCEL_BMSK                             0x7c00000
#define HWIO_SE_GENI_CFG_REG67_M_OPCODE_CMD_CANCEL_SHFT                                  0x16
#define HWIO_SE_GENI_CFG_REG67_DRIVE_DEFAULT_ON_START_EN_BMSK                        0x100000
#define HWIO_SE_GENI_CFG_REG67_DRIVE_DEFAULT_ON_START_EN_SHFT                            0x14
#define HWIO_SE_GENI_CFG_REG67_TX_BP_MODE_BMSK                                        0x80000
#define HWIO_SE_GENI_CFG_REG67_TX_BP_MODE_SHFT                                           0x13
#define HWIO_SE_GENI_CFG_REG67_TX_SOE_EDGE_SEL_BMSK                                   0x40000
#define HWIO_SE_GENI_CFG_REG67_TX_SOE_EDGE_SEL_SHFT                                      0x12
#define HWIO_SE_GENI_CFG_REG67_TX_SOUT_EDGE_SEL_BMSK                                  0x20000
#define HWIO_SE_GENI_CFG_REG67_TX_SOUT_EDGE_SEL_SHFT                                     0x11
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOE_VALUE_BMSK                         0x10000
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOE_VALUE_SHFT                            0x10
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOUT_VALUE_BMSK                         0x8000
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_PRIM_SOUT_VALUE_SHFT                            0xf
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_SOE_VALUE_BMSK                               0x4000
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_SOE_VALUE_SHFT                                  0xe
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_SOUT_VALUE_BMSK                              0x2000
#define HWIO_SE_GENI_CFG_REG67_TX_DEFAULT_SOUT_VALUE_SHFT                                 0xd
#define HWIO_SE_GENI_CFG_REG67_TX_CONST1_EFF_SIZE_BMSK                                 0x1c00
#define HWIO_SE_GENI_CFG_REG67_TX_CONST1_EFF_SIZE_SHFT                                    0xa
#define HWIO_SE_GENI_CFG_REG67_TX_CONST1_REG_BMSK                                       0x3fc
#define HWIO_SE_GENI_CFG_REG67_TX_CONST1_REG_SHFT                                         0x2

#define HWIO_SE_GENI_CFG_REG68_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000110)
#define HWIO_SE_GENI_CFG_REG68_RMSK                                                0xe03bfd3f
#define HWIO_SE_GENI_CFG_REG68_M_TIME_CNT_FR_DIV_BMSK                              0xc0000000
#define HWIO_SE_GENI_CFG_REG68_M_TIME_CNT_FR_DIV_SHFT                                    0x1e
#define HWIO_SE_GENI_CFG_REG68_M_TIME_CNT_EN_FR_SEL_BMSK                           0x20000000
#define HWIO_SE_GENI_CFG_REG68_M_TIME_CNT_EN_FR_SEL_SHFT                                 0x1d
#define HWIO_SE_GENI_CFG_REG68_M_GP_SYNC_CANCEL_CLEAR_BMSK                           0x200000
#define HWIO_SE_GENI_CFG_REG68_M_GP_SYNC_CANCEL_CLEAR_SHFT                               0x15
#define HWIO_SE_GENI_CFG_REG68_M_GP_CNT_DIRECTION_BMSK                               0x100000
#define HWIO_SE_GENI_CFG_REG68_M_GP_CNT_DIRECTION_SHFT                                   0x14
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_START_VAL_BMSK                           0x80000
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_START_VAL_SHFT                              0x13
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_STOP_DUR_BMSK                            0x30000
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_STOP_DUR_SHFT                               0x10
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_EN_BMSK                                   0x8000
#define HWIO_SE_GENI_CFG_REG68_TX_SOUT_MODUL_EN_SHFT                                      0xf
#define HWIO_SE_GENI_CFG_REG68_TX_LOAD_PAR_EN_BMSK                                     0x4000
#define HWIO_SE_GENI_CFG_REG68_TX_LOAD_PAR_EN_SHFT                                        0xe
#define HWIO_SE_GENI_CFG_REG68_TX_PARAM_PAR_EN_BMSK                                    0x1000
#define HWIO_SE_GENI_CFG_REG68_TX_PARAM_PAR_EN_SHFT                                       0xc
#define HWIO_SE_GENI_CFG_REG68_TX_FIFO_PAR_EN_BMSK                                      0x400
#define HWIO_SE_GENI_CFG_REG68_TX_FIFO_PAR_EN_SHFT                                        0xa
#define HWIO_SE_GENI_CFG_REG68_IO2_CONST_EFF_SIZE_BMSK                                  0x100
#define HWIO_SE_GENI_CFG_REG68_IO2_CONST_EFF_SIZE_SHFT                                    0x8
#define HWIO_SE_GENI_CFG_REG68_IO2_CONST_REG_BMSK                                        0x30
#define HWIO_SE_GENI_CFG_REG68_IO2_CONST_REG_SHFT                                         0x4
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOE_VALUE_BMSK                            0x8
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOE_VALUE_SHFT                            0x3
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOUT_VALUE_BMSK                           0x4
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_PRIM_SOUT_VALUE_SHFT                           0x2
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_SOE_VALUE_BMSK                                 0x2
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_SOE_VALUE_SHFT                                 0x1
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_SOUT_VALUE_BMSK                                0x1
#define HWIO_SE_GENI_CFG_REG68_IO2_DEFAULT_SOUT_VALUE_SHFT                                0x0

#define HWIO_SE_GENI_CFG_REG69_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000114)
#define HWIO_SE_GENI_CFG_REG69_RMSK                                                0x4ec03ff1
#define HWIO_SE_GENI_CFG_REG69_GENI_RX_MODE_BMSK                                   0x40000000
#define HWIO_SE_GENI_CFG_REG69_GENI_RX_MODE_SHFT                                         0x1e
#define HWIO_SE_GENI_CFG_REG69_RX_IO_SIN_SEL_BMSK                                   0xe000000
#define HWIO_SE_GENI_CFG_REG69_RX_IO_SIN_SEL_SHFT                                        0x19
#define HWIO_SE_GENI_CFG_REG69_RX_SIN_SEL_BMSK                                       0xc00000
#define HWIO_SE_GENI_CFG_REG69_RX_SIN_SEL_SHFT                                           0x16
#define HWIO_SE_GENI_CFG_REG69_S_GP_SYNC_CANCEL_CLEAR_BMSK                             0x2000
#define HWIO_SE_GENI_CFG_REG69_S_GP_SYNC_CANCEL_CLEAR_SHFT                                0xd
#define HWIO_SE_GENI_CFG_REG69_SW_FLUSH_SYNC_EN_BMSK                                   0x1000
#define HWIO_SE_GENI_CFG_REG69_SW_FLUSH_SYNC_EN_SHFT                                      0xc
#define HWIO_SE_GENI_CFG_REG69_AUTO_SYNC_FIFO_CLEAR_BMSK                                0x800
#define HWIO_SE_GENI_CFG_REG69_AUTO_SYNC_FIFO_CLEAR_SHFT                                  0xb
#define HWIO_SE_GENI_CFG_REG69_LAST_IS_DONE_BMSK                                        0x400
#define HWIO_SE_GENI_CFG_REG69_LAST_IS_DONE_SHFT                                          0xa
#define HWIO_SE_GENI_CFG_REG69_S_AUTO_OPCODE_CANCEL_BMSK                                0x200
#define HWIO_SE_GENI_CFG_REG69_S_AUTO_OPCODE_CANCEL_SHFT                                  0x9
#define HWIO_SE_GENI_CFG_REG69_S_OPCODE_CMD_CANCEL_BMSK                                 0x1f0
#define HWIO_SE_GENI_CFG_REG69_S_OPCODE_CMD_CANCEL_SHFT                                   0x4
#define HWIO_SE_GENI_CFG_REG69_S_DATA_CNT_MODE_BMSK                                       0x1
#define HWIO_SE_GENI_CFG_REG69_S_DATA_CNT_MODE_SHFT                                       0x0

#define HWIO_SE_GENI_CFG_REG70_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000118)
#define HWIO_SE_GENI_CFG_REG70_RMSK                                                     0x401
#define HWIO_SE_GENI_CFG_REG70_S_GP_CNT_DIRECTION_BMSK                                  0x400
#define HWIO_SE_GENI_CFG_REG70_S_GP_CNT_DIRECTION_SHFT                                    0xa
#define HWIO_SE_GENI_CFG_REG70_S_PROG_RAM_SEC_WORD_EN_BMSK                                0x1
#define HWIO_SE_GENI_CFG_REG70_S_PROG_RAM_SEC_WORD_EN_SHFT                                0x0

#define HWIO_SE_GENI_CFG_REG71_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000011c)
#define HWIO_SE_GENI_CFG_REG71_RMSK                                                0x3fffffff
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_EXT_BMSK                 0x3ff00000
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_EXT_SHFT                       0x14
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_BMSK                        0xffc00
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_0_SEL_RST_VAL_SHFT                            0xa
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_VEC_BMSK                                    0x3ff
#define HWIO_SE_GENI_CFG_REG71_COND_COMP_IN_VEC_SHFT                                      0x0

#define HWIO_SE_GENI_CFG_REG72_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000120)
#define HWIO_SE_GENI_CFG_REG72_RMSK                                                0x3fffffff
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_EXT_BMSK               0x3ff00000
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_EXT_SHFT                     0x14
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_BMSK                      0xffc00
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_0_SEL_RST_VAL_SHFT                          0xa
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_VEC_BMSK                                  0x3ff
#define HWIO_SE_GENI_CFG_REG72_S_COND_COMP_IN_VEC_SHFT                                    0x0

#define HWIO_SE_GENI_CFG_REG73_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000124)
#define HWIO_SE_GENI_CFG_REG73_RMSK                                                       0xf
#define HWIO_SE_GENI_CFG_REG73_SOUT_2D_SEL_BMSK                                           0xf
#define HWIO_SE_GENI_CFG_REG73_SOUT_2D_SEL_SHFT                                           0x0

#define HWIO_SE_GENI_CFG_REG74_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000128)
#define HWIO_SE_GENI_CFG_REG74_RMSK                                                       0x1
#define HWIO_SE_GENI_CFG_REG74_NOT_USED_BMSK                                              0x1
#define HWIO_SE_GENI_CFG_REG74_NOT_USED_SHFT                                              0x0

#define HWIO_SE_GENI_CFG_REG75_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000012c)
#define HWIO_SE_GENI_CFG_REG75_RMSK                                                       0x3
#define HWIO_SE_GENI_CFG_REG75_LOOPBACK_MODE_BMSK                                         0x3
#define HWIO_SE_GENI_CFG_REG75_LOOPBACK_MODE_SHFT                                         0x0

#define HWIO_SE_GENI_CFG_REG76_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000130)
#define HWIO_SE_GENI_CFG_REG76_RMSK                                                       0x7
#define HWIO_SE_GENI_CFG_REG76_IO2_ENGINE_INV_EN_BMSK                                     0x4
#define HWIO_SE_GENI_CFG_REG76_IO2_ENGINE_INV_EN_SHFT                                     0x2
#define HWIO_SE_GENI_CFG_REG76_RX_ENGINE_INV_EN_BMSK                                      0x2
#define HWIO_SE_GENI_CFG_REG76_RX_ENGINE_INV_EN_SHFT                                      0x1
#define HWIO_SE_GENI_CFG_REG76_TX_ENGINE_INV_EN_BMSK                                      0x1
#define HWIO_SE_GENI_CFG_REG76_TX_ENGINE_INV_EN_SHFT                                      0x0

#define HWIO_SE_GENI_CFG_REG77_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000134)
#define HWIO_SE_GENI_CFG_REG77_RMSK                                                      0x1f
#define HWIO_SE_GENI_CFG_REG77_SDA_NOISE_REJECT_BMSK                                     0x18
#define HWIO_SE_GENI_CFG_REG77_SDA_NOISE_REJECT_SHFT                                      0x3
#define HWIO_SE_GENI_CFG_REG77_SCL_NOISE_REJECT_BMSK                                      0x6
#define HWIO_SE_GENI_CFG_REG77_SCL_NOISE_REJECT_SHFT                                      0x1
#define HWIO_SE_GENI_CFG_REG77_LOW_PERIOD_NOISE_REJECT_EN_BMSK                            0x1
#define HWIO_SE_GENI_CFG_REG77_LOW_PERIOD_NOISE_REJECT_EN_SHFT                            0x0

#define HWIO_SE_GENI_CFG_REG78_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000138)
#define HWIO_SE_GENI_CFG_REG78_RMSK                                                       0x3
#define HWIO_SE_GENI_CFG_REG78_BUSY_INDICATION_SELECT_BMSK                                0x2
#define HWIO_SE_GENI_CFG_REG78_BUSY_INDICATION_SELECT_SHFT                                0x1
#define HWIO_SE_GENI_CFG_REG78_SDA_DELAYED_DETECTION_BMSK                                 0x1
#define HWIO_SE_GENI_CFG_REG78_SDA_DELAYED_DETECTION_SHFT                                 0x0

#define HWIO_SE_GENI_CFG_REG79_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000013c)
#define HWIO_SE_GENI_CFG_REG79_RMSK                                                       0x1
#define HWIO_SE_GENI_CFG_REG79_IO3_S_FE_FORCE_BMSK                                        0x1
#define HWIO_SE_GENI_CFG_REG79_IO3_S_FE_FORCE_SHFT                                        0x0

#define HWIO_SE_GENI_CFG_REG80_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000140)
#define HWIO_SE_GENI_CFG_REG80_RMSK                                                       0xf
#define HWIO_SE_GENI_CFG_REG80_IO_MACRO_MUX_SEL_BMSK                                      0xf
#define HWIO_SE_GENI_CFG_REG80_IO_MACRO_MUX_SEL_SHFT                                      0x0

#define HWIO_SE_GENI_CFG_REG81_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000144)
#define HWIO_SE_GENI_CFG_REG81_RMSK                                                       0xf
#define HWIO_SE_GENI_CFG_REG81_GENI_IO3_SOUT2_PULL_UP_EN_BMSK                             0x8
#define HWIO_SE_GENI_CFG_REG81_GENI_IO3_SOUT2_PULL_UP_EN_SHFT                             0x3
#define HWIO_SE_GENI_CFG_REG81_GENI_IO3_SOUT0_PULL_UP_EN_BMSK                             0x4
#define HWIO_SE_GENI_CFG_REG81_GENI_IO3_SOUT0_PULL_UP_EN_SHFT                             0x2
#define HWIO_SE_GENI_CFG_REG81_GENI_SOUT2_PULL_UP_EN_BMSK                                 0x2
#define HWIO_SE_GENI_CFG_REG81_GENI_SOUT2_PULL_UP_EN_SHFT                                 0x1
#define HWIO_SE_GENI_CFG_REG81_GENI_SOUT0_PULL_UP_EN_BMSK                                 0x1
#define HWIO_SE_GENI_CFG_REG81_GENI_SOUT0_PULL_UP_EN_SHFT                                 0x0

#define HWIO_SE_GENI_CFG_REG82_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000148)
#define HWIO_SE_GENI_CFG_REG82_RMSK                                                       0xf
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOE_VALUE_BMSK                            0x8
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOE_VALUE_SHFT                            0x3
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOUT_VALUE_BMSK                           0x4
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_PRIM_SOUT_VALUE_SHFT                           0x2
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_SOE_VALUE_BMSK                                 0x2
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_SOE_VALUE_SHFT                                 0x1
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_SOUT_VALUE_BMSK                                0x1
#define HWIO_SE_GENI_CFG_REG82_IO3_DEFAULT_SOUT_VALUE_SHFT                                0x0

#define HWIO_SE_GENI_CFG_REG83_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000014c)
#define HWIO_SE_GENI_CFG_REG83_RMSK                                                       0xf
#define HWIO_SE_GENI_CFG_REG83_IO3_DEMUX_OUTPUT_INV_BMSK                                  0xf
#define HWIO_SE_GENI_CFG_REG83_IO3_DEMUX_OUTPUT_INV_SHFT                                  0x0

#define HWIO_SE_GENI_CFG_REG84_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000150)
#define HWIO_SE_GENI_CFG_REG84_RMSK                                                      0x1f
#define HWIO_SE_GENI_CFG_REG84_IO3_DEMUX_FW_EN_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG84_IO3_DEMUX_FW_EN_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG84_IO3_DEMUX_SEL_BMSK                                         0xf
#define HWIO_SE_GENI_CFG_REG84_IO3_DEMUX_SEL_SHFT                                         0x0

#define HWIO_SE_GENI_CFG_REG85_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000154)
#define HWIO_SE_GENI_CFG_REG85_RMSK                                                       0x3
#define HWIO_SE_GENI_CFG_REG85_GENI_BYTE_GRANULARITY_BMSK                                 0x3
#define HWIO_SE_GENI_CFG_REG85_GENI_BYTE_GRANULARITY_SHFT                                 0x0

#define HWIO_SE_GENI_CFG_REG86_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000158)
#define HWIO_SE_GENI_CFG_REG86_RMSK                                                       0x1
#define HWIO_SE_GENI_CFG_REG86_GENI_DMA_MODE_EN_BMSK                                      0x1
#define HWIO_SE_GENI_CFG_REG86_GENI_DMA_MODE_EN_SHFT                                      0x0

#define HWIO_SE_GENI_CFG_REG87_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000015c)
#define HWIO_SE_GENI_CFG_REG87_RMSK                                                      0x3f
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP5_BMSK                                           0x20
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP5_SHFT                                            0x5
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP4_BMSK                                           0x10
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP4_SHFT                                            0x4
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP3_BMSK                                            0x8
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP3_SHFT                                            0x3
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP2_BMSK                                            0x4
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP2_SHFT                                            0x2
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP1_BMSK                                            0x2
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP1_SHFT                                            0x1
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP0_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG87_M_SW_COMP0_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG88_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000160)
#define HWIO_SE_GENI_CFG_REG88_RMSK                                                   0xfffff
#define HWIO_SE_GENI_CFG_REG88_VEC_1_START_INDEX_BMSK                                 0xf8000
#define HWIO_SE_GENI_CFG_REG88_VEC_1_START_INDEX_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG88_VEC_1_DIRECTION_BMSK                                    0x4000
#define HWIO_SE_GENI_CFG_REG88_VEC_1_DIRECTION_SHFT                                       0xe
#define HWIO_SE_GENI_CFG_REG88_VEC_1_LENGTH_BMSK                                       0x3800
#define HWIO_SE_GENI_CFG_REG88_VEC_1_LENGTH_SHFT                                          0xb
#define HWIO_SE_GENI_CFG_REG88_VEC_1_STOP_BMSK                                          0x400
#define HWIO_SE_GENI_CFG_REG88_VEC_1_STOP_SHFT                                            0xa
#define HWIO_SE_GENI_CFG_REG88_VEC_0_START_INDEX_BMSK                                   0x3e0
#define HWIO_SE_GENI_CFG_REG88_VEC_0_START_INDEX_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG88_VEC_0_DIRECTION_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG88_VEC_0_DIRECTION_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG88_VEC_0_LENGTH_BMSK                                          0xe
#define HWIO_SE_GENI_CFG_REG88_VEC_0_LENGTH_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG88_VEC_0_STOP_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG88_VEC_0_STOP_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG89_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000164)
#define HWIO_SE_GENI_CFG_REG89_RMSK                                                   0xfffff
#define HWIO_SE_GENI_CFG_REG89_VEC_3_START_INDEX_BMSK                                 0xf8000
#define HWIO_SE_GENI_CFG_REG89_VEC_3_START_INDEX_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG89_VEC_3_DIRECTION_BMSK                                    0x4000
#define HWIO_SE_GENI_CFG_REG89_VEC_3_DIRECTION_SHFT                                       0xe
#define HWIO_SE_GENI_CFG_REG89_VEC_3_LENGTH_BMSK                                       0x3800
#define HWIO_SE_GENI_CFG_REG89_VEC_3_LENGTH_SHFT                                          0xb
#define HWIO_SE_GENI_CFG_REG89_VEC_3_STOP_BMSK                                          0x400
#define HWIO_SE_GENI_CFG_REG89_VEC_3_STOP_SHFT                                            0xa
#define HWIO_SE_GENI_CFG_REG89_VEC_2_START_INDEX_BMSK                                   0x3e0
#define HWIO_SE_GENI_CFG_REG89_VEC_2_START_INDEX_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG89_VEC_2_DIRECTION_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG89_VEC_2_DIRECTION_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG89_VEC_2_LENGTH_BMSK                                          0xe
#define HWIO_SE_GENI_CFG_REG89_VEC_2_LENGTH_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG89_VEC_2_STOP_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG89_VEC_2_STOP_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG90_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000168)
#define HWIO_SE_GENI_CFG_REG90_RMSK                                                     0x3ff
#define HWIO_SE_GENI_CFG_REG90_M_GP_CNT0_BMSK                                           0x3ff
#define HWIO_SE_GENI_CFG_REG90_M_GP_CNT0_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG91_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000016c)
#define HWIO_SE_GENI_CFG_REG91_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG91_M_GP_CNT1_BMSK                                        0xffffff
#define HWIO_SE_GENI_CFG_REG91_M_GP_CNT1_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG92_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000170)
#define HWIO_SE_GENI_CFG_REG92_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG92_M_GP_CNT2_BMSK                                        0xffffff
#define HWIO_SE_GENI_CFG_REG92_M_GP_CNT2_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG93_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000174)
#define HWIO_SE_GENI_CFG_REG93_RMSK                                                  0xffffff
#define HWIO_SE_GENI_CFG_REG93_M_GP_CNT3_BMSK                                        0xffffff
#define HWIO_SE_GENI_CFG_REG93_M_GP_CNT3_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG94_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000178)
#define HWIO_SE_GENI_CFG_REG94_RMSK                                                0x3fffffff
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT6_BMSK                                      0x3ff00000
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT6_SHFT                                            0x14
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT5_BMSK                                         0xffc00
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT5_SHFT                                             0xa
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT4_BMSK                                           0x3ff
#define HWIO_SE_GENI_CFG_REG94_M_GP_CNT4_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG95_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000017c)
#define HWIO_SE_GENI_CFG_REG95_RMSK                                                     0x3ff
#define HWIO_SE_GENI_CFG_REG95_M_GP_CNT7_BMSK                                           0x3ff
#define HWIO_SE_GENI_CFG_REG95_M_GP_CNT7_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG96_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000180)
#define HWIO_SE_GENI_CFG_REG96_RMSK                                                      0x3f
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP5_BMSK                                           0x20
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP5_SHFT                                            0x5
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP4_BMSK                                           0x10
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP4_SHFT                                            0x4
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP3_BMSK                                            0x8
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP3_SHFT                                            0x3
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP2_BMSK                                            0x4
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP2_SHFT                                            0x2
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP1_BMSK                                            0x2
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP1_SHFT                                            0x1
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP0_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG96_S_SW_COMP0_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG97_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000184)
#define HWIO_SE_GENI_CFG_REG97_RMSK                                                   0xfffff
#define HWIO_SE_GENI_CFG_REG97_VEC_1_START_INDEX_BMSK                                 0xf8000
#define HWIO_SE_GENI_CFG_REG97_VEC_1_START_INDEX_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG97_VEC_1_DIRECTION_BMSK                                    0x4000
#define HWIO_SE_GENI_CFG_REG97_VEC_1_DIRECTION_SHFT                                       0xe
#define HWIO_SE_GENI_CFG_REG97_VEC_1_LENGTH_BMSK                                       0x3800
#define HWIO_SE_GENI_CFG_REG97_VEC_1_LENGTH_SHFT                                          0xb
#define HWIO_SE_GENI_CFG_REG97_VEC_1_STOP_BMSK                                          0x400
#define HWIO_SE_GENI_CFG_REG97_VEC_1_STOP_SHFT                                            0xa
#define HWIO_SE_GENI_CFG_REG97_VEC_0_START_INDEX_BMSK                                   0x3e0
#define HWIO_SE_GENI_CFG_REG97_VEC_0_START_INDEX_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG97_VEC_0_DIRECTION_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG97_VEC_0_DIRECTION_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG97_VEC_0_LENGTH_BMSK                                          0xe
#define HWIO_SE_GENI_CFG_REG97_VEC_0_LENGTH_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG97_VEC_0_STOP_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG97_VEC_0_STOP_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG98_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000188)
#define HWIO_SE_GENI_CFG_REG98_RMSK                                                   0xfffff
#define HWIO_SE_GENI_CFG_REG98_VEC_3_START_INDEX_BMSK                                 0xf8000
#define HWIO_SE_GENI_CFG_REG98_VEC_3_START_INDEX_SHFT                                     0xf
#define HWIO_SE_GENI_CFG_REG98_VEC_3_DIRECTION_BMSK                                    0x4000
#define HWIO_SE_GENI_CFG_REG98_VEC_3_DIRECTION_SHFT                                       0xe
#define HWIO_SE_GENI_CFG_REG98_VEC_3_LENGTH_BMSK                                       0x3800
#define HWIO_SE_GENI_CFG_REG98_VEC_3_LENGTH_SHFT                                          0xb
#define HWIO_SE_GENI_CFG_REG98_VEC_3_STOP_BMSK                                          0x400
#define HWIO_SE_GENI_CFG_REG98_VEC_3_STOP_SHFT                                            0xa
#define HWIO_SE_GENI_CFG_REG98_VEC_2_START_INDEX_BMSK                                   0x3e0
#define HWIO_SE_GENI_CFG_REG98_VEC_2_START_INDEX_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG98_VEC_2_DIRECTION_BMSK                                      0x10
#define HWIO_SE_GENI_CFG_REG98_VEC_2_DIRECTION_SHFT                                       0x4
#define HWIO_SE_GENI_CFG_REG98_VEC_2_LENGTH_BMSK                                          0xe
#define HWIO_SE_GENI_CFG_REG98_VEC_2_LENGTH_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG98_VEC_2_STOP_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG98_VEC_2_STOP_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG99_OFFS                                                (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000018c)
#define HWIO_SE_GENI_CFG_REG99_RMSK                                                     0x3ff
#define HWIO_SE_GENI_CFG_REG99_S_GP_CNT0_BMSK                                           0x3ff
#define HWIO_SE_GENI_CFG_REG99_S_GP_CNT0_SHFT                                             0x0

#define HWIO_SE_GENI_CFG_REG100_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000190)
#define HWIO_SE_GENI_CFG_REG100_RMSK                                                 0xffffff
#define HWIO_SE_GENI_CFG_REG100_S_GP_CNT1_BMSK                                       0xffffff
#define HWIO_SE_GENI_CFG_REG100_S_GP_CNT1_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG101_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000194)
#define HWIO_SE_GENI_CFG_REG101_RMSK                                                 0xffffff
#define HWIO_SE_GENI_CFG_REG101_S_GP_CNT2_BMSK                                       0xffffff
#define HWIO_SE_GENI_CFG_REG101_S_GP_CNT2_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG102_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000198)
#define HWIO_SE_GENI_CFG_REG102_RMSK                                                 0xffffff
#define HWIO_SE_GENI_CFG_REG102_S_GP_CNT3_BMSK                                       0xffffff
#define HWIO_SE_GENI_CFG_REG102_S_GP_CNT3_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG103_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000019c)
#define HWIO_SE_GENI_CFG_REG103_RMSK                                               0x3fffffff
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT6_BMSK                                     0x3ff00000
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT6_SHFT                                           0x14
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT5_BMSK                                        0xffc00
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT5_SHFT                                            0xa
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT4_BMSK                                          0x3ff
#define HWIO_SE_GENI_CFG_REG103_S_GP_CNT4_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG104_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001a0)
#define HWIO_SE_GENI_CFG_REG104_RMSK                                                    0x3ff
#define HWIO_SE_GENI_CFG_REG104_S_GP_CNT7_BMSK                                          0x3ff
#define HWIO_SE_GENI_CFG_REG104_S_GP_CNT7_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG105_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001a4)
#define HWIO_SE_GENI_CFG_REG105_RMSK                                                     0x1f
#define HWIO_SE_GENI_CFG_REG105_TX_PARITY_TOGGLE_EN_BMSK                                 0x10
#define HWIO_SE_GENI_CFG_REG105_TX_PARITY_TOGGLE_EN_SHFT                                  0x4
#define HWIO_SE_GENI_CFG_REG105_TX_FW_PAR_SEL_BMSK                                        0x8
#define HWIO_SE_GENI_CFG_REG105_TX_FW_PAR_SEL_SHFT                                        0x3
#define HWIO_SE_GENI_CFG_REG105_TX_PAR_MODE_BMSK                                          0x6
#define HWIO_SE_GENI_CFG_REG105_TX_PAR_MODE_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG105_TX_PAR_CALC_EN_BMSK                                       0x1
#define HWIO_SE_GENI_CFG_REG105_TX_PAR_CALC_EN_SHFT                                       0x0

#define HWIO_SE_GENI_CFG_REG106_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001a8)
#define HWIO_SE_GENI_CFG_REG106_RMSK                                                     0x1f
#define HWIO_SE_GENI_CFG_REG106_RX_PARITY_TOGGLE_EN_BMSK                                 0x10
#define HWIO_SE_GENI_CFG_REG106_RX_PARITY_TOGGLE_EN_SHFT                                  0x4
#define HWIO_SE_GENI_CFG_REG106_RX_FW_PAR_SEL_BMSK                                        0x8
#define HWIO_SE_GENI_CFG_REG106_RX_FW_PAR_SEL_SHFT                                        0x3
#define HWIO_SE_GENI_CFG_REG106_RX_PAR_MODE_BMSK                                          0x6
#define HWIO_SE_GENI_CFG_REG106_RX_PAR_MODE_SHFT                                          0x1
#define HWIO_SE_GENI_CFG_REG106_RX_PAR_CALC_EN_BMSK                                       0x1
#define HWIO_SE_GENI_CFG_REG106_RX_PAR_CALC_EN_SHFT                                       0x0

#define HWIO_SE_GENI_CFG_REG107_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001ac)
#define HWIO_SE_GENI_CFG_REG107_RMSK                                               0x80000003
#define HWIO_SE_GENI_CFG_REG107_IO3_SW_CONTROL_BMSK                                0x80000000
#define HWIO_SE_GENI_CFG_REG107_IO3_SW_CONTROL_SHFT                                      0x1f
#define HWIO_SE_GENI_CFG_REG107_IO3_FORCE_HIGH_BMSK                                       0x2
#define HWIO_SE_GENI_CFG_REG107_IO3_FORCE_HIGH_SHFT                                       0x1
#define HWIO_SE_GENI_CFG_REG107_IO3_FORCE_LOW_BMSK                                        0x1
#define HWIO_SE_GENI_CFG_REG107_IO3_FORCE_LOW_SHFT                                        0x0

#define HWIO_SE_GENI_CFG_REG108_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001b0)
#define HWIO_SE_GENI_CFG_REG108_RMSK                                                  0x7773f
#define HWIO_SE_GENI_CFG_REG108_TX_CORE_TO_IO_SAMP_EN_BMSK                            0x40000
#define HWIO_SE_GENI_CFG_REG108_TX_CORE_TO_IO_SAMP_EN_SHFT                               0x12
#define HWIO_SE_GENI_CFG_REG108_IO2_CORE_TO_IO_SAMP_EN_BMSK                           0x20000
#define HWIO_SE_GENI_CFG_REG108_IO2_CORE_TO_IO_SAMP_EN_SHFT                              0x11
#define HWIO_SE_GENI_CFG_REG108_IO3_CORE_TO_IO_SAMP_EN_BMSK                           0x10000
#define HWIO_SE_GENI_CFG_REG108_IO3_CORE_TO_IO_SAMP_EN_SHFT                              0x10
#define HWIO_SE_GENI_CFG_REG108_RX_SI_EN2IO_DELAY_BMSK                                 0x7000
#define HWIO_SE_GENI_CFG_REG108_RX_SI_EN2IO_DELAY_SHFT                                    0xc
#define HWIO_SE_GENI_CFG_REG108_RX_IO_EN2CORE_EN_DELAY_BMSK                             0x700
#define HWIO_SE_GENI_CFG_REG108_RX_IO_EN2CORE_EN_DELAY_SHFT                               0x8
#define HWIO_SE_GENI_CFG_REG108_RX_POS_FF_EN_SEL_BMSK                                    0x20
#define HWIO_SE_GENI_CFG_REG108_RX_POS_FF_EN_SEL_SHFT                                     0x5
#define HWIO_SE_GENI_CFG_REG108_RX_IO_POS_FF_EN_SEL_BMSK                                 0x10
#define HWIO_SE_GENI_CFG_REG108_RX_IO_POS_FF_EN_SEL_SHFT                                  0x4
#define HWIO_SE_GENI_CFG_REG108_RX_IO_INT_CLK_SAMP_EN_BMSK                                0x8
#define HWIO_SE_GENI_CFG_REG108_RX_IO_INT_CLK_SAMP_EN_SHFT                                0x3
#define HWIO_SE_GENI_CFG_REG108_RX_IO_PIPE_STAGE_SEL_BMSK                                 0x4
#define HWIO_SE_GENI_CFG_REG108_RX_IO_PIPE_STAGE_SEL_SHFT                                 0x2
#define HWIO_SE_GENI_CFG_REG108_CPOL_CTRL_BMSK                                            0x2
#define HWIO_SE_GENI_CFG_REG108_CPOL_CTRL_SHFT                                            0x1
#define HWIO_SE_GENI_CFG_REG108_CPHA_CTRL_BMSK                                            0x1
#define HWIO_SE_GENI_CFG_REG108_CPHA_CTRL_SHFT                                            0x0

#define HWIO_SE_GENI_CFG_REG109_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001b4)
#define HWIO_SE_GENI_CFG_REG109_RMSK                                                      0x1
#define HWIO_SE_GENI_CFG_REG109_RX_CPOL_BMSK                                              0x1
#define HWIO_SE_GENI_CFG_REG109_RX_CPOL_SHFT                                              0x0

#define HWIO_SE_UART_LOOPBACK_CFG_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000012c)
#define HWIO_SE_UART_LOOPBACK_CFG_RMSK                                                    0x3
#define HWIO_SE_UART_LOOPBACK_CFG_LOOPBACK_MODE_BMSK                                      0x3
#define HWIO_SE_UART_LOOPBACK_CFG_LOOPBACK_MODE_SHFT                                      0x0

#define HWIO_SE_UART_TX_TRANS_CFG_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000015c)
#define HWIO_SE_UART_TX_TRANS_CFG_RMSK                                                    0x3
#define HWIO_SE_UART_TX_TRANS_CFG_UART_CTS_MASK_BMSK                                      0x2
#define HWIO_SE_UART_TX_TRANS_CFG_UART_CTS_MASK_SHFT                                      0x1
#define HWIO_SE_UART_TX_TRANS_CFG_UART_PARITY_EN_BMSK                                     0x1
#define HWIO_SE_UART_TX_TRANS_CFG_UART_PARITY_EN_SHFT                                     0x0

#define HWIO_SE_UART_TX_WORD_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000168)
#define HWIO_SE_UART_TX_WORD_LEN_RMSK                                                   0x3ff
#define HWIO_SE_UART_TX_WORD_LEN_UART_TX_WORD_LEN_BMSK                                  0x3ff
#define HWIO_SE_UART_TX_WORD_LEN_UART_TX_WORD_LEN_SHFT                                    0x0

#define HWIO_SE_UART_TX_STOP_BIT_LEN_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000016c)
#define HWIO_SE_UART_TX_STOP_BIT_LEN_RMSK                                            0xffffff
#define HWIO_SE_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_BMSK                       0xffffff
#define HWIO_SE_UART_TX_STOP_BIT_LEN_UART_TX_STOP_BIT_LEN_SHFT                            0x0

#define HWIO_SE_UART_TX_TRANS_LEN_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000170)
#define HWIO_SE_UART_TX_TRANS_LEN_RMSK                                               0xffffff
#define HWIO_SE_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_BMSK                             0xffffff
#define HWIO_SE_UART_TX_TRANS_LEN_UART_TX_TRANS_LEN_SHFT                                  0x0

#define HWIO_SE_UART_RX_TRANS_CFG_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000180)
#define HWIO_SE_UART_RX_TRANS_CFG_RMSK                                                   0x14
#define HWIO_SE_UART_RX_TRANS_CFG_UART_PARITY_EN_BMSK                                    0x10
#define HWIO_SE_UART_RX_TRANS_CFG_UART_PARITY_EN_SHFT                                     0x4
#define HWIO_SE_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_BMSK                          0x4
#define HWIO_SE_UART_RX_TRANS_CFG_UART_RX_INSERT_STATUS_BIT_SHFT                          0x2

#define HWIO_SE_UART_RX_WORD_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000018c)
#define HWIO_SE_UART_RX_WORD_LEN_RMSK                                                   0x3ff
#define HWIO_SE_UART_RX_WORD_LEN_UART_RX_WORD_LEN_BMSK                                  0x3ff
#define HWIO_SE_UART_RX_WORD_LEN_UART_RX_WORD_LEN_SHFT                                    0x0

#define HWIO_SE_UART_RX_STALE_CNT_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000194)
#define HWIO_SE_UART_RX_STALE_CNT_RMSK                                               0xffffff
#define HWIO_SE_UART_RX_STALE_CNT_UART_RX_STALE_CNT_BMSK                             0xffffff
#define HWIO_SE_UART_RX_STALE_CNT_UART_RX_STALE_CNT_SHFT                                  0x0

#define HWIO_SE_UART_TX_PARITY_CFG_OFFS                                            (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001a4)
#define HWIO_SE_UART_TX_PARITY_CFG_RMSK                                                   0x7
#define HWIO_SE_UART_TX_PARITY_CFG_TX_PAR_MODE_BMSK                                       0x6
#define HWIO_SE_UART_TX_PARITY_CFG_TX_PAR_MODE_SHFT                                       0x1
#define HWIO_SE_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_BMSK                                    0x1
#define HWIO_SE_UART_TX_PARITY_CFG_TX_PAR_CALC_EN_SHFT                                    0x0

#define HWIO_SE_UART_RX_PARITY_CFG_OFFS                                            (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001a8)
#define HWIO_SE_UART_RX_PARITY_CFG_RMSK                                                   0x7
#define HWIO_SE_UART_RX_PARITY_CFG_RX_PAR_MODE_BMSK                                       0x6
#define HWIO_SE_UART_RX_PARITY_CFG_RX_PAR_MODE_SHFT                                       0x1
#define HWIO_SE_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_BMSK                                    0x1
#define HWIO_SE_UART_RX_PARITY_CFG_RX_PAR_CALC_EN_SHFT                                    0x0

#define HWIO_SE_UART_MANUAL_RFR_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x000001ac)
#define HWIO_SE_UART_MANUAL_RFR_RMSK                                               0x80000003
#define HWIO_SE_UART_MANUAL_RFR_UART_MANUAL_RFR_EN_BMSK                            0x80000000
#define HWIO_SE_UART_MANUAL_RFR_UART_MANUAL_RFR_EN_SHFT                                  0x1f
#define HWIO_SE_UART_MANUAL_RFR_UART_RFR_NOT_READY_BMSK                                   0x2
#define HWIO_SE_UART_MANUAL_RFR_UART_RFR_NOT_READY_SHFT                                   0x1
#define HWIO_SE_UART_MANUAL_RFR_UART_RFR_READY_BMSK                                       0x1
#define HWIO_SE_UART_MANUAL_RFR_UART_RFR_READY_SHFT                                       0x0

#define HWIO_SE_UART_RX_CHAR_HUNT_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000178)
#define HWIO_SE_UART_RX_CHAR_HUNT_RMSK                                                  0x3ff
#define HWIO_SE_UART_RX_CHAR_HUNT_CHAR_HUNT_PATTERN_BMSK                                0x3ff
#define HWIO_SE_UART_RX_CHAR_HUNT_CHAR_HUNT_PATTERN_SHFT                                  0x0

#define HWIO_SE_GENI_BYTE_GRANULARITY_OFFS                                         (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000154)
#define HWIO_SE_GENI_BYTE_GRANULARITY_RMSK                                                0x3
#define HWIO_SE_GENI_BYTE_GRANULARITY_GENI_BYTE_GRANULARITY_BMSK                          0x3
#define HWIO_SE_GENI_BYTE_GRANULARITY_GENI_BYTE_GRANULARITY_SHFT                          0x0

#define HWIO_SE_GENI_DMA_MODE_EN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000158)
#define HWIO_SE_GENI_DMA_MODE_EN_RMSK                                                     0x1
#define HWIO_SE_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK                                    0x1
#define HWIO_SE_GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_SHFT                                    0x0

#define HWIO_SE_GENI_TX_PACKING_CFG0_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000160)
#define HWIO_SE_GENI_TX_PACKING_CFG0_RMSK                                             0xfffff
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_START_INDEX_BMSK                           0xf8000
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_START_INDEX_SHFT                               0xf
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_DIRECTION_BMSK                              0x4000
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_DIRECTION_SHFT                                 0xe
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_LENGTH_BMSK                                 0x3800
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_LENGTH_SHFT                                    0xb
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_STOP_BMSK                                    0x400
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_1_STOP_SHFT                                      0xa
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_START_INDEX_BMSK                             0x3e0
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_START_INDEX_SHFT                               0x5
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_DIRECTION_BMSK                                0x10
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_DIRECTION_SHFT                                 0x4
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_LENGTH_BMSK                                    0xe
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_LENGTH_SHFT                                    0x1
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_STOP_BMSK                                      0x1
#define HWIO_SE_GENI_TX_PACKING_CFG0_VEC_0_STOP_SHFT                                      0x0

#define HWIO_SE_GENI_TX_PACKING_CFG1_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000164)
#define HWIO_SE_GENI_TX_PACKING_CFG1_RMSK                                             0xfffff
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_START_INDEX_BMSK                           0xf8000
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_START_INDEX_SHFT                               0xf
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_DIRECTION_BMSK                              0x4000
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_DIRECTION_SHFT                                 0xe
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_LENGTH_BMSK                                 0x3800
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_LENGTH_SHFT                                    0xb
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_STOP_BMSK                                    0x400
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_3_STOP_SHFT                                      0xa
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_START_INDEX_BMSK                             0x3e0
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_START_INDEX_SHFT                               0x5
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_DIRECTION_BMSK                                0x10
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_DIRECTION_SHFT                                 0x4
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_LENGTH_BMSK                                    0xe
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_LENGTH_SHFT                                    0x1
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_STOP_BMSK                                      0x1
#define HWIO_SE_GENI_TX_PACKING_CFG1_VEC_2_STOP_SHFT                                      0x0

#define HWIO_SE_GENI_RX_PACKING_CFG0_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000184)
#define HWIO_SE_GENI_RX_PACKING_CFG0_RMSK                                             0xfffff
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_START_INDEX_BMSK                           0xf8000
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_START_INDEX_SHFT                               0xf
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_DIRECTION_BMSK                              0x4000
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_DIRECTION_SHFT                                 0xe
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_LENGTH_BMSK                                 0x3800
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_LENGTH_SHFT                                    0xb
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_STOP_BMSK                                    0x400
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_1_STOP_SHFT                                      0xa
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_START_INDEX_BMSK                             0x3e0
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_START_INDEX_SHFT                               0x5
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_DIRECTION_BMSK                                0x10
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_DIRECTION_SHFT                                 0x4
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_LENGTH_BMSK                                    0xe
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_LENGTH_SHFT                                    0x1
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_STOP_BMSK                                      0x1
#define HWIO_SE_GENI_RX_PACKING_CFG0_VEC_0_STOP_SHFT                                      0x0

#define HWIO_SE_GENI_RX_PACKING_CFG1_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000188)
#define HWIO_SE_GENI_RX_PACKING_CFG1_RMSK                                             0xfffff
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_START_INDEX_BMSK                           0xf8000
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_START_INDEX_SHFT                               0xf
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_DIRECTION_BMSK                              0x4000
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_DIRECTION_SHFT                                 0xe
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_LENGTH_BMSK                                 0x3800
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_LENGTH_SHFT                                    0xb
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_STOP_BMSK                                    0x400
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_3_STOP_SHFT                                      0xa
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_START_INDEX_BMSK                             0x3e0
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_START_INDEX_SHFT                               0x5
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_DIRECTION_BMSK                                0x10
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_DIRECTION_SHFT                                 0x4
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_LENGTH_BMSK                                    0xe
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_LENGTH_SHFT                                    0x1
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_STOP_BMSK                                      0x1
#define HWIO_SE_GENI_RX_PACKING_CFG1_VEC_2_STOP_SHFT                                      0x0

#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_OFFS                                     (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000134)
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_RMSK                                           0x1f
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_BMSK                          0x18
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_SDA_NOISE_REJECT_SHFT                           0x3
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_BMSK                           0x6
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_SCL_NOISE_REJECT_SHFT                           0x1
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_BMSK                 0x1
#define HWIO_SE_I2C_NOISE_CANCELATION_CTL_LOW_PERIOD_NOISE_REJECT_EN_SHFT                 0x0

#define HWIO_SE_I2C_MONITOR_CTL_OFFS                                               (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000138)
#define HWIO_SE_I2C_MONITOR_CTL_RMSK                                                      0x1
#define HWIO_SE_I2C_MONITOR_CTL_SDA_DELAYED_DETECTION_BMSK                                0x1
#define HWIO_SE_I2C_MONITOR_CTL_SDA_DELAYED_DETECTION_SHFT                                0x0

#define HWIO_SE_I2C_TX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000016c)
#define HWIO_SE_I2C_TX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_I2C_TX_TRANS_LEN_I2C_TX_TRANS_LEN_BMSK                               0xffffff
#define HWIO_SE_I2C_TX_TRANS_LEN_I2C_TX_TRANS_LEN_SHFT                                    0x0

#define HWIO_SE_I2C_RX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000170)
#define HWIO_SE_I2C_RX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_I2C_RX_TRANS_LEN_I2C_RX_TRANS_LEN_BMSK                               0xffffff
#define HWIO_SE_I2C_RX_TRANS_LEN_I2C_RX_TRANS_LEN_SHFT                                    0x0

#define HWIO_SE_I2C_DELAY_COUNTER_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000174)
#define HWIO_SE_I2C_DELAY_COUNTER_RMSK                                               0xffffff
#define HWIO_SE_I2C_DELAY_COUNTER_I2C_DELAY_COUNTER_BMSK                             0xffffff
#define HWIO_SE_I2C_DELAY_COUNTER_I2C_DELAY_COUNTER_SHFT                                  0x0

#define HWIO_SE_I2C_SCL_COUNTERS_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000178)
#define HWIO_SE_I2C_SCL_COUNTERS_RMSK                                              0x3fffffff
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_BMSK                         0x3ff00000
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_SHFT                               0x14
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_BMSK                             0xffc00
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_SHFT                                 0xa
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_BMSK                             0x3ff
#define HWIO_SE_I2C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_SHFT                               0x0

#define HWIO_SE_SPI_CPHA_OFFS                                                      (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000124)
#define HWIO_SE_SPI_CPHA_RMSK                                                             0xf
#define HWIO_SE_SPI_CPHA_CPHA_BMSK                                                        0xf
#define HWIO_SE_SPI_CPHA_CPHA_SHFT                                                        0x0

#define HWIO_SE_SPI_LOOPBACK_CFG_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000012c)
#define HWIO_SE_SPI_LOOPBACK_CFG_RMSK                                                     0x3
#define HWIO_SE_SPI_LOOPBACK_CFG_LOOPBACK_MODE_BMSK                                       0x3
#define HWIO_SE_SPI_LOOPBACK_CFG_LOOPBACK_MODE_SHFT                                       0x0

#define HWIO_SE_SPI_CPOL_OFFS                                                      (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000130)
#define HWIO_SE_SPI_CPOL_RMSK                                                             0x4
#define HWIO_SE_SPI_CPOL_CPOL_BMSK                                                        0x4
#define HWIO_SE_SPI_CPOL_CPOL_SHFT                                                        0x2

#define HWIO_SE_SPI_DEMUX_OUTPUT_INV_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000014c)
#define HWIO_SE_SPI_DEMUX_OUTPUT_INV_RMSK                                                 0xf
#define HWIO_SE_SPI_DEMUX_OUTPUT_INV_CS_DEMUX_OUTPUT_INV_BMSK                             0xf
#define HWIO_SE_SPI_DEMUX_OUTPUT_INV_CS_DEMUX_OUTPUT_INV_SHFT                             0x0

#define HWIO_SE_SPI_DEMUX_SEL_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000150)
#define HWIO_SE_SPI_DEMUX_SEL_RMSK                                                       0x1f
#define HWIO_SE_SPI_DEMUX_SEL_CS_DEMUX_FW_EN_BMSK                                        0x10
#define HWIO_SE_SPI_DEMUX_SEL_CS_DEMUX_FW_EN_SHFT                                         0x4
#define HWIO_SE_SPI_DEMUX_SEL_CS_DEMUX_SEL_BMSK                                           0xf
#define HWIO_SE_SPI_DEMUX_SEL_CS_DEMUX_SEL_SHFT                                           0x0

#define HWIO_SE_SPI_TRANS_CFG_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000015c)
#define HWIO_SE_SPI_TRANS_CFG_RMSK                                                        0xe
#define HWIO_SE_SPI_TRANS_CFG_SPI_NOT_USED_CFG2_BMSK                                      0x8
#define HWIO_SE_SPI_TRANS_CFG_SPI_NOT_USED_CFG2_SHFT                                      0x3
#define HWIO_SE_SPI_TRANS_CFG_SPI_NOT_USED_CFG1_BMSK                                      0x4
#define HWIO_SE_SPI_TRANS_CFG_SPI_NOT_USED_CFG1_SHFT                                      0x2
#define HWIO_SE_SPI_TRANS_CFG_SPI_CS_TOGGLE_BMSK                                          0x2
#define HWIO_SE_SPI_TRANS_CFG_SPI_CS_TOGGLE_SHFT                                          0x1

#define HWIO_SE_SPI_WORD_LEN_OFFS                                                  (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000168)
#define HWIO_SE_SPI_WORD_LEN_RMSK                                                       0x3ff
#define HWIO_SE_SPI_WORD_LEN_SPI_WORD_LEN_BMSK                                          0x3ff
#define HWIO_SE_SPI_WORD_LEN_SPI_WORD_LEN_SHFT                                            0x0

#define HWIO_SE_SPI_TX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000016c)
#define HWIO_SE_SPI_TX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_SPI_TX_TRANS_LEN_SPI_TX_TRANS_LEN_BMSK                               0xffffff
#define HWIO_SE_SPI_TX_TRANS_LEN_SPI_TX_TRANS_LEN_SHFT                                    0x0

#define HWIO_SE_SPI_RX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000170)
#define HWIO_SE_SPI_RX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_SPI_RX_TRANS_LEN_SPI_RX_TRANS_LEN_BMSK                               0xffffff
#define HWIO_SE_SPI_RX_TRANS_LEN_SPI_RX_TRANS_LEN_SHFT                                    0x0

#define HWIO_SE_SPI_PRE_POST_CMD_DLY_OFFS                                          (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000174)
#define HWIO_SE_SPI_PRE_POST_CMD_DLY_RMSK                                            0xffffff
#define HWIO_SE_SPI_PRE_POST_CMD_DLY_SPI_PRE_POST_CMD_DLY_BMSK                       0xffffff
#define HWIO_SE_SPI_PRE_POST_CMD_DLY_SPI_PRE_POST_CMD_DLY_SHFT                            0x0

#define HWIO_SE_SPI_DELAYS_COUNTERS_OFFS                                           (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000178)
#define HWIO_SE_SPI_DELAYS_COUNTERS_RMSK                                              0xfffff
#define HWIO_SE_SPI_DELAYS_COUNTERS_SPI_CS_CLK_DLY_BMSK                               0xffc00
#define HWIO_SE_SPI_DELAYS_COUNTERS_SPI_CS_CLK_DLY_SHFT                                   0xa
#define HWIO_SE_SPI_DELAYS_COUNTERS_SPI_INTER_WORDS_DLY_BMSK                            0x3ff
#define HWIO_SE_SPI_DELAYS_COUNTERS_SPI_INTER_WORDS_DLY_SHFT                              0x0

#define HWIO_SE_I3C_TRANS_CFG_OFFS                                                 (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000015c)
#define HWIO_SE_I3C_TRANS_CFG_RMSK                                                        0x6
#define HWIO_SE_I3C_TRANS_CFG_I3C_USE_7E_BMSK                                             0x4
#define HWIO_SE_I3C_TRANS_CFG_I3C_USE_7E_SHFT                                             0x2
#define HWIO_SE_I3C_TRANS_CFG_I3C_IBI_NACK_CTRL_BMSK                                      0x2
#define HWIO_SE_I3C_TRANS_CFG_I3C_IBI_NACK_CTRL_SHFT                                      0x1

#define HWIO_SE_I3C_TX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000016c)
#define HWIO_SE_I3C_TX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_I3C_TX_TRANS_LEN_TX_TRANS_LEN_BMSK                                   0xffffff
#define HWIO_SE_I3C_TX_TRANS_LEN_TX_TRANS_LEN_SHFT                                        0x0

#define HWIO_SE_I3C_RX_TRANS_LEN_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000170)
#define HWIO_SE_I3C_RX_TRANS_LEN_RMSK                                                0xffffff
#define HWIO_SE_I3C_RX_TRANS_LEN_RX_TRANS_LEN_BMSK                                   0xffffff
#define HWIO_SE_I3C_RX_TRANS_LEN_RX_TRANS_LEN_SHFT                                        0x0

#define HWIO_SE_I3C_DELAY_COUNTER_OFFS                                             (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000174)
#define HWIO_SE_I3C_DELAY_COUNTER_RMSK                                               0xffffff
#define HWIO_SE_I3C_DELAY_COUNTER_DELAY_COUNTER_BMSK                                 0xffffff
#define HWIO_SE_I3C_DELAY_COUNTER_DELAY_COUNTER_SHFT                                      0x0

#define HWIO_SE_I3C_SCL_COUNTERS_OFFS                                              (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x00000178)
#define HWIO_SE_I3C_SCL_COUNTERS_RMSK                                              0x3fffffff
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_BMSK                         0x3ff00000
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_HIGH_COUNTER_SHFT                               0x14
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_BMSK                             0xffc00
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_LOW_COUNTER_SHFT                                 0xa
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_BMSK                             0x3ff
#define HWIO_SE_I3C_SCL_COUNTERS_I2C_SCL_CYCLE_COUNTER_SHFT                               0x0

#define HWIO_SE_I3C_SCL_LOW_OFFS                                                   (SE_GENI4_IMAGE_REGS_REG_BASE      + 0x0000017c)
#define HWIO_SE_I3C_SCL_LOW_RMSK                                                        0x3ff
#define HWIO_SE_I3C_SCL_LOW_I3C_SCL_LOW_COUNTER_BMSK                                    0x3ff
#define HWIO_SE_I3C_SCL_LOW_I3C_SCL_LOW_COUNTER_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_QUPV3_SE_DMA
 *--------------------------------------------------------------------------*/

#define SE_QUPV3_SE_DMA_REG_BASE                                            (SE_GENI4_CFG_REG_BASE      + 0x00000c00)

#define HWIO_SE_DMA_TX_PTR_L_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000030)
#define HWIO_SE_DMA_TX_PTR_L_RMSK                                           0xffffffff
#define HWIO_SE_DMA_TX_PTR_L_TX_PTR_L_BMSK                                  0xffffffff
#define HWIO_SE_DMA_TX_PTR_L_TX_PTR_L_SHFT                                         0x0

#define HWIO_SE_DMA_TX_PTR_H_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000034)
#define HWIO_SE_DMA_TX_PTR_H_RMSK                                           0xffffffff
#define HWIO_SE_DMA_TX_PTR_H_TX_PTR_H_BMSK                                  0xffffffff
#define HWIO_SE_DMA_TX_PTR_H_TX_PTR_H_SHFT                                         0x0

#define HWIO_SE_DMA_TX_ATTR_OFFS                                            (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000038)
#define HWIO_SE_DMA_TX_ATTR_RMSK                                                 0xf0f
#define HWIO_SE_DMA_TX_ATTR_GPII_INDEX_BMSK                                      0xf00
#define HWIO_SE_DMA_TX_ATTR_GPII_INDEX_SHFT                                        0x8
#define HWIO_SE_DMA_TX_ATTR_REQ_PRIORITY_BMSK                                      0x8
#define HWIO_SE_DMA_TX_ATTR_REQ_PRIORITY_SHFT                                      0x3
#define HWIO_SE_DMA_TX_ATTR_DMA_OPERATION_BMSK                                     0x6
#define HWIO_SE_DMA_TX_ATTR_DMA_OPERATION_SHFT                                     0x1
#define HWIO_SE_DMA_TX_ATTR_EOT_BMSK                                               0x1
#define HWIO_SE_DMA_TX_ATTR_EOT_SHFT                                               0x0

#define HWIO_SE_DMA_TX_LENGTH_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000003c)
#define HWIO_SE_DMA_TX_LENGTH_RMSK                                            0xffffff
#define HWIO_SE_DMA_TX_LENGTH_TX_LENGTH_BMSK                                  0xffffff
#define HWIO_SE_DMA_TX_LENGTH_TX_LENGTH_SHFT                                       0x0

#define HWIO_SE_DMA_TX_IRQ_STAT_OFFS                                        (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000040)
#define HWIO_SE_DMA_TX_IRQ_STAT_RMSK                                               0xf
#define HWIO_SE_DMA_TX_IRQ_STAT_RESET_DONE_BMSK                                    0x8
#define HWIO_SE_DMA_TX_IRQ_STAT_RESET_DONE_SHFT                                    0x3
#define HWIO_SE_DMA_TX_IRQ_STAT_SBE_BMSK                                           0x4
#define HWIO_SE_DMA_TX_IRQ_STAT_SBE_SHFT                                           0x2
#define HWIO_SE_DMA_TX_IRQ_STAT_EOT_BMSK                                           0x2
#define HWIO_SE_DMA_TX_IRQ_STAT_EOT_SHFT                                           0x1
#define HWIO_SE_DMA_TX_IRQ_STAT_DMA_DONE_BMSK                                      0x1
#define HWIO_SE_DMA_TX_IRQ_STAT_DMA_DONE_SHFT                                      0x0

#define HWIO_SE_DMA_TX_IRQ_CLR_OFFS                                         (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000044)
#define HWIO_SE_DMA_TX_IRQ_CLR_RMSK                                                0xf
#define HWIO_SE_DMA_TX_IRQ_CLR_RESET_DONE_CLR_BMSK                                 0x8
#define HWIO_SE_DMA_TX_IRQ_CLR_RESET_DONE_CLR_SHFT                                 0x3
#define HWIO_SE_DMA_TX_IRQ_CLR_SBE_CLR_BMSK                                        0x4
#define HWIO_SE_DMA_TX_IRQ_CLR_SBE_CLR_SHFT                                        0x2
#define HWIO_SE_DMA_TX_IRQ_CLR_EOT_CLR_BMSK                                        0x2
#define HWIO_SE_DMA_TX_IRQ_CLR_EOT_CLR_SHFT                                        0x1
#define HWIO_SE_DMA_TX_IRQ_CLR_DMA_DONE_CLR_BMSK                                   0x1
#define HWIO_SE_DMA_TX_IRQ_CLR_DMA_DONE_CLR_SHFT                                   0x0

#define HWIO_SE_DMA_TX_IRQ_EN_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000048)
#define HWIO_SE_DMA_TX_IRQ_EN_RMSK                                                 0xf
#define HWIO_SE_DMA_TX_IRQ_EN_RESET_DONE_EN_BMSK                                   0x8
#define HWIO_SE_DMA_TX_IRQ_EN_RESET_DONE_EN_SHFT                                   0x3
#define HWIO_SE_DMA_TX_IRQ_EN_SBE_EN_BMSK                                          0x4
#define HWIO_SE_DMA_TX_IRQ_EN_SBE_EN_SHFT                                          0x2
#define HWIO_SE_DMA_TX_IRQ_EN_EOT_EN_BMSK                                          0x2
#define HWIO_SE_DMA_TX_IRQ_EN_EOT_EN_SHFT                                          0x1
#define HWIO_SE_DMA_TX_IRQ_EN_DMA_DONE_EN_BMSK                                     0x1
#define HWIO_SE_DMA_TX_IRQ_EN_DMA_DONE_EN_SHFT                                     0x0

#define HWIO_SE_DMA_TX_IRQ_EN_SET_OFFS                                      (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000004c)
#define HWIO_SE_DMA_TX_IRQ_EN_SET_RMSK                                             0xf
#define HWIO_SE_DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK                           0x8
#define HWIO_SE_DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_SHFT                           0x3
#define HWIO_SE_DMA_TX_IRQ_EN_SET_SBE_EN_SET_BMSK                                  0x4
#define HWIO_SE_DMA_TX_IRQ_EN_SET_SBE_EN_SET_SHFT                                  0x2
#define HWIO_SE_DMA_TX_IRQ_EN_SET_EOT_EN_SET_BMSK                                  0x2
#define HWIO_SE_DMA_TX_IRQ_EN_SET_EOT_EN_SET_SHFT                                  0x1
#define HWIO_SE_DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK                             0x1
#define HWIO_SE_DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_SHFT                             0x0

#define HWIO_SE_DMA_TX_IRQ_EN_CLR_OFFS                                      (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000050)
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_RMSK                                             0xf
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_RESET_DONE_EN_CLR_BMSK                           0x8
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_RESET_DONE_EN_CLR_SHFT                           0x3
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_SBE_EN_CLR_BMSK                                  0x4
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_SBE_EN_CLR_SHFT                                  0x2
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_EOT_EN_CLR_BMSK                                  0x2
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_EOT_EN_CLR_SHFT                                  0x1
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_DMA_DONE_EN_CLR_BMSK                             0x1
#define HWIO_SE_DMA_TX_IRQ_EN_CLR_DMA_DONE_EN_CLR_SHFT                             0x0

#define HWIO_SE_DMA_TX_LENGTH_IN_OFFS                                       (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000054)
#define HWIO_SE_DMA_TX_LENGTH_IN_RMSK                                         0xffffff
#define HWIO_SE_DMA_TX_LENGTH_IN_TX_LENGTH_IN_BMSK                            0xffffff
#define HWIO_SE_DMA_TX_LENGTH_IN_TX_LENGTH_IN_SHFT                                 0x0

#define HWIO_SE_DMA_TX_FSM_RST_OFFS                                         (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000058)
#define HWIO_SE_DMA_TX_FSM_RST_RMSK                                                0x1
#define HWIO_SE_DMA_TX_FSM_RST_TX_FSM_RST_BMSK                                     0x1
#define HWIO_SE_DMA_TX_FSM_RST_TX_FSM_RST_SHFT                                     0x0

#define HWIO_SE_DMA_TX_MAX_BURST_SIZE_OFFS                                  (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000005c)
#define HWIO_SE_DMA_TX_MAX_BURST_SIZE_RMSK                                         0x3
#define HWIO_SE_DMA_TX_MAX_BURST_SIZE_TX_MAX_BURST_SIZE_BMSK                       0x3
#define HWIO_SE_DMA_TX_MAX_BURST_SIZE_TX_MAX_BURST_SIZE_SHFT                       0x0

#define HWIO_SE_DMA_RX_PTR_L_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000130)
#define HWIO_SE_DMA_RX_PTR_L_RMSK                                           0xffffffff
#define HWIO_SE_DMA_RX_PTR_L_RX_PTR_L_BMSK                                  0xffffffff
#define HWIO_SE_DMA_RX_PTR_L_RX_PTR_L_SHFT                                         0x0

#define HWIO_SE_DMA_RX_PTR_H_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000134)
#define HWIO_SE_DMA_RX_PTR_H_RMSK                                           0xffffffff
#define HWIO_SE_DMA_RX_PTR_H_RX_PTR_H_BMSK                                  0xffffffff
#define HWIO_SE_DMA_RX_PTR_H_RX_PTR_H_SHFT                                         0x0

#define HWIO_SE_DMA_RX_ATTR_OFFS                                            (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000138)
#define HWIO_SE_DMA_RX_ATTR_RMSK                                                 0xf0e
#define HWIO_SE_DMA_RX_ATTR_GPII_INDEX_BMSK                                      0xf00
#define HWIO_SE_DMA_RX_ATTR_GPII_INDEX_SHFT                                        0x8
#define HWIO_SE_DMA_RX_ATTR_REQ_PRIORITY_BMSK                                      0x8
#define HWIO_SE_DMA_RX_ATTR_REQ_PRIORITY_SHFT                                      0x3
#define HWIO_SE_DMA_RX_ATTR_DMA_OPERATION_BMSK                                     0x6
#define HWIO_SE_DMA_RX_ATTR_DMA_OPERATION_SHFT                                     0x1

#define HWIO_SE_DMA_RX_LENGTH_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000013c)
#define HWIO_SE_DMA_RX_LENGTH_RMSK                                            0xffffff
#define HWIO_SE_DMA_RX_LENGTH_RX_LENGTH_BMSK                                  0xffffff
#define HWIO_SE_DMA_RX_LENGTH_RX_LENGTH_SHFT                                       0x0

#define HWIO_SE_DMA_RX_IRQ_STAT_OFFS                                        (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000140)
#define HWIO_SE_DMA_RX_IRQ_STAT_RMSK                                             0xfff
#define HWIO_SE_DMA_RX_IRQ_STAT_GENI_CANCEL_IRQ_BMSK                             0x800
#define HWIO_SE_DMA_RX_IRQ_STAT_GENI_CANCEL_IRQ_SHFT                               0xb
#define HWIO_SE_DMA_RX_IRQ_STAT_GENI_GP_IRQ_BMSK                                 0x7e0
#define HWIO_SE_DMA_RX_IRQ_STAT_GENI_GP_IRQ_SHFT                                   0x5
#define HWIO_SE_DMA_RX_IRQ_STAT_FLUSH_DONE_BMSK                                   0x10
#define HWIO_SE_DMA_RX_IRQ_STAT_FLUSH_DONE_SHFT                                    0x4
#define HWIO_SE_DMA_RX_IRQ_STAT_RESET_DONE_BMSK                                    0x8
#define HWIO_SE_DMA_RX_IRQ_STAT_RESET_DONE_SHFT                                    0x3
#define HWIO_SE_DMA_RX_IRQ_STAT_SBE_BMSK                                           0x4
#define HWIO_SE_DMA_RX_IRQ_STAT_SBE_SHFT                                           0x2
#define HWIO_SE_DMA_RX_IRQ_STAT_EOT_BMSK                                           0x2
#define HWIO_SE_DMA_RX_IRQ_STAT_EOT_SHFT                                           0x1
#define HWIO_SE_DMA_RX_IRQ_STAT_DMA_DONE_BMSK                                      0x1
#define HWIO_SE_DMA_RX_IRQ_STAT_DMA_DONE_SHFT                                      0x0

#define HWIO_SE_DMA_RX_IRQ_CLR_OFFS                                         (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000144)
#define HWIO_SE_DMA_RX_IRQ_CLR_RMSK                                              0xfff
#define HWIO_SE_DMA_RX_IRQ_CLR_GENI_CANCEL_IRQ_CLR_BMSK                          0x800
#define HWIO_SE_DMA_RX_IRQ_CLR_GENI_CANCEL_IRQ_CLR_SHFT                            0xb
#define HWIO_SE_DMA_RX_IRQ_CLR_GENI_GP_IRQ_CLR_BMSK                              0x7e0
#define HWIO_SE_DMA_RX_IRQ_CLR_GENI_GP_IRQ_CLR_SHFT                                0x5
#define HWIO_SE_DMA_RX_IRQ_CLR_FLUSH_DONE_CLR_BMSK                                0x10
#define HWIO_SE_DMA_RX_IRQ_CLR_FLUSH_DONE_CLR_SHFT                                 0x4
#define HWIO_SE_DMA_RX_IRQ_CLR_RESET_DONE_CLR_BMSK                                 0x8
#define HWIO_SE_DMA_RX_IRQ_CLR_RESET_DONE_CLR_SHFT                                 0x3
#define HWIO_SE_DMA_RX_IRQ_CLR_SBE_CLR_BMSK                                        0x4
#define HWIO_SE_DMA_RX_IRQ_CLR_SBE_CLR_SHFT                                        0x2
#define HWIO_SE_DMA_RX_IRQ_CLR_EOT_CLR_BMSK                                        0x2
#define HWIO_SE_DMA_RX_IRQ_CLR_EOT_CLR_SHFT                                        0x1
#define HWIO_SE_DMA_RX_IRQ_CLR_DMA_DONE_CLR_BMSK                                   0x1
#define HWIO_SE_DMA_RX_IRQ_CLR_DMA_DONE_CLR_SHFT                                   0x0

#define HWIO_SE_DMA_RX_IRQ_EN_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000148)
#define HWIO_SE_DMA_RX_IRQ_EN_RMSK                                               0xfff
#define HWIO_SE_DMA_RX_IRQ_EN_GENI_CANCEL_IRQ_EN_BMSK                            0x800
#define HWIO_SE_DMA_RX_IRQ_EN_GENI_CANCEL_IRQ_EN_SHFT                              0xb
#define HWIO_SE_DMA_RX_IRQ_EN_GENI_GP_IRQ_EN_BMSK                                0x7e0
#define HWIO_SE_DMA_RX_IRQ_EN_GENI_GP_IRQ_EN_SHFT                                  0x5
#define HWIO_SE_DMA_RX_IRQ_EN_FLUSH_DONE_EN_BMSK                                  0x10
#define HWIO_SE_DMA_RX_IRQ_EN_FLUSH_DONE_EN_SHFT                                   0x4
#define HWIO_SE_DMA_RX_IRQ_EN_RESET_DONE_EN_BMSK                                   0x8
#define HWIO_SE_DMA_RX_IRQ_EN_RESET_DONE_EN_SHFT                                   0x3
#define HWIO_SE_DMA_RX_IRQ_EN_SBE_EN_BMSK                                          0x4
#define HWIO_SE_DMA_RX_IRQ_EN_SBE_EN_SHFT                                          0x2
#define HWIO_SE_DMA_RX_IRQ_EN_EOT_EN_BMSK                                          0x2
#define HWIO_SE_DMA_RX_IRQ_EN_EOT_EN_SHFT                                          0x1
#define HWIO_SE_DMA_RX_IRQ_EN_DMA_DONE_EN_BMSK                                     0x1
#define HWIO_SE_DMA_RX_IRQ_EN_DMA_DONE_EN_SHFT                                     0x0

#define HWIO_SE_DMA_RX_IRQ_EN_SET_OFFS                                      (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000014c)
#define HWIO_SE_DMA_RX_IRQ_EN_SET_RMSK                                           0xfff
#define HWIO_SE_DMA_RX_IRQ_EN_SET_GENI_CANCEL_IRQ_EN_SET_BMSK                    0x800
#define HWIO_SE_DMA_RX_IRQ_EN_SET_GENI_CANCEL_IRQ_EN_SET_SHFT                      0xb
#define HWIO_SE_DMA_RX_IRQ_EN_SET_GENI_GP_IRQ_EN_SET_BMSK                        0x7e0
#define HWIO_SE_DMA_RX_IRQ_EN_SET_GENI_GP_IRQ_EN_SET_SHFT                          0x5
#define HWIO_SE_DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_BMSK                          0x10
#define HWIO_SE_DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_SHFT                           0x4
#define HWIO_SE_DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK                           0x8
#define HWIO_SE_DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_SHFT                           0x3
#define HWIO_SE_DMA_RX_IRQ_EN_SET_SBE_EN_SET_BMSK                                  0x4
#define HWIO_SE_DMA_RX_IRQ_EN_SET_SBE_EN_SET_SHFT                                  0x2
#define HWIO_SE_DMA_RX_IRQ_EN_SET_EOT_EN_SET_BMSK                                  0x2
#define HWIO_SE_DMA_RX_IRQ_EN_SET_EOT_EN_SET_SHFT                                  0x1
#define HWIO_SE_DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK                             0x1
#define HWIO_SE_DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_SHFT                             0x0

#define HWIO_SE_DMA_RX_IRQ_EN_CLR_OFFS                                      (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000150)
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_RMSK                                           0xfff
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_GENI_CANCEL_IRQ_EN_CLR_BMSK                    0x800
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_GENI_CANCEL_IRQ_EN_CLR_SHFT                      0xb
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_GENI_GP_IRQ_EN_CLR_BMSK                        0x7e0
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_GENI_GP_IRQ_EN_CLR_SHFT                          0x5
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_FLUSH_DONE_EN_CLR_BMSK                          0x10
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_FLUSH_DONE_EN_CLR_SHFT                           0x4
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_RESET_DONE_EN_CLR_BMSK                           0x8
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_RESET_DONE_EN_CLR_SHFT                           0x3
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_SBE_EN_CLR_BMSK                                  0x4
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_SBE_EN_CLR_SHFT                                  0x2
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_EOT_EN_CLR_BMSK                                  0x2
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_EOT_EN_CLR_SHFT                                  0x1
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_DMA_DONE_EN_CLR_BMSK                             0x1
#define HWIO_SE_DMA_RX_IRQ_EN_CLR_DMA_DONE_EN_CLR_SHFT                             0x0

#define HWIO_SE_DMA_RX_LENGTH_IN_OFFS                                       (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000154)
#define HWIO_SE_DMA_RX_LENGTH_IN_RMSK                                         0xffffff
#define HWIO_SE_DMA_RX_LENGTH_IN_RX_LENGTH_IN_BMSK                            0xffffff
#define HWIO_SE_DMA_RX_LENGTH_IN_RX_LENGTH_IN_SHFT                                 0x0

#define HWIO_SE_DMA_RX_FSM_RST_OFFS                                         (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000158)
#define HWIO_SE_DMA_RX_FSM_RST_RMSK                                                0x1
#define HWIO_SE_DMA_RX_FSM_RST_RX_FSM_RST_BMSK                                     0x1
#define HWIO_SE_DMA_RX_FSM_RST_RX_FSM_RST_SHFT                                     0x0

#define HWIO_SE_DMA_RX_MAX_BURST_SIZE_OFFS                                  (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000015c)
#define HWIO_SE_DMA_RX_MAX_BURST_SIZE_RMSK                                         0x3
#define HWIO_SE_DMA_RX_MAX_BURST_SIZE_RX_MAX_BURST_SIZE_BMSK                       0x3
#define HWIO_SE_DMA_RX_MAX_BURST_SIZE_RX_MAX_BURST_SIZE_SHFT                       0x0

#define HWIO_SE_DMA_RX_FLUSH_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000160)
#define HWIO_SE_DMA_RX_FLUSH_RMSK                                                  0x1
#define HWIO_SE_DMA_RX_FLUSH_RX_FLUSH_BMSK                                         0x1
#define HWIO_SE_DMA_RX_FLUSH_RX_FLUSH_SHFT                                         0x0

#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_OFFS                                   (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000214)
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_RMSK                                          0xf
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_GENI_S_IRQ_HIGH_PRIORITY_BMSK                 0x8
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_GENI_S_IRQ_HIGH_PRIORITY_SHFT                 0x3
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_GENI_M_IRQ_HIGH_PRIORITY_BMSK                 0x4
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_GENI_M_IRQ_HIGH_PRIORITY_SHFT                 0x2
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_RX_IRQ_HIGH_PRIORITY_BMSK                     0x2
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_RX_IRQ_HIGH_PRIORITY_SHFT                     0x1
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_TX_IRQ_HIGH_PRIORITY_BMSK                     0x1
#define HWIO_SE_SE_IRQ_HIGH_PRIORITY_TX_IRQ_HIGH_PRIORITY_SHFT                     0x0

#define HWIO_SE_SE_GSI_EVENT_EN_OFFS                                        (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000218)
#define HWIO_SE_SE_GSI_EVENT_EN_RMSK                                               0xf
#define HWIO_SE_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_BMSK                               0x8
#define HWIO_SE_SE_GSI_EVENT_EN_GENI_S_EVENT_EN_SHFT                               0x3
#define HWIO_SE_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_BMSK                               0x4
#define HWIO_SE_SE_GSI_EVENT_EN_GENI_M_EVENT_EN_SHFT                               0x2
#define HWIO_SE_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_BMSK                               0x2
#define HWIO_SE_SE_GSI_EVENT_EN_DMA_TX_EVENT_EN_SHFT                               0x1
#define HWIO_SE_SE_GSI_EVENT_EN_DMA_RX_EVENT_EN_BMSK                               0x1
#define HWIO_SE_SE_GSI_EVENT_EN_DMA_RX_EVENT_EN_SHFT                               0x0

#define HWIO_SE_SE_IRQ_EN_OFFS                                              (SE_QUPV3_SE_DMA_REG_BASE      + 0x0000021c)
#define HWIO_SE_SE_IRQ_EN_RMSK                                                     0xf
#define HWIO_SE_SE_IRQ_EN_GENI_S_IRQ_EN_BMSK                                       0x8
#define HWIO_SE_SE_IRQ_EN_GENI_S_IRQ_EN_SHFT                                       0x3
#define HWIO_SE_SE_IRQ_EN_GENI_M_IRQ_EN_BMSK                                       0x4
#define HWIO_SE_SE_IRQ_EN_GENI_M_IRQ_EN_SHFT                                       0x2
#define HWIO_SE_SE_IRQ_EN_DMA_TX_IRQ_EN_BMSK                                       0x2
#define HWIO_SE_SE_IRQ_EN_DMA_TX_IRQ_EN_SHFT                                       0x1
#define HWIO_SE_SE_IRQ_EN_DMA_RX_IRQ_EN_BMSK                                       0x1
#define HWIO_SE_SE_IRQ_EN_DMA_RX_IRQ_EN_SHFT                                       0x0

#define HWIO_SE_DMA_IF_EN_RO_OFFS                                           (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000220)
#define HWIO_SE_DMA_IF_EN_RO_RMSK                                                  0x1
#define HWIO_SE_DMA_IF_EN_RO_DMA_IF_EN_BMSK                                        0x1
#define HWIO_SE_DMA_IF_EN_RO_DMA_IF_EN_SHFT                                        0x0

#define HWIO_SE_SE_HW_PARAM_0_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000224)
#define HWIO_SE_SE_HW_PARAM_0_RMSK                                          0x3f3f79ff
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_WIDTH_BMSK                            0x3f000000
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_WIDTH_SHFT                                  0x18
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK                              0x3f0000
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT                                  0x10
#define HWIO_SE_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_BMSK                          0x7000
#define HWIO_SE_SE_HW_PARAM_0_TX_ASYNC_FIFO_DEPTH_SHFT                             0xc
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_EN_BMSK                                    0x800
#define HWIO_SE_SE_HW_PARAM_0_TX_FIFO_EN_SHFT                                      0xb
#define HWIO_SE_SE_HW_PARAM_0_GEN_I3C_BMSK                                       0x100
#define HWIO_SE_SE_HW_PARAM_0_GEN_I3C_SHFT                                         0x8
#define HWIO_SE_SE_HW_PARAM_0_GEN_PROG_ROM_BMSK                                   0x80
#define HWIO_SE_SE_HW_PARAM_0_GEN_PROG_ROM_SHFT                                    0x7
#define HWIO_SE_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_BMSK                             0x40
#define HWIO_SE_SE_HW_PARAM_0_GEN_SCND_SEQUENCER_SHFT                              0x6
#define HWIO_SE_SE_HW_PARAM_0_AHB_M_OFFS_W_BMSK                                   0x3f
#define HWIO_SE_SE_HW_PARAM_0_AHB_M_OFFS_W_SHFT                                    0x0

#define HWIO_SE_SE_HW_PARAM_1_OFFS                                          (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000228)
#define HWIO_SE_SE_HW_PARAM_1_RMSK                                          0x3f3f7fff
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_WIDTH_BMSK                            0x3f000000
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_WIDTH_SHFT                                  0x18
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK                              0x3f0000
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT                                  0x10
#define HWIO_SE_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_BMSK                          0x7000
#define HWIO_SE_SE_HW_PARAM_1_RX_ASYNC_FIFO_DEPTH_SHFT                             0xc
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_EN_BMSK                                    0x800
#define HWIO_SE_SE_HW_PARAM_1_RX_FIFO_EN_SHFT                                      0xb
#define HWIO_SE_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_BMSK                             0x600
#define HWIO_SE_SE_HW_PARAM_1_PROG_RAM_MEM_TYPE_SHFT                               0x9
#define HWIO_SE_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_BMSK                          0x1ff
#define HWIO_SE_SE_HW_PARAM_1_PROG_REG_ARRAY_DEPTH_SHFT                            0x0

#define HWIO_SE_DMA_GENERAL_CFG_OFFS                                        (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000230)
#define HWIO_SE_DMA_GENERAL_CFG_RMSK                                             0x1ff
#define HWIO_SE_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_BMSK                            0x1c0
#define HWIO_SE_DMA_GENERAL_CFG_RX_DMA_IRQ_DELAY_SHFT                              0x6
#define HWIO_SE_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_BMSK                       0x20
#define HWIO_SE_DMA_GENERAL_CFG_TX_DMA_ZERO_PADDING_EN_SHFT                        0x5
#define HWIO_SE_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK                        0x8
#define HWIO_SE_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_SHFT                        0x3
#define HWIO_SE_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK                        0x4
#define HWIO_SE_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_SHFT                        0x2
#define HWIO_SE_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK                             0x2
#define HWIO_SE_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_SHFT                             0x1
#define HWIO_SE_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK                             0x1
#define HWIO_SE_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_SHFT                             0x0

#define HWIO_SE_DMA_DEBUG_REG0_OFFS                                         (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000240)
#define HWIO_SE_DMA_DEBUG_REG0_RMSK                                              0xff3
#define HWIO_SE_DMA_DEBUG_REG0_DMA_RX_STATE_BMSK                                 0xf00
#define HWIO_SE_DMA_DEBUG_REG0_DMA_RX_STATE_SHFT                                   0x8
#define HWIO_SE_DMA_DEBUG_REG0_DMA_TX_STATE_BMSK                                  0xf0
#define HWIO_SE_DMA_DEBUG_REG0_DMA_TX_STATE_SHFT                                   0x4
#define HWIO_SE_DMA_DEBUG_REG0_DMA_RX_ACTIVE_BMSK                                  0x2
#define HWIO_SE_DMA_DEBUG_REG0_DMA_RX_ACTIVE_SHFT                                  0x1
#define HWIO_SE_DMA_DEBUG_REG0_DMA_TX_ACTIVE_BMSK                                  0x1
#define HWIO_SE_DMA_DEBUG_REG0_DMA_TX_ACTIVE_SHFT                                  0x0

#define HWIO_SE_DMA_TEST_BUS_CTRL_OFFS                                      (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000244)
#define HWIO_SE_DMA_TEST_BUS_CTRL_RMSK                                             0x7
#define HWIO_SE_DMA_TEST_BUS_CTRL_DMA_TEST_BUS_SEL_BMSK                            0x7
#define HWIO_SE_DMA_TEST_BUS_CTRL_DMA_TEST_BUS_SEL_SHFT                            0x0

#define HWIO_SE_SE_TOP_TEST_BUS_CTRL_OFFS                                   (SE_QUPV3_SE_DMA_REG_BASE      + 0x00000248)
#define HWIO_SE_SE_TOP_TEST_BUS_CTRL_RMSK                                          0x3
#define HWIO_SE_SE_TOP_TEST_BUS_CTRL_SE_TOP_TEST_BUS_SEL_BMSK                      0x3
#define HWIO_SE_SE_TOP_TEST_BUS_CTRL_SE_TOP_TEST_BUS_SEL_SHFT                      0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_QUPV3_SEC
 *--------------------------------------------------------------------------*/

#define SE_QUPV3_SEC_REG_BASE                                                   (SE_GENI4_CFG_REG_BASE      + 0x00002000)

#define HWIO_SE_GENI_CLK_CTRL_OFFS                                              (SE_QUPV3_SEC_REG_BASE      + 0x00000000)
#define HWIO_SE_GENI_CLK_CTRL_RMSK                                                     0x1
#define HWIO_SE_GENI_CLK_CTRL_SER_CLK_SEL_BMSK                                         0x1
#define HWIO_SE_GENI_CLK_CTRL_SER_CLK_SEL_SHFT                                         0x0

#define HWIO_SE_DMA_IF_EN_OFFS                                                  (SE_QUPV3_SEC_REG_BASE      + 0x00000004)
#define HWIO_SE_DMA_IF_EN_RMSK                                                         0x1
#define HWIO_SE_DMA_IF_EN_DMA_IF_EN_BMSK                                               0x1
#define HWIO_SE_DMA_IF_EN_DMA_IF_EN_SHFT                                               0x0

#define HWIO_SE_FIFO_IF_DISABLE_OFFS                                            (SE_QUPV3_SEC_REG_BASE      + 0x00000008)
#define HWIO_SE_FIFO_IF_DISABLE_RMSK                                                   0x1
#define HWIO_SE_FIFO_IF_DISABLE_FIFO_IF_DISABLE_BMSK                                   0x1
#define HWIO_SE_FIFO_IF_DISABLE_FIFO_IF_DISABLE_SHFT                                   0x0

#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_OFFS                                   (SE_QUPV3_SEC_REG_BASE      + 0x0000000c)
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_RMSK                                          0x1
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_GENI_FW_MULTILOCK_PROTNS_BMSK                 0x1
#define HWIO_SE_GENI_FW_MULTILOCK_PROTNS_GENI_FW_MULTILOCK_PROTNS_SHFT                 0x0

#define HWIO_SE_GENI_FW_MULTILOCK_MSA_OFFS                                      (SE_QUPV3_SEC_REG_BASE      + 0x00000010)
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_RMSK                                             0x1
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_GENI_FW_MULTILOCK_MSA_BMSK                       0x1
#define HWIO_SE_GENI_FW_MULTILOCK_MSA_GENI_FW_MULTILOCK_MSA_SHFT                       0x0

#define HWIO_SE_GENI_FW_MULTILOCK_SP_OFFS                                       (SE_QUPV3_SEC_REG_BASE      + 0x00000014)
#define HWIO_SE_GENI_FW_MULTILOCK_SP_RMSK                                              0x1
#define HWIO_SE_GENI_FW_MULTILOCK_SP_GENI_FW_MULTILOCK_SP_BMSK                         0x1
#define HWIO_SE_GENI_FW_MULTILOCK_SP_GENI_FW_MULTILOCK_SP_SHFT                         0x0

/*----------------------------------------------------------------------------
 * MODULE: SE_QUPV3_SEC
 *--------------------------------------------------------------------------*/
#define HWIO_QUPV3_SE_AHB_M_CFG_OFFS                                            (SE_GENI4_CFG_REG_BASE + 0x2C000 + 0x00000118)
#define HWIO_QUPV3_SE_AHB_M_CFG_RMSK                                                   0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK                                  0x1
#define HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_SHFT                                  0x0

#endif /* __HW_REGS_H__ */
