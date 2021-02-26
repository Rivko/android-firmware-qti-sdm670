/**********************************************************************
 *
 * UFS memory layer - 
 * Main UFS memory interface layer, contain the code to initialize
 * a UFS memory device, formation of SCSI command to read/write to 
 * memory device and decoding device descriptor strings
 * This layer work on the data structures to read/write data or to 
 * get the device descriptor
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-05-22   jt      Skip getting descriptors if we already have them
2016-10-01   jt      Update bBootLunEn locally after successful update 
2016-09-21   jt      Secure write protect changes  
2016-08-18   jt      Skip fDeviceInit for XBL core  
2016-08-01   jt      Deinit hba after UFS provisioning  
2016-07-28   jt      Add new geometry descriptor fields   
2016-05-18   jt      Set selector = 0 for ufs_read_attributes  
2016-01-25   rh      BlockIO2 debug update
2015-05-14   jt      Use ReliableWriteCount for UFS RPMB write  
2015-04-14   rh      Use no timeout for format command
2015-03-26   jt/rm   Fix ufs rpmb read write
2015-03-26   rh      Add ufs_get_descriptor_rpmb
2015-02-18   rh      Split set/poll for fDeviceInit
2014-09-17   rh      Merge with UEFI driver changes
2014-04-22   rh      Adding the pre-prog config option
2014-01-08   rh      Adding report for ENH memory type 3/4 scale factor
2013-11-05   rh      Adding the format API
2013-10-22   rh      Ensure command req structure is 0 before request
2013-10-22   rh      Provide a way for ufs_api.c to change current gear
2013-10-03   rh      Adding SCSI command REQUEST SENSE
2013-09-25   rh      Adding Toshiba specific bRefClkFreq override
2013-09-18   rh      Adding write for bConfigDescrLock attribute
2013-09-09   rh      Adding a bitfield to indicate which LUN is enabled
2013-09-05   rh      Fix a translation mistake on WLUN BOOT ID 
2013-09-04   rh      Adding enhanced area capacity adjustment factor
2013-09-03   rh      Add more debug message
2013-08-20   rh      64k limitation should be block, not byte
2013-08-12   rh      Unit descriptor is one byte short
2013-01-10   rh      Initial creation
=======================================================================*/

#include "ufs_osal.h"

#include "ufs_api.h"
#include "ufs_memory.h"
#include "ufs_core.h"
#include "ufs_errno.h"
#include "ufs_utp_core.h"

#include "ufs_bsp.h"
#include "HALhwio.h"

// Define the number of HCI controller
// Current hardware only have one UFS HCI
#define MAX_UFS_HCI_ID              1
#define MAX_UFS_NOP_PING_RETRY      2       // How many time to retry the NOP UPIU
#define MAX_UFS_WAIT_DEV_INIT_FLAG  1000
#define MAX_UFS_WAIT_PURGE_ENABLE   100000

// SCSI Transfer Sizes
#define MAX_SCSI_READ10_LENGTH      16383
#define MAX_SCSI_WRITE10_LENGTH     16383

// RPMB message specific defines
#define RPMB_MSG_SIZE               512
#define RPMB_SECTOR_SIZE            256

// Use a static buffer for storage of RPMB messages
static uint8_t rpmb_msg_buf[RPMB_MSG_SIZE];

// General static buffer used to hold PRDT transaction
uint8_t ufs_static_buf[128];

ufs_host_t ufs_host_info_array[MAX_UFS_HCI_ID]; 

// Reliable write count currently used for RPMB operations  
extern UINT32 ReliableWriteCount;

#define UFS_DESCRIPTOR_BUF_SIZE        256

#define GET_PHYADDR(x)    ((uint64_t)x)

int32_t ufs_mem_init (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   int32_t rc = EOK;

   // Check the validation of the core
   if (hufs == NULL) {
      return -ENODEV;
   }

// TODO: Use PCD to obtain MAX_UFS_HCI_ID
   if (hufs->ufs_core_id >= MAX_UFS_HCI_ID) {
      return -ENODEV;
   }

   hba = &ufs_host_info_array[hufs->ufs_core_id];

   //ufs_osal_sync_enter(&g_ufs_drv_lock);

   if (!hba->is_initialized) {

      // Clear the device info array
      osal_memset (ufs_host_info_array, 0, sizeof(ufs_host_t) * MAX_UFS_HCI_ID);
      
      hba->ufs_core_id = hufs->ufs_core_id;
         
      // Call BSP for initialization of Phy
      
      // Call init for UTP layer
      rc = ufs_utp_init (hba);

      // Get the device info structure and update SCM transfer handle
      hba->is_initialized = 1;
   }

   //ufs_osal_sync_leave(&g_ufs_drv_lock);

   hufs->phc = hba;

   return rc;
}

int32_t ufs_scsi_process_sense (ufs_host_t *hba, ufs_cmd_req_t *req)
{
   int32_t rc = EOK;
   uint16_t sense_len;
   uint16_t i;

   sense_len = (req->req_sense[0] << 8) | req->req_sense[1];
   // Check there is a valid sense data
   if (sense_len < 12 && sense_len > 18) {
      ufs_error_log(hba, 0x02a0);
      return -EPROTO;
   }
   // Keep a copy of the sense data
   for (i = 0; i < sense_len; i++) {
      hba->last_req_sense[i] = req->req_sense[i + 2];
   }
   // Verify response code
   if ((req->req_sense[2] & 0x7f) != 0x70) {
      ufs_error_log(hba, 0x02a1);
      return -EPROTO;
   }
   if ((req->req_sense[4] & 0x0f) == SENSE_KEY_UNIT_ATTENTION) {
      // Power on condition, not an error
      rc = EOK;
   }
   else {
      // All other error are flagged and reported
      rc = -EIO;
   }
   // Update the request status to make sense data has been processed
   req->status = 0;
   return rc;
}

int32_t ufs_setup_pending_xfer (struct ufs_handle *hufs)
{
   ufs_host_t *hba = hufs->phc;

   // Return error if there is already a pending transfer
   if (hba->xfer_pend == 1) {
      return -EBUSY;
   }

   hba->non_blocking_en = 1;
   hba->xfer_pend = 0;
   return EOK;
}

int32_t ufs_finish_pending_xfer (struct ufs_handle *hufs)
{
   ufs_host_t *hba = hufs->phc;
   int32_t rc = EOK;

   // Return error if there is no pending transfer
   if (hba->xfer_pend != 1) {
      return -EINVAL;
   }

   rc = ufs_poll_pending_xfer (hba, UPIU_TYPE_RESPONSE);
   return rc;
}

int32_t ufs_scsi_read (struct ufs_handle *hufs, uint8_t *buf, 
                       uint32_t lba, uint32_t blocks)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Maximum size of transfer supported by CMD_READ 10 is 16k blocks
   if (blocks > 65535) {
      return -EINVAL;
   }

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_READ10;        // Command
   cdb[1] = 0;    
   cdb[2] = (lba >> 24) & 0xff;     // LBA
   cdb[3] = (lba >> 16) & 0xff;
   cdb[4] = (lba >> 8) & 0xff;
   cdb[5] = (lba) & 0xff;
   cdb[6] = 0;    // Group Number
   cdb[7] = (blocks >> 8) & 0xff;   // Transfer Len
   cdb[8] = (blocks) & 0xff;  
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_READ;
   req.expected_len = blocks * DEFAULT_SECTOR_SIZE;
   req.data_buf_phy = (uint64_t)buf;      // Address of the destination buffer

   if (hba->slow_xfer) {
      hba->db_infinite_poll = 1;    // If system is slow, don't time out
   }

   rc = ufs_do_scsi_command (&req, cdb);
   hba->db_infinite_poll = 0;

   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   else if (rc == -EBUSY && hba->xfer_pend) {
      // Pending transfer, return status 
      return rc;
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0280);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0281);
      return -EIO;
   }

   return rc;
}


