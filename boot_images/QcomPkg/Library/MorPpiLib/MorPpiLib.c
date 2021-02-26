/*                                                                    
                                                                   
                      MorPpiLib.c   

  This library implements the MOR and PPI feature for security purpose.
 
  Copyright (c) 2012-2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 12/10/13   sm       Updated print statements and variable types
 09/18/13   shl      Set initial size of debug policy to 1.
 09/09/13   shl      Added debug policy and PPI configuration check for PPI display.
 08/01/13   shl      Restored MorPpiFlag for better internal management.
 07/09/13   shl      Enable MOR message.
 06/28/13   niting   Added logging for MOR.
 05/21/13   shl      UIPPI fix cleanup.
 04/22/13 bmuthuku   Fix UI PPI issue. The UI user confirmation value appended to the LSB of the IMEM address for HLOS.
 04/12/13   shl      Changed to black screen and use UEFI utility to convert command
                     to avoid alignment crash.
 04/11/13   shl      Some cleanup.
 04/11/13   rsb      Added RaiseTPL during ProcessMor to avoid race condition
                     where an interrupt event callback may re-allocate memory
                     being cleared during MOR
 04/08/13   shl      Moved away from PCD library
 03/25/13   shl      Replace SCAN_F10 with SCAN_F12
 03/12/13   shl      Removed reboot after clear TPM
 02/02/13   shl      Add UI bringup per Microsoft requirement, fix memory free
 12/10/12   shl      Fixed warning
 09/14/12   bmuthuku Fix to not overwrite first byte in IMEM
 07/07/12   niting  Changed logging levels
 06/24/12   shl      Moved MOR bit detection to TCG Specs way.
 06/22/12   shl      Added TPM platform hierachy lock function.
 05/22/12   shl      Made a final fix to replace the hack.
 05/17/12   shl      Fixed crash due to compiler update, added unconditionally
                     MOR feature, and removed extra shift request bytes 
 02/12/12   shl      Removed UI bringup per Microsoft suggestion, added kludge 
                     of writing IMEM in favor of HLOS, once HLOS is ready, must 
                     remove the kludge.
 01/18/12   shl      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/EFITrEE.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi/UefiMultiPhase.h>
#include <Uefi/UefiSpec.h>
#include <Guid/MemoryOverwriteControl.h>
#include "TpmDefs.h"
#include "PpiText.h"
#include <Library/UefiCfgLib.h>
#include <Library/QcomBaseLib.h>

// debugpolicy guid
static EFI_GUID DebugModeVariableGuid = {0x77fa9abd, 0x0359, 0x4d32, { 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b}};

// MOR byte
UINT8   Mor;

#define PENDING_REQ_MASK     0x0000FF00
#define ACTED_REQ_MASK       0x00FF0000
#define ACTED_REQ_RESP_MASK  0xFF000000
UINT32  Ppi;

// TPM platform flags
#define NO_PPI_CLEAR_MASK           0x01
#define NO_PPI_PROVISION_MASK       0x02     
#define NO_PPI_MAINTENANCE_MASK     0x04
#define NO_MOR_PPI_MASK             0x10
#define MOR_PPI_FLAG_DEFAULT           0

//NO_PPI_CLEAR_FLAGS
//These values resemble the MSFT values for Noclear flags
//http://msdn.microsoft.com/en-us/library/windows/desktop/jj660278(v=vs.85).aspx
#define NO_PPI_CLEAR_FALSE          0x03
#define NO_PPI_CLEAR_TRUE           0x04

// used internally to keep the states
UINT8   MorPpiFlag;


// used to track Ppi display
BOOLEAN DisplayPpiFlag;


#define UEFI_FAIL                   0x81
#define UEFI_USER_ABANDON           0x82
// PPI response code
UINT8   PpiResponseCode;

// MorPpi IMEM offset
#define MORPPI_IMEM_OFFSET    0x00000830
#define MORPPI_IMEM_SIZE      0x00000004

// TrEE protocol pointer
static EFI_TREE_PROTOCOL  *pEfiTrEEProtocol = NULL; 

extern EFI_GUID gQcomTokenSpaceGuid;


/**
  Following functions are used to send various commnads to TPM.
  They are:
  SendfTPMClearCmd()
  SendfTPMClearControlCmd()
  
  @retval EFI_STATUS

**/
EFI_STATUS SubmitCommand
(
   UINT32  cmdSize,
   UINT8  *cmd, 
   UINT32  respSize,
   UINT8  *resp
)
{
  EFI_STATUS  Status;

  // locate TrEE protocol
  if( pEfiTrEEProtocol == NULL ) 
  {
    Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, (VOID **) &pEfiTrEEProtocol);
    if(EFI_ERROR (Status) )
    {
      DEBUG(( EFI_D_ERROR, "Locate TrEE protocol failed, Status = (0x%p)\r\n", Status));
      goto ErrorExit;
    }
  }

  Status = pEfiTrEEProtocol->SubmitCommand(pEfiTrEEProtocol, cmdSize, cmd, respSize, resp);
  if (Status != EFI_SUCCESS) 
  {
     DEBUG(( EFI_D_ERROR, " pEfiTrEEProtocol->SubmitCommand failed, Status = (0x%p)\r\n", Status));
     PpiResponseCode = UEFI_FAIL;
     goto ErrorExit;
  }

  // Fail if command failed
  PpiResponseCode = SwapBytes32(((TPM2_RESPONSE_HEADER *)resp)->responseCode);
  if ( PpiResponseCode != TPM_RC_SUCCESS )
  {
    DEBUG(( EFI_D_ERROR, "Response Code error! 0x%08x\r\n", PpiResponseCode ));
    Status = EFI_DEVICE_ERROR;
  }
  else
  {
    Status = EFI_SUCCESS;
  }

