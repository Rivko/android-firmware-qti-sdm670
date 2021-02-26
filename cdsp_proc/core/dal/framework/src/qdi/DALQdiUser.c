/*==============================================================================

                  DAL QDI USER Module

GENERAL DESCRIPTION
    This file provides an implementation of QDI based DAL remoting.
    It handles DAL FWK calls in USERPD.

EXTERNALIZED FUNCTIONS
    DALRemote_GetInterfaceVtbl()
 
  Copyright (c) 2008-2012,2016 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.

$Header: //components/rel/core.qdsp6/2.1.c4/dal/framework/src/qdi/DALQdiUser.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
01/12/18   pg      Modified earlier implementation with direct use of qurt api instead of qdi invocation in _DoRemoteCreateProxy and _DoRemoteReleaseProxy APIs
06/08/12   aa      Updated for QDI2.0.
05/10/2012 aa      Initial release for ADSP2.0 MPQuRT QURTOS
==============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "DALSys.h"
#include "DALSysCmn.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALQdiDefs.h"
#include "qurt_qdi.h"
#include "servreg_locator.h"
#include <stdlib.h>
#include <stdio.h>

/*=============================================================================
                 MACROS and DEFINITIONS
=============================================================================*/

/* QDI handle initialized at DALSYS_Init */
static int gRemoteDALInitHandle;

/*Global flag to indiciate RDAL initialization */
static int bgRDALConfigDone = 0;

typedef struct DALDeviceFWDCtx DALDeviceFWDCtx;
struct DALDeviceFWDCtx
{
    /* order of members can't be changed, add new element down */
    DalDeviceHandle DalRemoteHandle;
    int remoteQDIhandle;
};

extern void *qurt_qdi_get_dal_fwd_vtable(void); 

/*
 * _DoRemoteDetach handles Detach Request from DAL clients 
 * for remote Drivers.
 */
DALResult 
_DoRemoteDetach(DalDeviceHandle * hDalDevice)
{
    DALDeviceFWDCtx *pdalfwdobj = (DALDeviceFWDCtx *)hDalDevice;
    int remoteQDIRet = DAL_ERROR;

    /* if invalid handle */
    if (pdalfwdobj->remoteQDIhandle < 0)
        return remoteQDIRet;

    remoteQDIRet = qurt_qdi_close(pdalfwdobj->remoteQDIhandle);
    free(pdalfwdobj);    
    return remoteQDIRet;
}

/*
 * _PopulateDeviceObject, stores qdi handle and install a vtable
 * in a local device object . It also mark the handle to be QDI based.
 */
static void
_PopulateDeviceObject(int remoteQDIhandle, 
                      DALDeviceFWDCtx *pdalfwdobj,
                      DalDeviceHandle **phdalDevice)
{
    pdalfwdobj->remoteQDIhandle = remoteQDIhandle; //qdi handle
    pdalfwdobj->DalRemoteHandle.dwDalHandleId = DALDEVICE_QDI_HANDLE_ID;
    pdalfwdobj->DalRemoteHandle.pVtbl  = qurt_qdi_get_dal_fwd_vtable();
    pdalfwdobj->DalRemoteHandle.pClientCtxt = &pdalfwdobj->remoteQDIhandle;
    *phdalDevice = (DalDeviceHandle *)&(pdalfwdobj->DalRemoteHandle);
}

/*
 * _DoRemoteDeviceAttach handles attach request from DAL clients 
 * for remote DAL Drivers.
 */
DALResult 
_DoRemoteDeviceAttach(DALDEVICEID devId, DalDeviceHandle **phdalDevice)
{
    DALDeviceFWDCtx *pdalfwdobj = NULL;
    int remoteQDIRet = DAL_ERROR;

    if (gRemoteDALInitHandle <= 0)
    {
        DALSYS_LOG_ERROR_EVENT("remote DAL handle is NULL");
        return DAL_ERROR;
    }

    pdalfwdobj = (DALDeviceFWDCtx *)malloc(sizeof(DALDeviceFWDCtx));
    if (NULL == pdalfwdobj)
    {
        DALSYS_LOG_ERROR_EVENT("memory exhaustion");
        return DAL_ERROR;
    }
    memset(pdalfwdobj, 0, sizeof(*pdalfwdobj));

    //Do Remote Attach, return qdi handle on success
    remoteQDIRet = qurt_qdi_handle_invoke(gRemoteDALInitHandle, 
                                          DALREMOTE_MSG_ATTACH,
                                          devId);
    if (remoteQDIRet < 0)
    {
        DALSYS_LOG_ERROR_EVENT("qurt_qdi_handle_invoke failed "
              "remoteQdiRet:0x%x", remoteQDIRet);
        free(pdalfwdobj);
        return remoteQDIRet;
    }

    _PopulateDeviceObject(remoteQDIRet, pdalfwdobj, phdalDevice );
    return DAL_SUCCESS;
}

