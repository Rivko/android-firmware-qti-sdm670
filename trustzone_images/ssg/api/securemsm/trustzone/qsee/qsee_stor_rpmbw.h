
#ifndef QSEE_STOR_RPMBW_H
#define QSEE_STOR_RPMBW_H

/*===========================================================================
  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

#include "qsee_stor.h"

/******************************************************************************
* Name: qsee_rpmbw_init
*
* Description:
* This function initializes the RPMB. It first reads the RPMB write counter
* to see if the device is provisioned. If it is provisioned, it will then
* read the partition table.
*
* Arguments:
*    dev_id         [IN]:  Physical partition number
*    partition_guid [IN]:  GPT partition GUID
*    dev_handle     [OUT]: Pointer to a device handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_init(qsee_stor_device_id_type dev_id, uint8_t *partition_guid,
                    qsee_stor_device_handle_t *dev_handle);

/******************************************************************************
* Name: qsee_rpmbw_open_partition
*
* Description:
* This function opens a logical partition within the RPMB
*
* Arguments:
*    dev_handle     [IN]:  Pointer to a device handle from rpmbw_init();
*    partition_id   [IN]:  Logical partition ID
*    cli_handle     [OUT]: Pointer to a client handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_open_partition(qsee_stor_device_handle_t *dev_handle,
                              uint32_t partition_id,
                              qsee_stor_client_handle_t *cli_handle);

/******************************************************************************
* Name: qsee_rpmbw_device_get_info
*
* Description:
* This function gets the RPMB device info
*
* Arguments:
*    dev_handle      [IN]:  Pointer to a device handle from qsee_rpmbw_init();
*    dev_info        [OUT]: Pointer to a device info structure
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_device_get_info(qsee_stor_device_handle_t *dev_handle,
                               qsee_stor_device_info_t *dev_info);

/******************************************************************************
* Name: qsee_rpmbw_client_get_info
*
* Description:
* This function returns the client info
*
* Arguments:
*    cli_handle   [IN]:  Pointer a client handle from qsee_rpmbw_open_partition
*    cli_info     [OUT]: Pointer to a client info structure
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_client_get_info(qsee_stor_client_handle_t *cli_handle,
                               qsee_stor_client_info_t *cli_info);

/******************************************************************************
* Name: qsee_rpmbw_add_partition
*
* Description:
* This function adds a new logical partition to the RPMB
*
* Arguments:
*    dev_handle     [IN]:  Pointer a device handle from qsee_rpmbw_init();
*    partition_id   [IN]:  Logical Partition ID
*    num_sectors    [IN]:  Number of sectors of the new logical partition
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_add_partition(qsee_stor_device_handle_t *dev_handle,
                             uint32_t partition_id, uint16_t num_sectors);

/******************************************************************************
* Name: qsee_rpmbw_read_sectors
*
* Description:
* This function reads num_sectors of data from the RPMB partition starting from
* start_sector and stores it in data_buffer
*
* Arguments:
*    cli_handle   [IN]:  Pointer to a client handle from
*                        qsee_rpmbw_open_partition();
*    start_sector [IN]:  Starting sector of the read
*    num_sectors  [IN]:  Number of sectors to read
*    data_buffer  [OUT]: Buffer to hold the data read
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_read_sectors(qsee_stor_client_handle_t *cli_handle,
                            uint32_t start_sector, uint32_t num_sectors,
                            uint8_t *data_buffer);

/******************************************************************************
* Name: qsee_rpmbw_write_sectors
*
* Description:
* This function writes num_sectors of data in data_buffer to the RPMB partition,
* starting from start_sector.
*
* Arguments:
*    cli_handle   [IN]: Pointer to a client handle from
*                       qsee_rpmbw_open_partition();
*    start_sector [IN]: Starting sector of the write
*    num_sectors  [IN]: Number of sectors to write
*    data_buffer  [IN]: Data that needs to be written
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_write_sectors(qsee_stor_client_handle_t *cli_handle,
                             uint32_t start_sector, uint32_t num_sectors,
                             uint8_t *data_buffer);

/******************************************************************************
* Name: qsee_rpmbw_read_wp_config
*
* Description:
* This function attempts to read the Secure Write Protect Configuration Block
*
* Arguments:
*    wp_info       [IN/OUT]: Pointer to a buffer containing information about
*                            the secure write protect configuration
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_read_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/********************************************************************************
* Name: qsee_rpmbw_write_wp_config
*
* Description:
* This function attempts to write to the Secure Write Protect Configuration
* Block
*
* Arguments:
*    wp_info       [IN]: Pointer to a buffer containing information about the
*                        secure write protect configuration
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
*******************************************************************************/
int qsee_rpmbw_write_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/******************************************************************************
* Name: qsee_rpmbw_remove_client
*
* Description:
* This function closes a logical partition within the RPMB. If the partition is
* the last partition in the partition table, it will reclaim space. Otherwise,
* it will simply remove the client.
*
* Arguments:
*    cli_handle     [IN]:  Pointer to a client handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_rpmbw_remove_client(qsee_stor_client_handle_t *cli_handle);

#endif /* QSEE_STOR_RPMBW_H */
