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

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart_lite/src/uart_user_pd.c#1 $

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart.h"
#include "uart_qdi.h"
#include "err.h"

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
#define UART_LOGGING_FILE_ID 30
#define MAX_ID_LENGTH        40
#define MAX_PORTS            3
#define THREAD_STACK_SIZE    4096
#define STACK_ALIGNMENT      8


static char interrupt_thread_stack[THREAD_STACK_SIZE + STACK_ALIGNMENT - 1];
static int  uart_user_pd_qdi_handle;
/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void       uart_user_thread(void *unused);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: uart_user_thread

DESCRIPTION: main function for our worker thread. waits to be signaled that an interrupt has
happened, and then calls the appropriate callback

==================================================================================================*/
static void uart_user_thread(void *unused)
{
   uart_pd_event_cb_ctxt pd_ctxt;
   int result;

   while (1)
   {
      result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                            UART_WAIT_FOR_EVENT,
                            &pd_ctxt);
      if(result == QURT_EOK)
      {
         if(pd_ctxt.pd_event == UART_PD_EVENT_WAKEUP)
         {
            if(pd_ctxt.client_wakeup_cb != NULL)
            {
               pd_ctxt.client_wakeup_cb(pd_ctxt.cb_data);
            }
         }
         else if((pd_ctxt.pd_event == UART_PD_EVENT_TX_COMPLETION) ||
                 (pd_ctxt.pd_event == UART_PD_EVENT_RX_COMPLETION))
         {
            if(pd_ctxt.client_tfr_cb != NULL)
            {
               pd_ctxt.client_tfr_cb(pd_ctxt.bytes_transferred, pd_ctxt.cb_data);
            }
         }
      }

      /*
         If the return value is QURT_ECANCEL, it means the PD is
         restarting. so just exit the loop.
      */
      else if(result == QURT_ECANCEL)
      {
         break;
      }
   }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: Uart_spawn_thread

DESCRIPTION: init function for the driver used by the RC_INIT function

==================================================================================================*/
void uart_spawn_user_thread()
{
   int result;
   char *addr;
   qurt_thread_attr_t attr;
   qurt_thread_t tid;
   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_name(&attr, "uart_user_thread");
   qurt_thread_attr_set_stack_size(&attr, THREAD_STACK_SIZE);
   qurt_thread_attr_set_tcb_partition (&attr, 1);
   qurt_thread_attr_set_priority(&attr, 30);

   uart_user_pd_qdi_handle = qurt_qdi_open("uart");

   //force stack alignment to 8 byte boundry, per QURT thread specs.

   //make sure aligned address will fall inside our allocated stack
   addr = (char *)((unsigned long long) interrupt_thread_stack + STACK_ALIGNMENT - 1);
   //align to 8 byte boundary
   addr = (char *)((unsigned long long) addr & ~(STACK_ALIGNMENT - 1));

   qurt_thread_attr_set_stack_addr(&attr, addr);
   result = qurt_thread_create(&tid, &attr, uart_user_thread, NULL);
   if (result != QURT_EOK)
   {
      ERR_FATAL("uart interrupt thread creation failed", 0, 0, 0);
   }

}

/*==================================================================================================

FUNCTION: uart_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_close(uart_handle h)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_CLOSE,
                                   h);

   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;

}

/*==================================================================================================

FUNCTION: uart_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_open(uart_handle* h, uart_port_id id, uart_open_config* config)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_OPEN,
                                   h,
                                   id,
                                   config);
   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;

}

/*==================================================================================================

FUNCTION: uart_receive

DESCRIPTION:

==================================================================================================*/
uart_result uart_receive(uart_handle h, uint8* buf, uint32 buf_size, void* cb_data)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_RECEIVE,
                                   h,
                                   buf,
                                   buf_size,
                                   cb_data);
   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;

}

/*==================================================================================================

FUNCTION: uart_transmit

DESCRIPTION:

==================================================================================================*/
uart_result uart_transmit(uart_handle h, uint8* buf, uint32 bytes_to_tx, void* cb_data)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_TRANSMIT,
                                   h,
                                   buf,
                                   bytes_to_tx,
                                   cb_data);
   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;

}

/*==================================================================================================

FUNCTION: uart_power_off

DESCRIPTION:

==================================================================================================*/
uart_result      uart_power_off(uart_handle h, boolean wake_on_rx, UART_WAKEUP_CALLBACK wake_cb, void* wake_cb_data)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_POWER_OFF,
                                   h,
                                   wake_on_rx,
                                   wake_cb,
                                   wake_cb_data);
   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;
}
/*==================================================================================================

FUNCTION: uart_power_on

DESCRIPTION:

==================================================================================================*/
uart_result      uart_power_on(uart_handle h)
{
   int result;

   result = qurt_qdi_handle_invoke(uart_user_pd_qdi_handle,
                                   UART_POWER_ON,
                                   h);
   return (result == QURT_EOK) ? UART_SUCCESS : UART_ERROR;
}
