/** @file TzeLoaderDxe.h
  Copyright (c) 2011-2016, Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

#ifndef QCOMPKG_DRIVERS_TZDXE_TZELOADERDXE_H_
#define QCOMPKG_DRIVERS_TZDXE_TZELOADERDXE_H_

#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/EfiFileLib.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include "QcomLib.h"
#include <Protocol/EFITzeLoader.h>
#include <Library/QcomBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIScm.h>
#include <Library/UefiCfgLib.h>
#include <Library/BootConfig.h>
#include "QseeComDxe.h"

/*------------------------------------------------------------------------
                              PUBLIC METHODS
------------------------------------------------------------------------*/

EFI_STATUS TzGetFeatureVersion (UINT32  FeatureId, UINT32 *Version);

EFI_STATUS QseeAppsRegionNotificationSyscall (
  UINT64 applications_region_addr,
  UINT64 applications_region_size
  );

EFI_STATUS
LoadImageFromPartition(
    IN  EFI_TZE_LOADER_PROTOCOL        *This,
    IN  EFI_GUID                       *pPartitionGuid,
    IN  UINT16                         *pPartitionLabel,
    OUT UINT32                         *AppIdPtr
 );

EFI_STATUS
LoadImageFromPartitionLabel(
    IN  EFI_TZE_LOADER_PROTOCOL        *This,
    IN  UINT16                         *pPartitionLabel,
    OUT UINT32                         *AppIdPtr
 );

EFI_STATUS
LoadImageFromFile(
      IN  EFI_TZE_LOADER_PROTOCOL      *This,
      IN  CHAR8*                        FilePath,
      OUT UINT32                       *AppIdPtr
    );

EFI_STATUS
LoadImageFromBuffer(
      IN  EFI_TZE_LOADER_PROTOCOL     *This,
      IN  UINT32                      *Buffer,
      IN  UINTN                        BufferSize,
      OUT UINT32                      *AppIdPtr
    );

EFI_STATUS
LoadImageFromFileSystem(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   CHAR16                             *Path,
   OUT UINT32                         *AppIdPtr

);

EFI_STATUS
LoadSecureImageFromFileSystem(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   CHAR16                             *Path,
   IN BOOLEAN                         ServicesMbn,
   OUT UINT32                         *AppIdPtr
);

EFI_STATUS
LoadExternalImageFromFileSystem(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   IN  CHAR16                         *Path,
   IN  tz_load_image_type             ImageType,
   IN VOID                            *Buffer,
   OUT UINTN                          *BufferSize,
   OUT UINT32                         *AppIdPtr
);

int DisplayQseeLog(UINT32 startOffset, UINT32 endOffset);

UINT32
QseeAppStartSyscall (
   UINTN 			      mdt_len,
   UINTN 			      img_len,
   VOID 			      *pa,
   CHAR8 			      img_name[]
);
#endif /* QCOMPKG_DRIVERS_TZDXE_TZELOADERDXE_H_ */