ErrorExit:
  return Status;
}


EFI_STATUS SendfTPMLockPlatformHierarchy()
{
  // A blob containing TPM_CC_HiearchyControl with TPM_RH_PLATFORM set to
  // disable and NULL auth data. If auth data is ever changed by the fw in
  // future, this blob must be changed.
  // The actual formatting of command, should one wish to that instead, appears
  // in TrEEEfi/testcases.c
  UINT8 rawCmdBuffer [32] = {
        0x80, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x01, 0x21, 0x40, 0x00,
        0x00, 0x0C, 0x00, 0x00, 0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x0C, 0x00
  };

  UINT8 rawReferenceResBuffer[19] = {
        0x80, 0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00
  };

  UINT8 rawResBuffer[sizeof(rawReferenceResBuffer)];

  EFI_STATUS Status;

  Status = SubmitCommand(sizeof(rawCmdBuffer),
                                rawCmdBuffer,
                                sizeof(rawReferenceResBuffer),
                                rawResBuffer);

  if (EFI_ERROR(Status)) 
  {
    DEBUG((EFI_D_ERROR,"LockPlatformHierarchy : SubmitCommand failed, Status 0x%p\n", Status));
    goto ErrorExit;
  }
  
  // Validate response
  if (CompareMem(rawResBuffer, rawReferenceResBuffer, (UINTN)sizeof(rawReferenceResBuffer)) != 0)
  {
    DEBUG((EFI_D_ERROR,"LockPlatformHierarchy failed. \n"));
    Status = EFI_DEVICE_ERROR;
  }

ErrorExit:
  return Status;
}


