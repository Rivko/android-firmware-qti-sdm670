/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "qurt.h"
#include "rtld_uimg_mem_rw.h"

#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif
#include "HAP_farf.h"

//#include "sns_memmgr.h"
extern void SNS_OS_FREE(void *ptr);
extern void* SNS_OS_U_MALLOC(uint8_t module, uint32_t size);

int rtld_uimg_mem_rw_malloc(size_t length, void** rw, void** rw2)
{
  void* va = 0;

  va = SNS_OS_U_MALLOC(0, length);
  if (va){
    *rw = va;
    *rw2 = va;
    return 0;
  }
  FARF(ERROR, "rw malloc failed 0x%X bytes", length);
  return 1;
}

void rtld_uimg_mem_rw_free(void* va)
{
  SNS_OS_FREE(va);
}

int rtld_uimg_mem_rw_mprotect(void* va, size_t length, int prot, int ctx)
{
  // only RW allowed in this heap and pages already RW
  if (prot & PROT_EXEC) {
    // bail is execute privs requested
    FARF(ERROR, "Cannot mark RW heap as X");
    return -1;
  }
  return 0;
}

