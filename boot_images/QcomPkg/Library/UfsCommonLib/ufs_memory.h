#ifndef __UFS_MEMORY_H
#define __UFS_MEMORY_H
/******************************************************************************
 * ufs_memory.h
 *
 * This file provides UFS storage device description data structure and
 * SCSI related data structure
 *
 * Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UfsCommonLib/ufs_memory.h#1 $
  $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------
2016-09-02   jt      Add secure write protect definitions 
2015-03-26   rh      Add ufs_get_descriptor_rpmb
2015-02-18   rh      Split set/poll for fDeviceInit
2014-09-30   rh      Adding RPMB APIs
2014-09-17   rh      Merge with UEFI driver changes
2014-04-22   rh      Adding the pre-prog config option
2013-11-05   rh      Adding the format API
2013-10-03   rh      Adding SCSI command REQUEST SENSE
2013-09-18   rh      Adding write for bConfigDescrLock attribute
2013-09-10   rh      Adding define for WRITE/READ buffer SCSI command
2012-12-18   rh      Initial creation

=============================================================================*/

#include "ufs_osal.h"

// Define SCSI commands
#define SCSI_CMD_TEST_UNIT_RDY      0x00
#define SCSI_CMD_REQUEST_SENSE      0x03
#define SCSI_CMD_FORMAT             0x04
#define SCSI_CMD_INQUIRY            0x12
#define SCSI_CMD_START_STOP_UNIT    0x1B
#define SCSI_CMD_READ10             0x28
#define SCSI_CMD_READ_CAP10         0x25     // Read Capacity
#define SCSI_CMD_SYNC_CACHE10       0x35
#define SCSI_CMD_UNMAP              0x42
#define SCSI_CMD_WRITE10            0x2A
#define SCSI_CMD_SECPROT_IN         0xA2     // Security Protocal in
#define SCSI_CMD_SECPROT_OUT        0xB5     // Security Protocal out
#define SCSI_CMD_REPORT_LUNS        0xA0
#define SCSI_CMD_WRITE_BUFFER       0x3B
#define SCSI_CMD_READ_BUFFER        0x3C

#define SCSI_STATUS_GOOD            0x00
#define SCSI_STATUS_CHK_COND        0x02
#define SCSI_STATUS_BUSY            0x08
#define SCSI_STATUS_TASK_SET_FULL   0x08

#define SCSI_FLAG_FUA               0x08

// UFS descriptor type ident value
#define UFS_DESC_IDN_DEVICE         0x00
#define UFS_DESC_IDN_CONFIGURATION  0x01
#define UFS_DESC_IDN_UNIT           0x02
#define UFS_DESC_IDN_INTERCONNECT   0x04
#define UFS_DESC_IDN_STRING         0x05
#define UFS_DESC_IDN_GEOMETRY       0x07
#define UFS_DESC_IDN_POWER          0x08

// Attributes definitions
#define UFS_IDN_bBootLunEn          0x00
#define UFS_IDN_bCurrentPowerMode   0x01
#define UFS_IDN_bActiveICCLevel     0x03
#define UFS_IDN_bPurgeStatus        0x06
#define UFS_IDN_bRefClkFreq         0x0a
#define UFS_IDN_bConfigDescrLock    0x0b

// Flags definitions
#define UFS_IDN_fDeviceInit         0x01
#define UFS_IDN_fPermanentWPEn      0x02
#define UFS_IDN_fPowerOnWPEn        0x03
#define UFS_IDN_fPurgeEnable        0x06

// Placement of the message byte is determined by if 
// this is a request or a response
// For request, the message is in bit [7:0] while bit [15:8] is zero
// For response, the message is in bit [15:8] while bit [7:0] is zero
#define RPMB_REQ_KEY_PROG           0x01
#define RPMB_REQ_READ_WR_CNT        0x02
#define RPMB_REQ_DATA_WRITE         0x03
#define RPMB_REQ_DATA_READ          0x04
#define RPMB_REQ_RESULT_READ        0x05
#define RPMB_REQ_SECURE_WP_WRITE    0x06
#define RPMB_REQ_SECURE_WP_READ     0x07

