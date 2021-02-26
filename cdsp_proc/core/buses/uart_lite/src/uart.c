/*==================================================================================================

FILE: Uart.c

DESCRIPTION: This module provides the driver Software for the UART.

Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   uart_close
   uart_open
   uart_receive
   uart_transmit

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart_lite/src/uart.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/31/17   RC      Added the clock support and register sequence changes.
12/03/14   VV      Included cb_data option for event callback
09/25/13   VV      Included interrupt support.
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart_hwio.h"
#include "uart.h"
#include "uart_defs.h"
#include "uart_os.h"
#include "uart_hal.h"
#include "uart_log.h"
#include "stringl.h"
#include "qurt_island.h"

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

uart_context static_uart_contexts[UART_MAX_PORTS];

uint32 uart_cancel_dummy_word_cnt, uart_sw_flush_failed;
static const CLOCK_SETTINGS baud_table[] =
{
   // bit_rate, input_freq, divider
   {     115200,     7372800,        64 },
   {     230400,     7372800,        32 },
   {     460800,    14745600,        32 },
   {     921600,    29491200,        32 },
   {    1000000,    32000000,        32 },
   {0}
};

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void         check_rx_error(uart_context* uart_ctxt);
static uart_result  clock_enable(uart_context* uart_ctxt);
static void         flush_tx(uart_context* uart_ctxt);
static void         get_clock_param(uint32 bit_rate, uint32* freq, uint32* div);
static void         process_tx_data(uart_context* uart_ctxt);
static void         receive_data(uart_context* uart_ctxt);
static void         register_init(uart_context* uart_ctxt);
static void         rx_transfer_start(uart_context* uart_ctxt);
static void*        uart_isr(void *h);
static void*        uart_wakeup_isr(void* cb_data);
static void         rx_transfer_stop(uart_context* uart_ctxt);

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
   uint32 reg_value;
   uint32 freq;
   uint32 div;
   volatile uint32 base = uart_ctxt->properties->uart_base;

   get_clock_param(uart_ctxt->open_config.baud_rate, &freq, &div);

   if (!freq || !div )
   {
      return UART_ERROR;
   }
   if (UART_SUCCESS != uart_clock_open(uart_ctxt, freq))
   {
      return UART_ERROR;
   }

   reg_value = ((div/32) << 0x4) | 0x1;
   REG_OUT(base + GENI4_CFG, GENI_SER_M_CLK_CFG, reg_value);

   REG_OUT(base + GENI4_CFG, GENI_SER_S_CLK_CFG, reg_value);
   
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

FUNCTION: get_clock_param

DESCRIPTION:

==================================================================================================*/

static void get_clock_param(uint32 bit_rate,uint32* freq, uint32* div)
{
   uint8 i=0;
   while (1)
   {
      if (bit_rate == baud_table[i].bit_rate)
      {
         *freq = baud_table[i].input_freq;
         *div = baud_table[i].divider;
         return;
      }
      if (baud_table[i].bit_rate == 0)
      {
         *freq = 0;
         *div = 0;
         return;
      }
      i++;
   }
}

