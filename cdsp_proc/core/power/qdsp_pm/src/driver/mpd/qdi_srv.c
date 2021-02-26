/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: qdi_srv.c
@brief: Implementation of the QDI driver in the guest OS.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/mpd/qdi_srv.c#1 $
*/

#include <limits.h>
#include "stdlib.h"
#include "qurt.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "adsppm_qdi.h"
#include "mmpm.h"
#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "adsppmcb.h"
#include "asyncmgr.h"
#include "client_id_list.h"


//#define ADSPPMQDI_ULOG 1

// Macro to align an address to the nearest 8-byte boundary
#define ADSPPM_ALIGN8(val) (((val) + 7) & ~0x00000007)


#define COMPARE_PARAMETERS(fieldName, pd, os) \
if(pd != os) \
{ULOG_RT_PRINTF_2(gAdsppm.hLog, \
"    [ERROR] "fieldName": userPD=%u != guestOS=%u", pd, os);}

typedef struct
{
    qurt_qdi_obj_t qdiObj; // must be first field
    ClientIdList clientList;
} AdsppmPdState;

QDI_Adsppm_Ctx_t gAdsppm;


void print_request_data(
    int client_handle,
    MmpmRscExtParamType *pUserReq,
    MmpmRscExtParamType *pReqData)
{
#ifdef ADSPPMQDI_ULOG
    int i, j;
    if((NULL != pUserReq) && (NULL != gAdsppm.hLog))
    {
        ULOG_RT_PRINTF_3(gAdsppm.hLog,
            "print_request_data: User PD, client_handle=%d, apiType=%u, numOfReq=%u",
            client_handle, pUserReq->apiType, pUserReq->numOfReq);
        COMPARE_PARAMETERS("apiType", pUserReq->apiType, pReqData->apiType);
        COMPARE_PARAMETERS("numOfReq", pUserReq->numOfReq, pReqData->numOfReq);

        if(NULL != pUserReq->pReqArray)
        {
            for(i = 0; i < pUserReq->numOfReq; i++)
            {
                ULOG_RT_PRINTF_1(gAdsppm.hLog,
                    "    User PD rscId, rscId=%u", pUserReq->pReqArray[i].rscId);
                COMPARE_PARAMETERS("rscId",
                    pUserReq->pReqArray[i].rscId,
                    pReqData->pReqArray[i].rscId);

                switch(pUserReq->pReqArray[i].rscId)
                {
                case MMPM_RSC_ID_MIPS_EXT:
                {
                    MmpmMipsReqType *pPDpMipsExt = pUserReq->pReqArray[i].rscParam.pMipsExt;
                    MmpmMipsReqType *pOSpMipsExt = pReqData->pReqArray[i].rscParam.pMipsExt;

                    if(NULL != pPDpMipsExt)
                    {
                        ULOG_RT_PRINTF_4(gAdsppm.hLog,
                            "    MIPS: User PD pMipsExt, mipsTotal=%u, mipsPerThread=%u, codeLocation=%u, reqOperation=%u",
                            pPDpMipsExt->mipsTotal,
                            pPDpMipsExt->mipsPerThread,
                            pPDpMipsExt->codeLocation,
                            pPDpMipsExt->reqOperation);
                        COMPARE_PARAMETERS("mipsTotal",
                            pPDpMipsExt->mipsTotal,
                            pOSpMipsExt->mipsTotal);
                        COMPARE_PARAMETERS("mipsPerThread",
                            pPDpMipsExt->mipsPerThread,
                            pOSpMipsExt->mipsPerThread);
                        COMPARE_PARAMETERS("codeLocation",
                            pPDpMipsExt->codeLocation,
                            pOSpMipsExt->codeLocation);
                        COMPARE_PARAMETERS("reqOperation",
                            pPDpMipsExt->reqOperation,
                            pOSpMipsExt->reqOperation);
                    }
                    break;
                }
                case MMPM_RSC_ID_MPPS:
                {
                    MmpmMppsReqType *pPDpMppsReq = pUserReq->pReqArray[i].rscParam.pMppsReq;
                    MmpmMppsReqType *pOSpMppsReq = pReqData->pReqArray[i].rscParam.pMppsReq;

                    if(NULL != pPDpMppsReq)
                    {
                        ULOG_RT_PRINTF_2(gAdsppm.hLog,
                            "    MPPS: User PD pMppsReq, mppsTotal=%u, adspFloorClock=%u",
                            pPDpMppsReq->mppsTotal,
                            pPDpMppsReq->adspFloorClock);
                        COMPARE_PARAMETERS("mppsTotal",
                            pPDpMppsReq->mppsTotal,
                            pOSpMppsReq->mppsTotal);
                        COMPARE_PARAMETERS("adspFloorClock",
                            pPDpMppsReq->adspFloorClock,
                            pOSpMppsReq->adspFloorClock);
                    }
                    break;
                }
                case MMPM_RSC_ID_CORE_CLK:
                {
                    MmpmClkReqType *pPDpCoreClk = pUserReq->pReqArray[i].rscParam.pCoreClk;
                    MmpmClkReqType *pOSpCoreClk = pReqData->pReqArray[i].rscParam.pCoreClk;

                    if((NULL != pPDpCoreClk) && (NULL != pPDpCoreClk->pClkArray))
                    {
                        ULOG_RT_PRINTF_1(gAdsppm.hLog,
                            "    CORE CLOCK: User PD pCoreClk, numOfClk=%u",
                            pPDpCoreClk->numOfClk);
                        COMPARE_PARAMETERS("numOfClk",
                            pPDpCoreClk->numOfClk,
                            pOSpCoreClk->numOfClk);

                        for(j = 0; j < pPDpCoreClk->numOfClk; j++)
                        {
                            ULOG_RT_PRINTF_4(gAdsppm.hLog,
                                "        User PD pClkArray[%u], clkId=%u, clkFreqHz=%u, freqMatch=%u", j,
                                pPDpCoreClk->pClkArray[j].clkId.clkIdLpass,
                                pPDpCoreClk->pClkArray[j].clkFreqHz,
                                pPDpCoreClk->pClkArray[j].freqMatch);
                            COMPARE_PARAMETERS("clkId",
                                pPDpCoreClk->pClkArray[j].clkId.clkIdLpass,
                                pOSpCoreClk->pClkArray[j].clkId.clkIdLpass);
                            COMPARE_PARAMETERS("clkFreqHz",
                                pPDpCoreClk->pClkArray[j].clkFreqHz,
                                pOSpCoreClk->pClkArray[j].clkFreqHz);
                            COMPARE_PARAMETERS("freqMatch",
                                pPDpCoreClk->pClkArray[j].freqMatch,
                                pOSpCoreClk->pClkArray[j].freqMatch);
                        }
                    }
                    break;
                }
                case MMPM_RSC_ID_GENERIC_BW:
                case MMPM_RSC_ID_GENERIC_BW_EXT:
                {
                    MmpmGenBwReqType *pPDpGenBwReq = pUserReq->pReqArray[i].rscParam.pGenBwReq;
                    MmpmGenBwReqType *pOSpGenBwReq = pReqData->pReqArray[i].rscParam.pGenBwReq;

                    if((NULL != pPDpGenBwReq) && (NULL != pPDpGenBwReq->pBandWidthArray))
                    {
                        ULOG_RT_PRINTF_1(gAdsppm.hLog,
                            "    BANDWIDTH: User PD pGenBwReq, numOfBw=%u",
                            pPDpGenBwReq->numOfBw);
                        COMPARE_PARAMETERS("numOfBw",
                            pPDpGenBwReq->numOfBw,
                            pOSpGenBwReq->numOfBw);

                        for(j = 0; j < pPDpGenBwReq->numOfBw; j++)
                        {

                            ULOG_RT_PRINTF_6(gAdsppm.hLog,
                                "        User PD pBandWidthArray[%u], masterPort=%u, slavePort=%u, bwBytePerSec=%u, usagePercentage=%u, usageType=%u",
                                j,
                                pPDpGenBwReq->pBandWidthArray[j].busRoute.masterPort,
                                pPDpGenBwReq->pBandWidthArray[j].busRoute.slavePort,
                                (uint32) pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.bwBytePerSec,
                                pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usagePercentage,
                                pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usageType);
                            COMPARE_PARAMETERS("masterPort",
                                pPDpGenBwReq->pBandWidthArray[j].busRoute.masterPort,
                                pOSpGenBwReq->pBandWidthArray[j].busRoute.masterPort);
                            COMPARE_PARAMETERS("slavePort",
                                pPDpGenBwReq->pBandWidthArray[j].busRoute.slavePort,
                                pOSpGenBwReq->pBandWidthArray[j].busRoute.slavePort);
                            COMPARE_PARAMETERS("bwBytePerSec",
                                (uint32) pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.bwBytePerSec,
                                (uint32) pOSpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.bwBytePerSec);
                            COMPARE_PARAMETERS("usagePercentage",
                                pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usagePercentage,
                                pOSpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usagePercentage);
                            COMPARE_PARAMETERS("usageType",
                                pPDpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usageType,
                                pOSpGenBwReq->pBandWidthArray[j].bwValue.busBwValue.usageType);
                        }
                    }
                    break;
                }
                case MMPM_RSC_ID_MEM_POWER:
                {
                    MmpmMemPowerReqParamType *pPDpMemPowerState = pUserReq->pReqArray[i].rscParam.pMemPowerState;
                    MmpmMemPowerReqParamType *pOSpMemPowerState = pReqData->pReqArray[i].rscParam.pMemPowerState;

                    if(NULL != pPDpMemPowerState)
                    {
                        ULOG_RT_PRINTF_2(gAdsppm.hLog,
                            "    MEMORY POWER: User PD pMemPowerState, memory=%u, powerState=%u",
                            pPDpMemPowerState->memory,
                            pPDpMemPowerState->powerState);
                        COMPARE_PARAMETERS("memory",
                            pPDpMemPowerState->memory,
                            pOSpMemPowerState->memory);
                        COMPARE_PARAMETERS("powerState",
                            pPDpMemPowerState->powerState,
                            pOSpMemPowerState->powerState);
                    }
                    break;
                }
                case MMPM_RSC_ID_CORE_CLK_DOMAIN:
                {
                    MmpmClkDomainReqType *pPDpCoreClkDomain = pUserReq->pReqArray[i].rscParam.pCoreClkDomain;
                    MmpmClkDomainReqType *pOSpCoreClkDomain = pReqData->pReqArray[i].rscParam.pCoreClkDomain;

                    if((NULL != pPDpCoreClkDomain) && (NULL != pPDpCoreClkDomain->pClkDomainArray))
                    {
                        ULOG_RT_PRINTF_1(gAdsppm.hLog,
                            "    CORE CLOCK DOMAIN: User PD pCoreClkDomain, numOfClk=%u",
                            pPDpCoreClkDomain->numOfClk);
                        COMPARE_PARAMETERS("numOfClk",
                            pPDpCoreClkDomain->numOfClk,
                            pOSpCoreClkDomain->numOfClk);

                        for(j = 0; j < pPDpCoreClkDomain->numOfClk; j++)
                        {
                            ULOG_RT_PRINTF_4(gAdsppm.hLog,
                                "    User PD pCoreClkDomain[%u], clkId=%u, clkFreqHz=%u, clkDomainSrcIdLpass=%u", j,
                                pPDpCoreClkDomain->pClkDomainArray[j].clkId.clkIdLpass,
                                pPDpCoreClkDomain->pClkDomainArray[j].clkFreqHz,
                                pPDpCoreClkDomain->pClkDomainArray[j].clkDomainSrc.clkDomainSrcIdLpass);
                            COMPARE_PARAMETERS("clkId",
                                pPDpCoreClkDomain->pClkDomainArray[j].clkId.clkIdLpass,
                                pOSpCoreClkDomain->pClkDomainArray[j].clkId.clkIdLpass);
                            COMPARE_PARAMETERS("clkFreqHz",
                                pPDpCoreClkDomain->pClkDomainArray[j].clkFreqHz,
                                pOSpCoreClkDomain->pClkDomainArray[j].clkFreqHz);
                            COMPARE_PARAMETERS("clkDomainSrcIdLpass",
                                pPDpCoreClkDomain->pClkDomainArray[j].clkDomainSrc.clkDomainSrcIdLpass,
                                pOSpCoreClkDomain->pClkDomainArray[j].clkDomainSrc.clkDomainSrcIdLpass);
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
#endif
}


void print_setparam_data(
    MmpmParameterConfigType *pUserParamData,
    MmpmParameterConfigType *pParamData)
{
#ifdef ADSPPMQDI_ULOG
    //TODO
#endif
}


static void print_getinfo_data(
    MmpmInfoDataType *pUserInfoData,
    MmpmInfoDataType *pInfoData)
{
#ifdef ADSPPMQDI_ULOG
    if((NULL != pUserInfoData) && (NULL != pInfoData))
    {
        ULOG_RT_PRINTF_3(gAdsppm.hLog,
            "print_getinfo_data: User PD, infoId=%u, coreId=%u, instanceId=%u",
            pUserInfoData->infoId, pUserInfoData->coreId, pUserInfoData->instanceId);
        COMPARE_PARAMETERS("infoId",
            pUserInfoData->infoId,
            pInfoData->infoId);
        COMPARE_PARAMETERS("coreId",
            pUserInfoData->coreId,
            pInfoData->coreId);
        COMPARE_PARAMETERS("instanceId",
            pUserInfoData->instanceId,
            pInfoData->instanceId);

        switch(pInfoData->infoId)
        {
        case MMPM_INFO_ID_CLK_FREQ:
        case MMPM_INFO_ID_CORE_CLK_MAX:
            if((NULL != pUserInfoData->info.pInfoClkFreqType) && (NULL != pInfoData->info.pInfoClkFreqType))
            {
                ULOG_RT_PRINTF_3(gAdsppm.hLog,
                    "    User PD pInfoClkFreqType, clkId=%u, clkFreqHz=%u, forceMeasure=%u",
                    pUserInfoData->info.pInfoClkFreqType->clkId,
                    pUserInfoData->info.pInfoClkFreqType->clkFreqHz,
                    pUserInfoData->info.pInfoClkFreqType->forceMeasure);
                COMPARE_PARAMETERS("clkId",
                    pUserInfoData->info.pInfoClkFreqType->clkId,
                    pInfoData->info.pInfoClkFreqType->clkId);
                COMPARE_PARAMETERS("clkFreqHz",
                    pUserInfoData->info.pInfoClkFreqType->clkFreqHz,
                    pInfoData->info.pInfoClkFreqType->clkFreqHz);
                COMPARE_PARAMETERS("forceMeasure",
                    pUserInfoData->info.pInfoClkFreqType->forceMeasure,
                    pInfoData->info.pInfoClkFreqType->forceMeasure);
            }
            break;

        case MMPM_INFO_ID_MIPS_MAX:
            ULOG_RT_PRINTF_1(gAdsppm.hLog,
                "    User PD mipsValue, mipsValue=%u", pUserInfoData->info.mipsValue);
            COMPARE_PARAMETERS("mipsValue",
                pUserInfoData->info.mipsValue,
                pInfoData->info.mipsValue);
            break;

        case MMPM_INFO_ID_BW_MAX:
            ULOG_RT_PRINTF_1(gAdsppm.hLog,
                "    User PD bwValue, bwValue=%u", pUserInfoData->info.bwValue);
            COMPARE_PARAMETERS("bwValue",
                pUserInfoData->info.bwValue,
                pInfoData->info.bwValue);
            break;

        case MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS:
            ULOG_RT_PRINTF_1(gAdsppm.hLog,
                "    User PD aggregateClientClass, aggregateClientClass=%u",
                pUserInfoData->info.aggregateClientClass);
            COMPARE_PARAMETERS("aggregateClientClass",
                pUserInfoData->info.aggregateClientClass,
                pInfoData->info.aggregateClientClass);
            break;

        case MMPM_INFO_ID_DCVS_STATE:
            ULOG_RT_PRINTF_1(gAdsppm.hLog,
                "    User PD dcvsState, dcvsState=%u",
                pUserInfoData->info.dcvsState);
            COMPARE_PARAMETERS("dcvsState",
                pUserInfoData->info.dcvsState,
                pInfoData->info.dcvsState);
            break;

        case MMPM_INFO_ID_EXT_REQ:
            if(pUserInfoData->info.pExtInfo && pInfoData->info.pExtInfo)
            {
                AdsppmInfoPrivateType *pUserInfoDataExtInfo =
                    (AdsppmInfoPrivateType *) pUserInfoData->info.pExtInfo;
                AdsppmInfoPrivateType *pInfoDataExtInfo =
                    (AdsppmInfoPrivateType *) pInfoData->info.pExtInfo;
                ULOG_RT_PRINTF_1(gAdsppm.hLog,
                    "    User PD infoPrivateType, infoPrivateType=%u",
                    pUserInfoDataExtInfo->type);
                COMPARE_PARAMETERS("aggregateClientClass",
                    pUserInfoDataExtInfo->type,
                    pInfoDataExtInfo->type);
            }
            break;

        default:
            break;
        }
    }
#endif
}

// 32-bit addition with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 add_overflow_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a + b;
    if(((UINT_MAX - a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// 32-bit multiplication with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 mult_overflow_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a * b;
    if((a > 0) && ((UINT_MAX / a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// Alignment of an integer to the next multiple of 8, with overflow check.  The
// boolean flag pointed to by overflowFlag is set to true if overflow is
// detected; otherwise it is left unmodified.
static uint32 align8_overflow_check(uint32 a, boolean* overflowFlag)
{
    uint32 result = ADSPPM_ALIGN8(a);
    if(((UINT_MAX - 7) < a) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// Calculate the size of the request while taking into account padding
// needed to maintain the memory address alignment for the various structures
// in the request.
static uint32 calculate_padded_request_size(MmpmRscExtParamType *pUserReq,
    boolean* overflowFlag)
{
    uint32 size = 0, i;

    if(NULL != pUserReq)
    {
        // Top level structure
        size = add_overflow_check(size,
            align8_overflow_check(sizeof(MmpmRscExtParamType), overflowFlag),
            overflowFlag);

        if(NULL != pUserReq->pReqArray)
        {
            // Request array
            size = add_overflow_check(size,
                align8_overflow_check(mult_overflow_check(pUserReq->numOfReq,
                sizeof(MmpmRscParamType), overflowFlag), overflowFlag),
                overflowFlag);
            // Loop through all requests
            for(i = 0; i < pUserReq->numOfReq; i++)
            {
                switch(pUserReq->pReqArray[i].rscId)
                {
                case MMPM_RSC_ID_MIPS_EXT:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmMipsReqType),
                        overflowFlag), overflowFlag);
                    break;

                case MMPM_RSC_ID_MPPS:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmMppsReqType),
                        overflowFlag), overflowFlag);
                    break;

                case MMPM_RSC_ID_CORE_CLK:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmClkReqType),
                        overflowFlag), overflowFlag);
                    if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk)
                    {
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray)
                        {
                            size = add_overflow_check(size,
                                align8_overflow_check(mult_overflow_check(
                                pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk,
                                sizeof(MmpmClkValType), overflowFlag),
                                overflowFlag), overflowFlag);
                        }
                    }
                    break;

                case MMPM_RSC_ID_GENERIC_BW:
                case MMPM_RSC_ID_GENERIC_BW_EXT:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmGenBwReqType),
                        overflowFlag), overflowFlag);
                    if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq)
                    {
                        if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray)
                        {
                            size = add_overflow_check(size,
                                align8_overflow_check(mult_overflow_check(
                                pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw,
                                sizeof(MmpmGenBwValType), overflowFlag),
                                overflowFlag), overflowFlag);
                        }
                    }
                    break;

                case MMPM_RSC_ID_MEM_POWER:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmMemPowerReqParamType),
                        overflowFlag), overflowFlag);
                    break;

                case MMPM_RSC_ID_CORE_CLK_DOMAIN:
                    size = add_overflow_check(size,
                        align8_overflow_check(sizeof(MmpmClkDomainReqType),
                        overflowFlag), overflowFlag);
                    if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClkDomain)
                    {
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClkDomain->pClkDomainArray)
                        {
                            size = add_overflow_check(size,
                                align8_overflow_check(mult_overflow_check(
                                pUserReq->pReqArray[i].rscParam.pCoreClkDomain->numOfClk,
                                sizeof(MmpmClkDomainType), overflowFlag),
                                overflowFlag), overflowFlag);
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }
    return size;
}


