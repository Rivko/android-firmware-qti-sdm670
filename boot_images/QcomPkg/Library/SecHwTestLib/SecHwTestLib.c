/*                                                                    
                    SecHwTestLib.c   

  This library implements the Security Hardware Testability as per Microsoft specifications.

  Copyright (c) 2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

/*=============================================================================

                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/13/15   sk       Added Prodmode HOB
 07/07/15   sk       Removed rollback check
 06/03/15   sk       Removed uefi-secureboot check
 03/31/15   sk       Removed test calls within library
 03/04/15   sk       Moved definitions to header
 01/08/15   sk       Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIAdapterInformationRegister.h>
#include <Library/QcomLib.h>
#include <Library/SecHwTestLib.h>
#include <Library/FuseControlLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiLib.h>
#include <Protocol/EFIAdapterInformation.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Library/QcomTargetLib.h>
#include <Library/HobLib.h>
#include <PiDxe.h>


EFI_STATUS GetPlatformSecurityInfo 
(
  OUT VOID    **InformationBlock,
  OUT UINTN    *InformationBlockSize
)
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  PADAPTER_INFO_PLATFORM_SECURITY   InfoBlock = NULL;
  CHAR8                             StrProduct[64];
  CHAR8                             FullProduct[64];
  CHAR8                            *PlatformTypePtr, *ChipNamePtr, *ChipVerPtr;
  UINTN                             VarSize;
  BOOLEAN                           OverallSecurityCheckFlag = TRUE;     // Assume secure device
  CHAR8                             LocalImplementationID[256];
  CHAR8                             LocalErrorString[512];

  // Flags for various checks
  UINT8       SetupMode;
  BOOLEAN     rpmbcheckflag;
  BOOLEAN     shkcheckflag;
  BOOLEAN     rollbackcheckflag;
  BOOLEAN     dummykeycheckflag;
  BOOLEAN     proddevicecheckflag;
  BOOLEAN     prodmodecheckflag;
  BOOLEAN     debugpolicyflag;

  DEBUG(( EFI_D_INFO,"GetPlatformSecurityInfo() is called. \r\n"));

  if( InformationBlock == NULL || InformationBlockSize == NULL )
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  *InformationBlockSize = sizeof(ADAPTER_INFO_PLATFORM_SECURITY);
  *InformationBlock = AllocatePool(sizeof(ADAPTER_INFO_PLATFORM_SECURITY));
  if( *InformationBlock == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  SetMem(*InformationBlock, sizeof(ADAPTER_INFO_PLATFORM_SECURITY), 0x0);

  InfoBlock = (PADAPTER_INFO_PLATFORM_SECURITY) *InformationBlock;


  /* ======================== Fill InfoBlock->Version ====================================== */

  InfoBlock->Version = PLATFORM_SECURITY_VERSION_VNEXTCS;


  /* ======================== Fill InfoBlock->Role ========================================= */

  InfoBlock->Role = PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE;


  /* ======================== Fill InfoBlock->ImplementationID ============================= */

  Status = GetPlatformStrings(StrProduct, sizeof(StrProduct),
                      &PlatformTypePtr, &ChipNamePtr, &ChipVerPtr);

  SetMem(FullProduct,sizeof(FullProduct),0x0);
  AsciiStrCpy(FullProduct, "QUALCOMM:");
  AsciiStrCat(FullProduct, ChipNamePtr);
  AsciiStrCat(FullProduct, ":");
  AsciiStrCat(FullProduct, ChipVerPtr);
  AsciiStrCat(FullProduct, ":");
  AsciiStrCat(FullProduct, PlatformTypePtr);
  //AsciiStrCpy((CHAR8*)InfoBlock->ImplementationID, FullProduct);
  AsciiStrCpy(LocalImplementationID, FullProduct);
  AsciiStrToUnicodeStr(LocalImplementationID,InfoBlock->ImplementationID);      // Convert to CHAR16


  /* ======================== Fill InfoBlock->SecurityFeaturesSize ========================= */
  
  // 6 checks in total
  InfoBlock->SecurityFeaturesSize = 2;  // 1 byte is enough to hold the bit-masks


  /* ======================== Fill InfoBlock->SecurityFeaturesRequired ===================== */
  
  // Assume all 6 checks are required
  InfoBlock->SecurityFeaturesRequired[0] = FLAG_SECURE_BOOT | FLAG_RPMB_CHECK | FLAG_SHK_CHECK | FLAG_ROLLBACK_CHECK | FLAG_DUMMY_KEY_CHECK | FLAG_PRODUCTION_DEVICE_CHECK;
  InfoBlock->SecurityFeaturesRequired[1] = 0x0;


  /* ======================== Fill InfoBlock->SecurityFeaturesImplemented ================== */
  
  // Assume all 6 checks are implemented
  InfoBlock->SecurityFeaturesImplemented[0] = FLAG_SECURE_BOOT | FLAG_RPMB_CHECK | FLAG_SHK_CHECK | FLAG_ROLLBACK_CHECK | FLAG_DUMMY_KEY_CHECK | FLAG_PRODUCTION_DEVICE_CHECK;
  InfoBlock->SecurityFeaturesImplemented[1] = 0x0;


  /* ======================== Fill InfoBlock->SecurityFeaturesVerified ===================== */
  
  // Assume all checks are FALSE and then check various flags. Accordingly set TRUE
  InfoBlock->SecurityFeaturesVerified[0] = 0x0;  
  InfoBlock->SecurityFeaturesVerified[1] = 0x0;

  // Check 1: SecBoot test (Commented out currently, may enable in the future)
  /* Assume by default we are in setupmode and Secure Boot is not enabled */
  SetupMode = 1;
  VarSize = sizeof(SetupMode);
  Status = gRT->GetVariable (L"SetupMode", &gEfiGlobalVariableGuid,
                             NULL, &VarSize, &SetupMode);
  if (EFI_ERROR(Status))
  {
    //DEBUG(( EFI_D_WARN, "SetupMode Variable does not exist\n"));
  }
  if (SetupMode == 1)
  {
    /* In setup mode i.e. non-secure device */
    //AsciiPrint("SecureBoot check FALSE\r\n");
    //DEBUG((EFI_D_WARN, "UEFI-secureBoot check FALSE\r\n"));
    //AsciiStrCpy(LocalErrorString, "UEFI-secureBoot check fail \r\n");
    //StrCpy(InfoBlock->ErrorString, "UEFI-secureBoot check fail \r\n");
    //OverallSecurityCheckFlag = FALSE;
  }
  else
  {
    /* Not in setup mode i.e. secure device */
    //AsciiPrint("SecureBoot check TRUE\r\n");
    //DEBUG((EFI_D_WARN, "SecureBoot check TRUE\r\n"));
  }

  // Check 2: RPMB provision test
  rpmbcheckflag = IsRpmbProvisioned();              // TRUE implies secure device
  if(rpmbcheckflag)
  {
    //AsciiPrint("rpmbcheckflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "rpmbcheckflag TRUE\r\n"));
  }    
  else
  {
    //AsciiPrint("rpmbcheckflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "rpmbcheckflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "RPMB check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }   
  
  // Check 3: SHK provision test
  shkcheckflag = IsShkProvisioned();                // TRUE implies secure device
  if(shkcheckflag)
  {
    //AsciiPrint("shkcheckflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "shkcheckflag TRUE\r\n"));
  }    
  else
  {
    //AsciiPrint("shkcheckflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "shkcheckflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "SHK check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }    
  
  // Check 4: Rollback enable test
  rollbackcheckflag = IsRollbackEnabled();          // TRUE implies secure device
  if(rollbackcheckflag)
  {
    //AsciiPrint("rollbackcheckflag TRUE\r\n");
    //DEBUG((EFI_D_WARN, "rollbackcheckflag TRUE\r\n"));
  }    
  else
  {
    //AsciiPrint("rollbackcheckflag FALSE\r\n");
    //DEBUG((EFI_D_WARN, "rollbackcheckflag FALSE\r\n"));
    //AsciiStrCat(LocalErrorString, "RollBack check fail \r\n");
    //OverallSecurityCheckFlag = FALSE;
  }
  
  // Check 5: Dummy key use test
  dummykeycheckflag = DummyKeyCheck();              // TRUE implies secure device
  if(dummykeycheckflag)
  {
    //AsciiPrint("dummykeycheckflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "dummykeycheckflag TRUE\r\n"));
  }    
  else
  {
    //AsciiPrint("dummykeycheckflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "dummykeycheckflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "DummyKey check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }
  
  // Check 6: IsProductionDevice test
  proddevicecheckflag = IsProductionDevice();       // TRUE implies secure device
  if(proddevicecheckflag)
  {
    //AsciiPrint("proddevicecheckflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "proddevicecheckflag TRUE\r\n"));
  }
  else
  {
    //AsciiPrint("proddevicecheckflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "proddevicecheckflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "Prod-Device check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }

  // Check 7: PRODMODE enabled check
  prodmodecheckflag = GetProdmodeInfo();              // TRUE implies secure device
  if(prodmodecheckflag)
  {
    //AsciiPrint("prodmodecheckflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "prodmodecheckflag TRUE\r\n"));
  }
  else
  {
    //AsciiPrint("prodmodecheckflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "prodmodecheckflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "Prod-Mode check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }

  // Check 8: Debug Policy check
  debugpolicyflag = GetDebugPolicyFlag();              // TRUE implies secure device
  if(debugpolicyflag)
  {
    //AsciiPrint("debugpolicyflag TRUE\r\n");
    DEBUG((EFI_D_WARN, "debugpolicyflag TRUE\r\n"));
  }
  else
  {
    //AsciiPrint("debugpolicyflag FALSE\r\n");
    DEBUG((EFI_D_WARN, "debugpolicyflag FALSE\r\n"));
    AsciiStrCat(LocalErrorString, "Debug policy check fail \r\n");
    OverallSecurityCheckFlag = FALSE;
  }

  AsciiStrToUnicodeStr(LocalErrorString,InfoBlock->ErrorString);      // Convert to CHAR16



  if (OverallSecurityCheckFlag == TRUE)
  {
    InfoBlock->SecurityFeaturesVerified[0] = 0x1;
  }
  else
  {
    InfoBlock->SecurityFeaturesVerified[0] = 0x0;
  }

  /* ======================================================================================= */

ErrorExit:

  return Status;
}

BOOLEAN GetProdmodeInfo(VOID)
{
  EFI_HOB_GUID_TYPE *GuidHob;
  BOOLEAN           *ProdmodeInfo;
    
  GuidHob = GetFirstGuidHob(&gQcomProdmodeInfoGuid);
  if (GuidHob == NULL)
  {
    DEBUG((EFI_D_INFO, "Prodmode Info Not Found\n"));
    return FALSE;
  }

  ProdmodeInfo = GET_GUID_HOB_DATA(GuidHob);

  if (ProdmodeInfo != NULL)
  {
    return *ProdmodeInfo;
  }

  return FALSE;
}


BOOLEAN DummyKeyCheck(VOID)
{
  BOOLEAN           retVal = TRUE;    // Assume TRUE and later set FALSE if failure
  EFI_STATUS        Status;
  UINT32            AppId = 0;
  UINT8             AppName[] = "qcom.tz.winsecapp";
  WATER_MARK_REQ_T  Request;
  WATER_MARK_RSP_T  Response;
  QCOM_SCM_PROTOCOL *pQcomScmProtocol;
  UINT8 Result[AES256_KEY_SIZE] = {0x80, 0xD4, 0x3D, 0xE2, 0xA6, 0x81, 0x46, 0xE5, 0x4F, 0xDF, 0x85, 0x49, 0x3D, 0x66, 0x42, 0xB8, 0xC9, 0x9C, 0x83, 0xE5, 0x1, 0x9, 0x54, 0x69, 0xDA, 0x34, 0xB1, 0x59, 0xE4, 0x1E, 0x8, 0xC8};

  UINT8             Index;
  UINT8             Index_ini = 0;

  Request.commandId = TZEXEC_SVC_WATERMARK_ID;

  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&pQcomScmProtocol);
  if( Status != EFI_SUCCESS )
  {
    DEBUG((EFI_D_ERROR,"LocateProtocol failed... \r\n"));
    retVal = FALSE;
    goto ErrorExit;
  }

  // Get appid first
  Status = pQcomScmProtocol->ScmSendCommand( pQcomScmProtocol, 
                                             APP_QUERY_APPID_CMD, 
                                            &AppId,
                                             AppName,
                                             sizeof(AppName),
                                             NULL,
                                             0
                                            );
  if (EFI_ERROR(Status)) 
  {
    DEBUG((EFI_D_ERROR, "QueryTpmAppId failed... \r\n"));
    retVal = FALSE;
    goto ErrorExit;
  }
  
  Status = pQcomScmProtocol->ScmSendCommand( pQcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &AppId,
                                             &Request,
                                              sizeof(WATER_MARK_REQ_T),
                                             &Response,
                                              sizeof(WATER_MARK_RSP_T));
  if (EFI_ERROR(Status)) 
  {
    DEBUG((EFI_D_ERROR, "Get watermark failed... \r\n"));
    retVal = FALSE;
    goto ErrorExit;
  }

  //AsciiPrint ("\n");
  for (Index=0; Index < AES256_KEY_SIZE; Index++)
  {
    if (Index - Index_ini == 8) 
    {
      //AsciiPrint ("\n");
      Index_ini = Index;
    }
    //AsciiPrint ("%2x, ",Response.buf[Index]);
  }
  //AsciiPrint ("\n");
  

  // Check for production/dev device dummy key usage. If there is a match, then dummy key is used and the device is NOT secure
  if (CompareMem(Response.buf, Result, AES256_KEY_SIZE) == 0)
    retVal = FALSE;

ErrorExit:

  return retVal; 
}

BOOLEAN GetDebugPolicyFlag(VOID)
{
  BOOLEAN                     DebugPolicyFlag = FALSE;
  QCOM_SCM_PROTOCOL           *pQcomScmProtocol = NULL;
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT64                      Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                      Results[SCM_MAX_NUM_RESULTS]       = {0};

  tz_read_debug_policy_content_req_t *SysCallReq = (tz_read_debug_policy_content_req_t*)Parameters;
  tz_syscall_rsp_t            *SysCallRsp = (tz_syscall_rsp_t*)Results;

  sec_dbg_t*                  ptr_sec_dbg_t = (sec_dbg_t*)AllocatePool(sizeof(sec_dbg_t));
  if (ptr_sec_dbg_t == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  /* Initializing cmd structure for SCM sys call */
  SysCallReq->debug_policy        = (UINTN)ptr_sec_dbg_t;
  SysCallReq->debug_policy_size   = (UINTN)sizeof(sec_dbg_t);

  // Locate QCOM_SCM_PROTOCOL. 
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }
  // Make ScmSipSysCall 
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_INFO_GET_SECURE_STATE,
                                            TZ_INFO_GET_SECURE_STATE_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, " ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

  if (SysCallRsp->status != 1)
  {
    DEBUG((EFI_D_WARN," ScmSipSysCall() failed, common_rsp Status = (0x%x)\r\n", SysCallRsp->status));
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

ErrorExit:

  if(EFI_SUCCESS != Status)
    DebugPolicyFlag = FALSE;      // Error during getting DebugPolicyFlag
  else
    DebugPolicyFlag = TRUE;

  return DebugPolicyFlag;         // Return the DebugPolicyFlag
}


EFI_STATUS RegisterSecurityAdapter()
{
  EFI_STATUS                                  Status = EFI_SUCCESS;
  EFI_GUID                                    AdapterGuid;
  EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL  *pAdapterInformationRegisterProtocol = NULL;
  
  // Below needed only for testing
  
  //EFI_ADAPTER_INFORMATION_PROTOCOL           *pAdapterInformationProtocol = NULL;
  //VOID          *InfoBuffer = NULL;
  //UINTN          InfoBufferCount = 0;
  //UINT32         i;
  

  Status = gBS->LocateProtocol(&gAdapterInformationRegisterProtocolGuid, NULL, (VOID **) &pAdapterInformationRegisterProtocol);

  if ( EFI_ERROR (Status) || pAdapterInformationRegisterProtocol == NULL)
  {
    DEBUG(( EFI_D_ERROR,"RegisterSecurityAdapter: Locate AdapterInformationRegister protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // Install Platform Security Adapter. 
  AdapterGuid = (EFI_GUID)ADAPTER_INFO_PLATFORM_SECURITY_GUID;
  Status = pAdapterInformationRegisterProtocol->RegisterService( pAdapterInformationRegisterProtocol, 
                              &AdapterGuid,
                              &GetPlatformSecurityInfo,
                              NULL );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_ERROR, "RegisterSecurityAdapter: Install Platform Security Adapter failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // Below needed only for testing
  /*
  // AdapterInformationProtocol test case
  Status = gBS->LocateProtocol(&gAdapterInformationProtocolGuid, NULL, (VOID **) &pAdapterInformationProtocol);

  if ( EFI_ERROR (Status) || pAdapterInformationProtocol == NULL )
  {
    DEBUG(( EFI_D_WARN,"ERROR: Locate AdapterInformation protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  Status = pAdapterInformationProtocol->GetInformation( pAdapterInformationProtocol, 
                                                        &AdapterGuid,
                                                        &InfoBuffer,
                                                        &InfoBufferCount );
  if ( Status == EFI_SUCCESS)
  {
    for( i=0; i<InfoBufferCount; i++)
    {
      //DEBUG(( EFI_D_WARN,"Get InfoBuffer. %d \r\n", ((UINT8 *)InfoBuffer)[i]));
    }
  }
  else
  {
    //DEBUG(( EFI_D_WARN,"GetAdapterInformation test fail. \r\n"));
  }

  // release InfoBuffer, caller's responsibility
  if( InfoBuffer != NULL )
  {
    FreePool(InfoBuffer);
    InfoBuffer = NULL;
  }
  */


ErrorExit:
  return Status;
}


