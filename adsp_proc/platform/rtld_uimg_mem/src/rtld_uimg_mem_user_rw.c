/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

//#ifndef FARF_HIGH
//#define FARF_HIGH 1
//#endif
#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif
#ifndef FARF_FATAL
#define FARF_FATAL 1
#endif

#include "HAP_farf.h"

#include "qurt_types.h"
#include "qurt_memory.h"

#include "qlist.h"

#include "rtld_uimg_mem_rw.h"

#define FREEIF(p) do { if (p) free((void*)(p)), (p) = 0; } while(0)

// TODO must be ISLAND when released to PW
#define RTLD_UIMG_MEM_RW_POOL "QURTOS_ISLAND_POOL"
//#define RTLD_UIMG_MEM_RW_POOL "DEFAULT_PHYSPOOL"
#define RTLD_UIMG_MEM_RW_ALIGNMENT 4
#define RTLD_UIMG_MEM_RW_BUF_SIZE (35 * 0x1000)

#define round_down(a, x) ((x) & ~((a) - 1))
#define round_up(a, x) round_down(a, (x) + (a) - 1)

static int rtld_uimg_mem_is_init = 0; // is this heap initialized?
static qurt_mem_region_t rtld_uimg_mem_rw_reg = 0; // rw region
static qurt_addr_t rtld_uimg_mem_va = 0; // rw virtual address
static qurt_addr_t rtld_uimg_mem_pa = 0; // rw physical address

typedef struct rtld_uimg_mem_rw_qnode {
  QNode qn;
  int size;
  int va;
} rtld_uimg_mem_rw_qnode;

static QLIST_DEFINE_INIT(qlfree); // ordered by va
static QLIST_DEFINE_INIT(qlused);

void rtld_uimg_mem_rw_walk(void)
{
  #if FARF_HIGH == 1
  QNode* qn;
  int i = 0;

  // look for smallest free node larger then length
  QLIST_FOR_ALL(&qlfree, qn) {
    rtld_uimg_mem_rw_qnode* srrqn = (rtld_uimg_mem_rw_qnode*)qn;
    FARF(HIGH, "RWHEAP: free #%03d va:0x%06X %05d B", i++, srrqn->va, srrqn->size);
  }
  i = 0;
  // look for smallest free node larger then length
  QLIST_FOR_ALL(&qlused, qn) {
    rtld_uimg_mem_rw_qnode* srrqn = (rtld_uimg_mem_rw_qnode*)qn;
    FARF(HIGH, "RWHEAP: used #%03d va:0x%06X %05d B", i++, srrqn->va, srrqn->size);
  }
  #endif
}

// TODO untested and unused
#if 0
static void rtld_uimg_mem_rw_deinit(void)
{
  QNode* qn = 0;

  while (0 != (qn = QList_Pop(&qlfree))) {
    free(qn);
  }
  while (0 != (qn = QList_Pop(&qlused))) {
    free(qn);
  }

  rtld_uimg_mem_va = 0;
  rtld_uimg_mem_pa = 0;
  if (rtld_uimg_mem_rw_reg) {
    qurt_mem_region_delete(rtld_uimg_mem_rw_reg);
    rtld_uimg_mem_rw_reg = 0;
  }
  rtld_uimg_mem_is_init = 0;
}
#endif

static void rtld_uimg_mem_init_cleanup(rtld_uimg_mem_rw_qnode* srrqn)
{
  FARF(ERROR, "init failed rw %x pa %x", rtld_uimg_mem_va, rtld_uimg_mem_pa);
  if (srrqn) {
    QNode_Dequeue(&srrqn->qn);
    free(srrqn);
  }

  rtld_uimg_mem_va = 0;
  rtld_uimg_mem_pa = 0;
  if (rtld_uimg_mem_rw_reg) {
    qurt_mem_region_delete(rtld_uimg_mem_rw_reg);
    rtld_uimg_mem_rw_reg = 0;
  }
}

static int rtld_uimg_mem_rw_init(void)
{
  qurt_mem_pool_t pool;
  qurt_mem_region_attr_t attr = {0};
  rtld_uimg_mem_rw_qnode* srrqn = 0;

  if (rtld_uimg_mem_is_init) {
    return 0;
  }

  // allocate RW region
  qurt_mem_region_attr_init(&attr);
  qurt_mem_region_attr_set_mapping(&attr, QURT_MEM_MAPPING_VIRTUAL); // TODO use VIRTUAL_RANDOM
  qurt_mem_region_attr_set_perms(&attr, QURT_PERM_READ | QURT_PERM_WRITE);
  if (0 != qurt_mem_pool_attach(RTLD_UIMG_MEM_RW_POOL, &pool)) {
    rtld_uimg_mem_init_cleanup(srrqn);
    FARF(ERROR, "rw malloc: mem_pool_attach failed");
    return 1;
  }
  if (0 != qurt_mem_region_create(&rtld_uimg_mem_rw_reg,
                                  RTLD_UIMG_MEM_RW_BUF_SIZE, pool, &attr)) {
    rtld_uimg_mem_init_cleanup(srrqn);
    FARF(ERROR, "rw malloc: rw region_create failed");
    return 1;
  }
  if (0 != qurt_mem_region_attr_get(rtld_uimg_mem_rw_reg, &attr)) {
    rtld_uimg_mem_init_cleanup(srrqn);
    FARF(ERROR, "rw malloc: rw region_attr_get failed");
    return 1;
  }
  qurt_mem_region_attr_get_virtaddr(&attr, (qurt_addr_t*)&rtld_uimg_mem_va);
  qurt_mem_region_attr_get_physaddr(&attr, &rtld_uimg_mem_pa);

  // setup heap structures
  if ( 0 == (srrqn = (rtld_uimg_mem_rw_qnode*)malloc(sizeof(rtld_uimg_mem_rw_qnode)))) {
    rtld_uimg_mem_init_cleanup(srrqn);
    FARF(ERROR, "rw malloc: malloc failed");
    return 1;
  }
  srrqn->size = RTLD_UIMG_MEM_RW_BUF_SIZE;
  srrqn->va = (int)rtld_uimg_mem_va;
  QList_AppendNode(&qlfree, &srrqn->qn);

  // init done
  rtld_uimg_mem_is_init = 1;

  return 0;
}

