/**
 * @file UsbfnDwc3.c
 *
 * @brief USB Function Driver which provides the UsbfnIo Protocol
 *
 * Copyright (c) 2011,2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Portions Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
 * This program and the accompanying materials
 * are licensed and made available under the terms and conditions of the BSD License
 * which accompanies this distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17	pm		Port from Boot.xf.1.4
 06/20/16   ck      Add support to put the PHY in and out of the Non-Driving mode
 05/09/16   ck      Move product and manufacturer string values to PCD
 05/03/16   ck      Don't fail binding stop with non-critical errors
 02/23/16   ck      Remove the unused protocol
 01/22/16   ck      Port the USB SetConfig fix
 10/26/15   ck      Assume cable connection for pre-si platforms
 10/15/15   ck      Update debug message level
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 03/25/15   vsb     USB LPM, HS/SS PHY config changes, USB mode toggle, clamp UsbFn to HS if SS PHY failed
 01/20/15   ck      Remove deprecated functions
 11/20/14   ml      Migrate to official SuperSpeed support
 12/18/14   ck      Address software & hardware transfer cancel notification issue
 11/19/14   ck      Add toggle USB mode support
 10/07/14   wufan   Add support of HS Test Mode for compliance testing
 08/13/14   amitg   Clean Up Code
 06/06/14   ck      Add note for Boot Variable retesting
 05/20/14   ar      Add PCD value to override max bus speed
 04/30/14   ar      Cleanup for NDEBUG build
 04/25/14   amitg   Charger and Cable Detection Updates for MSM 8994 (Elessar)
 03/14/14   amitg   Updates for 8994
 08/23/13   mliss   Cleanup and stabilization
 01/20/11   cching  Initial rework for Synopsys Usbfn driver
 =============================================================================*/

//
// Includes
//
#include <Protocol/EFICardInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/PrintLib.h>

#include "UsbfnDwc3.h"
#include "UsbfnDwc3Ch9.h"
#include "UsbfnDwc3Impl.h"
#include "UsbfnDwc3Util.h"
#include "DeviceInfo.h"
#include "HALdwcHWIO.h"
#include "UsbSharedLib.h"

extern EFI_GUID gQcomTokenSpaceGuid;
extern EFI_GUID gEfiEventEnterLPMGuid;

/*******************************************************************************
 * Static Variables
 ******************************************************************************/

STATIC
USBFN_DEV
*Usbfn =  NULL;

#ifndef MDEPKG_NDEBUG
STATIC
CONST
CHAR8
*MsgToStr[] =
{
  "EfiUsbMsgNone",
  "EfiUsbMsgSetupPacket",
  "EfiUsbMsgEndpointStatusChangedRx",
  "EfiUsbMsgEndpointStatusChangedTx",
  "EfiUsbMsgBusEventDetach",
  "EfiUsbMsgBusEventAttach",
  "EfiUsbMsgBusEventReset",
  "EfiUsbMsgBusEventSuspend",
  "EfiUsbMsgBusEventResume",
  "EfiUsbMsgBusEventSpeed",
};
#endif // MDEPKG_NDEBUG

#define QUSB_SERIAL_NUMBER_LEN 256

/**
 * Array to define the maximum packet size of an endpoint on the basis of
 * bus speed and endpoint type. Used in place of hard coded logic throughout.
 * The specific values are defined in the USB 2.0 and USB 3.0 specifications.
 * This array is indexed by the EFI_USB_ENDPOINT_TYPE and EFI_USB_BUS_SPEED
 * enums and thus must have its values defined in the order of these enums.
 *
 * Example: UsbEpMaxPacketSize[UsbEndpointBulk][UsbBusSpeedHigh] == 512
 */
CONST
UINTN
UsbEpMaxPacketSize[USBFN_NUM_EP_TYPES][USBFN_NUM_BUS_SPEEDS] = {
// unknown, low,    full,   high,   super
  {0,       8,      64,     64,     512},   // control
  {0,       0,      1023,   1024,   1024},  // isoch
  {0,       0,      64,     512,    1024},  // bulk
  {0,       8,      64,     1024,   1024}   // interrupt
};


EFI_DRIVER_BINDING_PROTOCOL
gUsbFnDriverBinding = {
  UsbFnDriverBindingSupported,
  UsbFnDriverBindingStart,
  UsbFnDriverBindingStop,
  0x30,
  NULL,
  NULL
};


/*******************************************************************************
 * Initialization / Shutdown
 ******************************************************************************/


/**
 * @brief Driver exit function that is called on gBS->ExitBootServices()
 *
 * Disconnects from host if a connection is still active and frees memory.
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event hanlder private data
 */
VOID
EFIAPI
UsbfnExitBootService (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  USBFN_DEV   *Usbfn = NULL;
  UINT8       Ep, EpDir;

  FNC_ENTER_MSG ();

  if (NULL == Context) {
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = (USBFN_DEV *) Context;

  // Deinit controller and cancel pending URBs
  UsbfnDwcCoreShutdown(Usbfn);

  // Free the UrbPool and setup packet
  UsbfnDwcFreeXferRsc(Usbfn);

  // Free common layer buffers
  for (Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
    for (EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++) {
      if (NULL != Usbfn->TrbBuf[Ep][EpDir]) {
        UncachedFreeAlignedPool(Usbfn->TrbBuf[Ep][EpDir]);
      }
    }
  }
  if (NULL != Usbfn->EvtBuffer) {
    UncachedFreeAlignedPool(Usbfn->EvtBuffer);
  }

  // Free descriptors
  if (Usbfn->HijackData.RequestHijackingEnabled) {
    if (Usbfn->SSDeviceInfo) {
      FreeDeviceInfo(Usbfn->SSDeviceInfo);
    }
  }

  // Free hijacking buffers
  if (Usbfn->HijackData.HijackDataBuffer) {
    FreePool(Usbfn->HijackData.HijackDataBuffer);
  }
  if (Usbfn->HijackData.HijackStatusBuffer) {
    FreePool(Usbfn->HijackData.HijackStatusBuffer);
  }

  // Close the ExitBootServices event and free the device
  if (Usbfn->ExitBootServiceEvent) {
    gBS->CloseEvent(Usbfn->ExitBootServiceEvent);
    Usbfn->ExitBootServiceEvent = NULL;
  }

  FreePool(Usbfn);

ON_EXIT:
  FNC_LEAVE_MSG ();
}


/**
 * @brief Callback for the EFI_LOADED_IMAGE_PROTOCOL
 *
 * Calls ExitBootServices handler to clean up.
 *
 * @param [in]  ImageHandle     Handle to driver image
 *
 * @retval EFI_SUCCESS          Successfully shut down
 * @retval Others               Failed to shut down properly
 */
EFI_STATUS
UsbfnUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();

  UsbfnExitBootService(0, Usbfn);

  FNC_LEAVE_MSG ();
  return Status;
}


