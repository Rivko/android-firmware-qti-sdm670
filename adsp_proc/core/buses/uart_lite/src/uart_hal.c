/*==================================================================================================

FILE: Uart_hal.c

DESCRIPTION: This module provides the HAL layer for  Software for the UART.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:

    HALuart_DisableInterrupts
    HALuart_ReadRxFifo
    HALuart_ResetErrorStatus
    HALuart_ResetReceiver
    HALuart_ResetTransmitter
    HALuart_SetAutoInboundFlowCtl
    HALuart_SetCharFormat
    HALuart_SetCmdSynchronization
    HALuart_SetDivider
    HALuart_SetDmaEnable
    HALuart_SetIrdaEnable
    HALuart_SetOutboundFlowCtl
    HALuart_SetLoopback
    HALuart_SetRxEnable
    HALuart_SetRxSCEnable
    HALuart_SetRxWatermark
    HALuart_SetStaleEvent
    HALuart_SetStaleTimeout
    HALuart_SetTxEnable
    HALuart_SetTxFifoSize
    HALuart_SetTxSCEnable
    HALuart_SetTxWatermark
    HALuart_SetupDmaRx
    HALuart_WriteTxFifo

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/uart_lite/src/uart_hal.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included Enable interrupt and get MISR APIs.
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart_hal.h"


/*==================================================================================================
                                          Global FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: HALuart_EnableInterrupts

DESCRIPTION:

==================================================================================================*/
void HALuart_EnableInterrupts(uint32 base, uint32 flags)
{
   uint32 imr;

   imr = REG_IN(base, UART_DM_IMR);

   if ( (imr & flags) != flags )  // check whether interrupts are already enabled
   {
      imr |= flags;
      REG_OUT(base, UART_DM_IMR, imr);
   }
}

/*==================================================================================================

FUNCTION: HALuart_DisableInterrupts

DESCRIPTION:

==================================================================================================*/
void HALuart_DisableInterrupts(uint32 base, uint32 flags)
{
   uint32 imr;

   imr = REG_IN(base, UART_DM_IMR);

   if (imr & flags)  // check whether interrupts are already disabled
   {
      imr &= ~flags;
      REG_OUT(base, UART_DM_IMR, imr);
   }
}

/*==================================================================================================

FUNCTION: HALuart_GetDmaRxSnapChars

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetDmaRxSnapChars(uint32 base)
{
   uint32 rx_total_snap;

   rx_total_snap = REG_IN(base, UART_DM_RX_TOTAL_SNAP);
   return(rx_total_snap);
}

/*==================================================================================================

FUNCTION: HALuart_GetFifoSize

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetFifoSize(uint32 base)
{
   uint32 ram_addr_width;
   ram_addr_width = REG_IN(base, UART_DM_GENERICS) & UARTDM_GENERIC_RAM_ADDR_WIDTH;
   return (1 << ram_addr_width);
}

/*==================================================================================================

FUNCTION: HALuart_GetInterruptStatus

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetInterruptStatus(uint32 base)
{
   uint32 isr;

   isr = REG_IN(base, UART_DM_ISR);
   return isr;
}

/*==================================================================================================

FUNCTION: HALuart_GetMaskedInterruptStatus

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetMaskedInterruptStatus(uint32 base)
{
   uint32 misr;

   misr = REG_IN(base, UART_DM_MISR);
   return misr;
}

/*==================================================================================================

FUNCTION: HALuart_GetStatus

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetStatus(uint32 base)
{
   uint32 sr;

   sr = REG_IN(base,UART_DM_SR);
   return sr;
}

/*==================================================================================================

FUNCTION: HALuart_GetTxFifoSize

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_GetTxFifoSize(uint32 base)
{
   uint32 badr;

   badr = REG_IN(base, UART_DM_BADR);
   return(badr);
}

/*==================================================================================================

FUNCTION: HALuart_ReadRxFifo

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_ReadRxFifo(uint32 base,char* read_buf,uint32 bytes_to_read)
{
   uint32 bytes_read    = 0;
   uint32 fifo_word;
   uint32 rxfs;
   uint32 words_in_fifo = 0;
   uint8  i;
   uint8  valid_bytes;


   rxfs = REG_IN(base,UART_DM_RXFS);

   words_in_fifo  = (rxfs & UARTDM_RXFS_FIFO_STATE_MSB_MASK) >> UARTDM_RXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (rxfs & UARTDM_RXFS_FIFO_STATE_LSB_MASK) >> UARTDM_RXFS_FIFO_STATE_LSB_SHIFT;

   if (bytes_to_read > (words_in_fifo * 4))
   {
      bytes_to_read = words_in_fifo * 4;
   }

   while (bytes_to_read)
   {
      valid_bytes = (bytes_to_read < 4) ? (bytes_to_read) : (4);
      fifo_word = REG_IN(base,UART_DM_RF);

      for (i = 0; i < valid_bytes; i++)
      {
         read_buf[bytes_read++] = fifo_word >> (i * 8);
      }
      bytes_to_read -= valid_bytes;
   }

   return bytes_read;
}

/*==================================================================================================

FUNCTION: HALuart_ResetErrorStatus

DESCRIPTION:

==================================================================================================*/
void HALuart_ResetErrorStatus(uint32 base)
{
   REG_OUT(base, UART_DM_CR, UARTDM_CR_RESET_ERR);
}