int32_t ufs_scsi_write (struct ufs_handle *hufs, uint8_t *buf, 
                        uint32_t lba, uint32_t blocks)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Maximum size of transfer supported by CMD_READ 10 is 16k blocks
   if (blocks > 65535) {
      return -EINVAL;
   }

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_WRITE10;       // Command
   cdb[1] = SCSI_FLAG_FUA;          // flags
   cdb[2] = (lba >> 24) & 0xff;     // LBA
   cdb[3] = (lba >> 16) & 0xff;
   cdb[4] = (lba >> 8) & 0xff;
   cdb[5] = (lba) & 0xff;
   cdb[6] = 0;                      // Group Number
   cdb[7] = (blocks >> 8) & 0xff;   // Transfer Len
   cdb[8] = (blocks) & 0xff;  
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_WRITE;
   req.expected_len = blocks * DEFAULT_SECTOR_SIZE;
   req.data_buf_phy = (uint64_t)buf;      // Address of the source buffer

   if (hba->slow_xfer) {
      hba->db_infinite_poll = 1;    // If system is slow, don't time out
   }

   rc = ufs_do_scsi_command (&req, cdb);
   hba->db_infinite_poll = 0;
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   else if (rc == -EBUSY && hba->xfer_pend) {
      // Pending transfer, return status 
      return rc;
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0284);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0285);
      return -EIO;
   }

   return rc;
}


int32_t ufs_scsi_unmap (struct ufs_handle *hufs, 
                        uint32_t lba, uint32_t blocks)
{
   uint8_t cdb[16];
   uint8_t *param = ufs_static_buf;  // Unmap parameter list, 1 item used only, 24 bytes in size
   const uint8_t param_size = 24;
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Create the parameter list with a single entry
   osal_memset (param, 0, param_size);
   param[0] = 0;                       // n = 23
   param[1] = 22;                      // n - 1
   param[2] = 0;                       // 
   param[3] = 16;                      // n - 7

   param[8]  = 0x0;                    // Upper 32 bits of LBA
   param[9]  = 0x0;
   param[10] = 0x0;
   param[11] = 0x0;
   param[12] = (lba >> 24) & 0xff;     // Lower 32 bits of LBA
   param[13] = (lba >> 16) & 0xff;
   param[14] = (lba >> 8) & 0xff;
   param[15] = (lba) & 0xff;
   param[16] = (blocks >> 24) & 0xff;  // Blocks
   param[17] = (blocks >> 16) & 0xff;
   param[18] = (blocks >> 8) & 0xff;
   param[19] = (blocks) & 0xff;
   // Reserve 4 bytes

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_UNMAP;         // Command
   cdb[1] = 0;                      // Anchor
   cdb[6] = 0;                      // Group Number
   cdb[7] = 0;                      // Transfer Len = n
   cdb[8] = param_size;      
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_WRITE;
   req.expected_len = param_size;
   req.data_buf_phy = GET_PHYADDR(param);    // Address of the param buffer

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0288);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0289);
      return -EIO;
   }

   return rc;
}

int32_t ufs_scsi_format (struct ufs_handle *hufs)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_FORMAT;        // Command
   cdb[1] = 0;                      // Parameter
   cdb[2] = 0;                      // Vendor Specific
   cdb[5] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = 0;
   req.expected_len = 0;
   req.data_buf_phy = 0;
   hba->db_infinite_poll = 1;             // Use infinite poll for format

   rc = ufs_do_scsi_command (&req, cdb);

   hba->db_infinite_poll = 0;             
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x02a8);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x02a9);
      return -EIO;
   }

   return rc;
}

int32_t ufs_scsi_write_buf (struct ufs_handle *hufs, uint8_t *buf, 
                            uint8_t mode, uint8_t buf_id, 
                            uint32_t offset, uint32_t len)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // SCSI write buffer command has a limit on block size of up to 2^24 bytes
   if (len > 0xffffff) {
      return -EINVAL;
   }

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_WRITE_BUFFER;  // Command
   cdb[1] = mode;
   cdb[2] = buf_id;
   cdb[3] = (offset >> 16) & 0xff;
   cdb[4] = (offset >> 8) & 0xff;
   cdb[5] = (offset) & 0xff;        // Transfer Offset
   cdb[6] = (len >> 16) & 0xff;  
   cdb[7] = (len >> 8) & 0xff;   
   cdb[8] = (len) & 0xff;        // Transfer Len
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_WRITE;
   req.expected_len = len;
   req.data_buf_phy = (uint64_t)buf;      // Address of the source buffer
   hba->db_infinite_poll = 1;             // Use infinite poll for write buf

   rc = ufs_do_scsi_command (&req, cdb);

   hba->db_infinite_poll = 0;             
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0294);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0295);
      return -EIO;
   }

   return rc;
}

int32_t ufs_scsi_read_buf (struct ufs_handle *hufs, uint8_t *buf, 
                            uint8_t mode, uint8_t buf_id, 
                            uint32_t offset, uint32_t len)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // SCSI write buffer command has a limit on block size of up to 2^24 bytes
   if (len > 0xffffff) {
      return -EINVAL;
   }

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_READ_BUFFER;  // Command
   cdb[1] = mode;
   cdb[2] = buf_id;
   cdb[3] = (offset >> 16) & 0xff;
   cdb[4] = (offset >> 8) & 0xff;
   cdb[5] = (offset) & 0xff;        // Transfer Offset
   cdb[6] = (len >> 16) & 0xff;  
   cdb[7] = (len >> 8) & 0xff;   
   cdb[8] = (len) & 0xff;        // Transfer Len
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_READ;
   req.expected_len = len;
   req.data_buf_phy = (uint64_t)buf;      // Address of the source buffer

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0298);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0299);
      return -EIO;
   }

   return rc;
}


// Test Unit Ready command
int32_t ufs_scsi_unit_rdy (struct ufs_handle *hufs)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_TEST_UNIT_RDY; // Command
   cdb[9] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = 0;                // No data transfer expected
   req.expected_len = 0;
   req.data_buf_phy = 0;      // Address of the param buffer

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x028c);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status == SCSI_STATUS_BUSY) {
      ufs_error_log(hba, 0x028d);
      return -EBUSY;
   }
   else if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x028e);
      return -EIO;
   }

   return rc;
}

// Start stop unit
int32_t ufs_scsi_start_stop_unit (struct ufs_handle *hufs, uint32_t power_cond)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_START_STOP_UNIT; // Command
   cdb[4] = (power_cond << 4) & 0xf0; // New power condition
   cdb[5] = 0;                        // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = UFS_WLUN_DEVICE;  // LUN is always for the device
   req.flags = 0;             // No data transfer expected
   req.expected_len = 0;
   req.data_buf_phy = 0;      // Address of the param buffer

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x027c);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status == SCSI_STATUS_BUSY) {
      ufs_error_log(hba, 0x027d);
      return -EBUSY;
   }
   else if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x027e);
      return -EIO;
   }

   return rc;
}

// Request Sense command
int32_t ufs_scsi_req_sense (struct ufs_handle *hufs)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   uint8_t *sense_buf = ufs_static_buf;
   const uint8_t sense_buf_size = 18;
   int32_t rc = EOK;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_REQUEST_SENSE; // Command
   cdb[4] = 18;                     // 18 bytes for sense data
   cdb[5] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_READ;
   // Transfer up to 20 bytes of sense data, expect 18
   req.expected_len = sense_buf_size;  
   req.data_buf_phy = GET_PHYADDR(sense_buf);

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      // Decode and record the sense data
      rc = ufs_scsi_process_sense (hba, &req);
   }

   if (rc != EOK) {
      ufs_error_log(hba, 0x029c);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status == SCSI_STATUS_BUSY) {
      ufs_error_log(hba, 0x029d);
      return -EBUSY;
   }
   else if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x029e);
      return -EIO;            
   }

   return rc;
}


