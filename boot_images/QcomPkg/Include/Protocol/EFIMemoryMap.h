/** 
  @file  EFIMemoryMap.h
  @brief Subsystem Memory Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/20/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 10/04/11   jz      Initial Revision

=============================================================================*/

#ifndef __MEMORYMAP_H__
#define __MEMORYMAP_H__

/** @addtogroup efi_subsystem_memory_constants 
@{ */
/** Protocol version. */
#define EFI_MEMORYMAP_PROTOCOL_REVISION 0x0000000000010000

/** Descriptor version. */
#define EFI_SUBSYSTEM_MEMORY_DESCRIPTOR_REVISION 0x00000000
/** @} */ /* end_addtogroup efi_subsystem_memory_constants */

/* Protocol declaration */
/** @cond */
typedef struct _EFI_SUBSYSTEM_MEMORY_PROTOCOL EFI_SUBSYSTEM_MEMORY_PROTOCOL;
/** @endcond */

/** @ingroup efi_subsystem_memory_data_types
  Subsystem memory descriptor.
*/
typedef struct {
  UINT32 Revision;      /**< Revision to which this structure adheres. */
  EFI_GUID SubsystemIdentifier;             /**< Subsystem identifier. */
  EFI_PHYSICAL_ADDRESS PhysicalBaseAddress; /**< Physical base address. */
  EFI_VIRTUAL_ADDRESS VirtualBaseAddress;   /**< Virtual base address. */
  UINT64 PageCount;                         /**< Page count. */
  UINT64 Attribute;                         /**< Attribute. */
} EFI_SUBSYSTEM_MEMORY_DESCRIPTOR;
  
/* External reference to the subsystem memory map Protocol GUID */
/** @ingroup efi_subsystem_memory_protocol */
extern EFI_GUID gEfiMemoryMapProtocolGuid;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SUBSYSTEM_MEMORY_GET_SUBSYSTEM_MEMORY_MAP */
/** @ingroup efi_subsys_mem_get_subsystem_memory_map
  @par Summary
  Returns information about the platform memory map, categorized by subsystems.
  
  @param[in]     This      Pointer to the EFI_SUBSYSTEM_MEMORY_PROTOCOL 
                           instance.
  @param[in,out] Size      Pointer to the size, in bytes, of the MemoryMap 
                           buffer.
  @param[in,out] MemoryMap Pointer to the buffer for the subsystem memory 
                           map, which is an array of 
                           EFI_SUBSYSTEM_MEMORY_DESCRIPTOR.
  @param[out] DescriptorCount  Pointer to the location containing the 
                               descriptor count.
  
  @return
  EFI_SUCCESS           -- Function completed successfully.
  @par
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  @par
  EFI_BUFFER_TOO_SMALL  -- Buffer was not large enough to obtain the array. The 
                           required buffer size is returned in the Size 
                           parameter.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SUBSYSTEM_MEMORY_GET_SUBSYSTEM_MEMORY_MAP)(
  IN EFI_SUBSYSTEM_MEMORY_PROTOCOL       *This,
  IN OUT UINTN                           *Size,
  IN OUT EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *MemoryMap,
  OUT UINTN                              *DescriptorCount
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_subsystem_memory_protocol
  @par Summary
  Subsystem Memory Protocol interface.
  
  @par Parameters
  @inputprotoparams{memory_map_proto_params.tex} 
*/
struct _EFI_SUBSYSTEM_MEMORY_PROTOCOL
{
  UINT64                                        Revision;
  EFI_SUBSYSTEM_MEMORY_GET_SUBSYSTEM_MEMORY_MAP GetSubsystemMemoryMap;
};

#endif /* __MEMORYMAP_H__ */
