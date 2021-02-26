/**
 * @file UsbConfigPrivate.h
 *
 * @brief The private header file for UsbConfigDxe
 *
 * Copyright (C) 2014-2017 by Qualcomm Technologies, Inc.  All Rights Reserved. 
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------- 
 02/02/17   pm      Port from Boot.xf.1.2.2, Vbus Enablement in USB Config Protocol
 04/12/16   ck      Add the support for Usb init protocol
 02/23/16   ck      Add the support to query cable connection
 01/22/16   ck      Support on deman USBFn configuration. 
 02/27/15   amitg   UEFI Low Power Mode Updates
 11/19/14   ck      Add toggle USB mode support
 11/12/14   ck      Create header
 =============================================================================*/
#include <Base.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIChipInfo.h>
#include <Library/UsbConfigLib.h>
#include <Guid/EventGroup.h>

// Event Group GUID for toggling USB mode
extern EFI_GUID gEfiEventToggleUsbModeGuid;
// Event Group GUID for Start USB configuration on demand.
extern EFI_GUID gInitUsbControllerGuid;
// Get port partner connection status
extern EFI_STATUS EFIAPI GetUsbFnConnStatus(
  IN  QCOM_USB_CONFIG_CORE_TYPE    CoreType,
  OUT BOOLEAN                     *IsConnected);

/**
 * @brief Get the physical address of the USB core
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_CORE_BASE_ADDR
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetCoreBaseAddr(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  OUT  UINTN                      *BaseAddr
);


/**
 * @brief Configure USB core based on CoreNum and Mode
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_CONFIG_USB
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigConfigUsb(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE         Mode,
  IN  QCOM_USB_CORE_NUM          CoreNum
);


/**
 * @brief Reset USB core
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_RESET_USB
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigResetUsb(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  IN  BOOLEAN                    DeviceMode
);


/**
 * @brief Get appropriate core type for ChipIdea controller
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONFIG
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbFnConfig(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
);


/**
 * @brief Get appropriate core type for Synopsys controller
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_SS_USBFN_CONFIG
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetSSUsbFnConfig(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
);


/**
 * @brief Get connection state for controller
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_SS_USBFN_CONN_STATUS
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetConnStatus (
   IN  QCOM_USB_CONFIG_PROTOCOL   *This, 
   IN  QCOM_USB_CONFIG_CORE_TYPE  CoreType,
   OUT BOOLEAN                    *IsConnectionEstablished
);


/**
 * @brief Get the max number of cores supported
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_USB_MAXHOSTCORENUM
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbMaxHostCoreNum(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_MODE_TYPE        Mode,
  OUT UINT8                     *MaxCoreNum
);

/**
 * @brief Get the Vbus State for the Core (configured in Host Mode)
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_VBUS_STATUS
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetVbusState(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_CORE_NUM         CoreNum,
  OUT UINT32                   *VbusStatus
);


/**
 * @brief Enable the Vbus State for the Core/Port
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_ENABLE_VBUS
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigEnableVbus(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_CORE_NUM         CoreNum
);



/**
 * @brief Get appropriate core type to use for the specified host mode and index.
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_GET_USBHOST_CONFIG
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbHostConfig(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE          Mode,
  IN  QCOM_USB_CORE_NUM           CoreNum,
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
);


/**
 * @brief The function installs the relevant protocol(s) for the software stack
 * used on the core.
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_START_CONTROLLER
 *  
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI UsbConfigStartController(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM           CoreNum,
  IN   QCOM_USB_MODE_TYPE          ModeType
);


/**
 * @brief The function removes the relevant protocol(s) for the software stack
 * used on the core.
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_STOP_CONTROLLER
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI UsbConfigStopController(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM           CoreNum,
  IN   QCOM_USB_MODE_TYPE          ModeType
);


/**
 * @brief The function prepares the core for entering low power mode
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_ENTER_LPM
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigEnterLPM(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE          Mode,
  IN  QCOM_USB_CORE_NUM           CoreNum
);

/**
 * @brief The function prepares the core for exiting low power mode
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_EXIT_LPM
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigExitLPM(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE          Mode,
  IN  QCOM_USB_CORE_NUM           CoreNum
);


/**
 * @brief Toggle the USB mode on the core
 *
 * The function implements QCOM_USB_CONFIG_PROTOCOL_TOGGLE_USB_MODE
 *
 * @see EFIUsbConfig.h
 */
EFI_STATUS
EFIAPI
UsbConfigToggleMode(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM           CoreNum
);


/**
* @brief Get the USB core count in the system
*
* The function implements QCOM_USB_CONFIG_PROTOCOL_GET_CORE_COUNT
*
* @see EFIUsbConfig.h
*/
EFI_STATUS
EFIAPI
UsbConfigGetCoreCount(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT  UINT8                      *Count
);


/**
* @brief Get the supported USB mode on the core
*
* The function implements QCOM_USB_CONFIG_PROTOCOL_GET_SUPPORTED_MODE
*
* @see EFIUsbConfig.h
*/
EFI_STATUS
EFIAPI
UsbConfigGetSupportedMode(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT QCOM_USB_MODE_TYPE         *ModeType
);
