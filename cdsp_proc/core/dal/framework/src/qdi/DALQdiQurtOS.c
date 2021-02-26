/*==============================================================================

                  DAL QDI QURTOS Module

GENERAL DESCRIPTION
    This file provides an implementation of QDI based DAL remoting.
    It handles DAL FWK calls from USERPD trapped to QURTOS.

EXTERNALIZED FUNCTIONS
 
  Copyright (c) 2008-2012,2016 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.

$Header: //components/rel/core.qdsp6/2.1.c4/dal/framework/src/qdi/DALQdiQurtOS.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
01/12/18   pg      Removed DALREMOTE_MSG_CREATE_PROXY and corresponding implementation of DALRemote_CreateProxy api (same way for remove/release)
06/08/12   aa      Updated for QDI2.0.
05/10/2012 aa      Initial release for ADSP2.0 MPQuRT QURTOS
==============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "DALQdiDefs.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi_proxy.h"
#include "qurt.h"                           
#include "servreg_monitor.h"
#include <stdlib.h>
#include <stdio.h>

/*=============================================================================
                 MACROS and DEFINITIONS
=============================================================================*/

typedef struct DALRemoteGenericHandle DALRemoteGenericHandle;
typedef struct DALRemoteDeviceHandle DALRemoteDeviceHandle;

extern int dal_fwd_invoker(QDI_INVOKE_ARGS);
extern DALResult DAL_DeviceAttachLocalEx(const char *pszArg,DALDEVICEID DeviceId,
                                         DalDeviceHandle **phDevice, int ClientHandle);

int DALRemote_ServiceRequest(QDI_INVOKE_ARGS);

struct DALRemoteGenericHandle
{
    qurt_qdi_obj_t   qdiobj;
};

struct  DALRemoteDeviceHandle
{
    qurt_qdi_obj_t   qdiobj;
    DalDeviceHandle *pdalRemoteHandle;
    int qdiHandle;
};

/*
 * This function performs validation of user access to 
 * uint32 devices based on access list set by DALFWK in QURTOS. 
 */
static DALResult
DeviceId_ValidatePermission(DALDEVICEID devId)
{
    uint32 propHandle[2];
    DALSYSPropertyVar propValue;

    if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(devId, propHandle))
        return DAL_ERROR;

    if (DAL_SUCCESS != DALSYS_GetPropertyValue(propHandle,"IsRemotable", 0, &propValue))
        return DAL_ERROR_DEVICE_ACCESS_DENIED;

    /* if the uint32 based device is not remotable*/
    if (!propValue.Val.dwVal)
        return DAL_ERROR_DEVICE_ACCESS_DENIED;

    return DAL_SUCCESS;
}

/*
 * This function performs validation of user access to 
 * string named devices based on "IsRemotable" property defined
 * for the device in the config file. 
 */
static DALResult
StringDeviceId_ValidatePermission(const char *pszDevName)
{
    uint32 propHandle[2];
    DALSYSPropertyVar propValue;

    if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(pszDevName, propHandle))
        return DAL_ERROR;

    if (DAL_SUCCESS != DALSYS_GetPropertyValue(propHandle,"IsRemotable", 0, &propValue))
        return DAL_ERROR_DEVICE_ACCESS_DENIED;
   
   /* if the string based device is not remotable*/
    if (!propValue.Val.dwVal)
        return DAL_ERROR_DEVICE_ACCESS_DENIED;
   
   return DAL_SUCCESS;
}

/*
 * This function performs the cleanup operation by releasing dal 
 * handle and qdi based remote device object. The function is set
 * within the device object which framework invokes qdi handle 
 * reference count falls to 0.
 */
void DALRemote_DeviceDetach( qurt_qdi_obj_t *ptr)
{
    DALRemoteDeviceHandle *phDALRemoteDeviceObj = (DALRemoteDeviceHandle *)ptr;

    DAL_DeviceDetach(phDALRemoteDeviceObj->pdalRemoteHandle);
    free(phDALRemoteDeviceObj);
}

/*
 * _PopulateDeviceObject, add invoker  and release info for qdi fwk,
 * install a secure vtable in a remote device object, and abstract 
 * the informations in qdi handle.
 */
