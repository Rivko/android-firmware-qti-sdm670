/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: qdi_client.c
@brief: Implementation of the ADSPPM client library for QDI remoting.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/mpd/qdi_client.c#1 $
*/

#include "stdlib.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "qurt_thread.h"
#include "adsppm_qdi.h"
#include "mmpm.h"
#include "qurt_qdi_proxy.h"

#define STKSIZE  4096

typedef struct
{
    int handle;
    int initialized;
} QDI_Adsppm_Client_Ctx_Type;


static QDI_Adsppm_Client_Ctx_Type Adsppm_Client_Ctx;

static void Adsppmcb_init_Client(void);


/**
 * @func - MMPM_Register_Ext
 * @brief MMPM_Register_Ext API is used to register access to a device.
 *        This API has to be called prior to calls of MMPM_Request(), MMPM_Release(), MMPM_Resume(),
 *        and MMPM_Pause(), as the returned client ID from MMPM_Register_Ext() will be needed as input to these
 *        API calls.
 *        The MMPM_GetInfo call does not need registration to MMPM.
 * @param pRegParam - A pointer to the MmpmRegParamType structure.
 * @return   On success, a unique client ID. On failure, 0.
 */
uint32 MMPM_Register_Ext(MmpmRegParamType *pRegParam)
{
    uint32 clientId = 0;
    int ret = 0;
       if(Adsppm_Client_Ctx.initialized)
       {
        //QDI_QDSPPM_USER_ID will be defined in scons file- it will be set appropriately 
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_REGISTER, pRegParam, &clientId, QDI_QDSPPM_USER_ID);
         if(ret < 0)
         {
            clientId = 0;
         }
       }
    return clientId;
}


/**
 * @func - MMPM_Deregister_Ext
 * @brief MMPM_Deregister_Ext is used to deregister access to the device.
 *        All usecases associated with the device will be removed, with its requested resources released.
 *        The device register ID will be 0.
 * @param clientId - the registered client ID.
 * @return On success MMPM_STATUS_SUCCESS
 */
MMPM_STATUS MMPM_Deregister_Ext(uint32 clientId)
{
    MMPM_STATUS result = MMPM_STATUS_NOTINITIALIZED;
    int ret = 0;
    if(Adsppm_Client_Ctx.initialized)
    {
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_DEREGISTER, clientId, &result);
        if(ret < 0)
        {
            result = MMPM_STATUS_FAILED;
        }
    }
    return result;
}


/**
 * @func - MMPM_Request_Ext
 * @brief MMPM_Request_Ext is used to make multiple request calls within one call.
 *        Multiple requests will be executed and the API call will end when all requests are done execution.
 *        When each request inside the multiple requests is executed, its behavior is identical to that of MMPM_Request.
 *        For individual return status for each request, Client needs to parse the pStsArray inside the pRscExtParam.
 * @param clientId - the registered client ID.
 * @param pRscExtParam - A pointer to the MmpmRscExtParamType structure.that contains multiple requests' parameters
 * @return On success MMPM_STATUS_SUCCESS
 */
MMPM_STATUS MMPM_Request_Ext(uint32 clientId, MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS result = MMPM_STATUS_NOTINITIALIZED;
    int ret = 0;
    if(Adsppm_Client_Ctx.initialized)
    {
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_REQUEST, clientId, pRscExtParam, &result);
        if(ret < 0)
        {
            result = MMPM_STATUS_FAILED;
        }
    }
    return result;
}


/**
 * @func - MMPM_Release_Ext
 * @brief MMPM_Release_Ext is used to make multiple release calls within one call.
 *        Multiple releases will be executed and the API call will end when all releases are done execution.
 *        When each release inside the multiple releases is executed, its behavior is identical to that of MMPM_Release.
 *        For individual return status for each release, Client needs to parse the pStsArray inside the pRscExtParam.
 * @param clientId - the registered client ID.
 * @param pRscExtParam - A pointer to the MmpmRscExtParamType structure.that contains multiple releases' parameters
 * @return On success MMPM_STATUS_SUCCESS.
 */
MMPM_STATUS MMPM_Release_Ext(uint32 clientId, MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS result = MMPM_STATUS_NOTINITIALIZED;
    int ret = 0;
    if(Adsppm_Client_Ctx.initialized)
    {
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_RELEASE, clientId, pRscExtParam, &result);
        if(ret < 0)
        {
            result = MMPM_STATUS_FAILED;
        }
    }
    return result;
}


