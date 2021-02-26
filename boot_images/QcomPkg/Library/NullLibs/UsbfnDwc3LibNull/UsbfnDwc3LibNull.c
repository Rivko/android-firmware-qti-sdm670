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
 * @ref         "DWC_usb3_databook 2.5(a)" and "HwProgrammingGuide - usb30 and SS PHY".
 *
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
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
// $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/UsbfnDwc3LibNull/UsbfnDwc3LibNull.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $
//
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 05/09/2015   plc      Initial Revision 
// ===========================================================================

#include <Library/UsbfnDwc3Lib.h>
#include "UsbfnDwc3Private.h"
#include "HalusbHWIO.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIAdc.h>



//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------
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
  return;
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
  return;
}


