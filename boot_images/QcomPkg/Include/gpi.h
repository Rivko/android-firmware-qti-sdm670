#ifndef __GPI_H__
#define __GPI_H__

/**
  @file  gpi.h
  @brief GPI public data structures and APIs.

*/
/*
===============================================================================

Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/14/17   ah      Introduced user_data to non-transfer results, increased ring
04/11/17   ah      Add I3C definitions
04/11/17   ts      Add firmware load API
03/13/17   ah      Add GPII de-reg API
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "comdef.h"

#ifdef GPI_ST
#include "rex.h"
#include "cortex-m3.h"
#endif

#ifdef GPI_DALSYS
#include "DALStdDef.h"
#endif

/* *********************************************************************************************** */
/*                                         DEFINITIONS                                             */
/* *********************************************************************************************** */

typedef unsigned long  U_LONG;

#define MAX_NUM_GPI_RING_ELEM                         16

#define RESERVED_VALUE                                0xFFFF
#define GPI_PROTOCOL_TYPE                             0x2

#ifndef PACKED
#define PACKED __packed
#endif

#define GPI_CHAIN__MASK                               0x1
#define GPI_CHAIN__SHIFT                              0x0

#define GPI_IEOB__MASK                                0x100
#define GPI_IEOB__SHIFT                               0x8

#define GPI_IEOT__MASK                                0x200
#define GPI_IEOT__SHIFT                               0x9

#define GPI_BEI__MASK                                 0x400
#define GPI_BEI__SHIFT                                0xA

#define GPI_MAJOR__MASK                               0xF00000
#define GPI_MAJOR__SHIFT                              0x14
#define TRE_INVALID_MAJOR                             0x0
#define TRE_NOOP_MAJOR                                0x0
#define TRE_DMA_W_BUFFER_MAJOR                        0x1
#define TRE_DMA_W_SGL_MAJOR                           0x1
#define TRE_DMA_IMM_MAJOR                             0x1
#define TRE_GO_MAJOR                                  0x2
#define TRE_CONFIG_0_MAJOR                            0x2
#define TRE_CONFIG_1_MAJOR                            0x2
#define TRE_CONFIG_2_MAJOR                            0x2
#define TRE_CONFIG_3_MAJOR                            0x2
#define TRE_LOCK_MAJOR                                0x3
#define TRE_UNLOCK_MAJOR                              0x3

#define GPI_MINOR__MASK                               0xF0000
#define GPI_MINOR__SHIFT                              0x10
#define TRE_INVALID_MINOR                             0x0
#define TRE_NOOP_MINOR                                0x1
#define TRE_DMA_W_BUFFER_MINOR                        0x0
#define TRE_DMA_IMM_MINOR                             0x1
#define TRE_DMA_W_SGL_MINOR                           0x2
#define TRE_GO_MINOR                                  0x0
#define TRE_CONFIG_0_MINOR                            0x2
#define TRE_CONFIG_1_MINOR                            0x3
#define TRE_CONFIG_2_MINOR                            0x4
#define TRE_CONFIG_3_MINOR                            0x5
#define TRE_LOCK_MINOR                                0x0
#define TRE_UNLOCK_MINOR                              0x1

/* DMA TREs */
#define GPI_TRE_DMA_W_BUFFER_PTR__MASK                0xFFFFFFFF
#define GPI_TRE_DMA_W_BUFFER_PTR__SHIFT               0x0
#define GPI_TRE_DMA_W_BUFFER_LENGTH__MASK             0xFFFFFF
#define GPI_TRE_DMA_W_BUFFER_LENGTH__SHIFT            0x0

#define GPI_TRE_DMA_W_SGL_PTR__MASK                   0xFFFFFFFF
#define GPI_TRE_DMA_W_SGL_PTR__SHIFT                  0x0
#define GPI_TRE_DMA_W_SGL_LENGTH__MASK                0xFFFF
#define GPI_TRE_DMA_W_SGL_LENGTH__SHIFT               0x0

#define GPI_TRE_DMA_IMM_DATA__MASK                    0xFFFFFFFF
#define GPI_TRE_DMA_IMM_DATA__SHIFT                   0x0
#define GPI_TRE_DMA_IMM_LENGTH__MASK                  0xF
#define GPI_TRE_DMA_IMM_LENGTH__SHIFT                 0x0

/* SPI Configuration TREs */
#define GPI_TRE_SPI_GO_CMD__MASK                      0x1F
#define GPI_TRE_SPI_GO_CMD__SHIFT                     0x0
#define GPI_TRE_SPI_GO_CS__MASK                       0x700
#define GPI_TRE_SPI_GO_CS__SHIFT                      0x8
#define GPI_TRE_SPI_GO_FLAGS__MASK                    0xFF000000
#define GPI_TRE_SPI_GO_FLAGS__SHIFT                   0x18
#define GPI_TRE_SPI_GO_FLAGS_PRE_CMD_DELAY__SHIFT     0x0
#define GPI_TRE_SPI_GO_FLAGS_TSTAMP_BEFORE__SHIFT     0x1
#define GPI_TRE_SPI_GO_FLAGS_FRAG__SHIFT              0x2
#define GPI_TRE_SPI_GO_FLAGS_TSTAMP_AFTER__MASK       0x3
#define GPI_TRE_SPI_GO_FLAGS_POST_CMD_DELAY__MASK     0x4
#define GPI_TRE_SPI_GO_RX_LENGTH__MASK                0xFFFFFF
#define GPI_TRE_SPI_GO_RX_LENGTH__SHIFT               0x0

#define GPI_TRE_SPI_CONFIG_0_WORD_SZ__MASK            0x1F
#define GPI_TRE_SPI_CONFIG_0_WORD_SZ__SHIFT           0x0
#define GPI_TRE_SPI_CONFIG_0_FLAGS__MASK              0x3F00
#define GPI_TRE_SPI_CONFIG_0_FLAGS__SHIFT             0x8
#define GPI_TRE_SPI_CONFIG_0_FLAGS_LOOPBACK__SHIFT    0x0
#define GPI_TRE_SPI_CONFIG_0_FLAGS_HS__SHIFT          0x1
#define GPI_TRE_SPI_CONFIG_0_FLAGS_CS_POLARITY__SHIFT 0x2
#define GPI_TRE_SPI_CONFIG_0_FLAGS_CS_TOGGLE__SHIFT   0x3
#define GPI_TRE_SPI_CONFIG_0_FLAGS_CPHA__SHIFT        0x4
#define GPI_TRE_SPI_CONFIG_0_FLAGS_CPOL__SHIFT        0x5
#define GPI_TRE_SPI_CONFIG_0_PACKING__MASK            0xFF000000
#define GPI_TRE_SPI_CONFIG_0_PACKING__SHIFT           0x18

