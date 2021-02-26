/** @file DALHeap.c

  DAL Heap

  Copyright (c) 2015, 2016, 2017 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/20/17   ak      Added Multi Heap ability
 01/30/17   ak      Ported memheap_lite 
 03/27/15   unr     64 bit fixes
 08/11/14   sho     Creation

=============================================================================*/
#include "DALStdDef.h"
#include "DALHeap.h"
#include "DALSys.h" 
#include "memheap_lite.h"

#define MAX_NUM_DALHEAPS 5

typedef struct 
{
    mem_heap_type heap_struct;
    uint8_t initialized;
    void* pHeapBase;
}DALSYSMemState;

static DALSYSMemState memState[MAX_NUM_DALHEAPS] = {{{0}, FALSE},
													{{0}, FALSE},
													{{0}, FALSE},
													{{0}, FALSE},
													{{0}, FALSE}, 
												   }; 
  

DALResult DALSYS_HeapInit(VOID *pHeapBase, UINT32 dwHeapSz, 
      DALHEAP_MEM_TYPE memType ) 
{
    //Find an empty spot in memState array
    int idx =0;
    for(idx =0; idx < MAX_NUM_DALHEAPS; idx++)
    {
    	if(memState[idx].initialized == FALSE) break;
    }

    if(idx == MAX_NUM_DALHEAPS) return DAL_ERROR; //All slots taken

    // Set flag to indicate that we have initialized this heap
    memState[idx].initialized = 0x1;
    // Initialize heap structure with all zeros
    memset(&memState[idx].heap_struct, 0, sizeof(mem_heap_type));
    // Initialize the heap
    mem_init_heap(&memState[idx].heap_struct, pHeapBase, dwHeapSz);
    //keep track of heap base ptr
    memState[idx].pHeapBase = pHeapBase;
    return DAL_SUCCESS;

}

DALResult DALSYS_HeapDeInit(VOID *pHeapBase) 
{
	//Find which heap is being freed
    int idx =0;
    for(idx =0; idx < MAX_NUM_DALHEAPS; idx++)
    {
    	if(memState[idx].pHeapBase == pHeapBase)
    	{
    		mem_deinit_heap(&(memState[idx].heap_struct));
    		memState[idx].initialized = FALSE;
    		memset(&memState[idx].heap_struct, 0, sizeof(mem_heap_type));
    		memState[idx].pHeapBase = NULL;
    		return DAL_SUCCESS;
    	}
    }
    return DAL_ERROR;
}

DALResult DALSYS_HeapForceDeInit(VOID *pHeapBase) 
{
  //Find which heap is being freed
    int idx =0;
    for(idx =0; idx < MAX_NUM_DALHEAPS; idx++)
    {
      if(memState[idx].pHeapBase == pHeapBase)
      {
        mem_force_deinit_heap(&(memState[idx].heap_struct));
        memState[idx].initialized = FALSE;
        memset(&memState[idx].heap_struct, 0, sizeof(mem_heap_type));
        memState[idx].pHeapBase = NULL;
        return DAL_SUCCESS;
      }
    }
    return DAL_ERROR;
}

DALResult
DALSYS_Malloc(UINT32 dwSize, VOID **ppMem)
{
	int idx=0;
   for(idx =0; idx < MAX_NUM_DALHEAPS; idx++)
   {
	   // If the pointer to the pointer is null, skip it
	   if(ppMem != NULL)
	   {
	       // Call into mem_malloc
	       *ppMem = mem_malloc(&memState[idx].heap_struct, dwSize);
	       if(*ppMem != NULL)
	       {
	           return DAL_SUCCESS;
	       }
	   }
   }

   return DAL_ERROR;
}

DALResult
DALSYS_Free(VOID *pmem)
{
   int  idx =0;
   // Check to see which heap this pointer came from (and if that heap was initialized)
   for(idx =0; idx < MAX_NUM_DALHEAPS; idx++)
   {
   		if(memState[idx].initialized && \
      	((pmem >= (void*) memState[idx].heap_struct.first_block) && \
       	(pmem < (((void*) memState[idx].heap_struct.first_block) + memState[idx].heap_struct.total_bytes))))
   		{
   			mem_free(&memState[idx].heap_struct, pmem);
   			return DAL_SUCCESS;
   		}
   }
   if(idx == MAX_NUM_DALHEAPS) return DAL_ERROR; //pointer passed didn't come from any existing heaps
   return DAL_SUCCESS;
}