#define RPMB_RESULT_OK              0x00
#define RPMB_RESULT_FAILURE         0x01
#define RPMB_RESULT_AUTH_FAILURE    0x02
#define RPMB_RESULT_CNT_FAILURE     0x03
#define RPMB_RESULT_ADDR_FAILURE    0x04
#define RPMB_RESULT_WR_FAILURE      0x05
#define RPMB_RESULT_RD_FAILURE      0x06
#define RPMB_RESULT_NO_KEY          0x07

// SCSI Sense Keys
#define SENSE_KEY_NO_SENSE          0x0
#define SENSE_KEY_RECOVERED_ERROR   0x1
#define SENSE_KEY_NOT_READY         0x2
#define SENSE_KEY_MEDIUM_ERROR      0x3
#define SENSE_KEY_HARDWARE_ERROR    0x4
#define SENSE_KEY_ILLEGAL_REQUEST   0x5
#define SENSE_KEY_UNIT_ATTENTION    0x6
#define SENSE_KEY_DATA_PROTECT      0x7
#define SENSE_KEY_BLANK_CHECK       0x8
#define SENSE_KEY_VENDOR_SPECIFIC   0x9
#define SENSE_KEY_COPY_ABORTED      0xB
#define SENSE_KEY_VOLUME_OVERFLOW   0xD
#define SENSE_KEY_MISCOMPARE        0xE


// A data stucture used by the caller of the UFS driver as 
// a handle to access the APIs
typedef struct ufs_handle {
   struct ufs_host_info *phc;
   uint8_t        is_inuse;               // Inuse flag must be fist
   uint8_t        ufs_core_id;            // For this design, id = 0
   uint8_t        lun;
   uint32_t       id_magic;               // Magic # must be last

} ufs_handle_t;

// Function definitions

/* 
 * Initialize UFS memory device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success ENODEV when detection failed
 * */
int32_t ufs_mem_init (struct ufs_handle *hufs);

/* 
 * Read multiple blocks of data from UFS memory device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Output: buf  - Pointer to memory buffer for read operation
 * Input:  lba  - Logical block address for the first block
 * Input:  blocks - Number of blocks to read
 * Return: EOK for success EIO when read failed
 * */
int32_t ufs_scsi_read (struct ufs_handle *hufs, uint8_t *buf, 
                       uint32_t lba, uint32_t blocks);

/* 
 * Write mutiple blocks of data from UFS memory device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Input:  buf  - Pointer to memory buffer for write operation
 * Input:  lba  - Logical block address for the first block
 * Input:  blocks - Number of blocks to write
 * Return: EOK for success EIO when write failed
 * */
int32_t ufs_scsi_write (struct ufs_handle *hufs, uint8_t *buf, 
                        uint32_t lba, uint32_t blocks);

/* 
 * Unmap memory blocks from UFS memory device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Input:  lba  - Logical block address for the first block
 * Input:  blocks - Number of blocks to erase
 * Return: EOK for success EIO when erase failed
 * */
int32_t ufs_scsi_unmap (struct ufs_handle *hufs, 
                        uint32_t lba, uint32_t blocks);

/* 
 * Erase the entire UFS LU
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success EIO when erase failed
 * */
int32_t ufs_scsi_format (struct ufs_handle *hufs);

/* 
 * Test and see if the memory device is ready for operation
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EBUSY if the device is busy and
 *         EIO if IO error is encountered
 * */
int32_t ufs_scsi_unit_rdy (struct ufs_handle *hufs);
 
/* 
 * Set the new power condition for the UFS device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         power_cond: 1 - Active, 2 - UFS_sleep, 3 - PowerDown
 * Return: EOK for success, EBUSY if the device is busy and
 *         EIO if IO error is encountered
 * */
int32_t ufs_scsi_start_stop_unit (struct ufs_handle *hufs, 
                                  uint32_t power_cond);

/* 
 * Send the request sense command and decode the sense data
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EBUSY if the device is busy and
 *         EIO if IO error is encountered
 * */
