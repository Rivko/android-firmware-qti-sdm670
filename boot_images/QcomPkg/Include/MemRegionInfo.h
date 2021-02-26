/** @file MemRegionInfo.h
   
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/10/14   vk      Initial Version

=============================================================================*/
#ifndef __MEM_REGION_INFO__
#define __MEM_REGION_INFO__

#include <Uefi.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/ArmLib.h>

typedef enum 
{
  AddMem,                           // Add memory as specified
  AddMemForNonDebugMode,            // Add memory if debug mode is not enabled
  AddMemForNonDebugOrNonCrashMode,  // Add memory if debug mode is not enabled or
                                    // if debug mode is enabled but crash did not happen
  ReserveMemForNonCrashMode,        // Reserve memory if crash did not happen
  NoBuildHob,                       // Do not build HOB for this region
  AddPeripheral,                    // Hob, Cache Settings
  HobOnlyNoCacheSetting,            // Hob, No Cache Settings
  CacheSettingCarveOutOnly,         // No Hob, Cache Setting in DEBUG only
  AllocOnly,
  ErrorBuildHob = 99                // Return high invalid value on error
} BUILD_HOB_OPTION_TYPE;

/* Below flag is used for system memory, in platform config file */
#define SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS1                     \
                EFI_RESOURCE_ATTRIBUTE_PRESENT |                 \
                EFI_RESOURCE_ATTRIBUTE_INITIALIZED |             \
                EFI_RESOURCE_ATTRIBUTE_TESTED

//EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTED

#define SYSTEM_MEMORY_RESOURCE_ATTR_CAPABILITIES1                 \
                EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |             \
                EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |       \
                EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
                EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |    \
                EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTABLE |   \
                EFI_RESOURCE_ATTRIBUTE_READ_PROTECTABLE |        \
                EFI_RESOURCE_ATTRIBUTE_WRITE_PROTECTABLE

#define SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES        \
        (SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS1 |                  \
        SYSTEM_MEMORY_RESOURCE_ATTR_CAPABILITIES1)

#define MAX_MEM_LABEL_NAME 32

typedef struct
{
  CHAR8                        Name[MAX_MEM_LABEL_NAME];   // Region Name in ASCII
  UINT64                       MemBase;                    // Offset to DDR memory base
  UINT64                       MemSize;                    // Size (in bytes) of the memory region
  BUILD_HOB_OPTION_TYPE        BuildHobOption;             // Option to build HOB
  EFI_RESOURCE_TYPE            ResourceType;               // Resource type   
  EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttribute;          // Resource attribute
  EFI_MEMORY_TYPE              MemoryType;                 // Memory type, applicable for EFI_RESOURCE_SYSTEM_MEMORY only
  ARM_MEMORY_REGION_ATTRIBUTES CacheAttributes;            // Cache attributes
} MemRegionInfo;

#endif /* __MEM_REGION_INFO__ */