#define GPI_TRE_SPI_CONFIG_0_INTER_WORD_DELAY__MASK   0xFF
#define GPI_TRE_SPI_CONFIG_0_INTER_WORD_DELAY__SHIFT  0x0
#define GPI_TRE_SPI_CONFIG_0_CS_CLK_DELAY__MASK       0x1F00
#define GPI_TRE_SPI_CONFIG_0_CS_CLK_DELAY__SHIFT      0x8
#define GPI_TRE_SPI_CONFIG_0_INTER_TRANSF_DELAY__MASK 0xFFFF0000
#define GPI_TRE_SPI_CONFIG_0_INTER_TRANSF_DELAY__SHIFT 0x10

#define GPI_TRE_SPI_CONFIG_0_CLCK_DIV__MASK            0xFFF
#define GPI_TRE_SPI_CONFIG_0_CLCK_DIV__SHIFT           0x0
#define GPI_TRE_SPI_CONFIG_0_CLCK_SRC__MASK            0xF0000
#define GPI_TRE_SPI_CONFIG_0_CLCK_SRC__SHIFT           0x10

/* UART Configuration TREs */
#define GPI_TRE_UART_GO_CMD__MASK                      0x1F
#define GPI_TRE_UART_GO_CMD__SHIFT                     0x0

#define GPI_TRE_UART_CONFIG_0_CH_SIZE__MASK            0x7
#define GPI_TRE_UART_CONFIG_0_CH_SIZE__SHIFT           0x0
#define GPI_TRE_UART_CONFIG_0_STOP__MASK               0x18
#define GPI_TRE_UART_CONFIG_0_STOP__SHIFT              0x3
#define GPI_TRE_UART_CONFIG_0_PARITY__MASK             0xE0
#define GPI_TRE_UART_CONFIG_0_PARITY__SHIFT            0x5
#define GPI_TRE_UART_CONFIG_0_FLAGS__MASK              0x300
#define GPI_TRE_UART_CONFIG_0_FLAGS__SHIFT             0x8
#define GPI_TRE_UART_CONFIG_0_PACKING__MASK            0xFF000000
#define GPI_TRE_UART_CONFIG_0_PACKING__SHIFT           0x18
#define GPI_TRE_UART_CONFIG_0_RX_STALE_TO__MASK        0xFFFFF
#define GPI_TRE_UART_CONFIG_0_RX_STALE_TO__SHIFT       0x0
#define GPI_TRE_UART_CONFIG_0_CLK_DIV__MASK            0xFFF
#define GPI_TRE_UART_CONFIG_0_CLK_DIV__SHIFT           0x0

/* I2C Configuration TREs */
#define GPI_TRE_I2C_GO_CMD__MASK                       0x1F
#define GPI_TRE_I2C_GO_CMD__SHIFT                      0x0
#define GPI_TRE_I2C_GO_SLAVE_ID__MASK                  0x7F00
#define GPI_TRE_I2C_GO_SLAVE_ID__SHIFT                 0x8
#define GPI_TRE_I2C_GO_FLAGS__MASK                     0xFF000000
#define GPI_TRE_I2C_GO_FLAGS__SHIFT                    0x18
#define GPI_TRE_I2C_GO_FLAGS_PRE_CMD_DELAY__SHIFT      0x0
#define GPI_TRE_I2C_GO_FLAGS_TIMESTAMP_BEFORE__SHIFT   0x1
#define GPI_TRE_I2C_GO_FLAGS_STOP_STRETCH__SHIFT       0x2
#define GPI_TRE_I2C_GO_FLAGS_TIMESTAMP_AFTER__SHIFT    0x3
#define GPI_TRE_I2C_GO_FLAGS_POST_CMD_DELAY__SHIFT     0x4
#define GPI_TRE_I2C_GO_FLAGS_IGNORE_ADDR_NACK__SHIFT   0x5
#define GPI_TRE_I2C_GO_FLAGS_RX_ACK__SHIFT             0x6
#define GPI_TRE_I2C_GO_FLAGS_BYPASS_ADDR_PHASE__SHIFT  0x7
#define GPI_TRE_I2C_GO_RX_LENGTH__MASK                 0xFFFFFF
#define GPI_TRE_I2C_GO_RX_LENGTH__SHIFT                0x0

#define GPI_TRE_I2C_CONFIG_0_T_LOW__MASK               0xFF
#define GPI_TRE_I2C_CONFIG_0_T_LOW__SHIFT              0x0
#define GPI_TRE_I2C_CONFIG_0_T_HIGH__MASK              0xFF00
#define GPI_TRE_I2C_CONFIG_0_T_HIGH__SHIFT             0x8
#define GPI_TRE_I2C_CONFIG_0_T_CYCLE__MASK             0xFF0000
#define GPI_TRE_I2C_CONFIG_0_T_CYCLE__SHIFT            0x10
#define GPI_TRE_I2C_CONFIG_0_PACKING__MASK             0xFF000000
#define GPI_TRE_I2C_CONFIG_0_PACKING__SHIFT            0x18
#define GPI_TRE_I2C_CONFIG_0_NOISE_REJ_LEVEL__MASK     0xFF
#define GPI_TRE_I2C_CONFIG_0_NOISE_REJ_LEVEL__SHIFT    0x0
#define GPI_TRE_I2C_CONFIG_0_INTER_TRANSF_DELAY__MASK  0xFFFF0000
#define GPI_TRE_I2C_CONFIG_0_INTER_TRANSF_DELAY__SHIFT 0x10
#define GPI_TRE_I2C_CONFIG_0_CLCK_DIV__MASK            0xFFF
#define GPI_TRE_I2C_CONFIG_0_CLCK_DIV__SHIFT           0x0
#define GPI_TRE_I2C_CONFIG_0_CLCK_SRC__MASK            0xF0000
#define GPI_TRE_I2C_CONFIG_0_CLCK_SRC__SHIFT           0x10