int32_t ufs_scsi_req_sense (struct ufs_handle *hufs);

/* 
 * Get a list of all the active LUNs 
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_scsi_report_lun (struct ufs_handle *hufs);

/* 
 * Check the LUN supplied to see if it is one of the active LUN
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, ENOENT if LUN is not valid 
 * */
int32_t ufs_check_valid_lun (struct ufs_handle *hufs, uint8_t lun);

/* 
 * Get one of the string dscriptors from the UFS device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Input:  id - ID of the descriptor to get
 * Output: buf - Pointer to memory buffer to hold the descriptor info
 * Input:  size - Size of the memory buffer
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_str (struct ufs_handle *hufs, uint32_t id,
                                char *buf, uint32_t size);

/* 
 * Get one of the unit dscriptors from the UFS device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Input:  id - ID of the descriptor to get
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_unit (struct ufs_handle *hufs, uint32_t id);


/* 
 * Get the RPMB unit dscriptors from the UFS device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_rpmb (struct ufs_handle *hufs);


/* 
 * Get the device descriptor
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_device (struct ufs_handle *hufs);


/* 
 * Get the device configuration descriptor
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_config (struct ufs_handle *handle);


/* 
 * Get the geometry descriptor
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_descriptor_geometry (struct ufs_handle *hufs);

/* 
 * Get the flags  
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         info - UFS device information 
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_flags(struct ufs_handle *hufs, struct ufs_info_type *info);

/* 
 * Wait until device is responding to NOP UPIU requests
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_wait_device_awake (struct ufs_handle *handle);

/* 
 * Get the BOOT LUN configuration from the UFS device attribute
 * structure and store the value in the ufs_host_t data structure
 * The bBootLunEn flag identify which BOOT LUN (A or B) is mapped
 * to BOOT WLUN 
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_bBootLunEn (struct ufs_handle *hufs);

/* 
 * Set the boot LUN configuration for UFS device.  
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         id - which boot LUN to use: 
 *             0 : Disable BOOT WLUN
 *             1 : Set BOOT for A
 *             2 : Set BOOT for B
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_bBootLunEn (struct ufs_handle *hufs, uint8_t id);

/* 
 * Get the bConfigDescrLock from the UFS device attribute
 * structure and store the value in the ufs_host_t data structure
 * When bConfigDescrLock bit is set to 1, the device configuration
 * descriptor is locked
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_bConfigDescrLock (struct ufs_handle *hufs);

/* 
 * Set the bConfigDescrLock attribute for UFS device.  
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         val - 1 : Lock the congiguration descriptor 
 *             - This is a write once operation
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_bConfigDescrLock (struct ufs_handle *hufs, uint8_t val);

/* 
 * Set the fDeviceInit flag.  Setting this flag exits
 * the partial init mode for UFS device
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_fDeviceInit (struct ufs_handle *hufs);

/* 
 * Poll the fDeviceInit flag.  Blocking until the fDeviceInit
 * flag is back to 0.
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_poll_fDeviceInit (struct ufs_handle *hufs);

/* 
 * Set the fPowerOnWPEn flag.  Setting this flag enable
 * the Power On Write Protect fucntion.  The flag can not be
 * cleared without a power cycle
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_fPowerOnWPEn (struct ufs_handle *hufs);

/* 
 * Set the bLUWriteProtect field in the unit descriptor. 
 * Setting this field following by setting the fPowerOnWPEn 
 * flag enable the write protect feature
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         wp_setting - 0 : Not writeprotected
 *                      1 : Power on write protect
 *                      2 : Permanently write protected
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_bLUWriteProtect (struct ufs_handle *hufs, uint8_t wp_setting);

/* 
 * Set the fPurgeEnable flag.  Setting this flag starts
 * the purge fucntion.  The function will wait till the 
 * flag is resetted by the hardware to indicate completition
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_n_wait_fPurgeEnable (struct ufs_handle *hufs);

/* 
 * Send the scsi inquiry command and decode the returned value.
 * Result stored in the ufs_host_t data structure
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_scsi_inquiry (struct ufs_handle *hufs);

/* 
 * Function used to get the UFS device information, fill
 * in the ufs_info_type data structure
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         info - UFS device information
 * Return: EOK for success, EIO if IO error is encountered
 * */
