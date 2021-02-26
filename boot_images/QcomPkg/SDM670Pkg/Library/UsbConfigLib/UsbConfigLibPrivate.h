/** @file \UsbConfigPrivate.h
* The private header for the UsbConfigLib.c
* Copyright (c) 2015-2017 QUALCOMM Technologies Inc. All rights reserved.
*/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 04/19/17   amitg    Sec Core Updates, Primary Core enable pmic call
 03/17/17   pm       Remove UsbCSetPortRole
 03/06/17   amitg    EUD Support
 02/03/17   pm       Port from Boot.xf.1.4
 09/22/16   amitg    Napali Pre-Si Updates
 02/23/16   ck       Isolate USB Core and Port data to its own module
 11/24/15   ck       Virtio FBC support
 10/26/15   ck       Init Sdm845 Pre-Si support
=============================================================================*/

#ifndef _USB_CONFIG_PRIVATE_H_
#define _USB_CONFIG_PRIVATE_H_

#include "UsbConfigLibPublic.h"
#include "UsbConfigUsbCoresPublic.h"
#include "UsbConfigUsbPortsPublic.h"
#include "HalusbHWIO.h"
#include "QcomBaseLib.h"
//
// USB Configuration Device Signature
//
#define USB_CONFIG_DEV_SIGNATURE SIGNATURE_32 ('u', 's', 'b', 'c')

//
// Polling Interval for USB Type-C Connection State associated with PortStateChgEvt
//
#define USB_PORT_POLL_INTERVAL (500 * 10000U)

//
// Get charger port type max trial limit
//
#define MAX_CHARGER_TYPE_READ_ATTEMPTS (100U)
//
// Get charger port type wait time b/w each trial
//
#define CHARGER_TYPE_WAIT_INTERVAL (10 * 1000U)
//
// The max wait time for type-c port mode change
//
#define TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS 250U

//
// USB SS PHY LANE NUMBER
//
#define USB_SS_PHY_LANE_A 0
#define USB_SS_PHY_LANE_B 1

//
// Check if the charger type is SDP or CDP
//
#define IS_SDP_CDP(CHG_TYPE) (((CHG_TYPE) == (EFI_PM_USB_CHG_PORT_SDP)) || ((CHG_TYPE) == (EFI_PM_USB_CHG_PORT_CDP)))

//
// Check if the USB hardware has been initialized before (i.e InitCommon has been invoked)
//
#define IS_HARDWARE_CONFIUGRED(USB_MODE_TYPE) ((USB_MODE_TYPE) != (USB_INVALID_MODE))

//
// Check if the USB is configured for the peripheral mode
//
#define IS_USB_PERIPHERAL_MODE(MODE) (((MODE) == (USB_DEVICE_MODE_SS)) || ((MODE) == (USB_DEVICE_MODE_HS)))

//
// Check if the USB is configured for the host mode
//
#define IS_USB_HOST_MODE(MODE) (((MODE) == (USB_HOST_MODE_XHCI)) || ((MODE) == (USB_HOST_MODE_EHCI)))

//
// Check whether Type-C UFP port is detected.
//
#define IS_PORT_UFP(PORT) ((PORT) == (USB_TYPEC_PORT_MODE_UFP))

//
// Forward declaration of USB structure for parameter to function types.
//
typedef struct _USB_CONFIG_DEVICE USB_CONFIG_DEVICE;

//
// SDM845 RUMI USB HS PHY definition.
//
#define HS2ADDR (0xa720000)
#define QRBTC_BASE (0xa728000)

#undef HAL_SSUSB_SNPS_R3PC_DCM_CMD_RESET
#define HAL_SSUSB_SNPS_R3PC_DCM_CMD_RESET (0x1<<20)

#define QRBTC_USB2_PLL      QRBTC_BASE+0x1404
#define QRBTC_USB2_PLLCTRL1 QRBTC_BASE+0x1410
#define QRBTC_USB2_PLLCTRL2 QRBTC_BASE+0x1414
#define QRBTC_USB2_PLLCTRL3 QRBTC_BASE+0x1418
#define QRTBC_USB2_PLLTEST1 QRBTC_BASE+0x1408

//
// Port State Change Event. The event is signaled periodically 
// for the USB Config lib to check the Type-C port status
//
static EFI_EVENT                   PortStateChgEvt = NULL;

//
// Template of UsbConfigProtocol struct
//
extern QCOM_USB_CONFIG_PROTOCOL    gUsbConfigTemplate;

//
// USB mode static configuration data structure tracks whether a USB port should
// always be configured in host or peripheral mode.
// The configuration set here overrides the dynamic configuration based on the type-c
// port detection.
//
typedef struct _QCOM_USB_MODE_STATIC_CFG {
  BOOLEAN             Enabled; // static configuration enabled
  QCOM_USB_MODE_TYPE  UsbMode; // if Enable is true, the UsbMode specified the cfg mode
} QCOM_USB_MODE_STATIC_CFG;

/**
 * Structure representing USB controller
 */
struct _USB_CONFIG_DEVICE{
  UINT32                     Signature;
  EFI_HANDLE                 HostHandle;        // host controller handle
  EFI_HANDLE                 PeripheralHandle;  // peripheral controller handle
  QCOM_USB_CORE_INTERFACE   *UsbCoreInterface;  // USB core hardware configuration functions
  VOID                      *UsbPortHandle;     // USB port handle for Type-C related support
  QCOM_USB_CONFIG_PROTOCOL   UsbConfig;         // USB Config Protocol Handle
  QCOM_USB_MODE_STATIC_CFG   EnableStaticCfg;   // Record of static USB configuration
};


/**
Callback Function used for checking the USB Type-C Port Status to determine if device is attached or detached
This function is called periodically to determine new USB Type-C Port state

@param [in] Event                The event that is triggered.
@param [in] Context              The context to the event.

@return None.

**/
STATIC
VOID
EFIAPI
UsbPortStateChgCb(
  IN EFI_EVENT      Event,
  IN VOID           *Context
  );
#endif /* _USB_CONFIG_PRIVATE_H_ */

