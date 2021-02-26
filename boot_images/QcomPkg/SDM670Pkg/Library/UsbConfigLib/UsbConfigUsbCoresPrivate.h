/** @file \UsbConfigUsbCoresPrivate.h

  The private header file for UsbConfigUsbCore.c
  Copyright (c) 2016-2017 QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 04/26/17   jc       Added missing PMI8998 check to switch primary to SS3 LDO
 03/17/17   pm       Added configuration for sec Clock and core
 02/03/17   pm       Vbus Enablement in USB Config Protocol
 02/03/17   pm       Port from Boot.x.f.1.4
 09/22/16   amitg    Napali Pre-Si Updates
 02/18/16   ck       Init Check-In
=============================================================================*/

#ifndef _USB_CONFIG_USB_CORES_PRIV_H_
#define _USB_CONFIG_USB_CORES_PRIV_H_

#include "UsbConfigUsbCoresPublic.h"
#include "comdef.h"
#include "qusb_dci_common.h"

#include <Protocol/EFIClock.h>

#include <Protocol/EFIPmicVreg.h>
#include <Protocol/EFIPmicVersion.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFIPmicClkBuff.h>

#include <Protocol/EFII2C.h>
#include <Protocol/EFITlmm.h>

#include <Protocol/EFIChipInfo.h>

#include <Library/IoLib.h>
#include "HALhwio.h"
#include "HalusbHWIO.h"

#include <Library/npa.h>
#include <Library/pmapp_npa.h>

#define QMP_PHY_MAX_RW_VERIFY_ATTEMPTS    1000

//
// USB Core Offset Wrappers
//
#define HAL_USB_READ(BaseAddr, RegAddr) \
  MmioRead32 ((BaseAddr) + (RegAddr))
#define HAL_USB_WRITE(BaseAddr, RegAddr, Data) \
  MmioWrite32 ((BaseAddr) + (RegAddr), Data)
#define HAL_USB_OR(BaseAddr, RegAddr, Data) \
  MmioOr32 ((BaseAddr) + (RegAddr), Data)
#define HAL_USB_AND(BaseAddr, RegAddr, Data) \
  MmioAnd32 ((BaseAddr) + (RegAddr), Data)
#define HAL_USB_OUTM(BaseAddr, RegAddr,Mask, Data) \
  MmioAndThenOr32 ((BaseAddr) + (RegAddr), (~Mask), Data)

//
// Max trial to cheick QMP PHY lock up.
//
#define QMP_PHY_MAX_RW_VERIFY_ATTEMPTS    1000

//
// SD845 RUMI USB HS PHY definition for Primary Core.
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
// SD845 RUMI USB HS PHY definition for Secondary Core.
//

#define USER_REG_BASE            	   (0xa920000)
#define TI_BASE_SEC                    (0xa928000)

#define HWIO_HS2_EMU_VERSION_ADDR                        (USER_REG_BASE+0x00000000)
#define HWIO_HS2_LOCAL_RESET_REG_ADDR                    (USER_REG_BASE+0x00000004)
#define HWIO_HS2_CLK_STATUS_ADDR                         (USER_REG_BASE+0x00000010)
#define HWIO_HS2_CLK_STATUS_SEL_ADDR                     (USER_REG_BASE+0x00000014)
#define HWIO_HS2_IMEM_DELAY_CTRL_ADDR                    (USER_REG_BASE+0x00000100)
#define HWIO_HS2_IMEM_DELAY_MIN_DELAY_ADDR               (USER_REG_BASE+0x00000104)
#define HWIO_HS2_USB_CLK_CTRL_ADDR                       (USER_REG_BASE+0x00000020)
#define HWIO_HS2_USB_CLK_STATUS_ADDR                     (USER_REG_BASE+0x00000024)
#define HWIO_HS2_QRBTC_CONTROL_ADDR                   	 (USER_REG_BASE+0x00000028)
#define HWIO_HS2_QRBTC_CONTROL_OE_ADDR           		 (USER_REG_BASE+0x0000002c)
#define HWIO_HS2_USB30_CTRL_ADDR                         (USER_REG_BASE+0x00000034)
#define HWIO_HS2_USB30_STATUS_ADDR                       (USER_REG_BASE+0x00000038)
#define HWIO_HS2_USB_PIPE3_CLK_CTRL_ADDR     	         (USER_REG_BASE+0x0000003c)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_RISE_ADDR         (USER_REG_BASE+0x00000040)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_RISE_ADDR 		 (USER_REG_BASE+0x00000044)
#define HWIO_HS2_USB30_IRQ_DELAY_CTRL_ADDR    			 (USER_REG_BASE+0x00000048)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_FALL_ADDR 		 (USER_REG_BASE+0x00000060)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_FALL_ADDR 		 (USER_REG_BASE+0x00000064)
#define HWIO_HS2_SETUP_VERSION_ADDR                      (USER_REG_BASE+0x00000050)
#define HWIO_HS2_USB_UNIPHY_CLK_CTRL_ADDR      			 (USER_REG_BASE+0x00000054)
#define HWIO_HS2_ROM_STRING_SIZE_ADDR                    (USER_REG_BASE+0x00010004)
#define HWIO_HS2_ROM_STRING_START_ADDR        		     (USER_REG_BASE+0x00010008)