/**
 * @brief Initialize endpoint transfer lists
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
STATIC
VOID
UsbfnDwcInitXferLists (
  IN USBFN_DEV                  *Usbfn
  )
{
  UINT8 Ep, EpDir;

  for(Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++){
      for(EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++){
          InitializeListHead(&(Usbfn->ActiveTransfers)[Ep][EpDir]);
          InitializeListHead(&(Usbfn->PendingXfer)[Ep][EpDir]);
      }
  }
}


/**
 * @brief Initialize software transfer resources
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          Resources initialized successfully
 * @retval others               Failed to queue transfer
 */
STATIC
EFI_STATUS
UsbfnDwcInitXferRsc (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8      i;

  FNC_ENTER_MSG();

  //
  // Use SetupPkt to determine whether the transfer resources
  // need to be initialized, assume all or nothing
  //
  if (NULL == Usbfn->SetupPkt) {
    Usbfn->SetupPkt = UncachedAllocatePool(8);
    if (NULL == Usbfn->SetupPkt) {
      Status = EFI_OUT_OF_RESOURCES;
      DBG(EFI_D_ERROR, "out of memory");
      goto ON_EXIT;
    }

    // Init Urb pool
    InitializeListHead(&Usbfn->UrbPool);
    Usbfn->UrbPoolSize = 0;
    for(i = 0; i < USBFN_ENDPOINT_NUMBER_MAX * 2; i++){
      DWC_URB_NODE *UrbNode = AllocatePool(sizeof(DWC_URB_NODE));
      if (NULL == UrbNode) {
        Status = EFI_OUT_OF_RESOURCES;
        DBG(EFI_D_ERROR, "out of memory");
        goto ON_EXIT;
      }
      UrbNode->SgList = AllocatePool(sizeof(SCATTER_GATHER_LIST));
      if (NULL == UrbNode->SgList) {
        Status = EFI_OUT_OF_RESOURCES;
        DBG(EFI_D_ERROR, "out of memory");
        FreePool(UrbNode);
        goto ON_EXIT;
      }
      InsertTailList(&Usbfn->UrbPool, &UrbNode->Link);
      Usbfn->UrbPoolSize++;
    }
    DBG(EFI_D_INFO,"Total Urb Node in UrbPool at beginning %d",Usbfn->UrbPoolSize);

    // Init Active Transfer List and Init Ctrl Xfer Pending List
    UsbfnDwcInitXferLists(Usbfn);
  }

ON_EXIT:
  if (EFI_ERROR(Status)) {
    UsbfnDwcFreeXferRsc(Usbfn);
  }
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Create and initialize a USBFN_DEV instance
 *
 * @retval New instance on success
 * @retval NULL on failure
 */
STATIC
USBFN_DEV *
CreateUsbfnInstance (
  VOID
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  USBFN_DEV               *Usbfn = NULL;
  UINTN                   i = 0;
  EFI_USBFN_MESSAGE_NODE  *NewMsg;
  UINT8                   UefiVar;
  UINTN                   UefiVarSize;
  EFI_PLATFORMINFO_PROTOCOL *PlatformInfoProtocol;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;

  FNC_ENTER_MSG ();

  // Allocate the software device structure
  Usbfn = (USBFN_DEV *) AllocateZeroPool(sizeof(USBFN_DEV));
  if (!Usbfn) {
    DBG (EFI_D_ERROR, "memory allocation failed for USB FN");
    goto ON_EXIT;
  }

  Usbfn->Signature = USBFN_DEV_SIGNATURE;

  // USBFN_IO protocol.
  Usbfn->UsbfnIo.Revision                   = EFI_USBFN_IO_PROTOCOL_REVISION;
  Usbfn->UsbfnIo.DetectPort                 = UsbfnDetectPort;
  Usbfn->UsbfnIo.ConfigureEnableEndpoints   = UsbfnConfigureEnableEndpoints;
  Usbfn->UsbfnIo.GetEndpointMaxPacketSize   = UsbfnGetEndpointMaxPacketSize;
  Usbfn->UsbfnIo.GetDeviceInfo              = UsbfnGetDeviceInfo;
  Usbfn->UsbfnIo.GetVendorIdProductId       = UsbfnGetVendorIdProductId;
  Usbfn->UsbfnIo.AbortTransfer              = UsbfnAbortTransfer;
  Usbfn->UsbfnIo.GetEndpointStallState      = UsbfnGetEndpointStallState;
  Usbfn->UsbfnIo.SetEndpointStallState      = UsbfnSetEndpointStallState;
  Usbfn->UsbfnIo.EventHandler               = UsbfnEventHandler;
  Usbfn->UsbfnIo.Transfer                   = UsbfnTransfer;
  Usbfn->UsbfnIo.GetMaxTransferSize         = UsbfnGetMaxTransferSize;
  Usbfn->UsbfnIo.AllocateTransferBuffer     = UsbfnAllocateTransferBuffer;
  Usbfn->UsbfnIo.FreeTransferBuffer         = UsbfnFreeTransferBuffer;
  Usbfn->UsbfnIo.StartController            = UsbfnStartController;
  Usbfn->UsbfnIo.StopController             = UsbfnStopController;
  Usbfn->UsbfnIo.SetEndpointPolicy          = UsbfnSetEndpointPolicy;
  Usbfn->UsbfnIo.GetEndpointPolicy          = UsbfnGetEndpointPolicy;
  Usbfn->UsbfnIo.ConfigureEnableEndpointsEx = UsbfnConfigureEnableEndpointsEx;

  Usbfn->UsbConfigProtocol = NULL;

  // Set default pipe policy
  Usbfn->PipePolicy.ZeroLengthTerminationSupport = TRUE;
  for(i = 0; i < USBFN_ENDPOINT_NUMBER_MAX; ++i){
    Usbfn->PipePolicy.ZLT[i][EfiUsbEndpointDirectionDeviceTx] = FALSE;
    Usbfn->PipePolicy.ZLT[i][EfiUsbEndpointDirectionDeviceRx] = FALSE;
    Usbfn->PipePolicy.MaxTransferSize[i][EfiUsbEndpointDirectionDeviceTx] = USBFN_MAX_TRANSFER_SIZE;
    Usbfn->PipePolicy.MaxTransferSize[i][EfiUsbEndpointDirectionDeviceRx] = USBFN_MAX_TRANSFER_SIZE;
  }

  // Initialize device state
  Usbfn->CoreId          = 0;
  Usbfn->Enabled         = FALSE;
  Usbfn->IsAttached      = FALSE;
  Usbfn->IsSuspended     = FALSE;
  Usbfn->Speed           = UsbBusSpeedUnknown;
  Usbfn->Address         = 0;
  Usbfn->ConfigValue     = 0;
  Usbfn->MaxCurrent      = 0;
  Usbfn->ChgPortType     = DCI_DWC_CHG_PORT_UNKNOWN;
  Usbfn->Disconnecting   = FALSE;
  Usbfn->HsTestMode      = USB_TEST_DISABLE;
  Usbfn->StoreSELData    = FALSE;
  Usbfn->MaxSpeed        = DWC_DEV_SPEED_DEFAULT;

  // This will be set to FALSE when a ConnectDone event arrives. It
  // must be TRUE until then because a Disconnect event will not be
  // posted unless the connect happens and we would wait forever.
  Usbfn->DisconnectEvtPosted = TRUE;


  // Initialize the client messages
  InitializeListHead(&Usbfn->UsbfnMessages);
  InitializeListHead(&Usbfn->UsbfnMessagePool);

  for (i = 0; i < USBFN_MESSAGE_NODE_COUNT; ++i) {
    NewMsg = (EFI_USBFN_MESSAGE_NODE *)AllocateZeroPool(sizeof(EFI_USBFN_MESSAGE_NODE));
    InsertTailList(&Usbfn->UsbfnMessagePool, &NewMsg->Link);
  }

  // Fetch the platform info
  Status = gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid, NULL,
      (VOID **) &PlatformInfoProtocol);
  ERR_CHK ("Failed to get gEfiPlatformInfoProtocolGuid");

  Status = PlatformInfoProtocol->GetPlatformInfo(PlatformInfoProtocol,
      &PlatformInfo);
  ERR_CHK ("Failed to get PlatformInfo");

  Usbfn->PlatType = PlatformInfo;

  Usbfn->SetupPkt     = NULL;

  // Init SW resources associated with USB transfers
  Status = UsbfnDwcInitXferRsc(Usbfn);
  ERR_CHK("Failed to initialize software resources");

  // Read UEFI variable to check for request hijacking
  UefiVarSize = sizeof(UefiVar);
  Status = gRT->GetVariable(L"UsbfnHijackRequests", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
  if (EFI_ERROR(Status)) {
    UefiVar = 0;
  }

  // Need to retest boot variable setting.
  if (UefiVar) {
    DBG(EFI_D_WARN, "Request hijacking enabled");
    Usbfn->HijackData.RequestHijackingEnabled = TRUE;
  } else {
    Usbfn->HijackData.RequestHijackingEnabled = FALSE;
  }
  Usbfn->HijackData.HijackThisRequest    = FALSE;
  Usbfn->HijackData.HijackDescType       = USB_DESC_TYPE_UNDEFINED;
  Usbfn->HijackData.HijackedBuffer       = NULL;
  Usbfn->HijackData.HijackedBufferLength = 0;
  Usbfn->HijackData.HijackNextRxEvent    = FALSE;
  Usbfn->HijackData.HijackNextTxEvent    = FALSE;
  Usbfn->HijackData.HijackDataBuffer     = AllocateZeroPool(512); // max pkt size for ep 0
  Usbfn->HijackData.HijackStatusBuffer   = AllocateZeroPool(512);

  Status = EFI_SUCCESS;

ON_EXIT:
  if (EFI_ERROR (Status)) {
    Usbfn = NULL;
  }

  FNC_LEAVE_MSG ();
  return Usbfn;
}


/**
  Entry point for EFI drivers.

  @param  ImageHandle       EFI_HANDLE.
  @param  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
          EFI_DEVICE_ERROR  Fail.

**/
EFI_STATUS
EFIAPI
UsbfnDwc3DriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gUsbFnDriverBinding,
           ImageHandle,
           NULL,
           NULL
           );
}



