/*==================================================================================================

FILE: DalUart.c

DESCRIPTION: UART DAL (OS-independent layer)

                           Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/dal/src/DalUart.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   Uart_ClearBreak
   Uart_Close
   Uart_CxmTxDirectChar
   Uart_DeviceDeInit
   Uart_DeviceInit
   Uart_DumpRegs
   Uart_GetCTS
   Uart_GetCxmTxSticky
   Uart_GetStatus
   Uart_Info
   Uart_LoopbackSet
   Uart_Open
   Uart_OpenEx
   Uart_PostInit
   Uart_PowerCapabilities
   Uart_PowerEvent
   Uart_PowerGet
   Uart_PowerSet
   Uart_PurgeRx
   Uart_PurgeTx
   Uart_Read
   Uart_RxActive
   Uart_SetBreak
   Uart_SetCharFormat
   Uart_SetCtsControl
   Uart_SetCxm
   Uart_SetRate
   Uart_SetRtsControl
   Uart_TxComplete
   Uart_TxSingleChar
   Uart_Write

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#include "DalUart.tmh"
#else
#include "comdef.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIGPIOInt.h"
#endif

#include "tal.h"
#include "DalUart.h"
#include "DalUartDma.h"
#include "DalUartFwk.h"
#include "HALuart.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define UART_MEM_SIZE 4096
#define GSBI_MEM_SIZE 4096

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#define DMOV(uart_ctxt)   (uart_ctxt->uart_type == HAL_UART_DM)
#define BAM(uart_ctxt)    (uart_ctxt->uart_type == HAL_UART_BAM)

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef struct
{
   uint32 baud_rate;
   uint32 input_freq;
   uint32 divider;
} CLOCK_SETTINGS;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

// These baud table entries must satisfy the following criteria:
//
// baud_rate = input_freq / divider
// divider = one of {  16,  32,  48,   64,   96,  128,   192,   256,
//                       384, 512, 768, 1536, 3072, 6144, 12288, 24576  }

static const CLOCK_SETTINGS baud_table[] =
{
   // baud_rate, input_freq, divider
   {        300,    7372800,   24576 },
   {        600,    7372800,   12288 },
   {       1200,    7372800,    6144 },
   {       2400,    7372800,    3072 },
   {       4800,    7372800,    1536 },
   {       9600,    7372800,     768 },
   {      14400,    7372800,     512 },
   {      19200,    7372800,     384 },
   {      28800,    7372800,     256 },
   {      38400,    7372800,     192 },
   {      57600,    7372800,     128 },
   {     115200,    7372800,      64 },
   {     153600,    7372800,      48 },
   {     230400,    7372800,      32 },
   {     460800,    7372800,      16 },
   {     921600,   14745600,      16 },
   {    1000000,   16000000,      16 },
   {    1200000,   19200000,      16 },
   {    1209677,   19354839,      16 }, // Rounding error intended
   {    1250000,   20000000,      16 },
   {    1293103,   20689655,      16 }, // Rounding error intended
   {    1339286,   21428571,      16 }, // Rounding error intended
   {    1388889,   22222222,      16 }, // Rounding error intended
   {    1442308,   23076923,      16 }, // Rounding error intended
   {    1500000,   24000000,      16 },
   {    1562500,   25000000,      16 },
   {    1630435,   26086957,      16 }, // Rounding error intended
   {    1704545,   27272727,      16 }, // Rounding error intended
   {    1785714,   28571429,      16 }, // Rounding error intended
   {    2000000,   32000000,      16 },
   {    2500000,   40000000,      16 },
   {    3000000,   48000000,      16 },
   {    3200000,   51200000,      16 },
   {    3500000,   56000000,      16 },
   {    3686400,   58982400,      16 },
   {    4000000,   64000000,      16 },
   { 0 }  // blank entry to mark end of table
};

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static boolean       allocate_buffers        (UART_CONTEXT *uart_ctxt);
static boolean       check_rx_error          (UART_CONTEXT *uart_ctxt);
static boolean       classify_uart           (DALDEVICEID device, HALUartType *uart_type);
static void          clock_disable           (UART_CONTEXT *uart_ctxt);
static boolean       clock_enable            (UART_CONTEXT *uart_ctxt);
static void          close_devices           (UART_CONTEXT *uart_ctxt);
static void          configure_gsbi          (UART_CONTEXT *uart_ctxt);
static boolean       configure_interrupts    (UART_CONTEXT *uart_ctxt);
static boolean       create_objects          (UART_CONTEXT *uart_ctxt, UartInitConfig *init_cfg);
static void          deallocate_buffers      (UART_CONTEXT *uart_ctxt);
static void          deregister_isr_for_rx   (UART_CONTEXT *uart_ctxt);
static void          deregister_isr_for_uart (UART_CONTEXT *uart_ctxt);
static void          destroy_objects         (UART_CONTEXT *uart_ctxt);
static void          dump_regs               (UART_CONTEXT *uart_ctxt);
static boolean       get_properties          (UART_CONTEXT *uart_ctxt);
static void          get_property_dword      (UART_CONTEXT *uart_ctxt, DALSYSPropertyHandle handle,
                                                                       const char *property_name,
                                                                       uint32 *property_value,
                                                                       uint32 default_value);
static void          gpio_disable            (UART_CONTEXT *uart_ctxt);
static boolean       gpio_enable             (UART_CONTEXT *uart_ctxt);
static void          halt_line_detection     (UART_CONTEXT *uart_ctxt, boolean skip_halt_edge);
static void          halt_rx_path            (UART_CONTEXT *uart_ctxt);
static void          halt_tx_path            (UART_CONTEXT *uart_ctxt);
static boolean       map_hardware            (UART_CONTEXT *uart_ctxt);
static void          mu_close                (UART_CONTEXT *uart_ctxt);
static void          mu_deinit               (UART_CONTEXT *uart_ctxt);
static UART_CONTEXT *mu_init                 (UartInitConfig *init_cfg, DALDEVICEID device);
static boolean       mu_open                 (UART_CONTEXT *uart_ctxt, UartOpenConfig *open_cfg);
static void          notify_events           (UART_CONTEXT *uart_ctxt, uint32 events);
static void          notify_rxdata_available (UART_CONTEXT *uart_ctxt);
static void          notify_transmit_complete(UART_CONTEXT *uart_ctxt);
static boolean       open_devices            (UART_CONTEXT *uart_ctxt);
static DALResult     power_025               (UART_CONTEXT *uart_ctxt);
static DALResult     power_off               (UART_CONTEXT *uart_ctxt);
static DALResult     power_on                (UART_CONTEXT *uart_ctxt);
static boolean       prepare_for_sleep       (UART_CONTEXT *uart_ctxt);
static void          read_rx_fifo            (UART_CONTEXT *uart_ctxt, uint32 num_bytes);
static void          receive_chunk           (UART_CONTEXT *uart_ctxt);
static boolean       reg_init                (UART_CONTEXT *uart_ctxt, UartOpenConfig *open_cfg);
static boolean       register_isr_for_rx     (UART_CONTEXT *uart_ctxt, RX_TRIGGER_TYPE rx_trigger);
static boolean       register_isr_for_uart   (UART_CONTEXT *uart_ctxt);
static boolean       resume_from_sleep       (UART_CONTEXT *uart_ctxt);
static void          rx_dma_callback         (void *context);
static void *        rx_dpc                  (void *context);
static void *        rx_isr                  (void *context);
static void          rx_transfer_complete    (UART_CONTEXT *uart_ctxt, boolean *rx_data,
                                                                       boolean *rx_error);
static boolean       rx_transfer_start       (UART_CONTEXT *uart_ctxt);
static void          service_rx_break_start  (UART_CONTEXT *uart_ctxt);
static void          service_rx_dma          (UART_CONTEXT *uart_ctxt);
static void          service_rx_falling      (UART_CONTEXT *uart_ctxt);
static void          service_rx_rising       (UART_CONTEXT *uart_ctxt);
static void          service_rx_stale        (UART_CONTEXT *uart_ctxt);
static void          service_rx_watermark    (UART_CONTEXT *uart_ctxt);
static void          service_tx_dma          (UART_CONTEXT *uart_ctxt);
static void          service_tx_ready        (UART_CONTEXT *uart_ctxt);
static void          service_tx_watermark    (UART_CONTEXT *uart_ctxt);
static boolean       set_baud_rate           (UART_CONTEXT *uart_ctxt, uint32 baud_rate);
static void          set_char_format         (UART_CONTEXT *uart_ctxt, UartCharFormat *char_format);
static boolean       set_input_freq          (UART_CONTEXT *uart_ctxt, uint32 input_freq);
static void          set_rts_control         (UART_CONTEXT *uart_ctxt,
                                              UartRtsControlType rts_control);
static void          start_break_detection   (UART_CONTEXT *uart_ctxt);
static boolean       transmit_chunk          (UART_CONTEXT *uart_ctxt);
static void          tx_dma_callback         (void *context);
static void *        uart_dpc                (void *context);
static void *        uart_isr                (void *context);
static void          unmap_hardware          (UART_CONTEXT *uart_ctxt);
static boolean       valid_char_format       (UartCharFormat *char_format);
static boolean       valid_rts_control       (UartRtsControlType rts_control);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: allocate_buffers

DESCRIPTION:

==================================================================================================*/
static boolean allocate_buffers(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result;
   uint32 attributes;
   uint32 i;

   if (uart_ctxt->use_dma)  // use uncached buffers for DMA transfers
   {
      attributes = DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT;
   }
   else  // use regular cached buffers when not using DMA
   {
      attributes = DALSYS_MEM_PROPS_CACHED_WB | DALSYS_MEM_PROPS_PHYS_CONT;
   }

   result = DALSYS_MemRegionAlloc(attributes, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED, TX_BUF_SIZE,
                                  &uart_ctxt->tx_handle, NULL);

   if (result != DAL_SUCCESS) { goto error; }

   for (i = 0; i < RX_BUF_CNT; i++)
   {
      MSM_UART_RX_BUF *rx_buf = &uart_ctxt->rx_buf[i];

      result = DALSYS_MemRegionAlloc(attributes, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                     DALSYS_MEM_ADDR_NOT_SPECIFIED, RX_BUF_SIZE,
                                     &rx_buf->data_handle, NULL);

      if (result != DAL_SUCCESS) { goto error; }

      rx_buf->write_offset = 0;
      rx_buf->read_offset = 0;
   }

   uart_ctxt->rx_head = 0;
   uart_ctxt->rx_tail = 0;
   uart_ctxt->rx_out_of_buffers = FALSE;

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "allocate_buffers: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: check_rx_error

DESCRIPTION:
   Read UART's status register and process indications appropriately.

==================================================================================================*/
static boolean check_rx_error(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   boolean rx_error = FALSE;
   uint32 status;

   status = HALuart_GetStatus(uart_hal);

   if ( status & HAL_UART_STATUS_OVERRUN )
   {
      uart_ctxt->rx_overruns++;
      uart_ctxt->rx_cached_status |= STATUS_RX_OVERRUN_ERROR;
      rx_error = TRUE;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "check_rx_error: overrun detected");
   }

   if ( status & HAL_UART_STATUS_PARITY_FRAME_ERROR )
   {
      // we don't know which error so we will report them both
      uart_ctxt->rx_cached_status |= STATUS_RX_PARITY_ERROR;
      uart_ctxt->rx_cached_status |= STATUS_RX_FRAME_ERROR;
      rx_error = TRUE;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "check_rx_error: parity/frame error detected");
   }

   if (rx_error) { HALuart_ResetErrorStatus(uart_hal); }

   // Any kind of error recovery should probably be done here when we are between
   // RX transfers.  Beware of resetting the receiver because that will force RTS
   // to be deasserted if auto-RTS is not enabled.  That might conflict with the
   // client's previous setting.

   return(rx_error);
}

/*==================================================================================================

FUNCTION: classify_uart

DESCRIPTION:

==================================================================================================*/
static boolean classify_uart(DALDEVICEID device, HALUartType *uart_type)
{
   switch (device)
   {
      case DALDEVICEID_UARTDM_DEVICE_1:
      case DALDEVICEID_UARTDM_DEVICE_2:
      case DALDEVICEID_UARTGSBI_DEVICE_0:
      case DALDEVICEID_UARTGSBI_DEVICE_1:
      case DALDEVICEID_UARTGSBI_DEVICE_2:
      case DALDEVICEID_UARTGSBI_DEVICE_3:
      case DALDEVICEID_UARTGSBI_DEVICE_4:
      case DALDEVICEID_UARTGSBI_DEVICE_5:
      case DALDEVICEID_UARTGSBI_DEVICE_6:
      case DALDEVICEID_UARTGSBI_DEVICE_7:
      case DALDEVICEID_UARTGSBI_DEVICE_8:
      case DALDEVICEID_UARTGSBI_DEVICE_9:
      case DALDEVICEID_UARTGSBI_DEVICE_10:
      case DALDEVICEID_UARTGSBI_DEVICE_11:
      case DALDEVICEID_UARTGSBI_DEVICE_12:
         *uart_type = HAL_UART_DM; return(TRUE);
      case DALDEVICEID_UARTBAM_DEVICE_1:
      case DALDEVICEID_UARTBAM_DEVICE_2:
      case DALDEVICEID_UARTBAM_DEVICE_3:
      case DALDEVICEID_UARTBAM_DEVICE_4:
      case DALDEVICEID_UARTBAM_DEVICE_5:
      case DALDEVICEID_UARTBAM_DEVICE_6:
      case DALDEVICEID_UARTBAM_DEVICE_7:
      case DALDEVICEID_UARTBAM_DEVICE_8:
      case DALDEVICEID_UARTBAM_DEVICE_9:
      case DALDEVICEID_UARTBAM_DEVICE_10:
      case DALDEVICEID_UARTBAM_DEVICE_11:
      case DALDEVICEID_UARTBAM_DEVICE_12:
      case DALDEVICEID_UARTCXM:
         *uart_type = HAL_UART_BAM; return(TRUE);
      default:
         return(FALSE);
   }
}

