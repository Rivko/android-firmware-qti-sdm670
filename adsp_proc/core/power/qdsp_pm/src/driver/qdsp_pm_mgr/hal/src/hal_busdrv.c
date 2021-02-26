/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_busdrv.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "core.h"
#include "core_internal.h"
#include "hal_busdrv.h"
#include "icbarb.h"
#include "icbid.h"
#include "npa.h"
#include "npa_transaction.h"
#include "stdlib.h"
#include "stdio.h"


typedef struct
{
    AdsppmBusRouteType busdrvRoute;
    npa_client_handle  handle;
    char               clientName[MAX_ADSPPM_CLIENT_NAME];
} clientHandleType;

typedef struct
{
    DALSYSSyncHandle         lock;
    boolean                  initSuccess;
    uint32                   numMasterSlave;
    AsicBusExtRouteType      *extRouteArray;
    AdsppmBusBWDataIbAbType  *pExtBwValues;
    npa_transaction_handle   transactionHandle;
    clientHandleType         *bwRequestHandle;
    clientHandleType         mipsBwRequestHandle;
    npa_client_handle        snochandle;
    uint32                   numMipsRequest;
    const AdsppmBusRouteType *pMipsRouteArray;
    AdsppmBusBWDataIbAbType  *pMipsBwValues;
    npa_client_handle        alchandle;
} busDriverInfoType;

static busDriverInfoType gBusDriverInfo;
static void BwHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size);
static void SnocHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size);
static void AlcHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size);


const char *resources[] = {(ICBARB_NODE_NAME)};


