/**
 * @file UsbfnLoopback.h
 *
 * Usbfn Test Application top level header.
 *
 * Defines various USB related constants, data transfer structures/functions,
 * and an application level printing abstraction.
 *
 * Copyright (c) 2014, QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/02/16   ck      Update the max allowed xfer size in loopback test
 01/28/14   ml      Initial revision

=============================================================================*/

#ifndef _USBFN_LOOPBACK_H_
#define _USBFN_LOOPBACK_H_


#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/EFIUsbfnIo.h>
#include <Protocol/EFIUsbInitProtocol.h>


// Name used for TestInterface banners
#define TEST_NAME "UsbfnTest"


// USB VID/PID for Loopback device, must be supported by QC host serial driver
#define USBLB_VENDOR_ID               0x05C6
#define USBLB_PRODUCT_ID              0xF00D


// USB endpoint number for bulk data transfers (both IN/OUT used)
#define USBLB_BULK_EP                 1


// USB class request used by host tool for test configuration
#define USB_CLS_REQ_LOOPBACK          0


// Various Loopback app constants
#define USBLB_CONTROL_EP              0
#define USBLB_CONTROL_BUF_SIZE        (16 * 1024)
#define USBLB_MAX_TRANSFER_SIZE       (16 * 1024 * 1024) // max supported by host tool
#define USBLB_TEST_TIMEOUT            (10 * 1000 * 1000 * 10) // 10 s in 100ns timer units


// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Tx) \
  ((EndpointIndex) | ((Tx) ? 0x80 : 0x00))

// Get ep direction from USB request (see USB 3.0 ch 9.3)
#define USB_REQUEST_EPDIR(index)      (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
                                                               EfiUsbEndpointDirectionHostOut)

// Get ep and dir from wIndex field of USB request (see USB 3.0 ch 9.3.4)
#define USB_INDEX_TO_EP(index)        ((index) & 0xf)
#define USB_INDEX_TO_EPDIR(index)     (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
                                                               EfiUsbEndpointDirectionHostOut)

// Get descriptor type and index from wValue field of USB request (see USB 3.0 ch 9.4.3)
#define USB_VALUE_TO_DESC_TYPE(value) ((value) >> 8)
#define USB_VALUE_TO_DESC_IDX(value)  ((value) & 0xff)

// Other USB related constants/macros
#define USB_MAX_ENDPOINT_NUMBER       15

#define USB_SETUP_RT_TYPE_SHFT        5
#define USB_SETUP_RT_TYPE_BMSK        (0x3 << USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE(bmRT)       (((bmRT) & USB_SETUP_RT_TYPE_BMSK) >> USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE_STD         0
#define USB_SETUP_RT_TYPE_CLS         1
#define USB_SETUP_RT_TYPE_VND         2

#define USB_SETUP_RT_RCP_SHFT         0
#define USB_SETUP_RT_RCP_BMSK         (0x1f << USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP(bmRT)        (((bmRT) & USB_SETUP_RT_RCP_BMSK) >> USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP_DEV          0
#define USB_SETUP_RT_RCP_IFC          1
#define USB_SETUP_RT_RCP_EP           2
#define USB_SETUP_RT_RCP_OTH          3


// Transfer state
typedef enum XFER_STATE_TYPE {
  XFER_FREE,             // unused, but cannot be queued
  XFER_PENDING_ENQUEUE,  // the transfer is waiting to be queued
  XFER_PENDING_COMPLETE, // waiting for queued transfer to complete
} XFER_STATE_TYPE;


// Transfer type
typedef struct XFER_TYPE {
  UINT8                         EndpointIndex; // endpoint of transfer
  EFI_USBFN_ENDPOINT_DIRECTION  Direction;     // direction of transfer
  XFER_STATE_TYPE               XferState;     // state of transfer
  UINT8                         *Buffer;       // application supplied data buffer
  UINTN                         BufferSize;    // size of data buffer
  UINTN                         BufferOffset;  // offset into buffer of data start
  UINTN                         XferSize;      // size of data in buffer to transfer
} XFER_TYPE;


// Loopback test mode
typedef enum LB_TEST_MODE {
  LB_TEST_MODE_INVALID,
  LB_TEST_MODE_TX_ONLY,
  LB_TEST_MODE_RX_ONLY,
  LB_TEST_MODE_LOOPBACK
} LB_TEST_MODE;


// Loopback test configuration
typedef struct LB_TEST_CONFIG {
  UINT32          Mode;
  UINT32          Iterations;
  UINT32          BufSize;
} LB_TEST_CONFIG;


// Loopback test data
typedef struct LB_TEST_DATA {
  UINTN           RxIteration;
  UINTN           TxIteration;
  BOOLEAN         RxZlt;
  BOOLEAN         TxZlt;
  XFER_TYPE       Xfer;
  EFI_EVENT       TimeoutTimer;
} LB_TEST_DATA;


// Levels of verbosity supported by application
typedef enum APP_PRINT_LVL {
  APL_ERROR = 0,
  APL_WARN,
  APL_ALWAYS,
  APL_VERB,
  APL_VVERB
} APP_PRINT_LVL;


// Conditionally print a message based on application verbosity level
#define PRINT(Lvl, Str, args...)  \
  if (Lvl <= gAppPrintLvl)    \
    AsciiPrint ("[%d]%a: " Str "\n", Lvl, __FUNCTION__, ##args)


// For debug stack info.
#define FNC_ENTER_MSG() PRINT(APL_VVERB, "++")
#define FNC_LEAVE_MSG() PRINT(APL_VVERB, "--")


// global application verbosity level
extern INTN  gAppPrintLvl;


/**
 * Queues a transfer to the Usbfn driver.
 *
 * @param[in]     UsbfnIo   UsbfnIo protocol instance
 * @param[in,out] Xfer      The transfer to be queued
 *
 * @return Status from Usbfn
 */
EFI_STATUS
IssueXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  );


/**
 * Aborts a transfer that has been submitted to Usbfn.
 *
 * @param[in]     UsbfnIo   UsbfnIo protocol instance
 * @param[in,out] Xfer      The transfer to be aborted
 *
 * @return Status from Usbfn
 */
EFI_STATUS
AbortXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  );


#endif /* _USBFN_LOOPBACK_H_ */
