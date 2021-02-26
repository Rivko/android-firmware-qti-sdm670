#ifndef __SDCC_HANDLE_UTIL_H
#define __SDCC_HANDLE_UTIL_H
/******************************************************************************
 * File: sdcc_handle_util.h
 *
 * Services:
 *    This file provides SDCC handle util definitions for SDIO and memory
 *    devices.
 *
 * Description:
 *    This file contains the SDCC handle specific utility function prototypes.
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-07-29   bn      Initial version. Branched from 8994 UEFI

=============================================================================*/
#include "sdcc_priv.h"
#include "SdccCommon.h"

/* maximum number of client handle array entries */
#define SDCC_CLIENT_HANDLE_MAX_NUM         16

/* dereference handle to get the driveno */
#define SDCC_HANDLE_GET_DRIVENO(h) ((h)->hdev->driveno)


/******************************************************************************
* Name: sdcc_register_client_handle
*
* Description:
*    This function registers a client handle with the specified driveno and
*    phy_partition_num.  It returns a pointer to a client handle entry if the
*    registration succeeds.
*
* Arguments:
*    driveno           [IN] : a valid drive number between 0 and the total
*                             number of card slots - 1
*    phy_partition_num [IN] : physical partition number.  Use
*                             \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE
*                             to get a handle representing the entire memory
*                             device.  For eMMC v4.3, the valid boot partition
*                             numbers are 1 and 2.  User area partition number
*                             is 0.
*
* Returns:
*    A pointer to SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *sdcc_register_client_handle( UINT32 driveno,
                                                 UINT32 phy_partition_num );

/******************************************************************************
* Name: sdcc_deregister_client_handle
*
* Description:
*    This function de-registers a client handle.  It clears all client handle
*    fields in an entry and set the handle to NULL.
*
* Arguments:
*    handle     [IN/OUT] : a pointer to a pointer to the SDCC device
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_deregister_client_handle( struct sdcc_device **handle );

/******************************************************************************
* Name: sdcc_is_device_still_in_use
*
* Description:
*    This function examines if there are any entries in the client handle list
*    still referring to the SDIO/memory device specified in driveno.
*
* Arguments:
*    pdev           [IN] : pointer to sdcc device handle that is in use
*                             
*
* Returns:
*    TRUE if SDIO/memory device is still in use.  FALSE otherwise.
*
******************************************************************************/
BOOLEAN sdcc_is_device_still_in_use( sdcc_dev_type *pdev );

#endif /* __SDCC_HANDLE_UTIL_H */

