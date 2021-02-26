//============================================================================
/**
 * @file        UsbfnDwc3Lib.c
 * @author      kameya
 * @date        15-Feb-2013
 *
 * @brief       Platform library for the Synopsys Usbfn driver (UsbfnDwc3)
 *
 * @details     An instance of this library provides platform specific functionallity
 *              for the UsbfnDwc3 driver in order for it to remain platform independed.
 *              This is currently only used for charger detection.
 *
 * @ref         "DWC_usb3_databook 3.0(a)" and "HwProgrammingGuide - usb30 and SS PHY".
 *
 *              Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 *
 */
//============================================================================

// ===========================================================================
//
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
//
//
// when         who      what, where, why
// ----------   -----    ----------------------------------------------------------
// 04/04/17     pm       Removed Explicit Addressing from HalusbHWIO header file   
// 12/20/16     sm       Edited input parameter list for ChargerPort PmicUsbProtocol API
// 09/22/16     amitg    Napali Pre-Si Updates
// 02/23/16     ck       deprecate dci_dwc_detect_charger_type and add UsbfnDwc3GetChargerPortType
// 10/26/15     ck       Init 8998 Pre-Si support
// ===========================================================================

#include <Library/UsbfnDwc3Lib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "UsbfnDwc3Private.h"
#include "HalusbHWIO.h"
#include <Protocol/EFIPmicUsb.h>

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------

CHAR8* dci_dwc_print_charger_type(dci_dwc_chg_port_type charger_type) {

    switch(charger_type) {
        case DCI_DWC_CHG_PORT_SDP:
            return "Standard Downstream Port (SDP)";
        case DCI_DWC_CHG_PORT_CDP:
            return "Charging Downstream Port (CDP)";
        case DCI_DWC_CHG_PORT_DCP:
            return "Dedicated Charging Port (DCP)";
        case DCI_DWC_CHG_PORT_INVALID:
            return "Invalid charger";
        case DCI_DWC_CHG_PORT_UNKNOWN:
        default:
            return "Unknown charger";
    }

}

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

//============================================================================
/**
 * @function  dci_enable_external_vbus_config
 *
 * Enables external VBUS configuration. This in turn manually drives D+ pull-up.
 *
 *
 *
 * @param Core Qscratch Base Address
 *
 *
 *
 * @return none
 */
//=============================================================================
static void dci_enable_external_vbus_config(UINTN QscratchBase)
{
    /* Unused parameters */
	(void)QscratchBase;
  
 /*
    *Reference USB 3.0 HPG (Oct, 2014)  Default for Primary Core
    */
    //1.    Software updates HS_PHY_CTRL.UTMI_OTG_VBUS_VALID. 
    HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x1 << HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
    
    //2.    Software updates HS_PHY_CTRL.SW_SESSVLD_SEL to 1 indicating that the value is driven by UTMI_OTG_VBUS_VALID.
    HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x1 << HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));
    
    //3.    Software set lane0_power_present
    HWIO_USB3_PRI_SS_PHY_CTRL_OUTM(HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK, (0x1 << HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT)); 
}


//============================================================================
/**
 * @function  dci_enable_external_vbus_config
 *
 * Disables external VBUS configuration. This in turn manually drives D+ pull-down.
 *
 *
 *
 * @param Core Qscratch Base Address
 *
 *
 *
 * @return none
 */
//=============================================================================
static void dci_disable_external_vbus_config(UINTN QscratchBase)
{
    /* Unused parameters */
	(void)QscratchBase;
	
 /*
    *Reference USB 3.0 HPG (Oct, 2014)  Default for Primary Core
    */

    //1.    Software updates HS_PHY_CTRL.UTMI_OTG_VBUS_VALID. 
    HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x0 << HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
    
    //2.    Software updates HS_PHY_CTRL.SW_SESSVLD_SEL to 1 indicating that the value is driven by UTMI_OTG_VBUS_VALID.
    HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x0 << HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));
    
    //3.    Software clear lane0_power_present
    HWIO_USB3_PRI_SS_PHY_CTRL_OUTM(HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK, (0x0 << HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT)); 
}


//============================================================================
/**
 * @function  dci_connect
 *
 * @brief Connect the device to the USB bus.
 *
 * @Note :  Connect/Disconnect is done via PMIC
 *          It can be done via the USB core when using PIPE3 PHY.
 *
 * @param Core Qscratch Base Address
 *
 * @return none
 *
 * @ref USB 3.0 Controller HPG:
 *      Chapter 4.4.2.8 - ID and VBUS override 
 *
 */
//============================================================================
void dci_connect(UINTN QscratchBase)
{
    dci_enable_external_vbus_config(QscratchBase);
}


//============================================================================
/**
 * @function  dci_disconnect
 *
 * @brief Disconnect the device from the USB bus.
 *
 * @Note :  Connect/Disconnect is done via PMIC
 *          It can be done via the USB core when using PIPE3 PHY.
 *
 * @param Core Base Address
 *
 * @return none
 *
 * @ref USB 3.0 Controller HPG:
 *      Chapter 4.4.2.8 - ID and VBUS override 

 *
 */
//============================================================================
void dci_disconnect(UINTN QscratchBase)
{
    dci_disable_external_vbus_config(QscratchBase);
}

/**
 * @function  dci_dwc_detect_charger_type
 *
 * Returns the dci_dwc_chg_port_type based charger detection algorithm
 *
 *
 * @param Addr
 *
 * @return dci_dwc_chg_port_type - Charger port type.
 *
 * @deprecated
 */
dci_dwc_chg_port_type dci_dwc_detect_charger_type(uint32 core_id)
{
  return DCI_DWC_CHG_PORT_UNKNOWN;
}

EFI_STATUS
UsbfnDwc3GetChargerPortType(
  IN  UINT32 CoreId, 
  OUT USB_CHARGER_PORT_TYPE *ChargerPortType) 
{

  EFI_STATUS Status = EFI_SUCCESS;
  static EFI_QCOM_PMIC_USB_PROTOCOL *PmicUsbProtocol = NULL;
  EFI_PM_USB_CHGR_PORT_TYPE ChgPortType = EFI_PM_USB_CHG_PORT_INVALID;
	
  if (NULL == PmicUsbProtocol){
    // Find the PMIC USB charger protocol
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&PmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      PmicUsbProtocol = NULL;
      DEBUG((EFI_D_ERROR, "Failed to open PMIC USB protocol Status =  (0x%x)\r\n", Status));
      goto ON_EXIT;
    }
  }

  Status = PmicUsbProtocol->ChargerPort(&ChgPortType);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Failed to call PMIC ChargerPort Status =  (0x%x)\r\n", Status));
    goto ON_EXIT;
  }

  switch (ChgPortType)
  {
  case EFI_PM_USB_CHG_PORT_CDP:
    *ChargerPortType = DCI_DWC_CHG_PORT_CDP;
    break;
  case EFI_PM_USB_CHG_PORT_DCP:
    *ChargerPortType = DCI_DWC_CHG_PORT_DCP;
    break;
  case EFI_PM_USB_CHG_PORT_SDP:
    *ChargerPortType = DCI_DWC_CHG_PORT_SDP;
    break;
  case EFI_PM_USB_CHG_PORT_INVALID:
    *ChargerPortType = DCI_DWC_CHG_PORT_UNKNOWN;
    break;
  case EFI_PM_USB_CHG_PORT_OTHER:
    *ChargerPortType = DCI_DWC_CHG_PORT_INVALID;
    break;
  default:
    *ChargerPortType = DCI_DWC_CHG_PORT_UNKNOWN;
  }

ON_EXIT:
  return Status;
}

