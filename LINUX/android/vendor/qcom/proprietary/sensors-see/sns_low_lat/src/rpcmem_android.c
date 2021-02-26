/**********************************************************************
* Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif //VERIFY_PRINT_ERROR
#include "verify.h"
#include "rpcmem.h"
#include "AEEQList.h"
#include "AEEstd.h"
#include "remote.h"
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <android/log.h>

#define ION_HEAP_ID_SYSTEM_CONTIG 21
#define ION_HEAP_ID_ADSP 22
#define ION_HEAP_ID_PIL1 23
#define ION_HEAP_ID_SYSTEM 25

#define HEAP_ID_TO_MASK(bit) (1 << (bit))

#define ION_FLAG_CACHED 1
#define SET_FLAG(__cache, __flag)		((__cache) | (__flag))

typedef int ion_user_handle_t;

struct ion_allocation_data_kk {
	size_t len;
	size_t align;
	unsigned int heap_id_mask;
	unsigned int flags;
	ion_user_handle_t handle;
};

struct ion_allocation_data_ics {
	size_t len;
	size_t align;
	unsigned int flags;
	ion_user_handle_t handle;
};

struct ion_fd_data {
	ion_user_handle_t handle;
	int fd;
};
struct ion_handle_data {
	ion_user_handle_t handle;
};

#define ION_IOC_MAGIC 'I'
#define ION_IOC_ALLOC_KK _IOWR(ION_IOC_MAGIC, 0, struct ion_allocation_data_kk)
#define ION_IOC_ALLOC_ICS _IOWR(ION_IOC_MAGIC, 0, struct ion_allocation_data_ics)
#define ION_IOC_FREE _IOWR(ION_IOC_MAGIC, 1, struct ion_handle_data)
#define ION_IOC_MAP _IOWR(ION_IOC_MAGIC, 2, struct ion_fd_data)

#define ITRANSPORT_PREFIX "'\":;./\\"

/**
 * this heap id is used when -1 is passed to alloc.
 * users should change this to whatever heap id they expect.
 */
static QList lst;
static pthread_mutex_t mt;

struct mmap_info {
   QNode qn;
   int32_t fd;
   void *paddr;
   int bufsize;
   uint32 flags;
   struct ion_fd_data data;
};

static remote_handle remote_handle_fd;

static int rpcmem_contig_alloc(struct mmap_info *h, uint32 heap_mask, uint32 flags, int size, void **ppo);
static int rpcmem_contig_alloc_ics(struct mmap_info *h, uint32 heap_mask, int size, void **ppo);

void rpcmem_init() {
#ifdef ADSPRPC
   __android_log_print(ANDROID_LOG_INFO, "rpcmem_init:open remote handle for sensorspd", "");
   if (!remote_handle_fd) {
     if (remote_handle_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd", &remote_handle_fd)){
       __android_log_print(ANDROID_LOG_ERROR, "rpcmem_init:Failed to open remote handle for sensorspd", "");
     } else {
        __android_log_print(ANDROID_LOG_INFO, "rpcmem_init:opened remote handle for sensorspd", "");
         remote_handle remote_handle_fd = 1;
     }
   } else {
       __android_log_print(ANDROID_LOG_INFO, "rpcmem_init:using opened remote handle for sensorspd", "");
   }
#endif
   QList_Ctor(&lst);
   pthread_mutex_init(&mt, 0);
}


void rpcmem_deinit() {
   pthread_mutex_destroy(&mt);
}

extern void remote_register_buf(void* buf, int size, int fd);
#pragma weak  remote_register_buf

void register_buf(void* buf, int size, int fd) {
   __android_log_print(ANDROID_LOG_INFO, "rpcmem: edge:", "remote_register_buf: %p", remote_register_buf);
   /*fastrpc channel on ADSP needs below setting for SMMU*/
#ifdef ADSPRPC
   __android_log_print(ANDROID_LOG_INFO, "open remote handle for sensorspd", "");
   if (!remote_handle_fd) {
     if (remote_handle_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd", &remote_handle_fd)){
       __android_log_print(ANDROID_LOG_ERROR, "Failed to open remote handle for sensorspd", "");
     } else {
        __android_log_print(ANDROID_LOG_INFO, "opened remote handle for sensorspd", "");
         remote_handle remote_handle_fd = 1;
     }
   } else {
       __android_log_print(ANDROID_LOG_INFO, "using opened remote handle for sensorspd", "");
   }
#endif
   if(remote_register_buf) {
      remote_register_buf(buf, size, fd);
   }
}