static int copy_request_data(
    int client_handle,
    MmpmRscExtParamType *pUserReq,
    MmpmRscExtParamType *pReqData)
{
    int i, result = 0;
    unsigned char *pData = (unsigned char *)pReqData;

    if(NULL != pUserReq)
    {
        // Top level structure
        result = qurt_qdi_copy_from_user(
            client_handle,
            (void *)pReqData,
            (void *)pUserReq,
            sizeof(MmpmRscExtParamType));
        pData += ADSPPM_ALIGN8(sizeof(MmpmRscExtParamType));

        if((result >= 0) && (NULL != pUserReq->pReqArray))
        {
            // Set pointer to the flat structure
            pReqData->pReqArray = (MmpmRscParamType *)pData;
            result = qurt_qdi_copy_from_user(
                client_handle,
                (void *)pReqData->pReqArray,
                (void *)pUserReq->pReqArray,
                pUserReq->numOfReq * sizeof(MmpmRscParamType));
            // Request structure array
            pData += ADSPPM_ALIGN8(pUserReq->numOfReq * sizeof(MmpmRscParamType));

            // Loop through all requests
            if(result >= 0)
            {
                for(i = 0; i < pUserReq->numOfReq; i++)
                {
                    switch(pUserReq->pReqArray[i].rscId)
                    {
                    case MMPM_RSC_ID_MIPS_EXT:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMipsExt)
                        {
                            pReqData->pReqArray[i].rscParam.pMipsExt = (MmpmMipsReqType *)pData;
                            result = qurt_qdi_copy_from_user(
                                client_handle,
                                (void *)(pReqData->pReqArray[i].rscParam.pMipsExt),
                                (void *)(pUserReq->pReqArray[i].rscParam.pMipsExt),
                                sizeof(MmpmMipsReqType));
                            pData += ADSPPM_ALIGN8(sizeof(MmpmMipsReqType));
                        }
                        break;

                    case MMPM_RSC_ID_MPPS:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMppsReq)
                        {
                            pReqData->pReqArray[i].rscParam.pMppsReq = (MmpmMppsReqType *)pData;
                            result = qurt_qdi_copy_from_user(
                                client_handle,
                                (void *)(pReqData->pReqArray[i].rscParam.pMppsReq),
                                (void *)(pUserReq->pReqArray[i].rscParam.pMppsReq),
                                sizeof(MmpmMppsReqType));
                            pData += ADSPPM_ALIGN8(sizeof(MmpmMppsReqType));
                        }
                        break;

                    case MMPM_RSC_ID_CORE_CLK:
                        pReqData->pReqArray[i].rscParam.pCoreClk = (MmpmClkReqType *)pData;
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk)
                        {
                            if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray)
                            {
                                result = qurt_qdi_copy_from_user(
                                    client_handle,
                                    (void *)(pReqData->pReqArray[i].rscParam.pCoreClk),
                                    (void *)(pUserReq->pReqArray[i].rscParam.pCoreClk),
                                    sizeof(MmpmClkReqType));
                                pData += ADSPPM_ALIGN8(sizeof(MmpmClkReqType));
                                pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray = (MmpmClkValType *)pData;
                                if(result >= 0)
                                {
                                    result = qurt_qdi_copy_from_user(
                                        client_handle,
                                        (void *)(pReqData->pReqArray[i].rscParam.pCoreClk->pClkArray),
                                        (void *)(pUserReq->pReqArray[i].rscParam.pCoreClk->pClkArray),
                                        pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk * sizeof(MmpmClkValType));
                                    pData += ADSPPM_ALIGN8(
                                        pUserReq->pReqArray[i].rscParam.pCoreClk->numOfClk *
                                        sizeof(MmpmClkValType));
                                }
                            }
                        }
                        break;

                    case MMPM_RSC_ID_GENERIC_BW:
                    case MMPM_RSC_ID_GENERIC_BW_EXT:
                        pReqData->pReqArray[i].rscParam.pGenBwReq = (MmpmGenBwReqType *)pData;
                        if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq)
                        {
                            if(NULL != pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray)
                            {
                                result = qurt_qdi_copy_from_user(
                                    client_handle,
                                    (void *)(pReqData->pReqArray[i].rscParam.pGenBwReq),
                                    (void *)(pUserReq->pReqArray[i].rscParam.pGenBwReq),
                                    sizeof(MmpmGenBwReqType));
                                pData += ADSPPM_ALIGN8(sizeof(MmpmGenBwReqType));
                                pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray = (MmpmGenBwValType *)pData;
                                if(result >= 0)
                                {
                                    result = qurt_qdi_copy_from_user(
                                        client_handle,
                                        (void *)(pReqData->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray),
                                        (void *)(pUserReq->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray),
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw * sizeof(MmpmGenBwValType));
                                    pData += ADSPPM_ALIGN8(
                                        pUserReq->pReqArray[i].rscParam.pGenBwReq->numOfBw *
                                        sizeof(MmpmGenBwValType));
                                }
                            }
                        }
                        break;

                    case MMPM_RSC_ID_MEM_POWER:
                        if(NULL != pUserReq->pReqArray[i].rscParam.pMemPowerState)
                        {
                            pReqData->pReqArray[i].rscParam.pMemPowerState = (MmpmMemPowerReqParamType *)pData;
                            result = qurt_qdi_copy_from_user(
                                client_handle,
                                (void *)(pReqData->pReqArray[i].rscParam.pMemPowerState),
                                (void *)(pUserReq->pReqArray[i].rscParam.pMemPowerState),
                                sizeof(MmpmMemPowerReqParamType));
                            pData += ADSPPM_ALIGN8(sizeof(MmpmMemPowerReqParamType));
                        }
                        break;

                    case MMPM_RSC_ID_CORE_CLK_DOMAIN:
                        pReqData->pReqArray[i].rscParam.pCoreClkDomain = (MmpmClkDomainReqType *)pData;
                        if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClkDomain)
                        {
                            if(NULL != pUserReq->pReqArray[i].rscParam.pCoreClkDomain->pClkDomainArray)
                            {
                                result = qurt_qdi_copy_from_user(
                                    client_handle,
                                    (void *)(pReqData->pReqArray[i].rscParam.pCoreClkDomain),
                                    (void *)(pUserReq->pReqArray[i].rscParam.pCoreClkDomain),
                                    sizeof(MmpmClkDomainReqType));
                                pData += ADSPPM_ALIGN8(sizeof(MmpmClkDomainReqType));
                                pReqData->pReqArray[i].rscParam.pCoreClkDomain->pClkDomainArray = (MmpmClkDomainType *)pData;
                                if(result >= 0)
                                {
                                    result = qurt_qdi_copy_from_user(
                                        client_handle,
                                        (void *)(pReqData->pReqArray[i].rscParam.pCoreClkDomain->pClkDomainArray),
                                        (void *)(pUserReq->pReqArray[i].rscParam.pCoreClkDomain->pClkDomainArray),
                                        pUserReq->pReqArray[i].rscParam.pCoreClkDomain->numOfClk * sizeof(MmpmClkDomainType));
                                    pData += ADSPPM_ALIGN8(
                                        pUserReq->pReqArray[i].rscParam.pCoreClkDomain->numOfClk *
                                        sizeof(MmpmClkDomainType));
                                }
                            }
                        }
                        break;

                    default:
                        break;
                    }

                    if(result < 0)
                    {
                        break;
                    }
                }
            }
        }
        print_request_data(client_handle, pUserReq, pReqData);
    }
    return result;
}


