
/** @file
  Qualcomm ACPI Platform Driver call back function for chipinfo

  Copyright (c) 2014-2015, Qualcomm Technologies Inc. All rights
  reserved. 
**/ 

/*=============================================================================
                              EDIT HISTORY
 when           who     what, where, why
 --------   ---     -----------------------------------------------------------
07-09-15    wayne   Added for Chip Serial number callback
03-27-15    zzl       fix modem feature checking issue
03-20-15    zzl       fix modem feature checking issue
10-08-14    zzl       Created for Acpi chip info callback
=============================================================================*/
#include <AcpiPlatform.h>
#include "AcpiPlatformChipInfo.h"

EFI_STATUS EFIAPI AmlUpdateChipId (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoIdType chipID = EFICHIPINFO_NUM_IDS;
    EFI_STATUS Status;

    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML: ChipID Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(EFIChipInfoIdType))
    {
        DEBUG((EFI_D_WARN,"AML: ChipID input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }
    
    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));

    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }
    
    Status = pEfiChipInfoProtocol->GetChipId(pEfiChipInfoProtocol,&chipID);    
    if((chipID < EFICHIPINFO_NUM_IDS)&& (Status==EFI_SUCCESS))
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," chipID: %x\r\n", chipID ));      
      *((UINT32 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT32)chipID;
    }
    else
    {
      DEBUG((EFI_D_WARN,"GetChipId failed\r\n" ));  
    }     
    return Status;
    
}

EFI_STATUS EFIAPI AmlUpdateChipChipIdString (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
  EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
  UINTN i;
  CHAR8 ChipIdString[EFICHIPINFO_MAX_ID_LENGTH];
  EFI_STATUS Status;
  DEBUG((DEBUG_LOAD,"AML: Update Chip and Speed Bin values\r\n"));
  if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
  {
    DEBUG((EFI_D_WARN,"AML: Chip Info Buffer is empty\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  else
  {
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"  Chip info buffer:\r\n"));
  }
  
  //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
  if(AmlVariableBufferSize<EFICHIPINFO_MAX_ID_LENGTH)
  {
      DEBUG((EFI_D_WARN,"AML: ChipIdString input Buffer is too small\r\n"));
      return EFI_BUFFER_TOO_SMALL;
  }
  
  for(i=0; i<AmlVariableBufferSize; i++)
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));

  // locate chip info protocol
  Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
  if (EFI_ERROR(Status))
  {
      DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
      return Status;
  }
  
  Status = pEfiChipInfoProtocol->GetChipIdString( pEfiChipInfoProtocol, ChipIdString, EFICHIPINFO_MAX_ID_LENGTH);
  
  if((ChipIdString!=NULL)&&(Status == EFI_SUCCESS))
  {
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT," ChipIdString: %a\r\n", ChipIdString ));
    AsciiStrnCpy( ((CHAR8 *) *AmlVariableBuffer)+AML_NAMESPACE_HEADER_SIZE, ChipIdString, EFICHIPINFO_MAX_ID_LENGTH);
  }
  else
  {
    DEBUG((EFI_D_WARN,"GetChipIdString failed\r\n" ));  
  } 
  
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"  New chip info buffer:\r\n"));
  for(i=0; i<AmlVariableBufferSize; i++)
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
  return EFI_SUCCESS;
}



EFI_STATUS EFIAPI AmlUpdateChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoVersionType ChipVersion;
    EFI_STATUS Status;
    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML: ChipID Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }

    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
    
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(EFIChipInfoVersionType))
    {
        DEBUG((EFI_D_WARN,"AML: ChipVersion input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }
    

    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }

    Status = pEfiChipInfoProtocol->GetChipVersion(pEfiChipInfoProtocol,&ChipVersion);    
    if(Status ==  EFI_SUCCESS)
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," chipID: %x\r\n", ChipVersion ));      
      *((UINT32 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) =(UINT32) ChipVersion;
    }
    else{
      DEBUG((EFI_D_WARN," chipID: protocol failed\r\n" ));  
    }    
    
    return Status;
}


EFI_STATUS EFIAPI AmlUpdateMajorChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoVersionType ChipVersion;
    EFI_STATUS Status;
    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML: Major sChipID Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }

    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
    
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(UINT16))
    {
        DEBUG((EFI_D_WARN,"AML: MajorChip input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }

    
    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }

    Status = pEfiChipInfoProtocol->GetChipVersion(pEfiChipInfoProtocol,&ChipVersion);    
    if(Status ==  EFI_SUCCESS)
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," Major ChipVersion: %lx\r\n", ChipVersion ));      
      *((UINT16 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT16)(ChipVersion>>16);    
    }
    else{
      DEBUG((EFI_D_WARN," Major ChipVersion: protocol failed\r\n" ));  
    }
    return Status;
}

