/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: busmgr_slpi.c
@brief: QDSP_PM slpi specific bus manager functions.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/target/830/slpi/busmgr_slpi.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "busmgr.h"
#include "stdlib.h"
#include "asic_internal.h"

extern busMgrCtxType gBusMgrCtx;

void BusMgr_ApplyAdspDdrBwConcurrencyFactor(uint32 numVotes, uint64 maxBw, uint64* pAb, uint64* pIb, 
		AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues, AsicBwConcurrencySettingsType* settings )
{
	
    compensatedBWFudgeFactorValues->floorFudgeFactor = 0;
    compensatedBWFudgeFactorValues->ceilingFudgeFactor = 0;
	
	 ADSPPM_LOG_PRINTF_7(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Before Concurrency Adjustment - Num of Voters %u Max BW %llu Ab %llu Ib %llu",
        (uint32) numVotes,
        (uint32) maxBw, // two params for %llu
        (uint32) (maxBw >> 32),
        (uint32) *pAb, // two params for %llu
        (uint32) (*pAb >> 32),
        (uint32) *pIb, // two params for %llu
        (uint32) (*pIb >> 32));
	
	 // Concurrent Clients <= Threshold, fudgefactor = 1
    if(settings && (numVotes <= settings->adspDdrConcurrencyVotersThreshold))
    {
        *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[0])/ 256;
        *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[0])/ 256;
    }
    else if(settings && (*pAb < settings->adspDdrCompensatedBWThreshold) && (numVotes > settings->adspDdrConcurrencyVotersThreshold))
    {
        // Concurrent Clients > Threshold, maxbw/totalBw > 90% fudgefactor = 1.5
        if (((maxBw * 100)/ *pAb) >= 90 )
        {
            *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[1])/ 256;
            *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[1])/ 256;
        }
        // Concurrent Clients > Threshold, maxbw/totalBw < 90% fudgefactor = 2
        else
        {
            *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[2])/ 256;
            *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[2])/ 256;
        }
        //Ensure that floor/ceiling FF is zero
        compensatedBWFudgeFactorValues->floorFudgeFactor = 0;
        compensatedBWFudgeFactorValues->ceilingFudgeFactor = 0;
    }
    else if(settings)
    {
        //v2 algorithm of fudge factor has to be applied
        //In this case, only ceiling and floor FFs are updated. Real calculation is done afterwards
        compensatedBWFudgeFactorValues->floorFudgeFactor = settings->adspDdrConcurrencyFactorArray[1];
        compensatedBWFudgeFactorValues->ceilingFudgeFactor = settings->adspDdrConcurrencyFactorArray[2];

    }

    ADSPPM_LOG_PRINTF_8(ADSPPM_LOG_LEVEL_INFO,
        "After Concurrency Adjustment - Num of Voters %u Max BW %llu Ab %llu Ib %llu floorFF %u",
        (uint32) numVotes,
        (uint32) maxBw, // two params for %llu
        (uint32) (maxBw >> 32),
        (uint32) *pAb, // two params for %llu
        (uint32) (*pAb >> 32),
        (uint32) *pIb, // two params for %llu
        (uint32) (*pIb >> 32),
        (uint32) compensatedBWFudgeFactorValues->floorFudgeFactor);
}


