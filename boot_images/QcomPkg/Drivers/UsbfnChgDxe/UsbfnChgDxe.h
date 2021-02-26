/**
 * @file UsbfnChgDxe.h
 *
 * Header file of UsbfnChgDxe.c
 *
 * Defines structures related to USBfn charging device support and the simplified 
 * debug print macro. 
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/23/16   ck      Init version
=============================================================================*/

#ifndef _USBFN_CHG_H_
#define _USBFN_CHG_H_


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIUsbDevice.h>
#include <Protocol/EFIUsbfnChgSrv.h>

#define DBG(EfiD, Str, ...) \
  DEBUG((EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__))

typedef struct _USBFN_CHG_DEV USBFN_CHG_DEV;

#define USBFN_CHG_DEV_SIGNATURE       SIGNATURE_32 ('u', 'c', 'h', 'g')
#define USBFN_CHG_DEV_FROM_THIS(x)    CR(x, USBFN_CHG_DEV, UsbfnChgSrvProtocol, USBFN_CHG_DEV_SIGNATURE)

/* Charger Device State */
typedef enum _CHG_DEV_STATE {
  CHG_DEV_ST_UNINIT,     // default state
  CHG_DEV_ST_ENUM,       // USB enumeration happens
  CHG_DEV_ST_CHARGER,    // USB charger type detected
  CHG_DEV_ST_DISCONNECT, // USB cable disconnected
  CHG_DEV_ST_SPEED       // USB device speed detected
} CHG_DEV_STATE;

/* USBfn charger device */
struct _USBFN_CHG_DEV {
  UINTN                           Signature;
  EFI_USBFN_CHG_SRV_PROTOCOL      UsbfnChgSrvProtocol;
  EFI_USB_DEVICE_PROTOCOL        *UsbDeviceProtocol;
  EFI_EVENT                       EventHandlerTimer;
  EFI_EVENT                       ExitBootServiceEvent; 
  UINTN                           CtrlXferBufferSize; 
  VOID*                           CtrlXferBuffer;
  EFI_USBFN_CHG_SRV_CHARGER_TYPE  ChargerType;
  CHG_DEV_STATE                   ChgDevState;
  EFI_USBFN_CHG_SRV_SPEED_TYPE    DeviceSpeed; // Only apply if ChargerType is SDP
};

#endif //_USBFN_CHG_H_
