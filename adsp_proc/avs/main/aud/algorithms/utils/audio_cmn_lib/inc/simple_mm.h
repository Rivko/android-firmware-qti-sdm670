/*============================================================================
  @file simple_mm.h

  Simple memory management system for allocating and initializing data
  within a contiguous block.

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef __SIMPLE_MM_H_
#define __SIMPLE_MM_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "stddef.h"
#include "string.h"

/*----------------------------------------------------------------------------
  Macros
----------------------------------------------------------------------------*/
/* Align an offset to multiple of 4 (useful for aligning word offsets
   to 64-bit boundary */
#define ALIGN4(o)         (((o)+3)&(~3))
/* Align an offset to multiple of 8 (useful for aligning byte offsets
   to 64-bit boundary */
#define ALIGN8(o)         (((o)+7)&(~7))

#define ALIGN4096(o)         (((o)+4095)&(~4095))
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
   void *mem;
} SimpleMemMgr;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
/* Initialize simple memory manager */
void smm_init(SimpleMemMgr *smm, void *memstart);

/* Determine effective size that will be 
allocated by subsequent call to smm_malloc. */
size_t smm_calloc_size(size_t nmemb, size_t size);

size_t smm_calloc_size_4kaligned(size_t nmemb, size_t size);

/*Determine effective size that will be allocated
 by subsequent call to smm_calloc. */
size_t smm_malloc_size(size_t size);

/* Allocate a block of memory of the given size */
void *smm_calloc(SimpleMemMgr *smm, size_t nmemb, size_t size);

void *smm_calloc_4kaligned(SimpleMemMgr *smm, size_t nmemb, size_t size);

/* Allocate a block of memory of the given size */
void *smm_malloc(SimpleMemMgr *smm, size_t size);

void *smm_malloc_4kaligned(SimpleMemMgr *smm, size_t size);

/* Free allocated memory */
void smm_free(SimpleMemMgr *smm, void *ptr);

#endif  /* __SIMPLE_MM_H_ */
