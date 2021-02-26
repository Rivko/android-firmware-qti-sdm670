/*
  @file WinAcpiUpdate.c
	Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
	All Rights Reserved.
	Qualcomm Technologies Confidential and Proprietary.
  ============================================================================*/
  
  /*=============================================================================
								EDIT HISTORY
  
  
   when 	  who	  what, where, why
   --------   ---	  -----------------------------------------------------------
   02/17/17   vinitb  Added qccdi option related changes
   01/30/17   vinitb  Added sensors/gpu option related changes
   09/30/16   ltg     Added full implementation for patching ACPI, updating UEFI variable, and protocol API's
   08/4/16	 vahidd   Intial Draft for Patching DSDT tables.
  =============================================================================*/


#include <PiDxe.h>
#include <Protocol/AcpiTable.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Library/UefiCfgLib.h>
#include <IndustryStandard/Acpi.h>

/**
WinAcpiUpdate PROTOCOL interfaces
*/
#include <Protocol/EfiWinAcpiUpdate.h>

/*Include QC ACPIPlatform Protocol */
#include <Protocol/EFIAcpiPlatform.h>
#include <Library/QcomBaseLib.h>

/*Include Limits Protocol */
//#include <Protocol/EFILimits.h>
// Above needs to be un-commented after EFILimitsProtocol is available

/* Include ChipInfo Protocol */
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIChipInfoTypes.h>

#include "WinAcpiUpdate.h"


EFI_STATUS
GetFeatureVariable(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL	*This,
IN EFI_FEATURE_TYPE					Type,
OUT  BOOLEAN						*Enable
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN  VarSize = sizeof(gfeatureBitmask);
	Status = gRT->GetVariable(FEATURE_ENABLE_VAR_NAME, &FeatureEnableVarGuid, NULL, &VarSize, &gfeatureBitmask);

	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error GetFeatureVariable, Status = (0x%x) \r\n", Status));
	}
	else {
		// check if feature's bit is set or not, and set Enable to T/F accordingly
		*Enable = (gfeatureBitmask & (0x1 << Type)) ? TRUE : FALSE;
	}

	return Status;
}

EFI_STATUS
SetFeatureVariable(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL	*This,
IN  EFI_FEATURE_TYPE				Type,
IN  BOOLEAN							Enable
)
{
	EFI_STATUS Status = EFI_SUCCESS;

	// update global variable
	if (Enable == TRUE)
	{
		gfeatureBitmask |= (UINT32)(0x1 << Type);
	}
	else
	{
		gfeatureBitmask &= (UINT32)(~(0x1 << Type));
	}

	return Status;
}

EFI_STATUS
CommitFeatureVariable(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL	*This
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN VarSize = sizeof(gfeatureBitmask);
	UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS);

	Status = gRT->SetVariable(FEATURE_ENABLE_VAR_NAME, &FeatureEnableVarGuid, Attributes, VarSize, &gfeatureBitmask);

	return Status;
}

/**
EFI_WIN_ACPI_UPDATE Protocol implementation
*/
EFI_WIN_ACPI_UPDATE_PROTOCOL gEfiWinAcpiUpdateProtocol =
{
	EFI_WIN_ACPI_UPDATE_PROTOCOL_REVISION,
	GetFeatureVariable,
	SetFeatureVariable,
	CommitFeatureVariable
};

