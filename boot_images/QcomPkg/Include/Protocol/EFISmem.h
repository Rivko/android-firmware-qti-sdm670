/** 
  @file  EFISmem.h
  @brief Shared Memory Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 01/26/12   vishalo Merge in Techpubs Doxygen chang
 10/25/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 04/15/11   tl      Created DXE driver for shared memory

=============================================================================*/
#ifndef __EFISMEM_H__
#define __EFISMEM_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "smem_type.h"
/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_smem_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_SMEM_PROTOCOL_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_smem_constants */

/*  Protocol GUID definition */
/** @ingroup efi_smem_protocol */
#define EFI_SMEM_PROTOCOL_GUID \
  { 0xf4e5c7d0, 0xd239, 0x47cb, { 0xaa, 0xcd, 0x7f, 0x66, 0xef, 0x76, 0x32, 0x38 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC SMEM Protocol GUID.
 */
extern EFI_GUID gEfiSMEMProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SMEM_ALLOC */ 
/** @ingroup efi_smem_alloc
  @par Summary
  Allocates a shared memory item.

  @param[in]   item    Shared memory item to allocate. 
  @param[in]   size    Size to allocate.
  @param[out]  addr    Address in shared memory of the requested item.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef EFI_STATUS (EFIAPI *EFI_SMEM_ALLOC)(
  IN smem_mem_type        item,
  IN UINT32               size,
  OUT void               **addr
  );

/* EFI_SMEM_GET_ADDR */ 
/** @ingroup efi_smem_get_addr
  @par Summary
  Returns the address of an item if it has been allocated in shared memory.

  @param[in]   item    Shared memory item.
  @param[out]  size    Size of the allocated item.
  @param[out]  addr    If the item has been allocated, the address in shared 
                       memory of the requested item.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef EFI_STATUS (EFIAPI *EFI_SMEM_GET_ADDR)(
  IN smem_mem_type        item,
  OUT UINT32              *size,
  OUT void               **addr
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_smem_protocol
  @par Summary
  Shared Memory Protocol interface.

  @par Parameters
  @inputprotoparams{smem_proto_params.tex}
*/
typedef struct _EFI_SMEM_PROTOCOL {
  UINT64                Revision;
  EFI_SMEM_ALLOC        SmemAlloc;
  EFI_SMEM_GET_ADDR     SmemGetAddr;
} /** @cond */EFI_SMEM_PROTOCOL/** @endcond */;

#endif /* EFISMEM_H */
