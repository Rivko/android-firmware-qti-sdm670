/** @file UsbConfigUsbCoresPublic.h

  The public header file for UsbConfigUsbCore.c
  Copyright (c) 2016-2017 QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/03/17   pm       Vbus Enablement in USB Config Protocol
 02/03/17   pm       Port from Boot.xf.1.4
 02/18/16   ck       Init Check-in
=============================================================================*/

#ifndef _USB_CONFIG_USB_CORES_PUB_H_
#define _USB_CONFIG_USB_CORES_PUB_H_

#include "UsbConfigLibPublic.h"

typedef struct _QCOM_USB_CORE_INTERFACE QCOM_USB_CORE_INTERFACE;

/**
Perform USB core initialization common to host and device mode.

@param [in]  This         The USB core interface instance
@param [in]  ResetUsbOnly If TRUE, only reset the USB link and PHY
                          If FALSE, in addition to the USB link and PHY reset
                          Enable clocks and power rails
@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_COMMON) (
  IN QCOM_USB_CORE_INTERFACE              *This,
  IN BOOLEAN                               ResetUsbOnly
  );

/**
Perform USB core initialization specific to device mode. This function
must be called after the core's common initialization function.

@param [in]  This      The USB core interface instance

@retval EFI_SUCCESS       USB core successfully initialized in device mode
@retval EFI_UNSUPPORTED   USB core doesn't support device mode
@retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_DEVICE) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

/**
Perform USB core initialization specific to host mode. This function
must be called after the core's common initialization function.

@param [in]  This      The USB core interface instance

@retval EFI_SUCCESS       USB core successfully initialized in host mode
@retval EFI_UNSUPPORTED   USB core doesn't support host mode
@retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_HOST) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

/**
Get Vbus state for the Core

@param [in]  This          The USB core interface instance
@param [out] VbusState     Vbus State for the Core

@retval EFI_SUCCESS       USB core successfully initialized in host mode
@retval EFI_UNSUPPORTED   USB core doesn't support Vbus Status via this API
@retval Others            Error encountered
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_GET_VBUS) (
  IN QCOM_USB_CORE_INTERFACE              *This,
  OUT UINT32                              *VbusState
  );

/**
Enable Vbus/OTG in host mode for the core. This function can be called 
after the host mode core is enabled and if the Vbus was not enabled correctly
or Vbus is shut down due to Over Corrent Protection being triggered from PMIC

@param [in]  This      The USB core interface instance

@retval EFI_SUCCESS       Vbus Enabled sucessfully
@retval EFI_UNSUPPORTED   Vbus enablement not supported
@retval Others            Error encountered while enabling Vbus
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_ENABLE_VBUS) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );


/**
Perform a hardware reset on the USB core.

@param [in]  This      The USB core interface instance

@retval EFI_SUCCESS       USB core successfully reset
@retval Others            Error encountered resetting USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_RESET) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

/**
Perform a low power mode sequence on the USB core.

@param [in]  This      The USB core interface instance

@retval EFI_SUCCESS       USB core entered low power mode sucessfully
@retval Others            Error encountered entering low power mode for USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_LPM) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

/**
Select CC1 or CC2 for the core

@param [in]  This         The USB core interface instance
@param [in]  LaneNumber   The PHY lane number

@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_SET_PHY_LANE) (
  IN QCOM_USB_CORE_INTERFACE              *This,
  IN UINT8                                LaneNumber
  );

/**
Get the base register address of the USB core

@param [in]  This         The USB core interface instance
@param [out] BaseAddress  The register base address of the USB core

@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_GET_BASE_ADDR) (
  IN QCOM_USB_CORE_INTERFACE              *This,
  OUT UINTN                               *BaseAddress
  );

/**
@brief Reset the SuperSpeed USB PHY.

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_RESET_SS_PHY) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

/**
@brief Put the PHY in to the Non-Driving mode.

@param [in]  This           The USB core interface instance
@param [in]  NonDrivingMode TRUE  - Put the PHY in the Non-Driving mode
                            FALSE - Put the PHY out of the Non-Driving mode


@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_SET_HS_PHY_NON_DRIVING_MODE) (
  IN QCOM_USB_CORE_INTERFACE              *This,
  IN BOOLEAN                              NonDrivingMode
  );

/**
@brief Init the PHY but keep the PHY in power down state

@param [in]  This           The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_HS_PHY_PWR_DWN) (
  IN QCOM_USB_CORE_INTERFACE              *This
  );

//
// The public interface provided by the USB Core module. The interface should
// be implemented for each core based on the core specific configuration
// need.
//
struct _QCOM_USB_CORE_INTERFACE {
  QCOM_USB_MODE_TYPE                        ModeType;
  QCOM_USB_CORE_INIT_COMMON                 InitCommon;
  QCOM_USB_CORE_INIT_DEVICE                 InitDevice;
  QCOM_USB_CORE_INIT_HOST                   InitHost;
  QCOM_USB_CORE_GET_VBUS                    GetVbusStatus;
  QCOM_USB_CORE_ENABLE_VBUS                 EnableVbus;
  QCOM_USB_CORE_RESET                       Reset;
  QCOM_USB_CORE_LPM                         CoreEnterLPM;
  QCOM_USB_CORE_LPM                         CoreExitLPM;
  QCOM_USB_CORE_SET_PHY_LANE                SetPhyLane;
  QCOM_USB_CORE_GET_BASE_ADDR               GetBaseAddress;
  QCOM_USB_CORE_RESET_SS_PHY                ResetSSPhy;
  QCOM_USB_CORE_SET_HS_PHY_NON_DRIVING_MODE SetNonDrivingMode;
  QCOM_USB_CORE_INIT_HS_PHY_PWR_DWN         InitHsPhyPwrDwn;
};

/*******************************************************************************
* Public Functions
******************************************************************************/
/**
@brief Initialize the USB Core module for a specific core.
The client calls the function to obtain the QCOM_USB_CORE_INTERFACE for the core
identified by CoreNum.

@param [in]  CoreNum           The core number to which the UsbCoreInterface applies.
@param [out] UsbCoreInterface  The QCOM_USB_CORE_INTERFACE for the core specified by CoreNum

@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
EFI_STATUS
UsbConfigCoresInit(
  IN  UINTN                      CoreNum,
  OUT QCOM_USB_CORE_INTERFACE  **UsbCoreInterface
  );

/**
@brief De-initialize the USB Core module for a specific core.

The client calls the function to de-initialize the UsbCoreInterface obtained from
UsbConfigUsbCoresInit. The interface is invalid after the function is invoked.

@param [in] UsbCoreInterface  The QCOM_USB_CORE_INTERFACE to be de-initialized.

@retval EFI_SUCCESS       The operation succeeded
@retval Others            The operation failed
**/
EFI_STATUS
UsbConfigCoresDeinit(
  IN QCOM_USB_CORE_INTERFACE  *UsbCoreInterface
  );
#endif /* _USB_CONFIG_USB_CORES_PUB_H_ */