/*==================================================================================================

FUNCTION: write_to_fifo

DESCRIPTION:

==================================================================================================*/
static uint32 write_to_fifo(volatile uint32 base, uint8* ptr, uint32 num_bytes)
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

   if ( (ptr - (uint8 *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
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
   uint8* buf = uart_ctxt->tx_buf + uart_ctxt->bytes_txed;
   uint32 num_bytes = uart_ctxt->bytes_to_tx - uart_ctxt->bytes_txed;
   volatile uint32 base = uart_ctxt->properties->uart_base;

   if (num_bytes)
   {
       uart_ctxt->bytes_txed += write_to_fifo(base, buf, num_bytes);
       if (uart_ctxt->bytes_txed == uart_ctxt->bytes_to_tx)
       {
          REG_OUT(base + GENI4_DATA, GENI_TX_WATERMARK_REG, 0);
          REG_OUT(base + GENI4_DATA, GENI_M_IRQ_CLEAR, TF_FIFO_WATERMARK_IRQ);
          REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_CLEAR, TF_FIFO_WATERMARK_IRQ);
          REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_SET, M_CMD_DONE_IRQ);
       }
       else
       {
          REG_OUT(base + GENI4_DATA, GENI_TX_WATERMARK_REG, 4);
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
   uint8*  buf = uart_ctxt->rx_buf;
   uint32  buf_size = uart_ctxt->rx_buf_size;
   uint32  rx_fifo_status;
   uint32  partial_bytes_to_read = 0;
   uint32  words_to_read = 0;
   uint32  rx_word;
   uint32  s_irq_status;
   uint32  is_stale = FALSE;
   uint32  avail_bytes;
   uint32 i;

   if (buf == NULL)
   {
      return;
   }

   s_irq_status = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS) ;

   rx_fifo_status = REG_IN(base + GENI4_DATA, GENI_RX_FIFO_STATUS);

   UART_LOG(INFO,"receive_data: s status 0x%08x \n", s_irq_status);

   s_irq_status &= REG_IN(base + GENI4_DATA, GENI_S_IRQ_ENABLE);
   UART_LOG(INFO,"receive_data: s status 0x%08x \n", s_irq_status);

   if (s_irq_status & RX_LAST_IRQ)
   {
      partial_bytes_to_read = (rx_fifo_status & RX_LAST_VALID_BYTES_MASK) >> RX_LAST_VALID_BYTES_SHIFT;
      words_to_read = rx_fifo_status & RX_FIFO_WC;
      if (partial_bytes_to_read != 0 && partial_bytes_to_read != 4)
      {
         words_to_read -= 1;
      }
      is_stale = TRUE;
   }
   else if (s_irq_status & RX_FIFO_WATERMARK_IRQ)
   {
      words_to_read = REG_IN(base + GENI4_DATA, GENI_RX_WATERMARK_REG);
      partial_bytes_to_read = 0;
   }

   avail_bytes = (words_to_read * 4) + partial_bytes_to_read;

   // If the buffer is full, callback the client and disable the interrupts till a new buffer is availble
   // through uart_receive function.

   if ((uart_ctxt->rx_write_offset + avail_bytes) > buf_size)
   {
      // Disable the fifo wm and stale interrupts
      REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ | S_GP_SYNC_IRQ );

      UART_LOG(INFO, "Receive complete. Insufficient buffer.\n");

      uart_ctxt->open_config.rx_cb_isr(uart_ctxt->rx_write_offset, uart_ctxt->rx_cb_data);

      return;
   }

   // Since we are sure we can read all the bytes, clear the LAST interrupt before reading the fifo.
   // If it is cleared after FIFO read, we will miss a new stale and that will lead to HW going to infinite wait.
   // When the last word of this stale is read, the new transfer will kick in and there could be a stale of that transfer
   // that gets set. So if you clear the LAST bit after that, we will be clearing the new stale and not reading contents of
   // FIFO for that. This will lead to HW never generating another stale as the old one is never processed. this will put
   // it in infinite wait till FIFO is read.

   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, s_irq_status & (RX_LAST_IRQ | S_GP_SYNC_IRQ));

   buf += uart_ctxt->rx_write_offset;

   if ( (buf - (uint8 *)NULL) & 0x03 )  // buffer is not 32-bit word aligned
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
      if (is_stale && (uart_ctxt->rx_write_offset != 0))
      {
         // Disable the fifo wm and stale interrupts since the current buffer is complete
         // When the client queues the next buffer, the uart_receive function will automatically
         // enable the interrupts

         REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ | S_GP_SYNC_IRQ);

         UART_LOG(INFO, "Receive complete. Stale interrupt.\n");

         uart_ctxt->open_config.rx_cb_isr(uart_ctxt->rx_write_offset, uart_ctxt->rx_cb_data);
      }
   }

   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, s_irq_status & RX_FIFO_WATERMARK_IRQ);
}

