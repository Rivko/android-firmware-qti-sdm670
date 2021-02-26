#ifndef UART_HAL_H
#define UART_HAL_H
/*==================================================================================================

FILE: Uart_hal.h

DESCRIPTION: Defines the HAL layer definitions and functions for the UART driver.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart_lite/inc/uart_hal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included Get SNAP, Enable interrupt and get MISR APIs.
08/28/13   VV      Changing header Uart_comdefs.h to Uart_ComDefs.h
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "HALhwio.h"
#include "HALcomdef.h"
#include "uart_comdefs.h"
#include "uart_hwio.h"

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


//QUPV3_SE
#define AHB_M_CLK_CGC_ON HWIO_QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK

//DMA
#define AHB_SEC_SLV_CLK_CGC_ON_MASK      HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK
#define AHB_SEC_SLV_CLK_CGC_ON_SHIFT     HWIO_DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_SHFT

#define DMA_AHB_SLV_CLK_CGC_ON_MASK      HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK
#define DMA_AHB_SLV_CLK_CGC_ON_SHIFT     HWIO_DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_SHFT

#define DMA_TX_CLK_CGC_ON_MASK           HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK
#define DMA_TX_CLK_CGC_ON_SHIFT          HWIO_DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_SHFT

#define DMA_RX_CLK_CGC_ON_MASK           HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK
#define DMA_RX_CLK_CGC_ON_SHIFT          HWIO_DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_SHFT

//SE

#define GENI_M_IRQ_EN_MASK      HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_BMSK
#define GENI_M_IRQ_EN_SHIFT     HWIO_SE_IRQ_EN_GENI_M_IRQ_EN_SHFT
#define GENI_S_IRQ_EN_MASK      HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_BMSK
#define GENI_S_IRQ_EN_SHIFT     HWIO_SE_IRQ_EN_GENI_S_IRQ_EN_SHFT

#define TX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_BMSK
#define TX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_0_TX_FIFO_DEPTH_SHFT

#define RX_FIFO_DEPTH_MASK      HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_BMSK
#define RX_FIFO_DEPTH_SHIFT     HWIO_SE_HW_PARAM_1_RX_FIFO_DEPTH_SHFT


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
#define S_CMD_CANCEL_IRQ            HWIO_GENI_S_IRQ_STATUS_S_CMD_CANCEL_BMSK
#define S_CMD_DONE_IRQ              HWIO_GENI_S_IRQ_STATUS_S_CMD_DONE_BMSK
#define S_GP_SYNC_IRQ               HWIO_GENI_S_IRQ_STATUS_S_GP_SYNC_IRQ_0_BMSK
#define RX_BREAK_START_IRQ          HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_2_BMSK
#define RX_BREAK_END_IRQ            HWIO_GENI_S_IRQ_STATUS_S_GP_IRQ_3_BMSK
#endif

