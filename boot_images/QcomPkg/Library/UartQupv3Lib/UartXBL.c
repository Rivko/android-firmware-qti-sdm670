/*==================================================================================================

FILE: Uart.c

DESCRIPTION: This module provides the driver Software for the UART.

Copyright (c) 2016 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   uart_close
   uart_initialize
   uart_open
   uart_poll
   uart_receive
   uart_read
   uart_transmit
   uart_write

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartXBL.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/7/16    VV       Initial revision

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <Library/IoLib.h>    // MmioWrite32(), etc.
#include "Uart.h"
#include "UartApi.h"
#include "UartDefs.h"
#include "UartOs.h"
#include "UartHal.h"
#include "UartHwio.h"
#include "UartLog.h"
#include "stringl/stringl.h"
#include "UartFWLoad.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   uint32 bit_rate;
   uint32 input_freq;
   uint32 divider;
} CLOCK_SETTINGS;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
#define UART_LOGGING_FILE_ID 10

#define RING_SIZE      256  // size of the ring buffer (must be a power of 2)


uart_context static_uart_context[UART_MAX_PORTS];

static UINT8 ring_buffer[RING_SIZE];  // size must be a power of 2
static UINT32 read_index = 0;   // must be masked before subscripting
static UINT32 write_index = 0;  // must be masked before subscripting
static UINT32 tx_fifo_size = 0; // will be populated in register_init
static uart_handle uart_debug_handle;

static const CLOCK_SETTINGS baud_table[] =
{
   // bit_rate, input_freq, divider
   {     115200,     7372800,      64 },
   {0}
};

/*==================================================================================================
                                               MACROS
==================================================================================================*/

// The read/write indices do not wrap around to zero until integer overflow occurs.  This keeps
// the logic simple when determining the number of bytes in the ring buffer.  But they must be
// masked before subscripting and the size of the ring buffer must be a power of 2.

#define BYTES_IN_RING()     ( write_index - read_index )
#define ROOM_IN_RING()      ( RING_SIZE - BYTES_IN_RING() - 1 )  // always leave one empty slot
#define RING_GET_BYTE()     ( ring_buffer[read_index++  & (RING_SIZE - 1)] )
#define RING_PUT_BYTE(data) { ring_buffer[write_index++ & (RING_SIZE - 1)] = data; }


/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void         check_rx_error(uart_context* uart_ctxt);
static uart_result  clock_enable(uart_context* uart_ctxt);
static void         flush_tx(uart_context* uart_ctxt);
static void         process_tx_data(uart_context* uart_ctxt);
static void         receive_data(uart_context* uart_ctxt);
static void         register_init(uart_context* uart_ctxt);
static void         rx_transfer_start(uart_context* uart_ctxt);
static void*        uart_isr(void *h);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: check_rx_error

DESCRIPTION:

==================================================================================================*/
static void check_rx_error(uart_context* uart_ctxt)
{
   return;
}