//
// Get QCOM_USB_CORE from QCOM_USB_CORE_INTERFACE
// 
#define USB_CORE_FROM_IFC(Entry)    BASE_CR(Entry, QCOM_USB_CORE, CoreInterface)

//
// VSTATUS Bit Mask
//
#define VSEL_4_VSTS_PLL_LOCK_BMSK       0x1
#define VSEL_4_VSTS_AFE_CDR_DATAIN_BMSK 0x2
#define VSEL_4_VSTS_AFE_SQLT120_BMSK    0x4
#define VSEL_4_VSTS_CORE_SQUELCH_BMSK   0x8

/*******************************************************************************
 * USB core initialization functions for core structures
 ******************************************************************************/

/**
Performs initialization common to host and device mode on a Synopsys
super speed USB primary core.

@param [in]  This         The USB core interface instance
@param [in]  ResetUsbOnly If TRUE, only reset the USB link and PHY
                           If FALSE, in addition to the USB link and PHY reset
                           Enable clocks and power rails

@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core

@see QCOM_USB_CORE_INIT_COMMON
**/
STATIC
EFI_STATUS
SSUsb1InitCommon(
  IN  QCOM_USB_CORE_INTERFACE         *This,
  IN  BOOLEAN                          ResetUsbOnly
  );

/**
Performs host mode specific initialization on a Synopsys super speed USB core.

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core

@see QCOM_USB_CORE_INIT_HOST
**/
STATIC
EFI_STATUS
Usb30InitHost (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );

/**
Performs device mode specific initialization on the primary Synopsys SuperSpeed
USB core.

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core

@see QCOM_USB_CORE_INIT_DEVICE
**/
STATIC
EFI_STATUS
Usb30InitDevice (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );

/**
Perform a hardware reset on a Synopsys USB core 1.

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       USB core successfully reset
@retval Others            Error encountered resetting USB core

@see QCOM_USB_CORE_RESET
**/
STATIC
EFI_STATUS
SSUsb1Reset (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );

/**
Performs low power mode for the USB hardware
Disable USB primary clocks

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       USB core successfully disabled USB SNPS Core clocks
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered while disabling USB SNPS Core clocks

@see QCOM_USB_CORE_LPM
**/
STATIC
EFI_STATUS
SSUsb1EnterLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );
  

/**
Exits low power mode for the USB hardware

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

@see QCOM_USB_CORE_LPM
**/
STATIC
EFI_STATUS
Usb30ExitLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );


/**
Set which CC line (CC1 or CC2) to be used for SS connection.

LaneNum 0 indicates that CC1 is used for USB.
LaneNum 1 indicates that CC2 is used for USB.

@param [in]  This         The USB core interface instance
@param [in]  CcStatus     CC out status

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.
**/
STATIC
EFI_STATUS
Usb30SetPhyLane(
  IN QCOM_USB_CORE_INTERFACE           *This,
  IN UINT8                              LaneNum
  );


/**
Get the base address of the USB core

@param [in]  This         The USB core interface instance
@param [out] BaseAddr     The base address of the USB core

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

@see QCOM_USB_CORE_GET_BASE_ADDR

**/
STATIC
EFI_STATUS
Usb30GetBaseAddr(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  OUT  UINTN                             *BaseAddr
  );


/**
Reset SS USB PHY

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

@QCOM_USB_CORE_RESET_SS_USB_PHY
**/
STATIC
EFI_STATUS
Usb30ResetSSPhy(
  IN   QCOM_USB_CORE_INTERFACE           *This
  );

