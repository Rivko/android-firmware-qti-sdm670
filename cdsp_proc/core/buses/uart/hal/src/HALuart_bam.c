/*==================================================================================================

FILE: HALuart_bam.c

DESCRIPTION: HAL implementation for BAM-capable UARTs (UARTBAM)

                           Copyright (c) 2012-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/hal/src/HALuart_bam.c#1 $

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
#define IPR             0x18
#define TFWR            0x1C
#define RFWR            0x20
#define HCR             0x24
#define DMRX            0x34
#define DMEN            0x3C
#define NO_CHARS_FOR_TX 0x40
#define BADR            0x44
#define TXFS            0x4C
#define RXFS            0x50
#define SIM_CFG         0x80
#define CSR             0xA0
#define SR              0xA4
#define CR              0xA8
#define MISR            0xAC
#define IMR             0xB0
#define ISR             0xB4
#define IRDA            0xB8
#define RX_TOTAL_SNAP   0xBC
#define CLK_CTRL        0xC4
#define BCR             0xC8
#define RX_TRANS_CTRL   0xCC
#define DMRX_DBG        0xD0
#define FSM_STATUS      0xD4
#define HW_VERSION      0xD8
#define GENERICS        0xDC
#define TF              0x100
#define RF              0x140
#define CXM_CONFIG      0x1A4
#define CXM_TX_CONFIG   0x1A8
#define CXM_RX_STAT     0x1AC
#define CXM_DIR_UART_TR 0x1B4

/* Mode Register 1 */
#define UARTBAM_MR1_AUTO_RFR_LEVEL1  0xFFFFFF00
#define UARTBAM_MR1_RX_RDY_CTL       0x00000080
#define UARTBAM_MR1_CTS_CTL          0x00000040
#define UARTBAM_MR1_AUTO_RFR_LEVEL0  0x0000003F

/* Mode Register 2 */
#define UARTBAM_MR2_RFR_CTS_LOOPBACK  0x00000400   // RFR/CTS LOOPBACK
#define UARTBAM_MR2_RX_ER_CH_OFF      0x00000200   // RX_ERROR_CHAR_OFF
#define UARTBAM_MR2_RX_BK_Z_OFF       0x00000100   // RX_BREAK_ZERO_CHAR_OFF
#define UARTBAM_MR2_LOOPBACK          0x00000080   // LOOPBACK
#define UARTBAM_MR2_ERROR_MODE        0x00000040   // ERROR_MODE
#define UARTBAM_MR2_BITS_PER_CHAR     0x00000030
#define UARTBAM_MR2_STOP_BIT_LEN      0x0000000C
#define UARTBAM_MR2_PARITY_MODE       0x00000003
#define UARTBAM_MR2_5BPC              0x00000000   // 5 Bits per character
#define UARTBAM_MR2_6BPC              0x00000010   // 6 Bits per character
#define UARTBAM_MR2_7BPC              0x00000020   // 7 Bits per character
#define UARTBAM_MR2_8BPC              0x00000030   // 8 Bits per character
#define UARTBAM_MR2_05SB              0x00000000   // 0.5 Stop Bit
#define UARTBAM_MR2_1SB               0x00000004   // 1 Stop Bit
#define UARTBAM_MR2_15SB              0x00000008   // 1.5 Stop Bit
#define UARTBAM_MR2_2SB               0x0000000C   // 2 Stop Bits
#define UARTBAM_MR2_NOPAR             0x00000000   // No Parity
#define UARTBAM_MR2_OPAR              0x00000001   // Odd Parity
#define UARTBAM_MR2_EPAR              0x00000002   // Even Parity
#define UARTBAM_MR2_SPAR              0x00000003   // Space Parity

/* Interrupt Programming Register */
#define UARTBAM_IPR_STALE_TIMEOUT_MSB  0xFFFFFF80
#define UARTBAM_IPR_STALE_TIMEOUT_LSB  0x0000001F

/* Data Mover Enable Register */
#define UARTBAM_DMEN_RX_SC_ENABLE   0x00000020
#define UARTBAM_DMEN_TX_SC_ENABLE   0x00000010
#define UARTBAM_DMEN_RX_BAM_ENABLE  0x00000008
#define UARTBAM_DMEN_TX_BAM_ENABLE  0x00000004

/* TX FIFO State Register */
#define UARTBAM_TXFS_FIFO_STATE_MSB_MASK   0xFFFFC000
#define UARTBAM_TXFS_FIFO_STATE_MSB_SHIFT           7
#define UARTBAM_TXFS_FIFO_STATE_LSB_MASK   0x0000007F
#define UARTBAM_TXFS_FIFO_STATE_LSB_SHIFT           0

/* RX FIFO State Register */
#define UARTBAM_RXFS_FIFO_STATE_MSB_MASK   0xFFFFC000
#define UARTBAM_RXFS_FIFO_STATE_MSB_SHIFT           7
#define UARTBAM_RXFS_FIFO_STATE_LSB_MASK   0x0000007F
#define UARTBAM_RXFS_FIFO_STATE_LSB_SHIFT           0

/* CXM configuration */
#define UARTBAM_CXM_CONFIG_BYPASS_CXM     0x00000002
#define UARTBAM_CXM_CONFIG_TWO_WIRE_MODE  0x00000001

/* CXM TX configuration */
#define UARTBAM_CXM_TX_CONFIG_SAM_MASK    0x00000006
#define UARTBAM_CXM_TX_CONFIG_SAM_SHIFT            1
#define UARTBAM_CXM_TX_CONFIG_T0_WU_CLR            1   // TYPE0_WAKEUP_CLEAR

/* CXM Direct UART Transmit*/
#define UARTBAM_CXM_DIR_UART_TR_DIR_UART_TRIG                       1
#define UARTBAM_CXM_DIR_UART_TR_DIR_UART_COUNT_SHIFT                8
#define UARTBAM_CXM_DIR_UART_TR_DIR_UART_COUNT_MASK        0x0000FF00

/* CXM_UART_DM_CLK_CTRL */
#define UARTBAM_CXM_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN   0x00000200

/* CXM RX status */
#define UARTBAM_CXM_RX_STAT_WLAN_STICKY   0x00000001

