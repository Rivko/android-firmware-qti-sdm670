/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: api.c
@brief: MMPM API wrapper

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/request_mgr/api.c#5 $
*/

#include "mmpm.h"
#include "adsppm.h"
#include "adsppm_dcvs.h"
#include "adsppm_limits.h"
#include "adsppm_utils.h"
#include "stdlib.h"
#include "core.h"
#include "asyncmgr.h"
#include "coreUtils.h"
#include "asic.h"
#include "requestmgr.h"
#include "clientmgr.h"
#include "cachemgr.h"
#include "qurt_thread.h"
#include "dcvsmgr.h"
#include "busmgr.h"
#include "ahbmgr.h"
#include "core_internal.h"
#include "adspclkmgr.h"
#include "extbwmgr.h"
#include "systemcachemgr.h"


// This global flag set to 1 to disable MMPM_Request_Ext at runtime
// Returns only MMPM_STATUS_SUCCESS
static volatile uint32 gDISABLEADSPPMREQUESTFLAG = 0;

extern uint32 gADSPPMINITSTATE;

static const AdsppmClkIdType mmpmToAdsppmClk[MMPM_CLK_ID_LPASS_ENUM_MAX] =
{
    AdsppmClk_None,
    AdsppmClk_HwRsp_Core,
    AdsppmClk_Midi_Core,
    AdsppmClk_AvSync_Xo,
    AdsppmClk_AvSync_Bt,
    AdsppmClk_AvSync_Fm,
    AdsppmClk_Slimbus_Core,
    AdsppmClk_Avtimer_core,
    AdsppmClk_Atime_core,
    AdsppmClk_Atime2_core,
    AdsppmClk_Adsp_Core,
    AdsppmClk_Ahb_Root
};

static const AdsppmClkIdType mmpmtoCdsppmClk[MMPM_CLK_ID_COMPUTE_ENUM_MAX] =
{
    AdsppmClk_None,
    AdsppmClk_VAPSS_Core,
};

static const AdsppmClkIdType mmpmtoSlpipmClk[MMPM_CLK_ID_SLPI_ENUM_MAX] =
{
    AdsppmClk_None,
    AdsppmClk_Sdc_Core,
    AdsppmClk_Qup_Core,
    AdsppmClk_Sram_Core,
    AdsppmClk_Ccd_Core,
};

static const AdsppmBusPortIdType mmpmToAdsppmBusPort[MMPM_BW_PORT_ID_MAX] =
{
    AdsppmBusPort_None,
    AdsppmBusPort_Adsp_Master,
    AdsppmBusPort_Dml_Master,
    AdsppmBusPort_Aif_Master,
    AdsppmBusPort_Slimbus_Master,
    AdsppmBusPort_Midi_Master,
    AdsppmBusPort_HwRsmp_Master,
    AdsppmBusPort_Ext_Ahb_Master,
    AdsppmBusPort_Spdif_Master,
    AdsppmBusPort_Hdmirx_Master,
    AdsppmBusPort_Hdmitx_Master,
    AdsppmBusPort_Sif_Master,
    AdsppmBusPort_DPAudio_Master,
    AdsppmBusPort_Vapss_Hcp_Master,
    AdsppmBusPort_Vapss_Dma_Master,
    AdsppmBusPort_Sdc_Master,
    AdsppmBusPort_Qup_Master,
    AdsppmBusPort_Dml_Slave,
    AdsppmBusPort_Aif_Slave,
    AdsppmBusPort_Slimbus_Slave,
    AdsppmBusPort_Midi_Slave,
    AdsppmBusPort_HwRsmp_Slave,
    AdsppmBusPort_AvSync_Slave,
    AdsppmBusPort_Lpm_Slave,
    AdsppmBusPort_Sram_Slave,
    AdsppmBusPort_Ext_Ahb_Slave,
    AdsppmBusPort_Ddr_Slave,
    AdsppmBusPort_Ocmem_Slave,
    AdsppmBusPort_PerifNoc_Slave,
    AdsppmBusPort_Spdif_Slave,
    AdsppmBusPort_Hdmirx_Slave,
    AdsppmBusPort_Hdmitx_Slave,
    AdsppmBusPort_Sif_Slave,
    AdsppmBusPort_Bstc_Slave,
    AdsppmBusPort_Dcodec_Slave,
    AdsppmBusPort_DPAudio_Slave,
    AdsppmBusPort_Cdsp_TCM_Slave,
    AdsppmBusPort_Slpi_Sram_Slave,
    AdsppmBusPort_Core,

};


/**
 * @const busPortName - defines Bus ports name to be used by MMPM ULOG internally
 *                      keep updated with MmpmBwPortIdType in mmpm.h
 */
const char *MMPMBusPortName[MMPM_BW_PORT_ID_MAX] =
{
    "NONE",
    "ADSP_MASTER",
    "DML_MASTER",
    "AIF_MASTER",
    "SLIMBUS_MASTER",
    "MIDI_MASTER",
    "HWRSMP_MASTER",
    "EXT_AHB_MASTER",
    "SPDIF_MASTER",
    "HDMI_RX_MASTER",
    "HDMI_TX_MASTER",
    "SIF_MASTER",
    "DPAUDIO_MASTER",
    "VAPSS_HCP_MASTER",
    "VAPSS_DMA_MASTER",
    "SDC_MASTER",
    "QUP_MASTER",
    "DML_SLAVE",
    "AIF_SLAVE",
    "SLIMBUS_SLAVE",
    "MIDI_SLAVE",
    "HWRSMP_SLAVE",
    "AVSYNC_SLAVE",
    "LPM_SLAVE",
    "SRAM_SLAVE",
    "EXT_AHB_SLAVE",
    "DDR_SLAVE",
    "OCMEM_SLAVE",
    "PERIFNOC_SLAVE",
    "SPDIF_SLAVE",
    "HDMI_RX_SLAVE",
    "HDMI_TX_SLAVE",
    "SIF_SLAVE",
    "BSTC_SLAVE",
    "DCODEC_SLAVE",
    "DPAUDIO_SLAVE",
    "TCM_SLAVE",
    "SLPI_SRAM_SLAVE",
    "CORE",
};

static boolean is_lpass_slpi_client(AdsppmCoreIdType coreId)
{
	/* This check is only for clocks, Q6 Check isnt required */
	if (coreId == Adsppm_Core_Id_Slpi_SDC ||
		coreId == Adsppm_Core_Id_Slpi_QUP ||
		coreId == Adsppm_Core_Id_Slpi_CCD ||
		coreId == Adsppm_Core_Id_Slpi_SRAM)
	{
		return TRUE;
    }
	return FALSE;
}

