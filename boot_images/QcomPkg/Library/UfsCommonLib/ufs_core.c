/**********************************************************************
 *
 * UFS core driver layer -
 * Main UFS driver state machine, taking in SCSI command from the
 * api layer and attach the necessary header for the UFS HCI
 * This layer work on the UPIU data structure
 *
 *
 * Copyright (c) 2012-2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-07-31   jt      Log error codes to serial output 
2017-06-09   jt      Poll until completion if fDeviceInit is asynchronous 
2017-05-22   jt      Add response type to ufs_poll_pending_xfer
2014-09-17   rh      Merge with UEFI driver changes
2013-10-22   rh      Capture the sense key information
2013-09-10   rh      Adding generic UPIU write passthrough
2013-09-06   rh      Increase the size of the debug log buffer
2013-08-20   rh      64k limitation should be block, not byte
2013-08-20   rh      Write descriptor need to set data length
2012-12-18   rh      Initial creation
=======================================================================*/

#include "ufs_osal.h"

#include "ufs_api.h"
#include "ufs_core.h"
#include "ufs_utp_core.h"
#include "ufs_errno.h"
#include "ufs_bsp.h"

int32_t ufs_core_init (ufs_host_t *hba )
{
   // Reset non-blocking flags
   hba->non_blocking_en = 0;
   hba->xfer_pend = 0;

   // Call init for BSP and UTP layer
   return ufs_utp_init (hba);
}

// Issue a SCSI command 
int32_t ufs_do_scsi_command (ufs_cmd_req_t *cmdreq, uint8_t *cdb)
{
   ufs_host_t *hba = cmdreq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Check the destination buffer for DWARD alignment
   if ((cmdreq->data_buf_phy & 3) != 0 ) {
      ufs_error_log(hba, 0x0130);
      return -EINVAL;
   }

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 32);
   if (rc < 0) {
      ufs_error_log(hba, 0x0131);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_COMMAND;
   pupiu->flags = cmdreq->flags;
   pupiu->lun = cmdreq->lun;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = UPIU_CMDTYPE_SCSI;
   pupiu->dev_info = 0;       
   pupiu->data_segment_len = 0;
   pupiu->data_expected_len = cmdreq->expected_len;
   // Fill in the address of the output buffer
   utrd_info.data_buf_phy = cmdreq->data_buf_phy;
   utrd_info.data_xfer_len = cmdreq->expected_len;
   utrd_info.cmd_type = UTRD_CMDTYPE_SCSI;
   utrd_info.interrupt = 1;
   // Use a buffer to capture the sense key
   prupiu->aux_data_len = 32;
   prupiu->aux_data_buf = cmdreq->req_sense;     

   if (cmdreq->flags & UFS_XFER_FLAGS_READ) {
      utrd_info.data_dir = UTRD_DIR_READ;
   } 
   else if (cmdreq->flags & UFS_XFER_FLAGS_WRITE) {
      utrd_info.data_dir = UTRD_DIR_WRITE;
   }
   else {
      utrd_info.data_dir = UTRD_DIR_NODATA;
   }

   osal_memcpy (pupiu->param, cdb, 16);

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0132);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc == -EBUSY) {
      // non-blocking busy, return with busy status
      hba->xfer_pend = 1;
      return rc;
   }
   else if (rc < 0) {
      ufs_error_log(hba, 0x0133);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Pass the SCSI status back to the caller
   cmdreq->status = prupiu->status;

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_RESPONSE) {
      ufs_error_log(hba, 0x0134);
      return -EPROTO;
   }

   // Check to make sure the Response Code is SUCCESS
   if (prupiu->response != UPIU_RESP_SUCCESS) {
      // Check to see if there is a CHECK_CONDITION flag and valid payload
      if (prupiu->status == UPIU_SCSI_S_CHECK_CONDITION &&
          prupiu->data_segment_len != 0) {
         // Notify the memory layer to decode the sense data
         return -ECHKCND;
      }
      ufs_error_log(hba, 0x0135);
      return -EPROTO;
   }

   return EOK;
}