// Get list of active LUNs
int32_t ufs_scsi_inquiry (struct ufs_handle *hufs)
{
   uint8_t cdb[16];
   uint8_t *param = ufs_static_buf;       // Inquiry string is 96 bytes
   uint8_t param_size = 96;
   ufs_host_t *hba;
   ufs_cmd_req_t req;
   int32_t rc = EOK;

   hba = hufs->phc;

   if (hba->has_scsi_inquiry) {
      return EOK;
   }

   // Create the parameter list with a single entry
   osal_memset (param, 0, param_size);

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_INQUIRY; // Command
   cdb[1] = 0;                      // EVPD
   cdb[3] = 0;                      // Pagecode
   cdb[4] = param_size;          // Transfer Len = param array size
   cdb[5] = 0;                      // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   req.phc = hufs->phc;
   req.lun = hufs->lun;
   req.flags = UFS_XFER_FLAGS_READ;
   req.expected_len = param_size;
   req.data_buf_phy = GET_PHYADDR(param);    // Address of the param buffer

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0201);
      return rc;
   }

   // Verify SCSI transfer status
   hba->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0202);
      return -EIO;
   }

   // Copy the Vendor ID and store it
   osal_memcpy (hba->inquiry_vid, param + 8, 28);
   hba->has_scsi_inquiry = 1;
   
   return rc;
}


// Set the gear speed to operational speed
int32_t ufs_set_gear (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   int32_t rc = EOK;

   hba = hufs->phc;
   
   if (hba->has_gear_switched)
      return rc;

   rc = ufs_utp_gear_sw (hba);
   if (rc != EOK) {
      ufs_error_log(hba, 0x02b0);
   }
   else {
      hba->has_gear_switched = 1;
   }

   return rc;
}


int32_t ufs_check_valid_lun (struct ufs_handle *hufs, uint8_t lun)
{
   ufs_host_t *hba = hufs->phc;

   // Check the default always enabled LUs
   if (lun == UFS_WLUN_REPORT ||
       lun == UFS_WLUN_DEVICE  ||
       lun == UFS_WLUN_RPMB) {
      return EOK;
   }
   
   // If it is boot LU, check to see if the BootLU is enabled
   if (lun == UFS_WLUN_BOOT && hba->attr.bBootLunEn != 0) {
      return EOK;
   }
   
   // Check the list of enabled LUs
   if (hba->desc_cfg.unit_cfg[lun].bLUEnable) {
      return EOK;
   }

   return -ENOENT;
}


// Get the device descriptor string
int32_t ufs_get_descriptor_str (struct ufs_handle *hufs, uint32_t id,
                                char *buf, uint32_t size)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint32_t xsize;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));

   // Limit the read size to 512 bytes
   xsize = size;
   if (xsize > 512) {
      xsize = 512;
   }

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = xsize;
   req.resp_data_buf = (uint8_t *)buf;
   req.idn = UFS_DESC_IDN_STRING;
   req.idx = id;

   rc = ufs_read_dev_descriptor (&req);
   return rc;
}

// Get device size, LUN size, Number of LUN activated, store in dev_t 
int32_t ufs_get_descriptor_unit (struct ufs_handle *hufs, uint32_t id)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[0x22];
   uint8_t *pt;
   uint64_t qLogicalBlockCount;
   uint32_t dEraseBlockSize;

   // LUN ID can be from 0 to 7 for UFS device
   if (id > 7) {
      return -EINVAL;
   }

   hba = hufs->phc;
   if (hba->has.has_desc_unit[id] == 1) {
      return EOK; 
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = sizeof(buf);
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_UNIT;
   req.idx = id;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0210);
      return rc;
   }

   // Check if the content of unit descriptor is reasonable
   if (buf[1] != 0x02) {
      ufs_error_log(hba, 0x0211);
      return -EIO;
   }

   // Record the LUN in use flag and size
   pt = &buf[0xb];
   qLogicalBlockCount =  (uint64_t)*pt++ << 56;
   qLogicalBlockCount |= (uint64_t)*pt++ << 48;
   qLogicalBlockCount |= (uint64_t)*pt++ << 40;
   qLogicalBlockCount |= (uint64_t)*pt++ << 32;
   qLogicalBlockCount |= (uint64_t)*pt++ << 24;
   qLogicalBlockCount |= (uint64_t)*pt++ << 16;
   qLogicalBlockCount |= (uint64_t)*pt++ << 8;
   qLogicalBlockCount |= (uint64_t)*pt++;

   hba->desc_unit[id].qLogicalBlockCount = qLogicalBlockCount;

   pt = &buf[0x13];
   dEraseBlockSize = *pt++ << 24;
   dEraseBlockSize |= *pt++ << 16;
   dEraseBlockSize |= *pt++ << 8;
   dEraseBlockSize |= *pt++;
   
   hba->desc_unit[id].dEraseBlockSize = dEraseBlockSize;

   hba->has.has_desc_unit[id] = 1;
   return rc;
}

// Get the RPMB unit descriptor 
int32_t ufs_get_descriptor_rpmb (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[0x22];
   uint8_t *pt;
   uint64_t qLogicalBlockCount;
   uint32_t dEraseBlockSize;

   hba = hufs->phc;
   if (hba->has.has_desc_rpmb_unit) {
      return EOK; 
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = sizeof(buf);
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_UNIT;
   req.idx = UFS_WLUN_RPMB;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0218);
      return rc;
   }

   // Check if the content of unit descriptor is reasonable
   if (buf[1] != 0x02) {
      ufs_error_log(hba, 0x0219);
      return -EIO;
   }

   // Record the LUN in use flag and size
   pt = &buf[0xb];
   qLogicalBlockCount =  (uint64_t)*pt++ << 56;
   qLogicalBlockCount |= (uint64_t)*pt++ << 48;
   qLogicalBlockCount |= (uint64_t)*pt++ << 40;
   qLogicalBlockCount |= (uint64_t)*pt++ << 32;
   qLogicalBlockCount |= (uint64_t)*pt++ << 24;
   qLogicalBlockCount |= (uint64_t)*pt++ << 16;
   qLogicalBlockCount |= (uint64_t)*pt++ << 8;
   qLogicalBlockCount |= (uint64_t)*pt++;

   hba->desc_rpmb.qLogicalBlockCount = qLogicalBlockCount;

   pt = &buf[0x13];
   dEraseBlockSize = *pt++ << 24;
   dEraseBlockSize |= *pt++ << 16;
   dEraseBlockSize |= *pt++ << 8;
   dEraseBlockSize |= *pt++;
   
   hba->desc_rpmb.dEraseBlockSize = dEraseBlockSize;
   hba->desc_rpmb.bLogicalBlockSize = buf[0x0a];

   hba->has.has_desc_rpmb_unit = 1;
   return rc;
}

// Get device descriptor (1st configuration)
int32_t ufs_get_descriptor_device (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[0x20];

   hba = hufs->phc;
   if (hba->has.has_desc_device) {
      return EOK;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   osal_memset (buf, 0, sizeof(buf));

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = 0x1C;
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_DEVICE;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0212);
      return rc;
   }

   // Decode the buffer content
   if (buf[1] != 0x00) {
      // Device descriptor has bDescriptorType of 0x00
      ufs_error_log(hba, 0x0213);
      return -EIO;
   }

   // Record the LUN in use flag and size
   hba->desc_dev.bNumberLU = buf[0x06];
   hba->desc_dev.bNumberWLU = buf[0x07];
   hba->desc_dev.bBootEnable = buf[0x08];
   hba->desc_dev.bDescrAccessEn = buf[0x09];
   hba->desc_dev.wSpecVersion = (buf[0x10] << 8) | buf[0x11];
   hba->desc_dev.iManufacturerName = buf[0x14];
   hba->desc_dev.iProductName = buf[0x15];
   hba->desc_dev.iSerialNumber = buf[0x16];
   hba->desc_dev.iOemID = buf[0x17];
   hba->desc_dev.wManufacturerID = (buf[0x18] << 8) | buf[0x19];
   hba->has.has_desc_device = 1;
   return rc;
}


