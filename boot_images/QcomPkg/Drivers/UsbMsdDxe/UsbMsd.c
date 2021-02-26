/** @file UsbfnLoopback.c

  Tests for UEFI USB FUNCTION PROTOCOL

  Copyright (c) 2010-2011, 2014-2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port from Boot.xf.1.4
 01/22/16   ck      Support on demand USBFn configuration
 02/05/15   al      Changing TPL_CALLBACK to TPL_CALLBACK
 12/18/14   amitg   Fix KW Errors
 12/08/14   ck      Fix incorrect LunNum after unmount
 10/14/14   ck      Add state machine and remove global vars
 08/14/14   martinl Support for USB mass storage app 
 11/15/11   cching  initial check in for USB MSD support in peripheral

=============================================================================*/

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>      // Allocate* functions
#include <Library/UefiBootServicesTableLib.h> // gBS

#include "UsbMsd.h"        // Header
#include "UsbMsdDesc.h"    // device info
#include "UsbMsdSm.h"      // global State machine
#include "UsbMsdUtility.h" // *_CHK
#include "UsbError.h"      //  USB ASSERTs
#include "UsbMsdXfer.h"    // XFER_TYPE, Issue|Abort Xfer

// ExitBootSerice GUID
extern EFI_GUID gEfiEventExitBootServicesGuid;

/*
* @brief Implement EFI_USB_MSD_START_DEVICE
* @see EFIUsbMsdPeriperahl.h
*/
EFI_STATUS
EFIAPI
UsbMsdStartDevice(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This);


/*
* @brief Implement EFI_USB_MSD_STOP_DEVICE
* @see EFIUsbMsdPeriperahl.h
*/
EFI_STATUS
EFIAPI
UsbMsdStopDevice(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This);

/* 
* @brief Implement EFI_USB_MSD_ASSIGN_BLKIO_HANDLE
* @see EFIUsbMsdPeriperahl.h
*/
EFI_STATUS
EFIAPI
UsbMsdAssignBlkIoHandle(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This,
  IN  EFI_BLOCK_IO_PROTOCOL             *BlkIoHandle,
  IN  UINT32                            LunNum
);


/*
* @brief Implement EFI_USB_MSD_QUERY_MAX_LUN
* @see EFIUsbMsdPeriperahl.h
*/
EFI_STATUS
EFIAPI
UsbMsdQueryMaxLun(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This,
  OUT UINT8                             *NumLunSupported
);


/*
* @brief Implement EFI_USB_MSD_EVENT_HANDLER
* @see EFIUsbMsdPeriperahl.h
*/
EFI_STATUS
EFIAPI
UsbMsdEventHandler(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This
);


/**
* @brief Event notification callback for ExitBootService
*
* @param[in]  Event      Event whose notification function is being invoked
* @param[in]  Context    Pointer to the notification function’s context, which 
*                        is implementation-dependent
*/
VOID
EFIAPI
UsbMsdExitBootService(
  IN EFI_EVENT  Event,
  IN VOID      *Context
);


//
// @brief Create an instance of USBMSD_DEV
//
STATIC
USBMSD_DEV
*CreateUsbMsdInstance (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USBMSD_DEV *UsbMsd = NULL;

  FNC_ENTER_MSG ();

  UsbMsd = (USBMSD_DEV *) AllocateZeroPool(sizeof(USBMSD_DEV));
  MCR_CHK(UsbMsd);

  // USB MSD Peripheral protocol.
  UsbMsd->Signature                        = USBMSD_DEV_SIGNATURE;
  UsbMsd->UsbMsdProtocol.Revision          = EFI_USB_MSD_PERIPHERAL_PROTOCOL_REVISION;
  UsbMsd->UsbMsdProtocol.AssignBlkIoHandle = UsbMsdAssignBlkIoHandle;
  UsbMsd->UsbMsdProtocol.QueryMaxLun       = UsbMsdQueryMaxLun;
  UsbMsd->UsbMsdProtocol.EventHandler      = UsbMsdEventHandler;
  UsbMsd->UsbMsdProtocol.StartDevice       = UsbMsdStartDevice;
  UsbMsd->UsbMsdProtocol.StopDevice        = UsbMsdStopDevice;  

  // Init the LUN mapping
  gBS->SetMem(UsbMsd->UsbMsdMedia.BlkIoLunMap, USB_MSD_MAX_SUPPORTED_LUN * sizeof(USBMSD_LUN_ASSOCIATION), 0);

  // Register for boot services exit event.
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    UsbMsdExitBootService,
    UsbMsd,
    &gEfiEventExitBootServicesGuid,
    &UsbMsd->ExitBootServiceEvent);
  WRN_CHK("unable to create ExitBootServiceEvent with status code", Status);

  UsbMsd->DeviceStarted = FALSE;

  Status = EFI_SUCCESS;