/*==================================================================================================

FUNCTION: clock_enable

DESCRIPTION:

==================================================================================================*/
static uart_result clock_enable(uart_context* uart_ctxt)
{
   uint32 input_freq;
   uint32 divider;
   uint32 bit_rate;
   const CLOCK_SETTINGS *entry;

   bit_rate = uart_ctxt->open_config.baud_rate;
   for (entry = baud_table; entry->bit_rate; entry++)
   {
      if ( entry->bit_rate == bit_rate ) { break; }
   }

   if (entry->bit_rate == 0)
   {
      return UART_ERROR;
   }
   input_freq = entry->input_freq;
   divider    = entry->divider;

   if( uart_clock_open(uart_ctxt, input_freq)== UART_ERROR)
   {
      return UART_ERROR;
   }

   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: flush_tx

DESCRIPTION:

==================================================================================================*/
static void flush_tx(uart_context* uart_ctxt)
{
   return;
}

/*==================================================================================================

FUNCTION: write_to_fifo

DESCRIPTION:

==================================================================================================*/
static uint32 write_to_fifo(volatile uint32 base, char* ptr, uint32 num_bytes)
{
   uint32 full_words = num_bytes >> 2;
   uint32 partial_bytes = num_bytes & 0x03;
   uint32 empty_slots;
   uint32 words_in_fifo;
   uint32 tx_fifo_size;
   uint32 word_value;
   uint32 words_to_send;
   uint32 bytes_to_send;
   uint32 i;

   words_in_fifo = REG_IN(base + GENI4_DATA, GENI_TX_FIFO_STATUS) & TX_FIFO_WC;

   tx_fifo_size = (REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_0) & TX_FIFO_DEPTH_MASK) >> TX_FIFO_DEPTH_SHIFT;

   empty_slots = tx_fifo_size - words_in_fifo;

   words_to_send = (full_words >  empty_slots) ? (empty_slots) : (full_words);
   bytes_to_send = (full_words >= empty_slots) ? (0) : (partial_bytes);

   if ( (ptr - (char *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
   {
      for (i = 0; i < words_to_send; i++)
      {
         word_value = ptr[0] << 0 | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
         REG_OUTI(base + GENI4_DATA, GENI_TX_FIFOn, 0, word_value);
         ptr += 4;
      }
   }
   else  // buffer is 32-bit word aligned
   {
      for (i = 0; i < words_to_send; i++)
      {
         word_value = *(uint32 *)ptr;
         REG_OUTI(base + GENI4_DATA, GENI_TX_FIFOn, 0, word_value);
         ptr += 4;
      }
   }

   if (bytes_to_send)
   {
      word_value = 0;
      for (i = 0; i < bytes_to_send; i++) { word_value |= ptr[i] << i * 8; }
      REG_OUTI(base + GENI4_DATA, GENI_TX_FIFOn, 0, word_value);
   }

   return(words_to_send * 4 + bytes_to_send);

}
/*==================================================================================================

FUNCTION: process_tx_data

DESCRIPTION: This function calls the HAL layer to transmit data and also does polling for TXLEV
             and calculates the wait time.

==================================================================================================*/

static void process_tx_data(uart_context* uart_ctxt)
{
   uint32 num_bytes = uart_ctxt->bytes_to_tx - uart_ctxt->bytes_txed;
   char* buf = (uart_ctxt->tx_buf)+(uart_ctxt->bytes_txed);

   volatile uint32 base = uart_ctxt->properties->uart_base;

   if (num_bytes)
   {
       uart_ctxt->bytes_txed += write_to_fifo(base, buf, num_bytes);
       if (uart_ctxt->bytes_txed == uart_ctxt->bytes_to_tx)
       {
           REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_CLEAR, TF_FIFO_WATERMARK_IRQ);
       }
       else
       {
           REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_SET, TF_FIFO_WATERMARK_IRQ);
       }
   }
}

/*==================================================================================================

FUNCTION: receive_data

DESCRIPTION:

==================================================================================================*/
static void receive_data(uart_context* uart_ctxt)
{
   volatile uint32  base = uart_ctxt->properties->uart_base;
   char*   buf = uart_ctxt->rx_buf;
   uint32  buf_size = uart_ctxt->rx_buf_size;
   uint32  rx_fifo_status;
   uint32  partial_bytes_to_read = 0;
   uint32  words_to_read = 0;
   uint32  rx_word;
   uint32  s_irq_status;
   uint32  is_stale = FALSE;
   uint32  avail_bytes;
   uint32  i;

   if (buf == NULL)
   {
      return;
   }

   s_irq_status = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS);
   rx_fifo_status = REG_IN(base + GENI4_DATA, GENI_RX_FIFO_STATUS);


   if (s_irq_status & (RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ) )
   {
      partial_bytes_to_read = (rx_fifo_status & RX_LAST_VALID_BYTES_MASK) >> RX_LAST_VALID_BYTES_SHIFT;
      words_to_read = rx_fifo_status & RX_FIFO_WC;
      if (partial_bytes_to_read != 0 && partial_bytes_to_read != 4)
      {
         words_to_read -= 1;
      }
      else
      {
         partial_bytes_to_read = 0;
      }
      if (s_irq_status & RX_LAST_IRQ)
      {
         is_stale = TRUE;
      }
   }
   avail_bytes = (words_to_read * 4) + partial_bytes_to_read;

   // If the buffer is full, callback the client and disable the interrupts till a new buffer is availble
   // through uart_receive function.

   if ((uart_ctxt->rx_write_offset + avail_bytes) > buf_size)
   {
      if (buf_size - uart_ctxt->rx_write_offset < 4)
      {
         if (uart_ctxt->open_config.rx_cb_isr)
         {
            uart_ctxt->open_config.rx_cb_isr(uart_ctxt->rx_write_offset, uart_ctxt->rx_cb_data);
         }

          // Clear the interrupts
          REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, s_irq_status);
          // Disable the fifo wm and stale interrupts
          REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ);
          return;
      }
      else
      {
          words_to_read = ((buf_size - uart_ctxt->rx_write_offset) & 0x3 ) >> 2;
          partial_bytes_to_read = 0;
          avail_bytes = (words_to_read * 4) + partial_bytes_to_read;
      }
   }
   else
   {

      // If you are definitely going to read till the LAST byte, clear the interrupt.
      // The RX LAST STATUS is not a level source to the secondary engine interrupt register
      // This will result in RX getting stuck as clearing the interrupt without any reading
      // the last byte will cause the interrupt to be permanently lost.

      REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, (s_irq_status & RX_LAST_IRQ));
   }

   buf += uart_ctxt->rx_write_offset;

   if ( (buf - (char *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
   {
      for (i = 0; i < words_to_read; i++)
      {
         rx_word = REG_INI(base + GENI4_DATA, GENI_RX_FIFOn, 0);
         buf[0] = (uint8)(rx_word >>  0);
         buf[1] = (uint8)(rx_word >>  8);
         buf[2] = (uint8)(rx_word >> 16);
         buf[3] = (uint8)(rx_word >> 24);
         buf += 4;
      }
   }
   else  // buffer is 32-bit word aligned
   {
      for (i = 0; i < words_to_read; i++)
      {
         rx_word = REG_INI(base + GENI4_DATA, GENI_RX_FIFOn, 0);
         *(uint32 *)buf = rx_word;
         buf += 4;
      }
   }

   if (partial_bytes_to_read)
   {
      rx_word = REG_INI(base + GENI4_DATA, GENI_RX_FIFOn, 0);
      for (i = 0; i < partial_bytes_to_read; i++) { buf[i] = (uint8)(rx_word >> i * 8); }
   }

   uart_ctxt->rx_write_offset += avail_bytes;

   if (uart_ctxt->open_config.rx_cb_isr)
   {
      // If it is stale interrupt( end of rx transfer ) call the client callback
      if (is_stale)
      {
         // Disable the fifo wm and stale interrupts since the current buffer is complete
         // When the client queues the next buffer, the uart_receive function will automatically
         // enable the interrupts

         REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ);
         uart_ctxt->open_config.rx_cb_isr(uart_ctxt->rx_write_offset, uart_ctxt->rx_cb_data);
      }
   }
   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, (s_irq_status & RX_FIFO_WATERMARK_IRQ));

}