// Get configuration descriptor
int32_t ufs_get_descriptor_config (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[144];
   uint8_t *pt;
   int i;

   hba = hufs->phc;
   if (hba->has.has_desc_config) {
      return EOK;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   osal_memset (buf, 0, sizeof (buf));

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = sizeof(buf);
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_CONFIGURATION;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0220);
      return rc;
   }

   // Verify the content of the buffer is configuration descriptor
   if (buf[1] != 0x01) {
      // Configuration descriptor have identifier of 1
      ufs_error_log(hba, 0x0221);
      return -EIO;
   }

   // Decode the device configuration
   pt = &buf[2];
   hba->desc_cfg.dev_cfg.bDescriptorType = *pt++;
   hba->desc_cfg.dev_cfg.bBootEnable = *pt++;
   hba->desc_cfg.dev_cfg.bDescrAccessEn = *pt++;
   hba->desc_cfg.dev_cfg.bInitPowerMode = *pt++;
   hba->desc_cfg.dev_cfg.bHighPriorityLUN = *pt++;
   hba->desc_cfg.dev_cfg.bSecureRemovalType = *pt++;
   hba->desc_cfg.dev_cfg.bInitActiveICCLevel = *pt++;
   hba->desc_cfg.dev_cfg.wPeriodicRTCUpdate = *pt++ << 8;
   hba->desc_cfg.dev_cfg.wPeriodicRTCUpdate |= *pt++;

   for (i = 0; i < 8; i++) {
      pt = &buf[0x10 * i + 0x10];

      hba->desc_cfg.unit_cfg[i].bLUEnable = *pt++;
      hba->desc_cfg.unit_cfg[i].bBootLunID = *pt++;
      hba->desc_cfg.unit_cfg[i].bLUWriteProtect = *pt++;
      hba->desc_cfg.unit_cfg[i].bMemoryType = *pt++;
      hba->desc_cfg.unit_cfg[i].dNumAllocUnits = *pt++ << 24;
      hba->desc_cfg.unit_cfg[i].dNumAllocUnits |= *pt++ << 16;
      hba->desc_cfg.unit_cfg[i].dNumAllocUnits |= *pt++ << 8;
      hba->desc_cfg.unit_cfg[i].dNumAllocUnits |= *pt++;
      hba->desc_cfg.unit_cfg[i].bDataReliability = *pt++;
      hba->desc_cfg.unit_cfg[i].bLogicalBlockSize = *pt++;
      hba->desc_cfg.unit_cfg[i].bProvisioningType = *pt++;
      hba->desc_cfg.unit_cfg[i].wContextCapabilities = *pt++ << 8;
      hba->desc_cfg.unit_cfg[i].wContextCapabilities |= *pt++;
   }

   hba->has.has_desc_config = 1;

   return rc;
}


int32_t ufs_get_descriptor_geometry (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[0x48] = {0}; 

   hba = hufs->phc;
   if (hba->has.has_desc_geometry) {
      return EOK;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));

   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   if (hba->desc_dev.wSpecVersion >= 0x210) 
   {
      req.resp_data_len = sizeof(buf);
   }
   else
   {
      req.resp_data_len = 0x44;
   }
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_GEOMETRY;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0222);
      return rc;
   }

   // Geometry descriptor have identifier of 7
   if (buf[1] != 0x07) {
      ufs_error_log(hba, 0x0223);
      return -EIO;
   }

   // Record the LUN in use flag and size
   if (buf[0xc] == 0) 
   {
      hba->desc_geo.bMaxNumberLU = 8;
   }
   else if (buf[0xc] == 1) 
   {
      hba->desc_geo.bMaxNumberLU = 32;
   }

   hba->desc_geo.dSegmentSize =  (uint32_t)buf[0x10] | 
                                ((uint32_t)buf[0x0f] << 8) | 
                                ((uint32_t)buf[0x0e] << 16) | 
                                ((uint32_t)buf[0x0d] << 24);
   hba->desc_geo.qTotalRawDeviceCapacity =  
           (uint64_t)buf[0x0b] | ((uint64_t)buf[0x0a] << 8) | 
           ((uint64_t)buf[0x09] << 16) | ((uint64_t)buf[0x08] << 24) |
           ((uint64_t)buf[0x07] << 32) | ((uint64_t)buf[0x06] << 40) |
           ((uint64_t)buf[0x05] << 48) | ((uint64_t)buf[0x04] << 56);
   hba->desc_geo.bAllocationUnitSize = buf[0x11];
   hba->desc_geo.bMinAddrBlockSize = buf[0x12];
   hba->desc_geo.bRPMB_ReadWriteSize = buf[0x17];

   hba->desc_geo.wSupportedMemoryTypes = (uint16_t)buf[0x1f] | 
                                        ((uint16_t)buf[0x1e] << 8) ;
   hba->desc_geo.dEnhanced1MaxNAllocU =  (uint32_t)buf[0x2f] | 
                                        ((uint32_t)buf[0x2e] << 8) | 
                                        ((uint32_t)buf[0x2d] << 16) | 
                                        ((uint32_t)buf[0x2c] << 24);
   hba->desc_geo.wEnhanced1CapAdjFac =   (uint16_t)buf[0x31] | 
                                        ((uint16_t)buf[0x30] << 8) ;
   hba->desc_geo.dEnhanced2MaxNAllocU =  (uint32_t)buf[0x35] | 
                                        ((uint32_t)buf[0x34] << 8) | 
                                        ((uint32_t)buf[0x33] << 16) | 
                                        ((uint32_t)buf[0x32] << 24);
   hba->desc_geo.wEnhanced2CapAdjFac =   (uint16_t)buf[0x37] | 
                                        ((uint16_t)buf[0x36] << 8) ;

   hba->desc_geo.dEnhanced3MaxNAllocU =  (uint32_t)buf[0x3b] | 
                                        ((uint32_t)buf[0x3a] << 8) | 
                                        ((uint32_t)buf[0x39] << 16) | 
                                        ((uint32_t)buf[0x38] << 24);
   hba->desc_geo.wEnhanced3CapAdjFac =   (uint16_t)buf[0x3d] | 
                                        ((uint16_t)buf[0x3c] << 8) ;
   hba->desc_geo.dEnhanced3MaxNAllocU =  (uint32_t)buf[0x41] | 
                                        ((uint32_t)buf[0x40] << 8) | 
                                        ((uint32_t)buf[0x3f] << 16) | 
                                        ((uint32_t)buf[0x3e] << 24);
   hba->desc_geo.wEnhanced3CapAdjFac =   (uint16_t)buf[0x43] | 
                                        ((uint16_t)buf[0x42] << 8) ;

   hba->desc_geo.dOptimalLogicalBlockSize = (uint32_t)buf[0x47] | 
                                        ((uint32_t)buf[0x46] << 8) | 
                                        ((uint32_t)buf[0x45] << 16) | 
                                        ((uint32_t)buf[0x44] << 24);

   hba->has.has_desc_geometry = 1;

   return rc;
}

int32_t ufs_get_flags(struct ufs_handle *hufs, struct ufs_info_type *info)
{
   ufs_attr_req_t req;
   ufs_host_t *hba;
   int32_t rc = EOK;

   hba = hufs->phc;

   if (hba->need_flags.need_fPowerOnWPEn) 
   {
      /* Get fPowerOnWPEn flag  */
      osal_memset (&req, 0, sizeof (ufs_attr_req_t));
      req.phc = hufs->phc;
      req.idn = UFS_IDN_fPowerOnWPEn;
      req.idx = 0;
      req.val = 0;

      rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_READ);
      if (rc != EOK) {
         //ufs_error_log(hba, 0x02bc); get new error
         return rc; 
      }

      hba->flag.fPowerOnWPEn = req.val; 
      hba->need_flags.need_fPowerOnWPEn = 0; 
      info->fPowerOnWPEn = hba->flag.fPowerOnWPEn; 
   }

   if (hba->need_flags.need_fPermanentWPEn) 
   {
      /* Get fPermanentWPEn flag */
      osal_memset (&req, 0, sizeof (ufs_attr_req_t));
      req.phc = hufs->phc;
      req.idn = UFS_IDN_fPermanentWPEn;
      req.idx = 0;
      req.val = 0;

      rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_READ);
      if (rc != EOK) {
         //ufs_error_log(hba, 0x02bc); get new error
         return rc; 
      }

      hba->flag.fPermanentWPEn = req.val; 
      hba->need_flags.need_fPermanentWPEn = 0; 
      info->fPermanentWPEn = hba->flag.fPermanentWPEn; 
   }

   return rc;
}