MMPM_STATUS apiProcessSingleRequest(AdsppmRscIdType adsppmRscId, MmpmApiType apiType, uint32 *pAsyncRscMap)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    *pAsyncRscMap |= (1<<adsppmRscId);
    if(MMPM_API_TYPE_ASYNC != apiType)
    {
        sts = (MMPM_STATUS) RM_RequestResource(adsppmRscId);
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


MMPM_STATUS apiReleaseSingleResource(uint32 clientId, AdsppmRscIdType adsppmRscId, uint32 extData, MmpmApiType apiType, uint32 *asyncRscMap)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    MMPM_STATUS sts = (MMPM_STATUS) RM_ReleaseRequest(clientId, adsppmRscId, extData);
    switch(sts)
    {
    case MMPM_STATUS_SUCCESS:
        sts = apiProcessSingleRequest(adsppmRscId, apiType, asyncRscMap);
        break;

    case MMPM_STATUS_RESOURCENOTREQUESTED:
        sts = MMPM_STATUS_SUCCESS;
        break;

    default:
        break;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}

// By default we'll set the client class to audio if the current caller's
// thread is using the auxilliary partition.
static MmpmClientClassType getDefaultClientClass()
{
    qurt_cache_partition_t partition = qurt_thread_get_l2cache_partition();
    return (partition == AUX_PARTITION) ?
        MMPM_AUDIO_CLIENT_CLASS : MMPM_UNKNOWN_CLIENT_CLASS;
}

// By default ADSPPM will set
static MmpmDcvsParticipationType getDefaultDcvsParticipation()
{
    MmpmDcvsParticipationType defaultDcvsParticipation;
    defaultDcvsParticipation.enable = TRUE;
    defaultDcvsParticipation.enableOpt = MMPM_DCVS_ADJUST_UP_DOWN;
    return defaultDcvsParticipation;
}

static Adsppm_Status releaseCacheLineLocking(uint32 clientId)
{
    Adsppm_Status status = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    const AdsppmClientType* client = CM_GetClient(clientId);
    MmpmL2CacheLineLockParameterType disableParam;
    if(client)
    {
        // Only try to unlock if currently locked
        if((client->cacheLockParam.startAddress != 0) &&
            (client->cacheLockParam.size != 0))
        {
            disableParam.startAddress = 0;
            disableParam.size = 0;
            status = CPM_SetLineLocking(&client->cacheLockParam, &disableParam);
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return status;
}

/******************************************************************************
 *                     MMPM_Register_Ext
 *****************************************************************************/
uint32 MMPM_Register_Ext(MmpmRegParamType *pRegParam)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    const AdsppmClientType* newClient = NULL;
    boolean isCoreIDValid = FALSE;

    ADSPPM_LOG_FUNC_PROFILESTART;

    uint32 coreIdx = 0;
    

    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        if(ADSPPM_IsInitialized())
        {   
            if(NULL == pRegParam)
            {
                sts = MMPM_STATUS_NULLPOINTER;
            }
            else
            {
                if(ACM_GetCoreIdIdx(pRegParam->coreId,&coreIdx) == Adsppm_Status_Success)
                {
                    isCoreIDValid = TRUE;
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                        "Valid coreID=%u", pRegParam->coreId);
                }
                
                if( (!isCoreIDValid) ||
                    (NULL == pRegParam->pClientName) ||
                    (MAX_ADSPPM_CLIENT_NAME <= strlen(pRegParam->pClientName)) ||
                    (MMPM_CORE_INSTANCE_NONE == pRegParam->instanceId) ||
                    (MMPM_CORE_INSTANCE_MAX <= pRegParam->instanceId))
                {
                    sts = MMPM_STATUS_BADPARM;
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "bad parameters or null client name or client name is too long, coreId=%u",
                        pRegParam->coreId);
                }
                else
                {
                    AdsppmNewClientInfoType newClientInfo;

                    // Keep MMPM core ID and ADSPPM core id enum definition
                    // identical

                    newClientInfo.coreId =
                        (AdsppmCoreIdType) (pRegParam->coreId);
                    newClientInfo.instanceId =
                        (AdsppmInstanceIdType) (pRegParam->instanceId);
                    newClientInfo.clientName = pRegParam->pClientName;
                    newClientInfo.callBackFlag = pRegParam->callBackFlag;
                    newClientInfo.callBack = (void*) pRegParam->MMPM_Callback;
                    newClientInfo.periodicityType =
                        ((pRegParam->pwrCtrlFlag & PWR_CTRL_PERIODIC_CLIENT) ==
                        PWR_CTRL_PERIODIC_CLIENT) ?
                        Adsppm_ClientType_Periodic : Adsppm_ClientType_Normal;
                    newClientInfo.clientClass = getDefaultClientClass();
                    newClientInfo.dcvsParticipation = getDefaultDcvsParticipation();
                    newClient = CM_AddClient(&newClientInfo);
                    if(newClient != NULL)
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                             "Adding new client CoreID=%u", pRegParam->coreId);

                        if(RM_SetupClient(newClient) == Adsppm_Status_Success)
                        {
                             ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                             "Adding new client ClientID=%u", newClient->clientId);
                            clientId = newClient->clientId; // set return value
                        }
                        else
                        {
                            // Back out new client due to RM failure
                            CM_RemoveClient(newClient->clientId);
                        }
                    }
                }
            }
        }
        else
        {
            sts = MMPM_STATUS_NOTINITIALIZED;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "MMPM not initialized");
        }
    }
    else
    {
        ADSPPM_LOG_FUNC_PROFILEEND;
        return 1;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;

    return clientId;
}


/******************************************************************************
 *                     MMPM_Deregister_Ext
 *****************************************************************************/
MMPM_STATUS MMPM_Deregister_Ext(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        if(ADSPPM_IsInitialized())
        {
            if(clientId)
            {
                sts = (MMPM_STATUS) RM_TeardownClient(clientId);
                if(sts == MMPM_STATUS_SUCCESS)
                {
                    sts = (MMPM_STATUS) releaseCacheLineLocking(clientId);
                    if(sts == MMPM_STATUS_SUCCESS)
                    {
                        sts = (MMPM_STATUS) CM_RemoveClient(clientId);
                    }
                }
            }
            else
            {
                sts = MMPM_STATUS_BADPARM;
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "bad clientid in deregister=%u", clientId);
            }
        }
        else
        {
            sts = MMPM_STATUS_NOTINITIALIZED;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
    }
    else
    {
        sts = MMPM_STATUS_SUCCESS;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;

    return sts;
}


