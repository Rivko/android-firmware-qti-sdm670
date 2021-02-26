/*============================================================================
@file CorePool.c

Implements a memory pool.

Individual pools are stored in CorePoolType structs with block and element
linked lists.

"Blocks" are the larger blocks of memory divided into elements. Blocks are
stored in lists of CorePoolBlockType structs. 

"Elements" are the memory pieces allocated/freed (given out/returned to the pool.)
Elements are stored in a pool's free list of CorePoolElementType structs until they're
given out. The pool doesn't maintain any bookkeeping on the elements after they're
allocated (given out) to higher level code. When the element is free'd (returned to
pool) it is added back to the free list. 

There are three ways to create a pool. 
1) CorePoolInit   - Create a pool with memory the caller provides 
2) CorePoolCreate - Create a pool, the memory will be allocated in this function.
3) CorePoolConfig - Create a pool, the memory will be allocated in this function,
                    and set future block allocation sizes for this pool.

Pools can be customized even further by customizing the pool's allocation
function but this probably isn't done too often in practice, the default 
DALSYS_Malloc is probably appropriate for most use cases .

        Copyright 2009-2014,2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/10/14   sj      Merging mainline changes
10/16/09   yz      Initial import from sg's npa_pool.c

==============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "CorePool.h"
#include "CoreVerify.h"

#ifndef TARGET_UEFI
#include <stdio.h>
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** @brief These are the larger chunks of memory that are split into elements in a pool. */
typedef struct CorePoolBlockType
{
  struct CorePoolBlockType    *pNext;
  unsigned int                 uSize;
  char                         aData[];
} CorePoolBlockType;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * <!-- Core_PoolLock -->
 *
 * @brief Calls DALSYS_SyncEnter on the lock inside the pool.
 *
 * @param pPool : Pointer to a CorePoolType to be locked. 
 */
void Core_PoolLock( CorePoolType *pPool )
{
  if ( pPool && pPool->hLock ) DALSYS_SyncEnter( pPool->hLock );
}


/**
 * <!-- Core_PoolUnlock -->
 *
 * @brief Calls DALSYS_SyncLeave on the lock inside the pool.
 *
 * @param pPool : Pointer to a CorePoolType to be unlocked. 
 */
void Core_PoolUnlock( CorePoolType *pPool )
{
  if ( pPool && pPool->hLock ) DALSYS_SyncLeave( pPool->hLock );
}


/**
 * <!-- Core_PoolPush -->
 *
 * @brief Make this element available again by putting it on the pool's free list. 
 *
 * @param pPool : The pool to add the element back to.
 * @param pElem : The element to add back to the pool's free list.
 */
static void Core_PoolPush( CorePoolType *pPool, CorePoolElementType *pElem )
{
  CORE_VERIFY_PTR( pElem );

  /* Must be locked */
  Core_PoolLock( pPool );
  pElem->pNext = pPool->pFreeList;
  pPool->pFreeList = pElem;
  Core_PoolUnlock( pPool );
}


/**
 * <!-- Core_PoolPop -->
 *
 * @brief Remove an element from the pool's free list. 
 *
 * @param pPool : The pool to add the element back to.
 *
 * @return An element that has been removed for the caller to use.
 */
static CorePoolElementType *Core_PoolPop( CorePoolType *pPool )
{
  CorePoolElementType *pElem = NULL;

  /* Must be locked */
  Core_PoolLock( pPool );
  pElem = pPool->pFreeList;
  if ( NULL != pElem ) 
  { 
    pPool->pFreeList = pElem->pNext;
  }
  Core_PoolUnlock( pPool );
  return pElem;
}