AdsppmStatusType Bus_Init(void)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    gBusDriverInfo.initSuccess = FALSE;
    gBusDriverInfo.numMipsRequest = ACMBus_GetNumberOfMipsBwRoutes();
    gBusDriverInfo.pMipsRouteArray = ACMBus_GetMipsBwRoutes();
    // Getting number of Master/Slave routes and allocating memory for ExtRouteArray and ClientHandle arrarys
    gBusDriverInfo.numMasterSlave = ACMBus_GetNumberOfExtRoutes();
    if(0 != gBusDriverInfo.numMasterSlave)
    {
        gBusDriverInfo.extRouteArray =
            (AsicBusExtRouteType *) malloc(sizeof(AsicBusExtRouteType)*(gBusDriverInfo.numMasterSlave));
        gBusDriverInfo.bwRequestHandle =
            (clientHandleType *) malloc(sizeof(clientHandleType)*(gBusDriverInfo.numMasterSlave)); //extra handle for MIPS
        gBusDriverInfo.pExtBwValues =
            (AdsppmBusBWDataIbAbType *) malloc(sizeof(AdsppmBusBWDataIbAbType)*(gBusDriverInfo.numMasterSlave));
        gBusDriverInfo.pMipsBwValues =
            (AdsppmBusBWDataIbAbType *) malloc(sizeof(AdsppmBusBWDataIbAbType)*(gBusDriverInfo.numMipsRequest));
        if((NULL == gBusDriverInfo.extRouteArray) ||
            (NULL == gBusDriverInfo.bwRequestHandle) ||
            (NULL == gBusDriverInfo.pExtBwValues) ||
            (NULL == gBusDriverInfo.pMipsBwValues))
        {
            sts = Adsppm_Status_NoMemory;
            if(NULL != gBusDriverInfo.extRouteArray)
            {
                free(gBusDriverInfo.extRouteArray);
            }
            if(NULL != gBusDriverInfo.bwRequestHandle)
            {
                free(gBusDriverInfo.bwRequestHandle);
            }
            if(NULL != gBusDriverInfo.pExtBwValues)
            {
                free(gBusDriverInfo.pExtBwValues);
            }
            if(NULL != gBusDriverInfo.pMipsBwValues)
            {
                free(gBusDriverInfo.pMipsBwValues);
            }
        }
        else
        {
            if(DAL_SUCCESS != DALSYS_SyncCreate(
                DALSYS_SYNC_ATTR_RESOURCE,
                (DALSYSSyncHandle *)&gBusDriverInfo.lock,
                NULL))
            {
                sts = Adsppm_Status_Failed;
            }
            else
            {
                // Getting array of master/slave routes
                ACMBus_GetExtRoutes(gBusDriverInfo.extRouteArray);
                npa_resource_available_cb(ICBARB_NODE_NAME, BwHandleInitCb, NULL);
                npa_resource_available_cb(SNOC_NODE_NAME, SnocHandleInitCb, NULL);
                npa_resource_available_cb(ALC_NODE_NAME, AlcHandleInitCb, NULL);
            }
        }
    }
    else
    {
        sts = Adsppm_Status_Failed;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


static void BwHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ICBArb_CreateClientVectorType vector;
    ICBArb_CreateClientVectorType mipsVector;
    ICBArb_MasterSlaveType route;
    uint32 i, j;
    uint32 result = TRUE;
    ADSPPM_LOG_FUNC_PROFILESTART;

    //gets transaction handle
    gBusDriverInfo.transactionHandle = npa_define_transaction("Adsppm_Bw", NPA_TRANSACTION_LAZY_LOCKING, 1, resources);

    if(NULL != gBusDriverInfo.transactionHandle)
    {
        ICBArb_MasterSlaveType *mipsMasterSlavePairs =
            (ICBArb_MasterSlaveType *) malloc(sizeof(ICBArb_MasterSlaveType)*(gBusDriverInfo.numMipsRequest));

        if(NULL != mipsMasterSlavePairs)
        {
            for(i = 0; i < gBusDriverInfo.numMasterSlave ; i++)
            {
                route.eMaster = gBusDriverInfo.extRouteArray[i].masterPort.icbarbMaster;
                route.eSlave = gBusDriverInfo.extRouteArray[i].slavePort.icbarbSlave;

                gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort =
                    gBusDriverInfo.extRouteArray[i].masterPort.adsppmMaster;
                gBusDriverInfo.bwRequestHandle[i].busdrvRoute.slavePort =
                    gBusDriverInfo.extRouteArray[i].slavePort.adsppmSlave;

                snprintf(gBusDriverInfo.bwRequestHandle[i].clientName,
                    sizeof(gBusDriverInfo.bwRequestHandle[i].clientName),
                    "AdsppmM%dS%d",
                    gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort,
                    gBusDriverInfo.bwRequestHandle[i].busdrvRoute.slavePort);
					
				//A dummy busport,AdsppmBusPort_Snoc_Master, is being created to send split votes to bus driver; this is adhoc and
                //only valid for Napali and warlock

                if(AdsppmBusPort_Adsp_Master == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort ||
                   AdsppmBusPort_Snoc_Master == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort )
                {
                    gBusDriverInfo.bwRequestHandle[i].handle = npa_create_sync_client_ex(
                        ICBARB_NODE_NAME,
                        gBusDriverInfo.bwRequestHandle[i].clientName,
                        NPA_CLIENT_SUPPRESSIBLE_VECTOR,
                        ICBARB_CREATE_CLIENT_VECTOR(&vector, &route, 1, NULL));
                }
                else
                {
                    gBusDriverInfo.bwRequestHandle[i].handle = npa_create_sync_client_ex(
                        ICBARB_NODE_NAME,
                        gBusDriverInfo.bwRequestHandle[i].clientName,
                        NPA_CLIENT_VECTOR,
                        ICBARB_CREATE_CLIENT_VECTOR(&vector, &route, 1, NULL));
                }

                if(NULL == gBusDriverInfo.bwRequestHandle[i].handle)
                {
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                        "icbarb client create failed:M=%u, S=%u",
                        gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort,
                        gBusDriverInfo.bwRequestHandle[i].busdrvRoute.slavePort);
                    gBusDriverInfo.initSuccess = FALSE;
                    result = FALSE;
                    break;
                }

                for(j = 0; j < gBusDriverInfo.numMipsRequest; j++)
                {
                    if((gBusDriverInfo.extRouteArray[i].masterPort.adsppmMaster == gBusDriverInfo.pMipsRouteArray[j].masterPort) &&
                        (gBusDriverInfo.extRouteArray[i].slavePort.adsppmSlave == gBusDriverInfo.pMipsRouteArray[j].slavePort))
                    {
                        mipsMasterSlavePairs[j].eMaster = route.eMaster;
                        mipsMasterSlavePairs[j].eSlave = route.eSlave;
                    }
                }
            }
            if(TRUE == result)
            {
                snprintf(gBusDriverInfo.mipsBwRequestHandle.clientName, MAX_ADSPPM_CLIENT_NAME, "ADSPPM MIPS BW");
                gBusDriverInfo.mipsBwRequestHandle.handle = npa_create_sync_client_ex(
                    ICBARB_NODE_NAME,
                    gBusDriverInfo.mipsBwRequestHandle.clientName,
                    NPA_CLIENT_SUPPRESSIBLE_VECTOR,
                    ICBARB_CREATE_CLIENT_VECTOR(&mipsVector, mipsMasterSlavePairs, gBusDriverInfo.numMipsRequest, NULL));

                if(NULL != gBusDriverInfo.mipsBwRequestHandle.handle)
                {
                    gBusDriverInfo.initSuccess = TRUE;
                }
                else
                {
                    gBusDriverInfo.initSuccess = FALSE;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "Failed creation of MIPS icbarb client");
                }
            }
            free(mipsMasterSlavePairs);
        }
        else
        {
            gBusDriverInfo.initSuccess = FALSE;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "Failed to allocate memory");
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
}

