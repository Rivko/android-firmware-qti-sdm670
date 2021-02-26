/** 
  @file  EFIUsbfnChgSrv.h
  @brief USBFn charger support service API
*/
/*=============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 05/23/16   ck       Init version
=============================================================================*/
#ifndef _EFI_USBFN_CHG_SRV_H_
#define _EFI_USBFN_CHG_SRV_H_

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** Protocol version */
#define EFI_USBFN_CHG_SRV_REVISION  0x00010000


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the gEfiUsbfnChgSrvProtocolGuid
*/
extern EFI_GUID gEfiUsbfnChgSrvProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_USBFN_CHG_SRV_PROTOCOL EFI_USBFN_CHG_SRV_PROTOCOL;

/**
  USB Charger Type
*/
typedef enum {
  USB_CHARGER_UNKNOWN,
  USB_CHARGER_SDP, /**< High-speed USB 1. */
  USB_CHARGER_CDP,
  USB_CHARGER_DCP,
  USB_CHARGER_IDCP
} EFI_USBFN_CHG_SRV_CHARGER_TYPE;

/**
  USB Speed
*/
typedef enum {
  USB_CHG_SPD_UNKNOWN,
  USB_CHG_SPD_LOW,
  USB_CHG_SPD_FULL,
  USB_CHG_SPD_HIGH,
  USB_CHG_SPD_SUPER
} EFI_USBFN_CHG_SRV_SPEED_TYPE;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/* 
  @par Summary
  Start the USB controller for charger detection. If SDP is detected, 
  the function will enumerate the USB device.

  @param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  Others          -- Function completed with error
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_CHG_SRV_START_CONTROLLER)(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL   *This
  );


/*
  @par Summary
  Stop the USB controller and the background event handling timer.

  @param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  Others          -- Function completed with error
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_CHG_SRV_STOP_CONTROLLER)(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL   *This
);


/*
  @par Summary
  Get the charger type connected to the Device \n 
  Once the controller has been started by EFI_USBFN_CHG_SRV_START_CONTROLLER, 
  the client can call the function to get the charger type. 
  If USB_CHARGER_UNKNOWN is returned, it indicates that either the charger type 
  cannot be determined at this point or the hardware is not able detect the 
  charger type.

  @param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.
  @param[out] Type           Pointer to the detected charger type

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  Others          -- Function completed with error
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_CHG_SRV_GET_CHARGER_TYPE)(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL     *This,
  OUT  EFI_USBFN_CHG_SRV_CHARGER_TYPE *Type
);


/*
  @par Summary
  Get the device speed\n 
  The function is only valid if the charger type is SDP or CDP. 

  @param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.
  @param[out] Speed          Pointer to the device speed

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  Others          -- Function completed with error
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USBFN_CHG_SRV_GET_SPEED)(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL     *This,
  OUT  EFI_USBFN_CHG_SRV_SPEED_TYPE   *Speed
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/**
  @par Summary
  Qualcomm USBFn charger service protocol interface. 
*/
struct _EFI_USBFN_CHG_SRV_PROTOCOL {
  UINT64                                     Revision; 
  EFI_USBFN_CHG_SRV_START_CONTROLLER         StartController;
  EFI_USBFN_CHG_SRV_STOP_CONTROLLER          StopController;
  EFI_USBFN_CHG_SRV_GET_CHARGER_TYPE         GetChargerType;
  EFI_USBFN_CHG_SRV_GET_SPEED                GetSpeed;
};
  
#endif // _EFI_USBFN_CHG_SRV_H_