/**
Put the PHY in or out of the Non-driving mode

@param [in]  This           The USB core interface instance
@param [in]  NonDrivingMode TRUE  - Put the PHY in the Non-Driving mode
                            FALSE - Put the PHY out of the Non-Driving mode

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

**/
STATIC
EFI_STATUS
SSUsb1SetPhyNonDrivingMode(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  IN   BOOLEAN                            NonDrivingMode
  );

/**
Put the PHY turn OFF

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

**/
STATIC
EFI_STATUS
SSUsb1InitHSUSBPhyKeepPwrDwn (
  IN QCOM_USB_CORE_INTERFACE		   *This
  );
  
/**
Get the Vbus State for the Core in host mode

@param [in]  This           The USB core interface instance
@param [out]  VbusState     Vbus State per this enum type
                            VBUS_STATUS_ENABLED = 0,
                            VBUS_STATUS_DISABLED,
                            VBUS_STATUS_UNSUPPORTED


@retval EFI_SUCCESS        The operation succeeded.
@retval EFI_UNSUPPORTED    Operation Unsupported
@retval Others                    Failure
**/

STATIC
EFI_STATUS
Usb30GetVbusSt (
  IN  QCOM_USB_CORE_INTERFACE         *This,
  OUT UINT32                          *VbusSt
  );


/**
Enable the Vbus State for the Core in host mode

@param [in]  This           The USB core interface instance

@retval EFI_SUCCESS        The operation succeeded.
@retval EFI_UNSUPPORTED    Operation Unsupported
@retval Others             Failure
**/
STATIC
EFI_STATUS
Usb30EnableVbus (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );

/**
Performs initialization common to host and device mode on a Synopsys
super speed USB secondary core.

@param [in]  This         The USB core interface instance
@param [in]  ResetUsbOnly If TRUE, only reset the USB link and PHY
                           If FALSE, in addition to the USB link and PHY reset
                           Enable clocks and power rails

@retval EFI_SUCCESS       USB core successfully initialized
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered initializing USB core

@see QCOM_USB_CORE_INIT_COMMON
**/
STATIC
EFI_STATUS
SSUsb2InitCommon(
  IN  QCOM_USB_CORE_INTERFACE         *This,
  IN  BOOLEAN                          ResetUsbOnly
  );

/**
Performs low power mode for the USB hardware
Disable USB Secondary clocks

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       USB core successfully disabled USB SNPS Core clocks
@retval EFI_UNSUPPORTED   USB core not supported
@retval Others            Error encountered while disabling USB SNPS Core clocks

@see QCOM_USB_CORE_LPM
**/
STATIC
EFI_STATUS
SSUsb2EnterLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  );

/**
Put the PHY in or out of the Non-driving mode

@param [in]  This           The USB core interface instance
@param [in]  NonDrivingMode TRUE  - Put the PHY in the Non-Driving mode
                            FALSE - Put the PHY out of the Non-Driving mode

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

**/
STATIC
EFI_STATUS
SSUsb2SetPhyNonDrivingMode(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  IN   BOOLEAN                            NonDrivingMode
  );

/**
Put the PHY turn OFF

@param [in]  This         The USB core interface instance

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.

**/
STATIC
EFI_STATUS
SSUsb2InitHSUSBPhyKeepPwrDwn (
  IN QCOM_USB_CORE_INTERFACE		   *This
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
function pointers and data. The function pointers defined in CoreInterface
are the only public interface which can be accessed outside the USB Core
module. No data outside this structure should be required
by the initialization functions.
**/
typedef struct _QCOM_USB_CORE {
  QCOM_USB_CORE_INTERFACE       CoreInterface;
  QCOM_USB_CONFIG_CORE_TYPE     CoreType;
  UINTN                         BaseAddr;
  UINTN                         EnClockCount;
  QCOM_USB_CLOCK                *EnClocks;
  UINTN                         DisClockCount;
  QCOM_USB_CLOCK                *DisClocks;
  UINTN                         ClkRefCnt;    // This reference count is not per clock and is only to keep tracks of enabling/disabling all USB clocks per core
  BOOLEAN                       IsCoreInLPM;
  UINTN                         LaneNum;
} QCOM_USB_CORE;

#endif /* _USB_CONFIG_USB_CORES_PRIV_H_ */
