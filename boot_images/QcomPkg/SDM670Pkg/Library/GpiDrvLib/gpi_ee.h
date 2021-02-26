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
08/07/17   pc      Updated as per latest GPI core driver
01/31/17   ts      file ported to UEFI.
02/19/16   ah      Created

===============================================================================
                   Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "string.h"
#include "gpitgtcfgdata.h"

/*
 * static defines in header file for building UEFI 
 */
#define __UEFI_ENV__
#define GPI_XML_PROPS
#define GPI_MMU
#undef GPI_EE_DEBUG

#ifdef __UEFI_ENV__
 typedef UINT8  uint8;
 typedef UINT16 uint16;
 typedef UINT32 uint32;
 typedef UINT64 uint64;
#include <DebugLib.h>
#include <Library/CacheMaintenanceLib.h>

#endif /* __UEFI_ENV__ */

#ifndef GPI_EE_DEBUG
#include <stdio.h>
#endif

#include "gpi.h"
#include "gsihwio.h"


#if (defined GPI_DALSYS || defined GPI_XML_PROPS)
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIClock.h"
#endif

#ifdef MAPPED_HWIO
#include "DDIHWIO.h"
#endif

#ifdef GPI_EE_DEBUG
#include "err.h"
#include "ULogFront.h"
/* Ulog buffer size */
#define GPI_ULOG_BUFFER_MAX       4096*10
static void gpi_osal_uloginit(void);
#endif


#define MAX_GPI_CONFIG  0xD

#define GPI_ASSERT(x)

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
   uint32 none;
}gpi_sync_type;

/** @name Spinlock */
typedef gpi_sync_type gpi_spinlock_type;

typedef struct
{
   uint32 clock_id;
}gpi_clock_ctl_type;

/** @name Cache flags
 *  @brief Flags used for various cache operations
 */
typedef enum
{
    GPI_CACHE_INVALIDATE = 0x0,  /* Invalidates a cache */
    GPI_CACHE_FLUSH      = 0x1,  /* Flush a cache       */
    GPI_CACHE_CLEAN      = 0x2   /* Clean a cache       */
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

typedef struct gpi_ctxt gpi_ctxt;

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
 * Registers with the interrupt controller, an ISR to service an IRQ id.
 *
 * @param[in,out]  irqhandle    Handle to be used with the interrupt controller
 * @param[in]      irq          IRQ ID to be registered for
 * @param[in,out]  isr          ISR to be invoked to the service the irq
 * @param[in,out]  data         data to be supplied to the isr upon invocation
 *
 * @return   Success of the operation.
 */
#define gpi_isrinstall(...) GPI_STATUS_SUCCESS

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
void gpi_cachecmd(gpi_cache_op_type cache_op, U_LONG address, uint32 size);
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

/**
 * Deregisters the ISR from the interrupt controller.
 *
 * @param[in,out]  irqhandle    Interrupt controller handle to be used
 *
 * @return   None.
 */
void gpi_isruninstall(uint32 irq);

/**
 * @brief      Check with the interrupt controller if the supplied IRQ is set
 *
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @return     TRUE - Interrupt is set
 *             FALSE - Interrpt is not set
 */
boolean gpi_is_irq_set(uint32 irq);

/**
 * @brief      Enable IRQ
 *
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @return     GPI_STATUS_SUCCESS/GPI_STATUS_ERROR
 */
GPI_RETURN_STATUS gpi_enable_irq(uint32 irq);

/**
 * @brief      Disable IRQ
 *
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @return     GPI_STATUS_SUCCESS/GPI_STATUS_ERROR
 */
GPI_RETURN_STATUS gpi_disable_irq(uint32 irq);

/**
 * @brief      Mask Interrupt
 *
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @return     None
 */
void gpi_isrmask(uint32 irq);

/**
 * @brief      Unmask interrupt
 *
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @return     None
 */
void gpi_isrunmask(uint32 irq);

/**
 * Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
 * to re-activate the IRQ.
 *
 * @param[in,out]  irqhandle    Interrupt handle
 *
 * @return   None.
 */
void gpi_israck(uint32 irq);

#ifndef GPI_SDC
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
#endif

#define gpi_clock_ctl_init(...)

#define gpi_clock_ctl_enable(...)

#define gpi_clock_ctl_disable(...)

#define gpi_clock_ctl_deinit(...)

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
#define gpi_syncinit(...)  GPI_STATUS_SUCCESS

/**
 * Destroys a synchronization object.
 *
 * @param[in,out]  sync    Sync object to be destroyed
 *
 * @return         None.
 */
#define gpi_syncdestroy(...)  GPI_STATUS_SUCCESS

/**
 * Enters into a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
#define gpi_syncenter(...)

/**
 * Leaves a synchronized context.
 *
 * @param[in,out]  sync    Sync object to be used
 *
 * @return         None.
 */
#define gpi_syncleave(...)

/**
 * @brief      Wait for an event to be signalled
 *
 * @param[in]     handle - handle to event
 *
 * @return       None
 */
#define gpi_eventwait(...)

/**
 * @brief      Signal event
 *
 * @param[in]     handle
 *
 * @return        None
 */
#define gpi_eventsignal(...)

/**
 * @brief      Reset event
 *
 * @param[in]     handle
 *
 * @return        None
 */
#define gpi_eventreset(...)

/**
 * @brief      Signal task
 *
 * @param[in]     handle
 *
 * @return        None
 */
#define gpi_signal_task(...)

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
#define gpi_spawn_thread(...)  GPI_STATUS_SUCCESS

/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return  None.
*/
#ifdef GPI_EE_DEBUG
void gpi_log(gpi_msg_type msgLevel, const char *fmt, ...);
#else
#ifdef __UEFI_ENV__
  #define gpi_log(log_level, fmt, ...) \
    do { \
      UINT32 uefi_log_level = (log_level == GPI_ERROR)?EFI_D_ERROR:(log_level == GPI_WARNING)?EFI_D_WARN:(log_level == GPI_INFO)?EFI_D_INFO:EFI_D_VERBOSE; \
      \
      DEBUG(( uefi_log_level, fmt, ##__VA_ARGS__)); \
    } while(0)
#else
  #define gpi_log(...)
#endif
#endif

/**
  Sleep function.

  @param[in]  wait    Sleep amount in usec

  @return  None.
*/
void gpi_sleep(uint32 wait);

#endif /* #define __GPI_EE_H__ */
