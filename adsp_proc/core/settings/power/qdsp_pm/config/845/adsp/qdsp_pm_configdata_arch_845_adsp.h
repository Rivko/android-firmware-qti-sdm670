#ifndef  _DSPPM_ARCH_CONFIG_
#define  _DSPPM_ARCH_CONFIG_
/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: qdsp_pm_configdata_arch.h
@brief: Contains all 845/arch specific information.

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/config/845/adsp/qdsp_pm_configdata_arch_845_adsp.h#1 $
*/

#include "adsppm_settings.h"
#include "asic_settings.h"
#include "asic_internal_settings.h"
#include "adsppm_utils_settings.h"
#include "limits.h"
#include "systemcache/systemcache.h"

 
/**
* Array of register programming speeds in byte/sec
*/
const uint64 regProgSpeeds_array_845[] =
{
        0,          //Adsppm_RegProg_None
        19200000*4, //Adsppm_RegProg_Norm
        61410000*4  //Adsppm_RegProg_Fast
};

AsicRegProgSpeedArrayType regProgSpeeds_845 = {ADSPPM_ARRAY(regProgSpeeds_array_845)};

/**
* Array of thread leakage values, used in mppsv3 calc.
*/
const uint32 threadLeakageData_array_845[] =
{
    0,  // 1T Leakage
    20, // 2T Leakage
    33  // 3T Leakage
};

const AsicThreadLeakageArrayType threadLeakageData_845 = {ADSPPM_ARRAY(threadLeakageData_array_845)};



/**
 * Table of DDR compensation factors to be applied at different BW ranges
 * This is used to compensate for bus latencies/cache misses
 * Latency/Snoc floor vote also updated
 */
const AsicCompensatedDdrBwTableEntryType compensatedDdrBw_array_845[] = {
        //        bwThreshold,            bwAbVote,      bwIbVote,    latencyVote,   snocFloorVote,    honestFlag,    Fal_toleerance,     Idle_time,
        {             5000000,             5000000,      76000000,              0,               0,             0,             12500,         10000,},
        {            40000000,                   0,     840000000,              0,               0,             1,              2100,         10000,},
        {            75000000,                   0,     840000000,           1100,               0,             1,              1100,         10000,},
        {           100000000,                   0,    1800000000,            900,               0,             1,               900,         10000,},
        {           160000000,                   0,    2180000000,            600,               0,             1,               600,         10000,},
        {           200000000,                   0,    4064000000,            500,               0,             1,               500,         10000,},
        {          ULLONG_MAX,                   0,    5180000000,            300,               0,             1,               300,         10000,}
};

AsicCompensatedDdrBwTableType compensatedDdrBwTable_845 = {ADSPPM_ARRAY(compensatedDdrBw_array_845)};

//todo: correct values from audio arch team
const AsicSnocVoteTableEntryType adspsnocBwVote_array_845[] = {
        //        bwThreshold,      SnocAb,    SnocIb
        {             5000000,          5000000,             152000000 },
        {            40000000,                0,             592000000 },
        {            75000000,                0,             592000000 },
        {           100000000,                0,             792000000 },
        {           160000000,                0,            1592000000 },
        {           200000000,                0,            1592000000 },
        {          ULLONG_MAX,                0,            3192000000 }
};



AsicSnocVoteTableType adspsnocVoteTable_845 = {ADSPPM_ARRAY(adspsnocBwVote_array_845)};




