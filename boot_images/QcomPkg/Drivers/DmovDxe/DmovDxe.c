/** @file DmovDxe.c
   
  This file implements dmov EFI protocol interface

  Copyright (c) 2011,2014 Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/16/14   sho     Use DAL configuration to register devices   
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 05/25/11    rl     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Protocol/EFIDmov.h>
#include "DALDeviceId.h"
#include "DDIDmov.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/
#define EFI_DMOV_TRANS_REQUEST_EFI2DAL(pefi_xfer_request, pdal_xfer_request) \
    do\
    {\
       pdal_xfer_request->dwDevice = pefi_xfer_request->dwDevice; \
       pdal_xfer_request->dwChan = pefi_xfer_request->dwChan; \
       pdal_xfer_request->dwPriority = pefi_xfer_request->dwPriority;\
       pdal_xfer_request->pUserData = pefi_xfer_request->pUserData;\
       pdal_xfer_request->dwNumCmds = pefi_xfer_request->dwNumCmds; \
       pdal_xfer_request->CommandType = pefi_xfer_request->CommandType; \
       pdal_xfer_request->hEvent = 0;\
       pdal_xfer_request->TransferType = DMOV_TRANSFER_SYNCHRONOUS;\
       pdal_xfer_request->cmd_ptr = 0;\
       pdal_xfer_request->pCallbackFunction = 0;\
       pdal_xfer_request->options = 0;\
    }while(0)

/*=========================================================================
      Function Prototypes
==========================================================================*/

EFI_STATUS EFI_DmovOpen( 
                        IN EFI_DMOV_PROTOCOL  *This,
                        IN EFI_DMOV_DEVICE_ID deviceID);


EFI_STATUS EFI_DmovClose( 
                         IN EFI_DMOV_PROTOCOL  *This,
                         IN EFI_DMOV_DEVICE_ID deviceID);

EFI_STATUS EFI_DmovTransfer(
                            IN EFI_DMOV_PROTOCOL                *This,
                            IN EFI_DMOV_DEVICE_ID               deviceID,
                            IN CONST EFI_DMOV_TRANSFER_REQUEST  *pRequest, 
                            IN UINT32                           *pCommandList,
                            OUT EFI_DMOV_REQ_STATUS             *pRequestStatus);

EFI_STATUS EFI_DmovGetTransferResult(
                                     IN EFI_DMOV_PROTOCOL  *This, 
                                     IN EFI_DMOV_DEVICE_ID deviceID,          
                                     IN UINT32             dwToken,
                                     OUT EFI_DMOV_RESULT   *pResult);

EFI_STATUS EFI_DmovSetChanOperationMode(
                                        IN EFI_DMOV_PROTOCOL  *This, 
                                        IN EFI_DMOV_DEVICE_ID deviceID,
                                        IN UINT32             dwChan,
                                        IN UINT32             dwChanOpMode);

EFI_STATUS EFI_DmovFlushChannel(
                                IN EFI_DMOV_PROTOCOL  *This, 
                                IN EFI_DMOV_DEVICE_ID deviceID,
                                IN UINT32             dwChan, 
                                IN UINT32             bFlushQueues,
                                IN UINT32             bDiscardCurrentCmd);

EFI_STATUS EFI_DmovFlushDomain(
                               IN EFI_DMOV_PROTOCOL  *This, 
                               IN EFI_DMOV_DEVICE_ID deviceID,
                               IN UINT32             dwDomain );

EFI_STATUS EFI_DmovPrepareSGDescList(
                                     IN EFI_DMOV_PROTOCOL  *This, 
                                     IN EFI_DMOV_DEVICE_ID deviceID,
                                     IN UINT32             dwNumDescriptors, 
                                     OUT UINT32            *pDescList );

EFI_STATUS EFI_DmovPrepareCommand(
                                  IN EFI_DMOV_PROTOCOL    *This, 
                                  IN EFI_DMOV_DEVICE_ID   deviceID,
                                  IN EFI_DMOV_CMD_OPTIONS *pCommandOptions,
                                  OUT UINT32              *pCommand);

EFI_STATUS EFI_DmovPrepareCommandListPtr(
                                         IN EFI_DMOV_PROTOCOL  *This, 
                                         IN EFI_DMOV_DEVICE_ID deviceID,
                                         IN UINT32             *pCommandList, 
                                         IN UINT32             bLastPtr,
                                         OUT UINT32            *pCommandListPtr);


/*=========================================================================
      Data Declarations
==========================================================================*/
extern DALREG_DriverInfo DALDmov_DriverInfo;
static const DALREG_DriverInfo* DALDriverInfoArr[] = { &DALDmov_DriverInfo };
static DALSYSConfig DALSYSModConfig =
   {
      {0, NULL},              // string device info
      {1, DALDriverInfoArr}   // numeric driver info
   };