/* I3C Configuration TREs */
#define GPI_TRE_I3C_GO_CMD__MASK                       0x1F
#define GPI_TRE_I3C_GO_CMD__SHIFT                      0x0
#define GPI_TRE_I3C_GO_SLAVE_ID__MASK                  0x7F00
#define GPI_TRE_I3C_GO_SLAVE_ID__SHIFT                 0x8
#define GPI_TRE_I3C_GO_CCC__MASK                       0xFF0000
#define GPI_TRE_I3C_GO_CCC__SHIFT                      0x10
#define GPI_TRE_I3C_GO_FLAGS__MASK                     0xFF000000
#define GPI_TRE_I3C_GO_FLAGS__SHIFT                    0x18
#define GPI_TRE_I3C_GO_FLAGS_PRE_CMD_DELAY__SHIFT      0x0
#define GPI_TRE_I3C_GO_FLAGS_TIMESTAMP_BEFORE__SHIFT   0x1
#define GPI_TRE_I3C_GO_FLAGS_STOP_STRETCH__SHIFT       0x2
#define GPI_TRE_I3C_GO_FLAGS_TIMESTAMP_AFTER__SHIFT    0x3
#define GPI_TRE_I3C_GO_FLAGS_POST_CMD_DELAY__SHIFT     0x4
#define GPI_TRE_I3C_GO_FLAGS_IGNORE_ADDR_NACK__SHIFT   0x5
#define GPI_TRE_I3C_GO_FLAGS_RX_ACK__SHIFT             0x6
#define GPI_TRE_I3C_GO_FLAGS_BYPASS_ADDR_PHASE__SHIFT  0x7

#define GPI_TRE_I3C_GO_I3C_FLAGS__MASK                 0xF
#define GPI_TRE_I3C_GO_I3C_FLAGS__SHIFT                0x0
#define GPI_TRE_I3C_GO_I3C_FLAGS_USE_7E__SHIFT         0x0
#define GPI_TRE_I3C_GO_I3C_FLAGS_IBI_CONTROL__SHIFT    0x1
#define GPI_TRE_I3C_GO_I3C_FLAGS_CONT_MODE__SHIFT      0x2
#define GPI_TRE_I3C_GO_RX_LENGTH__MASK                 0xFFFFFF
#define GPI_TRE_I3C_GO_RX_LENGTH__SHIFT                0x0

#define GPI_TRE_I3C_CONFIG_0_T_LOW__MASK               0xFF
#define GPI_TRE_I3C_CONFIG_0_T_LOW__SHIFT              0x0
#define GPI_TRE_I3C_CONFIG_0_T_HIGH__MASK              0xFF00
#define GPI_TRE_I3C_CONFIG_0_T_HIGH__SHIFT             0x8
#define GPI_TRE_I3C_CONFIG_0_T_CYCLE__MASK             0xFF0000
#define GPI_TRE_I3C_CONFIG_0_T_CYCLE__SHIFT            0x10
#define GPI_TRE_I3C_CONFIG_0_PACKING__MASK             0x3000000
#define GPI_TRE_I3C_CONFIG_0_PACKING__SHIFT            0x18

#define GPI_TRE_I3C_CONFIG_0_I3C_T_HIGH__MASK          0xFF
#define GPI_TRE_I3C_CONFIG_0_I3C_T_HIGH__SHIFT         0x0
#define GPI_TRE_I3C_CONFIG_0_I3C_T_CYCLE__MASK         0xFF00
#define GPI_TRE_I3C_CONFIG_0_I3C_T_CYCLE__SHIFT        0x8
#define GPI_TRE_I3C_CONFIG_0_CMD_DELAY__MASK           0xFFFF0000
#define GPI_TRE_I3C_CONFIG_0_CMD_DELAY__SHIFT          0x10

#define GPI_TRE_I3C_CONFIG_0_CLCK_DIV__MASK            0xFF
#define GPI_TRE_I3C_CONFIG_0_CLCK_DIV__SHIFT           0x0
#define GPI_TRE_I3C_CONFIG_0_CLCK_SRC__MASK            0xF0000
#define GPI_TRE_I3C_CONFIG_0_CLCK_SRC__SHIFT           0x10

/* Lock TREs */
#define GPI_TRE_LOCK_PROTOCOL__MASK                    0x1
#define GPI_TRE_LOCK_PROTOCOL__SHIFT                   0x0

/* Scatter/Gather List Entry */
#define GPI_SGL_ENTRY_BUF_PTR_1__MASK                  0xFFFFFFFF
#define GPI_SGL_ENTRY_BUF_PTR_1__SHIFT                 0x0
#define GPI_SGL_ENTRY_BUF_PTR_2__MASK                  0xFFFFFFFF
#define GPI_SGL_ENTRY_BUF_PTR_2__SHIFT                 0x0
#define GPI_SGL_ENTRY_BUF_LEN__MASK                    0xFFFFF
#define GPI_SGL_ENTRY_BUF_LEN__SHIFT                   0x0

typedef unsigned long ptr_type;
/* *********************************************************************************************** */
/*                                       DATA STRUCTURES                                           */
/* *********************************************************************************************** */

/* GSI HW Instance */
typedef enum
{
   QUP_0   = 0,
   QUP_1   = 1,
   QUP_SSC = 2
}QUP_BLOCK_TYPE;