int32_t ufs_get_phy_capabilities (struct ufs_handle *hufs)
{
   ufs_host_t *hba;
   int32_t rc = EOK;

   hba = hufs->phc;

   if (hufs->phc->has_caps) {
      return EOK;
   }

   rc = ufs_utp_get_host_caps (hba);
   if (rc != EOK) {
      ufs_error_log(hba, 0x02bc);
   }

   hufs->phc->has_caps = 1;

   return rc;
}

// Wait until device is responding to NOP UPIU pings
int32_t ufs_wait_device_awake (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;
   int32_t i;

   if (hufs->phc->device_awake) {
      return EOK; 
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;

   for (i = 0; i < MAX_UFS_NOP_PING_RETRY; i++) {
      // Ping using NOP_OUT UPIU each try
      rc = ufs_send_nop_ping (&req);
      if (rc == EOK) {
         hufs->phc->device_awake = 1; 
         return rc;
      }
   }
   
   ufs_error_log(req.phc, 0x0228);
   return rc;
}

// Get the bBootLunEn field in the attributes
int32_t ufs_get_bBootLunEn (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   if (hufs->phc->has_attr.has_bBootLunEn) {
      return EOK;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_bBootLunEn;
   req.idx = 0;
   req.val = 0;
   req.sel = 0; 

   rc = ufs_read_attributes (&req);

   hufs->phc->attr.bBootLunEn = req.val;
   hufs->phc->has_attr.has_bBootLunEn = 1;
   return rc;
}

// Set the bBootLunEn field in the attributes
int32_t ufs_set_bBootLunEn (struct ufs_handle *hufs, uint8_t id)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_bBootLunEn;
   req.idx = 0;
   req.val = id;

   rc = ufs_write_attributes (&req);

   // If attribute written successfully, update bBootLunEn locally
   if (rc == EOK) {
      hufs->phc->attr.bBootLunEn = id; 
   }

   return rc;
}

// Get the bConfigDescrLock field in the attributes
int32_t ufs_get_bConfigDescrLock (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   if (hufs->phc->has_attr.has_bConfigDescrLock) {
      return EOK;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_bConfigDescrLock;
   req.idx = 0;
   req.val = 0;
   req.sel = 0; 

   rc = ufs_read_attributes (&req);

   hufs->phc->attr.bConfigDescrLock = req.val;
   hufs->phc->has_attr.has_bConfigDescrLock = 1;
   return rc;
}

// Set the bConfigDescrLock field in the attributes
int32_t ufs_set_bConfigDescrLock (struct ufs_handle *hufs, uint8_t val)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_bConfigDescrLock;
   req.idx = 0;
   req.val = val;

   rc = ufs_write_attributes (&req);
   return rc;
}

// Set the bRefClkFreq field in the attributes 
int32_t ufs_set_bRefClkFreq (struct ufs_handle *hufs, uint32_t freq)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_bRefClkFreq;
   req.idx = 0;
   req.sel = 0; 

   rc = ufs_read_attributes (&req);

   switch (freq) {
      case 19200000:
         if (req.val == 0) {
            return EOK;          // If value is set correctly, exit immidiately
         }
         req.val = 0;
         break;
      case 26000000:
         if (req.val == 1) {
            return EOK;
         }
         req.val = 1;
         break;
      case 38400000:
         if (req.val == 2) {
            return EOK;
         }
         req.val = 2;
         break;
      case 52000000:
         if (req.val == 3) {
            return EOK;
         }
         req.val = 3;
         break;
      default:
         return -EINVAL;
   }

   rc = ufs_write_attributes (&req);
   return rc;
}

// Set the fDeviceInit field in the flags and wait for the bit to set
int32_t ufs_set_fDeviceInit (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;
   ufs_host_t *hba;

   hba = hufs->phc;

   // If we are in XBL core, we can skip this 
   if (hba->core_skip) {
      return EOK; 
   }

   // Check if the internal fDevicInit flag is set, skip the entire function
   if (hba->fDeviceInit_sent) {
      return EOK;
   }

   if (hba->xfer_pend) {
      hba->fDeviceInit_async = 1; 
      rc = ufs_poll_pending_xfer (hba, UPIU_TYPE_QUERY_RESP);
      if (rc != EOK) {
         ufs_error_log(hba, 0x0230);
         return rc; 
      }
   }
   else
   {
      memset (&req, 0, sizeof (ufs_attr_req_t));
      req.phc = hufs->phc;
      req.idn = UFS_IDN_fDeviceInit;
      req.idx = 0;
      req.val = 0;

      // fDeviceInit is not set, sent set flag UPIU
      rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_SET);
      if (rc == -EBUSY && hba->xfer_pend) {
         // Pending transfer, return status
         return rc;
      }
      if (rc != EOK) {
         ufs_error_log(hba, 0x0255);
         return rc;
      }
   }

   hba->fDeviceInit_sent = 1;
   return rc;
}

// Poll the fDeviceInit field and wait for the bit to set
int32_t ufs_poll_fDeviceInit (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;
   int32_t i;
   ufs_host_t *hba;

   hba = hufs->phc;

   // If we are in XBL core, we can skip this 
   if (hba->core_skip) {
      return EOK; 
   }

   // Check if the internal fDevicInit flag is set, skip the entire function
   if (hba->fDeviceInit_done) {
      return EOK;
   }

   memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_fDeviceInit;
   req.idx = 0;
   req.val = 0;

   // Loop until flag is cleared
   i = MAX_UFS_WAIT_DEV_INIT_FLAG;
   do {
      rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_READ);
      if (rc != EOK) {
         return rc;
      }
      if (req.val == 0) {
         hba->fDeviceInit_done = 1;
         return EOK;
      }
   } while (i--);

   ufs_error_log(hba, 0x0231);
   // TIMEOUT
   return -ETIME;
}

// Set the fPowerOnWPEn field in the flags 
int32_t ufs_set_fPowerOnWPEn (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   req.phc = hufs->phc;
   req.idn = UFS_IDN_fPowerOnWPEn;
   req.idx = 0;
   req.val = 0;

   rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_SET);

   // If successful, we want to get the new value of the flag 
   if (rc == EOK) {
      hufs->phc->need_flags.need_fPowerOnWPEn = 1; 
   }

   return rc;
}

// Set and wait the fPurgeEnable field in the flags 
int32_t ufs_set_n_wait_fPurgeEnable (struct ufs_handle *hufs)
{
   ufs_attr_req_t req;
   int32_t rc = EOK;
   int32_t i;
   ufs_host_t *hba = hufs->phc;
   uint8_t non_blocking_wait = 0; 

   // Non_block_en needs to be 0 to ensure each flag operation is completed in sync, so 
   // use a separate variable to keep track. xfer_pend also needs to be 0, so that 
   // we can query the attributes to check the purge status   
   if (hba->non_blocking_en == 1) {
      non_blocking_wait = 1; 
      hba->non_blocking_en = 0; 
      hba->xfer_pend = 0; 
   }

   // If non-blocking and purge flag not set, set it now
   if ((non_blocking_wait == 1) && (hba->purge_set == 0)) {
      osal_memset (&req, 0, sizeof (ufs_attr_req_t));
      req.phc = hufs->phc;
      req.idn = UFS_IDN_fPurgeEnable;
      req.idx = 0;
      req.val = 0;

      rc = ufs_dev_flag_op (&req, UPIU_FLAG_OP_SET);
      if (rc != EOK) {
         return rc;
      }

      hba->purge_set = 1; 
   }

   i = MAX_UFS_WAIT_PURGE_ENABLE;
   do {
      req.phc = hufs->phc;
      req.idn = UFS_IDN_bPurgeStatus;
      req.idx = 0;
      req.val = 0;
      req.sel = 0; 

      rc = ufs_read_attributes (&req);
      if (rc != EOK) {
         ufs_error_log(hba, 0x0237); 
         return rc;
      }

      hufs->phc->attr.bPurgeStatus = req.val;
      if (req.val == 1) {
         if (non_blocking_wait == 1) {
            hba->non_blocking_en = 1; 
            hba->xfer_pend = 1; 
            return -EBUSY;       // Indicate pending 
         }
         // Wait for some more time
         ufs_bsp_busy_wait (1000);
         continue;
      }
      else if (req.val == 0 || req.val == 3) {
         // Operation completed
         hba->xfer_pend = 0;
         hba->non_blocking_en = 0; 
         hba->purge_set = 0; 
         return rc;
      }
      else {
         // Operation failed 
         ufs_error_log(hba, 0x0239); 
         return -EIO;
      }
   } while (i--);

   ufs_error_log(hba, 0x0238);   // Timeout
   return -ETIME;
}

