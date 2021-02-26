/** @file QcomLib.h
   
  Contains generic Qualcomm library functions.

  Copyright (c) 2011-2017, 2019 Qualcomm Technologies, Inc. All rights reserved. 
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
 12/01/17   kpa     relocate ReadFromPartition to MountFvUtils.h
 09/08/17   bd      Support for authenticated FV loading
 01/27/17   ai      Fix use of retail
 08/23/16   ai      Add support for connect controller on specific handles
 08/24/16   bh      Update timeout function to provide key info to caller
 08/18/16   bh      Add API to unmount FAT partition
 08/11/16   bh      Add EDL API
 07/05/16   bh      Add partition label parameter for file system APIs
 06/22/16   bh      Add function for debug tools mounting
 06/22/16   bh      Add retail check macro
 06/15/16   bd      Remove offlinecrashdump library dependency from QcomLib 
 06/08/16   sj      removed the enter mass storage cookie function
 03/07/16   ml      Renamed splash screen function
 03/04/16   bh      Allow fat mounting by partition type also
 02/08/16   vk      Remove GetOsType()
 10/29/15   vk      Add support to ProcessFvImageFile
 06/25/15   bh      Add options for GetBlkIO selection and Timeout functions
 05/15/15   sm      Added LoadFileFromVolume
 12/14/14   vk      Remove API
 12/02/14   bh      Add WaitForTimeout function 
 11/26/14   bh      Added LoadDebugFv to header file
 10/22/14   aus     Added support for EDL mode 
 10/16/14   vk      LoadFileFromPartition signature update
 10/28/13   niting  Added string to pass to ResetSystem for identifying UpdateCapsule
 05/13/13   niting  Added options to change state of BDS Menu
 04/03/13   niting  Moved InCarveoutMode to OfflineCrashDumpLib
 03/06/13   sm      Added ReadFromFV API
 01/18/13   nk      Added Dump base/offset address
 01/17/13   yg      Added BootOrder related Api
 11/02/12   vk      Added DeleteFile
 09/17/12   yg      Redesign ReadAnyKey API
 08/09/12   yg      Change GetAnyKey API param type
 07/18/12   jz      Added WriteFile()
 07/09/12   yg      Added HexStrToBin & LoadFileFromPartition
 07/05/12   rsb     Added FlushVariableServices()
 06/11/12   jz      Added InCarveoutMode()
 06/05/12   rs      Added RenderBGRTImage() and ConversionPixelCopy()  
 04/26/12   vishalo Added GetOsType()
 04/13/12   jz      Added GetProcessorName() 
 04/12/12   nk      Added function for returning pointer to EfiSimpleTextInProtocol
 04/2/12    kpa     Added String manipulation and File read api.
 03/21/12   shilpar Added API to read from a file to a buffer
 01/26/11   yg      Add API to get platform strings
 12/19/11   yg      Add API to get platform type
 11/15/11   vishalo Run image from specified partition 
 11/02/11   yg      Add eMMC Hw partition support to BlockIo handle select API.
 08/16/11   yg      Added BlockIo handle select API.
 07/22/11   pbitra  Added strncpy support.
 07/08/11   niting  Added mass storage support.

=============================================================================*/
#ifndef __QCOMLIB_H__
#define __QCOMLIB_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/BaseLib.h>
#include <Guid/Gpt.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include "com_dtypes.h"
#include <Protocol/EFIPlatformInfo.h>
#include <IndustryStandard/Acpi50.h>
#include <Library/MountFvUtils.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/**
  gQcomTokenSpaceGuid GUID definition.
 */
#define QCOM_TOKEN_SPACE_GUID \
        { 0x882f8c2b, 0x9646, 0x435f, { 0x8d, 0xe5, 0xf2, 0x08, 0xff, 0x80, 0xc1, 0xbd } }

/* Selection attributes for selecting the BlkIo handles */
#define   BLK_IO_SEL_MEDIA_TYPE_REMOVABLE          0x0001UL
#define   BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE      0x0002UL
#define   BLK_IO_SEL_PARTITIONED_GPT               0x0004UL
#define   BLK_IO_SEL_PARTITIONED_MBR               0x0008UL
#define   BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID     0x0010UL
#define   BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM     0x0020UL
#define   BLK_IO_SEL_SELECT_BY_VOLUME_NAME         0x0040UL