/** @name GPI Channel Command Type */
typedef enum
{
   GPI_CHAN_CMD_ALLOCATE      = 0,   /* Request channel allocation    */
   GPI_CHAN_CMD_START         = 1,   /* Request channel start         */
   GPI_CHAN_CMD_STOP          = 2,   /* Request channel stop          */
   GPI_CHAN_CMD_RESET         = 9,   /* Request channel reset         */
   GPI_CHAN_CMD_DE_ALLOC      = 10,  /* Request channel de-allocation */
   GPI_CHAN_CMD_DB_STOP       = 11,  /* Request channel doorbell stop */

   /* Async commands */
   /* generic (starts from 0x10) */
   QUP_CMD_GSI_STOP_ON_EOT    = 16,  /* Stop channel when current transfer is finished              */

   /* protocol specific (starts from 0x20) */
   QUP_CMD_UART_SW_STALE      = 32,  /* Force UART RX channel flush all its data and close transfer */
   QUP_CMD_UART_RFR_READY     = 33,  /* Set RFR signal to "ready" state                             */
   QUP_CMD_UART_RFR_NOT_READY = 34,  /* Set RFR signal to "not ready" state                         */
   QUP_CMD_UART_BREAK_START   = 35,  /* Initiate RX-break condition on TX channel                   */
   QUP_CMD_UART_BREAK_STOP    = 36,  /* Finish RX-break condition on TX channel                     */

   GPI_CHAN_CMD_NONE          = 0xFF
}GPI_CHAN_CMD;


/** @name GPI Channel State */
typedef enum
{
   GPI_CHAN_DISABLED      = 0,
   GPI_CHAN_ALLOCATED     = 1,
   GPI_CHAN_RUNNING       = 2,
   GPI_CHAN_STOPPED       = 3,
   GPI_CHAN_STOP_IN_PROG  = 4,
   GPI_CHAN_ERROR         = 15
}GPI_CHAN_STATE;

/** @name GPI Ring Element */
typedef PACKED struct _gpi_ring_elem
{
   uint32 dword_0;
   uint32 dword_1;
   uint32 dword_2;
   uint32 ctrl;
}gpi_ring_elem;

/** @name GPI Execution Env */
typedef enum
{
   GPI_SDC_EE   = 0x0,
   GPI_SSC_EE   = 0x1,
   GPI_MSS_EE   = 0x2,
   GPI_ADSP_EE  = 0x3,
   GPI_BOOT_EE  = 0x4,
   GPI_TZ_EE    = 0x5,
   GPI_APPS_EE  = 0x6,

   GPI_EXEC_ENV_MAX
}GPI_EXEC_ENV;

/** @name GPI protocol Type */
typedef enum
{
   GPI_SPI_PROTOCOL   = 1,
   GPI_UART_PROTOCOL  = 2,
   GPI_I2C_PROTOCOL   = 3,
   GPI_I3C_PROTOCOL   = 4,

   GPI_MAX_PROTOCOL
}GPI_PROTOCOL;

#define GSI_CHAN_PER_GPII     2

/** @name Channel Type */
typedef enum
{
   GPI_OUTBOUND_CHAN = 0,
   GPI_INBOUND_CHAN  = 1,
   GPI_EVT_RING      = 2,

   UNKNOWN_CHAN = 0xFF
}GPI_CHAN_TYPE;

/** @name TRE Type - combination of Majo/Minor types */
typedef enum
{
   GPI_TRE_INVALID       = 0,
   GPI_TRE_NOOP          = 1,
   /* DMA */
   GPI_TRE_DMA_W_BUFFER  = 16,
   GPI_TRE_DMA_TX_IMM    = 17,
   GPI_TRE_DMA_W_SGL     = 18,
   /* SPI - UART - I2C */
   GPI_TRE_GO            = 32,
   GPI_TRE_CONFIG_0      = 34,
   GPI_TRE_CONFIG_1      = 35,
   GPI_TRE_CONFIG_2      = 36,
   GPI_TRE_CONFIG_3      = 37,
   /* Lock */
   GPI_TRE_LOCK          = 48,
   GPI_TRE_UNLOCK        = 49
}GPI_TRE_TYPE;

/** @name SPI GO Command Type */
typedef enum
{
   GPI_SPI_GO_CMD_INVALID      = 0,
   GPI_SPI_GO_CMD_TX_ONLY      = 1,
   GPI_SPI_GO_CMD_RX_ONLY      = 2,
   GPI_SPI_GO_CMD_FULL_DUPLEX  = 3,
   GPI_SPI_GO_CMD_TX_RX        = 7,
   GPI_SPI_GO_CMD_CS_ASSERT    = 8,
   GPI_SPI_GO_CMD_CS_DEASSERT  = 9,
   GPI_SPI_GO_CMD_SCK_ONLY     = 10
}GPI_SPI_GO_CMD_TYPE;

/** @name UART GO Command Type */
typedef enum
{
   GPI_UART_GO_CMD_INVALID      = 0,
   GPI_UART_GO_CMD_START_TRANS  = 1,
   GPI_UART_GO_CMD_START_BREAK  = 8,
   GPI_UART_GO_CMD_STOP_BREAK   = 9,
   GPI_UART_GO_CMD_RESET_RFR_N  = 10,
   GPI_UART_GO_CMD_SET_RFR_N    = 11,
   GPI_UART_GO_CMD_SW_RX_STALE  = 12
}GPI_UART_GO_CMD_TYPE;

/** @name I2C GO Command Type */
typedef enum
{
   GPI_I2C_GO_CMD_TX_ONLY       = 1,
   GPI_I2C_GO_CMD_RX_ONLY       = 2,
   GPI_I2C_GO_CMD_TX_THEN_RX    = 3,
   GPI_I2C_GO_CMD_TX_ADDR_ONLY  = 4,
   GPI_I2C_GO_CMD_BUS_CLEAR     = 6,
   GPI_I2C_GO_CMD_STOP_ONLY     = 7,
}GPI_I2C_GO_CMD_TYPE;

/** @name I3C GO Command Type */
typedef enum
{
   GPI_I3C_GO_CMD_PRIVATE_WRITE         = 9,
   GPI_I3C_GO_CMD_PRIVATE_READ          = 10,
   GPI_I3C_GO_CMD_HDR_DDR_WRITE         = 11,
   GPI_I3C_GO_CMD_HDR_DDR_READ          = 12,
   GPI_I3C_GO_CMD_READ_IBI              = 15,
   GPI_I3C_GO_CMD_BROADCAST_CCC_WRITE   = 16,
   GPI_I3C_GO_CMD_DIRECT_CCC_WRITE      = 17,
   GPI_I3C_GO_CMD_DIRECT_CCC_READ       = 18,
}GPI_I3C_GO_CMD_TYPE;