static
EFI_STATUS
isSTAHandle(
IN EFI_ACPI_HANDLE AcpiHandle,
OUT BOOLEAN *isSTA
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_ACPI_DATA_TYPE DataType;
	CONST UINT8 *Data;
	UINTN DataSize;

	*isSTA = 0;
	Status = gAcpiSdt->GetOption(AcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
	if (EFI_ERROR(Status)){
		goto Error;
	}
	if ((DataType == EFI_ACPI_DATA_TYPE_OPCODE) && (Data[0] == AML_METHOD_OP)) {
		Status = gAcpiSdt->GetOption(AcpiHandle, 1, &DataType, (CONST VOID **) &Data, &DataSize);
		if (EFI_ERROR(Status)){
			goto Error;
		}
		if (0 == CompareMem("_STA", Data, DataSize)) {
			// we found it!!
			*isSTA = TRUE;
		}
	}
Error:
	return Status;
}

static
VOID
UpdateACPIGlobal(
IN EFI_ACPI_HANDLE AcpiHandle,
IN BOOLEAN Enabled
)
{
	EFI_STATUS Status;
	EFI_ACPI_DATA_TYPE DataType;
	CONST UINT8 *Data;
	UINTN DataSize;
	UINT8 GlobalValue;
	UINTN i = 0;

	if (NULL == AcpiHandle) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Handle is NULL. Exiting.\n"));
		goto Error;
	}

	// first make sure this is a NAME opcode
	Status = gAcpiSdt->GetOption(AcpiHandle, i, &DataType, (CONST VOID **) &Data, &DataSize);
	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (opcode for name operation)\n"));
	}
	else {
		if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_NAME_OP)) {
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Unexpected data found\n"));
			goto Error;
		}
	}

	// go through indices until we find the value of the global var
	while (DataType != EFI_ACPI_DATA_TYPE_CHILD){
		i++;
		Status = gAcpiSdt->GetOption(AcpiHandle, i, &DataType, (CONST VOID **) &Data, &DataSize);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data\n"));
		}
	}

	if (Enabled) {
		GlobalValue = 0x1;
	}
	else {
		GlobalValue = 0x0;
	}

	if (Data[0] != GlobalValue) {
		Status = gAcpiSdt->SetOption(AcpiHandle, i, &GlobalValue, sizeof(GlobalValue));
		if ((EFI_ERROR(Status))) {
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error setting global variable value\n"));
			goto Error;
		}
	}

Error:
	if (AcpiHandle){
		gAcpiSdt->Close(AcpiHandle);
	}
	return;
}

static
VOID
UpdateSTA(
IN EFI_ACPI_HANDLE STAAcpiHandle,
IN BOOLEAN Enabled
)
{
	EFI_ACPI_HANDLE ReturnAcpiHandle = NULL;
	EFI_ACPI_HANDLE ValueAcpiHandle = NULL;
	EFI_ACPI_DATA_TYPE DataType;
	CONST UINT8 *Data;
	UINTN DataSize;
	UINT8 STAValue;
	EFI_STATUS Status;

	if (NULL == STAAcpiHandle) 
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Handle is NULL. Exiting.\n"));
		goto Error;
	}

	Status = gAcpiSdt->GetChild(STAAcpiHandle, &ReturnAcpiHandle);
	if (EFI_ERROR(Status) || (NULL == ReturnAcpiHandle))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No child found\n"));
		goto Error;
	}

	Status = gAcpiSdt->GetOption(ReturnAcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
	if ((EFI_ERROR(Status)))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (opcode for return statement)\n"));
		goto Error;
	}
	if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_RETURN_OP))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Unexpected data found\n"));
		goto Error;
	}

	Status = gAcpiSdt->GetChild(ReturnAcpiHandle, &ValueAcpiHandle);
	if ((EFI_ERROR(Status)) || (NULL == ValueAcpiHandle))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No child found\n\n"));
		goto Error;
	}

	Status = gAcpiSdt->GetOption(ValueAcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
	if ((EFI_ERROR(Status)))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data\n"));
		goto Error;
	}
	if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_BYTE_PREFIX))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Unexpected data found\n"));
		goto Error;
	}

	// get the value of the return statement
	Status = gAcpiSdt->GetOption(ValueAcpiHandle, 1, &DataType, (CONST VOID **) &Data, &DataSize);
	if ((EFI_ERROR(Status)))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (STA return value)\n"));
		goto Error;
	}

	if ((DataType == EFI_ACPI_DATA_TYPE_UINT) && (DataSize == 1)) 
	{
		if (Enabled) 
		{
			STAValue = Data[0];
		}
		else 
		{
			STAValue = 0x0;
		}
		if (Data[0] != STAValue) 
		{
			Status = gAcpiSdt->SetOption(ValueAcpiHandle, 1, &STAValue, sizeof(STAValue));
			if ((EFI_ERROR(Status))){
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error setting STA return value\n"));
				goto Error;
			}
		}
	}
	else 
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Unexpected data found (STA return value)\n"));
		goto Error;
	}

Error:
	if (ValueAcpiHandle)
	{
		gAcpiSdt->Close(ValueAcpiHandle);
	}
	if (ReturnAcpiHandle)
	{
		gAcpiSdt->Close(ReturnAcpiHandle);
	}
}

