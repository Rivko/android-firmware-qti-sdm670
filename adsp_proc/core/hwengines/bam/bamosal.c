/**
  @file bamosal.c
  @brief
  This file contains implementation of interfaces that provide a thin
  abstraction to various OS-specific APIs used by the BAM driver.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.qdsp6/2.1/hwengines/bam/bamosal.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/12/15   SS      Optimize for 64bit suppport
12/04/12   SS      Added devcfg props support for tgtcfg
12/04/12   AN      virtual address in bam_osal_mallocmemregion is adjusted if the mapped physical address 
                   does not match the request one exactly
12/05/12   SS      Added devcfg props support for tgtcfg
08/31/12   PR      bam_osal_mallocmemregion is  modified to remove the signature write if the physical memory address is already passed.
06/19/12   SS      Qmem macros deprication clean up
05/21/12   SS      Added Ulog support.
04/12/12   NK      New funciton for destroying mutexes plus osal deinit support
03/15/12   MK      Added QURT support for MBA
03/31/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "com_dtypes.h"
#include "bamosal.h"
#include "bam.h"
#include "bamtgtcfg.h"

#ifdef BAM_TZOS
#include "tzbsp_log.h"
#include "tzbsp_mem.h"
extern void memory_barrier(void);
#endif

#ifdef BAM_OSAL_DEBUG
#include "stdio.h"
#endif

#ifdef BAM_MBA
#include <qurt.h>
#include <qube.h>
#else
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIClock.h"
#endif

#if !defined(BAM_TZOS) && !defined(BAM_MBA)
#include "ULogFront.h"
#define	BAM_ULOG_BUFFER_MAX		4096
#endif
#ifndef BAM_MBA
DalDeviceHandle* hBamOsalDALInterrupt = NULL;
DalDeviceHandle* hBamOsalDALClock = NULL;
#endif
#ifdef BAM_XML_PROPS 
DALSYS_PROPERTY_HANDLE_DECLARE(hPropBAM);
#endif

uint32 bam_osal_init_done = 0;
extern bam_target_config_type* bam_tgt_config_ptr;

#if !defined(BAM_TZOS) && !defined(BAM_MBA)
/*  ULog global handle */
ULogHandle	bamulog;
uint32	bam_osal_ulog_enable = 0;
static void bam_osal_uloginit(void);


/* ============================================================================
**  Function : bam_osal_ulogInit
** ============================================================================
*/
/**
  Ulog messsgae logging interface intialiization.

  Ulog message logging interface.Allocates buffer for logging.On successful intialization it will return the
  Ulog Bam handle.

  @return
  bam status(Success or failure)
*/
void bam_osal_uloginit(void)
{
    ULogResult status = ULOG_ERR_INITINCOMPLETE;;
	 /* Ulog Handle and buffer intialization. */
    if(!bam_osal_ulog_enable)
    {
        status = ULogFront_RealTimeInit(&bamulog, "BAM", BAM_ULOG_BUFFER_MAX, ULOG_MEMORY_LOCAL, ULOG_LOCK_NONE);
    }
	 if(!status)
	 {
        bam_osal_ulog_enable = 1;
	 }
	 return;
}

/* ============================================================================
**  Function : bam_osal_ulogdeinit
** ============================================================================
*/
/**
  Ulog messsgae logging interface Deintialiization.

  @param   None.
  @return
  None.

*/
void bam_osal_ulogdeinit(void)
{
    if(bam_osal_ulog_enable)
	 {
		 bam_osal_ulog_enable = 0;
	 }
	return;
}

#endif

/* ============================================================================
**  Function : bam_osal_init
** ============================================================================
*/
/**
  Initializes OSAL Interfaces.

  Initializes OSAL Interfaces. Performs any intiialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void bam_osal_init(void)
{
#ifdef BAM_XML_PROPS
    DALSYSPropertyVar bamprop;
	 DALResult status = DAL_SUCCESS;
#endif
    if(bam_osal_init_done)
    {
        return;
    }
#ifndef BAM_MBA
    DALSYS_InitMod(NULL);
    DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                     &hBamOsalDALInterrupt);
    DAL_DeviceAttach(DALDEVICEID_CLOCK, &hBamOsalDALClock);
#endif // #ifndef BAM_MBA

#ifdef BAM_XML_PROPS
    status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/bam", hPropBAM);
    if(status != DAL_SUCCESS) 
        return;
    if((status = DALSYS_GetPropertyValue(hPropBAM, "bam_tgt_config", 0, &bamprop)) != DAL_SUCCESS)
        return;

    bam_tgt_config_ptr = (bam_target_config_type*)bamprop.Val.pStruct;
#endif /* BAM_XML_PROPS */

