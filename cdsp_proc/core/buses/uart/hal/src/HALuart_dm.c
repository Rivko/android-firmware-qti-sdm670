/*==================================================================================================

FILE: HALuart_dm.c

DESCRIPTION: HAL implementation for DMA-capable UARTs (UARTDM)

                           Copyright (c) 2008-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/hal/src/HALuart_dm.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   (none)

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "HALuart.h"

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#if 0
#include "debug.h"  // include OS-specific implementation of trace functions
#else
#define trace_reg_read(name, val)
#define trace_reg_write(name, val)
#endif

#define REG_IN(base, reg, value_ptr) { \
   *(value_ptr) = *(volatile uint32 *)(base + reg); \
   trace_reg_read(#reg, *(value_ptr)); \
}

#define REG_OUT(base, reg, value) { \
   *(volatile uint32 *)(base + reg) = (value); \
   trace_reg_write(#reg, (value)); \
}

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define MR1             0x00
#define MR2             0x04
#define CSR             0x08  // write
#define SR              0x08  // read
#define CR              0x10  // write
#define MISR            0x10  // read
#define IMR             0x14  // write
#define ISR             0x14  // read
#define IPR             0x18
#define TFWR            0x1C
#define RFWR            0x20
#define HCR             0x24
#define CXM_RX_STAT     0x28
#define DMRX            0x34
#define IRDA            0x38  // write
#define RX_TOTAL_SNAP   0x38  // read
#define DMEN            0x3C
#define NO_CHARS_FOR_TX 0x40
#define BADR            0x44
#define TXFS            0x4C
#define RXFS            0x50
#define CXM_CONFIG      0x54
#define CXM_TX_CONFIG   0x58
#define CXM_RX_STAT_CLR 0x5C
#define TF              0x70  // write
#define RF              0x70  // read
#define SIM_CFG         0x80

/* Mode Register 1 */
#define UARTDM_MR1_AUTO_RFR_LEVEL1  0xFFFFFF00
#define UARTDM_MR1_RX_RDY_CTL       0x00000080
#define UARTDM_MR1_CTS_CTL          0x00000040
#define UARTDM_MR1_AUTO_RFR_LEVEL0  0x0000003F

/* Mode Register 2 */
#define UARTDM_MR2_RX_ER_CH_OFF   0x00000200   // RX_ERROR_CHAR_OFF
#define UARTDM_MR2_RX_BK_Z_OFF    0x00000100   // RX_BREAK_ZERO_CHAR_OFF
#define UARTDM_MR2_LOOPBACK       0x00000080   // LOOPBACK
#define UARTDM_MR2_ERROR_MODE     0x00000040   // ERROR_MODE
#define UARTDM_MR2_BITS_PER_CHAR  0x00000030
#define UARTDM_MR2_STOP_BIT_LEN   0x0000000C
#define UARTDM_MR2_PARITY_MODE    0x00000003
#define UARTDM_MR2_5BPC           0x00000000   // 5 Bits per character
#define UARTDM_MR2_6BPC           0x00000010   // 6 Bits per character
#define UARTDM_MR2_7BPC           0x00000020   // 7 Bits per character
#define UARTDM_MR2_8BPC           0x00000030   // 8 Bits per character
#define UARTDM_MR2_05SB           0x00000000   // 0.5 Stop Bit
#define UARTDM_MR2_1SB            0x00000004   // 1 Stop Bit
#define UARTDM_MR2_15SB           0x00000008   // 1.5 Stop Bit
#define UARTDM_MR2_2SB            0x0000000C   // 2 Stop Bits
#define UARTDM_MR2_NOPAR          0x00000000   // No Parity
#define UARTDM_MR2_OPAR           0x00000001   // Odd Parity
#define UARTDM_MR2_EPAR           0x00000002   // Even Parity
#define UARTDM_MR2_SPAR           0x00000003   // Space Parity

/* Status Register */
#define UARTDM_SR_RXBREAK_START  0x00000100   // RX Break start
#define UARTDM_SR_RXHUNT         0x00000080   // Hunt Character Received
#define UARTDM_SR_RXBREAK        0x00000040   // Break Received
#define UARTDM_SR_PF_ERR         0x00000020   // Parity or Framing Error
#define UARTDM_SR_OVR_ERR        0x00000010   // Overrun Error
#define UARTDM_SR_TXEMT          0x00000008   // Transmitter Empty
#define UARTDM_SR_TXRDY          0x00000004   // Transmitter Ready
#define UARTDM_SR_RXFULL         0x00000002   // Receiver Full
#define UARTDM_SR_RXRDY          0x00000001   // Receiver Ready

/* Command Register */
#define UARTDM_CR_ENA_RX                0x00000001   // Enable Receiver
#define UARTDM_CR_DIS_RX                0x00000002   // Disable Receiver
#define UARTDM_CR_ENA_TX                0x00000004   // Enable Transmitter
#define UARTDM_CR_DIS_TX                0x00000008   // Disable Transmitter
#define UARTDM_CR_RESET_RX              0x00000010   // Reset Receiver
#define UARTDM_CR_RESET_TX              0x00000020   // Reset Transmitter
#define UARTDM_CR_RESET_ERR             0x00000030   // Reset Error Status
#define UARTDM_CR_RESET_RXBREAK_CHANGE  0x00000040   // Reset RX break change interrupt
#define UARTDM_CR_STA_BRK               0x00000050   // Start Break
#define UARTDM_CR_STO_BRK               0x00000060   // Stop Break
#define UARTDM_CR_RESET_STALE           0x00000080   // Clears the stale interrupt
#define UARTDM_CR_SET_RFR               0x000000D0   // Set RFR
#define UARTDM_CR_RESET_RFR             0x000000E0   // Reset RFR
#define UARTDM_CR_ENA_CR_PROT           0x00000100   // Enable CR protection
#define UARTDM_CR_DIS_CR_PROT           0x00000200   // Disable CR protection
#define UARTDM_CR_RESET_TX_READY        0x00000300   // Clears the TX_READY interrupt
#define UARTDM_CR_ENA_STALE_EVT         0x00000500   // Enables Stale Event
#define UARTDM_CR_DIS_STALE_EVT         0x00000600   // Disables Stale Event
#define UARTDM_CR_RESET_RXBREAK_START   0x00000820   // Reset RX break start interrupt

