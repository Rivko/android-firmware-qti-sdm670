/*==================================================================================================

FILE: qdi_uart.c

DESCRIPTION: Defines the interface and functionality of a QDI Driver for the needed UART functions
while in the micro image.

                     Copyright (c) 2015,2018 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "uart_qdi.h"
#include "qurt.h"
#include "qurt_error.h"
#include "uart_log.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/
#define UART_MAX_EVENTS       8
#define UART_SIGNAL           0xFF
#define UART_MAX_USER_PORTS   2        // Max ports supported from User PDs
#define UART_TX_BUF_SIZE      128
#define UART_RX_BUF_SIZE      128
/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   qurt_qdi_obj_t obj;
} qdi_uart_opener;

typedef struct
{
   uart_handle    handle;

   UART_CALLBACK           user_tx_cb_isr;
   void                   *user_tx_cb_data;
   uint8                   root_pd_tx_buf[UART_TX_BUF_SIZE];
   uint8                  *user_pd_tx_buf;
   uint32                  user_pd_bytes_to_tx;
   uint32                  bytes_txed;

   UART_CALLBACK           user_rx_cb_isr;
   void                   *user_rx_cb_data;
   uint8                   root_pd_rx_buf[UART_RX_BUF_SIZE];
   uint8                   rx_head;
   uint8                   rx_tail;
   uint8                  *user_pd_rx_buf;
   uint32                  user_pd_rx_buf_size;

   UART_WAKEUP_CALLBACK    user_wakeup_cb;
   void                   *user_wakeup_cb_data;
} uart_root_pd_context;

typedef struct
{
   uart_pd_event           event;
   uart_root_pd_context   *ctxt;
   uint32                  bytes_transferred;
}uart_event_queue;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static int qdi_uart_invocation(int client_handle, qurt_qdi_obj_t *obj, int method,
                               qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                               qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                               qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);

static void qdi_uart_release(qurt_qdi_obj_t *obj);

static void uart_root_tx_cb(uint32 num_bytes, void* cb_data);

static void uart_root_rx_cb(uint32 num_bytes, void* cb_data);

static void uart_root_wakeup_cb(void* cb_data);
/*==================================================================================================
                                             LOCAL VARIABLES
==================================================================================================*/

static qdi_uart_opener user_pd_opener =
{
   {
     qdi_uart_invocation,
     QDI_REFCNT_PERM,
     qdi_uart_release
   }
};

static qurt_qdi_obj_t   uart_qdi_root_obj;

static qurt_mutex_t   uart_root_pd_mutex;
static qurt_signal_t  uart_root_pd_signal;

static uart_event_queue    event_queue[UART_MAX_EVENTS];
static uint32              event_queue_head;
static uint32              event_queue_tail;

static uart_root_pd_context   uart_contexts[UART_MAX_USER_PORTS];

/*==================================================================================================
                                             LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: uart_qdi_open

DESCRIPTION:

==================================================================================================*/
static int uart_qdi_open(int client_handle)
{
   return qurt_qdi_handle_create_from_obj_t(client_handle, &user_pd_opener.obj);
}

/*==================================================================================================

FUNCTION: uart_root_close

DESCRIPTION:

==================================================================================================*/
static int uart_root_close(uart_handle h)
{
   uart_root_pd_context* ctxt = NULL;

   if (h == NULL)
   {
      return QURT_EFAILED;
   }
   ctxt = (uart_root_pd_context*)h;

   uart_close(ctxt->handle);
   ctxt->handle = NULL;

   return QURT_EOK;

}

