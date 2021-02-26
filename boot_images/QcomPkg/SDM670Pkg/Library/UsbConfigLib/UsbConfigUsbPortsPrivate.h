/** @file

  The public header file for UsbConfigUsbCore.c
  Copyright (c) 2016 QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/18/16   ck       Re-arrange data structures.
=============================================================================*/

#ifndef _USB_CONFIG_USB_PORTS_PRIV_H_
#define _USB_CONFIG_USB_PORTS_PRIV_H_

#include "UsbConfigUsbPortsPublic.h"

extern EFI_QCOM_PMIC_USB_PROTOCOL *gPmicUsbProtocol;
extern EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;

/**
Structure encapsulating a USB port. Contains port specific state information
**/
typedef struct _QCOM_USB_PORT {
  USB_CFG_PORT_MESSAGE                 Message;
  USB_CFG_PORT_CONNECT_STATE           ConnectState;
  EFI_PM_USB_TYPEC_PORT_STATUS         PortStatus;
  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS ConnectModeStatus;
} QCOM_USB_PORT;

typedef enum _PM_DEVICE_INDEX
{
    PM_DEVICE_0,
    PM_DEVICE_1,
    PM_DEVICE_2,
    PM_DEVICE_3,
    PM_DEVICE_4,
    PM_DEVICE_INVALID
} PM_DEVICE_INDEX;

#endif /* _USB_CONFIG_USB_PORTS_PRIV_H_ */