static MMPM_STATUS apiAddBwRequest(uint32 clientId, MmpmRscIdType resourceId, MmpmGenBwReqType *pGenBwReq, MmpmApiType apiType, uint32 *pAsyncRscMap)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    AdsppmBwReqType adsppmBwReqData;
    uint32 bwNum;

    ADSPPM_LOG_FUNC_PROFILESTART;

    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
        "Client %u: BW request%s", clientId,
        (resourceId == MMPM_RSC_ID_GENERIC_BW_EXT) ? " (compensated)" : "");

    if(MAX_NUM_OF_ADSPPM_BW_ARRAY < pGenBwReq->numOfBw)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "Number of BW entries (%u) is too big",
            pGenBwReq->numOfBw);
        sts = MMPM_STATUS_BADPARM;
    }
    else
    {
        memset(&adsppmBwReqData, 0, sizeof(AdsppmBwReqType));
        // Both MMPM resource IDs MMPM_RSC_ID_GENERIC_BW and
        // MMPM_RSC_ID_GENERIC_BW_EXT map to ADSPPM resource ID
        // Adsppm_Rsc_Id_BW, then we use the "request class" to differentiate
        // them within the BW manager.  MMPM resource ID MMPM_RSC_ID_GENERIC_BW
        // maps to request class Adsppm_BwReqClass_Generic.  MMPM resource ID
        // MMPM_RSC_ID_GENERIC_BW_EXT maps to request class
        // Adsppm_BwReqClass_Compensated.
        adsppmBwReqData.requestClass =
            resourceId == MMPM_RSC_ID_GENERIC_BW_EXT ?
            Adsppm_BwReqClass_Compensated :
            Adsppm_BwReqClass_Generic;
        adsppmBwReqData.numOfBw = pGenBwReq->numOfBw;
        // Check Port ID and translate port ID to adsppm
        for(bwNum = 0; bwNum < pGenBwReq->numOfBw; bwNum++)
        {
            MmpmGenBwValType *pBwInfo = &pGenBwReq->pBandWidthArray[bwNum];
            if((MMPM_BW_PORT_ID_NONE == pBwInfo->busRoute.masterPort) ||
                (MMPM_BW_PORT_ID_DML_SLAVE <= pBwInfo->busRoute.masterPort) ||
                (MMPM_BW_PORT_ID_DML_SLAVE > pBwInfo->busRoute.slavePort) ||
                (MMPM_BW_PORT_ID_CORE <= pBwInfo->busRoute.slavePort) ||
                (MMPM_BW_USAGE_LPASS_NONE == pBwInfo->bwValue.busBwValue.usageType) ||
                (MMPM_BW_USAGE_LPASS_ENUM_MAX <= pBwInfo->bwValue.busBwValue.usageType))
            {
                sts = MMPM_STATUS_BADPARM;
                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR,
                    "BW route or usageType is invalid: master=%u, slave=%u, usageType=%u",
                    pBwInfo->busRoute.masterPort,
                    pBwInfo->busRoute.slavePort,
                    pBwInfo->bwValue.busBwValue.usageType);
                break;
            }
            else
            {
                adsppmBwReqData.pBwArray[bwNum].busRoute.masterPort =
                    AdsppmMapMMPMPortIdToADSPPMPortID(pBwInfo->busRoute.masterPort);
                adsppmBwReqData.pBwArray[bwNum].busRoute.slavePort =
                    AdsppmMapMMPMPortIdToADSPPMPortID(pBwInfo->busRoute.slavePort);
                adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.bwBytePerSec =
                    pBwInfo->bwValue.busBwValue.bwBytePerSec;
                adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.usagePercentage =
                    pBwInfo->bwValue.busBwValue.usagePercentage;
                adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.usageType =
                    (AdsppmBwUsageLpassType) pBwInfo->bwValue.busBwValue.usageType;
                adsppmBwReqData.pBwArray[bwNum].bwOperation = AdsppmBusBWOperation_BW;


                ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                    "    MMPM   M=%s(%u), S=%s(%u), Operation=%u",
                    MMPMBusPortName[pBwInfo->busRoute.masterPort],
                    pBwInfo->busRoute.masterPort,
                    MMPMBusPortName[pBwInfo->busRoute.slavePort],
                    pBwInfo->busRoute.slavePort,
                    adsppmBwReqData.pBwArray[bwNum].bwOperation);
                ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                    "    ADSPPM M=%s(%u), S=%s(%u), Operation=%u",
                    adsppm_getBusPortName(adsppmBwReqData.pBwArray[bwNum].busRoute.masterPort),
                    adsppmBwReqData.pBwArray[bwNum].busRoute.masterPort,
                    adsppm_getBusPortName(adsppmBwReqData.pBwArray[bwNum].busRoute.slavePort),
                    adsppmBwReqData.pBwArray[bwNum].busRoute.slavePort,
                    adsppmBwReqData.pBwArray[bwNum].bwOperation);
                ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                    "    BW=%llu at %u, usage=%u",
                    (uint32) adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.bwBytePerSec, // two params for %llu
                    (uint32) (adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.bwBytePerSec >> 32),
                    adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.usagePercentage,
                    adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.usageType);
                ADSPPM_QDSS_EVENT_5(ADSPPM_API_BW_REQ,
                    clientId,
                    adsppmBwReqData.pBwArray[bwNum].busRoute.masterPort,
                    adsppmBwReqData.pBwArray[bwNum].busRoute.slavePort,
                    (uint32) adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.bwBytePerSec,
                    adsppmBwReqData.pBwArray[bwNum].bwValue.busBwValue.usagePercentage);
            }
        }
        if(MMPM_STATUS_SUCCESS == sts)
        {
            sts = (MMPM_STATUS) RM_AddRequest(clientId, Adsppm_Rsc_Id_BW,
                &adsppmBwReqData, 0, (apiType == MMPM_API_TYPE_SYNC));
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/******************************************************************************
 *                     MMPM_Request_Ext
 *****************************************************************************/
MMPM_STATUS MMPM_Request_Ext(uint32 clientId, MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    AdsppmRscIdType adsppmRscId = Adsppm_Rsc_Id_None;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        if(ADSPPM_IsInitialized())
        {
            const AdsppmClientType * client_ptr = CM_GetClient(clientId);

            if((NULL == pRscExtParam) ||
               (0 == clientId) ||
               (client_ptr == NULL))
            {
                sts = MMPM_STATUS_BADPARM;
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "null pointer of pRscExtParam/ClientPtr or clientid is 0, clientid=%u", clientId);
            }
            else
            {
                if((NULL == pRscExtParam->pReqArray) || (NULL == pRscExtParam->pStsArray))
                {
                    sts = MMPM_STATUS_NULLPOINTER;
                }
                else
                {
                    boolean syncRequest = (pRscExtParam->apiType == MMPM_API_TYPE_SYNC);
                    if(Adsppm_Rsc_Id_Max > pRscExtParam->numOfReq)
                    {
                        uint32 index = 0;
                        uint32 asyncRscMap = 0;


                        // For each request in the array, do initial validation of the request
                        // and pass valid requests to the resource manager
                        for(index = 0; index < pRscExtParam->numOfReq; index++)
                        {
                            // Current request from array
                            MmpmRscParamType *pRequest = &pRscExtParam->pReqArray[index];
                            // Parameters for current request
                            MmpmRscParamStructType *pParams = &pRequest->rscParam;
                            // Status for current request
                            MMPM_STATUS *pStatus = &pRscExtParam->pStsArray[index];
                            // Init return sts array
                            *pStatus = MMPM_STATUS_SUCCESS;

                            switch(pRequest->rscId)
                            {
                            case MMPM_RSC_ID_POWER:
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Power request", clientId);

                                uint32 powerOn = 1;
                                adsppmRscId = Adsppm_Rsc_Id_Power;
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_PWR_REQ, clientId);

                                *pStatus = (MMPM_STATUS) RM_AddRequest(clientId,
                                    Adsppm_Rsc_Id_Power, &powerOn, 0,
                                    syncRequest);
                            }
                                break;

                            case MMPM_RSC_ID_REG_PROG:
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Register programming request", clientId);

                                // Covert to bw request
                                AdsppmBusBWRequestValueType adsppmBwReqData;
                                adsppmRscId = Adsppm_Rsc_Id_BW;
                                memset(&adsppmBwReqData, 0, sizeof(AdsppmBusBWRequestValueType));
                                AdsppmRegProgMatchType regProgMatch = (AdsppmRegProgMatchType) pParams->regProgMatch;

                                if((Adsppm_RegProg_None >= regProgMatch) || (Adsppm_RegProg_Enum_Max <= regProgMatch))
                                {
                                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                        "Reg Prog: bad param regProgMatch=%u", regProgMatch);
                                    *pStatus = MMPM_STATUS_BADPARM;
                                }
                                else
                                {
                                    uint64 regProgSpeed = ACMBus_GetRegProgSpeed(regProgMatch);
                                    if(0 == regProgSpeed)
                                    {
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                            "Reg Prog: Register Programming speed returned 0");
                                        *pStatus = MMPM_STATUS_FAILED;
                                    }
                                    else
                                    {
                                        adsppmBwReqData.busRoute.masterPort = AdsppmBusPort_Adsp_Master;
                                        //adsppmBwReqData.busRoute.slavePort will be identified in BUS_ProcessRequest()
                                        adsppmBwReqData.bwValue.busBwValue.bwBytePerSec = regProgSpeed;
                                        adsppmBwReqData.bwValue.busBwValue.usagePercentage = ADSPPM_REG_PROC_CLOCK_BW_PERCENTAGE;
                                        adsppmBwReqData.bwValue.busBwValue.usageType = Adsppm_BwUsage_DSP;
                                        adsppmBwReqData.bwOperation = AdsppmBusBWOperation_RegAccess;

                                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                            "    Adsppm Reg Prog: match=%u -> bw=%u",
                                            regProgMatch, regProgSpeed);
                                        ADSPPM_QDSS_EVENT_3(ADSPPM_API_REG_REQ,
                                            clientId, adsppmBwReqData.busRoute.masterPort,
                                            adsppmBwReqData.busRoute.slavePort);

                                        *pStatus = (MMPM_STATUS) RM_AddRequest(
                                            clientId, Adsppm_Rsc_Id_BW,
                                            &adsppmBwReqData, 1,
                                            syncRequest);
                                    }
                                }
                            }
                                break;

                            case MMPM_RSC_ID_CORE_CLK:
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Clock request, coreID=%d", clientId,client_ptr->coreId);

                                AdsppmClkRequestType adsppmClkReqData;
                                uint32 clkNum = 0;
                                boolean isCDSPClient = FALSE;
                                boolean isSlpiClient = FALSE;
								boolean isLpassSlpiClient = FALSE;
                                MmpmClkReqType *pCoreClkParams = pParams->pCoreClk;
                                adsppmRscId = Adsppm_Rsc_Id_Core_Clk;

                                if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
                                {
                                    if(client_ptr->coreId == Adsppm_Core_Id_Q6_DSP ||
                                       client_ptr->coreId == Adsppm_Core_Id_Compute_HCP ||
                                       client_ptr->coreId == Adsppm_Core_Id_Compute_DMA)
                                    {
                                        isCDSPClient = TRUE;
                                    }
                                }
                                else if (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Enabled)
                                {
                                    if (client_ptr->coreId == Adsppm_Core_Id_Q6_DSP   ||
                                        client_ptr->coreId == Adsppm_Core_Id_Slpi_SDC ||
                                        client_ptr->coreId == Adsppm_Core_Id_Slpi_QUP ||
                                        client_ptr->coreId == Adsppm_Core_Id_Slpi_CCD ||
                                        client_ptr->coreId == Adsppm_Core_Id_Slpi_SRAM)
                                    {
                                        isSlpiClient = TRUE;
                                    }

                                }
								else if (ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled)
                                {
	                                isLpassSlpiClient = is_lpass_slpi_client(client_ptr->coreId);
                                }
                                if(MAX_NUM_OF_ADSPPM_CLOCK_ARRAY < pCoreClkParams->numOfClk)
                                {
                                    *pStatus = MMPM_STATUS_BADPARM;
                                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                        "numbofclk exceed max numofclk=%u",
                                        pCoreClkParams->numOfClk);
                                }
                                else
                                {
                                    memset(&adsppmClkReqData, 0, sizeof(AdsppmClkRequestType));
                                    adsppmClkReqData.numOfClk = pCoreClkParams->numOfClk;
                                    // Translate clock Id to ADSPPM
                                    // and copy all the request values
                                    for(clkNum = 0; clkNum < pCoreClkParams->numOfClk; clkNum++)
                                    {
                                        MmpmClkValType *pClkInfo = &pCoreClkParams->pClkArray[clkNum];

                                        if( isCDSPClient &&
                                            ((MMPM_CLK_ID_COMPUTE_NONE == pClkInfo->clkId.clkIdCompute) ||
                                             (MMPM_CLK_ID_COMPUTE_VAPSS_CORE < pClkInfo->clkId.clkIdCompute) ||
                                             (MMPM_FREQ_MAX <= pClkInfo->freqMatch)))
                                        {
                                            *pStatus = MMPM_STATUS_BADPARM;
                                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                                "bad cdsp_clkid=%u or freqmatch=%u",
                                                pClkInfo->clkId.clkIdLpass, pClkInfo->freqMatch);
                                            break;
                                        }
                                        else if ( (isSlpiClient || isLpassSlpiClient) &&
                                                ((MMPM_CLK_ID_SLPI_NONE == pClkInfo->clkId.clkIdSlpi) ||
                                                 (MMPM_CLK_ID_SLPI_CCD_CORE < pClkInfo->clkId.clkIdSlpi) ||
                                                 (MMPM_FREQ_MAX <= pClkInfo->freqMatch)))
                                        {
                                            *pStatus = MMPM_STATUS_BADPARM;
                                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                                "bad slpi_clkid=%u or freqmatch=%u",
                                                pClkInfo->clkId.clkIdSlpi, pClkInfo->freqMatch);
                                        }
                                        else if( (MMPM_CLK_ID_LPASS_NONE == pClkInfo->clkId.clkIdLpass) ||
                                            (MMPM_CLK_ID_LPASS_ADSP_CORE <= pClkInfo->clkId.clkIdLpass) ||
                                            (MMPM_FREQ_MAX <= pClkInfo->freqMatch))
                                        {
                                            *pStatus = MMPM_STATUS_BADPARM;
                                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                                "bad lpassclkid=%u or freqmatch=%u",
                                                pClkInfo->clkId.clkIdLpass, pClkInfo->freqMatch);
                                            break;
                                        }
                                        else
                                        {
                                            /* Check if this is CDSPPM clients, we use the clock type from CDSP */
                                            if( isCDSPClient )
                                            {
                                                adsppmClkReqData.pClkArray[clkNum].clkId = mmpmtoCdsppmClk[pClkInfo->clkId.clkIdCompute];
                                            }
                                            else if (isSlpiClient)
                                            {
                                                adsppmClkReqData.pClkArray[clkNum].clkId = mmpmtoSlpipmClk[pClkInfo->clkId.clkIdSlpi];
                                            }
                                            else
                                            {
												if (isLpassSlpiClient)
													adsppmClkReqData.pClkArray[clkNum].clkId = mmpmtoSlpipmClk[pClkInfo->clkId.clkIdSlpi];
												else
													adsppmClkReqData.pClkArray[clkNum].clkId = mmpmToAdsppmClk[pClkInfo->clkId.clkIdLpass];
                                            }

                                            adsppmClkReqData.pClkArray[clkNum].clkFreqHz = pClkInfo->clkFreqHz;
                                            adsppmClkReqData.pClkArray[clkNum].freqMatch = pClkInfo->freqMatch;

                                            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                                                "    clkId=%u, freqHz=%u, match=%u",
                                                adsppmClkReqData.pClkArray[clkNum].clkId,
                                                adsppmClkReqData.pClkArray[clkNum].clkFreqHz,
                                                adsppmClkReqData.pClkArray[clkNum].freqMatch);
                                        }
                                    }
                                    if(MMPM_STATUS_SUCCESS == *pStatus)
                                    {
                                        *pStatus = (MMPM_STATUS) RM_AddRequest(
                                            clientId, Adsppm_Rsc_Id_Core_Clk,
                                            &adsppmClkReqData, 0,
                                            syncRequest);
                                    }
                                }
                            }
                                break;

                            case MMPM_RSC_ID_SLEEP_LATENCY:
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Latency request: latency=%u",
                                    clientId, pParams->sleepMicroSec);

                                uint32 sleepMicroSec = pParams->sleepMicroSec;
                                adsppmRscId = Adsppm_Rsc_Id_Sleep_Latency;
                                if(sleepMicroSec == 0)
                                {
                                    // 0 is not valid to underlying sleep driver
                                    *pStatus = MMPM_STATUS_BADPARM;
                                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                        "bad parameter: sleepMicroSec=%u", sleepMicroSec);
                                }
                                else
                                {
                                    ADSPPM_QDSS_EVENT_2(ADSPPM_API_LATENCY_REQ, clientId, sleepMicroSec);
                                    *pStatus = (MMPM_STATUS) RM_AddRequest(
                                        clientId, Adsppm_Rsc_Id_Sleep_Latency,
                                        &sleepMicroSec, 0,
                                        syncRequest);
                                }
                            }
                                break;

                            case MMPM_RSC_ID_MIPS_EXT:
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: MIPS request", clientId);

                                adsppmRscId = Adsppm_Rsc_Id_Mips;
                                AdsppmQ6ClockRequestInfoType adsppmQ6ClockRequestData;
                                adsppmQ6ClockRequestData.usageType =
                                    Adsppm_Q6ClockRequestUsageType_Mips;

                                AdsppmMipsRequestType *adsppmMipsReqData =
                                        &(adsppmQ6ClockRequestData.AdsppmQ6ClockRequestType.mipsRequestData);
                                MmpmMipsReqType *pMipsParams = pParams->pMipsExt;

                                if((MMPM_BW_PORT_ID_MAX <= pMipsParams->codeLocation) ||
                                    (MMPM_MIPS_REQUEST_NONE == pMipsParams->reqOperation) ||
                                    (MMPM_MIPS_REQUEST_ENUM_MAX <= pMipsParams->reqOperation))
                                {
                                    *pStatus = MMPM_STATUS_BADPARM;
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                        "bad parameters in codelocation(%u) or reqoperation(%u)",
                                        pMipsParams->codeLocation,
                                        pMipsParams->reqOperation);
                                }
                                else
                                {
                                    memset(adsppmMipsReqData,0, sizeof(AdsppmMipsRequestType));
                                    // Make sure MMPM MIPS request data structure is
                                    // the same as ADSPPM mips request data structure
                                    memscpy(adsppmMipsReqData, sizeof(AdsppmMipsRequestType),
                                        pMipsParams, sizeof(MmpmMipsReqType));

                                    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                                        "    total=%u, floor=%u, location=%u, operation=%u",
                                        adsppmMipsReqData->mipsTotal,
                                        adsppmMipsReqData->mipsPerThread,
                                        adsppmMipsReqData->codeLocation,
                                        adsppmMipsReqData->reqOperation);
                                    ADSPPM_QDSS_EVENT_5(ADSPPM_API_MIPS_REQ,
                                        clientId, adsppmMipsReqData->mipsTotal,
                                        adsppmMipsReqData->mipsPerThread,
                                        adsppmMipsReqData->codeLocation,
                                        adsppmMipsReqData->reqOperation);

                                    *pStatus = (MMPM_STATUS) RM_AddRequest(
                                        clientId, Adsppm_Rsc_Id_Mips,
                                        &adsppmQ6ClockRequestData, 0,
                                        syncRequest);
                                }
                            }
                                break;

                            case MMPM_RSC_ID_MPPS:
                            {
                                adsppmRscId = Adsppm_Rsc_Id_Mips;
                                AdsppmQ6ClockRequestInfoType adsppmQ6ClockRequestData;
                                adsppmQ6ClockRequestData.usageType =
                                    Adsppm_Q6ClockRequestUsageType_Mpps;

                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: MPPS request", clientId);

                                AdsppmMppsRequestType *adsppmMppsReqData =
                                    &(adsppmQ6ClockRequestData.AdsppmQ6ClockRequestType.mppsRequestData);
                                MmpmMppsReqType *pMppsParams = pParams->pMppsReq;

                                memset(adsppmMppsReqData,0, sizeof(AdsppmMppsRequestType));
                                // Make sure MMPM MPPS request data structure is
                                // the same as ADSPPM mpps request data structure
                                memscpy(adsppmMppsReqData, sizeof(AdsppmMppsRequestType),
                                    pMppsParams, sizeof(MmpmMppsReqType));

                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "  total=%u,  floor=%u",
                                    adsppmMppsReqData->mppsTotal,
                                    adsppmMppsReqData->adspFloorClock);

                                ADSPPM_QDSS_EVENT_3(ADSPPM_API_MIPS_REQ,
                                    clientId, adsppmMppsReqData->mppsTotal,
                                    adsppmMppsReqData->adspFloorClock);

                                *pStatus = (MMPM_STATUS) RM_AddRequest(
                                    clientId, Adsppm_Rsc_Id_Mips,
                                    &adsppmQ6ClockRequestData, 0,
                                    syncRequest);
                            }
                                break;

                            case MMPM_RSC_ID_GENERIC_BW:
                            case MMPM_RSC_ID_GENERIC_BW_EXT:
                                adsppmRscId = Adsppm_Rsc_Id_BW;
                                *pStatus = apiAddBwRequest(
                                    clientId, pRequest->rscId, pParams->pGenBwReq, pRscExtParam->apiType, &asyncRscMap);
                                break;

                            case MMPM_RSC_ID_THERMAL:
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Thermal request: level=%u",
                                    clientId, pParams->thermalMitigation);

                                // Check and translate Thermal level to ADSPPM
                                AdsppmThermalType adsppmThermalLevel = (AdsppmThermalType) pParams->thermalMitigation;
                                adsppmRscId = Adsppm_Rsc_Id_Thermal;

                                *pStatus = (MMPM_STATUS) RM_AddRequest(
                                    clientId, Adsppm_Rsc_Id_Thermal,
                                    &adsppmThermalLevel, 0,
                                    syncRequest);

                            }
                                break;

                            case MMPM_RSC_ID_MEM_POWER:
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Memory power request", clientId);

                                AdsppmMemPowerReqParamType adsppmMemPwrReqData;
                                MmpmMemPowerReqParamType *pMemPwrParams = pParams->pMemPowerState;
                                adsppmRscId = Adsppm_Rsc_Id_MemPower;

                                if((MMPM_MEM_NONE == pMemPwrParams->memory) ||
                                    (MMPM_MEM_MAX <=  pMemPwrParams->memory) ||
                                    (MMPM_MEM_POWER_NONE == pMemPwrParams->powerState) ||
                                    (MMPM_MEM_POWER_MAX <= pMemPwrParams->powerState))
                                {
                                    *pStatus = MMPM_STATUS_BADPARM;
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                        "bad memoryid=%u or bad powerstate=%u",
                                        pMemPwrParams->memory, pMemPwrParams->powerState);
                                }
                                else
                                {
                                    //check for LPM retention feature
                                    if((MMPM_MEM_LPASS_LPM == pMemPwrParams->memory) &&
                                        (MMPM_MEM_POWER_RETENTION == pMemPwrParams->powerState) &&
                                        (AsicFeatureState_Disabled == ACM_GetFeatureStatus(AsicFeatureId_LPMRetention)))
                                    {
                                        *pStatus = MMPM_STATUS_UNSUPPORTED;
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                            "request LPM retention not supported");
                                        break;
                                    }
                                    else if(MMPM_MEM_OCMEM == pMemPwrParams->memory)
                                    {
                                        *pStatus = MMPM_STATUS_UNSUPPORTED;
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                            "request OCMEM retention not supported");
                                        break;
                                    }
                                    else
                                    {
                                        memset(&adsppmMemPwrReqData, 0, sizeof(AdsppmMemPowerReqParamType));
                                        // Make sure mmpm mem power request parameter is the same structure with adsppm
                                        memscpy(&adsppmMemPwrReqData, sizeof(AdsppmMemPowerReqParamType),
                                            pMemPwrParams, sizeof(AdsppmMemPowerReqParamType));
                                        ADSPPM_QDSS_EVENT_3(ADSPPM_API_MEM_PWR_REQ,
                                            clientId, adsppmMemPwrReqData.memory, adsppmMemPwrReqData.powerState);

                                        *pStatus = (MMPM_STATUS) RM_AddRequest(
                                            clientId, Adsppm_Rsc_Id_MemPower,
                                            &adsppmMemPwrReqData, 0,
                                            syncRequest);
                                    }
                                }
                            }
                                break;

                            case MMPM_RSC_ID_CORE_CLK_DOMAIN:
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: Clock Domain request", clientId);

                                AdsppmClkDomainReqType adsppmClkDomainReqData;
                                MmpmClkDomainReqType *pClkDomParams = pParams->pCoreClkDomain;
                                uint32 clkNum = 0;
                                adsppmRscId = Adsppm_Rsc_Id_Core_Clk_Domain;

                                if(ACM_GetFeatureStatus(AsicFeatureId_DomainCoreClocks) == AsicFeatureState_Disabled)
                                {
                                    *pStatus = MMPM_STATUS_UNSUPPORTED;
                                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                        "   Domain core clocks voting is not supported");
                                }
                                else
                                {
                                    if(MAX_NUM_OF_ADSPPM_CLOCK_ARRAY < pClkDomParams->numOfClk)
                                    {
                                        *pStatus = MMPM_STATUS_BADPARM;
                                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                            "numbofclk in clkdomain exceed max numofclk=%u",
                                            pClkDomParams->numOfClk);
                                    }
                                    else
                                    {
                                        memset(&adsppmClkDomainReqData, 0, sizeof(AdsppmClkDomainReqType));
                                        adsppmClkDomainReqData.numOfClk = pClkDomParams->numOfClk;
                                        // Translate clock Id to ADSPPM
                                        // and copy all the request values
                                        for(clkNum = 0; clkNum < pClkDomParams->numOfClk; clkNum++)
                                        {
                                            MmpmClkDomainType *pClkDomInfo = &pClkDomParams->pClkDomainArray[clkNum];
                                            if((MMPM_CLK_ID_LPASS_NONE == pClkDomInfo->clkId.clkIdLpass) ||
                                                (MMPM_CLK_ID_LPASS_ADSP_CORE <= pClkDomInfo->clkId.clkIdLpass) ||
                                                (MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE == pClkDomInfo->clkDomainSrc.clkDomainSrcIdLpass) ||
                                                (MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX <= pClkDomInfo->clkDomainSrc.clkDomainSrcIdLpass))
                                            {
                                                *pStatus = MMPM_STATUS_BADPARM;
                                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                                    "bad lpassclkid=%u or clkdomainsource=%u",
                                                    pClkDomInfo->clkId.clkIdLpass,
                                                    pClkDomInfo->clkDomainSrc.clkDomainSrcIdLpass);
                                                break;
                                            }
                                            else
                                            {
                                                adsppmClkDomainReqData.pClkDomainArray[clkNum].clkId =
                                                    mmpmToAdsppmClk[pClkDomInfo->clkId.clkIdLpass];
                                                adsppmClkDomainReqData.pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass =
                                                    (AdsppmClkDomainSrcIdLpassType) pClkDomInfo->clkDomainSrc.clkDomainSrcIdLpass;
                                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                                                    "    clkId=%u, clkfreq=%u, clockdomainsrc=%u",
                                                    adsppmClkDomainReqData.pClkDomainArray[clkNum].clkId,
                                                    adsppmClkDomainReqData.pClkDomainArray[clkNum].clkFreqHz,
                                                    adsppmClkDomainReqData.pClkDomainArray[clkNum].clkDomainSrc.clkDomainSrcIdLpass);
                                            }
                                        }
                                        if(MMPM_STATUS_SUCCESS == *pStatus)
                                        {
                                            *pStatus = (MMPM_STATUS) RM_AddRequest(
                                                clientId, Adsppm_Rsc_Id_Core_Clk_Domain,
                                                &adsppmClkDomainReqData, 0,
                                                syncRequest);
                                        }
                                    }
                                }
                            }
                                break;

                            default:
                                *pStatus = MMPM_STATUS_UNSUPPORTED;
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                    "Client %u: Unsupported resource %u requested",
                                    clientId, pRequest->rscId);
                                break;
                            }

                            if(MMPM_STATUS_SUCCESS == *pStatus)
                            {
                                *pStatus = apiProcessSingleRequest(adsppmRscId, pRscExtParam->apiType, &asyncRscMap);
                            }
                        }

                        if(MMPM_API_TYPE_ASYNC == pRscExtParam->apiType)
                        {
                            sts = (MMPM_STATUS) AM_AsyncRequest(clientId, asyncRscMap, pRscExtParam->reqTag);
                        }

                        // Update return sts
                        for(index = 0; index < pRscExtParam->numOfReq; index++)
                        {
                            if(MMPM_STATUS_SUCCESS != pRscExtParam->pStsArray[index])
                            {
                                sts = MMPM_STATUS_FAILED;
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR,
                                    "Request for rscId %u failed (%u) for client %u",
                                    pRscExtParam->pReqArray[index].rscId,
                                    pRscExtParam->pStsArray[index], clientId);
                            }
                        }
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Invalid number of requests in bundle %u",
                            pRscExtParam->numOfReq);
                    }
                } //pRscExtParam->pReqArray; pRscExtParam->pStsArray NULL pointer
            } //pRscExtParam NULL pointer
        }
        else
        {
            sts = MMPM_STATUS_NOTINITIALIZED;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/******************************************************************************
 *                     MMPM_Release_Ext
 *****************************************************************************/
MMPM_STATUS MMPM_Release_Ext(uint32 clientId, MmpmRscExtParamType *pRscExtParam)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    AdsppmRscIdType adsppmRscId = Adsppm_Rsc_Id_None;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        if(ADSPPM_IsInitialized())
        {
            if((NULL == pRscExtParam) || (0 == clientId))
            {
                sts = MMPM_STATUS_BADPARM;
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "NULL pointer for pRscExtParam or clientID=0, clientid=%u", clientId);
            }
            else
            {
                if((NULL == pRscExtParam->pReqArray) || (NULL == pRscExtParam->pStsArray))
                {
                    sts = MMPM_STATUS_NULLPOINTER;
                }
                else
                {
                    if(Adsppm_Rsc_Id_Max > pRscExtParam->numOfReq)
                    {
                        uint32 index = 0;
                        uint32 asyncRscMap = 0;
                        sts = MMPM_STATUS_SUCCESS;

                        for(index = 0; index < pRscExtParam->numOfReq; index++)
                        {
                            // Init return sts array
                            pRscExtParam->pStsArray[index] = MMPM_STATUS_SUCCESS;

                            uint32 extData = 0;
                            switch(pRscExtParam->pReqArray[index].rscId)
                            {
                            case MMPM_RSC_ID_POWER:
                                adsppmRscId = Adsppm_Rsc_Id_Power;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: power release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_PWR_REL, clientId);
                                break;

                            case MMPM_RSC_ID_REG_PROG:
                                adsppmRscId = Adsppm_Rsc_Id_BW;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: regprog release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_REG_REL, clientId);
                                extData = 1;
                                break;

                            case MMPM_RSC_ID_CORE_CLK:
                                adsppmRscId = Adsppm_Rsc_Id_Core_Clk;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: coreclk release", clientId);
                                break;

                            case MMPM_RSC_ID_SLEEP_LATENCY:
                                adsppmRscId = Adsppm_Rsc_Id_Sleep_Latency;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: sleeplatency release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_LATENCY_REL, clientId);
                                break;

                            case MMPM_RSC_ID_MIPS_EXT:
                            case MMPM_RSC_ID_MPPS:
                                adsppmRscId = Adsppm_Rsc_Id_Mips;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: mipsext release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_MIPS_REL, clientId);
                                break;

                            case MMPM_RSC_ID_GENERIC_BW:
                            case MMPM_RSC_ID_GENERIC_BW_EXT:
                                adsppmRscId = Adsppm_Rsc_Id_BW;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: genBW release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_BW_REL, clientId);
                                break;

                            case MMPM_RSC_ID_THERMAL:
                                adsppmRscId = Adsppm_Rsc_Id_Thermal;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: thermal release", clientId);
                                break;

                            case MMPM_RSC_ID_MEM_POWER:
                                adsppmRscId = Adsppm_Rsc_Id_MemPower;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: mempower release", clientId);
                                ADSPPM_QDSS_EVENT_1(ADSPPM_API_MEM_PWR_REL, clientId);
                                break;

                            case MMPM_RSC_ID_CORE_CLK_DOMAIN:
                                adsppmRscId = Adsppm_Rsc_Id_Core_Clk_Domain;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Client %u: clkdomain release", clientId);
                                break;

                            default:
                                sts = MMPM_STATUS_UNSUPPORTED;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                    "Unsupported rscId = %u", pRscExtParam->pReqArray[index].rscId);
                                break;
                            }

                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                                "Releasing rscId=%u for clientId %u", adsppmRscId, clientId);
                            pRscExtParam->pStsArray[index] = apiReleaseSingleResource(
                                clientId, adsppmRscId, extData, pRscExtParam->apiType, &asyncRscMap);
                        }

                        if(MMPM_API_TYPE_ASYNC == pRscExtParam->apiType)
                        {
                            sts = (MMPM_STATUS) AM_AsyncRequest(
                                clientId, asyncRscMap, pRscExtParam->reqTag);
                        }

                        // Update return sts
                        for(index = 0; index < pRscExtParam->numOfReq; index++)
                        {
                            if(MMPM_STATUS_SUCCESS != pRscExtParam->pStsArray[index])
                            {
                                sts = MMPM_STATUS_FAILED;
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR,
                                    "Release for rscId %u failed (%u) for client %u",
                                    pRscExtParam->pReqArray[index].rscId,
                                    pRscExtParam->pStsArray[index],
                                    clientId);
                            }
                        }
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Invalid number of requests in bundle %u", pRscExtParam->numOfReq);
                    }
                } // pRscExtParam->pReqArray, pRscExtParam->pStsArray NULL pointer
            } // pRscExtParam NULL pointer
        }
        else
        {
            sts = MMPM_STATUS_NOTINITIALIZED;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
    }
    else
    {
        sts = MMPM_STATUS_SUCCESS;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/******************************************************************************
 *                     MMPM_GetInfo
 *****************************************************************************/
MMPM_STATUS MMPM_GetInfo(MmpmInfoDataType *pInfoData)
{
    MMPM_STATUS sts = MMPM_STATUS_NOTINITIALIZED;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(gDISABLEADSPPMREQUESTFLAG)
    {
        ADSPPM_LOG_FUNC_PROFILEEND;
        return MMPM_STATUS_SUCCESS;
    }

    if(!ADSPPM_IsInitialized())
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        ADSPPM_LOG_FUNC_PROFILEEND;
        return MMPM_STATUS_NOTINITIALIZED;
    }

    if(pInfoData == NULL)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pInfoData is NULL");
        ADSPPM_LOG_FUNC_PROFILEEND;
        return MMPM_STATUS_BADPARM;
    }

    switch(pInfoData->infoId)
    {
    case MMPM_INFO_ID_CLK_FREQ:
    case MMPM_INFO_ID_CORE_CLK_MAX:
    {
        MmpmInfoClkFreqType* pInfoClkFreqType = NULL;
        AdsppmInfoClkFreqType clkInfoFreq;

        pInfoClkFreqType = pInfoData->info.pInfoClkFreqType;

        //ADSP_CORE being the special clock, internally used and represent Q6DSP
        if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled
           && pInfoClkFreqType->clkId != MMPM_CLK_ID_LPASS_ADSP_CORE)
        {
            if((MMPM_CLK_ID_COMPUTE_NONE == pInfoClkFreqType->clkId) ||
                (MMPM_CLK_ID_COMPUTE_ENUM_MAX <= pInfoClkFreqType->clkId))
            {
                sts = MMPM_STATUS_BADPARM;
                break;
            }
            clkInfoFreq.clkId = mmpmtoCdsppmClk[pInfoClkFreqType->clkId];
        }
        else if (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Enabled &&
                 pInfoClkFreqType->clkId != MMPM_CLK_ID_LPASS_ADSP_CORE)
        {
             if((MMPM_CLK_ID_SLPI_NONE == pInfoClkFreqType->clkId) ||
                 (MMPM_CLK_ID_SLPI_ENUM_MAX <= pInfoClkFreqType->clkId))
             {
                sts = MMPM_STATUS_BADPARM;
                break;
             }
            clkInfoFreq.clkId = mmpmtoSlpipmClk[pInfoClkFreqType->clkId];
        }
        else
        {
			if (ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled)
			{
				if (is_lpass_slpi_client(pInfoData->coreId) == TRUE)
				{
					if((MMPM_CLK_ID_SLPI_NONE == pInfoClkFreqType->clkId) ||
						(MMPM_CLK_ID_SLPI_ENUM_MAX <= pInfoClkFreqType->clkId))
					{
						sts = MMPM_STATUS_BADPARM;
						break;
					}
					clkInfoFreq.clkId = mmpmtoSlpipmClk[pInfoClkFreqType->clkId];
				}
				else
				{
					if((MMPM_CLK_ID_LPASS_NONE == pInfoClkFreqType->clkId) ||
						(MMPM_CLK_ID_LPASS_ENUM_MAX <= pInfoClkFreqType->clkId))
					{
						sts = MMPM_STATUS_BADPARM;
						break;
					}
					clkInfoFreq.clkId = mmpmToAdsppmClk[pInfoClkFreqType->clkId];
				}
			}
			else /* Falls in this case for normal lpass case */
			{
				if((MMPM_CLK_ID_LPASS_NONE == pInfoClkFreqType->clkId) ||
					(MMPM_CLK_ID_LPASS_ENUM_MAX <= pInfoClkFreqType->clkId))
				{
					sts = MMPM_STATUS_BADPARM;
					break;
				}
				clkInfoFreq.clkId = mmpmToAdsppmClk[pInfoClkFreqType->clkId];
			}
        }

        clkInfoFreq.forceMeasure = pInfoClkFreqType->forceMeasure;
        clkInfoFreq.clkFreqHz = 0;

        if(MMPM_INFO_ID_CLK_FREQ == pInfoData->infoId)
        {
            sts = (MMPM_STATUS) RM_GetResourceInfo(Adsppm_Rsc_Id_Core_Clk,
                (void *) &clkInfoFreq, Adsppm_Info_Type_Current_Value);
        }
        else
        {
            sts = (MMPM_STATUS) RM_GetResourceInfo(Adsppm_Rsc_Id_Core_Clk,
                (void *) &clkInfoFreq, Adsppm_Info_Type_Max_Value);
        }
        if(sts == MMPM_STATUS_SUCCESS)
        {
            pInfoClkFreqType->clkFreqHz = clkInfoFreq.clkFreqHz;
        }
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "Freq: %u", pInfoClkFreqType->clkFreqHz);
        break;
    }

    case MMPM_INFO_ID_MIPS_MAX:
    {
        uint32 mipsValue = 0;

        sts = (MMPM_STATUS) RM_GetResourceInfo(Adsppm_Rsc_Id_Mips,
            (void *) &mipsValue, Adsppm_Info_Type_Max_Value);
        if(sts == MMPM_STATUS_SUCCESS)
        {
            pInfoData->info.mipsValue= mipsValue;
        }
        break;
    }

    case MMPM_INFO_ID_MPPS:
        sts = (MMPM_STATUS) RM_GetResourceInfo(Adsppm_Rsc_Id_Mips,
            (void*) pInfoData->info.pMppsInfo, Adsppm_Info_Type_Current_Value);
        break;

    case MMPM_INFO_ID_BW_MAX:
    {
        uint64 bwValue = 0;

        sts = (MMPM_STATUS) RM_GetResourceInfo(Adsppm_Rsc_Id_BW,
            (void *) &bwValue, Adsppm_Info_Type_Max_Value);
        if(sts == MMPM_STATUS_SUCCESS)
        {
            pInfoData->info.bwValue= bwValue;
        }
        break;
    }

    case MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS:
        pInfoData->info.aggregateClientClass = CM_GetActiveUseCaseClasses();
        sts = MMPM_STATUS_SUCCESS;
        break;

    case MMPM_INFO_ID_BW_EXT:
    {
        AdsppmCoreCtxType* pAdsppmCoreCtx = GetAdsppmCoreContext();
        MmpmBusRouteType* pMmpmBusRoute;
        AdsppmBusRouteType busRoute;

        if((pInfoData == NULL) || (pInfoData->info.pBwExtInfo == NULL))
        {
            sts = MMPM_STATUS_BADPARM;
            break;
        }

        // Translate MMPM bus route to ADSPPM bus route
        pMmpmBusRoute = &pInfoData->info.pBwExtInfo->busRoute,
        busRoute.masterPort = mmpmToAdsppmBusPort[pMmpmBusRoute->masterPort];
        busRoute.slavePort = mmpmToAdsppmBusPort[pMmpmBusRoute->slavePort];

        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "infoId: MMPM_INFO_ID_BW_EXT, masterPort: %s (%u), "
            "slavePort: %s (%u)",
            adsppm_getBusPortName(busRoute.masterPort), busRoute.masterPort,
            adsppm_getBusPortName(busRoute.slavePort), busRoute.slavePort);

        sts = (MMPM_STATUS) BusMgr_GetCompensatedBwInfo(
            &pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_BW-1], &busRoute,
            &pInfoData->info.pBwExtInfo->totalBw /* result */ );

        break;
    }

    case MMPM_INFO_ID_DCVS_STATE:
        pInfoData->info.dcvsState = DcvsMgr_GetDcvsEnabledDisabledFlag();
        sts = MMPM_STATUS_SUCCESS;
        break;

    case MMPM_INFO_ID_EXT_REQ:
    {
        AdsppmInfoPrivateType* privateInfo =
            (AdsppmInfoPrivateType*) pInfoData->info.pExtInfo;

        if(privateInfo == NULL)
        {
            sts = MMPM_STATUS_BADPARM;
            break;
        }

        switch(privateInfo->type)
        {
            case ADSPPM_PRIVATE_INFO_AGGREGATED_BW:
            {
                AdsppmInfoAggregatedBwType ddrBwInfo;
                AdsppmInfoAggregatedBwType *bwInfo;
                bwInfo =
                    (AdsppmInfoAggregatedBwType *) &privateInfo->aggregatedBw;
                sts = (MMPM_STATUS) RM_GetResourceInfo(
                    Adsppm_Rsc_Id_BW,
                    (void *) &ddrBwInfo,
                    Adsppm_Info_Type_Current_Value);
                if(MMPM_STATUS_SUCCESS == sts)
                {
                    *bwInfo = ddrBwInfo;
                }
                break;
            }
            case ADSPPM_PRIVATE_INFO_AHB:
            {
                AdsppmInfoAhbType ahbInfo;
                sts = (MMPM_STATUS) AHBM_GetInfo(&ahbInfo);
                if(MMPM_STATUS_SUCCESS == sts)
                {
                    privateInfo->ahb = ahbInfo; // copy result
                }
                break;
            }

            case ADSPPM_PRIVATE_INFO_HCPDDR_BW:
            case ADSPPM_PRIVATE_INFO_DMADDR_BW:
            {
                AdsppmInfoDcvsAdspDdrBwType coreDdrBw;

                if(ADSPPM_PRIVATE_INFO_HCPDDR_BW == privateInfo->type)
                {
                    sts = (MMPM_STATUS) ExtBwMgr_GetInfo(&coreDdrBw, Adsppm_DCVS_Route_HCP_DDR);
                }
                else if(ADSPPM_PRIVATE_INFO_DMADDR_BW == privateInfo->type)
                {
                    sts = (MMPM_STATUS) ExtBwMgr_GetInfo(&coreDdrBw, Adsppm_DCVS_Route_DMA_DDR);
                }
                else
                {
                    sts = MMPM_STATUS_BADPARM;
                }

                if(MMPM_STATUS_SUCCESS == sts)
                {
                    privateInfo->adspDdrBw = coreDdrBw; // copy result
                }
                break;
            }

            case ADSPPM_PRIVATE_INFO_ADSPDDR_BW:
            {
                AdsppmInfoDcvsAdspDdrBwType adspDdrBw;

                sts = (MMPM_STATUS) ExtBwMgr_GetInfo(&adspDdrBw, Adsppm_DCVS_Route_DSP_DDR);
                if(MMPM_STATUS_SUCCESS == sts)
                {
                    privateInfo->adspDdrBw = adspDdrBw; // copy result
                }
                break;
            }
            case ADSPPM_PRIVATE_INFO_ADSPCLOCK:
            {
                AdsppmInfoDcvsAdspClockType adspClock;
                sts = (MMPM_STATUS) AdspClkMgr_GetInfo(&adspClock);
                if(MMPM_STATUS_SUCCESS == sts)
                {
                    privateInfo->adspClock = adspClock; // copy result
                }
                break;
            }
            default:
                sts = MMPM_STATUS_UNSUPPORTED;
                break;
        }

        break;
    }

    default:
        sts = MMPM_STATUS_UNSUPPORTED;
        break;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/******************************************************************************
 *                     MMPM_SetParameter
 *****************************************************************************/
