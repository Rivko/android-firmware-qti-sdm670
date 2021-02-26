/*! @file PmicUsbProtocol.c 

*  PMIC- USB MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC USB module.
*
*  Copyright (c) 2015-16 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Inc Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/09/16   sm      Redefined GetOtgStatus API
06/02/16   sm      Added changes to remove support for PMI8998 v1.0
06/01/16   sm      Removed EFI_QCOM_CHARGER_EX_PROTOCOL related changes
05/10/16   sm      Added API to return charger presence
05/09/16   sm      Enabled code inside EFI_PmicUsbEnableOtg()
04/06/16   sm      Completed the API definitions.
04/06/16   sm      Made changes to always assume we have bad PMIv1.0 part 
                   and always run workaround
03/07/16   sm      Added TypeC related APIs (Skeleton)
04/14/15   al      added support for PMI8950 
02/11/15   sv      added support for 8909 and 8916
01/30/15   al      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Protocol/EFIPmicUsb.h>
#include "pm_uefi.h"

/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
EFI_STATUS EFIPmicUsbInit(IN  UINT32 PmicDeviceIndex)
{
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicUsbUsbinValid
(
  IN  UINT32 PmicDeviceIndex,
  OUT BOOLEAN *pValid
)
{   
    return EFI_SUCCESS;
}



/**
EFI_PmicChargerPortType ()

@brief
Gets charger port type
*/
EFI_STATUS
EFIAPI
EFI_PmicUsbChargerPortType
(
  IN  UINT32 PmicDeviceIndex,
  OUT EFI_PM_USB_CHGR_PORT_TYPE *PortType
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbSetOtgILimit
(
  IN  UINT32   PmicDeviceIndex,
  IN  UINT32   ImAmp
)
{   
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI 
EFI_PmicUsbEnableOtg
(
  IN  UINT32   PmicDeviceIndex,
  IN  BOOLEAN  Enable
)
{   
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicUsbGetOtgStatus
(
   IN  UINT32                      PmicDeviceIndex,
   OUT EFI_PM_USB_OTG_STATUS_TYPE  *pOtgStatus
)
{   
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCGetPortState
(
  IN   UINT32                        PmicDeviceIndex,
  OUT  EFI_PM_USB_TYPEC_PORT_STATUS  *TypeCStatus
)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCGetConnectState
(
  IN   UINT32                                PmicDeviceIndex,
  OUT  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS  *pTypeCConnStatus
)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCSetVConn
(
  IN  UINT32                         PmicDeviceIndex,
  IN  EFI_PM_USB_TYPEC_VCONN_CONFIG  *pVconnConfig
)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCSetPortRole
(
  IN  UINT32                      PmicDeviceIndex,
  IN  EFI_PM_USB_TYPEC_PORT_ROLE  PortRole
)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCEnable
(
  IN  UINT32   PmicDeviceIndex,
  IN  BOOLEAN  Enable
)
{

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbGetChargerPresence
(
  OUT BOOLEAN  *pChargerPresent
)
{
 
  return EFI_SUCCESS;
}


/**
PMIC USB UEFI Protocol implementation
*/
EFI_QCOM_PMIC_USB_PROTOCOL PmicUsbProtocolImplementation = 
{
    PMIC_USB_REVISION,
    EFI_PmicUsbUsbinValid,  
    EFI_PmicUsbChargerPortType, 
    EFI_PmicUsbSetOtgILimit,
    EFI_PmicUsbEnableOtg,
    EFI_PmicUsbGetOtgStatus,
    EFI_PmicUsbCGetPortState,
    EFI_PmicUsbCGetConnectState,
    EFI_PmicUsbCSetVConn,
    EFI_PmicUsbCSetPortRole,
    EFI_PmicUsbCEnable,
    EFI_PmicUsbGetChargerPresence,
};