static MmpmRscExtParamType *get_request_data_from_user(
    int client_handle,
    MmpmRscExtParamType *pUserReq)
{
    MmpmRscExtParamType *pData = NULL;
    boolean overflowFlag = FALSE;
    uint32 size = calculate_padded_request_size(pUserReq, &overflowFlag);
    if(overflowFlag)
    {
        ULOG_RT_PRINTF_0(gAdsppm.hLog,
            "detected integer overflow while handling request");
    }
    if((size > sizeof(MmpmRscExtParamType)) && !overflowFlag)
    {
        pData = malloc(size);
        if(NULL != pData)
        {
            if(0 > copy_request_data(client_handle, pUserReq, pData))
            {
                free(pData);
                pData = NULL;
            }
        }
    }
    return pData;
}


static int copy_request_status_to_user(
    int client_handle,
    MmpmRscExtParamType *pReqData,
    MmpmRscExtParamType *pUserReq)
{
    int ret = -1;
    if((NULL != pReqData) && (NULL != pUserReq))
    {
        if((NULL != pReqData->pStsArray) && (NULL != pUserReq->pStsArray))
        {
            ret = qurt_qdi_copy_to_user(
                client_handle,
                (void *)pUserReq->pStsArray,
                (void *)pReqData->pStsArray,
                pReqData->numOfReq * sizeof(MMPM_STATUS));
        }
    }
    return ret;
}