// Set the LUN write protect by updating the unit device configuration
int32_t ufs_set_bLUWriteProtect (struct ufs_handle *hufs, uint8_t wp_setting)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[16 * 8 + 16];     
   // 8 LUN (Each 16 bytes) + Configuration descriptor (16 bytes)

   // LUN ID can be from 0 to 7 for UFS device
   if (hufs->lun > 7) {
      return -EINVAL;
   }

   // WP setting can be either 0, 1, or 2
   if (wp_setting > 2) {
      return -EINVAL;
   }

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));
   hba = hufs->phc;
   req.phc = hufs->phc;
   req.req_data_len = 0;
   req.req_data_buf = NULL;
   req.resp_data_len = sizeof (buf);
   req.resp_data_buf = buf;
   req.idn = UFS_DESC_IDN_CONFIGURATION;
   req.idx = hufs->lun;

   rc = ufs_read_dev_descriptor (&req);
   if (rc != EOK) {
      return rc;
   }

   // Decode the buffer content
   if (buf[0] != 0x22) {
      // Unit descriptor is always 0x22 bytes long
      ufs_error_log(hba, 0x0232);
      return -EIO;
   }

   // Update the bLUWriteProtect field
   buf[16 + hufs->lun * 16 + 2] = wp_setting;

   // Write the updated data back
   req.phc = hba;
   req.req_data_len = sizeof (buf);
   req.req_data_buf = buf;
   req.resp_data_len = 0;
   req.resp_data_buf = NULL;
   req.idn = UFS_DESC_IDN_CONFIGURATION;
   req.idx = hufs->lun;
   rc = ufs_write_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0233);
   }
   return rc;
}

// Write the RPMB message buffer out using security protocal out operation
int32_t ufs_mem_rpmb_msg_out (struct ufs_handle *hufs, uint8_t *msg_buf)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   const uint32_t len = RPMB_MSG_SIZE;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_SECPROT_OUT;   // Command
   cdb[1] = 0xEC;                   // Security Protocal
   cdb[2] = 0x00;                   // 
   cdb[3] = 0x01;                   // Security Protocal Specific
   cdb[4] = 0x00;                   // Reserved
   cdb[5] = 0x00;                   // Reserved
   cdb[6] = (len >> 24) & 0xff;     // 
   cdb[7] = (len >> 16) & 0xff;     // 
   cdb[8] = (len >> 8) & 0xff;      // 
   cdb[9] = (len) & 0xff;           // Transfer Length
   cdb[10] = 0;                     // Reserve
   cdb[11] = 0;                     // Control

   osal_memset (&req, 0, sizeof (ufs_cmd_req_t));
   req.phc = hufs->phc;
   hba = hufs->phc;
   req.lun = UFS_WLUN_RPMB;
   req.flags = UFS_XFER_FLAGS_WRITE;
   req.expected_len = RPMB_MSG_SIZE;
   req.data_buf_phy = GET_PHYADDR (msg_buf);   

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0240);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0241);
      return -EIO;
   }

   return rc;
}

// Read the RPMB message buffer in using security protocal in operation
int32_t ufs_mem_rpmb_msg_in (struct ufs_handle *hufs, uint8_t *msg_buf)
{
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   int32_t rc = EOK;
   const uint32_t len = RPMB_MSG_SIZE;
   ufs_host_t *hba;

   // Fill in the CDB with SCSI command structure
   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_SECPROT_IN;    // Command
   cdb[1] = 0xEC;                   // Security Protocal
   cdb[2] = 0x00;                   // 
   cdb[3] = 0x01;                   // Security Protocal Specific
   cdb[4] = 0x00;                   // Reserved
   cdb[5] = 0x00;                   // Reserved
   cdb[6] = (len >> 24) & 0xff;     // 
   cdb[7] = (len >> 16) & 0xff;     // 
   cdb[8] = (len >> 8) & 0xff;      // 
   cdb[9] = (len) & 0xff;           // Transfer Length
   cdb[10] = 0;                     // Reserve
   cdb[11] = 0;                     // Control

   req.phc = hufs->phc;
   hba = hufs->phc;
   req.lun = UFS_WLUN_RPMB;
   req.flags = UFS_XFER_FLAGS_READ;
   req.expected_len = RPMB_MSG_SIZE;
   req.data_buf_phy = GET_PHYADDR(msg_buf);   

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0248);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0249);
      return -EIO;
   }

   return rc;
}

int32_t ufs_mem_rpmb_read_write_cnt (struct ufs_handle *hufs, uint8_t *pkt_buf)
{
   int32_t rc = EOK;
   uint16_t rst;
   ufs_host_t *hba;

   hba = hufs->phc;

   // Fill in the RPMB message buffer
   osal_memset (rpmb_msg_buf, 0, RPMB_MSG_SIZE);
   rpmb_msg_buf[510] = 0x00;
   rpmb_msg_buf[511] = RPMB_REQ_READ_WR_CNT;

   rc = ufs_mem_rpmb_msg_out (hufs, rpmb_msg_buf);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0250);
      return rc;
   }

   // Get the STATUS by issue a security protocal in message
   osal_memset (pkt_buf, 0, RPMB_MSG_SIZE);

   rc = ufs_mem_rpmb_msg_in (hufs, pkt_buf);

   // Check the mesage type to make sure it is good
   if (pkt_buf[510] != RPMB_REQ_READ_WR_CNT ||
       pkt_buf[511] != 0x00) {
      ufs_error_log(hba, 0x0251);
      return -EIO;
   }

   // Check the return status to make sure it is good
   rst = (pkt_buf[508] << 8) | pkt_buf[509];
   hufs->phc->last_rpmb_status = rst;
   if (rst != RPMB_RESULT_OK) {
      ufs_error_log(hba, 0x0252);
      return -EIO;
   }

   // Return the count value
   //*write_cnt = (rpmb_msg_buf[500] << 24) | (rpmb_msg_buf[501] << 16) | 
   //             (rpmb_msg_buf[502] << 8)  | (rpmb_msg_buf[503]);

   return rc;
}

int32_t ufs_mem_rpmb_write_key (struct ufs_handle *hufs, uint8_t *key)
{
   int32_t rc = EOK;
   uint16_t rst;
   ufs_host_t *hba;

   hba = hufs->phc;

   // Fill in the RPMB message buffer
   osal_memset (rpmb_msg_buf, 0, RPMB_MSG_SIZE);
   rpmb_msg_buf[510] = 0x00;
   rpmb_msg_buf[511] = RPMB_REQ_READ_WR_CNT;

   rc = ufs_mem_rpmb_msg_out (hufs, rpmb_msg_buf);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0258);
      return rc;
   }

   // Get the STATUS by issue a security protocal in message
   osal_memset (rpmb_msg_buf, 0, RPMB_MSG_SIZE);

   rc = ufs_mem_rpmb_msg_in (hufs, rpmb_msg_buf);

   // Check the mesage type to make sure it is good
   if (rpmb_msg_buf[510] != RPMB_REQ_READ_WR_CNT ||
       rpmb_msg_buf[511] != 0x00) {
      ufs_error_log(hba, 0x0259);
      return -EIO;
   }

   // Check the return status to make sure it is good
   rst = (rpmb_msg_buf[508] << 8) | rpmb_msg_buf[509];
   hufs->phc->last_rpmb_status = rst;
   if (rst != RPMB_RESULT_OK) {
      ufs_error_log(hba, 0x025a);
      return -EIO;
   }

   return rc;
}