/* Select only the root device handle indicated. Doesn't return
 * any partitions within.
 * Currently this filter applies only for eMMC device, not the external
 * device connected via USB */
#define   BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY       0x0080UL
/* Select the handle that's on the indicated root device.
 * Currently this filter applies only for eMMC device, not the external
 * device connected via USB */
#define   BLK_IO_SEL_MATCH_ROOT_DEVICE             0x0100UL

/* Select through partition name*/
#define   BLK_IO_SEL_MATCH_PARTITION_LABEL         0x0200UL

/* Do case insensetive string comparisons */
#define   BLK_IO_SEL_STRING_CASE_INSENSITIVE       0x0400UL

/* Partitioning scheme types for selecting the BlkIo handles */
#define PARTITIONED_TYPE_MBR    0x01
#define PARTITIONED_TYPE_GPT    0x02

#define EDL_STRING              L"EDL"

#define CAPSULE_RESET_STRING     L"CapsuleReset"

/* Options passed into WaitForTimeout function */
#define TIMEOUT_WAIT_FOR_KEY    0x01

/* BDS menu hotkey states */
typedef enum {
  ///
  /// Enable BDS Menu HotKey Detection (default state)
  ///
  BDSHotKeyStateDetect        = 0,

  ///
  /// Disable BDS Menu
  ///
  BDSHotKeyStateDisableDetect = 1,

  ///
  /// Force BDS HotKey Menu to always execute
  ///
  BDSHotKeyStateForceMenu     = 2,

  ///
  /// Maximum enumeration value that may be used for bounds checking.
  ///
  MaxBDSHotKeyState
} BDS_HOTKEY_STATE;

#define BDS_HOTKEY_STATE_VARNAME L"BDSHotKeyState"

/* BDS Clock Log states */
typedef enum {
  ///
  /// Clock Log State Disable (default state)
  ///
  BDSClockLogStateDisable    = 0,

  ///
  /// Clock Log State to Uart
  ///
  BDSClockLogStateToUart     = 1,

  ///
  /// Clock Log State to File
  ///
  BDSClockLogStateToFile     = 2,

  ///
  /// Maximum enumeration value that may be used for bounds checking.
  ///
  MaxBDSClockLogState
} BDS_CLOCKLOG_STATE;

#define BDS_CLOCKLOG_STATE_VARNAME L"BDSClockLogState"


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the gQcomTokenSpaceGuid GUID.
 */
extern EFI_GUID gQcomTokenSpaceGuid;

/**
 * Extern referecen to gEfiPartitionTypeGuid GUID.
 */
extern GUID gEfiPartitionTypeGuid;

/**
  Enable mass storage mode upon reset. 
   
  @retval EFI_UNSUPPORTED If mass storage is not enabled 
 */

extern EFI_GUID gEfiPartitionRecordGuid;

EFI_STATUS
WaitForTimeout (IN UINT32             Timeoutms, 
                IN UINT32             WaitOptions, 
                IN OUT EFI_INPUT_KEY *UserKey OPTIONAL);

EFI_STATUS
WaitForTimeoutNoKey (IN UINT32  Timeoutms);


/* Any data specific to additional attributes can be added here. */
typedef struct {
  EFI_GUID      *RootDeviceType;  /* GUID Selecting the root device type */
  EFI_GUID      *PartitionType;   /* Partition Type to match */
  CHAR8         *VolumeName;        /* Mounted filesystem volume name to match */
  CHAR16        *PartitionLabel;    /* Partition label to match */
} PartiSelectFilter;

/* Output data providing more information about the device handle */
typedef struct {
  /* Handle that has BlkIO protocol installed, returned for all type of filters */
  EFI_HANDLE                     *Handle;

  /* Block IO protocol interface is returned for all type of filters */
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;

  /* This HDD dev path is returned only if Matching Partition type is requested
   * It should be noted that the contents of this memory should NOT be changed */
  const HARDDRIVE_DEVICE_PATH    *PartitionInfo;
} HandleInfo;

/**
 *  Load and execute image from FV.
 * 
 * UiString: Application/image to be loaded.
 */