#if !defined(BAM_TZOS) && !defined(BAM_MBA)
	 bam_osal_uloginit();
#endif
    bam_osal_init_done = 1;

}

/* ============================================================================
**  Function : bam_osal_deinit
** ============================================================================
*/
/**
  DeInitializes OSAL Interfaces.

  Deinitializes OSAL Interfaces. Performs any deinitialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void bam_osal_deinit(void)
{

#if !defined(BAM_TZOS) && !defined(BAM_MBA)
    bam_osal_ulogdeinit();
#endif

    if (!bam_osal_init_done)
    {
        return;
    }
    bam_osal_init_done = 0;

    #ifndef BAM_MBA
    DAL_DeviceDetach(hBamOsalDALInterrupt);
    hBamOsalDALInterrupt=NULL;
    DAL_DeviceDetach(hBamOsalDALClock);
    hBamOsalDALClock = NULL;
    DALSYS_DeInitMod();
    #endif // #ifndef BAM_MBA
}

/* ============================================================================
**  Function : bam_osal_malloc
** ============================================================================
*/
/**
  Mallocs a memory (virtual) using OS Malloc API.

  Mallocs a memory (virtual) using OS Malloc API. The memory allocated
  is for SW usage only.

  @param[in]  size    Size of the memory to be allocated

  @return
  Pointer to the memory allocated. NULL if failed.

  @sa
  bam_osal_mallocmemregion, bam_osal_free
*/
void * bam_osal_malloc(uint32 size)
{
    void *ptr;
#if defined(BAM_MBA)
    ptr = qurt_malloc(size);
#elif defined(BAM_TZOS)
    ptr = tzbsp_malloc(size);
#else
    DALResult res = DALSYS_Malloc(size, &ptr);

    if (res != DAL_SUCCESS)
    {
        return NULL;
    }
#endif
    return ptr;
}

/* ============================================================================
**  Function : bam_osal_free
** ============================================================================
*/
/**
  Frees the memory allocated by bam_osal_malloc.

  Frees the memory (virtual) using OS Free API.

  @param[in,out]  mem    Pointer to the memory to be freed

  @return
  None.

  @sa
  bam_osal_malloc.
*/
void bam_osal_free(void *pmem)
{
#if defined(BAM_MBA)
    qurt_free(pmem);
#elif defined(BAM_TZOS)
    tzbsp_free(pmem);
#else
    DALSYS_Free(pmem);
#endif
}

/* ============================================================================
**  Function : bam_osal_memcpy
** ============================================================================
*/
/**
  Copies data in memory.

  Copies data in memory.

  @param[out]  dst    Pointer to the destination memory
  @param[in]   src    Pointer to the source memory
  @param[in]   size   size of data to be copied

  @return
  None.

  @sa
  bam_osal_malloc.
*/
void bam_osal_memcpy(void *dst, void* src, uint32 size)
{
#ifdef BAM_MBA
    memcpy(dst, src, size);
#else
    DALSYS_memcpy(dst, src, size);
#endif
}

