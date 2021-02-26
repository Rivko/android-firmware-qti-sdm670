/*=======================================================================*/
/**
  @file         UsbMsdScsi.c

  @brief        Handle the SCSI commands

  @details      
  Copyright (c) 2011 - 2014, 2017 QUALCOMM Technologies Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE


  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  02/01/2017  pm        Port from Boot.xf.1.4, Mark mass storage drive partition as always removable
  12/08/2014  ck        Fix incorrect LunNum after unmount
  12/05/2014  ck        Removed banned API
  10/14/2014  ck        Add state machine and remove global vars
  04/30/2014  ar        Cleanup for NDEBUG build  
  22/03/2011  sm        initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Library/UefiBootServicesTableLib.h> //gBS->*
#include <Protocol/BlockIo.h>                 // BlockIo for eMMC
#include <UsbError.h>                         // USB_ASSERT_*
#include "UsbMsdScsi.h"                       // scsi related defs
#include "UsbMsdDesc.h"                       // USBMSD_BULK_EP
#include "UsbMsdUtility.h"                    // *_CHK
#include <Library/QcomBaseLib.h>              // CopyMemS

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QHSUSB_SUCCESS 0
#define QHSUSB_ERROR   ((UINT32)(-1))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* keep the original MACRO constant used in the file*/
#define MS_EP_IN_IDX   USBMSD_BULK_EP
#define MS_EP_OUT_IDX  USBMSD_BULK_EP
#define MAX_NUM_LUNS   USB_MSD_MAX_SUPPORTED_LUN

/* @brief Store the relevant parameters in CBW for eMMC read and write. */
typedef struct
{
  UINT32                    lba;
  UINT8                     lun;
  UINT32                    buffer_size;
} transfer_type;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static const char vendor_str[MAX_VENDOR_STR_SIZE]     = "Qualcomm";
static const char product_str[MAX_PRODUCT_STR_SIZE]   = "MMC Storage     "; 
static const char prod_rev_str[MAX_PROD_REV_STR_SIZE] = "1.00";

/*----------------------------------------------------------------------------
 * Static Function Definitions
 * -------------------------------------------------------------------------*/
