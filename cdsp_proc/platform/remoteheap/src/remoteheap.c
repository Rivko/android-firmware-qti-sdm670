#include "AEEQList.h"
#include "AEEstd.h"
#include "amssheap.h"

#define FARF_ERROR 1
#define FARF_HIGH 1
#include "HAP_farf.h"
#include "verify.h"
#include <dlfcn.h>
#include "remoteheap.h"
#include "remote_heap_mgr.h"
#include "remote_heap_config.h"
#include "fastrpc_heap.h"
#include "AEEStdErr.h"

#include "platform_libs.h"
#include "apps_mem.h"
#include "adsp_mmap.h"
#include "uthash.h"
#include <string.h>

#include "qurt_signal.h"
#include "qurt_memory.h"
#include "qurt_types.h"

#define RPCMEM_HEAP_DEFAULT 0x80000000
#define RPCMEM_HEAP_UNCACHED 0x20000000

/* Type definitions*/
static struct remote_heap_cfg_t  rheap_cfg = {
    RHEAP_VER,
    RHEAP_FEATURE_MASK,
    RHEAP_REMOTE_HEAP_ID,

    RHEAP_LOW_HEAP_THRESHOLD,
    RHEAP_HIGH_HEAP_THRESHOLD,
    RHEAP_GROWTH_SIZE,

    CAP_HLOS_HEAP,
    CAP_DEFAULT_PHYSPOOL
};

enum remote_heap_section_type{
    HLOS,
    PHY_POOL
};

struct remote_heap_section_map_t{
    enum remote_heap_section_type src;
    int32 vaddr;
    qurt_mem_region_t region;
    UT_hash_handle    hh; /*Makes this hashable*/
};

struct tzbsp_securechannel_msg{
   uint8 subsystem_id;
   uint32 phy_addr_base;
   uint32 size;
   uint8 auth_key;
   uint8 flags;
};

struct rh_map {
	uint64 vaddr;
	uint64 handle;
	qurt_mem_region_t region;
	QNode qn;
};

QList rh_map_list;

// Macros
#define NUM_PREINIT_HEAPS 5
#define PAGE_SIZE   (4*1024)
#define ALIGN_4K(x) (((x) + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1))
#define MAX_SECTIONS 64


#define TZBSP_EBI1_SECCHANNEL_LPASS_BASE  0x866FE000
#define TZBSP_EBI1_SECCHANNEL_LPASS_END   0x866FF000
#define TZBSP_EBI1_SECCHANNEL_LPASS_SIZE  (TZBSP_EBI1_SECCHANNEL_LPASS_END - \
											TZBSP_EBI1_SECCHANNEL_LPASS_BASE)

typedef unsigned int (*heap_register_callbacks_t)(mem_sections_register_struct
														*sections_register);
typedef unsigned int (*heap_set_thresholds_t)(mem_heap_thresholds *thresholds);
typedef struct heap_register_cb {
	heap_register_callbacks_t heap_register_callbacks;
	heap_set_thresholds_t heap_set_thresholds;
}heap_register_cb_t;

heap_register_cb_t amss_heap_register_cb_ptr = {amss_mem_heap_register_callbacks,
													amss_mem_heap_set_thresholds};

// Static variables
static unsigned long hlos_alloced_bytes = 0;
static unsigned long hlos_freed_bytes = 0;
static unsigned long dpool_alloced_bytes = 0;
static unsigned long dpool_freed_bytes = 0;
static unsigned int bInit = 0;
struct remote_heap_section_map_t remote_heap_sections[MAX_SECTIONS];
struct remote_heap_section_map_t* remote_heap_section_hash = NULL;
static void * pre_init_heaps[NUM_PREINIT_HEAPS] = {&amss_heap_register_cb_ptr
													,0,0,0,0};
static unsigned int v_sec = 0;

// Static Methods
static int pl_memheap_warning_low_func(struct mem_heap_struct *heap_ptr);
static void pl_memheap_section_free_func(struct mem_heap_struct *heap_ptr,
    void *section_start, uint32 section_size);
static void pl_memheap_malloc_failure_func(struct mem_heap_struct *heap_ptr,
    size_t request_size);
static int pl_grow_heap_dpool(void  *heap_ptr, unsigned int size_needed,
    int32* _vaddr, qurt_mem_region_t* _region);
