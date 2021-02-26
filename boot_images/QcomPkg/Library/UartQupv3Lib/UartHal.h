#ifndef UART_HAL_H
#define UART_HAL_H
/*==================================================================================================

FILE: UartHal.h

DESCRIPTION: Defines the HAL layer definitions and functions for the UART driver.

Copyright (c) 2016 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartHal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/7/16    VV       Initial revision

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "HALhwio.h"
#include "UartHwio.h"

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#define REG_IN(base, hwiosym) HWIO_##hwiosym##_IN(base)

#define REG_OUT(base, hwiosym, value) HWIO_##hwiosym##_OUT(base, value)

#define REG_INI(base, hwiosym, n) HWIO_##hwiosym##_INI(base, n)

#define REG_OUTI(base, hwiosym, n, value) HWIO_##hwiosym##_OUTI(base, n, value)

/*==================================================================================================
                                               CONSTANTS
==================================================================================================*/
// Different base addresses offset
#define GENI4_CFG           0x0
#define GENI4_IMAGE_REGS    0x100
#define GENI4_DATA          0x600
#define QUPV3_SE_DMA        0xC00
#define GENI4_IMAGE         0x1000
#define QUPV3_SEC           0x2000


//SE

#define GENI_M_IRQ_EN_MASK      HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_BMSK
#define GENI_M_IRQ_EN_SHIFT     HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_SHFT
#define GENI_S_IRQ_EN_MASK      HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_BMSK
#define GENI_S_IRQ_EN_SHIFT     HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_SHFT

#define TX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK
#define TX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT

#define RX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK
#define RX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT

#define M_GENI_CMD_ACTIVE       HWIO_GENI_STATUS_M_GENI_CMD_ACTIVE_BMSK
#define S_GENI_CMD_ACTIVE       HWIO_GENI_STATUS_S_GENI_CMD_ACTIVE_BMSK

//FIFO
#define TX_FIFO_WC                   HWIO_GENI_TX_FIFO_STATUS_TX_FIFO_WC_BMSK

#define RX_LAST_VALID_BYTES_MASK     HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_BMSK
#define RX_LAST_VALID_BYTES_SHIFT    HWIO_GENI_RX_FIFO_STATUS_RX_LAST_BYTE_VALID_SHFT
#define RX_FIFO_WC                   HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_BMSK
#define RX_FIFO_WC_SHIFT             HWIO_GENI_RX_FIFO_STATUS_RX_FIFO_WC_SHFT

// IRQ
#define TF_FIFO_WATERMARK_IRQ       HWIO_GENI_M_IRQ_STATUS_TX_FIFO_WATERMARK_BMSK
#define M_CMD_DONE_IRQ              HWIO_GENI_M_IRQ_STATUS_M_CMD_DONE_BMSK
#define SEC_IRQ                     HWIO_GENI_M_IRQ_STATUS_SEC_IRQ_BMSK

#define RX_LAST_IRQ                 HWIO_GENI_S_IRQ_STATUS_RX_FIFO_LAST_BMSK
#define RX_FIFO_WATERMARK_IRQ       HWIO_GENI_S_IRQ_STATUS_RX_FIFO_WATERMARK_BMSK
#define RX_BREAK_START              HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_BMSK
#define RX_BREAK_END                HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_BMSK

#endif

