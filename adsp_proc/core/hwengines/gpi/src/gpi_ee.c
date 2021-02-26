/**
  @file gpi_ee.c
  @brief
  This file contains implementation of interfaces that provide a thin
  abstraction to various OS-specific APIs used by the GPI driver.
*/

/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/17   ah      Added isr enable/disable.
10/06/17   ah      Updated logging.
09/13/17   pc      Added support to enable top level qups and ssc qup from ADSP image
05/15/17   ah      QURT changes.
05/05/17   ah      ulog message change
02/22/17   ah      Sync to common GPI driver
02/19/16   ah      Created

===============================================================================
                   Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "gpi_utils.h"


static uint8 gpi_task_stack_arr[GPI_TASK_STACK_SIZE];

#ifdef GPI_DEBUGGING
gpi_msg_type GPI_MSG_LEVEL = GPI_MAX_DEBUG;

/* ULog */
ULogHandle gpi_ulog_handle;                                    /* ULog global handle */
ULogResult gpi_ulog_enable = ULOG_ERR_INITINCOMPLETE;  /* Ulog enabled flag */

/**
 * Ulog message logging interface.
 * Allocates buffer for logging.On successful initialization it will return the Ulog handle.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_ulog_init(void)
{
   /* Ulog Handle and buffer intialization. */
   if (gpi_ulog_enable == ULOG_ERR_INITINCOMPLETE)
   {
      gpi_ulog_enable = ULogFront_RealTimeInit(&gpi_ulog_handle,
                                               "gpi",
                                               GPI_ULOG_BUFFER_MAX,
                                               ULOG_MEMORY_LOCAL,
                                               ULOG_LOCK_OS);
      gpi_log(GPI_DEBUG, 1, "uloginit - gpi_ulog_enable: %d\n", gpi_ulog_enable);
   }
}

/**
 * Ulog message logging interface De-initialization.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_ulog_deinit(void)
{
   gpi_ulog_enable = ULOG_ERR_INITINCOMPLETE;
}

#ifdef ENABLE_GPI_MICRO_ULOG
micro_ULogHandle gpi_uulog_handle;
static uint8 gpi_uulog_buffer[GPI_UULOG_BUFFER_SIZE];

void gpi_uulog_init(void)
{
   uint32              actual_buf_size;
   micro_ULogResult    res;

   res = micro_ULog_CreateLog(&gpi_uulog_handle,
                              "ugpi",
                              (char *)gpi_uulog_buffer,
                              &actual_buf_size,
                              GPI_UULOG_BUFFER_SIZE);

   if ((res != MICRO_ULOG_SUCCESS) || (actual_buf_size == 0))
   {
      gpi_log(GPI_ERROR, 0, "uulog_init - ERR - uulog create failed!\n");
   }
}
#endif
#endif

#ifdef GPI_SSC
uint32 gpi_clocks_on(void)
{
   
   gpi_log(GPI_DEBUG, 0, "gpi_clocks_on - BEFORE calling uClock_IsClockOn!\n");
   if (uClock_IsClockOn(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
       uClock_IsClockOn(CLOCK_SCC_QUPV3_CORE_CLK)   &&
       uClock_IsClockOn(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
       uClock_IsClockOn(CLOCK_SCC_QUPV3_S_HCLK_CLK)
      )
   {
      gpi_log(GPI_DEBUG, 0, "gpi_clocks_on - clocks are on!\n");
      return 1;
   }

   gpi_log(GPI_ERROR, 0, "gpi_clocks_on - some clocks are off!\n");
   return 0;
}

/**
 * Enable GSI clocks.
 *
 * @param[in] none
 *
 * @return   GPI_RETURN_STATUS    Return status.
 */
GPI_RETURN_STATUS gpi_clock_enable(void)
{
   
   gpi_log(GPI_DEBUG, 0, "gpi_clock_enable - BEFORE uClock_EnableClock!\n");
   if (uClock_EnableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
       uClock_EnableClock(CLOCK_SCC_QUPV3_CORE_CLK)   &&
       uClock_EnableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
       uClock_EnableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK)
      )
   {
      gpi_log(GPI_DEBUG, 0, "gpi_clock_enable - AFTER uClock_EnableClock!\n");
      return GPI_STATUS_SUCCESS;
   }

   gpi_log(GPI_ERROR, 0, "gpi_clock_enable - ERROR - FAILED AFTER uClock_EnableClock!\n");
   return GPI_STATUS_ERROR;
}