// Poll the completion of pending transfer
int32_t ufs_poll_pending_xfer (ufs_host_t *hba, uint8_t resp_type)
{
   utrd_item_t utrd_info;
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Check if there is a pending transfer, if not return ENOENT
   if (!hba->xfer_pend || !hba->non_blocking_en) {
      return -ENOENT;
   }

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 32);
   if (rc < 0) {
      ufs_error_log(hba, 0x0181);
      return rc;
   }

   prupiu = &utrd_info.resp_upiu;

   // If we are calling this function to poll for fDeviceInit completion, 
   // poll till completion 
   if (hba->fDeviceInit_async) {
      hba->non_blocking_en = 0; 
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc == -EBUSY) {
      return rc; // non-blocking busy, return with busy status
   }
   else if (rc < 0) {
      ufs_error_log(hba, 0x0183);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   hba->xfer_pend = 0; 
   hba->non_blocking_en = 0;

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response matches expected response type
   if (prupiu->type != resp_type) {
      ufs_error_log(hba, 0x0184);
      return -EPROTO;
   }

   // Check to make sure the Response Code is SUCCESS
   if (prupiu->response != UPIU_RESP_SUCCESS) {
      // Check to see if there is a CHECK_CONDITION flag and valid payload
      if (prupiu->status == UPIU_SCSI_S_CHECK_CONDITION &&
          prupiu->data_segment_len != 0) {
         // Notify the memory layer to decode the sense data
         return -ECHKCND;
      }
      ufs_error_log(hba, 0x0185);
      return -EPROTO;
   }

   return EOK;
}


// Read UFS device descriptor, output stored in dest_buf
int32_t ufs_read_dev_descriptor (ufs_attr_req_t *preq)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // The length field should not exceed 64k bytes
   if (preq->resp_data_len > 65535) {
      ufs_error_log(hba, 0x0120);
      return -EINVAL;
   }

   // Start by using a standard URTD structure
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, preq->resp_data_len);
   if (rc < 0) {
      ufs_error_log(hba, 0x0121);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;       
   pupiu->query_fnc = UPIU_FNC_STD_READ;
   pupiu->data_segment_len = 0;  
   // Fill in the address of the output buffer
   utrd_info.data_buf_phy = 0;
   utrd_info.data_xfer_len = 0;
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   pupiu->param[0] = UPIU_QUERY_OP_READ_DESCRIPTOR;   // OPCODE
   pupiu->param[1] = preq->idn;                    // ATTRIBUTE IDN
   pupiu->param[2] = preq->idx;                    // INDEX
   pupiu->param[3] = preq->sel;                    // SELECTIOR
   pupiu->param[6] = preq->resp_data_len >> 8;        
   pupiu->param[7] = preq->resp_data_len & 0xff;   // LENGTH

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0122);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0123);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   prupiu->aux_data_len = preq->resp_data_len;     // Any payload?
   prupiu->aux_data_buf = preq->resp_data_buf;     
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x0124);
      return -EPROTO;
   }

   // Check to make sure the OPcode is correct and Query Response Code is SUCCESS
   if (prupiu->param[0] != UPIU_QUERY_OP_READ_DESCRIPTOR ||
       prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x0125);
      return -EPROTO;
   }

   return EOK;
}

// Write UFS device descriptor
int32_t ufs_write_dev_descriptor (ufs_attr_req_t *preq)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // The length field should not exceed 64k bytes
   if (preq->req_data_len > 65535) {
      ufs_error_log(hba, 0x0128);
      return -EINVAL;
   }

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, preq->req_data_len, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0129);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;       
   pupiu->query_fnc = UPIU_FNC_STD_WRITE;
   pupiu->data_segment_len = preq->req_data_len;  
   // Fill in the address of the output buffer
   utrd_info.data_buf_phy = 0;
   utrd_info.data_xfer_len = 0;
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   pupiu->param[0] = UPIU_QUERY_OP_WRITE_DESCRIPTOR;  // OPCODE
   pupiu->param[1] = preq->idn;                    // ATTRIBUTE IDN
   pupiu->param[2] = preq->idx;                    // INDEX
   pupiu->param[3] = preq->sel;                    // SELECTIOR
   pupiu->param[6] = preq->req_data_len >> 8;         
   pupiu->param[7] = preq->req_data_len & 0xff;    // LENGTH

   pupiu->aux_data_len = preq->req_data_len;     // Any payload?
   pupiu->aux_data_buf = preq->req_data_buf;     

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x012a);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x012b);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x012c);
      return -EPROTO;
   }

   // Check to make sure the OPcode is correct and Query Response Code is SUCCESS
   if (prupiu->param[0] != UPIU_QUERY_OP_WRITE_DESCRIPTOR ||
       prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x012d);
      return -EPROTO;
   }

   return EOK;
}

