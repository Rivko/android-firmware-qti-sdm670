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
08/07/17   pc      Updated as per latest GPI core driver
01/31/17   ts      file ported to UEFI.
02/19/16   ah      Created

===============================================================================
                   Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "gpi_utils.h"
#include "gpitgtcfgdata.h"


/* ULog */
#ifdef GPI_DEBUG
ULogHandle gpiulog;                                    /* ULog global handle */
ULogResult gpi_ulog_enable = ULOG_ERR_INITINCOMPLETE;  /* Ulog enabled flag */

static void gpi_uloginit(void);

/**
 * Ulog message logging interface.
 * Allocates buffer for logging.On successful initialization it will return the Ulog handle.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_uloginit(void)
{
   gpi_log(GPI_DEBUG, "gpi_uloginit() <-\n");

   /* Ulog Handle and buffer intialization. */
   if (gpi_ulog_enable == ULOG_ERR_INITINCOMPLETE)
   {
      gpi_ulog_enable = ULogFront_RealTimeInit(&gpiulog, "GPI",
                                               GPI_ULOG_BUFFER_MAX,
                                               ULOG_MEMORY_LOCAL,
                                               ULOG_LOCK_NONE);
   }
   gpi_log(GPI_DEBUG, "gpi_uloginit() ->\n");
}

/**
 * Ulog message logging interface De-initialization.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_ulogdeinit(void)
{
   gpi_log(GPI_DEBUG, "gpi_ulogdeinit() <-\n");
   gpi_ulog_enable = ULOG_ERR_INITINCOMPLETE;
   gpi_log(GPI_DEBUG, "gpi_ulogdeinit() ->\n");
}
#endif

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

#ifdef GPI_MMU
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
   gpi_log(GPI_DEBUG, "gpi_cachecmd() <-\n");

#if (defined __UEFI_ENV__)
   WriteBackInvalidateDataCacheRange((void *)address, size);
#else
   gpi_log(GPI_DEBUG, "cachecmd not available in this environment!\n");
#endif
   gpi_log(GPI_DEBUG, "gpi_cachecmd() ->\n");
}
#endif

/**
 * Performs a processor specific memory barrier operation.
 *
 * @param    None
 *
 * @return   None.
 */
void gpi_memorybarrier(void)
{
#if GPI_BOOT
   __asm__ __volatile__( "barrier\n" );
#else
   gpi_log(GPI_DEBUG, "memorybarrier not available in this environment!\n");
#endif
}

/**
 * @brief      Enable IRQ
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return     GPI_STATUS_SUCCESS/GPI_STATUS_ERROR
 */
GPI_RETURN_STATUS gpi_enable_irq(uint32 irq)
{
   gpi_log(GPI_DEBUG, "gpi_enable_irq() <-\n");

   gpi_log(GPI_DEBUG, "enable_irq not available in this environment!\n");

   gpi_log(GPI_DEBUG, "gpi_enable_irq() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * @brief      Disable IRQ
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return     GPI_STATUS_SUCCESS/GPI_STATUS_ERROR
 */
GPI_RETURN_STATUS gpi_disable_irq(uint32 irq)
{
   gpi_log(GPI_DEBUG, "gpi_disable_irq() <-\n");

   gpi_log(GPI_DEBUG, "disable_irq not available in this environment!\n");

   gpi_log(GPI_DEBUG, "gpi_disable_irq() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * @brief      Mask Interrupt
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return     None
 */
void gpi_isrmask(uint32 irq)
{
   gpi_log(GPI_DEBUG, "isrmask not available in this environment!\n");
}

/**
 * @brief      Unmask interrupt
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return     None
 */
void gpi_isrunmask(uint32 irq)
{
   gpi_log(GPI_DEBUG, "isrunmask not available in this environment!\n");
}

/**
 * Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
 * to re-activate the IRQ.
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return   None.
 */
void gpi_israck(uint32 irq)
{
   gpi_log(GPI_DEBUG, "israck not available in this environment!\n");
}

/**
 * @brief      Check with the interrupt controller if the supplied IRQ is set
 *
 * @param[in,out]  irq    The specific GPII irq
 *
 * @return     TRUE - Interrupt is set
 *             FALSE - Interrpt is not set
 */
boolean gpi_is_irq_set(uint32 irq)
{
    return FALSE;
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
   return buf;
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
   if (gpi->exec_mode == GPI_TASK)
   {
      /* Schedule GPI main task */
      gpi_signal_task();
   }
   else
   {
      /* Trigger GPI worker thread */
      gpi_eventsignal(gpi->gpi_thread.thread.hEvent);
   }
}

/**
  Sleep function.

  @param[in]  wait    Sleep amount (usec for Q6)

  @return  None.
*/
void gpi_sleep(uint32 wait)
{
   gpi_log(GPI_ERROR, "ERROR - sleep not supported!\n");
}

/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return  None.
*/
#ifdef GPI_DEBUG
void gpi_log(gpi_msg_type msgLevel, const char *fmt, ...)
{
   va_list arglist;
   const char *p = fmt;
   uint32 datacount = 0;

   if (msgLevel < GPI_MSG_LEVEL)
      return;

   va_start(arglist, fmt);

   va_end(arglist);
}
#endif