/*==================================================================================================

FUNCTION: uart_root_open

DESCRIPTION:

==================================================================================================*/
static int uart_root_open(int client_handle, uart_handle *h, uart_port_id id, uart_open_config *config)
{
   uint8    i;
   uart_root_pd_context *ctxt = NULL;
   uart_open_config     root_local_config;
   uint32  result = QURT_EFAILED;

   qurt_mutex_lock (&uart_root_pd_mutex);
   for (i = 0; i < UART_MAX_USER_PORTS; i++)
   {
      if (uart_contexts[i].handle == NULL)
      {
         ctxt = &uart_contexts[i];
         break;
      }
   }

   if (ctxt == NULL)
   {
      result = QURT_EFAILED;
	  goto exit;
   }

   if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                           (void *)&root_local_config,
                                           (void *)config,
                                           sizeof(uart_open_config)))
   {
      result = QURT_EFAILED;
	  goto exit;
   }


   ctxt->user_tx_cb_isr = root_local_config.tx_cb_isr;
   ctxt->user_rx_cb_isr = root_local_config.rx_cb_isr;

   root_local_config.tx_cb_isr = uart_root_tx_cb;
   root_local_config.rx_cb_isr = uart_root_rx_cb;

   if (UART_SUCCESS != uart_open(&ctxt->handle, id, &root_local_config))
   {
      result = QURT_EFAILED;
	  goto exit;
   }

   *h = (uart_handle)ctxt;
   result = QURT_EOK;
   
exit:
      qurt_mutex_unlock (&uart_root_pd_mutex);
      return result;
}

/*==================================================================================================

FUNCTION: uart_root_power_off

DESCRIPTION:

==================================================================================================*/
static int uart_root_power_off(int client_handle,
                               uart_handle h,
                               boolean wake_on_rx,
                               UART_WAKEUP_CALLBACK wakeup_cb,
                               void* cb_data)
{
   uart_root_pd_context* ctxt = NULL;
   
   if(qurt_island_get_status())
   {
	   return QURT_ENOTALLOWED;
   }
   
   if (h == NULL)
   {
      return QURT_EFAILED;
   }

   ctxt = (uart_root_pd_context*)h;

   if (ctxt == NULL)
   {
      return QURT_EFAILED;
   }

   if(wake_on_rx)
   {
      ctxt->user_wakeup_cb = wakeup_cb;
      ctxt->user_wakeup_cb_data = cb_data;
   }
   else
   {
      ctxt->user_wakeup_cb = NULL;
      ctxt->user_wakeup_cb_data = NULL;
   }

   if(uart_power_off(ctxt->handle, wake_on_rx, uart_root_wakeup_cb, (void*)ctxt) != UART_SUCCESS)
   {
      return QURT_EFAILED;
   }

   return QURT_EOK;
}

/*==================================================================================================

FUNCTION: uart_root_power_on

DESCRIPTION:

==================================================================================================*/
static int uart_root_power_on(int client_handle,
                               uart_handle h)
{
   uart_root_pd_context* ctxt = NULL;
   
   if(qurt_island_get_status())
   {
	   return QURT_ENOTALLOWED;
   }

   if (h == NULL)
   {
      return QURT_EFAILED;
   }

   ctxt = (uart_root_pd_context*)h;

   if (ctxt == NULL)
   {
      return QURT_EFAILED;
   }
   
   if(uart_power_on(ctxt->handle) != UART_SUCCESS)
   {
      return QURT_EFAILED;
   }

   return QURT_EOK;
}

/*==================================================================================================

FUNCTION: uart_root_receive

DESCRIPTION:

==================================================================================================*/
static int uart_root_receive(int client_handle, uart_handle h, uint8* buf, uint32 buf_size, void* cb_data)
{
   uart_root_pd_context* ctxt = NULL;
   uint32 size;

   if (h == NULL)
   {
      return QURT_EFAILED;
   }

   UART_LOG(INFO, "+uart_root_receve.\n");
   ctxt = (uart_root_pd_context*)h;

   ctxt->user_pd_rx_buf       = buf;
   ctxt->user_pd_rx_buf_size  = buf_size;
   ctxt->user_rx_cb_data      = cb_data;

   size = (buf_size > UART_RX_BUF_SIZE) ? UART_RX_BUF_SIZE : buf_size;

   if (UART_SUCCESS != uart_receive(ctxt->handle,
                                    ctxt->root_pd_rx_buf,
                                    size,
                                    ctxt))
   {
      ctxt->user_pd_rx_buf       = NULL;
      ctxt->user_pd_rx_buf_size  = 0;
      ctxt->user_rx_cb_data      = NULL;
      return QURT_EFAILED;
   }
   UART_LOG(INFO, "-uart_root_receve.\n");
   return QURT_EOK;
}