/*==================================================================================================

FUNCTION: reginit

DESCRIPTION:

==================================================================================================*/
void register_init(uart_context* uart_ctxt)
{
   volatile uint32 base = uart_ctxt->properties->uart_base;
   uint32 temp;

   REG_OUT(base + GENI4_IMAGE_REGS, GENI_DMA_MODE_EN, 0);

   REG_OUT(base + QUPV3_SE_DMA, SE_IRQ_EN, 0xFFFFFFFF);

   REG_OUT(base + QUPV3_SE_DMA, SE_GSI_EVENT_EN, 0);

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_CLEAR, 0xFFFFFFFF);

   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, 0xFFFFFFFF);
   
   temp = REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_0); // to get TXFIFO width

   temp = (temp & TX_FIFO_DEPTH_MASK) >> TX_FIFO_DEPTH_SHIFT;

   REG_OUT(base + GENI4_DATA, GENI_TX_WATERMARK_REG, 0);

   temp = REG_IN(base + QUPV3_SE_DMA, SE_HW_PARAM_1);// to get RXFIFO width

   temp = (temp & RX_FIFO_DEPTH_MASK) >> RX_FIFO_DEPTH_SHIFT;

   REG_OUT(base + GENI4_DATA, GENI_RX_WATERMARK_REG, temp - 8);

   REG_OUT(base + GENI4_DATA, GENI_RX_RFR_WATERMARK_REG, temp - 4);

   if (uart_ctxt->open_config.enable_loopback)
   {
      REG_OUT(base + GENI4_IMAGE_REGS, UART_LOOPBACK_CFG, 0x3);
   }
   else
   {
      REG_OUT(base + GENI4_IMAGE_REGS, UART_LOOPBACK_CFG, 0x0);
   }

   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_WORD_LEN, 0x8);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_RX_WORD_LEN, 0x8);

   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_PARITY_CFG, 0x0);

   if(uart_ctxt->open_config.enable_flow_ctrl || uart_ctxt->open_config.enable_loopback)
   {
      REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_TRANS_CFG, 0x0);
   }
   else
   {
      REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_TRANS_CFG, 0x2);
   }

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

   rx_transfer_stop(uart_ctxt);


   REG_OUT(base + GENI4_DATA, GENI_S_CMD0, 0x08000000);

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_SET, 0x80000000);
}

/*==================================================================================================

FUNCTION: rx_transfer_stop

DESCRIPTION:
   This function starts a new RX transfer.

==================================================================================================*/

static void rx_transfer_stop(uart_context* uart_ctxt)
{
   volatile uint32 base = uart_ctxt->properties->uart_base;
   uint32 rx_irq_en_val;
   uint32 rx_irq_status;
   uint32 rx_discard_word;
   uint32 word_cnt;
   uint32 poll_cnt = 0;
   rx_irq_en_val = REG_IN(base + GENI4_DATA, GENI_S_IRQ_ENABLE);
   UART_LOG(INFO, "rx_transfer_stop. S enable : 0x%08x\n", rx_irq_en_val);
   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, 0xFFFFFFF);
   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_CLEAR, 0x80000000);
   if(REG_IN(base + GENI4_CFG, GENI_STATUS) & 0x41000)
   {
      REG_OUT(base + GENI4_DATA, GENI_S_CMD_CTRL_REG, 0x4);
      do
      {
         rx_irq_status = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS);
         if(rx_irq_status & RX_LAST_IRQ)
         {
            REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, RX_LAST_IRQ | S_GP_SYNC_IRQ);
            word_cnt = REG_IN(base + GENI4_DATA, GENI_RX_FIFO_STATUS) & RX_FIFO_WC;
            while(word_cnt--)
            {
               uart_cancel_dummy_word_cnt++;
               rx_discard_word = REG_INI(base + GENI4_DATA, GENI_RX_FIFOn, 0);
            }
         }
         if(rx_irq_status & S_CMD_CANCEL_IRQ)
         {
            REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, S_CMD_CANCEL_IRQ | S_CMD_DONE_IRQ);
            break;
         }
      }while(poll_cnt++ < 1000);
   }
   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_SET, rx_irq_en_val);

}