static int calculate_setparam_size(MmpmParameterConfigType *pParamConfigData)
{
    int size = 0;
    if(NULL != pParamConfigData)
    {
        // Top level structure
        size += ADSPPM_ALIGN8(sizeof(MmpmParameterConfigType));

        if(NULL != pParamConfigData->pParamConfig)
        {
            switch(pParamConfigData->paramId)
            {
            case MMPM_PARAM_ID_RESOURCE_LIMIT:
                break;

            case MMPM_PARAM_ID_CLIENT_OCMEM_MAP:
                break;

            case MMPM_PARAM_ID_MEMORY_MAP:
                size += ADSPPM_ALIGN8(sizeof(MmpmMemoryMapType));
                break;

            case MMPM_PARAM_ID_CLIENT_CLASS:
                size += ADSPPM_ALIGN8(sizeof(MmpmClientClassType));
                break;

            case MMPM_PARAM_ID_L2_CACHE_LINE_LOCK:
                size += ADSPPM_ALIGN8(sizeof(MmpmL2CacheLineLockParameterType));
                break;

            case MMPM_PARAM_ID_DCVS_PARTICIPATION:
                size += ADSPPM_ALIGN8(sizeof(MmpmDcvsParticipationType));
                break;

            default:
                break;
            }
        }
    }
    return size;
}


