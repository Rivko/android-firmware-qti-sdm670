/*==================================================================================================

FILE: tal_interrupt.c

DESCRIPTION: This module defines the reference implementation of the interrupt APIs for the
             Target Abstraction Layer.  It uses the DAL interrupt architecture.

                     Copyright (c) 2012-2015 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/tal/src/tal_interrupt.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_interrupt_close
   tal_interrupt_deregister
   tal_interrupt_dpc_disable
   tal_interrupt_dpc_enable
   tal_interrupt_dpc_schedule
   tal_interrupt_open
   tal_interrupt_register
   tal_interrupt_sync_enter
   tal_interrupt_sync_exit

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "comdef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIGPIOInt.h"
#include "DDIInterruptController.h"
#include "tal.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define MAX_IRQS       2     // (1) UART (2) RX GPIO
#define TCSR_MEM_SIZE  4096

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   IRQ_ENABLE,
   IRQ_DISABLE,
} TCSR_CONFIG;

typedef enum
{ 
   SCHEDULED,
   RUNNING
} DPC_STATE_BITS;

typedef struct int_object
{
   TAL_INTERRUPT_ID      interrupt_id;     // contains properties describing this interrupt
   TAL_INTERRUPT_HANDLE  open_handle;      // pointer to TAL_INTERRUPT_CONTEXT
   DALSYSEventHandle     workloop_event;   // signaling this event will wake the workloop
   boolean               event_queued;     // TRUE if the event is added to the workloop
   DALSYSSyncHandle      isr_sync;         // mutex used to synchronize with the ISR
   boolean               isr_running;      // TRUE while the client ISR is running
   DALSYSSyncHandle      dpc_sync;         // mutex that protects DPC variables
   uint32                dpc_state;        // DPC scheduling and execution status
   uint32                dpc_disable_cnt;  // count of active disable requests
   TAL_INTERRUPT_ISR     client_isr;       // client ISR callback
   TAL_INTERRUPT_DPC     client_dpc;       // client DPC callback
   void                 *client_ctxt;      // context passed to client ISR and DPC callbacks
   boolean               isr_registered;   // TRUE if interrupt is registered with intr controller
} INT_OBJECT;

typedef struct tal_interrupt_context
{
   DALDEVICEID           client_id;        // device ID of client (used for logging)
   uint32                max_irqs;         // maximum number of registered interrupts
   INT_OBJECT           *interrupt_array;  // preallocated array of interrupt objects
   DalDeviceHandle      *interrupt_dal;    // handle to DAL interrupt device
   DalDeviceHandle      *gpioint_dal;      // handle to DAL GPIO interrupt device
   uint32                int_sel_addr;     // (from XML props) address of UART_INT_SEL register
   uint32                int_sel_mask;     // (from XML props) mask used to enable/disable IRQ
} TAL_INTERRUPT_CONTEXT;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

// Non-HLOS TN image does not allow workloop destroy. Creating one global workloop for all the
// ports.
static DALSYSWorkLoopHandle workloop_handle;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static DALResult           array_create(TAL_INTERRUPT_HANDLE handle);
static void                array_destroy(TAL_INTERRUPT_HANDLE handle);
static INT_OBJECT         *array_find(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt_id);
static void                atomic_bit_clear(uint32 volatile *addr, uint32 bit_position,
                                            DALSYSSyncHandle mutex);
static void                atomic_bit_set(uint32 volatile *addr, uint32 bit_position,
                                          DALSYSSyncHandle mutex);
static uint32              atomic_compare_exchange(uint32 volatile *addr, uint32 new,
                                                   uint32 expected, DALSYSSyncHandle mutex);
static uint32              atomic_decrement(uint32 volatile *addr, DALSYSSyncHandle mutex);
static uint32              atomic_increment(uint32 volatile *addr, DALSYSSyncHandle mutex);
static uint32              atomic_read(uint32 volatile *addr, DALSYSSyncHandle mutex);
static DALResult           configure_tcsr(TAL_INTERRUPT_CONTEXT *ctxt, TCSR_CONFIG config);
static void                deregister_isr(INT_OBJECT *int_object);
static DALResult           get_properties(TAL_INTERRUPT_CONTEXT *ctxt);
static void                get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                                              const char *property_name, uint32 *property_value,
                                              uint32 default_value);
static TAL_RESULT          register_isr(INT_OBJECT *int_object,
                                        TAL_INTERRUPT_ID *interrupt, TAL_INTERRUPT_TRIGGER trigger,
                                        TAL_INTERRUPT_ISR isr, TAL_INTERRUPT_DPC dpc, void *ctxt);
static void                service_dpc(INT_OBJECT *int_object);
static void               *service_interrupt(void *context);
static DALResult           service_workloop(DALSYSEventHandle event_handle, void *context);
static GPIOIntTriggerType  tal_to_gpio_trigger(TAL_INTERRUPT_TRIGGER trigger);
static uint32              tal_to_irq_trigger(TAL_INTERRUPT_TRIGGER trigger);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: array_create

DESCRIPTION:
   Allocate an array of interrupt objects.  This needs to be done during open because
   registration might be requested from an interrupt handler when we cannot allocate memory.

==================================================================================================*/
static DALResult array_create(TAL_INTERRUPT_HANDLE handle)
{
   INT_OBJECT *int_array;
   DALResult result;
   uint32 array_size;
   uint32 i;

   array_size = sizeof(INT_OBJECT) * handle->max_irqs;

   result = DALSYS_Malloc(array_size, (void **)&int_array);
   if (result != DAL_SUCCESS) { goto error1; }

   memset(int_array, 0, array_size);
   handle->interrupt_array = int_array;

   for (i = 0; i < handle->max_irqs; i++)
   {
      result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                  &int_array[i].workloop_event, NULL);
      if (result != DAL_SUCCESS) { goto error2; }

      result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &int_array[i].isr_sync, NULL);
      if (result != DAL_SUCCESS) { goto error2; }

      result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &int_array[i].dpc_sync, NULL);
      if (result != DAL_SUCCESS) { goto error2; }

      int_array[i].open_handle    = handle;

      // Each interrupt will get its own workloop event.  All workloop events
      // will use the same workloop and the same callback (service_workloop) but with
      // different context parameters (a pointer to a unique INT_OBJECT).

      result = DALSYS_AddEventToWorkLoop(workloop_handle, service_workloop, &int_array[i],
                                      int_array[i].workloop_event, NULL);

      if (result != DAL_SUCCESS) { goto error2; }
      int_array[i].event_queued = TRUE;
   }

   return(DAL_SUCCESS);