/** @name GPI status return */
typedef enum
{
   GPI_STATUS_SUCCESS            = 0x0,
   GPI_STATUS_ERROR              = 0x1,
   GPI_STATUS_RING_FULL          = 0x2,
   GPI_STATUS_GSI_NOT_READY      = 0x3,
   GPI_STATUS_GPII_NUM_EXCEEDED  = 0x4,
   GPI_STATUS_WP_OUT_OF_SYNC     = 0x5,
   GPI_STATUS_INVALID_CHAN       = 0x6
}GPI_RETURN_STATUS;

/* Event Ring Completion Status */
typedef enum
{
   EVT_COMPLETION_INVALID   = 0,
   EVT_COMPLETION_SUCCESS   = 1,
   EVT_COMPLETION_EOT       = 2,
   EVT_COMPLETION_OVERFLOW  = 3,
   EVT_COMPLETION_EOB       = 4,
   EVT_COMPLETION_OOB       = 5,
   EVT_COMPLETION_DB_MODE   = 6,
   EVT_COMPLETION_UNDEF     = 16,
   EVT_COMPLETION_MALFORMED = 17,

   EVT_COMPLETION_OTHER = RESERVED_VALUE
}GPI_EVT_CODE;

/* GPI Event Ring Element Type */
typedef enum
{
   EVT_STATE_CHANGE         = 32,
   EVT_CMD_COMPLETION       = 33,
   EVT_XFER_COMPLETION      = 34,
   EVT_IMM_CMD_COMPLETION   = 35,
   EVT_IMM_DATA             = 48,
   EVT_QUP_NOTIF            = 49,
   EVT_EE_STATE_CHANGE      = 64,

   GPI_EVT_OTHER            = 0xFF
} GPI_EVT_TYPE;

/* GPI Event Ring Element Type */
typedef enum
{
   /* Global error */
   GPI_ERROR_GLOBAL         = 0,

   /* General errors */
   BREAK_POINT              = 1,
   BUS_ERROR                = 2,
   CMD_FIFO_OVRFLOW         = 3,
   MCS_STACK_OVRFLOW        = 4,

   GPI_ERROR_OTHER          = 0xFF
} GPI_ERROR_TYPE;

/** @name GPI result
 *  chan:      Channel type: inbound, outbound, or event ring
 *  type:      type of event this result is for, if GPI_EVT_OTHER then error happened
 *  code:      Event code
 *  status:    Holds protocol specific TRE notification passed from GSI
 *  length:    if type is EVT_XFER_COMPLETION, length is the # of bytes processed
 *             if type is QUP_NOTIF_EVT, length will be the QUP Count
 *             if type is IMM_DATA_EVT, length is the Number of bytes received
 *  dword_0:   if IMM_DATA_EVT, dword_0 is the Immediate Data Bytes [3-0]
 *             if QUP_NOTIF_EVT, dword_0 is the QUP Status
 *             if CMD_COMPLETION_EVT then it represents the new chan state
 *             if GPI_EVT_OTHER, dword_0 holds the error type GPI_ERROR_TYPE
 *  dword_1:   if IMM_DATA_EVT, dword_1 is the Immediate Data Bytes [7-4]
 *             if QUP_NOTIF_EVT, dword_1 is the Timestamp
 *             if GPI_EVT_OTHER and dword_0 is GPI_ERROR_GLOBAL, dword_1 holds the error value
 *  user_data: user context
 */
typedef struct _gpi_result_type
{
   GPI_CHAN_TYPE    chan;
   GPI_EVT_TYPE     type;
   GPI_EVT_CODE     code;
   uint32           status;
   uint32           length;
   uint32           tre_idx;
   uint32           dword_0;
   uint32           dword_1;
   void *           user_data;
}gpi_result_type;

/** Ring info provided by clients.
 *  Clients allocate their respective channel TRE rings tailored to their specific needs.
 *  NOTE: clients should not allocate more than the size MAX_NUM_GPI_RING_ELEM defined above,
 *        due to memory restrictions.
 *
 *  tre_ring_size: Number of TRE's per ring
 *  ring_base_pa:  Physical address of the TRE ring base
 *  ring_base_va:  Virtual address of the TRE ring base
 */
typedef struct _gpi_tre_ring
{
   uint8         tre_ring_size;
   U_LONG      ring_base_pa;
   U_LONG      ring_base_va;
}gpi_tre_ring;

/** @name GPI Handle */
typedef void * GPI_CLIENT_HANDLE;

/** @name Client callback signature */
typedef void(*client_cb)(gpi_result_type *);

/** @name GPI TRE Request */
typedef PACKED struct _gpi_tre_req
{
   GPI_CLIENT_HANDLE  handle;
   GPI_CHAN_TYPE      chan;
   gpi_ring_elem    * tre_list;
   uint32             num_tre;
   void             * user_data;
}gpi_tre_req;

/** @name GPI interface parameters
 *
 *  [in, out] gpi_handle      GPI Interface client handle
 *  [in] gpii_id              GPI interface of this channel (from config file?)
 *  [in] protocol             SPI, UART, I2C, I3C
 *  [in] chan                 Inbound/Outbound channel settings
 *  [in] int_modt_val         Interrupt moderation timer value (optional)
 *  [in] int_modc_val         Interrupt moderation counter value (optional)
 *  [in] irq_mode             GPII in irq mode if true, otherwise polling
 *  [in] cb                   Callback function for tre requests
 *  [in] user_data            To be included in results passed to clients in non-transfer callbacks
 */
typedef struct _gpi_iface_params
{
   GPI_CLIENT_HANDLE  gpi_handle;
   uint8              gpii_id;
   QUP_BLOCK_TYPE     qup_type;
   GPI_PROTOCOL       protocol;
   gpi_tre_ring       chan[GSI_CHAN_PER_GPII];
   uint32             int_modt_val;
   uint32             int_modc_val;
   boolean            irq_mode;
   client_cb          cb;
   void             *user_data;
}gpi_iface_params;

/** @name Channel status
 *
 *  num_avail_tre     Number of available TRE's
 *  rp_index          RP index, 0..(tre_ring_size - 1)
 *  wp_index          WP index, 0..(tre_ring_size - 1)
 *  chan_state        Channel current state
 */
typedef struct _chan_status_info
{
   uint8  num_avail_tre;
   uint8  rp_index;
   uint8  wp_index;
   GPI_CHAN_STATE  chan_state;
}chan_status_info;

