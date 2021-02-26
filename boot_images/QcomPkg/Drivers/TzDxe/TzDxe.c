/** @file TzDxe.c

  This file calls initializations of TZ related DXEs.

  Copyright (c) 2012-2014 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 04/29/14   sm       Re-enabled Encryption protocol
 03/18/14   sm       Disabled loading of Encryption protocol
 01/28/13   jt       Removed Listener registrations for GPT and RPMB 
 01/10/13   bbabu    Added Listener registrations for GPT and RPMB - remove later
 11/18/12   shl      Initial version

 =============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "KeyStoreLoader.h"

extern EFI_STATUS ScmProtocolInit(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable);
extern EFI_STATUS TzeLoaderProtocolInit(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable);
extern EFI_STATUS EncryptionProtocolInit(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable);

/**
  Trust Zone DXEs Entry point

  @param  ImageHandle       EFI_HANDLE.
  @param  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
**/
EFI_STATUS
EFIAPI
TzEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS              Status;

  // init SCM protocol first, other function/protocol depend on it
  Status = ScmProtocolInit(ImageHandle,SystemTable);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  // load key store from EMMC to TZ. Will not handle return here because fail to load
  // key store should not block the normal bootup, but error message will be printed
  // inside LoadKeyStore() if there is an error.
  Status = LoadKeyStore();

  // init Tz Loader protocol
  Status = TzeLoaderProtocolInit(ImageHandle,SystemTable);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  /*Initialize the Encryption Protocol*/
  Status = EncryptionProtocolInit(ImageHandle,SystemTable);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  return Status;
}


