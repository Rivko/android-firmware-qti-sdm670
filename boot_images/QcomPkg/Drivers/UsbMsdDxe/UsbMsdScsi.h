#ifndef __USB_MSD_SCSI_H__
#define __USB_MSD_SCSI_H__

/*=============================================================================
  @file UsbMsdScsi.h

  SCSI protocol implementation
 
  Copyright (c) 2011, 2014 QUALCOMM Technologies Inc. All rights reserved.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/14/14   ck      Add history. Introduce state machine and remove global vars
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "com_dtypes.h"   /* common data type, such as uint8 */
#include "UsbMsd.h"       /* USBMSD_DEV struct */
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QHSUSB_CBW_SIGNATURE (0x43425355) 
#define QHSUSB_CSW_SIGNATURE (0x53425355) 
#define QHSUSB_CBW_LEN       31
#define QHSUSB_CSW_LEN       13
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* Command block wrapper definitions */
typedef struct qhsusb_cbw_type {
  uint32  dCBWSignature;          /* QHSUSB_CBW_SIGNATURE */
  uint32  dCBWTag;                /* Command Block Tag */
  uint32  dCBWDataTransferLength; /* number of data bytes */
  uint8   bmCBWFlags;             /* for direction  (bit #7) */
  uint8   bCBWLUN;                /* LUN number */
  uint8   bCBWCBLength;           /* Command block length (0x1-0x10)*/
  uint8   CBWCB[16];              /* Command block */
}qhsusb_cbw_t;

/* Command Status Wrapper definitions */
typedef struct qhsusb_csw_type {
  uint32  dCSWSignature;        /* QHSUSB_CSW_SIGNATURE' */
  uint32  dCSWTag;              /* Command Block Tag (same as in cbw) */
  uint32  dCSWDataResidue;      /* the difference between the amount of data expected and the actual amount of data */
  uint8   bCSWStatus;           /* success/failure of the command */
}qhsusb_csw_t;

/* Length of data related to SCSI commands */
#define MAX_VENDOR_STR_SIZE                      8
#define MAX_PRODUCT_STR_SIZE                     16
#define MAX_PROD_REV_STR_SIZE                    4
#define MAX_INQUERY_RES_SIZE                     36
#define MAX_READ_FORMAT_CAPACITY_SIZE            12
#define MAX_READ_CAPACITY_SIZE                   8
#define MAX_MODE_SENSE_SIZE                      4
#define MAX_REQUEST_SENSE_SIZE                   18

/* SCSI operation code */
#define SCSI_TEST_UNIT_READY                     0x00
#define SCSI_REQUEST_SENSE                       0x03
#define SCSI_INQUIRY                             0x12
#define SCSI_MODE_SELECT6                        0x15
#define SCSI_MODE_SENSE6                         0x1A
#define SCSI_READ_FORMAT_CAPACITY                0x23
#define SCSI_READ_CAPACITY10                     0x25
#define SCSI_READ10                              0x28
#define SCSI_WRITE10                             0x2A
#define SCSI_SYNCHRONIZE_CACHE10                 0x35
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL        0x1E
#define SCSI_REPORT_LUNS                         0xA0

/* CSW status field */
#define CSW_GOOD_STATUS 0
#define CSW_ERROR_STATUS 1

#define SWAP32(x) (((x&0x000000FF)<<(3*8)) | \
  ((x&0x0000FF00)<<(1*8)) | \
  ((x&0x00FF0000)>>(1*8)) | \
  ((x&0xFF000000)>>(3*8)))

/*----------------------------------------------------------------------------
 * Function Declarations 
 * -------------------------------------------------------------------------*/
/**
* @brief Handle the new CBW received from the host
*
* @param[in]  UsbMsd       The instance of USBMSD_DEV
*
* @retval DEV_STATE      The next USB MSD state machine state
*/
DEV_STATE handle_new_cbw(IN USBMSD_DEV *UsbMsd);


/**
* @function  sending_csw
*
* @brief sending csw to the host
*
* @param[in] UsbMsd        The USBMSD_DEV instance
* @param[in] csw_status    The status of handling the CBW
* @param[in] data_residue  The remaining data to be transferred.
*
* @return DEV_STATE        Next state machine state
*
*/
DEV_STATE sending_csw   (IN USBMSD_DEV *UsbMsd, IN uint8 csw_status, IN uint32 data_residue);


/**
* @brief Handle the host sent data for a SCSI command
*
* @param[in]  UsbMsd       The instance of USBMSD_DEV
*
* @retval DEV_STATE      The next USB MSD state machine state
*/
DEV_STATE handle_rx_data(IN USBMSD_DEV *UsbMsd);
#endif /* __USB_MSD_SCSI_H__ */