EFI_STATUS
LaunchFromFv(
IN  CHAR16      *UiString 
);

/**
  Returns a pointer to interface for EfiSimpleTextInProtocol,
  for which vendor guid is same as the guid published by keypad
   
  @retval KeypadProtocol if the operation was successful
 */
EFI_STATUS
EFIAPI
GetNativeKeypad ( 
                 OUT  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    **KeypadProtocol 
                );


/**
  Returns a list of BlkIo handles based on required criteria
  SelectionAttrib : Bitmask representing the conditions that need
                    to be met for the handles returned. Based on the
                    selections filter members should have valid values.
  FilterData      : Instance of Partition Select Filter structure that
                    needs extended data for certain type flags. For example
                    Partition type and/or Volume name can be specified.
  HandleInfoPtr   : Pointer Handle info where the information can be returned
  MaxBlkIopCnt    : On input, max number of handles the buffer can hold, 
                    On output, the number of handles returned.

  @retval EFI_SUCCESS if the operation was successful
 */
EFI_STATUS
EFIAPI
GetBlkIOHandles (
                  IN      UINT32                   SelectionAttrib,
                  IN      PartiSelectFilter        *FilterData,
                  OUT     HandleInfo               *HandleInfoPtr,
                  IN OUT  UINT32*                  MaxBlkIopCnt
                 );

/**
  Mount specific FAT partition 

  @param  PartitionLabel  Name of partition label to mount
  @return Status

**/
EFI_STATUS
MountFatPartition(
  CHAR16    *PartitionLabel
  );

/**
  Unmount specific FAT partition

  @param  BlkIoHandle  Block IO handle of partition to unmount FAT
  @return Status

**/
EFI_STATUS
UnmountFatPartition(
  EFI_HANDLE *UserHandle
  );

/**
  Initialize CRC32 table.

**/
VOID
InitializeCrc32Table (
  VOID
  );

/**
  Calculate CRC32 for data.

  @param Data            Pointer to the data to calculate CRC on.
  @param DataSize        Size in bytes of the data.
  @param CrcOut          Pointer to the CRC32 of the data.

  @retval  EFI_SUCCESS            The CRC32 for data is calculated successfully.
  @retval  EFI_INVALID_PARAMETER  Data or CrcOut is NULL.

**/
EFI_STATUS
EFIAPI
CalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  );

/**
  Start an EFI image only from a specified GPP partition  

  @param  Path   Image path
  @param  RootDeviceTypeGuid eg: gEfiEmmcGppPartition1Guid
  @param  Partition Type GUID eg: gEfiPartTypeSystemPartGuid
  @param  Boolean TRUE for Non-removable / FALSE for Removable 
  @param  Volume Name to Match
  @return Exit code from image  

**/
EFI_STATUS
ExecImgFromVolume(
  CHAR16        *Path,
  EFI_GUID      *RootDeviceType,
  EFI_GUID      *PartitionType,
  BOOLEAN       SelectNonRemovable,
  CHAR8         *VolumeName
  );

/**
 * Get the platform type
 *
 * @param  PlatformTypePtr   Pointer to Platform Type where result can be expected
 * **/
EFI_STATUS
GetPlatformType (EFI_PLATFORMINFO_PLATFORM_TYPE* PlatformTypePtr);

/**
 * Get the platform related strings
 *
 * @param  DestinationBuffer   Pointer to buffer where result can be expected
 * @param  BufferSize          Size of the buffer
 * @param  PlatformTypeStr     Optional Pointer to Platform type string
 * @param  ChipNameStr         Optional Pointer to Chip Name string
 * @param  ChipVerStr          Optional Pointer to Chip Version string
 * **/
EFI_STATUS
GetPlatformStrings (IN OUT  CHAR8*  DestinationBuffer,
                    IN      UINTN   BufferSize,
                    OUT     CHAR8** PlatformTypeStr  OPTIONAL,
                    OUT     CHAR8** ChipNameStr      OPTIONAL, 
                    OUT     CHAR8** ChipVerStr       OPTIONAL);


/**
 * Get processor name string
 *
 * @param  ProcNameStr   Pointer to buffer where result can be expected
 * @param  StrSize       Size of the buffer
 * **/
