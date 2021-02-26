#ifndef __USB_ENUM_H__
#define __USB_ENUM_H__

#include <Protocol/EFIUsbfnIo.h>

//
// Types
//

// stage of USB Control transfer
typedef enum _CtrlTransferStage {
  CtrlSetup,
  CtrlData,
  CtrlStatus,
  CtrlStageMax
} CtrlTransferStage;

// statte of Xfer
typedef enum _XferStateType {
  Free, //Unused, but cannot be queued.
  PendingEnqueue, // the transfer is waiting to be queued in hw.
  PendingCompletion, // the transfer has been process by hw. waiting for further application process, if any
} XferStateType;

// A transfer
typedef struct XferType {
  UINT8              EndpointIndex; //endpoint on which this transfer will be queued.
  BOOLEAN            Tx;            // is it a TX transfer.
  UINT8              *Buffer;       //the application supplied buffer to receive or send data
  XferStateType      XferState;     // state of the transfer
  UINTN              BufferOffset;  //offset in the buffer. the data to be sent/received start in this index
  UINTN              XferSize;      //the size of the data to be sent
} XferType;

// state of USB core
typedef enum DevStateType {
  Detached,
  Attached,
  Default,
  Address,
  Configured,
  Suspended_Detached,
  Suspended_Attached,
  Suspended_Default,
  Suspended_Address,
  Suspended_Configured
} DevStateType;

VOID
UsbQueryBusEvent (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

VOID
IssueXfer (
  IN EFI_USBFN_IO_PROTOCOL   *UsbfnIo,
  IN OUT XferType            *Xfer
  );

VOID
AbortXfer (
  IN EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN UINT8                    EndpointIndex,
  IN BOOLEAN                  Tx
  );

VOID 
HandleSetupPkt(
  IN EFI_USBFN_IO_PROTOCOL  *UsbfnIo,
  EFI_USB_DEVICE_REQUEST    *Req,
  XferType                  *FirstXfer  
  );

VOID
HandleGetDescriptor(
  IN  EFI_USB_DEVICE_REQUEST*  Req, 
  OUT VOID**                   TxSrcBuf,
  OUT UINTN*                   XferSize
  );

VOID
HandleUsbStandardRequest(
  IN EFI_USBFN_IO_PROTOCOL  *UsbfnIo,
  EFI_USB_DEVICE_REQUEST    *Req,
  XferType                  *CtrlXfer
  );

VOID
HandleUsbClassRequest(
  IN EFI_USBFN_IO_PROTOCOL  *UsbfnIo,
  EFI_USB_DEVICE_REQUEST    *Req,
  XferType                  *CtrlXfer
  );

EFI_STATUS
PrepareUsbEnumeration (
  VOID
  );

EFI_STATUS
PrepareUsbMsdUnload(
  VOID
  );

VOID
HandleUsbMsgEndptStatusChangeRx(
  EFI_USBFN_TRANSFER_RESULT *XferResult 
  );

VOID
HandleUsbMsgEndptStatusChangeTx(
  EFI_USBFN_TRANSFER_RESULT *XferResult 
  );

EFI_STATUS
StartEnumeration(
    VOID
  );

VOID
NotifyUsbClassDrvTransferComplete (
  EFI_USBFN_TRANSFER_RESULT *TransferResult
  );

VOID
NotifyUsbClassDrvDeviceConfigured(
  VOID
  );

VOID
NotifyUsbClassDrvDeviceReset(
  VOID
  );

VOID
NotifyUsbEnumerationStart(
  VOID
  );

#endif
