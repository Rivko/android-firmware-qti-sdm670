#ifndef FASTRPC_MEM_H
#define FASTRPC_MEM_H

#include "qurt.h"

struct fastrpc_mem_inits;
struct fastrpc_mem;
struct smq_phy_page;


int fastrpc_mem_create(struct fastrpc_mem_inits* inits, //! optinal private init args
                       char* poolname,                  //! default pool is used if null
                       uint64_t size,                   //! size of va to allocate 
                       struct fastrpc_mem **po,int sid);

void fastrpc_mem_destroy(struct fastrpc_mem* m);

int fastrpc_mem_needs_flush_all(struct fastrpc_mem* me);
void fastrpc_mem_set_flush_all(struct fastrpc_mem* me);

int fastrpc_mem_map(struct fastrpc_mem *me,
                    qurt_perm_t perm,
                    struct smq_phy_page *page, 
                    int numPages);

int fastrpc_mem_map_fixed(struct fastrpc_mem *me,
                          uint64_t va,
                          qurt_perm_t perm,
                          qurt_mem_cache_mode_t mode,
                          struct smq_phy_page *page, 
                          int numPages);

// create a RW copy page
int fastrpc_mem_map_copy_fixed(struct fastrpc_mem *me,
                               uint64_t va,
                               qurt_mem_cache_mode_t mode,
                               struct smq_phy_page *pages,
                               int num);

//! mark the cache as dirty
void fastrpc_mem_dirty(struct fastrpc_mem* me);
//! mark the cache as clean
void fastrpc_mem_clean(struct fastrpc_mem* me);

int fastrpc_mem_flush_all(int process_sid);

//! remove all the mapped physical memory
void fastrpc_mem_unmap_all(struct fastrpc_mem* me);

//! flush, delayed until cache_sync is called
int fastrpc_mem_flush(struct fastrpc_mem *me,
                      uint64_t va,
                      uint64_t size);

//! invalidate, delayed until cache_sync is called
int fastrpc_mem_inv(struct fastrpc_mem *me,
                    uint64_t va,
                    uint64_t size);

//! commit the cache operations
int fastrpc_mem_sync_flush(struct fastrpc_mem* me);
int fastrpc_mem_sync_inv(struct fastrpc_mem* me);

#endif // FASTRPC_MEM_H
