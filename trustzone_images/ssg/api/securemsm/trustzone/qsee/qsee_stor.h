#ifndef QSEE_STOR_H
#define QSEE_STOR_H

/* @file qsee_stor.h

  This file contains storage definitions for the secure apps

  Copyright (c) 2012-2015, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2015-12-09   jt      Add remove client API 
2015-09-14   jt      Secure write protect support 
2015-06-04   jt      Add access violation error code  
2015-04-24   jt      Add autoprovisioning disabled error code
2013-03-04   jt      Adjusted error codes
2012-12-12   jt      Initial version

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>

/* Storage Device Return Error Codes */
#define QSEE_STOR_SUCCESS                     0  /* Success */
#define QSEE_STOR_ERROR                      -1  /* Generic Failure */
#define QSEE_STOR_INVALID_PARAM_ERROR        -2  /* Invalid arguments passed to the API */
#define QSEE_STOR_NOT_FOUND_ERROR            -3  /* Device Not found Error */
#define QSEE_STOR_PARTI_NOT_FOUND_ERROR      -4  /* Partition Not found Error */
#define QSEE_STOR_VERSION_MISMATCH           -5  /* Listener version Version mismatch */
#define QSEE_STOR_OUT_OF_RESOURCES           -6  /* Out of memory/other resources*/
#define QSEE_STOR_NOT_SUPPORTED              -7  /* Operation Not supported */
#define QSEE_STOR_RPMB_UNKNOWN_ERROR         -8  /* Unknown error during RPMB access */
#define QSEE_STOR_RPMB_MAC_ERROR             -9  /* MAC autentication error during RPMB access */
#define QSEE_STOR_RPMB_WRITE_COUNTER_ERROR   -10 /* Write counter mismatch during RPMB access */
#define QSEE_STOR_RPMB_ADDR_ERROR            -11 /* RPMB address failure */
#define QSEE_STOR_RPMB_WRITE_ERROR           -12 /* Failure during RPMB Write */
#define QSEE_STOR_RPMB_READ_ERROR            -13 /* Failure during RPMB Read */
#define QSEE_STOR_RPMB_NOT_PROVISIONED_ERROR -14 /* RPMB Not provisioned */
#define QSEE_STOR_RPMB_MAC_GENERATION_ERROR  -15 /* Failure during MAC generation */
#define QSEE_STOR_RPMB_RNG_GENERATION_ERROR  -16 /* Failure during RNG generation */
#define QSEE_STOR_RPMB_NONCE_ERROR           -17 /* Nonce authentication error */
#define QSEE_STOR_RPMB_PROVISIONED_ERROR     -18 /* RPMB already provisioned */
#define QSEE_STOR_PARTITION_FOUND            -19 /* Partition Found */
#define QSEE_STOR_APP_ID_ERROR               -20 /* Failed to create/get application id */
#define QSEE_STOR_RPMB_AUTOPROV_DISABLED     -21 /* RPMB autoprovisioning has been disabled */
#define QSEE_STOR_ACCESS_VIOLATION_ERROR     -22 /* Access violation error */
#define QSEE_STOR_WP_CONFIG_ACCESS_ERROR     -23 /* WP config read/write failure */
#define QSEE_STOR_WP_CONFIG_INV_PARAM_ERROR  -24 /* WP config invalid param */
#define QSEE_STOR_WP_NOT_APPLICABLE_ERROR    -25 /* WP config not allowed */ 

/* Storage Device Types */
typedef enum
{
  QSEE_STOR_EMMC_USER = 0,            /* User Partition in eMMC */
  QSEE_STOR_EMMC_BOOT0,               /* Boot0 Partition in eMMC */
  QSEE_STOR_EMMC_BOOT1,               /* Boot1 Partition in eMMC */
  QSEE_STOR_EMMC_RPMB,                /* RPMB Partition in eMMC */
  QSEE_STOR_EMMC_GPP1,                /* GPP1 Partition in eMMC */
  QSEE_STOR_EMMC_GPP2,                /* GPP2 Partition in eMMC */
  QSEE_STOR_EMMC_GPP3,                /* GPP3 Partition in eMMC */
  QSEE_STOR_EMMC_GPP4,                /* GPP4 Partition in eMMC */
  QSEE_STOR_EMMC_ALL,                 /* Entire eMMC device */
  QSEE_STOR_ID_RESERVED = 0x7FFFFFFF  /* Reserved: Device ID Max */
} qsee_stor_device_id_type;

/* Device Information structure */
typedef struct
{
  qsee_stor_device_id_type dev_id;             /* Device ID (Physical partition number) */
  uint8_t                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32_t                   bytes_per_sector;   /* Bytes per Sector */
  uint32_t                   total_sectors;      /* Total size in sectors */
  uint32_t                   available_sectors;  /* Total available sectors for new partitions */
} __attribute__ ((packed)) qsee_stor_device_info_t;


/* Client Information structure */
typedef struct
{
  qsee_stor_device_id_type dev_id;             /* Device ID */
  uint8_t                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32_t                    bytes_per_sector;   /* Bytes per Sector */
  uint32_t                    total_sectors;      /* Total size available in sectors */
} __attribute__ ((packed)) qsee_stor_client_info_t;

/* Secure Write Protect Info Entry structure */
typedef struct 
{
   uint8_t  wp_enable;    /* UFS: WPF (Write Protect Flag), eMMC: SECURE_WP_MODE_ENABLE */
   uint8_t  wp_type_mask; /* UFS: WPT (Write Protect Type), eMMC: SECURE_WP_MODE_CONFIG */
   uint64_t addr;         /* UFS: LBA, eMMC: 0x1/0x2 (address of the device config register) */
   uint32_t num_blocks;   /* UFS: Num LBA, eMMC: Set to 0 */
} __attribute__ ((packed)) qsee_stor_secure_wp_info_entry_t;