static
VOID
UpdateACPI(
IN EFI_ACPI_HANDLE AcpiHandle,
IN BOOLEAN Enabled
)
{
	/*
	Check if there is an STA method for this feature:
	If there is one, then use UpdateSTA to patch ACPI
	If there isn't one, then use UpdateACPIGlobal to patch ACPI
	*/
	BOOLEAN isSTA = FALSE;
	EFI_STATUS Status = EFI_SUCCESS;

	Status = isSTAHandle(AcpiHandle, &isSTA);
	if (EFI_ERROR(Status)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding STA method. Exiting.\n"));
		return;
	}
	if (FALSE == isSTA) {
		UpdateACPIGlobal(AcpiHandle, Enabled);
	}
	else{
		UpdateSTA(AcpiHandle, Enabled);
	}
}

// Using device's AcpiHandle provided, returns if device is currently 
// enabled or disabled in ACPI
static
EFI_STATUS
GetCurrentStatus(
IN EFI_ACPI_HANDLE AcpiHandle,
OUT BOOLEAN *Enabled
)
{
	EFI_ACPI_HANDLE ReturnAcpiHandle = NULL;
	EFI_ACPI_HANDLE ValueAcpiHandle = NULL;
	EFI_ACPI_DATA_TYPE DataType;
	CONST UINT8 *Data;
	UINTN DataSize;
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN i = 0;
	BOOLEAN isSTA = FALSE;

	// check if this is a direct handle to _STA
	Status = isSTAHandle(AcpiHandle, &isSTA);
	if (EFI_ERROR(Status)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding STA method. Exiting.\n"));
		goto Exit;
	}
	if (isSTA){
		goto STAExit;
	}
	else {
		// if not, check if this is a global variable
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No STA Method found. Checking if this is set using a global variable...\n"));
		Status = gAcpiSdt->GetOption(AcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
		if (EFI_ERROR(Status)){
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data. Exiting.\n"));
			goto Exit;
		}
		if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_NAME_OP)) {
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No global variable found. Exiting.\n"));
			goto Exit;
		}
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Global variable found!\n"));

		// search for the value of this global variable
		while (DataType != EFI_ACPI_DATA_TYPE_CHILD){
			i++;
			Status = gAcpiSdt->GetOption(AcpiHandle, i, &DataType, (CONST VOID **) &Data, &DataSize);
			if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data\n"));
			}
		}
		*Enabled = (Data[0] != 0x0);
		goto Exit;
	}

STAExit:
	Status = gAcpiSdt->GetChild(AcpiHandle, &ReturnAcpiHandle);
	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No child found\n"));
		goto Exit;
	}

	Status = gAcpiSdt->GetOption(ReturnAcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (opcode for return statement)\n"));
		goto Exit;
	}
	if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_RETURN_OP)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (opcode for return statement) - Unexpected data found\n"));
		goto Exit;
	}

	Status = gAcpiSdt->GetChild(ReturnAcpiHandle, &ValueAcpiHandle);
	if ((EFI_ERROR(Status)) || (NULL == ValueAcpiHandle)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] No child found\n\n"));
		goto Exit;
	}

	Status = gAcpiSdt->GetOption(ValueAcpiHandle, 0, &DataType, (CONST VOID **) &Data, &DataSize);
	if (EFI_ERROR(Status)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (prefix for return value)\n"));
		goto Exit;
	}
	if ((DataType != EFI_ACPI_DATA_TYPE_OPCODE) || (Data[0] != AML_BYTE_PREFIX)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (prefix for return value) - Unexpected data found\n"));
		goto Exit;
	}

	// get the value of the return statement
	Status = gAcpiSdt->GetOption(ValueAcpiHandle, 1, &DataType, (CONST VOID **) &Data, &DataSize);
	if (EFI_ERROR(Status)){
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (STA return value)\n"));
		goto Exit;
	}
	if ((DataType == EFI_ACPI_DATA_TYPE_UINT) && (DataSize == 1)) {
		*Enabled = (Data[0] != 0x0);
	}
	else {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error finding data (STA return value) - Unexpected data found\n"));
		goto Exit;
	}

Exit:
	if (ValueAcpiHandle){
		gAcpiSdt->Close(ValueAcpiHandle);
	}
	if (ReturnAcpiHandle){
		gAcpiSdt->Close(ReturnAcpiHandle);
	}

	return Status;
}