static int pl_grow_heap_hlos(void  *heap_ptr, unsigned int size_needed, int32* _vaddr);
static int pl_release_heap_dpool(void * vaddr, unsigned int size,
    qurt_mem_region_t* region);
static int pl_release_heap_hlos(void *heap_ptr, void * vaddr, unsigned int size);

int pl_get_tz_secure_channel(uint8 *auth_key, uint32 *phys_addr, uint32 *size);
static int pl_map_hlos_tz_secure_channel_heap(void);

extern void mem_heap_add_section(mem_heap_type *heap_ptr, void *section_start,
   unsigned long section_size);
extern int HAP_rfs_wait(void);

#ifdef _DEBUG
// Used to farf out debug data

void * hndl_cfg_so = NULL;
struct remote_heap_cfg_t * (* hndl_get_heap_cfg)(void) = NULL;
struct remote_heap_cfg_t * heap_cfg = NULL;

void remote_heap_debug_msg(void){

    int i=0,j=0;
    struct mem_heap_struct *heap_ptr = pre_init_heaps[0];

    FARF(HIGH, "bInit is %d",  bInit);
    FARF(HIGH, "pre_init_heaps :");
    for(i=0; i<NUM_PREINIT_HEAPS; i++) {
        FARF(HIGH, "[%d] : 0x%p", i, pre_init_heaps[i]);
    }
    FARF(HIGH, "dlopen is %p",  hndl_cfg_so);

    if(hndl_cfg_so){
        FARF(HIGH, "dlsym is %p",  hndl_get_heap_cfg);


        FARF(HIGH, "heap cfg from so");
        FARF(HIGH, "heapid:%x, high_threshold:%d, low_threshold:%d",
                        heap_cfg->heap_id, heap_cfg->high_threshold,
                        heap_cfg->low_threshold);
        FARF(HIGH, "growth_size: %d, cap_hlos_heap:%d", heap_cfg->growth_size,
             heap_cfg->cap_hlos_heap);
    }

    FARF(HIGH, "heap cfg from STATIC");
    FARF(HIGH, "heapid:%x, low_threshold:%d, high_threshold:%d",
                    rheap_cfg.heap_id, rheap_cfg.high_threshold,
                    rheap_cfg.low_threshold);
    FARF(HIGH, "growth_size: %d, cap_hlos_heap:%d cap_default_pool:%d",
                    rheap_cfg.growth_size, rheap_cfg.cap_hlos_heap,
                    rheap_cfg.cap_default_pool);
    FARF(HIGH, "hlos_alloced_bytes: %d, hlos_freed_bytes:%d",
                    hlos_alloced_bytes, hlos_freed_bytes);
    FARF(HIGH, "dpool_alloced_bytes: %d, dpool_freed_bytes:%d",
                    dpool_alloced_bytes, dpool_freed_bytes);

    if(heap_ptr){
        FARF(HIGH, "num sections =%d,total_bytes=%d,used_bytes=%d",
             heap_ptr->number_of_sections,heap_ptr->total_bytes,
             heap_ptr->used_bytes);
        FARF(HIGH, "largest_free_block=%d high_mem_block_size=%d",
             heap_ptr->largest_free_block ,
             heap_ptr->high_mem_block_size);
        for(j = 0; j < heap_ptr->number_of_sections; j++){
            FARF(HIGH, "section=%d size=%d num_used=%d start_addr=%p. ",
                 j, heap_ptr->sections[j].size,heap_ptr->sections[j].num_used,
                 heap_ptr->sections[j].start_addr);
        }
    }

}
#endif /*_DEBUG*/