/* Interrupt Mask Register */
#define UARTDM_IMR_PAR_FRAME_ERR   0x00001000   // Parity/Framing error (mirrors bit in SR)
#define UARTDM_IMR_RXBREAK_END     0x00000800   // RX Break end
#define UARTDM_IMR_RXBREAK_START   0x00000400   // RX Break start
#define UARTDM_IMR_TX_DONE         0x00000200   // TX Done
#define UARTDM_IMR_TX_ERROR        0x00000100   // TX Ready
#define UARTDM_IMR_TX_READY        0x00000080   // TX Done
#define UARTDM_IMR_CUR_CTS         0x00000040   // Current CTS
#define UARTDM_IMR_DELTA_CTS       0x00000020   // Delta CTS
#define UARTDM_IMR_RXLEV           0x00000010   // RX Level exceeded
#define UARTDM_IMR_RXSTALE         0x00000008   // Stale RX character occurred
#define UARTDM_IMR_RXBREAK_CHANGE  0x00000004   // RX Break change occurred
#define UARTDM_IMR_RXHUNT          0x00000002   // RX Hunt character received
#define UARTDM_IMR_TXLEV           0x00000001   // TX Level or below met

/* Interrupt Programming Register */
#define UARTDM_IPR_STALE_TIMEOUT_MSB  0xFFFFFF80
#define UARTDM_IPR_STALE_TIMEOUT_LSB  0x0000001F

/* IRDA Register */
#define UARTDM_IRDA_IRDA_EN  0x00000001

/* Data Mover Enable Register */
#define UARTDM_DMEN_RX_DM_EN  0x00000002
#define UARTDM_DMEN_TX_DM_EN  0x00000001

/* TX FIFO State Register */
#define UARTDM_TXFS_FIFO_STATE_MSB_MASK   0xFFFFC000
#define UARTDM_TXFS_FIFO_STATE_MSB_SHIFT           7
#define UARTDM_TXFS_FIFO_STATE_LSB_MASK   0x0000007F
#define UARTDM_TXFS_FIFO_STATE_LSB_SHIFT           0

/* RX FIFO State Register */
#define UARTDM_RXFS_FIFO_STATE_MSB_MASK   0xFFFFC000
#define UARTDM_RXFS_FIFO_STATE_MSB_SHIFT           7
#define UARTDM_RXFS_FIFO_STATE_LSB_MASK   0x0000007F
#define UARTDM_RXFS_FIFO_STATE_LSB_SHIFT           0

/* CXM RX Stat */
#define UARTDM_CXM_RX_STAT_TX_STICKY_MASK 0x00000001

/* CXM configuration */
#define UARTDM_CXM_CONFIG_TWO_WIRE_MODE   0x00000001

/* CXM TX configuration */
#define UARTDM_CXM_TX_CONFIG_SAM_MASK     0x00000003
#define UARTDM_CXM_TX_CONFIG_SAM_SHIFT             0

/* CXM RX Stat clear */
#define UARTDM_CXM_RX_STAT_CLR_STKY_MASK  0x00000001

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static uint32 divider_to_csr(uint32 divider);
static uint32 flags_to_intmask(uint32 flags);
static uint32 intmask_to_flags(uint32 intmask);