/* ============================================================================
**  Function : bam_osal_mallocmemregion
** ============================================================================
*/
/**
  Allocates a physically contiguous memory location using OS specific API.

  Allocates a physically contiguous memory location using OS specific API. The
  memory allocated is expected to be share-able with the hw.

  @param[out]  meminfo    Info structure for the memory allocated.
  @param[in]      size    Size of the memory to be allocated.

  @return
  None.

  @sa
  bam_osal_freememregion.
*/
BAM_API_NON_PAGED void bam_osal_mallocmemregion(bam_osal_meminfo *meminfo, uint32 size, bam_mapping_op_type map_op)
{
   
#if (defined(BAM_MBA) || defined(BAM_TZOS))
    if (meminfo == NULL)
    {
        return;
    }
    meminfo->handle = NULL;
    meminfo->size = size;
    if(meminfo->pa == 0x0)
    {
        //In MBA VA:PA::1:1
        meminfo->handle = (void *)0xCAFEBABE;
#if defined(BAM_MBA)
        meminfo->va = meminfo->pa = (uint32)qurt_malloc(size);
#else
        meminfo->va = meminfo->pa = (uint32)tzbsp_malloc(size);
#endif
    }
    else
    {
        meminfo->handle = (void *)0xDEADDEAD; /*Indicate we didnot allocate, we just map va=pa */
        meminfo->va = meminfo->pa;
    }
#else
    DALSYSMemInfo dalmeminfo;
    uint32 attrb;
    switch (map_op) 
    {
    case BAM_DEVICE_MAPPING:
         attrb=DALSYS_MEM_PROPS_HWIO|DALSYS_MEM_PROPS_PHYS_CONT;
         break;
    case BAM_MEMORY_MAPPING:
         attrb=DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT;
         break;
    default:
         meminfo->handle = NULL;
         return;
    }
    if (meminfo == NULL)
    {
        return;
    }

    if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(attrb,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              meminfo->pa ? meminfo->pa : DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              size,
                                              &meminfo->handle,
                                              NULL) )
    {
        meminfo->handle = NULL;
        return;
    }

    DALSYS_MemInfo(meminfo->handle, &dalmeminfo);

    if (meminfo->pa != 0)
    {
        // check that requested region lies within the allocated space
        if (meminfo->pa < dalmeminfo.PhysicalAddr ||
            meminfo->pa + size > dalmeminfo.PhysicalAddr + dalmeminfo.dwLen)
        {
            DALSYS_DestroyObject(meminfo->handle);
            meminfo->handle = NULL;
            return;
        }

        // allocated region may not start right at the requested physical address
        // so the virtual address has to be adjusted accordingly
        meminfo->va = dalmeminfo.VirtualAddr + meminfo->pa - dalmeminfo.PhysicalAddr;
    }
    else
    {
        meminfo->va = dalmeminfo.VirtualAddr;
        meminfo->pa = dalmeminfo.PhysicalAddr;
    }

    meminfo->size = dalmeminfo.dwLen;
#endif
    return;
}

/* ============================================================================
**  Function : bam_osal_freememregion
** ============================================================================
*/
/**
  Frees the memory allocated using bam_osal_mallocmemregion.

  Frees the memory allocated using bam_osal_mallocmemregion.

  @param[in]  meminfo    Info structure for the memory region to be freed.

  @return
  None.

  @sa
  bam_osal_mallocmemregion.
*/
BAM_API_NON_PAGED void bam_osal_freememregion(bam_osal_meminfo *meminfo)
{
#if (defined(BAM_MBA) || defined(BAM_TZOS))
    if (meminfo != NULL)
    {
        if(meminfo->handle == (void *)0xCAFEBABE)
        {
            #if defined(BAM_MBA)
            qurt_free((void *)meminfo->va);
            #else
            tzbsp_free((void *)meminfo->pa);
            #endif
            meminfo->handle = 0;
        }
        if(meminfo->handle == (void *)0xDEADDEAD)
        {
            meminfo->handle = 0;
        }
    }

#else
    if (meminfo != NULL && meminfo->handle != NULL)
    {
        DALSYS_DestroyObject(meminfo->handle);
    }
#endif // BAM_MBA || BAM_TZOS
}