// Reads appropriate ACPI node (like _STA or as coded) for each FEATURE/DEVICE and based on value read
// updates the corresponding feature's bit position in gfeatureBitmask (1 for enabled, 0 for disabled)
EFI_STATUS
InitFeatureVariable(
IN EFI_ACPI_HANDLE					AcpiHandle
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_ACPI_HANDLE OutHandle = NULL;
	UINTN  VarSize = sizeof(gfeatureBitmask);
	UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS);
	BOOLEAN enabled = FALSE;

	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Creating UEFI variable... \r\n"));

	// loop through all features, check in ACPI if it is enabled/disabled by default, and construct global variable
	for (int i = 0; i < EFI_FEATURE_MAX; i++)
	{
		Status = gAcpiSdt->FindPath(AcpiHandle, (UINT8*)DEVICE_LIST[i], &OutHandle);
		if (EFI_ERROR(Status))
		{
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error gAcpiSdt->FindPath, Status = (0x%x) \r\n", Status));
			goto Error;
		}

		// check ACPI for status of feature (enabled/disabled)
		Status = GetCurrentStatus(OutHandle, &enabled);
		if (EFI_ERROR(Status)) {
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error getting current feature config, GetCurrentStatus Status = (0x%x) \r\n", Status));
			goto Error;
		}

		// update global variable
		if (enabled == TRUE)
		{
			gfeatureBitmask |= (UINT32)(0x1 << i);
		}
		else
		{
			gfeatureBitmask &= (UINT32)(~(0x1 << i));
		}
	}

	// create the uefi variable
	Status = gRT->SetVariable(FEATURE_ENABLE_VAR_NAME, &FeatureEnableVarGuid, Attributes, VarSize, &gfeatureBitmask);

	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error gRT->SetVariable, Status = (0x%x) \r\n", Status));
	}

Error:
	if (OutHandle){
		gAcpiSdt->Close(OutHandle);
	}
	return Status;
}

static 
EFI_STATUS
UpdateDsdtTable(
  IN EFI_ACPI_SDT_HEADER *Table,  
  IN EFI_ACPI_TABLE_VERSION Version,  
  IN UINTN TableKey )
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_ACPI_HANDLE AcpiHandle = NULL;
	EFI_ACPI_HANDLE OutHandle = NULL;
	UINTN  VarSize = sizeof(gfeatureBitmask);
	UINT32 enable = 0x1; //used to toggle feature if it's defective

	// open handle with top node of DSDT table
	Status = gAcpiSdt->Open((VOID*)(UINTN)Table+sizeof(EFI_ACPI_SDT_HEADER),&AcpiHandle);
	if (EFI_ERROR (Status))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: Error opening the handle for DSDT tables \r\n"));
		goto Error;
	}

	// get uefi variable for bitmask of features
	Status = gRT->GetVariable(FEATURE_ENABLE_VAR_NAME, &FeatureEnableVarGuid, NULL, &VarSize, &gfeatureBitmask);
	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: Error gRT->GetVariable, Status = (0x%x) \r\n", Status));

		// if this failed because the variable doesn't exist, we should create it here
		if (Status == EFI_NOT_FOUND){
			Status = InitFeatureVariable(AcpiHandle);
			if (EFI_ERROR(Status)){
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: Error InitFeatureVariable, Status = (0x%x) \r\n", Status));
				goto Error;
			}
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: InitFeatureVariable: Created uefi variable with value (0x%x) \r\n", gfeatureBitmask));
		}
		
		// if this failed for some other reason, check if there are any defective parts
		if(defectivePartsBitMask != 0x0) //There are some defective parts, so still need to disable these 
		{
			for (int i = 0; i < EFI_FEATURE_MAX; i++)
			{
				if((defectivePartsBitMask >> i) & 0x1)
				{
					DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: Disabling defective part (0x%x) \r\n", i));
					Status = gAcpiSdt->FindPath(AcpiHandle,(UINT8*)DEVICE_LIST[i],&OutHandle);
					if(EFI_ERROR(Status))
					{
						DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error gAcpiSdt->FindPath, Status = (0x%x) \r\n", Status));
					}
					else
					{
						UpdateACPI(OutHandle, FALSE);
					}
				}
			}
		}
		goto Error;
	}
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] gRT->GetVariable: Got uefi variable with value (0x%x) \r\n", gfeatureBitmask));

	// loop through features and apply config based on gfeatureBitmask value
	for (int i = 0; i < EFI_FEATURE_MAX; i++)
	{
		Status = gAcpiSdt->FindPath(AcpiHandle,(UINT8*)DEVICE_LIST[i],&OutHandle);
		if(EFI_ERROR(Status))
		{
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Error gAcpiSdt->FindPath, Status = (0x%x) \r\n", Status));
		}
		else
		{
			enable = 0x1; //assume part is non-defective
			if((defectivePartsBitMask >> i) & 0x1) //if part is defective
			{
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateDsdtTable: Defective part (0x%x) will be disabled\r\n", i));
				enable = 0x0;
			}
			UpdateACPI(OutHandle, ((gfeatureBitmask >> i) & enable));
		}
	}

