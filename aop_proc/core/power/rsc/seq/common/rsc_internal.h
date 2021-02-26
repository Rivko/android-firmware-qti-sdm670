#ifndef _RSC_INTERNAL_H_
#define _RSC_INTERNAL_H_

/*==============================================================================
  FILE:         rsc_internal.h

  OVERVIEW:     This file provides prototypes for internal or utility functions 
                for rsc driver.

  DEPENDENCIES: Functions declared in this file may require proper initialization
                before and are not expected to be called directly outside of rsc 
                module.
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "rsc.h"

/*=======================================================================
 *                           RSC INTERNAL MACROS
 *======================================================================*/

/**
 * @brief Macro to check rsc handle validity. It ensures that the rsc
 *        handle and its 'addr' field are not NULL. Usually 'addr' field
 *        points to rsc instance's address in memory map so it would be
 *        non NULL on successful rsc handle initialization.
 */
#define RSC_VALID_HANDLE(h) (NULL != (h) && NULL != (h)->addr)

/*=======================================================================
 *                           RSC INERNAL FUNCTIONS
 *======================================================================*/

/**
 * @brief Target specific initialization. This function must be called
 *        as part of rsc common driver initialization.
 */
void rsc_target_init(void);

/**
 * @brief Initialize RSC instance base address as per target memory map.
 *
 * @param Handle to RSC to which base address need to be initialized.
 */
void rsc_init_base_address(rsc_handle handle);

/**
 * @brief Read RSC instance version information.
 *
 * @param handle to RSC instance to which Id information needs to be retrieved.
 */
void rsc_read_id_info(rsc_handle handle);

/**
 * @brief Read basic RSC configuration such as:
 *        - Number of DRVs
 *        - number sequencer commands
 *        - number potential timestampable events etc
 *
 * @param handle to RSC to which RSC configuration needs to be read.
 */
void rsc_read_internal_config(rsc_handle handle);

/**
 * @brief Enable/Disable time stamp unit associated with RSC instance(qdsp6/mss).
 *
 * @param handle to RSC instance to which enable/disable the TS unit.
 * @param val: Boolean value to enable or disable the TS unit.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_enable_ts_events(rsc_handle handle, boolean val);

/**
 * @brief Copied given RSC's command sequences to actual HW.
 *
 * @param handle: Handle to RSC for which command sequences are copied.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_copy_cmd_seqs(rsc_handle handle);

/**
 * @brief Returns the index of the input mode for a given RSC instance.
 *
 * @param handle: Handle to RSC.
 * @param lpm: Mode id for which we want to find index.
 *
 * @return If mode is found on input RSC, its index else -1.
 */
uint32 rsc_mode_index(rsc_handle handle, uint8 lpm_id);
#endif