/*==================================================================================================

FUNCTION: uart_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_close(uart_handle h)
{
   uart_context* uart_ctxt;
   uart_result    result = UART_SUCCESS;

   UART_LOG(INFO,"+uart_close");

   if (NULL == h)
   {
      UART_LOG(ERROR,"Calling uart_close with a NULL handle.");
      return UART_ERROR;
   }

   uart_ctxt = (uart_context*)h;
   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG(ERROR,"Calling uart_close on a closed port.");
      return UART_ERROR;
   }

   uart_mutex_lock(uart_ctxt);
  
   if(uart_ctxt->clock_on)
   {
     rx_transfer_stop(uart_ctxt);                          
   }

   if(!uart_ctxt->clock_on)
   {
      if(UART_SUCCESS != clock_enable(uart_ctxt))
      {
         UART_LOG(ERROR, "uart_clock_open failed.");
         uart_mutex_unlock(uart_ctxt);
         return UART_ERROR;
      }
   }
   uart_ctxt->is_port_open = FALSE;
   uart_ctxt->clock_on = FALSE;
   if(uart_ctxt->wakeup_interrupt_registered)
   {
      if(UART_SUCCESS != uart_wakeup_interrupt_unregister(uart_ctxt, TRUE))
      {
         result = UART_ERROR;
      }
   }

   if(UART_SUCCESS != uart_tlmm_close(uart_ctxt))
   {
      UART_LOG(ERROR, "Uart_tlmm_close failed.");
      result = UART_ERROR;
   }

   if(UART_SUCCESS != uart_interrupt_close(uart_ctxt))
   {
      UART_LOG(ERROR, "Uart_interrupt_close failed.");
      result = UART_ERROR;
   }

   if(UART_SUCCESS != uart_clock_close(uart_ctxt))
   {
      UART_LOG(ERROR, "Uart_clock_close failed.");
      result = UART_ERROR;

   }

   uart_mutex_unlock(uart_ctxt);
   /* Don't destroy the mutex if ISR is running */
   while(uart_ctxt->isr_running);

   uart_mutex_destroy(uart_ctxt);

   UART_LOG(INFO,"-uart_close");
   return result;
}


/*==================================================================================================

FUNCTION: uart_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_open(uart_handle* h, uart_port_id id, uart_open_config* config)
{
   uart_context* uart_ctxt;
   boolean enable_island = FALSE;

   UART_LOG(INFO,"+uart_open");

   if (NULL == h)
   {
      UART_LOG(ERROR, "Calling uart_open with NULL handle pointer.");
      return UART_ERROR;
   }

   *h = NULL;

   // Defensive check to make sure the clients have moved to latest config structure.
   //if((config->mode != UART_ISLAND_MODE) && (config->mode != UART_NORMAL_MODE))
   //{
      //return UART_ERROR;
   //}

   if (id < UART_MAX_PORTS)
   {
      uart_ctxt = &static_uart_contexts[id];

      if (uart_ctxt->is_port_open == TRUE)
      {
          return UART_ERROR;
      }

      memset((void*)uart_ctxt, 0x0, sizeof(uart_context));
      uart_ctxt->port_id = id;
   }
   else
   {
      UART_LOG(ERROR, "Invalid Port ID. Port: %d", id);
      return UART_ERROR;
   }

   memscpy(&uart_ctxt->open_config, sizeof(uart_open_config), config, sizeof(uart_open_config));

   if(UART_SUCCESS != uart_get_properties(uart_ctxt))
   {
      UART_LOG(ERROR, "uart_get_properties failed.");
      return UART_ERROR;
   }

   uart_mutex_init(uart_ctxt);

   if(UART_SUCCESS != clock_enable(uart_ctxt))
   {
      UART_LOG(ERROR, "clock_enable failed.");
      return UART_ERROR;
   }

   uart_ctxt->clock_on = TRUE;
   if(UART_SUCCESS != uart_tlmm_open(uart_ctxt))
   {
      UART_LOG(ERROR, "uart_tlmm_open failed.");
      uart_close((uart_handle)uart_ctxt);
      return UART_ERROR;
   }

   if(config->mode == UART_ISLAND_MODE)
   {
      enable_island = TRUE;
   }
   else
   {
      enable_island = FALSE;
   }
   if(UART_SUCCESS != uart_interrupt_open(uart_ctxt,(void*)uart_isr, enable_island))
   {
      UART_LOG(ERROR, "uart_interrupt_open failed.");
      uart_close((uart_handle)uart_ctxt);
      return UART_ERROR;
   }

   if((config->mode == UART_ISLAND_MODE) && config->enable_wake_feature)
   {
      boolean enable_at_pdc = TRUE;// Register the interrupt at the PDC level, so during island mode, we can simply
                                   // enable/disable at the L2VIC level
      boolean enable_intr = FALSE; // Don't enable the interrupt
      boolean island_intr = TRUE;  // Mark it as island interrupt

      if(uart_wakeup_interrupt_register(uart_ctxt, uart_wakeup_isr, enable_at_pdc, enable_intr, island_intr) != UART_SUCCESS)
      {
         uart_close((uart_handle)uart_ctxt);
         return UART_ERROR;
      }
      uart_ctxt->wakeup_interrupt_registered = TRUE;
   }
   register_init(uart_ctxt);

   *h = (uart_handle)uart_ctxt;
   uart_ctxt->is_port_open = TRUE;

   UART_LOG(INFO,"-uart_open");
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

   UART_LOG(INFO,"+uart_isr");

   uart_ctxt->isr_running = TRUE;

   uart_mutex_lock(uart_ctxt);

   if(!uart_ctxt->clock_on)
   {
      uart_ctxt->isr_running = FALSE;

      uart_mutex_unlock(uart_ctxt);
      return NULL;
   }

   m_irq_status = REG_IN(base + GENI4_DATA, GENI_M_IRQ_STATUS) ;

   UART_LOG(INFO,"uart_isr: m status 0x%08x \n", m_irq_status);

   m_irq_status &= REG_IN(base + GENI4_DATA, GENI_M_IRQ_ENABLE);

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_CLEAR, m_irq_status);

   if (m_irq_status & TF_FIFO_WATERMARK_IRQ)
   {
      process_tx_data(uart_ctxt);
   }

   if (m_irq_status & M_CMD_DONE_IRQ)
   {
      REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_CLEAR, M_CMD_DONE_IRQ);
      UART_LOG(INFO, "Transmit complete. calling callback.\n");
      uart_ctxt->open_config.tx_cb_isr(uart_ctxt->bytes_txed, uart_ctxt->tx_cb_data);
   }

   if (m_irq_status & SEC_IRQ)
   {
      receive_data(uart_ctxt);
   }

   uart_mutex_unlock(uart_ctxt);

   uart_ctxt->isr_running = FALSE;

   return NULL;
}

/*==================================================================================================

FUNCTION: uart_wakeup_isr

DESCRIPTION:

==================================================================================================*/
static void* uart_wakeup_isr(void* cb_data)
{
   uart_context *uart_ctxt = (uart_context *)cb_data;
   UART_WAKEUP_CALLBACK wakeup_cb = uart_ctxt->wakeup_cb;

   boolean disable_at_pdc = FALSE;

   UART_LOG(INFO,"+uart_wakeup_isr");

   uart_ctxt->wakeup_cb = NULL;

   if(uart_ctxt->open_config.mode == UART_ISLAND_MODE)
   {
      disable_at_pdc = FALSE;
   }
   else
   {
      disable_at_pdc = TRUE;
   }

   uart_mutex_lock(uart_ctxt);

   if(uart_ctxt->wakeup_interrupt_enabled)
   {
      uart_wakeup_interrupt_unregister(uart_ctxt, disable_at_pdc);
      uart_ctxt->wakeup_interrupt_enabled = FALSE;
      if(disable_at_pdc)
      {
         uart_ctxt->wakeup_interrupt_registered = FALSE;
      }
   }

   uart_mutex_unlock(uart_ctxt);

   if(wakeup_cb != NULL)
   {
      wakeup_cb(uart_ctxt->wakeup_cb_data);
   }

   UART_LOG(INFO,"-uart_wakeup_isr");

   return NULL;
}