/**
 * @func - MMPM_GetInfo
 * @brief MMPM_GetInfo is used to retrive resources info, this API call does NOT need pre-registration.
 *        Based on the information id specified in the MmpmInfoDataType structure, the MMPM
 *        returns the requested information to the registered device ID.
 * @param pInfoData - A pointer to the MmpmInfoDataType structure.
 * @return On success MMPM_STATUS_SUCCESS
 */
MMPM_STATUS MMPM_GetInfo(MmpmInfoDataType *pInfoData)
{
    MMPM_STATUS result = MMPM_STATUS_NOTINITIALIZED;
    int ret = 0;
    if(Adsppm_Client_Ctx.initialized)
    {
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_GET_INFO, pInfoData, &result);
        if(ret < 0)
        {
            result = MMPM_STATUS_FAILED;
        }
    }
    return result;
}


/**
 * @func - MMPM_SetParameter
 * @brief MMPM_SetParameter is used to set various client specific and global parameters.
 * @param pParamConfig - A pointer to the MmpmParameterConfigType structure.
 * @return On success MMPM_STATUS_SUCCESS
 */
MMPM_STATUS MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfig)
{
    MMPM_STATUS result = MMPM_STATUS_NOTINITIALIZED;
    int ret = 0;
    if(Adsppm_Client_Ctx.initialized)
    {
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_SET_PARAM, clientId, pParamConfig, &result);
        if(ret < 0)
        {
            result = MMPM_STATUS_FAILED;
        }
    }
    return result;
}


/**
 * for corebsp image init
 */
void ADSPPM_Init_Client(void)
{
    Adsppm_Client_Ctx.initialized = 0;
    Adsppm_Client_Ctx.handle = qurt_qdi_open(ADSPPM_QDI_DRV_NAME);
    // Init mmpm user dispatcher thread
    Adsppmcb_init_Client();
    if(Adsppm_Client_Ctx.handle >= 0)
    {
        qurt_client_proxy_ref_add (Adsppm_Client_Ctx.handle);
        Adsppm_Client_Ctx.initialized = 1;
    }
}


/**
 * @func ADSPPM_Deinit_Client - MMPM_Deinit
 * @brief Deinitialize core MMPM modules
 */
void ADSPPM_Deinit_Client(void)
{
    qurt_client_proxy_ref_remove (Adsppm_Client_Ctx.handle);
    qurt_qdi_close(Adsppm_Client_Ctx.handle);
    Adsppm_Client_Ctx.initialized = 0;
}


/**
 * @func Adsppm_callback_dispatcher - MMPM call back dispatcher thread
 * @brief dispatch thread in mmpm user process
 */
static void Adsppm_callback_dispatcher(void *arg)
{
    Adsppm_cbinfo_Client_t info;
    int ret = 0;
    for(;;)
    {
        //QDI_QDSPPM_USER_ID will be defined in scons file...  will be set appropriately    
        ret = qurt_qdi_handle_invoke(Adsppm_Client_Ctx.handle, ADSPPM_GET_CB, &info, QDI_QDSPPM_USER_ID);
        if((0 == ret) && (NULL != info.pfn))
        {
            info.callbackParam.callbackData = (void*) &info.callbackValue;
            (*info.pfn)(&info.callbackParam);
        }
    }
}


static void Adsppmcb_init_Client(void)
{
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    qurt_thread_attr_init(&attr);
    qurt_thread_attr_set_stack_size(&attr, STKSIZE);
    qurt_thread_attr_set_stack_addr(&attr, malloc(STKSIZE));
    qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()));
    qurt_thread_create(&tid, &attr, Adsppm_callback_dispatcher, NULL);
}


uint32 MMPM_Init(char *cmd_line)
{
    MMPM_STATUS result = MMPM_STATUS_SUCCESS;
    ADSPPM_Init_Client();
    if(0 == Adsppm_Client_Ctx.initialized)
    {
        result = MMPM_STATUS_FAILED;
    }
    return result;
}


/**
 * for corebsp image init
 */
void MMPM_Init_Ext(void)
{
    MMPM_Init(NULL);
}

