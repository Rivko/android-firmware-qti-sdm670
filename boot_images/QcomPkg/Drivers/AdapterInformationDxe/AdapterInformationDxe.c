/** @file
   
 Implememtation of adapter information DXE Driver 

 EFI_ADAPTER_INFORMATION_PROTOCOL is defined in UEFI Specs 2.4

  Copyright (c) 2013, 2015, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Portions Copyright (c) 2015, Intel Corporation. All rights reserved.<BR> 
  This program and the accompanying materials are licensed and made available under 
  the terms and conditions of the BSD License that accompanies this distribution. 
  The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php. 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 


**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 02/27/15    sk      Removed 'revision' field
 12/10/13    sm      Updated size types
 06/18/13    shl     Initial version.

=============================================================================*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIAdapterInformation.h>
#include <Protocol/EFIAdapterInformationRegister.h>


typedef struct 
{
  EFI_GUID                              InformationType;
  AdapterInformationGetCallbackPtr      GetInformationCallback;
  AdapterInformationSetCallbackPtr      SetInformationCallback;
} AdapterInformationTableType;

// local table for recording Listener info
#define MAX_ADAPTER_INFORMATION_NUM   10
static  AdapterInformationTableType   Table[MAX_ADAPTER_INFORMATION_NUM];
static  UINT32                        CurrentTableIndex = 0;

/** 
  Get device state information from adapter returns the current state information for the adapter.

  @param[in] This                  A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[in] InformationBlock      This service returns a pointer to the buffer with the 
                                   InformationBlock structure which contains details about 
                                   the data specific to InformationType. This structure is
                                   defined based on the type of data returned, and will be 
                                   different for different data types. This service and caller 
                                   decode this structure and its contents based on InformationType.
                                   This buffer is allocated by this service, and it is the responsibility
                                   of the caller to free it after using it.
  @param[out] InformationBlockSize The driver returns the size of the InformationBlock in bytes.

  @return
    EFI_SUCCESS            The InformationType information was retrieved.
    EFI_DEVICE_ERROR       The device reported an error.
    EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources
	EFI_NOT_FOUND          The request info is not found
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  InformationBlock is NULL
    EFI_INVALID_PARAMETER  InformationBlockSize is NULL

*/
EFI_STATUS GetAdapterInformation
(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  IN  EFI_GUID                          *InformationType,
  OUT VOID                             **InformationBlock,
  OUT UINTN                             *InformationBlockSize
)
{
  EFI_STATUS     Status = EFI_NOT_FOUND;
  UINT32         i;

  if( This == NULL || InformationType == NULL || InformationBlock == NULL || InformationBlockSize == NULL )
  {
    return EFI_INVALID_PARAMETER;
  }

  for( i=0; i < CurrentTableIndex; i++)
  {
    if( CompareGuid (InformationType, &(Table[i].InformationType) ) )
    {
      // found one
      if( Table[i].GetInformationCallback != NULL )
	  {
	    Status = Table[i].GetInformationCallback( InformationBlock, InformationBlockSize);
		goto Exit;
	  }  
    }    
  }

Exit:
  return Status;
}


/** 
  Set state information for an adapter. 

  @param[in] This                  A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[in] InformationType       A pointer to an EFI_GUID that defines the contents of 
                                   InformationBlock. The caller must use the InformationType
                                   to specify the information it wants the service.
  @param[in] InformationBlock      A pointer to the InformationBlock structure which contains
                                   details about the data specific to InformationType. This 
                                   structure is defined based on the type of data sent, and
                                   will be different for different data types. The driver and 
                                   caller decode this structure and its contents based on InformationType.
                                   This buffer is allocated by the caller. It is the responsibility of
                                   the caller to free it after the caller has set the requested parameters.
  @param[in] InformationBlockSize  The size of the InformationBlock in bytes.

  @return
    EFI_SUCCESS           The information was received and interpreted successfully.
    EFI_UNSUPPORTED       The InformationType is not known.
    EFI_DEVICE_ERROR      The device reported an error.
    EFI_INVALID_PARAMETER This is NULL
    EFI_INVALID_PARAMETER InformationBlock is NULL
    EFI_WRITE_PROTECTED   The InformationType cannot be modified using EFI_ADAPTER_INFO_SET_INFO()

*/
EFI_STATUS SetAdapterInformation
(
  IN EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  IN EFI_GUID                          *InformationType,
  IN VOID                              *InformationBlock,
  IN UINTN                              InformationBlockSize
)
{
  EFI_STATUS     Status = EFI_NOT_FOUND;
  UINT32         i;

  if( This == NULL || InformationType == NULL || InformationBlock == NULL )
  {
    return EFI_INVALID_PARAMETER;
  }

  for( i=0; i < CurrentTableIndex; i++)
  {
    if( CompareGuid (InformationType, &(Table[i].InformationType) ) )
    {
      // found one
      if( Table[i].SetInformationCallback != NULL )
	  {
	    Status = Table[i].SetInformationCallback( InformationBlock, InformationBlockSize);
		goto Exit;
	  }  
    }    
  }

Exit:
  return Status;
}