MMPM_STATUS MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfigData)
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        if(ADSPPM_IsInitialized())
        {
            if((NULL == pParamConfigData) || (0 == clientId))
            {
                sts = MMPM_STATUS_BADPARM;
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "NULL pointer for pParamConfigData or clientid is 0, clientId=%u", clientId);
            }
            else
            {
                AdsppmParameterConfigType paramConfig;
                MmpmMemoryMapType *pMemoryMap;
                switch(pParamConfigData->paramId)
                {
                case MMPM_PARAM_ID_RESOURCE_LIMIT:
                    paramConfig.paramId = Adsppm_Param_Id_Resource_Limit;
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                        "Setting parameter %u for clientId %u",
                        paramConfig.paramId, clientId);
                    sts = (MMPM_STATUS) CM_SetParameter(clientId, &paramConfig);
                    break;

                case MMPM_PARAM_ID_CLIENT_OCMEM_MAP:
                    sts = MMPM_STATUS_UNSUPPORTED;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "OCMEM mapping in not supported");
                    break;

                case MMPM_PARAM_ID_MEMORY_MAP:
                    paramConfig.paramId = Adsppm_Param_Id_Memory_Map;
                    paramConfig.pParamConfig = pParamConfigData->pParamConfig;
                    pMemoryMap = (MmpmMemoryMapType *) pParamConfigData->pParamConfig;
                    if(NULL != pMemoryMap)
                    {
                        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                            "Setting memory map for clientId %u, memory %u: addr = 0x%X, size = %u",
                            clientId,
                            pMemoryMap->memory,
                            (uint32)pMemoryMap->startAddress,
                            pMemoryMap->size);
                        sts = (MMPM_STATUS) ACM_SetVirtMemAddressRange(
                            (AdsppmMemIdType) pMemoryMap->memory,
                            pMemoryMap->startAddress,
                            pMemoryMap->size);
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                            "Parameter pointer is NULL");
                    }
                    break;

                case MMPM_PARAM_ID_CLIENT_CLASS:
                {
                    MmpmClientClassType* clientClass =
                        (MmpmClientClassType*) pParamConfigData->pParamConfig;
                    if(clientClass)
                    {
                        sts = (MMPM_STATUS) CM_SetProperty(clientId,
                            CM_CLASS_PROPERTY, *clientClass);
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                            "Parameter pointer is NULL");
                    }
                    break;
                }
                case MMPM_PARAM_ID_L2_CACHE_LINE_LOCK:
                {
                    MmpmL2CacheLineLockParameterType* lockParam =
                        (MmpmL2CacheLineLockParameterType*)
                        pParamConfigData->pParamConfig;
                    if(lockParam)
                    {
                        const AdsppmClientType* client =
                            CM_GetClient(clientId);
                        if(client)
                        {
                            Adsppm_Status lockStatus = CPM_SetLineLocking(
                                &client->cacheLockParam, // existing params
                                lockParam);              // new params request
                            if(lockStatus == Adsppm_Status_Success)
                            {
                                sts = (MMPM_STATUS) CM_SetProperty(clientId,
                                    CM_CACHE_LINE_LOCK_PROPERTY,
                                    (uint32) lockParam);
                            }
                            else
                            {
                                sts = MMPM_STATUS_FAILED;
                            }
                        }
                        else
                        {
                            sts = MMPM_STATUS_BADPARM;
                        }
                    }
                    else
                    {
                        sts = MMPM_STATUS_BADPARM;
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                            "Parameter pointer is NULL");
                    }
                    break;
                }
                case MMPM_PARAM_ID_DCVS_PARTICIPATION:
                {
                    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
                        AsicFeatureState_Enabled)
                    {
                        MmpmDcvsParticipationType* dcvsParticipation =
                            (MmpmDcvsParticipationType*) pParamConfigData->pParamConfig;
                        if(dcvsParticipation)
                        {
                            sts = (MMPM_STATUS) CM_SetProperty(clientId,
                                CM_DCVS_PARTICIPATION_PROPERTY,
                                (uint32) dcvsParticipation);
                            //sts = (MMPM_STATUS) CM_SetProperty(clientId,
                            //         CM_ADSP_DDR_BW_REQ_ACTIVE_PROPERTY,
                            //       (boolean) dcvsParticipation->enable);

                            // Need to update BW and MIPS aggregation
                            if(sts == MMPM_STATUS_SUCCESS)
                            {
                                sts = (MMPM_STATUS) RM_RequestResource(Adsppm_Rsc_Id_BW);

                                if(sts == MMPM_STATUS_SUCCESS)
                                {
                                    sts = (MMPM_STATUS) RM_RequestResource(Adsppm_Rsc_Id_Mips);
                                }
                            }
                        }
                        else
                        {
                            sts = MMPM_STATUS_BADPARM;
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                "Parameter pointer is NULL");
                        }
                    }
                    break;
                }
                case MMPM_PARAM_ID_SYSCACHE_STREAM_ID:
                {
                    if(ACM_GetFeatureStatus(AsicFeatureId_SystemCacheSupport) ==
                        AsicFeatureState_Enabled)
                    {
                        MmpmSysCacheStreamId* pStreamIdMask =
                            (MmpmSysCacheStreamId*) pParamConfigData->pParamConfig;
                        if(pStreamIdMask)
                        {
                          sts = (MMPM_STATUS) CM_SetProperty(clientId,
                            CM_STREAM_ID_PROPERTY, pStreamIdMask->streamIdMask);
                        }
                        else
                        {
                            sts = MMPM_STATUS_BADPARM;
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                "Parameter pointer is NULL");
                        }
                    }
                    break;
                }

                default:
                    sts = MMPM_STATUS_UNSUPPORTED;
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Unsupported paramId = %u", pParamConfigData->paramId);
                    break;
                }
            }
        }
        else
        {
            sts = MMPM_STATUS_NOTINITIALIZED;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
    }
    else
    {
        sts = MMPM_STATUS_SUCCESS;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/******************************************************************************
 *                     MMPM_Init
 *****************************************************************************/
uint32 MMPM_Init(char *cmd_line)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    if(!gDISABLEADSPPMREQUESTFLAG)
    {
        // Do initialization only once. This cannot be protected using critical section
        // since this is done before initializing the lock for critical section
        if(!ADSPPM_IsInitialized())
        {
            sts = (MMPM_STATUS) ADSPPM_Init();
        }
    }
    else
    {
        sts = MMPM_STATUS_SUCCESS;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return (uint32)sts;
}

// For CoreBSP image initialization
void MMPM_Init_Ext(void)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    MMPM_Init(NULL);
    ADSPPM_LOG_FUNC_PROFILEEND;
}