static void SnocHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    
    gBusDriverInfo.snochandle = 
        npa_create_sync_client( SNOC_NODE_NAME, "adsppm_snoc", NPA_CLIENT_SUPPRESSIBLE);
        
    if( NULL == gBusDriverInfo.snochandle)
    {
        gBusDriverInfo.initSuccess = FALSE;
    }
    
    ADSPPM_LOG_FUNC_PROFILEEND;
}          

static void AlcHandleInitCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Alc))
    {
       gBusDriverInfo.alchandle = npa_create_sync_client_ex( ALC_NODE_NAME,
                                                             "ADSPPM Alc client",
                                                             NPA_CLIENT_VECTOR,
                                                             0,
                                                             NULL);
       if(NULL == gBusDriverInfo.alchandle)
       {
          ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                              "Failed creation of ADSPPM alc client");
          gBusDriverInfo.initSuccess = FALSE;
       }
    }
    
    ADSPPM_LOG_FUNC_PROFILEEND;
}

AdsppmStatusType Bus_IssueBWRequest_MIPS(AdsppmBusBWRequestValueType *pRequest)
{
    ICBArb_RequestType bwRequest[gBusDriverInfo.numMipsRequest];
    uint32 i, j;
    AdsppmStatusType sts = Adsppm_Status_Success;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if(FALSE == gBusDriverInfo.initSuccess)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gBusDriverInfo.lock);
        for(i = 0; i < BUS_NUMBER_MIPS_ROUTES; i++) // The request always contains 2 entries
        {
            for(j = 0; j < gBusDriverInfo.numMipsRequest; j++)
            {
                if((pRequest[i].busRoute.masterPort == gBusDriverInfo.pMipsRouteArray[j].masterPort) &&
                    (pRequest[i].busRoute.slavePort == gBusDriverInfo.pMipsRouteArray[j].slavePort))
                {
                    bwRequest[j].arbType = ICBARB_REQUEST_TYPE_3;
                    bwRequest[j].arbData.type3.uIb = pRequest[i].bwValue.busBwAbIb.Ib;
                    bwRequest[j].arbData.type3.uAb = pRequest[i].bwValue.busBwAbIb.Ab;
                    ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_INFO,
                        "%s(%u) to %s(%u), Ib: %u, Ab: %u",
                        adsppm_getBusPortName(pRequest[i].busRoute.masterPort),
                        pRequest[i].busRoute.masterPort,
                        adsppm_getBusPortName(pRequest[i].busRoute.slavePort),
                        pRequest[i].busRoute.slavePort,
                        (uint32)bwRequest[j].arbData.type3.uIb,
                        (uint32)bwRequest[j].arbData.type3.uAb);
                    ADSPPM_QDSS_EVENT_4(ADSPPM_BW_REQ,
                        pRequest[i].busRoute.masterPort, pRequest[i].busRoute.slavePort,
                        (uint32)bwRequest[j].arbData.type3.uIb,
                        (uint32)bwRequest[j].arbData.type3.uAb);
                    gBusDriverInfo.pMipsBwValues[j].Ab = pRequest[i].bwValue.busBwAbIb.Ab;
                    gBusDriverInfo.pMipsBwValues[j].Ib = pRequest[i].bwValue.busBwAbIb.Ib;
                }
            }
        }
        icbarb_issue_request(gBusDriverInfo.mipsBwRequestHandle.handle, bwRequest, gBusDriverInfo.numMipsRequest);
        adsppm_unlock(gBusDriverInfo.lock);
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