/* Status Register */
#define UARTBAM_SR_RXEVENT_HIGH   0x00000800   // RX transfer end event type (high bit)
#define UARTBAM_SR_RXEVENT_LOW    0x00000400   // RX transfer end event type (low bit)
#define UARTBAM_SR_TRANS_ACTIVE   0x00000200   // Transfer Active
#define UARTBAM_SR_RXBREAK_START  0x00000100   // RX Break start
#define UARTBAM_SR_RXHUNT         0x00000080   // Hunt Character Received
#define UARTBAM_SR_RXBREAK        0x00000040   // Break Received
#define UARTBAM_SR_PF_ERR         0x00000020   // Parity or Framing Error
#define UARTBAM_SR_OVR_ERR        0x00000010   // Overrun Error
#define UARTBAM_SR_TXEMT          0x00000008   // Transmitter Empty
#define UARTBAM_SR_TXRDY          0x00000004   // Transmitter Ready
#define UARTBAM_SR_RXFULL         0x00000002   // Receiver Full
#define UARTBAM_SR_RXRDY          0x00000001   // Receiver Ready

/* Command Register */
#define UARTBAM_CR_ENA_RX                0x00000001   // Enable Receiver
#define UARTBAM_CR_DIS_RX                0x00000002   // Disable Receiver
#define UARTBAM_CR_ENA_TX                0x00000004   // Enable Transmitter
#define UARTBAM_CR_DIS_TX                0x00000008   // Disable Transmitter
#define UARTBAM_CR_RESET_RX              0x00000010   // Reset Receiver
#define UARTBAM_CR_RESET_TX              0x00000020   // Reset Transmitter
#define UARTBAM_CR_RESET_ERR             0x00000030   // Reset Error Status
#define UARTBAM_CR_RESET_RXBREAK_CHANGE  0x00000040   // Reset RX break change interrupt
#define UARTBAM_CR_STA_BRK               0x00000050   // Start Break
#define UARTBAM_CR_STO_BRK               0x00000060   // Stop Break
#define UARTBAM_CR_RESET_STALE           0x00000080   // Clears the stale interrupt
#define UARTBAM_CR_SET_RFR               0x000000D0   // Set RFR
#define UARTBAM_CR_RESET_RFR             0x000000E0   // Reset RFR
#define UARTBAM_CR_ENA_CR_PROT           0x00000100   // Enable CR protection
#define UARTBAM_CR_DIS_CR_PROT           0x00000200   // Disable CR protection
#define UARTBAM_CR_RESET_TX_READY        0x00000300   // Clears the TX_READY interrupt
#define UARTBAM_CR_ENA_STALE_EVT         0x00000500   // Enables Stale Event
#define UARTBAM_CR_DIS_STALE_EVT         0x00000600   // Disables Stale Event
#define UARTBAM_CR_RESET_RXBREAK_START   0x00000820   // Reset RX break start interrupt
#define UARTBAM_CR_START_RX_BAM_IFC      0x00000850   // Initialize producer sidebands towards BAM
#define UARTBAM_CR_START_TX_BAM_IFC      0x00000860   // Initialize consumer sidebands towards BAM

/* Interrupt Mask Register */
#define UARTBAM_IMR_WWT             0x00010000   // WWT timeout
#define UARTBAM_IMR_TXCOMP          0x00008000   // TX complete
#define UARTBAM_IMR_RX_RD_ERROR     0x00004000   // attempt to read from empty RXFIFO
#define UARTBAM_IMR_TX_WR_ERROR     0x00002000   // attempt to write to full TXFIFO
#define UARTBAM_IMR_PAR_FRAME_ERR   0x00001000   // Parity/Framing error (mirrors bit in SR)
#define UARTBAM_IMR_RXBREAK_END     0x00000800   // RX Break end
#define UARTBAM_IMR_RXBREAK_START   0x00000400   // RX Break start
#define UARTBAM_IMR_TX_DONE         0x00000200   // TX Done
#define UARTBAM_IMR_TX_ERROR        0x00000100   // TX Ready
#define UARTBAM_IMR_TX_READY        0x00000080   // TX Done
#define UARTBAM_IMR_CUR_CTS         0x00000040   // Current CTS
#define UARTBAM_IMR_DELTA_CTS       0x00000020   // Delta CTS
#define UARTBAM_IMR_RXLEV           0x00000010   // RX Level exceeded
#define UARTBAM_IMR_RXSTALE         0x00000008   // Stale RX character occurred
#define UARTBAM_IMR_RXBREAK_CHANGE  0x00000004   // RX Break change occurred
#define UARTBAM_IMR_RXHUNT          0x00000002   // RX Hunt character received
#define UARTBAM_IMR_TXLEV           0x00000001   // TX Level or below met

/* IRDA Register */
#define UARTBAM_IRDA_IRDA_EN  0x00000001

/* Backwards Compatibility Register */
#define UARTBAM_BCR_DMRX_1BYTE_RES    0x00000020   // allows DMRX that is not a multiple of 16
#define UARTBAM_BCR_DMRX_FORCE_STALE  0x00000010   // DMRX event forces stale even if not enabled
#define UARTBAM_BCR_DMRX_LOW_EN       0x00000004   // allows writing DMRX less than bytes received
#define UARTBAM_BCR_STALE_IRQ_EMPTY   0x00000002   // stale interrupt fires when RXFIFO is empty
#define UARTBAM_BCR_TX_BREAK_DISABLE  0x00000001   // TX stays low when TX disabled in break state

/* RX Transfer Control Register */
#define UARTBAM_RXCTL_DMRX_CYCLIC  0x00000004   // use previous DMRX value
#define UARTBAM_RXCTL_AUTO_RX      0x00000002   // RX transfer automatically reactivated
#define UARTBAM_RXCTL_AUTO_STALE   0x00000001   // stale event automatically reenabled

/* UART_DM_GENERICS */
#define UARTBAM_GENERIC_BAM_IFC         0x00000080
#define UARTBAM_GENERIC_DM_IFC          0x00000040
#define UARTBAM_GENERIC_IRDA_IFC        0x00000020
#define UARTBAM_GENERIC_SIM_GLUE        0x00000010
#define UARTBAM_GENERIC_RAM_ADDR_WIDTH  0x0000000F

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static uint32 divider_to_csr(uint32 divider);
static uint32 flags_to_intmask(uint32 flags);
static uint32 intmask_to_flags(uint32 intmask);