/******************************************************************************
 *                     MMPM_Deinit
 *****************************************************************************/
/**
 * De-initialize core MMPM modules
 */
void MMPM_Deinit(void)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    ADSPPM_LOG_FUNC_PROFILEEND;
    return;
}


/**
 * Async API completion callback notification
 */
uint32 API_doClientCallback(AsyncMgrPipeDataType *receiveData)
{
    uint32 result = 0;
    AdsppmCallbackParamType callbackParam;
    MmpmCompletionCallbackDataType callbackData;
    const AdsppmClientType *pClient = NULL;
    ADSPPM_LOG_FUNC_PROFILESTART;

    pClient = CM_GetClient(receiveData->clientId);

    if(pClient != NULL && pClient->callBack)
    {
        memset(&callbackParam, 0, sizeof(AdsppmCallbackParamType));
        callbackData.reqTag = receiveData->reqTag;
        callbackData.result = receiveData->result;
        callbackParam.eventId = Adsppm_Callback_Event_Id_Async_Complete;
        callbackParam.clientId = receiveData->clientId;
        callbackParam.callbackDataSize = sizeof(callbackData);
        callbackParam.callbackData = &callbackData;

        // Call client's call back function
        result = pClient->callBack(&callbackParam);
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * @func API_Init - Initialize all components within ADSPPM API
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status API_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    result = AM_AsyncInit();
    if(Adsppm_Status_Success == result)
    {
        gADSPPMINITSTATE |= Adsppm_State_CoreAMAsyncInit;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/******************************************************************************
 *                     ADSPPM-DCVS Interface
 *****************************************************************************/
 ADSPPM_DCVS_STATUS adsppm_dcvs_request(
    AdsppmDcvsResourceId resourceId,
    AdsppmDcvsResourceVoteType *resourceVote)
{
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
        AsicFeatureState_Enabled)
    {
        if((resourceId == ADSPPM_DCVS_RSC_ID_NONE) ||
            (resourceId >= ADSPPM_DCVS_RSC_ID_MAX))
        {
            ADSPPM_LOG_FUNC_PROFILEEND;
            return ADSPPM_DCVS_STATUS_BADPARM;
        }

        if(resourceVote == NULL)
        {
            ADSPPM_LOG_FUNC_PROFILEEND;
            return ADSPPM_DCVS_STATUS_BADPARM;
        }

        switch(resourceId)
        {
        case ADSPPM_DCVS_RSC_ID_ADSP_CLOCK:
        {
            uint32 adspClockHz;

            adspClockHz = resourceVote->adspClockInHz;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                "New DCVS request: clk=%u", adspClockHz);

            DcvsMgr_SysMonDcvsAdspClockRequest(adspClockHz);
            break;
        }

        case ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW:
        {
            AdsppmBusBWDataIbAbType adsp2DdrBwIbAb;

            adsp2DdrBwIbAb.Ib = resourceVote->adsp2DdrBwInIbAb.iBw;
            adsp2DdrBwIbAb.Ab = resourceVote->adsp2DdrBwInIbAb.aBw;
            adsp2DdrBwIbAb.latencyNs = 0;

            ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                "New DCVS request: ib=%llu, ab=%llu",
                (uint32) adsp2DdrBwIbAb.Ib,
                (uint32) (adsp2DdrBwIbAb.Ib >> 32),
                (uint32) adsp2DdrBwIbAb.Ab,
                (uint32) (adsp2DdrBwIbAb.Ib >> 32));

            DcvsMgr_SysMonDcvsAdsp2DdrBwRequest(&adsp2DdrBwIbAb);
            break;
        }

        case ADSPPM_DCVS_RSC_ID_HCP_DDR_BW:
        case ADSPPM_DCVS_RSC_ID_DMA_DDR_BW:
        {
            AdsppmBusBWDataIbAbType vapss2DdrBwIbAb;

            vapss2DdrBwIbAb.Ib = resourceVote->vapss2DdrBwInIbAb.iBw;
            vapss2DdrBwIbAb.Ab = resourceVote->vapss2DdrBwInIbAb.aBw;
            vapss2DdrBwIbAb.latencyNs = 0;

            ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                "New VAPSS->DCVS request %u: ib=%llu, ab=%llu", resourceId,
                (uint32) vapss2DdrBwIbAb.Ib,
                (uint32) (vapss2DdrBwIbAb.Ib >> 32),
                (uint32) vapss2DdrBwIbAb.Ab,
                (uint32) (vapss2DdrBwIbAb.Ib >> 32));

            DcvsMgr_SysMonDcvsVapss2DdrBwRequest(resourceId,
                                                 &vapss2DdrBwIbAb);
            break;
        }


        default:
            ADSPPM_LOG_FUNC_PROFILEEND;
            return ADSPPM_DCVS_STATUS_FAILED;
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return ADSPPM_DCVS_STATUS_SUCCESS;

}

/******************************************************************************
 *                     ADSPPM-Limits Interface
 *****************************************************************************/
 ADSPPM_LIMIT_STATUS adsppm_limit_request(
    AdsppmLimitResourceId resourceId,
    AdsppmLimitResourceVoteType *resourceVote)
{
    ADSPPM_LOG_FUNC_PROFILESTART;

	if((resourceId == ADSPPM_LIMIT_RSC_ID_NONE) ||
		(resourceId >= ADSPPM_LIMIT_RSC_ID_MAX))
	{
		ADSPPM_LOG_FUNC_PROFILEEND;
		return ADSPPM_LIMIT_STATUS_BADPARM;
	}

	if(resourceVote == NULL)
	{
		ADSPPM_LOG_FUNC_PROFILEEND;
		return ADSPPM_LIMIT_STATUS_BADPARM;
	}

	switch(resourceId)
	{
	case ADSPPM_LIMIT_RSC_ID_ADSP_CLOCK_VOTE:
	{
		uint32 adspClockHz;

		adspClockHz = resourceVote->adspClockInHz;
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
			"New Limit request: Max ADSP clk=%u", adspClockHz);

		adspClkMgr_AdspMaxClockVoteUpdate(adspClockHz);
		break;
	}
	default:
		ADSPPM_LOG_FUNC_PROFILEEND;
		return ADSPPM_LIMIT_STATUS_FAILED;
	}

    ADSPPM_LOG_FUNC_PROFILEEND;
    return ADSPPM_LIMIT_STATUS_SUCCESS;

}


/******************************************************************************
 *                     MMPM_SysCacheActDeact()
 *****************************************************************************/
MMPM_STATUS MMPM_SysCacheActDeact(uint32 scid, boolean isActivated)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    Adsppm_Status tempResult = Adsppm_Status_Success;

    if(isActivated)
    {
        tempResult = syscacheMgrIncrementScidCount(scid);
    }
    else
    {
        tempResult = syscacheMgrDecrementScidCount(scid);
    }

    /* If no error found, we return success */
    if(tempResult == Adsppm_Status_Success)
    {
        sts = MMPM_STATUS_SUCCESS;
    }
    return sts;
}


/******************************************************************************
 *                     Helper functions
 *****************************************************************************/

AdsppmBusPortIdType AdsppmMapMMPMPortIdToADSPPMPortID(MmpmBwPortIdType mmpmPort)
{
   return mmpmToAdsppmBusPort[mmpmPort];
}