/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has UsbConfigProtocol installed will
  be supported.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          This driver supports this device.
  @return EFI_UNSUPPORTED      This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{

  QCOM_USB_CONFIG_PROTOCOL *pUsbConfigProtocol;
  EFI_STATUS              Status;

  // Check if UsbConfig protocol is installed
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &pUsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status))
  {
    return Status;
  }

  //check if controller is for a valid core that supports device USB2.0 mode
  if ( (pUsbConfigProtocol->coreNum < USB_CORE_MAX_NUM) && (pUsbConfigProtocol->modeType == USB_DEVICE_MODE_SS) )
  {
    DBG(EFI_D_INFO,
        "UsbFnDriverBindingSupported: This is core#%d, CoreType = (0x%x)",
        pUsbConfigProtocol->coreNum,
        pUsbConfigProtocol->modeType );
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = EFI_UNSUPPORTED;
  }

  // Close the USB_CONFIG used to perform the supported test
  gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}


EFI_STATUS
EFIAPI
UsbFnDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  Usbfn = CreateUsbfnInstance();
  if (!Usbfn) {
    Status = EFI_OUT_OF_RESOURCES;
    DBG (EFI_D_ERROR, "unable to create USB FN");
    goto ON_EXIT;
  }

  //Open the Plat Config protocol.
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &Usbfn->UsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    DBG(EFI_D_ERROR,
        "UsbFnDriverBindingStart: Unable to open USB Config Protocol, Status = %r",
        Status);
    return Status;
  }

  DBG(EFI_D_INFO,
      "UsbFnDriverBindingStart: This is core#%d, CoreType = (0x%x)",
      Usbfn->UsbConfigProtocol->coreNum,
      Usbfn->UsbConfigProtocol->modeType);

  //save core type
  Status = Usbfn->UsbConfigProtocol->GetSSUsbFnConfig(Usbfn->UsbConfigProtocol,
      &Usbfn->CoreType);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR,"Usbfn->Unable to get SSUSB FN config\r\n");
    goto ON_EXIT;
  }

  // Get base address
  Status = Usbfn->UsbConfigProtocol->GetCoreBaseAddr(Usbfn->UsbConfigProtocol,
      Usbfn->CoreType, &Usbfn->Usb30WrapperBase);
  ERR_CHK("failed to get USB core base address");
  Usbfn->DwcUsb3RegBase = Usbfn->Usb30WrapperBase;
  Usbfn->Usb30Qscratch  = Usbfn->Usb30WrapperBase + USB30_QSCRATCH_REG_BASE_OFFS;
  DBG(EFI_D_INFO,"Usbfn->Usb30WrapperBase = 0x%x\r\n",Usbfn->Usb30WrapperBase);

  //initialize USB core
  Status = Usbfn->UsbConfigProtocol->ConfigUsb(Usbfn->UsbConfigProtocol, USB_DEVICE_MODE_SS, Usbfn->UsbConfigProtocol->coreNum);
  ERR_CHK ("failed to perform platform configuration");

  // Install UsbfnIo protocol.
  Status = gBS->InstallMultipleProtocolInterfaces(&Controller,
      &gEfiUsbfnIoProtocolGuid, &Usbfn->UsbfnIo, NULL);
  ERR_CHK ("unable to install USBFN protocols");

  // Register for boot services exit event.
  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
      UsbfnExitBootService, Usbfn, &gEfiEventExitBootServicesGuid,
      &Usbfn->ExitBootServiceEvent);

  if (EFI_ERROR (Status)) {
    DBG(EFI_D_ERROR,"UsbFnDriverBindingStart: ExitBootServiceEvent creation failed %r", Status);
    Usbfn->ExitBootServiceEvent = NULL;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



EFI_STATUS
EFIAPI
UsbFnDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_USBFN_IO_PROTOCOL *UsbfnIo;

  FNC_ENTER_MSG ();

  //
  // Test whether the Controller handler passed in is a valid
  // Usb controller handle that should be supported, if not,
  // return the error status directly
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbfnIoProtocolGuid,
                  (VOID **) &UsbfnIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiUsbfnIoProtocolGuid,
                  UsbfnIo
                  );

  if (EFI_ERROR (Status)) {
    WRN_CHK("UninstallProtocolInterface: UninstallProtocolInterface failed %r", Status);
  }

  //stop all xfers and stop controller
  UsbfnExitBootService(0, Usbfn);

  //close UsbConfig handle on controller DriverBinding handle
  Status = gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    WRN_CHK("CloseProtocol: gQcomUsbConfigProtocolGuid failed %r", Status);
  }

  // Due to a known bug, closing DriverBindingHandle will fail with
  // EFI_NOT_FOUNC. Ignore the error so the usbfn can be stopped.
  Status = EFI_SUCCESS;
  Usbfn  = NULL;

  FNC_LEAVE_MSG ();
  return Status;
}