static void   HALuart_dm_SetRxEnable                (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetTxEnable                (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_ResetReceiver              (HALUartObject *uart);
static void   HALuart_dm_ResetTransmitter           (HALUartObject *uart);
static void   HALuart_dm_ResetErrorStatus           (HALUartObject *uart);
static void   HALuart_dm_StartBreak                 (HALUartObject *uart);
static void   HALuart_dm_StopBreak                  (HALUartObject *uart);
static void   HALuart_dm_SetCmdSynchronization      (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetStaleEvent              (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_EnableInterrupts           (HALUartObject *uart, uint32 flags);
static void   HALuart_dm_DisableInterrupts          (HALUartObject *uart, uint32 flags);
static uint32 HALuart_dm_GetMaskedInterruptStatus   (HALUartObject *uart);
static uint32 HALuart_dm_GetInterruptStatus         (HALUartObject *uart);
static void   HALuart_dm_ResetStaleInterrupt        (HALUartObject *uart);
static void   HALuart_dm_ResetRxBreakChangeInterrupt(HALUartObject *uart);
static void   HALuart_dm_ResetRxBreakStartInterrupt (HALUartObject *uart);
static void   HALuart_dm_ResetTxReadyInterrupt      (HALUartObject *uart);
static void   HALuart_dm_SetDivider                 (HALUartObject *uart, uint32 divider);
static void   HALuart_dm_SetCharFormat              (HALUartObject *uart,
                                                        HALUartBitsPerCharType char_len,
                                                        HALUartNumStopBitsType stop_bits,
                                                        HALUartParityModeType parity);
static uint32 HALuart_dm_GetTxFifoSize              (HALUartObject *uart);
static uint32 HALuart_dm_GetRxFifoSize              (HALUartObject *uart);
static void   HALuart_dm_SetTxWatermark             (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_dm_SetRxWatermark             (HALUartObject *uart, uint32 num_bytes);
static uint32 HALuart_dm_ReadRxFifo                 (HALUartObject *uart, uint8 *ptr,
                                                                          uint32 num_bytes);
static uint32 HALuart_dm_WriteTxFifo                (HALUartObject *uart, const uint8 *ptr,
                                                                          uint32 num_bytes);
static void   HALuart_dm_SetStaleTimeout            (HALUartObject *uart, uint32 stale_timeout);
static uint32 HALuart_dm_GetStatus                  (HALUartObject *uart);
static void   HALuart_dm_SetAutoRfrLevel            (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_dm_SetRFR                     (HALUartObject *uart, uint32 on);
static void   HALuart_dm_SetAutoInboundFlowCtl      (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetOutboundFlowCtl         (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetIrdaEnable              (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetRxDmaEnable             (HALUartObject *uart, uint32 enable);
static void   HALuart_dm_SetTxDmaEnable             (HALUartObject *uart, uint32 enable);
static void   HALuart_dm_SetTxFifoSize              (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_dm_SetDmaNumTxChars           (HALUartObject *uart, uint32 num_chars);
static void   HALuart_dm_SetupDmaRx                 (HALUartObject *uart, uint32 num_chars);
static uint32 HALuart_dm_GetDmaRxSnapChars          (HALUartObject *uart);
static void   HALuart_dm_GetFIFOPhysAddrs           (HALUartObject *uart, void **rx_fifo,
                                                                          void **tx_fifo);
static void   HALuart_dm_GetRegisters               (HALUartObject *uart,
                                                        HALUartRegisters *registers);
static void   HALuart_dm_SetNullCharOnBreak         (HALUartObject *uart, uint32 enable);
static void   HALuart_dm_LoopbackSet                (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetCxmEnable               (HALUartObject *uart,
                                                        HALUartCxmConfigType cxm_cfg, uint32 sam);
static void   HALuart_dm_StartRxSideband            (HALUartObject *uart);
static void   HALuart_dm_StartTxSideband            (HALUartObject *uart);
static void   HALuart_dm_SetAutoReactivateRx        (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetDmaBurst                (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_dm_EnableHwFixes              (HALUartObject *uart, uint32 enabled);
static void   HALuart_dm_SetType0WakeupClear        (HALUartObject *uart, uint32 clear);
static uint32 HALuart_dm_GetCxmTxSticky             (HALUartObject *uart, uint32 clear);
static uint32 HALuart_dm_CxmTxDirectChar            (HALUartObject *uart, uint8 character);
static uint32 HALuart_dm_GetDmaRx                   (HALUartObject *uart);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

const HALUartFcnTable functions_uart_dm =
{
   HALuart_dm_SetRxEnable,
   HALuart_dm_SetTxEnable,
   HALuart_dm_ResetReceiver,
   HALuart_dm_ResetTransmitter,
   HALuart_dm_ResetErrorStatus,
   HALuart_dm_StartBreak,
   HALuart_dm_StopBreak,
   HALuart_dm_SetCmdSynchronization,
   HALuart_dm_SetStaleEvent,
   HALuart_dm_EnableInterrupts,
   HALuart_dm_DisableInterrupts,
   HALuart_dm_GetMaskedInterruptStatus,
   HALuart_dm_GetInterruptStatus,
   HALuart_dm_ResetStaleInterrupt,
   HALuart_dm_ResetRxBreakChangeInterrupt,
   HALuart_dm_ResetRxBreakStartInterrupt,
   HALuart_dm_ResetTxReadyInterrupt,
   HALuart_dm_SetDivider,
   HALuart_dm_SetCharFormat,
   HALuart_dm_GetTxFifoSize,
   HALuart_dm_GetRxFifoSize,
   HALuart_dm_SetTxWatermark,
   HALuart_dm_SetRxWatermark,
   HALuart_dm_ReadRxFifo,
   HALuart_dm_WriteTxFifo,
   HALuart_dm_SetStaleTimeout,
   HALuart_dm_GetStatus,
   HALuart_dm_SetAutoRfrLevel,
   HALuart_dm_SetRFR,
   HALuart_dm_SetAutoInboundFlowCtl,
   HALuart_dm_SetOutboundFlowCtl,
   HALuart_dm_SetIrdaEnable,
   HALuart_dm_SetRxDmaEnable,
   HALuart_dm_SetTxDmaEnable,
   HALuart_dm_SetTxFifoSize,
   HALuart_dm_SetDmaNumTxChars,
   HALuart_dm_SetupDmaRx,
   HALuart_dm_GetDmaRxSnapChars,
   HALuart_dm_GetFIFOPhysAddrs,
   HALuart_dm_GetRegisters,
   HALuart_dm_SetNullCharOnBreak,
   HALuart_dm_LoopbackSet,
   HALuart_dm_SetCxmEnable,
   HALuart_dm_StartRxSideband,
   HALuart_dm_StartTxSideband,
   HALuart_dm_SetAutoReactivateRx,
   HALuart_dm_SetDmaBurst,
   HALuart_dm_EnableHwFixes,
   HALuart_dm_SetType0WakeupClear,
   HALuart_dm_GetCxmTxSticky,
   HALuart_dm_CxmTxDirectChar,
   HALuart_dm_GetDmaRx,
};

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: divider_to_csr

DESCRIPTION:

==================================================================================================*/
static uint32 divider_to_csr(uint32 divider)
{
   switch (divider)  // assumes the transmitter/receiver use the same divider
   {
      case    16: return 0xFF;
      case    32: return 0xEE;
      case    48: return 0xDD;
      case    64: return 0xCC;
      case    96: return 0xBB;
      case   128: return 0xAA;
      case   192: return 0x99;
      case   256: return 0x88;
      case   384: return 0x77;
      case   512: return 0x66;
      case   768: return 0x55;
      case  1536: return 0x44;
      case  3072: return 0x33;
      case  6144: return 0x22;
      case 12288: return 0x11;
      case 24576: return 0x00;
      default:    return 0xFFFFFFFF;
   }
}

/*==================================================================================================

FUNCTION: flags_to_intmask

DESCRIPTION:

==================================================================================================*/
static uint32 flags_to_intmask(uint32 flags)
{
   uint32 intmask = 0;

   if ( flags & HAL_UART_PAR_FRAME_ERR_IMR_FLAG  ) { intmask |= UARTDM_IMR_PAR_FRAME_ERR;  }
   if ( flags & HAL_UART_RXBREAK_END_IMR_FLAG    ) { intmask |= UARTDM_IMR_RXBREAK_END;    }
   if ( flags & HAL_UART_RXBREAK_START_IMR_FLAG  ) { intmask |= UARTDM_IMR_RXBREAK_START;  }
   if ( flags & HAL_UART_TX_DONE_IMR_FLAG        ) { intmask |= UARTDM_IMR_TX_DONE;        }
   if ( flags & HAL_UART_TX_ERROR_IMR_FLAG       ) { intmask |= UARTDM_IMR_TX_ERROR;       }
   if ( flags & HAL_UART_TX_READY_IMR_FLAG       ) { intmask |= UARTDM_IMR_TX_READY;       }
   if ( flags & HAL_UART_CURRENT_CTS_IMR_FLAG    ) { intmask |= UARTDM_IMR_CUR_CTS;        }
   if ( flags & HAL_UART_DELTA_CTS_IMR_FLAG      ) { intmask |= UARTDM_IMR_DELTA_CTS;      }
   if ( flags & HAL_UART_RXLEV_IMR_FLAG          ) { intmask |= UARTDM_IMR_RXLEV;          }
   if ( flags & HAL_UART_RXSTALE_IMR_FLAG        ) { intmask |= UARTDM_IMR_RXSTALE;        }
   if ( flags & HAL_UART_RXBREAK_CHANGE_IMR_FLAG ) { intmask |= UARTDM_IMR_RXBREAK_CHANGE; }
   if ( flags & HAL_UART_RXHUNT_IMR_FLAG         ) { intmask |= UARTDM_IMR_RXHUNT;         }
   if ( flags & HAL_UART_TXLEV_IMR_FLAG          ) { intmask |= UARTDM_IMR_TXLEV;          }

   return intmask;
}

/*==================================================================================================

FUNCTION: intmask_to_flags

DESCRIPTION:

==================================================================================================*/
static uint32 intmask_to_flags(uint32 intmask)
{
   uint32 flags = 0;

   if ( intmask & UARTDM_IMR_PAR_FRAME_ERR  ) { flags |= HAL_UART_PAR_FRAME_ERR_IMR_FLAG;  }
   if ( intmask & UARTDM_IMR_RXBREAK_END    ) { flags |= HAL_UART_RXBREAK_END_IMR_FLAG;    }
   if ( intmask & UARTDM_IMR_RXBREAK_START  ) { flags |= HAL_UART_RXBREAK_START_IMR_FLAG;  }
   if ( intmask & UARTDM_IMR_TX_DONE        ) { flags |= HAL_UART_TX_DONE_IMR_FLAG;        }
   if ( intmask & UARTDM_IMR_TX_ERROR       ) { flags |= HAL_UART_TX_ERROR_IMR_FLAG;       }
   if ( intmask & UARTDM_IMR_TX_READY       ) { flags |= HAL_UART_TX_READY_IMR_FLAG;       }
   if ( intmask & UARTDM_IMR_CUR_CTS        ) { flags |= HAL_UART_CURRENT_CTS_IMR_FLAG;    }
   if ( intmask & UARTDM_IMR_DELTA_CTS      ) { flags |= HAL_UART_DELTA_CTS_IMR_FLAG;      }
   if ( intmask & UARTDM_IMR_RXLEV          ) { flags |= HAL_UART_RXLEV_IMR_FLAG;          }
   if ( intmask & UARTDM_IMR_RXSTALE        ) { flags |= HAL_UART_RXSTALE_IMR_FLAG;        }
   if ( intmask & UARTDM_IMR_RXBREAK_CHANGE ) { flags |= HAL_UART_RXBREAK_CHANGE_IMR_FLAG; }
   if ( intmask & UARTDM_IMR_RXHUNT         ) { flags |= HAL_UART_RXHUNT_IMR_FLAG;         }
   if ( intmask & UARTDM_IMR_TXLEV          ) { flags |= HAL_UART_TXLEV_IMR_FLAG;          }

   return flags;
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetRxEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetRxEnable(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTDM_CR_ENA_RX : UARTDM_CR_DIS_RX));
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetTxEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetTxEnable(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTDM_CR_ENA_TX : UARTDM_CR_DIS_TX));
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetReceiver

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetReceiver(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_RX);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetTransmitter

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetTransmitter(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_TX);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetErrorStatus

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetErrorStatus(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_ERR);
}

/*==================================================================================================

FUNCTION: HALuart_dm_StartBreak

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_StartBreak(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_STA_BRK);
}

/*==================================================================================================

FUNCTION: HALuart_dm_StopBreak

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_StopBreak(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_STO_BRK);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetCmdSynchronization

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetCmdSynchronization(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTDM_CR_ENA_CR_PROT : UARTDM_CR_DIS_CR_PROT));
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetStaleEvent

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetStaleEvent(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTDM_CR_ENA_STALE_EVT : UARTDM_CR_DIS_STALE_EVT));
}

/*==================================================================================================

FUNCTION: HALuart_dm_EnableInterrupts

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_EnableInterrupts(HALUartObject *uart, uint32 flags)
{
   uint32 intmask = flags_to_intmask(flags);

   if ( (uart->imr_shadow & intmask) != intmask )  // check whether interrupts are already enabled
   {
      uart->imr_shadow |= intmask;
      REG_OUT(uart->base, IMR, uart->imr_shadow);
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_DisableInterrupts

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_DisableInterrupts(HALUartObject *uart, uint32 flags)
{
   uint32 intmask = flags_to_intmask(flags);

   if (uart->imr_shadow & intmask)  // check whether interrupts are already disabled
   {
      uart->imr_shadow &= ~intmask;
      REG_OUT(uart->base, IMR, uart->imr_shadow);
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetMaskedInterruptStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetMaskedInterruptStatus(HALUartObject *uart)
{
   uint32 misr;

   REG_IN(uart->base, MISR, &misr);
   return intmask_to_flags(misr);
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetInterruptStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetInterruptStatus(HALUartObject *uart)
{
   uint32 isr;

   REG_IN(uart->base, ISR, &isr);
   return intmask_to_flags(isr);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetStaleInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetStaleInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_STALE);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetRxBreakChangeInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetRxBreakChangeInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_RXBREAK_CHANGE);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetRxBreakStartInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetRxBreakStartInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_RXBREAK_START);
}

/*==================================================================================================

FUNCTION: HALuart_dm_ResetTxReadyInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_ResetTxReadyInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTDM_CR_RESET_TX_READY);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetDivider

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetDivider(HALUartObject *uart, uint32 divider)
{
   uint32 csr = divider_to_csr(divider);

   if (csr == 0xFFFFFFFF)
   {
      uart->last_error = HAL_UART_NOT_SUPPORTED;
   }
   else
   {
      REG_OUT(uart->base, CSR, csr);
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetCharFormat

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetCharFormat(HALUartObject *uart,
                                     HALUartBitsPerCharType char_len,
                                     HALUartNumStopBitsType stop_bits,
                                     HALUartParityModeType parity)
{
   uint32 mr2;

   REG_IN(uart->base, MR2, &mr2);

   mr2 &= ~UARTDM_MR2_BITS_PER_CHAR;
   mr2 &= ~UARTDM_MR2_STOP_BIT_LEN;
   mr2 &= ~UARTDM_MR2_PARITY_MODE;

   switch (char_len)
   {
      case HAL_UART_5_BITS_PER_CHAR: mr2 |= UARTDM_MR2_5BPC; break;
      case HAL_UART_6_BITS_PER_CHAR: mr2 |= UARTDM_MR2_6BPC; break;
      case HAL_UART_7_BITS_PER_CHAR: mr2 |= UARTDM_MR2_7BPC; break;
      default:
      case HAL_UART_8_BITS_PER_CHAR: mr2 |= UARTDM_MR2_8BPC; break;
   }

   switch (stop_bits)
   {
      case HAL_UART_0_5_STOP_BITS: mr2 |= UARTDM_MR2_05SB; break;
      default:
      case HAL_UART_1_0_STOP_BITS: mr2 |= UARTDM_MR2_1SB;  break;
      case HAL_UART_1_5_STOP_BITS: mr2 |= UARTDM_MR2_15SB; break;
      case HAL_UART_2_0_STOP_BITS: mr2 |= UARTDM_MR2_2SB;  break;
   }

   switch (parity)
   {
      default:
      case HAL_UART_NO_PARITY:    mr2 |= UARTDM_MR2_NOPAR; break;
      case HAL_UART_ODD_PARITY:   mr2 |= UARTDM_MR2_OPAR;  break;
      case HAL_UART_EVEN_PARITY:  mr2 |= UARTDM_MR2_EPAR;  break;
      case HAL_UART_SPACE_PARITY: mr2 |= UARTDM_MR2_SPAR;  break;
   }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetTxFifoSize

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetTxFifoSize(HALUartObject *uart)
{
   uint32 badr;

   REG_IN(uart->base, BADR, &badr);
   return(badr << 2);  // convert words to bytes
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetRxFifoSize

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetRxFifoSize(HALUartObject *uart)
{
   uint32 saved_rfwr;
   uint32 max_rfwr;
   uint32 badr;

   REG_IN (uart->base, RFWR, &saved_rfwr);  // save previous setting
   REG_OUT(uart->base, RFWR, 0xFFFFFFFF);
   REG_IN (uart->base, RFWR, &max_rfwr);
   REG_OUT(uart->base, RFWR, saved_rfwr);  // restore previous setting
   REG_IN (uart->base, BADR, &badr);

   return((max_rfwr + 1 - badr) << 2);  // convert words to bytes
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetTxWatermark

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetTxWatermark(HALUartObject *uart, uint32 num_bytes)
{
   if ( num_bytes & 0x03 )  // must be a multiple of 4 bytes
   {
      uart->last_error = HAL_UART_BAD_PARAMETER;
   }
   else
   {
      REG_OUT(uart->base, TFWR, num_bytes >> 2);  // TFWR expressed in 4-byte words
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetRxWatermark

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetRxWatermark(HALUartObject *uart, uint32 num_bytes)
{
   if ( num_bytes == 0 || num_bytes & 0x03 )  // must be nonzero and a multiple of 4 bytes
   {
      uart->last_error = HAL_UART_BAD_PARAMETER;
   }
   else
   {
      REG_OUT(uart->base, RFWR, (num_bytes >> 2) - 1);  // RFWR expressed in 4-byte words;
                                                        // subtract 1 because the UART HW sets
                                                        // the RXLEV bit when the number of words
                                                        // in the RXFIFO is *greater* than the
                                                        // value in RFWR
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_ReadRxFifo

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_ReadRxFifo(HALUartObject *uart, uint8 *ptr, uint32 num_bytes)
{
   uint32 full_words = num_bytes >> 2;
   uint32 partial_bytes = num_bytes & 0x03;
   uint32 rxfs;
   uint32 words_in_fifo;
   uint32 words_to_read;
   uint32 bytes_to_read;
   uint32 word_value;
   uint32 i;

   REG_IN(uart->base, RXFS, &rxfs);

   words_in_fifo  = (rxfs & UARTDM_RXFS_FIFO_STATE_MSB_MASK) >> UARTDM_RXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (rxfs & UARTDM_RXFS_FIFO_STATE_LSB_MASK) >> UARTDM_RXFS_FIFO_STATE_LSB_SHIFT;

   words_to_read = (words_in_fifo <  full_words) ? (words_in_fifo) : (full_words);
   bytes_to_read = (words_in_fifo <= full_words) ? (0) : (partial_bytes);

   if ( (ptr - (uint8 *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
   {
      for (i = 0; i < words_to_read; i++)
      {
         REG_IN(uart->base, RF, &word_value);
         ptr[0] = (uint8)(word_value >>  0);
         ptr[1] = (uint8)(word_value >>  8);
         ptr[2] = (uint8)(word_value >> 16);
         ptr[3] = (uint8)(word_value >> 24);
         ptr += 4;
      }
   }
   else  // buffer is 32-bit word aligned
   {
      for (i = 0; i < words_to_read; i++)
      {
         REG_IN(uart->base, RF, &word_value);
         *(uint32 *)ptr = word_value;
         ptr += 4;
      }
   }

   if (bytes_to_read)
   {
      REG_IN(uart->base, RF, &word_value);
      for (i = 0; i < bytes_to_read; i++) { ptr[i] = (uint8)(word_value >> i * 8); }
   }

   return(words_to_read * 4 + bytes_to_read);
}

/*==================================================================================================

FUNCTION: HALuart_dm_WriteTxFifo

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_WriteTxFifo(HALUartObject *uart, const uint8 *ptr, uint32 num_bytes)
{
   uint32 full_words = num_bytes >> 2;
   uint32 partial_bytes = num_bytes & 0x03;
   uint32 badr;
   uint32 txfs;
   uint32 words_in_fifo;
   uint32 empty_slots;
   uint32 words_to_send;
   uint32 bytes_to_send;
   uint32 word_value;
   uint32 i;

   REG_IN(uart->base, BADR, &badr);
   REG_IN(uart->base, TXFS, &txfs);

   words_in_fifo  = (txfs & UARTDM_TXFS_FIFO_STATE_MSB_MASK) >> UARTDM_TXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (txfs & UARTDM_TXFS_FIFO_STATE_LSB_MASK) >> UARTDM_TXFS_FIFO_STATE_LSB_SHIFT;
   empty_slots = badr - words_in_fifo;

   words_to_send = (full_words >  empty_slots) ? (empty_slots) : (full_words);
   bytes_to_send = (full_words >= empty_slots) ? (0) : (partial_bytes);

   if ( (ptr - (uint8 *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
   {
      for (i = 0; i < words_to_send; i++)
      {
         word_value = ptr[0] << 0 | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
         REG_OUT(uart->base, TF, word_value);
         ptr += 4;
      }
   }
   else  // buffer is 32-bit word aligned
   {
      for (i = 0; i < words_to_send; i++)
      {
         word_value = *(uint32 *)ptr;
         REG_OUT(uart->base, TF, word_value);
         ptr += 4;
      }
   }

   if (bytes_to_send)
   {
      word_value = 0;
      for (i = 0; i < bytes_to_send; i++) { word_value |= ptr[i] << i * 8; }
      REG_OUT(uart->base, TF, word_value);
   }

   return(words_to_send * 4 + bytes_to_send);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetStaleTimeout

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetStaleTimeout(HALUartObject *uart, uint32 stale_timeout)
{
   uint32 ipr;

   REG_IN(uart->base, IPR, &ipr);

   ipr &= ~UARTDM_IPR_STALE_TIMEOUT_LSB;
   ipr &= ~UARTDM_IPR_STALE_TIMEOUT_MSB;

   ipr |= (stale_timeout)      & UARTDM_IPR_STALE_TIMEOUT_LSB;
   ipr |= (stale_timeout << 2) & UARTDM_IPR_STALE_TIMEOUT_MSB;

   REG_OUT(uart->base, IPR, ipr);
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetStatus(HALUartObject *uart)
{
   uint32 sr;
   uint32 flags = 0;

   REG_IN(uart->base, SR, &sr);

   //  Seems like overkill translating between the HAL layer and the
   //  raw bits.  Maybe we can return the raw bits and put a compile
   //  check that the appropriate raw bit is equal to the appropriate
   //  HAL flag.

   if (sr & UARTDM_SR_RXBREAK_START) { flags |= HAL_UART_STATUS_RX_BREAK_START;     }
   if (sr & UARTDM_SR_RXHUNT       ) { flags |= HAL_UART_STATUS_HUNT_CHAR;          }
   if (sr & UARTDM_SR_RXBREAK      ) { flags |= HAL_UART_STATUS_RX_BREAK;           }
   if (sr & UARTDM_SR_PF_ERR       ) { flags |= HAL_UART_STATUS_PARITY_FRAME_ERROR; }
   if (sr & UARTDM_SR_OVR_ERR      ) { flags |= HAL_UART_STATUS_OVERRUN;            }
   if (sr & UARTDM_SR_TXEMT        ) { flags |= HAL_UART_STATUS_TX_EMPTY;           }
   if (sr & UARTDM_SR_TXRDY        ) { flags |= HAL_UART_STATUS_TX_READY;           }
   if (sr & UARTDM_SR_RXFULL       ) { flags |= HAL_UART_STATUS_RX_FULL;            }
   if (sr & UARTDM_SR_RXRDY        ) { flags |= HAL_UART_STATUS_RX_READY;           }

   return( flags );
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetAutoRfrLevel

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetAutoRfrLevel(HALUartObject *uart, uint32 num_bytes)
{
   if ( num_bytes == 0 || num_bytes & 0x03 )  // must be nonzero and a multiple of 4 bytes
   {
      uart->last_error = HAL_UART_BAD_PARAMETER;
   }
   else
   {
      uint32 mr1;
      uint32 num_words = (num_bytes >> 2) - 1;  // subtract 1 because the UART HW deasserts
                                                // RTS when the number of words in the RXFIFO
                                                // is *greater* than the value in RFR_AUTO_LEVEL

      REG_IN(uart->base, MR1, &mr1);

      mr1 &= ~UARTDM_MR1_AUTO_RFR_LEVEL0;
      mr1 &= ~UARTDM_MR1_AUTO_RFR_LEVEL1;

      mr1 |= (num_words)      & UARTDM_MR1_AUTO_RFR_LEVEL0;
      mr1 |= (num_words << 2) & UARTDM_MR1_AUTO_RFR_LEVEL1;

      REG_OUT(uart->base, MR1, mr1);
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetRFR

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetRFR(HALUartObject *uart, uint32 on)
{
   REG_OUT(uart->base, CR, (on ? UARTDM_CR_SET_RFR : UARTDM_CR_RESET_RFR));
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetAutoInboundFlowCtl

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetAutoInboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uint32 mr1;

   REG_IN(uart->base, MR1, &mr1);

   if (enabled) { mr1 |=  UARTDM_MR1_RX_RDY_CTL; }
   else         { mr1 &= ~UARTDM_MR1_RX_RDY_CTL; }

   REG_OUT(uart->base, MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetOutboundFlowCtl

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetOutboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uint32 mr1;

   REG_IN(uart->base, MR1, &mr1);

   if (enabled) { mr1 |=  UARTDM_MR1_CTS_CTL; }
   else         { mr1 &= ~UARTDM_MR1_CTS_CTL; }

   REG_OUT(uart->base, MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetIrdaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetIrdaEnable(HALUartObject *uart, uint32 enabled)
{
   // NOTE: This register will not be generated if IRDA_IFC_GEN is not TRUE.  However,
   // in this case it should be harmless to write to it (there will be no effect).

   if (enabled) { uart->irda_shadow |=  UARTDM_IRDA_IRDA_EN; }
   else         { uart->irda_shadow &= ~UARTDM_IRDA_IRDA_EN; }

   REG_OUT(uart->base, IRDA, uart->irda_shadow);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetRxDmaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetRxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uint32 dmen;

   REG_IN(uart->base, DMEN, &dmen);

   if (enable) { dmen |=  UARTDM_DMEN_RX_DM_EN; }
   else        { dmen &= ~UARTDM_DMEN_RX_DM_EN; }

   REG_OUT(uart->base, DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetTxDmaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetTxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uint32 dmen;

   REG_IN(uart->base, DMEN, &dmen);

   if (enable) { dmen |=  UARTDM_DMEN_TX_DM_EN; }
   else        { dmen &= ~UARTDM_DMEN_TX_DM_EN; }

   REG_OUT(uart->base, DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetTxFifoSize

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetTxFifoSize(HALUartObject *uart, uint32 num_bytes)
{
   if (num_bytes & 0x0F)  // must be a multiple of 16 bytes due to DMOV burst length
   {
      uart->last_error = HAL_UART_BAD_PARAMETER;
   }
   else
   {
      REG_OUT(uart->base, BADR, num_bytes >> 2);  // BADR expressed in 4-byte words
   }
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetDmaNumTxChars

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetDmaNumTxChars(HALUartObject *uart, uint32 num_chars)
{
   REG_OUT(uart->base, NO_CHARS_FOR_TX, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetupDmaRx

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetupDmaRx(HALUartObject *uart, uint32 num_chars)
{
   REG_OUT(uart->base, DMRX, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetDmaRxSnapChars

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetDmaRxSnapChars(HALUartObject *uart)
{
   uint32 rx_total_snap;

   REG_IN(uart->base, RX_TOTAL_SNAP, &rx_total_snap);
   return(rx_total_snap);
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetFIFOPhysAddrs

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_GetFIFOPhysAddrs(HALUartObject *uart, void **rx_fifo, void **tx_fifo)
{
   *rx_fifo = uart->physical + RF;
   *tx_fifo = uart->physical + TF;
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetRegisters

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_GetRegisters(HALUartObject *uart, HALUartRegisters *registers)
{
   if (registers == NULL) { return; }

   REG_IN(uart->base, MR1,             &registers->mr1 );
   REG_IN(uart->base, MR2,             &registers->mr2 );
   REG_IN(uart->base, SR,              &registers->sr  );
   REG_IN(uart->base, MISR,            &registers->misr);
   REG_IN(uart->base, ISR,             &registers->isr );
   REG_IN(uart->base, IPR,             &registers->ipr );
   REG_IN(uart->base, TFWR,            &registers->tfwr);
   REG_IN(uart->base, RFWR,            &registers->rfwr);
   REG_IN(uart->base, HCR,             &registers->hcr );
   REG_IN(uart->base, SIM_CFG,         &registers->sim );
   REG_IN(uart->base, DMRX,            &registers->dmrx);
   REG_IN(uart->base, RX_TOTAL_SNAP,   &registers->snap);
   REG_IN(uart->base, DMEN,            &registers->dmen);
   REG_IN(uart->base, NO_CHARS_FOR_TX, &registers->ncft);
   REG_IN(uart->base, BADR,            &registers->badr);
   REG_IN(uart->base, TXFS,            &registers->txfs);
   REG_IN(uart->base, RXFS,            &registers->rxfs);
   registers->csr  = 0;  // BAM only
   registers->imr  = 0;  // BAM only
   registers->irda = 0;  // BAM only
   registers->cctl = 0;  // BAM only
   registers->bcr  = 0;  // BAM only
   registers->rctl = 0;  // BAM only
   registers->ddbg = 0;  // BAM only
   registers->fsm  = 0;  // BAM only
   registers->ver  = 0;  // BAM only
   registers->gen  = 0;  // BAM only
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetNullCharOnBreak

DESCRIPTION:
   Sets or clears the RX_BREAK_ZERO_CHAR_OFF bit of UART_DM_MR2. When the bit
   is set, the UART hardware does not pass through the zero character received
   during a break condition.

==================================================================================================*/
static void HALuart_dm_SetNullCharOnBreak(HALUartObject *uart, uint32 enable)
{
   uint32 mr2;

   REG_IN(uart->base, MR2, &mr2);

   if(enable)
   {
      /* Clear the RX_BREAK_ZERO_CHAR_OFF bit to enable the zero character */
      mr2 &= ~UARTDM_MR2_RX_BK_Z_OFF;
   }
   else
   {
      /* Set the RX_BREAK_ZERO_CHAR_OFF bit to disable the zero character */
      mr2 |= UARTDM_MR2_RX_BK_Z_OFF;
   }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_dm_LoopbackSet

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_LoopbackSet(HALUartObject *uart, uint32 enabled)
{
   uint32 mr2;

   REG_IN(uart->base, MR2, &mr2);

   if (enabled) { mr2 |=  UARTDM_MR2_LOOPBACK; }
   else         { mr2 &= ~UARTDM_MR2_LOOPBACK; }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetCxmEnable

DESCRIPTION:
   Enables or disables two-wire UART coexistence, on cores that support
   coexistence. The cxm_cfg argument specifies the mode to use:

   HAL_UART_CXM_DISABLE: Disable the UART, returning the hardware to its
   power-on state.

   HAL_UART_CXM_BYPASS: Bypass two-wire coexistence, so the UART can be used
   as a traditional UART, without special processing by hardware.

   HAL_UART_CXM_ENABLE: Enable the UART for two-wire coexistence. In this
   mode, the two-bit station address, sam, is set.

==================================================================================================*/
static void HALuart_dm_SetCxmEnable(HALUartObject *uart, HALUartCxmConfigType cxm_cfg, uint32 sam)
{
   uint32 cxm_config, cxm_tx_config;

   if(cxm_cfg == HAL_UART_CXM_ENABLE)
   {
      REG_IN(uart->base, CXM_TX_CONFIG, &cxm_tx_config);

      cxm_tx_config &= ~UARTDM_CXM_TX_CONFIG_SAM_MASK;
      cxm_tx_config |= (sam << UARTDM_CXM_TX_CONFIG_SAM_SHIFT) & UARTDM_CXM_TX_CONFIG_SAM_MASK;

      REG_OUT(uart->base, CXM_TX_CONFIG, cxm_tx_config);
   }

   REG_IN(uart->base, CXM_CONFIG, &cxm_config);

   switch(cxm_cfg)
   {
      case HAL_UART_CXM_DISABLE:
         /* Disable the UART */
         cxm_config &= ~UARTDM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      case HAL_UART_CXM_BYPASS:
         /* Bypass CXM entirely, placing the UART hardware in traditional UART
          * mode. This is not possible in UARTDM; the best we can do is to
          * enable CXM. */
         cxm_config |=  UARTDM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      case HAL_UART_CXM_ENABLE:
         /* Enable CXM */
         cxm_config |=  UARTDM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      default:
         break;
   }

   REG_OUT(uart->base, CXM_CONFIG, cxm_config);
}

/*==================================================================================================

FUNCTION: HALuart_dm_StartRxSideband

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_StartRxSideband(HALUartObject *uart)
{
   // This is a no-op on UARTDM.  It is unnecessary to start the sideband (CRCI) signals.
}

/*==================================================================================================

FUNCTION: HALuart_dm_StartTxSideband

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_StartTxSideband(HALUartObject *uart)
{
   // This is a no-op on UARTDM.  It is unnecessary to start the sideband (CRCI) signals.
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetAutoReactivateRx

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetAutoReactivateRx(HALUartObject *uart, uint32 enabled)
{
   // This is a no-op on UARTDM.  Automatic RX transfer reactivation is not supported.
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetDmaBurst

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetDmaBurst(HALUartObject *uart, uint32 num_bytes)
{
   // This is a no-op on UARTDM.  The burst size is 16 bytes and cannot be changed.
}

/*==================================================================================================

FUNCTION: HALuart_dm_EnableHwFixes

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_EnableHwFixes(HALUartObject *uart, uint32 enabled)
{
   // This is a no-op on UARTDM.  There is no BCR register that enables/disables HW fixes.
}

/*==================================================================================================

FUNCTION: HALuart_dm_SetType0WakeupClear

DESCRIPTION:

==================================================================================================*/
static void HALuart_dm_SetType0WakeupClear(HALUartObject *uart, uint32 clear)
{
   // This is a no-op on UARTDM.  There is no Type0 clear register.
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetCxmTxSticky

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_GetCxmTxSticky(HALUartObject *uart, uint32 clear)
{
   uint32 tx_sticky;

   REG_IN(uart->base, CXM_RX_STAT, &tx_sticky);

   if(clear && (tx_sticky & UARTDM_CXM_RX_STAT_TX_STICKY_MASK))
   {
      REG_OUT(uart->base, CXM_RX_STAT_CLR, UARTDM_CXM_RX_STAT_CLR_STKY_MASK);
   }

   return tx_sticky & UARTDM_CXM_RX_STAT_TX_STICKY_MASK;
}

/*==================================================================================================

FUNCTION: HALuart_dm_CxmTxDirectChar

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_dm_CxmTxDirectChar(HALUartObject *uart, uint8 character)
{
   // This is a no-op on the A family CXM UART.  This feature is not supported in A family.
   return 0;
}

/*==================================================================================================

FUNCTION: HALuart_dm_GetDmaRx

DESCRIPTION:
   
==================================================================================================*/
static uint32 HALuart_dm_GetDmaRx(HALUartObject *uart)
{
   uint32 num_chars;

   REG_IN(uart->base, DMRX, &num_chars);
   return(num_chars);
}