/* ============================================================================
**  Function : bam_osal_cachecmd
** ============================================================================
*/
/**
  Performs a cache maintanence operation.

  Performs a cache maintanence operation.

  @param[in]  cache_op    Cache Operation to perform (clean/flush/invalidate)
  @param[in]  address     Memory for which cache maintanence to be performed.
  @param[in]  size        Size of the memory

  @return
  None.

*/
BAM_API_NON_PAGED void bam_osal_cachecmd(bam_cache_op_type cache_op,uint32 address, uint32 size)
{
#if defined(BAM_MBA)
    switch (cache_op)
    {
    case BAM_CACHE_INVALIDATE:
        (void)qurt_mem_cache_clean((qurt_addr_t)address, size, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);
        break;
    case BAM_CACHE_FLUSH:
        (void) qurt_mem_cache_clean((qurt_addr_t)address, size, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
        break;
    case BAM_CACHE_CLEAN:
        (void)qurt_mem_cache_clean((qurt_addr_t)address, size, QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE);
        break;
    default:
        return;
    }
#elif defined(BAM_TZOS)
#else
    uint32 cmd;

    switch (cache_op)
    {
    case BAM_CACHE_INVALIDATE:
        cmd = DALSYS_CACHE_CMD_INVALIDATE;
        break;
    case BAM_CACHE_FLUSH:
        cmd = DALSYS_CACHE_CMD_FLUSH;
        break;
    case BAM_CACHE_CLEAN:
        cmd = DALSYS_CACHE_CMD_CLEAN;
        break;
    default:
        return;
    }

    DALSYS_CacheCommand(cmd, address, size);
#endif
}

/* ============================================================================
**  Function : bam_osal_memorybarrier
** ============================================================================
*/
/**
  Performs a processor specific memory barrier operation.

  Performs a processor specific memory barrier operation.

  @param  None

  @return
  None.

*/
BAM_API_NON_PAGED void bam_osal_memorybarrier(void)
{
#if defined(BAM_MBA)
    __asm__ __volatile__( "barrier\n" );
#elif defined(BAM_TZOS)
    memory_barrier();
#else
    DALFW_MemoryBarrier();
#endif
}

/* ============================================================================
**  Function : bam_osal_syncinit
** ============================================================================
*/
/**
  Initializes a sync object.

  Initializes a sync object.

  @param[in,out] sync     Pointer to the object to be used for sync

  @return
  Success of the operation.

*/
bam_status_type bam_osal_syncinit(bam_osal_sync_type *sync)
{
#ifndef  BAM_MBA
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                       (DALSYSSyncHandle*)sync,
                                       NULL) )
  {
      *(DALSYSSyncHandle*)sync = NULL;
      return BAM_FAILED;
  }
#endif // #ifndef BAM_MBA
  return BAM_SUCCESS;
}
/* ============================================================================
**  Function : bam_osal_syncdestroy
** ============================================================================
*/
/**
  Destroys a synchronization object.

  Destroys a synchronization object.

  @param[in,out]  sync    Sync object to be destroyed

  @return
  None.

*/
bam_status_type bam_osal_syncdestroy(bam_osal_sync_type *sync)
{
#ifndef BAM_MBA
   DALSYS_DestroyObject(*((DALSYSObjHandle*)sync));
   *(DALSYSSyncHandle*)sync = NULL;
#endif
   return BAM_SUCCESS;

}
/* ============================================================================
**  Function : bam_osal_syncenter
** ============================================================================
*/
/**
  Enters into a synchronized context.

  Enters into a synchronized context.

  @param[in,out]  sync    Sync object to be used

  @return
  None.

*/
BAM_API_NON_PAGED void bam_osal_syncenter(bam_osal_sync_type *sync)
{
    #ifndef BAM_MBA
    DALSYS_SyncEnter(*((DALSYSSyncHandle*)sync));
    #endif // #ifndef BAM_MBA
}

/* ============================================================================
**  Function : bam_osal_syncleave
** ============================================================================
*/
/**
  Leaves a synchronized context.

  Leaves a synchronized context.

  @param[in,out]  sync    Sync object to be used

  @return
  None.

*/
BAM_API_NON_PAGED void bam_osal_syncleave(bam_osal_sync_type *sync)
{
    #ifndef BAM_MBA
    DALSYS_SyncLeave(*((DALSYSSyncHandle*)sync));
    #endif // #ifndef BAM_MBA
}


