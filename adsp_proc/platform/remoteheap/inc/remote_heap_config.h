#ifndef __REMOTE_HEAP_CFG_H
#define __REMOTE_HEAP_CFG_H


#ifndef RHEAP_VER
#define RHEAP_VER 0x01
#endif  // RHEAP_VER

#ifndef RHEAP_FEATURE_MASK
#define RHEAP_FEATURE_MASK 0x01
#endif  // RHEAP_FEATURE_MASK

#ifndef RHEAP_REMOTE_HEAP_ID
#define RHEAP_REMOTE_HEAP_ID        25
#endif // RHEAP_REMOTE_HEAP_ID

#ifndef RHEAP_GROWTH_SIZE
#define RHEAP_GROWTH_SIZE     3  * 1024 * 1024
#endif // RHEAP_GROWTH_SIZE

#ifndef RHEAP_LOW_HEAP_THRESHOLD  // Please don't modify this, the value is derived from growth size
#define RHEAP_LOW_HEAP_THRESHOLD   450 * 1024
#endif // RHEAP_LOW_HEAP_THRESHOLD

#ifndef RHEAP_HIGH_HEAP_THRESHOLD // Please don't modify this, the value is derived from growth size
#define RHEAP_HIGH_HEAP_THRESHOLD   512 * 1024
#endif // RHEAP_HIGH_HEAP_THRESHOLD

#ifndef CAP_HLOS_HEAP
#define CAP_HLOS_HEAP          6 * 1024 * 1024
#endif // CAP_HLOS_HEAP

#ifndef CAP_DEFAULT_PHYSPOOL
#define CAP_DEFAULT_PHYSPOOL    0
#endif // CAP_DEFAULT_PHYSPOOL


struct remote_heap_cfg_t {
    unsigned int version_number;    // the version of the config structure
    unsigned int feature_mask;      // reserved for future
    unsigned int heap_id;           // used by hlos to allocate memory from correct hlos heap/ion etc.

    unsigned int low_threshold;     // when largest block size in memheap falls below this number,
                                    // a callback is triggered to grow heap
    unsigned int high_threshold;    // when two largest blocks are above this threshold,
                                    // the free block will be relinquished
    unsigned int growth_size;       // the size of block to request from HLOS

    unsigned int cap_hlos_heap;     // if 0, then don't use HLOS heap
    unsigned int cap_default_pool;  // if 0, then don't user default_pool
};


#endif // __REMOTE_HEAP_CFG_H
