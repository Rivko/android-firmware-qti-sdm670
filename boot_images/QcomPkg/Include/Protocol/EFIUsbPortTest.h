/** @file EFIUsbPortTest.h
   
  QCOM_USB_PORT_TEST_PROTOCOL definition. 
  This protocol allows user interface to set up USB HW for host mode 
  electrical testing. 

  Copyright (c) 2011 - 2017, Qualcomm Technologies,, Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/04/17   gk	     UEFI USB compliance test changes
 01/04/12   wufan    Initial draft.
 04/30/12   wufan    Adding parameter get/set interface for port test.
 08/08/12   wufan    Add support of device mode signal quality testing.
 06/17/13   mliss    Added Identify function to support multiple instances
 
=============================================================================*/

#ifndef _EFI_USB_PORT_TEST_PRO_H_
#define _EFI_USB_PORT_TEST_PRO_H_

///
/// Forward reference for pure ANSI compatibility.
///
typedef struct _QCOM_USB_PORT_TEST_PROTOCOL  QCOM_USB_PORT_TEST_PROTOCOL;


/**
  Enumeration for Port Test Mode.
**/
typedef enum {
  USB_PORT_TEST_DISABLE,
  USB_PORT_TEST_J_STATE,
  USB_PORT_TEST_K_STATE,
  USB_PORT_TEST_SE0_NAK,
  USB_PORT_TEST_PACKET,
  USB_PORT_TEST_FORCE_ENABLE,
  USB_PORT_TEST_ENABLE_SS_COMPLIANCE,
  USB_PORT_TEST_GET_DISCONNECT_STATUS,
  USB_PORT_TEST_DISABLE_PERIODIC,
  USB_PORT_TEST_ENABLE_PERIODIC,
  USB_PORT_TEST_MODE_MAX
} QCOM_USB_PORT_TEST_MODE;


/**
   Enumeration for Port Test Parameters.
 **/
typedef enum {

  USB_PORT_TEST_PARAM_QTD_INDEX,
  USB_PORT_TEST_PARAM_MAX

} QCOM_USB_PORT_TEST_PARAM;


/**
  Run usb port test. Host mode procedures defined in EHCI spec 4.14.
   
  @param  TestMode   Test mode 
  @param  Data       [IN] *Data =1 means device mode. [OUT] Test results for some tests.

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Unsupported test mode or data is null 
                                  when output data is expected.
  @retval EFI_NOT_FOUND           Failed to locate USB config protocol

**/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_PORT_TEST_PROTOCOL_RUN_TEST) (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_MODE         TestMode,
  IN  OUT UINTN                       *Data 
  );




/**
  Set parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type not valid.

**/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_PORT_TEST_PROTOCOL_SET_PARAM) (
    IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
    IN  QCOM_USB_PORT_TEST_PARAM         ParamType,
    IN  INTN                             ParamValue
  );



/**
  Get parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type or pointer to value not valid.

**/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_PORT_TEST_PROTOCOL_GET_PARAM) (
    IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
    IN  QCOM_USB_PORT_TEST_PARAM         ParamType,
    OUT INTN                            *ParamValue
  );



/**
  Uniquely identify an instance of this protocol.

  @param This       Protocol instance pointer

  @return Identification string
**/
typedef
CHAR16 *
(EFIAPI *QCOM_USB_PORT_TEST_PROTOCOL_IDENTIFY) (
    IN  QCOM_USB_PORT_TEST_PROTOCOL     *This
  );



#define QCOM_USB_PORT_TEST_PROTOCOL_REVISION  0x00010002


///
/// QCOM_USB_PORT_TEST_PROTOCOL
///
struct _QCOM_USB_PORT_TEST_PROTOCOL {
  UINT32                                    Revision;
  QCOM_USB_PORT_TEST_PROTOCOL_RUN_TEST      RunPortTest;  
  QCOM_USB_PORT_TEST_PROTOCOL_GET_PARAM     GetParam;
  QCOM_USB_PORT_TEST_PROTOCOL_SET_PARAM     SetParam;
  QCOM_USB_PORT_TEST_PROTOCOL_IDENTIFY      Identify;
  INTN                                      QtdIndex;
};


extern EFI_GUID gQcomUsbPortTestProtocolGuid;

#endif




