
/*==============================================================================
  FILE:         pram_mgr.h
  
  OVERVIEW:     pram mgr API for clients to use

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/pram_mgr/inc/pram_mgr.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
==============================================================================*/

#ifndef PRAM_MGR_H
#define PRAM_MGR_H

#include "comdef.h"
#include "qurt_memory.h"
#include "qurt_types.h"

void *pram_acquire_partition(char *name, uint32 *size); //size in bytes

/* API only intended for rootPD */
int pram_mgr_lookup_phys_addr(int client_handle, qurt_addr_t vaddr);
int pram_mgr_lookup_phys_addr_by_name(char * name, qurt_addr_t vaddr);


#endif /* PRAM_MGR_H */
