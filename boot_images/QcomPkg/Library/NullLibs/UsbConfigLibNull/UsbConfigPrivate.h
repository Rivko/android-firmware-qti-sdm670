/** @file

  QCOM_USB_CONFIG_PROTOCOL 8974 internal defines.
  Copyright (c) 2011 - 2014, QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/UsbConfigLibNull/UsbConfigPrivate.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 08/13/14   amitg    Added Clock Enable/Disable structure
 06/04/14   amitg    MSM 8994 Bring Up Updates
 05/20/14   amitg    Enabling Vbus for Host Mode, CleanUp
 04/30/14   ar         Cleanup for NDEBUG build  
 04/23/14   amitg    Added NPA Support for HS1 and SS1 Cores
 03/14/14   amitg    Updates for 8994
 10/29/13   amitg    Initial Revision for MSM 8994
=============================================================================*/

#ifndef _USB_CONFIG_PRIVATE_H_
#define _USB_CONFIG_PRIVATE_H_

#include <Library/IoLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/UsbConfigLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/npa.h>
#include <Library/pmapp_npa.h>

#include <Protocol/EFIClock.h>
#include <Protocol/EFIPmicVreg.h>
#include <Protocol/EFIPmicMpp.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFII2C.h>
#include <Protocol/EFITlmm.h>
#include <Protocol/EFIPmicSmbchg.h>

#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPmicClkBuff.h>

#include "HalusbHWIO.h"
#include "HAL_hsusb.h"

#include <HALhwio.h>
#include "DDIChipInfo.h"
#include "UsbError.h"
#include <Library/MemoryAllocationLib.h>

#define PMIC_PRIM_DEVICE_INDEX_8996                                     0
#define PMIC_SEC_DEVICE_INDEX_8996                                      1

#define USB_DEV_SIGNATURE SIGNATURE_32 ('u', 's', 'b', 'c')

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
#define QMP_PHY_MAX_RW_VERIFY_ATTEMPTS   1000


#define DCTL_CSFTRST_VERIFY_ATTEMPTS   10000 



/*
 * Forward declaration of USB structure for parameter to function types.
 */
typedef struct QCOM_USB_CORE    QCOM_USB_CORE;


/**
  Perform USB core initialization common to host and device mode.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized
  @retval EFI_UNSUPPORTED   USB core not supported
  @retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_COMMON) (
  IN QCOM_USB_CORE              *UsbCore
  );


/**
  Perform USB core initialization specific to device mode. This function
  must be called after the core's common initialization function.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized in device mode
  @retval EFI_UNSUPPORTED   USB core doesn't support device mode
  @retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_DEVICE) (
  IN QCOM_USB_CORE              *UsbCore
  );


/**
  Perform USB core initialization specific to host mode. This function
  must be called after the core's common initialization function.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized in host mode
  @retval EFI_UNSUPPORTED   USB core doesn't support host mode
  @retval Others            Error encountered initializing USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_INIT_HOST) (
  IN QCOM_USB_CORE              *UsbCore
  );


/**
  Perform a hardware reset on the USB core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully reset
  @retval Others            Error encountered resetting USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_RESET) (
  IN QCOM_USB_CORE              *UsbCore
  );

/**
  Perform a low power mode sequence on the USB core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core entered low power mode sucessfully
  @retval Others            Error encountered entering low power mode for USB core
**/
typedef
EFI_STATUS
(*QCOM_USB_CORE_LPM) (
  IN QCOM_USB_CORE              *UsbCore
  );
  
  
/**
  Structure describing a clock supported by EfiClockProtocol. Clocks not
  supported by this protocol (e.x. clocks enabled through PMIC interface)
  must be handled separately. This structure is intended to be used as a
  statically defined array of clocks per core.
**/
typedef struct QCOM_USB_CLOCK {
  CONST CHAR8                   *Name;
  CONST UINT32                  Frequency;
  CONST UINT32                  Divider;
} QCOM_USB_CLOCK;


/**
  Structure encapsulating a USB core. Contains core specific initialization
  function pointers and data. No data outside this structure should be required
  by the initialization functions.
**/
struct QCOM_USB_CORE {
  QCOM_USB_CORE_INIT_COMMON     InitCommon;
  QCOM_USB_CORE_INIT_DEVICE     InitDevice;
  QCOM_USB_CORE_INIT_HOST       InitHost;
  QCOM_USB_CORE_RESET           Reset;
  QCOM_USB_CORE_LPM             CoreEnterLPM;
  QCOM_USB_CORE_LPM             CoreExitLPM;
  QCOM_USB_CONFIG_CORE_TYPE     CoreType;
  UINTN                         BaseAddr;
  UINTN                         EnClockCount;
  QCOM_USB_CLOCK                *EnClocks;
  UINTN                         DisClockCount;
  QCOM_USB_CLOCK                *DisClocks;
  UINTN                         ClkRefCnt;    // This reference count is not per clock and is only to keep tracks of enabling/disabling all USB clocks per core
  BOOLEAN                       isCoreInLPM;
};


/**
  Main structure representing USB controller
  **/
typedef struct {
  UINT32                     signature;
  EFI_HANDLE                 clientHandle;      //controller handle
  QCOM_USB_CORE              usbCore;
  QCOM_USB_CONFIG_PROTOCOL   usbConfig;
} USB_DEV;


/**
  Structure containing USB configuration data that pertains to more than 1 core
  whose contents will vary with the needs of each target. There shall be exactly
  one global instance of this structure referenced directly where needed.
**/
typedef struct QCOM_USB_CORE_CONFIGURATION {
  UINT32                        MSM8996Version;     ///< Value from QCOM_TARGET_VERSION enum
  UINT32                        UsbPort0Config;     /// Value of override for primary port
} QCOM_USB_CORE_CONFIGURATION;

/**
  Enumeration of possible values for QCOM_USB_CORE_CONFIGURATION.UsbPort0Core
**/
typedef enum USB_PORT0_CONFIG {
  USB_PORT0_CONFIG_CI_DEVICE,
  USB_PORT0_CONFIG_SNPS_HOST,
  USB_PORT0_CONFIG_SNPS_DEVICE,
  USB_PORT0_CONFIG_MAX
} USB_PORT0_CONFIG;

/**
  Enumeration of possible values for QCOM_USB_CORE_CONFIGURATION.APQ8084Version
**/
typedef enum QCOM_TARGET_VERSION{
  QCOM_MSM8996_UNKNOWN = 0,
  QCOM_MSM8996_V1,
  QCOM_MSM8996_V2,
  QCOM_MSM8996_V3,
  QCOM_MSM8996_VERSION_MAX
} QCOM_TARGET_VERSION;


/**
  USB core enumeration 
*/
typedef enum {
  USB_CORE_0_8996 = 0,      /**< Core0. */
  USB_CORE_1_8996,          /**< Core1. */
  USB_CORE_MAX_NUM_8996     /**< Invalid Core. */
} QCOM_USB_CORE_NUM_8996;

/**
  Global USB core configuration instance
**/
extern QCOM_USB_CORE_CONFIGURATION  gUsbCoreConfig;


/**
  Function used to consolidate the opening of EFI protocols required by the
  core initialization functions and prevent multiple openings of the same
  protocol. This should be called once at library initialization time.

  @retval EFI_SUCCESS   All protocols opened successfully
  @retval Others        Failed to open at least one protocol
**/
EFI_STATUS
UsbConfigLibOpenProtocols (
  VOID
  );


#endif /* _USB_CONFIG_PRIVATE_H_ */