/*==================================================================================================

FUNCTION: reginit

DESCRIPTION:

==================================================================================================*/
void register_init(uart_context* uart_ctxt)
{
   volatile uint32 base = uart_ctxt->properties->uart_base;
   uint32 temp;

   temp = REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_0); // to get TXFIFO size

   tx_fifo_size = ((temp & TX_FIFO_DEPTH_MASK) >> TX_FIFO_DEPTH_SHIFT << 2); // It will be in words.
                                                                             // so << 2.

   REG_OUT(base + GENI4_DATA, GENI_TX_WATERMARK_REG, 4);

   temp = REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_1);// to get RXFIFO width

   temp = (temp & RX_FIFO_DEPTH_MASK) >> RX_FIFO_DEPTH_SHIFT;

   REG_OUT(base + GENI4_DATA, GENI_RX_WATERMARK_REG, temp - 8);

   REG_OUT(base + GENI4_DATA, GENI_RX_RFR_WATERMARK_REG, temp - 4);


   REG_OUT(base + GENI4_CFG, GENI_SER_M_CLK_CFG, 0x21);

   REG_OUT(base + GENI4_CFG, GENI_SER_S_CLK_CFG, 0x21);

   //REG_OUT(base + GENI4_IMAGE_REGS, UART_LOOPBACK_CFG, 0x3);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_WORD_LEN, 0x8);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_RX_WORD_LEN, 0x8);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_PARITY_CFG, 0x0);
   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_TRANS_CFG, 0x2);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_RX_PARITY_CFG, 0x0);
   REG_OUT(base + GENI4_IMAGE_REGS, UART_RX_TRANS_CFG, 0x0);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_STOP_BIT_LEN, 0x0);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_RX_STALE_CNT, 0x16*10);

   REG_OUT(base + GENI4_IMAGE_REGS, GENI_TX_PACKING_CFG0, 0x4380E);
   REG_OUT(base + GENI4_IMAGE_REGS, GENI_TX_PACKING_CFG1, 0xC3E0E);

   REG_OUT(base + GENI4_IMAGE_REGS, GENI_RX_PACKING_CFG0, 0x4380E);
   REG_OUT(base + GENI4_IMAGE_REGS, GENI_RX_PACKING_CFG1, 0xC3E0E);

   rx_transfer_start(uart_ctxt);

}


