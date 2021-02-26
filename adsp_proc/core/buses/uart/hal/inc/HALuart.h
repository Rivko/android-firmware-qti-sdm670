#ifndef HALUART_H
#define HALUART_H
/*==================================================================================================

FILE: HALuart.h

DESCRIPTION: UART HAL (HW abstraction layer)

                           Copyright (c) 2008-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/hal/inc/HALuart.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
/*==================================================================================================
                                             DATATYPES
==================================================================================================*/

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _BOOLEAN_DEFINED
typedef unsigned char  boolean;  // must match definition in comdef.h
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef unsigned char  uint8;    // must match definition in DALStdDef.h
#define _UINT8_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef unsigned short uint16;   // must match definition in DALStdDef.h
#define _UINT16_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned long  uint32;   // must match definition in DALStdDef.h
#define _UINT32_DEFINED
#endif

/*==================================================================================================
                                            ENUMERATIONS
==================================================================================================*/

typedef enum
{
   HAL_UART_DM,
   HAL_UART_BAM,
} HALUartType;

typedef enum
{
   HAL_UART_5_BITS_PER_CHAR = (1<<0),
   HAL_UART_6_BITS_PER_CHAR = (1<<1),
   HAL_UART_7_BITS_PER_CHAR = (1<<2),
   HAL_UART_8_BITS_PER_CHAR = (1<<3),
} HALUartBitsPerCharType;

typedef enum
{
   HAL_UART_CXM_DISABLE,
   HAL_UART_CXM_BYPASS,
   HAL_UART_CXM_ENABLE,
} HALUartCxmConfigType;

typedef enum
{
   HAL_UART_0_5_STOP_BITS = (1<<0),
   HAL_UART_1_0_STOP_BITS = (1<<1),
   HAL_UART_1_5_STOP_BITS = (1<<2),
   HAL_UART_2_0_STOP_BITS = (1<<3),
} HALUartNumStopBitsType;

typedef enum
{
   HAL_UART_NO_PARITY    = (1<<0),
   HAL_UART_ODD_PARITY   = (1<<1),
   HAL_UART_EVEN_PARITY  = (1<<2),
   HAL_UART_SPACE_PARITY = (1<<3),
} HALUartParityModeType;

typedef enum
{
   HAL_UART_WWT_IMR_FLAG            = (1<<16),
   HAL_UART_TXCOMP_IMR_FLAG         = (1<<15),
   HAL_UART_RX_RD_ERROR_IMR_FLAG    = (1<<14),
   HAL_UART_TX_WR_ERROR_IMR_FLAG    = (1<<13),
   HAL_UART_PAR_FRAME_ERR_IMR_FLAG  = (1<<12),
   HAL_UART_RXBREAK_END_IMR_FLAG    = (1<<11),
   HAL_UART_RXBREAK_START_IMR_FLAG  = (1<<10),
   HAL_UART_TX_DONE_IMR_FLAG        = (1<<9),
   HAL_UART_TX_ERROR_IMR_FLAG       = (1<<8),
   HAL_UART_TX_READY_IMR_FLAG       = (1<<7),
   HAL_UART_CURRENT_CTS_IMR_FLAG    = (1<<6),
   HAL_UART_DELTA_CTS_IMR_FLAG      = (1<<5),
   HAL_UART_RXLEV_IMR_FLAG          = (1<<4),
   HAL_UART_RXSTALE_IMR_FLAG        = (1<<3),
   HAL_UART_RXBREAK_CHANGE_IMR_FLAG = (1<<2),
   HAL_UART_RXHUNT_IMR_FLAG         = (1<<1),
   HAL_UART_TXLEV_IMR_FLAG          = (1<<0),
} HALUartInterruptMaskRegisterFlags;

typedef enum
{
   HAL_UART_STATUS_RX_EVENT_HIGH      = (1<<11),
   HAL_UART_STATUS_RX_EVENT_LOW       = (1<<10),
   HAL_UART_STATUS_TRANS_ACTIVE       = (1<<9),
   HAL_UART_STATUS_RX_BREAK_START     = (1<<8),
   HAL_UART_STATUS_HUNT_CHAR          = (1<<7),
   HAL_UART_STATUS_RX_BREAK           = (1<<6),
   HAL_UART_STATUS_PARITY_FRAME_ERROR = (1<<5),
   HAL_UART_STATUS_OVERRUN            = (1<<4),
   HAL_UART_STATUS_TX_EMPTY           = (1<<3),
   HAL_UART_STATUS_TX_READY           = (1<<2),
   HAL_UART_STATUS_RX_FULL            = (1<<1),
   HAL_UART_STATUS_RX_READY           = (1<<0),
} HALUartStatusType;