/*
 * _DoRemoteDeviceAttachEx handles attach request from DAL clients 
 * for remote DAL Drivers along with version check.
 */

DALResult 
_DoRemoteDeviceAttachEx(const char *pszArg, DALDEVICEID devId, 
                        DALInterfaceVersion ClientVersion,
                        DalDeviceHandle **phdalDevice)
{
    DALDeviceFWDCtx *pdalfwdobj = NULL;
    int remoteQDIRet = DAL_ERROR;

    if (gRemoteDALInitHandle <= 0)
    {
        DALSYS_LOG_ERROR_EVENT("remote DAL handle is NULL");
        return DAL_ERROR;
    }

    pdalfwdobj = (DALDeviceFWDCtx *)malloc(sizeof(DALDeviceFWDCtx));
    if (NULL == pdalfwdobj)
    {
        DALSYS_LOG_ERROR_EVENT("memory exhaustion");
        return DAL_ERROR;
    }
    memset(pdalfwdobj, 0, sizeof(*pdalfwdobj));

    //Do Remote Attach, return qdi handle on success
    remoteQDIRet =  qurt_qdi_handle_invoke(gRemoteDALInitHandle, 
                                           DALREMOTE_MSG_ATTACH_EX,
                                           pszArg, devId,
                                           ClientVersion);
    if (remoteQDIRet < 0)
    {
        DALSYS_LOG_ERROR_EVENT("qurt_qdi_handle_invoke failed "
              "remoteQdiRet:0x%x", remoteQDIRet);
         free(pdalfwdobj);
        return remoteQDIRet;
    }

    _PopulateDeviceObject(remoteQDIRet, pdalfwdobj, phdalDevice );
    return DAL_SUCCESS;
}

/*
 * _DoRemoteStringDeviceAttach handles attach request from DAL clients 
 * for remote string named DAL Drivers.
 */

DALResult 
_DoRemoteStringDeviceAttach(const char *pszDevName, DalDeviceHandle **phdalDevice)
{
    DALDeviceFWDCtx *pdalfwdobj = NULL;
    int remoteQDIRet = DAL_ERROR;

    if (gRemoteDALInitHandle <= 0)
    {
        DALSYS_LOG_ERROR_EVENT("remote DAL handle is NULL");
        return DAL_ERROR;
    }

    pdalfwdobj = (DALDeviceFWDCtx *)malloc(sizeof(DALDeviceFWDCtx));
    if (NULL == pdalfwdobj)
    {
        DALSYS_LOG_ERROR_EVENT("memory exhaustion");
        return DAL_ERROR;
    }
    memset(pdalfwdobj, 0, sizeof(*pdalfwdobj));

    //Do Remote Attach, return qdi handle on success
    remoteQDIRet = qurt_qdi_handle_invoke(gRemoteDALInitHandle, 
                                          DALREMOTE_MSG_STRING_ATTACH,
                                          pszDevName);
    if (remoteQDIRet < 0)
    {
        DALSYS_LOG_ERROR_EVENT("qurt_qdi_handle_invoke failed "
              "remoteQdiRet:0x%x", remoteQDIRet);
        free(pdalfwdobj);
        return remoteQDIRet;
    }

    _PopulateDeviceObject(remoteQDIRet, pdalfwdobj, phdalDevice );
    return DAL_SUCCESS;
}

/*
 * _DoRemoteStringDeviceAttach handles attach request from DAL clients 
 * for remote string named DAL Drivers along with version check.
 */