/* ============================================================================
**  Function : bam_osal_isrinstall
** ============================================================================
*/
/**
  Registers an isr to an IRQ id.

  Registers with the interrupt controller, an ISR to service an IRQ.

  @param[in,out]  irqhandle    Handle to be used with the interrupt controller
  @param[in]      irq          IRQ ID to be registered for
  @param[in,out]  isr          ISR to be invoked to the service the irq
  @param[in,out]  data         data to be supplied to the isr upon invocation

  @return
  Success of the operation.

  @sa
  Other_Relevant_APIS_or_Delete.
*/
bam_status_type bam_osal_isrinstall(bam_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data)
{
    #ifndef BAM_MBA
    if (DAL_SUCCESS != DalInterruptController_RegisterISR(hBamOsalDALInterrupt,
                                                          irq,
                                                          (DALISR)isr,
                                                          (DALISRCtx)data,
                                                          DALINTRCTRL_ENABLE_DEFAULT_SETTINGS))
    {
        return BAM_FAILED;
    }

    irqhandle->dwIrqId = irq;
    #endif // #ifndef BAM_MBA
   return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bam_osal_isruninstall
** ============================================================================
*/
/**
  Deregisters the ISR from the interrupt controller.

  Deregisters the ISR from the interrupt controller.

  @param[in,out]  irqhandle    Interrupt controller handle to be used

  @return
  None.

*/
void bam_osal_isruninstall(bam_osal_irq_type *irqhandle)
{
    #ifndef BAM_MBA
    DalInterruptController_Unregister(hBamOsalDALInterrupt,
                                      irqhandle->dwIrqId);
    #endif // #ifndef BAM_MBA
}

/* ============================================================================
**  Function : bam_osal_israck
** ============================================================================
*/
/**
  Acknowledges the IRQ.

  Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
  to re-activate the IRQ.

  @param[in,out]  irqhandle    Interrupt handle

  @return
  None.

*/
void bam_osal_israck(bam_osal_irq_type *irqhandle)
{
    #ifndef BAM_MBA
    DalInterruptController_InterruptDone(hBamOsalDALInterrupt,
                                         irqhandle->dwIrqId);
    #endif // #ifndef BAM_MBA
}

/* ============================================================================
**  Function : bam_osal_clock_ctl_init
** ============================================================================
*/
/**
  Initializes a clock control object.

  Initializes a clock control object.

  @param[in,out] clock_ctl     Pointer to the object to be used for clock control
  @param[in,out] clock_id      Pointer to the object to identify the clock

  @return
  Success of the operation.

*/
bam_status_type bam_osal_clock_ctl_init(bam_osal_clock_ctl_type *clock_ctl, void* clock_id)
{
#ifndef BAM_MBA
  if (!clock_ctl)
  {
    return BAM_FAILED;
  }

  if (DalClock_GetClockId(hBamOsalDALClock, (char*)clock_id, &clock_ctl->clock_id) != DAL_SUCCESS)
  {
    return BAM_FAILED;
  }
#endif

  return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bam_osal_clock_ctl_deinit
** ============================================================================
*/
/**
  Deinitializes a clock control object.

  Deinitializes a clock control object.

  @param[in,out] clock_ctl     Pointer to the object to be used for clock control

  @return
  Success of the operation.

*/
bam_status_type bam_osal_clock_ctl_deinit(bam_osal_clock_ctl_type *clock_ctl)
{
  return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bam_osal_clock_ctl_enable
** ============================================================================
*/
/**
  Turn on clock.

  Turn on clock.

  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.

*/
bam_status_type bam_osal_clock_ctl_enable(bam_osal_clock_ctl_type *clock_ctl)
{
#ifndef BAM_MBA
  if (!clock_ctl)
  {
    return BAM_FAILED;
  }

  if (DalClock_EnableClock(hBamOsalDALClock, clock_ctl->clock_id) != DAL_SUCCESS)
  {
    return BAM_FAILED;
  }
#endif

  return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bam_osal_syncleave
** ============================================================================
*/
/**
  Turn off clock.

  Turn off clock.

  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.

*/
bam_status_type bam_osal_clock_ctl_disable(bam_osal_clock_ctl_type *clock_ctl)
{
#ifndef BAM_MBA
  if (!clock_ctl)
  {
    return BAM_FAILED;
  }

  if (DalClock_DisableClock(hBamOsalDALClock, clock_ctl->clock_id) != DAL_SUCCESS)
  {
    return BAM_FAILED;
  }
#endif

  return BAM_SUCCESS;
}


/* ============================================================================
**  Function : bam_osal_debug
** ============================================================================
*/
/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return
  None.

*/
#ifdef BAM_OSAL_DEBUG
BAM_API_NON_PAGED void bam_osal_debug(bam_osal_msg_type msgLevel, const char *fmt, ...)
{
   va_list arglist;
   const char *p = fmt;
   uint32 datacount = 0;

   if(BAM_OSAL_MSG_LEVEL > msgLevel)
   return;

   va_start( arglist, fmt );

    /* ulog message data count */
	while(*p != '\0')
	{
		if((*p == '%') && (*(p +1) == '%'))
		{
			p += 2;
		}
		else
		{
			if(*p == '%')
			{
				datacount++;
			}
			p++;
		}
	}


#if defined(BAM_TZOS)
   tzbsp_log_with_ap(msgLevel, fmt, arglist);
#elif !defined(BAM_MBA)
  if(!bam_osal_ulog_enable)
  {
   (void) vprintf( fmt, arglist);
  }
  else
  {
	  /* Ulog messages print */
     ULogFront_RealTimeVprintf(bamulog, datacount, fmt, arglist );
  }
#endif
   va_end( arglist );

}
#endif