//Public API
int pl_grow_heap_dynamically(void *_heap_ptr, unsigned int growth_size)
{
    int c = 0;

#ifndef _DEBUG
    void * hndl_cfg_so = NULL;
    struct remote_heap_cfg_t * (* hndl_get_heap_cfg)(void) = NULL;
    struct remote_heap_cfg_t * heap_cfg = NULL;
#endif

    mem_sections_register_struct sec_reg;
    mem_heap_thresholds thresholds;
    unsigned int ret = 0, nErr = AEE_SUCCESS;
    heap_register_cb_t* heap_cb_ptr = (heap_register_cb_t*)_heap_ptr;

    if (!bInit) {
        while (c < NUM_PREINIT_HEAPS && pre_init_heaps[c])
            c++;
        if (c >= NUM_PREINIT_HEAPS)
            goto bail; // retry because we don't support
                       // more than NUM_PREINIT_HEAPS heaps until fastrpc is up

        pre_init_heaps[c] = _heap_ptr;
        return 0; // We will add the heap later
     } else if (bInit == 1) {
        hndl_cfg_so = dlopen("remote_heap_config.so", RTLD_NOW);
        if (hndl_cfg_so) {
            hndl_get_heap_cfg = dlsym(hndl_cfg_so, "get_heap_cfg");
            if (hndl_get_heap_cfg) {
                heap_cfg = hndl_get_heap_cfg();
                if (heap_cfg) {
                    memcpy(&rheap_cfg, heap_cfg, sizeof(rheap_cfg));
                }
            }
        }
        bInit = 2;
    }

    sec_reg.mem_low_func_ptr = pl_memheap_warning_low_func;
    sec_reg.sect_free_func_ptr = pl_memheap_section_free_func;
    //sec_reg.mem_alloc_failed_func_ptr = pl_memheap_malloc_failure_func;
    sec_reg.fail_fnc_ptr = pl_memheap_malloc_failure_func;

    VERIFY(AEE_SUCCESS == (ret =
      		heap_cb_ptr->heap_register_callbacks(&sec_reg)));

    // Set threshold values
    thresholds.low_largest_block_size = rheap_cfg.low_threshold;
    thresholds.high_largest_block_size = rheap_cfg.high_threshold;
    heap_cb_ptr->heap_set_thresholds(&thresholds);
    return 0;

bail:
    FARF(ERROR, "pl_grow_heap_dynamically. Unable to grow dynamically. heap:%p",
    		heap_cb_ptr);
    return -1;

} // pl_grow_heap_dynamically

int pl_grow_heap(void *heap_ptr,unsigned int size_needed){

    int nErr = AEE_SUCCESS;
    struct remote_heap_section_map_t* section = NULL;
    int i = 0;
    int32 vaddr = 0;

    // Ensure that there is an empty spot where we can add this section
    if(((struct mem_heap_struct *)heap_ptr)->number_of_sections ==
        MEM_HEAP_SECTIONS_MAX - 1){
         FARF(ERROR, "Grow heap failed - No more free sections available.");
         return E_NO_MORE_FREE_SECTIONS;
    }

    // Find first empty element in the array
    // so we can store the address of the added section
    for(i=0;i<MAX_SECTIONS;i++){
        if (remote_heap_sections[i].vaddr == (int32)NULL){
            section = &remote_heap_sections[i];
            break;
        }
    }

    // If we cannot get an empty element to store the address of the
    // section to be added, error out.
    VERIFYC(section != NULL, AEE_ENOFREESECTION);

    nErr = -1;
    // Determine whether to use hlos or default pool
    if (rheap_cfg.cap_default_pool &&
        (ALIGN_4K(size_needed) + dpool_alloced_bytes - dpool_freed_bytes <=
            rheap_cfg.cap_default_pool) )
    {
        FARF(HIGH, "Growing heap from physpool");
        nErr = pl_grow_heap_dpool(heap_ptr, ALIGN_4K(size_needed), &vaddr,
            &section->region);
        section->src = PHY_POOL;
    }

    if (nErr != AEE_SUCCESS)
    {
        // Check for hlos cap is done inside the hlos func.
        FARF(HIGH, "Growing heap from hlos");
        nErr = pl_grow_heap_hlos(heap_ptr, size_needed,&vaddr);
        section->src = HLOS;
    }

    if (nErr == AEE_SUCCESS){
        section->vaddr = vaddr;
        FARF(HIGH, "Adding section to list %p",vaddr);
        HASH_ADD_INT(remote_heap_section_hash, vaddr, section);
        FARF(HIGH, "Added section to list");
    }
bail:
	if (nErr != AEE_SUCCESS)
		FARF(ERROR, "Error %x: pl_grow_heap failed", nErr);
    return nErr;
}
int pl_release_heap(void *heap_ptr, void * vaddr, unsigned int size)
{
    int nErr = AEE_SUCCESS;
    struct remote_heap_section_map_t* section = NULL;
    int32 addr = (int32) vaddr;

    FARF(HIGH, "Calling release for vaddr = %p", addr);
    HASH_FIND_INT(remote_heap_section_hash, &addr, section);
    VERIFYC(section, AEE_ENOFREESECTION);
    switch(section->src){
        case HLOS:
            FARF(HIGH, "Releasing heap from HLOS");
            nErr = pl_release_heap_hlos(heap_ptr, vaddr, size);
            break;
        case PHY_POOL:
            FARF(HIGH, "Releasing heap from physpool");
            nErr = pl_release_heap_dpool(vaddr, size, &section->region);
            break;
    }
    if(nErr == AEE_SUCCESS){
        HASH_DEL(remote_heap_section_hash,section);
        section->vaddr = (int32)NULL;
    }

bail:
    if(nErr != AEE_SUCCESS)
		FARF(ERROR, "Error %x: Couldnt find element in hash", nErr);
    return nErr;
}

