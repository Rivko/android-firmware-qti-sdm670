/** @file
  Header file for Guided FV specific code
   
  Copyright (c) 2011, 2016, 2017 Qualcomm Technologies, Inc. All rights reserved. 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Support for authenticated FV loading
 10/31/16   bh      Add required inclusions
 04/19/16   bh      Add cmd line arg support
 03/30/16   bh      Initial revision

=============================================================================*/
#ifndef _GUIDED_FV_SUPPORT_H_
#define _GUIDED_FV_SUPPORT_H_

#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/FirmwareVolume2.h>

/* Launches an app from a guided FV. Depends on FV being
   accessible as a simple file system

   FvGuid     Guid of FV to launch app from
   AppName    Ascii name of app to launch
   Args       Ascii command line args to pass to app */
EFI_STATUS 
LaunchAppFromGuidedFv(EFI_GUID *FvGuid, 
                      CHAR8    *AppName, 
                      CHAR8    *Args);

/* Reads a file into buffer from a specified guided FV. 
   Buffer is allocated within function and FileSize contains
   size read in bytes. If FvGuid is NULL, it searches in all
   loaded FVs.

   FvGuid     Guid of FV to launch app from
   FileName   File to be read 
   FileBuffer Buffer to to be read into
   FileSize   Size in bytes of file read */
EFI_STATUS
LoadFileFromGuidedFv (EFI_GUID   *FvGuid,
                      CHAR8      *FileName,
                      OUT UINT8 **FileBuffer,
                      OUT UINTN  *FileSize);

/* Checks if the FV matching "FvGuid" has been mounted */
BOOLEAN
GuidedFvIsMounted(EFI_GUID *FvGuid);

/* Loads and authenticates the given FV, using the TZ PIL commands 

   FvGuid           GUID of the FV to be loaded and authenticated
                    Returns error if FvGuid is NULL
   PartitionLabel   Partition name as in partition.xml  */
EFI_STATUS
LoadAndAuthenticateFV(EFI_GUID *FvGuid,
                      CHAR16   *PartitionLabel);

#endif // _GUIDED_FV_SUPPORT_H_
