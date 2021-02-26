/** @file ShLib.h
   
  Contains Shared Library loader Interface library functions.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/22/13   vk      Update return types for 64 bit
 09/17/12   yg      Initial Version

=============================================================================*/
#ifndef __SH_LIB__H__
#define __SH_LIB__H__

#include "Uefi.h"

#define  LIB_SUCCESS                        (EFI_SUCCESS)
#define  LIB_CLOSE_MATCH_FOUND              (EFI_NO_MAPPING)

#define  LIB_ERR_NOT_FOUND                  (EFI_NOT_FOUND)
#define  LIB_ERR_INVALID_PARAM              (EFI_INVALID_PARAMETER)
#define  LIB_ERR_ALREADY_INSTALLED          (EFI_ALREADY_STARTED)
#define  LIB_ERR_OUT_OF_RESOURCES           (EFI_OUT_OF_RESOURCES)
#define  LIB_ERR_INCOMPATIBLE_LOADER        (EFI_UNSUPPORTED)

#define  LIB_MAJOR_VERSION_NUMBER_MASK      (0xFFFF0000U)

/* Install a library that's implemented for client's usage
 *
 * Parameters:
 *   LibName    - Null terminated string that represents the library
 *   LibVersion - UINT32 version number (See Load_Lib for version related behavior) 
 *   LibIntf    - Library interface pointer (Clients should know how the interface looks like)
 *
 * Returns:
 *   LIB_SUCCESS  - Installed the Library successfully
 *   LIB_ERR_INVALID_PARAM  - Invalid parameters
 *   LIB_ERR_OUT_OF_RESOURCES  - Out of resources to install library (MAX_LIB_COUNT)
 *   LIB_ERR_INCOMPATIBLE_LOADER  - Loader replaced with incompatible version
 *
 * */
typedef EFI_STATUS (*INSTALL_LIB) (IN  CHAR8   *LibName,
                              IN  UINT32   LibVersion,
                              IN  VOID    *LibIntf);

/* Get library for client's usage, this returns the interface
 * for the library that's requested 
 *
 * Parameters:
 *   LibName    - Null terminated string that represents the library
 *   LibVersion - UINT32 version number
 *                  MSB 16 bits are major version number
 *                  LSB 16 bits are minor version number
 *                  Load library will return any version lib if the requested version is 0.
 *                  If requested exact version is not available but latest version
 *                    of the lib with the same major version number is available,
 *                    then that will be returned with return status as LIB_CLOSE_MATCH_FOUND
 *                  WARNING: If the Major version number is same, the LibAPI's should be
 *                           backward compatible.
 *   LibIntfPtr - Library interface pointer (Clients should know how the interface looks like)
 *
 * Returns:
 *   LIB_SUCCESS            - If requested Library with the requested version found
 *   LIB_ERR_INVALID_PARAM  - Invalid parameters
 *   LIB_ERR_NOT_FOUND      - Requested library is not found
 *   LIB_CLOSE_MATCH_FOUND  - Exact requested version is not found, but a later revision of lib
 *                            with same major version number found.
 *   LIB_ERR_INCOMPATIBLE_LOADER - Loader replaced with incompatible version
 *
 * */
typedef EFI_STATUS (*LOAD_LIB) (IN  CHAR8     *LibName,
                           IN  UINT32     LibVersion,
                           OUT VOID     **LibIntfPtr);

/* Shared Library Loader interface that's available in the system as shared lib itself */
typedef struct {
   UINT32          LoaderVersion;
   INSTALL_LIB     InstallLib;
   LOAD_LIB        LoadLib;
}ShLibLoaderType;

/*
 * This function returns the Shared Library Installer/Loader. The implementation is shared
 * among the users as shared library.
 * */
ShLibLoaderType* GetShLibLoader (VOID);

#endif /* __SH_LIB__H__ */

