#ifndef RTLD_MMAN_H
#define RTLD_MMAN_H
/*==============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <sys/mman.h>

void* rtld_umman_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset, int ctx);
int rtld_umman_munmap(void* addr, size_t length, int ctx);
int rtld_umman_mprotect(void* addr, size_t len, int prot, int ctx);

#endif // RTLD_MMAN_H


