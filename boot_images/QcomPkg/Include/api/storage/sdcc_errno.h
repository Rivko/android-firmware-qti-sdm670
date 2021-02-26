#ifndef __SDCC_ERRNO_H
#define __SDCC_ERRNO_H
/******************************************************************************
 * File: sdcc_errno.h
 *
 * Services: 
 *    Public API header for SDCC error code
 *
 * Description:
 *    This file contains the SDCC error code.
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-11-06   bn      Added eMMC FW Update errors.
2014-06-18   bn      Initial version. Branched from 8994 SBL

=============================================================================*/

/* enumeration values for errors that are used in SDCC */
#define SDCC_NO_ERROR                         0
#define SDCC_ERR_UNKNOWN                      1
#define SDCC_ERR_CMD_TIMEOUT                  2
#define SDCC_ERR_DATA_TIMEOUT                 3
#define SDCC_ERR_TIMEOUT                      3
#define SDCC_ERR_CMD_CRC_FAIL                 4
#define SDCC_ERR_DATA_CRC_FAIL                5
#define SDCC_ERR_CMD_SENT                     6
#define SDCC_ERR_PROG_DONE                    7
#define SDCC_ERR_CARD_READY                   8 
#define SDCC_ERR_INVALID_TX_STATE             9
#define SDCC_ERR_SET_BLKSZ                    10
#define SDCC_ERR_SDIO_R5_RESP                 11
#define SDCC_ERR_DMA                          12
#define SDCC_ERR_READ_FIFO                    13
#define SDCC_ERR_WRITE_FIFO                   14
#define SDCC_ERR_ERASE                        15
#define SDCC_ERR_SDIO                         16
#define SDCC_ERR_SDIO_READ                    17
#define SDCC_ERR_SDIO_WRITE                   18
#define SDCC_ERR_SWITCH                       19
#define SDCC_ERR_INVALID_PARAM                20
#define SDCC_ERR_CARD_UNDETECTED              21
#define SDCC_ERR_FEATURE_UNSUPPORTED          22
#define SDCC_ERR_SECURE_COMMAND_IN_PROGRESS   23 
#define SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED     24
#define SDCC_ERR_ABORT_READ_SEC_CMD           25
#define SDCC_ERR_CARD_INIT                    26
#define SDCC_ERR_CARD_REMOVED                 27
#define SDCC_ERR_PWR_ON_WRITE_PROT            28
#define SDCC_ERR_WP_VIOLATION                 29
#define SDCC_ERR_SPS_MODE                     30 
#define SDCC_ERR_RPMB_RESPONSE                31
#define SDCC_ERR_RPMB_RESULT                  32
#define SDCC_ERR_RPMB_NONCE                   33
#define SDCC_ERR_OP_NOT_SUPPORTED             34
#define SDCC_ERR_FW_GENERAL_ERROR             35
#define SDCC_ERR_FW_INSTALL                   36
#define SDCC_ERR_FW_DOWNLOAD                  37
#define SDCC_ERR_CARD_IN_USE                  38

typedef int SDCC_STATUS;
#endif /* __SDCC_ERRNO_H */