static void   HALuart_bam_SetRxEnable                (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetTxEnable                (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_ResetReceiver              (HALUartObject *uart);
static void   HALuart_bam_ResetTransmitter           (HALUartObject *uart);
static void   HALuart_bam_ResetErrorStatus           (HALUartObject *uart);
static void   HALuart_bam_StartBreak                 (HALUartObject *uart);
static void   HALuart_bam_StopBreak                  (HALUartObject *uart);
static void   HALuart_bam_SetCmdSynchronization      (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetStaleEvent              (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_EnableInterrupts           (HALUartObject *uart, uint32 flags);
static void   HALuart_bam_DisableInterrupts          (HALUartObject *uart, uint32 flags);
static uint32 HALuart_bam_GetMaskedInterruptStatus   (HALUartObject *uart);
static uint32 HALuart_bam_GetInterruptStatus         (HALUartObject *uart);
static void   HALuart_bam_ResetStaleInterrupt        (HALUartObject *uart);
static void   HALuart_bam_ResetRxBreakChangeInterrupt(HALUartObject *uart);
static void   HALuart_bam_ResetRxBreakStartInterrupt (HALUartObject *uart);
static void   HALuart_bam_ResetTxReadyInterrupt      (HALUartObject *uart);
static void   HALuart_bam_SetDivider                 (HALUartObject *uart, uint32 divider);
static void   HALuart_bam_SetCharFormat              (HALUartObject *uart,
                                                         HALUartBitsPerCharType char_len,
                                                         HALUartNumStopBitsType stop_bits,
                                                         HALUartParityModeType parity);
static uint32 HALuart_bam_GetTxFifoSize              (HALUartObject *uart);
static uint32 HALuart_bam_GetRxFifoSize              (HALUartObject *uart);
static void   HALuart_bam_SetTxWatermark             (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_bam_SetRxWatermark             (HALUartObject *uart, uint32 num_bytes);
static uint32 HALuart_bam_ReadRxFifo                 (HALUartObject *uart, uint8 *ptr,
                                                                           uint32 num_bytes);
static uint32 HALuart_bam_WriteTxFifo                (HALUartObject *uart, const uint8 *ptr,
                                                                           uint32 num_bytes);
static void   HALuart_bam_SetStaleTimeout            (HALUartObject *uart, uint32 stale_timeout);
static uint32 HALuart_bam_GetStatus                  (HALUartObject *uart);
static void   HALuart_bam_SetAutoRfrLevel            (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_bam_SetRFR                     (HALUartObject *uart, uint32 on);
static void   HALuart_bam_SetAutoInboundFlowCtl      (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetOutboundFlowCtl         (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetIrdaEnable              (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetRxDmaEnable             (HALUartObject *uart, uint32 enable);
static void   HALuart_bam_SetTxDmaEnable             (HALUartObject *uart, uint32 enable);
static void   HALuart_bam_SetTxFifoSize              (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_bam_SetDmaNumTxChars           (HALUartObject *uart, uint32 num_chars);
static void   HALuart_bam_SetupDmaRx                 (HALUartObject *uart, uint32 num_chars);
static uint32 HALuart_bam_GetDmaRxSnapChars          (HALUartObject *uart);
static void   HALuart_bam_GetFIFOPhysAddrs           (HALUartObject *uart, void **rx_fifo,
                                                                           void **tx_fifo);
static void   HALuart_bam_GetRegisters               (HALUartObject *uart,
                                                         HALUartRegisters *registers);
static void   HALuart_bam_SetNullCharOnBreak         (HALUartObject *uart, uint32 enable);
static void   HALuart_bam_LoopbackSet                (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetCxmEnable               (HALUartObject *uart,
                                                         HALUartCxmConfigType cxm_cfg, uint32 sam);
static void   HALuart_bam_StartRxSideband            (HALUartObject *uart);
static void   HALuart_bam_StartTxSideband            (HALUartObject *uart);
static void   HALuart_bam_SetAutoReactivateRx        (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetDmaBurst                (HALUartObject *uart, uint32 num_bytes);
static void   HALuart_bam_EnableHwFixes              (HALUartObject *uart, uint32 enabled);
static void   HALuart_bam_SetType0WakeupClear        (HALUartObject *uart, uint32 clear);
static uint32 HALuart_bam_GetCxmTxSticky             (HALUartObject *uart, uint32 clear);
static uint32 HALuart_bam_CxmTxDirectChar            (HALUartObject *uart, uint8 character);
static uint32 HALuart_bam_GetDmaRx                   (HALUartObject *uart);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

const HALUartFcnTable functions_uart_bam =
{
   HALuart_bam_SetRxEnable,
   HALuart_bam_SetTxEnable,
   HALuart_bam_ResetReceiver,
   HALuart_bam_ResetTransmitter,
   HALuart_bam_ResetErrorStatus,
   HALuart_bam_StartBreak,
   HALuart_bam_StopBreak,
   HALuart_bam_SetCmdSynchronization,
   HALuart_bam_SetStaleEvent,
   HALuart_bam_EnableInterrupts,
   HALuart_bam_DisableInterrupts,
   HALuart_bam_GetMaskedInterruptStatus,
   HALuart_bam_GetInterruptStatus,
   HALuart_bam_ResetStaleInterrupt,
   HALuart_bam_ResetRxBreakChangeInterrupt,
   HALuart_bam_ResetRxBreakStartInterrupt,
   HALuart_bam_ResetTxReadyInterrupt,
   HALuart_bam_SetDivider,
   HALuart_bam_SetCharFormat,
   HALuart_bam_GetTxFifoSize,
   HALuart_bam_GetRxFifoSize,
   HALuart_bam_SetTxWatermark,
   HALuart_bam_SetRxWatermark,
   HALuart_bam_ReadRxFifo,
   HALuart_bam_WriteTxFifo,
   HALuart_bam_SetStaleTimeout,
   HALuart_bam_GetStatus,
   HALuart_bam_SetAutoRfrLevel,
   HALuart_bam_SetRFR,
   HALuart_bam_SetAutoInboundFlowCtl,
   HALuart_bam_SetOutboundFlowCtl,
   HALuart_bam_SetIrdaEnable,
   HALuart_bam_SetRxDmaEnable,
   HALuart_bam_SetTxDmaEnable,
   HALuart_bam_SetTxFifoSize,
   HALuart_bam_SetDmaNumTxChars,
   HALuart_bam_SetupDmaRx,
   HALuart_bam_GetDmaRxSnapChars,
   HALuart_bam_GetFIFOPhysAddrs,
   HALuart_bam_GetRegisters,
   HALuart_bam_SetNullCharOnBreak,
   HALuart_bam_LoopbackSet,
   HALuart_bam_SetCxmEnable,
   HALuart_bam_StartRxSideband,
   HALuart_bam_StartTxSideband,
   HALuart_bam_SetAutoReactivateRx,
   HALuart_bam_SetDmaBurst,
   HALuart_bam_EnableHwFixes,
   HALuart_bam_SetType0WakeupClear,
   HALuart_bam_GetCxmTxSticky,
   HALuart_bam_CxmTxDirectChar,
   HALuart_bam_GetDmaRx,
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

   if ( flags & HAL_UART_WWT_IMR_FLAG            ) { intmask |= UARTBAM_IMR_WWT;            }
   if ( flags & HAL_UART_TXCOMP_IMR_FLAG         ) { intmask |= UARTBAM_IMR_TXCOMP;         }
   if ( flags & HAL_UART_RX_RD_ERROR_IMR_FLAG    ) { intmask |= UARTBAM_IMR_RX_RD_ERROR;    }
   if ( flags & HAL_UART_TX_WR_ERROR_IMR_FLAG    ) { intmask |= UARTBAM_IMR_TX_WR_ERROR;    }
   if ( flags & HAL_UART_PAR_FRAME_ERR_IMR_FLAG  ) { intmask |= UARTBAM_IMR_PAR_FRAME_ERR;  }
   if ( flags & HAL_UART_RXBREAK_END_IMR_FLAG    ) { intmask |= UARTBAM_IMR_RXBREAK_END;    }
   if ( flags & HAL_UART_RXBREAK_START_IMR_FLAG  ) { intmask |= UARTBAM_IMR_RXBREAK_START;  }
   if ( flags & HAL_UART_TX_DONE_IMR_FLAG        ) { intmask |= UARTBAM_IMR_TX_DONE;        }
   if ( flags & HAL_UART_TX_ERROR_IMR_FLAG       ) { intmask |= UARTBAM_IMR_TX_ERROR;       }
   if ( flags & HAL_UART_TX_READY_IMR_FLAG       ) { intmask |= UARTBAM_IMR_TX_READY;       }
   if ( flags & HAL_UART_CURRENT_CTS_IMR_FLAG    ) { intmask |= UARTBAM_IMR_CUR_CTS;        }
   if ( flags & HAL_UART_DELTA_CTS_IMR_FLAG      ) { intmask |= UARTBAM_IMR_DELTA_CTS;      }
   if ( flags & HAL_UART_RXLEV_IMR_FLAG          ) { intmask |= UARTBAM_IMR_RXLEV;          }
   if ( flags & HAL_UART_RXSTALE_IMR_FLAG        ) { intmask |= UARTBAM_IMR_RXSTALE;        }
   if ( flags & HAL_UART_RXBREAK_CHANGE_IMR_FLAG ) { intmask |= UARTBAM_IMR_RXBREAK_CHANGE; }
   if ( flags & HAL_UART_RXHUNT_IMR_FLAG         ) { intmask |= UARTBAM_IMR_RXHUNT;         }
   if ( flags & HAL_UART_TXLEV_IMR_FLAG          ) { intmask |= UARTBAM_IMR_TXLEV;          }

   return intmask;
}

/*==================================================================================================

FUNCTION: intmask_to_flags

DESCRIPTION:

==================================================================================================*/
static uint32 intmask_to_flags(uint32 intmask)
{
   uint32 flags = 0;

   if ( intmask & UARTBAM_IMR_WWT            ) { flags |= HAL_UART_WWT_IMR_FLAG;            }
   if ( intmask & UARTBAM_IMR_TXCOMP         ) { flags |= HAL_UART_TXCOMP_IMR_FLAG;         }
   if ( intmask & UARTBAM_IMR_RX_RD_ERROR    ) { flags |= HAL_UART_RX_RD_ERROR_IMR_FLAG;    }
   if ( intmask & UARTBAM_IMR_TX_WR_ERROR    ) { flags |= HAL_UART_TX_WR_ERROR_IMR_FLAG;    }
   if ( intmask & UARTBAM_IMR_PAR_FRAME_ERR  ) { flags |= HAL_UART_PAR_FRAME_ERR_IMR_FLAG;  }
   if ( intmask & UARTBAM_IMR_RXBREAK_END    ) { flags |= HAL_UART_RXBREAK_END_IMR_FLAG;    }
   if ( intmask & UARTBAM_IMR_RXBREAK_START  ) { flags |= HAL_UART_RXBREAK_START_IMR_FLAG;  }
   if ( intmask & UARTBAM_IMR_TX_DONE        ) { flags |= HAL_UART_TX_DONE_IMR_FLAG;        }
   if ( intmask & UARTBAM_IMR_TX_ERROR       ) { flags |= HAL_UART_TX_ERROR_IMR_FLAG;       }
   if ( intmask & UARTBAM_IMR_TX_READY       ) { flags |= HAL_UART_TX_READY_IMR_FLAG;       }
   if ( intmask & UARTBAM_IMR_CUR_CTS        ) { flags |= HAL_UART_CURRENT_CTS_IMR_FLAG;    }
   if ( intmask & UARTBAM_IMR_DELTA_CTS      ) { flags |= HAL_UART_DELTA_CTS_IMR_FLAG;      }
   if ( intmask & UARTBAM_IMR_RXLEV          ) { flags |= HAL_UART_RXLEV_IMR_FLAG;          }
   if ( intmask & UARTBAM_IMR_RXSTALE        ) { flags |= HAL_UART_RXSTALE_IMR_FLAG;        }
   if ( intmask & UARTBAM_IMR_RXBREAK_CHANGE ) { flags |= HAL_UART_RXBREAK_CHANGE_IMR_FLAG; }
   if ( intmask & UARTBAM_IMR_RXHUNT         ) { flags |= HAL_UART_RXHUNT_IMR_FLAG;         }
   if ( intmask & UARTBAM_IMR_TXLEV          ) { flags |= HAL_UART_TXLEV_IMR_FLAG;          }

   return flags;
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetRxEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetRxEnable(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTBAM_CR_ENA_RX : UARTBAM_CR_DIS_RX));
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetTxEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetTxEnable(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTBAM_CR_ENA_TX : UARTBAM_CR_DIS_TX));
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetReceiver

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetReceiver(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_RX);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetTransmitter

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetTransmitter(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_TX);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetErrorStatus

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetErrorStatus(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_ERR);
}

/*==================================================================================================

FUNCTION: HALuart_bam_StartBreak

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_StartBreak(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_STA_BRK);
}

/*==================================================================================================

FUNCTION: HALuart_bam_StopBreak

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_StopBreak(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_STO_BRK);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetCmdSynchronization

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetCmdSynchronization(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTBAM_CR_ENA_CR_PROT : UARTBAM_CR_DIS_CR_PROT));
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetStaleEvent

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetStaleEvent(HALUartObject *uart, uint32 enabled)
{
   REG_OUT(uart->base, CR, (enabled ? UARTBAM_CR_ENA_STALE_EVT : UARTBAM_CR_DIS_STALE_EVT));
}

/*==================================================================================================

FUNCTION: HALuart_bam_EnableInterrupts

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_EnableInterrupts(HALUartObject *uart, uint32 flags)
{
   uint32 imr;
   uint32 intmask = flags_to_intmask(flags);

   REG_IN(uart->base, IMR, &imr);

   if ( (imr & intmask) != intmask )  // check whether interrupts are already enabled
   {
      imr |= intmask;
      REG_OUT(uart->base, IMR, imr);
   }
}

/*==================================================================================================

FUNCTION: HALuart_bam_DisableInterrupts

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_DisableInterrupts(HALUartObject *uart, uint32 flags)
{
   uint32 imr;
   uint32 intmask = flags_to_intmask(flags);

   REG_IN(uart->base, IMR, &imr);

   if (imr & intmask)  // check whether interrupts are already disabled
   {
      imr &= ~intmask;
      REG_OUT(uart->base, IMR, imr);
   }
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetMaskedInterruptStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetMaskedInterruptStatus(HALUartObject *uart)
{
   uint32 misr;

   REG_IN(uart->base, MISR, &misr);
   return intmask_to_flags(misr);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetInterruptStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetInterruptStatus(HALUartObject *uart)
{
   uint32 isr;

   REG_IN(uart->base, ISR, &isr);
   return intmask_to_flags(isr);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetStaleInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetStaleInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_STALE);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetRxBreakChangeInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetRxBreakChangeInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_RXBREAK_CHANGE);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetRxBreakStartInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetRxBreakStartInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_RXBREAK_START);
}

/*==================================================================================================

FUNCTION: HALuart_bam_ResetTxReadyInterrupt

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_ResetTxReadyInterrupt(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_RESET_TX_READY);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetDivider

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetDivider(HALUartObject *uart, uint32 divider)
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

FUNCTION: HALuart_bam_SetCharFormat

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetCharFormat(HALUartObject *uart,
                                      HALUartBitsPerCharType char_len,
                                      HALUartNumStopBitsType stop_bits,
                                      HALUartParityModeType parity)
{
   uint32 mr2;

   REG_IN(uart->base, MR2, &mr2);

   mr2 &= ~UARTBAM_MR2_BITS_PER_CHAR;
   mr2 &= ~UARTBAM_MR2_STOP_BIT_LEN;
   mr2 &= ~UARTBAM_MR2_PARITY_MODE;

   switch (char_len)
   {
      case HAL_UART_5_BITS_PER_CHAR: mr2 |= UARTBAM_MR2_5BPC; break;
      case HAL_UART_6_BITS_PER_CHAR: mr2 |= UARTBAM_MR2_6BPC; break;
      case HAL_UART_7_BITS_PER_CHAR: mr2 |= UARTBAM_MR2_7BPC; break;
      default:
      case HAL_UART_8_BITS_PER_CHAR: mr2 |= UARTBAM_MR2_8BPC; break;
   }

   switch (stop_bits)
   {
      case HAL_UART_0_5_STOP_BITS: mr2 |= UARTBAM_MR2_05SB; break;
      default:
      case HAL_UART_1_0_STOP_BITS: mr2 |= UARTBAM_MR2_1SB;  break;
      case HAL_UART_1_5_STOP_BITS: mr2 |= UARTBAM_MR2_15SB; break;
      case HAL_UART_2_0_STOP_BITS: mr2 |= UARTBAM_MR2_2SB;  break;
   }

   switch (parity)
   {
      default:
      case HAL_UART_NO_PARITY:    mr2 |= UARTBAM_MR2_NOPAR; break;
      case HAL_UART_ODD_PARITY:   mr2 |= UARTBAM_MR2_OPAR;  break;
      case HAL_UART_EVEN_PARITY:  mr2 |= UARTBAM_MR2_EPAR;  break;
      case HAL_UART_SPACE_PARITY: mr2 |= UARTBAM_MR2_SPAR;  break;
   }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetTxFifoSize

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetTxFifoSize(HALUartObject *uart)
{
   uint32 badr;

   REG_IN(uart->base, BADR, &badr);
   return(badr << 2);  // convert words to bytes
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetRxFifoSize

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetRxFifoSize(HALUartObject *uart)
{
   uint32 generics;
   uint32 ram_addr_width;
   uint32 badr;

   // RX FIFO size (in 4-byte words) = 2^RAM_ADDR_WIDTH - RX_BASE_ADDR
   // RAM_ADDR_WIDTH (in 4-byte words) is in the GENERICS register
   // RX_BASE_ADDR (in 4-byte words) is in the BADR register

   REG_IN(uart->base, GENERICS, &generics);                    // Read generics register
   ram_addr_width = generics & UARTBAM_GENERIC_RAM_ADDR_WIDTH; // Mask to get RAM_ADDR_WIDTH
   REG_IN(uart->base, BADR, &badr);

   return(((1 << ram_addr_width) - badr) << 2);  // calculate Rx FIFO size in bytes
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetTxWatermark

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetTxWatermark(HALUartObject *uart, uint32 num_bytes)
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

FUNCTION: HALuart_bam_SetRxWatermark

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetRxWatermark(HALUartObject *uart, uint32 num_bytes)
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

FUNCTION: HALuart_bam_ReadRxFifo

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_ReadRxFifo(HALUartObject *uart, uint8 *ptr, uint32 num_bytes)
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

   words_in_fifo  = (rxfs & UARTBAM_RXFS_FIFO_STATE_MSB_MASK) >> UARTBAM_RXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (rxfs & UARTBAM_RXFS_FIFO_STATE_LSB_MASK) >> UARTBAM_RXFS_FIFO_STATE_LSB_SHIFT;

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

FUNCTION: HALuart_bam_WriteTxFifo

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_WriteTxFifo(HALUartObject *uart, const uint8 *ptr, uint32 num_bytes)
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

   words_in_fifo  = (txfs & UARTBAM_TXFS_FIFO_STATE_MSB_MASK) >> UARTBAM_TXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (txfs & UARTBAM_TXFS_FIFO_STATE_LSB_MASK) >> UARTBAM_TXFS_FIFO_STATE_LSB_SHIFT;
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

FUNCTION: HALuart_bam_SetStaleTimeout

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetStaleTimeout(HALUartObject *uart, uint32 stale_timeout)
{
   uint32 ipr;

   REG_IN(uart->base, IPR, &ipr);

   ipr &= ~UARTBAM_IPR_STALE_TIMEOUT_LSB;
   ipr &= ~UARTBAM_IPR_STALE_TIMEOUT_MSB;

   ipr |= (stale_timeout)      & UARTBAM_IPR_STALE_TIMEOUT_LSB;
   ipr |= (stale_timeout << 2) & UARTBAM_IPR_STALE_TIMEOUT_MSB;

   REG_OUT(uart->base, IPR, ipr);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetStatus

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetStatus(HALUartObject *uart)
{
   uint32 sr;
   uint32 flags = 0;

   REG_IN(uart->base, SR, &sr);

   //  Seems like overkill translating between the HAL layer and the
   //  raw bits.  Maybe we can return the raw bits and put a compile
   //  check that the appropriate raw bit is equal to the appropriate
   //  HAL flag.

   if (sr & UARTBAM_SR_RXEVENT_HIGH)  { flags |= HAL_UART_STATUS_RX_EVENT_HIGH;      }
   if (sr & UARTBAM_SR_RXEVENT_LOW)   { flags |= HAL_UART_STATUS_RX_EVENT_LOW;       }
   if (sr & UARTBAM_SR_TRANS_ACTIVE)  { flags |= HAL_UART_STATUS_TRANS_ACTIVE;       }
   if (sr & UARTBAM_SR_RXBREAK_START) { flags |= HAL_UART_STATUS_RX_BREAK_START;     }
   if (sr & UARTBAM_SR_RXHUNT       ) { flags |= HAL_UART_STATUS_HUNT_CHAR;          }
   if (sr & UARTBAM_SR_RXBREAK      ) { flags |= HAL_UART_STATUS_RX_BREAK;           }
   if (sr & UARTBAM_SR_PF_ERR       ) { flags |= HAL_UART_STATUS_PARITY_FRAME_ERROR; }
   if (sr & UARTBAM_SR_OVR_ERR      ) { flags |= HAL_UART_STATUS_OVERRUN;            }
   if (sr & UARTBAM_SR_TXEMT        ) { flags |= HAL_UART_STATUS_TX_EMPTY;           }
   if (sr & UARTBAM_SR_TXRDY        ) { flags |= HAL_UART_STATUS_TX_READY;           }
   if (sr & UARTBAM_SR_RXFULL       ) { flags |= HAL_UART_STATUS_RX_FULL;            }
   if (sr & UARTBAM_SR_RXRDY        ) { flags |= HAL_UART_STATUS_RX_READY;           }

   return( flags );
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetAutoRfrLevel

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetAutoRfrLevel(HALUartObject *uart, uint32 num_bytes)
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

      mr1 &= ~UARTBAM_MR1_AUTO_RFR_LEVEL0;
      mr1 &= ~UARTBAM_MR1_AUTO_RFR_LEVEL1;

      mr1 |= (num_words)      & UARTBAM_MR1_AUTO_RFR_LEVEL0;
      mr1 |= (num_words << 2) & UARTBAM_MR1_AUTO_RFR_LEVEL1;

      REG_OUT(uart->base, MR1, mr1);
   }
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetRFR

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetRFR(HALUartObject *uart, uint32 on)
{
   REG_OUT(uart->base, CR, (on ? UARTBAM_CR_SET_RFR : UARTBAM_CR_RESET_RFR));
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetAutoInboundFlowCtl

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetAutoInboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uint32 mr1;

   REG_IN(uart->base, MR1, &mr1);

   if (enabled) { mr1 |=  UARTBAM_MR1_RX_RDY_CTL; }
   else         { mr1 &= ~UARTBAM_MR1_RX_RDY_CTL; }

   REG_OUT(uart->base, MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetOutboundFlowCtl

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetOutboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uint32 mr1;

   REG_IN(uart->base, MR1, &mr1);

   if (enabled) { mr1 |=  UARTBAM_MR1_CTS_CTL; }
   else         { mr1 &= ~UARTBAM_MR1_CTS_CTL; }

   REG_OUT(uart->base, MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetIrdaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetIrdaEnable(HALUartObject *uart, uint32 enabled)
{
   uint32 irda;

   // NOTE: This register will not be generated if IRDA_IFC_GEN is not TRUE.  However,
   // in this case it should be harmless to write to it (there will be no effect).

   REG_IN(uart->base, IRDA, &irda);

   if (enabled) { irda |=  UARTBAM_IRDA_IRDA_EN; }
   else         { irda &= ~UARTBAM_IRDA_IRDA_EN; }

   REG_OUT(uart->base, IRDA, irda);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetRxDmaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetRxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uint32 dmen;

   REG_IN(uart->base, DMEN, &dmen);

   if (enable) { dmen |=  UARTBAM_DMEN_RX_BAM_ENABLE; }
   else        { dmen &= ~UARTBAM_DMEN_RX_BAM_ENABLE; }

   REG_OUT(uart->base, DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetTxDmaEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetTxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uint32 dmen;

   REG_IN(uart->base, DMEN, &dmen);

   if (enable) { dmen |=  UARTBAM_DMEN_TX_BAM_ENABLE; }
   else        { dmen &= ~UARTBAM_DMEN_TX_BAM_ENABLE; }

   REG_OUT(uart->base, DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetTxFifoSize

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetTxFifoSize(HALUartObject *uart, uint32 num_bytes)
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

FUNCTION: HALuart_bam_SetDmaNumTxChars

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetDmaNumTxChars(HALUartObject *uart, uint32 num_chars)
{
   REG_OUT(uart->base, NO_CHARS_FOR_TX, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetupDmaRx

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetupDmaRx(HALUartObject *uart, uint32 num_chars)
{
   REG_OUT(uart->base, DMRX, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetDmaRxSnapChars

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetDmaRxSnapChars(HALUartObject *uart)
{
   uint32 rx_total_snap;

   REG_IN(uart->base, RX_TOTAL_SNAP, &rx_total_snap);
   return(rx_total_snap);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetFIFOPhysAddrs

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_GetFIFOPhysAddrs(HALUartObject *uart, void **rx_fifo, void **tx_fifo)
{
   *rx_fifo = uart->physical + RF;
   *tx_fifo = uart->physical + TF;
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetRegisters

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_GetRegisters(HALUartObject *uart, HALUartRegisters *registers)
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
   REG_IN(uart->base, CSR,             &registers->csr);   // BAM only
   REG_IN(uart->base, IMR,             &registers->imr);   // BAM only
   REG_IN(uart->base, IRDA,            &registers->irda);  // BAM only
   REG_IN(uart->base, CLK_CTRL,        &registers->cctl);  // BAM only
   REG_IN(uart->base, BCR,             &registers->bcr);   // BAM only
   REG_IN(uart->base, RX_TRANS_CTRL,   &registers->rctl);  // BAM only
   REG_IN(uart->base, DMRX_DBG,        &registers->ddbg);  // BAM only
   REG_IN(uart->base, FSM_STATUS,      &registers->fsm);   // BAM only
   REG_IN(uart->base, HW_VERSION,      &registers->ver);   // BAM only
   REG_IN(uart->base, GENERICS,        &registers->gen);   // BAM only
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetNullCharOnBreak

DESCRIPTION:
   Sets or clears the RX_BREAK_ZERO_CHAR_OFF bit of UART_DM_MR2. When the bit
   is set, the UART hardware does not pass through the zero character received
   during a break condition.

==================================================================================================*/
static void HALuart_bam_SetNullCharOnBreak(HALUartObject *uart, uint32 enable)
{
   uint32 mr2;

   REG_IN(uart->base, MR2, &mr2);

   if(enable)
   {
      /* Clear the RX_BREAK_ZERO_CHAR_OFF bit to enable the zero character */
      mr2 &= ~UARTBAM_MR2_RX_BK_Z_OFF;
   }
   else
   {
      /* Set the RX_BREAK_ZERO_CHAR_OFF bit to disable the zero character */
      mr2 |= UARTBAM_MR2_RX_BK_Z_OFF;
   }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_bam_LoopbackSet

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_LoopbackSet(HALUartObject *uart, uint32 enabled)
{
   uint32 mr2;
   uint32 flags;

   REG_IN(uart->base, MR2, &mr2);

   //  Loop back both RX/TX and RFR/CTS.

   flags = UARTBAM_MR2_LOOPBACK |
           UARTBAM_MR2_RFR_CTS_LOOPBACK;

   if (enabled) { mr2 |=  flags; }
   else         { mr2 &= ~flags; }

   REG_OUT(uart->base, MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetCxmEnable

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetCxmEnable(HALUartObject *uart, HALUartCxmConfigType cxm_cfg, uint32 sam)
{
   uint32 cxm_config, cxm_tx_config;

   if(cxm_cfg == HAL_UART_CXM_ENABLE)
   {
      REG_IN(uart->base, CXM_TX_CONFIG, &cxm_tx_config);

      cxm_tx_config &= ~UARTBAM_CXM_TX_CONFIG_SAM_MASK;
      cxm_tx_config |= (sam << UARTBAM_CXM_TX_CONFIG_SAM_SHIFT) & UARTBAM_CXM_TX_CONFIG_SAM_MASK;

      REG_OUT(uart->base, CXM_TX_CONFIG, cxm_tx_config);
   }

   REG_IN(uart->base, CXM_CONFIG, &cxm_config);

   switch(cxm_cfg)
   {
      case HAL_UART_CXM_DISABLE:
         /* Disable the UART */
         cxm_config &= ~UARTBAM_CXM_CONFIG_BYPASS_CXM;
         cxm_config &= ~UARTBAM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      case HAL_UART_CXM_BYPASS:
         /* Bypass CXM entirely, placing the UART hardware in traditional UART
          * mode, by setting the BYPASS_CXM bit. */
         cxm_config |=  UARTBAM_CXM_CONFIG_BYPASS_CXM;
         cxm_config &=  ~UARTBAM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      case HAL_UART_CXM_ENABLE:
         /* Enable CXM */
         cxm_config &= ~UARTBAM_CXM_CONFIG_BYPASS_CXM;
         cxm_config |=  UARTBAM_CXM_CONFIG_TWO_WIRE_MODE;
         break;

      default:
         break;
   }

   REG_OUT(uart->base, CXM_CONFIG, cxm_config);
}

/*==================================================================================================

FUNCTION: HALuart_bam_StartRxSideband

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_StartRxSideband(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_START_RX_BAM_IFC);
}

/*==================================================================================================

FUNCTION: HALuart_bam_StartTxSideband

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_StartTxSideband(HALUartObject *uart)
{
   REG_OUT(uart->base, CR, UARTBAM_CR_START_TX_BAM_IFC);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetAutoReactivateRx

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetAutoReactivateRx(HALUartObject *uart, uint32 enabled)
{
   uint32 rx_trans_ctrl = 0;

   if (enabled)
   {
      rx_trans_ctrl = UARTBAM_RXCTL_DMRX_CYCLIC |
                      UARTBAM_RXCTL_AUTO_RX     |
                      UARTBAM_RXCTL_AUTO_STALE;
   }

   REG_OUT(uart->base, RX_TRANS_CTRL, rx_trans_ctrl);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetDmaBurst

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetDmaBurst(HALUartObject *uart, uint32 num_bytes)
{
   // Note that RFWR is used for two purposes on BAM UARTs.  In PIO mode it controls
   // when the RXLEV bit is set (the number of words in the RXFIFO is greater than RFWR).
   // In DMA mode it controls when the BAM sideband signal "data_avail_req" is
   // asserted (the number of words in the RXFIFO is greater or equal to RFWR).
   // TFWR is used in the same manner for TX transfers.

   if ( num_bytes & 0x03 )  // must be a multiple of 4 bytes
   {
      uart->last_error = HAL_UART_BAD_PARAMETER;
   }
   else
   {
      REG_OUT(uart->base, RFWR, num_bytes >> 2);  // RFWR expressed in 4-byte words
      REG_OUT(uart->base, TFWR, num_bytes >> 2);  // RFWR expressed in 4-byte words
   }
}

/*==================================================================================================

FUNCTION: HALuart_bam_EnableHwFixes

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_EnableHwFixes(HALUartObject *uart, uint32 enabled)
{
   uint32 bcr = 0;

   if (enabled)
   {
      bcr = UARTBAM_BCR_DMRX_1BYTE_RES   |
            UARTBAM_BCR_DMRX_FORCE_STALE |
            UARTBAM_BCR_DMRX_LOW_EN      |
            UARTBAM_BCR_STALE_IRQ_EMPTY  |
            UARTBAM_BCR_TX_BREAK_DISABLE;
   }

   REG_OUT(uart->base, BCR, bcr);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetType0WakeupClear

DESCRIPTION:

==================================================================================================*/
static void HALuart_bam_SetType0WakeupClear(HALUartObject *uart, uint32 clear)
{
   uint32 cxm_tx_config;

   REG_IN(uart->base, CXM_TX_CONFIG, &cxm_tx_config);

   if(clear)
   {
      cxm_tx_config |=  UARTBAM_CXM_TX_CONFIG_T0_WU_CLR;
   }
   else
   {
      cxm_tx_config &= ~UARTBAM_CXM_TX_CONFIG_T0_WU_CLR;
   }

   REG_OUT(uart->base, CXM_TX_CONFIG, cxm_tx_config);
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetCxmTxSticky

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_GetCxmTxSticky(HALUartObject *uart, uint32 clear)
{
   uint32 tx_sticky;

   REG_IN(uart->base, CXM_RX_STAT, &tx_sticky);

   if(clear && (tx_sticky & UARTBAM_CXM_RX_STAT_WLAN_STICKY))
   {
      REG_OUT(uart->base, CXM_RX_STAT, UARTBAM_CXM_RX_STAT_WLAN_STICKY);
   }

   return tx_sticky & UARTBAM_CXM_RX_STAT_WLAN_STICKY;
}

/*==================================================================================================

FUNCTION: HALuart_bam_CxmTxDirectChar

DESCRIPTION:

==================================================================================================*/
static uint32 HALuart_bam_CxmTxDirectChar(HALUartObject *uart, uint8 character)
{
   uint32 cxm_tx_config, cxm_config, cxm_dir_uart_tr, clk_ctrl;
   uint8 tx_clk_open_already_set = 0;
   int cnt = 10000;

   REG_IN(uart->base, CXM_TX_CONFIG, &cxm_tx_config);
   REG_IN(uart->base, CXM_CONFIG, &cxm_config);

   /* Ensure TYPE0_WAKEUP_CLEAR and BYPASS_CXM bits are clear*/
   if ((cxm_tx_config & UARTBAM_CXM_TX_CONFIG_T0_WU_CLR)
        || (cxm_config & UARTBAM_CXM_CONFIG_BYPASS_CXM))
   {
      return 0;
   }

   REG_IN(uart->base, CXM_DIR_UART_TR, &cxm_dir_uart_tr);

   /* Bail out if the previous transaction is not finished.
      Should not happen since it is synchronous */
   if (cxm_dir_uart_tr & UARTBAM_CXM_DIR_UART_TR_DIR_UART_TRIG)
   {
      return 0;
   }

   REG_IN(uart->base, CLK_CTRL, &clk_ctrl);

   if (clk_ctrl & UARTBAM_CXM_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN) 
   {
      tx_clk_open_already_set = 1;
   }
   else 
   {
      clk_ctrl |= UARTBAM_CXM_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN;
      REG_OUT(uart->base, CLK_CTRL, clk_ctrl);
   }

   /* Clear the previous TX byte first, write the new byte and send it out with trigger bit*/
   cxm_dir_uart_tr &= ~UARTBAM_CXM_DIR_UART_TR_DIR_UART_COUNT_MASK;
   cxm_dir_uart_tr |= (character << 8);
   cxm_dir_uart_tr |= UARTBAM_CXM_DIR_UART_TR_DIR_UART_TRIG;
   REG_OUT(uart->base, CXM_DIR_UART_TR, cxm_dir_uart_tr);

   do 
   {
      REG_IN(uart->base, CXM_DIR_UART_TR, &cxm_dir_uart_tr);
      cnt--;
   } while ((cxm_dir_uart_tr & UARTBAM_CXM_DIR_UART_TR_DIR_UART_TRIG) && (cnt > 0));

   /* only put it back to its original state if we change it */
   if (tx_clk_open_already_set == 0) 
   {
      clk_ctrl &= ~UARTBAM_CXM_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN;
      REG_OUT(uart->base, CLK_CTRL, clk_ctrl);
   }

   if (cnt > 0) 
   {
      return 1;
   }

   return 0;
}

/*==================================================================================================

FUNCTION: HALuart_bam_GetDmaRx

DESCRIPTION:
   
==================================================================================================*/
static uint32 HALuart_bam_GetDmaRx(HALUartObject *uart)
{
   uint32 num_chars;

   REG_IN(uart->base, DMRX, &num_chars);
   return(num_chars);
}