error2:
   array_destroy(handle);
error1:
   return(DAL_ERROR);
}

/*==================================================================================================

FUNCTION: array_destroy

DESCRIPTION:
   Deallocate the array of interrupt objects.

==================================================================================================*/
static void array_destroy(TAL_INTERRUPT_HANDLE handle)
{
   INT_OBJECT *int_array = handle->interrupt_array;
   uint32 i;

   for (i = 0; i < handle->max_irqs; i++)
   {
      if (int_array[i].event_queued) 
      { 
         DALSYS_DeleteEventFromWorkLoop(workloop_handle, int_array[i].workloop_event);
      }
      if (int_array[i].workloop_event) { DALSYS_DestroyObject(int_array[i].workloop_event); }
      if (int_array[i].isr_sync)       { DALSYS_DestroyObject(int_array[i].isr_sync);       }
      if (int_array[i].dpc_sync)       { DALSYS_DestroyObject(int_array[i].dpc_sync);       }
   }

   DALSYS_Free(int_array);
}

/*==================================================================================================

FUNCTION: array_find

DESCRIPTION:
   Deallocate an interrupt object from our local array.

==================================================================================================*/
static INT_OBJECT *array_find(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt_id)
{
   uint32 i;

   for (i = 0; i < handle->max_irqs; i++)
   {
      if ( handle->interrupt_array[i].interrupt_id.irq  == interrupt_id->irq  &&
           handle->interrupt_array[i].interrupt_id.gpio == interrupt_id->gpio    )
      {
         return(&handle->interrupt_array[i]);
      }
   }

   return(NULL);
}