/*==================================================================================================

FUNCTION: clock_disable

DESCRIPTION:
   This function disables the input clock(s).

==================================================================================================*/
static void clock_disable(UART_CONTEXT *uart_ctxt)
{
   tal_clock_disable(uart_ctxt->clock_handle);

   uart_ctxt->clock_on = FALSE;
}

/*==================================================================================================

FUNCTION: clock_enable

DESCRIPTION:
   This function enables the input clock(s).

==================================================================================================*/
static boolean clock_enable(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   if (tal_clock_enable(uart_ctxt->clock_handle) == TAL_SUCCESS)
   {
      uart_ctxt->clock_on = TRUE;
      return TRUE;
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "clock_enable: failed");
      return FALSE;
   }
}

/*==================================================================================================

FUNCTION: close_devices

DESCRIPTION:

==================================================================================================*/
static void close_devices(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt->interrupt_handle)
   {
       tal_interrupt_close(uart_ctxt->interrupt_handle);
       uart_ctxt->interrupt_handle = NULL;
   }

   if (uart_ctxt->tlmm_handle)
   {
      tal_tlmm_close(uart_ctxt->tlmm_handle);
      uart_ctxt->tlmm_handle = NULL;
   }

   if (uart_ctxt->clock_handle)
   {
      tal_clock_close(uart_ctxt->clock_handle);
      uart_ctxt->clock_handle = NULL;
   }
}

/*==================================================================================================

FUNCTION: configure_gsbi

DESCRIPTION:
   This function configures the GSBI HW block as a UART.  The GSBI DAL should be coming
   to replace this.

==================================================================================================*/
static void configure_gsbi(UART_CONTEXT *uart_ctxt)
{
   DALSYSMemInfo meminfo;
   uint8 *gsbi;

   if (uart_ctxt->prop_gsbi_control)
   {
      DALSYS_MemInfo(uart_ctxt->gsbi_mem_handle, &meminfo);
      gsbi = (uint8 *)meminfo.VirtualAddr;

      *(volatile uint32 *)(gsbi + 0x0000) = uart_ctxt->prop_gsbi_control;
   }
}

/*==================================================================================================

FUNCTION: configure_interrupts

DESCRIPTION:

==================================================================================================*/
static boolean configure_interrupts(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   uint32      rx_gpio;

   // The UART IRQ is mandatory.  It can be caller-specified (passed into Uart_PostInit)
   // or read from the DAL properties (caller-specified takes precedence).  A nonzero
   // value is considered specified.

   if (uart_ctxt->init_uart_irq)  // caller-specified
   {
      uart_ctxt->uart_interrupt.irq = uart_ctxt->init_uart_irq;
   }
   else if (uart_ctxt->prop_uart_irq)  // from DAL properties
   {
      uart_ctxt->uart_interrupt.irq = (void *)uart_ctxt->prop_uart_irq;
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "configure_interrupts: UART IRQ unspecified");
      return FALSE;
   }

   // The RX IRQ is optional (it is only needed for break detection and inband sleep).
   // It can be caller-specifed as an IRQ or obtained from TLMM driver as a GPIO
   // (caller-specified takes precedence).

   if (uart_ctxt->init_rx_irq)  // caller-specified
   {
      uart_ctxt->rx_interrupt.irq = uart_ctxt->init_rx_irq;
   }
   else if (tal_tlmm_get_rx_gpio_num(uart_ctxt->tlmm_handle, &rx_gpio) == TAL_SUCCESS)
   {
      uart_ctxt->rx_interrupt.gpio = rx_gpio;
   }

   return TRUE;
}

/*==================================================================================================

FUNCTION: create_objects

DESCRIPTION:

==================================================================================================*/
static boolean create_objects(UART_CONTEXT *uart_ctxt, UartInitConfig *init_cfg)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DALSYSCallbackFunc line_callback = (DALSYSCallbackFunc)    init_cfg->LineEventNotif;
   DALSYSCallbackFunc tx_callback   = (DALSYSCallbackFunc)    init_cfg->TransmitReadyNotif;
   DALSYSCallbackFunc rx_callback   = (DALSYSCallbackFunc)    init_cfg->ReceiveReadyNotif;
   DALSYSCallbackFuncCtx cb_ctxt    = (DALSYSCallbackFuncCtx) init_cfg->Ctxt;
   DALResult result;

   if (line_callback)
   {
      result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &uart_ctxt->line_event, NULL);
      if (result != DAL_SUCCESS) { goto error; }

      result = DALSYS_SetupCallbackEvent(uart_ctxt->line_event, line_callback, cb_ctxt);
      if (result != DAL_SUCCESS) { goto error; }
   }

   if (tx_callback)
   {
      result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &uart_ctxt->tx_event, NULL);
      if (result != DAL_SUCCESS) { goto error; }

      result = DALSYS_SetupCallbackEvent(uart_ctxt->tx_event, tx_callback, cb_ctxt);
      if (result != DAL_SUCCESS) { goto error; }
   }

   if (rx_callback)
   {
      result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &uart_ctxt->rx_event, NULL);
      if (result != DAL_SUCCESS) { goto error; }

      result = DALSYS_SetupCallbackEvent(uart_ctxt->rx_event, rx_callback, cb_ctxt);
      if (result != DAL_SUCCESS) { goto error; }
   }

   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &uart_ctxt->tx_sync, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &uart_ctxt->rx_sync, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &uart_ctxt->register_sync, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "create_objects: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: deallocate_buffers

DESCRIPTION:

==================================================================================================*/
static void deallocate_buffers(UART_CONTEXT *uart_ctxt)
{
   uint32 i;

   if (uart_ctxt->tx_handle)
   {
      DALSYS_DestroyObject(uart_ctxt->tx_handle);
      uart_ctxt->tx_handle = NULL;
   }

   for (i = 0; i < RX_BUF_CNT; i++)
   {
      if (uart_ctxt->rx_buf[i].data_handle)
      {
         DALSYS_DestroyObject(uart_ctxt->rx_buf[i].data_handle);
         uart_ctxt->rx_buf[i].data_handle = NULL;
      }
   }
}

/*==================================================================================================

FUNCTION: deregister_isr_for_rx

DESCRIPTION:

==================================================================================================*/
static void deregister_isr_for_rx(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt->rx_isr_registered)
   {
      tal_interrupt_deregister(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
      uart_ctxt->rx_trigger = NO_TRIGGER;
      uart_ctxt->rx_isr_registered = FALSE;
   }
}

/*==================================================================================================

FUNCTION: deregister_isr_for_uart

DESCRIPTION:

==================================================================================================*/
static void deregister_isr_for_uart(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt->uart_isr_registered)
   {
      tal_interrupt_deregister(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      uart_ctxt->uart_isr_registered = FALSE;
   }
}

/*==================================================================================================

FUNCTION: destroy_objects

DESCRIPTION:

==================================================================================================*/
static void destroy_objects(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt->line_event)    { DALSYS_DestroyObject(uart_ctxt->line_event);    }
   if (uart_ctxt->tx_event)      { DALSYS_DestroyObject(uart_ctxt->tx_event);      }
   if (uart_ctxt->rx_event)      { DALSYS_DestroyObject(uart_ctxt->rx_event);      }
   if (uart_ctxt->tx_sync)       { DALSYS_DestroyObject(uart_ctxt->tx_sync);       }
   if (uart_ctxt->rx_sync)       { DALSYS_DestroyObject(uart_ctxt->rx_sync);       }
   if (uart_ctxt->register_sync) { DALSYS_DestroyObject(uart_ctxt->register_sync); }
}

/*==================================================================================================

FUNCTION: dump_regs

DESCRIPTION:

==================================================================================================*/
static void dump_regs(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   HALUartRegisters registers;

   HALuart_GetRegisters(uart_hal, &registers);

   if ( DMOV(uart_ctxt) )
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MR1  (0x00) - 0x%08X", registers.mr1);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MR2  (0x04) - 0x%08X", registers.mr2);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SR   (0x08) - 0x%08X", registers.sr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MISR (0x10) - 0x%08X", registers.misr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: ISR  (0x14) - 0x%08X", registers.isr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: IPR  (0x18) - 0x%08X", registers.ipr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: TFWR (0x1C) - 0x%08X", registers.tfwr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: RFWR (0x20) - 0x%08X", registers.rfwr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: HCR  (0x24) - 0x%08X", registers.hcr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: DMRX (0x34) - 0x%08X", registers.dmrx);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SNAP (0x38) - 0x%08X", registers.snap);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: DMEN (0x3C) - 0x%08X", registers.dmen);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: NCFT (0x40) - 0x%08X", registers.ncft);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: BADR (0x44) - 0x%08X", registers.badr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: TXFS (0x4C) - 0x%08X", registers.txfs);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: RXFS (0x50) - 0x%08X", registers.rxfs);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SIM  (0x80) - 0x%08X", registers.sim);
   }
   else if ( BAM(uart_ctxt) )
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MR1  (0x00) - 0x%08X", registers.mr1);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MR2  (0x04) - 0x%08X", registers.mr2);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: IPR  (0x18) - 0x%08X", registers.ipr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: TFWR (0x1C) - 0x%08X", registers.tfwr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: RFWR (0x20) - 0x%08X", registers.rfwr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: HCR  (0x24) - 0x%08X", registers.hcr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: DMRX (0x34) - 0x%08X", registers.dmrx);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: DMEN (0x3C) - 0x%08X", registers.dmen);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: NCFT (0x40) - 0x%08X", registers.ncft);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: BADR (0x44) - 0x%08X", registers.badr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: TXFS (0x4C) - 0x%08X", registers.txfs);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: RXFS (0x50) - 0x%08X", registers.rxfs);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SIM  (0x80) - 0x%08X", registers.sim);

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: CSR  (0xA0) - 0x%08X", registers.csr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SR   (0xA4) - 0x%08X", registers.sr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: MISR (0xAC) - 0x%08X", registers.misr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: IMR  (0xB0) - 0x%08X", registers.imr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: ISR  (0xB4) - 0x%08X", registers.isr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: IRDA (0xB8) - 0x%08X", registers.irda);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: SNAP (0xBC) - 0x%08X", registers.snap);

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: CCTL (0xC4) - 0x%08X", registers.cctl);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: BCR  (0xC8) - 0x%08X", registers.bcr);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: RCTL (0xCC) - 0x%08X", registers.rctl);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: DDBG (0xD0) - 0x%08X", registers.ddbg);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: FSM  (0xD4) - 0x%08X", registers.fsm);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: VER  (0xD8) - 0x%08X", registers.ver);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UartDbg: GEN  (0xDC) - 0x%08X", registers.gen);
   }
}

/*==================================================================================================

FUNCTION: get_properties

DESCRIPTION:
   Get statically configurable DAL properties from XML file

==================================================================================================*/
static boolean get_properties(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   DALResult result;

   // Gpioxxxx: These are the TLMM GPIO configurations.  If the configurations are not
   //           present we don't need to configure the GPIOs ourselves.  For example, on
   //           some targets (ie. WM 8K) the BT driver does the GPIO configuration for us.

   // IsCxm: Some UART cores support two-wire LTE/ISM coexistence ("CXM"). If
   //        this flag is true, the UART driver will read and write the CXM
   //        registers.

   result = DALSYS_GetDALPropertyHandle(uart_ctxt->dal_id, handle);
   if (result != DAL_SUCCESS) { goto error; }

   get_property_dword(uart_ctxt, handle, "UartBase",    &uart_ctxt->prop_uart_physical, 0);
   get_property_dword(uart_ctxt, handle, "GsbiBase",    &uart_ctxt->prop_gsbi_physical, 0);
   get_property_dword(uart_ctxt, handle, "GsbiControl", &uart_ctxt->prop_gsbi_control,  0);
   get_property_dword(uart_ctxt, handle, "DetectBrk",   &uart_ctxt->prop_detect_break,  FALSE);
   get_property_dword(uart_ctxt, handle, "IsCxm",       &uart_ctxt->prop_is_cxm,        FALSE);
   get_property_dword(uart_ctxt, handle, "Irq",         &uart_ctxt->prop_uart_irq,      0);

   if (uart_ctxt->prop_uart_physical == 0) { goto error; }
   if (uart_ctxt->prop_gsbi_physical == 0 && uart_ctxt->prop_gsbi_control) { goto error; }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "get_properties: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(UART_CONTEXT *uart_ctxt, DALSYSPropertyHandle handle,
                               const char *property_name, uint32 *property_value,
                               uint32 default_value)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X",
                                                property_name, *property_value);
   }
   else
   {
      *property_value = default_value;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X (defaulted)",
                                                property_name, *property_value);
   }
}

/*==================================================================================================

FUNCTION: gpio_disable

DESCRIPTION:
   This function deconfigures the UART GPIO lines.

==================================================================================================*/
static void gpio_disable(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   TAL_RESULT result;

   result = tal_tlmm_gpio_disable(uart_ctxt->tlmm_handle);

   if (result != TAL_SUCCESS)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "gpio_disable: failed");
   }
}

/*==================================================================================================

FUNCTION: gpio_enable

DESCRIPTION:
   This function configures the GPIO lines for use by the UART.

==================================================================================================*/
static boolean gpio_enable(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   TAL_RESULT result;

   result = tal_tlmm_gpio_enable(uart_ctxt->tlmm_handle);

   if (result != TAL_SUCCESS)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "gpio_enable: failed");
      return FALSE;
   }

   return TRUE;
}

