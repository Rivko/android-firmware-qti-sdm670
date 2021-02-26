/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * clkmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "requestmgr.h"
#include "asic.h"
#include "clkmgr.h"
#include "core_internal.h"
#include "hal_clkrgm.h"


//Enumeration for core clock index
//Has to be in exactly the same order as core clocks arranged in the AdsppmClkIdType
typedef enum
{
    HwRsp_Core = 0,
    Midi_Core,
    AvSync_Xo,
    AvSync_Bt,
    AvSync_Fm,
    Slimbus_Core,
    Slimbus2_Core,
    Avtimer_Core,
    Atime_Core,
    Atime2_Core,
    VAPSS_Core,
    VAPSS_AXI,
    VAPSS_TCMS,
    Sdc_Core,
    Qup_Core,
    Ccd_Core
    
} ClockIndexType;

typedef struct
{
    AsicClkTypeType          type;
    AdsppmClkIdType          id;
    uint32                   freq;              //current freq
    uint32                   lastFreq;          //last set freq
    AdsppmClkDomainSrcIdType clkDomainSrc;      //current clock source
    AdsppmClkDomainSrcIdType clkDomainSrc_last; //last set clock source
} ClockInfoType;

static ClockInfoType gClockInfo[] =
{
   {.id = AdsppmClk_HwRsp_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Midi_Core,     .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_AvSync_Xo,     .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_AvSync_Bt,     .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_AvSync_Fm,     .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Slimbus_Core,  .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Slimbus2_Core, .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Avtimer_core,  .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Atime_core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Atime2_core,   .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_VAPSS_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Sdc_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Qup_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Sram_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
   {.id = AdsppmClk_Ccd_Core,    .freq = 0, .lastFreq = 0, .clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL},
};


Adsppm_Status CLK_Init(void)
{
    uint32 i;
    Adsppm_Status sts = Adsppm_Status_Success;
    for(i = 0; i < (sizeof(gClockInfo) / sizeof(gClockInfo[0])); i++)
    {
    if(!ACM_IsClkIdValid(gClockInfo[i].id))
      continue;

        gClockInfo[i].type = ACMClk_GetClockType(gClockInfo[i].id);
    }
    return sts;
}


Adsppm_Status CLK_ProcessRequest(coreUtils_Q_Type *pClkReqQ)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmClkRequestType *pClkReqData;
    RMHandleType *pRMHandle = NULL;
    uint32 index_a;
    uint32 index_b;
    uint32 index_c;

    if(NULL == pClkReqQ)
    {
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk);

        for(index_a = 0; index_a < (sizeof(gClockInfo) / sizeof(gClockInfo[0])); index_a++)
        {
            gClockInfo[index_a].freq = 0;
        }

        //checks with all clients
        pRMHandle = (RMHandleType *)coreUtils_Q_Check(pClkReqQ);
        for(index_a = 0; index_a < pClkReqQ->nCnt; index_a++)
        {
            if(NULL != pRMHandle)
            {
                if(RM_Valid == pRMHandle->validFlag)
                {
                    if((NULL == (AdsppmClkRequestType *)(pRMHandle->pRequestData)) || (NULL == pRMHandle->pClient))
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "pRequestData(%u) or pClient(%u) is NULL",
                                (AdsppmClkRequestType *)(pRMHandle->pRequestData), pRMHandle->pClient);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        pClkReqData = (AdsppmClkRequestType *)(pRMHandle->pRequestData);
                        //checks array of clock settings
                        for(index_b = 0; index_b < pClkReqData->numOfClk; index_b++)
                        {
                            //get core clock ID corresponding to this core instance if applicable
                            pClkReqData->pClkArray[index_b].clkId = ACMClk_GetInstanceCoreClockId(pRMHandle->pClient->coreId,
                                    pRMHandle->pClient->instanceId, pClkReqData->pClkArray[index_b].clkId);
                            ClockIndexType clkIdx = pClkReqData->pClkArray[index_b].clkId-AdsppmClk_HwRsp_Core;
                            gClockInfo[clkIdx].freq = MAX(gClockInfo[clkIdx].freq, pClkReqData->pClkArray[index_b].clkFreqHz);
                        }
                    }
                }
                pRMHandle = (RMHandleType *)coreUtils_Q_Next(pClkReqQ, &pRMHandle->link);
            }
        }
        for(index_c = 0; index_c < (sizeof(gClockInfo) / sizeof(gClockInfo[0])); index_c++)
        {
            //Adjust based on feature parameters
            gClockInfo[index_c].freq = ACM_AdjustParamValue(AsicFeatureId_CoreClk_Scaling, gClockInfo[index_c].freq);
            if((gClockInfo[index_c].lastFreq != gClockInfo[index_c].freq) && ACM_IsClkIdValid(gClockInfo[index_c].id))
            {
                if(Adsppm_Status_Success != ClkRgm_SetClock(gClockInfo[index_c].id, gClockInfo[index_c].freq))
                {
                    sts = Adsppm_Status_Failed;
                    break;
                }
                else
                {
                    gClockInfo[index_c].lastFreq = gClockInfo[index_c].freq;
                }
            }
        }
        ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk);
    }

    return sts;
}