const AsicCompensatedAhbBwTableEntryType compensatedAhbBw_array_845[] = {
    // bwThreshold,            bwVote
    {      9784305,          61440000 }, // target SVS2  19.2 Mhz
    {     19568611,         122880000 }, // target SVS2  38.4 Mhz
    {     39137222,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedAhbBwTable_845 = {ADSPPM_ARRAY(compensatedAhbBw_array_845)};

const AsicThreadLoadingInfoType threadLoadingData_array_845[] =
{
        { 90,   10,      0,     0 },    // oneDominantThreadLoading
        { 65,   25,     10,     0 },    // twoDominantThreadLoading
        { 50,   30,     20,     0 }     // threeDominantThreadLoading
};

AsicThreadLoadingInfoTableType threadLoadingData_845 = {ADSPPM_ARRAY(threadLoadingData_array_845)};

const uint32 audioVoiceCppFactors_array_845[] =
{
        3500,    // one thread active CPP   * 1000
        2500,    // two thread active CPP   * 1000
        1700,    // three thread active CPP * 1000
           0     // four thread active CPP  * 1000
};

AsicAudioVoiceCppTrendFactorsType audioVoiceCppFactors_845 = {
        ADSPPM_ARRAY(audioVoiceCppFactors_array_845)};


const AsicCppFactorsTableRowType audioCppFactors_array_845[] =
{
    {
        70,         // mppsThreshold
        {
            {
                3500,    // one   thread active CPP * 1000
                2500,    // two   thread active CPP * 1000
                1700,    // three thread active CPP * 1000
            }
        }
    },
    {
        ULONG_MAX,   // mppsThreshold
        {
            {
                3300,    // one   thread active CPP * 1000
                2250,    // two   thread active CPP * 1000
                1530,    // three thread active CPP * 1000
            }
        }
    },
};


AsicCppFactorsTableType audioCppFactors_845 = {
        ADSPPM_ARRAY(audioCppFactors_array_845)};

const AsicCppFactorsTableRowType voiceCppFactors_array_845[] =
{
    {
        ULONG_MAX,   // mppsThreshold
        {
            {
              3100,    // one   thread active CPP * 1000
              2000,    // two   thread active CPP * 1000
              1700,    // three thread active CPP * 1000
            }
        }
    },
};

AsicCppFactorsTableType voiceCppFactors_845 = {
        ADSPPM_ARRAY(voiceCppFactors_array_845)};

// Table for cache partition configuration.
// Audio and voice classes are grouped together for the purposes
// of determining class concurrency.
// Client class combinations not present in the table will default
// to a shared cache (no partitioning).
const AsicCachePartitionConfigType cachePartitionConfig_array_845[] = {

    // aggregateClass,                mainPartSize

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

};

AsicCachePartitionConfigTableType cachePartitionConfigTable_845 =
    { ADSPPM_ARRAY(cachePartitionConfig_array_845) };

/**
Table for system cache configuration mapping
SC driver takes usecaseid as input to provide SCID
First column represents Client class; Second priority ID and third corrosponding usecaseId
*/

const AsicSystemCacheConfigType systemcacheConfig_array_845[] = {

    // clientClass,                     UsecaseId

    { MMPM_VOICE_CLIENT_CLASS,         SC_USE_CASE_ID_VOICE }, 

    { MMPM_AUDIO_CLIENT_CLASS,         SC_USE_CASE_ID_AUDIO },
    
    { MMPM_UNKNOWN_CLIENT_CLASS,       SC_USE_CASE_ID_AUDIO },

};

AsicAdspSystemCacheConfigTableType systemcacheConfigTable_845 =
    { ADSPPM_ARRAY(systemcacheConfig_array_845) };

const AsicBwConcurrencySettingsType bwConcurrencySettings_845 =
{

    3,          /* adspDdrConcurrencyVotersThreshold*/
    15000000,   /* adspDdrCompensatedBWThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicAdspToAhbeFreqTableEntry adspToAhbeFreq_array_845v1[] = {
    // adspFreqHz,      abheFreqHz
    {   288000000,       25600000 }, // MinSVS f-max
    {   460800000,       51200000 }, // lowSVS f-max
    {   614400000,       102400000 }, // svs f-max
    {   787200000,       102400000 }, // svs_L1 f-max
    {   940800000,      204800000 }, // nominal ADSP f-max
    {   1190400000,      204800000 }, // turbo
 };


AsicAdspToAhbeFreqTableType adspToAhbeFreqTable_845v1 =
    { ADSPPM_ARRAY(adspToAhbeFreq_array_845v1) };


const AsicAdspToAhbIxFreqTableEntry adspToAhbIxFreq_array_845v1[] = {
    // adspFreqHz,      abhIxFreqHz
    {   288000000,       0 }, // MinSVS f-max
    {   460800000,       0 }, // lowSVS f-max
    {   614400000,       0 }, // svs f-max
    {   787200000,       0 }, // svs_L1 f-max
    {   940800000,       153600000 }, // nominal ADSP f-max
    {   1190400000,      153600000 }, // turbo
 };


AsicAdspToAhbIxFreqTableType adspToAhbIxFreqTable_845v1 =
    { ADSPPM_ARRAY(adspToAhbIxFreq_array_845v1) };

//Table to lookup MPPS threshold values for different client classes
//Class Name, Threshold value
const AsicAdspCacheSizeMPPSThresholdTableEntry adspcachesizeMPPSThreshold_array_845[] = {
    {   MMPM_AUDIO_CLIENT_CLASS,  100  },

};

AsicAdspCacheSizeMPPSThresholdTableType  adspCacheSizeMPPSThreshold_845 = {ADSPPM_ARRAY(adspcachesizeMPPSThreshold_array_845)};

const AsicAdspCacheSizeBWScalingTableEntry adspcachesizebwscaling_array_845[] = {
    {   FULL_SIZE,             67  },
    {   THREE_QUARTER_SIZE,   100  },
    {   HALF_SIZE,            100  },
};

AsicAdspCacheSizeBWScalingTableType  adspcachesizebwscaling_845 = {ADSPPM_ARRAY(adspcachesizebwscaling_array_845)};

#endif

