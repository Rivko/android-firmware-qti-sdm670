
#include "qurt_thread.h"
#include "qurt_signal.h"
#include "qurt_mutex.h"
#include "uart.h"
#include "msg.h"
#include "err.h"
#include "string.h"

/*==========================================================================
                       MACROS
==========================================================================*/
#define UART_THREAD_STACK_SIZE    2048
#define UART_TX_COMPLETION_SIGNAL 0x01
#define UART_RX_COMPLETION_SIGNAL 0x02
#define UART_GPIO_INT_SIGNAL      0x04

#define UART_TX_BUF_SIZE          512
#define UART_RX_BUF_SIZE          128

qurt_thread_attr_t       uart_thread_attr;
qurt_thread_t            uart_thread;
qurt_signal_t            uart_signal;
uint8                    uart_stack[UART_THREAD_STACK_SIZE];
uint32                   intr_count;
uart_handle              serial_port_test;

static uint8 uart_tx_buf[UART_TX_BUF_SIZE];
static uint8 uart_rx_buf[UART_RX_BUF_SIZE];

static uint32 tx_buf_head;
static uint32 tx_buf_tail;


void uart_gpio_int_cb(void* cb_data)
{
   intr_count++;
   qurt_signal_set(&uart_signal, UART_GPIO_INT_SIGNAL);

}


/*
   Simple circular queue management. Will be called during RX completion callback to queue all the
   received data in to the tx buffer for transmission from the worker thread.
   Did not consider all corner cases. If there is not enough space in the tx_buf, drop the data.
*/
void uart_queue_tx_data(uint32 bytes_to_copy)
{
   uint32 i = tx_buf_head;
   uint32 chunk_size = 0;
   uint32 bytes_available;

   if (tx_buf_tail <= tx_buf_head)
   {
      if ((i + bytes_to_copy) < UART_TX_BUF_SIZE)
      {
         memcpy((void *)(uart_tx_buf + i), (const void *)uart_rx_buf, bytes_to_copy);
         tx_buf_head = (tx_buf_head + bytes_to_copy) % UART_TX_BUF_SIZE;
      }

      else
      {
         chunk_size = UART_TX_BUF_SIZE - i;
         memcpy((void *)(uart_tx_buf + i), (const void *)uart_rx_buf, chunk_size);
         bytes_to_copy -= chunk_size;

         bytes_to_copy =  (tx_buf_tail <= bytes_to_copy) ? (tx_buf_tail - 1) : bytes_to_copy;
         {
            memcpy((void *)uart_tx_buf, (const void *)(uart_rx_buf + chunk_size), bytes_to_copy);
            tx_buf_head = bytes_to_copy;
         }
      }
   }
   else
   {
      bytes_available = tx_buf_tail - tx_buf_head ;
      bytes_to_copy = (bytes_available < bytes_to_copy) ? bytes_available : bytes_to_copy;
      memcpy((void *)(uart_tx_buf + i), (const void *)uart_rx_buf, bytes_to_copy);
      tx_buf_head = (tx_buf_head + bytes_to_copy) % UART_TX_BUF_SIZE;
   }
}

/*
   Callback function passed to the UART driver.
   In the test, same callback function is passed for both TX and RX completion events.
   cb_data = 1 for RX and 2 for TX.

   If needed separate callbacks can be used.

   It is called in interrupt context. So please keep the processing minimum and defer all other calls
   worker thread.
*/
void uart_cb(uint32 num_bytes, void* cb_data)
{
   int data = (int)cb_data;
   if (data == 1)
   {
      uart_queue_tx_data(num_bytes);
      qurt_signal_set(&uart_signal, UART_RX_COMPLETION_SIGNAL);
   }
   else if (data == 2)
   {
      /* Increment the tail to the count reported by TX. UART may not be able to transmit full
         requested size in some cases.
      */
      tx_buf_tail = (tx_buf_tail + num_bytes) % UART_TX_BUF_SIZE;
      qurt_signal_set(&uart_signal, UART_TX_COMPLETION_SIGNAL);
   }
   else if (data == 3)
   {
      qurt_signal_set(&uart_signal, UART_TX_COMPLETION_SIGNAL);
   }

}

/*
   Basic UART thread that handles everything.
   The test is simple echo back. It has a small circular TX buffer of 512 bytes and a normal RX buffer
   of 128 bytes.
   Any RX data is copied into the circular buffer and transmitted back. when a new data comes in and
   if the circular tx buffer is full, the extra data is dropped.
*/

