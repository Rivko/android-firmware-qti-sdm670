#ifndef QHSUSB_FD_MS_H
#define QHSUSB_FD_MS_H
/*=======================================================================*//**
  @file qhsusb_fd_ms.h

  Functional driver for mass storage.

  @file         qhsusb_fd_ms.h
 
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "UsbUrb.h"
#include <Protocol/EFIUsbMsdPeripheral.h>


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*USB MSD BOT specific standard requests*/
#define USB_SETUP_REQ_MS_RESET          0xff
#define USB_SETUP_REQ_MS_GET_MAX_LUN    0xfe

//
// TODO: Need to modify eMMC to read/write
// to handle the case where RX/TX BURFFER
// is smaller than the requested one.
//
#define QHSUSB_MS_RX_BUFFER_SIZE (256*1024)

#define QHSUSB_MS_TX_BUFFER_SIZE (256*1024)


#define MS_RX_BUF_SIZE (QHSUSB_MS_RX_BUFFER_SIZE) /**< size of each (of 2) Rx buffers */
#define MS_TX_BUF_SIZE (QHSUSB_MS_TX_BUFFER_SIZE) /**< size of  Tx buffer */

//typedef LUN_INFO_TYPE lun_info_type;

typedef enum {
  PREPARE_FOR_CBW,
  SEND_TO_HOST,
  RCV_FROM_HOST,
  HOST_READ,
  SEND_CSW,
  SEND_TO_HOST_REMAIN,
  HOST_READ_REMAIN
}xfer_status_enum;
#define FS_HOTPLUG_NAME_MAX 512

#define MAX_NUM_LUNS  16

#define MS_EP_IN_IDX   1
#define MS_EP_OUT_IDX  1
#define MS_EP_RX_IDX MS_EP_OUT_IDX
#define MS_EP_TX_IDX MS_EP_IN_IDX

/**
  @brief
  Properties of a LUN
 */
typedef struct _LUN_PROPERTY LUN_PROPERTY;
struct _LUN_PROPERTY {

  UINT32 enabled         :1;
  UINT32 removable       :1;
  UINT32 need_attention  :1;
  UINT32 read_only       :1;

};


/**
  @brief
  Client READ ONLY structure to provide current USB MSD configuration
 */
typedef struct
{
  /* LUN index */
  UINT8                             index;

  LUN_PROPERTY                      property;

  /* For the block device information */
  UINT32                            block_size;

  /* Number of blocks supported by the Block IO device */
  UINT32                            num_blocks;

}LUN_INFO_TYPE;

/**
  @brief
  Bookkeeping structure to track the LUN - BLKIO handle association
 */
typedef struct _USBMSD_LUN_ASSOCIATION USBMSD_LUN_ASSOCIATION;
struct _USBMSD_LUN_ASSOCIATION {
  EFI_BLOCK_IO_PROTOCOL *BlkIoHandle;
  LUN_INFO_TYPE         Lun;
};

typedef struct 
{
  uint8                    num_luns;
  USBMSD_LUN_ASSOCIATION   *luns;
}device_info_type;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//============================================================================
/**
 * @function  qhsusb_fd_ms_is_tx_active
 *
 * @brief check if there is Tx transfer active.
 *
 * @note
 *
 * @param None.
 *
 * @return True if MS Tx is active, False otherwise..
 *
 */
//============================================================================
boolean qhsusb_fd_ms_is_tx_active(void);
//============================================================================
/**
 * @function  qhsusb_fd_ms_send_request
 *
 * @brief send data to host
 *
 * @note
 *
 * @param buff, len, dir, status.
 *
 * @return status (o is OK)
 *
 */
//============================================================================
uint32 qhsusb_fd_ms_send_request(void* buff, uint32 len, uint8 dir,  xfer_status_enum status);
//============================================================================
/**
 * @function  set_ms_packet_size
 *
 * @brief update MS packet size according to the USB spped (HS/LS)
 *
 * @note
 *
 * @param wMaxPacketSize
 *
 * @return None.
 *
 */
//============================================================================
void set_ms_packet_size(uint16 wMaxPacketSize);
//============================================================================
/**
 * @function  qhsusb_fd_ms_hotplug_dev_read
 *
 * @brief read from eMMC device.
 *
 * @note
 *
 * @param lun_num, start_address, buffer_size
 *
 * @return True on success, False otherwise.
 *
 */
//============================================================================
boolean qhsusb_fd_ms_hotplug_dev_read (uint8 lun_num, uint32  start_address, uint32 buffer_size);
//============================================================================
/**
 * @function  qhsusb_fd_ms_hotplug_dev_write
 *
 * @brief write to eMMC device.
 *
 * @note
 *
 * @param lun_num, start_address, num_blocks
 *
 * @return True on success, False otherwise.
 *
 */
//============================================================================
boolean qhsusb_fd_ms_hotplug_dev_write(uint8 lun_num, uint32  start_address, uint32  buffer_size);
//============================================================================
/**
 * @function  qhsusb_fd_ms_init
 *
 * @brief Initialize urb params for MS Tx/Rx .
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
void qhsusb_fd_ms_init(void);
//============================================================================
/**
 * @function  qhsusb_ms_init
 *
 * @brief init ms parameters after the enum process is over.
 *
 * @note
 *
 * @param device
 *
 * @return None.
 *
 */
//============================================================================
void qhsusb_ms_init(const struct qhsusb_dcd_dsc_device* dev);

#endif /* #ifndef QHSUSB_FD_MS_H */