/*==================================================================================================

FUNCTION: uart_root_rx_cb

DESCRIPTION:

==================================================================================================*/
static void uart_root_rx_cb(uint32 num_bytes, void* cb_data)
{
   UART_LOG(INFO, "uart_root_rx_cb: num_bytes: %ld", num_bytes);
   qurt_mutex_lock (&uart_root_pd_mutex);

   event_queue[event_queue_head].bytes_transferred   = num_bytes;
   event_queue[event_queue_head].ctxt                = (uart_root_pd_context*)cb_data;
   event_queue[event_queue_head].event               = UART_PD_EVENT_RX_COMPLETION;
   event_queue_head = (event_queue_head + 1) % UART_MAX_EVENTS;

   qurt_mutex_unlock (&uart_root_pd_mutex);

   qurt_signal_set(&uart_root_pd_signal, UART_SIGNAL);

}

/*==================================================================================================

FUNCTION: uart_root_transmit

DESCRIPTION:

==================================================================================================*/
static int uart_root_transmit(int client_handle, uart_handle h, uint8* buf, uint32 bytes_to_tx, void* cb_data)
{
   uart_root_pd_context* ctxt = NULL;
   uint32 bytes_to_copy = 0;

   if (h == NULL)
   {
      return QURT_EFAILED;
   }

   UART_LOG(INFO, "+uart_root_transmit.\n");
   ctxt = (uart_root_pd_context*)h;

   // If there is already a pending tx, return error
   if(ctxt->user_pd_bytes_to_tx != 0)
   {
      return QURT_EFAILED;
   }

   ctxt->bytes_txed = 0;
   ctxt->user_pd_tx_buf = buf;
   ctxt->user_pd_bytes_to_tx = bytes_to_tx;
   ctxt->user_tx_cb_data = cb_data;

   bytes_to_copy = (bytes_to_tx > UART_TX_BUF_SIZE) ? UART_TX_BUF_SIZE : bytes_to_tx;

   if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                           (void *)ctxt->root_pd_tx_buf,
                                           (void *)ctxt->user_pd_tx_buf,
                                           bytes_to_copy))
   {
      return QURT_EFAILED;
   }

   if (UART_SUCCESS != uart_transmit(ctxt->handle, ctxt->root_pd_tx_buf, bytes_to_copy, ctxt))
   {
      ctxt->user_pd_tx_buf = NULL;
      ctxt->user_pd_bytes_to_tx = 0;
      ctxt->user_tx_cb_data = NULL;
      UART_LOG(INFO,"UART Transmit failed\n");

      return QURT_EFAILED;
   }

   UART_LOG(INFO,"UART Transmit queued. bytes_to_tx %ld \n", bytes_to_tx);
   return QURT_EOK;
}

/*==================================================================================================

FUNCTION: uart_root_tx_cb

DESCRIPTION:

==================================================================================================*/
static void uart_root_tx_cb(uint32 num_bytes, void* cb_data)
{
   UART_LOG(INFO,"UART Transmit cb. num_bytes %ld \n", num_bytes);
   qurt_mutex_lock (&uart_root_pd_mutex);

   ((uart_root_pd_context*)cb_data)->user_pd_bytes_to_tx -= num_bytes;
   ((uart_root_pd_context*)cb_data)->bytes_txed += num_bytes;
   event_queue[event_queue_head].bytes_transferred = num_bytes;
   event_queue[event_queue_head].ctxt           = (uart_root_pd_context*)cb_data;
   event_queue[event_queue_head].event          = UART_PD_EVENT_TX_COMPLETION;
   event_queue_head = (event_queue_head + 1) % UART_MAX_EVENTS;

   qurt_mutex_unlock (&uart_root_pd_mutex);

   qurt_signal_set(&uart_root_pd_signal, UART_SIGNAL);

}

