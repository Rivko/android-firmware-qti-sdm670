/** @file DelBootVars.c
   
  App for delete spec defined boot variables. valid for UEFI spec 2.6 and older.

  Copyright (c)  2017 Qualcomm Technologies Inc. All rights reserved.
                                                                  

**/

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------

 3/16/17    pawans	initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h> 
#include <Library/PrintLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BdsLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#define TEMP_BUFFER_SZ     512
#define VARIABLE_ATTRIBUTE (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS )

extern EFI_GUID gEfiGlobalVariableGuid;

struct Link {
	EFI_GUID 	VendorGuid;
	CHAR16* 	VariableName;
	UINTN 		VariableNameSize;
	struct Link *Next;
}*Start = NULL;


//
// this Function gets the list of all UEFI spec 2.6 defined Boot variable  
//

EFI_STATUS
GetNextVariableNameEx (
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  struct Link 		*Temp = NULL, *TempCur = NULL;
  CHAR16			*VariableName = NULL;
  UINTN 			VariableNameSize;
  EFI_GUID 			VendorGuid;
  UINTN         	Index;
  UINTN         	BootOrderSize;
  UINTN         	BootOrderCount = 0;
  UINT16*       	BootOrder = NULL; 
  VariableName =  AllocateZeroPool(TEMP_BUFFER_SZ);
  if(VariableName == NULL)
  	return EFI_OUT_OF_RESOURCES;
  Status = GetGlobalEnvironmentVariable(L"BootOrder", NULL, &BootOrderSize, (VOID**)&BootOrder);

	if ( Status == EFI_SUCCESS && BootOrder != NULL ) 
	{
		BootOrderCount = BootOrderSize / sizeof(UINT16);
	}
	else
	{
		return Status;
	}
	Status = EFI_SUCCESS;

  	while (Status != EFI_NOT_FOUND) {

		VariableNameSize = TEMP_BUFFER_SZ;	
		Status  = gRT->GetNextVariableName (&VariableNameSize, VariableName, &VendorGuid);
		if(Status ==  EFI_SUCCESS) {
		
			Temp = (struct Link*)AllocateZeroPool(sizeof(struct Link));
			if(Temp == NULL)
				return EFI_OUT_OF_RESOURCES;
			if((StrnCmp(VariableName, L"BootOrder", StrLen(L"BootOrder")) == 0 ) || 
					((StrnCmp(VariableName, L"BootNext", StrLen(L"BootNext")) == 0 )) || 
					((StrnCmp(VariableName, L"BootCurrent", StrLen(L"BootCurrent")) == 0 )) || 
					((StrnCmp(VariableName, L"BootOptionSupport", StrLen(L"BootOptionSupport")) == 0 ))) {
				
						Temp->VariableName = AllocateZeroPool(TEMP_BUFFER_SZ);
						if(Temp->VariableName == NULL)
							return EFI_OUT_OF_RESOURCES;
						CopyMemS(Temp->VariableName, VariableNameSize, (CHAR16*)VariableName, VariableNameSize);
						Temp->VariableNameSize = VariableNameSize;			
						Temp->VendorGuid = VendorGuid;
						Temp->Next = NULL;		
						if (Start == NULL) {
							Start = Temp;
							TempCur = Start;
						}
						else 
						{
							if(TempCur)
								TempCur->Next = Temp;
							TempCur = Temp;
						}
				}
				else  //check if current variable is present in bootorder
				{	
					CHAR16* 		OptionName = AllocateZeroPool(TEMP_BUFFER_SZ);
					if(OptionName == NULL)
						return EFI_OUT_OF_RESOURCES;
					for (Index = 0; Index < BootOrderCount; Index++) 
					{
						UnicodeSPrint (OptionName, TEMP_BUFFER_SZ, L"Boot%04x", BootOrder[Index]);
						OptionName[sizeof(OptionName)] ='\0';
						if(!StrnCmp(OptionName, VariableName, StrLen(VariableName)))
						 {
							Temp->VariableName = AllocateZeroPool(TEMP_BUFFER_SZ);
							if(Temp->VariableName == NULL)
								return EFI_OUT_OF_RESOURCES;
							CopyMemS(Temp->VariableName, VariableNameSize, (CHAR16*)VariableName, VariableNameSize);
							Temp->VariableNameSize = VariableNameSize;			
							Temp->VendorGuid = VendorGuid;	
							Temp->Next = NULL;			
							if (Start == NULL) {
								Start = Temp;
								TempCur = Start;
							}
							else 
							{
								if(TempCur)
									TempCur->Next = Temp;
								TempCur = Temp;
							}
						 }
					}

				FreePool (OptionName);
				}	
		}
		else if(Status == EFI_BUFFER_TOO_SMALL)
		{
			AsciiPrint("buffer too small !!\n");
		}
		else if(Status == EFI_INVALID_PARAMETER)
		{
			AsciiPrint("Invalid params");
			return Status;
		}
		else if(Status == EFI_DEVICE_ERROR)
		{
			AsciiPrint("hardware error!");
			return Status;
		}
	}
	if(Temp)
		Temp->Next = NULL;
	FreePool (VariableName);
	return EFI_SUCCESS;
}

//
// Delete Uefi spec definde BOOT variables
//

EFI_STATUS
EFIAPI DeleteVariable( 
)
{
	if(Start == NULL)
		return EFI_SUCCESS;

	EFI_STATUS Status = EFI_SUCCESS;	
	struct Link 	*DeleteVar = Start;

	while (DeleteVar != NULL){
		Status = gRT->SetVariable(DeleteVar->VariableName, &DeleteVar->VendorGuid, VARIABLE_ATTRIBUTE, 0, NULL);
		AsciiPrint("%s", DeleteVar->VariableName);
		if(Status == EFI_SUCCESS)
		{
			AsciiPrint(" Deleted\n");
		}
		else
		{
			AsciiPrint(" Delete Failed");
			return Status;
		}
		struct Link *prev = DeleteVar;
		DeleteVar = DeleteVar->Next;
		FreePool(prev);
	}
	// FreePool(Start);
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DelBootVars (
		  IN EFI_HANDLE        ImageHandle,
		  IN EFI_SYSTEM_TABLE  *SystemTable
		)
{
	EFI_STATUS Status = EFI_SUCCESS;
	TEST_START("DelBootVars");
	Status = GetNextVariableNameEx();
	if(Status != EFI_SUCCESS || Start == NULL)
	{
		AsciiPrint("%r", Status);
		goto Done;
	}
	else
	{
		Status = DeleteVariable();
	}
	Done:
	TestStatus("DelBootVars", Status);
	TEST_STOP("DelBootVars");
  return Status;
}