EFI_STATUS SendfTPMClearCmd( )
{
  EFI_STATUS          Status = EFI_DEVICE_ERROR;
  TPM2ClearCmdType    cmd;
  TPM2ClearRespType   resp;
  UINT8              *cmdBuffer = NULL;
  UINT32              cmdBufferSize;
  UINT8              *buffer;
  UINT32              bufferSize;
  UINT8              *authSizeOffset;
  UINT8              *parameterSizeOffset;

  // 8 is from handle and add in auth session
  cmdBufferSize = sizeof(cmd) + 8 ;
  cmdBuffer = AllocatePool( cmdBufferSize );
  if( cmdBuffer == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  SetMem(cmdBuffer,  cmdBufferSize, 0);

  // populate data
  buffer = cmdBuffer;

  // Construct command
  // tag
  WriteUnaligned16 ((UINT16 *)buffer, SwapBytes16(TPM_ST_SESSIONS));
  buffer += sizeof(UINT16);

  // skip paramSize temprarily
  parameterSizeOffset = buffer;
  buffer += sizeof(UINT32);

  // command code
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_CC_Clear));
  buffer += sizeof(UINT32);

  // handle, this is extra, not defined in structure, needed for TPM pasrsing.
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_RH_PLATFORM));
  buffer += sizeof(UINT32);

  // Add in Auth session, this is extra, not defined in structure, needed for TPM pasrsing.
  authSizeOffset = buffer;
  WriteUnaligned32 ((UINT32 *)buffer, 0);
  buffer += sizeof(UINT32);

  // authHandle
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_RS_PW));
  buffer += sizeof(UINT32);

  // nonce = nullNonce
  WriteUnaligned16 ((UINT16 *)buffer, 0);
  buffer += sizeof(UINT16);

  // sessionAttributes = 0
  *(UINT8 *)buffer = 0;
  buffer += sizeof(UINT8);

  // auth = nullAuth
  WriteUnaligned16 ((UINT16 *)buffer, 0);
  buffer += sizeof(UINT16);

  // authorizationSize
  *(UINT32 *)authSizeOffset = SwapBytes32((UINT32)(buffer - authSizeOffset - sizeof(UINT32)));

  bufferSize = (UINT32)(buffer - cmdBuffer);
  *(UINT32 *)parameterSizeOffset = SwapBytes32(bufferSize);

  Status = SubmitCommand( bufferSize, cmdBuffer, sizeof(resp), (UINT8*)&resp );
  if(Status != EFI_SUCCESS) 
  {
    DEBUG(( EFI_D_ERROR, "SubmitCommand failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

ErrorExit:
  if( cmdBuffer != NULL )
  {
    FreePool(cmdBuffer);
  }
  return Status;
}


EFI_STATUS SendfTPMClearControlCmd( )
{
  EFI_STATUS Status      = EFI_DEVICE_ERROR;
  TPM2ClearControlCmdType  cmd;
  TPM2ClearControlRespType resp;
  UINT8                   *cmdBuffer = NULL;
  UINT32                   cmdBufferSize;
  UINT8                   *buffer;
  UINT32                   bufferSize;
  UINT8                   *authSizeOffset;
  UINT8                   *parameterSizeOffset;

  // 8 is from handle and add in auth session
  cmdBufferSize = sizeof(cmd) + 8 ;
  cmdBuffer = AllocatePool( cmdBufferSize );
  if( cmdBuffer == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  SetMem(cmdBuffer, cmdBufferSize, 0);

  // populate data
  buffer = cmdBuffer;

  // Construct command
  // tag
  WriteUnaligned16 ((UINT16 *)buffer, SwapBytes16(TPM_ST_SESSIONS));
  buffer += sizeof(UINT16);

  // skip paramSize temprarily
  parameterSizeOffset = buffer;
  buffer += sizeof(UINT32);

  // command code
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_CC_ClearControl));
  buffer += sizeof(UINT32);

  // handle, this is extra, not defined in structure, needed for TPM pasrsing.
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_RH_PLATFORM));
  buffer += sizeof(UINT32);

  // Add in Auth session, this is extra, not defined in structure, needed for TPM pasrsing.
  authSizeOffset = buffer;
  WriteUnaligned32 ((UINT32 *)buffer, 0);
  buffer += sizeof(UINT32);

  // authHandle
  WriteUnaligned32 ((UINT32 *)buffer, SwapBytes32(TPM_RS_PW));
  buffer += sizeof(UINT32);

  // nonce = nullNonce
  WriteUnaligned16 ((UINT16 *)buffer, 0);
  buffer += sizeof(UINT16);

  // sessionAttributes = 0
  *(UINT8 *)buffer = 0;
  buffer += sizeof(UINT8);

  // auth = nullAuth
  WriteUnaligned16 ((UINT16 *)buffer, 0);
  buffer += sizeof(UINT16);

  // authorizationSize
  *(UINT32 *)authSizeOffset = SwapBytes32((UINT32)(buffer - authSizeOffset - sizeof(UINT32)));

  // disable
  *(UINT8 *)buffer = SwapBytes32(NO);
  buffer += sizeof(UINT8);

  bufferSize = (UINT32)(buffer - cmdBuffer);
  *(UINT32 *)parameterSizeOffset = SwapBytes32(bufferSize);

  Status = SubmitCommand( bufferSize, cmdBuffer, sizeof(resp), (UINT8*)&resp );
  if(Status != EFI_SUCCESS) 
  {
    DEBUG(( EFI_D_ERROR, "SubmitCommand failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

ErrorExit:
  if( cmdBuffer != NULL )
  {
    FreePool(cmdBuffer);
  }

  return Status;
}


/**
  This is toset new Mor
  
  @retval EFI_STATUS

**/
EFI_STATUS StoreMor( )
{
  EFI_STATUS  Status;
  UINTN       MorSize = sizeof(UINT8);

  Status = gRT->SetVariable ( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                              (EFI_VARIABLE_NON_VOLATILE | 
                               EFI_VARIABLE_BOOTSERVICE_ACCESS |
                               EFI_VARIABLE_RUNTIME_ACCESS),
                              MorSize,
                             &Mor);
  if( EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_ERROR, "set Mor ---.\r\n"));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "set Mor +++.\r\n"));
  }

  return Status;
}