EFI_STATUS
GetProcessorName (IN OUT  CHAR8*  ProcNameStr,
                  IN      UINTN    StrSize);

/**
  Return the size of a file

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  FileSize            Size of file
  @return EFI_SUCCESS         On successfully retreiving file-size 

**/ 
EFI_STATUS
GetFileSize(
  IN  CHAR16           *Path,
  IN  EFI_GUID         *RootDeviceType,
  IN  EFI_GUID         *PartitionType,
  IN  CHAR16           *PartitionLabel,
  IN  BOOLEAN          SelectNonRemovable,
  IN  CHAR8            *VolumeName,
  OUT UINT32           *FileSize
  );

/**
  Read a file to a buffer 

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  BytesToRead         # of bytes to read from file
  @param  ReadOffset          Read-offset in file in bytes
  @param  FileBuffer          Data read from file is copied to FileBuffer
  @param  FileBufferSize      Size of the file
  @return EFI_SUCCESS         If file was read successfully 

**/ 
EFI_STATUS
ReadFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     CHAR16           *PartitionLabel,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN OUT UINTN            *BytesToRead,
  IN     UINT64           ReadOffset,
  OUT    UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  );

/**
  Write a buffer to a file

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  BytesToRead         # of bytes to write to file
  @param  WriteOffset         Write-offset in file in bytes
  @param  FileBuffer          Data written to a file is copied from FileBuffer
  @param  FileBufferSize      Size of the file
  @return EFI_SUCCESS         If file was written successfully 

**/ 
EFI_STATUS
WriteFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     CHAR16           *PartitionLabel,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN OUT UINTN            *BytesToWrite,
  IN     UINT64           WriteOffset,
  OUT    UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  );

/**
  Delete a  file from specified partition  
  @param  Path            Path to Smbios file
  @param  RootDeviceType  Root Device Type GUID 
  @param  Partition Type  GUID eg: gEfiPartTypeSystemPartGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @return EFI_SUCCESS if successful
**/

EFI_STATUS
DeleteFile (
  IN  CHAR16           *Path,
  IN  EFI_GUID         *RootDeviceType,
  IN  EFI_GUID         *PartitionType,
  IN  CHAR16           *PartitionLabel,
  IN  BOOLEAN          SelectNonRemovable,
  IN  CHAR8            *VolumeName
  );

/**
  Loads a  file from a partition specified by Root Device type GUID 
  Partition Type GUID and Block IO attributes
  
  @param  BlkIOAttrib   Attributes to be used for fetching Block IO handle   
  @param  Path            Path to file to open
  @param  RootDeviceType  Root Device Type GUID 
  @param  Partition Type  GUID eg: gEfiPartTypeSystemPartGuid
  @param  PartitionLabel  eg: L"logfs"

  @return EFI_SUCCESS if successful
**/
EFI_STATUS
LoadFileFromPartition(
  IN   UINT32        BlkIOAttrib,
  IN   CHAR16        *Path,
  IN   EFI_GUID      *RootDeviceType,
  IN   EFI_GUID      *PartitionType,
  IN   CHAR16        *PartitionLabel,
  OUT  UINT8         **CfgBuffPtr,
  OUT  UINTN         *ReadFileSize  OPTIONAL
);

/**
  Loads a  file from a volume specified by name and Block IO attributes

  @param  Path            Path to file to open
  @param  VolumeName      Name of volume to load from
  @param  BlkIOAttrib     Attributes to be used for fetching Block IO handle
  @param  ReadFileSize    Ptr that can hold read file size

  @return EFI_SUCCESS if successful
**/

EFI_STATUS
LoadFileFromVolume(
  UINT32        BlkIOAttrib,
  CHAR16        *Path,
  CHAR8         *VolumeName,
  UINT8         **CfgBuffPtr,
  UINTN         *ReadFileSize
  );

/**
  Convert string to integer. The string should have numeric characters.
  conversion stops at first non-numeric character encountered.
  
  @param  Str     Null Terminated string of characters.
          StrSize Max size of buffer, to prevent against
                  strings that are not NULL terminated.
 
  @retval  int    converted integer value of valid numeric string.
                  0 if invalid string

**/
UINT32 AsciiStrToHex (CHAR8* Str, UINT32 StrSize);