// UFS device flag operation
int32_t ufs_dev_flag_op (ufs_attr_req_t *preq, uint8_t op)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0110);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;          // Not used in Attribute
   pupiu->data_segment_len = 0;  // Segment len not used

   switch (op) {
      case UPIU_FLAG_OP_READ:
         pupiu->param[0] = UPIU_QUERY_OP_READ_FLAG;      
         pupiu->query_fnc = UPIU_FNC_STD_READ;
         break;
      case UPIU_FLAG_OP_SET:
         pupiu->param[0] = UPIU_QUERY_OP_SET_FLAG;    
         pupiu->query_fnc = UPIU_FNC_STD_WRITE;
         break;
      case UPIU_FLAG_OP_CLEAR:
         pupiu->param[0] = UPIU_QUERY_OP_CLEAR_FLAG;     
         pupiu->query_fnc = UPIU_FNC_STD_WRITE;
         break;
      case UPIU_FLAG_OP_TOGGLE:
         pupiu->param[0] = UPIU_QUERY_OP_TOGGLE_FLAG;    
         pupiu->query_fnc = UPIU_FNC_STD_WRITE;
         break;
      default:
         // No valid operation defined
         ufs_error_log(hba, 0x0115);
         return -EINVAL;
   }
   pupiu->param[1] = preq->idn;                    // ATTRIBUTE IDN
   pupiu->param[2] = preq->idx;                    // INDEX
   pupiu->param[3] = preq->sel;                    // SELECTIOR
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0111);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc == -EBUSY) {
      // non-blocking busy, return with busy status
      hba->xfer_pend = 1;
      return rc;
   }
   if (rc < 0) {
      ufs_error_log(hba, 0x0112);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x0113);
      return -EPROTO;
   }

   // Check to make sure the OPcode is correct and Query Response Code is SUCCESS
   if (prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x0114);
      return -EPROTO;
   }

   // Extract the response field and return it
   preq->val = prupiu->param[11];
   return EOK;
}


// Read UFS attributes
int32_t ufs_read_attributes (ufs_attr_req_t *preq)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0100);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;          // Not used in Attribute
   pupiu->query_fnc = UPIU_FNC_STD_READ;
   pupiu->data_segment_len = 0;  // Segment len not used

   pupiu->param[0] = UPIU_QUERY_OP_READ_ATTRIBUTE; // OPCODE
   pupiu->param[1] = preq->idn;                    // ATTRIBUTE IDN
   pupiu->param[2] = preq->idx;                    // INDEX
   pupiu->param[3] = preq->sel;                    // SELECTIOR
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0101);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0102);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x0103);
      return -EPROTO;
   }

   // Check to make sure the OPcode is READ_ATTRIBUTE and
   // Query Response Code is SUCCESS
   if (prupiu->param[0] != UPIU_QUERY_OP_READ_ATTRIBUTE ||
       prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x0104);
      return -EPROTO;
   }

   // Extract the response field and return it
   preq->val = (prupiu->param[8] << 24) | (prupiu->param[9] << 16) |
               (prupiu->param[10] << 8) | (prupiu->param[11]);
   //preq->val = *(uint32_t *)(&prupiu->param[8]);
   return EOK;
}

// Write UFS attributes
int32_t ufs_write_attributes (ufs_attr_req_t *preq)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0110);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;          // Not used in Attribute
   pupiu->query_fnc = UPIU_FNC_STD_WRITE;
   pupiu->data_segment_len = 0; 

   pupiu->param[0] = UPIU_QUERY_OP_WRITE_ATTRIBUTE;   // OPCODE
   pupiu->param[1] = preq->idn;                    // ATTRIBUTE IDN
   pupiu->param[2] = preq->idx;                    // INDEX
   pupiu->param[3] = preq->sel;                    // SELECTIOR
   //*(uint32_t *)(&prupiu->param[8]) = preq->val;
   pupiu->param[8] = preq->val >> 24;
   pupiu->param[9] = preq->val >> 16;
   pupiu->param[10] = preq->val >> 8;
   pupiu->param[11] = preq->val & 0xff;
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0108);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0109);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x010a);
      return -EPROTO;
   }

   // Check to make sure the OPcode is WRITE_ATTRIBUTE and
   // Query Response Code is SUCCESS
   if (prupiu->param[0] != UPIU_QUERY_OP_WRITE_ATTRIBUTE ||
       prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x010b);
      return -EPROTO;
   }

   // Write completed
   return EOK;
}