typedef struct _gpi_sgl_elem
{
    uint32 PTR_31_00;
    uint32 PTR_63_32;
    uint32 length;
    uint32 rsvd;
} gpi_sgl_elem;

/* *********************************************************************************************** */
/*                                               MACROS                                            */
/* *********************************************************************************************** */

#define GPI_GET_TRE_TYPE(_major, _minor)   (((_major) << 4) | (_minor))

#define GPI_FIELD_MASK(_FIELD)                (GPI_##_FIELD##__MASK)
#define GPI_FIELD_SHIFT(_FIELD)               (GPI_##_FIELD##__SHIFT)
#define GPI_SUBFIELD_SHIFT(_FIELD,_SUBFIELD)  (SHIFT(_FIELD##_##_SUBFIELD))

/* DMA w. buffer */
#define GPI_BUILD_DMA_W_BUFFER_TRE_0(_PTR)                            \
   (((_PTR) << GPI_FIELD_SHIFT(TRE_DMA_W_BUFFER_PTR)) & GPI_FIELD_MASK(TRE_DMA_W_BUFFER_PTR))
#define GPI_BUILD_DMA_W_BUFFER_TRE_1(_PTR)                            \
   (((_PTR) << GPI_FIELD_SHIFT(TRE_DMA_W_BUFFER_PTR)) & GPI_FIELD_MASK(TRE_DMA_W_BUFFER_PTR))
#define GPI_BUILD_DMA_W_BUFFER_TRE_2(_LENGTH)                         \
   (((_LENGTH) << GPI_FIELD_SHIFT(TRE_DMA_W_BUFFER_LENGTH)) & GPI_FIELD_MASK(TRE_DMA_W_BUFFER_LENGTH))

/* DMA w. S/G list */
#define GPI_BUILD_DMA_W_SGL_TRE_0(_PTR)                            \
   (((_PTR) << GPI_FIELD_SHIFT(TRE_DMA_W_SGL_PTR)) & GPI_FIELD_MASK(TRE_DMA_W_SGL_PTR))
#define GPI_BUILD_DMA_W_SGL_TRE_1(_PTR)                            \
   (((_PTR) << GPI_FIELD_SHIFT(TRE_DMA_W_SGL_PTR)) & GPI_FIELD_MASK(TRE_DMA_W_SGL_PTR))
#define GPI_BUILD_DMA_W_SGL_TRE_2(_LENGTH)                         \
   (((_LENGTH) & GPI_FIELD_MASK(TRE_DMA_W_SGL_LENGTH)) << GPI_FIELD_SHIFT(TRE_DMA_W_SGL_LENGTH))

/* DMA Immediate TRE */
#define GPI_BUILD_DMA_IMM_TRE_0(_DATA)                              \
   (((_DATA) << GPI_FIELD_SHIFT(TRE_DMA_IMM_DATA)) & GPI_FIELD_MASK(TRE_DMA_IMM_DATA))
#define GPI_BUILD_DMA_IMM_TRE_1(_DATA)                              \
   (((_DATA) << GPI_FIELD_SHIFT(TRE_DMA_IMM_DATA)) & GPI_FIELD_MASK(TRE_DMA_IMM_DATA))
#define GPI_BUILD_DMA_IMM_TRE_2(_LENGTH)                            \
   (((_LENGTH) << GPI_FIELD_SHIFT(TRE_DMA_IMM_LENGTH)) & GPI_FIELD_MASK(TRE_DMA_IMM_LENGTH))

/* SPI GO */
#define GPI_BUILD_TRE_SPI_GO_0(_CMD,_CS,_FLAGS)      \
   ((((_CMD) << GPI_FIELD_SHIFT(TRE_SPI_GO_CMD)) & GPI_FIELD_MASK(TRE_SPI_GO_CMD))                | \
    (((_CS) << GPI_FIELD_SHIFT(TRE_SPI_GO_CS)) & GPI_FIELD_MASK(TRE_SPI_GO_CS))                   | \
    (((_FLAGS) << GPI_FIELD_SHIFT(TRE_SPI_GO_FLAGS)) & GPI_FIELD_MASK(TRE_SPI_GO_FLAGS)))
#define GPI_BUILD_TRE_SPI_GO_2(_RX_LENGTH)                             \
   (((_RX_LENGTH) << GPI_FIELD_SHIFT(TRE_SPI_GO_RX_LENGTH)) & GPI_FIELD_MASK(TRE_SPI_GO_RX_LENGTH))

/* SPI CONFIG-0 */
#define GPI_BUILD_TRE_SPI_CONFIG_0_0(_WORD_SZ,_FLAGS,_PACKING)                \
   ((((_WORD_SZ) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_WORD_SZ)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_WORD_SZ)) | \
    (((_FLAGS) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_FLAGS)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_FLAGS))             | \
    (((_PACKING) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_PACKING)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_PACKING)))
#define GPI_BUILD_TRE_SPI_CONFIG_0_1(_INTER_WORD_DELAY,_CS_CLK_DELAY,_INTER_TRANSF_DELAY)                \
   ((((_INTER_WORD_DELAY) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_INTER_WORD_DELAY)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_INTER_WORD_DELAY)) | \
    (((_CS_CLK_DELAY) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_CS_CLK_DELAY)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_CS_CLK_DELAY))             | \
    (((_INTER_TRANSF_DELAY) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_INTER_TRANSF_DELAY)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_INTER_TRANSF_DELAY)))
#define GPI_BUILD_TRE_SPI_CONFIG_0_2(_CLCK_DIV,_CLCK_SRC)                      \
   ((((_CLCK_DIV) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_CLCK_DIV)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_CLCK_DIV)) |\
    (((_CLCK_SRC) << GPI_FIELD_SHIFT(TRE_SPI_CONFIG_0_CLCK_SRC)) & GPI_FIELD_MASK(TRE_SPI_CONFIG_0_CLCK_SRC)))

/* UART GO */
#define GPI_BUILD_TRE_UART_GO_0(_CMD)  \
   (((_CMD) << GPI_FIELD_SHIFT(TRE_UART_GO_CMD)) & GPI_FIELD_MASK(TRE_UART_GO_CMD))