/**
  This is to store new Ppi value
  
  @retval EFI_STATUS

**/
VOID SetPpiValue ( )
{
  UINT32  PendingReq = ( Ppi & PENDING_REQ_MASK ) >> 8;

  // reset MorPpimem
  Ppi = 0;
  // set result of last acted on request
  Ppi |= (PpiResponseCode << 24);
  // set last acted on request
  Ppi |= PendingReq << 16;
  //Set the LSB with the Noclear flag value.
  if( ( MorPpiFlag & NO_PPI_CLEAR_MASK ) == 0 )
  {
    Ppi |= NO_PPI_CLEAR_FALSE;
  }
  else
  {
    Ppi |= NO_PPI_CLEAR_TRUE;
  } 
}


/**
  This is to set new Ppi
  
  @retval EFI_STATUS

**/
EFI_STATUS StorePpi( )
{
  EFI_STATUS  Status;
  UINT32      IMemBase = 0;
  UINT32     *IMemMorPpiBase = NULL;
//Changing the name of the PPI variable, this will contain 3 bytes of existing PPI info and 1 byte of UI info (internal state).
  Status = gRT->SetVariable (L"UIPPI", 
                             &gQcomTokenSpaceGuid, 
                              (EFI_VARIABLE_NON_VOLATILE | 
                               EFI_VARIABLE_BOOTSERVICE_ACCESS |
                               EFI_VARIABLE_RUNTIME_ACCESS),
                              sizeof(UINT32), 
                             &Ppi);
  if( EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_ERROR, "set UIPpi ---.\r\n"));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "set UIPpi +++.\r\n"));

    // retrieve from IMEM base address.
    Status = GetConfigValue ("SharedIMEMBaseAddr", &IMemBase);

    if ( Status != EFI_SUCCESS )
    {
      goto ErrorExit;
    }
    else
    {  	  
      IMemMorPpiBase = (UINT32 *)(UINTN)(IMemBase + MORPPI_IMEM_OFFSET);
    }

    // Following is kludge, because HLOS side is not ready, do this in favor of HLOS.
    // this has to be removed when HLOS is ready, this is target dependent code.
    // retrieve from IMEM.    
    *IMemMorPpiBase = Ppi;

    DEBUG(( EFI_D_INFO, " *IMemMorPpiBase = (0x%x)\r\n",*IMemMorPpiBase ));
  }

ErrorExit:
  return Status;
}