// Send NOP UPIU to check if the device is ready
int32_t ufs_send_nop_ping (ufs_attr_req_t *preq)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, 0, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0141);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_NOP_OUT;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;       
   pupiu->query_fnc = 0;
   pupiu->data_segment_len = 0;  
   // Fill in the address of the output buffer
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0142);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0143);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_NOP_IN) {
      ufs_error_log(hba, 0x0144);
      return -EPROTO;
   }

   return EOK;
}

// Generic query request with write data payload
int32_t ufs_generic_write_query (ufs_attr_req_t *preq, uint8_t *osf, 
                                 uint32_t q_func)
{
   ufs_host_t *hba = preq->phc;
   utrd_item_t utrd_info;
   upiu_item_t *pupiu;              // Request UPIU
   upiu_item_t *prupiu;             // Response UPIU
   int32_t rc;

   // The length field should not exceed 64k bytes
   if (preq->req_data_len > 65535) {
      ufs_error_log(hba, 0x0150);
      return -EINVAL;
   }

   // Build a standard empty UTRD first ... 
   rc = ufs_utp_build_std_utrd (hba, &utrd_info, preq->req_data_len, 0);
   if (rc < 0) {
      ufs_error_log(hba, 0x0151);
      return rc;
   }

   // Fill in a UPIU 
   pupiu = &utrd_info.req_upiu;
   prupiu = &utrd_info.resp_upiu;
   pupiu->type = UPIU_TYPE_QUERY_REQ;
   pupiu->flags = 0;
   pupiu->lun = 0;
   pupiu->task_tag = (uint8_t)hba->ufs_task_cnt++;
   pupiu->cmd_set_type = 0;
   pupiu->dev_info = 0;       
   pupiu->query_fnc = q_func;
   pupiu->data_segment_len = preq->req_data_len;  
   // Fill in the address of the output buffer
   utrd_info.data_buf_phy = 0;
   utrd_info.data_xfer_len = 0;
   utrd_info.cmd_type = UTRD_CMDTYPE_MANAGEMENT;
   utrd_info.data_dir = UTRD_DIR_NODATA;
   utrd_info.interrupt = 1;

   pupiu->param[0] = osf[0];
   pupiu->param[1] = osf[1];                    
   pupiu->param[2] = osf[2];                   
   pupiu->param[3] = osf[3];                  
   pupiu->param[4] = osf[4];                  
   pupiu->param[5] = osf[5];                  
   pupiu->param[6] = osf[6];         
   pupiu->param[7] = osf[7]; 

   pupiu->aux_data_len = preq->req_data_len;     // Any payload?
   pupiu->aux_data_buf = preq->req_data_buf;     

   rc = ufs_utp_submit_utrd (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0152);
      return rc;
   }

   // Wait for transfer to complete
   rc = ufs_utp_poll_utrd_complete (hba, &utrd_info);
   if (rc < 0) {
      ufs_error_log(hba, 0x0153);
      ufs_utp_abort_utrd (hba, &utrd_info);
      return rc;
   }

   // Decode the response to get the result
   prupiu->aux_data_len = preq->resp_data_len;     // Any payload?
   prupiu->aux_data_buf = preq->resp_data_buf;     
   ufs_utp_decode_upiu (utrd_info.utp_cmd_desc_ptr + 
                        utrd_info.resp_upiu_offset / 4, prupiu);

   // Check to make sure the response is a query response type
   if (prupiu->type != UPIU_TYPE_QUERY_RESP) {
      ufs_error_log(hba, 0x0154);
      return -EPROTO;
   }

   // Check to make sure the Query Response Code is SUCCESS
   if (prupiu->response != UPIU_RESP_SUCCESS) {
      ufs_error_log(hba, 0x0155);
      return -EPROTO;
   }

   return EOK;
}

// Record the error code in the debugging buffer
void ufs_error_log(ufs_host_t *hba, uint32_t eid)
{
   char log_buffer[32] = {0}; 

   hba->last_fail_id[hba->next_failure] = eid;
   hba->failure_cnt++;
   hba->next_failure++;
   if (hba->next_failure >= (sizeof(hba->last_fail_id) / sizeof(uint32_t))) {
      hba->next_failure = 0;
   }

   snprintf(log_buffer, 32, "ufs_error_log: %x", eid);  
   ufs_bsp_log_error(log_buffer);
}