/*==================================================================================================

FUNCTION: halt_line_detection

DESCRIPTION:
   This function halts detection of various RX line events:

   break start (during POWER_FULL)            - RX logic low for 10 bits
   break end   (during POWER_FULL, POWER_OFF) - RX rising edge
   RX wakeup   (during POWER_025)             - RX falling edge

   Note that only one type of line detection will be active at any given time.  This
   function is called when closing the port and during power state transitions.

==================================================================================================*/
static void halt_line_detection(UART_CONTEXT *uart_ctxt, boolean skip_halt_edge)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+halt_line_detection");

   // Disabling the RX DPC will be a no-op if the RX ISR is not registered.  That is
   // OK as long as nobody registers the RX ISR while we are in the critical section.
   // We can ensure that by disabling the UART DPC first.

   tal_interrupt_dpc_disable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   tal_interrupt_dpc_disable(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);

   uart_ctxt->break_halt = TRUE;  // prevent restart of break detection during POWER_OFF

   // Halt break start detection.  This is only necessary in the POWER_FULL state
   // because it has already been done in other states.  Attempting to do it again
   // will cause an unclocked register access.

   if (uart_ctxt->power_state == POWER_FULL)
   {
      tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      uart_ctxt->uart_events &= ~IRQ_RXBREAK_START;
      HALuart_DisableInterrupts(uart_hal, HAL_UART_RXBREAK_START_IMR_FLAG);
      tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   }

   // Halt edge detection (RX wakeup and break end).  This will be skipped during
   // POWER_FULL -> POWER_OFF transitions because we need to detect break end during
   // POWER_OFF.

   if (!skip_halt_edge)
   {
      tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
      uart_ctxt->rx_events &= ~IRQ_RX_FALLING;
      uart_ctxt->rx_events &= ~IRQ_RX_RISING;
      deregister_isr_for_rx(uart_ctxt);
      tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
   }

   tal_interrupt_dpc_enable(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
   tal_interrupt_dpc_enable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-halt_line_detection");
}

/*==================================================================================================

FUNCTION: halt_rx_path

DESCRIPTION:

==================================================================================================*/
static void halt_rx_path(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+halt_rx_path");

   DALSYS_SyncEnter(uart_ctxt->rx_sync);
   uart_ctxt->rx_halt = TRUE;
   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   if (uart_ctxt->use_dma)
   {
      // Disable RX CRCI interface (via the DMEN register) *before* flushing to prevent
      // DMOV and UART from getting out of sync when we reset the receiver below.
      // Note that it would not be sufficient to disable the receiver here.  If there is
      // a partial burst (less than 16 bytes) in the RX FIFO and/or packing buffer then
      // a CRCI request will be generated when the stale timeout occurs even if the
      // receiver has been disabled.

      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetRxDmaEnable(uart_hal, FALSE);
      DALSYS_SyncLeave(uart_ctxt->register_sync);

      dma_rx_purge(uart_ctxt->dma_ctxt);
      dma_rx_wait_for_idle(uart_ctxt->dma_ctxt);
   }

   // Temporarily disable the DPC and synchronize with the ISR.  Clear any unprocessed
   // RX DPC events and prevent the signaling of new ones.  Then re-enable the DPC.

   tal_interrupt_dpc_disable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   uart_ctxt->uart_events &= ~IRQ_RXSTALE;
   uart_ctxt->uart_events &= ~IRQ_RXLEV;

   HALuart_DisableInterrupts(uart_hal, HAL_UART_RXSTALE_IMR_FLAG);
   HALuart_DisableInterrupts(uart_hal, HAL_UART_RXLEV_IMR_FLAG);

   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   tal_interrupt_dpc_enable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   // HW bug: Contrary to what the SWI says resetting the receiver does not disable
   // it.  In fact, resetting the receiver will enable it even if it was previously
   // disabled (this was observed on 8960).  Therefore we must reset and disable,
   // in that order.  Note that this is not a perfect solution because there is
   // still a window (between the reset and disable) where data can unexpectedly
   // enter the RXFIFO.

   // HW bug: Disabling the receiver does not disable the stale event mechanism.
   // We must do this explicitly.  If we fail to do so we may experience a "partial
   // stale event" later when we restart the RX path.  This scenario is triggered
   // by the following sequence of events: (1) write DMRX (2) stale event occurs
   // (3) enable stale event (4) stale event occurs.  The stale event in step (4)
   // will be a "partial stale event" where data is flushed from the packing buffer
   // to the RXFIFO but SNAP is not updated.  This creates an unexpected gap in the
   // FIFO and introduces phantom NULL characters into the RX stream.  By disabling
   // stale events now we will prevent the stale event in step (2) and avoid this
   // scenario.

   HALuart_ResetReceiver(uart_hal);
   HALuart_SetRxEnable(uart_hal, FALSE);    // disable receiver
   HALuart_SetStaleEvent(uart_hal, FALSE);  // disable stale events

   uart_ctxt->rx_activated = FALSE;

   if (uart_ctxt->use_dma)
   {
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetRxDmaEnable(uart_hal, TRUE);
      DALSYS_SyncLeave(uart_ctxt->register_sync);
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-halt_rx_path");
}

/*==================================================================================================

FUNCTION: halt_tx_path

DESCRIPTION:

==================================================================================================*/
static void halt_tx_path(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+halt_tx_path");

   DALSYS_SyncEnter(uart_ctxt->tx_sync);
   uart_ctxt->tx_halt = TRUE;
   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   if (uart_ctxt->use_dma)
   {
      // Disable TX CRCI interface (via the DMEN register) *before* flushing to prevent
      // DMOV and UART from getting out of sync when we reset the transmitter below.

      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetTxDmaEnable(uart_hal, FALSE);
      DALSYS_SyncLeave(uart_ctxt->register_sync);

      dma_tx_purge(uart_ctxt->dma_ctxt);
      dma_tx_wait_for_idle(uart_ctxt->dma_ctxt);
   }

   // Temporarily disable the DPC and synchronize with the ISR.  Clear any unprocessed
   // TX DPC events and prevent the signaling of new ones.  Then re-enable the DPC.

   tal_interrupt_dpc_disable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   uart_ctxt->uart_events &= ~IRQ_TXLEV;
   uart_ctxt->uart_events &= ~IRQ_TX_READY;

   HALuart_DisableInterrupts(uart_hal, HAL_UART_TXLEV_IMR_FLAG);
   HALuart_DisableInterrupts(uart_hal, HAL_UART_TX_READY_IMR_FLAG);

   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   tal_interrupt_dpc_enable(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   HALuart_ResetTransmitter(uart_hal);

   if (uart_ctxt->use_dma)
   {
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetTxDmaEnable(uart_hal, TRUE);
      DALSYS_SyncLeave(uart_ctxt->register_sync);
   }

   DALSYS_SyncEnter(uart_ctxt->tx_sync);

   uart_ctxt->tx_ptr = NULL;
   uart_ctxt->tx_bytes_left = 0;
   uart_ctxt->tx_in_progress = FALSE;

   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-halt_tx_path");
}

/*==================================================================================================

FUNCTION: map_hardware

DESCRIPTION:
   This function generates the virtual addresses to access the physical hardware blocks.

==================================================================================================*/
static boolean map_hardware(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result;

   result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  (DALSYSMemAddr)uart_ctxt->prop_uart_physical,
                                  UART_MEM_SIZE, &uart_ctxt->uart_mem_handle, NULL);

   if (result != DAL_SUCCESS) { goto error; }

   if (uart_ctxt->prop_gsbi_control)
   {
      result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                     DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                     (DALSYSMemAddr)uart_ctxt->prop_gsbi_physical,
                                     GSBI_MEM_SIZE, &uart_ctxt->gsbi_mem_handle, NULL);

      if (result != DAL_SUCCESS) { goto error; }
   }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "map_hardware: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: mu_close

DESCRIPTION:
   The DAL client must ensure that no threads are inside any of the UART DAL APIs before
   calling this function (via DalDevice_Close).  On WM this is achieved via the device
   manager and COM_PreClose().  On AMSS this is achieved by forcing the DAL client to be
   single threaded.

==================================================================================================*/
static void mu_close(UART_CONTEXT *uart_ctxt)
{
   // if mu_close is called in mu_open failure and before open_devices succeeded, the call to halt_line_detection
   // will cause crash because of NULL interrupt handle
   if (uart_ctxt->interrupt_handle)
   {
      halt_line_detection(uart_ctxt, FALSE);
   }

   if (uart_ctxt->power_state == POWER_FULL)
   {
      halt_rx_path(uart_ctxt);
      halt_tx_path(uart_ctxt);
   }

   deregister_isr_for_uart(uart_ctxt);

   if (uart_ctxt->use_dma) { dma_close(uart_ctxt->dma_ctxt); }

   if (uart_ctxt->clock_handle != NULL)
   {
      if (uart_ctxt->clock_on || clock_enable(uart_ctxt))
      {
         if (uart_ctxt->prop_is_cxm)
         {
            // If this hardware core supports coexistence, reset the mux so
            // hardware CXM can be used.

            DALSYS_SyncEnter(uart_ctxt->register_sync);
            HALuart_SetCxmEnable(&uart_ctxt->uart_hal, HAL_UART_CXM_DISABLE, 0);
            DALSYS_SyncLeave(uart_ctxt->register_sync);
         }

         set_rts_control(uart_ctxt, RTS_DEASSERT);
         clock_disable(uart_ctxt);
      }
   }

   if (uart_ctxt->tlmm_handle != NULL)
   {
      gpio_disable(uart_ctxt);
   }
   close_devices(uart_ctxt);
   deallocate_buffers(uart_ctxt);
   uart_ctxt->power_state = POWER_OFF;
}

/*==================================================================================================

FUNCTION: mu_deinit

DESCRIPTION:
   The DAL client must ensure that they have closed the UART DAL and no threads are inside
   any of the UART DAL APIs before calling this function (via DAL_DeviceDetach).  On WM this
   is achieved via the device manager and COM_PreDeinit().  On AMSS this is achieved by
   forcing the DAL client to be single threaded.

==================================================================================================*/
static void mu_deinit(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt)
   {
      if (uart_ctxt->use_dma) { dma_deinit(uart_ctxt->dma_ctxt); }

      destroy_objects(uart_ctxt);
      unmap_hardware(uart_ctxt);
      DALSYS_Free(uart_ctxt);
   }
}

/*==================================================================================================

FUNCTION: mu_init

DESCRIPTION:
   The DAL client must ensure that they only call this function once per UART (via
   DalUart_PostInit) since we are not using a flag and critical section to control
   access.

==================================================================================================*/
static UART_CONTEXT *mu_init(UartInitConfig *init_cfg, DALDEVICEID device)
{
   UART_CONTEXT *uart_ctxt = NULL;
   HALUartObject *uart_hal;
   DALSYSMemInfo meminfo;
   TAL_DMA_CONFIG dma_config;

   if ( DALSYS_Malloc(sizeof(UART_CONTEXT), (void **)&uart_ctxt) != DAL_SUCCESS ) { goto error; }
   memset(uart_ctxt, 0, sizeof(UART_CONTEXT));

   if ( !classify_uart(device, &uart_ctxt->uart_type) ) { goto error; }

   uart_ctxt->dal_id        = device;
   uart_ctxt->use_dma       = (boolean)init_cfg->enable_dma;
   uart_ctxt->power_state   = POWER_OFF;
   uart_ctxt->init_uart_irq = init_cfg->uart_irq;
   uart_ctxt->init_rx_irq   = init_cfg->rx_irq;

   if ( !get_properties(uart_ctxt)                 ) { goto error; }
   if ( !map_hardware(uart_ctxt)                   ) { goto error; }
   if ( !create_objects(uart_ctxt, init_cfg)       ) { goto error; }

   uart_hal = &uart_ctxt->uart_hal;
   DALSYS_MemInfo(uart_ctxt->uart_mem_handle, &meminfo);
   HALuart_InitObject(uart_hal, uart_ctxt->uart_type, (void*)meminfo.VirtualAddr, (void*)meminfo.PhysicalAddr);

   if ( uart_ctxt->use_dma )
   {
      HALuart_GetFIFOPhysAddrs(uart_hal, &dma_config.rx_fifo, &dma_config.tx_fifo);

      dma_config.burst_size  = DMA_BURST_SIZE;
      dma_config.rx_callback = rx_dma_callback;
      dma_config.tx_callback = tx_dma_callback;
      dma_config.cb_context  = uart_ctxt;
      dma_config.client_id   = uart_ctxt->dal_id;

      uart_ctxt->dma_ctxt = dma_init(&dma_config);
      if (uart_ctxt->dma_ctxt == NULL) { goto error; }
   }

   return(uart_ctxt);
error:
   mu_deinit(uart_ctxt);
   return(NULL);
}

/*==================================================================================================

FUNCTION: mu_open

DESCRIPTION:
   The DAL client must ensure that they never call this function (via DalUart_Open or
   DalUart_OpenEx) when the port is already open since we are not using a flag and critical
   section to control access.

==================================================================================================*/
static boolean mu_open(UART_CONTEXT *uart_ctxt, UartOpenConfig *open_cfg)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   boolean xfer_result;

   if ( !valid_char_format(&open_cfg->char_format) || !valid_rts_control(open_cfg->rts_control) )
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "mu_open: invalid parameter");
      return FALSE;
   }

   uart_ctxt->rts_control = open_cfg->rts_control;
   uart_ctxt->break_notified = FALSE;
   uart_ctxt->rx_overruns = 0;
   uart_ctxt->rx_cached_status = 0;
   uart_ctxt->rx_activated = FALSE;
   uart_ctxt->rx_halt = FALSE;
   uart_ctxt->tx_halt = FALSE;
   uart_ctxt->tx_wait = FALSE;
   uart_ctxt->break_halt = FALSE;

   // The UART clocks must be enabled before we do anything that accesses UART/GSBI registers.
   // Also, there is a requirement for the BLSP clock to be enabled before we call any DMA
   // APIs for BAM based UARTs.  Therefore, clock_enable() must precede dma_open().

   if ( !allocate_buffers(uart_ctxt)      ) { goto error; }
   if ( !open_devices(uart_ctxt)          ) { goto error; }
   if ( !configure_interrupts(uart_ctxt)  ) { goto error; }
   if ( !clock_enable(uart_ctxt)          ) { goto error; }

   if (uart_ctxt->use_dma)
   {
      if ( !dma_open(uart_ctxt->dma_ctxt) ) { goto error; }
   }

   configure_gsbi(uart_ctxt);

   if ( !gpio_enable(uart_ctxt)           ) { goto error; }
   if ( !reg_init(uart_ctxt, open_cfg)    ) { goto error; }
   if ( !register_isr_for_uart(uart_ctxt) ) { goto error; }

   uart_ctxt->power_state = POWER_FULL;

   xfer_result = rx_transfer_start(uart_ctxt);

   if (xfer_result == FALSE)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "mu_open: failed to start RX transfer");
      goto error;
   }

   set_rts_control(uart_ctxt, uart_ctxt->rts_control);
   start_break_detection(uart_ctxt);
   return TRUE;
