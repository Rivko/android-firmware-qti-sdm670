/** @file UsbPortTest.c

  QCOM_USB_PORT_TEST_PROTOCOL driver.

  Copyright (c) 2011 - 2013, QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/14/13   mliss    Moved into Ehci driver, added multiple instance support
 08/08/12   wufan    Add support of device mode signal quality testing.
 04/30/12   wufan    Add parameter set/get interface.
 01/04/12   wufan    Initial draft

=============================================================================*/

#include <Library/HAL_hsusb.h>
#include <Library/HAL_ssusb.h>
#include <Library/UsbConfigLib.h>
#include "Ehci.h"


#define EHC_FROM_PORTTEST_THIS(a)       CR(a, USB2_HC_DEV, UsbPortTest, USB2_HC_DEV_SIGNATURE)


/**
  Run usb port test. Host mode procedures defined in EHCI spec 4.14.
   
  @param  TestMode   Test mode 
  @param  Data       [IN] *Data =1 means device mode. [OUT] Test results for some tests.

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Unsupported test mode or data is null 
                                  when output data is expected.
  @retval EFI_NOT_FOUND           Failed to locate USB config protocol
**/
EFI_STATUS
EFIAPI
EhcRunUsbPortTest (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_MODE         TestMode,
  IN OUT UINTN                        *Data
  );


/**
  Set parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type not valid.
**/
EFI_STATUS
EhcPortTestSetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            ParamValue
  );


/**
  Get parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type or pointer not valid.
**/
EFI_STATUS
EhcPortTestGetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            *ParamValue
  );


/**
  Uniquely identify an instance of this protocol.
   
  @param  This  Protocol instance pointer

  @retur Identification string
**/
CHAR16 *
EhcPortTestIdentify (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This
  );

//
// The global UsbPortTest protocol instance for Ehci driver.
//
QCOM_USB_PORT_TEST_PROTOCOL gEhciUsbPortTestTemplate = {
  QCOM_USB_PORT_TEST_PROTOCOL_REVISION,
  EhcRunUsbPortTest,
  EhcPortTestGetParam,
  EhcPortTestSetParam,
  EhcPortTestIdentify,
  -1
};


//
// Protocol implementation
//


