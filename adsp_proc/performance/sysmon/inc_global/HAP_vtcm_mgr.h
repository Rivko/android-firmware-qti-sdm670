/*-----------------------------------------------------------------------------
   Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef HAP_VTCM_MGR_H_
#define HAP_VTCM_MGR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @file HAP_vtcm_mgr.h
 *  @brief Header file with APIs to allocate, release and query VTCM memory.
 */
 
/**************************************************************************//**
 * @fn HAP_request_VTCM
 * @brief Request VTCM memory of desired size and single page requirement.
 *        Given size is aligned to 4K if single_page_flag == 0.
 *        Given size is aligned to the closest page size i.e,
 *                4KB / 16KB / 64KB / 256KB if single_page_flag == 1.
 * @param[in] size  size of the request
 * @param[in] single_page_flag  1 for single page requests, 0 otherwise
 * @return void* pointer to the created memory block. 0 on failure.
 *****************************************************************************/
void* HAP_request_VTCM(unsigned int size, unsigned int single_page_flag);

/**************************************************************************//**
 * @fn HAP_release_VTCM
 * @brief Release previously requested VTCM memory block.
 * @param[in] pVA pointer returned by the corresponding VTCM request call.
 * @return int 0 for success and non-zero for failure.
 *****************************************************************************/
int HAP_release_VTCM(void* pVA);

/**************************************************************************//**
 * @fn HAP_query_total_VTCM
 * @brief Returns maximum page size allocation possible in entire VTCM 
 *        as '*page_size' and number of such pages as '*page_count'.
 * @param[out] page_size Pointer to an unsigned int. If not NULL, returns 
 *                        maximum page size allocation possible in VTCM.
 * @param[out] page_count Pointer to an unsigned int. If not NULL, returns 
 *                         number of page_size blocks available.
 * @return int 0 for success and non-zero for failure.
 *****************************************************************************/
int HAP_query_total_VTCM(unsigned int* page_size, unsigned int* page_count);

/**************************************************************************//**
 * @fn HAP_query_avail_VTCM
 * @brief Returns maximum contiguous memory chunk available in VTCM as 
 *        '*avail_block_size'. Maximum possible page size allocation in available 
 *        portion of VTCM as '*max_page_size' and number of such maximum page 
 *        sizes available in VTCM as '*num_pages'
 * @param[out] avail_block_size Pointer to an unsigned int. If not NULL, 
 *                               returns maximum continguous memory chunk available 
 *                               in VTCM .
 * @param[out] max_page_size Pointer to an unsigned int. If not NULL, 
 *                            returns maximum possible page size allocation in 
 *                            available portion of VTCM.
 * @param[out] num_pages Pointer to an unisigned int. If not NULL, returns number 
 *                        of max_page_size blocks available in VTCM.
 * @return int 0 for success and non-zero for failure.
 *****************************************************************************/
int HAP_query_avail_VTCM(unsigned int* avail_block_size,
                          unsigned int* max_page_size,
                          unsigned int* num_pages);

#ifdef __cplusplus
}
#endif

#endif //HAP_VTCM_MGR_H_
