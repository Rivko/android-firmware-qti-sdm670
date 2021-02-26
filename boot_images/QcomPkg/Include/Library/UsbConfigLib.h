/** @file

  Defines the interface of a platform specific library to configure USB.

  Copyright (c) 2011 - 2017, QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 03/03/17   pm         Vbus Enablement in USB Config Protocol
 04/12/16   ck         Support USB init protocol
 02/27/15   amitg      UEFI Low Power Mode Updates
 11/19/14   ck         Add toggle USB mode support
 10/22/14   amitg      UEFI LPM support
 08/13/14   amitg      Updates in Clock Enable/Disable function
 06/05/14   amitg      MSM 8994 Bring Up, Added disable clock function
 08/29/13   ml         Added GetSSUsbFnConfig
 03/30/11   sniedzie   Created from PlatConfig.
 05/11/11   sbryksin   Added platform/core type selection for UsbFn
 08/11/11   sbryksin   Changes for 8960v2 w/ HSIC
 11/02/11   sbryksin   Added initial APQ8064 revision
=============================================================================*/

#ifndef _USB_CONFIG_LIB_H_
#define _USB_CONFIG_LIB_H_

#include <Library/DebugLib.h>
#include <Protocol/EFIUsbConfig.h>


//
// USB Core Offset Wrappers
//
#define HAL_HSUSB_READ(BaseAddr, RegAddr) \
  MmioRead32 ((BaseAddr) + (RegAddr))
#define HAL_HSUSB_WRITE(BaseAddr, RegAddr, Data) \
  MmioWrite32 ((BaseAddr) + (RegAddr), Data)
#define HAL_HSUSB_OR(BaseAddr, RegAddr, Data) \
  MmioOr32 ((BaseAddr) + (RegAddr), Data)
#define HAL_HSUSB_AND(BaseAddr, RegAddr, Data) \
  MmioAnd32 ((BaseAddr) + (RegAddr), Data)


//
// ULPI PHY Miscellaneous Definitions
//
#define ULPI_PHY_MAX_RW_VERIFY_ATTEMPTS   10000
#define ULPI_PHY_MAX_SUPPORTED_PORTS      1

//
// SS PHY Miscellaneous Definitions
//

#define SS_PHY_MAX_RW_VERIFY_ATTEMPTS     10000

//
// Clock Enable/Disable Attempts
//

#define MAX_CLK_ENDIS_ATTEMPTS     10

//
// Miscellaneous Definitions
//

// Tag all output with the calling function name, terminate with a newline, and
// eliminate the need for double parens with a variadic macro.
#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG (EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__);

// For debug stack info.
#define FNC_ENTER_MSG() DBG (EFI_D_INFO, "++")
#define FNC_LEAVE_MSG() DBG (EFI_D_INFO, "--")

// Cannot continue, exit function.
#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, ##__VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_WARN, WrnStr, ##__VA_ARGS__); \
    } \
  } while (0)

// Check parameters.
#define ARG_CHK(Expr) \
  do { \
    if (!(Expr)) { \
      Status = EFI_INVALID_PARAMETER; \
      ASSERT_EFI_ERROR (Status); \
      ERR_CHK ("invalid parameter"); \
    } \
  } while (0)

//
// API
//
EFI_STATUS
EFIAPI UsbStartController(
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  );
  

EFI_STATUS
EFIAPI UsbStopController(
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  );
  
EFI_STATUS
UlpiPhyRead (
  IN   UINT32 BaseAddr,
  IN   UINT8  RegAddr,
  OUT  UINT8  *Data
  );

  
EFI_STATUS
UlpiPhyWrite (
  IN UINT32  BaseAddr,
  IN UINT8   RegAddr,
  IN UINT8   Data
  );

  
EFI_STATUS
UlpiPhyWriteBmsk (
  IN UINT32  BaseAddr,
  IN UINT8   RegAddr,
  IN UINT8   Data,
  IN UINT8   Bmsk
  );


EFI_STATUS 
SSUSBPhyRead (
  IN   UINT32  BaseAddr,
  IN   UINT32  RegAddr,
  OUT  UINT16  *Data
  );


EFI_STATUS 
SSUSBPhyWrite (
    IN UINT32   BaseAddr,
    IN UINT32   RegAddr,
    IN UINT16   Data
  );


EFI_STATUS
EFIAPI
EnableClock (
  IN   CONST CHAR8  *ClockName,
  IN   UINT32       minFrequency,
  IN   UINT32       divide
  );

EFI_STATUS
EFIAPI
DisableClock (
  IN   CONST CHAR8  *ClockName
  );

EFI_STATUS
EFIAPI
GetCoreBaseAddr (
  IN   QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  OUT  UINTN                      *BaseAddr
  );

EFI_STATUS
EFIAPI
ConfigUsb (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  );


EFI_STATUS
EFIAPI
ResetUsb (
  IN QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  IN BOOLEAN                    DeviceMode
  );

EFI_STATUS
EFIAPI
FsUsbSpoofDetach (
  IN QCOM_USB_CONFIG_CORE_TYPE  CoreType
  );

EFI_STATUS
EFIAPI
GetUsbFnConfig (
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
  );

EFI_STATUS
EFIAPI
GetSSUsbFnConfig (
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
  );

EFI_STATUS
EFIAPI
GetUsbMaxHostCoreNum (
  IN  QCOM_USB_MODE_TYPE  Mode,
  OUT UINT8               *MaxCoreNum
  );

EFI_STATUS
EFIAPI
GetUsbHostConfig (
  IN  QCOM_USB_MODE_TYPE         Mode,
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType
  );

EFI_STATUS
EFIAPI
GetUsbVbusState (
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT UINT32                    *VbusStatus
  );

EFI_STATUS
EFIAPI
UsbEnableVbus (
  IN  QCOM_USB_CORE_NUM          CoreNum
  );

EFI_STATUS
EFIAPI
EnterLPM (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
);

EFI_STATUS
EFIAPI
ExitLPM (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
);


EFI_STATUS
EFIAPI
UsbToggleControllerMode (
  IN QCOM_USB_CORE_NUM          CoreNum
  );

EFI_STATUS
EFIAPI
UsbConfigInit (VOID );

EFI_STATUS
EFIAPI
UsbConfigLibExitBootService (VOID);


/**
@summary
Return the total number of USB cores in the system

@param [out] Count          Pointer to the count of the system USB cores.

@retval EFI_SUCCESS         The operation succeeded.
@retval Others              The operation failed.

**/
EFI_STATUS
EFIAPI
UsbConfigLibGetCoreCount(
  OUT   UINT8          *Count
  );


/**
@summary
Return the total number of USB cores in the system

@param [in]  CoreNum        Core number
@param [out] ModeType       Pointer to the supported mode type

@retval EFI_SUCCESS         The operation succeeded.
@retval Other               The operation failed.

**/
EFI_STATUS
UsbConfigLibGetSupportedMode(
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT QCOM_USB_MODE_TYPE         *ModeType
  );

#endif