/*==================================================================================================

FUNCTION: uart_receive

DESCRIPTION:

==================================================================================================*/
uart_result uart_receive(uart_handle h, uint8* buf, uint32 buf_size, void* cb_data)
{
   uart_context* uart_ctxt;
   volatile uint32 base;
   uart_result result = UART_ERROR;

   UART_LOG(INFO,"+uart_receive");

   if (NULL == h)
   {
      UART_LOG(ERROR,"Calling uart_receive with a NULL handle.");
      return UART_ERROR;
   }

   if (NULL == buf)
   {
      UART_LOG(ERROR,"Calling uart_receive with a NULL buffer.");
      return UART_ERROR;
   }

   if (0 == buf_size)          { return UART_ERROR; }

   uart_ctxt = (uart_context*)h;

   base = uart_ctxt->properties->uart_base;
   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG(ERROR,"Calling uart_receive on a closed port.");
      return UART_ERROR;
   }

   uart_mutex_lock(uart_ctxt);
   if(uart_ctxt->clock_on)
   {
   uart_ctxt->rx_buf = buf;
   uart_ctxt->rx_buf_size = buf_size;
   uart_ctxt->rx_cb_data = cb_data;
   uart_ctxt->rx_write_offset = 0;

   // Enable the receive interrupts only after receiving a buffer from client.
   // if not there is nothing to process when interrupt fires and we will end in interrupt storm

      REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_SET, RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ | S_GP_SYNC_IRQ);

      result = UART_SUCCESS;
   }
   else
   {
      UART_LOG(INFO, "uart_receive: clocks are off\n");
      result = UART_ERROR;
   }
   uart_mutex_unlock(uart_ctxt);
   UART_LOG(INFO,"-uart_receive");

   return result;
}