void* rpcmem_alloc(int heapid, uint32 flags, int size) {
   int nErr = 0;
   struct mmap_info *m = 0;
   void* po = 0;
   uint32 heap_mask;
   uint32 ion_flags;
   VERIFY(0 != (m = malloc(sizeof(*m))));
   QNode_CtorZ(&m->qn);

   //! default flags should be the same as ion cached
   C_ASSERT(ION_FLAG_CACHED == RPCMEM_DEFAULT_FLAGS);

   ion_flags = flags & ~RPCMEM_HEAP_UNCACHED & ~RPCMEM_HEAP_DEFAULT & ~RPCMEM_HEAP_NOREG;
   //! convert from deprecated flags
   if(flags & RPCMEM_HEAP_DEFAULT) {
      heapid = RPCMEM_DEFAULT_HEAP;
      if(!(flags & RPCMEM_HEAP_UNCACHED)) {
         ion_flags = SET_FLAG(ion_flags, ION_FLAG_CACHED);
      }
   }
   if(!(flags & RPCMEM_HEAP_DEFAULT)) {
      VERIFY(!(flags & RPCMEM_HEAP_UNCACHED));
   }
   if(heapid == RPCMEM_DEFAULT_HEAP) {
      heap_mask = HEAP_ID_TO_MASK(ION_HEAP_ID_ADSP);
      nErr = rpcmem_contig_alloc(m, heap_mask, ion_flags, size, &po);
      if(0 != nErr) {
         heap_mask = HEAP_ID_TO_MASK(ION_HEAP_ID_SYSTEM);
         VERIFY(0 == (nErr = rpcmem_contig_alloc(m, heap_mask, ion_flags, size, &po)));
      }
   } else {
      heap_mask = HEAP_ID_TO_MASK(heapid);
      nErr = rpcmem_contig_alloc(m, heap_mask, flags, size, &po);
      if(0 != nErr) {
         VERIFY_EPRINTF("rpcmem jb allocation error: %d: trying ics\n", nErr);
         VERIFY(0 == (nErr = rpcmem_contig_alloc_ics(m, heap_mask, size, &po)));
      }
   }
   pthread_mutex_lock(&mt);
   QList_AppendNode(&lst, &m->qn);
   pthread_mutex_unlock(&mt);
   m->flags = flags;
   if(!(flags & RPCMEM_HEAP_NOREG)) {
      register_buf(m->paddr, m->bufsize, m->data.fd);
   }
bail:
   if(nErr && m) {
      free(m);
      po = 0;
   }
   return po;
}

static int rpcmem_contig_free(struct mmap_info *h, void *po);


void rpcmem_free(void* po) {
   int nErr = 0;
   struct mmap_info *m, *mfree = 0;
   QNode* pn, *pnn;
   pthread_mutex_lock(&mt);
   QLIST_NEXTSAFE_FOR_ALL(&lst, pn, pnn) {
      m = STD_RECOVER_REC(struct mmap_info, qn, pn);
      if(m->paddr == po) {
         mfree = m;
         QNode_Dequeue(&m->qn);
         break;
      }
   }
   pthread_mutex_unlock(&mt);
   VERIFY(mfree);
   if(!(mfree->flags & RPCMEM_HEAP_NOREG)) {
      register_buf(mfree->paddr, mfree->bufsize, -1);
   }
   rpcmem_contig_free(mfree, po);
   free(mfree);

bail:
   return;
}

int rpcmem_to_fd(void* po) {
   struct mmap_info *m;
   int fd = -1;
   QNode* pn;
   pthread_mutex_lock(&mt);
   QLIST_FOR_ALL(&lst, pn) {
      m = STD_RECOVER_REC(struct mmap_info, qn, pn);
      if(STD_BETWEEN(po, m->paddr, (uintptr_t)m->paddr + m->bufsize)) {
         fd = m->data.fd;
         break;
      }
   }
   pthread_mutex_unlock(&mt);

   return fd;
}