error:
   mu_close(uart_ctxt);
   return FALSE;
}

/*==================================================================================================

FUNCTION: notify_events

DESCRIPTION:
   Notify the DAL client that one or more line events have occurred.  Note that this function
   executes the callback directly so make sure it is not called from inside a critical section
   or it could cause a deadlock due to interaction with the client's critical sections.

==================================================================================================*/
static void notify_events(UART_CONTEXT *uart_ctxt, uint32 events)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+notify_events");

   if (uart_ctxt->line_event)
   {
      if ( DALSYS_EventCtrlEx(uart_ctxt->line_event, DALSYS_EVENT_CTRL_TRIGGER,
                              events, NULL, 0) != DAL_SUCCESS )
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "notify_events: failed");
      }
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-notify_events");
}

/*==================================================================================================

FUNCTION: notify_rxdata_available

DESCRIPTION:
   Notify the DAL client that RX data is available to be read.  Note that this function
   executes the callback directly so make sure it is not called from inside a critical section
   or it could cause a deadlock due to interaction with the client's critical sections.

==================================================================================================*/
static void notify_rxdata_available(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+notify_rxdata_available");

   if (uart_ctxt->rx_event)
   {
      if ( DALSYS_EventCtrl(uart_ctxt->rx_event,
                            DALSYS_EVENT_CTRL_TRIGGER) != DAL_SUCCESS )
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "notify_rxdata_available: failed");
      }
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-notify_rxdata_available");
}

/*==================================================================================================

FUNCTION: notify_transmit_complete

DESCRIPTION:
   Notify the DAL client that the TX transaction has completed.  Note that this function
   executes the callback directly so make sure it is not called from inside a critical section.
   or it could cause a deadlock due to interaction with the client's critical sections.

==================================================================================================*/
static void notify_transmit_complete(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+notify_transmit_complete");

   if (uart_ctxt->tx_event)
   {
      if ( DALSYS_EventCtrl(uart_ctxt->tx_event,
                            DALSYS_EVENT_CTRL_TRIGGER) != DAL_SUCCESS )
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "notify_transmit_complete: failed");
      }
   }

   notify_events(uart_ctxt, EVENT_TXEMPTY);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-notify_transmit_complete");
}

/*==================================================================================================

FUNCTION: open_devices

DESCRIPTION:

==================================================================================================*/
static boolean open_devices(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   TAL_RESULT result;

   result = tal_interrupt_open(&uart_ctxt->interrupt_handle, uart_ctxt->dal_id);
   if (result != TAL_SUCCESS) { goto error; }

   result = tal_tlmm_open(&uart_ctxt->tlmm_handle, uart_ctxt->dal_id);
   if (result != TAL_SUCCESS) { goto error; }

   result = tal_clock_open(&uart_ctxt->clock_handle, uart_ctxt->dal_id);
   if (result != TAL_SUCCESS) { goto error; }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "open_devices: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: power_025

DESCRIPTION:

==================================================================================================*/
static DALResult power_025(UART_CONTEXT *uart_ctxt)
{
   DALResult result = DAL_SUCCESS;

   switch (uart_ctxt->power_state)  // check current state
   {
      case POWER_OFF:  // POWER_OFF -> POWER_025
         halt_line_detection(uart_ctxt, FALSE);
         if ( !register_isr_for_rx(uart_ctxt, FALLING_EDGE) ) { result = DAL_ERROR; break; }
         if ( !clock_enable(uart_ctxt) )                      { result = DAL_ERROR; break; }
         set_rts_control(uart_ctxt, RTS_ASSERT);
         clock_disable(uart_ctxt);
         break;
      case POWER_025:  // POWER_025 -> POWER_025
         break;
      case POWER_FULL:  // POWER_FULL -> POWER_025
         if ( !prepare_for_sleep(uart_ctxt) )                 { result = DAL_ERROR; break; }
         halt_line_detection(uart_ctxt, FALSE);
         if ( !register_isr_for_rx(uart_ctxt, FALLING_EDGE) ) { result = DAL_ERROR; break; }
         set_rts_control(uart_ctxt, RTS_ASSERT);
         clock_disable(uart_ctxt);
         break;
      default:
         break;
   }

   if (result == DAL_SUCCESS) { uart_ctxt->power_state = POWER_025; }
   return(result);
}

/*==================================================================================================

FUNCTION: power_off

DESCRIPTION:

==================================================================================================*/
static DALResult power_off(UART_CONTEXT *uart_ctxt)
{
   DALResult result = DAL_SUCCESS;

   switch (uart_ctxt->power_state)  // check current state
   {
      case POWER_OFF:  // POWER_OFF -> POWER_OFF
         break;
      case POWER_025:  // POWER_025 -> POWER_OFF
         halt_line_detection(uart_ctxt, FALSE);
         if ( !clock_enable(uart_ctxt) )      { result = DAL_ERROR; break; }
         set_rts_control(uart_ctxt, RTS_DEASSERT);
         clock_disable(uart_ctxt);
         break;
      case POWER_FULL:  // POWER_FULL -> POWER_OFF
         if ( !prepare_for_sleep(uart_ctxt) ) { result = DAL_ERROR; break; }
         halt_line_detection(uart_ctxt, TRUE);
         clock_disable(uart_ctxt);
         break;
      default:
         break;
   }

   if (result == DAL_SUCCESS) { uart_ctxt->power_state = POWER_OFF; }
   return(result);
}

/*==================================================================================================

FUNCTION: power_on

DESCRIPTION:

==================================================================================================*/
static DALResult power_on(UART_CONTEXT *uart_ctxt)
{
   DALResult result = DAL_SUCCESS;

   switch (uart_ctxt->power_state)  // check current state
   {
      case POWER_OFF:  // POWER_OFF -> POWER_FULL
         if ( !clock_enable(uart_ctxt) )      { result = DAL_ERROR; break; }
         if ( !resume_from_sleep(uart_ctxt) ) { result = DAL_ERROR; break; }
         break;
      case POWER_025:  // POWER_025 -> POWER_FULL
         halt_line_detection(uart_ctxt, FALSE);
         if ( !clock_enable(uart_ctxt) )      { result = DAL_ERROR; break; }
         set_rts_control(uart_ctxt, RTS_DEASSERT);
         if ( !resume_from_sleep(uart_ctxt) ) { result = DAL_ERROR; break; }
         break;
      case POWER_FULL:  // POWER_FULL -> POWER_FULL
         break;
      default:
         break;
   }

   if (result == DAL_SUCCESS) { uart_ctxt->power_state = POWER_FULL; }
   return(result);
}

/*==================================================================================================

FUNCTION: prepare_for_sleep

DESCRIPTION:

==================================================================================================*/
static boolean prepare_for_sleep(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 i;
   uint32 tx_complete;
   boolean tx_quiescent = FALSE;
   uint32 chars_to_wait;
   uint32 sleep_duration;

   // This function prepares the UART for sleep.  We must stop RX and TX activity
   // so that we don't get any data corruption when the UART clock is disabled.
   //
   // RX path: We will deassert RTS to stop incoming data.  However, it is possible
   //          that we are clocking in a character right now and the UART HW provides
   //          no way to query this.  We need to wait one character time to account
   //          for this.  We also need to wait for the stale timeout to expire to
   //          allow processing of any RX data that started before we deasserted RTS.
   //
   // TX path: We will (1) instruct our clients to avoid submitting a sleep request
   //          when a TX transfer is in progress (2) wait up to 500ms for an
   //          in-progress transfer to complete and (3) reject the sleep request if
   //          the transfer does not complete in that time.

   DALSYS_SyncEnter(uart_ctxt->tx_sync);
   uart_ctxt->tx_wait = TRUE;  // this will prevent any new TX transfers
   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   for (i = 0; i < 500; i++)  // wait up to 500ms for the TX path to quiesce
   {
      tx_complete = HALuart_GetStatus(uart_hal) & HAL_UART_STATUS_TX_EMPTY;
      if (!uart_ctxt->tx_in_progress && tx_complete) { tx_quiescent = TRUE; break; }
      fwk_sleep_ms(1);
   }

   if (!tx_quiescent)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "prepare_for_sleep: timeout waiting for TX idle");
      uart_ctxt->tx_wait = FALSE;
      return(FALSE);
   }

   set_rts_control(uart_ctxt, RTS_DEASSERT);  // stop incoming data

   chars_to_wait = STALE_DEFAULT + 1;  // add one for character currently clocking in
   sleep_duration = (chars_to_wait * 10 * 1000 + uart_ctxt->baud_rate - 1) / uart_ctxt->baud_rate;
   fwk_sleep_ms(sleep_duration);

   // During halt_rx_path() we will be performing a purge operation which will discard
   // any data from the flush call.  But since we waited for a stale interrupt there
   // should be no data.

   halt_rx_path(uart_ctxt);
   if (uart_ctxt->use_dma) { dma_close(uart_ctxt->dma_ctxt); }
   return(TRUE);
}

/*==================================================================================================

FUNCTION: read_rx_fifo

DESCRIPTION:

==================================================================================================*/
static void read_rx_fifo(UART_CONTEXT *uart_ctxt, uint32 num_bytes)
{
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   MSM_UART_RX_BUF *rx_head;
   DALSYSMemInfo rx_data_info;
   uint8 *write_ptr;
   uint32 bytes_copied;

   rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
   DALSYS_MemInfo(rx_head->data_handle, &rx_data_info);
   write_ptr = (uint8 *)rx_data_info.VirtualAddr + rx_head->write_offset;

   // The HALuart_ReadRxFifo() API is not atomic because it reads the SR and then reads
   // the RF (and repeats until the RXFIFO is empty).  However, we don't need a register
   // critical section because no two threads will ever enter this API at the same time.
   // It is only called from this function in the context of the IST.

   bytes_copied = HALuart_ReadRxFifo(uart_hal, write_ptr, num_bytes);
   rx_head->write_offset += bytes_copied;
}

/*==================================================================================================

FUNCTION: receive_chunk

DESCRIPTION:
   An RX transaction is broken up into "chunks" based on the size of the RX FIFO.
   We will start each chunk by enabling the RX WATERMARK interrupt.  When the
   interrupt fires we will remove RX WATERMARK bytes from the RX FIFO and then
   enable another RX WATERMARK interrupt.  This process continues until the
   RX transaction completes (due to a stale event or the receive buffer fills).

==================================================================================================*/
static void receive_chunk(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   MSM_UART_RX_BUF *rx_head;
   uint32 buffer_room;
   uint32 fifo_room;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+receive_chunk");

   // Note that the watermark must be a multiple of 4 bytes so all values
   // it is derived from must be multiples of 4.

   rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
   buffer_room = RX_BUF_SIZE - rx_head->write_offset;

   fifo_room = uart_ctxt->rx_fifo_size - RXLEV_MARGIN;
   uart_ctxt->rx_watermark = (fifo_room <= buffer_room) ? (fifo_room) : (buffer_room);

   HALuart_SetRxWatermark(uart_hal, uart_ctxt->rx_watermark);

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   HALuart_EnableInterrupts(uart_hal, HAL_UART_RXLEV_IMR_FLAG);
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-receive_chunk");
}

/*==================================================================================================

FUNCTION: reg_init

DESCRIPTION:
   This function initializes the UART registers.

==================================================================================================*/
static boolean reg_init(UART_CONTEXT *uart_ctxt, UartOpenConfig *open_cfg)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 total_fifo_size;

   // Should we deassert RTS here?  And do GPIO configuration after instead of before?
   // This would ensure the remote UART is flowed off until we are ready.

   if ( !set_baud_rate(uart_ctxt, open_cfg->baud_rate) ) { return FALSE; }

   set_char_format(uart_ctxt, &open_cfg->char_format);

   // Not all of these register accesses need critical section protection but many
   // of them do.  For simplicity we will put the whole block in a critical section.

   DALSYS_SyncEnter(uart_ctxt->register_sync);

   // Examples of FIFO size (bytes):  8250 legacy UART = 512/512 (RX/TX)
   //                                 8250 UARTDM      =     512 (total)
   //                                 8660 UARTDM      =     512 (total)
   //                                 6290 UARTDM      =    4096 (total)
   //                                 8974 UARTBAM     =     512 (total)

   if (uart_ctxt->use_dma)
   {
      HALuart_SetDmaBurst(uart_hal, DMA_BURST_SIZE);
      HALuart_SetTxFifoSize(uart_hal, DMA_BURST_SIZE * 2);
      HALuart_SetRxDmaEnable(uart_hal, TRUE);
      HALuart_SetTxDmaEnable(uart_hal, TRUE);
   }
   else
   {
      HALuart_SetTxWatermark(uart_hal, TXLEV_MARGIN);

      total_fifo_size = HALuart_GetRxFifoSize(uart_hal) +
                        HALuart_GetTxFifoSize(uart_hal);

      HALuart_SetTxFifoSize(uart_hal, total_fifo_size / 2);
      HALuart_SetRxDmaEnable(uart_hal, FALSE);
      HALuart_SetTxDmaEnable(uart_hal, FALSE);
   }

   uart_ctxt->rx_fifo_size = HALuart_GetRxFifoSize(uart_hal);
   uart_ctxt->tx_fifo_size = HALuart_GetTxFifoSize(uart_hal);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "RX FIFO size = %d", uart_ctxt->rx_fifo_size);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "TX FIFO size = %d", uart_ctxt->tx_fifo_size);

   HALuart_SetAutoRfrLevel      (uart_hal, uart_ctxt->rx_fifo_size - AUTORFR_MARGIN);
   HALuart_SetOutboundFlowCtl   (uart_hal, (open_cfg->cts_control == CTS_ENABLE));
   HALuart_LoopbackSet          (uart_hal, FALSE);
   HALuart_SetNullCharOnBreak   (uart_hal, FALSE);
   HALuart_SetCmdSynchronization(uart_hal, TRUE);
   HALuart_DisableInterrupts    (uart_hal, 0xFFFFFFFF);
   HALuart_SetStaleTimeout      (uart_hal, STALE_DEFAULT);
   HALuart_SetIrdaEnable        (uart_hal, FALSE);
   HALuart_EnableHwFixes        (uart_hal, TRUE);
   HALuart_SetAutoReactivateRx  (uart_hal, BAM(uart_ctxt) && uart_ctxt->use_dma);

   if (uart_ctxt->prop_is_cxm)
   {
      // If this hardware core supports coexistence, bypass the CXM hardware,
      // so that the UART can be used as a standard UART.
      HALuart_SetCxmEnable(uart_hal, HAL_UART_CXM_BYPASS, 0);
   }

   HALuart_SetRxEnable          (uart_hal, FALSE);
   HALuart_SetTxEnable          (uart_hal, FALSE);
   HALuart_ResetReceiver        (uart_hal);
   HALuart_ResetTransmitter     (uart_hal);
   HALuart_ResetErrorStatus     (uart_hal);
   HALuart_SetRxEnable          (uart_hal, TRUE);
   HALuart_SetTxEnable          (uart_hal, TRUE);

   DALSYS_SyncLeave(uart_ctxt->register_sync);

   return TRUE;
}