/*==================================================================================================

FUNCTION: HALuart_ResetReceiver

DESCRIPTION:

==================================================================================================*/
void HALuart_ResetReceiver(uint32 base)
{
   REG_OUT(base, UART_DM_CR, UARTDM_CR_RESET_RX);
}

/*==================================================================================================

FUNCTION: HALuart_ResetStaleInterrupt

DESCRIPTION:

==================================================================================================*/
void HALuart_ResetStaleInterrupt(uint32 base)
{
   REG_OUT(base,UART_DM_CR,UARTDM_CR_RESET_STALE);
}

/*==================================================================================================

FUNCTION: HALuart_ResetTransmitter

DESCRIPTION:

==================================================================================================*/
void HALuart_ResetTransmitter(uint32 base)
{
   REG_OUT(base, UART_DM_CR, UARTDM_CR_RESET_TX);
}

/*==================================================================================================

FUNCTION: HALuart_SetAutoInboundFlowCtl

DESCRIPTION:

==================================================================================================*/
void HALuart_SetAutoInboundFlowCtl(uint32 base, uint32 enabled)
{
   uint32 mr1;

   mr1 = REG_IN(base, UART_DM_MR1);

   if (enabled) { mr1 |=  UARTDM_MR1_RX_RDY_CTL; }
   else         { mr1 &= ~UARTDM_MR1_RX_RDY_CTL; }

   REG_OUT(base, UART_DM_MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_SetCharFormat

DESCRIPTION:

==================================================================================================*/
void HALuart_SetCharFormat(uint32 base,uint32 char_len,uint32 stop_bit,uint32 parity)
{
   uint32 mr2 = 0;
   mr2 = REG_IN(base,UART_DM_MR2);

   mr2 &= ~UARTDM_BITS_PER_CHAR_BMSK;
   mr2 &= ~UARTDM_STOP_BIT_BMSK;
   mr2 &= ~UARTDM_PARITY_BMSK;

   mr2 |= char_len << UARTDM_BITS_PER_CHAR_SHFT;
   mr2 |= stop_bit << UARTDM_STOP_BIT_SHFT;
   mr2 |= parity   << UARTDM_PARITY_SHFT;

   REG_OUT(base,UART_DM_MR2,mr2);
}

/*==================================================================================================

FUNCTION: HALuart_SetCmdSynchronization

DESCRIPTION:

==================================================================================================*/
void HALuart_SetCmdSynchronization(uint32 base, uint32 enabled)
{
   REG_OUT(base, UART_DM_CR, (enabled ? UARTDM_CR_ENA_CR_PROT : UARTDM_CR_DIS_CR_PROT));
}

/*==================================================================================================

FUNCTION: HALuart_SetDivider

DESCRIPTION:

==================================================================================================*/
void HALuart_SetDivider(uint32 base, uint32 divider)
{
   REG_OUT(base, UART_DM_CSR, divider);
}

/*==================================================================================================

FUNCTION: HALuart_SetDmaEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetDmaEnable(uint32 base, uint32 enable)
{
   uint32 dmen;

   dmen = REG_IN(base, UART_DM_DMEN);

   if (enable) { dmen |=  (UARTDM_DMEN_RX_BAM_ENABLE | UARTDM_DMEN_TX_BAM_ENABLE); }
   else        { dmen &= ~(UARTDM_DMEN_RX_BAM_ENABLE | UARTDM_DMEN_TX_BAM_ENABLE); }

   REG_OUT(base, UART_DM_DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_SetIrdaEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetIrdaEnable(uint32 base, uint32 enabled)
{
    uint32 irda;

   // NOTE: This register will not be generated if IRDA_IFC_GEN is not TRUE.  However,
   // in this case it should be harmless to write to it (there will be no effect).

   irda = REG_IN(base, UART_DM_IRDA);

   if (enabled) { irda |=  UARTDM_IRDA_IRDA_EN; }
   else         { irda &= ~UARTDM_IRDA_IRDA_EN; }

   REG_OUT(base, UART_DM_IRDA, irda);
}

/*==================================================================================================

FUNCTION: HALuart_SetLoopback

DESCRIPTION:

==================================================================================================*/
void HALuart_SetLoopback(uint32 base, uint32 enabled)
{
   uint32 mr2;

   mr2 = REG_IN(base, UART_DM_MR2);

   if (enabled) { mr2 |=  UARTDM_MR2_LOOPBACK; }
   else         { mr2 &= ~UARTDM_MR2_LOOPBACK; }

   REG_OUT(base, UART_DM_MR2, mr2);
}

/*==================================================================================================

FUNCTION: HALuart_bam_SetOutboundFlowCtl

DESCRIPTION:

==================================================================================================*/
void HALuart_SetOutboundFlowCtl(uint32 base, uint32 enabled)
{
   uint32 mr1;

   mr1 = REG_IN(base, UART_DM_MR1);

   if (enabled) { mr1 |=  UARTDM_MR1_CTS_CTL; }
   else         { mr1 &= ~UARTDM_MR1_CTS_CTL; }

   REG_OUT(base, UART_DM_MR1, mr1);
}

/*==================================================================================================

FUNCTION: HALuart_SetRxEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetRxEnable(uint32 base, uint32 enabled)
{
   REG_OUT(base, UART_DM_CR, (enabled ? UARTDM_CR_ENA_RX : UARTDM_CR_DIS_RX));
}

/*==================================================================================================

FUNCTION: HALuart_SetRxSCEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetRxSCEnable(uint32 base, uint32 enable)
{
   uint32 dmen;

   dmen = REG_IN(base, UART_DM_DMEN);

   if (enable) { dmen |=  UARTDM_DMEN_RX_SC_ENABLE; }
   else        { dmen &= ~UARTDM_DMEN_RX_SC_ENABLE; }

   REG_OUT(base, UART_DM_DMEN, dmen);
}


/*==================================================================================================

FUNCTION: HALuart_SetRxWatermark

DESCRIPTION:

==================================================================================================*/
void HALuart_SetRxWatermark(uint32 base, uint32 num_bytes)
{
   uint32 dmen;
   dmen = REG_IN(base, UART_DM_DMEN);
   if ( dmen & UARTDM_DMEN_RX_SC_ENABLE)
   {
      REG_OUT(base, UART_DM_RFWR, num_bytes - 1);       // RFWR expressed in 4-byte words;
                                                        // On SC mode, every byte occupies a word.
   }
   else
   {
      REG_OUT(base, UART_DM_RFWR, (num_bytes >> 2) - 1);   // RFWR expressed in 4-byte words;
                                                           // subtract 1 because the UART HW sets
                                                           // the RXLEV bit when the number of words
                                                           // in the RXFIFO is *greater* than the
                                                           // value in RFWR

   }
}


/*==================================================================================================

FUNCTION: HALuart_SetStaleEvent

DESCRIPTION:

==================================================================================================*/
void HALuart_SetStaleEvent(uint32 base, uint32 enabled)
{
   REG_OUT(base, UART_DM_CR, (enabled ? UARTDM_CR_ENA_STALE_EVT : UARTDM_CR_DIS_STALE_EVT));
}

/*==================================================================================================

FUNCTION: HALuart_SetStaleTimeout

DESCRIPTION:

==================================================================================================*/
void HALuart_SetStaleTimeout(uint32 base, uint32 stale_timeout)
{
   uint32 ipr;

   ipr = REG_IN(base, UART_DM_IPR);

   ipr &= ~UARTDM_IPR_STALE_TIMEOUT_LSB;
   ipr &= ~UARTDM_IPR_STALE_TIMEOUT_MSB;

   ipr |= (stale_timeout)      & UARTDM_IPR_STALE_TIMEOUT_LSB;
   ipr |= (stale_timeout << 2) & UARTDM_IPR_STALE_TIMEOUT_MSB;

   REG_OUT(base, UART_DM_IPR, ipr);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetTxEnable(uint32 base, uint32 enabled)
{
   REG_OUT(base, UART_DM_CR, (enabled ? UARTDM_CR_ENA_TX : UARTDM_CR_DIS_TX));
}

/*==================================================================================================

FUNCTION: HALuart_SetTxFifoSize

DESCRIPTION:

==================================================================================================*/
void HALuart_SetTxFifoSize(uint32 base, uint32 num_words)
{

   REG_OUT(base, UART_DM_BADR, num_words & (~0x3));  // BADR expressed in 4-byte words
}

/*==================================================================================================

FUNCTION: HALuart_SetTxSCEnable

DESCRIPTION:

==================================================================================================*/
void HALuart_SetTxSCEnable(uint32 base, uint32 enable)
{
   uint32 dmen;

   dmen = REG_IN(base, UART_DM_DMEN);

   if (enable) { dmen |=  UARTDM_DMEN_TX_SC_ENABLE; }
   else        { dmen &= ~UARTDM_DMEN_TX_SC_ENABLE; }

   REG_OUT(base, UART_DM_DMEN, dmen);
}

/*==================================================================================================

FUNCTION: HALuart_SetTxWatermark

DESCRIPTION:

==================================================================================================*/
void HALuart_SetTxWatermark(uint32 base, uint32 num_bytes)
{
   uint32 dmen;
   dmen = REG_IN(base, UART_DM_DMEN);
   if ( dmen & UARTDM_DMEN_TX_SC_ENABLE)
   {
      REG_OUT(base, UART_DM_TFWR, num_bytes );       // TFWR expressed in bytes.
                                                     // On SC mode, every byte occupies a word.
   }
   else
   {
      // num_bytes should be a multiple of 4.
      REG_OUT(base, UART_DM_TFWR, (num_bytes >> 2) );   // TFWR expressed in 4-byte words.
   }
}

/*==================================================================================================

FUNCTION: HALuart_SetupDmaRx

DESCRIPTION:

==================================================================================================*/
void HALuart_SetupDmaRx(uint32 base, uint32 num_chars)
{
   REG_OUT(base, UART_DM_DMRX, num_chars);
}

/*==================================================================================================

FUNCTION: HALuart_WriteTxFifo

DESCRIPTION:

==================================================================================================*/
uint32 HALuart_WriteTxFifo(uint32 base, char *buf, uint32 bytes_to_tx)
{
   uint32 num_bytes ;
   uint32 bytes_left = bytes_to_tx;
   uint32 data_word = 0;
   uint32 bytes_txed = 0;
   uint32 free_fifo = 0;
   uint32 words_in_fifo = 0;
   uint32 tfwr = 0;
   uint32 txfs = REG_IN(base,UART_DM_TXFS);
   uint32 tx_fifo_size = REG_IN(base, UART_DM_BADR);

   words_in_fifo  = (txfs & UARTDM_TXFS_FIFO_STATE_MSB_MASK) >> UARTDM_TXFS_FIFO_STATE_MSB_SHIFT;
   words_in_fifo |= (txfs & UARTDM_TXFS_FIFO_STATE_LSB_MASK) >> UARTDM_TXFS_FIFO_STATE_LSB_SHIFT;
   free_fifo = tx_fifo_size - words_in_fifo;

   // Driver uses SC mode so number of bytes & free_fifo can be directly matched.
   num_bytes = (bytes_left > free_fifo) ? free_fifo : bytes_left ;
   bytes_left -= num_bytes;
   while (num_bytes--)
   {
      data_word = (data_word & 0x00) | buf[bytes_txed++];
      REG_OUT(base,UART_DM_TF,data_word);
   }

   if (bytes_left)
   {
      tfwr = (bytes_left > tx_fifo_size) ? 0 : tx_fifo_size - bytes_left;
   }
   else
   {
      tfwr = 0;
   }
   HALuart_SetTxWatermark(base,tfwr);

   return bytes_txed;
}