/* Dmov Public Protocol  */
static EFI_DMOV_PROTOCOL DmovProtocol =
{
   .Revision=EFI_DMOV_PROTOCOL_VERSION,            
   .Open=EFI_DmovOpen,                         
   .Close=EFI_DmovClose,                        
   .Transfer=EFI_DmovTransfer,
   .GetTransferResult=EFI_DmovGetTransferResult,
   .SetChanOperationMode=EFI_DmovSetChanOperationMode,
   .FlushChannel=EFI_DmovFlushChannel,
   .FlushDomain=EFI_DmovFlushDomain,
   .PrepareSGDescList=EFI_DmovPrepareSGDescList,
   .PrepareCommand=EFI_DmovPrepareCommand,
   .PrepareCommandListPtr=EFI_DmovPrepareCommandListPtr,
};

static DalDeviceHandle *hDmov[EFI_DMOV_DEVICE_ID_MAX] = { NULL};


/*=========================================================================
      Functions
==========================================================================*/

EFI_STATUS
EFI_DmovOpen(
             IN EFI_DMOV_PROTOCOL  *This,
             IN EFI_DMOV_DEVICE_ID deviceID)
{

  if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
      return EFI_INVALID_PARAMETER;

  
  if ( DAL_SUCCESS != DalDevice_Open(hDmov[deviceID],DAL_OPEN_SHARED))
  {
     DEBUG((EFI_D_ERROR, "DevID:%d Opening dmov device failed\n",deviceID));
     return EFI_PROTOCOL_ERROR;
  } 
  return EFI_SUCCESS;
}


EFI_STATUS
EFI_DmovClose(
              IN EFI_DMOV_PROTOCOL  *This,
              IN EFI_DMOV_DEVICE_ID deviceID)
{
  if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
      return EFI_INVALID_PARAMETER;

  if ( DAL_SUCCESS != DalDevice_Close(hDmov[deviceID]))
  {
     DEBUG((EFI_D_ERROR, "DevID:%d Closing dmov device failed\n",deviceID));
     return EFI_PROTOCOL_ERROR;
  } 
  return EFI_SUCCESS;
}


EFI_STATUS 
EFI_DmovTransfer(
                 IN EFI_DMOV_PROTOCOL                *This,
                 IN EFI_DMOV_DEVICE_ID               deviceID,
                 IN CONST EFI_DMOV_TRANSFER_REQUEST  *pRequest, 
                 OUT UINT32                          *pCommandList,
                 OUT EFI_DMOV_REQ_STATUS             *pRequestStatus)
{
  DmovTransferRequest TransferRequest;
  DmovTransferRequest* pTransferRequest=&TransferRequest;

  if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
      return EFI_INVALID_PARAMETER;

  /* Transform pRequest from EFI_DMOV_TRANSFER_REQUEST to DmovTransferRequest */

  EFI_DMOV_TRANS_REQUEST_EFI2DAL(pRequest,pTransferRequest);

  if ( DAL_SUCCESS != DalDmov_Transfer(hDmov[deviceID],pTransferRequest,pCommandList,pRequestStatus))
  {
     DEBUG((EFI_D_ERROR, "DevID:%d Submitting a transfer request failed\n",deviceID));
     return EFI_PROTOCOL_ERROR;
  } 
  return EFI_SUCCESS;
}