ON_EXIT:

  if (EFI_ERROR(Status)){
    if (UsbMsd){    
      FreePool(UsbMsd);
      UsbMsd = NULL;
    }
  }

  FNC_LEAVE_MSG ();
  return UsbMsd;
}


/**
* @brief The entry point of the module
* @param[in]  ImageHandle         The handle of the image the EfiUsbMsdPeripheralProtocol
                                  will be installed on.
* @param[in]  EFI_SYSTEM_TABLE    EFI System Table 
*
* @retval EFI_SUCCESS Operation succeeded.
* @retval Others      Operation failed.
**/
EFI_STATUS
EFIAPI
UsbMsdDriverEntryPoint (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

  EFI_STATUS Status = EFI_SUCCESS;

  USBMSD_DEV *UsbMsd = NULL;

  FNC_ENTER_MSG();

  ARG_CHK(ImageHandle);

  UsbMsd = CreateUsbMsdInstance();
  MCR_CHK(UsbMsd);  

  // Install USB_MSD_PERIPHERAL_PROTOCOL protocol.
  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gEfiUsbMsdPeripheralProtocolGuid, 
    &UsbMsd->UsbMsdProtocol, 
    NULL);
  ERR_CHK("unable to install USB MSD Peripheral protocols %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
 @brief This is a driver exit point function that is called when
 gBS->ExitBootServices() is called.

 @param  Event                   Pointer to this event
 @param  Context                 Event handler private data

 **/
VOID
EFIAPI
UsbMsdExitBootService (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  USBMSD_DEV *UsbMsd  = NULL;
  EFI_STATUS  Status  = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  ARG_CHK(Context);

  UsbMsd = (USBMSD_DEV *)Context; 

  if(UsbMsd){   
    FreePool(UsbMsd);
    UsbMsd = NULL;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();

}


/* see function description above */
EFI_STATUS
RemoveBlkIoLunAssociation(
  USBMSD_DEV* CONST UsbMsd,
  CONST UINT32      LunNum
  )
{

  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  gBS->SetMem(&UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum], sizeof(USBMSD_LUN_ASSOCIATION), 0);
  UsbMsd->UsbMsdMedia.NumLuns -= 1;

  DBG(EFI_D_INFO, "Free LUN %d, BlkIo %p, Num of active LUN %d",
    LunNum,
    UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].BlkIoHandle,
    UsbMsd->UsbMsdMedia.NumLuns
    );

  FNC_LEAVE_MSG ();
  return Status;
}


/**
* @brief Check whether the association between the LUN and the BlkIoHandle is 
*       valid
*
* @param  UsbMsd                  The USBMSD_DEV instance
* @param  BlkIoHandle             The BlockIo handle to be associated with the Lun
*                                 specified by the LunNum.
* @param  LunNum                  The number of the LUN to be associated with BlkIoHandle
*
* @retval EFI_SUCCESS             The association between BlkIoHandle and LunNum is valid           
* @retval EFI_NOT_READY           The requested LUN is already associated with an BlkIo Handle return 
* @retval EFI_UNSUPPORTED         The BlkIoHandle is already associated with a LUN
**/
EFI_STATUS
CheckBlkIoLunAssociation(
  IN USBMSD_DEV*  CONST                 UsbMsd,
  IN EFI_BLOCK_IO_PROTOCOL CONST* CONST BlkIoHandle,
  IN CONST UINT32                       LunNum
  )
{

  UINTN      Index  = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  ARG_CHK(UsbMsd && BlkIoHandle);

  for(Index = 0; Index < USB_MSD_MAX_SUPPORTED_LUN; Index++) {

    if(UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].BlkIoHandle == BlkIoHandle){
      Status = EFI_UNSUPPORTED;
      ERR_CHK(
        "The BlkIoHanlde 0x%x is currently associated with LUN %d",
        BlkIoHandle,
        UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].Lun.Index
      );
    }

    if(LunNum == UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].Lun.Index &&
       UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].BlkIoHandle &&
       BlkIoHandle != UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].BlkIoHandle){
      Status = EFI_NOT_READY;
      ERR_CHK("The LUN %d is currently associated with BlkIoHanlde 0x%x",
        LunNum,
        UsbMsd->UsbMsdMedia.BlkIoLunMap[Index].BlkIoHandle
      );
    }
  }

  ON_EXIT:

  FNC_LEAVE_MSG ();
  return Status;
}