/* Secure Write Protect Info structure */
typedef struct 
{
   uint8_t                          lun_number;    /* UFS: LUN #, eMMC: Set to 0 */
   uint8_t                          num_entries;   /* Number of Secure wp entries */
   qsee_stor_secure_wp_info_entry_t wp_entries[4]; /* Max 4 entries total */
} __attribute__ ((packed)) qsee_stor_secure_wp_info_t;

typedef uint64_t qsee_stor_device_handle_t;
typedef uint64_t qsee_stor_client_handle_t;

/******************************************************************************
* Name: qsee_stor_device_init
*
* Description:
* This function attempts to initialize the device indicated by device_id
* and partition_guid
*
* Arguments:
*    device_id         [IN]:  Partition number of the device
*    partition_guid    [IN]:  Partition GUID (applies only for GPT partitions)
*    device_handle     [OUT]: Pointer to a device handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_device_init(qsee_stor_device_id_type device_id, uint8_t *partition_guid,
                          qsee_stor_device_handle_t *device_handle);

/******************************************************************************
* Name: qsee_stor_open_partition
*
* Description:
* This function attempts to open a logical partition
*
* Arguments:
*    device_handle     [IN]:  Pointer to a device handle obtained from
*                             qsee_stor_device_init()
*    partition_id      [IN]:  Logical partition ID
*    client_handle     [OUT]: Pointer to a client handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_open_partition(qsee_stor_device_handle_t *device_handle, uint32_t partition_id,
                             qsee_stor_client_handle_t *client_handle);

/******************************************************************************
* Name: qsee_stor_device_get_info
*
* Description:
* This function returns the device info
*
* Arguments:
*    device_handle   [IN]:  Pointer a device handle from qsee_stor_device_init()
*    device_info     [OUT]: Pointer to a device info structure
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_device_get_info(qsee_stor_device_handle_t *device_handle,
                              qsee_stor_device_info_t *device_info);

/******************************************************************************
* Name: qsee_stor_client_get_info
*
* Description:
* This function returns the client info
*
* Arguments:
*    client_handle   [IN]:  Pointer a client handle from qsee_stor_open_partition()
*    client_info     [OUT]: Pointer to a client info structure
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_client_get_info(qsee_stor_client_handle_t *client_handle,
                              qsee_stor_client_info_t *client_info);

/******************************************************************************
* Name: qsee_stor_add_partition
*
* Description:
* This function adds a new logical partition
*
* Arguments:
*    device_handle  [IN]:  Pointer a device handle from qsee_stor_device_init()
*    partition_id   [IN]:  Logical Partition ID
*    num_sectors    [IN]:  Number of sectors of the new logical partition
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_add_partition(qsee_stor_device_handle_t *device_handle,
                            uint32_t partition_id, uint16_t num_sectors);

/******************************************************************************
* Name: qsee_stor_read_sectors
*
* Description:
* This function attempts to read num_sectors of data from start_sector
* to data_buffer.
*
* Arguments:
*    client_handle [IN]:  Pointer to a client handle from
*                         qsee_stor_open_partition()
*    start_sector  [IN]:  Starting sector to read from
*    num_sectors   [IN]:  Number of sectors to read
*    data_buffer   [OUT]: Pointer to a buffer containing data that has been read
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_read_sectors(qsee_stor_client_handle_t *client_handle, uint32_t start_sector,
                           uint32_t num_sectors, uint8_t *data_buffer);

/******************************************************************************
* Name: qsee_stor_write_sectors
*
* Description:
* This function attempts to write num_sectors of data from data_buffer to
* start_sector.
*
* Arguments:
*    client_handle [IN]: Pointer to a client handle from
*                        qsee_stor_open_partition()
*    start_sector  [IN]: Starting sector to write to
*    num_sectors   [IN]: Number of sectors to write
*    data_buffer   [IN]: Pointer to a buffer containing data to be written
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_write_sectors(qsee_stor_client_handle_t *client_handle, uint32_t start_sector,
                            uint32_t num_sectors, uint8_t *data_buffer);

/******************************************************************************
* Name: qsee_stor_read_wp_config
*
* Description:
* This function attempts to read the Secure Write Protect Configuration Block 
*
* Arguments:
*    wp_info       [IN/OUT]: Pointer to a buffer containing information about the
*                            secure write protect configuration
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_read_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/********************************************************************************
* Name: qsee_stor_write_wp_config
*
* Description:
* This function attempts to write to the Secure Write Protect Configuration Block
* 
* Arguments:
*    wp_info       [IN]: Pointer to a buffer containing information about the
*                        secure write protect configuration 
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
********************************************************************************/
int qsee_stor_write_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/******************************************************************************
* Name: qsee_stor_remove_client
*
* Description:
* This function closes a logical partition within the RPMB. If the partition is
* the last partition in the partition table, it will reclaim space. Otherwise, 
* it will simply remove the client.  
*
* Arguments:
*    client_handle     [IN]:  Pointer to a client handle
*
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise
*
******************************************************************************/
int qsee_stor_remove_client(qsee_stor_client_handle_t *client_handle);

/******************************************************************************
* Name: qsee_query_rpmb_enablement
*
* Description:
* This function queries whether the OEM has configured the enablement of RPMB
* on the device.
*
* Arguments:
*    b        [OUT]:  Pointer to boolean query response
*
* Returns:
*    QSEE_SUCCESS if successful, error code otherwise
*
******************************************************************************/
int qsee_query_rpmb_enablement(bool* b);

#endif /* QSEE_STOR_H */