/*==================================================================================================

FUNCTION: uart_transmit

DESCRIPTION:

==================================================================================================*/
uart_result uart_transmit(uart_handle h, uint8* buf, uint32 bytes_to_tx, void* cb_data)
{
   uart_context* uart_ctxt;
   volatile uint32 base;
   uart_result result = UART_ERROR;

   UART_LOG(INFO,"+uart_transmit");

   if (NULL == h)
   {
      UART_LOG(ERROR,"Calling uart_transmit with a NULL handle.");
      return UART_ERROR;
   }

   if (NULL == buf)
   {
      UART_LOG(ERROR,"Calling uart_transmit with a NULL buffer.");
      return UART_ERROR;
   }

   if (0 == bytes_to_tx)            { return UART_SUCCESS; }

   uart_ctxt = (uart_context*)h;
   base = uart_ctxt->properties->uart_base;

   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG(ERROR,"Calling uart_transmit on a closed port.");
      return UART_ERROR;
   }

   uart_mutex_lock(uart_ctxt);
   if(uart_ctxt->clock_on)
   {
   uart_ctxt->tx_buf = buf;
   uart_ctxt->bytes_to_tx = bytes_to_tx;
   uart_ctxt->tx_cb_data = cb_data;
   uart_ctxt->bytes_txed = 0;


   // Start the TX state machine
   REG_OUT(base + GENI4_IMAGE_REGS, UART_TX_TRANS_LEN, bytes_to_tx);
   
   REG_OUT(base + GENI4_DATA, GENI_M_CMD0, 0x08000000);

   process_tx_data(uart_ctxt);
      result = UART_SUCCESS;
   }
   else
   {
      result = UART_ERROR;
   }

   uart_mutex_unlock(uart_ctxt);
   UART_LOG(INFO,"-uart_transmit");

   return result;
}

/*==================================================================================================

FUNCTION: uart_power_off

DESCRIPTION:

==================================================================================================*/
uart_result      uart_power_off(uart_handle h, boolean wake_on_rx, UART_WAKEUP_CALLBACK wake_cb, void* wake_cb_data)
{
   uart_context* uart_ctxt;
   boolean enable_at_pdc = FALSE;
   boolean enable_island_intr = FALSE;
   volatile uint32 base ;

   UART_LOG(INFO,"+uart_power_off");

   if (NULL == h)
   {
      UART_LOG(ERROR,"Calling uart_power_off with a NULL handle.");
      return UART_ERROR;
   }

   uart_ctxt = (uart_context*)h;
   base = uart_ctxt->properties->uart_base;

   uart_mutex_lock(uart_ctxt);
   uart_ctxt->prev_rx_intr_en_val = REG_IN(base + GENI4_DATA, GENI_S_IRQ_ENABLE);
   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_CLEAR, 0xFFFFFFF);
   rx_transfer_stop(uart_ctxt);

   if(uart_ctxt->clock_on)
   {
      if(UART_SUCCESS != uart_clock_close(uart_ctxt))
      {
         UART_LOG(ERROR, "Uart_clock_close failed.");
         uart_mutex_unlock(uart_ctxt);
         return UART_ERROR;
      }

      uart_ctxt->clock_on = FALSE;
   }

   if(wake_on_rx && uart_ctxt->open_config.enable_wake_feature)
   {
      uart_ctxt->wakeup_cb = wake_cb;
      uart_ctxt->wakeup_cb_data = wake_cb_data;

      if(uart_ctxt->open_config.mode == UART_ISLAND_MODE)
      {
         enable_at_pdc = FALSE;
         enable_island_intr = TRUE;
      }
      else
      {
         enable_at_pdc = TRUE;
         enable_island_intr = FALSE;
      }

      if(uart_wakeup_interrupt_register(uart_ctxt, uart_wakeup_isr, enable_at_pdc, TRUE, enable_island_intr) != UART_SUCCESS)
      {
         uart_mutex_unlock(uart_ctxt);
         return UART_ERROR;
      }
      uart_ctxt->wakeup_interrupt_enabled = TRUE;
      if(enable_at_pdc)
      {
         uart_ctxt->wakeup_interrupt_registered = TRUE;
      }
   }

   uart_mutex_unlock(uart_ctxt);

   UART_LOG(INFO,"-uart_power_off");

   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_power_on