typedef enum
{
   HAL_UART_SUCCESS,
   HAL_UART_NOT_SUPPORTED,
   HAL_UART_BAD_PARAMETER,
} HALUartErrorType;

/*==================================================================================================
                                             STRUCTURES
==================================================================================================*/

typedef struct HALUartFcnTable HALUartFcnTable;  // forward declaration due to circular reference
typedef struct HALUartObject HALUartObject;

struct HALUartObject
{
   const HALUartFcnTable *fcn_table;   // table of function pointers specific to UART type
   uint8                 *base;        // base of UART register block (virtual address)
   uint8                 *physical;    // base of UART register block (physical address)
   HALUartErrorType      last_error;   // error status of most recent HAL API call
   uint32                imr_shadow;   // shadow for write-only register requiring masked writes
   uint32                irda_shadow;  // shadow for write-only register requiring masked writes
};

typedef struct
{
   uint32  mr1;
   uint32  mr2;
   uint32  sr;
   uint32  misr;
   uint32  isr;
   uint32  ipr;
   uint32  tfwr;
   uint32  rfwr;
   uint32  hcr;
   uint32  dmrx;
   uint32  snap;
   uint32  dmen;
   uint32  ncft;
   uint32  badr;
   uint32  txfs;
   uint32  rxfs;
   uint32  sim;
   uint32  csr;   // BAM only
   uint32  imr;   // BAM only
   uint32  irda;  // BAM only
   uint32  cctl;  // BAM only
   uint32  bcr;   // BAM only
   uint32  rctl;  // BAM only
   uint32  ddbg;  // BAM only
   uint32  fsm;   // BAM only
   uint32  ver;   // BAM only
   uint32  gen;   // BAM only
} HALUartRegisters;

struct HALUartFcnTable
{
   void   (*SetRxEnable)                (HALUartObject *uart, uint32 enabled);
   void   (*SetTxEnable)                (HALUartObject *uart, uint32 enabled);
   void   (*ResetReceiver)              (HALUartObject *uart);
   void   (*ResetTransmitter)           (HALUartObject *uart);
   void   (*ResetErrorStatus)           (HALUartObject *uart);
   void   (*StartBreak)                 (HALUartObject *uart);
   void   (*StopBreak)                  (HALUartObject *uart);
   void   (*SetCmdSynchronization)      (HALUartObject *uart, uint32 enabled);
   void   (*SetStaleEvent)              (HALUartObject *uart, uint32 enabled);
   void   (*EnableInterrupts)           (HALUartObject *uart, uint32 flags);
   void   (*DisableInterrupts)          (HALUartObject *uart, uint32 flags);
   uint32 (*GetMaskedInterruptStatus)   (HALUartObject *uart);
   uint32 (*GetInterruptStatus)         (HALUartObject *uart);
   void   (*ResetStaleInterrupt)        (HALUartObject *uart);
   void   (*ResetRxBreakChangeInterrupt)(HALUartObject *uart);
   void   (*ResetRxBreakStartInterrupt) (HALUartObject *uart);
   void   (*ResetTxReadyInterrupt)      (HALUartObject *uart);
   void   (*SetDivider)                 (HALUartObject *uart, uint32 divider);
   void   (*SetCharFormat)              (HALUartObject *uart, HALUartBitsPerCharType char_len,
                                                              HALUartNumStopBitsType stop_bits,
                                                              HALUartParityModeType parity);
   uint32 (*GetTxFifoSize)              (HALUartObject *uart);
   uint32 (*GetRxFifoSize)              (HALUartObject *uart);
   void   (*SetTxWatermark)             (HALUartObject *uart, uint32 num_bytes);
   void   (*SetRxWatermark)             (HALUartObject *uart, uint32 num_bytes);
   uint32 (*ReadRxFifo)                 (HALUartObject *uart, uint8 *ptr,
                                                              uint32 num_bytes);
   uint32 (*WriteTxFifo)                (HALUartObject *uart, const uint8 *ptr,
                                                              uint32 num_bytes);
   void   (*SetStaleTimeout)            (HALUartObject *uart, uint32 stale_timeout);
   uint32 (*GetStatus)                  (HALUartObject *uart);
   void   (*SetAutoRfrLevel)            (HALUartObject *uart, uint32 num_bytes);
   void   (*SetRFR)                     (HALUartObject *uart, uint32 on);
   void   (*SetAutoInboundFlowCtl)      (HALUartObject *uart, uint32 enabled);
   void   (*SetOutboundFlowCtl)         (HALUartObject *uart, uint32 enabled);
   void   (*SetIrdaEnable)              (HALUartObject *uart, uint32 enabled);
   void   (*SetRxDmaEnable)             (HALUartObject *uart, uint32 enable);
   void   (*SetTxDmaEnable)             (HALUartObject *uart, uint32 enable);
   void   (*SetTxFifoSize)              (HALUartObject *uart, uint32 num_bytes);
   void   (*SetDmaNumTxChars)           (HALUartObject *uart, uint32 num_chars);
   void   (*SetupDmaRx)                 (HALUartObject *uart, uint32 num_chars);
   uint32 (*GetDmaRxSnapChars)          (HALUartObject *uart);
   void   (*GetFIFOPhysAddrs)           (HALUartObject *uart, void **rx_fifo, void **tx_fifo);
   void   (*GetRegisters)               (HALUartObject *uart, HALUartRegisters *registers);
   void   (*SetNullCharOnBreak)         (HALUartObject *uart, uint32 enable);
   void   (*LoopbackSet)                (HALUartObject *uart, uint32 enabled);
   void   (*SetCxmEnable)               (HALUartObject *uart, HALUartCxmConfigType cxm_cfg,
                                                              uint32 sam);
   void   (*StartRxSideband)            (HALUartObject *uart);
   void   (*StartTxSideband)            (HALUartObject *uart);
   void   (*SetAutoReactivateRx)        (HALUartObject *uart, uint32 enabled);
   void   (*SetDmaBurst)                (HALUartObject *uart, uint32 num_bytes);
   void   (*EnableHwFixes)              (HALUartObject *uart, uint32 enabled);
   void   (*SetType0WakeupClear)        (HALUartObject *uart, uint32 clear);
   uint32 (*GetCxmTxSticky)             (HALUartObject *uart, uint32 clear);
   uint32 (*CxmTxDirectChar)            (HALUartObject *uart, uint8 character);
   uint32 (*GetDmaRx)                   (HALUartObject *uart);
};