#define GPI_BUILD_TRE_UART_GO_1(_TRANS_LENGTH)  \
   (((_TRANS_LENGTH) << GPI_FIELD_SHIFT(TRE_UART_GO_TRANS_LENGTH)) & GPI_FIELD_MASK(TRE_UART_GO_TRANS_LENGTH))
#define GPI_BUILD_TRE_UART_GO_2(_RX_STALE_TO)  \
   (((_RX_STALE_TO) << GPI_FIELD_SHIFT(TRE_UART_GO_RX_STALE_TO)) & GPI_FIELD_MASK(TRE_UART_GO_RX_STALE_TO))

/* UART CONFIG-0 */
#define GPI_BUILD_TRE_UART_CONFIG_0_0(_CH_SIZE,_STOP,_PARITY,_FLAGS,_PACKING)         \
   ((((_CH_SIZE) << GPI_FIELD_SHIFT(TRE_UART_CONFIG_0_CH_SIZE)) & GPI_FIELD_MASK(TRE_UART_CONFIG_0_CH_SIZE)) | \
    (((_STOP) << GPI_FIELD_SHIFT(TRE_UART_GO_STOP)) & GPI_FIELD_MASK(TRE_UART_GO_STOP))  | \
    (((_PARITY) << GPI_FIELD_SHIFT(TRE_UART_GO_PARITY)) & GPI_FIELD_MASK(TRE_UART_GO_PARITY))  | \
    (((_FLAGS) << GPI_FIELD_SHIFT(TRE_UART_GO_FLAGS)) & GPI_FIELD_MASK(TRE_UART_GO_FLAGS))  | \
    (((_PACKING) << GPI_FIELD_SHIFT(TRE_UART_CONFIG_0_PACKING)) & GPI_FIELD_MASK(TRE_UART_CONFIG_0_PACKING)))
#define GPI_BUILD_TRE_UART_CONFIG_0_1(_RX_STALE_TO)          \
   (((_RX_STALE_TO) << GPI_FIELD_SHIFT(TRE_UART_CONFIG_0_RX_STALE_TO)) & GPI_FIELD_MASK(TRE_UART_CONFIG_0_RX_STALE_TO))
#define GPI_BUILD_TRE_UART_CONFIG_0_2(_CLK_DIV)                \
   (((_CLK_DIV) << GPI_FIELD_SHIFT(TRE_UART_CONFIG_0_CLK_DIV)) & GPI_FIELD_MASK(TRE_UART_CONFIG_0_CLK_DIV))

/* I2C GO */
#define GPI_BUILD_TRE_I2C_GO_0(_CMD,_SLAVE_ID,_FLAGS)               \
   ((((_CMD) << GPI_FIELD_SHIFT(TRE_I2C_GO_CMD)) & GPI_FIELD_MASK(TRE_I2C_GO_CMD))     | \
    (((_SLAVE_ID) << GPI_FIELD_SHIFT(TRE_I2C_GO_SLAVE_ID)) & GPI_FIELD_MASK(TRE_I2C_GO_SLAVE_ID))  | \
    (((_FLAGS) << GPI_FIELD_SHIFT(TRE_I2C_GO_FLAGS)) & GPI_FIELD_MASK(TRE_I2C_GO_FLAGS)))
#define GPI_BUILD_TRE_I2C_GO_2(_RX_LENGTH)                      \
   (((_RX_LENGTH) << GPI_FIELD_SHIFT(TRE_I2C_GO_RX_LENGTH)) & GPI_FIELD_MASK(TRE_I2C_GO_RX_LENGTH))

/* I2C CONFIG-0 */
#define GPI_BUILD_TRE_I2C_CONFIG_0_0(_T_LOW,_T_HIGH,_T_CYCLE,_PACKING)         \
   ((((_T_LOW) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_T_LOW)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_T_LOW)) | \
    (((_T_HIGH) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_T_HIGH)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_T_HIGH)) | \
    (((_T_CYCLE) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_T_CYCLE)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_T_CYCLE)) | \
    (((_PACKING) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_PACKING)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_PACKING)))
#define GPI_BUILD_TRE_I2C_CONFIG_0_1(_NOISE_REJ_LEVEL,_INTER_TRANSF_DELAY)             \
   ((((_NOISE_REJ_LEVEL) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_NOISE_REJ_LEVEL)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_NOISE_REJ_LEVEL)) | \
    (((_INTER_TRANSF_DELAY) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_INTER_TRANSF_DELAY)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_INTER_TRANSF_DELAY)))
#define GPI_BUILD_TRE_I2C_CONFIG_0_2(_CLCK_DIV,_CLCK_SRC)                 \
   ((((_CLCK_DIV) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_CLCK_DIV)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_CLCK_DIV)) | \
    (((_CLCK_SRC) << GPI_FIELD_SHIFT(TRE_I2C_CONFIG_0_CLCK_SRC)) & GPI_FIELD_MASK(TRE_I2C_CONFIG_0_CLCK_SRC)))

/* I3C GO */
#define GPI_BUILD_TRE_I3C_GO_0(_CMD,_SLAVE_ID,_CCC,_FLAGS)                                                              \
   ((((_CMD)        << GPI_FIELD_SHIFT(TRE_I3C_GO_CMD))         & GPI_FIELD_MASK(TRE_I3C_GO_CMD))                   |   \
    (((_SLAVE_ID)   << GPI_FIELD_SHIFT(TRE_I3C_GO_SLAVE_ID))    & GPI_FIELD_MASK(TRE_I3C_GO_SLAVE_ID))              |   \
    (((_CCC)        << GPI_FIELD_SHIFT(TRE_I3C_GO_CCC))         & GPI_FIELD_MASK(TRE_I3C_GO_CCC))                   |   \
    (((_FLAGS)      << GPI_FIELD_SHIFT(TRE_I3C_GO_FLAGS))       & GPI_FIELD_MASK(TRE_I3C_GO_FLAGS)))

#define GPI_BUILD_TRE_I3C_GO_1(_I3C_FLAGS)                                                                              \
   (((_I3C_FLAGS)   << GPI_FIELD_SHIFT(TRE_I3C_GO_I3C_FLAGS))   & GPI_FIELD_MASK(TRE_I3C_GO_I3C_FLAGS))