/**
  Convert string to Lowercase if lowercase alphabets found.
  
  @param  Str     Null Terminated string of characters. 
          StrSize Max size of buffer, to prevent against
                  strings that are not NULL terminated.

  @retval  void    

**/
void AsciiStrToLower (CHAR8* Str, UINT32 StrSize);

/**
  Convert Ascii string to unsigned Int.
  @param  Str     Null Terminated string of characters numbers. 
  @retval Value translated from String.
**/
UINTN AsciiToInt(CHAR8* Str);

/* Public Functions */

/**
  RenderSplashScreen - Load, process and display OEM logo bitmaps including processing needed to
                       support ACPI2.0 BGRT.

   - RenderSplashScreen is called with NULL which will render the logo (found in FV or HLOS partitions).
   - RenderSplashScreen is called with valid BGRT will load the 2nd logo and update the BGRT table.  

  @param  BGRTTable   Pointer to the BGRT table in ACPI, if NULL the primary logo is rendered with no BGRT information returned.

  @retval EFI_SUCCESS             Image loaded to BGRT location and to the screen.
  @retval EFI_INVALID_PARAMETER   Invalid input parameters passed in
  @retval EFI_UNSUPPORTED         Invalid image or image format

**/
EFI_STATUS
EFIAPI
RenderSplashScreen(
  OUT VOID *BGRTTable OPTIONAL);

  
/**
  RenderBGRTImage - Render an image on to the screen from the BGRT buffer

  @param[in]  BGRTImage      BGRT Source image buffer
  @param[in]  BGRTImageSize  BGRT image buffer size
  
  @retval EFI_SUCCESS            Image loaded successfully in to memory.
  @retval EFI_INVALID_PARAMETER  Invalid input parameters passed in
  @retval EFI_OUT_OF_RESOURCES   Not enough resources for buffer allocations

**/
EFI_STATUS
RenderBGRTImage(
    IN UINT8 *BGRTImage,
    IN UINT32 BGRTImageSize);

/**
  This function flushes the UEFI variable services to ensure that the variable operations
  are written to disk without requiring a reset.
  
  @retval  EFI_SUCCESS      Successfully flushed NV variable services
  @retval  EFI_UNSUPPORTED  Returned if not able to locate variable services protocol
**/
EFI_STATUS
FlushVariableServices(
  VOID
  );


/*
 * Convert Hex Representation of the string to Binary value.
 *
 * @param[in]  Buffer    Input buffer containing string
 * @param[in]  BuffSize  Input buffer size
 * @param[out]  OutBuffer Output buffer to get converted Binary value
 * @param[in]  OutSize   Output buffer size
 *
 * Returns the size bytes of the converted binary value
 * OutBuffer passed in should be atleast of BuffSize / 2
 * to acommodate if string contains all convertible values.
 * */
UINTN
HexStrToBin (IN  UINT8*   Buffer,
             IN  UINTN    BuffSize,
             OUT UINT32*  OutBuffer,
             IN  UINTN    OutSize);

#define READ_KEY_ATTRIB_NO_BLOCKING                  (1 << 0)
#define READ_KEY_ATTRIB_RESET_AFTER_READ             (1 << 1)
// #define READ_KEY_ATTRIB_USE_NATIVE_BUTTONS_ONLY      (1 << 2)

/*
 *  Read any key from the generic input interface and map it to internal
 *  Common key mapping.
 *
 *  NOTE: 
 *    Set Attrib READ_KEY_ATTRIB_NO_BLOCKING for a non blocking call
 *    Set Attrib READ_KEY_ATTRIB_USE_NATIVE_BUTTONS_ONLY to use only the
 *       physical buttons on the device (UART and any other input method is
 *       not used, This option is Not supported yet)
 *    If Attribs is 0, then the call is blocking until a key pressed and
 *       any input method can result into a key press
 *
 *  @param[out]  KeyPtr       Locatin where Key can be returned
 *  @param[in]   Attributes   Conditions that need to be used
 *
 *  @retval  EFI_SUCCESS       Successfully got the key input
 *  @retval  EFI_DEVICE_ERROR  Returned on error 
 * */