/*==================================================================================================
                                        EXTERN DECLARATIONS
==================================================================================================*/

// These external symbols will be defined in the UART type specific modules but they need
// to be declared here before using them in the inline functions below.

extern const HALUartFcnTable functions_uart_dm;
extern const HALUartFcnTable functions_uart_bam;

/*==================================================================================================
                                          INLINE FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: HALuart_SetRxEnable

DESCRIPTION:
   Enables or disables the channel receiver according to the boolean "enabled"
   by writing the proper value to the UART command register bits 0 and 1.

==================================================================================================*/
static __inline
void HALuart_SetRxEnable(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetRxEnable(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxEnable

DESCRIPTION:
   Enables or disables the channel transmitter according to boolean "enabled"
   by writing the proper value to the UART command register bits 2 and 3.
   On disabling terminates the operation of the transmitter after any character
   in the transmit shift register is sent.

==================================================================================================*/
static __inline
void HALuart_SetTxEnable(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetTxEnable(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_ResetReceiver

DESCRIPTION:
   Resets the receiver as if a hardware reset were issued. The receiver is
   disabled and the FIFO, packing buffer and shift registers are flushed.

==================================================================================================*/
static __inline
void HALuart_ResetReceiver(HALUartObject *uart)
{
   uart->fcn_table->ResetReceiver(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetTransmitter

DESCRIPTION:
   Resets the transmitter as if a hardware reset were issued. The transmitter
   signal goes high (marking) and the FIFO, unpacking register and shift
   register are flushed

==================================================================================================*/
static __inline
void HALuart_ResetTransmitter(HALUartObject *uart)
{
   uart->fcn_table->ResetTransmitter(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetErrorStatus

DESCRIPTION:
   Clears the overrun error and hunt char received status bits in both the
   character and block error modes. In the block error mode, it clears the
   error status and received break.

==================================================================================================*/
static __inline
void HALuart_ResetErrorStatus(HALUartObject *uart)
{
   uart->fcn_table->ResetErrorStatus(uart);
}

/*==================================================================================================

FUNCTION: HALuart_StartBreak

DESCRIPTION:
   This API forces the transmitter signal low. The transmitter must be enabled.
   If the transmitter is busy, the break is started when all characters in the
   transmit FIFO and the transmit shift register have been completely sent.

==================================================================================================*/
static __inline
void HALuart_StartBreak(HALUartObject *uart)
{
   uart->fcn_table->StartBreak(uart);
}

/*==================================================================================================

FUNCTION: HALuart_StopBreak

DESCRIPTION:
   If executed while the transmit channel is breaking, this command causes the
   transmitter signal to go high. The signal remains high for at least one bit
   time before sending out a new character.

==================================================================================================*/
static __inline
void HALuart_StopBreak(HALUartObject *uart)
{
   uart->fcn_table->StopBreak(uart);
}

/*==================================================================================================

FUNCTION: HALuart_SetCmdSynchronization

DESCRIPTION:
   This API enables or disables command register hardware protection according
   to parameter "enabled". When on and two consecutive writes to the command
   register are detected, the second write is delayed until the first command
   is finished. The delay is done by de-asserting the AHB ready and this ensures
   that the first command completes and the second one will be executed right
   afterward. When off, software is responsible for managing delay between
   writes to the command register.

==================================================================================================*/
static __inline
void HALuart_SetCmdSynchronization(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetCmdSynchronization(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetStaleEvent

DESCRIPTION:
   This API enable or disables the receive stale event mechanism according to
   the value of "enabled".

==================================================================================================*/
static __inline
void HALuart_SetStaleEvent(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetStaleEvent(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_EnableInterrupts

DESCRIPTION:
   This function turns the input bits on and preserves the other bits in the mask.

==================================================================================================*/
static __inline
void HALuart_EnableInterrupts(HALUartObject *uart, uint32 flags)
{
   uart->fcn_table->EnableInterrupts(uart, flags);
}

/*==================================================================================================

FUNCTION: HALuart_DisableInterrupts

DESCRIPTION:
   The analog of EnableInterrupts.  The input bits are turned off, all other bits are preserved.

==================================================================================================*/
static __inline
void HALuart_DisableInterrupts(HALUartObject *uart, uint32 flags)
{
   uart->fcn_table->DisableInterrupts(uart, flags);
}

/*==================================================================================================

FUNCTION: HALuart_GetMaskedInterruptStatus

DESCRIPTION:
   Returns the "AND" of the interrupt status and the set interrupt mask

==================================================================================================*/
static __inline
uint32 HALuart_GetMaskedInterruptStatus(HALUartObject *uart)
{
   return uart->fcn_table->GetMaskedInterruptStatus(uart);
}

/*==================================================================================================

FUNCTION: HALuart_GetInterruptStatus

DESCRIPTION:
   Provides the current status of the possible interrupt conditions.

==================================================================================================*/
static __inline
uint32 HALuart_GetInterruptStatus(HALUartObject *uart)
{
   return uart->fcn_table->GetInterruptStatus(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetStaleInterrupt

DESCRIPTION:
   Uses channel command to clear the stale receive data interrupt.

==================================================================================================*/
static __inline
void HALuart_ResetStaleInterrupt(HALUartObject *uart)
{
   uart->fcn_table->ResetStaleInterrupt(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetRxBreakChangeInterrupt

DESCRIPTION:
   Uses channel command to clear the RX break change interrupt.

==================================================================================================*/
static __inline
void HALuart_ResetRxBreakChangeInterrupt(HALUartObject *uart)
{
   uart->fcn_table->ResetRxBreakChangeInterrupt(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetRxBreakStartInterrupt

DESCRIPTION:
   Uses channel command to clear the RX break start interrupt.

==================================================================================================*/
static __inline
void HALuart_ResetRxBreakStartInterrupt(HALUartObject *uart)
{
   uart->fcn_table->ResetRxBreakStartInterrupt(uart);
}

/*==================================================================================================

FUNCTION: HALuart_ResetTxReadyInterrupt

DESCRIPTION:
   Uses general command to clear the TX_READY interrupt.

==================================================================================================*/
static __inline
void HALuart_ResetTxReadyInterrupt(HALUartObject *uart)
{
   uart->fcn_table->ResetTxReadyInterrupt(uart);
}

/*==================================================================================================

FUNCTION: HALuart_SetDivider

DESCRIPTION:
   Sets the divider for the transmit/receive baud rates.  The hardware provides
   the ability to set the transmit and receive baud rates differently but this
   API currently forces you to set them to be the same.

==================================================================================================*/
static __inline
void HALuart_SetDivider(HALUartObject *uart, uint32 divider)
{
   uart->fcn_table->SetDivider(uart, divider);
}

/*==================================================================================================

FUNCTION: HALuart_SetCharFormat

DESCRIPTION:
   Sets how many bits are transmitted or received per character,
   the duration of the stop bit that is appended to each transmitted character,
   and which parity mode is used.

==================================================================================================*/
static __inline
void HALuart_SetCharFormat(HALUartObject *uart,
                           HALUartBitsPerCharType char_len,
                           HALUartNumStopBitsType stop_bits,
                           HALUartParityModeType parity)
{
   uart->fcn_table->SetCharFormat(uart, char_len, stop_bits, parity);
}

/*==================================================================================================

FUNCTION: HALuart_GetTxFifoSize

DESCRIPTION:
   Returns the size (in bytes) of the TX FIFO

==================================================================================================*/
static __inline
uint32 HALuart_GetTxFifoSize(HALUartObject *uart)
{
   return uart->fcn_table->GetTxFifoSize(uart);
}

/*==================================================================================================

FUNCTION: HALuart_GetRxFifoSize

DESCRIPTION:
   Returns the size (in bytes) of the RX FIFO

==================================================================================================*/
static __inline
uint32 HALuart_GetRxFifoSize(HALUartObject *uart)
{
   return uart->fcn_table->GetRxFifoSize(uart);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxWatermark

DESCRIPTION:
   This API sets a value that determines the level of the transmit FIFO at
   which an interrupt is generated. The interrupt is generated when the
   number of bytes in the TX FIFO is less than or equal to num_bytes.

==================================================================================================*/
static __inline
void HALuart_SetTxWatermark(HALUartObject *uart, uint32 num_bytes)
{
   uart->fcn_table->SetTxWatermark(uart, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_SetRxWatermark

DESCRIPTION:
   This API sets a value that determines the level of the receive FIFO at
   which an interrupt is generated. The interrupt is generated when the
   number of bytes in the RX FIFO is greater than or equal to num_bytes.

==================================================================================================*/
static __inline
void HALuart_SetRxWatermark(HALUartObject *uart, uint32 num_bytes)
{
   uart->fcn_table->SetRxWatermark(uart, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_ReadRxFifo

DESCRIPTION:
   This API is used to copy data from the RX FIFO to a RAM buffer.
   This function will copy as much data as possible without overflowing
   the RAM buffer.

==================================================================================================*/
static __inline
uint32 HALuart_ReadRxFifo(HALUartObject *uart, uint8 *ptr, uint32 num_bytes)
{
   return uart->fcn_table->ReadRxFifo(uart, ptr, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_WriteTxFifo

DESCRIPTION:
   This API is used to copy data from a RAM buffer to the TX FIFO.
   This function will copy as much data as possible without overrunning
   the FIFO.  It will return the number of bytes copied.

==================================================================================================*/
static __inline
uint32 HALuart_WriteTxFifo(HALUartObject *uart, const uint8 *ptr, uint32 num_bytes)
{
   return uart->fcn_table->WriteTxFifo(uart, ptr, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_SetStaleTimeout

DESCRIPTION:
   This API sets the number of character times that may elapse without data
   received before a 'stale event' is generated. The character time is defined
   as 10 times the bit rate. "stale_timeout" may assume a value of 1 to
   2^(RAM_ADDR_WIDTH + 2) -1.

==================================================================================================*/
static __inline
void HALuart_SetStaleTimeout(HALUartObject *uart, uint32 stale_timeout)
{
   uart->fcn_table->SetStaleTimeout(uart, stale_timeout);
}

/*==================================================================================================

FUNCTION: HALuart_GetStatus

DESCRIPTION:
   Gets the state of the UART status register (SR).  This register is used to
   obtain the current state of the UART subsystem.  This register is updated
   asynchronously.

==================================================================================================*/
static __inline
uint32 HALuart_GetStatus(HALUartObject *uart)
{
   return uart->fcn_table->GetStatus(uart);
}

/*==================================================================================================

FUNCTION: HALuart_SetAutoRfrLevel

DESCRIPTION:
   Programs the level in the receive FIFO at which the RFR_N signal will be
   deasserted, if programmed to do so (see RX_RDY_CTL field of this register).
   The level counts the number of bytes inside the RX FIFO. It doesn't count
   the character that is being received (shift register) or characters in the
   packing buffer. This value is programmed from 1 to 2^RAM_ADDR_WIDTH.
   The RFR_N signal is deasserted when the RX FIFO level (the number of
   characters remaining in the RX FIFO) is greater than the level that is
   programmed into this register.

==================================================================================================*/
static __inline
void HALuart_SetAutoRfrLevel(HALUartObject *uart, uint32 num_bytes)
{
   uart->fcn_table->SetAutoRfrLevel(uart, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_SetRFR

DESCRIPTION:
   When the automatic ready-for-receive feature is turned off by
   HALuart_SetAutoInboundFlowCtl, the RFR_N signal can be used by normal
   signal port bit manipulation via this API.

==================================================================================================*/
static __inline
void HALuart_SetRFR(HALUartObject *uart, uint32 on)
{
   uart->fcn_table->SetRFR(uart, on);
}

/*==================================================================================================

FUNCTION: HALuart_SetAutoInboundFlowCtl

DESCRIPTION:
   This API sets the current setting for the automatic ready-for-receiving
   (RFR_N) control for the receiver. If RFR_N is set (1), due to a valid
   start bit received when the channel FIFO is at the level programmed by
   HALuart_SetAutoRfrLevel then TRUE is returned. Else, if the FIFO level
   has fallen below the programmed level, the RFR_N signal is turned on, or
   cleared (0) and FALSE is returned. When this feature is off, the
   RFR_N signal can be used by normal signal port bit manipulation by
   HALuart_SetRFR.

==================================================================================================*/
static __inline
void HALuart_SetAutoInboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetAutoInboundFlowCtl(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetOutboundFlowCtl

DESCRIPTION:
   When set bFlowEnabled TRUE), the transmitter checks the CTS_N input to
   determine whether to begin transmission of a new character. If CTS_N is
   low, the character is sent. Otherwise the transmitter continues marking
   until CTS_N goes low, then the next character is transmitted. A change on
   CTS_N during the transmission of a character has no effect on that
   character. When clear (bFlowEnabled FALSE), the CTS_N input for the channel
   has no effect on the transmitter.

==================================================================================================*/
static __inline
void HALuart_SetOutboundFlowCtl(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetOutboundFlowCtl(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetIrdaEnable

DESCRIPTION:
   This API enables or disables the IRDA transceiver. If "enabled" is set FALSE
   (0) the other IRDA settings are ignored.

==================================================================================================*/
static __inline
void HALuart_SetIrdaEnable(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetIrdaEnable(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetRxDmaEnable

DESCRIPTION:
   Enables or disables data mover for receive.

==================================================================================================*/
static __inline
void HALuart_SetRxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uart->fcn_table->SetRxDmaEnable(uart, enable);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxDmaEnable

DESCRIPTION:
   Enables or disables data mover for transmit.

==================================================================================================*/
static __inline
void HALuart_SetTxDmaEnable(HALUartObject *uart, uint32 enable)
{
   uart->fcn_table->SetTxDmaEnable(uart, enable);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxFifoSize

DESCRIPTION:
   On UARTDM both FIFOs use the same RAM (2^RAM_ADDR_WIDTH 32-bit entries)
   and this function is used to set the division of memory between the RXFIFO
   and the TXFIFO.  The size of the TXFIFO is determined by the num_bytes
   parameter (which must be a multiple of 16 bytes due to the length of a
   DMOV burst).  The size of the RXFIFO is the remaining RAM.

   On legacy UART this is a no-op because the TXFIFO and RXFIFO are fixed
   in size (and equal in size).

==================================================================================================*/
static __inline
void HALuart_SetTxFifoSize(HALUartObject *uart, uint32 num_bytes)
{
   uart->fcn_table->SetTxFifoSize(uart, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_SetDmaNumTxChars

DESCRIPTION:
   This API sets the total number of characters for transmission. Before
   writing a new value, the TX FIFO must be empty (as indicated by
   HAL_UART_TX_READY_IMR_FLAG set in result of
   HALuart_GetInterruptStatus or HALuart_GetMaskedInterruptStatus or
   after a reset). It is used by the transmitter to calculate how many
   characters to transmit in the last word. In DM mode it is also used for the
   CRCI mechanism. Any additional writes to the TX FIFO above the quantity set
   by this API will be discarded. The transmit-ready interrupt indicated by
   HAL_UART_TX_READY_IMR_FLAG is triggered after the quantity of characters
   set by this API is moved from the TX FIFO to the unpacking register.

==================================================================================================*/
static __inline
void HALuart_SetDmaNumTxChars(HALUartObject *uart, uint32 num_chars)
{
   uart->fcn_table->SetDmaNumTxChars(uart, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_SetupDmaRx

DESCRIPTION:
   In the DM mode, this API sets the number of chars in the RX FIFO that will
   be used for CRCI handshake in the next interaction with the DM. The written
   value of "num_chars" is forced to be a multiple of 16(bits [3:0] are treated
   as 0x0). After a value is written, the UART will generate CRCI requests as
   long as the value set by "num_chars" remains non-zero. Read of DMRX register
   gives the number of characters that were received since the end of the last
   transfer. It is reset at the end of each Rx transfer and is also used by
   the software to indicate 'transfer initialization'. See Software procedures.

==================================================================================================*/
static __inline
void HALuart_SetupDmaRx(HALUartObject *uart, uint32 num_chars)
{
   uart->fcn_table->SetupDmaRx(uart, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_GetDmaRxSnapChars

DESCRIPTION:
   Returns the number of characters received since the end of the last Rx
   transfer.  It is updated at the end of an Rx transfer.

   Rx tansfer ends when one of the conditions is met:

   1) The number of characters which were received since the end of the
      previous trnasfer equals the value which was written to the DMRX
      (HALuart_SetupDmaRx) at "Transfer initialization".

   2) A stale event occurred (flush operation already performed if was needed).

==================================================================================================*/
static __inline
uint32 HALuart_GetDmaRxSnapChars(HALUartObject *uart)
{
   return uart->fcn_table->GetDmaRxSnapChars(uart);
}

/*==================================================================================================

FUNCTION: HALuart_GetFIFOPhysAddrs

DESCRIPTION:
   Returns the physical addresses of RX and TX FIFOs required for DMA interaction.

==================================================================================================*/
static __inline
void HALuart_GetFIFOPhysAddrs(HALUartObject *uart, void **rx_fifo, void **tx_fifo)
{
   uart->fcn_table->GetFIFOPhysAddrs(uart, rx_fifo, tx_fifo);
}

/*==================================================================================================

FUNCTION: HALuart_GetRegisters

DESCRIPTION:
   Debug function.  Get the raw value from the registers without any processing.

==================================================================================================*/
static __inline
void HALuart_GetRegisters(HALUartObject *uart, HALUartRegisters *registers)
{
   uart->fcn_table->GetRegisters(uart, registers);
}

/*==================================================================================================

FUNCTION: HALuart_SetNullCharOnBreak

DESCRIPTION:
   When enable is FALSE, the UART hardware will suppress the zero character
   received during a break condition. When enable is non-zero, the UART
   hardware will receive a zero character during a break condition. Not all
   UART cores support this feature.

==================================================================================================*/
static __inline
void HALuart_SetNullCharOnBreak(HALUartObject *uart, uint32 enable)
{
   uart->fcn_table->SetNullCharOnBreak(uart, enable);
}

/*==================================================================================================

FUNCTION: HALuart_LoopbackSet

DESCRIPTION:
   Enable or disable loopback mode. This mode routes the TX line back to the RX line.
   For BAM UARTs, this mode also routes the RTS line back to the CTS line.
   The reason routing the RTS to CTS is only done on BAM UARTs is because it's only 
   supported in HW on BAM UARTs. Loopbacks are done internally (inside the UART block).

==================================================================================================*/
static __inline
void HALuart_LoopbackSet(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->LoopbackSet(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetCxmEnable

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
static __inline
void HALuart_SetCxmEnable(HALUartObject *uart, HALUartCxmConfigType cxm_cfg, uint32 sam)
{
   uart->fcn_table->SetCxmEnable(uart, cxm_cfg, sam);
}

/*==================================================================================================

FUNCTION: HALuart_StartRxSideband

DESCRIPTION:
   This command enables the BAM producer sideband signals before an RX transfer.

==================================================================================================*/
static __inline
void HALuart_StartRxSideband(HALUartObject *uart)
{
   uart->fcn_table->StartRxSideband(uart);
}

/*==================================================================================================

FUNCTION: HALuart_StartTxSideband

DESCRIPTION:
   This command enables the BAM consumer sideband signals before a TX transfer.

==================================================================================================*/
static __inline
void HALuart_StartTxSideband(HALUartObject *uart)
{
   uart->fcn_table->StartTxSideband(uart);
}

/*==================================================================================================

FUNCTION: HALuart_SetAutoReactivateRx

DESCRIPTION:
   This command enables automatic reactivation of RX transfers.  This is a new mode of operation
   for BAM UARTs that eliminates the need for SW intervention between transfers (other than
   queuing BAM descriptors).  When an RX transfer completes it will not be necessary to write
   the size of the next transfer to DMRX, enable the stale event, or enable the RX sideband.
   The UART HW will perform these operations automatically (using the most recent DMRX value).

==================================================================================================*/
static __inline
void HALuart_SetAutoReactivateRx(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->SetAutoReactivateRx(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetDmaBurst

DESCRIPTION:
   This command configures the DMA burst size.  It should only be used in DMA mode because
   it conflicts with HALuart_SetRxWatermark() and HALuart_SetTxWatermark() which are used in
   PIO mode.  This command modifies the RFWR/TFWR registers.
   
   On UARTDM (DMOV-based) this is a no-op because the burst size is hard-wired to 16 bytes.

==================================================================================================*/
static __inline
void HALuart_SetDmaBurst(HALUartObject *uart, uint32 num_bytes)
{
   uart->fcn_table->SetDmaBurst(uart, num_bytes);
}

/*==================================================================================================

FUNCTION: HALuart_EnableHwFixes

DESCRIPTION:
   This command enables the latest HW fixes.  This is a no-op for legacy and UARTDM.
   On UARTBAM it sets (enabled == TRUE) or clears (enabled == FALSE) all the bits in the BCR.

==================================================================================================*/
static __inline
void HALuart_EnableHwFixes(HALUartObject *uart, uint32 enabled)
{
   uart->fcn_table->EnableHwFixes(uart, enabled);
}

/*==================================================================================================

FUNCTION: HALuart_SetType0WakeupClear

DESCRIPTION:
   This command clears the Type 0 message status, on UARTBAM cores that
   support CXM. This is a no-op for legacy and UARTDM.

==================================================================================================*/
static __inline
void HALuart_SetType0WakeupClear(HALUartObject *uart, uint32 clear)
{
   uart->fcn_table->SetType0WakeupClear(uart, clear);
}

/*==================================================================================================

FUNCTION: HALuart_GetCxmTxSticky

DESCRIPTION:
   Reads and optionally clears the 802.11 TX sticky flag, on cores that
   support coexistence.

==================================================================================================*/
static __inline
uint32 HALuart_GetCxmTxSticky(HALUartObject *uart, uint32 clear)
{
   return uart->fcn_table->GetCxmTxSticky(uart, clear);
}

/*==================================================================================================

FUNCTION: HALuart_CxmTxDirectChar

DESCRIPTION:
   UART CXM: Send only byte in direct mode w/o using SW TX FIFO

==================================================================================================*/
static __inline
uint32 HALuart_CxmTxDirectChar(HALUartObject *uart, uint8 character)
{
   return uart->fcn_table->CxmTxDirectChar(uart, character);
}

/*==================================================================================================

FUNCTION: HALuart_GetDmaRx

DESCRIPTION:
   Gets the current value of the DMRX register.

==================================================================================================*/
static __inline
uint32 HALuart_GetDmaRx(HALUartObject *uart)
{
   return uart->fcn_table->GetDmaRx(uart);
}

/*==================================================================================================

FUNCTION: HALuart_InitObject

DESCRIPTION:
   Initializes the HALUart object.  Must be called first before any other HAL APIs.

==================================================================================================*/
static __inline
void HALuart_InitObject(HALUartObject *uart, HALUartType type, void *virt_addr, void *phys_addr)
{
   // The default case should be impossible to hit (unless HALUartType is extended without
   // updating this switch statement).  In this unlikely scenario a NULL pointer should
   // cause a data abort which will quickly alert the programmer to his mistake.

   switch (type)
   {
      case HAL_UART_DM:     uart->fcn_table = &functions_uart_dm;     break;
      case HAL_UART_BAM:    uart->fcn_table = &functions_uart_bam;    break;
      default:              uart->fcn_table = NULL;                   break;
   }

   uart->base        = virt_addr;
   uart->physical    = phys_addr;
   uart->last_error  = HAL_UART_SUCCESS;
   uart->imr_shadow  = 0;
   uart->irda_shadow = 0;
}

/*==================================================================================================

FUNCTION: HALuart_GetLastError

DESCRIPTION:
   Returns the last error that occurred with this UART, clearing it.

==================================================================================================*/
static __inline
HALUartErrorType HALuart_GetLastError(HALUartObject *uart)
{
   HALUartErrorType last_error;

   last_error = uart->last_error;
   uart->last_error = HAL_UART_SUCCESS;
   return last_error;
}

#endif  // HALUART_H
