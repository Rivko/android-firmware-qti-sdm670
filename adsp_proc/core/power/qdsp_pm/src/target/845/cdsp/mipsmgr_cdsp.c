/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: mipsmgr_cdsp.c
@brief: QDSP_PM cdsp specific mips manager functions.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/target/845/cdsp/mipsmgr_cdsp.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "requestmgr.h"
#include "asic.h"
#include "mipsmgr.h"


uint32 mipsMgr_CalcNumDominantThreads(
    uint32 mppsTotal, uint32 maxMpps, uint32 secondMaxMpps)
{
    // Default return 1 Dominant Thread
    uint32 dominantThreads = 1;
    // Sum MPPS other than max and second max
    uint32 sumOfRestClients = mppsTotal - (maxMpps + secondMaxMpps);

    if(maxMpps == 0)
    {
        // No Active MPPS Request In System
        dominantThreads = 0;
    }
    else if(maxMpps != 0 && secondMaxMpps == 0)
    {
        // Only 1 Active MPPS Request
        dominantThreads = 1;
    }
    else if((maxMpps != 0) && (secondMaxMpps != 0))
    {
        if(((secondMaxMpps + sumOfRestClients) * 100) / maxMpps > 80)
        {
            if(((sumOfRestClients * 100) / secondMaxMpps > 80))
            {
                dominantThreads = 3;
            }
            else
            {
                dominantThreads = 2;
            }
        }
    }

    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
        "MPPS total=%u, Max MPPS=%u, SecondMax MPPS=%u, Dominant Threads=%u",
        mppsTotal, maxMpps, secondMaxMpps, dominantThreads);

    return dominantThreads;
}


void mipsMgr_CalcMips(mipsMgrCtxType *self)
{
	if(NULL != self) //Assuming lock already obtained by caller
	{
		AdsppmMIPSToClockAggregateType *pMipsToClockData =
			&self->currentAggregateData.mipsToClockData;
	
		// Calculate number of dominant threads
        self->mppsAggregateData.classData[COMPUTE_INDEX].numDominantThreads = mipsMgr_CalcNumDominantThreads(
            self->mppsAggregateData.classData[COMPUTE_INDEX].mppsTotal,
            self->mppsAggregateData.classData[COMPUTE_INDEX].maxMppsVote,
            self->mppsAggregateData.classData[COMPUTE_INDEX].secondMaxMppsVote);

        // Calculate MIPS from MPPS
        self->mppsAggregateData.mipsTotal = ACM_GetMipsFromMpps(
            self->mppsAggregateData.classData[COMPUTE_INDEX].mppsTotal,
            self->mppsAggregateData.classData[COMPUTE_INDEX].numDominantThreads);

        // Aggregate MIPS from aggregated MPPS and MIPS
        pMipsToClockData->mipsData.mipsTotal =
            self->mipsAggregateData.mipsTotal +
            self->mppsAggregateData.mipsTotal;
        pMipsToClockData->mipsData.mipsPerThread = MAX(
            self->mipsAggregateData.mipsPerThread,
            self->mppsAggregateData.classData[COMPUTE_INDEX].adspFloorClock / ACM_GetHwThreadNumber());
	}
}