/**
  This is to set new MorPpiFlag
  
  @retval EFI_STATUS

**/
EFI_STATUS StoreMorPpiFlag( )
{
  EFI_STATUS Status;
  Status = gRT->SetVariable (L"MorPpiFlag", 
                             &gQcomTokenSpaceGuid, 
                              (EFI_VARIABLE_NON_VOLATILE | 
                               EFI_VARIABLE_BOOTSERVICE_ACCESS |
                               EFI_VARIABLE_RUNTIME_ACCESS),
                              sizeof(MorPpiFlag), 
                             &MorPpiFlag);
  if( EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_ERROR, "Set MorPpiFlag ---.\r\n"));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "Set MorPpiFlag +++.\r\n"));
  }

  return Status;
}


/**
  This is to set new DisplayPpiFlag
  
  @retval EFI_STATUS

**/
EFI_STATUS StoreDisplayPpiFlag( )
{
  EFI_STATUS Status;
  Status = gRT->SetVariable (L"DisplayPpiFlag", 
                             &gQcomTokenSpaceGuid, 
                              (EFI_VARIABLE_NON_VOLATILE | 
                               EFI_VARIABLE_BOOTSERVICE_ACCESS |
                               EFI_VARIABLE_RUNTIME_ACCESS),
                              sizeof(DisplayPpiFlag), 
                             &DisplayPpiFlag);
  if( EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_ERROR, "Set DisplayPpiFlag ---.\r\n"));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "Set DisplayPpiFlag +++.\r\n"));
  }

  return Status;
}

/**
  This is called to handle Memoery Overwrite Request.

  "A general description of the scheme is that after any type of
  Host Platform Reset event (with the exception of a CPU-only 
  reset that is used by some chipsets to turn off a CPU feature
  without re-setting other Host Platform components), if signaled
  to do so by the OS, the POST BIOS must, prior to executing any
  non-BIOS code, overwrite system memory."
  
  @retval EFI_STATUS

**/
EFIAPI EFI_STATUS ProcessMor( )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  UINTN                   MemoryMapSize = 0;
  EFI_MEMORY_DESCRIPTOR  *MemoryMap = NULL;
  EFI_MEMORY_DESCRIPTOR  *MemoryMapTemp = NULL;
  UINTN                   MapKey = 0;
  UINTN                   DescriptorSize = 0;
  UINT32                  DescriptorVersion = 0;
  UINTN                   Index;
  EFI_TPL                 OriginalTPL;

  DEBUG ((DEBUG_ERROR, "MOR Start : %d\n", GetTimerCountms ()));
  Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );

  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    MemoryMapSize += (DescriptorSize * 0x80);
    MemoryMap = AllocatePool( MemoryMapSize);

    if( MemoryMap != NULL )
    {
      // RaiseTPL to TPL_CALLBACK to run in Totify context during memory clear
      // to avoid race condition where an interrupt event callback may
      // re-allocate memory being cleared during MOR.
      OriginalTPL = gBS->RaiseTPL (TPL_CALLBACK);

      // Get System MemoryMap
      Status = gBS->GetMemoryMap (
                      &MemoryMapSize,
                      MemoryMap,
                      &MapKey,
                      &DescriptorSize,
                      &DescriptorVersion
                      );

      MemoryMapTemp = MemoryMap;

      if ( Status == EFI_SUCCESS )
      {
        DEBUG(( EFI_D_INFO, "Got memory map. \r\n"));

        // clear memory here
        for (Index = 0; Index < MemoryMapSize/DescriptorSize; Index++)
        {
          if(MemoryMapTemp->Type == EfiConventionalMemory )
          {
            SetMem( (VOID *)(UINTN)(MemoryMapTemp->PhysicalStart), MemoryMapTemp->NumberOfPages*EFI_PAGE_SIZE, 0 );    
            DEBUG(( EFI_D_INFO, " MOR memory set to 0.\r\n"));
          }
          MemoryMapTemp = NEXT_MEMORY_DESCRIPTOR (MemoryMapTemp, DescriptorSize);    
        }
      }
      // restore TPL
      gBS->RestoreTPL (OriginalTPL);

      FreePool(MemoryMap);
    }
  }
  DEBUG ((DEBUG_ERROR, "MOR Complete : %d\n", GetTimerCountms ()));

  return Status;
}