static DEV_STATE handle_scsi_command        (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_inquiry             (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_read_format_capacity(IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_read_capacity       (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_mode_sense          (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_mode_select         (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_request_sense       (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_test_unit_ready     (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_write               (IN USBMSD_DEV *UsbMsd);
static DEV_STATE handle_read                (IN USBMSD_DEV *UsbMsd);

#ifndef _MSC_VER
//============================================================================
/**
* @function  memset
*
* @brief wrapper function of gBS->SetMem 
*
* @param[in]  pDest Buffer to which the data are set
* @param[in]  iVal  The value to be set
* @param[in]  iLen  Number of bytes to be set
*
*/
//============================================================================
static void memset(VOID * pDest, INTN iVal, UINTN iLen)
{
  gBS->SetMem(pDest, (UINTN)iLen, (uint8)iVal);
}
#endif  // _MSC_VER
/* @breif reset sense data */
void reset_sense_data(
  IN USBMSD_DEV *UsbMsd,
  IN uint8 *sense_data)
{
  memset(sense_data, 0, MAX_REQUEST_SENSE_SIZE);
  sense_data[0] = 0x70;
  sense_data[7] = 0x0a;
}


//============================================================================
/**
* @function  qhsusb_fd_ms_send_request
*
* @brief send data to host
*
* @note wrapper function for IssueXfer
*
* @param[in]  UsbfnIo UsbfnIo protocol handle
* @param[in]  Xfer    Data to be transfered. 
*
* @return status (0 is OK)
*
*/
//============================================================================
__inline static uint32 qhsusb_fd_ms_send_request(IN EFI_USBFN_IO_PROTOCOL *UsbfnIo, IN XFER_TYPE *Xfer)
{
  EFI_STATUS  Status   = EFI_SUCCESS;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    (UsbfnIo && Xfer),
    ON_EXIT,
    EFI_INVALID_PARAMETER, 
    Status,
    "Invalid Parameter");

  Status = IssueXfer(UsbfnIo, Xfer);
  WRN_CHK("IssueXfer failed %r", Status);
  
ON_EXIT:
  FNC_LEAVE_MSG();
  return (EFI_SUCCESS == Status) ? QHSUSB_SUCCESS : QHSUSB_ERROR;
}

//============================================================================
/**
* @function  qhsusb_fd_ms_hotplug_dev_read
*
* @brief read from eMMC device.
*
* @note
*
* @param[in] UsbMsd        The instance of USBMSD_DEV
* @param[in] lun_num       The LUN to which the data is written. 
* @param[in] start_address The start address to write the data.
* @param[in] buffer_size   The number of bytes to be written to the LUN.
*
* @return True on success, False otherwise.
*
*/
//============================================================================
static boolean qhsusb_fd_ms_hotplug_dev_read(IN USBMSD_DEV *UsbMsd, IN uint8 lun_num, IN uint32  start_address, IN uint32 buffer_size)
{
  EFI_BLOCK_IO_PROTOCOL*           BlkIoProtocol = NULL;
  EFI_STATUS                       Status        = EFI_NO_MEDIA;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    ((UsbMsd) && (lun_num < MAX_NUM_LUNS)), 
    ON_EXIT, 
    EFI_INVALID_PARAMETER, 
    Status, 
    "Invalid Parameter");

  BlkIoProtocol = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].BlkIoHandle; 

  if (!BlkIoProtocol) {
    Status = EFI_DEVICE_ERROR;
    DBG(EFI_D_ERROR, "Data cannot be written. BlockIo Handle is NULL");
  } 
  else 
  {
    Status = BlkIoProtocol->ReadBlocks(
      BlkIoProtocol,
      BlkIoProtocol->Media->MediaId,
      start_address,
      buffer_size,
      (byte*)UsbMsd->StateMachine.BulkTxXfer.Buffer
      );
    ERR_CHK("BlkIoProtocol ReadBlocks Fails: %p", Status);
  }   

ON_EXIT:
  FNC_LEAVE_MSG();
  return (FALSE == EFI_ERROR(Status));
}


//============================================================================
/**
* @function  qhsusb_fd_ms_hotplug_dev_write
*
* @brief write to eMMC device.
*
* @note
*
* @param[in] UsbMsd        The instance of USBMSD_DEV
* @param[in] lun_num       The LUN to which the data is written.
* @param[in] start_address The start address to write the data.
* @param[in] buffer_size   The number of bytes to be written to the LUN.
*
* @return True on success, False otherwise.
*
*/
//============================================================================
static boolean qhsusb_fd_ms_hotplug_dev_write(IN USBMSD_DEV *UsbMsd, IN uint8 lun_num, IN uint32  start_address, IN uint32 buffer_size)
{

  EFI_BLOCK_IO_PROTOCOL*           BlkIoProtocol  = NULL;
  EFI_STATUS                       Status         = EFI_NO_MEDIA;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    ((UsbMsd) && (lun_num < MAX_NUM_LUNS)), 
    ON_EXIT,
    EFI_INVALID_PARAMETER, 
    Status,
    "Invalid Parameter");

  BlkIoProtocol = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].BlkIoHandle;

  if (!BlkIoProtocol) {
    Status = EFI_DEVICE_ERROR;
    DBG(EFI_D_ERROR, "Data cannot be written. BlockIo Handle is NULL");
  }
  else {
    Status = BlkIoProtocol->WriteBlocks(
      BlkIoProtocol,
      BlkIoProtocol->Media->MediaId,
      start_address,
      buffer_size,
      (byte*)UsbMsd->StateMachine.BulkRxXfer.Buffer
      );
    ERR_CHK("BlkIoProtocol WriteBlocks Fails: %p", Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return (FALSE == EFI_ERROR(Status));
}


//============================================================================
/**
 * @function  is_cbw_valid
 *
 * @brief check if the CBW received from the host is valid.
 *
 * @note
 *
 * @param[in] UsbMsd            The USBMSD_DEV instance
 *
 * @return True Buffer represents a valid CBW, False otherwise. 
 *
 */
//============================================================================
static boolean is_cbw_valid(IN USBMSD_DEV *UsbMsd)
{
  uint8          cbw_len       = 0;
  qhsusb_cbw_t  *cbw           = NULL;
  qhsusb_csw_t  *csw           = NULL;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  cbw_len       = UsbMsd->StateMachine.CBW.BufferOffset;
  cbw           = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  csw           = (qhsusb_csw_t *)UsbMsd->StateMachine.CSW.Buffer;

  /* check if CBW valid */
  if (cbw->bCBWCBLength && cbw_len == QHSUSB_CBW_LEN &&
      cbw->dCBWSignature == QHSUSB_CBW_SIGNATURE)
  { 
    /* check if CBW meaningful */
    if (cbw->bCBWLUN < MAX_NUM_LUNS)
    {      
      csw->dCSWTag = cbw->dCBWTag;
      return TRUE;
    }
  }
  else {
    // usbmassbulk spec 6.6.1
    // STALL the Bulk-Out pipe
    UsbMsd->UsbfnIoProtocol->SetEndpointStallState(UsbMsd->UsbfnIoProtocol, MS_EP_OUT_IDX, EfiUsbEndpointDirectionDeviceRx, TRUE);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return FALSE;
}

//============================================================================
/**
 * @function  handle_new_cbw
 *
 * @brief in case the CBW is valid, handle it.
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
DEV_STATE handle_new_cbw(IN USBMSD_DEV *UsbMsd)
{
  DEV_STATE   next_state  = DEV_STATE_SAME;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");
  
  if (is_cbw_valid(UsbMsd))
  {
    next_state = handle_scsi_command(UsbMsd);
  }
  else{
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, 0);
    USB_ASSERT_GOTO(FALSE, ON_EXIT);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_scsi_command
 *
 * @brief Parse the CBW and handle the scsi command accordingly.
 *
 * @note
 *
 * @param[in] UsbMsd            The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_scsi_command(IN USBMSD_DEV *UsbMsd)
{
  qhsusb_cbw_t     *cbw            = NULL;
  uint8             scsi_cmd       = 0;
  uint8             lun_num        = 0;
  boolean           need_attention = FALSE;
  boolean           lun_disabled   = FALSE;
  DEV_STATE         next_state     = DEV_STATE_SAME;  

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  // Init variables
  cbw                 = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  scsi_cmd            = cbw->CBWCB[0];
  lun_num             = cbw->bCBWLUN;
  USB_ASSERT_GOTO_STR((lun_num < MAX_NUM_LUNS), ON_EXIT, "Invalid Parameter");
  need_attention      = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.NeedAttention;
  lun_disabled        = (FALSE == UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.Enabled);
  next_state          = DEV_STATE_SAME;

  //
  // If the LUN is in the UNIT ATTENTION/DISABLED condition, it fails all commands
  // except INQUIRY, REPORT LUNS, and REQUEST SENSE
  //
  if( need_attention ){
    if(scsi_cmd != SCSI_INQUIRY && 
       scsi_cmd != SCSI_REPORT_LUNS && 
       scsi_cmd != SCSI_REQUEST_SENSE)
    {      
      DBG(
          EFI_D_WARN,
          "LUN %d in need_attention state, cannot handle scsi_cmd 0x%x",
          lun_num,
          scsi_cmd);
      UsbMsd->StateMachine.UsbfnIo->SetEndpointStallState(UsbMsd->StateMachine.UsbfnIo,MS_EP_IN_IDX, EfiUsbEndpointDirectionDeviceTx, TRUE);
      next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, cbw->dCBWDataTransferLength);
      goto ON_EXIT;
    }
  }

  if(lun_disabled) {
    if(scsi_cmd != SCSI_INQUIRY &&
       scsi_cmd != SCSI_REPORT_LUNS &&
       scsi_cmd != SCSI_REQUEST_SENSE &&
       scsi_cmd != SCSI_TEST_UNIT_READY
    ){
      DBG(
          EFI_D_WARN,
          "LUN %d is disabled cannot handle scsi_cmd 0x%x",
          lun_num,
          scsi_cmd);
      UsbMsd->StateMachine.UsbfnIo->SetEndpointStallState(UsbMsd->StateMachine.UsbfnIo,MS_EP_IN_IDX, EfiUsbEndpointDirectionDeviceTx, TRUE);
      next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, cbw->dCBWDataTransferLength);
      goto ON_EXIT;
    }
  }

  switch(scsi_cmd) 
  {
  case SCSI_INQUIRY:
    DBG(EFI_D_INFO, "handle inquiry");
    next_state = handle_inquiry(UsbMsd);
    break;
  case SCSI_READ_FORMAT_CAPACITY:   
    DBG(EFI_D_INFO, "handle read_format_cap");
    next_state = handle_read_format_capacity(UsbMsd);
    break;
  case SCSI_READ_CAPACITY10:
    DBG(EFI_D_INFO, "handle read_cap");
    next_state = handle_read_capacity(UsbMsd);
    break;
  case SCSI_READ10:
    DBG(EFI_D_INFO, "handle read");
    next_state = handle_read(UsbMsd);
    break;
  case SCSI_WRITE10:
    DBG(EFI_D_INFO, "handle write");
    next_state = handle_write(UsbMsd);
    break;
  case SCSI_MODE_SENSE6:
    DBG(EFI_D_INFO, "handle mode sense");
    next_state = handle_mode_sense(UsbMsd);
    break;
  case SCSI_REQUEST_SENSE:
    DBG(EFI_D_INFO, "handle req sense");
    next_state = handle_request_sense(UsbMsd);
    break;
  case SCSI_MODE_SELECT6:
    DBG(EFI_D_INFO, "handle mode select");
    next_state = handle_mode_select(UsbMsd);
    break;
  case SCSI_TEST_UNIT_READY:
    DBG(EFI_D_INFO, "handle test unit ready");
    next_state = handle_test_unit_ready(UsbMsd);
    break;
  case SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL:
  case SCSI_SYNCHRONIZE_CACHE10:
    /*assuming not removable media, ignore*/
    DBG(EFI_D_INFO, "handle remove %d",scsi_cmd);
    next_state = sending_csw(UsbMsd, CSW_GOOD_STATUS,0);
    break;
  default:
    /* other command not supported, just return OK */
    DBG(EFI_D_ERROR, "handle other 0x%x",scsi_cmd);
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS,cbw->dCBWDataTransferLength);
  }
ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_inquiry
 *
 * @brief handle inquiry SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_inquiry(IN USBMSD_DEV *UsbMsd)
{
  uint32            qstatus        = QHSUSB_SUCCESS;
  uint8             lun_num        = 0;
  uint8            *ms_tx_buffer   = NULL;
  UINTN             ms_tx_buf_siz  = 0;
  DEV_STATE         next_state     = DEV_STATE_ATTACHED;  

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");
  
  lun_num        = ((qhsusb_cbw_t *)(UsbMsd->StateMachine.CBW.Buffer))->bCBWLUN;
  USB_ASSERT_GOTO_SETSTS_STR(
    (lun_num < MAX_NUM_LUNS),
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  ms_tx_buffer   = UsbMsd->StateMachine.BulkTxXfer.Buffer;
  ms_tx_buf_siz  = UsbMsd->StateMachine.BulkTxXfer.BufferSize;

  memset(ms_tx_buffer, 0, MAX_INQUERY_RES_SIZE);

  // Regardless what the underlying BlkIo device indicates,
  // always indicated to the host that the block io device
  // is removable  
  //if (UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.Removable)
  //{
    ms_tx_buffer[1] = 0x80; /* Removable Medium */
  //}
  ms_tx_buffer[2] = 0x4; /* spc-2 */
  ms_tx_buffer[3] = 2;   /* Response data format*/
  ms_tx_buffer[4] = 31;  /* additionalLength */
  ms_tx_buffer   += 8; /* destination address for strings identification  */
  ms_tx_buf_siz  -= 8;
  CopyMemS(ms_tx_buffer, ms_tx_buf_siz, vendor_str, MAX_VENDOR_STR_SIZE);
  ms_tx_buffer  += MAX_VENDOR_STR_SIZE;
  ms_tx_buf_siz -= MAX_VENDOR_STR_SIZE;
  CopyMemS(ms_tx_buffer, ms_tx_buf_siz, product_str, MAX_PRODUCT_STR_SIZE);
  ms_tx_buffer  += MAX_PRODUCT_STR_SIZE;
  ms_tx_buf_siz -= MAX_PRODUCT_STR_SIZE;
  CopyMemS(ms_tx_buffer, ms_tx_buf_siz, prod_rev_str, MAX_PROD_REV_STR_SIZE);
 
  UsbMsd->StateMachine.BulkTxXfer.XferSize     = MAX_INQUERY_RES_SIZE;
  UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");    
    goto ON_EXIT;
  }

  next_state = DEV_STATE_SEND_TO_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_read_format_capacity
 *
 * @brief handle read format capacity SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_read_format_capacity(IN USBMSD_DEV *UsbMsd)
{
  uint8             lun_num        = 0;
  uint32            num_blocks     = 0;
  uint32            block_size     = 0;
  uint8            *ms_tx_buffer   = NULL;
  UINTN             ms_tx_buf_siz  = 0;
  qhsusb_cbw_t     *cbw            = NULL;
  qhsusb_csw_t     *csw            = NULL;
  uint32            qstatus        = QHSUSB_SUCCESS;
  DEV_STATE         next_state     = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  lun_num      = ((qhsusb_cbw_t *)(UsbMsd->StateMachine.CBW.Buffer))->bCBWLUN;
  USB_ASSERT_GOTO_SETSTS_STR(
    (lun_num < MAX_NUM_LUNS),
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  ms_tx_buffer  = UsbMsd->StateMachine.BulkTxXfer.Buffer;
  ms_tx_buf_siz = UsbMsd->StateMachine.BulkTxXfer.BufferSize;
  cbw           = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  csw           = (qhsusb_csw_t *)UsbMsd->StateMachine.CSW.Buffer;  

  memset(ms_tx_buffer, 0, MAX_READ_FORMAT_CAPACITY_SIZE);
  
  ms_tx_buffer[3]  = 8;          /* capacity list length */
  num_blocks = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.NumBlocks; /* for Last Logical Block Address */
  block_size = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.BlockSize;
  num_blocks = SWAP32(num_blocks);
  block_size = SWAP32(block_size);
  CopyMemS(&ms_tx_buffer[4], ms_tx_buf_siz - 4, &num_blocks, sizeof(uint32));
  CopyMemS(&ms_tx_buffer[8], ms_tx_buf_siz - 8, &block_size, sizeof(uint32));
  ms_tx_buffer[8] = 0x02;  /* Formatted Media - Current media capacity */
  csw->dCSWDataResidue = cbw->dCBWDataTransferLength - MAX_READ_FORMAT_CAPACITY_SIZE;

  UsbMsd->StateMachine.BulkTxXfer.XferSize     = MAX_READ_FORMAT_CAPACITY_SIZE;
  UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_SEND_TO_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_read_capacity
 *
 * @brief handle read capacity SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_read_capacity(IN USBMSD_DEV *UsbMsd)
{
  uint8             lun_num       = 0;
  uint32            num_blocks    = 0;
  uint32            block_size    = 0;
  uint8            *ms_tx_buffer  = NULL;
  UINTN             ms_tx_buf_siz = 0;
  uint32            qstatus       = QHSUSB_SUCCESS;
  DEV_STATE         next_state    = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  lun_num      = ((qhsusb_cbw_t *)(UsbMsd->StateMachine.CBW.Buffer))->bCBWLUN;
  USB_ASSERT_GOTO_SETSTS_STR(
    (lun_num < MAX_NUM_LUNS),
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  ms_tx_buffer  = UsbMsd->StateMachine.BulkTxXfer.Buffer;
  ms_tx_buf_siz = UsbMsd->StateMachine.BulkTxXfer.BufferSize;

  num_blocks = (UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.NumBlocks)-1; /* for Last Logical Block Address */
  block_size = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.BlockSize;

  num_blocks = SWAP32(num_blocks);
  block_size = SWAP32(block_size);

  memset(ms_tx_buffer, 0 ,MAX_READ_CAPACITY_SIZE);
  CopyMemS(ms_tx_buffer, ms_tx_buf_siz, &num_blocks, sizeof(uint32));
  CopyMemS(&ms_tx_buffer[4], ms_tx_buf_siz - 4, &block_size, sizeof(uint32));

  UsbMsd->StateMachine.BulkTxXfer.XferSize     = MAX_READ_CAPACITY_SIZE;
  UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_SEND_TO_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_mode_sense
 *
 * @brief handle mode sense SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_mode_sense(IN USBMSD_DEV *UsbMsd)
{
  uint8     *ms_tx_buffer = NULL;
  uint32     qstatus      = QHSUSB_SUCCESS;
  DEV_STATE  next_state   = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    UsbMsd,
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  ms_tx_buffer = UsbMsd->StateMachine.BulkTxXfer.Buffer;  

  memset(ms_tx_buffer, 0, MAX_MODE_SENSE_SIZE);
  ms_tx_buffer[0] = 0x03; // mode data length => number of bytes followed
  ms_tx_buffer[1] = 0x00; // medium type => SBC
  ms_tx_buffer[2] = 0x00; // device specific parameter => set none
  ms_tx_buffer[3] = 0x00; // block descriptor len => the len in bytes of all the block descriptors. We return none.

  UsbMsd->StateMachine.BulkTxXfer.XferSize     = MAX_MODE_SENSE_SIZE;
  UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_SEND_TO_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}
//============================================================================
/**
 * @function  handle_mode_select
 *
 * @brief handle mode select SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_mode_select(IN USBMSD_DEV *UsbMsd)
{
  qhsusb_cbw_t *cbw                   = NULL;
  uint8         parameter_list_length = 0;
  uint32        qstatus               = QHSUSB_SUCCESS;
  DEV_STATE     next_state            = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    UsbMsd,
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  cbw                   = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  parameter_list_length = cbw->CBWCB[4];  
  
  /* the host send us OUT transfer in the length of  parameter_list_length*/
  UsbMsd->StateMachine.BulkRxXfer.XferSize     = parameter_list_length;
  UsbMsd->StateMachine.BulkRxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkRxXfer);  
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_RCV_FROM_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_request_sense
 *
 * @brief handle request sense SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_request_sense(IN USBMSD_DEV *UsbMsd)
{
  uint8             lun_num       = 0;
  uint8            *ms_tx_buffer  = NULL;
  UINTN             ms_tx_buf_siz = 0;
  uint32            qstatus       = QHSUSB_SUCCESS;
  uint8             sense_data[MAX_REQUEST_SENSE_SIZE];
  DEV_STATE         next_state    = DEV_STATE_ATTACHED;
 
  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_SETSTS_STR(
    UsbMsd,
    ON_EXIT, 
    QHSUSB_ERROR,
    qstatus, 
    "Invalid Parameter");

  lun_num      = ((qhsusb_cbw_t *)(UsbMsd->StateMachine.CBW.Buffer))->bCBWLUN;
  USB_ASSERT_GOTO_SETSTS_STR(
    (lun_num < MAX_NUM_LUNS),
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  ms_tx_buffer  = UsbMsd->StateMachine.BulkTxXfer.Buffer;
  ms_tx_buf_siz = UsbMsd->StateMachine.BulkTxXfer.BufferSize;

  reset_sense_data(UsbMsd, &sense_data[0]);
  if(UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.NeedAttention) {    
    sense_data[2]  = 0x06;
    sense_data[12] = 0x28;  /*NOT READY TO READY CHANGE, MEDIUM MAY HAVE CHANGED*/
    sense_data[13] = 0x00;

    //After returning the sense data, the device is no longer in UNIT ATTENSTION state
    UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.NeedAttention  = FALSE;
    UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.Enabled        = TRUE;
    DBG(EFI_D_INFO, "LUN %d is out of NEED ATTENTION",lun_num);

  } else if(!UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.Enabled){    
    sense_data[2]  = 0x02;
    sense_data[12] = 0x3A;  /*MEDIUM NOT PRESENT*/
    sense_data[13] = 0x00;
  } else {
    DBG(EFI_D_ERROR, "RequestSenseResponse Not Set");
  }
  
  CopyMemS(ms_tx_buffer, ms_tx_buf_siz, sense_data, MAX_REQUEST_SENSE_SIZE);

  UsbMsd->StateMachine.BulkTxXfer.XferSize     = MAX_REQUEST_SENSE_SIZE;
  UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_SEND_TO_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_test_unit_ready
 *
 * @brief handle test unit ready SCSI command
 *
 * @note
 *
 * @param[in] UsbMsd        The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_test_unit_ready(IN USBMSD_DEV *UsbMsd)
{  
  qhsusb_cbw_t     *cbw          = NULL;
  uint8             lun_num      = 0;
  boolean           lun_enabled  = FALSE;
  DEV_STATE         next_state   = DEV_STATE_SAME; 

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  cbw          = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  lun_num      = cbw->bCBWLUN;
  USB_ASSERT_GOTO_STR((lun_num < MAX_NUM_LUNS), ON_EXIT, "Invalid Parameter");

  lun_enabled  = UsbMsd->UsbMsdMedia.BlkIoLunMap[lun_num].Lun.Property.Enabled;

  if(lun_enabled) {
    //DBG(EFI_D_ERROR, "LUN %d UNIT READY",current_xfer->lun);
    next_state = sending_csw(UsbMsd, CSW_GOOD_STATUS, 0);
  } else {
      //DBG(EFI_D_ERROR, "LUN %d UNIT NOT READY",current_xfer->lun);
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, cbw->dCBWDataTransferLength);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}


//============================================================================
/**
* @function  sending_csw
*
* @brief sending csw to the host
*
* @note
*
* @param[in] UsbMsd        The USBMSD_DEV instance
* @param[in] csw_status    The status of handling the CBW
* @param[in] data_residue  The remaining data to be transferred. 
*
* @return DEV_STATE        Next state machine state
*
*/
//============================================================================
DEV_STATE sending_csw(IN USBMSD_DEV *UsbMsd, IN uint8 csw_status, IN uint32 data_residue)
{
  qhsusb_cbw_t *cbw        = NULL;
  qhsusb_csw_t *csw        = NULL;
  uint32        qstatus    = QHSUSB_SUCCESS;
  DEV_STATE     next_state = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");
  USB_ASSERT_GOTO_SETSTS_STR(
    UsbMsd,
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  cbw     = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  csw     = (qhsusb_csw_t *)UsbMsd->StateMachine.CSW.Buffer;

  cbw->bCBWCBLength    = 0; /* we are ready for a new CBW*/
  csw->dCSWDataResidue = data_residue;
  csw->bCSWStatus      = csw_status;
  
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.CSW);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_CSW;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  update_current_xfer_params
 *
 * @brief update current xfer params from CBW
 *
 * @note
 *
 * @param[in]  UsbMsd            The USBMSD_DEV instance
 * @param[out] current_xfer      The transfer to be updated with eMMC read/write info from CBW
 *
 */
//============================================================================
static void update_current_xfer_params(IN USBMSD_DEV *UsbMsd, OUT transfer_type *current_xfer)
{
  uint32         temp         = 0;
  uint32         xfer_len     = 0;
  qhsusb_cbw_t  *cbw          = NULL;

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR((UsbMsd && current_xfer), ON_EXIT, "Invalid Parameter");

  cbw                       = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  current_xfer->lun         = cbw->bCBWLUN;
  xfer_len                  = cbw->dCBWDataTransferLength;
  CopyMemS (&temp, sizeof(temp), (void*)&cbw->CBWCB[2], sizeof(temp));
  current_xfer->lba         = SWAP32(temp);
  current_xfer->buffer_size = xfer_len;

  DBG (EFI_D_INFO,
      "current_xfer->lun %d, current_xfer->lba %d, current_xfer->buffer_size %d",
      current_xfer->lun,      
      current_xfer->lba,
      current_xfer->buffer_size
      );

ON_EXIT:
  FNC_LEAVE_MSG();
  return;
}

//============================================================================
/**
 * @function  handle_read
 *
 * @brief handle read SCSI command
 *
 * @note
 *
 * @param[in]  UsbMsd       The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_read(IN USBMSD_DEV *UsbMsd)
{
  qhsusb_cbw_t     *cbw          = NULL;
  boolean           read_status  = FALSE;
  uint32            qstatus      = QHSUSB_SUCCESS;
  DEV_STATE         next_state   = DEV_STATE_SAME;
  transfer_type     current_xfer = { 0 };
  EFI_STATUS        Status;
  UINTN             byte_read    = 0;

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  cbw = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;

  update_current_xfer_params(UsbMsd, &current_xfer);   

  //
  // If the block device is not enabled, stall the endpoint 
  // and return CSW with error condition
  // 
  if(FALSE == UsbMsd->UsbMsdMedia.BlkIoLunMap[current_xfer.lun].Lun.Property.Enabled){
    Status = UsbMsd->StateMachine.UsbfnIo->SetEndpointStallState(UsbMsd->StateMachine.UsbfnIo,MS_EP_IN_IDX, EfiUsbEndpointDirectionDeviceTx, TRUE);
    WRN_CHK("SetEndpointStallState failed %r", Status);
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, cbw->dCBWDataTransferLength);
    goto ON_EXIT;
  }

  if (current_xfer.buffer_size > UsbMsd->StateMachine.BulkTxXfer.BufferSize) {    
    void *buff = UsbMsd->StateMachine.BulkTxXfer.Buffer;
    DBG(EFI_D_WARN, "host requests buffer size %d is greater than current tx buffer size %d",
      current_xfer.buffer_size,
      UsbMsd->StateMachine.BulkTxXfer.BufferSize);
    Status = UsbMsd->UsbfnIoProtocol->AllocateTransferBuffer(UsbMsd->UsbfnIoProtocol, current_xfer.buffer_size, (void **) &UsbMsd->StateMachine.BulkTxXfer.Buffer);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to allocate new buffer");
      UsbMsd->StateMachine.BulkTxXfer.Buffer = buff;
    }
    else {
      // new buffer is rdy, free the old one
      Status = UsbMsd->UsbfnIoProtocol->FreeTransferBuffer(UsbMsd->UsbfnIoProtocol, buff);
      WRN_CHK("FreeTransferBuffer failed %r", Status);
      // update the buffer size
      UsbMsd->StateMachine.BulkTxXfer.BufferSize = current_xfer.buffer_size;
    }
  }

  byte_read = (UsbMsd->StateMachine.BulkTxXfer.BufferSize < current_xfer.buffer_size) ?
    UsbMsd->StateMachine.BulkTxXfer.BufferSize : current_xfer.buffer_size;

  read_status = qhsusb_fd_ms_hotplug_dev_read(UsbMsd, current_xfer.lun, current_xfer.lba, byte_read);

  if(FALSE == read_status) {

    //
    // Currently if reading to the media fails, assume that the device is removed
    //
    DBG(EFI_D_ERROR, "Media Read Fails: Lun %d, LBA %d, Size %d",
        current_xfer.lun,
        current_xfer.lba,
        current_xfer.buffer_size
        );
    UsbMsd->UsbMsdMedia.BlkIoLunMap[current_xfer.lun].Lun.Property.Enabled = FALSE;
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, cbw->dCBWDataTransferLength);
  }else{
    UsbMsd->StateMachine.BulkTxXfer.XferSize     = current_xfer.buffer_size;
    UsbMsd->StateMachine.BulkTxXfer.BufferOffset = 0;
    qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);    
    next_state = (QHSUSB_SUCCESS == qstatus) ? DEV_STATE_SEND_TO_HOST : DEV_STATE_ATTACHED;
    if (QHSUSB_SUCCESS != qstatus) {
      DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
      goto ON_EXIT;
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  handle_write
 *
 * @brief handle write SCSI command
 *
 * @note
 *
 * @param[in]  UsbMsd       The USBMSD_DEV instance
 *
 * @return DEV_STATE        Next state machine state
 *
 */
//============================================================================
static DEV_STATE handle_write(IN USBMSD_DEV *UsbMsd)
{
  uint32          qstatus      = QHSUSB_SUCCESS;
  transfer_type   current_xfer = { 0 };
  UINTN           byte_written = 0;
  EFI_STATUS      Status       = EFI_SUCCESS;
  DEV_STATE       next_state   = DEV_STATE_ATTACHED;

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_SETSTS_STR(
    UsbMsd,
    ON_EXIT,
    QHSUSB_ERROR,
    qstatus,
    "Invalid Parameter");

  update_current_xfer_params(UsbMsd, &current_xfer);

  if (current_xfer.buffer_size > UsbMsd->StateMachine.BulkRxXfer.BufferSize) {
    void *buff = UsbMsd->StateMachine.BulkRxXfer.Buffer;
    DBG(EFI_D_WARN, "host requests buffer size %d is greater than current rx buffer size %d",
      current_xfer.buffer_size,
      UsbMsd->StateMachine.BulkRxXfer.BufferSize);
    Status = UsbMsd->UsbfnIoProtocol->AllocateTransferBuffer(UsbMsd->UsbfnIoProtocol, current_xfer.buffer_size, (void **)&UsbMsd->StateMachine.BulkRxXfer.Buffer);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to allocate new buffer");
      UsbMsd->StateMachine.BulkRxXfer.Buffer = buff;
    }
    else {
      // new buffer is rdy, free the old one
      Status = UsbMsd->UsbfnIoProtocol->FreeTransferBuffer(UsbMsd->UsbfnIoProtocol, buff);
      WRN_CHK("FreeTransferBuffer failed %r", Status);
      // update the buffer size
      UsbMsd->StateMachine.BulkRxXfer.BufferSize = current_xfer.buffer_size;
    }
  }

  byte_written = (UsbMsd->StateMachine.BulkRxXfer.BufferSize < current_xfer.buffer_size) ?
    UsbMsd->StateMachine.BulkRxXfer.BufferSize : current_xfer.buffer_size;
  UsbMsd->StateMachine.BulkRxXfer.XferSize     = byte_written;
  UsbMsd->StateMachine.BulkRxXfer.BufferOffset = 0;
  qstatus = qhsusb_fd_ms_send_request(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkRxXfer);
  if (QHSUSB_SUCCESS != qstatus) {
    DBG(EFI_D_ERROR, "qhsusb_fd_ms_send_request failed.");
    goto ON_EXIT;
  }

  next_state = DEV_STATE_RCV_FROM_HOST;

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}

//============================================================================
/**
 * @function  write_to_emmc_card
 *
 * @brief write data received from host to eMMC card.
 *
 * @note
 *
 * @param[in]  UsbMsd       The USBMSD_DEV instance
 *
 * @return True on success, False otherwise.
 *
 */
//============================================================================
static boolean write_to_emmc_card(IN USBMSD_DEV *UsbMsd)
{
  transfer_type     current_xfer = { 0 };
  boolean           write_result = FALSE;  

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  update_current_xfer_params(UsbMsd, &current_xfer);
  write_result = qhsusb_fd_ms_hotplug_dev_write(UsbMsd, current_xfer.lun, current_xfer.lba, current_xfer.buffer_size);

  //
  // If writing to the media fails, assume that the device is removed
  //
  if (FALSE == write_result) {
    DBG(EFI_D_ERROR, "Media Write Fails: Lun %d, LBA %d, Size %d",
      current_xfer.lun,
      current_xfer.lba,
      current_xfer.buffer_size
      );
    UsbMsd->UsbMsdMedia.BlkIoLunMap[current_xfer.lun].Lun.Property.Enabled = FALSE;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return write_result;
}


//============================================================================
/**
* @function  handle_rx_data
*
* @brief Handle the data sent to the device by the host. 
*
* @note
*
* @param[in]  UsbMsd       The USBMSD_DEV instance
*
* @return DEV_STATE        Next state machine state
*
*/
//============================================================================
DEV_STATE handle_rx_data(IN USBMSD_DEV *UsbMsd) {
  qhsusb_cbw_t     *cbw            = NULL;
  uint8             scsi_cmd       = 0;
  DEV_STATE         next_state     = DEV_STATE_SAME;

  FNC_ENTER_MSG();
  USB_ASSERT_GOTO_STR(UsbMsd, ON_EXIT, "Invalid Parameter");

  cbw            = (qhsusb_cbw_t *)UsbMsd->StateMachine.CBW.Buffer;
  scsi_cmd       = cbw->CBWCB[0];

  switch (scsi_cmd) {
  case SCSI_WRITE10:
  {
    boolean write_result;
    DBG(EFI_D_INFO, "write to emmc");
    write_result = write_to_emmc_card(UsbMsd);
    next_state = sending_csw(UsbMsd, write_result ? CSW_GOOD_STATUS : CSW_ERROR_STATUS, 0);
  }
  break;
  default:
    next_state = sending_csw(UsbMsd, CSW_ERROR_STATUS, 0);
    DBG(EFI_D_WARN, "Data from scsi cmd %d not handled", scsi_cmd);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return next_state;
}