/*==================================================================================================

FUNCTION: register_isr_for_rx

DESCRIPTION:

==================================================================================================*/
static boolean register_isr_for_rx(UART_CONTEXT *uart_ctxt, RX_TRIGGER_TYPE rx_trigger)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   TAL_INTERRUPT_TRIGGER tal_trigger;
   TAL_RESULT result;

   tal_trigger = (rx_trigger == FALLING_EDGE) ?
                 (TAL_INTERRUPT_TRIGGER_FALLING) : (TAL_INTERRUPT_TRIGGER_RISING);

   // We expect the registration function to configure the interrupt type and polarity,
   // clear the interrupt, and enable it.

   result = tal_interrupt_register(uart_ctxt->interrupt_handle,
                                   &uart_ctxt->rx_interrupt, tal_trigger,
                                   rx_isr, rx_dpc, uart_ctxt);

   if (result == TAL_SUCCESS)
   {
      uart_ctxt->rx_trigger = rx_trigger;
      uart_ctxt->rx_isr_registered = TRUE;
      return(TRUE);
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "register_isr_for_rx: failed");
      return(FALSE);
   }
}

/*==================================================================================================

FUNCTION: register_isr_for_uart

DESCRIPTION:

==================================================================================================*/
static boolean register_isr_for_uart(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   TAL_RESULT result;

   result = tal_interrupt_register(uart_ctxt->interrupt_handle,
                                   &uart_ctxt->uart_interrupt, TAL_INTERRUPT_TRIGGER_HIGH,
                                   uart_isr, uart_dpc, uart_ctxt);

   if (result == TAL_SUCCESS)
   {
      uart_ctxt->uart_isr_registered = TRUE;
      return(TRUE);
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "register_isr_for_uart: failed");
      return(FALSE);
   }
}

/*==================================================================================================

FUNCTION: resume_from_sleep

DESCRIPTION:

==================================================================================================*/
static boolean resume_from_sleep(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   boolean restart_needed;

   if (uart_ctxt->use_dma)
   {
      if ( !dma_open(uart_ctxt->dma_ctxt) )
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "resume_from_sleep: dma_open() failed");
         return(FALSE);
      }
   }

   if (uart_ctxt->prop_is_cxm)
   {
      // If this hardware core supports coexistence, reset the type-0 status
      // (messages received by hardware from the remote end) inside the
      // coexistence hardware when resuming from sleep.
      //
      // Write 1 to clear WLAN_STICKY and WCN_PRIORITY, then write 0 after a
      // short delay to restore the proper behavior of the WLAN_STICKY and
      // WCN_PRIORITY signals.
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetType0WakeupClear(uart_hal, 1);
      DALSYS_SyncLeave(uart_ctxt->register_sync);

      fwk_sleep_ms(1);

      DALSYS_SyncEnter(uart_ctxt->register_sync);
      HALuart_SetType0WakeupClear(uart_hal, 0);
      DALSYS_SyncLeave(uart_ctxt->register_sync);
   }

   // Resume the RX path.

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   HALuart_SetRxEnable(uart_hal, TRUE);
   uart_ctxt->rx_halt = FALSE;  // set this now so we can restart the RX transfer below
   rx_transfer_start(uart_ctxt);

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   set_rts_control(uart_ctxt, uart_ctxt->rts_control);  // restore user setting

   // Resume the TX path.

   uart_ctxt->tx_wait = FALSE;

   // Resume break detection.  We need to detect a special case where break detection
   // is already active.  This will be true if the break condition was held continuously
   // throughout a POWER_FULL -> POWER_OFF -> POWER_FULL transition.  In this case we
   // must not attempt to restart break detection.

   tal_interrupt_dpc_disable(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);

   uart_ctxt->break_halt = FALSE;

   if (uart_ctxt->rx_trigger == RISING_EDGE) { restart_needed = FALSE; }
   else                                      { restart_needed = TRUE;  }

   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
   tal_interrupt_dpc_enable(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);

   if (restart_needed) { start_break_detection(uart_ctxt); }

   return(TRUE);
}

/*==================================================================================================

FUNCTION: rx_dma_callback

DESCRIPTION:
   This function is called when an RX DMA transfer completes.  The call originates from
   the DMA driver and it executes in an unknown context.  It might be an interrupt service
   thread (typical on NHLOS) or a DPC (typical on Windows).  We also don't know if any locks
   are currently held that might might cause a deadlock if we call back into the DMA driver
   from here.  Therefore we will defer processing of this event and handle it in the context
   of our own DPC.

==================================================================================================*/
static void rx_dma_callback(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+rx_dma_callback");

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   uart_ctxt->uart_events |= DMA_RX_COMPLETE;
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   tal_interrupt_dpc_schedule(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-rx_dma_callback");
}

/*==================================================================================================

FUNCTION: rx_dpc

DESCRIPTION:
   This is the RX deferred procedure call.  It is used to defer processing of RX edge
   detection until we are in a context where we are permitted to invoke client callbacks.

   We can assume that this function will not be called more than once concurrently.
   However, we must acquire the interrupt sync object if we need mutual exclusion from
   the ISR.

==================================================================================================*/
static void *rx_dpc(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   DALDEVICEID id = uart_ctxt->dal_id;
   uint32 rx_events;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+rx_dpc");

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
   rx_events = uart_ctxt->rx_events;
   uart_ctxt->rx_events = 0;
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);

   if (rx_events & IRQ_RX_FALLING) { service_rx_falling(uart_ctxt); }
   if (rx_events & IRQ_RX_RISING)  { service_rx_rising (uart_ctxt); }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-rx_dpc");
   return NULL;
}

/*==================================================================================================

FUNCTION: rx_isr

DESCRIPTION:
   This is the RX GPIO interrupt service routine.  It is called when the GPIO interrupt
   controller detects a falling edge or rising edge on the RX line (depending on how it
   was previously configured).

   We will assume that we have been called in interrupt context and the latched edge
   has not been cleared in HW.  Therefore we must quiesce the IRQ here and defer any
   client callbacks to the DPC.

   The interrupt sync object has already been acquired (there is no need to acquire it here).

==================================================================================================*/
static void *rx_isr(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+rx_isr");

   if (uart_ctxt->rx_trigger != NO_TRIGGER)
   {
      if      (uart_ctxt->rx_trigger == FALLING_EDGE) { uart_ctxt->rx_events |= IRQ_RX_FALLING; }
      else if (uart_ctxt->rx_trigger == RISING_EDGE)  { uart_ctxt->rx_events |= IRQ_RX_RISING;  }

      // On non-HLOS, the scheduling of DPC needs to happen before deregistering because during
      // deregister the interrupt object at TAL layer is marked free and the scheduling will not
      // happen.

      tal_interrupt_dpc_schedule(uart_ctxt->interrupt_handle, &uart_ctxt->rx_interrupt);
      deregister_isr_for_rx(uart_ctxt);
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-rx_isr");
   return NULL;
}

/*==================================================================================================

FUNCTION: rx_transfer_complete

DESCRIPTION:

==================================================================================================*/
static void rx_transfer_complete(UART_CONTEXT *uart_ctxt, boolean *rx_data, boolean *rx_error)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   MSM_UART_RX_BUF *rx_head;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+rx_transfer_complete");

   // This function is always called from the DPC so we do not need to synchronize with
   // the DPC when touching uart_events.  Attempting to do so would cause a deadlock.

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   uart_ctxt->uart_events &= ~IRQ_RXSTALE;
   uart_ctxt->uart_events &= ~IRQ_RXLEV;
   HALuart_DisableInterrupts(uart_hal, HAL_UART_RXSTALE_IMR_FLAG);
   HALuart_DisableInterrupts(uart_hal, HAL_UART_RXLEV_IMR_FLAG);
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   *rx_error = check_rx_error(uart_ctxt);

   rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];

   if (rx_head->write_offset)
   {
      *rx_data = TRUE;

      // increment rx_head
      uart_ctxt->rx_head = (uart_ctxt->rx_head + 1) & (RX_BUF_CNT - 1);
      rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];

      // check if we are out of buffers
      if (rx_head->write_offset) { uart_ctxt->rx_out_of_buffers = TRUE; }
   }
   else
   {
      *rx_data = FALSE;
   }

   rx_transfer_start(uart_ctxt);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-rx_transfer_complete");
}

/*==================================================================================================

FUNCTION: rx_transfer_start

DESCRIPTION:

==================================================================================================*/
static boolean rx_transfer_start(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   MSM_UART_RX_BUF *rx_head;
   DALSYSMemInfo rx_data_info;
   boolean enable_stale_interrupt;
   boolean activate_rx;
   boolean ret = FALSE;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+rx_transfer_start");

   if (uart_ctxt->rx_halt)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "RX path is halted");
      goto exit;
   }

   if (uart_ctxt->rx_out_of_buffers)
   {
      // If we have run out of buffers just exit for now and we will be called
      // later when a buffer has been freed.

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "ran out of buffers");
      goto exit;
   }

   if (uart_ctxt->use_dma)
   {
      // Initialize rx_xfer_size at the start of every transfer.  It will be used
      // to determine the number of bytes in the DMA buffer when the transfer completes.
      // If a stale event occurs this variable will be updated while processing the
      // stale interrupt.

      if ( DMOV(uart_ctxt) ) { uart_ctxt->rx_xfer_size = RX_BUF_SIZE; }

      rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
      DALSYS_MemInfo(rx_head->data_handle, &rx_data_info);

      rx_head->descriptor.physical_address  = (void*)rx_data_info.PhysicalAddr;
      rx_head->descriptor.bytes_to_transfer = RX_BUF_SIZE;
      rx_head->descriptor.bytes_transferred = 0;
      rx_head->descriptor.error             = 0;
      rx_head->descriptor.transfer_complete = 0;

      if ( !dma_receive(uart_ctxt->dma_ctxt, &rx_head->descriptor) )
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "rx_transfer_start: dma transfer failed");
         goto exit;
      }
   }

   if ( BAM(uart_ctxt) && uart_ctxt->use_dma )
   {
      // BAM UARTs do not require stale interrupts in DMA mode.  The DMA engine keeps
      // track of the number of bytes transferred and the DMA transfer will complete
      // on its own during a stale event.

      // HW bug: Manual reactivation of RX transfers (via DMRX writes) is broken on
      // BAM UARTs in DMA mode.  In this mode of operation the UART will fail to end
      // an RX transfer during a DMRX event.  EOT will not be signaled and an RX stall
      // or invalid SNAP will result.  Therefore we must use automatic reactivation
      // which requires us to suppress DMRX writes between transfers.

      enable_stale_interrupt = FALSE;
      activate_rx = uart_ctxt->rx_activated ? FALSE : TRUE;
   }
   else
   {
      enable_stale_interrupt = TRUE;
      activate_rx = TRUE;
   }

   // We must enable stale interrupts before starting the transfer.  This ensures
   // that a stale event always gets processed before a DMA event.

   if (enable_stale_interrupt)
   {
      tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      HALuart_EnableInterrupts(uart_hal, HAL_UART_RXSTALE_IMR_FLAG);
      tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   }

   if (activate_rx)
   {
      // We must write DMRX (start a new rx transfer) and then CR (enable stale
      // event), in that order.  If we reverse this order it is possible to get
      // a stale interrupt without a corresponding update to the SNAP register.

      HALuart_SetupDmaRx(uart_hal, RX_BUF_SIZE);
      HALuart_SetStaleEvent(uart_hal, TRUE);
      if ( BAM(uart_ctxt) && uart_ctxt->use_dma ) { HALuart_StartRxSideband(uart_hal); }

      uart_ctxt->rx_activated = TRUE;
   }

   if (!uart_ctxt->use_dma) { receive_chunk(uart_ctxt); }

   ret = TRUE;
exit:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-rx_transfer_start");
   return(ret);
}

