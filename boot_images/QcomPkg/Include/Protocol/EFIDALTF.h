/**
  @file  EFIDALTF.h
  @brief DALTF Protocol for UEFI.
*/

/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/23/15   jz      Created.

=============================================================================*/
#ifndef __EFIDALTF_H__
#define __EFIDALTF_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <DDITF.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup
@{ */
/**
  Protocol version.
*/
#define EFI_DALTF_PROTOCOL_VERSION 0x0000000000010002

/*  Protocol GUID definition */

#define EFI_DALTF_PROTOCOL_GUID \
  { 0x3bec87db, 0x4e76, 0x491c, { 0x96, 0x6c, 0x7c, 0xa5, 0x81, 0x2a, 0x64, 0xc9 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the DALTF Protocol GUID.
*/
extern EFI_GUID gEfiDaltfProtocolGuid;

/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

/* EFI_TFADDTESTS */
/** @ingroup efi_TFAddTests
  @par Summary
  add test cases into test framework

  @return
  EFI_SUCCESS -- Function completed successfully.
 */
typedef EFI_STATUS (EFIAPI *EFI_DALTF_ADD_TESTS)(
  IN CONST TF_TestFunction_Descr** Tests,
  IN UINTN							Num_Tests
);
 

/* EFI_TFREMOVETESTS */
/** @ingroup efi_TFRemoveTests
  @par Summary
  remove test cases from test framework

  @return
  EFI_SUCCESS -- Function completed successfully.
 */

typedef EFI_STATUS (EFIAPI *EFI_DALTF_REMOVE_TESTS)(
  IN CONST TF_TestFunction_Descr** Tests,
  IN UINTN							Num_Tests
);
/* EFI_TFHANDLECOMMAND */
/** @ingroup efi_TFHandleCommand
  @par Summary
  process given command

  @return
  EFI_SUCCESS -- Function completed successfully.
 */
typedef EFI_STATUS (EFIAPI *EFI_DALTF_HANDLE_COMMAND)(
   IN CONST CHAR8* Request
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_daltf_protocol
  @par Summary
  DALTF Protocol interface.

  @par Parameters
  @inputprotoparams
*/
typedef struct _EFI_DALTF_PROTOCOL {
   UINT64                           Version;
   EFI_DALTF_ADD_TESTS				AddTests;
   EFI_DALTF_REMOVE_TESTS			RemoveTests;
   EFI_DALTF_HANDLE_COMMAND			HandleCommand;
}EFI_DALTF_PROTOCOL;

#endif  /* __EFIDALTF_H__ */