DESCRIPTION:

==================================================================================================*/
uart_result      uart_power_on(uart_handle h)
{
   uart_context* uart_ctxt;
   boolean disable_at_pdc = FALSE;
   volatile uint32 base ;
   uint32 rx_fifo_status;
   uint32 rx_irq_status;
   uint32 rx_word;
   uint32 poll_cnt = 0;
   uint32 words_to_read;

   UART_LOG(INFO,"+uart_power_on");

   if (NULL == h)
   {
      UART_LOG(ERROR,"Calling uart_power_on with a NULL handle.");
      return UART_ERROR;
   }

   uart_ctxt = (uart_context*)h;
   base = uart_ctxt->properties->uart_base;

   uart_mutex_lock(uart_ctxt);

   if(uart_ctxt->wakeup_interrupt_enabled)
   {
      if(uart_ctxt->open_config.mode == UART_ISLAND_MODE)
      {
         disable_at_pdc = FALSE;
      }
      else
      {
         disable_at_pdc = TRUE;
      }

      uart_wakeup_interrupt_unregister(uart_ctxt, disable_at_pdc);
      uart_ctxt->wakeup_interrupt_enabled = FALSE;
      if(disable_at_pdc)
      {
         uart_ctxt->wakeup_interrupt_registered = FALSE;
      }
   }

   if(!uart_ctxt->clock_on)
   {
      if(UART_SUCCESS != clock_enable(uart_ctxt))
      {
         UART_LOG(ERROR, "uart_clock_open failed.");

         uart_mutex_unlock(uart_ctxt);

         return UART_ERROR;
      }

      uart_ctxt->clock_on = TRUE;
   }

   uart_mutex_unlock(uart_ctxt);

   rx_transfer_start(uart_ctxt);

   REG_OUT(base + GENI4_CFG, GENI_RX_FIFO_CTRL, 0x1);

   do
   {
      uart_busy_wait(50);

      rx_irq_status  = REG_IN(base + GENI4_DATA, GENI_S_IRQ_STATUS);
      rx_fifo_status = REG_IN(base + GENI4_DATA, GENI_RX_FIFO_STATUS);

      UART_LOG(INFO, "Inside SW stale loop. Irq status: 0x%08x, Fifo status: 0x%08x\n", rx_irq_status, rx_fifo_status);

      REG_OUT(base + GENI4_DATA, GENI_S_IRQ_CLEAR, rx_irq_status & (RX_LAST_IRQ | RX_FIFO_WATERMARK_IRQ | S_GP_SYNC_IRQ));

      words_to_read = rx_fifo_status & RX_FIFO_WC;

      while(words_to_read--)
      {
         UART_LOG(INFO, "reading dummy\n");
         rx_word = REG_INI(base + GENI4_DATA, GENI_RX_FIFOn, 0);
      }
      // The last IRQ will be set but not GP_SYNC only for SW stale. If there are pending stales when
      // SW flush is issued, HW will first report it with both these bits set. The SW flush will be processed
      // and only the LAST bit will be set.
      if((rx_irq_status & RX_LAST_IRQ) && !(rx_irq_status & S_GP_SYNC_IRQ))
      {
         UART_LOG(INFO, "Met SW FIFO condition exiting\n");

         break;
      }

   }while((poll_cnt++ < 100));

   if(poll_cnt >= 100)
   {
      if((rx_irq_status & RX_BREAK_START_IRQ) && !(rx_irq_status & RX_BREAK_END_IRQ))
      {
         UART_LOG(INFO," Break active. SW stale timedout\n");
      }
      else
      {
         uart_sw_flush_failed++;
      }
   }

   REG_OUT(base + GENI4_DATA, GENI_S_IRQ_EN_SET, uart_ctxt->prev_rx_intr_en_val);

   REG_OUT(base + GENI4_DATA, GENI_M_IRQ_EN_SET, SEC_IRQ);
   UART_LOG(INFO,"-uart_power_on");

   return UART_SUCCESS;
}