EFI_STATUS 
EFI_DmovGetTransferResult(
                          IN EFI_DMOV_PROTOCOL  *This, 
                          IN EFI_DMOV_DEVICE_ID deviceID,          
                          IN UINT32             dwToken,
                          OUT EFI_DMOV_RESULT   *pResult)
{
  if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
      return EFI_INVALID_PARAMETER;

  if ( DAL_SUCCESS != DalDmov_GetTransferResult(hDmov[deviceID],dwToken,pResult))
  {
     DEBUG((EFI_D_ERROR, "DevID:%d Failed to get transfer result\n",deviceID));
     return EFI_PROTOCOL_ERROR;
  } 
  return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovSetChanOperationMode(
                             IN EFI_DMOV_PROTOCOL  *This, 
                             IN EFI_DMOV_DEVICE_ID deviceID,
                             IN UINT32             dwChan,
                             IN UINT32             dwChanOpMode)
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_SetChanOperationMode(hDmov[deviceID],dwChan,dwChanOpMode))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Failed to set channel operation mode\n",deviceID));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovFlushChannel(
                     IN EFI_DMOV_PROTOCOL  *This, 
                     IN EFI_DMOV_DEVICE_ID deviceID,
                     IN UINT32             dwChan, 
                     IN UINT32             bFlushQueues,
                     IN UINT32             bDiscardCurrentCmd)
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_FlushChannel(hDmov[deviceID],dwChan,bFlushQueues,bDiscardCurrentCmd))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Failed to flush channel %d\n",deviceID,dwChan));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovFlushDomain(
                    IN EFI_DMOV_PROTOCOL  *This, 
                    IN EFI_DMOV_DEVICE_ID deviceID,
                    IN UINT32             dwDomain )
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_FlushDomain(hDmov[deviceID],dwDomain))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Failed to flush domain %d\n",deviceID,dwDomain));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovPrepareSGDescList(
                          IN EFI_DMOV_PROTOCOL  *This, 
                          IN EFI_DMOV_DEVICE_ID deviceID,
                          IN UINT32             dwNumDescriptors, 
                          OUT UINT32            *pDescList )
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_PrepareSGDescList(hDmov[deviceID],dwNumDescriptors,pDescList))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Error preparing SG descriptor list\n",deviceID));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovPrepareCommand(
                       IN EFI_DMOV_PROTOCOL    *This, 
                       IN EFI_DMOV_DEVICE_ID   deviceID,
                       IN EFI_DMOV_CMD_OPTIONS *pCommandOptions,
                       OUT UINT32              * pCommand)
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_PrepareCommand(hDmov[deviceID],pCommandOptions,pCommand))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Error preparing command\n",deviceID));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS 
EFI_DmovPrepareCommandListPtr(
                              IN EFI_DMOV_PROTOCOL  *This, 
                              IN EFI_DMOV_DEVICE_ID deviceID,
                              IN UINT32             *pCommandList, 
                              IN UINT32             bLastPtr,
                              OUT UINT32            *pCommandListPtr)
{
    if ( deviceID > EFI_DMOV_DEVICE_ID_MAX || hDmov[deviceID] == NULL)
        return EFI_INVALID_PARAMETER;

    if ( DAL_SUCCESS != DalDmov_PrepareCommandListPtr(hDmov[deviceID],pCommandList,bLastPtr,pCommandListPtr))
    {
        DEBUG((EFI_D_ERROR, "DevID:%d Error preparing command list pointer%d\n",deviceID));
        return EFI_PROTOCOL_ERROR;
    } 
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DmovDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE  handle = NULL;
  DALResult   eResult;
  EFI_STATUS  status;
  /* Init DALSys */
  DALSYS_InitMod(DALSYSModConfig);

  /* Attach to the DAL dmov drivers */
  if (FeaturePcdGet(PcdDmovSupportedDevicesADM0) )
  {
     if ( DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_DMOV_DEVICE_0, &hDmov[EFI_DMOV_DEVICE_ADM0]))
     {
         DEBUG((EFI_D_ERROR, "DevID:%d Error Attaching to ADM device\n",DALDEVICEID_DMOV_DEVICE_0));
         DALSYS_DeInitMod();
         return EFI_PROTOCOL_ERROR;
     }
     /* Turn on the ADM clocks. This is specifically done because ADM EA driver is not managing clocks*/
     /* we are going to open the device, which intern will vote for the adm clocks. A Open is required
     /* rather than a write to the clock register */
     if ( DAL_SUCCESS != DalDevice_Open(hDmov[EFI_DMOV_DEVICE_ADM0], DAL_OPEN_SHARED))
     {
         DEBUG((EFI_D_INFO, "DevID:%d Error opening ADM device\n",DALDEVICEID_DMOV_DEVICE_0));
         DALSYS_DeInitMod();
         return EFI_PROTOCOL_ERROR;
     }
  }

  if ( FeaturePcdGet(PcdDmovSupportedDevicesADM1) )
  {
      if (DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_DMOV_DEVICE_2, &hDmov[EFI_DMOV_DEVICE_ADM1]))
      {
          DEBUG((EFI_D_ERROR, "DevID:%d Error Attaching to ADM device\n",DALDEVICEID_DMOV_DEVICE_2));
          DALSYS_DeInitMod();
          return EFI_PROTOCOL_ERROR;
      }
  } 

  /* Publish the Dmov protocol */
  status = gBS->InstallMultipleProtocolInterfaces( &handle, &gEfiDmovProtocolGuid,&DmovProtocol, NULL);
  if ( EFI_SUCCESS != status )
  {
      DEBUG((EFI_D_ERROR, "Error installing DMOV protocol handle\n"));
      DALSYS_DeInitMod();
      return EFI_PROTOCOL_ERROR;
  }

  return EFI_SUCCESS;
}