void rtld_uimg_mem_rw_free(void* va)
{
  QNode* qn;
  rtld_uimg_mem_rw_qnode* srrqnused = 0;

  // look for matching used node
  QLIST_FOR_ALL(&qlused, qn) {
      srrqnused = (rtld_uimg_mem_rw_qnode*)qn;
      if (va == (void*)srrqnused->va) {
        break; // found it
      }
  }
  if (0 == srrqnused) {
    FARF(ERROR, "uimg rw heap: invalid ptr %p passed to free", va);
    return; // va not found
  }
  // remove from used list
  QNode_DequeueZ(&srrqnused->qn);

  {
    rtld_uimg_mem_rw_qnode* srrqnfree = 0;

    if (!QList_IsEmpty(&qlfree)) { 
      QLIST_FOR_ALL(&qlfree, qn) {
        srrqnfree = (rtld_uimg_mem_rw_qnode*)qn;
        if (srrqnused->va < srrqnfree->va) {
          QNode_InsPrev(&srrqnfree->qn, &srrqnused->qn);
          break;
        }
        srrqnfree = 0;
      }
    }
    if (0 == srrqnfree) {
      // larger then all nodes or empty list, append to end
      QList_AppendNode(&qlfree, &srrqnused->qn);
    }
  }
  // try and consolidate free nodes
  {
    QNode* qnnext = 0;
    rtld_uimg_mem_rw_qnode* srrqn = 0;
    rtld_uimg_mem_rw_qnode* srrqnnext = 0;
    QLIST_NEXTSAFE_FOR_ALL(&qlfree, qn, qnnext) {
      srrqn = (rtld_uimg_mem_rw_qnode*)qn;
      srrqnnext = (rtld_uimg_mem_rw_qnode*)qnnext;
      if (qnnext && srrqn->va + srrqn->size == srrqnnext->va) {
        srrqnnext->va = srrqn->va;
        srrqnnext->size += srrqn->size;
        QNode_Dequeue(qn);
        free(qn);
      }
    }
  }

  rtld_uimg_mem_rw_walk();
}

int rtld_uimg_mem_rw_malloc(size_t length, void** rw, void** rw2)
{
  QNode* qn;
  rtld_uimg_mem_rw_qnode* srrqnfree = 0;
  rtld_uimg_mem_rw_qnode* srrqnused = 0;
  unsigned int smallest = -1;

 
  if (0 != rtld_uimg_mem_rw_init()) {
    FARF(ERROR, "umalloc rw_init failed");
    return 1;
  }

  if (0 == length) {
    FARF(ERROR, "umalloc failed, zero size alloc not supported");
    return 1; // zero length not allowed
  }
  length = round_up(RTLD_UIMG_MEM_RW_ALIGNMENT, length); // aligned va's only
 
  // look for smallest free node larger then length
  QLIST_FOR_ALL(&qlfree, qn) {
    rtld_uimg_mem_rw_qnode* srrqn = (rtld_uimg_mem_rw_qnode*)qn;
    if (srrqn->size >= length && srrqn->size < smallest) {
      smallest = srrqn->size;
      srrqnfree = srrqn;
    }
  }
  if (0 == srrqnfree) {
    FARF(ERROR, "uimg rw heap: failed to alloc %d bytes", length);
    return 1; // no free blocks big enough
  }

  if (srrqnfree->size == length) {
    // request equals block length, use it
    QNode_Dequeue(&srrqnfree->qn);
    QList_AppendNode(&qlused, &srrqnfree->qn);
    srrqnused = srrqnfree;
  } else {
    // cut off length bytes from end of block into new node
    if ( 0 == (srrqnused = (rtld_uimg_mem_rw_qnode*)malloc(sizeof(rtld_uimg_mem_rw_qnode)))) {
      rtld_uimg_mem_init_cleanup(srrqnused);
      FARF(ERROR, "uimg rw heap: malloc of 0x%X bytes failed", (unsigned int) sizeof(rtld_uimg_mem_rw_qnode));
      return 1;
    }
    srrqnused->size = length;
    srrqnused->va = srrqnfree->va;
    srrqnfree->size -= length;
    srrqnfree->va += length;
    QList_AppendNode(&qlused, &srrqnused->qn);
  }

  if (srrqnused->va & 0x3) {
    FARF(ERROR, "Unaligned rw malloc 0x%X %d\n", srrqnused->va, length);
    return 1;
  }

  *rw = (void*)srrqnused->va;
  *rw2 = (void*)srrqnused->va;

  rtld_uimg_mem_rw_walk();

  return 0;
}

int rtld_uimg_mem_rw_mprotect(void* va, size_t length, int prot, int ctx)
{
  if (0 == (prot & PROT_WRITE) || 0 == (prot & PROT_READ)) {
    // can only be rw
    return -1;
  }
  return 0;
}