/* see function description above */
EFI_STATUS
EFIAPI
UsbMsdQueryMaxLun (
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This,
  OUT UINT8                             *NumLunSupported
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();
  ARG_CHK(This && NumLunSupported);
  *NumLunSupported = USB_MSD_MAX_SUPPORTED_LUN;
  FNC_LEAVE_MSG();
ON_EXIT:
  return Status;
}

/* see function description above */
EFI_STATUS
EFIAPI
UsbMsdAssignBlkIoHandle (
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This,
  IN  EFI_BLOCK_IO_PROTOCOL             *BlkIoHandle,
  IN  UINT32                             LunNum
  )
{
  USBMSD_DEV *UsbMsd  = NULL;
  EFI_STATUS  Status  = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  // When the client is going to unmount a LUN, BlkIoHandle is nULL. 
  // Thus, only This param cannot be NULL.
  ARG_CHK(This);

  UsbMsd = USBMSD_FROM_THIS(This);

  //
  // The device can support only up to MAX_SUPPORTED_LUN - 1
  //
  if(USB_MSD_MAX_SUPPORTED_LUN <= LunNum) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  //
  // Remove association
  //
  if(!BlkIoHandle) {
    Status = RemoveBlkIoLunAssociation(UsbMsd, LunNum);
    goto ON_EXIT;
  }

  //
  // Check whether the association can be supported
  //
  Status = CheckBlkIoLunAssociation(UsbMsd, BlkIoHandle, LunNum);
  if(EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  //
  // Now associate the BlkIo to LUN
  //
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].BlkIoHandle                 = BlkIoHandle;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.Index                   = LunNum;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.Property.Enabled        = TRUE;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.Property.Removable      = BlkIoHandle->Media->RemovableMedia;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.BlockSize               = BlkIoHandle->Media->BlockSize;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.NumBlocks               = BlkIoHandle->Media->LastBlock+1;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.Property.ReadOnly       = BlkIoHandle->Media->ReadOnly;
  UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].Lun.Property.NeedAttention  = FALSE;
  UsbMsd->UsbMsdMedia.NumLuns += 1;

  DBG(EFI_D_INFO, "Add LUN %d, BlkIo %p, Num of active LUN %d",
    LunNum,
    UsbMsd->UsbMsdMedia.BlkIoLunMap[LunNum].BlkIoHandle,
    UsbMsd->UsbMsdMedia.NumLuns
    );

  ON_EXIT:

  FNC_LEAVE_MSG();
  return Status;
}


