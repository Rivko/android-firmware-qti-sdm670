#ifndef  _DSPPM_ARCH_CONFIG_
#define  _DSPPM_ARCH_CONFIG_
/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: qdsp_pm_configdata_arch.h
@brief: Contains all 670/arch specific information.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/670/adsp/qdsp_pm_configdata_arch_670_adsp.h#1 $
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
const uint64 regProgSpeeds_array_670[] =
{
        0,          //Adsppm_RegProg_None
        19200000*4, //Adsppm_RegProg_Norm
        61410000*4  //Adsppm_RegProg_Fast
};

AsicRegProgSpeedArrayType regProgSpeeds_670 = {ADSPPM_ARRAY(regProgSpeeds_array_670)};

/**
* Array of thread leakage values, used in mppsv3 calc.
*/
const uint32 threadLeakageData_array_670[] =
{
    0,  // 1T Leakage
    20, // 2T Leakage
    33  // 3T Leakage
};

const AsicThreadLeakageArrayType threadLeakageData_670 = {ADSPPM_ARRAY(threadLeakageData_array_670)};

/**
 * Table of DDR compensation factors to be applied at different BW ranges
 * This is used to compensate for bus latencies/cache misses
 * Latency/Snoc floor vote also updated
 */
const AsicCompensatedDdrBwTableEntryType compensatedDdrBw_array_670[] = {
         //        bwThreshold,             Q6Ab,                  Q6Ib,          Latency Vote,      snocfloorVote      honestflag
        {             5000000,          5000000,             152000000,          0,                  0,                 0,},
        {            10000000,                0,             592000000,          0,                  0,                 1,},
        {            20000000,                0,             592000000,          0,                  0,                 1,},
        {            52000000,                0,             592000000,          0,                  0,                 1,},
        {            78000000,                0,             592000000,          0,                  0,                 1,},
        {           100000000,                0,             792000000,          0,                  0,                 1,},
        {           160000000,                0,            1592000000,          0,                  0,                 1,},
        {           200000000,                0,            1592000000,          0,                  0,                 1,},
        {          ULLONG_MAX,                0,            3192000000,          0,                  0,                 1,}
};

AsicCompensatedDdrBwTableType compensatedDdrBwTable_670 = {ADSPPM_ARRAY(compensatedDdrBw_array_670)};

const AsicSnocVoteTableEntryType adspsnocBwVote_array_670[] = {
        //        bwThreshold,            bwAbVote,      bwIbVote,    latencyVote,  Fal_toleerance,     Idle_time,      honestFlag
        {             5000000,             5000000,      76000000,          12500,           12500,         10000,            0,},
        {            10000000,                   0,     399000000,           5000,            5000,         10000,            1,},
        {            20000000,                   0,     799000000,           2500,            2500,         10000,            1,},
        {            52000000,                   0,     799000000,           1650,            1650,         10000,            1,},
        {            78000000,                   0,     840000000,           1100,            1100,         10000,            1,},
        {           100000000,                   0,    1800000000,            900,             900,         10000,            1,},
        {           160000000,                   0,    2180000000,            600,             600,         10000,            1,},
        {           200000000,                   0,    4064000000,            500,             500,         10000,            1,},
        {          ULLONG_MAX,                   0,    5180000000,            300,             300,         10000,            1,}
};

AsicSnocVoteTableType adspsnocVoteTable_670 = {ADSPPM_ARRAY(adspsnocBwVote_array_670)};

