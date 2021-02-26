/**
  @file EFITzeLoader.h
  @brief TZ exec application loader EFI protocol interface.
*/
/*=============================================================================
  Copyright (c) 2011-2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/23/16    pd      Added a new generic api for loading image from partition.
 06/21/13    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 11/12/12    ag     added load image from file system
 09/20/12    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 11/02/11    yg     Changed name and added interface functions
 10/13/11    ec     Created.
=============================================================================*/

#ifndef __EFITZELOADER_H__
#define __EFITZELOADER_H__

/** @cond */
/* Protocol type. */
typedef struct _EFI_TZE_LOADER_PROTOCOL EFI_TZE_LOADER_PROTOCOL;
/** @endcond */

/** @ingroup efi_tze_loader_constants
  Protocol version. 
*/
#define EFI_TZE_LOADER_PROTOCOL_REVISION 0x0000000000010006

/* Protocol GUID definition.
  {9A10DB8C-08BA-45CA-A894-40C2514D4645} */
/** @ingroup efi_tze_loader_protocol */
#define EFI_TZE_LOADER_PROTOCOL_GUID \
   { 0x9a10db8c, 0x8ba, 0x45ca, { 0xa8, 0x94, 0x40, 0xc2, 0x51, 0x4d, 0x46, 0x45 } }

/** @cond */
/* External reference to the Tze Loader Protocol GUID */
extern EFI_GUID gEfiTzeLoaderProtocolGuid;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_TZE_LOAD_IMAGE_FROM_PARTITION */
/** @ingroup efi_tze_loader_image_from_partition
  @par Summary
  Loads a TZExec application image from a partition.
 
  @param[in]  pThis          Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionGuid GUID of the partition that contains the TZExec 
                             application image. The partition is searched only 
                             in the nonremovable device's GUID partition table.
  @param[out] AppIdPtr       Application ID pointer.
    
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_NOT_FOUND          -- Elf image is invalid or was not found in the 
                            partition. \n
  EFI_LOAD_ERROR         -- Elf image is corrupted and cannot be loaded. \n
  EFI_DEVICE_ERROR       -- Unable to read from the device. \n
  EFI_ACCESS_DENIED      -- Image cannot be loaded into the memory region. \n
  EFI_OUT_OF_RESOURCES   -- Memory cannot be allocated. \n
  EFI_SECURITY_VIOLATION -- Image authentication failed. \n
  EFI_INVALID_PARAMETER  -- One or more parameters are invalid.
*/ 
typedef 
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_IMAGE_FROM_PARTITION)(
     IN  EFI_TZE_LOADER_PROTOCOL      *pThis,
     IN  EFI_GUID                     *pPartitionGuid,
     IN  UINT16                       *pPartitionLabel,
     OUT UINT32                       *AppIdPtr
);

/* EFI_TZE_LOAD_IMAGE_FROM_PARTITION_LABEL */
/** @ingroup efi_tze_loader_image_from_partition_label
  @par Summary
  Loads a TZExec application image from a partition based on label.
 
  @param[in]  pThis           Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionLabel GUID of the partition that contains the TZExec 
                              application image. The partition is searched only 
                              in the nonremovable device's GUID partition table.
  @param[out] AppIdPtr       Application ID pointer.
    
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_NOT_FOUND          -- Elf image is invalid or was not found in the 
                            partition. \n
  EFI_LOAD_ERROR         -- Elf image is corrupted and cannot be loaded. \n
  EFI_DEVICE_ERROR       -- Unable to read from the device. \n
  EFI_ACCESS_DENIED      -- Image cannot be loaded into the memory region. \n
  EFI_OUT_OF_RESOURCES   -- Memory cannot be allocated. \n
  EFI_SECURITY_VIOLATION -- Image authentication failed. \n
  EFI_INVALID_PARAMETER  -- One or more parameters are invalid.
*/ 
typedef 
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_IMAGE_FROM_PARTITION_LABEL)(
     IN  EFI_TZE_LOADER_PROTOCOL      *pThis,
     IN  UINT16                       *pPartitionLabel,
     OUT UINT32                       *AppIdPtr
);