/*==================================================================================================

FUNCTION: atomic_bit_clear

DESCRIPTION:
   Clear the designated bit in the word at the designated address.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static void atomic_bit_clear(uint32 volatile *addr, uint32 bit_position, DALSYSSyncHandle mutex)
{
   DALSYS_SyncEnter(mutex);
   *addr &= ~(1 << bit_position);
   DALSYS_SyncLeave(mutex);
}

/*==================================================================================================

FUNCTION: atomic_bit_set

DESCRIPTION:
   Set the designated bit in the word at the designated address.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static void atomic_bit_set(uint32 volatile *addr, uint32 bit_position, DALSYSSyncHandle mutex)
{
   DALSYS_SyncEnter(mutex);
   *addr |= (1 << bit_position);
   DALSYS_SyncLeave(mutex);
}

/*==================================================================================================

FUNCTION: atomic_compare_exchange

DESCRIPTION:
   Compare the word at the designated address with an expected value.  If it matches then write
   a new value.  Return the old value.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static uint32 atomic_compare_exchange(uint32 volatile *addr, uint32 new, uint32 expected,
                                      DALSYSSyncHandle mutex)
{
   uint32 old;

   DALSYS_SyncEnter(mutex);
   old = *addr;
   if (old == expected) { *addr = new; }
   DALSYS_SyncLeave(mutex);

   return(old);
}

/*==================================================================================================

FUNCTION: atomic_decrement

DESCRIPTION:
   Decrement the word at the designated address.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static uint32 atomic_decrement(uint32 volatile *addr, DALSYSSyncHandle mutex)
{
   uint32 new_value;

   DALSYS_SyncEnter(mutex);
   new_value = --(*addr);
   DALSYS_SyncLeave(mutex);

   return(new_value);
}

/*==================================================================================================

FUNCTION: atomic_increment

DESCRIPTION:
   Increment the word at the designated address.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static uint32 atomic_increment(uint32 volatile *addr, DALSYSSyncHandle mutex)
{
   uint32 new_value;

   DALSYS_SyncEnter(mutex);
   new_value = ++(*addr);
   DALSYS_SyncLeave(mutex);

   return(new_value);
}

/*==================================================================================================

FUNCTION: atomic_read

DESCRIPTION:
   Read the word at the designated address.

   This operation must be atomic (with respect to other calls to atomic_xxx), ordered (memory
   barriers before and after), and volatile (the compiler won't optimize it away).

==================================================================================================*/
static uint32 atomic_read(uint32 volatile *addr, DALSYSSyncHandle mutex)
{
   uint32 old;

   DALSYS_SyncEnter(mutex);
   old = *addr;
   DALSYS_SyncLeave(mutex);

   return(old);
}