/*==================================================================================================

FUNCTION: rx_transfer_start

DESCRIPTION:
   This function starts a new RX transfer.

==================================================================================================*/
static void rx_transfer_start(uart_context* uart_ctxt)
{
   volatile uint32 base = uart_ctxt->properties->uart_base;

   REG_OUT(base + GENI4_DATA, GENI_S_CMD0, 0x8000000);
}

/*==================================================================================================

FUNCTION: uart_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_close(uart_handle h)
{
   uart_context* uart_ctxt;
   uart_result    result = UART_SUCCESS;

   UART_LOG_0(INFO,"+uart_close");

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling uart_close with a NULL handle.");
      return UART_ERROR;
   }

   uart_ctxt = (uart_context*)h;
   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling uart_close on a closed port.");
      return UART_ERROR;
   }

   flush_tx(uart_ctxt);

   uart_ctxt->is_port_open = FALSE;


   if(UART_SUCCESS != uart_clock_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_clock_close failed.");
      result = UART_ERROR;
   }

   if(UART_SUCCESS != uart_tlmm_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_tlmm_close failed.");
      result = UART_ERROR;
   }
   if(UART_SUCCESS != uart_interrupt_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_interrupt_close failed.");
      result = UART_ERROR;
   }

   UART_LOG_0(INFO,"-uart_close");
   return result;
}


/*==================================================================================================

FUNCTION: uart_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_open(uart_handle* h, uart_port_id id, uart_open_config* config)
{
   uart_context* uart_ctxt;

   UART_LOG_0(INFO,"+uart_open");

   if (NULL == h)
   {
      UART_LOG_0(ERROR, "Calling uart_open with NULL handle pointer.");
      return UART_ERROR;
   }

   *h = NULL;

   if (id < UART_MAX_PORTS)
   {
      uart_ctxt = &static_uart_context[id];

      if (uart_ctxt->is_port_open == TRUE)
      {
          return UART_ERROR;
      }

      uart_ctxt->port_id = id;
   }
   else
   {
      UART_LOG_1(ERROR, "Invalid Port ID. Port: %d", id);
      return UART_ERROR;
   }

   memcpy((void*)&uart_ctxt->open_config,(void*) config, sizeof(uart_open_config));

   if(UART_SUCCESS != uart_get_properties(uart_ctxt))
   {
      UART_LOG_0(ERROR, "uart_get_properties failed.");
      return UART_ERROR;
   }

   if(UART_SUCCESS != clock_enable(uart_ctxt))
   {
      UART_LOG_0(ERROR, "clock_enable failed.");
      return UART_ERROR;
   }

   // If the secondary engine is active, we are in core and loader has already initialized the core.
   // So we can skip it.
   if(!(REG_IN(uart_ctxt->properties->uart_base + GENI4_CFG, GENI_STATUS) & S_GENI_CMD_ACTIVE))
   {
      if(UART_SUCCESS != uart_fw_load(uart_ctxt))
      {
         UART_LOG_0(ERROR, "clock_enable failed.");
         return UART_ERROR;
      }

      if(UART_SUCCESS != uart_tlmm_open(uart_ctxt))
      {
         UART_LOG_0(ERROR, "uart_tlmm_open failed.");
         uart_close((uart_handle)uart_ctxt);
         return UART_ERROR;
      }

      if(UART_SUCCESS != uart_interrupt_open(uart_ctxt,(void*)uart_isr))
      {
         UART_LOG_0(ERROR, "uart_interrupt_open failed.");
         uart_close((uart_handle)uart_ctxt);
         return UART_ERROR;
      }

      register_init(uart_ctxt);
   }

   *h = (uart_handle)uart_ctxt;
   uart_ctxt->is_port_open = TRUE;

   UART_LOG_0(INFO,"-uart_open");
   return UART_SUCCESS;
}


/*==================================================================================================

FUNCTION: uart_isr

DESCRIPTION:

==================================================================================================*/
static void *uart_isr(void *h)
{
   uart_context *uart_ctxt = (uart_context *)h;
   volatile uint32        base = uart_ctxt->properties->uart_base;
   uint32        m_irq_status = 0;

   UART_LOG_0(INFO,"+uart_isr");

   m_irq_status = REG_IN(base + GENI4_DATA, GENI_M_IRQ_STATUS);

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_CLEAR, m_irq_status);
   if (m_irq_status & TF_FIFO_WATERMARK_IRQ)
   {
      process_tx_data(uart_ctxt);
   }

   if (m_irq_status & M_CMD_DONE_IRQ)
   {
      if (uart_ctxt->open_config.tx_cb_isr == NULL)
      {
         //uart_os_signal_set(&uart_ctxt->tx_signal);
      }
      else
      {
         uart_ctxt->open_config.tx_cb_isr(uart_ctxt->bytes_txed, uart_ctxt->tx_cb_data);
      }
      uart_ctxt->tx_buf = NULL;
   }

   if ( uart_ctxt->open_config.rx_cb_isr && (m_irq_status & SEC_IRQ))
   {
      receive_data(uart_ctxt);
   }

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_CLEAR, m_irq_status);
   return NULL;
}