static int pl_memheap_warning_low_func(struct  mem_heap_struct *heap_ptr){

    // Memory is low for this heap. Get some memory from HLOS and add a
    // section to this heap. But we may be called inside a heap mutex, so lets
    // signal our hlos maping thead to add
    // a section for this heap
    struct remote_heap_mgr_work_item item;
    item.cmd = GROW_HEAP;
    item.ptr.heap_ptr = heap_ptr;
    item.size = rheap_cfg.growth_size;
    item.sleep_timer = 0;

    remote_heap_mgr_enqueue_cmd(&item);
    return 0;
}

static void pl_memheap_section_free_func(struct mem_heap_struct *heap_ptr,
    void *section_start, uint32 section_size){

    struct remote_heap_mgr_work_item item;
    item.cmd = SHRINK_HEAP;
    item.ptr.vaddr = section_start;
    item.size = section_size;
    item.sleep_timer = 0;

    FARF(LOW, "pl_memheap_section_free_func. Queuing free request");
    FARF(LOW, "-- low: %d, high:%d, largest: %d",
            heap_ptr->low_mem_block_size,
            heap_ptr->high_mem_block_size,
            heap_ptr->largest_free_block);
    FARF(LOW, "-- sec 0: %d, %d  sec 1: %d, %d",
            heap_ptr->sections[0].size,
            heap_ptr->sections[0].num_used,
            heap_ptr->sections[1].size,
            heap_ptr->sections[1].num_used );
    remote_heap_mgr_enqueue_cmd(&item);
}


static int pl_grow_heap_dpool(void  *heap_ptr, unsigned int size_needed,
    int32* _vaddr, qurt_mem_region_t* _region){

    int nErr = AEE_SUCCESS, nQErr = AEE_SUCCESS;
    unsigned int vaddr = 0;
    int nSize = 0;
    qurt_mem_region_attr_t attr;

    VERIFYC(_vaddr != NULL, AEE_EMEMPTR);
    VERIFYC(_region != NULL, AEE_EMEMPTR);
    nSize = ALIGN_4K(size_needed);

    FARF(HIGH, "dpool size_needed:%d. heap_ptr:%p",nSize, heap_ptr);
    FARF(HIGH, "dpool total alloced:%lx  freed:%lx diff:%u",
        dpool_alloced_bytes, dpool_freed_bytes,
        (dpool_alloced_bytes-dpool_freed_bytes));

    // if current size + alloced - freed > cap then don't allocate anymore
    if (nSize + dpool_alloced_bytes - dpool_freed_bytes >
        rheap_cfg.cap_default_pool) {
        FARF(ERROR,
             "CAP (%u) reached, cannot allocate any more physpool mem",
             rheap_cfg.cap_default_pool);
        return E_DPOOL_CAP_REACHED;
    }

   qurt_mem_region_attr_init(&attr);
   VERIFYC(QURT_EOK == (nQErr = qurt_mem_region_create( _region, nSize,
          qurt_mem_default_pool, &attr)), AEE_EQURTREGIONCREATE);

   VERIFYC(QURT_EOK == (nQErr = qurt_mem_region_attr_get( *_region, &attr )), AEE_EQURTREGIONGETATTR);
   qurt_mem_region_attr_get_virtaddr( &attr, &vaddr );

    FARF(HIGH, "pl_grow_heap_dpool: Adding section vaddr:%p, size:%d", vaddr,
         attr.size);
    mem_heap_add_section((mem_heap_type *) heap_ptr, (void *)vaddr,
        (unsigned long)attr.size);
    dpool_alloced_bytes += attr.size;

    FARF(HIGH, "pl_grow_heap_dpool: Section added successfully");
    FARF(LOW, "low: %d, high:%d, largest: %d",
            ((mem_heap_type *)heap_ptr)->low_mem_block_size,
            ((mem_heap_type *)heap_ptr)->high_mem_block_size,
            ((mem_heap_type *)heap_ptr)->largest_free_block);

bail:
   *_vaddr = vaddr;
   if(nErr != AEE_SUCCESS)
	   FARF(ERROR, "Error %x(%x): pl_grow_heap_dpool failed\n", nErr, nQErr);
   return (nErr == 0 ? nErr: E_APPS_BUSY_RETRY_LATER);
}

