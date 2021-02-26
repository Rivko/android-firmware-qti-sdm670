/** @file QcomLib.h
   
  Contains Qualcomm utility library functions.

  Copyright (c) 2013, 2015, Qualcomm Technologies, Inc. All rights reserved. 
  Portions Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.
   
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/02/15   vk      Added LoadDebugFv
 07/11/13   yg      Created

=============================================================================*/
#ifndef __QCOMUTILSLIB_H__
#define __QCOMUTILSLIB_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Uefi.h>
#include <Library/BaseLib.h>

/**
  Start an EFI image (PE32+ with EFI defined entry point).

  Argv[0] - device name and path
  Argv[1] - "" string to pass into image being started

  fs1:\Temp\Fv.Fv "arg to pass" ; load an FV from the disk and pass the
                                ; ascii string arg to pass to the image
  fv0:\FV                       ; load an FV from an FV (not common)
  LoadFile0:                    ; load an FV via a PXE boot

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line.
                 Argv[0] is the App to launch

  @return EFI_SUCCESS

**/
EFI_STATUS
LaunchApp (IN UINTN  Argc, IN CHAR8  **Argv);

/**
  Launch menu using the config file specified. First search will be performed
  in the removable media, then in the specified guided fv, then looked
  into the FV1. Config file defaults to BDS_Menu.cfg if not provided

  MenuFileName - Menufile name

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line.
                 Argv[0] is the App to launch

  @return EFI_SUCCESS

**/
EFI_STATUS
LaunchMenu (CHAR8* MenuFileName OPTIONAL);

EFI_STATUS
LoadFromFV (IN   CHAR8*    Name, 
            OUT  UINT8**   FileBuffer,
            OUT  UINTN*    FileSize);

/**
  Converts a string to GUID value.
  Guid Format is xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

  @param Str              The registry format GUID string that contains the GUID value.
  @param Guid             A pointer to the converted GUID value.

  @retval EFI_SUCCESS     The GUID string was successfully converted to the GUID value.
  @retval EFI_UNSUPPORTED The input string is not in registry format.
  @return others          Some error occurred when converting part of GUID value.

**/
EFI_STATUS
StrToGuid (
  IN  CHAR16   *Str,
  OUT EFI_GUID *Guid
  );

#endif /* __QCOMUTILSLIB_H__ */