/*==================================================================================================

FUNCTION: uart_receive

DESCRIPTION:

==================================================================================================*/
uart_result uart_receive(uart_handle h, char* buf, uint32 buf_size, void* cb_data)
{
   uart_context* uart_ctxt;
   volatile uint32 base;

   UART_LOG_0(INFO,"+uart_receive");

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling uart_receive with a NULL handle.");
      return 0;
   }

   if (NULL == buf)
   {
      UART_LOG_0(ERROR,"Calling uart_receive with a NULL buffer.");
      return 0;
   }

   if (0 == buf_size)          { return UART_ERROR; }

   uart_ctxt = (uart_context*)h;

   base = uart_ctxt->properties->uart_base;
   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling uart_receive on a closed port.");
      return 0;
   }

   uart_ctxt->rx_buf = buf;
   uart_ctxt->rx_buf_size = buf_size;
   uart_ctxt->rx_cb_data = cb_data;
   uart_ctxt->rx_write_offset = 0;

   // Enable the receive interrupts only after receiving a buffer from client.
   // if not there is nothing to process when interrupt fires and we will end in interrupt storm

   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_SET, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ);

   if (uart_ctxt->open_config.rx_cb_isr == NULL)
   {
      receive_data(uart_ctxt);

      /* Override cb_data as bytes received in case of polling option */
      if (cb_data != NULL)
      {
         *(uint32*)cb_data = uart_ctxt->rx_write_offset;
      }

      uart_ctxt->rx_buf = NULL;
      uart_ctxt->rx_buf_size = 0;
      uart_ctxt->rx_cb_data = NULL;
      uart_ctxt->rx_write_offset = 0;
   }

   UART_LOG_0(INFO,"-uart_receive");

   return UART_SUCCESS;
}


/*==================================================================================================

FUNCTION: uart_transmit

DESCRIPTION:

==================================================================================================*/
uart_result uart_transmit(uart_handle h, char* buf, uint32 bytes_to_tx, void* cb_data)
{
   uart_context* uart_ctxt;
   volatile uint32 base;

   UART_LOG_0(INFO,"+uart_transmit");

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling uart_transmit with a NULL handle.");
      return 0;
   }

   if (NULL == buf)
   {
      UART_LOG_0(ERROR,"Calling uart_transmit with a NULL buffer.");
      return 0;
   }

   if (0 == bytes_to_tx)            { return UART_SUCCESS; }

   uart_ctxt = (uart_context*)h;
   base = uart_ctxt->properties->uart_base;

   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling uart_transmit on a closed port.");
      return 0;
   }

   if (REG_IN(base + GENI4_CFG, GENI_STATUS) & M_GENI_CMD_ACTIVE)
   {
      if ((uart_ctxt->open_config.tx_cb_isr == NULL) && (cb_data != NULL))
      {
            *(uint32*)cb_data = 0;
           return UART_ERROR;
      }
   }

   uart_ctxt->tx_buf = buf;
   uart_ctxt->bytes_to_tx = bytes_to_tx;
   uart_ctxt->tx_cb_data = cb_data;
   uart_ctxt->bytes_txed = 0;

   // Start the TX state machine
   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_TRANS_LEN, uart_ctxt->bytes_to_tx);
   REG_OUT(base + GENI4_DATA, GENI_M_CMD0, 0x08000000);

   process_tx_data(uart_ctxt);

   if (cb_data != NULL)
   {
      *(uint32*)cb_data = uart_ctxt->bytes_to_tx;
   }

   UART_LOG_0(INFO,"-uart_transmit");

   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_initialize

