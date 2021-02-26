/** @file DmoviEFITest.c
   
  This file contains a sample usage of DMOV EFI Protocol interface.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 5/31/11    rl      Created.

=============================================================================*/



#include <Protocol/EFIDmov.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/CacheMaintenanceLib.h>
#include "DmovTestApp.h"

/* transfer request status returned by Dmov driver */
static EFI_DMOV_REQ_STATUS ReqStatus = { EFI_DMOV_REQUEST_INITIAL, EFI_DMOV_INVALID_TOKEN };

/* Dmov transfer result structure used while querying for the transfer result */
static EFI_DMOV_RESULT RequestResult = 
{ 
    .eState=EFI_DMOV_TRANSFER_WAITING_TO_BE_ISSUED,
    .dwResultValue = 0xFFFFFFFF,
    .bMoreResultsAvailable = FALSE,
    .FlushState = { 0x0,0x0,0x0,0x0,0x0,0x0},
    .eChanError = EFI_DMOV_CHAN_NO_ERROR,
    .pUserData = 0
};

/* First word of the dmov command structure */
static EFI_DMOV_CMD_OPTIONS CommandOptions =
{
    .dwAddrMode = EFI_DMOV_CMD_ADDR_MODE_SINGLE,
	.dwSrcCRCI = 0,
    .dwDstCRCI = 0,
    .dwSrcEndianness = 0,
    .dwDstEndianness = 0,
    .bBlockThisChannel = 0,
    .bBlockOtherChannel = 0,
    .bUnblockOtherChannel = 0,
    .bLastCommand = TRUE
};

/* transfer request submitted to Dmov driver */
static EFI_DMOV_TRANSFER_REQUEST TransferRequest =
{
    .dwDevice = 0,                  
	.dwChan = UEFI_CHANNEL,
	.dwPriority = 0,                 
	.pUserData = 0,
	.dwNumCmds = 1,
    .CommandType = EFI_DMOV_CMD_PTR_LIST,
};

/**
  DMOV EFI protocol test
*/
EFI_STATUS DmovTransferEFIProtocol(DmovTestMemoryHandle* pMemHandle,unsigned int deviceId)
{
    EFI_DMOV_PROTOCOL*   hDmovProtocol;
    EFI_STATUS           status;
    EFI_DMOV_SINGLE_CMD* pSM;

    AsciiPrint("\n\nDmov EFI API Test \n\n");

    /* Map the device Ids to DAL device Ids */
    deviceId = (deviceId==DMOV_TEST_ADM1)?EFI_DMOV_DEVICE_ADM1:EFI_DMOV_DEVICE_ADM0;

    /* Randomize the source buffer */
    RandomizeSrcBuffers((unsigned char*)pMemHandle->pSourceBuffer);

    /* manage cache */
    WriteBackDataCacheRange((void*)pMemHandle->pSourceBuffer,DMOV_BUFFER_SIZE);

    pSM = (EFI_DMOV_SINGLE_CMD*)pMemHandle->pCmd;
    pSM->dwSrcAddr = (unsigned int)pMemHandle->pSourceBuffer;
    pSM->dwDstAddr = (unsigned int)pMemHandle->pDestBuffer;
    pSM->dwLength = DMOV_BUFFER_SIZE;

    /* locate the dmov protocol handle */
    status = gBS->LocateProtocol(
                                 &gEfiDmovProtocolGuid,
                                 NULL,
                                 (void**)&hDmovProtocol);
    if (EFI_SUCCESS != status)
    {
        AsciiPrint("Locating Dmov EFI protocol        [FAILED]\n");
        return status; 
    }
    AsciiPrint("Locating Dmov EFI protocol       [SUCCESS]\n");

    /* open the device */
    status=hDmovProtocol->Open(hDmovProtocol,deviceId);

    if ( EFI_SUCCESS != status )
    {
        AsciiPrint("Open Dmov device                  [FAILED]\n");
        return status; 
    }
    AsciiPrint("Open Dmov device                 [SUCCESS]\n");

    /* prepare the command */
    status=hDmovProtocol->PrepareCommand(hDmovProtocol,deviceId,&CommandOptions,(unsigned int*)pSM);

    if ( status != EFI_SUCCESS)
    {
        AsciiPrint("Prepare Dmov command              [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Prepare Dmov command             [SUCCESS]\n");

    /* prepare the command list pointer*/
    status=hDmovProtocol->PrepareCommandListPtr(hDmovProtocol,
                                                EFI_DMOV_DEVICE_ADM0,
                                                (unsigned int*)pMemHandle->pCmd,
                                                TRUE,
                                                (unsigned int*)pMemHandle->pCmdPtr);
    if ( status != EFI_SUCCESS)
    {
        AsciiPrint ("Prepare command pointer list [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint ("Prepare command pointer list [SUCCESS]\n");

    /* set the channel mode to POLLING. In bootloaders POLLING is the only mode supported */
    status=hDmovProtocol->SetChanOperationMode(hDmovProtocol,
                                               EFI_DMOV_DEVICE_ADM0,
                                               UEFI_CHANNEL,
                                               EFI_DMOV_CHAN_MODE_POLLING);

    if ( status != EFI_SUCCESS)
    {
        AsciiPrint("Set ch operation mode to polling  [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Set ch operation mode to polling [SUCCESS]\n");

    /* manage cache */
    WriteBackInvalidateDataCacheRange((void*)pSM,sizeof(EFI_DMOV_SINGLE_CMD));
    WriteBackInvalidateDataCacheRange((void*)pMemHandle->pCmdPtr,CPLE_SIZE);

    /* Submit the transfer request */
    status=hDmovProtocol->Transfer(hDmovProtocol,
                                   deviceId,
                                   &TransferRequest,
                                   (unsigned int*)pMemHandle->pCmdPtr,
                                   &ReqStatus);

    if ( status != EFI_SUCCESS)
    {
        AsciiPrint("Submit a transfer request         [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Submit a transfer request        [SUCCESS]\n");

    /* check if the transfer request was successfull */
    if (EFI_DMOV_REQUEST_OK != ReqStatus.eState)
    {
        AsciiPrint("Check transfer request status     [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Check transfer request status    [SUCCESS]\n");


    AsciiPrint ("Starting the polling loop\n");
    /* Poll for a result */
    do
    {
        status=hDmovProtocol->GetTransferResult(hDmovProtocol,
                                                deviceId,
                                                ReqStatus.dwToken,
                                                &RequestResult);
        if ( status != EFI_SUCCESS)
        {
            AsciiPrint("Get transfer result               [FAILED]\n");
            goto cleanup;
        }

        if (EFI_DMOV_TRANSFER_IN_PROGRESS == RequestResult.eState)
        {
            AsciiPrint ("Transfer in progress....\n");
            continue;
        }
        else
        {
            AsciiPrint ("Transfer completed\n");
            break;
        }
    }while(1);

    if ( EFI_DMOV_TRANSFER_SUCCESSFULL != RequestResult.eState)
    {
        AsciiPrint ("Transfer was unsuccessfull\n");
        goto cleanup;
    }

    AsciiPrint ("Transfer Successfull \n");

    /* check if the transfer actually happened */
    if ( FALSE == IsDataValid((char*)pMemHandle->pSourceBuffer,(char*)pMemHandle->pDestBuffer))
    {
        AsciiPrint("Validating transfered data        [FAILED] \n");
        goto cleanup;
    }
    AsciiPrint("Validating transfered data       [SUCCESS] \n");

    AsciiPrint ("Closing Dmov device \n");

cleanup:
    hDmovProtocol->Close(hDmovProtocol,deviceId);
    return status;
}