AdsppmStatusType Bus_ReleaseBWRequest_MIPS(void)
{
    AdsppmStatusType sts = Adsppm_Status_Success;
    int i = 0;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(gBusDriverInfo.initSuccess == FALSE)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gBusDriverInfo.lock);
        icbarb_complete_request(gBusDriverInfo.mipsBwRequestHandle.handle);
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Complete bus request");
        for(i = 0; i < gBusDriverInfo.numMipsRequest; i++)
        {
            gBusDriverInfo.pMipsBwValues[i].Ab = 0;
            gBusDriverInfo.pMipsBwValues[i].Ib = 0;
        }
        adsppm_unlock(gBusDriverInfo.lock);
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


AdsppmStatusType Bus_IssueBWRequest(uint32 num, AdsppmBusBWRequestValueType *pRequest)
{
    uint8 i, j;
    ICBArb_RequestType bwRequest[1];
    AdsppmStatusType sts = Adsppm_Status_Success;
	const AsicBusPortDescriptorType *pSPortDescriptor;
	 
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(FALSE == gBusDriverInfo.initSuccess)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        if(0 < num)
        {
            adsppm_lock(gBusDriverInfo.lock);
            npa_begin_transaction(gBusDriverInfo.transactionHandle);
            bwRequest[0].arbType = ICBARB_REQUEST_TYPE_3_LAT;

            for(j = 0; j < num; j++)
            {
                for(i = 0; i < gBusDriverInfo.numMasterSlave; i++)
                {
                    if((pRequest[j].busRoute.masterPort == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort) &&
                        (pRequest[j].busRoute.slavePort == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.slavePort))
                    {
                        // Check ib and ab and issue request (or complete request)
                        if(NULL == gBusDriverInfo.bwRequestHandle[i].handle)
                        {
                            sts = Adsppm_Status_Failed;
                            break;
                        }
                        else
                        {
                            pSPortDescriptor = ACMBus_GetPortDescriptor(pRequest[j].busRoute.slavePort);
							
							if(pSPortDescriptor == NULL)
							{
								ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,"slavePortDescriptor is Null for Request : %u ",j);
								sts = Adsppm_Status_Failed;
								break;
							}
							
                            ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                                "%s(%u) to %s(%u) Bus Driver Slave Port Id %u",
                                adsppm_getBusPortName(pRequest[j].busRoute.masterPort),
                                pRequest[j].busRoute.masterPort,
                                adsppm_getBusPortName(pRequest[j].busRoute.slavePort),
                                pRequest[j].busRoute.slavePort,
                                pSPortDescriptor->icbarbId.icbarbSlave);
                            ADSPPM_QDSS_EVENT_4(ADSPPM_BW_REQ,
                                pRequest[j].busRoute.masterPort,
                                pRequest[j].busRoute.slavePort,
                                (uint32)pRequest[j].bwValue.busBwAbIb.Ib,
                                (uint32)pRequest[j].bwValue.busBwAbIb.Ab);
                            if((0 == pRequest[j].bwValue.busBwAbIb.Ab) &&
                                (0 == pRequest[j].bwValue.busBwAbIb.Ib))
                            {
                                icbarb_complete_request(gBusDriverInfo.bwRequestHandle[i].handle);
                                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Release bus request, ab, ib is 0");
                            }
                            else
                            {
                                bwRequest[0].arbData.type3.uIb = pRequest[j].bwValue.busBwAbIb.Ib;
                                bwRequest[0].arbData.type3.uAb = pRequest[j].bwValue.busBwAbIb.Ab;
                                bwRequest[0].arbData.type3.uLatencyNs = pRequest[j].bwValue.busBwAbIb.latencyNs;
                                // todo: enable when support is available
                                icbarb_issue_request(gBusDriverInfo.bwRequestHandle[i].handle, bwRequest, 1);
                                ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                                    "Ib: %llu, Ab: %llu Latency: %u",
                                    (uint32)bwRequest[0].arbData.type3.uIb, // two params for %llu
                                    (uint32)(bwRequest[0].arbData.type3.uIb >> 32),
                                    (uint32)bwRequest[0].arbData.type3.uAb, // two params for %llu
                                    (uint32)(bwRequest[0].arbData.type3.uAb >> 32),
                                    (uint32)bwRequest[0].arbData.type3.uLatencyNs);
                            }
                            gBusDriverInfo.pExtBwValues[i].Ab = pRequest[j].bwValue.busBwAbIb.Ab;
                            gBusDriverInfo.pExtBwValues[i].Ib = pRequest[j].bwValue.busBwAbIb.Ib;
                            gBusDriverInfo.pExtBwValues[i].latencyNs = pRequest[j].bwValue.busBwAbIb.latencyNs;
                        }
                    }
                }
            }
            
            npa_end_transaction(gBusDriverInfo.transactionHandle);
            adsppm_unlock(gBusDriverInfo.lock);
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}