/**
 * <!-- Core_PoolAddBlock -->
 *
 * @brief Add a block to the pool's blocklist and update the count. 
 *
 * This function is used by Core_PoolFill as the first step in the memory 
 * bookkeeping to store and ultimately slice memory up into the available
 * smaller pool elements. 
 *
 * Casts the provided memory as a CorePoolBlockType and links it into 
 * the pool's linked list of blocks. The remaining memory is sent back via
 * the return pointer to get sliced into new usable (free) elements in the 
 * pool. 
 *
 * @param pPool : The pool to add the block to.
 * @param pData : Pointer to the memory to use.
 * @param pBlockSize : Size of the block.
 *
 * @return Pointer to the memory in the block that will be used as elements. 
 */
static void *Core_PoolAddBlock( CorePoolType *pPool, void *pData, unsigned int *pBlockSize )
{
  CORE_VERIFY_PTR( pPool );
  CORE_VERIFY_PTR( pData );
  CorePoolBlockType *pBlock = (CorePoolBlockType *)pData;
  
  CORE_VERIFY_PTR( pBlockSize );
  CORE_VERIFY ( *pBlockSize >= sizeof( CorePoolBlockType ) );
  pBlock->uSize = *pBlockSize;

  Core_PoolLock( pPool );
  pBlock->pNext = pPool->pBlockList;
  pPool->pBlockList = pBlock;
  pPool->uBlockCount++;
  Core_PoolUnlock( pPool );

  /* This block's new size is the orininal pBlockSize passed in minus the space
    used to add this block to the pool's linked list of blocks.  */
  *pBlockSize -= sizeof( CorePoolBlockType ); 

  /* Return a pointer to the usable memory at the end of the struct. */
  return &pBlock->aData;
}


/**
 * <!-- Core_PoolFill -->
 *
 * @brief Add a block to the pools blocklist, divide it up into emements and add
 *  them to the pool's free list. 
 *
 * Used at pool initialization and subsequent block allocations to add more
 * available elements. 
 * 
 * @param pPool : The pool to add the memory to. 
 * @param pData : Pointer to the memory to use.
 * @param pBlockSize : Size of the block.
 */
static void Core_PoolFill( CorePoolType *pPool, void *pData, unsigned int uBlockSize )
{
  pData = Core_PoolAddBlock( pPool, pData, &uBlockSize );
  CORE_VERIFY_PTR( pData );

  while ( uBlockSize >= pPool->uElemSize )
  {
    CorePoolElementType *pElem = (CorePoolElementType *)pData;

    Core_PoolPush( pPool, pElem );

    Core_PoolLock( pPool );
    pPool->uElemCount++;
    Core_PoolUnlock( pPool );

    pData = (char *)pData + pPool->uElemSize;
    uBlockSize -= pPool->uElemSize;
  }
}

/*------------------------------------------------------------------------------
  public functions
------------------------------------------------------------------------------*/

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
void Core_PoolInit( CorePoolType *pPool, 
                    unsigned int uElemSize, 
                    void *pData, 
                    unsigned int uBlockSize )
{
  CORE_VERIFY_PTR( pPool );
  memset( pPool, 0, sizeof( CorePoolType ) );

  /* Most elements will be the size of the size requested, but this makes sure if there
     was ever a small element size, the pool elements are large enough to store all the 
     CorePoolElementType data fields.*/
  pPool->uElemSize  = MAX( uElemSize, sizeof(CorePoolElementType) );

  /* Create a lock for this pool. */
  CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &pPool->hLock, NULL) );

  if ( 0 != uBlockSize )
  {
    CORE_VERIFY_PTR( pData );
    pPool->uBlockSize = uBlockSize;
    Core_PoolFill( pPool, pData, uBlockSize );
  }
}


/**
 * <!-- Core_PoolCreate -->
 *
 * @brief Initialize a pool with memory the this function will allocate.
 *
 * This function dynamically allocates a pool of memory buffers from the
 * default DALSYS heap, initialize the pool, and set a default allocator
 * function for subsequent allocations from the same heap.
 *
 * Users should call Core_PoolInit() if they don't desire dynamic memory
 * allocation.
 *
 * @note See the description of Core_PoolInit() for the alignment issue.
 *
 * @param pPool : pointer to the pool to be initialized
 * @param uElemNum : the number of memory buffers in the pool
 * @param uElemSize : fixed size of memory elements being managed by pool
 */