Error:
	if (AcpiHandle){
		gAcpiSdt->Close(AcpiHandle);
	}
	if (OutHandle){
		gAcpiSdt->Close(OutHandle);
	}
	return Status;
}

static 
EFI_STATUS
EFIAPI
AcpiNotification (  
	IN EFI_ACPI_SDT_HEADER *Table,  
	IN EFI_ACPI_TABLE_VERSION Version,  
	IN UINTN TableKey)
{
	EFI_STATUS Status = EFI_SUCCESS;

	if (Table->Signature == EFI_ACPI_6_1_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE){
		Status = UpdateDsdtTable(Table,Version,TableKey);
	}
	
	return Status;
}

UINT32 GetDefaultDefectivesMask(VOID)
{
	/*Return all Gold Disabled by default as silver CPUs works fine on all Bin Parts*/
	//return (0xF0);
	return (0x00); //For now not disabling any CPUs by default
}

// Needs re-visit for future chipsets. This should be obtained from ChipInfo instead of hard-coding as below
UINT32  GetTotalNumCPU(VOID)
{
	return (NUM_CPU);
}

static VOID DisableCPUs(ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE *Blob, UINT8 CPUMask)
{
	UINT32 Mask=0x1, num=0x0;
	UINT32 i=0;
	num=GetTotalNumCPU();
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]DisableCPUs: CPUMask (0x%x) \r\n", CPUMask));
	for(i=0;i<num;i++)
	{
		if(CPUMask&(Mask<<i))
		{
			if(Blob->LocalGic[i].Flags)
			{
				Blob->LocalGic[i].Flags = 0x0;
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]DisableCPUs: Disabling Processor 0x%x Blob->LocalGic[i].Flags =0x%x \r\n", 
					                                                Blob->LocalGic[i].AcpiProcessorId, 
					                                                Blob->LocalGic[i].Flags));
			}
		}
	}
}

static VOID PrintBlobInfo(ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE *Blob)
{
	UINT8 i=0;	
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]PrintBlobInfo: Printing Only Flags Information from Blob \r\n"));
	for (i = 0; i<NUM_CPU;i++)
	{
		DEBUG((EFI_D_ERROR, "LocalGic[%d]: Flag=0x%x, ProcessorID:0x%x \r\n", 
															i,
															Blob->LocalGic[i].Flags,
															Blob->LocalGic[i].AcpiProcessorId));			
	}
}

/**
	AcpiMADTCallback() - Implements MADT patching.
*/
static EFI_STATUS EFIAPI AcpiMADTCallback(
  IN OUT VOID         **AcpiTableBlob,
  IN OUT UINTN         *AcpiTableSize
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE *Madt= NULL;
    UINT8 CPUDisableMask = 0;

	if(AcpiTableBlob == NULL || *AcpiTableBlob== NULL  || AcpiTableSize == NULL)
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: MADT table was not found by ACPIPlatformDxe \r\n"));
		Status = EFI_INVALID_PARAMETER;
		goto Exit;
	}	
	else
	{
		Madt=(ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE*)*AcpiTableBlob;
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: MADT Address (0x%p), Size (0x%08x) \r\n", Madt,*AcpiTableSize));
	}

	CPUDisableMask = (UINT8) GetDefaultDefectivesMask(); // this will be used as fall back mask 
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: Start with Default CPUDisableMask (0x%x) \r\n", CPUDisableMask));

	//Limits Throttle
	/* Needs to be un-commented when EFILimitsProtocol is available */