/*==================================================================================================

FUNCTION: configure_tcsr

DESCRIPTION:
   This function enables/disables the IRQ for the current UART device.

==================================================================================================*/
static DALResult configure_tcsr(TAL_INTERRUPT_CONTEXT *ctxt, TCSR_CONFIG config)
{
   DALDEVICEID id = ctxt->client_id;
   DALSYSMemHandle tcsr_handle;
   DALSYSMemInfo meminfo;
   uint32 base;
   uint32 offset;
   uint32 mask;
   DALResult result;

   if (ctxt->int_sel_addr == 0) { return(DAL_SUCCESS); }

   // This function allocates a block of virtual memory that is page aligned but we are
   // passing a physical address (of the UART_INT_SEL register) that is not page aligned.
   // The virtual and physical addresses of the resulting block will be adjusted down to
   // the nearest page boundary.

   result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED, ctxt->int_sel_addr,
                                  TCSR_MEM_SIZE, &tcsr_handle, NULL);

   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "failed to map physical address");
      return(DAL_ERROR);
   }

   DALSYS_MemInfo(tcsr_handle, &meminfo);
   base   = meminfo.VirtualAddr;
   offset = ctxt->int_sel_addr - meminfo.PhysicalAddr;
   mask   = ctxt->int_sel_mask;

   switch (config)
   {
      case IRQ_ENABLE:  *(volatile uint32 *)(base + offset) |=  mask; break;
      case IRQ_DISABLE: *(volatile uint32 *)(base + offset) &= ~mask; break;
      default:                                                        break;
   }

   DALSYS_DestroyObject(tcsr_handle);
   return(DAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: deregister_isr

DESCRIPTION:
   Deregister the ISR.  After this call the IRQ is disabled.

==================================================================================================*/
static void deregister_isr(INT_OBJECT *int_object)
{
   TAL_INTERRUPT_HANDLE handle = int_object->open_handle;

   if (int_object->interrupt_id.irq)
   {
      DalInterruptController_Unregister(handle->interrupt_dal,
                                        (DALInterruptID)int_object->interrupt_id.irq);
   }
   else if (int_object->interrupt_id.gpio)
   {
      GPIOInt_DeregisterIsr(handle->gpioint_dal,
                            int_object->interrupt_id.gpio, (GPIOINTISR)service_interrupt);
   }

   int_object->isr_registered = FALSE;
}

/*==================================================================================================

FUNCTION: get_properties

DESCRIPTION:
   Get statically configurable DAL properties from XML file.

==================================================================================================*/
static DALResult get_properties(TAL_INTERRUPT_CONTEXT *ctxt)
{
   DALDEVICEID id = ctxt->client_id;
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   DALResult result;

   result = DALSYS_GetDALPropertyHandle(id, handle);
   if (result != DAL_SUCCESS) { goto error; }

   // UartIntSelBase: This is the address of the UART_INT_SEL register in TCSR.
   // UartIntSelVal:  This is the UART_INT_SEL register mask used to enable/disable
   //                 the IRQ for the current UART device.

   get_property_dword(id, handle, "UartIntSelBase", &ctxt->int_sel_addr, 0);
   get_property_dword(id, handle, "UartIntSelVal",  &ctxt->int_sel_mask, 0);

   if ((ctxt->int_sel_addr == 0) && (ctxt->int_sel_mask != 0)) { goto error; }
   if ((ctxt->int_sel_addr != 0) && (ctxt->int_sel_mask == 0)) { goto error; }

   return(DAL_SUCCESS);
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "get_properties: failed");
   return(DAL_ERROR);
}