int grow_remote_heap(size_t size, uint64 *_vaddr) {
	qurt_addr_t vaddr;
	uint64_t vapps, handle = 0;
    qurt_mem_pool_t pool;
	qurt_mem_region_attr_t attr;
	qurt_mem_region_t region = 0;
	size_t growSize = size;
	qurt_paddr_64_t paddr;
	struct rh_map *map = NULL;
	int nErr = AEE_SUCCESS, nQErr = AEE_SUCCESS;

	(void)vapps;

retry:
    VERIFYC(QURT_EOK == (nQErr = qurt_mem_pool_attach("HLOS_PHYSPOOL", &pool)), AEE_EQURTMEMPOOLATTACH);
    qurt_mem_region_attr_init(&attr);
    qurt_mem_region_attr_set_mapping(&attr, QURT_MEM_MAPPING_VIRTUAL_RANDOM);
    if (QURT_EOK == qurt_mem_region_create(&region, size, pool, &attr)) {
    	qurt_mem_region_attr_get(region, &attr);
    	qurt_mem_region_attr_get_physaddr_64(&attr, &paddr);
    }
	if(region == 0) {
#ifdef REMOTE_HEAP_XPU
	   VERIFY(AEE_SUCCESS == apps_mem_request_map64(0, (RPCMEM_HEAP_DEFAULT | RPCMEM_HEAP_UNCACHED),
			   ADSP_MMAP_HEAP_ADDR, 0, growSize, &vapps, &handle));
#else
	   VERIFY(AEE_SUCCESS == apps_mem_request_map64(0, (RPCMEM_HEAP_DEFAULT | RPCMEM_HEAP_UNCACHED),
	   			   ADSP_MMAP_ADD_REMOTE_PAGES, 0, growSize, &vapps, &handle));
#endif
	   FARF(LOW, "apps_mem_request_map64 returned vapps = %llx, vadsp = %llx", vapps, handle);
	   goto retry;
	}
	qurt_mem_region_attr_get(region, &attr);
	qurt_mem_region_attr_get_virtaddr(&attr, &vaddr);
	VERIFYC(NULL != (map = rpc_malloc(sizeof(*map))), AEE_ENOMEMORY);
	map->handle = handle;
	map->vaddr = vaddr;
	map->region = region;
	QList_AppendNode(&rh_map_list, &map->qn);
	*_vaddr = vaddr;
bail:
   if(nErr != AEE_SUCCESS)
	   FARF(ERROR, "Error %x(%x): grow_remote_heap failed\n", nErr, nQErr);
   return nErr;
}

static int pl_grow_heap_hlos(void  *heap_ptr, unsigned int size_needed,
    int32* _vaddr){

    int nErr = AEE_SUCCESS;
    uint64 vaddr=0;

    VERIFYC(_vaddr != NULL, AEE_EMEMPTR);

    FARF(HIGH, "hlos size_needed:%d. heap_ptr:%p",size_needed, heap_ptr);
    FARF(HIGH, "hlos total alloced:%lx  freed:%lx diff:%u",
        hlos_alloced_bytes, hlos_freed_bytes,
        (hlos_alloced_bytes-hlos_freed_bytes));

    // if current size + alloced - freed > cap then don't allocate anymore
    if (size_needed + hlos_alloced_bytes - hlos_freed_bytes >
        rheap_cfg.cap_hlos_heap) {
        FARF(ERROR,
             "-- pl grow- CAP (%u) reached, cannot allocate any more hlos mem",
             rheap_cfg.cap_hlos_heap);
        return E_HLOS_CAP_REACHED;
    }

    VERIFY(AEE_SUCCESS == grow_remote_heap(size_needed, &vaddr));
    FARF(HIGH, "pl_grow_heap_hlos: Adding section vaddr:%llx, size:%d", vaddr,
         size_needed);
    mem_heap_add_section((mem_heap_type *) heap_ptr, (void *)vaddr,
        (unsigned long)size_needed);
    hlos_alloced_bytes += size_needed;

    FARF(HIGH, "pl_grow_heap_hlos: Section added successfully");
    FARF(LOW, "low: %d, high:%d, largest: %d",
            ((mem_heap_type *)heap_ptr)->low_mem_block_size,
            ((mem_heap_type *)heap_ptr)->high_mem_block_size,
            ((mem_heap_type *)heap_ptr)->largest_free_block);

bail:
   *_vaddr = vaddr;
   if(nErr)
	   FARF(ERROR, "Error %x : pl_grow_heap_hlos failed", nErr);
   return (nErr == 0 ? nErr: E_APPS_BUSY_RETRY_LATER);
}

