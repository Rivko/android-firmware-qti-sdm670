#ifndef _BAM_DRV_H_
#define _BAM_DRV_H_

/**
  @file bam_drv.h
  @brief Public interface include file for the BAM driver initialization.

  This file contains definitions of interfaces that provide construction and
  destruction operations of the BAM drive.

  These APIs are only intended to be called from a single threaded environment
  and during subsystem initialization ( ex. rcinit, boot loaders )

  Clients of BAM driver are not expected to call these routines directly.
*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/10/12   RL      Created

===============================================================================
                   Copyright (c) 2012 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "bam.h"

/**
 * @brief Initialize the bam driver
 *
 * This function initializes the bam driver data structures. 
 * Clients of bam driver are not expected to call this function
 * directly. 
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_drv_init(void);

/**
 * @brief De-initialize the bam driver
 *
 * This function de-initializes the bam driver data structures. 
 * Clients of bam driver are not expected to call this function
 * directly. 
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_drv_deinit(void);
#endif /* BAM_H_ */
