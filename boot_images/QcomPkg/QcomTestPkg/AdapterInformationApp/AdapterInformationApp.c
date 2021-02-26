/** 
                    AdapterInformationApp.c

  AdapterInformationApp.c is responsible for testing AdapterInformationDxe driver.

  Copyright (c) 2013, 2015, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 03/04/15   sk       Updated test prints for HSTI
 01/08/15   sk       Updated for HSTI
 12/10/13   sm       Updated print statements
 11/11/13   sm       Added Test Interface macros
 07/10/13   shl      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/EFIAdapterInformation.h>
#include <Protocol/EFIAdapterInformationRegister.h>
#include <Library/TestInterface.h>

// HSTI:
#include <Library/SecHwTestLib.h>

EFI_ADAPTER_INFORMATION_PROTOCOL          *pAdapterInformationProtocol = NULL;
EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL *pAdapterInformationRegisterProtocol = NULL;

// test GUIDs
static EFI_GUID TestGuid[] = { \
  { 0x59c9ace0, 0x7913, 0x4d2c, { 0x91, 0x34, 0x50, 0x57, 0x9, 0x49, 0x4b, 0x4 } },  \
  { 0x6e24c32,  0x7f8f, 0x4f4a, { 0x8e, 0x63, 0xbd, 0x86, 0x1, 0x8c, 0xe, 0xab } }  \
};

// HSTI:
//InformationType
#define ADAPTER_INFO_PLATFORM_SECURITY_GUID {0x6be272c7, 0x1320, 0x4ccd, { 0x90, 0x17, 0xd4, 0x61, 0x2c, 0x01, 0x2b, 0x25 }}

// test data
UINT8 InfoData[]={0,1};
UINT8 InfoDataInput[]={100,101};

// test get info callback function
EFI_STATUS GetInfo 
(
  OUT VOID         **InformationBlock,
  OUT UINTN         *InformationBlockSize
)
{
  if(InformationBlock == NULL || InformationBlockSize == NULL )
  {
    return EFI_INVALID_PARAMETER;
  }

  *InformationBlockSize = sizeof(InfoData);

  *InformationBlock = (EFI_GUID *)AllocatePool(*InformationBlockSize);
  if(  *InformationBlock == NULL )
  {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem((UINT8 *)(*InformationBlock), InfoData,  *InformationBlockSize);
  DEBUG(( EFI_D_WARN,"GetInfo() is called. \r\n"));
  return EFI_SUCCESS;
}

// test set info callback function
EFI_STATUS SetInfo 
(
  IN VOID         *InformationBlock,
  IN UINTN         InformationBlockSize
)
{
  if(InformationBlock == NULL || InformationBlockSize == 0 )
  {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem( InfoData, (UINT8 *)InformationBlock, InformationBlockSize);

  DEBUG(( EFI_D_WARN,"SetInfo() is called. \r\n"));
  return EFI_SUCCESS;
}


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
AdapterInformationAppMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;  
  EFI_GUID      *InfoTypesBuffer = NULL;
  UINTN          InfoTypesBufferCount = 0;
  VOID          *InfoBuffer = NULL;
  UINTN          InfoBufferCount = 0;
  UINTN          InputSize;
  UINT32         i,j;

  // HSTI:
  // Temp variable to store status before HSTI testing. 
  // This is to ensure that HSTI failure will not affect normal testing. 
  // This temp variable will be removed in future builds
  EFI_STATUS     PreHstiStatus;  
  ADAPTER_INFO_PLATFORM_SECURITY TempObject;


  // skubair: Below needed for HSTI testing
  EFI_GUID       AdapterGuid = (EFI_GUID)ADAPTER_INFO_PLATFORM_SECURITY_GUID;

  TEST_START("AdapterInformationApp");

  Status = gBS->LocateProtocol(&gAdapterInformationRegisterProtocolGuid, NULL, (VOID **) &pAdapterInformationRegisterProtocol);
  PreHstiStatus = Status; // Initialize PreHstiStatus for the first time

  if ( EFI_ERROR (Status) || pAdapterInformationRegisterProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN,"ERROR: Locate AdapterInformationRegister protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  Status = gBS->LocateProtocol(&gAdapterInformationProtocolGuid, NULL, (VOID **) &pAdapterInformationProtocol);

  if ( EFI_ERROR (Status) || pAdapterInformationProtocol == NULL )
  {
    DEBUG(( EFI_D_WARN,"ERROR: Locate AdapterInformation protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // AdapterInformationRegisterProtocol test case 1
  Status = pAdapterInformationRegisterProtocol->RegisterService( pAdapterInformationRegisterProtocol, 
	                                                       &TestGuid[0],
														   &GetInfo,
														   &SetInfo );
  if ( Status == EFI_INVALID_PARAMETER)
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 1 pass. \r\n"));
  }
  else
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 1 fail. \r\n"));
  }

  // AdapterInformationRegisterProtocol test case 2
  Status = pAdapterInformationRegisterProtocol->RegisterService( pAdapterInformationRegisterProtocol, 
	                                                       &TestGuid[0],
														   &GetInfo,
														   NULL );
  if ( Status == EFI_SUCCESS)
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 2 pass. \r\n"));
  }
  else
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 2 fail. \r\n"));
  }

  // AdapterInformationRegisterProtocol test case 3
  Status = pAdapterInformationRegisterProtocol->RegisterService( pAdapterInformationRegisterProtocol, 
	                                                       &TestGuid[1],
														   NULL,
														   &SetInfo );
  if ( Status == EFI_SUCCESS)
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 3 pass. \r\n"));
  }
  else
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 3 fail. \r\n"));
  }

  // AdapterInformationProtocol test case 1
  Status = pAdapterInformationProtocol->GetSupportedTypes( pAdapterInformationProtocol, 
	                                                       &InfoTypesBuffer,
														   &InfoTypesBufferCount );
  if ( Status == EFI_SUCCESS)
  {
    if( InfoTypesBufferCount == 2 )
	{
      for( i=0; i<2; i++)
	  {
        if(CompareGuid( InfoTypesBuffer+i, &TestGuid[i]) )
        {
          continue;   
        }
		else
		{
          DEBUG(( EFI_D_WARN,"GetSupportedTypes test fail. \r\n"));
		  goto ErrorExit;
		}
	  }
      DEBUG(( EFI_D_WARN,"GetSupportedTypes test pass. \r\n"));
	}
  }
  else
  {
    DEBUG(( EFI_D_WARN,"RegisterService test 1 fail. \r\n"));
  }

  // release InfoTypesBuffer, caller's responsibility
  if( InfoTypesBuffer != NULL )
  {
    FreePool(InfoTypesBuffer);
    InfoTypesBuffer = NULL;
  }

  // AdapterInformationProtocol test case 2
  Status = pAdapterInformationProtocol->GetInformation( pAdapterInformationProtocol, 
	                                                       &TestGuid[0],
	                                                       &InfoBuffer,
														   &InfoBufferCount );
  if ( Status == EFI_SUCCESS)
  {
    for( i=0; i<InfoBufferCount; i++)
	{
      DEBUG(( EFI_D_WARN,"Get InfoBuffer. %d \r\n", ((UINT8 *)InfoBuffer)[i]));
	}
  }
  else
  {
    DEBUG(( EFI_D_WARN,"GetAdapterInformation test fail. \r\n"));
  }

  // release InfoBuffer, caller's responsibility
  if( InfoBuffer != NULL )
  {
    FreePool(InfoBuffer);
    InfoBuffer = NULL;
  }

  // AdapterInformationProtocol test case 3
  InputSize = sizeof(InfoDataInput);
  Status = pAdapterInformationProtocol->SetInformation( pAdapterInformationProtocol, 
	                                                   &TestGuid[1],
	                                                    InfoDataInput,
                                                        InputSize );
  if ( Status == EFI_SUCCESS)
  {
     DEBUG(( EFI_D_WARN,"SetAdapterInformation test pass. \r\n"));
  }
  else
  {
     DEBUG(( EFI_D_WARN,"SetAdapterInformation test fail. \r\n"));
  }

  // AdapterInformationProtocol test case 4
  Status = pAdapterInformationProtocol->GetInformation( pAdapterInformationProtocol, 
	                                                       &TestGuid[0],
	                                                       &InfoBuffer,
														   &InfoBufferCount );
  if ( Status == EFI_SUCCESS)
  {
    for( i=0; i<InfoBufferCount; i++)
	{
      DEBUG(( EFI_D_WARN,"Get InfoBuffer. %d \r\n", ((UINT8 *)InfoBuffer)[i]));
	}
  }
  else
  {
    DEBUG(( EFI_D_WARN,"GetAdapterInformation test fail. \r\n"));
  }

  // release InfoBuffer, caller's responsibility
  if( InfoBuffer != NULL )
  {
    FreePool(InfoBuffer);
    InfoBuffer = NULL;
  }
  
  // HSTI:

  PreHstiStatus = Status;    // Store value of Status in PreHstiStatus, before HSTI test
  
  // skubair: Below needed for HSTI testing
  //////////////////
  Status = gBS->LocateProtocol(&gAdapterInformationProtocolGuid, NULL, (VOID **) &pAdapterInformationProtocol);

  if ( EFI_ERROR (Status) || pAdapterInformationProtocol == NULL )
  {
    DEBUG(( EFI_D_WARN,"HSTI ERROR: Locate AdapterInformation protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  Status = pAdapterInformationProtocol->GetInformation( pAdapterInformationProtocol, 
                                                        &AdapterGuid,
                                                        &InfoBuffer,
                                                        &InfoBufferCount );
  if ( Status == EFI_SUCCESS)
  {
    DEBUG(( EFI_D_WARN,"\r\n ===HSTI output=== \r\n"));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->Version: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->Version));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->Role: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->Role));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->ImplementationID: "));
    for( i = 8, j = 0; j < sizeof(TempObject.ImplementationID); j+=2)
    {
      AsciiPrint("%c", ((CHAR8 *)InfoBuffer)[i + j]);
    }
    DEBUG(( EFI_D_WARN,"\r\n"));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->SecurityFeaturesSize: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->SecurityFeaturesSize));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->SecurityFeaturesRequired: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->SecurityFeaturesRequired[0]));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->SecurityFeaturesImplemented: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->SecurityFeaturesImplemented[0]));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->SecurityFeaturesVerified: %x \r\n", ((PADAPTER_INFO_PLATFORM_SECURITY)(UINT8 *)InfoBuffer)->SecurityFeaturesVerified[0]));
    DEBUG(( EFI_D_WARN,"HSTI: InfoBuffer->ErrorString: "));
    for( i = 530, j = 0; j < sizeof(TempObject.ErrorString); j+=2)
    {
      AsciiPrint("%c", ((CHAR8 *)InfoBuffer)[i + j]);
    }
    DEBUG(( EFI_D_WARN,"\r\n"));

    //for( i=0; i<InfoBufferCount; i++)
    //{
    //  DEBUG(( EFI_D_WARN,"%c", ((UINT8 *)InfoBuffer)[i]));
    //}
    DEBUG(( EFI_D_WARN,"\r\n"));
  }
  else
  {
    DEBUG(( EFI_D_WARN,"HSTI: GetAdapterInformation test fail. \r\n"));
  }

  // release InfoBuffer, caller's responsibility
  if( InfoBuffer != NULL )
  {
    FreePool(InfoBuffer);
    InfoBuffer = NULL;
  }
  //////////////////

ErrorExit:
  // HSTI:  
  Status = PreHstiStatus;    // Restore value of Status as it was before HSTI test
  TestStatus("AdapterInformationApp", Status);
  TEST_STOP("AdapterInformationApp");  

  return Status;
}