void ufs_fill_in_device_info (struct ufs_handle *hufs, 
                             struct ufs_info_type *info);

/* 
 * Main function used for the initial provisioning of the UFS device
 * and configuration of the LU layout
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         cfg - LU configuration information
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_do_config_device( struct ufs_handle *hufs, 
                              struct ufs_config_descr *cfg );

/* 
 * Set the value of the bRefClkFreq
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         freq - RefClk frequency in Hz
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_bRefClkFreq (struct ufs_handle *hufs, uint32_t freq);

/* 
 * Send the scsi write buffer command 
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         buf - Pointer to data to write
 *         mode - Mode field in the SCSI CDB
 *         buf_id - Bufer ID field in the SCSI CDB
 *         offset - Buffer Offset field in the SCSI CDB
 *         len - Data transfer length in bytes
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_scsi_write_buf (struct ufs_handle *hufs, uint8_t *buf, 
                            uint8_t mode, uint8_t buf_id, 
                            uint32_t offset, uint32_t len);

/* 
 * Send the scsi read buffer command 
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         buf - Pointer to data to write
 *         mode - Mode field in the SCSI CDB
 *         buf_id - Bufer ID field in the SCSI CDB
 *         offset - Buffer Offset field in the SCSI CDB
 *         len - Data transfer length in bytes
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_scsi_read_buf (struct ufs_handle *hufs, uint8_t *buf, 
                           uint8_t mode, uint8_t buf_id, 
                           uint32_t offset, uint32_t len);

/* 
 * Set the UFS gear speed to operational speed
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_set_gear (struct ufs_handle *hufs);

/* 
 * Control the operation of the pre-programming mode
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         code - Vendor specific operation code
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_vend_pprog_ctrl (struct ufs_handle *hufs, uint32_t code);

/* 
 * Get UNIPRO/MPHY Capabilities
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success, EIO if IO error is encountered
 * */
int32_t ufs_get_phy_capabilities (struct ufs_handle *hufs);

/* 
 * Write the RPMB command message buffer out using 
 * security protocal out operation
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         msg_buf - Message buffer containing the message
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_mem_rpmb_msg_out (struct ufs_handle *hufs, uint8_t *msg_buf);

/* 
 * Read the RPMB commang message buffer in using 
 * security protocal in operation
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         msg_buf - Message buffer containing the message
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_mem_rpmb_msg_in (struct ufs_handle *hufs, uint8_t *msg_buf);

/* 
 * Read the write counter value
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         pkt_buf - Message buffer containing the RPMB response message
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_mem_rpmb_read_write_cnt (struct ufs_handle *hufs, uint8_t *pkt_buf);

/* 
 * Write data blocks
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         blocks - Number of data block to be transferred
 *         cmd_buf - Message buffer containing the command message + data
 *         rsp_buf - Response buffer containing the result of the operation
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_mem_rpmb_write_data (struct ufs_handle *hufs, uint32_t blocks, 
                                 uint8_t *cmd_buf, uint8_t *rsp_buf);

/* 
 * Read data blocks
 *
 * Input:  hufs - Pointer to the UFS handle structure
 *         pkt_buf - Message buffer containing the RPMB response message
 * Return: EOK for success, EIO if operation failed
 * */
int32_t ufs_mem_rpmb_read_data (struct ufs_handle *hufs, uint32_t blocks, 
                                uint8_t *cmd_buf, uint8_t *data_buf);

/* 
 * Enable non-blocking SCSI commands for the next transfer
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success
 * */
int32_t ufs_setup_pending_xfer (struct ufs_handle *hufs);

/* 
 * Poll any pending non-blocking transfer
 *
 * Input:  hufs - Pointer to the UFS handle structure
 * Return: EOK for success
 * */
int32_t ufs_finish_pending_xfer (struct ufs_handle *hufs);

#endif /* UFS_MEMORY_H */