static int
_PopulateDeviceObject(int client_handle, 
                      DALRemoteDeviceHandle *phDALRemoteDeviceObj,
                      DalDeviceHandle *phDalDevice)
{   
    int qdiHandle = -1;

    phDALRemoteDeviceObj->qdiobj.invoke = dal_fwd_invoker;
    phDALRemoteDeviceObj->qdiobj.refcnt  = QDI_REFCNT_INIT;
    phDALRemoteDeviceObj->qdiobj.release = DALRemote_DeviceDetach;
    phDALRemoteDeviceObj->pdalRemoteHandle = phDalDevice; 
    qdiHandle = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t *)phDALRemoteDeviceObj); 
    phDALRemoteDeviceObj->qdiHandle = qdiHandle;
    return qdiHandle;
}

/*
 * This function performs an attach on string named devices in 
 * QURTOS and return a qdi handle to user process. 
 */

static int 
DALRemote_StringDeviceAttachEx(int client_handle, 
                               const char * pszArg, 
                               const char *pszDevName, 
                               DALInterfaceVersion ClientVersion){

    DALRemoteDeviceHandle *phDALRemoteDeviceObj = NULL;
    int qdiHandle = -1;
    DalDeviceHandle *phDalDevice = NULL;
    DALResult dalErrCode = DAL_ERROR; 
    
    dalErrCode = StringDeviceId_ValidatePermission(pszDevName);
    if (DAL_SUCCESS != dalErrCode)
        return dalErrCode;

    phDALRemoteDeviceObj = (DALRemoteDeviceHandle *) malloc(sizeof(DALRemoteDeviceHandle));
    if (NULL == phDALRemoteDeviceObj)
        return DAL_ERROR;

    dalErrCode =  DAL_StringDeviceAttachEx(pszArg, pszDevName, ClientVersion, &phDalDevice);
    if ( DAL_SUCCESS!= dalErrCode)
    {
        free((void *)phDALRemoteDeviceObj);
        return DAL_ERROR;
    }

    qdiHandle = _PopulateDeviceObject(client_handle, phDALRemoteDeviceObj, phDalDevice);
    if (qdiHandle < 0)
    {
        free((void *)phDALRemoteDeviceObj);
        DAL_DeviceDetach(phDalDevice);
        return DAL_ERROR;
    }
    return qdiHandle;    
}

/*
 * This function performs an attach on string named devices in 
 * QURTOS and return a qdi handle to user process. 
 */

static int 
DALRemote_StringDeviceAttach(int client_handle, const char *pszDevName){

    DALRemoteDeviceHandle *phDALRemoteDeviceObj = NULL;
    int qdiHandle = -1;
    DalDeviceHandle *phDalDevice = NULL;
    DALResult dalErrCode = DAL_ERROR; 
    
    dalErrCode = StringDeviceId_ValidatePermission(pszDevName);
    if (DAL_SUCCESS != dalErrCode)
        return dalErrCode;
        
    phDALRemoteDeviceObj = (DALRemoteDeviceHandle *) malloc(sizeof(DALRemoteDeviceHandle));
    if (NULL == phDALRemoteDeviceObj)
        return DAL_ERROR;

    dalErrCode =  DAL_StringDeviceAttach(pszDevName, &phDalDevice);
    if ( DAL_SUCCESS!= dalErrCode)
    {
        free((void *)phDALRemoteDeviceObj);
        return DAL_ERROR;
    }

    qdiHandle = _PopulateDeviceObject(client_handle, phDALRemoteDeviceObj, phDalDevice);
    if (qdiHandle < 0)
    {
        free((void *)phDALRemoteDeviceObj);
        DAL_DeviceDetach(phDalDevice);
        return DAL_ERROR;
    }
    return qdiHandle;    
}

/*
 * This function performs an attach on uint based devices in 
 * QURTOS and return a qdi handle to user process. 
 */

static int 
DALRemote_DeviceAttachEx(int client_handle, 
                         const char * pszArg, 
                         DALDEVICEID devId, 
                         DALInterfaceVersion ClientVersion){

    DALRemoteDeviceHandle *phDALRemoteDeviceObj = NULL;
    int qdiHandle = -1;
    DalDeviceHandle *phDalDevice = NULL;
    DALResult dalErrCode = DAL_ERROR;

    dalErrCode = DeviceId_ValidatePermission(devId);
    if (DAL_SUCCESS != dalErrCode)
        return dalErrCode;

    phDALRemoteDeviceObj = (DALRemoteDeviceHandle *) malloc(sizeof(DALRemoteDeviceHandle));
    if (NULL == phDALRemoteDeviceObj)
        return DAL_ERROR;

    dalErrCode =  DAL_DeviceAttachEx(pszArg, devId, ClientVersion, &phDalDevice);
    if ( DAL_SUCCESS!= dalErrCode)
    {
        free((void *)phDALRemoteDeviceObj);
        return DAL_ERROR;
    }

    qdiHandle = _PopulateDeviceObject(client_handle, phDALRemoteDeviceObj, phDalDevice);
    if (qdiHandle < 0)
    {
        free((void *)phDALRemoteDeviceObj);
        DAL_DeviceDetach(phDalDevice);
        return DAL_ERROR;
    }
    return qdiHandle;    
}

