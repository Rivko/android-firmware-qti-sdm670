/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_sleep.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "npa.h"
#include "adsppm_utils.h"
#include "adsppm.h"
#include "asic.h"
#include "core.h"
//#include "stdio.h"
#include "stdlib.h"
#include "hal_sleep.h"
#include "sleep_lpr.h"
#include "sleep_npa.h"



const char SLEEP_LATENCY_NODE_NAME[] = "/core/cpu/latency/usec";
const char ADSPPM_SLEEP_CLIENT_NAME[] = "/core/power/adsppm";


typedef struct
{
    npa_client_handle handle;
    char clientName[MAX_ADSPPM_CLIENT_NAME];
} clientHandleType;

typedef struct
{
    boolean          slpInitSuccess;
    DALSYSSyncHandle slpdrvCtxLock;
    clientHandleType slpRequestHandle;
	clientHandleType slpRpmLpmHandle;
    boolean          slpRpmLpmSuccess;
} gSlpInfoType;

static gSlpInfoType gSlpInfo;

static void SlpHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size);
static void SlpRpmLpmHandleCb(void *context, unsigned int event_type, void *data, unsigned int data_size);


// Enter and exit function currently unused
AdsppmStatusType Slp_Init(void (*sleep_enter_func)(uint32), void (*sleep_exit_func)(void))
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    gSlpInfo.slpInitSuccess = FALSE;
	gSlpInfo.slpRpmLpmSuccess = FALSE;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        (DALSYSSyncHandle *)&gSlpInfo.slpdrvCtxLock,
        NULL))
    {
        sts = Adsppm_Status_Failed;
    }
    else
    {
        strlcpy(gSlpInfo.slpRequestHandle.clientName, "adsppmsleep", MAX_ADSPPM_CLIENT_NAME);
		strlcpy(gSlpInfo.slpRpmLpmHandle.clientName,  "dsppmsleeprpmlpm", MAX_ADSPPM_CLIENT_NAME);
        npa_resource_available_cb(SLEEP_LATENCY_NODE_NAME, SlpHandleInitCb, NULL);
		npa_resource_available_cb(SLEEP_RPM_LPR_NODE_NAME, SlpRpmLpmHandleCb, NULL);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


static void SlpHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    gSlpInfo.slpRequestHandle.handle = npa_create_sync_client(
        SLEEP_LATENCY_NODE_NAME,
        gSlpInfo.slpRequestHandle.clientName,
        NPA_CLIENT_REQUIRED);
    if(NULL != gSlpInfo.slpRequestHandle.handle)
    {
        gSlpInfo.slpInitSuccess = TRUE;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
}

static void SlpRpmLpmHandleCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    gSlpInfo.slpRpmLpmHandle.handle = npa_create_sync_client( SLEEP_RPM_LPR_NODE_NAME,
                                                              gSlpInfo.slpRpmLpmHandle.clientName,
                                                              NPA_CLIENT_REQUIRED);
    if(NULL != gSlpInfo.slpRpmLpmHandle.handle)
    {
        gSlpInfo.slpRpmLpmSuccess = TRUE;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
}

AdsppmStatusType Slp_RequestRpmLpm (uint32 status)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (FALSE == gSlpInfo.slpRpmLpmSuccess)
    {
       sts = Adsppm_Status_NotInitialized;
    }
    else
    {
       adsppm_lock(gSlpInfo.slpdrvCtxLock);
       ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "NPA request for %s, status: %d",
                SLEEP_RPM_LPR_NODE_NAME, status);
       npa_issue_required_request(gSlpInfo.slpRpmLpmHandle.handle, status);
       adsppm_unlock(gSlpInfo.slpdrvCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


AdsppmStatusType Slp_RequestLatency(uint32 latency)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if (FALSE == gSlpInfo.slpInitSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gSlpInfo.slpdrvCtxLock);
        if(HAL_SLEEP_LATENCY_DONT_CARE == latency)
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                "Complete NPA request for %s",
                SLEEP_LATENCY_NODE_NAME);
            npa_complete_request(gSlpInfo.slpRequestHandle.handle);
        }
        else
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "NPA request for %s, latency: %d",
                SLEEP_LATENCY_NODE_NAME, latency);
            npa_issue_required_request(gSlpInfo.slpRequestHandle.handle, latency);
        }
        adsppm_unlock(gSlpInfo.slpdrvCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


AdsppmStatusType Slp_Release()
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(FALSE == gSlpInfo.slpInitSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gSlpInfo.slpdrvCtxLock);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Complete NPA request for %s",
            SLEEP_LATENCY_NODE_NAME);
        npa_complete_request(gSlpInfo.slpRequestHandle.handle);
        adsppm_unlock(gSlpInfo.slpdrvCtxLock);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}