/**
 * Disable GSI clocks.
 *
 * @param[in] none
 *
 * @return   GPI_RETURN_STATUS    Return status.
 */
GPI_RETURN_STATUS gpi_clock_disable(void)
{
   gpi_log(GPI_DEBUG, 0, "gpi_clock_disable - BEFORE uClock_DisableClock!\n");
   if (uClock_DisableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
       uClock_DisableClock(CLOCK_SCC_QUPV3_CORE_CLK)   &&
       uClock_DisableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
       uClock_DisableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK)
      )
   {
      gpi_log(GPI_DEBUG, 0, "gpi_clock_disable - AFTER uClock_DisableClock!\n");
      return GPI_STATUS_SUCCESS;
   }

   gpi_log(GPI_ERROR, 0, "gpi_clock_disable - ERROR - FAILED AFTER uClock_DisableClock!\n");
   return GPI_STATUS_ERROR;
}
#endif

void gpi_int_attach(GPI_DEV_HANDLE ** dev)
{
   DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, dev);
}

void gpi_dev_detach(GPI_DEV_HANDLE * dev)
{
   DAL_DeviceDetach(dev);
}

/**
 * Copies memory of a specofied size to the destination.
 *
 * @param[in] dst Pointer to the destination memory address
 * @param[in] src Pointer to the source memory address
 * @param[in] size Number of bytes to copy
 *
 * @return   None.
 */
void gpi_memcpy(void *dst, void *src, uint32 size)
{
   memcpy(dst, src, size);
}

/**
 *  Sets memory of specified size to specified value.
 *
 *  @param[in] mem Pointer to the block of memory to fill
 *  @param[in] val Value of byte to fill the memory with
 *  @param[in] size Number of bytes to fill
 *
 *  @return   None.
 */
void gpi_memset(void *mem, uint8 val, uint32 size)
{
   memset(mem, val, size);
}

/**
 * SDC EE needs to translate local to system addresses.
 * For SDC this function is defined in system driver.
 *
 * @param[in]   addr    Address to translate
 *
 * @return      Translated address.
 */
U_LONG get_system_address(U_LONG addr)
{
   return addr;
}

/**
 * SDC EE needs to translate system to local addresses.
 * For SDC this function is defined in system driver.
 *
 * @param[in]   addr    Address to translate
 *
 * @return      Translated address.
 */
U_LONG get_local_address(U_LONG addr)
{
   return addr;
}

/**
 * Performs a cache maintanence operation. 
 *
 * @param[in]  cache_op    Cache Operation to perform (clean/flush/invalidate)
 * @param[in]  address     Memory for which cache maintanence to be performed.
 * @param[in]  size        Size of the memory
 *
 * @return   None.
 */
