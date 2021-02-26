#ifndef __GPI_EE_H__
#define __GPI_EE_H__

/**
  @file gpi_ee.h
  @brief
  This file contains implementation of interfaces that provide a thin
  abstraction to various OS-specific APIs used by the GPI driver.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/17   ah      Clock APIs.
10/06/17   ah      Updated logging.
05/15/17   ah      QURT changes.
05/05/17   ah      Added ulog function declarations.
03/08/17   ah      General cleanup
02/19/16   ah      Created

===============================================================================
                   Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "string.h"
#include "gpi.h"
#include "gsihwio.h"
#include "gpitgtcfgdata.h"
#include "qurt_memory.h"
#include "qurt_timer.h"
#include "qurt_mutex.h"
#include "qurt_thread.h"
#include "err.h"
#include "DDIInterruptController.h"
#include "DALDeviceId.h"
#ifdef GPI_USES_PRAM
#include "qurt_island.h"
#include "pram_mgr.h"
#endif
#ifdef GPI_SSC
#include "uClock.h"
#include "uInterruptController.h"
#endif

#include "DALFramework.h"

#ifdef GPI_DEBUGGING
#include "err.h"
#include "ULogFront.h"
#define GPI_ULOG_BUFFER_MAX       0x6000
#ifdef ENABLE_GPI_MICRO_ULOG
#include "micro_ULog.h"
#define GPI_UULOG_BUFFER_SIZE     0x800
#endif
#endif

#define GPI_TASK_PRI            31
#define GPI_TASK_STACK_SIZE     2048
#define GPI_SIG_MASK            (0x1)

#define GPI_ASSERT(condition) \
    if( !(condition) ) \
    { \
      ERR_FATAL( "gpi " #condition, 0, 0, 0 ); \
    }

/** @name Message Type */
typedef enum
{
   GPI_CRITICAL,   /* Unexpected unrecoverable error (e.g. hard disk error)               */
   GPI_ERROR,      /* Unexpected recoverable error (e.g. needed to reset a HW controller) */
   GPI_WARNING,    /* Expected error (e.g. parity error on a serial port)                 */
   GPI_NOTICE,     /* Warnings (e.g. out of paper)                                        */
   GPI_INFO,       /* Information (e.g. printing page 3)                                  */
   GPI_DEBUG,      /* Debug messages                                                      */

   GPI_MAX_DEBUG   /* Show all messages                                                   */
}gpi_msg_type;

/** @name Sync object */
typedef struct
{
  qurt_mutex_t lock;
  boolean lock_initialized;
} gpi_sync_type;

/** @name Spinlock */
typedef gpi_sync_type gpi_spinlock_type;

/** @name Thread Object */
typedef struct
{
   qurt_thread_t     id;
   qurt_anysignal_t  hEvent;
}generic_thread_type;

/** @name Cache flags
 *  @brief Flags used for various cache operations
 */
typedef enum
{
    GPI_CACHE_INVALIDATE = 0x0,  /* Invalidates a cache */
    GPI_CACHE_FLUSH      = 0x1,  /* Flush a cache       */
    GPI_CACHE_CLEAN      = 0x2,  /* Clean a cache       */
    GPI_CACHE_FLUSH_INV  = 0x3   /* Flush + Invalidate  */
}gpi_cache_op_type;

/** @name Mapping flags
 *  @brief Flags used for device and memory mapping
 */
typedef enum
{
   GPI_DEVICE_MAPPING = 0x0, /* Mapps GPI device space */
   GPI_MEMORY_MAPPING = 0x1  /* Maps DDR Memory space  */
}gpi_mapping_op_type;


#define GPI_DEV_HANDLE DalDeviceHandle
/*************************************************************************/
/************************   Function Prototypes   ************************/
/*************************************************************************/

void gpi_int_attach(GPI_DEV_HANDLE **dev);
void gpi_dev_detach(GPI_DEV_HANDLE *dev);

#ifdef GPI_DEBUGGING
/**
 * Ulog message logging interface.
 * Allocates buffer for logging.On successful initialization it will return the Ulog handle.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_ulog_init(void);

/**
 * Ulog message logging interface De-initialization.
 *
 * @param    None.
 *
 * @return   None.
 */
void gpi_ulog_deinit(void);

#ifdef ENABLE_GPI_MICRO_ULOG
void gpi_uulog_init(void);
#endif
#endif

#ifdef GPI_SSC
GPI_RETURN_STATUS gpi_clock_enable(void);
GPI_RETURN_STATUS gpi_clock_disable(void);
uint32 gpi_clocks_on(void);
#else
#define gpi_clock_enable(...)  GPI_STATUS_SUCCESS
#define gpi_clock_disable(...)  GPI_STATUS_SUCCESS
#define gpi_clocks_on(...)  1
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
void gpi_memcpy(void *dst, void *src, uint32 size);

/**
 *  Sets memory of specified size to specified value.
 *
 *  @param[in] mem Pointer to the block of memory to fill
 *  @param[in] val Value of byte to fill the memory with
 *  @param[in] size Number of bytes to fill
 *
 *  @return   None.
 */
