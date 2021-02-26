/*========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
  ====================================================================== */
#ifndef FASTRPC_INVOKE_H
#define FASTRPC_INVOKE_H

#include "remote64.h"
#include "qurt.h"
#include "AEEStdDef.h"
#include "AEEQList.h"
#include "fastrpc_port.h"
#include "fastrpc_internal.h"
#include "HAP_farf.h"
#include "verify.h"

#define ADSP_MMAP_CPZ         0x2000

struct fastrpc_invoke_process;
struct fastrpc_mem;
struct static_map;

struct cache_entry {
   qurt_mem_cache_mode_t in;
   qurt_mem_cache_mode_t rout;
};

struct fastrpc_invoke_thread {
   struct fastrpc_invoke_process* pinfo;
   struct fastrpc_mem *iregion;
   struct fastrpc_mem *hdr;
   uint32_t inval;
   void* buf;
   int bufSize;
   uint32_t sc;
   remote_arg* lpra;
   remote_arg64* rpra;
   int nStaticMaps;
   void (*rsp_fn)(void* ctx, int ret);
   void* rsp_ctx;
};

#define PAGE_SIZE  0x1000
#define PAGE_MASK  ~(PAGE_SIZE-1)

static __inline void* uint64_to_ptr(uint64_t pv) {
   void* rv = (void*)(uintptr_t)pv;
   if((uint64_t)(uintptr_t)rv != pv) {
      FARF(MEDIUM, "uint64_t to void* conversion error %p %llx", rv, pv);
   }
   return rv;
}
static __inline uint32_t uint64_to_uint32(uint64_t pv) {
   uint32_t rv = (uint32)pv;
   if((uint64_t)rv != pv) {
      FARF(MEDIUM, "uint64_t to uint32_t conversion error");
   }
   return rv;
}
static __inline unsigned uint64_to_unsigned(uint64_t pv) {
   unsigned rv = (unsigned)pv;
   if((uint64_t)rv != pv) {
      FARF(MEDIUM, "uint64_t to unsigned conversion error");
   }
   return rv;
}
static __inline uintptr_t uint64_to_uintptr(uint64_t pv) {
   uintptr_t rv = (uintptr_t)pv;
   if((uint64_t)rv != pv) {
      FARF(MEDIUM, "uint64_t to uintptr_t conversion error");
   }
   return rv;
}
static __inline int uint64_to_int(uint64_t pv) {
   int rv = (int)pv;
   if((uint64_t)rv != pv) {
      FARF(MEDIUM, "uint64_t to int conversion error");
   }
   return rv;
}
static __inline int int64_to_int(int64_t pv) {
   int rv = (int)pv;
   if((int64_t)rv != pv) {
      FARF(MEDIUM, "int64 to int conversion error");
   }
   return rv;
}
static __inline qurt_addr_t uint64_to_qurt_addr(uint64_t pv) {
   qurt_addr_t rv = (qurt_addr_t)pv;
   if((uint64_t)rv != pv) {
      FARF(MEDIUM, "64 to qurt_addr_t conversion error");
   }
   return rv;
}
static __inline uint64_t qurt_addr_to_uint64(qurt_addr_t pv) {
   uint64_t rv = (uint64_t)pv;
   if((qurt_addr_t)rv != pv) {
      FARF(MEDIUM, "64 to qurt_addr_t conversion error");
   }
   return rv;
}
static __inline qurt_addr_t ptr_to_qurt_addr(void* pv) {
   qurt_addr_t rv = (qurt_addr_t)pv;
   if((void*)rv != pv) {
      FARF(MEDIUM, "ptr to qurt_addr_t conversion error");
   }
   return rv;
}
static __inline uint64_t ptr_to_uint64(void* pv) {
   C_ASSERT(sizeof(uint64_t) >= sizeof(void*));
   return (uint64_t)(uintptr_t)pv;
}

static __inline uint64_t buf_page_start(uint64_t buf) {
   uint64_t start = buf & PAGE_MASK;
   return start;
}
static __inline uint64_t buf_page_start_ptr(void* pv) {
   return buf_page_start((uintptr_t)pv);
}