/*==================================================================================================

FUNCTION: service_rx_break_start

DESCRIPTION:
   This function is called when the UART detects a break start (RX logic low for 10 bits).

==================================================================================================*/
static void service_rx_break_start(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_break_start");

   notify_events(uart_ctxt, EVENT_BREAK);
   uart_ctxt->break_notified = TRUE;
   start_break_detection(uart_ctxt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_break_start");
}

/*==================================================================================================

FUNCTION: service_rx_dma

DESCRIPTION:
   This function handles an RX DMA event.

==================================================================================================*/
static void service_rx_dma(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   DMA_STATE interrupt_type;
   MSM_UART_RX_BUF *rx_head;
   boolean dma_error;
   uint32 bytes_transferred;
   boolean rx_data;
   boolean rx_error;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_dma");

   interrupt_type = dma_rx_intr_type(uart_ctxt->dma_ctxt);
   dma_rx_intr_done(uart_ctxt->dma_ctxt);

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
   dma_error = rx_head->descriptor.error;
   bytes_transferred = DMOV(uart_ctxt) ? uart_ctxt->rx_xfer_size :
                                         rx_head->descriptor.bytes_transferred;

   if ( !dma_error && interrupt_type != PURGE )
   {
      rx_head->write_offset = bytes_transferred;
   }

   rx_transfer_complete(uart_ctxt, &rx_data, &rx_error);

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   if (rx_error) { notify_events(uart_ctxt, EVENT_ERROR); }  // callback outside critsec
   if (rx_data)  { notify_rxdata_available(uart_ctxt);    }  // callback outside critsec

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_dma");
}

/*==================================================================================================

FUNCTION: service_rx_falling

DESCRIPTION:
   This function is called when a falling edge is detected on the RX line.  This is
   used for wakeup purposes when in the POWER_025 state.

==================================================================================================*/
static void service_rx_falling(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_falling");

   notify_events(uart_ctxt, EVENT_RX_WAKEUP);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_falling");
}

/*==================================================================================================

FUNCTION: service_rx_rising

DESCRIPTION:
   This function is called when a rising edge is detected on the RX line.  This is
   used to detect the end of an RX break condition.

==================================================================================================*/
static void service_rx_rising(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_rising");

   notify_events(uart_ctxt, EVENT_BREAK_END);
   uart_ctxt->break_notified = FALSE;
   start_break_detection(uart_ctxt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_rising");
}

/*==================================================================================================

FUNCTION: service_rx_stale

DESCRIPTION:

==================================================================================================*/
static void service_rx_stale(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   MSM_UART_RX_BUF *rx_head;
   uint32 snapshot = 0;  // snapshot of the incoming byte counter when the rx transaction ended
   boolean rx_data = FALSE;
   boolean rx_error = FALSE;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_stale");

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   snapshot = HALuart_GetDmaRxSnapChars(uart_hal);
   HALuart_ResetStaleInterrupt(uart_hal);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "SNAP = 0x%08X", snapshot);

   if ( uart_ctxt->use_dma )
   {
      // DMOV UARTs need a DMA flush operation during stale event processing.  For
      // BAM UARTs the DMA transfer will complete on its own.  We do not enable stale
      // interrupts for BAM UARTs in DMA mode so if we are here it must be a DMOV UART.

      uart_ctxt->rx_xfer_size = snapshot;
      dma_rx_flush(uart_ctxt->dma_ctxt);
   }
   else
   {
      rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
      read_rx_fifo(uart_ctxt, snapshot - rx_head->write_offset);
      rx_transfer_complete(uart_ctxt, &rx_data, &rx_error);
   }

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   if (rx_error) { notify_events(uart_ctxt, EVENT_ERROR); }  // callback outside critsec
   if (rx_data)  { notify_rxdata_available(uart_ctxt);    }  // callback outside critsec

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_stale");
}

/*==================================================================================================

FUNCTION: service_rx_watermark

DESCRIPTION:
   Handles RXLEV interrupt (used only in non-DMA mode).

==================================================================================================*/
static void service_rx_watermark(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   MSM_UART_RX_BUF *rx_head;
   uint32 buffer_room;
   uint32 watermark;
   uint32 bytes_to_copy;
   boolean rx_data = FALSE;
   boolean rx_error = FALSE;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_rx_watermark");

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   rx_head = &uart_ctxt->rx_buf[uart_ctxt->rx_head];
   buffer_room = RX_BUF_SIZE - rx_head->write_offset;
   watermark = uart_ctxt->rx_watermark;

      // HW bug: If the packing buffer and the RXFIFO are both empty when a stale
      // event occurs then the SNAP register will be updated but the RXSTALE bit
      // (in ISR) will not be set and future stale events will not be detected.
      // To avoid this scenario we must not empty the RXFIFO unless we are at the
      // end of an RX transfer.  We subtract 4 because each FIFO slot is 4 bytes
      // for UARTDM.

      bytes_to_copy = (buffer_room == watermark) ? (watermark) : (watermark - 4);

   read_rx_fifo(uart_ctxt, bytes_to_copy);
   buffer_room = RX_BUF_SIZE - rx_head->write_offset;

   if (buffer_room) { receive_chunk(uart_ctxt); }
   else             { rx_transfer_complete(uart_ctxt, &rx_data, &rx_error); }

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   if (rx_error) { notify_events(uart_ctxt, EVENT_ERROR); }  // callback outside critsec
   if (rx_data)  { notify_rxdata_available(uart_ctxt);    }  // callback outside critsec

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_rx_watermark");
}

/*==================================================================================================

FUNCTION: service_tx_dma

DESCRIPTION:
   This function is to handle a TX DMA event.

==================================================================================================*/
static void service_tx_dma(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_tx_dma");

   dma_tx_intr_done(uart_ctxt->dma_ctxt);

   DALSYS_SyncEnter(uart_ctxt->tx_sync);

   if (uart_ctxt->tx_descriptor.error)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "service_tx_dma: error detected");
   }

   // Set TX_READY to interrupt us when the FIFO is empty
   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   HALuart_EnableInterrupts(uart_hal, HAL_UART_TX_READY_IMR_FLAG);
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_tx_dma");
}

/*==================================================================================================

FUNCTION: service_tx_ready

DESCRIPTION:
   Handles TX_READY interrupt.

==================================================================================================*/
static void service_tx_ready(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_tx_ready");

   // All data has been written to the TXFIFO and the TXFIFO has drained.  However,
   // the transaction may not have completed over the wire yet because there may
   // still be data in the unpacking register and/or shift register.

   DALSYS_SyncEnter(uart_ctxt->tx_sync);
   HALuart_ResetTxReadyInterrupt(uart_hal);
   uart_ctxt->tx_in_progress = FALSE;
   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   notify_transmit_complete(uart_ctxt);  // callback outside critsec

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_tx_ready");
}

/*==================================================================================================

FUNCTION: service_tx_watermark

DESCRIPTION:
   Handles TXLEV interrupt.

==================================================================================================*/
static void service_tx_watermark(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+service_tx_watermark");

   // WARNING: If transmit_chunk() fails here then the transaction will stall and
   // the only way to recover is for the client to close and reopen the port.

   DALSYS_SyncEnter(uart_ctxt->tx_sync);
   transmit_chunk(uart_ctxt);
   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-service_tx_watermark");
}

/*==================================================================================================

FUNCTION: set_baud_rate

DESCRIPTION:
   Sets the HW baud rate.  The UART must be open before calling this function.  Be aware
   that, due to the CLKREGIM call to set the input rate, the input clock will be halted
   briefly.  Therefore, do not call this function during a transaction or it will be
   corrupted.

==================================================================================================*/
static boolean set_baud_rate(UART_CONTEXT *uart_ctxt, uint32 baud_rate)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   const CLOCK_SETTINGS *entry;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "set_baud_rate: %d", baud_rate);

   for (entry = baud_table; entry->baud_rate; entry++)
   {
      if ( entry->baud_rate == baud_rate ) { break; }
   }

   if (entry->baud_rate == 0) { goto error; }  // baud rate not found

   // It is the DAL layer's responsibility to request the correct UART input frequency
   // from CLKRGM.  The HAL layer will modify the clock divider internal to the UART.

   if ( !set_input_freq(uart_ctxt, entry->input_freq) ) { goto error; }

   HALuart_SetDivider(uart_hal, entry->divider);
   uart_ctxt->baud_rate = baud_rate;
   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "set_baud_rate: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: set_char_format

DESCRIPTION:
   This function configures the character format fields in the MR2 register.

==================================================================================================*/
static void set_char_format(UART_CONTEXT *uart_ctxt, UartCharFormat *char_format)
{
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_SyncEnter(uart_ctxt->register_sync);

   HALuart_SetCharFormat(uart_hal, (HALUartBitsPerCharType)char_format->BitsPerChar,
                                   (HALUartNumStopBitsType)char_format->NumStopBits,
                                   (HALUartParityModeType)char_format->ParityMode);

   DALSYS_SyncLeave(uart_ctxt->register_sync);
}

/*==================================================================================================

FUNCTION: set_input_freq

DESCRIPTION:
   Set the clock input frequency to the UART HW block.

==================================================================================================*/
static boolean set_input_freq(UART_CONTEXT *uart_ctxt, uint32 input_freq)
{
   if(tal_clock_set_frequency(uart_ctxt->clock_handle, input_freq) == TAL_SUCCESS)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*==================================================================================================

FUNCTION: set_rts_control

DESCRIPTION:
   Receive flow control setting.

==================================================================================================*/
static void set_rts_control(UART_CONTEXT *uart_ctxt, UartRtsControlType rts_control)
{
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_SyncEnter(uart_ctxt->register_sync);

   switch (rts_control)
   {
      case RTS_DEASSERT:
         HALuart_SetRFR(uart_hal, FALSE);                 // deassert RTS
         HALuart_SetAutoInboundFlowCtl(uart_hal, FALSE);  // disable auto RTS control
         break;
      case RTS_ASSERT:
         HALuart_SetRFR(uart_hal, TRUE);                  // assert RTS
         HALuart_SetAutoInboundFlowCtl(uart_hal, FALSE);  // disable auto RTS control
         break;
      case RTS_AUTO:
         HALuart_SetAutoInboundFlowCtl(uart_hal, TRUE);   // enable auto RTS control
         break;
      default:
         break;
   }

   DALSYS_SyncLeave(uart_ctxt->register_sync);
}

/*==================================================================================================

FUNCTION: start_break_detection

DESCRIPTION:
   This function is called to activate break detection.  It must only be called when
   break detection is inactive.

==================================================================================================*/
static void start_break_detection(UART_CONTEXT *uart_ctxt)
{
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 break_asserted;  // current status of RX break (0 = deasserted, 1 = asserted)
   boolean missed_break_change;

   if ( !uart_ctxt->prop_detect_break ) { return; }  // feature not enabled
   if (  uart_ctxt->break_halt   ) { return; }  // auto-reactivation temporarily disabled

   // Check the break state and report missed break state changes.  Repeat until the
   // current break state matches the reported break state.

   do
   {
      break_asserted = HALuart_GetStatus(uart_hal) & HAL_UART_STATUS_RX_BREAK_START;
      missed_break_change = ( break_asserted && !uart_ctxt->break_notified) ||
                            (!break_asserted &&  uart_ctxt->break_notified);

      if (missed_break_change)
      {
         notify_events(uart_ctxt, break_asserted ? EVENT_BREAK : EVENT_BREAK_END);
         uart_ctxt->break_notified = break_asserted ? TRUE : FALSE;
      }

   } while (missed_break_change);


   // Enable the appropriate interrupt to detect the next break state change.

   if (break_asserted)
   {
      // Note that we cannot use the UART to detect break end because the client
      // may power down the UART in response to break start.  We must therefore use
      // an RX rising edge interrupt.

      register_isr_for_rx(uart_ctxt, RISING_EDGE);
   }
   else
   {
      HALuart_ResetRxBreakStartInterrupt(&uart_ctxt->uart_hal);

      tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      HALuart_EnableInterrupts(&uart_ctxt->uart_hal, HAL_UART_RXBREAK_START_IMR_FLAG);
      tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   }
}

/*==================================================================================================

FUNCTION: transmit_chunk

DESCRIPTION:
   A TX transaction is broken up into "chunks" based on how much data we can fit
   in the TX FIFO.  After one chunk finishes we will get a TX WATERMARK interrupt
   and we will transmit the next chunk.  This continues until all data has been
   transmitted.

==================================================================================================*/
static boolean transmit_chunk(UART_CONTEXT *uart_ctxt)
{
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 bytes_sent = 0;
   uint32 bytes_left = uart_ctxt->tx_bytes_left;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+transmit_chunk");

   DALSYS_SyncEnter(uart_ctxt->register_sync);
   bytes_sent = HALuart_WriteTxFifo(uart_hal, uart_ctxt->tx_ptr, bytes_left);
   DALSYS_SyncLeave(uart_ctxt->register_sync);

   if (bytes_sent)
   {
      if (bytes_sent == bytes_left)  // last chunk
      {
         tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
         HALuart_EnableInterrupts(uart_hal, HAL_UART_TX_READY_IMR_FLAG);
         tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      }
      else
      {
         tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
         HALuart_EnableInterrupts(uart_hal, HAL_UART_TXLEV_IMR_FLAG);
         tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
      }
   }

   uart_ctxt->tx_ptr += bytes_sent;
   uart_ctxt->tx_bytes_left -= bytes_sent;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-transmit_chunk");
   return (bytes_sent != 0);
}

/*==================================================================================================

FUNCTION: tx_dma_callback

DESCRIPTION:
   This function is called when a TX DMA transfer completes.  The call originates from
   the DMA driver and it executes in an unknown context.  It might be an interrupt service
   thread (typical on NHLOS) or a DPC (typical on Windows).  We also don't know if any locks
   are currently held that might might cause a deadlock if we call back into the DMA driver
   from here.  Therefore we will defer processing of this event and handle it in the context
   of our own DPC.

==================================================================================================*/
static void tx_dma_callback(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+tx_dma_callback");

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   uart_ctxt->uart_events |= DMA_TX_COMPLETE;
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   tal_interrupt_dpc_schedule(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-tx_dma_callback");
}

/*==================================================================================================

FUNCTION: uart_dpc

DESCRIPTION:
   This is the UART deferred procedure call.  It is used to defer processing of HW events
   (from the ISR) and SW events (from DMA callbacks).

   Depending on the OS we may be called in thread context (such as a DAL workloop) or in
   an elevated context that is lower than interrupt context (DISPATCH_LEVEL on Windows).
   We are permitted to invoke client callbacks from here.

   We can assume that this function will not be called more than once concurrently.
   However, we must acquire the interrupt sync object if we need mutual exclusion from
   the ISR.

==================================================================================================*/
static void *uart_dpc(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   DALDEVICEID id = uart_ctxt->dal_id;
   uint32 uart_events;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+uart_dpc");

   tal_interrupt_sync_enter(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   uart_events = uart_ctxt->uart_events;
   uart_ctxt->uart_events = 0;
   tal_interrupt_sync_exit(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);

   if ( (uart_events & IRQ_RXLEV) && (uart_events & IRQ_RXSTALE) )
   {
      // Simultaneous RXLEV/RXSTALE interrupts can only occur in PIO mode.
      // We can process both RXLEV and RXSTALE (in that order) or we can process
      // RXSTALE only.  The latter is more efficient because stale processing
      // completes the transfer.  We must NOT process RXSTALE and RXLEV (in that
      // order) because the RXLEV will no longer be relevant after completing
      // the transfer.

      uart_events &= ~IRQ_RXLEV;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "RXLEV/RXSTALE both set - ignoring RXLEV");
   }

   // IRQ_RXSTALE must be processed before DMA_RX_COMPLETE.  That is because there is a
   // corner condition when a stale event occurs after receiving between 1 and 15 bytes
   // less than the RX transfer size.  When that happens the UART HW will zero-pad the
   // FIFO and signal the final CRCI and the DMA transfer will complete on its own
   // before we flush it.  It is imperative that we handle RXSTALE first so we read the
   // SNAP register and determine the correct number of bytes in the transfer.

   if (uart_events & IRQ_RXSTALE)       { service_rx_stale      (uart_ctxt); }
   if (uart_events & IRQ_RXLEV)         { service_rx_watermark  (uart_ctxt); }
   if (uart_events & IRQ_TXLEV)         { service_tx_watermark  (uart_ctxt); }
   if (uart_events & IRQ_TX_READY)      { service_tx_ready      (uart_ctxt); }
   if (uart_events & IRQ_RXBREAK_START) { service_rx_break_start(uart_ctxt); }
   if (uart_events & DMA_RX_COMPLETE)   { service_rx_dma        (uart_ctxt); }
   if (uart_events & DMA_TX_COMPLETE)   { service_tx_dma        (uart_ctxt); }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-uart_dpc");
   return NULL;
}

/*==================================================================================================

FUNCTION: uart_isr

DESCRIPTION:
   This is the UART interrupt service routine.  It is called when the UART IRQ has been
   signaled by HW (this is a level-based active-high interrupt).

   Depending on the OS we may be called in thread context after the IRQ has been
   disabled at the QGIC level or in interrupt context while the IRQ is still enabled
   at the QGIC level.  We must assume the latter and must (at the minimum) quiesce
   the IRQ at the UART level.  Failure to do so would cause an interrupt storm on
   some architectures.

   The interrupt sync object has already been acquired (there is no need to acquire it here).

==================================================================================================*/
static void *uart_isr(void *context)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)context;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   DALDEVICEID id = uart_ctxt->dal_id;
   uint32 isr, misr, sr;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+uart_isr");

   isr  = HALuart_GetInterruptStatus(uart_hal);
   misr = HALuart_GetMaskedInterruptStatus(uart_hal);
   sr   = HALuart_GetStatus(uart_hal);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "isr=0x%08X, misr=0x%08X, sr=0x%04X", isr, misr, sr);

   if (misr)
   {
      if (misr & HAL_UART_TXLEV_IMR_FLAG)         { uart_ctxt->uart_events |= IRQ_TXLEV;         }
      if (misr & HAL_UART_RXSTALE_IMR_FLAG)       { uart_ctxt->uart_events |= IRQ_RXSTALE;       }
      if (misr & HAL_UART_RXLEV_IMR_FLAG)         { uart_ctxt->uart_events |= IRQ_RXLEV;         }
      if (misr & HAL_UART_TX_READY_IMR_FLAG)      { uart_ctxt->uart_events |= IRQ_TX_READY;      }
      if (misr & HAL_UART_RXBREAK_START_IMR_FLAG) { uart_ctxt->uart_events |= IRQ_RXBREAK_START; }

      HALuart_DisableInterrupts(uart_hal, misr);
      tal_interrupt_dpc_schedule(uart_ctxt->interrupt_handle, &uart_ctxt->uart_interrupt);
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-uart_isr");
   return NULL;
}