void uart_worker_thread(void* cb_data)
{
   uint32 set_sig;
   uart_open_config c;
   boolean tx_in_progress = FALSE;
   uint32 bytes_to_tx;
   boolean power_down_port = FALSE;


   /* Basic configuration */
   c.baud_rate = 115200;
   c.parity_mode = UART_NO_PARITY;
   c.num_stop_bits = UART_1_0_STOP_BITS;
   c.bits_per_char = UART_8_BITS_PER_CHAR;
   c.enable_loopback = FALSE;
   c.enable_flow_ctrl = FALSE;
   c.mode = UART_NORMAL_MODE;
   c.enable_wake_feature = TRUE;
   c.tx_cb_isr = uart_cb; /* Called in IST context */
   c.rx_cb_isr = uart_cb; /* Called in IST context */
   uart_open(&serial_port_test, UART_DEBUG_INSTANCE, &c);

   uart_receive(serial_port_test, uart_rx_buf, UART_RX_BUF_SIZE, (void *)1);

   uart_power_off(serial_port_test, TRUE, uart_gpio_int_cb, NULL);

   /*
      refer to uart.h for the instances.
      Current support is for debug instance and BTLE instance which is UART_INSTANCE_04
   */

   while (1)
   {
      set_sig = qurt_signal_wait(&uart_signal,0xFF, QURT_SIGNAL_ATTR_WAIT_ANY);
      qurt_signal_clear(&uart_signal, set_sig);

      if (set_sig & UART_TX_COMPLETION_SIGNAL)
      {
         /*
            In the event of TX completion, check for more data. If not, mark the progress flag to
            false and continue.
         */
         if(power_down_port)
         {
            uart_power_off(serial_port_test, TRUE, uart_gpio_int_cb, NULL);
            power_down_port = FALSE;
            tx_in_progress = FALSE;
         }
         else if (tx_buf_tail == tx_buf_head)
         {
            tx_in_progress = FALSE;
         }
         else
         {
            bytes_to_tx = (tx_buf_head < tx_buf_tail) ? (UART_TX_BUF_SIZE -  tx_buf_tail) : (tx_buf_head - tx_buf_tail);
            uart_transmit(serial_port_test, uart_tx_buf + tx_buf_tail, bytes_to_tx , (void *)2);
         }
      }

      if (set_sig & UART_RX_COMPLETION_SIGNAL)
      {
         if(uart_rx_buf[0] == '`')
         {
            power_down_port = TRUE;
            uart_transmit(serial_port_test, (uint8*)"Powering down UART port.\n\r", 26, (void*)3);
         }
         /*
            If there is already a TX in progress, skip calling uart_transmit. The tx completion will
            take care of it. If not, call uart_transmit.
         */
         else
         {
            if(!tx_in_progress)
            {
               tx_in_progress = TRUE;

               bytes_to_tx = (tx_buf_head < tx_buf_tail) ? (UART_TX_BUF_SIZE -  tx_buf_tail) : (tx_buf_head - tx_buf_tail);
               uart_transmit(serial_port_test, uart_tx_buf + tx_buf_tail, bytes_to_tx , (void *)2);
            }

            /*
               The data would have been copied to the tx buffer in the callback itself. Queue the rx buffer
               back.
            */
            uart_receive(serial_port_test, uart_rx_buf, UART_RX_BUF_SIZE, (void *)1);
         }
      }

      if(set_sig & UART_GPIO_INT_SIGNAL)
      {
         uart_power_on(serial_port_test);
         uart_receive(serial_port_test, uart_rx_buf, UART_RX_BUF_SIZE, (void *)1);
         tx_in_progress = TRUE;
         uart_transmit(serial_port_test, (uint8*)"Powering up UART port.\n\r", 24, (void*)3);
      }
   }
}

void uart_test_init(void)
{
   qurt_signal_init(&uart_signal);

   // Create the UART thread with default attributes.
   qurt_thread_attr_init(&uart_thread_attr);
   qurt_thread_attr_set_name(&uart_thread_attr, "thread_uart_test");
   qurt_thread_attr_set_priority(&uart_thread_attr, QURT_THREAD_ATTR_PRIORITY_DEFAULT);
   qurt_thread_attr_set_stack_size(&uart_thread_attr, UART_THREAD_STACK_SIZE);
   qurt_thread_attr_set_stack_addr(&uart_thread_attr, (void *)uart_stack);

   qurt_thread_create( &uart_thread,
                     &uart_thread_attr,
                     uart_worker_thread,
                     NULL);

}
