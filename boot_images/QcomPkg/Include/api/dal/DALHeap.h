#ifndef _DAL_HEAP
#define _DAL_HEAP
/*==============================================================================
@file DALHeap.h


        Copyright (c) 2014 Qualcomm Technologies, Inc.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary


==============================================================================*/

// Supported memory types for heap
typedef enum 
{
   DALHEAP_IMEM,
   DALHEAP_EXT_MEM,
   DALHEAP_NUM
}
DALHEAP_MEM_TYPE;

/*=============================================================================
                           PROTOTYPE DECLARATIONS
=============================================================================*/
/**
  @brief Initialize internal and external heap

  Invoke to provide heap memory to heap manager
  to heap manager.

  @param pHeapBase  : Base address of the heap
  @param dwHeapSz   : Size of the heap
  @param memType    : Type of heap

  @return Dal - Success/Failure
*/
DALResult DALSYS_HeapInit(VOID *pHeapBase, UINT32 dwHeapSz, 
      DALHEAP_MEM_TYPE memType);

/**
  @brief De-Initialize a heap given its base address

  Invoke to remove a heap memory region from the list of heaps
  currently managed by heap manager.

  @param pHeapBase  : Base address of the heap

  @return Dal - Success/Failure
*/
DALResult DALSYS_HeapDeInit(VOID *pHeapBase); 


/**
  @brief Force De-Initialize a heap given its base address

  Invoke to remove a heap memory region from the list of heaps
  currently managed by heap manager even if the heap is under use 
  and not in reset state.

  The API client is responsible for any side effect of the 
  heap force deinit such as :
   - the user who had the memory still allocated , if later calls free() for that memory ptr.
   - the user who had the memory still allocated, may still be actively using the memory so 
     if after the force_deinit heap the heap memory relinquished for some other purpose
   then the active user may corrupt it since it still holds the heap memory address
  
  @param pHeapBase  : Base address of the heap

  @return Dal - Success/Failure
*/
DALResult DALSYS_HeapForceDeInit(VOID *pHeapBase); 
#endif //_DAL_HEAP

