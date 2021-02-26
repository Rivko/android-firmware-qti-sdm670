/*-----------------------------------------------------------------------------
   Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef SYSMON_CACHELOCK_H_
#define SYSMON_CACHELOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Function: HAP_cache_lock
 * Description: Allocates memory and locks cache for given memory size (in Bytes)
                and returns locked virtual address. In case of failure returns 0.
                "paddr_ptr" argument is to get the locked 64bit physical address.
				If physical address is not needed then can pass NULL.   				
 *****************************************************************************/
void* HAP_cache_lock(unsigned int size, unsigned long long *paddr_ptr);
 
 
/******************************************************************************
 * Function: HAP_cache_unlock
 * Description: Unlocks cache and deallocates memory based on given virtual
                address. Returns success -> 0 else failure -> non-zero value.
 *****************************************************************************/
int HAP_cache_unlock(void* vaddr_ptr);


/******************************************************************************
 * Function: HAP_query_avail_cachelock
 * Description: Query API to get the size of max contiguous memory block
                available for cache locking. In case of failure returns -1 
				else available size (in Bytes).
 *****************************************************************************/
int HAP_query_avail_cachelock(void);


/******************************************************************************
 * Function: HAP_query_total_cachelock
 * Description: Query API to get the total locked cache size. In case of 
                failure returns -1 else total locked cache size (in Bytes).
 *****************************************************************************/
int HAP_query_total_cachelock(void); 


/******************************************************************************
 * Function: HAP_cache_lock_addr
 * Description: Locks cache for given virtual address and memory size (in Bytes).
                Returns success -> 0 else failure -> non-zero value. Only one 
				request will be honored at a time and there is maximum size limit
				of 64KB. This API is exclusively for code locking purpose.			
 *****************************************************************************/
int HAP_cache_lock_addr(void* vaddr_ptr, unsigned int size);


/******************************************************************************
 * Function: HAP_cache_unlock_addr
 * Description: Unlocks cache for given virtual address. Returns success -> 0
                else failure -> non-zero value. This API should be used along
				with "HAP_cache_lock_addr" API.
 *****************************************************************************/
int HAP_cache_unlock_addr(void* vaddr_ptr);

#ifdef __cplusplus
}
#endif

#endif /* SYSMON_CACHELOCK_H_ */
