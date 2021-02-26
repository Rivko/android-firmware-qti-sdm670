/*==============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#ifndef FARF_HIGH
#define FARF_HIGH 1
#endif
#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif

#include "HAP_farf.h"

#include "qurt_types.h"
#include "qurt_mmap.h"
#include "qurt_memory.h"

// TODO must be ISLAND when released to PW
#define RTLD_UMMAN_POOL "QURTOS_ISLAND_POOL"
//#define RTLD_UMMAN_POOL "DEFAULT_PHYSPOOL"

void* rtld_umman_mmap(void* addr, size_t length, int prot, int flags, int fd,
                      off_t offset, int ctx)
{
  qurt_mem_pool_t pool; 
  void* pv = 0; 
  int nErr; 
   
  nErr = qurt_mem_pool_attach(RTLD_UMMAN_POOL, &pool); 
  if (QURT_EOK != nErr) { 
    FARF(ERROR, "qurt_mem_pool failed %d", nErr);
    return QURT_MAP_FAILED;
  } 

  pv = qurt_mem_mmap(-1, pool, 0, addr, length, prot, flags, fd, offset);
  if (QURT_MAP_FAILED == pv || 0 == pv) { 
    FARF(ERROR, "qurt_mem_mmap failed %d", pv);
    return QURT_MAP_FAILED;
  } 
  return pv;
}

int rtld_umman_munmap(void* addr, size_t length, int ctx)
{
  return qurt_mem_munmap(addr, length);
}

int rtld_umman_mprotect(void* addr, size_t len, int prot, int ctx)
{
  return qurt_mem_mprotect(addr, len, prot);
}