/*==================================================================================================

FUNCTION: unmap_hardware

DESCRIPTION:
   This function releases the virtual address space allocated for the HW blocks.

==================================================================================================*/
static void unmap_hardware(UART_CONTEXT *uart_ctxt)
{
   if (uart_ctxt->uart_mem_handle) { DALSYS_DestroyObject(uart_ctxt->uart_mem_handle); }
   if (uart_ctxt->gsbi_mem_handle) { DALSYS_DestroyObject(uart_ctxt->gsbi_mem_handle); }
}

/*==================================================================================================

FUNCTION: valid_char_format

DESCRIPTION:
   This function determines if the given character format settings are valid.

==================================================================================================*/
static boolean valid_char_format(UartCharFormat *char_format)
{
   switch (char_format->ParityMode)
   {
      case UART_NO_PARITY:
      case UART_ODD_PARITY:
      case UART_EVEN_PARITY:
      case UART_SPACE_PARITY:
         break;
      default:
         return FALSE;
   }

   switch (char_format->BitsPerChar)
   {
      case UART_5_BITS_PER_CHAR:
      case UART_6_BITS_PER_CHAR:
      case UART_7_BITS_PER_CHAR:
      case UART_8_BITS_PER_CHAR:
         break;
      default:
         return FALSE;
   }

   switch (char_format->NumStopBits)
   {
      case UART_0_5_STOP_BITS:
      case UART_1_0_STOP_BITS:
      case UART_1_5_STOP_BITS:
      case UART_2_0_STOP_BITS:
         break;
      default:
         return FALSE;
   }

   return TRUE;
}