static int copy_setparam_data(
    int client_handle,
    MmpmParameterConfigType *pUserParamData,
    MmpmParameterConfigType *pParamData)
{
    int result = 0;
    unsigned char *pData = (unsigned char *)pParamData;
    if(NULL != pUserParamData)
    {
        // Top level structure
        result = qurt_qdi_copy_from_user(
            client_handle,
            pData,
            (void *)pUserParamData,
            sizeof(MmpmParameterConfigType));
        pData += ADSPPM_ALIGN8(sizeof(MmpmParameterConfigType));

        if((result >= 0) && (NULL != pUserParamData->pParamConfig))
        {
            switch(pUserParamData->paramId)
            {
            case MMPM_PARAM_ID_RESOURCE_LIMIT:
                break;

            case MMPM_PARAM_ID_CLIENT_OCMEM_MAP:
                break;

            case MMPM_PARAM_ID_MEMORY_MAP:
                pParamData->pParamConfig = (MmpmMemoryMapType *)pData;
                result = qurt_qdi_copy_from_user(
                    client_handle,
                    pData,
                    (void *)(pUserParamData->pParamConfig),
                    sizeof(MmpmMemoryMapType));
                pData += ADSPPM_ALIGN8(sizeof(MmpmMemoryMapType));
                break;

            case MMPM_PARAM_ID_CLIENT_CLASS:
                pParamData->pParamConfig = (MmpmClientClassType *)pData;
                result = qurt_qdi_copy_from_user(
                    client_handle,
                    pData,
                    (void *)(pUserParamData->pParamConfig),
                    sizeof(MmpmClientClassType));
                pData += ADSPPM_ALIGN8(sizeof(MmpmClientClassType));
                break;

            case MMPM_PARAM_ID_L2_CACHE_LINE_LOCK:
                pParamData->pParamConfig =
                    (MmpmL2CacheLineLockParameterType*) pData;
                result = qurt_qdi_copy_from_user(
                    client_handle,
                    pData,
                    (void *)(pUserParamData->pParamConfig),
                    sizeof(MmpmL2CacheLineLockParameterType));
                pData += ADSPPM_ALIGN8(
                    sizeof(MmpmL2CacheLineLockParameterType));
                break;

            case MMPM_PARAM_ID_DCVS_PARTICIPATION:
                pParamData->pParamConfig =
                    (MmpmDcvsParticipationType*) pData;
                result = qurt_qdi_copy_from_user(
                    client_handle,
                    pData,
                    (void *)(pUserParamData->pParamConfig),
                    sizeof(MmpmDcvsParticipationType));
                pData += ADSPPM_ALIGN8(
                    sizeof(MmpmDcvsParticipationType));
                break;

            default:
                break;
            }
        }
    }
    return result;
}


static MmpmParameterConfigType *get_setparam_data_from_user(
    int client_handle,
    MmpmParameterConfigType *pUserParam)
{
    MmpmParameterConfigType *pData = NULL;
    int size = calculate_setparam_size(pUserParam);
    if(size > sizeof(MmpmParameterConfigType))
    {
        pData = malloc(size);
        if(NULL != pData)
        {
            if(0 > copy_setparam_data(client_handle, pUserParam, pData))
            {
                free(pData);
                pData = NULL;
            }
        }
    }
    return pData;
}


// Calculate the size of the GetInfo request while taking into account padding
// needed to maintain the memory address alignment for the various structures
// in the request.
static int calculate_getinfo_padded_size(MmpmInfoDataType *pGetInfoData)
{
    int size = 0;
    if(NULL != pGetInfoData)
    {
        // Top level structure
        size += ADSPPM_ALIGN8(sizeof(MmpmInfoDataType));

        // Padding for alignment not necessary if it's the last part
        // of the request, but do it anyway for consistency
        switch(pGetInfoData->infoId)
        {
        case MMPM_INFO_ID_CLK_FREQ:
        case MMPM_INFO_ID_CORE_CLK_MAX:
            size += ADSPPM_ALIGN8(sizeof(MmpmInfoClkFreqType));
            break;

        case MMPM_INFO_ID_MIPS_MAX:
            break;

        case MMPM_INFO_ID_BW_MAX:
            break;

        case MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS:
            break;

        case MMPM_INFO_ID_MPPS:
            size += ADSPPM_ALIGN8(sizeof(MmpmInfoMppsType));
            break;

        case MMPM_INFO_ID_BW_EXT:
            size += ADSPPM_ALIGN8(sizeof(MmpmInfoBwExtType));
            break;

        case MMPM_INFO_ID_DCVS_STATE:
            break;

        case MMPM_INFO_ID_EXT_REQ:
            size += ADSPPM_ALIGN8(sizeof(AdsppmInfoPrivateType));
            break;

        default:
            break;

        }
    }
    return size;
}


