/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

#ifndef __TEETEST_CACHE_H__
#define __TEETEST_CACHE_H__

#include <stddef.h>
#include <stdint.h>

extern void clean_and_inval_dcache_region(uint64_t vaddr, uint64_t len);

#endif /* __TEETEST_CACHE_H__ */
