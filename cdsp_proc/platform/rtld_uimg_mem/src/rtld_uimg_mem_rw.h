#ifndef RTLD_UIMG_MEM_RW_H
#define RTLD_UIMG_MEM_RW_H
/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <sys/mman.h>

int rtld_uimg_mem_rw_malloc(size_t length, void** rw, void** rw2);
void rtld_uimg_mem_rw_free(void* va);
int rtld_uimg_mem_rw_mprotect(void* va, size_t length, int prot, int ctx);

#endif // RTLD_UIMG_MEM_RW_H