void gpi_cachecmd(gpi_cache_op_type cache_op, U_LONG address, uint32 size)
{
   int status;

   switch (cache_op)
   {
      case GPI_CACHE_INVALIDATE:
         status = qurt_mem_cache_clean(address, size, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);
         break;
      case GPI_CACHE_FLUSH:
         status = qurt_mem_cache_clean(address, size, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
         break;
      case GPI_CACHE_CLEAN:
         status = qurt_mem_cache_clean(address, size, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);
         break;
      case GPI_CACHE_FLUSH_INV:
         status = qurt_mem_cache_clean(address, size, QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE);
         break;
      default:
         gpi_log(GPI_ERROR, 0, "cachecmd - ERR - default cache_op!\n");
         return;
   }

   if (QURT_EOK != status)
   {
      switch (status)
      {
         case QURT_EVAL:
            gpi_log(GPI_DEBUG, 0, "cachecmd - ERR - Invalid cache type!\n");
            break;
         case QURT_EALIGN:
            gpi_log(GPI_DEBUG, 0, "cachecmd - ERR - Aligned data or address fail!\n");
            break;
         default:
            gpi_log(GPI_ERROR, 0, "cachecmd - ERR - Unknown cache error!\n");
            return;
      }
   }
}

/**
 * Performs a processor specific memory barrier operation.
 *
 * @param    None
 *
 * @return   None.
 */
void gpi_memorybarrier(void)
{
   qurt_mem_barrier();
}

/**
 * @brief      Get physical address of a given virtual address
 *
 * @param[in]  buf - Virtual address
 *
 * @return     Physical address
 */
void* gpi_va_to_pa(void *buf)
{
#ifdef GPI_USES_PRAM
   return ((void *)pram_mgr_lookup_phys_addr_by_name("GPI", (qurt_addr_t)buf));
#else
   return ((void *)qurt_lookup_physaddr((qurt_addr_t)buf));
#endif
}

#ifdef GPI_USES_PRAM
void* gpi_acquire_partition(uint32 *size)
{
   return pram_acquire_partition("GPI", size);
}
#endif

/**
 * @brief Initializes a sync object.
 *
 * @param[in,out] sync     Pointer to the object to be used for sync
 *
 * @return Success of the operation.
*/
GPI_RETURN_STATUS gpi_syncinit(gpi_sync_type *sync)
{
   GPI_ASSERT(sync);

   if (FALSE == sync->lock_initialized)
   {
     qurt_mutex_init(&(sync->lock));
     sync->lock_initialized = TRUE;
   }

   return GPI_STATUS_SUCCESS;
}

/**
 * Destroys a synchronization object.
 *
 * @param[in,out]  sync    Sync object to be destroyed
 *
 * @return         None.
 */
GPI_RETURN_STATUS gpi_syncdestroy(gpi_sync_type *sync)
{
   GPI_ASSERT(sync && sync->lock_initialized);
   qurt_mutex_destroy(&(sync->lock));
   sync->lock_initialized = FALSE;

   return GPI_STATUS_SUCCESS;
}

/**
 * Enters into a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
void gpi_syncenter(gpi_sync_type *sync)
{
   GPI_ASSERT(sync && sync->lock_initialized);
   qurt_mutex_lock(&(sync->lock));
}

/**
 * Leaves a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
void gpi_syncleave(gpi_sync_type *sync)
{
   GPI_ASSERT(sync && sync->lock_initialized);
   qurt_mutex_unlock(&(sync->lock));
}

/**
 * @brief      Initializes an any-signal object
 *
 * @param[in]     handle - handle to event
 *
 * @return       None
 */
void gpi_eventInit(qurt_anysignal_t * hEvent)
{
   qurt_anysignal_init(hEvent);
}

/**
 * @brief      Wait for an event to be signalled
 *
 * @param[in]     handle - handle to event
 *
 * @return       None
 */
unsigned int gpi_eventwait(qurt_anysignal_t * hEvent)
{
   return qurt_anysignal_wait(hEvent, GPI_SIG_MASK);
}

/**
 * @brief      Signal event
 *
 * @param[in]     handle
 *
 * @return        None
 */
void gpi_eventsignal(qurt_anysignal_t * hEvent)
{
   qurt_anysignal_set(hEvent, GPI_SIG_MASK);
}

/**
 * @brief      Signal event
 *
 * @param[in]     handle
 *
 * @return        None
 */
void gpi_eventreset(qurt_anysignal_t * hEvent)
{
   qurt_anysignal_clear(hEvent, GPI_SIG_MASK);
}

/**
 * @brief      Spawn a worker thread
 *
 * @param[in]  *obj   - Thread object
 * @param[in]  *name  - Name of thread
 * @param[in]  *entry - Entry point function
 * @param[in]  *ctx   - Context to pass to entry
 * @param[in]  priority - Priority of thread
 *
 * @return        None
 */
GPI_RETURN_STATUS gpi_spawn_thread(generic_thread_type *thread_obj,
                                   char *name, void *entry, void *ctx, uint32 priority)
{
   qurt_thread_attr_t tattr;
   unsigned int stackbase;

   stackbase = (unsigned int)&gpi_task_stack_arr;

   qurt_thread_attr_init(&tattr);
   qurt_thread_attr_set_stack_size(&tattr, (GPI_TASK_STACK_SIZE - 8));
   qurt_thread_attr_set_stack_addr(&tattr, (void *)((stackbase + 7) & (~7)));
   qurt_thread_attr_set_priority(&tattr, priority - 1);
#ifdef GPI_USES_PRAM
   qurt_thread_attr_set_tcb_partition(&tattr, 1); // This task should reside in TCM Memory
#endif
   qurt_thread_attr_set_name(&tattr, name);

   (void)qurt_thread_create(&thread_obj->id, &tattr, entry, ctx);

   return GPI_STATUS_SUCCESS;
}

/**
 * @brief Signal a worker thread or task
 *
 * @param[in,out ]     gpi_ctxt           GPI context 
 *
 * @return       None.
 *
 */
void gpi_signal_worker(gpi_ctxt *gpi)
{
   gpi_eventsignal(&gpi->gpi_thread.thread.hEvent);
}

#ifdef GPI_SSC
/**
 * Enable interrupt
 *
 * @param[in]      handle       Interrupt handle
 * @param[in]      irq          IRQ ID to be registered for
 *
 * @return   Success of the operation. 
 */
GPI_RETURN_STATUS gpi_isr_enable(GPI_DEV_HANDLE *handle, uint32 irq)
{
      if (UINTERRUPT_SUCCESS != uInterruptController_Enable(irq, TRUE))
      {
         gpi_log(GPI_ERROR, 0, "isr_enable - ERR - uInterruptController_Enable failed!\n");
         return GPI_STATUS_ERROR;
      }

   return GPI_STATUS_SUCCESS;
}

/**
 * Disable interrupt
 *
 * @param[in]      handle       Interrupt handle
 * @param[in]      irq          IRQ ID to be registered for
 *
 * @return   Success of the operation. 
 */
GPI_RETURN_STATUS gpi_isr_disable(GPI_DEV_HANDLE *handle, uint32 irq)
{
      if (DAL_SUCCESS != uInterruptController_Enable(irq, FALSE))
      {
         gpi_log(GPI_ERROR, 0, "isr_disable - ERR - uInterruptController_Enable failed!\n");
         return GPI_STATUS_ERROR;
      }

   return GPI_STATUS_SUCCESS;
}
#endif

/**
 * Registers with the interrupt controller, an ISR to service an IRQ id.
 *
 * @param[in]      irq          IRQ ID to be registered for
 * @param[in,out]  isr          ISR to be invoked to the service the irq
 * @param[in,out]  data         data to be supplied to the isr upon invocation
 *
 * @return   Success of the operation. 
 */
GPI_RETURN_STATUS gpi_isrinstall(uint32 irq, isr_fct isr, void *data, GPI_DEV_HANDLE * handle)
{
   if (DAL_SUCCESS != DalInterruptController_RegisterISR(handle,
                                                         irq,
                                                         (DALISR)isr,
                                                         (DALISRCtx)data,
#ifdef GPI_USES_PRAM
                                                         DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, 0, 1)))
#else
                                                         DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER))
#endif
   {
         gpi_log(GPI_ERROR, 0, "isrinstall - ERR - Register ISR failed!\n");
      return GPI_STATUS_ERROR;
   }

   return GPI_STATUS_SUCCESS;
}

/**
  Sleep function.

  @param[in]  wait    Sleep amount (usec for Q6)

  @return  None.
*/
void gpi_sleep(uint32 wait)
{
   qurt_timer_sleep(wait);
}

/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return  None.
*/
#ifdef GPI_DEBUGGING
void gpi_log(gpi_msg_type msgLevel, uint32 arg_count, const char *fmt, ...)
{
   va_list args;

   if (msgLevel > GPI_MSG_LEVEL)
      return;

   va_start(args, fmt);

#ifdef GPI_USES_PRAM
   if (qurt_island_get_status())
   {
#ifdef ENABLE_GPI_MICRO_ULOG
      micro_ULog_RealTimeVprintf(gpi_uulog_handle, arg_count, fmt, args);
#endif
   }
   else
#endif
   {
      ULogFront_RealTimeVprintf(gpi_ulog_handle, arg_count, fmt, args);
   }

   va_end(args);
}
#endif

