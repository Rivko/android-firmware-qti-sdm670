#ifndef APPS_MEM_HEAP_H
#define APPS_MEM_HEAP_H

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * initialize the heap for the calling process.  This heap exists
 * for the lifetime of the calling hlos process and should be
 * initialized once with the ion heap id to use for allocation.
 * 
 * @param ion_heap, the ion heap id to use
 * @param ion_flags, the flags to pass to ion when mapping/creating
 * @param flags, flags used by adsprpc
 * @param minsize, minimum size to map from remote heap, must be page
 *        aligned
 *
 * @retval, 0 on success
 */
int apps_mem_heap_init(int ion_heap, uint32 ion_flags, uint32 flags, int minsize);

/**
 * realloc function, not zero initialized
 */
void* apps_mem_heap_realloc(void* po, int size);

/**
 * malloc function, not zero initialized
 */
void* apps_mem_heap_malloc(int size);

/**
 * free function
 */
void apps_mem_heap_free(void* po);

/**
 * translate the local address to an apps address if its managed by this heap.
 * @param po, an address from a buffer allocated by realloc or malloc
 * @param vapps, the address on apps
 * @retval, 0 on success
 */

int apps_mem_heap_get_apps_addr(void* po, uint64* vapps);

/**
 * utility to convert uint64 to addres pointer, copy this to your apps code
 */
static __inline void* apps_mem_heap_to_ptr(uint64 vapps) {
   return (void*)(unsigned)vapps;
}

#ifdef __cplusplus
}
#endif
#endif //APPS_MEM_HEAP_H
