/*==================================================================================================

FILE: tal_interrupt_qurt.c

DESCRIPTION: This module defines the Qurt-specific implementation of the interrupt APIs
             for the Target Abstraction Layer (TAL). This implementation uses a workloop
             to synchronize access to IRQ interrupts used by the UART driver.

                              Copyright (c) 2012-2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/tal/src/qurt/tal_interrupt_qurt.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_interrupt_close
   tal_interrupt_done
   tal_interrupt_open
   tal_interrupt_register
   tal_interrupt_trigger
   tal_interrupt_unregister

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIGPIOInt.h"
#include "DDIInterruptController.h"
#endif

#include "tal.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define WL_PRIORITY                   0   // Use the default workloop priority
#define TAL_INTERRUPT_MAX             2   // Max number of IRQ interrupts supported
#define MIN_PAGE_SIZE                 (4*1024)
#define UART_INT_SEL_MEM_SIZE         MIN_PAGE_SIZE

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

// Workloop event types
// Keep HW events in numerical order from 0
typedef enum
{
  WL_SW_TRIGGERED_EVENT_0 = 0,
  WL_SW_TRIGGERED_EVENT_1 = 1,
  WL_MAX_EVENTS           = 2
} WL_EVENT_TYPE;

typedef struct
{
   TAL_INTERRUPT_HANDLE  handle;          // Handle to tal_interrupt
   TAL_INTERRUPT_ID      tal_int_id;      // TAL interrupt ID for the interrupt
   TAL_INTERRUPT_TRIGGER trigger;         // the TAL interrupt trigger
   DALSYSSyncHandle      isr_sync;        // This mutex must be locked while calling the isr
   TAL_INTERRUPT_ISR     isr;             // The ISR for the interrupt
   void                  *context;        // The interrupt ISR parameter
   WL_EVENT_TYPE         wl_event_hw;     // event type for hw triggered event
} STUB_CONTEXT;

typedef struct tal_interrupt_context
{
   DALDEVICEID              client_id;                      // DALDEVICEID
   DalDeviceHandle          *daldev_intc;                   // handle to DAL device for interrupt controller
   DalDeviceHandle          *daldev_gpio;                   // handle to DAL device for GPIO controller
   DALSYSWorkLoopHandle     workloop_handle;                // handle to the DAL workloop
   DALSYSEventHandle        workloop_event[WL_MAX_EVENTS];  // workloop events

   // (from XML props) physical address of the TCSR UART int_sel register
   uint32               uart_int_sel_physical;

   // contains the virtual address mapping for UART int sel register
   DALSYSMemHandle      uart_int_sel_mem_handle;

   // Following used for workloop in stub function and when triggering
   STUB_CONTEXT stub_context[TAL_INTERRUPT_MAX];
} TAL_INTERRUPT_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static boolean            configure_irq(TAL_INTERRUPT_HANDLE handle, boolean configure);
static void               get_property_dword(DALDEVICEID id,
                                             DALSYSPropertyHandle handle,
                                             const char *property_name,
                                             uint32 *property_value);
static void *             stub_isr(void *ctxt);
static void               tcsr_config_close(TAL_INTERRUPT_HANDLE handle);
static TAL_RESULT         tcsr_config_open(TAL_INTERRUPT_HANDLE phandle, uint32 client_id);
static void               wl_close(TAL_INTERRUPT_HANDLE handle);
static DALResult          wl_handle_sw_event(DALSYSEventHandle event_handle, void *context);
static TAL_RESULT         wl_open(TAL_INTERRUPT_HANDLE handle);
static void               wl_trigger_event(TAL_INTERRUPT_HANDLE handle, WL_EVENT_TYPE wl_event);
static uint32             tal_trigger_to_dal_irq_trigger(TAL_INTERRUPT_TRIGGER trigger);
static GPIOIntTriggerType tal_trigger_to_gpio_trigger(TAL_INTERRUPT_TRIGGER trigger);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: configure_irq

DESCRIPTION:
   This function routes the UART IRQ to the appropriate processor.
   It returns TRUE if successful, or FALSE otherwise

==================================================================================================*/
static boolean configure_irq(TAL_INTERRUPT_HANDLE handle, boolean configure)
{
   DALSYSMemInfo          meminfo;
   volatile uint32       *uart_int_sel;
   uint8                 *ptr;
   uint32                 val;
   uint32                 mask;
   DALSYSMemHandle       *mem_handle;
   uint32                 offset;
   DALDEVICEID            id = handle->client_id;
   DALResult              dal_result;
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "+configure_irq()");

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "Routing UART IRQ");

   //  Get a pointer to the register.  This is harder than it looks.  The
   //  DALSYS malloced region will be on a MIN_PAGE_SIZE boundary (which may
   //  be OS/target/platform specific).  The actual physical address in the
   //  meminfo will be adjusted downward.  The virtual address must be adjusted
   //  by this same size;

   mem_handle = &handle->uart_int_sel_mem_handle;
   DALSYS_MemInfo(*mem_handle, &meminfo);

   ptr = (uint8*) meminfo.VirtualAddr;
   offset = handle->uart_int_sel_physical - meminfo.PhysicalAddr;
   ptr += offset;
   uart_int_sel = (uint32*) ptr;

   //  Get the UART_INT_SEL register bitmask value for the device
   //  to turn on/off the IRQ for this device.
   dal_result = DALSYS_GetDALPropertyHandle(id, property_handle);
   if (dal_result != DAL_SUCCESS)
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                    "DALSYS_GetDALPropertyHandle failed %d", dal_result );
      return FALSE;
   }

   get_property_dword(id, property_handle, "UartIntSelVal", &mask);
   if (0 == mask)
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                    "Invalid Interrupt Select value for UART TCSR. Value = %d", mask );
      return FALSE;
   }

   //  Twiddle the bit.
   val = *uart_int_sel;

   if (configure)
   {
     DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "Writing 0x%x to 0x%08x",
         mask, meminfo.PhysicalAddr);
      val |= mask;
   }
   else
   {
     DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "Clearing 0x%x from 0x%08x",
         mask, meminfo.PhysicalAddr);
      val &= ~mask;
   }

   *uart_int_sel = val;


   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "-configure_irq()");

   return TRUE;
}

