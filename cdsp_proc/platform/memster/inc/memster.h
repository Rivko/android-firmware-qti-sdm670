#ifndef MEMSTER_H
#define MEMSTER_H

#include <stdint.h>

struct memster;

struct memster {
   struct memster **allocs; //! size num
   unsigned num;
   unsigned max;
   uint64_t start;
   uint64_t end;
   struct memster *parent;
   void (*pfn)(void* ctx);
   void *ctx;
};

/**
 * initialize the heap at the end of the memory region
 * @param start, memmory start
 * @param end, memmory end
 * @param po, the memster pointer
 * @retval, 0 on success
 */
int memster_init(uint64_t start, uint64_t end, struct memster** po);

/**
 * destroy the memster and its children
 * render all child pointer invalid
 * release memory owned by this memster on the parent
 */
void memster_destroy(struct memster* me);

/**
 * destroy the memsters children
 */
void memster_destroy_children(struct memster* me);


/**
 * acquire some space
 * @param me, the memster struct
 * @param align, the starting alignment for the requested space
 * @param size, the size for the requested space
 * @param ppo, the acquired space
 * @retval, 0 on success
 */
int memster_acquire(struct memster* me, unsigned align, unsigned size, uint64_t* ppo);

/**
 * insert a fixed allocation of space
 * @retval 0 on success, will fail with -1, if the space is unavaiable
 */
int memster_insert(struct memster* me, uint64_t addr, unsigned size);

/**
 * acquire a child over allready acquired space
 * @param me, the memster struct
 * @param addr, address to the space we already acquired
 * @param ppo, the memster for the acquired space, destroying it frees the memory
 *             from the parent.
 * @retval, 0 on success
 */
int memster_child(struct memster* me, uint64_t addr, struct memster **ppo);

/**
 * install a callback to be called when the memster is destroyed
 * only one callback is allowed.
 *
 * callback is called after the childred have been destroyed and after the 
 * memory has been released from the parent.
 */
int memster_at_dtor(struct memster* me, void (*pfn)(void*), void *ctx);


/*
 * release some space
 * @param me, the memster struct
 * @param addr, the starting address of the space to release
 * @param size, the size to release
 * @retval, 0 on success, -1 when cant create any more gaps
 */
int memster_release(struct memster* me, uint64_t addr, unsigned size);

#endif //memster_H