/*******************************************************************************
 * Generate UsbFnIO protocol messages for the client
 ******************************************************************************/


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnSetupDevCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_USBFN_MESSAGE_NODE  *NewMsg = NULL;
  LIST_ENTRY              *ListEntry = NULL;
  UINT8                   DescType;
  VOID                    *Buffer;
  UINTN                   BufferSize;

  FNC_ENTER_MSG ();

  USB_ASSERT_RETURN((Usbfn && Req), EFI_INVALID_PARAMETER);

  if (Usbfn->Disconnecting) {
    DBG(EFI_D_INFO, "Disconnecting, not sending client notification");
    goto ON_EXIT;
  }

  // handle request hijacking
  if ( Usbfn->HsTestMode != USB_TEST_DISABLE ||
       ( Usbfn->HijackData.RequestHijackingEnabled &&
         Usbfn->Speed == UsbBusSpeedSuper) ) {

    // hijack descriptors
    if (Req->Request == USB_REQ_GET_DESCRIPTOR) {
      DescType = Req->Value >> 8 & 0xFF;

      // hijack device and config descriptors on return trip from client
      if (DescType == USB_DESC_TYPE_DEVICE ||
          DescType == USB_DESC_TYPE_CONFIG) {
        Usbfn->HijackData.HijackThisRequest = TRUE;
        Usbfn->HijackData.HijackDescType = DescType;
      }

      // don't let client know about BOS descriptor request
      if (DescType == USB_DESC_TYPE_BOS) {
        DBG(EFI_D_WARN, "Hijacking BOS Descriptor");

        // queue BOS data stage
        Buffer     = Usbfn->SSDeviceInfo->BosDescriptor;
        BufferSize = Usbfn->SSDeviceInfo->BosDescriptor->TotalLength;

        Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_TX, BufferSize, Buffer, NULL);
        ERR_CHK("failed to queue BOS descriptor data stage");

        // queue BOS status stage
        Buffer     = Usbfn->HijackData.HijackStatusBuffer;
        BufferSize = 0;

        Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_RX, BufferSize, Buffer, NULL);
        ERR_CHK("failed to queue BOS descriptor status stage");

        // hijack the completion events
        Usbfn->HijackData.HijackNextRxEvent = TRUE;
        Usbfn->HijackData.HijackNextTxEvent = TRUE;
        goto ON_EXIT;
      }
    }

    // hijack SET_ISOCH_DELAY
    if (Req->Request == USB_REQ_SET_ISOCH_DELAY) {
      DBG(EFI_D_WARN, "Hijacking SET_ISOCH_DELAY request");

      // queue status stage
      Buffer     = Usbfn->HijackData.HijackStatusBuffer;
      BufferSize = 0;

      Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_TX, BufferSize, Buffer, NULL);
      ERR_CHK("failed to queue SET_ISOCH_DELAY status stage");

      // hijack the completion events
      Usbfn->HijackData.HijackNextTxEvent = TRUE;
      goto ON_EXIT;
    }

    // hijack SET_SEL
    if (Req->Request == USB_REQ_SET_SEL) {
      DBG(EFI_D_WARN, "Hijacking SET_SEL request");

      // queue data to recieve SEL values
      Buffer     = Usbfn->HijackData.HijackDataBuffer;
      BufferSize = 6;

      Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_RX, BufferSize, Buffer, NULL);
      ERR_CHK("failed to queue SET_SEL data stage");

      // queue status stage
      Buffer     = Usbfn->HijackData.HijackStatusBuffer;
      BufferSize = 0;

      Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_TX, BufferSize, Buffer, NULL);
      ERR_CHK("failed to queue SET_SEL status stage");

      // hijack the completion events
      Usbfn->HijackData.HijackNextRxEvent = TRUE;
      Usbfn->HijackData.HijackNextTxEvent = TRUE;
      goto ON_EXIT;
    }

    // hijack SET_FEATURE (SS only features)
    if (Req->Request == USB_REQ_SET_FEATURE) {
      switch (Req->Value) {
        case USB_FEATURE_U1_ENABLE:
        case USB_FEATURE_U2_ENABLE:
        case USB_FEATURE_LTM_ENABLE:
        case USB_FEATURE_HS_TEST_MODE:

          DBG(EFI_D_WARN, "Hijacking SET_FEATURE 0x%x request", Req->Value);

          // queue status stage
          Buffer     = Usbfn->HijackData.HijackStatusBuffer;
          BufferSize = 0;

          Status = UsbfnDwcTransfer(Usbfn, 0, DWC_EP_DIR_TX, BufferSize, Buffer, NULL);
          ERR_CHK("failed to queue SET_FEATURE status stage");

          // hijack the completion event
          Usbfn->HijackData.HijackNextTxEvent = TRUE;
          goto ON_EXIT;

        default:
          DBG(EFI_D_INFO, "NOT Hijacking SET_FEATURE 0x%x request", Req->Value);
          break;
      }
    }
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&Usbfn->UsbfnMessagePool);
  USB_ASSERT_RETURN(ListEntry, EFI_DEVICE_ERROR);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgSetupPacket;
  CopyMem (&(NewMsg->Data.SetupCbRequest), Req, sizeof (NewMsg->Data.SetupCbRequest));
  InsertTailList(&Usbfn->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnSetupIfcCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT_RETURN((Usbfn && Req), EFI_INVALID_PARAMETER);

  if (Usbfn->Disconnecting) {
    DBG(EFI_D_INFO, "Disconnecting, not sending client notification");
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&Usbfn->UsbfnMessagePool);
  USB_ASSERT_RETURN(ListEntry, EFI_DEVICE_ERROR);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgSetupPacket;
  CopyMem (&(NewMsg->Data.SetupCbRequest), Req, sizeof (NewMsg->Data.SetupCbRequest));
  InsertTailList(&Usbfn->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
UsbfnXtachCb (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsAttached
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT(Usbfn);

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&Usbfn->UsbfnMessagePool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = IsAttached
                  ? EfiUsbMsgBusEventAttach
                  : EfiUsbMsgBusEventDetach;
  InsertTailList(&Usbfn->UsbfnMessages, &NewMsg->Link);

  FNC_LEAVE_MSG ();
}


/*
 * See header for documentation.
 */
VOID
UsbfnBusEventCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USBFN_MESSAGE          Message
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (Usbfn);

  if (Usbfn->Disconnecting) {
    DBG(EFI_D_INFO, "Disconnecting, not sending client notification");
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&Usbfn->UsbfnMessagePool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = Message;
  InsertTailList(&Usbfn->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
}


/*
 * See header for documentation.
 */
VOID
UsbfnSpeedCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_BUS_SPEED          Speed
  )
{

  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (Usbfn);

  if (Usbfn->Disconnecting) {
    DBG(EFI_D_INFO, "Disconnecting, not sending client notification");
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&Usbfn->UsbfnMessagePool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);


  NewMsg->Message = EfiUsbMsgBusEventSpeed;
  NewMsg->Data.SpeedCbSpeed = Speed;
  InsertTailList(&Usbfn->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
}


/*******************************************************************************
 * Protocol Implementation
 ******************************************************************************/


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnDetectPort (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT EFI_USBFN_PORT_TYPE    *PortType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == PortType) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  if (!Usbfn->IsAttached) {
    Status = EFI_NOT_READY;
    *PortType = EfiUsbUnknownPort;
    goto ON_EXIT;
  }

  // Convert to USBFN_IO port type
  switch (Usbfn->ChgPortType) {
    case DCI_DWC_CHG_PORT_SDP:
      *PortType = EfiUsbStandardDownstreamPort;
      break;
    case DCI_DWC_CHG_PORT_CDP:
      *PortType = EfiUsbChargingDownstreamPort;
      break;
    case DCI_DWC_CHG_PORT_DCP:
      *PortType = EfiUsbDedicatedChargingPort;
      break;
    case DCI_DWC_CHG_PORT_INVALID:
      *PortType = EfiUsbInvalidDedicatedChargingPort;
      break;
    case DCI_DWC_CHG_PORT_UNKNOWN:
    default:
      Status = EFI_DEVICE_ERROR;
      *PortType = EfiUsbUnknownPort;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpoints (
  IN EFI_USBFN_IO_PROTOCOL      *This,
  IN EFI_USB_DEVICE_INFO        *DeviceInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == DeviceInfo) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  // Disconnect first if we are already attached
  UsbfnDwcCoreShutdown(Usbfn);

  // Save client's descriptor set
  Usbfn->DeviceInfo = DeviceInfo;

  // Update descriptor set only when hijacking enabled
  if (Usbfn->HijackData.RequestHijackingEnabled) {
    if (Usbfn->SSDeviceInfo) {
      FreeDeviceInfo(Usbfn->SSDeviceInfo);
      Usbfn->SSDeviceInfo = NULL;
    }

    // Upgrade the descriptor set to USB 3.0
    Status = UpdateDeviceInfo(Usbfn, DeviceInfo, &Usbfn->SSDeviceInfo);
    ERR_CHK ("failed to update device info structures");
  } else {
    Usbfn->SSDeviceInfo = NULL;
  }

  // validate descriptors
  Status = ValidateDeviceInfo(Usbfn->DeviceInfo, Usbfn->SSDeviceInfo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "invalid DeviceInfo");
    goto ON_EXIT;
  }

  // Iniialize the core
  UsbfnDwcCoreInit(Usbfn);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpointsEx (
  IN EFI_USBFN_IO_PROTOCOL           *This,
  IN EFI_USB_DEVICE_INFO             *DeviceInfo,
  IN EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == DeviceInfo || NULL == SSDeviceInfo) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS(This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  // Disconnect first if we are already attached
  UsbfnDwcCoreShutdown(Usbfn);

  // Save client's descriptor sets
  Usbfn->DeviceInfo = DeviceInfo;
  Usbfn->SSDeviceInfo = SSDeviceInfo;

  // validate descriptors
  Status = ValidateDeviceInfo(Usbfn->DeviceInfo, Usbfn->SSDeviceInfo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "invalid DeviceInfo");
    goto ON_EXIT;
  }

  // Iniialize the core
  UsbfnDwcCoreInit(Usbfn);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointMaxPacketSize (
  IN  EFI_USBFN_IO_PROTOCOL     *This,
  IN  EFI_USB_ENDPOINT_TYPE     EndpointType,
  IN  EFI_USB_BUS_SPEED         BusSpeed,
  OUT UINT16                    *MaxPacketSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == MaxPacketSize ||
      (EndpointType > UsbEndpointInterrupt) || (BusSpeed > UsbBusSpeedSuper)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  *MaxPacketSize = UsbEpMaxPacketSize[EndpointType][BusSpeed];

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Get the USB serial number.

  The serial number is based on the unique serial number from the non-removable emmc.

  @param  BufferSize            On input, the size of the user buffer.
                                On output, the size of the serial number in bytes
  @param  Buffer                The buffer provided by the user

  @retval EFI_SUCCESS           USB serial number is successfully obtained.
  @retval other                 Operation failed
**/
STATIC
EFI_STATUS
GetUsbSerialNumber (
  IN OUT UINTN                  *BufferSize,
  OUT    VOID                   *Buffer
  )
{
  EFI_STATUS                  Status = EFI_DEVICE_ERROR;
  EFI_MEM_CARDINFO_PROTOCOL  *CardInfo = NULL;
  MEM_CARD_INFO               CardInfoData;
  EFI_HANDLE                 *Handles = NULL;
  UINTN                       NumHandles = 0;
  UINT8                       Index = 0;
  UINT32                      SerialNum = 0;
  UINTN                       CharWrittenLength;
  UINTN                       NeededBuffSize;
  CHAR8                       Sn[QUSB_SERIAL_NUMBER_LEN] = {0};

  FNC_ENTER_MSG ();

  // check parameters
  if (!BufferSize || (*BufferSize > 0 && !Buffer)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // Locate all instances of MemCardInfoProtocol
  Status = gBS->LocateHandleBuffer(
      ByProtocol,
      &gEfiMemCardInfoProtocolGuid,
      NULL,
      &NumHandles,
      &Handles);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR,
        "LocateHandleBuffer(gEfiMemCardInfoProtocolGuid) failed: %r",
        Status);
    goto ON_EXIT;
  }

  // locate the MemCardInfo for UFS.
  for (Index = 0; Index < NumHandles; Index++) {
    EFI_BLOCK_IO_PROTOCOL *BlkIo;

    Status = gBS->HandleProtocol (
                  Handles[Index],
                  &gEfiBlockIoProtocolGuid,
                  (VOID **) &BlkIo);
    if (EFI_ERROR(Status)) continue;

    if (FALSE == BlkIo->Media->RemovableMedia) {
      Status = gBS->HandleProtocol(
        Handles[Index],
        &gEfiMemCardInfoProtocolGuid,
        (VOID **)&CardInfo);

      if (EFI_ERROR(Status)) {
        CardInfo = NULL;
        continue;
      }
      // found one.
      break;
    }
  }

  // free the buffer for the handles
  gBS->FreePool(Handles);

  // CardInfo Protocol shall be valid now.
  if (!CardInfo) {
    Status = EFI_DEVICE_ERROR;
    DBG(EFI_D_ERROR, "Cannot locate gEfiMemCardInfoProtocolGuid");
  }
  else {
    Status = CardInfo->GetCardInfo (CardInfo, &CardInfoData);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "GetCardInfo failed: %r", Status);
      goto ON_EXIT;
    }

    Status = gBS->CalculateCrc32(CardInfoData.product_serial_num, CardInfoData.serial_num_len, &SerialNum);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "CalculateCrc32 failed: %r", Status);
      goto ON_EXIT;
    }

    CharWrittenLength = AsciiSPrint(Sn, QUSB_SERIAL_NUMBER_LEN, "%x", SerialNum);

    // no ascii char is written
    if (CharWrittenLength == 0){
      Status = EFI_DEVICE_ERROR;
      DBG(EFI_D_ERROR, "core_snprintf failed, %d", CharWrittenLength);
      goto ON_EXIT;
    }

    // String needs to be UNICODE, which double the size.
    NeededBuffSize = CharWrittenLength * 2;

    // check if buffer is too small
    if (*BufferSize < NeededBuffSize) {
      DBG(EFI_D_INFO, "BufferSize is %d, but need %d", *BufferSize, NeededBuffSize);
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = NeededBuffSize;
      goto ON_EXIT;
    }

    // covert to unicode. The host will consider the serial number invalid if
    // the returned value in string descriptor is not UNICODE and the enumeration
    // can fail.
    for (Index = 0; Index < CharWrittenLength; Index++) {
      ((CHAR8 *) Buffer)[Index*2]   = Sn[Index];
      ((CHAR8 *) Buffer)[Index*2+1] = '\0';
    }

    Status = EFI_SUCCESS;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Get the USB Manufacturer Name.

  @param  BufferSize            On input, the size of the user buffer.
                                On output, the size of the manufacturer name in bytes
  @param  Buffer                The buffer provided by the user

  @retval EFI_SUCCESS           USB Manufacturer Name is successfully obtained.
  @retval other                 Operation failed
**/
STATIC
EFI_STATUS
GetUsbManufacturerName (
  IN OUT UINTN                  *BufferSize,
  OUT    VOID                   *Buffer)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CONST UINT16* Str = NULL;
  UINTN Size = 0;

  FNC_ENTER_MSG ();

  // The string defined in the PCD is null-terminated, but when reporting
  // the USB Manufacturer String, the null should be removed.
  Str = PcdGetPtr(PcdUsbManufacturerStr);
  Size = StrSize(Str) - 2;

  // BufferSize cannot be NULL
  if (BufferSize == NULL) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_WARN, "BufferSize is NULL");
    goto ON_EXIT;
  }

  // If BufferSize is not zero, Buffer cannot be NULL
  if ((*BufferSize != 0) && (Buffer == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_WARN, "BufferSize is not zero, but Buffer is NULL");
    goto ON_EXIT;
  }

  // BufferSize is too small
  if (*BufferSize < Size) {
    Status = EFI_BUFFER_TOO_SMALL;
    DBG(EFI_D_INFO, "BufferSize is too small");
    *BufferSize = Size;
    goto ON_EXIT;
  }

  // Copy the string.
  CopyMem(Buffer, Str, Size);
  *BufferSize = Size;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Get the USB Product String

  @param  BufferSize            On input, the size of the user buffer.
                                On output, the size of the product string in bytes
  @param  Buffer                The buffer provided by the user

  @retval EFI_SUCCESS           USB Product String is successfully obtained.
  @retval other                 Operation failed
