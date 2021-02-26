/*==============================================================================

FILE:      Dal<driver>Fwk.c

DESCRIPTION: This file implements a <driver> DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "<interface_header_file>"
#include "DAL<driver>.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
<driver>_<interface>_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
<driver>_<interface>_AddRef(<interface>Handle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
<driver>_<interface>_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	<non_static_client_context_req> if(0 == dwref)	{
	<non_static_client_context_req>	//Release the client ctxt;
    <non_static_client_context_req>    DALSYS_Free(h->pClientCtxt);
	<non_static_client_context_req>}
	return dwref;
}

static DALResult 
<driver>_<interface>_Init(DalDeviceHandle *h)
{  
   <driver>ClientCtxt *pClientCtxt = (<driver>ClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->p<driver>DevCtxt->DevId,
	                           pClientCtxt->p<driver>DevCtxt->hProp);
   return <driver>_DeviceInit(h->pClientCtxt);
}

static DALResult 
<driver>_<interface>_DeInit(DalDeviceHandle *h)
{
   return <driver>_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
<driver>_<interface>_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return <driver>_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
<driver>_<interface>_Open(DalDeviceHandle* h, uint32 mode)
{
    return <driver>_Open(h->pClientCtxt,mode);
}

static DALResult 
<driver>_<interface>_Close(DalDeviceHandle* h)
{
    return <driver>_Close(h->pClientCtxt);
}

static DALResult 
<driver>_<interface>_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = <dal_interface_version>;
   return <driver>_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
<driver>_<interface>_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in <interface> Interface. 
------------------------------------------------------------------------------*/

<cs_user_interface_function_definition>

static void 
<driver>_InitInterface(<driver>ClientCtxt* pclientCtxt)
{
    static const <interface> vtbl = {
       {
          <driver>_<interface>_Attach,
          <driver>_<interface>_Detach,
          <driver>_<interface>_Init,
          <driver>_<interface>_DeInit,
          <driver>_<interface>_Open,
          <driver>_<interface>_Close,
          <driver>_<interface>_Info,
          <driver>_<interface>_PowerEvent,
          <driver>_<interface>_SysRequest
        } <user_interface_function_vtbl_entry>
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt-><interface>Handle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt-><interface>Handle.pVtbl  = &vtbl;
    pclientCtxt-><interface>Handle.pClientCtxt = pclientCtxt;

}

DALResult 
<driver>_<interface>_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static <driver>DrvCtxt drvCtxt = {{<driver>_DriverInit,
									   <driver>_DriverDeInit
									  },<driver_num_devices>,
									  sizeof(<driver>DevCtxt)};
    <static_client_context_declaration> <driver>ClientCtxt clientCtxt;

    <driver>ClientCtxt *pclientCtxt = <client_context_declaration>;

    <non_static_client_context_req>nErr = DALSYS_Malloc(sizeof(<driver>ClientCtxt),(void **)&pclientCtxt);
    <non_static_client_context_req>DALSYS_memset(pclientCtxt,0,sizeof(<driver>ClientCtxt));

	*phDalDevice = NULL;

    <non_static_client_context_req> if (DAL_SUCCESS == nErr) {
        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            <driver>_InitInterface(pclientCtxt);
			<driver>_<interface>_AddRef(&(pclientCtxt-><interface>Handle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt-><interface>Handle);
        }
    <non_static_client_context_req> } 
    return nErr;
}