// BW Compensation table for AHB-IX
const AsicCompensatedAhbBwTableEntryType compensatedAhbBw_array_670[] = {
    // bwThreshold,            bwVote
    {     17514253,          81920000 }, // target SVS2  19.2 Mhz
    {     34524612,         163840000 }, // target SVS2  38.4 Mhz
    {     68070020,         327680000 }, // target SVS   76.8 Mhz
    {     98563725,         655360000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedAhbBwTable_670 = {ADSPPM_ARRAY(compensatedAhbBw_array_670)};

// BW Compensation table for AHBE/Noc
const AsicCompensatedAhbBwTableEntryType compensatedNocBw_array_670[] = {
    // bwThreshold,            bwVote
    {     24922927,         160000000 }, // target SVS2  19.2 Mhz
    {     48831664,         320000000 }, // target SVS2  38.4 Mhz
    {     68070020,         640000000 }, // target SVS   76.8 Mhz
    {     98563725,         640000000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedNocBwTable_670 = {ADSPPM_ARRAY(compensatedNocBw_array_670)};

const AsicThreadLoadingInfoType threadLoadingData_array_670[] =
{
        { 90,   10,      0,     0 },    // oneDominantThreadLoading
        { 65,   25,     10,     0 },    // twoDominantThreadLoading
        { 50,   30,     20,     0 }     // threeDominantThreadLoading
};

AsicThreadLoadingInfoTableType threadLoadingData_670 = {ADSPPM_ARRAY(threadLoadingData_array_670)};

const uint32 audioVoiceCppFactors_array_670[] =
{
        3500,    // one thread active CPP   * 1000
        2500,    // two thread active CPP   * 1000
        1700,    // three thread active CPP * 1000
           0     // four thread active CPP  * 1000
};

AsicAudioVoiceCppTrendFactorsType audioVoiceCppFactors_670 = {
        ADSPPM_ARRAY(audioVoiceCppFactors_array_670)};


const AsicCppFactorsTableRowType audioCppFactors_array_670[] =
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


AsicCppFactorsTableType audioCppFactors_670 = {
        ADSPPM_ARRAY(audioCppFactors_array_670)};

const AsicCppFactorsTableRowType voiceCppFactors_array_670[] =
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

AsicCppFactorsTableType voiceCppFactors_670 = {
        ADSPPM_ARRAY(voiceCppFactors_array_670)};

// Table for cache partition configuration.
// Audio and voice classes are grouped together for the purposes
// of determining class concurrency.
// Client class combinations not present in the table will default
// to a shared cache (no partitioning).
const AsicCachePartitionConfigType cachePartitionConfig_array_670[] = {

    // aggregateClass,                mainPartSize

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

};

AsicCachePartitionConfigTableType cachePartitionConfigTable_670 =
    { ADSPPM_ARRAY(cachePartitionConfig_array_670) };

/**
Table for system cache configuration mapping
SC driver takes usecaseid as input to provide SCID
First column represents Client class; Second priority ID and third corrosponding usecaseId
*/

const AsicSystemCacheConfigType systemcacheConfig_array_670[] = {

    // clientClass,                     UsecaseId

    { MMPM_VOICE_CLIENT_CLASS,         SC_USE_CASE_ID_VOICE }, 

    { MMPM_AUDIO_CLIENT_CLASS,         SC_USE_CASE_ID_AUDIO },
    
    { MMPM_UNKNOWN_CLIENT_CLASS,       SC_USE_CASE_ID_AUDIO },

};

AsicAdspSystemCacheConfigTableType systemcacheConfigTable_670 =
    { ADSPPM_ARRAY(systemcacheConfig_array_670) };

const AsicBwConcurrencySettingsType bwConcurrencySettings_670 =
{

    3,          /* adspDdrConcurrencyVotersThreshold*/
    15000000,   /* adspDdrCompensatedBWThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicAdspToAhbFreqTableEntry adspToAhbeFreq_array_670v1[] = {
    // adspFreqHz,      abheFreqHz
    {    844800000,              0 }, // svs_L1 f-max
    {    940800000,      200000000 }, // nominal ADSP f-max
    {   1190400000,      200000000 }, // turbo
 }; 


AsicAdspToAhbFreqTableType adspToAhbeFreqTable_670v1 = 
    { ADSPPM_ARRAY(adspToAhbeFreq_array_670v1) };


const AsicAdspToAhbFreqTableEntry adspToAhbIxFreq_array_670v1[] = {
    // adspFreqHz,      abhIxFreqHz
    {    844800000,              0 }, // svs_L1 f-max
    {    940800000,      204800000 }, // nominal ADSP f-max
    {   1190400000,      204800000 }, // turbo
 };

AsicAdspToAhbFreqTableType adspToAhbIxFreqTable_670v1 =
    { ADSPPM_ARRAY(adspToAhbIxFreq_array_670v1) };

//Table to lookup MPPS threshold values for different client classes
//Class Name, Threshold value
const AsicAdspCacheSizeMPPSThresholdTableEntry adspcachesizeMPPSThreshold_array_670[] = {
    {   MMPM_AUDIO_CLIENT_CLASS,  100  },

};

AsicAdspCacheSizeMPPSThresholdTableType  adspCacheSizeMPPSThreshold_670 = {ADSPPM_ARRAY(adspcachesizeMPPSThreshold_array_670)};

const AsicAdspCacheSizeBWScalingTableEntry adspcachesizebwscaling_array_670[] = {
    {   FULL_SIZE,             67  },
    {   THREE_QUARTER_SIZE,   100  },
    {   HALF_SIZE,            100  },    
};

AsicAdspCacheSizeBWScalingTableType  adspcachesizebwscaling_670 = {ADSPPM_ARRAY(adspcachesizebwscaling_array_670)};

#endif

