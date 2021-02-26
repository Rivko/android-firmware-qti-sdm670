/**
  @file EFIAcpiPlatformTest.h
  @brief Declaration of AcpiPlatform DXE Driver Test Interface
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc., Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 02/14/14    swi	Added test register to protocol to enable callback testing

=============================================================================*/
#ifndef __EFI_ACPIPLATFORMTEST_H__
#define __EFI_ACPIPLATFORMTEST_H__

/*===========================================================================
  DATA TYPE DEFINITIONS
===========================================================================*/
/** @ingroup efi_acpiPlatformTest_protocol */
extern EFI_GUID gQcomAcpiPlatformTestProtocolGuid;

/** @ingroup efi_acpiPlatformTest_data_types */

/** @cond */
typedef struct _EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL;
/** @endcond */

/** @ingroup efi_acpiPlatformTest_constants */
/** Protocol version. */
#define ACPIPLATFORMTEST_REVISION 0x0000000000010000

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ACPI_PLATFORM_REGISTER_TEST */
/** @ingroup efi_acpi_platform_register_test
  @par Test of the register callback function to support ACPI table query/update services.
  
  @param[in]   This           Pointer to EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL instance.
  @param[in]   TestCase       ACPITESTCASE enum value for test case being registered 						  
							  
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_ACPI_PLATFORM_REGISTER_TEST)(
  IN EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL    *This,
  IN UINT8    						 	    TestCase
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_acpiPlatformTest_protocol
  @par Summary
  Qualcomm Advanced Configuration and Power Interface (ACPI) Platform Protocol 
  Test interface.

  @par Parameters
  @inputprotoparams{acpi_platform_test_proto_params.tex} 
*/
struct _EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL {
   UINT64                                            Revision;
   EFI_ACPI_PLATFORM_REGISTER_TEST                   AcpiTableRegisterTest;
}; 

#endif