/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(DALDEVICEID id,
                                   DALSYSPropertyHandle property_handle,
                                   const char *property_name,
                                   uint32 *property_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;


   result = DALSYS_GetPropertyValue(property_handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
   }
   else
   {
      *property_value = 0;
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_property_dword: Property name = %s, Value = 0x%08X",
                   property_name,
                   *property_value);

}

/*==================================================================================================

FUNCTION: stub_isr

DESCRIPTION:
   The DAL Interrupt Controller calls this function in the context of an interrupt
   service thread (IST) when a HW interrupt for a registered IRQ occurs.  It
   calls the registered ISR directly.

   On Qurt, this ISR is called from a dedicated interrupt service thread, so
   this function can lock the mutex to guarantee that only one event (a
   hardware interrupt, or a software-generated event) is calling the 'real'
   ISR at any time, even though the function calls will take place from
   different threads.

==================================================================================================*/
static void * stub_isr(void *ctxt)
{
   STUB_CONTEXT *context = (STUB_CONTEXT *) ctxt;
   TAL_INTERRUPT_ISR isr;

   DALSYS_SyncEnter(context->isr_sync);
   isr = context->isr;
   if(isr)
   {
      isr(context->context);
   }
   DALSYS_SyncLeave(context->isr_sync);

   return NULL;
}

/*==================================================================================================

FUNCTION: tcsr_config_close

DESCRIPTION:

==================================================================================================*/
static void tcsr_config_close(TAL_INTERRUPT_HANDLE handle)
{
   configure_irq(handle, FALSE);

   if (handle->uart_int_sel_mem_handle)
   {
      DALSYS_DestroyObject(handle->uart_int_sel_mem_handle);
   }

}

