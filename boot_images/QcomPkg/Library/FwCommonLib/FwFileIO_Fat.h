/** @file FwFileIO_Fat.h
   
  Header for externally-available functions of the 
  FwFileIO_Fat

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2011/10/12     jthompso    Initial version

=============================================================================*/

#ifndef __QCOM_FW_FILEIO_FAT_H__
#define __QCOM_FW_FILEIO_FAT_H__


#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathUtilities.h>
#include <Library/FwCommonLib.h>


/**
 * Creates a directory in the given File system root.
 *  
 * Called by FwUpdateFatFileOperation when writing a file 
 *  fails because the directory is nonexistent. 
 * 
 * @param FsRoot      - File System Root where directory will be
 *                      created.
 * @param FileName    - Full path of file that 
 *  				    FwUpdateFatFileOperation was attempting
 *  			        to create.
 * @param Attributes  - Attributes passed in from 
 *                      FwUpdateFatFileOperation
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateCreateDirectory(
    IN EFI_FILE_PROTOCOL    *FsRoot,
    IN UINT16               *FileName,
    IN UINT64               Attributes);

/**
 * Reads the FileInfo from a given file.  Caller needs to
 * free the allocated FileInfo.
 * 
 * @param File         - Handle to File
 * @param FileInfoSize - Output to file info size
 * @param FileInfo     - Returns modified pointer to FileInfo
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateFatGetFileInfo(
    IN EFI_FILE_PROTOCOL    *File,
    IN OUT UINTN *FileInfoSize,
    IN OUT EFI_FILE_INFO       **FileInfo
    );


/**
 * Performs either a read or 
 *  write on a FAT filesystem. 
 * 
 * 
 * @param PartHandle  - Handle for the FAT partition. 
 * @param FileBuffer  - Buffer for bytes read or bytes 
 *  				    to write out.
 * @param FwFile      - Enum for the file being sought.
 * @param FileSize    - For read, the number of bytes read, 
 *  				    For write, the number of bytes to write.
 * @param Read        - Whether this operation is read or 
 *  				    write.
 * @param FileHandle  - Optional, return the handle of the file.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatFileOperation(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize,
    IN BOOLEAN                  Read
    );


/**
 * Reads file from a FAT partition
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes read. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFat(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    OUT UINTN                   *FileSize
    );




/**
 * Writes file to a FAT partition
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes to read. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteFileToFat(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize

    );


/** 
 *  
 * Gets the device path for the device which contains the 
 * requested file and adds a File Path entry for the requested 
 * file to the device path. 
 *  
 * This device path is passed to LoadImage(). 
 *  
 * This function should only be called for 
 * the Payload Application. 
 * 
 * @param FwFile  - Desired file
 * @param SrcPart - Desired File's partition
 * @param DevPath - Return the device path with the 
 *  			    File Path entry added.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFilePathDevPath(
    IN  QCOM_FW_UPDATE_FW_FILE      FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT EFI_DEVICE_PATH_PROTOCOL    **DevPath
    );



/**
 * Performs either a read or 
 *  write on a FAT filesystem. 
 * 
 * 
 * @param PartHandle   - Handle for the FAT partition. 
 * @param FileName     - Unicode Null-terminated file name.
 * @param FileBuffer   - Buffer for bytes read or bytes 
 *  				     to write out.
 * @param FileSize     - For read, the number of bytes read, 
 *  					 For write, the number of bytes to
 *  					 write.
 * @param Read         - Whether this operation is read or 
 *  				     write.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatFileOperation2(
    IN EFI_HANDLE   PartHandle,
    IN CHAR16       *FileName,
    IN OUT VOID     **FileBuffer,
    IN OUT UINTN    *FileSize,
    IN BOOLEAN      Read
    );


/**
 * Reads file from a FAT partition
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem 
 * @param FileName   - Unicode Null-terminated file name. 
 * @param FileBuffer - Buffer to store the file
  @param FileSize    - File size, number of bytes read. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFat2(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    IN OUT VOID                 **FileBuffer,
    OUT UINTN                   *FileSize
    );



/**
 * Writes file to a FAT partition
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem 
 * @param FileName   - Unicode Null-terminated file name. 
 * @param FileBuffer - Buffer to store the file
 * @param FileSize   - File size, number of bytes to write. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteFileToFat2(
    IN EFI_HANDLE   PartHandle,
    IN CHAR16       *FileName,
    IN VOID         **FileBuffer,
    IN UINTN        *FileSize
    );


/**
 * Create a File in FAT
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes to read. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CheckandCreateFilesinFat(
        VOID
      );



/**
 * Given a FAT directory path, retrieve the names of all files  
 * that immediately reside under specified path. This is not a 
 * recursive operation so subfolders are not pinged.
 * 
 * @param PartHandle - Handle to FAT partition
 * @param FwFile     - Enumeration of directory path. 
 * @param Path       - Upon return, store the unicode string of 
 *                     directory path 
 * @param DirName    - Upon return, store the bottom-level 
 *  				   subfolder name of specified directory
 *  			       path, e.g. if path is "\DPP\QCOM\", then
 *  			       return "QCOM".
 * @param FileList   - Upon return, a pointer to a list of 
 *  			       unicode Null-terminated file names.
 * @param FileCount  - Upon return, the number of files. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileList(
    IN  EFI_HANDLE  PartHandle,
    IN  CHAR16      *Path,
    OUT CHAR16      **Subfolder,
    OUT CHAR16      ***FileList,
    OUT UINTN       *FileCount
    );



#endif