static int pl_map_hlos_tz_secure_channel_heap(void)
{
   int nErr = AEE_SUCCESS, nQErr = AEE_SUCCESS;
   if(v_sec != 0) {
   		return 0;
   	}
   qurt_mem_region_t rgn = 0;
   qurt_mem_region_attr_t attr;

   qurt_mem_region_attr_init(&attr);
   qurt_mem_region_attr_set_mapping(&attr,QURT_MEM_MAPPING_NONE);

   VERIFYC(QURT_EOK == (nQErr = qurt_mem_region_create(&rgn,
		   TZBSP_EBI1_SECCHANNEL_LPASS_END - TZBSP_EBI1_SECCHANNEL_LPASS_BASE,
               qurt_mem_default_pool, &attr)), AEE_EQURTREGIONCREATE);
   VERIFYC(QURT_EOK == (nQErr = qurt_mem_region_attr_get(rgn, &attr)), AEE_EQURTREGIONGETATTR);
   qurt_mem_region_attr_get_virtaddr(&attr, &v_sec);

   VERIFYC(QURT_EOK == (nQErr = qurt_mapping_create(v_sec, TZBSP_EBI1_SECCHANNEL_LPASS_BASE,
		   TZBSP_EBI1_SECCHANNEL_LPASS_END - TZBSP_EBI1_SECCHANNEL_LPASS_BASE,
		   QURT_MEM_CACHE_NONE,
		   QURT_PERM_READ)), AEE_EQURTREGIONCREATE);
   FARF(HIGH, "pl_map_verify_hlos_tz_secure_channel_heap. Secure channel mapped on dsp");
   {
	   struct tzbsp_securechannel_msg * msg = (struct tzbsp_securechannel_msg * )
	   												((unsigned int)v_sec + 0x64);
	   FARF(HIGH, "pl_map_hlos_tz_secure_channel_heap auth_key = %d, phys_addr = 0x%x, size = %d",
			   	   	   	   	   	   	   	   	   	   msg->auth_key, msg->phy_addr_base, msg->size);
   }
bail:
   if(nErr != AEE_SUCCESS)
	   FARF(ERROR, "Error %x(%x): pl_map_hlos_tz_secure_channel_heap failed\n", nErr, nQErr);
   return nErr;
}

static int pl_release_heap_dpool(void * vaddr, unsigned int size,
    qurt_mem_region_t* region){

    int nErr = AEE_SUCCESS;
    VERIFY(region != NULL);
    FARF(HIGH, "pl_release_heap_dpool. unmapping: %p, size:%d", vaddr, size);

    VERIFYC(QURT_EOK == qurt_mem_region_delete(*region), AEE_EQURTREGIONDELETE);
    region = NULL;

    FARF(LOW, "pl_release_heap_dpool. ref released");

    dpool_freed_bytes += size;
bail:
   if(nErr != AEE_SUCCESS)
	   FARF(ERROR, "Error %x: pl_release_heap_dpool failed\n", nErr);
    return nErr;
}