/**
  This is called to send TPM2_ClearContorl(NO) and TMP2_Clear commandds to TPM.
  It will reboot after command is sent. If it is failed, we are on the safe side.

**/
EFI_STATUS SendTpmClearCmd( )
{
  EFI_STATUS Status; 

  // TPM2_ClearContorl(NO) + TMP2_Clear....
  if( (Status = SendfTPMClearControlCmd( )) != EFI_SUCCESS ||
      (Status = SendfTPMClearCmd( ))        != EFI_SUCCESS  ) 
  {
    // set the return value
    PpiResponseCode = UEFI_FAIL;
 
    // something happened, what to do?
    DEBUG(( EFI_D_ERROR, "SendfTPMClearControlCmd/SendfTPMClearCmd ---.\r\n"));
  }

  return Status;
}

/**
 	Variable GUID:  77fa9abd-0359-4d32-bd60-28f4e78f784b 
 	Variable Name: "CurrentPolicy"
 
    If the variable exist, and its value size is just one byte
 	then it is a dummy policy and the system is in production
 	mode. If the variable exist and its size is larger, then the
 	system may either have a custom production policy or a debug
 	policy. The former is unlikely as MSFT do not intend to
 	create any custom production policies and so you will be
 	able to determine that the system is in debug mode.
 
    @return TRUE   if debug policy is enabled
	        FLASE  if not enabled
 
*/
BOOLEAN IsDebugMode( VOID)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       DebugMode ;
    UINTN       VarSize = sizeof(DebugMode);

    VarSize = sizeof(DebugMode);
    Status = gRT->GetVariable (L"CurrentPolicy", &DebugModeVariableGuid,
                               NULL, &VarSize, &DebugMode);

    if( Status == EFI_BUFFER_TOO_SMALL )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
}