/* EFI_TZE_LOAD_IMAGE_FROM_FILE */
/** @ingroup efi_tze_loader_image_from_file
  @par Summary
  Loads a TZExec application image from a file path.

  @param[in]  This      Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  FilePath  Pointer to the file path where the image is located.
  @param[out] AppIdPtr  Application ID pointer.

  @return
  EFI_INVALID_PARAMETER -- File path is invalid. \n
  EFI_LOAD_ERROR        -- Unable to load the file.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_IMAGE_FROM_FILE)(
      IN  EFI_TZE_LOADER_PROTOCOL      *This,
      IN  CHAR8*                       FilePath,
      OUT UINT32                       *AppIdPtr
    );

/* EFI_TZE_LOAD_IMAGE_FROM_BUFFER */
/** @ingroup efi_tze_loader_image_from_buffer
  @par Summary
  Loads a TZExec application image from a buffer.

  @param[in]  This        Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  Buffer      Pointer to the buffer where the image is located.
  @param[in]  BufferSize  Buffer size.
  @param[out] AppIdPtr    Application ID pointer.

  @return
  EFI_OUT_OF_RESOURCES -- Memory cannot be allocated. \n
  EFI_LOAD_ERROR       -- Error occurred when loading the file, or TrustZone 
                          returned an invalid application ID.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_IMAGE_FROM_BUFFER)(
      IN  EFI_TZE_LOADER_PROTOCOL      *This,
      IN  UINT32                       *Buffer,
      IN  UINTN                        BufferSize,
      OUT UINT32                       *AppIdPtr
    );

/* EFI_TZE_LOAD_IMAGE_FROM_FILE_SYSTEM */
/** @ingroup efi_tze_loader_image_from_file_system
  @par Summary
  Loads a TzExec application image from a file system partition. 

  @param[in]  This           Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionGuid GUID of the partition that contains the TZExec 
                             application image. The partition is searched only 
                             in the nonremovable device's GUID partition table.
  @param[in]  Path           Pointer to the path where the image is located.
  @param[out] AppIdPtr       Application ID pointer.

  @return
  EFI_INVALID_PARAMETER -- Path is invalid. \n
  EFI_LOAD_ERROR        -- Unable to load the file.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_IMAGE_FROM_FILE_SYSTEM)(
     IN  EFI_TZE_LOADER_PROTOCOL      *This,
     IN  EFI_GUID                     *pPartitionGuid,
     CHAR16                           *Path,
     OUT UINT32                       *AppIdPtr
  );

  
/** Types of Truszone syscall that can be executed based on type of image. 
    each of these syscall have different functionality.
    EXTERNAL_IMAGE :Load external image using TZ_OS_LOAD_EXTERNAL_IMAGE
    SERVICE_IMAGE  : Load a secure services image in secure user space 
                   using TZ_OS_LOAD_SERVICES_IMAGE_ID
    APP_IMAGE      : Load secure application in secure user space 
                   using TZ_OS_APP_START_ID 

*/  
typedef enum
 {
   EXTERNAL_IMAGE,      //Load external image in secure memory
   SERVICE_IMAGE,       //load service image in secure memory
   APP_IMAGE,           //load application image in secure memory
   LOAD_IMAGE           //load image in non secure memory and return pointer 
 }tz_load_image_type;

/* EFI_TZE_LOAD_EXTERNAL_IMAGE_FROM_FILE_SYSTEM */
/** @ingroup efi_tze_load_external_image_from_file_system
  @par Summary
  Loads an external image into secure user space from a file system partition. 

  @param[in]  This           Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionGuid GUID of the partition that contains the TZExec 
                             application image. The partition is searched only 
                             in the nonremovable device's GUID partition table.
  @param[in]  Path           Pointer to the path where the image is located.
  @param[in] Buffer         Pointer to buffer where the image is loaded in ns El1
  @param[out] BufferSize     Size of buffer where the image is loaded.
  @param[out] tz_load_
              image_type     Enum image_type to choose type of syscall to be executed to
                             load the image.
                             
  @param[out] AppIdPtr       Application ID pointer.
  

  @return
  EFI_INVALID_PARAMETER -- Path is invalid. \n
  EFI_LOAD_ERROR        -- Unable to load the file.
*/  
typedef
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_EXTERNAL_IMAGE_FROM_FILE_SYSTEM)(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   IN  CHAR16                         *Path,
   IN  tz_load_image_type             ImageType,
   IN VOID                            *Buffer,
   OUT UINTN                          *BufferSize,
   OUT UINT32                         *AppIdPtr
); 
   
  
/** @cond */
typedef
EFI_STATUS
(EFIAPI *EFI_TZE_LOAD_SECURE_APPS)(
     IN  EFI_TZE_LOADER_PROTOCOL      *This,
     IN EFI_HANDLE                     ImageHandle,
     IN EFI_SYSTEM_TABLE              *SystemTable,
     OUT UINT32                       *AppIdPtr
  );
/** @endcond */

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_tze_loader_protocol
  @par Summary
  TrustZone Executable (TZExec) Loader protocol interface.  

  @par Parameters
  @inputprotoparams{tze_loader_proto_params.tex} 
*/
struct _EFI_TZE_LOADER_PROTOCOL {
   UINT64                                        Revision;
   EFI_TZE_LOAD_IMAGE_FROM_PARTITION             LoadImageFromPartition;
   EFI_TZE_LOAD_IMAGE_FROM_FILE                  LoadImageFromFile;
   EFI_TZE_LOAD_IMAGE_FROM_BUFFER                LoadImageFromBuffer;
   EFI_TZE_LOAD_IMAGE_FROM_FILE_SYSTEM           LoadImageFromFileSystem;
/** @cond */
   EFI_TZE_LOAD_SECURE_APPS                      LoadSecureApps;
/** @endcond */
   EFI_TZE_LOAD_IMAGE_FROM_PARTITION_LABEL       LoadImageFromPartitionLabel;
   EFI_TZE_LOAD_EXTERNAL_IMAGE_FROM_FILE_SYSTEM  LoadExternalImageFromFileSystem;
}; 


#endif /* __EFITZELOADER_H__ */