Adsppm_Status BusMgr_GetCompensatedFudgedBwValue(
    coreUtils_Q_Type* pBwRequestQueue,
    uint64* pAb, 
    uint64* pIb,
    AsicCompensatedBWFudgeFactor *pcompensatedBWFudgeFactorValues
    )
{
    RMHandleType* pRequest = NULL;
    uint32 ceilingFF= 0, floorFF=0, abFF=0; 
    uint64 tempAb=0, tempIb = 0, finalAb=0, finalIb=0 ;
    uint32 BwUsageFactor = 0;
    uint64 bwValue =  0;
    AdsppmBusBWDataIbAbType bwValueAbIb;
    AdsppmBusBWDataType *pBwValue;
    Adsppm_Status sts = Adsppm_Status_Success;

    if((pBwRequestQueue == NULL) ||
        (pAb== NULL) ||
        (pIb== NULL) ||
        (pcompensatedBWFudgeFactorValues == NULL))
    {
        return Adsppm_Status_BadParm;
    }
    tempAb = *pAb;
    tempIb = *pIb;

    ceilingFF = (pcompensatedBWFudgeFactorValues->ceilingFudgeFactor * 10/256);//Multiplying by 10 as it is decimal no.
    floorFF = ((pcompensatedBWFudgeFactorValues->floorFudgeFactor * 10)/256);//Multiplying by 10 as it is decimal no.

    // Go through each client's bw request; aggregate the BW requests only for
    // compensated request types
    pRequest = (RMHandleType*) coreUtils_Q_Check(pBwRequestQueue);
    while(pRequest != NULL)
    {
        AdsppmBwReqType* pBwReqData = (AdsppmBwReqType*) pRequest->pRequestData;
        if((pRequest->validFlag == RM_Valid) &&
            (pBwReqData != NULL) &&
            (pBwReqData->requestClass == Adsppm_BwReqClass_Compensated))
        {
            uint32 bwIndex;
            // For each sub-request within current client's request
            for(bwIndex = 0; bwIndex < pBwReqData->numOfBw; bwIndex++)
            {
                AdsppmBusBWRequestValueType* pBwReq =
                    &pBwReqData->pBwArray[bwIndex];
                if((pBwReq->busRoute.masterPort == AdsppmBusPort_Adsp_Master) &&
                    (pBwReq->busRoute.slavePort == AdsppmBusPort_Ddr_Slave))
                {
                    pBwValue = &pBwReq->bwValue; 
                    bwValue = pBwValue->busBwValue.bwBytePerSec;
                    BwUsageFactor = ADSPPM_BW_USAGE_FACTOR; 
                    
                    // Apply bus usage factor to bw
                    bwValue = scale64(bwValue, BwUsageFactor, PERCENTS_IN_A_WHOLE);
                    // Translate bw to ab and ib
                    bwValueAbIb.Ab = scale64(bwValue, pBwValue->busBwValue.usagePercentage, 100);
                    bwValueAbIb.Ib = bwValue;
                    //As per the algorithm, if Ab/Client's Ab is greater than ceiling FF, use "ceiling" fudge factor
                    if(bwValueAbIb.Ab && (((tempAb*10)/bwValueAbIb.Ab) > ceilingFF))
                        {
                            finalAb += ((bwValueAbIb.Ab * ceilingFF)/10);
                            finalIb += ((bwValueAbIb.Ib * ceilingFF)/10);
                        }
                    //As per the algorithm, if Ab/Client's Ab is less than floor FF, use "floor" fudge factor
                    else if (bwValueAbIb.Ab && (((tempAb * 10)/bwValueAbIb.Ab) < floorFF))
                        {
                            finalAb += ((bwValueAbIb.Ab * floorFF)/10);  //Since it is decimal no.
                            finalIb += ((bwValueAbIb.Ib * floorFF)/10);
                        }
                    //As per the algorithm, if Ab/Client's Ab is between ceiling and floor FF, use Ab/Client's Ab as fudge factor
                    else if (bwValueAbIb.Ab)
                        {
                            abFF = ((tempAb * 10)/bwValueAbIb.Ab);
                            finalAb += tempAb;
                            finalIb += ((abFF * bwValueAbIb.Ib)/10);
                        }
                

                }
            }
        }
        pRequest = (RMHandleType*) coreUtils_Q_Next(pBwRequestQueue,
            &pRequest->link);
    }

    *pAb = finalAb;
    *pIb = finalIb;
    gBusMgrCtx.bwAfterCompensatedFFAb = finalAb;
    gBusMgrCtx.bwAfterCompensatedFFIb = finalIb;    
    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
        "After BW Concurrency Adjustment v2 - Ab %llu Ib %llu ",
        (uint32) *pAb, // two params for %llu
        (uint32) (*pAb >> 32),
        (uint32) *pIb, // two params for %llu
        (uint32) (*pIb >> 32));

    return sts;
}


void BusMgr_GetCompensatedBw(
					uint64* pAb, uint64* pIb, coreUtils_Q_Type* pBwRequestQueue,	
					AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues)
{
	AsicBwConcurrencySettingsType* settings;
        
	if((NULL != compensatedBWFudgeFactorValues) && 
	   (NULL != pAb) &&
	   (NULL != pIb))
	{
		// Get BW concurrency settings table
		settings = ACM_GetAdspDdrBwConcurrencySettings();
		
		BusMgr_ApplyAdspDdrBwConcurrencyFactor(gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes,
			gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw,
			pAb, pIb,
			compensatedBWFudgeFactorValues, settings);
		
		if(compensatedBWFudgeFactorValues->floorFudgeFactor && compensatedBWFudgeFactorValues->ceilingFudgeFactor)
		{
			ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
							"Ab value is more than threshold, apply v2 algo\n");
			BusMgr_GetCompensatedFudgedBwValue (pBwRequestQueue, 
											pAb,pIb,
											compensatedBWFudgeFactorValues);
		}
	}
}