#define GPI_BUILD_TRE_I3C_GO_2(_RX_LENGTH)                                                                              \
   (((_RX_LENGTH)   << GPI_FIELD_SHIFT(TRE_I3C_GO_RX_LENGTH))   & GPI_FIELD_MASK(TRE_I3C_GO_RX_LENGTH))

/* I3C CONFIG-0 */
#define GPI_BUILD_TRE_I3C_CONFIG_0_0(_T_LOW,_T_HIGH,_T_CYCLE,_PACKING)                                                          \
   ((((_T_LOW)          << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_T_LOW))         & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_T_LOW))       |   \
    (((_T_HIGH)         << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_T_HIGH))        & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_T_HIGH))      |   \
    (((_T_CYCLE)        << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_T_CYCLE))       & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_T_CYCLE))     |   \
    (((_PACKING)        << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_PACKING))       & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_PACKING)))

#define GPI_BUILD_TRE_I3C_CONFIG_0_1(_I3C_T_HIGH,_I3C_T_CYCLE,_CMD_DELAY)                                                        \
   ((((_I3C_T_HIGH)     << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_I3C_T_HIGH))    & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_I3C_T_HIGH)) |   \
    (((_I3C_T_CYCLE)    << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_I3C_T_CYCLE))   & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_I3C_T_CYCLE))|   \
    (((_CMD_DELAY)      << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_CMD_DELAY))     & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_CMD_DELAY)))

#define GPI_BUILD_TRE_I3C_CONFIG_0_2(_CLCK_DIV,_CLCK_SRC)                                                                       \
   ((((_CLCK_DIV)       << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_CLCK_DIV))      & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_CLCK_DIV))    |   \
    (((_CLCK_SRC)       << GPI_FIELD_SHIFT(TRE_I3C_CONFIG_0_CLCK_SRC))      & GPI_FIELD_MASK(TRE_I3C_CONFIG_0_CLCK_SRC)))

/* Control DWORD */
#define GPI_BUILD_TRE_CTRL(_MAJOR,_MINOR,_BEI,_IEOT,_IEOB,_CHAIN)    \
   ((((_MAJOR) << GPI_FIELD_SHIFT(MAJOR)) & GPI_FIELD_MASK(MAJOR)) | \
    (((_MINOR) << GPI_FIELD_SHIFT(MINOR)) & GPI_FIELD_MASK(MINOR)) | \
    (((_BEI) << GPI_FIELD_SHIFT(BEI)) & GPI_FIELD_MASK(BEI))       | \
    (((_IEOT) << GPI_FIELD_SHIFT(IEOT)) & GPI_FIELD_MASK(IEOT))    | \
    (((_IEOB) << GPI_FIELD_SHIFT(IEOB)) & GPI_FIELD_MASK(IEOB))    | \
    (((_CHAIN) << GPI_FIELD_SHIFT(CHAIN)) & GPI_FIELD_MASK(CHAIN)))

/* Scatter/Gather List Entry */
#define GPI_BUILD_SGL_ENTRY(_BUF_PTR_1,_BUF_PTR_2,_LEN)               \
   ((((_BUF_PTR_1) << GPI_FIELD_SHIFT(SGL_ENTRY_BUF_PTR_1)) & GPI_FIELD_MASK(SGL_ENTRY_BUF_PTR_1)) | \
    (((_SLAVE_ID) << GPI_FIELD_SHIFT(SGL_ENTRY_BUF_PTR_2)) & GPI_FIELD_MASK(SGL_ENTRY_BUF_PTR_2))  | \
    (((_LEN) << GPI_FIELD_SHIFT(SGL_ENTRY_BUF_LEN)) & GPI_FIELD_MASK(SGL_ENTRY_BUF_LEN)))
/* *********************************************************************************************** */
/*                                               APIs                                              */
/* *********************************************************************************************** */

/**
 * Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface registration parameters
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_reg(gpi_iface_params *params);

/**
 * De-Registers a client GPI interface.
 *
 * @param[in]    handle            Handle to GPI Interface
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_dereg(GPI_CLIENT_HANDLE handle);

/**
 * Queries whether a GPI interface is running in interrupt or polling mode.
 *
 * @param[in]        handle            Handle to GPI Interface
 * @param[in, out]   irq_mode          1 if Interface in IRQ mode, 0 otherwise
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_in_irq_mode(GPI_CLIENT_HANDLE handle, uint8 *irq_mode);

/**
 * Enables/Disables a GPI interface IRQ mode.
 *
 * @param[in]    handle           Handle to the GPI Interface
 * @param[in]    irq_en           TRUE to enable IRQ mode, FALSE for polling
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_set_irq_mode(GPI_CLIENT_HANDLE handle, boolean irq_en);

/**
 * Queries the channel current status.
 *
 * @param[in]         handle           Handle to the GPI Interface
 * @param[in]         chan             Virtual Channel ID
 * @param[in, out]    chan_status      Information about channel current status
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_query_chan_status(GPI_CLIENT_HANDLE handle,
                                        GPI_CHAN_TYPE chan,
                                        chan_status_info * chan_status);

/**
 * Polls a GPI interface.
 * The corresponding callback will be invoked with any available results.
 *
 * @param[in]    handle               GPI client handle
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_poll(GPI_CLIENT_HANDLE handle);

/**
 * Issues a channel command.
 *
 * @param[in]      handle              Handle to GPI Interface
 * @param[in]      chan                Virtual Channel ID
 * @param[in]      cmd_type            Command type
 * @param[in]      user_data           Client context data for this command
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_issue_cmd(GPI_CLIENT_HANDLE handle,
                                GPI_CHAN_TYPE chan,
                                GPI_CHAN_CMD cmd_type,
                                void *user_data);

/**
 * Process TREs request.
 *
 * @param[in]    tre_request           Request details
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_process_tre(gpi_tre_req *tre_request);

/**
 * Main function to initialize GPI EE, target, and gloabl data structure.
 * To be called from the main task before any GPI request is made.
 *
 * @param[in]    None.
 *
 * @return       None.
 */
void gpi_init(void);

GPI_RETURN_STATUS gpi_firmware_load(const void *addr, uint32 size);


#endif /* __GPI_H__ */