/*	EFI_LIMITS_PROTOCOL	*pEfiLimitsProtocol = NULL;
	UINT32 APC1Throttle = 0, ACP1ThrottleValue= 0 ;
	// Locate the Limits protocol
    Status = gBS->LocateProtocol(&gEfiLimitsProtocolGuid, NULL, (VOID**) &pEfiLimitsProtocol);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: Locate Limits Protocol failed, Status = (0x%x)\r\n", Status));
      goto Exit;	
    }
	else
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: Locate Limits Protocol succeeded, Status = (0x%x)\r\n", Status));
	}
    Status = pEfiLimitsProtocol->SubSysThrottle(pEfiLimitsProtocol,
  												 EFI_LIMITS_SUBSYS_APC1,
  												 &APC1Throttle,
  												 &ACP1ThrottleValue);
	if (EFI_ERROR(Status))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] AcpiMADTCallback: Limits Protocol - disabling Gold CPUs by default failed, Status = (0x%x)\r\n", Status));
		goto Exit;
	}
	else
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: disabling Gold CPUs by default succeeded, Status = (0x%x)\r\n", Status));
	}
	
	if(APC1Throttle==EFI_LIMITS_THROTTLE_MAX_DISABLE)
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: Leakage Value for this Part is more than 10x - Leaving Gold CPUs disabled\r\n"));
		goto Exit;	
	}
*/	

	//Call ChipInfo Protocol GetDefectiveCPUs 
	/* Needs re-visit for future chipsets. Even though Napali (845) actually has 2 clusters- GOLD and SILVER with 4 CPUs or cores for each cluster, 
	as per ChipInfo module, it's treated as having one cluster of 8 cores. This inconsistency is due to definition of cluster being used, which
	is driven by the values that come out of the MPIDR register. Also on Napali, ChipInfo will return 0xF0 if any of the gold cores is defective
	and binning of silver cores isn't required/ supported. So effectively, for Napali, we expect to see for this call only 2 values: 
	0xF0 (if any gold cores are defective) and 0x00 (if none of the gold cores are defective). */
    Status = pChipInfoProtocol->GetDefectiveCPUs(pChipInfoProtocol, cluster_num, &defectiveCPUsBitMask);    
	if (EFI_ERROR(Status))
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: GetDefectiveCPUs call failed, Status = (0x%x)\r\n", Status));
		CPUDisableMask = (UINT8) GetDefaultDefectivesMask(); // this will be used as fall back mask 
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate] Restored Default CPUDisableMask (0x%x) \r\n", CPUDisableMask));
	}
	else
	{
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: GetDefectiveCPUs returned defectiveCPUsBitMask (0x%x) \r\n", defectiveCPUsBitMask));
		CPUDisableMask = (UINT8) defectiveCPUsBitMask;
		DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: CPUDisableMask (0x%x) after updating with defective CPUs \r\n", CPUDisableMask));
	}
	//CPUDisableMask = 96; //0x60 or 01100000, which is test value to disable 2 of the gold cores (5 and 6) out of 8 total cores
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: Calling DisableCPUs with CPUDisableMask (0x%x)\r\n", CPUDisableMask));

	Exit:
	DisableCPUs(Madt, CPUDisableMask);	
	PrintBlobInfo(Madt);
	
	return Status;
}
/* From CPUPartialGoods.c END */

static VOID process_defective_part( IN  EFIChipInfoPartType defectivePart)
{
	switch(defectivePart)
		{
			case EFICHIPINFO_PART_GPU:
	  		case EFICHIPINFO_PART_VIDEO:
  			case EFICHIPINFO_PART_DISPLAY:
				defectivePartsBitMask |= (UINT32)(0x1 << EFI_FEATURE_GPU);
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: Marking EFI_FEATURE_GPU for disabling. Updated defectivePartsBitMask to (0x%x) \r\n", defectivePartsBitMask));
				break;
	  		case EFICHIPINFO_PART_CAMERA:
				defectivePartsBitMask |= (UINT32)(0x1 << EFI_FEATURE_FRONT_CAMERA);
				defectivePartsBitMask |= (UINT32)(0x1 << EFI_FEATURE_BACK_CAMERA);  //Verify with ChipInfo if EFICHIPINFO_PART_CAMERA means both cameras
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: Marking EFI_FEATURE_FRONT_CAMERA and EFI_FEATURE_BACK_CAMERA for disabling. Updated defectivePartsBitMask to (0x%x) \r\n", defectivePartsBitMask));
				break;
	  		case EFICHIPINFO_PART_AUDIO:
				defectivePartsBitMask |= (UINT32)(0x1 << EFI_FEATURE_AUDIO);
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]AcpiMADTCallback: Marking EFI_FEATURE_AUDIO for disabling. Updated defectivePartsBitMask to (0x%x) \r\n", defectivePartsBitMask));
				break;
			default:
				break;
		}
	
	return;
}


