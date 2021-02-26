#ifndef CORE_POOL_H
#define CORE_POOL_H

/*==============================================================================
  @file CorePool.h

  Memory pool Function/Structure Declarations

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DALSys.h"
#ifdef WINSIM
#include "..\pc\CorePoolOffline.h"
#endif

#ifndef MAX
#define MAX( a, b ) ((a)>(b)) ? (a) : (b)
#endif


/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/

typedef struct CorePoolType
{
  struct CorePoolBlockType    *pBlockList;
  unsigned int uBlockSize; 
  unsigned int uBlockCount;

  struct CorePoolElementType  *pFreeList;
  unsigned int uElemSize; 
  unsigned int uElemCount;

  void *( * pfnAllocatorFcn )( unsigned int uSize );

  DALSYSSyncHandle hLock;

} CorePoolType;

typedef struct CorePoolElementType
{
  struct CorePoolElementType    *pNext;
  char                           aData[];
} CorePoolElementType;

/* The pool allocator is a fixed size allocation */
#define CORE_POOL_BLOCK_SIZE( N, SizeOfN )                                     \
  ( ( N ) ? sizeof(struct CorePoolBlockType *) + sizeof(unsigned int) +        \
  ( N ) * ( MAX( SizeOfN, sizeof(struct CorePoolElementType) ) ) : 0 )  

typedef struct CorePoolConfigType
{
  unsigned int size;          /* The size of each element */
  unsigned int initial;       /* The number of elements in the initial allocation */
  unsigned int subsequent;    /* The number of elements in subsequent allocations */
} CorePoolConfigType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
   @brief Core_PoolInit - Initialize the pool

   The pool is a collection of fixed size memory buffers.  This
   function partitions the initial block of data into uElemSize
   chunks.  If the pool is not assigned an allocation_function, the
   pool will be limited to this initial allocation. If an
   allocation_function is provided, the pool will dynamically grow as
   needed.

   This implementation does not enforce any alignment restrictions. If 
   a memory buffer is used to store multiple structures, the user 
   should be responsible for the enforcement of memory alignment.

   Users should call this function if they don't desire dynamic memory
   allocation, otherwise, call Core_PoolCreate().

   @param pPool : pointer to the pool to be initialized
   @param uElemSize : fixed size of memory elements being managed by pool
   @param pData : pointer to initial block of data used to fill the pool
   @param uBlockSize : size of initial block of data and default size for 
   subsequent alloactions
*/
void Core_PoolInit( CorePoolType   *pPool, 
                    unsigned int    uElemSize, 
                    void           *pData, 
                    unsigned int    uBlockSize );

/**
   @brief Core_PoolCreate - Create a memory pool and initialize it

   This function dynamically allocates a pool of memory buffers from the 
   default DALSYS heap, initialize the pool, and set a default allocator 
   function for subsequent allocations from the same heap.

   See the description of Core_PoolInit() for the alignment issue.

   Users should call Core_PoolInit() if they don't desire dynamic memory
   allocation. 
  
   @param pPool : pointer to the pool to be initialized
   @param uElemNum  : the number of memory buffers in the pool
   @param uElemSize : fixed size of memory elements being managed by pool   
*/
void Core_PoolCreate( CorePoolType *pPool, 
                      unsigned int uElemNum, 
                      unsigned int uElemSize);

/**
   @brief Core_PoolConfig - Create a memory pool, initialize and 
   configure subsequent allocations

   This function dynamically allocates a pool of memory buffers from the 
   default DALSYS heap, initialize the pool. If the number of elements in 
   subsequent allocations is not zero, which is specified by the function 
   parameter pConfig, it sets the allocator to a default one and the 
   subsequent block size accordingly.

   See the description of Core_PoolInit() for the alignment issue.

   Users should call Core_PoolInit() if they don't desire dynamic memory
   allocation. 
  
   @param pPool : pointer to the pool to be initialized
   @param pConfig  : the number of memory buffers in the initial and 
   subsequent allocations
   @param uElemSize : fixed size of memory elements being managed by pool   
*/
void Core_PoolConfig(       CorePoolType       *pPool, 
                      const CorePoolConfigType *pConfig );

/**
   @brief Core_PoolAlloc -  Allocate an element from the pool

   @param pPool : pointer to the pool to allocate from
   @return memory allocated, or NULL if there was no memory available
*/
void *Core_PoolAlloc( CorePoolType *pPool );

/**
   @brief Core_PoolFree - Free an element that was allocated from the pool

   @param pPool: pointer to the pool the element was allocated from
   @param pData: pointer to data being freed
*/
void Core_PoolFree( CorePoolType *pPool, void *pData );

/**
   @brief Core_PoolSetAllocationSize - Size for subsequent allocations
   
   This function sets the reallocation size if an allocation is
   attempted when the pool is empty and there is an allocation
   function registered.  If a uElemNum is not set, the initial
   allocation size will be used. 

   The allocation size is in the number of elements.

   @param pPool : pointer to the pool
   @param uElemNum : the number of elements to be allocated in sunsequent 
   allocations
*/
void Core_PoolSetAllocationSize( CorePoolType *pPool, unsigned int uElemNum);

/**
   @brief Core_AllocatorFcn - Memory allocation function

   This function is a wrapper of DALSYS_Malloc(), providing a utility memory
   allocator of the well-known standard signature.

   @param uSize : size of memory block
*/
void *Core_DALSYSAllocatorFcn( unsigned int uSize );

/**
   @brief Core_PoolUseAllocatorFcn - Define a pool allocator function
   (malloc)

   This function allows the pool to dynamically allocate memory if the
   pool is empty and an allocation is attempted.  If an allocator
   function is not assigned, the pool will be limited to the memory
   initilly allocated to it at construction time

   @param pPool: pointer to pool being assigned allocator function
   @param pfnAllocatorFcn: allocator fucntion to be used
*/
void Core_PoolUseAllocatorFcn( CorePoolType *pPool, void *(*pfnAllocatorFcn)( unsigned int uSize ) );
#endif /* CORE_POOL_H */