static int rpcmem_contig_free(struct mmap_info *h, void *po) {
   struct mmap_info *m = (struct mmap_info *)h;
   (void)po;
   if(m->fd) {
      int size = (m->bufsize + 4095) & (~4095);
      if(m->paddr) {
         munmap(m->paddr, size);
         m->paddr = 0;
      }
      if(m->data.fd) {
         close(m->data.fd);
         m->data.fd = 0;
      }
      if(m->data.handle) {

         ioctl(m->fd, ION_IOC_FREE, &m->data);
         m->data.handle = 0;
      }
      close(m->fd);
      m->fd = 0;
   }
   return 0;
}

static int rpcmem_contig_alloc(struct mmap_info *h, uint32 heap_mask, uint32 flags, int size, void **ppo) {
   static uint32_t align[] = {
      0x100000, 0x40000, 0x10000, 0x4000, 0x1000
   };
   struct mmap_info *m = (struct mmap_info *)h;
   struct ion_allocation_data_kk alloc;
   int ii, nErr = 0;
   m->fd = 0;
   m->paddr = 0;
   m->data.handle = 0;
   m->data.fd = 0;
   m->bufsize = size;
   VERIFY(0 < (m->fd = open("/dev/ion", O_RDONLY)));
   alloc.len = (m->bufsize + 4095) & (~4095);
   alloc.align = 0x1000;
   if(0 == (heap_mask & HEAP_ID_TO_MASK(ION_HEAP_ID_SYSTEM))) {
      for(ii = 0; ii < STD_ARRAY_SIZE(align); ++ii) {
         if(alloc.len >= align[ii]) {
            alloc.align = align[ii];
            break;
         }
      }
   }

   alloc.heap_id_mask = heap_mask;
   alloc.flags = flags;
   VERIFY_IPRINTF("alloc data %p", &m->data);
   if(ioctl(m->fd, ION_IOC_ALLOC_KK, &alloc) < 0) {
      nErr = -1;
      goto bail;
   }
   m->data.handle = alloc.handle;
   VERIFY_IPRINTF("mmap data %p", &m->data);
   VERIFY(0 <= ioctl(m->fd, ION_IOC_MAP, &m->data));
   VERIFY(MAP_FAILED != (m->paddr = (void *)mmap(NULL, alloc.len,
                           PROT_READ|PROT_WRITE, MAP_SHARED, m->data.fd, 0)));
   *ppo = m->paddr;
bail:
   if (nErr) {
      rpcmem_contig_free((struct mmap_info *)m, 0);
   }
   return nErr;
}

static int rpcmem_contig_alloc_ics(struct mmap_info *h, uint32 heap_mask, int size, void **ppo) {
   static uint32_t align[] = {
      0x100000, 0x40000, 0x10000, 0x4000, 0x1000
   };
   struct mmap_info *m = (struct mmap_info *)h;
   struct ion_allocation_data_ics alloc;
   int ii, nErr = 0;
   m->fd = 0;
   m->paddr = 0;
   m->data.handle = 0;
   m->data.fd = 0;
   m->bufsize = size;
   VERIFY(0 < (m->fd = open("/dev/ion", O_RDONLY)));
   alloc.len = (m->bufsize + 4095) & (~4095);
   for(ii = 0; ii < STD_ARRAY_SIZE(align); ++ii) {
      if(alloc.len >= align[ii]) {
         alloc.align = align[ii];
         break;
      }
   }

   alloc.flags = heap_mask;

   VERIFY(0 <= ioctl(m->fd, ION_IOC_ALLOC_ICS, &alloc));
   m->data.handle = alloc.handle;
   VERIFY(0 <= ioctl(m->fd, ION_IOC_MAP, &m->data));
   VERIFY(MAP_FAILED != (m->paddr = (void *)mmap(NULL, alloc.len,
                           PROT_READ|PROT_WRITE, MAP_SHARED, m->data.fd, 0)));
   *ppo = m->paddr;
bail:
   if (nErr) {
      rpcmem_contig_free((struct mmap_info *)m, 0);
   }
   return nErr;
}