/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                               const char *property_name, uint32 *property_value,
                               uint32 default_value)
{
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

FUNCTION: register_isr

DESCRIPTION:
   Register the ISR.  After this call the IRQ is enabled and the ISR may be called.

==================================================================================================*/
static TAL_RESULT register_isr(INT_OBJECT *int_object,
                               TAL_INTERRUPT_ID *interrupt, TAL_INTERRUPT_TRIGGER trigger,
                               TAL_INTERRUPT_ISR isr, TAL_INTERRUPT_DPC dpc, void *ctxt)
{
   TAL_INTERRUPT_HANDLE handle = int_object->open_handle;
   DALResult result;

   int_object->interrupt_id    = *interrupt;
   int_object->isr_running     = FALSE;
   int_object->dpc_state       = 0;
   int_object->dpc_disable_cnt = 0;
   int_object->client_isr      = isr;
   int_object->client_dpc      = dpc;
   int_object->client_ctxt     = ctxt;
   int_object->isr_registered  = TRUE;

   if (int_object->interrupt_id.irq)
   {
      result = DalInterruptController_RegisterISR(handle->interrupt_dal,
                                                  (DALInterruptID)int_object->interrupt_id.irq,
                                                  service_interrupt, int_object,
                                                  tal_to_irq_trigger(trigger));
   }
   else if (int_object->interrupt_id.gpio)
   {
      result = GPIOInt_RegisterIsr(handle->gpioint_dal,
                                   int_object->interrupt_id.gpio,
                                   tal_to_gpio_trigger(trigger),
                                   (GPIOINTISR)service_interrupt, (GPIOINTISRCtx)int_object);
   }
   else
   {
      result = DAL_ERROR;
   }

   if (result != DAL_SUCCESS) { goto error; }

   return(TAL_SUCCESS);
error:
   int_object->isr_registered = FALSE;
   return(TAL_ERROR);
}

/*==================================================================================================

FUNCTION: service_dpc

DESCRIPTION:
   This function checks if a DPC is scheduled and executes it.  It can be called concurrently
   from multiple threads and guarantees that only one thread will execute the DPC.  If a DPC
   is scheduled while it is running it will run again when it completes.

==================================================================================================*/
static void service_dpc(INT_OBJECT *int_object)
{
   DALSYSSyncHandle mutex = int_object->dpc_sync;
   uint32 expected;
   uint32 previous;

   for (;;)
   {
      expected = 1 << SCHEDULED;  // scheduled and not running
      previous = atomic_compare_exchange(&int_object->dpc_state, 1 << RUNNING, expected, mutex);

      // We can exit if atomic_compare_exchange() failed.  This will happen if the DPC is not
      // scheduled or the DPC is already running.

      if (previous != expected) { break; }

      // If the DPC has been disabled we must set the SCHEDULED bit, clear the RUNNING
      // bit, and exit.  This function will be called again when the DPC is re-enabled.

      if ( atomic_read(&int_object->dpc_disable_cnt, mutex) )
      {
         atomic_bit_set(&int_object->dpc_state, SCHEDULED, mutex);
         atomic_bit_clear(&int_object->dpc_state, RUNNING, mutex);
         break;
      }

      // Execute the client DPC and clear the RUNNING bit.

      int_object->client_dpc(int_object->client_ctxt);
      atomic_bit_clear(&int_object->dpc_state, RUNNING, mutex);
   }
}

/*==================================================================================================

FUNCTION: service_interrupt

DESCRIPTION:
   This function is called in the context of an interrupt service thread (IST) when a
   HW interrupt is signaled.

==================================================================================================*/
static void *service_interrupt(void *context)
{
   INT_OBJECT *int_object = (INT_OBJECT *)context;
   TAL_INTERRUPT_HANDLE handle = int_object->open_handle;

   DALSYS_SyncEnter(int_object->isr_sync);
   int_object->isr_running = TRUE;
   int_object->client_isr(int_object->client_ctxt);
   int_object->isr_running = FALSE;
   DALSYS_SyncLeave(int_object->isr_sync);

   if (int_object->interrupt_id.irq)
   {
      // re-enable the IRQ at the QGIC level
      DalInterruptController_InterruptDone(handle->interrupt_dal,
                                           (DALInterruptID)int_object->interrupt_id.irq);
   }

   service_dpc(int_object);
   return(NULL);
}

/*==================================================================================================

FUNCTION: service_workloop

DESCRIPTION:
   This function is called in the context of a workloop thread when a workloop event
   is signaled.

==================================================================================================*/
static DALResult service_workloop(DALSYSEventHandle event_handle, void *context)
{
   service_dpc((INT_OBJECT *)context);
   return(DAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: tal_to_gpio_trigger

DESCRIPTION:
   Convert from TAL_INTERRUPT_TRIGGER to GPIOINT_TRIGGER.

==================================================================================================*/
static GPIOIntTriggerType tal_to_gpio_trigger(TAL_INTERRUPT_TRIGGER trigger)
{
   switch (trigger)
   {
      case TAL_INTERRUPT_TRIGGER_HIGH:    return(GPIOINT_TRIGGER_HIGH);
      case TAL_INTERRUPT_TRIGGER_LOW:     return(GPIOINT_TRIGGER_LOW);
      case TAL_INTERRUPT_TRIGGER_RISING:  return(GPIOINT_TRIGGER_RISING);
      case TAL_INTERRUPT_TRIGGER_FALLING: return(GPIOINT_TRIGGER_FALLING);
      default:                            return(GPIOINT_TRIGGER_HIGH);
   }
}

/*==================================================================================================

FUNCTION: tal_to_irq_trigger

DESCRIPTION:
   Convert from TAL_INTERRUPT_TRIGGER to DALINTRCTRL_ENABLE.

==================================================================================================*/
static uint32 tal_to_irq_trigger(TAL_INTERRUPT_TRIGGER trigger)
{
   switch (trigger)
   {
      case TAL_INTERRUPT_TRIGGER_HIGH:    return(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
      case TAL_INTERRUPT_TRIGGER_LOW:     return(DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER);
      case TAL_INTERRUPT_TRIGGER_RISING:  return(DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
      case TAL_INTERRUPT_TRIGGER_FALLING: return(DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER);
      default:                            return(DALINTRCTRL_ENABLE_DEFAULT_SETTINGS);
   }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_interrupt_close

==================================================================================================*/
void tal_interrupt_close(TAL_INTERRUPT_HANDLE handle)
{
   INT_OBJECT *int_object;
   uint32 i;

   for (i = 0; i < handle->max_irqs; i++)
   {
      int_object = &handle->interrupt_array[i];

      if (int_object->isr_registered)
      {
         deregister_isr(int_object);
      }
   }

   configure_tcsr(handle, IRQ_DISABLE);
   array_destroy(handle);
   DalDevice_Close(handle->gpioint_dal);
   DalDevice_Close(handle->interrupt_dal);
   DAL_DeviceDetach(handle->gpioint_dal);
   DAL_DeviceDetach(handle->interrupt_dal);
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_interrupt_deregister

==================================================================================================*/
void tal_interrupt_deregister(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;

   int_object = array_find(handle, interrupt);
   if (int_object == NULL) { return; }

   deregister_isr(int_object);
}

/*==================================================================================================

FUNCTION: tal_interrupt_dpc_disable

==================================================================================================*/
void tal_interrupt_dpc_disable(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;
   DALSYSSyncHandle mutex;

   int_object = array_find(handle, interrupt);
   if (int_object == NULL) { return; }

   mutex = int_object->dpc_sync;
   atomic_increment(&int_object->dpc_disable_cnt, mutex);

   while ( atomic_read(&int_object->dpc_state, mutex) & (1 << RUNNING) ) { ; }  // busy wait
}

/*==================================================================================================

FUNCTION: tal_interrupt_dpc_enable

==================================================================================================*/
void tal_interrupt_dpc_enable(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;
   DALSYSSyncHandle mutex;
   uint32 dpc_disable_cnt;
   uint32 dpc_state;

   int_object = array_find(handle, interrupt);
   if (int_object == NULL) { return; }

   mutex = int_object->dpc_sync;
   dpc_disable_cnt = atomic_decrement(&int_object->dpc_disable_cnt, mutex);

   if (dpc_disable_cnt == 0)
   {
      while ( (dpc_state = atomic_read(&int_object->dpc_state, mutex)) & (1 << RUNNING) ) { ; }

      if (dpc_state & (1 << SCHEDULED))
      {
         DALSYS_EventCtrl(int_object->workloop_event, DALSYS_EVENT_CTRL_TRIGGER);
      }
   }
}

/*==================================================================================================

FUNCTION: tal_interrupt_dpc_schedule

==================================================================================================*/
void tal_interrupt_dpc_schedule(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;
   DALSYSSyncHandle mutex;

   int_object = array_find(handle, interrupt);
   if (int_object == NULL) { return; }

   mutex = int_object->dpc_sync;
   atomic_bit_set(&int_object->dpc_state, SCHEDULED, mutex);

   if (!int_object->isr_running)
   {
      // It would be functionally correct to signal the worker thread unconditionally.
      // However, we want to optimize the common case where the DPC is scheduled from
      // inside the ISR.  In this case we will let the interrupt service thread execute
      // the DPC after the ISR completes.

      DALSYS_EventCtrl(int_object->workloop_event, DALSYS_EVENT_CTRL_TRIGGER);
   }
}

/*==================================================================================================

FUNCTION: tal_interrupt_open

==================================================================================================*/
TAL_RESULT tal_interrupt_open(TAL_INTERRUPT_HANDLE *phandle, uint32 client_id)
{
   TAL_INTERRUPT_CONTEXT *ctxt;
   DALResult result;

   result = DALSYS_Malloc(sizeof(TAL_INTERRUPT_CONTEXT), (void **)&ctxt);
   if (result != DAL_SUCCESS) { goto error1; }

   ctxt->client_id = client_id;
   ctxt->max_irqs  = MAX_IRQS;

   result = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &ctxt->interrupt_dal);
   if (result != DAL_SUCCESS) { goto error2; }

   result = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &ctxt->gpioint_dal);
   if (result != DAL_SUCCESS) { goto error3; }

   result = DalDevice_Open(ctxt->interrupt_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS) { goto error4; }

   result = DalDevice_Open(ctxt->gpioint_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS) { goto error5; }

   if (!workloop_handle)
   {
      result = DALSYS_RegisterWorkLoop(0, ctxt->max_irqs, &workloop_handle, NULL); 
      if (result != DAL_SUCCESS) { goto error6; }
   }

   result = array_create(ctxt);
   if (result != DAL_SUCCESS) { goto error7; }

   result = get_properties(ctxt);
   if (result != DAL_SUCCESS) { goto error8; }

   result = configure_tcsr(ctxt, IRQ_ENABLE);
   if (result != DAL_SUCCESS) { goto error8; }

   *phandle = ctxt;
   return(TAL_SUCCESS);

error8:
   array_destroy(ctxt);
error7:
   // Current DAL implementation on MDM APSS image does not allow workloops being
   // destroyed. Uncomment it once DAL team fixes this issue.
   
   //DALSYS_DestroyObject(workloop_handle);
error6:
   DalDevice_Close(ctxt->gpioint_dal);
error5:
   DalDevice_Close(ctxt->interrupt_dal);
error4:
   DAL_DeviceDetach(ctxt->gpioint_dal);
error3:
   DAL_DeviceDetach(ctxt->interrupt_dal);
error2:
   DALSYS_Free(ctxt);
error1:
   *phandle = NULL;
   return(TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_interrupt_register

==================================================================================================*/
TAL_RESULT tal_interrupt_register(TAL_INTERRUPT_HANDLE handle,
                                  TAL_INTERRUPT_ID *interrupt, TAL_INTERRUPT_TRIGGER trigger,
                                  TAL_INTERRUPT_ISR isr, TAL_INTERRUPT_DPC dpc, void *ctxt)
{
   INT_OBJECT *int_object;
   TAL_INTERRUPT_ID null_interrupt = { 0, 0 };
   TAL_RESULT result;

   int_object = array_find(handle, interrupt);  // check if IRQ is already available
   
   if (int_object == NULL)
   {
      int_object = array_find(handle, &null_interrupt);  // find unused interrupt object
      if (int_object == NULL) { return(TAL_ERROR); }
   }

   result = register_isr(int_object, interrupt, trigger, isr, dpc, ctxt);
   if (result != TAL_SUCCESS) { return(TAL_ERROR); }

   return(TAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: tal_interrupt_sync_enter

==================================================================================================*/
void tal_interrupt_sync_enter(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;

   int_object = array_find(handle, interrupt);
   if (int_object) { DALSYS_SyncEnter(int_object->isr_sync); }
}

/*==================================================================================================

FUNCTION: tal_interrupt_sync_exit

==================================================================================================*/
void tal_interrupt_sync_exit(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt)
{
   INT_OBJECT *int_object;

   int_object = array_find(handle, interrupt);
   if (int_object) { DALSYS_SyncLeave(int_object->isr_sync); }
}
