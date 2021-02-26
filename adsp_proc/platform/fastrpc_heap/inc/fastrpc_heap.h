#ifndef FASTRPC_HEAP_H
#define FASTRPC_HEAP_H

#include <stdlib.h>

#define ADSP_MMAP_ANY_ADDR    0
#define ADSP_MMAP_FIXED_ADDR  1
#define ADSP_MMAP_HEAP_ADDR   4
#define ADSP_MMAP_ADD_PAGES   0x1000
#define ADSP_MMAP_ADD_REMOTE_PAGES   8

#define REMOTE_HEAP_BLOCK_MAX 8

int mem_region_alloc(void** region, unsigned addr, int size, int fsregion);
int mem_region_free(void* region, int size, int fsregion);
int grow_user_heap(void* mht, size_t size, unsigned int * _vaddr);
int shrink_user_heap(void* mht, void* addr, unsigned int size);

int add_user_heap(void *ptr, int size, int granularity);
void mem_gc_thread_disable(void);

void* rpc_realloc(void* ptr, size_t size);
void* rpc_malloc(size_t size);
void* rpc_calloc(size_t num_elem, size_t elem_size);
void* rpc_memalign(size_t block_size, size_t size);
void  rpc_free(void *ptr);

#endif // FASTRPC_HEAP_H