DALResult 
_DoRemoteStringDeviceAttachEx(const char *pszArg, const char *pszDevName, 
                              DALInterfaceVersion ClientVersion,
                              DalDeviceHandle **phdalDevice)
{
    DALDeviceFWDCtx *pdalfwdobj = NULL;
    int remoteQDIRet = DAL_ERROR;

    if (gRemoteDALInitHandle <= 0)
    {
        DALSYS_LOG_ERROR_EVENT("remote DAL handle is NULL");
        return DAL_ERROR;
    }

    pdalfwdobj = (DALDeviceFWDCtx *)malloc(sizeof(DALDeviceFWDCtx));
    if (NULL == pdalfwdobj)
    {
        DALSYS_LOG_ERROR_EVENT("memory exhaustion");
        return DAL_ERROR;
    }
    memset(pdalfwdobj, 0, sizeof(*pdalfwdobj));

    //Do Remote Attach, return qdi handle on success
    remoteQDIRet =  qurt_qdi_handle_invoke(gRemoteDALInitHandle, 
                                           DALREMOTE_MSG_STRING_ATTACH_EX,
                                           pszArg, pszDevName,
                                           ClientVersion);
    if (remoteQDIRet < 0)
    {
        DALSYS_LOG_ERROR_EVENT("qurt_qdi_handle_invoke failed "
              "remoteQdiRet:0x%x", remoteQDIRet);
        free(pdalfwdobj);
        return remoteQDIRet;
    }

    _PopulateDeviceObject(remoteQDIRet, pdalfwdobj, phdalDevice );
    return DAL_SUCCESS;
}

/*
* _DoRemoteAddProxy makes a qdi call into root implmentation of proxy service to add
* a proxy for given dal device handle 'hdalDevice'.
* Modified earlier implementation with direct use of qurt api instead of qdi invocation
*/
DALResult 
_DoRemoteCreateProxy(DalDeviceHandle *hdalDevice)
{
    if(qurt_client_proxy_ref_add(((DALDeviceFWDCtx *) hdalDevice)-> remoteQDIhandle) == QURT_EOK) return DAL_SUCCESS;
    else return DAL_ERROR;
}

/*
* _DoRemoteReleaseProxy makes a qdi call into root implmentation of proxy service to remove
* proxy for given dal device handle 'hdalDevice'.
* Modified earlier implementation with direct invocation of qurt api instead of qdi invocation
*/
DALResult 
_DoRemoteReleaseProxy(DalDeviceHandle *hdalDevice)
{
    if(qurt_client_proxy_ref_remove(((DALDeviceFWDCtx *) hdalDevice)-> remoteQDIhandle) == QURT_EOK) return DAL_SUCCESS;
    else return DAL_ERROR;
}


/*
 * Init function in USERPD. It opens a dal dispatcher driver in QURTOS 
 * and returns a qdi handle for it. Invoked in DALSYS_InitMod, 
 * exposed via function table. 
 */
DALResult DALRemoteInit(void)
{
    // create a  non-negative one time qdi handle.
    if (!bgRDALConfigDone)
    {
        gRemoteDALInitHandle = qurt_qdi_open("qdi_dal_dispatcher");
        if (gRemoteDALInitHandle > 0)
        {
            bgRDALConfigDone = 1;
            return DAL_SUCCESS;
        }
        else
        {
            DALSYS_LOG_ERROR_EVENT("failed - gRemoteDALInitHandle=0x%x",
                  gRemoteDALInitHandle);
            return DAL_ERROR;
        }
   }
   return DAL_SUCCESS;
}

/*
 * DeInit function in USERPD. Close Dal dispatcher driver in QURTOS. 
 * Invoked in DALSYS_DeInitMod, exposed via function table. 
 */
DALResult DALRemoteDeinit(void)
{
    int remoteQDIRet;
    
    // release the qdi handle, return 0 when success.
    if (bgRDALConfigDone)
    {
        remoteQDIRet = qurt_qdi_close(gRemoteDALInitHandle);
        if (remoteQDIRet == 0)
        {
            gRemoteDALInitHandle = -1;
            bgRDALConfigDone = 0;
            return DAL_SUCCESS;
        }
        else
        {
            return DAL_ERROR;
        }
    }
    return DAL_SUCCESS;
}