int32_t ufs_mem_rpmb_write_data (struct ufs_handle *hufs, 
                                 uint32_t blocks, 
                                 uint8_t *cmd_buf, 
                                 uint8_t *rsp_buf)
{
   int32_t rc = EOK;
   ufs_host_t *hba = hufs->phc;
   uint16_t rst;
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   uint32_t len;
   uint32_t current_bl;
   uint32_t total_bl;
   uint32_t offset = 0;
   uint8_t result_request[RPMB_MSG_SIZE];
   
   total_bl = blocks;
   while (total_bl) {
      #if 0 
      current_bl = total_bl > hba->desc_geo.bRPMB_ReadWriteSize ? 
                   hba->desc_geo.bRPMB_ReadWriteSize : total_bl;
      #endif 
      current_bl = ReliableWriteCount; 
      total_bl -= current_bl;
      len = current_bl * 512;          // Length is 512 bytes per block to transfer

      // Fill in the CDB with SCSI command structure
      osal_memset (cdb, 0, sizeof(cdb));
      cdb[0] = SCSI_CMD_SECPROT_OUT;   // Command
      cdb[1] = 0xEC;                   // Security Protocal
      cdb[2] = 0x00;                   // 
      cdb[3] = 0x01;                   // Security Protocal Specific
      cdb[4] = 0x00;                   // Reserved
      cdb[5] = 0x00;                   // Reserved
      cdb[6] = (len >> 24) & 0xff;     // 
      cdb[7] = (len >> 16) & 0xff;     // 
      cdb[8] = (len >> 8) & 0xff;      // 
      cdb[9] = (len) & 0xff;           // Transfer Length
      cdb[10] = 0;                     // Reserve
      cdb[11] = 0;                     // Control

      osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
      req.phc = hufs->phc;
      req.lun = UFS_WLUN_RPMB;
      req.flags = UFS_XFER_FLAGS_WRITE;
      req.expected_len = RPMB_MSG_SIZE * current_bl;
      req.data_buf_phy = GET_PHYADDR(cmd_buf + RPMB_MSG_SIZE * offset);   

      rc = ufs_do_scsi_command (&req, cdb);
      if (rc == -ECHKCND) {
         rc = ufs_scsi_process_sense (hba, &req);
      }
      if (rc != EOK) {
         ufs_error_log(hba, 0x0261);
         return rc;
      }

      // Verify SCSI transfer status
      hufs->phc->last_scsi_status = req.status;
      if (req.status != SCSI_STATUS_GOOD) {
         ufs_error_log(hba, 0x0262);
         return -EIO;
      }
      
      // Send result register read request
      osal_memset (result_request, 0, RPMB_MSG_SIZE);
      result_request[511] = 0x05;
      
      rc = ufs_mem_rpmb_msg_out(hufs, result_request);
      
      // Get the STATUS by issue a security protocal in message
      osal_memset (rsp_buf, 0, RPMB_MSG_SIZE);

      rc = ufs_mem_rpmb_msg_in (hufs, rsp_buf);

      // Check the message type to make sure it is good
      if (rsp_buf[510] == RPMB_REQ_DATA_WRITE)
      {
         if(rsp_buf[511] != 0x00)
         {
            ufs_error_log(hba, 0x0265);
         }            
      }
      else if (rsp_buf[510] == RPMB_REQ_KEY_PROG)
      {
         if(rsp_buf[511] != 0x00)
         {
            ufs_error_log(hba, 0x0266);
         }      
      }
      else if (rsp_buf[510] == RPMB_REQ_SECURE_WP_WRITE)
      {
         if(rsp_buf[511] != 0x00)
         {
            ufs_error_log(hba, 0x0267);
         }      
      }
      else
      {
         ufs_error_log(hba, 0x0263);
      }   

      // Check the return status to make sure it is good
      rst = (rsp_buf[508] << 8) | rsp_buf[509];
      hufs->phc->last_rpmb_status = rst;
      if (rst != RPMB_RESULT_OK) {
         ufs_error_log(hba, 0x0264);
      }
      offset += current_bl;
   }

   return rc;
}

int32_t ufs_mem_rpmb_read_data (struct ufs_handle *hufs, 
                                uint32_t blocks, 
                                uint8_t *cmd_buf, 
                                uint8_t *data_buf)
{
   int32_t rc = EOK;
   uint8_t cdb[16];
   ufs_cmd_req_t req;
   ufs_host_t *hba = hufs->phc;
   uint32_t len;
   uint32_t current_bl;
   uint32_t total_bl;
   uint32_t cmd_offset = 0, data_offset = 0;

   total_bl = blocks;
   while (total_bl) {
       current_bl = total_bl > hba->desc_geo.bRPMB_ReadWriteSize ? 
                    hba->desc_geo.bRPMB_ReadWriteSize : total_bl;
       total_bl -= current_bl;
   
   // Send the command for read data block
       rc = ufs_mem_rpmb_msg_out (hufs, cmd_buf + cmd_offset * RPMB_MSG_SIZE);
   if (rc != EOK) {
      return rc;
   }

   // Write the actual data in a SECPROTO out packet
       len = current_bl * 512;          // Length is 512 bytes per block to transfer

   // Fill in the CDB with SCSI command structure
   osal_memset (cdb, 0, sizeof(cdb));
   cdb[0] = SCSI_CMD_SECPROT_IN;    // Command
   cdb[1] = 0xEC;                   // Security Protocal
   cdb[2] = 0x00;                   // 
   cdb[3] = 0x01;                   // Security Protocal Specific
   cdb[4] = 0x00;                   // Reserved
   cdb[5] = 0x00;                   // Reserved
   cdb[6] = (len >> 24) & 0xff;     // 
   cdb[7] = (len >> 16) & 0xff;     // 
   cdb[8] = (len >> 8) & 0xff;      // 
   cdb[9] = (len) & 0xff;           // Transfer Length
   cdb[10] = 0;                     // Reserve
   cdb[11] = 0;                     // Control

   osal_memset (&req, 0, sizeof(ufs_cmd_req_t));
   req.phc = hufs->phc;
   req.lun = UFS_WLUN_RPMB;
   req.flags = UFS_XFER_FLAGS_READ;
   req.expected_len = len;
       req.data_buf_phy = GET_PHYADDR(data_buf + data_offset * RPMB_MSG_SIZE);   

   rc = ufs_do_scsi_command (&req, cdb);
   if (rc == -ECHKCND) {
      rc = ufs_scsi_process_sense (hba, &req);
   }
   if (rc != EOK) {
      ufs_error_log(hba, 0x0268);
      return rc;
   }

   // Verify SCSI transfer status
   hufs->phc->last_scsi_status = req.status;
   if (req.status != SCSI_STATUS_GOOD) {
      ufs_error_log(hba, 0x0269);
      return -EIO;
   }   
       cmd_offset += 1; 
       data_offset += current_bl; 
   }

   return rc;
}

