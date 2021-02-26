/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic_common_func.c
@brief: Contains ASIC specific functions for the Config Manager.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/request_mgr/asic_common_func.c#1 $
*/

#include <limits.h>
#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "asic_internal.h"
#include "asic_common_func.h"


#define DSP_BWUSAGE_FACTOR  256
#define DMA_BWUSAGE_FACTOR  256
#define EXTCPU_BWUSAGE_FACTOR 256

#define INSTRUCTION_WIDTH  32
#define BUS_WIDTH  64
#define AHB_BUS_WIDTH  32

/**
 * Factor used for mapping a targeted MIPS value to a Q6 DSP clock frequency.
 * The factor is used as follows:
 *   dsp_freq = factor * target_MIPS
 * The factor takes into account the number of clusters present on the
 * current platform's Hexagon DSP.
 * On a DSP with two clusters (Hexagon v60), in theory the DSP frequency can
 * set to half the required MIPS (factor of 0.5).  We set the factor higher to
 * include a safety margin, and to account for assumed contention of shared
 * resources.
 */
#define MULTI_CLUSTER_MIPS_TO_CLOCK_FACTOR 256 // 256/256 = 1, until more
                                               // profiling data is available

/**
 * Array defining MIPS efficiency depending on number of clients
 * the factors are in PERCENTS_IN_A_WHOLE
 * elements should be sorted by number of clients
 */
const AsicFactorType adspMipsFactors[] =
{
    {1, 256},
    {2, 256},
    {8, 270}
};

/**
 * Apply scaling of 32-bit input:
 * output = input * num / denom
 * UINT_MAX is returned if result would overflow 32-bit unsigned integer
 */
inline uint32 scale32(uint32 input, uint32 num, uint32 denom)
{
    uint64 interm = (uint64)input * num / denom;
    return (interm > UINT_MAX) ? UINT_MAX : interm;
}

/**
 * Apply scaling of 64-bit input:
 *     output = input * num / denom
 * denom must be non-zero
 */
inline uint64 scale64(uint64 input, uint32 num, uint32 denom)
{
    uint64 interm = input * num;
    if(num == 0)
        return num;                 // handle num == 0 case, avoid divide by 0
    else if((interm / num) == input)
        return interm / denom;      // no overflow
    else
        return ULLONG_MAX / denom;  // multiplication in 'interm' overflowed
}

Adsppm_Status getBWFromMips(AdsppmMIPSToBWAggregateType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Success;

    if(NULL != pMipsAggregateData)
    {
        AdsppmBusBWRequestValueType *pBw = &pMipsAggregateData->mipsToBW;
        uint32 maxMips = MAX(
            pMipsAggregateData->mipsData.mipsTotal,
            ACM_GetHwThreadNumber() * pMipsAggregateData->mipsData.mipsPerThread);

        // Translate MIPS into IPS
        // then translate IPS into BW in bytes per second
        uint64 bwVal = scale64(maxMips, 1000000 * INSTRUCTION_WIDTH, 8);

        if((bwVal > 0) && IsPeriodicUseCase() &&
            (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationBw)))
        {
            // Only periodic clients are active
            // Select baseline or aggregated value if it is higher
            bwVal = ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationBw, bwVal);
        }

        pBw->bwValue.busBwAbIb.Ab = bwVal;
        pBw->bwValue.busBwAbIb.Ib = bwVal;

        // Apply minimum IB vote to keep bus clocks ON for DDR accesses
        // from lower level drivers (ADSP clock/sleep/icb/rpm/etc drivers)
        if((ACM_GetFeatureStatus(AsicFeatureId_Min_Adsp_BW_Vote) == AsicFeatureState_Enabled) &&
            (pBw->busRoute.slavePort == AdsppmBusPort_Ddr_Slave))
        {
            pBw->bwValue.busBwAbIb.Ib =
                ACM_AdjustParamValue(AsicFeatureId_Min_Adsp_BW_Vote, pBw->bwValue.busBwAbIb.Ib);
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pMipsAggregateData is NULL");
        result = Adsppm_Status_NullPointer;
    }

    return result;
}