/** 
   Get a list of supported information types for this instance of the protocol.

  @param[in]  This                  A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance.
  @param[out] InfoTypesBuffer       A pointer to the list of InformationType GUID pointers that
                                    are supported by This. This buffer is allocated by this 
                                    service, and it is the responsibility of the caller to free it
                                    after using it
  @param[out] InfoTypesBufferCount  A pointer to the number of GUID pointers present in InfoTypesBuffer.

  @return
    EFI_SUCCESS            The list of information type GUIDs that are supported on this
                           adapter was returned in InfoTypesBuffer. The number of information
                           type GUIDs was returned in InfoTypesBufferCount.
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  InfoTypesBuffer is NULL
    EFI_INVALID_PARAMETER  InfoTypesBufferCount is NULL
    EFI_OUT_OF_RESOURCES   There is not enough pool memory to store the results

*/
EFI_STATUS GetAdapterInformationSupportedTypes
(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL  *This,
  OUT EFI_GUID                         **InfoTypesBuffer,
  OUT UINTN                             *InfoTypesBufferCount
) 
{
  UINTN          GuidSize = sizeof(EFI_GUID);
  UINT32         i;

  if( This == NULL || InfoTypesBufferCount == NULL || InfoTypesBuffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  *InfoTypesBufferCount = CurrentTableIndex;
 
  // need to allocate some buffer, this buffer will be released by the caller after the call is returned.
  if( CurrentTableIndex != 0)
  {
    *InfoTypesBuffer = (EFI_GUID *)AllocatePool(CurrentTableIndex*GuidSize);
	if( *InfoTypesBuffer == NULL )
	{
      return EFI_OUT_OF_RESOURCES;
	}
  }
  else
  {
    return EFI_SUCCESS;
  }

  for( i=0; i < CurrentTableIndex; i++)
  {
    CopyGuid ((*InfoTypesBuffer)+i, &(Table[i].InformationType) );
  }

  return EFI_SUCCESS;
}


EFI_ADAPTER_INFORMATION_PROTOCOL AdapterInformationProtocolImpl =
{
  GetAdapterInformation,
  SetAdapterInformation,
  GetAdapterInformationSupportedTypes
};


/** 
  Register callback function to support adapter information services.

  @param[in] This                    A pointer to the EFI_ADAPTER_INFORMATION_REGIDTER_PROTOCOL instance.
  @param[in] InformationType         A pointer to an EFI_GUID that defines the contents of 
                                     InformationBlock. The caller must use the InformationType
                                     to specify the information it wants the service.
  @param[in] InformationGetCallback  This is the pointer to get info callback function. Could be NULL.
                                     It it mutual exclusive with InformationSetCallback. One of them must be NULL.
  @param[in] InformationSetCallback  This is the pointer to set info callback function. Could be NULL.
                                     It it mutual exclusive with InformationGetCallback. One of them must be NULL. 

  @return
    EFI_SUCCESS            The InformationType information was retrieved.
    EFI_DEVICE_ERROR       The device reported an error.
    EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  InformationType is NULL

*/
EFI_STATUS
AdapterInformationRegister
(
  IN  EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL  *This,
  IN  EFI_GUID                                   *InformationType,
  IN  AdapterInformationGetCallbackPtr            GetInformationCallback,
  IN  AdapterInformationSetCallbackPtr            SetInformationCallback
)
{
  if( This == NULL || InformationType == NULL || ( GetInformationCallback != NULL && SetInformationCallback != NULL) )
  {
    return EFI_INVALID_PARAMETER;
  }

  if( CurrentTableIndex < MAX_ADAPTER_INFORMATION_NUM )
  {
    CopyGuid (&(Table[CurrentTableIndex].InformationType), InformationType );

    Table[CurrentTableIndex].GetInformationCallback = GetInformationCallback;
    Table[CurrentTableIndex].SetInformationCallback = SetInformationCallback;

    // increase the Index
    CurrentTableIndex++;

    return EFI_SUCCESS;
  }
  else
  {
    DEBUG(( EFI_D_WARN, "WARNING: Reached table limit, Please increase the table size. \r\n"));
    return EFI_OUT_OF_RESOURCES;    
  }
}

EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL AdapterInformationRegisterProtocolImpl =
{
  ADAPTER_INFORMATION_REGISTER_REVISION,
  AdapterInformationRegister
};

  
/**
  Adapter Information driver initialization, it is the entry point oof this driver.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       Pointer to the System Table

  @retval EFI_SUCCESS       Protocol registered
  @retval Others            Initialization failed.

**/
EFI_STATUS
EFIAPI
AdapterInformationDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  // Install AdapterInformation protocols. 
  Status = gBS->InstallMultipleProtocolInterfaces (
                      &ImageHandle,
                      &gAdapterInformationRegisterProtocolGuid,
                      (void **)&AdapterInformationRegisterProtocolImpl,
                      &gAdapterInformationProtocolGuid,
                      (void **)&AdapterInformationProtocolImpl,
                       NULL
                    );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "AdapterInformationDxeInitialize: Install Protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

ErrorExit:
  return Status;
}