Adsppm_Status CLKDomain_ProcessRequest(coreUtils_Q_Type *pClkDomainReqQ)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmClkDomainReqType *pClkDomainReqData;
    RMHandleType *pRMHandle = NULL;
    uint32 index_a;
    uint32 index_b;
    uint32 index_c;

    if(NULL == pClkDomainReqQ)
    {
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk_Domain);

        for(index_a = 0; index_a < (sizeof(gClockInfo) / sizeof(gClockInfo[0])); index_a++)
        {
            gClockInfo[index_a].clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Primary_PLL;
        }

        //checks with all clients
        pRMHandle = (RMHandleType *)coreUtils_Q_Check(pClkDomainReqQ);
        for(index_a = 0; index_a < pClkDomainReqQ->nCnt; index_a++)
        {
            if(NULL != pRMHandle)
            {
                if(RM_Valid == pRMHandle->validFlag)
                {
                    if((NULL == (AdsppmClkDomainReqType *)(pRMHandle->pRequestData)) || (NULL == pRMHandle->pClient))
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR, "pRequestData(%u) or pClient(%u) is NULL",
                                (AdsppmClkDomainReqType *)(pRMHandle->pRequestData), pRMHandle->pClient);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        pClkDomainReqData = (AdsppmClkDomainReqType *)(pRMHandle->pRequestData);
                        //checks array of clock settings
                        for(index_b = 0; index_b < pClkDomainReqData->numOfClk; index_b++)
                        {
                            //get core clock ID corresponding to this core instance if applicable
                            pClkDomainReqData->pClkDomainArray[index_b].clkId = ACMClk_GetInstanceCoreClockId(pRMHandle->pClient->coreId,
                                    pRMHandle->pClient->instanceId, pClkDomainReqData->pClkDomainArray[index_b].clkId);
                            ClockIndexType clkIdx = pClkDomainReqData->pClkDomainArray[index_b].clkId - AdsppmClk_HwRsp_Core;
                            gClockInfo[clkIdx].clkDomainSrc.clkDomainSrcIdLpass = pClkDomainReqData->pClkDomainArray[index_b].clkDomainSrc.clkDomainSrcIdLpass;
                        }
                    }
                }
                pRMHandle = (RMHandleType *)coreUtils_Q_Next(pClkDomainReqQ, &pRMHandle->link);
            }
        }
        for(index_c = AdsppmClk_HwRsp_Core; index_c < AdsppmClk_EnumMax; index_c++)
        {
            ClockIndexType clkIdx = index_c-AdsppmClk_HwRsp_Core;
            switch(gClockInfo[clkIdx].type)
            {
            case AsicClk_TypeDalFreqSet:
            case AsicClk_TypeDalEnable:
            case AsicClk_TypeDalDomainSrc:
                if(gClockInfo[clkIdx].clkDomainSrc.clkDomainSrcIdLpass != gClockInfo[clkIdx].clkDomainSrc_last.clkDomainSrcIdLpass)
                {
                    if(Adsppm_Status_Success !=
                            ClkRgm_SelectClkSource(index_c, gClockInfo[clkIdx].clkDomainSrc.clkDomainSrcIdLpass)) //TODO can change source
                    {
                        sts = Adsppm_Status_Failed;
                        break;
                    }
                    else
                    {
                        gClockInfo[clkIdx].clkDomainSrc_last.clkDomainSrcIdLpass = gClockInfo[clkIdx].clkDomainSrc.clkDomainSrcIdLpass;
                    }
                }
                break;
            default:
                break;
            }
        }
        ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk_Domain);
    }

    return sts;
}


Adsppm_Status CLK_GetInfo(AdsppmInfoClkFreqType *pClockInfo)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    if(NULL == pClockInfo)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pClockInfo is NULL");
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        if(pClockInfo->forceMeasure)
        {
            sts = ClkRgm_CalcClockFrequency(pClockInfo->clkId, &pClockInfo->clkFreqHz);
        }
        else
        {
            sts = ClkRgm_GetClockFrequency(pClockInfo->clkId, &pClockInfo->clkFreqHz);
        }
    }
    return sts;
}