EFI_STATUS
EFIAPI
EhcRunUsbPortTest (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_MODE         TestMode,
  IN OUT UINTN                        *Data )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  USB2_HC_DEV               *Ehc;
  UINTN                     DevCoreNum;
  QCOM_USB_CONFIG_CORE_TYPE CoreType;
  UINT32                    BaseAddr = 0;

  if (NULL == This) {
    DBG(EFI_D_ERROR, "invalid parameter");
    return EFI_INVALID_PARAMETER;
  }

  Ehc = EHC_FROM_PORTTEST_THIS(This);

  // Get core base address for this instance
  Status = Ehc->PciIo->GetLocation (Ehc->PciIo, NULL, NULL, &DevCoreNum, NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Ehc->UsbConfig->GetUsbHostConfig (Ehc->UsbConfig,
                                             USB_HOST_MODE_EHCI,
                                             (QCOM_USB_CORE_NUM)DevCoreNum,
                                             &CoreType);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Status = Ehc->UsbConfig->GetCoreBaseAddr (Ehc->UsbConfig,
                                            CoreType,
                                            (UINTN *)&BaseAddr);

  ERR_CHK ( "unable to obtain Usb core base address" );

  DBG (EFI_D_INFO, "USB port test mode 0x%x", TestMode);

  switch ( TestMode ) {
  case USB_PORT_TEST_GET_DISCONNECT_STATUS:
          //retrieve the connection status
          if ( Data == NULL ) {
              return EFI_INVALID_PARAMETER;
          }
          *Data = HAL_HSUSB_READ ( BaseAddr, HAL_HSUSB_PORTSC_ADDR(0) ) & HAL_HSUSB_PORTSC_CCS_BMSK ;
          break;
    
  case USB_PORT_TEST_DISABLE_PERIODIC:
          // Disable periodic schedule
          HAL_HSUSB_AND (BaseAddr, HAL_HSUSB_USBCMD_ADDR, ~HAL_HSUSB_USBCMD_PSE_BMSK);
          break;
    
  case USB_PORT_TEST_ENABLE_PERIODIC:
          // Enable periodic schedule
          HAL_HSUSB_OR (BaseAddr, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_PSE_BMSK);
          break;
    
  case USB_PORT_TEST_DISABLE:
          //clear the port test bits
          HAL_HSUSB_AND  ( BaseAddr, HAL_HSUSB_PORTSC_ADDR(0),  ~HAL_HSUSB_PORTSC_PTC_BMSK );
    
          // if halted set run/stop bit to one
          if ( HAL_HSUSB_READ (BaseAddr, HAL_HSUSB_USBSTS_ADDR ) & HAL_HSUSB_USBSTS_HCH_BMSK ) {
              HAL_HSUSB_OR (BaseAddr, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK);
          }
    
          // Enable async schedule
          HAL_HSUSB_OR (BaseAddr, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASE_BMSK);
    
          // Enable periodic schedule
          HAL_HSUSB_OR (BaseAddr, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_PSE_BMSK);
          break;
    
  case USB_PORT_TEST_J_STATE:
  case USB_PORT_TEST_K_STATE:
  case USB_PORT_TEST_SE0_NAK:
  case USB_PORT_TEST_PACKET:
  case USB_PORT_TEST_FORCE_ENABLE:
       
          if ( Data == NULL || *Data != 1)  //check for host mode
          {
              // Disable async schedule
              HAL_HSUSB_AND (BaseAddr, HAL_HSUSB_USBCMD_ADDR, ~HAL_HSUSB_USBCMD_ASE_BMSK);
        
              // Disable periodic schedule
              HAL_HSUSB_AND (BaseAddr, HAL_HSUSB_USBCMD_ADDR, ~HAL_HSUSB_USBCMD_PSE_BMSK);
        
              // Set the port to suspend state
              HAL_HSUSB_AND  ( BaseAddr, HAL_HSUSB_PORTSC_ADDR(0),  ~HAL_HSUSB_PORTSC_SUSP_BMSK );
        
              // Set the Run/Stop bit in the USBCMD register to zero
              HAL_HSUSB_AND (BaseAddr, HAL_HSUSB_USBCMD_ADDR, ~HAL_HSUSB_USBCMD_RS_BMSK);
        
              // wait for the HCHalted bit in the USBSTS register to transition to one
              do {
                  gBS->Stall (100);
              } while ( ! (HAL_HSUSB_READ (BaseAddr, HAL_HSUSB_USBSTS_ADDR ) & HAL_HSUSB_USBSTS_HCH_BMSK) );
          }
    
          // Set Test Mode
          HAL_HSUSB_AND  ( BaseAddr, HAL_HSUSB_PORTSC_ADDR(0),  ~HAL_HSUSB_PORTSC_PTC_BMSK );
          HAL_HSUSB_OR   ( BaseAddr, HAL_HSUSB_PORTSC_ADDR(0),  TestMode << HAL_HSUSB_PORTSC_PTC_SHFT );
    
          if ( ( Data==NULL || *Data != 1) && TestMode == USB_PORT_TEST_FORCE_ENABLE ) {
              // Set the Run/Stop bit in the USBCMD register to one, host mode only
              HAL_HSUSB_OR (BaseAddr, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK);
          }
          break;

  default:
      DBG (EFI_D_WARN, "invalid USB port test mode");
      Status = EFI_INVALID_PARAMETER;
      break;
    
  }

ON_EXIT: 
  return Status;
}


EFI_STATUS
EhcPortTestSetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            ParamValue
  )
{
  if (NULL == This) {
    return EFI_INVALID_PARAMETER;
  }

  switch ( ParamType )
  {
    case USB_PORT_TEST_PARAM_QTD_INDEX: 
      This->QtdIndex = ParamValue;
      break;
    default:
      return EFI_INVALID_PARAMETER;     
  }
  return EFI_SUCCESS;
}



/**
  Get parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type or pointer not valid.

**/
EFI_STATUS
EhcPortTestGetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            *ParamValue
  )
{
  if (NULL == This || NULL == ParamValue) {
    return EFI_INVALID_PARAMETER;
  }

  switch ( ParamType )
  {
    case USB_PORT_TEST_PARAM_QTD_INDEX: 
      *ParamValue = This->QtdIndex;
      break;
    default:
      return EFI_INVALID_PARAMETER;     
  }
  return EFI_SUCCESS;
}


CHAR16 *
EhcPortTestIdentify (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This
  )
{
  USB2_HC_DEV     *Ehc;

  if (NULL == This) {
    DBG (EFI_D_ERROR, "invalid parameter");
    return L"EHCI PortTest, unknown instance";
  }

  Ehc = EHC_FROM_PORTTEST_THIS (This);

  return Ehc->UsbPortTestIdentity;
}