/*==================================================================================================

FUNCTION: tcsr_config_open

DESCRIPTION:

==================================================================================================*/
static TAL_RESULT tcsr_config_open(TAL_INTERRUPT_HANDLE phandle, uint32 client_id)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);
   DALResult result;
   uint32 uart_int_sel_physical;

   DALSYSMemHandle uart_int_sel_mem_handle;

   uart_int_sel_mem_handle = 0;

   result = DALSYS_GetDALPropertyHandle(client_id, property_handle);
   if (result != DAL_SUCCESS) { goto error; }

   get_property_dword(client_id, property_handle, "UartIntSelBase", &uart_int_sel_physical);

   if (0 == uart_int_sel_physical)
   {
      DALSYS_LogEvent(client_id,
                      DALSYS_LOGEVENT_INFO,
                      "Not configuring the TCSR as UartIntSelBase is 0");
      return TAL_SUCCESS;
   }

   //  Map in UART int sel register.

   result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  uart_int_sel_physical,
                                  UART_INT_SEL_MEM_SIZE,
                                  &uart_int_sel_mem_handle,
                                  NULL);

   if (result != DAL_SUCCESS) { goto error; }

   phandle->uart_int_sel_physical   = uart_int_sel_physical;
   phandle->uart_int_sel_mem_handle = uart_int_sel_mem_handle;

   if (configure_irq(phandle, TRUE) != TRUE)
   {
      goto error;
   }

   return TAL_SUCCESS;

error:

   if (uart_int_sel_mem_handle)
   {
      DALSYS_DestroyObject(uart_int_sel_mem_handle);
   }

   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: wl_close

DESCRIPTION:
   De-initializes the tal_interrupt_wl.

==================================================================================================*/
static void wl_close(TAL_INTERRUPT_HANDLE handle)
{
   int i;
   DALDEVICEID id = handle->client_id;
   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+wl_close");

   for( i = 0; i < WL_MAX_EVENTS; i++ )
   {
      if (handle->workloop_event[i] != NULL)
         DALSYS_DestroyObject(handle->workloop_event[i]);
   }

   if (handle->workloop_handle != NULL)
      DALSYS_DestroyObject(handle->workloop_handle);

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-wl_close");
}

/*==================================================================================================

FUNCTION: wl_handle_sw_event

DESCRIPTION:
   This function is the handler for workloop events originating from software triggered
   IRQ interrupts.  It is called by the workloop thread.  It invokes the registered
   ISR for the IRQ using the registered parameter.  The workloop handler function does
   not re-register the interrupt.

==================================================================================================*/
static DALResult wl_handle_sw_event(DALSYSEventHandle event_handle, void *context)
{
   STUB_CONTEXT *stub_context = (STUB_CONTEXT *) context;
   DALDEVICEID id = stub_context->handle->client_id;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+wl_handle_sw_event");

   DALSYS_SyncEnter(stub_context->isr_sync);

   // Invoke isr
   stub_context->isr( stub_context->context );

   DALSYS_SyncLeave(stub_context->isr_sync);

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-wl_handle_sw_event");

   return DAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: wl_open

DESCRIPTION:
   Initializes the tal_interrupt_wl for a UART port.

==================================================================================================*/
static TAL_RESULT wl_open(TAL_INTERRUPT_HANDLE handle)
{
   DALResult             result;
   DALSYSWorkLoopExecute workloop_func = NULL;
   DALDEVICEID           id = handle->client_id;
   int                   i;
   void                  *context;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+wl_open");

   // Register the workloop
   result = DALSYS_RegisterWorkLoop( WL_PRIORITY,
                                     WL_MAX_EVENTS,
                                     &handle->workloop_handle,
                                     NULL );

   if( result != DAL_SUCCESS )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "Error %d creating workloop.", result );
      goto error;
   }

   // Create workloop events
   for( i = 0; i < WL_MAX_EVENTS; i++ )
   {
      result = DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                   &handle->workloop_event[i],
                                   NULL );

      if( result != DAL_SUCCESS )
      {
         DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "Error %d creating event.", result );
         goto error;
      }

      switch (i)
      {
         case WL_SW_TRIGGERED_EVENT_0:
            workloop_func = wl_handle_sw_event;
            context = &handle->stub_context[0];
            break;

         case WL_SW_TRIGGERED_EVENT_1:
            workloop_func = wl_handle_sw_event;
            context = &handle->stub_context[1];
            break;

         default:
            DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "Unrecognized event", result );
            goto error;
      }

      // Add event to workloop
      result = DALSYS_AddEventToWorkLoop( handle->workloop_handle,
                                          workloop_func,
                                          context,
                                          handle->workloop_event[i],
                                          NULL );

      if ( result != DAL_SUCCESS )
      {
         DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                          "Error %d adding event to workloop.", result );
         goto error;
      }
   }

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-wl_open");
   return TAL_SUCCESS;

