#ifndef _BAM_OSAL_H_
#define _BAM_OSAL_H_

/**
  @file bamosal.h
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
05/20/13   RL      Updates related to LPAE. Physical address in meminfo set to 64bit
04/04/12   NK      New function for destroying mutexes and osal deinit support
03/31/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/



#include "com_dtypes.h"
#include "bam.h"

/** @brief
    Information on a piece of memory allocated
  */

typedef struct _bam_meminfo{
    uint64 pa;     /**< -- Physical address of the memory */
    bam_vaddr va;     /**< -- Virtual address of the memory */
    uint32 size;   /**< -- Size of the memory */
    void *handle;  /**< -- Target specific handle to the memory (reserved) */
}bam_osal_meminfo;


/** @name Cache flags
 *  @brief Flags used for various cache operations
 */

typedef enum
{
    BAM_CACHE_INVALIDATE=0x0, /**< -- Invalidates a cache */
    BAM_CACHE_FLUSH=0x1,      /**< -- Flush a cache */
    BAM_CACHE_CLEAN=0x2       /**< -- Clean a cache */  
}bam_cache_op_type;

/** @name Sync object
  */

typedef struct {
   uint32 none;
}bam_osal_sync_type;

/** @name Clock control object
  */

typedef struct {
   uint32 clock_id;
}bam_osal_clock_ctl_type;

/** @name IrqHandle Object */

typedef struct {
    uint32           dwIrqId;        /**< -- IRQ number to be registered to */
}bam_osal_irq_type;

/** @name Message Type */ 
typedef enum {
   MSG_LOW,
   MSG_MEDIUM,
   MSG_HIGH, 
   MSG_ERR
}bam_osal_msg_type; 


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
void bam_osal_init(void);

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
void bam_osal_deinit(void);


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
void * bam_osal_malloc(uint32 size);

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
void bam_osal_free(void *mem);

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
void bam_osal_memcpy(void *dst, void* src, uint32 size);

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
void bam_osal_mallocmemregion(bam_osal_meminfo *meminfo, uint32 size);

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
void bam_osal_freememregion(bam_osal_meminfo *meminfo);

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
void bam_osal_cachecmd(bam_cache_op_type cache_op,bam_vaddr address, uint32 size);


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
void bam_osal_memorybarrier(void);


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
bam_status_type bam_osal_syncinit(bam_osal_sync_type *sync);


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
bam_status_type bam_osal_syncdestroy(bam_osal_sync_type *sync);

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
void bam_osal_syncenter(bam_osal_sync_type *sync);

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
void bam_osal_syncleave(bam_osal_sync_type *sync);

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
bam_status_type bam_osal_isrinstall(bam_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data);

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
void bam_osal_isruninstall(bam_osal_irq_type *irqhandle);

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
void bam_osal_israck(bam_osal_irq_type *irqhandle);


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
bam_status_type bam_osal_clock_ctl_init(bam_osal_clock_ctl_type *clock_ctl, void* clock_id);

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
bam_status_type bam_osal_clock_ctl_deinit(bam_osal_clock_ctl_type *clock_ctl);

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
bam_status_type bam_osal_clock_ctl_enable(bam_osal_clock_ctl_type *clock_ctl);

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
bam_status_type bam_osal_clock_ctl_disable(bam_osal_clock_ctl_type *clock_ctl);


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
void bam_osal_debug(bam_osal_msg_type msgLevel, const char *fmt, ...);
#else
#define bam_osal_debug(...)
#endif

/* Bam Profiling EVENT IDs */
#define BAM_PIPE_TRANSFER_ENTRY  500
#define BAM_PIPE_TRANSFER_SUBMIT 501
#define BAM_PIPE_TRANSFER_EXIT   502
#define BAM_PIPE_ISR_ENTRY       503
#define BAM_PIPE_ISR_EXIT        504
#define BAM_PIPE_CB_START        505 /* Callback function called in the pipe isr */
#define BAM_PIPE_CB_END          506
#define BAM_ISR_ENTRY            507
#define BAM_ISR_EXIT             508

#define BAM_OSAL_TRACE(EVENT_ID,PAYLOAD0, PAYLOAD1)


#endif // #define _BAM_OSAL_H_