static int copy_getinfo_data_from_user(
    int client_handle,
    MmpmInfoDataType *pUserInfoData,
    MmpmInfoDataType *pInfoData)
{
    int result = 0;
    unsigned char *pData = (unsigned char *)pInfoData;
    if(NULL != pUserInfoData)
    {
        // Top level structure
        result = qurt_qdi_copy_from_user(
            client_handle,
            (void *)pInfoData,
            (void *)pUserInfoData,
            sizeof(MmpmInfoDataType));
        pData += ADSPPM_ALIGN8(sizeof(MmpmInfoDataType));
        if(result >= 0)
        {
            switch(pUserInfoData->infoId)
            {
            case MMPM_INFO_ID_CLK_FREQ:
            case MMPM_INFO_ID_CORE_CLK_MAX:
                if(NULL != pUserInfoData->info.pInfoClkFreqType)
                {
                    pInfoData->info.pInfoClkFreqType = (MmpmInfoClkFreqType *)pData;
                    result = qurt_qdi_copy_from_user(
                        client_handle,
                        (void *)(pInfoData->info.pInfoClkFreqType),
                        (void *)(pUserInfoData->info.pInfoClkFreqType),
                        sizeof(MmpmInfoClkFreqType));
                    pData += ADSPPM_ALIGN8(sizeof(MmpmInfoClkFreqType));
                }
                break;

            case MMPM_INFO_ID_MIPS_MAX:
                break;

            case MMPM_INFO_ID_BW_MAX:
                break;

            case MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS:
                break;

            case MMPM_INFO_ID_MPPS:
                if(NULL != pUserInfoData->info.pMppsInfo)
                {
                    pInfoData->info.pMppsInfo = (MmpmInfoMppsType*) pData;
                    result = qurt_qdi_copy_from_user(
                        client_handle,
                        (void *)(pInfoData->info.pMppsInfo),
                        (void *)(pUserInfoData->info.pMppsInfo),
                        sizeof(MmpmInfoMppsType));
                    pData += ADSPPM_ALIGN8(sizeof(MmpmInfoMppsType));
                }
                break;

            case MMPM_INFO_ID_BW_EXT:
                if(NULL != pUserInfoData->info.pBwExtInfo)
                {
                    pInfoData->info.pBwExtInfo = (MmpmInfoBwExtType*) pData;
                    result = qurt_qdi_copy_from_user(
                        client_handle,
                        (void *)(pInfoData->info.pBwExtInfo),
                        (void *)(pUserInfoData->info.pBwExtInfo),
                        sizeof(MmpmInfoBwExtType));
                    pData += ADSPPM_ALIGN8(sizeof(MmpmInfoBwExtType));
                }
                break;

            case MMPM_INFO_ID_DCVS_STATE:
                break;

            case MMPM_INFO_ID_EXT_REQ:
                if(NULL != pUserInfoData->info.pExtInfo)
                {
                    pInfoData->info.pExtInfo = (AdsppmInfoPrivateType *)pData;
                    result = qurt_qdi_copy_from_user(
                        client_handle,
                        (void *)(pInfoData->info.pExtInfo),
                        (void *)(pUserInfoData->info.pExtInfo),
                        sizeof(AdsppmInfoPrivateType));
                    pData += ADSPPM_ALIGN8(sizeof(AdsppmInfoPrivateType));
                }
                break;

            default:
                break;
            }
        }
        print_getinfo_data(pUserInfoData, pInfoData);
    }
    return result;
}


static int copy_getinfo_data_to_user(
    int client_handle,
    MmpmInfoDataType *pUserInfoData,
    MmpmInfoDataType *pInfoData)
{
    int result = -1;
    if((NULL != pUserInfoData) && (NULL != pInfoData))
    {
        switch(pInfoData->infoId)
        {
        case MMPM_INFO_ID_CLK_FREQ:
        case MMPM_INFO_ID_CORE_CLK_MAX:
            if((NULL != pUserInfoData->info.pInfoClkFreqType) &&
                (NULL != pInfoData->info.pInfoClkFreqType))
            {
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)(pUserInfoData->info.pInfoClkFreqType),
                    (void *)(pInfoData->info.pInfoClkFreqType),
                    sizeof(MmpmInfoClkFreqType));
            }
            break;

        case MMPM_INFO_ID_MIPS_MAX:
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)&(pUserInfoData->info.mipsValue),
                (void *)&(pInfoData->info.mipsValue),
                sizeof(uint32));
            break;

        case MMPM_INFO_ID_BW_MAX:
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)&(pUserInfoData->info.bwValue),
                (void *)&(pInfoData->info.bwValue),
                sizeof(uint64));
            break;

        case MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS:
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)&(pUserInfoData->info.aggregateClientClass),
                (void *)&(pInfoData->info.aggregateClientClass),
                sizeof(uint32));
            break;

        case MMPM_INFO_ID_MPPS:
            if((NULL != pUserInfoData->info.pMppsInfo) &&
                (NULL != pInfoData->info.pMppsInfo))
            {
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)(pUserInfoData->info.pMppsInfo),
                    (void *)(pInfoData->info.pMppsInfo),
                    sizeof(MmpmInfoMppsType));
            }
            break;

        case MMPM_INFO_ID_BW_EXT:
            if((NULL != pUserInfoData->info.pBwExtInfo) &&
                (NULL != pInfoData->info.pBwExtInfo))
            {
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)(pUserInfoData->info.pBwExtInfo),
                    (void *)(pInfoData->info.pBwExtInfo),
                    sizeof(MmpmInfoBwExtType));
            }
            break;

        case MMPM_INFO_ID_DCVS_STATE:
            break;

        case MMPM_INFO_ID_EXT_REQ:
            if((NULL != pUserInfoData->info.pExtInfo) &&
                (NULL != pInfoData->info.pExtInfo))
            {
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)(pUserInfoData->info.pExtInfo),
                    (void *)(pInfoData->info.pExtInfo),
                    sizeof(AdsppmInfoPrivateType));
            }
            break;

        default:
            break;
        }
        print_getinfo_data(pUserInfoData, pInfoData);
    }
    return result;
}


static MmpmInfoDataType *get_getinfo_data_from_user(
    int client_handle,
    MmpmInfoDataType *pUserInfoData)
{
    MmpmInfoDataType *pData = NULL;
    int size = calculate_getinfo_padded_size(pUserInfoData);
    if(size > sizeof(MmpmInfoDataType))
    {
        pData = malloc(size);
        if(NULL != pData)
        {
            if(0 > copy_getinfo_data_from_user(client_handle, pUserInfoData, pData))
            {
                free(pData);
                pData = NULL;
            }
        }
    }
    return pData;
}