EFI_STATUS EFIAPI AmlUpdateMinorChipVersion (
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoVersionType ChipVersion;
    EFI_STATUS Status;
    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML:Minor ChipID Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }

    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
    
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(UINT16))
    {
        DEBUG((EFI_D_WARN,"AML: MinorChip input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }

    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }

    Status = pEfiChipInfoProtocol->GetChipVersion(pEfiChipInfoProtocol,&ChipVersion);    
    if(Status ==  EFI_SUCCESS)
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," Minor ChipVersion: %x\r\n", ChipVersion ));      
      *((UINT16 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT16)(ChipVersion&0xffff);
    }
    else{
      DEBUG((EFI_D_WARN," Minor ChipVersion: protocol failed\r\n" ));  
    }
    return Status;
}

EFI_STATUS EFIAPI AmlUpdateChipFamily(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoFamilyType ChipFamily;
    EFI_STATUS Status;

    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML: ChipFamily Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }

    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
    
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(UINT16))
    {
        DEBUG((EFI_D_WARN,"AML: ChipFamily input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }
    
    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }

    Status = pEfiChipInfoProtocol->GetChipFamily(pEfiChipInfoProtocol,&ChipFamily);    
    if((ChipFamily < EFICHIPINFO_NUM_FAMILIES)&&(Status == EFI_SUCCESS))
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," ChipFamily: %x\r\n", ChipFamily ));      
      *((UINT16 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT16)ChipFamily;
    }
    else{
        DEBUG((EFI_D_WARN," ChipFamily: protocol failed\r\n" ));  
    }
    return Status;
}


EFI_STATUS EFIAPI AmlUpdateModemInfo(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoModemType ModemInfo;
    EFI_STATUS Status;

    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((EFI_D_WARN,"AML: ModemInfo Buffer is empty\r\n"));
      return EFI_INVALID_PARAMETER;
    }

    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
    
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(EFIChipInfoModemType))
    {
        DEBUG((EFI_D_WARN,"AML: ModemInfo input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }

    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }
    Status = pEfiChipInfoProtocol->GetModemSupport(pEfiChipInfoProtocol,&ModemInfo);    
    if(Status == EFI_SUCCESS)
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," ModemInfo: %x\r\n", ModemInfo ));      
      *((UINT16 *) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT16 ) ModemInfo;
    }
    else
    {
        DEBUG((EFI_D_WARN,"GetModemSupport failed\r\n" ));  
        return Status;
    }    
    return Status;
}


EFI_STATUS EFIAPI AmlUpdateChipSerialNum(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
    EFI_CHIPINFO_PROTOCOL *pEfiChipInfoProtocol = NULL;
    UINTN i;
    EFIChipInfoSerialNumType chipSerialNum = 0;
    EFIChipInfoQFPROMChipIdType chipIDinfo = 0;
    EFI_STATUS Status;

    if((AmlVariableBuffer == NULL)||(*AmlVariableBuffer == NULL))
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"AML: Chip Serial num Buffer is empty\r\n"));
      return EFI_SUCCESS;
    }
    //check aml variablebuffersize should be equal or bigger than the type of varaible we  suppose it to be . 
    if(AmlVariableBufferSize<sizeof(EFIChipInfoSerialNumType))
    {
        DEBUG((EFI_D_WARN,"AML:  Chip Serial num  input Buffer is too small\r\n"));
        return EFI_BUFFER_TOO_SMALL;
    }
    
    for(i=0; i<AmlVariableBufferSize; i++)
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));

    // locate chip info protocol
    Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL, (VOID**) &pEfiChipInfoProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN," gEfiChipInfoProtocolGuid protocol failed\r\n" ));  
        return Status;
    }
    
    Status = pEfiChipInfoProtocol->GetSerialNumber(pEfiChipInfoProtocol,&chipSerialNum);    
    if(Status==EFI_SUCCESS)
    {   
      *((UINT32*) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE)) = (UINT32)chipSerialNum;
    }
    else
    {
      DEBUG((EFI_D_WARN,"Get  Chip Serial num  failed\r\n" ));  
      return Status;
    }     

    Status = pEfiChipInfoProtocol->GetQFPROMChipId(pEfiChipInfoProtocol,&chipIDinfo);    
    if(Status==EFI_SUCCESS)
    {   
        *((UINT32*) ((CHAR8 *)(*AmlVariableBuffer) + AML_NAMESPACE_HEADER_SIZE+sizeof(UINT32))) = (UINT32)chipIDinfo;
    }
    else
    {
      DEBUG((EFI_D_WARN,"Get  Chip Serial num : id info failed\r\n" ));  
    }     

    
    return Status;
    
}
