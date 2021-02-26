#ifndef RTLD_UIMG_MEM_RX_H
#define RTLD_UIMG_MEM_RX_H
/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <sys/mman.h>

// disclaimer
// TODO
// This heap makes the following assumptions
// - single threaded access
// - only callable from big image
// - poor fragmentation management
// - uses general heap to manage nodes

int rtld_uimg_mem_rx_malloc(size_t length, void** rx, void** rw);
void rtld_uimg_mem_rx_free(void* va);
int rtld_uimg_mem_rx_mprotect(void* va, size_t length, int prot, int ctx);

#endif // RTLD_UIMG_MEM_RX_H
