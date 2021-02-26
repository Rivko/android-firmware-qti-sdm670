/** @file DmoviDALTest.c
   
  This file contains a sample usage of DMOV DAL interface.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 5/31/11    rl      Created.

=============================================================================*/

#include "DDIDmov.h"
#include "DALSys.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DmovTestApp.h"
#include <Library/DebugLib.h>
#include <Library/CacheMaintenanceLib.h>

/* transfer request status returned by Dmov driver */
static DmovReqStatus RequestStatus =
{
  DMOV_REQUEST_INITIAL,
  DMOV_INVALID_TOKEN
};

/* Dmov transfer result structure used while querying for the transfer result */
static DmovResult RequestResult = 
{
    .eState=DMOV_TRANSFER_WAITING_TO_BE_ISSUED,
    .dwResultValue = 0xFFFFFFFF,
    .bMoreResultsAvailable = FALSE,
    .FlushState = { 0x0,0x0,0x0,0x0,0x0,0x0},
    .eChanError = DMOV_CHAN_NO_ERROR,
    .pUserData = 0 
};

/* First word of the dmov command structure */
static DmovCommandOptions CommandOptions =
{
    .dwAddrMode = DMOV_CMD_ADDR_MODE_SINGLE,
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
static DmovTransferRequest TransferRequest =
{
    .dwDevice = 0,                  
    .dwChan = UEFI_CHANNEL,
    .dwPriority = 0,                 
    .pUserData = 0,
    .hEvent = 0,
    .dwNumCmds = 1,
    .TransferType = DMOV_TRANSFER_SYNCHRONOUS,
    .CommandType = DAL_DMOV_CMD_PTR_LIST,
    .cmd_ptr = 0,            
    .pCallbackFunction = 0,  
    .options = 0            
};

/*
 *  Use DAL API and perform a data transfer from memory->memory
 */

EFI_STATUS DmovTransferDALApi(DmovTestMemoryHandle* pMemHandle,unsigned int deviceId)
{
    DALResult dwResult;
    DalDeviceHandle* hDmov;
    DmovSingleCommand* pSM;

    AsciiPrint("\nDmov DAL API Test \n\n");

    DALSYS_InitMod(NULL);

    /* Map the device Ids to DAL device Ids */
    deviceId= (deviceId==DMOV_TEST_ADM1)?DALDEVICEID_DMOV_DEVICE_2:DALDEVICEID_DMOV_DEVICE_0;

    /* Randomize the source buffer */
    RandomizeSrcBuffers((unsigned char*)pMemHandle->pSourceBuffer);

    /* manage cache */
    WriteBackDataCacheRange((void*)pMemHandle->pSourceBuffer,DMOV_BUFFER_SIZE);

    pSM = (DmovSingleCommand*)pMemHandle->pCmd;
    pSM->dwSrcAddr = (uint32)pMemHandle->pSourceBuffer;
    pSM->dwDstAddr = (uint32)pMemHandle->pDestBuffer;
    pSM->dwLength = DMOV_BUFFER_SIZE;


    AsciiPrint("Testing Device 0x%x\n",deviceId);
    /* Attach to the dmov device */
    if ( DAL_SUCCESS != DAL_DmovDeviceAttachEx("UEFI_TEST",
                                                deviceId, 
                                                &hDmov))
    {
        AsciiPrint("Attaching to Dmov DAL device      [FAILED]\n");
        return EFI_PROTOCOL_ERROR;
    }
    AsciiPrint("Attaching to Dmov DAL device     [SUCCESS]\n");

    /* open the device */
    if ( DAL_SUCCESS != DalDevice_Open(hDmov, DAL_OPEN_SHARED))
    {
        AsciiPrint("Opening Dmov Device               [FAILED]\n");
        return EFI_PROTOCOL_ERROR;
    }
    AsciiPrint("Opening Dmov Device              [SUCCESS]\n");

    /* prepare the command */
    dwResult = DalDmov_PrepareCommand(hDmov,&CommandOptions,(unsigned int*)pSM);
    if ( DAL_SUCCESS != dwResult)
    {
        AsciiPrint("Prepare Command                   [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Prepare Command                  [SUCCESS]\n");

    /* prepare the command list pointer*/
    dwResult=DalDmov_PrepareCommandListPtr(hDmov,
                                           (unsigned int*)pMemHandle->pCmd,
                                           TRUE,
                                           (unsigned int*)pMemHandle->pCmdPtr);
    if ( DAL_SUCCESS != dwResult )
    {
        AsciiPrint("Prepare command list pointer      [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Prepare command list pointer     [SUCCESS]\n");

    /* set the channel mode to POLLING. In bootloaders POLLING is the only mode supported */
    dwResult = DalDmov_SetChanOperationMode(hDmov,UEFI_CHANNEL,
                                            DMOV_CHAN_MODE_POLLING);
    if ( DAL_SUCCESS != dwResult )
    {
        AsciiPrint("Set ch operation mode to polling  [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Set ch operation mode to polling [SUCCESS]\n");
    
    /* Manage the cache */
    WriteBackInvalidateDataCacheRange((void*)pSM,sizeof(DmovSingleCommand));
    WriteBackInvalidateDataCacheRange((void*)pMemHandle->pCmdPtr,CPLE_SIZE);

    /* Submit the transfer request */
    dwResult = DalDmov_Transfer(hDmov,
                                &TransferRequest,
                                (unsigned int*)pMemHandle->pCmdPtr, 
                                &RequestStatus);
    if ( DAL_SUCCESS != dwResult )
    {
        AsciiPrint("Submit transfer                   [FAILED]\n");
        goto cleanup;
    }
    AsciiPrint("Submit transfer                  [SUCCESS]\n");

    if (DMOV_REQUEST_OK != RequestStatus.eState)
    {
        AsciiPrint ("Transfer not queued\n");
        DalDevice_Close(hDmov);
        return EFI_PROTOCOL_ERROR;
    }

    AsciiPrint("Starting the polling loop\n");

    do
    {
        dwResult = DalDmov_GetTransferResult(hDmov, 
                                             RequestStatus.dwToken,
                                             &RequestResult);

        if (DAL_SUCCESS != dwResult)
        {
            AsciiPrint("Get transfer result               [FAILED]\n");
            goto cleanup;
        }

        if (DMOV_TRANSFER_IN_PROGRESS == RequestResult.eState)
        {
            AsciiPrint("Transfer in progress....\n");
            continue;
        }
        else
        {
            AsciiPrint ("Transfer completed\n");
            break;
        }
    }while(1);


    if ( DMOV_TRANSFER_SUCCESSFULL != RequestResult.eState)
    {
        AsciiPrint ("Transfer was unsuccessfull\n");
        DalDevice_Close(hDmov);
        return EFI_PROTOCOL_ERROR;
    }
    AsciiPrint ("Transfer Successfull \n");

    /* check if the transfer actually happened */
    if ( FALSE == IsDataValid((char*)pMemHandle->pSourceBuffer,(char*)pMemHandle->pDestBuffer))
    {
        AsciiPrint("Validating transfered data        [FAILED] \n");
        DalDevice_Close(hDmov);
        return EFI_PROTOCOL_ERROR;
    }

    AsciiPrint("Validating transfered data       [SUCCESS] \n");

    AsciiPrint ("Closing Dmov device \n");

cleanup:
    DalDevice_Close(hDmov);
    return dwResult == DAL_SUCCESS? EFI_SUCCESS: EFI_PROTOCOL_ERROR;
}
