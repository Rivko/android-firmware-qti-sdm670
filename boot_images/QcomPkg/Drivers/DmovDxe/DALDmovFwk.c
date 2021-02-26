/*==============================================================================

FILE:      DalDmovFwk.c

DESCRIPTION: This file implements a Dmov DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
10/18/10   an      moved client registration to Dmov_Open
08/12/10   py      Add support for using the same channel in different modes in parallel 
03/03/10   an      renamed drvCtxt to dmovDrvCtxt to ease debugging
03/27/09   MK      Added clock management 
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIDmov.h"
#include "DALDmov.h"
#include "string.h" //for memset warning


DALResult 
Dmov_DalDmov_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Dmov_DalDmov_AddRef(DalDmovHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Dmov_DalDmov_Detach(DalDeviceHandle* h)
{
    uint32 dwref = ((DALClientCtxt *)(h->pClientCtxt))->dwRefs;
    if(1 == dwref)	{
       /* Deregister the client from the device
          - We do this before DALFW_Release as the API would
          set the devCtxt in the ClientCtxt to NULL */
       Dmov_DeregisterClient(h->pClientCtxt);
    }
     dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	 if(0 == dwref)	{
		//Release the client ctxt;
        DALSYS_Free(h->pClientCtxt);
	}
	return dwref;
}

static DALResult 
Dmov_DalDmov_Init(DalDeviceHandle *h)
{  
   DmovClientCtxt *pClientCtxt = (DmovClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pDmovDevCtxt->DevId,
	                           pClientCtxt->pDmovDevCtxt->hProp);
   return Dmov_DeviceInit(h->pClientCtxt);
}

static DALResult 
Dmov_DalDmov_DeInit(DalDeviceHandle *h)
{
   return Dmov_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Dmov_DalDmov_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Dmov_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Dmov_DalDmov_Open(DalDeviceHandle* h, uint32 mode)
{
    return Dmov_Open(h->pClientCtxt,mode);
}

static DALResult 
Dmov_DalDmov_Close(DalDeviceHandle* h)
{
    return Dmov_Close(h->pClientCtxt);
}

static DALResult 
Dmov_DalDmov_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALDMOV_INTERFACE_VERSION;
   return Dmov_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
Dmov_DalDmov_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalDmov Interface. 
------------------------------------------------------------------------------*/

static DALResult 
Dmov_DalDmov_Transfer( DalDeviceHandle * h,  const DmovTransferRequest*  pTransferRequest, uint32*  pCommandList, DmovReqStatus*  pRequestStatus) 
{
	return Dmov_Transfer(((DalDmovHandle *)h)->pClientCtxt, pTransferRequest, pCommandList, pRequestStatus);
}
static DALResult 
Dmov_DalDmov_GetTransferResult( DalDeviceHandle * h, uint32  dwToken,  DmovResult*  pResult) 
{
	return Dmov_GetTransferResult(((DalDmovHandle *)h)->pClientCtxt, dwToken, pResult);
}
static DALResult 
Dmov_DalDmov_SetChanOperationMode( DalDeviceHandle * h, uint32  dwChan, uint32  dwChanOpMode) 
{
	return Dmov_SetChanOperationMode(((DalDmovHandle *)h)->pClientCtxt, dwChan, dwChanOpMode);
}
static DALResult 
Dmov_DalDmov_FlushChannel( DalDeviceHandle * h, uint32  dwChan, uint32  bFlushQueues, uint32  bDiscardCurrentCmd) 
{
	return Dmov_FlushChannel(((DalDmovHandle *)h)->pClientCtxt, dwChan, bFlushQueues, bDiscardCurrentCmd);
}
static DALResult 
Dmov_DalDmov_FlushDomain( DalDeviceHandle * h, uint32  dwDomain) 
{
	return Dmov_FlushDomain(((DalDmovHandle *)h)->pClientCtxt, dwDomain);
}
static DALResult 
Dmov_DalDmov_PrepareSGDescList( DalDeviceHandle * h, uint32  dwNumDescriptors, uint32 *  pDescList) 
{
	return Dmov_PrepareSGDescList(((DalDmovHandle *)h)->pClientCtxt, dwNumDescriptors, pDescList);
}
static DALResult 
Dmov_DalDmov_PrepareCommand( DalDeviceHandle * h, DmovCommandOptions * pCommandOptions, uint32*  pCommand) 
{
	return Dmov_PrepareCommand(((DalDmovHandle *)h)->pClientCtxt, pCommandOptions, pCommand);
}
static DALResult 
Dmov_DalDmov_PrepareCommandListPtr( DalDeviceHandle * h, uint32 *  pCommandList, uint32  bLastPtr, uint32 *  pCommandListPtr) 
{
	return Dmov_PrepareCommandListPtr(((DalDmovHandle *)h)->pClientCtxt, pCommandList, bLastPtr, pCommandListPtr);
}

static void 
Dmov_InitInterface(DmovClientCtxt* pclientCtxt)
{
    static const DalDmov vtbl = {
       {
          Dmov_DalDmov_Attach,
          Dmov_DalDmov_Detach,
          Dmov_DalDmov_Init,
          Dmov_DalDmov_DeInit,
          Dmov_DalDmov_Open,
          Dmov_DalDmov_Close,
          Dmov_DalDmov_Info,
          Dmov_DalDmov_PowerEvent,
          Dmov_DalDmov_SysRequest
        } ,
		Dmov_DalDmov_Transfer,
		Dmov_DalDmov_GetTransferResult,
		Dmov_DalDmov_SetChanOperationMode,
		Dmov_DalDmov_FlushChannel,
		Dmov_DalDmov_FlushDomain,
		Dmov_DalDmov_PrepareSGDescList,
		Dmov_DalDmov_PrepareCommand,
		Dmov_DalDmov_PrepareCommandListPtr
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalDmovHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalDmovHandle.pVtbl  = &vtbl;
    pclientCtxt->DalDmovHandle.pClientCtxt = pclientCtxt;

}

DALResult 
Dmov_DalDmov_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;

    static DmovDrvCtxt dmovDrvCtxt = {{0}};

    DmovClientCtxt *pclientCtxt =  NULL;

    if (dmovDrvCtxt.DmovDALVtbl.Dmov_DriverInit == NULL)
    {
        dmovDrvCtxt.DmovDALVtbl.Dmov_DriverInit = Dmov_DriverInit;
        dmovDrvCtxt.DmovDALVtbl.Dmov_DriverDeInit = Dmov_DriverDeInit;
        dmovDrvCtxt.dwNumDev = sizeof(dmovDrvCtxt.DmovDevCtxt)/sizeof(DmovDevCtxt);
        dmovDrvCtxt.dwSizeDevCtxt = sizeof(DmovDevCtxt);
    }

    nErr = DALSYS_Malloc(sizeof(DmovClientCtxt),(void **)&pclientCtxt);

    if (DAL_SUCCESS == nErr) 
    {
        DALSYS_memset(pclientCtxt,0,sizeof(DmovClientCtxt));

        *phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&dmovDrvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            Dmov_InitInterface(pclientCtxt);
            Dmov_DalDmov_AddRef(&(pclientCtxt->DalDmovHandle));
            pclientCtxt->nHandleID = DMOV_CLIENT_NOT_REGISTERED;
            *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalDmovHandle);
            if(NULL != pszArg)
            {
              pclientCtxt->pszClientName = pszArg;
            }
        }
     }
 
     return nErr;
}