int shrink_remote_heap(uint64 vaddr, size_t size) {
	struct rh_map *map;
	qurt_mem_region_t region = 0;
	uint64 handle = 0;
	QNode *pn;
	int nErr = AEE_SUCCESS;
    QLIST_FOR_ALL(&rh_map_list, pn){
       map = STD_RECOVER_REC(struct rh_map, qn, pn);
       if(map->vaddr == vaddr) {
    	  region = map->region;
    	  handle = map->handle;
          break;
       }
    }
    VERIFY(region);
    VERIFY(QURT_EOK == qurt_mem_cache_clean((qurt_addr_t)vaddr, (qurt_size_t)size, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE));
    FARF(LOW, "shrink_remote_heap qurt_mem_pool_attach size = %x VADDR = %lx, handle = %llx, region = %x", size, (qurt_addr_t)vaddr, handle, region);
    VERIFYC(AEE_SUCCESS == (nErr = qurt_mem_region_delete((qurt_mem_region_t)region)), AEE_EQURTREGIONDELETE);
    QNode_Dequeue(&map->qn);
    rpc_free(map);
    VERIFY(AEE_SUCCESS == (nErr = HAP_rfs_wait()));
	if(apps_mem_request_unmap64(handle, size)) {
		FARF(ERROR, "Error %x: shrink_remote_heap apps_mem_request_unmap64 failed\n", nErr);
		hlos_freed_bytes += size;
	} else {
		hlos_freed_bytes += size;
	}

bail:
	if(nErr != AEE_SUCCESS) {
		FARF(ERROR, "Error %x: shrink_remote_heap failed\n", nErr);
		//nErr = -1;
	}
	return nErr;
}

static int pl_release_heap_hlos(void *heap_ptr, void * vaddr, unsigned int size){

    int nErr = AEE_SUCCESS;
    FARF(HIGH, "pl_release_heap_hlos. unmapping: %p, size:%d", vaddr, size);
    VERIFY(AEE_SUCCESS == (nErr = shrink_remote_heap((uint64)vaddr, size)));
	//hlos_freed_bytes += size;

bail:
	if(nErr != AEE_SUCCESS) {
		FARF(ERROR, "Error %x: pl_release_heap_hlos failed", nErr);
	}
	return nErr;
}

int pl_get_tz_secure_channel(uint8 *auth_key, uint32 *phys_addr, uint32 *size)
{
	int nErr = AEE_SUCCESS;
	if(v_sec == 0) {
		VERIFY(AEE_SUCCESS == (nErr = pl_map_hlos_tz_secure_channel_heap()));
	}
	struct tzbsp_securechannel_msg * msg = (struct tzbsp_securechannel_msg * )
												((unsigned int)v_sec + 0x64);
	*auth_key = msg->auth_key;
	*phys_addr = msg->phy_addr_base;
	*size = msg->size;
	return 0;
bail:
	if(nErr != AEE_SUCCESS)
		FARF(ERROR, "Error %x: pl_get_tz_secure_channel_auth_key. Invalid \
			secure channel memory", nErr);
	return nErr;
}


static void pl_memheap_malloc_failure_func(struct mem_heap_struct *heap_ptr,
    size_t request_size){

    remote_heap_mgr_handle_failure_cb_handler(heap_ptr,request_size,
        rheap_cfg.growth_size);
}

PL_DEP(fastrpc_smd);
PL_DEP(remote_heap_mgr);

int remoteheap_Init(void)
{
    int nErr = AEE_SUCCESS;
    int c=0;
    VERIFY(AEE_SUCCESS == PL_INIT(fastrpc_smd));
    VERIFY(AEE_SUCCESS == PL_INIT(remote_heap_mgr));

    QList_Ctor(&rh_map_list);
    memset(remote_heap_sections,
           (int32)NULL,sizeof(struct remote_heap_section_map_t)*MAX_SECTIONS);
    struct remote_heap_mgr_work_item item;
    bInit = 1; // We are good to go for servicing heap growths
    //FARF(ERROR, "pl_grow_heap_dynamically.REMOTEHEAP INIT");
    while (c < NUM_PREINIT_HEAPS && pre_init_heaps[c]) {
        item.cmd = INIT_REMOTEHEAP;
        item.ptr.heap_cb_ptr= pre_init_heaps[c];
        item.size = 0;
        item.sleep_timer = 0;

        //FARF(ERROR, "pl_grow_heap_dynamically. Commands being enqueud %d",  c);
        remote_heap_mgr_enqueue_cmd(&item);

        c++;
    }

bail:
    return nErr;
}

void remoteheap_Deinit(void)
{
    PL_DEINIT(remote_heap_mgr);
    PL_DEINIT(fastrpc_smd);
}
PL_DEFINE(remoteheap, remoteheap_Init, remoteheap_Deinit);