AdsppmStatusType Bus_IssueSnocRequest(uint32 snocVoteMhz)
{
    AdsppmStatusType status = Adsppm_Status_Success;

    if(!gBusDriverInfo.initSuccess)
        return Adsppm_Status_NotInitialized;

    if(NULL == gBusDriverInfo.snochandle)
        return Adsppm_Status_Failed;                

    ADSPPM_LOG_FUNC_PROFILESTART;

    adsppm_lock(gBusDriverInfo.lock);

    if(snocVoteMhz == 0)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Complete NPA request for %s",
            SNOC_NODE_NAME);
        npa_complete_request(gBusDriverInfo.snochandle);         
    }
    else
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
            "NPA request for %s, value: %d (Khz)",
            SNOC_NODE_NAME, snocVoteMhz * 1000);
        // NPA resource units is kHz
        npa_issue_required_request(
            gBusDriverInfo.snochandle, snocVoteMhz * 1000);
    }

    adsppm_unlock(gBusDriverInfo.lock);

    ADSPPM_LOG_FUNC_PROFILEEND;

    return status;

}

AdsppmStatusType Bus_IssueAlcRequest (uint32 fal_tolerance, uint32 idle_time)
{
    AdsppmStatusType status = Adsppm_Status_Success;

    if(!gBusDriverInfo.initSuccess)
        return Adsppm_Status_NotInitialized;

    if(NULL == gBusDriverInfo.alchandle)
        return Adsppm_Status_Failed;      
 
    ADSPPM_LOG_FUNC_PROFILESTART;

    adsppm_lock(gBusDriverInfo.lock);

    if(fal_tolerance == 0 &&
       idle_time == 0)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Complete NPA request for %s",
            ALC_NODE_NAME);
        npa_complete_request(gBusDriverInfo.alchandle);         
    }
    else
    {
        ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
            "NPA request for %s, Fal tolerance : %d, Idle time %d",
            ALC_NODE_NAME, fal_tolerance,idle_time );
        
        Adsppm_IcbArb_FalType request = { 0, 0 };
        request.fal_tolerance_ns = fal_tolerance;
        request.idle_time_ns = idle_time;
 
        npa_issue_vector_request( gBusDriverInfo.alchandle, 
                                  sizeof(request)/sizeof(npa_resource_state), 
                                  (npa_resource_state *)&request );
    }

    adsppm_unlock(gBusDriverInfo.lock);

    ADSPPM_LOG_FUNC_PROFILEEND;

    return status;

}