EFI_STATUS
ReadAnyKey (OUT EFI_INPUT_KEY *KeyPtr OPTIONAL, UINT32 Attributes OPTIONAL);


/* Add the BootOrder entry as first instead of last  */
#define ADD_FLAG_ADD_FIRST      0x00000001
/* Do NOT add the entry to BootNext */
#define ADD_FLAG_NO_BOOT_NEXT   0x00000002

/*
 *  Add a device path to BootOrder variables to automatically launch 
 *  by BDS according to UEFI Spec.
 *
 *  @param  ImageHandle    ImageHandle of the Image that can be used to derive
 *                          the device path to add to BootOrder. This is optional
 *                          Param and must present if DevPath is NULL. If DevPath
 *                          is valid, then ImageHandle is not used.
 *  @param  DevPath       Explicit Devicepath that need to be added to the BootOrder
 *                          This is optional param, if the value is NULL then the
 *                          ImageHandle is used to derive the DevPath needed
 *  @param  Description   Description that needs to go into BootOrder Entry
 *  @param  OptionalData  Optional Data that will go into LoadOptions, typically
 *                          all the command line arguments will be passed as
 *                          LoadOptions.
 *  @param  OptionDataSize If OptionalData is given, then the size of data in bytes
 *  @param  AddFlags       Flags that control how the addition happens. Default
 *                         behavior is:
 *                           - Entry is added to last
 *                           - BootNext is set to the newly added/updated entry
 *                           Above behavior can be changed using the Flags
 *                           For description of the entries, refer to the macro
 *                           description.
 *
 *  @retval  EFI_SUCCESS    
 *  @retval  EFI_INVALID_PARAMETER  
 *  @retval  EFI_OUT_OF_RESOURCES
 * */
EFI_STATUS
AddDevPathToBootOrder (IN EFI_HANDLE                 ImageHandle    OPTIONAL,
                       IN EFI_DEVICE_PATH_PROTOCOL  *DevPath        OPTIONAL,
                       IN CHAR16                    *Description,
                       IN UINT8                     *OptionalData   OPTIONAL,
                       IN UINTN                      OptionDataSize,
                       IN UINTN                      AddFlags);

/*
 *  Remove a BootOrder entries that start with the given Description. All the 
 *  entries with matching Description will be deleted and BootOrder is updated
 *  to remove those entries.
 *
 *  Description of L"Test" will remove Boot#### entries with Descriptions
 *  L"Test1", L"Test Case" etc.
 *
 *  @param  Description    String that can be matched to remove the Entry with
 *                          the given description
 *
 *  @retval  EFI_SUCCESS    
 *  @retval  EFI_INVALID_PARAMETER  
 *  @retval  EFI_OUT_OF_RESOURCES
 */
EFI_STATUS
RemoveFromBootOrder (IN CHAR16 *Description);

/*
 *  Update the in RAM array that stores the BootOrder data. This function will take the BootOptionNumber
 *  that corresponds to a Boot#### option, and the BootOrderIndex that indicates what Index that option
 *  is to be placed in the BootOrderData, and proceed to place that option at that index. This function
 *  will not actually write to the BootOrder variable. That is the responsibility of the caller.
 *
 *  @param  BootOrderData       The BootOrder variable data
 *  @param  BootOrderDataSize   Size in bytes of the BootOrder array
 *  @param  BootOptionNumber    Boot#### option number to move
 *  @param  BootOrderIndex      Index into BootOrder array to move the
 *                              BootOptionNumber to.
 * 
 *  @retval  EFI_SUCCESS            Successfully updated the BootOrder
 *  @retval  EFI_INVALID_PARAMETER  BootOptionNumber not found in BootOrder
 *  @retval  EFI_INVALID_PARAMETER  Invalid BootOrderIndex or other parameters
 */
EFI_STATUS
ReorderBootOrderIndex(IN OUT UINT16* BootOrderData, 
                      IN     UINTN   BootOrderDataSize,
                      IN     UINT16  BootOptionNumber,
                      IN     UINTN   BootOrderIndex);