/*==================================================================================================

FUNCTION: valid_rts_control

DESCRIPTION:
   This function determines if the given RTS control settings are valid.

==================================================================================================*/
static boolean valid_rts_control(UartRtsControlType rts_control)
{
   switch (rts_control)
   {
      case RTS_DEASSERT:
      case RTS_ASSERT:
      case RTS_AUTO:
         return TRUE;
      default:
         return FALSE;
   }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: Uart_ClearBreak

DESCRIPTION:

==================================================================================================*/
DALResult Uart_ClearBreak(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_ClearBreak(0x%08p)", h);

   HALuart_StopBreak(uart_hal);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_ClearBreak(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_Close

DESCRIPTION:

==================================================================================================*/
DALResult Uart_Close(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_Close(0x%08p)", h);

   mu_close(uart_ctxt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_Close(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_CxmTxDirectChar

DESCRIPTION:

==================================================================================================*/
DALResult Uart_CxmTxDirectChar   (uint32 ClientHandle, DalDeviceHandle *h, uint8 character)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   DALResult result = DAL_ERROR;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_CxmTxDirectChar(0x%08p, 0x%02X)",
       h, character);

   if (uart_ctxt->prop_is_cxm)
   {
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      if (HALuart_CxmTxDirectChar(uart_hal, character))
      {
         result = DAL_SUCCESS;
      }
      else
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_CxmTxDirectChar Failed");
      }
      DALSYS_SyncLeave(uart_ctxt->register_sync);
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_CxmTxDirectChar Failed Not Supported");
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_CxmTxDirectChar(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_DeviceDeInit

DESCRIPTION:

==================================================================================================*/
DALResult Uart_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   DalUartHandle *uart_handle = (DalUartHandle *)h;
   DALDEVICEID id = uart_handle->pClientCtxt->pDALDevCtxt->DevId;
   UART_CONTEXT *uart_ctxt = uart_handle->uart_ctxt;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_DeviceDeInit(0x%08p)", h);

   mu_deinit(uart_ctxt);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_DeviceDeInit(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_DeviceInit

DESCRIPTION:

==================================================================================================*/
DALResult Uart_DeviceInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   DalUartHandle *uart_handle = (DalUartHandle *)h;
   DALDEVICEID id = uart_handle->pClientCtxt->pDALDevCtxt->DevId;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_DeviceInit(0x%08p)", h);

   // defer initialization until Uart_PostInit()

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_DeviceInit(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_DumpRegs

DESCRIPTION:

==================================================================================================*/
DALResult Uart_DumpRegs(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_DumpRegs(0x%08p)", h);

   /* Read the GSBI register to double-check the protocol code */
   if (uart_ctxt->gsbi_mem_handle)
   {
      DALSYSMemInfo meminfo;
      volatile uint32 *gsbi;

      result = DALSYS_MemInfo(uart_ctxt->gsbi_mem_handle, &meminfo);
      if (result == DAL_SUCCESS)
      {
         gsbi = (volatile uint32 *)meminfo.VirtualAddr;
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "GSBIn_CTRL_REG=0x%08X", *gsbi);
      }
   }

   if (uart_ctxt->clock_on)
   {
      dump_regs(uart_ctxt);
   }
   else
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "UART clocks are off");
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_DumpRegs(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_GetCTS

DESCRIPTION:

==================================================================================================*/
DALResult Uart_GetCTS(uint32 ClientHandle, DalDeviceHandle *h, uint32 *cts_state)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_GetCTS(0x%08p, 0x%08p)", h, cts_state);

   // The HAL API returns the CTS voltage (0 = low, 1 = high).  Since CTS is active low we
   // must negate the voltage when reporting the state (0 = deasserted, 1 = asserted).

   *cts_state = !(HALuart_GetInterruptStatus(uart_hal) & HAL_UART_CURRENT_CTS_IMR_FLAG);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_GetCTS(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_GetCxmTxSticky

DESCRIPTION:

==================================================================================================*/
DALResult Uart_GetCxmTxSticky(uint32 ClientHandle, DalDeviceHandle *h, uint32 *tx_sticky, uint32 clear)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   DALResult result = DAL_ERROR;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_GetCxmTxSticky(0x%08p, 0x%08p, 0x%08X)",
       h, tx_sticky, clear);

   if (uart_ctxt->prop_is_cxm)
   {
      *tx_sticky = HALuart_GetCxmTxSticky(uart_hal, clear);
      result = DAL_SUCCESS;
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_GetCxmTxSticky(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_GetStatus

DESCRIPTION:

==================================================================================================*/
DALResult Uart_GetStatus(uint32 ClientHandle, DalDeviceHandle *h, uint32 *status)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 sr;  // UART status register

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_GetStatus(0x%08p, 0x%08p)", h, status);
   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   *status = 0;

   sr = HALuart_GetStatus(uart_hal);  // get immediate status

   if (sr & HAL_UART_STATUS_HUNT_CHAR)          { *status |= STATUS_RX_HUNT_CHAR_DETECTED; }
   if (sr & HAL_UART_STATUS_RX_BREAK)           { *status |= STATUS_RX_BREAK;              }
   if (sr & HAL_UART_STATUS_PARITY_FRAME_ERROR) { *status |= STATUS_RX_PARITY_ERROR;
                                                  *status |= STATUS_RX_FRAME_ERROR;        }
   if (sr & HAL_UART_STATUS_OVERRUN)            { *status |= STATUS_RX_OVERRUN_ERROR;      }
   if (sr & HAL_UART_STATUS_TX_EMPTY)           { *status |= STATUS_TX_EMPTY;              }
   if (sr & HAL_UART_STATUS_TX_READY)           { *status |= STATUS_TX_READY;              }
   if (sr & HAL_UART_STATUS_RX_FULL)            { *status |= STATUS_RX_FULL;               }
   if (sr & HAL_UART_STATUS_RX_READY)           { *status |= STATUS_RX_READY;              }

   *status |= uart_ctxt->rx_cached_status;  // get cached status (overrun/parity/framing)

   // The HAL API returns the CTS voltage (0 = low, 1 = high).  Since CTS is active low we
   // must negate the voltage when reporting the state (0 = deasserted, 1 = asserted).

   if ( !(HALuart_GetInterruptStatus(uart_hal) & HAL_UART_CURRENT_CTS_IMR_FLAG) )
   {
      *status |= STATUS_CTS_ASSERTED;
   }

   HALuart_ResetErrorStatus(uart_hal);  // clear rx hunt, break, and error bits
   uart_ctxt->rx_cached_status = 0;  // zero out the now stale cached status

   DALSYS_SyncLeave(uart_ctxt->rx_sync);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_GetStatus(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_Info

DESCRIPTION:

==================================================================================================*/
DALResult Uart_Info(uint32 ClientHandle, DalDeviceHandle *h, DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_Info(0x%08p, 0x%08p, 0x%08X)",
                   h, pdeviceInfo, dwSize);

   pdeviceInfo->Version = DALUART_INTERFACE_VERSION;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_Info(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_LoopbackSet

DESCRIPTION:

==================================================================================================*/
DALResult Uart_LoopbackSet(uint32 ClientHandle, DalDeviceHandle *h, uint32 enabled)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+Uart_LoopbackSet(0x%08p, 0x%08X)", h, enabled);

   DALSYS_SyncEnter(uart_ctxt->register_sync);
   HALuart_LoopbackSet(uart_hal, enabled);
   DALSYS_SyncLeave(uart_ctxt->register_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_LoopbackSet(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_Open

DESCRIPTION:

==================================================================================================*/
DALResult Uart_Open(uint32 ClientHandle, DalDeviceHandle *h, uint32 dwaccessMode)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   UartOpenConfig open_cfg;  // default settings
   DALResult result;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_Open(0x%08p, 0x%08X)", h, dwaccessMode);

   open_cfg.baud_rate               = 115200;
   open_cfg.char_format.ParityMode  = UART_NO_PARITY;
   open_cfg.char_format.BitsPerChar = UART_8_BITS_PER_CHAR;
   open_cfg.char_format.NumStopBits = UART_1_0_STOP_BITS;
   open_cfg.rts_control             = RTS_AUTO;
   open_cfg.cts_control             = CTS_ENABLE;

   result = mu_open(uart_ctxt, &open_cfg) ? (DAL_SUCCESS) : (DAL_ERROR);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_Open(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_OpenEx

DESCRIPTION:

==================================================================================================*/
DALResult Uart_OpenEx(uint32 ClientHandle, DalDeviceHandle *h, UartOpenConfig *open_cfg)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_OpenEx(0x%08p, 0x%08p)", h, open_cfg);

   result = mu_open(uart_ctxt, open_cfg) ? (DAL_SUCCESS) : (DAL_ERROR);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_OpenEx(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_PostInit

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PostInit(uint32 ClientHandle, DalDeviceHandle *h, UartInitConfig *init_cfg)
{
   DalUartHandle *uart_handle = (DalUartHandle *)h;
   DALDEVICEID id = uart_handle->pClientCtxt->pDALDevCtxt->DevId;
   DALResult result;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_PostInit(0x%08p, 0x%08p)", h, init_cfg);

   uart_handle->uart_ctxt = mu_init(init_cfg, id);
   result = (uart_handle->uart_ctxt) ? (DAL_SUCCESS) : (DAL_ERROR);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PostInit(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_PowerCapabilities

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PowerCapabilities(uint32 ClientHandle, DalDeviceHandle *h, uint32 *power_mask)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+Uart_PowerCapabilities(0x%08p, 0x%08p)", h, power_mask);

   *power_mask = POWER_FULL | POWER_025 | POWER_OFF;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PowerCapabilities(DAL_SUCCESS)");
   return(DAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: Uart_PowerEvent

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd, DalPowerDomain PowerDomain)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+Uart_PowerEvent(0x%08p, 0x%08X, 0x%08X)", h, PowerCmd, PowerDomain);

   // Values for DalPowerCmd apparently haven't been defined yet,
   // treat it as a UartPowerType for now.
   result = Uart_PowerSet(ClientHandle, h, (UartPowerType)PowerCmd);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PowerEvent(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_PowerGet

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PowerGet(uint32 ClientHandle, DalDeviceHandle *h, UartPowerType *power_state)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_PowerGet(0x%08p, 0x%08p)", h, power_state);

   *power_state = uart_ctxt->power_state;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PowerGet(DAL_SUCCESS)");
   return(DAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: Uart_PowerSet

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PowerSet(uint32 ClientHandle, DalDeviceHandle *h, UartPowerType power_state)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult ret;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_PowerSet(0x%08p, 0x%08X)", h, power_state);

   switch (power_state)  // requested power state
   {
      case POWER_OFF:  // used for out of band sleep
         ret = power_off(uart_ctxt);
         break;
      case POWER_025:  // used for inband sleep
         ret = power_025(uart_ctxt);
         break;
      case POWER_FULL:
         ret = power_on(uart_ctxt);
         break;
      case POWER_050:
      case POWER_075:
      default:
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_PowerSet: invalid power state");
         ret = DAL_ERROR;
         break;
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PowerSet(%d)", ret);
   return(ret);
}

/*==================================================================================================

FUNCTION: Uart_PurgeRx

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PurgeRx(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 i;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_PurgeRx(0x%08p)", h);

   // halt the RX path, purge the RX buffers, and restart the RX path

   halt_rx_path(uart_ctxt);

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   for (i = 0; i < RX_BUF_CNT; i++)
   {
      uart_ctxt->rx_buf[i].read_offset = 0;
      uart_ctxt->rx_buf[i].write_offset = 0;
   }

   uart_ctxt->rx_head = 0;
   uart_ctxt->rx_tail = 0;
   uart_ctxt->rx_out_of_buffers = FALSE;

   HALuart_SetRxEnable(uart_hal, TRUE);
   uart_ctxt->rx_halt = FALSE;
   rx_transfer_start(uart_ctxt);

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PurgeRx(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_PurgeTx

DESCRIPTION:

==================================================================================================*/
DALResult Uart_PurgeTx(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_PurgeTx(0x%08p)", h);

   halt_tx_path(uart_ctxt);
   HALuart_SetTxEnable(uart_hal, TRUE);  // is this necessary after TX reset?
   uart_ctxt->tx_halt = FALSE;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_PurgeTx(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_Read

DESCRIPTION:

==================================================================================================*/
DALResult Uart_Read(uint32 ClientHandle, DalDeviceHandle *h, uint8 *read_buffer, uint32 buffer_len,
                    uint32 *bytes_read, uint32 *overrun_events)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   MSM_UART_RX_BUF *rx_tail;
   DALSYSMemInfo rx_data_info;
   uint32 bytes_present;
   uint32 num_bytes;
   uint8 *src_ptr;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_Read(0x%08p, 0x%08p, 0x%08X, 0x%08p, 0x%08p)",
                   h, read_buffer, buffer_len, bytes_read, overrun_events);

   DALSYS_SyncEnter(uart_ctxt->rx_sync);

   *bytes_read = 0;
   *overrun_events = uart_ctxt->rx_overruns;  // report number of overrun events since last read
   uart_ctxt->rx_overruns = 0;

   // Loop through each RX buffer, copying whatever data is available, and
   // terminate the loop when out of data or out of room in the caller's buffer.
   for (;;)
   {
      rx_tail = &uart_ctxt->rx_buf[uart_ctxt->rx_tail];
      bytes_present = rx_tail->write_offset - rx_tail->read_offset;

      if (!bytes_present) { break; }  // no RX data available
      if (!buffer_len)    { break; }  // no room left in caller's buffer

      num_bytes = (bytes_present <= buffer_len) ? (bytes_present) : (buffer_len);

      DALSYS_MemInfo(rx_tail->data_handle, &rx_data_info);
      src_ptr = (uint8 *)rx_data_info.VirtualAddr + rx_tail->read_offset;

      memcpy(read_buffer, src_ptr, num_bytes);
      rx_tail->read_offset += num_bytes;
      read_buffer += num_bytes;
      buffer_len -= num_bytes;
      *bytes_read += num_bytes;

      if (rx_tail->read_offset == rx_tail->write_offset)  // tail buffer empty
      {
         if (uart_ctxt->rx_tail != uart_ctxt->rx_head || uart_ctxt->rx_out_of_buffers)
         {
            // reset tail buffer and increment rx_tail
            rx_tail->read_offset = rx_tail->write_offset = 0;
            uart_ctxt->rx_tail = (uart_ctxt->rx_tail + 1) & (RX_BUF_CNT - 1);

            if (uart_ctxt->rx_out_of_buffers)
            {
               uart_ctxt->rx_out_of_buffers = FALSE;
               rx_transfer_start(uart_ctxt);
            }
         }
      }
   }

   DALSYS_SyncLeave(uart_ctxt->rx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_Read(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_RxActive

DESCRIPTION:
   This function will read DMRX register to check if there is an active RX transfer.
==================================================================================================*/
DALResult Uart_RxActive(uint32 ClientHandle, DalDeviceHandle *h, uint32 *rx_active)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_RxActive(0x%08p, 0x%08p)", h, rx_active);

   *rx_active = (HALuart_GetDmaRx(uart_hal) != 0) ? 1 : 0;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_RxActive(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetBreak

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetBreak(uint32 ClientHandle, DalDeviceHandle *h)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_SetBreak(0x%08p)", h);

   HALuart_StartBreak(uart_hal);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetBreak(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetCharFormat

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetCharFormat(uint32 ClientHandle, DalDeviceHandle *h, UartCharFormat *char_format)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+Uart_SetCharFormat(0x%08p, 0x%08p)", h, char_format);

   if ( !valid_char_format(char_format) ) { return DAL_ERROR; }
   set_char_format(uart_ctxt, char_format);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetCharFormat(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetCtsControl

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetCtsControl(uint32 ClientHandle, DalDeviceHandle *h, UartCtsControlType cts_control)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "+Uart_SetCtsControl(0x%08p, 0x%08X)", h, cts_control);

   DALSYS_SyncEnter(uart_ctxt->register_sync);
   HALuart_SetOutboundFlowCtl(uart_hal, (cts_control == CTS_ENABLE));
   DALSYS_SyncLeave(uart_ctxt->register_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetCtsControl(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetRate

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetRate(uint32 ClientHandle, DalDeviceHandle *h, uint32 baud_rate)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_SetRate(0x%08p, %d)", h, baud_rate);

   if ( !set_baud_rate(uart_ctxt, baud_rate) ) { return DAL_ERROR; }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetRate(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetRtsControl

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetRtsControl(uint32 ClientHandle, DalDeviceHandle *h, UartRtsControlType rts_control)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_SetRtsControl(0x%08p, %d)", h, rts_control);

   if ( !valid_rts_control(rts_control) ) { return DAL_ERROR; }
   uart_ctxt->rts_control = rts_control;
   set_rts_control(uart_ctxt, uart_ctxt->rts_control);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetRtsControl(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_SetCxm

DESCRIPTION:

==================================================================================================*/
DALResult Uart_SetCxm(uint32 ClientHandle, DalDeviceHandle *h, uint32 enabled, uint32 sam)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   DALResult result = DAL_ERROR;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_SetCxm(0x%08p, %d, %x)", h, enabled, sam);

   if (uart_ctxt->prop_is_cxm)
   {
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      if (enabled)
      {
         HALuart_SetCxmEnable(&uart_ctxt->uart_hal, HAL_UART_CXM_ENABLE, sam);
      }
      else
      {
         HALuart_SetCxmEnable(&uart_ctxt->uart_hal, HAL_UART_CXM_BYPASS, 0);
      }
      DALSYS_SyncLeave(uart_ctxt->register_sync);

      result = DAL_SUCCESS;
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_SetCxm(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_TxComplete

DESCRIPTION:

==================================================================================================*/
DALResult Uart_TxComplete(uint32 ClientHandle, DalDeviceHandle *h, uint32 *tx_complete)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_TxComplete(0x%08p, 0x%08p)", h, tx_complete);

   *tx_complete = HALuart_GetStatus(uart_hal) & HAL_UART_STATUS_TX_EMPTY;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_TxComplete(DAL_SUCCESS)");
   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_TxSingleChar

DESCRIPTION:
   WARNING: This function is broken.  It does not send the character right
   away and it may corrupt a transfer in progress.  It needs to be rethought
   and rewritten.

==================================================================================================*/
DALResult Uart_TxSingleChar(uint32 ClientHandle, DalDeviceHandle *h, uint8 character)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   uint32 bytes_sent;
   uint32 i;
   DALResult result = DAL_ERROR;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_TxSingleChar(0x%08p, 0x%02X)", h, character);

   DALSYS_SyncEnter(uart_ctxt->tx_sync);

   HALuart_SetDmaNumTxChars(uart_hal, 1);

   for (i = 0; i < 10000; i++)
   {
      DALSYS_SyncEnter(uart_ctxt->register_sync);
      bytes_sent = HALuart_WriteTxFifo(uart_hal, &character, 1);
      DALSYS_SyncLeave(uart_ctxt->register_sync);

      if (bytes_sent == 1) { result = DAL_SUCCESS; break; }
   }

   DALSYS_SyncLeave(uart_ctxt->tx_sync);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_TxSingleChar(%d)", result);
   return result;
}

/*==================================================================================================

FUNCTION: Uart_Write

DESCRIPTION:

==================================================================================================*/
DALResult Uart_Write(uint32 ClientHandle, DalDeviceHandle *h, const uint8 *write_buffer, uint32 buffer_len,
                     uint32 *bytes_written)
{
   UART_CONTEXT *uart_ctxt = ((DalUartHandle *)h)->uart_ctxt;
   DALDEVICEID id = uart_ctxt->dal_id;
   HALUartObject *uart_hal = &uart_ctxt->uart_hal;
   DALResult dal_result = DAL_ERROR;
   boolean transmit_ok;
   DALSYSMemInfo tx_data_info;
   uint32 num_bytes;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+Uart_Write(0x%08p, 0x%08p, 0x%08X, 0x%08p)",
                                             h, write_buffer, buffer_len, bytes_written);

   *bytes_written = 0;

   if (buffer_len == 0)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_Write: No data to send");
   }
   else
   {
      DALSYS_SyncEnter(uart_ctxt->tx_sync);

      if (uart_ctxt->tx_in_progress)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_Write: TX transfer in progress");
      }
      else if (uart_ctxt->tx_halt || uart_ctxt->tx_wait)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "Uart_Write: TX halt/wait in progress");
      }
      else
      {
         uart_ctxt->tx_in_progress = TRUE;

         // Copy data out of client's buffer
         DALSYS_MemInfo(uart_ctxt->tx_handle, &tx_data_info);
         num_bytes = (buffer_len <= TX_BUF_SIZE) ? (buffer_len) : (TX_BUF_SIZE);
         memcpy((uint8 *)tx_data_info.VirtualAddr, write_buffer, num_bytes);

         if (uart_ctxt->use_dma)
         {
            if ( BAM(uart_ctxt) ) { HALuart_StartTxSideband(uart_hal); }
            else                  { HALuart_SetDmaNumTxChars(uart_hal, num_bytes); }

            uart_ctxt->tx_descriptor.physical_address  = (void*)tx_data_info.PhysicalAddr;
            uart_ctxt->tx_descriptor.bytes_to_transfer = num_bytes;
            uart_ctxt->tx_descriptor.bytes_transferred = 0;
            uart_ctxt->tx_descriptor.error             = 0;
            uart_ctxt->tx_descriptor.transfer_complete = 0;

            transmit_ok = dma_transmit(uart_ctxt->dma_ctxt, &uart_ctxt->tx_descriptor);
         }
         else
         {
            HALuart_SetDmaNumTxChars(uart_hal, num_bytes);

            uart_ctxt->tx_ptr        = (uint8 *)tx_data_info.VirtualAddr;
            uart_ctxt->tx_bytes_left = num_bytes;

            transmit_ok = transmit_chunk(uart_ctxt);
         }

         if (transmit_ok) { *bytes_written = num_bytes; dal_result = DAL_SUCCESS; }
         else             { uart_ctxt->tx_in_progress = FALSE; }
      }

      DALSYS_SyncLeave(uart_ctxt->tx_sync);
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-Uart_Write(%d)", dal_result);
   return dal_result;
}