/*==================================================================================================

FUNCTION: uart_root_wait_for_event

DESCRIPTION:

==================================================================================================*/
static int uart_root_wait_for_event(int client_handle, uart_pd_event_cb_ctxt* event_ctxt)
{
   unsigned int mask = 0;
   boolean done_processing = FALSE;

   UART_LOG(INFO, "+uart_root_wait_for_event.\n");
   do
   {
      if (event_queue_tail == event_queue_head)
      {
         if (QURT_EOK == qurt_signal_wait_cancellable(&uart_root_pd_signal,
                                                     UART_SIGNAL,
                                                     QURT_SIGNAL_ATTR_WAIT_ANY,
                                                     &mask))
         {
            UART_LOG(INFO, "UART uart_root_wait_for_event. clearing signal \n");
            qurt_signal_clear(&uart_root_pd_signal, UART_SIGNAL);
         }
         else
         {
            return QURT_ECANCEL;
         }
      }
      else
      {
         UART_LOG(INFO, "UART uart_root_wait_for_event. clearing signal head <> tail\n");
         // If the tail and head do not match, then the signal might be set before we looped around.
         // So clear the signal.
         qurt_signal_clear(&uart_root_pd_signal, UART_SIGNAL);
      }
      if (event_queue_tail == event_queue_head)
      {
         continue;
      }
      UART_LOG(INFO, "uart_root_wait_for_event. head : %ld tail : %ld \n", event_queue_head, event_queue_tail);

      uart_root_pd_context *ctxt = event_queue[event_queue_tail].ctxt;
      if (ctxt->handle == NULL)
      {
         event_queue_tail = (event_queue_tail + 1) % UART_MAX_EVENTS;
         continue;
      }

      if (event_queue[event_queue_tail].event == UART_PD_EVENT_TX_COMPLETION)
      {
         // If there are pending bytes, copy it local buffer and start the next transfer. Do not
         // notify the client.

         if(ctxt->user_pd_bytes_to_tx)
         {
            uint32 bytes_to_copy = (ctxt->user_pd_bytes_to_tx > UART_TX_BUF_SIZE) ? UART_TX_BUF_SIZE : ctxt->user_pd_bytes_to_tx;

            UART_LOG(INFO, "UART UART_PD_EVENT_TX_COMPLETION. Copying remaining bytes. %ld\n", bytes_to_copy);

            if (QURT_EOK != qurt_qdi_copy_from_user(client_handle,
                                                    (void *)ctxt->root_pd_tx_buf,
                                                    (void *)(ctxt->user_pd_tx_buf + ctxt->bytes_txed),
                                                    bytes_to_copy))
            {
               UART_LOG(INFO, "UART UART_PD_EVENT_TX_COMPLETION. Transmit failed. %ld\n", ctxt->bytes_txed);
            }

            else
            {
               // If the uart_transmit is success, continue to the next event as there is nothing to
               // report to client
               if (UART_SUCCESS == uart_transmit(ctxt->handle, ctxt->root_pd_tx_buf, bytes_to_copy, ctxt))
               {
                  event_queue_tail = (event_queue_tail + 1) % UART_MAX_EVENTS;

                  continue;
               }
            }
         }

         done_processing = TRUE;

         event_ctxt->client_tfr_cb = ctxt->user_tx_cb_isr;
         event_ctxt->cb_data       = ctxt->user_tx_cb_data;
         event_ctxt->bytes_transferred = ctxt->bytes_txed;
         event_ctxt->pd_event = event_queue[event_queue_tail].event;

         UART_LOG(INFO, "UART UART_PD_EVENT_TX_COMPLETION. Transmit done. %ld\n", ctxt->bytes_txed);
         ctxt->user_pd_tx_buf = NULL;
         ctxt->user_pd_bytes_to_tx = 0;
         ctxt->user_tx_cb_data = NULL;
         ctxt->bytes_txed = 0;

         event_queue_tail = (event_queue_tail + 1) % UART_MAX_EVENTS;
      }

      else if (event_queue[event_queue_tail].event == UART_PD_EVENT_RX_COMPLETION)
      {
         done_processing = TRUE;
         UART_LOG(INFO, "UART UART_PD_EVENT_RX_COMPLETION. Bytes rxed: %ld\n", event_queue[event_queue_tail].bytes_transferred);

         if (QURT_EOK != qurt_qdi_copy_to_user(client_handle,
                                               (void *)ctxt->user_pd_rx_buf,
                                               (void *)ctxt->root_pd_rx_buf,
                                               event_queue[event_queue_tail].bytes_transferred))
         {
            event_ctxt->bytes_transferred = 0;
         }
         else
         {
            event_ctxt->bytes_transferred = event_queue[event_queue_tail].bytes_transferred;
         }

         event_ctxt->client_tfr_cb = ctxt->user_rx_cb_isr;
         event_ctxt->cb_data       = ctxt->user_rx_cb_data;
         event_ctxt->pd_event = event_queue[event_queue_tail].event;

         event_queue_tail = (event_queue_tail + 1) % UART_MAX_EVENTS;
      }

      else if (event_queue[event_queue_tail].event == UART_PD_EVENT_WAKEUP)
      {
         done_processing = TRUE;

         event_ctxt->client_wakeup_cb = ctxt->user_wakeup_cb;
         event_ctxt->cb_data          = ctxt->user_wakeup_cb_data;
         event_ctxt->pd_event         = event_queue[event_queue_tail].event;

         event_queue_tail = (event_queue_tail + 1) % UART_MAX_EVENTS;

      }

   }while((!done_processing));

   UART_LOG(INFO, "-uart_root_wait_for_event.\n");
   return QURT_EOK;
}