void gpi_memset(void *mem, uint8 val, uint32 size);

/**
 * Performs a processor specific memory barrier operation.
 *
 * @param    None
 *
 * @return   None.
 */
void gpi_memorybarrier(void);

/**
 * Performs a cache maintanence operation.
 *
 * @param[in]  cache_op    Cache Operation to perform (clean/flush/invalidate)
 * @param[in]  address     Memory for which cache maintanence to be performed.
 * @param[in]  size        Size of the memory
 *
 * @return   None.
 */
void gpi_cachecmd(gpi_cache_op_type cache_op, U_LONG address, uint32 size);

#ifdef GPI_SSC
/**
 * Enable interrupt
 *
 * @param[in]      handle       Interrupt handle
 * @param[in]      irq          IRQ ID to be registered for
 *
 * @return   Success of the operation. 
 */
GPI_RETURN_STATUS gpi_isr_enable(GPI_DEV_HANDLE *handle, uint32 irq);

/**
 * Disable interrupt
 *
 * @param[in]      handle       Interrupt handle
 * @param[in]      irq          IRQ ID to be registered for
 *
 * @return   Success of the operation. 
 */
GPI_RETURN_STATUS gpi_isr_disable(GPI_DEV_HANDLE *handle, uint32 irq);
#else
#define gpi_isr_enable(...)  GPI_STATUS_SUCCESS
#define gpi_isr_disable(...)  GPI_STATUS_SUCCESS
#endif

/**
 * Registers with the interrupt controller, an ISR to service an IRQ id.
 *
 * @param[in,out]  irqhandle    Handle to be used with the interrupt controller
 * @param[in]      irq          IRQ ID to be registered for
 * @param[in,out]  isr          ISR to be invoked to the service the irq
 * @param[in,out]  data         data to be supplied to the isr upon invocation
 *
 * @return   Success of the operation.
 */
typedef void (*isr_fct) (void*);
GPI_RETURN_STATUS gpi_isrinstall(uint32 irq, isr_fct isr, void *data, GPI_DEV_HANDLE * handle);

/**
 * SDC EE needs to translate local to system addresses.
 * For SDC this function is defined in system driver.
 *
 * @param[in]   addr    Address to translate
 *
 * @return      Translated address.
 */
U_LONG get_system_address(U_LONG addr);

/**
 * SDC EE needs to translate system to local addresses.
 * For SDC this function is defined in system driver.
 *
 * @param[in]   addr    Address to translate
 *
 * @return      Translated address.
 */
U_LONG get_local_address(U_LONG addr);

/**
 * @brief      Get physical address of a given virtual address
 *
 * @param[in]  buf - Virtual address
 *
 * @return     Physical address
 */
void* gpi_va_to_pa(void *buf);

/**
 * @brief Initializes a sync object.
 *
 * @param[in,out] sync     Pointer to the object to be used for sync
 *
 * @return Success of the operation.
*/
GPI_RETURN_STATUS gpi_syncinit(gpi_sync_type *sync);

/**
 * Destroys a synchronization object.
 *
 * @param[in,out]  sync    Sync object to be destroyed
 *
 * @return         None.
 */
GPI_RETURN_STATUS gpi_syncdestroy(gpi_sync_type *sync);

/**
 * Enters into a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
void gpi_syncenter(gpi_sync_type *sync);

/**
 * Leaves a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
void gpi_syncleave(gpi_sync_type *sync);

/**
 * @brief      initialize an event
 *
 * @param[in]     handle - handle to event
 *
 * @return       None
 */
void gpi_eventInit(qurt_anysignal_t * hEvent);

/**
 * @brief      Wait for an event to be signalled
 *
 * @param[in]     handle - handle to event
 *
 * @return       None
 */
unsigned int gpi_eventwait(qurt_anysignal_t * hEvent);

/**
 * @brief      Signal event
 *
 * @param[in]     handle
 *
 * @return        None
 */
void gpi_eventsignal(qurt_anysignal_t * hEvent);

/**
 * @brief      Reset event
 *
 * @param[in]     handle
 *
 * @return        None
 */
void gpi_eventreset(qurt_anysignal_t * hEvent);

/**
 * @brief      Signal task
 *
 * @param[in]     handle
 *
 * @return        None
 */
void gpi_signal_task(void);

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
GPI_RETURN_STATUS gpi_spawn_thread(generic_thread_type *obj,
                                   char *name, void *entry, void *ctx, uint32 priority);

/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return  None.
*/

#ifdef GPI_DEBUGGING
void gpi_log(gpi_msg_type msgLevel, uint32 data_count, const char *fmt, ...);
#else
#define gpi_log(...)
#endif

#ifdef GPI_USES_PRAM
void * gpi_acquire_partition(uint32 *size);
#else
#define gpi_acquire_partition(...)
#endif

/**
  Sleep function.

  @param[in]  wait    Sleep amount in usec

  @return  None.
*/
void gpi_sleep(uint32 wait);

#endif /* #define __GPI_EE_H__ */