/* see function description above */
EFI_STATUS
EFIAPI
UsbMsdEventHandler(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This)
{
  EFI_STATUS                 Status      = EFI_SUCCESS;
  EFI_USBFN_IO_PROTOCOL     *UsbfnIo     = NULL;
  EFI_USBFN_MESSAGE          Msg         = EfiUsbMsgNone;
  EFI_USBFN_MESSAGE_PAYLOAD  Payload;
  UINTN                      PayloadSize = sizeof (Payload);
  USBMSD_DEV                *UsbMsd      = NULL;

  ARG_CHK(This);

  UsbMsd  = USBMSD_FROM_THIS(This);
  ARG_CHK(UsbMsd);
  UsbfnIo = UsbMsd->UsbfnIoProtocol;  

  if (!UsbfnIo) {
    Status = EFI_PROTOCOL_ERROR;
    goto ON_EXIT;
  }

  // Always check for new transfers or poll the controller. ------------------
  Status = UsbfnIo->EventHandler(
    UsbfnIo,
    &Msg,
    &PayloadSize,
    &Payload
    );

  // Pass meaningful events to state machine
  if (Msg != EfiUsbMsgNone) {
    Status = DevSmProcessEvent(&UsbMsd->StateMachine, Msg, PayloadSize, &Payload);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "State machine failed to process event: %r", Status);      
    }

    // abort all of the xfers when disconnected.
    if (Msg == EfiUsbMsgBusEventDetach) {
      Status = EFI_NOT_READY;
      
      // move xfer to free state
      if (NULL != UsbMsd->StateMachine.CtrlXfer.Buffer && UsbMsd->UsbfnIoProtocol) {
        AbortXfer(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.CtrlXfer);
      }

      if (NULL != UsbMsd->StateMachine.BulkRxXfer.Buffer && UsbMsd->UsbfnIoProtocol) {
        AbortXfer(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkRxXfer);
      }

      if (NULL != UsbMsd->StateMachine.BulkTxXfer.Buffer && UsbMsd->UsbfnIoProtocol) {
        AbortXfer(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.BulkTxXfer);
      }

      if (NULL != UsbMsd->StateMachine.CBW.Buffer && UsbMsd->UsbfnIoProtocol) {
        AbortXfer(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.CBW);
      }

      if (NULL != UsbMsd->StateMachine.CSW.Buffer && UsbMsd->UsbfnIoProtocol) {
        AbortXfer(UsbMsd->UsbfnIoProtocol, &UsbMsd->StateMachine.CSW);
      }
    }
  } 

ON_EXIT:
  return Status;
}


/* see function description above */
EFI_STATUS
EFIAPI
UsbMsdStartDevice(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  USBMSD_DEV     *UsbMsd = NULL;

  FNC_ENTER_MSG();

  ARG_CHK(This);

  UsbMsd  = USBMSD_FROM_THIS(This);
  ARG_CHK(UsbMsd);

  if (UsbMsd && UsbMsd->DeviceStarted) {
    Status = EFI_NOT_READY;
    ERR_CHK("Usb MSD device is already started");
  }

  UsbMsd->DeviceStarted = TRUE;
  
  // Locate the USB FN IO protocol.
  Status = gBS->LocateProtocol(
    &gEfiUsbfnIoProtocolGuid,
    NULL,
    (VOID **)&UsbMsd->UsbfnIoProtocol
    );
  ERR_CHK("Failed to located USB FN IO protocol: %r", Status);

  // Init State Machine
  Status = DevSmInitialize(&UsbMsd->StateMachine, UsbMsd->UsbfnIoProtocol);
  ERR_CHK("Usb MSD state machine initialization failed %r", Status);

  // Starts USB enumeration
  Status = UsbMsd->UsbfnIoProtocol->ConfigureEnableEndpoints(UsbMsd->UsbfnIoProtocol, (EFI_USB_DEVICE_INFO *)&UsbMsdDeviceInfo);
  ERR_CHK("Failed to configure and enable endpoints %r", Status); 

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* see function description above */
EFI_STATUS
EFIAPI
UsbMsdStopDevice(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL   *This)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  USBMSD_DEV     *UsbMsd = NULL;
  EFI_USBFN_IO_PROTOCOL  *UsbfnIo = NULL;

  FNC_ENTER_MSG();

  ARG_CHK(This);

  UsbMsd  = USBMSD_FROM_THIS(This);
  ARG_CHK(UsbMsd);
  UsbfnIo = UsbMsd->UsbfnIoProtocol;  

  if (UsbMsd && !UsbMsd->DeviceStarted) {
    Status = EFI_NOT_READY;
    ERR_CHK("Usb MSD device is already stopped");
  }

  UsbMsd->DeviceStarted = FALSE;
  
  if (!UsbfnIo) {
    Status = EFI_PROTOCOL_ERROR;
    goto ON_EXIT;
  }

  // Stop the controller
  Status = UsbfnIo->StopController(UsbfnIo);
  ERR_CHK("Failed to stop controller %r", Status);

  // clear up state machine
  DevSmCleanup(&UsbMsd->StateMachine);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;  
}