int32_t ufs_do_config_device( struct ufs_handle *hufs, 
                              struct ufs_config_descr *cfg )
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t buf[0x90];
   uint8_t *pt;
   int i;

   osal_memset (&req, 0, sizeof (ufs_attr_req_t));

   hba = hufs->phc;
   req.phc = hba;
   req.req_data_len = sizeof(buf);
   req.req_data_buf = buf;
   req.resp_data_len = 0;
   req.resp_data_buf = NULL;
   req.idn = UFS_DESC_IDN_CONFIGURATION;
   req.idx = 0;

   // Fill in the buffer with configuration data
   pt = buf;
   *pt++ = 0x90;        // bLength
   *pt++ = 0x01;        // bDescriptorType
   *pt++ = 0;           // Reserved in UFS2.0 and onward
   *pt++ = cfg->bBootEnable;
   *pt++ = cfg->bDescrAccessEn;
   *pt++ = cfg->bInitPowerMode;
   *pt++ = cfg->bHighPriorityLUN;
   *pt++ = cfg->bSecureRemovalType;
   *pt++ = cfg->bInitActiveICCLevel;
   *pt++ = (cfg->wPeriodicRTCUpdate >> 8) & 0xff;
   *pt++ = cfg->wPeriodicRTCUpdate & 0xff;
   *pt++ = 0;           // Reserved fields
   *pt++ = 0;
   *pt++ = 0;
   *pt++ = 0;
   *pt++ = 0;

   for (i = 0; i < 8; i++) {
      *pt++ = cfg->unit[i].bLUEnable;
      *pt++ = cfg->unit[i].bBootLunID;
      *pt++ = cfg->unit[i].bLUWriteProtect;
      *pt++ = cfg->unit[i].bMemoryType;
      *pt++ = (cfg->unit[i].dNumAllocUnits >> 24) & 0xff;
      *pt++ = (cfg->unit[i].dNumAllocUnits >> 16) & 0xff;
      *pt++ = (cfg->unit[i].dNumAllocUnits >> 8) & 0xff;
      *pt++ = (cfg->unit[i].dNumAllocUnits) & 0xff;
      *pt++ = cfg->unit[i].bDataReliability;
      *pt++ = cfg->unit[i].bLogicalBlockSize;
      *pt++ = cfg->unit[i].bProvisioningType;
      *pt++ = (cfg->unit[i].wContextCapabilities >> 8) & 0xff;
      *pt++ = cfg->unit[i].wContextCapabilities;
      *pt++ = 0;        // Reserved fields
      *pt++ = 0;
      *pt++ = 0;
   }

   rc = ufs_write_dev_descriptor (&req);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0270);
      return rc;
   }

   if (cfg->bConfigDescrLock) {
      rc = ufs_set_bConfigDescrLock (hufs, cfg->bConfigDescrLock);
      if (rc != EOK) {
         ufs_error_log(hba, 0x0271);
         return rc;
      }
   }

   /* Set host intialized flag to FALSE, so that any reinit
      happens with the new config information */
   hba->is_initialized = FALSE; 

   return rc;
}

void ufs_fill_in_device_info (struct ufs_handle *hufs, 
                             struct ufs_info_type *info)
{
   ufs_host_t *hba = hufs->phc;
   uint8_t lun;
   uint8_t bBootLunEn;
   int i;

   lun = hufs->lun;
   if (lun > 7 && lun == UFS_WLUN_BOOT && hba->desc_dev.bBootEnable) {
      bBootLunEn = hba->attr.bBootLunEn;
      if (bBootLunEn == 1 || bBootLunEn == 2) {
         for (i = 0; i < 8; i++) {
            if (hba->desc_cfg.unit_cfg[i].bBootLunID == bBootLunEn) {
               lun = i;
            }
         }
      }
   }

   info->fInitialized = hba->is_initialized;
   info->bLUN = lun;
   info->bDeviceEmbedded         = (hba->desc_dev.bDeviceSubClass >> 1) & 0x1; 
   info->bDeviceBootable         = hba->desc_dev.bDeviceSubClass & 0x1;
   info->bNumberLu               = hba->desc_dev.bNumberLU;
   info->wManufacturerID         = hba->desc_dev.wManufacturerID;
   info->bBootEnable             = hba->desc_dev.bBootEnable;
   info->qTotalRawDeviceCapacity = hba->desc_geo.qTotalRawDeviceCapacity;
   info->bMinAddrBlockSize       = hba->desc_geo.bMinAddrBlockSize;
   info->dSegmentSize            = hba->desc_geo.dSegmentSize; 
   info->bAllocationUnitSize     = hba->desc_geo.bAllocationUnitSize;
   info->bRPMB_ReadWriteSize     = hba->desc_geo.bRPMB_ReadWriteSize;
   info->wSupportedMemoryTypes   = hba->desc_geo.wSupportedMemoryTypes;
   info->dEnhanced1MaxNAllocU    = hba->desc_geo.dEnhanced1MaxNAllocU;
   info->wEnhanced1CapAdjFac     = hba->desc_geo.wEnhanced1CapAdjFac;
   info->dEnhanced2MaxNAllocU    = hba->desc_geo.dEnhanced2MaxNAllocU;
   info->wEnhanced2CapAdjFac     = hba->desc_geo.wEnhanced2CapAdjFac;
   info->dEnhanced3MaxNAllocU    = hba->desc_geo.dEnhanced3MaxNAllocU;
   info->wEnhanced3CapAdjFac     = hba->desc_geo.wEnhanced3CapAdjFac;
   info->dEnhanced4MaxNAllocU    = hba->desc_geo.dEnhanced4MaxNAllocU;
   info->wEnhanced4CapAdjFac     = hba->desc_geo.wEnhanced4CapAdjFac;
   info->iManufacturerName       = hba->desc_dev.iManufacturerName;
   info->iProductName            = hba->desc_dev.iProductName;
   info->iSerialNumber           = hba->desc_dev.iSerialNumber;
   info->iOemID                  = hba->desc_dev.iOemID;
   info->bConfigDescrLock        = hba->attr.bConfigDescrLock;
   info->qHostBaseAddr           = (uint64_t)hba->ufs_hci_addr;

   // Populate the LUN enabled bit field
   info->dLunEnabled = 0;
   for (i = 0; i < 8; i++) {
      info->dLunEnabled |= (hba->desc_cfg.unit_cfg[i].bLUEnable ? 1 : 0) << i;
   }

   // Copy the inquiry field
   for (i = 0; i < sizeof(hba->inquiry_vid); i++) {
      info->inquiry_str[i] = hba->inquiry_vid[i];
   }
   info->inquiry_str[i] = 0;

   if (lun <= 7) {
      // LUN specific area
      info->dNumAllocUnits     = hba->desc_cfg.unit_cfg[lun].dNumAllocUnits;
      info->bLogicalBlockSize  = hba->desc_cfg.unit_cfg[lun].bLogicalBlockSize;
      info->bProvisioningType  = hba->desc_cfg.unit_cfg[lun].bProvisioningType;
      info->bLUWriteProtect    = hba->desc_cfg.unit_cfg[lun].bLUWriteProtect;
      info->bBootLunID         = hba->desc_cfg.unit_cfg[lun].bBootLunID;
      info->bMemoryType        = hba->desc_cfg.unit_cfg[lun].bMemoryType;
      info->qLogicalBlockCount = hba->desc_unit[lun].qLogicalBlockCount;
   }

   if (lun == UFS_WLUN_RPMB) {
      info->bLogicalBlockSize  = hba->desc_rpmb.bLogicalBlockSize;
      info->qLogicalBlockCount = hba->desc_rpmb.qLogicalBlockCount;
   }
}


// Vendor specific command, use to control pre-programming mode
int32_t ufs_vend_pprog_ctrl (struct ufs_handle *hufs, uint32_t code)
{
   ufs_host_t *hba;
   ufs_attr_req_t req;
   int32_t rc = EOK;
   uint8_t osf[8];     
   uint8_t buffer[512];

   hba = hufs->phc;
   req.phc = hufs->phc;
   req.req_data_len = 0x200;
   req.req_data_buf = (uint8_t *)buffer;
   req.resp_data_len = 0x200;
   req.resp_data_buf = (uint8_t *)buffer;
   req.idn = 0;
   req.idx = 0;

   // Program the buffer
   osal_memset (buffer, 0x00, sizeof (buffer));
   buffer[0] = 0x0b;
   buffer[4] = (code >> 24) & 0xff;
   buffer[5] = (code >> 16) & 0xff;
   buffer[6] = (code >> 8) & 0xff;
   buffer[7] = (code) & 0xff;
   buffer[8] = 0x01;
   buffer[9] = 0x01;

   rc = ufs_generic_write_query (&req, osf, 0xc2);
   if (rc != EOK) {
      ufs_error_log(hba, 0x0287);
      return rc;
   }

   return rc;
}

