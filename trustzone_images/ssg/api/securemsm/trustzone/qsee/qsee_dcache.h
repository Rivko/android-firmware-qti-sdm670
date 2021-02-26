#ifndef QSEE_DCACHE_H
#define QSEE_DCACHE_H

/**
@file qsee_dcache.h
@brief Provide DCache functionality
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_dcache.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when      who       what, where, why
--------  --------  ------------------------------------
03/04/15  cmihalik  Initial revision

===========================================================================*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Invalidate a memory region in the cache. Please note that the data
 * in the cache will not be written back to the main memory. However,
 * any further accesses to data in this region would result in a
 * cache-miss.
 *
 * @param addr[in] Start address of memory region
 * @param length[in] Length of memory region
 */

void qsee_dcache_inval_region(void *addr, size_t length);

/**
 * Cleans and invalidates a memory region in the cache. Please note
 * that the data in the cache would be written back to main memory if
 * dirty and the region invalidated. So any further access to the data
 * would result in a cache-miss.
 *
 * @param addr[in] Start address of memory region
 * @param length[in] Length of memory region
 */

void qsee_dcache_flush_region(void *addr, size_t length);

/**
 * Cleans a memory region in the cache. Please note that this will
 * write back any data that is dirty but it will not invalidate the
 * cache region.  So any further accesses to data in this region would
 * result in a cache-hit.
 *
 * @param addr[in] Start address of memory region
 * @param length[in] Length of memory region
 */

void qsee_dcache_clean_region(void *addr, size_t length);

#endif 