void Core_PoolCreate( CorePoolType *pPool, 
                      unsigned int uElemNum, 
                      unsigned int uElemSize)
{
  void *pData = NULL;
  unsigned int uBlockSize = CORE_POOL_BLOCK_SIZE( uElemNum, uElemSize );

  CORE_VERIFY_PTR( pPool );
  CORE_DAL_VERIFY( DALSYS_Malloc(uBlockSize, &pData) );

  Core_PoolInit(pPool, uElemSize, pData, uBlockSize);
  Core_PoolUseAllocatorFcn(pPool, Core_DALSYSAllocatorFcn);
}


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
void Core_PoolConfig( CorePoolType *pPool, 
                      const CorePoolConfigType *pConfig )
{
  CORE_VERIFY_PTR( pPool );
  CORE_VERIFY_PTR( pConfig );

  Core_PoolCreate( pPool, pConfig->initial, pConfig->size );
  Core_PoolSetAllocationSize( pPool, pConfig->subsequent );
}


/**
 * <!-- Core_PoolAlloc -->
 * 
 * @brief Allocate (return a pointer to) an element from the pool.
 *
 * @param pPool : Pointer to the pool to allocate from
 *
 * @return memory allocated, or NULL if there was no memory available
*/
void *Core_PoolAlloc( CorePoolType *pPool )
{
  CorePoolElementType *pElem;
  CORE_VERIFY_PTR( pPool );

  pElem = Core_PoolPop( pPool );
  
  if ( ( NULL == pElem ) && 
       ( NULL != pPool->pfnAllocatorFcn ) && 
       ( pPool->uBlockSize > 0 ) )
  {
    unsigned int uPoolSize = pPool->uBlockSize;
    void *pData = pPool->pfnAllocatorFcn( uPoolSize );
    if ( NULL != pData )
    {
      Core_PoolFill( pPool, pData, uPoolSize );
      pElem = Core_PoolPop( pPool );
    }
  }
  return pElem;
}


/**
 * <!-- Core_PoolFree -->
 * 
 * @brief Free an element that was allocated from the pool. (Return it to the pool)
 *
 * @param pPool : pointer to the pool the element was allocated from
 * @param pData : pointer to data being freed
 */
void Core_PoolFree( CorePoolType *pPool, void *pData )
{
  CORE_VERIFY_PTR( pPool );
  CORE_VERIFY_PTR( pData );
  Core_PoolPush( pPool, (CorePoolElementType *)pData );
}


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
 *  allocations
 */
void Core_PoolSetAllocationSize( CorePoolType *pPool, 
                                 unsigned int  uElemNum)
{
  CORE_VERIFY_PTR( pPool );
  
  /* CORE_POOL_BLOCK_SIZE calculates the space needed for */
  pPool->uBlockSize = CORE_POOL_BLOCK_SIZE(uElemNum, pPool->uElemSize);
}


/**
 * <!-- Core_DALSYSAllocatorFcn -->
 *
 * @brief Memory allocation function
 *
 * This function is a wrapper of DALSYS_Malloc(), providing a utility memory
 * allocator of the well-known standard signature.
 *
 * @param uSize : Size of memory block to allocate
 *
 * @return The allocated memory or NULL if failure.
 */
void *Core_DALSYSAllocatorFcn( unsigned int uSize )
{
  void *pData = NULL;

  if( DAL_SUCCESS != DALSYS_Malloc( uSize, &pData ) )
    return NULL;

  return pData;
}


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
void Core_PoolUseAllocatorFcn( CorePoolType *pPool, void *(*pfnAllocatorFcn)( unsigned int uSize ) )
{
  CORE_VERIFY_PTR( pPool );
  pPool->pfnAllocatorFcn = pfnAllocatorFcn;
}