Adsppm_Status getClockFromMips(AdsppmMIPSToClockAggregateType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Success;

    if(NULL != pMipsAggregateData)
    {
        uint32 maxMips = MAX(
            pMipsAggregateData->mipsData.mipsTotal,
            ACM_GetHwThreadNumber() * pMipsAggregateData->mipsData.mipsPerThread);
        uint32 dspClockMhz = 0;

        // Check if only periodic clients are active
        if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationMips)) && IsPeriodicUseCase())
        {
            // Only periodic clients are active
            // Select baseline or aggregated value if it is higher
            maxMips = ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationMips, maxMips);
        }

        // Apply MIPS-to-clock factor
        dspClockMhz = scale32(maxMips, MULTI_CLUSTER_MIPS_TO_CLOCK_FACTOR,
            PERCENTS_IN_A_WHOLE);
        // Adjust MIPS based on feature config
        dspClockMhz = ACM_AdjustParamValue(AsicFeatureId_Adsp_Clock_Scaling, dspClockMhz);
        // Translate to Hz
        pMipsAggregateData->qDSP6Clock = dspClockMhz * 1000000;
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pMipsAggregateData is NULL");
        result = Adsppm_Status_NullPointer;
    }

    return result;
}


Adsppm_Status getClockFromBW(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData)
{
    Adsppm_Status result = Adsppm_Status_Success;
    if((NULL != pClock) && (NULL != pAHBBwData))
    {
        uint64 maxBW = MAX(pAHBBwData->Ab, pAHBBwData->Ib);
        // Translate into bits per second, then translate into clock in Hz by
        // dividing by bus width
        *pClock = scale32(maxBW, 8, AHB_BUS_WIDTH);
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pAHBBwData is NULL");
        result = Adsppm_Status_NullPointer;
    }
    return result;
}


/**
 * @fn aggregateBusBW - aggregate bw. So far the function is common for all chips
 * @param input: AdsppmBusBWDataType *pBwValue. BW is specified in BW, Usage %
 * @param output: AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue. BW output in Ab, Ib form
 * @return Adsppm_Status
 */
Adsppm_Status aggregateBusBW(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 BwUsageFactor = 0;
    uint64 bwValue =  0;
    AdsppmBusBWDataIbAbType bwValueAbIb;

    if((NULL == pAggregateBwIbAbValue) || (NULL == pBwValue))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "pAggregateBwIbAbValue or pBwValue is NULL");
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        bwValue = pBwValue->busBwValue.bwBytePerSec;
        switch(pBwValue->busBwValue.usageType)
        {
        case Adsppm_BwUsage_DSP:
            BwUsageFactor = DSP_BWUSAGE_FACTOR;
            break;
        case Adsppm_BwUsage_DMA:
            BwUsageFactor = DMA_BWUSAGE_FACTOR;
            break;
        case Adsppm_BwUsage_EXT_CPU:
            BwUsageFactor = EXTCPU_BWUSAGE_FACTOR;
            break;
        default:
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Invalid usageType=%u", pBwValue->busBwValue.usageType);
            sts = Adsppm_Status_BadParm;
            break;
        }
        // Apply bus usage factor to bw
        bwValue = scale64(bwValue, BwUsageFactor, PERCENTS_IN_A_WHOLE);
        // Translate bw to ab and ib
        bwValueAbIb.Ab = scale64(bwValue, pBwValue->busBwValue.usagePercentage, 100);
        bwValueAbIb.Ib = bwValue;
        // Aggregate bw
        pAggregateBwIbAbValue->Ab += bwValueAbIb.Ab;
        pAggregateBwIbAbValue->Ib = MAX(pAggregateBwIbAbValue->Ib, bwValueAbIb.Ib);
    }

    return sts;
}