/*==================================================================================================

FUNCTION: uart_root_wakeup_cb

DESCRIPTION:

==================================================================================================*/
static void uart_root_wakeup_cb(void* cb_data)
{
   qurt_mutex_lock (&uart_root_pd_mutex);

   event_queue[event_queue_head].bytes_transferred = 0;
   event_queue[event_queue_head].ctxt           = (uart_root_pd_context*)cb_data;
   event_queue[event_queue_head].event          = UART_PD_EVENT_WAKEUP;
   event_queue_head = (event_queue_head + 1) % UART_MAX_EVENTS;

   qurt_mutex_unlock (&uart_root_pd_mutex);

   qurt_signal_set(&uart_root_pd_signal, UART_SIGNAL);

}

/*==================================================================================================
                                             GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: qdi_uart_invocation

DESCRIPTION: function to handle calling all necessary functions in the Root PD

==================================================================================================*/
static int qdi_uart_invocation(int client_handle, qurt_qdi_obj_t *obj, int method,
                               qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                               qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                               qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{

   switch (method)
   {
      case QDI_OPEN:
         return uart_qdi_open(client_handle);

      case UART_OPEN:
         return uart_root_open(client_handle, (uart_handle*)a1.ptr, (uart_port_id) a2.num, (uart_open_config*)a3.ptr);

      case UART_TRANSMIT:
         return uart_root_transmit(client_handle,(uart_handle)a1.ptr, (uint8*)a2.ptr, (uint32)a3.num, (void*)a4.ptr);

      case UART_RECEIVE:
         return uart_root_receive(client_handle,(uart_handle)a1.ptr, (uint8*)a2.ptr, (uint32)a3.num, (void*)a4.ptr);

      case UART_WAIT_FOR_EVENT:
         return uart_root_wait_for_event(client_handle, (uart_pd_event_cb_ctxt*)a1.ptr);

      case UART_POWER_OFF:
         return uart_root_power_off(client_handle, (uart_handle)a1.ptr, (boolean)a2.num, (UART_WAKEUP_CALLBACK)a3.ptr, (void*)a4.ptr);

      case UART_POWER_ON:
         return uart_root_power_on(client_handle, (uart_handle)a1.ptr);

      case UART_CLOSE:
         return uart_root_close((uart_handle)a1.ptr);

      default:
         return qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6,
                              a7, a8, a9);
   }
}

/*==================================================================================================

FUNCTION: QDI_UART_release

DESCRIPTION: Free resources used by the QDI structs

==================================================================================================*/
static void qdi_uart_release(qurt_qdi_obj_t *obj)
{
   event_queue_head = event_queue_tail = 0;
   qurt_signal_clear(&uart_root_pd_signal, UART_SIGNAL);
}

/*==================================================================================================
                                             GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: qdi_uart_init

DESCRIPTION: Registers the UART QDI driver

==================================================================================================*/
void qdi_uart_init()
{
  qurt_mutex_init (&uart_root_pd_mutex);
  qurt_signal_init(&uart_root_pd_signal);

  uart_qdi_root_obj.invoke  = qdi_uart_invocation;
  uart_qdi_root_obj.refcnt  = QDI_REFCNT_PERM;
  uart_qdi_root_obj.release = qdi_uart_release;

  qurt_qdi_register_devname("uart", &uart_qdi_root_obj);
}