void QDI_Adsppm_do_callback(QDI_Adsppm_cbinfo_t *ptr, MmpmCallbackParamType *pValue, uint32 pd_num)
{
    QDI_Adsppm_CbqElem_t *cbq;
    MmpmCompletionCallbackDataType *pReturnCallbackValue =
        (MmpmCompletionCallbackDataType *) pValue->callbackData;
    cbq = malloc(sizeof(QDI_Adsppm_CbqElem_t));
    if(cbq)
    {
        cbq->cbinfo = ptr;
        cbq->callbackParam.eventId = pValue->eventId;
        cbq->callbackParam.clientId = pValue->clientId;
        cbq->callbackParam.callbackDataSize = pValue->callbackDataSize;
        cbq->callbackParam.callbackData = &cbq->callbackValue;
        cbq->callbackValue.reqTag = pReturnCallbackValue->reqTag;
        cbq->callbackValue.result = pReturnCallbackValue->result;
        qurt_rmutex_lock(&(ptr->object->mtx));
        cbq->next = ptr->object->cbqueue;
        ptr->object->cbqueue = cbq;
        qurt_rmutex_unlock(&(ptr->object->mtx));
        qurt_signal_set(&ptr->object->sigLocal[pd_num], 1);
    }
}


static int QDI_Adsppm_get_callback(
    int client_handle,
    QDI_Adsppm_Ctx_t *pCbDrv,
    void *buf,
    unsigned int pd_num)
{
    Adsppm_cbinfo_Client_t info;
    QDI_Adsppm_CbqElem_t *cbq = NULL;
    QDI_Adsppm_CbqElem_t *ptr = NULL;
	QDI_Adsppm_CbqElem_t *ptrPdMatched = NULL;
	QDI_Adsppm_CbqElem_t *ptrPdMatchedPrev = NULL;
    QDI_Adsppm_CbqElem_t *prev = NULL;
    unsigned signalRead;
    int result = QURT_EOK;

    if(buf == NULL)
        return -1;

    for(;;)
    {
        qurt_rmutex_lock(&pCbDrv->mtx);
        ptr = pCbDrv->cbqueue;
        while(ptr)
        {          
			// Remember non-NULL item whose pdNum matches
			if(ptr->cbinfo->pdNum == pd_num)
				{
					ptrPdMatchedPrev = prev;
					ptrPdMatched = ptr;
				}
			prev = ptr;
            // Get to the end of the list
            ptr = ptr->next;
        }
		

		if(ptrPdMatched)
        {
          	cbq = ptrPdMatched;
            if(ptrPdMatchedPrev)
            {
                ptrPdMatchedPrev->next = ptrPdMatched->next;
            }
            else
            {
            	// If the prev is NULL, then ptrPdMatched is first node
                // Point the head to ptrPdMatched->next
                pCbDrv->cbqueue = ptrPdMatched->next;
            }
            qurt_rmutex_unlock(&pCbDrv->mtx);
            break;
        }
        else
        {
            qurt_rmutex_unlock(&pCbDrv->mtx);
            result = qurt_signal_wait_cancellable(
                &pCbDrv->sigLocal[pd_num], 1, QURT_SIGNAL_ATTR_WAIT_ANY, &signalRead);
            if(QURT_ECANCEL == result)
            {
                ULOG_RT_PRINTF_1(gAdsppm.hLog,
                    "%s: signal wait is cancelled", __FUNCTION__);
                break;
            }
            else
            {
                qurt_signal_clear(&pCbDrv->sigLocal[pd_num], signalRead);
            }
        }
    }

    if(QURT_EOK == result)
    {
        info.pfn = cbq->cbinfo->pfn;
        // Pointer to the data should be substituted on the client's side
        info.callbackParam.callbackDataSize = cbq->callbackParam.callbackDataSize;
        info.callbackParam.clientId = cbq->callbackParam.clientId;
        info.callbackParam.eventId = cbq->callbackParam.eventId;
        info.callbackValue.reqTag = cbq->callbackValue.reqTag;
        info.callbackValue.result = cbq->callbackValue.result;
        result = qurt_qdi_copy_to_user(client_handle, buf, &info, sizeof(info));
    }
    else
    {
        // Wait on signal probably got cancelled.
        // Clear the data to be returned to the caller, to be safe.
        memset(&info, 0, sizeof(Adsppm_cbinfo_Client_t));
        qurt_qdi_copy_to_user(client_handle, buf, &info, sizeof(Adsppm_cbinfo_Client_t));
    }

    if(NULL != cbq)
        free(cbq);

    return result;
}


void QDI_Adsppm_Release(qurt_qdi_obj_t *qdiobj)
{
    AdsppmPdState* pPdState = (AdsppmPdState*) qdiobj;
    size_t index;
	
    ULOG_RT_PRINTF_1(gAdsppm.hLog, "Enter release: pPdState 0x%lx",
        (uint32) pPdState);

    for(index = 0; index < ClientIdListLength(&(pPdState->clientList)); index++)
    {
        uint32 clientId;
        ClientIdListGet(&(pPdState->clientList), index, &clientId);
        Adsppmcb_deRegister_callback_Srv(clientId);
        MmpmStatusType ret = MMPM_Deregister_Ext(clientId);
        if(ret == MMPM_STATUS_SUCCESS)
        {
            ULOG_RT_PRINTF_2(gAdsppm.hLog,
                "Deregistered clientId %u to pPdState 0x%x due to device close",
                clientId, (uint32)pPdState);
        }
        else
        {
            ULOG_RT_PRINTF_2(gAdsppm.hLog,
                "Failed to deregister clientId %lu to pPdState 0x%lx due to "
                "device close", clientId, (uint32) pPdState);
        }
    }
    ClientIdListCleanup(&(pPdState->clientList));
    free(pPdState);
    ULOG_RT_PRINTF_1(gAdsppm.hLog,
        "Exit release: pPdState 0x%lx", (uint32) pPdState);

}


