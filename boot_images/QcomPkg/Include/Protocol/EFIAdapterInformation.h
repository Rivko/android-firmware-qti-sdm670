/** @file
   
 Declaration of Adapter Information Driver Interface, this is defined in UEFI SPecs 2.4

  Copyright (c) 2013 - 2015, Qualcomm Technologies, Inc. All rights reserved.

  Portions Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 02/27/15    sk     Removed 'revision' field
 02/25/15    vk     Fix LLVM warning
 10/14/14    sk     Updated copyright
 02/03/14    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 06/11/13    shl    Initial version.

=============================================================================*/
#ifndef __EFI_ADAPTER_INFORMATION_PROTOCOL_H__
#define __EFI_ADAPTER_INFORMATION_PROTOCOL_H__

/** @cond */
typedef struct _EFI_ADAPTER_INFORMATION_PROTOCOL EFI_ADAPTER_INFORMATION_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ADAPTER_INFORMATION_GET_INFO */
/** @ingroup efi_adapter_information_get_info
  @par Summary
  Gets the device state information from the adapter and returns the current 
  state information of the adapter.

  @param[in] This                  Pointer to the 
                                   EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[in] InformationType       Pointer to an EFI_GUID that defines the 
                                   contents of InformationBlock. The caller 
                                   must use InformationType to specify the 
                                   information it wants from the service.
  @param[out] InformationBlock     The service returns a pointer to the 
                                   buffer with the InformationBlock structure 
                                   that contains details about the data 
                                   specific to InformationType. \n
                                   @vertspace{3}
                                   This structure is defined based on the type 
                                   of data returned and is different for 
                                   different data types. The service and 
                                   caller decode this structure and its 
                                   contents based on InformationType. \n
                                   @vertspace{3}
                                   The buffer is allocated by the service, 
                                   and it is the responsibility of the caller 
                                   to free the buffer after using it.
  @param[out] InformationBlockSize The driver returns the size of 
                                   InformationBlock in bytes.

  @return
  EFI_SUCCESS           -- InformationType information was retrieved.
  @par
  EFI_DEVICE_ERROR      -- Device reported an error.
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources.
  @par
  EFI_INVALID_PARAMETER -- Possible descriptions are:
                           - The <i>This</i> parameter is NULL
                           - The <i>InformationBlock</i> parameter is NULL
                           - The <i>InformationBlockSize</i> parameter is NULL

*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFORMATION_GET_INFO)(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  IN  EFI_GUID                          *InformationType,
  OUT VOID                             **InformationBlock,
  OUT UINTN                             *InformationBlockSize
);


/* EFI_ADAPTER_INFORMATION_SET_INFO */
/** @ingroup efi_adapter_information_set_info 
  @par Summary
  Sets the state information for an adapter. 

  @param[in] This                  Pointer to the 
                                   EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[in] InformationType       Pointer to an EFI_GUID that defines the 
                                   contents of InformationBlock. The caller 
                                   must use InformationType to specify the 
                                   information it wants from the service.
  @param[in] InformationBlock      Pointer to the InformationBlock structure 
                                   that contains details about the data 
                                   specific to InformationType. \n
                                   @vertspace{3}
                                   This structure is defined based on the type 
                                   of data sent and is different for 
                                   different data types. The driver and 
                                   caller decode this structure and its 
                                   contents based on InformationType. \n
                                   @vertspace{3}
                                   The buffer is allocated by the caller. It 
                                   is the responsibility of the caller to free 
                                   the buffer after the caller has set the 
                                   requested parameters.
  @param[in] InformationBlockSize  Size of InformationBlock in bytes.

  @return
  EFI_SUCCESS           -- Information was received and interpreted successfully.
  @par
  EFI_UNSUPPORTED       -- InformationType parameter is not known.
  @par
  EFI_DEVICE_ERROR      -- Device reported an error.
  @par
  EFI_INVALID_PARAMETER -- Possible descriptions are:
                           - The <i>This</i> parameter is NULL
                           - The <i>InformationBlock</i> parameter is NULL
  @par
  EFI_WRITE_PROTECTED   -- InformationType parameter cannot be modified using 
                           %EFI_ADAPTER_INFORMATION_SET_INFO().

*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFORMATION_SET_INFO)(
  IN EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  IN EFI_GUID                          *InformationType,
  IN VOID                              *InformationBlock,
  IN UINTN                              InformationBlockSize
);


/* EFI_ADAPTER_INFORMATION_GET_SUPPORTED_TYPES */
/** @ingroup efi_adapter_information_get_supported_types
   Gets a list of supported information types for this instance of the protocol.

  @param[in]  This                  Pointer to the 
                                    EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[out] InfoTypesBuffer       Pointer to the list of InformationType 
                                    GUID pointers that are supported by 
                                    the This parameter. The buffer is allocated 
                                    by the service, and it is the responsibility 
                                    of the caller to free the buffer after 
                                    using it.
  @param[out] InfoTypesBufferCount  Pointer to the number of GUID pointers 
                                    present in InfoTypesBuffer.

  @return
  EFI_SUCCESS           -- The list of information type GUIDs that are 
                           supported on this adapter was returned in 
                           InfoTypesBuffer. The number of information
                           type GUIDs was returned in InfoTypesBufferCount.
  @par
  EFI_INVALID_PARAMETER -- Possible descriptions are:
                           - The <i>This</i> parameter is NULL
                           - The <i>InfoTypesBuffer</i> parameter is NULL
                           - The <i>InfoTypesBufferCount</i> parameter is NULL
  @par
  EFI_OUT_OF_RESOURCES  -- Not enough pool memory to store the results.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFORMATION_GET_SUPPORTED_TYPES)(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  OUT EFI_GUID                         **InfoTypesBuffer,
  OUT UINTN                             *InfoTypesBufferCount
) ;


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_adapterInfo_protocol
  @par Summary
  Adapter Information Protocol interface.

  @par Parameters
  @inputprotoparams{adapterInfo_proto_params.tex} 
*/
struct _EFI_ADAPTER_INFORMATION_PROTOCOL
{
  EFI_ADAPTER_INFORMATION_GET_INFO             GetInformation;
  EFI_ADAPTER_INFORMATION_SET_INFO             SetInformation;
  EFI_ADAPTER_INFORMATION_GET_SUPPORTED_TYPES  GetSupportedTypes;
};


/** @ingroup efi_adapterInfo_protocol */
extern EFI_GUID gAdapterInformationProtocolGuid;

#endif