/*
 * This function performs an attach on uint based devices in 
 * QURTOS and return a qdi handle to user process. 
 */

static int 
DALRemote_DeviceAttach(int client_handle, DALDEVICEID devId ){

    DALRemoteDeviceHandle *phDALRemoteDeviceObj = NULL;
    int qdiHandle = -1;
    DalDeviceHandle *phDalDevice = NULL;
    DALResult dalErrCode = DAL_ERROR;

    dalErrCode = DeviceId_ValidatePermission(devId);
    if (DAL_SUCCESS != dalErrCode)
        return dalErrCode;

    phDALRemoteDeviceObj = (DALRemoteDeviceHandle *) malloc(sizeof(DALRemoteDeviceHandle));
    if (NULL == phDALRemoteDeviceObj)
        return DAL_ERROR;

    dalErrCode =  DAL_DeviceAttachLocalEx(NULL, devId, &phDalDevice, client_handle);
    if ( DAL_SUCCESS!= dalErrCode)
    {
        free((void *)phDALRemoteDeviceObj);
        return DAL_ERROR;
    }

    qdiHandle = _PopulateDeviceObject(client_handle, phDALRemoteDeviceObj, phDalDevice);
    if (qdiHandle < 0)
    {
        free((void *)phDALRemoteDeviceObj);
        DAL_DeviceDetach(phDalDevice);
        return DAL_ERROR;
    }
    return qdiHandle;    
}

/* DALFWK Service Handler Object */
static const DALRemoteGenericHandle gRemoteDALQDIObj = {{DALRemote_ServiceRequest, 
        QDI_REFCNT_PERM, 0}};

/*
 * This a entry function for a device attach on a opener qdi handle. 
 * Handles all DALFWK from USERPD except Detach. 
 * Return qdi handle on success.
 */

int
DALRemote_ServiceRequest(int client_handle, 
                         qurt_qdi_obj_t *ptr, int method, 
                         qurt_qdi_arg_t a1, 
                         qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3,
                         qurt_qdi_arg_t a4,
                         qurt_qdi_arg_t a5,
                         qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7,
                         qurt_qdi_arg_t a8,
                         qurt_qdi_arg_t a9 )
{

    switch (method)
    {
    
    case QDI_OPEN:
        // return attacher handle.
        return qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t *)&gRemoteDALQDIObj); 

    case DALREMOTE_MSG_ATTACH :
        return DALRemote_DeviceAttach(client_handle, a1.num);                     

    case DALREMOTE_MSG_ATTACH_EX :
        return DALRemote_DeviceAttachEx(client_handle, a1.ptr, a2.num, a3.num );   

    case DALREMOTE_MSG_STRING_ATTACH :
        return DALRemote_StringDeviceAttach(client_handle, a1.ptr);               

    case DALREMOTE_MSG_STRING_ATTACH_EX :
        return DALRemote_StringDeviceAttachEx(client_handle, a1.ptr, a2.ptr, a3.num );

    default:
        return qurt_qdi_method_default(client_handle, ptr, 
                                       method, a1, a2, a3, 
                                       a4, a5, a6, a7, a8, 
                                       a9 );
    }
    // nop , make compiler and klockwork happy.
    return DAL_ERROR;  
}

/*
 * Init function in QURTOS. Register DALFWK in QURTOS to QDIFWK. 
 * Invoked in DALSYS_InitMod, exposed via function table. 
 */
DALResult DALRemoteInit(void)
{
    static uint32 bConfigDone = 1;

    // register device in QurtOS, return 0 when success.
    if (bConfigDone)
    {
        bConfigDone = qurt_qdi_devname_register("qdi_dal_dispatcher", 
                                                (void*)&gRemoteDALQDIObj);
    }

    if (!bConfigDone)
    {
       return DAL_SUCCESS;
    }
    else
    {
       return DAL_ERROR;
    }
}

/*
 * DeInit function in QURTOS. DeRegister DALFWK in QURTOS to QDIFWK. 
 * Invoked in DALSYS_DeInitMod, exposed via function table. 
 */

DALResult DALRemoteDeinit(void)
{
    return DAL_ERROR;
}