**/
STATIC
EFI_STATUS
GetUsbProductString (
  IN OUT UINTN                  *BufferSize,
  OUT    VOID                   *Buffer)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CONST UINT16* Str = NULL;
  UINTN Size = 0;

  FNC_ENTER_MSG ();

  // The string defined in the PCD is null-terminated, but when reporting
  // the USB Manufacturer String, the null should be removed.
  Str = PcdGetPtr(PcdUsbProductStr);
  Size = StrSize(Str) - 2;

  // BufferSize cannot be NULL
  if (BufferSize == NULL) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_WARN, "BufferSize is NULL");
    goto ON_EXIT;
  }

  // If BufferSize is not zero, Buffer cannot be NULL
  if ((*BufferSize != 0) && (Buffer == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_WARN, "BufferSize is not zero, but Buffer is NULL");
    goto ON_EXIT;
  }

  // BufferSize is too small
  if (*BufferSize < Size) {
    Status = EFI_BUFFER_TOO_SMALL;
    DBG(EFI_D_INFO, "BufferSize is too small");
    *BufferSize = Size;
    goto ON_EXIT;
  }

  // Copy the string.
  //CopyMem(Buffer, Str, Size);
  Size = usb_get_product_str_desc_with_chip_serial(
               (CHAR8 *)Str, (CHAR8 *)Buffer, *BufferSize);
  if(0 == Size) {
   Status = EFI_BUFFER_TOO_SMALL; 
   DBG(EFI_D_ERROR,"Product string is NULL");
   goto ON_EXIT;
  }
  
  *BufferSize = Size;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetDeviceInfo (
  IN     EFI_USBFN_IO_PROTOCOL      *This,
  IN     EFI_USBFN_DEVICE_INFO_ID   Id,
  IN OUT UINTN                      *BufferSize,
  OUT    VOID                       *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == BufferSize) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // Decode request into source string and source string size.
  switch (Id) {
  case EfiUsbDeviceInfoSerialNumber:
    Status = GetUsbSerialNumber(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoManufacturerName:
    Status = GetUsbManufacturerName(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoProductName:
    Status = GetUsbProductString(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoUnknown:
  default:
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetVendorIdProductId (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINT16                 *Vid,
  OUT UINT16                 *Pid
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (NULL == This || NULL == Vid || NULL == Pid) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  if (!Usbfn->DeviceInfo->DeviceDescriptor) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  *Vid = Usbfn->DeviceInfo->DeviceDescriptor->IdVendor;
  *Pid = Usbfn->DeviceInfo->DeviceDescriptor->IdProduct;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnAbortTransfer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if ((NULL == This) || (EndpointIndex >= USBFN_ENDPOINT_NUMBER_MAX) ||
      ((UINTN) Direction >= USBFN_ENDPOINT_DIR_MAX)) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  DBG(EFI_D_INFO,"Ep = %d, EpDir %d",EndpointIndex,Direction);

  // Abort transfer.
  Status = UsbfnDwcCancelTransfer(Usbfn, EndpointIndex, Direction);

ON_EXIT:
    FNC_LEAVE_MSG ();
    return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointStallState (
  IN  EFI_USBFN_IO_PROTOCOL         *This,
  IN  UINT8                         EndpointIndex,
  IN  EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  OUT BOOLEAN                       *State
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  DWC_STATUS    DwcStatus = DWC_SUCCESS;
  USBFN_DEV     *Usbfn = NULL;
  DWC_EP_STATE  EpState;

  FNC_ENTER_MSG ();
  if (NULL == This || NULL == State) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  // get state from common layer
  *State = FALSE;
  DwcStatus = DwcGetEPState(&Usbfn->DwcDevice, EndpointIndex, Direction, &EpState);
  DWC_ERR_CHK("Failed to get endpoint stall state: 0x%08x", DwcStatus);
  if (DWC_EP_STATE_STALLED == EpState || DWC_EP_STATE_STALLED_XFER_PENDING == EpState) {
    *State = TRUE;
  }

ON_EXIT:
  if (DWC_ERROR(DwcStatus)) {
    Status = EFI_DEVICE_ERROR;
  }
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnSetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       State
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();
  if (NULL == This) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  Status = UsbfnDwcSetEndpointStallState(Usbfn,EndpointIndex,(DWC_ENDPOINT_DIR)Direction,State);
  ERR_CHK("Failed to %a ep %d, dir %d", State ? "stall" : "unstall",
      EndpointIndex, Direction);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnEventHandler (
  IN      EFI_USBFN_IO_PROTOCOL      *This,
  OUT     EFI_USBFN_MESSAGE          *Message,
  IN OUT  UINTN                      *PayloadSize,
  OUT     EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  USBFN_DEV               *Usbfn = NULL;
  VOID                    *Src = NULL;
  UINTN                   SrcSize = 0;
  EFI_USBFN_MESSAGE       NoneMsg = EfiUsbMsgNone;
  EFI_USBFN_MESSAGE       *Msg = NULL;
  LIST_ENTRY              *ListEntry =  NULL;
  EFI_USBFN_MESSAGE_NODE  *CurrentMsgNode = NULL;

  if (NULL == This || NULL == Message || NULL == PayloadSize ||
      NULL == Payload || *PayloadSize == 0) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  if (IsListEmpty(&Usbfn->UsbfnMessages)) {
    Msg      =  &NoneMsg;
  } else {
    ListEntry = GetFirstNode(&Usbfn->UsbfnMessages);
    USB_ASSERT_RETURN(ListEntry, EFI_DEVICE_ERROR);
    CurrentMsgNode = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);
    Msg = &CurrentMsgNode->Message;

    // Check for a message payload, if any.
    switch (CurrentMsgNode->Message) {

    case EfiUsbMsgEndpointStatusChangedRx:
    case EfiUsbMsgEndpointStatusChangedTx:
      Src     = &(CurrentMsgNode->Data.XferCbResult);
      SrcSize = sizeof (CurrentMsgNode->Data.XferCbResult);
      break;

    case EfiUsbMsgSetupPacket:
      Src     = &(CurrentMsgNode->Data.SetupCbRequest);
      SrcSize = sizeof (CurrentMsgNode->Data.SetupCbRequest);
      break;

    case EfiUsbMsgBusEventSpeed:
      Src     = &(CurrentMsgNode->Data.SpeedCbSpeed);
      SrcSize = sizeof (CurrentMsgNode->Data.SpeedCbSpeed);
      break;
    default:
      ;
    }
  }
  USB_ASSERT_RETURN((NULL != Msg && *Msg <= EfiUsbMsgBusEventSpeed), EFI_DEVICE_ERROR);
  if (*Msg > EfiUsbMsgNone) {
    DBG (EFI_D_INFO, "%a", MsgToStr[*Msg]);
  }

  USB_ASSERT_RETURN(((Src && SrcSize) || (!Src && !SrcSize)), EFI_DEVICE_ERROR);

  if (*Msg == EfiUsbMsgNone) {
    // No messages in queue. It's safe to poll again.
    UsbfnDwcCorePoll(Usbfn);
  } else {
    // At least one message in queue. Try to copy it.
    if (*PayloadSize < SrcSize) {
      Status = EFI_BUFFER_TOO_SMALL;
    } else {
      CopyMem(Payload, Src, SrcSize);
    }
  }

  // Always update client.
  *Message = *Msg;
  if (SrcSize) {
    *PayloadSize = sizeof(*Payload);
  } else {
    *PayloadSize = 0;
  }

ON_EXIT:
  if(ListEntry) {
    //
    // Remove the message from the active
    // message list and put it back to the
    // memory pool
    //
    RemoveEntryList(ListEntry);
    InsertTailList(&Usbfn->UsbfnMessagePool, ListEntry);
  }
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnTransfer (
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT  UINTN                         *BufferSize,
  IN OUT  VOID                          *Buffer
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USBFN_DEV   *Usbfn = NULL;
  UINTN       XferBufferSize;
  VOID        *XferBuffer;

  FNC_ENTER_MSG ();

  if ((NULL == This) || (NULL == BufferSize) || (NULL == Buffer) ||
      (EndpointIndex >= USBFN_ENDPOINT_NUMBER_MAX) || ((UINTN) Direction >= USBFN_ENDPOINT_DIR_MAX)) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  // set client's buffer
  XferBuffer     = Buffer;
  XferBufferSize = *BufferSize;

  // override client's buffer if hijacking enabled
  if (Usbfn->HijackData.RequestHijackingEnabled && Usbfn->HijackData.HijackThisRequest) {
    switch (Usbfn->HijackData.HijackDescType) {

      case USB_DESC_TYPE_DEVICE:
        DBG(EFI_D_WARN, "Hijacking Device Descriptor");
        XferBuffer     = Usbfn->SSDeviceInfo->DeviceDescriptor;
        XferBufferSize = Usbfn->SSDeviceInfo->DeviceDescriptor->Length;
        break;

      case USB_DESC_TYPE_CONFIG:
        DBG(EFI_D_WARN, "Hijacking Config Descriptor");
        XferBuffer     = Usbfn->SSDeviceInfo->ConfigInfoTable[USBFN_DEFAULT_CONFIG_INDEX]->ConfigDescriptor;
        XferBufferSize = Usbfn->SSDeviceInfo->ConfigInfoTable[USBFN_DEFAULT_CONFIG_INDEX]->ConfigDescriptor->TotalLength;
        break;
    }
    Usbfn->HijackData.HijackDescType       = USB_DESC_TYPE_UNDEFINED;
    Usbfn->HijackData.HijackedBuffer       = Buffer;
    Usbfn->HijackData.HijackedBufferLength = *BufferSize;
  }

  Status = UsbfnDwcTransfer(Usbfn, EndpointIndex, Direction, XferBufferSize, XferBuffer, Buffer);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetMaxTransferSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINTN                  *MaxTransferSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  if (NULL == This || NULL == MaxTransferSize) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  *MaxTransferSize = USBFN_MAX_TRANSFER_SIZE;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnAllocateTransferBuffer (
  IN   EFI_USBFN_IO_PROTOCOL  *This,
  IN   UINTN                  Size,
  OUT  VOID                   **Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  if (NULL == Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  *Buffer = UncachedAllocateAlignedPool(Size, USBFN_XFER_BUFFER_ALIGNMENT);
  if (NULL == *Buffer) {
    Status = EFI_OUT_OF_RESOURCES;
    DBG(EFI_D_ERROR, "out of memory");
    goto ON_EXIT;
  }
  DBG(EFI_D_POOL, "Transfer Buffer addr 0x%x. Transfer Buffer Size %d", *Buffer, Size);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnFreeTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  IN VOID                   *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  if (NULL == Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  UncachedFreeAlignedPool(Buffer);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnStartController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  )
{
  FNC_ENTER_MSG ();

  // Nothing to be done here

  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnStopController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV  *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (NULL == This) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  // Will force a disconnect
  UsbfnDwcCoreShutdown(Usbfn);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnSetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN      UINTN                         BufferSize,
  IN      VOID                          *Buffer
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  USBFN_DEV      *Usbfn  = NULL;

  FNC_ENTER_MSG();

  if(!This || !Buffer || EndpointIndex > USB_ENDPOINT_NUMBER_MAX ||
     Direction > EfiUsbEndpointDirectionHostIn){
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  DBG(EFI_D_INFO, "EP %d, Dir %d, PolicyType %d, BufferSize %d, Buffer %p",
      EndpointIndex,
      Direction,
      PolicyType,
      BufferSize,
      Buffer
      );

  Usbfn = USBFN_FROM_THIS(This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  switch (PolicyType) {
  case EfiUsbPolicyZeroLengthTermination:
    if (Direction == EfiUsbEndpointDirectionDeviceRx) {
      Status = EFI_UNSUPPORTED;
      ERR_CHK("Enabling ZLT on an RX endpoint is not supported");
    }
    Usbfn->PipePolicy.ZLT[EndpointIndex][Direction] = *((BOOLEAN *)Buffer);
    break;
  default:
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN OUT  UINTN                         *BufferSize,
  OUT     VOID                          *Buffer
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  USBFN_DEV          *Usbfn  = NULL;

  FNC_ENTER_MSG ();

  if (!This || !BufferSize || !Buffer ||
      EndpointIndex > USB_ENDPOINT_NUMBER_MAX ||
      Direction > EfiUsbEndpointDirectionHostIn) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  DBG(EFI_D_INFO, "EP %d, Dir %d, PolicyType %d, BufferSize %d, Buffer %p",
    EndpointIndex,
    Direction,
    PolicyType,
    *BufferSize,
    Buffer);

  Usbfn = USBFN_FROM_THIS(This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  switch (PolicyType){
  case EfiUsbPolicyMaxTransactionSize:
    if (*BufferSize < sizeof(UINT32)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(UINT32);
      ERR_CHK("buffer too small");
    }

    *BufferSize = sizeof(UINT32);
    * ((UINT32 *)Buffer) = Usbfn->PipePolicy.MaxTransferSize[EndpointIndex][Direction];
    break;
  case EfiUsbPolicyZeroLengthTerminationSupport:
    // check for buffer size
    if(*BufferSize < sizeof(BOOLEAN)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(BOOLEAN);
      ERR_CHK("buffer too small");
    }
    // SNPS core supports ZLT on TX endpoints
    if (Direction == EfiUsbEndpointDirectionDeviceTx) {
      *((BOOLEAN *)Buffer) = TRUE;
    } else {
      *((BOOLEAN *)Buffer) = FALSE;
    }
    *BufferSize = sizeof(BOOLEAN);
    break;
  case EfiUsbPolicyZeroLengthTermination:
    if (*BufferSize < sizeof(BOOLEAN)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(BOOLEAN);
      ERR_CHK("buffer too small");
    }

    *BufferSize = sizeof(BOOLEAN);
    if (Direction == EfiUsbEndpointDirectionDeviceRx) {
      *((BOOLEAN *) Buffer) = FALSE;
    }
    else {
      *((BOOLEAN *)Buffer) = Usbfn->PipePolicy.ZLT[EndpointIndex][Direction];
    }
    break;
  default:
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}
