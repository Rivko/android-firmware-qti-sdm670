#ifndef __UFS_UTP_CORE_H
#define __UFS_UTP_CORE_H
/**********************************************************************
 *
 * UFS UTP interface layer, contain data structure and function definitions
 *
 * Copyright (c) 2012-2013, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-07-31   jt      Add debug reg dump   
2013-09-30   rh      Adding the UIC debug dump
2012-12-18   rh      Initial creation
=======================================================================*/

#include "ufs_osal.h"

#define UPIU_TYPE_NOP_OUT              0x00
#define UPIU_TYPE_NOP_IN               0x20
#define UPIU_TYPE_COMMAND              0x01
#define UPIU_TYPE_RESPONSE             0x21
#define UPIU_TYPE_QUERY_REQ            0x16
#define UPIU_TYPE_QUERY_RESP           0x36
#define UPIU_TYPE_REJECT               0x3f
#define UPIU_TYPE_TASK_MAN             0x04
#define UPIU_TYPE_TASK_MAN_RESP        0x24

// Use this structure to either construct or decode UPIU
typedef struct utp_upiu_item {
   uint8_t        type;
   uint8_t        flags;
   uint8_t        lun;
   uint8_t        task_tag;
   uint8_t        cmd_set_type;
   uint8_t        query_fnc;           
   uint8_t        response;
   uint8_t        status;
   uint8_t        dev_info;
   uint16_t       data_segment_len;
   uint32_t       data_expected_len;   // Requested length
   uint32_t       data_residual_len;   // Response to leftover length
   uint8_t        param[16];           // Payload, operation specefic field
                                       // This filed is CDB for Command
                                       // Parameter for management and query
   uint32_t       aux_data_len;        // Attached AUX data length
   uint8_t        *aux_data_buf;       // Attached AUX data buffer pointer
} upiu_item_t;

// cmd_type field
#define UTRD_CMDTYPE_SCSI              0x0
#define UTRD_CMDTYPE_NATIVE_UFS        0x1
#define UTRD_CMDTYPE_MANAGEMENT        0x2

// data_dir field
#define UTRD_DIR_NODATA                0x0
#define UTRD_DIR_WRITE                 0x1
#define UTRD_DIR_READ                  0x2

// interrupt field
#define UTRD_REGULAR_CMD               0x0
#define UTRD_INTERRUPT_CMD             0x1

// Overall Command status field
#define OCS_SUCCESS                     0x0
#define OCS_INVALID_COM_TABLE_ATTR      0x1
#define OCS_INVALID_PRDT_ATTR           0x2
#define OCS_MISMATCH_DATA_BUFFER_SIZE   0x3
#define OCS_MISMATCH_RESPONSE_UPIU_SIZE 0x4
#define OCS_COMMUNICATION_FAILURE       0x5
#define OCS_ABORTED                     0x6
#define OCS_INVALID_OCS_VALUE           0xf

// Standard data structure defines
#define UPIU_STD_HDR_LEN               0x20

// Use this data structure for contruction of UTRD
typedef struct utp_utrd_item {
   uint8_t        cmd_type;
   uint8_t        data_dir;
   uint8_t        interrupt;
   uint8_t       *utrd_desc_ptr;
   uint32_t      *utp_xfr_desc_ptr;    // Buffer for xfer request descriptor
   uint64_t       utp_xfr_desc_phy;
   uint32_t      *utp_cmd_desc_ptr;    // Buffer for command descriptor
   uint64_t       utp_cmd_desc_phy;
   uint32_t       resp_upiu_offset;    // Offset for the resp UPIU in bytes
   uint32_t       prdt_offset;         // Offset for PRDT in bytes
   uint32_t       prdt_len;
   upiu_item_t    req_upiu;            // Request UPIU
   uint32_t       req_upiu_len;        // Length of the request
   uint64_t       data_buf_phy;        // Physical address of data buffer
   uint32_t       data_xfer_len;       // Length of the data transfer
   uint32_t       utp_doorbell_bit;    // Indicate which cmd slot in use
   // Response information
   upiu_item_t    resp_upiu;           // Response UPIU
   uint32_t       resp_upiu_len;
   uint8_t        cmd_status;          // Overall command status
} utrd_item_t;

// Use this data structure for contruction of UTMRD
typedef struct utp_utmrd_item {
   uint8_t        interrupt;
   uint32_t      *utp_xfr_desc_ptr;
   uint64_t       utp_xfr_desc_phy;
   uint8_t       *utmrl_desc_ptr;
   uint64_t       utmrl_desc_phy;
   uint8_t       *req_upiu_desc_ptr;
   uint32_t       response_upiu_offset;
   uint32_t       response_upiu_len;
   upiu_item_t    req_upiu;            // Request UPIU
   upiu_item_t    resp_upiu;           // Response UPIU
   uint32_t       utp_doorbell_bit;    // Indicate which cmd slot in use
} utmrd_item_t;


// Fill in an UTRD with default values
int32_t ufs_utp_init (ufs_host_t *hdev);

// Submit a UTRD by ringing the doorbell register
int32_t ufs_utp_submit_utrd (ufs_host_t *hdev, utrd_item_t *pinfo);

// Build the UPIU data structure in memory
void ufs_utp_build_upiu (uint32_t *dest_in, upiu_item_t *pupiu);

// Send a generic UIC command without any extended attribute
int32_t ufs_utp_send_uic (ufs_host_t *hdev, uint32_t uic);

// Send a UIC command with extended attribute and wait for completition flag 
int32_t ufs_utp_uic_getset (ufs_host_t *hdev, uint32_t uic, uint32_t MIBattrIndex, 
                            uint32_t MIBvalSet, uint32_t *MIBvalGet);

// Wait for a UTRD to complete using polling method
int32_t ufs_utp_poll_utrd_complete (ufs_host_t *hdev, utrd_item_t *pinfo);

// Abort a transfer by clearing a UTRD doorbell register
void ufs_utp_abort_utrd (ufs_host_t *hdev, utrd_item_t *pinfo);

// Decode the response UPIU
void ufs_utp_decode_upiu (uint32_t *dest_in, upiu_item_t *pupiu);

// Build a standard UTRD for data transfer
int32_t ufs_utp_build_std_utrd (ufs_host_t *hdev, utrd_item_t *pinfo, 
                            uint32_t req_dlen, uint32_t resp_dlen);

// Switch the interface to high speed operation
int32_t ufs_utp_gear_sw (ufs_host_t *hba);

// Get UNIPRO/MPHY capabilities
int32_t ufs_utp_get_host_caps (ufs_host_t *hba);

// Debug dump of the UIC attributes 
void ufs_uic_debug_dump (ufs_host_t *hba);

// Debug dump of select UFS registers 
void ufs_dump_regs (ufs_host_t *hba);

#endif