static __inline uint64_t buf_page_offset(uint64_t buf) {
   uint64_t offset = buf & (PAGE_SIZE-1);
   return offset;
}
static __inline uint64_t buf_page_offset_ptr(void* pv) {
   return buf_page_offset((uintptr_t)pv);
}

static __inline uint64_t buf_page_size(uint64_t size) {
   uint64_t sz = (size + (PAGE_SIZE - 1)) & PAGE_MASK;
   return sz;
}

static __inline uint64_t buf_page_aligned_size(uint64_t buf, uint64_t size) {
   uint64_t end = buf + size + (PAGE_SIZE-1);
   uint64_t sz = buf_page_start(end) - buf_page_start(buf);
   return sz;
}

static __inline uint64_t buf_page_aligned_size_ptr(void *buf, uint32_t size) {
   return buf_page_aligned_size(ptr_to_uint64(buf), size);
}


int fastrpc_get_cache_attr(struct fastrpc_invoke_process* pinfo, boolean bInBuf, qurt_mem_cache_mode_t* pattr);
int fastrpc_set_cache_attr(struct fastrpc_invoke_process* pinfo, boolean bInBuf, qurt_mem_cache_mode_t attr);

int fastrpc_invoke_thread_init(struct fastrpc_invoke_process* pinfo, 
                               struct fastrpc_invoke_thread* tn,
                               void (*rsp_fn)(void* ctx, int ret),
                               void* ctx);

void fastrpc_invoke_thread_deinit(struct fastrpc_invoke_thread* me);

void fastrpc_invoke_dispatch(struct fastrpc_invoke_thread* tn,
                             struct fastrpc_port_msg* msg);

int fastrpc_invoke_mmap_create32(struct fastrpc_invoke_process* pinfo,
                                 uint32_t flags, uint32_t virt, struct smq_phy_page *pgstart,
                                 int numPages, uint32* vaddrout);
int fastrpc_invoke_mmap_create(struct fastrpc_invoke_process* pinfo,
                               uint32_t flags, uint64_t virt, struct smq_phy_page *pgstart,
                               int numPages, uint64* vaddrout);
int fastrpc_invoke_mmap_addref(struct fastrpc_invoke_process* pinfo,
                               void* buf, int bufLen, uint32_t flags);
int fastrpc_invoke_mmap_release(struct fastrpc_invoke_process* pinfo,
                                void* buf, int bufLen);
int fastrpc_invoke_mmap_get_cpz_phys(struct fastrpc_invoke_process* pinfo, int size, uint64_t *phys);
int fastrpc_invoke_mmap_destroy(struct fastrpc_invoke_process* pinfo,
								uint64 buf, int64 bufLen, uint32_t *flags);

int fastrpc_invoke_process_create(struct fastrpc_invoke_process **po);
void fastrpc_invoke_process_destroy(struct fastrpc_invoke_process *po);
int fastrpc_invoke_process_setup_mem(struct fastrpc_invoke_process *me, uint64_t size);

int fastrpc_mapping_create(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size, qurt_mem_cache_mode_t cache, qurt_perm_t perm);
int fastrpc_mapping_remove(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size);
int fastrpc_invoke_add_mem(struct fastrpc_invoke_process *me, struct smq_phy_page *page);
int fastrpc_invoke_fd_mmap_get(struct fastrpc_invoke_process* info, int fd,
                               void **vaddr, uint64 *paddr);
int fastrpc_invoke_fd_mmap_put(struct fastrpc_invoke_process* info, int fd);
int fastrpc_invoke_fd_getinfo(struct fastrpc_invoke_process* info, int fd, uint32_t *len);
void* fastrpc_invoke_fd_mmap_create(struct fastrpc_invoke_process* info,
                                  int fd, size_t len, int flags, off_t offset);
int fastrpc_invoke_fd_mmap_destroy(struct fastrpc_invoke_process* info,
                                  void *addr, size_t len);

#endif