/**
  This is called to handle Physical Presence Interface.
  
  @retval EFI_STATUS

**/
EFIAPI EFI_STATUS ProcessPpi( )
{
  EFI_STATUS     Status = EFI_SUCCESS; 
  EFI_INPUT_KEY  Key = {0};
  UINT32         PendingReq = (Ppi & PENDING_REQ_MASK) >> 8;
  UINT32         i, SizeOfPtrArray;
  UINT32         ReadKeyAttrib = 0;

  ReadKeyAttrib |= READ_KEY_ATTRIB_NO_BLOCKING;

  //set response code
  PpiResponseCode = TPM_RC_SUCCESS;

  // process pending request
  switch( PendingReq )
  {
    case 0:
      break;

    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 15:
    case 16:
    case 19:
    case 20:
      // no operation, but need to remember operation number and return success.
      break;

    case 5:
    case 14:
    case 21:
    case 22:
      if( (IsDebugMode() == TRUE) && (DisplayPpiFlag == FALSE) )
      {
        // factory mode, no need for bringup user confirmation screen, just clear TPM.
        Status = SendTpmClearCmd();
        break;
      }

      if( ( MorPpiFlag & NO_PPI_CLEAR_MASK ) == 0 )
      {
        // bring up user confirmation screen
        gST->ConOut->SetAttribute(gST->ConOut, (EFI_WHITE | EFI_BACKGROUND_BLUE) );
        // draw blue screen 
        gST->ConOut->ClearScreen (gST->ConOut);
	
        SizeOfPtrArray = sizeof( ClearLinePtr )/sizeof(ClearLinePtr[0]);
        for( i=0; i<SizeOfPtrArray; i++)
        {
          AsciiPrint((const CHAR8 *)ClearLinePtr[i]); 
        }
  
        while(TRUE)
        { 
          Status = ReadAnyKey (&Key, ReadKeyAttrib);

          if ( (Key.ScanCode == SCAN_UP ) || (Key.ScanCode == SCAN_DOWN)  ||
               (Key.ScanCode == SCAN_F12) || (Key.ScanCode == SCAN_ESC )  )
          {
            break;
          }
        }

        // If user hits the YES Response key, reset
        if ( (Key.ScanCode == SCAN_UP )||( Key.ScanCode == SCAN_F12 ))
        {
          // user confirmed, should reboot after clear
          Status = SendTpmClearCmd();
        }
        else if((Key.ScanCode == SCAN_DOWN) || (Key.ScanCode == SCAN_ESC) )
        {
          // user rejected, no clear
          PpiResponseCode = UEFI_USER_ABANDON;
        }
        // clear user confirmation screen         
        gST->ConOut->SetAttribute(gST->ConOut, (EFI_WHITE | EFI_BACKGROUND_BLACK) );
        gST->ConOut->ClearScreen (gST->ConOut);
      } // need user confirmation case
      else
      {
        // no need for bringup user confirmation screen, just clear TPM.
        Status = SendTpmClearCmd();
      }
      break;

    case 17:
      // Optional, set NoPpiClearFlag to FALSE, no physical presence needed.
      if( MorPpiFlag & NO_PPI_CLEAR_MASK)
      {
        MorPpiFlag &= ~NO_PPI_CLEAR_MASK;
        Status = StoreMorPpiFlag( );
        if( EFI_ERROR (Status))
        {
          PpiResponseCode = UEFI_FAIL;
        }
      }
      break;

    case 18:
      if( (IsDebugMode() == TRUE) && (DisplayPpiFlag == FALSE) )
      {
        // factory mode for not press key. Don't show PPI screen
        if((MorPpiFlag & NO_PPI_CLEAR_MASK) == 0)
        {
          MorPpiFlag |= NO_PPI_CLEAR_MASK;
          Status = StoreMorPpiFlag( );
          if( EFI_ERROR (Status))
          {
            PpiResponseCode = UEFI_FAIL;
          }
        }
        break;
      }

      // set NoPpiClearFlag to TRUE, always needs physical presence
      // bring up user confirmation screen. 
      // bring up user confirmation screen
      gST->ConOut->SetAttribute(gST->ConOut, (EFI_WHITE | EFI_BACKGROUND_BLUE) );
      // draw blue screen 
      gST->ConOut->ClearScreen (gST->ConOut);
	
      SizeOfPtrArray = sizeof( NoPpiClearLinePtr )/sizeof(NoPpiClearLinePtr[0]);
      for( i=0; i<SizeOfPtrArray; i++)
      {
        AsciiPrint((const CHAR8 *)NoPpiClearLinePtr[i]); 
      }
  
      while(TRUE)
      {	 
        Status = ReadAnyKey (&Key, ReadKeyAttrib);

        if ( (Key.ScanCode == SCAN_UP ) || (Key.ScanCode == SCAN_DOWN)  ||
             (Key.ScanCode == SCAN_F12) || (Key.ScanCode == SCAN_ESC )  )
        {
          break;
        }
      }

      if ( (Key.ScanCode == SCAN_UP )||( Key.ScanCode == SCAN_F12 ))
      {
        // user confirmed
        if((MorPpiFlag & NO_PPI_CLEAR_MASK) == 0)
        {
          MorPpiFlag |= NO_PPI_CLEAR_MASK;
          Status = StoreMorPpiFlag( );
          if( EFI_ERROR (Status))
          {
            PpiResponseCode = UEFI_FAIL;
          }
        }
      }
      else if((Key.ScanCode == SCAN_DOWN) || (Key.ScanCode == SCAN_ESC) )
      {
        // user rejected, no clear
        PpiResponseCode = UEFI_USER_ABANDON;
      }
      // clear user confirmation screen
      gST->ConOut->SetAttribute(gST->ConOut, (EFI_WHITE | EFI_BACKGROUND_BLACK) );
      gST->ConOut->ClearScreen (gST->ConOut);
      break;

    default:
      // not supported request
      PpiResponseCode = UEFI_FAIL;
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  This is called to process the MOR/PPI request. 
  
  @retval EFI_STATUS

**/
EFIAPI EFI_STATUS ProcessMorPpi( )
{
  EFI_STATUS   Status;
  UINTN        MorSize = sizeof(UINT8);
  UINTN        PpiSize = sizeof(UINT32);
  UINTN        MorPpiFlagSize = sizeof(UINT8);
  UINTN        DisplayPpiFlagSize = sizeof(BOOLEAN); 

  // read MorPpiFlag variable. This is internal flag
  Status = gRT->GetVariable (L"DisplayPpiFlag",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DisplayPpiFlagSize,
                             &DisplayPpiFlag);
  switch(Status)
  {
    case EFI_SUCCESS:
      DEBUG(( EFI_D_INFO, " get DisplayPpiFlag successfully \r\n" ));
      break;

    case EFI_NOT_FOUND:
      // init DisplayPpiFlag, should do just once life time
      DisplayPpiFlag = FALSE;
      Status = StoreDisplayPpiFlag( );
      DEBUG(( EFI_D_INFO, "Store DisplayPpiFlag \r\n" ));
      break;

    default:
      //other error code, let it go, continue boot
      DisplayPpiFlag = TRUE;
      break;
  }


  // read MorPpiFlag variable. This is internal flag
  Status = gRT->GetVariable (L"MorPpiFlag",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &MorPpiFlagSize,
                             &MorPpiFlag);
  switch(Status)
  {
    case EFI_SUCCESS:
      if(MorPpiFlag & NO_MOR_PPI_MASK)
      { 
        // this boot is coming from previous MOR/PPI session, reset flag
        MorPpiFlag &= ~NO_MOR_PPI_MASK;
        Status = StoreMorPpiFlag( );

        DEBUG(( EFI_D_INFO, "this boot is coming from previous MOR/PPI session\r\n" ));

        // bail out to continue boot      
        goto Exit;
      }

      DEBUG(( EFI_D_INFO, "regular MOR/PPI session\r\n" ));

      break;

    case EFI_NOT_FOUND:
      // init MorPpiFlag, should do just once life time
      MorPpiFlag = MOR_PPI_FLAG_DEFAULT;
      Status = StoreMorPpiFlag( );

      DEBUG(( EFI_D_INFO, "First time MorPpiFlag \r\n" ));

      break;

    default:
      //other error code, let it go, continue boot
      break;
  }

  // read Mor variable. This is interface between OS and UEFI
  Status = gRT->GetVariable (MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             NULL,
                             &MorSize,
                             &Mor);
  switch(Status)
  {
    case EFI_SUCCESS:
      // check MOR request
      if( Mor & MOR_CLEAR_MEMORY_BIT_MASK )
      {
        // process memory overwrite request
        Status = ProcessMor(); 
        DEBUG(( EFI_D_WARN, "ProcessMor() Status = (0x%p)\r\n", Status));
        Mor &= ~MOR_CLEAR_MEMORY_BIT_MASK;
        // set and store new value
       Status = StoreMor( );
      }

      break;

    case EFI_NOT_FOUND:
      // init Mor, do it once for safe 
      Status = ProcessMor(); 
      // create Mor variable
      Mor = 0;        
      Status = StoreMor( );
      DEBUG(( EFI_D_WARN, "First time Mor \r\n" ));

      break;

    default:
      //other error code
      break;
  }

  // read Ppi variable. This is interface bwteen OS and UEFI
  Status = gRT->GetVariable (L"UIPPI",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &PpiSize,
                             &Ppi);
  switch(Status)
  {
    case EFI_SUCCESS:
      // process PPI request
      if( Ppi & PENDING_REQ_MASK )
      {
        // physical presence interface
        Status = ProcessPpi(); 
        DEBUG(( EFI_D_INFO, " Ppi = (0x%x)\r\n", Ppi ));
      }

      // set and store new value
      SetPpiValue();
      Status = StorePpi( );
      break;

    case EFI_NOT_FOUND:
      // init Ppi, 
      Ppi = 0;
      Status = StorePpi( );
      DEBUG(( EFI_D_INFO, "First time Ppi\r\n" ));
      break;

    default:
      //other error code
      break;
  }

  Exit:
  // lock the TPM platform hierarchy
  Status = SendfTPMLockPlatformHierarchy();

  return Status;
}