AdsppmStatusType Bus_ReleaseBWRequest(uint32 num, AdsppmBusBWRequestValueType *pRequest)
{
    uint8 i, j;
    AdsppmStatusType sts = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(gBusDriverInfo.initSuccess == FALSE)
    {
        sts = Adsppm_Status_NotInitialized;
    }
    else
    {
        adsppm_lock(gBusDriverInfo.lock);
        npa_begin_transaction(gBusDriverInfo.transactionHandle);

        for(j = 0; j < num; j++)
        {
            for(i = 0; i < gBusDriverInfo.numMasterSlave; i++)
            {
                if((pRequest[j].busRoute.masterPort == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.masterPort) &&
                    (pRequest[j].busRoute.slavePort == gBusDriverInfo.bwRequestHandle[i].busdrvRoute.slavePort))
                {
                    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                        "%s(%u) to %s(%u) release bw",
                        adsppm_getBusPortName(pRequest[j].busRoute.masterPort),
                        pRequest[j].busRoute.masterPort,
                        adsppm_getBusPortName(pRequest[j].busRoute.slavePort),
                        pRequest[j].busRoute.slavePort);
                    ADSPPM_QDSS_EVENT_2(ADSPPM_BW_REL, pRequest[j].busRoute.masterPort, pRequest[j].busRoute.slavePort);
                    // Check ib and ab and issue request (or complete request)
                    if(NULL == gBusDriverInfo.bwRequestHandle[i].handle)
                    {
                        sts = Adsppm_Status_Failed;
                        break;
                    }
                    icbarb_complete_request(gBusDriverInfo.bwRequestHandle[i].handle);

                    gBusDriverInfo.pExtBwValues[i].Ab = 0;
                    gBusDriverInfo.pExtBwValues[i].Ib = 0;
                    break;
                }
            }
        }
        npa_end_transaction(gBusDriverInfo.transactionHandle);
        adsppm_unlock(gBusDriverInfo.lock);
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/**
 * Get last requested BW value for a specific route
 * @param pRoute - external BW route
 * @return BW value
 */
AdsppmBusBWDataIbAbType Bus_GetRequestedBw(AdsppmBusRouteType *pRoute)
{
    AdsppmBusBWDataIbAbType bw = {0, 0};
    int i = 0;
    if((gBusDriverInfo.initSuccess == TRUE) && (NULL != pRoute))
    {
        for(i = 0; i < gBusDriverInfo.numMasterSlave; i++)
        {
            if((gBusDriverInfo.extRouteArray[i].masterPort.adsppmMaster == pRoute->masterPort) &&
                (gBusDriverInfo.extRouteArray[i].slavePort.adsppmSlave == pRoute->slavePort))
            {
                bw.Ab = gBusDriverInfo.pExtBwValues[i].Ab;
                bw.Ib = gBusDriverInfo.pExtBwValues[i].Ib;
                break;
            }
        }
        for(i = 0; i < gBusDriverInfo.numMipsRequest; i++)
        {
            if((gBusDriverInfo.pMipsRouteArray[i].masterPort == pRoute->masterPort) &&
                (gBusDriverInfo.pMipsRouteArray[i].slavePort == pRoute->slavePort))
            {
                bw.Ab += gBusDriverInfo.pMipsBwValues[i].Ab;
                bw.Ib = MAX(bw.Ib, gBusDriverInfo.pMipsBwValues[i].Ib);
                break;
            }
        }
    }
    return bw;
}