/**
  Extract the Command line arguments from the Image handle, process them and return
  in Argc, Argv format.

  Note that no arguments is considered as success and returns argc = 0

  @param[in] ImageHandle    The firmware allocated handle for the EFI image to extract Cmd line
  @param[in] ArgcPtr        Pointer to Argc count variable.
  @param[in] ArgvArrayPtr   Pointer to Argv variable. If Argc>0 then Argv need to be freed using
                             FreePool call
  
  @retval EFI_SUCCESS           Successfully processed
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for the buffers
**/

EFI_STATUS
GetCmdLineArgs (IN  EFI_HANDLE      ImageHandle,
                OUT UINTN          *ArgcPtr,
                OUT CHAR8        ***ArgvArrayPtr);

/**
  This function retrieves the EFI_SECTION_RAW data and size of a EFI_FV_FILETYPE_FREEFORM file using the given UI string
  located in any of the firmware volumes registered on the device.

  The data and size is returned by Buffer and Size. The caller is responsible to free the Buffer allocated 
  by this function.

  If UiString is NULL, then ASSERT() 
  If DataBuffer is NULL, then ASSERT()
  If DataSize is NULL, then ASSERT()

  @param[in]  UiString      The UI String of the file to search in the firmware volumes 
  @param[out] DataBuffer    On output, DataBuffer contains the the data read 
                            from the section in the Firmware File found.
  @param[out] DataSize      On output, the size of DataBuffer.
  
  @retval  EFI_SUCCESS          The image is found and data and size is returned.
  @retval  EFI_NOT_FOUND        The image specified by UiString cannot be found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
  @retval  EFI_DEVICE_ERROR     A hardware error occurs during reading from the 
                                firmware volume.
  @retval  EFI_ACCESS_DENIED    The firmware volume containing the searched 
                                firmware file is configured to disallow reads.
**/
EFI_STATUS 
ReadFromFV (
  IN  CONST CHAR16 *UiString,
  OUT VOID        **DataBuffer,
  OUT UINTN        *DataSize
  );

/** 
  This function returns a number that is unique to each system. It can return up to 16 bytes Device Unique ID
 
  @param[in] Buffer          The caller's buffer to hold the unique number
  @param[in] BufferSize      Size in bytes of the buffer (specified by caller)
  
  @retval  EFI_SUCCESS       Operation was successful.
  @retval  Any other         Error
   
**/
EFIAPI
EFI_STATUS
GetUUID (
  OUT VOID  *Buffer,
  IN  UINT32 BufferSize
  );


/**
  Process the buffer to obtain the decompressed buffer and use that to produce the 
  FVB protocol on the buffer.
  Parts of the function used from MdeModulePkg/Core/Dxe/Dispatcher/Dispatcher.c
  Parts of the function used from EmbeddedPkg/Library/PrePiLib/FwVol.c

  @param  Buffer                The FV buffer to be processed

  @retval EFI_OUT_OF_RESOURCES  No enough memory or other resource.
  @retval EFI_VOLUME_CORRUPTED  Corrupted volume.
  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_INVALID_PARAMETER Invalid parameters

**/
EFI_STATUS
ProcessFvImageFile (
  IN  VOID *Buffer
  );

/**
  Load the Debug FV image by parsing the MBN header 

  @retval EFI_NOT_FOUND         Couldn't locate the MBN imformation
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_INVALID_PARAMETER Invalid parameter.

**/
EFI_STATUS
LoadDebugFv(
    VOID
    );

/**
  Calls Connect controller for all Handles that match the input
  ProtocolGuid. No recursive dispatch.

  @param  ProtocolGuid         The Guid to be processed 

  @retval EFI_SUCCESS          Success 
  @retval EFI_INVALID_PARAMTER ProtocolGuid is NULL 
  @retval EFI_NOT_FOUND        No handles match the search
  @retval EFI_UNSUPPORTED      LocateHandleBuffer() returned invalid data

**/
EFI_STATUS 
ConnectByProtocol (
    EFI_GUID *ProtocolGuid
    );


/* Load the debug FV from the partition into memory and mount */
EFI_STATUS 
LoadDebugToolsFv (VOID);


/** Enter EDL mode */
VOID EnterEDLMode ( VOID );

#endif /* __QCOMLIB_H__ */
