/** @file DelSecBootVars.c
   

  Copyright (c) 2017 Qualcomm Technologies Inc. All rights reserved.
  Portions Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved. 
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/21/17   bd      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/ScmCmdLib.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFIVariableServices.h>
#include <Drivers/VariableDxe/ScmCmdVar.h>
#include <Library/MemoryAllocationLib.h>

#define SERVICE_UEFI_VAR_CREATE_CMD(x) (TZEXEC_SVC_UEFI_VAR_ID | x)
#define SERVICE_UEFI_VAR_DEL_SECBOOT_VARS                   \
        SERVICE_UEFI_VAR_CREATE_CMD(13)
        
#define RETURN_UINTN_EFI_STATUS(a)                       \
        (((a) == (0)) ? (a) : (ENCODE_ERROR((a)&0xFF)))


STATIC SCM_CMD_PARAMS_T ScmCmdInitParams = {NULL, 0, 0, NULL};

QCOM_SCM_PROTOCOL *pQcomScmProtocol;
EFI_VARIABLESERVICES_PROTOCOL *VariableServicesProtocol;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length; 

  
} TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS;

/**
  This function query for APP ID 

  
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

UINT32 QueryUefiSecAppId()
{
  EFI_STATUS    Status;
  UINT32        AppId = 0;
  UINT8  AppName[] = "qcom.tz.uefisecapp";

  if(pQcomScmProtocol == NULL )
  {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  Status = pQcomScmProtocol->ScmSendCommand(
                                             pQcomScmProtocol, 
                                             APP_QUERY_APPID_CMD, 
                                            &AppId,
                                             AppName,
                                             sizeof(AppName),
                                             NULL,
                                             0
                                            );
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint ("QueryTpmAppId failed... \n");
  }

Exit:
  return AppId;
}


/**
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI 
test_scm_command(void)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32        AppId = 0;
  
  TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS *Request = (TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS *) ScmCmdInitParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;

  
  AsciiPrint ("test_scm_command start\n");
  Request->CommandId = SERVICE_UEFI_VAR_DEL_SECBOOT_VARS;
  
  Request->Length = sizeof(TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS);
  RequestLength = Request->Length;

  
  ResponseOffset = ALIGN_VALUE(Request->Length, 32);
  RequestLength = ResponseOffset;
  Response = (TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS *)((UINT8*)Request + ResponseOffset);
  ResponseLength = sizeof(TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS);
  
  //get appid first
  AppId = QueryUefiSecAppId();
  
  // Validate the size of the request
  if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  // Validate the size of response + request 
  if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  if(NULL  == pQcomScmProtocol)
  {
    AsciiPrint("pQcomScmProtocol is NULL \r\n");
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  
  Status = pQcomScmProtocol->ScmSendCommand(pQcomScmProtocol,
                                           APP_SEND_DATA_CMD,
                                           &AppId,
                                           Request,
                                           RequestLength,
                                           Response,
                                           ResponseLength);
  if (EFI_ERROR (Status)) 
  {
     DEBUG(( EFI_D_ERROR, "ScmSendCommand() failed, Status = (0x%x)\r\n", Status));
     goto Exit;
  }

  /* Copy output parameters */
  Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  if (Status == EFI_UNSUPPORTED) 
  {
    Status = EFI_SUCCESS;
    DEBUG(( EFI_D_ERROR, "** Failure Expected here,Test Case PASS **\n"));
    goto Exit;
  }
  else if (Status == EFI_NOT_FOUND) 
  {
    Status = EFI_NOT_FOUND;
    DEBUG(( EFI_D_ERROR, "** SecBoot variables not found,Test Case INCONCLUSIVE **\n"));
    goto Exit;
  }
  else if (Status == EFI_SUCCESS) 
  {
    DEBUG(( EFI_D_ERROR, "** SUCCESS not expected here,Test Case FAIL **\n"));
  }
  
  Status = EFI_DEVICE_ERROR ;
  Exit:
  return Status;
}

/**
    
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI 
test_var_services(void)
{
  EFI_STATUS  Status= EFI_SUCCESS;  
  
  Status = gBS->LocateProtocol(&gEfiVariableServicesProtocolGuid, NULL, (VOID **)&VariableServicesProtocol);
  if( Status != EFI_SUCCESS )
  {
    AsciiPrint ("LocateProtocol:VariableServicesProtocol failed... \n");
    goto Exit;
  }

  Status = VariableServicesProtocol->DelSecBootVars(VariableServicesProtocol);
  if (Status == EFI_UNSUPPORTED) 
  {
    Status = EFI_SUCCESS;
    DEBUG(( EFI_D_ERROR, "** Failure Expected here,Test Case PASS **\n"));
    goto Exit;
  }
  else if (Status == EFI_NOT_FOUND) 
  {
    Status = EFI_NOT_FOUND;
    DEBUG(( EFI_D_ERROR, "** SecBoot variables not found,Test Case INCONCLUSIVE **\n"));
    goto Exit;
  }
  else if (Status == EFI_SUCCESS) 
  {
    DEBUG(( EFI_D_ERROR, "** SUCCESS not expected here,Test Case FAIL **\n"));
  }
  
  Status = EFI_DEVICE_ERROR ;
  Exit:
  return Status;
  
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
DelSecBootVarsMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  
  EFI_STATUS  Status= EFI_SUCCESS;  
  
  TEST_START("DelSecBootVars");
  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&pQcomScmProtocol);
  if( Status != EFI_SUCCESS )
  {
    AsciiPrint ("LocateProtocol failed... \n");
    goto Exit;
  }
  
  ScmCmdInitParams.ScmCmdBufferSize = 0x20000; /* 128KB */
  UINTN ScmCmdBufferPages = ScmCmdInitParams.ScmCmdBufferSize/EFI_PAGE_SIZE;
  ScmCmdInitParams.ScmCmdBuffer = AllocateAlignedPages(ScmCmdBufferPages, EFI_PAGE_SIZE);

  AsciiPrint ("\n --------------------------- \n");
  
  Status = test_scm_command();
  AsciiPrint ("Test SCM COMMAND Status = %r \n", Status);

  AsciiPrint ("\n --------------------------- \n");
  
  Status = test_var_services();
  AsciiPrint ("Test Variable Services status = %r \n", Status);  
  
Exit:
  TestStatus("DelSecBootVars", Status);
  TEST_STOP("DelSecBootVars");
  return Status;
}