error:;
   wl_close(handle);
   DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "-wl_open -- FAILED");
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: wl_trigger_event

DESCRIPTION
   Triggers the event that causes the workloop thread to act.

==================================================================================================*/
static void wl_trigger_event(TAL_INTERRUPT_HANDLE handle, WL_EVENT_TYPE wl_event)
{
   DALDEVICEID id = handle->client_id;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+tal_interrupt_wl_trigger");

   DALSYS_EventCtrl( handle->workloop_event[wl_event], DALSYS_EVENT_CTRL_TRIGGER );

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-tal_interrupt_wl_trigger");
}

/*==================================================================================================

FUNCTION: tal_trigger_to_dal_irq_trigger

DESCRIPTION:

==================================================================================================*/
static uint32 tal_trigger_to_dal_irq_trigger(TAL_INTERRUPT_TRIGGER trigger)
{
   switch (trigger)
   {
      case TAL_INTERRUPT_TRIGGER_HIGH:    return DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      case TAL_INTERRUPT_TRIGGER_LOW:     return DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
      case TAL_INTERRUPT_TRIGGER_RISING:  return DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      case TAL_INTERRUPT_TRIGGER_FALLING: return DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
      default:                            return DALINTRCTRL_ENABLE_DEFAULT_SETTINGS;
   }
}

