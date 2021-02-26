/**
  @file bamosal.c
  @brief
  This file contains implementation of interfaces that provide a thin 
  abstraction to various OS-specific APIs used by the BAM driver.
  
*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/14   RL      64-bit support
04/12/12   NK      New funciton for destroying mutexes plus osal deinit support
03/31/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2016 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "com_dtypes.h"
#include "bamosal.h"
#include "bam.h"
#include "DALSys.h"
#include "DALFramework.h"

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
    DALResult res = DALSYS_Malloc(size, &ptr);

    if (res != DAL_SUCCESS)
    {
    return NULL;
    }

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
    DALSYS_Free(pmem);
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
    DALSYS_memcpy(dst, src, size);
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
void bam_osal_mallocmemregion(bam_osal_meminfo *meminfo, uint32 size)
{    
    DALSYSMemInfo dalmeminfo;

    if (meminfo == NULL)
    {
        return;
    }

    if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
                                              DALSYS_MEM_PROPS_PHYS_CONT,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              meminfo->pa ? (bam_vaddr)meminfo->pa : DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              size,
                                              &meminfo->handle,
                                              NULL) )
    {
        meminfo->handle = NULL;
        return;
    }

    DALSYS_MemInfo(meminfo->handle, &dalmeminfo);

    meminfo->pa = dalmeminfo.PhysicalAddr;
    meminfo->va = dalmeminfo.VirtualAddr;
    meminfo->size = dalmeminfo.dwLen;
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
void bam_osal_freememregion(bam_osal_meminfo *meminfo)
{
    if (meminfo != NULL && meminfo->handle != NULL)
    {
        DALSYS_DestroyObject(meminfo->handle);
    }
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
void bam_osal_cachecmd(bam_cache_op_type cache_op,bam_vaddr address, uint32 size)
{
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
void bam_osal_memorybarrier(void)
{
    DALFW_MemoryBarrier();
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
void bam_osal_syncenter(bam_osal_sync_type *sync)
{
    
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
void bam_osal_syncleave(bam_osal_sync_type *sync)
{
    
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
void bam_osal_debug(bam_osal_msg_type msgLevel, const char *fmt, ...)
{
   va_list arglist;

   if(BAM_OSAL_MSG_LEVEL > msgLevel)
   return;

   va_start( arglist, fmt );
   tzbsp_log_with_ap(msgLevel, fmt, arglist);
   va_end( arglist );
	
}
#endif