static
EFI_STATUS
GetDefectiveParts(VOID)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 nMask = 0x0;
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]GetDefectiveParts: Creating UEFI variable... \r\n"));

	// loop through all features, check in ACPI if it is enabled/disabled by default, and construct global variable
	for (int i = 0; i < EFICHIPINFO_NUM_PARTS; i++)
	{
		Status = pChipInfoProtocol->GetDefectivePart(pChipInfoProtocol, i, &nMask);
		if (EFI_ERROR(Status))
		{
			DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]GetDefectiveParts: GetDefectivePart failed, Status = (0x%x), default to Non-Defective Part (0x%x) \r\n", Status, i));
		}
		else
		{      
			// nMask is non-zero if the part is defective. "non-zero" currently means 1, but that might change in the future      
			// if more granularity is needed to determine defectiveness      
			if (nMask != 0)      
			{        
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]GetDefectiveParts: GetDefectivePart got Defective Part (0x%x) \r\n", i));
				process_defective_part((EFIChipInfoPartType)i);  //updates defectivePartsBitMask for defective part
			}
			else
			{   
				DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]GetDefectiveParts: GetDefectivePart got Non-Defective Part (0x%x) \r\n", i));
			}    
		}
	}
	return Status;
}


EFI_STATUS
EFIAPI
UpdateACPIEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
   EFI_STATUS Status = EFI_SUCCESS;   
   
   // Locate ACPI SDT Protocol and register for callback for loading of ACPI tables 
   Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID**) &gAcpiSdt);
   if (EFI_ERROR (Status)) {
	DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Failed to Locate SDT protocol \r\n"));
	goto Error;
   }

   Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle,
	   &gEfiWinAcpiUpdateProtocolGuid,
	   (void *)&gEfiWinAcpiUpdateProtocol,
	   NULL);

   if (EFI_ERROR(Status)) {
	   DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Failed to Install EfiWinAcpiUpdate protocol \r\n"));
	   goto Error;
   }

    // Register callback for when any ACPI table is loaded- used only for DSDT table patching
  	gAcpiSdt->RegisterNotify(TRUE,AcpiNotification);

   
   // Locate ACPI Platform Protocol and Register for Callback when MADT Table is loaded 
   Status = gBS->LocateProtocol(&gQcomAcpiPlatformProtocolGuid, NULL, (VOID**) &pEfiAcpiPlatformProtocol);
   if (EFI_ERROR(Status))
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Locate ACPI Protocol failed, Status = (0x%x)\r\n", Status));
	 goto Error; 
   }
   else
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Locate ACPI Protocol succeeded, Status = (0x%x)\r\n", Status));
   }
   
   Status = pEfiAcpiPlatformProtocol->AcpiTableRegister(
			 pEfiAcpiPlatformProtocol, EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
			 GETMODE_PREFIX, NULL, AcpiMADTCallback);
   
   if (EFI_ERROR(Status))
   {
	 DEBUG((EFI_D_WARN, "[WinAcpiUpdate]UpdateACPIEntry: ACPI MADT Callback Register failed, Status = (0x%x)\r\n", Status));
	 goto Error;
   }	 
   else
   {
	 DEBUG((EFI_D_WARN, "[WinAcpiUpdate]UpdateACPIEntry: ACPI MADT Callback Register succeeded, Status = (0x%x)\r\n", Status));
   }

   // Locate ChipInfo Protocol to get Defective Parts and CPUs
   Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid, NULL, (VOID **) &pChipInfoProtocol);
   if (EFI_ERROR(Status))
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Locate ChipInfo Protocol failed, Status = (0x%x)\r\n", Status));
	 goto Error; 
   }
   else
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Locate ChipInfo Protocol succeeded, Status = (0x%x)\r\n", Status));
   }
   
   // Get defective parts
   Status = GetDefectiveParts();
   if (EFI_ERROR(Status))
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: GetDefectiveParts failed, Status = (0x%x)\r\n", Status));
	 defectivePartsBitMask = GetDefaultDefectivesMask();
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: Using Default defectivePartsBitMask = (0x%x)\r\n", defectivePartsBitMask));
	 goto Error; 
   }
   else
   {
	 DEBUG((EFI_D_ERROR, "[WinAcpiUpdate]UpdateACPIEntry: GetDefectiveParts succeeded returning defectivePartsBitMask = (0x%x)\r\n", defectivePartsBitMask));
   }

Error:
  	return Status;
  
}