DESCRIPTION:

==================================================================================================*/
RETURN_STATUS EFIAPI
uart_initialize(void)
{
   uart_open_config c;

   c.baud_rate = 115200;
   c.parity_mode = UART_NO_PARITY;
   c.num_stop_bits = UART_1_0_STOP_BITS;
   c.bits_per_char = UART_8_BITS_PER_CHAR;
   c.enable_loopback = FALSE;
   c.enable_flow_ctrl = FALSE;
   c.tx_cb_isr = NULL;
   c.rx_cb_isr = NULL;

   uart_open(&uart_debug_handle, UART_DEBUG_PORT, &c);

   return RETURN_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_poll

DESCRIPTION:

==================================================================================================*/
BOOLEAN EFIAPI
uart_poll(void)
{
   volatile uint32  base = ((uart_context*)uart_debug_handle)->properties->uart_base;
 //  if (props == NULL) { props = get_properties(); }
  // if (props == NULL) { return FALSE; }

  // service_rx_fifo();
   uint32 s_irq_status = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS);

   if( (s_irq_status & (RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ)) || ( BYTES_IN_RING() > 0 ) )
   {
      return  1;
   }
   return 0;
}
/*==================================================================================================

FUNCTION: uart_read

DESCRIPTION:

==================================================================================================*/
UINTN EFIAPI
uart_read(OUT UINT8 *user_buffer, IN UINTN bytes_requested)
{
   UINT8 *ptr = user_buffer;
   UINT32 room_avail = bytes_requested;
   UINT32 bytes_copied = 0;
   UINT32 bytes_read = 0;
   UINT32 bytes_avail = 0;
   UINT32 num_bytes;
   UINT32 i;

   //if (props == NULL) { props = get_properties(); }
   //if (props == NULL) { return 0; }

   while (1)  // loop until user buffer is full or no more bytes are available
   {
      if (read_index == write_index)
      {
         read_index = write_index = 0;
      }
      if (RING_SIZE - write_index > 4)
      {
         uart_receive(uart_debug_handle, (char*)(ring_buffer + write_index), RING_SIZE - write_index, &bytes_read);
         write_index += bytes_read;
      }
      if(read_index < write_index)
      {
         bytes_avail = write_index - read_index;
      }
      else
      {
         bytes_avail = 0;
      }


      num_bytes = (room_avail < bytes_avail) ? (room_avail) : (bytes_avail);
      if (num_bytes == 0) { break; }
      for (i = 0; i < num_bytes; i++) { *ptr++ = RING_GET_BYTE(); }
      bytes_copied += num_bytes;
      room_avail -= num_bytes;
   }

   return(bytes_copied);

}

/*==================================================================================================

FUNCTION: uart_write

DESCRIPTION:

==================================================================================================*/
UINTN EFIAPI
uart_write(IN UINT8 *user_buffer, IN UINTN bytes_to_send)
{
   volatile uint32  base = ((uart_context*)uart_debug_handle)->properties->uart_base;
   UINT32 bytes_sent = 0;
   uint32 temp;

   if (tx_fifo_size == 0)
   {
      temp = REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_0); // to get TXFIFO size

      tx_fifo_size = ((temp & TX_FIFO_DEPTH_MASK) >> TX_FIFO_DEPTH_SHIFT << 2); // It will be in words.
                                                                                // so << 2.
   }
   bytes_to_send = (bytes_to_send > tx_fifo_size) ? tx_fifo_size : bytes_to_send;
   uart_transmit(uart_debug_handle, (char *)user_buffer, bytes_to_send, &bytes_sent);
   return(bytes_sent);
}

/*==================================================================================================

FUNCTION: uart_write

DESCRIPTION:

==================================================================================================*/
BOOLEAN EFIAPI
uart_is_cable_connected(void)
{
   volatile uint32  base = ((uart_context*)uart_debug_handle)->properties->uart_base;
   uint32 s_irq_status = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS);

   if ((s_irq_status & RX_BREAK_START) && !(s_irq_status & RX_BREAK_END))
   {
      return FALSE;
   }

   return TRUE;
}