/*==================================================================================================

FUNCTION: tal_trigger_to_gpio_trigger

DESCRIPTION:

==================================================================================================*/
static GPIOIntTriggerType tal_trigger_to_gpio_trigger(TAL_INTERRUPT_TRIGGER trigger)
{
   switch (trigger)
   {
      case TAL_INTERRUPT_TRIGGER_HIGH:    return GPIOINT_TRIGGER_HIGH;
      case TAL_INTERRUPT_TRIGGER_LOW:     return GPIOINT_TRIGGER_LOW;
      case TAL_INTERRUPT_TRIGGER_RISING:  return GPIOINT_TRIGGER_RISING;
      case TAL_INTERRUPT_TRIGGER_FALLING: return GPIOINT_TRIGGER_FALLING;
      default:                            return GPIOINT_TRIGGER_HIGH;
   }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_interrupt_close

DESCRIPTION:

==================================================================================================*/
void tal_interrupt_close(TAL_INTERRUPT_HANDLE handle)
{
   int i;

   for (i = 0; i < TAL_INTERRUPT_MAX; i++)
   {
      if (handle->stub_context[i].isr_sync)
      {
         DALSYS_DestroyObject(handle->stub_context[i].isr_sync);
      }
   }
   wl_close(handle);
   DalDevice_Close(handle->daldev_intc);
   DAL_DeviceDetach(handle->daldev_intc);
   DalDevice_Close(handle->daldev_gpio);
   DAL_DeviceDetach(handle->daldev_gpio);
   // Close the TCSR configuration.
   tcsr_config_close(handle);

   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_interrupt_done

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_interrupt_done(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *tal_int_id)
{
   DALResult result = DAL_ERROR;

   switch (tal_int_id->type)
   {
      case TAL_INTERRUPT_TYPE_IRQ:
         result = DalInterruptController_InterruptDone(handle->daldev_intc,
                                                       (DALInterruptID)tal_int_id->u.irq_num);
         break;
      case TAL_INTERRUPT_TYPE_GPIO:
         // There is no GPIOInt_DoneInterrupt
         break;
      default:
         break;
   }

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_interrupt_open

DESCRIPTION:
   Opens and attaches to both the DAL interrupt controller and the GPIO controller

==================================================================================================*/
TAL_RESULT tal_interrupt_open(TAL_INTERRUPT_HANDLE *phandle, uint32 client_id)
{
   TAL_INTERRUPT_CONTEXT *tal_interrupt_ctxt = NULL;
   DalDeviceHandle *interrupt_dal = NULL;
   DalDeviceHandle *gpioint_dal = NULL;
   DALResult result;
   TAL_RESULT tal_result;
   boolean intc_opened = FALSE;
   boolean gpio_opened = FALSE;
   int i;

   result = DALSYS_Malloc(sizeof(TAL_INTERRUPT_CONTEXT), (void **)(&tal_interrupt_ctxt));
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   memset(tal_interrupt_ctxt, 0, sizeof(TAL_INTERRUPT_CONTEXT));

   result = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &interrupt_dal);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   result = DalDevice_Open(interrupt_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }
   intc_opened = TRUE;

   result = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &gpioint_dal);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   result = DalDevice_Open(gpioint_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }
   gpio_opened = TRUE;

   for (i = 0; i < TAL_INTERRUPT_MAX; i++)
   {
      result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
            &tal_interrupt_ctxt->stub_context[i].isr_sync, NULL);
      if (result != DAL_SUCCESS)
      {
         goto error;
      }
   }

   // create the workloop
   tal_result = wl_open(tal_interrupt_ctxt);
   if (tal_result != TAL_SUCCESS)
   {
      goto error;
   }

   tal_interrupt_ctxt->daldev_intc = interrupt_dal;
   tal_interrupt_ctxt->daldev_gpio = gpioint_dal;
   tal_interrupt_ctxt->client_id   = client_id;
   *phandle = tal_interrupt_ctxt;

   // Configure the TCSR registers.
   tal_result = tcsr_config_open(tal_interrupt_ctxt, client_id);

   if (tal_result != TAL_SUCCESS)
   {
      goto error;
   }

   return TAL_SUCCESS;

error:
   for (i = 0; i < TAL_INTERRUPT_MAX; i++)
   {
      if (tal_interrupt_ctxt->stub_context[i].isr_sync)
      {
         DALSYS_DestroyObject(tal_interrupt_ctxt->stub_context[i].isr_sync);
      }
   }
   if (intc_opened)        { DalDevice_Close(interrupt_dal); }
   if (interrupt_dal)      { DAL_DeviceDetach(interrupt_dal); }
   if (gpio_opened)        { DalDevice_Close(gpioint_dal); }
   if (gpioint_dal)        { DAL_DeviceDetach(gpioint_dal); }
   if (tal_interrupt_ctxt) { DALSYS_Free(tal_interrupt_ctxt); }
   *phandle = NULL;
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_interrupt_register

DESCRIPTION:
   Registers a tal_interrupt

==================================================================================================*/
TAL_RESULT tal_interrupt_register(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *tal_int_id,
                                  TAL_INTERRUPT_TRIGGER trigger, TAL_INTERRUPT_ISR isr,
                                  void *context)
{
   DALResult dal_result = DAL_ERROR;
   uint32 dal_irq_trigger;
   GPIOIntTriggerType gpio_trigger;
   DALDEVICEID id = handle->client_id;
   int32 i;

   switch (tal_int_id->type)
   {
      case TAL_INTERRUPT_TYPE_IRQ:
         // Parameters need to be saved
         // Check whether it's already been saved
         // If not, find the first available slot
         // Once found, save (or re-save) the parameters
         for (i=0;i<TAL_INTERRUPT_MAX;i++)
         {
            if ((handle->stub_context[i].tal_int_id.u.irq_num == 0) ||
                (handle->stub_context[i].tal_int_id.u.irq_num == tal_int_id->u.irq_num))
            {
               handle->stub_context[i].handle = handle;
               handle->stub_context[i].tal_int_id.type = TAL_INTERRUPT_TYPE_IRQ;
               handle->stub_context[i].tal_int_id.u.irq_num = tal_int_id->u.irq_num;
               handle->stub_context[i].isr = isr;
               handle->stub_context[i].context = context;
               handle->stub_context[i].trigger = trigger;
               handle->stub_context[i].wl_event_hw = (WL_EVENT_TYPE) i;
               break;
            }
         }

         // if not found or no space left for this interrupt, return error
         if (i == TAL_INTERRUPT_MAX)
         {
            DALSYS_LogEvent( id,
                             DALSYS_LOGEVENT_ERROR,
                             "tal_interrupt_register error: no slot found, %d",
                             tal_int_id->u.irq_num );
            break;
         }

         dal_irq_trigger = tal_trigger_to_dal_irq_trigger(trigger);
         dal_result = DalInterruptController_RegisterISR(handle->daldev_intc,
                                                         tal_int_id->u.irq_num,
                                                         stub_isr,
                                                         &handle->stub_context[i],
                                                         dal_irq_trigger);
         break;
      case TAL_INTERRUPT_TYPE_GPIO:
         gpio_trigger = tal_trigger_to_gpio_trigger(trigger);
         dal_result = GPIOInt_RegisterIsr(handle->daldev_gpio,
                                          tal_int_id->u.gpio_num,
                                          gpio_trigger,
                                          (GPIOINTISR) isr,
                                          (GPIOINTISRCtx) context);
         break;
      default:
         break;
   }

   if (dal_result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "tal_interrupt_register: failed, 0x%08X",
                      dal_result);
   }

   return (dal_result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_interrupt_trigger

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_interrupt_trigger(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *tal_int_id)
{
   WL_EVENT_TYPE wl_event;
   DALDEVICEID id = handle->client_id;
   DALResult result = DAL_ERROR;

   switch (tal_int_id->type)
   {
      case TAL_INTERRUPT_TYPE_IRQ:
         // DalInterruptController_InterruptTrigger doesn't work on QuRT for level interrupts
         // Instead, trigger an event on the workloop
         // Determine which context this is so the correct event is used
         if (tal_int_id->u.irq_num == handle->stub_context[0].tal_int_id.u.irq_num)
         {
            wl_event = WL_SW_TRIGGERED_EVENT_0;
         }
         else if (tal_int_id->u.irq_num == handle->stub_context[1].tal_int_id.u.irq_num)
         {
            wl_event = WL_SW_TRIGGERED_EVENT_1;
         }
         else
         {
            DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "tal_interrupt_trigger error: bad ID" );
            return NULL;
         }

         wl_trigger_event(handle, wl_event);
         break;
      case TAL_INTERRUPT_TYPE_GPIO:
         result = GPIOInt_TriggerInterrupt(handle->daldev_gpio, tal_int_id->u.gpio_num);
         break;
      default:
         break;
   }

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_interrupt_unregister

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_interrupt_unregister(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *tal_int_id,
                                    TAL_INTERRUPT_ISR isr)
{
   DALResult result = DAL_ERROR;

   switch (tal_int_id->type)
   {
      case TAL_INTERRUPT_TYPE_IRQ:
         result = DalInterruptController_Unregister(handle->daldev_intc,
                                                    (DALInterruptID)tal_int_id->u.irq_num);
         break;
      case TAL_INTERRUPT_TYPE_GPIO:
         result = GPIOInt_DeregisterIsr(handle->daldev_gpio, tal_int_id->u.gpio_num,
                                        (GPIOINTISR)isr);
         break;
      default:
         break;
   }

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}
