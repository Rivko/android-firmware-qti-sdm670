/** 
    @file  ext_stubs.h
    @brief stubs for external dependencies
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __EXT_STUBS_H__
#define __EXT_STUBS_H__

#include <qurt_types.h>
#include "uClock.h"
#include "uTlmm.h"
// #include "micro_ULog.h"

void *pram_acquire_partition(char *name, uint32 *size);
int pram_mgr_lookup_phys_addr(int client_handle, qurt_addr_t vaddr);
int pram_mgr_lookup_phys_addr_by_name(char *name, qurt_addr_t vaddr);

#endif
