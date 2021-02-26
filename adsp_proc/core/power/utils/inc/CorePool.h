/*==============================================================================
@file CorePool.h

Memory pool API

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CorePool.h#1 $
==============================================================================*/
#ifndef COREPOOL_H
#define COREPOOL_H

#include "CoreMacros.h"
#include "CoreMutex.h"

#ifdef WINSIM
#include "..\pc\CorePoolOffline.h"
#endif

/** @brief Top level container for individual pools */
typedef struct CorePoolType
{
  /** @brief Blocks are the larger memory regions split into elements. */
  struct CorePoolBlockType    *pBlockList;
  unsigned int uBlockSize;  /* size for future memory block allocations */
  unsigned int uBlockCount; /* incremented when Core_PoolAddBlock is called */

  /** @brief If an element hasn't been given out, it is on the free list. */
  struct CorePoolElementType  *pFreeList;
  unsigned int uElemSize; 
  unsigned int uElemCount;

  void *( * pfnAllocatorFcn )( unsigned int uSize );

  CoreMutex *hLock;
} CorePoolType;


/** @brief Elements are the memory pieces from larger blocks that are 
           allocated/freed (given out/returned to memory pools.) */
typedef struct CorePoolElementType
{
  struct CorePoolElementType *pNext;
  char                        aData[];
} CorePoolElementType;


/** @brief Calculate the size of a block to allocate based on the size of the
   elements, the number to allocated for, and the size of the CorePoolBlock
   The pool allocator is a fixed size allocation */
#define CORE_POOL_BLOCK_SIZE( N, SizeOfN )                                     \
  ( ( N ) ? sizeof(struct CorePoolBlockType *) + sizeof(unsigned int) +        \
  ( N ) * ( MAX( SizeOfN, sizeof(struct CorePoolElementType) ) ) : 0 )  


/** @brief Used by Core_PoolConfig to define the initial pool size and
     subsequent block allocations */
typedef struct CorePoolConfigType
{
  unsigned int size;          /* The size of each element */
  unsigned int initial;       /* The number of elements in the initial allocation */
  unsigned int subsequent;    /* The number of elements in subsequent allocations */
} CorePoolConfigType;


/**
 * <!-- Core_PoolInit -->
 *
 * @brief Initialize a pool with memory the caller provides.
 *
 * The pool is a collection of fixed size memory buffers.  This
 * function partitions the initial block of data into uElemSize
 * chunks.  If the pool is not assigned an allocation_function, the
 * pool will be limited to this initial allocation. If an
 * allocation_function is provided, the pool will dynamically grow as
 * needed.
 *
 * This implementation does not enforce any alignment restrictions. If 
 * a memory buffer is used to store multiple structures, the user 
 * should be responsible for the enforcement of memory alignment.
 *
 * Users should call this function if they don't desire dynamic memory
 * allocation, otherwise, call Core_PoolCreate().
 *
 * @param pPool : pointer to the pool to be initialized
 * @param uElemSize : fixed size of memory elements being managed by pool
 * @param pData : pointer to initial block of data used to fill the pool
 * @param uBlockSize : size of initial block of data and default size for 
 * subsequent alloactions
 */
void Core_PoolInit( CorePoolType *pPool, unsigned int uElemSize, 
                    void         *pData, unsigned int uBlockSize );


/**
 * <!-- Core_PoolCreate -->
 *
 * @brief Initialize a pool with memory the this function will allocate.
 *
 * This function dynamically allocates a pool of memory buffers from the
 * default DALSYS heap, initialize the pool, and set a default allocator
 * function for subsequent allocations from the same heap.
 *
 * Users should call Core_PoolInit() if they don't want dynamic memory
 * allocation.
 *
 * @note See the description of Core_PoolInit() for the alignment issue.
 *
 * @param pPool : pointer to the pool to be initialized
 * @param uElemNum : the number of memory buffers in the pool
 * @param uElemSize : fixed size of memory elements being managed by pool
 */
void Core_PoolCreate( CorePoolType *pPool, 
                      unsigned int uElemNum, unsigned int uElemSize);


/**
 * <!-- Core_PoolConfig -->
 *
 * @brief Initialize a pool with memory the this function will allocate and
 * configure subsequent allocations.
 *
 * This function dynamically allocates a pool of memory buffers from the 
 * default DALSYS heap, initialize the pool. If the number of elements in 
 * subsequent allocations is not zero, which is specified by the function 
 * parameter pConfig, it sets the allocator to a default one and the 
 * subsequent block size accordingly.
 *
 * Users should call Core_PoolInit() if they don't desire dynamic memory
 * allocation. 
 * 
 * @note See the description of Core_PoolInit() for the alignment issue.
 *
 * @param pPool : pointer to the pool to be initialized
 * @param pConfig : the number of memory buffers in the initial and 
 *  subsequent allocations
 * @param uElemSize : fixed size of memory elements being managed by pool   
 */
void Core_PoolConfig( CorePoolType             *pPool, 
                      const CorePoolConfigType *pConfig );


/**
 * <!-- Core_PoolAlloc -->
 * 
 * @brief Allocate (return a pointer to) an element from the pool.
 *
 * @param pPool : Pointer to the pool to allocate from
 *
 * @return memory allocated, or NULL if there was no memory available
*/
void *Core_PoolAlloc( CorePoolType *pPool );


/**
 * <!-- Core_PoolFree -->
 * 
 * @brief Free an element that was allocated from the pool. (Return it to the pool)
 *
 * @param pPool : pointer to the pool the element was allocated from
 * @param pData : pointer to data being freed
 */
void Core_PoolFree( CorePoolType *pPool, void *pData );


/**
 * <!-- Core_PoolSetAllocationSize -->
 *
 * @brief Set the number of elements to allocate in subsequent block allocations.
 *  
 * This function sets the reallocation size if an allocation is
 * attempted when the pool is empty and there is an allocation
 * function registered.  If a uElemNum is not set, the initial
 * allocation size will be used. 
 *
 * The allocation size is in the number of elements.
 *
 * @param pPool : pointer to the pool
 * @param uElemNum : the number of elements to be allocated in subsequent
 * allocations
 */
void Core_PoolSetAllocationSize( CorePoolType *pPool, unsigned int uElemNum );

/**
 * <!-- Core_PoolUseAllocatorFcn -->
 *
 * @brief  Define a pool allocator function (malloc)
 *
 * This function allows the pool to dynamically allocate memory if the
 * pool is empty and an allocation is attempted.  If an allocator
 * function is not assigned, the pool will be limited to the memory
 * initilly allocated to it at construction time
 *
 * @param pPool : Pointer to pool being assigned allocator function
 * @param pfnAllocatorFcn : Allocator fucntion to be used
 */
void 
Core_PoolUseAllocatorFcn( CorePoolType *pPool, 
                          void *(*pfnAllocatorFcn)( unsigned int uSize ) );

#endif /* COREPOOL_H */