int QDI_Adsppm_Invocation(
    int client_handle,
    qurt_qdi_obj_t *obj,
    int method,
    qurt_qdi_arg_t a1,
    qurt_qdi_arg_t a2,
    qurt_qdi_arg_t a3,
    qurt_qdi_arg_t a4,
    qurt_qdi_arg_t a5,
    qurt_qdi_arg_t a6,
    qurt_qdi_arg_t a7,
    qurt_qdi_arg_t a8,
    qurt_qdi_arg_t a9)
{
    QDI_Adsppm_cbinfo_t CbInfo;
    int result = 0;
    MmpmStatusType ret = MMPM_STATUS_SUCCESS;
    MmpmRegParamType *pRegParam = NULL;
    uint32 clientId = 0;
    MmpmRscExtParamType *pRscExtParam = NULL;
    MmpmInfoDataType *pInfoData = NULL;
    MmpmParameterConfigType *pSetParamData = NULL;

    AdsppmPdState* pPdState = (AdsppmPdState*) obj;

    memset(&CbInfo, 0, sizeof(QDI_Adsppm_cbinfo_t));

    switch(method)
    {
    case QDI_OPEN:
        pPdState = (AdsppmPdState*) malloc(sizeof(AdsppmPdState));
        if(pPdState != NULL)
        {
            ULOG_RT_PRINTF_1(gAdsppm.hLog,
                "Created pPdState 0x%lx", (uint32) pPdState);
            pPdState->qdiObj.invoke = QDI_Adsppm_Invocation;
            pPdState->qdiObj.refcnt = QDI_REFCNT_INIT;
            pPdState->qdiObj.release = QDI_Adsppm_Release;
            ClientIdListInit(&(pPdState->clientList));
            result = qurt_qdi_new_handle_from_obj_t(client_handle,
                (qurt_qdi_obj_t *)pPdState);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_REGISTER:
        pRegParam = malloc(sizeof(MmpmRegParamType));
        if(NULL != pRegParam)
        {
            result = qurt_qdi_copy_from_user(
                client_handle,
                (void *)pRegParam,
                (void *)a1.ptr,
                sizeof(MmpmRegParamType));
            if(0 > result)
            {
                result = -1;
            }
            else
            {
                if(CALLBACK_NONE != pRegParam->callBackFlag)
                {
                    CbInfo.object = &gAdsppm;
                    // Replace clients' callback with ADSPPM callback that
                    // handles user PD clients' callbacks
                    CbInfo.pfn = pRegParam->MMPM_Callback;
                    pRegParam->MMPM_Callback = Adsppmcb_notify_callbacks_Srv;
                }
                clientId = MMPM_Register_Ext(pRegParam);
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)a2.ptr,
                    (void *)&clientId,
                    sizeof(clientId));
                if(clientId != 0)
                {
                    if(!ClientIdListInsertEnd(&(pPdState->clientList),
                        &clientId))
					{
						ULOG_RT_PRINTF_2(gAdsppm.hLog,
                            "Failed to insert clientId %lu to pPdState 0x%lx",
                            clientId, (uint32) pPdState);
					}
                    if(pRegParam->callBackFlag != CALLBACK_NONE)
                    {
                        CbInfo.clientId = clientId;
						CbInfo.pdNum = a3.num;
                        Adsppmcb_register_callback_Srv(&CbInfo);
                    }
                }
                else
                {
                    result = -1;
                }
            }
            free(pRegParam);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_REQUEST:
        pRscExtParam = get_request_data_from_user(
            client_handle,
            (MmpmRscExtParamType *)a2.ptr);
        if(NULL != pRscExtParam)
        {
            ret = MMPM_Request_Ext(a1.num, pRscExtParam);
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)a3.ptr,
                (void *)&ret,
                sizeof(ret));
            if(result >= 0)
            {
                // Copy status array to user
                result = copy_request_status_to_user(
                    client_handle,
                    pRscExtParam,
                    (MmpmRscExtParamType *)a2.ptr);
            }
            if(MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            free(pRscExtParam);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_RELEASE:
        pRscExtParam = malloc(sizeof(MmpmRscExtParamType));
        if(NULL != pRscExtParam)
        {
            result = qurt_qdi_copy_from_user(
                client_handle,
                (void *)pRscExtParam,
                (void *)a2.ptr,
                sizeof(MmpmRscExtParamType));
            if(result < 0)
            {
                result = -1;
            }
            else
            {
                ret = MMPM_Release_Ext(a1.num, pRscExtParam);
                result = qurt_qdi_copy_to_user(
                    client_handle,
                    (void *)a3.ptr,
                    (void *)&ret,
                    sizeof(ret));
                if(MMPM_STATUS_SUCCESS != ret)
                {
                    result = -1;
                }
            }
            free(pRscExtParam);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_DEREGISTER:
        // a1.num is client ID
        if(a1.num)
        {
            uint32 clientId = (uint32) a1.num;
            Adsppmcb_deRegister_callback_Srv(a1.num);
            ret = MMPM_Deregister_Ext(a1.num);
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)a2.ptr,
                (void *)&ret,
                sizeof(ret));
            if(MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            else
            {
                if(!ClientIdListRemoveItem(&(pPdState->clientList), &clientId))
                {
                    ULOG_RT_PRINTF_2(gAdsppm.hLog,
                        "Failed to remove clientId %u from pPdState 0x%x",
                        clientId, (uint32) pPdState);
                }
            }
        }
        else
        {
            result = -1;
            ULOG_RT_PRINTF_3(gAdsppm.hLog,
                "%s: bad client ID %d from  list for PD 0x%x",
                __FUNCTION__, a1.num, client_handle);
        }
        break;

    case ADSPPM_GET_INFO:
        pInfoData = get_getinfo_data_from_user(
            client_handle,
            (MmpmInfoDataType *)a1.ptr);
        if(NULL != pInfoData)
        {
            ret = MMPM_GetInfo(pInfoData);
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)a2.ptr,
                (void *)&ret,
                sizeof(ret));
            if(MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            else
            {
                result = copy_getinfo_data_to_user(
                    client_handle,
                    (MmpmInfoDataType *)a1.ptr,
                    pInfoData);
            }
            free(pInfoData);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_SET_PARAM:
        pSetParamData = get_setparam_data_from_user(
            client_handle,
            (MmpmParameterConfigType *)a2.ptr);
        if(NULL != pSetParamData)
        {
            ret = MMPM_SetParameter(a1.num, pSetParamData);
            result = qurt_qdi_copy_to_user(
                client_handle,
                (void *)a3.ptr,
                (void *)&ret,
                sizeof(ret));
            if(MMPM_STATUS_SUCCESS != ret)
            {
                result = -1;
            }
            free(pSetParamData);
        }
        else
        {
            result = -1;
        }
        break;

    case ADSPPM_GET_CB:
        result = QDI_Adsppm_get_callback(client_handle, &gAdsppm, a1.ptr, a2.num);
        break;

    default:
        result = qurt_qdi_method_default(
            client_handle, obj, method,
            a1, a2, a3, a4, a5, a6, a7, a8, a9);
        break;
    }
    return result;
}


const QDI_Adsppm_Opener_t adsppm_opener =
{
    {
        QDI_Adsppm_Invocation,
        QDI_REFCNT_PERM,
        0
    } // 0 as object is PERM, never released
};


/**
 * @fn QDI_adsppm_init - registers driver with QDI framework
 * @brief This function is called by RCINIT after ADSPPM is initialized.
 */
void QDI_adsppm_init(void)
{
    unsigned int i=0;	
    gAdsppm.log_buffer_size = ADSPPM_QDIULOG_BUFFER_SIZE;
    ULogFront_RealTimeInit(
        &gAdsppm.hLog,
        "ADSPPM SRV Log",
        gAdsppm.log_buffer_size,
        ULOG_MEMORY_LOCAL,
        ULOG_LOCK_OS);
    gAdsppm.cbqueue = NULL;
    qurt_rmutex_init(&gAdsppm.mtx);

    for (i =0; i < QDSPPM_NUM_PDS; i++)
	    qurt_signal_init(&gAdsppm.sigLocal[i]); 

	ULOG_RT_PRINTF_1(gAdsppm.hLog, "QDSPPM_NUM_PDS=%u", QDSPPM_NUM_PDS);

    Adsppmcb_Init_Server();
    qurt_qdi_devname_register(ADSPPM_QDI_DRV_NAME, (void *)&adsppm_opener);
}

