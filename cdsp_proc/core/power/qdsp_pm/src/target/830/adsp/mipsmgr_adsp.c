/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: mipsmgr_adsp.c
@brief: QDSP_PM adsp specific mips manager functions.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/target/830/adsp/mipsmgr_adsp.c#1 $
*/
#include <limits.h>
#include <stdlib.h>

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

static int uint32Compare(const void* p1, const void* p2)
{
    uint32 a = *((const uint32*) p1);
    uint32 b = *((const uint32*) p2);
    if(a > b) return -1;
    if(b > a) return 1;
    return 0;
}

// 32-bit addition with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 add_of_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a + b;
    if(((UINT_MAX - a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// 32-bit multiplication with overflow check.  The boolean flag pointed to by
// overflowFlag is set to true if overflow is detected; otherwise it is left
// unmodified.
static uint32 mult_of_check(uint32 a, uint32 b, boolean* overflowFlag)
{
    uint32 result = a * b;
    if((a != 0) && ((UINT_MAX / a) < b) && (overflowFlag != NULL))
        *overflowFlag = TRUE;
    return result;
}

// Shortcuts for add and mult operations withoverflow check;
// assuming overflow flag variable is "of"
#define add2_of(a, b)           add_of_check((a), (b), &of)
#define add3_of(a, b, c)        add2_of((a), add2_of((b), (c)))
#define add4_of(a, b, c, d)     add2_of((a), add3_of((b), (c), (d)))
#define mult_of(a, b)           mult_of_check((a), (b), &of)

static uint32 mipsMgr_mppsv3(mipsMgrCtxType *self)
{
    mipsMgrMppsClassDataType* audioMppsData =
        &self->mppsAggregateData.classData[AUDIO_INDEX];
    mipsMgrMppsClassDataType* voiceMppsData =
        &self->mppsAggregateData.classData[VOICE_INDEX];
    uint32 *mppsReqList = &self->mppsAggregateData.mppsReqList[0];
    uint32 numReq = self->mppsAggregateData.numReq;
    uint32 leakFactor[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 utilFactor[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 hwThreadUsage[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 threadLoading[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 tLoad[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 tLeak[ADSP_HW_THREAD_NUMBER_DEFAULT] = {0};
    uint32 i = 0;
    int32 j = 0;
    const uint32 nT = ADSP_HW_THREAD_NUMBER_DEFAULT;
    const AsicCppFactorsType* audioCppFactors = NULL;
    const AsicCppFactorsType* voiceCppFactors = NULL;
    AsicCppFactorsType weightedCpp;
    uint32 totalMpps = 0;
    uint64 dspFreq = 0;
    boolean of = FALSE;                      // overflow flag

    totalMpps = add2_of(audioMppsData->mppsTotal, voiceMppsData->mppsTotal);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
        "total MPPS votes: %d MPPS", totalMpps);

    if(totalMpps == 0)
        return (of ? UINT_MAX : 0);

    audioCppFactors = ACM_GetAudioCppFactors(audioMppsData->mppsTotal);
    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "audio CPP: [%d %d %d]",
        audioCppFactors->cpp[0],
        audioCppFactors->cpp[1],
        audioCppFactors->cpp[2]);

    voiceCppFactors = ACM_GetVoiceCppFactors(voiceMppsData->mppsTotal);
    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "voice CPP: [%d %d %d]",
        voiceCppFactors->cpp[0],
        voiceCppFactors->cpp[1],
        voiceCppFactors->cpp[2]);

    for(i = 0; i < nT; i++)
    {
        weightedCpp.cpp[i] = 
            add2_of(
                mult_of(audioMppsData->mppsTotal, audioCppFactors->cpp[i]),
                mult_of(voiceMppsData->mppsTotal, voiceCppFactors->cpp[i])) /
            totalMpps;
    }

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
        "weighted CPP: [%d %d %d]",
        weightedCpp.cpp[0],
        weightedCpp.cpp[1],
        weightedCpp.cpp[2]);

    qsort(mppsReqList, numReq, sizeof(uint32), uint32Compare);
    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_DEBUG, "Sorted MPPS Req List");
    for (i = 0; i < numReq; i++)
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG, "MPPS Req[%d] = %d",
                i, mppsReqList[i]);

    for (i = 0; i < numReq;) {
        for (j = 0; j < nT ; j++)
            hwThreadUsage[j] += mppsReqList[i+j];
        for (j = (nT - 1), i += nT; j >= 0; j--, i++)
            hwThreadUsage[j] += mppsReqList[i];
    }

    for (i = 0; i < nT; i++)
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG,
                "Total HW Thread[%d] Usage = %d", i, hwThreadUsage[i]);
    qsort(hwThreadUsage, nT, sizeof(uint32), uint32Compare);
    for (i = 0; i < nT; i++)
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG,
                "Sorted Total HW Thread[%d] Usage = %d",
                i, hwThreadUsage[i]);

    for (i = 0; i < nT; i++) {
        uint32 hwTU = hwThreadUsage[i+1];
        if ((i + 1) == nT)
            hwTU = 0;

        threadLoading[i] = (hwThreadUsage[i] - hwTU) * (i+1);
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG,
                "Total Thread[%d] Loading = %d", i, threadLoading[i]);
    }

    for (i = 0; i < nT; i++) {
        leakFactor[i] = ACM_GetThreadLeakage(i);
        utilFactor[i] = 100 - leakFactor[i];
    }

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
        "Leakage Factor: [%d%% %d%% %d%%]",
        leakFactor[0], leakFactor[1], leakFactor[2]);
    
    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
        "Utilization Factor: [%d%% %d%% %d%%]",
        utilFactor[0], utilFactor[1], utilFactor[2]);

    //Calculate T3 threadloading and leakage @ T3 leakage.
    tLoad[2] = mult_of(threadLoading[2], utilFactor[2]) / 100;
    tLeak[2] = mult_of(threadLoading[2], leakFactor[2]) / 100;

    //Calculate T2 threadloading and leakage @ T2 leakage.
    tLeak[1] = mult_of(threadLoading[1], leakFactor[1]) / 100;
    tLoad[1] = mult_of(threadLoading[1], utilFactor[1]) / 100;
    //Carry over the leakage from T3 to T2 @ T1 utilization factor.
    tLoad[1] += (mult_of(tLeak[2], utilFactor[1]) / 100);

    //Re-eval T3 leakage @ T2 leak factor.
    tLeak[2] = mult_of(tLeak[2], leakFactor[1]) / 100;

    //Calculate T1 threadloading and leakage @ T1 leakage.
    tLeak[0] = mult_of(threadLoading[0], leakFactor[0]) / 100;
    tLoad[0] = mult_of(threadLoading[0], utilFactor[0]) / 100;

    //Carry over the leakage from T2 to T1 @ T1 utilization factor.
    tLoad[0] += (mult_of(tLeak[1], utilFactor[0]) / 100);

    //Carry over the leakage from T3 to T1 @ T1 utilization factor.
    tLoad[0] += (mult_of(tLeak[2], utilFactor[0]) / 100);

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_DEBUG,
        "Adjusted Thread Loading: [%d %d %d]",
        tLoad[0], tLoad[1], tLoad[2]);

    for (i = 0; i < nT; i++)
        tLoad[i] = mult_of(tLoad[i], weightedCpp.cpp[i]);

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_DEBUG,
        "Weighted Thread Loading: [%d %d %d]",
        tLoad[0], tLoad[1], tLoad[2]);

    dspFreq = add3_of(tLoad[0], tLoad[1], tLoad[2]);

    self->mppsAggregateData.kFactor = dspFreq/totalMpps;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Calculated kFactor: %d", (uint32)self->mppsAggregateData.kFactor);

    // Final DSP freq = dspFreq / ((% factor) * (weighted cPP factor))
    dspFreq = dspFreq / (100 * 1000);

    if(of || (dspFreq > UINT_MAX)) {
        dspFreq = UINT_MAX;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING, "overflow detected");
    }

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Final DSP Freq: %d MHz", (uint32)dspFreq);

    return (uint32)dspFreq;
}



void mipsMgr_CalcMips(mipsMgrCtxType *self)
{
	if(NULL != self) //Assuming lock already obtained by caller
	{
		AdsppmMIPSToClockAggregateType *pMipsToClockData =
			&self->currentAggregateData.mipsToClockData;

    self->mppsAggregateData.mipsTotal = mipsMgr_mppsv3(self);
	
   ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
        "Audio adspfloorClock: %d, Voice adspfloorClock: %d ", self->mppsAggregateData.classData[AUDIO_INDEX].adspFloorClock,
                                                               self->mppsAggregateData.classData[VOICE_INDEX].adspFloorClock);

        // Aggregate MIPS from aggregated MPPS and MIPS
        pMipsToClockData->mipsData.mipsTotal =
            self->mipsAggregateData.mipsTotal +
            self->mppsAggregateData.mipsTotal;
        pMipsToClockData->mipsData.mipsPerThread = MAX(
            self->mipsAggregateData.mipsPerThread,
      MAX(self->mppsAggregateData.classData[AUDIO_INDEX].adspFloorClock,
          self->mppsAggregateData.classData[VOICE_INDEX].adspFloorClock) /
      ACM_GetHwThreadNumber());

	}
}


